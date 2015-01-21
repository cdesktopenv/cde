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
/* $XConsortium: gpftobdf.c /main/5 1996/11/08 02:04:24 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "font.h"
#include "misc.h"
#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/Xmd.h>
#include <ctype.h>
#include <errno.h>
#include "fontstruct.h"
#include "snfstruct.h"
#include <time.h>
#include "bdftosnf.h"
#include <memory.h>
#if defined( SVR4 )
#include <sys/mman.h>
#endif

#include "udccom.h"
#include "udcutil.h"

#define	GLYPHPADOPTIONS			4
#define SIZEOF_COMPRESSED_METRIC	5

#include	"gtobdf.h"
#include	"pcf.h"

typedef	struct	pcf_tmp {
	FontInfoRec 	info;
	int 	ntables;
	PCFTablePtr 	tables;
	int 	nprops;
	FontPropPtr 	props;
	char	*isStringProp;
	int 	str_length;
	char	*string;
	int 	nbitmaps;
	CARD32	bmp_fmt;	/* bitOrder ,byteOrder, glyph, scan */
	CARD32	*offsets;
	int 	sizebitmaps;
	char	*bitmaps;
	CARD32	enc_fmt;
	CARD16	*encodingOffsets;
	size_t	pcf_bufsz;
	caddr_t 	pcf_buffer;

	/* scale width	*/
	CARD32	swd_fmt ;
	int 	swd_num ;
	CARD32	*swidth ;

	/* glyph name	*/
	CARD32	glyph_fmt ;
	unsigned int 	glyphs ;
	int 	*glyph_offset ;
	char	*glyph_name ;

	/* metrics	*/
	int 	compress ;
	CARD32	mtr_fmt ;
	int 	mtr_num ;
	xCharInfo	*metrics;

} PcfTmp;

typedef struct prop_tmp {
	char	*font_name;
	FontPropRec 	psize;
	FontPropRec 	resolution;
} PropTmp;



static	char	*readfontfile();
static	void	Anafprop();
static	void	pSIZE();
static	void	pPROPS();
static	void	pCHARS();
static	void	pfixCHARS();
static	int 	getarg();

static	Bool	seekToType() ;
static	void	getMetric() ;
static	Bool	getAccel() ;
static	Bool	getProperties() ;
static	void	putPtn() ;
static	void	ByteSwap() ;
static	void	invertBits() ;

static	int	getINT16() ;
static	int	getINT32() ;
static	CARD32	getLSB32();
static	char	*make_toptn();
static	void	pSIZE_pcf();
static	PcfTmp	*openPcfFont();
static	void	setProp();
static	void	pPROPS_pcf();
static	void	pCHARS_pcf();
static	void	BitOrderInvert();
static	char	*bufp,buf[2048];

extern int 	ChkPcfFontFile();


main( argc,argv )
int 	argc;
char	*argv[];
{
	int         i,k;


	char        *fp;		/* font				*/
	FontInfoRec *fip;		/* font information		*/
	CharInfoRec *cip, *wkcip;	/* character information	*/
	unsigned int glyphstotal;	/* glyph			*/
	unsigned int charInfoNum ;	/* character information's number */
	unsigned int charNum;		/* define character number         */
	char        *glyphPtr;		/* glyph			*/
	FontPropRec *fpropPtr,*fplistPtr;	/* font property list	*/
	FontPropRec psprop;		/* point size property		*/
	FontPropRec resprop;		/* resolution property		*/
	char        *fpropvnPtr;

	unsigned int fplistNum;		/* font property number		*/
	char        *fontname;		/* fontname			*/
	time_t      clock;
	struct      tm *tm;
	char        tmbuf[80] ;

	char fname[1024];		/* snf filename			*/
	int  bitorder;			/* bitorder			*/
	int byteorder;			/* byteorder			*/
	int scanunit;			/* scanunit			*/
	int glyphPad ;			/* glyph			*/
	int outLevel;			/* output information level	*/
	int fix;


	int 	isPcf;

	PcfTmp	*pcf;
	PropTmp 	proptmp;
	char	*tmpfname;
	extern	char	*GetRealFileName();


	if (getarg(argc,argv,fname,&bitorder,&byteorder,&scanunit,&glyphPad,
	    &outLevel,&fix) != 0) {
		exit( -1 );
	}

	/*
	 *  read to fontfile 
	 */
	if ( ( tmpfname = GetRealFileName( fname ) ) == NULL ) {
		USAGE("dtgpftobdf: cannot refer to substance file.\n" );
		exit( -1 );
	}
	if ( strcmp( tmpfname, fname ) != 0 ){
		strcpy( fname, tmpfname );
	}

	if ( ChkPcfFontFile( fname ) == 0 ){
		/* pcf font */
		isPcf = 1;
		if ( !( pcf = openPcfFont( fname ) ) ) {
			USAGE("dtgpftobdf: cannot open font\n" );
			exit( -1 );
		}
	} else {
		/* snf font */
		isPcf = 0;
		if ( ( fp = readfontfile( fname ) ) == (char *)-1 ) {
			exit( -1 );
		}
	}


	if ( !isPcf ) {
		/* SNF format */
		COMM_SNF_GETHEADER( fip, fp ) ;
		/* font file check */
		if ((fip->version1 != fip->version2) || 
		    (fip->version1 != FONT_FILE_VERSION)) {
			USAGE("dtgpftobdf: illegal font version\n");
			exit( -1 );
		}

		cip = (CharInfoRec *)((char *)fip + sizeof(FontInfoRec));

		charInfoNum = (fip->lastCol - fip->firstCol + 1)
		    *(fip->lastRow - fip->firstRow + 1);
		glyphPtr = ((char *)cip) + ( charInfoNum * sizeof(CharInfoRec) );

		charNum = charInfoNum ;
		wkcip = cip ;

		glyphstotal = fip->maxbounds.byteOffset;

		glyphPad = (((fip->maxbounds.metrics.rightSideBearing
		    - fip->maxbounds.metrics.leftSideBearing)+31)/32)*4;

		if ( fix == TRUE ) {
		    for ( i = 0; i < charInfoNum; i++ ) {
			if ( wkcip->exists == FALSE ) {
			    charNum-- ;
			}
			wkcip++;
		    }
		} else {
		    glyphstotal = 0 ;
		    for ( i = 0; i < charInfoNum; i++ ) {
			if (wkcip->exists == FALSE) {
			    charNum-- ;
			    wkcip++;
			    continue;
			}
			glyphstotal += (wkcip->metrics.ascent
			    + wkcip->metrics.descent) * glyphPad;
			wkcip++;
		    }
		}

		fpropPtr = (FontPropRec *)(glyphPtr + glyphstotal);
		fpropvnPtr = ((char *)fpropPtr) + (fip->nProps) * sizeof(FontPropRec);

	}


	if ( isPcf ) {
		setProp( pcf, &proptmp );
	} else {
		fontname = (char *)0;
		Anafprop( 
		    fip, fpropPtr, fpropvnPtr, 
		    &fontname, &psprop, &resprop, 
		    &fplistPtr, &fplistNum 
		    );
	}

	/*
	 *¡¡output to BDF information
	 */
	bufp = buf;
	memcpy(bufp,"STARTFONT 2.1\n",14);
	bufp += 14;
	memcpy(bufp,"COMMENT\n",8);
	bufp += 8;
	memcpy(bufp,"COMMENT  This BDF was created by dtgpftobdf.\n",45 );
	bufp += 45 ;
	memcpy(bufp,"COMMENT\n",8);
	bufp += 8;
	time(&clock);
	tm = localtime(&clock);

	strcpy( tmbuf, asctime(tm) ) ;

	k = sprintf(bufp,"COMMENT    Created date : %s\n",tmbuf );
	bufp += k-1;
	memcpy(bufp,"COMMENT\n",8);
	bufp += 8;

	if ( isPcf ) {
		k = sprintf(bufp,"FONT %s\n",proptmp.font_name);
		bufp += k;
		*bufp = '\0';
		fprintf( stdout,"%s",buf );
		pSIZE_pcf( &proptmp );
		fprintf( stdout,"FONTBOUNDINGBOX %d %d %d %d\n",
		    pcf->info.maxbounds.metrics.rightSideBearing 
		    - pcf->info.maxbounds.metrics.leftSideBearing,
		    pcf->info.maxbounds.metrics.ascent 
		    + pcf->info.maxbounds.metrics.descent,
		    pcf->info.maxbounds.metrics.leftSideBearing,
		    - ( pcf->info.maxbounds.metrics.descent )
			);
		pPROPS_pcf( pcf );
		if ( outLevel == FALSE ) {
			pCHARS_pcf( pcf, fix );
		}
	} else {
		k = sprintf( bufp,"FONT %s\n",fontname );
		bufp += k;
		*bufp = '\0';
		fprintf( stdout,"%s",buf );
		pSIZE( &psprop, &resprop );
		fprintf( stdout,"FONTBOUNDINGBOX %d %d %d %d\n",
		    cip->metrics.rightSideBearing 
		    - cip->metrics.leftSideBearing,
		    cip->metrics.ascent + cip->metrics.descent,
		    cip->metrics.leftSideBearing,
		    -(cip->metrics.descent)
			);
		pPROPS( fip,fplistPtr,fplistNum );
		if ( outLevel == FALSE ) {
		    if ( fix == TRUE ) {
			pfixCHARS( fip,cip,glyphPtr,charNum, glyphPad );
		    } else {
			pCHARS(fip,cip,glyphPtr,charNum,glyphPad);
		    }
		}

	}

	fprintf( stdout,"ENDFONT\n" );
	exit( 0 );
}

