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
/* $XConsortium: ilformat.c /main/5 1996/06/19 12:21:36 ageorge $ */
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

        /*  /ilc/ilformat.c : Reference by ilConvert in /ilc/ilconvert.c .
            Contains pipe functions and an ilFormatRec, which points to the functions
            and defines how the formatter should be added by ilConvert().
        */
#include "ilint.h"
#include "ilpipelem.h"
#include "ilconvert.h"
#include "ilerrors.h"

        /*  ------------------------- ilExecuteBitAlign ------------------------- */
        /*  Execute() function: convert the given # of src lines, changing
            the rowBitAlign.  Easy; just bcopy each line, bump by src/dstRowBytes,
            which presumably are different because of different bit alignment.
        */
static ilError ilExecuteBitAlign (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilImagePlaneInfo           *pImagePlane;
long                        srcRowBytes, dstRowBytes;
long               nLinesM1, nBytes;
ilPtr              pSrc, pDst;

        /*  Use bcopy to copy each line; copy the lesser of the src/dstRowBytes.
            Exit if no lines or pixels.
        */
    pImagePlane = &pData->pSrcImage->plane[0];
    srcRowBytes = pImagePlane->nBytesPerRow;
    pSrc = pImagePlane->pPixels + pData->srcLine * srcRowBytes;

    pImagePlane = &pData->pDstImage->plane[0];
    dstRowBytes = pImagePlane->nBytesPerRow;
    pDst = pImagePlane->pPixels + dstLine * dstRowBytes;

    nBytes = (srcRowBytes < dstRowBytes) ? srcRowBytes : dstRowBytes;
    if (nBytes <= 0)
        return IL_OK;
    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return IL_OK;
    nLinesM1--;

    do {
        bcopy ((char *)pSrc, (char *)pDst, nBytes);
        pSrc += srcRowBytes;
        pDst += dstRowBytes;
        } while (--nLinesM1 >= 0);

    return IL_OK;
}

        /*  Table exported to ilConvert(), declared in /ilc/ilconvert.h .
        */
IL_PRIVATE ilFormatRec _ilFormatRowBitAlign = {
    IL_NPF,                                     /* AddElement() */
    0,                                          /* nBytesPrivate */
    IL_NPF,                                     /* Init() */
    IL_NPF,                                     /* Cleanup() */
    IL_NPF,                                     /* Destroy() */
    ilExecuteBitAlign                           /* Execute() */
    };


        /*  ------------------------- ilExecute8Bit3PlaneToPixel ------------------------- */
        /*  Execute() function: convert the given # of src lines, changing
            format from planar to pixel.  Handles 3 planes of 8 bits/pixel only.
        */
static ilError ilExecute8Bit3PlaneToPixel (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilImagePlaneInfo           *pImagePlane;
long                        src0RowBytes, src1RowBytes, src2RowBytes, dstRowBytes;
long                        nPixelsM1Init;
long               nLinesM1, nPixelsM1;
ilPtr                       pSrc0Line, pSrc1Line, pSrc2Line, pDstLine;
ilPtr              pSrc0, pSrc1, pSrc2, pDst;

        /*  Get ptr to start of 3 src planes and to dst */
    pImagePlane = pData->pSrcImage->plane;
    src0RowBytes = pImagePlane->nBytesPerRow;
    pSrc0Line = pImagePlane->pPixels + pData->srcLine * src0RowBytes;
    pImagePlane++;
    src1RowBytes = pImagePlane->nBytesPerRow;
    pSrc1Line = pImagePlane->pPixels + pData->srcLine * src1RowBytes;
    pImagePlane++;
    src2RowBytes = pImagePlane->nBytesPerRow;
    pSrc2Line = pImagePlane->pPixels + pData->srcLine * src2RowBytes;

    pImagePlane = pData->pDstImage->plane;
    dstRowBytes = pImagePlane->nBytesPerRow;
    pDstLine = pImagePlane->pPixels + dstLine * dstRowBytes;

        /*  Exit if no lines or pixels. */
    nPixelsM1Init = pData->pSrcImage->width;
    if (nPixelsM1Init <= 0)
        return IL_OK;
    nPixelsM1Init--;
    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return IL_OK;
    nLinesM1--;

    do {
        pSrc0 = pSrc0Line;
        pSrc0Line += src0RowBytes;
        pSrc1 = pSrc1Line;
        pSrc1Line += src1RowBytes;
        pSrc2 = pSrc2Line;
        pSrc2Line += src2RowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes;
        nPixelsM1 = nPixelsM1Init;
        do {
            *pDst++ = *pSrc0++;
            *pDst++ = *pSrc1++;
            *pDst++ = *pSrc2++;
            } while (--nPixelsM1 >= 0);
        } while (--nLinesM1 >= 0);

    return IL_OK;
}

        /*  Table exported to ilConvert(), declared in /ilc/ilconvert.h .
        */
