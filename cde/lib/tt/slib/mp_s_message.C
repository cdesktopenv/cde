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
//%%  $TOG: mp_s_message.C /main/4 1999/09/17 18:29:54 mgreess $ 			 				
/*
 *
 * @(#)mp_s_message.C	1.53 94/11/17
 *
 * Tool Talk Message Passer (MP) - mp_s_message.cc
 *
 * Copyright (c) 1990,1992,1993 by Sun Microsystems, Inc.
 */

/* 
 * Server-specific methods for the _Tt_message class
 */
#include "mp_s_global.h"
#include "mp/mp_arg.h"
#include "mp/mp_msg_context.h"
#include "mp_s_file.h"
#include "mp_s_message.h"
#include "mp_s_mp.h"
#include "mp/mp_mp.h"
#include "mp_observer.h"
#include "mp_otype.h"
#include "mp_s_pattern.h"
#include "mp_s_procid.h"
#include "mp_ptype.h"
#include "mp_rpc_implement.h"
#include "mp_s_session.h"
#include "util/tt_enumname.h"
#include "mp/mp_trace.h"
#include "util/tt_global_env.h"
#include <unistd.h>
#include "util/tt_port.h"

_Tt_s_message::
_Tt_s_message()
{
	// The active message count is just to enforce the limit
	// on the number of active messages.  Doing this in the
	// constructor was expedient, but because we construct
	// a message before _tt_mp is initialized, we have
	// to omit it in that case.
	if (_tt_s_mp) {
		_tt_s_mp->active_messages++;
	}
	_when_last_matched = 0;
	_state_reported = 0;
	_num_recipients_yet_to_vote = 0;
}


_Tt_s_message::
~_Tt_s_message()
{
	if (_tt_s_mp && _tt_s_mp->active_messages>0) {
		_tt_s_mp->active_messages--;
	}
}


// 
// Makes a copy of a message. The main reason for copying messages is for
// observers. This is so that the observer will see the message in the
// state in which it was observed (otherwise, a message could be observed
// in one state but when the observer gets around to receiving the
// message the message could have been changed).
// 
// If the _Tt_observer_ptr parameter is null then this is a copy of a
// message for a dynamic observer. If it isn't null then this is a copy
// of a message for a static observer. The difference is that static
// observers get to "shadow" certain properties of a message. For
// example, a message may have TT_DISCARD reliability but a static
// observer may request TT_START reliability when observing the message.
// Furthermore, when the static observer receives the message, it expects
// to see the original message (in this case the one with TT_DISCARD as
// the value of the reliability field). 
// 
// The mechanism to handle this is to have a special field _observer in a
// message. In a message with a non-null _observer, the methods that
// return the value of the "shadowable" properties return the value of
// the observer, otherwise they return the value in the message itself.
// However, when transmitting a message to a recipient the message values
// are always used.
// 
_Tt_s_message::
_Tt_s_message(_Tt_s_message *m, _Tt_observer_ptr &o)
{
	base_constructor();
	_when_last_matched = 0;
	_state_reported = 0;
	_full_msg_guards = m->_full_msg_guards;
	if (o.is_null()) {
		_state		= m->_state;
		_scope		= m->_scope;
		_reliability	= m->_reliability;
		_opnum		= m->_opnum;
		_handler_ptype	= m->_handler_ptype;
	} else {
		_state		= m->_state;
		o->set_state(_state);
		set_scope(o->scope());
		set_reliability(o->reliability());
		set_opnum(o->opnum());
		set_handler_ptype(m->_handler_ptype);
		// note: _observer must be set after the above fields,
		// otherwise the set_* functions won't set the
		// appropiate flags in _full_msg_guards
		_observer	= o;
	}
	_status		= m->_status;
	_status_string	= m->_status_string;
	// XXX holtz 18 Jul 94 Observers probably see latest args and
	// contexts because these two lists are shared, notwithstanding
	// comment above about showing observers a message snapshot.
	_args		= m->_args;
	_contexts	= m->_contexts;
	_message_class	= m->_message_class;
	_paradigm	= m->_paradigm;
	_object		= m->_object;
	_file		= m->_file;
	_op		= m->_op;
	_otype		= m->_otype;
	_session	= m->_session;
	_sender		= m->_sender;
	_handler	= m->_handler;
	// Note: no need for observers to see partial _abstainers etc.
	// Note: handler_ptype explicitly not set for observer copy.
	_sender_ptype	= m->_sender_ptype;
	_pattern_id	= m->_pattern_id;
	_id		= m->_id;
	_api_id		= m->_api_id;
	_gid		= m->_gid;
	_uid		= m->_uid;
	_flags		= m->_flags;
	_rsessions	= m->_rsessions;
	_when_last_matched = m->_when_last_matched;
	_original	= m;
	_num_recipients_yet_to_vote = 0;
}

// 
// Called by change_state when invoked with TT_REJECTED. Causes
// re-delivery of the message to attempt to find another handler. The
// deliver method will detect when all possible procids have been tried
// and send the message status to the original message sender.
// 
Tt_status _Tt_s_message::
rejected(const _Tt_msg_trace &trace)
{
	if (_message_class != TT_REQUEST) { // XXX can only reject requests
		return TT_OK;
	}
	// add handler to list of tried procids for this message.
	// This list is never decremented so this insures that this
	// handler will never see this message again.
	// --> Should a procid be able to accept a message it had
	// previously rejected?

	if (!_handler.is_null()) {
		if (_tried.is_null()) {
			_tried = new _Tt_procid_list();
		}
		// explicitly clear this so that starting will be
		// properly handled if rejected.
		_state_reported &= ~(1<<TT_STARTED);
		_tried->push(_handler);
		unset_handler_procid();
	}

	// attempt re-delivery, but not to observers. The state gets
	// changed back to TT_SENT which is what a prospective handler
	// will be expecting as the state of an incoming request.
	set_state(TT_SENT);
	deliver( trace, 0 );
	return(TT_OK);
}


//
// Returns 1 if this message needs to be matched against observer
// patterns.  This check involves _when_last_matched and the
// _tt_s_mp->when_last_observer_registered.
// _when_last_matched gets updated every
// time this message is matched against observer patterns. Every time
// the message matches an observer pattern a flag called
// _TT_MSG_OBSERVERS_MATCH is set. Thus if _when_last_matched is the same
// as _tt_s_mp->now and this flag is not set then
// re-matching this message against observer patterns would be
// useless. This lets us avoid pattern matching after every state
// change for a message if it is known that no patterns would match
// anyway. 
//
int _Tt_s_message::
needs_observer_match()
{
	return(_when_last_matched == 0 ||
	       _flags&(1<<_TT_MSG_OBSERVERS_MATCH) ||
	       _when_last_matched < _tt_s_mp->when_last_observer_registered);
}


