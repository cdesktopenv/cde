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
/*%%  $XConsortium: isdelcurr.c /main/3 1995/10/23 11:37:32 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isdelcurr.c
 *
 * Description:
 *	Delete current record from ISAM file. 
 */


#include "isam_impl.h"
#include <sys/time.h>

static int _amdelcurr();

/*
 * err = isdelcurr(isfd, record)
 *
 * Isdelcurr() modifies the current record in ISAM file. 
 * All indexes of the ISAM file are updated.
 *
 * Current record position is not changed.
 * isrecnum is set to to the deleted record.
 *
 * Returns 0 if successful, or -1 of any error.
 *
 * Errors:
 *	ELOCKED The record or file has been locked by another process.
 *	ENOTOPEN isfd does not correspond to an open ISAM file, or the
 *		ISAM file was not opened with ISINOUT mode.
 *	ENOCURR	Record with record number recnum does not exist, or it
 *		was deleted by another process.
 */

int 
isdelcurr(int isfd)
{
    int			_am_delcurr();
    Fab	*fab;
    int			ret;
    int			recnum;

    /*
     * Get File Access Block.
     */
    if ((fab = _isfd_find(isfd)) == NULL) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    /*
     * Check that the open mode was  ISINOUT.
     */
    if (fab->openmode != OM_INOUT) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    if ((ret = _amdelcurr(&fab->isfhandle, &fab->curpos,
			  &recnum, &fab->errcode)) == ISOK) {
	isrecnum = recnum;		     /* Set isrecnum */
    }

    _seterr_errcode(&fab->errcode);

    return (ret);			     /* Successful write */
}

/*
 * _amdelcurr(isfhandle, curpos, recnum, errcode)
 *
 * _amdelcurr() deletes a record from ISAM file.
 *
 * Input params:
 *	isfhandle	Handle of ISAM file
 *	curpos		Curent record position
 *
 * Output params:
 *	recnum		record number of the deleted record
 *	errcode		error status of the operation
 *
 */

static int
_amdelcurr(Bytearray *isfhandle, Bytearray *curpos, Recno *recnum,
           struct errcode *errcode)
{
    Fcb			*fcb = NULL;
    Crp			*crp;
    char		recbuf[ISMAXRECLEN];
    int			reclen;
    int			(*rec_read)();
    int			(*rec_delete)();

    _isam_entryhook();

    /*
     * Get FCB corresponding to the isfhandle handle.
     */
    if ((fcb = _openfcb(isfhandle, errcode)) == NULL) {
	_isam_exithook();
	return (ISERROR);
    }

    rec_read = (fcb->varflag?_vlrec_read:_flrec_read);
    rec_delete = (fcb->varflag?_vlrec_delete:_flrec_delete);

    /*
     * Get info from current record position structure.
     */
    crp = (Crp *) curpos->data;

    if (crp->flag != CRP_ON) {
	_amseterrcode(errcode, ENOCURR);
	goto ERROR;
    }

    /*
     * Update information in FCB from CNTL page on the disk
     */
    (void)_isfcb_cntlpg_r2(fcb);

    /*
     * We must read the record first to be able to delete keys.
     */
    if (rec_read(fcb, recbuf, crp->recno, &reclen) != ISOK) {
	_amseterrcode(errcode, ENOCURR);
	goto ERROR;
    }

    if (rec_delete(fcb, crp->recno) != ISOK) {
	_amseterrcode(errcode, ENOCURR);
	goto ERROR;
    }

    *recnum = crp->recno;

    fcb->nrecords--;

    /*
     * Delete associated entries from all indexes.
     */
    _delkeys(fcb, recbuf, crp->recno);

    _amseterrcode(errcode, ISOK);

    _issignals_mask();
    _isdisk_commit();
    _isdisk_sync();
    _isdisk_inval();

    /*
     * Update CNTL Page from the FCB.
     */
    (void)_isfcb_cntlpg_w2(fcb);
    _issignals_unmask();

    _isam_exithook();
    return (ISOK);

 ERROR:
    _isdisk_rollback();
    _isdisk_inval();

    /*
     * Restore FCB from CNTL page.
     */
    if (fcb) (void)_isfcb_cntlpg_r2(fcb);

    _isam_exithook();
    return (ISERROR);
}




