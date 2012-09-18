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
/* $XConsortium: falfont.c /main/8 1996/07/04 02:35:37 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include	<signal.h>
#include	<locale.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>

#include	<sys/mman.h>
#include	<errno.h>

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>

#include	"FaLib.h"
#include	"falfont.h"
#include	"udcutil.h"


#define	MKFONTLIST	(1<<1)
#define	SRCHFNAME	(1<<2)
#define	FONTOFID	(1<<3)

typedef struct _FontIDInfo {
	FalFontID	fid ;
	int		cd_set ;
	int		dspcode ;
	int		islock ;
} FontIDInfo ;

typedef struct _MngFontID {
	int		num ;
	FontIDInfo	**finf ;
} MngFontID ;

/*** structures for UDC fontpath ***/
static FalFontPath	orgn = { 0, NULL };	/* for libfal */
static FalFontPath	copy = { 0, NULL };	/* report to the user */
static FalFontPath	wpath = { 0, NULL };	/* for libfal */

/***** Variables for notice kind of error *****/
int 	fal_utyerror;
int 	fal_utyderror;
int 	fal_utyexists;
unsigned long	fal_utyerrno ;
char	fal_err_file[ FAL_LINE_MAX+1 ] ;

char	fal_err_file_buf[ sizeof(fal_err_file) ] ;

#define	fal_seterrcode( uty, der, errno, func ) { \
    fal_utyerror  =  (uty); \
    fal_utyderror =  (der) ; \
    fal_utyerrno  =  (errno) ; \
    fal_utyerrno  |= ((func)<<8) ; \
}

/***** flags *****/
static	int	execDefined ;
static	int	execUndefined ;
static	int	IsDefaultPath = FALSE ;
static	MngFontID	mngfid = { 0, NULL } ;
static	int	tmp_codeset = 0 ;
static	char	*fal_locale = NULL ;
static	char	*charset_str_buf = NULL ;
static	int	*codeset_list_sav = NULL ;
static	int	codeset_list_num = 0 ;

/* declaration of inner functions */
char	*fal_get_base_name();

static	FILE	*open_fonts_list() ;
static	int	falGetGlyph() ;
static	int	falGetCharIndex() ;
static	int	falZoom() ;
static	int	exline() ;
static	char	falGetMask() ;
static	int	chk_key_str() ;
static	int	fal_make_fontlist() ;
static	int	fal_sort_fontlist() ;
static	int	set_default_path() ;
static	int	cpy_default_path() ;
static	int	comp_default_path() ;
static	int	fal_clear_font_path() ;
static	int	set_font_pathlist() ;

static	int	fal_split_data() ;
static	int	fal_clear_data() ;
static	int	CR_to_NULL() ;
static	int	fal_cmp_data() ;
static	int	new_target() ;

int	set_struct() ;
int	fal_eq_data() ;

void	set_errfile_str() ;

static	int	fal_init() ;
static	int	fal_add_fidinf() ;
static	int	fal_read_fidinf() ;
static	int	fal_del_fidinf() ;
static	int	fal_conv_code_to_glyph() ;
static	int	fal_conv_glyph_to_code() ;
static	int	falReadFontInfoLists() ;
static	int	fal_get_def_fontdata() ;
static	int	falgetfontlist() ;
static	int	fal_get_undef_fontdata() ;
static	void	clear_charset_info() ;
static	int	make_default_path() ;
static	int	file_lock() ;
static	int	file_unlock() ;
static	int	is_lock() ;

extern	int	falReadFontProp() ;
extern	int	falReadGpfProp() ;
extern	int	falInitReadPcfProp() ;
extern	int	fal_get_codeset() ;
extern	int	falInitReadPcf() ;
extern	void	falGetPcfGSize() ;
extern	int	falPcfGlyph() ;
extern	int	fal_code_to_glyph() ;
extern	int	fal_glyph_to_code() ;



static int 
fal_init()
{
    memset( fal_err_file, '\0', sizeof(fal_err_file) ) ;
    memset( fal_err_file_buf, '\0', sizeof(fal_err_file_buf) ) ;

    if ( !fal_locale ){
	char	*lcl ;
	if( (lcl = (char *)getenv( "LANG" )) == NULL ){
	    lcl = "C" ;
	}
	if( (fal_locale = (char *)strdup( lcl )) == NULL ){
	    fal_utyerror = _FAL_MALOC_ER;
	    fal_utyderror = 0;
	    fal_utyerrno = FAL_ERR_MALLOC ;
	    return FAL_ERROR;
	} 
    } 
    return 0 ;
}

void
set_errfile_str( obuf, ibuf )
char	*obuf ;
char	*ibuf ;
{
    char	*sp ;

    if( (strlen(ibuf)+1) > sizeof(fal_err_file) ){
	sp = ibuf ;
	sp += ((strlen(ibuf)+1) - sizeof(fal_err_file)) ;
	strcpy( obuf, sp ) ;
    }else{
	strcpy( obuf, ibuf ) ;
    }
}

/*
 * open a fontfile by "RDONLY"
 */
FalFontID 
FalOpenFont( file, protect_key_data, codeset )
char    *file;
int     protect_key_data;
int     codeset;
{
	FalFontID	__FalOpenFont();

	return	__FalOpenFont( file, protect_key_data, codeset, 0 );
}

/*
 * open a fontfile by "RDWR"
 *
 *
 * lock flag
 * 	1: ON ---  open mode "RDWR"
 * 	0: OFF --  for "FalOpenFont()"
 */
FalFontID
__FalOpenFont( file, protect_key_data, codeset, lockflag )
char	*file;
int     protect_key_data;
int	codeset;
int	lockflag;
{
	int     fd ;
	char    *buf;
	char	*openfontfile;
	struct  stat st;
	Oak_FontInf  *finf;

	if ( !(openfontfile = (char *)malloc( strlen( file ) + 1 )) ) {
	    fal_seterrcode( _FAL_MALOC_ER, 0,
			    FAL_ERR_MALLOC , FAL_FUNCNUM_OPNFNT ) ;
	    return	(FalFontID)FAL_ERROR;
	}
	strcpy( openfontfile, file );

	/* read a condition of a fontfile */
	if ( stat( openfontfile, &st ) < 0 ) {
		set_errfile_str( fal_err_file, openfontfile ) ;
		free( openfontfile );
		fal_utyerror = _FAL_STAT_ER;
		fal_utyderror = errno;
		fal_utyerrno = FAL_ERR_STAT ;
		fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		return	(FalFontID)FAL_ERROR;
	}

	CHK_PROTECT_KEY( protect_key_data, openfontfile, fal_utyerror, fal_utyderror ) ;

	if ( st.st_size < sizeof( FontInfoRec ) ) {
		set_errfile_str( fal_err_file, openfontfile ) ;
		free( openfontfile );
		fal_utyerror = _FAL_FONT_ER;
		fal_utyderror = 0;
		fal_utyerrno = FAL_ERR_FONT ;
		fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		return	(FalFontID)FAL_ERROR;
	}

	/* open a fontfile */
	if ( lockflag == 1 ) {
		int	ret ; 
		if ( (fd = open( openfontfile, O_RDWR )) < 0 ) { 
		    set_errfile_str( fal_err_file, openfontfile ) ;
		    free( openfontfile );
		    fal_utyerror = _FAL_OPEN_ER;
		    fal_utyderror = errno;
		    fal_utyerrno = FAL_ERR_FNT_OPN ;
		    fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		    return (FalFontID)FAL_ERROR;
		}
		ret = is_lock( fd ) ;
		if( ret != 0 ){
		    set_errfile_str( fal_err_file, openfontfile ) ;
		    free( openfontfile );
		    fal_utyerror = _FAL_OPEN_ER;
		    fal_utyderror = errno;
		    fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		    return (FalFontID)FAL_ERROR;
		}
	} else {
		if ( (fd = open( openfontfile, O_RDONLY)) < 0 ) { 
		    set_errfile_str( fal_err_file, openfontfile ) ;
		    free( openfontfile );
		    fal_utyerror = _FAL_OPEN_ER;
		    fal_utyderror = errno;
		    fal_utyerrno = FAL_ERR_FNT_OPN ;
		    fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		    return (FalFontID)FAL_ERROR;
		}
	}

	/* read a fontfile */
	if ( !(finf = ( Oak_FontInf * )malloc(sizeof( Oak_FontInf ) )) ) {
		free( openfontfile );
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyderror = 0;
		fal_utyerrno = FAL_ERR_MALLOC ;
		fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		close( fd );
		return	(FalFontID)FAL_ERROR;
	}


#if	defined( SVR4 )
	/* use "mmap()" */
	buf = (char *)mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if ( buf != (char *)-1 ) {
	    /* if "mmap" is normal end */
	    if ( lockflag == 0 ) {
		    close( fd );
	    }
	    finf->ismmap = TRUE;
	    finf->fsize = st.st_size;
	} else {
#endif
	    /* if "mmap" is abnormal end , try "read()" */
	    finf->ismmap = FALSE;
	    if ( !(buf = (char *)malloc(st.st_size)) ) {
		free( openfontfile );
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyderror = 0;
		fal_utyerrno = FAL_ERR_MALLOC ;
		fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		free( finf );
		close( fd );
		return	(FalFontID)FAL_ERROR;
	    }
	    if ( read(fd, buf, st.st_size) != st.st_size ) {
		set_errfile_str( fal_err_file, openfontfile ) ;
		free( openfontfile );
		fal_utyerror = _FAL_READ_ER;
		fal_utyderror = errno;
		fal_utyerrno = FAL_ERR_FNT_RD ;
		fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		free( finf );
		free( buf );
		close( fd );
		return	(FalFontID)FAL_ERROR;
	    }
#if	defined( SVR4 )
	}
#endif

	if ( !(finf->fname = (char *)strdup( openfontfile )) ) {
	    fal_seterrcode( _FAL_MALOC_ER, 0,
			    FAL_ERR_MALLOC , FAL_FUNCNUM_OPNFNT ) ;
	    return	(FalFontID)FAL_ERROR;
	}

	/* case of a pcf font */
	if ( strcmp( FILE_SUFFIX( openfontfile ), PCFSUFFIX ) == 0 ) {
	    if ( falInitReadPcf( &finf->pcfinf, buf ) ) {
		fal_utyerror = _FAL_PKEY_ER;
		fal_utyderror = 0;
		fal_utyerrno = FAL_ERR_FONT ;
		fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		goto FalError01 ;
	    }
	    finf->isFef = FALSE;
	    finf->isPcf = TRUE;
	    finf->pFinf = &finf->pcfinf.info;
	} else {
	    FAL_GET_SNF_HEADER( buf, protect_key_data, fal_utyerror, fal_utyderror, fd, finf, openfontfile ) ;
	}
	finf->buf = buf;

	if ( finf->isFef ){
		finf->pCinf 
		    = (CharInfoPtr)( (char *)finf->pFinf 
		    + sizeof( FontInfoRec ) );
		finf->pGlyphs 
		    = (unsigned char *)((char *)finf->pCinf
		    + (sizeof(CharInfoRec) * GETNUMCHARS(finf->pFinf)));
		finf->width 
		    = finf->pFinf->maxbounds.metrics.rightSideBearing 
		    - finf->pFinf->maxbounds.metrics.leftSideBearing;
		finf->height 
		    = finf->pFinf->maxbounds.metrics.ascent 
		    + finf->pFinf->maxbounds.metrics.descent;
	} else if ( finf->isPcf ) {
		finf->pCinf = NULL;
		finf->pGlyphs = NULL;
		falGetPcfGSize( &finf->pcfinf, &finf->width, &finf->height);
	} else {
	    fal_utyerror = _FAL_FONT_ER;
	    fal_utyderror = 0;
	    fal_utyerrno = FAL_ERR_FONT ;
	    fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
	    goto FalError01 ;
	}

	finf->start = (finf->pFinf->firstRow << 8) | finf->pFinf->firstCol;
	finf->end = (finf->pFinf->lastRow << 8) | finf->pFinf->lastCol;

	finf->fd = fd;
	finf->sptnBufL = 0;
	finf->sptnBuf = (char *)0;
	finf->dptnBufL = 0;
	finf->dptnBuf = (char *)0;
	finf->dbufL = 0;
	finf->dbuf = (char *)0;

	finf->sptnBufL = ((finf->width + 7) / 8) * (finf->height);
	finf->sptnBuf = (char *)malloc(finf->sptnBufL); 
	if ( finf->sptnBuf == NULL ) {
	    fal_utyerror = _FAL_MALOC_ER;
	    fal_utyderror = 0;
	    fal_utyerrno = FAL_ERR_MALLOC ;
	    fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
	    goto FalError01 ;
	}

	/* Save font information */
	if( fal_add_fidinf( (FalFontID)finf, codeset,
		((tmp_codeset)?tmp_codeset:codeset),
		lockflag ) == FAL_ERROR )
	{
		fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		goto FalError01 ;
	}

	/* Lock the font file */
	if ( lockflag == 1 ) {
	    if ( file_lock( finf->fd ) == FAL_ERROR ) {
		set_errfile_str( fal_err_file, openfontfile ) ;
		fal_utyerror = _FAL_OPEN_ER;
		fal_utyderror = errno;
		fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		goto FalError01 ;
	    }
	}
	free( openfontfile );

	return( ( FalFontID ) finf );

FalError01:

#if	defined( SVR4 )
	if ( finf->ismmap == TRUE ) {
	    munmap( buf, finf->fsize );
	} else {
	    free( buf );
	    close( fd );
	}
#else
	free( buf );
	close( fd );
#endif
	set_errfile_str( fal_err_file, openfontfile ) ;
	free( openfontfile );
	free( finf->fname );
	finf->fname = NULL;
	free(finf);

	return	(FalFontID)FAL_ERROR;
}



