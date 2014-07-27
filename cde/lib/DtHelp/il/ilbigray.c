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
/* $XConsortium: ilbigray.c /main/6 1996/09/24 17:12:45 drk $ */
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

        /*  /ilc/ilbigray.c : Contains ilBitonalToGray(), which adds a pipe element 
            to convert the pipe image (which must be IL_BITONAL) to gray scale.
        */

#include "ilint.h"
#include "ilpipelem.h"
#include "ilscaleint.h"
#include "ilerrors.h"

#ifdef LSB_BIT_ORDER
extern void flip_bits(register ilPtr start, register unsigned n);
#else
# define flip_bits(s,n)          /*EMPTY*/
#endif

#define IL_FILTER_SIZE         3                   /* size of one side of filter square */
#define IL_FILTER_SQUARE (IL_FILTER_SIZE*IL_FILTER_SIZE)  /* square size of filter */
#define MIN_DESIRED_STRIP     16                   /* arbitrary: desired size of strips */

        /*  Private data for pipe elements.
            See comments under ilInit9BitFilter() for a description of "filterValues".
        */
typedef struct {

        /*  Data inited by ilScaleBitonalToGray() when pipe element added. */
    ilByte          filterValues [512];         /* should be first for efficiency! */
    ilBool          bitonalOutput;              /* true: outputting bitonal, else gray */
    int            *pGrayThreshold;             /* bitonal out only: ptr to threshold */
    int             defaultGrayThreshold;       /* pGrayThreshold -> this if defaulting */
    long            srcWidth, srcHeight;        /* size of src (pipe) image */
    long            dstWidth, dstHeight;        /* size of dst (output) image */
    long            widthDiff, heightDiff;      /* src - dst of width/height */
    long            srcBufferHeight;            /* height of perm src image, else 0 */

        /*  Data inited by ilBiGrayInit() */
    unsigned int    grayThreshold;              /* *pGrayThreshold for this pass */
    long            lineAcc;                    /* line accumulator for vert scaling */
    long            srcRowBytes;                /* bytes/row of src image */
    ilPtr           pSrcPixels;                 /* ptr to start of src pixels */
    long            dstRowBytes;                /* bytes/row of dst image */
    ilPtr           pDstPixels;                 /* ptr to start of dst pixels */
    } ilBiGrayPrivRec, *ilBiGrayPrivPtr;


        /*  --------------------------- ilInit9BitFilter ---------------------------- */
        /*  Init "*pTable", a 512 entry table, with the values to be indexed by a 9 bit
            number formed as follows:
                             -----------
                            | 8 | 7 | 6 |   <- top source line
                             -----------
                            | 5 | 4 | 3 |   <- bit "4" is logical center point
                             -----------
                            | 2 | 1 | 0 |
                             -----------
            The bits are shifted into a 9 bit number, as above, then used as an index
            into the table.  The value in entry 0xff (all bits on) should equal
            the max gray value ("nLevels" - 1).
        */
