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
/*%%  $XConsortium: isamerror.c /main/3 1995/10/23 11:34:39 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isamerror.c
 *
 * Description: 
 *	Error handling functions for the Access Method layer.
 *	
 *
 */

#include "isam_impl.h"

#define ESENTINEL	-100		     /* Must not be a valid errno */

static struct errstattab {
    int		iserrno;
    char	isstat1;
    char	isstat2;
} errstattab[] = {
    { ISOK,	'0', '0'},
    { ENOREC,	'2', '3'},
    { ENOCURR,	'2', '1'},
    { EENDFILE,	'1', '0'},
    { EDUPL, 	'2', '2'},
    { ENOTOPEN, '9', '0'},
    { EBADARG,	'9', '0'},
    { EBADKEY,	'9', '0'},
    { ETOOMANY, '9', '0'},
    { EBADFILE, '9', '0'},
    { ENOTEXCL, '9', '0'},
    { ELOCKED, 	'9', '0'},
    { EKEXISTS, '9', '0'},
    { EPRIMKEY, '9', '0'},
    { EFLOCKED, '9', '0'},
    { EFNAME, 	'9', '0'},
    { EFATAL, 	'9', '0'},

    /* Unix errors */

    { ENOENT,	'9', '0'},
    { EACCES, 	'9', '0'},
    { EEXIST, 	'9', '0'},
    { ENOSPC, 	'9', '0'},

    /* search sentinel */
    { ESENTINEL, '9', '0'}
};

/*
 * _amseterrcode(errcode, is_errno, is_stat1, is_stat2, is_stat3, is_stat4)
 *
 * Set errcode block with ISAM error codes.
 */

void
_amseterrcode(struct errcode *errcode, int is_errno)
{
    struct errstattab 	*p;
    errcode->iserrno = is_errno;

    /*
     * Search errstattab to get appropriate isstat[12] values.
     */

    for (p = errstattab; p->iserrno != ESENTINEL; p++) {
	if (p->iserrno == is_errno) {
	    errcode->isstat[0] = p->isstat1;
	    errcode->isstat[1] = p->isstat2;

	    break;
	}
    }

    if (is_errno == 0 && isdupl == 1)
	errcode->isstat[1] = '2';	     /* Indicate allowed duplicates */
}

