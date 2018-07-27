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
/* $XConsortium: readpcf.c /main/5 1996/06/05 16:42:29 ageorge $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<errno.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<X11/Intrinsic.h>
#include	"FaLib.h"
#include	"falfont.h"

#ifndef GLYPHPADOPTIONS
#define	GLYPHPADOPTIONS	4
#endif

static	CARD32	getLSB32();
static	int	getINT32();
static	int	getINT16();
static	Bool	seekToType();
static	void	getMetric();
static	Bool	getAccel();
int 	falInitReadPcf();
static 	void	ByteSwap();
static	void	repadBits();
int 	falPcfGlyph();
void	falGetPcfGSize();
int	falInitReadPcfProp() ;
int	falInitReadSnfProp() ;
int	falReadGpfProp() ;

extern	void	BitOrderInvert() ;
extern	void    set_errfile_str() ;

static CARD32
getLSB32(unsigned char *p)
{
	CARD32	c;

	c = *p++;
	c |= (CARD32)(*p++) << 8;
	c |= (CARD32)(*p++) << 16;
	c |= (CARD32)(*p) << 24;

	return c;
}

static int
getINT32(unsigned char *p, CARD32 format)
{
	CARD32         c;

	if (PCF_BYTE_ORDER(format) == MSBFirst) {
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

	return (int)c;
}

static int
getINT16(unsigned char *p, CARD32 format)
{
	CARD32         c;

	if (PCF_BYTE_ORDER(format) == MSBFirst) {
		c = (CARD32)(*p++) << 8;
		c |= (CARD32)(*p);
	} else {
		c = (CARD32)(*p++);
		c |= (CARD32)(*p) << 8;
	}

	return (int)c;
}

static Bool
seekToType(
PCFTablePtr tables,
int         ntables,
CARD32      type,
CARD32	*formatp,
CARD32	*sizep,
CARD32	*offsetp)
{
	int	i;

	for ( i = 0; i < ntables; i++) {
		if ( getLSB32( (unsigned char *)&tables[i].type ) == type) {
			if ( formatp)
				*formatp = getLSB32( (unsigned char *)&tables[i].format);
			if ( sizep)
				*sizep = getLSB32( (unsigned char *)&tables[i].size);
			if ( offsetp)
				*offsetp = getLSB32( (unsigned char *)&tables[i].offset);
			return(TRUE);
		}
	}
	return(FALSE);
}



static void
getMetric(caddr_t buf, CARD32 format, xCharInfo *metric)
{
	metric->leftSideBearing = getINT16( (unsigned char *)buf, (CARD32)format);
	buf += 2;
	metric->rightSideBearing = getINT16( (unsigned char *)buf, (CARD32)format);
	buf += 2;
	metric->characterWidth = getINT16( (unsigned char *)buf, (CARD32)format);
	buf += 2;
	metric->ascent = getINT16( (unsigned char *)buf, (CARD32)format);
	buf += 2;
	metric->descent = getINT16( (unsigned char *)buf, (CARD32)format);
	buf += 2;
	metric->attributes = getINT16( (unsigned char *)buf, (CARD32)format);
	buf += 2;
}

static Bool
getAccel(
FontInfoPtr	pFontInfo,
xCharInfo	*maxink,
caddr_t		buf_top,
PCFTablePtr	tables,
int		ntables,
CARD32		type)
{
	CARD32	format;
	CARD32	offset;
	caddr_t	buffer;

	if ( !seekToType( tables, ntables, (CARD32)type, &format, (CARD32 *)NULL, &offset))
		return FALSE;

	buffer = buf_top + offset;
	format = getLSB32( (unsigned char *)buffer );
	buffer += 4;
	if (!PCF_FORMAT_MATCH(format, PCF_DEFAULT_FORMAT) &&
	    !PCF_FORMAT_MATCH(format, PCF_ACCEL_W_INKBOUNDS)) {
		return FALSE;
	}
	pFontInfo->noOverlap = *buffer++;
	pFontInfo->constantMetrics = *buffer++;
	pFontInfo->terminalFont = *buffer++;
	pFontInfo->constantWidth = *buffer++;
	pFontInfo->inkInside = *buffer++;
	pFontInfo->inkMetrics = *buffer++;
	pFontInfo->drawDirection = *buffer++;
	/*  pFontInfo->anamorphic = FALSE; */
	/* natural alignment */ buffer++;
	pFontInfo->fontAscent = getINT32( (unsigned char *)buffer, (CARD32)format);
	buffer += 4;
	pFontInfo->fontDescent = getINT32( (unsigned char *)buffer, (CARD32)format);
	buffer +=4;

	/*  pFontInfo->maxOverlap = getINT32( (unsigned char *)buffer, (CARD32)format); */ buffer += 4;
	getMetric(buffer, format, &pFontInfo->minbounds);
	buffer += 12;
	getMetric(buffer, format, &pFontInfo->maxbounds);
	buffer += 12;
	if (PCF_FORMAT_MATCH(format, PCF_ACCEL_W_INKBOUNDS)) {
		buffer += 12;
		getMetric( buffer, format, maxink);
	} else {
		*maxink = pFontInfo->maxbounds;
	}
	return TRUE;
}

