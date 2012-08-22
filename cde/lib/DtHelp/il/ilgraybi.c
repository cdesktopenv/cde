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
/* $XConsortium: ilgraybi.c /main/6 1996/10/30 11:09:02 drk $ */
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

#include <stdlib.h>

#include "ilint.h"
#include "ilpipelem.h"
#include "ilconvert.h"
#include "ilerrors.h"

#ifndef LSB_BIT_ORDER
# define flip_bits(start,n)      /*EMPTY*/
#else
extern void flip_bits(register ilPtr start, register unsigned n);

extern void
flip_bits(register ilPtr start,
          register unsigned n )
{
	register int	i;
	unsigned char   c;

	for (i = 0; i < n; i++) {
	    c = *start;
	    *start++ = ilBitReverseTable[c];
	}
}
#endif

/*  =========================== BITONAL DITHER CODE =============================== */ 

        /*  Private data for error diffusion to bitonal, inited by Init() function. */
typedef struct {
    long            width;                  /* width of src/dst images */
    long            srcRowBytes;            /* bytes/row of src image */
    ilPtr           pSrcPixels;             /* ptr to start of src pixels */
    long            dstRowBytes;            /* bytes/row of dst image */
    ilPtr           pDstPixels;             /* ptr to start of dst pixels */
    ilBool          blackIsZero;            /* src image: true if 0 is black, else white */
    short          *pError1, *pError2;      /* ptr to error accumulator buffers */
    } ilBiDiffusionPrivRec, *ilBiDiffusionPrivPtr;


        /*  Init() function: malloc and zero error accumulator buffers. */
static ilError ilInitBiDiffusion (
    ilBiDiffusionPrivPtr pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
    pPriv->width = pSrcImage->width;
    pPriv->srcRowBytes = pSrcImage->plane[0].nBytesPerRow;
    pPriv->pSrcPixels = pSrcImage->plane[0].pPixels;
    pPriv->dstRowBytes = pDstImage->plane[0].nBytesPerRow;
    pPriv->pDstPixels = pDstImage->plane[0].pPixels;
    pPriv->blackIsZero = pSrcImage->pDes->blackIsZero;

        /*  Create error accumulator buffers, 2 larger than width because the pixels
            above to the left and right are examined.
        */
    pPriv->pError1 = (short *)IL_MALLOC_ZERO (sizeof (short) * (pPriv->width + 2));
    pPriv->pError2 = (short *)IL_MALLOC_ZERO (sizeof (short) * (pPriv->width + 2));
    if (!pPriv->pError1 || !pPriv->pError2)
        return IL_ERROR_MALLOC;

    return IL_OK;
}

        /*  Cleanup() function: dispose error accumulator buffers. */
static ilError ilCleanupBiDiffusion (
    ilBiDiffusionPrivPtr pPriv
    )
{
    if (pPriv->pError1)
        IL_FREE (pPriv->pError1);
    if (pPriv->pError2)
        IL_FREE (pPriv->pError2);
    return IL_OK;
}

        /*  ----------------------- ilDiffuseGrayToBitonal ------------------------- */
        /*  Dithers using error diffusion to bitonal.
            Input image:  IL_DES_GRAY, IL_FORMAT_BYTE.
            Output image: IL_DES_BITONAL, IL_FORMAT_BIT
        */

        /*  Execute() function: dither and pack the given # of src lines.  */
