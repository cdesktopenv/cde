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
/* $XConsortium: renexttick.c /main/5 1996/11/21 19:46:22 drk $ */
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
	SameInterval = 0,
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

static Tick NextDayTick(const Tick, const Tick, const RepeatEvent *,
			RepeatEventState *);
static Tick NextWeekTick(const struct tm *, const struct tm *,
			 const RepeatEvent *, const DayTime *,
			 const MoveIndicator, RepeatEventState *);
static Tick NextMonthTick(const struct tm *, const RepeatEvent *,
			  const WeekDayTime *, const MoveIndicator,
			  RepeatEventState *);
extern void FillInRepeatEvent(const Tick, RepeatEvent *);

Tick
NextTick(
        const Tick               cur_time,
	const Tick		 start_time,
        RepeatEvent       	*re,
        RepeatEventState        *res)
{
	Tick 			 next_time;

	if (!re) return (Tick)NULL;

	FillInRepeatEvent(start_time, re);

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
	Tick			 next_time;
	Duration		 res_duration = res->res_duration;
	unsigned int		 res_time = RES_DSTATE(res).res_time;

	if (RE_DAILY(re)->dd_ntime) {
		/* Last event was on the last time for this day.  Now must
		 * move to the next day/interval.
		 */
		if (RES_DSTATE(res).res_time == RE_DAILY(re)->dd_ntime - 1) {
			next_time = NextDayTick(cur_time, start_time, re, res);
			RES_DSTATE(res).res_time = 0;
		} else {
			struct tm		*tm;
			_Xltimeparams		localtime_buf;

			/* There is a later valid time on this day, use it */
                        tm = _XLocaltime(&cur_time, localtime_buf);
                        tm->tm_hour = 
                                RE_DAILY(re)->
				      dd_time[++RES_DSTATE(res).res_time] / 100;
                        tm->tm_min = 
                                RE_DAILY(re)->dd_time[RES_DSTATE(res).res_time]
                                % 100;
                        tm->tm_isdst = -1;
                        next_time = mktime(tm);
		}
	} else {
		/* No alternate times for this day/move to the next interval. */
		next_time = NextDayTick(cur_time, start_time, re, res);
	}

	/* If we went beyond the end date then restore the state info, ret 0 */
	if (re->re_end_date && re->re_end_date < next_time) {
		res->res_duration = res_duration;
		RES_DSTATE(res).res_time = res_time;
		return 0;
	}

	return next_time;
}

static Tick
DoWeek(
	const Tick		 cur_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	unsigned int		 res_daytime = RES_WSTATE(res).res_daytime,
				 res_time = RES_WSTATE(res).res_time,
				 re_ntime,
				 re_ndaytime = RE_WEEKLY(re)->wd_ndaytime;
	Duration		 res_duration = res->res_duration;
	struct tm		 cur_tm,
				 start_tm;
	DayTime			*daytime = RE_WEEKLY(re)->wd_daytime;
	Tick			 next_time;
	_Xltimeparams		 localtime_buf;

	cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);
	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);

	re_ntime = daytime[0].dt_ntime;

	/*
	 * We want to stop when the duration == 1 and we have handled all
	 * the days in the rule for that week.
	 */
	if ((res->res_duration == 0) || ((res->res_duration == 1) && 
	    ((re_ndaytime - 1) == RES_WSTATE(res).res_daytime)))
		return (Tick)0;
		
	/* If these are equal then we are at the end of the time
	 * slots for this day */
	if ((re_ntime - 1) == res_time) {
		/* If these are equal then we used up all the
		 * days for this week */
		if ((re_ndaytime - 1) == res_daytime) {

			/* End of week */
			RES_WSTATE(res).res_daytime = 0;
			RES_WSTATE(res).res_time = 0;
			next_time = NextWeekTick(&cur_tm, &start_tm, re,
						 daytime, NextInterval, res);
		} else {
			/* Move to the next day event (same week), use
			 * the earliest time.
			 */
			RES_WSTATE(res).res_time = 0;
			RES_WSTATE(res).res_daytime++;
			next_time = NextWeekTick(&cur_tm, &start_tm, re,
						daytime, SameInterval, res);
		}
	} else {
		/* Move to the next time, same day. */
		RES_WSTATE(res).res_time++;
		next_time = NextWeekTick(&cur_tm, &start_tm, re, daytime,
						 SameInterval, res);
	}

	/* If we went beyond the end date then restore the state info, ret 0 */
	if (re->re_end_date && re->re_end_date < next_time) {
		res->res_duration = res_duration;
		RES_WSTATE(res).res_time = res_time;
		RES_WSTATE(res).res_daytime = res_daytime;
		return 0;
	}

	return (next_time);
}

