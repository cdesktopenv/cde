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
/* $XConsortium: repeat.c /main/7 1996/11/21 19:46:39 drk $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>
#include "csa.h"
#include "rtable4.h"
#include "cm.h"
#include "repeat.h"
#include "attr.h"
#include "iso8601.h"

#define	EOT			2147483647

extern	int	debug;

static time_t bot, eot;

typedef enum {
	minsec		= 60,
	fivemins	= 300,
	hrsec		= 3600,
	daysec		= 86400,
	wksec		= 604800,
	yrsec		= 31536000,
	leapyrsec	= 31622400
} Unit;

static unsigned int weekdaymasks[] = {
	0x1,	/* sunday */
	0x2,	/* monday */
	0x4,	/* tuesday */
	0x8,	/* wednesday */
	0x10,	/* thursday */
	0x20,	/* friday */
	0x40	/* saturday */
};

static int monthsecs[12] = {
	31*daysec,	28*daysec,	31*daysec,
	30*daysec,	31*daysec,	30*daysec,
	31*daysec,	31*daysec,	30*daysec,
	31*daysec,	30*daysec,	31*daysec
};

int monthdays[12] = {
	31,     28,     31,
	30,     31,     30,
	31,     31,     30,
	31,     30,     31
};

static int lastapptofweek(u_int mask);
static int ntimes_this_week(u_int weekmask, int firstday);
static boolean_t nthweekdayofmonth(time_t t, int *nth);
static time_t next_nmonth(time_t t, int n);
static int adjust_dst(time_t start, time_t next); 
static time_t prev_nmonth(time_t t, int n);
static time_t nextnyear(time_t t, int n);
static int timeok(time_t t);
static time_t prevnyear(time_t t, int n);
static time_t prevmonth_exactday(time_t t);
static time_t nextmonth_exactday(time_t t);
static time_t previousmonth(time_t t);
static int monthseconds(time_t t);
static int get_ndelta(time_t startdate, Period_4 period, int ntimes);
static time_t lastnthweekday(time_t t, int nth, int ntimes);
static time_t nextnthweekday(time_t t, int nth);
static time_t prevnthweekday(time_t t, int nth);
static time_t nextnday_exacttime(time_t t, int n);
static time_t prevnday_exacttime(time_t t, int n);
static time_t nextnwk_exacttime(time_t t, int n);
static time_t prevnwk_exacttime(time_t t, int n);
static time_t nextnmth_exactday(time_t t, int n);
static time_t prevnmth_exactday(time_t t, int n);
static time_t nextmonTofri(time_t t);
static time_t prevmonTofri(time_t t);
static time_t nextmonwedfri(time_t t);
static time_t prevmonwedfri(time_t t);
static time_t nexttuethur(time_t t);
static time_t prevtuethur(time_t t);
static time_t nextdaysofweek(time_t t, int weekmask);
static time_t prevdaysofweek(time_t t, int weekmask);

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
	time_t	t;
	_Xltimeparams localtime_buf;
	_Xgtimeparams gmtime_buf;

#ifdef __osf__
	char *tzptr;
#endif

#ifdef __osf__
	/* Fix for QAR 31607 */
	if (getenv("TZ") == NULL){
		tzset();
		tzptr = malloc(strlen(tzname[0]) + strlen(tzname[1]) + 10);
		sprintf (tzptr,"TZ=%s%d%s", tzname[0], timezone/3600, tzname[1]);
		putenv(tzptr);
		tzset();
	}
	else
		tzset();
#endif

	t		= time(0);
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

extern void
_DtCms_adjust_appt_startdate(Appt_4 *appt)
{
	struct tm *tm;
	_Xltimeparams localtime_buf;

	if (appt->period.period < monThruFri_4 ||
	    appt->period.period > tueThur_4)
		return;

	tm = _XLocaltime(&(appt->appt_id.tick), localtime_buf);
	switch (appt->period.period) {
	case monThruFri_4:
		if (tm->tm_wday < 1 || tm->tm_wday > 5)
			appt->appt_id.tick = _DtCms_next_tick_v4(
						appt->appt_id.tick,
						appt->period);
		break;
	case monWedFri_4:
		if ((tm->tm_wday % 2) == 0)
			appt->appt_id.tick = _DtCms_next_tick_v4(
						appt->appt_id.tick,
						appt->period);
		break;
	case tueThur_4:
		if (tm->tm_wday != 2 && tm->tm_wday != 4)
			appt->appt_id.tick = _DtCms_next_tick_v4(
						appt->appt_id.tick,
						appt->period);
		break;
	}
}

/*
 * Calculate the actual number of instances of the repeating event.
 */
