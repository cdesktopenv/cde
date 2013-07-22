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
/* $XConsortium: ildecompjpeg.c /main/5 1996/06/19 12:23:41 ageorge $ */
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
#include "ilpipelem.h"
#include "ilerrors.h"
#include "ildecomp.h"
#include "iljpgdecode.h"
#include "ilutiljpeg.h"

    /*  Private data when decompressing JPEG data */
typedef struct {
    iljpgDataRec        jpgData;        /* data for iljpg?() functions */
    ilBool              firstStrip;     /* set true by Init(): this is first strip */
    long                nLinesWritten;  /* inited to 0 by Init() */
    long                stripHeight;    /* height of strips being written */
    iljpgPtr            pJPEGPriv;      /* iljpg private data */
    iljpgDataPtr        pJPEGData;      /* JIF only: ptr to JPEG data */
    ilJPEGDecodeStream  streamRec;      /* JIF only: src decoding stream */
    } ilJPEGPrivRec, *ilJPEGPrivPtr;


    /*  -------------------- ilDecompInit -------------------------- */
    /*  Init() function for decompressing JPEG.
    */
static ilError ilDecompInit (
    ilJPEGPrivPtr       pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
    pPriv->firstStrip = TRUE;
    pPriv->nLinesWritten = 0;
    pPriv->pJPEGData = (iljpgDataPtr)NULL;
    pPriv->pJPEGPriv = (iljpgPtr)NULL;
    return IL_OK;
}


    /*  -------------------- ilDecompCleanup -------------------------- */
    /*  Cleanup() function for decompressing JPEG.
    */
static ilError ilDecompCleanup (
    ilJPEGPrivPtr       pPriv,
    ilBool              aborting
    )
{
    ilError         error;

        /*  Cleanup from JPEG decode, but only if firstStrip setup done */
    if (!pPriv->firstStrip) {
        if (pPriv->pJPEGPriv) 
            if ((error = iljpgDecodeCleanup (pPriv->pJPEGPriv)))
                return error;
        if (pPriv->pJPEGData)
            return iljpgFreeData (pPriv->pJPEGData);
        }
    return IL_OK;
}


    /*  -------------------- ilDecompRawExecute -------------------------- */
    /*  Execute() function for decompressing "raw" (non-JIF) JPEG: handle strips.
    */
static ilError ilDecompRawExecute (
    ilExecuteData      *pData,
    unsigned long       dstLine,
    unsigned long      *pNLines
    )
{
    register ilJPEGPrivPtr pPriv;
    ilJPEGDecodeStream  streamRec;
    ilError             error;
    iljpgPtr            pPixels[ILJPG_MAX_COMPS];
    long                nBytesPerRow[ILJPG_MAX_COMPS];
    int                 i;
    ilImagePlaneInfo   *pPlane;
    register ilImageInfo *pSrcImage, *pDstImage;

    pPriv = (ilJPEGPrivPtr)pData->pPrivate;
    pSrcImage = pData->pSrcImage;
    pDstImage = pData->pDstImage;

        /*  If first strip, setup iljpg data and call iljpg Init() function */
    if (pPriv->firstStrip) {
        _ilJPEGTablesIn ((ilJPEGData *)pSrcImage->pCompData, &pPriv->jpgData);
        if ((error = iljpgDecodeInit (&pPriv->jpgData, &pPriv->pJPEGPriv)))
            return error;
        pPriv->firstStrip = FALSE;
        }

        /*  Decode one strip into dst pixels */
    streamRec.pData = pSrcImage->plane[0].pPixels + pData->compressed.srcOffset;
    streamRec.nBytesLeft = pData->compressed.nBytesToRead;

    for (i = 0, pPlane = pData->pDstImage->plane; 
      i < pData->pDstImage->pDes->nSamplesPerPixel; i++, pPlane++) {
        pPixels[i] = pPlane->pPixels + 
            (dstLine * pPriv->jpgData.comp[i].vertFactor / pPriv->jpgData.maxVertFactor) *
                                            pPlane->nBytesPerRow;
        nBytesPerRow[i] = pPlane->nBytesPerRow;
        }

    return iljpgDecodeExecute (pPriv->pJPEGPriv, &streamRec, TRUE, *pNLines, 
                               pPixels, nBytesPerRow);
}


    /*  -------------------- ilDecompJIFExecute -------------------------- */
    /*  Execute() function for decompressing JIF: a single strip.
    */
