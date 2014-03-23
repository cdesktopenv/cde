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
/* $XConsortium: rtable4.c /main/5 1996/10/02 17:31:51 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

/*
 * version 4 of calendar manager rpc protocol functions.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "rtable4.h"
#include <sys/param.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <rpc/rpc.h>
#if defined(CSRG_BASED)
#define MAXINT INT_MAX
#else
#include <values.h>
#endif
#include <string.h>
#include <pwd.h>
#ifdef SUNOS
#include <netdir.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#endif
#include "cm.h"
#include "access.h"
#include "laccess.h"
#include "callback.h"
#include "appt4.h"		
#include "log.h"
#include "tree.h"
#include "list.h"
#include "cmscalendar.h"
#include "v4ops.h"
#include "v5ops.h"
#include "reminder.h"
#include "repeat.h"
#include "utility.h"
#include "lutil.h"
#include "rpcextras.h"
#include "rtable4_tbl.i"
#include "lookup.h"
#include "cmsdata.h"
#include "attr.h"
#include "convert5-4.h"
#include "convert4-5.h"
#include "cmsconvert.h"
#include "misc.h"
#include "insert.h"
#include "delete.h"
#include "update.h"


extern	int	debug;
extern	char	*pgname;

/*****************************************************************************
 * forward declaration of static functions used within the file
 *****************************************************************************/

static Appt_4 * rtable_lookup_internal(_DtCmsCalendar *cal, char **p_src,
			Id_4 *key);

static Access_Status_4 csastat2accessstat(CSA_return_code stat);

static Registration_Status_4 csastat2regstat(CSA_return_code stat);

static Table_Status_4 csastat2tablestat(CSA_return_code stat);

static Table_Res_4 * table_lookup_next(Table_Args_4 *args,
			struct svc_req *svcrq, caddr_t (* rb_func)(),
			Appt_4 *(* rp_func)());

static Appt_4 * repeater_next_smaller(List_node *p_lnode, Id_4 *key);

static Appt_4 * repeater_next_larger(List_node *p_lnode, Id_4 *key);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

/*
 * supports both data format
 */
extern Table_Res_4 *
_DtCm_rtable_lookup_4_svc (Table_Args_4 *args, struct svc_req *svcrq)
{
	static Table_Res_4 res;
	CSA_return_code		stat;
	Appt_4		*p_appt;
	Appt_4		*h = NULL;
	_DtCmsCalendar	*cal;
	Uid_4		*p_keys;
	Id_4		*key;
	char		*user;
	uint		access;
	cms_entry	*entries;
	cms_key		cmskey;
	time_t		tmptick = 0;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_lookup_4_svc called\n");

	if (res.res.Table_Res_List_4_u.a)
		_DtCm_free_appt4(res.res.Table_Res_List_4_u.a);

	res.status = access_other_4;
	res.res.tag = AP_4;
	res.res.Table_Res_List_4_u.a = NULL;
	if ((p_keys = args->args.Args_4_u.key) == NULL)
		return (&res);

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		if (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
			if (_DTCMS_HAS_V4_BROWSE_ACCESS(access))
				res.status = access_ok_4;
			else
				res.status = access_failed_4;
		} else if (!_DTCMS_HAS_VIEW_ACCESS(access)) {
			res.status = access_failed_4;
			return (&res);
		} else
			res.status = access_ok_4;
	} else {
		res.status = csastat2accessstat(stat);
		return (&res);
	}

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		while (p_keys != NULL) {
			cmskey.time = p_keys->appt_id.tick;
			cmskey.id = p_keys->appt_id.key;

			if ((stat = _DtCmsGetEntryAttrByKey(cal, user, access,
			    cmskey, 0, NULL, &entries, NULL))
			    == CSA_SUCCESS) {

				if ((stat = _DtCmsCmsentriesToAppt4ForClient(
				    entries, &p_appt)) == CSA_SUCCESS) {
					/* link to appt list */
					h = _AddApptInOrder(h, p_appt);
				}
				_DtCm_free_cms_entries(entries);
			}

			if (stat != CSA_SUCCESS) {
				res.status = csastat2accessstat(stat);
				if (h) {
					_DtCm_free_appt4(h);
					h = NULL;
				}
				break;
			}
			p_keys = p_keys->next;
		}

		res.res.Table_Res_List_4_u.a = h;
		return (&res);
	}

	/* do lookup on old format calendar */
	while (p_keys != NULL)
	{
		key = &p_keys->appt_id;

		if (debug) {
			fprintf(stderr,
			    "_DtCm_rtable_lookup_4_svc at (key %ld)%s\n",
			    key->key, ctime(&key->tick));
		}

		p_appt = (Appt_4 *)rb_lookup(APPT_TREE(cal), (caddr_t)key);
		if (p_appt == NULL) {
			if ((p_appt = (Appt_4 *)hc_lookup(REPT_LIST(cal),
			    (caddr_t)key)) != NULL) {
				if (!_DtCms_in_repeater(key, p_appt, B_FALSE))
					p_appt = NULL;
			}
		}

		if (p_appt != NULL) {
			if (p_appt->appt_id.tick != key->tick) {
				tmptick = p_appt->appt_id.tick;
				p_appt->appt_id.tick = key->tick;
			}

			stat = _AddToLinkedAppts(p_appt, user, access,
					(caddr_t *)&h);

			if (tmptick) p_appt->appt_id.tick = tmptick;

			if (stat != CSA_SUCCESS) {
				if (h) {
					_DtCm_free_appt4(h);
					h = NULL;
				}
				res.status = csastat2accessstat(stat);
			}
			break;
		}

		p_keys = p_keys->next;
	}

	res.res.Table_Res_List_4_u.a = h;
	return (&res);
}

/*
 * supports old data format only
 */
extern Table_Res_4 *
_DtCm_rtable_lookup_next_larger_4_svc(Table_Args_4 *args, struct svc_req *svcrq)
{
	Table_Res_4 *res;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_lookup_next_larger_4_svc called\n");

	res = table_lookup_next(args, svcrq, rb_lookup_next_larger,
				repeater_next_larger);
	return (res);
}

/*
 * supports old data format only
 */
extern Table_Res_4 *
_DtCm_rtable_lookup_next_smaller_4_svc(
	Table_Args_4	*args,
	struct svc_req	*svcrq)
{
	Table_Res_4 *res;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_lookup_next_smaller_4_svc called\n");

	res = table_lookup_next(args, svcrq, rb_lookup_next_smaller,
				repeater_next_smaller);
	return (res);
}

/*
 * supports both data format
 */
