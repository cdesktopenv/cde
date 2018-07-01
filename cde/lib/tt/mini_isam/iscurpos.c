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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company                   */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.     */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.                    */
/*%%  (c) Copyright 1993, 1994 Novell, Inc.                              */
/*%%  $XConsortium: iscurpos.c /main/3 1995/10/23 11:37:08 rswiston $                                                       */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * @(#)iscurpos.c	1.2 95/01/06
 *
 * Description:
 *	Save and restore current record position functions.
 */

#include "isam_impl.h"
#include <sys/file.h>

/*
 * err = isgetcurpos(isfd, len, buf)
 *
 * Get current record position and save it in user buffer.
 *
 * The user buffer buf is filled with (if the information is N bytes):
 *   	buf[0], buf[1]		total length as u_short (set to N + 2)
 *	buf[2] .. buf[N + 1]	current record position information
 */

int 
isgetcurpos(int isfd, int *len, char **buf)
{
    Fab		*fab;
    u_short	total_len;

    /*
     * Get File Access Block.
     */
    if ((fab = _isfd_find(isfd)) == NULL) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    total_len = sizeof(total_len) + fab->curpos.length;

    if (*buf != NULL && *len < (int)total_len) {	
	_setiserrno2(E2BIG, '9', '0');
	return (ISERROR);
    }

    if (*buf == NULL) {
	*len = total_len;
	*buf = _ismalloc((unsigned int)total_len);
    }

    memcpy(*buf, (char *) &total_len, sizeof(total_len));
    memcpy(*buf+sizeof(total_len), fab->curpos.data, (int)fab->curpos.length);

    return (ISOK);
}

/*
 * err = issetcurpos(isfd, buf)
 *
 * Get current record position and save it in user buffer.
 */

int 
issetcurpos(int isfd, char *buf)
{
    Fab		*fab;
    u_short	len;

    /*
     * Get File Access Block.
     */
    if ((fab = _isfd_find(isfd)) == NULL) {
	_setiserrno2(ENOTOPEN, '9', '0');
	return (ISERROR);
    }

    memcpy((char *)&len, buf, sizeof(len));
    len -= sizeof (len);

    _bytearr_free(&fab->curpos);
    fab->curpos = _bytearr_new(len, buf + sizeof(len));


    return (ISOK);
}
