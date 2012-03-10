/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: iskeyconv.c /main/3 1995/10/23 11:41:54 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)iskeyconv.c 1.4 89/07/17 Copyr 1988 Sun Micro";
#endif

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * iskeyconv.c
 *
 * Description:
 *	Conversion functions between internal and external key descriptor
 *	
 */

#include "isam_impl.h"

/* 
 * _iskey_itox(pikdesc,pxkdesc) 
 *
 * Convert internal key desc. to X/OPEN key descriptor. 
 */

_iskey_itox(pikdesc,pxkdesc)
    register struct keydesc2	*pikdesc;    /* NetISAM internal format */
    register struct keydesc	*pxkdesc;    /* X/OPEN format */
{
    int 		nparts;
    register int 	i;

    memset ((char *)pxkdesc, 0, sizeof (*pxkdesc));

    pxkdesc->k_flags = pikdesc->k2_flags;
    nparts = pxkdesc->k_nparts = pikdesc->k2_nparts;

    for (i = 0; i < nparts;i++) {
	pxkdesc->k_part[i].kp_start = pikdesc->k2_part[i].kp2_start;
	pxkdesc->k_part[i].kp_leng = pikdesc->k2_part[i].kp2_leng;
	pxkdesc->k_part[i].kp_type = pikdesc->k2_part[i].kp2_type;
    }
}


/* 
 * _iskey_xtoi()
 *
 * Convert X/OPEN key descriptor to internal key descriptor.
 *
 */

_iskey_xtoi(pikdesc,pxkdesc)
    register struct keydesc2	*pikdesc;    /* NetISAM internal format */
    register struct keydesc	*pxkdesc;    /* X/OPEN format */
{
    int 		nparts;
    register int 	i;
    int			offset;		     /* Keep track of offset in key */

    memset ((char *)pikdesc, 0, sizeof (*pikdesc));

    pikdesc->k2_flags = pxkdesc->k_flags;
    nparts = pikdesc->k2_nparts = pxkdesc->k_nparts;

    offset = 0;
    /*
     * Every key entry starts with record number.
     */
    offset += RECNOSIZE;

    /*
     * If index allows duplicates, the key is augmented with duplicate
     * serial number.
     */
    if ((pxkdesc->k_flags & DUPSMASK) == ISDUPS)
	offset += DUPIDSIZE;

    for (i = 0; i < nparts; i++) {
	pikdesc->k2_part[i].kp2_start = pxkdesc->k_part[i].kp_start;
	pikdesc->k2_part[i].kp2_leng = pxkdesc->k_part[i].kp_leng;
	pikdesc->k2_part[i].kp2_type = pxkdesc->k_part[i].kp_type;
	pikdesc->k2_part[i].kp2_offset = offset;

	offset += pxkdesc->k_part[i].kp_leng;
    }

    /* Append recno to key descriptors. */
    pikdesc->k2_part[i].kp2_start = 0;	     /* not used */
    pikdesc->k2_part[i].kp2_leng = RECNOSIZE;
    pikdesc->k2_part[i].kp2_type = RECNOTYPE;
    pikdesc->k2_part[i].kp2_offset = KEY_RECNO_OFF; /* at the beginning of key */
    
    if ((pxkdesc->k_flags & DUPSMASK) == ISDUPS) {
	/* Append duplicate serial number to key descriptors. */
	pikdesc->k2_part[i].kp2_start = 0;   /* not used */
	pikdesc->k2_part[i].kp2_leng = DUPIDSIZE;
	pikdesc->k2_part[i].kp2_type = DUPIDTYPE;
	pikdesc->k2_part[i].kp2_offset = KEY_DUPS_OFF; /* after recno field */
    }

    /* Round up to next multiple of 2. */
    offset = (offset+1) & ~1;
    
    pikdesc->k2_len = offset;		     /* Length of the entire key */
}
