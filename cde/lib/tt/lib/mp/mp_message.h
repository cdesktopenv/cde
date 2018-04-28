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
/*%%  $XConsortium: mp_message.h /main/3 1995/10/23 10:24:32 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_message.h	1.28 93/09/07
 *
 * Tool Talk Message Passer (MP) - mp_message.h
 * 
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 * 
 * This file implements the _Tt_message object which represents the
 * message that is being sent and received by a process. The message
 * object in server mode implements the methods that are responsible for
 * delivering the message. Thus this is the heart of the message server. 
 */
#ifndef _MP_MESSAGE_H
#define _MP_MESSAGE_H
#include <sys/types.h>
#include "mp/mp_global.h"
#include "mp/mp_arg_utils.h"
#include "mp/mp_msg_context_utils.h"
#include "mp/mp_message_utils.h"
#include "mp/mp_pattern_utils.h"
#include "mp/mp_procid_utils.h"
#include "mp/mp_session_utils.h"
#include "util/tt_iostream.h"

/* 
 * The following enum defines bit masks used in xdr'ing a _Tt_message
 * structure. Since there are important cases where less fields need
 * to be transmitted (for example, whenever it is known that the
 * recipient has seen the message before, the entire structure need
 * not be transmitted) these bit fields control whether the relevant
 * piece of the _Tt_message struct is xdr'ed or not. For example, if
 * _TT_MSK_ID is off then the _id field of the _Tt_message isn't
 * transmitted. 
 */
enum _Tt_message_ptr_masks {
	_TT_MSK_ID		= (1<<0),
	_TT_MSK_MESSAGE_CLASS	= (1<<1),
	_TT_MSK_STATE		= (1<<2),
	_TT_MSK_PARADIGM	= (1<<3),
	_TT_MSK_SCOPE		= (1<<4),
	_TT_MSK_RELIABILITY	= (1<<5),
	_TT_MSK_OPNUM		= (1<<6),
	_TT_MSK_UID		= (1<<7),
	_TT_MSK_GID		= (1<<8),
	_TT_MSK_STATUS		= (1<<9),
	_TT_MSK_FLAGS		= (1<<10),
	_TT_MSK_SESSION		= (1<<11),
	_TT_MSK_SENDER		= (1<<12),
	_TT_MSK_HANDLER		= (1<<13),
	_TT_MSK_FILE		= (1<<14),
	_TT_MSK_OBJECT		= (1<<15),
	_TT_MSK_OP		= (1<<16),
	_TT_MSK_ARGS		= (1<<17),
	_TT_MSK_OTYPE		= (1<<18),
	_TT_MSK_SENDER_PTYPE	= (1<<19),
	_TT_MSK_HANDLER_PTYPE	= (1<<20),
	_TT_MSK_PATTERN_ID	= (1<<21),
	_TT_MSK_RSESSIONS	= (1<<22),
	_TT_MSK_STATUS_STRING	= (1<<23),
	_TT_MSK_CONTEXTS	= (1<<24),
	_TT_MSK_OFFEREES	= (1<<25)
};

#define	SET_PTR_GUARD(test, msk)\
if (test) (_ptr_guards) |= msk; else (_ptr_guards) &= ~(msk)

#define	SET_GUARD(guard, test, msk)\
if (test) (guard) |= msk; else (guard) &= ~(msk)

enum _Tt_message_flagbits {
	_TT_MSG_IS_REMOTE,	/* message sender not in current session */
	_TT_MSG_OBSERVERS_ONLY,	/* only attempt observer patterns */
	_TT_MSG_IS_SUPER,	/* method invocation to parent class */
	_TT_MSG_UPDATE_XDR_MODE, /* message is being sent as an update */
	                         /* (used by the xdr method) */
	_TT_MSG_OBSERVERS_MATCH,
	_TT_MSG_IS_START_MSG,
	_TT_MSG_OBSERVER,
	_TT_MSG_AWAITING_REPLY
};

class _Tt_message : public _Tt_object {
      public:
	void			base_constructor();
	_Tt_message();
	virtual ~_Tt_message();
	/* getting message fields */

	Tt_class		message_class() const {
		return _message_class;
	}
	Tt_state		state() const {
		return _state;
	}

	Tt_scope		scope() const {
		return _scope;
	}
	int			opnum() const {
		return _opnum;
	}
	const _Tt_string	&handler_ptype() const {
		return _handler_ptype;
	}
	Tt_disposition		reliability() const {
		return _reliability;
	}

