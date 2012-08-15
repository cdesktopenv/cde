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
/* $XConsortium: oakfuty.c /main/12 1996/11/11 10:55:57 drk $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include  <sys/types.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <errno.h>
#ifndef SVR4
#if !defined( SYSV )
#include <sys/resource.h>
#endif
#ifdef __osf__
#define _BSD
#endif
#include  <sys/wait.h>
#ifdef __osf__
#undef _BSD
#endif
#else
#include  <sys/mman.h>
#include  <wait.h>
#endif

#include  <unistd.h>
#include  <sys/stat.h>
#include  <signal.h>
#include  <fcntl.h>

#include  "bdfgpf.h"
#include  "udcutil.h"

static	void	exline();

char	*oakgtobdf;
char	*bdftosnf = BDFTOSNF;
char	*bdftopcf;

static	int	put_file_create_err_msg = 0;

#define	DEVICE_FAIL		2
#define	WRITE_FAIL		3
#define	OPEN_FAIL		4
#define	READ_FAIL		5

int
#if NeedFunctionPrototypes
ReadBdfHeader(
    struct btophead *head,
    char	*buf
)
#else
ReadBdfHeader( head, buf )
struct btophead *head;
char	*buf;
#endif
{
	char *p;
	unsigned int	getstat = 0;

	fgets(buf, BUFSIZE, head->input);
	p = buf;
	SCAN_TO_NONSP(p)

	if (strncmp(p, STARTFONT, STARTFONTsz)) {
		return(BDF_INVAL);
	}

	while( 1 ) {
		if (fgets(buf, BUFSIZE, head->input) == NULL) {
			return (BDF_INVAL);
		}
		p = buf;
		SCAN_TO_NONSP(p)
		    if (!strncmp(p, SIZE, SIZEsz)) {
			if ((sscanf(p, "SIZE %f%d",
			    &(head->bdf_point), &(head->bdf_xdpi))) != 2) {
				return(BDF_INVAL);
			}
			getstat |= 0x01;
		} else if (!strncmp(p, FONTBOUNDINGBOX, FONTBOUNDINGBOXsz)) {
			if (sscanf(p, "FONTBOUNDINGBOX %d%d%d%d",
				&(head->bdf_width), &(head->bdf_height), 
				&(head->bdf_x), &(head->bdf_y)) != 4) {
				return	BDF_INVAL;
			}
			getstat |= 0x02;

		} else if (!strncmp(p, CHARS, CHARSsz)) {
			if ((sscanf(p, "CHARS %d", &(head->num_chars))) != 1) {
				return(BDF_INVAL);
			}
			getstat |= 0x04;
		} else if (!strncmp(p, STARTCHAR, STARTCHARsz)) {
			break;
		}
	}

	if (getstat != 0x07) {
		return(BDF_INVAL);
	}
	return	0;
}


int
#if NeedFunctionPrototypes
ReadGpfHeader(
    struct ptobhead *head,
    char   *buf
)
#else
ReadGpfHeader(head, buf)
struct ptobhead *head;
char   *buf;
#endif
{
	char *p;
	unsigned int  getstat = 0;

	while(1) {
		if (getstat == 0x07) break;

		if (fgets(buf, BUFSIZE, head->input) == NULL) {
			return	GPF_INVAL;
		}
		p = buf;
		SCAN_TO_NONSP(p)

		    if (!strncmp(p, NUMFONTS, NUMFONTSsz)) {
			if (sscanf(p, "numfonts:%d", &(head->num_chars)) != 1) {
				return(GPF_INVAL);
			}
			getstat |= 0x01;
		} else if (!strncmp(p, WIDTH, WIDTHsz)) {
			if (sscanf(p, "width:%d", &(head->p_width)) != 1) {
				return(GPF_INVAL);
			}
			getstat |= 0x02;
		} else if (!strncmp(p, HEIGHT, HEIGHTsz)) {
			if (sscanf(p, "height:%d", &(head->p_height)) != 1) {
				return(GPF_INVAL);
			}
			getstat |= 0x04;
		}
	}
	return	0;
}


int
#if NeedFunctionPrototypes
WriteBdfHeader( struct ptobhead *head )
#else
WriteBdfHeader(head)
struct ptobhead *head;
#endif
{
	FILE	*fp;
	int		fd[2];
	unsigned int		getstat;
	char  buf[BUFSIZE], *p;

	pid_t	chld_pid = 0;
#if defined( SVR4 ) || defined( SYSV ) || defined(__FreeBSD__)
	int	chld_stat ;
#else
	union	wait	chld_stat ;
#endif


	if (head->snf_file != NULL) {
		if (pipe(fd) != 0) {
			return	PIPE_ERROR;
		}
		switch (chld_pid = fork()) {
		case	0:
			close(1);
			if(dup(fd[1]) < 0) {
				return(DUP_ERROR);
			}
			close(fd[0]);
			close(fd[1]);
			execl( oakgtobdf, oakgtobdf, head->snf_file, 0);

			return	EXEC_ERROR;

		case	-1:
			return(FORK_ERROR);

		default:
			break;
		}
		close(fd[1]);
		if((fp = (FILE *)fdopen(fd[0], "r")) == NULL) {
			close( fd[0] );
			kill( chld_pid, SIGKILL );
			WaitID( chld_pid, chld_stat ) ;
			return	FDOPEN_ERROR;
		}
	} else {
		if ((fp = fopen(head->bdf_file, "r")) == NULL) {
			return(BDF_OPEN_HEAD);
		}
	}

	getstat = 0;

	while ( 1 ) {
		if (fgets(buf, BUFSIZE, fp) == NULL) {
		    fclose( fp );
		    if (head->snf_file != NULL) {
			kill( chld_pid, SIGKILL );
			WaitID( chld_pid, chld_stat ) ;
		    }
		    return(BDF_INVAL);
		}
		p = buf;
		SCAN_TO_NONSP(p);

		if (!strncmp(p, CHARS, CHARSsz)) {
		    fclose( fp );
		    if (head->snf_file != NULL) {
			kill( chld_pid, SIGKILL );
			WaitID( chld_pid, chld_stat ) ;
		    }
		    break;
		}

		fprintf(head->output, "%s", buf);

		if (!strncmp(p, SIZE, SIZEsz)) {
		    if ((sscanf(p, "SIZE %f%d",
			&(head->bdf_point), &(head->bdf_xdpi))) != 2) {
			fclose(fp);
			if (head->snf_file != NULL) {
			    kill( chld_pid, SIGKILL );
			    WaitID( chld_pid, chld_stat ) ;
			}
			return(BDF_INVAL);
		    }
		    getstat |= 0x01;
		    continue;
		}
		if (!strncmp(p, FONTBOUNDINGBOX, FONTBOUNDINGBOXsz)) {
		    if ( sscanf( p, "FONTBOUNDINGBOX %d%d%d%d", 
						    &(head->bdf_width), 
						    &(head->bdf_height), 
						    &(head->bdf_x), 
						    &(head->bdf_y)) != 4
		    ) {
			fclose(fp);
			if (head->snf_file != NULL) {
			    kill( chld_pid, SIGKILL );
			    WaitID( chld_pid, chld_stat ) ;
			}
			return	BDF_INVAL;
		    }
		    getstat |= 0x02;
		    continue;
		}
		get_charset_registry(head, p) ;	
	}


	if (getstat != 0x03) {
		return	BDF_INVAL;
	}

	return	0;
}


void
#if NeedFunctionPrototypes
WriteGpfHeader( struct btophead *head )
#else
WriteGpfHeader(head)
struct btophead *head;
#endif
{
	fprintf(head->output, "numfonts:%d\n", head->num_chars);
	fprintf(head->output, "width:%d\n",    head->p_width  );
	fprintf(head->output, "height:%d\n",   head->p_height );
}




int
#if NeedFunctionPrototypes
WritePtnToBdf( struct ptobhead *head )
#else
WritePtnToBdf(head)
struct ptobhead *head;
#endif
{
	int     msize, swidth, rtn, i, nchar;
	char    *zoomptn;
	int	bbw, bbh, bbx, bby, dw ;
	char	glyph_name[BUFSIZE] ;

	nchar = head->num_chars;
	put_default_chars(head, nchar, rtn) ;

	if (head->zoomf) {
		msize = ((head->bdf_width + 7) / 8) * head->bdf_height;
		if ((zoomptn = (char *)malloc(msize)) == NULL) {
			return(MALLOC_ERROR);
		}
	}

	for (i=0; i<head->num_chars; i++) {
	    sprintf( glyph_name, "%x", head->code[i]);
	    swidth = (head->bdf_width * 72270)/((int)(head->bdf_point * (float)head->bdf_xdpi));
	    dw  = head->bdf_width ;
	    bbw = head->bdf_width ;
	    bbh = head->bdf_height ;
	    bbx = head->bdf_x ;
	    bby = head->bdf_y ;

	    fprintf(head->output, "STARTCHAR %s\n", glyph_name );
	    fprintf(head->output, "ENCODING %d\n", head->code[i]);
	    fprintf(head->output, "SWIDTH %d 0\n", swidth );
	    fprintf(head->output, "DWIDTH %d 0\n", dw );
	    fprintf(head->output, "BBX %d %d %d %d\n", bbw, bbh, bbx, bby );
	    fprintf(head->output, "BITMAP\n");

	    if (head->zoomf) {
		    if ((rtn = PtnZoom(zoomptn, head->ptn[i], head->p_width,
			head->p_height, head->bdf_width, head->bdf_height))) {
			    return(rtn);
		    }
		    if( (rtn = PutBdfPtn(zoomptn, head->bdf_width,
			head->bdf_height, head->output))) {
			    return(rtn);
		    }
	    } else {
		    if( (rtn = PutBdfPtn(head->ptn[i],
			head->bdf_width, head->bdf_height, head->output))) {
			    return(rtn);
		    }
	    }
	    fprintf(head->output, "ENDCHAR\n");
	    free(head->ptn[i]);
	}
	fprintf(head->output, "ENDFONT\n");
	return(0);
}




int
#if NeedFunctionPrototypes
putDefaultChars( struct ptobhead *head )
#else
putDefaultChars(head )
struct ptobhead *head;
#endif
{
	int     swidth, bytew, i, j;
	unsigned int     posbit ;
	unsigned char    *ptnbuf, *ptnbuf2 ;
	unsigned char    tmp;

	put_default_encode( head ) ;
	swidth = (head->bdf_width * 72270)/((int)(head->bdf_point * (float)head->bdf_xdpi));
	fprintf(head->output, "SWIDTH %d 0\n", swidth);
	fprintf(head->output, "DWIDTH %d 0\n", head->bdf_width);
	fprintf( head->output, "BBX %d %d %d %d\n", head->bdf_width, head->bdf_height, head->bdf_x, head->bdf_y );
	fprintf(head->output, "BITMAP\n");

	bytew = (head->bdf_width + 7) / 8;
	if ((ptnbuf = (unsigned char *)malloc(bytew * 2)) == NULL) {
		return(MALLOC_ERROR);
	}
	put_default_bitmap(head, bytew, posbit, i, j, ptnbuf, ptnbuf2, tmp) ;

	fprintf(head->output, "ENDCHAR\n");
	free(ptnbuf);
	return	0;
}


int
#if NeedFunctionPrototypes
WritePtnToGpf( struct btophead *head )
#else
WritePtnToGpf(head)
struct btophead *head;
#endif
{
	int     msize, rtn, i;
	char    *zoomptn;


	if (head->zoomf) {
		msize = ((head->p_width + 7) / 8) * head->p_height;
		if ((zoomptn = (char *)malloc(msize)) == NULL) {
			return(MALLOC_ERROR);
		}
	}

	for (i=0; i<head->num_chars; i++) {

		if ( head->code[i] > 0xff ) {
			fprintf(head->output, "code:0x%x\n", SHIFT_ON(head->code[i]) );
		} else {
			fprintf(head->output, "code:0x%x\n", head->code[i]);
		}
		if (head->zoomf) {
			if ((rtn = PtnZoom(zoomptn, head->ptn[i], head->bdf_width,
			    head->bdf_height, head->p_width, head->p_height))) {
				return(rtn);
			}
			if ((rtn = PutGpfPtn(zoomptn, head->p_width,
			    head->p_height, head->output))) {
				return(rtn);
			}
		} else {
			if ((rtn = PutGpfPtn(head->ptn[i],
			    head->bdf_width, head->bdf_height, head->output))) {
				return(rtn);
			}
		}
		free(head->ptn[i]);
	}
	fprintf(head->output, "enddata\n");
	return(0);
}


int
#if NeedFunctionPrototypes
ReadBdfToMemory(
    struct btophead *head,
    char   *buf
)
#else
ReadBdfToMemory(head, buf)
struct btophead *head;
char   *buf;
#endif
{
	int	    code, mwidth, num_char, bsize, rtn;
	char    *ptn;

	num_char = 0;
	mwidth = (head->bdf_width + 7) / 8;
	bsize = mwidth * head->bdf_height;
	while(1) {
		if ((rtn = GetBdfCode(head, buf, &code)) < 0) {
			return(rtn);	/* contain BDF_INVAL */
		} else if (rtn == FILE_END) {
			head->num_chars = num_char;
			break;
		}

		if ( ( code > head->end_code )
			|| ( code < head->start_code )
			|| ( !IN_CODE( head->code_category, SHIFT_ON( code ) ) )
			|| COMM_ISDEFAULTCHAR( code )
		) {
			continue;
		}
		head->code[num_char] = code;
		if ( ( ptn = head->ptn[num_char++] = (char *)malloc( bsize ) ) == NULL ) {
			return(MALLOC_ERROR);
		}

		if ((rtn = GetBdfPtn(head, buf, ptn, mwidth, bsize)) != 0) {
			return(rtn);
		}
	}
	return(0);
}


