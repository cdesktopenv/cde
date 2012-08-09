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
//%%  $TOG: mp_stream_socket.C /main/10 1998/03/19 18:58:53 mgreess $ 			 				
/*
 *
 * mp_socket.cc
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "tt_options.h"
#include <stdio.h>
#include "mp/mp_stream_socket.h"
#if defined(linux)
#include <sys/poll.h>
#else
#include <poll.h>
#endif
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>
#include "util/tt_global_env.h"
#include "util/tt_host.h"
#include "util/tt_port.h"

#if defined(OPT_TLI)
#	include <mp/mp_rpc_fns.h>
#	include <tiuser.h>
#  if defined(OPT_BUG_USL)
   extern int t_errno;
#  endif
#else
#	include <netinet/tcp.h>
#endif

#include <sys/time.h>

#if defined(ultrix)
extern "C" unsigned long inet_addr(char *);
#endif

#include <arpa/inet.h>

#if defined(OPT_BUG_USL)
extern char *t_errlist[];

char *t_strerror(int t_errno)
{
	return(t_errlist[t_errno]);
}
#endif

/* 
 * Constructs a socket object. Using (char *)0 for host means use the
 * current host. Specifying a portnum of 0 indicates that the first
 * available port number should be chosen.
 */
_Tt_stream_socket::
_Tt_stream_socket()
{
}

_Tt_stream_socket::
_Tt_stream_socket(_Tt_host_ptr &host, int portnum)
{
	_msgsock = -1;
	_host = host;
	memset(&_hostaddr, 0, sizeof(_hostaddr));
        _hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	_hostaddr.sin_port = htons(portnum);
	_hostaddr.sin_family = AF_INET;
}


/* 
 * Closes the connection on the socket.
 */
_Tt_stream_socket::
~_Tt_stream_socket()
{
#ifndef OPT_TLI
	close(_sock);
	close(_msgsock);
#else
	t_close(_sock);
	if (_msgsock != _sock) {
		t_close(_msgsock);
	}
#endif
}

int _Tt_stream_socket::
sock()
{
	return(_sock);
}

/* 
 * Returns the file descriptor associated with a socket.
 */
int _Tt_stream_socket::
fd()
{
	if (_is_source) {
		if ((_msgsock != -1) || (accept() != -1)) {
			return(_msgsock);
		} else {
			return(-1);
		}
	} else {
		return(_sock);
	}
}


/* 
 * Returns the external port number attached to a socket.
 */
int _Tt_stream_socket::
port()
{
#if defined(OPT_TLI)
	return(_port);
#else
        return((int)ntohs(_hostaddr.sin_port));
#endif
}


/* 
 * Initializes a stream socket. This method must be called before using a
 * socket to receive messages. The method assumes that if hostname is
 * (char *)0 then this is the "from" end of a socket. Otherwise, it is
 * assumed that this is an object which connects to a socket (presumed
 * already open) on the host named in _host.
 */
int _Tt_stream_socket::
init(int init_as_source)
{
	_Tt_host	host;

#if defined(OPT_TLI)
	_sock = t_open("/dev/tcp", O_RDWR, 0);
	if (_sock < 0) {
		_tt_syslog( 0, LOG_ERR,
			    "_Tt_stream_socket::init(): t_open(): %s",
			    t_strerror( t_errno ) );
		return 0;
	}
#else
#if defined(linux) || defined(CSRG_BASED)
	socklen_t	len;
#else
	int		len;
#endif
	int		optval;
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock < 0) {
		_tt_syslog( 0, LOG_ERR,
			    "_Tt_stream_socket::init(): socket(): %m" );
		return 0;
	}
#endif
	if (-1==fcntl(_sock, F_SETFD, 1)) {
		_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
			    "fcntl(F_SETFD): %m");
	}		

	_is_source = init_as_source;
	if (init_as_source) { /* 'from' end of socket */
#if !defined(OPT_TLI)
		optval = 1;
#ifndef linux
		if (setsockopt(_sock, SOL_SOCKET, SO_USELOOPBACK,
			       (char *)&optval, sizeof(int)) == -1) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "setsockopt(SO_USELOOPBACK): %m" );
			close(_sock);
			return(0);
		}
#endif
		if (setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY,
			       (char *)&optval, sizeof(int)) == -1) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "setsockopt(TCP_NODELAY): %m" );
		}

		if (bind(_sock, (struct sockaddr *)&_hostaddr,
			 sizeof(_hostaddr)) < 0) {
			close(_sock);
			return(0);
		}
		len = sizeof(sockaddr_in);
