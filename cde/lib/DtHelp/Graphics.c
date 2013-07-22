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
/************************************<+>*************************************
 ****************************************************************************
 **
 **  File:	Graphics.c
 **
 **  Project:    CDE Help System
 **
 **  Description: This code processes a graphic (X11 bitmap, X11 pixmap,
 **               X11 XWD file, and TIFF). It will degrade graphics to
 **		  match the type of monitor the image is displayed upon.
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
/******************************************************************************
 **  The GreyScale and Perform_Dither routine were take and modified from
 **  the g-job 'xgedit' created by Mark F. Cook. The following is the
 **  disclaimer from the dither.c source code
 ******************************************************************************
 **  Program:		xgedit.c
 **
 **  Description:	X11-based graphics editor
 **
 **  File:		dither.c, containing the following
 **                     subroutines/functions:
 **                       Dither_Image()
 **                       Perform_Dither()
 **
 **  Copyright 1988 by Mark F. Cook and Hewlett-Packard, Corvallis
 **  Workstation Operation.  All rights reserved.
 **
 **  Permission to use, copy, and modify this software is granted, provided
 **  that this copyright appears in all copies and that both this copyright
 **  and permission notice appear in all supporting documentation, and that
 **  the name of Mark F. Cook and/or Hewlett-Packard not be used in advertising
 **  without specific, written prior permission.  Neither Mark F. Cook or
 **  Hewlett-Packard make any representations about the suitibility of this
 **  software for any purpose.  It is provided "as is" without express or
 **  implied warranty.
 **
 ******************************************************************************
 **  CHANGE LOG:
 **
 **     1.00.00 - 1/11/89 - Start of Rev. number.
 **
 ** @@@ 4.05.00 - 12/07/89 - Created Dither_Image() and Perform_Dither() to
 ** @@@           allow conversion of color images into a dithered black &
 ** @@@           white image suitable for output on a laser printer.
 **
 ******************************************************************************/

/*
static char rcs_id[]="$XConsortium: Graphics.c /main/23 1996/12/06 11:12:54 cde-hp $";
*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <X11/Xlib.h>
#include <X11/XWDFile.h>
#include <X11/ImUtil.h>
#ifndef	STUB
#include "il.h"
#include "ilfile.h"
#include "ilX.h"
#include "ilerrors.h"
#endif
#include <Xm/Xm.h>
#include <Xm/XmPrivate.h>
#include <Dt/Dts.h>
#include <Dt/xpm.h>

#include "bufioI.h"
#include "Access.h"
#include "AccessI.h"
#include "GraphicsP.h"
#include "GraphicsI.h"
#include "StringFuncsI.h"
#include "GifUtilsI.h"
#include "XbmUtilsI.h"
#include "JpegUtilsI.h"
#include "Lock.h"

/*****************************************************************************/
/*		      Private data					     */
/*****************************************************************************/
#define GR_SUCCESS            0       /* everything okay */
#define GR_DRAWABLE_ERR       1       /* couldn't get drawable attr/geom */
#define GR_FILE_ERR           2       /* file open/read/write problem */
#define GR_REQUEST_ERR        3       /* bad placement or size */
#define GR_ALLOC_ERR          4       /* memory allocation failure */
#define GR_HEADER_ERR         5       /* file header version/size problem */

#define	MAX_GREY_COLORS		8

#define RES_TOLERANCE		25	/* resolution scaling tolerance */

static	short		GreyAllocated = False;
static	Pixel		GreyScalePixels[MAX_GREY_COLORS];
static	char		*GreyScaleColors[MAX_GREY_COLORS] =
			{
				"#212121212121",
				"#424242424242",
				"#636363636363",
				"#737373737373",
				"#949494949494",
				"#adadadadadad",
				"#bdbdbdbdbdbd",
				"#dededededede",
			};

#define Missing_bm_width 75
#define Missing_bm_height 47
static unsigned char Missing_bm_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0x7f, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00,
   0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x04, 0x01, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xc0, 0x00, 0x18, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xc0, 0x00, 0x18, 0x8c, 0x21, 0x00, 0x00, 0x20, 0x00, 0x00, 0xc0, 0x00,
   0x18, 0x54, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x24,
   0x31, 0xf0, 0xf0, 0x30, 0xe8, 0xf0, 0xc1, 0x00, 0x18, 0x04, 0x21, 0x08,
   0x08, 0x20, 0x18, 0x09, 0xc1, 0x00, 0x18, 0x04, 0x21, 0xf0, 0xf0, 0x20,
   0x08, 0x09, 0xc1, 0x00, 0x18, 0x04, 0x21, 0x00, 0x01, 0x21, 0x08, 0x09,
   0xc1, 0x00, 0x18, 0x04, 0x21, 0x08, 0x09, 0x21, 0x08, 0x09, 0xc1, 0x00,
   0x18, 0x04, 0x71, 0xf0, 0xf0, 0x70, 0x08, 0xf1, 0xc1, 0x00, 0x18, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x18, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xf0, 0xc0, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xc0, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00,
   0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0xf0,
   0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x08, 0x01, 0x00,
   0x00, 0x08, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x08, 0x00, 0x00, 0x00, 0x08,
   0x20, 0x00, 0xc0, 0x00, 0x18, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
   0xc0, 0x00, 0x18, 0x08, 0xd8, 0xf0, 0xe8, 0xf8, 0x30, 0xf0, 0xc0, 0x00,
   0x18, 0xc8, 0x31, 0x01, 0x19, 0x09, 0x21, 0x08, 0xc1, 0x00, 0x18, 0x08,
   0x11, 0xf0, 0x09, 0x09, 0x21, 0x08, 0xc0, 0x00, 0x18, 0x08, 0x11, 0x08,
   0x09, 0x09, 0x21, 0x08, 0xc0, 0x00, 0x18, 0x08, 0x11, 0x08, 0x09, 0x09,
   0x21, 0x08, 0xc1, 0x00, 0x18, 0xf0, 0x10, 0xf0, 0xf9, 0x08, 0x71, 0xf0,
   0xc0, 0x00, 0x18, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0xc0, 0x00,
   0x18, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x00,
   0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xc0, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xc0, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00,
   0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf8, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00};

/* Used to hold image type converter registry info */
typedef struct {
    char *image_type;
    _DtGrLoadProc convert_proc;
    _DtGrDestroyContextProc destroy_context_proc;
} _DtGrRegistryRec;

/* 
 * This array maps image file name extensions to the corresponding
 * CDE data type.  It is used by the DetermineImageType function.
 * The array must be NULL-terminated.
 */
static char * img_extensions[] = {
    "xpm",  "PM",
    "pm",   "PM",
    "gif",  "GIF",
    "jpeg", "JPEG",
    "jpg",  "JPEG",
    "xbm",  "BM",
    "bm",   "BM",
    "tiff", "TIFF",
    "tif",  "TIFF",
    "xwd",  "XWD",
    "cgm",  "CGM",
    NULL
};
   
/*****************************************************************************/
/*		      Private declarations				     */
/*****************************************************************************/

static int		Do_Direct(
				Display *dpy,
				int      screen,
				XWDFileHeader *header,
				Colormap colormap,
				int ncolors,
				XColor *colors,
				enum _DtGrColorModel 	force,
				XImage *in_image,
				XImage *out_image,
				unsigned long **ret_colors,
				int  *ret_number );
static int		Do_Pseudo(
				Display *dpy,
				int	 screen,
				Colormap colormap,
				int ncolors,
				XColor *colors,
				enum _DtGrColorModel	force,
				XImage *in_image,
				XImage *out_image,
				unsigned long **ret_colors,
				int  *ret_number );
static int		GreyScale (
				Display   *dpy,
				int	   screen,
				Colormap   cmap,
				XImage    *in_image,
				XImage    *out_image,
				XColor    *colors,
				int        ncolors,
				enum _DtGrColorModel	force,
				int        rshift,
				int        gshift,
				int        bshift,
				Pixel      rmask,
				Pixel      gmask,
				Pixel      bmask );
static unsigned int	Image_Size(
				XImage *image );
static void		Perform_Dither(
				Display *dpy,
				int      screen,
				XImage  *image,
				int     *greyscale );
static enum _DtGrLoadStatus processBitmap(
                                _DtGrStream           *image,
				Screen                *screen,
				int                   depth,
				Colormap              colormap,
				Visual                *visual,
				Pixel                 foreground,
				Pixel                 background,
				GC                    gc,
				enum _DtGrColorModel  color_model,
				Boolean               allow_reduced_colors,
				Dimension             *in_out_width,
				Dimension             *in_out_height,
				unsigned short        media_resolution,
				Pixmap                *ret_pixmap,
				Pixmap                *ret_mask,
				Pixel                 **ret_colors,
				int                   *ret_num_colors,
			        _DtGrContext          *context);
#ifndef	STUB
static enum _DtGrLoadStatus processTiff(
                                _DtGrStream           *stream,
				Screen                *screen,
				int                   depth,
				Colormap              colormap,
				Visual                *visual,
				Pixel                 foreground,
				Pixel                 background,
				GC                    gc,
				enum _DtGrColorModel  color_model,
				Boolean               allow_reduced_colors,
				Dimension             *in_out_width,
				Dimension             *in_out_height,
				unsigned short        media_resolution,
				Pixmap                *ret_pixmap,
				Pixmap                *ret_mask,
				Pixel                 **ret_colors,
				int                   *ret_num_colors,
				_DtGrContext          *context);
static void destroyTiffContext(
                                _DtGrContext          *context);
#endif
static enum _DtGrLoadStatus processXwd(
                                _DtGrStream           *stream,
				Screen                *screen,
				int                   depth,
				Colormap              colormap,
				Visual                *visual,
				Pixel                 foreground,
				Pixel                 background,
				GC                    gc,
				enum _DtGrColorModel  color_model,
				Boolean               allow_reduced_colors,
				Dimension             *in_out_width,
				Dimension             *in_out_height,
				unsigned short        media_resolution,
				Pixmap                *ret_pixmap,
				Pixmap                *ret_mask,
				Pixel                 **ret_colors,
				int                   *ret_num_colors,
			        _DtGrContext          *context);
static void		_swaplong (
				register char *bp,
				register unsigned n );
static void		_swapshort (
				register char *bp,
				register unsigned n );
static	int		XwdFileToPixmap (
				Display  *dpy,
				int	  screen,
				int	  depth,
				Pixmap    pixmap,
				Colormap  colormap,
				Visual	 *visual,
				GC        gc,
                                enum _DtGrColorModel  color_model,
				int       src_x,
				int       src_y,
				int       dst_x,
				int       dst_y,
				int       width,
				int       height,
				_DtGrStream *stream,
				unsigned long **ret_colors,
				int      *ret_number );
static enum _DtGrLoadStatus processXpm(
                                _DtGrStream           *stream,
				Screen                *screen,
				int                   depth,
				Colormap              colormap,
				Visual                *visual,
				Pixel                 foreground,
				Pixel                 background,
				GC                    gc,
				enum _DtGrColorModel  color_model,
				Boolean               allow_reduced_colors,
				Dimension             *in_out_width,
				Dimension             *in_out_height,
				unsigned short        media_resolution,
				Pixmap                *ret_pixmap,
				Pixmap                *ret_mask,
				Pixel                 **ret_colors,
				int                   *ret_num_colors,
				_DtGrContext          *context);
