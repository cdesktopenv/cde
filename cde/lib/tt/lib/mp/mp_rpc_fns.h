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
/*%%  $XConsortium: mp_rpc_fns.h /main/3 1995/10/23 10:28:33 rswiston $ 			 				 */
/*
 *
 * mp_rpc_fns.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 *
 * This file just implements two function wrappers around two RPC
 * functions in order to workaround a problem with the C++ header files
 * (bug has already been submitted).   
 */
#ifndef _TT_MP_RPC_FNS_H
#define _TT_MP_RPC_FNS_H
#include <stdlib.h>
#include <tt_options.h>

extern "C" { void _tt_svc_freeargs(SVCXPRT *, xdrproc_t, char *); }
extern "C" { int  _tt_svc_getargs(SVCXPRT *, xdrproc_t, char *); }

#ifdef OPT_TLI
extern "C" { int _tt_bind_endpoint(int, char *, char *); }
extern "C" { int _tt_tli_set_nodelay(int fd); }
#endif				/* OPT_TLI */

#endif				/* _TT_MP_RPC_FNS_H */
