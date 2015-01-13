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
/* $XConsortium: reutil.c /main/7 1996/11/21 19:47:12 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "rerule.h"
#include "reutil.h"
#include "repeat.h"

#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>

extern int monthdays[12];
extern RepeatEvent      *_DtCm_repeat_info;
extern char             *_DtCm_rule_buf;
extern void 		 _DtCm_rule_parser();
extern char 		*ReToString(RepeatEvent *);


/*
 * Check to make sure the current interval number is not greater than the
 * duration.
 */
int
InTimeRange(
        const unsigned int    ninterval,
        const Duration        duration)
{
        if (duration == RE_INFINITY || duration == RE_NOTSET) return TRUE;

        if (ninterval >= duration)
                return FALSE;

        return TRUE;
}

/*
 * Given two days of the week, compute the forward difference between them. 
 */
int
GetWDayDiff(
        const int     start,
        const int     end)
{
        if (start <= end)
                return end - start;

        return (7 - (start - end));
}

/*
 * Returns true if a day exists in the specified month and year.
 */
int
DayExists(
        const int     day,
        const int     month,
        const int     year)
{
	int valid_day = DayOfMonth(day, month, year);

        if (valid_day < 29) return TRUE;

	/* month = 0 = January */
        if ((month == 1) && leapyr(year + 1900)) {
                if (valid_day == 29)
                        return TRUE;
                else
                        return FALSE;
        }

        if (valid_day <= monthdays[month]) 
                return TRUE;

        return FALSE;
}

/* 
 * Given a date (specifically a month and year) determine if any of the
 * occurences of a weekday (e.g. 4th Sunday) exist in the given month.
 */
int
OccurenceExists(
        const WeekDayTime	*wdt_list,
	const unsigned int	 nwdt_list,
        const Tick     	 	 date)
{
	int 		 i, j, k;

        for (i = 0; i < nwdt_list; i++) {
                for (j = 0; j < wdt_list[i].wdt_nweek; j++) {
                	for (k = 0; k < wdt_list[i].wdt_nday; k++) {
				if (WeekNumberToDay(date,
						wdt_list[i].wdt_week[j],
						wdt_list[i].wdt_day[k])) {
					return TRUE;
				}
			}
                }
        }

	return FALSE;
}

WeekNumber
GetWeekNumber(
        const Tick date)
{
        switch (DayToWeekNumber(date)) {
        case 1:
                return WK_F1;
        case 2:
                return WK_F2;
        case 3:
                return WK_F3;
        case 4:
                return WK_F4;
        case 5:
                return WK_F5;
        default:
                return WK_L1;
        }
}

/*
 * Calculate the position of a week day (e.g. SU) in the month:
 *      7/1/94  would return 1 since this is the first Friday of the month.
 *      7/6/94  would return 1 even though it is in the second week since this
 *              is the first Wed of the month.
 *      7/15/94 would return 3.
 */
WeekNumber
DayToWeekNumber(
        const Tick date)
{
	_Xltimeparams	 localtime_buf;
        struct tm       *date_tm = _XLocaltime(&date, localtime_buf);
        int              week_number;

        week_number = date_tm->tm_mday / 7;
 
        if (date_tm->tm_mday % 7)
                week_number++;
 
        return week_number;
}

/*
 * Given a week number and a day of the week determine what day of the month
 * it falls on given a month in ``date''.
 */
Tick
WeekNumberToDay(
        const Tick            date,
        const WeekNumber      week,
        const WeekDay         weekday)
{
	_Xltimeparams	 localtime_buf;
        struct tm       *date_tm = _XLocaltime(&date, localtime_buf);
        int              first_weekday,
                         last_weekday,
                         day_of_month,
                         initial_month_number = date_tm->tm_mon;
	Tick		 _date;

        /* From the first day (or last day in the WK_L* cases) of the month
         * work forward (or backward) to find the weekday requested. 
         */
        if (week <= (const WeekDay)WK_F5) {
                day_of_month = 1;
                first_weekday = fdom(date);
                if (weekday != first_weekday)
                        day_of_month += GetWDayDiff(first_weekday, weekday);
        } else {
                day_of_month = monthlength(date);
                last_weekday = ldom(date);
                if (weekday != last_weekday)
                        day_of_month -= GetWDayDiff(weekday, last_weekday);
        }

        /* Now move forward or backward through the month added or subtracting
         * the appropriate number of weeks to get to the correct location.
         */
        if (week <= (const WeekDay)WK_F5) {
                date_tm->tm_mday = day_of_month + (int)week * 7;
        } else {
                /* ((int)week - WK_L1) normalizes the WK_L* to the values
                 * of 0 to 4.  See the WeekNumber enum.
                 */
                date_tm->tm_mday = day_of_month - ((int)week - WK_L1) * 7;
        } 

        date_tm->tm_isdst = -1;
        _date = mktime(date_tm);
        date_tm = _XLocaltime(&_date, localtime_buf);

        /* It is possible that the requested week number is not in this
         * month.
         */
        if (date_tm->tm_mon != initial_month_number)
                return ((Tick)NULL);

        return (_date);
}

