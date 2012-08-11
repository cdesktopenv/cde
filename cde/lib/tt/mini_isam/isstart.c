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
/*%%  $XConsortium: isstart.c /main/3 1995/10/23 11:45:08 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isstart.c 1.9 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isstart.c
 *
 * Description:
 *	Select index and set record position.
 */


#include "isam_impl.h"
#include <sys/time.h>
#include <stdlib.h>

static int _amstart();

/*
 * err =  isstart(isfd, keydesc, length, record, mode)
 *
 * Isstart() selects index for subsequent read operations and set new
 *	current record position.
 *
 * isrecnum is set to indicate the start record.
 *
 *
 * Returns 0 if successful, or -1 of any error.
 *
 * Errors:
 *	ENOTOPEN isfd does not correspond to an open ISAM file, or the
 *		ISAM file was opened with ISOUTPUT mode.
 *	EBADARG Bad mode parameter.
 *	EBADARG keylen is out of range.
 *	ENOREC	Specified record cannot be found (random access read).
 *	EBADKEY Error in the key descriptor.
 */

int 
isstart(isfd, keydesc, length, record, mode)
    int			isfd;
    struct keydesc	*keydesc;
    int			length;
    char		*record;
    int			mode;
{
    register Fab	*fab;
    int			reclen;
    Recno		recnum;
    int			ret;
    enum readmode	readmode;
    char		dummy_record [1];    /* used for ISFIRST and ISLAST */
    char		*precord;

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
     * Some arguments are used only when a particular mode is specified.
     */
    if (readmode == RM_FIRST || readmode == RM_LAST || USE_PHYS_ORDER(keydesc)) {
	precord = dummy_record;
	reclen = 0;
    }
    else {
	precord = record;
	reclen = fab->minreclen;
    }
    
    reclen = fab->minreclen;
	
    recnum = isrecnum;

    if ((ret = _amstart(&fab->isfhandle, precord, reclen,
			readmode, keydesc, length, &fab->curpos,
			&recnum, &fab->errcode)) == ISOK) {
	isrecnum = recnum;		     /* Set isrecnum */
    }

    _seterr_errcode(&fab->errcode);

    return (ret);			     /* Successful start */
}

/*
 * _amstart(isfhandle, record, reclen, readmode,
 * 	    keydesc, keylen, curpos, recnum, errcode)
 *
 * _amstart() reads a record from ISAM file
 *
 * Input params:
 *	isfhandle	Handle of ISAM file
 *	readmode	Specifies access mode (random or sequential)
 *	curpos		old record position
 *	recnum		copy of isrecnum
 *	keydesc		key descriptor
 *	keylen          # of bytes of key to match
 *	record		extract key from this record buffer
 *
 * Output params:
 *	curpos		new current position
 *	recnum		record number
 *	errcode		error status of the operation
 *	reclen		actual length of the record
 *
 * Note:
 * Successfull isstart() returns the new curpos and frees the old curpos.
 */

