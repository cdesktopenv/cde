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
/*******************************************************************************
**
**  timeops.c
**
**  $XConsortium: timeops.c /main/6 1996/11/21 19:44:23 drk $
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

#ifndef lint
static char sccsid[] = "@(#)timeops.c 1.13 95/05/02 Copyr 1991 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>
#if defined(linux)
#define SVR4
#endif

#include <sys/param.h>
#include <sys/time.h>
#include <csa.h>
#include "util.h"
#include "timeops.h"
#include "getdate.h"

extern int debug;

typedef enum {dstoff, dston, nochange} DSTchange;

Tick bot, eot;

char *months[] = {"",
	   (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL,
	   (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL,
	   (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL
};

char *months2[] = {"",
	   (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL,
	   (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL,
	   (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL
};

int monthdays[12] = {
	31,	28,	31,
	30,	31,	30,
	31,	31,	30,
	31,	30,	31
};

int monthsecs[12] = {
	31*daysec,	28*daysec,	31*daysec,
	30*daysec,	31*daysec,	30*daysec,
	31*daysec,	31*daysec,	30*daysec,
	31*daysec,	30*daysec,	31*daysec
};

char *days[8] = {(char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL};

char *days2[8] = {(char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL};

char *days3[8] = {(char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL};

char *days4[8] = {(char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL};

char *numbers[32] = {"",
        " 1 ", " 2 ", " 3 ", " 4 ", " 5 ", " 6 ", " 7 ",
        " 8 ", " 9 ", "10", "11", "12", "13", "14",
        "15", "16", "17", "18", "19", "20", "21",
        "22", "23", "24", "25", "26", "27", "28",
        "29", "30", "31"
};

char *hours[24] = {
	"12", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11",
	"12", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11"
};

/*
 * Given a weekmask and the first day of the week, calculate
 * the number of times outstanding in the week.
 */
extern int
ntimes_this_week(u_int weekmask, int firstday)
{
	int i, ntimes, weekdaymask = 1 << firstday;

	if (weekmask == 0)
		return 0;

	for (i=firstday, ntimes=0; i < 7; i++, weekdaymask <<= 1) {
		if (weekdaymask & weekmask)
			ntimes++;
	}
	return ntimes;
}

/* Return beginning of time */
extern Tick
get_bot()
{
	return bot;
}

/* Return end of time */
extern Tick
get_eot()
{
	return eot;
}

/* Given a weekmask, find the last appointment in the week */
extern int
lastapptofweek(u_int mask)
{
	int n;

	if (mask == 0)
		return -1;

	for (n = -1; mask != 0; n++, mask = mask >> 1);

	return n;
}

/*      Remove after v3 release */
extern boolean_t
magic_time(Tick t)
{
        boolean_t magic=B_FALSE;
        struct tm *tm;
	_Xltimeparams localtime_buf;

        tm = _XLocaltime(&t, localtime_buf);

        if (tm->tm_hour == 3 && tm->tm_min == 41)
                 magic=B_TRUE;
        return(magic);
}
/* for 12 hour clock, if 24 subtract 12 */
extern boolean_t
adjust_hour(int *hr)
{
	boolean_t am = B_TRUE;

	if (*hr == 0)
		*hr = 12;
	else if (*hr >= 12) {
		if (*hr > 12)
			*hr -= 12;
		am = B_FALSE;
	}
	return am;
}


extern int
timeok( Tick t)
{
	int r =((t >= bot) &&(t <= eot));
	return(r);
}

static DSTchange
dst_changed(Tick old, Tick new)
{	
	struct tm oldtm;
	struct tm newtm;
	_Xltimeparams localtime_buf;

	oldtm	= *_XLocaltime(&old, localtime_buf);
	newtm	= *_XLocaltime(&new, localtime_buf);
	
	if(oldtm.tm_isdst ==  newtm.tm_isdst) return(nochange);
	switch(oldtm.tm_isdst) {
		case 1:
			return(dstoff);
		case 0:
			return(dston);
		default:
			return(nochange);
	}
}
	