extern Table_Res_4 *
_DtCm_rtable_lookup_range_4_svc(Table_Args_4 *args, struct svc_req *svcrq)
{
	static Table_Res_4 res;
	CSA_return_code	stat;
	_DtCmsCalendar	*cal;
	char		*user;
	uint		access;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_lookup_range_4_svc called\n");

	if (res.res.Table_Res_List_4_u.a)
		_DtCm_free_appt4(res.res.Table_Res_List_4_u.a);

	res.res.tag = AP_4;
	res.res.Table_Res_List_4_u.a = NULL;

	res.status = access_other_4;
	if (args->args.Args_4_u.range == NULL)
		return (&res);

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		if (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
			if (_DTCMS_HAS_V4_BROWSE_ACCESS(access))
				res.status = access_ok_4;
			else
				res.status = access_failed_4;
		}
	} else {
		res.status = csastat2accessstat(stat);
		return (&res);
	}

	stat = _DtCmsLookupRangeV4(cal, user, access, args->args.Args_4_u.range,
		B_TRUE, 0, 0, NULL, 0, NULL, NULL,
		&res.res.Table_Res_List_4_u.a, NULL);

	res.status = csastat2accessstat(stat);
	return (&res);
}

/*
 * supports both data format
 */
extern Table_Res_4 *
_DtCm_rtable_abbreviated_lookup_range_4_svc(
	Table_Args_4	*args,
	struct svc_req	*svcrq)
{
	static Table_Res_4 res;
	CSA_return_code	stat;
	_DtCmsCalendar	*cal;
	char		*user;
	uint		access;

	if (debug)
		fprintf(stderr,
			"_DtCm_rtable_abbreviated_lookup_range_4_svc called\n");

	if (res.res.Table_Res_List_4_u.b)
		_DtCm_free_abbrev_appt4(res.res.Table_Res_List_4_u.b);

	res.res.tag = AB_4;
	res.res.Table_Res_List_4_u.b = NULL;

	res.status = access_other_4;
	if (args->args.Args_4_u.range == NULL)
		return (&res);

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		if (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
			if (_DTCMS_HAS_V4_BROWSE_ACCESS(access))
				res.status = access_ok_4;
			else
				res.status = access_failed_4;
		}
	} else {
		res.status = csastat2accessstat(stat);
		return (&res);
	}

	stat = _DtCmsLookupRangeV4(cal, user, access, args->args.Args_4_u.range,
		B_TRUE, 0, 0, NULL, 0, NULL, NULL, NULL,
		&res.res.Table_Res_List_4_u.b);

	res.status = csastat2accessstat(stat);
	return (&res);
}

extern Table_Res_4 *
_DtCm_rtable_insert_4_svc(Table_Args_4 *args, struct svc_req *svcrq)
{
	static Table_Res_4	res;
	_DtCmsCalendar		*cal;
	CSA_return_code		stat;
	char			*author;
	char			*user;
	uint			access;
	Appt_4			*ap, *appt, *prev=NULL, *a;
	cms_entry		*entry;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_insert_4_svc called\n");

	/* clean out left over */
	if (res.res.Table_Res_List_4_u.a != NULL)
		_DtCm_free_appt4(res.res.Table_Res_List_4_u.a);

	res.status = access_other_4;
	res.res.tag = AP_4;
	res.res.Table_Res_List_4_u.a = NULL;

	/* check arguments */
	if (args->target == NULL)
		return (&res);
	if ((ap = args->args.Args_4_u.appt) == NULL)
		return (&res);

	/* do some sanity checks before inserting : check appt data */
	for (appt = args->args.Args_4_u.appt; appt != NULL; appt = appt->next)
	{
		/* ntimes should be 0 or positive */
		if (appt->ntimes < 0 ||
		    (appt->period.period > single_4 && appt->ntimes == 0))
			return(&res);

		/* period beyond daysOfWeek is not supported */
		if (appt->period.period > daysOfWeek_4) {
			res.status = access_notsupported_4;
			return(&res);
		}

		/* if weekmask of daysOfWeek appt is set incorrectly, return */
		if (appt->period.period == daysOfWeek_4 &&
			(appt->period.nth == 0 || appt->period.nth > 127))
			return(&res);
	}

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		if (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
			if (!_DTCMS_HAS_V4_WRITE_ACCESS(access)) {
				res.status = access_failed_4;
				return (&res);
			}
		} else if (!_DTCMS_HAS_INSERT_ACCESS(access)) {
			res.status = access_failed_4;
			return (&res);
		} else
			res.status = access_ok_4;
	} else {
		res.status = csastat2accessstat(stat);
		return (&res);
	}

	/* make copy of the appointments */
	/* this copy is used in the result and will be freed
	 * when this routine is called again
	 */
	if ((appt = _DtCm_copy_appt4(args->args.Args_4_u.appt)) == NULL) {
		/* memory problem */
		return (&res);
	}

	ap = appt;
	while (appt != NULL) {

		/*
		 * we used to calculate the correct start day,
		 * but we should return an error instead
		 */
		_DtCms_adjust_appt_startdate(appt);

		if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
			/* convert to attributes */
			if (_DtCmsAppt4ToCmsentry(args->target, appt, &entry,
			    B_TRUE))
				goto insert_error;

			/* null out readonly attributes */
			_DtCm_free_cms_attribute_value(entry->attrs\
				[CSA_ENTRY_ATTR_ORGANIZER_I].value);
			entry->attrs[CSA_ENTRY_ATTR_ORGANIZER_I].value = NULL;
			_DtCm_free_cms_attribute_value(entry->attrs\
				[CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I].value);
			entry->attrs[CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I].\
				value = NULL;

			if (_DtCmsCheckInitialAttributes(entry)) {
				_DtCm_free_cms_entry(entry);
				goto insert_error;
			}

			/* set organizer */
			if (_DtCm_set_string_attrval(user, &entry->attrs\
			    [CSA_ENTRY_ATTR_ORGANIZER_I].value,
	    		    CSA_VALUE_CALENDAR_USER) != CSA_SUCCESS) {
				_DtCm_free_cms_entry(entry);
				goto insert_error;
			}

			/* insert entry and log it */
			if (_DtCmsInsertEntryAndLog(cal, entry)) {
				_DtCm_free_cms_entry(entry);
				goto insert_error;
			}

			appt->appt_id.key = entry->key.id;
			_DtCm_free_cms_entry(entry);

		} else {
			if ((a = _DtCm_copy_one_appt4(appt)) == NULL)
				goto insert_error;

			/* We don't trust the author field; we set our own. */
			free(a->author);
			if ((a->author = strdup(user)) == NULL) {
				_DtCm_free_appt4(a);
				goto insert_error;
			}

			/* Note, the key in appt will be set if its value is 0. */
			if ((stat = _DtCmsInsertApptAndLog(cal, a)) != CSA_SUCCESS) {
				res.status = csastat2accessstat(stat);
				goto insert_error;
			}

			/* get the new key */
			appt->appt_id.key = a->appt_id.key;
		}

		prev = appt;
		appt = appt->next;
	}

	cal->modified = B_TRUE;

	/* do callbacks */
	cal->rlist = _DtCmsDoV1Callback(cal->rlist, user, args->pid, ap);

	for (appt = ap; appt != NULL; appt = appt->next) {
		cal->rlist = _DtCmsDoInsertEntryCallback(cal->rlist,
				cal->calendar, user, appt->appt_id.key,
				args->pid);
	}

	res.status = access_ok_4;
	res.res.Table_Res_List_4_u.a = ap;

	return (&res);