extern int
_DtCms_get_ninstance_v4(Appt_4 *appt)
{
	struct tm *tm;
	int i, pdelta, ndelta, ninstance, timesperweek;
	double dninstance;
	_Xltimeparams localtime_buf;

	if (appt->ntimes == _DtCM_OLD_REPEAT_FOREVER)
		return(appt->ntimes);

	switch (appt->period.period) {
	case everyNthDay_4:
	case everyNthWeek_4:
	case everyNthMonth_4:
		ninstance = (appt->ntimes+(appt->period.nth-1))/appt->period.nth;
		break;

	case monThruFri_4:
		tm = _XLocaltime(&(appt->appt_id.tick), localtime_buf);
		pdelta = 6 - tm->tm_wday;
		ndelta = get_ndelta(appt->appt_id.tick, appt->period,
				appt->ntimes);
		dninstance = (double)(appt->ntimes - 1) * 5 + pdelta - ndelta;
		ninstance = (dninstance > _DtCM_OLD_REPEAT_FOREVER) ?
				_DtCM_OLD_REPEAT_FOREVER : (int)dninstance;
		break;

	case monWedFri_4:
		tm = _XLocaltime(&(appt->appt_id.tick), localtime_buf);
		pdelta = (7 - tm->tm_wday) / 2;
		ndelta = get_ndelta(appt->appt_id.tick, appt->period,
				appt->ntimes);
		dninstance = (double)(appt->ntimes - 1) * 3 + pdelta - ndelta;
		ninstance = (dninstance > _DtCM_OLD_REPEAT_FOREVER)
				? _DtCM_OLD_REPEAT_FOREVER : (int)dninstance;
		break;

	case tueThur_4:
		tm = _XLocaltime(&(appt->appt_id.tick), localtime_buf);
		pdelta = (tm->tm_wday == 2) ? 2 : 1;
		ndelta = get_ndelta(appt->appt_id.tick, appt->period,
				appt->ntimes);
		dninstance = (double)(appt->ntimes - 1) * 2 + pdelta - ndelta;
		ninstance = (dninstance > _DtCM_OLD_REPEAT_FOREVER) ?
				_DtCM_OLD_REPEAT_FOREVER : (int)dninstance;
		break;

	case daysOfWeek_4:
		tm = _XLocaltime(&(appt->appt_id.tick), localtime_buf);
		timesperweek = ntimes_this_week((u_int)appt->period.nth, 0);
		pdelta = ntimes_this_week((u_int)appt->period.nth, tm->tm_wday);
		ndelta = get_ndelta(appt->appt_id.tick, appt->period,
				appt->ntimes);
		dninstance = (double)(appt->ntimes-1) * timesperweek +
				pdelta - ndelta;
		ninstance = (dninstance > _DtCM_OLD_REPEAT_FOREVER) ?
				_DtCM_OLD_REPEAT_FOREVER : (int)dninstance;

		break;
	default:
		ninstance = appt->ntimes;
	}

	return ninstance;
}

/*
 * calculate the ntimes value which, depending on the
 * repeating event type, may not be the same
 * as the actual number of instances
 */
extern int
_DtCms_get_new_ntimes_v4(Period_4 period, time_t tick, int ninstance)
{
	struct tm *tm;
	int ntimes;
	int delta = 0, firstweek, timesperweek;
	_Xltimeparams localtime_buf;

	switch (period.period) {
	case everyNthDay_4:
	case everyNthWeek_4:
	case everyNthMonth_4:
		ntimes = ninstance * period.nth;
		break;
	case monThruFri_4:
		tm = _XLocaltime(&tick, localtime_buf);
		if (ninstance % 5)
			delta = ((ninstance % 5) > (6 - tm->tm_wday)) ? 2 : 1;
		else if (tm->tm_wday != 1)
			delta = 1;
		ntimes = (ninstance/5) + delta;
		break;
	case monWedFri_4:
		tm = _XLocaltime(&tick, localtime_buf);
		if (ninstance % 3)
			delta = ((ninstance % 3) > ((7-tm->tm_wday)/2)) ? 2:1;
		else if (tm->tm_wday != 1)
			delta = 1;
		ntimes = (ninstance/3) + delta;
		break;
	case tueThur_4:
		tm = _XLocaltime(&tick, localtime_buf);
		if (ninstance % 2 || tm->tm_wday != 2)
			delta = 1;
		ntimes = (ninstance/2) + delta;
		break;
	case daysOfWeek_4:
		tm = _XLocaltime(&tick, localtime_buf);
		timesperweek = ntimes_this_week((u_int)period.nth, 0);
		firstweek=ntimes_this_week((u_int)period.nth,tm->tm_wday);
		if (ninstance % timesperweek)
			delta = ((ninstance % timesperweek) > firstweek) ? 2:1;
		else if (firstweek != timesperweek)
			delta = 1;
		ntimes = (ninstance/timesperweek) + delta;
		break;
	default:
		ntimes = ninstance;
		break;
	}

	return ntimes;
}

extern time_t
_DtCms_first_tick_v4(time_t t, Period_4 period, int ordinal)
{
	int i;
	time_t ftick;

	ftick = t;
	for (i = 1; i < ordinal; i++)
		ftick = _DtCms_prev_tick_v4(ftick, period);

	return(ftick);
}


/*
 * Given a time, calculate the closest instance whose
 * tick is later than the time.
 * If the calculated tick does not pass timeok(), ftick is
 * returned and ordinal set to 1.
 */
