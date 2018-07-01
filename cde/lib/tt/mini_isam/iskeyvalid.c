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
/*%%  $XConsortium: iskeyvalid.c /main/3 1995/10/23 11:42:06 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * iskeydesc.c
 *
 * Description:
 *	Functions dealing with the keydesc structure.
 *	
 *
 */

#include "isam_impl.h"

static int _check_typelen();

/*
 * _validate_keydesc(keydesc, minreclen)
 *
 * Validate key descriptor. minreclen is needed to make suree that all
 * the key parts have a meaningful offset.
 *
 */

int
_validate_keydesc(struct keydesc *keydesc, int minreclen)
{
    int 		nparts;
    int 	i;
    int			type, start, length;
    int			keylen = 0;

    nparts =  keydesc->k_nparts;

    if (nparts <= 0 || nparts > NPARTS)
	return (ISERROR);

    for (i = 0; i < nparts;i++) {

	type = keydesc->k_part[i].kp_type & ~ISDESC;
	start = keydesc->k_part[i].kp_start;
	length = keydesc->k_part[i].kp_leng;

	if(_check_typelen(type, length) == ISERROR)
	    return (ISERROR);

	if (type < MINTYPE || type >= MAXTYPE)
	    return (ISERROR);

	if (start < 0 || start + length > minreclen)
	    return (ISERROR);

	keylen += length;
    }

    if(keylen > MAXKEYSIZE)
	return (ISERROR);

    return (ISOK);
}

/*
 * _check_typelen()
 *
 * Check length against the length of the corresponding type.
 */

static int
_check_typelen(int type, int length)
{
    switch (type) {
    case INTTYPE:
	return ((length == INTSIZE) ? ISOK : ISERROR);
    case LONGTYPE:
	return ((length == LONGSIZE) ? ISOK : ISERROR);
    case FLOATTYPE:
	return ((length == FLOATSIZE) ? ISOK : ISERROR);
    case DOUBLETYPE:
	return ((length == DOUBLESIZE) ? ISOK : ISERROR);
    case CHARTYPE:
    case BINTYPE:
	return ((length > 0) ? ISOK : ISERROR);
    default:
	return (ISERROR);
    }
}
    

