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
/*%%  $XConsortium: isadd1key.c /main/3 1995/10/23 11:33:17 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isadd1key.c
 *
 * Description: 
 *	Add an entry to B tree.
 *	
 *
 */

#include "isam_impl.h"

extern long *ismaxlong;

int
_add1key(Fcb *fcb, Keydesc2 *pkeydesc2, char *record, Recno recnum, char *newkey)
{
    char		keybuf[MAXKEYSIZE];
    Btree		*btree;
    char		*pkey;
    
    /*
     * Special case when there is no primary key.
     */
    if (pkeydesc2->k2_nparts == 0)
	return (ISOK);
    
    memset((char *)keybuf, 0, pkeydesc2->k2_len);
    _iskey_extract(pkeydesc2, record, keybuf);
    
    btree = _isbtree_create(fcb, pkeydesc2);
    
    if (ALLOWS_DUPS2(pkeydesc2)) {
	
	/* 
	 * Duplicates allowed on this key. 
	 * Try to read the last duplicate (with the highest serial number).
	 * If such duplicate exists, the inserted key entry will be
	 * assigned next higher value for duplicate serial number.
	 * If there is no such duplicate, assign duplicate serial number 1.
	 */
	strecno(recnum, keybuf + KEY_RECNO_OFF);
	memcpy(keybuf + KEY_DUPS_OFF, (char *)ismaxlong, LONGSIZE);
	_isbtree_search(btree, keybuf);
	
	if ((pkey = _isbtree_current(btree)) != NULL &&
	    memcmp(pkey + RECNOSIZE + DUPIDSIZE, 
		   keybuf + RECNOSIZE + DUPIDSIZE,
		   pkeydesc2->k2_len - RECNOSIZE - DUPIDSIZE) == 0) {
	    
	    /*
	     * A duplicate exists. Assign +1 value for the new serial
	     * number.
	     */
	    stlong(ldlong(pkey + KEY_DUPS_OFF) + 1, keybuf + KEY_DUPS_OFF);

	    /*
	     * Set isdupl to 1 to indicate allowed duplicates exist.
	     */
	    isdupl = 1;
	}
	else {
	    
	    /*
	     * This is the first duplicate. Assign serial number 1.
	     */
	    stlong(1L, keybuf + KEY_DUPS_OFF);
	}
        
	_isbtree_insert(btree, keybuf);
	_isbtree_destroy(btree);
    }
    else {
	
	/* Duplicates not allowed on this key.
	 * Set TID part to maximum value, causing the search path 
	 * to point just past the possible duplicate in the key file 
	 * If there is no duplicate, this is the correct spot to 
	 * insert the new key entry.                              
	 */
	
	memcpy( keybuf + KEY_RECNO_OFF,(char *)ismaxlong, LONGSIZE);
	_isbtree_search (btree, keybuf);
	
	if ((pkey = _isbtree_current(btree)) != NULL &&
	    memcmp(pkey + RECNOSIZE, keybuf + RECNOSIZE,
		   pkeydesc2->k2_len - RECNOSIZE) == 0) {
	    _isbtree_destroy(btree);
	    return (EDUPL);
	}
	
	strecno(recnum, keybuf + KEY_RECNO_OFF);
	_isbtree_insert(btree, keybuf);
	_isbtree_destroy(btree);
    }

    /*
     * Return new key position.
     */
    if (newkey != NULL)
	memcpy( newkey,keybuf, pkeydesc2->k2_len);

    return (ISOK);
}