static enum _DtGrLoadStatus processGIF(
                                _DtGrStream           *image,
				Screen                *screen,
				int                   depth,
				Colormap              colormap,
				Visual                *visual,
				Pixel                 foreground,
				Pixel                 background,
				GC                    gc,
				enum _DtGrColorModel  color_model,
				Boolean               allow_reduced_colors,
				Dimension             *in_out_width,
				Dimension             *in_out_height,
				unsigned short        media_resolution,
				Pixmap                *ret_pixmap,
				Pixmap                *ret_mask,
				Pixel                 **ret_colors,
				int                   *ret_num_colors,
				_DtGrContext          *context);
static enum _DtGrLoadStatus processJPEG(
                                _DtGrStream           *image,
				Screen                *screen,
				int                   depth,
				Colormap              colormap,
				Visual                *visual,
				Pixel                 foreground,
				Pixel                 background,
				GC                    gc,
				enum _DtGrColorModel  color_model,
				Boolean               allow_reduced_colors,
				Dimension             *in_out_width,
				Dimension             *in_out_height,
				unsigned short        media_resolution,
				Pixmap                *ret_pixmap,
				Pixmap                *ret_mask,
				Pixel                 **ret_colors,
				int                   *ret_num_colors,
				_DtGrContext          *context);
static enum _DtGrLoadStatus DetermineImageType(
                                _DtGrStream *stream,
				char        *image_type);
static enum _DtGrLoadStatus GetConverterAndDestructor(
                                char *image_type, 
				_DtGrLoadProc *converter,
				_DtGrDestroyContextProc *destructor);
static float ComputeRatio(int media_res, int image_res);

/***************************************************************************** 
 *         Image type registry declarations
 *****************************************************************************/

/* Registry of converters and destructors for default image types */
static _DtGrRegistryRec registry[] =
{
    {
        "PM",
	processXpm,
        NULL
    },

    {
        "BM",
	processBitmap,
        NULL
    },

    {
        "GIF",
	processGIF,
        NULL
    },

    {
        "JPEG",
	processJPEG,
        NULL
    },

    {
        "TIFF",
	processTiff,
        destroyTiffContext
    },

    {
        "XWD",
	processXwd,
        NULL
    },
};

static int registry_count = XtNumber(registry);

/* Registry of converters and destructors for non-default image types */
static _DtGrRegistryRec *new_registry = NULL;
static int new_registry_count = 0;

/***************************************************************************** 
 *         Private Routines
 *****************************************************************************/
/***************************************************************************** 
 * taken straight out of xwud and modified.
 *****************************************************************************/
/* Copyright 1985, 1986, 1988 Massachusetts Institute of Technology */

static void
_swapshort (
    register char *bp,
    register unsigned n )
{
    register char c;
    register char *ep = bp + n;

    while (bp < ep) {
	c = *bp;
	*bp = *(bp + 1);
	bp++;
	*bp++ = c;
    }
}

static void
_swaplong (
    register char *bp,
    register unsigned n )
{
    register char c;
    register char *ep = bp + n;
    register char *sp;

    while (bp < ep) {
	sp = bp + 3;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	sp = bp + 1;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	bp += 2;
    }
}

static float
ComputeRatio(int media_res, int image_res)
{
    float ratio;
    int iratio, dx;

    if (media_res >= image_res) {
	ratio = (float)media_res / (float)image_res;
	if (media_res % image_res == 0)
	    return ratio;
	iratio = ratio + 0.5;
	dx = media_res - image_res * iratio;
	if (dx >= -RES_TOLERANCE && dx <= RES_TOLERANCE)
	    ratio = (float)iratio;
    } else {
	ratio = (float)image_res / (float)media_res;
	if (image_res % media_res == 0)
	    return 1.0 / ratio;
	iratio = ratio + 0.5;
	dx = image_res - media_res * iratio;
	if (dx >= -RES_TOLERANCE && dx <= RES_TOLERANCE)
	    ratio = (float)iratio;
	ratio = 1.0 / ratio;
    }
    return ratio;
}

/*****************************************************************************
 * Function: GreyScale
 *
 * Turn a color image into a 8 grey color image
 * If it can't create a grey scale image, GreyScale will call
 * Perform_Dither to create a bi-tonal image.
 *
 *****************************************************************************
 * The GreyScale code was ported from xgedit and changed to fit our needs
 *****************************************************************************/
static	int
GreyScale (
    Display   *dpy,
    int	       screen,
    Colormap   cmap,
    XImage    *in_image,
    XImage    *out_image,
    XColor    *colors,
    int        ncolors,
    enum _DtGrColorModel force,
    int        rshift,
    int        gshift,
    int        bshift,
    Pixel      rmask,
    Pixel      gmask,
    Pixel      bmask )
{
  int i, j, x;
  int     inc;
  int     count;
  int	  width = in_image->width;
  int	  height = in_image->height;
  int     value;
  int    *grey_scale;
  Pixel   valueArray[256];
  Pixel   n;
  XColor  ret_color;

/*--- allocate an array big enough to map each pixel in the image  ---*/
/*--- into a corresponding greyscale value (in the range [0-255]). ---*/

  grey_scale = (int *) calloc(width*height, sizeof(int));
  if (grey_scale == NULL) {
     return GR_ALLOC_ERR;
   }

/*---          < ESTABLISH THE GREYSCALE IMAGE >            ---*/
/*--- The NTSC formula for converting an RGB value into the ---*/
/*--- corresponding greyscale value is:                     ---*/
/*---      luminosity = .299 red + .587 green + .114 blue   ---*/

  /*
   * zero the flag array
   */
  for (i = 0; i < 256; i++)
      valueArray [i] = 0;

  for (j=0, x = 0; j<height; j++)
    for (i=0; i<width; i++, x++) {
      n = XGetPixel(in_image, i, j);
      if (rshift)
	{
	  ret_color.red   = (n >> rshift) & rmask;
	  ret_color.green = (n >> gshift) & gmask;
	  ret_color.blue  = (n >> bshift) & bmask;
	  if (ncolors)
	    {
	      ret_color.red   = colors[ret_color.red  ].red;
	      ret_color.green = colors[ret_color.green].green;
	      ret_color.blue  = colors[ret_color.blue ].blue;
	    }
	  else
	    {
	      ret_color.red   = (((Pixel) ret_color.red  ) * 65535) / rmask;
	      ret_color.green = (((Pixel) ret_color.green) * 65535) / gmask;
	      ret_color.blue  = (((Pixel) ret_color.blue ) * 65535) / bmask;
	    }

          value = (((int)(ret_color.red*299) + (int)(ret_color.green*587) +
				(int)(ret_color.blue*114)) / 1000) >> 8;
	}
      else
          value = (((int)(colors[n].red*299) + (int)(colors[n].green*587) +
				(int)(colors[n].blue*114)) / 1000) >> 8;
      grey_scale[x] = value;
      valueArray[value]++;
     } /* for(i...) */

  /*
   * Find out if we can/have allocate the pre-defined grey colors.
   */
  _DtHelpProcessLock();
  if (!GreyAllocated && force != _DtGrBITONAL)
    {
      for (i = 0; !GreyAllocated && i < MAX_GREY_COLORS; i++)
	{
	  if (!XParseColor (dpy, cmap, GreyScaleColors[i], &ret_color) ||
		!XAllocColor (dpy, cmap, &ret_color))
	    {
	      /*
	       * Have a problem allocating one of the pre-defined
	       * grey colors. Free the already allocated pixels
	       * and set the flag.
	       */
	      if (i)
	          XFreeColors (dpy, cmap, GreyScalePixels, i, 0);
	      GreyAllocated = -1;
	    }
	  else
	      GreyScalePixels[i] = ret_color.pixel;
	}
      if (!GreyAllocated)
          GreyAllocated = True;
    }

  /*
   * Had a problem allocating the pre-defined grey colors.
   * Try to dither into black and white.
   */
  if (force == _DtGrBITONAL || GreyAllocated == -1)
    {
      Perform_Dither (dpy, screen, out_image, grey_scale);
      free(grey_scale);
      _DtHelpProcessUnlock();
      return GR_SUCCESS;
    }
  _DtHelpProcessUnlock();

  /*
   * Find out how many grey scale colors there are.
   */
  for (count = 0, i = 0; i < 256; i++)
      if (valueArray[i])
	  count++;

  /*
   * If we have less than maximum grey colors we want to spread the load
   * between the colors
   */
  if (count < MAX_GREY_COLORS)
      inc = count;
  else
      inc = MAX_GREY_COLORS;

  /*
   * This is rather esoteric code.
   * The line
   *	valueArray[n] = GreyScalePixels[i * MAX_GREY_COLORS / inc]
   *
   * causes the colors to be evenly distributed if the total number
   * of calculated grey shades is less than the number of pre-defined
   * grey colors.
   *
   * The if stmt
   *	if (j >= count / (MAX_GREY_COLORS - i))
   *
   * uniformly reduces the calculated grey shades into the pre-defined
   * grey colors, if the total number of grey shades is greater than
   * the number of pre-defined grey colors.
   *
   * If after reading the following code, the reader still doesn't
   * understand it, pick a couple of numbers between 1-255 for 'count'
   * and walk each of them thru the 'for' loop. Hopefully it will help.
   * (Suggestion - pick a large number for one value and pick
   * a number less than 8 for the other value).
   */
  for (i = 0, n = 0, j = 0; n < 256 && count && i < MAX_GREY_COLORS; n++)
    {
      if (valueArray[n])
	{
	  _DtHelpProcessLock();
	  valueArray[n] = GreyScalePixels[i * MAX_GREY_COLORS / inc];
	  _DtHelpProcessUnlock();
	  j++;
	  if (j >= count / (MAX_GREY_COLORS - i))
	    {
	      count -= j;
	      i++;
	      j = 0;
	    }
	}
    }

  /*
   * Now replace the image pixels with the grey pixels
   */
  for (j = 0, x = 0; j < height; j++)
    for (i = 0; i < width; i++, x++) {
      XPutPixel (out_image, i, j, valueArray[grey_scale[x]]);
     } /* for(i...) */

  free(grey_scale);

  return GR_SUCCESS;
}

/***************************************************************************
 *                                                                         *
 * Routine:   Perform_Dither                                               *
 *                                                                         *
 * Purpose:   Given a color XImage and a greyscale representation of it,   *
 *            for each pixel in the image, determine whether it should be  *
 *            coverted to Black or White base on the weighted average of   *
 *            the greyscale value of it and the pixels surrounding it.     *
 *            Be sure to do bounds checking for pixels that are at the     *
 *            edge of the image.                                           *
 *                                                                         *
 *            The dithering is done using the Floyd-Steinberg error        *
 *            diffusion algorithm, which incorporates a Stucki error       *
 *            filter (specifics can be found in Chapter 8, "Dithering      *
 *            with Blue Noise", of the book "Digital Halftoning" by        *
 *            Robert Ulichney (MIT Press, 1988).                           *
 *                                                                         *
 *X11***********************************************************************/
