/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: mp_trace.h /main/3 1995/10/23 10:30:33 rswiston $ 			 				 */
/* -*-C++-*-
 *
 * @(#)mp_trace.h	1.2 93/08/15
 * 
 * Copyright 1990,1993 Sun Microsystems, Inc.  All rights reserved.
 */
#ifndef _MP_TRACE_H
#define _MP_TRACE_H

#include "util/tt_trace.h"

class _Tt_msg_trace : public _Tt_trace {
    public:
	_Tt_msg_trace() {};
	_Tt_msg_trace(
		_Tt_message &msg,
		_Tt_dispatch_reason reason
	);
	_Tt_msg_trace(
		_Tt_message &msg,
		const _Tt_procid  &recipient
	);
	_Tt_msg_trace(
		_Tt_message &msg,
		Tt_state           old_state
	);
	~_Tt_msg_trace();
};


#endif