extern time_t
_DtCms_closest_tick_v4(time_t target, time_t ftick, Period_4 period, int *ordinal)
{
	time_t ctick;
	int delta = 0;
	int remainder = 0;
	int ndays;
	struct tm *tm;
	struct tm tm1, tm2;
	_Xltimeparams localtime_buf;

	if (target <= ftick) {
		*ordinal = 1;
		return(ftick);
	}

	if (period.period < monthly_4 || period.period == everyNthDay_4 ||
	    period.period == everyNthWeek_4) {
		tm1 = *_XLocaltime(&ftick, localtime_buf);
		tm2 = *_XLocaltime(&target, localtime_buf);
	}
	switch(period.period) {
	case daily_4:
		delta = (target - ftick) / daysec;
		remainder = target - ftick - daysec * delta;
		if (tm1.tm_isdst == 1 && tm1.tm_isdst != tm2.tm_isdst)
			remainder -= hrsec;
		*ordinal = delta + (remainder>0?1:0) + 1;
		ctick = nextnday_exacttime(ftick, *ordinal - 1);
		break;
	case weekly_4:
		delta = (target - ftick) / wksec;
		remainder = target - ftick - wksec * delta;
		if (tm1.tm_isdst == 1 && tm1.tm_isdst != tm2.tm_isdst)
			remainder -= hrsec;
		*ordinal = delta + (remainder>0?1:0) + 1;
		ctick = nextnwk_exacttime(ftick, *ordinal - 1);
		break;
	case biweekly_4:
		delta = (target - ftick) / (wksec * 2);
		remainder = target - ftick - wksec * 2 * delta;
		if (tm1.tm_isdst == 1 && tm1.tm_isdst != tm2.tm_isdst)
			remainder -= hrsec;
		*ordinal = delta + (remainder>0?1:0) + 1;
		ctick = nextnwk_exacttime(ftick, 2 * (*ordinal - 1));
		break;
	case monthly_4:
		tm = _XLocaltime(&ftick, localtime_buf);
		/*
		 * Calculate the closest tick only if the date
		 * is < 29; otherwise just return the first tick.
		 * Use 32 to take care of dst time difference.
		 * Without dst, we can use 31.
		 */
		if (tm->tm_mday < 29) {
			delta = (target - ftick) / (daysec * 32);
			remainder = target - ftick - (daysec * 32) * delta;
			*ordinal = delta + (remainder>0?1:0) + 1;
			ctick = nextnmth_exactday(ftick, *ordinal - 1);
		} else {
			ctick = ftick;
			*ordinal = 1;
		}
		break;
	case yearly_4:
		tm = _XLocaltime(&ftick, localtime_buf);
		if (tm->tm_mday == 29 && tm->tm_mon == 1) {
			delta = (target - ftick) / (yrsec * 4 + daysec);
			remainder = target - ftick - (yrsec * 4) * delta;
			*ordinal = delta + (remainder>0?1:0) + 1;
			ctick = nextnyear(ftick, (*ordinal - 1) * 4);
		} else {
			delta = (target - ftick) / yrsec;
			/* adjustment for leap year */
			remainder = tm->tm_year % 4;
			if (remainder == 0 || (remainder + delta) > 3)
				delta--;
			remainder = target - ftick - yrsec * delta;
			*ordinal = delta + (remainder>0?1:0) + 1;
			ctick = nextnyear(ftick, *ordinal - 1);
		}
		break;
	case nthWeekday_4:
		/* 36 is 5 weeks ==> maximum interval between 2 instances */
		delta = (target - ftick) / (daysec * 36);
		remainder = target - ftick - (daysec * 36) * delta;
		*ordinal = delta + (remainder>0?1:0) + 1;
		ctick = lastnthweekday(ftick, period.nth, *ordinal - 1);
		break;
	case everyNthDay_4:
		delta = (target - ftick) / (daysec * period.nth);
		remainder = target - ftick - (daysec * period.nth) * delta;
		if (tm1.tm_isdst == 1 && tm1.tm_isdst != tm2.tm_isdst)
			remainder -= hrsec;
		*ordinal = delta + (remainder>0?1:0) + 1;
		ctick = nextnday_exacttime(ftick,
				period.nth * (*ordinal - 1));
		break;
	case everyNthWeek_4:
		delta = (target - ftick) / (wksec * period.nth);
		remainder = target - ftick - (wksec * period.nth) * delta;
		if (tm1.tm_isdst == 1 && tm1.tm_isdst != tm2.tm_isdst)
			remainder -= hrsec;
		*ordinal = delta + (remainder>0?1:0) + 1;
		ctick = nextnwk_exacttime(ftick,
				period.nth * (*ordinal - 1));
		break;
	case everyNthMonth_4:
		tm = _XLocaltime(&ftick, localtime_buf);
		if (tm->tm_mday < 29) {
			delta = (target - ftick) / (daysec * 32 * period.nth);
			remainder = target-ftick-(daysec*32*period.nth)*delta;
			*ordinal = delta + (remainder>0?1:0) + 1;
			ctick = nextnmth_exactday(ftick,
					period.nth * (*ordinal - 1));
		} else {
			ctick = ftick;
			*ordinal = 1;
		}
		break;
	case monThruFri_4:
	case monWedFri_4:
	case tueThur_4:
	case daysOfWeek_4:
		delta = (target - ftick) / wksec;
		tm = _XLocaltime(&ftick, localtime_buf);

		switch (period.period) {
		case monThruFri_4:
			*ordinal = delta * 5 + 6 - tm->tm_wday;
			break;
		case monWedFri_4:
			*ordinal = delta * 3 + (7 - tm->tm_wday) / 2;
			break;
		case tueThur_4:
			*ordinal = delta * 2 + ((tm->tm_wday == 2) ? 2 : 1);
			break;
		case daysOfWeek_4:
			*ordinal = delta * ntimes_this_week((u_int)period.nth,0)
					+ ntimes_this_week((u_int)period.nth,
						tm->tm_wday);
		}

		/* delta*daysperweek+(lastapptofweek-firstday in first week) */
		if (period.period == daysOfWeek_4) {
			ndays = delta * 7 +
				lastapptofweek((u_int)period.nth) - tm->tm_wday;
			ctick = ftick + ndays * daysec;
		} else if (period.period == tueThur_4) {
			ndays = delta * 7 + 4 - tm->tm_wday;
			ctick = ftick + ndays * daysec;
		} else {
			ndays = delta * 7 + 5 - tm->tm_wday;
			ctick = ftick + ndays * daysec;
		}

		if (ctick > target) { /* need to go back 1 week */
			ndays -= 7;
			if (ndays < 0) {
				*ordinal = 1;
				ctick = ftick;
			} else {
				if (period.period == monThruFri_4)
					*ordinal -= 5;
				else if (period.period == monWedFri_4)
					*ordinal -= 3;
				else if (period.period == tueThur_4)
					*ordinal -= 2;
				ctick -= (7 * daysec);
			}
		}
		ctick = adjust_dst(ftick, ctick);
		break;
	default:
		*ordinal = 1;
		ctick = ftick;
	}

	if (timeok(ctick))
		return(ctick);
	else {
		*ordinal = 1;
		return(ftick);
	}
}

