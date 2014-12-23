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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isbtree3.c /main/3 1995/10/23 11:36:12 rswiston $ 			 				 */
#ifndef lint
    static char sccsid[] = "@(#)isbtree3.c 1.5 89/07/17 Copyr 1988 Sun Micro";
#endif

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isbtree3.c
 *
 * Description:
 *	B-tree operations: REMOVE
 *	
 */

#include "isam_impl.h"

extern int _iskeycmp();

static void remove_entry(Btree *, char *, int);
static void move_from_right(Btree *, char *, char *, int);
static void move_from_left(Btree *, char *, char *, int);

/* _isbtree_remove() - Remove entry from B-tree ----------------------------*/

void
_isbtree_remove(Btree *btree)
{
    struct keydesc2	*pkeydesc2 = btree->keydesc2;
    int			nkeys;		     /* Number of keys in the page */
    register int	i;
    char		*pkp, *pkp2, *pkp3;
    struct bufhdr	*kp2bhdr;
    Blkno		blkno2;
    int			level;
    int			capac, halfcapac;
    int			move_keys;
    
    /* Set comparison function. */
    _iskeycmp_set(pkeydesc2, pkeydesc2->k2_nparts+1); /* +1 for recno field */
    
    /*
     * Main loop.
     *  Remove he current entry from leaf.
     *  If it was leftmost entry, propagate new leftmost to upper levels.
     *  If block has less than capac/2 + 1 entries, either move some entries
     *  from next left or right block, or merge with next left or right
     *  block.
     */
    
    for (i = btree->depth - 1; i >= 0; i--) {
	
	/* Re-fix the current page for update. */
	btree->bufhdr[i] = _isdisk_refix(btree->bufhdr[i], ISFIXWRITE);
	pkp = btree->bufhdr[i]->isb_buffer;
	
	level = ldshort(pkp + BT_LEVEL_OFF);
	capac = ldshort(pkp + BT_CAPAC_OFF); /* Block capacity */
	nkeys = ldshort(pkp + BT_NKEYS_OFF); /* Number of keys in block */
	
	halfcapac = (capac+1) >> 1;	     /* same as capac/2 + 1 */
	
	assert(level + i + 1 == btree->depth);
	assert(i == 0 || nkeys >= halfcapac);
	
	/* Remove entry from this page. */
	remove_entry(btree, pkp, btree->curpos[i]);
	nkeys--;
	
	/* Must propagate new leftmost key up. */
	if (btree->curpos[i] == 0 && i > 0)
	    leftkey_up(btree, i);
	
	if (nkeys >= halfcapac || i == 0) {
	    
	    /*
	     * Page is still balanced. No changes are necessary.
	     * Also, no chages are needed if this is root block.
	     */
	    
	    break;
	}
	else {
	    
	    if(btree->curpos[i-1] > 0) {
		
		/*
		 * Block pkp is not the leftmost descendent of its parent.
		 * Either merge with left brother, or move a few entries
		 * from it.
		 */
		
		pkp3 = btree->bufhdr[i-1]->isb_buffer;
		
		blkno2 = ldblkno(pkp3 + ISPAGESIZE - 
			(btree->curpos[i-1]) * BLKNOSIZE);
		
		/* Fix the left brother. */
		kp2bhdr = _isdisk_fix(btree->fcb, btree->fcb->indfd,
				      blkno2, ISFIXWRITE);
		pkp2 = kp2bhdr->isb_buffer;
		
		if (ldshort(pkp2 + BT_NKEYS_OFF) + nkeys > capac) {
		    
		    /*
		     * Current page cannot be merged with the left brother.
		     * Move some entries from the left brother into current.
		     */
		    
		    move_keys = (ldshort(pkp2 + BT_NKEYS_OFF) - halfcapac +1)/2;
		    assert(move_keys > 0);
		    
		    move_from_left (btree, pkp2, pkp, move_keys);
		    
		    leftkey_up(btree, i);     /* Propagate new leftmost key */
		    
		    break;		     /* From main loop */
		}
		else {
		    
		    /*
		     * Current page and left brotehr will be merged.
		     * Move all entries from current into left, and free
		     * current.
		     */
		    
		    /* Move all entries from current into left. */
		    move_from_right(btree, pkp2, pkp, nkeys); 
		    
		    /* Free current page. */
		    _isindfreel_free(btree->fcb, btree->bufhdr[i]->isb_blkno);
		    
		    btree->bufhdr[i] = kp2bhdr; /* Update search path */
		    
		    /* The loop must iterate to delete entry in parent block. */
		}
	    }
	    else {
		
		/*
		 * Block pkp is the leftmost descendent of its parent.
		 * Either merge with right brother, or move a few entries
		 * from it.
		 */
		
		pkp3 = btree->bufhdr[i-1]->isb_buffer;
		
		blkno2 = ldblkno(pkp3 + ISPAGESIZE - 
			(btree->curpos[i-1] + 2) * BLKNOSIZE);
		
		/* Fix the right brother. */
		kp2bhdr = _isdisk_fix(btree->fcb, btree->fcb->indfd,
				      blkno2, ISFIXWRITE);
		pkp2 = kp2bhdr->isb_buffer;
		
		if (ldshort(pkp2 + BT_NKEYS_OFF) + nkeys > capac) {
		    
		    /*
		     * Current page cannot be merged with the right brother.
		     * Move some entries from the right brother into current.
		     */
		    
		    move_keys = (ldshort(pkp2 + BT_NKEYS_OFF) - halfcapac +1)/2;
		    assert(move_keys > 0);
		    
		    move_from_right(btree, pkp, pkp2, move_keys);
		    
		    /* Update search path to go through right brother. */
		    btree->curpos[i-1]++;
		    btree->bufhdr[i] = kp2bhdr;
		    btree->curpos[i] = 0;
		    leftkey_up(btree, i);     /* Propagate new leftmost key */
		    
		    break;		     /* From main loop */
		}
		else {
		    
		    /*
		     * Current page and right brother will be merged.
		     * Move all entries from right into current, and free
		     * right.
		     */
		    
		    /* Move all entries from right brother into current. */
		    move_from_right(btree, pkp, pkp2, ldshort(pkp2 + BT_NKEYS_OFF)); 
		    
		    /* Free right brother page. */
		    _isindfreel_free(btree->fcb, kp2bhdr->isb_blkno);
		    
		    /* Update search path to point at right brother*/
		    btree->curpos[i-1]++;
		    /* The loop must iterate to delete entry in parent block. */

		}
	    }
	}
    }  /* end of main loop */
    
    if (btree->depth > 1) {
	pkp = btree->bufhdr[0]->isb_buffer;
	
	if (ldshort(pkp + BT_NKEYS_OFF) < 2) {
	    
	    /*
	     * Root now has only 1 entry and it is not the sole block.
	     * Replace root with its only descendant, don't change
	     * root blkno.
	     */
	    pkp2 = btree->bufhdr[1]->isb_buffer;
	    memcpy( pkp,pkp2, ISPAGESIZE);
	    
	    /* Free page. */
	    _isindfreel_free(btree->fcb, btree->bufhdr[1]->isb_blkno);
	}
    }
}


