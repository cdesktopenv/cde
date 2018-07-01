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
/*%%  $TOG: isamopen.c /main/5 1998/04/10 08:03:57 mgreess $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isamopen.c
 *
 * Description: _amopen()
 *	Open an ISAM file.
 *	
 *
 */

#include <stdlib.h>
#include "isam_impl.h"

/*
 * _amopen(isfname, varflag, minlen, maxlen, primkey, 
 *	    isfhandle, curpos, errcode)
 *
 * _amopen() opens an ISAM file.
 *
 * Input params:
 * isfname ISAM file name
 * 
 * Output params:
 * isfhandle a file handle to be used in subsequent operations on the file
 * varflag TRUE if file is for variable length records
 * minlen, maxlen minimum and maximum record length
 * curpos  initial current record position
 * errcode {iserrno, isstat1-4} 
 *
 * _amopen() returns 0 if successful, or -1 to indicate an error.
 */

/* ARGSUSED */
int
_amopen(char *isfname, enum openmode openmode, Bool *varflag,
        int *minlen, int *maxlen, Bytearray *isfhandle,
        Bytearray *curpos, struct errcode *errcode)
{
    Fcb			*fcb;
    Bytearray		isfhandle0;
    Crp			*crp;


    _isam_entryhook();

    /*
     * Make isfhandle0. 
     */

    isfhandle0 = _makeisfhandle(isfname);

    /*
     * Invalidate the FCB cache entry to avoid a problem found when using 
     * multiple servers.
     */
    if ((fcb = _mngfcb_find(&isfhandle0)) != NULL) {
	(void) _watchfd_decr(_isfcb_nfds(fcb));
	_isfcb_close(fcb);
	_mngfcb_delete(&isfhandle0);
    }

    /*
     * Get the FCB that corresponds to the isfhandle handle.
     */
    if ((fcb = _openfcb(&isfhandle0, errcode)) == NULL) {
	goto ERROR;
    }

    /* 
     * Check if the FCB allows writes if INOUT or OUTPUT is needed.
     */
    if (fcb->rdonly==TRUE && (openmode==OM_INOUT || openmode==OM_OUTPUT)) {
	_amseterrcode(errcode, EACCES);
	goto ERROR;
    }
    
    /*
     * Fill output parameters. 
     */
    *minlen = fcb->minreclen;
    *maxlen = fcb->maxreclen;
    *varflag = fcb->varflag;
    *isfhandle = isfhandle0;

    /*
     * Initial current record position.
     */
    if (FCB_NOPRIMARY_KEY(fcb)) {
	/* Use physical order. */
	crp = (Crp *) _ismalloc(sizeof(*crp));
	memset ((char *) crp, 0, sizeof(*crp));

	crp->keyid = PHYS_ORDER;
	crp->flag = CRP_BEFOREANY;

	curpos->length = sizeof(*crp);
	curpos->data = (char *) crp;
    }
    else {
	/* 
	 * Use primary key order. 
	 */

	crp = (Crp *) _ismalloc((unsigned)(sizeof(*crp) + fcb->keys[0].k2_len));
	memset((char *) crp, 0, (sizeof(*crp) + fcb->keys[0].k2_len)); 

	crp->keyid = fcb->keys[0].k2_keyid;
	crp->flag = CRP_BEFOREANY;

	_iskey_fillmin(&fcb->keys[0], crp->key);

	curpos->length = sizeof(*crp) + fcb->keys[0].k2_len;
	curpos->data = (char *) crp;
	
	/*
	 * Set full key length as the number of bytes to match in key comparison
	 */
	crp->matchkeylen = fcb->keys[0].k2_len - RECNOSIZE;
	
	if (ALLOWS_DUPS2(&fcb->keys[0]))
	    crp->matchkeylen -= DUPIDSIZE;
    }

    _isam_exithook();
    return (ISOK);

 ERROR:
    _bytearr_free(&isfhandle0);
    _isam_exithook();
    return (ISERROR);
}


/*
 * fcb = _openfcb(isfhandle, errcode)
 *
 * Try to locate FCB in the FCB cache. If not found, open the FCB and
 * insert it into the cache. 
 *
 * Return a pointer to the FCB, or NULL in the case of any error.
 */