/*
 * read to font file
 *
 */
static	char	*readfontfile( fname )
char	*fname;
{
	int 	fd;
	char	*fp;
	struct stat stat;

	fd = open( fname,O_RDONLY );
	if ( fd < 0 ) {
		USAGE("dtgpftobdf: can't open file.\n" );
		return	( char * )-1;
	}
#if defined( SVR4 )
	if ( fstat( fd,&stat ) == 0 ) {
		fp = (char *)mmap( 0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0 );
		if ( fp == (char *)-1 ) {
			if ( ( fp = ( char * )malloc( stat.st_size ) ) == NULL ) {
				USAGE("dtgpftobdf: malloc error.\n" );
				close( fd );
				return	( char * )-1;
			}
/*			if ( read( fd, fp, stat.st_size ) < 0 ) {*/
			if ( read( fd, fp, stat.st_size ) != stat.st_size ) {
				USAGE("dtgpftobdf: can't read file.\n" );
				close( fd );
				return	( char * )-1;
			}
		}
	} else {
		USAGE("dtgpftobdf: can't read file\n" );
		close( fd );
		return	( char * )-1;
	}
	close( fd );
#else	/* pfa */
	if ( fstat(fd,&stat) == 0 ) {
		if ( ( fp = (char *)malloc(stat.st_size) ) == NULL ) {
			USAGE("dtgpftobdf: malloc error.\n" );
			close( fd );
			return	( char * )-1;
		}
		if ( read( fd, fp, stat.st_size ) < 0 ) {
			USAGE("dtgpftobdf: can't read file.\n" );
			close( fd );
			return	( char * )-1;
		}
	} else {
		USAGE("dtgpftobdf: can't read file\n" );
		close( fd );
		return	( char * )-1;
	}
	close( fd );
#endif /* pfa */
	return	fp;
}


#define getINT8( p ) ( *p++ ) ;

static CARD32
getLSB32( p )
unsigned char *p;
{
	CARD32	c;

	c = *p++;
	c |= (CARD32)(*p++) << 8;
	c |= (CARD32)(*p++) << 16;
	c |= (CARD32)(*p) << 24;

	return	c;
}


static int
getINT32( p, format )
unsigned char	*p;
CARD32	format;
{
	CARD32	c;

	if ( PCF_BYTE_ORDER(format) == MSBFirst ) {
		c = (CARD32)(*p++) << 24;
		c |= (CARD32)(*p++) << 16;
		c |= (CARD32)(*p++) << 8;
		c |= (CARD32)(*p);
	} else {
		c = (CARD32)(*p++);
		c |= (CARD32)(*p++) << 8;
		c |= (CARD32)(*p++) << 16;
		c |= (CARD32)(*p) << 24;
	}

	return ( int )c;
}

