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
/*%%  $XConsortium: isvarrec.c /main/3 1995/10/23 11:45:36 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isvarrec.c
 *
 * Description:
 *	Fixed length record access (VLRA) module.
 */

#include "isam_impl.h"

/* Local functions */
long _vl_getpos();			     /* Get offset in .rec file */
int  _vl_deleted();			     /* 0/1 returns 1 if record is deleted */
static void remove_from_chain2();	     /* used by _vlrec_wrrec() */
long _istail_insert();
static void _istail_delete();
static int _istail_read();

/*
 * _vlrec_write(fcb, record, recnum, reclen)
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
_vlrec_write(Fcb *fcb, char *record, Recno *recnum, int reclen)
{
    Recno		recnum2;
    long		rec_position;
    long		tailoff = VL_RECNOTAIL;
    char		recnobuf [RECNOSIZE];
    char		tailoffbuf[LONGSIZE];

    /*
     * Reuse a deleted record if one exits.
     * Otherwise, extend .rec file by a record.
     */
    if (fcb->freerecno != NULL_RECNO) {
	recnum2 = fcb->freerecno;

	/*
	 * Remove record from the chain of deleted records.
	 */
	rec_position = _vl_getpos(fcb, recnum2); /* Offset in .rec file */
	_cp_fromfile(fcb, fcb->datfd, recnobuf, rec_position + LONGSIZE, RECNOSIZE);
	fcb->freerecno = ldrecno(recnobuf);
    }
    else {
	recnum2 = ++(fcb->lastrecno);

	/* 
	 * Extend .rec file size if necessary.
	 */
	while (_vl_getpos(fcb, recnum2 + 1) > fcb->datsize * ISPAGESIZE) {
	    fcb->datsize = _extend_file(fcb, fcb->datfd, fcb->datsize);
	}
	rec_position = _vl_getpos(fcb, recnum2); /* Offset in .rec file */
    }

    /* 
     * Store variable part of record (the 'tail') in .var file.
     */
    tailoff = _istail_insert(fcb, record + fcb->minreclen, 
			     reclen - fcb->minreclen);

    /*
     * Copy record to the .rec file. Mark record as undeleted.
     */
    stlong(tailoff, tailoffbuf);
    _cp_tofile(fcb, fcb->datfd, tailoffbuf, rec_position, LONGSIZE); 
    _cp_tofile(fcb, fcb->datfd, record, rec_position + LONGSIZE, fcb->minreclen); 

    *recnum = recnum2;

    return (ISOK);
}

/*
 * _vlrec_read(fcb, record, recnum, reclen)
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
_vlrec_read(Fcb *fcb, char *record, Recno recnum, int *reclen)
{
    long		rec_position;
    long		tailoff;
    char		tailoffbuf[LONGSIZE];

    /*
     * Check that recnum is within the range of existing record numbers.
     */
    if (recnum < 1 || recnum > fcb->lastrecno)
	return (EENDFILE);

    rec_position = _vl_getpos(fcb, recnum); /* Offset in .rec file */

    /*
     * Check that the record is not marked as deleted.
     */
    _cp_fromfile(fcb, fcb->datfd, tailoffbuf, rec_position, LONGSIZE);
    tailoff = ldlong(tailoffbuf);
    if (tailoff == VL_RECDELETED) {
	return (ENOREC);
    }

    /*
     * Copy record from the .at file. 
     */
    _cp_fromfile(fcb, fcb->datfd, record, rec_position + LONGSIZE, fcb->minreclen); 

    *reclen = fcb->minreclen;

    /*
     * Get the 'tail' of the record if any.
     */
    *reclen += _istail_read(fcb, tailoff, record + fcb->minreclen);

    if (*reclen > fcb->maxreclen)
	_isfatal_error("Corrupted file: too long variable length record");

    return (ISOK);
}

/*
 * pos = _vl_getpos(fcb, recnum)
 *
 * Calculate the position of record in .rec file.
 */

long
_vl_getpos(Fcb *fcb, Recno recnum)
{
    return ((long)(ISCNTLSIZE + (fcb->minreclen + LONGSIZE) * (recnum -1)));
}

/*
 * _vlrec_rewrite(fcb, record, recnum, reclen)
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
_vlrec_rewrite(Fcb *fcb, char *record, Recno recnum, int reclen)
{
    long		rec_position;
    long		tailoff;
    char		tailoffbuf[LONGSIZE];

    /*
     * Check that recnum is within the range of existing record numbers.
     */
    if (recnum < 1 || recnum > fcb->lastrecno)
	return (EENDFILE);

    rec_position = _vl_getpos(fcb, recnum); /* Offset in .rec file */

    /*
     * Check that the record is not marked as deleted.
     */
    _cp_fromfile(fcb, fcb->datfd, tailoffbuf, rec_position, LONGSIZE); 
    tailoff = ldlong(tailoffbuf);
    if (tailoff == VL_RECDELETED) {
	return (ENOREC);
    }

    /* 
     * Store variable part of record (the 'tail') in .var file.
     */
    tailoff = _istail_modify(fcb, tailoff, record + fcb->minreclen, 
			     reclen - fcb->minreclen);

    /*
     * Copy new record to the .rec file. 
     */
    stlong(tailoff, tailoffbuf);
    _cp_tofile(fcb, fcb->datfd, tailoffbuf, rec_position, LONGSIZE);
    _cp_tofile(fcb, fcb->datfd, record, rec_position + LONGSIZE, fcb->minreclen); 

    return (ISOK);
}

