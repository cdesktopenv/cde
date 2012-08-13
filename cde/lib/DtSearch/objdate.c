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
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: is_objdatestr
 *		is_objdatetm
 *		objdate2fzkstr
 *		objdate2tm
 *		objdate_in_range
 *		tm2objdate
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1994,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*************************** OBJDATE.C *************************
 * $XConsortium: objdate.c /main/8 1996/11/21 19:51:44 drk $
 * November 1994.
 * Utilites for generic manipulation of austext objdates.
 * Most of these functions were originally in msgutil.c
 *
 * $Log$
 * Revision 2.5  1996/03/05  18:00:36  miker
 * Replaced hardcoded strings with refs to NULLDATESTR.
 *
 * Revision 2.4  1996/02/13  16:44:40  miker
 * Allow \n to terminate null date string in is_objdatestr.
 *
 * Revision 2.3  1995/10/25  16:38:10  miker
 * Added prolog.
 *
 * Revision 2.2  1995/10/02  20:37:30  miker
 * Cosmetic cleanup only.
 *
 * Revision 2.1  1995/09/22  21:25:35  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/05  18:24:09  miker
 * Remove refs to usrblk so objdate can be used in offline programs.
 * Name changes for DtSearch.
 */

#include "SearchP.h"
#include <stdlib.h>
#define X_INCLUDE_STRING_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>

#define PROGNAME	"OBJDATE"

/************************************************/
/*						*/
/*		  is_objdatestr			*/
/*						*/
/************************************************/
/* Converts OBJDATESTR formatted string as found in .fzk files
 * to DtSrObjdate long integer if string is valid.
 * Returns TRUE if passed string is correctly formatted
 * and conversion successful.  Returns FALSE and
 * does not alter passed objdate if string is not valid.
 * String format is: "yy/mm/dd~hh:mm[\n]" (see OBJDATESTR in SearchP.h).
 * The slashes and tilde are mandatory, the final \n is optional.
 * Each field maps to an objdate bitfield; bitfields map
 * to struct tm fields (see fuzzy.h).
 * Can be used merely to test for valid string format by
 * passing NULL for objdate pointer.
 */
int	is_objdatestr (char *string, DtSrObjdate *objdptr)
{
    static char     parsebuf[24];
    int             i;
    char           *token;
    DtSrObjdate     myobjdate = 0L;
    _Xstrtokparams  strtok_buf;

    /* Test for "null" objdate (which is valid) */
    if (strncmp (string, NULLDATESTR, 9) == 0) {
	if (string[9] == 0 || string[9] == '\n') {
	    if (objdptr)
		*objdptr = 0L;
	    return TRUE;
	}
    }

    strncpy (parsebuf, string, sizeof (parsebuf));
    parsebuf[sizeof (parsebuf) - 1] = '\0';

    if ((token = _XStrtok(parsebuf, "/", strtok_buf)) == NULL)
	return FALSE;
    i = atoi (token);
    if (i < 1 || i > 4095)	/* yy */
	return FALSE;
    else
	myobjdate |= (i << 20);

    if ((token = _XStrtok(NULL, "/", strtok_buf)) == NULL)
	return FALSE;
    i = atoi (token);
    if (i < 1 || i > 12)	/* mm */
	return FALSE;
    else
	myobjdate |= (--i << 16);

    if ((token = _XStrtok(NULL, "~", strtok_buf)) == NULL)
	return FALSE;
    i = atoi (token);
    if (i < 1 || i > 31)	/* dd */
	return FALSE;
    else
	myobjdate |= (i << 11);

    if ((token = _XStrtok(NULL, ":", strtok_buf)) == NULL)
	return FALSE;
    i = atoi (token);
    if (i < 0 || i > 23)	/* hh */
	return FALSE;
    else
	myobjdate |= (i << 6);

    if ((token = _XStrtok(NULL, "\n", strtok_buf)) == NULL)
	return FALSE;
    i = atoi (token);
    if (i < 0 || i > 59)	/* mm */
	return FALSE;
    else
	myobjdate |= i;

    if (objdptr)
	*objdptr = myobjdate;
    return TRUE;
}  /* is_objdatestr() */



