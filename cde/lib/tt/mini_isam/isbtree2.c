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
/*%%  $XConsortium: isbtree2.c /main/3 1995/10/23 11:36:02 rswiston $ 			 				 */
#ifndef lint
    static char sccsid[] = "@(#)isbtree2.c 1.5 89/07/17 Copyr 1988 Sun Micro";
#endif

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isbtree2.c
 *
 * Description:
 *	B-tree operations:  INSERT
 *	
 */

#include "isam_impl.h"

extern int _iskeycmp();

void leftkey_up(Btree *, int);

static void insert_key(Btree *, char *, int, char *, Blkno);
static void splitblock(Btree *, char *, char *, int);


/* _isbtree_insert() - Insert entry into B-tree ----------------------------*/

void
_isbtree_insert(Btree *btree, char *key)
{
    Keydesc2		*pkeydesc2 = btree->keydesc2;
    int			keylength = pkeydesc2->k2_len;
    int			nkeys;		     /* Number of keys in the page */
    int			capac;
    char		keybuf[MAXKEYSIZE];
    register int	i;
    Blkno		blkno;
    char		*pkp, *pkp2, *pkp3;
    Bufhdr		*kp2bhdr, *kp3bhdr;
    Blkno		blkno2, blkno3;
    int			level;
    int			halfcapac;
    
    
    /* 
     * Set key comparison function. 
     */
    _iskeycmp_set(pkeydesc2, pkeydesc2->k2_nparts+1); /* +1 for recno field */
    
    /* 
     * Initialize data structure before main loop. 
     */
    blkno = NULL_BLKNO;
    memcpy( keybuf,key, keylength);
    
    /*
     * Main loop:
     *   Starting from the leaf, insert key after current search position.
     *   If necessary make block split and propage changes to upper levels.
     *   Root split is handled differently because we don't want to change
     *	 root block number.
     */
    
    for (i = btree->depth - 1; i >= 0; i--) {
	
	/* We have to fix the block for update. */
	btree->bufhdr[i] = _isdisk_refix(btree->bufhdr[i], ISFIXWRITE);
	pkp = btree->bufhdr[i]->isb_buffer;
	
	level = ldshort(pkp + BT_LEVEL_OFF);
	capac = ldshort(pkp + BT_CAPAC_OFF); /* Block capacity */
	nkeys = ldshort(pkp + BT_NKEYS_OFF); /* Number of keys in block */
	
	assert(level + i + 1 == btree->depth);
	assert(nkeys <= capac);
	
	if (nkeys == capac) {
	    /*
	     * This block must be split.
	     * Allocate new block, move half of the entries into the new
	     * block, and insert the key into the appropriate 
	     * block. If this block is not root, the loop will iterate 
	     * to update upper levels.
	     */
	    
	    /* Get new page. */
	    kp2bhdr = _allockpage(btree->fcb, capac, level, &blkno2);
	    pkp2 = kp2bhdr->isb_buffer;
	    
	    halfcapac = (nkeys+1) >> 1;	     /* Same as nkeys/2 + 1 */
	    
	    splitblock(btree, pkp, pkp2, btree->curpos[i]);
	    
	    if (btree->curpos[i] > halfcapac - 2) {
		
		/* Insert entry into right block. */
		insert_key(btree, pkp2, btree->curpos[i] - halfcapac, keybuf,
			   blkno);
	    }
	    else {
		/* Insert entry into left block. */
		insert_key(btree, pkp, btree->curpos[i], keybuf, blkno);
		
		if (btree->curpos[i] == -1) /* Leftmost entry inserted - */
		    leftkey_up(btree, i);   /* propagate to upper levels. */
	    }
	    
	    /* Set variables for next loop iteration. */
	    memcpy( keybuf,pkp2 + BT_KEYS_OFF, keylength); /* Leftmost key */
	    blkno = blkno2;
	    
	    /* 
	     * Next loop iteration will insert entry pointing to 
	     * new allocated page into next upper level.
	     */
	}
	else {
	    /*
	     * Block split is not necessary. Simply insert key into 
	     * the block. If the inserted key becomes the leftmost entry
	     * in the block,  update upper levels.
	     */
	    
	    insert_key(btree, pkp, btree->curpos[i], keybuf, blkno);
	    
	    if (btree->curpos[i] == -1 && i > 0)
		leftkey_up(btree, i);
	    
	    break;			     /* from main loop */
	}
	
    }
    
    if (i < 0) {
	
	/*
	 * Root was split.
	 * Allocate new page.  However, to keep root block the same, 
	 * the new page is used for the left son of root.
	 *
	 * pkp is the new root, pkp3 is the left son, 
	 * and pkp3 is the right of the root.
	 */
	
	kp3bhdr = _allockpage(btree->fcb, capac, 1, &blkno3);
	pkp3 = kp3bhdr->isb_buffer;
	
	memcpy( pkp3,pkp, ISPAGESIZE);
	
	stshort((short) getkeyspernode(keylength), pkp + BT_CAPAC_OFF);
	stshort((short) 0, pkp + BT_NKEYS_OFF);
	stshort((short)btree->depth, pkp + BT_LEVEL_OFF);
	
	insert_key(btree, pkp, -1, pkp3 + BT_KEYS_OFF, blkno3);
	insert_key(btree, pkp, 0 , keybuf, blkno);
    }
}