#if NeedFunctionPrototypes
FalCloseFont(
	FalFontID	fid )
#else
FalCloseFont( fid )
FalFontID	fid;
#endif
{
	int	__FalCloseFont();
	FontIDInfo	fontid_inf ;

	if( fal_read_fidinf( fid, &fontid_inf ) == FAL_ERROR ) {
		fal_utyerrno |= (FAL_FUNCNUM_CLSFNT<<8) ;
		return	FAL_ERROR;
	}

	return	__FalCloseFont( fid, fontid_inf.islock );
}


/*
 * close a fontfile
 *
 *
 * lock flag
 * 	1: ON ---  open a font by "RDWR" mode
 * 	0: OFF --  for "FalOpenFont()"
 */
__FalCloseFont( fid, lockflag )
FalFontID	fid;
int 	lockflag;
{
	Oak_FontInf  *finf;

	finf = ( Oak_FontInf * )fid;

	if ( finf == NULL ) {
		fal_utyerror = _FAL_PARM_ER;
		fal_utyderror = 0;
		fal_utyerrno = FAL_ERR_PARM ;
		fal_utyerrno |= (FAL_FUNCNUM_CLSFNT<<8) ;
		return	FAL_ERROR;
	}

	set_errfile_str( fal_err_file_buf, finf->fname ) ;
	if ( finf->fname ) {
		free( finf->fname );
	}
	if ( finf->buf != NULL ) {
#if	defined( SVR4 )
		if ( finf->ismmap == TRUE ) {
			munmap( finf->buf, finf->fsize );
			if ( lockflag == 1 ) {
			    close( finf->fd );
			}
		} else {
			free( finf->buf );
			close( finf->fd );
		}
#else
		free( finf->buf );
		close( finf->fd );
#endif

		if ( finf->sptnBuf ) {
			free( finf->sptnBuf );
		}
		if ( finf->dptnBuf ){
			free( finf->dptnBuf );
		}
		if ( finf->dbuf ) {
			free( finf->dbuf );
		}
		free ( finf );
	}
	if ( lockflag == 1 ) {
	    if ( file_unlock( finf->fd ) == FAL_ERROR ) {
		set_errfile_str( fal_err_file, fal_err_file_buf ) ;
		fal_utyerrno |= (FAL_FUNCNUM_CLSFNT<<8) ;
		return	FAL_ERROR;
	    }
	}

	if ( fal_del_fidinf( fid ) == FAL_ERROR ) {
		fal_utyerrno |= (FAL_FUNCNUM_CLSFNT<<8) ;
		return	FAL_ERROR;
	}

	return	_FAL_OK;
}

#if NeedFunctionPrototypes
FalQueryFont(
	FalFontID	fid,
	FalFontinfo	*fontinfo )
#else
FalQueryFont( fid, fontinfo )
FalFontID	fid;
FalFontinfo	*fontinfo;
#endif
{
	Oak_FontInf	*finf;
	unsigned int	inner_start, inner_end ;
	FontIDInfo	fontid_inf ;

	if( fal_read_fidinf( fid, &fontid_inf ) == FAL_ERROR ){
		fal_utyerrno |= (FAL_FUNCNUM_QRY<<8) ;
		return FAL_ERROR;
	}
	finf = ( Oak_FontInf * )fid;
	if ( finf == NULL || fontinfo == NULL ) {
		fal_utyerror = _FAL_PARM_ER;
		if (finf == NULL) {
			fal_utyderror = _FAL_Q_P_FINF_DER;
		} else {
			fal_utyderror = _FAL_Q_P_FONT_DER;
		}
		fal_utyerrno = FAL_ERR_PARM ;
		fal_utyerrno |= (FAL_FUNCNUM_QRY<<8) ;
		return	FAL_ERROR;
	}

	fontinfo->width  = (int)finf->width;
	fontinfo->height = (int)finf->height;

	if( (fal_conv_glyph_to_code( finf, fontid_inf.dspcode,
	    fontid_inf.cd_set, finf->start, &inner_start ) == FAL_ERROR ) ||
	    (fal_conv_glyph_to_code( finf, fontid_inf.dspcode,
	    fontid_inf.cd_set, finf->end, &inner_end  ) == FAL_ERROR ))
	{
		fal_utyerrno |= (FAL_FUNCNUM_QRY<<8) ;
		return	FAL_ERROR;
	}

	fontinfo->top    = inner_start ;
	fontinfo->bottom = inner_end ;

	return	_FAL_OK;
}

#if NeedFunctionPrototypes
char	*
FalReadFont(
	FalFontID	fid,
	int 		code,
	int 		width,
	int 		height )
#else
char	*
FalReadFont( fid, code, width, height )
FalFontID	fid;
int 		code ;
int 		width ;
int 		height ;
#endif
{
	Oak_FontInf	*finf;
	int 	zoom_on ;
	char	*toglyph;
	int 	ptnBsize;
	unsigned int 	inner_code;	/* an inside code of a file */

	finf = ( Oak_FontInf * )fid;

	if ( finf == NULL ) {
		fal_utyerror = _FAL_PARM_ER;
		fal_utyderror = _FAL_R_P_FINF_DER;
		fal_utyerrno = FAL_ERR_PARM ;
		fal_utyerrno |= (FAL_FUNCNUM_RDFNT<<8) ;
		return	(char *)FAL_ERROR;
	}
	if (width < 0) {
		fal_utyerror = _FAL_PARM_ER;
		fal_utyderror = _FAL_R_P_W_DER;
		fal_utyerrno = FAL_ERR_PARM ;
		fal_utyerrno |= (FAL_FUNCNUM_RDFNT<<8) ;
		return	(char *)FAL_ERROR;
	}
	if (height < 0) {
		fal_utyerror = _FAL_PARM_ER;
		fal_utyderror = _FAL_R_P_H_DER;
		fal_utyerrno = FAL_ERR_PARM ;
		fal_utyerrno |= (FAL_FUNCNUM_RDFNT<<8) ;
		return	(char *)FAL_ERROR;
	}

	if (width == 0) {
		width = finf->width;
	}
	if (height == 0) {
		height = finf->height;
	}
	if ( (width != finf->width) || (height != finf->height) ) {
		zoom_on = TRUE;
	} else {
		zoom_on = FALSE;
	}

	memset(finf->sptnBuf, 0, finf->sptnBufL);

	if( fal_conv_code_to_glyph( fid, code, &inner_code ) == FAL_ERROR ) {
		fal_utyexists = 1;
		fal_utyerrno |= (FAL_FUNCNUM_RDFNT<<8) ;
		return	(char *)0;
	}

	if ( finf->isPcf) {
		/* case of a pcf font */
		falPcfGlyph( finf->sptnBuf, finf, inner_code );
	} else {
		/* case of a snf font */
		falGetGlyph( finf->sptnBuf, finf, inner_code );
	}
	if ( zoom_on == TRUE ) {
		ptnBsize = ((width + 7)/8)*height;
		if ( ptnBsize > finf->dptnBufL ) {
			if ( finf->dptnBuf == NULL ) {
				toglyph = (char *)malloc( ptnBsize );
			} else {
				toglyph = (char *)realloc( finf->dptnBuf, ptnBsize );
			}
			if ( toglyph == NULL ) {
				fal_utyerror = _FAL_MALOC_ER;
				fal_utyderror = 0;
				fal_utyerrno = FAL_ERR_MALLOC ;
				fal_utyerrno |= (FAL_FUNCNUM_RDFNT<<8) ;
				return	(char *)FAL_ERROR;
			}
			finf->dptnBufL = ptnBsize;
			finf->dptnBuf = toglyph;
		}
		ptnBsize = (width + 7) / 8;
		if ( ptnBsize > finf->dbufL ) {
			if ( finf->dbuf == NULL ) {
				toglyph = (char *)malloc( ptnBsize );
			} else {
				toglyph = (char *)realloc( finf->dbuf, ptnBsize );
			}
			if ( toglyph == NULL ) {
				fal_utyerror = _FAL_MALOC_ER;
				fal_utyderror = 0;
				fal_utyerrno = FAL_ERR_MALLOC ;
				fal_utyerrno |= (FAL_FUNCNUM_RDFNT<<8) ;
				return	(char *)FAL_ERROR;
			}
			finf->dbufL = ptnBsize;
			finf->dbuf = toglyph;
		}
		falZoom(
		    finf->dptnBuf, finf->sptnBuf,
		    finf->width, finf->height, width, height, finf->dbuf
		    );
		return( finf->dptnBuf );
	} else {
		return	finf->sptnBuf;
	}
}

static
falGetGlyph( glyph, finf, code )
char	*glyph;
Oak_FontInf	*finf;
int 	code;
{
	int 	in_dwidth, out_dwidth, ix, i, j;
	char	*glyph_p, *inp, p_mask, falGetMask();
	CharInfoPtr	CharInfP;
	int 	codeRow, codeCol;
	unsigned int 	inner_code;             /* an inside code of a file */

	inner_code = code;
	codeCol = inner_code & 0xff;	/* a lower byte of an inside code */
	codeRow = (inner_code >> 8) & 0xff; /* a upper byte of a code */

	/* code check */
	if (
	    ((code < finf->start) || (code > finf->end)) || 
	    ((codeCol < finf->pFinf->firstCol) ||
	    (codeCol > finf->pFinf->lastCol)) ||
	    ((codeRow < finf->pFinf->firstRow) ||
	    (codeRow > finf->pFinf->lastRow))
	    ) {
		fal_utyexists = 1;
		return	0;
	}

	/* Get a character index */
	ix = falGetCharIndex( finf, inner_code );
	CharInfP = finf->pCinf;

	if ( !CharInfP[ix].exists ) {
		fal_utyexists = 1;
		return	0;
	} else {
		fal_utyexists = 0;
	}

	in_dwidth 
	    = (finf->width + SNF_BOUND - 1)
	    / SNF_BOUND * (SNF_BOUND / 8);
	out_dwidth = (finf->width + 7) / 8;

	p_mask = falGetMask( finf->width );

	glyph_p = (char *)finf->pGlyphs + CharInfP[ix].byteOffset;
	for ( i = 0; i < finf->height; i++ ) {
		inp = glyph_p + ( in_dwidth * i );
		for ( j = 0; j < out_dwidth-1; j++ ) {
			*glyph++ = *inp++;
		}
		*glyph++ = *inp++ & p_mask;
	}
	return	0;
}

static
falGetCharIndex( finf, code )
Oak_FontInf	*finf;
int	code;   /* an inside code of a file */
{
	int 	nColperRow, nRow, nCol;

	nColperRow = finf->pFinf->lastCol  - finf->pFinf->firstCol + 1;
	nRow = ((code >> 8) & 0xff) - finf->pFinf->firstRow;
	nCol = (code & 0xff) - finf->pFinf->firstCol;

	return	( nRow * nColperRow + nCol );
}

static
falZoom( dmem, smem, sw, sh, dw, dh, dbuf )
char    *dmem;
char    *smem;
int     sw;
int     sh;
int     dw;
int     dh;
char    *dbuf;
{
	int 	swidth;
	int 	dwidth;
	int  	i, lcnt;
	char	*sp, *dp;

	swidth = (sw + 7) / 8;
	dwidth = (dw + 7) / 8;

	lcnt = 0;
	sp = smem;
	dp = dmem;
	for ( i=0; i < sh; i++ ) {
		lcnt += dh;
		if ( lcnt >= sh ) {
			exline( sp, dbuf, sw, dw );
			sp += swidth;
			lcnt -= sh;
			memcpy( dp, dbuf, dwidth );
			dp += dwidth;
			for ( ; lcnt >= sh; lcnt -= sh ) {
				memcpy( dp, dbuf, dwidth );
				dp += dwidth;
			}
		} else {
			sp += swidth;
		}
	}
	return	0;
}

