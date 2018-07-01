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
/*%%  $XConsortium: isindfreel.c /main/3 1995/10/23 11:41:03 rswiston $ 			 				 */

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isfreelist.c
 *
 * Description:
 *	Free list maintenance functions
 */

#include "isam_impl.h"

extern Bufhdr *_isdisk_fix();

/*
 * blkno = _isfreel_alloc()
 *
 * Allocate a new index page.
 */

Blkno 
_isindfreel_alloc(Fcb *fcb)
{
    Bufhdr		*pbhdr;
    char		*p;
    int			npointers;
    Blkno		blkno;

    if (fcb->indfreelist == FREELIST_NOPAGE) {

	/* 
	 * We must write something to the buffer, or we will get 
	 * segmentation fault when using mapped I/O.
	 */
	fcb->indsize = _extend_file(fcb, fcb->indfd, fcb->indsize);

	return (fcb->indsize - 1);
    }

    pbhdr = _isdisk_fix(fcb, fcb->indfd, fcb->indfreelist, ISFIXWRITE);
    p = pbhdr->isb_buffer;
    
    npointers = ldshort(p + FL_NPOINTERS_OFF);
    
    if (npointers > 0) {
	blkno = ldblkno(p + FL_POINTERS_OFF + npointers * BLKNOSIZE);
	npointers--;
	stshort((short)npointers, p + FL_NPOINTERS_OFF);
	
	return (ldblkno(p + FL_POINTERS_OFF + npointers * BLKNOSIZE));
    }
    else {
	blkno = fcb->indfreelist;
	fcb->indfreelist = ldblkno(p + FL_NEXT_OFF);
	
	return (blkno);
    }
}

/*
 * _isfreel_free()
 *
 * Free an index page.
 */

void
_isindfreel_free(Fcb *fcb, Blkno blkno)
{
    Bufhdr		*pbhdr;
    char		*p;
    int			npointers;

    if (fcb->indfreelist != FREELIST_NOPAGE) {
	pbhdr = _isdisk_fix(fcb, fcb->indfd, fcb->indfreelist, ISFIXWRITE);
	p = pbhdr->isb_buffer;

	npointers = ldshort(p + FL_NPOINTERS_OFF);
	
	if (npointers < FL_MAXNPOINTERS) {
	    stblkno(blkno, p + FL_POINTERS_OFF + npointers * BLKNOSIZE);
	    npointers++;
	    stshort((short)npointers, p + FL_NPOINTERS_OFF);

	    return;
	}
    }


    pbhdr = _isdisk_fix(fcb, fcb->indfd, blkno, ISFIXWRITE);
    p = pbhdr->isb_buffer;
    
    /* Mark page to indicate that it is in the free list. */
    stshort((short)PT_FREELIST, p + FL_TYPE_OFF);

    stshort((short)0, p + FL_NPOINTERS_OFF);
    stblkno(fcb->indfreelist, p + FL_NEXT_OFF);

    fcb->indfreelist = blkno;
}
