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
/* $XConsortium: insert.c /main/5 1996/10/03 10:29:24 drk $ */
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
#include "insert.h"
#include "cm.h"
#include "cmscalendar.h"
#include "cmsdata.h"
#include "attr.h"
#include "delete.h"
#include "log.h"
#include "tree.h"
#include "list.h"
#include "iso8601.h"
#include "rerule.h"
#include "reutil.h"
#include "lutil.h"
#include "v5ops.h"
#include "repeat.h"

extern char *_DtCm_rule_buf;		/* buffer to hold a rule for parser */
extern RepeatEvent *_DtCm_repeat_info;	/* parsed recurrence info */

/******************************************************************************
 * forward declaration of static functions used within the file
 ******************************************************************************/
static boolean_t _IsOnetimeEntry(cms_entry *entry);
static CSA_return_code _RuleToRepeatInfo(cms_entry *entry, RepeatEvent *re);
static int _RuleToRepeatType(RepeatEvent *re);
static int _DailyRuleToRepeatType(RepeatEvent *re);
static int _WeeklyRuleToRepeatType(RepeatEvent *re);
static int _MonthlyRuleToRepeatType(RepeatEvent *re);

/*****************************************************************************
 * extern functions used in the library
 *****************************************************************************/

extern CSA_return_code
_DtCmsInsertEntry(_DtCmsCalendar *cal, cms_entry *entry)
{
	CSA_return_code	stat;
	Rb_Status	rb_stat;
	List_node	*lnode = NULL;
	cms_entry	*newptr;
	time_t		current_time;
	time_t		key, tick, endtime;
	char		*date, buf[80];
	CSA_opaque_data	opq;
	cms_attribute	*aptr;
	RepeatEvent	*re = NULL;
	RepeatEventState *res;
	extern		void _DtCm_rule_parser();
	uint		count;
	int		i;

	if (cal == NULL || entry == NULL)
		return (CSA_E_INVALID_PARAMETER);

	/* assign key if this is a new appointment */
	key = entry->key.id;
	_DtCmsGenerateKey(cal, &(entry->key.id));

	if (key == 0) {
		/* set start date */
		date = entry->attrs[CSA_ENTRY_ATTR_START_DATE_I].value->\
			item.date_time_value;
		_csa_iso8601_to_tick(date, &entry->key.time);

		/* set reference id */
		sprintf(buf, "%ld:%s@%s", entry->key.id, cal->calendar,
			_DtCmGetHostAtDomain());
		opq.size = strlen(buf);
		opq.data = (unsigned char *)buf;
		if ((stat = _DtCm_set_opaque_attrval(&opq,
		    &entry->attrs[CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I].value))		    != CSA_SUCCESS) {
			return (stat);
		}
	}

	/* check recurrence rule */
	if (_IsOnetimeEntry(entry) == B_FALSE) {
		/* check recurrence rule */
		aptr = &entry->attrs[CSA_ENTRY_ATTR_RECURRENCE_RULE_I];
		_DtCm_rule_buf = aptr->value->item.string_value;
		_DtCm_rule_parser();
		if ((re = _DtCm_repeat_info) == NULL)
			return (CSA_E_INVALID_RULE);

		/* get number of recurrences */
		aptr = &entry->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I];
		count = CountEvents(entry->key.time, re,
			(aptr->value ?
			aptr->value->item.date_time_list_value : NULL));

		if (count == 1) {
			/* turn into onetime entry */
			_DtCmsConvertToOnetime(entry, re);
			re = NULL;
		} else {

			if (count == 0)
				return (CSA_E_INVALID_RULE);
			else if (count == RE_INFINITY)
				count = CSA_X_DT_DT_REPEAT_FOREVER;

			if ((stat = _DtCm_set_uint32_attrval(count,
			    &entry->attrs[CSA_ENTRY_ATTR_NUMBER_RECURRENCES_I].\
			    value)) != CSA_SUCCESS)
				return (stat);

			/* adjust start date */
			tick = ClosestTick(entry->key.time, entry->key.time,
				re, &res);
			if (tick != entry->key.time &&
			    !_DtCmsInExceptionList(entry, tick)) {

				/* start date */
				_csa_tick_to_iso8601(tick, entry->attrs\
					[CSA_ENTRY_ATTR_START_DATE_I].value->\
					item.date_time_value);

				/* end date */
				_csa_iso8601_to_tick(entry->attrs\
					[CSA_ENTRY_ATTR_END_DATE_I].value->\
					item.date_time_value, &endtime);
				endtime += (tick - entry->key.time);
				_csa_tick_to_iso8601(endtime, entry->attrs\
					[CSA_ENTRY_ATTR_END_DATE_I].value->\
					item.date_time_value);

				/* start tick */
				entry->key.time = tick;
			}
		}
	}

	if ((stat = _DtCmsCheckStartEndTime(entry)) != CSA_SUCCESS)
		return (stat);

	if ((stat = _RuleToRepeatInfo(entry, re)) != CSA_SUCCESS)
		return (stat);
 
	if ((stat = _DtCm_copy_cms_entry(entry, &newptr)) != CSA_SUCCESS)
		return (stat);

	/* Add the entry into the data structure */
	if (re == NULL) {
		rb_stat = rb_insert (cal->tree, (caddr_t)newptr,
				(caddr_t)&(newptr->key));
	} else {
		rb_stat = hc_insert (REPT_LIST(cal), (caddr_t)newptr,
				(caddr_t)&(newptr->key), re, &lnode);
	}

	if (rb_stat == rb_ok) {
		/* Add the qualified reminder attrs to the reminder queue */
		_DtCmsAddReminders4Entry(&cal->remq, newptr, lnode);
	}

	return (_DtCmsRbToCsaStat(rb_stat));
}

