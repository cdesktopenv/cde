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
/* $XConsortium: update.c /main/4 1995/11/09 12:53:47 rswiston $ */
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
#include "update.h"
#include "cm.h"
#include "attr.h"
#include "updateattrs.h"
#include "cmsdata.h"
#include "cmsentry.h"
#include "access.h"
#include "repeat.h"
#include "delete.h"
#include "insert.h"
#include "log.h"
#include "v5ops.h"
#include "iso8601.h"
#include "rerule.h"
#include "reutil.h"

extern char *_DtCm_rule_buf;		/* buffer to hold a rule for parser */
extern RepeatEvent *_DtCm_repeat_info;	/* parsed recurrence info */

extern boolean_t RulesMatch(char *rule1, char *rule2);

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/
static boolean_t _SameRecurrenceRule(
			cms_attribute_value *newval,
			cms_attribute_value *oldval);

static CSA_return_code _SetNewStartDate(
			cms_entry	*olde,
			RepeatEvent	*oldre,
			cms_entry	*newe,
			RepeatEvent	*newre,
			cms_key		*key);

static CSA_return_code _AdjustStartEndTimeForUpdateInst(
			cms_entry	*newe,
			cms_entry	*olde,
			cms_key		*key,
			uint		num_attrs,
			cms_attribute	*attrs);

static CSA_return_code _AdjustStartEndTimeForUpdateEntry(
			List_node	*lnode,
			cms_entry	*newe,
			cms_key		*key,
			uint		num_attrs,
			cms_attribute	*attrs);

static void _GetStartEndIndex(
			uint		num_attrs,
			cms_attribute	*attrs,
			int		*starti,
			int		*endi);

static void _AdjustExceptionDates(cms_entry *entry, time_t delta);

static int _NumberExceptionDates(cms_entry *entry);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

extern CSA_return_code
_DtCmsUpdateEntry(
	_DtCmsCalendar	*cal,
	char		*sender,
	uint		access,
	cms_key		*key,
	uint		num_attrs,
	cms_attribute	*attrs,
	cms_entry	**oldentry,
	cms_entry	**newentry)
{
	CSA_return_code	stat;
	cms_entry	*olde, *newe;
	List_node	*lnode = NULL;
	int		file_size;

	/* get the entry from the tree */
	if ((olde = (cms_entry *)rb_lookup(cal->tree, (caddr_t)key)) == NULL) {
		/* find entry in the repeating entry list */
		if ((lnode = hc_lookup_node(cal->list, (caddr_t)key)) == NULL)
			return (CSA_X_DT_E_ENTRY_NOT_FOUND);
		else
			olde = (cms_entry *)lnode->data;
	}

	if (olde == NULL)
		return (CSA_X_DT_E_ENTRY_NOT_FOUND);

	/* check access rights */
	if ((stat = _DtCmsCheckChangeAccess(sender, access, olde))
	    != CSA_SUCCESS)
		return (stat);

	/* copy the entry and apply updates */
	if ((stat = _DtCm_copy_cms_entry(olde, &newe)) != CSA_SUCCESS)
		return (stat);

	if ((stat = _DtCmUpdateAttributes(num_attrs, attrs, &newe->num_attrs,
	    &newe->attrs, &cal->entry_tbl, B_FALSE, &cal->types, B_FALSE))
	    != CSA_SUCCESS) {
		_DtCm_free_cms_entry(newe);
		return (stat);
	}

	/* update start date */
	_csa_iso8601_to_tick(newe->attrs[CSA_ENTRY_ATTR_START_DATE_I].value->\
		item.date_time_value, &newe->key.time);

	if (lnode != NULL && (stat = _AdjustStartEndTimeForUpdateEntry(
	    lnode, newe, key, num_attrs, attrs)) != CSA_SUCCESS) {
		_DtCm_free_cms_entry(newe);
		return (stat);
	}

	/* make sure end time is not earlier than start time */
	if ((stat = _DtCmsCheckStartEndTime(newe)) != CSA_SUCCESS) {
		_DtCm_free_cms_entry(newe);
		return (stat);
	}

	/* set last update */
	if ((stat = _DtCmsSetLastUpdate(newe)) != CSA_SUCCESS) {
		_DtCm_free_cms_entry(newe);
		return (stat);
	}

	/* save file size in case we need to roll back */
	if ((stat = _DtCmsGetFileSize(cal->calendar, &file_size))
	    != CSA_SUCCESS) {
		_DtCm_free_cms_entry(newe);
		return (stat);
	}

	/* remove old entry */
	if ((stat = _DtCmsDeleteEntryAndLog(cal, NULL, 0, key, &olde))
	    != CSA_SUCCESS){
		_DtCm_free_cms_entry(newe);
		return (stat);
	}

	/* insert new entry */
	if ((stat = _DtCmsInsertEntryAndLog(cal, newe)) != CSA_SUCCESS) {

		_DtCmsTruncateFile(cal->calendar, file_size);
		_DtCmsInsertEntry(cal, olde);
		_DtCm_free_cms_entry(newe);
		_DtCm_free_cms_entry(olde);

	} else {
		 if (newentry)
			*newentry = newe;
		else
			_DtCm_free_cms_entry(newe);

		if (oldentry)
			*oldentry = olde;
		else
			_DtCm_free_cms_entry(olde);
	}

	return (stat);
}