static	void
Perform_Dither(
    Display *dpy,
    int      screen,
    XImage  *image,
    int   *greyscale )
{
  int i, j, width, height;
  int result, error, max_lum;
  Pixel    blackPixel = XBlackPixel (dpy, screen);
  Pixel    whitePixel = XWhitePixel (dpy, screen);

  width  = image->width;
  height = image->height;
  max_lum = 256;

  for (j=0; j<height; j++) /* rows */
    for (i=0; i<width; i++) { /* columns */
      if (greyscale[(j*width)+i] < (max_lum/2))
	result = 0;
      else
        result = max_lum - 1;
      error = greyscale[(j*width)+i] - result;
      if (i+1 < width)			/*--- G [j] [i+1] ---*/
        greyscale[(j*width)+i+1] += (error * 8)/42;
      if (i+2 < width)			/*--- G [j] [i+2] ---*/
        greyscale[(j*width)+i+2] += (error * 4)/42;

      if (j+1 < height) {
        if (i-2 >= 0)			/*--- G [j+1] [i-2] ---*/
          greyscale[((j+1)*width)+i-2] += (error * 2)/42;
        if (i-1 >= 0)			/*--- G [j+1] [i-1] ---*/
          greyscale[((j+1)*width)+i-1] += (error * 4)/42;
	/*--- G [j+1] [i] ---*/
        greyscale[((j+1)*width)+i] += (error * 8)/42;
        if (i+1 < width)		/*--- G [j+1] [i+1] ---*/
          greyscale[((j+1)*width)+i+1] += (error * 4)/42;
        if (i+2 < width)		/*--- G [j+1] [i+2] ---*/
          greyscale[((j+1)*width)+i+2] += (error * 2)/42;
       }

      if (j+2 < height) {
        if (i-2 >= 0)			/*--- G [j+2] [i-2] ---*/
          greyscale[((j+2)*width)+i-2] += error/42;
        if (i-1 >= 0)			/*--- G [j+2] [i-1] ---*/
          greyscale[((j+2)*width)+i-1] += (error * 2)/42;
	/*--- G [j+2] [i] ---*/
        greyscale[((j+2)*width)+i] += (error * 4)/42;
        if (i+1 < width)		/*--- G [j+2] [i+1] ---*/
          greyscale[((j+2)*width)+i+1] += (error * 2)/42;
        if (i+2 < width)		/*--- G [j+2] [i+2] ---*/
          greyscale[((j+2)*width)+i+2] += error/42;
       }

      if (result)
	XPutPixel(image, i, j, whitePixel);
      else
	XPutPixel(image, i, j, blackPixel);
     } /* for(i...) */
}


static int
Do_Pseudo(
    Display *dpy,
    int	     screen,
    Colormap colormap,
    int ncolors,
    XColor *colors,
    enum _DtGrColorModel force,
    XImage *in_image,
    XImage *out_image,
    unsigned long **ret_colors,
    int  *ret_number )
{
    register int i, x, y, colorCount = 0;
    register XColor *color;
    int      result = 0;
    Pixel    pixel;

    for (i = 0; i < ncolors; i++)
	colors[i].flags = 0;

    *ret_colors = NULL;
    *ret_number = 0;

    /*
     * beware 'result'.
     * It is set to one upon entering this routine.
     * The only way it can be modified is by the call to XAllocColor.
     */
    if (force == _DtGrCOLOR)
	result = 1;

    for (y = 0; result && y < in_image->height; y++)
      {
        for (x = 0; result && x < in_image->width; x++)
	  {
	    pixel = XGetPixel(in_image, x, y);
	    color = &colors[pixel];
	    if (!color->flags)
	      {
		color->flags = DoRed | DoGreen | DoBlue;
		result = XAllocColor(dpy, colormap, color);
		if (!result)
		    color->flags = 0;
		else
		    colorCount++;
	      }
	    if (result)
	        XPutPixel(out_image, x, y, color->pixel);
	  }
      }

    /*
     * If result == 0, a call to XAllocColor failed
     * Try to grey scale the image.
     */
    if (!result)
      {
	if (colorCount)
	  {
	    for (i = 0; i < ncolors; i++)
	      {
	        if (colors[i].flags)
	          {
	            XFreeColors (dpy, colormap, &(colors[i].pixel), 1, 0);
		    colors[i].flags = 0;
	          }
	      }
	  }
	result = GreyScale (dpy, screen, colormap, in_image, out_image, colors,
			ncolors, force, 0, 0, 0, 0, 0, 0);
      }
    else if (colorCount)
      {
	result = GR_SUCCESS;
        *ret_colors = (unsigned long *) malloc (
					sizeof (unsigned long) * colorCount);
	if (*ret_colors == NULL)
	  {
	    colorCount = 0;
	    result = GR_ALLOC_ERR;
	  }

        for (i = 0, x = 0; i < ncolors && x < colorCount; i++)
	    if (colors[i].flags)
	        (*ret_colors)[x++] = colors[i].pixel;

	*ret_number = colorCount;
      }

    /*
     * result was set to a XHPIF value via the 'else' stmt or
     * returned from GreyScale routine.
     */
    return result;
}

static int
Do_Direct(
    Display *dpy,
    int      screen,
    XWDFileHeader *header,
    Colormap colormap,
    int ncolors,
    XColor *colors,
    enum _DtGrColorModel force,
    XImage *in_image,
    XImage *out_image,
    unsigned long **ret_colors,
    int  *ret_number )
{
    register int x, y;
    XColor color;
    unsigned long rmask, gmask, bmask;
    int   rshift = 0, gshift = 0, bshift = 0;
    int   i;
    int   result;
    int   pixMax = 256;
    int   pixI   = 0;
    Pixel pix;
    Pixel *oldPixels;
    Pixel *newPixels;

    oldPixels = (Pixel *) malloc (sizeof (Pixel) * pixMax);
    newPixels = (Pixel *) malloc (sizeof (Pixel) * pixMax);

    if (oldPixels == NULL || newPixels == NULL)
      {
	if (oldPixels)
	    free (oldPixels);
	if (newPixels)
	    free (newPixels);

	return GR_ALLOC_ERR;
      }

    rmask = header->red_mask;
    while (!(rmask & 1)) {
	rmask >>= 1;
	rshift++;
    }
    gmask = header->green_mask;
    while (!(gmask & 1)) {
	gmask >>= 1;
	gshift++;
    }
    bmask = header->blue_mask;
    while (!(bmask & 1)) {
	bmask >>= 1;
	bshift++;
    }
    if (in_image->depth <= 12)
	pix = 1 << in_image->depth;

    if (force == _DtGrCOLOR)
        color.flags = DoRed | DoGreen | DoBlue;
    else
	color.flags = 0;

    for (y = 0; color.flags && y < in_image->height; y++)
      {
	for (x = 0; color.flags && x < in_image->width; x++)
	  {
	    pix = XGetPixel(in_image, x, y);

	    i = 0;
	    while (i < pixI && oldPixels[i] != pix)
		i++;

	    if (i == pixI)
	      {
		color.red = (pix >> rshift) & rmask;
		color.green = (pix >> gshift) & gmask;
		color.blue = (pix >> bshift) & bmask;
		if (ncolors) {
		    color.red = colors[color.red].red;
		    color.green = colors[color.green].green;
		    color.blue = colors[color.blue].blue;
		} else {
		    color.red = ((unsigned long)color.red * 65535) / rmask;
		    color.green = ((unsigned long)color.green * 65535) / gmask;
		    color.blue = ((unsigned long)color.blue * 65535) / bmask;
		}
		if (!XAllocColor(dpy, colormap, &color))
		    color.flags = 0;
		else 
		  {
		    if (pixI >= pixMax)
		      {
			pixMax += 128;
			oldPixels = (Pixel *) realloc ((void *) oldPixels,
					(sizeof (Pixel) * pixMax));
			newPixels = (Pixel *) realloc ((void *) newPixels,
					(sizeof (Pixel) * pixMax));

			/*
			 * check the realloc
			 */
			if (oldPixels == NULL || newPixels == NULL)
			  {
			    if (oldPixels)
				free (oldPixels);

			    if (newPixels)
			      {
				XFreeColors(dpy, colormap, newPixels, pixI, 0);
				free (newPixels);
			      }

			    return GR_ALLOC_ERR;
			  }
		      }
		    oldPixels[pixI]   = pix;
		    newPixels[pixI++] = color.pixel;
		  }
	      }
	    if (color.flags)
	        XPutPixel(out_image, x, y, newPixels[i]);
	  }
      }
    if (color.flags)
      {
	result = GR_SUCCESS;
        if (pixI < pixMax)
	  {
	    newPixels = (Pixel *) realloc ((void *) newPixels,
					(sizeof (Pixel) * pixI));
	    if (newPixels == NULL)
	        result = GR_ALLOC_ERR;
	  }

	free (oldPixels);

        *ret_colors = newPixels;
        *ret_number = pixI;
      }
    else
      {
	if (pixI)
	    XFreeColors (dpy, colormap, newPixels, pixI, 0);

        free (oldPixels);
        free (newPixels);

	result = GreyScale(dpy, screen, colormap, in_image, out_image, colors,
			ncolors, force, rshift, gshift, bshift, rmask, gmask, bmask);
      }

    return result;
}

static unsigned int
Image_Size(
     XImage *image )
{
    if (image->format != ZPixmap)
      return(image->bytes_per_line * image->height * image->depth);

    return((unsigned)image->bytes_per_line * image->height);
}

