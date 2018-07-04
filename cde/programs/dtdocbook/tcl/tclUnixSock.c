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
/* $XConsortium: tclUnixSock.c /main/2 1996/08/08 14:47:01 cde-hp $ */
/* 
 * tclUnixSock.c --
 *
 *	This file contains Unix-specific socket related code.
 *
 * Copyright (c) 1995 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * SCCS: @(#) tclUnixSock.c 1.5 96/04/04 15:28:39
 */

#include "tcl.h"
#include "tclPort.h"

/*
 * The following variable holds the network name of this host.
 */

#ifndef SYS_NMLN
#   define SYS_NMLN 100
#endif

static char hostname[SYS_NMLN + 1];
static int  hostnameInited = 0;

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetHostName --
 *
 *	Get the network name for this machine, in a system dependent way.
 *
 * Results:
 *	A string containing the network name for this machine.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_GetHostName(void)
{
    struct utsname u;
    struct hostent *hp;

    if (hostnameInited) {
        return hostname;
    }
    
    if (uname(&u) > -1) {
        hp = gethostbyname(u.nodename);
        if (hp != NULL) {
            snprintf(hostname, sizeof(hostname), "%s", hp->h_name);
        } else {
            snprintf(hostname, sizeof(hostname), "%s", u.nodename);
        }
        hostnameInited = 1;
        return hostname;
    }
    return (char *) NULL;
}