/*
 * Calculate the tick of the last instance of a repeating event.
 * If the calculated tick does not pass timeok(), EOT is returned.
 */
extern time_t
_DtCms_last_tick_v4(time_t ftick, Period_4 period, int ntimes)
{
	struct tm *tm;
	double dltick;
	time_t ltick;
	int i;
	_Xltimeparams localtime_buf;

	if (ntimes >= _DtCM_OLD_REPEAT_FOREVER)
		return(EOT);

	if (period.enddate != 0)
		return(period.enddate);

	switch(period.period) {
	case weekly_4:
		ltick = nextnwk_exacttime(ftick, ntimes - 1);
		break;
	case biweekly_4:
		/* 2 * (ntimes-1) won't overflow an integer since
		 * we make sure ntimes is < EOT
		 */
		ltick = nextnwk_exacttime(ftick, 2 * (ntimes - 1));
		break;
	case daily_4:
		ltick = nextnday_exacttime(ftick, ntimes - 1);
		break;
	case monthly_4:
		tm = _XLocaltime(&ftick, localtime_buf);
		/*
		 * calculate the last tick only if the date
		 * is < 29; otherwise return EOT to force calculation
		 */
		if (tm->tm_mday < 29)
			ltick = nextnmth_exactday(ftick, ntimes - 1);
		else
			ltick = EOT;
		break;
	case yearly_4:
		/* 2038 is the last year that can be represented.
		 * this check is to prevent (ntimes-1)*4 from integer overflow
		 */
		if (ntimes > 2038)
			ltick = EOT;
		else {
			tm = _XLocaltime(&ftick, localtime_buf);
			if (tm->tm_mday == 29 && tm->tm_mon == 1)
				ltick = nextnyear(ftick, (ntimes - 1) * 4);
			else
				ltick = nextnyear(ftick, ntimes - 1);
		}
		break;
	case nthWeekday_4:
		ltick = lastnthweekday(ftick, period.nth, ntimes - 1);
		break;
	case everyNthDay_4:
		ltick = nextnday_exacttime(ftick, period.nth * 
			(((ntimes+(period.nth-1))/period.nth) - 1));
		break;
	case everyNthWeek_4:
		ltick = nextnwk_exacttime(ftick, period.nth *
			(((ntimes+(period.nth-1))/period.nth) - 1));
		break;
	case everyNthMonth_4:
		tm = _XLocaltime(&ftick, localtime_buf);
		if (tm->tm_mday < 29)
			ltick = nextnmth_exactday(ftick, period.nth *
				(((ntimes+(period.nth-1))/period.nth) -1));
		else
			ltick = EOT;
		break;
	case monThruFri_4:
	case monWedFri_4:
	case tueThur_4:
	case daysOfWeek_4:
		tm = _XLocaltime(&ftick, localtime_buf);

		/* (ntimes-1)*daysperweek+(lastapptofweek-fstapptofFstweek) */
		if (period.period == daysOfWeek_4)
			dltick = ftick +
				((double)(ntimes - 1) * 7 +
				lastapptofweek((u_int)period.nth) - tm->tm_wday)
				* daysec;
		else if (period.period == tueThur_4)
			dltick = ftick +
				((double)(ntimes - 1) * 7 + (4 - tm->tm_wday)) *
				daysec;
		else
			dltick = ftick +
				((double)(ntimes - 1) * 7 + (5 - tm->tm_wday)) *
				daysec;

		if (dltick >= EOT || dltick < 0)
			ltick = EOT;
		else
			ltick = adjust_dst(ftick, (time_t)dltick);
		break;
	default:
		break;
	}
	if(timeok(ltick))
		return(ltick);
	else
		return(EOT);
}

/*
 * Calculate the tick of next instance.
 * If the calculated tick does not pass timeok(), EOT is returned.
 */
extern time_t
_DtCms_next_tick_v4(time_t tick, Period_4 period)
{
        time_t next;
	struct tm *tm;
	_Xltimeparams localtime_buf;

        switch(period.period) {
                case weekly_4:
                        next = nextnwk_exacttime(tick, 1);
                        break;
                case biweekly_4:
                        next = nextnwk_exacttime(tick, 2);
                        break;
                case daily_4:
                        next = nextnday_exacttime(tick, 1);
                        break;
                case monthly_4:
                        next = nextmonth_exactday(tick);
                        break;
                case yearly_4:
			tm = _XLocaltime(&tick, localtime_buf);
			if (tm->tm_mday == 29 && tm->tm_mon == 1)
				next = nextnyear(tick, 4);
			else
                        	next = nextnyear(tick, 1);
                        break;
		case nthWeekday_4:
			next = nextnthweekday(tick, period.nth);
			break;
		case everyNthDay_4:
			next = nextnday_exacttime(tick, period.nth);
			break;
		case everyNthWeek_4:
			next = nextnwk_exacttime(tick, period.nth);
			break;
		case everyNthMonth_4:
			next = nextnmth_exactday(tick, period.nth);
			break;
		case monThruFri_4:
			next = nextmonTofri(tick);
			break;
		case monWedFri_4:
			next = nextmonwedfri(tick);
			break;
		case tueThur_4:
			next = nexttuethur(tick);
			break;
		case daysOfWeek_4:
			next = nextdaysofweek(tick, period.nth);
			break;
                default:
                        break;
        }
        if(next != tick && timeok(next)) return(next);
        else return(EOT);
}

