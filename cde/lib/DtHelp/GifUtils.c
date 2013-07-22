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
/*
 * $XConsortium: GifUtils.c /main/5 1996/05/07 13:21:42 drk $
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */


/* +-------------------------------------------------------------------+ */
/* | Portions lifted from giftoppm.c (pbmplus version 10dec91)         | */
/* |                                                                   | */
/* | Copyright 1990, David Koblas.                                     | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */

#define C_Gif
#define L_Graphics
#define C_MessageMgr
#define L_Managers

/* include files */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <Xm/XmPrivate.h>
#include "GifUtilsI.h"

#ifndef __STDC__
#define debug1(s, x)  s <<  "x" << " = " << (x) << "\n"
#else
#define debug1(s, x)  s << #x << " = " << (x) << "\n"
#endif

#define MAX_GHANDS 16   /* maximum # of GRAF handles */

#define PPM_ASSIGN(p,red,grn,blu) (p) = ((pixel) (red) << 20) | ((pixel) (grn) << 10) | (pixel) (blu)
#define PPM_GETR(p) (((p) & 0x3ff00000) >> 20)
#define PPM_GETG(p) (((p) & 0xffc00) >> 10)
#define PPM_GETB(p) ((p) & 0x3ff)

#define	MAXCOLORMAPSIZE		256

#define	TRUE	1
#define	FALSE	0

#define CM_RED		0
#define CM_GREEN	1
#define CM_BLUE		2

#define	MAX_LWZ_BITS		12

#define INTERLACE		0x40
#define LOCALCOLORMAP	0x80
#define BitSet(byte, bit)	(((byte) & (bit)) == (bit))

#define LM_to_uint(a,b)			(((b)<<8)|(a))

typedef struct {
	unsigned int	Width;
	unsigned int	Height;
	unsigned char	ColorMap[3][MAXCOLORMAPSIZE];
	unsigned int	BitPixel;
	unsigned int	ColorResolution;
	unsigned int	Background;
	unsigned int	AspectRatio;
} GifScreenType;

typedef struct {
	int	transparent;
	int	delayTime;
	int	inputFlag;
	int	disposal;
} Gif89Type;

/*
 * This structure holds variables that were formerly global or static
 * within a function in the original giftoppm code.  They have been
 * moved into this object to ensure thread re-entrancy of the routines
 * that use them.  A unique instance of this object is created for each
 * thread and passed to the appropriate routines.
 */
typedef struct {
        /* Formerly global variables */
        GifScreenType    GifScreen;
        Gif89Type        Gif89;
        int              ZeroDataBlock;
        /* Formerly static variables declared in DoExtension */
        char             ext_buf[256];
        /* Formerly static variables declared in GetCode */
        unsigned char    buf[280];
        int              curbit, lastbit, done, last_byte;
        /* Formerly static variables declared in LWZReadByte*/
        int              fresh;
        int              code_size, set_code_size;
        int              max_code, max_code_size;
        int              firstcode, oldcode;
        int              clear_code, end_code;
        int              table[2][(1<< MAX_LWZ_BITS)];
        int              stack[(1<<(MAX_LWZ_BITS))*2], *sp;
} GifState;

/*********************/
static void
_gif_error( char *format, ... )
{
    va_list args;

    va_start( args, format );

    fprintf( stderr, "GifObject: " );
    (void) vfprintf( stderr, format, args );
    fputc( '\n', stderr );
    va_end( args );
    /*  throw (Exception()); */
#if 0
    abort();
#endif
}

/*********************/
static void
_gif_message( char *format, ... )
{
    va_list args;

    va_start( args, format );

    fprintf( stderr, "GifObject: " );
    (void) vfprintf( stderr, format, args );
    fputc( '\n', stderr );
    va_end( args );
}

/*********************/
static pixel **
_gif_allocarray( int cols, int rows, int size )
{
    char** its;
    int i;

    its = (char**) malloc( rows * sizeof(char*) );
    if ( its == (char**) 0 )
        _gif_error( "out of memory allocating an array", 0 );
    its[0] = (char*) malloc( rows * cols * size );
    if ( its[0] == (char*) 0 )
        _gif_error( "out of memory allocating an array", 0 );
    for ( i = 1; i < rows; ++i )
        its[i] = &(its[0][i * cols * size]);
    return (pixel **)its;
}

/*********************/
static int
GetDataBlock(byte **inbuf, unsigned char *buf, GifState *g)
{
    unsigned char    count;

    count = (*inbuf)[0];
    (*inbuf)++;

    g->ZeroDataBlock = count == 0;

    if (count) {
        memcpy (buf, *inbuf, count);
        *inbuf += count;
    }

    return count;
}

