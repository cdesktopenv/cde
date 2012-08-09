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
/* $TOG: iso8601.c /main/2 1997/12/29 10:46:50 bill $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "iso8601.h"

static void
set_timezone(char *tzname)
{
        static char tzenv[BUFSIZ];
 
        if (tzname==NULL)
		system("unset TZ\n");
        else {
                snprintf(tzenv, sizeof tzenv, "TZ=%s", tzname);
                (void) putenv(tzenv);
                tzset();
        }
}

static int
validate_iso8601(char *buf)
{
	/* validation rules:
         *             - sscanf returns # of matches, which must be 6.
         *             - crude range check on each numerical value scanned.
         *             - length of input is fixed: strlen("CCYYMMDDThhmmssZ")
         *             - last char must be Z, indicating UTC time
         */
	int	year, month, day, hour, min, sec;
	int	scan_ret=0;
static  char	tmp[] = "CCYYMMDDThhmmssZ";

	scan_ret=sscanf(buf, "%4d%2d%2dT%2d%2d%2dZ",
	    &year, &month, &day, &hour, &min, &sec);

	/* the rules:  if any fail, whole test fails so return */
	if (strlen(buf) != strlen(tmp)) return (-1);
	if (buf[strlen(buf)-1] != 'Z')	return (-1);
	if (scan_ret != 6)		return (-1);
	if ((year<1970) || (year>2038))	return (-1);
	if ((month<1) || (month>12))	return (-1);
	if ((day<1) || (day>31))	return (-1);
	if ((hour<0) || (hour>24))	return (-1);
	if ((min<0) || (min>59))	return (-1);
	if ((sec<0) || (sec>59))	return (-1);

	return (0);
}

/*
 * _csa_iso8601_to_tick - convert a standard date/time string to a tick
 *
 * Note 1:This function supports a limited subset of the iso8601 standard.
 *	  Only one of the variations described by the standard is
 *        supported, namely:
 *
 *		CCYYMMDDThhmmssZ
 *
 *        ...known in the standard as "Complete Representation, Basic Format"
 *        for calendar date and "Coordinated Universal Time (UTC), Basic Format"
 *        for time.
 *
 *	  This can carry all the information required for date+time by
 *	  the CDE 1.0 Calendar component.  More general support, if ever
 *	  needed (say for interoperability or finer granularity) can be
 *	  implemented inside this function without modifying its interface.
 *
 * Note 2:All output time information is in UTC, and all input 
 *        time information is assumed to be pre-converted to UTC.
 *
 *	  dac 19940728T224055Z  :-)
 */ 
int
_csa_iso8601_to_tick(char *buf, time_t *tick_out)
{
	int		year, month, day, hour, min, sec;
	struct tm	time_str;
	char		tz_orig[BUFSIZ];
	boolean_t	orig_tzset = B_FALSE;
	int 		scan_ret=0;

	scan_ret=sscanf(buf, "%4d%2d%2dT%2d%2d%2dZ",
	    &year, &month, &day, &hour, &min, &sec);

	if (validate_iso8601(buf) != 0)
		return(-1);

	time_str.tm_year	= year - 1900;
	time_str.tm_mon		= month - 1;
	time_str.tm_mday	= day;
	time_str.tm_hour	= hour;
	time_str.tm_min		= min;
	time_str.tm_sec		= sec;
	time_str.tm_isdst	= -1;

	if (getenv("TZ")) {
		strncpy(tz_orig, getenv("TZ"), sizeof(tz_orig));
		tz_orig[sizeof(tz_orig)-1] = '\0';
		orig_tzset = B_TRUE;
	}

#ifdef __osf__
	set_timezone("GMT0");
#else
	set_timezone("GMT");
#endif

	*tick_out = mktime(&time_str);

	if (orig_tzset == B_TRUE)
		set_timezone(tz_orig);
	else
		set_timezone(NULL);

	if (*tick_out != (long)-1)
		return(0);
	else
		return(-1);
}

/*
 * _csa_tick_to_iso8601 - convert from tick to iso8601 time string
 *
 * Note 1: Similar comments to the above.  This function complements
 *         _csa_iso8601_to_tick, providing bi-directional conversion.
 *
 * Note 2: All input and output time information is UTC.
 */