// 
// Called by change_state when invoked with TT_QUEUED.  Causes
// notification of the sender (if this message is a request). Also,
// re-delivery is done to catch observers of this state and of
// course the message is added to the appropiate queue. Note that
// file-scope queueing of requests is not implemented.
//
Tt_status _Tt_s_message::
queued(const _Tt_msg_trace &trace)
{
	_Tt_ptype_ptr		ptype;

	//
	// queueing is only allowed for a message addressed to a
	// ptype.
	//
	if (! handler_ptype().len()) {
		return(TT_ERR_PTYPE);
	}
	if (! _tt_s_mp->ptable->lookup(handler_ptype(), ptype)) {
		return(TT_ERR_PTYPE);
	}

	_Tt_message_ptr		m = this;

	switch (scope()) {
	      case TT_SESSION:
	      case TT_FILE_IN_SESSION:
		_tt_s_mp->initial_s_session->queue_message(m);
		break;
	      case TT_FILE:
	      case TT_BOTH:
		set_status((int)TT_ERR_UNIMP);
		return(TT_ERR_UNIMP);
	      case TT_SCOPE_NONE:
	      default:
		return(TT_ERR_SCOPE);
	}

	report_state_change();

	if (needs_observer_match()) {
		(void)deliver( trace );
	}
	return(TT_OK);
}


// 
// Called by _Tt_s_message::change_state to change the state of the
// message to TT_STARTED state. In order to do this we invoke the
// _Tt_ptype::start method on the handler ptype to launch a new instance
// of the ptype and then we report the state change to the sender of the
// message. We then redeliver the message to any observers.
// 
Tt_status _Tt_s_message::
started(const _Tt_msg_trace &trace)
{
	if (! handler_ptype().len()) {
		return(TT_ERR_PTYPE);
	}


	_Tt_ptype_ptr		ptype;
	_Tt_procid_ptr		proc;
	_Tt_s_message_ptr	mptr = this;
	Tt_status		status;

	if (! _tt_s_mp->ptable->lookup(handler_ptype(), ptype)) {
		return(TT_ERR_PTYPE);
	}

	if ((status = ptype->start(mptr, trace)) != TT_OK) {
		return(status);
	}

	report_state_change();

	// re-deliver message to observers.
	if (needs_observer_match()) {
		(void)deliver( trace );
	}
	return(TT_OK);
}


// 
// Called on to inform the sender of a message about a state change in
// the message.  If the sender is local then we just use the
// _Tt_s_procid::add_message method to add the message to the sender's
// undelivered queue. Otherwise, we invoke an rpc call on the remote
// session to send it the message. That remote session is then
// responsible for forwarding the message on to the sender (see
// _tt_rpc_hupdate_msg in mp/mp_rpc_implement.cc for more details).
// Note that only requests will get sent back to the sender.
//
void _Tt_s_message::
report_state_change()
{
	if (   (is_handler_copy() && _message_class == TT_REQUEST)
	    || (_message_class == TT_OFFER))
	{
		if (! (_flags&(1<<_TT_MSG_IS_REMOTE))) {
			((_Tt_s_procid *)_sender.c_pointer())->add_message(this);
		} else {
			_Tt_update_args		args;

			args.message = this;
			args.newstate = state();
			(void)_session->call(TT_RPC_HUPDATE_MSG,
					     (xdrproc_t)tt_xdr_update_args,
					     (char *)&args,
					     (xdrproc_t)xdr_void,
					     (char *)0);
		}
	}
}


// 
// Called to change the state of a message to s.  The _state_reported
// field in the message keeps track of whether this message has been in
// this state before. If it has then this method just returns.  One
// exception to this is the TT_REJECTED state which a message is allowed
// to go to more than once (because eventually all the handlers for the
// message will be exhausted and the message will get either failed or
// handled).  The other two exceptions are TT_ACCEPTED and TT_ABSTAINED.
//
// If changer.is_null(), the _sender is assumed to be the changer.
// This probably has no effect, since the absence of a changer usually
// implies TT_FAILED/TT_RETURNED, in which case the changer is
// irrelevant.
// 
Tt_status _Tt_s_message::
change_state(const _Tt_procid_ptr &changer, Tt_state s,
	     const _Tt_msg_trace &trace)
{
	Tt_state		oldstate = state();

	//
	// Lots of code calls change_state(TT_FAILED) if the wheels
	// come off.  TT_OFFERs cannot fail, they just get abstained
	// or returned.
	//
	if ((_message_class == TT_OFFER) && (s == TT_FAILED)) {
		if (changer.is_null() || (changer->is_equal( sender() ))) {
			s = TT_RETURNED;
		} else {
			s = TT_ABSTAINED;
		}
	}

	switch (s) {
	    case TT_REJECTED:
	    case TT_ACCEPTED:
	    case TT_ABSTAINED:
		break;
	    default:
		if (_state_reported&(1<<s)) {
			return TT_OK;
		} else {
			_state_reported |= (1<<s);
		}
		break;
	}

	set_state(s);
	_Tt_msg_trace state_trace( *this, oldstate );

	Tt_status status = TT_OK;
	switch (s) {
	      case TT_HANDLED:
	      case TT_FAILED:
	      case TT_RETURNED:
		if (needs_observer_match()) {
			/* attempt re-delivery of this message */
			// XXX why do this for TT_FAILED?
			deliver_to_observers_and_handlers( trace );
		}
		report_state_change();
		break;
	      case TT_REJECTED:
		status = rejected( trace );
		break;
	      case TT_QUEUED:
		status = queued( trace );
		break;
	      case TT_STARTED:
		status = started( trace );
		break;
	}
	if (! changer.is_null()) {
		add_voter( changer, s, trace );
	} else if ((! _sender.is_null()) && (s != TT_REJECTED)) {
		//
		// A null changer means the changer is the sender
		// XXX Exclude TT_REJECTED until TT_FILE-scoped
		// TT_OFFERs are implemented; see _tt_rpc_hupdate_msg().
		//
		add_voter( _sender, s, trace );
	}
	return status;
}


// 
// This method gets called just before a message is going to be sent to a
// handler or offeree. It basically sends all the fields of the message that are
// non-empty except for the _state field which is always TT_SENT unless
// this message is a TT_STARTED message indicating it is a start message
// for the handler. The code in _Tt_message::xdr will set the _state of a
// message to TT_SENT if it isn't sent.
// 
void _Tt_s_message::
set_send_handler_flags()
{
	_flags &= ~(1<<_TT_MSG_UPDATE_XDR_MODE);	
	_ptr_guards = _full_msg_guards;
	SET_PTR_GUARD(_state == TT_STARTED, _TT_MSK_STATE);
}


