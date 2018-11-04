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
//%%  $XConsortium: mp_s_procid.C /main/3 1995/10/23 11:58:39 rswiston $ 			 				
/* %w% @(#)
 *
 * mp_s_procid.cc
 *
 * Copyright (c) 1990, 1992 by Sun Microsystems, Inc.
 */
#include <fcntl.h>
#include "tt_options.h"
#include "mp_s_file.h"
#include "mp_s_message.h"
#include "mp_s_mp.h"
#include "mp/mp_mp.h"
#include "mp_s_pattern.h"
#include "mp_s_procid.h"
#include "mp_ptype.h"
#include "mp_rpc_implement.h"
#include "mp_s_session.h"
#include "mp/mp_stream_socket.h"
#include "mp/mp_trace.h"
#include "util/tt_int_rec.h"
#include "util/tt_global_env.h"
#include "util/tt_xdr_version.h"
#include "util/tt_host.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "mp_signature.h"
#include <arpa/inet.h>

_Tt_s_procid::
_Tt_s_procid()
{
	_itimeout = -1;
}

_Tt_s_procid::
_Tt_s_procid(const _Tt_s_procid_ptr &p)
{

	_proc_host_ipaddr = p->_proc_host_ipaddr;
	_pid = p->_pid;
	_id = p->_id;
	_itimeout = -1;
}

_Tt_s_procid::
_Tt_s_procid(const _Tt_procid_ptr &p)
{
	// For some reason we can't access the protected members like
	// p->_proc_host_ipaddr, even though we're in a member function
	// of a class which inherits from _Tt_procid.
	_proc_host_ipaddr = p->proc_host_ipaddr();
	_pid = p->pid();
	_id = p->id();
	_itimeout = -1;
}


_Tt_s_procid::
~_Tt_s_procid()
{
}


//
// Adds a message to the undelivered queue of a procid. This will signal
// the process and then if successful add the message to the queue.
// Note that, unless OPT_ADDMSG_DIRECT is enabled, the message isn't
// actually sent to the procid until the procid invokes the
// tt_message_receive api call.
//
int _Tt_s_procid::
add_message(const _Tt_s_message_ptr &m)
{
	_Tt_xdr_version			xvers(_version);

	if (! _flags&(1<<_TT_PROC_ACTIVE)) {
		_tt_syslog(0, LOG_ERR,
			   catgets(_ttcatd, 2, 4,
				   "A ToolTalk client died before it "
				   "could be signalled to retrieve a "
				   "message it was due"));
		return(0);
	}

	if (   (_version < TT_OFFER_RPC_VERSION)
	    && (m->message_class() == TT_OFFER))
	{
		// Keep this in sync with ::update_message()
		_Tt_msg_trace trace( *m, *this ); // XXX White lie
		// We are ignorant of TT_OFFERs, so automatically abstain
		m->change_state( this, TT_ABSTAINED, trace );
		return 1;
	}

	// initialize _undelivered list if necessary. Otherwise, check
	// if this message is already in the _undelivered list. This
	// can happen if the message has changed state before this
	// procid got around to retrieving its messages.

	if (_undelivered.is_null()) {
		_undelivered = new _Tt_message_list();
	} else if (_undelivered->count() > 0) {
		_Tt_message_list_cursor	mc(_undelivered);
		int			in_queue = 0;

		while (mc.next()) {
			if ((*mc).is_eq(m)) {
				// message already in queue. remove
				// message from its current position
				// to preserve message ordering.
 
				mc.remove();
				in_queue = 1;
			}
		}
		if (in_queue) {
			// message was already on the undelivered
			// queue (but in an earlier state). No need to
			// signal the procid. Just append the message
			// to the end of the undelivered queue.
			//
			_undelivered->append(m);
			_Tt_msg_trace trace( *m, *this );
			return(1);
		}
	}

	const char *error_string =
		catgets(_ttcatd, 2, 5,
			"Connection to ToolTalk client "
			"lost while signalling it to retrieve a message");

#ifdef OPT_ADDMSG_DIRECT
	// for post-version 2 clients, if this option is turned on we
	// use the _Tt_s_procid::signal_new_message(_Tt_message_ptr)
	// method to send the message directly down the signalling
	// channel. If this procedure fails then we invoke the
	// _Tt_s_procid::set_active method with a 0 argument to
	// deactivate this procid.
	//
	if (_version >= TT_ADDMSG_VERS) {
		if (signal_new_message(m) == TT_OK) {
			_Tt_msg_trace trace( *m, *this );
			return(1);
		} else {
			_tt_syslog(0, LOG_ERR, error_string );
			set_active(0);
			return(0);
		}
	}
	// note that if _version < TT_ADDMSG_VERS we fall through the
	// ifdef to signal the message in the normal way.

#endif				// OPT_ADDMSG_DIRECT
	//
	// Signal this procid that it has a new message.
	// We append first, because _Tt_self_procid::signal_new_message()
	// actually processes _undelivered.
	//
	_undelivered->append(m);
	// XXX what if this observation does not count e.g. TT_STARTED?
	m->add_eligible_voter( this );
	_Tt_msg_trace trace( *m, *this );
	if  (signal_new_message() == TT_OK) {
		set_timeout_for_message(*m);
		return(1);
	}
	// we're here if we failed to deliver the message
	_tt_syslog(0, LOG_ERR, error_string );
	return(0);
}


