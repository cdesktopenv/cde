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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: bufio.c /main/5 1996/11/01 10:11:35 drk $
 *
 * Copyright 1991 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Keith Packard, MIT X Consortium
 */


/* #include    "fontmisc.h" */
#include    <errno.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    "bufioI.h"

#include <X11/Xos.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif

#ifndef	MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define FileDes(f)		((int) (f)->hidden)
#define CompressFileDes(f)	(((CECompressInfoPtr) (f)->hidden)->fd)
#define CompressSize(f)		(((CECompressInfoPtr) (f)->hidden)->size)

static int
BufFileRawSkip (
    BufFilePtr	f,
    int		count )
{
    int	    curoff;
    int	    fileoff;
    int	    todo;

    curoff = f->bufp - f->buffer;
    fileoff = curoff + f->left;
    if (curoff + count <= fileoff) {
	f->bufp += count;
	f->left -= count;
    } else {
	todo = count - (fileoff - curoff);
	if (lseek (FileDes(f), todo, 1) == -1) {
	    if (errno != ESPIPE)
		return BUFFILEEOF;
	    while (todo) {
		curoff = BUFFILESIZE;
		if (curoff > todo)
		    curoff = todo;
		fileoff = read (FileDes(f), f->buffer, curoff);
		if (fileoff <= 0)
		    return BUFFILEEOF;
		todo -= fileoff;
	    }
	}
	f->left = 0;
    }
    return count;
}

static int
BufFileRawFlush (
    int		c,
    BufFilePtr	f )
{
    int	cnt;

    if (c != BUFFILEEOF)
	*f->bufp++ = c;
    cnt = f->bufp - f->buffer;
    f->bufp = f->buffer;
    f->left = BUFFILESIZE;
    if (write (FileDes(f), f->buffer, cnt) != cnt)
	return BUFFILEEOF;
    return c;
}

BufFilePtr
_DtHelpCeBufFileOpenWr (int fd)
{
    BufFilePtr	f;

    f = _DtHelpCeBufFileCreate ((char *) fd, BufFileRawFlush, NULL, _DtHelpCeBufFileFlush);
    f->bufp = f->buffer;
    f->left = BUFFILESIZE;
    return f;
}

#ifdef	obsolete_function
_DtHelpCeBufFileWrite (
    BufFilePtr	f,
    char	*b,
    int		n )
{
    int	    cnt;
    cnt = n;
    while (cnt--) {
	if (BufFilePut (*b++, f) == BUFFILEEOF)
	    return BUFFILEEOF;
    }
    return n;
}
#endif

int
_DtHelpCeBufFileFlush (BufFilePtr f, int doClose)
{
    if (f->bufp != f->buffer)
	(*f->io) (BUFFILEEOF, f);

    if (doClose)
	return (close (FileDes(f)));

    return 0;
}

/*****************************************************************************
 *			Private Routines
 *****************************************************************************/
/*****************************************************************************
 *			Routines working on a File descriptor
 *****************************************************************************/
static int
FdRawRead (BufFilePtr f)
{
    int	left;

    left = read (FileDes(f), f->buffer, BUFFILESIZE);
    if (left <= 0) {
	f->left = 0;
	return BUFFILEEOF;
    }
    f->left = left - 1;
    f->bufp = f->buffer + 1;
    return f->buffer[0];
}

static	int
FdClose (
    BufFilePtr	f,
    int         doClose)
{
    if (doClose)
	close (FileDes (f));
    return 1;
}

/*****************************************************************************
 *			Routines working on a Raw Compressed file
 *****************************************************************************/
static int
CompressRawRead (BufFilePtr f)
{
    int	left;


    left = read (CompressFileDes(f), f->buffer,
					MIN(CompressSize(f),BUFFILESIZE));
    if (left <= 0) {
	f->left = 0;
	CompressSize(f) = 0;
	return BUFFILEEOF;
    }
    CompressSize(f) -= left;
    f->left = left - 1;
    f->bufp = f->buffer + 1;
    return f->buffer[0];
}

static	int
CompressRawClose (
    BufFilePtr	f,
    int         doClose)
{
    if (doClose)
	close (CompressFileDes (f));
    free(f->hidden);
    return 1;
}

/*****************************************************************************
 *			Routines working on a Pipe
 *****************************************************************************/
/*****************************************************************************
 * Function:	int RdPipeStream (BufFilePtr f)
 *
 * Returns:
 *
 * Purpose:
 *
 *****************************************************************************/
