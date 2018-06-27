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
/* $XConsortium: ilcompressjpeg.c /main/4 1996/01/08 12:14:02 lehors $ */
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
#include "ilcompress.h"
#include "ilcodec.h"
#include "ilerrors.h"
#include "ilutiljpeg.h"
#include "iljpgencode.h"


    /*  ============================= JIF to Raw =================================== */
    /*  This body of code supports converting JIF format JPEG to "raw" (stripped).
    */

    /*  Private data for ilJIFToRawExecute() */
typedef struct {
    ilJPEGData             *pCompData;          /* ptr to IL JPEG data */
    } ilJIFToRawPrivRec, *ilJIFToRawPrivPtr;


    /*  ----------------------- ilJIFToRawDestroy ------------------------------ */
    /*  Destroy() function for converting JIF format JPEG to "raw" JPEG.
    */
static ilError ilJIFToRawDestroy (
    ilJIFToRawPrivPtr       pPriv
    )
{
        /*  Free pCompData including associated tables */
    if (pPriv->pCompData) {
        _ilJPEGFreeTables (pPriv->pCompData);
        IL_FREE (pPriv->pCompData);
        }
    return IL_OK;
}

    /*  ----------------------- ilJIFToRawExecute ------------------------------ */
    /*  Execute() function for converting JIF format JPEG to "raw" JPEG.
    */
static ilError ilJIFToRawExecute (
    ilExecuteData          *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
    ilJPEGDecodeStream      streamRec;
    iljpgDataPtr            pJPEGData;
    ilError                 error;
    int                     i;
    ilJPEGData             *pCompData;
    ilImagePlaneInfo       *pDstPlane;
    long                    dstOffset, nBytes;

        /*  Parse the src data, which is a JIF data stream */
    streamRec.pData = pData->pSrcImage->plane[0].pPixels;
    streamRec.nBytesLeft = pData->compressed.nBytesToRead;
    if (error = iljpgDecodeJIF (&streamRec, &pJPEGData))
        return error;

        /*  Point pCompData to tables decoded from JIF, and copy table indices. */
    pCompData = ((ilJIFToRawPrivPtr)pData->pPrivate)->pCompData;
    for (i = 0; i < 4; i++) {
        pCompData->QTables[i] = pJPEGData->QTables[i];
        pCompData->DCTables[i] = pJPEGData->DCTables[i];
        pCompData->ACTables[i] = pJPEGData->ACTables[i];
        }
    for (i = 0; i < ILJPG_MAX_COMPS; i++) {
        pCompData->sample[i].QTableIndex = pJPEGData->comp[i].QTableIndex;
        pCompData->sample[i].DCTableIndex = pJPEGData->comp[i].DCTableIndex;
        pCompData->sample[i].ACTableIndex = pJPEGData->comp[i].ACTableIndex;
        }

        /*  Copy restartInterval from JIF info; it will be zero (0) if no DRI marker,
            implying no restart markers.
        */
    pCompData->restartInterval = pJPEGData->restartInterval;
    
        /*  Free decode data but not tables; they will be freed by ilJIFToRawDestroy() */
    IL_FREE (pJPEGData);

        /*  Write rest of data (actual JPEG data, after tables etc.) to next filter.
            Exclude the EOI marker if present (which it really should be).
        */
    nBytes = streamRec.nBytesLeft;
    if ((nBytes >= 2) 
     && (streamRec.pData[nBytes-2] == ILJPGM_FIRST_BYTE)
     && (streamRec.pData[nBytes-1] == ILJPGM_EOI))
        nBytes -= 2;                                        /* remove EOI marker */

    dstOffset = *pData->compressed.pDstOffset;
    pDstPlane = pData->pDstImage->plane;
    if (pDstPlane->bufferSize < (nBytes + dstOffset))
        if (!_ilReallocCompressedBuffer (pData->pDstImage, 0, (nBytes + dstOffset)))
            return IL_ERROR_MALLOC;
    bcopy ((char *)streamRec.pData,
	   (char *)(pDstPlane->pPixels + dstOffset), nBytes);
    *pData->compressed.pNBytesWritten = nBytes;

    return IL_OK;
}


    /*  ----------------------- ilJIFToRaw ------------------------------ */
    /*  Called by _ilCompressJPEG() when the pipe image is JIF format, to
        convert to "raw" JPEG format.
    */
