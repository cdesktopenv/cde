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
/*%%  $XConsortium: isindexconv.c /main/3 1995/10/23 11:40:48 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isindexconv.c
 *
 * Description:
 *	Encode/decode key descriptor
 */

#include "isam_impl.h"

void
stkey(Keydesc2 *pkeydesc2, char *p)
{
    int                         i;
    struct keypart2   	*ppart;
    char               *pp;
    int                         nparts;

    stshort(pkeydesc2->k2_flags, p + K2_FLAGS_OFF);
    stshort(pkeydesc2->k2_nparts, p + K2_NPARTS_OFF);
    stshort(pkeydesc2->k2_len, p + K2_LEN_OFF);
    stblkno(pkeydesc2->k2_rootnode, p + K2_ROOT_OFF);
    stlong((long)pkeydesc2->k2_keyid, p + K2_KEYID_OFF);

    ppart = pkeydesc2->k2_part;
    pp    = p + K2_KEYPART_OFF;
    nparts = pkeydesc2->k2_nparts + 1;	     /* +1 for recno part */

    if (ALLOWS_DUPS2(pkeydesc2))	     /* +1 for dups serial number*/
	nparts++;
 
    for (i = 0; i < nparts;i++) {
        stshort(ppart->kp2_start, pp);
        pp += SHORTSIZE;
 
        stshort(ppart->kp2_leng, pp);
        pp += SHORTSIZE;
 
        stshort(ppart->kp2_type, pp);
        pp += SHORTSIZE;
 
        stshort(ppart->kp2_offset, pp);
        pp += SHORTSIZE;
        
        ppart++;
    }
}      

void
ldkey(struct keydesc2 *pkeydesc2, char *p)
{
    int                         i;
    struct keypart2    *ppart;
    char               *pp;
    int                         nparts;

    memset ((char *) pkeydesc2, 0, sizeof (*pkeydesc2));
    pkeydesc2->k2_flags = ldshort(p + K2_FLAGS_OFF);
    pkeydesc2->k2_nparts = ldshort(p + K2_NPARTS_OFF);
    pkeydesc2->k2_len = ldshort(p + K2_LEN_OFF);
    pkeydesc2->k2_rootnode = ldblkno(p + K2_ROOT_OFF);
    pkeydesc2->k2_keyid = ldlong(p + K2_KEYID_OFF);

    ppart = pkeydesc2->k2_part;
    pp    = p + K2_KEYPART_OFF;
    nparts = pkeydesc2->k2_nparts + 1;	     /* +1 for recno part */

    if (ALLOWS_DUPS2(pkeydesc2))	     /* +1 for dups serial number*/
	nparts++;

    for (i = 0; i < nparts;i++) {
        ppart->kp2_start = ldunshort(pp);
        pp += SHORTSIZE;

        ppart->kp2_leng = ldshort(pp);
        pp += SHORTSIZE;

        ppart->kp2_type = ldshort(pp);
        pp += SHORTSIZE;

        ppart->kp2_offset = ldunshort(pp);
        pp += SHORTSIZE;
        
        ppart++;
    }
}
