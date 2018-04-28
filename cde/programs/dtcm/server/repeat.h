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
/* $XConsortium: repeat.h /main/4 1995/11/09 12:49:18 rswiston $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#ifndef _REPEAT_H
#define _REPEAT_H

#include <EUSCompat.h>
#include "ansi_c.h"
#include "cm.h"
#include "rtable4.h"
#include "rerule.h"

typedef time_t	Tick;

extern int	monthdays[];

extern void init_time P(());

extern time_t _DtCms_closest_tick_v4 P((time_t target,
					time_t ftick,
					Period_4 period,
					int *ordinal));

extern time_t _DtCms_last_tick_v4 P((time_t ftick,
				     Period_4 period,
				     int ntimes));

extern time_t _DtCms_next_tick_v4 P((time_t tick, Period_4 period));

extern time_t _DtCms_prev_tick_v4 P((time_t tick, Period_4 period));

extern time_t _DtCms_first_tick_v4 P((time_t t, Period_4 period, int ordinal));

extern void _DtCms_adjust_appt_startdate P((Appt_4 *appt));

extern time_t next_nmins P((time_t t, int m));

extern time_t next_ndays P((time_t t, int n));

extern int _DtCms_marked_4_cancellation P((Appt_4 *a, int i));

extern int _DtCms_get_new_ntimes_v4 P((Period_4 period,
				       time_t tick,
				       int ninstance));

extern int _DtCms_get_ninstance_v4 P((Appt_4 *appt));

extern int _DtCms_in_repeater P((Id_4 *key,
				 Appt_4 *p_appt,
				 boolean_t dont_care_cancel));

extern int      monthlength             P((Tick));
extern int      leapyr                  P((int));
extern int      fdom                    P((Tick));
extern int      ldom                    P((Tick));

/*
 * returns the tick of the beginning of the day
 */
extern time_t _DtCmsBeginOfDay(time_t t);

/*
 * return the tick since begin of day
 */
extern time_t _DtCmsTimeOfDay(time_t t);

/*
 * routines to deal with recurrence rule and exception dates
 */
extern boolean_t _DtCmsInExceptionList P((cms_entry *eptr, time_t tick));

#endif
