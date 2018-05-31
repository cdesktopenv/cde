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
//%%  $XConsortium: mp_observer.C /main/3 1995/10/23 11:50:12 rswiston $ 			 				
/*
 *
 * mp_observer.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "mp_observer.h"


// 
// Methods for _Tt_observer objects which just serve as records that
// hold the message fields in a _Tt_message object that can be
// different for static observers. See _Tt_s_message documentation for
// more details.
//


_Tt_observer::
_Tt_observer()
{
	_ptid = (char *)0;
	_reliability = TT_DISCARD;
	_opnum = -1;

	_scope = TT_SCOPE_NONE;
	_state = TT_STATE_LAST; // BUG This class member is not set anywhere, but a getter function exists
}


_Tt_observer::
_Tt_observer(_Tt_string ptid, int opnum,
	     Tt_disposition reliability, Tt_scope s)
{
	_ptid = ptid;
	_reliability = reliability;
	_opnum = opnum;
	_scope = s;

	_state = TT_STATE_LAST; // BUG This class member is not set anywhere, but a getter function exists
}


_Tt_observer::
~_Tt_observer()
{
}





