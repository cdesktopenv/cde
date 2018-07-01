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
/*%%  $XConsortium: iswrite.c /main/3 1995/10/23 11:46:15 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * iswrite.c
 *
 * Description:
 *	Write a record to ISAM file. 
 */


#include "isam_impl.h"
#include <sys/file.h>
#include <sys/time.h>

static int _am_write();

/*
 * err =  iswrite(isfd, record)
 *
 * Iswrite() adds a new record to an ISAM file. All indexes of the ISAM
 * file are updated.
 *
 * Current record position is not changed.
 * isrecnum is set to indicate the new record.
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
 *		ISAM file was opened with ISINPUT mode.
 *	EBADARG Unacceptable value of isreclen for variable length record.
 */

int 
iswrite(int isfd, char *record)
{
    int			_am_write();
    Fab	*fab;
    int			reclen;
    Recno		recnum;
    Bytearray		curpos;
    int			ret;

    /*
     * Get File Access Block.
     */
    if ((fab = _isfd_find(isfd)) == NULL) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    /*
     * Check that the open mode was ISOUTPUT, or ISINOUT.
     */
    if (fab->openmode != OM_OUTPUT && fab->openmode != OM_INOUT) {
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

    /*
     * Call the Access Method
     */
    curpos = _bytearr_dup(&fab->curpos);

    if ((ret = _am_write(fab, record, reclen, &curpos, &recnum)) == ISOK) {
	isrecnum = recnum;		     /* Set isrecnum */
    }
    _bytearr_free(&curpos);

    _seterr_errcode(&fab->errcode);

    return (ret);			     /* Successful write */
}


Static int _am_write(Fab *fab, char *record, int reclen,
                     Bytearray *curpos, Recno *recnum)
{
    return (_amwrite(&fab->isfhandle, record, reclen,
		     curpos, recnum, &fab->errcode));
}