// 
// This method gets called just before a message is going to be xdr'ed to
// its original sender.  We can assume that the sender has a copy of this
// message containing the fields that haven't changed when this message
// was failed or handled. Furthermore, tt_message_destroy doesn't destroy
// messages that are in a non-final state. The result is that we can
// assume that fields that weren't changed by the message being handled
// or failed (such as for example the op field) don't need to be sent.
// Furthermore, we don't have to send back fields that have a default
// value such as (_reliability == TT_DISCARD). Most importantly, only
// TT_OUT or TT_INOUT arguments need to be sent back since the TT_IN
// arguments couldn't have changed.
// 
void _Tt_s_message::
set_return_sender_flags()
{
	const int	req_fields = _TT_MSK_ID |
	                             _TT_MSK_STATE |
				     _TT_MSK_STATUS |
				     _TT_MSK_FLAGS;


	// turn on this flag to let _Tt_message::xdr know that it
	// should send out the _out_args field instead of the _args
	// field. 
	_flags |= (1<<_TT_MSG_UPDATE_XDR_MODE);

	// initialize the required fields.
	_ptr_guards = req_fields;

	switch (_paradigm) {
	      case TT_OTYPE:
	      case TT_OBJECT:
		// scope can change for these messages
		_ptr_guards |= _TT_MSK_SCOPE;
		// fall into
	      case TT_PROCEDURE:
		// don't send default TT_DISCARD reliability
		SET_PTR_GUARD(_reliability != TT_DISCARD, _TT_MSK_RELIABILITY);
		break;
	      default:
		break;
	}

	// collect all TT_OUT and TT_INOUT args
	if (_out_args.is_null() && !_args.is_null() && _args->count()) {
		_Tt_arg_list_cursor	argc(_args);

		_out_args = new _Tt_arg_list();
		while (argc.next()) {
			add_out_arg(*argc);
		}
	}
	
	// send the following fields only if they're turned on in
	// _full_msg_guards which means they have non-default or
	// non-empty values.
	//
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_SCOPE, _TT_MSK_SCOPE);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_STATUS_STRING,
		      _TT_MSK_STATUS_STRING);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_FILE, _TT_MSK_FILE);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_OBJECT, _TT_MSK_OBJECT);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_ARGS, _TT_MSK_ARGS);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_CONTEXTS, _TT_MSK_CONTEXTS);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_OFFEREES, _TT_MSK_OFFEREES);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_HANDLER_PTYPE,
		      _TT_MSK_HANDLER_PTYPE);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_HANDLER, _TT_MSK_HANDLER);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_PATTERN_ID,_TT_MSK_PATTERN_ID);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_OPNUM, _TT_MSK_OPNUM);
}

//
// Take a message off the wire and set its internal state to
// be consistent with this message server.
// On error, sets status, fails, and returns:
//	TT_ERR_PROCID
//	TT_ERR_SESSION
//
Tt_status _Tt_s_message::
indoctrinate(const _Tt_msg_trace &trace)
{
	if (_sender.is_null()) {
		// in principle the client library makes sure the
		// sender field is non-null but it doesn't hurt to
		// check. 
		_tt_syslog(0, LOG_ERR, "_sender.is_null()");
		set_status((int)TT_ERR_PROCID);
		change_state(0, TT_FAILED, trace);
		return TT_ERR_PROCID;
	}

	//
	// the _sender field is a _Tt_procid_ptr but it only points to
	// an uninitialized _Tt_procid object that just contains an
	// id. We use the _Tt_s_mp::find_proc method to map the
	// _sender field to the _Tt_procid object that has the same id
	// and has already been initialized.
	//
	_Tt_s_procid_ptr s_sender;
	if (! _tt_s_mp->find_proc(_sender, s_sender, 1)) {
		set_status((int)TT_ERR_PROCID);
		change_state(0, TT_FAILED, trace);
		return TT_ERR_PROCID;
	}
	_sender = s_sender;

	// check to see if the message is "local" (in our session) or
	// "nonlocal" (sent from another session)
	if (_session->address_string() == _tt_s_mp->initial_session->address_string()) {
		_flags &= ~(1<<_TT_MSG_IS_REMOTE);
	} else {
		// if the message is non-local then remap the _session
		// pointer to point to an initialized _Tt_session
		// object representing the remote session. See
		// _Tt_mp::find_session for more details.
		//
		_flags |= (1<<_TT_MSG_IS_REMOTE);


		if (TT_OK != _tt_s_mp->find_session(_session->process_tree_id(),
					            _session, 1)){
			set_status((int)TT_ERR_SESSION);
			change_state(0, TT_FAILED, trace);
			return TT_ERR_SESSION;
		}
	}
	return TT_OK;
}

// 
// Dispatches a message which means that the message is compared against
// all the ptype and otype signatures to determine whether any signatures
// match. The special case of a point-to-point message (ie. a TT_HANDLER
// message) doesn't require matching against signatures. This method is
// also responsible for verifying some message fields such as verifying
// that the _sender field is valid and setting the _TT_MSG_IS_REMOTE flag
// if the message's session field is not the same as the server's session.
// This method is only called for new messages (see _tt_rpc_dispatch and
// _tt_rpc_dispatch_2 in slib/mp_rpc_implement.C for details of how this
// method is invoked).
//
// Errors (in all cases, ::change_state(TT_FAILED) has been called):
//	TT_ERR_PROCID	unknown sender or handler
//	TT_ERR_SESSION	unknown original session
//	TT_ERR_PTYPE	unknown handler_ptype
//	TT_ERR_OTYPE	unknown otype
//	TT_ERR_SCOPE	invalid scope
// 
Tt_status _Tt_s_message::
dispatch(const _Tt_msg_trace &trace)
{
	Tt_status	result;

	//
	// This message is in TT_CREATED state so set the state to
	// TT_SENT since this is a new message.  _Tt_c_message::dispatch()
	// does the same thing on the client side.
	//
	Tt_state old_state = state();
	set_state(TT_SENT);
	result = indoctrinate( trace );
	{
		// indoctrinate() first, to update _sender
		_Tt_msg_trace state_trace( *this, old_state );
	}
	if (result != TT_OK) {
		return result;
	}

	// now dispatch the message according to its address type.
	switch (paradigm()) {
	      case TT_PROCEDURE:
		result = procedural_dispatch( trace );
		break;
	      case TT_OBJECT:
	      case TT_OTYPE:
		result = object_oriented_dispatch( trace );
		break;
	      case TT_HANDLER:
		result = handler_dispatch( trace );
		break;
	      case TT_ADDRESS_LAST:
	      default:
		result = TT_ERR_ADDRESS;
		break;
	}
	return result;
}