#if defined(_AIX) && (OSMAJORVERSION==4) && (OSMINORVERSION==2)
		if (getsockname(_sock, (sockaddr *)&_hostaddr, (size_t *)&len)
							< 0) {
#else
		if (getsockname(_sock, (sockaddr *)&_hostaddr, &len) < 0) {
#endif
			return(0);
		}
		return(listen(_sock,5) == 0);
#else
		struct t_bind	*bind;

		if ((bind = (struct t_bind *)t_alloc(_sock, T_BIND, T_ADDR)) ==
		    (struct t_bind *)0) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "t_alloc(T_BIND): %s",
				    t_strerror( t_errno ) );
			return(0);
		}
		// We use random port selection.
		// This means that we always let t_bind choose a
		// suitable port number for us and we ignore the
		// portnum argument given in the constructor for this
		// class.  If we were ever to claim _Tt_stream_socket
		// as a general C++ wrapper class for socket/TLI, this
		// would have to change.
		bind->addr.len = 0;
		bind->qlen = 8;
		if (t_bind(_sock, bind, bind) < 0) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "t_bind(): %s", t_strerror( t_errno ) );
			t_free((char *)bind, T_BIND);
			return(0);
		}
		_port = ntohs(((sockaddr_in *)(bind->addr.buf))->sin_port);
		t_free((char *)bind, T_BIND);
		_srequest = (struct t_call *)t_alloc(_sock, T_CALL, T_ADDR);
		if (_srequest == (t_call *)0) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "t_alloc(T_CALL): %s",
				    t_strerror( t_errno ) );
			return(0);
		}
#endif // !OPT_TLI
	} else { // 'to' end of socket
		//
		// If both Client and Server or (To and From) sockets are 
		// on the same host, it is better to use the localhost ip 
		// address. This removes ToolTalks dependency on the state 
		// of the network and  permits standalone operation.
		//
		// In this instance 'local_host' is the host where ttsession 
		// is running. '_host' is the host where the 'procid' 
		// proccess is running.  If both are on the same host, copy 
		// in the the localhost ip address rather than the actual 
		// "configured" ip address.
		// 
		_Tt_host_ptr	local_host;
		if ( (_tt_global->get_local_host(local_host) ) && 
			( _host->stringaddr() == local_host->stringaddr() ) ) {
			//
			// _host_ == local_host => (both on same host)
			//
			_hostaddr.sin_addr.s_addr = inet_addr((char *)"127.0.0.1");    	    
		} else {
			//
			// _host_ != local_host => (on different hosts)
			//
			memcpy((char *)&_hostaddr.sin_addr,
			(char *)_host->addr(),
			_host->addr_length());
		}	   
#if !defined(OPT_TLI)
		// set up socket options to insure that a close will
		// immediately send the message to a socket. This is
		// essential for the use of sockets as signalling 
		// mechanisms.

		if (setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY,
			       (char *)&optval, sizeof(int)) == -1) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "setsockopt(TCP_NODELAY): %m" );
		}


#ifndef linux
		if (setsockopt(_sock, SOL_SOCKET, SO_USELOOPBACK,
			       (char *)&optval, sizeof(int)) == -1) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "setsockopt(SO_USELOOPBACK): %m" );
			close(_sock);
			return(0);
		}
#endif
		if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR,
			       (char *)&optval, sizeof(int)) == -1) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "setsockopt(SO_REUSEADDR): %m" );
			close(_sock);
			return(0);
		}

#if defined(sun)
		// XXX: It's not at all clear that we need to do this
		// anywhere.. default seems to be don't linger anyway.
		if (setsockopt(_sock, SOL_SOCKET, ~SO_LINGER,
			       (char *)&optval, sizeof(int)) == -1) {
			_tt_syslog( 0, LOG_WARNING, "_Tt_stream_socket::init(): "
				    "setsockopt(~SO_LINGER): %m" );
		}
		
#endif // sun
		if (connect(_sock,
			    (struct sockaddr *)&_hostaddr,
			    sizeof(_hostaddr)) < 0) {
			close(_sock);
			return(0);
		}
