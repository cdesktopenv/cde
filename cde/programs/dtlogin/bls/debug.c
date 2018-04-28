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
/*
 * xdm - display manager daemon
 *
 * $XConsortium: debug.c /main/3 1995/07/14 13:23:25 drk $
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

 /***************
    debug.c
 ****************/

#ifndef NDEBUG
/* don't compile anything in this file unless this is pre-release code */
#include <stdio.h> 
#include <signal.h>
#include "../vg.h"
#include "bls.h"

# include <stdarg.h>
# define Va_start(a,b) va_start(a,b)

char *DisplayName=NULL;


/****************************************************************************
 *
 *  Debug
 *
 *  Write a debug message to stderr
 *
 ****************************************************************************/

static int  DoName=TRUE;
static int  debugLevel=0;


int
BLS_ToggleDebug( int arg)
{
	debugLevel = !debugLevel;
	(void) signal(SIGHUP,BLS_ToggleDebug);
}



void 
Debug( char *fmt, ...)
{
	static int sentinel = 0;
	static char *debugLog;

    va_list  args;

    Va_start(args,fmt);



    if ( !sentinel ) {
    /* 
     * open up an error log for dtgreet
     */
	if ((debugLog = getenv("VG_DEBUG")) == 0)
		debugLog = "/usr/lib/X11/dt/Dtlogin/dtgreet.log";

	if ( !freopen(debugLog,"a",stderr)) {
		perror("Debug:");
	}
	DisplayName=dpyinfo.name;
	sentinel = 1;
    }

    if (debugLevel > 0)
    {
	if ( strlen(DisplayName) > 0 && DoName)
	    fprintf(stderr, "(%s) ", DisplayName);

	vfprintf (stderr,fmt, args);
	fflush (stderr);

	/*
	 * don't prepend the display name next time if this debug message
	 * does not contain a "new line" character...
	 */

	if ( strchr(fmt,'\n') == NULL )
	    DoName=FALSE;
	else
	    DoName=TRUE;
	    
    }

    va_end(args);
}

#else

/*
 * Debug stub for product purposes
 */

void 
Debug( )
{ }

#endif	/* NDEBUG */