// 
// Adds a pattern to the list of patterns registered on behalf of a
// procid. This pattern may have been generated from a ptype/otype
// signature or it may be a dynamic pattern. In either case, we update
// the table of patterns (which maps op names to lists of patterns). If
// the pattern is file-scoped then we tell the _tt_s_mp object that there
// is a new pattern for the file(s) mentioned in this pattern. This is
// used to notify clients that send a file-scoped message to a session
// whether or not they should remove the session from the file's joined
// sessions (ie. this session was stale because some client exited
// without quitting the file).
//
Tt_status _Tt_s_procid::
add_pattern(const _Tt_s_pattern_ptr &p)
{

	// check to see if the pattern contains the current session id
	// in its sessions list. If it does then we set a special flag
	// in the pattern. This flag is an optimization for
	// session-scoped patterns since it very quickly tells us
	// whether the pattern should match a session-scoped message
	// (in contrast to comparing the session id every time).

	_Tt_string_list_cursor		sessions(p->sessions());
	while (sessions.next()) {
		if (_tt_s_mp->initial_session->has_id(*sessions)) {
			p->set_in_session();
			break;
		}
	}


	// if this is a duplicate pattern then return an error

	if (! _patterns.is_null()) {
		_Tt_pattern_list_cursor	patc(_patterns);

		while (patc.next()) {
			if (p->id() == patc->id()) {
				return TT_ERR_INVALID;
			}
		}
	}

	if (   (p->scopes() & (1<<TT_FILE) || p->scopes() & (1<<TT_BOTH))
	    && (   p->category() == TT_HANDLE_PUSH
		|| p->category() == TT_HANDLE_ROTATE))
	{
		return TT_ERR_UNIMP;
	}
		
	// we set the pattern's owner to this procid.
	_Tt_procid_ptr		pr(this);

	p->set_procid(pr);


	// Increment the server's clock.
	_tt_s_mp->now++;

	// if this is an observer pattern then we increment the global
	// timestamp of observer patterns. This timestamp is used as
	// an optimization (see _Tt_s_message::needs_observer_match)
	// to avoid unnecessary observer pattern matching.
	switch (p->category()) {
	    case TT_OBSERVE:
		_tt_s_mp->when_last_observer_registered = _tt_s_mp->now;
		break;
	    case TT_HANDLE_PUSH:
		p->set_timestamp( _tt_s_mp->now );
		break;
	}

	// invoke the set_active method to activate this procid.
	// XXX: this may be unnecessary
	set_active(1);

	if (_patterns.is_null()) {
		_patterns = new _Tt_pattern_list();
	}


	// if this pattern doesn't have an op field then we add it to
	// the global list of patterns with no ops. These patterns
	// can't take advantage of the optimization of hashing
	// patterns based on their op field so each message has to be
	// matched against each one of these patterns.

	if (p->ops()->count() == 0) {
		_tt_s_mp->opless_pats->push(p);
	} else {
		// we're here if this pattern does contain an op field
		// in which case we add it to the table of patterns
		// hashed on the op field. This greatly reduces the
		// number of patterns that need to be examined for a
		// given message since typically patterns contain
		// operation names. 

		_Tt_patlist_ptr		po;
		_Tt_string_list_cursor	ops(p->ops());
		while (ops.next()) {
			if (! _tt_s_mp->opful_pats->lookup(*ops,po)) {
				po = new _Tt_patlist();
				po->set_op(*ops);
				po->patterns = new _Tt_pattern_list();
				_tt_s_mp->opful_pats->insert(po);
			}
			po->patterns->push(p);
		}
	}
	// add the pattern to the _patterns field. This field is used
	// to keep a record of which patterns this procid has
	// registered in order to allow for easy iteration over this
	// patterns to process join/quit_session/file requests.
	_patterns->push(p);


	// if the pattern we're adding will cause the current
	// session to be written in the file scope record for this
	// file then we add it to the cache of files.
	int scopes = p->scopes();

	if (scopes&(1<<TT_FILE) || scopes&(1<<TT_BOTH)) {
		_Tt_string_list_cursor	files(p->files());

		while (files.next()) {
			_tt_s_mp->mod_file_scope(*files, 1);
		}
	}

	return(TT_OK);
}


// 
// Deletes a pattern from a procid. This means that the pattern has to
// be deleted from either the global pats_with_no_op list or the
// pattern table. In addition, if the pattern is file-scoped then we
// update the global table of files to number of patterns registered
// for the file. Note that this method is not itself responsible for
// deleting the pattern from the _patterns list. That is done by
// either _Tt_s_procid::del_pattern(_Tt_string) for deletion of a
// specific pattern at the request of a client or by
// _Tt_s_procid::set_active when it has been determined that this
// procid has exited on the client side.
//  
void _Tt_s_procid::
del_pattern(_Tt_pattern_ptr &p)
{
	_Tt_pattern_list_cursor		pc;

	if (p->ops()->count() == 0) {
		// if the pattern doesn't contain op fields then we
		// remove it from the global list of patterns with no
		// op. 

		pc.reset(_tt_s_mp->opless_pats);
		while (pc.next()) {
			if (pc->id() == p->id()) {
				pc.remove();
			}
		}
	} else {
		// delete the pattern from the table of patterns with
		// ops. 

		_Tt_patlist_ptr			po;
		_Tt_string_list_cursor		ops(p->ops());

		while (ops.next()) {
			po = (_Tt_patlist *)0;
			po = _tt_s_mp->opful_pats->lookup(*ops);
			if (! po.is_null()) {
				pc.reset(po->patterns);
				while (pc.next()) {
					if (pc->id() == p->id()) {
						pc.remove();
					}
				}
				if (0==po->patterns->count()) {
					_tt_s_mp->opful_pats->remove(*ops);
				}
			}
		}
	}

	// if the pattern we're deleting would have caused the current
	// session to be written in the file scope record for this
	// file then we delete it from the cache of files.
	int scopes = p->scopes();

	if (scopes&(1<<TT_FILE) || scopes&(1<<TT_BOTH)) {
		_Tt_string_list_cursor	files(p->files());

		while (files.next()) {
			_tt_s_mp->mod_file_scope(*files, 0);
		}
	}
}


