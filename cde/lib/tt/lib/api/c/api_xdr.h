/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: api_xdr.h /main/3 1995/10/23 09:55:57 rswiston $ 			 				 */
/*
 *
 * api_xdr.h
 *
 * XDR routines for datatypes visible at the API level.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#ifndef _API_XDR_H
#define _API_XDR_H

#include <stdio.h>
#include <rpc/xdr.h>
#include <util/tt_rpc_xdr.h>

#if defined(ultrix)
#include <rpc/xdr.h>
#define bool_t int
#endif

bool_t	_tt_xdr_cstring_list(XDR *xdrs, char ***ar);

#endif /* _API_XDR_H */
