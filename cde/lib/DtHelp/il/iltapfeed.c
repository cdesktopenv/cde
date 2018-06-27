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
/* $XConsortium: iltapfeed.c /main/3 1995/10/23 16:00:47 rswiston $ */
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

        /*  /ilc/iltapfeed.c : Code for feeding and tapping pipes.
        */

#include "ilint.h"
#include "ilpipelem.h"
#include "ilpipeint.h"
#include "ilerrors.h"

        /*  ------------------------ ilFeedFromImage ---------------------------------- */
        /*  Public function: see spec.
            Adds the given image as a producer of the pipe, but it is only a "partial"
            producer.  It will not contain all the bits needed; instead a strip's worth
            of bits (of max height "stripHeight") will be in the image each time 
            ilFeedPipe() is called.  "height" is the logical final height of the image.
        */
ilBool ilFeedFromImage (
    ilPipe              pipe,
    ilObject            image,
    long                height,
    long                stripHeight,
    ilBool              constantStrip
    )
{
ilImagePtr     pImage;

        /*  Validate that pipe and image are such, and that they have the same context. */
    pImage = (ilImagePtr)image;
    if (pipe->objectType != IL_PIPE) {
        pipe->context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }
    if ((pImage->o.p.objectType != IL_INTERNAL_IMAGE) 
     && (pImage->o.p.objectType != IL_CLIENT_IMAGE))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_OBJECT_TYPE);

    if (pImage->o.p.context != pipe->context)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_CONTEXT_MISMATCH);

        /*  Get pipe state - must be IL_PIPE_EMPTY or declare an error. */
    if (ilGetPipeInfo (pipe, FALSE, (ilPipeInfo *)NULL, (ilImageDes *)NULL, 
                                    (ilImageFormat *)NULL) != IL_PIPE_EMPTY) {
        if (!pipe->context->error)
            ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);
        return FALSE;
        }

        /*  Make internal call to set this image as the producer of the pipe. */
    return _ilAddProducerImage (pipe, pImage, IL_PIPE_FEED_IMAGE, height, stripHeight, 
                               constantStrip, TRUE);
}


        /*  ------------------------ ilFeedPipe ---------------------------------- */
        /*  Public function: see spec.
        */
int ilFeedPipe (
    ilPipe              pipe,
    long                start,
    long                nLines,
    long                nCompBytes,
    unsigned long       mustBeZero
    )
{

    if (mustBeZero) {
        pipe->context->error = IL_ERROR_PAR_NOT_ZERO;
        return IL_EXECUTE_ERROR;
        }
    if (pipe->objectType != IL_PIPE) {
        pipe->context->error = IL_ERROR_OBJECT_TYPE;
        return IL_EXECUTE_ERROR;
        }
    if (!_ilSetFeedPipeData (pipe, start, nLines, nCompBytes)) {
        pipe->context->error = IL_ERROR_FEED_PIPE;
        return IL_EXECUTE_ERROR;
        }

    return ilExecutePipe (pipe, 1);
}



/*  ================================= TAP PIPE =================================== */

    /*  Private data for element added by ilTapPipe() to tapped pipe.
    */
typedef struct {
    ilImageInfo        *pFeedInfo;          /* ptr into feed image structure */
    ilClientImage       feedImage;          /* feed image to offshoot pipe */
    ilPipe              pipe;               /* offshoot pipe (the one to be fed) */
    } ilTapPrivRec, *ilTapPrivPtr;


        /*  ------------------------ ilTapInit ---------------------------------- */
        /*  Init() function for element added by ilTapPipe() to the tapped pipe.
            Point the feed image for the offshoot pipe to the pixels in the src
            image, so the offshoot pipe will read directly from the bits in the main pipe.
        */
static ilError ilTapInit (
    ilTapPrivPtr        pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
int            plane;
ilImageInfo            *pFeedInfo;

    pFeedInfo = pPriv->pFeedInfo;
    for (plane = 0; plane < IL_MAX_SAMPLES; plane++)
        pFeedInfo->plane[plane] = pSrcImage->plane[plane];
    return IL_OK;
}

        /*  ------------------------ ilTapDestroy ---------------------------------- */
        /*  Destroy() function for element added by ilTapPipe() to the tapped pipe.
            Destroy the offshoot pipe and the feed image.
        */
static ilError ilTapDestroy (
    ilTapPrivPtr        pPriv
    )
{
    ilDestroyObject (pPriv->pipe);
    ilDestroyObject (pPriv->feedImage);
    return IL_OK;
}

        /*  ------------------------ ilTapExecute ---------------------------------- */
        /*  Execute() function for element added by ilTapPipe() to the tapped pipe.
            Feed the pipe bits entering this filter down the offshoot pipe.
        */
