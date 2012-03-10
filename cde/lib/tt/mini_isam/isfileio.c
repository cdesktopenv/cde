/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: isfileio.c /main/3 1995/10/23 11:39:26 rswiston $ 			 				 */
#ifndef lint
static char sccsid[] = "@(#)isfileio.c 1.3 89/07/17 Copyr 1988 Sun Micro";
#endif
/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */

/*
 * isfileio.c
 *
 * Description:
 *	I/O operations on UNIX files associated with FCB
 */

#include "isam_impl.h"

static int _getoffset();
static Blkno _getblkno();


/*
 * _cp_tofile(fcb, unixfd, data, pos, len)
 *
 * Copy data to a file associated with FCB (unixfd distinguishes among 
 * .rec, .ind., and .var file). The data are copied from buffer data to
 * the file at offset pos. The number of bytes to copy is in len.
 */

void
_cp_tofile(fcb, unixfd, data, pos, len)
    Fcb		*fcb;
    int		unixfd;
    char	*data;
    long	pos;
    int		len;
{
    int		offset;			     /* Offset within a page */
    Blkno	blkno;
    Bufhdr	*bufhdr;
    int		nbytes;

    /*
     * Data may span multiple blocks.
     */
    while (len > 0) {
	blkno = _getblkno(pos);
	offset = _getoffset(pos);
	nbytes = (len < ISPAGESIZE - offset) ? len : ISPAGESIZE - offset;

	bufhdr = _isdisk_fix(fcb, unixfd, blkno, ISFIXWRITE);
	memcpy( bufhdr->isb_buffer + offset,data, nbytes);
	_isdisk_unfix(bufhdr);

	pos  += nbytes;
	data += nbytes;
	len  -= nbytes;
    }
}

/*
 * _cp_fromfile(fcb, unixfd, data, pos, len)
 *
 * Copy data from a file associated with FCB (unixfd distinguishes among 
 * .rec, .ind., and .var file). The data are copied to buffer data from
 * the file at offset pos. The number of bytes to copy is in len.
 */

void
_cp_fromfile(fcb, unixfd, data, pos, len)
    Fcb		*fcb;
    int		unixfd;
    char	*data;
    long	pos;
    int		len;
{
    int		offset;			     /* Offset within a page */
    Blkno	blkno;
    Bufhdr	*bufhdr;
    int		nbytes;

    /*
     * Data may span multiple blocks.
     */
    while (len > 0) {
	blkno = _getblkno(pos);
	offset = _getoffset(pos);
	nbytes = (len < ISPAGESIZE - offset) ? len : ISPAGESIZE - offset;

	bufhdr = _isdisk_fix(fcb, unixfd, blkno, ISFIXREAD);
	memcpy( data,bufhdr->isb_buffer + offset, nbytes);
	_isdisk_unfix(bufhdr);

	pos  += nbytes;
	data += nbytes;
	len  -= nbytes;
    }
}

/*
 * extend_file(fcb, unixfd, oldsize)
 *
 * Extend UNIX file by one block.
 */

Blkno 
_extend_file(fcb, unixfd, oldsize)
    Fcb			*fcb;
    int			unixfd;
    Blkno		oldsize;
{
    Bufhdr		*bufhdr;
    char		buf[ISPAGESIZE];

    /* 
     * We must write something to the buffer, or we will get 
     * segmentation fault when using mapped I/O.
     */
    _isseekpg(unixfd, oldsize);
    _iswritepg(unixfd, buf);

    bufhdr = _isdisk_fix(fcb, unixfd, oldsize, ISFIXNOREAD); 
    _isdisk_unfix(bufhdr);

    return (oldsize + 1);
}

/*
 * off = _getoffset(pos)
 *
 * Calculate offset relative to the beginning of page
 */

Static int
_getoffset(pos)
    long		pos;
{
    return ((int)(pos % ISPAGESIZE));
}

/*
 * blkno = _getblkno(pos)
 *
 * Calculate block number of the block containing position pos.
 */

Static Blkno
_getblkno(pos)
    long		pos;
{
    return ((int)(pos / ISPAGESIZE));
}