insert_error:
	if (prev != NULL) {
		cal->modified = B_TRUE;
		/* some appts were inserted successfully */
		res.status = access_partial_4;
		prev->next = NULL;
		res.res.Table_Res_List_4_u.a = ap;

		/* do callback */
		cal->rlist = _DtCmsDoV1Callback(cal->rlist, user, args->pid,
			ap);
		for (appt = ap; appt != NULL; appt = appt->next) {
			cal->rlist = _DtCmsDoInsertEntryCallback(cal->rlist,
					cal->calendar, user,
					appt->appt_id.key, args->pid);
		}

	} else {
		/* first appt in bunch that failed */
		res.status = csastat2accessstat(stat);
	}
	_DtCm_free_appt4(appt);

	return (&res);
}

extern Table_Res_4 *
_DtCm_rtable_delete_4_svc(Table_Args_4 *args, struct svc_req *svcrq)
{
	static Table_Res_4	res;
	_DtCmsCalendar		*cal;
	CSA_return_code		stat;
	char			*user;
	uint			access;
	Uidopt_4		*p_keys;
	Appt_4			*h = NULL;
	Appt_4			*a;
	int			d, n, nf;
	cms_key			key;
	cms_entry		*entry;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_delete_4_svc called\n");

	/* clean out left over */
	if (res.res.Table_Res_List_4_u.a)
		_DtCm_free_appt4(res.res.Table_Res_List_4_u.a);

	res.status = access_other_4;
	res.res.tag = AP_4;
	res.res.Table_Res_List_4_u.a = NULL;

	/* check arguments */
	if (args->target == NULL)
		return (&res);
	if ((p_keys = args->args.Args_4_u.uidopt) == NULL)
		return (&res);

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
		    !_DTCMS_HAS_CHANGE_ACCESS(access)) {
			res.status = access_failed_4;
			return (&res);
		}
	} else {
		res.status = csastat2accessstat(stat);
		return (&res);
	}

	nf = d = n = 0;
	while (p_keys != NULL) {
		n++;
		if (debug) {
			fprintf (stderr, "Delete: (key %ld)%s\n",
			      p_keys->appt_id.key,
			      ctime(&p_keys->appt_id.tick));
		}

		if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
			key.time = p_keys->appt_id.tick;
			key.id = p_keys->appt_id.key;

			if (p_keys->option == do_all_4)
				stat = _DtCmsDeleteEntryAndLog(cal, user,
					access, &key, &entry);
			else
				stat = _DtCmsDeleteInstancesAndLog(cal, user,
					access, &key,
					(p_keys->option == CSA_SCOPE_ONE ?
					CSA_SCOPE_ONE : CSA_SCOPE_FORWARD),
					NULL, &entry); 

			if (stat == CSA_SUCCESS)
				stat = _DtCm_cms_entry_to_appt4(entry, &a);

			_DtCm_free_cms_entry(entry);

		} else {

			/* single or all in a repeating series */
			if (p_keys->option == do_all_4)
				stat = _DtCmsDeleteApptAndLog(cal, user,
					access, &p_keys->appt_id, &a);
			else {
				stat = _DtCmsDeleteApptInstancesAndLog(cal,
					user, access, &p_keys->appt_id,
					p_keys->option, NULL, &a);
			}
		}

		if (stat == CSA_SUCCESS) {
			if (p_keys->option != do_all_4)
				APPT_TICK(a) = p_keys->appt_id.tick;

			d++;
			a->next = h;
			h = a;
		} else if (stat == CSA_X_DT_E_ENTRY_NOT_FOUND)
			nf++;

		p_keys = p_keys->next;
	}

	if (h != NULL) {
		cal->modified = B_TRUE;

		/* do callback */
		cal->rlist = _DtCmsDoV1Callback(cal->rlist, user, args->pid, h);
		for (a = h, p_keys = args->args.Args_4_u.uidopt;
		    a != NULL; a = a->next) {
			while (a->appt_id.key != p_keys->appt_id.key)
				p_keys = p_keys->next;

			cal->rlist = _DtCmsDoDeleteEntryCallback(cal->rlist,
					cal->calendar, user, a->appt_id.key,
					p_keys->option,
					(p_keys->option == do_all_4 ?
					a->appt_id.tick : p_keys->appt_id.tick),
					args->pid);
		}
	}

	if (d == 0) {
		if (stat != 0)
			res.status = csastat2accessstat(stat);
		else
			res.status = (nf < n) ? access_failed_4 : access_ok_4;
	} else if (d < n)
		res.status = access_partial_4;
	else
		res.status = access_ok_4;

	res.res.Table_Res_List_4_u.a = h;

	return (&res);
}

extern Table_Res_4 *
_DtCm_rtable_delete_instance_4_svc(Table_Args_4 *args, struct svc_req *svcrq)
{
	static Table_Res_4 res;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_delete_instance_4_svc called\n");

	res.status = access_notsupported_4;
	res.res.tag = AP_4;
	res.res.Table_Res_List_4_u.a = NULL;

	return(&res);
}

