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
/*%%  $XConsortium: isindexinfo.c /main/3 1995/10/23 11:40:56 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isindexinfo.c
 *
 * Description:
 *	Access file status information
 */


#include "isam_impl.h"
#include <sys/time.h>

#define ZERO 0

/*
 * err = isindexinfo(isfd, buffer, number)
 *
 * Return information about index.
 * Return information about the data file (if number == 0).
 *
 * (this function is overloaded).
 *
 * Errors:
 *	EBADARG	number is out of range
 *	ENOTOPEN isfd is not ISAM file descriptor of open ISAM file
 */

#define dibuf ((struct dictinfo *)buffer)

int
isindexinfo(int isfd, struct keydesc *buffer, int number)
{
    Fab	*fab;
    int			ret;
    Fcb			*fcb = NULL;

    /*
     * Get File Access Block.
     */
    if ((fab = _isfd_find(isfd)) == NULL) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    /*
     * Call the Access Method or RPC client function, depending whether
     * the file is local or remote.
     */

    _isam_entryhook();

    /*
     * Get FCB corresponding to the isfhandle handle.
     */
    if ((fcb = _openfcb(&fab->isfhandle, &fab->errcode)) == NULL) {
	_isam_exithook();
	return (ISERROR);
    }

    /*
     * Update information in FCB from CNTL page on the disk
     */
    (void)_isfcb_cntlpg_r(fcb);

    /*
     * Validate number argument.
     */
    if (number < 0 || number > fcb->nkeys) {
	_amseterrcode(&fab->errcode, EBADARG);
	goto ERROR;
    }

    if (number == 0) {
	
	/*
	 * Return dictinfo structure.
	 */
	dibuf->di_nkeys = fcb->nkeys;
	dibuf->di_recsize = fcb->maxreclen;
	dibuf->di_idxsize = fcb->keys[0].k2_len;
	dibuf->di_nrecords = fcb->nrecords;

	/* Set msb of di_nkeys for variable length records. */
	if (fcb->varflag == TRUE) 
	    dibuf->di_nkeys |= DICTVARLENBIT;
    }
    else {

	/*
	 * Return index information.
	 */
	_iskey_itox(fcb->keys + number - 1, buffer);
    }

    _amseterrcode(&fab->errcode, ISOK);

/* XXX This fixes a core dump that occurs when isindexinfo is
*      called on brand new tables
    _isdisk_commit();
    _isdisk_sync();
    _isdisk_inval();
*/

    _isam_exithook();
    ret = ISOK;
    goto CLEANUP;

 ERROR:
    /*
     * Restore FCB from CNTL page.
     */

    _isdisk_rollback();
    _isdisk_inval();

    _isam_exithook();
    ret = ISERROR;

 CLEANUP: 

    if (ret == ISOK)
	isreclen = fab->minreclen;	     /* for variable length */

    _seterr_errcode(&fab->errcode);

    /*
     * This is a patch to conform with the VSX 3.0 test that checks
     * that k_leng == 2 and k_type == 1 for index 1 if the ISAM file
     * has no primary key. I suspect that these numbers are returned by
     * C-ISAM and the author of VSX tests diligently checks them even 
     * though they have no meaning.
     */
    if (ret == ISOK && number == 1 && buffer->k_nparts == 0) {
	buffer->k_leng = 2;
	buffer->k_type = INTTYPE;
    }
    
    return (ret);		
}

