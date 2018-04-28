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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_s_xdr_functions.h /main/3 1995/10/23 12:00:44 rswiston $ 			 				 */
/*
 *
 * mp_s_xdr_functions.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * This file implements the wrappers needed for calling
 * RPC argument XDR routines on the server side.
 */
#ifndef MP_S_XDR_FUNCTIONS_H
#define MP_S_XDR_FUNCTIONS_H
#include <rpc/rpc.h>
#include "api/c/tt_c.h"
#include "mp_s_message_utils.h"
#include "mp_s_pattern_utils.h"
#include "mp_s_procid_utils.h"

struct _Tt_s_add_pattern_args: public _Tt_allocated {
	_Tt_s_procid_ptr	procid;
	_Tt_s_pattern_ptr	pattern;
};

struct _Tt_s_del_pattern_args: public _Tt_allocated {
	_Tt_s_procid_ptr	procid;
	_Tt_string		pattern_id;
};

struct _Tt_s_update_args: public _Tt_allocated {
	_Tt_s_message_ptr	message;
	Tt_state		newstate;
};

bool_t		tt_s_xdr_add_pattern_args(XDR *xdrs,
					_Tt_s_add_pattern_args *args);
bool_t		tt_s_xdr_del_pattern_args(XDR *xdrs,
					_Tt_s_del_pattern_args *args);
bool_t		tt_s_xdr_update_args(XDR *xdrs,
					_Tt_s_update_args *args);
#endif				/*  MP_XDR_FUNCTIONS_H */