/* ARGSUSED */
static int
_amstart(isfhandle, record, reclen, readmode,
	 keydesc, keylen, curpos, recnum, errcode)
    Bytearray		*isfhandle;
    enum readmode      	readmode;
    char		*record;
    int			*reclen;
    Bytearray		*curpos;
    Recno		*recnum;
    struct errcode	*errcode;
    struct keydesc	*keydesc;
    int			keylen;
{
    Fcb			*fcb;
    Recno		recnum2;
    int			err;
    Crp			*newcrp = NULL;
    char		recbuf [ISMAXRECLEN];
    Keydesc2		keydesc2;
    Keydesc2		*pkeydesc2;
    int			newcrpsize = 0;
    char		keybuf1 [MAXKEYSIZE], keybuf2 [MAXKEYSIZE];
    int			matchkeylen;
    int			skipbytes;
    char		*pkey;
    Btree		*btree = NULL;
    int			reclen2;
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

    if (USE_PHYS_ORDER(keydesc)) {
	/*
	 * Physical order in use.
	 */

	/*
	 * Allocate new current position structure.
	 */
	newcrpsize = sizeof(*newcrp);
	newcrp = (Crp *) _ismalloc(sizeof(*newcrp));
	memset ((char *)newcrp, 0, sizeof(*newcrp));
	newcrp->keyid = PHYS_ORDER;
	
	switch (readmode) {
	case RM_EQUAL:
	    recnum2 = *recnum;		     /* passed from isrecnum */
	    if ((err = rec_read(fcb, recbuf, recnum2, &reclen2)) != ISOK) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
	    newcrp->flag = CRP_BEFORE;
	    newcrp->recno = recnum2;
	    break;

	case RM_GREAT:
	    recnum2 = *recnum + 1;
	    if (recnum2 < 1) recnum2 = 1;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, recbuf, recnum2, &reclen2)) != ISOK &&
		   err == ENOREC) 
		recnum2++;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
	    newcrp->flag = CRP_BEFORE;
	    newcrp->recno = recnum2;
	    break;

	case RM_GTEQ:
	    recnum2 = *recnum;
	    if (recnum2 < 1) recnum2 = 1;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, recbuf, recnum2, &reclen2)) != ISOK &&
		   err == ENOREC) 
		recnum2++;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
	    newcrp->flag = CRP_BEFORE;
	    newcrp->recno = recnum2;
	    break;

	case RM_LESS:
	    recnum2 = *recnum - 1;
	    if (recnum2 > fcb->lastrecno) recnum2 = fcb->lastrecno;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, recbuf, recnum2, &reclen2)) != ISOK &&
		   err == ENOREC) 
		recnum2--;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
	    newcrp->flag = CRP_AFTER;
	    newcrp->recno = recnum2;
	    break;

	case RM_LTEQ:
	    recnum2 = *recnum;
	    if (recnum2 > fcb->lastrecno) recnum2 = fcb->lastrecno;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, recbuf, recnum2, &reclen2)) != ISOK &&
		   err == ENOREC) 
		recnum2--;
 
	    if (err != ISOK) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
	    newcrp->flag = CRP_AFTER;
	    newcrp->recno = recnum2;
	    break;

	case RM_FIRST:
	    recnum2 = 1;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, recbuf, recnum2, &reclen2)) != ISOK &&
		   err == ENOREC) 
		recnum2++;
 
	    if (err == ISOK) {
		newcrp->flag = CRP_BEFORE;
		newcrp->recno = recnum2;
	    }
	    else {
		newcrp->flag = CRP_AFTERANY;
	    }
	    break;
	case RM_LAST:
	    recnum2 = fcb->lastrecno;
	    /*
	     * Skip deleted records.
	     */
	    while ((err = rec_read(fcb, recbuf, recnum2, &reclen2)) != ISOK &&
		   err == ENOREC) 
		recnum2--;

	    if (err == ISOK) {
		newcrp->flag = CRP_AFTER;
		newcrp->recno = recnum2;
	    }
	    else {
		newcrp->flag = CRP_BEFOREANY;
	    }
	    break;
	default:
	    _isfatal_error("Invalid readmode");
	}

	*recnum = recnum2;

	/*
	 * Build new curpos, deallocate old curpos.
	 */
	_bytearr_free(curpos);
	*curpos = _bytearr_new(sizeof(*newcrp), (char *)newcrp);

    } /* physical order */
    else {

	/*
	 * Use order defined by some key.
	 */
	
	/*
	 * Check key descriptor for validity.
	 */
	if (_validate_keydesc(keydesc, fcb->minreclen) != ISOK) {
	    _amseterrcode(errcode, EBADKEY);
	    goto ERROR;
	}
	
	/*
	 * Convert key descriptor to internal form.
	 */
	_iskey_xtoi (&keydesc2, keydesc);
	
	/* Find key decriptor in the FCB. */
	if ((pkeydesc2 = _isfcb_findkey(fcb ,&keydesc2)) == NULL) {
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
	 * Validate keylen.
	 */
	if (keylen < 0 || keylen > pkeydesc2->k2_len - skipbytes) {
	    _amseterrcode(errcode, EBADARG);
	    goto ERROR;
	}

	/*
	 * Special case if keylen == 0: use the entire key.
	 */
	if (keylen == 0)
	    matchkeylen = pkeydesc2->k2_len - skipbytes;
	else
	    matchkeylen = keylen;

	/*
	 * Allocate new current record position.
	 */
	newcrpsize = sizeof(Crp) + pkeydesc2->k2_len;
	newcrp = (Crp *) _ismalloc((unsigned)newcrpsize);
	memset((char *)newcrp, 0, newcrpsize);

	newcrp->keyid = pkeydesc2->k2_keyid; /* Key identifier in FCB */
	newcrp->matchkeylen = matchkeylen;   /* number of bytes to match */

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
            memcpy( keybuf1 + skipbytes,keybuf2 + skipbytes, matchkeylen);

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
		       matchkeylen) != 0) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }

	    newcrp->flag = CRP_BEFORE;
	    newcrp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy( newcrp->key,pkey, pkeydesc2->k2_len);
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
            memcpy( keybuf1 + skipbytes,keybuf2 + skipbytes, matchkeylen);

            /*
	     * Position pointer in the B-tree after the searched value. 
	     */
            _isbtree_search(btree, keybuf1);
        
            if ((pkey = _isbtree_next(btree)) == NULL) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
            
	    newcrp->flag = CRP_BEFORE;
	    newcrp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy( newcrp->key,pkey, pkeydesc2->k2_len);
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
            memcpy( keybuf1 + skipbytes,keybuf2 + skipbytes, matchkeylen);

            /*
	     * Position pointer in the B-tree after the searched value. 
	     */
            _isbtree_search(btree, keybuf1);
        
            if ((pkey = _isbtree_current(btree)) == NULL) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
            
	    newcrp->flag = CRP_AFTER;
	    newcrp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy( newcrp->key,pkey, pkeydesc2->k2_len);
	    break;

	case RM_LTEQ:
            /* 
	     * Make sure that you will the last duplicate.
	     */
            _iskey_fillmax(pkeydesc2, keybuf1);
 
            /* 
	     * Extract key fields from record. 
	     */
            _iskey_extract(pkeydesc2, record, keybuf2);
            memcpy( keybuf1 + skipbytes,keybuf2 + skipbytes, matchkeylen);

            /*
	     * Position pointer in the B-tree in before the searched value. 
	     */
            _isbtree_search(btree, keybuf1);
        
            if ((pkey = _isbtree_current(btree)) == NULL) {
		_amseterrcode(errcode, ENOREC);
		goto ERROR;
	    }
            
	    newcrp->flag = CRP_AFTER;
	    newcrp->recno = ldrecno(pkey + KEY_RECNO_OFF);
	    memcpy( newcrp->key,pkey, pkeydesc2->k2_len);
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
		newcrp->flag = CRP_AFTERANY;
	    }
            else {
		newcrp->flag = CRP_BEFORE;
		newcrp->recno = ldrecno(pkey + KEY_RECNO_OFF);
		memcpy( newcrp->key,pkey, pkeydesc2->k2_len);
	    }
	    break;

	case RM_LAST:
            /* 
	     * Fill key buffer with +infinity.
	     */
            _iskey_fillmax(pkeydesc2, keybuf1);
 
            /*
	     * Position pointer in the B-tree after all entries.
	     */
            _isbtree_search(btree, keybuf1);
        
            if ((pkey = _isbtree_current(btree)) == NULL) {
		newcrp->flag = CRP_BEFOREANY;
	    }
	    else {
		newcrp->flag = CRP_AFTER;
		newcrp->recno = ldrecno(pkey + KEY_RECNO_OFF);
		memcpy( newcrp->key,pkey, pkeydesc2->k2_len);
	    }
	    break;

	default:
	    _isfatal_error("Invalid readmode");
	}

	*recnum = newcrp->recno;

	/*
	 * Build new curpos, deallocate old curpos data.
	 */
	_bytearr_free(curpos);
	*curpos = _bytearr_new((u_short)newcrpsize, (char *)newcrp);

	_isbtree_destroy(btree);
    }

    _amseterrcode(errcode, ISOK);

    /* Clean-up work. */
    if (newcrp != NULL)
	free(newcrp);

    _isdisk_commit();			     /* This will only check
					      * that we unfixed all fixed
					      * buffers */
    _isdisk_inval();

    _isam_exithook();
    return (ISOK);

 ERROR:

    if (btree != NULL)
	_isbtree_destroy(btree);

    /*
     * If error is EBADKEY, make the current position undefined.
     */
    if (errcode->iserrno == EBADKEY) {
	((Crp *)curpos->data)->flag = CRP_UNDEF;
    }

    /*
     * If error is ENOREC, switch to the new key, but set the current
     * record position undefined.
     */
    if (errcode->iserrno == ENOREC && newcrp != NULL) {
	_bytearr_free(curpos);
	*curpos = _bytearr_new((u_short)newcrpsize, (char *)newcrp);
	((Crp *)curpos->data)->flag = CRP_UNDEF;
    }

    if (newcrp != NULL)
	free((char *)newcrp);
    _isdisk_inval();

    _isam_exithook();
    return (ISERROR);
}


