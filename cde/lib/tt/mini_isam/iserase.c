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
/*%%  $XConsortium: iserase.c /main/3 1995/10/23 11:38:08 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * iserase.c
 *
 * Description:
 *	Erase an ISAM file. 
 */


#include "isam_impl.h"
#include <unistd.h>
#include <sys/time.h>

static void _unlink_datfile(), _unlink_indfile(), _unlink_varfile();
static int _amerase();

/*
 * isfd = iserase(isfname, mode)
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
iserase(char *isfname)
{
    Isfd		isfd;
    Fab			*fab;

    /*
     * Open the file
     */
    if ((isfd = isopen(isfname, ISINOUT)) == -1)
	return (ISERROR);		     /* iserrno is set */

    /*
     * Get File Access Block.
     */
    if ((fab = _isfd_find(isfd)) == NULL) {
	_isfatal_error("iserase() cannot find FAB");
	_setiserrno2(EFATAL, '9', '0');
	return ISERROR;
    }

    if (_amerase(&fab->isfhandle, &fab->errcode)
	        && fab->errcode.iserrno != ENOENT) {
	_seterr_errcode(&fab->errcode); 
	(void)isclose(isfd);
	return (ISERROR);
    }

    _fab_destroy(fab);			     /* Deallocate Fab object */
    _isfd_delete(isfd);
    
    return (ISOK);			     /* Successful iserase() */
}

/*
 * _amerase(isfhandle)
 *
 * _amerase() erases ISAM file
 *
 * Input params:
 *	isfhandle	Handle of ISAM file
 *
 * Output params:
 *	errcode		Error code
 *
 */

static int
_amerase(Bytearray *isfhandle, struct errcode *errcode)
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
     * Unlink all UNIX files.
     */
    _unlink_datfile(isfname);
    _unlink_indfile(isfname);
    _unlink_varfile(isfname);

    _isam_exithook();
    return (ISOK);

 ERROR:

    _isam_exithook();
    return (ISERROR);
}


Static void
_unlink_datfile(char *isfname)
{
    char	namebuf[MAXPATHLEN];

    snprintf(namebuf, sizeof(namebuf), "%s", isfname);
    _makedat_isfname(namebuf);

    (void)unlink(namebuf);
}


Static void
_unlink_indfile(char *isfname)
{
    char	namebuf[MAXPATHLEN];

    snprintf(namebuf, sizeof(namebuf), "%s", isfname);
    _makeind_isfname(namebuf);

    (void)unlink(namebuf);
}


Static void
_unlink_varfile(char *isfname)
{
    char	namebuf[MAXPATHLEN];

    snprintf(namebuf, sizeof(namebuf), "%s", isfname);
    _makevar_isfname(namebuf);

    (void)unlink(namebuf);
}
