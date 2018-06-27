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
/* $XConsortium: ilxpmP.h /main/4 1995/10/23 16:03:34 rswiston $ */
/* Copyright 1990,91 GROUPE BULL -- See licence conditions in file COPYRIGHT */
/*****************************************************************************\
* xpmP.h:                                                                     *
*                                                                             *
*  XPM library                                                                *
*  Private Include file                                                       *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

#ifndef XPMP_h
#define XPMP_h

#ifdef Debug
/* memory leak control tool */
#include <mnemosyne.h>
#endif

/*
#ifdef VMS
#include "decw$include:Xlib.h"
#include "decw$include:Intrinsic.h"
#include "sys$library:stdio.h"
#else
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <stdio.h>
#endif
*/

#include "ilxpm.h"
#ifndef IL_H
#include "il.h"
#endif

/* we keep the same codes as for Bitmap management */
/*
#ifndef _XUTIL_H_
#ifdef VMS
#include "decw$include:Xutil.h"
#else
#include <X11/Xutil.h>
#endif
#endif
*/


typedef struct {
    unsigned int type;
    union {
	FILE *file;
	char **data;
    }     stream;
    char *cptr;
    unsigned int line;
    int CommentLength;
    char Comment[BUFSIZ];
    char *Bcmt, *Ecmt, Bos, Eos;
    unsigned int InsideString;		/* used during parsing: 0 or 1
					 * whether we are inside or not */
}      xpmData;

#define XPMARRAY 0
#define XPMFILE  1
#define XPMPIPE  2

typedef unsigned char byte;

#define EOL '\n'
#define TAB '\t'
#define SPC ' '

typedef struct {
    char *type;				/* key word */
    char *Bcmt;				/* string begining comments */
    char *Ecmt;				/* string ending comments */
    char Bos;				/* character begining strings */
    char Eos;				/* character ending strings */
    char *Strs;				/* strings separator */
    char *Dec;				/* data declaration string */
    char *Boa;				/* string begining assignment */
    char *Eoa;				/* string ending assignment */
}      xpmDataType;

extern xpmDataType xpmDataTypes[];

/*
 * rgb values and ascii names (from rgb text file) rgb values,
 * range of 0 -> 65535 color mnemonic of rgb value
 */
typedef struct {
    int r, g, b;
    char *name;
}      xpmRgbName;

/* Maximum number of rgb mnemonics allowed in rgb text file. */
#define MAX_RGBNAMES 1024

extern char *xpmColorKeys[];

#define TRANSPARENT_COLOR "None"	/* this must be a string! */

/* number of xpmColorKeys */
#define NKEYS 5

/*
 * key numbers for visual type, they must fit along with the number key of
 * each corresponding element in xpmColorKeys[] defined in xpm.h
 */
#define MONO	2
#define GRAY4	3
#define GRAY 	4
#define COLOR	5

/* structure containing data related to an Xpm pixmap */
typedef struct {
    char *name;
    unsigned int width;
    unsigned int height;
    unsigned int cpp;
    unsigned int ncolors;
    char ***colorTable;
    unsigned int *pixelindex;
/*    XColor *xcolors;  */
    char **colorStrings;
    unsigned int mask_pixel;		/* mask pixel's colorTable index */
    unsigned short     *pPalette;       /* color palette derived for xpm Image */
    short       nPaletteColors;         /* current entry count in palette */
}      xpmInternAttrib;

#define UNDEF_PIXEL 0x80000000

/* XPM private routines */

FUNC(xpmCreateImage, int, (ilObject      object,
			   xpmInternAttrib * attrib,
			   ilClientImage * image_return,
			   ilClientImage * shapeimage_return,
			   XpmAttributes * attributes));

FUNC(xpmParseData, int, (xpmData * data,
			 xpmInternAttrib * attrib_return,
			 XpmAttributes * attributes));

/* 
FUNC(xpmScanImage, int, (Display * display,
			 XImage * image,
			 XImage * shapeimage,
			 XpmAttributes * attributes,
			 xpmInternAttrib * attrib));

FUNC(xpmVisualType, int, (Visual * visual));
*/

