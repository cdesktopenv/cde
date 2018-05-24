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
/* $XConsortium: reprevtick.c /main/6 1996/11/21 19:46:55 drk $ */
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

#include "rerule.h"
#include "repeat.h"
#include "reutil.h"

typedef enum {
        SameInterval,
        NextInterval
} MoveIndicator;

static Tick DoMinute(const Tick, const Tick, const RepeatEvent *,
			RepeatEventState *);
static Tick DoDay(const Tick, const Tick, const RepeatEvent *,
			RepeatEventState *);
static Tick DoWeek(const Tick, const Tick, const RepeatEvent *,
			RepeatEventState *);
static Tick DoMonthDay(const Tick, const Tick, const RepeatEvent *,
			RepeatEventState *);
static Tick DoMonthPos(const Tick, const Tick, const RepeatEvent *,
			RepeatEventState *);
static Tick DoYearByMonth(const Tick, const Tick, const RepeatEvent *,
			RepeatEventState *);
static Tick DoYearByDay(const Tick, const Tick, const RepeatEvent *,
			RepeatEventState *);

static Tick PrevDayTick(const Tick, const int, const int, const RepeatEvent *,
			RepeatEventState *);
static Tick PrevWeekTick(const struct tm *, const struct tm *,
			 const RepeatEvent *, const DayTime *,
			 const MoveIndicator, RepeatEventState *);
static Tick PrevMonthTick(const struct tm *, const RepeatEvent *, 
			  const WeekDayTime *, const MoveIndicator,
			  RepeatEventState *);
extern void FillInRepeatEvent(const Tick, RepeatEvent *);

static int
DurationCheck(
        const Duration    res_dur,
        const Duration    re_dur)
{
	if (re_dur == RE_INFINITY || re_dur == RE_NOTSET) return TRUE;

	if (res_dur > re_dur)
		return FALSE;

	return TRUE;
}

Tick
PrevTick(
        const Tick		cur_time,
	const Tick		start_time,
        RepeatEvent		*re,
        RepeatEventState        *res)
{
	Tick 			 next_time;
	Tick			 _start_time;

        if (!re) return (Tick)NULL;

	if (!start_time)
        	FillInRepeatEvent(cur_time, re);
	else
        	FillInRepeatEvent(start_time, re);
 
        if (!DurationCheck(res->res_duration, re->re_duration)) return (Tick)0;

	switch (re->re_type) {
	case RT_MINUTE:
		next_time = DoMinute(cur_time, start_time, re, res);
		break;
	case RT_DAILY:
		next_time = DoDay(cur_time, start_time, re, res);
		break;
	case RT_WEEKLY:
		next_time = DoWeek(cur_time, start_time, re, res);
		break;
	case RT_MONTHLY_POSITION:
		next_time = DoMonthPos(cur_time, start_time, re, res);
		break;
	case RT_MONTHLY_DAY:
		next_time = DoMonthDay(cur_time, start_time, re, res);
		break;
	case RT_YEARLY_MONTH:
		next_time = DoYearByMonth(cur_time, start_time, re, res);
		break;
	case RT_YEARLY_DAY:
		next_time = DoYearByDay(cur_time, start_time, re, res);
		break;
	}

        /* 
         * If the duration was not set (thus strictly using the end date) 
         * reset the RepeatEventState duration back to not-set.  This is 
         * cleaner than having conditionals through out the code checking 
         * to see if the duration needs to be updated. 
         */ 
        if (re->re_duration == RE_NOTSET)
                res->res_duration = RE_NOTSET;

	return next_time;
}


/*
 * If DoMinute is called, RT_MINUTE must be on top of res stack.
 */
static Tick
DoMinute(
	const Tick		 cur_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	return (Tick)NULL;
}

static Tick
DoDay(
	const Tick		 cur_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	Tick			 prev_time;
	Duration                 res_duration = res->res_duration;
	unsigned int		 dd_ntime = RE_DAILY(re)->dd_ntime,
				 res_time = RES_DSTATE(res).res_time;

	if (dd_ntime) {
		/* Last event was on the first time for this day.  Now must
		 * move to the previous day/interval - latest hour.
		 */
		if (RES_DSTATE(res).res_time == 0) {
		 	prev_time = PrevDayTick(cur_time,
				RE_DAILY(re)->dd_time[dd_ntime - 1]/100,
				RE_DAILY(re)->dd_time[dd_ntime - 1]%100,
				re, res);
			RES_DSTATE(res).res_time = dd_ntime - 1;
		} else {
			struct tm               *tm;
			_Xltimeparams		localtime_buf;

			/* There is an earlier valid time on this day, use it */
                        tm = _XLocaltime(&cur_time, localtime_buf);
                        tm->tm_hour = 
                                RE_DAILY(re)->
                                      dd_time[--RES_DSTATE(res).res_time] / 100;                        tm->tm_min = 
                                RE_DAILY(re)->dd_time[RES_DSTATE(res).res_time]
                                % 100;
                        tm->tm_isdst = -1;
                        prev_time = mktime(tm);
                }
	} else {
		struct tm	*start_tm;
		_Xltimeparams	localtime_buf;

		/* No alternate times for this day/move to the previous
		 * interval.
		 */
		if (start_time)
        		start_tm = _XLocaltime(&start_time, localtime_buf);
		else
        	 	start_tm = _XLocaltime(&cur_time, localtime_buf);
		 prev_time = PrevDayTick(cur_time, start_tm->tm_hour,
					 start_tm->tm_min, re, res);
	}

	/* We can not move before the start time. */
	if (prev_time < start_time) {
		res->res_duration = res_duration;
		RES_DSTATE(res).res_time = res_time;
		return 0;
	}

	return prev_time;
}

