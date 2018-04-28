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
/*%%  $XConsortium: mp_procid.h /main/3 1995/10/23 10:27:28 rswiston $ 			 				 */
/* 
 * mp_procid.h
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file contains the implementation of _Tt_procid which is an object
 * that represents an active process communicating with a message server.
 * There are two instances of this object depending on whether the object
 * is in client or server mode. The client instance is basically just a
 * handle on it's id. The server instance maintains information such as
 * what patterns are registered for a process, what signalling channel is
 * used, and also what messages are waiting to be delivered to the client
 * instance of the procid. 
 */
#ifndef MP_PROCID_H
#define MP_PROCID_H

#include "util/tt_table.h"
#include "util/tt_host_utils.h"
#include "mp/mp_global.h"
#include "mp/mp_stream_socket_utils.h"
#include "mp/mp_session_utils.h"
#include "mp/mp_message_utils.h"
#include "mp/mp_pattern_utils.h"
#include "mp/mp_procid_utils.h"
#include "mp_xdr_functions.h"
#define  _TT_MESSAGE_TIMEOUT	300 /* default message timeout in seconds */


#ifdef OPT_ADDMSG_DIRECT
/* 
 * if this option is turned on then TT_ADDMSG_VERS should be set to
 * the version number at which addmsg_direct is used. The code assumes
 * that all versions later than this version also support this
 * optimization. 
 * 
 *  XXX: This should probably be changed to a mechanism that allows
 *  clients to tell their session that they would like this optimization
 *  to be used. The session can then deny their request meaning it doesn't
 *  support the optimization.
 */
#	define TT_ADDMSG_VERS	3
#else
/* 
 * defining this as -1 assures that no code will make use of it 
 */
#	define TT_ADDMSG_VERS	-1
#endif


enum _Tt_procid_flagbits {
	_TT_PROC_ACTIVE,	/* is an active proc */
	_TT_PROC_FD_CHANNEL_ON,	/* fd channel is valid */
	_TT_PROC_COMMITTED,	/* committed to default session */
	_TT_PROC_IS_LOCAL,	/* is local to server's machine */
	_TT_PROC_SIGNALLED,	/* has been signalled about new msgs */
	_TT_PROC_MSGSENT	/* msg sent directly to proc */
};

class _Tt_procid : public _Tt_object {
      public:
	_Tt_procid();
	virtual ~_Tt_procid();
	void			add_joined_file(_Tt_string fpath);
	void			del_joined_file(_Tt_string fpath);
	int			fd();
	const _Tt_string	&id() const;
	int			is_equal(const _Tt_procid_ptr &p) {
		return(_id == p->_id);
	}
	int			is_equal(_Tt_string &id);
	int			joined_to_file(_Tt_string fpath);
	int			processing(const _Tt_message &m);
	_Tt_pattern_list_ptr	&patterns() {
		return _patterns;
	}
	pid_t			pid() const {
		return _pid;
	}
	int			port() const;
	void			print(const _Tt_ostream &os) const;
	_Tt_host_ptr		&proc_host() {
		return _proc_host;
	}
	int			program() const {
		return _program;
	}
	void			set_start_token(const _Tt_string &token) {
		_start_token = token;
	}
	_Tt_string		&start_token() {
		return _start_token;
	}
	int			version() const {
		return _version;
	}
	_Tt_string		&proc_host_ipaddr() {
		return _proc_host_ipaddr;
	}
	_Tt_string_list_ptr	&ptypes() {
		return _declared_ptypes;
	}
	bool_t			xdr(XDR *xdrs);

      protected:
	int			_fd;
	int			_flags;
	int			_program;
	int			_version;
	pid_t			_pid;
	_Tt_string		_id;
	_Tt_string		_start_token;
	_Tt_string		_proc_host_ipaddr;
	_Tt_string_list_ptr	_joined_files;
	_Tt_string_list_ptr	_declared_ptypes;
	_Tt_pattern_list_ptr	_patterns;
	_Tt_host_ptr		_proc_host;
	_Tt_stream_socket_ptr	_socket;
	XDR			*_mxdr_stream;
};

_Tt_string _tt_procid_id(_Tt_object_ptr &o);
bool_t tt_xdr_procid(XDR *xdrs, _Tt_procid_ptr *patp);
bool_t tt_xdr_message_list(XDR *xdrs, _Tt_message_list_ptr *mptr);

#ifdef OPT_ADDMSG_DIRECT
int _tt_xdr_readit(char *iohandle, char *buf, int nbytes);
int _tt_xdr_writeit(char *iohandle, char *buf, int nbytes);
#endif				/*  OPT_ADDMSG_DIRECT */
#endif				/* MP_PROCID_H */

