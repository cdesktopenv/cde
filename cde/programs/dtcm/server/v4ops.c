/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: v4ops.c /main/5 1996/10/02 17:21:09 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <time.h>
#if defined(CSRG_BASED)
#include <sys/limits.h>
#define MAXINT INT_MAX
#else
#include <values.h>
#endif
#ifdef SunOS
#include <sys/systeminfo.h>
#endif
#include "v4ops.h"
#include "rtable4.h"
#include "cmscalendar.h"
#include "tree.h"
#include "list.h"
#include "log.h"
#include "appt4.h"		/* Internal appointment data structure */
#include "reminder.h"
#include "access.h"
#include "laccess.h"
#include "lookup.h"
#include "cmsconvert.h"
#include "cmsdata.h"
#include "repeat.h"
#include "misc.h"
#include "utility.h"

extern	int	debug;
extern	char	*pgname;

/*****************************************************************************
 * forward declaration of static functions used within the file
 *****************************************************************************/
static Exception_4 append_exception_list(Appt_4 *p_appt, int ordinal);
static void trunc_exception_list(Appt_4 *p_appt, int ordinal);
static int num_exception(Appt_4 *p_appt);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

/*
 * Insert one appointment into the single appointment tree structure
 * or the repeat appointment list structure.
 * The passed in appointment is stored in the structure.
 */
extern CSA_return_code
_DtCmsInsertAppt(_DtCmsCalendar *cal, Appt_4 *appt4)
{
	CSA_return_code	stat;
	Rb_Status	rb_stat;
	time_t		current_time;
	Attr_4		p_attr;

	if (cal == NULL || appt4 == NULL)
		return (CSA_E_INVALID_PARAMETER);

	/* assign key if this is a new appointment */
	_DtCmsGenerateKey(cal, &(appt4->appt_id.key));

	if (debug) {
		fprintf(stderr, "Insert appt4: (%ld)%s\n",
			appt4->appt_id.key, ctime(&(appt4->appt_id.tick)));
	}

	/* Add the appointment into the data structure */
	if (appt4->period.period == single_4)
		rb_stat = rb_insert (cal->tree, (caddr_t)appt4,
				(caddr_t)&(appt4->appt_id));
	else
		rb_stat = hc_insert (REPT_LIST(cal), (caddr_t)appt4,
				(caddr_t)&(appt4->appt_id), NULL, NULL);

	if (rb_stat == rb_ok) {
		/* Add the qualified reminder attrs to the reminder queue */
		current_time = time(0);
		p_attr = appt4->attr;
		while (p_attr != NULL)
		{
			time_t	tick;
			Rm_que	*p_reminder;

			tick = appt4->appt_id.tick - atol(p_attr->value);
			p_reminder = build_reminder (current_time, appt4,
							p_attr, tick, 1);
			if (p_reminder != NULL)
				_DtCmsAddReminderV4(&cal->rm_queue, p_reminder);
			p_attr = p_attr->next;
		}
	}

	return (_DtCmsRbToCsaStat(rb_stat));
}

/*
 * If p_auth is null, the initiator is the owner of the calendar.  Permission
 * to delete any appointment is always granted.  If p_auth is not null, we need
 * to check if it matches the author of the deleting appointment.  Only the
 * author can delete his/her appointment.
 */
extern CSA_return_code
_DtCmsDeleteAppt(
	_DtCmsCalendar	*cal,
	char		*user,
	uint		access,
	Id_4		*p_key,
	Appt_4		**appt_r)
{
	Appt_4		*p_appt;
	List_node	*p_lnode;
	Tree_node	*p_node;

	/*
	 * Before we delete an event from the single appointment tree, we
	 * need to check if the initiator is the author of the appointment.
	 */
	if ((user != NULL) && (p_appt = (Appt_4 *)rb_lookup(APPT_TREE(cal),
	    (caddr_t)p_key)) != NULL) {
		if (!(access & (access_delete_4|CSA_OWNER_RIGHTS)) &&
		    !_DtCmIsSameUser(user, p_appt->author))
			return (CSA_E_NO_AUTHORITY);
	}

	if ((p_node = rb_delete (APPT_TREE(cal), (caddr_t)p_key)) != NULL) {
		p_appt = (Appt_4*)p_node->data;
		_DtCmsObsoleteReminderV4(&cal->rm_queue, p_appt, 0, B_FALSE);
		free (p_node);
		if (debug)
		{
			fprintf (stderr, "Deleted (%ld)%s\n",
				APPT_KEY(p_appt),
				ctime(&(p_appt->appt_id.tick)));
		}
		if (appt_r != NULL)
			*appt_r = p_appt;
		else
			_DtCm_free_appt4(p_appt);
		return (CSA_SUCCESS);
	}

	/* Attempt to delete the event from the repeating appointment list */
	p_lnode = (List_node *)hc_lookup_node(REPT_LIST(cal),
			(caddr_t)p_key);

	if (p_lnode != NULL) {
		p_appt = (Appt_4 *)p_lnode->data;

		if (user != NULL &&
		    !(access & (access_delete_4|CSA_OWNER_RIGHTS)) &&
		    !_DtCmIsSameUser(user, p_appt->author))
			return (CSA_E_NO_AUTHORITY);

		_DtCmsObsoleteReminderV4(&cal->rm_queue, p_appt, 0, B_FALSE);
		(void) hc_delete_node (REPT_LIST(cal), p_lnode);
		free (p_lnode);
		if (debug)
		{
			fprintf (stderr, "Deleted (%ld)%s\n",
				APPT_KEY(p_appt),
				ctime(&(p_appt->appt_id.tick)));
		}
		if (appt_r != NULL)
			*appt_r = p_appt;
		else
			_DtCm_free_appt4(p_appt);
		return (CSA_SUCCESS);
	}
	return (CSA_E_USER_NOT_FOUND | CSA_E_INVALID_ENTRY_HANDLE);
}

