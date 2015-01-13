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
/* $XConsortium: reclotick.c /main/6 1996/11/21 19:45:29 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>

#include <stdlib.h>
#include "rerule.h"
#include "repeat.h"
#include "reutil.h"

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
static void DoDSTAdjustment(const Tick, struct tm *);
static RepeatEventState	*InitRepeatEventState(const RepeatEvent *);
static Tick DSTAdjustment(const struct tm *, const struct tm *);
static int GetMonthDiff(const struct tm *, const struct tm *);
static int MonthDayNumIntervals(struct tm *, struct tm *, 
				const RepeatEvent *, const unsigned int *,
				struct tm *);
static int MonthPosNumIntervals(struct tm *, struct tm *,
				const RepeatEvent *, const WeekDayTime *,
				const unsigned int, struct tm *);
void FillInRepeatEvent(const Tick, RepeatEvent *);

/*
 * Return the closest time following or equal to the target time given a 
 * recurrence rule.
 */
Tick
ClosestTick(
        const Tick		 _target_time,
        const Tick		 start_time,
        RepeatEvent		*re,
        RepeatEventState        **res)
{
	Tick 			 closest_tick,
				 real_start_time,
				 target_time = _target_time;

	if (!re) return (Tick)NULL;

	FillInRepeatEvent(start_time, re);

	if (!(*res = InitRepeatEventState(re)))
		return (Tick)NULL;

	if (target_time < start_time)
		target_time = start_time;

	switch (re->re_type) {
	case RT_MINUTE:
		closest_tick = DoMinute(target_time, start_time, re, *res);
		break;
	case RT_DAILY:
		closest_tick = DoDay(target_time, start_time, re, *res);
		break;
	case RT_WEEKLY:
		closest_tick = DoWeek(target_time, start_time, re, *res);
		break;
	case RT_MONTHLY_POSITION:
		/* Establish the real start time */
		real_start_time = DoMonthPos(start_time, start_time, re, *res);
		if (target_time < real_start_time) 
			target_time = real_start_time;
		if (target_time == real_start_time) {
			(*res)->res_duration = re->re_duration - 1;
			closest_tick = real_start_time;
		} else
			closest_tick = DoMonthPos(target_time, 
						real_start_time, re, *res);
		break;
	case RT_MONTHLY_DAY:
		closest_tick = DoMonthDay(target_time, start_time, re, *res);
		break;
	case RT_YEARLY_MONTH:
		closest_tick = DoYearByMonth(target_time, start_time, re, *res);
		break;
	case RT_YEARLY_DAY:
		closest_tick = DoYearByDay(target_time, start_time, re, *res);
		break;
	}

	/*
	 * Make sure the closest time is not past the appt's end time.
	 */
	if ((!closest_tick) ||
	    (re->re_end_date && re->re_end_date < closest_tick)) { 
		free (*res);
		*res = NULL;
		return (Tick)NULL;
	}

	/*
	 * If the duration was not set (thus strictly using the end date)
	 * reset the RepeatEventState duration back to not-set.  This is
	 * cleaner than having conditionals through out the code checking
	 * to see if the duration needs to be updated.
	 */
	if (re->re_duration == RE_NOTSET)
		(*res)->res_duration == RE_NOTSET;

	return closest_tick;
}

/*
 * Example M60 #4
 */
static Tick
DoMinute(
	const Tick		 target_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	int			 delta_seconds;
	int			 num_intervals;
	Tick			 closest_tick;

	delta_seconds = target_time - start_time;

		/* The number of intervals required to span the time from the
		 * start_time to the target_time given the interval size.
		 * The interval size comes from the rule (e.g. M5 or 5 * 60)
		 */
	num_intervals = delta_seconds / (re->re_interval * 60) + 1;

	if (num_intervals > re->re_duration) { 
		/* The Minute portion of rule does not allow us to reach
		 * the target_time because of the duration limit.
		 */
		closest_tick = re->re_duration * re->re_interval * 60;
		/* Pop the stack */
	} else if (num_intervals < re->re_duration) {
		/* In this case we reached the target_time without using
		 * up all of the intervals allotted to us by the duration.
		 */
		closest_tick = num_intervals * re->re_interval * 60;
		/* res->res_duration -= (num_intervals + 1); */
	} else {
		closest_tick = num_intervals * re->re_interval * 60;
		/* res->res_duration -= (num_intervals + 1); */
	}

	return closest_tick;
}

/*
 * Example: D2 #4
 */
