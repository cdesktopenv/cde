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
/* $XConsortium: getfname.c /main/9 1996/11/08 02:06:43 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include  <sys/types.h>
#include  <sys/stat.h>
#include  <locale.h>
#include  <unistd.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <errno.h>

#include  "FaLib.h"
#include  "falfont.h"
#include  "udcutil.h"

#include  <string.h>

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
#include  <wait.h>
#endif	/* SVR4 */
#include  <signal.h>

#include  "bdfgpf.h"

static 	void	ErrMsgTable_GetDefaultFile();
static	int	falcom_split_data() ;
static	void	dsp_font_list() ;
static	int	search_Font_File_Name();

extern	int	searchFontFileName();
extern	int	falReadFontProp() ;

/* 
 * get "character size" and "letter size" from a character of an interface
 *
 * 	return code：
 * 		 1... get "character size"
 * 		 2... get "character size" and "letter size"
 * 		-1... NG
 */

#define	BODY_ONLY	1
#define	BODY_AND_LETTER	2

static int
GetSize( size_str, body, letter )
char	*size_str;	/* a character of a size */
int 	*body;		/* set "character size" (output) */
int 	*letter;	/* set "letter size" (output) */
{
	char	*str_dot;
	/* char	*str_end; */
	int 	bdy, ltr;

	str_dot = strchr( size_str, '.');
	/* str_end = strchr( size_str, '\0'); */

	if ( str_dot == NULL) {
		/* change a size of "character size"*/
		if (falcom_atoi( size_str, '\0', &bdy ) ==-1) {
			return	-1;
		}
		*body = bdy;
		return	BODY_ONLY;
	}

	/* change a size of "character size"*/
	if (falcom_atoi( size_str, '.', &bdy ) ==-1) {
		return	-1;
	}

	/* change a size of "letter size"*/
	str_dot++;
	if (falcom_atoi( str_dot, '\0', &ltr ) ==-1) {
		return	-1;
	}
	*body   = bdy;
	*letter = ltr;
	return	BODY_AND_LETTER;
}


/*
 *
 * change numerical character for numarical data
 *
 */

int
#if NeedFunctionPrototypes
falcom_atoi(
    char	*str,	/* numerical character */
    char	ed_code,/* an end code in numerical character */
    int 	*val	/* set numarical data */
)
#else
falcom_atoi(str, ed_code, val)
char	*str;	/* numerical character */
char	ed_code;/* an end code in numerical character */
int 	*val;	/* set numarical data */
#endif
{
	char	*ptr;
	char	*str_end;
	int 	tmp;

	/* get an end index */
	str_end = strchr( str, ed_code );

	/* change numarical data */
	tmp = (int)strtol(str, &ptr, 10);

	/* error check */
	if ((ptr == str) || (ptr != str_end)) {
		return(-1);
	}
	*val = tmp;
	return	0;
}


static int
falcom_cr_to_null(buf)
char	*buf;
{
	buf = strchr( buf, '\n');
	if (buf != NULL) {
		*buf = '\0';
	}
	return	0;
}



#define	FALCOM_DATA_ELM		3

#define	FALCOM_ELM_SIZE		0
#define	FALCOM_ELM_STYLE	1
#define	FALCOM_ELM_FNAME	2


int
#if NeedFunctionPrototypes
GetDefaultFile( 
    int		size,
    char	*style,
    char	*fname
)
#else
GetDefaultFile( size, style, fname )
int	size;		/* character size */
char	*style;		/* character style */
char	*fname;		/* buffer */
#endif
{
	FILE	*fp;
	int 	ret;


	char	*locale;
	char	*fal_search_path;
	char	uflist_d[BUFSIZE];
	char	uflist_c[ BUFSIZE ];
	char	uflist_l[ BUFSIZE ];

	GETLISTFILEPATH( locale, fal_search_path, uflist_d, uflist_c, uflist_l, UDFLIST_DEF ) ;

	if ( ( fp = fopen( uflist_l, "r" ) ) == NULL ) {
	    if ( ( fp = fopen( uflist_c, "r" ) ) == NULL ) {
		if ( ( fp = fopen( uflist_d, "r" ) ) == NULL ) {
		    return  FAL_ERROR;
		}
	    }
	}

	ret = get_default_file( fp, size, style, fname );
	fclose( fp );
	return	ret;
}