extern Table_Res_4 *
_DtCm_rtable_change_4_svc(Table_Args_4 *args, struct svc_req *svcrq)
{
	static Table_Res_4	res;
	_DtCmsCalendar		*cal;
	CSA_return_code		stat;
	char			*user;
	uint			access;
	Id_4			*p_key;
	Appt_4			*newa, *olda;
	Appt_4			tmpappt;
	Options_4		option;
	cms_entry		*entry, *oldentry, *newentry = NULL;
	cms_key			key;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_change_4_svc called\n");

	/* clean out left over */
	res.status = access_other_4;
	res.res.tag = AP_4;
	res.res.Table_Res_List_4_u.a = NULL;

	/* check arguments */
	if (args->target == NULL)
		return (&res);
	if ((p_key = args->args.Args_4_u.apptid.oid) == NULL)
		return (&res);
	if ((newa = args->args.Args_4_u.apptid.new_appt) == NULL)
		return (&res);

	/* ntimes should be 0 or positive */
	if (newa->ntimes < 0 ||
	    (newa->period.period > single_4 && newa->ntimes == 0))
		return(&res);

	/* period beyond daysOfWeek is not supported */
	if (newa->period.period > daysOfWeek_4) {
		res.status = access_notsupported_4;
		return(&res);
	}

	/* if weekmask of daysOfWeek appt is not set correctly, return */
	if (newa->period.period == daysOfWeek_4 &&
	    (newa->period.nth == 0 || newa->period.nth > 127))
		return(&res);

	option = args->args.Args_4_u.apptid.option;
	if (option < do_all_4 || option > do_forward_4)
		return (&res); 

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		if ((cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
		    !_DTCMS_HAS_CHANGE_ACCESS(access)) ||
		    (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
		    !_DTCMS_HAS_V4_WRITE_ACCESS(access))) {

			res.status = access_failed_4;
			return (&res);
		}
	} else {
		res.status = csastat2accessstat(stat);
		return (&res);
	}

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		/* convert to attributes */
		if ((stat = _DtCmsAppt4ToCmsentry(args->target, newa, &entry,
		    B_TRUE)) == CSA_SUCCESS) {

			key.time = p_key->tick;
			key.id = p_key->key;

			/* null out readonly attributes */
			_DtCm_free_cms_attributes(1,
				&entry->attrs[CSA_ENTRY_ATTR_ORGANIZER_I]);
			_DtCm_free_cms_attributes(1, &entry->attrs\
				[CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I]);
			_DtCm_free_cms_attributes(1,
				&entry->attrs[CSA_ENTRY_ATTR_TYPE_I]);

			/* update entry */
			if (option == do_all_4)
				stat = _DtCmsUpdateEntry(cal, user, access,
					&key, entry->num_attrs,
					&entry->attrs[1], &oldentry, NULL);
			else
				stat = _DtCmsUpdateInstances(cal, user,
					access, &key, (option == do_one_4 ?
					CSA_SCOPE_ONE : CSA_SCOPE_FORWARD),
					entry->num_attrs, &entry->attrs[1],
					&oldentry, &newentry);

			_DtCm_free_cms_entry(entry);
		}
	} else {

		if (option == do_all_4)
			stat = _DtCmsChangeAll(cal, user, access, p_key, newa,
				&olda);
		else
			stat = _DtCmsChangeSome(cal, user, access,p_key, newa,
				option, &olda);
	}

	if (stat == CSA_SUCCESS) {
		if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
			tmpappt.appt_id.tick = oldentry->key.time;
			tmpappt.appt_id.key = oldentry->key.id;
			olda = &tmpappt;
			_DtCm_free_cms_entry(oldentry);
			if (newentry) {
				newa->appt_id.key = newentry->key.id;
				_DtCm_free_cms_entry(newentry);
			}
		}

		/* If the date/time is changed, we do a callback
		 * with the old and new appointments.  Otherwise,
		 * we only do callback with the new appointmnt.
		 */
		if (APPT_TICK(newa) == APPT_TICK(olda)) {
			cal->rlist = _DtCmsDoV1Callback(cal->rlist, user,
					args->pid, newa);
		} else {
			olda->next = newa;
			cal->rlist = _DtCmsDoV1Callback(cal->rlist, user,
					args->pid, olda);
			olda->next = NULL;
		}

		cal->rlist = _DtCmsDoUpdateEntryCallback(cal->rlist,
				cal->calendar, user,
				(newa->appt_id.key == olda->appt_id.key ?
				0 : newa->appt_id.key),
				olda->appt_id.key, option, (option == do_all_4 ?
				olda->appt_id.tick : p_key->tick),
				args->pid);

		cal->modified = B_TRUE;

		/* Return the new appointment. */
		res.res.Table_Res_List_4_u.a = newa;
		res.status = access_ok_4;

		if (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION)
			_DtCm_free_appt4(olda);
	} else
		res.status = csastat2accessstat(stat);

	return (&res);
}

extern Table_Res_4 *
_DtCm_rtable_change_instance_4_svc(Table_Args_4 *args, struct svc_req *svcrq)
{
	static Table_Res_4 res;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_change_instance_4_svc called\n");

	res.status = access_notsupported_4;
	res.res.tag = AP_4;
	res.res.Table_Res_List_4_u.a = NULL;

	return(&res);
}

extern Table_Res_4 *
_DtCm_rtable_lookup_next_reminder_4_svc(
	Table_Args_4	*args,
	struct svc_req	*svcrq)
{
	static	Table_Res_4	res;
	CSA_return_code		stat;
	char			*user;
	uint			access;
	_DtCmsCalendar		*cal;
	Reminder_4		*p_reminder;
	Rm_que			*p_node;
	Rm_que			*p_prev;
	Rm_que			*p_new;
	Rm_que			*p_next;
	time_t			tick;
	cms_reminder_ref	*rems;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_lookup_next_reminder_4_svc called\n");

	/* clean up left over */
	if (res.res.Table_Res_List_4_u.r)
		_DtCm_free_reminder4(res.res.Table_Res_List_4_u.r);

	res.status = access_other_4;
	res.res.tag = RM_4;
	res.res.Table_Res_List_4_u.r = NULL;

	if (args->target == NULL)
		return (&res);

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		/* only user with owner rights can lookup reminders */
		if (!(access & CSA_OWNER_RIGHTS)) {

			res.status = access_failed_4;
			return (&res);
		}
	} else {
		res.status = csastat2accessstat(stat);
		return (&res);
	}

	tick = args->args.Args_4_u.tick;

	if (debug)
		fprintf(stderr, "Next reminder after %s", ctime(&tick));

	if (cal->fversion > 1) {
		if ((stat = _DtCmsLookupReminder(cal->remq, tick, 0, NULL,
		    &rems)) == CSA_SUCCESS) {
			stat = _DtCmsReminderRefToReminder(rems,
			    &res.res.Table_Res_List_4_u.r);
			_DtCmsFreeReminderRef(rems);
		}
	} else {

		stat = _DtCmsGetV4Reminders(cal, tick,
			&res.res.Table_Res_List_4_u.r, NULL);
	}

	res.status =  csastat2accessstat(stat);
	return (&res);
}

extern Table_Status_4 *
_DtCm_rtable_check_4_svc(Table_Args_4 *args, struct svc_req *svcrq)
{
	static	Table_Status_4 s;
	CSA_return_code	stat;
	Rb_Status rbstat;
	_DtCmsCalendar	*cal;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_check_4_svc called\n");

	if ((stat = _DtCmsGetCalendarByName(args->target, B_TRUE, &cal))
	    != CSA_SUCCESS)
	{
		s = csastat2tablestat(stat);
		return (&s);
	}

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		s = tbl_notsupported_4;
	} else {

		rbstat = rb_check_tree (cal->tree);
		if (rbstat == rb_ok)
			rbstat = hc_check_list (cal->list);

		if (rbstat == rb_ok)
			s = ok_4;
		else
			s = other_4;
	}

	return (&s);
}

