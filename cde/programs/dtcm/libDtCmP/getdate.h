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
**  getdate.h
**
**  static char sccsid[] = "@(#)getdate.h 1.6 94/11/07 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: getdate.h /main/3 1995/11/03 10:37:54 rswiston $
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

#ifndef _GETDATE_H
#define _GETDATE_H

#if defined(USG) || defined(__OpenBSD__)
struct timeb
{
        time_t  time;
        unsigned short millitm;
        short   timezone;
        short   dstflag;
};
#else
#include <sys/timeb.h>
#endif
#include <time.h>
#include "ansi_c.h"

#define BOT_YEAR	1970
#define EOT_YEAR	2037
#define DATE_PARSE	-1	/* date simply did not parse */
#define DATE_BBOT	-2	/* date was before beginning of time */
#define	DATE_AEOT	-3	/* date was after end of time */
#define DATE_BMONTH	-4	/* date had a bad month number */
#define DATE_BDAY	-5	/* date had a bad day number */
#define DATE_BMIN	-6	/* date had a bad minute number */
#define DATE_BHOUR	-7	/* date had a bad hour number */
#define DATE_CONV	-8	/* date converted poorly for am/pm/24hr */

extern time_t		dateconv		P((int, int, int, int, int, int, int, int, int));
extern time_t		dayconv			P((int, int, time_t));
extern time_t		timeconv		P((int, int, int, int));
extern time_t		monthadd		P((time_t, time_t));
extern time_t		daylcorr		P((time_t, time_t));
extern time_t		cm_getdate		P((char*, struct timeb *));


#endif
