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
/*%%  $XConsortium: ispageio.c /main/3 1995/10/23 11:42:51 rswiston $ 			 				 */
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * ispageio.c
 *
 * Description: 
 *	I/O functions for file pages.
 *	
 *
 */

#include "isam_impl.h"
#include <unistd.h>

/*
 * _isseekpg(fd, pgno)
 *
 * Set current file pointer to the page pgno.
 */

void
_isseekpg(int fd, Blkno pgno)
{
    long	offset = pgno * ISPAGESIZE;

    if (lseek(fd, offset, 0) != offset)
	_isfatal_error("lseek failed");
}

/*
 * _isreadpg(fd, buf)
 *
 * Read eon block from UNIX file into a buffer.
 */

void
_isreadpg(int fd, char *buf)
{
    if (read(fd, buf, ISPAGESIZE) != ISPAGESIZE)
	_isfatal_error("read failed");
}

/*
 * _iswritepg(fd, buf)
 *
 * Write one block to UNIX file.
 */

void
_iswritepg(int fd, char *buf)
{
    if (write(fd, buf, ISPAGESIZE) != ISPAGESIZE)
	_isfatal_error("write failed");
}