static Tick
DoWeek(
	const Tick		 cur_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
        unsigned int             res_daytime = RES_WSTATE(res).res_daytime,
        			 res_time = RES_WSTATE(res).res_time,
        			 re_ntime,
        			 re_ndaytime = RE_WEEKLY(re)->wd_ndaytime;
	Duration                 res_duration = res->res_duration;
        struct tm                cur_tm,
                                 start_tm;
        DayTime                 *daytime = RE_WEEKLY(re)->wd_daytime;
        Tick                     prev_time;
	_Xltimeparams		 localtime_buf;
 
        cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);
	if (start_time)
	  start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	else
	  start_tm = cur_tm;

	re_ntime = daytime[0].dt_ntime;
 
        /* If this is 0 then we are at the first time for this day. */
        if (!res_time) {
                /* If this is 0 then we are at the first day for this week. */
                if (!res_daytime) {

                        /* Beginning of week - move to an earlier week */
                        RES_WSTATE(res).res_daytime = re_ndaytime - 1;
                        RES_WSTATE(res).res_time = re_ntime - 1;
                        prev_time = PrevWeekTick(&cur_tm, &start_tm, re,
                                                 daytime, NextInterval, res);
                } else {
                        /* Move to the prev day event (same week), use
                         * the latest time.
                         */
                        RES_WSTATE(res).res_time = re_ntime - 1;
                        RES_WSTATE(res).res_daytime--;
                        prev_time = PrevWeekTick(&cur_tm, &start_tm, re,
                                                 daytime, SameInterval, res);
                }
        } else {
                /* Move to an earlier time, same day. */
                RES_WSTATE(res).res_time--;
                prev_time = PrevWeekTick(&cur_tm, &start_tm, re, daytime,
						SameInterval, res);
        }

	/* We can not move before the start time. */
	if (prev_time < start_time) {
		res->res_duration = res_duration;
		RES_WSTATE(res).res_time = res_time;
		RES_WSTATE(res).res_daytime = res_daytime;
		return 0;
	}

        return (prev_time);
}

static Tick
DoMonthDay(
	const Tick		 cur_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
        unsigned int            *md_days = RE_MONTHLY(re)->md_days,
        			 md_nitems = RE_MONTHLY(re)->md_nitems,
				 res_day = RES_MSTATE(res).res_day;
	Duration                 res_duration = res->res_duration;
        struct tm                cur_tm;
	Tick			 _cur_time;
	int			 next_interval = FALSE;
	_Xltimeparams		 localtime_buf;
 
        cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);
 
        /* Check each event day in the month, move to the previous interval
	 * when we run out of event days for the month.  Make sure the event
	 * day * exists in that month.  e.g. the 31st of June does not exist.
         */
        do {
                if (!RES_MSTATE(res).res_day) {
                        cur_tm.tm_mon -= 1 * re->re_interval;
                        RES_MSTATE(res).res_day = md_nitems - 1;
			next_interval = TRUE;
                } else
                        RES_MSTATE(res).res_day--;
 
                cur_tm.tm_isdst = -1;
                cur_tm.tm_mday = 1;
                _cur_time = mktime(&cur_tm);
                cur_tm = *_XLocaltime((const time_t *)&_cur_time, localtime_buf);
        } while (!DayExists(md_days[RES_MSTATE(res).res_day], cur_tm.tm_mon,
                            cur_tm.tm_year));
 
        cur_tm.tm_mday = DayOfMonth(md_days[RES_MSTATE(res).res_day],
				    cur_tm.tm_mon, cur_tm.tm_year);
        cur_tm.tm_isdst = -1;
        _cur_time = mktime(&cur_tm);
 
        if (next_interval) res->res_duration++;

	/* We can not move before the start time. */
	if (_cur_time < start_time) {
		res->res_duration = res_duration;
		RES_MSTATE(res).res_day = res_day;
		return 0;
	}

        return (_cur_time);
}