extern Table_Status_4 *
_DtCm_rtable_flush_table_4_svc(Table_Args_4 *args, struct svc_req *svcrq)
{
	static Table_Status_4 s;
	Id_4		key;
	CSA_return_code		stat;
	_DtCmsCalendar	*cal;
	int		n = 0;
	Tree_node	*p_node;
	List_node	*p_lnode;
	List_node	*p_next;
	Appt_4	*p_appt;

	if (!debug) {
		s = ok_4;
		return (&s);
	}

	if (debug)
		fprintf (stderr, "rtable_flush_table\n");

	if ((stat = _DtCmsGetCalendarByName(args->target, B_TRUE, &cal))
	    != CSA_SUCCESS) {
		s = csastat2tablestat(stat);
		return (&s);
	}

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		s = tbl_notsupported_4;
		return (&s);
	}

	if (debug)
	{
		fprintf(stderr, "%s: before flush.. rbsize= %d\n", pgname,
			rb_size(APPT_TREE(cal))+hc_size(REPT_LIST(cal)));
	}
	
	/* Flushing the single appointment tree. */
	key.key = 0;
	key.tick = args->args.Args_4_u.tick;
	while (p_appt = (Appt_4 *) rb_lookup_next_larger(APPT_TREE(cal),
	    (caddr_t)&key)) {
		p_node = rb_delete (APPT_TREE(cal),
				(caddr_t)&(p_appt->appt_id));
		if (p_node != NULL)
		{
			n++;
			_DtCm_free_appt4 ((Appt_4 *) p_node->data);
			free(p_node);
		}
	}

	/* Flushing the repeating appointment list */
	key.key = 0;
	key.tick = args->args.Args_4_u.tick;
	p_lnode = cal->list->root;
	while (p_lnode != NULL)
	{
		p_next = hc_lookup_next (p_lnode);
		p_appt = (Appt_4*)p_lnode->data;
		if (APPT_TICK(p_appt) > key.tick)
		{
			n++;
			_DtCm_free_appt4 (p_appt);
			(void) hc_delete_node (REPT_LIST(cal), p_lnode);
			free (p_lnode);
		}
		p_lnode = p_next;
	}
	s = ok_4;
	if (debug)
	{
		fprintf (stderr, "%s: entries deleted= %d\n", pgname, n);
		fprintf (stderr, "%s: after flush.. rbsize= %d\n", pgname,
			rb_size(APPT_TREE(cal))+hc_size(REPT_LIST(cal)));
	}

	return (&s);
}

extern int *
_DtCm_rtable_size_4_svc(Table_Args_4 *args, struct svc_req *svcrq)
{
	/* must be static! */
	static int size;
	CSA_return_code stat;
	_DtCmsCalendar	*cal;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_size_4_svc called\n");

	size = 0;
	if ((stat = _DtCmsGetCalendarByName(args->target, B_TRUE, &cal))
	    != CSA_SUCCESS)
		return(&size);

	size = rb_size (APPT_TREE(cal)) + hc_size (REPT_LIST(cal));

	return(&size);
}

extern Registration_Status_4 *
_DtCm_register_callback_4_svc(Registration_4 *r, struct svc_req *svcrq)
{
	static Registration_Status_4 regstat;
	CSA_return_code stat;
	char *source;
	_DtCmsCalendar *cal = NULL;
        _DtCmsRegistrationInfo *copy=NULL;

	if (debug)
		fprintf(stderr, "_DtCm_register_callback_4_svc called\n");

	/* Error */
        if (r->target == NULL) {
		regstat = failed_4;
                return(&regstat);
	}

	if ((stat = _DtCmsGetClientInfo(svcrq, &source)) != CSA_SUCCESS) {
		regstat = csastat2regstat(stat);
		return (&regstat);
	}

	/* check if the target exists */
	if ((stat = _DtCmsGetCalendarByName(r->target, B_TRUE, &cal))
	    != CSA_SUCCESS) {
		regstat = csastat2regstat(stat);
		return (&regstat);
	}

	/* Check for duplicate registrations */
	if (_DtCmsGetRegistration(&(cal->rlist), source, r->prognum, r->versnum,
	    r->procnum, r->pid) == NULL) {
		/* not registered */

        	/* Make a copy of the callback info. */

		if ((copy = _DtCmsMakeRegistrationInfo(source, 0, r->prognum,
		    r->versnum, r->procnum, r->pid)) == NULL) {
			regstat = failed_4;
			return (&regstat);
		}

        	/* Store it away so that it can later be called. */
		copy->next = cal->rlist;
		cal->rlist = copy;

		if (debug) {
			fprintf(stderr, "%s requested registration on %s. registered pid= %d\n",
				source, r->target, r->pid);
			_DtCmsListRegistration(cal->rlist,
					cal->calendar);
		}
		regstat = registered_4;
        	return(&regstat);
	} else {
		/* already registered */
		regstat = registered_4;
		return(&regstat);
	}
}

/* de-register an rpc callback proc from the client */
extern Registration_Status_4 *
_DtCm_deregister_callback_4_svc(Registration_4 *r, struct svc_req *svcrq)
{
	static Registration_Status_4 regstat;
	CSA_return_code stat;
	_DtCmsCalendar *cal;
	char *source;
        _DtCmsRegistrationInfo *p = NULL, *q = NULL;

	if (debug)
		fprintf(stderr, "_DtCm_deregister_callback_4_svc called\n");

        if (r->target == NULL) {
		regstat = failed_4;
                return(&regstat);
	}
 
	if ((stat = _DtCmsGetClientInfo(svcrq, &source)) != CSA_SUCCESS) {
		regstat = csastat2regstat(stat);
		return (&regstat);
	}

	if ((stat = _DtCmsGetCalendarByName(r->target, B_FALSE, &cal))
	    != CSA_SUCCESS) {
		regstat = csastat2regstat(stat);
		return (&regstat);
	}

	if (cal == NULL) {
		regstat = failed_4;
		return (&regstat);
	}

	q = p = cal->rlist;
	while (p != NULL) {

		/* This says:
		 * 1) if the name of the caller requesting deregistration
		 * is the same as the original caller who requested
		 * requested registration, and
		 * 2) if the (transient) program, version, & procnum match
		 * the original registration, and
		 * 3) if the process id of the client matches the
		 *  orignal registration 
		 *  
		 *  ... only then is it ok to decommission the ticket.
		 */


		if ((strcmp(p->client, source)==0) &&
		    (p->prognum==r->prognum) &&
		    (p->versnum==r->versnum) &&
		    (p->procnum==r->procnum) &&
		    (p->pid==r->pid)) {	/* a match */
			if (debug) {
				fprintf(stderr, "%s requested deregistration on %s. registered pid= %d\n", source, r->target, r->pid);
			}
			if (p==q)
				cal->rlist = p->next;
			else
				q->next = p->next;
			_DtCmsFreeRegistrationInfo(p);
			if (debug) {
				_DtCmsListRegistration(cal->rlist,
					cal->calendar);
			}
			regstat = deregistered_4;
			return(&regstat);
		}
		q = p;
		p = p->next;
	}

	/* not found */
	regstat = failed_4;
	return(&regstat);
}