	Tt_address		paradigm() const {
		return _paradigm;
	}
	const _Tt_string 	&file() const {
		return _file;
	}
	const _Tt_session_ptr	&session() const {
		return _session;
	}
	const _Tt_string	&op() const {
		return _op;
	}
	const _Tt_arg_list_ptr	&args() const {
		return _args;
	}
	_Tt_msg_context_ptr	context(const char *slotname) const;
	_Tt_msg_context_ptr	context(int i) const;
	int			contextsCount() const;
	const _Tt_string	&object() const {
		return _object;
	}
	const _Tt_string	&otype() const {
		return _otype;
	}
	const _Tt_procid_ptr 	&sender() const {
		return _sender;
	}
	const _Tt_procid_ptr	&handler() const {
		return _handler;
	}
	const _Tt_procid_list_ptr	&abstainers() const {
		return _abstainers;
	}
	const _Tt_procid_list_ptr	&accepters() const {
		return _accepters;
	}
	const _Tt_procid_list_ptr	&rejecters() const {
		return _rejecters;
	}
	const _Tt_string	&sender_ptype() const {
		return _sender_ptype;
	}
	int			queue() const {
		return reliability() & TT_QUEUE;
	}
	int			start() const {
		return reliability() & TT_START;
	}
	int			id() const;
	const _Tt_string	&api_id() const;
	uid_t			uid() const;
	gid_t			gid() const;
	int			status() const {
		return _status;
	}
	_Tt_string		&status_string();

	/* setting message fields */
	Tt_status		set_message_class(Tt_class mclass);
	Tt_status		set_state(Tt_state state);
	Tt_status		set_paradigm(Tt_address p);
	Tt_status		set_scope(Tt_scope s);
	Tt_status		set_file(_Tt_string file);
	Tt_status		set_session(_Tt_session_ptr &s);
	Tt_status		set_session(_Tt_string sessid);
	Tt_status		set_op(_Tt_string op);
	Tt_status		set_opnum(int o);
	Tt_status		add_arg(_Tt_arg_ptr &arg);
	Tt_status		add_context(_Tt_msg_context_ptr &context);
	Tt_status		set_object(_Tt_string oid);
	Tt_status		set_otype(_Tt_string ot);
	Tt_status		set_sender(_Tt_procid_ptr &s);
	Tt_status		set_handler_procid(const _Tt_procid_ptr &h);
	Tt_status		unset_handler_procid(void);
	Tt_status		add_voter(const _Tt_procid_ptr &p,
					Tt_state vote);
	Tt_status		add_accepter(const _Tt_procid_ptr &p);
	Tt_status		add_rejecter(const _Tt_procid_ptr &p);
	Tt_status		set_sender_ptype(_Tt_string s);
	Tt_status		set_handler_ptype(_Tt_string h);
	Tt_status		set_reliability(Tt_disposition r);
	void			set_super();
	Tt_status		set_status(int st);
	Tt_status		set_status_string(_Tt_string st);
	void			set_start_message(int flag = 1);
	int			is_start_message() const;
	void			set_awaiting_reply(int flag = 1);
	int			is_awaiting_reply() const;
	void			print(const _Tt_ostream &sink) const;
	void			update_message(const _Tt_message_ptr &m);
	int			is_equal(const _Tt_message_ptr &m);
	void			set_observer_procid(const _Tt_procid_ptr &h);
	void			clr_observer_procid();
	int			is_observer() {
		return(_flags&(1<<_TT_MSG_OBSERVER));
	}
	bool_t			xdr(XDR *xdrs);
	_Tt_string		&pattern_id();
	void			set_pattern_id(_Tt_string id);
	Tt_status		set_id();
      protected:
	void			add_out_arg(_Tt_arg_ptr &arg);

	_Tt_string		_pattern_id;
	Tt_state		_state;
	int			_status;
        Tt_address              _paradigm;
	Tt_scope		_scope;
	Tt_disposition		_reliability;
	int			_opnum;
	_Tt_string		_object;
	_Tt_string		_file;
	_Tt_string		_op;
	_Tt_string		_otype;
	_Tt_session_ptr		_session;
	_Tt_procid_ptr		_sender;
	_Tt_procid_ptr		_handler;
	_Tt_procid_list_ptr	_abstainers;
	_Tt_procid_list_ptr	_accepters;
	_Tt_procid_list_ptr	_rejecters;
	_Tt_string		_sender_ptype;
	_Tt_string		_handler_ptype;
	_Tt_string_list_ptr	_rsessions;
	int			_id;
	_Tt_string		_api_id;
	_Tt_arg_list_ptr	_args;
	_Tt_arg_list_ptr	_out_args;
	_Tt_msg_context_list_ptr _contexts;
	int			_flags;
	int			_ptr_guards;
	uid_t			_uid;
	gid_t			_gid;
	int			_full_msg_guards;
        Tt_class                _message_class;
        _Tt_string              _status_string;

      private:
	Tt_status		_set_id(int id);
};

bool_t	tt_xdr_message(XDR *xdrs, _Tt_message_ptr *msgp);
#endif				/* _MP_MESSAGE_H */
