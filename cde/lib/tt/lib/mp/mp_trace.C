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
//%%  $XConsortium: mp_trace.C /main/4 1995/11/21 19:25:54 cde-sun $ 			 				
/*
 * @(#)mp_trace.cc	1.2 93/08/15
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include "mp/mp_trace.h"

_Tt_msg_trace::_Tt_msg_trace(
	_Tt_message &msg,
	_Tt_dispatch_reason reason
)
{
	entry( msg, reason );
}

_Tt_msg_trace::_Tt_msg_trace(
	_Tt_message &msg,
	const _Tt_procid  &recipient
)
{
	entry( msg, recipient );
}

_Tt_msg_trace::_Tt_msg_trace(
	_Tt_message &msg,
	Tt_state           old_state
)
{
	entry( msg, old_state );
}

_Tt_msg_trace::~_Tt_msg_trace()
{
	exitq();
}