static ilError ilExecuteBiDiffusion (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines             /* ignored on input */
    )
{
#ifdef LSB_BIT_ORDER
# define SPECIAL_MASK_BIT    0x00000001          /* for LSB bit order */
# define SHIFT_MASK(m)       ((m) <<= 1)
#else
# define SPECIAL_MASK_BIT    0x80000000          /* for MSB bit order */
# define SHIFT_MASK(m)       ((m) >>= 1)
#endif
ilBiDiffusionPrivPtr        pPriv;
long                        nLinesM1, nPixelsM1Init, nPixelsM1;
register short             *pPrevError, *pError;
register int                errorAcc, invert;
long                        srcRowBytes, dstRowBytes;
ilPtr                       pSrcLine, pDstLine;
register ilPtr              pSrc;
register CARD32             mask, *pDst;
CARD32			    outLong;

    pPriv = (ilBiDiffusionPrivPtr)pData->pPrivate;
    srcRowBytes = pPriv->srcRowBytes;
    pSrcLine = pPriv->pSrcPixels + pData->srcLine * srcRowBytes;
    dstRowBytes = pPriv->dstRowBytes;
    pDstLine = pPriv->pDstPixels + dstLine * dstRowBytes;

    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return IL_OK;
    nLinesM1--;

    nPixelsM1Init = pPriv->width;
    if (nPixelsM1Init <= 0)
        return IL_OK;
    nPixelsM1Init--;

        /*  Set "invert" to 0 if blackIsZero, else to 0xff, and xor each src pixel
            with invert.  Thus if 0 is white, the xor turns 0 into 255, 1 into 254, etc.
        */
    invert = (pPriv->blackIsZero) ? 0 : 0xff;

        /*  Dither: do Floyd-Steinberg dither (error diffusion) requiring src to be
            256 gray levels.  True error diffusion would map:
                (srcLevels-1)/(dstLevels-1) = 255/1
            but we will use 256/1 to make muls/divs easy.  Use the following:
                    1/16    5/16    3/16
                    7/16     x
            where the fractions indicate what portion of the error from the surrounding
            pixels should be added to this pixels value (x) to determine dst pixel to
            write at "x".
        */
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst = (CARD32 *)pDstLine;
        pDstLine += dstRowBytes;

            /*  Point to error accumulator buffers and switch, so *pError becomes
                *pPrevError for next line.  Bump ptrs by 1: prev scan line error is
                examined above to left and right: need zero extra value there to handle
                beginning/end of line.
            */
        pError = pPriv->pError1;
        pPrevError = pPriv->pError2;
        pPriv->pError1 = pPrevError;
        pPriv->pError2 = pError;
        pError++; 
        pPrevError++;
        errorAcc = 0;                           /* holds error from pixel to the left */

        mask = SPECIAL_MASK_BIT;

        outLong = 0;
        nPixelsM1 = nPixelsM1Init;
        do {
            errorAcc *= 7;                      /* 7 * error to left */
            errorAcc += *(pPrevError - 1);      /* 1 * error above left */
            errorAcc += 5 * *pPrevError++;      /* 5 * error above */
            errorAcc += 3 * *pPrevError;        /* 3 * error above right */
            errorAcc >>= 4;                     /* /16 */
            errorAcc += *pSrc++ ^ invert;       /* + "x"; w/ blackIsZero handled */
            if (errorAcc >= 128)                /* output white: dont set out bit */
                errorAcc -= 255;                /* sub value of white from errorAcc */
            else                                /* output black: errorAcc -= 0 */
                outLong |= mask;
            *pError++ = errorAcc;               /* store error into buffer */
            if (!(SHIFT_MASK(mask))) {
                flip_bits((ilPtr)&outLong, sizeof(outLong));
                *pDst++ = outLong;
                mask = SPECIAL_MASK_BIT;
                outLong = 0;
                }
            } while (--nPixelsM1 >= 0);

        if (mask != SPECIAL_MASK_BIT) {         /* bits left in outLong; write them */
            flip_bits((ilPtr)&outLong, sizeof(outLong));
            *pDst++ = outLong;
	}

    } while (--nLinesM1 >= 0);

    return IL_OK;
}

        /*  Table exported to ilConvert(), declared in /ilc/ilconvert.h . */
IL_PRIVATE ilConvertRec _ilDiffuseGrayToBitonal = {
    IL_NPF,                                     /* CheckFormat() */
    IL_STD_FORMAT_BYTE,                         /* srcFormatCode */
    IL_NPF,                                     /* AddElement() */
    IL_DES_BITONAL,                             /* pDstDes */
    IL_FORMAT_BIT,                              /* pDstFormat */
    sizeof (ilBiDiffusionPrivRec),              /* nBytesPrivate */
    ilInitBiDiffusion,                          /* Init() */
    ilCleanupBiDiffusion,                       /* Cleanup() */
    IL_NPF,                                     /* Destroy() */
    ilExecuteBiDiffusion                        /* Execute() */
    };


/*  =========================== BITONAL THRESHOLD CODE =============================== */ 

        /*  ----------------------- ilThresholdGrayToBitonal ------------------------- */
        /*  Convert gray to bitonal using a user-defined threshold.
            Input image:  IL_DES_GRAY, IL_FORMAT_BYTE.
            Output image: IL_DES_BITONAL, IL_FORMAT_BIT
        */

        /*  Private data for threshold conversion, inited by Init() function. */
typedef struct {
    CARD32         *pThreshold;             /* ptr to threshold for such gray->bi cvts */
    long            width;                  /* width of src/dst images */
    long            srcRowBytes;            /* bytes/row of src image */
    ilPtr           pSrcPixels;             /* ptr to start of src pixels */
    long            dstRowBytes;            /* bytes/row of dst image */
    ilPtr           pDstPixels;             /* ptr to start of dst pixels */
    ilBool          blackIsZero;            /* true if 0 is black, else is white */
    } ilThresholdPrivRec, *ilThresholdPrivPtr;

        /*  AddElement function: save copy of pThreshold (pOptionData to ilConvert())
            into private.  This ptr is dereferenced every time the pipe is run, allowing
            the caller to change the threshold without reforming the pipe.
        */