// 
// Deletes the pattern with the given id from this procid. Uses
// _Tt_s_procid::del_pattern(_Tt_pattern_ptr) to do the actual work.
// 
Tt_status _Tt_s_procid::
del_pattern(_Tt_string id)
{
	if (! _patterns.is_null()) {
		_Tt_pattern_list_cursor		patc(_patterns);

		while (patc.next()) {
			if (patc->id() == id) {
				del_pattern(*patc);
				patc.remove();
				return(TT_OK);
			}
		}
	}
		
	return(TT_WRN_NOTFOUND);
}


// 
// Registers patterns derived from the given ptype for this procid. If
// the ptype is not found in the table of ptypes then an error is
// returned. 
//
Tt_status _Tt_s_procid::
declare_ptype(_Tt_string ptid)
{
	Tt_status	status;
	_Tt_ptype_ptr	ptype;

	if (! _tt_s_mp->ptable->lookup(ptid,ptype)) {
		return(TT_ERR_PTYPE);
	}
	status = add_signature_patterns(ptype, TT_HANDLE);
	if (status ==  TT_OK) {
		status = add_signature_patterns(ptype, TT_OBSERVE);
	}
	if (status == TT_OK) {
		if (_declared_ptypes.is_null()) {
			_declared_ptypes = new _Tt_string_list();
		}
		_declared_ptypes->push(ptid);

		// XXX: do we really need to call pattern_added here?
		// These patterns don't have the session id in them so
		// only file-scope patterns could potentially match at
		// this point.

		_tt_s_mp->initial_s_session->pattern_added();
	}
	return(status);
}


// 
// Removes patterns derived from the given ptype for this procid. If
// the ptype is not found in the table of ptypes then an error is
// returned. 
//
Tt_status _Tt_s_procid::
undeclare_ptype(_Tt_string ptid)
{
	Tt_status		status;
	_Tt_ptype_ptr		ptype;

	ptype = _tt_s_mp->ptable->lookup(ptid);
	if (ptype.is_null()) {
		return(TT_ERR_PTYPE);
	}
	status = remove_signature_patterns(ptype);
	if (status == TT_OK) {
		if (!_declared_ptypes.is_null()) {
			_Tt_string_list_cursor c(_declared_ptypes);
			while (c.next()) {
				if (*c == ptid) {
					c.remove();
				}
			}
		}

	}
	return(status);
}

// 
// Checks to see if the ptype is found in the table of ptypes.
//
Tt_status _Tt_s_procid::
exists_ptype(_Tt_string ptid)
{
	Tt_status		status;
	_Tt_ptype_ptr			ptype;

	ptype = _tt_s_mp->ptable->lookup(ptid);
	if (ptype.is_null()) {
		status = TT_ERR_PTYPE;
	} else {
		status = TT_OK;
	}
	return(status);
}

// 
// Unblock and deliver any messages being queued up until the caller
// has finished initializing after receiveing the first message,
// the start message.
//
Tt_status _Tt_s_procid::
unblock_ptype(_Tt_string ptid)
{
	_Tt_ptype_ptr		ptype;

	ptype = _tt_s_mp->ptable->lookup(ptid);
	if (ptype.is_null()) {
		return(TT_ERR_PTYPE);
	}
	ptype->proc_replied();

	return(TT_OK);
}


// 
// Adds patterns corresponding to the handler or observe signatures in
// ptype. category determines which list of signatures is being
// considered. 
// 
Tt_status _Tt_s_procid::
add_signature_patterns(_Tt_ptype_ptr &ptype, Tt_category category)
{
	_Tt_s_pattern_ptr			pat;
	_Tt_signature_list_cursor	sigC;
	Tt_status			status;
	
	sigC.reset((category != TT_OBSERVE) ?
			 ptype->hsigs() : ptype->osigs());
	while (sigC.next()) {
		// The _Tt_s_pattern(_Tt_signature_ptr) constructor
		// constructs the proper pattern given this signature. 
		pat = new _Tt_s_pattern(*sigC);

		pat->set_category(sigC->category());
		pat->generating_ptype(ptype);
		if ((status = add_pattern(pat)) != TT_OK) {
			return(status);
		}
	}
	return(TT_OK);
}

