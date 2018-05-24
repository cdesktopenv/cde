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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: relasttick.c /main/6 1996/11/21 19:46:04 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#if defined(__linux__)
#undef SVR4
#endif
#include <X11/Xos_r.h>

#include <stdlib.h>
#include "rerule.h"
#include "repeat.h"
#include "reutil.h"

static Tick DoMinute(const Tick, const RepeatEvent *);
static Tick DoDay(const Tick, const RepeatEvent *);
static Tick DoWeek(const Tick, RepeatEvent *);
static Tick DoMonthDay(const Tick, const RepeatEvent *);
static Tick DoMonthPos(const Tick, RepeatEvent *);
static Tick DoYearByMonth(const Tick, RepeatEvent *);
static Tick DoYearByDay(const Tick, RepeatEvent *);
static Tick LastOccurence(const Tick, const WeekDayTime *, const unsigned int);
static Tick LastTickFromEndDate(const Tick, const RepeatEvent *);
static int LastDayExists(const struct tm *, const unsigned int, 
			 const unsigned int *);
extern void FillInRepeatEvent(const Tick, RepeatEvent *);
extern Tick ClosestTick(const Tick, const Tick, RepeatEvent *, 
			RepeatEventState **);
extern Tick PrevTick(const Tick, const Tick, RepeatEvent *, RepeatEventState *);

Tick
LastTick(
	const Tick		 start_time,
        RepeatEvent		*re)
{
	Tick 			 last_time;

	if (!re) return (Tick)NULL;

	if (re->re_duration == RE_INFINITY) return EOT;

	FillInRepeatEvent(start_time, re);

	switch (re->re_type) {
	case RT_MINUTE:
		last_time = DoMinute(start_time, re);
		break;
	case RT_DAILY:
		last_time = DoDay(start_time, re);
		break;
	case RT_WEEKLY:
		last_time = DoWeek(start_time, re);
		break;
	case RT_MONTHLY_POSITION:
		last_time = DoMonthPos(start_time, re);
		break;
	case RT_MONTHLY_DAY:
		last_time = DoMonthDay(start_time, re);
		break;
	case RT_YEARLY_MONTH:
		last_time = DoYearByMonth(start_time, re);
		break;
	case RT_YEARLY_DAY:
		last_time = DoYearByDay(start_time, re);
		break;
	}

	return last_time;
}

static Tick
DoMinute(
	const Tick		 start_time,
	const RepeatEvent	*re)
{
	return (Tick)NULL;
}

static Tick
DoDay(
	const Tick		 start_time,
	const RepeatEvent	*re)
{
	Tick			 last_time1 = EOT,
				 last_time2 = EOT;

	if (re->re_end_date) {
		last_time1 = LastTickFromEndDate(start_time, re);
	}

	if (re->re_duration != RE_NOTSET) {
		struct tm	*start_tm;
		_Xltimeparams	 localtime_buf;

		start_tm = _XLocaltime((const time_t *)&start_time, localtime_buf);

		/* Go to the last day an event can happen on. */
		start_tm->tm_mday += (re->re_duration - 1) * re->re_interval;
		start_tm->tm_isdst = -1;

		/* Go to the last time an event can happen on the last day. */
		if (RE_DAILY(re)->dd_ntime) {
			start_tm->tm_hour =
			RE_DAILY(re)->dd_time[RE_DAILY(re)->dd_ntime - 1] / 100;
			start_tm->tm_min = 
			RE_DAILY(re)->dd_time[RE_DAILY(re)->dd_ntime - 1] % 100;
		}
		last_time2 = mktime(start_tm);
	}

	return ((last_time1 < last_time2) ? last_time1 : last_time2);
}

