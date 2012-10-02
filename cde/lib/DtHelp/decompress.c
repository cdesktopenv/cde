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
/* $XConsortium: decompress.c /main/4 1995/10/26 12:36:44 rswiston $ */
/* 
 * decompress - cat a compressed file
 *
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 */

/* #include "fontmisc.h" */
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "bufioI.h"

#define BITS	16

/*
 * a code_int must be able to hold 2**BITS values of type int, and also -1
 */
#if BITS > 15
typedef long int	code_int;
#else
typedef int		code_int;
#endif

typedef long int	  count_int;

#ifdef NO_UCHAR
 typedef char	char_type;
#else
 typedef	unsigned char	char_type;
#endif /* UCHAR */

static int magic_header[] = { 0x1F, 0x9D };

/* Defines for third byte of header */
#define BIT_MASK	0x1f
#define BLOCK_MASK	0x80
/* Masks 0x40 and 0x20 are free.  I think 0x20 should mean that there is
   a fourth header byte (for expansion).
*/

#define INIT_BITS 9			/* initial number of bits/code */

#ifdef COMPATIBLE		/* But wrong! */
# define MAXCODE(n_bits)	(1 << (n_bits) - 1)
#else
# define MAXCODE(n_bits)	((1 << (n_bits)) - 1)
#endif /* COMPATIBLE */

static code_int getcode();

/*
 * the next two codes should not be changed lightly, as they must not
 * lie within the contiguous general code space.
 */ 
#define FIRST	257	/* first free entry */
#define	CLEAR	256	/* table clear output code */

#define STACK_SIZE  8192

typedef struct _compressedFILE {
    BufFilePtr	    file;

    char_type	    *stackp;
    code_int	    oldcode;
    char_type	    finchar;

    int		block_compress;
    int		maxbits;
    code_int	maxcode, maxmaxcode;

    code_int	free_ent;
    int		clear_flg;
    int		n_bits;

    /* bit buffer */
    int		offset, size;
    char_type	buf[BITS];

    char_type	    de_stack[STACK_SIZE];
    char_type	    *tab_suffix;
    unsigned short  *tab_prefix;
} CompressedFile;


static	int hsize_table[] = {
    5003,	/* 12 bits - 80% occupancy */
    9001,	/* 13 bits - 91% occupancy */
    18013,	/* 14 bits - 91% occupancy */
    35023,	/* 15 bits - 94% occupancy */
    69001	/* 16 bits - 95% occupancy */
};

static int  BufCompressedFill(), BufCompressedSkip(), BufCompressedClose();

BufFilePtr
_DtHelpCeBufFilePushZ (BufFilePtr f)
{
    int		    code;
    int		    maxbits;
    int		    hsize;
    CompressedFile  *file;
    int		    extra;

    if ((BufFileGet(f) != magic_header[0]) ||
	(BufFileGet(f) != magic_header[1]))
    {
	return 0;
    }
    code = BufFileGet (f);
    maxbits = code & BIT_MASK;
    if (maxbits > BITS || maxbits < 12)
	return 0;
    hsize = hsize_table[maxbits - 12];
    extra = (1 << maxbits) * sizeof (char_type) +
	    hsize * sizeof (unsigned short);
    file = (CompressedFile *) malloc (sizeof (CompressedFile) + extra);
    if (!file)
	return 0;
    file->file = f;
    file->maxbits = maxbits;
    file->block_compress = code & BLOCK_MASK;
    file->maxmaxcode = 1 << file->maxbits;
    file->tab_suffix = (char_type *) &file[1];
    file->tab_prefix = (unsigned short *) (file->tab_suffix + file->maxmaxcode);
    /*
     * As above, initialize the first 256 entries in the table.
     */
    file->maxcode = MAXCODE(file->n_bits = INIT_BITS);
    for ( code = 255; code >= 0; code-- ) {
	file->tab_prefix[code] = 0;
	file->tab_suffix[code] = (char_type) code;
    }
    file->free_ent = ((file->block_compress) ? FIRST : 256 );
    file->clear_flg = 0;
    file->offset = 0;
    file->size = 0;
    file->stackp = file->de_stack;
    file->finchar = file->oldcode = getcode (file);
    if (file->oldcode != -1)
	*file->stackp++ = file->finchar;
    return _DtHelpCeBufFileCreate ((char *) file,
			  BufCompressedFill,
			  BufCompressedSkip,
			  BufCompressedClose);
}

static int
BufCompressedClose (
    BufFilePtr	f,
    int         doClose )
{
    CompressedFile  *file;
    BufFilePtr	    raw;

    file = (CompressedFile *) f->hidden;
    raw = file->file;
    free (file);
    _DtHelpCeBufFileClose (raw, doClose);
    return 1;
}