// 
// Removes patterns generated from the signatures in ptype. 
// 
Tt_status _Tt_s_procid::
remove_signature_patterns(_Tt_ptype_ptr &ptype)
{
	_Tt_s_pattern_ptr		pat;
	_Tt_pattern_list_cursor		patterns;
	
	patterns.reset(_patterns);
	_Tt_s_pattern_ptr ptn;

	while (patterns.next()) {
		// Since we\'re on the server side, we know that every
		// pattern in the list is really a _Tt_s_pattern.
		ptn = (_Tt_s_pattern *)(*patterns).c_pointer();
		if (ptn->is_from_ptype() &&
		    *ptn->generating_ptype() == *ptype) {
			del_pattern(ptn);
			patterns.remove();
		}
	}
	return(TT_OK);
}


// 
// Initializes a procid. This consists of validating the host address on
// which the procid lives on and setting the _version field to the
// current global xdr version (which is set to the actual version of the
// client that represents this procid, see _tt_service_rpc in
// mp/mp_rpc_implement.cc). 
// 
Tt_status _Tt_s_procid::
init()
{
		
	if (! _tt_global->find_host(_proc_host_ipaddr, _proc_host, 1)) {
		// we got contacted by a procid on a host
		// address that we can't decode.
		return(TT_ERR_PROCID);
	}
	_version = _tt_global->xdr_version();
	return(TT_OK);
}


// 
// This method is called when a message that was offered to this procid
// as a handler is updated to a new state. The main action is to map the
// message to the same message in the queue of delivered messages and
// then invoke the _Tt_message::update_message to update the message
// found in the queue with the new values in the given message m.
// Finally, the _Tt_s_message::change_state method is invoked on the
// message in the queue in order to perform the work necessary for
// changing that message to the new state.
// 
Tt_status _Tt_s_procid::
update_message(const _Tt_message_ptr &m, Tt_state newstate)
{
	// if message was a start message then we notify the handler
	// ptype that this procid is done registering any patterns in
	// response to the starting message. 
	if (m->is_start_message()) {
		_Tt_ptype_ptr pt(_tt_s_mp->ptable->lookup(m->handler_ptype()));
			
		if (!pt.is_null()) {
			pt->proc_replied();
		}
		m->set_start_message(0);
	}
	if (!_delivered.is_null() && _delivered->count()) {
		_Tt_message_list_cursor		mcursor(_delivered);
		_Tt_s_message_ptr		dm;
				
		// map the message to its former self in the queue of
		// delivered messages, update the message and then
		// change it to its new state.

		_Tt_dispatch_reason reason;

		while (mcursor.next()) {
			if (mcursor->is_equal(m)) {
				dm = (_Tt_s_message *)(*mcursor).c_pointer();
				mcursor.remove();
				if (dm.c_pointer() != m.c_pointer()) {
					m->set_state(newstate);
					dm->update_message(m);
				} else {
					// Can happen when ttsession is
					// a client of itself.
				}

				switch (newstate) {
				    case TT_FAILED:
					reason = TTDR_MESSAGE_FAIL;
					break;
				    case TT_REJECTED:
					reason = TTDR_MESSAGE_REJECT;
					break;
				    case TT_HANDLED:
					reason = TTDR_MESSAGE_REPLY;
					break;
				    case TT_ACCEPTED:
					reason = TTDR_MESSAGE_ACCEPT;
					break;
				    case TT_ABSTAINED:
					reason = TTDR_MESSAGE_ABSTAIN;
					break;
				}
#ifdef OPT_BUG_SUNOS_5
				{
#endif
				// Keep this in sync with ::add_message()
				_Tt_msg_trace trace( *dm, reason );
				dm->change_state(this, newstate, trace);

#ifdef OPT_BUG_SUNOS_5
				// SunPro cfront calls
				// trace->~_Tt_msg_trace after the
				// break, resulting in statement not
				// reached
				}
#endif
				break;
			}
		}
	}

	return(TT_OK);
}


/* 
 *
 * XXX: This old definition of _Tt_s_procid::next_message is commented
 * out because it proved to be slower than the newer definition. It
 * implements an optimization that is rarely applicable in practice. The
 * optimization consists of batching notifications to a client. In
 * practice, clients rarely have more than one message outstanding so
 * this optimization rarely applies.
 *
 * Returns the next undelivered message for this procid. This method is
 * assumed to be invoked by a client process wanting to know what the
 * next message is for it. If there is a message for the process then it
 * is returned after having placed it on the delivered queue (but only if
 * it is a request because notifications don't need to be kept around
 * anymore once they're delivered). In client mode, an RPC call is
 * done to get the next message for this procid.
 */ 