/*
 * Calculate the tick of previous instance.
 * If the calculated tick does not pass timeok(), bot-1 is returned.
 */
extern time_t
_DtCms_prev_tick_v4(time_t tick, Period_4 period)
{
        time_t prev;
	struct tm *tm;
	_Xltimeparams localtime_buf;

        switch(period.period) {
                case weekly_4:
                        prev = prevnwk_exacttime(tick, 1);
                        break;
                case biweekly_4:
                        prev = prevnwk_exacttime(tick, 2);
                        break;
                case daily_4:
                        prev = prevnday_exacttime(tick, 1);
                        break;
                case monthly_4:
                        prev = prevmonth_exactday(tick);
                        break;
                case yearly_4:
			tm = _XLocaltime(&tick, localtime_buf);
			if (tm->tm_mday == 29 && tm->tm_mon == 1)
				prev = prevnyear(tick, 4);
			else
                        	prev = prevnyear(tick, 1);
                        break;
		case nthWeekday_4:
			prev = prevnthweekday(tick, period.nth);
			break;
		case everyNthDay_4:
			prev = prevnday_exacttime(tick, period.nth);
			break;
		case everyNthWeek_4:
			prev = prevnwk_exacttime(tick, period.nth);
			break;
		case everyNthMonth_4:
			prev = prevnmth_exactday(tick, period.nth);
			break;
		case monThruFri_4:
			prev = prevmonTofri(tick);
			break;
		case monWedFri_4:
			prev = prevmonwedfri(tick);
			break;
		case tueThur_4:
			prev = prevtuethur(tick);
			break;
		case daysOfWeek_4:
			prev = prevdaysofweek(tick, period.nth);
			break;
                default:
                        break;
        }
        if(prev != tick && timeok(prev)) return(prev);
        else return(bot-1);
}

/*
 * dont_care_cancel:
 *	TRUE - it is a match regard_DtCmsIsLess the event is cancelled, 
 *	FALSE - it is not a match if the event is cancelled.
 */
extern int
_DtCms_in_repeater(Id_4 *key, Appt_4 *p_appt, boolean_t dont_care_cancel)
{
	Period_4 period;
	int	ordinal;
	int	ntimes;
	time_t	tick;

	ntimes = _DtCms_get_ninstance_v4(p_appt);
	period = p_appt->period;
	tick = _DtCms_closest_tick_v4(key->tick, p_appt->appt_id.tick, period, &ordinal);
	ordinal--;
	while (++ordinal <= ntimes)
	{
		if (tick > key->tick)		/* out-of-bound */
			break;
		if (tick == key->tick)
		{
			if (dont_care_cancel)
				return (ordinal);
			if (!_DtCms_marked_4_cancellation (p_appt, ordinal))
				return (ordinal);
		}
		tick = _DtCms_next_tick_v4 (tick, period);
	}

	return (0);
}

extern int
_DtCms_marked_4_cancellation(Appt_4 *a, int i)
{
	Except_4 *p;

	if (a==NULL)
		return(0);

	p = a->exception;	/* in descending order for faster access */
	while (p!=NULL) {
		if (i > p->ordinal)
			break;
		if (i == p->ordinal)
			return(1);
		p = p->next;
	}
	return(0);
}

extern time_t
next_ndays(time_t t, int n)
{
	time_t next;
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
	next		= next + n * daysec;
	next		= adjust_dst(t, next);
	return(next);
}

extern time_t
next_nmins(time_t t, int m)
{
	time_t next;
	struct tm tm;
	_Xltimeparams localtime_buf;

        tm              = *_XLocaltime(&t, localtime_buf);
        tm.tm_sec       = 0;
        tm.tm_min       = 0;
 
	next            = mktime(&tm);
	next		= next + m * minsec;
	next		= adjust_dst(t, next);
	return(next);
}

extern time_t
_DtCmsBeginOfDay(time_t t)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm		=  *_XLocaltime(&t, localtime_buf);
	tm.tm_sec	=  0;
	tm.tm_min	=  0;
	tm.tm_hour	=  0;
	return(mktime(&tm));
}

extern time_t
_DtCmsTimeOfDay(time_t t)
{
	struct tm tm;
	_Xltimeparams localtime_buf;

	tm		=  *_XLocaltime(&t, localtime_buf);
	tm.tm_sec	=  0;
	tm.tm_min	=  0;
	tm.tm_hour	=  0;
	return(t - mktime(&tm));
}

/*
 * Given a weekmask, find the last appointment in the week
 */
static int
lastapptofweek(u_int mask)
{
	int n;

	if (mask == 0)
		return -1;

	for (n = -1; mask != 0; n++, mask = mask >> 1);

	return n;
}

/*
 * Given a weekmask and the first day of the week, calculate
 * the number of times outstanding in the week.
 */
static int
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

static boolean_t
nthweekdayofmonth(time_t t, int *nth)
{
	struct tm tm, tm2, tmfirstday;
	time_t	firstday;
	_Xltimeparams localtime_buf;

	tmfirstday = tm = *_XLocaltime(&t, localtime_buf);

	*nth = (12 + tm.tm_mday - tm.tm_wday)/7;

	tmfirstday.tm_hour = 0;
	tmfirstday.tm_min = 0;
	tmfirstday.tm_sec = 0;
	tmfirstday.tm_mday = 1;
	firstday = mktime(&tmfirstday);
	tmfirstday = *_XLocaltime(&firstday, localtime_buf);

	if (tm.tm_wday < tmfirstday.tm_wday)
		(*nth)--;

	if (*nth < 4)
		return B_FALSE;
	else {
		t += (7 * daysec);
		tm2 = *_XLocaltime(&t, localtime_buf);

		return((tm.tm_mon == tm2.tm_mon) ? B_FALSE : B_TRUE);
	}
}