static int
XwdFileToPixmap (
    Display  *dpy,
    int       screen,
    int       depth,
    Pixmap    pixmap,
    Colormap  colormap,
    Visual   *visual,
    GC        gc,
    enum _DtGrColorModel  color_model,
    int       src_x,
    int       src_y,
    int       dst_x,
    int       dst_y,
    int       width,
    int       height,
    _DtGrStream *stream,
    unsigned long **ret_colors,
    int      *ret_number )
{
    int result;
    register int i;
    XImage in_image, *out_image;
    register char *buffer;
    unsigned long swaptest = 1;
    int count;
    unsigned buffer_size;
    int ncolors;
    Bool rawbits = False;
    XColor *colors = NULL;
#ifdef __alpha
/* Use a different structure for compatibility with 32-bit platform */
    XWDColor   xwd_color;
#endif /* __alpha */
     XWDFileHeader header;

    /* Reset the pointer to the beginning of the stream */
    _DtGrSeek(stream, 0, SEEK_SET);
    
    /*
     * Read in header information.
     */
    if(_DtGrRead((char *)&header, sizeof(header), 1, stream) != 1)
      {
        return GR_HEADER_ERR;
      }

    if (*(char *) &swaptest)
	_swaplong((char *) &header, sizeof(header));

    /* check to see if the dump file is in the proper format */
    if (header.file_version != XWD_FILE_VERSION)
      {
	return GR_HEADER_ERR;
      }

    if (header.header_size < sizeof(header))
      {
	return GR_HEADER_ERR;
      }

     /*
      * skip the window name
      */
    if (_DtGrSeek(stream, (header.header_size - sizeof(header)), 1))
      {
	return GR_FILE_ERR; 
      }

    /*
     * initialize the input image
     */
    in_image.width = (int) header.pixmap_width;
    in_image.height = (int) header.pixmap_height;
    in_image.xoffset = (int) header.xoffset;
    in_image.format = (int) header.pixmap_format;
    in_image.byte_order = (int) header.byte_order;
    in_image.bitmap_unit = (int) header.bitmap_unit;
    in_image.bitmap_bit_order = (int) header.bitmap_bit_order;
    in_image.bitmap_pad = (int) header.bitmap_pad;
    in_image.depth = (int) header.pixmap_depth;
    in_image.bits_per_pixel = (int) header.bits_per_pixel;
    in_image.bytes_per_line = (int) header.bytes_per_line;
    in_image.red_mask = header.red_mask;
    in_image.green_mask = header.green_mask;
    in_image.blue_mask = header.blue_mask;
    in_image.obdata = NULL;
    _XInitImageFuncPtrs(&in_image);

    /* read in the color map buffer */
    ncolors = header.ncolors;
    if (ncolors) {
	colors = (XColor *)malloc((unsigned) ncolors * sizeof(XColor));
	if (!colors)
          {
	    return GR_ALLOC_ERR;
	  }

#ifdef __alpha
/* Use XWDColor instead of XColor. Byte-swapping if it is necessary.
 * Move values back into Xcolor structure.
 */
        for (i = 0; i < ncolors; i++) {
            if (_DtGrRead( (char *) &xwd_color, sizeof(XWDColor), 1 , stream)  != 1 )
            {
                XFree ((char *) colors);
                return GR_FILE_ERR;
            }

            if (*(char *) &swaptest) {
                _swaplong((char *) &xwd_color.pixel, sizeof(xwd_color.pixel));
                _swapshort((char *) &xwd_color.red, 3 * sizeof(xwd_color.red));
            }

            colors[i].pixel = xwd_color.pixel;
            colors[i].red   = xwd_color.red;
            colors[i].green = xwd_color.green;
            colors[i].blue  = xwd_color.blue;
            colors[i].flags = xwd_color.flags;
        }
#else
	if(_DtGrRead((char *) colors, sizeof(XColor), ncolors, stream) != ncolors)
	  {
	    XFree ((char *) colors);
	    return GR_FILE_ERR;
	  }

	if (*(char *) &swaptest) {
	    for (i = 0; i < ncolors; i++) {
		_swaplong((char *) &colors[i].pixel, sizeof(long));
		_swapshort((char *) &colors[i].red, 3 * sizeof(short));
	    }
	}
#endif /* __alpha */
    }

    /*
     * alloc the pixel buffer
     */
    buffer_size = Image_Size(&in_image);
    buffer = (char *) malloc(buffer_size);
    if (buffer == NULL)
      {
	if (NULL != colors)
	    XFree ((char *) colors);
        return GR_ALLOC_ERR;
      }

    /* read in the image data */
    count = _DtGrRead(buffer, sizeof(char), (int)buffer_size, stream);

    if (count != buffer_size)
      {
	if (NULL != colors)
	    XFree ((char *) colors);
	XFree (buffer);
        return GR_FILE_ERR;
      }

    if (in_image.depth == 1) {
	in_image.format = XYBitmap;
	rawbits = True;
    }
    in_image.data = buffer;

    /* create the output image */
    result = GR_SUCCESS;

    if (rawbits) {
	out_image = &in_image;
    } else {
	out_image = XCreateImage(dpy, visual, depth,
			 (depth == 1) ? XYBitmap : in_image.format,
				 in_image.xoffset, NULL,
				 in_image.width, in_image.height,
				 XBitmapPad(dpy), 0);

	out_image->data = (char *) malloc(Image_Size(out_image));

	if ((header.visual_class == TrueColor) ||
		   (header.visual_class == DirectColor))
	    result = Do_Direct(dpy, screen, &header, colormap, ncolors, colors,
			color_model,
		      &in_image, out_image, ret_colors, ret_number);
	else
	    result = Do_Pseudo(dpy, screen, colormap, ncolors, colors, color_model,
			&in_image, out_image, ret_colors, ret_number);
    }

    if (result != GR_ALLOC_ERR)

    _XmPutScaledImage(dpy, pixmap, gc, out_image,
		      src_x, src_y, dst_x, dst_y,
		      in_image.width, in_image.height,
		      width, height);

    /*
     * free the buffers
     */
    if (NULL != colors)
        XFree ((char *) colors);
    XFree (buffer);
    if (!rawbits)
	XDestroyImage (out_image);

    return result;
}

/*****************************************************************************/
/*  General functions for changing a file into a pixmap                      */
/*                                                                           */
/*****************************************************************************/
/***********
 *
 * Function processBitmap
 *
 * takes a string.
 *	Make a bitmap into the pixmap for the graphic.
 *
 ***********/
static enum _DtGrLoadStatus processBitmap(
    _DtGrStream           *stream,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context
)
{
    int           result;
    int           junk;
    unsigned int  width, height;
    unsigned char *data;
    Pixmap        scaled_pixmap = 0;
    Display       *dpy = DisplayOfScreen(screen);
    Drawable      drawable = RootWindowOfScreen(screen);
    XImage        ximage;
    float          ratio = 1.0;

    if (media_resolution == 0) 
        return(_DtGrCONVERT_FAILURE);

    if (*in_out_width == 0 && *in_out_height == 0) 
	 ratio = ComputeRatio(media_resolution, 100);

    if (stream->type == _DtGrNONE)
        return(_DtGrCONVERT_FAILURE);

    result = _DtGrReadBitmapStreamData (stream,
					&width, &height, &data, &junk, &junk);

    /*  Be sure to implement XvpCopyPlane later         */
    if (result == BitmapSuccess)
    {
	ximage.height = height;
	ximage.width = width;
	ximage.depth = 1;
	ximage.bits_per_pixel = 1;
	ximage.xoffset = 0;
	ximage.format = XYBitmap;
	ximage.data = (char *)data;
	ximage.byte_order = LSBFirst;
	ximage.bitmap_unit = 8;
	ximage.bitmap_bit_order = LSBFirst;
	ximage.bitmap_pad = 8;
	ximage.bytes_per_line = (width+7)/8;
	XInitImage(&ximage);

        *in_out_width = (Dimension) width * ratio + 0.5;
        *in_out_height = (Dimension) height * ratio + 0.5;
	if (*in_out_width == 0)
	   *in_out_width = 1;
	if (*in_out_height == 0) 
	   *in_out_height = 1;

	scaled_pixmap = XCreatePixmap (dpy, drawable, (*in_out_width),
				       (*in_out_height), depth);
       _XmPutScaledImage(dpy, scaled_pixmap, gc, &ximage,0, 0, 0, 0,
			 width,height,(*in_out_width),(*in_out_height));
       XFree((char *)data);
       *ret_pixmap = scaled_pixmap;
    }

    if (result == BitmapSuccess)
        return(_DtGrSUCCESS);
    else if (result == BitmapOpenFailed)
        return(_DtGrOPEN_FAILED);
    else if (result == BitmapFileInvalid)
        return(_DtGrFILE_INVALID);
    else if (result == BitmapNoMemory)
        return(_DtGrNO_MEMORY);
    else 
        return(_DtGrCONVERT_FAILURE);
}

#ifndef	STUB
/***********
 *
 * Function processTiff
 *
 * takes a string.
 *	Make a tiff into the pixmap for the graphic.
 *
 ***********/
static enum _DtGrLoadStatus processTiff(
    _DtGrStream           *stream,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context
)
{
    int		 result = -1;
    Pixmap       pixmap = 0;
    ilFile	 inFile;
    ilPipe	 inPipe;
    ilFileImage	 inImage;
    const ilImageDes   *inDes;
    static ilContext	IlContext = NULL;
    Display      *dpy = DisplayOfScreen(screen);
    Drawable      drawable = RootWindowOfScreen(screen);  
    ilXWC	 *tiff_xwc;
    float         ratio = 1.0;

    _DtHelpProcessLock();
    if ((context == NULL) || (stream->type == _DtGrNONE))
      {
	_DtHelpProcessUnlock();
        return (_DtGrCONVERT_FAILURE);
      }

    if (media_resolution == 0) 
      {
	_DtHelpProcessUnlock();
        return(_DtGrCONVERT_FAILURE);
      }

    tiff_xwc =  (ilXWC *) &(context->context);
    if (IlContext == NULL)
      {
	if (IL_CREATE_CONTEXT (&IlContext, 0))
	  {
	    IlContext = NULL;
	    _DtHelpProcessUnlock();
 	    return (_DtGrCONVERT_FAILURE);
	  }
      }

    if (*tiff_xwc == NULL)
      {
	/* Fill in the context record fields */
        *tiff_xwc = ilCreateXWC (IlContext, dpy, visual, colormap, gc, 0, 0);
        if (*tiff_xwc == NULL)
	  {
	    _DtHelpProcessUnlock();
 	    return(_DtGrCONVERT_FAILURE);
	  }
        context->image_type = XtNewString("TIFF");
      }

    inFile = ilConnectFile (IlContext, stream, 0, 0);
    if (inFile)
      {
	inImage = ilListFileImages (inFile, 0);
	if (inImage)
	  {
	    int image_resolution = 100 ; /* assume that if not in file */


	    if (inImage->xRes != 0) {
		image_resolution = inImage->xRes ; /* dpi from the file */
	    }

	    if (*in_out_width == 0 && *in_out_height == 0)
		ratio = ComputeRatio(media_resolution, image_resolution);

	    *in_out_width = inImage->width * ratio + 0.5;
	    *in_out_height = inImage->height * ratio + 0.5;
	    if (*in_out_width == 0)
		*in_out_width = 1;
	    if (*in_out_height == 0) 
		*in_out_height = 1;

	    pixmap = XCreatePixmap (dpy, drawable, (*in_out_width),
				(*in_out_height), depth);
	    if (pixmap)
	      {
	        inPipe = ilCreatePipe (IlContext, 0);
	        if (inPipe)
	          {
		    if (color_model == _DtGrGRAY_SCALE)
			inDes = IL_DES_GRAY;
		    else if (color_model == _DtGrBITONAL)
			inDes = IL_DES_BITONAL;

		    result = 0;
		    if (!ilReadFileImage (inPipe, inImage, (ilRect *) NULL, 0))
			result = -1;

		    if (result == 0 && color_model != _DtGrCOLOR &&
			ilConvert (inPipe, inDes, ((ilImageFormat *) NULL), 0, NULL) != True)
			result  = -1;

		    if (result == 0 &&
			ilWriteXDrawable (inPipe, pixmap, *tiff_xwc,
						(ilRect *) NULL, 0, 0, 0))
		      {
			ilExecutePipe (inPipe, 0, ratio);
			if (IlContext->error != 0)
			    result = -1;
		      }
		    ilDestroyObject (inPipe);
	          }
		if (result == -1)
		  {
		    XFreePixmap (dpy, pixmap);
		    pixmap = 0;
		  }
	      }
	  }
	ilDestroyObject (inFile);
      }

    *ret_pixmap = pixmap;
    if (result < 0)
      {
        if ((IlContext->error == IL_ERROR_FILE_IO) ||
            (IlContext->error == IL_ERROR_FILE_NOT_TIFF))
	  {
	     _DtHelpProcessUnlock();
             return(_DtGrFILE_INVALID);
	  }
        else if (IlContext->error == IL_ERROR_MALLOC)
	  {
	     _DtHelpProcessUnlock();
             return(_DtGrNO_MEMORY);
	  }
        else
	  {
	     _DtHelpProcessUnlock();
             return(_DtGrCONVERT_FAILURE);
	  }
      }
    else
      {
	_DtHelpProcessUnlock();
        return(_DtGrSUCCESS);
      }
}
#endif