static Tick
DoWeek(
	const Tick		 start_time,
	RepeatEvent		*re)
{
	struct tm               *start_tm;
	unsigned int		 wd_ndaytime = RE_WEEKLY(re)->wd_ndaytime,
				 dt_ntime,
				 start_hour,
				 start_min;
	Tick			 _start_time = start_time,
				 last_time1 = EOT,
				 last_time2 = EOT;
	RepeatEventState        *res;
	_Xltimeparams		 localtime_buf;

	if (re->re_end_date) {
		last_time1 = LastTickFromEndDate(start_time, re);

		if (re->re_duration == RE_NOTSET)
			return last_time1;
	}

	/* Find the real start time */
	_start_time = ClosestTick(start_time, start_time, re, &res);
	free(res);
	start_tm = _XLocaltime((const time_t *)&_start_time, localtime_buf);

	start_hour = start_tm->tm_hour;
	start_min = start_tm->tm_min;

	/* Go to the last day an event can happen on. */
	start_tm->tm_mday += (re->re_duration - 1) * re->re_interval * 7;
	start_tm->tm_isdst = -1;

	if (wd_ndaytime) {
        	_start_time = mktime(start_tm);
        	start_tm = _XLocaltime((const time_t *)&_start_time, localtime_buf);

		/* Normalize to the beginning of the week */
        	start_tm->tm_mday -= start_tm->tm_wday;
        	start_tm->tm_sec = start_tm->tm_min = start_tm->tm_hour = 0;
        	start_tm->tm_isdst = -1;
        	_start_time = mktime(start_tm);
        	start_tm = _XLocaltime((const time_t *)&_start_time, localtime_buf);

		/* Move forward to the proper week day */
		start_tm->tm_mday += 
			RE_WEEKLY(re)->wd_daytime[wd_ndaytime - 1].dt_day;

		dt_ntime = RE_WEEKLY(re)->wd_daytime[wd_ndaytime - 1].dt_ntime;

		/* Set the proper time */
		if (dt_ntime) {
			start_tm->tm_hour = RE_WEEKLY(re)->
			      wd_daytime[wd_ndaytime - 1].dt_time[dt_ntime - 1]
			      / 100;
			start_tm->tm_min = RE_WEEKLY(re)->
			      wd_daytime[wd_ndaytime - 1].dt_time[dt_ntime - 1]
			      % 100;
		} else {
			start_tm->tm_hour = start_hour;
			start_tm->tm_min = start_min;
		}
	} 

	start_tm->tm_isdst = -1;
	last_time2 = mktime(start_tm);

	return ((last_time1 < last_time2) ? last_time1 : last_time2);
}

static Tick
DoMonthDay(
	const Tick		 start_time,
	const RepeatEvent	*re)
{
	struct tm                start_tm,
				*cur_tm;
	unsigned int		 md_nitems = RE_MONTHLY(re)->md_nitems,
				*md_days = RE_MONTHLY(re)->md_days,
				 interval = 1;
	Tick			 cur_time,
				 last_time1 = EOT,
				 last_time2 = EOT;
	_Xltimeparams		 localtime_buf;

	if (re->re_end_date) {
		last_time1 = LastTickFromEndDate(start_time, re);

		if (re->re_duration == RE_NOTSET)
			return last_time1;
	}

	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	cur_tm = _XLocaltime((const time_t *)&start_time, localtime_buf);
	start_tm.tm_isdst = -1;

	/* The 28th - 31st may not exist in a given month thus if only these
	 * days are specified in a rule it is necessary to calculate the
	 * correct month by brute force versus as a mathimatical calculation.
	 */
	if (md_days[0] > 28) {
		cur_tm->tm_mday = 1;

		/* Compute last event by brute force */
		do {
			cur_tm->tm_mon += re->re_interval;
			cur_tm->tm_isdst = -1;
			cur_time = mktime(cur_tm);
			cur_tm = _XLocaltime((const time_t *)&cur_time, localtime_buf);

	    		if (DayExists(
				DayOfMonth(md_days[0], cur_tm->tm_mon,
					   cur_tm->tm_year),
				cur_tm->tm_mon, cur_tm->tm_year))
				interval++;
	    	} while (interval < re->re_duration);

		start_tm.tm_mon = cur_tm->tm_mon;
		start_tm.tm_year = cur_tm->tm_year;
		start_tm.tm_mday = LastDayExists(cur_tm, md_nitems, md_days); 

	} else if (md_nitems) {
		start_tm.tm_mon += (re->re_duration - 1) * re->re_interval;
		start_tm.tm_mday = 1;
		/* Have the year and month updated */
		cur_time = mktime(&start_tm);
		start_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);
		/* Get the right day (LASTDAY converted to a real day) */
		start_tm.tm_isdst = -1;
		start_tm.tm_mday = DayOfMonth(
				md_days[md_nitems - 1],
				start_tm.tm_mon, start_tm.tm_year);
	} else 
		start_tm.tm_mon += (re->re_duration - 1) * re->re_interval;

	last_time2 = mktime(&start_tm);
	return ((last_time1 < last_time2) ? last_time1 : last_time2);
}

