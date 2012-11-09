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
/* $XConsortium: JpegUtils.c /main/3 1996/10/06 19:38:48 rws $ */
/*
** JpegUtils.c
**
** This module provides utilities for converting jpeg data associated with
** a _DtGrStream into an XImage.  The module is based on code from several 
** of the files from the Independent JPEG library, version 6a (copyright 
** attached below).
*/

/*
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 */

/*
** Include files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include "GraphicsP.h"
#include "jpeglib.h"
#include "cdjpeg.h"
#include "JpegUtilsI.h"

/*
** Macro definitions
*/

#define BYTESPERSAMPLE 1
#define MAX_COLORS 64        /* Not allowed to be higher than 256 */
#define BYTE_MAXVAL 0xFF
#define XCOL_MAXVAL 0xFFFF
#define INTERP_TO_XCOLORSPACE(val) ((XCOL_MAXVAL/BYTE_MAXVAL) * val)
#define INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */

/*
** Type definitions
*/

/* 
** Custom destination manager structure 
*/
typedef struct {
  struct djpeg_dest_struct pub;	/* public fields */
  JSAMPROW pixrow;		/* decompressor output buffer */
  XImage *ximage;               /* XImage to put pixel info into */
} ximg_dest_struct;

typedef ximg_dest_struct * ximg_dest_ptr;

/* 
** Custom source manager structure 
*/
typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */

  _DtGrStream *stream;          /* source stream */
  unsigned long input_buf_size; /* size of input buffer */
  JOCTET * buffer;		/* start of buffer */
  boolean start_of_file;	/* have we gotten any data yet? */
} stream_source_mgr;

typedef stream_source_mgr * stream_src_ptr;

/* 
** Custom error handler structure
*/

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
** Routines
*/

/******************************************************************************
 *
 * Function my_error_exit
 *
 * This routine replaces the standard fatal error handling routine.  Instead
 * of exiting the program, the routine longjmps back to jpeg_to_ximage,
 * which cleans up and returns a _DtGrCONVERT_FAILURE.
 *
 *****************************************************************************/