/*
 * If the result falls beyond the system limit, -1 is returned by mktime().
 */
static time_t
next_nmonth(time_t t, int n)
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

static int
adjust_dst(time_t start, time_t next) 
{
	struct tm oldt;
	struct tm newt;
	_Xltimeparams localtime_buf;

	oldt = *_XLocaltime(&start, localtime_buf);
	newt = *_XLocaltime(&next, localtime_buf);

	if (oldt.tm_isdst == newt.tm_isdst) {
		return (next);
	} else if (oldt.tm_isdst == 1) {
		return (next + (int)hrsec);
	} else {
		return (next - (int)hrsec);
	}
}

static time_t
prev_nmonth(time_t t, int n)
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

extern int /* find dow(0-6) that 1st dom falls on */
fdom(Tick t)
{
        struct tm tm;
	_Xltimeparams localtime_buf;

        tm              = *_XLocaltime(&t, localtime_buf);
        tm.tm_mday      = 1;
        tm.tm_isdst     = -1;
        t               = mktime(&tm);
        tm              = *_XLocaltime(&t, localtime_buf);
        return(tm.tm_wday);
}

extern int
ldom(Tick t /* find dow(0-6) that last dom falls on */ )
{
        struct tm tm;
	_Xltimeparams localtime_buf;

        tm              = *_XLocaltime(&t, localtime_buf);
        tm.tm_mday      = monthlength(t);
        tm.tm_isdst     = -1;
        t               = mktime(&tm);
        tm              = *_XLocaltime(&t, localtime_buf);
        return(tm.tm_wday);
}

extern boolean_t
_DtCmsInExceptionList(cms_entry *eptr, time_t tick)
{
	CSA_date_time_entry	*dt = NULL;
	time_t			time;

	if (eptr->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value)
		dt = eptr->attrs[CSA_ENTRY_ATTR_EXCEPTION_DATES_I].value->\
			item.date_time_list_value;

	for (; dt != NULL; dt = dt->next) {
		if (_csa_iso8601_to_tick(dt->date_time, &time))
			continue;

		if (time == tick)
			return (B_TRUE);
	}

	return (B_FALSE);
}

static time_t
nextnyear(time_t t, int n)
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

static int
timeok(time_t t)
{
	int r =((t >= bot) &&(t <= eot));
	return(r);
}

static time_t
prevnyear(time_t t, int n)
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