/*
 * _vlrec_delete;(fcb, recnum)
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
_vlrec_delete(Fcb *fcb, Recno recnum)
{
    long		rec_position;
    long		tailoff;
    char		tailoffbuf[LONGSIZE];
    char		recnobuf [RECNOSIZE];

    /*
     * Check that recnum is within the range of existing record numbers.
     */
    if (recnum < 1 || recnum > fcb->lastrecno)
	return (EENDFILE);

    rec_position = _vl_getpos(fcb, recnum); /* Offset in .rec file */

    /*
     * Check that the record is not marked as deleted.
     */
    _cp_fromfile(fcb, fcb->datfd, tailoffbuf, rec_position, LONGSIZE); 
    tailoff = ldlong(tailoffbuf);
    if (tailoff == VL_RECDELETED) {
	return (ENOREC);
    }

    /*
     * Set the delete flag to VL_RECDELETED.
     */
    tailoff = VL_RECDELETED;
    stlong(tailoff, tailoffbuf);
    _cp_tofile(fcb, fcb->datfd, tailoffbuf, rec_position, LONGSIZE);

    /*
     * Insert record into chain of deleted records.
     */
    strecno(fcb->freerecno, recnobuf);
    _cp_tofile(fcb, fcb->datfd, recnobuf, rec_position + LONGSIZE, RECNOSIZE);
    fcb->freerecno = recnum;

    /*
     * Delete tail from .var file.
     */

    return (ISOK);
}

/*
 * _vlrec_wrrec(fcb, record, recnum, reclen)
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
 * Note that _vlrec_wrrec() commits updates and syncs the FCB to avoid
 * 	buffer pool overflow.
 */

