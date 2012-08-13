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
/* $XConsortium: delete.c /main/4 1995/11/09 12:43:26 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <time.h>
#if !defined(CSRG_BASED)
#include <values.h>
#endif
#ifdef SunOS
#include <sys/systeminfo.h>
#endif
#include "cmscalendar.h"
#include "delete.h"
#include "cm.h"
#include "tree.h"
#include "list.h"
#include "cmsdata.h"
#include "log.h"
#include "access.h"
#include "insert.h"
#include "v5ops.h"
#include "repeat.h"
#include "rerule.h"
#include "reutil.h"
#include "iso8601.h"
#include "attr.h"

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/
static boolean_t _InSequence(List_node *node, time_t time);
static CSA_return_code _AddException(cms_attribute *attr, time_t time);
static CSA_return_code _AddEndDateToRule(cms_attribute *attr, RepeatEvent *re,
					time_t time);
static void _TruncateExceptionDates(cms_entry *newe, time_t ltick);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

extern CSA_return_code
_DtCmsDeleteEntry(
	_DtCmsCalendar	*cal,
	char		*sender,
	uint		access,
	cms_key		*key,
	cms_entry	**entry_r)
{
	CSA_return_code	stat;
	cms_entry	*entry;
	List_node	*lnode = NULL;
	Tree_node	*tnode;

	if ((entry = (cms_entry *)rb_lookup(cal->tree, (caddr_t)key)) == NULL) {
		/* find entry in the repeating entry list */
		if ((lnode = hc_lookup_node(cal->list, (caddr_t)key)) == NULL)
			return (CSA_X_DT_E_ENTRY_NOT_FOUND);
		else
			entry = (cms_entry *)lnode->data;
	}

	if (entry == NULL)
		return (CSA_X_DT_E_ENTRY_NOT_FOUND);

	if (sender && (stat = _DtCmsCheckChangeAccess(sender, access, entry))
	    != CSA_SUCCESS)
		return (stat);

	if (lnode == NULL) {
		if ((tnode = rb_delete(cal->tree, (caddr_t)key)) != NULL) {
			_DtCmsObsoleteReminder4Entry(cal->remq, entry, NULL,
				0, B_FALSE);
			free(tnode);
		} else
			return (CSA_X_DT_E_ENTRY_NOT_FOUND);
	} else {
		_DtCmsObsoleteReminder4Entry(cal->remq, entry, lnode,
			0, B_FALSE);
		hc_delete_node(cal->list, lnode);
		free(lnode);
	}


	if (entry_r)
		*entry_r = entry;
	else
		_DtCm_free_cms_entry(entry);

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCmsDeleteEntryAndLog(
	_DtCmsCalendar	*cal,
	char		*sender,
	uint		access,
	cms_key		*key,
	cms_entry	**entry_r)
{
	CSA_return_code	stat;
	cms_entry	*entry;

	if ((stat = _DtCmsDeleteEntry(cal, sender, access, key, &entry))
	    == CSA_SUCCESS) {
		if ((stat = _DtCmsV5TransactLog(cal, entry, _DtCmsLogRemove))
		    != CSA_SUCCESS) {
			(void)_DtCmsInsertEntry(cal, entry);
			_DtCm_free_cms_entry(entry);
		} else if (entry_r)
			*entry_r = entry;
		else
			_DtCm_free_cms_entry(entry);
	}

	return (stat);
}

