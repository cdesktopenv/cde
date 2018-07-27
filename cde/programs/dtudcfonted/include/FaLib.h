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
/* $XConsortium: FaLib.h /main/5 1996/06/25 20:08:31 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#ifndef _FALIB_H
#define _FALIB_H

#ifdef __cplusplus
extern "C" {
#endif

#define FAL_ERROR		-1

extern int fal_utyerror;	/* library error information */
extern int fal_utyderror;	/* library error information */
extern int fal_utyexists;	/* existence's flag of font pattern */

extern	unsigned long fal_utyerrno ;	/* library error information */
extern	char	fal_err_file[] ;

/*
 *
 * structure of the font information
 *
 */

/*
 * the equipment of output permission
 */
#define FAL_FONT_DISPLAY	1
#define FAL_FONT_PRINTER	2


#define FAL_FONT_GLYPH_INDEX	-1
/*
 * codeset
 */
#define FAL_FONT_CS0		1
#define FAL_FONT_CS1		2
#define FAL_FONT_CS2		3
#define FAL_FONT_CS3		4

#define FAL_FONT_CS4		5
#define FAL_FONT_CS5		6
#define FAL_FONT_CS6		7
#define FAL_FONT_CS7		8
#define FAL_FONT_CS8		9

/*
 * font search mask
 */
#define	FAL_FONT_MASK_XLFDNAME		(1L<<1)
#define	FAL_FONT_MASK_SIZE_W		(1L<<2)
#define	FAL_FONT_MASK_SIZE_H		(1L<<3)
#define	FAL_FONT_MASK_LETTER_W		(1L<<4)
#define	FAL_FONT_MASK_LETTER_H		(1L<<5)
#define	FAL_FONT_MASK_LETTER_X		(1L<<6)
#define	FAL_FONT_MASK_LETTER_Y		(1L<<7)
#define	FAL_FONT_MASK_STYLE_DEF		(1L<<8)
#define	FAL_FONT_MASK_STYLE_NAME	(1L<<9)
#define	FAL_FONT_MASK_SHAPE_DEF		(1L<<10)
#define	FAL_FONT_MASK_SHAPE_NAME	(1L<<11)


#define	FAL_FONT_MASK_CODE_SET		(1L<<14)
#define	FAL_FONT_MASK_PERMISSION	(1L<<15)
#define	FAL_FONT_MASK_DEL_EQUALDATA	(1L<<16)
#define	FAL_FONT_MASK_CHECK_ALL		0x1FFFF

#define	FAL_FONT_MASK_GLYPH_INDEX	(1L<<17)
#define	FAL_FONT_MASK_UPDATE		(1L<<18)
#define	FAL_FONT_MASK_DEFINED		(1L<<19)
#define	FAL_FONT_MASK_UNDEFINED		(1L<<20)


/*
 * undefined the character style,shape
 */
#define	FAL_FONT_UNKNOWN	0

/*
 * definition's data of character style
 */
#define	FAL_FONT_MINCHO			1
#define	FAL_FONT_STR_MINCHO		"mincho"
#define	FAL_FONT_GOTHIC			2
#define	FAL_FONT_STR_GOTHIC		"gothic"
#define	FAL_FONT_MARU_GOTHIC		3
#define	FAL_FONT_STR_MARU_GOTHIC	"maru_gothic"
#define	FAL_FONT_TEXTBOOK		4
#define	FAL_FONT_STR_TEXTBOOK		"textbook"
#define	FAL_FONT_BRASH_GYOU		5
#define	FAL_FONT_STR_BRASH_GYOU		"brash_gyou"
#define	FAL_FONT_BRASH_KAI		6
#define	FAL_FONT_STR_BRASH_KAI		"brash_kai"
#define	FAL_FONT_HANDWRITING		7
#define	FAL_FONT_STR_HANDWRITING	"handwriting"

/*
 * definition's data of character shape
 */
