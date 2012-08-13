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
//%%  $XConsortium: ttdtfile.C /main/3 1995/10/23 10:32:19 rswiston $ 			 				
/*
 * @(#)ttdtfile.C	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include <unistd.h>
#include "api/c/tt_c.h"
#include "api/c/api_api.h"
#include "tttk/tttk.h"
#include "tttk/tttkpattern.h"
#include "tttk/tttkmessage.h"
#include "tttk/tttk2free.h"
#include "tttk/tttkutils.h"
 
typedef struct {
	Tt_scope	theScope;
	const char     *pathname;
	Ttdt_file_cb	cb;
	void	       *clientData;
} TtDtFileJoinInfo;

//
// Parse file message and pass it to user callback
//
static Tt_message
_ttDtFileCB(
	Tt_message	msg,
	Tt_pattern	,		// Not needed
	void	       *clientCB,
	void	       *clientData
)
{
	if (clientCB == 0) {
		return msg;
	}
	_TttkItem2Free fuse = msg;
	char *file = tt_message_file( msg );
	Tt_status status = tt_ptr_error( file );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	char *opname = tt_message_op( msg );
	status = tt_ptr_error( opname );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	Tttk_op op = tttk_string_op( opname );
	tt_free( opname );
	int trust = (    ( geteuid() == tt_message_uid( msg ))
		      && ( getegid() == tt_message_gid( msg )));
	int from_me = 0;
	char *sender = tt_message_sender( msg );
	char *me = tt_default_procid();
	if ((tt_ptr_error( sender ) == TT_OK) && (tt_ptr_error(me) == TT_OK)) {
		from_me = (strcmp( sender, me ) == 0);
	}
	tt_free( sender );
	tt_free( me );
	if ((op == TTDT_GET_MODIFIED) && (_tttk_message_am_handling(msg))) {
		//
		// If our pattern even matches Get_Modified, it is because
		// the client called ttdt_file_event(,TTDT_MODIFIED,,).
		// Reverting or saving unregisters this pattern, so if
		// we even see the question we know the answer is yes.
		//
		tt_message_arg_ival_set( msg, 1, 1 );
		tt_message_reply( msg );
		// message destroyed by fuse
		return 0;
	}
	Ttdt_file_cb _cb = (Ttdt_file_cb)clientCB;
	msg = (*_cb)( msg, op, file, clientData, trust, from_me );
	fuse = (caddr_t)0; // prevent message destruction
	return msg;
}

//
// Create the pattern registered at file-join-time
//
static Tt_pattern
_ttDtFileJoinPat(
	const char     *pathname,
	Tt_scope	theScope,
	Ttdt_file_cb	cb,
	void           *clientData
)
{
	Tt_pattern pat = _ttDtPatternCreate( TT_OBSERVE, theScope,
					     theScope != TT_FILE, pathname,
					     TTDT_DELETED, _ttDtFileCB,
					     (void *)cb, clientData, 0 );
	Tt_status status = tt_ptr_error( pat );
	if (status != TT_OK) {
		return pat;
	}
	_TttkItem2Free fuse = pat;
	tt_pattern_op_add( pat, _ttDtOp( TTDT_MODIFIED ) );
	tt_pattern_op_add( pat, _ttDtOp( TTDT_REVERTED ) );
	tt_pattern_op_add( pat, _ttDtOp( TTDT_MOVED ) );
	tt_pattern_op_add( pat, _ttDtOp( TTDT_SAVED ) );
	status = tt_pattern_class_add( pat, TT_NOTICE );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	status = tt_pattern_arg_add( pat, TT_IN, Tttk_file, 0 );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	status = tt_pattern_register( pat );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	fuse = (caddr_t)0;
	return pat;
}

Tt_pattern *
ttdt_file_join(
	const char     *pathname,
	Tt_scope	theScope,
	int		join,
	Ttdt_file_cb	cb,
	void           *clientData
)
{
	//
	// One pattern to observe notices, one to handle requests, and
	// a null terminator.
	//
	Tt_pattern *pPats = (Tt_pattern *)malloc( 3 * sizeof(Tt_pattern) );
	if (pPats == 0) {
		return (Tt_pattern *)tt_error_pointer( TT_ERR_NOMEM );
	}
	_TttkList2Free fuses( 3 );
	fuses += (caddr_t)pPats;
	pPats[1] = 0;
	pPats[2] = 0;
	Tt_scope toTry = theScope;
	if (theScope == TT_SCOPE_NONE) {
		toTry = TT_BOTH;
	}
	pPats[0] = _ttDtFileJoinPat( pathname, toTry, cb, clientData );
	Tt_status status = tt_ptr_error( pPats[0] );
	if ((status != TT_OK) && (theScope == TT_SCOPE_NONE)) {
		toTry = TT_FILE_IN_SESSION;
		pPats[0] = _ttDtFileJoinPat( pathname, toTry, cb, clientData );
		status = tt_ptr_error( pPats[0] );
	}
	if (status != TT_OK) {
		return (Tt_pattern *)tt_error_pointer( status );
	}
	fuses += pPats[0];
	TtDtFileJoinInfo *info = (TtDtFileJoinInfo *)
				 malloc( sizeof( TtDtFileJoinInfo ));
	if (info == 0) {
		return (Tt_pattern *)tt_error_pointer( TT_DESKTOP_ENOMEM );
	}
	fuses += (caddr_t)info;
	info->pathname = _tt_strdup( pathname );
	info->theScope = toTry;
	info->cb       = cb;
	info->clientData = clientData;
	status = tt_pattern_user_set( pPats[0], _TttkJoinInfoKey, info );
	if (status != TT_OK) {
		return (Tt_pattern *)tt_error_pointer( status );
	}
	if (join) {
		status = tt_file_join( pathname );
		if (status != TT_OK) {
			return (Tt_pattern *)tt_error_pointer( status );
		}
	}
	fuses.flush();
	return pPats;
}

Tt_message
ttdt_file_notice(
	Tt_message	context,
	Tttk_op		op,
	Tt_scope	theScope,
	const char     *file,
	int		sendAndDestroy
)
{
	if ((op == TTDT_MOVED) && sendAndDestroy) {
		return (Tt_message)tt_error_pointer( TT_DESKTOP_EINVAL );
	}
	Tt_message msg = tttk_message_create( context, TT_NOTICE, theScope, 0,
					      _ttDtOp( op ), 0 );
	Tt_status status = tt_ptr_error( msg );
	if (status != TT_OK) {
		return msg;
	}
	//
	// Guarantees that msg will be destroyed when this function returns
	//
	_TttkItem2Free fuse = msg;
	status = tt_message_file_set( msg, file );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	status = tt_message_arg_add( msg, TT_IN, Tttk_file, 0 );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	if (sendAndDestroy) {
		status = tt_message_send( msg );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
		return 0;
	}
	fuse = (caddr_t)0;
	return msg;
}

Tt_message
ttdt_file_request(
	Tt_message	context,
	Tttk_op		op,
	Tt_scope	theScope,
	const char     *file,
	Ttdt_file_cb	cb,
	void           *clientData,
	int		send
)
{
	Tt_message msg = _ttDtPMessageCreate( context, TT_REQUEST, theScope, 0,
					      op, _ttDtFileCB, (void *)cb, clientData);
	Tt_status status = tt_ptr_error( msg );
	if (status != TT_OK) {
		return msg;
	}
	_TttkItem2Free fuse = msg;
	status = tt_message_file_set( msg, file );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	status = tt_message_arg_add( msg, TT_IN, Tttk_file, 0 );
	if (status != TT_OK) {
		return (Tt_message)tt_error_pointer( status );
	}
	if (op == TTDT_GET_MODIFIED) {
		status = tt_message_arg_add( msg, TT_OUT, Tttk_boolean, 0 );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
	}
	if (send) {
		status = tt_message_send( msg );
		if (status != TT_OK) {
			return (Tt_message)tt_error_pointer( status );
		}
	}
	fuse = (caddr_t)0;
	return msg;
}

//
// Create the pattern registered at file-event-time
//
static Tt_pattern
_ttDtFileEventPat(
	const char     *pathname,
	Tt_scope	theScope,
	Ttdt_file_cb	cb,
	void           *clientData
)
{
	Tt_pattern pat = _ttDtPatternCreate( TT_HANDLE, theScope,
					     theScope != TT_FILE, pathname,
					     TTDT_SAVE, _ttDtFileCB,
					     (void *)cb, clientData, 0 );
	Tt_status status = tt_ptr_error( pat );
	if (status != TT_OK) {
		return pat;
	}
	_TttkItem2Free fuse = pat;
	tt_pattern_op_add( pat, _ttDtOp( TTDT_GET_MODIFIED ) );
	tt_pattern_op_add( pat, _ttDtOp( TTDT_REVERT ) );
	status = tt_pattern_class_add( pat, TT_REQUEST );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	status = tt_pattern_arg_add( pat, TT_IN, Tttk_file, 0 );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	status = tt_pattern_register( pat );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	fuse = (caddr_t)0;
	return pat;
}

Tt_status
ttdt_file_event(
	Tt_message	context,
	Tttk_op		event,
	Tt_pattern     *pats,
	int		send
)
{
	if ((pats == 0) || (tt_ptr_error( pats ) != TT_OK)) {
		return TT_ERR_POINTER;
	}
	TtDtFileJoinInfo *info = (TtDtFileJoinInfo *)
				 tt_pattern_user( pats[0], _TttkJoinInfoKey );
	Tt_status status = tt_ptr_error( info );
	if ((status != TT_OK) || (info == 0)) {
		return status;
	}
	if (event == TTDT_MODIFIED) {
		pats[1] = _ttDtFileEventPat( info->pathname, info->theScope,
					     info->cb, info->clientData );
		Tt_status status = tt_ptr_error( pats[1] );
		if (status != TT_OK) {
			// Leave pats[1] as an error pointer
			return status;
		}
	}
	if (send) {
		Tt_message msg = ttdt_file_notice( context, event,
					info->theScope, info->pathname, 1 );
		status = tt_ptr_error( msg );
		if (status != TT_OK) {
			return status;
		}
	}
	if ((event == TTDT_SAVED) || (event == TTDT_REVERTED)) {
		status = tt_pattern_unregister( pats[1] );
		if (status != TT_OK) {
			return status;
		}
	}
	return TT_OK;
}

Tt_status
ttdt_file_quit(
	Tt_pattern     *pats,
	int		quit
)
{
	if ((pats == 0) || (tt_ptr_error( pats ) != TT_OK)) {
		return TT_ERR_POINTER;
	}
	TtDtFileJoinInfo *info = (TtDtFileJoinInfo *)
				 tt_pattern_user( pats[0], _TttkJoinInfoKey );
	Tt_status status = tt_ptr_error( info );
	if ((status != TT_OK) || (info == 0)) {
		return status;
	}
	status = _tttk_patterns_destroy( pats );
	if (quit) {
		if (info->pathname != 0) {
			status = tt_file_quit( info->pathname );
			tt_free( (char *)info->pathname );
		}
		free( info );
	}
	return status;
}

//
// Parse Get_Modified reply and set *(int *)clientData
//
static Tt_message
_ttDtGetModifiedCB(
	Tt_message      msg,
	Tttk_op		,
	char           *pathName,
	void	       *clientData,
	int		,
	int
)
{
	tt_free( pathName );
	if (! _tttk_message_in_final_state( msg )) {
		// Not in final state; our address space is probably handler
		return msg;
	}
	*(int *)clientData = 0; // assume not modified
	if (tt_message_state(msg) == TT_HANDLED) {
		int modified;
		Tt_status status = tt_message_arg_ival( msg, 1, &modified );
		if (status != TT_OK) {
			tttk_message_destroy( msg );
			return 0;
		}
		if (modified) {
			*(int *)clientData = -1;
		}
	}
	tttk_message_destroy( msg );
	return 0;
}

int
ttdt_Get_Modified(
	Tt_message	context,
	const char     *pathname,
	Tt_scope	theScope,
	XtAppContext	app2run,
	int		msTimeOut
)
{
	int waiting = 1;
	Tt_message msg = ttdt_file_request( context, TTDT_GET_MODIFIED,
					    theScope, pathname,
					    _ttDtGetModifiedCB, &waiting, 1 );
	Tt_status status = tt_ptr_error( msg );
	if (status != TT_OK) {
		return 0;
	}
	status = tttk_block_while( app2run, &waiting, msTimeOut );
	if (status == TT_DESKTOP_ETIMEDOUT) {
		//
		// _ttDtGetModifiedCB() would have destroyed msg,
		// but this removes the callback.  Now, any
		// late-breaking reply will pop out of
		// tt_message_receive().  Well-behaved applications
		// should do [what is done by] tttk_message_abandon(),
		// but poorly-written apps might panic. XXX
		//
		tttk_message_destroy( msg );
	}
	return (waiting < 0);
}

//
// Private Ttdt_file_cb hung on requests (Get_Modified, Save, Revert)
// that we send synchronously.
//
static Tt_message
_ttDtFileRequestCB(
	Tt_message      msg,
	Tttk_op		,
	char           *pathName,
	void	       *clientData,
	int		,
	int
)
{
	tt_free( pathName );
	return _ttTkNoteReplyStatus( msg, 0, 0, clientData );
}

Tt_status
_ttdt_file_request(
	Tttk_op		op,
	Tt_message	context,
	const char     *pathname,
	Tt_scope	theScope,
	XtAppContext	app2run,
	int		msTimeOut
)
{
	int result;
	Tt_message msg = ttdt_file_request( context, op,
					    theScope, pathname,
					    _ttDtFileRequestCB, &result, 1 );
	Tt_status status = tt_ptr_error( msg );
	if (status != TT_OK) {
		return status;
	}
	result = 1;
	status = tttk_block_while( app2run, &result, msTimeOut );
	if (status == TT_DESKTOP_ETIMEDOUT) {
		tttk_message_destroy( msg );
		return status;
	}
	return (Tt_status)-result;
}

Tt_status
ttdt_Save(
	Tt_message	context,
	const char     *pathname,
	Tt_scope	theScope,
	XtAppContext	app2run,
	int		msTimeOut
)
{
	return _ttdt_file_request( TTDT_SAVE, context, pathname, theScope,
				   app2run, msTimeOut );
}

Tt_status
ttdt_Revert(
	Tt_message	context,
	const char     *pathname,
	Tt_scope	theScope,
	XtAppContext	app2run,
	int		msTimeOut
)
{
	return _ttdt_file_request( TTDT_REVERT, context, pathname, theScope,
				   app2run, msTimeOut );
}
