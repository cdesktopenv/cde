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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: mp_c_procid.C /main/5 1999/09/17 18:27:38 mgreess $ 			 				
/* 
 * @(#)mp_c_procid.C	1.30 93/07/30
 *
 * Copyright (c) 1990,1992 by Sun Microsystems, Inc.
 */
#include <unistd.h>
#include <fcntl.h>
#include <tt_options.h>
#include <util/tt_global_env.h>
#include <util/tt_host.h>
#include <util/tt_threadspecific.h>
#include <mp/mp_c_file.h>
#include <mp/mp_c_message.h>
#include <mp/mp_c_mp.h>
#include <mp/mp_c_global.h>
#include <mp/mp_pattern.h>
#include <mp/mp_c_procid.h>
#include <mp/mp_rpc_interface.h>
#include <mp/mp_c_session.h>
#include <mp/mp_stream_socket.h>
#include <util/tt_int_rec.h>
#include <arpa/inet.h>


//
// This file contains client-side only methods for procids.
//



_Tt_c_procid::
_Tt_c_procid()
{
}

_Tt_c_procid::
~_Tt_c_procid()
{
}


_Tt_c_procid::
_Tt_c_procid(const _Tt_string &id)
{
	_id = id;
}


// 
// Used by the client library to register a new pattern. The real work is
// done on the server-side.
// 
Tt_status _Tt_c_procid::
add_pattern(_Tt_pattern_ptr &p)
{
	Tt_status			status;
	Tt_status			rstatus;
	_Tt_add_pattern_args		args;

		
	if (p->category() == TT_CATEGORY_LAST) {
		// category needs to be set
		return(TT_ERR_CATEGORY);
	}
	
	args.procid = this;
	args.pattern = p;
	
	rstatus = default_session()->call(   p->contextsCount()
					  ? TT_RPC_ADD_PATTERN_WITH_CONTEXT
					  : TT_RPC_ADD_PATTERN,
					 (xdrproc_t)tt_xdr_add_pattern_args,
					 (char *)&args,
					 (xdrproc_t)xdr_int,
					 (char *)&status);
	if (rstatus != TT_OK) {
		return rstatus;
	}
	if (status != TT_OK) {
		return status;
	}
	return p->join_files( default_session()->process_tree_id() );
}


// 
// Invokes the right rpc call to cause this procid to be recognized as an
// instance of the given ptype. The server-side does most of the work
// here. The only subtlety here is that we keep a list of the ptypes that
// this procid has declared itself as so that file-scope message queueing
// can work properly.
// 
Tt_status _Tt_c_procid::
declare_ptype(_Tt_string &ptid)
{
	Tt_status		status;
	Tt_status		rstatus;
	_Tt_declare_ptype_args	args;
		
	args.procid = this;
	args.ptid = ptid;
		
	rstatus = default_session()->call(TT_RPC_DECLARE_PTYPE,
					 (xdrproc_t)tt_xdr_declare_ptype_args,
					 (char *)&args,
					 (xdrproc_t)xdr_int,
					 (char *)&status);
	if (rstatus == TT_OK && status == TT_OK) {
		if (_declared_ptypes.is_null()) {
			_declared_ptypes = new _Tt_string_list();
		}
		_declared_ptypes->push(ptid);
	}
	return((rstatus == TT_OK) ? status : rstatus);
}

// 
// Invokes the right rpc call to cause this procid to stop being recognized
// as an instance of the given ptype. The server-side does most of the work
// here. The only subtlety here is that we keep a list of the ptypes that
// this procid has declared itself as so that file-scope message queueing
// can work properly, and that has to be updated too. We also use
// this local list to return a bad error code if the ptype was never
// declared \(I suppose it would be a optimization to only make the
// RPC call if the ptype\'s in the list...\)
// 
Tt_status _Tt_c_procid::
undeclare_ptype(_Tt_string &ptid)
{
	Tt_status		status;
	Tt_status		rstatus;
	_Tt_declare_ptype_args	args;
		
	args.procid = this;
	args.ptid = ptid;
		
	rstatus = default_session()->call(TT_RPC_UNDECLARE_PTYPE,
					 (xdrproc_t)tt_xdr_declare_ptype_args,
					 (char *)&args,
					 (xdrproc_t)xdr_int,
					 (char *)&status);
	if (rstatus == TT_OK && status == TT_OK) {
		if (!_declared_ptypes.is_null()) {
			_Tt_string_list_cursor c(_declared_ptypes);
			int was_declared = 0;
			while (c.next()) {
				if (*c == ptid) {
					c.remove();
					was_declared = 1;
				}
			}
			if (!was_declared) {
				status = TT_ERR_PTYPE;
			}
		}
	}
	return((rstatus == TT_OK) ? status : rstatus);
}