#define FDNEEDED	3		     /* Needs 3 UNIX fd to open a file*/
Fcb *
_openfcb(Bytearray *isfhandle, struct errcode *errcode)
{
    Fcb			*fcb;
    Bytearray		*isfhandle2;
    char		errbuf[BUFSIZ];
    char		*errmsg0;

    if ((fcb = _mngfcb_find(isfhandle)) != NULL)
	goto out;

    /*
     * Check that there are UNIX file descriptors available.	
     */
    while (_watchfd_check() < FDNEEDED) {
	/*
	 * Find victim (LRU FCB) and close it.
	 */
	if((isfhandle2 = _mngfcb_victim()) == NULL) 
	    _isfatal_error ("_openfcb() cannot find LRU victim");

	fcb = _mngfcb_find(isfhandle2);
	(void) _watchfd_decr(_isfcb_nfds(fcb));
	_isfcb_close(fcb);
	_mngfcb_delete(isfhandle2);
    }

    /*
     * Open files, create FCB block.
     */
    if ((fcb = _isfcb_open(_getisfname(isfhandle), errcode)) == NULL) {
	return (NULL);
    }

    /*
     * Check that ISAM file is not corrupted (only check magic number).
     */
    if (_check_isam_magic(fcb) != ISOK) {
	_amseterrcode(errcode, EBADFILE);
	_isfcb_close(fcb);
	return (NULL);
    }

    /*
     * Read information from CNTL PAGE and store it in the FCB.
     */
    if (_isfcb_cntlpg_r(fcb) == ISERROR)
	_isfatal_error("_openfcb() cannot read CNTL PAGE");

    /*
     * Register UNIX file descriptors consumed.
     */
    (void) _watchfd_incr(_isfcb_nfds(fcb));

    /*
     * Insert new entry into the FCB cache.
     */
    _mngfcb_insert(fcb, isfhandle); 

 out:

    /*
     * Check that all file descriptors are open. This is needed to handle
     * user errors such as removing .ind or .var files.
     */
    if (fcb->varflag==TRUE && fcb->varfd == -1) {
	char *fmt = "%s.var has been removed";
	if (strlen(fmt) + strlen(fcb->isfname) + 1 >= BUFSIZ)
	  errmsg0 = (char*) malloc(strlen(fmt) + strlen(fcb->isfname) + 1);
	else
	  errmsg0 = errbuf;

	(void)sprintf(errmsg0, fmt, fcb->isfname);
	_isam_warning(errmsg0);
	_amseterrcode(errcode, EBADFILE);
	if (errmsg0 != errbuf) free(errmsg0);
	goto err;
    }

    if ((fcb->nkeys > 1 || !FCB_NOPRIMARY_KEY(fcb)) && fcb->indfd == -1) {
	char *fmt = "%s.ind has been removed";
	if (strlen(fmt) + strlen(fcb->isfname) + 1 >= BUFSIZ)
	  errmsg0 = (char*) malloc(strlen(fmt) + strlen(fcb->isfname) + 1);
	else
	  errmsg0 = errbuf;

	(void)sprintf(errmsg0, fmt, fcb->isfname);
	_isam_warning(errmsg0);
	_amseterrcode(errcode, EBADFILE);
	if (errmsg0 != errbuf) free(errmsg0);
	goto err;
    }

    return (fcb);

 err:
    /*
     * Delete FCB and remove it from FCB cache. Close UNIX fds.
     *
     * This is needed to recover netisamd server if users remove .ind or
     * or .var files.
     */
    (void) _watchfd_decr(_isfcb_nfds(fcb));
    _isfcb_close(fcb);
    _mngfcb_delete(isfhandle);

    return (NULL);

}

/*
 * isfname = _getisfname(isfhandle)
 *
 * Get ISAM file name from ISAM file handle.
 */

char *
_getisfname(Bytearray *isfhandle)
{
    return (isfhandle->data);
}


/*
 * isfhandle = _makeisfhandle(isfname)
 *
 * Make ISAM file handle.
 */

Bytearray 
_makeisfhandle(char *isfname)
{
    return (_bytearr_new((u_short)(strlen(isfname) + 1), isfname));
}