static Tick
DoMonthPos(
	const Tick		 cur_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
        WeekDayTime             *wdt_list = RE_MONTHLY(re)->md_weektime;
        struct tm                cur_tm;
	Duration                 res_duration = res->res_duration;
	Tick			 _cur_time;
        unsigned int             md_nitems = RE_MONTHLY(re)->md_nitems,
                                 res_weektime = RES_MSTATE(res).res_weektime,
                                 res_wday = RES_MSTATE(res).res_wday,
                                 res_wtime = RES_MSTATE(res).res_wtime,
                                 res_wweek = RES_MSTATE(res).res_wweek;
	_Xltimeparams		 localtime_buf;
 
        cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);

        /* XXX: This assumes rules of this form only: MP<n> 1+ WE #4 */
        if (!res_wday) {
                if (!res_wweek) {
                        if (!res_weektime) {
                                RES_MSTATE(res).res_weektime = md_nitems - 1;
                                RES_MSTATE(res).res_wday =
					wdt_list[md_nitems - 1].wdt_nday - 1;
                                RES_MSTATE(res).res_wtime =
					wdt_list[md_nitems - 1].wdt_ntime - 1;
                                RES_MSTATE(res).res_wweek =
					wdt_list[md_nitems - 1].wdt_nweek - 1;
                                _cur_time = PrevMonthTick(&cur_tm, re, wdt_list,
                                                         NextInterval, res);
                        } else {
                                _cur_time = 0;
                        }
                } else {
                        _cur_time = 0;
                }
        } else {
                _cur_time = 0;
        }

	/* We can not move before the start time. */
	if (_cur_time < start_time) {
		res->res_duration = res_duration;
		RES_MSTATE(res).res_weektime = res_weektime;
                RES_MSTATE(res).res_wday = res_wday;
                RES_MSTATE(res).res_wtime = res_wtime;
                RES_MSTATE(res).res_wweek = res_wweek;
		return 0;
	}

        return (_cur_time);
}

static Tick
DoYearByMonth(
	const Tick		 cur_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
        struct tm                start_tm,
                                 cur_tm;
        Duration                 res_duration = res->res_duration;
	Tick			 _cur_time;
        unsigned int            *month_list = RE_YEARLY(re)->yd_items,
                                 nitems = RE_YEARLY(re)->yd_nitems,
                                 res_month = RES_YSTATE(res).res_daymonth;
	_Xltimeparams		 localtime_buf;

        cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);
	if (start_time) {
	    start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	    cur_tm.tm_mday = start_tm.tm_mday;
	    cur_tm.tm_hour = start_tm.tm_hour;
	    cur_tm.tm_min = start_tm.tm_min;
	    cur_tm.tm_sec = start_tm.tm_sec;
	} else
	    start_tm = cur_tm;

        /* If this equals 0 then we are at the first event slot of the year,
  	 * we must move backward one interval/year.
         */
        if (!res_month) {
                cur_tm.tm_year -= re->re_interval;
                cur_tm.tm_mon = month_list[nitems - 1] - 1; /* 0 = January */
                RES_YSTATE(res).res_daymonth = nitems - 1;
                res->res_duration++;
        } else {
                /* Take the next month in the month_list, same year */
                cur_tm.tm_mon = month_list[--RES_YSTATE(res).res_daymonth] - 1;
        }

	cur_tm.tm_isdst = -1;
	_cur_time = mktime(&cur_tm);

	/* We can not move before the start time. */
	if (_cur_time < start_time) {
		res->res_duration = res_duration;
		RES_YSTATE(res).res_daymonth = res_month;
		return 0;
	}

        return (_cur_time);
}

static Tick
DoYearByDay(
	const Tick		 cur_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
        struct tm                start_tm,
                                 cur_tm;
        Duration                 res_duration = res->res_duration;
	Tick			 _cur_time;
        unsigned int            *day_list = RE_YEARLY(re)->yd_items,
                                 nitems = RE_YEARLY(re)->yd_nitems,
                                 res_month = RES_YSTATE(res).res_daymonth;
	_Xltimeparams		 localtime_buf;

        cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);
	if (start_time)
	    start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	else
	    start_tm = cur_tm;

        cur_tm.tm_mday = start_tm.tm_mday;
        cur_tm.tm_hour = start_tm.tm_hour;
        cur_tm.tm_min = start_tm.tm_min;
        cur_tm.tm_sec = start_tm.tm_sec;
        cur_tm.tm_mon = 0;
	cur_tm.tm_isdst = -1;

        /* If this equals 0 then we are at the first event slot of the year,
  	 * we must move backward one interval/year.
         */
        if (!res_month) {
                cur_tm.tm_year -= re->re_interval;
                cur_tm.tm_mday = day_list[nitems - 1];
                RES_YSTATE(res).res_daymonth = nitems - 1;
                res->res_duration++;
        } else {
                /* Take the next day in the day_list, same year */
                cur_tm.tm_mday = day_list[--RES_YSTATE(res).res_daymonth];
        }

	_cur_time = mktime(&cur_tm);

	/* We can not move before the start time. */
	if (_cur_time < start_time) {
		res->res_duration = res_duration;
		RES_YSTATE(res).res_daymonth = res_month;
		return 0;
	}

        return (_cur_time);
}

