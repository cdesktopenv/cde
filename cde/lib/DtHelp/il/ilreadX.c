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
/* $XConsortium: ilreadX.c /main/3 1995/10/23 15:59:17 rswiston $ */
/**---------------------------------------------------------------------
***	
***    (c)Copyright 1991 Hewlett-Packard Co.
***    
***                             RESTRICTED RIGHTS LEGEND
***    Use, duplication, or disclosure by the U.S. Government is subject to
***    restrictions as set forth in sub-paragraph (c)(1)(ii) of the Rights in
***    Technical Data and Computer Software clause in DFARS 252.227-7013.
***                             Hewlett-Packard Company
***                             3000 Hanover Street
***                             Palo Alto, CA 94304 U.S.A.
***    Rights for non-DOD U.S. Government Departments and Agencies are as set
***    forth in FAR 52.227-19(c)(1,2).
***
***-------------------------------------------------------------------*/


#include "ilint.h"
#include "ilcontext.h"
#include "ilX.h"
#include <math.h>
#include <X11/Xutil.h>
#include "ilpipelem.h"
#include "ilerrors.h"

        /*  Bit-flip table, in /ilc/ildata.c */
extern const unsigned char ilBitReverseTable [];

        /*  Private data for all ilReadXDrawable() pipe functions. */
typedef struct {
    Display            *display;            /* copy of X display ptr */
    Drawable            drawable;           /* given to / derived by ilReadXDrawable() */
    Visual             *visual;             /* ptr to visual or null if a bitmap */
    Colormap            colormap;           /* ptr to colormap or null if a bitmap */
    int                 colormapSize;       /* # of entries in the colormap */
    unsigned short     *pPalette;           /* palette for Pseudo/StaticColor or null */
    ilClientImage       grayMapImage;       /* ilMap() image for gray or null */
    ilPtr               pGrayMapPixels;     /* ptr to pixels in grayMapImage if non-null */
    ilBool              isLongImage;        /* long/pixel format (24 bit drawable) */
    ilClientImage       rgbMapImage;        /* ilMap() image for rgb or null */
    ilPtr               pRGBMapPixels;      /* ptr to pixels in rgbMapImage if non-null */
    ilRect              rect;               /* piece of X image to read */
    long                stripHeight;        /* height of each piece of drawable to read */
    int                 copyPixmapDepth;    /* depth for copyPixmap, or 0 => don't create */
    Pixmap              copyPixmap;         /* pixmap to copy to/GetImage() from or null */
    GC                  copyGC;             /* GC to use to copy to "copyPixmap" */
    int                 nRedOnes;
    int                 nGreenOnes;
    int                 nBlueOnes;
    int                 nRedZeros;
    int                 nGreenZeros;
    int                 nBlueZeros;
    Bool                SlowMode;
    int                 OrgRedMask;
    int                 OrgGreenMask;
    int                 OrgBlueMask; 
    } ilReadXPrivRec, *ilReadXPrivPtr;


        /*  ------------------------ ilReadXInit ------------------------------- */
        /*  Init() function for ilReadXDrawable() pipe elements.
        */
