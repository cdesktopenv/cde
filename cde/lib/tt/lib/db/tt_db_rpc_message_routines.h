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
/*%%  $XConsortium: tt_db_rpc_message_routines.h /main/3 1995/10/23 10:05:17 rswiston $ 			 				 */
/*
 * tt_db_rpc_message_routines.h - Declares routines for converting TT 
 *                                message classes to RPC arguments.
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#ifndef _TT_DB_RPC_MESSAGE_ROUTINES_H
#define _TT_DB_RPC_MESSAGE_ROUTINES_H

#include "mp/mp_message_utils.h"
#include "db/db_server.h"

extern void _tt_get_rpc_message (const _tt_message&, _Tt_message_ptr&);
extern void _tt_get_rpc_messages (const _tt_message_list&,
				  _Tt_message_list_ptr&);
extern _Tt_db_results _tt_set_rpc_message (const _Tt_message_ptr&,
					   _tt_message&);

#endif /* _TT_DB_RPC_MESSAGE_ROUTINES_H */