IL_PRIVATE ilFormatRec _ilFormat8Bit3PlaneToPixel = {
    IL_NPF,                                     /* AddElement() */
    0,                                          /* nBytesPrivate */
    IL_NPF,                                     /* Init() */
    IL_NPF,                                     /* Cleanup() */
    IL_NPF,                                     /* Destroy() */
    ilExecute8Bit3PlaneToPixel                  /* Execute() */
    };

        /*  ------------------------- ilExecute8Bit3PixelToPlane ------------------------- */
        /*  Execute() function: convert the given # of src lines, changing
            format from pixel to planar.  Handles 3 planes of 8 bits/pixel only.
        */
static ilError ilExecute8Bit3PixelToPlane (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilImagePlaneInfo           *pImagePlane;
long                        dst0RowBytes, dst1RowBytes, dst2RowBytes, srcRowBytes;
long                        nPixelsM1Init;
long               nLinesM1, nPixelsM1;
ilPtr                       pDst0Line, pDst1Line, pDst2Line, pSrcLine;
ilPtr              pDst0, pDst1, pDst2, pSrc;

        /*  Get ptr to start src plane and to 3 dst planes */
    pImagePlane = pData->pSrcImage->plane;
    srcRowBytes = pImagePlane->nBytesPerRow;
    pSrcLine = pImagePlane->pPixels + pData->srcLine * srcRowBytes;

    pImagePlane = pData->pDstImage->plane;
    dst0RowBytes = pImagePlane->nBytesPerRow;
    pDst0Line = pImagePlane->pPixels + dstLine * dst0RowBytes;
    pImagePlane++;
    dst1RowBytes = pImagePlane->nBytesPerRow;
    pDst1Line = pImagePlane->pPixels + dstLine * dst1RowBytes;
    pImagePlane++;
    dst2RowBytes = pImagePlane->nBytesPerRow;
    pDst2Line = pImagePlane->pPixels + dstLine * dst2RowBytes;

        /*  Exit if no lines or pixels. */
    nPixelsM1Init = pData->pSrcImage->width;
    if (nPixelsM1Init <= 0)
        return IL_OK;
    nPixelsM1Init--;
    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return IL_OK;
    nLinesM1--;

    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst0 = pDst0Line;
        pDst0Line += dst0RowBytes;
        pDst1 = pDst1Line;
        pDst1Line += dst1RowBytes;
        pDst2 = pDst2Line;
        pDst2Line += dst2RowBytes;
        nPixelsM1 = nPixelsM1Init;
        do {
            *pDst0++ = *pSrc++;
            *pDst1++ = *pSrc++;
            *pDst2++ = *pSrc++;
            } while (--nPixelsM1 >= 0);
        } while (--nLinesM1 >= 0);

    return IL_OK;
}

        /*  Table exported to ilConvert(), declared in /ilc/ilconvert.h .
        */
IL_PRIVATE ilFormatRec _ilFormat8Bit3PixelToPlane = {
    IL_NPF,                                     /* AddElement() */
    0,                                          /* nBytesPrivate */
    IL_NPF,                                     /* Init() */
    IL_NPF,                                     /* Cleanup() */
    IL_NPF,                                     /* Destroy() */
    ilExecute8Bit3PixelToPlane                  /* Execute() */
    };


        /*  ------------------------- ilExecute4To8Bit ------------------------- */
        /*  Execute() function: convert the given # of src lines, changing nibbles
            to bytes, by copying the nibbles to the low order of the bytes.
                Handles any number of samples, but pixel order only if multi-sample.
        */