extern int
seconds(int n, Unit unit)
{
	return(n *(int)unit);
}

extern double
seconds_dble(double n, Unit unit)
{
	return((double)(n * (int)unit));
}

extern int
year(Tick t)
{
	struct tm *tm;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);
	return(tm->tm_year + 1900);
}

extern int
month(t)
	Tick t;
{
	struct tm *tm;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);
	return(tm->tm_mon+1);
}

extern int
hour(Tick t)
{
	struct tm *tm;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);
	return(tm->tm_hour);
}

extern int
minute(Tick t)
{
	struct tm *tm;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);
	return(tm->tm_min);
}

extern int
leapyr(int y)
{
	return
	 (y % 4 == 0 && y % 100 !=0 || y % 400 == 0);
}

extern int
monthlength(Tick t)
{
	int mon;
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm = *_XLocaltime(&t, localtime_buf);
	mon = tm.tm_mon;
	return(((mon==1) && leapyr(tm.tm_year+1900))? 29 : monthdays[mon]);
}

extern int
monthseconds(Tick t)
{
	int mon;
	struct tm tm;
	_Xltimeparams localtime_buf;
	
	tm = *_XLocaltime(&t, localtime_buf);
	mon = tm.tm_mon;
	return(((mon==1) && leapyr(tm.tm_year+1900))? 29*daysec : monthsecs[mon]);
}

extern int
dom(Tick t)
{
	struct tm *tm;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);
	return(tm->tm_mday);
}

extern int
wom(Tick t)
{
	struct tm *tm;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);
	return((12 + tm->tm_mday - tm->tm_wday)/7);
}

/*
 * If the result falls beyond the system limit, -1 is returned by mktime().
 */
Tick
next_nmonth(Tick t, int n)
{
	struct tm tm;
	int	n12;
	_Xltimeparams localtime_buf;

	n12 = n/12;
	n = n%12;

	tm = *_XLocaltime(&t, localtime_buf);
	tm.tm_hour=0;
	tm.tm_min=0;
	tm.tm_sec=0;
	tm.tm_mday=1;
	if (n12 > 0)
		tm.tm_year += n12;

	if ((tm.tm_mon = tm.tm_mon + n) > 11) {
		tm.tm_mon -= 12;
		tm.tm_year++;
	}

	tm.tm_isdst = -1;
	return(mktime(&tm));
}

extern Tick
nextmonth(Tick t)
{
	return(next_nmonth(t, 1));
}

extern boolean_t
weekofmonth(Tick t, int *wk)
{
	struct tm tm, tm1, tm2;
	Tick	firstday;
	_Xltimeparams localtime_buf;

	tm = *_XLocaltime(&t, localtime_buf);
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	tm.tm_mday = 1;
	firstday = mktime(&tm);
	tm = *_XLocaltime(&firstday, localtime_buf);

	/* need to get it again since data in tm is changed */
	tm1 = *_XLocaltime(&t, localtime_buf);

	*wk = wom(t);
	if (tm1.tm_wday < tm.tm_wday)
		(*wk)--;

	if (*wk < 4)
		return B_FALSE;
	else {
		t += seconds(7, daysec);
		tm2 = *_XLocaltime(&t, localtime_buf);

		return((tm1.tm_mon == tm2.tm_mon) ? B_FALSE : B_TRUE);
	}
}

extern int
dow(Tick t)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm = *_XLocaltime(&t, localtime_buf);
	return(tm.tm_wday);
}

extern int /* find dow(0-6) that 1st dom falls on */
fdom(Tick t)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm		= *_XLocaltime(&t, localtime_buf);
	tm.tm_mday	= 1;
	tm.tm_isdst	= -1;
	t               = mktime(&tm);
	tm		= *_XLocaltime(&t, localtime_buf);
	return(tm.tm_wday);
}

extern int
ldom(Tick t /* find dow(0-6) that last dom falls on */ )
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm		= *_XLocaltime(&t, localtime_buf);
	tm.tm_mday	= monthlength(t);
	tm.tm_isdst	= -1;
	t               = mktime(&tm);
	tm		= *_XLocaltime(&t, localtime_buf);
	return(tm.tm_wday);
}

