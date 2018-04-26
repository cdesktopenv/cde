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
/*%%  $XConsortium: mp_session_prop.h /main/3 1995/10/23 10:29:09 rswiston $ 			 				 */
/*
 *
 * mp_session_prop.h
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */

#if !defined(_MP_SESSION_PROP_H)
#define _MP_SESSION_PROP_H

#include "util/tt_string.h"

class _Tt_session_prop : public _Tt_object {
      public:
	_Tt_session_prop();
	_Tt_session_prop(_Tt_string name, _Tt_string_list_ptr values);
	_Tt_session_prop(_Tt_string name, _Tt_string value);
	virtual ~_Tt_session_prop();
	_Tt_string		_name;
	_Tt_string_list_ptr	_values;
};

#endif