/*********************/
static int
ReadColorMap(byte **inbuf, int number, unsigned char buffer[3][MAXCOLORMAPSIZE])
{
    int              i;
    unsigned char    *rgb;

    for (i = 0; i < number; ++i) {
        rgb = (unsigned char *)*inbuf;
        *inbuf += 3;

        buffer[CM_RED][i] = rgb[0] ;
        buffer[CM_GREEN][i] = rgb[1] ;
        buffer[CM_BLUE][i] = rgb[2] ;
    }
    return FALSE;
}

/*********************/
static int
DoExtension(byte **inbuf, int label, GifState *g)
{
    char           *str;
    char           *buf = g->ext_buf;

    switch (label) {
    case 0x01:        /* Plain Text Extension */
        str = "Plain Text Extension";
#ifdef notdef
        if (GetDataBlock(inbuf, (unsigned char*) buf, g) == 0)
            ;

        lpos   = LM_to_uint(buf[0], buf[1]);
        tpos   = LM_to_uint(buf[2], buf[3]);
        width  = LM_to_uint(buf[4], buf[5]);
        height = LM_to_uint(buf[6], buf[7]);
        cellw  = buf[8];
        cellh  = buf[9];
        foreground = buf[10];
        background = buf[11];

        while (GetDataBlock(inbuf, (unsigned char*) buf, g) != 0) {
            PPM_ASSIGN(image[ypos][xpos],
                    cmap[CM_RED][v],
                    cmap[CM_GREEN][v],
                    cmap[CM_BLUE][v]);
            ++index;
        }

        return FALSE;
#else
        break;
#endif
    case 0xff:        /* Application Extension */
        str = "Application Extension";
        break;
    case 0xfe:        /* Comment Extension */
        str = "Comment Extension";
        while (GetDataBlock(inbuf, (unsigned char*) buf, g) != 0) {
#ifdef DEBUG
            _gif_message("gif comment: %s", buf );
#endif
        }
        return FALSE;
    case 0xf9:        /* Graphic Control Extension */
        str = "Graphic Control Extension";
        (void) GetDataBlock(inbuf, (unsigned char*) buf, g);
        g->Gif89.disposal    = (buf[0] >> 2) & 0x7;
        g->Gif89.inputFlag   = (buf[0] >> 1) & 0x1;
        g->Gif89.delayTime   = LM_to_uint(buf[1], buf[2]);
        if ((buf[0] & 0x1) != 0)
            g->Gif89.transparent = buf[3];

        while (GetDataBlock(inbuf, (unsigned char*) buf, g) != 0)
            ;
        return FALSE;
    default:
        str = buf;
        sprintf(buf, "UNKNOWN (0x%02x)", label);
        break;
    }

#ifdef DEBUG
    _gif_message("got a '%s' extension", str );
#endif

    while (GetDataBlock(inbuf, (unsigned char*) buf, g) != 0)
        ;

    return FALSE;
}

/*********************/
static int
GetCode(byte **inbuf, int code_size, int flag, GifState *g)
{
    int            i, j, ret;
    unsigned char  count;
    unsigned char  *buf = g->buf;

    if (flag) {
        for (i = 0; i < 280; i++)
                buf[i] = 0;
        g->last_byte = 0;
        g->curbit = 0;
        g->lastbit = 0;
        g->done = FALSE;
        return 0;
    }

    if ( (g->curbit+code_size) >= g->lastbit) {
        if (g->done) {
            if (g->curbit >= g->lastbit)
                _gif_error("ran off the end of my bits", 0 );
            return -1;
        }
        if (g->last_byte) {
                buf[0] = buf[g->last_byte-2];
                buf[1] = buf[g->last_byte-1];
        }

        if ((count = GetDataBlock(inbuf, &buf[2], g)) == 0)
            g->done = TRUE;

        g->last_byte = 2 + count;
        g->curbit = (g->curbit - g->lastbit) + 16;
        g->lastbit = (2+count)*8 ;
    }

    ret = 0;
    for (i = g->curbit, j = 0; j < code_size; ++i, ++j)
        ret |= ((buf[ i / 8 ] & (1 << (i % 8))) != 0) << j;

    g->curbit += code_size;

    return ret;
}

