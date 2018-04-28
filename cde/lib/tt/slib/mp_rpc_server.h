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
/*%%  $TOG: mp_rpc_server.h /main/4 1999/08/30 11:03:46 mgreess $ 			 				 */
/*
 *
 * @(#)mp_rpc_server.h	1.8 93/09/07
 *
 * Copyright (c) 1990,1993 by Sun Microsystems, Inc.
 *
 * Implementation of _Tt_rpc_server which represents a provider of an
 * RPC service and _Tt_rpc_client which represents the corresponding
 * RPC client.
 */
#ifndef _TT_MP_RPC_SERVER_H
#define _TT_MP_RPC_SERVER_H

#include "mp/mp_auth.h"
#include "mp/mp_rpc.h"

class _Tt_rpc_server : public _Tt_object {
      public:
	_Tt_rpc_server() { _version = 0; _socket = 0; _program = 0; _rpc_fd = 0; _transp = NULL; };
	_Tt_rpc_server(int program, int version, int Rsocket, _Tt_auth &auth);
	virtual ~_Tt_rpc_server();
	int			init(void (*service_fn)(svc_req *, SVCXPRT *));
	_Tt_rpcsrv_err		run_until(int *stop, int sec_timeout,
				    _Tt_int_rec_list_ptr &efds);
	int			program() { return _program; };
	int			version() { return _version; };
      private:
	_Tt_auth		_auth;
	int			_program;
	int			_version;
	int			_socket;
	int			_rpc_fd;
	SVCXPRT			*_transp;
};

#endif				/* _TT_MP_RPC_SERVER_H */
