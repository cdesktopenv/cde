/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isrename.c /main/3 1995/10/23 11:43:48 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isrename.c 1.10 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isrename.c
 *
 * Description:
 *	Rename an ISAM file. 
 */


#include "isam_impl.h"
#include <sys/time.h>

void _removelast();
static void _removelast2();
char *_lastelement();
static void _rename_datfile(), _rename_indfile(), _rename_varfile();
static int _amrename();

/*
 * isfd = isrename(oldname, newname)
 *
 *
 * Errors:
 *	EBADFILE ISAM file is corrupted or it is not an NetISAM file
 *	EFLOCKED The file is exclusively locked by other process.
 *	EFNAME	Invalid ISAM file name 
 *	EFNAME	ISAM file does not exist
 *	ETOOMANY Too many ISAM file descriptors are in use (128 is the limit)
 *
 * The following error code is "borrowed" from UNIX:
 *	EACCES	UNIX file system protection denies access to the file:
 *	         - mode is INOUT or OUTPUT and ISAM file is on 
 *	           a Read-Only mounted file system
 *		 - UNIX file permissions don't allow access to the file
 */

int 
isrename(oldname, newname)
    char		*oldname, *newname;
{
    Isfd		isfd, isfd2;
    Fab			*fab;
    char	  	olddir [MAXPATHLEN];
    char	  	newdir [MAXPATHLEN];
    char		datfname[MAXPATHLEN];

    /*
     * Check that the odl and new filename are in the same directory.
     */
    (void)strcpy(olddir, oldname);
    _removelast2(olddir);
    (void)strcpy(newdir, newname);
    _removelast2(newdir);

    if (strcmp(newdir, olddir) != 0) {
	_setiserrno2(EFNAME, 9, 0);
	return (ISERROR);
    }

    /*
     * Open the file
     */
    if ((isfd = isopen(oldname, ISINOUT)) == -1)
	return (ISERROR);		     /* iserrno is set */

    /*
     * Reject rename if 'newfile' exists.
     */
    if ((isfd2 = isopen(newname, ISINOUT)) != -1 || iserrno != ENOENT) {

	_setiserrno2(EEXIST, 9, 0);
	if (isfd2 >= 0) (void)isclose(isfd2);
	return (ISERROR);		     /* iserrno is set */
    }

    /*
     * Get File Access Block.
     */
    if ((fab = _isfd_find(isfd)) == NULL) {
	_isfatal_error("isrename() cannot find FAB");
        _setiserrno2(EFATAL,'9','0');
        return (ISERROR);
    }

    if (_amrename(&fab->isfhandle, _lastelement(newname), &fab->errcode)) {
	_seterr_errcode(&fab->errcode);
	(void)isclose(isfd);
	return (ISERROR);
    }

    _fab_destroy(fab);			     /* Deallocate Fab object */
    _isfd_delete(isfd);

    /*
     * We must unlink() the .rec file, or isbuild() with the same 
     * following immediatly would fail because for NFS files the client
     * still thinks that the file exists for a few seconds.
     */

    (void)strcpy(datfname, oldname);
    _makedat_isfname(datfname);
    (void)unlink(datfname);

    return (ISOK);			     /* Successful isrename() */
}

/*
 * _removelast2(path)
 *
 * Remove last element of path. E.g. /usr/db/part yields /usr/db.
 * Unlike _removelast() that path does not have to start with '/'.
 */

Static void
_removelast2(path)
    char		*path;
{
    register char	*p;

    for (p = path + strlen(path); *--p != '/' && p >= path; ) 
	*p = '\0';
}

/*
 * _amrename(isfhandle, newname)
 *
 * _amrename() renames ISAM file
 *
 * Input params:
 *	isfhandle	Handle of ISAM file
 *
 * Output params:
 *	errcode		Error code
 *
 */


static int
_amrename(isfhandle, newname, errcode)
    Bytearray		*isfhandle;
    char		*newname;
    struct errcode	*errcode;
{
    Fcb			*fcb;
    char		*isfname = _getisfname(isfhandle);

    _isam_entryhook();

    /*
     * Get FCB corresponding to the isfhandle handle.
     */
    if ((fcb = _openfcb(isfhandle, errcode)) == NULL) {
	goto ERROR;
    }

    /*
     * Delete FCB and remove it from FCB cache.
     */
    (void) _watchfd_decr(_isfcb_nfds(fcb));
    _isfcb_close(fcb);
    _mngfcb_delete(isfhandle);

    /*
     * Rename all UNIX files.
     */
    _rename_datfile(isfname, newname);
    _rename_indfile(isfname, newname);
    _rename_varfile(isfname, newname);

    _isam_exithook();
    return (ISOK);

 ERROR:

    _isam_exithook();
    return (ISERROR);
}


Static void
_rename_datfile(isfname, newname)
    char	*isfname;
    char	*newname;		     /* with no prefix */
{
    char	namebuf[MAXPATHLEN];
    char	newbuf[MAXPATHLEN];

    (void) strcpy(namebuf, isfname);
    (void) strcpy(newbuf, isfname);

    /*
     * Replace the last element of the old path with newname.
     */
    _removelast(newbuf);
    if (strcmp(newbuf, "/") != 0)
	(void) strcat(newbuf, "/");
    (void)strcat(newbuf, newname);

    _makedat_isfname(namebuf);
    _makedat_isfname(newbuf);

    (void)rename(namebuf, newbuf);
}

Static void
_rename_indfile(isfname, newname)
    char	*isfname;
    char	*newname;		     /* with no prefix */
{
    char	namebuf[MAXPATHLEN];
    char	newbuf[MAXPATHLEN];

    (void) strcpy(namebuf, isfname);
    (void) strcpy(newbuf, isfname);

    /*
     * Replace the last element of the old path with newname.
     */
    _removelast(newbuf);
    if (strcmp(newbuf, "/") != 0)
	(void) strcat(newbuf, "/");
    (void)strcat(newbuf, newname);

    _makeind_isfname(namebuf);
    _makeind_isfname(newbuf);

    (void)rename(namebuf, newbuf);
}

Static void
_rename_varfile(isfname, newname)
    char	*isfname;
    char	*newname;		     /* with no prefix */
{
    char	namebuf[MAXPATHLEN];
    char	newbuf[MAXPATHLEN];

    (void) strcpy(namebuf, isfname);
    (void) strcpy(newbuf, isfname);

    /*
     * Replace the last element of the old path with newname.
     */
    _removelast(newbuf);
    if (strcmp(newbuf, "/") != 0)
	(void) strcat(newbuf, "/");
    (void)strcat(newbuf, newname);

    _makevar_isfname(namebuf);
    _makevar_isfname(newbuf);

    (void)rename(namebuf, newbuf);
}