/***********
 *
 * Function processXwd
 *
 * takes a string.
 *	Make an XWD file into the pixmap for the graphic.
 *
 ***********/
static enum _DtGrLoadStatus processXwd(
    _DtGrStream           *stream,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context
)
{
    Pixmap         pixmap;
    XWDFileHeader  header;
    int            result;
    unsigned long swaptest = TRUE;
    Display       *dpy = DisplayOfScreen(screen);
    Drawable       drawable = RootWindowOfScreen(screen);  
    int            screen_num = XScreenNumberOfScreen(screen);
    float          ratio = 1.0;

    if (media_resolution == 0) 
        return(_DtGrCONVERT_FAILURE);

    if (stream->type == _DtGrNONE)
        return(_DtGrCONVERT_FAILURE);

    /* Read in XWDFileHeader structure */
    result = _DtGrRead((char *)&header, sizeof(header), 1, stream);

    if (result == 1 && *(char *) &swaptest)
	_swaplong((char *) &header, sizeof(header));

    if (result != 1 || header.file_version != XWD_FILE_VERSION)
	return (_DtGrFILE_INVALID);

    if (*in_out_width == 0 && *in_out_height == 0) 
	 ratio = ComputeRatio(media_resolution, 100);

    *in_out_width = header.pixmap_width * ratio + 0.5;
    *in_out_height = header.pixmap_height * ratio + 0.5;

     if (*in_out_width == 0)
	*in_out_width = 1;
     if (*in_out_height == 0) 
	*in_out_height = 1;

    pixmap = XCreatePixmap (dpy, drawable, (*in_out_width), 
			     (*in_out_height),depth);

    if (pixmap) {
        if ((result=XwdFileToPixmap (dpy, screen_num, depth, pixmap, colormap, 
                visual, gc, color_model, 0, 0, 0, 0,
		(*in_out_width), (*in_out_height), stream,
		ret_colors, ret_num_colors))
			== GR_SUCCESS) {
	    *ret_pixmap = pixmap;
            return (_DtGrSUCCESS);
        }
        else if (result == GR_HEADER_ERR)
            return (_DtGrFILE_INVALID);
        else if (result == GR_FILE_ERR)
            return (_DtGrFILE_INVALID);
        else if (result == GR_ALLOC_ERR)
            return (_DtGrNO_MEMORY);
        else
            return (_DtGrCONVERT_FAILURE);
    }
    else
        return (_DtGrNO_MEMORY);
}

/************************************
 * myXpmReadFileToPixmap
 ***********************************/
static int
myXpmReadFileToPixmap(
    Display        *display,
    Screen         *screen,
    Drawable        d,
    _DtGrStream    *stream,
    Pixmap         *pixmap_return,
    Pixmap         *shapemask_return,
    XpmAttributes  *attributes,
    GC              gc,
    Pixel	    bg,
    Pixel	    fg,
    int             depth,
    float           ratio
)
{
    XImage *image = NULL;
    XImage **imageptr = 0;
    XImage *shapeimage, **shapeimageptr = NULL;
    int ErrorStatus;
    int switchFlag = 0;
    int scaledWidth, scaledHeight;
    /*
     * initialize return values
     */

    if (pixmap_return) {
        *pixmap_return = 0;
        imageptr = &image;
    }
    if (shapemask_return) {
        *shapemask_return = 0;
        shapeimageptr = &shapeimage;
    }

    /*
     * create the images
     */

    if (stream->type == _DtGrFILE)
    {
        if (stream->source.file.uncompressed_filename != NULL)
            ErrorStatus = _DtXpmReadFileToImage(display, 
                                    stream->source.file.uncompressed_filename, 
                                    imageptr, shapeimageptr, attributes);
        else
            ErrorStatus = _DtXpmReadFileToImage(display, 
                                    stream->source.file.filename, 
                                    imageptr, shapeimageptr, attributes);
    }
    else if (stream->type == _DtGrBUFFER)
    {
            ErrorStatus = XpmCreateImageFromBuffer(display, 
                                    (char *) stream->source.buffer.base, 
                                    imageptr, shapeimageptr, attributes);
    }
    else
        ErrorStatus = XpmFileInvalid; /* stream type of _DtGrNONE */

    if (ErrorStatus < 0)
        return (ErrorStatus);

    /*
     * Check to see if we will need to switch the foreground and
     * background colors.  When forced to a depth of 1, the Xpm call
     * returns a ZPixmap ready to place in pixmap of depth 1.
     * Unfortunately, usually the pixmap is of a different depth.  This
     * causes a depth mismatch for the XPutImage.  Therefore, we change
     * the format type to XYBitmap and XPutImage is happy.  But on
     * servers where BlackPixel is not at pixel 1 in the colormap, this
     * causes the image to be inverted.  So by switching the foreground
     * and background in the gc, the image is corrected for these
     * systems.
     *
     * The other way of doing this, is to create a pixmap of depth 1 and
     * do an XPutImage to it.  Then do a XCopyPlane to a pixmap of the
     * correct depth.  But this is a major performance hit compared to
     * switching the colors.
     *
     * The downside of switching colors is that it works only when
     * BlackPixel and WhitePixel fill colormap entries 0 and 1.  But
     * since this is the 99.9% case, we'll go with it.
     */
    if (image->depth == 1 && BlackPixelOfScreen(screen) != 1L)
	switchFlag = 1;

    /*
     * create the pixmaps
     */
    if (imageptr && image) {
	scaledWidth = image->width * ratio + 0.5;
	scaledHeight = image->height * ratio + 0.5;

	if (scaledWidth == 0)
	    scaledWidth = 1;
	if (scaledHeight == 0)
	    scaledHeight = 1;
	
        *pixmap_return = XCreatePixmap(display, d, scaledWidth,
                                       scaledHeight, depth);
	if (image->depth == 1)
	    image->format = XYBitmap;

	if (switchFlag)
	  {
            XSetBackground (display, gc, fg);
            XSetForeground (display, gc, bg);
	  }
        _XmPutScaledImage(display, *pixmap_return, gc, image, 
			  0, 0, 0, 0,
			  image->width, image->height,
			  scaledWidth, scaledHeight);
	if (switchFlag)
	  {
	    XSetBackground (display, gc, bg);
	    XSetForeground (display, gc, fg);
	  }

        XDestroyImage(image);
    }

    if (shapeimageptr && shapeimage)
        XDestroyImage(shapeimage);

    return (ErrorStatus);
}

static XpmColorSymbol colorSymbol = {"none", NULL, 0};

/***********
 *
 * Function processXpm
 *
 * takes a string.
 *	Make an XPM file into the pixmap for the graphic.
 *
 ***********/

static enum _DtGrLoadStatus processXpm(
    _DtGrStream           *stream,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context
)
{
    int		    i, j;
    int             result;
    short           done;
    XpmAttributes   xpmAttr;
    Visual          vis2;
    float           ratio = 1.0;

    enum _DtGrLoadStatus status = _DtGrSUCCESS;

    if (media_resolution == 0) 
        return(_DtGrCONVERT_FAILURE);

    if (stream->type == _DtGrNONE)
        return(_DtGrCONVERT_FAILURE); /* stream type of _DtGrNONE */


    _DtHelpProcessLock();
    do
      {
        colorSymbol.pixel = background;

        xpmAttr.valuemask    = XpmVisual       | XpmReturnPixels |
			       XpmColorSymbols | XpmColormap     | XpmDepth;
        xpmAttr.visual       = visual;
        xpmAttr.colorsymbols = &colorSymbol;
        xpmAttr.colormap     = colormap;
        xpmAttr.numsymbols   = 1;
        xpmAttr.depth        = depth;
        xpmAttr.pixels       = NULL;

        /*
         * If not color, force to black and white.
         */
        if (color_model != _DtGrCOLOR)
          {
            memcpy (&vis2, xpmAttr.visual, sizeof(Visual));

            vis2.class       = StaticGray;
            vis2.map_entries = 2;

            xpmAttr.depth  = 1;
            xpmAttr.visual = &vis2;
          }
	if (*in_out_width == 0 && *in_out_height == 0) 
	    ratio = ComputeRatio(media_resolution, 100);

        result = myXpmReadFileToPixmap (DisplayOfScreen(screen), 
					screen, 
					RootWindowOfScreen(screen), 
					stream, ret_pixmap,
					ret_mask, &xpmAttr, gc,
					background, foreground, depth,
					ratio);
	done = True;
	/*
	 * if we did not successfully get our icon, force the color
	 * to black and white.
	 */
	if (result == XpmColorFailed && color_model != _DtGrBITONAL)
	  {
            if (allow_reduced_colors)
	    {
	        color_model = _DtGrBITONAL;
                status = _DtGrCOLOR_REDUCE;
	        done = False;
            }
            else
		status = _DtGrCOLOR_FAILED;
          }

      } while (done == False);

    if (result == XpmSuccess || result == XpmColorError)
      {
	*in_out_width   = xpmAttr.width * ratio + 0.5;
	*in_out_height  = xpmAttr.height * ratio + 0.5;
	if (*in_out_width == 0)
	   *in_out_width = 1;
	if (*in_out_height == 0) 
	   *in_out_height = 1;
	*ret_colors     = xpmAttr.pixels;
	*ret_num_colors = xpmAttr.npixels;

	/*
	 * squeeze out the pixel used for the transparent color since we
	 * don't want to free it when we free the other colors
	 */
	i = 0;
	while (i < xpmAttr.npixels && xpmAttr.pixels[i] != colorSymbol.pixel)
	    i++;

	if (i < xpmAttr.npixels)
	  {
	    for (j = i, i = i + 1; i < xpmAttr.npixels; i++, j++)
		xpmAttr.pixels[j] = xpmAttr.pixels[i];

	    *ret_num_colors = j;
	  }
      }
    _DtHelpProcessUnlock();

    if (result == XpmOpenFailed)
        status = _DtGrOPEN_FAILED;
    else if (result == XpmFileInvalid)
        status = _DtGrFILE_INVALID;
    else if (result == XpmNoMemory)
        status = _DtGrNO_MEMORY;
    else if (result == XpmColorFailed)
        status = _DtGrCOLOR_FAILED;

    return(status);
}

/******************************************************************************
 *
 * Function processGIF
 *
 * Default gif converter, creates a pixmap from a file-associated or 
 * buffer-associated stream of gif data.
 *
 *****************************************************************************/
