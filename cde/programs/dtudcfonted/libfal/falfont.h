/* $XConsortium: falfont.h /main/5 1996/06/25 20:05:39 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#define GETNUMCHARS(pFinf) \
	(((pFinf)->lastCol - (pFinf)->firstCol + 1) * \
	 ((pFinf)->lastRow - (pFinf)->firstRow + 1))

#define GETHIGHT(pCinf) \
        ((pCinf)->metrics.ascent + (pCinf)->metrics.descent)

#define _FAL_OK		 1
/* fal_utyerror */
#define _FAL_OPEN_ER		1
#define	_FAL_DIR_NOACCSS	1
#define _FAL_READ_ER		2
#define _FAL_READ_PATH_ER	2
#define _FAL_MALOC_ER		3
#define _FAL_PKEY_ER		4
#define _FAL_PARM_ER		5
#define _FAL_STAT_ER		6
#define _FAL_FONT_ER		7
#define _FAL_FLST_ER		7
#define _FAL_DATA_OPEN_ER	8
#define _FAL_DATA_READ_ER	9

/* fal_utyderror */
/* FalOpenFont */
#define _FAL_O_P_PKEY_DER 1
#define _FAL_O_P_CS_DER   2
/* FalQueryFont */
#define _FAL_Q_P_FINF_DER 1
#define _FAL_Q_P_FONT_DER 2
/* FalReadFont */
#define _FAL_R_P_FINF_DER 1
#define _FAL_R_P_W_DER    2
#define _FAL_R_P_H_DER    3

/* FalGetFontList */
#define	_FAL_L_P_LST_DER  1
#define	_FAL_L_P_KEY_DER  2

#define	_FAL_TRY_NEXT	0xFF

#include  "udccom.h"

#define SNF_BOUND	32


#define	FILE_SUFFIX( file )	\
	( strrchr( file, '.' ) ? strrchr( file, '.' ) : "" )


#define	FAL_DATA_ELM	13
#define FAL_LINE_MAX	1024

/* update _Oak_Finf */
#define FAL_UPDATE_FONTINFO	1
#define FAL_READ_FONTINFO	0

/* Buffer of file name */
extern  char    fal_err_file_buf[] ;

