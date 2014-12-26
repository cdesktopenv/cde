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
/* $XConsortium: ilrwimage.c /main/6 1996/06/19 12:21:00 ageorge $ */
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

        /*  /ilc/ilrwimage.c : A misnomer.  Now contains pipe throttles
            inserted by ilAddPipeElement().
                NOTE: code in this file handles uncompressed images only.
        */

#include "ilint.h"
#include "ilimage.h"
#include "ilpipelem.h"
#include "ilpipeint.h"
#include "ilerrors.h"


        /*  Private structure for all throttle elements.
            "Init()" are fields that Init() must init, "Add()" when pipe element is added,
            else temp values used during pipe execution.
        */
typedef struct {
    ilBool              beingRecalled;      /* Init(): ilExecutePipe() recalling us? */
    long                nBufferLines;       /* Init(): # of unprocessed lines in buffer */
    long                nLinesReceived;     /* Init(): total # src lines received */
    long                imageHeight;        /* Add(): total height of image */
    long                stripHeight;        /* Add(): size of each output strip */
    } ilThrottlePrivRec, *ilThrottlePrivPtr;


        /*  ---------------------- ilThrottleInit --------------------------------- */
        /*  Init() function for throttle pipe elements.
        */
static ilError ilThrottleInit (
    ilThrottlePrivPtr   pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
    pPriv->beingRecalled = FALSE;
    pPriv->nBufferLines = 0;
    pPriv->nLinesReceived = 0;

    return IL_OK;
}

        /*  --------------------- ilProducerThrottleExecute -------------------------- */
        /*  Execute() for a filter inserted by ilAddThrottlePipeElement() to be the 
            logical producer, i.e. the pipe began with an ilReadImage().  Must
            output constant strips (except possibly the last) and return "last strip"
            error on last one.
        */
static ilError ilProducerThrottleExecute (
    register ilExecuteData  *pData,
    long                    dstLine,            /* ignored */
    long                   *pNLines             /* ignored on input */
    )
{
register ilThrottlePrivPtr  pPriv;

        /*  Set the next elements srcLine to our current line.  Bump srcLine by
            stripHeight (# lines just "written"), but if that is to/past end of image,
            return last strip error, else "ok", to do more strips.
        */
    pPriv = (ilThrottlePrivPtr)pData->pPrivate;
    *pData->pNextSrcLine = pData->srcLine;
    if ((pData->srcLine + pPriv->stripHeight) >= pPriv->imageHeight) {
        *pNLines = pPriv->imageHeight - pData->srcLine;
        return IL_ERROR_LAST_STRIP;
        }
    else {
        *pNLines = pPriv->stripHeight;
        pData->srcLine += pPriv->stripHeight;
        return IL_OK;
        }
}

        /*  --------------------- ilFeedProducerThrottleExecute ----------------------- */
        /*  Execute() for a filter inserted by ilAddThrottlePipeElement() to be the 
            logical producer when the pipe began with an ilFeedFromImage().
                Coupled to ilExecutePipe(): *pNLines on input must be nLines and 
            pData->srcLine must be startLine, as passed to ilFeedPipe()
        */
static ilError ilFeedProducerThrottleExecute (
    register ilExecuteData  *pData,
    long                    dstLine,            /* ignored */
    long                   *pNLines
    )
{
register ilThrottlePrivPtr  pPriv;
long                        nLines;

        /*  Point next element to the feed data, set into pData by ilExecutePipe():
            srcLine (for uncompressed) and srcOffset, nBytesToRead for compressed data.
            If all lines received (count in nLinesReceived), return "last strip", else ok.
        */
    pPriv = (ilThrottlePrivPtr)pData->pPrivate;
    *pData->pNextSrcLine = pData->srcLine;
    *pData->compressed.pDstOffset = pData->compressed.srcOffset;
    *pData->compressed.pNBytesWritten = pData->compressed.nBytesToRead;

    nLines = *pNLines;
    if ((nLines + pPriv->nLinesReceived) >= pPriv->imageHeight) {
        *pNLines = pPriv->imageHeight - pPriv->nLinesReceived;
        return IL_ERROR_LAST_STRIP;
        }
    else {
        pPriv->nLinesReceived += nLines;
        return IL_OK;
        }
}


        /*  --------------------- ilNonConstantThrottleExecute --------------------- */
        /*  Execute() for when not the producer, but constant strips are not required.
        */