static Tick
DoDay(
	const Tick		 target_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	int			 delta_seconds,
				 num_intervals,
				 dst_adj,
				 daysec = 60 * 60 * 24,
	 			 i;
	struct tm		 target_tm,
				 start_tm,
				 base_tm;
	unsigned int		 ntime = RE_DAILY(re)->dd_ntime;
	Tick			 base_time,
				 next_time = 0;
	Time			*time_list = RE_DAILY(re)->dd_time;
	_Xltimeparams		 localtime_buf;

	target_tm = *_XLocaltime((const time_t *)&target_time, localtime_buf);
	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	dst_adj = DSTAdjustment(&start_tm, &target_tm);

	/* Normalize time to 00:00 */
	start_tm.tm_sec = 0;
	start_tm.tm_min = 0;
	start_tm.tm_hour = 0;
	start_tm.tm_isdst = -1;
	base_time = mktime(&start_tm);

	delta_seconds = target_time - base_time + dst_adj;

		/* The number of intervals required to span the time
		 * from the start_time to the target_time given the
		 * interval size.  The interval size comes from the
		 * rule (e.g. D5 or 5 * daysec)
	 	 */
	num_intervals = delta_seconds / (re->re_interval * daysec);

		/* This brings us to the interval closest to the target
	 	 * time, although it may not actually be the same day.
	 	 */
	base_time += num_intervals * (re->re_interval * daysec) - dst_adj;

	if (!InTimeRange(num_intervals, re->re_duration)) {
		/* We hit the duration limit. */
		next_time = 0;
		goto done;
	}

	base_tm = *_XLocaltime(&base_time, localtime_buf);

		/* If we are not on the same day we need to move
		 * forward one interval and take the earliest time.
		 * XXX: This won't work with composite rules.
	 	 */
	if (!SAME_DAY(&base_tm, &target_tm)) { 
		/* Add one interval to the base_time. */
		base_time += 1 * (re->re_interval * daysec);
		num_intervals++;

		/* By moving ahead one day we might have crossed a dst border.*/
		DoDSTAdjustment(base_time, &base_tm);

		if (!InTimeRange(num_intervals, re->re_duration)) {
			/* We hit the duration limit. */
			next_time = 0;
		 	goto done;
		}
	}
		/* Take into account any specific times that are a part
		 * of this daily repeating rule: e.g. D2 0100 1000 1400 #3.
		 * We walk through the times for this appointment looking for
		 * one later than the target time.  
		 */
	for (i = 0; i < ntime; i++) {
			/* Add the hour that is to be tested to the normalized
			 * time and see if it is later than the target time.
			 */
		base_tm.tm_min = time_list[i]%100;
		base_tm.tm_hour = time_list[i]/100;
		base_tm.tm_isdst = -1;
		next_time = mktime(&base_tm);
		if (next_time >= target_time) {
			res->res_duration = re->re_duration - 
							(num_intervals + 1);
			RES_DSTATE(res).res_time = i;
			goto done;
		}
	}

		/* The target time falls after the latest time on
		 * this appt day.  We must move forward one interval 
		 * and take the earliest time.
		 * XXX: Composite rules issue.
		 */
	base_tm = *_XLocaltime(&base_time, localtime_buf);
		/* Add one interval to the base_time. */
	base_time += 1 * (re->re_interval * daysec);
	num_intervals++;

	if (!InTimeRange(num_intervals, re->re_duration)) {
		/* We hit the duration limit. */
		next_time = 0;
	 	goto done;
	}

	/* By moving ahead one day we might have crossed a dst border.*/
	DoDSTAdjustment(base_time, &base_tm);

		/* Add the hour that is to be tested to the normalized
		 * time and see if it is later than the target time.
		 */
	base_tm.tm_min = time_list[0]%100;
	base_tm.tm_hour = time_list[0]/100;
	base_tm.tm_isdst = -1;
	next_time = mktime(&base_tm);

	res->res_duration = re->re_duration - (num_intervals + 1);
	RES_DSTATE(res).res_time = 0;

done:
	return (next_time);
}

/*
 * Example: W2 MO WE FR #4
 */
