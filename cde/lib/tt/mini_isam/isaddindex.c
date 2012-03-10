/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isaddindex.c /main/3 1995/10/23 11:33:26 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isaddindex.c 1.8 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isaddindex.c
 *
 * Description:
 *	Add secondary index to ISAM file
 */


#include "isam_impl.h"
#include <sys/time.h>

/*
 * err = isaddindex(isfd, keydesc)
 *
 * Isaddindex() is used to add an alternate index to ISAM file. 
 *
 * Errors:
 *	EBADKEY	error in keydesc	
 *	EDUPL	there are duplicate keys and the keydesc does not allow
 *		duplicate keys
 *	EKEXISTS key with the same key descriptor already exists
 *	ENOTEXCL ISAM file is not open in exclusive mode
 *	ENOTOPEN the ISAM file is not open in ISINOUT mode.
 *	EACCES	Cannot create index file because of UNIX error.
 */

int 
isaddindex(isfd, keydesc)
    int			isfd;
    struct keydesc	*keydesc;
{
    int			_amaddindex();
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
     * Check that the open mode was ISOUTPUT
     */
    if (fab->openmode != OM_INOUT) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }
    
    ret = _amaddindex(&fab->isfhandle, keydesc, &fab->errcode);
    _seterr_errcode(&fab->errcode);

    return (ret);			     /* Successful write */
}