static void ilInit9BitFilter (
    long            srcWidth,
    long            srcHeight,
    long            dstWidth,
    long            dstHeight,
    unsigned long   nLevels,
    ilBool          blackIsZero,
    ilByte         *pTable
    )
{
double              filter [IL_FILTER_SQUARE];
double              fraction, widthScale, heightScale, totalArea, widthSmall, heightSmall;
long                value, nLevelsM1;
int                 i;

        /*  Setup "filter" with the fractional (0..1) contribution of each pixel to
            the whole, in the bit order described above (bit 0 is the top-left pixel).
            The sum of all values should ~= 1.0.  The portion of each pixel selected
            is based on a square of size "srcWidth/dstWidth" by "srcHeight/dstHeight",
            mapped over the source pixels, centered on pixel 4 (above).  If the scale
            factor is greater than 3.0 in either direction, make it 3.0 (all pixels
            contribute equally in that direction.
        */
    widthScale = (double)srcWidth / (double)dstWidth;
    if (widthScale > 3.0)
        widthScale = 3.0;
    heightScale = (double)srcHeight / (double)dstHeight;
    if (heightScale > 3.0)
        heightScale = 3.0;
    totalArea = widthScale * heightScale;

        /*  The center pixel is a 1x1 square.
            The four corner pixels are overlayed by a rect of width or height (wh) =
            (wh - 1) / 2: the whole side is "wh", - 1 for center, / 2 cause there are 2.
            The four side pixels have width/height of corner pixels, but other dim = 1.
        */
    filter [4] = 1.0 / totalArea;
    widthSmall = (widthScale - 1.0) / 2.0;
    heightSmall = (heightScale - 1.0) / 2.0;
    filter [0] = filter [2] = filter [6] = filter [8] = 
        (widthSmall * heightSmall) / totalArea;
    filter [1] = filter [7] = heightSmall / totalArea;
    filter [3] = filter [5] = widthSmall / totalArea;

        /*  Convert the filter to an array indexed by a 9 bit number: the filter bits
            as shown above, in order "876543210".  If whitePixel is 0, add the filter
            fraction if bit is off. If whitePixel not zero than add the filter fraction 
            if bit is on.
        */
    nLevelsM1 = nLevels - 1;
    if (!blackIsZero) {
        for (i = 0; i < 512; i++) {
            fraction = 0.0;
            if (!(i & 1))   fraction += filter [0];
            if (!(i & 2))   fraction += filter [1];
            if (!(i & 4))   fraction += filter [2];
            if (!(i & 8))   fraction += filter [3];
            if (!(i & 16))  fraction += filter [4];
            if (!(i & 32))  fraction += filter [5];
            if (!(i & 64))  fraction += filter [6];
            if (!(i & 128)) fraction += filter [7];
            if (!(i & 256)) fraction += filter [8];
            value = fraction * nLevelsM1 + 0.5;
            if (value > nLevelsM1)
                value = nLevelsM1;
            pTable [i] = value;
            }
        }
    else {          /* a 1 is white */
        for (i = 0; i < 512; i++) {
            fraction = 0.0;
            if (i & 1)   fraction += filter [0];
            if (i & 2)   fraction += filter [1];
            if (i & 4)   fraction += filter [2];
            if (i & 8)   fraction += filter [3];
            if (i & 16)  fraction += filter [4];
            if (i & 32)  fraction += filter [5];
            if (i & 64)  fraction += filter [6];
            if (i & 128) fraction += filter [7];
            if (i & 256) fraction += filter [8];
            value = fraction * nLevelsM1 + 0.5;
            if (value > nLevelsM1)
                value = nLevelsM1;
            pTable [i] = value;
            }
        }
}

        /*  --------------------- ilBiGrayInit -------------------------- */
        /*  Init() function: copy values from given images to private for fast reference.
        */
