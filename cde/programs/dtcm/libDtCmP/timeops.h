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
/*******************************************************************************
**
**  timeops.h
**
**  static char sccsid[] = "@(#)timeops.h 1.10 94/11/07 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: timeops.h /main/3 1995/11/03 10:39:02 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _TIMEOPS_H
#define _TIMEOPS_H

#include <EUSCompat.h>
#include "ansi_c.h"

typedef enum {
	minsec		= 60,
	fivemins	= 300,
	hrsec		= 3600,
	daysec		= 86400,
	wksec		= 604800,
	yrsec		= 31536000,
	leapyrsec	= 31622400
} Unit;

/* use time_t typedef for Tick, not a hardcoded long */
typedef time_t  Tick;		/* tick = seconds since epoch */

extern Tick	bot;		/* beginning of time */
extern char	*days[];
extern char	*days2[];
extern char	*days3[];
extern char	*days4[];
extern Tick	eot;		/* end of time */
extern char	*hours[];
extern int	monthdays[];
extern char	*months[];
extern char	*months2[];
extern int	monthsecs[];
extern char	*numbers[];

extern int	days_to_seconds		P((int));
extern int	dom			P((Tick));
extern int	dow			P((Tick));
extern int	fdom			P((Tick));
extern Tick	first_dom		P((Tick));
extern Tick	first_dow		P((Tick));
extern Tick	get_eot			P(());
extern Tick	get_bot			P(());
extern long	gmt_off			P(());
extern int	hour			P((Tick));
extern int	hours_to_seconds	P((int));
extern void	init_time		P(());
extern Tick	jan1			P((Tick));
extern Tick	last_dom		P((Tick));
extern Tick	last_dow		P((Tick));
extern Tick	last_ndays		P((Tick t, int));
extern int	lastapptofweek		P((u_int));
extern Tick	lastjan1		P((Tick));
extern int	ldom			P((Tick));
extern Tick	lower_bound		P((int, Tick));
extern Tick	lowerbound		P((Tick));
extern boolean_t magic_time		P((Tick t));
extern Tick	midnight		P(());
extern int	minutes_to_seconds	P((int));
extern int	minute			P((Tick));
extern int	month			P((Tick));
extern Tick	monthdayyear		P((int, int, int));
extern int	monthlength		P((Tick));
extern Tick	next_ndays		P((Tick, int));
extern Tick	next_nhours		P((Tick, int));
extern Tick	next2weeks		P((Tick));
extern Tick	nextday			P((Tick));
extern Tick	nextjan1		P((Tick));
extern Tick	nextmonth		P((Tick t));
extern Tick	next_nmonth		P((Tick t, int n));
extern Tick	nextmonth_exactday	P((Tick t));
extern Tick	nextweek		P((Tick t));
extern Tick	nextyear		P((Tick));
extern Tick	nextnyear		P((Tick t, int n));
extern Tick	now			P(());
extern int	ntimes_this_week	P((u_int, int));
extern int	numwks			P((Tick));
extern Tick	prev_nmonth		P((Tick t, int n));
extern Tick	prevmonth_exactday	P((Tick));
extern Tick	previousmonth		P((Tick t));
extern Tick	prevweek		P((Tick t));
extern Tick	prev2weeks		P((Tick t));
extern Tick	prevnyear		P((Tick t, int n));
extern int	seconds			P((int n, Unit unit));
extern double	seconds_dble		P((double n, Unit unit));
extern int	seconds_to_days		P((int));
extern int	seconds_to_hours	P((int));
extern int	seconds_to_minutes	P((int));
extern int	seconds_to_weeks	P((int));
extern void	set_timezone		P((char*));
extern int	timeok			P((Tick t));
extern Tick	upperbound		P((Tick));
extern int	weeks_to_seconds	P((int));
extern int	wom			P((Tick));
extern boolean_t weekofmonth		P((Tick, int*));
extern Tick	xytoclock		P((int, int, Tick));
extern int	year			P((Tick));
extern int	adjust_dst		P((Tick, Tick));
extern boolean_t adjust_hour		P((int*));
extern int	leapyr			P((int));
extern Tick	prevday			P((Tick));

#endif
