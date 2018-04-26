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
/*%%  $XConsortium: mp_self_procid.h /main/3 1995/10/23 12:01:07 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_self_procid.h	1.1 93/07/09
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef _MP_SELF_PROCID_H
#define _MP_SELF_PROCID_H

#include "mp_s_procid.h"
#include "mp_s_pattern_utils.h"

class _Tt_self_procid : public _Tt_s_procid {
    public:
				_Tt_self_procid();
	virtual			~_Tt_self_procid();

	virtual Tt_status	init();
	virtual Tt_status	signal_new_message();

	static Tt_callback_action handle_Session_Trace(
					const _Tt_message_ptr &msg,
					void		      *self_proc
				);
	static Tt_callback_action observe_Saved(
					const _Tt_message_ptr &msg,
					void		      *self_proc
				);
	//
	// Server-side equivalent of tt_pattern_create()
	//
	static _Tt_s_pattern   *s_pattern_create();
    private:
	Tt_status		_process_msg(
					const _Tt_message_ptr &msg
				);
	//
	// Server-side equivalent of tt_message_reply(), tt_message_fail()
	//
	Tt_status		_reply(
					const _Tt_message_ptr &msg
				);
	Tt_status		_fail(
					const _Tt_message_ptr &msg,
					int		       status
				);
	virtual Tt_status	update_message(
					const _Tt_message_ptr &msg,
					Tt_state	       new_state
				);
	//
	// Custom callbacks for doing things ttsession likes to do
	//
	Tt_callback_action	_handle_Session_Trace(
					const _Tt_message_ptr &msg
				);
	Tt_callback_action	_observe_Saved(
					const _Tt_message_ptr &msg
				);
};

#endif /* _MP_SELF_PROCID_H */
