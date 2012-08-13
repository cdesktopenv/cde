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
#include <stdlib.h>
#include <rpc/rpc.h>
#include "api/c/api_xdr.h"
#include <mp/mp_xdr_functions.h>

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