static enum _DtGrLoadStatus processGIF(
    _DtGrStream           *stream,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context
)
{
    struct stat stbuf;
    char *buffer;
    GifObj g;
    enum _DtGrLoadStatus status;
    Display *display = DisplayOfScreen(screen);   
    Drawable drawable = RootWindowOfScreen(screen);
    int i, size=0;
    float ratio = 1.0;

    if (media_resolution == 0) 
        return(_DtGrCONVERT_FAILURE);

    if (stream->type == _DtGrNONE)
        return(_DtGrCONVERT_FAILURE); /* stream type of _DtGrNONE */

    *ret_pixmap = 0; /* Initialize the return pixmap to zero */

    /* 
    ** The gif-to-pixmap utilities operate only on in-memory buffers,
    ** so if this is a buffer-based stream, simply pass the buffer, and
    ** if this is a file-based stream, read the file contents into a buffer 
    ** and pass that.
    */

    if (stream->type == _DtGrBUFFER)
    {
        buffer = (char *) stream->source.buffer.base;
        size = stream->source.buffer.size;
    }
    else
    {
       if (stream->source.file.uncompressed_filename != NULL)
           stat (stream->source.file.uncompressed_filename, &stbuf);
       else
           stat (stream->source.file.filename, &stbuf);
       size = stbuf.st_size;
       buffer = XtMalloc(size);
       fread (buffer, 1, size, stream->source.file.fileptr);  
    }

    /* Initialize the gif object */
    status = InitGifObject (&g, display, drawable, screen, depth, colormap, 
                            visual, gc, color_model, allow_reduced_colors);

    if (*in_out_width == 0 && *in_out_height == 0) 
	 ratio = ComputeRatio(media_resolution, 100);

    /* Create an X pixmap from the gif object */
    if ((status == _DtGrSUCCESS) || (status == _DtGrCOLOR_REDUCE))
        *ret_pixmap = gif_to_pixmap (&g, (unsigned char *) buffer, size, 
                                     in_out_width, in_out_height, 
                                     g.f_black, g.f_white, ratio);

    /* Set the returned colors parameters */
    if (*ret_pixmap != 0)
    {
        if (g.f_do_visual == DO_COLOR)
	{
            *ret_num_colors = g.total_colors;       
            *ret_colors = (Pixel *) malloc(*ret_num_colors * sizeof(Pixel));
            for (i=0; i<*ret_num_colors; i++)
                (*ret_colors)[i] = g.GifCMap[i].pixel;
        }
        else /* DO_GREY */
	{
            *ret_num_colors = g.f_total_greys;       
            *ret_colors = (Pixel *) malloc(*ret_num_colors * sizeof(Pixel));
            for (i=0; i<*ret_num_colors; i++)
                (*ret_colors)[i] = g.GifGMap[i];
        }
    }

    /* Free up any resources associated with the gif object */
    DeleteGifObjectResources (&g);

    /* If we allocated a buffer, free it */
    if (stream->type == _DtGrFILE)
        XtFree(buffer);

    /* Return the status */
    return (status);
}

/******************************************************************************
 *
 * Function processJPEG
 *
 * Default jpeg converter, creates a pixmap from a file-associated or 
 * buffer-associated stream of jpeg data.
 *
 * The function first converts the jpeg stream to an XImage with a virtual
 * colormap using the jpeg_to_ximage call, then generates a new XImage that
 * uses the X colormap by calling the Do_Pseudo routine.  The Do_Pseudo 
 * routine, which is also used by the XWD converter, automatically handles X 
 * color allocation and color model degradation where necessary.  Finally,
 * a pixmap is generated from the XImage returned by Do_Pseudo.
 *
 *****************************************************************************/
static enum _DtGrLoadStatus processJPEG(
    _DtGrStream           *stream,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context
)
{
    enum _DtGrLoadStatus status;
    XImage *in_image, *out_image;
    XColor *colors;
    int ncolors;
    int xres;
    int result = GR_SUCCESS;
    Display *dpy = DisplayOfScreen(screen); 
    int screen_num = XScreenNumberOfScreen(screen);
    Drawable drawable = RootWindowOfScreen(screen); 
    float ratio = 1.0;
    unsigned int scaledWidth,scaledHeight;

    if (media_resolution == 0) 
        return(_DtGrCONVERT_FAILURE);

    if (*in_out_width == 0 && *in_out_height == 0) 
	ratio = 0.0;

    if (stream->type == _DtGrNONE)
        return(_DtGrCONVERT_FAILURE); /* stream type of _DtGrNONE */

    /*
    ** Convert the stream to an XImage with a virtual colormap
    */
    status = jpeg_to_ximage (stream, screen, visual, in_out_width,
                             in_out_height, &in_image, &colors, &ncolors,
			     &xres);

    if (ratio == 0.0) {
	if (!xres)
	    xres = 100;
	ratio = ComputeRatio(media_resolution, xres);
    }

    if (status == _DtGrSUCCESS)
    {
        if (in_image->depth == 1)
	{
            /*
	    ** Bitmap, no further image transformation necessary 
            */
	    out_image = in_image; 
            result = GR_SUCCESS;
        }
        else
        {
            /*
	    ** Create an output image and have Do_Pseudo allocate the needed
            ** pixels and store image data that uses these pixels into
            ** the output image.
            */
	    out_image = XCreateImage(dpy, visual, depth,
			           (depth == 1) ? XYBitmap : in_image->format,
				   in_image->xoffset, NULL,
				   in_image->width, in_image->height,
				   XBitmapPad(dpy), 0);

	    out_image->data = (char *) malloc(Image_Size(out_image));

            result = Do_Pseudo(dpy, screen_num, colormap, ncolors, colors, 
                               color_model, in_image, out_image, 
                               ret_colors, ret_num_colors);
	}

        /*
	** If we've got a good XImage, go ahead and make a pixmap out of it
        */
        if (result != GR_ALLOC_ERR)
	{
            /*
	    ** Create a pixmap the same size as the XImage 
            */        

	    scaledWidth = out_image->width * ratio + 0.5;
	    scaledHeight = out_image->height * ratio + 0.5;

	    if (scaledWidth == 0)
		scaledWidth = 1;
	    if (scaledHeight == 0)
		scaledHeight = 1;
	
            *ret_pixmap = XCreatePixmap (dpy, drawable, scaledWidth, 
                                         scaledHeight, depth);
            if (*ret_pixmap)
	    {
                /*
		** Copy the XImage into the pixmap and set the other
                ** return parameters.
                */
                _XmPutScaledImage(dpy, *ret_pixmap, gc, out_image, 0, 0, 0, 0, 
				  out_image->width, out_image->height, 
		                  scaledWidth, scaledHeight); 

                *in_out_width = scaledWidth;
                *in_out_height = scaledHeight;              
                status = _DtGrSUCCESS;
            }
            else 
                status = _DtGrNO_MEMORY;

            if (out_image != in_image)
	        XDestroyImage (out_image);
        }
        else
            status = _DtGrNO_MEMORY;

        /*
	** Free the colors array and the initial XImage 
        */
        XFree ((char *) colors);
        XDestroyImage (in_image);
    }

    return (status);
}

/******************************************************************************
 *
 * Function destroyTiffContext
 *
 * Default tiff context destructor, destroys the ilXWC object pointed to
 * by context->context and frees the context->image_type string.
 *
 *****************************************************************************/
static void destroyTiffContext(
    _DtGrContext          *context)
{
    if (context != NULL)
    {
        if (context->context != NULL)
	  {
	    _DtHelpProcessLock();
            ilDestroyObject ((ilObject) context->context);
	    _DtHelpProcessUnlock();
	  }
        if (context->image_type != NULL)
            XtFree(context->image_type);
    }
}

/******************************************************************************
 *
 * Function DetermineImageType
 *
 * Determines the image type of the specified stream and copies the
 * string representing that image type into the image_type buffer.
 *
 * If the stream is a buffer, the buffer is passed to DtDtsBufferToData
 * in hopes that the file might be identified through content criteria.
 * 
 * If the stream is a file, the following checks are made:
 *
 *   1. The file name extension is checked against a list of known image file 
 *      type extensions.
 *   2. The filename is passed to DtDtsFileToDataType in hopes that the file
 *      type might be identified through filename extension or content 
 *      criteria. 
 *   3. If the file was uncompressed into a temporary file, the temporary
 *      filename is passed to DtDtsFileToDataType in hopes that the file
 *      type might be identified through content criteria.
 *
 * If none of these checks are successful, _DtCONVERT_FAILURE is returned.
 * Otherwise, _DtGrSUCCESS is returned.
 *
 *****************************************************************************/
static enum _DtGrLoadStatus DetermineImageType(
    _DtGrStream *stream,
    char        *image_type)
{
    int i=0;
    char *ext;

    if (stream->type == _DtGrFILE)
    {
        /* First try to match the file name extension to a known mapping */
        if (stream->source.file.filename != NULL)
	{
            /* Get the filename extension */
            if (_DtHelpCeStrrchr(stream->source.file.filename, ".", 
                                 MB_CUR_MAX, &ext) == 0)
	    {
                ext++; /* Increment past the dot */
        	while (img_extensions[i] != NULL)
		{
                    if (strcmp(ext, img_extensions[i]) == 0)
		    {
                        /* Found a match */
		        strcpy (image_type,img_extensions[++i]);
                        return (_DtGrSUCCESS);
                    }
                    /* Skip two ahead to the next file extension name */
                    i += 2;
                }
            }
	}

        /* 
         * Didn't work, see if CDE datatyping can determine the image type.  
         * First try the filename, then try the uncompressed temporary 
         * filename, if it exists.
         */
        if ((ext = DtDtsFileToDataType(stream->source.file.filename)) == NULL)
            if (stream->source.file.uncompressed_filename != NULL)
                ext = DtDtsFileToDataType(
                                   stream->source.file.uncompressed_filename);

        /* If successful, save the image type and return */
        if (ext != NULL)
	{
            strcpy (image_type, ext);
            DtDtsFreeDataType(ext);
            return (_DtGrSUCCESS);
        }
    }
    else if (stream->type == _DtGrBUFFER)
    {
        /* See if the CDE datatyping mechanism can determine the image type */
        ext = DtDtsBufferToDataType (stream->source.buffer.base,
				     stream->source.buffer.size,
				     NULL);

        /* If successful, save the image type and return */
        if (ext != NULL)
	{
            strcpy (image_type, ext);
            DtDtsFreeDataType(ext);
            return (_DtGrSUCCESS);
        }
     }

    /* Couldn't figure out the type, return failure */
    return (_DtGrCONVERT_FAILURE);
}

/******************************************************************************
 *
 * Function GetConverterAndDestructor
 *
 * This function returns the converter and context destructor functions for
 * the specified image type.  The registry of default image types is searched
 * first, followed by the registry of new image types, until a match is made.
 * The function returns _DtGrSUCCESS if the image_type has been registered 
 * and _DtGrCONVERT_FAILURE if it has not.
 *
 *****************************************************************************/
