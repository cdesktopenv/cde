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
/* $XConsortium: ilpipe.c /main/6 1996/06/19 12:21:13 ageorge $ */
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

        /*  /ilc/ilpipe.c : General pipe handling code.
            Also contains ilCrop() as this function is very much tied to the 
            implementation of pipes.
        */

#include <stdlib.h>

#include "ilcodec.h"
#include "ilint.h"
#include "ilimage.h"
#include "ilpipelem.h"
#include "ilpipeint.h"
#include "ilerrors.h"

    /*  One element in the pipe, added by ilAddElement().  Notes:

            pNext, pPrev    forward/back ptrs to next/prev element in list.  First element
                            in list is *ilPipePtr->elementHead.pNext; last is *pPrev. 

            elementType     One of: IL_PRODUCER, IL_FILTER, IL_CONSUMER.  Not needed or
                            used, but very handy for debugging.

            flags           flag values, e.g. IL_ADD_PIPE_NO_DST

            exec            par block to Execute() for this element: ptrs to src/dst image
                            src/dst lines to read.

            Init()
            Cleanup()
            Destroy()
            Execute()       functions passed to ilAddPipeElement(); see spec for details

            clientPrivate   start of client's private area; MUST BE LAST
    */
typedef struct _ilElementRec {
    struct _ilElementRec *pNext, *pPrev;
    unsigned int        elementType;
    unsigned long       flags;
    ilExecuteData       exec;
    ilError           (*Init)();
    ilError           (*Cleanup)();
    ilError           (*Destroy)();
    ilError           (*ExecuteThree)();
    ilError           (*ExecuteFour)(register ilExecuteData *,
				     long,
				     long *,
				     float);
    unsigned long       clientPrivate;          /* client private: MUST BE LAST */
    } ilElementRec, *ilElementPtr;


    /*  Actual contents of a ilPipe.   Notes:

            o               std object header: MUST BE FIRST

            state           current state of the pipe as returned by ilGetPipeInfo(); one
                            of:

                IL_PIPE_INVALID     an error occurred in forming the pipe, and the pipe 
                                    cannot be used until ilEmptyPipe() is done.  No more 
                                    elements should be added.  The pipe is in fact empty,
                                    only the state is different, to prevent other elements
                                    from being added.  This state is set by calling 
                                    ilDeclarePipeInvalid().

                IL_PIPE_EMPTY       pipe is empty; ready to add a producer (read)

                IL_PIPE_FORMING     pipe has a producer and zero or more filters; ready to
                                    add filters or a consumer (write)

                IL_PIPE_COMPLETE    pipe has a consumer (write); ilExecutePipe() can now
                                    be called on it (executable, from caller's view).  
                                    Error if an attempt to add an element.

                IL_PIPE_EXECUTING   one or more, but not all, strips have been processed.

            producerCode    code that defines producer; one of:
                
                IL_PIPE_NOT_IMAGE   the producer was not an image
                IL_PIPE_IMAGE       producer was an ilReadImage() call
                IL_PIPE_FEED_IMAGE  producer was an ilFeedFromImage() call
            
            feedStartLine
            feedNLines      
            feedCompOffset
            feedCompNBytes
            feedDone        used only if producerCode == IL_PIPE_FEED_IMAGE: values
                            passed to / used by ilSetFeedPipeData()

            elementHead     head/tail of list of ilElementRec, the elements in the pipe

            hookHead        head/tail of list of IL_HOOK ilElementRec, the pseudo-elements
                            in the pipe of type "hook" - not in elementHead list.

            stackIndex      index of top of "execStack"; see below.

            execStack       stack of ptrs to elements that returned IL_ERROR_ELEMENT_AGAIN
                            "stackIndex" points to the top of stack. [0] contains the
                            first element in the list.

            nDestroyObjects # of elements in "destroyObjects" array; see below.

            destroyObjects  an array of "nDestroyObjects" objects which must be destroyed
                            (by calling ilDestroyObject()) when the pipe is emptied.  This
                            array is added to by calling ilAddPipeDestroyObject().

            lastStrip       inited to false when pipe started, set true when "last strip"
                            error returned from an element.  Pipe execution is complete
                            when lastStrip && stackIndex <= 0 (> 0 => last strip has still
                            not made it all the way through the pipe).

            needProducerThrottle true iff ilAddProducerImage() just called: pipe began
                            with an ilReadImage() and no filters added yet.

            copyToConsumerImage  true iff a copy filter must be inserted if the next
                            element is a consumer, specifying a consumerImage, e.g.
                            is ilWriteImage().  This would be true right after 
                            ilAddProducerImage() is called, or right after any 
                            IL_ADD_PIPE_NO_DST filter.

            imageHead       head/tail of list of ilImageRec, the temporary images 
                            associated with this pipe.  The images object prev/next ptrs
                            are used to maintain this list.  Note that the images in this
                            list are not official objects; they must be deleted when then
                            the pipe is emptied.  The images have buffers only when state
                            is IL_PIPE_EXECUTING; the list is null when state is IL_PIPE
                            INVALID or EMPTY.

            pSrcPipeImage   if non-null, points the source (input) pipe image to the next
                            element to be added.  If null, must allocate a src image for
                            the next element unless it is a ilWriteImage().

            image           describes the current pipe image.  The values in this struct
                            meaningful only if pipe forming (state == IL_PIPE_FORMING).
                            The fields are as returned by ilGetPipeInfo().
    */

#define IL_MAX_EXEC_STACK      100
#define IL_MAX_DESTROY_OBJECTS  10

typedef struct {
    ilObjectRec         o;                  /* std header: MUST BE FIRST */
    unsigned int        state;
    unsigned int        producerCode;
    long                feedStartLine, feedNLines, feedCompOffset, feedCompNBytes;
    ilBool              feedDone;
    ilElementRec        elementHead;
    ilElementRec        hookHead;
    int                 stackIndex;
    ilElementPtr        execStack [IL_MAX_EXEC_STACK];
    int                 nDestroyObjects;
    ilObject            destroyObjects [IL_MAX_DESTROY_OBJECTS];
    ilBool              lastStrip;
    ilBool              needProducerThrottle;
    ilBool              copyToConsumerImage;
    ilObjectRec         imageHead;
    ilImageInfo        *pSrcPipeImage;
    struct {
        ilPipeInfo      info;
        ilImageDes      des;
        ilImageFormat   format;
        } image;
    } ilPipeRec, *ilPipePtr;


    /*  Default optimal strip size, in bytes.  Used by ilRecommendedStripHeight().
            NOTE: currently the same as TIFF strip size, "IL_WRITE_TIFF_STRIP_SIZE" in
        /ilc/iltiffwrite.c .  The two are not directly coupled, but making them different
        may adversely affect performance, e.g. cause de/recompression!
    */
