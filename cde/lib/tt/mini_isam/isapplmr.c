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
/*%%  $XConsortium: isapplmr.c /main/3 1995/10/23 11:35:25 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isapplmr.c
 *
 * Description:
 *	Read Application magic string
 */


#include "isam_impl.h"
#include <unistd.h>
#include <sys/file.h>
#include <sys/time.h>
#include "isam_impl.h"

/*
 * string = isapplmr(isfd)
 *
 * Isapplmr() returns the aplication magic string written to the ISAM file
 * by isapplmw() function. No magic string is returned as "". The value
 * of -1 returned indicates an error (iserrno is set).
 *
 */

int 
_isapplmr(int isfd, char *buffer)
{
    Fab	*fab;
    Fcb                 *fcb = NULL;
    char                cntl_page[CP_NKEYS_OFF];

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
	return (ISERROR);
    }

    _isseekpg(fcb->datfd, ISCNTLPGOFF);
    (void)read(fcb->datfd, cntl_page, sizeof(cntl_page));
    strncpy(buffer, cntl_page + CP_APPLMAGIC_OFF, CP_APPLMAGIC_LEN);

    _amseterrcode(&fab->errcode, ISOK);
    _isam_exithook();

    _seterr_errcode(&fab->errcode);

    return (ISOK);			     /* Successful write */
}