static time_t
prevmonth_exactday(time_t t)
{
	time_t prev; int day;
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

static time_t
nextmonth_exactday(time_t t)
{
	time_t next; int day;
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

static time_t
previousmonth(time_t t)
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

static int
monthseconds(time_t t)
{
	int mon;
	struct tm tm;
	_Xltimeparams localtime_buf;
	
	tm = *_XLocaltime(&t, localtime_buf);
	mon = tm.tm_mon;
	return(((mon==1) && leapyr(tm.tm_year+1900)) ?
		29*daysec : monthsecs[mon]);
}

/*
 * find the number of instances to be subtracted
 */
static int
get_ndelta(time_t startdate, Period_4 period, int ntimes)
{
	struct tm *tm;
	int ndelta = 0;
	time_t lastdate;
	double dlastdate;
	_Xltimeparams localtime_buf;

	if (period.enddate == 0)
		return(ndelta);

	/* find last day of the series */
	dlastdate = startdate + (double)wksec * (ntimes - 1); /* last week */
	if (dlastdate > EOT)
		return(ndelta);
	else
		lastdate = (time_t)dlastdate;

	tm = _XLocaltime(&lastdate, localtime_buf);
	if (period.period == monThruFri_4 || period.period == monWedFri_4)
		lastdate = lastdate + daysec * (5 - tm->tm_wday);
	else if (period.period == tueThur_4)
		lastdate = lastdate + daysec * (4 - tm->tm_wday);
	else if (period.period == daysOfWeek_4)
		lastdate = lastdate + daysec *
			(lastapptofweek((u_int)period.nth) - tm->tm_wday);

	if (period.enddate > lastdate)
		return(ndelta);

	tm = _XLocaltime(&period.enddate, localtime_buf);
	switch (period.period) {
	case monThruFri_4:
		ndelta = 5 - tm->tm_wday;
		break;
	case monWedFri_4:
		if (tm->tm_wday < 3)
			ndelta = 2;
		else if (tm->tm_wday < 5)
			ndelta = 1;
		break;
	case tueThur_4:
		if (tm->tm_wday < 2)
			ndelta = 2;
		else if (tm->tm_wday < 4)
			ndelta = 1;
		break;
	case daysOfWeek_4:
		ndelta = ntimes_this_week((u_int)period.nth, tm->tm_wday) - 1;
		break;
	}
	return(ndelta);
}

static time_t
lastnthweekday(time_t t, int nth, int ntimes)
{
	struct tm tm1, tm2;
	time_t tick, ntick;
	int delta;
	int sdelta;
	_Xltimeparams localtime_buf;

	/*
	 * if nth is not specified, assume it's the
	 * 4th week for the ambiguous case.
	 */
	if (nth == 0) {
		nthweekdayofmonth(t, &nth);
		if (nth > 4)
			nth = -1;
	}

	tm1 = *_XLocaltime(&t, localtime_buf);
	sdelta = tm1.tm_hour * hrsec + tm1.tm_min * minsec + tm1.tm_sec; 

	if (nth > 0) {
		if ((tick = next_nmonth(t, ntimes)) == EOT || tick < 0)
			return(EOT);

		tm2 = *_XLocaltime(&tick, localtime_buf);

		delta = tm1.tm_wday - tm2.tm_wday;
		if (delta < 0)
			delta += 7;

		ntick = tick + (((nth - 1) * 7 + delta) * daysec) + sdelta;
	} else {
		if ((tick = next_nmonth(t, ntimes + 1)) == EOT || tick < 0)
			return(EOT);

		tm2 = *_XLocaltime(&tick, localtime_buf);

		delta = tm2.tm_wday - tm1.tm_wday;
		if (tm1.tm_wday >= tm2.tm_wday)
			delta += 7;

		ntick = tick - (delta * daysec) + sdelta;
	}
	ntick = adjust_dst(tick, ntick);

	return (ntick);
}

static time_t
nextnthweekday(time_t t, int nth)
{
	struct tm tm1, tm2;
	time_t tick, ntick;
	int delta;
	int sdelta;
	_Xltimeparams localtime_buf;

	/*
	 * if nth is not specified, assume it's the
	 * 4th week for the ambiguous case.
	 */
	if (nth == 0) {
		nthweekdayofmonth(t, &nth);
		if (nth > 4)
			nth = -1;
	}

	tm1 = *_XLocaltime(&t, localtime_buf);
	sdelta = tm1.tm_hour * hrsec + tm1.tm_min * minsec + tm1.tm_sec; 

	if (nth > 0) {
		tick = next_nmonth(t, 1);
		tm2 = *_XLocaltime(&tick, localtime_buf);

		delta = tm1.tm_wday - tm2.tm_wday;
		if (delta < 0)
			delta += 7;

		ntick = tick + (((nth - 1) * 7 + delta) * daysec) + sdelta;
	} else {
		tick = next_nmonth(t, 2);
		tm2 = *_XLocaltime(&tick, localtime_buf);

		delta = tm2.tm_wday - tm1.tm_wday;
		if (tm1.tm_wday >= tm2.tm_wday)
			delta += 7;

		ntick = tick - (delta * daysec) + sdelta;
	}
	ntick = adjust_dst(tick, ntick);

	return (ntick);
}

static time_t
prevnthweekday(time_t t, int nth)
{
	struct tm tm1, tm2;
	time_t tick, ptick;
	int delta;
	int sdelta;
	_Xltimeparams localtime_buf;

	/*
	 * if nth is not specified, assume it's the
	 * 4th week for the ambiguous case.
	 */
	if (nth == 0) {
		nthweekdayofmonth(t, &nth);
		if (nth > 4)
			nth = -1;
	}

	tm1 = *_XLocaltime(&t, localtime_buf);
	sdelta = tm1.tm_hour * hrsec + tm1.tm_min * minsec + tm1.tm_sec; 

	if (nth > 0) {
		tick = prev_nmonth(t, 1);
		tm2 = *_XLocaltime(&tick, localtime_buf);

		delta = tm1.tm_wday - tm2.tm_wday;
		if (delta < 0)
			delta += 7;

		ptick = tick + (((nth - 1) * 7 + delta) * daysec) + sdelta;
	} else {
		tick = prev_nmonth(next_nmonth(t, 1), 1);
		tm2 = *_XLocaltime(&tick, localtime_buf);

		delta = tm2.tm_wday - tm1.tm_wday;
		if (tm1.tm_wday >= tm2.tm_wday)
			delta += 7;

		ptick = tick - (delta * daysec) + sdelta;
	}
	ptick = adjust_dst(tick, ptick);

	return (ptick);
}

/* use double in this routine to avoid integer overflow
 * in case n is very large.
 */
static time_t
nextnday_exacttime(time_t t, int n)
{
	double next;

	next	= t + (double)n * daysec;
	if (next >= EOT || next < 0)
		return(EOT);
	else {
		next = adjust_dst(t, (time_t)next);
		return((time_t)next);
	}
}

/*
 * This is defined in the private library and is used also by the front
 * end -- should it be here?
 */
static time_t
prevnday_exacttime(time_t t, int n)
{
	time_t prev;

	prev	= t - (n * daysec);
	prev	= adjust_dst(t, prev);
	return(prev);
}

/* use double in this routine to avoid integer overflow
 * in case n is very large.
 */
static time_t
nextnwk_exacttime(time_t t, int n)
{
	double next;

	next	= t + (double)n * 7 * daysec;
	if (next >= EOT || next < 0)
		return(EOT);
	else {
		next = adjust_dst(t, (time_t)next);
		return((time_t)next);
	}
}

static time_t
prevnwk_exacttime(time_t t, int n)
{
	time_t prev;

	prev	= t - n * 7 * daysec;
	prev	= adjust_dst(t, prev);
	return(prev);
}

static time_t
nextnmth_exactday(time_t t, int n)
{
	struct tm tm1, tm2;
	boolean_t done = B_FALSE;
	time_t next;
	_Xltimeparams localtime_buf;

	tm1 = *_XLocaltime(&t, localtime_buf);
	while (!done) {
		if ((next = next_nmonth(t, n)) == EOT || next < 0)
			return(EOT);

		tm2 = *_XLocaltime(&next, localtime_buf);

		/* 1. at least 30 days except feb
		 * 2. 2/29 on leap year
		 * 3. 31st on the appropriate month
		 */
		if ((tm1.tm_mday < 31 && tm2.tm_mon != 1) ||
		    (tm2.tm_mon == 1 && (tm1.tm_mday < 29 ||
		      (tm1.tm_mday == 29 && leapyr(tm2.tm_year + 1900)))) ||
		    (tm1.tm_mday == 31 && ((tm2.tm_mon > 6 && tm2.tm_mon % 2) ||
		      ((tm2.tm_mon <= 6 && (tm2.tm_mon % 2 == 0)))))) {
			tm2.tm_sec = tm1.tm_sec;
			tm2.tm_min = tm1.tm_min;
			tm2.tm_hour = tm1.tm_hour;
			tm2.tm_mday = tm1.tm_mday;
			done = B_TRUE;
		} else
			t = next;
	}

#ifdef SVR4
	tm2.tm_isdst = -1;
	next = mktime(&tm2);
#else
	next = (timelocal(&tm2));
#endif
	return(next);
}

static time_t
prevnmth_exactday(time_t t, int n)
{
	struct tm tm1, tm2;
	boolean_t done = B_FALSE;
	time_t prev;
	_Xltimeparams localtime_buf;

	tm1 = *_XLocaltime(&t, localtime_buf);
	while (!done) {
		prev = prev_nmonth(t, n);
		tm2 = *_XLocaltime(&prev, localtime_buf);

		if ((tm1.tm_mday < 30 && tm2.tm_mon != 1) ||
		    (tm2.tm_mon == 1 && (tm1.tm_mday < 29 ||
		      (tm1.tm_mday == 29 && leapyr(tm2.tm_year + 1900)))) ||
		    (tm1.tm_mday == 31 && ((tm2.tm_mon > 6 && tm2.tm_mon % 2) ||
		      ((tm2.tm_mon <= 6 && (tm2.tm_mon % 2 == 0)))))) {
			tm2.tm_sec = tm1.tm_sec;
			tm2.tm_min = tm1.tm_min;
			tm2.tm_hour = tm1.tm_hour;
			tm2.tm_mday = tm1.tm_mday;
			done = B_TRUE;
		} else
			t = prev;
	}

#ifdef SVR4
	tm2.tm_isdst = -1;
	prev = mktime(&tm2);
#else
	prev = (timelocal(&tm2));
#endif
	return(prev);
}

static time_t
nextmonTofri(time_t t)
{
	struct tm *tm;
	time_t next;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);

	if (tm->tm_wday < 5)
		next = t + (int)daysec;
	else
		next = t + (int)daysec * (8 - tm->tm_wday);

	next = adjust_dst(t, next);
	return(next);
}

static time_t
prevmonTofri(time_t t)
{
	struct tm *tm;
	time_t prev;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);

	if (tm->tm_wday > 1)
		prev = t - (int)daysec;
	else
		prev = t - (int)daysec * (2 + tm->tm_wday);

	prev = adjust_dst(t, prev);
	return(prev);
}