/* 
 * Tt_status _Tt_s_procid::
 * next_message(_Tt_next_message_args &args)
 * {
 * 	if (!_undelivered.is_null() && _undelivered->count() > 0) {
 * 		args.msgs = new _Tt_message_list();
 * 		if (_undelivered->count() == 1) {
 * 			_Tt_s_message_ptr	nm((_Tt_s_message *)(_undelivered->top().c_pointer()));
 * 			
 * 			args.msgs->push((_Tt_message *)nm.c_pointer());
 * 			if (nm->message_class() == TT_REQUEST) {
 * 				if (handling(nm)) {
 * 					nm->set_send_handler_flags();
 * 					if (_delivered.is_null()) {
 * 						_delivered = new _Tt_message_list();
 * 					}
 * 					_delivered->push(nm);
 * 				} else if (nm->sender()->id() == _id) {
 * 					nm->set_return_sender_flags();
 * 				}
 * 			}
 * 			(void)_undelivered->pop();
 * 		} else {
 * 			_Tt_message_list_cursor		mc(_undelivered);
 * 			_Tt_s_message			*nm;
 * 			int				mcounter;
 * 			int				request_seen;
 * 			
 * 			for (mcounter = 0, request_seen = 0; mcounter < 10 && mc.prev();
 * 			     mcounter++, mc.remove()) {
 * 				// A message will be updated if the message is
 * 				// a request or if this procid is handling the
 * 				// message as opposed to just observing the
 * 				// message.
 * 				nm = (_Tt_s_message *)(*mc).c_pointer();
 * 				if (mc->message_class() == TT_REQUEST) {
 * 					if (handling(*mc)) {
 * 						// for requests we need to
 * 						// ensure that two requests
 * 						// are not sent to the receiver.
 * 						if (! request_seen) {
 * 							nm->set_send_handler_flags();
 * 							if (_delivered.is_null()) {
 * 								_delivered = new _Tt_message_list();
 * 							}
 * 							_delivered->push(*mc);
 * 							request_seen = 1;
 * 						} else {
 * 							// exit out of for
 * 							// loop (and don't
 * 							// execute the
 * 							// statement to push
 * 							// this message onto
 * 							// the list of
 * 							// messages sent back
 * 							// to the sender.)
 * 							break;
 * 						}
 * 					}
 * 					if (!request_seen) {
 * 						if (nm->sender()->id() == _id) {
 * 							nm->set_return_sender_flags();
 * 						}
 * 					}
 * 				}
 * 				args.msgs->push(*mc);
 * 			}
 * 		}
 * 		
 * 		if (_undelivered->count() == 0) {
 * 			args.clear_signal = (_flags&(1<<_TT_PROC_SIGNALLED));
 * 			_flags &= ~(1<<_TT_PROC_SIGNALLED);
 * 			_itimeout = -1;
 * 		} else {
 * 			args.clear_signal = 0;
 * 		}
 * 		
 * 		return(TT_OK);
 * 	} else {
 * 		args.clear_signal = (_flags&(1<<_TT_PROC_SIGNALLED));
 * 		_flags &= ~(1<<_TT_PROC_SIGNALLED);
 * 		args.msgs = (_Tt_message_list *)0;
 * 		
 * 		return(TT_WRN_NOTFOUND);
 * 	}
 * }
 */


// 
// Returns the next undelivered message for this procid. Actually returns
// a list of one message to allow for future optimizations with batching
// messages if they prove to be useful (see commented old definition of
// this method above.).  
// 
// This method is responsible for telling the client side by way of the
// special clear_signal field in args whether the client should clear the
// signalling channel. This will be the case if the message returned is
// the last undelivered message for this procid. Otherwise the client
// should not clear the signalling channel.
// 
// See _Tt_c_procid::next_message for the associated client-side
// processing. 
//
Tt_status _Tt_s_procid::
next_message(_Tt_next_message_args &args)
{
	if (_undelivered.is_null() || _undelivered->count() == 0) {
		// set clear_signal argument to whatever the value of
		// the _TT_PROC_SIGNALLED is. The effect is that if
		// this procid was signalled then clear_signal is set
		// to 1 which instructs the client to clear the
		// signal. Otherwise clear_signal is 0 because there
		// is no need to clear the signal on the client side. 

		args.clear_signal = (_flags&(1<<_TT_PROC_SIGNALLED));

		// clear flag since signalling channel will be cleared
		// or is clear already.
		_flags &= ~(1<<_TT_PROC_SIGNALLED);

		// no undelivered messages found
		args.msgs = (_Tt_message_list *)0;
		
		return(TT_WRN_NOTFOUND);
	}

	// we're here if there are undelivered messages.

	args.msgs = new _Tt_message_list();
	_Tt_s_message	*nm;

	nm = (_Tt_s_message *)(_undelivered->top().c_pointer());
	args.msgs->push(_undelivered->top());
	if (   (nm->message_class() == TT_REQUEST)
	    || (nm->message_class() == TT_OFFER))
	{
		if (processing(*_undelivered->top())) {
			// if this procid is handling this message
			// then set special flags to optimize xdr
			// process and put this message on the queue
			// of delivered messages.

			nm->set_send_handler_flags();
			if (_delivered.is_null()) {
				_delivered = new _Tt_message_list();
			}
			//
			// _delivered holds all the messages for which this
			// procid owes us an update.  Currently, these are:
			// 1. TT_REQUESTs being handled
			// 2. TT_OFFERs being voted on
			//
			_delivered->push(_undelivered->top());
		} else if (nm->sender()->id() == _id) {
			// if this procid is the original sender of
			// the message then set special flags to
			// optimize the xdr process.

			nm->set_return_sender_flags();
		}
	}
	(void)_undelivered->pop();

	if (_undelivered->count() == 0) {
		// the message was the last undelivered message so now
		// we tell the client side to clear the signalling
		// channel. 

		args.clear_signal = (_flags&(1<<_TT_PROC_SIGNALLED));
#ifdef OPT_ADDMSG_DIRECT
		// if this option is turned on then we clear the flag
		// that this procid was signalled and we also clear a
		// flag that says we sent the message down the
		// signalling channel.
		_flags &= ~((1<<_TT_PROC_SIGNALLED)|(1<<_TT_PROC_MSGSENT));
#else
		// no more undelivered messages so signalling channel
		// will be cleared by the client side.
		_flags &= ~(1<<_TT_PROC_SIGNALLED);
#endif				// OPT_ADDMSG_DIRECT
		_itimeout = -1;
	} else {
		// still more undelivered messages so client side
		// should not clear signalling fd.

		args.clear_signal = 0;
	}

	return(TT_OK);
}