static enum _DtGrLoadStatus GetConverterAndDestructor(
    char *image_type, 
    _DtGrLoadProc *converter,
    _DtGrDestroyContextProc *destructor)
{
    int i;

    /* No image_type string, return failure */
    if (image_type == NULL)
        return (_DtGrCONVERT_FAILURE);

    /* Search the default image type registry for the specified image type */
    _DtHelpProcessLock();
    for (i=0; i<registry_count; i++)
    {
        if (strcmp (registry[i].image_type, image_type) == 0)
	{
	    /* Found it, return the converter and destructor */
            *converter = registry[i].convert_proc;
            *destructor = registry[i].destroy_context_proc;
	    _DtHelpProcessUnlock();
            return (_DtGrSUCCESS);
	}
    }    

    /* Search the new image type registry for the specified image type */
    for (i=0; i<new_registry_count; i++)
    {
        if (strcmp (new_registry[i].image_type, image_type) == 0)
	{
	    /* Found it, return the converter */
            *converter = new_registry[i].convert_proc;
            *destructor = registry[i].destroy_context_proc;
	    _DtHelpProcessUnlock();
            return (_DtGrSUCCESS);
	}
    }    
    _DtHelpProcessUnlock();

    /* Image type wasn't registered, return failure */
    return (_DtGrCONVERT_FAILURE);
}

/******************************************************************************
 *
 * Public Functions
 *
 *****************************************************************************/
/******************************************************************************
 *
 * Function _DtHelpProcessGraphic
 *
 * takes a string.
 *	Get the pixmap for the graphic.
 *
 *****************************************************************************/
Pixmap 
_DtHelpProcessGraphic(
	Display    *dpy,
	Drawable    drawable,
	int	    screen,
	int	    depth,
	GC          gc,
        Pixmap	   *def_pix,
	Dimension  *def_pix_width,
	Dimension  *def_pix_height,
        _DtGrContext *context,
	Colormap    colormap,
	Visual	   *visual,
	Pixel	    fore_ground,
	Pixel	    back_ground,
        char       *filename,
	unsigned short media_resolution,
	Dimension  *width,
	Dimension  *height,
	Pixmap     *ret_mask,
	Pixel     **ret_colors,
	int	   *ret_number )
{

    unsigned int    pixWidth  = 0;
    unsigned int    pixHeight = 0; 
    Dimension       pWidth, pHeight;
    Pixmap pix = 0, mask = 0;
    _DtGrStream stream;
    Screen *scrptr = ScreenOfDisplay (dpy, screen);
    static enum _DtGrColorModel ForceColor = _DtGrCOLOR;
    static Boolean first_time = TRUE;

    /*
     * initialize the return values
     */
    *ret_colors = NULL;
    *ret_number = 0;
    *ret_mask   = None;

    /*
     * Initialize the Force Variable if this is the first time
     */
    _DtHelpProcessLock();
    if (first_time)
      {
	char    *ptr;
	char    *ptr2;
	char    *xrmName  = NULL;
	char    *xrmClass = NULL;
	char	*retStrType;
	XrmValue retValue;

	ForceColor = _DtGrCOLOR;

	XtGetApplicationNameAndClass (dpy, &ptr, &ptr2);
	xrmName  = malloc (strlen (ptr) + 14);
	xrmClass = malloc (strlen (ptr2) + 14);
	if (xrmName && xrmClass)
	  {
	    strcpy (xrmName , ptr);
	    strcat (xrmName , ".helpColorUse");

	    strcpy (xrmClass, ptr2);
	    strcat (xrmClass, ".HelpColorUse");

	    if (XrmGetResource (XtDatabase (dpy), xrmName, xrmClass,
					&retStrType, &retValue) == True)
	      {
		ptr = (char *) retValue.addr;
		/*
		 * check for GreyScale
		 */
		if (*ptr == 'G' || *ptr == 'g')
		    ForceColor = _DtGrGRAY_SCALE;
		/*
		 * check for BlackWhite
		 */
		else if (*ptr == 'B' || *ptr == 'b')
		    ForceColor = _DtGrBITONAL;
	      }

	    free (xrmName);
	    free (xrmClass);
	  }

	/*
	 * choose the correct visual type to use
	 */
	if (ForceColor == _DtGrCOLOR &&
		(visual->class == GrayScale || visual->class == StaticGray))
	    ForceColor = _DtGrGRAY_SCALE;

	if (ForceColor != _DtGrBITONAL && depth == 1)
	    ForceColor = _DtGrBITONAL;

        first_time = FALSE;
      }
    _DtHelpProcessUnlock();

    if (filename != NULL)
      {
	int   result;

        /* Open the file and associate with a stream */
	result = _DtGrOpenFile(&stream, filename);

	if (result != 0)
	    pix = 0;
        else 
	{
	    /* Create a pixmap from the image data stream */
           _DtGrLoad(&stream, NULL, scrptr, depth, colormap, visual,
                     fore_ground, back_ground, gc, ForceColor, TRUE, &pWidth, 
                     &pHeight, media_resolution, &pix, &mask, ret_colors, 
		     ret_number, context);
           pixWidth = pWidth;
           pixHeight = pHeight;
           /* Close the stream and the file associated with it */
           _DtGrCloseStream( &stream );

        }
      }

    if (pix == 0)
      {
	/*
	 * Try to get a localized pixmap
         * NOTE....
         * This is a cached pixmap.....
         * Make sure XFreePixmaps does not free this one.
	 */
	if (*def_pix == 0)
	  {
	    *def_pix = XmGetPixmap(scrptr, "Dthgraphic", fore_ground, back_ground);
	    if (*def_pix != XmUNSPECIFIED_PIXMAP)
	      {
	        Window        root_id;
	        int           x, y;
	        unsigned int  border;
	        unsigned int  depth;

	        if (XGetGeometry(dpy, *def_pix, &root_id, &x, &y,
				&pixWidth, &pixHeight, &border, &depth) == 0)
	          {
		    XmDestroyPixmap(scrptr, *def_pix);
		    *def_pix = 0;
	          }
	      }
	    else
	        *def_pix = 0;

	    /*
	     * couldn't get a localized pixmap, go with a build in default
	     */
	    if (*def_pix == 0)
	      {
	        pixWidth  = Missing_bm_width;
	        pixHeight = Missing_bm_height;
	        *def_pix  = XCreatePixmapFromBitmapData ( dpy, drawable,
					(char *) Missing_bm_bits,
					pixWidth, pixHeight,
					fore_ground, back_ground, depth);
	      }

	    *def_pix_width  = (Dimension) pixWidth;
	    *def_pix_height = (Dimension) pixHeight;
	  }

	pix       = *def_pix;
        pixWidth  = *def_pix_width;
        pixHeight = *def_pix_height;
      }

    *width  = (Dimension) pixWidth;
    *height = (Dimension) pixHeight;

    return pix;
}


/******************************************************************************
 *
 * Function _DtGrLoad
 *
 * Determines the image type of the specified stream and calls the appropriate
 * imagetype-to-pixmap converter function.  If the conversion is successful
 * and the caller passed the address of a pointer to NULL in the image_type
 * parameter, a copy of the image type string is allocated and stored at this
 * address.  The caller is responsible for freeing this string.  The caller
 * is also responsible for freeing the values passed back in the ret_pixmap,
 * ret_mask, ret_colors, and context parameters.  The _DtGrLoad function 
 * returns _DtGrCONVERT_FAILURE if the appropriate converter could not be
 * determined, otherwise it returns the value returned by the converter.
 *
 *****************************************************************************/
enum _DtGrLoadStatus _DtGrLoad(
    _DtGrStream           *stream,
    char                  **image_type,
    Screen                *screen,
    int                   depth,
    Colormap              colormap,
    Visual                *visual,
    Pixel                 foreground,
    Pixel                 background,
    GC                    gc,
    enum _DtGrColorModel  color_model,
    Boolean               allow_reduced_colors,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    unsigned short        media_resolution,
    Pixmap                *ret_pixmap,
    Pixmap                *ret_mask,
    Pixel                 **ret_colors,
    int                   *ret_num_colors,
    _DtGrContext          *context
)
{
    enum _DtGrLoadStatus status;
    _DtGrLoadProc converter;
    _DtGrDestroyContextProc destructor;
    char buf[20],*itype;

    /* Determine the image type */
    if ((image_type != NULL) && (*image_type != NULL))
        itype = *image_type; /* Caller specified image type */
    else
    {
        /* Image type not specified by caller, try to figure it out */
        if ((status = DetermineImageType(stream, buf)) != _DtGrSUCCESS)
	    return (status); /* Return failure if image type is unknown */
        itype = &buf[0];     
    }

    /* Look up the proper converter for this image type */
    if ((status=GetConverterAndDestructor(itype, &converter, &destructor)) == 
	                                                     _DtGrSUCCESS)
    {
        /* Call the converter */
        if (converter != NULL)
	{
	    status = (*converter)(stream, screen, depth, colormap, visual,
                                  foreground, background, gc, color_model, 
                                  allow_reduced_colors, in_out_width,
                                  in_out_height, media_resolution, ret_pixmap, 
	                          ret_mask, ret_colors, ret_num_colors, 
	                          context);
	}
        else
            status = _DtGrCONVERT_FAILURE;
    }
    
    /* Return a copy of the image type string if successful */
    if (status == _DtGrSUCCESS || status == _DtGrCOLOR_REDUCE)
        if ((image_type != NULL) && (*image_type == NULL) && (itype != NULL))
	    *image_type = XtNewString(itype);

    /* Return the conversion status */
    return (status);
}

/******************************************************************************
 *
 * Function _DtGrDestroyContext
 *
 * Obtains and calls the context destructor function for the image type
 * indicated by the context parameter.
 *
 *****************************************************************************/
void _DtGrDestroyContext(
    _DtGrContext *context)
{
    _DtGrLoadProc converter;
    _DtGrDestroyContextProc destructor;

    if (GetConverterAndDestructor(context->image_type, &converter, 
                                  &destructor) == _DtGrSUCCESS)
    {
        if (destructor != NULL)
	    (*destructor)(context);
    }
}

/******************************************************************************
 *
 * Function _DtGrRegisterConverter
 *
 * Registers a converter and context destructor function for the specified
 * image type.  If the image_type has not been registered, a new record for
 * it is created in the new image type registry.  If the image_type has been
 * registered, the current converter and context destructor functions are
 * replaced by those specified by the caller.  If the caller passes valid
 * addresses in the current_convert_proc and current_destroy_proc parameters, 
 * the current converter and context destructor functions will be saved there
 * first so that the caller may restore them at a later time.
 *
 *****************************************************************************/
