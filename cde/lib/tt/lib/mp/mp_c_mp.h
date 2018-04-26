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
/*%%  $XConsortium: mp_c_mp.h /main/5 1995/11/28 19:27:40 cde-sun $ 			 				 */

/* 
 * mp_c_mp.h
 * 
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 * 
 * This file declares the _Tt_c_mp object which represents the client-only
 * global information for the MP component. There should only be one instance
 * of a _Tt_mp object in either server or client mode.
 */
#ifndef _MP_C_MP_H
#define _MP_C_MP_H

#include "mp/mp.h"
#include "mp/mp_c_session_utils.h"
#include "mp/mp_c_procid_utils.h"

class _Tt_c_mp : public _Tt_mp {
      public:
	_Tt_c_mp();
	virtual ~_Tt_c_mp();
	Tt_status			c_init();
	Tt_status			set_default_procid(_Tt_string id,
							   int thread_only = 0);
	_Tt_procid_ptr			&default_procid();
	_Tt_c_procid_ptr		&default_c_procid();
	Tt_status			create_new_procid();
	Tt_status			c_remove_procid(_Tt_procid_ptr &proc);
	_Tt_c_session_ptr               initial_c_session;
	int				procid_count();
#ifdef OPT_XTHREADS
	void				set_multithreaded();
#endif

	_Tt_c_session_ptr               default_c_session;

      protected:
	_Tt_c_procid_ptr		_default_c_procid;
};
	
#endif				/* _MP_C_MP_H */