/* returns tick of last day of month */
extern Tick
last_dom(Tick tick)
{
        return(upperbound(next_ndays(tick, monthlength(tick) - dom(tick))));
}

/* returns tick of first day of month */
extern Tick
first_dom(Tick tick)
{
        return(lowerbound(last_ndays(tick, dom(tick)-1)));
}

/* returns tick of first day of week */
extern Tick
first_dow(Tick tick)
{
	int d;

	if ((d = dow(tick)) == 0)
		d = 6;
	else 
		d--;

        return(lowerbound(last_ndays(tick, d)));
}

/* returns tick of first day of week */
extern Tick
last_dow(Tick tick)
{
	int d;

	if ((d = dow(tick)) == 0)
		d = 6;
	else 
		d--;
	d = 6 - d;

        return(upperbound(next_ndays(tick, d)));
}
/* returns number of weeks in month */
extern int
numwks(Tick tick)
{
        return (wom(last_dom(tick)));
}

extern int
adjust_dst(Tick start, Tick next) 
{
	DSTchange change;
	change = dst_changed(start, next);
	switch(change) {
		case nochange:
		break;
		case dstoff:
			next+=(int) hrsec;
		break;
		case dston:
			next-=(int) hrsec;
		break;
	}
	return(next);
}

extern Tick
next_nhours(Tick t, int n)
{
	Tick next;
	struct tm tm;
	_Xltimeparams localtime_buf;

        tm              = *_XLocaltime(&t, localtime_buf);
        tm.tm_sec       = 0;
        tm.tm_min       = 0;
 
	next            = mktime(&tm);
	next		= next + seconds(n, hrsec);
	next		= adjust_dst(t, next);
	return(next);
}

extern Tick
last_ndays(Tick t, int n)
{
	Tick last;
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm		= *_XLocaltime(&t, localtime_buf);
	tm.tm_sec	= 0;
	tm.tm_min	= 0;
	tm.tm_hour	= 0;

	last            = mktime(&tm);
	last		= last - seconds(n, daysec);
	last		= adjust_dst(t, last);
	return(last);
}

extern Tick
next_ndays(Tick t, int n)
{
	Tick next;
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm		= *_XLocaltime(&t, localtime_buf);
	tm.tm_sec	= 0;
	tm.tm_min	= 0;
	tm.tm_hour	= 0;

#ifdef SVR4
	next            = mktime(&tm);
#else
	next		= timelocal(&tm);
#endif /* SVR4 */
	next		= next + seconds(n, daysec);
	next		= adjust_dst(t, next);
	return(next);
}

extern Tick
nextday(Tick t)
{
	Tick next;

	next	= t +(int)daysec;
	next	= adjust_dst(t, next);
	return(next);
}

extern Tick
prevday(Tick t)
{
	Tick prev;

	prev	= t - (int)daysec;
	prev	= adjust_dst(t, prev);
	return(prev);
}

extern Tick
nextweek(Tick t)
{
	Tick next;

	next	= t + seconds(7, daysec);
	next	= adjust_dst(t, next);
	return(next);
}

extern Tick
prevweek(Tick t)
{
	Tick prev;

	prev	= t - seconds(7, daysec);
	prev	= adjust_dst(t, prev);
	return(prev);
}

extern Tick
next2weeks(Tick t)
{
	Tick next;

	next	= t + seconds(14, daysec);
	next	= adjust_dst(t, next);
	return(next);
}

extern Tick
prev2weeks(Tick t)
{
	Tick prev;

	prev	= t - seconds(14, daysec);
	prev	= adjust_dst(t, prev);
	return(prev);
}

