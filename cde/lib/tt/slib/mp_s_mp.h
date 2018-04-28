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
/*%%  $XConsortium: mp_s_mp.h /main/4 1996/08/30 17:02:52 drk $ 			 				 */
/* 
 * @(#)mp_s_mp.h	1.20	94/11/17
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file implements the _Tt_s_mp object which represents the global
 * information for the server MP component. There should only be one
 * instance of a _Tt_s_mp object in the server.
 */
#ifndef _MP_S_MP_H
#define _MP_S_MP_H

#include "tt_options.h"
#include "mp/mp_mp.h"
#include "util/tt_table.h"
#include <rpc/rpc.h>
#include <signal.h>
#include "mp/mp_global.h"
#include "mp/mp_session_utils.h"
#include "mp_s_session_utils.h"
#include "mp/mp_file_utils.h"
#include "mp/mp_procid.h"
#include "mp/mp_procid_utils.h"
#include "mp_s_procid.h"
#include "mp_s_procid_utils.h"
#include "mp/mp_pattern_utils.h"
#include "mp_typedb_utils.h"
#include "mp_ptype_utils.h"
#include "mp_otype_utils.h"
#include "mp_rpc_implement.h"
#include "mp_signature_utils.h"
#include "util/tt_int_rec.h"

const int SIGTYPES = SIGUSR2;

class _Tt_s_mp : public _Tt_mp {
      public:
					_Tt_s_mp();
	virtual				~_Tt_s_mp();

	Tt_status			init();
	void				main_loop();
	_Tt_string			alloc_procid_key();
	int				find_proc(const _Tt_procid_ptr &procid,
						  _Tt_s_procid_ptr &p,
						  int create_ifnot);
	void				set_timeout(int timeout);
	void			install_ptable(_Tt_ptype_table_ptr &p);
	void			install_otable(_Tt_otype_table_ptr &o);
	void			remove_signatures(const _Tt_ptype &p);
	void			remove_signatures(const _Tt_otype &o);
	void			install_signatures(_Tt_signature_list_ptr &s);
	_Tt_ptype_table_ptr		ptable;
	_Tt_otype_table_ptr		otable;
	_Tt_sigs_by_op_table_ptr	sigs;
	_Tt_patlist_table_ptr		opful_pats;
	_Tt_pattern_list_ptr		opless_pats;
	unsigned int			now;
	unsigned int			when_last_observer_registered;
	_Tt_update_args			update_args;
	int				max_active_messages;
	_Tt_string			udb;
	_Tt_string			sdb;
	_Tt_string			ndb;
	int				map_ptypes;
	_Tt_pid_t_rec_list_ptr		failed_proc_starts;
	int				exit_main_loop;
	int				unix_cred_chk_flag;
	int				xfd;
#ifdef OPT_UNIX_SOCKET_RPC
	int				unix_fd;
#endif
	int				fin;
	int				fout;
	int				in_file_scope(const _Tt_string &f);
	void				mod_file_scope(const _Tt_string &f,
						       int add);
	_Tt_s_procid_table_ptr		active_procs;
	_Tt_s_session_ptr               initial_s_session;
	Tt_status			add_procid(_Tt_s_procid_ptr &proc);
	Tt_status			s_remove_procid(_Tt_s_procid &proc);
	Tt_status			s_init();
	Tt_status			init_self();
	_Tt_typedb_ptr			tdb;

	pid_t				garbage_collector_pid;

      private:
	Tt_status			_handle_Session_Trace();
	Tt_status			_observe_Saved();

	//
	// These are used to clean out dead ttsession's from dbserver.
	// The SIGCHLD from its PID (garbage_collector_pid - above) can be
	// ignored. Garbage collection occurs at startup and once per
	// day and sets _next_garbage_run to be current time + 24 hours. 
	//
	void				_collect_garbage();
	long				_next_garbage_run;

	int				_mp_start_time;
	int				_next_procid_key;
	_Tt_int_rec_list_ptr		_active_fds;
	_Tt_string_list_ptr		_active_fds_procids;
	_Tt_int_rec_list_ptr		_file_scope_refcounts;
	_Tt_string_list_ptr		_file_scope_paths;
	_Tt_s_procid_ptr		_last_proc_hit;
	_Tt_s_procid_ptr		_self;
	int				_min_timeout;
	friend class _Tt_s_procid;	

};
extern _Tt_s_mp		*_tt_s_mp;

#endif				/* _MP_S_MP_H */