static ilError ilReadXInit (
    ilReadXPrivPtr      pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
#define                 NCOLORS 256
#define                 NGRAYS  256
#define                 NRGBS   256
ilReadXPrivRec              priv;  
        /*  Init the palette if present by querying the colormap, with # entries =
            pPriv->colormapSize (limit to NCOLORS; may be less, so init unused to black).
        */
    if (pPriv->pPalette) {
        int             i;
        XColor          cells [NCOLORS], *pColor;
        unsigned short *pPalette;

        for (i = 0, pColor = cells; i < NCOLORS; i++, pColor++) {
            pColor->red = pColor->green = pColor->blue = 0;
            pColor->pixel = i;
            }
        XQueryColors (pPriv->display, pPriv->colormap, cells, 
                      (pPriv->colormapSize > NCOLORS) ? NCOLORS : pPriv->colormapSize);
        for (i = 0, pPalette = pPriv->pPalette, pColor = cells; i < NCOLORS;
         i++, pPalette++, pColor++) {
            pPalette[0*256] = pColor->red;
            pPalette[1*256] = pColor->green;
            pPalette[2*256] = pColor->blue;
            }
        }

        /*  If a grayMapImage, reading from grayscale: query the colormap and store
            the upper 8 bits of the green value into grayMapImage, which is used
            by an ilMap() filter which follows the ilReadXDrawable() (added there).
        */
    else if (pPriv->grayMapImage) {
        int             i;
        XColor          cells [NGRAYS], *pColor;
        ilPtr           pByte;

        for (i = 0, pColor = cells; i < NGRAYS; i++, pColor++) {
            pColor->red = pColor->green = pColor->blue = 0;
            pColor->pixel = i;
            }
        XQueryColors (pPriv->display, pPriv->colormap, cells, 
                      (pPriv->colormapSize > NGRAYS) ? NGRAYS : pPriv->colormapSize);
        for (i = 0, pByte = pPriv->pGrayMapPixels, pColor = cells; i < NGRAYS;
         i++, pColor++)
            *pByte++ = pColor->green >> 8;
        }

        /*  If an rgbMapImage, reading from True/DirectColor: query color - RGBs are
            independent.  Init the mapImage which is 3 byte (rgb) per pixel.
        */
    else if (pPriv->rgbMapImage) {
        int             i;
        XColor          cells [NRGBS], *pColor;
        ilPtr           pByte;

        for (i = 0, pColor = cells; i < NRGBS; i++, pColor++) {
            
          if(!(pPriv->SlowMode)){
            pColor->red = pColor->green = pColor->blue = 0;
            pColor->pixel = ((i << 16) | (i << 8) | i);
        	  }
          else {
             
	    pColor->red = pColor->green = pColor->blue = 0;
            pColor->pixel = ((i << pPriv->nRedZeros) | (i << pPriv->nGreenZeros) | 
                              i << pPriv->nBlueZeros);
	          }
            }
        XQueryColors (pPriv->display, pPriv->colormap, cells, 
                      (pPriv->colormapSize > NRGBS) ? NRGBS : pPriv->colormapSize);
        for (i = 0, pByte = pPriv->pRGBMapPixels, pColor = cells; i < NRGBS;
         i++, pColor++) {
            *pByte++ = pColor->red >> 8;
            *pByte++ = pColor->green >> 8;
            *pByte++ = pColor->blue >> 8;
            }
        }

        /*  Create copyPixmap if required (copyPixmapDepth != 0).
        */
    if (pPriv->copyPixmapDepth) {
        pPriv->copyPixmap = XCreatePixmap (pPriv->display, pPriv->drawable, pPriv->rect.width,
                                           pPriv->stripHeight, pPriv->copyPixmapDepth);
        if (!pPriv->copyPixmap)
            return IL_ERROR_X_RESOURCE;
        }

    return IL_OK;
}

        /*  ------------------------ ilReadXCleanup ------------------------------- */
        /*  Cleanup() function for ilReadXDrawable() pipe elements.
            Free anything in private that was created by Init().
        */
static ilError ilReadXCleanup (
    ilReadXPrivPtr      pPriv
    )
{
    if (pPriv->copyPixmap) {
        XFreePixmap (pPriv->display, pPriv->copyPixmap);
        pPriv->copyPixmap = (Pixmap)0;
        }
    return IL_OK;
}

        /*  ------------------------ ilReadXDestroy ------------------------------- */
        /*  Destroy() function for ilReadXDrawable() pipe elements.
            Free anything in private that was created when the element was added.
        */
static ilError ilReadXDestroy (
    ilReadXPrivPtr      pPriv
    )
{
    if (pPriv->pPalette)
        IL_FREE (pPriv->pPalette);
    if (pPriv->grayMapImage)
        ilDestroyObject (pPriv->grayMapImage);
    return IL_OK;
}



        /*  ------------------------ ilReadXExecute ------------------------------- */
        /*  Execute() function for ilReadXDrawable() pipe elements.
        */
