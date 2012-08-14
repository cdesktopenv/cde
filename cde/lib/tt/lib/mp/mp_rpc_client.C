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
//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $TOG: mp_rpc_client.C /main/9 1999/09/08 18:21:02 mgreess $ 			 				
/*
 *
 * mp_rpc_client.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include <sys/time.h>		// ultrix
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include "tt_options.h"
#include "mp/mp_auth.h"
#include "mp/mp_rpc_client.h"
#include "mp/mp_xdr_functions.h"
#include "util/tt_host.h"
#include "util/tt_port.h"

#if defined(ultrix)
extern "C"
{ extern struct XSizeHints;
  extern struct XStandardColormap;
  extern struct XTextProperty;
  extern struct XWMHints;
  extern struct XClassHint;
};
#endif

#if defined(_AIX)
/* AIX's FD_ZERO macro uses bzero() without declaring it. */
#include <strings.h>
/* And arpa/inet.h has a buggy declaration of inet_addr */
extern "C" in_addr_t inet_addr(const char *);
#endif

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <memory.h>

#if defined(sgi)
/* SGI's FD_ZERO macro uses bzero() without declaring it. */
#include <CC/libc.h>
#endif	

#include <sys/resource.h>
#include <util/tt_global_env.h>
#include <mp/mp_rpc_fns.h>

/* 
 * Constructs an rpc client. 
 */
_Tt_rpc_client::
_Tt_rpc_client(int conn_socket)
{
	_socket = conn_socket;
	_client = (CLIENT *)0;
}


/* 
 * Destroys an rpc client (breaks off connections)
 */
_Tt_rpc_client::
~_Tt_rpc_client()
{
	if (_client != (CLIENT *)0) {
		clnt_destroy(_client);
	}
	_host = (_Tt_host *)0;
}


/* 
 * Returns the socket associated with an rpc client. --> it would be
 * nice to use this socket rather than open up a new one when signalling
 * procids.   
 */
int _Tt_rpc_client::
socket()
{
	return(_socket);
}

/* 
 * create client connection to host,program,version
 */
int _Tt_rpc_client::
init(_Tt_host_ptr &host, int program, int version,
     uid_t servuid, _Tt_auth &auth)
{
	int		optval;

	optval = (_socket == RPC_ANYSOCK);
	_auth = auth;
	_host = host;
	_program = program;
	_version = version;
	_server_uid = servuid;
	if (_client != (CLIENT *)0) {
		if (_auth.auth_level() == _TT_AUTH_UNIX) {
			auth_destroy(_client->cl_auth);
		}
		clnt_destroy(_client);
	}
#if defined(OPT_SECURE_RPC)
	if (_auth.auth_level() == _TT_AUTH_DES) {
		if (_server_uid == 0) {
			host2netname(_servername, (char *)_host->name(), 0);
		} else {
			user2netname(_servername, _server_uid, 0);
		}
	}
#endif /* OPT_SECURE_RPC */	
#ifndef	OPT_TLI

	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons((optval) ? 0 : 4000);
	_server_addr.sin_addr.s_addr = inet_addr((char *)(_host->stringaddr()));
	_client = clnttcp_create(&_server_addr, _program,
			         _version, &_socket, 4000, 4000);
	if (_client == 0) {
		// XXX only when in some kind of debug mode
		//clnt_pcreateerror("_Tt_rpc_client::init(): clnttcp_create()");
		return 0;
	}
	if (_auth.auth_level() == _TT_AUTH_UNIX) {
		_client->cl_auth = authunix_create_default();
	}
		
	if (optval) {
#ifndef linux
		if (setsockopt(_socket, SOL_SOCKET, SO_USELOOPBACK,
			       (char *)&optval, sizeof(int)) == -1) {
			_tt_syslog( 0, LOG_ERR, "_Tt_rpc_client::init(): "
				    "setsockopt(SO_USELOOPBACK): %m");
		}
#endif
		if (setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY,
			       (char *)&optval, sizeof(int)) == -1) {
			_tt_syslog( 0, LOG_ERR, "_Tt_rpc_client::init(): "
				    "setsockopt(TCP_NODELAY): %m");
		}
	}
#else

	_client = clnt_create((char *)_host->name(),
			      _program, _version, "circuit_v");
	if (_client == 0) {
		// XXX only when in some kind of debug mode
		//clnt_pcreateerror( "_Tt_rpc_client::init(): clnt_create()" );
		return 0;
	}

	if (!clnt_control(_client, CLGET_FD, (char *)&_socket)) {
		// I cannot imagine how this could ever fail at this point.
		return 0;
	}
	// We used to call _tt_tli_set_nodelay here on the RPC connection.
	// This stopped working mysteriously, suddenly the endpoint started
	// coming back in state T_DATAXFER instead of T_IDLE from clnt_create.
	// Fortunately, in the meantime the TIRPC library has started
	// setting NODELAY on RPC/TCP connections, so we don't need to
	// do it here anymore anyway.

	if (_auth.auth_level() == _TT_AUTH_UNIX) {
		_client->cl_auth = authunix_create_default();
	}

#endif				// !OPT_TLI

	// Set close-on-exec bit so a libtt client which forks and execs won't
	// be short some fd's in the child.

