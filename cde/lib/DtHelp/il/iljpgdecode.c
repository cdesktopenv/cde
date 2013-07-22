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
/* $XConsortium: iljpgdecode.c /main/3 1995/10/23 15:54:14 rswiston $ */
/**---------------------------------------------------------------------
***	
***    (c)Copyright 1992 Hewlett-Packard Co.
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

#include "iljpgdecodeint.h"


    /*  -------------------- iljpgDecodeInit -------------------------- */
    /*  Init for JPEG decoding and return ptr to private block.
    */
    ILJPG_PUBLIC_EXTERN
iljpgError iljpgDecodeInit (
    register iljpgDataPtr pData,
    iljpgPtr           *pPrivate                /* RETURNED */
    )
{
    register iljpgDecodePrivPtr pPriv;
    register iljpgCompPtr pComp;
    int                 comp;
    iljpgCompDataPtr    pCompData;
    iljpgError          error;

        /*  Validate *pData: valid hori/vertFactor, tables present, etc. */
    if (!_iljpgValidPars (pData))
        return ILJPG_ERROR_DECODE_PARS;

        /*  Allocate and return private block */
    pPriv = (iljpgDecodePrivPtr)ILJPG_MALLOC_ZERO (sizeof (iljpgDecodePrivRec));
    if (!pPriv)
        return ILJPG_ERROR_DECODE_MALLOC;
    pPriv->pData = pData;
    *pPrivate = (iljpgPtr)pPriv;

    pPriv->mcuRestartCount = 0;

        /*  Init Huffman and DCT decoders */
    if ((error = _iljpgDehuffInit (pPriv))
     || (error = _iljpgDeDCTInit (pPriv)))  {
        ILJPG_FREE (pPriv);
        return error;
        }

        /*  Setup static part of per-comp data; copy from other places */
    for (comp = 0, pCompData = pData->comp, pComp = pPriv->compData; 
      comp < pData->nComps; comp++, pCompData++, pComp++) {
        pComp->pRevScale = pPriv->DCTRevScaleTables[pCompData->QTableIndex];
        pComp->horiFactor = pCompData->horiFactor;
        pComp->vertFactor = pCompData->vertFactor;
        pComp->width = pData->width * pCompData->horiFactor / pData->maxHoriFactor;
        pComp->mcuXInc = 8 * pCompData->horiFactor;
        pComp->mcuYInc = 8 * pCompData->vertFactor;
        }

    return 0;
}


    /*  -------------------- iljpgDecodeCleanup -------------------------- */
    /*  Cleanup after JPEG decoding.
    */
    ILJPG_PUBLIC_EXTERN
iljpgError iljpgDecodeCleanup (
    iljpgPtr            pPrivate
    )
{
    iljpgError          error1, error2;

        /*  Call the Huffman and DCT cleanup functions.
        */
    error1 = _iljpgDehuffCleanup ((iljpgDecodePrivPtr)pPrivate);
    error2 = _iljpgDeDCTCleanup ((iljpgDecodePrivPtr)pPrivate);

        /*  Free the given private data.  Note that pPrivate->pData is not
            freed; that is done when the caller calls iljpgFreeData().
        */
    ILJPG_FREE (pPrivate);

        /*  Return first error code or success if neither an error */
    if (error1)
        return error1;
    if (error2)
        return error2;
    return 0;
}



    /*  -------------------- iljpgDecodeExecute -------------------------- */
    /*  Decode (decompress) the JPEG data read using "stream", into the per-plane
        buffers pointed to by "pPixels": one ptr per component (# of components = 
        "nComps" in the iljpgData passed to iljpgDecodeInit()).
        "nBytesPerRow" is length of each row in the corresponding buffers
        in "pPixels".  "pPrivate" is the ptr returned by iljpgDecodeInit().
            "nDstLines" is the # of lines to write; decoding stops when nDstLines
        scan lines have been decoded.  "nDstLines" should be a multiple of the MCU
        height, except at the last strip where it should be the # of lines remaining
        to be decoded; this function will clip.
            "doReset" is true if an implicit reset should be done at the beginning
        of this function, i.e. if an implicit restart is assumed before each strip.
    */
    ILJPG_PUBLIC_EXTERN