// 
// signal the arrival of a message to a procid. Also, if the
// signalling fails and the failure is "severe" then invoke the
// _Tt_s_procid::set_active method to deactivate the procid. This is
// probably harsher treatment than necessary but it's hard to figure
// out if we failed to signal the client because of a transient
// network problem or because the client exited. 
// 
Tt_status _Tt_s_procid::
signal_new_message()
{
	int		signal_succeeded = 0;
	
	if (_flags&(1<<_TT_PROC_FD_CHANNEL_ON)) {
		if (! _socket.is_null()) {
			if (_flags&(1<<_TT_PROC_SIGNALLED)) {
				signal_succeeded++;
			} else {
				if (_socket->send("s", 1)) {
					signal_succeeded++;
				}
				_flags |= (1<<_TT_PROC_SIGNALLED);
			}
		}
	}
	if (! signal_succeeded) {
		set_active(0);
	}
	
	return((signal_succeeded > 0) ? TT_OK : TT_ERR_NOMP);
}



#ifdef OPT_ADDMSG_DIRECT

// 
// This method is similar to its overloaded cousin, with the exception
// that it sends the given message down the procid's signalling channel.
// Only one message is allowed to be sent down the signalling channel so
// if one was already sent (ie. _flags&(1<<_TT_PROC_MSGSENT)) then this
// method will just send a null byte down the signalling channel. The
// client-side will interpret this to mean that there is a new message
// and it should be retrieved by way of an rpc call rather than reading
// it off the signalling channel.
// 
Tt_status _Tt_s_procid::
signal_new_message(_Tt_message_ptr &m)
{
#if defined(sun) && defined(SVR4)
	typedef int (*Xdrrec_reader)(...);
	typedef int (*Xdrrec_writer)(...);
#elif defined(sun) && !defined(SVR4)
	typedef int (*Xdrrec_reader)();
	typedef int (*Xdrrec_writer)();
#else
	typedef int (*Xdrrec_reader)(void *, void *, u_int);
	typedef int (*Xdrrec_writer)(void *, void *, u_int);
#endif
	if (! (_flags&(1<<_TT_PROC_FD_CHANNEL_ON))) {
		return(TT_ERR_NOMP);
	}

	/* send the message down the signalling socket */
	if (_mxdr_stream == (XDR *)0) {
		_mxdr_stream = (XDR *)malloc((size_t)sizeof(XDR));
		xdrrec_create(_mxdr_stream, 0, 0,
			      (char *)_socket.c_pointer(),
			      (Xdrrec_reader)_tt_xdr_readit,
			      (Xdrrec_writer)_tt_xdr_writeit);
		_mxdr_stream->x_op = XDR_ENCODE;
	}

	if (_flags&(1<<_TT_PROC_MSGSENT)) {
		// we've already sent a message down the socket. Can't
		// send another one because that might overflow the
		// client. Instead we send a null message ptr. This
		// will indicate to the client to use an rpc call to
		// retrieve the message.
		_Tt_message_ptr	nomsg;
		if (nomsg.xdr(_mxdr_stream)) {
			xdrrec_endofrecord(_mxdr_stream, TRUE);
			return(TT_OK);
		}
		return(TT_ERR_NOMP);
	} 

	_Tt_s_message	*n = (_Tt_s_message *)m.c_pointer();

	if (processing(m)) {
		// note that this code is similar to code in
		// _Tt_s_procid::next_message to push this message on
		// the queue of delivered messages. However, here we
		// push the message on the undelivered queue. Only
		// when the client gets around to invoking the rpc
		// that causes _Tt_s_procid::msgread to be called do
		// we put this message (which will be the last
		// undelivered message) on the queue of delivered
		// messages.
		//
		// XXX: There should probably be a separate field to
		// hold a reference to this message rather than using
		// the _undelivered list. In particular this mechanism
		// may have timing problems if a message is added to
		// the _undelivered list before the client issues the
		// msgread method.

		n->set_send_handler_flags();
		if (_undelivered.is_null()) {
			_undelivered = new _Tt_message_list();
		}
		_undelivered->append(m);
	} else if (m->sender()->id() == _id) {
		n->set_return_sender_flags();
	}

	// now xdr the message down the stream and flush the pipe
	// using the xdrrec_endofrecord call.
	if (m.xdr(_mxdr_stream)) {
		xdrrec_endofrecord(_mxdr_stream, TRUE);
		_flags |= (1<<_TT_PROC_MSGSENT);
		return(TT_OK);
	} else {
		return(TT_ERR_NOMP);
	}
}


// 
// When this option is turned on, we require the client to invoke an rpc
// call to tell us when it has read the message off of the signalling
// channel. This is done in order to not flood the signalling channel
// (and risk blocking on it). 
// 
//  XXX: A better but somewhat more complicated design of this
//  architecture would explicitly deal with the problem of partial
//  writes. Note that as this option is implemented now, blocking is
//  still possible if a particularly large message is sent down the
//  signalling channel.
// 
void _Tt_s_procid::
msgread()
{
	_flags &= ~(1<<_TT_PROC_MSGSENT);
	if (!_undelivered.is_null() &&
	    _undelivered->count() &&
	    processing(_undelivered->top())) {
		if (_delivered.is_null()) {
			_delivered = new _Tt_message_list();
		}
		_delivered->push(_undelivered->top());
		_undelivered->pop();
	}
}