static ilBool ilJIFToRaw (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat
    )
{
    ilJPEGData             *pCompData;
    ilJIFToRawPrivPtr       pPriv;
    ilDstElementData        dstData;

        /*  Allocate an IL raw JPEG data block; point to it for pipe element */
    if (!(pCompData = (ilJPEGData *)IL_MALLOC_ZERO (sizeof (ilJPEGData))))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_MALLOC);

    dstData.producerObject = (ilObject)NULL;
    pDes->compInfo.JPEG.reserved |= IL_JPEGM_RAW;   /* now writing raw JPEG */
    dstData.pDes = pDes;
    dstData.pFormat = pFormat;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = pInfo->height;            /* write one strip = whole image */
    dstData.constantStrip = TRUE;
    dstData.pPalette = (unsigned short *)NULL;
    dstData.pCompData = (ilPtr)pCompData;

    pPriv = (ilJIFToRawPrivPtr)ilAddPipeElement (pipe, IL_FILTER,
            sizeof (ilJIFToRawPrivRec), 0, (ilSrcElementData *)NULL, &dstData,
            IL_NPF, IL_NPF, ilJIFToRawDestroy, ilJIFToRawExecute, 0);
    if (!pPriv)
        return FALSE;

    pPriv->pCompData = pCompData;
    return TRUE;
}


    /*  ============================= Raw to JIF =================================== */
    /*  This body of code supports converting "raw" (stripped) format JPEG to JIF.
    */

    /*  Private data for ilRawToJIFExecute() */
typedef struct {
    ilJPEGData             *pCompData;          /* ptr to IL JPEG data */
    long                    width;              /* width of image */
    long                    height;             /* total # lines in image */
    long                    stripHeight;        /* # lines per strip */
    ilBool                  nStripsWritten;     /* inited to 0 by Init() */
    long                    nLinesWritten;      /* inited to 0 by Init() */
    long                    dstOffset;          /* inited to 0 by Init() */
    } ilRawToJIFPrivRec, *ilRawToJIFPrivPtr;


    /*  ----------------------- ilRawToJIFInit ------------------------------ */
    /*  Init() function for converting JIF format pipe image to raw format.
    */
static ilError ilRawToJIFInit (
    ilRawToJIFPrivPtr       pPriv,
    ilImageInfo             *pSrcImage,
    ilImageInfo             *pDstImage
    )
{
    pPriv->nStripsWritten = 0;
    pPriv->nLinesWritten = 0;
    pPriv->dstOffset = 0;
    return IL_OK;
}


    /*  ----------------------- ilRawToJIFExecute ------------------------------ */
    /*  Execute() function for converting "raw" JPEG to JIF format.
    */
