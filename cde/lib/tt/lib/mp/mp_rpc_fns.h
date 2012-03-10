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