extern CSA_return_code
_DtCmsDeleteApptAndLog(
	_DtCmsCalendar	*cal,
	char		*user,
	uint		access,
	Id_4		*key,
	Appt_4		**oldappt)
{
	CSA_return_code	stat;
	Appt_4 *appt;

	if ((stat = _DtCmsDeleteAppt(cal, user, access, key, &appt))
	    == CSA_SUCCESS) {

		/* Transact the log */
		if ((stat = v4_transact_log(cal->calendar, appt,
		    _DtCmsLogRemove)) != CSA_SUCCESS) {

			(void)_DtCmsInsertAppt(cal, appt);

		} else if (oldappt) {
			*oldappt = appt;
		} else {
			_DtCm_free_appt4(appt);
		}
	}

	return (stat);
}

extern CSA_return_code
_DtCmsDeleteApptInstancesAndLog(
	_DtCmsCalendar	*cal,
	char		*source,
	uint		access,
	Id_4		*key,
	Options_4	option,
	int		*remain,
	Appt_4		**oldappt)
{
	List_node	*p_lnode;
	CSA_return_code	stat = CSA_SUCCESS;
	Appt_4		*p_appt, *oldcopy;
	int		ordinal;
	int		f, file_size, ntimes, ninstance, status = 0;
	struct stat	info;

	p_lnode = (List_node *)hc_lookup_node (REPT_LIST(cal), (caddr_t)key);
	if (p_lnode == NULL)
		return (CSA_X_DT_E_ENTRY_NOT_FOUND);

	p_appt = (Appt_4*)p_lnode->data;
	if (!(access & (access_delete_4 | CSA_OWNER_RIGHTS)) &&
	    !_DtCmIsSameUser(source, p_appt->author))
		return (CSA_E_NO_AUTHORITY);

	if ((ordinal = _DtCms_in_repeater (key, p_appt, B_TRUE)) == 0)
		return (CSA_X_DT_E_ENTRY_NOT_FOUND);

	if (debug)
		fprintf(stderr,"Delete instance: Ordinal=%d\n",ordinal);

	/*
	 * save file size in case the first log transaction
	 * succeeds but the second log transaction fails.
	 */
	if ((stat = _DtCmsGetFileSize(cal->calendar, &file_size))
	    != CSA_SUCCESS)
		return (stat);

	/* remove from log */
	if ((stat = v4_transact_log(cal->calendar, p_appt, _DtCmsLogRemove))
	    != 0) {
		return (stat);
	}
	ninstance = _DtCms_get_ninstance_v4(p_appt);

	/* calculate the ntimes value for the part
	 * of the sequence that start from the ordinal to the end
	 */
	if (remain != NULL)
		*remain = _DtCms_get_new_ntimes_v4(p_appt->period, key->tick,
				(ninstance - ordinal + 1));

	/*
	 * make a copy of the original appointment,
	 * in case we need a rollback
	 */
	if ((oldcopy = _DtCm_copy_one_appt4(p_appt)) == NULL) {
		stat = CSA_E_INSUFFICIENT_MEMORY;
		goto delete_cleanup;
	}

	/* remove from memory */
	if (option == do_one_4) {
		if (!append_exception_list (p_appt, ordinal)) {
			stat = CSA_E_INSUFFICIENT_MEMORY;
			goto delete_cleanup;
		}
	} else {
		ninstance = ordinal - 1;
		if (ninstance == 1) {
			/* convert to one-time event */
			p_appt->period.period = single_4;
			p_appt->period.nth = 0;
			p_appt->period.enddate = 0;
			p_appt->ntimes = 0;
			if (p_appt->exception) {
				_DtCm_free_excpt4(p_appt->exception);
				p_appt->exception = NULL;
			}
			stat = _DtCmsRbToCsaStat(rb_insert(cal->tree,
				(caddr_t)p_appt, (caddr_t)&(p_appt->appt_id)));
		} else {
			p_appt->ntimes = _DtCms_get_new_ntimes_v4(
					p_appt->period, p_appt->appt_id.tick,
					ninstance);

			/* update enddate just for M-F, MWF, TTh
			 * and weekdaycombo
			 */
			switch (p_appt->period.period) {
			case monThruFri_4:
			case monWedFri_4:
			case tueThur_4:
			case daysOfWeek_4:
				p_appt->period.enddate = _DtCms_prev_tick_v4(
								key->tick,
								p_appt->period);
			}
			trunc_exception_list(p_appt, ordinal);
		}
	}

	/* The last one from the series has been deleted, no more left. */
	if (ninstance == num_exception (p_appt))
		ordinal = 0;

	if (ordinal == 0) {
		/* Obsolete the reminders which match the ordinal */
		_DtCmsObsoleteReminderV4(&cal->rm_queue, p_appt, ordinal,
			(option == do_one_4 ? B_FALSE : B_TRUE));
		_DtCm_free_appt4(p_appt);
		hc_delete_node (REPT_LIST(cal), p_lnode);
		free (p_lnode);
	} else {
		/* Write out the series with new exception list. */
		if (stat || (stat = v4_transact_log(cal->calendar, p_appt,
		    _DtCmsLogAdd)) != CSA_SUCCESS) {
			/* reverse memory update */
			p_appt->ntimes = oldcopy->ntimes;
			p_appt->period = oldcopy->period;
			_DtCm_free_excpt4(p_appt->exception);
			p_appt->exception = oldcopy->exception;
			oldcopy->exception = NULL;
			goto delete_cleanup;
		}

		if (ninstance == 1) {
			time_t	current_time;
			Attr_4	p_attr;

			_DtCmsObsoleteReminderV4(&cal->rm_queue, p_appt, 0,
				(option == do_one_4 ? B_FALSE : B_TRUE));
			hc_delete_node(REPT_LIST(cal), p_lnode);
			free(p_lnode);

			/* Add the qualified reminder attrs to
			 * the reminder queue
			 */
			current_time = time(0);
			p_attr = p_appt->attr;
			while (p_attr != NULL) {
				time_t	tick;
				Rm_que	*p_reminder;

				tick = p_appt->appt_id.tick-atol(p_attr->value);
				p_reminder = build_reminder(current_time,
						p_appt, p_attr, tick, 1);
				if (p_reminder != NULL)
					_DtCmsAddReminderV4(&cal->rm_queue,
						p_reminder);
				p_attr = p_attr->next;
			}

		} else {
			_DtCmsObsoleteReminderV4(&cal->rm_queue, p_appt,
				ordinal,
				(option == do_one_4 ? B_FALSE : B_TRUE));
		}
	}

	if (oldappt)
		*oldappt = oldcopy;
	else
		_DtCm_free_appt4(oldcopy);

	return (CSA_SUCCESS);

delete_cleanup:
	/* rollback log file */
	_DtCmsTruncateFile(cal->calendar, file_size);

	if (oldcopy)
		_DtCm_free_appt4(oldcopy);

	return (stat);
}