static Tick
DoWeek(
	const Tick		 _target_time,
	const Tick		 _start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	int			 delta_seconds,
				 num_intervals,
				 dst_adj,
				 appt_time,
				 daysec = 60 * 60 * 24,
				 wksec = daysec * 7;
	unsigned int		 ntime = RE_WEEKLY(re)->wd_ndaytime;
	struct tm		 target_tm,
				 start_tm,
				 base_tm;
	Tick			 target_time = _target_time,
				 start_time = _start_time,
				 base_time,
				 begin_time,
				 adj_start_time,
				 next_time = 0;
	DayTime			*day_list = RE_WEEKLY(re)->wd_daytime;
	RepeatEventState	*unused;
	_Xltimeparams		 localtime_buf;

	/* Make sure the start time is on the first real event slot. */
	if (_target_time) {
		if (!(unused = InitRepeatEventState(re)))
			return (Tick)NULL;
		start_time = DoWeek(0, _start_time, re, unused);
		free(unused);
		if (_target_time < start_time)
			target_time = start_time;
	} else
		target_time = _start_time;

	target_tm = *_XLocaltime((const time_t *)&target_time, localtime_buf);
	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	appt_time = start_tm.tm_hour * 100 + start_tm.tm_min;
	dst_adj = DSTAdjustment(&start_tm, &target_tm);

	/* Normalize start time to the beginning of the week. */
	start_tm.tm_mday -= start_tm.tm_wday;
	start_tm.tm_sec = start_tm.tm_min = start_tm.tm_hour = 0;
	start_tm.tm_isdst = -1;
	begin_time = mktime(&start_tm);
	start_tm = *_XLocaltime((const time_t *)&begin_time, localtime_buf);

	delta_seconds = target_time - begin_time + dst_adj;

		/* The number of intervals required to span the time
		 * from the start_time to the target_time given the
		 * interval size.  The interval size comes from the
		 * rule (e.g. W5 or 5 * wksec)
	 	 */
	num_intervals = delta_seconds / (re->re_interval * wksec);

		/* This brings us to the interval closest to the target
	 	 * time, although it may not actually be the right week.
	 	 */
	base_time = begin_time + num_intervals * (re->re_interval * wksec);
	base_tm = *_XLocaltime(&base_time, localtime_buf);
	dst_adj = DSTAdjustment(&start_tm, &base_tm);

	if (!InTimeRange(num_intervals, re->re_duration)) {
		/* We hit the duration limit. */
		 goto done;
	}

	if (dst_adj) {
		base_time -= dst_adj;
		base_tm = *_XLocaltime(&base_time, localtime_buf);
	}

	if (same_week(&target_tm, &base_tm)) {
		int i;
		int event_wday = -1;
		/* Take the next event */
		for (i = 0; i < ntime; i++) {
			if (day_list[i].dt_day > target_tm.tm_wday) {
				event_wday = day_list[i].dt_day;
				break;
			} else if (day_list[i].dt_day == target_tm.tm_wday) {
				/* If they are the same day, the day_list time
				 * must be later than the target time.
			 	 */
				int day_time = (day_list[i].dt_time) ? 
						day_list[i].dt_time[0]:
						appt_time;
				/* XXX: Must walk the time list too. */
				if (TIME_OF_DAY(&target_tm) <= 
						HOURTOSEC(day_time)) {
					event_wday = day_list[i].dt_day;
					break;
				}
			}
		}

		RES_WSTATE(res).res_daytime = i;
		RES_WSTATE(res).res_time = 0;

		/* The target date is on the same week, but falls after the
		 * last weekday the event could happen on.
		 */
		if (event_wday == -1) {
			/* XXX: Lose the goto. */
			goto nextinterval;
		}
		base_tm.tm_mday += GetWDayDiff(base_tm.tm_wday, event_wday);
	} else {
nextinterval:
		/* We will need to go one more interval */
		if (!InTimeRange(++num_intervals, re->re_duration)) {
			/* We hit the duration limit. */
			next_time = 0;
		 	goto done;
		}
		/* Since the base_tm has been normalized to the beginning
		 * of the week, we can assume we are on Sunday.
		 */
		base_tm.tm_mday += re->re_interval * 7;
		/* If the target day is smaller than the base day then we
		 * can take the first day in the next event week.
		 */
		if (base_tm.tm_mday > target_tm.tm_mday) {
			base_tm.tm_mday += day_list[0].dt_day;
		}
		RES_WSTATE(res).res_daytime = 0;
		RES_WSTATE(res).res_time = 0;
	}

	base_tm.tm_hour = appt_time / 100;
	base_tm.tm_min = appt_time % 100;
	base_tm.tm_isdst = -1;
	res->res_duration = re->re_duration - num_intervals;
	next_time = mktime(&base_tm);

done:
	return (next_time);
}

/*
 * Example: MD2 1 10 20 30 #10
 */