static ilError ilReadXExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilReadXPrivPtr     pPriv;
Drawable                    readDrawable;
ilBool                      lastStrip;
long                        nLines;
int                         srcX, srcY;
XImage                     *Ximage;
ilPtr                       pSrcLine, pDstLine;
long                        srcRowBytes, dstRowBytes, nBytesToCopy;

        /*  Read pPriv->stripHeight lines from the drawable, but if that takes
            us off the end, read less, and note that this is lastStrip.
        */
    pPriv = (ilReadXPrivPtr)pData->pPrivate;
    if ((pData->srcLine + pPriv->stripHeight) >= pPriv->rect.height) {
        nLines = pPriv->rect.height - pData->srcLine;
        lastStrip = TRUE;
        }
    else {
        nLines = pPriv->stripHeight;
        lastStrip = FALSE;
        }
    if (nLines <= 0)
        return (lastStrip) ? IL_ERROR_LAST_STRIP : IL_OK;

        /*  If "copyPixmap" non-null, blt from drawable into it at (0,0), and set src (X,Y)
            to (0,0) - always GetImage() from left-top of it.  Then XGetImage() "nLines" 
            from readDrawable (drawable or pixmap), and copy the bits into pDstImage's 
            buffer at (0, "dstLine").
        */
    if (pPriv->copyPixmap) {
        XCopyArea (pPriv->display, pPriv->drawable, pPriv->copyPixmap, pPriv->copyGC, 
            pPriv->rect.x, pPriv->rect.y + pData->srcLine, pPriv->rect.width, nLines, 0, 0);
        readDrawable = pPriv->copyPixmap;
        srcX = srcY = 0;
        }
    else {
        readDrawable = pPriv->drawable;
        srcX = pPriv->rect.x;
        srcY = pPriv->rect.y + pData->srcLine;
        }
    Ximage = XGetImage (pPriv->display, readDrawable, srcX, srcY, pPriv->rect.width, 
                        nLines, ~0, ZPixmap);
    if (!Ximage)
        return IL_ERROR_X_GET_IMAGE;

        /*  Bump srcLine by # of lines gotten, and copy that many lines to pDstImage.
            Set nBytesToCopy to lesser of src and dstRowBytes.
        */
    *pNLines = nLines;
    pData->srcLine += nLines;
    pSrcLine = (ilPtr)Ximage->data;
    srcRowBytes = Ximage->bytes_per_line;
    dstRowBytes = pData->pDstImage->plane[0].nBytesPerRow;
    nBytesToCopy = (srcRowBytes < dstRowBytes) ? srcRowBytes : dstRowBytes;
    pDstLine = pData->pDstImage->plane[0].pPixels + dstLine * dstRowBytes;

        /*  If a long/pixel image, must extract IL RGB bytes from X long/pixel image.
        */
    if (pPriv->isLongImage) {
        ilPtr          pDst;
        unsigned long  temp, *pSrc;
        long           nLongsM1;

        while (nLines-- > 0) {
            pSrc = (unsigned long *)pSrcLine;
            pSrcLine += srcRowBytes;
            pDst = pDstLine;
            pDstLine += dstRowBytes;
            nLongsM1 = pPriv->rect.width - 1;   /* width > 0, from ilReadXDrawable() */
            do {
                temp = *pSrc++;                 /* long = <unused,r,g,b> each 8 bits */
                *(pDst + 2) = temp;             /* red */
                temp >>= 8;
                *(pDst + 1) = temp;             /* green */
                temp >>= 8;
                *pDst = temp;                   /* blue */
                pDst += 3;                      /* next dst pixel */
                } while (--nLongsM1 >= 0);
            }
        }
    else {
            /*  Byte or bit/pixel: if 1 bit/pixel image and LSBFirst bit order, reverse
                the bits using lookup table, else copy: if bytes/row same for X/IL images,
                 copy buffer, else one line at a time.
            */
        if ((Ximage->depth == 1) && (Ximage->bitmap_bit_order == LSBFirst)) {
            ilPtr          pSrc, pDst;
            long           nBytesM1;

            if (nBytesToCopy > 0)
                while (nLines-- > 0) {
                    pSrc = pSrcLine;
                    pSrcLine += srcRowBytes;
                    pDst = pDstLine;
                    pDstLine += dstRowBytes;
                    nBytesM1 = nBytesToCopy - 1;
                    do {
                        *pDst++ = ilBitReverseTable[*pSrc++];
                        } while (--nBytesM1 >= 0);
                    }
            }
        else {
            if (srcRowBytes == dstRowBytes)
                bcopy ((char *)pSrcLine, (char *)pDstLine, nLines * srcRowBytes);
            else {
                while (nLines-- > 0) {
                    bcopy ((char *)pSrcLine, (char *)pDstLine, nBytesToCopy);
                    pSrcLine += srcRowBytes;
                    pDstLine += dstRowBytes;
                    }
                }
            }
        }

    XDestroyImage (Ximage);
    return (lastStrip) ? IL_ERROR_LAST_STRIP : IL_OK;
}



        /*  ------------------------ ilReadXExecuteSlow ------------------------------- */
        /*  Execute() function for ilReadXDrawable() pipe elements.
        */