extern CSA_return_code
_DtCmsDeleteInstancesAndLog(
	_DtCmsCalendar	*cal,
	char		*sender,
	uint		access,
	cms_key		*key,
	int		scope,
	cms_entry	**newe,
	cms_entry	**olde)
{
	CSA_return_code	stat;
	cms_entry	*entry, *nentry;
	List_node	*lnode;
	int		fsize, count;
	uint		tmp_num;
	cms_attribute	*tmp_attrs, *aptr;
	boolean_t	delentry = B_FALSE;

	if ((lnode = hc_lookup_node(cal->list, (caddr_t)key)) == NULL)
		return (CSA_X_DT_E_ENTRY_NOT_FOUND);

	entry = (cms_entry *)lnode->data;

	if ((stat = _DtCmsCheckChangeAccess(sender, access, entry))
	    != CSA_SUCCESS)
		return (stat);

	if (_DtCmsInExceptionList(entry, key->time) ||
	    !_InSequence(lnode, key->time))
		return (CSA_X_DT_E_ENTRY_NOT_FOUND);

	if ((stat = _DtCm_copy_cms_entry(entry, &nentry)) != CSA_SUCCESS)
		return (stat);

	if (scope == CSA_SCOPE_ONE)
		stat = _AddException(
			&nentry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I],
			key->time);
	else {
		/* check whether we are deleting from fst instance */
		if (key->time == nentry->key.time)
			delentry = B_TRUE;
		else {
			stat = _DtCmsAddEndDateToRule(&nentry->attrs\
				[CSA_ENTRY_ATTR_RECURRENCE_RULE_I],
				lnode->re, key->time - 1);
			lnode->re->re_end_date = key->time - 1;

			_TruncateExceptionDates(nentry, key->time);
		}
	}

	if (stat != CSA_SUCCESS) {
		_DtCm_free_cms_entry(nentry);
		return (stat);
	}

	if (!delentry) {
		if ((stat = _DtCmsSetLastUpdate(nentry)) != SUCCESS) {
			_DtCm_free_cms_entry(nentry);
			return (stat);
		}

		/* remove original entry from log */
		if ((stat = _DtCmsGetFileSize(cal->calendar, &fsize))
		    != CSA_SUCCESS) {
			_DtCm_free_cms_entry(nentry);
			return (stat);
		}

		if ((stat = _DtCmsV5TransactLog(cal, entry, _DtCmsLogRemove))
		    != CSA_SUCCESS) {
			_DtCm_free_cms_entry(nentry);
			return (stat);
		}

		aptr = &nentry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I];
		count = CountEvents(nentry->key.time, lnode->re, (aptr->value ?
			aptr->value->item.date_time_list_value : NULL));
	}

	if (count == 0 || delentry) {
		/*
		 * *** obsolete reminders
		 */
		_DtCmsObsoleteReminder4Entry(cal->remq, entry, lnode,
			0, B_FALSE);

		hc_delete_node(cal->list, lnode);
		free(lnode);

		_DtCm_free_cms_entry(nentry);

		if (olde)
			*olde = entry;
		else
			_DtCm_free_cms_entry(entry);

		if (newe)
			*newe = NULL;

	} else {
		/* add new entry in memory and log in file */

		/* update the count */
		if (count == 1) {

			_DtCmsConvertToOnetime(nentry, lnode->re);
			stat = _DtCmsRbToCsaStat(rb_insert(cal->tree,
			    (caddr_t)nentry, (caddr_t)&(nentry->key)));

		} else if (count != RE_INFINITY) {
			nentry->attrs[CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I].\
				value->item.uint32_value = count;
		}

		/* the new entry should be copied when it's updated
		 * with all the info
		 */
		if (stat == CSA_SUCCESS && newe)
			stat = _DtCm_copy_cms_entry(nentry, newe);

		if (stat || (stat = _DtCmsV5TransactLog(cal, nentry,
		    _DtCmsLogAdd)) != CSA_SUCCESS) {
			_DtCmsTruncateFile(cal->calendar, fsize);
			_DtCm_free_cms_entry(nentry);
			if (newe) free(*newe);
		} else {
			if (count == 1) {
				_DtCmsObsoleteReminder4Entry(cal->remq, entry,
					lnode, 0, B_FALSE);
				hc_delete_node(cal->list, lnode);
				free(lnode);
				_DtCmsAddReminders4Entry(&cal->remq, nentry, NULL);
			} else {
				/* need to do the swap since the original entry
				 * pointer is stored in the reminder info
				 */
				tmp_num = entry->num_attrs;
				tmp_attrs = entry->attrs;
				entry->num_attrs = nentry->num_attrs;
				entry->attrs = nentry->attrs;
				nentry->num_attrs = tmp_num;
				nentry->attrs = tmp_attrs;

				_DtCmsObsoleteReminder4Entry(cal->remq, entry,
					lnode, key->time,
					(scope == CSA_SCOPE_ONE ? B_FALSE :
					B_TRUE));

				if (scope == CSA_SCOPE_FORWARD ||
				    key->time == lnode->lasttick) {
					lnode->lasttick = LastTick(
								entry->key.time,
								lnode->re);
				}
			}

			if (olde)
				*olde = (count == 1) ? entry : nentry;
			else
				_DtCm_free_cms_entry((count==1)?entry:nentry);
		}
	}

	return (stat);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