/*********************/
static int
LWZReadByte(byte **inbuf, int flag, int input_code_size, GifState *g)
{
    int        code, incode;
    register int    i;

    if (flag) {
        g->set_code_size = input_code_size;
        g->code_size = g->set_code_size+1;
        g->clear_code = 1 << g->set_code_size ;
        g->end_code = g->clear_code + 1;
        g->max_code_size = 2*g->clear_code;
        g->max_code = g->clear_code+2;

        GetCode(inbuf, 0, TRUE, g);
        
        g->fresh = TRUE;

        for (i = 0; i < g->clear_code; ++i) {
            g->table[0][i] = 0;
            g->table[1][i] = i;
        }
        for (; i < (1<<MAX_LWZ_BITS); ++i)
            g->table[0][i] = g->table[1][0] = 0;

        g->sp = g->stack;

        return 0;
    } else if (g->fresh) {
       g->fresh = FALSE;
        do {
            g->firstcode = g->oldcode =
                GetCode(inbuf, g->code_size, FALSE, g);
        } while (g->firstcode == g->clear_code);
        return g->firstcode;
    }

    if (g->sp > g->stack)
        return *--(g->sp);

    while ((code = GetCode(inbuf, g->code_size, FALSE, g)) >= 0) {
        if (code == g->clear_code) {
            for (i = 0; i < g->clear_code; ++i) {
                g->table[0][i] = 0;
                g->table[1][i] = i;
            }
            for (; i < (1<<MAX_LWZ_BITS); ++i)
                g->table[0][i] = g->table[1][i] = 0;
            g->code_size = g->set_code_size+1;
            g->max_code_size = 2*g->clear_code;
            g->max_code = g->clear_code+2;
            g->sp = g->stack;
            g->firstcode = g->oldcode =
                    GetCode(inbuf, g->code_size, FALSE, g);
            return g->firstcode;
        } else if (code == g->end_code) {
            int        count;
            unsigned char    buf[260];

            if (g->ZeroDataBlock)
                return -2;

            while ((count = GetDataBlock(inbuf, buf, g)) > 0)
                ;

#ifdef DEBUG
            if (count != 0)
                _gif_message("missing EOD in data stream (common occurence)");
#endif
            return -2;
        }

        incode = code;

        if (code >= g->max_code) {
            *(g->sp)++ = g->firstcode;
            code = g->oldcode;
        }

        while (code >= g->clear_code) {
            *(g->sp++) = g->table[1][code];
            if (code == g->table[0][code])
                _gif_error("circular table entry BIG ERROR", 0);
            code = g->table[0][code];
        }

        *(g->sp)++ = g->firstcode = g->table[1][code];

        if ((code = g->max_code) <(1<<MAX_LWZ_BITS)) {
            g->table[0][code] = g->oldcode;
            g->table[1][code] = g->firstcode;
            ++(g->max_code);
            if ((g->max_code >= g->max_code_size) &&
                (g->max_code_size < (1<<MAX_LWZ_BITS))) {
                g->max_code_size *= 2;
                ++(g->code_size);
            }
        }

        g->oldcode = incode;

        if (g->sp > g->stack)
            return *--(g->sp);
    }
    return code;
}

/*********************/
pixel **
ReadImage(byte **inbuf, int len, int height, unsigned char cmap[3][MAXCOLORMAPSIZE], int interlace, int ignore, GifState *g)
{
    unsigned char    c;    
    int              v;
    int              xpos = 0, ypos = 0, pass = 0;
    pixel            **image;

    /*
    **  Initialize the Compression routines
    */
    c = (*inbuf)[0];
    (*inbuf)++;

    if (LWZReadByte(inbuf, TRUE, c, g) < 0)
        _gif_error("error reading image", 0 );

    /*
    **  If this is an "uninteresting picture" ignore it.
    */
    if (ignore) {
#ifdef DEBUG
        _gif_message("skipping image..." );
#endif
        while (LWZReadByte(inbuf, FALSE, c, g) >= 0)
            ;
        return NULL;
    }

    if ((image = _gif_allocarray(len, height, sizeof(pixel))) == NULL)
        _gif_error("couldn't alloc space for image", 0 );

#ifdef DEBUG
    _gif_message("reading %d by %d%s GIF image",
                 len, height, interlace ? " interlaced" : "" );
#endif

    while ((v = LWZReadByte(inbuf,FALSE,c, g)) >= 0 ) {
        PPM_ASSIGN(image[ypos][xpos], cmap[CM_RED][v],
                    cmap[CM_GREEN][v], cmap[CM_BLUE][v]);

        ++xpos;
        if (xpos == len) {
            xpos = 0;
            if (interlace) {
                switch (pass) {
                case 0:
                case 1:
                    ypos += 8; break;
                case 2:
                    ypos += 4; break;
                case 3:
                    ypos += 2; break;
                }

                if (ypos >= height) {
                    ++pass;
                    switch (pass) {
                    case 1:
                        ypos = 4; break;
                    case 2:
                        ypos = 2; break;
                    case 3:
                        ypos = 1; break;
                    default:
                        goto fini;
                    }
                }
            } else {
                ++ypos;
            }
        }
        if (ypos >= height)
            break;
    }

fini:
#ifdef DEBUG
    if (LWZReadByte(inbuf,FALSE,c, g)>=0)
        _gif_message("too much input data, ignoring extra...");
#endif
    return image;

}


