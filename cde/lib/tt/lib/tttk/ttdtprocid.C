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
//%%  $XConsortium: ttdtprocid.C /main/3 1995/10/23 10:32:27 rswiston $ 			 				
/*
 * @(#)ttdtprocid.C	1.3 93/09/07
 *
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */

#include <limits.h>
#include <X11/Intrinsic.h>
#include "util/tt_Xlib.h"
#include "api/c/api_api.h"
#include "tttk/ttdtprocid.h"
#include "tttk/tttkmessage.h"
#include "tttk/tttkpattern.h"
#include "tttk/tttkutils.h"
#include "tttk/tttk2free.h"
#include "tttk/ttdesktop.h"

_TtDtProcid::_TtDtProcid(
	const char     *toolName,
	const char     *vendor,
	const char     *version
)
{
	// XXX Initialize procid's default contexts from environ(5).
	_toolname = _tt_strdup( toolName );
	_vendor = _tt_strdup( vendor );
	_version = _tt_strdup( version );
}

_TtDtProcid::~_TtDtProcid()
{
	tt_free( _toolname );
	tt_free( _vendor );
	tt_free( _version );
}

//
// Create patterns for either session_join or message_accept
//
Tt_pattern *_TtDtProcid::
pats_create(
	Tt_message	contract,
	Ttdt_contract_cb cb,
	Widget		shell,
	void	       *clientData
) const
{
	int numPats = 11;
	if (contract == 0) {
		numPats = 13;
	}
	Tt_pattern *pats = (Tt_pattern *)malloc(numPats * sizeof(Tt_pattern));
	if (pats == 0) {
		return (Tt_pattern *)tt_error_pointer( TT_ERR_NOMEM );
	}
	int i = -1;
	char *sess = tt_default_session();
	if (shell != 0) {
		pats[++i] = ttdt_Get_Geometry_pat( TT_HANDLE, contract,
				_ttdt_do_GSet_Geometry, shell, 1 );
		pats[++i] = ttdt_Set_Geometry_pat( TT_HANDLE, contract,
				_ttdt_do_GSet_Geometry, shell, 1 );
		pats[++i] = ttdt_Get_Iconified_pat( TT_HANDLE, contract,
				_ttdt_do_wm_state, shell, 0 );
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_GET_MAPPED ) );
		tt_pattern_session_add( pats[i], sess );
		tt_pattern_register( pats[i] );
		pats[++i] = ttdt_Set_Iconified_pat( TT_HANDLE, contract,
				_ttdt_do_wm_state, shell, 0 );
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_SET_MAPPED ) );
		tt_pattern_session_add( pats[i], sess );
		tt_pattern_register( pats[i] );
		pats[++i] = ttdt_Get_XInfo_pat(	TT_HANDLE, contract,
				_ttdt_do_Get_XInfo, shell, 1 );
		pats[++i] = ttdt_Set_XInfo_pat(	TT_HANDLE, contract,
				_ttdt_do_Set_XInfo, shell, 1 );
		pats[++i] = _ttdt_pat(TTDT_RAISE, _ttdt_contract_cb, TT_HANDLE,
				contract, (void *)_ttdt_do_RaiseLower, shell, 0 );
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_LOWER ) );
		tt_pattern_session_add( pats[i], sess );
		tt_pattern_register( pats[i] );
	} else {
		pats[++i] = ttdt_Get_Geometry_pat( TT_HANDLE, contract,
				(Ttdt_Geometry_out_cb)cb, clientData, 1 );
		pats[++i] = ttdt_Set_Geometry_pat( TT_HANDLE, contract,
				(Ttdt_Geometry_out_cb)cb, clientData, 1 );
		pats[++i] = ttdt_Get_Iconified_pat( TT_HANDLE, contract,
				(Ttdt_Iconified_out_cb)cb, clientData, 0 );
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_GET_MAPPED ) );
		tt_pattern_session_add( pats[i], sess );
		tt_pattern_register( pats[i] );
		pats[++i] = ttdt_Set_Iconified_pat( TT_HANDLE, contract,
				(Ttdt_Iconified_out_cb)cb, clientData, 0 );
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_SET_MAPPED ) );
		tt_pattern_session_add( pats[i], sess );
		tt_pattern_register( pats[i] );
		pats[++i] = ttdt_Get_XInfo_pat(	TT_HANDLE, contract,
				(Ttdt_XInfo_out_cb)cb, clientData, 1 );
		pats[++i] = ttdt_Set_XInfo_pat(	TT_HANDLE, contract,
				(Ttdt_XInfo_in_cb)cb, clientData, 1 );
		pats[++i] = _ttdt_pat(TTDT_RAISE, _ttdt_contract_cb, TT_HANDLE,
				contract, (void *)cb, clientData, 0 );
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_LOWER ) );
		tt_pattern_session_add( pats[i], sess );
		tt_pattern_register( pats[i] );
	}
	void *_clientData = clientData;
	if (cb == 0) {
		// No callback, so set up our own client data
		if (contract == 0) {
			// tttk will send WM_DELETE_WINDOW to shell...
			_clientData = shell;
		} else {
			// ...but that quits entire app, so do not do it
			// for ttdt_message_accept().  Seeing no shell,
			// _ttdt_contract_cb() will fail the Quit.
			_clientData = 0;
		}
	}
	pats[++i] = _ttdt_pat(	TTDT_QUIT, _ttdt_contract_cb, TT_HANDLE,
				contract, (void *)cb, _clientData, 1 );
	pats[++i] = _ttdt_pat(	TTDT_PAUSE, _ttdt_contract_cb, TT_HANDLE,
				contract, (void *)cb, (cb ? clientData : shell), 0 );
	tt_pattern_op_add( pats[i], _ttDtOp( TTDT_RESUME ));
	tt_pattern_session_add( pats[i], sess );
	tt_pattern_register( pats[i] );
	pats[++i] = _ttdt_pat(	TTDT_GET_STATUS, _ttdt_contract_cb, TT_HANDLE,
				contract, (void *)cb, clientData, 1 );
	if (contract == 0) {
		pats[++i]= _ttdt_pat( TTDT_DO_COMMAND, _ttdt_contract_cb,
				      TT_HANDLE, contract, (void *)cb, clientData, 1 );
		pats[++i]= _ttdt_pat( TTDT_SET_LOCALE, _ttdt_posix_cb,
				      TT_HANDLE, contract, 0, 0, 0 );
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_GET_LOCALE ));
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_SET_ENVIRONMENT ));
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_GET_ENVIRONMENT ));
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_SET_SITUATION ));
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_GET_SITUATION ));
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_SIGNAL ));
		tt_pattern_op_add( pats[i], _ttDtOp( TTDT_GET_SYSINFO ));
		tt_pattern_session_add( pats[i], sess );
		tt_pattern_register( pats[i] );
	}
	pats[++i] = 0;
	Tt_pattern *pat = pats;
	while (*pat != 0) {
		Tt_status status = tt_ptr_error( *pat );
		if (status != TT_OK) {
			_tttk_patterns_destroy( pats );
			return (Tt_pattern *)tt_error_pointer( status );
		}
		pat++;
	}
	tt_free( sess );
	return pats;
}