static boolean_t
_InSequence(List_node *node, time_t time)
{
	time_t		tick;
	cms_entry	*entry = (cms_entry *)node->data;
	RepeatEventState *restate;

	for (tick = ClosestTick(time, entry->key.time, node->re, &restate);
	    tick <= node->lasttick;
	    tick = NextTick(tick, entry->key.time, node->re, restate))
	{
		if (tick <= 0 || tick > node->lasttick)
			break;

		if (tick == time)
			return (B_TRUE);
	}

	return (B_FALSE);
}

static CSA_return_code
_AddException(cms_attribute *attr, time_t time)
{
	CSA_date_time_entry	*dt, *dlist, *prev;
	cms_attribute_value	*val;
	time_t			tick;
	char			buf[20];

	if ((dt = (CSA_date_time_entry *)calloc(1, sizeof(CSA_date_time_entry)))
	    == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if (_csa_tick_to_iso8601(time, buf)) {
		free(dt);
		return (CSA_E_INVALID_DATE_TIME);
	} else if ((dt->date_time = strdup(buf)) == NULL) {
		free(dt);
		return (CSA_E_INSUFFICIENT_MEMORY);
	}

	if (attr->value == NULL) {
		if ((val = (cms_attribute_value *)calloc(1,
		    sizeof(cms_attribute_value))) == NULL) {
			free(dt->date_time);
			free(dt);
			return (CSA_E_INSUFFICIENT_MEMORY);
		}
		attr->value = val;
		val->type = CSA_VALUE_DATE_TIME_LIST;
	}

	if (attr->value->item.date_time_list_value == NULL) {
		val->item.date_time_list_value = dt;
	} else {
		for (dlist = attr->value->item.date_time_list_value, prev=NULL;
		    dlist != NULL;
		    prev = dlist, dlist = dlist->next) {
			_csa_iso8601_to_tick(dlist->date_time, &tick);
			if (time <= tick)
				break;
		}

		dt->next = dlist;
		if (prev == NULL)
			attr->value->item.date_time_list_value = dt;
		else
			prev->next = dt;
	}

	return (CSA_SUCCESS);
}

static void
_TruncateExceptionDates(cms_entry *newe, time_t ltick)
{
	time_t			tick;
	CSA_date_time_list	dt, prev, head;

	if (newe->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value == NULL ||
	    newe->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value->item.\
	    date_time_list_value == NULL)
		return;

	head = newe->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value->item.\
		date_time_list_value;

	for (dt = head, prev = NULL; dt != NULL; prev = dt, dt = dt->next) {
		_csa_iso8601_to_tick(dt->date_time, &tick);
		if (ltick < tick) {
			if (prev) {
				prev->next = NULL;
				_DtCm_free_date_time_list(dt);
			} else {
				free(newe->attrs\
					[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].\
					value);
				newe->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].\
					value = NULL;
			}
			break;
		}
	}
}