static
exline( sp, dbuf, sw, dw )
char	*sp;
char	*dbuf;
int 	sw;
int 	dw;
{
	int 	i, bit, sval, dval, dcnt, bcnt;

	bcnt = dval = 0;
	dcnt = 8;

	for ( i = 0; i < sw; i++ ) {
		if ( i % 8 == 0 ) {
			sval = *sp++;
		}
		bit = ( (sval & 0x80) ? 1 : 0 );
		sval <<=  1;
		for ( bcnt += dw; bcnt >= sw; bcnt -= sw ) {
			dval = ( dval << 1 ) | bit;
			if ( --dcnt == 0 ) {
				*dbuf++ = (char)dval;
				dval = 0;
				dcnt = 8;
			}
		}
	}
	if ( dcnt != 8 ) {
		dval <<= dcnt;
		*dbuf = (char)dval;
	}
	return	0;
}

static unsigned char	_Fal_Mask_Tab[8] = {
	0xff, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe 
};

static char
falGetMask( width )
int     width;
{
	int 	ix = width % 8;
	return	_Fal_Mask_Tab[ix];
}


/* a difinition of a style */
static FalFontDB fal_db_style[] = {
	{ FAL_FONT_MINCHO,      FAL_FONT_STR_MINCHO },
	{ FAL_FONT_GOTHIC,      FAL_FONT_STR_GOTHIC },
	{ FAL_FONT_MARU_GOTHIC, FAL_FONT_STR_MARU_GOTHIC },
	{ FAL_FONT_TEXTBOOK,    FAL_FONT_STR_TEXTBOOK },
	{ FAL_FONT_BRASH_GYOU,  FAL_FONT_STR_BRASH_GYOU },
	{ FAL_FONT_BRASH_KAI,   FAL_FONT_STR_BRASH_KAI },
	{ FAL_FONT_HANDWRITING, FAL_FONT_STR_HANDWRITING },
	{ FAL_FONT_UNKNOWN,     ""}
};

/* a difinition of a shape */
static FalFontDB fal_db_shape[] = {
	{ FAL_FONT_ZENKAKU,     FAL_FONT_STR_ZENKAKU },
	{ FAL_FONT_HANKAKU,     FAL_FONT_STR_HANKAKU },
	{ FAL_FONT_QUARTER,     FAL_FONT_STR_QUARTER },
	{ FAL_FONT_UNKNOWN,     ""}
};

FAL_DB_OPTION ;

/*********************************************************/
/* make a font information list and carry to user      */
/*********************************************************/

#if NeedFunctionPrototypes
int
FalGetFontList(
	FalFontData *key_data,
	int mask,
	FalFontDataList **list_ret )
#else
int
FalGetFontList( key_data, mask, list_ret )
FalFontData	*key_data;	/* a structure of a searching information */
int 		mask;		/* a mask */
FalFontDataList	**list_ret;	/* maked a address of a structure */
#endif
{

	FalFontDataList	*fls;	/* a pointer of a structure of "FalFontDataList()" */

	/* clear an error's data */
	fal_utyerror  = 0;
	fal_utyderror = 0;
	fal_utyerrno = 0 ;

	/* change mode */
	SET_EXECDEFAULT( mask, execDefined, execUndefined ) ;

	/* interface check */
	if ( list_ret == NULL ) {
		fal_utyerror  = _FAL_PARM_ER;
		fal_utyderror = _FAL_L_P_LST_DER;
		fal_utyerrno = FAL_ERR_PARM ;
		fal_utyerrno |= (FAL_FUNCNUM_GFLST<<8) ;
		return FAL_ERROR ;
	}

	/* character check */
	if ( chk_key_str( key_data, mask ) == FAL_ERROR ) {
		fal_utyerrno |= (FAL_FUNCNUM_GFLST<<8) ;
		return	FAL_ERROR;
	}

	/* error check */
	if ( orgn.path == NULL ) {
		if ( set_default_path() == FAL_ERROR ) {
			fal_utyerrno |= (FAL_FUNCNUM_GFLST<<8) ;
			return FAL_ERROR ;
		}
	}

	/* make an structure of "FalFontDataList()" */
	if ((fls = (FalFontDataList *)malloc( sizeof( FalFontDataList ))) == NULL){
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyerrno = FAL_ERR_MALLOC ;
		fal_utyerrno |= (FAL_FUNCNUM_GFLST<<8) ;
		return FAL_ERROR ;
	}

	/* inital set for search */
	fls->num  = 0;
	fls->list = NULL;

	/* make font infomation list */
	if ( fal_make_fontlist( fls, key_data, mask ) == FAL_ERROR) {
		switch( fal_utyerror ) {
			case _FAL_FLST_ER :
			case _FAL_DATA_OPEN_ER :
				fal_utyderror = 1;
		}
		FalFreeFontList( fls );
		fal_utyerrno &= 0xff;
		fal_utyerrno |= (FAL_FUNCNUM_GFLST<<8) ;
		return	FAL_ERROR;
	}
	/* sort element of font infomation list */
	if( fal_sort_fontlist( fls ) == FAL_ERROR ) {
		FalFreeFontList( fls );
		fal_utyerrno &= 0xff;
		fal_utyerrno |= (FAL_FUNCNUM_GFLST<<8) ;
		return FAL_ERROR ;
	}

	*list_ret = fls;
	return	0;
}



/***********************************************************************/
/* search a character "key" of a structure                             */
/***********************************************************************/

static int
chk_key_str( key, mask )
FalFontData     *key;		/* a structure of saerching information */
int		mask;		/* a mask                               */
{
	int     flg = 0;

	if( key == NULL ) {
		return 0 ;
	}

	/* check a character of setting a mask */
	/* xlfd name */
	if ((mask & FAL_FONT_MASK_XLFDNAME) && (key->xlfdname == NULL)) {
		flg++;
	}
	/* style */
	if ((mask & FAL_FONT_MASK_STYLE_NAME) && (key->style.name == NULL)) {
		flg++;
	}
	/* shape */
	if ((mask & FAL_FONT_MASK_SHAPE_NAME) && (key->shape.name == NULL)) {
		flg++;
	}
	CHK_KEY_STR_OPTION( mask, key, flg ) ;
	/* error check */
	if ( flg ) {
		fal_utyerror  = _FAL_PARM_ER;
		fal_utyderror = _FAL_L_P_KEY_DER;
		fal_utyerrno = FAL_ERR_PARM ;
		return FAL_ERROR ;
	}
	return 0 ;
}


/***************************************************************/
/* read a font information and add a list or make a list       */
/***************************************************************/


static int
fal_make_fontlist( fls, key, mask )
FalFontDataList *fls;	/* a pointer of a structure of a font information list */
FalFontData	*key;	/* a structure of searching information */
int		mask;	/* a mask for a saerch */
{
	return	falReadFontInfoLists(MKFONTLIST,
				key, mask, fls,
				NULL, NULL,
				NULL, NULL) ;
}


/**************************************************/
/* free a structure of "FalFontDataList()"        */
/**************************************************/

#if NeedFunctionPrototypes
int
FalFreeFontList(
	FalFontDataList *list )
#else
int
FalFreeFontList( list )
FalFontDataList *list;
#endif
{
	int 	i;

	/* parametter check */
	if ( list == NULL ) {
		fal_utyerror = _FAL_PARM_ER;
		fal_utyerrno = FAL_ERR_PARM ;
		fal_utyerrno |= (FAL_FUNCNUM_FRFLST<<8) ;
		return	FAL_ERROR;
	}
	/* free a structure's array of "FalFontData()" */
	if ( list->list != NULL ) {
		for ( i = 0;  i < list->num; i++ ) {
			/* free a structure's character of "FalFontData()" */
			if ( list->list[i].xlfdname ) {
				free( list->list[i].xlfdname );
			}
			if ( list->list[i].style.name ) {
				free( list->list[i].style.name );
			}
			if ( list->list[i].shape.name ) {
				free( list->list[i].shape.name );
			}
			FAL_FREE_FONTLIST_OPTION( list ) ;
		}
		/* free a structure's array of "FalFontData()" */
		free( list->list );
	}
	list->num  = 0 ;
	list->list = NULL ;

	/* free a structure of " FalFontDataList()" */
	free( list );
	return	0;
}


/***********************************************************************/
/* split string for "FalFontData()"                                    */
/***********************************************************************/

static int
fal_split_data( buf, elm_num, elm)
char	*buf;
int 	elm_num;
char	*elm[];	
{
	int         cnt;
	int         strtop_flg;

	/* check a comment */
	if ((*buf == '#') || (*buf == '\0')) {
		return	_FAL_TRY_NEXT;
	}
	/* check a pathname */
	if ( *buf == '/' ) {
		return	_FAL_TRY_NEXT;
	}
	/* divide a data */
	for ( cnt = 0, strtop_flg = 1 ; *buf != '\0';  buf++ ){
		if ( strtop_flg ) {
			elm[cnt] = buf;
			cnt++;
			strtop_flg = 0;
		}
		if ( *buf == ':' ) {
			*buf = '\0';
			strtop_flg++;
		}
	}
	if ( cnt != elm_num ) {
		set_errfile_str( fal_err_file, fal_err_file_buf ) ;
		fal_utyerror = _FAL_FLST_ER;
		fal_utyerrno = FAL_ERR_FDATA_DSC ;
		return	FAL_ERROR;
	}
	return	0;
}

/***********************************************************************/
/* clear a structure of "FalFontData()"                                */
/***********************************************************************/

static int
fal_clear_data(tmp)
FalFontData     *tmp;
{
	/* search a character */
	if ( tmp->xlfdname != NULL ) {
		free(tmp->xlfdname);
	}
	if (tmp->style.name != NULL) {
		free(tmp->style.name);
	}
	if (tmp->shape.name != NULL) {
		free(tmp->shape.name);
	}
	FAL_CLEAR_DATA_OPTION( tmp ) ;
	memset(tmp, 0, sizeof(*tmp));
	return 0  ;
}

static int
fal_check_already_exist( data, lst )
FalFontData     *data;
FalFontDataList *lst;
{
	int             i;
	int             mask ;

	mask =  FAL_FONT_MASK_XLFDNAME | FAL_FONT_MASK_STYLE_NAME
	    | FAL_FONT_MASK_SIZE_H | FAL_FONT_MASK_CODE_SET ;

	for ( i=0 ; i < lst->num ; i++ ){
		switch ( fal_cmp_data( &(lst->list[i]), data, mask ) ) {
		case 0:
			return(i);
		default:
			continue;
		}
	}
	return(-1);
}



static int
fal_sort_fontlist( lst )
FalFontDataList *lst;
{
	int             i,j;
	FalFontDataList srt;
	FalFontData             target;
	int                     target_entry;

	if( lst->num == 0 ){
	    return 0 ;
	}

	srt.num  = lst->num;

	if (( srt.list = malloc(sizeof(FalFontData) * srt.num)) == NULL) {
		fal_utyerrno = FAL_ERR_MALLOC ;
		return	FAL_ERROR;
	}
	memmove (srt.list, lst->list, sizeof(FalFontData) * srt.num);
	/* start to sort */
	for ( i=0 ; i < srt.num -1 ; i++ ) {

		memmove( &target, &(srt.list[i]), sizeof(FalFontData));
		target_entry = i;

		for ( j=i+1 ; j < srt.num ; j++ ) {
			if ( new_target( &target, &(srt.list[j]) ) ) {
				memmove( &target, &(srt.list[j]), sizeof(FalFontData));
				target_entry = j;
			}
		}
		if (target_entry != i) {
			memmove( &target, &(srt.list[target_entry]), 
			    sizeof(FalFontData)
			    );
			memmove( &(srt.list[i+1]), &(srt.list[i]), 
			    sizeof(FalFontData)*(target_entry -i)
			    );
			memmove( &(srt.list[i]), &target, 
			    sizeof(FalFontData)
			    );
		}
	}
	free(lst->list);
	lst->list = srt.list;
	return 0 ;
}



/***************************************/
/* change a character for a number     */
/***************************************/