static int
RdPipeStream (BufFilePtr f)
{
    int    left;

    left = fread(f->buffer, 1, BUFFILESIZE, FileStream(f));

    if (left <= 0)
      {
        f->left = 0;
        return BUFFILEEOF;
      }

    clearerr(FileStream(f));
    f->left = left - 1;
    f->bufp = f->buffer + 1;
    return f->buffer[0];
}

/*********************************************************************
 * Procedure: int ClosePipeStream (BufFilePtr f);
 *
 * Returns:
 *
 * Purpose:
 *
 ********************************************************************/
static int
ClosePipeStream (
    BufFilePtr  f,
    int         doClose)
{
    if (doClose)
        pclose(FileStream(f));

    return 1;
}

/*****************************************************************************
 *			Semi-Public Routines
 *****************************************************************************/
/*****************************************************************************
 * Function:	BufFilePtr _DtHelpCeBufFileCreate (char *hidden,
 *					int (*io)(), int (*skip)(),
 *					int (*close)())
 *
 * Returns:	A pointer to malloc'ed memory or NULL.
 *
 * Purpose:	Create a buffered i/o mechanism.
 *
 *****************************************************************************/
BufFilePtr
_DtHelpCeBufFileCreate (
    char    *hidden,
    int	    (*io)(),
    int	    (*skip)(),
    int	    (*close)() )
{
    BufFilePtr	f;

    f = (BufFilePtr) malloc (sizeof *f);
    if (!f)
	return 0;
    f->hidden = hidden;
    f->bufp = f->buffer;
    f->left = 0;
    f->io = io;
    f->skip = skip;
    f->close = close;
    return f;
}

/*****************************************************************************
 * Function:	BufFilePtr _DtHelpCeBufFileRdWithFd (int fd)
 *
 * Returns:	A pointer to malloc'ed memory or NULL.
 *
 * Purpose:	Create a buffered i/o mechanism using a file descriptor
 *		as private data and attaching a raw read to the i/o
 *		routine.
 *
 *****************************************************************************/
BufFilePtr
_DtHelpCeBufFileRdWithFd (int fd)
{
    return _DtHelpCeBufFileCreate ((char *) fd, FdRawRead, BufFileRawSkip, FdClose);
}

/*****************************************************************************
 * Function:	BufFilePtr _DtHelpCeBufFileRdRawZ (CECompressInfoPtr file)
 *
 * Returns:	A pointer to malloc'ed memory or NULL.
 *
 * Purpose:	Create a buffered i/o mechanism using a file descriptor
 *		as private data and attaching a raw read of compressed
 *		data to the i/o routine.
 *
 *****************************************************************************/
BufFilePtr
_DtHelpCeBufFileRdRawZ (CECompressInfoPtr file)
{
    return _DtHelpCeBufFileCreate ((char *) file, CompressRawRead, NULL,
							CompressRawClose);
}

/*****************************************************************************
 * Function:	void _DtHelpCeBufFileClose (BufFilePtr file, int doClose)
 *
 * Returns:	nothing
 *
 * Purpose:	Calls the close routine associated with the pointer.
 *		Frees the BufFile information.
 *
 *****************************************************************************/
void
_DtHelpCeBufFileClose (
    BufFilePtr	f,
    int         doClose )
{
    (void) (*f->close) (f, doClose);
    free (f);
}

/*****************************************************************************
 * Function:	void _DtHelpCeBufFileRd (BufFilePtr file, int doClose)
 *
 * Returns:	nothing
 *
 * Purpose:	Calls the close routine associated with the pointer.
 *		Frees the BufFile information.
 *
 *****************************************************************************/
int
_DtHelpCeBufFileRd (
    BufFilePtr	f,
    char	*b,
    int		n )
{
    int	    c, cnt;
    cnt = n;
    while (cnt--) {
	c = BufFileGet (f);
	if (c == BUFFILEEOF)
	    break;
	*b++ = c;
    }
    return n - cnt - 1;
}

/*****************************************************************************
 * Function:	BufFilePtr _DtHelpCeCreatePipeBufFile (FILE *stream)
 *
 * Returns:	A pointer to malloc'ed memory or NULL.
 *
 * Purpose:	Create a buffered i/o mechanism using a pipe descriptor
 *		as private data and attaching a raw read to the i/o
 *		routine.
 *
 *****************************************************************************/
BufFilePtr
_DtHelpCeCreatePipeBufFile (FILE *stream)
{
    return _DtHelpCeBufFileCreate ((char *) stream,
					RdPipeStream, NULL, ClosePipeStream);
}
