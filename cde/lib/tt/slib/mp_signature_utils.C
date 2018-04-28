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
//%%  $XConsortium: mp_signature_utils.C /main/3 1995/10/23 12:01:47 rswiston $ 			 				
/*
 *
 * mp_signature_utils.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "mp_signature.h"

_Tt_sigs_by_op::
_Tt_sigs_by_op()
{
}

_Tt_sigs_by_op::
~_Tt_sigs_by_op()
{
}

_Tt_sigs_by_op::
_Tt_sigs_by_op(_Tt_string o)
{
	_op = o;
}

_Tt_string
_tt_sigs_by_op_op(_Tt_object_ptr &o)
{
	return(((_Tt_sigs_by_op *)o.c_pointer())->op());
}

implement_list_of(_Tt_signature)
implement_list_of(_Tt_sigs_by_op)
implement_table_of(_Tt_sigs_by_op)
