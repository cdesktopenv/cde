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
/* $XConsortium: v5ops.c /main/4 1995/11/09 12:55:44 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "v5ops.h"
#include "iso8601.h"
#include "attr.h"
#include "updateattrs.h"
#include "rerule.h"
#include "repeat.h"
#include "reutil.h"

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

extern _DtCmsComparisonResult
_DtCmsCompareEntry(cms_key *key, caddr_t data)
{
	cms_entry *entry = (cms_entry *)data;

	/* check the time only if it's not zero */
	if (key->time < entry->key.time)
		return (_DtCmsIsLess);
	if (key->time > entry->key.time)
		return (_DtCmsIsGreater);

	/* tick's are _DtCmsIsEqual */
	if (key->id < entry->key.id)
		return (_DtCmsIsLess);
	if (key->id > entry->key.id)
		return (_DtCmsIsGreater);

	return (_DtCmsIsEqual);
}

extern _DtCmsComparisonResult
_DtCmsCompareRptEntry(cms_key *key, caddr_t data)
{
	cms_entry *entry = (cms_entry *)data;

	if (key->id < entry->key.id)
		return (_DtCmsIsLess);
	if (key->id > entry->key.id)
		return (_DtCmsIsGreater);
	return (_DtCmsIsEqual);
}

extern caddr_t
_DtCmsGetEntryKey(caddr_t data)
{
	return ((caddr_t) &(((cms_entry *)data)->key));
}

extern CSA_return_code
_DtCmsSetLastUpdate(cms_entry *entry)
{
	char			datestr[20];
	cms_attribute_value	val;

	_csa_tick_to_iso8601(time(0), datestr);

	val.type = CSA_VALUE_DATE_TIME;
	val.item.date_time_value = datestr;

	/* CSA_ENTRY_ATTR_LAST_UPDATE_I */
	return (_DtCmUpdateStringAttrVal(&val,
		&entry->attrs[CSA_ENTRY_ATTR_LAST_UPDATE_I].value));
}

extern void
_DtCmsConvertToOnetime(cms_entry *entry, RepeatEvent *re)
{
	time_t			ctick, lasttick, diff = 0;
	RepeatEventState	*res;

	lasttick = LastTick(entry->key.time, re);
	for (ctick = ClosestTick(entry->key.time, entry->key.time, re, &res);
	    ctick <= lasttick;
	    ctick = NextTick(ctick, entry->key.time, re, res))
	{
		if (ctick <= 0 || !_DtCmsInExceptionList(entry, ctick))
			break;
	}

	if (ctick != entry->key.time) {
		if (entry->attrs[CSA_ENTRY_ATTR_END_DATE_I].value) {
			_csa_iso8601_to_tick(entry->attrs\
				[CSA_ENTRY_ATTR_END_DATE_I].value->item.\
				date_time_value, &diff);
			diff = diff - entry->key.time;
		}

		entry->key.time = ctick;
		_csa_tick_to_iso8601(ctick, entry->attrs\
			[CSA_ENTRY_ATTR_START_DATE_I].value->item.\
			date_time_value);

		if (entry->attrs[CSA_ENTRY_ATTR_END_DATE_I].value)
			_csa_tick_to_iso8601(ctick+diff, entry->attrs\
				[CSA_ENTRY_ATTR_END_DATE_I].value->item.\
				date_time_value);
	}

	if (entry->attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I].value)
		entry->attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I].value->item.\
			sint32_value = CSA_X_DT_REPEAT_ONETIME;
	else
		_DtCm_set_sint32_attrval(CSA_X_DT_REPEAT_ONETIME,
			&entry->attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I].value); 

	_DtCmUpdateDateTimeListAttrVal(NULL, &entry->attrs\
		[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value);

	_DtCmUpdateSint32AttrVal(NULL, &entry->attrs\
		[CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I].value);

	_DtCmUpdateStringAttrVal(NULL, &entry->attrs\
		[CSA_ENTRY_ATTR_RECURRENCE_RULE_I].value);

	_DtCmUpdateSint32AttrVal(NULL, &entry->attrs\
		[CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I].value);

	_DtCmUpdateSint32AttrVal(NULL, &entry->attrs\
		[CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I].value);

	_DtCmUpdateSint32AttrVal(NULL, &entry->attrs\
		[CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I].value);

	_DtCmUpdateStringAttrVal(NULL, &entry->attrs\
		[CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I].value);
}