/*--------- remove supporting local functions -------------------------------*/

static void
remove_entry(Btree *btree, char *pkp, int pos)
{
    int    		keylength = btree->keydesc2->k2_len;
    int			nkeys = ldshort(pkp + BT_NKEYS_OFF);
    int			level = ldshort(pkp + BT_LEVEL_OFF);
    
    assert(nkeys > 0);
    assert(pos >= 0 && pos < nkeys);
    
    /* Shift nkeys - pos - 1 entries to the left. */
    memcpy(pkp + BT_KEYS_OFF + pos * keylength,
	   pkp + BT_KEYS_OFF + (pos + 1) * keylength,
	   (nkeys - pos - 1) * keylength);
    
    /* For non-leaf nodes, remove block number from table of down pointers. */
    if (level > 0) {
	
	memmove(pkp + ISPAGESIZE - (nkeys - 1) * BLKNOSIZE,
	       pkp + ISPAGESIZE - nkeys * BLKNOSIZE,
	       (nkeys - pos - 1) * BLKNOSIZE);
    }
    
    stshort((short) (nkeys - 1), pkp + BT_NKEYS_OFF);
}

static void
move_from_right(Btree *btree, char *l, char *r, int move_keys)
{
    int    	keylength = btree->keydesc2->k2_len;
    int		lnkeys = ldshort(l + BT_NKEYS_OFF);
    int		rnkeys = ldshort(r + BT_NKEYS_OFF);
    int		level = ldshort(r + BT_LEVEL_OFF);
    
    /* Move move_keys from l into r block. */
    memcpy( l + BT_KEYS_OFF + lnkeys * keylength,r + BT_KEYS_OFF,
	  move_keys * keylength);
    
    /* Move remaining entries in r to the left side. */
    memcpy( r + BT_KEYS_OFF,r + BT_KEYS_OFF + move_keys * keylength,
	  (rnkeys - move_keys) * keylength);
    
    /* If non-leaf, move the pointers stored at the end of block. */
    if (level > 0) {
	memcpy(l + ISPAGESIZE - (lnkeys + move_keys) * BLKNOSIZE,
	       r + ISPAGESIZE - move_keys * BLKNOSIZE,
	       move_keys * BLKNOSIZE);

	memmove(r + ISPAGESIZE - (rnkeys - move_keys) * BLKNOSIZE,
	       r + ISPAGESIZE - rnkeys * BLKNOSIZE,
	       (rnkeys - move_keys) * BLKNOSIZE);
    }
    
    lnkeys += move_keys;
    rnkeys -= move_keys;
    
    stshort((short) lnkeys, l + BT_NKEYS_OFF);
    stshort((short) rnkeys, r + BT_NKEYS_OFF);
}

static void
move_from_left(Btree *btree, char *l, char *r, int move_keys)
{
    int    	keylength = btree->keydesc2->k2_len;
    int		lnkeys = ldshort(l + BT_NKEYS_OFF);
    int		rnkeys = ldshort(r + BT_NKEYS_OFF);
    int		level = ldshort(r + BT_LEVEL_OFF);
    
    /* Move entries in r to the right side to create space for move_keys. */
    memmove( r + BT_KEYS_OFF + move_keys * keylength,r + BT_KEYS_OFF,
	  rnkeys * keylength);
    
    /* Move move_keys from l into r block. */
    memcpy( r + BT_KEYS_OFF,l + BT_KEYS_OFF + (lnkeys - move_keys) * keylength,
	  move_keys * keylength);
    
    /* If non-leaf,  move the pointers stored at the end of block. */
    if (level > 0) {
	
	memcpy(r + ISPAGESIZE - (rnkeys + move_keys) * BLKNOSIZE,
	       r + ISPAGESIZE - rnkeys * BLKNOSIZE,
	       rnkeys * BLKNOSIZE);
	
	memcpy(r + ISPAGESIZE - move_keys * BLKNOSIZE,
	       l + ISPAGESIZE - lnkeys * BLKNOSIZE,
	       move_keys * BLKNOSIZE);
    }
    
    lnkeys -= move_keys;
    rnkeys += move_keys;
    
    stshort((short) lnkeys, l + BT_NKEYS_OFF);
    stshort((short) rnkeys, r + BT_NKEYS_OFF);
}