extern CSA_return_code
_DtCmsUpdateInstances(
	_DtCmsCalendar	*cal,
	char		*sender,
	uint		access,
	cms_key		*key,
	int		scope,
	uint		num_attrs,
	cms_attribute	*attrs,
	cms_entry	**oldentry,
	cms_entry	**newentry)
{
	CSA_return_code	stat;
	cms_entry	*olde, *newe = NULL, *updatedold;
	List_node	*lnode = NULL;
	int		file_size;
	int		i, remain, rulei;
	cms_attribute_value *oaptr, *naptr;

	/* save file size in case we need to roll back */
	if ((stat = _DtCmsGetFileSize(cal->calendar, &file_size))
	    != CSA_SUCCESS)
		return (stat);

	/* remove old entry */
	if ((stat = _DtCmsDeleteInstancesAndLog(cal, sender, access, key, scope,
	    &updatedold, &olde))
	    != CSA_SUCCESS)
		return (stat);

	/* copy the entry and apply updates */
	if ((stat = _DtCm_copy_cms_entry(olde, &newe)) != CSA_SUCCESS)
		goto _cleanup;

	if ((stat = _DtCmUpdateAttributes(num_attrs, attrs, &newe->num_attrs,
	    &newe->attrs, &cal->entry_tbl, B_FALSE, &cal->types, B_FALSE))
	    != CSA_SUCCESS)
		goto _cleanup;

	/* check recurrence rule */
	for (i = 0, rulei = -1; i < num_attrs; i++) {
		if (attrs[i].name.num == CSA_ENTRY_ATTR_RECURRENCE_RULE_I) {
			rulei = i;
			break;
		}
	}

	if (scope == CSA_SCOPE_ONE) {
		if (rulei == -1) {
			_DtCmUpdateSint32AttrVal(NULL, &newe->attrs\
				[CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I].value);
			_DtCmUpdateStringAttrVal(NULL, &newe->attrs\
				[CSA_ENTRY_ATTR_RECURRENCE_RULE_I].value);
			_DtCmUpdateSint32AttrVal(NULL, &newe->attrs\
				[CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I].value);
			_DtCmUpdateSint32AttrVal(NULL, &newe->attrs\
				[CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I].value);
			_DtCmUpdateSint32AttrVal(NULL, &newe->attrs\
				[CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I].value);
			_DtCmUpdateSint32AttrVal(NULL, &newe->attrs\
				[CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I].\
				value);
			_DtCmUpdateStringAttrVal(NULL, &newe->attrs\
				[CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I].value);
		}
	} else {
		if (rulei == -1 ||
		    _SameRecurrenceRule(attrs[rulei].value, olde->\
		    attrs[CSA_ENTRY_ATTR_RECURRENCE_RULE_I].value)) {

			/*
			 * if recurrence info is not changed, replace
			 * the deleted part with the new one, i.e.,
			 * duration of new equals to the number of
			 * deleted instances of the old one.
			 * Also, update the exception list.
			 */

			if (newe->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value)
			{
				_DtCmsCleanupExceptionDates(newe, key->time);
			}

			oaptr = olde->attrs\
				[CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I].value;

			naptr = (updatedold == NULL) ? NULL :
				updatedold->attrs[\
				CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I].\
				value;

			if (oaptr->item.uint32_value == 0)
				remain = 0;
			else
				remain = oaptr->item.uint32_value -
					(naptr ? naptr->item.uint32_value : 0) +
					_DtCmsNumberExceptionDates(newe);

			if ((stat = _DtCmsUpdateDurationInRule(newe, remain))
			    != CSA_SUCCESS)
				goto _cleanup;
		}
	}

	if ((stat = _AdjustStartEndTimeForUpdateInst(newe, olde, key,
	    num_attrs, attrs)) != CSA_SUCCESS)
		goto _cleanup;

	/* set last update */
	if ((stat = _DtCmsSetLastUpdate(newe)) != CSA_SUCCESS)
		goto _cleanup;

	/* insert new entry */
	newe->key.id = 0;
	if ((stat = _DtCmsInsertEntryAndLog(cal, newe)) != CSA_SUCCESS) {

		goto _cleanup;

	}

	if (newentry)
		*newentry = newe;
	else
		_DtCm_free_cms_entry(newe);

	if (oldentry)
		*oldentry = olde;
	else
		_DtCm_free_cms_entry(olde);

	if (updatedold) _DtCm_free_cms_entry(updatedold);

	return (stat);

_cleanup:
	_DtCmsTruncateFile(cal->calendar, file_size);
	if (updatedold == NULL)
		_DtCmsInsertEntry(cal, olde);
	else {
		_DtCm_free_cms_entry(updatedold);
		if (lnode = hc_lookup_node(cal->list, (caddr_t)key)) {
			updatedold = (cms_entry *)lnode->data;
			lnode->data = (caddr_t)olde;
			olde = updatedold;
		}
	}
	_DtCm_free_cms_entry(olde);

	if (newe) _DtCm_free_cms_entry(newe);

	return (stat);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

static boolean_t
_SameRecurrenceRule(cms_attribute_value *newval, cms_attribute_value *oldval)
{
	if (newval == NULL || newval->item.string_value == NULL)
		return (B_FALSE);

	if (strcmp(newval->item.string_value, oldval->item.string_value))
		return (B_FALSE);
	else
		return (B_TRUE);
}

static CSA_return_code
_AdjustStartEndTimeForUpdateInst(
	cms_entry	*newe,
	cms_entry	*olde,
	cms_key		*key,
	uint		num_attrs,
	cms_attribute	*attrs)
{
	CSA_return_code	stat;
	time_t		oldbod, newbod, endtime, delta;
	int		i, starti, endi;

	/* update start date */
	_GetStartEndIndex(num_attrs, attrs, &starti, &endi);

	oldbod = _DtCmsBeginOfDay(olde->key.time);
	if (starti >= 0 && endi == -1 &&
	    newe->attrs[CSA_ENTRY_ATTR_END_DATE_I].value) {

		/* adjust end date */

		_csa_iso8601_to_tick(newe->attrs[CSA_ENTRY_ATTR_START_DATE_I].\
			value->item.date_time_value, &newe->key.time);
		newbod = _DtCmsBeginOfDay(newe->key.time);

		_csa_iso8601_to_tick(newe->attrs[CSA_ENTRY_ATTR_END_DATE_I].\
			value->item.date_time_value, &endtime);
		endtime += (newbod - oldbod);
		_csa_tick_to_iso8601(endtime, newe->attrs\
			[CSA_ENTRY_ATTR_END_DATE_I].value->item.\
			date_time_value);

	} else if (starti == -1 && endi >= 0) {
		/* ajust start date */
		if (newe->attrs[CSA_ENTRY_ATTR_END_DATE_I].value) {
			_csa_iso8601_to_tick(newe->attrs\
				[CSA_ENTRY_ATTR_END_DATE_I].value->\
				item.date_time_value, &endtime);
			newbod = _DtCmsBeginOfDay(endtime);
		} else
			newbod = _DtCmsBeginOfDay(key->time);

		newe->key.time += (newbod - oldbod);
		_csa_tick_to_iso8601(newe->key.time, newe->attrs\
			[CSA_ENTRY_ATTR_START_DATE_I].value->item.\
			date_time_value);

	} else if (starti == -1 && endi == -1) {
		/* adjust both start and end date */

		newe->key.time = key->time;
		_csa_tick_to_iso8601(key->time, newe->attrs[\
			CSA_ENTRY_ATTR_START_DATE_I].value->item.\
			date_time_value);

		if (newe->attrs[CSA_ENTRY_ATTR_END_DATE_I].value) {
			newbod = _DtCmsBeginOfDay(newe->key.time);

			_csa_iso8601_to_tick(newe->attrs\
				[CSA_ENTRY_ATTR_END_DATE_I].value->\
				item.date_time_value, &endtime);
			endtime += (newbod - oldbod);
			_csa_tick_to_iso8601(endtime, newe->attrs\
				[CSA_ENTRY_ATTR_END_DATE_I].value->\
				item.date_time_value);
		}
	} else {
		_csa_iso8601_to_tick(newe->attrs[CSA_ENTRY_ATTR_START_DATE_I].\
			value->item.date_time_value, &newe->key.time);
	}

	if ((stat = _DtCmsCheckStartEndTime(newe)) != CSA_SUCCESS) {
		return (stat);
	}

	if ((delta = _DtCmsTimeOfDay(newe->key.time) -
	    _DtCmsTimeOfDay(key->time)) != 0)
		_AdjustExceptionDates(newe, delta);

	return (CSA_SUCCESS);
}

static CSA_return_code
_SetNewStartDate(
	cms_entry	*olde,
	RepeatEvent	*oldre,
	cms_entry	*newe,
	RepeatEvent	*newre,
	cms_key		*key)
{
	if ((newe->key.time = DeriveNewStartTime(olde->key.time, oldre,
	    key->time, newe->key.time, newre)) == 0)
		return (CSA_E_FAILURE);

	if (_csa_tick_to_iso8601(newe->key.time, newe->attrs\
	    [CSA_ENTRY_ATTR_START_DATE_I].value->item.date_time_value))
		return (CSA_E_FAILURE);

	return (CSA_SUCCESS);
}

/*
 * The assumption is that there's only one instance per day.
 * When we support more than one instance per day, this
 * needs to be updated.
 */
static void
_AdjustExceptionDates(cms_entry *entry, time_t delta)
{
	time_t			tick;
	CSA_date_time_list	dt, head;

	if (entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value == NULL ||
	    entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value->item.\
	    date_time_list_value == NULL)
		return;

	head = entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value->item.\
		date_time_list_value;

	for (dt = head; dt != NULL; dt = dt->next) {
		_csa_iso8601_to_tick(dt->date_time, &tick);
		tick += delta;
		_csa_tick_to_iso8601(tick, dt->date_time);
	}
}

static void
_GetStartEndIndex(
	uint		num_attrs,
	cms_attribute	*attrs,
	int		*starti,
	int		*endi)
{
	int	i;

	for (i = 0, *starti = -1, *endi = -1; i < num_attrs; i++) {
		if (attrs[i].name.num == CSA_ENTRY_ATTR_START_DATE_I)
			*starti = i;
		else if (attrs[i].name.num == CSA_ENTRY_ATTR_END_DATE_I)
			*endi = i;
	}
}

static CSA_return_code
_AdjustStartEndTimeForUpdateEntry(
	List_node	*lnode,
	cms_entry	*newe,
	cms_key		*key,
	uint		num_attrs,
	cms_attribute	*attrs)
{
	CSA_return_code	stat;
	cms_entry	*olde = (cms_entry *)lnode->data;
	time_t		newbod, instbod, fstbod, newfstbod, endtime;
	int		starti, endi, enddelta = 0;
	cms_attribute_value *oldaptr, *newaptr, *endaptr;
	RepeatEvent	*newre;

extern	void _DtCm_rule_parser();

	/* check to make sure repeating type is not changed */
	oldaptr = olde->attrs[CSA_ENTRY_ATTR_RECURRENCE_RULE_I].value;
	newaptr = newe->attrs[CSA_ENTRY_ATTR_RECURRENCE_RULE_I].value;
	endaptr = newe->attrs[CSA_ENTRY_ATTR_END_DATE_I].value;
	if (endaptr)
		_csa_iso8601_to_tick(endaptr->item.date_time_value, &endtime);

	if (key->time != olde->key.time &&
	    (newaptr == NULL || newaptr->item.string_value == NULL ||
	    RulesMatch(oldaptr->item.string_value, newaptr->item.string_value)
	    == B_FALSE)) {
		return (CSA_E_INVALID_ATTRIBUTE_VALUE);
	}

	_GetStartEndIndex(num_attrs, attrs, &starti, &endi);
	newbod = _DtCmsBeginOfDay(newe->key.time);
	instbod = _DtCmsBeginOfDay(key->time);
	fstbod = _DtCmsBeginOfDay(olde->key.time);

	if (starti >= 0 && key->time != olde->key.time) {

		if (newbod == instbod) {
			/* keep the same start day */
			newe->key.time -= (newbod - fstbod); 
			_csa_tick_to_iso8601(newe->key.time,
				newe->attrs[CSA_ENTRY_ATTR_START_DATE_I].\
				value->item.date_time_value);

			if (endi >= 0)
				enddelta = newbod - fstbod;
		} else {
			/* parse the rule */
			_DtCm_rule_buf = newaptr->item.string_value;
			_DtCm_rule_parser();
			if ((newre = _DtCm_repeat_info) == NULL)
				return (CSA_E_INVALID_RULE);

			/* get new start date */
			if ((stat = _SetNewStartDate(olde, lnode->re,
			    newe, newre, key)) != CSA_SUCCESS) {
				_DtCm_free_re(newre);
				return (stat); 
			}
			_DtCm_free_re(newre);
			newfstbod = _DtCmsBeginOfDay(newe->key.time);
			if (endi < 0)
				enddelta = fstbod - newfstbod;
			else
				enddelta = newbod - newfstbod;
		}
	} else if (starti >= 0 && endi < 0 && newbod != fstbod) {

		enddelta = fstbod - newbod;

	} else if (starti < 0 && endi >= 0 && key->time != olde->key.time &&
		endaptr)
	{
		enddelta = _DtCmsBeginOfDay(endtime) - fstbod;
	}

	/* fix end date */
	if (enddelta && endaptr) {

		endtime -= enddelta;
		_csa_tick_to_iso8601(endtime, endaptr->item.date_time_value);
	}

	return (CSA_SUCCESS);
}