#define	FAL_FONT_ZENKAKU		1
#define	FAL_FONT_STR_ZENKAKU		"zenkaku"
#define	FAL_FONT_HANKAKU		2
#define	FAL_FONT_STR_HANKAKU		"hankaku"
#define	FAL_FONT_QUARTER		3
#define	FAL_FONT_STR_QUARTER		"1/4kaku"

/*
 *	structures for font informations
 */
typedef struct {
	int	width;		/* width */
	int	height;		/* height */
	int	top;		/* the head code */
	int	bottom;		/* the final code */
} FalFontinfo ;

typedef struct {
	int	num;
	char	**path;
} FalFontPath;

typedef struct {
	int	def;		/* definition data */
	char	*name;		/* the information of the character line */
}FalFontDB;


/*
 * structure of the font information
 */
typedef struct {
	char	*xlfdname;	/* xlfd name */
	struct {
		int	w;	/* the width of the character size */
		int	h;	/* the height of the character size */
	}size;
	struct {
		int	w;	/* the width of the letter size */
		int	h;	/* the height of the letter size */
		int	x;	/* position x of the letter size */
		int	y;	/* position y of the letter size */
	}letter;
	FalFontDB	style;	/* character style */
	FalFontDB	shape;	/* character shape */
	FalFontDB	reserve ; /* reserve  */
	int	cd_set;		/* codeset */
	int	prm;		/* the equipment of output permission */
}FalFontData;

typedef struct {
	int	num;		/* the number of font data*/
	FalFontData	*list;	/* the pointer to the array of
					FalFontData structure */
}FalFontDataList;

/*
 *	UDC infomations
 */
typedef struct _FalCodeRegion {
	int	start ;		/* first code */
	int	end ;		/* last code */
} FalCodeRegion ;

typedef struct _FalGlyphRegion {
	int	start ;		/* first glyph index */
	int	end ;		/* last glyph index */
} FalGlyphRegion ;

typedef struct _FalGIInf {
	char	*charset_str;	/* charcter set */
	int	glyph_index;	/* glyph index */
} FalGIInf;

/*
 * structure of the font identifier
 */
#include <X11/Xos.h>
#include <X11/Xmd.h>
#include <X11/X.h>
#include <X11/Xproto.h>
#include <stdlib.h>
#include <dirent.h>

#ifdef Bool
#undef Bool
#endif

#include  "snfstruct.h"
#include  <X11/fonts/fontstruct.h>
#include  "pcf.h"

struct	pcf_inf {
	FontInfoRec     info;
	xCharInfo       org_bounds;
	PCFTablePtr     tables;
	int     ntables;
	CARD32  bmp_fmt;        /* bitOrder ,byteOrder, glyph, scan */
	int     nbitmaps;
	CARD32  *offsets;
	int     sizebitmaps;
	char    *bitmaps;
	CARD32  enc_fmt;
	CARD16  *encodingOffsets;
};

typedef struct _Oak_Finf {
    int 	ismmap;		/* used mmap()? */
    int 	fsize;		/* file size */
    int 	fd;		/* file descripter */
    int 	isFef;
    char	*buf;		/* font file allocated on memory */
    char	*fname;		/* name of font file */
/* added by Rudie */
    int 	isPcf;
    struct	pcf_inf	pcfinf;
/* added by Rudie */
    FontInfoPtr	  pFinf;
    CharInfoPtr   pCinf;
    unsigned char *pGlyphs;
    unsigned int  width;
    unsigned int  height;
    unsigned int  start;
    unsigned int  end;
    int	          sptnBufL;
    char          *sptnBuf;
    int           dptnBufL;
    char          *dptnBuf;
    int           dbufL;
    char          *dbuf;
} Oak_FontInf;

typedef Oak_FontInf * FalFontID;	/* font distinction */

/*
 *	function prototypes
 */
extern	FalFontID	FalOpenSysFont(
	FalFontData*		/* open_font_data */,
	int 			/* font_data_mask */,
	FalFontDataList**	/* missing_font_list_return */
) ;

extern	int	FalGetFontList(
	FalFontData* 		/* key_data */,
	int 			/* mask */,
	FalFontDataList** 	/* list_ret */
) ;