/*		WORK OUT A BETTER WAY TO DO THIS!!	*/
extern Tick
prevmonth_exactday(Tick t)
{
	Tick prev; int day;
	struct tm tm;
	int sdelta;
	_Xltimeparams localtime_buf;

	tm = *_XLocaltime(&t, localtime_buf);
	sdelta = tm.tm_hour * hrsec + tm.tm_min * minsec + tm.tm_sec; 
	day = tm.tm_mday;
	if((tm.tm_mday < 31 && tm.tm_mon != 0) ||	/* at least 30 days everywhere, except Feb.*/
 	   (tm.tm_mday==31 && tm.tm_mon==6)    ||	/* two 31s -- Jul./Aug.		*/
	   (tm.tm_mday==31 && tm.tm_mon==11)   ||	/* two 31s -- Dec./Jan.		*/
	   (tm.tm_mon == 0 &&(tm.tm_mday < 29  ||(tm.tm_mday==29 && leapyr(tm.tm_year+1900))))) {	
		prev = t-monthseconds(previousmonth(t));
		prev = adjust_dst(t, prev);
	}
	else {  /* brute force */
		prev = previousmonth(previousmonth(t));		/* hop over the month */
		tm = *_XLocaltime(&prev, localtime_buf);
		tm.tm_mday = day;
#ifdef SVR4
		tm.tm_isdst = -1;
		prev =(mktime(&tm)) + sdelta;
#else
		prev =(timelocal(&tm)) + sdelta;
#endif /* SVR4 */

	}
	return(prev);
}

/*		WORK OUT A BETTER WAY TO DO THIS!!	*/
extern Tick
nextmonth_exactday(Tick t)
{
	Tick next; int day;
	struct tm tm;
	int sdelta;
	_Xltimeparams localtime_buf;

	tm = *_XLocaltime(&t, localtime_buf);
	sdelta = tm.tm_hour * hrsec + tm.tm_min * minsec + tm.tm_sec; 
	day = tm.tm_mday;
	if((tm.tm_mday < 31 && tm.tm_mon != 0) ||	/* at least 30 days everywhere, except Feb.*/
 	   (tm.tm_mday==31 && tm.tm_mon==6)    ||	/* two 31s -- Jul./Aug.		*/
	   (tm.tm_mday==31 && tm.tm_mon==11)   ||	/* two 31s -- Dec./Jan.		*/
	   (tm.tm_mon == 0 &&(tm.tm_mday < 29  ||(tm.tm_mday==29 && leapyr(tm.tm_year+1900))))) {	
		next = t+monthseconds(t);
		next = adjust_dst(t, next);
	}
	else {  /* brute force */
		next = next_nmonth(t, 2);		/* hop over the month */
		tm = *_XLocaltime(&next, localtime_buf);
		tm.tm_mday = day;
#ifdef SVR4
		tm.tm_isdst = -1;
		next = mktime(&tm) + sdelta;
#else
		next =(timelocal(&tm)) + sdelta;
#endif /* SVR4 */
	}
	return(next);
}

extern Tick
nextnyear(Tick t, int n)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm	= *_XLocaltime(&t, localtime_buf);
	tm.tm_year += n;
#ifdef SVR4
	return(mktime(&tm));
#else
	return(timelocal(&tm));
#endif /* SVR4 */
}

extern Tick
nextyear(Tick t)
{
	return(nextnyear(t, 1));
}

extern Tick
prevnday_exacttime(Tick t, int n)
{
	Tick prev;

	prev	= t - seconds(n, daysec);
	prev	= adjust_dst(t, prev);
	return(prev);
}

extern Tick
prevnyear(Tick t, int n)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm = *_XLocaltime(&t, localtime_buf);
	tm.tm_year -= n;
#ifdef SVR4
	return(mktime(&tm));
#else
	return(timelocal(&tm));
#endif /* SVR4 */
}

extern Tick
prevyear(Tick t)
{
	return(prevnyear(t, 1));
}

extern Tick
previousmonth(Tick t)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm = *_XLocaltime(&t, localtime_buf);
	tm.tm_hour=0;
	tm.tm_min=0;
	tm.tm_sec=0;
	if(tm.tm_mon==0) {
		tm.tm_mon=11;
		tm.tm_mday=1;
		tm.tm_year--;
	}
	else {
		tm.tm_mday=1;
		tm.tm_mon--;
	}
#ifdef SVR4
	tm.tm_isdst = -1;
	return(mktime(&tm));