int
#if NeedFunctionPrototypes
ReadBdfToMemory_with_init(
    struct btophead *head,
    int	init_start,
    int	init_end,
    char   *buf
)
#else
ReadBdfToMemory_with_init(head, init_start, init_end, buf)
struct btophead *head;
int	init_start;
int	init_end;
char   *buf;
#endif
{
	int	    code, mwidth, num_char, bsize, rtn;
	char    *ptn;

	num_char = 0;
	mwidth = (head->bdf_width + 7) / 8;
	bsize = mwidth * head->bdf_height;
	while(1) {
		if ((rtn = GetBdfCode(head, buf, &code)) < 0) {
			return(rtn);	/* contain BDF_INVAL */
		} else if (rtn == FILE_END) {
			head->num_chars = num_char;
			break;
		}

		if ( ( code > head->end_code )
			|| ( code < head->start_code )
			|| ( !IN_CODE( head->code_category, SHIFT_ON( code ) ) )
			|| ( ( code >= init_start ) && ( code <= init_end ) )
			|| COMM_ISDEFAULTCHAR( code )
		) {
			continue;
		}

		head->code[num_char] = code;
		if ((ptn = head->ptn[num_char++] =
		    (char *)malloc(bsize)) == NULL) {
			return(MALLOC_ERROR);
		}

		if ((rtn = GetBdfPtn(head, buf, ptn, mwidth, bsize)) != 0) {
			return(rtn);
		}
	}
	return(0);
}


