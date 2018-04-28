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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_s_pattern.C /main/3 1995/10/23 11:57:37 rswiston $ 			 				
/*
 * @(#)mp_s_pattern.C	1.27 93/09/07
 *
 * mp_s_pattern.cc
 *
 * Copyright (c) 1990, 1992 by Sun Microsystems, Inc.
 */
#include "mp_s_global.h"
#include "mp/mp_arg.h"
#include "mp_s_pat_context.h"
#include "mp/mp_mp.h"
#include "mp_s_mp.h"
#include "mp_s_pattern.h"
#include "mp_s_message.h"
#include "mp_signature.h"
#include "mp_observer.h"
#include "mp_ptype.h"
#include "mp_rpc_implement.h"
#include "mp_s_session.h"
#include "mp/mp_xdr_functions.h"
#include "util/tt_enumname.h"
#include "mp/mp_trace.h"
#include "util/tt_global_env.h"
#include "util/tt_assert.h"


// 
// Creates a pattern from a signature object by extracting the relevant
// fields from the signature. 
// 
_Tt_s_pattern::
_Tt_s_pattern(const _Tt_signature_ptr &sig)
{
	_timestamp = 0;
	set_id(_tt_s_mp->initial_session->address_string());
	set_category(sig->category());
	add_scope(sig->scope());
	add_op(sig->op());
	if (sig->opnum() != -1) {
		add_opnum(sig->opnum());
	}
	if (sig->category()!=TT_OBSERVE) {
		add_handler_ptype(sig->ptid());
	}
	if (sig->otid().len() > 0) {
		add_otype(sig->otid());
		add_paradigm(TT_OTYPE);
		add_paradigm(TT_OBJECT);
	} else {
		add_paradigm(TT_PROCEDURE);
	}
	add_reliability(sig->reliability());
	add_message_class(sig->message_class());

	// We share the arg list from the signature
	// by just assigning the pointer and letting the ref-counting
	// system do its thing.  Conceivably, this might cause
	// consternation if we ever let clients update patterns created
	// from signatures, since updating the arg list would change
	// the in-memory copy of the signature and every other pattern
	// generated from the signature!  However, the most likely
	// way such updates would be generated would be to send a whole
	// new pattern over from the client, so sharing the arg list
	// wouldn\'t cause trouble.

	_args = sig->args();

	// We *cannot* share the context list from the signature as
	// the signature contains plain _Tt_context entries, while
	// the pattern should contain _Tt_pat_context entries.

	_Tt_context_list_cursor c(sig->contexts());

	while (c.next()) {
		_Tt_pat_context_ptr nc = new _Tt_pat_context(**c);
		add_context(nc);
	}
}


_Tt_s_pattern::_Tt_s_pattern ()
{
	_timestamp = 0;
}

_Tt_s_pattern::~_Tt_s_pattern ()
{
}