static ilError ilBiGrayInit (
    ilBiGrayPrivPtr        pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{

    pPriv->lineAcc = pPriv->heightDiff;
    pPriv->srcRowBytes = pSrcImage->plane[0].nBytesPerRow;
    pPriv->pSrcPixels = pSrcImage->plane[0].pPixels;
    pPriv->dstRowBytes = pDstImage->plane[0].nBytesPerRow;
    pPriv->pDstPixels = pDstImage->plane[0].pPixels;

        /*  Copy caller's threshold if scaling to bitonal.
        */
    if (pPriv->bitonalOutput)
        pPriv->grayThreshold = *pPriv->pGrayThreshold;

    return IL_OK;
}

        /*  --------------------- ilBiGrayExecute -------------------------- */
        /*  Execute() pipe element function for scaling bitonal to gray.
        */
static ilError ilBiGrayExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
#define ULPTR               CARD32 *
#ifdef LSB_BIT_ORDER
# define SPECIAL_MASK_BIT    0x00000001          /* for LSB bit order */
# define SHIFT_MASK(m)       ((m) <<= 1)
#else
# define SPECIAL_MASK_BIT    0x80000000          /* for MSB bit order */
# define SHIFT_MASK(m)       ((m) >>= 1)
#endif
long                        nLines, nLinesWritten;
ilPtr                       pSrcLine, pDstLine;
long                        colAcc, nDstBits, dstWidth, bottomLine, line;
register long               srcBytesAbove, srcBytesBelow;
register ilBiGrayPrivPtr    pPriv;
register CARD32             index, *pSrc;
register int                shift, bitNumber;

        /*  Point pSrcLine to srcLine: = middle line of 3x3 matrix.
            Set bottomLine to last available line in the buffer.
        */
    pPriv = (ilBiGrayPrivPtr)pData->pPrivate;
    dstWidth = pPriv->dstWidth;
    pSrcLine = pPriv->pSrcPixels + pData->srcLine * pPriv->srcRowBytes;
    pDstLine = pPriv->pDstPixels + dstLine * pPriv->dstRowBytes;

    nLines = *pNLines;
    if (nLines <= 0)
        return 0;                               /* no lines, EXIT */
    if (pPriv->srcBufferHeight)
         bottomLine = pPriv->srcBufferHeight - 1;
    else bottomLine = pData->srcLine + nLines - 1;
    line = pData->srcLine - 1;                  /* line of top of 3x3 matrix */

    nLinesWritten = 0;
    while (TRUE) {
        while (pPriv->lineAcc > 0) {
            if (nLines-- <= 0) goto BGLinesDone;
            pPriv->lineAcc -= pPriv->dstHeight;
            pSrcLine += pPriv->srcRowBytes;
            line++;
            }
        if (nLines-- <= 0) goto BGLinesDone;
        pPriv->lineAcc += pPriv->heightDiff;

            /*  Algorithm: skip src bits, using basic scale algorithm.  To form dst bit,
                form 9 bit value from 3x3, with top bits in high order bits.  Lookup
                the 9 bit value in table of correct gray scale.  "bitNumber" is the
                # of the left bit of 3x3; 31 = left bit of long.  If bitNumber is < 2,
                the bits cross a long boundary: get bits from bytes, low order of this
                long and high order byte of next long.
                    pSrc will point to the middle line of 3x3; the line above is 
                "-srcBytesAbove" away; the line below is "srcBytesBelow" away.  If the 3x3
                is outside of the buffer being read, set "srcBytesAbove/Below" to 0,
                causing the middle line to be re-read and replicated to replace the
                unaccessible line.
            */
        srcBytesAbove = (line < 0) ? 0 : pPriv->srcRowBytes;
        srcBytesBelow = ((line + 2) > bottomLine) ? 0 : pPriv->srcRowBytes;
        nDstBits = pPriv->dstWidth;

            /*  Set pSrc and bitNumber to point to the rightmost bit of the long before
                the first long.  The "colAcc > 0" check below will be true, causing a bump
                to the next bit (first bit in row), _except_ when "widthDiff" is 0 (no
                hori scaling).  In this case, set colAcc to 1, forcing the first colAcc>0
                check to be true and bumping past first bit - the rest proceeds normally.
            */
        pSrc = ((ULPTR)pSrcLine) - 1;
        bitNumber = 0;
        colAcc = pPriv->widthDiff;
        if (colAcc <= 0)
            colAcc = 1;

            /*  If bitonal output, logically produce a gray pixel, then output a 1 if
                gray is < threshold.
            */
        if (pPriv->bitonalOutput) {
            ULPTR           pDst;
            CARD32          mask, outLong;

            pDst = (ULPTR)pDstLine;
            mask = SPECIAL_MASK_BIT;
            outLong = 0;                /* fill with 1's as necessary */

            while (TRUE) {
                while (colAcc > 0) {
                    colAcc -= dstWidth;
                    if (--bitNumber < 0) {
                        pSrc++;
                        bitNumber = 31;
                        }
                    }
                colAcc += pPriv->widthDiff;

                   /*  If done and mask != left bit on, output bits in "outLong". */
                if (nDstBits <= 0) {
                    if (mask != SPECIAL_MASK_BIT) {
			 flip_bits((ilPtr)&outLong, sizeof(outLong));
                        *pDst++ = outLong;
		     }
                    break;                      /* done this line; exit while */
                    }
                if (--nDstBits <= 0) {
                    INT32 rightBit = (((ilPtr)pSrc - pSrcLine) << 3) + 34 - bitNumber;
                    if ((rightBit > pPriv->srcWidth) && (pPriv->srcWidth > 3))
                        if (++bitNumber > 31) {bitNumber = 0; pSrc--; }
                    }
                if (bitNumber >= 2) {
                    shift = bitNumber - 2;
                    index = (*((ULPTR)((ilPtr)pSrc - srcBytesAbove)) >> shift) & 7;
                    index <<= 3;
                    index |= (*pSrc >> shift) & 7;
                    index <<= 3;
                    index |= (*((ULPTR)((ilPtr)pSrc + srcBytesBelow)) >> shift) & 7;
                    }
                else {
                    ilPtr pSrcTemp = (ilPtr)pSrc - srcBytesAbove + 3;
                    shift = 6 + bitNumber;
                    index = ((CARD32)((*pSrcTemp << 8) | *(pSrcTemp + 1)) >> shift) & 7;
                    index <<= 3;
                    pSrcTemp += srcBytesAbove;
                    index |= ((CARD32)((*pSrcTemp << 8) | *(pSrcTemp + 1)) >> shift) & 7;
                    index <<= 3;
                    pSrcTemp += srcBytesBelow;
                    index |= ((CARD32)((*pSrcTemp << 8) | *(pSrcTemp + 1)) >> shift) & 7;
                    }
                if (pPriv->filterValues [index] < pPriv->grayThreshold)
                    outLong |= mask;
                if (!(SHIFT_MASK(mask))) {
                    flip_bits((ilPtr)&outLong, sizeof(outLong));
                    *pDst++ = outLong;
                    mask = SPECIAL_MASK_BIT;
                    outLong = 0;
                    }
                if (--bitNumber < 0) {              /* next src bit */
                    pSrc++;
                    bitNumber = 31;
                    }
                }   /* END column loop */
            }
        else {      /* outputting gray */
            ilPtr       pDst;
            pDst = pDstLine;

            while (TRUE) {
                while (colAcc > 0) {
                    colAcc -= dstWidth;
                    if (--bitNumber < 0) {
                        pSrc++;
                        bitNumber = 31;
                        }
                    }
                colAcc += pPriv->widthDiff;
                if (nDstBits <= 0)
                    break;                      /* done this line; exit while */
                if (--nDstBits <= 0) {
                    INT32 rightBit = (((ilPtr)pSrc - pSrcLine) << 3) + 34 - bitNumber;
                    if ((rightBit > pPriv->srcWidth) && (pPriv->srcWidth > 3))
                        if (++bitNumber > 31) {bitNumber = 0; pSrc--; }
                    }
                if (bitNumber >= 2) {
                    shift = bitNumber - 2;
                    index = (*((ULPTR)((ilPtr)pSrc - srcBytesAbove)) >> shift) & 7;
                    index <<= 3;
                    index |= (*pSrc >> shift) & 7;
                    index <<= 3;
                    index |= (*((ULPTR)((ilPtr)pSrc + srcBytesBelow)) >> shift) & 7;
                    }
                else {
                    ilPtr pSrcTemp = (ilPtr)pSrc - srcBytesAbove + 3;
                    shift = 6 + bitNumber;
                    index = ((CARD32)((*pSrcTemp << 8) | *(pSrcTemp + 1)) >> shift) & 7;
                    index <<= 3;
                    pSrcTemp += srcBytesAbove;
                    index |= ((CARD32)((*pSrcTemp << 8) | *(pSrcTemp + 1)) >> shift) & 7;
                    index <<= 3;
                    pSrcTemp += srcBytesBelow;
                    index |= ((CARD32)((*pSrcTemp << 8) | *(pSrcTemp + 1)) >> shift) & 7;
                    }
                *pDst++ = pPriv->filterValues [index];
                if (--bitNumber < 0) {              /* next src bit */
                    pSrc++;
                    bitNumber = 31;
                    }
                }   /* END column loop */
            }

            /*  Line done; next src, dst lines; inc line index and # lines written.
            */
        pSrcLine += pPriv->srcRowBytes;
        line++;
        pDstLine += pPriv->dstRowBytes;
        nLinesWritten++;
        }       /* END loop, one dst line */

BGLinesDone:
    *pNLines = nLinesWritten;
    return IL_OK;
}


    /*  ------------------------- ilScaleBitonalToGray ---------------------------- */
    /*  Called by ilScale().
        Adds an element to "pipe" to scale the pipe image to "dstWidth" by "dstHeight"
        which both must be > 0.  The input pipe image must be a bitonal,
        bit-per-pixel image, with the given "blackIsZero".  "pInfo" points to the
        pipe info, "dstWidth/Height" is the (> 0) size to scale to, and nGrayLevels
        is the number of levels of gray.  The output image is a gray image.
            Only a scale down in both directions is allowed; the following must be true:
        srcWidth must be >= dstWidth and srcHeight must be >= dstHeight!
    */
