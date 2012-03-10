/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isdelrec.c /main/3 1995/10/23 11:37:39 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isdelrec.c 1.8 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isdelrec.c
 *
 * Description:
 *	Delete a record in ISAM file. 
 */


#include "isam_impl.h"
#include <sys/time.h>

/*
 * err =  isdelrec(isfd, recnum)
 *
 * Isdelete() deletes a record from ISAM file. The record is identified 
 * by its record number. All indexes of the ISAM file are updated.
 *
 * Current record position is not changed.
 * isrecnum is set to recnum.
 *
 *
 * Returns 0 if successful, or -1 of any error.
 *
 * Errors:
 *	ELOCKED The file has been locked by another process.
 *	ENOTOPEN isfd does not correspond to an open ISAM file, or the
 *		ISAM file was not opened with ISINOUT mode.
 *	ENOREC	Record with record number recnum does not exist.
 */

int 
isdelrec(isfd, recnum)
    int			isfd;
    long		recnum;
{
    int			_amdelrec();
    register Fab	*fab;
    int			ret;

    /*
     * Get File Access Block.
     */
    if ((fab = _isfd_find(isfd)) == NULL) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    /*
     * Check that the open mode was ISINOUT.
     */
    if (fab->openmode != OM_INOUT) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    if ((ret = _amdelrec(&fab->isfhandle, recnum, &fab->errcode)) == ISOK) {
	isrecnum = recnum;		     /* Set isrecnum */
    }

    _seterr_errcode(&fab->errcode);

    return (ret);			     /* Successful write */
}