static int
fal_atoi(str, val)
char    *str;
int     *val;
{
	char    *ptr;
	char    *str_end;
	int             tmp;

	str_end = strchr( str, '\0' );

	tmp = (int)strtol(str, &ptr, 10);

	/* error check */
	if ((ptr == str) || (ptr != str_end)) {
		set_errfile_str( fal_err_file, fal_err_file_buf ) ;
		fal_utyerror = _FAL_FLST_ER;
		fal_utyerrno = FAL_ERR_FDATA_DSC ;
		return(FAL_ERROR);
	}
	*val = tmp;
	return 0 ;
}

/*******************************/
/* get a codeset               */
/*******************************/

static int
fal_set_cs(str, cs)
char    *str;
int     *cs;
{
	if (!strcmp(str, "CS0")) {
		*cs = FAL_FONT_CS0;
	} else if (!strcmp(str, "CS1")) {
		*cs = FAL_FONT_CS1;
	} else if (!strcmp(str, "CS2")) {
		*cs = FAL_FONT_CS2;
	} else if (!strcmp(str, "CS3")) {
		*cs = FAL_FONT_CS3;
	} else {
		set_errfile_str( fal_err_file, fal_err_file_buf ) ;
		fal_utyerror = _FAL_FLST_ER;
		fal_utyerrno = FAL_ERR_FDATA_DSC ;
		return(FAL_ERROR);
	}
	return(0);
}


/******************************************/
/* get a equipment of an output permission*/
/******************************************/


static int
fal_set_prm(str, prm)
char    *str;
int     *prm;
{
	int	tmp = 0;
	for (     ; *str != '\0'  ; str++) {
	    if( *str == 'P' ) {
		tmp |= FAL_FONT_PRINTER;
	    } else if( *str == 'D' ) {
		tmp |= FAL_FONT_DISPLAY;
	    } else {
		set_errfile_str( fal_err_file, fal_err_file_buf ) ;
		fal_utyerror = _FAL_FLST_ER;
		fal_utyerrno = FAL_ERR_FDATA_DSC ;
		return FAL_ERROR ;
	    }
	}
	*prm = tmp;
	return 0 ;
}

static int fal_read_db( str, db )
char    *str;
FalFontDB       *db;
{
	int             i;

	for(i=0 ; db[i].def != FAL_FONT_UNKNOWN ; i++) {
		if (!strcmp(str, db[i].name)) {
			return(db[i].def);
		}
	}
	return FAL_FONT_UNKNOWN ;
}


/***************************************/
/* make a structure of "FalFontPath()" */
/***************************************/

int
FalGetFontPath( dlist_ret )
FalFontPath	**dlist_ret;
{

	/* clear an error data */
	fal_utyerror  = 0;
	fal_utyderror = 0;
	fal_utyerrno = 0 ;

	/* get current locale */
	if( fal_init() )	return FAL_ERROR ;

	/* parametter check */
	if ( dlist_ret == NULL ) {
		fal_utyerror = _FAL_PARM_ER;
		fal_utyerrno = FAL_ERR_PARM ;
		return	FAL_ERROR;
	}
	if ( orgn.path == NULL ) {
		if ( set_default_path() == FAL_ERROR ) {
			return	FAL_ERROR;
		}
		if ( cpy_default_path( &copy ) == FAL_ERROR ){
			return	FAL_ERROR;
		}
	} else if ( copy.path == NULL ) {
		if ( cpy_default_path(&copy) == FAL_ERROR ){
			return	FAL_ERROR;
		}
	} else {

		if ( comp_default_path( &copy ) == FAL_ERROR ) {
			fal_clear_font_path( &copy );
			if ( cpy_default_path(&copy) == FAL_ERROR ){
				return	FAL_ERROR;
			}
		}
	}
	*dlist_ret = &copy;
	return	0;
}

static int
set_default_path()
{
	return	set_font_pathlist(&orgn, 0) ;
}

static int
cpy_default_path( p )
FalFontPath     *p;
{
	int         i;
	FalFontPath tmp;

	if ((orgn.num == 0) || (orgn.path == NULL)) {
		p->num  = 0;
		p->path = NULL;
		return	0;
	}

	if (
		(tmp.path = (char **)malloc(sizeof(char *) * (orgn.num) )) 
		== NULL
	){ 
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyerrno = FAL_ERR_MALLOC ;
		return	FAL_ERROR;
	}
	tmp.num = 0;
	for ( i = 0 ; i< orgn.num ; i++ ) {
	char    *tmp_str;
	/* malloc & copy in strdup */
		if ((tmp_str = (char *)strdup((orgn.path)[i])) == NULL) {
			fal_clear_font_path( &tmp );
			fal_utyerror = _FAL_MALOC_ER;
			fal_utyerrno = FAL_ERR_MALLOC ;
			return FAL_ERROR ;
		}
		tmp.path[i] = tmp_str;
		tmp.num++;
	}
	p->num  = tmp.num;
	p->path = tmp.path;
	return	0;
}

static int
comp_default_path(p)
FalFontPath     *p;
{
	int         i;

	if ( p->num != orgn.num ) {
	    set_errfile_str( fal_err_file, fal_err_file_buf ) ;
	    fal_utyerrno = FAL_ERR_FPATH ;
	    return	FAL_ERROR;
	}
	for ( i = 0; i < p->num; i++ ) {
	    if( strcmp( p->path[i], orgn.path[i] ) ) {
		set_errfile_str( fal_err_file, fal_err_file_buf ) ;
		fal_utyerrno = FAL_ERR_FPATH ;
		return	FAL_ERROR;
	     }
	}
	return	0;
}


static int
fal_clear_font_path( ls )
FalFontPath     *ls;
{
	int             i;

	if (ls->path != NULL) {
		/* free a fontpath character */
		for (i=0 ; i < ls->num ; i++) {
			if ((ls->path[i] != NULL) && (ls->path[i] == orgn.path[i])) {
				free(ls->path[i]);
			}
		}
		/* free a character pointer array */
		free( ls->path );
		ls->path = NULL;
	}
	ls->num = 0;

	return	0;
}


/* access number */
#define FAL_FONT_ELM_FILENAME   0
#define FAL_FONT_ELM_SIZE_W	1
#define FAL_FONT_ELM_SIZE_H	2
#define FAL_FONT_ELM_LETTER_W   3
#define FAL_FONT_ELM_LETTER_H   4
#define FAL_FONT_ELM_LETTER_X   5
#define FAL_FONT_ELM_LETTER_Y   6
#define FAL_FONT_ELM_STYLE_NAME 7
#define FAL_FONT_ELM_SHAPE_NAME 8

#define FAL_FONT_ELM_CODE_SET   10
#define FAL_FONT_ELM_PERMISSION 11
#define FAL_FONT_ELM_XLFDNAME   12

int
set_struct( tmp_data, elm )
FalFontData	*tmp_data;
char	*elm[];
{
	char    *dup_p;

	/* data information */

	/* a width of a character size */
	if ( fal_atoi( 
			elm[FAL_FONT_ELM_SIZE_W], &(tmp_data->size.w ) 
			) == FAL_ERROR 
	) {
		goto FalError02 ;
	}
	/* a height of a character size */
	if ( fal_atoi(
			 elm[FAL_FONT_ELM_SIZE_H], &(tmp_data->size.h )
			 ) == FAL_ERROR
	) {
		goto FalError02 ;
	}
	/* a width of a letter size */
	if ( fal_atoi(
			elm[FAL_FONT_ELM_LETTER_W], &(tmp_data->letter.w )
			) == FAL_ERROR 
	) {
		goto FalError02 ;
	}
	/* a height of a letter size */
	if ( fal_atoi( 
			elm[FAL_FONT_ELM_LETTER_H], &(tmp_data->letter.h )
			) == FAL_ERROR
	) {
		goto FalError02 ;
	}
	/* position x of a letter size */
	if ( fal_atoi(
			elm[FAL_FONT_ELM_LETTER_X], &(tmp_data->letter.x )
			) == FAL_ERROR
	) {
		goto FalError02 ;
	}
	/* position y of a letter size */
	if ( fal_atoi(
			elm[FAL_FONT_ELM_LETTER_Y], &(tmp_data->letter.y )
			) == FAL_ERROR 
	) {
		goto FalError02 ;
	}
	/* codeset */
	if ( fal_set_cs(
			elm[FAL_FONT_ELM_CODE_SET], &(tmp_data->cd_set )
			) == FAL_ERROR 
	) {
		goto FalError02 ;
	}
	/* a equipment of an output permission */
	if ( fal_set_prm(
			elm[FAL_FONT_ELM_PERMISSION], &(tmp_data->prm )
			) == FAL_ERROR 
	) {
		goto FalError02 ;
	}

	/* character information */

	/* xlfd name */
	if ( ( dup_p = (char *)strdup( elm[FAL_FONT_ELM_XLFDNAME] ) ) == NULL ) {
		fal_utyerrno = FAL_ERR_MALLOC ;
		return	FAL_ERROR;
	}
	tmp_data->xlfdname = dup_p;

	/* character style (character) */
	if ( ( dup_p = (char *)strdup( elm[FAL_FONT_ELM_STYLE_NAME] ) ) == NULL) {
		free( tmp_data->xlfdname );
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyerrno = FAL_ERR_MALLOC ;
		return	FAL_ERROR;
	}
	tmp_data->style.name = dup_p;

	/* character style (numerical) */
	tmp_data->style.def 
		= fal_read_db( tmp_data->style.name, (FalFontDB *)&fal_db_style );

	/* character shape (character) */
	if ( ( dup_p = (char *)strdup( elm[FAL_FONT_ELM_SHAPE_NAME] ) ) == NULL ) {
		free( tmp_data->style.name );
		free( tmp_data->xlfdname );
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyerrno = FAL_ERR_MALLOC ;
		return	FAL_ERROR;
	}
	tmp_data->shape.name = dup_p;

	/* character shape (numerical) */
	tmp_data->shape.def 
		= fal_read_db( tmp_data->shape.name, (FalFontDB *)&fal_db_shape );

	SET_STRUCT_OPTION( dup_p, elm, tmp_data, fal_utyerror, fal_db_group ) ;
	return	0;

FalError02:
	set_errfile_str( fal_err_file, fal_err_file_buf ) ;
	fal_utyerrno = FAL_ERR_FDATA_DSC ;
	return	FAL_ERROR;
}

/* get a full path name */
int     searchFontFileName( data, fullPathName )
FalFontData     data;
char    *fullPathName;
{
	int	rtn ;
	rtn =	falReadFontInfoLists(SRCHFNAME,
				NULL, NULL, NULL,
				data, fullPathName,
				NULL, NULL) ;
	if( fal_utyerrno != 0 ){
	    fal_utyerrno |= (FAL_FUNCNUM_SRCHFNM<<8) ;
	}
	return rtn ;
}


int     fal_eq_data( data, tmp_data )
FalFontData     data;
FalFontData     tmp_data;
{
	int     flg = 0;

	/* xlfd name */
	if ( strcmp( data.xlfdname, tmp_data.xlfdname ) != 0 ) {
		flg++;
	}

	/* a width of a character size */
	if ( ( flg == 0 ) && ( data.size.h != tmp_data.size.h ) ) {
		flg++;
	}

	/* a height of a character size */
	if ( ( flg == 0 ) && ( data.size.w != tmp_data.size.w ) ) {
		flg++;
	}
	/* a height of a letter size */
	if ( ( flg == 0 ) && ( data.letter.h != tmp_data.letter.h ) ) {
		flg++;
	}

	/* a width of a letter size */
	if ( ( flg == 0 ) && ( data.letter.w != tmp_data.letter.w ) ) {
		flg++;
	}

	/* position x of a letter size */
	if ( ( flg == 0 ) && ( data.letter.x != tmp_data.letter.x ) ) {
		flg++;
	}

	/* position y of a letter size */
	if ( ( flg == 0 ) && ( data.letter.y != tmp_data.letter.y ) ) {
		flg++;
	}

	/* a definition data of a character style */
	if ( ( flg == 0 ) && ( data.style.def != tmp_data.style.def ) ) {
		flg++;
	}

	/* character style */
	if ( (data.style.name != NULL) && (tmp_data.style.name != NULL) ) {
	    if ( ( flg == 0 ) && ( strcmp( data.style.name, tmp_data.style.name ) != 0 ) ) {
		flg++;
	    }
	}

	/* a definition data of a character shape */
	if ( ( flg == 0 ) && ( data.shape.def != tmp_data.shape.def ) ) {
		flg++;
	}

	/* character shape */
	if ( (data.shape.name != NULL) && (tmp_data.shape.name != NULL) ) {
	    if ( ( flg == 0 ) && ( strcmp( data.shape.name, tmp_data.shape.name ) != 0 ) ) {
		flg++;
	    }
	}

	FAL_EQ_DATA_OPTION( flg, data, tmp_data ) ;

	/* code set */
	if ( ( flg == 0 ) && ( data.cd_set != tmp_data.cd_set ) ) {
		flg++;
	}

	/* an equipment of an output permission */
	if ( ( flg == 0 ) && ( data.prm != tmp_data.prm ) ) {
		flg++;
	}

	if ( flg == 0 ) {
		return  0;
	} else if ( flg != 0 ) {
		return  _FAL_TRY_NEXT;
	} else {
		set_errfile_str( fal_err_file, fal_err_file_buf ) ;
		fal_utyerrno = FAL_ERR_FDATA_DSC ;
		return  FAL_ERROR;
	}
}


