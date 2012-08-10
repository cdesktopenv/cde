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
//%%  $XConsortium: tttkpattern.C /main/3 1995/10/23 10:33:41 rswiston $ 			 				
/*
 * @(#)tttkpattern.C	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include "api/c/tt_c.h"
#include "tttk/tttk.h"
#include "tttk/tttkmessage.h"
#include "tttk/tttk2free.h"
#include "tttk/tttkutils.h"

Tt_status
_tttk_patterns_destroy(
	Tt_pattern	       *pats
)
{
	if (pats == 0) {
		return TT_OK;
	}
	Tt_status worst = tt_ptr_error( pats );
	if (worst != TT_OK) {
		return worst;
	}
	Tt_pattern *pats2free = pats;
	while (*pats != 0) {
		if (tt_ptr_error( *pats ) == TT_OK) {
			Tt_status status = tt_pattern_destroy( *pats );
			if (status != TT_OK) {
				worst = status;
			}
		}
		pats++;
	}
	if (pats2free != 0) {
		free( (caddr_t)pats2free );
	}
	return worst;
}

//
// This is the only pattern callback that we use.  It retrieves and
// calls the op-specific internal unmarshalling routine, which in turn
// will call the user's op-specific callback.
//
static Tt_callback_action
_ttDtPatternCB(
	Tt_message msg,
	Tt_pattern pat
)
{
	// XXX optimization: use 1 key, store a struct w/ all 3 datums
	_TtDtMessageCB cb = (_TtDtMessageCB)
		tt_pattern_user( pat, _TttkCBKey );
	Tt_status status = tt_ptr_error( cb );
	if (status != TT_OK) {
		return TT_CALLBACK_PROCESSED;
	}
	if (cb == 0) {
		return TT_CALLBACK_CONTINUE;
	}
	void *clientCB = tt_pattern_user( pat, _TttkClientCBKey );
	status = tt_ptr_error( clientCB );
	if (status != TT_OK) {
		return TT_CALLBACK_PROCESSED;
	}
	// clientCB is allowed to be 0. cf. TTDT_QUIT in ttdt_session_join
	void *clientData = tt_pattern_user( pat, _TttkClientDataKey );
	status = tt_ptr_error( clientData );
	if (status != TT_OK) {
		return TT_CALLBACK_PROCESSED;
	}
	msg = (*cb)( msg, pat, clientCB, clientData );
	return _ttDtCallbackAction( msg );
}

Tt_pattern
_ttDtPatternCreate(
	Tt_category		category,
	Tt_scope		theScope,
	int			addDefaultSess,
	const char	       *file,
	Tttk_op			op,
	_TtDtMessageCB		cb,
	void		       *clientCB,
	void		       *clientData,
	int			registerIt
)
{
	Tt_pattern pat = tt_pattern_create();
	Tt_status status = tt_ptr_error( pat );
	if (status != TT_OK) {
		return pat;
	}
	_TttkItem2Free fuse( pat );
	status = tt_pattern_category_set( pat, category );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	// we only observe notices and only handle requests
	if (category == TT_HANDLE) {
		status = tt_pattern_class_add( pat, TT_REQUEST );
	}
	if (category == TT_OBSERVE) {
		status = tt_pattern_class_add( pat, TT_NOTICE );
	}
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	status = tt_pattern_scope_add( pat, theScope );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	if (addDefaultSess) {
		char *sess = tt_default_session();
		status = tt_pattern_session_add( pat, sess );
		tt_free( sess );
		if (status != TT_OK) {
			return (Tt_pattern)tt_error_pointer( status );
		}
	}
	if (file != 0) {
		status = tt_pattern_file_add( pat, file );
		if (status != TT_OK) {
			return (Tt_pattern)tt_error_pointer( status );
		}
	}
	if (op != 0) {
		status = tt_pattern_op_add( pat, _ttDtOp( op ) );
		if (status != TT_OK) {
			return (Tt_pattern)tt_error_pointer( status );
		}
	}
	status = tt_pattern_callback_add( pat, _ttDtPatternCB );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	status = tt_pattern_user_set( pat, _TttkCBKey, (void *)cb );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	status = tt_pattern_user_set( pat, _TttkClientCBKey, clientCB );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	status = tt_pattern_user_set( pat, _TttkClientDataKey, clientData );
	if (status != TT_OK) {
		return (Tt_pattern)tt_error_pointer( status );
	}
	if (registerIt) {
		status = tt_pattern_register( pat );
		if (status != TT_OK) {
			return (Tt_pattern)tt_error_pointer( status );
		}
	}
	fuse = (caddr_t)0;
	return pat;
}

Tt_message
_tttk_pattern_contract(
	Tt_pattern pat
)
{
	void *p = tt_pattern_user( pat, _TttkContractKey );
	Tt_status status = tt_ptr_error( p );
	if (status != TT_OK) {
		return 0;
	}
	return (Tt_message)p;
}