/* ////////////////////////////////////////////////////////////
// class constructor
// ////////////////////////////////////////////////////////// */

enum _DtGrLoadStatus InitGifObject (
    GifObj               *go, 
    Display              *dpy, 
    Drawable             drawable,
    Screen               *screen,
    int                  depth,
    Colormap             colormap,
    Visual               *visual,
    GC                   gc,
    enum _DtGrColorModel colorModel, 
    Boolean              allowReducedColors)
{
    int r, g, b, i, visualsMatched;
    XVisualInfo vTemplate, *visualList;

    /* 
    ** Initialize structure values
    */
    go->bits_per_pixel = 2;
    go->colors_per_pixel = (int) pow (2, go->bits_per_pixel);
    go->total_colors = (int) pow (go->colors_per_pixel, 3);
    go->f_color_map_constructed = 0;
    go->f_total_greys = 2;
    go->f_ximage    = NULL;
    go->f_dpy       = dpy;
    go->f_drawable  = drawable;
    go->f_screen    = XScreenNumberOfScreen(screen);
    go->f_dft_depth = depth;
    go->f_cmap      = colormap;
    go->f_gc        = gc;
    go->f_visual    = visual;
    go->f_ncells    = DisplayCells(go->f_dpy, go->f_screen);
    go->f_nplanes   = DisplayPlanes(go->f_dpy,go->f_screen);
    go->f_white     = WhitePixel(go->f_dpy, go->f_screen); 
    go->f_black     = BlackPixel(go->f_dpy, go->f_screen);
    go->f_allow_reduced_colors = allowReducedColors;
    go->f_color_reduction_used = FALSE;

    /*
    ** Initialize color allocation fields according to the color model
    ** specified by the caller.
    */
    switch (colorModel)
    {
        case _DtGrCOLOR:
             go->f_do_visual = DO_COLOR;
             go->f_init_total_greys = 32;
             break;
        case _DtGrGRAY_SCALE:
             go->f_do_visual = DO_GREY;
             go->f_init_total_greys = 32;
             break;
        case _DtGrBITONAL:
             go->f_do_visual = DO_GREY;
             go->f_init_total_greys = 2;
             break;
        default:
	    /* Should never get here */
             go->f_do_visual = DO_COLOR;
             go->f_init_total_greys = 32;
    }

    /* Return if the colormap is already allocated */
    if ( go->f_color_map_constructed )
        return (_DtGrSUCCESS);

    /* find the visual class code */

    vTemplate.screen = go->f_screen;
    vTemplate.depth = go->f_dft_depth;

    visualList = XGetVisualInfo( go->f_dpy, 
                                 VisualScreenMask | VisualDepthMask,
                                 &vTemplate, &visualsMatched );

    /* Return failure if we can't find a matching visual */
    if ( visualsMatched == 0 ) 
        return (_DtGrCONVERT_FAILURE);

    go->f_visual_class = StaticGray;

    for ( i=0; i<visualsMatched; i++ ) 
    {
        if ( visualList[i].visual == go->f_visual ) 
        {
            go->f_visual_class = visualList[i].class;
            break;
        }
    }

    XFree(visualList);

    /* Construct a 4x4x4 color cube */
    i = 0;
    for (r = 0; r < go->colors_per_pixel; r++)
        for (g = 0; g < go->colors_per_pixel; g++)
           for (b = 0; b < go->colors_per_pixel; b++)
           {
               go->GifCMap[i].red   = ((r * 65535)/(go->colors_per_pixel - 1));
               go->GifCMap[i].green = ((g * 65535)/(go->colors_per_pixel - 1));
               go->GifCMap[i].blue  = ((b * 65535)/(go->colors_per_pixel - 1));
               i++;
           }

    /* 
    ** Allocate X pixels, either color or greyscale values depending upon
    ** visual class and color model.
    */
    switch ( go->f_visual_class ) 
    {
        case StaticGray:
        case GrayScale:
        case StaticColor:
        {
            /*
	    ** Return failure if caller is insisting on color and this
            ** visual can't provide it.
            */
            if ((colorModel == _DtGrCOLOR) && !allowReducedColors)
                return (_DtGrCOLOR_FAILED);

            if ( allocate_greys(go) != 0 )
                return (_DtGrCOLOR_FAILED);

            break;
        }

        case PseudoColor:
        case DirectColor:
        case TrueColor:
        {
           if (colorModel == _DtGrCOLOR)
	   {
               if ( allocate_colors(go) != 0 )
                   return (_DtGrCOLOR_FAILED);
           }
           else
	   {
               if ( allocate_greys(go) != 0 )
                   return (_DtGrCOLOR_FAILED);
           }
           break;
        }

        default:
            return (_DtGrCONVERT_FAILURE);
    }

    /*
    ** Colors successfully allocated, return status code indicating
    ** whether we had to fallback to a degraded color model.
    */
    if (go->f_color_reduction_used)
        return (_DtGrCOLOR_REDUCE);
    else
        return (_DtGrSUCCESS);
}