int
#if NeedFunctionPrototypes
GetBdfCode(
    struct btophead *head,
    char   *buf,
    int    *code
)
#else
GetBdfCode(head, buf, code)
struct btophead *head;
char   *buf;
int    *code;
#endif
{
	char *p;

	while(1) {
		if (fgets(buf, BUFSIZE, head->input) == NULL) {
			return (BDF_INVAL);
		}
		p = buf;
		SCAN_TO_NONSP(p)
		    if (!strncmp(p, ENDFONT, ENDFONTsz)) {
			return(FILE_END);
		}

		if (!strncmp(p, ENCODING, ENCODINGsz)) {
			if ((sscanf(p, "ENCODING %d", code)) != 1) {
				return(BDF_INVAL);
			}
			break;
		}
	}
	return(0);
}


int
#if NeedFunctionPrototypes
GetBdfPtn(
    struct btophead *head,
    char   *buf,
    char   *ptn,
    int    mwidth,
    int    bsize
)
#else
GetBdfPtn(head, buf, ptn, mwidth, bsize)
struct btophead *head;
char   *buf;
char   *ptn;
int    mwidth;
int    bsize;
#endif
{
	int	    skip, i, j;
	char    *p;

	while(1) {
		if (fgets(buf, BUFSIZE, head->input) == NULL) {
			return (BDF_INVAL);
		}
		p = buf;
		SCAN_TO_NONSP(p)
		    if (!strncmp(p, BITMAP, BITMAPsz)) {
			break;
		}
	}

	ptn[bsize - 1] = NULL;

	for (i=0, skip=0; i<head->bdf_height; i++) {
		if (skip) {
			for(j=0; j<mwidth; j++) {
				ptn[j] = NULL;
			}
			ptn += mwidth;
			continue;
		}
		if (fgets(buf, BUFSIZE, head->input) == NULL) {
			return (BDF_INVAL);
		}
		p = buf;
		SCAN_TO_NONSP(p);
		if (!strncmp(p, ENDCHAR, ENDCHARsz)) {
			skip = 1;
			for(j=0; j<mwidth; j++) {
				ptn[j] = NULL;
			}
			ptn += mwidth;
			continue;
		}
		GetBdfLinePtn(ptn, buf, head->bdf_width);
		ptn += mwidth;
	}
	return(0);
}