static ilError ilExecute4To8Bit (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilImageInfo                *pSrcImage;
ilImagePlaneInfo           *pImagePlane;
long                        srcRowBytes, dstRowBytes, nBytesM1Init, nLinesM1;
ilBool                      oddNumberOfNibbles;
ilPtr                       pSrcLine, pDstLine;
long               nBytesM1;
ilPtr              pSrc, pDst;
ilByte             byte;

        /*  Exit if no lines or pixels. */
    pSrcImage = pData->pSrcImage;
    pImagePlane = &pSrcImage->plane[0];
    srcRowBytes = pImagePlane->nBytesPerRow;
    pSrcLine = pImagePlane->pPixels + pData->srcLine * srcRowBytes;

    pImagePlane = &pData->pDstImage->plane[0];
    dstRowBytes = pImagePlane->nBytesPerRow;
    pDstLine = pImagePlane->pPixels + dstLine * dstRowBytes;

    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return IL_OK;
    nLinesM1--;

        /*  Set "nBytesM1Init" to whole # of src bytes to do (2 nibbles each),
            where the # of nibbles is width * nSamplesPerPixel.
            Set "oddNumberOfNibbles" true if width is odd (do one last nibble).
            Copy each src byte (2 nibbles) to two dst bytes; do optional last nibble.
        */
    nBytesM1Init = pSrcImage->width * pSrcImage->pDes->nSamplesPerPixel;
    oddNumberOfNibbles = ((nBytesM1Init & 1) != 0);
    nBytesM1Init = (nBytesM1Init >> 1) - 1;             /* width/2 - 1 */
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes;
        nBytesM1 = nBytesM1Init;
        if (nBytesM1 >= 0) {
            do {
                byte = *pSrc++;
                *pDst++ = byte >> 4;
                *pDst++ = byte & 0xf;
                } while (--nBytesM1 >= 0);
            }
        if (oddNumberOfNibbles) {
            byte = *pSrc++;
            *pDst++ = byte >> 4;
            }
        } while (--nLinesM1 >= 0);

    return IL_OK;
}

        /*  Table exported to ilConvert(), declared in /ilc/ilconvert.h .
        */
IL_PRIVATE ilFormatRec _ilFormat4To8Bit = {
    IL_NPF,                                     /* AddElement() */
    0,                                          /* nBytesPrivate */
    IL_NPF,                                     /* Init() */
    IL_NPF,                                     /* Cleanup() */
    IL_NPF,                                     /* Destroy() */
    ilExecute4To8Bit                            /* Execute() */
    };


        /*  ------------------------- ilExecute8To4Bit ------------------------- */
        /*  Pack pipe image which is byte/pixel into nibble/pixel.
            Handles any number of samples, but pixel order only if multi-sample.
        */
static ilError ilExecute8To4Bit (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilImageInfo                *pSrcImage;
ilImagePlaneInfo           *pImagePlane;
long                        srcRowBytes, dstRowBytes, nBytesM1Init, nLinesM1;
ilBool                      oddNumberOfNibbles;
ilPtr                       pSrcLine, pDstLine;
long               nBytesM1;
ilPtr              pSrc, pDst;
ilByte             byte0, byte1;

        /*  Exit if no lines or pixels. */
    pSrcImage = pData->pSrcImage;
    pImagePlane = &pSrcImage->plane[0];
    srcRowBytes = pImagePlane->nBytesPerRow;
    pSrcLine = pImagePlane->pPixels + pData->srcLine * srcRowBytes;

    pImagePlane = &pData->pDstImage->plane[0];
    dstRowBytes = pImagePlane->nBytesPerRow;
    pDstLine = pImagePlane->pPixels + dstLine * dstRowBytes;

    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return IL_OK;
    nLinesM1--;

        /*  Set "nBytesM1Init" to whole # of dst bytes to do (2 nibbles each),
            where the # of nibbles is width * nSamplesPerPixel.
            Set "oddNumberOfNibbles" true if width is odd (do one last nibble).
            Pack each pair of src bytes into one dst nibble; do optional last nibble.
        */
    nBytesM1Init = pSrcImage->width * pSrcImage->pDes->nSamplesPerPixel;
    oddNumberOfNibbles = ((nBytesM1Init & 1) != 0);
    nBytesM1Init = (nBytesM1Init >> 1) - 1;             /* width/2 - 1 */
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes;
        nBytesM1 = nBytesM1Init;
        if (nBytesM1 >= 0) {
            do {
                byte0 = *pSrc++;
                byte1 = *pSrc++;
                *pDst++ = (byte0 << 4) | (byte1 & 0xf);
                } while (--nBytesM1 >= 0);
            }
        if (oddNumberOfNibbles) {
            byte0 = *pSrc++;
            *pDst++ = byte0 << 4;
            }
        } while (--nLinesM1 >= 0);

    return IL_OK;
}

        /*  Table exported to ilConvert(), declared in /ilc/ilconvert.h .
        */