/* /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////// */

void DeleteGifObjectResources(GifObj *g)
{
}

int allocate_colors(GifObj *g)
{
    int i, j;
    /*return allocate_greys(); // use this to test grey-scale */

   XColor color;
   unsigned long* colors;

   color.flags = DoRed | DoGreen | DoBlue;

   for (i = 0; i < g->total_colors; i++) {

      color.red   = g->GifCMap[i].red;
      color.green = g->GifCMap[i].green;
      color.blue  = g->GifCMap[i].blue;
    
      /*printf ("Allocating %3d: ", i); */
      if ( !XAllocColor (g->f_dpy, g->f_cmap, &color) ) {

	  /*puts ("FAILED!!!"); */
         colors = (unsigned long *) malloc (sizeof(unsigned long)  * i);
         for (j = 0; j < i; j++)
           colors[j] = g->GifCMap[j].pixel;

         /*cerr << "Xfree in allocate_colors(): " << i << "\n"; */
         XFreeColors (g->f_dpy, g->f_cmap, colors, i, 0);

         free(colors);

         /* fallback to greys */
         if (g->f_allow_reduced_colors)
	 {
             g->f_color_reduction_used = TRUE;
             return allocate_greys(g);
         }
         else
             return (_DtGrCOLOR_FAILED);
      }

      /*fprintf(stderr, "i=%d pixel=%d\n", i, color.pixel);*/

      /*printf ("@ %d\n", color.pixel); */
      g->GifCMap[i].pixel = color.pixel;
    }

   g->f_do_visual = DO_COLOR;
   g->f_color_map_constructed = 1;

   return 0;
}

int allocate_greys(GifObj *g)
{
   XColor color;
   int i, j;
   unsigned long* colors;

   color.flags = DoRed | DoGreen | DoBlue;

   for ( i=0; i<g->total_colors; i++ ) {

/*
debug1(cerr, i);
debug1(cerr, GifCMap[i].red);
debug1(cerr, GifCMap[i].green);
debug1(cerr, GifCMap[i].blue);
debug1(cerr, 0.299 * GifCMap[i].red + 0.587 * GifCMap[i].green +
                                         0.114 * GifCMap[i].blue);
debug1(cerr, GifCMap[i].grey);
*/

      g->GifCMap[i].grey = (unsigned short)(0.299 * g->GifCMap[i].red +     
                                         0.587 * g->GifCMap[i].green +     
                                         0.114 * g->GifCMap[i].blue);     
   }

/*   
   if ( StaticColor == g->f_visual_class ||
        TrueColor == g->f_visual_class ) 
*/
   if ( StaticColor == g->f_visual_class)
   {
      g->f_do_visual = DO_GREY;
      g->f_total_greys = 2;
      return 0;
   }


   for ( g->f_total_greys=g->f_init_total_greys; g->f_total_greys>=2; 
         g->f_total_greys/=2 ) 
   {
      /*fprintf(stderr, "f_total_greys = %d\n", g->f_total_greys); */

      /*
      ** Return failure if we're about to downgrade from greyscale
      ** to dithered monochrome and we don't allow reduced colors.
      */ 
      if ((g->f_total_greys == 2) && (g->f_init_total_greys > 2) &&
          (!g->f_allow_reduced_colors))
          return -1;

      for (i = 0; i<g->f_total_greys; i++) {

         color.red = 
         color.green = 
         color.blue  = (i*65535)/(g->f_total_greys - 1);
   
         /*fprintf (stderr, "Allocating %3d: ", i);*/
         if ( !XAllocColor (g->f_dpy, g->f_cmap, &color) ) {

	     /*fprintf(stderr, "alloc Grey FAILED!!!");*/
            colors = (unsigned long *) malloc (sizeof(unsigned long)  * i);
            for (j = 0; j < i; j++)
              colors[j] = g->GifGMap[j];

            /*cerr << "Xfree in allocate_greys()\n"; */
            XFreeColors (g->f_dpy, g->f_cmap, colors, i, 0);

            free(colors);
     
            break;
         }

         /*printf ("@ %d\n", color.pixel); */
         g->GifGMap[i] = color.pixel;
      }

      if ( i == g->f_total_greys ) {

/*
for ( int l=0; l<i; l++ )
cerr << "GifGMap[l]= " << GifGMap[l] << "\n";
*/

         g->f_color_map_constructed = 1;
         g->f_do_visual = DO_GREY;
         /* If greyscape was downgraded to bitonal, record the fact */
         if ((g->f_total_greys == 2) && (g->f_init_total_greys > 2))
             g->f_color_reduction_used = TRUE;
         return 0;
      }
   }

   return -1;
}