#else
		t_call		*sndcall, *rcvcall;

		if (t_bind(_sock, 0, 0) < 0) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "t_bind(,0,0): %s", t_strerror( t_errno ) );
			return(0);
		}
		(void)_tt_tli_set_nodelay(_sock);
		sndcall = (t_call *)t_alloc(_sock, T_CALL, 0);
		if (sndcall == 0) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "t_alloc(T_CALL,0): %s",
				    t_strerror( t_errno ) );
			return(0);
		}
		sndcall->addr.maxlen = sizeof(_hostaddr);
		sndcall->addr.len = sizeof(_hostaddr);
		sndcall->addr.buf = (char *)&_hostaddr;
		sndcall->opt.len = 0;
		sndcall->udata.len = 0;
		rcvcall = (t_call *)t_alloc(_sock, T_CALL, T_OPT|T_ADDR);
		if (rcvcall == 0) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::init(): "
				    "t_alloc(T_CALL, T_OPT|T_ADDR): %s",
				    t_strerror( t_errno ) );
			if (t_free((char *)sndcall, T_CALL) < 0) {
				_tt_syslog( 0, LOG_ERR, "t_free(): %s",
					    t_strerror( t_errno ) );
			}
			return(0);
		}
		rcvcall->udata.maxlen = 0;
		if (t_connect(_sock, sndcall, rcvcall) < 0) {
			_tt_syslog( 0, LOG_ERR, "t_connect(): %s",
				    t_strerror( t_errno ) );
			sndcall->addr.buf = 0;
			if (t_free((char *)sndcall, T_CALL) < 0) {
				_tt_syslog( 0, LOG_ERR, "t_free(sndcall): %s",
					    t_strerror( t_errno ) );
			}
			if (t_free((char *)rcvcall, T_CALL) < 0) {
				_tt_syslog( 0, LOG_ERR, "t_free(rcvcall): %s",
					    t_strerror( t_errno ) );
			}
			t_close(_sock);
			return(0);
		}
		sndcall->addr.buf = 0;
		if (t_free((char *)sndcall, T_CALL) < 0) {
			_tt_syslog( 0, LOG_ERR, "t_free(sndcall): %s",
				    t_strerror( t_errno ) );
		}
		if (t_free((char *)rcvcall, T_CALL) < 0) {
			_tt_syslog( 0, LOG_ERR, "t_free(rcvcall): %s",
				    t_strerror( t_errno ) );
		}
#endif				// !OPT_TLI

	}

	return(1);
}


/* 
 * Sends a message to a socket. Since the socket object is used by the mp
 * primarily to signal clients, a close is done after the write to insure
 * the message gets flushed out to the receiving socket.
 * 
 * --> It would be extremely helpful if all the really "fatal" cases of
 * failing to write to a socket can be identified since this information
 * could be fed back to the mp server to determine when a process has
 * died or is no longer listening on a socket.
 */
int _Tt_stream_socket::
send(char *msg, int len)
{
	int	rval;

#ifndef OPT_TLI
	if ((rval = ::send(_sock, msg, len, 0)) == len) {
		return(rval);
	} else {
		close(_sock);
		return(0);
	}
#else
#if defined(OPT_BUG_USL)
	t_sync(_sock);
#endif
	if ((rval = t_snd(_sock, msg, len, 0)) == len) {
		return(rval);
	} else {
		_tt_syslog(0, LOG_ERR,
			   "==> ERROR FROM SEND: len = %d, rval = %d, err = %d\n",
			   len, rval, t_errno);
		t_close(_sock);
		return(0);
	}
#endif				// !OPT_TLI
}


int _Tt_stream_socket::
accept()
{
	if (_msgsock == -1) {
#ifndef OPT_TLI
#if defined(linux) || defined(CSRG_BASED)
		socklen_t		addrlen = sizeof(sockaddr_in);
#else
		int			addrlen = sizeof(sockaddr_in);
#endif
		sockaddr_in		saddr;

#if defined(_AIX) && (OSMAJORVERSION==4) && (OSMINORVERSION==2)
		_msgsock = ::accept(_sock, (struct sockaddr *)&saddr,
				    (size_t *)&addrlen);
#else
		_msgsock = ::accept(_sock, (struct sockaddr *)&saddr,
				    &addrlen);
#endif
		if (_msgsock < 0) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::accept(): "
				    "accept(): %m" );
			return -1;
		}
		if (-1==fcntl(_msgsock, F_SETFD, 1)) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::accept(): "
				    "fcntl(F_SETFD): %m");
		}		