iljpgError iljpgDecodeExecute (
    iljpgPtr            pPrivate,
    ILJPG_DECODE_STREAM stream,
    int                 doReset,
    long                nDstLines,
    iljpgPtr            pDstPixels[],
    long                nDstBytesPerRow[]
    )
{
    iljpgDecodePrivPtr  pPriv;
    iljpgDataPtr        pData;
    int                 comp, v, h, mcuWidth, mcuHeight;
    unsigned int        blockType;
    iljpgPtr            pPixels;
    iljpgError          error;
    long                nBytesPerRow, mcuMaxX, mcuMaxY, bX, bY;
    int                 outHuff[64];
    iljpgByte           outDCT[64];
    int                 restartInterval;
    register iljpgCompPtr pComp;
    int                 nBytesInit;
    register int        nLines, dc;
    register iljpgPtr   pDstLine;


    pPriv = (iljpgDecodePrivPtr)pPrivate;
    pData = pPriv->pData;

        /*  Decode "interleaved" JPEG data, where all components are grouped together
            in Minimum Coded Unit (MCU) size, = 8 * max hori(width) or vert(height)
            factor.  The "factors" are the inverse of IL subsample factors.  For
            example, if component 0 is not subsampled and 1 and 2 are subsampled by 2,
            then hori/vertFactor for comp 0 is 2, for comps 1..2 is 1, and max hori/vert
            factor is 2 (there are 4x as many comp 0 pixels as comps 1 or 2).
                So: loop over y, and over x within y, and decode one MCU (all components),
            advancing x by mcuWidth and y by mcuHeight.
        */
    mcuWidth = 8 * pData->maxHoriFactor;
    mcuHeight = 8 * pData->maxVertFactor;
    restartInterval = pData->restartInterval;

        /*  Reset temp vars in comp data in private; "height" is size of one strip */
    for (comp = 0, pComp = pPriv->compData; comp < pData->nComps; comp++, pComp++) {
        pComp->height = nDstLines * pComp->vertFactor / pData->maxVertFactor;
        pComp->x = 0;
        pComp->y = 0;
        }

        /*  Reset Huffman decoding: beginning of a strip/restart interval */
    if (doReset) {
        if ((error = _iljpgDehuffReset (pPriv)))
            return error;
        }

        /*  Loop over y, and over x within y, and decode one MCU (all components),
            advancing x by mcuWidth and y by mcuHeight.
        */
    for (mcuMaxY = 0; mcuMaxY < nDstLines; mcuMaxY += mcuHeight) {
        for (mcuMaxX = 0; mcuMaxX < pData->width; mcuMaxX += mcuWidth) {

            /*  Decode one MCU, all components, to (mcuX, mcuY). For each component 
                there are horiFactor * vertFactor 8x8 blocks that go across then down.
                If a restart interval and mcu count >, do a reset/restart.
            */
            if (restartInterval && (pPriv->mcuRestartCount >= restartInterval)) {
                if ((error = _iljpgDehuffReset (pPriv)))
                    return error;
                pPriv->mcuRestartCount = 0;
                }

            for (comp = 0, pComp = pPriv->compData; comp < pData->nComps; comp++, pComp++) {
                nBytesPerRow = nDstBytesPerRow[comp];
                pPixels = pDstPixels[comp];

                for (v = 0, bY = pComp->y; v < pComp->vertFactor; v++, bY += 8) {
                    for (h = 0, bX = pComp->x; h < pComp->horiFactor; h++, bX += 8) {
                        if ((error = _iljpgDehuffExecute (pPriv, stream, comp, outHuff,
                                                         &blockType)))
                            return error;

                            /*  Add previous DC to this one, save away for next */
                        dc = outHuff[0] + pComp->lastDC;
                        pComp->lastDC = dc;

                            /*  If an 8x8 block fits in output buffer, decode the DCT
                                (based on return from Huffman decode) directly into
                                the output buffer; otherwise decode into "outDCT" and
                                copy top-left part of it into buffer below.
                            */
                        pDstLine = pPixels + (bY * nBytesPerRow) + bX;
                        nLines = pComp->height - bY;
                        nBytesInit = pComp->width - bX;
                        if ((nLines >= 8) && (nBytesInit >= 8)) {
                            switch (blockType) {
                              case HUFF_DC_ONLY:
                                dc = (int)(dc * *pComp->pRevScale + 128.0);
                                if (dc < 0) dc = 0; else if (dc > 255) dc = 255;
                                nLines = 7;
                                do {
                                    pDstLine[0] = dc; 
                                    pDstLine[1] = dc; 
                                    pDstLine[2] = dc; 
                                    pDstLine[3] = dc; 
                                    pDstLine[4] = dc; 
                                    pDstLine[5] = dc; 
                                    pDstLine[6] = dc; 
                                    pDstLine[7] = dc; 
                                    pDstLine += nBytesPerRow;
                                    } while (--nLines >= 0);
                               break;
                              case HUFF_FOURX4:
                               outHuff[0] = dc;
                               _iljpgDeDCT4x4 (outHuff, nBytesPerRow, pDstLine, 
                                               pComp->pRevScale);
                               break;
                              case HUFF_FULL:
                               outHuff[0] = dc;
                               _iljpgDeDCTFull (outHuff, nBytesPerRow, pDstLine, 
                                                pComp->pRevScale);
                               break;
                               }
                            }   /* END whole 8x8 block */
                        else {  /* no space for 8x8; clip */
                            outHuff[0] = dc;
                            switch (blockType) {
                              case HUFF_DC_ONLY:
                               _iljpgDeDCTDCOnly (outHuff, 8, outDCT, pComp->pRevScale);
                               break;
                              case HUFF_FOURX4:
                               _iljpgDeDCT4x4 (outHuff, 8, outDCT, pComp->pRevScale);
                               break;
                              case HUFF_FULL:
                               _iljpgDeDCTFull (outHuff, 8, outDCT, pComp->pRevScale);
                               break;
                               }

                                /*  Clip and output 8x8 block to position (bX, bY) */
                            {   register int    nBytesM1;
                                register iljpgPtr pSrc, pSrcLine, pDst;

                                if (nBytesInit > 8) 
                                    nBytesInit = 8;
                                if (nLines > 8) 
                                    nLines = 8;
                                if ((nLines > 0) && (nBytesInit > 0)) {
                                    nLines--;               /* make # lines/bytes - 1 */
                                    nBytesInit--;
                                    pSrcLine = outDCT;
                                    do {
                                        nBytesM1 = nBytesInit;
                                        pSrc = pSrcLine;
                                        pSrcLine += 8;
                                        pDst = pDstLine;
                                        pDstLine += nBytesPerRow;
                                        do {
                                            *pDst++ = *pSrc++;
                                            } while (--nBytesM1 >= 0);
                                        } while (--nLines >= 0);
                                    }
                            }
                            }   /* END clip */
                        }       /* END hori, one 8x8 block */
                    }       /* END vert */

                pComp->x += pComp->mcuXInc;     /* move component one MCU to right */
                }       /* END one component */

            pPriv->mcuRestartCount++;           /* inc count of mcus since restart */
            }           /* END one hori MCU */

          /*  Move each component one MCU down, reset to left edge */
        for (comp = 0, pComp = pPriv->compData; comp < pData->nComps; comp++, pComp++) {
            pComp->y += pComp->mcuYInc;
            pComp->x = 0;
            }
        }             /* END one vert MCU */

    return 0;
}

