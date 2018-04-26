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
//%%  $TOG: mp_xdr_functions.C /main/4 1999/08/30 11:00:02 mgreess $ 			 				
/*
 *
 * @(#)mp_xdr_functions.C	1.14 93/07/30
 *
 * Tool Talk Message Passer (MP) - mp_xdr_functions.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include <string.h>
#include "mp/mp_xdr_functions.h"

/*
 * XDR function to de/encode args when TT_AUTH_ICEAUTH authorization is
 * in effect.
 */
bool_t
tt_xdr_auth_iceauth_args(XDR *xdrs, _Tt_auth_iceauth_args *args)
{
        return(args->auth_cookie.xdr(xdrs) && args->inproc(xdrs, args->inargs));
}

/* 
 * XDR encodes/decodes a null terminated string.
 */
bool_t
tt_xdr_bstring(XDR *xdrs, char **sp)
{
	int		length;

	if (xdrs->x_op == XDR_ENCODE) {
 		length = ((sp && *sp) ? strlen(*sp) : 0);
	}
	if (!xdr_int(xdrs, &length)) return 0;
	if (0==length) {
		if (xdrs->x_op == XDR_DECODE) {
			*sp = (char *)0;
		}
		return(1);
	}
	if (xdrs->x_op == XDR_DECODE) {
		*sp = (char *)malloc(length+1);
	}
	if (!xdr_string(xdrs, sp, length)) return 0;
	if (xdrs->x_op == XDR_FREE) {
		free((MALLOCTYPE *) *sp);
	}
	return(1);
}


/* 
 * XDR function to encode args to declare_ptype operation.
 */
bool_t
tt_xdr_declare_ptype_args(XDR *xdrs, _Tt_declare_ptype_args *args)
{
	return(args->procid.xdr(xdrs) && args->ptid.xdr(xdrs));
}


/* 
 * XDR function to encode args to join_file operation.
 */
bool_t
tt_xdr_file_join_args(XDR *xdrs, _Tt_file_join_args *args)
{
	return(args->procid.xdr(xdrs) && args->path.xdr(xdrs));
}


/* 
 * XDR function to encode args identifying a <context,procID> pair
 */
bool_t
tt_xdr_context_join_args(XDR *xdrs, _Tt_context_join_args *args)
{
	return args->procid.xdr(xdrs) && args->context.xdr(xdrs);
}


/* 
 * XDR encodes/decodes a _Tt_fd_args structure.
 */
bool_t
tt_xdr_fd_args(XDR *xdrs, _Tt_fd_args *args)
{
	return(args->procid.xdr(xdrs)
	       && xdr_int(xdrs, &args->fd)
	       && args->start_token.xdr(xdrs));
}


bool_t
tt_xdr_add_pattern_args(XDR *xdrs,_Tt_add_pattern_args *args)
{
	return(args->procid.xdr(xdrs) && args->pattern.xdr(xdrs));
}


bool_t
tt_xdr_del_pattern_args(XDR *xdrs,_Tt_del_pattern_args *args)
{
	return(args->procid.xdr(xdrs) && args->pattern_id.xdr(xdrs));
}


/* 
 * XDR function to encode/decode property values
 */
bool_t
tt_xdr_prop_args(XDR *xdrs, _Tt_prop_args *args)
{
	return(args->prop.xdr(xdrs) && args->value.xdr(xdrs) &&
	       xdr_int(xdrs, &args->num));
}

/* 
 * XDR function to encode/decode otype query values
 */
bool_t
tt_xdr_otype_args(XDR *xdrs, _Tt_otype_args *args)
{
	return(args->derived_otid.xdr(xdrs) &&
	       args->base_otid.xdr(xdrs) &&
	       xdr_int(xdrs, &args->num) &&
	       xdr_int(xdrs, &args->num2));
}

/* 
 * XDR function to encode/decode the result of a session property request
 * or a otype query request.
 */
bool_t
tt_xdr_rpc_result(XDR *xdrs, _Tt_rpc_result *args)
{
	return(xdr_int(xdrs, (int *)&args->status) && args->str_val.xdr(xdrs)
	       && xdr_int(xdrs, &args->int_val));
}


bool_t
tt_xdr_update_args(XDR *xdrs, _Tt_update_args *args)
{
	return( xdr_int(xdrs, (int *)&args->newstate) &&
	       args->message.xdr(xdrs));
}


bool_t
tt_xdr_next_message_args(XDR *xdrs, _Tt_next_message_args *args)
{
	return(args->msgs.xdr(xdrs) &&
	       xdr_int(xdrs, (int *)&args->clear_signal));
}
	
bool_t
tt_xdr_dispatch_reply_args(XDR *xdrs, _Tt_dispatch_reply_args *args)
{
	return(xdr_int(xdrs, (int *)&args->status) &&
	       args->qmsg_info.xdr(xdrs));
}

bool_t
tt_xdr_load_types_args(XDR *xdrs,_Tt_load_types_args *args)
{
	return(args->xdrtypes.xdr(xdrs));
}