static ilError ilRawToJIFExecute (
    ilExecuteData          *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
    ilJPEGEncodeStream      streamRec;
    ilRawToJIFPrivPtr       pPriv;
    iljpgDataRec            data;
    ilImageInfo            *pImage;
    ilError                 error;
    long                    nBytes, startOffset;
    ilBool                  lastStrip;


    pPriv = (ilRawToJIFPrivPtr)pData->pPrivate;

        /*  Setup stream to point to dst image buffer; set size, offset also.
            "dstOffset" is offset into dst buffer; it is incremented by the # of
            bytes written on each strip.  When done, it is size of JIF image written.
        */
    pImage = pData->pDstImage;
    streamRec.pBuffer = pImage->plane[0].pPixels;
    streamRec.pDst = streamRec.pBuffer + pPriv->dstOffset;
    streamRec.pPastEndBuffer = streamRec.pBuffer + pImage->plane[0].bufferSize;
    startOffset = ILJPG_ENCODE_OFFSET(&streamRec);  /* stream offset before any writes */
    pImage = pData->pSrcImage;                      /* point to src (raw) image */

        /*   If first strip write out the JIF header */
    if (pPriv->nStripsWritten == 0) {

            /*  Setup iljpg data block: general info and tables from IL */
        _ilJPEGDataIn (pImage->pDes, pPriv->width, pPriv->height, &data);
        _ilJPEGTablesIn (pPriv->pCompData, &data);

            /*  Set restart interval: if a single strip, it is the restart interval
                in the raw data (already set from above).  If not a single strip, then
                the raw data must not have a restart interval or an error - can't code
                both of them!  Otherwise restart interval is determined by strip size.
            */
        if (pPriv->stripHeight != pPriv->height) {  /* not single strip */
            long        mcuWidth, mcuHeight;
            if (data.restartInterval)
                return IL_ERROR_COMPRESSED_DATA;    /* have restartInterval; error */
            mcuWidth = 8 * data.maxHoriFactor;
            mcuHeight = 8 * data.maxVertFactor;
            data.restartInterval =                  /* # of mcus across * # down */
                ((data.width + mcuWidth - 1) / mcuWidth) *
                ((pPriv->stripHeight + mcuHeight - 1) / mcuHeight);
            }

            /*  Write JIF to output stream, from IL's comp data */
        if (error = iljpgEncodeJIF (&streamRec, &data, (iljpgJIFOffsetsPtr)NULL))
            return error;
        }

        /*  Write data from this strip to output buffer, reallocating if necessary. 
            Allocate 2 extra bytes for EOI or RSTn marker.
        */
    nBytes = pData->compressed.nBytesToRead;
    if ((streamRec.pDst + nBytes + 2) >= streamRec.pPastEndBuffer) {
        if (error = _ilReallocJPEGEncode (&streamRec, nBytes + 2))
            return error;
        }
    bcopy ((char *)(pImage->plane[0].pPixels + pData->compressed.srcOffset),
	   (char *)streamRec.pDst, nBytes);
    streamRec.pDst += nBytes;                   /* bump past data just copied */

        /*  If last strip, write 2 byte EOI marker else write 2 byte restart marker. */
    pPriv->nLinesWritten += *pNLines;
    lastStrip = (pPriv->nLinesWritten >= pPriv->height);
    if (lastStrip) {
        *streamRec.pDst++ = ILJPGM_FIRST_BYTE;  /* last strip; write EOI marker */
        *streamRec.pDst++ = ILJPGM_EOI;
        }
    else {
            /*  Not last strip; write RST marker, with modulo 8 strip count */
        *streamRec.pDst++ = ILJPGM_FIRST_BYTE;
        *streamRec.pDst++ = ILJPGM_RST0 | (pPriv->nStripsWritten & 7);
        }
    pPriv->nStripsWritten += 1;

        /*  Store stream data into dst image; bump dst offset by # bytes written */
    pImage = pData->pDstImage;
    pImage->plane[0].pPixels = streamRec.pBuffer;
    pImage->plane[0].bufferSize = streamRec.pPastEndBuffer - streamRec.pBuffer;
    pPriv->dstOffset += ILJPG_ENCODE_OFFSET(&streamRec) - startOffset;

        /*  If last strip, pass # bytes written to next filter (= current dst offset;
            note that dst offset set to 0 on first strip), and return # lines = 
            height of image, as JIF is one big strip.  If not last strip, return
            # lines as zero and next filter will not be called.
        */
    if (lastStrip) {
        *pNLines = pPriv->height;
        *pData->compressed.pNBytesWritten = pPriv->dstOffset;
        }
    else *pNLines = 0;

    return IL_OK;
}


    /*  ----------------------- ilRawToJIF ------------------------------ */
    /*  Called by _ilCompressJPEG() when the pipe image is "raw" JPEG format,
        to convert to JIF format.
    */
