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
/*%%  $XConsortium: isread.c /main/3 1995/10/23 11:43:35 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isread.c
 *
 * Description:
 *	Read a record from ISAM file. 
 */


#include "isam_impl.h"
#include <sys/time.h>

static int _amread();

/*
 * err =  isread(isfd, record, mode)
 *
 * Isread() reads a new record from an ISAM file. 
 *
 * Current record position is set.
 * isrecnum is set to indicate the read record.
 *
 * If the ISAM file is for variable length records, the isreclen variable
 * is set to indicate the actual length of the record.
 *
 * Returns 0 if successful, or -1 of any error.
 *
 * Errors:
 *	ELOCKED The record or the entire file is locked by another process.
 *	ENOTOPEN isfd does not correspond to an open ISAM file, or the
 *		ISAM file was opened with ISOUTPUT mode.
 *	EBADARG Bad mode parameter.
 *	ENOCURR Mode is ISCURR and the current record position is not set.
 *	ENOREC	Specified record cannot be found (random access read)
 *	EENDFILE The end file of is reached (sequential read).
 *	EBADKEY Index was deleted by other process (can happen only
 *		when lock file is purged).
 */

int 
isread(int isfd, char *record, int mode)
{
    Fab	*fab;
    int			reclen;
    Recno		recnum;
    int			ret;
    enum readmode	readmode;

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

    /*
     * Extract read mode.
     */
    if ((readmode = _getreadmode(mode)) == RM_BADMODE) {
	_setiserrno2(EBADARG, '9', '0');
	return (ISERROR);
    }

    /*
     * All keys must be in the minimum record length.
     * So send just the minimum length part of the record.
     */
    reclen = fab->minreclen;

    /*
     * Call the Access Method
     */
    recnum = isrecnum;

    if ((ret = _amread(&fab->isfhandle, record, &reclen,
			readmode, &fab->curpos, &recnum,
			&fab->errcode)) == ISOK) {
	isrecnum = recnum;		     /* Set isrecnum */
    }

    isreclen = reclen;
    _seterr_errcode(&fab->errcode);

    return (ret);			     /* Successful read */
}

/*
 * _amread(isfhandle, record, reclen, readmode, curpos, recnum, errcode)
 *
 * _amread() reads a record from ISAM file
 *
 * Input params:
 *	isfhandle	Handle of ISAM file
 *	readmode	Specifies access mode (random or sequential)
 *	curpos		current record position
 *	recnum		copy if isrecnum
 *
 * Output params:
 *	curpos		new current position
 *	recnum		record number
 *	errcode		error status of the operation
 *	reclen		actual length of the record
 *	record		filled with data
 *
 */

static int
_amread(Bytearray *isfhandle, char *record, int *reclen,
        enum readmode readmode, Bytearray *curpos, Recno *recnum,
        struct errcode *errcode)
{
    Fcb			*fcb = NULL;
    Recno		recnum2;
    int			err;
    Crp			*crp;
    Btree		*btree = NULL;
    Keydesc2		*pkeydesc2;
    char		keybuf1[MAXKEYSIZE], keybuf2[MAXKEYSIZE];
    char		*pkey, *pkeynext;
    int			skipbytes;
    int			ret;
    Bytearray		oldcurpos;
    int			(*rec_read)();

    _isam_entryhook();

    /*
     * Get FCB corresponding to the isfhandle handle.
     */
    if ((fcb = _openfcb(isfhandle, errcode)) == NULL) {
	_isam_exithook();
	return (ISERROR);
    }

    rec_read = (fcb->varflag?_vlrec_read:_flrec_read);

    /*
     * Update information in FCB from CNTL page on the disk
     */
    (void)_isfcb_cntlpg_r2(fcb);

    /*
     * Save the old record position.
     */
    oldcurpos = _bytearr_dup(curpos);

    /*
     * Get info from current record position structure.
     */
    crp = (Crp *) curpos->data;

