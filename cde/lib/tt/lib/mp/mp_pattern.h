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
/*%%  $XConsortium: mp_pattern.h /main/3 1995/10/23 10:26:51 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_pattern.h	1.15 30 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_pattern.h
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 * 
 * This file implements the _Tt_pattern object which represents the
 * pattern that a process can register in order to receive messages. The
 * fundamental method on a _Tt_pattern object add and return pattern
 * fields and match a pattern to a message.
 */

#ifndef _MP_PATTERN_H
#define _MP_PATTERN_H

#include <mp/mp_global.h>
#include <mp/mp_message.h>
#include <mp/mp_pat_context_utils.h>
#include <mp/mp_pattern_utils.h>
#include <mp/mp_mp.h>
#include <mp/mp_xdr_functions.h>
#include <mp/mp_procid.h>
#include <util/tt_int_rec.h>

typedef Tt_callback_action	(*_Tt_server_callback)(
					const _Tt_message_ptr &msg,
					void		       *server_data
				);

enum _Tt_pattern_flags {
	_TT_PATTERN_IS_REGISTERED,
	_TT_PATTERN_IN_SESSION
};

class _Tt_pattern : public _Tt_object {
      public:
	void			base_constructor();
	_Tt_pattern();
	virtual ~_Tt_pattern();
	Tt_category		category() const;
	Tt_status		set_category(Tt_category c);
	void			set_procid(const _Tt_procid_ptr &pid);
	int			classes() const;
	int			states() const;
	int			paradigms() const;
	int			scopes() const;
	int			reliabilities() const;
	const _Tt_string_list_ptr	&files() const;
	const _Tt_string_list_ptr	&sessions() const;
	const _Tt_string_list_ptr	&ops() const;
	const _Tt_int_rec_list_ptr	&opnums() const;
	const _Tt_string_list_ptr	&objects() const;
	const _Tt_string_list_ptr	&otypes() const;
	const _Tt_string_list_ptr	&senders() const;
	const _Tt_string_list_ptr	&handlers() const;
	const _Tt_string_list_ptr	&sender_ptypes() const;
	const _Tt_string_list_ptr	&handler_ptypes() const;
	const _Tt_arg_list_ptr		&args() const;
	_Tt_pat_context_ptr	context(const char *slotname) const;
	_Tt_pat_context_ptr	context(int i) const;
	int			contextsCount() const;
	Tt_status		add_message_class(Tt_class mclass);
	Tt_status		add_paradigm(Tt_address p);
	Tt_status		add_scope(Tt_scope s);
	Tt_status		add_file(const _Tt_string &file);
	Tt_status		add_netfile(
					const _Tt_string &file,
					int fallback_2_local_netfile = 0
				);
	Tt_status		del_file(const _Tt_string &file);
	Tt_status		add_session(const _Tt_string &s);
	Tt_status		del_session(const _Tt_string &s);
	Tt_status		add_op(const _Tt_string &op);
	Tt_status		add_opnum(int o);
	Tt_status		add_object(const _Tt_string &oid);
	Tt_status		add_otype(const _Tt_string &ot);
	Tt_status		add_sender(const _Tt_string &s);
	Tt_status		add_handler(const _Tt_string &h);
	Tt_status		add_sender_ptype(const _Tt_string &s);
	Tt_status		add_state(Tt_state s);
	Tt_status		add_handler_ptype(const _Tt_string &h);
	Tt_status		add_observer_ptype(const _Tt_string &o);
	Tt_status		add_reliability(Tt_disposition r);
	Tt_status		add_arg(const _Tt_arg_ptr &arg);
	Tt_status		add_context(
					const _Tt_pat_context_ptr &context);
	bool_t			xdr(XDR *xdrs);
	void			set_id(const _Tt_string &sender);
	_Tt_string		id() const;
	const _Tt_procid_ptr	&procid() const;
	_Tt_string		&sender();	
	int			is_registered() const;
	void			set_registered();
	void			clr_registered();
	void			set_in_session();
	void			clr_in_session();
	int			in_session() const;
	Tt_status		join_files(const _Tt_string &sessID) const;
	void			print(const _Tt_ostream &os) const;

	_Tt_server_callback	server_callback;
      protected:
	// convenience methods to add values to integer list and
	// string list fields. (see comment below for a description of
	// the different types of pattern fields.
	void			add_field(int val,
					  _Tt_int_rec_list_ptr &vlist);
	void			add_field(const _Tt_string &val,
					  _Tt_string_list_ptr &vlist);

	// procid that registered this pattern
	_Tt_procid_ptr		_procid;

	// id of this pattern. For dynamic patterns, ids are relative
	// to the procid that registered the pattern. For patterns
	// generated from signatures, patterns are relative to the
	// current session. In either case, pattern ids are unique
	// among all patterns in a session (and BAD things happen if this
	// isn't the case.)
	_Tt_string		_pattern_id;

	// bit vector of _Tt_pattern_flags that are on or off for this
	// object. 
	int			_flags;

	// pattern fields. There are three kinds of fields in a
	// pattern. The simplest of these are string fields which are
	// just lists of strings. Then there are integer lists (only
        // opums). Finally, there are fields which represent
	// collections of enum values. For example  _classes is
	// conceptually a list of Tt_class enums. For these fields we
	// optimize storage as well as matching by representing them
	// as integers such that if enum value x is in the list then
	// <pfield>&(1<<x) is true where pfield is the given pattern
	// field. This means matching on these fields is fast and they
	// take up less space (much less than a real list  of integers).
	//
	// The drawback is that enums with more than 32 members or
	// enums where each member is assigned a number > 32 aren't
        // supported.  

	Tt_category		_category;
	int			_classes;
	int			_paradigms;
	int			_reliabilities;
	int			_scopes;
	int			_states;
	_Tt_int_rec_list_ptr	_opnums;
	_Tt_arg_list_ptr	_args;
	_Tt_pat_context_list_ptr _contexts;
	_Tt_string_list_ptr	_files;
	_Tt_string_list_ptr	_handler_ptypes;
	_Tt_string_list_ptr	_handlers;
	_Tt_string_list_ptr	_objects;
	_Tt_string_list_ptr	_observer_ptypes;
	_Tt_string_list_ptr	_ops;
	_Tt_string_list_ptr	_otypes;
	_Tt_string_list_ptr	_sender_ptypes;
	_Tt_string_list_ptr	_senders;
	_Tt_string_list_ptr	_sessions;

	friend class		_Tt_mp;
	friend class		_Tt_message;
};
#include <mp/mp_pattern_inlines.h>

bool_t tt_xdr_pattern(XDR *xdrs, _Tt_pattern_ptr *pat);
#endif				/* _MP_PATTERN_H */
