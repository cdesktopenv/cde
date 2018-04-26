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
/*%%  $TOG: mp_rpc_client.h /main/4 1999/08/30 10:57:12 mgreess $ 			 				 */
/*
 *
 * mp_rpc_server.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * Declaration of
 * _Tt_rpc_client which represents the ttsession
 * RPC client.
 */
#ifndef _TT_MP_RPC_CLIENT
#define _TT_MP_RPC_CLIENT
#include "mp/mp_auth.h"
#include "mp/mp_rpc.h"

class _Tt_rpc_client : public _Tt_object {
      public:
	_Tt_rpc_client(int conn_socket = RPC_ANYSOCK);
	virtual ~_Tt_rpc_client();
	// create client connection to host,program,version
	int			init(_Tt_host_ptr &host,
				     int program, int version,
				     uid_t servuid,
				     _Tt_auth &auth);
	int			socket();
	// invoke rpc procedure
	clnt_stat		call(int procnum,
				     xdrproc_t inproc,
				     char *in,
				     xdrproc_t outproc,
				     char *out,
				     int timeout);
	CLIENT			*rpc_handle() { return _client; };
      private:
	_Tt_host_ptr		_host;
	int			_program;
	int			_version;
	int			_socket;
	uid_t			_server_uid;
	_Tt_auth		_auth;
	char			_servername[MAXNETNAMELEN];
	CLIENT			*_client;
	clnt_stat		_clnt_stat;
#if defined(OPT_TLI)
	int			_server_addr;
#else
	sockaddr_in		_server_addr;
#endif /* OPT_TLI */
};

#endif				/* _TT_MP_RPC_CLIENT */