static int
CR_to_NULL(buf)
char    *buf;
{
	for(  ; *buf != '\0'; buf++ ) {
		if (*buf == '\n') {
			*buf = '\0';
			break;
		}
	}
	return 0 ;
}

char *fal_get_base_name( str )
char    *str;
{
	char    *str_slash;

	str_slash = strrchr( str, '/');
	if ( str_slash == NULL ) {
		return( str );
	}
	return( ++str_slash );
}


static int
fal_cmp_data( op1, key, mask )
FalFontData     *op1;
FalFontData     *key;
int	mask ;
{
	if ( mask == 0 ) {
		return(0);
	}

	if ( key == NULL ) {
		return(0);
	}

	if ( 
		( mask & FAL_FONT_MASK_XLFDNAME ) 
		&& strcmp( op1->xlfdname, key->xlfdname ) 
	) {
		return	_FAL_TRY_NEXT;
	}
	if ( 
		( mask & FAL_FONT_MASK_SIZE_W ) 
		&& (op1->size.w != key->size.w ) 
	) {
		return	_FAL_TRY_NEXT;
	}
	if ( 
		( mask & FAL_FONT_MASK_SIZE_H ) 
		&& (op1->size.h != key->size.h )
	) {
		return	_FAL_TRY_NEXT;
	}
	if ( 
		( mask & FAL_FONT_MASK_LETTER_W ) 
		&& ( op1->letter.w != key->letter.w )
	) {
		return	_FAL_TRY_NEXT;
	}
	if ( 
		( mask & FAL_FONT_MASK_LETTER_H ) 
		&& ( op1->letter.h != key->letter.h ) 
	) {
		return	_FAL_TRY_NEXT;
	}
	if ( 
		( mask & FAL_FONT_MASK_LETTER_X ) 
		&& ( op1->letter.x != key->letter.x )
	) {
		return	_FAL_TRY_NEXT;
	}
	if ( 
		( mask & FAL_FONT_MASK_LETTER_Y ) 
		&& ( op1->letter.y != key->letter.y ) 
	) {
		return	_FAL_TRY_NEXT;
	}
	if ( 
		( mask & FAL_FONT_MASK_STYLE_DEF ) 
		&& ( op1->style.def != key->style.def ) 
	) {
		return	_FAL_TRY_NEXT;
	}
	if( (op1->style.name != NULL) && (key->style.name != NULL) ) {
	    if ( 
		( mask & FAL_FONT_MASK_STYLE_NAME ) 
		&& strcmp( op1->style.name, key->style.name ) 
	    ) {
		return	_FAL_TRY_NEXT;
	    }
	}
	if ( 
		( mask & FAL_FONT_MASK_SHAPE_DEF ) 
		&& ( op1->shape.def != key->shape.def )
	) {
		return	_FAL_TRY_NEXT;
	}
	if( (op1->shape.name != NULL) && (key->shape.name != NULL) ) {
	    if ( 
		( mask & FAL_FONT_MASK_SHAPE_NAME ) 
		&& strcmp( op1->shape.name, key->shape.name )
	    ) {
		return	_FAL_TRY_NEXT;
	    }
	}
	FAL_CMP_DATA_OPTION( mask, op1, key ) ;	
	if ( 
		( mask & FAL_FONT_MASK_CODE_SET ) 
		&& ( op1->cd_set != key->cd_set )
	) {
		return	_FAL_TRY_NEXT;
	}
	if ( mask & FAL_FONT_MASK_PERMISSION ) {
		int     cmp_prm;
		cmp_prm = key->prm & (FAL_FONT_DISPLAY | FAL_FONT_PRINTER);
		if ( ( op1->prm & cmp_prm ) != cmp_prm ) {
			return	_FAL_TRY_NEXT;
		}
	}
	return	0;
}


/* sort flag                      */
/* return data ¡§   sort    ... 1 */
/*                  no sort ... 0 */

static int
new_target( target, choose )
FalFontData	*target;
FalFontData	*choose;
{
	FalFontData	diff;
	DEF_STR_CHK ;

	DEF_STR_SET ;

	diff.style.def = choose->style.def - target->style.def;
	if( (choose->style.name != NULL) && (target->style.name != NULL) ) {
	    str_chk.style =  strcmp( choose->style.name, target->style.name ) ;
	}

	NEW_TARGET_SET( diff, choose, target, str_chk ) ;

	diff.shape.def = choose->shape.def - target->shape.def;
	if( (choose->shape.name != NULL) && (target->shape.name != NULL) ) {
	    str_chk.shape =  strcmp( choose->shape.name, target->shape.name ) ;
	}

	diff.cd_set   = choose->cd_set   - target->cd_set;
	diff.size.h   = choose->size.h   - target->size.h;
	diff.size.w   = choose->size.w   - target->size.w;
	diff.letter.h = choose->letter.h - target->letter.h;
	diff.letter.w = choose->letter.w - target->letter.w;

	/* style */
	if ( target->style.def == FAL_FONT_UNKNOWN ) {
		if ( choose->style.def > 0 ) { 
			return	1; 
		}
		/* style is FAL_FONT_UNKNOWN both "target" and "choose" */
		if ( str_chk.style < 0 ) { 
			return	1; 
		}
		if ( str_chk.style > 0) { 
			return	0; 
		}
	} else if ( choose->style.def == FAL_FONT_UNKNOWN ) {
		return	0;
	}
	/* target->style.def and choose->style.def is not FAL_FONT_UNKNOWN */
	if (diff.style.def < 0) { 
		return	1; 
	}
	if ( diff.style.def > 0) { 
		return	0; 
	}

	NEW_TARGET_CHK( diff, choose, target, str_chk ) ;

	/* character shape */
	if ( target->shape.def == FAL_FONT_UNKNOWN ) {  
		if ( choose->shape.def > 0 ) { 
			return	1; 
		}
		if ( str_chk.shape < 0 ) { 
			return	1; 
		}
		if ( str_chk.shape > 0 ) { 
			return	0; 
		}
	} else if (choose->shape.def == FAL_FONT_UNKNOWN ) {
		return	0;
	}
	if ( diff.shape.def < 0 ) { 
		return	1; 
	}
	if ( diff.shape.def > 0 ) { 
		return	0; 
	}

	/* codeset */
	if ( diff.cd_set  < 0 ) { 
		return	1; 
	}
	if (diff.cd_set > 0) { 
		return	0; 
	}

	/* character size height */
	if ( diff.size.h < 0 ) { 
		return	1; 
	}
	if ( diff.size.h > 0 ) { 
		return	0; 
	}

	/* letter size hieght */
	if ( diff.letter.h < 0 ) { 
		return	1; 
	}
	if ( diff.letter.h > 0 ) { 
		return	0; 
	}

	/* character size wide */
	if ( diff.size.w < 0 ) { 
		return	1; 
	}
	if ( diff.size.w > 0 ) { 
		return	0; 
	}

	/* letter size wide */
	if ( diff.letter.w < 0 ) { 
		return	1; 
	}
	if ( diff.letter.w > 0 ) { 
		return	0; 
	}

	return	0;
}

#if NeedFunctionPrototypes
int 	FalFontOfFontID(
	FalFontID	fid,
	FalFontData	*fontdata )
#else
int 	FalFontOfFontID( fid, fontdata )
FalFontID	fid;
FalFontData	*fontdata;
#endif
{
	int	rtn ;
	rtn =	falReadFontInfoLists(FONTOFID,
				NULL, NULL, NULL,
				NULL, NULL,
				fid, fontdata) ;
	if( fal_utyerrno != 0 ){
	    fal_utyerrno |= (FAL_FUNCNUM_FID<<8) ;
	}
	return rtn ;
}

#if NeedFunctionPrototypes
FalFontID
FalOpenSysFont(
	FalFontData	*open_font_data,
	int 		font_data_mask,
	FalFontDataList	**missing_font_list_return )
#else
FalFontID
FalOpenSysFont( open_font_data, font_data_mask, missing_font_list_return )
FalFontData	*open_font_data;
int 		font_data_mask;
FalFontDataList	**missing_font_list_return;
#endif
{
	int 		retFL;  /* return a data of FontList */
	FalFontID 	retOF;  /* return a data of OpenFont */
	char	fname[FAL_LINE_MAX];

	/* follow "font_data_mask" and call "FalGetFontList()" */
	retFL = FalGetFontList( 
			open_font_data, font_data_mask, missing_font_list_return 
		);
	/* case of an abnormal end */
	if ( retFL != 0 ) {
		*missing_font_list_return = NULL;
		return	0;
	}

	if ( (*missing_font_list_return)->num > 1 ) {
		return  0;
	} else if ( (*missing_font_list_return)->num == 1 ) {
	/* investigator an font file name */
		int	slct_cdset, prm, islock, dspcode ;
		if ( 
			searchFontFileName( 
				(*missing_font_list_return)->list[0], fname 
			) == FAL_ERROR 
		){
			FalFreeFontList( *missing_font_list_return  );
			fal_utyerrno &= 0xff ;
			fal_utyerrno |= (FAL_FUNCNUM_SRCHFNM<<8) ;
			*missing_font_list_return = NULL;
			return	0;
		}
		/* open a font by "__FalOpenFont()" */
		slct_cdset = (*missing_font_list_return)->list[0].cd_set ;
		dspcode  = ( font_data_mask & FAL_FONT_MASK_GLYPH_INDEX )?
			FAL_FONT_GLYPH_INDEX : slct_cdset ;
		prm = ((*missing_font_list_return)->list[0].prm ) ?
			(*missing_font_list_return)->list[0].prm :
			FAL_FONT_DISPLAY | FAL_FONT_PRINTER ;
		islock = (( font_data_mask & FAL_FONT_MASK_UPDATE )? 1 : 0 );
		retOF = __FalOpenFont( fname, prm, dspcode, islock );

		if ( retOF == (FalFontID)FAL_ERROR ) {
		/* abnomal end */
			FalFreeFontList( *missing_font_list_return );
			fal_utyerrno &= 0xff ;
			fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
			*missing_font_list_return = NULL;
			return	0;
		} else {
		/* normal end ( return  retOF ) */
			*missing_font_list_return = NULL;
			FalFreeFontList( *missing_font_list_return );
			if( fal_add_fidinf( retOF,
				dspcode, slct_cdset, islock ) == FAL_ERROR ){
				fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
				return 0 ;
			}
			return  retOF;
		}
	} else if ( (*missing_font_list_return)->num == 0 ) {
	/* 
	 * set NULL in "missing_font_list_return"
	 */
		FalFreeFontList( *missing_font_list_return );
		*missing_font_list_return = NULL;
		return  0;
	} else {
	/* case abnormal */
		*missing_font_list_return = NULL;
		fal_utyerrno = FAL_ERR_FATAL ;
		fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ;
		return  0;
	}
}


/********************************************************
*	function to access fonts.list 
********************************************************/
static
FILE	*open_fonts_list()
{

	FILE    *fp;
	char	*locale			= NULL ;
	char	*fal_search_path	= NULL ;
	char	flist_d[FAL_LINE_MAX] ;
	char	flist_c[FAL_LINE_MAX] ;
	char	flist_l[FAL_LINE_MAX] ;

	GETLISTFILEPATH( locale, fal_search_path, flist_d, flist_c, flist_l, FONTS_LIST_FILE ) ;

	if ( ( fp = fopen( flist_l, "r" ) ) == NULL ) {
	    if( (fp = fopen( flist_c, "r" )) == NULL ) { 
		if( (fp = fopen( flist_d, "r" )) == NULL ) { 
		    set_errfile_str( fal_err_file, flist_d ) ;
		    set_errfile_str( fal_err_file_buf, flist_d ) ;
		    fal_utyerror = _FAL_DATA_OPEN_ER;
		    fal_utyderror = errno;
		    fal_utyerrno = FAL_ERR_FDATA_OPN ;
		}else{
		    /* Save full path of fonts.list. */
		    set_errfile_str( fal_err_file_buf, flist_d ) ;
		    if( IsDefaultPath == TRUE )	IsDefaultPath = FALSE ;
		}
	    }else{
		/* Save full path of fonts.list. */
		set_errfile_str( fal_err_file_buf, flist_c ) ;
		if( IsDefaultPath == TRUE )	IsDefaultPath = FALSE ;
	    }
	}else{
	    /* Save full path of fonts.list. */
	    set_errfile_str( fal_err_file_buf, flist_l ) ;
	    if( IsDefaultPath == TRUE )	IsDefaultPath = FALSE ;
	}

	return( fp ) ;
}