static int
BufCompressedFill (BufFilePtr f)
{
    CompressedFile  *file;
    register char_type *stackp, *de_stack;
    register char_type finchar;
    register code_int code, oldcode, incode;
    BufChar	    *buf, *bufend;

    file = (CompressedFile *) f->hidden;

    buf = f->buffer;
    bufend = buf + BUFFILESIZE;
    stackp = file->stackp;
    de_stack = file->de_stack;
    finchar = file->finchar;
    oldcode = file->oldcode;
    while (buf < bufend) {
	while (stackp > de_stack && buf < bufend)
	    *buf++ = *--stackp;

	if (buf == bufend)
	    break;

	if (oldcode == -1)
	    break;

	code = getcode (file);
	if (code == -1)
	    break;
    
    	if ( (code == CLEAR) && file->block_compress ) {
	    for ( code = 255; code >= 0; code-- )
	    	file->tab_prefix[code] = 0;
	    file->clear_flg = 1;
	    file->free_ent = FIRST - 1;
	    if ( (code = getcode (file)) == -1 )	/* O, untimely death! */
	    	break;
    	}
    	incode = code;
    	/*
     	 * Special case for KwKwK string.
     	 */
    	if ( code >= file->free_ent ) {
	    *stackp++ = finchar;
	    code = oldcode;
    	}
    
    	/*
     	 * Generate output characters in reverse order
     	 */
    	while ( code >= 256 )
    	{
	    *stackp++ = file->tab_suffix[code];
	    code = file->tab_prefix[code];
    	}
	finchar = file->tab_suffix[code];
	*stackp++ = finchar;
    
    	/*
     	 * Generate the new entry.
     	 */
    	if ( (code=file->free_ent) < file->maxmaxcode ) {
	    file->tab_prefix[code] = (unsigned short)oldcode;
	    file->tab_suffix[code] = finchar;
	    file->free_ent = code+1;
    	} 
	/*
	 * Remember previous code.
	 */
	oldcode = incode;
    }
    file->oldcode = oldcode;
    file->stackp = stackp;
    file->finchar = finchar;
    if (buf == f->buffer) {
	f->left = 0;
	return BUFFILEEOF;
    }
    f->bufp = f->buffer + 1;
    f->left = (buf - f->buffer) - 1;
    return f->buffer[0];
}

/*****************************************************************
 * TAG( getcode )
 *
 * Read one code from the standard input.  If BUFFILEEOF, return -1.
 * Inputs:
 * 	stdin
 * Outputs:
 * 	code or -1 is returned.
 */

static char_type rmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

static code_int
getcode(CompressedFile  *file)
{
    register code_int code;
    register int r_off, bits;
    register char_type *bp = file->buf;
    register BufFilePtr	raw;

    if ( file->clear_flg > 0 || file->offset >= file->size ||
	file->free_ent > file->maxcode )
    {
	/*
	 * If the next entry will be too big for the current code
	 * size, then we must increase the size.  This implies reading
	 * a new buffer full, too.
	 */
	if ( file->free_ent > file->maxcode ) {
	    file->n_bits++;
	    if ( file->n_bits == file->maxbits )
		file->maxcode = file->maxmaxcode;	/* won't get any bigger now */
	    else
		file->maxcode = MAXCODE(file->n_bits);
	}
	if ( file->clear_flg > 0) {
    	    file->maxcode = MAXCODE (file->n_bits = INIT_BITS);
	    file->clear_flg = 0;
	}
	bits = file->n_bits;
	raw = file->file;
	while (bits > 0 && (code = BufFileGet (raw)) != BUFFILEEOF)
	{
	    *bp++ = code;
	    --bits;
	}
	bp = file->buf;
	if (bits == file->n_bits)
	    return -1;			/* end of file */
	file->size = file->n_bits - bits;
	file->offset = 0;
	/* Round size down to integral number of codes */
	file->size = (file->size << 3) - (file->n_bits - 1);
    }
    r_off = file->offset;
    bits = file->n_bits;
    /*
     * Get to the first byte.
     */
    bp += (r_off >> 3);
    r_off &= 7;
    /* Get first part (low order bits) */
#ifdef NO_UCHAR
    code = ((*bp++ >> r_off) & rmask[8 - r_off]) & 0xff;
#else
    code = (*bp++ >> r_off);
#endif /* NO_UCHAR */
    bits -= (8 - r_off);
    r_off = 8 - r_off;		/* now, offset into code word */
    /* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
    if ( bits >= 8 ) {
#ifdef NO_UCHAR
	code |= (*bp++ & 0xff) << r_off;
#else
	code |= *bp++ << r_off;
#endif /* NO_UCHAR */
	r_off += 8;
	bits -= 8;
    }
    /* high order bits. */
    code |= (*bp & rmask[bits]) << r_off;
    file->offset += file->n_bits;

    return code;
}

static int
BufCompressedSkip (
    BufFilePtr	f,
    int		bytes)
{
    int		    c;
    while (bytes-- && ((c = BufFileGet(f)) != BUFFILEEOF))
	    ;
    return c;
}

int
_DtHelpCeUncompressFile(
  char *infile,
  char *outfile )
{
    BufFilePtr	    inputraw, input, output;
    int		    c;
    int             inFd, outFd;
    struct stat     statBuf;
    CECompressInfoPtr	myInfo;
    

    inFd = open(infile, O_RDONLY);
    if (inFd < 0) return -1;

    if (fstat(inFd, &statBuf) < 0)
	{
	close(inFd);
	return -1;
	}

    outFd = open(outfile, O_CREAT | O_WRONLY, 0666);
    if (outFd < 0)
	{
	close(inFd);
	return -1;
	}

    myInfo = (CECompressInfoPtr) malloc (sizeof(CECompressInfo));
    if (myInfo == NULL)
      {
	close(inFd);
	close(outFd);
	return -1;
      }

    myInfo->fd   = inFd;
    myInfo->size = statBuf.st_size;

    inputraw = _DtHelpCeBufFileRdRawZ (myInfo);
    input    = _DtHelpCeBufFilePushZ (inputraw);
    output   = _DtHelpCeBufFileOpenWr (outFd);
    while ((c = BufFileGet (input)) != -1)
	BufFilePut (c, output);

    c = myInfo->size;

    _DtHelpCeBufFileClose (input, TRUE);
    _DtHelpCeBufFileClose (output, TRUE);

    if (c != 0) /* didn't consume all the bytes in the file */
	return -1;

    return 0;
}
