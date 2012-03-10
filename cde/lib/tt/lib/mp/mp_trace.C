//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_trace.C /main/4 1995/11/21 19:25:54 cde-sun $ 			 				
/*
 * @(#)mp_trace.cc	1.2 93/08/15
 * 
 * Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 */
#include "mp/mp_trace.h"

_Tt_msg_trace::_Tt_msg_trace(
	_Tt_message &msg,
	_Tt_dispatch_reason reason
)
{
	entry( msg, reason );
}

_Tt_msg_trace::_Tt_msg_trace(
	_Tt_message &msg,
	const _Tt_procid  &recipient
)
{
	entry( msg, recipient );
}

_Tt_msg_trace::_Tt_msg_trace(
	_Tt_message &msg,
	Tt_state           old_state
)
{
	entry( msg, old_state );
}

_Tt_msg_trace::~_Tt_msg_trace()
{
	exitq();
}
