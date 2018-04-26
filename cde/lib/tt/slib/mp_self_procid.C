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
//%%  $XConsortium: mp_self_procid.C /main/3 1995/10/23 12:00:55 rswiston $ 			 				
/*
 * @(#)mp_self_procid.cc	1.3 93/07/25
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include <unistd.h>
#include <errno.h>
#include "mp_self_procid.h"
#include "mp_s_mp.h"
#include "mp/mp_session.h"
#include "mp/mp_message.h"
#include "mp/mp_arg.h"
#include "mp_s_pattern.h"
#include "util/tt_global_env.h"
#include "util/tt_host.h"
#include "util/tt_port.h"
#include "util/tt_trace.h"

_Tt_self_procid::_Tt_self_procid()
{
}

_Tt_self_procid::~_Tt_self_procid()
{
}

Tt_status
_Tt_self_procid::init()
{
	_id = _tt_s_mp->alloc_procid_key();
	_id = _id.cat(" ").cat(_tt_s_mp->initial_session->address_string());
	_pid = getpid();
	// XXX Probably could just set _version and return at this point
	
	// _Tt_s_procid::init() expects _proc_host_ipaddr to be initialized
	if (! _tt_global->get_local_host(_proc_host)) {
		return(TT_ERR_NOMP);
	}
	_proc_host_ipaddr = _proc_host->addr();
	return _Tt_s_procid::init();
}

//
// We just go ahead and process the messages when we signal ourself
// that we have new messages.
//
Tt_status
_Tt_self_procid::signal_new_message()
{
	// XXX do we need to muck with _TT_PROC_SIGNALLED?  I think not.
	_Tt_next_message_args args;
	Tt_status status = next_message( args );
	_Tt_message_list_cursor msgC( args.msgs );

	while (status == TT_OK) {
		//
		// In theory, we should at this point do the things
		// that _Tt_c_procid::next_message() and tt_message_receive()
		// do: check the queue for file-scoped messages,
		// and run pattern callbacks.  But ttsession declares
		// no ptypes, and we lack the client-side machinery
		// for callbacks.  So we use our own.
		//
		msgC.reset();
		while (msgC.next()) {
			_process_msg( *msgC );
		}
		status = next_message( args );
	}
	if (status == TT_WRN_NOTFOUND) {
		return TT_OK;
	}
	return status;
}

Tt_callback_action
_Tt_self_procid::handle_Session_Trace(
	const _Tt_message_ptr &msg,
	void		      *proc
)
{
	return ((_Tt_self_procid *)proc)->_handle_Session_Trace( msg );
}

Tt_callback_action
_Tt_self_procid::observe_Saved(
	const _Tt_message_ptr &msg,
	void		      *proc
)
{
	return ((_Tt_self_procid *)proc)->_observe_Saved( msg );
}

_Tt_s_pattern *
_Tt_self_procid::s_pattern_create()
{
	_Tt_s_pattern *pat = new _Tt_s_pattern();
	pat->set_id(_tt_s_mp->initial_session->address_string());
	return pat;
}

//
// This is a very simple callback server-side pattern callback scheme.
// It does not do everything that client-side callbacks do.
//
Tt_status
_Tt_self_procid::_process_msg(
	const _Tt_message_ptr &msg
)
{
	_Tt_string pat_id = msg->pattern_id();
	if (pat_id.len() == 0) {
		return TT_OK;
	}
	_Tt_pattern_list_cursor patC( _patterns );
	while (patC.next()) {
		if (patC->id() == pat_id) {
			if (patC->server_callback != 0) {
				if (    (*(patC->server_callback))( msg, this )
				     == TT_CALLBACK_PROCESSED)
				{
					break;
				}
			}
		}
	}
	return TT_OK;
}

Tt_status
_Tt_self_procid::_reply(
	const _Tt_message_ptr &msg
)
{
	return update_message( msg, TT_HANDLED );
}

Tt_status
_Tt_self_procid::_fail(
	const _Tt_message_ptr &msg,
	int return_status
)
{
	Tt_status status = msg->set_status( return_status );
	if (status != TT_OK) {
		return status;
	}
	return update_message( msg, TT_FAILED );
}

Tt_status
_Tt_self_procid::update_message(
	const _Tt_message_ptr &msg,
	Tt_state	       new_state
)
{
	//
	// This is the sort of checking done by
	// _Tt_c_procid::update_message().
	//
	if (msg->message_class() != TT_REQUEST) {
		return TT_ERR_CLASS;
	}
	if (msg->handler().is_null()) {
		return TT_ERR_NOTHANDLER;
	}
	_Tt_procid_ptr me = (_Tt_procid *)this;
	if (! me->is_equal( msg->handler() )) {
		return TT_ERR_NOTHANDLER;
	}
	switch (msg->state()) {
	      case TT_FAILED:
	      case TT_HANDLED:
		return(TT_ERR_INVALID);
	      default:
		break;
	}
	return _Tt_s_procid::update_message( msg, new_state );
}

Tt_callback_action
_Tt_self_procid::_handle_Session_Trace(
	const _Tt_message_ptr &msg
)
{
	_Tt_arg_list_ptr args = new _Tt_arg_list( *msg->args() );
	_Tt_arg_list_cursor argC( args );
	if (! argC.next()) {
		_fail( msg, TT_DESKTOP_EPROTO );
		return TT_CALLBACK_PROCESSED;
	}
	_Tt_string script;
	Tt_status status = (*argC)->data_string( script );
	if (status != TT_OK) {
		_fail( msg, (int)status );
		return TT_CALLBACK_PROCESSED;
	}
	if ((script.len() == 0) && (msg->file().len() > 0)) {
		script = _tt_network_path_to_local_path(msg->file());
	}
	if (argC.next()) {
		_fail( msg, TT_DESKTOP_ENOTSUP );
		return TT_CALLBACK_PROCESSED;
	}
	if (_tt_putenv( TRACE_SCRIPT, script ) == 0) {
		_fail( msg, TT_DESKTOP_ENOMEM );
		return TT_CALLBACK_PROCESSED;
	}
	tt_trace_control( 0 );
	tt_trace_control( 1 );
	_reply( msg );
	return TT_CALLBACK_PROCESSED;
}

Tt_callback_action
_Tt_self_procid::_observe_Saved(
	const _Tt_message_ptr &
)
{
	if (kill( getpid(), SIGTYPES ) < 0) {
		_tt_syslog(0, LOG_ERR, "kill(): %m");
	}
	return TT_CALLBACK_PROCESSED;
}