int
#if NeedFunctionPrototypes
get_default_file( 
    FILE	*fp,
    int		size,
    char	*style,	
    char	*fname	
)
#else
get_default_file( fp, size, style, fname )
FILE	*fp;
int	size;		/* character size */
char	*style;		/* character style */
char	*fname;		/* buffer */
#endif
{
	char	buf[BUFSIZE];		/* buffer */
	int 	size_tmp;		/* size (read file) */
	int 	ret;
	char	*elm[FALCOM_DATA_ELM];

	ret = 0;

	/* read from a file */
	while( fgets(buf, BUFSIZE, fp) != (char *)NULL ) {

		falcom_cr_to_null( buf );
		if ((buf[0] == '#') || (buf[0] == '\0')) {
			continue;
		}
		if (falcom_split_data( buf, FALCOM_DATA_ELM, &elm ) == -1) {
			ret = -4;
			continue;
		}
		if (falcom_atoi( elm[FALCOM_ELM_SIZE], '\0', &size_tmp ) == -1) {
			ret = -4;
			continue;
		}
		if ( style ) {
		    if ((size_tmp == size) && !strcmp(elm[FALCOM_ELM_STYLE], style)) {
			strcpy( fname, elm[FALCOM_ELM_FNAME] );
			fclose( fp );
			return	0;
		    }
		}
	}
	if (ret != 0) {
		return	ret;
	}

	if ( feof(fp) != 0 ) {
		return	-2;
	}
	return	-3;
}

/**************************************************************/
/* read from a file and divide a data that read from file     */
/**************************************************************/

static int
falcom_split_data( buf, entry_num, elm )
char	*buf;		/* read a font information character array from a file */
int 	entry_num;
char	*elm[];
{
	int 	cnt;		/* counter */
	int 	strtop_flg;	/* flag of a head of a character array */

	/* divide a data */
	strtop_flg = 1;
	for (cnt=0  ; *buf != '\0' && cnt < entry_num  ;  buf++){
		/* skip a separate character */
		if ((*buf == '\t') || (*buf == ' ')) {
			/* change NULL */
			*buf = '\0';
			/* skip a separate character */
			do {
				buf++;
			} while((*buf == '\t') || (*buf == ' '));
			strtop_flg = 1;
		}
		if (strtop_flg) {
			elm[cnt] = buf;
			cnt++;
			strtop_flg = 0;
		}
	}
	/* check a data's number */
	if (cnt != entry_num ) {
		return	-1;
	}
	return	0;
}


char *
#if NeedFunctionPrototypes
falcom_get_base_name( char	*str )	
#else
falcom_get_base_name( str )
char	*str;	/* a full path character of a file name */
#endif
{
	char	*str_slash;

	str_slash = strrchr( str, '/');
	if ( str_slash == NULL ) {
		return	str;
	}
	return	++str_slash;
}


static int
is_letter_size_not_only_one( lst )
FalFontDataList	*lst;	/* a font list */
{
	int	i, letter_cmp;

	letter_cmp = lst->list[0].letter.h;
	for (i=0 ; i < lst->num ; i++) {
		if (lst->list[i].letter.h != letter_cmp){
			return	1;
		}
	}
	return	0;
}


/* 
 * get a real file name from a character of "size" and "style"
 *
 * return code：
 * 		 0 ... OK
 * 		-1 ... NG
 *
 */
