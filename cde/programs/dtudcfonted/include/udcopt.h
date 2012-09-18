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
/* $XConsortium: udcopt.h /main/4 1996/11/08 01:59:43 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

/***********************************************************************
 * SNF format font
 **********************************************************************/
#define BDFTOSNF_CMD	"bdftosnf"
#ifndef SNFFONTC
#define BDFTOSNF	"/usr/bin/X11/bdftosnf"
#else
#define BDFTOSNF	SNFFONTC
#endif
#define SNFSUFFIX	".snf"

#define FONT_FILE_PARM	0

#define	COMM_SNF_POPEN( permission, snf_file, out_file, ep, buf, command ) { \
	strcpy( (buf), " -i " ) ; \
	sprintf( (command), "%s %s > %s", bdftosnf, (buf), (out_file) ); \
}
#define	COMM_SNF_FILEVERSION( snf_fd, finf, buf, permission ) { \
	read( (snf_fd), (buf), sizeof(FontInfoRec) ); \
	close( (snf_fd) ); \
	(finf) = (FontInfoRec *) (buf) ; \
	if( \
	    ((finf)->version1 != FONT_FILE_VERSION) || \
	    ((finf)->version2 != FONT_FILE_VERSION) \
	) \
	    (permission) = -1 ; \
}
#define COMM_SNF_NEWTARGET( permission )	(permission) = 0
#define	COMM_SNF_EXECLBDFTOSNF( permission, buf, snf_file ) { \
	execl( bdftosnf, bdftosnf, "-i", 0 ); \
}
#define	COMM_SNF_GETHEADER( fip, fp ) { \
	(fip) = (FontInfoRec *)(fp); \
}

#define	FAL_FONT_PRM	0
#define	CHK_PROTECT_KEY( protect_key_data, openfontfile,\
			fal_utyerror, fal_utyderror ) 
/* case of a snf former font */
#define	FAL_GET_SNF_HEADER( buf, protect_key_data, fal_utyerror, fal_utyderror, fd, finf, openfontfile ) { \
	(finf)->pFinf = ( FontInfoPtr )(buf); \
	if ( \
	    ((finf)->pFinf->version1 != (finf)->pFinf->version2) || \
	    ((finf)->pFinf->version1 != FONT_FILE_VERSION) \
	) { \
	    set_errfile_str( fal_err_file, (openfontfile) ) ; \
	    (fal_utyerror) = _FAL_FONT_ER; \
	    (fal_utyderror) = 0; \
	    fal_utyerrno = FAL_ERR_FONT ; \
	    fal_utyerrno |= (FAL_FUNCNUM_OPNFNT<<8) ; \
	    goto FalError01 ; \
	} else { \
	    (finf)->isFef = TRUE ; \
	    (finf)->isPcf = FALSE; \
	} \
}

/* falReadGpfProp() */
#define	FAL_READ_SNF_HEADER( finf, protect_key_data, fal_utyerror, fal_utyderror ) { \
    (finf)->isFef = TRUE; \
    (finf)->isPcf = FALSE; \
    (finf)->pFinf = ( FontInfoPtr )(finf)->buf ; \
    if ( /* not a snf or fef format */ \
	((finf)->pFinf->version1 != (finf)->pFinf->version2) || \
	((finf)->pFinf->version1 != FONT_FILE_VERSION) \
    ) { \
	return _FAL_TRY_NEXT ; \
    } \
} 

#define	ISOPTFONT( file )	1

/***********************************************************************
 * glyph index
 **********************************************************************/
#define	SHIFTMASK	0x7F7F
#define SHIFT_OFF(co)	(co)
#define SHIFT_ON(co)	(co)
#define DEFAULTCHAR	0x2121
#define CONVGLYPHINDEX( code )	(code)

#define DISPCODEPOINT( char_set, code ) (code)

#define	FAL_OPT_CONVGITOCP( codeset, code )
#define	FAL_OPT_CONVCPTOGI( codeset, code )

/***********************************************************************
 * options for commands
 **********************************************************************/
#define	NORMAL	0
#define	SYSTM	1

#define	CDSET1	(1<<2)

/* UDC code area */
#define IN_CODE( cc, co )        (( cc ) ? 1 : 1 )

/* libfuty */
#define	COMM_SBOPT_CHECKSBOPT( com, code_area )

#define	COMM_SBOPT_STRCMP( argv, cnt )	(((argv)[(cnt)] ? 0 : 0 ))
#define	COMM_SBOPT_SETSBOPT( codearea )

#define	COMM_TYPE_STRCMP( argv, cnt )	( ((argv)[(cnt)] ? 0 : 0 ))
#define	COMM_TYPE_SETTYPE( argc, argv, cnt, type )

/***********************************************************************
 * options for commands
 **********************************************************************/
#define	COMM_SETDEFAULTSTYLE( style )	(style) = (char *)NULL

#define	COMM_SETSTYLE_TO_FONTDATA( style, key, mask ) { \
	if( style ) { \
	    char	stylebuf[ BUFSIZE ] ; \
	    strcpy( stylebuf, style ) ; \
	    key.style.name = stylebuf ; \
	    mask |= FAL_FONT_MASK_STYLE_NAME ; \
	} \
}
#define COMM_ISDEFAULTCHAR( code )	0

