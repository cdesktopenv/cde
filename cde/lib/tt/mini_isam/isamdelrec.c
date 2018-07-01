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
/*%%  $XConsortium: isamdelrec.c /main/3 1995/10/23 11:34:30 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isamdelrec.c
 *
 * Description: _amdelrec()
 *	Delete record from ISAM file.
 *	
 *
 */

#include "isam_impl.h"

void _delkeys();

/*
 * _amdelrec(isfhandle, recnum, errcode)
 *
 * _amdelrec() deletes a record from ISAM file.
 *
 * Input params:
 *	isfhandle	Handle of ISAM file
 *	recnum		record number of the record to be deleted
 *
 * Output params:
 *	errcode		error status of the operation
 *
 */

int
_amdelrec(Bytearray *isfhandle, Recno recnum, struct errcode *errcode)
{
    Fcb			*fcb = NULL;
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
     * Update information in FCB from CNTL page on the disk
     */
    (void)_isfcb_cntlpg_r2(fcb);

    /*
     * We must read the record first to be able to delete keys.
     */
    if (rec_read(fcb, recbuf, recnum, &reclen) != ISOK) {
	_amseterrcode(errcode, ENOREC);
	goto ERROR;
    }

    if (rec_delete(fcb, recnum) != ISOK) {
	_amseterrcode(errcode, ENOREC);
	goto ERROR;
    }

    fcb->nrecords--;

    /*
     * Delete associated entries from all indexes.
     */
    _delkeys(fcb, recbuf, recnum);

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

/*
 * _isdelkeys()
 *
 * Delete key entry from all indexes.
 */

void
_delkeys(Fcb *fcb, char *record, Recno recnum)
{
    int                         nkeys = fcb->nkeys;
    int                i;

    for (i = 0; i < nkeys; i++) {
        _del1key(fcb, fcb->keys + i, record, recnum);
    }
}      

