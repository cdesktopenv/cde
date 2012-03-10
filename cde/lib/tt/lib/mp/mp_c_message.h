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