static ilError ilNonConstantThrottleExecute (
    register ilExecuteData  *pData,
    long                    dstLine,            /* ignored */
    long                   *pNLines
    )
{
register ilThrottlePrivPtr  pPriv;


        /*  Not from a producer, but !constantStrip; easy: caller just wants strips
            of size "stripHeight" or smaller.  For each strip written to this throttle,
            break it into "stripHeight" or smaller pieces; completely process each strip.
                Move srcLine thru image; always have next element read from srcLine.
            If not beingRecalled, then previous filter actually wrote something to us:
            store how much was written into nBufferLines; exit if nothing.
        */
    *pData->pNextSrcLine = pData->srcLine;
    pPriv = (ilThrottlePrivPtr)pData->pPrivate;
    if (!pPriv->beingRecalled) {
        pPriv->nBufferLines = *pNLines;
        if (!pPriv->nBufferLines)
            return IL_OK;                                   /* EXIT */
        }

        /*  Write "pPriv->stripHeight" sized strips.  If that or less in buffer, write it
            and cancel the recall if on; reset srcLine to be beginning of buffer.
        */
    if ((pData->srcLine + pPriv->stripHeight) >= pPriv->nBufferLines) {
        *pNLines = pPriv->nBufferLines - pData->srcLine;
        pData->srcLine = 0;
        if (pPriv->beingRecalled) {
            pPriv->beingRecalled = FALSE;
            return IL_ERROR_ELEMENT_COMPLETE;               /* EXIT */
            }
        else return IL_OK;                                  /* EXIT */
        }

        /*  More than pPriv->stripHeight lines in buffer: write stripHeight lines, bump
            srcLine by that amount, and if not beingRecalled, make it so.
        */
    *pNLines = pPriv->stripHeight;
    pData->srcLine += pPriv->stripHeight;
    if (!pPriv->beingRecalled) {
        pPriv->beingRecalled = TRUE;
        return IL_ERROR_ELEMENT_AGAIN;                      /* EXIT */
        }
    else return IL_OK;                                      /* EXIT */

}


        /*  --------------------- ilConstantThrottleExecute --------------------- */
        /*  Execute() for when not a producer and constant strips are required.
        */
