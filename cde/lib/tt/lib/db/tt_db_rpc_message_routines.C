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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: tt_db_rpc_message_routines.C /main/3 1995/10/23 10:05:09 rswiston $ 			 				
/* @(#)tt_db_rpc_message_routines.C	1.10 94/11/17
 * tt_db_rpc_message_routines.cc - Defines routines for converting TT 
 *                                 message classes to RPC arguments.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#include "mp/mp_message.h"
#include "util/tt_string.h"
#include "util/tt_xdr_utils.h"
#include "db/tt_db_rpc_message_routines.h"

// If this file is being compiled with -g, then DEFINE_NEW_AND_DELETE
// must be defined in order to prevent a dependency on the libC library.
// The dependency is due to the fact that cfront generates an extern
// reference to vec_new and vec_delete if it sees any access to arrays
// of calsses or structures.  In the case of this file, tt_message_list
// contains a pointer to a structure that causes the references to be
// generated.
#ifdef DEFINE_NEW_AND_DELETE
extern "C" {
void *__vec_new (void *, int , int , void *)
{ return (void *)NULL; }

void __vec_delete (void *, int , int , void *, int , int )
{}
}
#endif

void
_tt_get_rpc_message (const _tt_message &rpc_message,
_Tt_message_ptr   &message)
{
	message = (_Tt_message *)NULL;

	if (rpc_message.body.body_len) {
		XDR xdrs;
		
		xdrmem_create(&xdrs,
			      rpc_message.body.body_val,
			      (u_int)rpc_message.body.body_len,
			      XDR_DECODE);
		
		message = new _Tt_message;
		(void)message->xdr(&xdrs);
	}
}

void
_tt_get_rpc_messages (const _tt_message_list &rpc_messages,
		      _Tt_message_list_ptr   &messages)
{
	messages = (_Tt_message_list *)NULL;

	if (rpc_messages.messages_len) {
		messages = new _Tt_message_list;
		
		for (unsigned int i=0; i < rpc_messages.messages_len; i++) {
			_Tt_message_ptr message_ptr;

			_tt_get_rpc_message(rpc_messages.messages_val [i],
					    message_ptr);
			(void)messages->append(message_ptr);
		}
	}
}

_Tt_db_results
_tt_set_rpc_message (const _Tt_message_ptr &message,
		     _tt_message           &rpc_message)
{
	if (message.is_null()) {
		rpc_message.body.body_val = (char *)NULL;
		rpc_message.body.body_len = 0;
	} else {
		XDR    xdrs;
		_Tt_xdr_size_stream xdrsz;
		u_int  length;
		
		if (!message->xdr((XDR *)xdrsz)) {
			return TT_DB_ERR_ILLEGAL_MESSAGE;
		} else {
			length = (u_int)xdrsz.getsize();
		}

		_Tt_string temp_string((int)length);
		xdrmem_create(&xdrs, (char *)temp_string, length, XDR_ENCODE);
		if (!message->xdr(&xdrs)) {
			return TT_DB_ERR_ILLEGAL_MESSAGE;
		}
		
		rpc_message.body.body_val = (char *)malloc(length+1);
		(void)memcpy(rpc_message.body.body_val, (char *)temp_string, length);
		rpc_message.body.body_val[length] = '\0';
		rpc_message.body.body_len = length;
	}

	return TT_DB_OK;
}