extern CSA_return_code
_DtCmsChangeAll(
	_DtCmsCalendar	*cal,
	char		*source,
	uint		access,
	Id_4		*p_key,
	Appt_4		*newa,
	Appt_4		**oldappt)
{
	CSA_return_code	stat = CSA_SUCCESS;
	Appt_4	*olda, *newcopy = NULL;
	int	ordinal;
	int	file_size;

	if ((stat = _DtCmsGetFileSize(cal->calendar, &file_size))
	    != CSA_SUCCESS)
		return (stat);

	/*
	 * first, remove the old appointment from internal data structure
	 * and the callog file
	 */
	if ((stat = _DtCmsDeleteApptAndLog(cal, source, access, p_key, &olda))
	    != CSA_SUCCESS)
		return (stat);

	if (is_repeater(olda) && is_repeater(newa)) {
		if (_DtCmsBeginOfDay(APPT_TICK(newa)) ==
		    _DtCmsBeginOfDay(p_key->tick) &&
		    olda->period.period == newa->period.period)
		{
			/* keep the start day of the original
			 * appointment if the date of the
			 * key matches that of the new
			 * appointment and the interval
			 * is not changed
			 */
			APPT_TICK(newa) -= _DtCmsBeginOfDay(APPT_TICK(newa)) -
					_DtCmsBeginOfDay(APPT_TICK(olda));
		} else {
			/* otherwise, calculate new parent */
			if ((ordinal = _DtCms_in_repeater(p_key, olda, B_TRUE))
			    == 0) {
				stat = CSA_X_DT_E_ENTRY_NOT_FOUND;
				goto change_all_cleanup;
			} else {
				APPT_TICK(newa) = _DtCms_first_tick_v4(
							APPT_TICK(newa),
							newa->period, ordinal);
				/* if enddate exist, adjust it */
				if (olda->period.enddate &&
				    newa->period.period == olda->period.period
				    && newa->period.nth == olda->period.nth) {
					newa->period.enddate +=
					    (_DtCmsBeginOfDay(APPT_TICK(newa)) -
					     _DtCmsBeginOfDay(APPT_TICK(olda)));
				}
			}
		}

		/* ????? */
		/* We use the same exception list for the
		 * new appt.  ?? is this reasonable for
		 * all cases ??
		 */
		newa->exception = _DtCm_copy_excpt4(olda->exception);
	}

	/* adjust start date */
	_DtCms_adjust_appt_startdate(newa);

	/* make copy of new appointment */
	if ((newcopy = _DtCm_copy_one_appt4(newa)) == NULL) {
		stat = CSA_E_INSUFFICIENT_MEMORY;
		goto change_all_cleanup;
	}

	/* reuse the key */
	newcopy->appt_id.key = olda->appt_id.key;

	/* keep the original author */
	free(newcopy->author);
	if ((newcopy->author = strdup(olda->author)) == NULL) {
		stat = CSA_E_INSUFFICIENT_MEMORY;
		goto change_all_cleanup;
	}

	if ((stat = _DtCmsInsertApptAndLog(cal, newcopy)) != CSA_SUCCESS) {
		goto change_all_cleanup;
	}
	newa->appt_id.key = newcopy->appt_id.key;

	if (oldappt)
		*oldappt = olda;
	else
		_DtCm_free_appt4(olda);

	return (stat);

change_all_cleanup:
	if (newcopy)
		_DtCm_free_appt4(newcopy);
	_DtCmsTruncateFile(cal->calendar, file_size);
	(void)_DtCmsInsertAppt(cal, olda);
	return (stat);
}