int
#if NeedFunctionPrototypes
ReadGpfToMemory(
    struct ptobhead *head,
    char   *buf
)
#else
ReadGpfToMemory(head, buf)
struct ptobhead *head;
char   *buf;
#endif
{
	int	    code, mwidth, num_char, bsize, rtn;
	char    *ptn;

	num_char = 0;
	mwidth = (head->p_width + 7) / 8;
	bsize = mwidth * head->p_height;
	while(1) {
		if ((rtn = GetGpfCode(head, buf, &code)) < 0) {
			return(rtn);	/* contain GPF_INVAL */
		} else if (rtn == FILE_END) {
			head->num_chars = num_char;
			break;
		}
		if ( ( code > head->end_code ) 
			|| (code < head->start_code ) 
			|| ( !IN_CODE( head->code_category, SHIFT_ON( code ) ) )
			|| COMM_ISDEFAULTCHAR( code )
		) {
			continue;
		}
		head->code[num_char] = code;
		if ((ptn = head->ptn[num_char++] =
		    (char *)malloc(bsize)) == NULL) {
			return(MALLOC_ERROR);
		}

		if ((rtn = GetGpfPtn(head, buf, ptn, mwidth, bsize)) != 0) {
			return(rtn);
		}
	}
	return(0);
}


int
#if NeedFunctionPrototypes
GetGpfCode(
    struct ptobhead *head,
    char   *buf,
    int    *code
)
#else
GetGpfCode(head, buf, code)
struct ptobhead *head;
char   *buf;
int    *code;
#endif
{
	char    *p;

	while(1) {
		p = buf;
		SCAN_TO_NONSP(p)

		    if (!strncmp(p, ENDDATA, ENDDATAsz)) {
			return(FILE_END);
		}
		if (!strncmp(p, CODE, CODEsz)) {
			*code = (int)strtol(buf+CODEsz, NULL, 0);
			CONVGLYPHINDEX( *code ) ;
			break;
		}

		if (fgets(buf, BUFSIZE, head->input) == NULL) {
			return (GPF_INVAL);
		}
	}
	return(0);
}


int
#if NeedFunctionPrototypes
GetGpfPtn(
    struct ptobhead *head,
    char    *buf,
    char    *ptn,
    int	mwidth,
    int	bsize
)
#else
GetGpfPtn(head, buf, ptn, mwidth, bsize)
struct ptobhead *head;
char    *buf;
char    *ptn;
int	mwidth;
int	bsize;
#endif
{
	int     skip, i, j;
	char    *p;

	for (i=0, skip=0; i<head->p_height; i++) {
		if (skip) {
			for (j=0; j<mwidth; j++) {
				ptn[j] = NULL;
			}
			ptn += mwidth;
			continue;
		}
		if (fgets(buf, BUFSIZE, head->input) == NULL) {
			return (GPF_INVAL);
		}
		p = buf;
		SCAN_TO_NONSP(p);
		if ((!strncmp(p, CODE, CODEsz)) ||
		    (!strncmp(p, ENDDATA, ENDDATAsz))) {
			skip = 1;
			for (j=0; j<mwidth; j++) {
				ptn[j] = NULL;
			}
			ptn += mwidth;
			continue;
		}

		GetGpfLinePtn(ptn, buf, head->p_width);
		ptn += mwidth;
	}
	return(0);
}


void
#if NeedFunctionPrototypes
GetBdfLinePtn(
    char	*mem,
    char	*buf,
    int	width
)
#else
GetBdfLinePtn(mem, buf, width)
char	*mem;
char	*buf;
int	width;
#endif
{
	int	i, iend, len;
	char   *p, str[3];

	str[2] = NULL;

	SCAN_TO_NONSP(buf);

	iend = (width + 3) / 4;

	if ((len = strlen(buf)) < iend) {
		p = buf+len;
		for (i=0; i<(iend-len); i++) {
			*p++ = '0';
		}
		*p ='\0';
	}

	for (i=0; i<iend/2; i++) {
		str[0] = *buf++;
		str[1] = *buf++;
		*mem++ = (char)strtol(str, NULL, 16);
	}
	if (iend%2) {
		str[0] = *buf;
		str[1] = NULL;
		*mem = (char)strtol(str, NULL, 16) << 4;
	}
}

void
#if NeedFunctionPrototypes
GetGpfLinePtn(
    char	*mem,
    char	*buf,
    int	width
)
#else
GetGpfLinePtn(mem, buf, width)
char	*mem;
char	*buf;
int	width;
#endif
{
	unsigned int	   skip, i, iend, j;
	unsigned char   ptn;

	SCAN_TO_NONSP(buf);

	iend = (width + 7) / 8;

	for (i=0, skip=0; i<iend; i++) {
		if (skip) {
			*mem++ = NULL;
			continue;
		}
		for (j=0, ptn = NULL; j<8; j++) {
			if ((*buf == '\n') || (*buf == NULL)) {
				skip = 1;
				ptn <<= (8-j);
				break;
			}
			if (j) ptn <<= 1;
			if (*buf == '0') {
				ptn |= 1;
			}
			buf++;
		}
		*mem++ = ptn;
	}
}