// 
// Called to dispatch a TT_HANDLER message. This type of message is
// special in that it contains the specific handler procid that should
// receive the message. Therefore, we just add the message to the
// procid's undelivered queue, regardless of any pattern matching. If we
// fail, we change the state of the message to TT_FAILED to notify the
// sender of the problem.
// We do check the handler\'s patterns for any matches, just to merge in
// the pattern id so any pattern callbacks can get run.
// 
Tt_status _Tt_s_message::
handler_dispatch(const _Tt_msg_trace &trace)
{
	_Tt_s_procid_ptr s_handler;
	int failed = 0;

	if (!(_tt_s_mp->find_proc(_handler, s_handler, 0))) {
		failed = 1;
	} else {
		_handler = s_handler;
		_Tt_pattern_list_cursor	pcursor(s_handler->patterns());
		int best_match = 0;
		Tt_category best_category = TT_CATEGORY_UNDEFINED;
		unsigned int best_timestamp = 0;

		// match against this procid\'s patterns, just to get the
		// best matching pattern id in the message.  This ultimately
		// allows any pattern callbacks to be run back on the client
		// side.
		while (pcursor.next()) {
			// In slib, we know they are _Tt_s_patterns
			const _Tt_s_pattern *spat = (const _Tt_s_pattern *)
				(*pcursor).c_pointer();
			if (match_handler(*spat, trace, best_match,
					  best_category, best_timestamp))
			{
				set_pattern_id( pcursor->id() );
			}
		}
		// Even if no patterns match, we still just ram the
		// point-to-point message down the handler\'s throat.
		if (!s_handler->add_message(this)) {
			failed = 1;
		}
	}
	if (failed && (scope() != TT_FILE) && (scope() != TT_BOTH)) {
		//
		// Message send failed, and since no other session
		// will get a chance to find the handler, we fail it now.
		//
		set_status((int)TT_ERR_PROCID);
		change_state(0, TT_FAILED, trace);
		return(TT_ERR_PROCID);
	} else {
		return TT_OK;
	}
}


// 
// Does the dispatch operation on a message with TT_OBJECT or TT_OTYPE
// address. It assumes the otype for the message is filled in. If this is
// a send super call then we replace the otype in the message with it's
// parent otype. Since we implement multiple-inheritance finding the
// parent otype isn't trivial so we use the
// _Tt_s_message::match_super_sig to determine which otype to use. We
// then call _Tt_s_message::procedural_dispatch to finish the dispatch
// process. 
// 
Tt_status _Tt_s_message::
object_oriented_dispatch(const _Tt_msg_trace &trace)
{
	_Tt_otype_ptr		ot;
	
	// verify that the otype field in the message is valid.
	ot = _tt_s_mp->otable->lookup(_otype);
	if (ot.is_null()) {
		// can't find otype def for this otype. This could
		// happen if the otype databases are not installed
		// properly.
		set_status((int)TT_ERR_OTYPE);
		change_state(0, TT_FAILED, trace);
		return(TT_ERR_OTYPE);
	}
	
	// if this is a send super call, we need to replace the otype
	if (_flags&(1<<_TT_MSG_IS_SUPER)) {
		_Tt_signature_ptr sig;
		if (!match_super_sig(ot, sig, trace)) {
			set_status((int)TT_ERR_OTYPE);
			change_state(0, TT_FAILED, trace);
			return(TT_ERR_OTYPE);
		}
		if (sig->super_otid().is_null()) {
			set_status((int)TT_ERR_OTYPE);
			change_state(0, TT_FAILED, trace);
			return(TT_ERR_OTYPE);
		}
		_otype = sig->super_otid();
		_flags &= ~(1<<_TT_MSG_IS_SUPER);
	}
	
	// procedural dispatch will now match against the correct
	// handler and observer signatures for this  message. Matching
	// can't be done in this function because it would cause
	// duplicate observer signatures to be matched for this otype.
	return(procedural_dispatch( trace ));
}


// 
// Compares a message against all ptype and otype signatures to determine
// if there is a match. The method _Tt_s_message::match_signatures does
// the actual work of matching the signatures and modifies the message
// accordingly if there is a match. If the message already has a handler
// ptype filled in then it is verfied to be a valid ptype. Otherwise the
// message is failed.
// 
Tt_status _Tt_s_message::
procedural_dispatch(const _Tt_msg_trace &trace)
{
	int				matched_handler = 0;
	
	
	// if handler_ptype is filled in then verify it is a valid
	// ptype. 
	if (handler_ptype().len() != 0) {
		_Tt_ptype_ptr pt;
		if (! _tt_s_mp->ptable->lookup(handler_ptype(), pt)) {
			//
			// notify sender if necessary of failed request.
			//
			set_status((int)TT_ERR_PTYPE);
			(void)change_state(0, TT_FAILED, trace);
			return(TT_ERR_PTYPE);
		}
	}
	
	// now match the relevant signatures for this matching by
	// getting a list of otype and ptype signatures that have the
	// same op field as this message.
	//
	if (_op.len()) {
		_Tt_sigs_by_op_ptr so;
		if (_tt_s_mp->sigs->lookup(_op,so)) {
			(void)match_signatures(so->sigs, trace);
		}
	}
	
	if (scope() == TT_SCOPE_NONE) {
		// dispatch hasn't set the scope of this
		// message to a valid scope so return
		// this message to sender in an undeliverable
		// state. (if it's a request)
		if (   _message_class == TT_REQUEST
		    || _message_class == TT_OFFER)
		{
			set_status((int)TT_ERR_SCOPE);
			change_state(0, TT_FAILED, trace);
		}
		return(TT_ERR_SCOPE);
	}
	
	return(TT_OK);
}


// 
// Given a set of signatures, this method iterates through the signatures
// until one is found that matches the message.
// 
// The scope, message class, op, otype and args are compared against all
// signatures in each ptype or otype. When a match is found, action
// depends on the pattern category specified:
// 
// Handle signatures:
// 
// At most one ptype should contain a signature that matches the op and
// args and specifies a handler signature. This should be enforced at
// type compile time.
// 
// If a handler ptype is found, then fill in opnum, handler_ptype, and
// reliability from the signature.
// 
// Observe signatures:
// 
// If the signature specifies queue or start, attach a "observer promise"
// record to the message, specifying the ptype and reliability options.
// This is done for all ptype that contain a matching observe signature.
//
// Returns:
//	TT_OK
//	TT_ERR_NO_MATCH	Did not match a handler
// 
Tt_status _Tt_s_message::
match_signatures(_Tt_signature_list_ptr &siglist, const _Tt_msg_trace &trace)
{
	_Tt_signature_ptr		best_sig;
	unsigned int			best_timestamp = 0;
	Tt_category			best_category = TT_CATEGORY_UNDEFINED;
	int				best_match = 0;
	_Tt_signature_list_cursor	sigC(siglist);
	Tt_disposition			sr;
	Tt_scope			sc;
	int				so;
	
	while (sigC.next()) {
		switch (sigC->category()) {
		    case TT_HANDLE:
		    case TT_HANDLE_ROTATE:
		    case TT_HANDLE_PUSH:
			if (match_handler(**sigC, trace, best_match,
					  best_category, best_timestamp))
			{
				best_sig = *sigC;
			}
			break;
		      case TT_OBSERVE:
			if (match_observer(**sigC, trace)) {
				// add an observer promise to this message
				if (_observers.is_null()) {
					_observers = new _Tt_observer_list();
				}
				so = sigC->opnum();
				sr = sigC->reliability();
				sc = sigC->scope();
				_flags |= (1<<_TT_MSG_OBSERVERS_MATCH);
				_observers->push(new _Tt_observer(sigC->ptid(),
								  so,
								  sr,
								  sc));
			}
			break;
		}
	}
	if (best_sig.is_null()) {
		return TT_ERR_NO_MATCH;
	}
	set_opnum(best_sig->opnum());
	set_handler_ptype(best_sig->ptid());
	set_reliability(best_sig->reliability());
	if (best_sig->otid().len() > 0) {
		// for obj-oriented methods,
		// fill in scope as well as
		// other fields. 
		set_scope(best_sig->scope());
	}
	return TT_OK;
}