static Tick
DoMonthDay(
	const Tick		 cur_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	unsigned int		*md_days = RE_MONTHLY(re)->md_days,
				 md_nitems = RE_MONTHLY(re)->md_nitems,
				 res_day = RES_MSTATE(res).res_day;
	Duration		 res_duration = res->res_duration;
	Tick			 _cur_time;
	struct tm		 start_tm,
				 cur_tm;
	int			 next_interval = FALSE;
	_Xltimeparams		 localtime_buf;

	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);

	/*
	 * We want to stop when the duration == 1 and we have handled all
	 * the days listed in the rule for that month.
	 */
	if ((res->res_duration == 0) || ((res->res_duration == 1) && 
	    ((md_nitems - 1) == RES_MSTATE(res).res_day)))
		return (Tick)0;

	/* Check each event day in the month, move to the next interval when
	 * we run out of event days for the month.  Make sure the event day
	 * exists in that month.  e.g. the 31st of June does not exist.
	 */
	do {
		if ((md_nitems - 1) == RES_MSTATE(res).res_day) {
			cur_tm.tm_mon += 1 * re->re_interval;
			RES_MSTATE(res).res_day = 0;
			next_interval = TRUE;
		} else
			RES_MSTATE(res).res_day++;

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

	if (next_interval) res->res_duration--;

	/* If we went beyond the end date then restore the state info, ret 0 */
	if (re->re_end_date && re->re_end_date < _cur_time) {
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
	struct tm		 start_tm,
				 cur_tm;
	Tick			 _cur_time;
	Duration		 res_duration = res->res_duration;
	unsigned int		 md_nitems = RE_MONTHLY(re)->md_nitems,
				 wdt_nday = wdt_list[md_nitems-1].wdt_nday,
				 wdt_nweek = wdt_list[md_nitems-1].wdt_nweek,
				 res_weektime = RES_MSTATE(res).res_weektime,
				 res_wday = RES_MSTATE(res).res_wday,
				 res_wtime = RES_MSTATE(res).res_wtime,
				 res_wweek = RES_MSTATE(res).res_wweek;
	_Xltimeparams		 localtime_buf;

	if (res->res_duration == 0)
		return (Tick)0;
		
	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);

	/* XXX: This assumes rules of this form only: MP<n> 1+ WE #4 */
	if ((wdt_list[res_weektime].wdt_nday - 1) == res_wday) {
		if ((wdt_list[res_weektime].wdt_nweek - 1) == res_wweek) {
			if ((md_nitems - 1) == res_weektime) {
				RES_MSTATE(res).res_weektime = 0;
				RES_MSTATE(res).res_wday = 0;
				RES_MSTATE(res).res_wtime = 0;
				RES_MSTATE(res).res_wweek = 0;
				_cur_time = NextMonthTick(&cur_tm, re, wdt_list,
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

	/* If we went beyond the end date then restore the state info, ret 0 */
	if (re->re_end_date && re->re_end_date < _cur_time) {
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
	struct tm		 start_tm,
				 cur_tm;
	Tick			 _cur_time;
	Duration		 res_duration = res->res_duration;
	unsigned int		*month_list = RE_YEARLY(re)->yd_items,
				 nitems = RE_YEARLY(re)->yd_nitems,
				 res_month = RES_YSTATE(res).res_daymonth;
	_Xltimeparams		 localtime_buf;

	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);

	/*
	 * We want to stop when the duration == 1 and we have handled all
	 * the months listed in the rule for that year.
	 */
	if ((res->res_duration == 0) || ((res->res_duration == 1) && 
	    ((nitems - 1) == RES_YSTATE(res).res_daymonth)))
		return (Tick)0;

	cur_tm.tm_mday = start_tm.tm_mday;
	cur_tm.tm_hour = start_tm.tm_hour;
	cur_tm.tm_min = start_tm.tm_min;
	cur_tm.tm_sec = start_tm.tm_sec;

	/* If these equal then we used up all the months for this year.  
	 * We must now move to the next interval.
	 */
	if ((nitems - 1) == res_month) {
		cur_tm.tm_year += re->re_interval;
		cur_tm.tm_mon = month_list[0] - 1; /* 0 = January */
		RES_YSTATE(res).res_daymonth = 0;
		res->res_duration--;
	} else {
		/* Take the next month in the month_list, same year */
		cur_tm.tm_mon = month_list[++RES_YSTATE(res).res_daymonth] - 1;
	}

	cur_tm.tm_isdst = -1;
	_cur_time = mktime(&cur_tm);

	/* If we went beyond the end date then restore the state info, ret 0 */
	if (re->re_end_date && re->re_end_date < _cur_time) {
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
	struct tm		 start_tm,
				 cur_tm;
	Tick			 _cur_time;
	Duration		 res_duration = res->res_duration;
	unsigned int		*day_list = RE_YEARLY(re)->yd_items,
				 nitems = RE_YEARLY(re)->yd_nitems,
				 res_month = RES_YSTATE(res).res_daymonth;
	_Xltimeparams		 localtime_buf;

	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);

	/*
	 * We want to stop when the duration == 1 and we have handled all
	 * the days listed in the rule for that year.
	 */
	if ((res->res_duration == 0) || ((res->res_duration == 1) && 
	    ((nitems - 1) == RES_YSTATE(res).res_daymonth)))
		return (Tick)0;

	cur_tm.tm_mday = start_tm.tm_mday;
	cur_tm.tm_hour = start_tm.tm_hour;
	cur_tm.tm_min = start_tm.tm_min;
	cur_tm.tm_sec = start_tm.tm_sec;
	cur_tm.tm_mon = 0;
	cur_tm.tm_isdst = -1;

	/* If these equal then we used up all the days for this year.  
	 * We must now move to the next interval.
	 */
	if ((nitems - 1) == res_month) {
		cur_tm.tm_year += re->re_interval;
		cur_tm.tm_mday = day_list[0];
		RES_YSTATE(res).res_daymonth = 0;
		res->res_duration--;
	} else {
		/* Take the next day in the day_list, same year */
		cur_tm.tm_mday = day_list[++RES_YSTATE(res).res_daymonth];
	}

	_cur_time = mktime(&cur_tm);

	/* If we went beyond the end date then restore the state info, ret 0 */
	if (re->re_end_date && re->re_end_date < _cur_time) {
		res->res_duration = res_duration;
		RES_YSTATE(res).res_daymonth = res_month;
		return 0;
	}

	return (_cur_time);
}

static Tick
NextDayTick(
	const Tick		 cur_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	struct tm	*tm;
	struct tm	 start_tm;
	_Xltimeparams	 localtime_buf;

	if (res->res_duration == 0) return (Tick)0;

	res->res_duration--;

	start_tm = *_XLocaltime(&start_time, localtime_buf);
	tm = _XLocaltime(&cur_time, localtime_buf);

	tm->tm_mday += re->re_interval;
	tm->tm_hour = start_tm.tm_hour;
	tm->tm_min = start_tm.tm_min;
	tm->tm_isdst = -1;

	return (mktime(tm));
}

static Tick
NextWeekTick(
	const struct tm		*current_tm,
	const struct tm		*start_tm,
	const RepeatEvent 	*re,
	const DayTime		*wd_daytime,
	const MoveIndicator	 move,
	RepeatEventState 	*res)
{
	struct tm		*cur_tm = (struct tm *)current_tm;
	unsigned int             res_daytime = RES_WSTATE(res).res_daytime;
	unsigned int             res_time = RES_WSTATE(res).res_time;

	/* Move forward to the next interval (at least another week) */
	if (move == (const MoveIndicator) NextInterval) {
		/* Normalize the date to the beginning of the week. */
		cur_tm->tm_mday -= cur_tm->tm_wday;
		cur_tm->tm_sec = cur_tm->tm_min = cur_tm->tm_hour = 0;
        	cur_tm->tm_isdst = -1;

		/* Add an interval */
		cur_tm->tm_mday += re->re_interval * 7;

		/* Put it on the correct day. */
		cur_tm->tm_mday += wd_daytime[0].dt_day;
		/* Put it on the correct time. */
		cur_tm->tm_hour = wd_daytime[0].dt_time[0] / 100;
		cur_tm->tm_min = wd_daytime[0].dt_time[0] % 100;

		res->res_duration--;

		return (mktime(cur_tm));
	}

	/* SameInterval */

	/* Move the appropriate number of days forward */
	cur_tm->tm_mday += GetWDayDiff(cur_tm->tm_wday,
				       wd_daytime[res_daytime].dt_day);
				
	/* Use the indicated time if available */
	if (RE_WEEKLY(re)->wd_daytime[res_daytime].dt_time) {
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
NextMonthTick(
	const struct tm		*current_time,
	const RepeatEvent	*re,
	const WeekDayTime	*wdt_list,
	const MoveIndicator	 move,
	RepeatEventState 	*res)
{
	struct tm		*cur_tm = (struct tm *)current_time;
	unsigned int             res_weektime = RES_MSTATE(res).res_weektime,
				 res_wweek = RES_MSTATE(res).res_wweek,
				 res_wday = RES_MSTATE(res).res_wday;
	Tick			 next_time;

	/* Move forward to the next interval (at least another month) */
	if (move == (const MoveIndicator) NextInterval) {
		cur_tm->tm_mday = 1;

		do {
			/* Add an interval */
			cur_tm->tm_mon += re->re_interval;
			cur_tm->tm_isdst = -1;

		} while (!(next_time = WeekNumberToDay(mktime(cur_tm),
		  		wdt_list[res_weektime].wdt_week[res_wweek],
		  		wdt_list[res_weektime].wdt_day[res_wday])));
		res->res_duration--;
	} else {
		/* SameInterval */
		next_time = 0;
	}

	return (next_time);
}