// 
// Matches a pattern with a message and returns a number indicating how
// well the pattern matches the message. 0 indicates no match whereas a
// positive number indicates some level of matching. The greater the
// level, the "better" the pattern matches the message.  A level of 1
// indicates all the pattern fields are wildcards so this is the minimum
// level of matching. Any level greater than 1 is controlled by the
// number returned from each field match. Thus twiddling these numbers
// gives some control as to what field matches are to be considered more
// important than others. Presently, the algorithm for selecting match
// numbers is that wildcard matches count as 0 whereas any specific match
// counts as 1. The total is added up for each field and returned as the
// value of the match. The variable tm below holds the total count of
// matches. It is initially 1 because wildcard matches don't increment
// it. In any submethod called to do matching the variable tm is passed
// to the method to be incremented.
// 
int _Tt_s_pattern::
match(const _Tt_s_message &msg, const _Tt_msg_trace &trace) const
{
	int			tm = 1;

	if (msg.is_handler_copy()) {
		// message is the original
		if (   (! _generating_ptype.is_null())
		    && (category() == TT_OBSERVE))
		{
			//
			// A static observer pattern can only match
			// the message copy promised to it,
			// and cannot match the original.
			//
			return 0;
		}
	} else {
		// message is a copy promised to observer ptype
		if (   (_generating_ptype.is_null())
		    || (   msg.observer()->ptid()
			!= _generating_ptype->ptid()))
		{
			//
			// If this pattern is not owned by the
			// promised ptype, or is owned by no
			// ptype, then this pattern cannot
			// fulfill the promise.
			//
			return 0;
		}
	}

	trace << "Tt_message & Tt_pattern {\n";
	trace.incr_indent();
	trace << "timestamp:\t" << _timestamp << "\n";
	trace << *this;
	trace.decr_indent();
	trace << "} ";

	// The code for this method is quite repetitive. For each
	// field, the list is checked to see if there are any values. If
	// there are and the appropiate match method returns 0 then the
	// pattern fails to match. The only exception is match_scopes
	// which defaults to not matching if there are no values specified.


	// This function used to not match the op field of the
	// message, because in the usual code path it is
	// hashed on the basis of the op field so it would be
	// redundant to check the op field here.  However, when
	// matching TT_HANDLER messages just to get callbacks run,
	// this is not the case.  To save a little time, we skip
	// the op check if the message paradigm is not TT_HANDLER,
	// even though that looks really funny here!

	if (0 != _ops->count()) {
		if (msg.paradigm() == TT_HANDLER &&
		    ! match_field(msg.op(), _ops,
				  tm, trace, "op" ))
		{
			return(0);
		}
		tm++;
	}



	if (_reliabilities != 0) {
		if (!(_reliabilities&(1<<msg.reliability()))) {
			trace << "== 0; /* Tt_disposition */\n";
			return(0);
		}
		tm++;
	}

	if (_states != 0) {
		if (!(_states&(1<<msg.state()))) {
			trace << "== 0; /* Tt_state */\n";
			return(0);
		}
		tm++;
	}
	if (_classes != 0) {
		if (!(_classes&(1<<TT_CLASS_UNDEFINED))
		    && !(_classes&(1<< msg.message_class()))) {
			trace << "== 0; /* Tt_class */\n";
			return(0);
		}
		tm++;
	}
	if (_paradigms != 0) {
		if (!(_paradigms&(1<<msg.paradigm()))) {
			trace << "== 0; /* Tt_address */\n";
			return(0);
		}
		tm++;
	}


	if (_handlers->count()) {
		_Tt_string		h;

		if (! msg.handler().is_null()) {
			h = msg.handler()->id();
		}
		if (! match_field(h, _handlers, tm, trace, "handler")) {
			return(0);
		}
	}

	if (_handler_ptypes->count() &&
	    ! match_field(msg.handler_ptype(), _handler_ptypes,
			  tm, trace, "handler_ptype" ))
	{
		return(0);
	}


	if (! match_scopes(msg, tm, trace)) {
		return(0);
	}
	if (msg.scope() == TT_SESSION) {
		if (_files->count() &&
		    ! match_field(msg.file(), _files,
				  tm, trace, "file"))
		{
			return(0);
		}
	}
	if (_objects->count() &&
	    ! match_field(msg.object(), _objects,
			  tm, trace, "object"))
	{
		return(0);
	}

	if (_otypes->count() &&
	    ! match_field(msg.otype(), _otypes,
			  tm, trace, "otype"))
	{
		return(0);
	}
	if (_senders->count() &&
	    ! match_field(msg.sender()->id(), _senders,
			  tm, trace, "sender"))
	{
		return(0);
	}

	if (_sender_ptypes->count() &&
	    ! match_field(msg.sender_ptype(), _sender_ptypes,
			  tm, trace, "sender_ptype"))
	{
		return(0);
	}


	if (_args->count() && ! match_args(msg, tm, trace)) {
		return(0);
	}

	if (! match_contexts(msg, tm, trace)) {
		return(0);
	}

	trace << "== " << tm << ";\n";
	return(tm);
}


// 
// Generic matching function for lists of integers. If the given pattern
// values, pvals, is empty then 0 is returned indicating a mismatch.
// Otherwise, 1 is returned if "val" is in the list and the variable tm
// is incremented to update the matching score in _Tt_pattern::match.
// 
int _Tt_s_pattern::
match_field(int val, const _Tt_int_rec_list_ptr &pvals, int &tm,
	    const _Tt_msg_trace &trace, const char *failure_note) const
{
	_Tt_int_rec_list_cursor c(pvals);

	while (c.next()) {
		if (val == c->val) {
			tm++;
			return(1);
		}
	}
	if (failure_note != 0) {
		trace <<  "== 0; /* " << failure_note << " */\n";
	}
	return(0);
}


// 
// Generic matching function for lists of strings. If the given pattern
// values, pvals, is empty then 0 is returned indicating a mismatch.
// Otherwise, 1 is returned if "val" is in the list and the variable tm
// is incremented to update the matching score in _Tt_pattern::match.
//      
int _Tt_s_pattern::
match_field(const _Tt_string &val, const _Tt_string_list_ptr &pvals,
	    int &tm, const _Tt_msg_trace &trace,
	    const char *failure_note) const
{
	_Tt_string_list_cursor c(pvals);

	if (val.len() != 0) {
		while (c.next()) {
			if (val == *c) {
				tm++;
				return(1);
			}
		}
	}
	if (failure_note != 0) {
		trace <<  "== 0; /* " << failure_note << " */\n";
	}
	return(0);
}