#else
	return(timelocal(&tm));
#endif /* SVR4 */
}

extern Tick
prev_nmonth(Tick t, int n)
{
	struct tm tm;
	int	n12;
	_Xltimeparams localtime_buf;

	n12 = n/12;
	n = n%12;

	tm = *_XLocaltime(&t, localtime_buf);
	tm.tm_hour=0;
	tm.tm_min=0;
	tm.tm_sec=0;
	tm.tm_mday=1;
	if (n12 > 0)
		tm.tm_year -= n12;

	if ((tm.tm_mon = tm.tm_mon - n) < 0) {
		tm.tm_mon += 12;
		tm.tm_year--;
	}
#ifdef SVR4
	tm.tm_isdst = -1;
	return(mktime(&tm));
#else
	return(timelocal(&tm));
#endif /* SVR4 */
}

extern Tick
jan1(Tick t)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm		= *_XLocaltime(&t, localtime_buf);
	tm.tm_mon	= 0;
	tm.tm_mday	= 1;
#ifdef SVR4
	tm.tm_isdst = -1;
	return(mktime(&tm));
#else
	return(timelocal(&tm));
#endif /* SVR4 */
}

extern Tick
nextjan1(Tick t)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm		= *_XLocaltime(&t, localtime_buf);
	tm.tm_mon	= 0;
	tm.tm_mday	= 1;
	tm.tm_year++;	 
#ifdef SVR4
	tm.tm_isdst = -1;
	return(mktime(&tm));
#else
	return(timelocal(&tm));
#endif /* SVR4 */
}

extern Tick
lastjan1(Tick t)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm		= *_XLocaltime(&t, localtime_buf);
	tm.tm_mon	= 0;
	tm.tm_mday	= 1;
	tm.tm_year--;	 
#ifdef SVR4
	tm.tm_isdst = -1;
	return(mktime(&tm));
#else
	return(timelocal(&tm));
#endif /* SVR4 */
}

extern Tick
lowerbound(Tick t)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm		=  *_XLocaltime(&t, localtime_buf);
	tm.tm_sec	=  0;
	tm.tm_min	=  0;
	tm.tm_hour	=  0;
	tm.tm_isdst	= -1;
#ifdef SVR4
	return(mktime(&tm));
#else
	return(timelocal(&tm));
#endif /* SVR4 */
}
extern Tick
lower_bound(int i, Tick t)
{
        struct tm tm;
	_Xltimeparams localtime_buf;

        tm              =  *_XLocaltime(&t, localtime_buf);
        tm.tm_sec       =  0;
        tm.tm_min       =  0;
        tm.tm_hour      =  i;
	tm.tm_isdst	= -1;
#ifdef SVR4
	return(mktime(&tm));
#else
        return (timelocal(&tm));
#endif /* SVR4 */
}
extern Tick
upperbound(Tick t)
{
        struct tm tm;
	_Xltimeparams localtime_buf;

        tm              =  *_XLocaltime(&t, localtime_buf);
        tm.tm_sec       =  59;
        tm.tm_min       =  59;
        tm.tm_hour      =  23;
	tm.tm_isdst	= -1;
#ifdef SVR4
	return(mktime(&tm)-1);
#else
        return(timelocal(&tm)-1);
#endif /* SVR4 */
}

static int
leapsB4(int y)
{
	return((y-1)/4 -(y-1)/100 +(y-1)/400);
}

extern Tick
xytoclock(int x, int y, Tick t)
{
	int dd, mn, yr, ly, leaps;
	char buf[10];
	struct tm tm;
	struct tm timestruct;
	Tick tick;
	_Xltimeparams localtime_buf;

	memset(&timestruct, NULL, sizeof(struct tm));

	tm	= *_XLocaltime(&t, localtime_buf);
	mn	= tm.tm_mon + 1;
	yr	= tm.tm_year + 1900;
	leaps	= leapsB4(yr);
	ly	= leapyr(yr);

	dd = 7*(y-1) + x - 
	 (yr+leaps+3055L*(mn+2)/100-84-(mn>2)*(2-ly))%7;

	timestruct.tm_mon = tm.tm_mon;
	timestruct.tm_mday = dd;
	timestruct.tm_year = tm.tm_year;
	timestruct.tm_sec = 0;
	timestruct.tm_isdst = -1;

	tick = mktime(&timestruct);
	return(tick);
}

