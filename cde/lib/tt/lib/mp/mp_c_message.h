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
/*%%  $XConsortium: mp_c_message.h /main/3 1995/10/23 10:20:18 rswiston $ 			 				 */
/* 
 * mp_c_message.h
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file implements the client side of the _Tt_message object.
 */
#ifndef _MP_C_MESSAGE_H
#define _MP_C_MESSAGE_H
#include <mp/mp_message.h>
#include <mp/mp_rpc_interface.h>
#include <mp/mp_c_message_utils.h>

class _Tt_c_message : public _Tt_message {
      public:
	_Tt_c_message();
	~_Tt_c_message();
	void			set_return_handler_flags();
	Tt_status		dispatch(int observers_only = 0);
	Tt_status		dispatch_on_exit();
	int			is_a_diff();
      private:
	Tt_status	dispatch_file_scope_notification(_Tt_file_ptr &f);
	Tt_status		resolve();
	_Tt_rpc_procedure_number
				_rpc_dispatch() const;
	_Tt_rpc_procedure_number
				_rpc_dispatch_2() const;
	// See _Tt_c_procid::next_message() before adding data members!
};

#endif				/* _MP_C_MESSAGE_H */