int
#if NeedFunctionPrototypes
PutBdfPtn(
    unsigned char	*ptn,
    int	width,
    int	height,
    FILE	*fp
)
#else
PutBdfPtn(ptn, width, height, fp)
unsigned char	*ptn;
int	width;
int	height;
FILE	*fp;
#endif
{
	int	    i, j, nbyte ;
	unsigned char    *pbuf, x, c;
	static unsigned char    *buf=NULL;

	if (buf==NULL) {
		buf = (unsigned char *)malloc(width*height+2);
		if ( buf == NULL) {
			return(MALLOC_ERROR);
		}
	}
	nbyte = (width + 7) / 8;

	pbuf=buf;
	for(i=0; i<height; i++) {
		for (j=0 ; j<nbyte; j++) {
			x = *ptn >> 4;
			c = (x>=10)? 'a'-0xa : '0';
			*pbuf++ = c + x;
			x = *ptn++ & 0x0f;
			c = (x>=10)? 'a'-0xa : '0';
			*pbuf++ = c + x;
		}
		*pbuf++ = '\n';
	}
	*pbuf = '\0';
	fprintf(fp, "%s", buf);
	return(0);
}




int
#if NeedFunctionPrototypes
PutGpfPtn(
    char	*ptn,
    int	width,
    int	height,
    FILE	*fp
)
#else
PutGpfPtn(ptn, width, height, fp)
char	*ptn;
int	width;
int	height;
FILE	*fp;
#endif
{
	int	    i, j, k, nbyte, tw;
	unsigned char    p, *pbuf;
	static unsigned char    *buf=NULL;

	if (buf==NULL) {
	    buf = (unsigned char *)malloc(((width+1)*height)+1);
	    if ( buf == NULL) {
		return(MALLOC_ERROR);
	    }
	}

	nbyte = (width + 7) / 8;

	pbuf=buf;
	for(i=0; i<height; i++) {
	    for (j=0, tw=width; j<nbyte; j++) {
		p = *ptn++;
		for (k=0; k<8 && tw>0; k++, tw--) {
		    if (p & 0x80) {
			    *pbuf++ = '0';
		    } else {
			    *pbuf++ = '-';
		    }
		    p = p << 1;
		}
	    }
	    *pbuf++ = '\n';
	}
	*pbuf = '\0';
	fprintf(fp, "%s", buf);
	return(0);
}


int
#if NeedFunctionPrototypes
PtnZoom(
    char	*dmem,
    char	*smem,
    int		sw,
    int		sh,
    int		dw,
    int		dh
)
#else
PtnZoom(dmem, smem, sw, sh, dw, dh)
char	*dmem;
char	*smem;
int	sw;
int	sh;
int	dw;
int	dh;
#endif
{
	int	    swidth;
	int	    dwidth;
	int	    i, lcnt;
	char   *dbuf, *sp, *dp;

	swidth = (sw + 7) / 8;
	dwidth = (dw + 7) / 8;

	if ((dbuf = (char *)malloc(dwidth)) == NULL) {
		return(MALLOC_ERROR);
	}

	lcnt = 0;
	sp = smem;
	dp = dmem;

	for(i=0; i < sh; i++) {
		lcnt += dh;
		if (lcnt >= sh) {
			exline(sp, dbuf, sw, dw);
			sp += swidth;
			lcnt -= sh;
			memcpy(dp, dbuf, dwidth);
			dp += dwidth;
			for (; lcnt >= sh; lcnt -= sh) {
				memcpy(dp, dbuf, dwidth);
				dp += dwidth;
			}
		} else {
			sp += swidth;
		}
	}
	free(dbuf);
	return(0);
}


static void
exline(sp, dbuf, sw, dw)
char	*sp;
char	*dbuf;
int	sw;
int	dw;
{
	unsigned int	i, bit, sval, dval, dcnt, bcnt;

	bcnt = dval = 0;
	dcnt = 8;

	for(i=0; i<sw; i++) {
		if (i % 8 == 0) {
			sval = *sp++;
		}
		bit = ((sval & 0x80) ? 1 : 0);
		sval <<=  1;
		for (bcnt += dw; bcnt >= sw; bcnt -= sw) {
			dval = (dval << 1) | bit;
			if (--dcnt == 0) {
				*dbuf++ = (char)dval;
				dval = 0;
				dcnt = 8;
			}
		}
	}
	if (dcnt != 8) {
		dval <<= dcnt;
		*dbuf = (char)dval;
	}
}


char *
#if NeedFunctionPrototypes
GetTmpPath( char *path )
#else
GetTmpPath( path )
char	*path;
#endif
{
	char	*p=NULL, *sp, *buf ;
	int	len ;
	struct	stat	statbuf ;

	/* Get directory of temporary file */
	if( !(p = (char *)getenv( "TMPDIR" )) ){
	    p = DEFAULT_TMPPATH ;
	}
	/* Get memory for temporary file name */
	sp = p ;
	len = strlen(p) ;
	if( *(sp + len -1) == '/' ){
	    len += (strlen( TEMPFILEKEY ) + 1) ;
	}else{
	    len += (strlen( TEMPFILEKEY ) +1 + 1) ;
	}
	if ((buf = (char *)malloc( len )) == NULL) {
	    return NULL;
	}
	/* Get temporary file path */
	strcpy( buf, p ) ;
	sp = buf + strlen(buf) -1 ;
	if( *sp == '/' )	*sp-- = '\0' ;
	sprintf( sp+1, "/%s", TEMPFILEKEY ) ;
	/* Get temporary file name */
	return mktemp( buf ) ;
}


int
#if NeedFunctionPrototypes
Link_NewFile( char *rd_file, char *wr_file )
#else
Link_NewFile( rd_file, wr_file )
char	*rd_file ;
char	*wr_file ;
#endif
{
	FILE	*rfp, *wfp ;
	int	i, c ;
	int	rfd, wfd ;
	struct	stat	statbuf ;
	unsigned char	bufc ;
	char	*buf ;
	int	ismmap = 0 ;

	if( stat( wr_file, &statbuf ) == -1 ){
	    if( (wfd = open( wr_file, O_CREAT | O_WRONLY, 0644 )) == -1 ){
		return -1 ;
	    }
	}else{
	    if( (wfd = open( wr_file, O_WRONLY | O_TRUNC )) == -1 ){
		return -1 ;
	    }
	}
	if( stat( rd_file, &statbuf ) == -1 ){
	    close(wfd) ; 
	    return -1 ;
	}
	if( (rfd = open( rd_file, O_RDONLY )) == -1 ){
	    close(wfd) ; 
	    return -1 ;
	}

#if	defined( SVR4 )
	if( (buf = (char *)mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, rfd, 0)) == (char *)-1 ){
#endif	/* SVR4 */
	    if( !(buf = (char *)malloc(statbuf.st_size)) ){
		close(wfd) ; 
		close(rfd) ; 
		return -1 ;
	    }
	    if( read(rfd, buf, statbuf.st_size) != statbuf.st_size ){
		close(wfd) ; 
		close(rfd) ; 
		return -1 ;
	    }
#if	defined( SVR4 )
	}else{
	    ismmap = 1 ;
	}
