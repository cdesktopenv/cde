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
//%%  $XConsortium: mp_s_session_prop.C /main/3 1995/10/23 12:00:00 rswiston $ 			 				
/*
 *
 * mp_s_session_prop.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "mp/mp_global.h"
#include "mp_s_mp.h"
#include "mp/mp_mp.h"
#include "mp_rpc_server.h"
#include "mp_rpc_implement.h"
#include "mp_s_session.h"
#include "mp/mp_session_prop.h"     
#include "mp/mp_xdr_functions.h"

Tt_status _Tt_s_session::
s_setprop(_Tt_string prop, _Tt_string val)
{
	_Tt_session_prop_list_cursor propc(_properties);
	_Tt_string_list_cursor valp;
	while (propc.next()) {
		if (propc->_name == prop) {
			// found the property
			valp.reset(propc->_values);
			// remove all of the existing values
			while (valp.next()) {
				valp.remove();
			}
			// append the new value
			propc->_values->append(val);
			break;
		}
	}
	if (!propc.is_valid()) {
		// we didn't find the property, so add it
		_properties->append(_Tt_session_prop_ptr
				    (new _Tt_session_prop(prop, val)));
	}
	return TT_OK;
}

Tt_status _Tt_s_session::
s_addprop(_Tt_string prop, _Tt_string val)
{
	_Tt_session_prop_list_cursor propc(_properties);
	while (propc.next()) {
		if (propc->_name == prop) {
			// found the property
			// append the new value
			propc->_values->append(val);
			break;
		}
	}
	if (!propc.is_valid()) {
		// we didn't find the property, so add it
		_properties->append(_Tt_session_prop_ptr
				    (new _Tt_session_prop(prop, val)));
	}
	return TT_OK;
}


Tt_status _Tt_s_session::
s_getprop(_Tt_string prop, int i, _Tt_string &value)
{
	_Tt_session_prop_list_cursor propc(_properties);

	while (propc.next()) {
		if (propc->_name == prop) {
			// found the property
			if (i >= propc->_values->count()) {
				return TT_ERR_NUM;
			} else {
				value = (*propc->_values)[i];
			}
			break;
		}
	}
	if (!propc.is_valid()) {
		// we didn't find the property
		return TT_ERR_PROPNAME;
	} else {
		return TT_OK;
	}
}


Tt_status _Tt_s_session::
s_propcount(_Tt_string prop, int &cnt)
{
	_Tt_session_prop_list_cursor propc(_properties);
	while (propc.next()) {
		if (propc->_name == prop) {
			// found the property
			cnt = propc->_values->count();
			return TT_OK;
		}
	}
	// we didn't find the property
	return TT_ERR_PROPNAME;
}


Tt_status _Tt_s_session::
s_propname(int i, _Tt_string &prop)
{
	if (i >= _properties->count()) {
		return TT_ERR_NUM;
	}
	prop = (*_properties)[i]->_name;
	return TT_OK;
}


Tt_status _Tt_s_session::
s_propnames_count(int &cnt)
{
	cnt = _properties->count();
	return TT_OK;
}