unsigned int
DayOfMonth(
        const int     day,
        const int     month,
        const int     year)
{

        if (day != RE_LASTDAY) return day;

	/* month = 0 = January */
        if ((month == 1) && leapyr(year + 1900))
		return 29;
	else
		return monthdays[month];
}

int
same_week(
	struct tm	*tm1,
	struct tm	*tm2)
{
	struct tm	 tm11 = *tm1;
	struct tm	 tm22 = *tm2;
	Tick		 time1, time2;
	_Xltimeparams	 localtime_buf;

	tm11.tm_mday -= tm11.tm_wday;
	tm22.tm_mday -= tm22.tm_wday;
        time1 = mktime(&tm11);
        time2 = mktime(&tm22);
        tm11 = *_XLocaltime(&time1, localtime_buf);
        tm22 = *_XLocaltime(&time2, localtime_buf);

	if (tm11.tm_yday == tm22.tm_yday)
		return TRUE;

	return FALSE;
}

Tick
DeriveNewStartTime(
	const Tick	 start_time,
	RepeatEvent	*old_re,
	const Tick	 current_time,
	const Tick	 target_time,
	RepeatEvent	*new_re)
{
	Tick		 end_date;
	Tick		 an_event;
	int		 num_events;
	RepeatEventState *res;

	/* Count the number of events from the start time to the current time */
	end_date = old_re->re_end_date;
        old_re->re_end_date = current_time;
	/* XXX: Need to deal with excluded events */
        num_events = CountEvents(start_time, old_re, NULL);
        old_re->re_end_date = end_date;
 
	if (!ClosestTick(current_time, start_time, old_re, &res)) {
		/* XXX: Are we at the last tick or are there other problems? */
		return 0;
	}

	an_event = target_time;
 
	/* Walk backwards from the new target time to where the new start
	 * time should be.
	 */
	while (--num_events &&
	       (an_event = PrevTick(an_event, 0, new_re, res))) {
	       ;
	}

	return an_event;
}

/*
 * Return True if rule1 is the same as rule2.  This function returns True
 * if the rule portion of the rules are == reguardless of the duration or 
 * end_date's. 
 * re1 should point to the RepeatEvent struct for rule1. If you pass in
 * NULL for re1 the function *may* parse rule1 for you and return the
 * RepeatEvent struct through re1.  You will need to free it.
 * The same applies for re2.
 */
boolean_t
RulesMatch(
	char		*rule1,
	char		*rule2)
{
	Duration	 old_duration;
	time_t		 old_end_date;
	char		*new_rule1,
			*new_rule2;
	RepeatEvent	*re1,
			*re2;

	/* If rules are the same then we are done */
	if (!strcmp(rule1, rule2))
		return TRUE;

	_DtCm_rule_buf = rule1;
	_DtCm_rule_parser();
	if (!_DtCm_repeat_info) {
		/* Bad rule - fail */
		return FALSE;
	}
	re1 = _DtCm_repeat_info;

	_DtCm_rule_buf = rule2;
	_DtCm_rule_parser();
	if (!_DtCm_repeat_info) {
		/* Bad rule - fail */
		_DtCm_free_re(re1);
		return FALSE;
	}
	re2 = _DtCm_repeat_info;

	/* If rule1 != rule2 and the duration and end_date are the same
	 * then the rules themselves must be different.
	 */
	if (re1->re_duration == re2->re_duration &&
	    re1->re_end_date == re2->re_end_date) {
		_DtCm_free_re(re1);
		_DtCm_free_re(re2);
		return FALSE;
	}

	
	/* If the duration or end_date are different, the rules themselves
	 * may still be different.  So we make the durations and end_dates
	 * the same and reconstruct the rules.
	 */
	old_duration = re2->re_duration;
	old_end_date = re2->re_end_date;

	re2->re_duration = re1->re_duration;
	re2->re_end_date = re1->re_end_date;

	new_rule1 = ReToString(re1);
	new_rule2 = ReToString(re2);

	re2->re_duration = old_duration;
	re2->re_end_date = old_end_date;

	if (!strcmp(new_rule1, new_rule2)) {
		_DtCm_free_re(re1);
		_DtCm_free_re(re2);
		free(new_rule1);
		free(new_rule2);
		return TRUE;
	}
	free(new_rule1);
	free(new_rule2);
	_DtCm_free_re(re1);
	_DtCm_free_re(re2);
	return FALSE;
}