/*--------- insert supporting local functions -------------------------------*/

/* leftkey_up() - Update upper levels with new leftmost entry -----------*/
void
leftkey_up(Btree *btree, int level)
{
    int    		keylength = btree->keydesc2->k2_len;
    char   	 	*pkp;
    char		*key;
    
    pkp = btree->bufhdr[level]->isb_buffer;
    key = pkp + BT_KEYS_OFF;		     /* Leftmost key */
    
    while (--level >= 0) {
	
	btree->bufhdr[level] = _isdisk_refix(btree->bufhdr[level], ISFIXWRITE);
	pkp = btree->bufhdr[level]->isb_buffer;
	
	memcpy( pkp + BT_KEYS_OFF + (btree->curpos[level] * keylength),key,
	      keylength);
	
	if (btree->curpos[level] > 0)
	    break;
    }
}

/* insert_key - Insert key into block ------------------------*/
static void
insert_key(Btree *btree, char *pkp, int pos, char *key, Blkno blkno)
{
    int    	keylength = btree->keydesc2->k2_len;
    int		nkeys = ldshort(pkp + BT_NKEYS_OFF);
    int		capac = ldshort(pkp + BT_CAPAC_OFF);
    int		level = ldshort(pkp + BT_LEVEL_OFF);
    
    assert(nkeys < capac);
    
    /* Shift nkeys - pos - 1 entries to the right. */
    /* memmove() handle overlaps correctly */

    memmove(pkp + BT_KEYS_OFF + (pos + 2) * keylength,
	   pkp + BT_KEYS_OFF + (pos + 1) * keylength,
	   (nkeys - pos - 1) * keylength);
    
    /* Copy new key entry into the block. */
    memcpy( pkp + BT_KEYS_OFF + (pos + 1) * keylength,key, keylength);
    
    /* For non-leaf nodes,  insert block number into table of down pointers. */
    if (level > 0) {
	
	memcpy(pkp + ISPAGESIZE - (nkeys + 1) * BLKNOSIZE,
	       pkp + ISPAGESIZE - nkeys * BLKNOSIZE,
	       (nkeys - pos - 1) * BLKNOSIZE);
	
	stblkno(blkno, pkp + ISPAGESIZE - (pos + 2) * BLKNOSIZE);
    }
    
    stshort((short) (nkeys + 1), pkp + BT_NKEYS_OFF);
}

/* splitblock() - Split block into two -----------------------------*/
static void
splitblock(Btree *btree, char *fullpage, char *newpage, int pos)
{
    int    		keylength = btree->keydesc2->k2_len;
    int			nkeys, capac, level;
    int			halfcapac;
    int			newpage_nkeys;
    int			fullpage_nkeys;
    
    nkeys = ldshort(fullpage + BT_NKEYS_OFF);
    capac = ldshort(fullpage + BT_NKEYS_OFF);
    level = ldshort(fullpage + BT_LEVEL_OFF);
    
    assert(nkeys == capac);
    
    halfcapac = (capac + 1) >> 1;	     /* same as capac/2 + 1 */
    
    if (pos > halfcapac - 2) {
	
	/* New entry will go into right page(fullpage). */
	fullpage_nkeys = halfcapac;
	newpage_nkeys   = halfcapac - 1;
    }
    else {
	
	/* New entry will go into left page (newpage). */
	fullpage_nkeys = halfcapac - 1;
	newpage_nkeys   = halfcapac;
    }
    
    /* Move newpage_nkeys keys into newpage. */
    memcpy(newpage + BT_KEYS_OFF,
	   fullpage + BT_KEYS_OFF + fullpage_nkeys * keylength,
	   keylength * newpage_nkeys);
    
    /* If non-leaf, move corresponding entries from block number table. */
    if (level > 0) {
	memcpy(newpage + ISPAGESIZE - newpage_nkeys * BLKNOSIZE,
	       fullpage + ISPAGESIZE - nkeys * BLKNOSIZE,
	       newpage_nkeys * BLKNOSIZE);
    }
    
    stshort((short) fullpage_nkeys, fullpage + BT_NKEYS_OFF);
    stshort((short) newpage_nkeys, newpage + BT_NKEYS_OFF);
}