int
#if NeedFunctionPrototypes
GetFileName(
    char	*com,
    char	*size_str,
    char	*style,	
    int 	codeset,
    char	*ofile	
)
#else
GetFileName(com, size_str, style, codeset, ofile)
char	*com;		/* a character of a command name(case error) */
char	*size_str;	/* a character of "size" */
char	*style;		/* a character of "style" */
int 	codeset;	/* a codeset */
char	*ofile;		/* set a target file name (output) */
#endif
{
	int 	body, letter;
	int 	size_sw;		/* a size flag */
	char	buf[BUFSIZE] ;		/* an area for a file name */
	char	*filename;		/* a pointer for a file name */
	char	fname[BUFSIZE];		/* a pointer for a file name */
	FalFontDataList	*lst;		/* a font list */
	FalFontData 	key;		/* search an information of a font */
	unsigned int 	mask;		/* search a mask of a font */
	int 	i, ret;
	struct	stat	statbuf ;
	char	*locale;

	char	*fal_search_path;
	char	uflist_d[ BUFSIZE ];
	char	uflist_c[ BUFSIZE ];
	char	uflist_l[ BUFSIZE ];

	GETLISTFILEPATH( locale, fal_search_path, uflist_d, uflist_c, uflist_l, UDFLIST_DEF ) ;

	/* change a character of a size */
	if ((size_sw = GetSize(size_str, &body, &letter)) == -1) {
		USAGE2("%s : The specification of the character size is improper.\"%s\"\n",
			com, size_str);
		return	-1;
	}

	mask = 0;
	mask |= FAL_FONT_MASK_UNDEFINED ;
	mask |= FAL_FONT_MASK_DEFINED ;
	memset( &key, '\0', sizeof(FalFontData) ) ;
	/* set information to search fonts */

	key.cd_set = FALGETFALCODESET( codeset ) ;
	mask |= FAL_FONT_MASK_CODE_SET ;

	switch( size_sw ) {
	case BODY_AND_LETTER:
		key.letter.h  = letter;		
		mask |= FAL_FONT_MASK_LETTER_H;
	case BODY_ONLY:
		key.size.h    = body;		
		mask |= FAL_FONT_MASK_SIZE_H;
	}

	/* aet and check an information of a style */
	COMM_SETSTYLE_TO_FONTDATA( style, key, mask ) ;	

	/* get a font information */
	if (FalGetFontList( &key, mask, &lst ) == FAL_ERROR ) {
		ErrMsgTable_FalGetFontList( com, fal_utyerror, fal_utyderror );
		return	-1;
	}

	if ( lst->num == 0 ) {	
		USAGE3("%s : The user defined character of the specified size and style does not exist.\"%s , %s\"\n",
			com, size_str, (style)?style:" ");
		FalFreeFontList( lst );
		return	-1;
	}

	memset( fname, '\0', sizeof(fname) ) ;
	switch( size_sw ) {
	case BODY_ONLY:

	    if ( is_letter_size_not_only_one(lst) ) {
		ret = GetDefaultFile( key.size.h, style, buf) ;	
		if ( ret < 0 ) {
		    COMM_GET_DEFAULTFILE_ERROR( buf, com, ret, key.size.h, style, uflist_d, uflist_c, uflist_l, statbuf ) ;
		}
		for ( i = 0 ; i < lst->num; i++ ) {
		    ret = search_Font_File_Name( lst->list[i], fname );
		    if ( ret == FAL_ERROR ) {
			FalFreeFontList( lst );
			return	-1;
		    }
		    filename = falcom_get_base_name( fname );
		    if ( (buf[0] != '\0' && fname[0] != '\0')
			&& !strcmp( buf, filename ) ) {
			strcpy( ofile, fname );
			FalFreeFontList( lst );
			return	0;
		    }
		}
		COMM_DISPERRORFILE( com, lst, style, i, size_str ) ;
		FalFreeFontList( lst );
		return	-1;
	    }

	case BODY_AND_LETTER:
		switch( lst->num ) {
		case 1:
			ret = search_Font_File_Name( lst->list[0], fname );
			if ( ret == FAL_ERROR ) {
				FalFreeFontList( lst );
				return  -1;
			}
			strcpy( ofile, fname );
			FalFreeFontList( lst );
			return	0;
		default:
			ret = GetDefaultFile( key.size.h, style, buf);
			if ( ret < 0 ) {
				buf[0]='\0';
			}
			for( i = 0 ; i < lst->num; i++ ) {
			    ret = search_Font_File_Name( lst->list[0], fname );
			    if ( ret == FAL_ERROR ) {
				FalFreeFontList( lst );
				return  -1;
			    }
			    filename = falcom_get_base_name( fname );
			    if ( (buf[0] != '\0' && fname[0] != '\0')
				&& !strcmp( buf, filename ) ) {
				strcpy( ofile, fname );
				FalFreeFontList( lst );
				return	0;
			    }
			}
			USAGE1("%s : Two or more corresponding user defined characters exist.\n", com);
			for ( i = 0 ; i < lst->num; i++ ) {
			    ret = search_Font_File_Name( lst->list[i], fname );
			    if ( ret == FAL_ERROR ) {
				FalFreeFontList( lst );
				return  -1;
			    }
			    fprintf( stdout, "\t%s\n", fname );
			}
			FalFreeFontList( lst );
			return	-1;
		}		/* <--- switch( lst->num ) { */
	}			/* <--- switch( size_sw ) { */
	return -1 ;
}