// 
// Invokes the right rpc call to test if a ptype is known by the current
// ttsession.
// 
Tt_status _Tt_c_procid::
exists_ptype(_Tt_string &ptid)
{
	Tt_status		status;
	Tt_status		rstatus;
	_Tt_declare_ptype_args	args;
		
	args.procid = this;
	args.ptid = ptid;
		
	rstatus = default_session()->call(TT_RPC_EXISTS_PTYPE,
					 (xdrproc_t)tt_xdr_declare_ptype_args,
					 (char *)&args,
					 (xdrproc_t)xdr_int,
					 (char *)&status);
	return((rstatus == TT_OK) ? status : rstatus);
}

// 
// Invokes the right rpc call to unblock any messages being held for
// this ptype until this process is intialized.
// 
Tt_status _Tt_c_procid::
unblock_ptype(const _Tt_string &ptid)
{
	Tt_status		status;
	Tt_status		rstatus;
	_Tt_declare_ptype_args	args;
		
	args.procid = this;
	args.ptid = ptid;
		
	rstatus = default_session()->call(TT_RPC_UNBLOCK_PTYPE,
					 (xdrproc_t)tt_xdr_declare_ptype_args,
					 (char *)&args,
					 (xdrproc_t)xdr_int,
					 (char *)&status);
	return((rstatus == TT_OK) ? status : rstatus);
}

// 
// Unregister a pattern for this procid. The pattern is identified by its
// id which is matched up on the server-side to the real pattern object.
// 
Tt_status _Tt_c_procid::
del_pattern(const _Tt_string &id)
{
	Tt_status			status;
	Tt_status			rstatus;
	_Tt_del_pattern_args		args;
		
	if (id.len() == 0) {
		return(TT_ERR_INVALID);
	}
		
	args.procid = this;
	args.pattern_id = id;
		
	rstatus = default_session()->call(TT_RPC_DEL_PATTERN,
					 (xdrproc_t)tt_xdr_del_pattern_args,
					 (char *)&args,
					 (xdrproc_t)xdr_int,
					 (char *)&status);
	return((rstatus == TT_OK) ? status : rstatus);
}		


// Invokes the right rpc call to load a set of types, contained in a 
// string which is the image of an XDR types file, into the ttsession.
// 
Tt_status _Tt_c_procid::
load_types(_Tt_string &typebuffer)
{
	Tt_status		status;
	Tt_status		rstatus;
	_Tt_load_types_args	args;
		
	args.xdrtypes = typebuffer;
		
	rstatus = default_session()->call(TT_RPC_LOAD_TYPES,
					 (xdrproc_t)tt_xdr_load_types_args,
					 (char *)&args,
					 (xdrproc_t)xdr_int,
					 (char *)&status);
	return((rstatus == TT_OK) ? status : rstatus);
}

_Tt_c_session_ptr& _Tt_c_procid::
default_session()
{
#ifdef OPT_XTHREADS
	if (_tt_global->multithreaded()) {

		// Use the thread-specific session if there is one

		_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
		xthread_get_specific(_tt_global->threadkey, (void **) &tss);
		if (!tss) {
			// thread-specific storage not yet initialized -- do it
			tss = new _Tt_threadspecific(_tt_c_mp->default_c_session,
						     this);
			xthread_set_specific(_tt_global->threadkey, tss);
		}

		_Tt_c_session_ptr sptr = tss->thread_c_session();

		if (!sptr.is_null()) {
			return tss->thread_c_session();
		}
		else {
			tss->set_thread_c_session(_default_session);
			return _default_session;
		}
	}
	else {
		return _default_session;
	}
#else
	return _default_session;
#endif
}