extern Access_Status_4 *
_DtCm_rtable_set_access_4_svc(Access_Args_4 *args, struct svc_req *svcrq)
{
	/* must be static! */
	static Access_Status_4	s;
	CSA_return_code		stat;
	char			*user;
	uint			access;
	_DtCmsCalendar		*cal = NULL;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_set_access_4_svc called\n");

	s = access_other_4;

	if (args->target == NULL)
		return (&s);

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		/* only user with owner rights can lookup reminders */
		if ((cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
		    !(access&(CSA_OWNER_RIGHTS|CSA_CHANGE_CALENDAR_ATTRIBUTES)))
		    || (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
		    !(access & CSA_OWNER_RIGHTS))) {

			s = access_failed_4;
			return (&s);
		}
	} else {
		s = csastat2accessstat(stat);
		return(&s);
        }

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		cms_attribute		attr;
		cms_attribute_value	attrval;
		cms_access_entry	*alist;

		if (args->access_list && (alist =
		    _DtCmsConvertV4AccessList(args->access_list)) == NULL) {
			s = csastat2accessstat(CSA_E_INSUFFICIENT_MEMORY);
			return (&s);
		}

		attr.name.name = CSA_CAL_ATTR_ACCESS_LIST;
		attr.name.num = 0;
		attr.value = &attrval;
		attrval.type = CSA_VALUE_ACCESS_LIST;
		attrval.item.access_list_value = alist;

		stat = _DtCmsUpdateCalAttributesAndLog(cal, 1, &attr, access);
	} else {
		stat = _DtCmsSetV4AccessListAndLog(cal, args->access_list);
	}

	if ((s = csastat2accessstat(stat)) == access_ok_4) {

		cms_attribute	attr;

		attr.name.name = CSA_CAL_ATTR_ACCESS_LIST;
		cal->rlist = _DtCmsDoUpdateCalAttrsCallback(cal->rlist,
				cal->calendar, user, 1, &attr, -1);
	}

	return(&s);
}

extern Access_Args_4 *
_DtCm_rtable_get_access_4_svc(Access_Args_4 *args, struct svc_req *svcrq)
{
	static Access_Args_4	res;
	CSA_return_code		stat;
	_DtCmsCalendar		*cal;
	char			*target, *user;
	uint			access;
	boolean_t		useronly = B_FALSE;
	cms_access_entry	aentry;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_get_access_4_svc called\n");

	if (res.target != NULL)
		free(res.target);
	if (res.access_list)
		_DtCm_free_access_list4(res.access_list);

	res.target = NULL;
	res.access_list = (Access_Entry_4 *) NULL;

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION &&
		    !_DTCMS_HAS_VIEW_CALENDAR_ATTR_ACCESS(access)) {
			useronly = B_TRUE;
		}
	} else {
		return (&res);
	}

	if (cal->fversion > 1) {
		if (useronly) {
			aentry.user = user;
			aentry.rights = access;
			aentry.next = NULL;
			res.access_list = _DtCmsConvertV5AccessList(&aentry,
						B_TRUE);
		} else {
			res.access_list = _DtCmsConvertV5AccessList(
					cal->attrs[CSA_CAL_ATTR_ACCESS_LIST_I].\
					value->item.access_list_value, B_TRUE);
		}
	} else {
		res.access_list = _DtCmsCalendarAccessList(cal);
		if (debug)
			_DtCmsShowAccessList (res.access_list);
	}

	res.target = strdup(args->target); 

	return (&res);
}

extern Table_Res_4 *
_DtCm_rtable_abbreviated_lookup_key_range_4_svc(
	Table_Args_4	*args,
	struct svc_req	*svcrq)
{
	static	Table_Res_4 res;
	CSA_return_code	stat;
	_DtCmsCalendar	*cal;
	char		*user;
	uint		access;
	Keyrange_4	*p_range;
	Abb_Appt_4	*abbr_r = NULL;

	if (debug)
		fprintf(stderr,
		    "_DtCm_rtable_abbreviated_lookup_key_range_4_svc called\n");

	if (res.res.Table_Res_List_4_u.b)
		_DtCm_free_abbrev_appt4(res.res.Table_Res_List_4_u.b);

	res.res.tag = AB_4;
	res.res.Table_Res_List_4_u.b = NULL;

	res.status = access_other_4;
	if ((p_range = args->args.Args_4_u.keyrange) == NULL)
		return (&res);

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		if (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
			if (_DTCMS_HAS_V4_BROWSE_ACCESS(access))
				res.status = access_ok_4;
			else
				res.status = access_failed_4;
		}
	} else {
		res.status = csastat2accessstat(stat);
		return (&res);
	}

	while (p_range != NULL)
	{

		if ((stat = _DtCmsLookupKeyrangeV4(cal, user, access,
		    B_FALSE, B_TRUE, p_range->tick1, p_range->tick2, 0, 0,
		    p_range->key, NULL, 0, NULL, NULL, NULL, &abbr_r))
		    != CSA_SUCCESS) {
			break;
		}

		p_range = p_range->next;
	}

	if (stat == CSA_SUCCESS)
		res.res.Table_Res_List_4_u.b = abbr_r;
	else
		_DtCm_free_abbrev_appt4(abbr_r);

	res.status = csastat2accessstat(stat);
	return (&res);
}

extern long *
_DtCm_rtable_gmtoff_4_svc(void *args, struct svc_req *svcrq)
{
	static long gmtoff;
#if !defined(CSRG_BASED)
	extern long timezone;
#else
	struct tm *t;
#endif

	if (debug)
		fprintf(stderr, "_DtCm_rtable_gmtoff_4_svc called\n");

#if defined(CSRG_BASED)
	t = localtime(time(NULL));
	gmtoff = t->tm_gmtoff;
#else
	gmtoff = timezone;
#endif
	return(&gmtoff);
}