/* ////////////////////////////////////////////////////////////
// Free allocated raw image data
// ////////////////////////////////////////////////////////// */

void
free_raw_image( pixel **image )
{
    free( (char *)image[0] );
    free( (char *)image );
}

/* ////////////////////////////////////////////////////////////
// Decompress GIF data into raw bytes
// ////////////////////////////////////////////////////////// */

pixel **
create_raw_image( byte *inbuf, unsigned int buflen, int *width, int *height, int imageNumber )
{
    unsigned char    *buf;
    unsigned char    *start_of_buf = inbuf;
    unsigned char    c;
    unsigned char    localColorMap[3][MAXCOLORMAPSIZE];
    int              useGlobalColormap;
    int              bitPixel;
    int              imageCount = 0;
    char             version[4];
    pixel            **image;
    GifState         g;

    /* Initialize the GIF state object */
    g.Gif89.transparent = g.Gif89.delayTime = g.Gif89.inputFlag = -1;
    g.Gif89.disposal = 0;
    g.ZeroDataBlock = FALSE;
    g.fresh = FALSE;

    /*  XXXSWM -- hack */
    if (buflen < 13) {
	fprintf (stderr, "Not GIF Data, buffer too small\n");
	return NULL;
    }

    buf = (unsigned char *)inbuf;
    inbuf += 6;

    if (strncmp((const char *)buf,"GIF",3) != 0) {
        fprintf( stderr, "GifObject: not GIF data\n" );
        return NULL;
    }

    strncpy(version, (const char *)(buf + 3), 3);
    version[3] = '\0';

    if ((strcmp(version, "87a") != 0) && (strcmp(version, "89a") != 0)) {
        fprintf (stderr, "bad version number, not '87a' or '89a'\n" );
	return NULL;
    }

    buf = (unsigned char *)inbuf;
    inbuf += 7;

    g.GifScreen.Width           = LM_to_uint(buf[0],buf[1]);
    g.GifScreen.Height          = LM_to_uint(buf[2],buf[3]);
    g.GifScreen.BitPixel        = 2<<(buf[4]&0x07);
    g.GifScreen.ColorResolution = (((buf[4]&0x70)>>3)+1);
    g.GifScreen.Background      = buf[5];
    g.GifScreen.AspectRatio     = buf[6];

    if (BitSet(buf[4], LOCALCOLORMAP)) {    /* Global Colormap */
        if (ReadColorMap(&inbuf,g.GifScreen.BitPixel,g.GifScreen.ColorMap)) {
            fprintf (stderr, "error reading global colormap\n" );
	    return NULL;
	}
    }

    if (g.GifScreen.AspectRatio != 0 && g.GifScreen.AspectRatio != 49) {
        float    r;
        r = ( (float) g.GifScreen.AspectRatio + 15.0 ) / 64.0;
#ifdef DEBUG
        _gif_message("warning - non-square pixels; to fix do a 'pnmscale -%cscale %g'",
            r < 1.0 ? 'x' : 'y',
            r < 1.0 ? 1.0 / r : r );
#endif
    }

    image = NULL;

    for (;;) {
	if (inbuf - start_of_buf >= buflen) {
	    fprintf (stderr, "Premature EOF in GIF data\n");
	    return NULL;
	}
        c = inbuf[0];
        inbuf++;

        if (c == ';') {        /* GIF terminator */
            if (imageCount < imageNumber)
                _gif_error("only %d image%s found in file",
                     imageCount, imageCount>1?"s":"" );
            return image;
        }

        if (c == '!') {     /* Extension */
	    if (inbuf - start_of_buf >= buflen) {
		fprintf  (stderr, "Premature EOF in GIF data\n");
		return NULL;
	    }
            c = inbuf[0];
            inbuf++;
            DoExtension(&inbuf, c, &g);
            continue;
        }

        if (c != ',') {        /* Not a valid start character */
#ifdef DEBUG
            _gif_message("bogus character 0x%02x, ignoring", (int) c );
#endif
            continue;
        }

        ++imageCount;

        buf = (unsigned char *)inbuf;
        inbuf += 9;

        useGlobalColormap = ! BitSet(buf[8], LOCALCOLORMAP);

        bitPixel = 1<<((buf[8]&0x07)+1);

        *width = LM_to_uint(buf[4],buf[5]);
        *height = LM_to_uint(buf[6],buf[7]);

        if (! useGlobalColormap) {
            if (ReadColorMap(&inbuf, bitPixel, localColorMap))
                _gif_error("error reading local colormap", 0 );
            image = ReadImage(&inbuf, *width, *height, localColorMap,
                       BitSet(buf[8], INTERLACE), imageCount != imageNumber,
                       &g);
        } else {
            image = ReadImage(&inbuf, *width, *height, g.GifScreen.ColorMap,
                       BitSet(buf[8], INTERLACE), imageCount != imageNumber,
                       &g);
        }

    }
}