extern CSA_return_code
_DtCmsChangeSome(
	_DtCmsCalendar	*cal,
	char		*source,
	uint		access,
	Id_4		*p_key,
	Appt_4		*newa,
	Options_4	option,
	Appt_4		**oldappt)
{
	CSA_return_code	stat;
	Appt_4		*olda, *newcopy = NULL;
	List_node 	*lnode;
	Exception_4 	*newexcept;
	int		file_size;
	int		remain;

	if ((stat = _DtCmsGetFileSize(cal->calendar, &file_size))
	    != CSA_SUCCESS)
		return (stat);

	/*
	 * first, remove the old appointment from internal data structure
	 * and the callog file
	 */
	if ((stat = _DtCmsDeleteApptInstancesAndLog(cal, source, access,
	    p_key, option, &remain, &olda)) != CSA_SUCCESS)
		return (stat);

	/* adjust start date */
	_DtCms_adjust_appt_startdate(newa);

	/* make copy of new appointment */
	newcopy = _DtCm_copy_one_appt4(newa);

	if (option == do_forward_4) {
		/* if repeating info is not changed, replace
		 * the deleted part with the new series, i.e.,
		 * ntimes of new series _DtCmsIsEquals to the number
		 * of deleted instances. Also get new 
		 * exception list.	
		 */
		if (olda->period.period == newa->period.period &&
		    olda->period.nth == newa->period.nth &&
		    olda->ntimes == newa->ntimes) { 

			newcopy->ntimes = remain;
			if (olda->exception != NULL) {
				_DtCm_free_excpt4(newcopy->exception);
				if ((stat = _DtCmsTruncateElist(olda, remain,
				    &newcopy->exception)) != CSA_SUCCESS) {
					goto change_some_cleanup;
				}
			}
		}
	}

	/* keep the original author */
	free(newcopy->author);
	if ((newcopy->author = strdup(olda->author)) == NULL) {
		stat = CSA_E_INSUFFICIENT_MEMORY;
		goto change_some_cleanup;
	}

	APPT_KEY(newcopy) = 0;

	if ((stat = _DtCmsInsertApptAndLog(cal, newcopy)) != CSA_SUCCESS) {
		goto change_some_cleanup;
	} else
		newa->appt_id.key = newcopy->appt_id.key;

	if (oldappt)
		*oldappt = olda;
	else
		_DtCm_free_appt4(olda);

	return (stat);

change_some_cleanup:
	if (newcopy)
		_DtCm_free_appt4(newcopy);
	_DtCmsTruncateFile(cal->calendar, file_size);
	(void)_DtCmsInsertAppt(cal, olda);
	return (stat);
}

/*
 * the passed in appt is stored in the internal data structure
 */
extern CSA_return_code
_DtCmsInsertApptAndLog(_DtCmsCalendar *cal, Appt_4 *appt)
{
	CSA_return_code	stat;

	if ((stat = _DtCmsInsertAppt(cal, appt)) == CSA_SUCCESS) {

		/* Add the new appointment to the log */
		if ((stat = v4_transact_log(cal->calendar, appt,
		    _DtCmsLogAdd)) != CSA_SUCCESS) {

			(void)_DtCmsDeleteAppt(cal, NULL, 0, &appt->appt_id,
				&appt);
		}
	}
	return (stat);
}

extern _DtCmsComparisonResult
_DtCmsCompareAppt(Id_4 *key, caddr_t data)
{
	Appt_4 *appt = (Appt_4 *)data;

	/* Composite key */
	if (key->tick < appt->appt_id.tick)
		return (_DtCmsIsLess);
	if (key->tick > appt->appt_id.tick)
		return (_DtCmsIsGreater);

	/* tick's are _DtCmsIsEqual */
	if (key->key < appt->appt_id.key)
		return (_DtCmsIsLess);
	if (key->key > appt->appt_id.key)
		return (_DtCmsIsGreater);

	return (_DtCmsIsEqual);
}

extern _DtCmsComparisonResult
_DtCmsCompareRptAppt(Id_4 *key, caddr_t data)
{
	Appt_4 *appt = (Appt_4 *)data;

	if (key->key < appt->appt_id.key)
		return (_DtCmsIsLess);
	if (key->key > appt->appt_id.key)
		return (_DtCmsIsGreater);
	return (_DtCmsIsEqual);
}

extern caddr_t
_DtCmsGetApptKey (caddr_t data)
{
	return ((caddr_t) &(((Appt_4 *) data)->appt_id));
}

extern CSA_return_code
v4_transact_log(char *calendar, Appt_4 *a, _DtCmsLogOps op)
{
	CSA_return_code	stat;
	char	*log=NULL;

	if ((log = _DtCmsGetLogFN(calendar)) == NULL)
		 return (CSA_E_INSUFFICIENT_MEMORY);
	else {
		stat = _DtCmsAppendAppt4ByFN(log, a, op);
		free(log);
	}
	return(stat);
}