extern CSA_return_code
_DtCmsInsertEntryAndLog(_DtCmsCalendar *cal, cms_entry *entry)
{
	CSA_return_code	stat;

	if ((stat = _DtCmsInsertEntry(cal, entry)) == CSA_SUCCESS) {
		/* append entry to the log file */
		if ((stat = _DtCmsV5TransactLog(cal, entry, _DtCmsLogAdd))
		    != CSA_SUCCESS) {
			(void)_DtCmsDeleteEntry(cal, NULL, 0, &entry->key,
				NULL); 
		}
	}
	return (stat);
}

/*****************************************************************************
 * static functions used within the file
 *****************************************************************************/

static boolean_t
_IsOnetimeEntry(cms_entry *entry)
{
	cms_attribute	*attr;

	if (entry->attrs[CSA_ENTRY_ATTR_RECURRENCE_RULE_I].value == NULL)
		return (B_TRUE);
	else
		return (B_FALSE);
}

static CSA_return_code
_RuleToRepeatInfo(cms_entry *entry, RepeatEvent *re)
{
	CSA_return_code	stat;
	uint	duration;
	char	buf[BUFSIZ];
	int	type;

	if (re == NULL)
	    return (_DtCm_set_sint32_attrval(CSA_X_DT_REPEAT_ONETIME,
		&entry->attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I].value)); 

	if ((stat = _DtCm_set_sint32_attrval(_RuleToRepeatType(re),
	    &entry->attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I].value))
	    != CSA_SUCCESS)
		return (stat);

	type = entry->attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I].value->\
		item.sint32_value;

	if (re->re_duration != RE_NOTSET) {
		if (re->re_duration == RE_INFINITY) {
			duration = 0;
		} else if (type == CSA_X_DT_REPEAT_EVERY_NDAY ||
		    type == CSA_X_DT_REPEAT_EVERY_NWEEK ||
		    type == CSA_X_DT_REPEAT_EVERY_NMONTH) {

			duration = re->re_duration * re->re_interval;
		} else
			duration = re->re_duration;

		if ((stat = _DtCm_set_uint32_attrval(duration,
		    &entry->attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I].value))
		    != CSA_SUCCESS)
			return (stat);
	}

	if ((stat = _DtCm_set_uint32_attrval(re->re_interval,
	    &entry->attrs[CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I].value))
	    != CSA_SUCCESS)
		return (stat);

	if (_csa_tick_to_iso8601(re->re_end_date, buf) == 0) {
		if ((stat = _DtCm_set_string_attrval(buf,
		    &entry->attrs[CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I].\
		    value, CSA_VALUE_DATE_TIME)) != CSA_SUCCESS)
			return (stat);
	}

	return (CSA_SUCCESS);
}