/*ARGSUSED*/
int
_vlrec_wrrec(Fcb *fcb, char *record, Recno recnum, int reclen)
{
    long		rec_position;
    long		tailoff;
    char		tailoffbuf[LONGSIZE];
    Recno		recnum2;
    char		recnumbuf [RECNOSIZE];

    /*
     * Check that recnum is not negative.
     */
    if (recnum < 1)
	return (EBADARG);

    rec_position = _vl_getpos(fcb, recnum); /* Offset in .rec file */

    if (recnum > fcb->lastrecno)  {

	/*
	 * If the recnum is bigger than the highest record number in the .rec
	 * file, extend the .rec file.
	 */
	while (_vl_getpos(fcb, recnum + 1) > fcb->datsize * ISPAGESIZE) {
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
	tailoff = VL_RECDELETED;
	stlong(tailoff, tailoffbuf);
	for (recnum2 = fcb->lastrecno + 1; recnum2 <= recnum; recnum2++) {	
	    _cp_tofile(fcb, fcb->datfd, tailoffbuf, _vl_getpos(fcb, recnum2), LONGSIZE); 
	    strecno(fcb->freerecno, recnumbuf);
	    _cp_tofile(fcb, fcb->datfd, recnumbuf,
		       _vl_getpos(fcb, recnum2) + LONGSIZE, RECNOSIZE); 
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
     * has been deleted. _vlrec_wrrec() does not override existing record.
     */
    _cp_fromfile(fcb, fcb->datfd, tailoffbuf, rec_position, LONGSIZE); 
    tailoff = ldlong(tailoffbuf);
    if (tailoff != VL_RECDELETED) {
	return (EDUPL);
    }

    /*
     * Remove the record from the chain of deleted records.
     */
    remove_from_chain2(fcb, recnum);

    /* 
     * Store variable part of record (the 'tail') in .var file.
     */
    tailoff = _istail_insert(fcb, record + fcb->minreclen, reclen - fcb->minreclen);

    /*
     * Copy new record to the .rec file. 
     */
    stlong(tailoff, tailoffbuf);
    _cp_tofile(fcb, fcb->datfd, tailoffbuf, rec_position, LONGSIZE); 
    _cp_tofile(fcb, fcb->datfd, record, rec_position + LONGSIZE, fcb->minreclen); 

    return (ISOK);
}

/*
 * remove_from_chain(fcb, recnum)
 *
 * Remove record from the chain of deleted records.
 */

static void
remove_from_chain2(Fcb *fcb, Recno recnum)
{
    char		recnobuf1 [RECNOSIZE] , recnobuf2 [RECNOSIZE];
    long		pos1, pos2;
    Recno		recnum2;

    pos1 = _vl_getpos(fcb, recnum);
    _cp_fromfile(fcb, fcb->datfd, recnobuf1, pos1 + LONGSIZE, RECNOSIZE); 

    if (fcb->freerecno == recnum) {
	fcb->freerecno = ldrecno(recnobuf1);
    }
    else {
	recnum2 = fcb->freerecno;
	do {
	    pos2 = _vl_getpos(fcb, recnum2);
	    _cp_fromfile(fcb, fcb->datfd, recnobuf2, pos2 + LONGSIZE, RECNOSIZE); 
	    recnum2 = ldrecno(recnobuf2);
	} while (recnum2 != recnum && recnum2 != NULL_RECNO);

	_cp_tofile(fcb, fcb->datfd, recnobuf1, pos2 + LONGSIZE, RECNOSIZE); 
    }
}


/*
 * The following are functions that manipulate the 'tails' of variable
 * records.  The tail is the actual record with the fixed part removed
 * (fixed part starts at offset zero and its length is minimum record 
 * length.  The tails are stored in the .var file.
 */


/* Insert tail into .var file. Return offset in .var file */

long _istail_insert(Fcb *fcb, char *tailp, int taillen)
{
    char		frameheadbuf [2 * SHORTSIZE];
    int			framelen;
    long		offset;

/*    printf ("_insert called, taillen %d\n", taillen); */

    if (taillen == 0)
	return (VL_RECNOTAIL);

    framelen = taillen + 2 * SHORTSIZE;

    /*
     * Set up frame header.
     */
    stshort((short)taillen, frameheadbuf + VR_FRAMELEN_OFF);
    stshort((short)taillen, frameheadbuf + VR_TAILLEN_OFF);

    offset = fcb->varend;

    /*
     * Extend .var file if that is necesary.	
     */
    while (offset + framelen > fcb->varsize * ISPAGESIZE)
	fcb->varsize = _extend_file(fcb, fcb->varfd, fcb->varsize);

    /*
     * Copy frame head and tail to .var file.
     */
    _cp_tofile(fcb, fcb->varfd, frameheadbuf, offset, 2 * SHORTSIZE);
    _cp_tofile(fcb, fcb->varfd, tailp, offset + 2 * SHORTSIZE, taillen);
 
    fcb->varend += taillen + 2 * SHORTSIZE;

    return (offset);
}

/* Remove tail from .var file */

/* ARGSUSED */
Static void _istail_delete(Fcb *fcb, long offset)
{
    /* 
     * Don't do anything in NetISAM 1.0. The tails are lost, the space
     * will be re-used after next restructuring: "copy -c file" command
     */
    return;
}

/* Read tail from .var file */

Static int _istail_read(Fcb *fcb, long offset, char *buffer)
{
    char		frameheadbuf [2 * SHORTSIZE];
    int			taillen;

/*    printf ("_read called, offset %d\n", offset); */

    if (offset == VL_RECNOTAIL)
	return (0);

    /* 
     * Read frame header.
     */
     _cp_fromfile(fcb, fcb->varfd, frameheadbuf, offset, 2 * SHORTSIZE);
    taillen = ldshort(frameheadbuf + VR_TAILLEN_OFF);
     _cp_fromfile(fcb, fcb->varfd, buffer, offset + 2 * SHORTSIZE, taillen);
    
    return (taillen);
}
    
/* Rewrite tail. Returns -1 if the new tail is longer than the original frame */

int _istail_modify(Fcb *fcb, long offset, char *tailp, int taillen)
{
    char		frameheadbuf [2 * SHORTSIZE];
    int			framelen;

    /*
     * Trivial case: no old frame, no new tail.
     */
    if (offset == VL_RECNOTAIL && taillen == 0)
	return (offset);

    if (offset != VL_RECNOTAIL) {
	/* 
	 * Read frame header.
	 */
	_cp_fromfile(fcb, fcb->varfd, frameheadbuf, offset, 2 * SHORTSIZE);
	framelen = ldshort(frameheadbuf + VR_FRAMELEN_OFF);
    }
    else
	framelen = 0;

    if (taillen > framelen) {
	/* 
	 * Delete the old frame if the new tail does not fit.
	 * Insert the new tail at the end of .var file.
	 */

	_istail_delete(fcb, offset);
	return (_istail_insert(fcb, tailp, taillen));
    }
    else {
	/*
	 * The new tail fits in the existing frame.
	 */
	stshort((short)taillen, frameheadbuf + VR_TAILLEN_OFF);
	_cp_tofile(fcb, fcb->varfd, frameheadbuf, offset, 2 * SHORTSIZE);
	_cp_tofile(fcb, fcb->varfd, tailp, offset + 2 * SHORTSIZE, taillen);

	return (offset);
    }
}