extern	int	FalFreeFontList(
	FalFontDataList* 	/* list */
) ;

extern	char	*
FalReadFont(
	FalFontID	/* fid */ ,
	int 		/* code */ ,
	int 		/* width */ ,
	int 		/* height */
) ;

extern	int	FalQueryFont(
	FalFontID	/* fid */ ,
	FalFontinfo*	/* fontinfo */
) ;

extern	int 	FalFontOfFontID(
	FalFontID	/* fid */,
	FalFontData*	/* fontdata */
) ;

extern	int	FalCloseFont(
	FalFontID	/* fid */
) ;

extern	int	FalFontIDToFileName(
	FalFontID	/* fid */ ,
	char**		/* file_name */
) ;

extern	int	FalFree(
	void*	/* list */
) ;

extern	int	FalGetUDCCPArea(
	char*		/* locale */ ,
	int		/* code_set */ ,
	char*		/* charset_str */ ,
	FalCodeRegion**	/* cr */ ,
	int*		/* num_cr */
) ;

extern	int	FalGetUDCGIArea(
	char*		/* locale */ ,
	int		/* codeset */ ,
	char*		/* charset_str */ ,
	FalGlyphRegion**	/* gr */ ,
	int*		/* num_gr */
) ;

extern	FalFontID	FalOpenFont() ;

/*
 * Code set
 */
#define	FALGETFALCODESET( cd_set )	( ((cd_set)==-1) ? FAL_FONT_GLYPH_INDEX : (cd_set)+1 )

#define	FALCODESETTONUM( cd_set ) ( ((cd_set)==FAL_FONT_GLYPH_INDEX) ? -1 : (cd_set)-1 )

/*
 * fal_utyerrno
 */
#define	FAL_FUNCNUM_GFLST	0x0
#define	FAL_FUNCNUM_SRCHFNM	0x0
#define	FAL_FUNCNUM_OPNFNT	0x0
#define	FAL_FUNCNUM_FRFLST	0x0
#define	FAL_FUNCNUM_RDFNT	0x0
#define	FAL_FUNCNUM_QRY		0x0
#define	FAL_FUNCNUM_FID		0x0
#define	FAL_FUNCNUM_IDTOFNM	0x0
#define	FAL_FUNCNUM_CLSFNT	0x0
#define	FAL_FUNCNUM_CPA		0x0
#define	FAL_FUNCNUM_GIA		0x0
#define	FAL_FUNCNUM_FREE	0x0

#define	FAL_ERR_NON		0x00

#define	FAL_ERR_FNT_OPN		0x01
#define	FAL_ERR_FNT_RD		0x02
#define	FAL_ERR_MALLOC		0x03
#define	FAL_ERR_PARM		0x04
#define	FAL_ERR_STAT		0x05
#define	FAL_ERR_FONT		0x06
#define	FAL_ERR_FDATA_OPN	0x07
#define	FAL_ERR_FDATA_RD	0x08
#define	FAL_ERR_FDATA_DSC	0x09
#define	FAL_ERR_FPATH		0x0a

#define	FAL_ERR_FNTDIR_OPN	0x0b
#define	FAL_ERR_FNTDIR_RD	0x0c
#define	FAL_ERR_PROP_RD		0x0d
#define	FAL_ERR_PROP_FONT	0x0e
#define	FAL_ERR_PROP_FNAME	0x0f
#define FAL_ERR_LCKD		0x10
#define FAL_ERR_FILELCK		0x11
#define FAL_ERR_FILEUNLCK	0x12
#define FAL_ERR_FILEGETLCK	0x13
#define FAL_ERR_NOFONT		0x14

#define	FAL_ERR_XLC_CRTDB	0x15
#define	FAL_ERR_XLC_GETCHARST	0x16
#define	FAL_ERR_XLC_NOCHARST	0x17

#define	FAL_ERR_FID_RD		0x18
#define	FAL_ERR_FATAL		0x19


#ifdef __cplusplus
}
#endif
#endif /* _FALIB_H */
