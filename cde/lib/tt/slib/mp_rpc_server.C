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
//%%  $TOG: mp_rpc_server.C /main/11 1999/08/30 11:03:00 mgreess $ 			 				
/*
 *
 * @(#)mp_rpc_server.C	1.46	94/11/17
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#include "tt_options.h"

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

#include "mp_rpc_server.h"
#include "util/tt_port.h"
#include "util/tt_gettext.h"
#include "util/tt_global_env.h"
#include "mp/mp_mp.h"
#include "mp/mp_rpc.h"

#if defined(OPT_TLI)
#include <netdir.h>
static int	gettransient(int, netconfig *, netbuf *);
#if defined(OPT_BUG_SUNOS_5) || defined(OPT_BUG_UXP)
extern "C" { char *	nc_sperror(); }
#endif
# if defined(OPT_BUG_USL) || defined(OPT_BUG_UXP)
    extern int   t_errno;
    extern char *t_strerror(int t_errno);
# endif
#else 
#include <rpc/pmap_clnt.h>
#include <netinet/tcp.h>
static int	gettransient(int,int,int *);
#endif /* OPT_TLI */

#if defined(OPT_BUG_USL)
     typedef void (*SERVICE_FN_TYPE)(const struct svc_req *, const SVCXPRT*);
#elif defined(OPT_BUG_AIX)
     typedef void (*SERVICE_FN_TYPE)();
#else
     typedef void (*SERVICE_FN_TYPE)(struct svc_req *, SVCXPRT*);
#endif


/* 
 * Constructs an rpc server for the given program, version and socket.
 */
_Tt_rpc_server::
_Tt_rpc_server(int program, int version, int Rsocket, _Tt_auth &auth)
{
	_version = version;
	_socket = Rsocket;
	_program = program;
	_auth = auth;
}


/* 
 * Destroys an rpc server. Unsets the program,version mapping in the
 * portmapper. 
 */
_Tt_rpc_server::
~_Tt_rpc_server()
{
#ifndef OPT_TLI	
/* 
 * 	pmap_unset(_program, _version);
 */
#else
	for (int version = _version; version >= 1; version--) {
		rpcb_unset(_program, version, (netconfig *)0);
	}
#endif				// OPT_TLI
}


/* 
 * Initializes an rpc server with a service function. If _program is set
 * to -1 then an unused program number is obtained using the gettransient
 * function. If _socket is anything other than RPC_ANYSOCK then it will
 * be used to create the rpc transport using svfd_create and the rpc
 * numbers will not be registered with the portmapper.
 */
int _Tt_rpc_server::
init(void (*service_fn)(struct svc_req *, SVCXPRT *))
{
	char		*bufopt = (char *)0;

#ifndef OPT_TLI

	bufopt = getenv("TT_BUFSIZE");
	unsigned int buffersize = (bufopt != (char *)0) ? atoi(bufopt) : 32000;

	if (_socket != RPC_ANYSOCK) {
#ifdef OPT_UNIX_SOCKET_RPC
		_transp = svcfd_create(_socket, buffersize, buffersize);
		if (_transp == (SVCXPRT *)0) {
			return(0);
		}
		if (!svc_register(_transp, _program, _version,
				  (SERVICE_FN_TYPE)service_fn, 0))
		{
			_tt_syslog(0, LOG_ERR, "svc_register(): %m");
			return(0);
		}
#endif

		return(1);
	}

		
	if (_program == -1) {
		if (! (_program =
		       gettransient(IPPROTO_TCP, _version, &_socket))) {
			return(0);
		}
	} else {
		_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (_socket < 0) {
			_tt_syslog(0, LOG_ERR,
				   "_Tt_rpc_server::init(): socket(): %m");
			return 0;
		}
	}
	int optval = 1;
	if (setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY,
		       (char *)&optval, sizeof(int)) == -1) {
		_tt_syslog(0, LOG_ERR, "setsockopt(TCP_NODELAY): %m");
	}
	if (setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char *)&buffersize,
		       sizeof(int)) == -1) {
		_tt_syslog(0, LOG_ERR, "setsockopt(SO_RCVBUF): %m");
	}
	if (setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char *)&buffersize,
		       sizeof(int)) == -1) {
		_tt_syslog(0, LOG_ERR, "setsockopt(SO_SNDBUF): %m");
	}
	_transp = svctcp_create(_socket, buffersize, buffersize);
	if (_transp == (SVCXPRT *)0) {
		return(0);
	}
	if (   !svc_register(_transp, _program, _version,
			     (SERVICE_FN_TYPE)service_fn, IPPROTO_TCP)
	    || !svc_register(_transp, _program, 1,
			     (SERVICE_FN_TYPE)service_fn, 0))
	{
		_tt_syslog(0, LOG_ERR, "svc_register(): %m");
		return(0);
	}