static Tick
DoMonthDay(
	const Tick		 target_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	int			 num_intervals,
				 event_day,
				 nmonths;
	unsigned int		 ndays = RE_MONTHLY(re)->md_nitems;
	struct tm		 target_tm,
				 start_tm,
				 base_tm;
	Tick			 base_time,
				 next_time = 0;
	unsigned int		*day_list = RE_MONTHLY(re)->md_days;
	_Xltimeparams		 localtime_buf;

	target_tm = *_XLocaltime((const time_t *)&target_time, localtime_buf);
	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	event_day = day_list[0];
	num_intervals = MonthDayNumIntervals(&start_tm, &target_tm, re,
					     day_list, &base_tm);

	if (!InTimeRange(num_intervals, re->re_duration)) {
		/* We hit the duration limit. */
		goto done;
	}

	if (SAME_MONTH(&base_tm, &target_tm)) {
		int	next_interval = TRUE,
			i;

		for (i = 0; i < ndays; i++) {
			unsigned int	day;

			day = DayOfMonth(day_list[i], base_tm.tm_mon,
						     base_tm.tm_year);

			if (day < target_tm.tm_mday)
				continue;
			if (day == target_tm.tm_mday)
				/* If it is on the same day, the event time
				 * must be later than the target time.
				 */
				if (TIME_OF_DAY(&target_tm)
						> TIME_OF_DAY(&start_tm))
					continue;
				else {
					event_day = day;
					next_interval = FALSE;
					break;
				}
			if (day > target_tm.tm_mday) {
					event_day = day;
					next_interval = FALSE;
					break;
			}
		}
		/* We are on the right month and we found a time after the
		 * target time.
		 */
		if (!next_interval) {
			base_tm.tm_mday = event_day;
			base_tm.tm_isdst = -1;
			next_time = mktime(&base_tm);
			/* If the day exists (e.g. 31st in July) we're done */
			if (DayExists(event_day, base_tm.tm_mon,
						 base_tm.tm_year)) {
				/* Update repeat state info */
				res->res_duration = re->re_duration 
								- num_intervals;
				RES_MSTATE(res).res_day = i;
				next_time = mktime(&base_tm);
				goto done;
			}
		}
	}

	num_intervals++;

	if (!InTimeRange(num_intervals, re->re_duration)) {
		/* We hit the duration limit. */
		next_time = 0;
	 	goto done;
	}

	/* Since we are moving to the next interval, use the first day */
	event_day = day_list[0];
	do {
		/* Event is in the next interval */
		base_tm.tm_mon += 1 * re->re_interval;
		base_tm.tm_mday = 1;
		base_tm.tm_isdst = -1;
		base_time = mktime(&base_tm);
		base_tm = *_XLocaltime(&base_time, localtime_buf);

		/* Stop when the day exists in that month */
	} while (!DayExists(event_day, base_tm.tm_mon, base_tm.tm_year));

	base_tm.tm_mday = DayOfMonth(event_day, base_tm.tm_mon,
			             base_tm.tm_year);
	base_tm.tm_isdst = -1;
	next_time = mktime(&base_tm);

	res->res_duration = re->re_duration - num_intervals;
	RES_MSTATE(res).res_day = 0;

done:
	return (next_time);
		
}

/*
 * Example: MP2 1+ MO TU 2- TH #3
 */
static Tick
DoMonthPos(
	const Tick		 target_time,
	const Tick		 start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	int			 num_intervals,
				 nmonths;
	unsigned int		 ndays = RE_MONTHLY(re)->md_nitems;
	struct tm		 target_tm,
				 start_tm,
				 base_tm;
	Tick			 base_time = 0;
	WeekDayTime		*wdt_list = RE_MONTHLY(re)->md_weektime;
	_Xltimeparams		 localtime_buf;

	target_tm = *_XLocaltime((const time_t *)&target_time, localtime_buf);
	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	num_intervals = MonthPosNumIntervals(&start_tm, &target_tm, re,
					     wdt_list, ndays, &base_tm);

	do {
		if (!InTimeRange(num_intervals, re->re_duration)) {
			/* We hit the duration limit. */
			base_time = 0;
		 	goto done;
		}
		base_tm.tm_isdst = -1;

		if (SAME_MONTH(&target_tm, &base_tm)) {
			base_time = mktime(&base_tm);
			base_tm = *_XLocaltime((const time_t *)&base_time, localtime_buf);
			base_time = WeekNumberToDay(base_time,
					       wdt_list[0].wdt_week[0],
					       wdt_list[0].wdt_day[0]);
			if (base_time >= target_time)
				break;
			/* target_time came after the slot for this month */
			if (base_time)
				num_intervals++;
		}

		base_tm.tm_mon += re->re_interval;
		base_tm.tm_isdst = -1;
		/* Move to the first interval after the target time */
		base_time = mktime(&base_tm);
		base_tm = *_XLocaltime((const time_t *)&base_time, localtime_buf);
		base_time = WeekNumberToDay(base_time,
					       wdt_list[0].wdt_week[0],
					       wdt_list[0].wdt_day[0]);
	} while (!base_time); 

	num_intervals++;

	/* Update repeat state info */
	res->res_duration = re->re_duration - num_intervals;
	RES_MSTATE(res).res_weektime = 0;
	RES_MSTATE(res).res_wday = 0;
	RES_MSTATE(res).res_wtime = 0;
	RES_MSTATE(res).res_wweek = 0;

done:
	return (base_time);
}