static int
getINT16( p, format )
unsigned char	*p;
CARD32	format;
{
	CARD32	c;

	if ( PCF_BYTE_ORDER(format) == MSBFirst ) {
		c = (CARD32)(*p++) << 8;
		c |= (CARD32)(*p);
	} else {
		c = (CARD32)(*p++);
		c |= (CARD32)(*p) << 8;
	}

	return ( int )c;
}

static Bool
seekToType( tables, ntables, type, formatp, sizep, offsetp)
PCFTablePtr 	tables;
int 	ntables;
CARD32	type;
CARD32	*formatp;
CARD32	*sizep;
CARD32	*offsetp;
{
	int 	i;

	for ( i = 0; i < ntables; i++ ) {
		if ( getLSB32( (unsigned char *)&tables[i].type) == type) {
			*formatp = getLSB32( (unsigned char *)&tables[i].format);
			*sizep = getLSB32( (unsigned char *)&tables[i].size);
			*offsetp = getLSB32( (unsigned char *)&tables[i].offset);
			return	TRUE;
		}
	}
	return	FALSE;
}


static void
getMetric( buf, format, metric )
caddr_t 	buf;
CARD32	format;
xCharInfo	*metric;
{
	metric->leftSideBearing = getINT16( (unsigned char *)buf, format);
	buf += 2;
	metric->rightSideBearing = getINT16( (unsigned char *)buf, format);
	buf += 2;
	metric->characterWidth = getINT16( (unsigned char *)buf, format);
	buf += 2;
	metric->ascent = getINT16( (unsigned char *)buf, format);
	buf += 2;
	metric->descent = getINT16( (unsigned char *)buf, format);
	buf += 2;
	metric->attributes = getINT16( (unsigned char *)buf, format);
	buf += 2;
}

static Bool
getAccel( pFontInfo, maxink, buf_top, tables, ntables, type )
FontInfoPtr 	pFontInfo;
xCharInfo	*maxink;
caddr_t 	buf_top;
PCFTablePtr 	tables;
int 	ntables;
CARD32	type;
{
	CARD32	format;
	CARD32	size;
	CARD32	offset;
	caddr_t 	buffer;

	if ( !seekToType( tables, ntables, type, &format, &size, &offset)) {
		return	FALSE;
	}

	buffer = buf_top + offset;
	format = getLSB32( (unsigned char *)buffer);
	buffer += 4;
	if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT )
	    && !PCF_FORMAT_MATCH( format, PCF_ACCEL_W_INKBOUNDS )
	    ) {
		return	FALSE;
	}
	pFontInfo->noOverlap = *buffer++;
	pFontInfo->constantMetrics = *buffer++;
	pFontInfo->terminalFont = *buffer++;
	pFontInfo->constantWidth = *buffer++;
	pFontInfo->inkInside = *buffer++;
	pFontInfo->inkMetrics = *buffer++;
	pFontInfo->drawDirection = *buffer++;
	/* natural alignment */

	buffer++;

	pFontInfo->fontAscent = getINT32( (unsigned char *)buffer, format);
	buffer += 4;

	pFontInfo->fontDescent = getINT32((unsigned char *)buffer, format);
	buffer +=4;

	buffer += 4;

	getMetric(buffer, format, &pFontInfo->minbounds.metrics);
	buffer += 12;

	getMetric(buffer, format, &pFontInfo->maxbounds.metrics);
	buffer += 12;

	if ( PCF_FORMAT_MATCH( format, PCF_ACCEL_W_INKBOUNDS ) ) {
		buffer += 12;
		getMetric( buffer, format, maxink);
	} else {
		*maxink = pFontInfo->maxbounds.metrics;
	}

	return	TRUE;
}

static Bool
getProperties( pcf, buf_top, tables, ntables)
PcfTmp	*pcf;
caddr_t 	buf_top;
PCFTablePtr 	tables;
int 	ntables;
{
	CARD32	format;
	CARD32	size;
	CARD32	offset;
	caddr_t 	buffer;
	int 	i;

	if ( !seekToType( tables, ntables, (CARD32)PCF_PROPERTIES, &format, &size, &offset ) ) {
		return	FALSE;
	}

	buffer = buf_top + offset;
	format = getLSB32( (unsigned char *)buffer );
	buffer += 4;
	if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) ) {
		return	FALSE;
	}
	pcf->nprops = getINT32((unsigned char *)buffer, format);
	buffer += 4;

	if ( !( pcf->props = ( FontPropPtr )malloc( pcf->nprops * sizeof( FontPropRec ) ) ) ) {
		return	FALSE;
	}

	if ( !( pcf->isStringProp = ( char * )malloc(pcf->nprops * sizeof( char ) ) ) ) {
		return	FALSE;
	}

	for ( i = 0; i < pcf->nprops; i++ ) {
		pcf->props[i].name = getINT32((unsigned char *)buffer, format);
		buffer += 4;
		pcf->isStringProp[i] = *buffer++;
		pcf->props[i].value = getINT32((unsigned char *)buffer, format);
		buffer += 4;
	}
	if ( pcf->nprops & 3 ) {
		i = 4 - (pcf->nprops & 3);
		buffer += i;
	}

	pcf->str_length = getINT32((unsigned char *)buffer, format);
	buffer += 4;

	pcf->string = (char *)buffer;
	return	TRUE;
}


