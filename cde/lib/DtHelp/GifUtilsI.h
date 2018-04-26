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
/* $XConsortium: GifUtilsI.h /main/3 1996/05/09 03:42:55 drk $ */
#ifndef _DtGifUtilsI_h
#define _DtGifUtilsI_h

/* Include files */
#include <X11/Xlib.h>
#include "GraphicsP.h"

/* Enumerated values */
enum op_t { DO_COLOR, DO_GREY };

/* Type definitions */
typedef unsigned char byte;
typedef unsigned long pixel;

/* Data structures */

typedef struct 
  {
    unsigned long pixel;
    unsigned short red, green, blue;
    unsigned short grey;
  } GifColors;

typedef struct 
{
  XImage        *f_ximage;
  Display       *f_dpy;
  int            f_screen;
  Colormap       f_cmap;
  Drawable       f_drawable;
  GC             f_gc;
  Visual        *f_visual;
  unsigned int   f_ncells;
  unsigned int   f_nplanes;
  unsigned long  f_black;
  unsigned long  f_white;
  unsigned long  f_fg;
  unsigned long  f_bg;
  int		 f_dft_depth;
  int		 f_visual_class;
  int            f_color_map_constructed;
  int            f_do_visual;
  int            bits_per_pixel ;  
  int            colors_per_pixel;
  int            total_colors ; 
  int            f_total_greys;
  int            f_init_total_greys;  
  Boolean        f_allow_reduced_colors;
  Boolean        f_color_reduction_used;     
  GifColors      GifCMap[64];
  unsigned long  GifGMap[32];
} GifObj;

/* Function prototypes */

/* Initializes a gif object structure */
enum _DtGrLoadStatus InitGifObject( 
    GifObj                *g, 
    Display               *dpy, 
    Drawable               drawable, 
    Screen                *screen,
    int                    depth, 
    Colormap               colormap, 
    Visual                *visual, 
    GC                     gc,
    enum _DtGrColorModel   colorModel, 
    Boolean                allowReducedColors
);

/* Deletes resources associated with a gif object structure */
void DeleteGifObjectResources(
    GifObj *g 
);

/* Converts a gif buffer to an X pixmap */
Pixmap gif_to_pixmap(
    GifObj        *g, 
    byte          *inbuf, 
    unsigned int   buflen, 
    Dimension     *w, 
    Dimension     *h, 
    Pixel          fg, 
    Pixel          bg,
    float          ratio
);

/* Creates a raw PPM-style image from a GIF buffer */
pixel **create_raw_image(
    byte          *inbuf, 
    unsigned int   buflen, 
    int           *width, 
    int           *height, 
    int            imageNumber
);

/* Creates an X pixmap from a raw PPM-style image */
Pixmap create_pixmap(
    GifObj    *g, 
    pixel   **image, 
    int        width, 
    int        height, 
    Pixel      fg, 
    Pixel      bg,
    float      ratio
);

/* Frees raw image data */
void free_raw_image(
    pixel  **image
);

/* Allocates X pixels needed for the color cube */
int allocate_colors(
    GifObj  *g
);

/* Allocates X pixels needed for greyscale rendering */
int allocate_greys(
    GifObj  *g
);

#endif /* _DtGifUtilsI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