#else
		int			rval;
		struct t_call	       *call_data;
		struct t_bind	       *bind_data;

		call_data = (struct t_call *)t_alloc(_sock, T_CALL, T_ALL);

		if (t_listen(_sock, call_data) < 0) {
			_Tt_string errstr(t_strerror(t_errno));
			if (t_errno == TSYSERR) {
				// Add in errno info
				errstr = errstr.cat(": ").cat(strerror(errno));
			}
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::accept(): "
				    "t_listen(): %s", (char *) errstr );
			return(-1);
		}
		// Since we expect no further connections on this
		// endpoint, it would theoretically be possible
		// to use the same fd in arg 1 and arg 2 of t_accept.
		// This appears to be an actual advantage of TLI over
		// sockets -- I don't think you can do this with
		// sockets.  However, I can't get it to work!
		// So what I do is open a new endpoint, accept to
		// that, and then close the original fd since we
		// don't need it any more.
		
		_msgsock = t_open("/dev/tcp", O_RDWR, 0);
		if (_msgsock < 0) {
			_tt_syslog( 0, LOG_ERR,
				    "_Tt_stream_socket::accept(): "
				    "t_open(): %s",
				    t_strerror( t_errno ) );
			t_free((char *)call_data, T_CALL);
			return -1;
		}
		bind_data = (struct t_bind *)t_alloc(_msgsock, T_BIND, T_ALL);
		if (t_bind(_msgsock, bind_data, bind_data) < 0) {
			_tt_syslog( 0, LOG_ERR, "_Tt_stream_socket::accept(): "
				    "t_bind(): %s", t_strerror( t_errno ) );
			if (bind_data) t_free((char *)bind_data, T_BIND);
			if (call_data) t_free((char *)call_data, T_CALL);
			return -1;
		}
		
		rval = t_accept(_sock, _msgsock, call_data);
		if (rval == -1) {
			_tt_syslog( 0, LOG_ERR, "t_accept(): %s",
				    t_strerror( t_errno ) );
			t_free((char *)bind_data, T_BIND);
			t_free((char *)call_data, T_CALL);
			return(-1);
		} else {
			(void)_tt_tli_set_nodelay(_msgsock);
			if (-1==fcntl(_msgsock, F_SETFD, 1)) {
				_tt_syslog( 0, LOG_ERR,
					    "_Tt_stream_socket::accept(): "
					    "fcntl(F_SETFD): %m");
			}		
		}
		t_free((char *)bind_data, T_BIND);
		t_free((char *)call_data, T_CALL);
		t_close(_sock);
		_sock = _msgsock;
#endif				// !OPT_TLI
	}

	return(_msgsock);
}


/* 
* Receives a message from a socket. This method will block if there is
* no input so if nonblocking is required it should be called only after
* the socket fd has been checked for activity.
* 
* --> sockets can be set to be nonblocking. Should this be the default
* for the sockets the mp opens?
*/
int _Tt_stream_socket::
recv(char *msg, int msglen)
{
	int rval;
	
	if (_msgsock == -1 && accept() == -1) {
		return(-1);
	}
	
#ifndef OPT_TLI
	if ((rval = ::recv(_msgsock, msg, msglen, 0)) < 0) {
		close(_msgsock); 
		return(-1);
	}
#else
	int			flags;
	
	rval = t_rcv(_msgsock, msg, msglen, &flags);
	if (rval == -1) {
		if  (t_errno == TLOOK) {
			if (t_look(_msgsock) == T_DISCONNECT &&
			    t_rcvdis(_msgsock,(struct t_discon *)0) < 0) {
				_tt_syslog( 0, LOG_ERR, "t_rcvdis(): %s",
					    t_strerror( t_errno ) );
				return(-1);
			} else {
				return(0);
			}
		} else {
			_tt_syslog( 0, LOG_ERR, "t_rcv(): %s",
				    t_strerror( t_errno ) );
			return(-1);
		}
	}
#endif				// !OPT_TLI
	msg[rval] = 0;
	return(rval);
}



// read_would_block is not a predicate.  It returns 1 if a read
// is safe (would not block.  It returns 0 if a read would block,
// and -1 if there is some error condition.

int _Tt_stream_socket::
read_would_block()
{
	struct pollfd fds[1];

	fds[0].fd = _msgsock;
	fds[0].events = POLLIN;
	fds[0].revents = 0;

	while(-1 == poll(fds, (sizeof fds)/(sizeof (struct pollfd)), 0)) {
		if (errno==EAGAIN || errno==EINTR) {
			// interrupted, try again.
		} else {
			// something is wrong
			return -1;
		}
	}

	if (0 != (fds[0].revents & (POLLHUP|POLLNVAL|POLLERR)) ) {
		return -1;
	} else if (0 != (fds[0].revents & POLLIN) ) {
		return 1;
	} else {
		return 0;
	}
}