/* take out an error message of "FalGetFontList()" */

/*
 * Error messages(by origin)
 */
char	*fal_errmsg_org[0xff] = {
/* 0x00 */
    " ",
    "Cannot open the font file.",
    "Cannot read the font file.",
    "There is no more memory .",
    "Fatal error occurred.",
    "The specified font file does not exist.",
    "This font is not a pcf or snf format.",
    "Cannot open fonts.list file.",
    "The format of fonts.list file is illegal.",
    "The descriptions of the fonts.list file are incorrect.",
    "The format of fonts.list file is illegal.",
    "Cannot open fonts.dir file.",
    "Cannot read fonts.dir file.",
    "Cannot read font properties.",
    "Cannot get \"FONT\" property.",
    "Cannot get \"FAMILY_NAME\" property.",
/* 0x10 */
    "This font file is already opened by other application.",
    "Cannot lock font file.",
    "Cannot unlock font file.",
    "Cannot get lock information from the font file.",
    "Cannot find the specified font file.",
    "Cannot read NLS database.",
    "Cannot get charset names from NLS database.",
    "Charset name not defined in NLS database.",
    "The specified font has not been opened.",
    "Fatal error occurred.",
    " "," "," "," "," "," ",
/* 0x20 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ", 
/* 0x30 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ", 
/* 0x40 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0x50 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0x60 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0x70 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0x80 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0x90 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0xa0 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0xb0 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0xc0 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0xd0 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0xe0 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ",
/* 0xf0 */
    " "," "," "," "," "," "," "," "," "," "," "," "," "," "," "
} ;


void
#if NeedFunctionPrototypes
ErrMsgTable_FalGetFontList(
    char	*com,
    int		utyerr,
    int		utyderr
)
#else
ErrMsgTable_FalGetFontList( com, utyerr, utyderr )
char	*com;		/* command name */
int	utyerr;		/* fal_utyerror */
int	utyderr;	/* fal_utyderror */
#endif
{
	char	*fontsdir, *locale ;

	if( (locale = (char *)getenv( "LANG" )) == NULL ){
	    locale = "C" ;
	}

	if( (fontsdir = (char *)getenv( "DTUDCFONTS" )) == NULL ) {
	    fontsdir = DTUDCFONTSLISTDIR ;
	}
	switch( utyerr ) {
	case 1:
		if( fal_utyerrno ) {
		    USAGE2("%s : %s\n", com, fal_errmsg_org[ fal_utyerrno & 0xff ] ) ;
		    return ;
		}
		USAGE4("%s : The font directory information file(%s/%s/%s) cannot be opened.\n",
			com, fontsdir, locale, FONTS_LIST_FILE ) ;
		break;
	case 2:
		USAGE4("%s : Information cannot be read from the font directory information file(%s/%s/%s).\n", com, fontsdir, locale, FONTS_LIST_FILE );
		break;
	case 3:
		USAGE1("%s : The work area cannot be secured.\n", com );
		break;
	case 7:
		USAGE4("%s :  Abnormality is found in the content of the font directory information file((%s/%s/%s).\n",
			com , fontsdir, locale, FONTS_LIST_FILE);
		break;
	case 8:
		USAGE4("%s : The font directory information file(%s/%s/%s) cannot be opened.\n",
			com , fontsdir, locale, FONTS_LIST_FILE);
		break;
	case 9:
		USAGE4("%s : Information cannot be read from the font directory information file(%s/%s/%s).\n",
			com , fontsdir, locale, FONTS_LIST_FILE);
	default:
		if( fal_utyerrno ) {
		    USAGE2("%s : %s\n", com, fal_errmsg_org[ fal_utyerrno & 0xff ] ) ;
		    return ;
		}
		USAGE1("%s : Terminates abnormally.\n", com );
	}
}