/*
 * Example: YM1 2 5 9 #4
 */
static Tick
DoYearByMonth(
	const Tick		 _target_time,
	const Tick		 _start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	int			 num_intervals,
				 nyears;
	unsigned int		 nitems = RE_YEARLY(re)->yd_nitems;
	struct tm		 target_tm,
				 start_tm,
				 base_tm;
	Tick			 base_time = 0,
				 start_time = _start_time,
				 target_time = _target_time;
	unsigned int		*month_list = RE_YEARLY(re)->yd_items;
	RepeatEventState	*unused;
	_Xltimeparams		 localtime_buf;

	/* Make sure the start time is on the first real event slot. */
	if (_target_time) {
		if (!(unused = InitRepeatEventState(re)))
			return (Tick)NULL;
		start_time = DoYearByMonth(0, _start_time, re, unused);
		free(unused);
		if (_target_time < start_time)
			target_time = start_time;
	} else
		target_time = _start_time;

	target_tm = *_XLocaltime((const time_t *)&target_time, localtime_buf);
	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	nyears = target_tm.tm_year - start_tm.tm_year;
	num_intervals = nyears / re->re_interval;

	if (!InTimeRange(num_intervals, re->re_duration)) {
		/* We hit the duration limit. */
	 	goto done;
	}

	base_tm = start_tm;
	base_tm.tm_year += num_intervals * re->re_interval;
	base_tm.tm_isdst = -1;

	base_time = mktime(&base_tm);
	base_tm = *_XLocaltime((const time_t *)&base_time, localtime_buf);

	if (base_tm.tm_year == target_tm.tm_year) {
		int i;

		/* Look for a month that is >= the target month */
		for (i = 0; i < nitems; i++) {
			/* If the months are equal the target time has to be
			 * less than the next tick.
			 */
			if (month_list[i] - 1 == target_tm.tm_mon) {
				base_tm.tm_mon = month_list[i] - 1;
				base_tm.tm_isdst = -1;
				base_time = mktime(&base_tm);
				base_tm = *_XLocaltime(&base_time, localtime_buf);
				if (TIMEOFMONTH(&base_tm) >=
						       TIMEOFMONTH(&target_tm)){
					res->res_duration = re->re_duration - 
								num_intervals;
					RES_YSTATE(res).res_daymonth = i;
					goto done;
				}
			} else if (month_list[i] - 1 >= target_tm.tm_mon) {
				base_tm.tm_mon = month_list[i] - 1;
				base_tm.tm_isdst = -1;
				base_time = mktime(&base_tm);
				res->res_duration = re->re_duration - 
								num_intervals;
				RES_YSTATE(res).res_daymonth = i;
				goto done;
			}
		}
	}

	/*
	 * The base year is greater than the target year, take the first
	 * month.
	 */
	if (!InTimeRange(++num_intervals, re->re_duration)) {
		/* We hit the duration limit. */
		base_time = 0;
	 	goto done;
	}

	base_tm.tm_year += re->re_interval;
	base_tm.tm_mon = month_list[0] - 1;
	base_tm.tm_isdst = -1;
	base_time = mktime(&base_tm);

	res->res_duration = re->re_duration - num_intervals;
	RES_YSTATE(res).res_daymonth = 0;
done:
	return (base_time);
}

/*
 * Example: YD1 100 200 300 #4
 */