// 
// This method is called when we want to determine the parent otype to
// use for a send super call. Since we implement multiple-inheritance we
// have to attempt a match for all the signatures in the given otype. Any
// signature that matches is returned. This signature will then have a
// super_otid field pointing to the correct parent otype.
// 
int _Tt_s_message::
match_super_sig(_Tt_otype_ptr ot, _Tt_signature_ptr &sig,
	const _Tt_msg_trace &trace)
{
	_Tt_signature_list_cursor	sigs;
	
	sigs.reset(ot->hsigs());
	while (sigs.next()) {
		int was_exact;
		if (sigs->match(scope(), _message_class, _op, _args, _otype,
				_contexts, &trace, was_exact))
		{
			sig = *sigs;
			return 1;
		}
	}
	sigs.reset(ot->osigs());
	while (sigs.next()) {
		int was_exact;
		if (sigs->match(scope(), _message_class, _op, _args, _otype,
				_contexts, &trace, was_exact))
		{
			sig = *sigs;
			return 1;
		}
	}
	return 0;
}


// 
// Returns 1 if the given procid has already been attempted as a recipient
// of this message.
// 
int _Tt_s_message::
already_tried(const _Tt_procid_ptr &proc)
{
	_Tt_procid_list_cursor		pcursor(_tried);
	
	while (pcursor.next()) {
		if (proc->is_equal(*pcursor)) {
			return(1);
		}
	}
	
	return(0);
}


//
// Load up the environment with the entries that need to be
// set when this message causes a start.
//
Tt_status _Tt_s_message::
set_start_env() const
{
	_Tt_msg_context_list_cursor	cntxtC( _contexts );

	while (cntxtC.next()) {
		if (cntxtC->isEnvEntry()) {
			if (_tt_putenv( cntxtC->enVarName(),
					cntxtC->stringRep() ) == 0)
			{
				return TT_ERR_NOMEM;
			}
		}
	}
	return TT_OK;
}


// 
// Deliver the message to a handler and make one copy per observer
// promise, which will get delivered if the original does not
// fulfill the promise.
// 
void _Tt_s_message::
deliver_to_observers_and_handlers(const _Tt_msg_trace &trace)
{
	// deliver copies of the message to any static observers. Note
	// that this is done first before invoking deliver on the
	// original message since the process of delivering the
	// message to a handler may change its state.

	if (!_observers.is_null() && _observers->count()) {
		_Tt_observer_list_cursor	observers(_observers);
		_Tt_s_message_ptr		m;
		
		while (observers.next()) {
			// make a copy of this message taking any
			// relevant fields from this observer.

			m = new _Tt_s_message(this, *observers);
			//
			// Fulfill the promise by deliver()ing this
			// copy of the message.  It will only match
			// patterns created as a result of the relevant
			// static signature.  If no such pattern exists,
			// the signature's disposition will be fulfilled.
			//
			// XXX holtz 93/09/20 It seems wasteful that
			// every pattern should be looked at just to
			// find a pattern generated by this observer
			// promise.  Why not just point from the signature
			// to the most-recent pattern instantiating
			// it?  Indeed, there is not enough info in
			// a _Tt_observer to tell which promise it is.
			//
			m->deliver(trace, 1);
		}
	}
	
	// Now invoke deliver on the original message to locate any
	// handlers and deal with any state transitions to this
	// message. 

	(void)deliver(trace, 1);
	count_ballots(_sender, trace);
}


// 
// Delivers a message to its intended recipients. This method is
// repeatedly invoked until the message is replied to or rejected in the
// case of a request or sent in the case of a notification.
// 
// This method returns 1 if the message was processed without queueing. 
// 
int _Tt_s_message::
deliver(const _Tt_msg_trace &trace, int deliver_to_observers)
{
	if (deliver_to_observers) {
		// We mark set the _when_last_matched to be the current
		// db key. See _Tt_s_message::change_state to see how
		// it is used.

		_when_last_matched = _tt_s_mp->now;
	}

	// Match the message against all relevant patterns, which
	// means, all the patterns that contain an op field that
	// matches the op field for this message or else patterns that
	// don't specify an op field. If most patterns do contain an
	// op field then we don't incur a linear scan of all the
	// patterns.
	_Tt_pattern_list_ptr		pats2match;
	_Tt_s_pattern_ptr		best_pattern;
	_Tt_procid_ptr			handler_procid;
	_Tt_s_procid_ptr		dummy;
	int				found_observer = 0;
	int				best_match = 0;

	_Tt_patlist_ptr opful_pats = _tt_s_mp->opful_pats->lookup(_op);
	if (opful_pats.is_null()) {
		pats2match = _tt_s_mp->opless_pats;
	} else {
		if (_tt_s_mp->opless_pats->count() > 0) {
			//
			// XXX The price you pay for opless patterns:
			// we create a new list and copy both lists into it.
			//
			pats2match =
				new _Tt_pattern_list( *_tt_s_mp->opless_pats );
			pats2match->append( opful_pats->patterns );
		} else {
			pats2match = opful_pats->patterns;
		}
	}
	if (! pats2match.is_null()) {
		found_observer = match_patterns( pats2match,
						  trace, best_pattern,
						  deliver_to_observers);
	}
	if (! best_pattern.is_null()) {
		handler_procid = best_pattern->procid();
		if (best_pattern->category() == TT_HANDLE_ROTATE) {
			best_pattern->set_timestamp( _tt_s_mp->now );
		}
	}
	
	
	// now we're done pattern matching so we try to deliver the
	// message to any handlers or deal with the case where nobody
	// handled the message (in the case of requests) or observed
	// the message (in the case of notifications).
	
	if (! is_handler_copy()) {
		// message represents an observer promise
		if (found_observer) {
			// promise will be fulfilled
			return(1);
		} else {
			handle_no_recipients( trace );
			return 0;
		}
	}  else if (! handler_procid.is_null()) {
		// a handler was found for the message
		// deliver to handler
		set_handler_procid(handler_procid);
		// always give requests to handlers in
		// TT_SENT state.
		set_state(TT_SENT);
		if (! ((_Tt_s_procid *)handler_procid.c_pointer())->add_message(this)) {
			// rematch because this
			// procid can't receive messages
			//
			// XXX: rather than doing a recursive
			// call, keep a list of the best matches
			// in sorted order and just go down the list
			// until one of them succeeds.
			return(deliver(trace, 0));
		} else {
			return(1);
		}
	} else if (   (paradigm() == TT_HANDLER)
		   && (! _handler.is_null())
		   && (_tt_s_mp->find_proc(_handler, dummy, 0)))
	{
		//
		// XXX Do nothing; the message was already delivered
		// in handler_dispatch().  handler_dispatch() should
		// probably set the state, or better yet, let the
		// delivery happen here instead of there.  This
		// is really gross.
		//
	} else if ((_message_class == TT_REQUEST &&
		    (_state == TT_SENT ||
		     _state == TT_CREATED ||
		     _state == TT_STARTED ))
		   ||
		   (   (   _message_class == TT_NOTICE
			|| _message_class == TT_OFFER)
		    && (start() || queue()))
		   ||
		   (paradigm() == TT_HANDLER)) {

		// Either this message is a request in a non-final
		// state, or it is a notice that needs to be queued or
		// started for a static handler, or it is a point-to-point
		// message whose handler is not in this session

		handle_no_recipients( trace );
		return(0);
	}

	return(1);
}


