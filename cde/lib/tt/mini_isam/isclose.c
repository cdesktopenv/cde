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
/*%%  $XConsortium: isclose.c /main/3 1995/10/23 11:36:50 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isclose.c
 *
 * Description:
 *	Close an ISAM file
 */

#include "isam_impl.h"
#include <sys/time.h>

/*
 * isclose(isfd)
 *
 * Isclose() closes the ISAM file associated with the file descriptor isfd.
 * There may be other ISAM file descriptors in use which are associated 
 * with the same ISAM file; are not effected by isclose().
 *
 * Isclose() returns -1 if an errors was detected, or 0 if the file was closed
 * successfully.
 *
 * Errors:
 *	ENOTOPEN isfd is not ISAM file descriptor of an open ISAM file.
 */

int
isclose(int isfd)
{
    Fab		*fab;
    Fcb         *fcb;

    if ((fab = _isfd_find(isfd)) == NULL) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    _isam_entryhook();

    /*
     * Get FCB corresponding to the isfhandle handle.
     */
    if ((fcb = _openfcb(&fab->isfhandle, &fab->errcode)) == NULL) {
	_isam_exithook();
	return (ISERROR);
    }

    /*
     * Delete FCB and remove it from FCB cache. Close UNIX fds.
     *
     * This is desirable when ISAM files are removed from other processes,
     * or simply by 'rm file.*'. 
     */
    (void) _watchfd_decr(_isfcb_nfds(fcb));
    _isfcb_close(fcb);
    _mngfcb_delete(&fab->isfhandle);

    _isam_exithook();

    _fab_destroy(fab);			     /* Deallocate Fab object */
    _isfd_delete(isfd);
    
    return (ISOK);
}