#if defined(linux)
        // JET - for linux, we need to do this properly - I don't know
        // how the original code below can be correct, so we'll do it
        // differently.
        {
          long flags;

          if ((flags = fcntl(_socket, F_GETFD)) == -1)
            {
              _tt_syslog( 0, LOG_ERR, "_Tt_rpc_client::init(): "
                          "fcntl(F_GETFD): %m");
            }
          else
            {
              if (fcntl(_socket, F_SETFD, flags | FD_CLOEXEC) == -1)
		_tt_syslog( 0, LOG_ERR, "_Tt_rpc_client::init(): "
			    "fcntl(F_SETFD): %m");
            }
        }
#else        

	if (-1==fcntl(_socket, F_SETFD, 1)) {
		_tt_syslog( 0, LOG_ERR, "_Tt_rpc_client::init(): "
			    "fcntl(F_SETFD): %m");
	}		
#endif // linux

	return(1);
}


/* 
 * invoke rpc procedure
 */
clnt_stat _Tt_rpc_client::
call(int procnum, xdrproc_t inproc, char *in,
     xdrproc_t outproc, char *out, int timeout)
{
	fd_set		bogus;
	timeval		tmout;
	timeval		total_timeout;
	struct sigaction curr_action;
	int		need2reset_sigpipe = 0;
	_Tt_auth_iceauth_args  args;

	if (_client == (CLIENT *)0) {
		return(RPC_CANTSEND);
	}
	total_timeout.tv_sec = ((timeout < 0) ? 0 : timeout);
	total_timeout.tv_usec = 0;
	clnt_control(_client, CLSET_TIMEOUT, (char *) &total_timeout);

	switch (_auth.auth_level()) {
	      case _TT_AUTH_UNIX:
		break;
	      case _TT_AUTH_ICEAUTH:
		break;
#if defined(OPT_SECURE_RPC)
	      case _TT_AUTH_DES:
#ifdef OPT_TLI
		_client->cl_auth =
		authdes_seccreate(_servername, 60, (char *)_host->name(),
				  (des_block *)0);
		if (_client->cl_auth == 0) {
			_tt_syslog( 0, LOG_WARNING, "authdes_seccreate(): 0" );
			// XXX what todo when authdes_seccreate() fails?
			return RPC_AUTHERROR;
		}
#else 
		_client->cl_auth =
		authdes_create(_servername, 60, &_server_addr,
			       (des_block *)0);
		if (_client->cl_auth == 0) {
			_tt_syslog( 0, LOG_WARNING, "authdes_seccreate(): 0" );
			// XXX what todo when authdes_seccreate() fails?
			return RPC_AUTHERROR;
		}
#endif /* OPT_TLI */
		break;
#endif /* OPT_SECURE_RPC */
              case _TT_AUTH_NONE:
		break;
	      default:
		return(RPC_AUTHERROR);
	}
	
	if (timeout == 0) {
		FD_ZERO(&bogus);
		FD_SET(_socket, &bogus);
		tmout.tv_sec = 0;
		tmout.tv_usec = 0;
		select(FD_SETSIZE, &bogus, 0, 0, &tmout);

		if (FD_ISSET(_socket, &bogus)) {
			return(RPC_CANTSEND);
		}
	}

#if defined(OPT_BUG_USL) || defined(OPT_BUG_UXP)
	if (timeout <= 0)
	{
		outproc = (xdrproc_t) NULL;
	}
#endif

	//
	// tcp write errors (when the rpc_server on the other end dies)
	// cause a SIGPIPE.  We need to make sure the SIGPIPE is caught,
	// or the process dies.
	//
	if (sigaction(SIGPIPE, 0, &curr_action) != 0) {
		_tt_syslog( 0, LOG_ERR, "sigaction(): %m" );
	}
#if defined(OPT_BUG_SUNOS_5) || defined(OPT_BUG_UW_1)
	if ((SIG_TYP)curr_action.sa_handler == SIG_DFL)
#else
#if defined(OPT_BUG_UW_2) || defined(OPT_BUG_UXP)
	if ((void(*)(int))curr_action.sa_handler == SIG_DFL)
#else
	if (curr_action.sa_handler == SIG_DFL)
#endif
#endif
	{
		need2reset_sigpipe = 1;
		signal(SIGPIPE, SIG_IGN);
	}

	if (_TT_AUTH_ICEAUTH == _auth.auth_level()) {
	    args.auth_level = _auth.auth_level();
	    args.auth_cookie = _auth.auth_cookie();
	    args.inproc = (xdr_auth_proc_t) inproc;
	    args.inargs = (caddr_t) in;

	    _clnt_stat = clnt_call(_client, procnum,
			           (xdrproc_t) tt_xdr_auth_iceauth_args,
			       	   (char*) &args,
			           outproc, out,
			           total_timeout);
	}
	else {
	    _clnt_stat = clnt_call(_client, procnum,
			           inproc, in,
			           outproc, out,
			           total_timeout);
	}
#if defined(OPT_BUG_USL) || defined(OPT_BUG_UXP)
	if (timeout <= 0)
	{
		total_timeout.tv_sec = 10;
		total_timeout.tv_usec = 0;
		clnt_control(_client, CLSET_TIMEOUT, (char *) &total_timeout);
		clnt_call(_client, NULLPROC, (xdrproc_t) xdr_void, (char *)NULL,
		(xdrproc_t) xdr_void, (char *) NULL, total_timeout);
	}
#endif
	if (need2reset_sigpipe) {
		signal(SIGPIPE, SIG_DFL);
	}
#if !defined(OPT_BUG_RPCINTR)
	if (_clnt_stat == RPC_INTR) {
		return(RPC_CANTSEND);
	}
#endif	
	if (_auth.auth_level() == _TT_AUTH_DES) {
		auth_destroy(_client->cl_auth);
	}

	return(_clnt_stat);
}