static Tick
DoYearByDay(
	const Tick		 _target_time,
	const Tick		 _start_time,
	const RepeatEvent	*re,
	RepeatEventState	*res)
{
	int			 num_intervals,
				 nyears;
	unsigned int		 nitems = RE_YEARLY(re)->yd_nitems;
	struct tm		 target_tm,
				 start_tm,
				 base_tm;
	Tick			 base_time = 0,
				 target_time = _target_time,
				 start_time = _start_time;
	unsigned int		*day_list = RE_YEARLY(re)->yd_items;
	RepeatEventState	*unused;
	_Xltimeparams		 localtime_buf;

	/* Make sure the start time is on the first real event slot. */
	if (_target_time) {
		if (!(unused = InitRepeatEventState(re)))
			return (Tick)NULL;
		start_time = DoYearByDay(0, _start_time, re, unused);
		free(unused);
		if (_target_time < start_time)
			target_time = start_time;
	} else
		target_time = _start_time;

	target_tm = *_XLocaltime((const time_t *)&target_time, localtime_buf);
	start_tm = *_XLocaltime((const time_t *)&start_time, localtime_buf);
	nyears = target_tm.tm_year - start_tm.tm_year;
	num_intervals = nyears / re->re_interval;

	if (!InTimeRange(num_intervals, re->re_duration)) {
		/* We hit the duration limit. */
	 	goto done;
	}

	/* 
	 * XXX: day_list == 366 is a special case...that is not supported
	 * right now.
	 */

	base_tm = start_tm;
	base_tm.tm_year += num_intervals * re->re_interval;

	/* If the years are the same then go down the list of days looking
	 * for one later than the target time.
	 */
	if (base_tm.tm_year == target_tm.tm_year) {
		int i;
		for (i = 0; i < nitems; i++) {
			base_tm.tm_mday = day_list[i];
			base_tm.tm_mon = 0;
			base_tm.tm_isdst = -1;
			base_time = mktime(&base_tm);

			/* We found the closest tick */
			if (base_time >= target_time) {
				res->res_duration = re->re_duration - 
								num_intervals;
				RES_YSTATE(res).res_daymonth = i;
				goto done;
			}
		}
	}

	/* 
	 * We either were not on the same year or the above fell through
	 * as we crossed into the next interval.
	 */

	num_intervals++;

	if (!InTimeRange(num_intervals, re->re_duration)) {
		/* We hit the duration limit. */
		base_time = 0;
	 	goto done;
	}

	base_tm.tm_year += 1 * re->re_interval;
	base_tm.tm_mday = day_list[0];
	base_tm.tm_mon = 0;
	base_tm.tm_isdst = -1;
	base_time = mktime(&base_tm);

	res->res_duration = re->re_duration - num_intervals;
	RES_YSTATE(res).res_daymonth = 0;

done:
	return (base_time);
}

/* Calculate the number of months between two dates */
/* 3/20/90 - 1/2/94 = 46 months */ 
static int
GetMonthDiff(
	const struct tm	*start_tm,
	const struct tm	*end_tm)
{
	return ((end_tm->tm_year - start_tm->tm_year + 1) * 12 -
		(start_tm->tm_mon + 1) - (12 - (end_tm->tm_mon + 1)));
}

static Tick
DSTAdjustment(
	const struct tm	*tm1,
	const struct tm	*tm2)
{
	if (tm1->tm_isdst == -1 || tm2->tm_isdst == -1)
		return 0;

	if (tm1->tm_isdst != tm2->tm_isdst) {
		if (tm1->tm_isdst) 	/* From day light savings to standard */
			return -3600;
		else 			/* From standard to day light savings */
			return 3600;
	}
	return 0;
}

static void 
DoDSTAdjustment(
	const Tick	 begin_time,
	struct tm	*end_time)	/* Return */
{
	struct tm	 next_day;
	Tick		 dst_adj,
			 _begin_time = begin_time;
	_Xltimeparams	 localtime_buf;

	/* By moving ahead one day we might have crossed a dst border.*/
	next_day = *_XLocaltime(&begin_time, localtime_buf);
	dst_adj = DSTAdjustment(end_time, &next_day);
	if (dst_adj) {
		_begin_time -= dst_adj;
		*end_time = *_XLocaltime(&_begin_time, localtime_buf);
	} else
		*end_time = next_day;
}

/*
 * Initialize the RepeatEventState struct.
 */
static RepeatEventState	*
InitRepeatEventState(
	const RepeatEvent	*re)
{
	RepeatEventState	*res;

	if (!(res = (RepeatEventState *)calloc(1, sizeof(RepeatEventState))))
		return (RepeatEventState *)NULL;

	res->res_re = re;
	res->res_duration = re->re_duration;

	return res;
}