static long ilDefaultStripSize = (16 * 1024);

        /*  ------------------------ _ilSetDefaultStripSize  ------------------------ */
        /*  "Back door" call to set ilDefaultStripSize.  The IL test suite uses this 
             call to ensure that small images are stripped, and so that strip
             size is consistent between test baselevels.
        */
void _ilSetDefaultStripSize (
    long                stripSize
    )
{
    ilDefaultStripSize = stripSize;
}


        /*  ------------------------ ilFreeTempImageBuffers ------------------------ */
        /*  Free the buffers associated with the temp images of this pipe.  The image
            structures are not freed; just the pixels.
        */
static void ilFreeTempImageBuffers (
    register ilPipePtr  pPipe
    )
{
register ilImagePtr     pImage;

    pImage = (ilImagePtr)pPipe->imageHead.pNext;
    while (pImage != (ilImagePtr)&pPipe->imageHead) {
        _ilFreeImagePixels (pImage);
        pImage = (ilImagePtr)pImage->o.pNext;
        }
}

        /*  -------------------------- ilAllocTempImage ----------------------- */
        /*  Allocates a temporary (buffer) image, inits it and returns a ptr to it or
            null if failure.  The buffer for the pixels is not allocated.  The 
            height of the image is not set; the caller must do that.
        */
static ilImagePtr ilAllocTempImage (
    register ilPipePtr      pPipe,
    ilPipeInfo             *pInfo,
    ilImageDes             *pDes,
    ilImageFormat          *pFormat
    )
{
register ilImagePtr         pImage;

    pImage = (ilImagePtr)IL_MALLOC (sizeof (ilImageRec));
    if (!pImage)
        return (ilImagePtr)NULL;                            /* EXIT */

    pImage->des = *pDes;
    pImage->format = *pFormat;
    pImage->pStripOffsets = (long *)NULL;   /* freed if non-null */

    pImage->i.pDes = &pImage->des;
    pImage->i.pFormat = &pImage->format;
    pImage->i.width = pInfo->width;
    pImage->i.height = 0;

        /*  Set clientPixels/Palette/CompData true so they are *not* deallocated */
    pImage->i.clientPixels = TRUE;
    pImage->i.clientPalette = TRUE;
    pImage->i.clientCompData = TRUE;
    pImage->i.pPalette = pInfo->pPalette;
    pImage->i.pCompData = pInfo->pCompData;

        /*  Link image into list of temp images, using object pPrev/pNext ptrs.
        */
    pImage->o.pNext = pPipe->imageHead.pNext;
    pImage->o.pPrev = (ilPtr)&pPipe->imageHead;
    ((ilImagePtr)pPipe->imageHead.pNext)->o.pPrev = (ilPtr)pImage;
    pPipe->imageHead.pNext = (ilPtr)pImage;

    return pImage;
}


        /*  ------------------------ ilCleanupRunningPipe ----------------------------- */
        /*  Cleanup a pipe which had been running, i.e. its state was IL_PIPE_EXECUTING.
            "aborting" should be TRUE if the pipe is being aborted (was not finished).
            The pipe state is set to IL_PIPE_COMPLETE.
        */
static ilError ilCleanupRunningPipe (
    register ilPipePtr  pPipe,
    ilBool              aborting
    )
{
register ilElementPtr   pElement, pElementHead;
ilError                 error, prevError;

        /*  Run thru the list of elements and call Cleanup function if non-null, then
            do same for "hook" list.  Pass given aborting flag to Cleanup().
            If an error return, switch to "aborting" mode and return first error returned.
        */
    prevError = IL_OK;
    pElementHead = &pPipe->elementHead;
    while (TRUE) {
        pElement = pElementHead->pNext;
        while (pElement != pElementHead) {
            if (pElement->Cleanup) {
                error = (*pElement->Cleanup) ((ilPtr)pElement->exec.pPrivate, aborting);
                if (error) {
                    if (prevError == IL_OK)
                        prevError = error;
                    aborting = TRUE;                    /* error; now considered aborted */
                    }
                }
            pElement = pElement->pNext;
            }
        if (pElementHead == &pPipe->hookHead)
            break;                                      /* both lists scanned; done */
        pElementHead = &pPipe->hookHead;                /* run thru hook list */
        }

    ilFreeTempImageBuffers (pPipe);
    pPipe->stackIndex = 0;
    pPipe->state = IL_PIPE_COMPLETE;
    return prevError;
}


        /*  ------------------------ ilEmptyPipe -------------------------------- */
        /*  Public function: see spec.
            Also called locally, for example when aborting a pipe.
        */