//
// In a multiple-procid, multiple-session usage this method means that
// the current procid has done some action that requires it to be bound
// to the current session. This method just turns on a flag in the procid
// object that means it is now bound to the current session.
// 
Tt_status _Tt_c_procid::
commit()
{
	if (default_session().is_null()) {
		return(TT_ERR_NOMP);
	}
	_flags |= (1<<_TT_PROC_COMMITTED);
	return(TT_OK);
}


// 
// Initializes a procid for use. The main duties are to set the id of a
// procid and to initialize a signalling channel. Setting the id involves
// asking the server for a unique prefix to make the procid's id unique
// among all procids currently registered with the session. (see set_id
// method).  The signalling channel is initialized here because it is the
// main mechanism for the server to know if this procid has exited (since
// breaking the signalling channel will wake up the server).
// 
Tt_status _Tt_c_procid::
init()
{
	Tt_status		status;
			
	if (_default_session.is_null()) {
			
		// set the default session to the initial session.

#ifdef OPT_XTHREADS
        	if (_tt_global->multithreaded()) {

		        // Use the thread-specific session if there is one

			_Tt_threadspecific* tss = (_Tt_threadspecific *) 0;
			xthread_get_specific(_tt_global->threadkey,
					     (void **) &tss);
			if (!tss) {
				// thread-specific storage not yet
				// initialized -- do it
				tss = new _Tt_threadspecific(_tt_c_mp->
							         default_c_session,
							     this);
				xthread_set_specific(_tt_global->threadkey, tss);
			}

		        _Tt_c_session_ptr sptr = tss->thread_c_session();

			if (!sptr.is_null()) {
				_default_session = sptr;
			}
			else {
				tss->set_thread_c_session(_tt_c_mp->
							  default_c_session);
				_default_session = _tt_c_mp->default_c_session;
			}
		}
		else {
			_default_session = _tt_c_mp->default_c_session;
		}
#else
		_default_session = _tt_c_mp->default_c_session;
#endif
		if (_default_session.is_null()) {
			return(TT_ERR_NOMP);
		}
	}
		
	_version = TT_RPC_VERSION;
	_pid = getpid();
	if (! _tt_global->get_local_host(_proc_host)) {
		return(TT_ERR_NOMP);
	}
	_proc_host_ipaddr = _proc_host->addr();
		
	status = _default_session->call(TT_RPC_ALLOC_PROCID_KEY,
					(xdrproc_t)xdr_void, 0,
					(xdrproc_t)tt_xdr_string,
					(char *)&_id);
		
	
	if (status != TT_OK) {
		return(status);
	}
	if (!_id.len()) {
		return(TT_ERR_INTERNAL);
	}
	_id = _id.cat(" ").cat(_default_session->address_string());
		
	// now set up an fd channel which is used by the
	// server side to notify this procid of new messages
	// as well as a notification mechanism of this procid
	// going away (by breaking the fd channel).
	
	if (set_fd_channel() && fd() != -1) {
		return(TT_OK);
	} else {
		return(TT_ERR_PROCID);
	}
}