void _DtGrRegisterConverter(
    char                    *image_type,
    _DtGrLoadProc           convert_proc,
    _DtGrDestroyContextProc destroy_context_proc,
    _DtGrLoadProc           *current_convert_proc,
    _DtGrDestroyContextProc *current_destroy_proc)
{
    int i;

    if (image_type == NULL)
        return;

    /* Search the default converter registry for the specified image type */
    _DtHelpProcessLock();
    for (i=0; i<registry_count; i++)
    {
        if (strcmp (registry[i].image_type, image_type) == 0)
	{
	    /* Found it, save the current procs, then replace them */
            /* with the new ones and return                        */
	    if (current_convert_proc != NULL)
	        *current_convert_proc = registry[i].convert_proc;
	    if (current_destroy_proc != NULL)
	        *current_destroy_proc = registry[i].destroy_context_proc;
            registry[i].convert_proc = convert_proc;
	    registry[i].destroy_context_proc = destroy_context_proc;
	    _DtHelpProcessUnlock();
            return;
	}
    }    

    /* Search the new converter registry for the specified image type */
    for (i=0; i<new_registry_count; i++)
    {
        if (strcmp (new_registry[i].image_type, image_type) == 0)
	{
	    /* Found it, save the current procs, then replace them */
            /* with the new ones and return                        */
	    if (current_convert_proc != NULL)
	        *current_convert_proc = new_registry[i].convert_proc;
	    if (current_destroy_proc != NULL)
	        *current_destroy_proc = new_registry[i].destroy_context_proc;
            new_registry[i].convert_proc = convert_proc;
	    new_registry[i].destroy_context_proc = destroy_context_proc;
	    _DtHelpProcessUnlock();
            return;
	}
    }    
    
    /* If we make it here, we've got a new image type to register */
    new_registry = (_DtGrRegistryRec *) XtRealloc ((char *) new_registry, 
                    sizeof(_DtGrRegistryRec) * (new_registry_count + 1));
    new_registry[new_registry_count].image_type = XtNewString (image_type);
    new_registry[new_registry_count].convert_proc = convert_proc;
    new_registry[new_registry_count].destroy_context_proc = 
                                                        destroy_context_proc;
    new_registry_count++;
    _DtHelpProcessUnlock();

    /* Newly registered type, so return NULL for current function params */
    if (current_convert_proc != NULL)
        *current_convert_proc = NULL;
    if (current_destroy_proc != NULL)
        *current_destroy_proc = NULL;

    return;
}

/******************************************************************************
 *
 * Input stream functions
 * 
 * These functions allow the creation and manipulation of a stream that
 * can be associated with either a file or a buffer.  They are intended to
 * be used by image data to pixmap converter functions that need to handle
 * both image files and in-memory buffers containing file data.
 *
 *****************************************************************************/

/******************************************************************************
 *
 * Function _DtGrOpenFile
 *
 * Opens a file for reading and associates it with the specified stream.  If 
 * the file is compressed, the function uncompresses it prior to opening.  
 * Returns 0 for success, -1 or the value of errno as set by fopen for 
 * failure.
 *
 *****************************************************************************/
int _DtGrOpenFile(
    _DtGrStream *stream,
    const char  *path)
{
    char *fname = NULL;

    if (stream == NULL) 
        return (-1);    /* Failure */

    /* Uncompress the file if necessary and obtain the new filename */
    if (_DtHelpCeGetUncompressedFileName(path, &fname) == -1)
    {
        stream->type = _DtGrNONE;
        return (-1);  /* Failure */
    }

    /* Open the file */
    stream->source.file.fileptr = fopen(fname,"r");
    if (stream->source.file.fileptr == NULL)
    {
        /* Failure, couldn't open the file */
        stream->type = _DtGrNONE;
        if ((fname != path) && (fname != NULL))
	{
            unlink (fname);
            free (fname);             
        }   
        return(errno);
    }

    /* Set the stream type and copy the filename */
    stream->type = _DtGrFILE;
    stream->source.file.filename = XtNewString(path);

    /* If the file was uncompressed and renamed, save the new name */
    if (fname == path)
        stream->source.file.uncompressed_filename = NULL;
    else
        stream->source.file.uncompressed_filename = fname;

    return(0);    /* Success */
}

/******************************************************************************
 *
 * Function _DtGrOpenBuffer
 *
 * Associates the specified in-memory buffer with the specified stream.
 * The function returns 0 for success and the EINVAL error code for failure.
 *
 *****************************************************************************/
int _DtGrOpenBuffer(
    _DtGrStream *stream,
    const char  *buffer,
    int         buffer_size)
{
    if ((stream == NULL) || (buffer == NULL) || (buffer_size < 0))
    {
        if (stream != NULL)
	    stream->type = _DtGrNONE;
        return(EINVAL); /* Failure */
    }

    /* Set the appropriate stream fields */
    stream->type = _DtGrBUFFER;
    stream->source.buffer.base = buffer;
    stream->source.buffer.size = buffer_size;
    stream->source.buffer.current = (char *)buffer;
    stream->source.buffer.end = (char *)(buffer + buffer_size - 1);
    return(0); /* Success */
}

/******************************************************************************
 *
 * Function _DtGrCloseStream
 *
 * The function closes the specified stream by setting the stream type to
 * _DtGrNONE.  If the stream is associated with a file, then the file is
 * closed and the filename in the stream structure is freed.  If the file
 * required decompression, then the uncompressed file is unlinked and the
 * uncompressed filename is freed. 
 *
 *****************************************************************************/
int _DtGrCloseStream(
    _DtGrStream *stream)
{
    int status;

    if ((stream == NULL) || (stream->type == _DtGrNONE))
        return(EOF);    /* Failure */

    if (stream->type == _DtGrFILE)
    {
        /* Close the file and free the filename */
        status = fclose(stream->source.file.fileptr);
        if (stream->source.file.filename != NULL)
            XtFree(stream->source.file.filename);      
        if (stream->source.file.uncompressed_filename != NULL)
	{ 
            /* Unlink the uncompressed file and free the filename */
            unlink(stream->source.file.uncompressed_filename);
            free(stream->source.file.uncompressed_filename);
        }
    }
    else if (stream->type == _DtGrBUFFER)
    {
        status = 0;    /* Success */
    }

    stream->type = _DtGrNONE;
    return (status);
}


/******************************************************************************
 *
 * Function _DtGrRead
 *
 * Stream version of fread, reads data from a stream into a buffer.  If the 
 * stream is file-associated, a call to fread is made.  If the stream is
 * buffer-associated, an analogous operation is performed on the stream 
 * buffer.  The number of items read from the stream is returned to the caller.
 *
 *****************************************************************************/
size_t _DtGrRead(
    void        *buffer,
    size_t      size,
    size_t      num_items,
    _DtGrStream *stream)
{
    unsigned nleft;
    int n;

    if ((stream == NULL) || (stream->type == _DtGrNONE))
        return(0);

    if (stream->type == _DtGrFILE)
    {
        return (fread(buffer, size, num_items, stream->source.file.fileptr));
    }
    else if (stream->type == _DtGrBUFFER)
    {
        /* This code mirrors that of fread in libc */

        if (size <= 0 || num_items <= 0)
            return (0);

	for (nleft = num_items * size; ; ) 
        {
            if (stream->source.buffer.current > stream->source.buffer.end) 
            { 
                /* past end of stream */
                if (stream->source.buffer.current == 
                    stream->source.buffer.end + 1)
                    return (num_items - (nleft + size - 1)/size);
                stream->source.buffer.current--;
            }
            n = (nleft < stream->source.buffer.end - 
                         stream->source.buffer.current + 1 ? nleft :
                 stream->source.buffer.end - 
                 stream->source.buffer.current + 1); 
            /* Copy the items into the caller-supplied buffer */
            buffer = (char *)memcpy(buffer, 
                                   (void *) stream->source.buffer.current, 
                                   (size_t)n) + n;
            stream->source.buffer.current += n;
            if ((nleft -= n) == 0)
                return (num_items);
        }
    }
}

/******************************************************************************
 *
 * Function _DtGrSeek
 *
 * Stream version of fseek, repositions the file or buffer pointer of a 
 * stream.  If the stream is file-associated, the return value is the value
 * returned by fseek.  If the stream is buffer-associated, 0 is returned if
 * the requested position is inside the buffer, -1 if it is not.
 *
 *****************************************************************************/
int _DtGrSeek(
    _DtGrStream *stream,
    long        offset,
    int         whence)
{        
    long newpos;

    if ((stream == NULL) || (stream->type == _DtGrNONE))
        return(-1); /* Failure */

    if (stream->type == _DtGrFILE)
    {
        return (fseek(stream->source.file.fileptr,offset,whence));
    }
    else if (stream->type == _DtGrBUFFER)
    {
        switch (whence) 
	{
	    case SEEK_SET:
                newpos = (long)stream->source.buffer.base + offset;
                break;
	    case SEEK_CUR:
                newpos = (long)stream->source.buffer.current + offset;
                break;
	    case SEEK_END:
                newpos = (long)stream->source.buffer.end + 1 + offset;
                break;
	    default:
                return (-1);
        }

        if ((newpos >= (long)stream->source.buffer.base) &&
            (newpos <= (long)stream->source.buffer.end))
	{
            /* New position is within buffer, reposition pointer */
            stream->source.buffer.current = (char *)newpos;
            return(0); /* Success */
        }
        else
            return(-1); /* Failure */
    }
}

/******************************************************************************
 *
 * Function _DtGrGetChar
 *
 * Stream version of fgetc, reads  a character from a stream and advances the 
 * stream position.  The next byte in the stream is returned, or EOF if an
 * error occurs or the end of the stream is reached.
 *
 *****************************************************************************/
int _DtGrGetChar(
    _DtGrStream *stream)
{
    if ((stream == NULL) || (stream->type == _DtGrNONE))
        return(EOF);

    if (stream->type == _DtGrFILE)
    {
        return (fgetc(stream->source.file.fileptr));
    }
    else if (stream->type == _DtGrBUFFER)
    {
        if (stream->source.buffer.current > stream->source.buffer.end)
            return (EOF);
        else
	    return ((unsigned char) *(stream->source.buffer.current++));
    }
}

/******************************************************************************
 *
 * Function _DtGrGetString
 *
 * Stream version of fgets, reads a string from a stream and advances the 
 * stream position.  If an error occurs or the end of the stream is 
 * encountered and no characters have been read, no characters are transferred
 * to the buffer and a NULL pointer is returned.  Otherwise, the buffer is
 * returned.
 *
 *****************************************************************************/
char *_DtGrGetString(
    char        *buffer,
    int         num_bytes,
    _DtGrStream *stream)
{
    char *p, *save = buffer;
    int i;

    if ((stream == NULL) || (stream->type == _DtGrNONE))
        return(NULL); /* Failure */

    if (stream->type == _DtGrFILE)
    {
        return (fgets(buffer,num_bytes,stream->source.file.fileptr));
    }
    else if (stream->type == _DtGrBUFFER)
    {
        /* This code mirrors that of fgets in libc */
        for (num_bytes--; num_bytes > 0; num_bytes -= i) 
        {
            if (stream->source.buffer.current > stream->source.buffer.end) 
            {
                if (stream->source.buffer.current == 
                    stream->source.buffer.end + 1)
                {
                    if (save == buffer)
                        return (NULL);
                    break; /* no more data */
                }
                stream->source.buffer.current--;
            }
            i = (num_bytes < stream->source.buffer.end - 
		             stream->source.buffer.current + 1 ? num_bytes :
                             stream->source.buffer.end - 
                             stream->source.buffer.current + 1);
            /* Copy the data into the buffer */
            if ((p = memccpy((void *)buffer,
			     (void *)stream->source.buffer.current,
			     (int)'\n',(size_t)i)) != NULL)
                i = p - buffer;
            buffer += i;
            stream->source.buffer.current += i;
            if (p != NULL)
                break; /* found '\n' in buffer */
        }
        *buffer = '\0';
        return (save);
    }
}
