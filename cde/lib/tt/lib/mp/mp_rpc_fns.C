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
/*%%  $XConsortium: mp_rpc_fns.C /main/4 1996/04/21 19:12:28 drk $ 			 				 */
/*
 * mp_rpc_fns.C
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include <tt_options.h>
#include <rpc/rpc.h>

#if defined(OPT_TLI)

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <tiuser.h>
#include <sys/socket.h>


/* 
 * Sets the TCP_NODELAY option for a tli stream bound using tcp. This
 * option is crucial to good message performance since otherwise there is
 * a 200 ms delay between messages. This code is based on the source code
 * for tli rpc (since documentation was sorely lacking).
 */
extern "C" 
int
_tt_tli_set_nodelay(int fd)
{
	struct t_optmgmt *options;
	struct sochdr {
		struct opthdr opthdr;
		long value;
	} sochdr;

	options = (struct t_optmgmt *)t_alloc(fd, T_OPTMGMT, 0);
	if (options == (struct t_optmgmt *)0) {
		return (0);
	}
	sochdr.opthdr.level = IPPROTO_TCP;
	sochdr.opthdr.name = TCP_NODELAY;
	sochdr.opthdr.len = 4;
	sochdr.value = 1;
	options->opt.maxlen = sizeof(sochdr);
	options->opt.len = sizeof(sochdr);
	options->opt.buf =  (char *) &sochdr;
	options->flags = T_NEGOTIATE;
	if (t_optmgmt(fd, options, options) == -1) {
		t_error("t_optmgmt");
		return(0);
	}
	options->opt.buf = 0;
	(void) t_free((char *)options, T_OPTMGMT);
	return(1);
}


extern "C" 
int
_tt_bind_endpoint(int	fd,
		  char	*s,
		  char	*r)
{
	return(t_bind(fd, (struct t_bind *)s, (struct t_bind *)r));
}
#endif				/* OPT_TLI */


/* 
 *   XXX: the following are needed because of bugs in the C++ header files
 *   for these functions. Should use the native definitions when the header
 *   files are fixed.
 */

extern "C" 
void
_tt_svc_freeargs(SVCXPRT	*transp,
		 xdrproc_t	inproc,
		 char		*in)
{
	svc_freeargs(transp, inproc, in);
}

extern "C" 
int
_tt_svc_getargs(SVCXPRT		*transp,
		xdrproc_t	inproc,
		char		*in)
{
	return(svc_getargs(transp, inproc, in));
}