static Tick
DoMonthPos(
	const Tick		 _start_time,
	RepeatEvent		*re)
{
	struct tm                start_tm,
				 cur_tm;
	Tick			 last_time1 = EOT,
				 last_time2 = EOT,
				 start_time = _start_time;
	unsigned int		 nwdt_list = RE_MONTHLY(re)->md_nitems,
				 num_intervals = 1,
				 i, j,
				 brute_force = TRUE;
	WeekDayTime		*wdt_list = RE_MONTHLY(re)->md_weektime;
	RepeatEventState        *res;
	_Xltimeparams		 localtime_buf;

	if (re->re_end_date) {
		last_time1 = LastTickFromEndDate(start_time, re);

		if (re->re_duration == RE_NOTSET)
			return last_time1;
	}

	/* Find the real start time */
	start_time = ClosestTick(start_time, start_time, re, &res);
	free(res);
	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);

        for (i = 0; i < nwdt_list; i++) {
                for (j = 0; j < wdt_list[i].wdt_nweek; j++) {
                        if ((wdt_list[i].wdt_week[j] != WK_F5) &&
                            (wdt_list[i].wdt_week[j] != WK_L5)) {
                                brute_force = FALSE;
                                break;
                        }
                }
                if (brute_force == FALSE) break;
        }

	if (brute_force) {
		cur_tm = start_tm;
                cur_tm.tm_mday = 1;

	    	while (num_intervals < re->re_duration) {
                        cur_tm.tm_mon += re->re_interval;
                        cur_tm.tm_isdst = -1;
                        last_time2 = mktime(&cur_tm);
                        cur_tm = *_XLocaltime((const time_t *)&last_time2, 
					    localtime_buf);

                        if (OccurenceExists(wdt_list, nwdt_list, last_time2))
                                num_intervals++;

                        if (!InTimeRange(num_intervals, re->re_duration))
                                break;
                }
	} else {
		start_tm.tm_mon += (re->re_duration - 1) * re->re_interval;
		start_tm.tm_isdst = -1;
                start_tm.tm_mday = 1;
		last_time2 = mktime(&start_tm);
	}

	/*
	 * Given the occurence information, find the last valid day in the
	 * month.
	 */
	last_time2 = LastOccurence(last_time2, wdt_list, nwdt_list);

	return ((last_time1 < last_time2) ? last_time1 : last_time2);
}

static Tick
DoYearByMonth(
	const Tick		 start_time,
	RepeatEvent		*re)
{
	struct tm               *start_tm;
	int			 start_day; 
	Tick			 _start_time,
				 last_time1 = EOT,
				 last_time2 = EOT;
	RepeatEventState        *res;
	_Xltimeparams		 localtime_buf;

	if (re->re_end_date) {
		last_time1 = LastTickFromEndDate(start_time, re);

		if (re->re_duration == RE_NOTSET)
			return last_time1;
	}

	/* Find the real start time */
	_start_time = ClosestTick(start_time, start_time, re, &res);
	free(res);
	start_tm = _XLocaltime((const time_t *)&_start_time, localtime_buf);
	start_day = start_tm->tm_mday;

	/* Go to the last day an event can happen on. */
	start_tm->tm_year += (re->re_duration - 1) * re->re_interval;
	start_tm->tm_isdst = -1;

	/* XXX: If the only month used is Feb and the date is the 29th, then
	 * we must use a special case.
	 */

	/* Go to the last time an event can happen on on the last month. */
	if (RE_YEARLY(re)->yd_nitems) {
		int 	i;

		_start_time = mktime(start_tm);
		start_tm = _XLocaltime((const time_t *)&_start_time, localtime_buf);

		for (i = RE_YEARLY(re)->yd_nitems - 1; i >= 0; i++) {
			if (DayExists(start_day, RE_YEARLY(re)->yd_items[i],
							start_tm->tm_year)) {
				start_tm->tm_mon = RE_YEARLY(re)->yd_items[i]-1;
				start_tm->tm_isdst = -1;
				return (mktime(start_tm));
			}
		}
		/* No months have a day that can be used */
		return ((Tick)NULL); 
	}

	last_time2 = mktime(start_tm);
	return ((last_time1 < last_time2) ? last_time1 : last_time2);
}