// 
// Matches the message against each pattern in "patterns". Uses the
// methods _Tt_s_message::match_handler and _Tt_s_message::match_observer
// to match handler and observer patterns respectively.
// best_pattern is set to the best handler pattern that matched.
// 
// If "deliver_to_observers" is 1 then delivery is attempted for observer
// patterns.
// 
int _Tt_s_message::
match_patterns(_Tt_pattern_list_ptr &patterns, const _Tt_msg_trace &trace,
	       _Tt_pattern_ptr &best_pattern, int deliver_to_observers)
{
	int			found_observer = 0;
	unsigned int		best_timestamp = 0;
	Tt_category		best_category = TT_CATEGORY_UNDEFINED;
	int			best_match = 0;
	_Tt_pattern_list_cursor	pcursor(patterns);

	//
	// Point-to-point messages aren't pattern-matched.
	//
	if (paradigm() == TT_HANDLER) {
		return 0;
	}
	while (pcursor.next()) {
		_Tt_s_procid_ptr registrant = (_Tt_s_procid *)
			pcursor->procid().c_pointer();
		if (registrant.is_null()) {
			return(0);
		}
		if (! registrant->is_active()) {
			return(0);
		}
		const _Tt_s_pattern *spat;
		switch (pcursor->category()) {
		      case TT_HANDLE:
		      case TT_HANDLE_ROTATE:
		      case TT_HANDLE_PUSH:
			if (! is_handler_copy()) {
				// Can only handle original, not copies
				continue;
			}
			if (   (_flags&(1<<_TT_MSG_OBSERVERS_ONLY))
			    || state() != TT_SENT)
			{
				// Not looking for a handler
				continue;
			}
			if (  (! _tried.is_null()) && (_tried->count() > 0)
			    && already_tried(registrant))
			{
				// You had your chance, bub
				continue;
			}
			// In slib, we know they are _Tt_s_patterns
			spat = (const _Tt_s_pattern *)(*pcursor).c_pointer();
			if (match_handler(*spat, trace, best_match,
					 best_category, best_timestamp))
			{
				best_pattern = *pcursor;
			}
			break;
		      case TT_OBSERVE:
			// In slib, we know they are _Tt_s_patterns
			spat = (const _Tt_s_pattern *)(*pcursor).c_pointer();
			if (deliver_to_observers) {
				// XXX: duplicates might get delivered in the
				// case of file-scope messages. This needs to
				// be fixed!
				found_observer += match_observer(*spat,
							registrant, trace);
			}
			break;
		      default:
			continue;
		}
	}
	if (! best_pattern.is_null()) {
		set_pattern_id( best_pattern->id() );
		_tt_s_mp->now++;
	}
	return found_observer;
}

static int
_tt_excludes(Tt_category best_category, Tt_category curr_category)
{
	switch (curr_category) {
	    case TT_HANDLE:
		if (best_category == TT_HANDLE_ROTATE) {
			return 1;
		}
		// fall through
	    case TT_HANDLE_ROTATE:
		if (best_category == TT_HANDLE_PUSH) {
			return 1;
		}
	}
	return 0;
}

static int
_tt_excludes(Tt_category best_category, int best_match,
	     unsigned int best_timestamp,
	     Tt_category category, int score, int timestamp)
{
	switch (category) {
	    case TT_HANDLE_PUSH:
		switch (best_category) {
		    case TT_HANDLE_PUSH:
			if (best_match > score) {
				return 0;
			}
			if (   (best_match == score)
			    && (best_timestamp >= timestamp))
			{
				// Newest PUSH pattern wins
				return 1;
			}
			break;
		    case TT_HANDLE_ROTATE:
		    case TT_HANDLE:
		    default:
			if (score <= 0) {
				// Matching PUSH pattern trumps all
				return 1;
			}
			break;
		}
		break;
	    case TT_HANDLE_ROTATE:
		switch (best_category) {
		    case TT_HANDLE_ROTATE:
			if (best_match > score) {
				return 1;
			}
			if (   (best_match == score)
			    && (best_category == TT_HANDLE_ROTATE)
			    && (best_timestamp < timestamp))
			{
				// Coldest ROTATE pattern wins
				return 1;
			}
			break;
		    case TT_HANDLE:
			if (score <= 0) {
				// Matching ROTATE trumps HANDLE
				return 1;
			}
			break;
		}
		break;
	    case TT_HANDLE:
		// best_category is either TT_HANDLE or unset
		if (best_match >= score) {
			return 1;
		}
	}
	return 0;
}


// 
// Match the message against the given handler pattern. "best_match"
// points to the current best matching score for this message. If "pat"
// gets a higher score when matching this message than "best_match" then
// this method returns 1 and sets "best_match" to the new best score.
// Otherwise, 0 is returned.
// 
int _Tt_s_message::
match_handler(const _Tt_s_pattern &pat, const _Tt_msg_trace &trace,
	int &best_match, Tt_category &best_category,
	unsigned int &best_timestamp) const
{
	if (_tt_excludes(best_category, pat.category())) {
		return 0;
	}
	int score = pat.match(*this, trace);
	if (score <= 0) {
		return 0;
	}
	if (_tt_excludes(best_category, best_match, best_timestamp,
			 pat.category(), score, pat.timestamp()))
	{
		return 0;
	}
	best_match = score;
	best_category = pat.category();
	best_timestamp = pat.timestamp();
	return 1;
}