extern Table_Status_4 *
_DtCm_rtable_create_4_svc(Table_Op_Args_4 *args, struct svc_req *svcrq)
{
	static	Table_Status_4 res;
	CSA_return_code stat;
	_DtCmsCalendar	*cal;
	char	*source;
	char	*calname;
	char	*log;
	char	*ptr;
	char	*id, *user, *domain;
	Access_Entry_4 aentry;

	if (debug)
		fprintf(stderr, "_DtCm_rtable_create_4_svc called\n");

	res = other_4;
	if ((stat = _DtCmsGetClientInfo(svcrq, &source)) != CSA_SUCCESS) {
		res = csastat2tablestat(stat);
		return (&res);
	}

	/* check domain if domain info is available */
	/* only user in the local domain can create file */
	if (ptr = strchr(source, '.')) {
		if (debug)
			fprintf(stderr, "rpc.cmsd: %s %s(target) and %s(sender)\n",
				"check domains, comparing",
				args->target, source);

		if ((domain = _DtCmsTarget2Domain(args->target)) != NULL) {

			if (!_DtCmIsSamePath(domain, ++ptr)) {
				res = denied_4;
				free(domain);
				return(&res);
			}
		}
	}

	if ((calname = _DtCmsTarget2Name(args->target)) == NULL)
		return(&res);

	/* if the file is loaded in memory, the file already exists */
	if ((stat = _DtCmsGetCalendarByName(calname, B_FALSE, &cal))
	    == CSA_SUCCESS && cal != NULL) {
		res = tbl_exist_4;
		free(calname);
		return(&res);
	}

	/*
	 * If identifier of the calendar name is a user name,
	 * make sure it's the same as sender.
	 * format of calendar name assumed: identifier.name
	 */
	if ((id = _DtCmGetPrefix(calname, '.')) == NULL) {
		free(calname);
		return(&res);
	}
	if ((user = _DtCmGetPrefix(source, '@')) == NULL) {
		free(calname);
		free(id);
		return(&res);
	}

	if (getpwnam(id) && strcmp(user, id)) {
		free(calname);
		free(id);
		free(user);
		res = denied_4;
		return(&res);
	}
	free(id);

	if ((log = _DtCmsGetLogFN(calname)) == NULL)
	{
		free(calname);
		free(user);
		return(&res);
	} else if (debug)
		fprintf(stderr, "new file = '%s'\n", log);

	if ((stat = _DtCmsCreateLogV1(user, log)) == CSA_E_CALENDAR_EXISTS)
		res = tbl_exist_4;
	else if (stat != CSA_SUCCESS)
		res = other_4;
	else
		res = ok_4;

	/* initialize the access list to be "WORLD", access_read_4 */
	aentry.next = NULL;
	aentry.who = WORLD;
	aentry.access_type = access_read_4;
	if ((stat = _DtCmsAppendAccessByFN(log, access_read_4, &aentry))
	    != CSA_SUCCESS) {
		unlink(log);
	}

	free(user);
	free(calname);
	free(log);
	return (&res);
}

extern Table_Status_4 *
_DtCm_rtable_remove_4_svc(Table_Op_Args_4 *args, struct svc_req *svcrq)
{
	static	Table_Status_4 res;

	res = tbl_notsupported_4;
	return (&res);
}

extern Table_Status_4 *
_DtCm_rtable_rename_4_svc(Table_Op_Args_4 *args, struct svc_req *svcrq)
{
	static	Table_Status_4 res;

	res = tbl_notsupported_4;
	return (&res);
}

extern void *
_DtCm_rtable_ping_4_svc(void *args, struct svc_req *svcrq)
{
	if (debug)
		fprintf(stderr, "_DtCm_rtable_ping_4_svc called\n");

	return(NULL); /* for RPC reply */
}