static ilError ilConstantThrottleExecute (
    register ilExecuteData  *pData,
    long                    dstLine,            /* ignored */
    long                   *pNLines
    )
{
register ilThrottlePrivPtr  pPriv;
long                        nLines, nLinesToWrite;
register ilError            returnCode;

    pPriv = (ilThrottlePrivPtr)pData->pPrivate;
    if (pPriv->beingRecalled)
         nLines = 0;                        /* # lines passed is garbage on a recall */
    else {
        nLines = *pNLines;
        pPriv->nBufferLines += nLines;
        pPriv->nLinesReceived += nLines;
        }

            /*  If have (at least) a strip's worth in buffer, "write" it and request a
                recall.  If beingRecalled (i.e. this code was done on the previous call),
                bump srcLine by stripHeight (# lines done before), else set srcLine to
                zero to start at the beginning of the buffer.
            */
    if (pPriv->nBufferLines >= pPriv->stripHeight) {
        nLinesToWrite = pPriv->stripHeight;
        pPriv->nBufferLines -= pPriv->stripHeight;
        if (pPriv->beingRecalled) {
            pData->srcLine += pPriv->stripHeight;
            returnCode = IL_OK;
            }
        else {
            pPriv->beingRecalled = TRUE;
            pData->srcLine = 0;
            returnCode = IL_ERROR_ELEMENT_AGAIN;
            }
        }
    else {
            /*  Dont have a strip's worth in buffer.  If beingRecalled, then some lines
                have been read out from beginning of buffer by next element.  Copy the
                the lines above that point down to the beginning of the buffer, and
                set srcLine to the line past them.
                    If not being recalled, bump srcLine by # lines received this strip.
            */
        if (pPriv->beingRecalled) {
            int                 nPlanes;
            ilImagePlaneInfo   *pPlane; 
            const ilImageDes   *pDes = pData->pSrcImage->pDes;

            pData->srcLine += pPriv->stripHeight;
            if (pPriv->nBufferLines > 0) {
                nPlanes = (pData->pSrcImage->pFormat->sampleOrder == IL_SAMPLE_PIXELS) ? 
                             1 : pDes->nSamplesPerPixel;
                pPlane = pData->pSrcImage->plane;

                    /*  If a YCbCr image, copy subsampled buffers properly. */
                if (pDes->type == IL_YCBCR) {
                    int vertShift;
                    const ilYCbCrSampleInfo *pSample = pDes->typeInfo.YCbCr.sample;
                    while (nPlanes-- > 0) {
                        vertShift = _ilSubsampleShift [pSample->subsampleVert];
                        pSample++;
                        bcopy ((char *)(pPlane->pPixels + 
                            (pData->srcLine >> vertShift) * pPlane->nBytesPerRow),
                            (char *)pPlane->pPixels, 
                            (pPriv->nBufferLines >> vertShift) * pPlane->nBytesPerRow);
                        pPlane++;
                        }
                    }
                else {
                    while (nPlanes-- > 0) {
                        bcopy ((char *)(pPlane->pPixels + 
                            pData->srcLine * pPlane->nBytesPerRow),
                            (char *)pPlane->pPixels, 
                            pPriv->nBufferLines * pPlane->nBytesPerRow);
                        pPlane++;
                        }
                    }
                }
            pData->srcLine = pPriv->nBufferLines;
            }
        else pData->srcLine += nLines;

            /*  Dont have strip's worth, need more lines: if all lines read, write a 
                short final strip, otherwise write 0 lines and wait for more lines to 
                come in, written to pData->srcLine.
            */
        if (pPriv->nLinesReceived >= pPriv->imageHeight) {
            nLinesToWrite = pPriv->nBufferLines;        /* write last (short) strip */
            pPriv->nBufferLines = 0;
            pData->srcLine = 0;
            }
        else nLinesToWrite = 0;

            /*  Cancel recall if one in effect, mark that we are not in a recall. */
        returnCode = (pPriv->beingRecalled) ? IL_ERROR_ELEMENT_COMPLETE : IL_OK;
        pPriv->beingRecalled = FALSE;
        }       /* END need more lines */

            /*  Tell next element to read "nLinesWrite" starting at "pData->srcLine". */
    *pNLines = nLinesToWrite;
    *pData->pNextSrcLine = pData->srcLine;
    return returnCode;
}


        /*  ------------------ ilCopyImageExecute ------------------------------- */
        /*  Copies the src image to the dst image.  Used when pipe is formed by an 
            ilReadImage() followed immediately by an ilWriteImage().
                Handles uncompressed images only; compressed images handled separately.
        */