// 
// Retrieves the next message for this procid. Conceptually, this means
// retrieving the next message from the queue of undelivered messages
// (the _undelivered field). This field is mainly kept on the server-side
// so it involves an rpc call to the server to dequeue the next message
// for this procid. For optimization's sake, this method is coded so the
// rpc call actually returns a list of messages. This allows batching of
// message delivery. The list is assigned to our local copy of
// _undelivered and messages are consumed from this list until it is
// exhausted at which time we invoke the rpc call again. 
// 
// If OPT_ADDMSG_DIRECT is defined then it will read the message directly
// from the signalling socket.
// 
Tt_status _Tt_c_procid::
next_message(_Tt_c_message_ptr &msg)
{
	_Tt_procid_ptr		proc = this;

	if (! _unvoted.is_null()) {
		update_message( _unvoted, TT_ABSTAINED );
		_unvoted = 0;
	}

#ifdef OPT_ADDMSG_DIRECT
	if (default_session()->rpc_version() >= TT_ADDMSG_VERS) {
		if (_mxdr_stream == (XDR *)0) {
			_mxdr_stream = (XDR *)malloc(sizeof(XDR));
			xdrrec_create(_mxdr_stream, 0, 0,
				      (char *)_socket.c_pointer(),
				      (int (*)())_tt_xdr_readit,
				      (int (*)())_tt_xdr_writeit);
			_mxdr_stream->x_op = XDR_DECODE;
		}
		msg = (_Tt_message *)0;
		(void)xdrrec_skiprecord(_mxdr_stream);
		if (! msg.xdr(_mxdr_stream)) {
			msg = (_Tt_message *)0;
			if (_flags&(1<<_TT_PROC_FD_CHANNEL_ON)) {
				dup2(_socket->sock(), _socket->fd());
				_flags &= ~(1<<_TT_PROC_FD_CHANNEL_ON);
			}
			return(TT_ERR_NOMP);
		}
		if (! msg.is_null()) {
			if (msg->state() == TT_STARTED && handling(msg)) {
				msg->set_state(TT_SENT);
				msg->set_start_message();
			}
		
			// We need to notify the server that we've
			// read the message. Otherwise, it would never
			// know that it should send us messages
			// through the fd again. This notification
			// also tells the server to reset the timer
			// for this message.
			
			// XXX: it would be nice to find a protocol
			// that didn't need this extra rpc call.
			
			default_session()->call(TT_RPC_MSGREAD_2,
					       (xdrproc_t)tt_xdr_procid,
					       (char *)&proc,
					       (xdrproc_t)xdr_void,
					       (char *)0);
			return(TT_OK);
		}
	}
#endif				// OPT_ADDMSG_DIRECT

	// We're here if we're talking to a version 1 server or if a
	// null message ptr was sent down the signalling fd. This
	// means that we are to use an rpc call to get the message for
	// us. 

	Tt_status		rstatus = TT_OK;

	if (_undelivered.is_null() || _undelivered->count() == 0) {
		_Tt_next_message_args	args;

		rstatus = default_session()->call(TT_RPC_NEXT_MESSAGE,
						 (xdrproc_t)tt_xdr_procid,
						 (char *)&proc,
						 (xdrproc_t)tt_xdr_next_message_args,
						 (char *)&args);
		switch (rstatus) {
		      case TT_OK:
			_undelivered = args.msgs;
			// set the signalled flag to tell us whether we should
			// clear the new message signal when all messages have
			// been retrieved from the local _undelivered queue.
			if (args.clear_signal) {
				_flags &= ~(1<<_TT_PROC_SIGNALLED);
			} else {
				_flags |= (1<<_TT_PROC_SIGNALLED);
			}
			break;
		      case TT_ERR_NOMP:
			// server got disconnected either because it
			// was killed or because of network errors.
			// We now dup the previous fd so that it won't
			// stay active.
			if (_flags&(1<<_TT_PROC_FD_CHANNEL_ON)) {
				dup2(_socket->sock(), _socket->fd());
				_flags &= ~(1<<_TT_PROC_FD_CHANNEL_ON);
			}
			break;
		      default:
			break;
		}
	}

	switch (rstatus) {
	      case TT_OK:
	      case TT_WRN_NOTFOUND:
		break;
	      default:
		return(rstatus);
	}

	if (_undelivered.is_null() || _undelivered->count() == 0) {
		msg = 0;
	} else {
		msg = (_Tt_c_message *)_undelivered->bot().c_pointer();
		_undelivered->dequeue();
			
		if (msg->queue() && msg->scope() == TT_FILE) {
			_Tt_file_ptr	fptr;
			
			if (_tt_mp->find_file(msg->file(), fptr, 1) == TT_OK) {
				((_Tt_c_file *)fptr.c_pointer())->
				 process_message_queue(0);
			}
		}
		if (msg->state() == TT_STARTED && processing(*msg)) {
			msg->set_state(TT_SENT);
			msg->set_start_message();
		}
		

#ifdef OPT_ADDMSG_DIRECT
		if (default_session()->rpc_version() >= TT_ADDMSG_VERS) {
			return(rstatus);
		}
#endif
		// unless we're getting messages through the
		// signalling fd, we have to clear the input from the
		// fd if there are no more messages.

		if ((! (_flags&(1<<_TT_PROC_SIGNALLED))) &&
		    (_undelivered.is_null()
		     || _undelivered->count() == 0)) {
			clear_signal();
		}
		if ((msg->message_class() == TT_OFFER) && processing(*msg)) {
			_unvoted = msg;
		}
	}
	
	return(rstatus);
}