/* ////////////////////////////////////////////////////////////
// Create X pixmap from raw image data
// ////////////////////////////////////////////////////////// */

/* from "Computer Graphics" by Foley, VanDam, Feiner, Hughes */
/*       2nd edition */
static int dither_8X8[8][8] = 
{
 { 0,  32,  8, 40,  2, 34, 10, 42 },
 { 48, 16, 56, 24, 50, 18, 58, 26 },
 { 42, 44,  4, 36, 14, 46,  6, 38 },
 { 60, 28, 52, 20, 62, 30, 54, 22 },
 { 3,  35, 11, 43,  1, 33,  9, 41 },
 { 51, 19, 59, 27, 49, 17, 57, 25 },
 { 15, 47,  7, 39, 13, 45,  5, 37 },
 { 63, 31, 55, 23, 61, 29, 53, 21 }
};

/*static int dither_6X6[6][6] =
//{
// { 24, 32, 16, 26, 34, 18},
// {  4,  0, 12,  6, 2,  14},
// { 20,  8, 28, 22, 10, 30},
// { 27, 35, 19, 25, 33, 17},
// {  7,  3, 15,  5,  1, 13},
// { 23, 11, 31, 21,  9, 29}
//};
*/

/*
static int dither_4X4[4][4] = 
{
 { 0, 8, 2, 10 },
 { 12, 4, 14, 6},
 { 3, 11, 1, 9},
 { 15, 7, 13, 5}
};


static int dither_3X3[3][3] = 
{
 { 6, 8, 4 },
 { 1, 0, 3},
 { 5, 2, 7}
};


static int dither_2X2[2][2] = 
{
 { 0, 2 },
 { 3, 1}
};
*/

/*static int dither_matrix_sz = 2; */
/*static int dither_matrix_sz = 4; */
/*static int dither_matrix_sz = 3; */
static int dither_matrix_sz = 8;