int _Tt_s_message::
match_handler(const _Tt_signature &sig, const _Tt_msg_trace &trace,
	int &best_match, Tt_category &best_category,
	unsigned int &best_timestamp) const
{
	if (_tt_excludes(best_category, sig.category())) {
		return 0;
	}
	int was_exact;
	int score = sig.match(scope(), _message_class, _op,
			      _args, _otype, _contexts, &trace, was_exact);
	if (score <= 0) {
		return 0;
	}
	if (_tt_excludes(best_category, best_match, best_timestamp,
			 sig.category(), score, sig.timestamp()))
	{
		return 0;
	}
	best_match = score;
	best_category = sig.category();
	best_timestamp = sig.timestamp();
	return 1;
}

// 
// Match the message against the given observer pattern. "proc" points
// to the procid that registered the pattern. If the pattern matches,
// then this message is immediately put on
// "proc"'s undelivered messages queue using the
// _Tt_s_procid::add_message method.
//
int _Tt_s_message::
match_observer(const _Tt_s_pattern &pat, const _Tt_s_procid_ptr &proc,
	       const _Tt_msg_trace &trace)
{
	// match against observer patterns
	if (pat.match(*this, trace) > 0) {
		_Tt_s_message_ptr mcopy;
		
		// if there is no observer field then we have to
		// create a copy. Otherwise this already is a copy.
		if (_observer.is_null()) {
			mcopy = new _Tt_s_message(this, _observer);
		} else {
			mcopy = this;
		}
		mcopy->set_pattern_id(pat.id());
		if (proc->add_message(mcopy)) {
			_flags |= (1<<_TT_MSG_OBSERVERS_MATCH);
			return(1);
		}
	}
	
	return(0);
}

int _Tt_s_message::
match_observer(const _Tt_signature &sig, const _Tt_msg_trace &trace)
{
	int was_exact;
	return sig.match(scope(), _message_class, _op,
			 _args, _otype, _contexts, &trace, was_exact) >= 0;
}


//
// Delegate responsibility for finding a handler to the sessions in
// _rsessions.  Returns:
//	TT_OK		successful delegation
//	TT_ERR_SESSION	no more sessions
//
Tt_status _Tt_s_message::
hdispatch()
{
	_Tt_message_ptr		m = this;
	_Tt_string_list_cursor	rc( _rsessions );
	_Tt_string		init_rc;
	_Tt_session_ptr		rs;

	while (rc.next()) {
		init_rc = *rc;
		rc.remove();
		if (_tt_s_mp->initial_session->address_string() == init_rc) {
			continue;
		}
		if (TT_OK != _tt_s_mp->find_session(init_rc,rs,1)) {
			continue;
		}
		Tt_status status = rs->call(TT_RPC_HDISPATCH,
					    (xdrproc_t)tt_xdr_message,
					    (char *)&m,
					    (xdrproc_t)xdr_void,
					    (char *)0);
		if (status == TT_OK) {
			//
			// Flush the list of remote sessions, so that
			// we will not retry these sessions if
			// responsibility gets bucked back around to us.
			// [See _tt_rpc_hupdate_msg().]
			//
			_rsessions->flush();
			return TT_OK;
		}
	}
	return TT_ERR_SESSION;
}


// 
// This method is invoked when disposition options for the message need
// to be honored either because no handler was found for the message or
// there is a static observer that wants start or queue reliability for
// this message.
// 
// The general mechanism is to use _Tt_s_message::change_state to change
// the state of the message to TT_STARTED or TT_QUEUED as appropiate. The
// exception is for file-scoped messages which are handled somewhat
// differently. If we are in the native session
// then we first attempt to forward the message to one
// of the remote sessions mentioned in the _rsessions field in the
// message (this field is set on the client side in the
// _Tt_c_message::dispatch method). If none of the remote sessions
// takes responsibility for the message, or ! try_rsessions, then we
// honor reliability options for the message as if were a
// non-file-scoped message.
// 
void _Tt_s_message::
handle_no_recipients(const _Tt_msg_trace &trace)
{
	if (_flags&(1<<_TT_MSG_OBSERVERS_ONLY)) {
		//
		// Disposition is not performed during inter-scope
		// observation phase.  Disposition is only performed
		// by the native session, during handler-dispatch phase.
		//
		return;
	}
	if (   ((scope()==TT_FILE) || (scope()==TT_BOTH))
	    && is_handler_copy())
	{
		if (hdispatch() == TT_OK) {
			// We have passed the buck.
			return;
		}
		if (_flags&(1<<_TT_MSG_IS_REMOTE)) {
			//
			// We are the last session tried, so
			// fail it, even if it is a notice.
			// Native session will perform disposition.
			//
			set_status((int)TT_ERR_NO_MATCH);
			change_state(0, TT_FAILED, trace);
			return;
		}
	}

	// We're here if the message is non-file-scoped or if no
	// remote session would take responsibility for the message

	if (start() || queue()) {
		int	reliability_attempted = 0;

		// if the message has start disposition then change its state
		// to TT_STARTED.
		
		// rfm 28 Sept 1992: Used to bypass this if state had
		// already been through TT_STARTED, apparently on
		// the theory that we didn\'t need to do this again.
		// But, there is a legitimate reason to be redelivering
		// messages already in TT_STARTED state, if they were
		// blocked on the ptype waiting for proc_replied
		// to happen. In that case the extra check
		// that caused the code at the end of this
		// block to see reliablity_attempted as 0 and conclude
		// that the message should be failed for lack of
		// a handler.

		if (start()) {
			if (change_state(0, TT_STARTED, trace) != TT_OK) {
				// now set reliability to TT_DISCARD
				// because starting can't succeed
				// for this message.
				set_reliability(TT_DISCARD);
				set_status((int)TT_ERR_PTYPE_START);
				change_state(0, TT_FAILED, trace);
				return;
			} else {
				reliability_attempted = 1;
			}
		}

		// if the message has queue disposition and it hasn't
		// gone to the TT_QUEUED state then change its state
		// to TT_QUEUED.

		// XXX: I don\'t think this has the same problem as above,
		// since once a process declares a ptype it gets all the
		// queued messages offered to it.  If the process
		// doesn\'t handle it, requeuing it is pointless \(while
		// doing a start is meaningful, the process might be designed
		// to start, do one message, and quit.\)  \[ Queued
		// requests are kind of silly anyway.\]  rfm 28 Sept 1992
		if (queue() && !(_state_reported&(1<<TT_QUEUED))) {
			if (change_state(0, TT_QUEUED, trace) != TT_OK) {
				change_state(0, TT_FAILED, trace);
				return;
			} else {
				reliability_attempted = 1;
			}
		}
		// if we didn't honor reliability options then we fail
		// the message with the appropiate "no match found"
		// status code.
		if (! reliability_attempted) {
			set_status((int)TT_ERR_NO_MATCH);
			change_state(0, TT_FAILED, trace);
		}
	} else if (is_handler_copy()) {
		set_status((int)TT_ERR_NO_MATCH);
		change_state(0, TT_FAILED, trace);
	}
}