extern CSA_return_code
_DtCmsLookupRangeV4(
	_DtCmsCalendar	*cal,
	char		*user,
	uint		access,
	Range_4		*p_range,
	boolean_t	no_end_time_range,
	long		end1,
	long		end2,
	boolean_t	(*match_func)(),
	uint		num_attrs,
	cms_attribute	*attrs,
	CSA_enum	*ops,
	Appt_4		**appt_r,
	Abb_Appt_4	**abbr_r)
{
	CSA_return_code	stat = CSA_SUCCESS;
	Period_4	period;
	caddr_t		ilp = NULL;
	int		tmp_tick, endtick;
	Id_4		lo_key;
	int		n;
	List_node	*p_lnode;
	time_t		hi_tick, lo_tick;
	int		tick;
	int		ordinal;
	int		ntimes;
	Appt_4		*p_appt;
	cms_entry	*entries;
	CSA_return_code	(*add_list_func)();

	/* do lookup on new format calendar */
	if (cal->fversion > 1) {
		if ((stat = _DtCmsLookupEntries(cal, user, access,
		    p_range->key1, p_range->key2, no_end_time_range, end1, end2,
		    num_attrs, attrs, ops, &entries)) != CSA_SUCCESS) {
			return (stat);
		}

		if (appt_r)
			stat = _DtCmsCmsentriesToAppt4ForClient(entries,
				appt_r);
		else
			stat = _DtCmsCmsentriesToAbbrAppt4ForClient(entries,
				abbr_r);

		_DtCm_free_cms_entries(entries);
		return (stat);
	}

	if (appt_r)
		add_list_func = _AddToLinkedAppts;
	else
		add_list_func = _AddToLinkedAbbrAppts;

	while (p_range != NULL)
	{
		lo_key.key = MAXINT;
		lo_key.tick = p_range->key1;
		hi_tick = p_range->key2;

		if (debug)
		{
			fprintf(stderr,"Range lookup from %s",
				ctime(&lo_key.tick));
			fprintf(stderr, " to %s\n", ctime(&hi_tick));
		}

		n = 0;

		/* Get a range of appointments in single appointment tree */
		while ((p_appt = (Appt_4 *)rb_lookup_next_larger(
		    APPT_TREE(cal),(caddr_t)&lo_key))
		    && (APPT_TICK(p_appt) < hi_tick))
		{
			n++;

			endtick = p_appt->appt_id.tick + p_appt->duration;
			if ((!no_end_time_range && (p_appt->duration == 0 ||
			    endtick <= end1 || endtick >= end2)) ||
			    (match_func && !match_func(p_appt, num_attrs,
			    attrs, ops))) {
				lo_key.key = APPT_KEY(p_appt);
				lo_key.tick = APPT_TICK(p_appt);
				continue;
			}

			if ((stat = (*add_list_func)(p_appt, user,
			    access,&ilp)) != CSA_SUCCESS) {
				if (ilp != NULL) {
					if (appt_r)
						_DtCm_free_appt4((Appt_4 *)ilp);
					else
						_DtCm_free_abbrev_appt4(
							(Abb_Appt_4 *)ilp);
					ilp = NULL;
				}
				break;
			}

			lo_key.key = APPT_KEY(p_appt);
			lo_key.tick = APPT_TICK(p_appt);
		}

		/* Get a range of events from repeating appointment list */
		p_lnode = REPT_LIST(cal)->root;
		while (p_lnode != NULL)
		{
			lo_tick = p_range->key1;
			hi_tick = p_range->key2;

			p_appt = (Appt_4*)p_lnode->data;

			/* calculate the last tick */
			if (p_lnode->lasttick == 0)
				p_lnode->lasttick = _DtCms_last_tick_v4(
							APPT_TICK(p_appt),
							p_appt->period,
							p_appt->ntimes);

			if (p_lnode->lasttick <= lo_tick ||
			    APPT_TICK(p_appt) >= hi_tick ||
			    (!no_end_time_range && p_appt->duration == 0) ||
			    (!no_end_time_range &&
			     (p_lnode->lasttick+p_appt->duration) <= end1) ||
			    (!no_end_time_range &&
			     (p_appt->appt_id.tick+p_appt->duration) >= end2) ||
			    (match_func &&
			     !match_func(p_appt, num_attrs, attrs, ops)) ||
			    _GetAccessLevel(user, access, p_appt) == private_4)
			{ 
				p_lnode = hc_lookup_next (p_lnode);
				continue;
			}

			if (!no_end_time_range &&
			    lo_tick < (end1 - p_appt->duration))
				lo_tick = end1 - p_appt->duration;

			if (!no_end_time_range &&
			    ((end2 - p_appt->duration) < hi_tick))
				hi_tick = end2 - p_appt->duration;

			ntimes = _DtCms_get_ninstance_v4(p_appt);
			period = p_appt->period;
			for (tick = _DtCms_closest_tick_v4(lo_tick,
				APPT_TICK(p_appt), period, &ordinal), ordinal--;
			     tick < hi_tick;
			     tick = _DtCms_next_tick_v4(tick, period))
			{
				/* Repeater but beyond the scope */
				if (++ordinal > ntimes ||
				    tick > p_lnode->lasttick)
					break;

				if (tick <= lo_tick ||
				    _DtCms_marked_4_cancellation(p_appt,
				    ordinal))
					continue;

				n++;

				/* Replace the parent key by the
				 * current tick for the repeating event
				 */
				tmp_tick = APPT_TICK(p_appt);
				APPT_TICK(p_appt) = tick; 

				/* Add to list, restore parent key */
				stat = (*add_list_func)(p_appt, user,
					access, &ilp);

				APPT_TICK(p_appt) = tmp_tick;

				if (stat != CSA_SUCCESS) {
					if (ilp != NULL) {
						if (appt_r)
						    _DtCm_free_appt4(
							(Appt_4 *)ilp);
						else
						    _DtCm_free_abbrev_appt4(
							(Abb_Appt_4 *)ilp);
						ilp = NULL;
					}
					break;
				}
			}

			p_lnode = hc_lookup_next (p_lnode);
		}

		p_range = p_range->next;
	}

	if (debug)
		fprintf (stderr, "Found %d entries in range lookup\n", n);

	if (stat == CSA_SUCCESS) {
		if (appt_r)
			*appt_r = (Appt_4 *)ilp;
		else
			*abbr_r = (Abb_Appt_4 *)ilp;
	}

	return (stat);
}

