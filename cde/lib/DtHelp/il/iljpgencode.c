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
/* $XConsortium: iljpgencode.c /main/3 1995/10/23 15:55:46 rswiston $ */
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

#include "iljpgencodeint.h"


    /*  -------------------- iljpgEncodeInit -------------------------- */
    /*  Init for JPEG encoding and return ptr to private block.
    */
    ILJPG_PUBLIC
iljpgError iljpgEncodeInit (
    iljpgDataPtr pData,
    iljpgPtr           *pPrivate                /* RETURNED */
    )
{
    iljpgEncodePrivPtr pPriv;
    iljpgECompPtr pComp;
    iljpgCompDataPtr    pCompData;
    int                 comp;
    iljpgError          error;
    unsigned int   index, i;

        /*  Validate *pData: valid hori/vertFactor, tables present, etc. */
    if (!_iljpgValidPars (pData))
        return ILJPG_ERROR_ENCODE_PARS;

        /*  Allocate and return private block */
    pPriv = (iljpgEncodePrivPtr)ILJPG_MALLOC_ZERO (sizeof (iljpgEncodePrivRec));
    if (!pPriv)
        return ILJPG_ERROR_ENCODE_MALLOC;
    pPriv->pData = pData;
    *pPrivate = (iljpgPtr)pPriv;

        /*  For DCT encoding, get and store into private a
            scaled version of each Q table.
        */
    for (i = 0; i < 4; i++)
        pPriv->DCTScaleTables[i] = (float *)NULL;
    for (i = 0; i < 4; i++) {
        if (pData->QTables[i]) {
            if (!(pPriv->DCTScaleTables[i] = (float *)ILJPG_MALLOC(sizeof(float) * 64)))
                return ILJPG_ERROR_ENCODE_MALLOC;
            if (error = _iljpgEnDCTScale (pData->QTables[i], pPriv->DCTScaleTables[i]))
                return error;
            }
        }

        /*  Init Huffman encoding */
    if (error = _iljpgEnhuffInit (pPriv))
        return error;

        /*  Setup static part of per-comp data; copy from other places */
    for (comp = 0, pCompData = pData->comp, pComp = pPriv->compData; 
      comp < pData->nComps; comp++, pCompData++, pComp++) {
        pComp->pScale = pPriv->DCTScaleTables[pCompData->QTableIndex];
        pComp->horiFactor = pCompData->horiFactor;
        pComp->vertFactor = pCompData->vertFactor;
        pComp->width = pData->width * pCompData->horiFactor / pData->maxHoriFactor;
        pComp->mcuXInc = 8 * pCompData->horiFactor;
        pComp->mcuYInc = 8 * pCompData->vertFactor;
        }

    return 0;
}


    /*  -------------------- iljpgEncodeCleanup -------------------------- */
    /*  Cleanup after JPEG encoding.
    */
    ILJPG_PUBLIC
iljpgError iljpgEncodeCleanup (
    iljpgPtr            pPrivate
    )
{
    int        i;
    iljpgEncodePrivPtr pPriv;
    iljpgError          error;

        /*  Free any scaled Q tables created by Init() */
    pPriv = (iljpgEncodePrivPtr)pPrivate;
    for (i = 0; i < 4; i++) 
        if (pPriv->DCTScaleTables[i])
            ILJPG_FREE (pPriv->DCTScaleTables[i]);

        /*  Cleanup Huffman */
    error = _iljpgEnhuffCleanup(pPriv);

        /*  Free the given private data.  Note that pPrivate->pData is not
            freed; that is done when the caller calls iljpgFreeData().
        */
    ILJPG_FREE (pPrivate);

    return error;
}



    /*  -------------------- iljpgEncodeExecute -------------------------- */
    /*  Encode (compress) the pixels from the per-plane buffers pointed to 
        by "pSrcPixels": one ptr per component (# of components = "nComps" in 
        the iljpgData passed to iljpgEncodeInit()).  "nSrcBytesPerRow" is length
        of each row in the corresponding buffers in "pSrcPixels".  
        "pPrivate" is the ptr returned by iljpgEncodeInit().
        "nSrcLines" is the # of lines to read; encoding stops when nSrcLines
        lines have been encoded.
    */
    ILJPG_PUBLIC