//
// Quits out of any joined files.
//
void _Tt_c_procid::
close()
{
	_Tt_string_list_cursor	filec(_joined_files);
	_Tt_c_file_ptr		fp;
	_Tt_procid_ptr		pptr = this;
		
	while (filec.next()) {
		if (_tt_mp->find_file(*filec, fp, 0) == TT_OK) {
			(void)fp->c_quit(pptr);
		}
		filec.remove();
	}
}


// 
// Sets the default session for this procid. If a zero-length string is
// passed in as the session id then the procid is detached from the
// current session.
// 
Tt_status _Tt_c_procid::
set_default_session(_Tt_string &sid)
{
	_Tt_c_session_ptr		session;
	Tt_status			status;
	
	if (sid.len() == 0) { // detach from default session
		default_session() = (_Tt_c_session *)0;
		_flags &= ~(1<<_TT_PROC_COMMITTED);
		
		status = TT_OK;
	} else {		// set default session
		// can only join one session for now. If a session is
		// "committed" then we can't switch sessions for this procid.

		_Tt_string this_id = default_session()->id();
		if (_flags&(1<<_TT_PROC_COMMITTED)) {
			if (sid != this_id) {
				return(TT_ERR_SESSION);
			}
		}
		
		// initialize session from sid
		status = _tt_c_mp->find_session(sid, default_session(), 1, 1);
	}
	return(status);
}


// 
// Updates the message with the same id as "msg" that is presumed to be in
// the queue of delivered messages for this procid on the server-side.
// (see _Tt_s_procid::update_message for details).  This method is
// invoked when a handler is replying, failing, or rejecting a message.
// It is also involved in the special case of an observer replying to its
// start message.
// 
Tt_status _Tt_c_procid::
update_message(const _Tt_c_message_ptr &msg, Tt_state newstate)
{
	Tt_status		rstatus;
	_Tt_update_args		args;
	int			clr_obs = 0;

	if (msg.is_null()) {
		return TT_ERR_POINTER;
	}
	switch (newstate) {
	    case TT_HANDLED:
		if (msg->is_start_message()) {
			// OK to reply() instead of accept() 
			break;
		}
		if (msg->message_class() != TT_REQUEST) {
			return TT_ERR_CLASS;
		}
		if (msg->handler().is_null()) {
			return TT_ERR_NOTHANDLER;
		}
		if (! is_equal( msg->handler())) {
			return TT_ERR_NOTHANDLER;
		}
		break;
	    case TT_ABSTAINED:
		if (msg->message_class() != TT_OFFER) {
			return TT_ERR_CLASS;
		}
		break;
	    case TT_REJECTED:
		if ((msg->state() != TT_SENT) && (msg->state() != TT_STARTED)) {
			return TT_ERR_STATE;
		}
	}
	// if this message is already in a "final" state then
	// don't allow the state change
	switch (msg->state()) {
	      case TT_FAILED:
	      case TT_HANDLED:
	      case TT_RETURNED:
		return TT_ERR_STATE;
	}

	msg->add_voter( this, newstate );

	// optimize message xdr'ing for this case
	msg->set_return_handler_flags();
	args.message = msg;
	args.newstate = newstate;

	// invoke the appropiate rpc procedure depending on the
	// version of the server we're connected with
	switch (default_session()->rpc_version()) {
	      case 1:
		Tt_status		status;

		rstatus = default_session()->call(TT_RPC_UPDATE_MSG,
						 (xdrproc_t)tt_xdr_update_args,
						 (char *)&args,
						 (xdrproc_t)xdr_int,
						 (char *)&status);
		msg->set_state(newstate);
		return((rstatus == TT_OK) ? status : rstatus);
	      case 2:
	      default:
		if (msg->is_start_message() && msg->handler().is_null()) {
			//
			// If this procid is an observer replying to
			// its start message then we have to
			// temporarily set its handler procid in order
			// to tell ttsession which procid we are.
			// See _tt_update_msg() for more details. 
			//
			msg->set_observer_procid(this);
			clr_obs = 1;
		}
		rstatus = default_session()->call(TT_RPC_UPDATE_MSG_2,
						 (xdrproc_t)tt_xdr_update_args,
						 (char *)&args,
						 (xdrproc_t)xdr_void,
						 (char *)0);
		if (clr_obs) {
			// clear the handler procid if it was just set
			// for the special case of an observer
			// replying to a start message.

			msg->clr_observer_procid();
		}

		// update the state of the message to the new state
		msg->set_state(newstate);
		return(rstatus);
	}
}