extern CSA_return_code
_DtCmsLookupKeyrangeV4(
	_DtCmsCalendar	*cal,
	char		*user,
	uint		access,
	boolean_t	no_start_time_range,
	boolean_t	no_end_time_range,
	time_t		start1,
	time_t		start2,
	time_t		end1,
	time_t		end2,
	long		id,
	boolean_t	(*match_func)(),
	uint		num_attrs,
	cms_attribute	*attrs,
	CSA_enum	*ops,
	Appt_4		**appt_r,
	Abb_Appt_4	**abbr_r)
{
	CSA_return_code	stat = CSA_SUCCESS;
	Period_4	period;
	long		tmp_tick, endtick;
	Id_4		lo_key;
	int		n;
	List_node	*p_lnode;
	int		tick;
	int		ordinal;
	int		ntimes;
	Appt_4		*p_appt, *tappt;
	Abb_Appt_4	*tabbr;
	cms_entry	*entries;
	CSA_return_code (*add_list_func)();

	/* do lookup on new format calendar */
	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		if ((stat = _DtCmsLookupEntriesById(cal, user, access,
		    no_start_time_range, no_end_time_range, start1, start2,
		    end1, end2, id, num_attrs, attrs, ops, &entries))
		    != CSA_SUCCESS) {
			return (stat);
		}

		if (appt_r) {
			stat = _DtCmsCmsentriesToAppt4ForClient(entries,
				&tappt);
			*appt_r = _AddApptInOrder(*appt_r, tappt);
		} else {
			stat = _DtCmsCmsentriesToAbbrAppt4ForClient(entries,
				&tabbr);
			*abbr_r = _AddAbbApptInOrder(*abbr_r, tabbr);
		}

		_DtCm_free_cms_entries(entries);
		return (stat);
	}

	if (appt_r)
		add_list_func = _AddToLinkedAppts;
	else
		add_list_func = _AddToLinkedAbbrAppts;

	lo_key.key = id;
	lo_key.tick = start1;

	/* Search from repeating appointments first for optimization */
	p_lnode = hc_lookup_node (REPT_LIST(cal), (caddr_t)&lo_key);
	if (p_lnode != NULL) {
		p_appt = (Appt_4*)p_lnode->data;
		if ((match_func && !match_func(p_appt, num_attrs, attrs, ops))
		    || _GetAccessLevel(user, access, p_appt) == private_4)
			return (CSA_SUCCESS);

		/* just return the first event */
		if (no_start_time_range && no_end_time_range)
			return ((*add_list_func)(p_appt, user, access,
				(appt_r ? (caddr_t)appt_r : (caddr_t)abbr_r)));

		/* Get the range of events from this appointment. */
		ntimes = _DtCms_get_ninstance_v4(p_appt);
		period = p_appt->period;

		/* calculate the last tick */
		if (p_lnode->lasttick == 0)
			p_lnode->lasttick = _DtCms_last_tick_v4(
						APPT_TICK(p_appt),
						p_appt->period, p_appt->ntimes);

		if (p_lnode->lasttick <= start1 ||
		    p_appt->appt_id.tick >= start2 ||
		    (!no_end_time_range && p_appt->duration == 0) ||
		    (!no_end_time_range &&
		     (p_lnode->lasttick+p_appt->duration) <= end1) ||
		    (!no_end_time_range &&
		     (p_appt->appt_id.tick+p_appt->duration) >= end2))
			return (SUCCESS);

		if (!no_end_time_range && (start1 < (end1 - p_appt->duration)))
			start1 = end1 - p_appt->duration;

		if (!no_end_time_range && ((end2 - p_appt->duration) < start2))
			start2 = end2 - p_appt->duration;

		for (tick = _DtCms_closest_tick_v4(start1,
		    APPT_TICK(p_appt), period, &ordinal), ordinal--;
		    stat == CSA_SUCCESS && tick < start2;
		    tick = _DtCms_next_tick_v4(tick, period))
		{
			/* Repeater but beyond the scope */
			if (++ordinal > ntimes || tick > p_lnode->lasttick)
				break;

			if (tick <= start1)
				continue;

			/* If not cancelled, add to linked list */
			if (!_DtCms_marked_4_cancellation (p_appt, ordinal))
			{
				n++;

				/* Replace the parent key by
				 * the current tick for the
				 * repeating event
				 */
				tmp_tick = APPT_TICK(p_appt);
				APPT_TICK(p_appt) = tick; 

				/* Add to list, restore parent key */
				stat = (*add_list_func)(p_appt, user, access,
					(appt_r ? (caddr_t)appt_r :
					(caddr_t)abbr_r));

				APPT_TICK(p_appt) = tmp_tick;

			}
		}
		return (stat);
	} else {
		/* Check if it is in single appointment tree */
		while ((p_appt = (Appt_4 *)rb_lookup_next_larger(
		    APPT_TREE(cal), (caddr_t) &lo_key)) &&
		    (APPT_TICK(p_appt) < start2))
		{
			if (p_appt->appt_id.key != lo_key.key) {
				lo_key.tick = APPT_TICK(p_appt);
				lo_key.key = APPT_KEY(p_appt);
			} else {
				endtick = p_appt->appt_id.tick+p_appt->duration;

				if ((!no_end_time_range &&
				    (p_appt->duration == 0 || endtick <= end1 ||
				    endtick >= end2)) ||
				    (match_func && !match_func(p_appt,
				    num_attrs, attrs, ops))) {

					return (CSA_SUCCESS);
				} else {
					return ((*add_list_func)(p_appt, user,
						access, (appt_r ?
						(caddr_t)appt_r :
						(caddr_t)abbr_r)));
				}
			}
		}
		return (CSA_SUCCESS);
	}
}

extern CSA_return_code
_AddToLinkedAppts(
	Appt_4	*p_appt,
	char	*user,
	uint	access,
	caddr_t	*ilp)
{
	Appt_4	*p_prev;
	Appt_4	*copy;

	switch (_GetAccessLevel(user, access, p_appt)) {
	case public_4:
		copy = _DtCm_copy_one_appt4(p_appt);
		break;
	case semiprivate_4:
		copy = _DtCm_copy_semiprivate_appt4(p_appt);
		break;
	default:
		return (CSA_SUCCESS);
	}

	if (copy == NULL)
		return(CSA_E_INSUFFICIENT_MEMORY);
	else {
		*ilp = (caddr_t)_AddApptInOrder((Appt_4 *)*ilp, copy);
		return (CSA_SUCCESS);
	}
}