iljpgError iljpgEncodeExecute (
    iljpgPtr            pPrivate,
    ILJPG_ENCODE_STREAM stream,
    long                nSrcLines,
    iljpgPtr            pSrcPixels[],
    long                nSrcBytesPerRow[]
    )
{
    iljpgEncodePrivPtr  pPriv;
    iljpgDataPtr        pData;
    iljpgECompPtr pComp;
    iljpgPtr            pPixels;
    iljpgError          error;
    long                nBytesPerRow, mcuMaxX, mcuMaxY, bX, bY;
    int                 comp, v, h, mcuWidth, mcuHeight;
    int                 pixels[64];

    pPriv = (iljpgEncodePrivPtr)pPrivate;
    pData = pPriv->pData;

        /*  Encode "interleaved" JPEG data, where all components are grouped together
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

        /*  Reset temp vars in comp data in private; "height" is size of one strip */
    for (comp = 0, pComp = pPriv->compData; comp < pData->nComps; comp++, pComp++) {
        pComp->height = nSrcLines * pComp->vertFactor / pData->maxVertFactor;
        pComp->x = 0;
        pComp->y = 0;
        pComp->lastDC = 0;          /* new strip; clear as if a reset/restart */
        }

        /*  Loop over y, and over x within y, and encode one MCU (all components),
            advancing x by mcuWidth and y by mcuHeight.
        */
    for (mcuMaxY = 0; mcuMaxY < nSrcLines; mcuMaxY += mcuHeight) {
        for (mcuMaxX = 0; mcuMaxX < pData->width; mcuMaxX += mcuWidth) {

            /*  Encode one MCU, all components, to (mcuX, mcuY). For each component 
                there are horiFactor * vertFactor 8x8 blocks that go across then down.
            */
            for (comp = 0, pComp = pPriv->compData; comp < pData->nComps; comp++, pComp++) {
                nBytesPerRow = nSrcBytesPerRow[comp];
                pPixels = pSrcPixels[comp];

                for (v = 0, bY = pComp->y; v < pComp->vertFactor; v++, bY += 8) {
                    for (h = 0, bX = pComp->x; h < pComp->horiFactor; h++, bX += 8) {

                            /*  Extract one 8x8 block from position (bX, bY).  If
                                clipped replicate pixel or above scan line out to 8x8.
                            */
                        {   int             nBytesInit;
                            int    nLines, nBytesM1, *pDst, *pDstLine;
                            iljpgPtr pSrc, pSrcLine;

                            nLines = pComp->height - bY;
                            if (nLines > 8) 
                                nLines = 8;
                            nBytesInit = pComp->width - bX;
                            if (nBytesInit > 8) 
                                nBytesInit = 8;
                            if ((nLines > 0) && (nBytesInit > 0)) {
                                pSrcLine = pPixels + (bY * nBytesPerRow) + bX;
                                pDstLine = pixels;

                                    /*  If clipped; do nLines, replicate at right edge */
                                if ((nLines < 8) || (nBytesInit < 8)) {
                                    int     i, j, pixel;
                                    for (i = 0; i < nLines; i++) {
                                        pSrc = pSrcLine;
                                        pSrcLine += nBytesPerRow;
                                        pDst = pDstLine;
                                        pDstLine += 8;
                                        for (j = 0; j < nBytesInit; j++)
                                            *pDst++ = pixel = *pSrc++;
                                        for (j = nBytesInit; j < 8; j++)
                                            *pDst++ = pixel;    /* replicate right */
                                        }
                                        /*  Replicate last dst line out to 8 lines */
                                    pDst = pDstLine;
                                    pDstLine -= 8;              /* back up one line */
                                    for (i = nLines; i < 8; i++) {
                                        for (j = 0; j < 8; j++)
                                            *pDst++ = pDstLine[j];
                                        }
                                    }   /* END clipped */
                                else {  /* all 8x8 available; do simple case */
                                    nLines--;               /* make # lines/bytes - 1 */
                                    nBytesInit--;
                                    do {
                                        nBytesM1 = nBytesInit;
                                        pSrc = pSrcLine;
                                        pSrcLine += nBytesPerRow;
                                        pDst = pDstLine;
                                        pDstLine += 8;
                                        do {
                                            *pDst++ = *pSrc++;
                                            } while (--nBytesM1 >= 0);
                                        } while (--nLines >= 0);
                                    }
                                }
                        }

                            /*  Do DCT encoding, from "pixels" back into "pixels" */
                        _iljpgEnDCT (pixels, pComp->pScale);

                            /*  Subtract previous DC from this one, save this one for next */
                        {   int     dc;
                            dc = pixels[0];
                            pixels[0] = dc - pComp->lastDC;
                            pComp->lastDC = dc;
                        }

                            /*  Do Huffman encoding of block */
                        if (error = _iljpgEnhuffExecute (pPriv, comp, pixels, stream))
                            return error;
                        }   /* END hori, one 8x8 block */
                    }       /* END vert */

                pComp->x += pComp->mcuXInc;     /* move component one MCU to right */
                }       /* END one component */
            }           /* END one hori MCU */

          /*  Move each component one MCU down, reset to left edge */
        for (comp = 0, pComp = pPriv->compData; comp < pData->nComps; comp++, pComp++) {
            pComp->y += pComp->mcuYInc;
            pComp->x = 0;
            }
        }             /* END one vert MCU */

        /*  Flush any bits not yet output from Huffman encoding and exit */
    return _iljpgEnhuffFlush (pPriv, stream);
}