/* call XListPixFormat() to get bits/pix and pads ? */
Pixmap
create_pixmap( GifObj *g, pixel **image, int width, int height, Pixel fg, Pixel bg, float ratio)
{
  int nullCount = (4 - (width % 4)) & 0x03;
  int ximWidth = width + nullCount; 
  byte *ximData = 0;
  pixel *ipp = *image;
  int spacing;
  long pixval;
  int x, y;
  int index;
  Pixmap pm;
  int scaledWidth, scaledHeight;

  if ( g->f_nplanes > 8 )
     ximData = (byte *) malloc(ximWidth * height * 4 );
  else
     ximData = (byte *) malloc(ximWidth * height );

  if (!ximData) {
    fprintf(stderr, "Could not allocate ximage data\n");
    return None;
  }

  /* Monochrome */
  if (g->f_nplanes == 1)
     g->f_ximage = XCreateImage(g->f_dpy, g->f_visual, g->f_nplanes, XYPixmap,
                          0, (char *)ximData, width, height, 32, 0);
  /* 8 bit color */
  /*else if (g->f_nplanes == 8) */

  /* non-mono display */
  else  
     g->f_ximage = XCreateImage(g->f_dpy, g->f_visual, g->f_nplanes, ZPixmap,
                          0, (char *)ximData, width, height, 32, 0);

  if (!g->f_ximage) {
    fprintf(stderr, "XCreateImage failed\n");
    return None;
  }


/* RGB to Pixel Conversion */

  if ( g->f_total_greys == 2 ) 
     spacing = 65536 / (dither_matrix_sz * dither_matrix_sz);
  else
     spacing = 65536 / g->f_total_greys;

/*cerr << "spacing" << spacing << "\n"; */

  for (y=0; y < height; y++) {
    for (x=0; x < width; x++) {
      pixval = (long)*ipp; 

/*      XColor cellDef; */
/*      cellDef.red   = (short)PPM_GETR(pixval);    */
/*      cellDef.green = (short)PPM_GETG(pixval);    */
/*      cellDef.blue  = (short)PPM_GETB(pixval);    */

          index = (((short)PPM_GETR(pixval))/64)*16 +
                  (((short)PPM_GETG(pixval))/64)*4  +
                  ((short)PPM_GETB(pixval))/64;

/*fprintf(stderr, "grey= %d, grey/space=%d\n", g->GifCMap[index].grey, g->GifCMap[index].grey / spacing);*/
      switch (g->f_do_visual) {
        case DO_GREY:

         switch ( g->f_total_greys ) {
          case 2:

/*cerr << "index=" << index << "\n"; */
/*cerr << "GifCMap[index].grey" << GifCMap[index].grey << "\n"; */
/*cerr << "GifCMap[index].grey/spacing" << GifCMap[index].grey / spacing << "\n"; */

            if ( dither_8X8[x%dither_matrix_sz][y%dither_matrix_sz] < g->GifCMap[index].grey / spacing ) {
               XPutPixel(g->f_ximage,x,y,g->f_white);
            } else {
               XPutPixel(g->f_ximage,x,y,g->f_black);
            }

            break;


         default:
/*cerr << GifCMap[index].grey / spacing << " "; */

           XPutPixel(g->f_ximage,x,y, g->GifGMap[g->GifCMap[index].grey / spacing]);
         }
         
         break;


        case DO_COLOR:
#ifdef FLOOD
fprintf(stderr, "%03d %03d %03d -- %03d %03d %03d ",
(short)PPM_GETR(pixval),(short)PPM_GETG(pixval),(short)PPM_GETB(pixval),
g->GifCMap[index].red>>8, g->GifCMap[index].green>>8, g->GifCMap[index].blue>>8
                            );
if ((short)PPM_GETR(pixval) != (GifCMap[index].red>>8) || 
    (short)PPM_GETG(pixval) != (GifCMap[index].green>>8) || 
    (short)PPM_GETB(pixval) != (GifCMap[index].blue>>8))
  puts (" *");
else
  puts (" -"); 
#endif


             XPutPixel(g->f_ximage,x,y, g->GifCMap[index].pixel);
/*
         switch ( g->f_visual_class ) {
          case StaticColor:
          case PseudoColor:
             XPutPixel(g->f_ximage,x,y, g->GifCMap[index].pixel);
             break;

          case TrueColor:
          case DirectColor:
             break;

         }
*/

       }

      ipp++;
    }
    for (x=width;x<ximWidth;x++)
      XPutPixel(g->f_ximage,x,y,bg);   /* padding */
  }

  scaledWidth = width * ratio + 0.5;
  scaledHeight = height * ratio + 0.5;
  if (scaledWidth == 0)
      scaledWidth = 1;
  if (scaledHeight == 0)
     scaledHeight = 1; 
  pm = XCreatePixmap(g->f_dpy,g->f_drawable,
		     scaledWidth,scaledHeight,
		     g->f_nplanes);

  if (!pm) {
    fprintf(stderr, "could not create pixmap\n");
    return None;
  }

  _XmPutScaledImage (g->f_dpy,pm,g->f_gc,g->f_ximage,
		     0,0,0,0,width,height,
		     scaledWidth,scaledHeight);

  XDestroyImage(g->f_ximage);
  g->f_ximage = NULL;
  
  return(pm);
}


/* /////////////////////////////////////////////////////////////////
// Load pixmap from GIF data
// /////////////////////////////////////////////////////////////// */

Pixmap
gif_to_pixmap(GifObj *g, byte *inbuf, unsigned int buflen, Dimension *w, Dimension *h, Pixel fg, Pixel bg, float ratio)
{
  Pixmap pixmap;
  pixel  **raw_image;
  int    width, height;

  /* Create raw image from compress GIF data */
  raw_image = create_raw_image (inbuf, buflen, &width, &height, 1);
  if (!raw_image) return None;

  /* Create X pixmap from raw image data */
  pixmap = create_pixmap(g, raw_image, width, height, fg, bg, ratio);

  /* Free raw image data */
  free_raw_image(raw_image);

  /* Set X pixmap dimensions */
  *w = (Dimension) width * ratio + 0.5;
  *h = (Dimension) height * ratio + 0.5;
  if (*w == 0)
      *w = 1;
  if (*h == 0)
      *h = 1;
  /* Return X pixmap */
  return pixmap;
}