IL_PRIVATE void _ilScaleBitonalToGray (
    ilPipe              pipe,
    unsigned long       dstWidth,
    unsigned long       dstHeight,
    unsigned long       nGrayLevels,
    ilBool              blackIsZero,
    ilPipeInfo         *pInfo
    )
{
register ilBiGrayPrivPtr pPriv;
ilDstElementData        dstData;
ilImageDes              des;


    /* Idiot check the number of gray levels */

    if ((nGrayLevels < 2) || (nGrayLevels > 256)) {
        ilDeclarePipeInvalid (pipe, IL_ERROR_LEVELS_PER_SAMPLE);
        return;                                                     /* EXIT */
        }

        /*  Add a filter to do the scale, then init *pPriv.
        */
    dstData.producerObject = (ilObject)NULL;
    des = *IL_DES_GRAY;
    des.nLevelsPerSample[0] = nGrayLevels;
    dstData.pDes = &des;
    dstData.pFormat = IL_FORMAT_BYTE;
    dstData.width = dstWidth;
    dstData.height = dstHeight;
    dstData.stripHeight = ((dstHeight * pInfo->recommendedStripHeight) / pInfo->height) + 1;
    dstData.constantStrip = FALSE;
    dstData.pPalette = (unsigned short *)NULL;
    pPriv = (ilBiGrayPrivPtr)ilAddPipeElement (pipe, IL_FILTER, sizeof (ilBiGrayPrivRec), 
                         IL_ADD_PIPE_HOLD_SRC, (ilSrcElementData *)NULL, &dstData, 
                         ilBiGrayInit, IL_NPF, IL_NPF, ilBiGrayExecute, NULL, 0);
    if (!pPriv)
        return;

        /*  Pipe element added, init pPriv.  If not a tempImage, reading directly from
            a permanent image: set srcHeight to image height; else set to 0.
        */
    pPriv->bitonalOutput = FALSE;
    pPriv->srcWidth = pInfo->width;
    pPriv->srcHeight = pInfo->height;
    pPriv->dstWidth = dstWidth;
    pPriv->dstHeight = dstHeight;
    pPriv->widthDiff = pInfo->width - dstWidth;
    pPriv->heightDiff = pInfo->height - dstHeight;
    if (!pInfo->tempImage)
         pPriv->srcBufferHeight = pInfo->height;
    else pPriv->srcBufferHeight = 0;

        /*  Init the filter table in private based on scale factors and # gray levels.
        */
    ilInit9BitFilter (pPriv->srcWidth, pPriv->srcHeight, pPriv->dstWidth, 
                      pPriv->dstHeight, nGrayLevels, blackIsZero, pPriv->filterValues);

    pipe->context->error = IL_OK;
}


    /*  ------------------------- ilAreaScaleBitonal ---------------------------- */
    /*  Called by ilScale().
        Adds an element to "pipe" to scale the pipe image to "dstWidth" by "dstHeight"
        which both must be > 0.  The input pipe image must be a bitonal,
        bit-per-pixel image, with _any_ "blackIsZero".  The resulting image is of the 
        same type. "pInfo" points to the pipe info, "dstWidth/Height" is the (> 0) size 
        to scale to.
            Only a scale down in both directions is allowed; the following must be true:
        srcWidth must be >= dstWidth and srcHeight must be >= dstHeight!
            Logically the same as ilScaleBitonalToGray(): a gray byte is produced for
        each destination pixel, using 256 levels of gray.  If that byte is 
        >= "*pGrayThreshold"; a white pixel is written; else a black pixel is written.
            NOTE: the value at "*pGrayThreshold" is read each time the pipe is executed
        (during Init()); the caller change its value and get different results without
        recreating the pipe.  If "pGrayThreshold" is NULL, a default value is used.
    */
