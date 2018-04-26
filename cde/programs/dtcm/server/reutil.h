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
/* $XConsortium: reutil.h /main/4 1995/11/09 12:51:01 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _REUTIL_H
#define _REUTIL_H

#include "rerule.h"
#include "repeat.h"

#define EOT	2147483647

#define RE_DAILY(re)    (re->re_data.re_daily)
#define RE_WEEKLY(re)   (re->re_data.re_weekly)
#define RE_MONTHLY(re)  (re->re_data.re_monthly)
#define RE_YEARLY(re)   (re->re_data.re_yearly)
#define RES_DSTATE(res) (res->res_data.ds)
#define RES_WSTATE(res) (res->res_data.ws)
#define RES_MSTATE(res) (res->res_data.ms)
#define RES_YSTATE(res) (res->res_data.ys)
#define SAME_DAY(tm1, tm2)  (((tm1)->tm_year == (tm2)->tm_year && \
                             (tm1)->tm_mon == (tm2)->tm_mon && \
                             (tm1)->tm_mday == (tm2)->tm_mday))
#define SAME_MONTH(tm1, tm2) (((tm1)->tm_year == (tm2)->tm_year && \
                              (tm1)->tm_mon == (tm2)->tm_mon))
#define TIME_OF_DAY(tm)   ((tm)->tm_hour * 60 * 60 + \
                         (tm)->tm_min * 60 + \
                         (tm)->tm_sec)
#define TIMEOFMONTH(tm) (((tm)->tm_mday - 1) * 24 * 60 * 60 + \
			 (tm)->tm_hour * 60 * 60 + \
                         (tm)->tm_min * 60 + \
                         (tm)->tm_sec)
#define HOURTOSEC(time) ((time/100) * 60 * 60 + \
                         (time%100) * 60)

extern int GetWDayDiff(const int, const int);
extern int DayExists(const int, const int, const int);
extern WeekNumber GetWeekNumber(const Tick);
extern WeekNumber DayToWeekNumber(const Tick);
extern Tick WeekNumberToDay(const Tick, const WeekNumber, const WeekDay);
extern int OccurenceExists(const WeekDayTime *, const unsigned int, const Tick);
extern unsigned int DayOfMonth(const int, const int, const int);
extern int InTimeRange(const unsigned int, const Duration);
extern int same_week(struct tm *, struct tm *);

extern Tick ClosestTick(const Tick, const Tick, RepeatEvent *,
                        RepeatEventState **);
extern Tick NextTick(const Tick, const Tick, RepeatEvent *,
                        RepeatEventState *);
extern Tick PrevTick(const Tick, const Tick, RepeatEvent *,
                        RepeatEventState *);
extern Tick LastTick(const Tick, RepeatEvent *);
extern int CountEvents(Tick, RepeatEvent *re, CSA_date_time_entry *);
extern Tick DeriveNewStartTime(const Tick, RepeatEvent *old_re, const Tick, 
			const Tick, RepeatEvent *new_re);


#endif /* _REUTIL_H */