ilBool ilEmptyPipe (
    ilPipe              pipe
    )
{
register ilPipePtr      pPipe;
register ilElementPtr   pElement, pNextElement, pElementHead;
register ilImagePtr     pImage, pNextImage;
int                     i;
ilError                 error;

    pPipe = (ilPipePtr)pipe;
    if (pPipe->o.p.objectType != IL_PIPE) {
        pPipe->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

        /*  Action to empty a pipe depends on current state.  Transition from running
            (abort) to complete (empty element list) to invalid (mark as empty).
            FALL THRU (no break) from each state.
        */
    error = IL_OK;                  /* assume no error; log error from Cleanup() */
    switch (pPipe->state) {
      case IL_PIPE_EXECUTING:       /* executing: must abort, fall thru to complete */
        error = ilCleanupRunningPipe (pPipe, TRUE);

      case IL_PIPE_COMPLETE:        /* elements but no buffers allocated */
      case IL_PIPE_FORMING:         /* forming is same as complete */

            /*  Run thru list of elements and free them.  Call optional Destroy 
                function if non-null.  Make list head empty.
            */
        pElementHead = &pPipe->elementHead;
        while (TRUE) {
            pElement = pElementHead->pNext;
            while (pElement != pElementHead) {
                if (pElement->Destroy)
                    (*pElement->Destroy) ((ilPtr)pElement->exec.pPrivate);
                pNextElement = pElement->pNext;
                IL_FREE (pElement);
                pElement = pNextElement;
                }
            if (pElementHead == &pPipe->hookHead)
                break;                                      /* both lists scanned; done */
            pElementHead = &pPipe->hookHead;                /* run thru hook list */
            }
        pPipe->elementHead.pNext = pPipe->elementHead.pPrev = &pPipe->elementHead;
        pPipe->hookHead.pNext = pPipe->hookHead.pPrev = &pPipe->hookHead;

            /*  Destroy all temp images associated with this pipe.  The buffers are
                already gone, because they only exist when state is IL_PIPE_EXECUTING,
                and ilCleanupRunningPipe() would be called in that case.
            */
        pImage = (ilImagePtr)pPipe->imageHead.pNext;
        while (pImage != (ilImagePtr)&pPipe->imageHead) {
            pNextImage = (ilImagePtr)pImage->o.pNext;
            IL_FREE (pImage);
            pImage = pNextImage;
            }
        pPipe->imageHead.pNext = pPipe->imageHead.pPrev = (ilPtr)&pPipe->imageHead;

            /*  Destroy all objects in destroyObjects array. */
        for (i = 0; i < pPipe->nDestroyObjects; i++)
            ilDestroyObject (pPipe->destroyObjects[i]);
        pPipe->nDestroyObjects = 0;

      case IL_PIPE_INVALID:         /* pipe already empty; change state */
        pPipe->state = IL_PIPE_EMPTY;

      case IL_PIPE_EMPTY:           /* pipe in desired state */
        break;
        }

    pPipe->o.p.context->error = error;
    return (error == IL_OK);
}



        /*  ------------------------ ilCreatePipe -------------------------------- */
        /*  Public function: see spec.
        */

ilPipe ilCreatePipe (
    ilContext           context,
    unsigned long       mustBeZero
    )
{
register ilPipePtr   pPipe;

    if (mustBeZero != 0) {
        context->error = IL_ERROR_PAR_NOT_ZERO;
        return (ilPipe)NULL;
        }

    pPipe = (ilPipePtr)_ilCreateObject (context, IL_PIPE, ((void (*)())ilEmptyPipe), 
                                       sizeof (ilPipeRec));
    if (!pPipe)
        return (ilPipe)NULL;

        /*  Declare pipe empty, set element and temp image lists to empty state.
            The rest of the pipe state must be set by ilAddPipeElement() on each element.
                Init some of elementHead, which itself is an element, and will be
            referenced as the current element in a pipe with no elements.
        */
    pPipe->state = IL_PIPE_EMPTY;
    pPipe->elementHead.pNext = pPipe->elementHead.pPrev = &pPipe->elementHead;
    pPipe->hookHead.pNext = pPipe->hookHead.pPrev = &pPipe->hookHead;
    pPipe->imageHead.pNext = pPipe->imageHead.pPrev = (ilPtr)&pPipe->imageHead;
    pPipe->elementHead.flags = 0;
    pPipe->stackIndex = 0;
    pPipe->nDestroyObjects = 0;
    pPipe->image.info.stripHeight = 0;

    context->error = IL_OK;
    return (ilPipe)pPipe;
}


        /*  ------------------------ ilDeclarePipeInvalid ----------------------------- */
        /*  Public function; see spec.
            Empties the pipe, sets state to invalid and posts given error.
            FALSE is always returned, to allow the following:
               if (<something wrong while forming pipe>)
                   return ilDeclarePipeInvalid (pipe, errorCode);
        */
ilBool ilDeclarePipeInvalid (
    ilPipe              pipe,
    ilError             error
    )
{
register ilPipePtr   pPipe;

    pPipe = (ilPipePtr)pipe;
    ilEmptyPipe ((ilPipe)pPipe);
    if (pPipe->o.p.objectType == IL_PIPE)
        pPipe->state = IL_PIPE_INVALID;
    pPipe->o.p.context->error = error;
    return FALSE;
}

        
        /*  ------------------------ ilQueryPipe  ----------------------------- */
        /*  Public function; see spec.
            Return description of current image, from pipe header, if pipe forming.
        */
unsigned int ilQueryPipe (
    ilPipe              pipe,
    long               *pWidth,                 /* RETURNED */
    long               *pHeight,                /* RETURNED */
    ilImageDes         *pDes                    /* RETURNED */
    )
{
register ilPipePtr   pPipe;

    pPipe = (ilPipePtr)pipe;
    if (pPipe->o.p.objectType != IL_PIPE) {
        pPipe->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return IL_PIPE_INVALID;                 /* return phony state because of error */
        }

    if (pPipe->state == IL_PIPE_FORMING) {
        if (pWidth)
            *pWidth = pPipe->image.info.width;
        if (pHeight)
            *pHeight = pPipe->image.info.height;
        if (pDes)
            *pDes = pPipe->image.des;
        }

    pPipe->o.p.context->error = IL_OK;
    return pPipe->state;
}


        /*  ------------------------ ilGetPipeInfo ----------------------------- */
        /*  Public function; see spec.
            If pipe state is "forming": if "forceDecompress" and image is compressed, make 
            it uncompressed, then return description of current pipe state.
        */
unsigned int ilGetPipeInfo (
    ilPipe              pipe,
    ilBool              forceDecompress,
    ilPipeInfo         *pInfo,
    ilImageDes         *pDes,
    ilImageFormat      *pFormat
    )
{
register ilPipePtr   pPipe;

    pPipe = (ilPipePtr)pipe;
    if (pPipe->o.p.objectType != IL_PIPE) {
        pPipe->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return IL_PIPE_INVALID;                             /* EXIT w/ pseudo-error */
        }
    if (pPipe->state == IL_PIPE_FORMING) {

            /*  Decompress image if it is compressed and caller asked for decompressed.
            */
        if (forceDecompress && (pPipe->image.des.compression != IL_UNCOMPRESSED)) {
            _ilDecompress ((ilPipe)pPipe);
            if (pPipe->o.p.context->error)
                return IL_PIPE_INVALID;                     /* EXIT w/ pseudo-error */
            }

        if (pInfo)
            *pInfo = pPipe->image.info;
        if (pDes)
            *pDes = pPipe->image.des;
        if (pFormat)
            *pFormat = pPipe->image.format;
        }

    pPipe->o.p.context->error = IL_OK;
    return pPipe->state;
}


        /*  ----------------------- ilRecommendedStripHeight ------------------------ */
        /*  Public function; see spec.
        */
long ilRecommendedStripHeight (
    const ilImageDes         *pDes,
    const ilImageFormat      *pFormat,
    long                width,
    long                height
    )
{
long                    bytesPerRow [IL_MAX_SAMPLES];
register long           stripHeight, nBytes;
int                     i;

        /*  Get the bytes/row, per plane.  If pixel format, all bytes are in plane 0,
            else in pDes->nSamplesPerPixel planes.
        */
    if (height <= 0)
        stripHeight = 0;
    else {
        ilGetBytesPerRow (pDes, pFormat, width, bytesPerRow);
        if (pFormat->sampleOrder == IL_SAMPLE_PIXELS)
            nBytes = bytesPerRow[0];            /* all bytes in one plane */
        else {
            nBytes = 0;
            for (i = 0; i < pDes->nSamplesPerPixel; i++)
                nBytes += bytesPerRow[i];
            }
        if (nBytes <= 0)
            stripHeight = 1;
        else {
            stripHeight = ilDefaultStripSize / nBytes;
            if (stripHeight > height)
                stripHeight = height;
            if (stripHeight <= 0)
                stripHeight = 1;
            }
        }
    return stripHeight;
}


        /*  ----------------------- ilChangeStripHeight ------------------------ */
        /*  Calcs the recommended strip height, in ilPipeInfo for the pipe.
            Should be called whenever the pipe stripHeight is changed, after the
            pipe height is changed (limits stripHeight to image height).
        */
static void ilChangeStripHeight (
    register ilPipePtr  pPipe
    )
{
register long           stripHeight;

        /*  Limit stripHeight to pipe height. */
    stripHeight = pPipe->image.info.stripHeight;
    if (stripHeight > pPipe->image.info.height) 
        pPipe->image.info.stripHeight = stripHeight = pPipe->image.info.height;
    else if (stripHeight <= 0)
        pPipe->image.info.stripHeight = stripHeight = 1;

        /*  If uncompressed image, calc recommendedStripHeight else current stripHeight */
    if (pPipe->image.des.compression == IL_UNCOMPRESSED) {
        register long   i;
        i = ilRecommendedStripHeight (&pPipe->image.des, &pPipe->image.format, 
                                      pPipe->image.info.width, pPipe->image.info.height);
        pPipe->image.info.recommendedStripHeight = (i > stripHeight) ? stripHeight : i;
        }
    else pPipe->image.info.recommendedStripHeight = stripHeight;
}


        /*  ------------------------ ilAddPipeDestroyObject ------------------------ */
        /*  IL internal function, declared in /ilc/ilpipeint.h .
            Add the given object to a list of objects to be destroyed when this pipe
            is emptied.  The object's refCount should be inc'd before this call, so that
            the object cannot be freed until this pipe is destroyed.
            The pipe must be in the forming or complete state.
            NOTE: a hard limit exists on the # of objects which can be added; not intended
            for unlimited use, e.g. by filters!
            Sets context->error to error code, returns true if success.
        */
IL_PRIVATE ilBool _ilAddPipeDestroyObject (
    ilPipe              pipe,
    ilObject            object
    )
{
register ilPipePtr      pPipe;

    pPipe = (ilPipePtr)pipe;
    if (pPipe->nDestroyObjects >= IL_MAX_DESTROY_OBJECTS) {
        pipe->context->error = IL_ERROR_MALLOC;     /* should not happen, return phony */
        return FALSE;
        }
    pPipe->destroyObjects[pPipe->nDestroyObjects] = object;
    pPipe->nDestroyObjects++;
    pipe->context->error = IL_OK;
    return TRUE;
}


        /*  ---------------------------- ilAddProducerImage ------------------------ */
        /*  IL internal function, declared in /ilc/ilpipeint.h .
            Called by ilReadImage() to set *pImage as the producer to this pipe. 
            The pipe MUST be in the empty state.  "needProducerThrottle" is true iff a 
            throttle must be inserted before next element added by ilAddPipeElement().
        */
IL_PRIVATE ilBool _ilAddProducerImage (
    ilPipe              pipe,
    register ilImagePtr pImage,
    unsigned int        producerCode,
    long                height,
    long                stripHeight,
    ilBool              constantStrip,
    ilBool              needProducerThrottle
    )
{
register ilPipePtr      pPipe;

        /*  Copy image data into pipe info, point to given image as producer. */
    pPipe = (ilPipePtr)pipe;
    pPipe->producerCode = producerCode;
    pPipe->image.des = pImage->des;
    pPipe->image.format = pImage->format;
    pPipe->image.info.producerObject = (ilObject)pImage;
    pPipe->image.info.tempImage = FALSE;
    pPipe->image.info.width = pImage->i.width;
    pPipe->image.info.height = height;
    pPipe->image.info.constantStrip = constantStrip;
    pPipe->image.info.stripHeight = stripHeight;
    ilChangeStripHeight (pPipe);

    pPipe->image.info.producerObject = (ilObject)pImage;
    pPipe->image.info.pPalette = pImage->i.pPalette;        /* point to image palette */
    pPipe->image.info.pCompData = pImage->i.pCompData;      /* point to comp data */
    pPipe->pSrcPipeImage = &pImage->i;
    pPipe->state = IL_PIPE_FORMING;
    pPipe->feedDone = FALSE;            /* ilSetFeedPipeData not done yet. */

        /*  Signal ilAddPipeElement() to add a throttle before next filter is added,
            and that a copy filter must be added if ilWriteImage() is called next.
        */
    pPipe->needProducerThrottle = needProducerThrottle;
    pPipe->copyToConsumerImage = TRUE;

        /*  Inc pImage->refCount, and add as destroyObject for when pipe destroyed. */
    pImage->o.refCount++;
    return _ilAddPipeDestroyObject ((ilPipe)pPipe, (ilObject)pImage);
}


        /*  ------------------------ ilSetFeedPipeData ------------------------------- */
        /*  Called by ilFeedPipe() to set info for the first element in the given pipe,
            which will be ilFeedProducerThrottleExecute().  If feeding compressed data:
            "start" is the byte offset into the compressed data, and "nCompBytes" is
            the number of bytes at that offset.  If uncompressed data: "start" is the
            starting line, and "nCompBytes" is ignored.  Returns FALSE if the pipe was 
            not started with a ilFeedFromImage() producer.
        */
IL_PRIVATE ilBool _ilSetFeedPipeData (
    ilPipe              pipe,
    long                start,
    long                nLines,
    long                nCompBytes
    )
{
register ilPipePtr      pPipe;
register ilImagePtr     pImage;

    pPipe = (ilPipePtr)pipe;
    if (pPipe->producerCode != IL_PIPE_FEED_IMAGE)
        return FALSE;

        /*  If uncompressed image, starting line plus # lines must be <= height, and
            "start" is starting line; if compressed "start" is starting byte offset.
        */
    pImage = (ilImagePtr)pPipe->image.info.producerObject;
    if (pImage->des.compression == IL_UNCOMPRESSED) {
        if ((start + nLines) > pImage->i.height)
            return FALSE;
        pPipe->feedStartLine = start;
        pPipe->feedCompNBytes = 0;
        pPipe->feedCompOffset = 0;
        }
    else {
        pPipe->feedStartLine = 0;
        pPipe->feedCompOffset = start;
        pPipe->feedCompNBytes = nCompBytes;
        }
    pPipe->feedNLines = nLines;
    pPipe->feedDone = TRUE;                     /* pipe now fed */
    return TRUE;
}


        /*  ------------------------ ilSetPipeDesFormat ------------------------------- */
        /*  IL internal function, declared in /ilc/ilpipeint.h .
            Called by ilConvert() to "typecast" the pipe image to the given des and/or
            format (ignored if pDes/pFormat null).  No validation is done on the result!
        */
IL_PRIVATE void _ilSetPipeDesFormat (
    ilPipe              pipe,
    ilImageDes         *pDes,
    ilImageFormat      *pFormat
    )
{
register ilPipePtr      pPipe;

    pPipe = (ilPipePtr)pipe;
    if (pDes)
        pPipe->image.des = *pDes;
    if (pFormat)
        pPipe->image.format = *pFormat;
}


        /*  ------------------------ ilAddPipeElement ------------------------------- */
        /*  Public function: see spec.
            Add a pipe element if correct state.
        */
ilPtr ilAddPipeElement (
    ilPipe              pipe,
    int                 elementType,
/* Use portable type for sizeof() operator, bug report OSF_QAR# 32082 */
    size_t              nBytesPrivate,
    unsigned long       flags,
    ilSrcElementData   *pSrcData,
    ilDstElementData   *pDstData,
    ilError           (*Init)(),
    ilError           (*Cleanup)(),
    ilError           (*Destroy)(),
/*
** Added another execute function for passing in a fourth
** parameter which is a floating pointing.
*/
    ilError           (*ExecuteThree)(),
    ilError           (*ExecuteFour)(register ilExecuteData *,
					      long,
					      long *,
					      float),
    unsigned long       mustBeZero
    )
{
register ilPipePtr      pPipe;
unsigned int            newState;           /* pipe state after this element added */
register ilElementPtr   pElement;           /* ptr to new element to add to list */
ilElementPtr            pPrevElement;
ilError                 error;
long                    stripHeight, srcBufferHeight;
ilBool                  constantStrip, haveConsumerImage, insertCopyFilter;

    stripHeight = 0; /* initializing the stripHeight variable */
    pPipe = (ilPipePtr)pipe;
    if (pPipe->o.p.objectType != IL_PIPE) {
        pPipe->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return (ilPtr)NULL;                                 /* EXIT */
        }
    if (mustBeZero != 0) {
        ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_PAR_NOT_ZERO);
        return (ilPtr)NULL;
        }

        /*  Validate elementType and pipe state: "empty" for producer, else "forming".
            Set newState to state pipe becomes after this element is added.
            If a "hook" element, add hook element and EXIT.
                For filters and consumers: set stripHeight to source (input) strip height
            required by this element; set constantStrip true if every strip (except last)
            must be stripHeight lines high.  Default if !pSrcData or stripHeight == 0:
            !constantStrip; filters: recommended strip height; consumers: whole strip.
                Set some flags, explained when used below.
        */
    haveConsumerImage = FALSE;
    insertCopyFilter = FALSE;

    switch (elementType) {

        case IL_PRODUCER:
            if (pPipe->state != IL_PIPE_EMPTY) {
                ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_PIPE_STATE);
                return (ilPtr)NULL;
                }
            newState = IL_PIPE_FORMING;
            break;

        case IL_FILTER:
            if (pPipe->state != IL_PIPE_FORMING) {
                ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_PIPE_STATE);
                return (ilPtr)NULL;
                }
            newState = IL_PIPE_FORMING;
            if (pSrcData && (pSrcData->minBufferHeight > 0))
                insertCopyFilter = pPipe->copyToConsumerImage;
            if (!pSrcData || !pSrcData->stripHeight) {
                stripHeight = pPipe->image.info.recommendedStripHeight;
                constantStrip = FALSE;
                }
            else  {
                stripHeight = pSrcData->stripHeight;
                constantStrip = pSrcData->constantStrip;
                }
            break;

        case IL_CONSUMER:
            if (pPipe->state != IL_PIPE_FORMING) {
                ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_PIPE_STATE);
                return (ilPtr)NULL;
                }
            newState = IL_PIPE_COMPLETE;
            if (pSrcData) {
                if (pSrcData->consumerImage) {
                    haveConsumerImage = TRUE;
                    insertCopyFilter = pPipe->copyToConsumerImage;
                    }
                else if (pSrcData->minBufferHeight > 0)
                    insertCopyFilter = pPipe->copyToConsumerImage;
                }
            if (!pSrcData || !pSrcData->stripHeight) {
                stripHeight = pPipe->image.info.stripHeight;  /* accept strip height */
                constantStrip = FALSE;
                }
            else  {
                stripHeight = pSrcData->stripHeight;
                constantStrip = pSrcData->constantStrip;
                }
            break;

            /*  A "hook" element: invalid if pipe executing or invalid, else add element
                to hook list with Init/Cleanup/Destroy functions copied.
            */
        case IL_HOOK:
            if ((pPipe->state == IL_PIPE_INVALID) || (pPipe->state == IL_PIPE_EXECUTING)) {
                ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_PIPE_STATE);
                return (ilPtr)NULL;
                }
            if (ExecuteThree || ExecuteFour) {
                ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_PAR_NOT_ZERO);
                return (ilPtr)NULL;
                }
            pElement = (ilElementPtr)IL_MALLOC_ZERO (sizeof(ilElementRec) + nBytesPrivate);
            if (!pElement) {
                ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_MALLOC);
                return (ilPtr)NULL;                                 /* EXIT */
                }
            pElement->pPrev = pPipe->hookHead.pPrev;
            pElement->pNext = &pPipe->hookHead;
            pPipe->hookHead.pPrev->pNext = pElement;
            pPipe->hookHead.pPrev = pElement;
            pElement->elementType = IL_HOOK;
            pElement->exec.pPrivate = (ilPtr)&pElement->clientPrivate;
            pElement->exec.pSrcImage = pElement->exec.pDstImage = (ilImageInfo *)NULL;
            pElement->Init = Init;
            pElement->Cleanup = Cleanup;
            pElement->Destroy = Destroy;

            pPipe->o.p.context->error = IL_OK;
            return (ilPtr)pElement->exec.pPrivate;                  /* done; EXIT */
            break;

        default:
            ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_ELEMENT_TYPE);
            return (ilPtr)NULL;
            break;
        }   /* END switch elementType */


        /*  Force stripHeight to be in range 1..image height */
    if (stripHeight <= 0)
        stripHeight = 1;
    else if (stripHeight > pPipe->image.info.height)
        stripHeight = pPipe->image.info.height;

        /*  If pipe begins with an ilReadImage() and this is first filter, add
            a throttle to entire strip height - this throttle knows to read whole strip
            and return IL_ERROR_LAST_STRIP - otherwise ilExecutePipe() would not stop.
                Note: ilAddThrottlePipeElement() calls this function: recursion!
            Also, note that no pipe state has been changed up to this point. BUT,
            set needProducerThrottle to false or below check causes infinite recursion.
        */
    if ((elementType != IL_PRODUCER) && pPipe->needProducerThrottle) {
        pPipe->needProducerThrottle = FALSE;        /* prevent recursion */
        if (!_ilAddThrottlePipeElement ((ilPipe)pPipe, stripHeight, 
                constantStrip, pPipe->producerCode, 
                &pPipe->image.info.stripHeight, &pPipe->image.info.constantStrip)) {
            ilDeclarePipeInvalid ((ilPipe)pPipe, pPipe->o.p.context->error);
            return (ilPtr)NULL;
            }
        }

        /*  If writing consumerImage (e.g. ilWriteImage()) or inserting a throttle
            requiring minBufferHeight, insert copy filter if flagged: e.g. previous 
            element was "no dst" filter or ilReadImage(); else possibly add throttle:
                If not a producer and pipe image is not compressed, check "stripHeight", 
            the desired strip height, set above.
                If != current strip height, or caller requires a constant strip
            and current is not constant, add a filter which makes it so.
                If throttle added successfully, set image info to what was asked for, but
            first save stripHeight - it is the size of the src buffer to create, if any.
        */
    srcBufferHeight = pPipe->image.info.stripHeight;
    if (insertCopyFilter) {
        if (pPipe->image.des.compression == IL_UNCOMPRESSED) {
            if (!_ilInsertCopyFilter ((ilPipe)pPipe, pPipe->image.info.stripHeight))
                return (ilPtr)NULL;
            }
        else if (!_ilInsertCompressedCopyFilter ((ilPipe)pPipe))
            return (ilPtr)NULL;
        }
    else if ((elementType != IL_PRODUCER)
     && (pPipe->image.des.compression == IL_UNCOMPRESSED)
     && ((stripHeight != pPipe->image.info.stripHeight)
      || (constantStrip && !pPipe->image.info.constantStrip)) ) {
            if (!_ilAddThrottlePipeElement ((ilPipe)pPipe, stripHeight, 
                constantStrip, IL_PIPE_NOT_IMAGE,
                &pPipe->image.info.stripHeight, &pPipe->image.info.constantStrip)) {
                    ilDeclarePipeInvalid ((ilPipe)pPipe, pPipe->o.p.context->error);
                    return (ilPtr)NULL;
                    }
            }

        /*  Allocate and zero element struct and add to end of list; exit if error. */
    pElement = (ilElementPtr)IL_MALLOC_ZERO (sizeof(ilElementRec) + nBytesPrivate);
    if (!pElement) {
        ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_MALLOC);
        return (ilPtr)NULL;                                 /* EXIT */
        }
    pElement->pPrev = pPipe->elementHead.pPrev;
    pElement->pNext = &pPipe->elementHead;
    pPipe->elementHead.pPrev->pNext = pElement;
    pPipe->elementHead.pPrev = pElement;

        /*  Copy passed data into the element struct and init element.
            Each element's pNextSrcLine points to the next one's srcLine, and compressed.
            pOffset/pNBytesWritten point to next one's srcOffset/nBytesToRead.
                Set "Destroy" to null for now, so it will not be called if we fail here.
            Set it to passed function only when sure of success.
        */
    pElement->elementType = elementType;
    pElement->flags = flags;
    pElement->exec.pPrivate = (ilPtr)&pElement->clientPrivate;
    pPrevElement = pElement->pPrev;
    pPrevElement->exec.pNextSrcLine = &pElement->exec.srcLine;
    pPrevElement->exec.compressed.pDstOffset = &pElement->exec.compressed.srcOffset;
    pPrevElement->exec.compressed.pNBytesWritten = &pElement->exec.compressed.nBytesToRead;
    pElement->pNext->exec.srcLine = 0;
    pElement->Init = Init;
    pElement->Cleanup = Cleanup;
    pElement->Destroy = IL_NPF;         /* set when successful */
    pElement->ExecuteThree = ExecuteThree;
    pElement->ExecuteFour = ExecuteFour;

        /*  If a producer, must have valid dst data and pDes/Format, or error.
            Skip allocation of src image for producers - they get src on their own;
            set pSrcPipeImage to NULL.  Init # src lines to read to strip height.
                Set ilPipeInfo.pPalette to callers if palette image, else to null
        */
    if (elementType == IL_PRODUCER) {
        if (!pDstData || !pDstData->pDes || !pDstData->pFormat) {
            ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_PRODUCER_DATA);
            return (ilPtr)NULL;                             /* EXIT */
            }
        if ((pDstData->width <= 0) || (pDstData->height <= 0)) {
            ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_ZERO_SIZE_IMAGE);
            return (ilPtr)NULL;                             /* EXIT */
            }
        error = _ilValidateDesFormat (TRUE, pDstData->pDes, pDstData->pFormat);
        if (error) {
            ilDeclarePipeInvalid ((ilPipe)pPipe, error);
            return (ilPtr)NULL;                            /* EXIT */
            }

        pPipe->producerCode = IL_PIPE_NOT_IMAGE;    /* not a read from a (feed) image */
        pPipe->pSrcPipeImage = (ilImageInfo *)NULL;

        pPipe->image.des = *pDstData->pDes;
        pPipe->image.format = *pDstData->pFormat;
        pPipe->image.info.producerObject = pDstData->producerObject;
        pPipe->image.info.tempImage = TRUE;

        pPipe->image.info.width = pDstData->width;
        pPipe->image.info.height = pDstData->height;
        pPipe->image.info.constantStrip = pDstData->constantStrip;
        pPipe->image.info.stripHeight = pDstData->stripHeight;
        ilChangeStripHeight (pPipe);

        pPipe->image.info.pPalette = (pPipe->image.des.type == IL_PALETTE) ? 
                                           pDstData->pPalette : (unsigned short *)NULL;
        pPipe->image.info.pCompData = pDstData->pCompData;
        pPipe->needProducerThrottle = FALSE;        /* producer not an image */
        pPipe->copyToConsumerImage = FALSE;

        pElement->exec.pSrcImage = (ilImageInfo *)NULL;
        }
    else  {
            /*  Not a producer.  Create src image to this element, unless a consumer which
                specifies its source image (e.g. ilWriteImage()), or src image specified
                as output from last element = a producer (e.g. ilReadImage()): pSrcImage.
            */
        if (pPipe->pSrcPipeImage) {
            pElement->exec.pSrcImage = pPipe->pSrcPipeImage;
            pPipe->pSrcPipeImage = (ilImageInfo *)NULL;
            }
        else if (haveConsumerImage) {
            pElement->exec.pSrcImage = &((ilImagePtr)pSrcData->consumerImage)->i;
            }
        else {
                /*  Src image not provided; must create a temp and add to list of temps.
                    Set image height = max (minHeight if present, srcBufferHeight).
                */
            ilImagePtr      pImage;
            pImage = ilAllocTempImage (pPipe, &pPipe->image.info, &pPipe->image.des, 
                                       &pPipe->image.format);
            if (!pImage) {
                ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_MALLOC);
                return (ilPtr)NULL;                         /* EXIT */
                }
            pElement->exec.pSrcImage = &pImage->i;
            if (pSrcData && (pSrcData->minBufferHeight > srcBufferHeight))
                srcBufferHeight = pSrcData->minBufferHeight;
            pImage->i.height = srcBufferHeight;
            }

            /*  Src image to this element is dst image of previous element.
                If no dst from this element, then src to this element is src to next,
                and must copy if next element is ilWriteImage(); otherwise, src to next 
                is necessarily a temp image, and don't need to copy.
            */
        pElement->pPrev->exec.pDstImage = pElement->exec.pSrcImage;
        if (flags & IL_ADD_PIPE_NO_DST) {
            pPipe->pSrcPipeImage = pElement->exec.pSrcImage;
            pPipe->copyToConsumerImage = TRUE;
            }
        else {
            pPipe->image.info.tempImage = TRUE;
            pPipe->copyToConsumerImage = FALSE;
            }

            /*  If not a consumer and have pDstData: if non-null des/format ptr, copy
                data from it into the pipe image area - element changes des or format.
                Set rest of data from pDstData: stripHeight if not zero.
                Set ilPipeInfo.pPalette to null if not a palette image, or to callers
                    value if it is not null.
                Validate pDes and pFormat together if either non-null (else no change).
            */
        if (pDstData && (elementType != IL_CONSUMER)) {
            const ilImageDes      *pDes;
            if ((pDstData->width <= 0) || (pDstData->height <= 0)) {
                ilDeclarePipeInvalid ((ilPipe)pPipe, IL_ERROR_ZERO_SIZE_IMAGE);
                return (ilPtr)NULL;                         /* EXIT */
                }
            pPipe->image.info.width = pDstData->width;
            pPipe->image.info.height = pDstData->height;
            if (pDstData->stripHeight != 0) {
                pPipe->image.info.constantStrip = pDstData->constantStrip;
                pPipe->image.info.stripHeight = pDstData->stripHeight;
                }
            if ((pDes = pDstData->pDes)) {
                if (pDes->type == IL_PALETTE) {
                    if (pDstData->pPalette)
                        pPipe->image.info.pPalette = pDstData->pPalette;
                    }
                else pPipe->image.info.pPalette = (unsigned short *)NULL;
                pPipe->image.des = *pDes;
                }
            pPipe->image.info.pCompData = pDstData->pCompData;
            if (pDstData->pFormat)
                pPipe->image.format = *pDstData->pFormat;
            if (pDes || pDstData->pFormat) {
                error = _ilValidateDesFormat (TRUE /* allow private types */, 
                                             &pPipe->image.des, &pPipe->image.format);
                if (error) {
                    ilDeclarePipeInvalid ((ilPipe)pPipe, error);
                    return (ilPtr)NULL;                            /* EXIT */
                    }
                }
            ilChangeStripHeight (pPipe);        /* reset (recommended) strip height */
            }   /* END have pDstData, not consumer */
        }       /* END not a producer */

        /*  Success: set pipe to new state; set pElement->Destroy, return pPriv. */
    pPipe->state = newState;
    pElement->Destroy = Destroy;

    pPipe->o.p.context->error = IL_OK;
    return (ilPtr)pElement->exec.pPrivate;
}


        /*  ------------------------ ilAbortPipe -------------------------------- */
        /*  Public function: see spec.
            Aborts the given pipe; if not running, a noop.
        */