/*
 *	functions to read "fonts.dir" files
 */

/***************************************************************/
/* read a font information and add a list or make a list       */
/***************************************************************/


static int
set_font_pathlist(pathlist, nodef)
FalFontPath	*pathlist ;
int		nodef ;
{
	FILE    *fp;
	char    buf[FAL_LINE_MAX];
	char    *font_path = NULL ;
	char    **tmp_list ;
	int     num;

	errno = 0;
	/* open a font directory information file */
	if ((fp = open_fonts_list()) == NULL) {
	    /* Make default path list */
	    if( errno == ENOENT || errno == EACCES || errno == EISDIR ){
		if ( make_default_path(pathlist) ) {
		    return FAL_ERROR ;
		} 
		return 0 ;
	    }
	    return FAL_ERROR ;
	} 
	memset( buf, '\0', sizeof(buf) ) ;

	while ( fgets( buf, FAL_LINE_MAX, fp ) != (char *)NULL ){
		struct	stat	statbuf ;
		char	*ep ;

		CR_to_NULL(buf);

		if( buf[0] == '#' ){		/* comments */
		    continue ;
		}else if( buf[0] == '/' ){	/* font path */

		    /* add font_path to pathlist */
		    if( font_path ){
			num = pathlist->num;
			if (pathlist->path == NULL) {
			    tmp_list = (char **)malloc(sizeof(char *)*(num +1));
			} else {
			    tmp_list = (char **)realloc(pathlist->path, sizeof(char *)*(num+1));
			}
			if (tmp_list == NULL) {
			    fclose( fp );
			    free( font_path );
			    fal_clear_font_path( pathlist );
			    fal_utyerror = _FAL_MALOC_ER;
			    fal_utyerrno = FAL_ERR_MALLOC ;
			    return FAL_ERROR ;
			}
			pathlist->path = tmp_list;
			pathlist->path[num] = font_path;
			pathlist->num++;
		    }

		    /* get font_path */
		    if ((font_path = (char *)strdup(buf)) == NULL) {
			fclose( fp );
			fal_clear_font_path( pathlist );
			fal_utyerror = _FAL_MALOC_ER;
			fal_utyerrno = FAL_ERR_MALLOC ;
			return FAL_ERROR ;
		    }
		    ep = font_path + strlen( font_path ) - 1 ;
		    *ep = '/';			/* ':' -> '/' */
		    if( *(ep-1) == '/' )	*ep = '\0';

		    if( stat( font_path, &statbuf ) ) {
			free( font_path ) ;
			font_path = NULL ;
			continue;
		    }
		    if( !(statbuf.st_mode & S_IFDIR) ) {
			free( font_path ) ;
			font_path = NULL ;
			continue;
		    }

		}else{	/* font file */
		    if( nodef ){
			if( font_path ){
			    free( font_path ) ;
			    font_path = NULL ;
			}
		    }
		    continue ;
		}

	}
	/* add font_path to pathlist */
	if( font_path ){
	    num = pathlist->num;
	    if (pathlist->path == NULL) {
		    tmp_list = (char **)malloc(sizeof(char *)*(num +1));
	    } else {
		    tmp_list = (char **)realloc(pathlist->path, sizeof(char *)*(num+1));
	    }
	    if (tmp_list == NULL) {
		fclose( fp );
		free( font_path );
		fal_clear_font_path( pathlist );
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyerrno = FAL_ERR_MALLOC ;
		return FAL_ERROR ;
	    }
	    pathlist->path = tmp_list;
	    pathlist->path[num] = font_path;
	    pathlist->num++;
	}

	if ( feof(fp) != 0 ){
	    fclose(fp);
	    return 0 ;
	}
	fclose(fp);
	fal_clear_font_path( pathlist );
	set_errfile_str( fal_err_file, fal_err_file_buf ) ;
	fal_utyerror = _FAL_READ_PATH_ER;
	fal_utyerrno = FAL_ERR_FPATH ;
	return	FAL_ERROR;
}



static int
make_default_path(pathlist)
FalFontPath	*pathlist ;
{
	struct	stat	statbuf ;
	char    pbuf[FAL_LINE_MAX], *dir, *p ;
	char    *path, **plist ;
	char    *sp, *ep ;
	int     len, num;

	memset( pbuf, '\0', sizeof(pbuf) ) ;

	p = pbuf ;
	if( (dir = getenv( "DTUDCFONTPATH" )) != NULL ) {
		len = strlen( dir ) ;
		strcpy( p, dir ) ;
		p[len++] = ':' ;
		p += len ;
	}
	strcpy( p, DTUDCFONTPATH ) ;
	strcat( p, ":" ) ;

	sp = pbuf ;
	while ( (ep = strchr( sp, ':' )) != (char *)NULL ){

	    /* get path */
	    *ep = '\0' ;
	    if ((path = (char *)malloc(sizeof(char) * (strlen(sp)+2))) == NULL) {
		fal_clear_font_path( pathlist );
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyerrno = FAL_ERR_MALLOC ;
		return FAL_ERROR ;
	    }
	    strcpy( path, sp ) ;
	    len = strlen( sp ) ;
	    path[len++] = '/' ;
	    path[len] = '\0' ;
	    if( path[len-2] == '/' )	path[len-1] = '\0';

	    if( stat(path, &statbuf) ) {
		free( path ) ;
		sp = ep + 1 ;
		continue;
	    }
	    if( !(statbuf.st_mode & S_IFDIR) ) {
		free( path ) ;
		sp = ep + 1 ;
		continue;
	    }

	    /* add path to pathlist */
	    num = pathlist->num;
	    if (pathlist->path == NULL) {
		plist = (char **)malloc(sizeof(char *)*(num +1));
	    } else {
		plist = (char **)realloc(pathlist->path, sizeof(char *)*(num+1));
	    }
	    if (plist == NULL) {
		fal_clear_font_path( pathlist );
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyerrno = FAL_ERR_MALLOC ;
		return FAL_ERROR ;
	    }
	    pathlist->path = plist;
	    pathlist->path[num] = path;
	    pathlist->num++;
	    sp = ep + 1 ;
	}
	if( execDefined == TRUE )	execDefined = FALSE ;
	if( IsDefaultPath == FALSE )	IsDefaultPath = TRUE ;
	fal_utyerror = 0 ;
	fal_utyderror = 0 ;
	fal_utyerrno = 0 ;
	return	0 ;
}




static	int
falReadFontInfoLists(func, lstkey, mask, fls, fnkey, fullpath, fid, fdata)
int		func ;
			    /* parameters for FalGetFontList	*/
FalFontData	*lstkey;
int		mask;
FalFontDataList	*fls;
			    /* parameters for searchFontFileName	*/
FalFontData	fnkey;
char		*fullpath;
			    /* parameters for FalFontOfFontID	*/