#else
	netconfig		*nconf;
	void			*handlep;
	t_info			tinfo;
	int			fd;


	if ((handlep = setnetconfig()) == (void *)0) {
		_tt_syslog(0, LOG_ERR, "setnetconfig(): %s", nc_sperror());
		return(0);
	}

	// Find a connection-oriented transport.
	while (nconf = getnetconfig(handlep)) {
		if ((nconf->nc_semantics == NC_TPI_COTS) ||
		    (nconf->nc_semantics == NC_TPI_COTS_ORD)) {

		        // Make sure this netconfig maps to an address
		        if (0 == strcmp(nconf->nc_protofmly, NC_INET))
			    break;
		}
	}

	// If we failed to find a suitable transport, exit.
	if (nconf == (netconfig *)0) {
		endnetconfig(handlep);
		_tt_syslog(0, LOG_ERR,
			   catgets(_ttcatd, 2, 3,
				   "No connection-oriented transport"));
		return(0);
	}
	
	fd = t_open(nconf->nc_device, O_RDWR, &tinfo);
	if (fd == -1) {
		_tt_syslog(0, LOG_ERR,
			   "_Tt_rpc_server::init(): t_open(): %s",
			   t_strerror( t_errno ) );
		endnetconfig(handlep);
		return 0;
	}
	// No longer need to try to set NODELAY here as TIRPC does it for us
	// tinfo.tsdu can be negative, but that's not a valid buf size.
	u_int buf_size = 0;
	if (tinfo.tsdu > 0) {
		buf_size = (u_int)tinfo.tsdu;
	}
	_transp = svc_tli_create(fd, nconf, (struct t_bind *)0,
				 buf_size, buf_size);
	if (_transp == (SVCXPRT *)0) {
		_tt_syslog(0, LOG_ERR, "svc_tli_create(): 0");
		(void)t_close(fd);
		return(0);
	}
	if (_program == -1 &&
	    (! (_program = gettransient(_version, nconf,
					&_transp->xp_ltaddr)))) {
		_tt_syslog(0, LOG_ERR, "gettransient(): 0");
		return(0);
	}
	for (int version = _version; version >= 1; version--) {
		if (!svc_reg(_transp, _program, version,
			     (SERVICE_FN_TYPE)service_fn, nconf)) {
			_tt_syslog(0, LOG_ERR, "svc_reg(,,%d): 0", version);
			return(0);
		}
	}
	// it is important to not call endnetconfig until one is done
	// using nconf as endnetconfig frees the nconf storage.
	(void)endnetconfig(handlep);
#endif				/* OPT_TLI */
	// now figure out what fd the rpc package is using
	int maxfds = _tt_global->maxfds();
	for (int i=0; i < maxfds; i++) {
		if (FD_ISSET(i, &svc_fdset)) {
			_rpc_fd = i;
		}
	}

	return(1);
}


/* 
 * Runs an rpc server. If a non-negative timeout is given then this
 * function will return if the timeout expired before any rpc requests
 * came in. The values returned are: -1 for error, 0 for timeout, 1
 * for when timeout is 0 and an rpc request was serviced.
 */
_Tt_rpcsrv_err _Tt_rpc_server::
run_until(int *stop, int timeout, _Tt_int_rec_list_ptr &efds)
{
	fd_set			readfds;
	timeval			tmout;
	int			fd;
	int			done = 0;
	int			select_stat;
	_Tt_rpcsrv_err		status = _TT_RPCSRV_OK;

	tmout.tv_sec = timeout;
	tmout.tv_usec = 0;
	_Tt_int_rec_list_cursor	efds_c(efds);
	do {
		// Add our fd's to a copy of the rpc fdset.
		readfds = svc_fdset;
		efds_c.reset();
		while (efds_c.next()) {
			fd = efds_c->val;
			// NOTE that it is crucially important that the bit
			// for fd 0 not be set.  fd 0 (stdin) is always set
			// to /dev/null, which is always active.
			// The reason fd 0 is in efds at all is that 
			// _Tt_self_procid uses it as a dummy entry
			// for ttsession itself, which doesn\'t need a
			// signalling channel.

			// I haven\'t verified this, but I bet it\'s possible
			// for negative entries to be in the efds list too,
			// representing signalling channels that were found
			// active on a previous pass but are not yet cleared
			// out.
			if (fd > 0) {
				FD_SET(fd, &readfds);
			}
		}

		// Drop the global mutex around any polling or RPC calls.
		
		_tt_global->drop_mutex();
		
		select_stat = 
			select(FD_SETSIZE,&readfds, 0, 0,
			       (timeout >= 0) ? &tmout : (timeval *)0);

		_tt_global->grab_mutex();

		switch (select_stat) {
		      case -1:
			return(_TT_RPCSRV_ERR);
		      case 0:
			return(_TT_RPCSRV_TMOUT);
		      default:
			// check for exception fds
			efds_c.reset();
			while (efds_c.next()) {
				fd = efds_c->val;
				if (fd < 0) continue;	// -1 => not valid fd
				if (FD_ISSET(fd, &readfds)) {
					efds_c->val = (0 - fd);
					status = _TT_RPCSRV_FDERR;
					done = 1;
				}

				// Clear our fd from the fdset so
				// svc_getreqset() won't get confused (bug
				// 2000972).

				FD_CLR(fd, &readfds);
			}
			svc_getreqset(&readfds);
		}
	} while ((! done) && ((stop == 0) || (! *stop)));
	return status;
}