IL_PRIVATE void _ilAreaScaleBitonal (
    ilPipe              pipe,
    unsigned long       dstWidth,
    unsigned long       dstHeight,
    int                *pGrayThreshold,
    ilPipeInfo         *pInfo
    )
{
register ilBiGrayPrivPtr pPriv;
ilDstElementData        dstData;
#define                 DEFAULT_GRAY_THRESHOLD  128     /* assumes 256 levels */

        /*  Add a filter to do the scale, then init *pPriv. 
        */
    dstData.producerObject = (ilObject)NULL;
    dstData.pDes = (ilImageDes *)NULL;
    dstData.pFormat = IL_FORMAT_BIT;
    dstData.width = dstWidth;
    dstData.height = dstHeight;
    dstData.stripHeight = ((dstHeight * pInfo->recommendedStripHeight) / pInfo->height) + 1;
    dstData.constantStrip = FALSE;
    dstData.pPalette = (unsigned short *)NULL;
    pPriv = (ilBiGrayPrivPtr)ilAddPipeElement (pipe, IL_FILTER, sizeof (ilBiGrayPrivRec), 
                         IL_ADD_PIPE_HOLD_SRC, (ilSrcElementData *)NULL, &dstData, 
                         ilBiGrayInit, IL_NPF, IL_NPF, ilBiGrayExecute, NULL, 0);
    if (!pPriv)
        return;

        /*  Pipe element added, init pPriv.  If not a tempImage, reading directly from
            a permanent image: set srcHeight to image height; else set to 0.
        */
    pPriv->bitonalOutput = TRUE;
    if (pGrayThreshold)
        pPriv->pGrayThreshold = pGrayThreshold;
    else {
        pPriv->defaultGrayThreshold = DEFAULT_GRAY_THRESHOLD;
        pPriv->pGrayThreshold = &pPriv->defaultGrayThreshold;
        }
    pPriv->srcWidth = pInfo->width;
    pPriv->srcHeight = pInfo->height;
    pPriv->dstWidth = dstWidth;
    pPriv->dstHeight = dstHeight;
    pPriv->widthDiff = pInfo->width - dstWidth;
    pPriv->heightDiff = pInfo->height - dstHeight;
    if (!pInfo->tempImage)
         pPriv->srcBufferHeight = pInfo->height;
    else pPriv->srcBufferHeight = 0;

        /*  Init the filter table in private based on scale factors and 256 gray levels.
            Note that "blackIsZero" is passed as FALSE below, because we want to output
            the same blackIsZero as input.  We output a zero when the gray is 
            >= threshold; this works for either sense of blackIsZero.
        */
    ilInit9BitFilter (pPriv->srcWidth, pPriv->srcHeight, pPriv->dstWidth, 
                      pPriv->dstHeight, 256, FALSE, pPriv->filterValues);

    pipe->context->error = IL_OK;
}


