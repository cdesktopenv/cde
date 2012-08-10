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
//%%  $TOG: tttkmessage.C /main/5 1998/03/19 18:34:28 mgreess $ 			 				
/*
 * @(#)tttkmessage.C	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include "api/c/tt_c.h"
#include "api/c/api_mp.h"
#include "tttk/tttk.h"
#include "tttk/tttkpattern.h"
#include "tttk/tttkmessage.h"
#include "tttk/tttk2free.h"
#include "tttk/tttkutils.h"
 
Tt_message
tttk_message_create( // XXX propagate contexts
	Tt_message		,
	Tt_class		theClass,
	Tt_scope		theScope,
	const char	       *handler,
	const char	       *op,
	Tt_message_callback	callback
)
{
	Tt_message msg = tt_message_create();
	Tt_status status = tt_ptr_error( msg );
	if (status != TT_OK) {
		return msg;
	}
	// fuse msg for automatic destruction if we return before defusing
	_TttkItem2Free temp = msg;
	status = tt_message_class_set( msg, theClass );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	status = tt_message_scope_set( msg, theScope );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	Tt_address address = TT_PROCEDURE;
	if (handler != 0) {
		status = tt_message_handler_set( msg, handler );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
		address = TT_HANDLER;
	}
	status = tt_message_address_set( msg, address );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	if (op != 0) {
		status = tt_message_op_set( msg, op );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
	}
	if (callback != 0) {
		status = tt_message_callback_add( msg, callback );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
	}
	temp = (caddr_t)0;  // defuse
	return msg;
}

// extern "C" { extern Tt_status _tt_message_destroy( Tt_message ); }

Tt_status
tttk_message_destroy(
	Tt_message msg
)
{
	Tt_pattern *pats1;
	Tt_pattern *pats2;
	pats1 = (Tt_pattern *)tt_message_user( msg, _TttkContractKey );
	pats2 = (Tt_pattern *)tt_message_user( msg, _TttkSubContractKey );
	Tt_status status = _tt_message_destroy( msg );
	if (status != TT_WRN_STOPPED) {
		_tttk_patterns_destroy( pats1 );
		_tttk_patterns_destroy( pats2 );
	}
	return status;
}

//
// This is the only message callback we ever use.  It retrieves
// the clientdata, client callback, and a tttk internal callback.
// That internal callback will parse the message and pass the
// info (along with the clientdata) to the client callback.
//
Tt_callback_action
_ttDtMessageCB(
	Tt_message msg,
	Tt_pattern pat
)
{
	_TtDtMessageCB cb = (_TtDtMessageCB)
		tt_message_user( msg, _TttkCBKey );
	Tt_status status = tt_ptr_error( cb );
	if (status != TT_OK) {
		return TT_CALLBACK_PROCESSED;
	}
	if (cb == 0) {
		return TT_CALLBACK_CONTINUE;
	}
	void *clientCB = tt_message_user( msg, _TttkClientCBKey );
	status = tt_ptr_error( clientCB );
	if (status != TT_OK) {
		return TT_CALLBACK_PROCESSED;
	}
	// clientCB is allowed to be 0.  cf. ttmedia_Deposit()
	void *clientData = tt_message_user( msg, _TttkClientDataKey );
	status = tt_ptr_error( clientData );
	if (status != TT_OK) {
		return TT_CALLBACK_PROCESSED;
	}
	msg = (*cb)( msg, pat, clientCB, clientData );
	return _ttDtCallbackAction( msg );
}

//
// Create a message and and store on it an internal callback, a
// client callback, and a client datum.
//
Tt_message
_ttDtPMessageCreate(
	Tt_message		context,
	Tt_class		theClass,
	Tt_scope		theScope,
	const char	       *handler,
	Tttk_op			op,
	_TtDtMessageCB		cb,
	void		       *clientCB,
	void		       *clientData
)
{
	Tt_message msg = tttk_message_create( context, theClass, theScope,
				handler, _ttDtOp( op ), _ttDtMessageCB );
	Tt_status status = tt_ptr_error( msg );
	if (status != TT_OK) {
		return msg;
	}
	_TttkItem2Free fuse( msg );
	status = tt_message_user_set( msg, _TttkCBKey, (void *)cb );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	status = tt_message_user_set( msg, _TttkClientCBKey, clientCB );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	status = tt_message_user_set( msg, _TttkClientDataKey, clientData );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	fuse = (caddr_t)0;
	return msg;
}

//
// Set the status info on a message, fail or reject it, and optionally
// destroy it.
//
Tt_status
_ttDtMessageGong(
	Tt_message  msg,
	Tt_status   status,
	const char *statusString,
	int         reject,
	int	    destroy
)
{
	tt_message_status_set( msg, status );
	if (statusString != 0) {
		tt_message_status_string_set( msg, statusString );
	}
	Tt_status ttStatus;
	if (reject) {
		ttStatus = tt_message_reject( msg );
	} else {
		ttStatus = tt_message_fail( msg );
	}
	if (destroy) {
		tttk_message_destroy( msg );
	}
	return ttStatus;
}

Tt_status
tttk_message_reject(
	Tt_message  msg,
	Tt_status   status,
	const char *statusString,
	int	    destroy
)
{
	return _ttDtMessageGong( msg, status, statusString, 1, destroy );
}

Tt_status
tttk_message_fail(
	Tt_message  msg,
	Tt_status   status,
	const char *statusString,
	int	    destroy
)
{
	if (tt_message_class( msg ) == TT_REQUEST) {
		return _ttDtMessageGong( msg, status, statusString, 0,
					 destroy );
	} else if (destroy) {
		return tttk_message_destroy( msg );
	}
	return TT_OK;
}

//
// Set the default procid and call tt_message_receive()
//
Tt_message
_tttk_message_receive(
	const char *procid
)
{
	Tt_status          status;
	if (procid != 0) {
		status = tt_default_procid_set( procid );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
	}
	return tt_message_receive();
}

//
// Used internally to reply to messages that might be edicts (i.e. notices)
//
Tt_status
_tttk_message_reply(
	Tt_message  msg
)
{
	if (tt_message_class( msg ) == TT_REQUEST) {
		return tt_message_reply( msg );
	}
	return TT_OK;
}

Tt_status
tttk_message_abandon(
	Tt_message msg
)
{
	if (_tttk_message_am_handling( msg )) {
		int fail = 0;
		if (tt_message_address( msg ) == TT_HANDLER) {
			fail = 1;
		} else if (tt_message_status( msg ) == TT_WRN_START_MESSAGE) {
			fail = 1;
		}
		if (fail) {
			return tttk_message_fail(  msg, TT_DESKTOP_ENOTSUP,0,1);
		} else {
			return tttk_message_reject(msg,	TT_DESKTOP_ENOTSUP,0,1);
		}
	} else {
		return tttk_message_destroy( msg );
	}
}

//
// Returns the sender-set message id if any, otherwise the tt_message_id()
//
char *
_tttk_message_id(
	Tt_message msg,
	int        arg
)
{
	int numArgs = tt_message_args_count( msg );
	Tt_status status = tt_int_error( numArgs );
	if (status != TT_OK) {
		return 0;
	}
	for (int i = arg; i < numArgs; i++) {
		char *type = tt_message_arg_type( msg, i );
		status = tt_ptr_error( type );
		if (status != TT_OK) {
			return 0;
		}
		if (type == 0) {
			continue;
		}
		int miss = strcmp( type, Tttk_message_id );
		tt_free( type );
		if (miss) {
			continue;
		}
		char *msgID = tt_message_arg_val( msg, i );
		status = tt_ptr_error( type );
		if (status != TT_OK) {
			return 0;
		}
		return msgID;
	}
	return tt_message_id( msg );
}

int
_tttk_message_in_final_state(
	Tt_message msg
)
{
	Tt_state theState = tt_message_state( msg );
	switch (tt_message_class( msg )) {
	    case TT_NOTICE:
		return theState == TT_SENT;
	    case TT_REQUEST:
		return (theState == TT_HANDLED) || (theState == TT_FAILED);
	    case TT_OFFER:
		return theState == TT_RETURNED;
	}
	return 0;
}

//
// Can I reply to this message?
//
int
_tttk_message_am_handling(
	Tt_message msg
)
{
	if (tt_message_class( msg ) != TT_REQUEST) {
		return 0;
	}
	if (tt_message_state( msg ) != TT_SENT) {
		return 0;
	}
	char *handler = tt_message_handler( msg );
	int am_handling = 0;
	if ((tt_ptr_error( handler ) == TT_OK) && (handler != 0)) {
		am_handling = 1;
	}
	tt_free( handler );
	return am_handling;
}

//
// Get arg value and return default_val on error.
//
int
_tttk_message_arg_ival(
	Tt_message		msg,
	unsigned int		n,
	int			default_val
)
{
	if (! _tttk_message_arg_is_set( msg, n )) {
		return default_val;
	}
	int val;
	Tt_status status = tt_message_arg_ival( msg, n, &val );
	if (status != TT_OK) {
		return default_val;
	}
	return val;
}

char *
_tttk_message_arg_val(
	Tt_message		msg,
	unsigned int		n,
	const char	       *default_val
)
{
	if (! _tttk_message_arg_is_set( msg, n )) {
		return (char *)default_val;
	}
	char *val = tt_message_arg_val( msg, n );
	Tt_status status = tt_ptr_error( val );
	if (status != TT_OK) {
		return (char *)default_val;
	}
	return val;
}

//
// Does arg n have a value?
//
int
_tttk_message_arg_is_set(
	Tt_message		msg,
	unsigned int		n
)
{
	int val;
	Tt_status status = tt_message_arg_ival( msg, n, &val );
	if (status == TT_OK) {
		return 1;
	}
	if (status != TT_ERR_NUM) {
		return 0;
	}
	unsigned char *s;
	int len;
	status = tt_message_arg_bval( msg, n, &s, &len );
	if (status != TT_OK) {
		return 0;
	}
	return (s != 0);
}

//
// Generic _TtDtMessageCB that sets *clientData to the negated
// tt_message_status() of the reply (or failure) of the message.
// Useful when sending synchronous requests and you only care to
// know the Tt_status of the reply/failure.
//
Tt_message
_ttTkNoteReplyStatus(
	Tt_message	msg,
	Tt_pattern	,
	void	       *,
	void	       *clientData
)
{
	if (! _tttk_message_in_final_state( msg )) {
		return msg;
	}
	int *result = (int *)clientData;
	*result = -TT_DESKTOP_ETIMEDOUT;
	switch (tt_message_state( msg )) {
	    case TT_HANDLED:
		*result = -TT_OK;
		break;
	    case TT_FAILED:
		*result = -tt_message_status( msg );
		if (*result == -TT_OK) {
			//
			// The handler did not say why he failed the
			// request, so we have to make up a reason
			// to pass back through e.g. ttdt_Save().
			//
			*result = -TT_DESKTOP_EPROTO;
		}
		break;
	}
	tttk_message_destroy( msg );
	// Protect against bogus message status causing infinite loop
	if (*result > 0) {
		*result = -(*result);
	}
	return 0;
}
