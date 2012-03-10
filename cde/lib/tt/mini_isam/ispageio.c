/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: ispageio.c /main/3 1995/10/23 11:42:51 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)ispageio.c 1.3 89/07/17 Copyr 1988 Sun Micro";
#endif
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

/*
 * _isseekpg(fd, pgno)
 *
 * Set current file pointer to the page pgno.
 */

void
_isseekpg(fd, pgno)
    int		fd;
    Blkno	pgno;
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
_isreadpg(fd, buf)
    int		fd;
    char	*buf;
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
_iswritepg(fd, buf)
    int		fd;
    char	*buf;
{
    if (write(fd, buf, ISPAGESIZE) != ISPAGESIZE)
	_isfatal_error("write failed");
}