#endif	/* SVR4 */

	close(rfd) ; 

	if( write(wfd, buf, statbuf.st_size) != statbuf.st_size ){
#if	defined( SVR4 )
	    if( ismmap ){
		munmap(buf, statbuf.st_size) ;
	    }else{
#endif	/* SVR4 */
		free(buf) ;
#if	defined( SVR4 )
	    }
#endif	/* SVR4 */
	    close(wfd) ; 
	    return (DEVICE_FAIL);
	}
#if	defined( SVR4 )
	if( ismmap ){
	    munmap(buf, statbuf.st_size) ;
	}else{
#endif	/* SVR4 */
	    free(buf) ;
#if	defined( SVR4 )
	}
#endif	/* SVR4 */
	close(wfd) ;

	return 0 ;
}


int
#if NeedFunctionPrototypes
ChkNumString( char	*str )
#else
ChkNumString( str )
char	*str;
#endif
{
	char	*tmp;
	int		num;

	num = (int)strtol( str, &tmp, 10 );
	if ((num == 0)&&(tmp == str)) {
		return(-1);
	}
	if ((size_t)(tmp - str) != strlen(str)){
		return(-1);
	}
	return( 0 );
}


void
#if NeedFunctionPrototypes
ErrMsgTable_AndExit(
    int		er_no,
    char	*snf_in,
    char	*snf_out,
    char	*gpf_in,
    char	*gpf_out,
    char	*bdf_in,
    char	*prog_name 
)
#else
ErrMsgTable_AndExit(er_no, snf_in, snf_out, gpf_in, gpf_out, bdf_in, prog_name)
int	er_no;
char	*snf_in;
char	*snf_out;
char	*gpf_in;
char	*gpf_out;
char	*bdf_in;
char	*prog_name ;
#endif
{
	int	rtn = 0 ;
	switch(er_no) {
	case 0:
		break;
	case BDF_OPEN_IN :
		USAGE2("%s : The input font file cannot be opened.\"%s\"．\n", prog_name, (snf_in != NULL)? snf_in : "\0" );
		rtn = OPEN_FAIL ;
		break;

	case BDF_OPEN_OUT :
		USAGE2("%s : The output font file cannot be opened.\"%s\"．\n", prog_name, (snf_out != NULL)? snf_out : "\0" );
		rtn = OPEN_FAIL ;
		break;


	case BDF_READ :
		USAGE2("%s : Information from the font file cannot be extracted.\"%s\"． \n", prog_name, (snf_in != NULL) ? snf_in : "\0" );
		rtn = READ_FAIL ;
		break;

	case BDF_WRITE :
		USAGE2("%s : It is not possible to write to the font file.\"%s\"．\n", prog_name, (snf_out != NULL) ? snf_out : "\0" );
		rtn = DEVICE_FAIL ;
		break;

	case BDF_INVAL :
		USAGE2("%s : The format of the font file is illegal.\"%s\"． \n", prog_name, (snf_in != NULL) ? snf_in : "\0" );
		rtn = 1 ;
		break;

	case BDF_OPEN_HEAD :
		USAGE2("%s : The BDF file cannot be opened.\"%s\"．\n", prog_name, (bdf_in != NULL)? bdf_in : "\0" );
		rtn = OPEN_FAIL ;
		break;

	case BDF_READ_HEAD :
		USAGE2("%s : Information of the BDF file cannot be extracted.\"%s\"． \n", prog_name, (bdf_in != NULL) ? bdf_in : "\0" );
		rtn = READ_FAIL ;
		break;

	case GPF_OPEN_IN :
		USAGE2("%s : The input character pattern file cannot be opened.\"%s\"．\n", prog_name, (gpf_in != NULL) ? gpf_in : "\0" );
		rtn = OPEN_FAIL ;
		break;

	case GPF_OPEN_OUT :
		USAGE2("%s : The output character pattern file cannot be opened.\"%s\"．\n", prog_name, (gpf_out != NULL) ? gpf_out : "\0" );
		rtn = OPEN_FAIL ;
		break;

	case GPF_READ :
		USAGE2("%s : The character pattern file cannot be read.\"%s\"．\n", prog_name, (gpf_in != NULL) ? gpf_in : "\0" );
		rtn = READ_FAIL ;
		break;

	case GPF_WRITE :
		USAGE2("%s : It is not possible to write to the character pattern file.\"%s\"．\n", prog_name, (gpf_out != NULL) ? gpf_out : "\0" );
		rtn = DEVICE_FAIL ;
		break;

	case GPF_INVAL :
		USAGE2("%s : The format of the character pattern file is illegal.\"%s\"．\n", prog_name, (gpf_in != NULL) ? gpf_in : "\0" );
		rtn = 1 ;
		break;

	default :
		fatal_err_msg( er_no, prog_name );
		rtn = 1 ;
		break;
	}
	exit(rtn);
}


