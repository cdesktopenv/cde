/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isindfreel.c /main/3 1995/10/23 11:41:03 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isindfreel.c 1.3 89/07/17 Copyr 1988 Sun Micro";
#endif

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
_isindfreel_alloc(fcb)
    Fcb			*fcb;
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
_isindfreel_free(fcb, blkno)
    Fcb			*fcb;
    Blkno		blkno;
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