static Tick
DoYearByDay(
	const Tick		 start_time,
	RepeatEvent		*re)
{
	struct tm               *start_tm;
	int			 start_day; 
	Tick			 _start_time,
				 last_time1 = EOT,
				 last_time2 = EOT;
	RepeatEventState        *res;
	_Xltimeparams		 localtime_buf;

	if (re->re_end_date) {
		last_time1 = LastTickFromEndDate(start_time, re);

		if (re->re_duration == RE_NOTSET)
			return last_time1;
	}

	/* Find the real start time */
	_start_time = ClosestTick(start_time, start_time, re, &res);
	free(res);
	start_tm = _XLocaltime((const time_t *)&_start_time, localtime_buf);

	/* Go to the last year an event can happen on. */
	start_tm->tm_year += (re->re_duration - 1) * re->re_interval;
	start_tm->tm_isdst = -1;

	/* Go to the last time an event can happen on. */
	if (RE_YEARLY(re)->yd_nitems) {
		start_tm->tm_mon = 0; 
		start_tm->tm_mday =
			RE_YEARLY(re)->yd_items[RE_YEARLY(re)->yd_nitems - 1];
	}

	last_time2 = mktime(start_tm);
	return ((last_time1 < last_time2) ? last_time1 : last_time2);
}

/*
 * Given a month/year (from cur_tm), and a list of days of the month
 * determine the last day in that list that is valid in that month.
 */
static int
LastDayExists(
	const struct tm		*cur_tm,
	const unsigned int	 md_nitems,
	const unsigned int	*md_days)
{
	int	i;
	int	day;

	for (i = md_nitems - 1; i >= 0; i--) {  
		day = DayOfMonth(md_days[i], cur_tm->tm_mon, cur_tm->tm_year);
	    	if (DayExists(day, cur_tm->tm_mon, cur_tm->tm_year))
			return(day);
	}

	return 0;
}

/*
 * Given a month/year (in cur_time) determine the last occurence of week/day/
 * time in the month.
 */
static Tick
LastOccurence(
	const Tick		 cur_time,
	const WeekDayTime	*wdt_list,
	const unsigned int	 nwdt_list)
{
        int	i, j, k;
	Tick	oldest_time = 0,
		current_time;
 
        for (i = 0; i < nwdt_list; i++) {
                for (j = 0; j < wdt_list[i].wdt_nweek; j++) {
                        for (k = 0; k < wdt_list[i].wdt_nday; k++) {
                                if (current_time = WeekNumberToDay(cur_time,
                                                wdt_list[i].wdt_week[j],
                                                wdt_list[i].wdt_day[k])) {
					if (current_time > oldest_time)
						oldest_time = current_time;
					
				}
			}
		}
	}

	return oldest_time;
}

/*
 * Given a time and a rule find the last tick before the end date.
 */
static Tick
LastTickFromEndDate(
	const Tick		 cur_time,
	const RepeatEvent	*re)
{
	RepeatEventState        *res;
	RepeatEvent		*_re = (RepeatEvent *)re;
	Tick                     end_date = re->re_end_date,
				 last_time;
	Duration                 duration = re->re_duration;
 
	/* Take the end date out of the equation. */
	_re->re_end_date = 0;
	_re->re_duration = RE_INFINITY;
 
	/* Use the end date to get the closest tick after it, then
	 * step back one tick to get the last tick before the
	 * end date.
	 */
	last_time = ClosestTick(end_date, cur_time, _re, &res);
	/*
	 * An event that occurs at the same time as the end_date is an
	 * event.
	 */
	if (last_time != end_date)
		last_time = PrevTick(last_time, cur_time, _re, res);
 
	/* Return the re to its original state. */
	_re->re_end_date = end_date;
	_re->re_duration = duration;
	free (res);

	return last_time;
}
