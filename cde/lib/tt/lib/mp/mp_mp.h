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
/*%%  $TOG: mp_mp.h /main/4 1999/09/17 18:29:12 mgreess $ 			 				 */
/* 
 * mp_mp.h
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file implements the _Tt_mp object which represents the global
 * information for the MP component. There should only be one instance of
 * a _Tt_mp object in either server or client mode. 
 */
#ifndef _MP_MP_H
#define _MP_MP_H

#include "tt_options.h"
#include "mp/mp_global.h"
#include "util/tt_table.h"
#include "mp/mp_file_utils.h"
#include "mp/mp_procid_utils.h"
#include "mp/mp_session_utils.h"

enum _Tt_mp_flags {
	_TT_MP_IN_SERVER,
	_TT_MP_INIT_DONE
};

class _Tt_mp : public _Tt_object {
      public:
	_Tt_mp();
	virtual ~_Tt_mp();
	int				in_server() {
		return(_flags&(1<<_TT_MP_IN_SERVER));
	};
	Tt_status			find_file(_Tt_string path,
						  _Tt_file_ptr &f,
						  int create_ifnot);
	void				remove_file(_Tt_string path);
	void				remove_session(_Tt_string id);
	void			save_session_fd(int fd);
	int			find_session_by_fd(int fd, _Tt_session_ptr &sp);
	void			check_if_sessions_alive();
	int				generate_pattern_id();	int				generate_message_id();
	_Tt_session_ptr			initial_session;

	int				active_messages;
	Tt_status			find_session(_Tt_string id,
						     _Tt_session_ptr &sp,
						     int create_ifnot,
						     int auto_start = 0);
	_Tt_procid_table_ptr		active_procs;


      protected:
	int				_current_message_id;
	int				_current_pattern_id;
	_Tt_file_table_ptr		_file_cache;
	_Tt_session_table_ptr		_session_cache;	
	int				_flags;
	fd_set				_session_fds;
};	
	
#endif				/* _MP_MP_H */