static	PcfTmp  *
openPcfFont( fontname )
char	*fontname;
{
	PcfTmp	*pcf_tmp;

	CARD32	format;
	CARD32	size;
	CARD32	offset;
	CARD32	*bitmapSizes;
	xCharInfo	maxink;
	caddr_t 	buffp;
	struct stat 	st;

	pcf_tmp = ( PcfTmp * )calloc( 1, sizeof( PcfTmp ) );
	if ( !pcf_tmp ) {
		USAGE("dtgpftobdf : calloc() error.\n" ) ;
		goto Bail;
	}

	if ( stat( fontname, &st ) ) {
		goto Bail;
	}

	if ( ( pcf_tmp->pcf_buffer = readfontfile( fontname)) == (char *)-1 ) {
		goto Bail;
	}

	pcf_tmp->pcf_bufsz = st.st_size;

	if ( (format = getLSB32( (unsigned char *)pcf_tmp->pcf_buffer )) != PCF_FILE_VERSION ) {
		USAGE1("dtgpftobdf : pcf file version(0x%x) error.\n", format ) ;
		goto Bail;
	}

	pcf_tmp->ntables = getLSB32( (unsigned char *)(pcf_tmp->pcf_buffer + 4) );
	pcf_tmp->tables = (PCFTablePtr)(pcf_tmp->pcf_buffer + 8 );

	if ( !getAccel( 
	    &pcf_tmp->info, &maxink, pcf_tmp->pcf_buffer,
	    pcf_tmp->tables, pcf_tmp->ntables, (CARD32)PCF_BDF_ACCELERATORS
	    )
	    ) {
		if ( !getAccel( 
		    &pcf_tmp->info, &maxink, pcf_tmp->pcf_buffer,
		    pcf_tmp->tables, pcf_tmp->ntables, (CARD32)PCF_ACCELERATORS
		    )
		    ) {
			USAGE("dtgpftobdf : Cannot get accelerators.\n" ) ;
			goto Bail;
		}
	}

	if ( !getProperties( 
	    pcf_tmp, pcf_tmp->pcf_buffer, 
	    pcf_tmp->tables, pcf_tmp->ntables
	    )
	    ) {
		USAGE("dtgpftobdf : getProperties error.\n" ) ;
		goto Bail;
	}
	if ( !seekToType( 
	    pcf_tmp->tables, pcf_tmp->ntables, 
	    (CARD32)PCF_BITMAPS, &format, &size, &offset
	    )
	    ) {
		USAGE("dtgpftobdf : PCF_BITMAPS error.\n" ) ;
		goto Bail;
	}

	buffp = pcf_tmp->pcf_buffer + offset;

	format = getLSB32( (unsigned char *)buffp);
	buffp += 4;

	if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) ) {
		USAGE("dtgpftobdf : error.!PCF_FORMAT_MATCH(PCF_BITMAPS)\n" ) ;
		goto Bail;
	}

	pcf_tmp->nbitmaps = getINT32( (unsigned char *)buffp, format);
	buffp += 4;
	pcf_tmp->offsets = (CARD32 *)buffp;
	buffp += sizeof( *pcf_tmp->offsets) * pcf_tmp->nbitmaps;


	bitmapSizes = (CARD32 *)buffp;
	pcf_tmp->sizebitmaps 
	    = getINT32( (unsigned char *)&bitmapSizes[PCF_GLYPH_PAD_INDEX(format)], format);
	pcf_tmp->bmp_fmt = format;
	buffp += sizeof( *bitmapSizes) * GLYPHPADOPTIONS;
	pcf_tmp->bitmaps = buffp;
	buffp += pcf_tmp->sizebitmaps;

	if ( !seekToType( 
	    pcf_tmp->tables, pcf_tmp->ntables, 
	    (CARD32)PCF_BDF_ENCODINGS, &format, &size, &offset
	    )
	    ) {
		USAGE("dtgpftobdf : error.(PCF_BDF_ENCODINGS)\n" ) ;
		goto Bail;
	}

	buffp = pcf_tmp->pcf_buffer + offset;
	format = getLSB32( (unsigned char *)buffp);
	buffp += 4;
	if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) ) {
		USAGE("dtgpftobdf : error.!PCF_FORMAT_MATCH(PCF_BDF_ENCODINGS)\n" ) ;
		goto Bail;
	}

	pcf_tmp->info.firstCol = getINT16( (unsigned char *)buffp, format);
	buffp += 2;
	pcf_tmp->info.lastCol = getINT16( (unsigned char *)buffp, format);
	buffp += 2;
	pcf_tmp->info.firstRow = getINT16( (unsigned char *)buffp, format);
	buffp += 2;
	pcf_tmp->info.lastRow = getINT16( (unsigned char *)buffp, format);
	buffp += 2;
	pcf_tmp->info.chDefault = getINT16( (unsigned char *)buffp, format);
	buffp += 2;

	pcf_tmp->info.allExist  = FALSE;
	pcf_tmp->enc_fmt = format;
	pcf_tmp->encodingOffsets = (CARD16 *)buffp;

	/*
	* get scale width infomations 
	*/
	if ( !seekToType( 
	    pcf_tmp->tables, pcf_tmp->ntables, 
	    (CARD32)PCF_SWIDTHS, &format, &size, &offset
	    )
	    ) {
		goto Bail;
	}

	buffp = pcf_tmp->pcf_buffer + offset;
	format = getLSB32( (unsigned char*)buffp);
	buffp += 4;
	if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) ) {
		goto Bail;
	}
	pcf_tmp->swd_fmt = (CARD32)format ;
	pcf_tmp->swd_num = getINT32( (unsigned char*)buffp, format ) ;
	buffp += 4;

	pcf_tmp->swidth = (CARD32 *)buffp ;

	/*
	* get glyph names
	*/
	if ( !seekToType( 
	    pcf_tmp->tables, pcf_tmp->ntables, 
	    (CARD32)PCF_GLYPH_NAMES, &format, &size, &offset
	    )
	    ) {
		goto Bail;
	}

	buffp = pcf_tmp->pcf_buffer + offset;
	format = getLSB32( (unsigned char*)buffp);
	buffp += 4;
	if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) ) {
		goto Bail;
	}
	pcf_tmp->glyphs = getINT32( (unsigned char*)buffp, format ) ;
	buffp += 4;
	pcf_tmp->glyph_offset = (int *)buffp ;
	buffp += 4 * (pcf_tmp->glyphs + 1) ;
	pcf_tmp->glyph_name = (char *)buffp ;
	pcf_tmp->glyph_fmt = (CARD32)format ;

	/*
	* get metrics
	*/
	if ( !seekToType( 
	    pcf_tmp->tables, pcf_tmp->ntables, 
	    (CARD32)PCF_METRICS, &format, &size, &offset
	    )
	    ) {
		goto Bail;
	}

	buffp = pcf_tmp->pcf_buffer + offset;
	format = getLSB32( (unsigned char*)buffp);
	buffp += 4;
	if ( PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT ) ) {
	    pcf_tmp->compress = 0 ;
	    pcf_tmp->mtr_num = getINT32( (unsigned char*)buffp, format ) ;
	    buffp += 4;
	}else if ( PCF_FORMAT_MATCH( format, PCF_COMPRESSED_METRICS ) ) {
	    pcf_tmp->compress = 1 ;
	    pcf_tmp->mtr_num = (int)getINT16( (unsigned char*)buffp, format ) ;
	    buffp += 2;
	}else{
		goto Bail;
	}
	pcf_tmp->metrics = (xCharInfo *)buffp ;
	pcf_tmp->mtr_fmt = (CARD32)format ;

	return	pcf_tmp;

