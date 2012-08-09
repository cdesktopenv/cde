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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $TOG: mp_session.h /main/4 1999/08/30 10:59:15 mgreess $ 			 				 */
/* 
 * @(#)mp_session.h	1.36 95/01/25
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file contains the implementation of the _Tt_session object which
 * represents the minimal message-passing domain. The primary
 * responsibility of the _Tt_session is to advertise its presence in
 * whatever form is appropriate (ie. as an X atom for X11, in the root
 * directory of an NSE environment, etc.). When the _Tt_session object is
 * created as part of a client application it represents mostly a
 * connection to a _Tt_session object that is part of the message server.
 * This server instance is responsible for storing session-queued
 * messages, handling RPC requests to the server, and storing client
 * properties. When clients will be able to join multiple sessions the
 * server instance of _Tt_session will also be responsible for
 * forwarding messages to other sessions (ie. the clients still pass
 * messages through the same default session (to insure the proper
 * flowcontrol) but those messages are passed on to their respective
 * sessions. 
 */
#ifndef MP_SESSION_H
#define MP_SESSION_H
#include "tt_options.h"
#include "util/tt_host_utils.h"
#include "mp/mp_auth.h"
#include "mp/mp_global.h"
#include "mp/mp_mp.h"
#include "mp/mp_rpc.h"
#include "mp/mp_desktop_utils.h"
#include "mp/mp_rpc_client_utils.h"
#include "mp/mp_session_utils.h"
#include "mp/mp_session_prop_utils.h"
#include "mp/mp_message_utils.h"
#include "mp/mp_pattern_utils.h"
#include "mp/mp_procid_utils.h"

/* 
 * string used to advertise a tt session in both the X and
 * process-tree sessions. Note that in X ICCCM compliance
 * dictates that the atom name be prefixed by the vendor
 * string. Since there's no good reason for them to be different,
 * the same name is used for process-tree sessions.
 */
#define		TT_XATOM_NAME		"_SUN_TT_SESSION"
#define		TT_CDE_XATOM_NAME	"TT_SESSION"
#define		TT_START_TOKEN		"_SUN_TT_TOKEN"
#define		TT_CDE_START_TOKEN	"TT_TOKEN"
#define		TT_FILE_HINT		"_SUN_TT_FILE"
#define		TT_CDE_FILE_HINT	"TT_FILE"
#define		TT_START_SID		"_SUN_TT_SID"
#define		TT_CDE_START_SID	"_TT_SID"
enum _Tt_env {
	_TT_ENV_X11,
	_TT_ENV_PROCESS_TREE,
	_TT_ENV_LAST
};


class _Tt_session : public _Tt_object {
      public:
	_Tt_session();
	virtual ~_Tt_session();

	_Tt_string		address_string();
	_Tt_string		auth_cookie() {
		return _auth.auth_cookie();
	}
	_Tt_auth_level		auth_level() {
		return _auth.auth_level();
	}
	Tt_status		call(int rpc_proc,
				     xdrproc_t xdr_arg_fn, char *arg,
				     xdrproc_t xdr_res_fn, char *res,
				     int timeout = -1, int rebind = 0);
	_Tt_env			env();
	const _Tt_host_ptr	&host() {
		return _host;
	}
	const _Tt_string	&id() const {
		return _id;
	}

	Tt_status		ping();
	//void			print(FILE *fs = stdout) const;
	int			rpc_program() {
		return _rpc_program;
	}
	int			rpc_version() {
		return _rpc_version;
	}
	Tt_status		set_auth_level(_Tt_auth_level auth_level) {
		return _auth.set_auth_level(auth_level);
	}
	void			set_env(_Tt_env env, _Tt_string arg);
	_Tt_string		Xid(_Tt_string xdisp);
        bool_t			xdr(XDR *xdrs);
	int			desktop_event_callback();
	_Tt_string		displayname();
	Tt_status		set_id(char *sid = (char *)0);
	Tt_status		client_session_init();
	_Tt_string 		process_tree_id();

	// Determine if this session is in a list of sessions.
	int			has_id(const _Tt_string &id);
	int			has_id(const _Tt_string_list_ptr slist_p);

      protected:
	Tt_status		auto_start_init();
	Tt_status		client_session_init_byid(_Tt_string sid); 
	Tt_status		find_advertised_address(_Tt_string &addr);
	Tt_status		parsed_address(_Tt_string &session_addr);

	//
	// state variables
	//
	_Tt_string			_address_string;
	_Tt_auth			_auth;
	_Tt_string			_displayname;
	_Tt_env				_env;
	_Tt_host_ptr			_host;
	_Tt_string			_hostaddr;
	_Tt_string			_id;
	int				_is_server;
	int				_is_dead;
	pid_t				_pid;
	_Tt_session_prop_list_ptr 	_properties;
	_Tt_message_list_ptr		_queued_messages;
	_Tt_rpc_client_ptr		_rpc_client;
	int				_rpc_program;
	int				_rpc_version;
	pid_t				_server_num;
	uid_t				_server_uid;
	_Tt_string			_type;
	_Tt_desktop_ptr			_desktop;
#ifdef OPT_UNIX_SOCKET_RPC
	int				_u_sock;
	char				*local_socket_name();
	_Tt_string			_socket_name;
#endif 			/* OPT_UNIX_SOCKET_RPC */

	friend class _Tt_s_mp;
	friend class _Tt_s_session;
	friend int _Tt_mp::find_session_by_fd(int, _Tt_session_ptr &);
};

_Tt_string _tt_session_address(_Tt_object_ptr &o);
#endif				/*  MP_SESSION_H */