static ilError ilDecompJIFExecute (
    ilExecuteData      *pData,
    unsigned long       dstLine,
    unsigned long      *pNLines
    )
{
    register ilJPEGPrivPtr pPriv;
    ilError             error;
    int                 i;
    iljpgPtr            pPixels[ILJPG_MAX_COMPS];
    long                nBytesPerRow[ILJPG_MAX_COMPS];
    ilImagePlaneInfo   *pPlane;
    ilBool              firstStrip;

    pPriv = (ilJPEGPrivPtr)pData->pPrivate;

        /*  If firstStrip decode header, determine # of strips and init for decode */
    firstStrip = pPriv->firstStrip;
    pPriv->firstStrip = FALSE;
    if (firstStrip) {
        pPriv->streamRec.pData = pData->pSrcImage->plane[0].pPixels;
        pPriv->streamRec.nBytesLeft = pData->compressed.nBytesToRead;
        if ((error = iljpgDecodeJIF (&pPriv->streamRec, &pPriv->pJPEGData)))
            return error;
        if ((error = iljpgDecodeInit (pPriv->pJPEGData, &pPriv->pJPEGPriv)))
            return error;
        }

        /*  Decode one strip into dst pixels, or remaining lines if last strip */
    for (i = 0, pPlane = pData->pDstImage->plane; 
      i < pData->pDstImage->pDes->nSamplesPerPixel; i++, pPlane++) {
        pPixels[i] = pPlane->pPixels + 
            (dstLine * pPriv->jpgData.comp[i].vertFactor / pPriv->jpgData.maxVertFactor) *
                                            pPlane->nBytesPerRow;
        nBytesPerRow[i] = pPlane->nBytesPerRow;
        }
    if ((pPriv->nLinesWritten + pPriv->stripHeight) > pPriv->jpgData.height)
         *pNLines = pPriv->jpgData.height - pPriv->nLinesWritten;
    else *pNLines = pPriv->stripHeight;
    if ((error = iljpgDecodeExecute (pPriv->pJPEGPriv, &pPriv->streamRec, FALSE, *pNLines,
                                    pPixels, nBytesPerRow)))
        return error;

        /*  Handle based on whether firstStrip and/or last strip:
                first?  last?
                YES     YES     normal return; image is one strip, no call backs needed.
                YES     NO      return "call back" error code: want this filter to
                                be called again until another return cancels it
                NO      YES     cancel call back; don't want to be called again
                NO      NO      normal return; more strips to decompress
        */
    pPriv->nLinesWritten += pPriv->stripHeight;
    if (firstStrip)
        if (pPriv->nLinesWritten >= pPriv->jpgData.height)      /* last strip */
             return IL_OK;
        else return IL_ERROR_ELEMENT_AGAIN;
    else
        if (pPriv->nLinesWritten >= pPriv->jpgData.height)
             return IL_ERROR_ELEMENT_COMPLETE;
        else return IL_OK;
}


    /*  -------------------- _ilDecompJPEG -------------------------- */
    /*  Called by ilDecompress() when pipe image is JPEG compressed.  Add a filter to
        decompress the pipe image.
    */
ilBool _ilDecompJPEG (
    ilPipe              pipe,
    ilPipeInfo         *pInfo,                              
    ilImageDes         *pDes
    )
{
    ilDstElementData    dstData;
    ilImageDes          des;
    ilImageFormat       format;
    ilJPEGPrivPtr       pPriv;
    iljpgDataRec        jpgData;

        /*  Define decompressed output from this filter: planar format unless
            a single component image (e.g. gray).
        */
    dstData.producerObject = (ilObject)NULL;
    des = *pDes;
    des.compression = IL_UNCOMPRESSED;
    dstData.pDes = &des;
    if (pDes->nSamplesPerPixel == 1)
        dstData.pFormat = IL_FORMAT_BYTE;
    else {
        IL_INIT_IMAGE_FORMAT (&format);
        format.sampleOrder = IL_SAMPLE_PLANES;
        format.byteOrder = IL_MSB_FIRST;
        format.rowBitAlign = 8;
        format.nBitsPerSample[0] =
            format.nBitsPerSample[1] =
            format.nBitsPerSample[2] = 8;
        dstData.pFormat = &format;
        }
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.pPalette = (unsigned short *)NULL;
    dstData.pCompData = (ilPtr)NULL;

        /*  Init iljpg package data */
    _ilJPEGDataIn (&des, pInfo->width, pInfo->height, &jpgData);

        /*  If "raw" JPEG, decode in strips, the size of those presented to this filter */
    if (pDes->compInfo.JPEG.reserved & IL_JPEGM_RAW) {
        dstData.stripHeight = pInfo->stripHeight;   /* decode strips as they come in */
        dstData.constantStrip = pInfo->constantStrip;
        pPriv = (ilJPEGPrivPtr)ilAddPipeElement(pipe, IL_FILTER, 
                sizeof(ilJPEGPrivRec), 0, (ilSrcElementData *)NULL, &dstData, 
                ilDecompInit, ilDecompCleanup, IL_NPF, ilDecompRawExecute, NULL, 0);
        }
    else {
            /*  JIF: a single data stream (strip) with tables in the stream: decompress
                in IL's default strip size for this image, made a multiple of MCUs.
            */
        long        stripHeight, mcuHeight;
        stripHeight = ilRecommendedStripHeight (dstData.pDes, dstData.pFormat,
                                                dstData.width, dstData.height);
        mcuHeight = 8 * jpgData.maxVertFactor;
        if (mcuHeight == 0)
            mcuHeight = 32;                         /* ? error, default to max possible */
        stripHeight = ((stripHeight + mcuHeight - 1) / mcuHeight) * mcuHeight;
        if (stripHeight > dstData.height)
            stripHeight = dstData.height;
        dstData.stripHeight = stripHeight;
        dstData.constantStrip = TRUE;
        pPriv = (ilJPEGPrivPtr)ilAddPipeElement(pipe, IL_FILTER, 
                sizeof(ilJPEGPrivRec), 0, (ilSrcElementData *)NULL, &dstData, 
                ilDecompInit, ilDecompCleanup, IL_NPF, ilDecompJIFExecute, NULL, 0);
        }

        /*  Fill in data into private; copy jpgData */
    if (!pPriv) 
        return FALSE;
    pPriv->jpgData = jpgData;
    pPriv->stripHeight = dstData.stripHeight;

    return TRUE;
}



