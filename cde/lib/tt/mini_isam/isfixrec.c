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
/*%%  $XConsortium: isfixrec.c /main/3 1995/10/23 11:39:42 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isfixrec.c
 *
 * Description:
 *	Fixed length record access (FLRA) module.
 */

#include "isam_impl.h"

/* Local functions */
long _fl_getpos();			     /* Get offset in .rec file */
int  _fl_deleted();			     /* 0/1 returns 1 if record is deleted */
static void remove_from_chain();	     /* used by _flrec_wrrec() */

/*
 * _flrec_write(fcb, record, recnum, reclen)
 *
 * Write a record.
 *
 * Input params:
 *	FCB	File Control Block
 *	record	record buffer
 *	reclen	record length (NOT USED)
 *
 * Output params:
 *	recnum	record number of the new record
 *
 * Returns 0 if record was written successfully, or -1 if any error.
 */

/*ARGSUSED*/
int
_flrec_write(Fcb *fcb, char *record, Recno *recnum, int reclen)
{
    Recno		recnum2;
    long		rec_position;
    char		delflag = FL_RECEXISTS;
    char		recnobuf [RECNOSIZE];

    /*
     * Reuse a deleted record if one exits.
     * Otherwise, extend .rec file by a record.
     */
    if (fcb->freerecno != NULL_RECNO) {
	recnum2 = fcb->freerecno;

	/*
	 * Remove record from the chain of deleted records.
	 */
	rec_position = _fl_getpos(fcb, recnum2); /* Offset in .rec file */
	_cp_fromfile(fcb, fcb->datfd, recnobuf, rec_position + 1, RECNOSIZE);
	fcb->freerecno = ldrecno(recnobuf);
    }
    else {
	recnum2 = ++(fcb->lastrecno);

	/* 
	 * Extend .rec file size if necessary.
	 */
	while (_fl_getpos(fcb, recnum2 + 1) > fcb->datsize * ISPAGESIZE) {
	    fcb->datsize = _extend_file(fcb, fcb->datfd, fcb->datsize);
	}
	rec_position = _fl_getpos(fcb, recnum2); /* Offset in .rec file */
    }


    /*
     * Copy record to the .at file. Mark record as undeleted.
     */
    _cp_tofile(fcb, fcb->datfd, &delflag, rec_position, 1); 
    _cp_tofile(fcb, fcb->datfd, record, rec_position + 1, fcb->minreclen); 

    *recnum = recnum2;

    return (ISOK);
}

/*
 * _flrec_read(fcb, record, recnum, reclen)
 *
 * Read a record.
 *
 * Input params:
 *	FCB	File Control Block
 *	recnum	record number of the record
 *	reclen  filled with the record size for compatibilty with
 *		variable length records
 *
 * Output params:
 *	record	record buffer is filled with data
 *
 * Returns 0 if record was read successfully, or error code if any error.
 */

int
_flrec_read(Fcb *fcb, char *record, Recno recnum, int *reclen)
{
    long		rec_position;
    char		delflag;

    /*
     * Check that recnum is within the range of existing record numbers.
     */
    if (recnum < 1 || recnum > fcb->lastrecno)
	return (EENDFILE);

    rec_position = _fl_getpos(fcb, recnum); /* Offset in .rec file */

    /*
     * Check that the record is not marked as deleted.
     */
    _cp_fromfile(fcb, fcb->datfd, &delflag, rec_position, 1); 
    if (delflag == FL_RECDELETED) {
	return (ENOREC);
    }

    /*
     * Copy record from the .at file. 
     */
    _cp_fromfile(fcb, fcb->datfd, record, rec_position + 1, fcb->minreclen); 

    *reclen = fcb->minreclen;

    return (ISOK);
}

/*
 * pos = _fl_getpos(fcb, recnum)
 *
 * Calculate the position of record in .rec file.
 */

long
_fl_getpos(Fcb *fcb, Recno recnum)
{
    return ((long)(ISCNTLSIZE + (fcb->minreclen + 1) * (recnum -1)));
}

/*
 * _flrec_rewrite(fcb, record, recnum, reclen)
 *
 * Rewrite a record.
 *
 * Input params:
 *	FCB	File Control Block
 *	recnum	record number of the record
 *	record	new record
 *	int	reclen (NOT USED)
 *
 * Returns 0 if record was rewritten successfully, or error code if any error.
 */

/*ARGSUSED*/
int
_flrec_rewrite(Fcb *fcb, char *record, Recno recnum, int reclen)
{
    long		rec_position;
    char		delflag;

    /*
     * Check that recnum is within the range of existing record numbers.
     */
    if (recnum < 1 || recnum > fcb->lastrecno)
	return (EENDFILE);

    rec_position = _fl_getpos(fcb, recnum); /* Offset in .rec file */

    /*
     * Check that the record is not marked as deleted.
     */
    _cp_fromfile(fcb, fcb->datfd, &delflag, rec_position, 1); 
    if (delflag == FL_RECDELETED) {
	return (ENOREC);
    }

    /*
     * Copy new record to the .rec file. 
     */
    _cp_tofile(fcb, fcb->datfd, record, rec_position + 1, fcb->minreclen); 

    return (ISOK);
}

/*
 * _flrec_delete(fcb, recnum)
 *
 * Rewrite a record.
 *
 * Input params:
 *	FCB	File Control Block
 *	recnum	record number of the record
 *
 * Returns 0 if record was rewritten successfully, or error code if any error.
 */

