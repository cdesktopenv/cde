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
/*%%  $XConsortium: mp_pattern_inlines.h /main/3 1995/10/23 10:26:59 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_pattern_inlines.h	1.6 30 Jul 1993
 *
 * mp_pattern_inlines.h
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 */

// inline functions definitions for selected _Tt_pattern methods.

inline Tt_category _Tt_pattern::
category() const
{
	return _category;
}

inline Tt_status _Tt_pattern::
set_category(Tt_category c)
{
	_category = c;
	return(TT_OK);
}

inline void _Tt_pattern::
set_procid(const _Tt_procid_ptr &pid)
{
	_procid = pid;
}

inline int _Tt_pattern::
classes() const
{
	return _classes;
}

inline int _Tt_pattern::
states() const
{
	return _states;
}

inline int _Tt_pattern::
paradigms() const
{
	return _paradigms;
}

inline int _Tt_pattern::
scopes() const
{
	return _scopes;
}

inline int _Tt_pattern::
reliabilities() const
{
	return _reliabilities;
}

inline const _Tt_string_list_ptr & _Tt_pattern::
files() const
{
	return _files;
}

inline const _Tt_string_list_ptr & _Tt_pattern::
sessions() const
{
	return _sessions;
}

inline const _Tt_string_list_ptr & _Tt_pattern::
ops() const
{
	return _ops;
}

inline const _Tt_int_rec_list_ptr & _Tt_pattern::
opnums() const
{
	return _opnums;
}

inline const _Tt_string_list_ptr & _Tt_pattern::
objects() const
{
	return _objects;
}

inline const _Tt_string_list_ptr & _Tt_pattern::
otypes() const
{
	return _otypes;
}

inline const _Tt_string_list_ptr & _Tt_pattern::
senders() const
{
	return _senders;
}

inline const _Tt_string_list_ptr & _Tt_pattern::
handlers() const
{
	return _handlers;
}

inline const _Tt_string_list_ptr & _Tt_pattern::
sender_ptypes() const
{
	return _sender_ptypes;
}


inline const _Tt_string_list_ptr & _Tt_pattern::
handler_ptypes() const
{
	return _handler_ptypes;
}


inline const _Tt_arg_list_ptr & _Tt_pattern::
args() const 
{
	return _args;
}

inline	Tt_status _Tt_pattern::
add_message_class(Tt_class mclass)
{
	_classes |= (1<<mclass);
	return(TT_OK);
}

inline	Tt_status _Tt_pattern::
add_paradigm(Tt_address p)
{
	_paradigms |= (1<<p);
	return(TT_OK);
}

inline	Tt_status _Tt_pattern::
add_scope(Tt_scope s)
{
	_scopes |= (1<<s);
	return(TT_OK);
}

inline	Tt_status _Tt_pattern::
add_file(const _Tt_string &file)
{
	add_field(file, _files);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_session(const _Tt_string &s)
{
	add_field(s, _sessions);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_op(const _Tt_string &op)
{
	add_field(op, _ops);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_opnum(int o)
{
	add_field(o, _opnums);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_otype(const _Tt_string &ot)
{
	add_field(ot, _otypes);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_sender(const _Tt_string &s)
{
	add_field(s, _senders);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_handler(const _Tt_string &h)
{
	add_field(h, _handlers);
	return(TT_OK);
}

inline	Tt_status _Tt_pattern::
add_sender_ptype(const _Tt_string &s)
{
	add_field(s, _sender_ptypes);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_state(Tt_state s)
{
	_states |= (1<<s);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_handler_ptype(const _Tt_string &h)
{
	add_field(h, _handler_ptypes);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_reliability(Tt_disposition r)
{
	_reliabilities |= (1<<r);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_arg(const _Tt_arg_ptr &arg)
{
	_args->append(arg);
	return(TT_OK);
}

inline Tt_status _Tt_pattern::
add_context(const _Tt_pat_context_ptr &context)
{
	_contexts->append_ordered(context);
	return(TT_OK);
}

inline _Tt_string _Tt_pattern::
id() const
{
	return _pattern_id;
}

inline const _Tt_procid_ptr & _Tt_pattern::
procid() const
{
	return _procid;
}

inline int _Tt_pattern::
is_registered() const
{
	return _flags&(1<<_TT_PATTERN_IS_REGISTERED);
}

inline void _Tt_pattern::
set_registered()
{
	_flags |= (1<<_TT_PATTERN_IS_REGISTERED);
}

inline void _Tt_pattern::
clr_registered()
{
	_flags &= ~(1<<_TT_PATTERN_IS_REGISTERED);
}

inline void _Tt_pattern::
set_in_session()
{
	_flags |= (1<<_TT_PATTERN_IN_SESSION);
}

inline void _Tt_pattern::
clr_in_session()
{
	_flags &= ~(1<<_TT_PATTERN_IN_SESSION);
}

inline int _Tt_pattern::
in_session() const
{
	return(_flags&(1<<_TT_PATTERN_IN_SESSION));
}