/* take out an error message of "GetDefaultFile()" */

static void
ErrMsgTable_GetDefaultFile( com, ret, size, style, fname )
char	*com;			/* a command name */
int 	ret;			/* return code */
int 	size;			/* a character size */
char	*style;			/* a character style */
char	*fname;			/* user defined character information file name */
{
	switch (ret) {
	case -1:
		USAGE2("%s : The user defined character information file cannot be opened. \"%s \"\n",
			com, fname);
		break;
	case -2:
		USAGE4("%s : The representative user defined character of the character size (%d) and style (%s) is not defined.\"%s\"\n",
			com, size, (style)?style:" ", fname);
		break;
	case -3:
		USAGE2("%s : Information cannot be read from the user defined character information file.\"%s\"\n", com, fname);
		break;
	case -4:
		USAGE2("%s : The mistake is found in the description of the user defined character information file. \"%s\"\n", com, fname);
		break;
	}
}


/* 
 * if an end character of a file name is "/",it is cleared
 *
 * return code：none
 *      none
 */

void
#if NeedFunctionPrototypes
fal_cut_tailslash( char *name )
#else
fal_cut_tailslash( name )
char *name;
#endif
{
	char *p;

	p = strrchr( name, '\0');
	if (p == NULL) {
		return;
	}
	for (p-- ; (p>=name) && (*p == '/') ; p--) {
		*p = '\0';
	}
}


/* 
 * get a real file name from a link file name
 *
 * return code：
 * 	OK　．．a pointer of a file name
 * 	NG　．．NULL
 *
 */

char	*
#if NeedFunctionPrototypes
GetRealFileName( char	*name )
#else
GetRealFileName( name )
char	*name;
#endif
{
	static char	*ret_name ;
	char	link_name[BUFSIZE];
	int		len;

	struct stat st;

	AllocString( ret_name, name, NULL ) ;
	/* strcpy( ret_name, name ) ; */
	fal_cut_tailslash( ret_name ); /* for example /tmp/aa/ --> /tmp/aa  */

	while ( 1 ) {
		if ( lstat( ret_name, &st ) == -1) {
			FreeString( ret_name ) ;
			return	NULL;
		}
		errno = 0;
		if ( ( len = readlink( ret_name, link_name, BUFSIZE ) ) == -1 ) {
			if ( errno == EINVAL ) {
				return( ret_name ) ;
				/* return( strdup( ret_name ) ) ; */
			} else {
		    		FreeString( ret_name ) ;
				return	NULL;
			}
		}
		link_name[len] = '\0';
		if ( link_name[0] == '/' ) {
			/* strcpy( ret_name, link_name ) ; */
		    	FreeString( ret_name ) ;
			AllocString( ret_name, link_name, NULL ) ;
		} else {
			char	*slash;

			slash = strrchr( ret_name, '/' );
			if (slash != NULL) {
				*slash = '\0';
				fal_cut_tailslash( ret_name );
				ResizeString( ret_name, NULL );
				AddString(ret_name, "/", NULL);
			} else {
				/* ret_name[0] = '\0' ; */
				FreeString( ret_name ) ;
				AllocString(ret_name, "/", NULL);
			}
			/* strcat( ret_name, "/" ) ;
			strcat( ret_name, link_name ) ; */
			AddString( ret_name, link_name, NULL ) ;
		}
	}
}



/*
 *	funtctions for search fonts by XLFD and NLS database
 */