static ilError ilTapExecute (
    ilExecuteData  *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilTapPrivPtr   pPriv;
int            plane;
ilImageInfo            *pFeedInfo, *pSrcImage;
long                    start, nCompBytes;

        /*  Point feedImage directly to the src data in the pipe.  This was done in
            ilTapInit(), but must be done again in case the pipe image is compressed,
            in which case pPixels for example could change on each strip.
        */
    pPriv = (ilTapPrivPtr)pData->pPrivate;
    pFeedInfo = pPriv->pFeedInfo;
    pSrcImage = pData->pSrcImage;
    for (plane = 0; plane < IL_MAX_SAMPLES; plane++)
        pFeedInfo->plane[plane] = pSrcImage->plane[plane];

        /*  Feed the src lines or bytes if compressed down the offshoot pipe. This is a
            "no dst" filter, so point next filter to our data: lines or bytes (compressed)
        */
    if (pSrcImage->pDes->compression == IL_UNCOMPRESSED) {
        start = pData->srcLine;
        *pData->pNextSrcLine = start;
        nCompBytes = 0;
        }
    else {
        start = pData->compressed.srcOffset;
        *pData->compressed.pDstOffset = start;
        nCompBytes = pData->compressed.nBytesToRead;
        *pData->compressed.pNBytesWritten = nCompBytes;
        }
    if (ilFeedPipe (pPriv->pipe, start, *pNLines, nCompBytes, 0) == IL_EXECUTE_ERROR)
        return pPriv->pipe->context->error;

    return IL_OK;
}


        /*  ------------------------ ilTapPipe ---------------------------------- */
        /*  Public function: see spec.
        */
ilPipe ilTapPipe (
    ilPipe     pipe,
    unsigned long       mustBeZero,
    void               *mustBeNull
    )
{
ilContext               context;
ilPipe                  newPipe;
ilClientImage           feedImage;
ilPipeInfo              info;
ilImageDes              des;
ilImageFormat           format;
ilImageInfo             imageInfo, *pFeedInfo;
ilSrcElementData        srcData;
ilError                 error;
ilTapPrivPtr   pPriv;

    context = pipe->context;
    feedImage = (ilClientImage)NULL;
    newPipe = (ilPipe)NULL;

        /*  Get pipe info - must be IL_PIPE_FORMING or declare an error. */
    if (ilGetPipeInfo (pipe, FALSE, &info, &des, &format) != IL_PIPE_FORMING) {
        if (!context->error)
            context->error = IL_ERROR_PIPE_STATE;
        goto tpError;
        }

    if (mustBeZero || mustBeNull) {
        context->error = IL_ERROR_PAR_NOT_ZERO;
        goto tpError;
        }

        /*  If pipe image is "raw" JPEG, convert to JIF format before adding tap.
            This is because we use the pipe image as a client image, but only JIF
            format client images are supported.
        */
    if ((des.compression == IL_JPEG) && (des.compInfo.JPEG.reserved & IL_JPEGM_RAW)) {
        if (!ilCompress (pipe, IL_JPEG, (ilPtr)NULL, 0, 0))
            goto tpError;
        }

        /*  Create newPipe, to returned by this function, into which bits will be fed
            as they stream down this pipe.  Create feedImage with which to feed newPipe,
            with des/format of pipe image, but with no buffer yet.  Feed into newPipe.
        */
    if (!(newPipe = ilCreatePipe (context, 0)))
        goto tpError;

    imageInfo.pDes = &des;
    imageInfo.pFormat = &format;
    imageInfo.pPalette = info.pPalette;             /* point to tapped pipe's palette */
    imageInfo.pCompData = info.pCompData;
    imageInfo.width = info.width;
    imageInfo.height = info.height;
    imageInfo.clientPalette = TRUE;                 /* don't allocate anything */
    imageInfo.clientCompData = TRUE;
    imageInfo.clientPixels = TRUE;
    if (!(feedImage = ilCreateClientImage (context, &imageInfo, 0))
     || !ilFeedFromImage (newPipe, feedImage, info.height, info.stripHeight,
                          info.constantStrip))
        goto tpError;

        /*  Add an element to the pipe to be tapped to feed the offshoot pipe; init *pPriv.
            Get a ptr into the feed image data structure, to make point to input image.
        */
    srcData.consumerImage = (ilObject)NULL;
    srcData.stripHeight = info.stripHeight;         /* accept input strip height */
    srcData.constantStrip = FALSE;
    srcData.minBufferHeight = 0;
    if (!(pPriv = (ilTapPrivPtr)ilAddPipeElement (pipe, IL_FILTER, sizeof (ilTapPrivRec),
                    IL_ADD_PIPE_NO_DST, &srcData, (ilDstElementData *)NULL,
                    ilTapInit, IL_NPF, ilTapDestroy, ilTapExecute, 0)))
        goto tpError;

    ilQueryClientImage (feedImage, &pPriv->pFeedInfo, 0);
    pPriv->feedImage = feedImage;
    pPriv->pipe = newPipe;

    context->error = IL_OK;
    return newPipe;

    /*  Goto point if error - free objects, post "context->error" to pipe, return null. */
tpError:
    error = context->error;             /* save error; ilDestroyObject() will overwrite */
    if (newPipe)
        ilDestroyObject (newPipe);
    if (feedImage)
        ilDestroyObject (feedImage);
    ilDeclarePipeInvalid (pipe, error);
    return (ilPipe)NULL;
}