void
#if NeedFunctionPrototypes
fatal_err_msg( 
    int		er_no,
    char	*prog_name
)
#else
fatal_err_msg( er_no, prog_name )
int	er_no;
char	*prog_name;
#endif
{
	USAGE1("%s : The font file failed to be converted. ", prog_name);
	switch( er_no ) {
	case MALLOC_ERROR:
		USAGE("There is no memory any more.\n");
		break;
	case FORK_ERROR:
		USAGE("The child process cannot be created.\n" );
		break;
	case EXEC_ERROR:
		USAGE("Cannot execute dtgpftobdf.\n" );
		break;
	case PIPE_ERROR:
		USAGE("The pipe buffer cannot be open.\n" );
		break;
	case DUP_ERROR:
		USAGE("The file descripter cannot be duplicated.\n" );
		break;
	case POPEN_ERROR:
		USAGE("The X font compiler cannot be executed.\n" );
		break;
	case PCLOSE_ERROR:
		USAGE("The pipe buffer cannot be close.\n" );
		break;
	case FDOPEN_ERROR:
		USAGE("The file descripter cannot be got.\n" );
		break;
	case STAT_ERROR:
		USAGE("The status of font cannot be got.\n" );
		break;
	case MKTMP_ERROR:
		USAGE("The temporally file cannot be got.\n" );
		break;
	case PCFFONTC_ERROR:
		USAGE("Cannot execute bdftopcf.\n" );
		break;
	case SNFFONTC_ERROR:
		USAGE("Cannot execute bdftosnf.\n" );
		break;
	default:
		USAGE1("Terminates abnormally. ( err_code(%d) )\n", er_no);
		break;
	}
	return;
}



static	int	sig_flg=0;

#define	GPF_BUCK_UP_FAIL	1
#define	GPF_MODIFY_FAIL		2
#define	RESTORE_FAIL		3
#define	CATCH_SIGNAL		10

void
ChatchSig()
{
	sig_flg = 1;
}

void
#if NeedFunctionPrototypes
Put_File_Create_Err_Msg( 
    int		msg_level,
    char	*org_name,
    char	*tmp_name,
    char	*save_name,
    char	*com
)
#else
Put_File_Create_Err_Msg( msg_level, org_name, tmp_name, save_name, com )
int	msg_level;
char	*org_name;
char	*tmp_name;
char	*save_name;
char	*com;
#endif
{
	switch( msg_level ) {
	case GPF_BUCK_UP_FAIL:
		USAGE1("%s : Failed to write temporary file. Terminates abnormally.\n", com );
		put_file_create_err_msg = DEVICE_FAIL ;
		break;

	case GPF_MODIFY_FAIL:
		USAGE1("%s : Failed in the renewal of the font file. Terminates abnormally.\n", com );
		put_file_create_err_msg = WRITE_FAIL ;
		break;

	case RESTORE_FAIL:
		USAGE3("%s : The file before updating cannot be changed to former file name.\tPlease execute the following command after the inquiry the system manager.\n\t mv %s %s\n",
			com, save_name, org_name);
		put_file_create_err_msg = WRITE_FAIL ;
		break;

	case CATCH_SIGNAL:
		USAGE1("%s : The signal was received. Terminates abnormally.\n", com);
		put_file_create_err_msg = 1 ;
		break;
	}
}

int
#if NeedFunctionPrototypes
Make_NewFefFile( 
    char	*org_name,
    char	*tmp_name,
    mode_t	mode,
    uid_t	owner,
    gid_t	group,
    char	*com
)
#else
Make_NewFefFile( org_name, tmp_name, mode, owner, group, com )
char	*org_name;
char	*tmp_name;
mode_t	mode;
uid_t	owner;
gid_t	group;
char	*com;
#endif
{
	struct	stat	statbuf;
	char	*save_name = NULL;
	int		ret_val;
	int		msg_level;
	int		rtn ;

#ifndef SVR4
	signal( SIGHUP , (void(*)())ChatchSig);
	signal( SIGINT , (void(*)())ChatchSig);
	signal( SIGQUIT, (void(*)())ChatchSig);
	signal( SIGTERM, (void(*)())ChatchSig);
#else
	sigset( SIGHUP , (void(*)())ChatchSig);
	sigset( SIGINT , (void(*)())ChatchSig);
	sigset( SIGQUIT, (void(*)())ChatchSig);
	sigset( SIGTERM, (void(*)())ChatchSig);
#endif

	errno = 0;
	if (sig_flg || ((save_name = GetTmpPath( org_name )) == NULL)) {
		msg_level = (sig_flg) ? CATCH_SIGNAL : GPF_BUCK_UP_FAIL;
		Put_File_Create_Err_Msg( msg_level, org_name, tmp_name,
							(char *)NULL, com );
		Unlink_Tmpfile ( tmp_name, com );
		return(put_file_create_err_msg);
	}

	if (sig_flg || ((rtn=Link_NewFile( org_name, save_name )) != 0)) {
		msg_level = (sig_flg) ? CATCH_SIGNAL : GPF_BUCK_UP_FAIL;
		Put_File_Create_Err_Msg( msg_level, org_name, tmp_name,
							save_name, com );
		Unlink_Tmpfile ( save_name, com );
		Unlink_Tmpfile ( tmp_name, com );
		return( (put_file_create_err_msg == DEVICE_FAIL)?
			put_file_create_err_msg : rtn );
	}

	if (sig_flg || ((rtn=Link_NewFile( tmp_name, org_name )) != 0)) {
		msg_level = (sig_flg) ? CATCH_SIGNAL : GPF_MODIFY_FAIL;
		Put_File_Create_Err_Msg( msg_level, org_name, tmp_name,
							save_name, com );
		if ( (rtn=Link_NewFile( save_name, org_name )) != 0 ) {
			Put_File_Create_Err_Msg( RESTORE_FAIL, org_name,
						    tmp_name, save_name, com );
			Unlink_Tmpfile ( tmp_name, com );
		} else {
			Unlink_Tmpfile ( tmp_name, com );
			Unlink_Tmpfile ( save_name, com );
		}
		return( (put_file_create_err_msg == DEVICE_FAIL)?
			put_file_create_err_msg : rtn );
	}

	ret_val = 0;
	ret_val += Unlink_Tmpfile ( tmp_name,  com );
	ret_val += Unlink_Tmpfile ( save_name, com );

	return( (ret_val)? 1 : 0 );
}