int
_csa_tick_to_iso8601(time_t tick, char *buf_out)
{
	struct tm	*time_str;
	time_t		tk=tick;
	char 		tz_orig[BUFSIZ];
	boolean_t	orig_tzset = B_FALSE;

	/* tick must be +ve to be valid */
	if (tick < 0) {
	   return(-1);
	}

	if (getenv("TZ")) {
		strncpy(tz_orig, getenv("TZ"), sizeof(tz_orig));
		tz_orig[sizeof(tz_orig)-1] = '\0';
		orig_tzset = B_TRUE;
	}

#ifdef __osf__
	set_timezone("GMT0");
#else
	set_timezone("GMT");
#endif

	time_str = localtime(&tk);

	if (orig_tzset == B_TRUE)
		set_timezone(tz_orig);
	else
		set_timezone(NULL);

	/* format string forces fixed width (zero-padded) fields */
	sprintf(buf_out, "%04d%02d%02dT%02d%02d%02dZ",
		time_str->tm_year + 1900,
		time_str->tm_mon + 1,
		time_str->tm_mday,
		time_str->tm_hour,
		time_str->tm_min,
		time_str->tm_sec);

	return (0);
}

/*
 * Convert iso8601 date time range to a start tick and an end tick
 *
 * iso8601 range is:
 *                   <start> "/" <end>
 *
 * start and end are iso8601 strings in CCYYMMDDThhmmssZ format.
 */

int
_csa_iso8601_to_range(char *buf, time_t *start, time_t *end)
{
    int nchars;
    char tmpstr[BUFSIZ];
    char *p;

    if ((p = strchr(buf, '/')) == NULL) {
        return (-1);
    }

    nchars=(p-buf);
    strncpy(tmpstr, buf, (size_t)nchars);
    tmpstr[nchars]='\0';

    if (_csa_iso8601_to_tick(tmpstr, start) != 0) {
        return (-1);
    }

    p++;
    if (_csa_iso8601_to_tick(p, end) != 0) {
        return (-1);
    }

    if (end < start)
	return (-1);
    else
	return(0);
}

/*
 * Convert time range specified as start/end ticks to iso8601 format
 *
 * iso8601 result is:
 *                   <start> "/" <end>
 *
 * start and end are iso8601 strings in CCYYMMDDThhmmssZ format.
 */
int
_csa_range_to_iso8601(time_t start, time_t end, char *buf)
{
    char tmpstr1[BUFSIZ], tmpstr2[BUFSIZ];

    /* validate: ticks must be +ve, and end can't preceed start */
    if ((start < 0) || (end < 0) || (end < start)) {
        return(-1);
    }

    if (_csa_tick_to_iso8601(start, tmpstr1) != 0) {
        return (-1);
    }
    if (_csa_tick_to_iso8601(end, tmpstr2) != 0) {
        return (-1);
    }

    if (sprintf(buf, "%s/%s", tmpstr1, tmpstr2) < 0) {
        return (-1);
    }
    else
        return(0);
}

static int
not_sign(char c)
{
   if ((c=='+') || (c=='-'))
      return (0);
   else
      return (1);
}

/*
 * This converts from a string representation of a quantity of time,
 * to (signed) integer number of * seconds.
 *   The first character (byte) must be a '+' or * a '-', indicating
 * the sense of the period.  This can be used however you like - it's
 * just a way of carrying round the sign, while keeping the main part
 * of the string in ISO 8601.
 *   The string must be in the format described by ISO 8601, clause
 * 5.5.1 (b), with the added restriction that only seconds may be specified.
 * format: [+/-]PTnS
 */
int
_csa_iso8601_to_duration(char *buf, time_t *sec)
{
   /* buf must begin with '+' or '-', then 'P', end with 'S' */
   char	sign, *ptr, *ptr2, *numptr;
   int	num=0;

   ptr2 = ptr = buf;
   sign = *ptr++;

   if (not_sign(sign)) {
	if (*ptr2++ != 'P' || *ptr2++ != 'T') {
		return (-1);
	}
   } else if (not_sign(sign) || *ptr++ != 'P' || *ptr++ != 'T') {
	return (-1);
   }

   if (not_sign(sign))
	ptr = ptr2;

   numptr = ptr;
   while (*ptr >= '0' && *ptr <= '9') ptr++;

   if (numptr == ptr || !(*ptr && *ptr++ == 'S' && *ptr == NULL))
	return (-1);
   else {
	num = atoi(numptr);
	*sec = (sign == '-') ? -num : num;
	return (0);
   }
}

/*
 * This converts from a (signed) integer number of seconds to a string
 * representation.  The string format is a sign character followed by
 * an IS0 8601 string as described above for _csa_iso8601_to_duration.
 */
int
_csa_duration_to_iso8601(time_t sec, char *buf)
{
   sprintf(buf, "%cPT%dS", (sec < 0) ? '-': '+', abs(sec));
   return(0);
}

