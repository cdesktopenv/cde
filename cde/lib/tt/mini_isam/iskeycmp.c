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
/*%%  $XConsortium: iskeycmp.c /main/3 1995/10/23 11:41:41 rswiston $ 			 				 */

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * iskeycmp.c
 *
 * Description:
 *	ISAM index comparison functions
 */

#include "isam_impl.h"

static struct keypart2 *_curtab;		     /* Current comparison */
					     /* descriptor table */
static int _ncurtab;			     /* Number of entries */

/*
 * _iskeycmp_set()
 *
 * Set key decriptor and number of parts for subsequent key comparison.s
 * nparts, Use only so many parts
 */

void
_iskeycmp_set (Keydesc2 *pkeydesc2, int nparts)
{
    _ncurtab = nparts;
    _curtab = pkeydesc2->k2_part;
    assert(_ncurtab <= pkeydesc2->k2_nparts + 1); /* + 1 for recno */
}

/*
 * Return number that is > 0 if l > r,
 *			 = 0 if l = r,
 *			 < 0 if l < r.
 */

int
_iskeycmp(char *lkey, char *rkey)
{
    int		   	     i, ret;
    struct keypart2 *p;
      char   	     *l, *r;
    long	     llong, rlong;
    double		     ldouble, rdouble;

    ret = 0;
    for (i = 0, p = _curtab; ret == 0 && i < _ncurtab;i++, p++) {
	
	l = lkey + p->kp2_offset;
	r = rkey + p->kp2_offset;

	switch (p->kp2_type) {
	case CHARTYPE:
	case BINTYPE:
	    ret = memcmp(l, r, p->kp2_leng);
	    break;

	case LONGTYPE:
	    llong = ldlong(l);
	    rlong = ldlong(r);

	    if (llong > rlong)
		ret = 1;
	    else if (llong < rlong)
		ret = -1;
	    break;

	case SHORTTYPE:
	    llong = (long)ldshort(l);
	    rlong = (long)ldshort(r);

	    if (llong > rlong)
		ret = 1;
	    else if (llong < rlong)
		ret = -1;
	    break;

	case DOUBLETYPE:
	    ldouble = lddbl(l);
	    rdouble = lddbl(r);

	    if (ldouble > rdouble)
		ret = 1;
	    else if (ldouble < rdouble)
		ret = -1;
	    break;

	case FLOATTYPE:
	    ldouble = (double)ldfloat(l);
	    rdouble = (double)ldfloat(r);

	    if (ldouble > rdouble)
		ret = 1;
	    else if (ldouble < rdouble)
		ret = -1;
	    break;

	case CHARTYPE + ISDESC:
	case BINTYPE + ISDESC:
	    ret = memcmp(r, l, p->kp2_leng);
	    break;

	case LONGTYPE + ISDESC:
	    llong = ldlong(l);
	    rlong = ldlong(r);

	    if (llong > rlong)
		ret = -1;
	    else if (llong < rlong)
		ret = 1;
	    break;

	case SHORTTYPE + ISDESC:
	    llong = (long)ldshort(l);
	    rlong = (long)ldshort(r);

	    if (llong > rlong)
		ret = -1;
	    else if (llong < rlong)
		ret = 1;
	    break;

	case DOUBLETYPE + ISDESC:
	    ldouble = lddbl(l);
	    rdouble = lddbl(r);

	    if (ldouble > rdouble)
		ret = -1;
	    else if (ldouble < rdouble)
		ret = 1;
	    break;

	case FLOATTYPE + ISDESC:
	    ldouble = (double)ldfloat(l);
	    rdouble = (double)ldfloat(r);

	    if (ldouble > rdouble)
		ret = -1;
	    else if (ldouble < rdouble)
		ret = 1;
	    break;

	default:
	    _isfatal_error("Bad data conversion descriptor");
	    break;
	}
    }
    return (ret);
}
