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
