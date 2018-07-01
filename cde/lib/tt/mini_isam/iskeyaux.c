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
/*%%  $XConsortium: iskeyaux.c /main/3 1995/10/23 11:41:11 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */


/*
 * iskeyaux.c
 *
 * Description:
 *	Auxiliary key related functions
 */

#include "isam_impl.h"

Blkno _isgetfreekpage();

/* 
 * _isindex_extract() 
 *
 * Extract key parts from record buffer
 */

void
_iskey_extract (Keydesc2 *pkeydesc2, char *recp, char *keyp)
{
    int 	i;
    struct keypart2 *ppart;
    int 		nparts;

    nparts = pkeydesc2->k2_nparts;
    ppart = pkeydesc2->k2_part;

    /*
     * XXX - This is a kludge to fix a problem with keys of an odd (ODD as
     * not even) length that are extended to the nearest multiple of 2.
     *
     * It would be better to use bzero() here and not to call it in
     * add1key etc. But this change was done just during the FCS process
     * and may be robust even though it's not the cleanest thing under
     * the sun.
     */

    keyp[pkeydesc2->k2_len - 1] = '\0';

    for (i = 0; i < nparts; i++) {
	memcpy( keyp + ppart->kp2_offset,recp + ppart->kp2_start,
	       ppart->kp2_leng);
	ppart++;
    }
}

/*
 * _allockpage()
 *
 * Allocate an initialize new key page.
 * 
 * blkno, Output parameter
 */

Bufhdr *
_allockpage(Fcb *fcb, int capac, int level, Blkno *blkno)
{
    Bufhdr		*pbufhdr;
    char		*p;

    *blkno = _isindfreel_alloc(fcb);

    /* Fix the block in cache */
    pbufhdr = _isdisk_fix(fcb, fcb->indfd, *blkno, ISFIXNOREAD);
    
    p = pbufhdr->isb_buffer;

    memset(p, 0, ISPAGESIZE);

    /* Mark page as B-tree page. */
    stshort((short)PT_INDEX, p + BT_TYPE_OFF);

    /* Store page capacity. */
    stshort((short)capac, p + BT_CAPAC_OFF);

    /* Store B-tree level. */
    stshort((short)level, p + BT_LEVEL_OFF);

    return (pbufhdr);
}