// 
// Sets the signalling channel for this procid to an fd which is
// implemented here by opening a TCP socket so signal the procid. It is
// desirable to use a TCP socket because it is "reliable" which means
// that when signalling the procid fails, it's a good bet the procid has
// gone away.
// 
// This method is somewhat tricky because it has to open a socket and
// leave the socket in a state ready to accept connections (the details
// of this are in _Tt_stream_socket::init). Then it invokes an rpc call
// telling the server the port number of the socket. At this point the
// server-side method _Tt_s_procid::set_fd_channel will attempt to
// connect to this socket. The _Tt_c_procid::fd method will complete this
// socket connection (by invoking the _Tt_stream_socket::fd method) so
// it should be called after this method is called. This is done above
// in _Tt_c_procid::init. 
// 
//  XXX: It seems like the code in _Tt_c_procid::fd should have just been
//  included before returning in this method.
// 
int _Tt_c_procid::
set_fd_channel(int p)
{
	_Tt_fd_args		fd_args;
	Tt_status		rstatus;
	Tt_status		status;
		
	if (_flags&(1<<_TT_PROC_FD_CHANNEL_ON)) {
		return(1);
	}
	_socket = new _Tt_stream_socket(_proc_host, p);
	if (! _socket->init(1)) {
		_socket = (_Tt_stream_socket *)0;
		return(-1);
	}
	
	fd_args.procid = this;
	fd_args.start_token = _start_token;
	fd_args.fd = port();
	rstatus = default_session()->call(TT_RPC_SET_FD_CHANNEL,
					 (xdrproc_t)tt_xdr_fd_args,
					 (char *)&fd_args,
					 (xdrproc_t)xdr_int,
					 (char *)&status);
	if (rstatus != TT_OK || status != TT_OK) {
		return(0);
	}
	// clear the start token so the server won't attempt
	// to issue a proc_started event on the ptype each
	// time we contact it.
	_start_token = (char *)0;
	
	_flags |= (1<<_TT_PROC_FD_CHANNEL_ON);
	return(1);
}


// 
// Clears the signalling socket by reading the byte that gets sent to it
// by the server to notify the arrival of new messages. A read is avoided
// if it would block.
// 
void _Tt_c_procid::
clear_signal()
{
	char		sig[2];
	
	if (_flags&(1<<_TT_PROC_FD_CHANNEL_ON)) {
		sig[1] = 0;
		if (_socket->read_would_block() != 1) {
			return;
		}
		if (_socket->recv(sig, 1) < 0) {
		}
	}
}


// 
// Sets the default ptype for this procid.
// 
void _Tt_c_procid::
set_default_ptype(_Tt_string &ptid)
{
	_default_ptype = ptid;
}


// 
// Sets the default file for this procid.
// 
void _Tt_c_procid::
set_default_file(const _Tt_string &file)
{
	_default_file = file;
}