IL_PRIVATE ilFormatRec _ilFormat8To4Bit = {
    IL_NPF,                                     /* AddElement() */
    0,                                          /* nBytesPrivate */
    IL_NPF,                                     /* Init() */
    IL_NPF,                                     /* Cleanup() */
    IL_NPF,                                     /* Destroy() */
    ilExecute8To4Bit                            /* Execute() */
    };


        /*  ------------------------- ilExecute4To8BitScaled ------------------------- */
        /*  Execute() function: convert the given # of src lines, changing nibbles
            to bytes, by copying the nibbles to the low order and high order nibbles
            of the bytes, thus scaling from 0..15 to 0..255.
                Handles any number of samples, but pixel order only if multi-sample.
        */
static ilError ilExecute4To8BitScaled (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilImageInfo                *pSrcImage;
ilImagePlaneInfo           *pImagePlane;
long                        srcRowBytes, dstRowBytes, nBytesM1Init, nLinesM1;
ilBool                      oddNumberOfNibbles;
ilPtr                       pSrcLine, pDstLine;
long               nBytesM1;
ilPtr              pSrc, pDst;
ilByte             byte;

        /*  Use bcopy to copy each line; copy the lesser of the src/dstRowBytes.
            Exit if no lines or pixels.
        */
    pSrcImage = pData->pSrcImage;
    pImagePlane = &pSrcImage->plane[0];
    srcRowBytes = pImagePlane->nBytesPerRow;
    pSrcLine = pImagePlane->pPixels + pData->srcLine * srcRowBytes;

    pImagePlane = &pData->pDstImage->plane[0];
    dstRowBytes = pImagePlane->nBytesPerRow;
    pDstLine = pImagePlane->pPixels + dstLine * dstRowBytes;

    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return IL_OK;
    nLinesM1--;

        /*  Set "nBytesM1Init" to whole # of src bytes to do (2 nibbles each),
            where the # of nibbles is width * nSamplesPerPixel.
            Set "oddNumberOfNibbles" true if width is odd (do one last nibble).
            Make from each src byte (2 nibbles) two dst bytes; do optional last nibble.
            Replicate each src nibble to upper/lower nibble of dst byte, thus scaling
            to new # of levels (from 16 to 256): 0 => 0, 1 => 17, 15 => 255.
        */
    nBytesM1Init = pSrcImage->width * pSrcImage->pDes->nSamplesPerPixel;
    oddNumberOfNibbles = ((nBytesM1Init & 1) != 0);
    nBytesM1Init = (nBytesM1Init >> 1) - 1;             /* width/2 - 1 */
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes;
        nBytesM1 = nBytesM1Init;
        if (nBytesM1 >= 0) {
            do {
                byte = *pSrc++;
                *pDst++ = (byte & 0xf0) | (byte >> 4);
                byte &= 0xf;
                *pDst++ = (byte << 4) | byte;
                } while (--nBytesM1 >= 0);
            }
        if (oddNumberOfNibbles) {
            byte = *pSrc++;
            *pDst++ = (byte & 0xf0) | (byte >> 4);
            }
        } while (--nLinesM1 >= 0);

    return IL_OK;
}


        /*  ------------------------- ilExecuteByte16To256Level ---------------------- */
        /*  Execute() function: convert the given # of src lines, changing bytes from
            16 levels to 256 levels, by replicating the low-order nibble to the high
            order nibble, thus scaling from 0..15 to 0..255.
                Handles any number of samples, but pixel order only if multi-sample.
        */