/*
 * Determine the number of intervals between the start_tm and the target_tm,
 * the base_tm which is returned is the last event generated before the
 * target_tm.
 */
static int
MonthDayNumIntervals(
	struct tm		*start_tm,
	struct tm		*target_tm,
	const RepeatEvent	*re,
	const unsigned int	*md_days,
	struct tm		*base_tm)	/* Return */ 
{
	int		 num_intervals = 0;	
	struct tm	 cur_tm;
	Tick		 cur_time,
			 base_time,
			 last_time,
			 target_time;
	_Xltimeparams	 localtime_buf;

        /* The 28th - 31st may not exist in a given month thus if only these
         * days are specified in a rule it is necessary to calculate the
         * correct month by brute force versus using a mathematical calculation.
         */
        if (md_days[0] > 28) {	
		*base_tm = *start_tm;
		cur_tm = *start_tm;
		cur_tm.tm_mday = 1;
		cur_tm.tm_isdst = -1;
		cur_time = mktime(&cur_tm);
		target_tm->tm_isdst = -1;
		target_time = mktime((struct tm*)target_tm);
		last_time = cur_time;

                while (cur_time < target_time) {
                        cur_tm.tm_mon += re->re_interval;
                        cur_tm.tm_isdst = -1;
                        cur_time = mktime(&cur_tm);
                        cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);

                        if (DayExists(md_days[0], cur_tm.tm_mon,
                                                        cur_tm.tm_year)) {
				if (cur_time >= target_time) {
					cur_time = last_time;
                        		cur_tm = *_XLocaltime((const time_t *)
								    &last_time, localtime_buf);
					break;
				}
				/* Remember the last time in case we need to
				 * back up one interval.
				 */
				last_time = cur_time;
				*base_tm = cur_tm;
                                num_intervals++;
			}

			if (!InTimeRange(num_intervals, re->re_duration))
				break;

			if (SAME_MONTH(target_tm, &cur_tm)) break;
                }
	} else {
		num_intervals = GetMonthDiff(start_tm, target_tm)
							/ re->re_interval;
		*base_tm = *start_tm;
		base_tm->tm_isdst = -1;
		/* Move to the closest interval before the target time */
		base_tm->tm_mon += num_intervals * re->re_interval;
		base_time = mktime(base_tm);
		*base_tm = *_XLocaltime(&base_time, localtime_buf);
	}

	return (num_intervals);
}

/*
 * Count the number of intervals up to, but before the target time.  The
 * base time returned in the last valid interval before the target time.
 */
static int
MonthPosNumIntervals(
	struct tm		*start_tm,
	struct tm		*target_tm,
	const RepeatEvent	*re,
	const WeekDayTime	*wdt_list,
	const unsigned int	 nwdt_list,
	struct tm		*base_tm)	/* Return */
{
	int		 num_intervals = 0,
			 brute_force = TRUE,
			 i, j;	
	struct tm	 cur_tm;
	Tick		 cur_time,
			 base_time,
			 target_time;
	_Xltimeparams	 localtime_buf;

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

        /* The weekday associated with +5 or -5 may not exist in a given
	 * month thus if only these weekdays are specified in a rule it is
	 * necessary to calculate the correct month by brute force versus
	 * using a mathematical calculation.
         */
        if (brute_force){
		*base_tm = *start_tm;
		cur_tm = *start_tm;
		cur_tm.tm_isdst = -1;
		cur_tm.tm_mday = 1;
		cur_time = mktime(&cur_tm);
		target_tm->tm_isdst = -1;
		target_time = mktime((struct tm *)target_tm);

		/* Count the start_time */
		if (cur_time < target_time)
			num_intervals++;

                while (cur_time < target_time) {
			if (SAME_MONTH(target_tm, &cur_tm)) break;

                        cur_tm.tm_mon += re->re_interval;
                        cur_tm.tm_isdst = -1;
                        cur_time = mktime(&cur_tm);
                        cur_tm = *_XLocaltime((const time_t *)&cur_time, localtime_buf);

                        if (OccurenceExists(wdt_list, nwdt_list, cur_time)) {
                                num_intervals++;
				/* Only update the cur_tm if valid slot there */
				*base_tm = cur_tm;
			}

			if (!InTimeRange(num_intervals, re->re_duration))
				break;

                }
	} else {
		num_intervals = GetMonthDiff(start_tm, target_tm)
							/ re->re_interval;
		*base_tm = *start_tm;
		base_tm->tm_isdst = -1;
		/* Move to the closest interval before the target time */
		base_tm->tm_mon += num_intervals * re->re_interval;
		base_time = mktime(base_tm);
		*base_tm = *_XLocaltime(&base_time, localtime_buf);
	}