static ilError ilReadXExecuteSlow (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilReadXPrivPtr     pPriv;
ilReadXPrivRec              priv;  
Drawable                    readDrawable;
ilBool                      lastStrip;
long                        nLines;
int                         srcX, srcY;
XImage                      *Ximage;
ilPtr                       pSrcLine, pDstLine;
long                        srcRowBytes, dstRowBytes, nBytesToCopy;

        /*  Read pPriv->stripHeight lines from the drawable, but if that takes
            us off the end, read less, and note that this is lastStrip.
        */
    pPriv = (ilReadXPrivPtr)pData->pPrivate;
    if ((pData->srcLine + pPriv->stripHeight) >= pPriv->rect.height) {
        nLines = pPriv->rect.height - pData->srcLine;
        lastStrip = TRUE;
        }
    else {
        nLines = pPriv->stripHeight;
        lastStrip = FALSE;
        }
    if (nLines <= 0)
        return (lastStrip) ? IL_ERROR_LAST_STRIP : IL_OK;

        /*  If "copyPixmap" non-null, blt from drawable into it at (0,0), and set src (X,Y)
            to (0,0) - always GetImage() from left-top of it.  Then XGetImage() "nLines" 
            from readDrawable (drawable or pixmap), and copy the bits into pDstImage's 
            buffer at (0, "dstLine").
        */
    if (pPriv->copyPixmap) {
        XCopyArea (pPriv->display, pPriv->drawable, pPriv->copyPixmap, pPriv->copyGC, 
            pPriv->rect.x, pPriv->rect.y + pData->srcLine, pPriv->rect.width, nLines, 0, 0);
        readDrawable = pPriv->copyPixmap;
        srcX = srcY = 0;
        }
    else {
        readDrawable = pPriv->drawable;
        srcX = pPriv->rect.x;
        srcY = pPriv->rect.y + pData->srcLine;
        }
    Ximage = XGetImage (pPriv->display, readDrawable, srcX, srcY, pPriv->rect.width, 
                        nLines, ~0, ZPixmap);
    if (!Ximage)
        return IL_ERROR_X_GET_IMAGE;

        /*  Bump srcLine by # of lines gotten, and copy that many lines to pDstImage.
            Set nBytesToCopy to lesser of src and dstRowBytes.
        */
    *pNLines = nLines;
    pData->srcLine += nLines;
    pSrcLine = (ilPtr)Ximage->data;
    srcRowBytes = Ximage->bytes_per_line;
    dstRowBytes = pData->pDstImage->plane[0].nBytesPerRow;
    nBytesToCopy = (srcRowBytes < dstRowBytes) ? srcRowBytes : dstRowBytes;
    pDstLine = pData->pDstImage->plane[0].pPixels + dstLine * dstRowBytes;

        /*  If a long/pixel image, must extract IL RGB bytes from X long/pixel image.
        */
    if (pPriv->isLongImage) {
        ilPtr          pDst;
        unsigned long  temp, *pSrc;
        long           nLongsM1;
        unsigned int            X, Y;

	XVisualInfo             *pVisualInfo;
	unsigned int            pixel;
        int                     bit, count;
/************* slow pixel code*****************/
               
            if (((pPriv->nRedOnes <= 8) && (pPriv->nRedOnes >= 1)) 
                  && ((pPriv->nGreenOnes <= 8) && (pPriv->nGreenOnes >= 1)) 
                  && ((pPriv->nBlueOnes <= 8) && (pPriv->nBlueOnes >= 1))) {   
           
               for ( Y = 0; Y < nLines; Y++) {
                pDst = pDstLine;
                pDstLine += dstRowBytes;
                for ( X = 0 ; X < pPriv->rect.width; X++) {
                  pixel = XGetPixel(Ximage, X, Y );
                 /* Red */
                  *pDst++ = (pixel & pPriv->OrgRedMask) >> pPriv->nRedZeros;
                  /* Green */
                  *pDst++ = (pixel & pPriv->OrgGreenMask) >> pPriv->nGreenZeros; 
                  /* Blue */
                 *pDst++ = (pixel & pPriv->OrgBlueMask) >> pPriv->nBlueZeros;
                 
		}
             }
          }
        }
    
     
    else {
            /*  Byte or bit/pixel: if 1 bit/pixel image and LSBFirst bit order, reverse
                the bits using lookup table, else copy: if bytes/row same for X/IL images,
                 copy buffer, else one line at a time.
            */
        if ((Ximage->depth == 1) && (Ximage->bitmap_bit_order == LSBFirst)) {
            ilPtr          pSrc, pDst;
            long           nBytesM1;

            if (nBytesToCopy > 0)
                while (nLines-- > 0) {
                    pSrc = pSrcLine;
                    pSrcLine += srcRowBytes;
                    pDst = pDstLine;
                    pDstLine += dstRowBytes;
                    nBytesM1 = nBytesToCopy - 1;
                    do {
                        *pDst++ = ilBitReverseTable[*pSrc++];
                        } while (--nBytesM1 >= 0);
                    }
            }
        else {
            if (srcRowBytes == dstRowBytes)
                bcopy ((char *)pSrcLine, (char *)pDstLine, nLines * srcRowBytes);
            else {
                while (nLines-- > 0) {
                    bcopy ((char *)pSrcLine, (char *)pDstLine, nBytesToCopy);
                    pSrcLine += srcRowBytes;
                    pDstLine += dstRowBytes;
                    }
                }
            }
        }

    XDestroyImage (Ximage);
    return (lastStrip) ? IL_ERROR_LAST_STRIP : IL_OK;
}


        /*  ------------------------ ilReadXDrawable ---------------------------- */
        /*  Public function; see spec.
        */
