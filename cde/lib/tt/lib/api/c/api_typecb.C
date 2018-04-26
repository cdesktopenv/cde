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
//%%  $TOG: api_typecb.C /main/5 1999/10/14 18:39:57 mgreess $ 			 				
/* @(#)api_typecb.C	1.2 93/07/30 SMI
 *
 * Copyright (c) 1993
 * 
 * Implementation of the class that holds <typename,opnum,callbacklist>
 * triples that are attached to the _Tt_c_procid instance to hold callbacks
 * registered by tt_ptype_opnum_callback_add and tt_otype_opnum_callback_add.
 */

#include <stdlib.h>
#include "api/c/api_typecb.h"
#include "api/c/api_handle.h"
#include "util/tt_trace.h"
#include "util/tt_global_env.h"

_Tt_typecb::
_Tt_typecb()
{
	clientdata = 0;
	_opnum = 0;
}

_Tt_typecb::
_Tt_typecb(const _Tt_string &typenm, int opnum)
{
	_typename = typenm;
	_opnum = opnum;
	clientdata = 0;
}

_Tt_typecb::
~_Tt_typecb()
{
}

_Tt_string & _Tt_typecb::
get_typename()
{
	return _typename;
}

int _Tt_typecb::
opnum()
{
	return _opnum;
}

// XXX: these functions are copies of _Tt_api_handle::add_callback
// and _Tt_api_handle::run_callbacks.
// Should there be a common parent class of _Tt_api_handle and _Tt_typecb
// to reflect that they both contain lists of callbacks?
// Or should there be a "smart" callback-list class that wraps around
// the _Tt_api_callback_list class and implements add_ and run_?

void _Tt_typecb::
add_callback(Tt_message_callback f)
{     
	if (cblp.is_null()) {
		cblp = new _Tt_api_callback_list;
	}

	_Tt_api_callback_ptr cb = new _Tt_api_callback;
	cb->callback = f;

	cblp->push(cb);
}

Tt_callback_action _Tt_typecb::
run_callbacks(Tt_message mh, Tt_pattern ph)
{
	Tt_callback_action result = TT_CALLBACK_CONTINUE;
	_Tt_api_callback_list_cursor c(cblp);
	_Tt_trace trace;

	_tt_global->drop_mutex();
	
	while (result==TT_CALLBACK_CONTINUE && c.next()) {
		trace.entry(c->callback, mh, ph);
		result = (c->callback)(mh,ph);
		trace.exit(result);
	}

	_tt_global->grab_mutex();
	
	return result;
}