// 
// This method is invoked to determine if any of the static observers of
// this message require file-scope queueing.  For each static observer
// that requires queueing we add the ptype that requires the message
// queued to the _Tt_qmsg_info data structure.
// 
void _Tt_s_message::
qmsg_info(_Tt_qmsg_info_ptr &qm)
{
	// XXX what about TT_BOTH? Do we queue them?
	if (_scope == TT_FILE && queue() && _handler_ptype.len()) {
		// XXX: return null list since file-scope queued
		// requests are not implemented.

		qm = (_Tt_qmsg_info *)0;
		return;
	}
	if (!_observers.is_null() && _observers->count() > 0) {
		_Tt_observer_list_cursor	observers(_observers);
		
		while (observers.next()) {
			if (observers->scope() == TT_FILE &&
			    observers->reliability() & TT_QUEUE) {
				if (qm.is_null()) {
					qm = new _Tt_qmsg_info;
					qm->m_id = _id;
					qm->sender = _sender->id();
				}
				qm->categories->push(new _Tt_int_rec((int)TT_OBSERVE));
				qm->ptypes->push(observers->ptid());
			}
		}
	}
}


// 
// Removes any reference to the given procid contained in this message.
// This is typically done when the procid has exited and we want to
// remove any reference to it in the system so its storage can be
// reclaimed. 
// 
void _Tt_s_message::
remove_procid(const _Tt_procid_ptr &p)
{
	if (! _tried.is_null() && _tried->count()) {
		_Tt_procid_list_cursor		procs;
		
		procs.reset(_tried);
		while (procs.next()) {
			if (procs->is_equal(p)) {
				procs.remove();
				break;
			}
		}
	}
}


// 
// Returns the scope of the message (returns observer's scope if
// present). 
// 
Tt_scope _Tt_s_message::
scope() const
{
	if (_observer.is_null()) {
		return(_scope);
	} else {
		return(_observer->scope());
	}
}


// 
// Returns the opnum of the message or the opnum of its static observer
// if present.
// 
int _Tt_s_message::
opnum() const
{
	if (_observer.is_null()) {
		return(_opnum);
	} else {
		return(_observer->opnum());
	}
}


// 
// Returns the message's handler_ptype or the ptype of the static
// observer if present.
// 
const _Tt_string & _Tt_s_message::
handler_ptype() const
{
	if (_observer.is_null()) {
		return(_handler_ptype);
	} else {
		return(_observer->ptid());
	}
}


// 
// Returns the message's reliability field (which represents the
// disposition of the message.). If this message has a static observer
// then the disposition of the observer is returned.
// 
Tt_disposition _Tt_s_message::
reliability() const
{
	if (_observer.is_null()) {
		return(_reliability);
	} else {
		return(_observer->reliability());
	}
}


// 
// Sets the state of a message. If the value is not the default TT_SENT
// value then the appropiate field in _full_msg_guards is turned on. If
// the message has an attached static observer then we just set the state
// for the observer.
// 
Tt_status _Tt_s_message::
set_state(Tt_state state)
{
	if (_observer.is_null()) {
		_state = state;
		SET_GUARD(_full_msg_guards,_state != TT_SENT,_TT_MSK_STATE);
	} else {
		_observer->set_state(state);
	}
	
	return(TT_OK);
}


// 
// Sets the scope of a message. If the value is not the default
// TT_SESSION value then we turn on the appropiate field in
// _full_msg_guards. If there is an attached static observer then we just
// set the scope for the observer.
// 
Tt_status _Tt_s_message::
set_scope(Tt_scope s)
{
	if (_observer.is_null()) {
		_scope = s;
		if (_tt_global->xdr_version() > 1) {
			// version 1 tooltalk clients didn't set the
			// default scope properly so always send them
			// the scope value.
			//
			SET_GUARD(_full_msg_guards,
				  _scope != TT_SESSION,_TT_MSK_SCOPE);
		}
	} else {
		_observer->set_scope(s);
	}
	
	return(TT_OK);
}


// 
// Sets the reliability (disposition) of a message. If the value is
// not the default TT_DISCARD value then we turn on the appropiate
// field in _full_msg_guards. If there is an attached static observer
// then we just set the disposition for the observer.
// 
Tt_status _Tt_s_message::
set_reliability(Tt_disposition r)
{
	if (_observer.is_null()) {
		_reliability = r;
		SET_GUARD(_full_msg_guards,
			  _reliability != TT_DISCARD, _TT_MSK_RELIABILITY);
	} else {
		_observer->set_reliability(r);
	}
	
	return(TT_OK);
}


// 
// Sets the state of the message or the state of the static observer if
// present. 
// 
Tt_state _Tt_s_message::
state() const
{
	if (_observer.is_null()) {
		return(_state);
	} else {
		return(_observer->state());
	}
}


void _Tt_s_message::
add_eligible_voter(const _Tt_procid_ptr &)
{
	if (_message_class != TT_OFFER) {
		return;
	}
	_Tt_s_message_ptr orig = this;
	while (! orig->_original.is_null()) {
		orig = orig->_original;
	}
	orig->_num_recipients_yet_to_vote++;
}


Tt_status _Tt_s_message::
add_voter(const _Tt_procid_ptr &voter, Tt_state vote,
	  const _Tt_msg_trace &trace)
{
	if (_message_class != TT_OFFER) {
		return TT_OK;
	}
	switch (vote) {
	    case TT_ACCEPTED:
	    case TT_REJECTED:
	    case TT_ABSTAINED:
		break;
	    default:
		// Not a vote; bail out.
		return TT_OK;
	}
	if (! voter->processing( *this )) {
		return TT_OK;
	}
	//
	// This is probably just a copy given to an observer.
	// Backtrack to the sender's original copy, which we
	// save on the sender's _delivered list.
	//
	_Tt_s_message_ptr orig = this;
	while (! orig->_original.is_null()) {
		orig = orig->_original;
	}
	Tt_status status = orig->_Tt_message::add_voter( voter, vote );
	if (status != TT_OK) {
		return status;
	}
	orig->_num_recipients_yet_to_vote--;
	count_ballots( voter, trace );
	return TT_OK;
}


Tt_status _Tt_s_message::
count_ballots(const _Tt_procid_ptr &last_voter, const _Tt_msg_trace &trace)
{
	if (_message_class != TT_OFFER) {
		return TT_OK;
	}
	//
	// This is probably just a copy given to an observer.
	// Backtrack to the sender's original copy, which we
	// save on the sender's _delivered list.
	//
	_Tt_s_message_ptr orig = this;
	while (! orig->_original.is_null()) {
		orig = orig->_original;
	}
	if (orig->_num_recipients_yet_to_vote <= 0) {
		orig->change_state( last_voter, TT_RETURNED, trace );
	}
	return TT_OK;
}
