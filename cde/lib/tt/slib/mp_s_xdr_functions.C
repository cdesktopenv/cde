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
//%%  $XConsortium: mp_s_xdr_functions.C /main/3 1995/10/23 12:00:33 rswiston $ 			 				
/*
 *
 * mp_s_xdr_functions.cc
 *
 * Server-side only xdr functions.
 *
 * In order that the server has _s_ versions of the routines,
 * we use slightly different xdr routines for decoding!  This
 * is a violation of XDR dogma \(but not really, since the XDR ing
 * is only done in the parent _Tt_xxx class; the only difference
 * is the constructor that gets called.\)
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */
#include "mp_s_xdr_functions.h"

bool_t
tt_s_xdr_add_pattern_args(XDR *xdrs,_Tt_s_add_pattern_args *args)
{
	return(args->procid.xdr(xdrs) && args->pattern.xdr(xdrs));
}


bool_t
tt_s_xdr_del_pattern_args(XDR *xdrs,_Tt_s_del_pattern_args *args)
{
	return(args->procid.xdr(xdrs) && args->pattern_id.xdr(xdrs));
}


bool_t
tt_s_xdr_update_args(XDR *xdrs, _Tt_s_update_args *args)
{
	return( xdr_int(xdrs, (int *)&args->newstate) &&
	       args->message.xdr(xdrs));
}