extern CSA_return_code
_AddToLinkedAbbrAppts(
	Appt_4	*p_appt,
	char	*user,
	uint	access,
	caddr_t	*ilp)
{
	Abb_Appt_4 *copy;

	switch (_GetAccessLevel(user, access, p_appt)) {
	case public_4:
		copy = _DtCm_appt_to_abbrev4(p_appt);
		break;
	case semiprivate_4:
		copy = _DtCm_appt_to_semiprivate_abbrev4(p_appt);
		break;
	default:
		return (CSA_SUCCESS);
	}

	if (copy == NULL)
		return(CSA_E_INSUFFICIENT_MEMORY);
	else {
		*ilp = (caddr_t)_AddAbbApptInOrder((Abb_Appt_4 *)*ilp, copy);
		return (CSA_SUCCESS);
	}
}

extern Privacy_Level_4
_GetAccessLevel(char *user, uint access, Appt_4 *p_appt)
{
	if (access & CSA_OWNER_RIGHTS ||
	    ((access & access_read_4) && p_appt->privacy == public_4) ||
	    _DtCmIsSameUser(user, p_appt->author)) {

		return (public_4);

	} else if (p_appt->privacy == private_4) {

		return (private_4);

	} else
		return (semiprivate_4);

}

extern Appt_4 *
_AddApptInOrder(Appt_4 *head, Appt_4 *aptr)
{
	Appt_4	*p_appt, *p_prev, *p_next;

	while (aptr) {
		p_next = aptr->next;
		aptr->next = NULL;

		/* Add the item to the linked list in ascending order */
		p_prev = NULL;
		p_appt = head;
		while (p_appt != NULL)
		{
			if (APPT_TICK(aptr) <= APPT_TICK(p_appt))
				break;
			p_prev = p_appt;
			p_appt = p_appt->next;
		}

		if (p_prev == NULL)
		{
			aptr->next = p_appt;
			head = aptr;
		} else {
			aptr->next = p_prev->next;
			p_prev->next = aptr;
		}

		aptr = p_next;
	}

	return (head);
}

extern Abb_Appt_4 *
_AddAbbApptInOrder(Abb_Appt_4 *head, Abb_Appt_4 *aptr)
{
	Abb_Appt_4	*p_appt, *p_prev, *p_next;

	while (aptr) {
		p_next = aptr->next;
		aptr->next = NULL;

		/* Add the item to the linked list in ascending order */
		p_prev = NULL;
		p_appt = head;
		while (p_appt != NULL)
		{
			if (APPT_TICK(aptr) <= APPT_TICK(p_appt))
				break;
			p_prev = p_appt;
			p_appt = p_appt->next;
		}

		if (p_prev == NULL)
		{
			aptr->next = p_appt;
			head = aptr;
		} else {
			aptr->next = p_prev->next;
			p_prev->next = aptr;
		}

		aptr = p_next;
	}

	return (head);
}

