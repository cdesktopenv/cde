//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_s_msg_context.C /main/3 1995/10/23 11:56:51 rswiston $ 			 				
/*
 *
 * @(#)mp_s_msg_context.C	1.4 29 Jul 1993
 *
 * Tool Talk Message Passer (MP) - mp_s_msg_context.cc
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 *
 * _Tt_s_msg_context knows the server side of the context RPC interface.
 */

#include <mp/mp_arg.h>
#include <mp/mp_procid.h>
#include "mp_s_pattern.h"
#include "mp_s_msg_context.h"

_Tt_s_msg_context::_Tt_s_msg_context()
{
}

_Tt_s_msg_context::~_Tt_s_msg_context()
{
}

Tt_status
_Tt_s_msg_context::s_join(
	_Tt_procid_ptr &procID
) const
{
	_Tt_pattern_list_cursor	patC( procID->patterns() );
	Tt_status		status;
	Tt_status		worstStatus = TT_OK;

	while (patC.next()) {
		status = ((_Tt_s_pattern &)**patC).join_context( *this );
		switch (status) {
		    case TT_OK:
		    case TT_WRN_NOTFOUND:
			break;
		    default:
			worstStatus = status;
		}
	}
	return worstStatus;
}

Tt_status
_Tt_s_msg_context::s_quit(
	_Tt_procid_ptr &procID
) const
{
	_Tt_pattern_list_cursor	patC( procID->patterns() );
	Tt_status		status;
	Tt_status		worstStatus = TT_OK;

	while (patC.next()) {
		status = ((_Tt_s_pattern &)**patC).quit_context( *this );
		switch (status) {
		    case TT_OK:
		    case TT_WRN_NOTFOUND:
			break;
		    default:
			worstStatus = status;
		}
	}
	return worstStatus;
}
