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
//%%  $TOG: mp_c_message.C /main/5 1999/10/14 18:40:53 mgreess $ 			 				
/*
 * @(#)mp_c_message.C	1.46 95/04/27
 *
 * @(#)mp_c_message.C	1.36 93/09/07
 *
 * Tool Talk Message Passer (MP) - mp_c_message.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include "api/c/api_spec_map_ref.h"
#include "api/c/api_error.h"
#include "mp/mp_c_global.h"
#include "mp/mp_arg.h"
#include "mp/mp_c_file.h"
#include "mp/mp_c_message.h"
#include "mp/mp_c_mp.h"
#include "mp/mp_c_procid.h"
#include "mp/mp_rpc_interface.h"
#include "mp/mp_c_session.h"
#include "util/tt_enumname.h"
#include "mp/mp_trace.h"
#include "util/tt_gettext.h"
#include "util/tt_port.h"


//
// The methods in this file implement the client-only functionality
// required for messages. 
//



_Tt_c_message::
_Tt_c_message()
{
}

_Tt_c_message::
~_Tt_c_message()
{
}


// 
// Will return 1 if this message has the update xdr flag turned on
// indicating it is a reply to an old message rather than an entirely new
// message. 
//
int _Tt_c_message::
is_a_diff()
{
	return(_flags&(1<<_TT_MSG_UPDATE_XDR_MODE));
}


// 
// Dispatches a message. If observers_only is 1 then the only observer
// patterns will be matched for this message. One of the important
// responsibilities of this method is to fill in any fields in the
// message that require database lookups. For example, if it is an
// object-oriented message then this method will fill in the otype field
// in the message by asking the right database for the type field of the
// given object field. This is done to free ttsession from ever having to
// do any database lookups. For file-scope messages, this method will
// also append the list of remote sessions that have procids joined to
// the file mentioned in this message. This is used by ttsession when
// routing the message.
// 
// A somewhat tricky case of freeing ttsession from invoking database
// operations arises in message-queueing. When a file-scope message
// requires queueing the return argument from the rpc call to dispatch
// the message will indicate what ptypes the message should be queued
// for. It is then this method which then goes ahead and queues the
// message on the file.
Tt_status _Tt_c_message::
dispatch(int observers_only)
{
	Tt_status			status;
	_Tt_db_results			dbStatus;
	Tt_status			resolve_status = TT_OK;
	Tt_status			rpc_status;
	_Tt_message_ptr			mptr;
	_Tt_file_ptr			fptr;
	// The following two pointers are not smart ptrs, to save
	// two trips into constructor and destructor per message send.
	// This doesn\'t mess up ref counting since the default procid
	// and session never change while in this method.
	_Tt_c_session			*d_session;
	_Tt_c_procid			*d_procid;
	_Tt_dispatch_reply_args		args;

	if (_message_class == TT_CLASS_UNDEFINED ||
	    _message_class == TT_CLASS_LAST) {
		return(TT_ERR_CLASS);
	}
	if (_state != TT_CREATED && _state != TT_QUEUED) {
		// we shouldn't be re-sending a message whose state
		// isn't created or queued
		return(TT_ERR_STATE);
	}

	d_procid = _tt_c_mp->default_c_procid().c_pointer();
	d_session = d_procid->default_session().c_pointer();
		
	// take any necessary actions according to the message
	// paradigm (as well as check for legitimate paradigm
	// fields). 
	switch (_paradigm) {
	      case TT_HANDLER:
       		if (handler().is_null()) {
			// Point-to-point messages must have a handler.
			return TT_ERR_PROCID;
		}
		// fall through to check scope
	      case TT_PROCEDURE:
		if (_scope == TT_SCOPE_NONE) {
			return(TT_ERR_SCOPE);
		}
		break;
	      case TT_OBJECT:
		if (_object.is_null()) {
			return TT_ERR_OBJID;
		}
		resolve_status = resolve();
		switch (resolve_status) {
		      case TT_OK:
		      case TT_WRN_STALE_OBJID:
			break;
		      default:
			return(resolve_status);
		}
		break;
	      case TT_OTYPE:
		if (_otype.is_null()) {
			return TT_ERR_OTYPE;
		}
		break;
	      case TT_ADDRESS_LAST:
	      default:
		return(TT_ERR_ADDRESS);
	}
	if ((_message_class == TT_OFFER) && (_paradigm != TT_PROCEDURE)) {
		return TT_ERR_ADDRESS;
	}
	//
	// Since we know the server's RPC version, we only try the
	// call if we know it will work, else we generate our own
	// TT_ERR_UNIMP.  Prior to 1.2, we let RPC_PROCUNAVAIL become
	// TT_ERR_UNIMP.  Now we use classic RPC versioning.
	//
	if (   (message_class() == TT_OFFER)
	    && (d_session->rpc_version() < TT_OFFER_RPC_VERSION))
	{
		return TT_ERR_UNIMP;
	}

	if (_file.len() &&
	    (_scope == TT_FILE || _scope == TT_BOTH ||
	     _paradigm==TT_OTYPE || _paradigm == TT_OBJECT)) {
		// If the message is file-scoped then we append the
		// list of remote sessions to send the message to. 
		
		// XXX: For object-oriented msgs we don't know the
		// scope yet so we append the sessions anyway if the
		// file attribute is filled in. Probably need to
		// refine the protocol so that we first ask if the
		// message is file-scoped (pure dispatch) and then we
		// deliver accordingly. This is mostly just
		// performance-tuning.
		if (_tt_mp->find_file(_file, fptr, 1) == TT_OK) {
			_rsessions = fptr->getSessions();
			dbStatus = fptr->getDBResults();
			status = _tt_get_api_error( dbStatus, _TT_API_FILE );
			if (status == TT_ERR_INTERNAL) {
				_tt_syslog( 0, LOG_WARNING,
					    "_Tt_db_file::getSessions(): %d",
					    dbStatus );
			}
			if (status != TT_OK) {
				_rsessions = (_Tt_string_list *)0;
				return status;
			}
			// we set the msg guard bit for the _rsessions
			// field to 1 if it is non-empty.
			SET_GUARD(_full_msg_guards,
				  (!_rsessions.is_null()),
				  _TT_MSK_RSESSIONS);
		} else {
			return(TT_ERR_FILE);
		}
	}
	
	if (! _rsessions.is_null() && _rsessions->count()) {
		// now we dispatch file scope notifications to remote
		// sessions, this is done here to minimize the amount
		// of inter-server communication done.
		// If the message is handler-addressed, don\'t bother
		// doing this as pattern matching doesn\'t work on point-
		// to-point messages anyway, and sending it to the foreign
		// ttsession just seems to confuse it.
		if (_paradigm!=TT_HANDLER &&
		    (_scope == TT_FILE || _scope == TT_BOTH)) {
			_tt_mp->check_if_sessions_alive();
			_flags |= (1<<_TT_MSG_OBSERVERS_ONLY);
			(void)dispatch_file_scope_notification(fptr);
		}
	}
	
	// send the message to the default session
	if (!observers_only) {
		_flags &= ~(1<<_TT_MSG_OBSERVERS_ONLY);
	}
	mptr = this;
	args.status = TT_OK;
	args.qmsg_info = (_Tt_qmsg_info *)0;
	
	switch (d_session->rpc_version()) {
	      case 1:
		// version 1 (tooltalk version 1.0) dispatch routine
		// expects to return arguments
		rpc_status = d_session->call(_rpc_dispatch(),
					     (xdrproc_t)tt_xdr_message,
					     (char *)&mptr,
					     (xdrproc_t)tt_xdr_dispatch_reply_args,
					     (char *)&args);
		break;
	      case 2:
	      default:
		// Only send _session field if scope is TT_FILE or TT_BOTH
		SET_GUARD(_full_msg_guards,
			  (_scope==TT_FILE || _scope==TT_BOTH),
			  _TT_MSK_SESSION);

		// version 2 (tooltalk version 1.0.1) uses a one-way
		// rpc if the scope of the message doesn't imply
		// file-scope or if the message is already in queued
		// state. 
		rpc_status = d_session->call(((_scope == TT_SESSION ||
					       _scope == TT_FILE_IN_SESSION ||
					       _state == TT_QUEUED) ?
					      _rpc_dispatch_2() :
					      _rpc_dispatch()),
					     (xdrproc_t)tt_xdr_message,
					     (char *)&mptr,
					     (xdrproc_t)tt_xdr_dispatch_reply_args,
					     (char *)&args);
		break;
	}
	//
	// _Tt_s_message::dispatch() has set the state to TT_SENT
	//
	Tt_state old_state = state();
	set_state( TT_SENT );
	switch (_message_class) {
	    case TT_REQUEST:
	    case TT_OFFER:
		set_awaiting_reply();
	}
	_Tt_msg_trace trace( *this, old_state );
	if (args.status == TT_ERR_FILE) {
		//
		// TT_ERR_FILE is used to indicate that the session
		// doesn't contain any processes joined to the file
		// contained in the message.  But in this case, we
		// do not want to try to delete the default session
		// from the interest list, because we do not even know
		// if it is *on* the interest list.
		//
		args.status = TT_OK;
	}

	// queue the message if necessary. XXX what to do if queueing fails?
	if (!(args.qmsg_info).is_null()) {
		if (_tt_mp->find_file(_file, fptr, 1) == TT_OK) {
			dbStatus = fptr->queueMessage(args.qmsg_info->ptypes,
						      mptr);
			if (dbStatus != TT_DB_OK) {
				_tt_syslog( 0, LOG_WARNING,
					    catgets( _ttcatd, 1, 18,
						     "could not queue a message"
						     " on file \"%s\" because "
						     "of internal error %d" ),
					    (char *)fptr->getLocalPath(),
					    dbStatus );
			}
		}
	}

	if (rpc_status != TT_OK) {
		return rpc_status;
	}

	if (args.status != TT_OK) {
		return args.status;
	}

	return resolve_status;
}



// Sends a message to the server for dispatching if this process exits
// without calling tt_close.
Tt_status _Tt_c_message::
dispatch_on_exit()
{
	Tt_status			rpc_status;
	_Tt_message_ptr			mptr;
	_Tt_c_session_ptr		d_session;
	_Tt_c_procid_ptr		d_procid;
	_Tt_dispatch_reply_args		args;

	d_procid = _tt_c_mp->default_c_procid();
	d_session = d_procid->default_session();
		
	// take any necessary actions according to the message
	// paradigm (as well as check for legitimate paradigm
	// fields). 
	switch (_paradigm) {
	      case TT_PROCEDURE:
	      case TT_HANDLER:
		if (_scope != TT_SESSION) {
			return(TT_ERR_SCOPE);
		}
		break;
	      case TT_OBJECT:
	      case TT_OTYPE:
	      case TT_ADDRESS_LAST:
	      default:
		return(TT_ERR_ADDRESS);
	}
	
	mptr = this;
	args.status = TT_OK;
	args.qmsg_info = (_Tt_qmsg_info *)0;
	

	rpc_status = d_session->call(TT_RPC_DISPATCH_ON_EXIT,
				     (xdrproc_t)tt_xdr_message,
				     (char *)&mptr,
				     (xdrproc_t)tt_xdr_dispatch_reply_args,
				     (char *)&args);

	if (rpc_status != TT_OK) {
		return rpc_status;
	} else if (args.status != TT_OK) {
		return args.status;
	}
	//
	// _Tt_s_message::dispatch() will set the state to TT_SENT
	// if we die.  This is our only chance to trace it.
	//
	Tt_state old_state = state();
	set_state( TT_SENT );
	// Do not set_awaiting_reply().
	_Tt_msg_trace trace( *this, old_state );
	return TT_OK;
}


// 
// Set up special flags to optimize xdr'ing the message when it the
// current client replies to it. See _Tt_message::xdr for a
// description of the message xdr'ing scheme.
// 
void _Tt_c_message::
set_return_handler_flags()
{
	const int	req_fields = _TT_MSK_ID | 
	                             _TT_MSK_MESSAGE_CLASS |
				     _TT_MSK_SENDER |
				     _TT_MSK_STATUS |
				     _TT_MSK_FLAGS;


	// turning on this flag means that when xdr'ed the _out_args
	// arglist (which holds only TT_OUT/TT_INOUT args) is sent
	// rather than the normal _args arglist. (see _Tt_message::xdr)
	_flags |= (1<<_TT_MSG_UPDATE_XDR_MODE);

	// turn on the required fields
	_ptr_guards = req_fields;

	if (is_start_message()) {
		// if this message was a start message then we need to
		// send the handler_ptype field because the
		// _Tt_s_procid::update_message method needs to make
		// use of the field before it has matched up this
		// message with its server-side copy.
		SET_PTR_GUARD(_full_msg_guards&_TT_MSK_HANDLER_PTYPE,
			      _TT_MSK_HANDLER_PTYPE);
	}

	// set up _out_arg list by adding any arguments that are
	// TT_OUT or TT_INOUT.
	SET_GUARD(_full_msg_guards, 0, _TT_MSK_ARGS);
	if (_out_args.is_null() && !_args.is_null() && _args->count()) {
		_Tt_arg_list_cursor	argc(_args);

		_out_args = new _Tt_arg_list();
		while (argc.next()) {
			add_out_arg(*argc);
		}
	}

	// An optimization assumes that we normally sent SESSION
	// scoped messages, so we don't update the guards for
	// the message scope.  If this message isn't SESSION
	// scoped, we set the guard anyhow.
	if (_scope != TT_SESSION) {
		SET_PTR_GUARD(_full_msg_guards&_TT_MSK_SCOPE, _TT_MSK_SCOPE);
	}

	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_HANDLER, _TT_MSK_HANDLER);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_STATUS_STRING,
		      _TT_MSK_STATUS_STRING);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_FILE, _TT_MSK_FILE);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_OBJECT, _TT_MSK_OBJECT);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_ARGS, _TT_MSK_ARGS);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_CONTEXTS, _TT_MSK_CONTEXTS);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_OFFEREES, _TT_MSK_OFFEREES);
	SET_PTR_GUARD(_full_msg_guards&_TT_MSK_OPNUM, _TT_MSK_OPNUM);
}


// 
// Looks up the otype of the object field of the message. If the lookup
// fails then an error code is returned. If the otype of the message is
// already filled in then the resolve succeeds (issue: should it check to
// make sure the otype is valid for this object id?)
// 
Tt_status _Tt_c_message::
resolve()
{
	if (_otype.len() == 0) {
		_Tt_api_spec_map_ref	spec_map;
		_Tt_objid_spec_ptr	spec;
		_Tt_string		fresh_objid;
		Tt_status		val2return = TT_OK;
		
		// look up oid in object database;
		if (_object.len() == 0) {
			return(TT_ERR_OBJID);
		}

		spec = spec_map.getSpec(_object);
		if (spec.is_null()) {
			return(TT_ERR_OBJID);
		}

		switch (spec->getDBResults()) {
		      case TT_DB_OK:
			val2return = TT_OK;
			break;
		      case TT_DB_WRN_FORWARD_POINTER:
			fresh_objid = spec->getObjectID();
			set_object(fresh_objid);
			val2return = TT_WRN_STALE_OBJID;
			break;
		      default:
			return(TT_ERR_OBJID);
		}
		// fill in otype and file field for oid;
		_otype = spec->getType();
		if (spec->getDBResults() != TT_DB_OK) {
			return(TT_ERR_OBJID);
		}
		SET_GUARD(_full_msg_guards, _otype.len(), _TT_MSK_OTYPE);

		// fill in file field in message
		// --> what if get_path fails?
		_file = spec->getFile();
		if (spec->getDBResults() != TT_DB_OK) {
			_file = (char *)0;
		}
		SET_GUARD(_full_msg_guards, _file.len(), _TT_MSK_FILE);

		return(val2return);
	}
	return(TT_OK);
}


// 
// This function dispatches in observer-only mode this message to all the
// remote file-scope sessions interested in the file in this message. The
// reason this is done here is to minimize inter-ttsession communication.
// A side-benefit is that if message sending to a remote session fails
// here, the remote session is removed from the _rsessions list and
// ttsession won't try to contact the failed remote session. This method
// also handles removing stale sessions from the file object (where a
// stale session is one that is still listed in the file object's session
// list because the client that was joined to the file in that session
// didn't exit cleanly). 
// 
// XXX: what is the status that should be returned from this function?
// Ideally one would want to know if every remote session got the
// message or all of them failed? What about partial failures?
Tt_status _Tt_c_message::
dispatch_file_scope_notification(_Tt_file_ptr &fp)
{
	_Tt_string			sid;
	_Tt_string_list_cursor		rc(_rsessions);
	_Tt_session_ptr			rs;
	_Tt_message_ptr			mptr = this;
	_Tt_dispatch_reply_args		args;
	_Tt_db_results			dbStatus;
	
	while (rc.next()) {
		sid = *rc;
		if (_session->has_id(sid)) {
			rc.remove();
			continue;
		}
		if (TT_OK != _tt_c_mp->find_session(sid, rs, 1)) {
			//
			// stale session id on the file
			//
			dbStatus = fp->deleteSession(sid);
			rc.remove();
			if (   (dbStatus != TT_DB_OK)
			    && (dbStatus != TT_DB_ERR_NO_SUCH_PROPERTY))
			{
				_tt_syslog( 0, LOG_WARNING,
					    catgets( _ttcatd, 1, 19,
						     "could not delete stale "
						     "session <%s> from "
						     "interest list of file "
						     "\"%s\" because of "
						     "internal error %d" ),
					    (char *)sid,
					    (char *)fp->getLocalPath(),
					    dbStatus );
			}
			continue;
		}
		// XXX: optimization: maybe we should have two
		// DISPATCH rpc calls, one that replies with just a
		// status and the other that replies with a
		// _Tt_dispatch_reply_args (which is only needed once
		// to know if the message needs to be queued).
		if (rs->call(_rpc_dispatch(),
			     (xdrproc_t)tt_xdr_message, (char *)&mptr,
			     (xdrproc_t)tt_xdr_dispatch_reply_args,
			     (char *)&args)
		    != TT_OK)
		{
			// remove this session from message list to
			// minimize the chances of the server doing
			// bad rpc calls when looking for handlers.
			continue;
		}
		if (args.status == TT_ERR_FILE) {
			// indication from remote session that it
			// shouldn't be listed in this file's scope
			dbStatus = fp->deleteSession(sid);
			rc.remove();
			//
			// If the client that registered the file-scoped
			// pattern quits the file after the sender
			// does a getSessions call, then the session may
			// show up as stale, but will not be in the 
			// database any more.  This is why
			// TT_DB_ERR_NO_SUCH_PROPERPTY is an OK 
			// return code.
			//
			if (   (dbStatus != TT_DB_OK)
			    && (dbStatus != TT_DB_ERR_NO_SUCH_PROPERTY))
			{
				_tt_syslog( 0, LOG_WARNING,
					    catgets( _ttcatd, 1, 20,
						     "could not delete "
						     "uninterested session "
						     "<%s> from interest list "
						     "of file \"%s\" because "
						     "of internal error %d" ),
					    (char *)sid,
					    (char *)fp->getLocalPath(),
					    dbStatus );
			}
		}
	}
	
	return(TT_OK);
}

_Tt_rpc_procedure_number _Tt_c_message::
_rpc_dispatch() const
{
	if (_contexts.is_null()) {
		return TT_RPC_DISPATCH;
	}
	if (_contexts->count() <= 0) {
		return TT_RPC_DISPATCH;
	}
	return TT_RPC_DISPATCH_WITH_CONTEXT;
}

_Tt_rpc_procedure_number _Tt_c_message::
_rpc_dispatch_2() const
{
	if (_contexts.is_null()) {
		return TT_RPC_DISPATCH_2;
	}
	if (_contexts->count() <= 0) {
		return TT_RPC_DISPATCH_2;
	}
	return TT_RPC_DISPATCH_2_WITH_CONTEXT;
}
