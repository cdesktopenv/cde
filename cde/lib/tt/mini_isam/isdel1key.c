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
/*%%  $XConsortium: isdel1key.c /main/3 1995/10/23 11:37:24 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isdel1key.c
 *
 * Description: 
 *	Delete a key entry from index
 *	
 *
 */

#include "isam_impl.h"

void
_del1key(Fcb *fcb, Keydesc2 *pkeydesc2, char *record, Recno recnum)
{
    char		keybuf[MAXKEYSIZE];
    Btree		*btree;
    char		*pkey;

    /*
     * Special case when there is no primary key.
     */
    if (pkeydesc2->k2_nparts == 0)
	return;

    memset((char *)keybuf, 0,pkeydesc2->k2_len);
    _iskey_extract(pkeydesc2,record,keybuf);
    strecno(recnum, keybuf);

    btree = _isbtree_create(fcb, pkeydesc2);
    _isbtree_search(btree, keybuf);

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
}
