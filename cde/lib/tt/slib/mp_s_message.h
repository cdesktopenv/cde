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
/*%%  $XConsortium: mp_s_message.h /main/3 1995/10/23 11:55:57 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_s_message.h	1.13 94/11/17
 *
 * Tool Talk Message Passer (MP) - mp_s_message.h
 * 
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 * 
 * This file implements the server side of the _Tt_message object.
 */
#ifndef _MP_S_MESSAGE_H
#define _MP_S_MESSAGE_H
#include "mp/mp_message.h"
#include "mp/mp_trace.h"
#include "mp_otype_utils.h"
#include "mp_signature_utils.h"
#include "mp_observer_utils.h"
#include "mp_s_message_utils.h"
#include "mp_s_procid_utils.h"

class _Tt_s_pattern;

class _Tt_s_message : public _Tt_message {
      public:
	_Tt_s_message();
	virtual ~_Tt_s_message();
	_Tt_s_message(_Tt_s_message *m, _Tt_observer_ptr &o);
	Tt_scope		scope() const;
	int			opnum() const;
	Tt_state		state() const;
	const _Tt_string	&handler_ptype() const;
	Tt_disposition		reliability() const;
	void			qmsg_info(_Tt_qmsg_info_ptr &qm);
	Tt_status		indoctrinate(const _Tt_msg_trace &trace);
	Tt_status		dispatch(const _Tt_msg_trace &trace);
	Tt_status		set_start_env() const;
	int			deliver(const _Tt_msg_trace &trace,
					int deliver_to_observers = 1);
	void			deliver_to_observers_and_handlers(
					const _Tt_msg_trace &trace );
	void			set_return_sender_flags();
	void			set_send_handler_flags();
	Tt_status		change_state(const _Tt_procid_ptr &changer,
					Tt_state s,
					const _Tt_msg_trace &trace );
	void			remove_procid(const _Tt_procid_ptr &p);
	int			is_handler_copy() const {
		return(_observer.is_null());
	}
	const _Tt_observer_ptr &observer() const {
		return _observer;
	}
	Tt_status		set_state(Tt_state state);
	Tt_status		set_scope(Tt_scope s);
	Tt_status		set_reliability(Tt_disposition r);
	void			add_eligible_voter(const _Tt_procid_ptr &p);
      private:
	int			already_tried(const _Tt_procid_ptr &proc);
	Tt_status		started(const _Tt_msg_trace &trace);
	Tt_status		hdispatch();
	void			handle_no_recipients(
					const _Tt_msg_trace &trace);
	Tt_status		handler_dispatch(const _Tt_msg_trace &trace);
	int			match_handler(const _Tt_s_pattern &pat,
					const _Tt_msg_trace &trace,
					int &best_match,
					Tt_category &best_category,
					unsigned int &best_timestamp) const;
	int			match_handler(const _Tt_signature &sig,
					const _Tt_msg_trace &trace,
					int &best_match,
					Tt_category &best_category,
					unsigned int &best_timestamp) const;
	int			match_observer(const _Tt_s_pattern &pat,
					const _Tt_s_procid_ptr &proc,
					const _Tt_msg_trace &trace);
	int			match_observer(const _Tt_signature &pat,
					const _Tt_msg_trace &trace);
	int			match_patterns(_Tt_pattern_list_ptr &patterns,
					const _Tt_msg_trace &trace,
					_Tt_pattern_ptr &best_pattern,
					int deliver_to_observers);
	Tt_status		match_signatures(_Tt_signature_list_ptr &s,
					const _Tt_msg_trace &trace);
	int			match_super_sig(_Tt_otype_ptr ot,
					_Tt_signature_ptr &sig,
					const _Tt_msg_trace &trace);
	Tt_status		object_oriented_dispatch(
					const _Tt_msg_trace &trace);
	Tt_status		procedural_dispatch(
					const _Tt_msg_trace &trace);
	Tt_status		queued(const _Tt_msg_trace &trace);
	Tt_status		rejected(const _Tt_msg_trace &trace);
	void			report_state_change();
	int			needs_observer_match();
	Tt_status		add_voter(const _Tt_procid_ptr &p,
					Tt_state vote,
					const _Tt_msg_trace &trace);
	Tt_status		count_ballots(const _Tt_procid_ptr &last_voter,
					const _Tt_msg_trace &trace);

	unsigned int		_when_last_matched;
	_Tt_procid_list_ptr	_tried;
	int			_state_reported;
	_Tt_observer_list_ptr	_observers;
	_Tt_observer_ptr	_observer;
	int			_num_recipients_yet_to_vote;
	_Tt_s_message_ptr	_original;
};

#endif				/* _MP_S_MESSAGE_H */
