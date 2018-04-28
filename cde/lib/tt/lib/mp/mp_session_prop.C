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
//%%  $XConsortium: mp_session_prop.C /main/3 1995/10/23 10:29:02 rswiston $ 			 				
/*
 *
 * mp_session_prop.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "mp/mp_session_prop.h"
#include "util/tt_string.h"


_Tt_session_prop::
_Tt_session_prop()
{
}

_Tt_session_prop::
_Tt_session_prop(_Tt_string name, _Tt_string_list_ptr values)
{
	_name = name;
	_values = values;
}

_Tt_session_prop::
_Tt_session_prop(_Tt_string name, _Tt_string value)
{
	_name = name;
	_values = new _Tt_string_list();
	_values->append(value);
}

_Tt_session_prop::
~_Tt_session_prop()
{
}
