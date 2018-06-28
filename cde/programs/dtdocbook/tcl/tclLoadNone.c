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
/* $XConsortium: tclLoadNone.c /main/2 1996/08/08 14:45:21 cde-hp $ */
/* 
 * tclLoadNone.c --
 *
 *	This procedure provides a version of the TclLoadFile for use
 *	in systems that don't support dynamic loading; it just returns
 *	an error.
 *
 * Copyright (c) 1995-1996 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * SCCS: @(#) tclLoadNone.c 1.5 96/02/15 11:43:01
 */

#include "tclInt.h"

/*
 *----------------------------------------------------------------------
 *
 * TclLoadFile --
 *
 *	This procedure is called to carry out dynamic loading of binary
 *	code;  it is intended for use only on systems that don't support
 *	dynamic loading (it returns an error).
 *
 * Results:
 *	The result is TCL_ERROR, and an error message is left in
 *	interp->result.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclLoadFile(
    Tcl_Interp *interp,		/* Used for error reporting. */
    char *fileName,		/* Name of the file containing the desired
				 * code. */
    char *sym1, char *sym2,	/* Names of two procedures to look up in
				 * the file's symbol table. */
    Tcl_PackageInitProc **proc1Ptr, Tcl_PackageInitProc **proc2Ptr
				/* Where to return the addresses corresponding
				 * to sym1 and sym2. */
)
{
    interp->result =
	    "dynamic loading is not currently available on this system";
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TclGuessPackageName --
 *
 *	If the "load" command is invoked without providing a package
 *	name, this procedure is invoked to try to figure it out.
 *
 * Results:
 *	Always returns 0 to indicate that we couldn't figure out a
 *	package name;  generic code will then try to guess the package
 *	from the file name.  A return value of 1 would have meant that
 *	we figured out the package name and put it in bufPtr.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclGuessPackageName(
    char *fileName,		/* Name of file containing package (already
				 * translated to local form if needed). */
    Tcl_DString *bufPtr		/* Initialized empty dstring.  Append
				 * package name to this if possible. */
)
{
    return 0;
}
