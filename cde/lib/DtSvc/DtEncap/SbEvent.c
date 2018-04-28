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
 * File:         SbEvent.c $XConsortium: SbEvent.c /main/3 1995/10/26 15:35:08 rswiston $
 * Language:     C
 *
 * (c) Copyright 1990, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#define __need_fd_set

#include <bms/sbport.h>	 /* NOTE: sbport.h must be the first include. */
#include <errno.h>

typedef char Boolean;
#include <bms/XeUserMsg.h>

#ifdef _AIX
   /*
    * The AIX compiler gives an internal error if it includes
    * <bms/SbEvent.h>.  The required typedefs were removed from
    * this header and placed within this file.
    */
typedef void (*SbInputCallbackProc) ();
typedef unsigned long SbInputId;
#else
#include <bms/SbEvent.h> 
#endif

#include <sys/types.h>   /* for fd_set, FD_SET macros, et. al. */

SbInputId (*SbAddInput_hookfn) (
	int fd, 
	SbInputCallbackProc proc, 
	void* data) = NULL;

SbInputId (*SbAddException_hookfn) (
	int fd, 
	SbInputCallbackProc proc, 
	void* data) = NULL;

void  (*SbRemoveInput_hookfn) (
	SbInputId id) = NULL;

void  (*SbRemoveException_hookfn) (
	SbInputId id) = NULL;

void  (*SbMainLoopUntil_hookfn) (
	Boolean *flag) = NULL;

void  (*SbBreakMainLoop_hookfn) (
	void) = NULL;

/* spcd calls this guy */

void XeCall_SbMainLoopUntil(Boolean *flag)
{
 (*SbMainLoopUntil_hookfn)(flag);
}

