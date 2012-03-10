//%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			
//%%  (c) Copyright 1993, 1994 International Business Machines Corp.	
//%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			
//%%  (c) Copyright 1993, 1994 Novell, Inc. 				
//%%  $XConsortium: api_xdr.C /main/3 1995/10/23 09:55:50 rswiston $ 			 				
/*
 *
 * api_xdr.cc
 *
 * XDR routines for datatypes visible at the API level.
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */

#include "tt_options.h"
#if defined(OPT_BUG_SUNOS_4) && defined(__GNUG__)
#	include <stdlib.h>
#endif
#include <rpc/rpc.h>
#include "api/c/api_xdr.h"
#include <mp/mp_xdr_functions.h>
#include <malloc.h>

bool_t
_tt_xdr_cstring_list(XDR *xdrs, char ***ar)
{
	int count;
	char **tar;
	int i;

	if (xdrs->x_op == XDR_ENCODE) {
		// count the number of entries in the list
		tar = *ar;
		while (*tar++);
		count = tar-*ar;
	}
	if (!xdr_int(xdrs, &count)) return 0;
	if (xdrs->x_op == XDR_DECODE) {
		// allocate a new list
		tar = (char **)malloc((count+1)*sizeof(char *));
		if (tar==(char **)0) return 0;
		tar[count] = (char *)0;
		*ar = tar;
	} else {
		tar = *ar;
	}
	for (i=0;i<count;++i) {
		if (!tt_xdr_bstring(xdrs, &tar[i])) return 0;
	}
	if (xdrs->x_op == XDR_FREE) {
		free((MALLOCTYPE *)tar);
	}
	return 1;
}
