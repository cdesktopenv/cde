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
/* $XConsortium: rerule.h /main/1 1996/04/21 19:24:15 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _RERULE_H
#define _RERULE_H

typedef unsigned long	Time;
typedef unsigned int	Interval;
typedef int		Duration;

#define RE_INFINITY		-1
#define RE_NOTSET		-2
#define RE_ERROR		-3
#define RE_LASTDAY		32

#define RE_STOP_FLAG			0x80000000
#define RE_SET_FLAG(data)		(data |= RE_STOP_FLAG)
#define RE_STOP_IS_SET(data)		(data & RE_STOP_FLAG)
#define RE_MASK_STOP(data)		(data & ~RE_STOP_FLAG)

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

typedef enum {
	WD_SUN		= 0,
	WD_MON,
	WD_TUE,
	WD_WED,
	WD_THU,
	WD_FRI,
	WD_SAT
} WeekDay;

typedef enum {
	WK_F1		= 0,
	WK_F2,
	WK_F3,
	WK_F4,
	WK_F5,
	WK_L1,
	WK_L2,
	WK_L3,
	WK_L4,
	WK_L5
} WeekNumber;

typedef enum {
	RT_MINUTE,
	RT_DAILY,
	RT_WEEKLY,
	RT_MONTHLY_POSITION,
	RT_MONTHLY_DAY,
	RT_YEARLY_MONTH,
	RT_YEARLY_DAY
} RepeatType;

typedef struct _NumberList {
	struct _NumberList	*nl_next;
	unsigned int		 nl_number;
} NumberList;

typedef struct _DayTime {
	WeekDay		 dt_day;
	unsigned int	 dt_ntime;
	Time		*dt_time;
} DayTime;

typedef struct _DayTimeList {
	struct _DayTimeList	*dtl_next;
	DayTime		 	*dtl_daytime;
} DayTimeList;

typedef struct _WeekDayTime {
	unsigned int	 wdt_nday;
	WeekDay		*wdt_day;
	unsigned int	 wdt_ntime;
	Time		*wdt_time;
	unsigned int	 wdt_nweek;
	WeekNumber	*wdt_week;
} WeekDayTime;

typedef struct _WeekDayTimeList {
	struct _WeekDayTimeList	*wdtl_next;
	WeekDayTime		*wdtl_weektime;
} WeekDayTimeList;

typedef struct _DailyData {
	unsigned int	 dd_ntime;
	Time		*dd_time;
} DailyData;

typedef struct _WeeklyData {
	unsigned int	 wd_ndaytime;
	DayTime		*wd_daytime;
} WeeklyData;

typedef struct _MonthlyData {
	unsigned int	 md_nitems;	/* # of md_weektime or md_days */
	WeekDayTime	*md_weektime;   /* Used in MP command */
	unsigned int	*md_days;	/* Array of days for MD command */
} MonthlyData;

typedef struct _YearlyData {
	unsigned int	 yd_nitems;
	unsigned int	*yd_items;	/* Array of days/months event occurs */
} YearlyData;

typedef struct _RepeatEvent {
	struct _RepeatEvent	*re_next;
	struct _RepeatEvent	*re_prev;
	Interval		 re_interval;
	Duration		 re_duration;
	time_t			 re_end_date;
	RepeatType	 	 re_type;
	union {
		DailyData	*re_daily;
		WeeklyData	*re_weekly;
		MonthlyData	*re_monthly;
		YearlyData	*re_yearly;
	} re_data;
} RepeatEvent;

typedef struct _RepeatEventState {
	struct _RepeatEventState	*res_next;
	const RepeatEvent		*res_re;
	Duration			 res_duration; /* # events remaining */
	union {
		struct _DailyState {
			unsigned int	res_time;
		} ds;
		struct _WeeklyState {
			unsigned int	res_daytime;
			unsigned int	res_time;
		} ws;
		struct _MonthlyState {
			unsigned int	res_day;
			unsigned int	res_weektime;
			unsigned int	res_wday;
			unsigned int	res_wtime;
			unsigned int	res_wweek;
		} ms;
		struct _YearlyState {
			unsigned int	res_daymonth;
		} ys;
	} res_data;
} RepeatEventState;

extern void _DtCm_free_re(RepeatEvent *);
extern void _DtCm_free_re_state(RepeatEventState *);

#endif /* _RERULE_H */
