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
/* $XConsortium: FontAttrI.h /main/7 1995/12/06 18:36:56 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **  File:  FontAttrI.h
 **  Project:  Common Desktop Environment
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/
#ifndef _DtHelpFontAttrI_h
#define _DtHelpFontAttrI_h

#ifdef __cplusplus
extern "C" {
#endif

/********    Public Defines Declarations    ********/
/*
 * font attributes
 */
#define _CEFONT_CHAR_SET       6
#define _CEFONT_LANG_TER       5
#define _CEFONT_TYPE           4
#define _CEFONT_WEIGHT         3
#define _CEFONT_ANGLE          2
#define _CEFONT_SIZE           1
#define _CEFONT_SPACING        0

#define _CEFONT_END            7

#define _CEFontAttrNumber     8

/********    Public Enum Declarations    ********/
enum    _dtHelpFontValue
    {
	_DtHelpFontValueBad,
/* styles  */
	_DtHelpFontStyleSerif      ,
	_DtHelpFontStyleSanSerif   ,
	_DtHelpFontStyleSymbol,
/* spacing */
	_DtHelpFontSpacingMono     ,
	_DtHelpFontSpacingProp     ,
/* weights */
	_DtHelpFontWeightMedium    ,
	_DtHelpFontWeightBold      ,
/* slant   */
	_DtHelpFontSlantRevItalic  ,
	_DtHelpFontSlantItalic     ,
	_DtHelpFontSlantRoman      ,
/* special */
	_DtHelpFontSpecialUnderLine,
	_DtHelpFontSpecialStrikeOut,
	_DtHelpFontSpecialNone
    };

/********    Public Typedef Declarations    ********/

typedef	enum _dtHelpFontValue	_DtHelpFontValue;

/********    Public Structures Declarations    ********/

typedef struct  _dtHelpFontHints {
	char	*language;
        char    *char_set;
	int	 pointsz;       /* height of font in points */
	int      set_width;     /* width  of font in points */
	char	*color;         /* 1 of 42 UDT colors or 'RGB:rrrr/gggg/bbbb */
	char	*xlfd;          /* xlfd name for use on X Window System */
	char	*xlfdb;         /* xlfd name for use on X Window System */
	char	*xlfdi;         /* xlfd name for use on X Window System */
	char	*xlfdib;        /* xlfd name for use on X Window System */
	char	*typenam;       /* typeface spec for MS-Windows interface */
	char	*typenamb;      /* typeface spec for MS-Windows interface */
	char	*typenami;      /* typeface spec for MS-Windows interface */
	char	*typenamib;     /* typeface spec for MS-Windows interface */
	_DtHelpFontValue  style;    /* font style              */
	_DtHelpFontValue  spacing;  /* font spacing            */
	_DtHelpFontValue  weight;   /* font weight             */
	_DtHelpFontValue  slant;    /* font slant              */
	_DtHelpFontValue  special;  /* special characteristics */
	void    *expand;        /* reserved pointer for later expansion */
} _DtHelpFontHints;

/********    Public Structure Typedef Declarations    ********/

/********    Public Prototyped Procedures    ********/

/********    Public Macro Declarations    ********/
#ifndef	_DtHelpFontHintsColor
#define	_DtHelpFontHintsColor(x)	((x).color)
#endif

#ifndef	_DtHelpFontHintsLang
#define	_DtHelpFontHintsLang(x)		((x).language)
#endif

#ifndef	_DtHelpFontHintsCharSet
#define	_DtHelpFontHintsCharSet(x)	((x).char_set)
#endif

#ifndef	_DtHelpFontHintsPtSize
#define	_DtHelpFontHintsPtSize(x)	((x).pointsz)
#endif

#ifndef	_DtHelpFontHintsWeight
#define	_DtHelpFontHintsWeight(x)	((x).weight)
#endif

#ifndef	_DtHelpFontHintsXlfd
#define	_DtHelpFontHintsXlfd(x)		((x).xlfd)
#endif

#ifndef	_DtHelpFontHintsXlfdb
#define	_DtHelpFontHintsXlfdb(x)	((x).xlfdb)
#endif

#ifndef	_DtHelpFontHintsXlfdi
#define	_DtHelpFontHintsXlfdi(x)	((x).xlfdi)
#endif

#ifndef	_DtHelpFontHintsXlfdib
#define	_DtHelpFontHintsXlfdib(x)	((x).xlfdib)
#endif

#ifndef	_DtHelpFontHintsTypeNam
#define	_DtHelpFontHintsTypeNam(x)	((x).typenam)
#endif

#ifndef	_DtHelpFontHintsTypeNamb
#define	_DtHelpFontHintsTypeNamb(x)	((x).typenamb)
#endif

#ifndef	_DtHelpFontHintsTypeNami
#define	_DtHelpFontHintsTypeNami(x)	((x).typenami)
#endif

#ifndef	_DtHelpFontHintsTypeNamib
#define	_DtHelpFontHintsTypeNamib(x)	((x).typenamib)
#endif

#ifndef	_DtHelpFontPtrPtSize
#define	_DtHelpFontPtrPtSize(x)	((x)->pointsz)
#endif

#ifndef	_DtHelpFontPtrWeight
#define	_DtHelpFontPtrWeight(x)	((x)->weight)
#endif

/********    Semi-Public Function Declarations    ********/

/********    Public Function Declarations    ********/
extern	void	_DtHelpCeCopyDefFontAttrList(_DtHelpFontHints *font_attr );
extern	int	_DtHelpDupFontHints(_DtHelpFontHints *font_attr );
extern	void	_DtHelpFreeFontHints(_DtHelpFontHints *font_attr );

/********    End Public Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _DtHelpFontAttrI_h */
