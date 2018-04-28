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
//%%  $XConsortium: mp_ce_attrs.C /main/3 1995/10/23 11:49:31 rswiston $ 			 				
/* @(#)mp_ce_attrs.C	1.3 @(#)
 *
 * mp_ce_attrs.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "mp_ce_attrs.h"


// 
// Given an the _Tt_ce_attr enum value x, this method should return the
// string that corresponds to it in a Classing Engine database. Changing
// the strings returned by this method should be done with care as it may
// disable binary compatibility with older Classing Engine databases.
// 
const char *
_tt_ce_attr_string(_Tt_ce_attr x)
{
	switch (x) {
	      case _TYPE_NAME:
		return "TYPE_NAME";
	      case _TT_OP:
		return "TT_OP";
	      case _TT_ARG:
		return "TT_ARG";
	      case _TT_SCOPE:
		return "TT_SCOPE";
	      case _TT_MSET_SCOPE:
		return "TT_MSET_SCOPE";
	      case _TT_DISPOSITION:
		return "TT_DISPOSITION";
	      case _TT_MSET_DISPOSITION:
		return "TT_MSET_DISPOSITION";
	      case _TT_MSET_OPNUM:
		return "TT_MSET_OPNUM";
	      case _TT_OPNUM:
		return "TT_OPNUM";
	      case _TT_MSET_HANDLER_PTYPE:
		return "TT_MSET_HANDLER_PTYPE";
	      case _TT_HANDLER_PTYPE:
		return "TT_HANDLER_PTYPE";
	      case _TT_MSET_OTYPE:
		return "TT_MSET_OTYPE";
	      case _TT_OTYPE:
		return "TT_OTYPE";
	      case _TT_PARENT:
		return "TT_PARENT";
	      case _TT_OUT:
		return "TT_OUT";
	      case _TT_IN:
		return "TT_IN";
	      case _TT_INOUT:
		return "TT_INOUT";
	      case _TT_SESSION:
		return "TT_SESSION";
	      case _TT_FILE:
		return "TT_FILE";
	      case _TT_BOTH:
		return "TT_BOTH";
	      case _TT_FILE_IN_SESSION:
		return "TT_FILE_IN_SESSION";
	      case _TT_DISCARD:
		return "TT_DISCARD";
	      case _TT_QUEUE:
		return "TT_QUEUE";
	      case _TT_START:
		return "TT_START";
	      case _TT_CATEGORY:
		return "TT_CATEGORY";
	      case _TT_OBSERVE:
		return "TT_OBSERVE";
	      case _TT_HANDLE:
		return "TT_HANDLE";
	      case _TT_CLASS:
		return "TT_CLASS";
	      case _TT_REQUEST:
		return "TT_REQUEST";
	      case _TT_NOTICE:
		return "TT_NOTICE";
	      case _TT_TOOLTALK_TYPE:
		return "SUN_TOOLTALK_TYPE";
	      case _TT_TOOLTALK_PTYPE:
		return "SUN_TOOLTALK_PTYPE";
	      case _TT_TOOLTALK_OTYPE:
		return "SUN_TOOLTALK_OTYPE";
	      case _TT_TOOLTALK_SIGNATURE:
		return "SUN_TOOLTALK_SIGNATURE";
	      case _TT_CE_ATTR_LAST:
	      default:
		return "! _Tt_ce_attr";
	}
}
