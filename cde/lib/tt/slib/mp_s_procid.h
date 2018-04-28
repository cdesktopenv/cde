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
/*%%  $XConsortium: mp_s_procid.h /main/3 1995/10/23 11:59:00 rswiston $ 			 				 */
/* %w% @(#) 
 * 
 * Copyright (c) 1990, 1992 by Sun Microsystems, Inc.
 * 
 * This file implements the server side of the _Tt_procid object.
 */
#ifndef _MP_S_PROCID_H
#define _MP_S_PROCID_H
#include "mp/mp_procid.h"
#include "mp/mp_trace.h"
#include "mp_ptype_utils.h"
#include "mp_s_procid_utils.h"
#include "mp_s_message_utils.h"
#include "mp_s_pattern_utils.h"

class _Tt_s_procid : public _Tt_procid {
      public:
	_Tt_s_procid();
	_Tt_s_procid(const _Tt_s_procid_ptr &p);
	_Tt_s_procid(const _Tt_procid_ptr &p);
	~_Tt_s_procid();
	int			add_message(const _Tt_s_message_ptr &m);
	Tt_status		add_pattern(const _Tt_s_pattern_ptr &p);
	Tt_status		add_signature_patterns(_Tt_ptype_ptr &ptype,
						       Tt_category category);
	Tt_status		remove_signature_patterns(_Tt_ptype_ptr &ptype);
	Tt_status		declare_ptype(_Tt_string ptid);
	Tt_status		undeclare_ptype(_Tt_string ptid);
	Tt_status		unblock_ptype(_Tt_string ptid);
	Tt_status		exists_ptype(_Tt_string ptid);
	Tt_status		del_pattern(_Tt_string id);
	void			del_pattern(_Tt_pattern_ptr &p);
	//
	// Must be virtual, since _Tt_self_procid redefines it
	//
	virtual Tt_status	init();
	int			is_active() const {
		return(_flags&(1<<_TT_PROC_ACTIVE));
	}
	int			is_ptype(_Tt_string ptid);
	Tt_status		next_message(_Tt_next_message_args &args);
	int			service_timeout(int timeout);
	void			activate();
	void			deactivate(int remove_fds);
	void			set_active(int on);
	int			set_fd(int fd = 0);
	int			set_fd_channel(int port = 0);
	void			set_timeout_for_message(const _Tt_message &m);
	//
	// Must be virtual, since _Tt_self_procid redefines them,
	// and we put self_procids into lists of s_procids.
	//
	virtual Tt_status	signal_new_message();
	virtual Tt_status	update_message(const _Tt_message_ptr &m,
					       Tt_state newstate);
#ifdef OPT_ADDMSG_DIRECT
	Tt_status		signal_new_message(_Tt_message_ptr  &m);
	void			msgread();
#endif
	int			timeout() {
		return _itimeout;
	}
	Tt_status		add_on_exit_message(_Tt_s_message_ptr &m);
	void			cancel_on_exit_messages();
	void			send_on_exit_messages();
      private:
	int			_itimeout;
				// messages not yet tt_message_receive()d
	_Tt_message_list_ptr	_undelivered;
				// requests and offers not yet reacted to
	_Tt_message_list_ptr	_delivered;
	_Tt_s_message_list_ptr	_on_exit_messages;
};

#endif				/* _MP_S_PROCID_H */
