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
/*%%  $XConsortium: isfcbindex.c /main/3 1995/10/23 11:38:43 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isfcbindex.c
 *
 * Description: 
 *	Functions that deal with the key descriptors in FCB
 *	
 *
 */

#include "isam_impl.h"

Static int _keypart2cmp();
/*
 * _isfcb_primkeyadd(fcb, keydesc2)
 *
 * Add priamry key descriptor to FCB.
 *
 */

int
_isfcb_primkeyadd(Fcb *fcb, Keydesc2 *keydesc2)
{
    /*
     * Assign keyid.
     */
    keydesc2->k2_keyid = ++fcb->lastkeyid;

    fcb->keys[0] = *keydesc2;

    return (ISOK);
}

/*
 * _isfcb_primkeyel(fcb)
 *
 * Delete primary key
 *
 */

int
_isfcb_primkeydel(Fcb *fcb)
{
    if (FCB_NOPRIMARY_KEY(fcb))
	return (EBADARG);

    memset((char *)&fcb->keys[0], 0, sizeof(fcb->keys[0]));

    return (ISOK);
}

/*
 * _isfcb_altkeyadd(fcb, keydesc2)
 *
 * Add alternate key descriptor to FCB.
 *
 */

int
_isfcb_altkeyadd(Fcb *fcb, Keydesc2 *keydesc2)
{
    assert (fcb->nkeys < MAXNKEYS); 

    /*
     * Assign keyid.
     */
    keydesc2->k2_keyid = ++fcb->lastkeyid;

    /*
     * Reallocate fcb->keys table.
     */
    fcb->keys = (Keydesc2 *) 
	_isrealloc((char *)fcb->keys, 
		   (unsigned) (sizeof(Keydesc2) * (fcb->nkeys + 1)));

    fcb->keys[fcb->nkeys] = *keydesc2;

    fcb->nkeys++;
    return (ISOK);
}


/*
 * pkeydesc2 = _isfcb_findkey(fcb, keydesc2)
 *
 * Find key descriptor.
 *
 */

Keydesc2 *
_isfcb_findkey(Fcb *fcb, Keydesc2 *keydesc2)
{
    int                 nkeys = fcb->nkeys;
    Keydesc2	*kp2;
    int	j, i;
    int			nparts;

    for (i = 0; i < nkeys; i++) {
	kp2 = fcb->keys + i;

	if (keydesc2->k2_nparts == kp2->k2_nparts) {
	    
	    nparts = keydesc2->k2_nparts;
	    for (j = 0; j < nparts; j++) {
		if (_keypart2cmp(keydesc2->k2_part + j, kp2->k2_part + j) != 0)
		    break;
	    }	
	    
	    if (j == nparts)
		return (kp2);
	}
    }  
    
    return ((struct keydesc2 *) 0);	     /* Key descriptor not found */
}

/*
 * pkeydesc2 = _isfcb_altkeydel(fcb, keydesc2)
 *
 * Delete key descriptor from FCB.
 *
 */

int
_isfcb_altkeydel(Fcb *fcb, Keydesc2 *keydesc2)
{
    int                 nkeys = fcb->nkeys;
    int        i, j;
    Keydesc2	*kp2;
    int			nparts;

    for (i = 0; i < nkeys; i++) {
	kp2 = fcb->keys + i;

	if (keydesc2->k2_nparts == kp2->k2_nparts) {
	    
	    nparts = keydesc2->k2_nparts;
	    for (j = 0; j < nparts; j++) {
		if (_keypart2cmp(keydesc2->k2_part + j, kp2->k2_part + j) != 0)
		    break;
	    }	
	    
	    if (j == nparts)
		break;			     /* Key found */
	}
    }  

    if (i >= nkeys)
	return (EBADKEY);		     /* Key descriptor not found */

    if (i == 0) {
	return (EPRIMKEY);		     /* Cannot delete primary key */
    }
    
    /*
     * Shift the end of the table toward the beginning to delete the entry.
     */
    if (i < nkeys - 1) {
	memcpy( (char *)(fcb->keys + i),(char *)(fcb->keys + i + 1), 
	      (nkeys - 1 - i) * sizeof (fcb->keys[0]));
    }

    fcb->nkeys--;

    return (ISOK);
}

/* compare key parts */
Static int
_keypart2cmp(struct keypart2 *l, struct keypart2 *r)
{
    return !(l->kp2_type == r->kp2_type && l->kp2_start == r->kp2_start &&
	     l->kp2_leng == r->kp2_leng);
}

/*
 * pkeydesc2 = _isfcb_indfindkey(fcb, keyind)
 *
 * Find key descriptor by its keyind value.
 *
 */

Keydesc2 *
_isfcb_indfindkey(Fcb *fcb, int keyid)
{
    int                 nkeys = fcb->nkeys;
    Keydesc2	*keys = fcb->keys;
    int	i;
    
    for (i = 0; i < nkeys; i++) {
	if (keys[i].k2_keyid == keyid)
	    break;
    }  
    
    return ((i == nkeys) ? NULL : keys + i);
}