static Tick
PrevDayTick(
        const Tick		 tick,
        const int		 hour,
	const int		 min,
        const RepeatEvent	*re,
        RepeatEventState        *res)
{
        struct tm       *tm;
	_Xltimeparams	localtime_buf;
 
        res->res_duration++;
 
        tm = _XLocaltime(&tick, localtime_buf);
 
        tm->tm_mday -= re->re_interval;
        tm->tm_hour = hour;
        tm->tm_min = min;
 
        return (mktime(tm));
}

static Tick
PrevWeekTick(
	const struct tm		*current_tm,
	const struct tm		*start_tm,
	const RepeatEvent	*re,
	const DayTime		*wd_daytime,
	const MoveIndicator	 move,
	RepeatEventState        *res)
{
	struct tm		*cur_tm = (struct tm *)current_tm;
        unsigned int             res_daytime = RES_WSTATE(res).res_daytime;
        unsigned int             res_time = RES_WSTATE(res).res_time;
 
        /* Move backward to the previous interval (at least another week) */
        if (move == (const MoveIndicator)NextInterval) {
                /* Normalize the date to the beginning of the week. */
                cur_tm->tm_mday -= cur_tm->tm_wday;
                cur_tm->tm_sec = cur_tm->tm_min = cur_tm->tm_hour = 0;
                cur_tm->tm_isdst = -1;
 
                /* Subtract an interval */
                cur_tm->tm_mday -= re->re_interval * 7;
 
                /* Put it on the correct day. */
                cur_tm->tm_mday += wd_daytime[res_daytime].dt_day;

                /* Put it on the correct time. */
        	if (wd_daytime[res_daytime].dt_time) {
                	cur_tm->tm_hour =
				wd_daytime[res_daytime].dt_time[res_time] / 100;
                	cur_tm->tm_min =
				wd_daytime[res_daytime].dt_time[res_time] % 100;
		} else {
                	/* Use the time from the first appt */
                	cur_tm->tm_hour = start_tm->tm_hour; 
                	cur_tm->tm_min = start_tm->tm_min;
        	}
 
                res->res_duration++;
 
                return (mktime(cur_tm));
        }
 
        /* SameInterval */

        /* Move the appropriate number of days forward */
        cur_tm->tm_mday -= GetWDayDiff(
				RE_WEEKLY(re)->wd_daytime[res_daytime].dt_day,
				cur_tm->tm_wday);
                                
        /* Use the indicated time if available */
        if (wd_daytime[res_daytime].dt_time) {
                cur_tm->tm_hour = wd_daytime[res_daytime].dt_time[res_time]
									/ 100;
		cur_tm->tm_min = wd_daytime[res_daytime].dt_time[res_time]
									% 100;
	} else {
                /* Use the time from the first appt */
                cur_tm->tm_hour = start_tm->tm_hour; 
                cur_tm->tm_min = start_tm->tm_min;
        }

        cur_tm->tm_isdst = -1;
        return (mktime(cur_tm));
}

static Tick
PrevMonthTick(
        const struct tm		*current_tm,
        const RepeatEvent	*re,
        const WeekDayTime	*wdt_list,
        const MoveIndicator	 move,
        RepeatEventState	*res)
{
	struct tm 		*cur_tm = (struct tm *)current_tm;
        unsigned int             res_weektime = RES_MSTATE(res).res_weektime,
                                 res_wweek = RES_MSTATE(res).res_wweek,
                                 res_wday = RES_MSTATE(res).res_wday;
        Tick                     next_time;

        /* Move backward to the previous interval (at least another month) */
        if (move == (const MoveIndicator)NextInterval) {
                cur_tm->tm_mday = 1;

                do {
                        /* Add an interval */
                        cur_tm->tm_mon -= re->re_interval;
                	cur_tm->tm_isdst = -1;

                } while (!(next_time = WeekNumberToDay(mktime(cur_tm),
                                wdt_list[res_weektime].wdt_week[res_wweek],
                                wdt_list[res_weektime].wdt_day[res_wday])));
                res->res_duration++;
        } else {
                /* SameInterval */
                next_time = 0;
        }

        return (next_time);
}