Bail:
	if ( pcf_tmp ) {
		free( pcf_tmp );
	}
	return	NULL;
}


static	void
Anafprop(fip,fpropPtr,fpropvnPtr,fontname,ps,res,fplistPtr,fplistNum)
FontInfoRec		*fip;
FontPropRec 	*fpropPtr;
char	*fpropvnPtr;
char	**fontname;
FontPropRec 	**fplistPtr;
FontPropRec 	*ps;
FontPropRec 	*res;
unsigned int	*fplistNum;
{
	FontPropRec 	*wkp ,*wklp;
	int 	i, fpnL;
	char	*fpnp;

	*fplistNum = fip->nProps - 1;
	*fplistPtr = (FontPropRec *)malloc(*fplistNum * sizeof(FontPropRec));
	wkp = fpropPtr;
	wklp = *fplistPtr;
	for (i = 0; i < fip->nProps; i++) {
		fpnp = fpropvnPtr + wkp->name;
		fpnL = strlen(fpnp);
		if ((fpnL == 4) && (strncmp(fpnp,"FONT",4) == 0)) {
			*fontname = fpropvnPtr + wkp->value;
			wkp++;
			continue;
		}
		if (fpnL == 10) {
			if (strncmp(fpnp,"POINT_SIZE",10) == 0) {
				ps->value = wkp->value;
			}
			if (strncmp(fpnp,"RESOLUTION",10) == 0) {
				res->value = wkp->value;
			}
		}
		wklp->name = (CARD32)fpropvnPtr + wkp->name;
		if (wkp->indirect == TRUE) {
			wklp->value = (INT32)fpropvnPtr + wkp->value;
		} else {
			wklp->value = wkp->value;
		}
		wklp->indirect = wkp->indirect;
		wkp++;
		wklp++;
	}
}



static void
setProp( pcf, proptmp )
PcfTmp	*pcf;
PropTmp 	*proptmp;
{
	int 	i, fpnL;
	char	*fpnp;

	for ( i = 0; i < pcf->nprops; i++ ) {
		fpnp = pcf->string + pcf->props[i].name;
		fpnL = strlen(fpnp);
		if ( ( fpnL == 4 ) && ( strncmp( fpnp,"FONT", 4 ) == 0 ) ) {
			proptmp->font_name = pcf->string + pcf->props[i].value;
			continue;
		}
		if ( fpnL == 10 ) {
			if ( strncmp( fpnp,"POINT_SIZE", 10 ) == 0 ) {
				proptmp->psize.value = pcf->props[i].value;
			} else if ( strncmp( fpnp, "RESOLUTION", 10 ) == 0 ) {
				proptmp->resolution.value = pcf->props[i].value;
			}
		}
	}
}



/*
 *  output to SIZE (SIZE point resolutionX resolutionY) 
 * 
 */
static	void
pSIZE( ps, res )
FontPropRec *ps, *res;
{
	int 	k;
	float	f;
	char	buf1[16], buf2[16];

	f = ps->value / 10.0;
	if ( ( ps->value % 10 ) != 0 ) {
		sprintf( buf1, "%.1f", f );
	} else {
		sprintf( buf1, "%.0f", f );
	}
	k = ( ( res->value * 72.27 ) / 100.0 ) + 1;
	sprintf( buf2, "%d %d", k, k );
	fprintf( stdout, "SIZE %s %s\n", buf1, buf2 );
	return;
}


/* output to SIZE of pcf font */

static void	
pSIZE_pcf( proptmp )
PropTmp 	*proptmp;
{
	int 	k;
	float	f;
	char	buf1[16],buf2[16];

	f = proptmp->psize.value / 10.0;
	if ( ( proptmp->psize.value % 10 ) != 0 ) {
		sprintf( buf1,"%.1f",f );
	} else {
		sprintf(buf1,"%.0f",f);
	}
	k = ( ( proptmp->resolution.value * 72.27 ) / 100.0 ) + 1;
	sprintf( buf2, "%d %d", k, k );
	fprintf( stdout,"SIZE %s %s\n", buf1, buf2 );
	return;
}



static	void
pPROPS( fip,fplistPtr,fplistNum )
FontInfoRec 	*fip;
FontPropRec 	*fplistPtr;
unsigned int 	fplistNum;
{
	FontPropRec 	*wkp;
	int 	i, k;

	bufp = buf;
	k = sprintf( bufp,"STARTPROPERTIES %d\n", fplistNum+3 );
	bufp += k;
	k = sprintf( bufp, "FONT_ASCENT %d\n", fip->fontAscent );
	bufp += k;
	k = sprintf( bufp, "FONT_DESCENT %d\n", fip->fontDescent );
	bufp += k;
	k = sprintf( bufp, "DEFAULT_CHAR %d\n", fip->chDefault );
	bufp += k;
	wkp = fplistPtr;
	for ( i = 0; i < fplistNum; i++ ) {
		if ( wkp->indirect == TRUE ) {
			k = sprintf( bufp, "%s \"%s\"\n", wkp->name, wkp->value );
			bufp += k;
		} else {
			k = sprintf( bufp, "%s %d\n", wkp->name, wkp->value );
			bufp += k;
		}
		wkp++;
	}
	k = sprintf( bufp, "ENDPROPERTIES\n" );
	bufp += k;
	*bufp = '\0';
	fprintf( stdout, "%s", buf );
	return;

}