int
#if NeedFunctionPrototypes
Unlink_Tmpfile( 
    char	*file,
    char	*com
)
#else
Unlink_Tmpfile( file, com )
char	*file;
char	*com;
#endif
{
	errno = 0;
	if ( unlink( file ) != 0 ) {
		USAGE2("%s : The work file cannot be deleted. Please unlink the following files\tafter the inquiry the system manager.\"%s\" \n", com, file );
		return(1);
	}
	return(0);
}


int
#if NeedFunctionPrototypes
Chmod_File ( 
    char	*fname,
    mode_t	mode,
    char	*com
)
#else
Chmod_File ( fname, mode, com )
char	*fname;
mode_t	mode;
char	*com;
#endif
{
	errno = 0;
	if ( mode == 0 )	return( 0 ) ;
	if ( chmod (fname, mode) != 0) {
		USAGE3("%s : Failed in the mode change of the following files. Please change the mode to %o\tafter the inquiry the system manager.\"%s\" \n", com, (int)mode, fname );
		return( 1 );
	}
	return( 0 );
}


int
#if NeedFunctionPrototypes
Chown_File ( 
    char	*fname,
    uid_t	owner,
    gid_t	group,
    char	*com
)
#else
Chown_File ( fname, owner, group, com )
char	*fname;
uid_t	owner;
gid_t	group;
char	*com;
#endif
{
	errno = 0;
	if ( chown (fname, owner, group) != 0) {
		USAGE2("%s : Failed in setteing of the owner and the group of the following files. \tPlease change to the file of the owner and the group of following ID \tafter the inquiry the system manager.\"%s\"\n", com, fname );
		USAGE1("\t\t Owner ID ：%d\n", (int)owner);
		USAGE1("\t\t Group ID ：%d\n", (int)group);
		return( 1 );
	}
	return( 0 );
}




int 	
#if NeedFunctionPrototypes
ChkPcfFontFile( char	*filename )
#else
ChkPcfFontFile( filename )
char	*filename;
#endif
{
	char	*suffix;

	if ( !filename ) {
		return	-1;
	}
	suffix = ( char * )strrchr( filename, '.' );
	if ( !suffix ) {
		return	-1;
	}

	return	strcmp( PCFSUFFIX, suffix );
}


int 	
#if NeedFunctionPrototypes
ChkSnfFontFile( char	*filename )
#else
ChkSnfFontFile( filename )
char	*filename;
#endif
{
	char	*suffix;

	if ( !filename ) {
		return	-1;
	}
	suffix = ( char * )strrchr( filename, '.' );
	if ( !suffix ) {
		return	-1;
	}

	return	strcmp( SNFSUFFIX, suffix );
}


char	*
#if NeedFunctionPrototypes
get_cmd_path( 
    char	*path,
    char	*cmd
)
#else
get_cmd_path( path, cmd )
char	*path;
char	*cmd;
#endif
{
	char	*cmd_path;
	struct stat	st;
	char	*end;
	char	chr_sv;

	if ( !path || !cmd ) {
		return	NULL;
	}

	for ( ; end = ( char * )strchr( path, ':' ); path = end + 1 ) {
		chr_sv = *end;
		*end = NULL;
		AllocString( cmd_path, path, NULL ) ;
		*end = chr_sv;

		AddString( cmd_path, "/", NULL ) ;
		AddString( cmd_path, cmd, NULL ) ;

		if ( stat( cmd_path, &st ) == 0 ) {
		    if( st.st_mode & S_IFREG ) {
			cmd_path = realloc( cmd_path, strlen( cmd_path ) + 1 );
			return	cmd_path;
		    } else	continue ;
		}
		FreeString( cmd_path ) ;
	}
	AllocString( cmd_path, path, NULL ) ;
	AddString( cmd_path, "/", NULL ) ;
	AddString( cmd_path, cmd, NULL ) ;
	if ( stat( cmd_path, &st ) == 0 ) {
	    if( st.st_mode & S_IFREG ) {
		cmd_path = realloc( cmd_path, strlen( cmd_path ) + 1 );
		return	cmd_path;
	    }
	}
	free( cmd_path );
	return	NULL;
}

int
#if NeedFunctionPrototypes
SetCmdPath( 
    char	*com ,
    char	**path ,
    char	*dflt_path ,
    char	*cmd 
)
#else
SetCmdPath( com, path, dflt_path, cmd )
char	*com ;
char	**path ;
char	*dflt_path ;
char	*cmd ;
#endif
{
	struct	stat	statbuf ;
	char	*pbuf ;

	if( stat( dflt_path, &statbuf ) ){
	    if( !( pbuf = (char *)get_cmd_path( getenv( "PATH" ), cmd )) ){
		USAGE2("%s: There is not \"%s\" command in \"PATH\".\n", com, cmd ) ;
		return -1 ;
	    }
	    if( stat( pbuf, &statbuf ) ){
		USAGE2("%s: There is not \"%s\" command.\n", com, cmd ) ;
		return STAT_ERROR ;
	    }
	    if( !(statbuf.st_mode & S_IXUSR) ){
		USAGE2("%s: \"%s\" command don't have permission to execute.\n", com, cmd ) ;
		return STAT_ERROR ;
	    }
	}else{
	    if( !(statbuf.st_mode & S_IXUSR) ){
		USAGE2("%s: \"%s\" command don't have permission to execute.\n", com, cmd ) ;
		return STAT_ERROR ;
	    }
	    pbuf = dflt_path ;
	}
	*path = pbuf ;
	return 0 ;
}

/*****************************< end of oakfuty.c >************************/