static ilError ilCopyImageExecute (
    register ilExecuteData  *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilImageInfo                *pSrcImage, *pDstImage;
ilImagePlaneInfo           *pSrcPlane, *pDstPlane;
const ilYCbCrSampleInfo    *pSample;
long                        srcRowBytes, dstRowBytes;
long                        nLines, nBytes, srcStart, dstStart;
int                         nPlanes;
ilPtr                       pSrcLine, pDstLine;
register long               lineCount;

        /*  Point to src/dst image and first plane of each */
    pSrcImage = pData->pSrcImage;
    pDstImage = pData->pDstImage;
    pSrcPlane = &pSrcImage->plane[0];
    pDstPlane = &pDstImage->plane[0];

    nLines = *pNLines;

        /*  If a YCbCr image point pSample to first plane's sample info; else null */
    if (pSrcImage->pDes->type == IL_YCBCR)
         pSample = pSrcImage->pDes->typeInfo.YCbCr.sample;
    else pSample = (ilYCbCrSampleInfo *)NULL;

        /*  Loop thru and copy, plane by plane: 1 plane if pixel order.  If YCbCr image 
            (pSample) divide nLines and src/dstStart by subsample factor (1,2 or 4).
        */
    nPlanes = (pSrcImage->pFormat->sampleOrder == IL_SAMPLE_PIXELS) ? 
                        1 : pSrcImage->pDes->nSamplesPerPixel;
    while (nPlanes-- > 0) {
        lineCount = nLines;
        srcStart = pData->srcLine;
        dstStart = dstLine;
        if (pSample) {
            int         vertShift;
            vertShift = _ilSubsampleShift [pSample->subsampleVert];
            lineCount >>= vertShift;
            srcStart >>= vertShift;
            dstStart >>= vertShift;
            pSample++;
            }

        srcRowBytes = pSrcPlane->nBytesPerRow;
        pSrcLine = pSrcPlane->pPixels + srcStart * srcRowBytes;
        pSrcPlane++;
        dstRowBytes = pDstPlane->nBytesPerRow;
        pDstLine = pDstPlane->pPixels + dstStart * dstRowBytes;
        pDstPlane++;

            /*  Copy min(src,dst) row bytes from each line; set to -1 */
        nBytes = (srcRowBytes < dstRowBytes) ? srcRowBytes : dstRowBytes;
        if (nBytes <= 0)
            continue;                /* no bytes (? perhaps subsampled ?), next plane */

        while (lineCount-- > 0) {
            bcopy ((char *)pSrcLine, (char *)pDstLine, nBytes);
            pSrcLine += srcRowBytes;
            pDstLine += dstRowBytes;
            }
        }   /* END one plane */

    return IL_OK;
}


        /*  ---------------------- ilInsertCopyFilter ------------------------------ */
        /*  Insert a "filter" which merely copies the pipe image to the dest image.
            This is necessary when the pipe is "<Read Image> <WriteImage>", because both
            ilReadImage() allows the next filter to read directly from its image, and 
            ilWriteImage() allows the previous filter to write directly to its image,
            but together, there needs to be a filter in between.  It is also used after
            an IL_ADD_PIPE_NO_DST filter.
        */
IL_PRIVATE ilBool _ilInsertCopyFilter (
    ilPipe              pipe,
    long                height
    )
{
ilSrcElementData        srcData;

        /*  Add a filter which copies src to dst.  No private needed; the Execute()
            function ilCopyImageExecute() does all necessary setup.  Specify src
            stripHeight of whole image, to do whole copy in one strip.
        */
    srcData.consumerImage = (ilObject)NULL;
    srcData.stripHeight = height;
    srcData.constantStrip = FALSE;
    srcData.minBufferHeight = 0;
    if (!ilAddPipeElement (pipe, IL_FILTER, 0, 0, &srcData, (ilDstElementData *)NULL, 
                           IL_NPF, IL_NPF, IL_NPF, ilCopyImageExecute, NULL, 0))
        return FALSE;

    pipe->context->error = IL_OK;
    return TRUE;
}



    /*  ----------------------- ilAddThrottlePipeElement ---------------------- */
    /*  Called by ilAddPipeElement to add a "throttle" pipe element to feed
        strips of height "stripHeight" or zero (0) to the next element.  
            For uncompressed images only: if "constantStrip" is true, the strips out of
        this filter must all be "stripHeight" or zero lines high, except for the last 
        strip;  if false, the height of each strip must be <= "stripHeight".
            If "producerCode" is not IL_PIPE_NOT_IMAGE, then the throttle is a 
        logical producer: the pipe began with an ilReadImage() or ilFeedFromImage()
        and must signal IL_ERROR_LAST_STRIP; else the throttle is not the producer.
        This flag is used in place of "tempImage" in pipe info which IS INCORRECT -
        ilExecutePipe() changes it to avoid infinite recursion.
            The strip height from this filter is returned to "*pStripHeight", and 
        "*pConstantStrip" is set true if constant strips are output.
            Returns: true if success, else false.
    */