/* output to font property information of pcf fontpcf */
static	void	
pPROPS_pcf( pcf )
PcfTmp	*pcf;
{
	FontPropPtr 	wkp;
	int 	i, k;

	bufp = buf;
	k = sprintf( bufp, "STARTPROPERTIES %d\n", pcf->nprops+3 );
	bufp += k;
	k = sprintf( bufp, "FONT_ASCENT %d\n", 
	    (pcf->info.fontAscent >= pcf->info.maxbounds.metrics.ascent)
	    ? pcf->info.fontAscent : pcf->info.maxbounds.metrics.ascent
	    );
	bufp += k;
	k = sprintf( bufp, "FONT_DESCENT %d\n",
	    (pcf->info.fontDescent >= pcf->info.maxbounds.metrics.descent)
	    ? pcf->info.fontDescent : pcf->info.maxbounds.metrics.descent
	    );
	bufp += k;
	k = sprintf( bufp, "DEFAULT_CHAR %d\n", pcf->info.chDefault );
	bufp += k;
	wkp = pcf->props;
	for ( i = 0; i < pcf->nprops; i++ ) {
		if ( pcf->isStringProp[i] ) {
			k = sprintf(bufp,"%s \"%s\"\n", 
			    pcf->string + wkp->name, pcf->string + wkp->value
			    );
			bufp += k;
		} else {
			k = sprintf(bufp,"%s %d\n", 
			    pcf->string + wkp->name, wkp->value
			    );
			bufp += k;
		}
		wkp++;
	}
	k = sprintf( bufp, "ENDPROPERTIES\n" );
	bufp += k;
	*bufp = '\0';
	fprintf( stdout,"%s",buf );
	return;

}



/*
 * output to character information and patern
 *
 */
static	void	
pCHARS(fip,cip,glyphPtr,charNum,glyphPad)
FontInfoRec 	*fip;
CharInfoRec 	*cip;
char	*glyphPtr;
unsigned int	charNum;
unsigned int	glyphPad;

{
	CharInfoRec 	*wkp;
	int 	i, j;
	unsigned int	frow, lrow, row, fcol, lcol, col;
	unsigned int	bbw, bbh;
	unsigned int	bml;
	char	*glyph;

	fprintf( stdout, "CHARS %d\n", charNum );

	frow = fip->firstRow;
	lrow = fip->lastRow;
	fcol = fip->firstCol;
	lcol = fip->lastCol;
	wkp = cip;
	glyph = glyphPtr;
	row = frow;

	for ( i = 0; i < 256; i++ ) {
		cvtp[i] = &cvt[i];
	}

	for ( row = frow; row <= lrow; row++ ) {
		for ( col = fcol; col <= lcol; col++ ) {
			if ( wkp->exists == FALSE ) {
				wkp++;
				continue;
			}
			fprintf( stdout, "STARTCHAR %.2x%.2x\n", row,col );
			fprintf( stdout, "ENCODING %d\n", (row << 8) + col );
			fprintf( stdout, "SWIDTH 256 0\nDWIDTH %d %d\n",
			    wkp->metrics.characterWidth,0 );
			bbw = wkp->metrics.rightSideBearing
			    - wkp->metrics.leftSideBearing;
			bbh = wkp->metrics.ascent + wkp->metrics.descent;
			fprintf( stdout, "BBX %d %d %d %d\nBITMAP\n", bbw, bbh,
			    wkp->metrics.leftSideBearing,
			    - ( wkp->metrics.descent ));
			bml = (bbw + 7) / 8;
			for ( i = 0; i < bbh; i++ ) {
				bufp = buf;
				for ( j = 0; j < bml; j++ ) {
					sprintf( bufp, "%s", cvtp[(unsigned char)glyph[j]] );
					bufp += 2;
				}
				fprintf( stdout, "%.*s\n", bml*2, buf );
				glyph += glyphPad;
			}
			fprintf( stdout,"ENDCHAR\n" );
			wkp++;
		}
	}
	return;
}


/*
 * output to character information and patern
 *
 */
static	void	
pfixCHARS(fip,cip,glyphPtr,charNum,glyphPad)
FontInfoRec 	*fip;
CharInfoRec 	*cip;
char	*glyphPtr;
unsigned int	charNum;
unsigned int	glyphPad;
{
	CharInfoRec 	*wkp;
	register int	i, j, k;
	unsigned int	frow,lrow,fcol,lcol;
	register int	row, col;
	unsigned int	bbw, bbh, bbox, bboy;
	unsigned int	xd, yd;
	unsigned int	bml;
	register char	*glyph;
	char	fixbuf[240], *fixbufp;
	int 	fixdl;

	fprintf( stdout,"CHARS %d\n",charNum );

	for ( i = 0; i < 256; i++ ) {
		cvtp[i] = &cvt[i];
	}

	frow = fip->firstRow;
	lrow = fip->lastRow;
	fcol = fip->firstCol;
	lcol = fip->lastCol;
	xd =  cip->metrics.characterWidth;
	yd = 0;
	bbw = cip->metrics.rightSideBearing - cip->metrics.leftSideBearing;
	bbh = cip->metrics.ascent + cip->metrics.descent;
	bbox = cip->metrics.leftSideBearing;
	bboy = -(cip->metrics.descent);

	fixbufp = fixbuf;
	fixdl = sprintf( fixbufp, "SWIDTH 256 0\nDWIDTH %d %d\n", xd, yd );
	fixbufp += fixdl;
	k = sprintf(fixbufp,"BBX %d %d %d %d\nBITMAP\n", 
	    bbw, bbh, bbox, bboy 
	    );
	fixdl += k;

	bml = (bbw + 7) / 8;
	wkp = cip;
	glyph = glyphPtr;
	for ( row = frow; row <= lrow; row++ ) {
		for ( col = fcol; col <= lcol; col++ ) {
			if ( wkp->exists == FALSE ) {
				wkp++;
				continue;
			}
			bufp = buf;
			memcpy(bufp,"STARTCHAR ",10);
			bufp += 10;
			memcpy(bufp,cvtp[row],2);
			bufp += 2;
			memcpy(bufp,cvtp[col],3);
			bufp += 3;
			memcpy(bufp,"ENCODING ",9);
			bufp += 9;
			k = sprintf(bufp,"%d\n",(row << 8) + col);
			bufp += k;
			memcpy(bufp,fixbuf,fixdl);
			bufp += fixdl;
			for (i = 0; i < bbh; i++) {
				for (j = 0; j < bml; j++) {
					memcpy(bufp, cvtp[(unsigned char)glyph[j]],3);
					bufp += 2;
				}
				bufp ++;
				glyph += glyphPad;
			}
			memcpy( bufp, "ENDCHAR\n", 8 );
			bufp += 8;
			*bufp = '\0';
			fprintf( stdout, "%s", buf );
			wkp++;
		}
	}
	return;
}


