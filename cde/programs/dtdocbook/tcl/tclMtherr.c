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
/* $XConsortium: tclMtherr.c /main/2 1996/08/08 14:45:38 cde-hp $ */
/* 
 * tclMatherr.c --
 *
 *	This function provides a default implementation of the
 *	"matherr" function, for SYS-V systems where it's needed.
 *
 * Copyright (c) 1993-1994 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * SCCS: @(#) tclMtherr.c 1.11 96/02/15 11:58:36
 */

#include "tclInt.h"
#include <math.h>

#ifndef TCL_GENERIC_ONLY
#include "tclPort.h"
#else
#define NO_ERRNO_H
#endif

#ifdef NO_ERRNO_H
extern int errno;			/* Use errno from tclExpr.c. */
#define EDOM 33
#define ERANGE 34
#endif

/*
 * The following variable is secretly shared with Tcl so we can
 * tell if expression evaluation is in progress.  If not, matherr
 * just emulates the default behavior, which includes printing
 * a message.
 */

extern int tcl_MathInProgress;

/*
 * The following definitions allow matherr to compile on systems
 * that don't really support it.  The compiled procedure is bogus,
 * but it will never be executed on these systems anyway.
 */

#ifndef NEED_MATHERR
struct exception {
    int type;
};
#define DOMAIN 0
#define SING 0
#endif

/*
 *----------------------------------------------------------------------
 *
 * matherr --
 *
 *	This procedure is invoked on Sys-V systems when certain
 *	errors occur in mathematical functions.  Type "man matherr"
 *	for more information on how this function works.
 *
 * Results:
 *	Returns 1 to indicate that we've handled the error
 *	locally.
 *
 * Side effects:
 *	Sets errno based on what's in xPtr.
 *
 *----------------------------------------------------------------------
 */

int
matherr(xPtr)
    struct exception *xPtr;	/* Describes error that occurred. */
{
    if (!tcl_MathInProgress) {
	return 0;
    }
    if ((xPtr->type == DOMAIN) || (xPtr->type == SING)) {
	errno = EDOM;
    } else {
	errno = ERANGE;
    }
    return 1;
}