static ilBool ilRawToJIF (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat
    )
{
    ilRawToJIFPrivPtr       pPriv;
    ilDstElementData        dstData;

    dstData.producerObject = (ilObject)NULL;
    pDes->compInfo.JPEG.reserved &= ~IL_JPEGM_RAW;  /* now writing JIF, not raw JPEG */
    dstData.pDes = pDes;
    dstData.pFormat = pFormat;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = pInfo->height;            /* write one strip = whole image */
    dstData.constantStrip = TRUE;
    dstData.pPalette = (unsigned short *)NULL;
    dstData.pCompData = (ilPtr)NULL;

        /*  Add the filter.  "Hold dst" flag set because filter will write to
            output buffer, returning # lines = 0 until last strip written, after which
            whole output buffer will be passed to next filter, # lines = height.
        */
    pPriv = (ilRawToJIFPrivPtr)ilAddPipeElement (pipe, IL_FILTER,
            sizeof (ilRawToJIFPrivRec), IL_ADD_PIPE_HOLD_DST, (ilSrcElementData *)NULL, 
            &dstData, ilRawToJIFInit, IL_NPF, IL_NPF, ilRawToJIFExecute, 0);
    if (!pPriv)
        return FALSE;

    pPriv->pCompData = (ilJPEGData *)pInfo->pCompData;
    pPriv->width = pInfo->width;
    pPriv->height = pInfo->height;
    pPriv->stripHeight = pInfo->stripHeight;        /* input strip height */

    return TRUE;
}


    /*  ============================= Compress Raw =================================== */
    /*  This body of code supports compressing to JPEG "raw" format.
    */

    /*  Private data for ilCompressJPEGExecute() */
typedef struct {
    ilJPEGData             *pCompData;          /* ptr to IL's view of JPEG data */
    iljpgDataRec            jpgData;            /* data for iljpg functions */
    iljpgPtr                jpgPriv;            /* pointer to iljpg private data */
    } ilCompressJPEGPrivRec, *ilCompressJPEGPrivPtr;


    /*  -------------------- ilCompressJPEGInit -------------------------- */
    /*  Init() function for compressing to "raw" JPEG.
    */
static ilError ilCompressJPEGInit (
    ilCompressJPEGPrivPtr   pPriv,
    ilImageInfo            *pSrcImage,
    ilImageInfo            *pDstImage
    )
{
    ilError                 error;

        /*  Init iljpg package for compression */
    if (error = iljpgEncodeInit (&pPriv->jpgData, &pPriv->jpgPriv))
        return error;
    return IL_OK;
}


    /*  -------------------- ilCompressJPEGCleanup -------------------------- */
    /*  Cleanup() function for compressing to "raw" JPEG.
    */
static ilError ilCompressJPEGCleanup (
    ilCompressJPEGPrivPtr   pPriv,
    ilBool                  aborting
    )
{
    ilError                 error;

        /*  Call iljpg package cleanup function */
    return iljpgEncodeCleanup (pPriv->jpgPriv);
}


    /*  ----------------------- ilCompressJPEGDestroy ------------------------------ */
    /*  Destroy() function for compressing to "raw" JPEG.
    */
static ilError ilCompressJPEGDestroy (
    ilCompressJPEGPrivPtr   pPriv
    )
{
    int            i;
    ilPtr                   pTable;
    ilJPEGData    *pCompData;

        /*  Free pCompData and any non-default Q tables */
    if (pCompData = pPriv->pCompData) {
        for (i = 0; i < 4; i++) {
            if ((pTable = pCompData->QTables[i])
             && (pTable != iljpgLuminanceQTable) 
             && (pTable != iljpgChrominanceQTable))
                IL_FREE (pTable);
            }
        IL_FREE (pCompData);
        }
    return IL_OK;
}


    /*  ----------------------- ilCompressJPEGExecute ------------------------------ */
    /*  Execute() function for compressing to "raw" JPEG.
    */
