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
/*%%  $XConsortium: ischange1key.c /main/3 1995/10/23 11:36:41 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * ischange1key.c
 *
 * Description: 
 *	Update an index if necessary.
 *	
 *
 */

#include "isam_impl.h"

extern long *ismaxlong;

int
_change1key(Fcb *fcb, Keydesc2 *pkeydesc2, char *record,
            char *oldrecord, Recno recnum, char *newkey)
{
    char		keybuf1[MAXKEYSIZE];
    char		keybuf2[MAXKEYSIZE];
    Btree		*btree;
    char		*pkey;

    /*
     * Special case when there is no primary key.
     */
    if (pkeydesc2->k2_nparts == 0)
	return (ISOK);

    /* Get old key value. */
    memset((char *)keybuf1, 0, pkeydesc2->k2_len);
    _iskey_extract(pkeydesc2, oldrecord, keybuf1);
    strecno(recnum, keybuf1);
    
    /* Get new key value. */
    memset((char *)keybuf2, 0, pkeydesc2->k2_len);
    _iskey_extract(pkeydesc2, record, keybuf2);
    strecno(recnum, keybuf2);

  
    /*
     * See if the key changed.
     */
    if (memcmp(keybuf1, keybuf2, pkeydesc2->k2_len)) {
	
	/*
	 * Delete the old key entry from B tree.
	 */
	btree = _isbtree_create(fcb, pkeydesc2);
	_isbtree_search(btree, keybuf1);

	if (ALLOWS_DUPS2(pkeydesc2)) {
	    
	    /*
	     * We must now scan all the duplicates till we found one with
	     * matching recno. We are positioned just before the first duplicate.
	     * Remember that the duplicates are ordered by their serial number.
	     *
	     * If too many duplicate entries exists, the performance will be
	     * poor.
	     */
	    while ((pkey = _isbtree_next(btree)) != NULL) {
		if (ldrecno(pkey + KEY_RECNO_OFF) == recnum)
		    break;			     /* We found the entry */
	    }
	    
	    if (pkey == NULL) 
		_isfatal_error("_del1key() cannot find entry in B tree");
	    
	    _isbtree_remove(btree);
	}
	else {
	    
	    if ((pkey = _isbtree_current(btree)) == NULL ||
		ldrecno(pkey + KEY_RECNO_OFF) != recnum)
		_isfatal_error("_del1key() cannot find entry in B tree");
	    
	    _isbtree_remove(btree);
	}
	_isbtree_destroy(btree);

	/*
	 * Insert new key entry into B tree.
	 */
	
	btree = _isbtree_create(fcb, pkeydesc2);
	
	if (ALLOWS_DUPS2(pkeydesc2)) {
	    
	    /* 
	     * Duplicates allowed on this key. 
	     * Try to read the last duplicate (with the highest serial number).
	     * If such duplicate exists, the inserted key entry will be
	     * assigned next higher value for duplicate serial number.
	     * If there is no such duplicate, assign duplicate serial number 1.
	     */
	    strecno(recnum, keybuf2 + KEY_RECNO_OFF);
	    memcpy( keybuf2 + KEY_DUPS_OFF,(char *)ismaxlong, LONGSIZE);
	    _isbtree_search(btree, keybuf2);
	    
	    if ((pkey = _isbtree_current(btree)) != NULL &&
		memcmp(pkey + RECNOSIZE + DUPIDSIZE, 
		       keybuf2 + RECNOSIZE + DUPIDSIZE,
		       pkeydesc2->k2_len - RECNOSIZE - DUPIDSIZE) == 0) {
		
		/*
		 * A duplicate exists. Assign +1 value for the new serial
		 * number.
		 */
		stlong(ldlong(pkey + KEY_DUPS_OFF) + 1, keybuf2 + KEY_DUPS_OFF);

		/*
		 * Indicate that there are allowed duplicate key values.
		 */
		isdupl = 1;
	    }
	    else {
		
		/*
		 * This is the first duplicate. Assign serial number 1.
		 */
		stlong(1L, keybuf2 + KEY_DUPS_OFF);
	    }
	    
	    _isbtree_insert(btree, keybuf2);
	    _isbtree_destroy(btree);
	}

	
	
	else {
	    
	    /* Duplicates not allowed on this key.
	     * Set TID part to maximum value, causing the search path 
	     * to point just past the possible duplicate in the key file 
	     * If there is no duplicate, this is the correct spot to 
	     * insert the new key entry.                              
	     */
	    
	    memcpy( keybuf2 + KEY_RECNO_OFF,(char *)ismaxlong, LONGSIZE);
	    _isbtree_search (btree, keybuf2);
	    
	    if ((pkey = _isbtree_current(btree)) != NULL &&
		memcmp(pkey + RECNOSIZE, keybuf2 + RECNOSIZE,
		       pkeydesc2->k2_len - RECNOSIZE) == 0) {
		_isbtree_destroy(btree);
		return (EDUPL);
	    }
	    
	    strecno(recnum, keybuf2 + KEY_RECNO_OFF);
	    _isbtree_insert(btree, keybuf2);
	    _isbtree_destroy(btree);
	}
    }

    /*
     * Return new key position.
     */
    if (newkey != NULL)
	memcpy( newkey,keybuf2, pkeydesc2->k2_len);
	
    return (ISOK);
}
    
