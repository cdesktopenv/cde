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
/* $XConsortium: iltruegray.c /main/3 1995/10/23 16:02:03 rswiston $ */
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

/* ======================================================================================
     /ilc/iltruegray.c : Image Library conversion routines fron RGB to grayscale
   =================================================================================== */


#include "ilint.h"
#include "ilpipelem.h"
#include "ilconvert.h"



typedef struct {
    unsigned long   R[256], G[256], B[256]; /* RGB->gray multiply lookup table */
    long            nPixels;                /* width of src/dst images */
    long            srcRowBytes;            /* bytes/row of src image */
    ilPtr           pSrcPixels;             /* ptr to start of src pixels */
    long            dstRowBytes;            /* bytes/row of dst image */
    ilPtr           pDstPixels;             /* ptr to start of dst pixels */
    } ilRGBToGrayRec, *ilRGBToGrayRecPtr;


/* =============================================================================================================================
             ilInitRGBToGray     -   Init() function.
        NOTE: also used for Gray to RGB conversion below !
   ============================================================================================================================= */


static ilError ilInitRGBGrayConversions (
    ilRGBToGrayRecPtr  pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{

    pPriv->nPixels = pSrcImage->width;
    pPriv->srcRowBytes = pSrcImage->plane[0].nBytesPerRow;
    pPriv->pSrcPixels = pSrcImage->plane[0].pPixels;
    pPriv->dstRowBytes = pDstImage->plane[0].nBytesPerRow;
    pPriv->pDstPixels = pDstImage->plane[0].pPixels;

    return IL_OK;
}

/* =============================================================================================================================
             ilAddElementRGBGrayConversions - AddElement() function
        Called only for RGB->gray conversion: init RGB lookup/multiply tables in pPriv
   ============================================================================================================================= */

static ilError ilAddElementRGBGrayConversions (
    ilRGBToGrayRecPtr   pPriv,
    unsigned short     *pPalette,           /* not used */
    void               *pOptionData         /* not used */
    )
{
    register int        i;

        /*  Init tables, index by R/G/B, which effectively multiply each component
            (0..255) by the NTSC conversion formula: 0.30 * R + 0.587 * G + 0.114 * B,
            except that each value is * 65536 for extra accuracy: the result is
            shifted right by 16 before storing.
            Note that "19660" would probably be more accurate at "19661", but the 
            old code used 19660 and for that little difference it is silly to 
            invalidate the old checksum tests.  Note also that if R/G/B are all = 255
            the result is still <= 255 after the shift right.
        */
    for (i = 0; i < 256; i++) {
        pPriv->R[i] = 19660 * i;
        pPriv->G[i] = 38469 * i;
        pPriv->B[i] = 7471 * i;
        }
    return IL_OK;
}


/* =============================================================================================================================
            ilExecuteRGBToGray    -    Execute() function: convert the given # of src lines.

         Converts RGB images to a grayscale with format byte per pixel.
            Input image:   uncompressed IL_DES_RGB, IL_FORMAT_3BYTE_PIXEL
            Output image:  uncompressed IL_GRAY, 255 levels, IL_FORMAT_BYTE.

   ============================================================================================================================= */

static ilError ilExecuteRGBToGray  (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
register ilRGBToGrayRecPtr  pPriv;
long                        srcnbytes, dstnbytes;
register ilPtr              psrcline, pdstline;
register ilPtr              psrc, pdst;
register long               nLines, nPixels;
register INT32		    i;



    pPriv = (ilRGBToGrayRecPtr)pData->pPrivate;
    srcnbytes = pPriv->srcRowBytes;
    psrcline = pPriv->pSrcPixels + pData->srcLine * srcnbytes;
    dstnbytes = pPriv->dstRowBytes;
    pdstline = pPriv->pDstPixels + dstLine * dstnbytes;

    if (pPriv->nPixels < 0)  return 0;

    nLines = *pNLines;
    if (nLines <= 0) return 0;

    while ( nLines--  > 0 ) {

          psrc = psrcline;
          pdst = pdstline;
          nPixels = pPriv->nPixels;

          /*  --    grayscale from:     Y  =   0.30red  + 0.587green + 0.114blue    */
          while ( nPixels--  >  0 ) {
                i = pPriv->R[*psrc++];
                i += pPriv->G[*psrc++];
                i += pPriv->B[*psrc++];
                *pdst++ = i >> 16;
                }

          psrcline += srcnbytes;
          pdstline += dstnbytes;
    }


    return IL_OK;
}


/* =============================================================================================================================
          ilRGBToGray          -    Table exported to ilConvert(), declared in /ilc/ilconvert.h 
   ============================================================================================================================= */

IL_PRIVATE ilConvertRec _ilRGBToGray = {
    IL_NPF,                                     /* CheckFormat() */
    IL_STD_FORMAT_3BYTE_PIXEL,                  /* srcFormatCode */
    ilAddElementRGBGrayConversions,             /* AddElement() */
    IL_DES_GRAY,                                /* pDstDes */
    IL_FORMAT_BYTE,                             /* pDstFormat */
    sizeof (ilRGBToGrayRec),                    /* nBytesPrivate */
    ilInitRGBGrayConversions,                   /* Init() */
    IL_NPF,                                     /* Cleanup() */
    IL_NPF,                                     /* Destroy() */
    ilExecuteRGBToGray                          /* Execute() */
    };


/* =====================================================================================
                                   GRAY TO RGB
        Convert byte/pixel gray to 3byte/pixel rgb.

   ==================================================================================== */

static ilError ilExecuteGrayToRGB  (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilRGBToGrayRecPtr           pPriv;
register long               nLinesM1, nPixelsM1;
long                        srcRowBytes, dstRowBytes, nPixelsM1Init;
ilPtr                       pSrcLine, pDstLine;
register ilPtr              pSrc, pDst;
register ilByte             byte;

    pPriv = (ilRGBToGrayRecPtr)pData->pPrivate;
    nPixelsM1Init = pPriv->nPixels - 1;
    nLinesM1 = *pNLines - 1;
    if ((nLinesM1 < 0) || (nPixelsM1Init < 0))
        return IL_OK;

    srcRowBytes = pPriv->srcRowBytes;
    pSrcLine = pPriv->pSrcPixels + pData->srcLine * srcRowBytes;
    dstRowBytes = pPriv->dstRowBytes;
    pDstLine = pPriv->pDstPixels + dstLine * dstRowBytes;

        /*  Simply replicate each gray byte 3 times to form RGB - tough, eh?
            However if black not zero must invert the gray byte.
        */
    if (pData->pSrcImage->pDes->blackIsZero) {
        do {
            pSrc = pSrcLine;
            pSrcLine += srcRowBytes;
            pDst = pDstLine;
            pDstLine += dstRowBytes;
            nPixelsM1 = nPixelsM1Init;
            do {
                byte = *pSrc++;
                *pDst++ = byte;
                *pDst++ = byte;
                *pDst++ = byte;
                } while (--nPixelsM1 >= 0);
            } while (--nLinesM1 >= 0);
        }
    else {
        do {
            pSrc = pSrcLine;
            pSrcLine += srcRowBytes;
            pDst = pDstLine;
            pDstLine += dstRowBytes;
            nPixelsM1 = nPixelsM1Init;
            do {
                byte = ~*pSrc++;
                *pDst++ = byte;
                *pDst++ = byte;
                *pDst++ = byte;
                } while (--nPixelsM1 >= 0);
            } while (--nLinesM1 >= 0);
        }

    return IL_OK;
}


/* =====================================================================================
          ilGrayToRGB  -    Table exported to ilConvert(), declared in /ilc/ilconvert.h 
   ==================================================================================== */

IL_PRIVATE ilConvertRec _ilGrayToRGB = {
    IL_NPF,                                     /* CheckFormat() */
    IL_STD_FORMAT_BYTE,                         /* srcFormatCode */
    IL_NPF,                                     /* AddElement() */
    IL_DES_RGB,                                 /* pDstDes */
    IL_FORMAT_3BYTE_PIXEL,                      /* pDstFormat */
    sizeof (ilRGBToGrayRec),                    /* nBytesPrivate */
    ilInitRGBGrayConversions,                   /* Init() */
    IL_NPF,                                     /* Cleanup() */
    IL_NPF,                                     /* Destroy() */
    ilExecuteGrayToRGB                          /* Execute() */
    };