int
falInitReadPcf(struct pcf_inf *pcfinf, caddr_t buftop)
{
	CARD32	format;
	CARD32	offset;
	CARD32      *bitmapSizes;
	xCharInfo	maxink;
	caddr_t	buffp;

	if ( getLSB32( (unsigned char *)buftop ) != PCF_FILE_VERSION)
		return -1;

	pcfinf->ntables = getLSB32( (unsigned char *)(buftop + 4) );

	pcfinf->tables = (PCFTablePtr)(buftop + 8);

	if ( !getAccel( &pcfinf->info, &maxink, buftop, pcfinf->tables, pcfinf->ntables,
	    (CARD32)PCF_BDF_ACCELERATORS))
		if ( !getAccel( &pcfinf->info, &maxink, buftop, pcfinf->tables, pcfinf->ntables,
		    (CARD32)PCF_ACCELERATORS))
			return -1;

	pcfinf->org_bounds = pcfinf->info.maxbounds;

	if ( !seekToType( pcfinf->tables, pcfinf->ntables, (CARD32)PCF_BITMAPS,
			&format, (CARD32 *)NULL, &offset))
		return -1;

	buffp = buftop + offset;

	format = getLSB32( (unsigned char *)buffp );
	buffp += 4;
	if (!PCF_FORMAT_MATCH(format, PCF_DEFAULT_FORMAT))
		return -1;

	pcfinf->nbitmaps = getINT32( (unsigned char *)buffp, (CARD32)format);
	buffp += 4;
	pcfinf->offsets = (CARD32 *)buffp;
	buffp += sizeof( *pcfinf->offsets) * pcfinf->nbitmaps;

	bitmapSizes = (CARD32 *)buffp;
	pcfinf->sizebitmaps = getINT32( (unsigned char *)&bitmapSizes[PCF_GLYPH_PAD_INDEX(format)], (CARD32)format);
	pcfinf->bmp_fmt = format;
	buffp += sizeof( *bitmapSizes) * GLYPHPADOPTIONS;
	pcfinf->bitmaps = buffp;
	buffp += pcfinf->sizebitmaps;

	if ( !seekToType( pcfinf->tables, pcfinf->ntables, (CARD32)PCF_BDF_ENCODINGS,
			&format, (CARD32 *)NULL, &offset))
		return -1;

	buffp = buftop + offset;
	format = getLSB32( (unsigned char *)buffp );
	buffp += 4;
	if (!PCF_FORMAT_MATCH(format, PCF_DEFAULT_FORMAT))
		return -1;

	pcfinf->info.firstCol	= getINT16( (unsigned char *)buffp, (CARD32)format);
	buffp += 2;
	pcfinf->info.lastCol	= getINT16( (unsigned char *)buffp, (CARD32)format);
	buffp += 2;
	pcfinf->info.firstRow	= getINT16( (unsigned char *)buffp, (CARD32)format);
	buffp += 2;
	pcfinf->info.lastRow	= getINT16( (unsigned char *)buffp, (CARD32)format);
	buffp += 2;
	/*
	    pcfinf->info.defaultCh	= getINT16( (unsigned char *)buffp, (CARD32)format); buffp += 2;
	*/
	pcfinf->info.defaultCh	= getINT16( (unsigned char *)buffp, (CARD32)format);
	buffp += 2;

	pcfinf->info.allExist	= FALSE;
	pcfinf->enc_fmt = format;
	pcfinf->encodingOffsets = (CARD16 *)buffp;

	return 0;
}

