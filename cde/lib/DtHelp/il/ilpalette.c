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
/* $XConsortium: ilpalette.c /main/3 1995/10/23 15:58:32 rswiston $ */
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

        /*  /ilc/ilpalette.c : Code for handling IL_PALETTE images.
            Mostly converters to other types (e.g. to RGB), referenced by 
            /ilc/ilconvert.c
        */
#include "ilint.h"
#include "ilpipelem.h"
#include "ilconvert.h"

        /*  ----------------------- ilPaletteToRGBByte ------------------------- */
        /*  Converts byte palette image to RGB, each a byte
            Input image:   uncompressed IL_PALETTE, any # of levels, IL_FORMAT_BYTE.
            Output image:  IL_DES_RGB, IL_FORMAT_3BYTE_PIXEL.
        */

        /*  Private data, inited by Init() function.
            "palette" is inited when the element is added. It is a copy of the palette,
            as longwords: <8 unused> <8 blue> <8 green> <8 red> for each value 0..255
            for faster lookup of each palette pixel.  Each 8 is the upper 8 bits of
            the palette entry.
        */
typedef struct {
    long            nPixelsM1;              /* width - 1 of src/dst images */
    long            srcRowBytes;            /* bytes/row of src image */
    ilPtr           pSrcPixels;             /* ptr to start of src pixels */
    long            dstRowBytes;            /* bytes/row of dst image */
    ilPtr           pDstPixels;             /* ptr to start of dst pixels */
    unsigned short *pPalette;               /* ptr to palette, set by AddElement() */
    unsigned long   palette [256];          /* copy of palette: see above */
    ilBool          firstStrip;             /* true if first strip to Execute() */
    } ilPaletteToRGBPrivRec, *ilPaletteToRGBPrivPtr;


        /*  AddElement() function, called by ilConvert() after the pipe element is added.
            Merely copy the given pPalette into private.
        */
static ilError ilAddElementPaletteToRGBByte (
    ilPaletteToRGBPrivPtr pPriv,
    unsigned short       *pPalette
    )
{
    pPriv->pPalette = pPalette;
    return IL_OK;
}

        /*  Init() function: init values in private */
static ilError ilInitPaletteToRGBByte (
    ilPaletteToRGBPrivPtr pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
    pPriv->nPixelsM1 = pSrcImage->width - 1;
    pPriv->srcRowBytes = pSrcImage->plane[0].nBytesPerRow;
    pPriv->pSrcPixels = pSrcImage->plane[0].pPixels;
    pPriv->dstRowBytes = pDstImage->plane[0].nBytesPerRow;
    pPriv->pDstPixels = pDstImage->plane[0].pPixels;
    pPriv->firstStrip = TRUE;
    return IL_OK;
}

        /*  Called by Execute() to setup "palette" in private */
static void ilInitPackedPalette (
    ilPaletteToRGBPrivPtr pPriv
    )
{
register int                i;
register unsigned long     *pPacked, packed;
register unsigned short    *pRed, *pGreen, *pBlue, temp;

        /*  Pack the upper 8 bits of the rgb value, with blue in the high order bits. */
    pPacked = pPriv->palette;
    pRed   = &pPriv->pPalette [0 * 256];
    pGreen = &pPriv->pPalette [1 * 256];
    pBlue  = &pPriv->pPalette [2 * 256];
    for (i = 0; i < 256; i++) {
        temp = *pBlue++;
        packed = temp >> 8;
        packed <<= 8;
        temp = *pGreen++;
        packed |= temp >> 8;
        packed <<= 8;
        temp = *pRed++;
        packed |= temp >> 8;
        *pPacked++ = packed;
        }
}

        /*  Execute() function: convert the given # of src lines.
            Lookup each palette byte (0..255) and get a long which looks like:
                <8 unused> <8 blue> <8 green> <8 red>
            output the lower 8 bits, then shift to get RGB.
        */
static ilError ilExecutePaletteToRGBByte (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines             /* ignored on input */
    )
{
ilPaletteToRGBPrivPtr       pPriv;
long                        srcRowBytes, dstRowBytes, nPixelsM1;
ilPtr                       pSrcLine, pDstLine;
register ilPtr              pSrc, pDst;
register unsigned long      rgb, *pPackedPalette;
register long               nLinesM1, nPixelsM1Temp;

        /*  If firstStrip, init the packed palette in private */
    pPriv = (ilPaletteToRGBPrivPtr)pData->pPrivate;
    if (pPriv->firstStrip) {
        ilInitPackedPalette (pPriv);
        pPriv->firstStrip = FALSE;
        }

        /*  Setup src,dst line ptrs and rowBytes, and nLines/PixelsM1 ("minus 1");
            exit if no lines or pixels.
        */
    srcRowBytes = pPriv->srcRowBytes;
    pSrcLine = pPriv->pSrcPixels + pData->srcLine * srcRowBytes;
    dstRowBytes = pPriv->dstRowBytes;
    pDstLine = pPriv->pDstPixels + dstLine * dstRowBytes;

    nPixelsM1 = pPriv->nPixelsM1;
    if (nPixelsM1 < 0)
        return 0;
    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return 0;
    nLinesM1--;

        /*  For each src byte: get a long from the cvt'd palette table, then take
            the low eight bits to get r,g,b, shifting down by 8 to get next component.
        */
    pPackedPalette = pPriv->palette;
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes;
        nPixelsM1Temp = nPixelsM1;
        do {
            rgb = pPackedPalette [*pSrc++];
            *pDst++ = (ilByte)rgb;          /* red */
            rgb >>= 8;
            *pDst++ = (ilByte)rgb;          /* green */
            rgb >>= 8;
            *pDst++ = (ilByte)rgb;          /* blue */
            } while (--nPixelsM1Temp >= 0);
        } while (--nLinesM1 >= 0);

    return IL_OK;
}

        /*  Table exported to ilConvert(), declared in /ilc/ilconvert.h .
        */
IL_PRIVATE ilConvertRec _ilPaletteToRGBByte = {
    IL_NPF,                                     /* CheckFormat() */
    IL_STD_FORMAT_BYTE,                         /* srcFormatCode */
    ilAddElementPaletteToRGBByte,               /* AddElement() */
    IL_DES_RGB,                                 /* pDstDes */
    IL_FORMAT_3BYTE_PIXEL,                      /* pDstFormat */
    sizeof (ilPaletteToRGBPrivRec),             /* nBytesPrivate */
    ilInitPaletteToRGBByte,                     /* Init() */
    IL_NPF,                                     /* Cleanup() */
    IL_NPF,                                     /* Destroy() */
    ilExecutePaletteToRGBByte                   /* Execute() */
    };



