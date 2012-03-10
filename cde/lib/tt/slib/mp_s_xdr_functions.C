//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: mp_s_xdr_functions.C /main/3 1995/10/23 12:00:33 rswiston $ 			 				
/*
 *
 * mp_s_xdr_functions.cc
 *
 * Server-side only xdr functions.
 *
 * In order that the server has _s_ versions of the routines,
 * we use slightly different xdr routines for decoding!  This
 * is a violation of XDR dogma \(but not really, since the XDR ing
 * is only done in the parent _Tt_xxx class; the only difference
 * is the constructor that gets called.\)
 *
 * Copyright (c) 1992 by Sun Microsystems, Inc.
 */
#include "mp_s_xdr_functions.h"

bool_t
tt_s_xdr_add_pattern_args(XDR *xdrs,_Tt_s_add_pattern_args *args)
{
	return(args->procid.xdr(xdrs) && args->pattern.xdr(xdrs));
}


bool_t
tt_s_xdr_del_pattern_args(XDR *xdrs,_Tt_s_del_pattern_args *args)
{
	return(args->procid.xdr(xdrs) && args->pattern_id.xdr(xdrs));
}


bool_t
tt_s_xdr_update_args(XDR *xdrs, _Tt_s_update_args *args)
{
	return( xdr_int(xdrs, (int *)&args->newstate) &&
	       args->message.xdr(xdrs));
}