#endif		//  OPT_ADDMSG_DIRECT



// 
// Called to change the active status of a procid. A procid is active if
// it has a signalling channel which hasn't failed so far and if the
// procid hasn't timed out on any request. This method checks to see if
// the active status is being changed from what it already is set to.
// Particularly, if the status changes from 0 to 1 then pattern_added is
// called to redeliver any queued messages. If the status changes from 1
// to 0 then any undelivered messages are re-delivered. This method
// is called any time that a procid initiates contact with the server
// which indicates that if it had been ruled out because of a timeout, it
// should be considered active again. 
// 
// XXX: The original intent of this method is that procids could become
// active, inactive and then active again. For example, if a network
// problem would temporarily disconnect a procid then the server-side
// would just mark it inactive and go on. At a later time, if we heard
// from this procid again then this method would be called to mark it
// active again. This proved unworkable because in practice we have no
// way of telling if a procid is temporarily out or whether it's gone for
// good. Thus the method now effectively destroys the procid if it goes
// from being active to inactive (ie. messages are re-delivered,
// signalling channel is destroyed and patterns are unregistered). A
// subsequent rewrite of this module should reflect this change to make
// the code clearer. This would also entail considering whether it is
// necessary to call set_active from so many places as is done now.
// 
void _Tt_s_procid::
set_active(int on)
{
	int				remove_fds = 1;
	
	// XXX: temporary hack to disable removing this procid's fd
	// from the mp _active_procid_fds list (used by
	// _Tt_mp::main_loop). 
	if (on < 0) {
		remove_fds = 0;
		on = 0;
	}
	if (on) {
		activate();
	} else {
		deactivate(remove_fds);
	}
}

void _Tt_s_procid::
activate()
{
	if (!is_active() && _flags&(1<<_TT_PROC_FD_CHANNEL_ON)) {
		// procid is not active and a signalling
		// channel has been established for it.

		_flags |= (1<<_TT_PROC_ACTIVE);
		if (!_patterns.is_null()) {
			_tt_s_mp->initial_s_session->pattern_added();
		}
		// re-signal any undelivered messages.
		if (! _undelivered.is_null()) {
			for (int i = _undelivered->count(); i > 0; i--) {
				if (signal_new_message() != TT_OK) {
					return;
				}
			}
		}

		// if start_token is set, then we notify
		// the appropiate ptype (which is named by the
		// start token) that this procid is now
		// active. See _Tt_ptype::start for a full
		// discussion of how the ptype launch
		// mechanism works.

		if (_start_token.len() != 0) {
			_Tt_ptype_ptr   pt;
			if (_tt_s_mp->ptable->lookup(_start_token,pt)){
				pt->proc_started(this);
			}
			// set _start_token to 0 since it's
			// incorrect to notify a ptype more
			// than once about the same procid.
			_start_token = (char *)0;
		}
	}
}

void _Tt_s_procid::
deactivate(int remove_fds)
{
	if (! is_active()) {
		return;
	}
	_flags &= ~(1<<_TT_PROC_ACTIVE);

	// if this procid had a signalling channel
	// established then we remove the procid's id
	// and its signalling fd from the _Tt_s_mp
	// lists _active_fds and _active_fds_procids
	// (see _Tt_s_mp::find_proc for a description
	// of them) and we close the signalling
	// channel. 

	if (_flags&(1<<_TT_PROC_FD_CHANNEL_ON)) {
		_flags &=
		~(1<<_TT_PROC_FD_CHANNEL_ON);
		if (remove_fds) {
			_Tt_int_rec_list_cursor	fd_list(_tt_s_mp->_active_fds);
			_Tt_string_list_cursor	fd_procid_list(
						_tt_s_mp->_active_fds_procids);

			while (fd_list.next() &&
			       fd_procid_list.next()) {
				if (*fd_procid_list == _id) {
					fd_list.remove();
					fd_procid_list.remove();
					break;
				}
			}
		}
		// XXX: explicitly clear socket
		_socket = (_Tt_stream_socket *)0;
	}

	_Tt_message_list_cursor		orphanedC;
	_Tt_s_message			*orphaned;

	if (! _undelivered.is_null()) {
		// It's safe to redeliver undelivered
		// messages because it is known that
		// the receiving procid couldn't have
		// partially acted on the message.

		orphanedC.reset(_undelivered);
		while (orphanedC.next()) {
			orphaned = (_Tt_s_message *)(*orphanedC).c_pointer();
			if (processing(**orphanedC)) {
				_Tt_msg_trace trace( **orphanedC,
						     TTDR_ERR_PROCID );
				orphaned->set_status(TT_ERR_PROCID);
				Tt_state newstate = TT_FAILED;
				if (orphaned->message_class() == TT_OFFER) {
					newstate = TT_ABSTAINED;
				}
				orphaned->change_state(this, newstate, trace);
			}
			// remove references to this procid
			orphaned->remove_procid(this);
			orphanedC.remove();
		}
	}
	if (! _delivered.is_null()) {
		// Fail all messages that have been
		// delivered to the procid that is
		// exiting. The reason for doing this
		// is that if we redelivered the
		// messages then if the procid
		// partially did the computation
		// associated with the request the 
		// sender would have no way of
		// knowing. Therefore, here the sender
		// knows the message request failed or
		// was perhaps partially completed.
		// Now they can easily implement
		// whatever recovery they want (like 
		// resubmitting the request).

		orphanedC.reset(_delivered);
		while (orphanedC.next()) {
			orphaned = (_Tt_s_message *)(*orphanedC).c_pointer();
			orphaned->set_status(TT_ERR_PROCID);
			_Tt_msg_trace trace( **orphanedC, TTDR_ERR_PROCID );
			Tt_state newstate = TT_FAILED;
			if (orphaned->message_class() == TT_OFFER) {
				newstate = TT_ABSTAINED;
			}
			orphaned->change_state(this, newstate, trace);
			orphaned->remove_procid(this);
			orphanedC.remove();
		}
	}

	// unregister all patterns that were
	// registered on behalf of this procid.

	if (! _patterns.is_null()) {
		_Tt_pattern_list_cursor		pc(_patterns);

		while (pc.next()) {
			del_pattern(*pc);
			pc.remove();
		}
	}
}