FUNC(xpmFreeColorTable, int, (char ***colorTable, int ncolors));

FUNC(xpmInitInternAttrib, int, (xpmInternAttrib * coedata));

FUNC(xpmFreeInternAttrib, int, (xpmInternAttrib * coedata));

FUNC(xpmSetAttributes, int, (xpmInternAttrib * attrib,
			     XpmAttributes * attributes));

FUNC(xpmGetAttributes, int, (XpmAttributes * attributes,
			     xpmInternAttrib * attrib));

/* I/O utility */

FUNC(xpmNextString, int, (xpmData * mdata));
FUNC(xpmNextUI, int, (xpmData * mdata, unsigned int *ui_return));
FUNC(xpmGetC, int, (xpmData * mdata));
FUNC(xpmUngetC, int, (int c, xpmData * mdata));
FUNC(xpmNextWord, unsigned int, (xpmData * mdata, char *buf));
FUNC(xpmGetCmt, int, (xpmData * mdata, char **cmt));
FUNC(xpmReadFile, int, (char *filename, xpmData * mdata));
FUNC(xpmWriteFile, int, (char *filename, xpmData * mdata));
FUNC(xpmOpenArray, int, (char **data, xpmData * mdata));
FUNC(XpmDataClose, int, (xpmData * mdata));

/* RGB utility */
 
/*
FUNC(xpmReadRgbNames, int, (char *rgb_fname, xpmRgbName * rgbn));
FUNC(xpmGetRgbName, char *, (xpmRgbName * rgbn, int rgbn_max,
			     int red, int green, int blue));
FUNC(xpmFreeRgbNames, void, (xpmRgbName * rgbn, int rgbn_max));

FUNC(xpm_xynormalizeimagebits, int, (unsigned char *bp,
				     XImage * img));
FUNC(xpm_znormalizeimagebits, int, (unsigned char *bp,
				    XImage * img));
*/
/*
 * Macros
 * 
 * The XYNORMALIZE macro determines whether XY format data requires 
 * normalization and calls a routine to do so if needed. The logic in
 * this module is designed for LSBFirst byte and bit order, so 
 * normalization is done as required to present the data in this order.
 *
 * The ZNORMALIZE macro performs byte and nibble order normalization if 
 * required for Z format data.
 * 
 * The XYINDEX macro computes the index to the starting byte (char) boundary
 * for a bitmap_unit containing a pixel with coordinates x and y for image
 * data in XY format.
 * 
 * The ZINDEX* macros compute the index to the starting byte (char) boundary 
 * for a pixel with coordinates x and y for image data in ZPixmap format.
 * 
 */

/*
#define XYNORMALIZE(bp, img) \
    if ((img->byte_order == MSBFirst) || (img->bitmap_bit_order == MSBFirst)) \
	xpm_xynormalizeimagebits((unsigned char *)(bp), img)

#define ZNORMALIZE(bp, img) \
    if (img->byte_order == MSBFirst) \
	xpm_znormalizeimagebits((unsigned char *)(bp), img)

#define XYINDEX(x, y, img) \
    ((y) * img->bytes_per_line) + \
    (((x) + img->xoffset) / img->bitmap_unit) * (img->bitmap_unit >> 3)

#define ZINDEX(x, y, img) ((y) * img->bytes_per_line) + \
    (((x) * img->bits_per_pixel) >> 3)

#define ZINDEX32(x, y, img) ((y) * img->bytes_per_line) + ((x) << 2)

#define ZINDEX16(x, y, img) ((y) * img->bytes_per_line) + ((x) << 1)

#define ZINDEX8(x, y, img) ((y) * img->bytes_per_line) + (x)

#define ZINDEX1(x, y, img) ((y) * img->bytes_per_line) + ((x) >> 3)

*/


#if defined(__STDC__)
#define Const const
#else
#define Const				/**/
#endif

#endif
