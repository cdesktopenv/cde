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
//%%  $XConsortium: mp_s_pat_context.C /main/3 1995/10/23 11:57:14 rswiston $ 			 				
/*
 *
 * @(#)mp_s_pat_context.C	1.6 29 Jul 1993
 *
 * Tool Talk Pattern Passer (MP) - mp_s_pat_context.cc
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * _Tt_s_pat_context knows server-side context matching.
 */

#include <mp/mp_arg.h>
#include <mp/mp_message.h>
#include "mp_s_pat_context.h"

_Tt_s_pat_context::
_Tt_s_pat_context()
{
}

_Tt_s_pat_context::
_Tt_s_pat_context(const _Tt_context &c) : _Tt_pat_context(c)
{
}

_Tt_s_pat_context::
~_Tt_s_pat_context()
{
}

int _Tt_s_pat_context::
matchVal(
	const _Tt_message &msg
)
{
	_Tt_msg_context_ptr msgCntxt = msg.context( slotName() );
	if (msgCntxt.is_null()) {
		return 0;
	}
	if (_values->count() == 0) {
		// No values means we are a wild card.
		return 1;
	}
	_Tt_arg_list_cursor argC( _values );
	while (argC.next()) {
		if (**argC == msgCntxt->value()) {
			return 2;
		}
	}
	return 0;
}