int
#if NeedFunctionPrototypes
IsInRegion(
    int			code ,
    int			num_gr ,
    FalGlyphRegion	*gr
)
#else
IsInRegion(code, num_gr, gr)
int		code ;
int		num_gr ;
FalGlyphRegion	*gr ;
#endif
{
	int	i ;
	if( code < MIN_CODE || code > MAX_CODE )	return -1 ;
	for( i=0; i<num_gr; i++ ) {
	    if( gr[i].start < MIN_CODE || gr[i].end > MAX_CODE )	continue ;
	    if( gr[i].start <= code && gr[i].end >= code )
		return 0 ;
	}
	return -1 ;
}




static	int
search_Font_File_Name( data, fname )
FalFontData	data ;
char		*fname ;
{
	FalFontID	fid ;
	FalFontDataList	*flist ;
	int		mask ;
	char		*tmp_fname ;

	mask = FAL_FONT_MASK_UNDEFINED | FAL_FONT_MASK_DEFINED ;

	if( data.xlfdname )	mask |= FAL_FONT_MASK_XLFDNAME ; 
	if( data.size.h )	mask |= FAL_FONT_MASK_SIZE_H ;
	if( data.size.w >= 0 )	mask |= FAL_FONT_MASK_SIZE_W ;
	if( data.style.name )	mask |= FAL_FONT_MASK_STYLE_NAME ;
	if( data.cd_set )	mask |= FAL_FONT_MASK_CODE_SET ;

	if( data.letter.w )	mask |= FAL_FONT_MASK_LETTER_W ;
	if( data.letter.h )	mask |= FAL_FONT_MASK_LETTER_H ;
	if( data.letter.x )	mask |= FAL_FONT_MASK_LETTER_X ;
	if( data.letter.y )	mask |= FAL_FONT_MASK_LETTER_Y ;

	if( data.style.def )	mask |= FAL_FONT_MASK_STYLE_DEF ;
	if( data.shape.name )	mask |= FAL_FONT_MASK_SHAPE_NAME ;
	if( data.shape.def )	mask |= FAL_FONT_MASK_SHAPE_DEF ;
	COMM_SBCHR_SETFONTDATA( data, mask ) ;
	if( data.prm )		mask |= FAL_FONT_MASK_PERMISSION ;

	fid = FalOpenSysFont( &data, mask, &flist ) ;
	if( fid == 0 ) {
	    memset( fname, '\0', sizeof(fname) ) ;
	    if( flist ){
		dsp_font_list( flist ) ;
		FalFreeFontList( flist ) ;
		return(-1);
	    } else {
		switch( fal_utyerror ) {
		case _FAL_OPEN_ER :
		case _FAL_READ_ER :
		case _FAL_STAT_ER :
		    switch( fal_utyderror ) {
		    case EACCES :
		    case ENOENT :
			return(0) ;
		    default :
			USAGE3("There is no font file correspond to specified font.\n(%s)\n fal_utyerror = %d fal_utyderror = %d\n",
			((data.xlfdname)?data.xlfdname:"null font name"), fal_utyerror, fal_utyderror ) ;
			return(-2) ;
		    }
		case _FAL_MALOC_ER :
		    USAGE("System call error occured.\n" ) ;
		    return(-2) ;
		default :
		    USAGE3("There is no font file correspond to specified font.\n(%s)\n fal_utyerror = %d fal_utyderror = %d\n",
		    ((data.xlfdname)?data.xlfdname:"null font name"), fal_utyerror, fal_utyderror ) ;
			return(-2) ;
		}
	    }
	} 
	if( FalFontIDToFileName( fid, &tmp_fname ) == FAL_ERROR ) {
	    USAGE3("There is no font file correspond to specified font.\n(%s)\n fal_utyerror = %d fal_utyderror = %d\n",
	    ((data.xlfdname)?data.xlfdname:"null font name"), fal_utyerror, fal_utyderror ) ;
	    FalCloseFont( fid ) ;
	    return(-1) ;
	}
	strcpy( fname, tmp_fname ) ;
	FalFree( tmp_fname ) ;
	FalCloseFont( fid ) ;

	return(0) ;
}