extern int
_DtCmsGetDuration(cms_entry *eptr)
{
	time_t	stime, etime;

	if (eptr->attrs[CSA_ENTRY_ATTR_END_DATE_I].value) {
		_csa_iso8601_to_tick(eptr->attrs[CSA_ENTRY_ATTR_END_DATE_I].\
			value->item.date_time_value, &etime);

		_csa_iso8601_to_tick(eptr->attrs[CSA_ENTRY_ATTR_START_DATE_I].\
			value->item.date_time_value, &stime);

		return (etime - stime);
	} else
		return (0);
}

extern CSA_return_code
_DtCmsCheckInitialAttributes(cms_entry *entry)
{
	CSA_return_code	stat;
	cms_attribute	*attrs;
	char		datestr[80];

	if (entry == NULL)
		return (CSA_E_INVALID_PARAMETER);

	attrs = entry->attrs;

	/* fill in server generated value */

	/* CSA_ENTRY_ATTR_DATE_CREATED_I */
	_csa_tick_to_iso8601(time(0), datestr);
	if ((stat = _DtCm_set_string_attrval(datestr,
	    &attrs[CSA_ENTRY_ATTR_DATE_CREATED_I].value, CSA_VALUE_DATE_TIME))
	    != CSA_SUCCESS)
		return (stat);

	/* CSA_ENTRY_ATTR_LAST_UPDATE_I */
	if ((stat = _DtCm_set_string_attrval(datestr,
	    &attrs[CSA_ENTRY_ATTR_LAST_UPDATE_I].value, CSA_VALUE_DATE_TIME))
	    != CSA_SUCCESS)
		return (stat);

	/* fill in default values when not specified */
	if (attrs[CSA_ENTRY_ATTR_SUMMARY_I].value == NULL &&
	    (stat = _DtCm_set_string_attrval("",
	    &attrs[CSA_ENTRY_ATTR_SUMMARY_I].value, CSA_VALUE_STRING))
	    != CSA_SUCCESS)
		return (stat);

	if (attrs[CSA_X_DT_ENTRY_ATTR_SHOWTIME_I].value == NULL &&
	    (stat = _DtCm_set_sint32_attrval(1,
	    &attrs[CSA_X_DT_ENTRY_ATTR_SHOWTIME_I].value)) != CSA_SUCCESS)
		return (stat);

	if (attrs[CSA_ENTRY_ATTR_CLASSIFICATION_I].value == NULL &&
	    (stat = _DtCm_set_uint32_attrval(CSA_CLASS_PUBLIC,
	    &attrs[CSA_ENTRY_ATTR_CLASSIFICATION_I].value)) != CSA_SUCCESS)
		return (stat);

	if (attrs[CSA_ENTRY_ATTR_STATUS_I].value == NULL &&
	    (stat = _DtCm_set_uint32_attrval(CSA_X_DT_STATUS_ACTIVE,
	    &attrs[CSA_ENTRY_ATTR_STATUS_I].value)) != CSA_SUCCESS)
		return (stat);

	if (attrs[CSA_ENTRY_ATTR_SUBTYPE_I].value == NULL &&
	    attrs[CSA_ENTRY_ATTR_TYPE_I].value->item.uint32_value ==
	    CSA_TYPE_EVENT &&
	    (stat = _DtCm_set_string_attrval(CSA_SUBTYPE_APPOINTMENT,
	    &attrs[CSA_ENTRY_ATTR_SUBTYPE_I].value, CSA_VALUE_STRING))
	    != CSA_SUCCESS)
		return (stat);

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCmsCheckStartEndTime(cms_entry *entry)
{
	time_t	endtime;

	if (entry->attrs[CSA_ENTRY_ATTR_END_DATE_I].value) {
		_csa_iso8601_to_tick(entry->attrs[CSA_ENTRY_ATTR_END_DATE_I].\
			value->item.date_time_value, &endtime);
		if (endtime < entry->key.time)
			return (CSA_E_INVALID_ATTRIBUTE_VALUE);
		else
			return (CSA_SUCCESS);
	}
	return (CSA_SUCCESS);
}

extern void
_DtCmsCleanupExceptionDates(cms_entry *entry, long ftick)
{
	time_t			tick;
	CSA_date_time_list	dt, prev, head;

	if (entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value == NULL ||
	    entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value->item.\
	    date_time_list_value == NULL)
		return;

	head = entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value->item.\
		date_time_list_value;

	for (dt = head, prev = NULL; dt != NULL; prev = dt, dt = dt->next) {
		_csa_iso8601_to_tick(dt->date_time, &tick);
		if (ftick <= tick) {
			if (prev) {
				prev->next = NULL;
				_DtCm_free_date_time_list(head);
			}
			break;
		}
	}

	if (dt == NULL) {
		free(entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value);
		entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value = NULL;
	} else
		entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value->item.\
			date_time_list_value = dt;
}

extern int
_DtCmsNumberExceptionDates(cms_entry *entry)
{
	cms_attribute_value	*vptr;
	CSA_date_time_list	dt;
	int			count;

	if ((vptr = entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value)
	    == NULL)
		return (0);

	count = 0;
	for (dt = vptr->item.date_time_list_value; dt != NULL; dt = dt->next)
		count++;

	return (count);
}

extern CSA_return_code
_DtCmsUpdateDurationInRule(cms_entry *entry, uint remain)
{
	char			*newrule, *ptr;
	char			buf[BUFSIZ];
	cms_attribute_value	*vptr;

	vptr = entry->attrs[CSA_ENTRY_ATTR_RECURRENCE_RULE_I].value;
	if ((newrule = malloc(strlen(vptr->item.string_value) + 20)) == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	sprintf(buf, "#%d", remain);
	if (ptr = strchr(vptr->item.string_value, '#')) {
		*ptr = '\0';
		strcpy(newrule, vptr->item.string_value);
		strcat(newrule, buf);
		if (ptr = strchr(ptr + 1, ' '))
			strcat(newrule, ptr);
	} else {
		if (ptr = strchr(vptr->item.string_value, ' ')) {
			*ptr = '\0';
			sprintf(newrule, "%s %s %s", vptr->item.string_value,
				buf, ptr+1);
		} else
			sprintf(newrule, "%s %s", vptr->item.string_value, buf);
	}

	free (vptr->item.string_value);
	entry->attrs[CSA_ENTRY_ATTR_RECURRENCE_RULE_I].value->item.string_value
		= newrule;

	entry->attrs[CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I].value->\
		item.uint32_value = remain;

	return (CSA_SUCCESS);
}

extern CSA_return_code
_DtCmsAddEndDateToRule(cms_attribute *attr, RepeatEvent *re, long time)
{
	char	*newrule, *ptr;
	char	buf[20];

	if (_csa_tick_to_iso8601(time, buf))
		return (CSA_E_INVALID_DATE_TIME);

	if ((newrule = malloc(strlen(attr->value->item.string_value)+20))
	    == NULL)
		return (CSA_E_INSUFFICIENT_MEMORY);

	if (re->re_end_date == 0) {
		sprintf(newrule, "%s %s", attr->value->item.string_value, buf);
	} else {
		/* end date is always at the end of the rule */
		strcpy(newrule, attr->value->item.string_value);
		ptr = strrchr(newrule, ' ');
		sprintf(ptr, " %s", buf);
	}

	free(attr->value->item.string_value);
	attr->value->item.string_value = newrule;

	return (CSA_SUCCESS);
}