static ilError ilAddElementThreshold (
    ilThresholdPrivPtr  pPriv,
    unsigned short     *pPalette,       /* not used */
    CARD32             *pThreshold      /* pOptionData to ilConvert() */
    )
{
    pPriv->pThreshold = pThreshold;
    return IL_OK;
}

        /*  Init() function: init the counter of "y" within private; load image pixel
            address and rowBytes into private for faster reference in Execute().
        */
static ilError ilInitThreshold (
    ilThresholdPrivPtr  pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
    pPriv->width = pSrcImage->width;
    pPriv->srcRowBytes = pSrcImage->plane[0].nBytesPerRow;
    pPriv->pSrcPixels = pSrcImage->plane[0].pPixels;
    pPriv->dstRowBytes = pDstImage->plane[0].nBytesPerRow;
    pPriv->pDstPixels = pDstImage->plane[0].pPixels;

    pPriv->blackIsZero = pSrcImage->pDes->blackIsZero;
    return IL_OK;
}

        /*  Execute() function: dither and pack the given # of src lines.
        */
static ilError ilExecuteThreshold (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines             /* ignored on input */
    )
{
#ifdef LSB_BIT_ORDER
# define SPECIAL_MASK_BIT    0x00000001          /* for LSB bit order */
# define SHIFT_MASK(m)       ((m) <<= 1)
#else
# define SPECIAL_MASK_BIT    0x80000000          /* for MSB bit order */
# define SHIFT_MASK(m)       ((m) >>= 1)
#endif
ilThresholdPrivPtr          pPriv;
long                        nLinesM1, nPixelsM1Init;
long                        srcRowBytes, dstRowBytes;
ilPtr                       pSrcLine, pDstLine;
register long               nPixelsM1;
register ilPtr              pSrc;
register CARD32             mask, *pDst;
CARD32			    outLong;
register ilByte             threshold;

    pPriv = (ilThresholdPrivPtr)pData->pPrivate;
    threshold = *pPriv->pThreshold;
    srcRowBytes = pPriv->srcRowBytes;
    pSrcLine = pPriv->pSrcPixels + pData->srcLine * srcRowBytes;
    dstRowBytes = pPriv->dstRowBytes;
    pDstLine = pPriv->pDstPixels + dstLine * dstRowBytes;

    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return IL_OK;;
    nLinesM1--;
    nPixelsM1Init = pPriv->width;
    if (nPixelsM1Init <= 0)
        return IL_OK;
    nPixelsM1Init--;

        /*  For each pixel, compare to the threshold and output black if 
            < threshold (0 = black) or >= threshold (0 = white).
        */
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst = (CARD32 *)pDstLine;
        pDstLine += dstRowBytes;
        mask = SPECIAL_MASK_BIT;
        outLong = 0;
        nPixelsM1 = nPixelsM1Init;

        if (pPriv->blackIsZero) {
            do {
                if (*pSrc++ < threshold)
                     outLong |= mask;
                if (! SHIFT_MASK(mask)) {
                    flip_bits((ilPtr)&outLong, sizeof(outLong));
                    *pDst++ = outLong;
                    mask = SPECIAL_MASK_BIT;
                    outLong = 0;
                    }
                } while (--nPixelsM1 >= 0);
            }
        else {
            do {
                if (*pSrc++ >= threshold)
                     outLong |= mask;
                if (! SHIFT_MASK(mask)) {
                    flip_bits((ilPtr)&outLong, sizeof(outLong));
                    *pDst++ = outLong;
                    mask = SPECIAL_MASK_BIT;
                    outLong = 0;
                    }
                } while (--nPixelsM1 >= 0);
            }

            /*  If mask != left bit on, some bits in outLong; output them.  Next line.
            */
        if (mask != SPECIAL_MASK_BIT) {
            flip_bits((ilPtr)&outLong, sizeof(outLong));
            *pDst++ = outLong;
          }

        } while (--nLinesM1 >= 0);

    return IL_OK;
}

        /*  Table exported to ilConvert(), declared in /ilc/ilconvert.h .
            Convert using a user-supplied threshold.
            Input image:  IL_DES_GRAY (either form of blackIsZero), IL_FORMAT_BYTE.
            Output image: IL_DES_BITONAL, IL_FORMAT_BIT
        */
IL_PRIVATE ilConvertRec _ilThresholdGrayToBitonal = {
    IL_NPF,                                     /* CheckFormat() */
    IL_STD_FORMAT_BYTE,                         /* srcFormatCode */
    ilAddElementThreshold,                      /* AddElement() */
    IL_DES_BITONAL,                             /* pDstDes */
    IL_FORMAT_BIT,                              /* pDstFormat */
    sizeof (ilThresholdPrivRec),                /* nBytesPrivate */
    ilInitThreshold,                            /* Init() */
    IL_NPF,                                     /* Cleanup() */
    IL_NPF,                                     /* Destroy() */
    ilExecuteThreshold                          /* Execute() */
    };

