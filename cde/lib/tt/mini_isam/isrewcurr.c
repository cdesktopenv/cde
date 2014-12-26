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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isrewcurr.c /main/3 1995/10/23 11:44:15 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isrewcurr.c 1.8 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isrewcurr.c
 *
 * Description:
 *	Rewrite current record in ISAM file. 
 */


#include "isam_impl.h"
#include <sys/time.h>

static int _amrewcurr(), _changekeys2();

/*
 * err = isrewcurr(isfd, record)
 *
 * Isrewcurr() modifies the current record in ISAM file. 
 * All indexes of the ISAM file are updated.
 *
 * Current record position is changed in relation to the new key value.
 * isrecnum is set to the changed record.
 *
 * If the ISAM file is for variable length records, the isreclen variable
 * must be set to indicate the actual length of the record, which must
 * be between the minimum and maximum length, as specified in isbuild().
 *
 * Returns 0 if successful, or -1 of any error.
 *
 * Errors:
 *	EDUPL	The write woul result in a duplicate on a key that
 *		does not allow duplicates.
 *	ELOCKED The file has been locked by another process.
 *	ENOTOPEN isfd does not correspond to an open ISAM file, or the
 *		ISAM file was not opened with ISINOUT mode.
 *	ENOCURR	Record with record number recnum does not exist, or it
 *		was deleted by another process.
 */

int 
isrewcurr(isfd, record)
    int			isfd;
    char		*record;
{
    register Fab	*fab;
    int			reclen;
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

    /*
     * Determine record length. Check it against min and max record length.
     */
    reclen = (fab->varlength == TRUE) ? isreclen : fab->minreclen;
    if (reclen < fab->minreclen || reclen > fab->maxreclen) {
	_setiserrno2(EBADARG, '9', '0');
	return (ISERROR);
    }

    if ((ret = _amrewcurr(&fab->isfhandle, record, reclen, &fab->curpos,
			  &recnum, &fab->errcode)) == ISOK) {
	isrecnum = recnum;		     /* Set isrecnum */
    }

    _seterr_errcode(&fab->errcode);

    return (ret);			     /* Successful write */
}

/*
 * _amrewcurr(isfhandle, record, reclen, curpos, recnum, errcode)
 *
 * _amrewcurr() rewrites the current record in ISAM file.
 *
 * Input params:
 *	isfhandle	Handle of ISAM file
 *	record		record
 *	reclen		length of the record
 *	curpos		curent record position
 *
 * Output params:
 *	errcode		error status of the operation
 *	curpos		new current position
 *	recnum		record number fo current record
 *
 */

static int
_amrewcurr(isfhandle, record, reclen, curpos, recnum, errcode)
    Bytearray		*isfhandle;
    char		*record;
    int			reclen;
    Recno		*recnum;
    Bytearray		*curpos;
    struct errcode	*errcode;
{
    Fcb			*fcb;
    Crp			*crp;
    Bytearray		newcurpos;
    int			err;
    char		oldrecord[ISMAXRECLEN];
    int			reclen2;
    int			(*rec_read)();
    int			(*rec_rewrite)();

    _isam_entryhook();

    /*
     * Get FCB corresponding to the isfhandle handle.
     */
    if ((fcb = _openfcb(isfhandle, errcode)) == NULL) {
	_isam_exithook();
	return (ISERROR);
    }

    rec_read = (fcb->varflag?_vlrec_read:_flrec_read);
    rec_rewrite = (fcb->varflag?_vlrec_rewrite:_flrec_rewrite);

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
    if (rec_read(fcb, oldrecord, crp->recno, &reclen2) != ISOK) {
	_amseterrcode(errcode, ENOCURR);
	goto ERROR;
    }

    if (rec_rewrite(fcb, record, crp->recno, reclen) != ISOK) {
	_amseterrcode(errcode, ENOCURR);
	goto ERROR;
    }
	     
    /*
     * Update keys, set new key position.
     */
    newcurpos = _bytearr_dup(curpos);
    if ((err = _changekeys2 (fcb, record, oldrecord, crp->recno, &newcurpos)) 
	!= ISOK) {
	_bytearr_free(&newcurpos);
	_amseterrcode(errcode, err);	
	goto ERROR;
    }

    /* 
     * This takes care of new record position if the physical order is in use.
     */
    *recnum = crp->recno;	
    
    _bytearr_free(curpos);
    crp = NULL;                 /* was aliased to freed curpos->data */
    *curpos = newcurpos;

    _amseterrcode(errcode, ISOK);
    _issignals_mask();
    _isdisk_commit();
    _isdisk_sync();
    _isdisk_inval();

    /*
     * Update control page.
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
    (void)_isfcb_cntlpg_r2(fcb);

    _isam_exithook();
    return (ISERROR);
}

Static int
_changekeys2(fcb, record, oldrecord, recnum, curpos)
    Fcb			*fcb;
    char                *record;
    char                *oldrecord;
    Recno	        recnum;
    Bytearray		*curpos;
{
    int                	nkeys = fcb->nkeys;
    register int        i;
    int			err;
    Crp			*crp;
    int			keyid;
    Keydesc2		*keydesc2;

    crp = (Crp *)curpos->data;
    keyid = crp->keyid;

    for (i = 0; i < nkeys; i++) {
	keydesc2 = fcb->keys + i;
        if ((err =_change1key(fcb, keydesc2, record, oldrecord, recnum,
			      (keydesc2->k2_keyid == keyid) ?
			      crp->key : (char *) NULL)) != ISOK)
	    return (err);
    }

    return (ISOK);
}      
