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
// $XConsortium: tt_threadspecific.C /main/3 1996/05/09 20:50:37 drk $
//%%  (c) Copyright 1995 Sun Microsystems, Inc.			

#include <util/tt_threadspecific.h>
#ifdef OPT_XTHREADS

_Tt_threadspecific::
_Tt_threadspecific(const _Tt_c_session_ptr& sess, const _Tt_c_procid_ptr& proc)
{
	_thread_c_session = sess;
	_thread_c_procid = proc;
	_thread_stgstack = 0;
}

_Tt_api_stg_stack* _Tt_threadspecific::
thread_stgstack()
{
	if (!_thread_stgstack) {
		_thread_stgstack = new _Tt_api_stg_stack;
	}

	return _thread_stgstack;
}

void _Tt_threadspecific::
free_procid()
{
	_thread_c_procid = (_Tt_c_procid *) 0;
}
#endif	// OPT_XTHREADS