// 
// Matches the scope of the message with the pattern scopes. For the
// different scopes, matching is defined differently as specified below. 
// 
int _Tt_s_pattern::
match_scopes(const _Tt_message &msg, int &tm,
	     const _Tt_msg_trace &trace) const
{
	Tt_scope	s = msg.scope();
	int		valid_scope_mask = 0;
	int		rval;

	ASSERT(TT_SCOPE_NONE==0 && TT_SESSION==1 && TT_FILE==2 &&
	       TT_BOTH==3 && TT_FILE_IN_SESSION==4,
	       "Tt_scope enum values changed.  This breaks the following "
	       "code, and also breaks binary compatibility for libtt users. "
	       "You probably don't want to do that, do you?")

	static int valid_scope_masks[] = {
		0,		// TT_SCOPE_NONE
		(1<<TT_SESSION) | (1<<TT_BOTH),	// TT_SESSION
		(1<<TT_FILE) | (1<<TT_BOTH),	// TT_FILE
		(1<<TT_SESSION) | (1<<TT_FILE) | (1<<TT_BOTH), // TT_BOTH
		(1<<TT_FILE_IN_SESSION)};	// TT_FILE_IN_SESSION

	if (! (_scopes & valid_scope_masks[s])) {
		trace << "== 0; /* " << _tt_enumname(s) << " != Tt_scopes */\n";
		return 0;
	}

	// if this is not a file-scoped message and the pattern
	// doesn't contain a value for the _sessions list then we
	// immediately fail the match.
	if (s != TT_FILE &&
	    (msg.session().is_null() || _sessions->count() == 0)) {
		trace << "== 0; /* pattern not joined to "
			 "tt_message_session() */\n";
		return 0;
	}

	switch (s) {
	      case TT_FILE:
		return match_field(msg.file(), _files,
				   tm, trace, "file" );
	      case TT_FILE_IN_SESSION:
		if (msg.session()->has_id(_sessions)) {
			return((_files->count() == 0)
			       || match_field(msg.file(), _files,
					   tm, trace, "file"));
		} else {
			trace <<  "== 0; /* file in session */\n";
			return(0);
		}
	      case TT_SESSION:

		// The session-scope case is so common that we avoid
		// matching the session id by setting a flag on a
		// pattern that we know is in the session (see
		// _Tt_s_procid::add_pattern and
		// _Tt_session::mod_session_id_in_patterns to see how
		// the flag is set). This flag is turned on if this
		// pattern is joined to the current session or not.
 
		if (in_session()) {
			return(1);
		} else {
			trace << "== 0; /* session */\n";
		}
		return(0);

	      case TT_BOTH:
		rval = (msg.session()->has_id(_sessions) +
			match_field(msg.file(), _files, tm, trace, 0));
		if (rval == 0) {
			trace << "== 0; /* file and session */\n";
		}
		return(rval);

	      case TT_SCOPE_NONE:
	      default:
		trace << "== 0; /* Tt_scope */\n";
		return(0);
	}
}


// 
// Returns 1 if the message's args match the pattern args. The
// _Tt_arg::is_match method is used to do the bulk of the work.
// 
int _Tt_s_pattern::
match_args(const _Tt_message &msg, int &tm,
	   const _Tt_msg_trace &trace) const
{
	if (msg.args()->count() == 0) {
		// arg matching was specified but the
		// message doesn't contain any args.
		trace << "== 0; /* args */\n";
		return(0);
	}

	_Tt_arg_list_cursor p_args(_args);
	_Tt_arg_list_cursor m_args(msg.args());
	int cumulative_args_score = 0;

	while (p_args.next()) {

		if (!m_args.next()) {
			trace << "== 0; /* args */\n";
			return(0);
		}

		int used_wildcard;
		int score = p_args->match_score(*m_args, used_wildcard);
		if (score <= 0) {
			trace << "== 0; /* args */\n";
			return(0);
		}
		cumulative_args_score += score;
	}
	tm += cumulative_args_score;
	return(1);
}

//
// Returns 1 if the message's contexts match pattern's contexts.
//
int _Tt_s_pattern::
match_contexts(const _Tt_message &msg, int &tm,
	   const _Tt_msg_trace &trace) const
{
	_Tt_pat_context_list_cursor cntxtC( _contexts );
	while (cntxtC.next()) {
		if (((_Tt_s_pat_context &)**cntxtC).matchVal( msg ) == 0) {
			trace << "== 0; /* contexts */\n";
			return 0;
		}
	}
	tm++;
	return 1;
}



// Routines to set and get the ptype that this pattern was generated
// from, and to test if it was so generate.

_Tt_ptype_ptr &_Tt_s_pattern::
generating_ptype(_Tt_ptype_ptr &pt)
{
	_generating_ptype = pt;
	return pt;
}

_Tt_ptype_ptr &_Tt_s_pattern::
generating_ptype()
{
	return _generating_ptype;
}

int _Tt_s_pattern::
is_from_ptype()
{
	return !_generating_ptype.is_null();
}

Tt_status _Tt_s_pattern::
join_context(const _Tt_msg_context &msgCntxt)
{
	_Tt_pat_context_list_cursor contextC( _contexts );
	while (contextC.next()) {
		if (contextC->slotName() == msgCntxt.slotName()) {
			return contextC->addValue( msgCntxt );
		}
	}
	return TT_WRN_NOTFOUND;
}

Tt_status _Tt_s_pattern::
quit_context(const _Tt_msg_context &msgCntxt)
{
	_Tt_pat_context_list_cursor contextC( _contexts );
	while (contextC.next()) {
		if (contextC->slotName() == msgCntxt.slotName()) {
			return contextC->deleteValue( msgCntxt );
		}
	}
	return TT_WRN_NOTFOUND;
}