int
#if NeedFunctionPrototypes
GetUdcFileName(
    char	*com ,
    int 	code_no ,
    char	*xlfdname ,
    char	*fname
)
#else
GetUdcFileName( com, code_no, xlfdname, fname )
char	*com ;
int	code_no ;
char	*xlfdname ;
char	*fname ;
#endif
{
	FalFontData	data ;

	if( !xlfdname || !fname ) {
	    USAGE1("%s: null parameter\n", com ) ;
	    return(-1) ;
	}
	memset( &data, '\0', sizeof(FalFontData) ) ;
	data.size.w = -1 ;
	data.xlfdname = xlfdname ;
	data.cd_set = FALGETFALCODESET( code_no ) ;
	return( search_Font_File_Name( data, fname ) ) ;
}


static	void
dsp_font_list( flist )
FalFontDataList	*flist ;
{
	int	i ;

	USAGE(" code set / size / style / xlfd\n" ) ; 
	USAGE("-------------------------------------------------------\n" ) ; 
	for( i=0; i<flist->num; i++ ) {
	    USAGE4("\t%d\t%d\t%s\t%s\n", 
		    FALCODESETTONUM( flist->list[i].cd_set ),
		    flist->list[i].size.h,
		    flist->list[i].style.name,
		    flist->list[i].xlfdname
		) ;
	}
}


int
#if NeedFunctionPrototypes
GetUdcRegion( 
    char	*com ,
    int		codeset ,
    char	*gpf_file ,
    int		*num_gr ,
    FalGlyphRegion	**gr
)
#else
GetUdcRegion( com, codeset, gpf_file, num_gr, gr )
char		*com ;
int		codeset ;
char		*gpf_file ;
int		*num_gr ;
FalGlyphRegion	**gr ;
#endif
{
	FalFontData	fdata ;
	char	*locale, *char_set, *tmp_gpf ;	

	if( (locale = (char *)getenv( "LANG" )) == NULL ){
	    locale = "C" ;
	}
	/* parameter check */
	if( gpf_file == NULL || gr == NULL ) {
		USAGE1("%s : null parameters.\n", com);
		return( -1 ) ;
	}

	/* get font properties */
	tmp_gpf = (char *)GetRealFileName( gpf_file ) ;
	if( tmp_gpf == NULL ){
		return -1 ;
	}
	if( falReadFontProp( tmp_gpf, FAL_FONT_PRM, &fdata, 1 ) ) {
		USAGE1("%s : Failed to get font properties.\n", com ) ;
		return -1 ;
	}

	GETCHARSETSTR( char_set, fdata.xlfdname ) ;
	if( FalGetUDCGIArea( locale, FALGETFALCODESET(codeset), char_set, gr, num_gr ) == FAL_ERROR )
	{
		USAGE3("%s : Failed to get UDC code region. ( code set %d charset %s )\n",
				com, codeset, char_set ) ;
		return( -1 );	
	}

	if( *num_gr == 0 ) {
		USAGE3("%s : Failed to get UDC code region. ( code set %d charset %s )\n",
				com, codeset, char_set ) ;
		return -1 ;
	}

	return(0) ;
}



#define	MAX_CODESET	8

int
#if NeedFunctionPrototypes
DispUdcCpArea( FILE	*fp )
#else
DispUdcCpArea( fp )
FILE	*fp ;
#endif
{
	int	cd_set, j ;
	FalFontDataList	*fls ;
	FalFontData	fdata ;
	int		mask ;
	FalCodeRegion	*cr ;
	int		num_cr ;
	char		*locale ;

	if( (locale = (char *)getenv( "LANG" )) == NULL ){
	    locale = "C" ;
	}
	memset( &fdata, 0, sizeof(FalFontData) ) ;
	mask = 0 ;
	mask =  FAL_FONT_MASK_DEFINED | FAL_FONT_MASK_UNDEFINED |
		FAL_FONT_MASK_CODE_SET ;

	for( cd_set=0; cd_set<MAX_CODESET; cd_set++ ){
		char	*char_set ;
		fdata.cd_set = FALGETFALCODESET(cd_set) ;
		if( FalGetFontList(&fdata, mask, &fls) == FAL_ERROR ){
		    return -1 ;
		}
		if( fls->num == 0 ){
		    continue ;
		}
		GETCHARSETSTR( char_set, fls->list[0].xlfdname ) ;
		if( FalGetUDCCPArea( locale,
				fls->list[0].cd_set, char_set, &cr, &num_cr ) == FAL_ERROR ){
		    return -1 ;
		}
		if( num_cr == 0 ){
		    continue ;
		}
		/* display UDC code region */
		USAGE1("  %d", cd_set ) ;
		for( j=0; j<num_cr; j++ ){
		    USAGE2("\t\t0x%x - 0x%x\n", cr[j].start, cr[j].end ) ;
		}
		if( FalFreeFontList( fls ) == FAL_ERROR ){
		    return -1 ;
		}
	}
	return 0 ;
}