	return (num_intervals);
}

void
FillInRepeatEvent(
        const Tick		 start_time,
        RepeatEvent		*re)
{
	struct tm		*start_tm;
	int			 i;
	_Xltimeparams		 localtime_buf;

	start_tm = _XLocaltime(&start_time, localtime_buf);

	switch (re->re_type) {
	case RT_MINUTE:
		break;
	case RT_DAILY:
		if (!RE_DAILY(re)->dd_ntime) {
			RE_DAILY(re)->dd_time = (Time *)calloc(1, sizeof(Time));
			RE_DAILY(re)->dd_time[0] = start_tm->tm_hour * 100 + 
						   start_tm->tm_min;
			RE_DAILY(re)->dd_ntime = 1;
		}
		break;
	case RT_WEEKLY:
		if (!RE_WEEKLY(re)->wd_ndaytime) {
			RE_WEEKLY(re)->wd_daytime = 
					  (DayTime *)calloc(1, sizeof(DayTime));
			RE_WEEKLY(re)->wd_daytime[0].dt_day = start_tm->tm_wday;
			RE_WEEKLY(re)->wd_daytime[0].dt_ntime = 1;
			RE_WEEKLY(re)->wd_daytime[0].dt_time =
					  (Time *)calloc(1, sizeof(Time));
			RE_WEEKLY(re)->wd_daytime[0].dt_time[0] =
						start_tm->tm_hour * 100 +
						start_tm->tm_min;
			RE_WEEKLY(re)->wd_ndaytime = 1;
		} else {
			int i;
			for (i = 0; i < RE_WEEKLY(re)->wd_ndaytime; i++) {
				if (!RE_WEEKLY(re)->wd_daytime[i].dt_ntime) {
					RE_WEEKLY(re)->wd_daytime[i].dt_ntime =
									      1;
					RE_WEEKLY(re)->wd_daytime[i].dt_time =
					  	(Time *)calloc(1, sizeof(Time));
					RE_WEEKLY(re)->wd_daytime[i].dt_time[0]=
						start_tm->tm_hour * 100 +
						start_tm->tm_min;
				}
			}
		}
		break;
	case RT_MONTHLY_POSITION:
		if (!RE_MONTHLY(re)->md_nitems) {
			RE_MONTHLY(re)->md_weektime =
				  (WeekDayTime *)calloc(1, sizeof(WeekDayTime));
			RE_MONTHLY(re)->md_weektime[0].wdt_nday = 1;
			RE_MONTHLY(re)->md_weektime[0].wdt_day =
				  	(WeekDay *)calloc(1, sizeof(WeekDay));
			RE_MONTHLY(re)->md_weektime[0].wdt_day[0] =
							start_tm->tm_wday;
			RE_MONTHLY(re)->md_weektime[0].wdt_nweek = 1;
			RE_MONTHLY(re)->md_weektime[0].wdt_week =
				  (WeekNumber *)calloc(1, sizeof(WeekNumber));
			RE_MONTHLY(re)->md_weektime[0].wdt_week[0] =
						GetWeekNumber(start_time);
			RE_MONTHLY(re)->md_nitems = 1;
		}
		break;
	case RT_MONTHLY_DAY:
		if (!RE_MONTHLY(re)->md_nitems) {
			RE_MONTHLY(re)->md_days = 
				(unsigned int *)calloc(1, sizeof(unsigned int));
			RE_MONTHLY(re)->md_days[0] = start_tm->tm_mday; 
			RE_MONTHLY(re)->md_nitems = 1;
		}
		break;
	case RT_YEARLY_MONTH:
		if (!RE_YEARLY(re)->yd_nitems) {
			RE_YEARLY(re)->yd_items = 
				(unsigned int *)calloc(1, sizeof(unsigned int));
			RE_YEARLY(re)->yd_items[0] = start_tm->tm_mon + 1;
			RE_YEARLY(re)->yd_nitems = 1;
		}
		break;
	case RT_YEARLY_DAY:
		if (!RE_YEARLY(re)->yd_nitems) {
			RE_YEARLY(re)->yd_items = 
				(unsigned int *)calloc(1, sizeof(unsigned int));
			RE_YEARLY(re)->yd_items[0] = start_tm->tm_yday;
			RE_YEARLY(re)->yd_nitems = 1;
		}
		break;
	}
}