static void
putPtn( bits, width, height)
unsigned char   *bits;          /* 1 byte boundary , no byte swap data */
int     width, height;
{
	int     bytewidth;
	int     i, j;

	bytewidth = ( width + 7) / 8;

	for ( i = height; i-- > 0;) {
		for ( j = bytewidth; j-- > 0; bits++) {
			fprintf(stdout, "%.2x", *bits);
		}
		fprintf(stdout, "\n");
	}

}

static void
ByteSwap( p, scan)
char    *p;
int     scan;
{
	char    w;

	switch( scan) {
	case 1:
		break;
	case 2:
		w = *p;
		*p = *(p + 1);
		*(p + 1) = w;
		break;
	case 4:
		w = *p;
		*p = *(p + 3);
		*(p + 3) = w;
		w = *(p + 1);
		*(p + 1) = *(p + 2);
		*(p + 2) = w;
		break;
	}
}

static unsigned char _reverse_byte[0x100] = {
		        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
		        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
		        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
		        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
		        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
		        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
		        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
		        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
		        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
		        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
		        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
		        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
		        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
		        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
		        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
		        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
		        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
		        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
		        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
		        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
		        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
		        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
		        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
		        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
		        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
		        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
		        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
		        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
		        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
		        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
		        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
		        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

static	void
BitOrderInvert(buf, nbytes)
register unsigned char *buf;
register int nbytes;
{
	register unsigned char *rev = _reverse_byte;

	for (; --nbytes >= 0; buf++)
		*buf = rev[*buf];
}

static void
invertBits( src, format, width, height, dest)
char    *src;
CARD32  format;
int     width, height;
char    *dest;
{
	int     bit, byte, glyph, scan;
	int     src_bytewidth, dest_bytewidth;
	char    work[8];
	int     i, j;

	bit = PCF_BIT_ORDER( format);
	byte = PCF_BYTE_ORDER( format);
	glyph = PCF_GLYPH_PAD( format);
	scan = PCF_SCAN_UNIT( format);

	src_bytewidth = (( width + ( 8 * glyph ) - 1)/( 8 * glyph)) * glyph;
	dest_bytewidth = ( width + 7) /8;

	for ( i = 0; i < height; i++, src += src_bytewidth,
	    dest += dest_bytewidth) {
		for ( j = 0; j < src_bytewidth; j += scan) {
			memcpy( work, src + j, scan);
			if ( bit == LSBFirst)
				BitOrderInvert( (unsigned char *)work, scan);
			if ( byte == LSBFirst)
				ByteSwap( work, scan);
			if (( j + scan) >= dest_bytewidth) {
				memcpy( dest + j, work, dest_bytewidth - j);
				break;
			}
			memcpy( dest + j, work, scan);
		}
	}

}

static	void
pCHARS_pcf (pcf, fix)
PcfTmp  *pcf;
int	fix ;
{
	char    *bmp_ptn;
	char    *bitmap;
	int     ptn_width, ptn_height;
	int     bmp_width, bmp_height;
	int     cpy_height;
	int     row, col;
	int     row_width;
	int     encoding;
	int     encodingOffset;
	int     nencoding;
	int     bmp_adj, ptn_adj;
	int     adj_hi;
	int     width_bytes;
	INT16	bl, br, ba, bd, bdw ;
	unsigned char	cl, cr, ca, cd, cdw ;
	unsigned char	*comp ;
	int	bbw, bbh, bbx, bby, dwidth ;

	fprintf(stdout,"CHARS %d\n",pcf->nbitmaps);

	row_width = pcf->info.lastCol - pcf->info.firstCol + 1;
	nencoding = row_width * (pcf->info.lastRow - pcf->info.firstRow + 1);

	if( fix == TRUE ) {
	    bmp_width = pcf->info.maxbounds.metrics.leftSideBearing 
		    + pcf->info.maxbounds.metrics.rightSideBearing ;
	    bmp_height = pcf->info.fontAscent + pcf->info.fontDescent ;
	    ptn_width  = bmp_width ;
	    ptn_height = pcf->info.maxbounds.metrics.ascent +
			pcf->info.maxbounds.metrics.descent;

	    if (!(bmp_ptn = make_toptn( ptn_width, ptn_height)))
		    return;

	    if ( ( adj_hi = pcf->info.fontAscent - pcf->info.maxbounds.metrics.ascent ) > 0) {
		    width_bytes =  8 * PCF_GLYPH_PAD( pcf->bmp_fmt);
		    width_bytes = (( width_bytes + bmp_width - 1)/width_bytes) * PCF_GLYPH_PAD( pcf->bmp_fmt);
		    bmp_adj = width_bytes * adj_hi;
		    ptn_adj = 0;
		    if (( cpy_height = bmp_height - adj_hi) > ptn_height)
			    cpy_height = ptn_height ;
	    } else if ( adj_hi < 0) {
		    adj_hi *= -1;
		    width_bytes = ( ptn_width + 7) / 8;
		    bmp_adj = 0;
		    ptn_adj = width_bytes * adj_hi;
		    if (( cpy_height = ptn_height - adj_hi) > bmp_height)
			    cpy_height = bmp_height ;
	    } else {
		    bmp_adj = 0;
		    ptn_adj = 0;
		    cpy_height = ptn_height ;
	    }
	}


	for ( encoding = 0; encoding < nencoding; encoding++) {
		if(fix == TRUE)	memset( bmp_ptn, '\0', (ptn_width + 7)/8 * ptn_height);
		encodingOffset = getINT16( (unsigned char *)(pcf->encodingOffsets + encoding), pcf->enc_fmt);
		if (encodingOffset == 0xFFFF) continue;

		row = pcf->info.firstRow + encoding / row_width;
		col = pcf->info.firstCol + encoding % row_width;

		fprintf(stdout,"STARTCHAR %s\n",pcf->glyph_name + 
                   getINT32( (unsigned char*)&pcf->glyph_offset[encodingOffset],
                   pcf->glyph_fmt) );
		fprintf(stdout,"ENCODING %d\n",(row << 8) + col);

		fprintf( stdout,"SWIDTH %d 0\n", getINT32( (unsigned char*)(pcf->swidth + encodingOffset), pcf->swd_fmt) );

		if( pcf->compress ){
		    comp = (unsigned char*)pcf->metrics + 
				encodingOffset * SIZEOF_COMPRESSED_METRIC ;
		    cl  = getINT8( comp ) ;
		    cr  = getINT8( comp ) ;
		    cdw = getINT8( comp ) ;
		    ca  = getINT8( comp ) ;
		    cd  = getINT8( comp ) ;

		    dwidth = (int)(cdw - 0x80) ;
		    bbw = (int)((cr - 0x80) - (cl - 0x80)) ;
		    bbh = (int)((ca - 0x80) + (cd - 0x80)) ;
		    bbx = (int)(cl - 0x80) ;
		    bby = (int)( -(cd - 0x80) ) ;
		}else{
		    bl = getINT16( (unsigned char *)&pcf->metrics[encodingOffset].leftSideBearing, pcf->mtr_fmt ) ;
		    br = getINT16( (unsigned char *)&pcf->metrics[encodingOffset].rightSideBearing, pcf->mtr_fmt ) ;
		    bdw = getINT16( (unsigned char *)&pcf->metrics[encodingOffset].characterWidth, pcf->mtr_fmt ) ;
		    ba = getINT16( (unsigned char *)&pcf->metrics[encodingOffset].ascent, pcf->mtr_fmt ) ;
		    bd = getINT16( (unsigned char *)&pcf->metrics[encodingOffset].descent, pcf->mtr_fmt ) ;
		    dwidth = bdw ;
		    bbw = br-bl ;
		    bbh = ba+bd ;
		    bbx = bl ;
		    bby = -bd ;
		}
		fprintf( stdout,"DWIDTH %d 0\n", dwidth ) ;
		fprintf( stdout,"BBX %d %d %d %d\nBITMAP\n", bbw, bbh, bbx, bby ) ;

		if( fix == FALSE ) {
		    bmp_width = pcf->info.maxbounds.metrics.leftSideBearing 
			    + pcf->info.maxbounds.metrics.rightSideBearing ;
		    bmp_height = pcf->info.fontAscent + pcf->info.fontDescent ;
		    ptn_width  = bbw ;
		    ptn_height = bbh ;

		    if (!(bmp_ptn = make_toptn( ptn_width, ptn_height)))
			    return;

		    if ( ( adj_hi = pcf->info.fontAscent - pcf->info.maxbounds.metrics.ascent ) > 0) {
			width_bytes =  8 * PCF_GLYPH_PAD( pcf->bmp_fmt);
			width_bytes = (( width_bytes + bmp_width - 1)/width_bytes) * PCF_GLYPH_PAD( pcf->bmp_fmt);
			bmp_adj = width_bytes * adj_hi;
			ptn_adj = 0;
			if (( cpy_height = bmp_height - adj_hi) > ptn_height)
				cpy_height = ptn_height ;
		    } else if ( adj_hi < 0) {
			adj_hi *= -1;
			width_bytes = ( ptn_width + 7) / 8;
			bmp_adj = 0;
			ptn_adj = width_bytes * adj_hi;
			if (( cpy_height = ptn_height - adj_hi) > bmp_height)
				cpy_height = bmp_height ;
		    } else {
			bmp_adj = 0;
			ptn_adj = 0;
			cpy_height = ptn_height ;
		    }
		    memset( bmp_ptn, '\0', (ptn_width + 7)/8 * ptn_height);
		}

		bitmap = pcf->bitmaps + getINT32( (unsigned char *)(pcf->offsets + encodingOffset),
		    pcf->bmp_fmt);
		invertBits( bitmap + bmp_adj, pcf->bmp_fmt, ptn_width, cpy_height, bmp_ptn + ptn_adj);
		putPtn( (unsigned char *)bmp_ptn, ptn_width, ptn_height );
		fprintf(stdout,"ENDCHAR\n");
		if( fix == FALSE )	free(bmp_ptn);
	}
	if( fix == TRUE )	free(bmp_ptn);
	return;
}

static	char *
make_toptn( width, height)
int     width, height;
{
	int     byte_width;

	byte_width = (width + 7)/8;

	return (char *)malloc( byte_width * height);
}




static	int 	
getarg(argc,argv,fname,bitorder,byteorder,scanunit,glyphPad,outLevel,fix)
int  argc;
char *argv[];
char *fname;
int  *bitorder;
int  *byteorder;
int  *scanunit;
int  *glyphPad;
int  *outLevel;
int  *fix;
{
	int i;
	int already;
	char *arg;
	char *usage="dtgpftobdf [-H] [-V] [-help]  font_file_name";

	*glyphPad = DEFAULTGLPAD;
	*bitorder = DEFAULTBITORDER;
	*scanunit = DEFAULTSCANUNIT;
	*byteorder = DEFAULTBYTEORDER;
	*outLevel = FALSE;
	*fix = FALSE ;
	already = 0 ;

	for (i = 1; i < argc; i++ ) {
		arg = argv[i];
		if (*arg == '-') {
			*arg++;
			switch (*arg) {
			case 'p' :
				*glyphPad = atoi(arg+1) ;
				if (*glyphPad != 1 &&
				    *glyphPad != 2 &&
				    *glyphPad != 4 &&
				    *glyphPad != 8 ) {
					USAGE("dtgpftobdf : illegal padding number (1/2/4/8)\n");
					return	-1;
				}
				break;
			case 'H' :
				*outLevel = TRUE ;
				break;
			case 'h' :
				USAGE1("usage: %s\n",usage);
				return (-1);
			case 'V' :
				*fix = TRUE ;
				break;
			default :
				USAGE1("dtgpftobdf : illegal option -- %s\n",arg);
				USAGE1("usage: %s\n",usage);
				return	-1;
			}

		} else {
			if (already == FALSE) {
				strcpy(fname,arg);
				already = TRUE;
			} else {
				USAGE1("dtgpftobdf : extra parameter --- %s\n",arg);
				USAGE1("usage: %s\n", usage );
				return	-1;
			}
		}
	}
	if (already == FALSE) {
		USAGE("dtgpftobdf : require file name\n");
		USAGE1("usage: %s\n",usage);
		return	-1;
	}
	return	0;
}

