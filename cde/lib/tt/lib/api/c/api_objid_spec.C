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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: api_objid_spec.C /main/3 1995/10/23 09:53:54 rswiston $ 			 				
/* -*-C++-*-
 *
 * api_objid_spec.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 */

#include "api/c/api_objid_spec.h"


_Tt_objid_spec ::
_Tt_objid_spec()
{
	onDiskFlag = FALSE;
}	// end -_Tt_objid_spec()-

_Tt_objid_spec ::
_Tt_objid_spec(_Tt_string objid)

	: _Tt_db_object(objid)
{
	onDiskFlag = TRUE;
}	// end -_Tt_objid_spec()-


_Tt_objid_spec ::
~_Tt_objid_spec()
{
	// nothing special
}	// end -~_Tt_objid_spec()-


// onDiskFlag - Rules for flag value are:
//
// 1) Set sync flag to TRUE by default, if the
//    object is already on disk.  If it is a
//    brand new object that has never been written
//    to disk, set the flag to FALSE.
// 2) If a memory only set is done, set sync
//    flag to FALSE.
// 3) If a write to disk is done, set sync flag
//    to TRUE.
// 4) If sync flag is TRUE, read from disk by
//    refreshing memory version and calling read
//    member functions.
// 5) If sync flag is FALSE, read from memory.
//

void _Tt_objid_spec ::
setOnDiskFlag(bool_t flag)
{
	onDiskFlag = flag;
}	// end -setOnDiskFlag()-


bool_t _Tt_objid_spec ::
getOnDiskFlag()
{
	return onDiskFlag;
}	// end -getOnDiskFlag()-