/************************************************/
/*						*/
/*		   is_objdatetm			*/
/*						*/
/************************************************/
/* Returns TRUE if passed structure is correctly formatted
 * for conversion to DtSrObjdate variable, else returns FALSE.
 */
int	is_objdatetm (struct tm *objdatetm)
{
    if (objdatetm->tm_year < 0)
	return FALSE;
    if (objdatetm->tm_year > 4095)
	return FALSE;
    if (objdatetm->tm_mon < 0)
	return FALSE;
    if (objdatetm->tm_mon > 11)
	return FALSE;
    if (objdatetm->tm_mday < 1)
	return FALSE;
    if (objdatetm->tm_mday > 31)
	return FALSE;
    if (objdatetm->tm_hour < 0)
	return FALSE;
    if (objdatetm->tm_hour > 23)
	return FALSE;
    if (objdatetm->tm_min < 0)
	return FALSE;
    if (objdatetm->tm_min > 59)
	return FALSE;
    return TRUE;
}  /* is_objdatetm() */


/************************************************/
/*						*/
/*		    objdate2tm			*/
/*						*/
/************************************************/
/* Converts DtSrObjdate formatted long int and returns
 * pointer to static, standard unix tm structure.
 * Bitfields map to struct tm fields (see fuzzy.h).
 * The output tm structure is suitable for mktime(),
 * but may only work for some formats of strftime().
 */
struct tm      *objdate2tm (DtSrObjdate objdate)
{
    static struct tm	mytm;
    memset (&mytm, 0, sizeof (struct tm));
    mytm.tm_year = objdate >> 20;
    mytm.tm_mon = (objdate >> 16) & 0xf;
    mytm.tm_mday = (objdate >> 11) & 0x1f;
    mytm.tm_hour = (objdate >> 6) & 0x1f;
    mytm.tm_min = objdate & 0x3f;
    mytm.tm_isdst = -1;
    return &mytm;
}  /* objdate2tm() */


/************************************************/
/*						*/
/*		  objdate2fzkstr		*/
/*						*/
/************************************************/
/* Converts DtSrObjdate formatted long int and returns pointer
 * to static date string in .fzk format for debugging.
 */
char           *objdate2fzkstr (DtSrObjdate objdate)
{
    struct tm      *tmptr;
    static char     strbuf[36];
    if (objdate == 0L)
	return NULLDATESTR;
    tmptr = objdate2tm (objdate);
    sprintf (strbuf, OBJDATESTR,
	tmptr->tm_year, tmptr->tm_mon + 1,
	tmptr->tm_mday, tmptr->tm_hour, tmptr->tm_min);
    return strbuf;
}  /* objdate2fzkstr() */


/************************************************/
/*						*/
/*		    tm2objdate			*/
/*						*/
/************************************************/
/* Converts specific fields in a tm structure to
 * an DtSrObjdate formatted long int and returns it.
 * DtSrObjdate bitfields map to struct tm fields (see fuzzy.h).
 * Does not validate tm fields (use is_objdate() to confirm).
 */
DtSrObjdate	tm2objdate (struct tm *tmptr)
{
    return ((tmptr->tm_year << 20) |
	(tmptr->tm_mon << 16) |
	(tmptr->tm_mday << 11) |
	(tmptr->tm_hour << 6) |
	tmptr->tm_min);
}  /* tm2objdate() */


/************************************************/
/*						*/
/*		objdate_in_range		*/
/*						*/
/************************************************/
/* Returns TRUE if passed record objdate (presumably read from
 * an austext record) is within the objdate ranges specified
 * in usrblk.  Returns FALSE if record is in any way disqualified
 * from inclusion on a hitlist because of date.
 * Zero in any of the three dates is automatic "in range" qualification.
 */
int	objdate_in_range (
		DtSrObjdate	recdate,
		DtSrObjdate	date1,
		DtSrObjdate	date2)
{
    if (recdate == 0L)	/* Null dated record always qualifies */
	return TRUE;
    if (date1 != 0L && date1 > recdate)
	return FALSE;
    if (date2 != 0L && date2 < recdate)
	return FALSE;
    return TRUE;
}  /* objdate_in_range() */

/********************** OBJDATE.C *************************/
