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
/*%%  $XConsortium: isaddprimary.c /main/3 1995/10/23 11:33:35 rswiston $ 			 				 */
/* @(#)isaddprimary.c	1.7 93/09/07
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isaddprimary.c
 *
 * Description:
 *	Add secondary index to ISAM file
 */


#include "isam_impl.h"
#include <sys/time.h>

Static int _am_addprimary();


/*
 * err = isaddprimary(isfd, keydesc)
 *
 * Isaddprimary() is used to add primary index to ISAM file. 
 *
 * This call is not part of the X/OPEN sprec.
 *
 * Errors:
 *	EBADKEY	error in keydesc	
 *	EDUPL	there are duplicate keys and the keydesc does not allow
 *		duplicate keys
 *	EKEXISTS key with the same key descriptor already exists
 *	EKEXISTS the ISAM file already has a primary index.
 *	ENOTEXCL ISAM file is not open in exclusive mode
 *	ENOTOPEN the ISAM file is not open in ISINOUT mode.
 *	EACCES	Cannot create index file because of UNIX error.
 */

int 
isaddprimary(int isfd, struct keydesc *keydesc)
{
    Fab	*fab;
    int			ret;

    /*
     * Get File Access Block.
     */
    if ((fab = _isfd_find(isfd)) == NULL) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    /*
     * Check that the open mode was ISOUTPUT
     */
    if (fab->openmode != OM_INOUT) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    ret = _am_addprimary(fab, keydesc);
    _seterr_errcode(&fab->errcode);

    return (ret);			     /* Successful write */
}

Static int _am_addprimary(Fab *fab, struct keydesc *keydesc)
{
    return (_amaddprimary(&fab->isfhandle, keydesc, &fab->errcode));
}