#define	get_charset_registry(head, p)
#define	put_default_chars(head, nchar, rtn) {\
    if ( !(nchar) ) \
    {\
	(nchar)++;\
	fprintf( (head)->output, "CHARS %d\n", (nchar) );\
	if ( ( (rtn) = putDefaultChars( (head) ) ) != 0 ) {\
	    return (rtn);\
	}\
    } else {\
	fprintf( (head)->output, "CHARS %d\n", (nchar) );\
    }\
}
#define	put_default_encode( head ) {\
    fprintf((head)->output, "STARTCHAR %04x\n", DEFAULTCHAR );\
    fprintf((head)->output, "ENCODING %d\n", DEFAULTCHAR );\
}
#define	put_default_bitmap(head, bytew, posbit, i, j, ptnbuf, ptnbuf2, tmp) { \
    for ((i) = 0; (i) < (bytew); (i)++) \
	*((ptnbuf) + (i)) = (unsigned char)0x00;\
    for ((j) = 0; (j) < (head)->bdf_height; (j)++) {\
	for ((i) = 0; (i) < (bytew); (i)++) \
	    fprintf((head)->output, "%.2x", *((ptnbuf) + (i)));\
	fprintf((head)->output, "\n");\
    }\
}
#define	COMM_SBFNT_ISSBFNT( p )	0
#define	COMM_SBFNT_CONVCODETOGI( char_set, head )
#define	COMM_SBFNT_SETUDCAREA( com, head, char_set, code_area, num_gr, gr ) { \
	(head).code_category 	= ALL_CODE ; \
	(head).start_code 	= (gr)[0].start ; \
	(head).end_code 	= (gr)[(num_gr)-1].end ; \
}
#define	COMM_SET_CODESET( codeset, code_area )
#define	COMM_SET_CODECATEGORY( char_set, code_area, head )

/*
 * help message
 */
#define COMM_HELP_MSG { \
	USAGE("              [-codeset number] [-help]\n"); \
	USAGE("\n"); \
}
#define	COMM_DSP_LETTER_T
#define COMM_DSP_PRM
#define	COMM_HELP_MSG_STYLE	USAGE("style      (The default value is mincho style.)\n")

/***********************************************************************
 * Options for libfal
 **********************************************************************/
#define COMM_SBCHR_SETFONTDATA( data, mask )
#define COMM_SBCHR_STRDUP_P( data, odata )

#define FAL_DB_OPTION 	static	FalFontDB	*fal_db_opt
#define	CHK_KEY_STR_OPTION( mask, key, flg )
#define	FAL_FREE_FONTLIST_OPTION( list )
#define	FAL_CLEAR_DATA_OPTION( tmp )
#define	SET_STRUCT_OPTION( dup_p, elm, tmp_data, fal_utyerror, fal_db_group )
#define	FAL_EQ_DATA_OPTION( flg, data, tmp_data )
#define	FAL_CMP_DATA_OPTION( mask, op1, key )
#define	DEF_STR_CHK 	struct { int 	style, shape; }str_chk 
#define	DEF_STR_SET { \
	str_chk.style = 0; \
	str_chk.shape = 0; \
}

#define	NEW_TARGET_SET( diff, choose, target, str_chk )
#define	NEW_TARGET_CHK( diff, choose, target, str_chk )


/***********************************************************************
 * udflist.def
 **********************************************************************/
#define	COMM_GET_DEFAULTFILE_ERROR( buf, com, ret, height, style, list_d, list_c, list_l, statbuf ) {\
	buf[0] = '\0' ;\
}
#define	COMM_DISPERRORFILE( com, lst, style, i, size_str ) {\
	USAGE1("%s : Two or more corresponding user defined characters exist.\n", com);\
	USAGE("\tcode set\tXLFD\n" );\
	USAGE("\t--------------------------------------------------\n" );\
	for ( i = 0 ; i < lst->num; i++ ) {\
	    USAGE2("\t%d\t%s\n", lst->list[i].cd_set-1, lst->list[i].xlfdname );\
	}\
}

/***********************************************************************
 * fonts.list
 **********************************************************************/
#define GETLISTFILEPATH( locale, srch_path, list_d, list_c, list_l, list_file ) { \
	if ( ( (locale) = getenv( "FAL_LANG" ) ) == NULL ) { \
	    if( ((locale) = (char *)getenv( "LANG" )) == NULL ){\
		(locale) = "C" ;\
	    }\
	} \
	if ( ( (srch_path) = getenv( DTUDCFONTS ) ) == NULL ) { \
	    sprintf( (list_d), "%s/%s", DTUDCFONTSLISTDIR, (list_file) ); \
	    sprintf( (list_c), "%s/C/%s", DTUDCFONTSLISTDIR, (list_file) ); \
	    sprintf( (list_l), "%s/%s/%s", DTUDCFONTSLISTDIR, (locale), (list_file) ); \
	} else { \
	    sprintf( (list_d), "%s/%s", (srch_path), (list_file) ); \
	    sprintf( (list_c), "%s/C/%s", (srch_path), (list_file) ); \
	    sprintf( (list_l), "%s/%s/%s", (srch_path), (locale), (list_file) ); \
	} \
}

#define	SET_EXECDEFAULT( mask, def, undef ) { \
    if( !((mask) & FAL_FONT_MASK_DEFINED) && !((mask) & FAL_FONT_MASK_UNDEFINED) ){ \
	(def)	= FALSE ; \
	(undef)	= TRUE ; \
    }else{ \
	if( (mask) & FAL_FONT_MASK_DEFINED )	(def)	= TRUE ; \
	else					(def)	= FALSE ; \
	if( (mask) & FAL_FONT_MASK_UNDEFINED )	(undef)	= TRUE ; \
	else					(undef)	= FALSE ; \
    } \
}

/*************************< end of udcopt.h >**************************/