static time_t
nextmonwedfri(time_t t)
{
	struct tm *tm;
	time_t next;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);

	if (tm->tm_wday == 5)
		next = t + (int)daysec * 3;
	else if (tm->tm_wday % 2 || tm->tm_wday == 6)
		next = t + (int)daysec * 2;
	else
		next = t + (int)daysec;

	next = adjust_dst(t, next);
	return(next);
}

static time_t
prevmonwedfri(time_t t)
{
	struct tm *tm;
	time_t prev;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);

	if (tm->tm_wday == 1)
		prev = t - (int)daysec * 3;
	else if (tm->tm_wday % 2 || tm->tm_wday == 0)
		prev = t - (int)daysec * 2;
	else
		prev = t - (int)daysec;

	prev = adjust_dst(t, prev);
	return(prev);
}

static time_t
nexttuethur(time_t t)
{
	struct tm *tm;
	time_t next;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);

	if (tm->tm_wday < 4) {
		if (tm->tm_wday % 2)
			next = t + (int)daysec;
		else
			next = t + (int)daysec * 2;
	} else
		next = t + (int)daysec * (9 - tm->tm_wday);

	next = adjust_dst(t, next);
	return(next);
}

static time_t
prevtuethur(time_t t)
{
	struct tm *tm;
	time_t prev;
	_Xltimeparams localtime_buf;

	tm = _XLocaltime(&t, localtime_buf);

	if (tm->tm_wday > 2) {
		if (tm->tm_wday % 2)
			prev = t - (int)daysec;
		else
			prev = t - (int)daysec * 2;
	} else
		prev = t - (int)daysec * (3 + tm->tm_wday);

	prev = adjust_dst(t, prev);
	return(prev);
}

/*
 * the 7-bit mask should be put in the last 7 bits of the int
 */
static time_t
nextdaysofweek(time_t t, int weekmask)
{
	unsigned int doublemask;
	struct tm *tm; 
	int i, ndays, daymask;
	time_t next;
	_Xltimeparams localtime_buf;

	doublemask = weekmask | (weekmask << 7);
	tm = _XLocaltime(&t, localtime_buf);
	daymask = weekdaymasks[tm->tm_wday] << 1;

	for (i = 0, ndays = 1; i < 7; i++) {
		if (daymask & doublemask)
			break;
		else {
			ndays++;
			doublemask >>= 1;
		}
	}

	next = t + (int)daysec * ndays;
	next = adjust_dst(t, next);
	return(next);
}

static time_t
prevdaysofweek(time_t t, int weekmask)
{
	unsigned int doublemask, daymask;
	struct tm *tm; 
	int i, ndays;
	time_t prev;
	_Xltimeparams localtime_buf;

	doublemask = weekmask | (weekmask << 7);
	tm = _XLocaltime(&t, localtime_buf);
	daymask = weekdaymasks[tm->tm_wday] << 6; 

	for (i = 0, ndays = 1; i < 7; i++) {
		if (daymask & doublemask)
			break;
		else {
			ndays++;
			doublemask <<= 1;
		}
	}

	prev = t - (int)daysec * ndays;
	prev = adjust_dst(t, prev);
	return(prev);
}