extern Tick
now()
{
	return(time(0));
}

extern void
set_timezone(char *tzname)
{
	static char tzenv[MAXPATHLEN];

#ifdef SVR4
        /* I don't like using 'system', but this does the right
	 * thing according to tha man pages
	 */
	if (tzname==NULL) system("unset TZ\n");
#else
	if (tzname==NULL) tzsetwall();
#endif /* SVR4 */

	else {
		sprintf(tzenv, "TZ=%s", tzname);
		putenv(tzenv);
		tzset();
	}
}
 
extern long
gmt_off()
{
        struct tm tm;
        Tick t;
        static Tick gmt;
 
#ifdef SVR4
	extern long timezone;

        gmt             = timezone;
#else
	_Xltimeparams localtime_buf;

        t       = now();
        tm      = *_XLocaltime(&t, localtime_buf);
        gmt = tm.tm_gmtoff;
#endif /* SVR4 */
        return(gmt);
}


/* [ytso 1/26/93]
 * cm now supports to up end of 1999.  This is due to the limitation
 * of cm_getdate() which can only handle up to end of 1999.
 * When cm_getdate() is improved to handle up to the system limit,
 * definitions of eot and EOT need to be changed as well as some
 * of the routines in this file and the caller of these routines.
 */
extern void
init_time()
{
	struct tm tm, gm;
	Tick	t;
	_Xltimeparams localtime_buf;
	_Xgtimeparams gmtime_buf;

#ifdef SVR4
	/* Fix for QAR 31607 */
	tzset();
	if (getenv("TZ") == NULL){
		char *tzptr;
		tzptr = malloc(strlen(tzname[0]) + strlen(tzname[1]) + 10);
		sprintf (tzptr,"TZ=%s%d%s", tzname[0], timezone/3600, tzname[1]);
		putenv(tzptr);
		tzset();
	}
#endif

	t		= now();
	tm		= *_XLocaltime(&t, localtime_buf);
	gm		= *_XGmtime(&t, gmtime_buf);

	bot		= mktime(&gm) - mktime(&tm);

	tm.tm_sec	=59;
	tm.tm_min	=59;
	tm.tm_hour	=23;
	tm.tm_mday	=31;
	tm.tm_mon	=11;
	tm.tm_year	=137;			/* Dec. 31, 2037 */
	tm.tm_isdst = -1;
	eot             =mktime(&tm);
}

extern int
seconds_to_hours(int n)
{
        return(n/(int)hrsec);
}

extern int
hours_to_seconds(int n)
{
        return(n *(int)hrsec);
}

extern int
seconds_to_minutes(int n)
{
        return(n/60);
}

extern int
minutes_to_seconds(int n)
{
        return(n *(int)minsec);
}


extern int
days_to_seconds(int n)
{
        return(n *(int)daysec);
}

seconds_to_days(int n)
{
        return(n/(int)daysec);
}

/*
extern int
weeks_to_seconds(int n)
{
        return(n *(int)wksec);
}

extern int
seconds_to_weeks(int)
{
        return(n/(int)wksec);
}
*/

extern Tick
monthdayyear(int m, int d, int y)
{
        int t, t1;
        char buf[10];

	struct tm timestruct;

	memset(&timestruct, NULL, sizeof(struct tm));
	timestruct.tm_mon = m - 1;
	timestruct.tm_mday = d;
	timestruct.tm_year = y - 1900;
	timestruct.tm_sec = 0;
	timestruct.tm_isdst = -1;

	t = mktime(&timestruct);
/*
        sprintf(buf, "%d/%d/%d", m, d, y);
        t = cm_getdate(buf, NULL);

printf("monthdayyear, m= %d, d = %d, y = %d, t1 = %d, t = %d\n", m, d, y, t1, t);
	
*/
        return(t);
}