int	
#if NeedFunctionPrototypes
GetUdcFontName( 
    char	*gpf_file ,
    char	*bdf_file ,
    char	**fontname
)
#else
GetUdcFontName( gpf_file, bdf_file, fontname )
char	*gpf_file ;
char	*bdf_file ;
char	**fontname ;
#endif
{
	FILE	*fp ;
	pid_t	chld_pid = 0;
#if defined( SVR4 ) || defined( SYSV ) || defined(CSRG_BASED)
	int	chld_stat ;
#else
	union	wait	chld_stat ;
#endif
	struct	ptobhead head ;
	char	*p, *tmp_font ;
	char	readbuf[BUFSIZE], fntbuf[BUFSIZE] ;
	unsigned int	getstat;
	int	fd[2];

	p = tmp_font = NULL ;
	/*
	 * get temorary file
	 */
	memset( &head, 0, sizeof(struct ptobhead) ) ;
	if( gpf_file ){
	    head.snf_file = GetRealFileName( gpf_file ) ;
	    if( head.snf_file == NULL )	return STAT_ERROR ;
	}else if( bdf_file ){
	    head.bdf_file = GetRealFileName( bdf_file ) ;
	    if( head.bdf_file == NULL )	return STAT_ERROR ;
	}else{
	    return STAT_ERROR ;
	}

	/*
	 * write BDF header into FIFO
	 */
	if (head.snf_file != NULL) {
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
		execl( oakgtobdf, oakgtobdf, head.snf_file, "-H", 0);
		USAGE1("Cannot execute %s\n", oakgtobdf ) ;
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
	    if ((fp = fopen(head.bdf_file, "r")) == NULL) {
		return(BDF_OPEN_HEAD);
	    }
	}

	/*
	 * read BDF header
	 */
	getstat = 0 ;
	while ( 1 ) {
	    if (fgets(readbuf, BUFSIZE, fp) == NULL) {
		fclose( fp );
		if (head.snf_file != NULL) {
		    kill( chld_pid, SIGKILL );
		    WaitID( chld_pid, chld_stat ) ;
		}
		return(BDF_INVAL);
	    }
	    p = readbuf;
	    SCAN_TO_NONSP(p);

	    if (!strncmp( p, "ENDPROPERTIES", strlen("ENDPROPERTIES") )) {
		fclose( fp );
		if (head.snf_file != NULL) {
		    kill( chld_pid, SIGKILL );
		    WaitID( chld_pid, chld_stat ) ;
		}
		return(BDF_INVAL);
	    }
	    if (!strncmp( p, "FONT ", strlen("FONT ") )) {
		char	*sp, *ep ;
		sp = ep = p ;
		SCAN_TO_NONSP(sp);
		sp++ ;
		while( *ep != '\n' ){
		    ep++ ;
		}
		*ep = '\0' ;
		strcpy(fntbuf, sp) ;
		getstat |= 0x01 ;
		fclose( fp );
		if (head.snf_file != NULL) {
		    kill( chld_pid, SIGKILL );
		    WaitID( chld_pid, chld_stat ) ;
		}
		break;
	    }
	}

	if( getstat != 0x01 ){
		return BDF_INVAL ;
	}

	/*
	 * dup font name
	 */
	if( (tmp_font = (char *)strdup( fntbuf )) == NULL ){
	    return MALLOC_ERROR ;
	}
	*fontname = tmp_font ;

	return 0 ;
}


