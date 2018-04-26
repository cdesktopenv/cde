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
/*%%  $XConsortium: mp_stream_socket.h /main/3 1995/10/23 10:29:57 rswiston $ 			 				 */
/* 
 * mp_stream_socket.h
 * 
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 * 
 * This file implements the _Tt_stream_socket which is an object used to
 * deal with TCP sockets. The methods for this object handle creation,
 * destruction and communicating over TCP sockets. Note that the
 * implementation of stream sockets is tailored specifically to the use
 * of sockets by the MP as a signalling mechanism. The methods here are
 * not intended to be a general C++ wrapper for sockets.
 */
#ifndef MP_STREAM_SOCKET_H
#define MP_STREAM_SOCKET_H
#include <sys/types.h>
#include <tt_options.h>

#ifdef OPT_TLI
#	include <tiuser.h>
#endif
#include <netinet/in.h>
#include <util/tt_object.h>
#include <util/tt_ptr.h>
#include <util/tt_host_utils.h>

class _Tt_stream_socket : public _Tt_object {
      public:
	_Tt_stream_socket();
	_Tt_stream_socket(_Tt_host_ptr &host, int port);
	virtual ~_Tt_stream_socket();
	int		init(int init_as_source);
	int		send(char *msg, int len);
	int		recv(char *msg, int len);
	int		port();
	int		fd();
	int		sock();
	int		read_would_block();
      private:
	int		accept();
	int		_is_source;
	int		_msgsock;
	int		_sock;
	_Tt_host_ptr	_host;
	sockaddr_in	_hostaddr;
#ifdef OPT_TLI
	int		_port;
	t_call		*_srequest;
#endif				/* OPT_TLI */
};


#endif				/* MP_STREAM_SOCKET_H */