static void my_error_exit (j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    (*cinfo->err->output_message) (cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

/******************************************************************************
 *
 * Function my_output_message
 *
 * This routine replaces the standard error message outputter.  Instead
 * of outputting JPEG library error and warning messages to stderr, it
 * suppresses them.
 *
 *****************************************************************************/
static void my_output_message (j_common_ptr cinfo)
{
    /* 
    ** Uncommenting the lines below will cause error and warning messages
    ** from the JPEG library to be displayed to stderr instead of suppressed
    */

    /*
    ** char buffer[JMSG_LENGTH_MAX];
    */

    /* 
    ** Create the message 
    **
    ** (*cinfo->err->format_message) (cinfo, buffer); 
    */

    /* 
    ** Send it to stderr, adding a newline 
    **
    ** fprintf(stderr, "%s\n", buffer);  
    */
}

/******************************************************************************
 *
 * Function init_source
 *
 * This is the custom source manager's initialization routine, called by 
 * jpeg_read_header before any data is actually read.  It currently does
 * nothing.
 *
 *****************************************************************************/
static void init_source (
    j_decompress_ptr cinfo)
{
/*    stream_src_ptr src = (stream_src_ptr) cinfo->src; */
}

/******************************************************************************
 *
 * Function fill_input_buffer
 *
 * This is the custom source manager's fill input buffer routine, called by
 * the JPEG library whenever it has finished processing the data in the 
 * source buffer and needs it to be refilled with the next chunk of data.
 *
 *****************************************************************************/
static boolean fill_input_buffer (
    j_decompress_ptr cinfo)
{
    stream_src_ptr src = (stream_src_ptr) cinfo->src;
    size_t nbytes;

    /*
    ** Since we always process buffer-based streams in a single chunk, the
    ** only reason read_input_buffer should be called for one is if the data
    ** in the buffer was truncated or otherwise bogus.  If this is the case
    ** we set nbytes to zero, allocate a new buffer to hold a fake EOI marker
    ** so the stream buffer isn't overwritten, and let the error-handling 
    ** code below take care of things.
    */
    if (src->stream->type == _DtGrBUFFER)
    {
        nbytes = 0;
        src->buffer = (JOCTET *) (*cinfo->mem->alloc_small) (
                                          (j_common_ptr) cinfo, JPOOL_IMAGE,
				          2 * SIZEOF(JOCTET));
    }
    else /* _DtGrFILE, read the next chunk of data */
        nbytes = _DtGrRead (src->buffer, 1, src->input_buf_size, src->stream);
 
    if (nbytes <= 0) 
    {
        if (src->start_of_file)	/* Treat empty input file as fatal error */
            ERREXIT(cinfo, JERR_INPUT_EMPTY);
        WARNMS(cinfo, JWRN_JPEG_EOF);
        /* Insert a fake EOI marker */
        src->buffer[0] = (JOCTET) 0xFF;
        src->buffer[1] = (JOCTET) JPEG_EOI;
        nbytes = 2;
    }

    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;
    src->start_of_file = FALSE;

    return TRUE;
}

/******************************************************************************
 *
 * Function skip_input_data
 *
 * This is the custom source manager's skip input data function, called
 * by the JPEG library when it wants to skip over a potentially large
 * amount of uninteresting data (such as an APPn marker).
 *
 *****************************************************************************/
static void skip_input_data (
    j_decompress_ptr cinfo, 
    long num_bytes)
{
    stream_src_ptr src = (stream_src_ptr) cinfo->src;

    /* Just a dumb implementation for now.  Could use fseek() except
    ** it doesn't work on pipes.  Not clear that being smart is worth
    ** any trouble anyway --- large skips are infrequent.
    */
    if (num_bytes > 0) 
    {
        while (num_bytes > (long) src->pub.bytes_in_buffer) 
        {
            num_bytes -= (long) src->pub.bytes_in_buffer;
            (void) fill_input_buffer(cinfo);
            /* note we assume that fill_input_buffer will never return FALSE,
	    ** so suspension need not be handled.
            */
        }
        src->pub.next_input_byte += (size_t) num_bytes;
        src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
}

/******************************************************************************
 *
 * Function term_source
 *
 * This is the custom source manager's termination routine, it currently
 * does nothing.
 *
 *****************************************************************************/
static void term_source (
    j_decompress_ptr cinfo)
{
  /* no work necessary here */
  return;
}

/******************************************************************************
 *
 * Function jpeg_stream_src
 *
 * This is the custom source manager's creation routine.  Most of the custom
 * source manager code is based on the JPEG library's stdio source manager
 * code in the jdatasrc.c file included with the JPEG library distribution.
 *
 *****************************************************************************/
static void jpeg_stream_src (
    j_decompress_ptr cinfo, 
    _DtGrStream *stream)
{
    stream_src_ptr src;

    /*
    ** Create the custom source manager structure
    */
    cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small) (
                                            (j_common_ptr) cinfo, JPOOL_IMAGE,
				            SIZEOF(stream_source_mgr));
    src = (stream_src_ptr) cinfo->src;

    src->stream = stream;

    /*
    ** If this is a file-based stream, we need to allocate a buffer to
    ** read data into.  If this is a buffer-based stream, we just use the
    ** buffer already attached to the stream.  Note that this implies that
    ** we always process buffer-based streams in a single chunk, if there
    ** is ever a reason to do otherwise, this routine and fill_input_buffer 
    ** will need to be modified appropriately.
    */
    if (stream->type == _DtGrFILE)
    {
        src->buffer = (JOCTET *) (*cinfo->mem->alloc_small) (
                                        (j_common_ptr) cinfo, JPOOL_IMAGE,
				        INPUT_BUF_SIZE * SIZEOF(JOCTET));
        src->input_buf_size = INPUT_BUF_SIZE;
        src->pub.bytes_in_buffer = 0;    /* forces fill_input_buffer call */
        src->pub.next_input_byte = NULL; /* on first read                 */
        src->start_of_file = TRUE;
    }
    else /* _DtGrBUFFER */
    {
        src->buffer = (unsigned char *) stream->source.buffer.base;
        src->input_buf_size = stream->source.buffer.size;
        src->pub.bytes_in_buffer = src->input_buf_size;
        src->pub.next_input_byte = src->buffer;
        src->start_of_file = FALSE;
    }

    /*
    ** Initialize the method procedures
    */
    src->pub.init_source = init_source;
    src->pub.fill_input_buffer = fill_input_buffer;
    src->pub.skip_input_data = skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart; /* default method */
    src->pub.term_source = term_source;
}

/******************************************************************************
 *
 * Function copy_pixels
 *
 * This routine loops through a scanline of decompressed, quantized
 * JPEG data and uses XPutPixel to copy the pixel values into the 
 * XImage associated with the destination manager.
 *
 *****************************************************************************/
static void copy_pixels(
    j_decompress_ptr cinfo, 
    djpeg_dest_ptr   dinfo,
    JDIMENSION       rows_supplied)
{
    register int pixval;
    register JSAMPROW ptr;
    register JDIMENSION col;
    ximg_dest_ptr dest = (ximg_dest_ptr) dinfo;

    ptr = dest->pub.buffer[0];
    for (col=0; col < cinfo->output_width; col++) 
    {
        pixval = GETJSAMPLE(*ptr++);
        XPutPixel(dest->ximage,col,cinfo->output_scanline-1, pixval);
    }
}


/******************************************************************************
 *
 * Function start_output_ximg
 *
 * This is the data destination manager startup routine, it currently does
 * nothing.
 *
 *****************************************************************************/
static void start_output_ximg (
    j_decompress_ptr cinfo, 
    djpeg_dest_ptr dinfo)
{
    return;
}

/******************************************************************************
 *
 * Function finish_output_ximg
 *
 * This is the data destination manager shutdown routine, it currently does 
 * nothing.
 *
 *****************************************************************************/
static void finish_output_ximg (j_decompress_ptr cinfo, djpeg_dest_ptr dinfo)
{
}

/******************************************************************************
 *
 * Function init_jpeg_dest_mgr
 *
 * This routine allocates and initializes a data destination manager
 * that the JPEG library will use as a sink for JPEG data after the
 * data has been decompressed.
 *
 *****************************************************************************/
djpeg_dest_ptr init_jpeg_dest_mgr (
    j_decompress_ptr cinfo)
{
    ximg_dest_ptr dest;

    /* 
    ** Create module interface object, fill in the method pointers 
    */
    dest = (ximg_dest_ptr) (*cinfo->mem->alloc_small) (
                                            (j_common_ptr) cinfo, JPOOL_IMAGE, 
                                            SIZEOF(ximg_dest_struct));
    dest->pub.start_output = start_output_ximg;
    dest->pub.finish_output = finish_output_ximg;
    dest->pub.put_pixel_rows = copy_pixels;

    /*
    ** Calculate the output image dimensions so we can allocate the
    ** right amount of space 
    */
    jpeg_calc_output_dimensions(cinfo);

    /*
    ** Create a buffer for the JPEG library to write decompressed
    ** scanline data into.
    */
    dest->pixrow = (JSAMPROW) (*cinfo->mem->alloc_small) (
                          (j_common_ptr) cinfo, JPOOL_IMAGE,
                          cinfo->output_width * cinfo->out_color_components );
    dest->pub.buffer = &dest->pixrow;
    dest->pub.buffer_height = 1;

    /*
    ** Return the initialized destination manager 
    */
    return (djpeg_dest_ptr) dest;
}

/******************************************************************************
 *
 * Function jpeg_to_ximage
 *
 * This routine converts compressed jpeg data associated with a _DtGrStream 
 * into an XImage.  
 *
 * No X color allocation is done.  The image is quantized down to MAX_COLORS 
 * during decompression, and an array of XColor structures with the red, 
 * green, and blue fields initialized to the colors used in the image is
 * returned to the caller.  Each pixel value in the XImage data is an index
 * into this array.  The caller must use this information to allocate X
 * color cells and substitute the appropriate pixel values into the XImage
 * data array before using the XImage.
 *
 * The routine makes use of a custom source data manager to allow the JPEG
 * data source to be a _DtGrStream, a custom data destination manager to 
 * allow the decompressed and post-processed data to be written to an XImage,
 * and a custom error handler to allow standard _DtGr error codes to be
 * returned to the caller in the event of a JPEG library error.
 *
 *****************************************************************************/
enum _DtGrLoadStatus jpeg_to_ximage (
    _DtGrStream           *stream,
    Screen                *screen,
    Visual                *visual,
    Dimension             *in_out_width,
    Dimension             *in_out_height,
    XImage                **ximage,
    XColor                **xcolors,
    int                    *ncolors,
    int                    *xres)
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    djpeg_dest_ptr dest_mgr = NULL;
    ximg_dest_ptr dest;
    int i, num_scanlines, nullCount, ximWidth;
    unsigned char *ximData = NULL;
    Display *display = DisplayOfScreen(screen);
    int nplanes = DisplayPlanes(display,XScreenNumberOfScreen(screen));
    XColor *colors = NULL;

    /*
    ** Initialize the return values 
    */
    *ximage = NULL;
    *xres = *ncolors = *in_out_width = *in_out_height = 0;

    /*
    ** Initialize the jpeg library error handler with our custom routines
    */

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    jerr.pub.output_message = my_output_message;

    /* 
    ** Establish the setjmp return context for my_error_exit to use
    */
    if (setjmp(jerr.setjmp_buffer)) 
    {
        /* If we get here, the JPEG code has signaled an error.  We need to
        ** free memory, clean up the JPEG object, and return a failure code.
        */
        if (*ximage != NULL)
            XDestroyImage (*ximage);
        if (colors != NULL)
            free (colors);
        jpeg_destroy_decompress(&cinfo);
        return (_DtGrCONVERT_FAILURE);
    }

    /*
    ** Create a jpeg decompression object
    */
    jpeg_create_decompress(&cinfo);

    /*
    ** Create a custom source data manager
    */
    jpeg_stream_src(&cinfo, stream);

    /*
    ** Read the jpeg header 
    */
    jpeg_read_header(&cinfo, TRUE);
    if (cinfo.X_density > 0 &&
	(cinfo.density_unit == 1 || cinfo.density_unit == 2)) {
	if (cinfo.density_unit == 1)
	    *xres = cinfo.X_density;
	else
	    *xres = cinfo.X_density * 2.54 + 0.5;
    }

    /*
    ** Initialize our desired post-processing attributes
    */
    cinfo.quantize_colors = TRUE;
    cinfo.desired_number_of_colors = MAX_COLORS;

    /*
    ** Create a custom data destination manager to allow our processed data
    ** to be channeled into an XImage.
    */
    dest_mgr = init_jpeg_dest_mgr(&cinfo);

    /*
    ** Initialize the decompression state
    */
    jpeg_start_decompress(&cinfo);
    (*dest_mgr->start_output) (&cinfo, dest_mgr);

    /*
    ** Create an XImage to hold the processed data
    */
        
    nullCount = (4 - (cinfo.output_width % 4)) & 0x03;
    ximWidth = cinfo.output_width + nullCount;

    if (nplanes > 8 )
        ximData = (unsigned char *) malloc(ximWidth * 
                                           cinfo.output_height * 4 );
    else
        ximData = (unsigned char *) malloc(ximWidth * cinfo.output_height );

    if (!ximData) 
    {
        jpeg_destroy_decompress(&cinfo);
        return (_DtGrNO_MEMORY);
    }

    *ximage = XCreateImage(display, visual, nplanes,
                           (nplanes == 1) ? XYPixmap : ZPixmap, 
                           0, (char *)ximData, cinfo.output_width, 
                           cinfo.output_height, 32, 0);

    if (!*ximage) 
    {
        free (ximData);
        jpeg_destroy_decompress(&cinfo);
        return (_DtGrCONVERT_FAILURE);
    }

    /*
    ** Store the XImage in the custom destination manager
    */
    dest = (ximg_dest_ptr) dest_mgr;
    dest->ximage = *ximage;

    /*
    ** Process scanlines until there are none left
    */

    while (cinfo.output_scanline < cinfo.output_height) 
    {
        num_scanlines = jpeg_read_scanlines(&cinfo, 
                                            (JSAMPARRAY)dest_mgr->buffer, 
                                            dest_mgr->buffer_height);
        (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
    }

    /*
    ** Return the colormap info as an array of XColors which can be
    ** used later for X color allocation purposes.
    */

    if (cinfo.actual_number_of_colors) 
    {
        colors = (XColor *) malloc((unsigned) sizeof(XColor) * 
                                   cinfo.actual_number_of_colors);
	if (!colors)
	{
            XDestroyImage (*ximage);
            jpeg_destroy_decompress(&cinfo);
	    return (_DtGrNO_MEMORY);
        }
        for (i=0; i<cinfo.actual_number_of_colors; i++)
	{
            if (cinfo.out_color_space == JCS_GRAYSCALE)
	    {
                colors[i].red = colors[i].green = colors[i].blue = 
                    INTERP_TO_XCOLORSPACE(cinfo.colormap[0][i]);
            }
            else /* JCS_RGB */
	    {
                colors[i].red   = INTERP_TO_XCOLORSPACE(cinfo.colormap[0][i]);
                colors[i].green = INTERP_TO_XCOLORSPACE(cinfo.colormap[1][i]);
                colors[i].blue  = INTERP_TO_XCOLORSPACE(cinfo.colormap[2][i]);
            }
        }

        *xcolors = colors;	      
        *ncolors = cinfo.actual_number_of_colors;
    }

    /*
    ** Set the other return parameters
    */
    *in_out_width = cinfo.output_width;
    *in_out_height = cinfo.output_height;
 
    /*
    ** Shut down the decompression engine and free the allocated memory
    */
    (*dest_mgr->finish_output) (&cinfo, dest_mgr);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    /*
    ** Return success
    */
    return (_DtGrSUCCESS);
}