/* 
 * Returns an unused transient program number. Definition taken out of
 * the RPC manual.
 */
#ifdef OPT_TLI
static int
gettransient(int vers, netconfig *nconf, netbuf *address)
#else
static int
gettransient(int proto, int vers, int *sockp)
#endif				/* OPT_TLI */
{
	int			prognum;

#ifndef OPT_TLI
	int			found;
	int			s;
#if defined(linux) || defined(CSRG_BASED)
	socklen_t		len;
#else
	int			len;
#endif
	int			socktype;
	sockaddr_in		addr;
	sockaddr_in		tport;
	sockaddr_in		uport;

	switch (proto) {
	      case IPPROTO_UDP:
		socktype = SOCK_DGRAM;
		break;
	      case IPPROTO_TCP:
		socktype = SOCK_STREAM;
		break;
	      default:
		return(0);
	}
	if (*sockp == RPC_ANYSOCK) {
		s = socket(AF_INET, socktype, 0);
		if (s < 0) {
			_tt_syslog(0, LOG_ERR, "gettransient(): socket(): %m");
			return 0;
		}
		*sockp = s;
	} else {
		s = *sockp;
	}
        memset(&addr, 0, sizeof(addr));
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(0);
        addr.sin_family = AF_INET;
	len = sizeof(addr);
	bind(s, (sockaddr *)&addr, len);
#if defined (_AIX) && (OSMAJORVERSION==4) && (OSMINORVERSION==2)
	if (getsockname(s, (sockaddr *)&addr, (size_t *)&len) < 0) {
#else
	if (getsockname(s, (sockaddr *)&addr, &len) < 0) {
#endif
		_tt_syslog(0, LOG_ERR, "getsockname(): %m");
		return(0);
	}

	int optval = 0;
#if !defined(linux)
	if (setsockopt(s, SOL_SOCKET, SO_USELOOPBACK,
		       (char *)&optval, sizeof(optval)) == -1) {
	}
#endif
#endif				/* !OPT_TLI */


	// Search for a transient rpc number in the range 0x40000000 -
	// 0x5fffffff by starting in the middle of the range searching
	// up and then searching down if that fails. The reason for
	// this is to make it less likely for other programs to grab
	// this transient number (since pmap_getport doesn't complain
	// if you try to grab a number for udp and we have it grabbed
	// for tcp).

	// search up in the range 0x4fffffff - 0x5fffffff
	for (prognum = 0x4fffffff; prognum <= 0x5fffffff; prognum++) {
		/* XXX: pmap_set allows the same prognum for different	*/
		/* protocols so we hack around that by attemptint to	*/
		/* set both tcp and udp. */

#ifndef OPT_TLI
		found = (!pmap_getport(&uport, prognum, vers,
				       IPPROTO_UDP) &&
			 !pmap_getport(&tport, prognum, vers, proto));
		if (found &&
		    (found = pmap_set(prognum,
				      vers,
				      proto,
				      ntohs(addr.sin_port))) &&
		    (vers==1 || (found = pmap_set(prognum,
						  1,
						  proto,
						  ntohs(addr.sin_port))))) {
			return(prognum);
		}
#else
		if (rpcb_set(prognum, vers, nconf, address) &&
		    (vers==1 || rpcb_set(prognum, 1, nconf, address))) {
			return(prognum);
		}
#endif				/* !OPT_TLI */
	}

	// search down in the range 0x4ffffffe - 0x40000000
	for (prognum = 0x4ffffffe; prognum >= 0x40000000; prognum--) {
		/* XXX: pmap_set allows the same prognum for different	*/
		/* protocols so we hack around that by attemptint to	*/
		/* set both tcp and udp. */
#ifndef OPT_TLI
		found = (!pmap_getport(&uport, prognum, vers,
				       IPPROTO_UDP) &&
			 !pmap_getport(&tport, prognum, vers, proto));
		if (found &&
		    (found = pmap_set(prognum,
				      vers,
				      proto,
				      ntohs(addr.sin_port)))) {
			return(prognum);
		}
#else
		if (rpcb_set(prognum, vers, nconf, address)) {
			return(prognum);
		}
#endif				/* !OPT_TLI */
	}

	return(0);
}