    if (crp->keyid == PHYS_ORDER) {
	/*
	 * Physical order in use.
	 */
	
	switch (readmode) {
	case RM_EQUAL:
	    recnum2 = *recnum;		     /* passed from isrecnum */
	    if ((err = rec_read(fcb, record, recnum2, reclen)) != ISOK) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
	    break;

	case RM_GREAT:
	    recnum2 = *recnum + 1;
	    if (recnum2 < 1) recnum2 = 1;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, record, recnum2, reclen)) != ISOK &&
		   err == ENOREC) 
		recnum2++;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
	    break;

	case RM_GTEQ:
	    recnum2 = *recnum;
	    if (recnum2 < 1) recnum2 = 1;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, record, recnum2, reclen)) != ISOK &&
		   err == ENOREC) 
		recnum2++;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
	    break;

	case RM_LESS:
	    recnum2 = *recnum - 1;
	    if (recnum2 > fcb->lastrecno) recnum2 = fcb->lastrecno;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, record, recnum2, reclen)) != ISOK &&
		   err == ENOREC) 
		recnum2--;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
	    break;

	case RM_LTEQ:
	    recnum2 = *recnum;
	    if (recnum2 > fcb->lastrecno) recnum2 = fcb->lastrecno;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, record, recnum2, reclen)) != ISOK &&
		   err == ENOREC) 
		recnum2--;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
	    break;

	case RM_FIRST:
	    recnum2 = 1;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, record, recnum2, reclen)) != ISOK &&
		   err == ENOREC) 
		recnum2++;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    }
	    break;
	case RM_CURR:
	    switch (crp->flag) {
	    case CRP_ON:
	    case CRP_BEFORE:
	    case CRP_AFTER:
		recnum2 = crp->recno;
		break;
	    case CRP_BEFOREANY:
		recnum2 = 1;
		break;
	    default:
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
		
	    if( rec_read(fcb, record, recnum2, reclen) != ISOK) {
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
	    break;
	case RM_NEXT:
	    switch (crp->flag) {
	    case CRP_ON:
	    case CRP_AFTER:
		recnum2 = crp->recno + 1;
		break;
	    case CRP_BEFOREANY:
		recnum2 = 1;
		break;
	    case CRP_BEFORE:
		recnum2 = crp->recno;
		break;
	    case CRP_AFTERANY:
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    default:
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
		
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, record, recnum2, reclen)) != ISOK &&
		   err == ENOREC) 
		recnum2++;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    }
	    break;
	case RM_PREV:
	    switch (crp->flag) {
	    case CRP_ON:
	    case CRP_BEFORE:
		recnum2 = crp->recno - 1;
		break;
	    case CRP_AFTER:
		recnum2 = crp->recno;
		break;
	    case CRP_BEFOREANY:
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    default:
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
		
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, record, recnum2, reclen)) != ISOK &&
		   err == ENOREC) 
		recnum2--;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    }
	    break;
	case RM_LAST:
	    recnum2 = fcb->lastrecno;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, record, recnum2, reclen)) != ISOK &&
		   err == ENOREC) 
		recnum2--;

	    if (err != ISOK) {
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    }
	    break;
	default:
	    _isfatal_error("Invalid readmode");
	}

	*recnum = recnum2;

	/*
	 * Set new current record position.
	 */
	 crp->recno = recnum2;
	 crp->flag = CRP_ON;

    } /* physical order */
    else {
	/*
	 * Find key descriptor in FCB
	 */
	if ((pkeydesc2 = _isfcb_indfindkey(fcb, crp->keyid)) == NULL) {
	    _amseterrcode(errcode, EBADKEY);
	    goto ERROR;
	}

	/*
	 * skipkeybytes is set to the number of bytes in the beginning
	 * of the key:
	 *  RECNOSIZE for ISNODUPS keys to skip recno part
	 *  RECNOSIZE + DUPIDSIZE to skip recno and duplicate serial number
	 */
	skipbytes = RECNOSIZE;
	if (ALLOWS_DUPS2(pkeydesc2))
	    skipbytes += DUPIDSIZE;
		
	/*
	 * Create B tree object.
	 */
	btree = _isbtree_create(fcb, pkeydesc2);

	switch (readmode) {
	case RM_EQUAL:
	case RM_GTEQ:
            /* 
	     * Make sure that you will read the first duplicate. 
	     */
            _iskey_fillmin(pkeydesc2, keybuf1);
 
            /* 
	     * Extract key fields from record. 
	     */
            _iskey_extract(pkeydesc2, record, keybuf2);
            memcpy((void *)(keybuf1 + skipbytes),
		   (const void *)(keybuf2 + skipbytes), crp->matchkeylen);

            /*
	     * Position pointer in the B-tree in before the searched value. 
	     */
            _isbtree_search(btree, keybuf1);
        
            if ((pkey = _isbtree_next(btree)) == NULL) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
            
            if (readmode == RM_EQUAL &&
                memcmp(keybuf1 + skipbytes, pkey + skipbytes, 
		       crp->matchkeylen) != 0) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }

	    crp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy((void *)crp->key, (const void *)pkey, pkeydesc2->k2_len);
		
	    if( rec_read(fcb, record, crp->recno, reclen) != ISOK) {
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
	    recnum2 = crp->recno;
	    break;

	case RM_GREAT:
            /* 
	     * Make sure that you will read past all matching records.
	     */
            _iskey_fillmax(pkeydesc2, keybuf1);
 
            /* 
	     * Extract key fields from record. 
	     */
            _iskey_extract(pkeydesc2, record, keybuf2);
            memcpy((void *)(keybuf1 + skipbytes),
		   (const void *)(keybuf2 + skipbytes), crp->matchkeylen);

            /*
	     * Position pointer in the B-tree in before the searched value. 
	     */
            _isbtree_search(btree, keybuf1);
        
            if ((pkey = _isbtree_next(btree)) == NULL) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
            
	    crp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy((void *)crp->key, (const void *)pkey, pkeydesc2->k2_len);
		
	    if( rec_read(fcb, record, crp->recno, reclen) != ISOK) {
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
	    recnum2 = crp->recno;
	    break;

	case RM_LESS:
            /* 
	     * Make sure that you will read before all matching records.
	     */
            _iskey_fillmin(pkeydesc2, keybuf1);
 
            /* 
	     * Extract key fields from record. 
	     */
            _iskey_extract(pkeydesc2, record, keybuf2);
            memcpy((void *)(keybuf1 + skipbytes),
		   (const void *)(keybuf2 + skipbytes), crp->matchkeylen);

            /*
	     * Position pointer in the B-tree in before the searched value. 
	     */
            _isbtree_search(btree, keybuf1);
        
            if ((pkey = _isbtree_current(btree)) == NULL) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
            
	    crp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy((void *)crp->key, (const void *)pkey, pkeydesc2->k2_len);
		
	    if( rec_read(fcb, record, crp->recno, reclen) != ISOK) {
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
	    recnum2 = crp->recno;
	    break;

	case RM_LTEQ:
            /* 
	     * Make sure that you will read the last duplicate.
	     */
            _iskey_fillmax(pkeydesc2, keybuf1);
 
            /* 
	     * Extract key fields from record. 
	     */
            _iskey_extract(pkeydesc2, record, keybuf2);
            memcpy((void *)(keybuf1 + skipbytes),
		   (const void *)(keybuf2 + skipbytes), crp->matchkeylen);

            /*
	     * Position pointer in the B-tree in before the searched value. 
	     */
            _isbtree_search(btree, keybuf1);
        
            if ((pkey = _isbtree_current(btree)) == NULL) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
            
	    crp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy((void *)crp->key, (const void *)pkey, pkeydesc2->k2_len);
		
	    if( rec_read(fcb, record, crp->recno, reclen) != ISOK) {
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
	    recnum2 = crp->recno;
	    break;

	case RM_CURR:
	    switch (crp->flag) {
	    case CRP_ON:
	    case CRP_BEFORE:
	    case CRP_AFTER:
		/*
		 * We have check if the record has not been deleted
		 * since the current record position was set up.
		 */
		_isbtree_search(btree, crp->key);
		pkey = _isbtree_current(btree);

		if (pkey == NULL || 
		    ldrecno(pkey + KEY_RECNO_OFF) != crp->recno) {
		    _amseterrcode(errcode, ENOCURR);
		    goto ERROR;
		}
		break;
	    case CRP_BEFOREANY:
		_isbtree_search(btree, crp->key);
		pkey = _isbtree_next(btree);

		if (pkey == NULL) {
		    _amseterrcode(errcode, EENDFILE);
		    goto ERROR;
		}
           
		crp->recno = ldrecno(pkey + KEY_RECNO_OFF);
		memcpy((void *)crp->key, (const void *)pkey, pkeydesc2->k2_len);
		break;
	    default:
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
		
	    if( rec_read(fcb, record, crp->recno, reclen) != ISOK) {
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
	    recnum2 = crp->recno;
	    break;

	case RM_NEXT:

	    /*
	     * Validate that current position has been set.
	     */
	    switch (crp->flag) {
	    case CRP_ON:
	    case CRP_BEFORE:
	    case CRP_BEFOREANY:
		break;
	    case CRP_AFTERANY:
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    default:
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
 
            /*
	     * Position pointer to current position.
	     */
            _isbtree_search(btree, crp->key);
        
	    if (crp->flag == CRP_BEFORE) 
		pkey = _isbtree_current(btree);
	    else 			   
		/* crp->flag == CRP_ON || crp->flag == CRP_BEFOREANY */
		pkey = _isbtree_next(btree);

	    if (pkey == NULL) {
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    }
           
	    crp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy((void *)crp->key, (const void *)pkey, pkeydesc2->k2_len);
		
	    if( rec_read(fcb, record, crp->recno, reclen) != ISOK) {
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
	    recnum2 = crp->recno;
	    break;

	case RM_PREV:

	    /*
	     * Validate that current position has been set.
	     */
	    switch (crp->flag) {
	    case CRP_ON:
	    case CRP_BEFORE:
		/*
		 * To get to the previous record, we must decrement
		 * the TID part for unique keys, or duplicate serial number 
		 * for non-unique keys.
		 */
		memcpy((void *)keybuf1,
		       (const void *)crp->key, pkeydesc2->k2_len);
		if (ALLOWS_DUPS2(pkeydesc2)) {
		    stlong(ldlong(keybuf1 + KEY_DUPS_OFF) - 1, 
			    keybuf1 + KEY_DUPS_OFF);
		}
		else {
		    strecno(ldrecno(keybuf1 + KEY_RECNO_OFF) - 1, 
			    keybuf1 + KEY_RECNO_OFF);
		}
		break;
	    case CRP_AFTER:
		memcpy((void *)keybuf1, (const void *)crp->key, pkeydesc2->k2_len);
		break;
	    case CRP_BEFOREANY:
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    default:
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
 
            /*
	     * Position pointer to current position.
	     */
            _isbtree_search(btree, keybuf1);
        
	    pkey = _isbtree_current(btree);

	    if (pkey == NULL) {
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    }
           
	    crp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy((void *)crp->key, (const void *)pkey, pkeydesc2->k2_len);
		
	    if( rec_read(fcb, record, crp->recno, reclen) != ISOK) {
		_amseterrcode(errcode, ENOCURR);
		goto ERROR;
	    }
	    recnum2 = crp->recno;
	    break;

	case RM_FIRST:
            /* 
	     * Fill key buffer with -infinity.
	     */
            _iskey_fillmin(pkeydesc2, keybuf1);
 
            /*
	     * Position pointer in the B-tree before any key entry.
	     */
            _isbtree_search(btree, keybuf1);
        
            if ((pkey = _isbtree_next(btree)) == NULL) {
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    }
            
	    crp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy((void *)crp->key, (const void *)pkey, pkeydesc2->k2_len);
		
	    if( rec_read(fcb, record, crp->recno, reclen) != ISOK) {
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    }
	    recnum2 = crp->recno;
	    break;

	case RM_LAST:
            /* 
	     * Fill key buffer with +infinity.
	     */
            _iskey_fillmax(pkeydesc2, keybuf1);
 
            /*
	     * Position pointer in the B-tree before any key entry.
	     */
            _isbtree_search(btree, keybuf1);
        
            if ((pkey = _isbtree_current(btree)) == NULL) {
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    }
            
	    crp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy((void *)crp->key, (const void *)pkey, pkeydesc2->k2_len);
		
	    if( rec_read(fcb, record, crp->recno, reclen) != ISOK) {
		_amseterrcode(errcode, EENDFILE);
		goto ERROR;
	    }
	    recnum2 = crp->recno;
	    break;

	default:
	    _isfatal_error("Invalid readmode");
	}

	*recnum = recnum2;
	 crp->flag = CRP_ON;

	/*
	 * Set up isdupl to handle isstat2 value for keys that allow
	 * duplicate values.
	 */
	if (ALLOWS_DUPS2(pkeydesc2) && (pkeynext = _isbtree_next(btree)) &&
	    memcmp(pkey + skipbytes, pkeynext + skipbytes, 
		   crp->matchkeylen) == 0) {
	    isdupl = 1;
	}


	_isbtree_destroy(btree);
    }

    _amseterrcode(errcode, ISOK);
    ret = ISOK;

    /* Clean-up work. */
    _isdisk_commit();			     /* This will only check
					      * that we unfixed all fixed
					      * buffers */
    _isdisk_inval();
    _bytearr_free(&oldcurpos);

    _isam_exithook();
    return (ret);

 ERROR:

    *reclen = 0;

    _isdisk_inval();

    _bytearr_free(&oldcurpos);

    /*
     * If error is ENOREC,  set the current record position undefined.
     */
    if (errcode->iserrno == ENOREC || errcode->iserrno == EENDFILE) {
	((Crp *)curpos->data)->flag = CRP_UNDEF;
    }

    if (btree != NULL)
	_isbtree_destroy(btree);

    _isam_exithook();
    return (ISERROR);
}