FalFontID	fid ;
FalFontData	*fdata; 
{
    FILE    	*fp;
    char    	pname[ FAL_LINE_MAX ] ;
    Oak_FontInf *finf;

    FalFontData	tmp_data;
    FontIDInfo	fontid_inf;
    char	*elm[FAL_DATA_ELM * 3];
    char	buf[FAL_LINE_MAX * 3];

    struct	stat	statbuf ;
    char	*keyfname, *fontFileName, *fonts_list_file ;
    int     	i, j ;
    int		fontnum ;
    int		codeset_num, *codeset_list ;

    keyfname = fontFileName = fonts_list_file = NULL ;
    codeset_num = 0 ;
    codeset_list = NULL ;
    if( func & FONTOFID ){
	finf = (Oak_FontInf *)fid ;
	keyfname = finf->fname ;
    }

    /* get current locale */
    if( fal_init() )	return FAL_ERROR ;

    /* get font path list */
    if( wpath.path==NULL ) {
	if( set_font_pathlist( &wpath, 1 ) ) {
	    return FAL_ERROR ;
	}
    }
    /*
     * If execDefined is true, this function read detail informations
     * of the fonts descripted in fonts.list.
     */
    if( execDefined == TRUE && IsDefaultPath == FALSE ) {
	/* open an information file */
	if ((fp = open_fonts_list()) == NULL) {
	    return FAL_ERROR ;
	}
	memset( pname, 0, sizeof( pname ) );

	while ( fgets( buf, FAL_LINE_MAX, fp ) != ( char * ) NULL ) {
	    int     eq;

	    /* clear an area */
	    memset( elm, 0, sizeof( elm ) );
	    memset( &tmp_data, 0, sizeof( tmp_data ) );
	    CR_to_NULL( buf );

	    /*
	     * get font information of a font
	     */
	    switch( fal_get_def_fontdata(func, pname, buf, elm, &tmp_data, keyfname ) ) {
		case    _FAL_TRY_NEXT:
		    if( buf[0] == '/' ) {
			buf[strlen( buf ) -1] = '/' ;
			strcpy( pname, buf ) ;
		    }
		    continue;
		case    0:
		    break;
		case    FAL_ERROR:
		default:
		    goto FalError ;
	    }

	    switch( func ) {
	    case MKFONTLIST :		/* FalGetFontList 	*/
		switch( falgetfontlist(&tmp_data, lstkey, mask, fls, 1) ){
		case _FAL_TRY_NEXT :
		    continue ;
		case 0:
		    break;
		case FAL_ERROR:
		default:
		    goto FalError ;
		}
		break ;
	    case SRCHFNAME :		/* searchFontFileName 	*/
		eq = fal_eq_data( fnkey, tmp_data ) ;
		switch ( eq ) {
		    case 0 :
			sprintf( fullpath, "%s%s", pname, elm[ FAL_FONT_ELM_FILENAME ] );
			fal_clear_data( &tmp_data ) ;
			fclose(fp) ;
			return 0 ;
		    case _FAL_TRY_NEXT :
			fal_clear_data( &tmp_data ) ;
			break ;
		    default :	/* FAL_ERROR */
			goto FalError ;
		}
		break ;
	    case FONTOFID :		/* FalFontOfFontID 	*/
		memmove( fdata, &tmp_data, sizeof(FalFontData) ) ;
		fclose(fp) ;
		return 0 ;
	    default :
		fal_utyerrno = FAL_ERR_PARM ;
		goto FalError ;
	    }

	}	/* end of while loop */
	/* EOF */
	if ( feof( fp ) != 0 ) {
	    fclose( fp );
	}else{
	    set_errfile_str( fal_err_file, fal_err_file_buf ) ;
	    fal_utyerror = _FAL_DATA_READ_ER;
	    fal_utyerrno = FAL_ERR_FDATA_RD ;
	    fclose( fp );
	    return FAL_ERROR ;
	}

	if( (func & SRCHFNAME) && (execUndefined == FALSE) ){
	    fal_utyerrno = FAL_ERR_NOFONT ;
	    return  FAL_ERROR  ;
	}

    }	/* execDefined */

    /*
     * If execUnefined is true, this function read list of font path,
     * and get the name of font file from fonts.dir.
     */
    if( execUndefined == TRUE ) {

	/* 
	* search font path
	*/
	for( i=0; i<wpath.num; i++ ) {
	    FalFontData	*p_bak ;

	    fonts_list_file = (char *)malloc( sizeof(char) * ( strlen(wpath.path[i]) + strlen(FONTSDIR) + 1 ) ) ;
	    if( fonts_list_file == (char *)NULL ) {
		fal_utyerror = _FAL_MALOC_ER;
		fal_utyerrno = FAL_ERR_MALLOC ;
		return FAL_ERROR ;
	    }
	    sprintf( fonts_list_file, "%s%s", wpath.path[i], FONTSDIR ) ;

	    if( stat( fonts_list_file, &statbuf ) ) {
		FreeString( fonts_list_file ) ;
		continue ;
	    }
	    if( !(statbuf.st_mode & S_IFREG) ) {
		FreeString( fonts_list_file ) ;
		continue ;
	    }
	    errno = 0 ;
	    /* open "fonts.dir" */
	    if ((fp = fopen( fonts_list_file, "r" )) == NULL) {
		if( errno == EACCES ) {
		    FreeString( fonts_list_file ) ;
		    continue ;
		}else{
		    set_errfile_str( fal_err_file, fonts_list_file ) ;
		    FreeString( fonts_list_file ) ;
		    fal_utyerror = _FAL_OPEN_ER;
		    fal_utyderror = errno ;
		    fal_utyerrno = FAL_ERR_FNTDIR_OPN ;
		    return FAL_ERROR ;
		}
	    } 
	    /* Save full path of fonts.dir */
	    set_errfile_str( fal_err_file_buf, fonts_list_file ) ;
	    FreeString( fonts_list_file ) ;

	    /* Get number of fonts in current path */
	    if( fgets( buf, FAL_LINE_MAX, fp ) == (char *)NULL ) {
		set_errfile_str( fal_err_file, fal_err_file_buf ) ;
		fal_utyerror = _FAL_DATA_READ_ER;
		fal_utyerrno = FAL_ERR_FNTDIR_RD ;
		fclose(fp);
		return	FAL_ERROR;
	    }
	    fontnum = atoi( buf );

	    /* Get list of font informations */
	    if( func & MKFONTLIST){
		if ( fls->list == NULL ) {
		    p_bak  = (FalFontData *)malloc(sizeof(FalFontData) * (fls->num + fontnum));
		} else {
		    p_bak  = (FalFontData *)realloc(fls->list, sizeof(FalFontData)*(fls->num + fontnum));
		}
		if ( p_bak == NULL ) {
		    fal_utyerror = _FAL_MALOC_ER;
		    fal_utyerrno = FAL_ERR_MALLOC ;
		    return  FAL_ERROR  ;
		}
		fls->list = p_bak ;
	    }

	    memset( buf, 0, sizeof(buf));

	    while( fgets( buf, FAL_LINE_MAX, fp ) != (char *)NULL ) {
		int	tmp_num, eq ;
		char	*ep, *xlfd ;

		/* set FalFontData */
		CR_to_NULL(buf);	/* '\n' -> '\0' */
		memset( &tmp_data, 0, sizeof(FalFontData)); 

		/* parse string of font file */
		if( (ep = strchr( buf, ' ' )) == (char *)NULL ) continue ;
		else	*ep = 0 ;
		/* refuse font file (not gpf) */
		if(
		    ( strcmp( FILE_SUFFIX(buf), PCFSUFFIX ) != 0 ) &&
		    ( strcmp( FILE_SUFFIX(buf), SNFSUFFIX ) != 0 ) &&
		    ( ISOPTFONT(buf) != 0 )
		){
		    continue ;
		}

		/* get string of font file */
		if( (func & SRCHFNAME) || (func & FONTOFID) ){
		    fontFileName = (char *)malloc( sizeof(char) * ( strlen(wpath.path[i]) + strlen(buf) + 1 ) ) ;
		    if( fontFileName == (char *)NULL ) {
			fal_utyerror = _FAL_MALOC_ER;
			fal_utyerrno = FAL_ERR_MALLOC ;
			return FAL_ERROR ;
		    }
		    sprintf( fontFileName, "%s%s", wpath.path[i], buf ) ;
		}

		/* read font file and get properties */
		xlfd = ++ep ;
		switch( fal_get_undef_fontdata(func, fontFileName, xlfd, &tmp_data, keyfname, &codeset_num, &codeset_list) )
		{
		    case _FAL_TRY_NEXT :
			if( (func & SRCHFNAME) || (func & FONTOFID) )	FreeString( fontFileName ) ;
			fal_clear_data( &tmp_data );
			memset( buf, 0, sizeof(buf));
			continue ;
		    case 0:
			if( (func & FONTOFID) )	FreeString( fontFileName ) ;
			break;
		    case FAL_ERROR:
		    default:
			if( (func & SRCHFNAME) || (func & FONTOFID) )	FreeString( fontFileName ) ;
			goto FalError ;
		}

		if( !(func & FONTOFID) && (codeset_num == 0) ){
		    if( (func & SRCHFNAME) )	FreeString( fontFileName ) ;
		    fal_clear_data( &tmp_data );
		    memset( buf, 0, sizeof(buf));
		    continue ;
		}

		switch( func ) {
		case MKFONTLIST :		/* FalGetFontList 	*/
		    tmp_num = fls->num ;
		case SRCHFNAME :		/* searchFontFileName 	*/
		    /* set font data */
		    for( j=0; j<codeset_num; j++ ) {
			tmp_data.cd_set = FALGETFALCODESET( codeset_list[j] ) ;

			if( func & MKFONTLIST ){	/* FalGetFontList */
			    /* Ignore fonts already exists. */
			    if ( fal_check_already_exist(&tmp_data, fls) >= 0 ) {
				continue;
			    }

			    switch( falgetfontlist(&tmp_data, lstkey, mask, fls, 1) ){
			    case _FAL_TRY_NEXT :
				continue ;
			    case 0:
				break;
			    case FAL_ERROR:
			    default:
				goto FalError ;
			    }
			}
			if( func & SRCHFNAME ){		/* searchFontFileName */
			    eq = fal_eq_data( fnkey, tmp_data ) ;
			    switch ( eq ) {
				case 0 :
				    strcpy( fullpath, fontFileName );
				    fal_clear_data( &tmp_data ) ;
				    FreeString( fontFileName ) ;
				    tmp_codeset = tmp_data.cd_set ;
				    fclose(fp) ;
				    /* clear code set informations */
				    clear_charset_info() ;
				    return 0  ;
				case _FAL_TRY_NEXT :
				    break ;
				default :	/* FAL_ERROR */
				    FreeString( fontFileName ) ;
				    goto FalError ;
			    }
			}
		    }	/* for loop */
		    if( func & MKFONTLIST ){	/* FalGetFontList */
			if( fls->num == tmp_num )	fal_clear_data( &tmp_data ) ;
		    }
		    if( func & SRCHFNAME ){	/* searchFontFileName */
			fal_clear_data( &tmp_data ) ;
			FreeString( fontFileName ) ;	/* _FAL_TRY_NEXT */
		    }
		    break ;

		case FONTOFID :		/* FalFontOfFontID 	*/
		    memmove( fdata, &tmp_data, sizeof(FalFontData) ) ;
		    if( fal_read_fidinf( fid, &fontid_inf ) != FAL_ERROR )
			fdata->cd_set = fontid_inf.cd_set ;
		    else {
			fdata->cd_set = 0 ;
			goto FalError ;
		    }
		    fclose(fp) ;
		    return 0  ;

		default :
		    goto FalError ;
		}

		memset(buf, 0, sizeof(buf));
	    }	/* while loop */

	    if ( feof(fp) != 0 ) {
		fclose(fp);
	    }else{
		set_errfile_str( fal_err_file, fal_err_file_buf ) ;
		fal_utyerror = _FAL_DATA_READ_ER;
		fal_utyerrno = FAL_ERR_FNTDIR_RD ;
		fclose(fp);
		return	FAL_ERROR;
	    }

	}	/* for loop */

	/* clear code set informations */
	clear_charset_info() ;

	if( (func & SRCHFNAME) || (func & FONTOFID) ){
	    fal_utyerrno = FAL_ERR_NOFONT ;
	    return  FAL_ERROR  ;
	}

    }	/* execUndefined */

    return 0  ;

FalError:

    clear_charset_info() ;
    fal_clear_data( &tmp_data ) ;
    fclose(fp) ;
    return FAL_ERROR ;

}


/* clear code set informations */
static void
clear_charset_info()
{
	if( charset_str_buf )	free( charset_str_buf ) ;
	charset_str_buf = NULL ;
	if( codeset_list_sav )	free( codeset_list_sav ) ;
	codeset_list_sav = NULL ;
	codeset_list_num = 0 ;
}


static	int
fal_get_def_fontdata(func, pname, buf, elm, tmp_data, key_fname)
int		func ;
char		*pname ;	/* font path */
char		*buf ;		/* buffer for fgets() */
char		**elm ;
FalFontData	*tmp_data ;
char		*key_fname ;
{
	int	rtn ;
	char	tmp_fname[FAL_LINE_MAX] ;

	if( (rtn = fal_split_data( buf, FAL_DATA_ELM, elm )) ) {
	    return  rtn ;
	}

	if( func & FONTOFID ){
	    sprintf( tmp_fname, "%s%s", pname, elm[ FAL_FONT_ELM_FILENAME ] );
	    if( strcmp( tmp_fname, key_fname ) )	return _FAL_TRY_NEXT ;
	}

	/* set informations to structrue */
	if ( set_struct( tmp_data, elm ) == FAL_ERROR ) {
	    return  FAL_ERROR;
	}
	return 0 ;
}



static	int
falgetfontlist(tmp_data, key, mask, fls, fontnum)
FalFontData	*tmp_data ;
FalFontData	*key ;
int		mask ;
FalFontDataList	*fls ;
int		fontnum ;
{
    FalFontData	*p_bak ;

    if( fontnum == 0 ){
	return _FAL_TRY_NEXT ;
    }

    /* compare with key data */
    if ( fal_cmp_data(tmp_data, key, mask) == _FAL_TRY_NEXT ) {
	return _FAL_TRY_NEXT ;
    }

    if ( fls->list == NULL ) {
	p_bak  = malloc(sizeof(FalFontData) * (fls->num + fontnum));
    } else {
	p_bak  = realloc(fls->list, sizeof(FalFontData)*(fls->num + fontnum));
    }
    if ( p_bak == NULL ) {
	fal_utyerror = _FAL_MALOC_ER;
	fal_utyerrno = FAL_ERR_MALLOC ;
	return  FAL_ERROR  ;
    }
    fls->list = p_bak;
    memmove(&(fls->list[fls->num]), tmp_data, sizeof(FalFontData));
    fls->num++;	/* update number of fonts */

    return 0 ;
}



static	int
fal_get_undef_fontdata(func, full_path, xlfd, tmp_data, key_fname, codeset_num, codeset_list)
int		func ;
char		*full_path ;
char		*xlfd ;
FalFontData	*tmp_data ;
char		*key_fname ;
int		*codeset_num ;
int		**codeset_list ;
{
	int	rtn, pix ;
	char	*char_set ;
	char	xlfdname[ FAL_LINE_MAX ], *family, *pixsize ;
	char	*cbuf, *ep ;

	/* FalFontOfFontID() */
	if( func & FONTOFID ){
	    if( strcmp( full_path, key_fname ) )  return _FAL_TRY_NEXT ;
	}

	/*
	 * parse XLFD and get font informations
	 */
	cbuf = ep = family = pixsize = NULL ;
	/* XLFD */
	strcpy( xlfdname, xlfd ) ;
	/* pixel size */
	GETXLFDELMSTR( pixsize, xlfd, XLFD_ELM_PIXEL_SIZE ) ;
	ep = (char *)strchr( pixsize, '\0' ) ;
	pix = (int)strtol( pixsize, &cbuf, 10 ) ;
	if( cbuf == pixsize || cbuf != ep ){
	    return _FAL_TRY_NEXT ;	
	}
	/* family name */
	GETXLFDELMSTR( family, xlfd, XLFD_ELM_FAMILY_NAME ) ;

	/* set font data */
	if( (tmp_data->style.name = (char *)strdup( family )) == NULL ) {
	    fal_utyerror = _FAL_MALOC_ER ;
	    fal_utyerrno = FAL_ERR_MALLOC ;
	    return FAL_ERROR ;
	}
	if( (tmp_data->xlfdname = (char *)strdup( xlfdname )) == NULL ) {
	    fal_utyerror = _FAL_MALOC_ER ;
	    fal_utyerrno = FAL_ERR_MALLOC ;
	    return FAL_ERROR ;
	}
	tmp_data->size.h = pix ;
	tmp_data->size.w = -1 ;

	/* FalGetFontList() and searchFontFilename() */
	if( (func & MKFONTLIST) || (func & SRCHFNAME) ){
	    GETCHARSETSTR( char_set, tmp_data->xlfdname ) ;
	    /* code set infomations exist */
	    if( charset_str_buf && !strcmp( charset_str_buf, char_set ) ) {
		*codeset_num  = codeset_list_num ;
		*codeset_list = codeset_list_sav ;
	    }else{
		/* clear code set infomations */
		clear_charset_info() ;
		/* get code set infomations */
		if( (rtn = fal_get_codeset( fal_locale, char_set, codeset_list, codeset_num )) ){
		    return rtn ;
		}
		/* save code set infomations */
		if( (charset_str_buf = (char *)strdup( char_set )) == NULL ) {
		    fal_utyerror = _FAL_MALOC_ER ;
		    fal_utyerrno = FAL_ERR_MALLOC ;
		    return FAL_ERROR ;
		}
		codeset_list_num = *codeset_num  ;
		codeset_list_sav = *codeset_list ;
	    }
	}
	return 0 ;
}