static ilError ilExecuteByte16To256Level (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilImageInfo                *pSrcImage;
ilImagePlaneInfo           *pImagePlane;
long                        srcRowBytes, dstRowBytes, nBytesM1Init, nLinesM1;
ilPtr                       pSrcLine, pDstLine;
long               nBytesM1;
ilPtr              pSrc, pDst;
ilByte             byte;

        /*  Use bcopy to copy each line; copy the lesser of the src/dstRowBytes.
            Exit if no lines or pixels.
        */
    pSrcImage = pData->pSrcImage;
    pImagePlane = &pSrcImage->plane[0];
    srcRowBytes = pImagePlane->nBytesPerRow;
    pSrcLine = pImagePlane->pPixels + pData->srcLine * srcRowBytes;

    pImagePlane = &pData->pDstImage->plane[0];
    dstRowBytes = pImagePlane->nBytesPerRow;
    pDstLine = pImagePlane->pPixels + dstLine * dstRowBytes;

    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return IL_OK;
    nLinesM1--;

        /*  Set "nBytesM1Init" to whole # of src bytes to do, = width * # samples/pixel
            (note that each sample must be a byte!)
            Replicate low-order nibble to upper nibble of dst byte, thus scaling
            to new # of levels (from 16 to 256): 0 => 0, 1 => 17, 15 => 255.
        */
    nBytesM1Init = pSrcImage->width * pSrcImage->pDes->nSamplesPerPixel - 1;
    if (nBytesM1Init < 0)
        return IL_OK;

    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes;
        nBytesM1 = nBytesM1Init;
        do {
            byte = *pSrc++;
            byte &= 0xf;
            *pDst++ = (byte << 4) | byte;
            } while (--nBytesM1 >= 0);
        } while (--nLinesM1 >= 0);

    return IL_OK;
}


        /*  ---------------------- ilAddLevelConversionFilter ------------------------ */
        /*  Add a conversion filter to the given pipe which converts from the levels
            in "*pPipeDes" (i.e. the levels of the current pipe image) to those in
            "*pDes" (note - only nLevelsPerSample is reference from pDes).
            If failure, declares pipe invalid (which sets error code) and returns false; 
            else returns true and "*pInfo", "*pPipeDes" and "*pFormat" are updated
            with the new pipe state info.
        */
IL_PRIVATE ilBool _ilAddLevelConversionFilter (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pPipeDes,
    const ilImageDes       *pDes,
    ilImageFormat          *pFormat
    )
{
unsigned int                sample;
ilPtr                       pPriv;
ilDstElementData            dstData;
ilError                     (*executeFunction)();
ilBool                      all4Bit, all8Bit;

        /*  Determine if we support requested conversion.
            Currently only support: 4 or 8 bit, 16 level => 8 bit, 256 level, any # of
            samples/pixel, but pixel mode only if # samples > 1.
        */
    if ((pDes->nSamplesPerPixel > 1) && (pFormat->sampleOrder != IL_SAMPLE_PIXELS))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_CANT_CONVERT);

    all4Bit = all8Bit = TRUE;
    for (sample = 0; sample < pPipeDes->nSamplesPerPixel; sample++) {
        if ((pPipeDes->nLevelsPerSample[sample] != 16)
         || (pDes->nLevelsPerSample[sample] != 256))
            return ilDeclarePipeInvalid (pipe, IL_ERROR_CANT_CONVERT);
        pPipeDes->nLevelsPerSample[sample] = 256;

        if (pFormat->nBitsPerSample[sample] == 4) {
            all8Bit = FALSE;
            pFormat->nBitsPerSample[sample] = 8;
            }
        else if (pFormat->nBitsPerSample[sample] == 8)
            all4Bit = FALSE;
        else return ilDeclarePipeInvalid (pipe, IL_ERROR_CANT_CONVERT);
        }

        /*  All src # levels == 16, dst == 256.  If all src formats are 4 bit or 8 bit,
            can handle, else error.  pFormat->nBitsPerSample now == 8 for all samples.
        */
    if (all4Bit)
        executeFunction = ilExecute4To8BitScaled;
    else if (all8Bit)
        executeFunction = ilExecuteByte16To256Level;
    else return ilDeclarePipeInvalid (pipe, IL_ERROR_CANT_CONVERT);

        /*  Update *pPipeDes, *pFormat and add filter or error.
            Update pInfo after successful add; pPipeDes and pFormat already updated.
        */
    dstData.producerObject = (ilObject)NULL;
    dstData.pDes = pPipeDes;
    dstData.pFormat = pFormat;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = 0;
    dstData.constantStrip = FALSE;
    dstData.pPalette = pInfo->pPalette;
    dstData.pCompData = pInfo->pCompData;
    pPriv = ilAddPipeElement (pipe, IL_FILTER, 0, 0,
                (ilSrcElementData *)NULL, &dstData, 
                IL_NPF, IL_NPF, IL_NPF, executeFunction, NULL, 0);
    if (!pPriv) return FALSE;                               /* EXIT */

    ilGetPipeInfo (pipe, FALSE, pInfo, (ilImageDes *)NULL, (ilImageFormat *)NULL);
    return TRUE;
}