static int
_RuleToRepeatType(RepeatEvent *re)
{
	switch (re->re_type) {
	/* not supported in this release
	 * case RT_MINUTE:
	 */
	case RT_DAILY:
		return (_DailyRuleToRepeatType(re));

	case RT_WEEKLY:
		return (_WeeklyRuleToRepeatType(re));

	case RT_MONTHLY_POSITION:
		return (_MonthlyRuleToRepeatType(re));

	case RT_MONTHLY_DAY:
		return (_MonthlyRuleToRepeatType(re));

	case RT_YEARLY_MONTH:
		if ((re->re_data.re_yearly->yd_nitems == 1 ||
		    re->re_data.re_yearly->yd_nitems == 0) &&
		    re->re_interval == 1)
			return (CSA_X_DT_REPEAT_YEARLY);
		else
			return (CSA_X_DT_REPEAT_OTHER_YEARLY);

	case RT_YEARLY_DAY:
		return (CSA_X_DT_REPEAT_YEARLY);

	default:
		return (CSA_X_DT_REPEAT_OTHER);
	}
}

static int
_DailyRuleToRepeatType(RepeatEvent *re)
{
	if (re->re_interval == 1)
		return (CSA_X_DT_REPEAT_DAILY);
	else
		return (CSA_X_DT_REPEAT_EVERY_NDAY);
}

#define _DtCms_MON_TO_FRI_MASK	0x3e
#define _DtCms_MON_WED_FRI_MASK	0x2a
#define	_DtCms_TUE_THUR_MASK	0x14

static int
_WeeklyRuleToRepeatType(RepeatEvent *re)
{
	int	i, mask, temp;

	if (re->re_data.re_weekly->wd_ndaytime == 1 ||
	    re->re_data.re_weekly->wd_ndaytime == 0) {
		if (re->re_interval == 1)
			return (CSA_X_DT_REPEAT_WEEKLY);
		else if (re->re_interval == 2)
			return (CSA_X_DT_REPEAT_BIWEEKLY);
		else
			return (CSA_X_DT_REPEAT_EVERY_NWEEK);
	} else if (re->re_interval > 1)
		return (CSA_X_DT_REPEAT_OTHER_WEEKLY);

	/* check for MWF, M-F, TuTh */
	for (i = 0, mask = 0; i < re->re_data.re_weekly->wd_ndaytime; i++) {
		temp = re->re_data.re_weekly->wd_daytime[i].dt_day;
		temp = 0x1 << re->re_data.re_weekly->wd_daytime[i].dt_day;
		mask |= (0x1 << re->re_data.re_weekly->wd_daytime[i].dt_day);
	}

	if (mask == _DtCms_MON_TO_FRI_MASK)
		return (CSA_X_DT_REPEAT_MON_TO_FRI);
	else if (mask == _DtCms_MON_WED_FRI_MASK)
		return (CSA_X_DT_REPEAT_MONWEDFRI);
	else if (mask == _DtCms_TUE_THUR_MASK)
		return (CSA_X_DT_REPEAT_TUETHUR);
	else
		return (CSA_X_DT_REPEAT_WEEKDAYCOMBO);
}

static int
_MonthlyRuleToRepeatType(RepeatEvent *re)
{
	if (re->re_data.re_monthly->md_nitems == 1 ||
	    re->re_data.re_monthly->md_nitems == 0) {
		if (re->re_interval == 1)
			if (re->re_type == RT_MONTHLY_POSITION)
				return (CSA_X_DT_REPEAT_MONTHLY_BY_WEEKDAY);
			else
				return (CSA_X_DT_REPEAT_MONTHLY_BY_DATE);
		else
			return (CSA_X_DT_REPEAT_EVERY_NMONTH);
	} else
		return (CSA_X_DT_REPEAT_OTHER_MONTHLY);
}


