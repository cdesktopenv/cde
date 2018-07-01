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
/*%%  $XConsortium: isopen.c /main/3 1995/10/23 11:42:40 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isopen.c
 *
 * Description:
 *	Open an ISAM file. 
 */

#include "isam_impl.h"
#include <netdb.h>     
#include <sys/file.h>
#include <sys/time.h>

static int _am_open();
/*
 * isfd = isopen(isfname, mode)
 *
 * Isopen() determines on which machine the ISAM file resides, 
 * checks if the file exists, creates a File access block,
 * and initilizes it. It also checks permissions. Returns an ISAM file
 * descriptor (isfd), or a value of -1 if the open failed.
 *
 * Errors:
 *	EBADARG	Improper mode parameter
 *	EBADFILE ISAM file is corrupted or it is not an NetISAM file
 *	EFLOCKED The file is exclusively locked by other process.
 *	EFNAME	Invalid ISAM file name 
 *	EFNAME	ISAM file does not exist
 *	ETOOMANY Too many ISAM file descriptors are in use (128 is the limit)
 *
 * The following error code is "borrowed" from UNIX:
 *	EACCES	UNIX file system protection denies access to the file:
 *	         - mode is INOUT or OUTPUT and ISAM file is on 
 *	           a Read-Only mounted file system
 *		 - UNIX file permissions don't allow access to the file
 */

int 
isopen(char *isfname, int mode)
{
    Fab			*fab;
    Isfd		isfd;
    enum openmode	openmode;

    /*
     * Check if the user is allowed to access the ISAM file.
     * Use UNIX and NFS permissions.
     */

    /* Get file open mode part of the mode parameter. */
    if ((openmode = _getopenmode(mode)) == OM_BADMODE) {
	_setiserrno2(EBADARG, '9', '0');
	return (NOISFD);
    }
    /* Create a Fab object. */
    fab = _fab_new(isfname,
		   openmode,
		   (Bool)((mode & ISLENMODE) == ISVARLEN),
		   0,
		   0);
    if (fab == NULL) {
	return (NOISFD);		     /* iserrno is set by fab_new */
    }

    /* Get an ISAM file descriptor for this fab */
    if ((isfd = _isfd_insert(fab)) == NOISFD) {
	/* Table of ISAM file descriptors would overflow. */
	_fab_destroy(fab);
	_setiserrno2(ETOOMANY, '9', '0');
	return (NOISFD);
    }
    FAB_ISFDSET(fab, isfd);
    if (_am_open(fab)) {
	_seterr_errcode(&fab->errcode);
	_fab_destroy(fab);
	return (NOISFD);
    }

    isreclen = fab->maxreclen;

    return ((int)isfd);			     /* Successful isopen() */
}

Static int _am_open(Fab *fab)
{
    return (_amopen(fab->isfname, fab->openmode, &fab->varlength,
		    &fab->minreclen, &fab->maxreclen, &fab->isfhandle,
		    &fab->curpos, &fab->errcode));
}