extern void
initrtable4(program_handle ph)
{
        ph->program_num = TABLEPROG;
        ph->prog[TABLEVERS_4].vers = &tableprog_4_table[0];
        ph->prog[TABLEVERS_4].nproc = sizeof(tableprog_4_table)/sizeof(tableprog_4_table[0]);
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

static Appt_4 *
rtable_lookup_internal(_DtCmsCalendar *cal, char **p_src, Id_4 *key)
{
	Appt_4	*p_appt;
	Privacy_Level_4	p_level;

	/* Check if it hits a single appointment */
	p_appt = (Appt_4 *)rb_lookup(APPT_TREE(cal), (caddr_t)key);

	if (p_appt != NULL) {
		switch (_DtCmCheckPrivacyLevel(p_src, p_appt)) {
		case public_4:
			p_appt = _DtCm_copy_one_appt4(p_appt);
			return(p_appt);
		case semiprivate_4:
			p_appt = _DtCm_copy_semiprivate_appt4(p_appt);
			return(p_appt);
		case private_4:
		default:
			return(NULL);
		}
	}
	
	/* Check if it hits an event in any repeating appointment */
	p_appt = (Appt_4 *) hc_lookup (REPT_LIST(cal), (caddr_t)key);

	if (p_appt != NULL) {
		if ((p_level = _DtCmCheckPrivacyLevel(p_src, p_appt)) != private_4)
		{
			if (_DtCms_in_repeater (key, p_appt, B_FALSE)) {
				if (p_level == public_4)
					p_appt = _DtCm_copy_one_appt4(p_appt);
				else
					p_appt = _DtCm_copy_semiprivate_appt4(p_appt);
				APPT_TICK(p_appt) = key->tick;
				return(p_appt);
			}
		}
	}

	return (NULL);
}

static Appt_4 *
repeater_next_larger(List_node *p_lnode, Id_4 *key)
{
	static Appt_4 appt;
	Period_4	period;
	Appt_4	*p_appt;
	Appt_4	*p_save = NULL;
	Id_4	id, next_larger_id;
	int	ord;
	int	ntimes;

	next_larger_id.tick = MAXINT;
	next_larger_id.key = MAXINT;
	while (p_lnode != NULL)
	{
		p_appt = (Appt_4*)p_lnode->data;

		/* check last tick: if it's before the lookup range, skip it */
		if (p_lnode->lasttick == 0)
			p_lnode->lasttick = _DtCms_last_tick_v4(APPT_TICK(p_appt),
				p_appt->period, p_appt->ntimes);

		if ((p_lnode->lasttick < key->tick) ||
		    (p_lnode->lasttick == key->tick &&
		    APPT_KEY(p_appt) <= key->key)) {
			p_lnode = hc_lookup_next(p_lnode);
			continue;
		}

		period = p_appt->period;
		ntimes = _DtCms_get_ninstance_v4(p_appt);
		id.tick = _DtCms_closest_tick_v4(key->tick, APPT_TICK(p_appt),
				period, &ord);
		ord--;
		id.key = APPT_KEY(p_appt);
		while (++ord <= ntimes)
		{
			if ((id.tick < key->tick) || (id.tick == key->tick &&
					id.key <= key->key))
		     		id.tick = _DtCms_next_tick_v4 (id.tick, period);
			else if (!_DtCms_marked_4_cancellation (p_appt, ord)) {
				if ((id.tick < next_larger_id.tick) ||
				    (id.tick == next_larger_id.tick &&
					id.key < next_larger_id.key))
				{
					next_larger_id = id;
					p_save = p_appt;
				}
				break;
			} else
				id.tick = _DtCms_next_tick_v4(id.tick, period);
		}
		p_lnode = hc_lookup_next (p_lnode);
	}

	if (p_save != NULL)
	{
		appt = *p_save;
		APPT_TICK(&appt) = next_larger_id.tick;
		p_save = &appt;
	}

	return (p_save);
}

static Appt_4 *
repeater_next_smaller(List_node *p_lnode, Id_4 *key)
{
	static Appt_4 appt;
	Appt_4	*p_appt;
	Period_4	period;
	Appt_4	*p_save = NULL;
	Id_4	id, next_smaller_id;
	int	ord;
	int	ntimes;

	next_smaller_id.tick = 0;
	next_smaller_id.key = 0;
	while (p_lnode != NULL)
	{
		p_appt = (Appt_4*)p_lnode->data;
		ord = 0;
		ntimes = _DtCms_get_ninstance_v4(p_appt);
		period = p_appt->period;
		id.tick = APPT_TICK(p_appt);
		id.key = APPT_KEY(p_appt);

		/* Very inefficient loop because it has to check if each
		 * instance is cancelled.  If there is a function to calculate
		 * last tick, this loop can be rewritten in an efficient way.
		 */
		while ((++ord <= ntimes) && (id.tick <= key->tick))
		{
			if (id.tick == key->tick && id.key >= key->key)
				/* this will get us out of the loop */
				/* faster than continue.	    */
				id.tick = _DtCms_next_tick_v4 (id.tick, period);
			else {
				if (!_DtCms_marked_4_cancellation (p_appt, ord)) {
					if ((id.tick > next_smaller_id.tick) ||
					    (id.tick == next_smaller_id.tick &&
						id.key > next_smaller_id.key))
					{
						next_smaller_id = id;
						p_save = p_appt;
					}
				}
				id.tick = _DtCms_next_tick_v4 (id.tick, period);
			}
		}

		p_lnode = hc_lookup_next (p_lnode);
	}

	if (p_save != NULL)
	{
		appt = *p_save;
		APPT_TICK(&appt) = next_smaller_id.tick;
		p_save = &appt;
	}

	return (p_save);
}

static Table_Res_4 *
table_lookup_next(
	Table_Args_4 *args,
	struct svc_req *svcrq,
	caddr_t (* rb_func)(),
	Appt_4 *(* rp_func)())
{
	static	Table_Res_4 res;
	CSA_return_code stat;
	Privacy_Level_4 p_level;
	Id_4	key;
	_DtCmsCalendar *cal;
	Appt_4	*p_appt;
	Appt_4	*p_appt1;
	char	*user;
	uint	access;

	if (res.res.Table_Res_List_4_u.a)
		_DtCm_free_appt4(res.res.Table_Res_List_4_u.a);

	res.status = access_other_4;
	res.res.tag = AP_4;
	res.res.Table_Res_List_4_u.a = NULL;

	switch (args->args.tag) {
	case TICK_4:
		key.tick = args->args.Args_4_u.tick;
		key.key = 0;
		break;
	case UID_4:
		key = args->args.Args_4_u.key->appt_id;
		break;
	default:
		return(&res);
	}

	if ((stat = _DtCmsV4LoadAndCheckAccess(svcrq, args->target, &user,
	    &access, &cal)) == CSA_SUCCESS) {
		if (cal->fversion < _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
			if (_DTCMS_HAS_V4_BROWSE_ACCESS(access))
				res.status = access_ok_4;
			else
				res.status = access_failed_4;
		}
	} else {
		res.status = csastat2accessstat(stat);
		return (&res);
	}

	if (cal->fversion >= _DtCM_FIRST_EXTENSIBLE_DATA_VERSION) {
		res.status = access_notsupported_4;
		return (&res);
	}

	p_appt = (Appt_4 *) (*rb_func) (APPT_TREE(cal), &key);
	p_appt1 = (*rp_func) (REPT_LIST(cal)->root, &key);

	if (p_appt1 != NULL) {
		if (rb_func == rb_lookup_next_larger) {
			if ((p_appt==NULL) ||
			    (APPT_TICK(p_appt) > APPT_TICK(p_appt1)) ||
			    ((APPT_TICK(p_appt) == APPT_TICK(p_appt1)) &&
				(APPT_KEY(p_appt) > APPT_KEY(p_appt1)))) {
				p_appt = p_appt1;
			}
		} else {
			if ((p_appt==NULL) ||
			    (APPT_TICK(p_appt) < APPT_TICK(p_appt1)) ||
			    ((APPT_TICK(p_appt) == APPT_TICK(p_appt1)) &&
				(APPT_KEY(p_appt) < APPT_KEY(p_appt1)))) {
				p_appt = p_appt1;
			}
		}
	}

	if (p_appt != NULL) {
		switch (_GetAccessLevel(user, access, p_appt)) {
		case public_4:
			p_appt = _DtCm_copy_one_appt4(p_appt);
			break;
		case semiprivate_4:
			p_appt = _DtCm_copy_semiprivate_appt4(p_appt);
			break;
		default:
			p_appt = NULL;
			break;
		}
	}

	res.res.Table_Res_List_4_u.a = p_appt;
	return (&res);
}

static Access_Status_4
csastat2accessstat(CSA_return_code stat)
{
	switch (stat) {
	case CSA_SUCCESS:
		return (access_ok_4);
	case CSA_E_CALENDAR_EXISTS:
		return (access_exists_4);
	case CSA_E_CALENDAR_NOT_EXIST:
		return (access_notable_4);
	case CSA_E_INSUFFICIENT_MEMORY:
	case CSA_E_NO_AUTHORITY:
		return (access_failed_4);
	case CSA_X_DT_E_BACKING_STORE_PROBLEM:
	case CSA_E_DISK_FULL:
		return (access_incomplete_4);
	case CSA_E_NOT_SUPPORTED:
		return (access_notsupported_4);
	case CSA_E_INVALID_PARAMETER:
	case CSA_E_FAILURE:
	case CSA_X_DT_E_ENTRY_NOT_FOUND:
	default:
		return (access_other_4);
	}
}

static Table_Status_4
csastat2tablestat(CSA_return_code stat)
{
	switch (stat) {
	case CSA_SUCCESS:
		return (ok_4);
	case CSA_E_CALENDAR_EXISTS:
		return (tbl_exist_4);
	case CSA_E_CALENDAR_NOT_EXIST:
		return (notable_4);
	case CSA_E_NOT_SUPPORTED:
		return (tbl_notsupported_4);
	case CSA_E_FAILURE:
	case CSA_E_INSUFFICIENT_MEMORY:
	case CSA_E_INVALID_PARAMETER:
	default:
		return (other_4);
	}
}

static Registration_Status_4
csastat2regstat(CSA_return_code stat)
{
	switch (stat) {
	case CSA_E_CALENDAR_NOT_EXIST:
		return(reg_notable_4);
	default:
		return (failed_4);
	}
}

