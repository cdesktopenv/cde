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
//%%  $XConsortium: mp_c_msg_context.C /main/3 1995/10/23 10:20:54 rswiston $ 			 				
/*
 *
 * @(#)mp_c_msg_context.C	1.4 30 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_c_msg_context.cc
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * _Tt_c_msg_context knows the client side of the context RPC interface.
 */

#include <mp/mp_arg.h>
#include <mp/mp_c_msg_context.h>
#include <mp/mp_rpc_interface.h>
#include <mp/mp_xdr_functions.h>

_Tt_c_msg_context::_Tt_c_msg_context()
{
}

_Tt_c_msg_context::~_Tt_c_msg_context()
{
}

Tt_status
_Tt_c_msg_context::c_join(
	_Tt_session &session,
	_Tt_procid_ptr &procID
)
{
	Tt_status		status;
	Tt_status		rstatus;
	_Tt_context_join_args	args;

	args.procid = procID;
	args.context = this;
	rstatus = session.call( TT_RPC_JOIN_CONTEXT,
			        (xdrproc_t)tt_xdr_context_join_args,
			        (char *)&args,
			        (xdrproc_t)xdr_int,
			        (char *)&status );
	return (rstatus == TT_OK) ? status : rstatus;
}

Tt_status
_Tt_c_msg_context::c_quit(
	_Tt_session &session,
	_Tt_procid_ptr &procID
)
{
	Tt_status		status;
	Tt_status		rstatus;
	_Tt_context_join_args	args;

	args.procid = procID;
	args.context = this;
	rstatus = session.call( TT_RPC_QUIT_CONTEXT,
			        (xdrproc_t)tt_xdr_context_join_args,
			        (char *)&args,
			        (xdrproc_t)xdr_int,
			        (char *)&status );
	return (rstatus == TT_OK) ? status : rstatus;
}