ilBool ilReadXDrawable (
    ilPipe                  pipe,
    Display                *display,
    Drawable                drawable,
    Visual                 *visual,
    Colormap                colormap,
    ilBool                  blackIsZero,
    ilRect                 *pSrcRect,
    ilBool                  copyToPixmap,
    unsigned long           flags
    )
{
ilReadXPrivRec              priv;           /* pre-inited private block; becomes *pPriv */
ilPipeInfo                  info;
ilImageDes                  des;
ilImageFormat               format;
ilError                     error;
int                         pixelSize, notUsed;
ilDstElementData            dstData;
XVisualInfo                 template, *pVisualInfo;
ilReadXPrivPtr              pPriv;
Window                      root;
int                         x, y;
unsigned int                border_width;
unsigned int                width, height, depth;   /* values for drawable */
Bool                        SlowMode = FALSE;
unsigned int                pixel;
int                         bit, count, nRedZeros, nGreenZeros, nBlueZeros;
int                         nRedOnes, nGreenOnes, nBlueOnes;
int                         RedMask, GreenMask, BlueMask;
/*int                         OrgRedMask, OrgGreenMask, OrgBlueMask;*/

    if (pipe->objectType != IL_PIPE) {
        pipe->context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }
    if (flags & ~1)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_PAR_NOT_ZERO);

        /*  Get pipe info; if pipe not in IL_PIPE_EMPTY state: error.
            Get width, height and depth of the requested drawable.
        */
    if (ilGetPipeInfo (pipe, FALSE, &info, &des, &format) != IL_PIPE_EMPTY) {
        if (!pipe->context->error)
            ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);
        return FALSE;
        }
    if (!XGetGeometry (display, drawable, &root, &x, &y, &width, &height, 
                       &border_width, &depth))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_X_DRAWABLE);

        /*  Init priv with what we have so far. Set "priv.rect" to rectangle to read from 
            drawable: bounds of drawable, intersected with pSrcRect if present.
                Null priv. objects - call ilReadXDestroy() on failure to free non-nulls.
        */
    priv.display = display;
    priv.drawable = drawable;
    priv.visual = visual;
    priv.colormap = colormap;
    priv.pPalette = (unsigned short *)NULL;
    priv.grayMapImage = (ilClientImage)NULL;
    priv.isLongImage = FALSE;
    priv.rgbMapImage = (ilClientImage)NULL;
    priv.rect.x = priv.rect.y = 0;
    priv.rect.width = width;
    priv.rect.height = height;
    if (pSrcRect)
        _ilIntersectRect (pSrcRect, &priv.rect);
    if ((priv.rect.width <= 0) || (priv.rect.height <= 0))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_ZERO_SIZE_IMAGE);

        /*  Do type-specific setup: set pixelSize based on des.type; set des and format.
            If no visual or colormap: if not depth 1, error; else a bitmap: handle here.
        */
    if (!visual || !colormap) {
        if (depth != 1)                                         /* not a bitmap */
            return ilDeclarePipeInvalid (pipe, IL_ERROR_X_COLORMAP_VISUAL);

        des = *IL_DES_BITONAL;
        des.blackIsZero = blackIsZero;
        format = *IL_FORMAT_BIT;
        pixelSize = 1;
        }
    else {
            /*  Not a bitmap: get info from "visual"; depths must match.
                Set "supported" true if this visual handled, else break (becomes error).
            */
        if (!colormap || !visual)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_X_COLORMAP_VISUAL);

        template.visualid = XVisualIDFromVisual (visual);
        pVisualInfo = XGetVisualInfo (display, VisualIDMask, &template, &notUsed);
        if (!pVisualInfo)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_X_RESOURCE);
        if (pVisualInfo->depth != depth)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_UNSUPPORTED_VISUAL);
        priv.colormapSize = pVisualInfo->colormap_size;

        switch (pVisualInfo->class) {

                /*  Support 1 and 8 bit gray scale.  For 1 bit, query color map to 
                    determine blackIsZero (assume is if pixel 0 = rgb of 0,0,0.)
                    For 8 bit, add an ilMap() element using pPriv->grayMapImage, which
                    is setup in ilReadXInit() with the gray value for each X pixel.
                    However, skip this step if rawMode.
                */
            case GrayScale:
            case StaticGray:
                if (depth == 1) {
                    XColor color;

                    color.pixel = 0;
                    XQueryColor (display, colormap, &color);
                    des = *IL_DES_BITONAL;
                    des.blackIsZero = (!color.red && !color.green && !color.blue);
                    format = *IL_FORMAT_BIT;
                    pixelSize = 1;
                    priv.SlowMode = SlowMode;
                    }
                else if ((depth <= 8) && (depth > 1)) {
                    ilImageInfo     imageInfo, *pImageInfo;

                    if (!(flags & IL_READ_X_RAW_MODE)) {
                        imageInfo.pDes = IL_DES_GRAY;
                        imageInfo.pFormat = IL_FORMAT_BYTE;
                        imageInfo.width = 256;
                        imageInfo.height = 1;
                        imageInfo.clientPixels = FALSE;
                        priv.grayMapImage = ilCreateClientImage (pipe->context, 
                                                                 &imageInfo, 0);
                        if (!priv.grayMapImage)
                            return FALSE;
                        ilQueryClientImage (priv.grayMapImage, &pImageInfo, 0);
                        priv.pGrayMapPixels = pImageInfo->plane[0].pPixels;
                        }
                    des = *IL_DES_GRAY;
                    format = *IL_FORMAT_BYTE;
                    pixelSize = 8;

                    }
		
                else return ilDeclarePipeInvalid (pipe, IL_ERROR_UNSUPPORTED_VISUAL);
                  SlowMode = depth < 8;
                  priv.SlowMode = SlowMode;
                break;

                /*  Support 8 bit Pseudo/StaticColor as palette image; alloc palette. */
            case StaticColor:
            case PseudoColor:
                if (depth > 8)
                    return ilDeclarePipeInvalid (pipe, IL_ERROR_UNSUPPORTED_VISUAL);

                des = *IL_DES_GRAY;
                des.type = IL_PALETTE;
                des.blackIsZero = FALSE;
                format = *IL_FORMAT_BYTE;
                pixelSize = 8;
               if (!(priv.pPalette = (unsigned short *)
                        IL_MALLOC_ZERO (sizeof(unsigned short) * (3 * 256))))
                    return ilDeclarePipeInvalid (pipe, IL_ERROR_MALLOC);
		    SlowMode = depth < 8;
                    priv.SlowMode = SlowMode;
                    
                break;

               /*  Support True/DirectColor only if format = "<unused 8><8R><8G><8B>". */
            case DirectColor:
            case TrueColor:
                if ((depth == 24)
                 && (pVisualInfo->red_mask == 0xff0000)
                 && (pVisualInfo->green_mask == 0xff00)
                 && (pVisualInfo->blue_mask == 0xff)) {
                    ilImageInfo     imageInfo, *pImageInfo;

                    if (!(flags & IL_READ_X_RAW_MODE)) {
                        imageInfo.pDes = IL_DES_RGB;
                        imageInfo.pFormat = IL_FORMAT_3BYTE_PIXEL;
                        imageInfo.width = 256;
                        imageInfo.height = 1;
                        imageInfo.clientPixels = FALSE;
                        priv.rgbMapImage = ilCreateClientImage (pipe->context, 
                                                                &imageInfo, 0);
                        if (!priv.rgbMapImage)
                            return FALSE;
                        ilQueryClientImage (priv.rgbMapImage, &pImageInfo, 0);
                        priv.pRGBMapPixels = pImageInfo->plane[0].pPixels;
                        }
                    des = *IL_DES_RGB;
                    format = *IL_FORMAT_3BYTE_PIXEL;
                    pixelSize = 24;
                    priv.isLongImage = TRUE;
                    priv.SlowMode = SlowMode;
                    }
                    
               /*suport for Gacko and 12 bit display depth */

              else { 

               /*counting the number of ones and zeros in each red, 
                 green and blue mask */
	       /* calculating Number of zeros and ones for red_mask */
               /******************************************************/
                 priv.OrgRedMask = pVisualInfo->red_mask;
                 RedMask = pVisualInfo->red_mask;
                 bit = 1;
                 nRedOnes = 0;
                     while(bit<0xffffff) {
                       if (RedMask & bit) nRedOnes++;
                       bit <<= 1;
		     }
                      
            /*     nRedZeros = 8 - nRedOnes;
                 priv.nRedZeros = nRedZeros; */
                 priv.nRedOnes = nRedOnes;


                 bit = 1;
                 count = 0;
                     while(!(RedMask & bit)){
                       count++;
                       RedMask >>= 1;
                      }
                 nRedZeros = count;
                 priv.nRedZeros = nRedZeros;


		 

		 /* calculating Number of zeros and ones for Green_mask */
                 /*******************************************************/
		 priv.OrgGreenMask = pVisualInfo->green_mask;
		 GreenMask = pVisualInfo->green_mask;
                 bit = 1;
                 nGreenOnes = 0;
                     while(bit<0xffffff) {
                       if (GreenMask & bit) nGreenOnes++;
                       bit <<= 1;
		     }
            /*     nGreenZeros = 8 - nGreenOnes;
                 priv.nGreenZeros = nGreenZeros; */
                 priv.nGreenOnes = nGreenOnes;

                 bit = 1;
                 count = 0;
                     while(!(GreenMask & bit )){
                       count++;
                       GreenMask >>= 1;
                      }
                 nGreenZeros = count;
                 priv.nGreenZeros = nGreenZeros;




               /* calculating Number of zeros and ones for blue_mask */
               /******************************************************/
		 priv.OrgBlueMask = pVisualInfo->blue_mask;
		 BlueMask = pVisualInfo->blue_mask;
		 bit = 1;
                 nBlueOnes = 0;
                     while(bit<0xffffff) {
                       if (BlueMask & bit) nBlueOnes++;
                       bit <<= 1;
		     }
               /*  nBlueZeros = 8 - nBlueOnes;
                 priv.nBlueZeros = nBlueZeros; */
		 priv.nBlueOnes = nBlueOnes;

		 bit = 1;
                 count = 0;
                     while(!(BlueMask & bit)){
                       count++;
                       BlueMask >>= 1;
                      }
                 nBlueZeros = count;
                 priv.nBlueZeros = nBlueZeros;




                 if ((depth <=32)
                  &&((nRedOnes <= 8) && (nRedOnes >= 1)) 
                  &&((nGreenOnes <= 8) && (nGreenOnes >= 1)) 
                  &&((nBlueOnes <= 8) && (nBlueOnes >= 1))) {
                
                 ilImageInfo     imageInfo, *pImageInfo;

                    if (!(flags & IL_READ_X_RAW_MODE)) {
                        imageInfo.pDes = IL_DES_RGB;
                        imageInfo.pFormat = IL_FORMAT_3BYTE_PIXEL;
                        imageInfo.width = 256;
                        imageInfo.height = 1;
                        imageInfo.clientPixels = FALSE;
                        priv.rgbMapImage = ilCreateClientImage (pipe->context, 
                                                                &imageInfo, 0);
                        if (!priv.rgbMapImage)
                            return FALSE;
                        ilQueryClientImage (priv.rgbMapImage, &pImageInfo, 0);
                        priv.pRGBMapPixels = pImageInfo->plane[0].pPixels;
                        }
                    des = *IL_DES_RGB;
                    format = *IL_FORMAT_3BYTE_PIXEL;
                    pixelSize = 24;
                    priv.isLongImage = TRUE;
		    SlowMode = depth<= 32;                
                    priv.SlowMode = SlowMode; 

                    }
	       
                else return ilDeclarePipeInvalid (pipe, IL_ERROR_UNSUPPORTED_VISUAL);

                }   
               /* SlowMode = depth<= 32;                
                pPriv.SlowMode = SlowMode;*/
                break;
            }   /* END switch visual class */
        }       /* END not a bitmap */
      
        /*  Visual (or a bitmap) supported; read in strips to conserve memory. */
    priv.stripHeight = ilRecommendedStripHeight (&des, &format, priv.rect.width, 
                                                 priv.rect.height);

        /*  Create a GC if copyToPixmap (set copyPixmapDepth != 0).  Null ptrs
            out and call ilReadXDestroy() if failure - it will free non-null objects.
        */
    priv.copyPixmap = (Pixmap)0;
    priv.copyPixmapDepth = 0;
    priv.copyGC = (GC)NULL;
    if (copyToPixmap) {
        XGCValues   values;
        values.subwindow_mode = IncludeInferiors;       /* get subwindow contents */
        priv.copyGC = XCreateGC (priv.display, priv.drawable, GCSubwindowMode, &values);
        if (!priv.copyGC) {
            ilReadXDestroy (&priv);
            return ilDeclarePipeInvalid (pipe, IL_ERROR_X_RESOURCE);
            }
        priv.copyPixmapDepth = depth;
        }

        /*  Add a producer element to read from the X drawable; copy priv into *pPriv. */
    dstData.producerObject = (ilObject)NULL;
    dstData.pDes = &des;
    dstData.pFormat = &format;
    dstData.width = priv.rect.width;
    dstData.height = priv.rect.height;
    dstData.stripHeight = priv.stripHeight;
    dstData.constantStrip = TRUE;
    dstData.pPalette = priv.pPalette;
    
  
    pPriv = (ilReadXPrivPtr)ilAddPipeElement (pipe, IL_PRODUCER, sizeof (ilReadXPrivRec),
                0, (ilSrcElementData *)NULL, &dstData, ilReadXInit, ilReadXCleanup,
                ilReadXDestroy, ((SlowMode)?ilReadXExecuteSlow:ilReadXExecute), 0);
                
  
/*    
    pPriv = (ilReadXPrivPtr)ilAddPipeElement (pipe, IL_PRODUCER, sizeof (ilReadXPrivRec),
                0, (ilSrcElementData *)NULL, &dstData, ilReadXInit, ilReadXCleanup,
                ilReadXDestroy, ilReadXExecuteSlow, 0);
                            
*/                
                
    if (!pPriv) {
        ilReadXDestroy (&priv);
        return FALSE;
        }
    *pPriv = priv;

        /*  If a gray/rgbMapImage, use ilMap() to remap from colormap values. */
    if (priv.grayMapImage)
        return ilMap (pipe, priv.grayMapImage);
    else if (priv.rgbMapImage)
        return ilMap (pipe, priv.rgbMapImage);

    pipe->context->error = IL_OK;
    return TRUE;
}










