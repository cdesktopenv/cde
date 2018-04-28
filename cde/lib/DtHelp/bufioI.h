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
/* $XConsortium: bufioI.h /main/5 1995/10/26 12:36:33 rswiston $ */
#ifndef	_DtHelpbufioI_h
#define	_DtHelpbufioI_h

#include <stdio.h>		/* for FILE */

#ifndef	NULL
#define	NULL	0
#endif

#define BUFFILESIZE	4096
#define BUFFILEEOF	-1

typedef unsigned char BufChar;

typedef struct _buffile {
    BufChar *bufp;
    int	    left;
    BufChar buffer[BUFFILESIZE];
    int	    (*io)(/* BufFilePtr f */);
    int	    (*skip)(/* BufFilePtr f, int count */);
    int	    (*close)(/* BufFilePtr f */);
    char    *hidden;
} BufFileRec, *BufFilePtr;

typedef	struct _compressInfo{
    int     fd;
    int     size;
} CECompressInfo, *CECompressInfoPtr;

extern BufFilePtr   __DtBufFileCreate ();
extern BufFilePtr   _DtHelpCeBufFilePushZ ();
extern BufFilePtr   _DtHelpCeBufFileOpenWr ();
extern int	    _DtHelpCeBufFileFlush ();
#define BufFileGet(f)	((f)->left-- ? *(f)->bufp++ : (*(f)->io) (f))
#define BufFilePut(c,f)	(--(f)->left ? *(f)->bufp++ = (c) : (*(f)->io) (c,f))
#define BufFilePutBack(c,f) { (f)->left++; *(--(f)->bufp) = (c); }
#define BufFileSkip(f,c)    ((*(f)->skip) (f, c))

#define FileStream(f)   ((FILE *)(f)->hidden)

extern	void		_DtHelpCeBufFileClose (
				BufFilePtr	f,
				int		doClose);
extern	BufFilePtr	_DtHelpCeBufFileCreate (
				char		*hidden,
				int		(*io)(),
				int		(*skip)(),
				int		(*close)());
extern	int		_DtHelpCeBufFileRd (
				BufFilePtr	 f,
				char		*buffer,
				int		 request_size);
extern	BufFilePtr	_DtHelpCeBufFileRdWithFd (
				int		fd);
extern	BufFilePtr	_DtHelpCeBufFileRdRawZ (
				CECompressInfoPtr  file_info);
extern	BufFilePtr	_DtHelpCeCreatePipeBufFile (
				FILE		*stream);
extern	int		_DtHelpCeUncompressFile (
				char		*in_file,
				char		*out_file);

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif /* _DtHelpbufioI_h */
