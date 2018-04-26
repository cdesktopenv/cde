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
 *+SNOTICE
 *
 *
 *	$XConsortium: DynamicLib.C /main/4 1996/04/21 19:48:04 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

#include "DynamicLib.hh"
#include "DlDynamicLib.hh"
#include "NotDynamic.hh"

// The purpose of this function is to map the virtual DynamicLib object
// into a platform specific implementation at compile time. This is
// a fairly ugly process that requires lots of ifdef's based on the
// particular implementation of shared libraries.
//
// Adding a new dynamic library implementation for a platform involves
// two steps:
//
//     	1) Create a new implementation of the DynamicLib object (see
//	   DlDynamicLib.C for an example).
//
//	2) Adding the appropriate ifdef construction step to this function.
//

DynamicLib *
CreatePlatformDl(const char * path)
{
#if defined(DL_DYNAMIC_LIBS)
    DynamicLib * dl = new DlDynamicLib(path);
#endif

#if defined(DL_NOT_DYNAMIC)
    DynamicLib * dl = new NotDynamic(path);
#endif

    // WARNING WARNING WARNING WARNING WARNING WARNING WARNING
    // *******************************************************
    // If you get a compile error because "dl" is not defined,
    // then you have not specified your platforms dynamic library
    // implementation. You MUST do this for this code to work!
    // *******************************************************
    // WARNING WARNING WARNING WARNING WARNING WARNING WARNING
    return(dl);
}

#ifndef CAN_INLINE_VIRTUALS
DynamicLib::~DynamicLib( void )
{
}
#endif /* ! CAN_INLINE_VIRTUALS */