/*
 *	get file name of fonts
 */

#if NeedFunctionPrototypes
int
FalFontIDToFileName(
	FalFontID	fid,
	char		**file_name )
#else
int
FalFontIDToFileName( fid, file_name )
FalFontID	fid;
char		**file_name;
#endif
{
	char		*fname ;
	Oak_FontInf	*finf ;
	if( fid == NULL ) {
		fal_utyerror  = _FAL_PARM_ER ;
		fal_utyderror = _FAL_R_P_FINF_DER ;
		fal_utyerrno = FAL_ERR_PARM ;
		fal_utyerrno |= (FAL_FUNCNUM_IDTOFNM<<8) ;
		return FAL_ERROR ;
	}
	if( file_name == NULL ) {
		fal_utyerror  = _FAL_PARM_ER ;
		fal_utyderror = _FAL_R_P_W_DER ;
		fal_utyerrno = FAL_ERR_PARM ;
		fal_utyerrno |= (FAL_FUNCNUM_IDTOFNM<<8) ;
		return FAL_ERROR ;
	}
	finf = (Oak_FontInf *)fid ;
	if( (fname = (char *)strdup( finf->fname )) == NULL ) {
		fal_utyerror = _FAL_MALOC_ER ;
		fal_utyerrno = FAL_ERR_MALLOC ;
		fal_utyerrno |= (FAL_FUNCNUM_IDTOFNM<<8) ;
		return FAL_ERROR ;
	}
	*file_name = fname ;
	return 0 ;
}


#if NeedFunctionPrototypes
int
FalFree(
	void	*list )
#else
int
FalFree( list )
void	*list ;
#endif
{
	if( list == NULL ) {
		fal_utyerror  = _FAL_PARM_ER ;
		fal_utyerrno = FAL_ERR_PARM ;
		fal_utyerrno |= (FAL_FUNCNUM_FREE<<8) ;
		return FAL_ERROR  ;
	}
	free( list ) ;
	return 0  ;
}



int
FalFreeGI( ginf, num )
FalGIInf	*ginf ;
int		num ;
{
	int	i ;
	if( ginf == NULL ) {
		fal_utyerror  = _FAL_PARM_ER ;
		fal_utyerrno = FAL_ERR_PARM ;
		return FAL_ERROR ;
	}
	for( i=0; i<num; i++ ) {
		if( ginf[i].charset_str != NULL )
			free( ginf[i].charset_str ) ;
	}
	free( ginf ) ;
	return 0  ;
}


/*
 *	This functions manage the relation of FontID 
 *	between code set number.
 */

static	int
fal_add_fidinf( fid, dspcode, cd_set, islock )
FalFontID	fid ;
int		dspcode ;
int		cd_set ;
int		islock ;
{
	int		exist, i, cnt ;
	FontIDInfo	**flist ;
	if( fid == NULL ) {
		fal_utyerror  = _FAL_PARM_ER ;
		fal_utyerrno = FAL_ERR_PARM ;
		return FAL_ERROR ;
	}
	exist = 0 ;
	for( i=0; i<mngfid.num; i++ ) {
		if( fid == mngfid.finf[i]->fid ) {
			exist++ ;
			mngfid.finf[i]->dspcode = dspcode ;
			mngfid.finf[i]->cd_set  = cd_set ;
			mngfid.finf[i]->islock  = islock ;
			break ;
		}
	}
	/* add new data */
	if( !exist ) {
	    if( mngfid.finf == NULL ) {
		cnt = 0 ;
		if( (flist = (FontIDInfo **)malloc( sizeof(FontIDInfo *) )) == NULL ) {
			fal_utyerror = _FAL_MALOC_ER ;
			fal_utyerrno = FAL_ERR_MALLOC ;
			return FAL_ERROR ;
		}
	    } else {
		cnt = mngfid.num ;
		if( (flist = (FontIDInfo **)realloc( 
			mngfid.finf, sizeof(FontIDInfo *) * (cnt + 1) )) == NULL )
		{
			fal_utyerror = _FAL_MALOC_ER ;
			fal_utyerrno = FAL_ERR_MALLOC ;
			return FAL_ERROR ;
		}
	    }
	    if( (flist[cnt] = (FontIDInfo *)malloc( sizeof(FontIDInfo) )) == NULL ) {
		fal_utyerror = _FAL_MALOC_ER ;
		fal_utyerrno = FAL_ERR_MALLOC ;
		return FAL_ERROR ;
	    }
	    flist[cnt]->fid 	= fid ;
	    flist[cnt]->cd_set 	= cd_set ;
	    flist[cnt]->dspcode 	= dspcode ;
	    flist[cnt]->islock	= islock ;

	    mngfid.num	= cnt + 1 ;
	    mngfid.finf	= flist ;
	}

	return 0 ;
}

static	int
fal_read_fidinf( fid, fontid_inf )
FalFontID	fid ;
FontIDInfo	*fontid_inf;
{
	int	i ;
	if( fid == NULL ) {
		fal_utyerror = _FAL_PARM_ER ;
		fal_utyerrno = FAL_ERR_PARM ;
		return FAL_ERROR ;
	}
	for( i=0; i<mngfid.num; i++ ) {
		if( mngfid.finf[i]->fid == fid ) {
			fontid_inf->fid     =  fid ;
			fontid_inf->cd_set  =  mngfid.finf[i]->cd_set ;
			fontid_inf->dspcode =  mngfid.finf[i]->dspcode ;
			fontid_inf->islock  =  mngfid.finf[i]->islock ;
			return 0 ;
		}
	}
	fal_utyerror = _FAL_PARM_ER ;
	fal_utyerrno = FAL_ERR_FID_RD ;
	return FAL_ERROR ;
}


static	int
fal_del_fidinf( fid )
FalFontID	fid ;
{
	int	i, cnt, target ;
	FontIDInfo	**flist ;

	for( i=0; i<mngfid.num; i++ ) {
		if( mngfid.finf[i]->fid == fid ) {
			target = i ;
			break ;
		}
	}
	cnt = mngfid.num - 1 ;
	free( mngfid.finf[target] ) ;
	for( i=target; i<mngfid.num; i++ ) {
		mngfid.finf[i] = mngfid.finf[i+1] ;
	}
	if( cnt > 0 ) {
		if( (flist = (FontIDInfo **)realloc( 
			mngfid.finf, sizeof(FontIDInfo *) * cnt )) == NULL )
		{
			fal_utyerror = _FAL_MALOC_ER ;
			fal_utyerrno = FAL_ERR_MALLOC ;
			return FAL_ERROR ;
		}
		mngfid.num = cnt ;
		mngfid.finf = flist ;
	} else {
		free( mngfid.finf ) ;
		mngfid.num = 0 ;
		mngfid.finf = NULL ;
	}
	return 0 ;
}

/*
 *	convert codepoint into glyph index
 */

static int
fal_conv_code_to_glyph( fid, code, glidx )
FalFontID	fid ;
unsigned int	code ;
unsigned int	*glidx ;
{
	Oak_FontInf	*finf ;
	FalFontData	tmp_data ;
	FalGIInf	*gi ;
	FontIDInfo	fontid_inf ;
	int		gnum ;
	int		i ;
	int		inner_code = 0 ;
	char		*char_set ;
	static FalFontID	cmpfid = 0 ;
	static	char	charset_name[FAL_LINE_MAX] ;

	finf = (Oak_FontInf *)fid ;
	/*
	 *	get code set number
	 */
	if( fal_read_fidinf( fid, &fontid_inf ) == FAL_ERROR ) {
		return	FAL_ERROR;
	}

	if( fontid_inf.dspcode != FAL_FONT_GLYPH_INDEX ){
		/*
		 *	get string of "CHARSET_REGISTRY"
	 	 */
		memset( &tmp_data, 0, sizeof(FalFontData) ) ;

		if( cmpfid != fid ){
		    if( falReadGpfProp( FAL_READ_FONTINFO, finf, FAL_FONT_PRM, &tmp_data ) ) {
			    fal_clear_data( &tmp_data ) ;
			    return	FAL_ERROR;
		    }
		    GETCHARSETSTR( char_set, tmp_data.xlfdname ) ;
		    strcpy( charset_name, char_set ) ;
		}

		/*
	 	*	convert codepoint into glyph index
	 	*/
		if( COMM_SBFNT_ISSBFNT( charset_name ) ) {
		    inner_code = code ;
		    FAL_OPT_CONVCPTOGI( fontid_inf.dspcode, inner_code ) ;
		} else {
		    if( fal_code_to_glyph( fal_locale, code, &gi, &gnum ) ) {
			fal_clear_data( &tmp_data ) ;
			return	FAL_ERROR;
		    }
		    if( gnum==1 ){
			inner_code = gi[0].glyph_index ;
		    }else{ 
			for( i=0; i<gnum; i++ ) {
			    if( !strcmp( gi[i].charset_str, char_set ) ) {
				inner_code = gi[i].glyph_index ;
				break ;
			    }
			}
		    } 	
		    FalFreeGI( gi, gnum ) ;
		}
		fal_clear_data( &tmp_data ) ;
	}else{
		inner_code = code;			/* glyph index */
	}
	*glidx = inner_code ;

	return 0 ;
}


/*
 *	convert codepoint into glyph index
 */

static int
fal_conv_glyph_to_code( finf, dspcode, cd_set, glidx, code )
Oak_FontInf	*finf ;
int		dspcode ;
int		cd_set ;
unsigned int	glidx ;
unsigned int	*code ;
{
	FalFontData	tmp_data ;
	int		inner_code ;
	char		*char_set ;

	/*
	 *	get code set number
	 */
	if( dspcode != FAL_FONT_GLYPH_INDEX ){
		/*
		 *	get string of "CHARSET_REGISTRY"
	 	 */
		memset( &tmp_data, 0, sizeof(tmp_data) ) ;
		if( falReadGpfProp( FAL_READ_FONTINFO, finf, FAL_FONT_PRM, &tmp_data ) ) {
			fal_clear_data( &tmp_data ) ;
			return	FAL_ERROR;
		}
		GETCHARSETSTR( char_set, tmp_data.xlfdname ) ;

		/*
	 	*	convert glyph index into codepoint 
	 	*/
		if( COMM_SBFNT_ISSBFNT( char_set ) ) {
		    inner_code = glidx ;
		    FAL_OPT_CONVGITOCP( dspcode, inner_code ) ;
		} else {
		    if( fal_glyph_to_code( fal_locale, char_set, cd_set, glidx, &inner_code ) ) {
			fal_clear_data( &tmp_data ) ;
			return	FAL_ERROR;
		    }
		}
		fal_clear_data( &tmp_data ) ;
	}else{
		inner_code = glidx ;		/* glyph index */
	}
	*code = inner_code ;

	return 0 ;
}


static	int 
file_lock( fd )
int 	fd;		/* a file descripter */
{
	struct flock	flpar;

	flpar.l_type = F_RDLCK;
	flpar.l_start = 0;
	flpar.l_len = 0;
	flpar.l_whence = 0;

	if ( fcntl( fd, F_SETLK, &flpar ) == -1 ) {
		fal_utyerrno = FAL_ERR_FILELCK ;
		return	FAL_ERROR ; 
	}

	return	0;
}

static	int 
file_unlock( fd )
int 	fd;	/* a file descripter */
{
	struct flock	flpar;

	flpar.l_type = F_UNLCK;
	flpar.l_start = 0;
	flpar.l_len = 0;
	flpar.l_whence = 0;

	if ( fcntl( fd, F_SETLK, &flpar ) == -1 ){
		fal_utyerrno = FAL_ERR_FILEUNLCK ;
		return	FAL_ERROR ; 
	}
	return	0;
}

static	int 
is_lock( fd )
int 	fd;	/* file descripter */
{
	struct flock	flpar;

	flpar.l_type = F_WRLCK;
	flpar.l_start = 0;
	flpar.l_len = 0;
	flpar.l_whence = 0;

	if ( fcntl( fd, F_GETLK, &flpar ) == -1 ) {
		fal_utyerrno = FAL_ERR_FILEGETLCK ;
		return	FAL_ERROR ; 
	}

	if ( flpar.l_type == F_UNLCK ){
		return	0 ;
	} else {
		fal_utyerrno = FAL_ERR_LCKD ;
		return	1 ;
	}
}

/********************< end of falfont.c >*********************************/
