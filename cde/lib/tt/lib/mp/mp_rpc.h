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
/*%%  $TOG: mp_rpc.h /main/5 1999/08/30 10:55:41 mgreess $ 			 				 */
/*
 *
 * @(#)mp_rpc.h	1.15 93/09/07
 *
 * Copyright (c) 1990,1993 by Sun Microsystems, Inc.
 *
 * Common declarations for RPC implementation classes.
 */
#ifndef _TT_MP_RPC
#define _TT_MP_RPC
#include "tt_options.h"

#if defined(__linux__)
# define __SVR4_I386_ABI_L1__
#endif
#include <rpc/rpc.h>
#if defined(__linux__)
# undef __SVR4_I386_ABI_L1__
#endif

#if defined(_AIX)
extern "C" {
  void svcerr_auth (SVCXPRT *, enum auth_stat);
  SVCXPRT *svcfd_create(int,int,int);
  void svc_getreqset(fd_set *);
};
#endif

#if defined(OPT_TLI)
#	include <netconfig.h>
#	include <tiuser.h>
#else
#	include <sys/socket.h>
#	include <netinet/in.h>
#endif /* OPT_TLI */

#include <sys/types.h>
#include "util/tt_object.h"
#include "util/tt_ptr.h"
#include "util/tt_int_rec.h"
#include "mp/mp_rpc_fns.h"
#include "util/tt_host_utils.h"

enum _Tt_rpcsrv_err {
	_TT_RPCSRV_OK,
	_TT_RPCSRV_ERR,
	_TT_RPCSRV_TMOUT,
	_TT_RPCSRV_FDERR
};

#endif				/* _TT_MP_RPC */
