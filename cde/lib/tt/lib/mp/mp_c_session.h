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
/*%%  $XConsortium: mp_c_session.h /main/3 1995/10/23 10:22:09 rswiston $ 			 				 */
/* 
 * mp_c_session.h
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file contains the implementation of the client side of
 * the _Tt_session object.
 */
#ifndef MP_C_SESSION_H
#define MP_C_SESSION_H
#include <mp/mp_session.h>
#include <mp/mp_c_session_utils.h>

class _Tt_c_session : public _Tt_session {
      public:
	_Tt_c_session();
	virtual ~_Tt_c_session();

	Tt_status		c_addprop(_Tt_string prop, _Tt_string val);
	Tt_status		c_getprop(_Tt_string prop, int i,
					  _Tt_string &value);
	Tt_status		c_init();
	Tt_status		c_join(_Tt_procid_ptr &procid);
	Tt_status		c_propcount(_Tt_string prop, int &cnt);
	Tt_status		c_propname(int i, _Tt_string &prop);
	Tt_status		c_propnames_count(int &cnt);
	Tt_status		c_quit(_Tt_procid_ptr &procid);
	Tt_status		c_setprop(_Tt_string prop, _Tt_string val);
      private:
	int			startup_ttsession(_Tt_string &command);
};

#endif				/*  MP_C_SESSION_H */