static ilError ilCompressJPEGExecute (
    ilExecuteData          *pData,
    unsigned long           dstLine,
    unsigned long          *pNLines
    )
{
    ilCompressJPEGPrivPtr pPriv;
    ilJPEGEncodeStream      streamRec;
    ilImageInfo            *pImage;
    ilImagePlaneInfo       *pSrcPlane;
    long                    nBytes, startOffset;
    ilError                 error;
    int                     i;
    iljpgPtr                pSrcPixels[IL_MAX_SAMPLES];
    long                    nSrcBytesPerRow[IL_MAX_SAMPLES];

    pPriv = (ilCompressJPEGPrivPtr)pData->pPrivate;

        /*  Setup stream to point to dst image buffer; set size, offset also */
    pImage = pData->pDstImage;
    streamRec.pBuffer = pImage->plane[0].pPixels;
    streamRec.pDst = streamRec.pBuffer + *pData->compressed.pDstOffset;
    streamRec.pPastEndBuffer = streamRec.pBuffer + pImage->plane[0].bufferSize;
    startOffset = ILJPG_ENCODE_OFFSET(&streamRec);  /* stream offset before any writes */

        /*  Encode one strip, writing output to dst buffer (pointed to by streamRec).
            Input is from src buffer, starting at srcLine, divided by subsample factor
            (which is "inverse" of JPEG factors; = max factor / component factor).
        */
    pSrcPlane = pData->pSrcImage->plane;
    for (i = 0; i < pPriv->jpgData.nComps; i++, pSrcPlane++) {
        pSrcPixels[i] = pSrcPlane->pPixels + (pData->srcLine * 
            pPriv->jpgData.comp[i].vertFactor / pPriv->jpgData.maxVertFactor) * 
            pSrcPlane->nBytesPerRow;
        nSrcBytesPerRow[i] = pSrcPlane->nBytesPerRow;
        }
    if (error = iljpgEncodeExecute (pPriv->jpgPriv, &streamRec, *pNLines, 
                                    pSrcPixels, nSrcBytesPerRow))
        return error;

        /*  Set stream data back into dst image structure.  Inform next filter of
            the # bytes written = current stream offset - start offset.
        */
    pImage->plane[0].pPixels = streamRec.pBuffer;
    pImage->plane[0].bufferSize = streamRec.pPastEndBuffer - streamRec.pBuffer;
    *pData->compressed.pNBytesWritten = ILJPG_ENCODE_OFFSET(&streamRec) - startOffset;

    return IL_OK;
}


    /*  ------------------------- _ilScaleQTable -------------------------------- */
    /*  Malloc a new 64 byte table, copy given src Q table scaling by given Q factor
        and return ptr to new table, or null if error.
    */
static ilPtr _ilScaleQTable (
    ilPtr          pSrc,           /* ptr to 64 byte Q table to scale */
    int                     Q               /* IL Q factor: != 0, or no scale needed */
    )
{
    ilPtr                   pTable;
    ilPtr          pDst;
    int            i, value;

    if (!(pTable = (ilPtr)IL_MALLOC (64)))
        return (ilPtr)NULL;

        /*  Scale src to dst (pTable) by: dst = src * Qfactor / 50.
            I don't know where this came from, but it is how the old code worked.
            The old code however only allowed a max Q of 255; larger values became 255.
            Here we will allow the maximum value of a short.
        */
    for (i = 0, pDst = pTable; i < 64; i++) {
        value = (*pSrc++ * Q) / 50;
        if (value > 255)
            value = 255;
        else if (value < 1)
            value = 1;
        *pDst++ = value;
        }
    return pTable;
}


    /*  ----------------------- _ilCompressJPEG ------------------------------ */
    /*  Called by ilCompress() when desired compression is JPEG.  The image
        may already be JPEG-compressed, but possibly in the wrong format (JIF/raw).
        This function handles conversions between those cases, or does
        nothing if image is already in correct format.
            On entry, the image must be either JPEG-compressed or uncompressed.
    */
