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
/*%%  $XConsortium: issync.c /main/3 1995/10/23 11:45:20 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * issync.c
 *
 * Description:
 *	Sync all kernel buffers to the disk.
 *
 * Note: issync() flushes changed kernel buffers that are local to
 * 	the application that issued the call.
 * 
 * See sync(2) UNIX manual for what actually happens if sync() is called.
 */

#include "isam_impl.h"
#include <unistd.h>
#include <sys/file.h>
#include <sys/time.h>

/*
 * int  issync()
 */

int 
issync(void)
{
    return iscntl(ALLISFD, ISCNTL_FSYNC);
}

/*
 * int  isfsync(fd)
 */

int 
isfsync(int isfd)
{
    return iscntl(isfd, ISCNTL_FSYNC);
}


int _issync(void)
{
    int		i;

    for (i = 0; i < MAXISFD; i++)
	(void)_isfsync(i);

    return (ISOK);
}

int _isfsync(int isfd)
{
    Fab	*fab;
    Fcb                 *fcb;
    int			ret;

    /*
     * Get File Access Block.
     */
    if ((fab = _isfd_find(isfd)) == NULL) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    /*
     * Check that the open mode was ISINPUT, or ISINOUT.
     */
    if (fab->openmode != OM_INPUT && fab->openmode != OM_INOUT) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    _isam_entryhook();

    /*
     * Get FCB corresponding to the isfhandle handle.
     */
    if ((fcb = _openfcb(&fab->isfhandle, &fab->errcode)) == NULL) {
	_isam_exithook();
	ret = ISERROR;
    }
    else {

        if (fcb->datfd != -1)
	    (void)fsync(fcb->datfd);

        if (fcb->indfd != -1)
	    (void)fsync(fcb->indfd);

        if (fcb->varfd != -1)
	    (void)fsync(fcb->varfd);

        _amseterrcode(&fab->errcode, ISOK);
        _isam_exithook();
        ret = ISOK;
    }

    _seterr_errcode(&fab->errcode);

    return (ret);			     /* Successful write */
}