static void
ByteSwap(char *p, int scan)
{
	char	w;

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
static void
repadBits(char *src, CARD32 format, int width, int height, char *dest)
{
	int	bit, byte, glyph, scan;
	int	src_bytewidth, dest_bytewidth;
	char	work[8];
	int	i, j;

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
				BitOrderInvert( work, scan );
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

int
falPcfGlyph(char *glyph, Oak_FontInf *finf, int code)
{
	int encode;
	int inner_code;
	char *bitmap;
	int encodingOffset;
	int codeRow, codeCol;
	int bytewidth;
	int bmp_adj, ptn_adj;
	int adj_hi;
	int cpy_height;
	int bmp_height;

	if ( !glyph){
		fal_utyerrno = FAL_ERR_PARM ;
		return FAL_ERROR ;
	}

	inner_code = code;
	codeCol = inner_code & 0xff;
	codeRow = (inner_code >> 8) & 0xff;

	/* code check */
	if (
	    ((code < finf->start) || (code > finf->end))||
	    ((codeCol < finf->pFinf->firstCol)||(codeCol > finf->pFinf->lastCol))||
	    ((codeRow < finf->pFinf->firstRow)||(codeRow > finf->pFinf->lastRow))
	    ) {
		fal_utyexists = 1;
		return(-1);
	}

	encode = (codeRow - finf->pFinf->firstRow) * ( finf->pFinf->lastCol - finf->pFinf->firstCol + 1);
	encode += codeCol - finf->pFinf->firstCol;
	encodingOffset = getINT16( (unsigned char *)(finf->pcfinf.encodingOffsets + encode), finf->pcfinf.enc_fmt);

	if (encodingOffset == 0xFFFF) {
		fal_utyexists = 1;
		return(-1);
	}
	fal_utyexists = 0;

	bitmap = finf->pcfinf.bitmaps + getINT32( (unsigned char *)(finf->pcfinf.offsets + encodingOffset), finf->pcfinf.bmp_fmt);

	bmp_height = finf->pFinf->maxbounds.ascent
	    + finf->pFinf->maxbounds.descent;
	if (( adj_hi = finf->pFinf->maxbounds.ascent
	    - finf->pcfinf.org_bounds.ascent) > 0) {
		bytewidth = 8 * PCF_GLYPH_PAD( finf->pcfinf.bmp_fmt);
		bytewidth = (( finf->width + bytewidth - 1)/ bytewidth ) * PCF_GLYPH_PAD( finf->pcfinf.bmp_fmt);
		bmp_adj = bytewidth * adj_hi;
		ptn_adj = 0;
		if (( cpy_height = bmp_height - adj_hi) > finf->height)
			cpy_height = finf->height;
	} else if ( adj_hi < 0) {
		adj_hi *= -1;
		bytewidth = ( finf->width + 7) / 8;
		bmp_adj = 0;
		ptn_adj = bytewidth * adj_hi;
		if (( cpy_height = finf->height - adj_hi) > bmp_height)
			cpy_height = bmp_height;
	} else {
		bmp_adj = 0;
		ptn_adj = 0;
		cpy_height = finf->height;
	}

	repadBits( bitmap + bmp_adj , finf->pcfinf.bmp_fmt, finf->width, cpy_height, glyph + ptn_adj);
	return(0);
}

void
falGetPcfGSize(
struct pcf_inf *pcfinf,
unsigned int   *widthp,
unsigned int   *heightp)
{
	unsigned int	w, h;

	w = pcfinf->org_bounds.rightSideBearing
	    - pcfinf->org_bounds.leftSideBearing;
	h = pcfinf->org_bounds.ascent + pcfinf->org_bounds.descent;

	*widthp = w;
	*heightp = h;
}




/********************************************************
 *
 * functions for collect GPF file properties
 *
 *******************************************************/

#include <X11/fonts/fontstruct.h>

static	char	*getPcfFontProp();
static	char	*getSnfFontProp();
/*
* read properties and get font style and
* letter size
*/

int
falReadFontProp(
char			*file,		/* name of font file */
int			protect_key_data,
FalFontData		*databuff,
int			islock)
{
	Oak_FontInf	finf;
	int		fd ;
	char		*buf;
	char		*openfontfile;
	struct	stat	st;
	int		rtn ;

	/* initialezation  */
	openfontfile = file ;

	/* read a condition of a fontfile */
	if ( stat( openfontfile, &st ) < 0 ) {
		set_errfile_str( fal_err_file, openfontfile ) ;
		fal_utyerror = _FAL_STAT_ER;
		fal_utyderror = errno;
		fal_utyerrno = FAL_ERR_STAT ;
		return	FAL_ERROR;
	}

	if ( st.st_size < sizeof( FontInfoRec ) ) {
		fal_utyerror = _FAL_FONT_ER;
		fal_utyderror = 0;
		fal_utyerrno = FAL_ERR_FONT ;
		return	FAL_ERROR;
	}

	/* open a fontfile */
	if ( (fd = open( openfontfile, ((islock)? O_RDONLY : O_RDWR) )) < 0 ) {
	     switch( errno ) {
		case EACCES :
		    return _FAL_TRY_NEXT ;
		default :
		    set_errfile_str( fal_err_file, openfontfile ) ;
		    fal_utyerror = _FAL_OPEN_ER;
		    fal_utyderror = errno;
		    fal_utyerrno = FAL_ERR_FNT_OPN ;
		    return	FAL_ERROR;
	     }
	}

#if	defined( SVR4 )
	buf = (char *)mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if ( buf != (char *)-1 ) {
		/* if "mmap" is normal end */
		close( fd );
		finf.ismmap = TRUE;
		finf.fsize = st.st_size;
	} else {
		/* if "mmap" is abnormal end , try "read()" */
		finf.ismmap = FALSE;
		if ( !(buf = (char *)malloc(st.st_size)) ) {
			fal_utyerror = _FAL_MALOC_ER;
			fal_utyderror = 0;
			fal_utyerrno = FAL_ERR_MALLOC ;
			close( fd );
			return	FAL_ERROR;
		}
		if ( read(fd, buf, st.st_size) != st.st_size ) {
			set_errfile_str( fal_err_file, openfontfile ) ;
			fal_utyerror = _FAL_READ_ER;
			fal_utyderror = errno;
			fal_utyerrno = FAL_ERR_FNT_RD ;
			free( buf );
			close( fd );
			return	FAL_ERROR;
		}
	}
#else
	finf.ismmap = FALSE;
	if ( !(buf = (char *)malloc( st.st_size )) ) {
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyderror = 0;
		fal_utyerrno = FAL_ERR_MALLOC ;
		close( fd );
		return	FAL_ERROR;
	}
	if ( read(fd, buf, st.st_size) != st.st_size ) {
		set_errfile_str( fal_err_file, openfontfile ) ;
		fal_utyerror = _FAL_READ_ER;
		fal_utyderror = errno;
		fal_utyerrno = FAL_ERR_FNT_RD ;
		free( buf );
		close( fd );
		return	FAL_ERROR;
	}
#endif

	finf.fname = openfontfile ;
	finf.buf   = buf ;

	/*
	*	open GPF font file
	*/
	rtn = falReadGpfProp( FAL_UPDATE_FONTINFO, &finf, protect_key_data, databuff ) ;
#if	defined( SVR4 )
	if ( finf.ismmap == TRUE ) {
		munmap( buf, finf.fsize );
	} else {
		free(buf);
		close(fd);
	}
#else
	free( buf );
	close( fd );
#endif

	return( rtn ) ;
}


/*
*	get properties of GPF format file
*/
int
falReadGpfProp(
int		updflg,
Oak_FontInf	*finf,
int		protect_key_data,
FalFontData	*databuff)
{
	char		*openfontfile;
	int		rtn ;

	/* initialize  */
	openfontfile = finf->fname ;

	if( updflg == FAL_UPDATE_FONTINFO ) {
	    /* case of a PCF format font */
	    if ( strcmp( FILE_SUFFIX( openfontfile ), PCFSUFFIX ) == 0 ) {

		finf->isFef = FALSE;
		finf->isPcf = TRUE;

	    }
	    else	/* case of a SNF format font */
	    {
		FAL_READ_SNF_HEADER( finf, protect_key_data, fal_utyerror, fal_utyderror ) ;
	    }
	}	/* updflg */

	/*
	*	get font properties
	*/
	if ( finf->isFef ){
		/* read SNF format property */
		if ( rtn = falInitReadSnfProp( finf, (caddr_t)finf->pFinf, databuff ) ) {
			if( rtn == _FAL_TRY_NEXT ){
				return( rtn ) ;
			}
			fal_utyerror = _FAL_FONT_ER;
			fal_utyderror = 0;
			return	FAL_ERROR;
		}
	} else if ( finf->isPcf ) {
		/* read PCF format property */
		if ( rtn = falInitReadPcfProp( updflg, finf, databuff ) ) {
			if( rtn == _FAL_TRY_NEXT ){
				return( rtn ) ;
			}
			fal_utyerror = _FAL_FONT_ER;
			fal_utyderror = 0;
			return	FAL_ERROR;
		}

		if( updflg == FAL_UPDATE_FONTINFO ) {
			finf->pFinf = &finf->pcfinf.info;
			finf->pCinf = NULL;
			finf->pGlyphs = NULL;
		}
	} else {
		fal_utyerror = _FAL_FONT_ER;
		fal_utyderror = 0;
		fal_utyerrno = FAL_ERR_FONT ;
		return	FAL_ERROR;
	}

	return(0);
}


/*
*	get properties of PCF format file
*/
int
falInitReadPcfProp(int updflg, Oak_FontInf *finf, FalFontData *databuff)
{
	struct pcf_inf	*pcfinf;
	caddr_t		buftop;
	xCharInfo	maxink;
	caddr_t	buffp;
	int	lb, rb, as, ds ;



	pcfinf = &finf->pcfinf ;
	buftop = finf->buf ;

	if ( getLSB32( (unsigned char *)buftop ) != PCF_FILE_VERSION)
		return _FAL_TRY_NEXT ;

	if( updflg == FAL_UPDATE_FONTINFO ) {
		pcfinf->ntables = getLSB32( (unsigned char *)(buftop + 4) );

		pcfinf->tables = (PCFTablePtr)(buftop + 8);

		if ( !getAccel( &pcfinf->info, &maxink, buftop, pcfinf->tables,
			pcfinf->ntables, (CARD32)PCF_BDF_ACCELERATORS)) {
			if ( !getAccel( &pcfinf->info, &maxink, buftop, pcfinf->tables,
				pcfinf->ntables, (CARD32)PCF_ACCELERATORS)) {
			    fal_utyerrno = FAL_ERR_FONT ;
			    return -1;
			}
		}

	}

	/*
	*	read property
	*/

	if( updflg == FAL_UPDATE_FONTINFO ) {
		pcfinf->org_bounds = pcfinf->info.maxbounds;
	}

	lb = pcfinf->org_bounds.leftSideBearing ;
	rb = pcfinf->org_bounds.rightSideBearing ;
	as = pcfinf->org_bounds.ascent  ;
	ds = pcfinf->org_bounds.descent ;

	databuff->size.w = rb - lb ;
	databuff->size.h = as + ds ;

	/*
	* read property "FONT"
	*/
	if ( buffp = getPcfFontProp( buftop, pcfinf->tables,
				     pcfinf->ntables, "FONT" )) {
		if( (databuff->xlfdname = (char *)strdup( buffp )) == (char *)NULL ){
			fal_utyerrno = FAL_ERR_MALLOC ;
			return -1;
		}
	}else{
		set_errfile_str( fal_err_file, finf->fname ) ;
		fal_utyerrno = FAL_ERR_PROP_FONT ;
		return( _FAL_TRY_NEXT ) ;
	}

	/*
	* read property "FAMILY_NAME"
	*/
	if ( buffp = getPcfFontProp( buftop, pcfinf->tables,
				     pcfinf->ntables, "FAMILY_NAME")) {
		if( (databuff->style.name = (char *)strdup( buffp )) == NULL ){
			fal_utyerrno = FAL_ERR_MALLOC ;
			return -1;
		}
	}else{
		set_errfile_str( fal_err_file, finf->fname ) ;
		fal_utyerrno = FAL_ERR_PROP_FNAME ;
		return( _FAL_TRY_NEXT ) ;
	}

	return 0;

}



static char *
getPcfFontProp(caddr_t buftop, PCFTablePtr tables, int ntables, char *propname)
{
	caddr_t	buffer;
	int name_ofs;
	int i ;
	int nprops;
	char *propstr ;
	CARD32	format, offset;

	if ( !seekToType( tables, ntables, (CARD32)PCF_PROPERTIES, &format, (CARD32 *)NULL, &offset))
		return NULL;

	buffer = buftop + offset;
	format = getLSB32( (unsigned char *)buffer );
	buffer += 4;
	if ( !PCF_FORMAT_MATCH( format, PCF_DEFAULT_FORMAT))
		return NULL;

	nprops = getINT32( (unsigned char *)buffer, (CARD32)format);
	buffer += 4;
	propstr = buffer + (4 * ((nprops * 9 + 3) / 4)) + 4;

	for ( i=0; i < nprops ; i++ ) {
		name_ofs = getINT32( (unsigned char *)buffer, (CARD32)format ) ;
		buffer += 4 ;	/* name */
		if ( strcmp( propstr + name_ofs, propname) == 0) {
			if ( *buffer) { 	/* isStringProp */
				return( propstr + getINT32( (unsigned char *)(buffer + 1), (CARD32)format) );
			}else{
				return((char *)(intptr_t)getINT32( (unsigned char *)(buffer + 1), (CARD32)format) );
			}
		}
		buffer += 5 ;	/* isStringProp + value */
	}

	return( NULL);
}


/*
*	get properties of SNF format file
*/
int
falInitReadSnfProp(
Oak_FontInf		*finf,		/* pointer to the infomation structure */
caddr_t			buftop,		/* font file */
FalFontData		*databuff)
{
	caddr_t	stprop ;
	int	lb, rb, as, ds ;
	char	*propptr ;
	char	*fnt = NULL, *fam ;
	int	nprops, bitmapSize, num_chars ;


	/* initialize pointer */
	nprops    = finf->pFinf->nprops ;
	num_chars = ( finf->pFinf->lastRow - finf->pFinf->firstRow + 1 ) *
		    ( finf->pFinf->lastCol - finf->pFinf->firstCol + 1 ) ;

	stprop = buftop ;
	stprop += sizeof(FontInfoRec) ;
	stprop += num_chars * sizeof(CharInfoRec) ;

	/*
	*	 read property "FONTBOUNDINGBOX"
	*/
	lb = finf->pFinf->maxbounds.leftSideBearing ;
	rb = finf->pFinf->maxbounds.rightSideBearing ;
	as = finf->pFinf->maxbounds.ascent  ;
	ds = finf->pFinf->maxbounds.descent ;

	/*
	* 	read property "FONT"
	*/
	if ( propptr = getSnfFontProp( stprop, nprops, "FONT" )) {
		if( (fnt = (char *)strdup( propptr )) == NULL ){
			fal_utyerrno = FAL_ERR_MALLOC ;
			free(fnt);
			return -1;
		}
	}else{
		set_errfile_str( fal_err_file, finf->fname ) ;
		fal_utyerrno = FAL_ERR_PROP_FONT ;
		return( _FAL_TRY_NEXT ) ;
	}

	/*
	* 	read property "FAMILY_NAME"
	*/
	if ( propptr = getSnfFontProp( stprop, nprops, "FAMILY_NAME")) {
		if( (fam = (char *)strdup( propptr )) == NULL ){
			fal_utyerrno = FAL_ERR_MALLOC ;
			free(fnt);
			return -1;
		}
	}else{
		set_errfile_str( fal_err_file, finf->fname ) ;
		fal_utyerrno = FAL_ERR_PROP_FNAME ;
		free(fnt);
		return( _FAL_TRY_NEXT ) ;
	}

	/*
	*	set data buffer
	*/

	databuff->size.w = rb - lb ;
	databuff->size.h = as + ds ;

	databuff->xlfdname 	= fnt ;
	databuff->style.name 	= fam ;

	return 0;
}



static char *
getSnfFontProp(caddr_t buftop, int nprops, char *propname)
{
	caddr_t	buffer;
	int name_ofs;
	int i ;
	char *propstr ;
	FontPropRec	*ProcRec ;

	ProcRec = (FontPropRec *)buftop ;
	buffer  = (char *)buftop ;
	propstr = buffer + nprops * sizeof(FontPropRec) ;

	for ( i=0; i < nprops ; i++, ProcRec++ ) {
		name_ofs = ProcRec->name ;
		if( strcmp( propstr + name_ofs, propname ) == 0 ){
			    return( (char *) (intptr_t) ProcRec->value ) ;
		}
	}

	return( NULL);
}

/***********************< end of readpcf.c >********************/