ilBool ilAbortPipe (
    ilPipe              pipe
    )
{
register ilPipePtr      pPipe;

    pPipe = (ilPipePtr)pipe;
    if (pPipe->o.p.objectType != IL_PIPE) {
        pPipe->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

    if (pPipe->state == IL_PIPE_EXECUTING)
        return ((pPipe->o.p.context->error = ilCleanupRunningPipe (pPipe, TRUE)) == IL_OK);
    else {
        pPipe->o.p.context->error = IL_OK;
        return TRUE;
        }
}

        /*  ------------------------ ilExecutePipe -------------------------------- */
        /*  Public function: see spec.
        */
int ilExecutePipe (
    ilPipe              pipe,
    long                nStrips,
    float               ratio
    )
{
register ilPipePtr      pPipe;
register ilElementPtr   pElement, pExecHead, pElementHead;
ilImagePtr              pImage;
long                    nLines, initNLines;
ilError                 error;

        /*  Validate zero par and that pipe is one.
        */
    pPipe = (ilPipePtr)pipe;
    if (pPipe->o.p.objectType != IL_PIPE) {
        pPipe->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return IL_EXECUTE_ERROR;                            /* EXIT */
        }

        /*  Execute based on pipe state.  Error if pipe invalid, empty or forming.
            Otherwise: abort if requested, cleanup if in progress.  Compile pipe
            if complete, or executable and producer changed.  If in progress and
            producer changed, cleanup and return error.
        */
    pExecHead = &pPipe->elementHead;            /* head of "executable" (non-hook) list */
    switch (pPipe->state) {
      case IL_PIPE_INVALID:
      case IL_PIPE_EMPTY:
      case IL_PIPE_FORMING:
        pPipe->o.p.context->error = IL_ERROR_PIPE_STATE;
        return IL_EXECUTE_ERROR;                            /* EXIT */

            /*  Pipe complete: do setup to become executable.
                Allocate pixel buffers for all temp images of this pipe.
                If error, don't cleanup, as init not done yet.
            */
      case IL_PIPE_COMPLETE:
        pImage = (ilImagePtr)pPipe->imageHead.pNext;
        while (pImage != (ilImagePtr)&pPipe->imageHead) {
            ilError error = _ilMallocImagePixels (pImage);
            if (error) {
                ilFreeTempImageBuffers (pPipe);
                pPipe->o.p.context->error = error;
                return IL_EXECUTE_ERROR;                    /* EXIT */
                }
            pImage = (ilImagePtr)pImage->o.pNext;
            }

            /*  Call elements' Init() functions if non-null. If Init() returns error:
                call Cleanup() of previous elements, whose Init()s were already called,
                and free image buffers allocated above, and return error in errorInfo.
                Init all srcLines and srcOffsets to 0; always start at line/byte 0.
                    Do above first for "hook" list, then for regular element list.
            */
        pElementHead = &pPipe->hookHead;
        while (TRUE) {
            pElement = pElementHead->pNext;
            while (pElement != pElementHead) {
                pElement->exec.srcLine = 0;
                pElement->exec.compressed.srcOffset = 0;
                if (pElement->Init) {
                    ilError error = (*pElement->Init) ((ilPtr)pElement->exec.pPrivate, 
                                     pElement->exec.pSrcImage, pElement->exec.pDstImage);
                    if (error) {
                        while (TRUE) {
                            while (pElement->pPrev != pElementHead) {
                                pElement = pElement->pPrev;
                                if (pElement->Cleanup)
                                    (*pElement->Cleanup) (pElement->exec.pPrivate, TRUE);
                                }
                            if (pElementHead == &pPipe->hookHead)
                                break;                          /* both lists done */
                            pElementHead = &pPipe->hookHead;
                            pElement = pElementHead;
                            }
                        ilFreeTempImageBuffers (pPipe);
                        pPipe->o.p.context->errorInfo = error;
                        pPipe->o.p.context->error = (error < 0) ? 
                                        IL_ERROR_USER_PIPE_ELEMENT : IL_ERROR_PIPE_ELEMENT;
                        return IL_EXECUTE_ERROR;                /* EXIT */
                        }
                    }
                pElement = pElement->pNext;
                }   /* END while, one element list */
            if (pElementHead == pExecHead)
                break;                                      /* both lists scanned; done */
            pElementHead = pExecHead;                       /* run thru executable list */
            }       /* END while TRUE, all element lists */

            /*  Now executing; FALL THRU to execute one pass.  Point top of execStack to
                first element; it changes to top of execStack on a push.
            */
        pPipe->stackIndex = 0;
        pPipe->execStack[0] = pExecHead->pNext;
        pPipe->state = IL_PIPE_EXECUTING;
        pPipe->lastStrip = FALSE;

            /*  Pipe being executed, possibly first time. Execute nStrips strips (or all
                if !nStrips). dstLine for each element is "srcLine" of next element.
                Start with top element of "execStack".
                    "nLines" on entry to Execute() = # of src lines to read; 
                on exit = # of dst lines written (== 0, skip rest of pipe).
                If error: check for pseudo-error, else abort.  Pseudo-errors:
                    IL_ERROR_LAST_STRIP: finish processing this strip, then done
                    IL_ERROR_ELEMENT_AGAIN: element states that it needs to be recalled
                        (e.g. a throttle with more left in its input buffer). Push
                        pElement onto execStack; exit if overflow.
                    IL_ERROR_ELEMENT_COMPLETE: element states that it previously issued
                        "AGAIN" and is now done.  Pop stack; exit if underflow.
            */
      case IL_PIPE_EXECUTING:
        if (pPipe->producerCode == IL_PIPE_FEED_IMAGE) {
            ilElementPtr        pFirstElement;

            if (!pPipe->feedDone) {         /* pipe executed directly instead of fed */
                ilCleanupRunningPipe (pPipe, TRUE);
                pPipe->o.p.context->error = IL_ERROR_PIPE_NOT_FED;
                return IL_EXECUTE_ERROR;
                }
            pPipe->feedDone = FALSE;
            initNLines = pPipe->feedNLines;
            pFirstElement = pExecHead->pNext;
            pFirstElement->exec.srcLine = pPipe->feedStartLine;
            pFirstElement->exec.compressed.srcOffset = pPipe->feedCompOffset;
            pFirstElement->exec.compressed.nBytesToRead = pPipe->feedCompNBytes;
            }
        else initNLines = pPipe->image.info.height;  /* first element reads whole image */

        while (TRUE) {                          /* until all or nStrips done */
            nLines = initNLines;
            pElement = pPipe->execStack [pPipe->stackIndex];
            while (pElement != pExecHead) {
/*
** Added another execute function for passing in a fourth
** parameter which is a floating pointing.  So check to see
** which execute function is not null and call that one.
*/
		if(pElement->ExecuteThree)
		    {
		    error = (*pElement->ExecuteThree) (&pElement->exec,                                      pElement->pNext->exec.srcLine, 
				      &nLines);
		    }
		else
		    {
		    error = (*pElement->ExecuteFour) (&pElement->exec, 
                                               pElement->pNext->exec.srcLine, 
					       &nLines, ratio);
		    }
                if (error) {
                    if (error == IL_ERROR_ELEMENT_AGAIN) {
                        if (++pPipe->stackIndex >= IL_MAX_EXEC_STACK) {
                            ilCleanupRunningPipe (pPipe, TRUE);
                            pPipe->o.p.context->error = IL_ERROR_EXECUTE_STACK_OVERFLOW;
                            return IL_EXECUTE_ERROR;            /* EXIT */
                            }
                        pPipe->execStack [pPipe->stackIndex] = pElement;
                        }
                    else if (error == IL_ERROR_ELEMENT_COMPLETE) {
                        if (--pPipe->stackIndex < 0) {
                            ilCleanupRunningPipe (pPipe, TRUE);
                            pPipe->o.p.context->error = IL_ERROR_EXECUTE_STACK_UNDERFLOW;
                            return IL_EXECUTE_ERROR;            /* EXIT */
                            }
                        }
                    else if (error == IL_ERROR_LAST_STRIP)
                        pPipe->lastStrip = TRUE;
                    else {
                        ilCleanupRunningPipe (pPipe, TRUE);
                        pPipe->o.p.context->errorInfo = error;
                        pPipe->o.p.context->error = (error < 0) ? 
                                    IL_ERROR_USER_PIPE_ELEMENT : IL_ERROR_PIPE_ELEMENT;
                        return IL_EXECUTE_ERROR;                /* EXIT */
                        }
                    }
                if (nLines <= 0)                /* no lines written; skip rest of pipe */
                    break;
                pElement = pElement->pNext;
                }

                    /*  If stack is empty (if not empty, still pushing the current strip
                        thru the pipe): if last strip, do cleanup and exit, else downcount
                        nStrips (if != 0 => all strips) and return if 0.
                    */
            if (pPipe->stackIndex <= 0) {
                if (pPipe->lastStrip) {
                    ilError error = ilCleanupRunningPipe (pPipe, FALSE);
                    if (error == IL_OK) 
                        return IL_EXECUTE_COMPLETE;
                    else {
                        pPipe->o.p.context->errorInfo = error;
                        pPipe->o.p.context->error = (error < 0) ? 
                                    IL_ERROR_USER_PIPE_ELEMENT : IL_ERROR_PIPE_ELEMENT;
                        return IL_EXECUTE_ERROR;
                        }
                    }
                if ((nStrips != 0) && (--nStrips <= 0))
                    return IL_EXECUTE_AGAIN;
                }

            }   /* END while true: execute strips */
        }       /* END switch pipe state */
}