IL_PRIVATE ilBool _ilAddThrottlePipeElement (
    ilPipe              pipe,
    long                stripHeight,
    ilBool              constantStrip,
    unsigned int        producerCode,
    long               *pStripHeight,
    ilBool             *pConstantStrip
    )
{
ilPipeInfo              info;
ilSrcElementData        srcData;
ilThrottlePrivPtr  pPriv = NULL;

    ilGetPipeInfo (pipe, FALSE, &info, (ilImageDes *)NULL, (ilImageFormat *)NULL);
    srcData.consumerImage = (ilObject)NULL;
    srcData.stripHeight = info.stripHeight;
    srcData.constantStrip = FALSE;
    srcData.minBufferHeight = 0;

    switch (producerCode) {

        /*  If reading directly from whole image, always output constant strips. */
      case IL_PIPE_IMAGE:
        pPriv = (ilThrottlePrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
            sizeof (ilThrottlePrivRec), IL_ADD_PIPE_NO_DST, 
            &srcData, (ilDstElementData *)NULL, ilThrottleInit, IL_NPF, IL_NPF, 
            ilProducerThrottleExecute, NULL, 0);
        constantStrip = TRUE;
        break;

        /*  If feeding pipe from an image, first add producer filter to "write"
            the number of lines passed to ilFeedPipe().  If stripHeight written by feed
            (info.stripHeight) is < requested strip height and !constantStrip, or if
            equal and feed gives constant strips, done; else insert a filter to copy
            the feed image to output buffer (else throttle will never get a buffer),
            fall thru and treat same as if pipe not started with a read from an image.
        */
      case IL_PIPE_FEED_IMAGE:
        pPriv = (ilThrottlePrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
            sizeof (ilThrottlePrivRec), IL_ADD_PIPE_NO_DST, 
            &srcData, (ilDstElementData *)NULL, ilThrottleInit, IL_NPF, IL_NPF, 
            ilFeedProducerThrottleExecute, NULL, 0);
        if (!pPriv) return FALSE;
        pPriv->imageHeight = info.height;
        pPriv->stripHeight = stripHeight;

        if (((info.stripHeight <= stripHeight) && !constantStrip)
         || ((info.stripHeight == stripHeight) && info.constantStrip)) 
            break;                              /* else ! FALL THRU TO ... */

            /*  Copy feed image data to buffer, fall thru to add throttle */
        _ilInsertCopyFilter (pipe, info.stripHeight);


        /*  If not constant strips are not required, simple, else tougher:
            Must buffer those passed to us, possibly requesting a recall when we have
            more than one strip's worth in the buffer.  Need space in buffer for 
            worst case: we have 1 line less than needed for (requested) "stripHeight",
            and then previous element then writes "info.stripHeight" lines.
        */
      case IL_PIPE_NOT_IMAGE:
        if (!constantStrip) {
            pPriv = (ilThrottlePrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
                sizeof (ilThrottlePrivRec), IL_ADD_PIPE_NO_DST, 
                &srcData, (ilDstElementData *)NULL, ilThrottleInit, IL_NPF, IL_NPF, 
                ilNonConstantThrottleExecute, NULL, 0);
            }
        else {
            srcData.minBufferHeight = stripHeight + info.stripHeight - 1;
            if (srcData.minBufferHeight > info.height)
                srcData.minBufferHeight = info.height;
            pPriv = (ilThrottlePrivPtr)ilAddPipeElement (pipe, IL_FILTER, 
                sizeof (ilThrottlePrivRec), IL_ADD_PIPE_NO_DST, 
                &srcData, (ilDstElementData *)NULL, ilThrottleInit, IL_NPF, IL_NPF, 
                ilConstantThrottleExecute, NULL, 0);
            }
        break;
        }

        /*  Check for successful element add, init private */
    if (!pPriv) return FALSE;
    pPriv->imageHeight = info.height;
    pPriv->stripHeight = stripHeight;

        /*  Return new strip height and whether constantStrip */
    *pStripHeight = stripHeight;
    *pConstantStrip = constantStrip;
    return TRUE;
}