IL_PRIVATE ilBool _ilCompressJPEG (
    ilPipe                  pipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat,
    ilSrcElementData       *pSrcData,
    ilPtr                   pEncodeData
    )
{
    ilCompressJPEGPrivPtr   pPriv;
    ilJPEGData             *pCompData;
    ilDstElementData        dstData;
    ilJPEGEncodeControl     *pEncode;
    int                     i, tableIndex, luminanceMask;
    ilPtr                   pTable;
    ilBool                  mustConvert;
    long                    stripHeight, maxVert;
    static ilJPEGEncodeControl  ilDefaultJpegEncodeControl = {
        0,         /* short               mustbezero; */
        0          /* short               Q;     */
        };

        /*  Check given encode data; default to JIF format, Q of 0 if null.  
            Unused part of mustbezero must be zero or error.  
        */
    if (!pEncodeData)
         pEncode = &ilDefaultJpegEncodeControl;
    else pEncode = (ilJPEGEncodeControl *)pEncodeData;
    if (pEncode->mustbezero & ~IL_JPEGM_RAW)
       return ilDeclarePipeInvalid (pipe, IL_ERROR_PAR_NOT_ZERO);

        /*  If already JPEG-compressed cvt to requested format (JIF/raw) if necessary */
    if (pDes->compression == IL_JPEG) {
        if (pDes->compInfo.JPEG.reserved & IL_JPEGM_RAW)            /* src is raw */
            if (pEncode->mustbezero & IL_JPEGM_RAW) {               /* raw->raw */
                pipe->context->error = IL_OK;
                return TRUE;
                }
            else {                                                  /* raw->JIF */
                return ilRawToJIF (pipe, pInfo, pDes, pFormat);
                }
        else                                                        /* src is JIF */
            if (pEncode->mustbezero & IL_JPEGM_RAW) {               /* JIF->raw */
                return ilJIFToRaw (pipe, pInfo, pDes, pFormat);
                }
            else {                                                  /* JIF->JIF */
                pipe->context->error = IL_OK;
                return TRUE;
                }
        }

        /*  Image is not JPEG; must be uncompressed.  Check for supported image type.
            Set maxVert to max vert subsampling; luminanceMask to sample #.
        */
    switch (pDes->type) {
      case IL_GRAY:
        maxVert = 1;
        luminanceMask = 1;              /* gray is luminance */
        break;
      case IL_RGB:
        maxVert = 1;                    /* no subsampling of RGB */
        luminanceMask = ~0;             /* use luminance tables for all components */
        break;
      case IL_YCBCR:
        maxVert = 1;
        if (pDes->typeInfo.YCbCr.sample[1].subsampleVert > maxVert)
            maxVert = pDes->typeInfo.YCbCr.sample[1].subsampleVert;
        if (pDes->typeInfo.YCbCr.sample[2].subsampleVert > maxVert)
            maxVert = pDes->typeInfo.YCbCr.sample[2].subsampleVert;
        luminanceMask = 1;            /* Y is luminance */
        break;
      default:
        return ilDeclarePipeInvalid (pipe, IL_ERROR_IMAGE_TYPE);
        }

        /*  Force all samples to be 8 bit, 256 levels / sample */
    for (i = 0, mustConvert = FALSE; i < pDes->nSamplesPerPixel; i++) {
        if (pDes->nLevelsPerSample[i] != 256) {
            pDes->nLevelsPerSample[i] = 256;
            mustConvert = TRUE;
            }
        if (pFormat->nBitsPerSample[i] != 8) {
            pFormat->nBitsPerSample[i] = 8;
            mustConvert = TRUE;
            }
        }
    if (mustConvert)
        if (!ilConvert (pipe, pDes, pFormat, 0, (ilPtr)NULL))
            return FALSE;

        /*  Convert to planar format if more than 1 sample/pixel. */
    if ((pDes->nSamplesPerPixel > 1) && (pFormat->sampleOrder != IL_SAMPLE_PLANES)) {
        pFormat->sampleOrder = IL_SAMPLE_PLANES;
        if (!ilConvert (pipe, (ilImageDes *)NULL, pFormat, 0, (ilPtr)NULL))
            return FALSE;
        }

        /*  Check pSrcData->stripHeight: make it a multiple of MCU. */
    maxVert *= 8;                             /* now height of an MCU */
    stripHeight = ((pSrcData->stripHeight + maxVert - 1) / maxVert) * maxVert;
    if (stripHeight <= 0)
        stripHeight = maxVert;
    if (stripHeight > pInfo->height)
        stripHeight = pInfo->height;
    pSrcData->stripHeight = stripHeight;

        /*  Allocate an IL raw JPEG data block; point to it for pipe element */
    if (!(pCompData = (ilJPEGData *)IL_MALLOC_ZERO (sizeof (ilJPEGData))))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_MALLOC);

        /*  Add the pipe element */
    dstData.producerObject = (ilObject)NULL;
    pDes->compInfo.JPEG.reserved |= IL_JPEGM_RAW;   /* now writing raw JPEG */
    pDes->compression = IL_JPEG;
    dstData.pDes = pDes;
    pFormat->sampleOrder = IL_SAMPLE_PIXELS;        /* JPEG interleaved order */
    dstData.pFormat = pFormat;
    dstData.width = pInfo->width;
    dstData.height = pInfo->height;
    dstData.stripHeight = stripHeight;
    dstData.constantStrip = TRUE;
    dstData.pPalette = (unsigned short *)NULL;
    dstData.pCompData = (ilPtr)pCompData;

    pPriv = (ilCompressJPEGPrivPtr)ilAddPipeElement (pipe, IL_FILTER,
            sizeof (ilCompressJPEGPrivRec), 0, pSrcData, &dstData,
            ilCompressJPEGInit, ilCompressJPEGCleanup, ilCompressJPEGDestroy, 
            ilCompressJPEGExecute, 0);
    if (!pPriv)
        return FALSE;

        /*  Init tables to use: default luminance (index 0) or chrominance (index 1).
            If "Q" not zero, scale up Q tables; must destroy them in Destroy().
        */
    pPriv->pCompData = pCompData;
    pCompData->restartInterval = 0;                 /* no restart markers added */

    for (i = 0; i < pDes->nSamplesPerPixel; i++) {
        if ((1 << i) & luminanceMask) {
            pCompData->DCTables[0] = iljpgLuminanceDCTable;
            pCompData->ACTables[0] = iljpgLuminanceACTable;
            tableIndex = 0;
            pTable = iljpgLuminanceQTable;
            }
        else {
            pCompData->DCTables[1] = iljpgChrominanceDCTable;
            pCompData->ACTables[1] = iljpgChrominanceACTable;
            tableIndex = 1;
            pTable = iljpgChrominanceQTable;
            }
        if (!pCompData->QTables[tableIndex]) {      /* no Q table here yet; make one */
            if (pEncode->Q)                         /* Q != 0; scale copy of Q table */
                if (!(pTable = _ilScaleQTable (pTable, pEncode->Q)))
                    return ilDeclarePipeInvalid (pipe, IL_ERROR_MALLOC);
            pCompData->QTables[tableIndex] = pTable;
            }
        pCompData->sample[i].QTableIndex = pCompData->sample[i].DCTableIndex = 
            pCompData->sample[i].ACTableIndex = tableIndex;
        }

        /*  Init iljpg package data in private, based on IL view of JPEG comp data */
    _ilJPEGDataIn (pDes, pInfo->width, pInfo->height, &pPriv->jpgData);
    _ilJPEGTablesIn (pCompData, &pPriv->jpgData);

        /*  Data is now compressed to "raw" format.  Convert to JIF if requested */
    if (!(pEncode->mustbezero & IL_JPEGM_RAW)) {
        ilGetPipeInfo (pipe, FALSE, pInfo, pDes, pFormat);  /* update pipe info */
        return ilRawToJIF (pipe, pInfo, pDes, pFormat);
        }

    return TRUE;
}