int
_Tt_s_procid::set_fd(
	int fd
)
{
	_flags |= (1<<_TT_PROC_FD_CHANNEL_ON);
	_Tt_int_rec_list_cursor	fd_list(_tt_s_mp->_active_fds);
	_Tt_string_list_cursor	fd_procid_list(_tt_s_mp->_active_fds_procids);
	while (fd_list.next() && fd_procid_list.next()) {
		if (*fd_procid_list == _id) {
			fd_list->val = fd;
			break;
		}
	}
	set_active(1);
	return(1);
}

// 
// The given argument is the port number of the socket opened by the
// client procid. This function will then attempt to connect to this
// socket using the _Tt_stream_socket methods (in particular the fd
// method causes the actual connection attempt). The fd is then installed
// in the _Tt_s_mp lists, _active_fds and _active_fds_procids, which are
// parallel lists with the first containing fds and the second containing
// procid ids. See _Tt_s_mp::find_proc for more details.
//  
int _Tt_s_procid::
set_fd_channel(int portnum)
{
	_socket = new _Tt_stream_socket(_proc_host, portnum);
	if (! _socket->init(0)) {
		_socket = (_Tt_stream_socket *)0;
		return(0);
	}
	return set_fd( _socket->fd() );
}


// 
// This function sets the internal timeout for this procid based on the
// message argument given. For messages which are to be handled by this
// procid a standard timeout value is used. Otherwise, the timeout value
// is unaltered.
// 
void _Tt_s_procid::
set_timeout_for_message(const _Tt_message &m)
{
	if (_itimeout == -1 && processing(m)) {
		_itimeout = _TT_MESSAGE_TIMEOUT;
	}
}


// 
// Called by _Tt_mp::service_timeout when a timeout event happened with
// the given timeout. This function checks to see whether the timeout is
// equal to or greater than the internal timeout (which is only set if we
// signalled the client procid for a new message). If the timeout exceeds
// the internal timeout then this procid is turned into an inactive
// procid.
// 
//  XXX: The timeout handling is probably incorrect. Rather that turning
//  the procid inactive (which effectively removes it) the message that
//  triggered the timeout should be redelivered.
// 
int _Tt_s_procid::
service_timeout(int timeout)
{
	if (_itimeout == -1) { // timeout not set, return exception value
		return(-1);
	}
	_itimeout -= timeout;
	if (_itimeout > 0) { // still haven't timed out
		return(_itimeout);
	} else {		// timeout!
		set_active(0);
		
		return(-1);
	}
}


//  
// Returns 1 if the this procid has declared itself to be of the given
// ptype. 
// 
int _Tt_s_procid::
is_ptype(_Tt_string ptid)
{
	if (! _declared_ptypes.is_null() &&
	    0 != _declared_ptypes->count()) {
		_Tt_string_list_cursor	pt(_declared_ptypes);
		
		while (pt.next()) {
			if (*pt == ptid) {
				return(1);
			}
		}
	}
	return(0);
}

// Put the given message on the list of messages to be delivered if the
// procid exits without calling tt_close. 
Tt_status _Tt_s_procid::
add_on_exit_message(_Tt_s_message_ptr &m)
{
	if (_on_exit_messages.is_null()) {
		_on_exit_messages = new _Tt_s_message_list;
	}

	_on_exit_messages->append(m);
	return TT_OK;
}
	
// Called when the process does tt_close.
// Throw away all the messages saved off on _on_exit_messages.
void _Tt_s_procid::
cancel_on_exit_messages()
{
	_on_exit_messages = (_Tt_s_message_list *)0;
}

// Called when the process drops its connection to the server.
// Send all the messages saved off on _on_exit_messages.
// If a tt_close has been done, the _on_exit_messages list will
// be empty.
void _Tt_s_procid::
send_on_exit_messages()
{
	if (_on_exit_messages.is_null()) {
		return;
	}

	_Tt_s_message_list_cursor c(_on_exit_messages);

	while(c.next()) {
		_Tt_msg_trace trace( **c, TTDR_MESSAGE_SEND_ON_EXIT );
		c->dispatch( trace );
		c->deliver_to_observers_and_handlers( trace );
	}
}
	