extern CSA_return_code
_DtCmsSetV4AccessListAndLog(_DtCmsCalendar *cal, Access_Entry_4 *alist)
{
	CSA_return_code	stat;
	char	*name, *log;
	int	file_size;

	/* update access list for old format calendar */
	if ((name = _DtCmsTarget2Name(cal->calendar)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if ((log = _DtCmsGetLogFN(name)) == NULL) {
		free(name);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	/*
	 * save file size in case the first log transaction
	 * succeeds but the second log transaction fails.
	 */
	if ((stat = _DtCmsGetFileSize(cal->calendar, &file_size))
	    != CSA_SUCCESS) {
		free (name);
		free (log);
		return (stat);
	}

	/* Set to the data structure */
	if ((stat = _DtCmsSetV4AccessListInCal(cal, alist)) == CSA_SUCCESS) {

		if (((stat = _DtCmsAppendAccessByFN(log, access_read_4,
		    GET_R_ACCESS(cal))) != CSA_SUCCESS) ||
		    ((stat = _DtCmsAppendAccessByFN(log, access_write_4,
		    GET_W_ACCESS(cal))) != CSA_SUCCESS) ||
		    ((stat = _DtCmsAppendAccessByFN(log, access_delete_4,
		    GET_D_ACCESS(cal))) != CSA_SUCCESS) ||
		    ((stat = _DtCmsAppendAccessByFN(log, access_exec_4,
		    GET_X_ACCESS(cal))) != CSA_SUCCESS)) {

			/* rollback log file */
			_DtCmsTruncateFile(cal->calendar, file_size);
		} else
			cal->modified = B_TRUE;
	}

	free (name);
	free(log);
	return (stat);
}

extern CSA_return_code
_DtCmsGetV4Reminders(
	_DtCmsCalendar	*cal,
	long		tick,
	Reminder_4	**rem_r,
	_DtCmsEntryId	**ids_r)
{
	Rm_que		*p_node, *p_prev, *p_next, *p_new;
	Reminder_4	*rptr, *v4rem = NULL;
	_DtCmsEntryId	*idptr, *ids = NULL;

	if (rem_r == NULL) return (CSA_E_INVALID_PARAMETER);

	/* do lookup in old format calendar */
	p_prev = NULL;
	p_node = cal->rm_queue;
	while (p_node != NULL)
	{
		/* It is still a future reminder. */
		if (tick < p_node->remind_at)
			break;

		/* The reminder expired.  It either needs to be recalculated
		 * (repeating appointment) or dropped (non-repeating appt.)
		 */
		p_next = _DtCmsRemoveReminderV4(&cal->rm_queue, p_prev, p_node);

		if (is_repeater(p_node->appt)) {
			/* Calculate next reminder for repeating appointment */
			p_new = build_reminder(tick+1, p_node->appt,
					p_node->attr, p_node->remind_at,
					p_node->remind_ord);
			if (p_new != NULL)
			{
				_DtCmsAddReminderV4(&cal->rm_queue, p_new);
				if (p_new->next == p_next)
					p_prev = p_new;
			}
		}

		free (p_node);
		p_node = p_next;
	}

	/* Pick the first one from the active reminder queue because it is
	 * always >= the given key.
	 */
	p_node = cal->rm_queue;
	if (p_node != NULL)
	{
		tick = p_node->remind_at;
		do
		{
			rptr = _DtCmsGetReminderInfoV4(p_node);
			rptr->next = v4rem;
			v4rem = rptr;
			if (ids_r) {
				idptr = (_DtCmsEntryId *)calloc(1,
					sizeof(_DtCmsEntryId));
				idptr->id = p_node->appt->appt_id.key;
				idptr->next = ids;
				ids = idptr;
			}
			p_node = p_node->next;
		} while ((p_node != NULL) && (p_node->remind_at == tick));
	}

	*rem_r = v4rem;
	if (ids_r) *ids_r = ids;
	return (CSA_SUCCESS);
}

extern void
_DtCmsFreeEntryIds(_DtCmsEntryId *ids)
{
	_DtCmsEntryId *ptr;

	while (ids != NULL) {
		ptr = ids->next;
		free(ids);
		ids = ptr;
	}
}

extern CSA_return_code
_DtCmsTruncateElist(Appt_4 *parent_p, int remain, Except_4 **excpt)
{
        int except_no, ntimes_diff;
        Except_4 *e, *p, *last_e = NULL, *head = NULL;

	ntimes_diff = _DtCms_get_ninstance_v4(parent_p) - remain;
	p = parent_p->exception;
	while(p != NULL) {
		if ((except_no = (p->ordinal - ntimes_diff)) > 0 && 
			except_no <= remain) {
			if ((e = (Except_4*)calloc(1, sizeof(Except_4)))
			    == NULL) {
				_DtCm_free_excpt4(head);
				return (CSA_E_INSUFFICIENT_MEMORY);
			}
			e->ordinal = except_no;
			e->next = NULL;
			if (last_e != NULL) 
				last_e->next = e;
			else
				head = e;
			last_e = e;
		}
                p = p->next;
        }

	*excpt = head;
	return (CSA_SUCCESS);
}

extern CSA_return_code 
_DtCmsSetV4AccessListInCal(_DtCmsCalendar *cal, Access_Entry_4 *e)
{
	Access_Entry_4	*q;

	/* Wipe out the old access lists. */
	_DtCm_free_access_list4(GET_R_ACCESS (cal));
	_DtCm_free_access_list4(GET_W_ACCESS (cal));
	_DtCm_free_access_list4(GET_D_ACCESS (cal));
	_DtCm_free_access_list4(GET_X_ACCESS (cal));
	SET_R_ACCESS(cal, NULL);
	SET_W_ACCESS(cal, NULL);
	SET_D_ACCESS(cal, NULL);
	SET_X_ACCESS(cal, NULL);
	_DtCm_free_access_list4(cal->alist);

	/* Split the access list to 3 differnt operation lists */
	while (e != NULL)
	{
		if (e->access_type & access_read_4) {
			q = _DtCm_make_access_entry4(e->who, e->access_type);
			q->next = GET_R_ACCESS(cal);
			SET_R_ACCESS(cal, q);
		}
		if (e->access_type & access_write_4) {
			q = _DtCm_make_access_entry4(e->who, e->access_type);
			q->next = GET_W_ACCESS(cal);
			SET_W_ACCESS(cal, q);
		}
		if (e->access_type & access_delete_4) {
			q = _DtCm_make_access_entry4(e->who, e->access_type);
			q->next = GET_D_ACCESS(cal);
			SET_D_ACCESS(cal, q);
		}
		if (e->access_type & access_exec_4) {
			q = _DtCm_make_access_entry4(e->who, e->access_type);
			q->next = GET_X_ACCESS(cal);
			SET_X_ACCESS(cal, q);
		}
		e = e->next;
	}

	cal->alist = _DtCmsCalendarAccessList(cal);

	return (CSA_SUCCESS);
}

/******************************************************************************
 * static functions used within the file
 ******************************************************************************/

static Exception_4
append_exception_list(Appt_4 *p_appt, int ordinal)
{
	Exception_4 p_excpt;
	Exception_4 p_prev;
	Exception_4 p_ex;

	if ((p_excpt = (Exception_4)calloc(1, sizeof(*p_excpt))) == NULL)
		return (NULL);
	p_excpt->ordinal = ordinal;
	p_prev = NULL;
	p_ex = p_appt->exception;
	while (p_ex != NULL)
	{
		/* Exception list is in descending order for faster access */
		if (ordinal > p_ex->ordinal)
			break;
		p_prev = p_ex;
		p_ex = p_ex->next;
	}
	if (p_prev == NULL)
	{
		p_excpt->next = p_appt->exception;
		p_appt->exception = p_excpt;
	}
	else
	{
		p_excpt->next = p_prev->next;
		p_prev->next = p_excpt;
	}

	return (p_excpt);
}

/*
 * remove exceptions that are larger than ordinal
 */
static void
trunc_exception_list(Appt_4 *p_appt, int ordinal)
{
	Exception_4 p_next;
	Exception_4 p_ex;

	p_ex = p_appt->exception;

	/* Exception list is in descending order for faster access */
	while ((p_ex != NULL) && (p_ex->ordinal > ordinal))
	{
		p_next = p_ex->next;
		free(p_ex);
		p_ex = p_next;
	}
	p_appt->exception = p_ex;
}

static int
num_exception(Appt_4 *p_appt)
{
	int ntimes;
	Except_4 *p;

	ntimes = 0;
	p = p_appt->exception;
	while (p != NULL)
	{
		ntimes++;
		p = p->next;
	}
	return (ntimes);
}