int
_flrec_delete(Fcb *fcb, Recno recnum)
{
    long		rec_position;
    char		delflag;
    char		recnobuf [RECNOSIZE];

    /*
     * Check that recnum is within the range of existing record numbers.
     */
    if (recnum < 1 || recnum > fcb->lastrecno)
	return (EENDFILE);

    rec_position = _fl_getpos(fcb, recnum); /* Offset in .rec file */

    /*
     * Check that the record is not marked as deleted.
     */
    _cp_fromfile(fcb, fcb->datfd, &delflag, rec_position, 1); 
    if (delflag == FL_RECDELETED) {
	return (ENOREC);
    }

    /*
     * Set the delete flag to FL_RECDELETED.
     */
    delflag = FL_RECDELETED;
    _cp_tofile(fcb, fcb->datfd, &delflag, rec_position, 1);

    /*
     * Insert record into chain of deleted records.
     */
    strecno(fcb->freerecno, recnobuf);
    _cp_tofile(fcb, fcb->datfd, recnobuf, rec_position + 1, RECNOSIZE);
    fcb->freerecno = recnum;

    return (ISOK);
}

/*
 * _flrec_wrrec(fcb, record, recnum, reclen)
 *
 * Write a record by record number.
 *
 * Input params:
 *	FCB	File Control Block
 *	recnum	record number of the record
 *	record	record buffer
 *	int	reclen (NOT USED)
 *
 * Returns 0 if record was written successfully, or error code if any error.
 *
 * Note that _flrec_wrrec() commits updates and syncs the FCB to avoid
 * 	buffer pool overflow.
 */

/*ARGSUSED*/
int
_flrec_wrrec(Fcb *fcb, char *record, Recno recnum, int reclen)
{
    long		rec_position;
    char		delflag;
    Recno		recnum2;
    char		recnumbuf [RECNOSIZE];

    /*
     * Check that recnum is not negative.
     */
    if (recnum < 1)
	return (EBADARG);

    rec_position = _fl_getpos(fcb, recnum); /* Offset in .rec file */

    if (recnum > fcb->lastrecno)  {

	/*
	 * If the recnum is bigger than the highest record number in the .rec
	 * file, extend the .rec file.
	 */
	while (_fl_getpos(fcb, recnum + 1) > fcb->datsize * ISPAGESIZE) {
	    fcb->datsize = _extend_file(fcb, fcb->datfd, fcb->datsize);
	    
	    /* Sync the updates to avoid buffer pool overflow. */
	    _isdisk_commit();
	    _isdisk_sync();
	    (void)_isfcb_cntlpg_w2(fcb);
	}

	/*
	 * Mark all records in the range <fcb->lastrecno+1, recnum>  as
	 * deleted.
	 */
	delflag = FL_RECDELETED;
	for (recnum2 = fcb->lastrecno + 1; recnum2 <= recnum; recnum2++) {	
	    _cp_tofile(fcb, fcb->datfd, &delflag, _fl_getpos(fcb, recnum2), 1); 
	    strecno(fcb->freerecno, recnumbuf);
	    _cp_tofile(fcb, fcb->datfd, recnumbuf,
		       _fl_getpos(fcb, recnum2) + 1, RECNOSIZE); 
	    fcb->freerecno = recnum2;
	    
	    /* Sync the updates to avoid buffer pool overflow. */
	    _isdisk_commit();
	    _isdisk_sync();
	    fcb->lastrecno = recnum;
	    (void)_isfcb_cntlpg_w2(fcb);
	}
	
	/*
	 * Note that the disk structures are in a consistent state now,
	 * the .rec was extended by a few records marked as 'deleted'.
	 * This is important for subsequent rollbacks.
	 */
    } 

    /*
     * If recnum specifies a record that has existed, check whether it
     * has been deleted. _flrec_wrrec() does not override existing record.
     */
    _cp_fromfile(fcb, fcb->datfd, &delflag, rec_position, 1); 
    if (delflag == FL_RECEXISTS) {
	return (EDUPL);
    }

    /*
     * Remove the record from the chain of deleted records.
     */
    remove_from_chain(fcb, recnum);

    /*
     * Copy new record to the .rec file. 
     */
    delflag = FL_RECEXISTS;
    _cp_tofile(fcb, fcb->datfd, &delflag, rec_position, 1); 
    _cp_tofile(fcb, fcb->datfd, record, rec_position + 1, fcb->minreclen); 

    return (ISOK);
}

/*
 * remvoe_from_chain(fcb, recnum)
 *
 * Remove record from the chain of deleted records.
 */

static void
remove_from_chain(Fcb *fcb, Recno recnum)
{
    char		recnobuf1 [RECNOSIZE] , recnobuf2 [RECNOSIZE];
    long		pos1, pos2;
    Recno		recnum2;

    pos1 = _fl_getpos(fcb, recnum);
    _cp_fromfile(fcb, fcb->datfd, recnobuf1, pos1 + 1, RECNOSIZE); 

    if (fcb->freerecno == recnum) {
	fcb->freerecno = ldrecno(recnobuf1);
    }
    else {
	recnum2 = fcb->freerecno;
	do {
	    pos2 = _fl_getpos(fcb, recnum2);
	    _cp_fromfile(fcb, fcb->datfd, recnobuf2, pos2 + 1, RECNOSIZE); 
	    recnum2 = ldrecno(recnobuf2);
	} while (recnum2 != recnum && recnum2 != NULL_RECNO);

	_cp_tofile(fcb, fcb->datfd, recnobuf1, pos2 + 1, RECNOSIZE); 
    }
}
