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
/*%%  $XConsortium: isminmax.c /main/3 1995/10/23 11:42:16 rswiston $ 			 				 */

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isminmax.c 
 *
 * Description:
 *	NetISAM minimum and maximum values functions
 */

#include "isam_impl.h"

static unsigned char ismaxlongarr[LONGSIZE] = ISMAXLONG;
static unsigned char isminlongarr[LONGSIZE] = ISMINLONG;

static unsigned char ismaxshortarr[SHORTSIZE] = ISMAXSHORT;
static unsigned char isminshortarr[SHORTSIZE] = ISMINSHORT;

static unsigned char ismaxdoublearr[DOUBLESIZE] = ISMAXDOUBLE;
static unsigned char ismindoublearr[DOUBLESIZE] = ISMINDOUBLE;

static unsigned char ismaxfloatarr[FLOATSIZE] = ISMAXFLOAT;
static unsigned char isminfloatarr[FLOATSIZE] = ISMINFLOAT;

/* These two are used globally. */
long *ismaxlong = (long *)ismaxlongarr;
long *isminlong = (long *)isminlongarr;

static short *ismaxshort = (short *)ismaxshortarr;
static short *isminshort = (short *)isminshortarr;

static double *ismaxdouble = (double *)ismaxdoublearr;
static double *ismindouble = (double *)ismindoublearr;

static float *ismaxfloat = (float *)ismaxfloatarr;
static float *isminfloat = (float *)isminfloatarr;


/* 
 * _iskey_fillmax() 
 *
 * Fill key buffer with maximum values 
 */

void
_iskey_fillmax(struct keydesc2 *pkeydesc2, char *keybuf)
{
    int 	i;
    struct keypart2 *ppart;
    int 		nparts;

    nparts = pkeydesc2->k2_nparts;
    ppart = pkeydesc2->k2_part;

    for (i = 0; i < nparts + 1;i++) {	     /* +1 is for recnum part */
	switch (ppart->kp2_type) {
	case CHARTYPE:
	    (void) memset (keybuf + ppart->kp2_offset, ISMAXCHAR, 
			   ppart->kp2_leng);
	    break;
	case BINTYPE:
	    (void) memset (keybuf + ppart->kp2_offset, ISMAXBIN, 
			   ppart->kp2_leng);
	    break;
	case LONGTYPE:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)ismaxlong, LONGSIZE);
	    break;
	case SHORTTYPE:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)ismaxshort, SHORTSIZE);
	    break;
	case FLOATTYPE:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)ismaxfloat, FLOATSIZE);
	    break;
	case DOUBLETYPE:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)ismaxdouble, DOUBLESIZE);
	    break;

	case CHARTYPE + ISDESC:
	    (void) memset (keybuf + ppart->kp2_offset, ISMINCHAR, 
			   ppart->kp2_leng);
	    break;
	case BINTYPE + ISDESC:
	    (void) memset (keybuf + ppart->kp2_offset, ISMINBIN, 
			   ppart->kp2_leng);
	    break;
	case LONGTYPE + ISDESC:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)isminlong, LONGSIZE);
	    break;
	case SHORTTYPE + ISDESC:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)isminshort, SHORTSIZE);
	    break;
	case FLOATTYPE + ISDESC:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)isminfloat, FLOATSIZE);
	    break;
	case DOUBLETYPE + ISDESC:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)ismindouble, DOUBLESIZE);
	    break;
	default:
	    _isfatal_error("_iskey_fillmax");
	}
	ppart++;
    }
}

/* 
 * _iskey_fillmin() 
 *
 * Fill key buffer with minimum values 
 */

void
_iskey_fillmin(struct keydesc2 *pkeydesc2, char *keybuf)
{
    int 	i;
    struct keypart2 *ppart;
    int 		nparts;

    nparts = pkeydesc2->k2_nparts;
    ppart = pkeydesc2->k2_part;

    for (i = 0; i < nparts + 1;i++) {	     /* +1 is for recnum part */
	switch (ppart->kp2_type) {
	case CHARTYPE:
	    (void) memset (keybuf + ppart->kp2_offset, ISMINCHAR, 
			   ppart->kp2_leng);
	    break;
	case BINTYPE:
	    (void) memset (keybuf + ppart->kp2_offset, ISMINBIN, 
			   ppart->kp2_leng);
	    break;
	case LONGTYPE:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)isminlong, LONGSIZE);
	    break;
	case SHORTTYPE:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)isminshort, SHORTSIZE);
	    break;
	case FLOATTYPE:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)isminfloat, FLOATSIZE);
	    break;
	case DOUBLETYPE:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)ismindouble, DOUBLESIZE);
	    break;

	case CHARTYPE + ISDESC:
	    (void) memset (keybuf + ppart->kp2_offset, ISMAXCHAR, 
			   ppart->kp2_leng);
	    break;
	case BINTYPE + ISDESC:
	    (void) memset (keybuf + ppart->kp2_offset, ISMAXBIN, 
			   ppart->kp2_leng);
	    break;
	case LONGTYPE + ISDESC:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)ismaxlong, LONGSIZE);
	    break;
	case SHORTTYPE + ISDESC:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)ismaxshort, SHORTSIZE);
	    break;
	case FLOATTYPE + ISDESC:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)ismaxfloat, FLOATSIZE);
	    break;
	case DOUBLETYPE + ISDESC:
	    memcpy ( keybuf + ppart->kp2_offset,(char *)ismaxdouble, DOUBLESIZE);
	    break;
	default:
	    _isfatal_error("_iskey_fillmin");
	}
	ppart++;
    }
}
