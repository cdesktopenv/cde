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
/* $XConsortium: ilwriteimage.c /main/4 1996/01/08 12:17:24 lehors $ */
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
#include "ilimage.h"
#include "ilerrors.h"
#include "ilutiljpeg.h"


        /*  Private data for Read/WriteImage pipe functions. */
typedef struct {
    ilImagePtr          pImage;         /* ptr to image read/written */
    unsigned short     *pPalette;       /* ptr to palette or null if not palette image */
    ilPtr               pCompData;      /* ptr to comp data or null if none */
    long                nLinesWritten;  /* Init(): # of lines written so far */

        /*  Remaining fields used only for compressed images  */
    long                nStripsWritten; /* Init(): # of strips written to image */
    } ilImagePrivRec, *ilImagePrivPtr;


    /*  ------------------------- ilWriteImageInit ----------------------------------- */
    /*  Init() function for ilWriteImage().
    */
static ilError ilWriteImageInit (
    ilImagePrivPtr      pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
    pPriv->nStripsWritten = 0;
    pPriv->nLinesWritten = 0;
    return IL_OK;
}


    /*  ------------------------ ilWriteImageDestroy ------------------------------ */
    /*  Destroy() function for ilWriteImage().  Calls ilDestroyObject() with
        element's pObject, which should point to the image.  The image's refCount
        is inc'd when pipe element added.  ilDestroyObject() will dec it and free
        the image if the refCount is 0.  Using refCount this way prevents the user
        from destroying an image still attached to a pipe.
    */
static ilError ilWriteImageDestroy (
    ilImagePrivPtr      pPriv
    )
{
    ilDestroyObject ((ilObject)pPriv->pImage);
    return IL_OK;
}


/*  ============================ WRITE COMPRESSED CODE ============================= */

    /*  ------------------------ ilWriteCompressedCleanup ------------------------- */
    /*  Cleanup() function for ilWriteImage(), writing compressed images.
    */
static ilError ilWriteCompressedCleanup (
    ilImagePrivPtr      pPriv,
    ilBool              aborting
    )
{
ilImagePlaneInfo *pImagePlane;
ilImagePtr     pImage;
ilError                 error;

    pImage = pPriv->pImage;
    pImagePlane = &pImage->i.plane[0];

        /*  If # of strips written by pipe is not equal to # of strips in image, error */
    if (!aborting && (pPriv->nStripsWritten != pImage->nStrips)) {
        aborting = TRUE;                        /* take error path below */
        error = IL_ERROR_MALFORMED_IMAGE_WRITE;
        }
    else error = IL_OK;

        /*  If aborting, free up pixels if they don't belong to client */
    if (aborting) {
        if (!pImage->i.clientPixels && pImagePlane->pPixels) {
            IL_FREE (pImagePlane->pPixels);
            pImagePlane->pPixels = (ilPtr)NULL;
            pImagePlane->bufferSize = 0;
            }
        }
    else {
            /*  Success: realloc (shrink) the image down to the written size (offset to
                strip past last strip), to free up extra unused-but-allocated space.  
            */
        pImagePlane->bufferSize = pImage->pStripOffsets[pImage->nStrips];
        if (pImagePlane->pPixels) 
            pImagePlane->pPixels = (ilPtr)IL_REALLOC (pImagePlane->pPixels, 
                                                      pImagePlane->bufferSize);
        }

    return error;
}


    /*  ------------------------ ilWriteCompressedExecute ------------------------- */
    /*  Execute() function for ilWriteImage(), writing compressed images.
    */
static ilError ilWriteCompressedExecute (
    ilExecuteData  *pData,
    long                dstLine,
    long               *pNLines
    )
{
ilImagePrivPtr pPriv;
ilImagePtr     pImage;

    pPriv = (ilImagePrivPtr)pData->pPrivate;
    pImage = pPriv->pImage;

        /*  If first strip: copy non-null pipe palette (pPriv->pPalette) and comp data
            (NOTE: assumed to be JPEG!), after freeing old tables, to the image.
        */
    if (pData->srcLine <= 0) {
        if (pPriv->pPalette)
            bcopy ((char *)pPriv->pPalette, (char *)pPriv->pImage->i.pPalette, 
                   sizeof (unsigned short) * (3 * 256));
        if (pPriv->pCompData) {
            _ilJPEGFreeTables ((ilJPEGData *)pPriv->pImage->i.pCompData);
            if (!_ilJPEGCopyData ((ilJPEGData *)pPriv->pCompData, 
                                  (ilJPEGData *)pPriv->pImage->i.pCompData))
            return IL_ERROR_MALLOC;
            }
        }

        /*  Bump srcLine (index to next line); if > # lines in image, error */
    pData->srcLine += *pNLines;
    if (pData->srcLine > pImage->i.height)
        return IL_ERROR_MALFORMED_IMAGE_WRITE;

        /*  If # lines written != strip size and not last strip, inconstant strips */
    if ((*pNLines != pImage->stripHeight) && (pData->srcLine != pImage->i.height))
        return IL_ERROR_MALFORMED_IMAGE_WRITE;

        /*  Store offset to this and next strip; bump offset for next write */
    pImage->pStripOffsets[pPriv->nStripsWritten++] = pData->compressed.srcOffset;
    pData->compressed.srcOffset += pData->compressed.nBytesToRead;
    pImage->pStripOffsets[pPriv->nStripsWritten] = pData->compressed.srcOffset;

    return IL_OK;
}


    /*  -------------------- ilWriteCompressedImage -------------------- */
    /*  Called by ilWriteImage() to add a consumer to "pipe" which writes compressed
        data to the given "image".  "pipe" and "image" are guaranteed to be the correct 
        object types and to belong to the same context.  The pipe is guaranteed to
        be in the correct state (IL_PIPE_FORMING) and "image" is guaranteed to be the
        correct size (size of the pipe image).
    */
static ilBool ilWriteCompressedImage (
    ilPipe              pipe,
    ilObject            image
    )
{
ilImagePtr     pImage;
ilPipeInfo              info;                              
ilImageDes              des;
ilImageFormat           format;
ilBool                  mustConvert;
ilImagePrivPtr pPriv;
ilSrcElementData        srcData;
long                    stripHeight;
ilPtr                   pCompData;

        /* Get pipe info, don't force decompression (yet). */
    ilGetPipeInfo (pipe, FALSE, &info, &des, &format);

        /*  Convert (and decompress) as necessary to dest image des */
    pImage = (ilImagePtr)image;
    mustConvert = FALSE;
    if ((des.type != pImage->des.type)
     || (des.blackIsZero != pImage->des.blackIsZero)
     || (des.nSamplesPerPixel != pImage->des.nSamplesPerPixel))
        mustConvert = TRUE;
    else {
        int     plane;
        for (plane = 0; plane < des.nSamplesPerPixel; plane++)
            if ((des.nLevelsPerSample[plane] != pImage->des.nLevelsPerSample[plane])
             || (format.nBitsPerSample[plane] != pImage->format.nBitsPerSample[plane])) {
                mustConvert = TRUE;
                break;
                }
        }
    if (mustConvert) {
        ilImageDes      tempDes;
        tempDes = *pImage->i.pDes;
        tempDes.compression = IL_UNCOMPRESSED;      /* ilConvert() error otherwise */
        if (!ilConvert (pipe, &tempDes, (ilImageFormat *)NULL, 0, (ilPtr)NULL))
            return FALSE;
        des.compression = IL_UNCOMPRESSED;          /* force recompress below */
        }

        /*  Compress to compression type of image, including type-specific (G3/JPEG)
            compression data.  Compress to image's strip height if is established 
            (pStripOffsets non-null), else set "stripHeight" to 0 and use default.
        */
    if (pImage->des.compression == IL_G3)
         pCompData = (ilPtr)&pImage->des.compInfo.g3.flags;
    else if (pImage->des.compression == IL_G4)
         pCompData = (ilPtr)&pImage->des.compInfo.g4.flags;
    else if (pImage->des.compression == IL_JPEG)
         pCompData = (ilPtr)&pImage->des.compInfo.JPEG;
    else pCompData = (ilPtr)NULL;

    stripHeight = (pImage->pStripOffsets) ? pImage->stripHeight : 0;
    if (!ilCompress (pipe, pImage->des.compression, pCompData, stripHeight, 0))
        return FALSE;
    ilGetPipeInfo (pipe, FALSE, &info, (ilImageDes *)NULL, (ilImageFormat *)NULL);                    

        /*  If image does not yet have a strip height, set it to pipe's strip height */
    if (!stripHeight)
        if (!_ilAllocStripOffsets (pImage, info.stripHeight))
            return ilDeclarePipeInvalid (pipe, pipe->context->error);

        /*  Add a consumer to write to the image - merely bumps offsets. */
    srcData.consumerImage = (ilObject)pImage;
    srcData.stripHeight = 0;
    srcData.constantStrip = FALSE;
    srcData.minBufferHeight = 0;
    pPriv = (ilImagePrivPtr)ilAddPipeElement(pipe, IL_CONSUMER, sizeof(ilImagePrivRec), 0,
         &srcData, (ilDstElementData *)NULL, ilWriteImageInit, ilWriteCompressedCleanup, 
         ilWriteImageDestroy, ilWriteCompressedExecute, 0);
    if (!pPriv)
        return FALSE;

    pPriv->pImage = pImage;
    pPriv->pPalette = (pImage->des.type == IL_PALETTE) ? info.pPalette : 
                                                         (unsigned short *)NULL;

        /*  NOTE: assumes JPEG pCompData!  Point to pipe comp data if raw JPEG */
    if ((pImage->des.compression == IL_JPEG) 
     && (pImage->des.compInfo.JPEG.reserved & IL_JPEGM_RAW))
         pPriv->pCompData = info.pCompData;
    else pPriv->pCompData = (ilPtr)NULL;

    pImage->o.refCount++;               /* see ilWriteImageDestroy() */

    pipe->context->error = IL_OK;
    return TRUE;
}



/*  ============================ WRITE UNCOMPRESSED CODE ============================= */


        /*  --------------------- ilWriteImageExecute -------------------------------- */
        /*  Execute() for WriteImage(): merely bumps srcLine by # lines written.
        */
static ilError ilWriteImageExecute (
    ilExecuteData  *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
    ilImagePrivPtr      pPriv;

    pPriv = (ilImagePrivPtr)pData->pPrivate;

        /*  If first strip, copy pipe palette (pPriv->pPalette) to the image palette. */
    if ((pData->srcLine <= 0) && pPriv->pPalette) {
        bcopy ((char *)pPriv->pPalette, (char *)pPriv->pImage->i.pPalette, 
               sizeof (unsigned short) * (3 * 256));
        }

        /*  Bump srcLine (index to next line); if > # lines in image, error */
    pData->srcLine += *pNLines;
    if (pData->srcLine > pPriv->pImage->i.height)
        return IL_ERROR_MALFORMED_IMAGE_WRITE;

    return IL_OK;
}


        /*  ------------------------ ilWriteImage ---------------------------------- */
        /*  Public function: see spec.
            Adds the given image as a producer of the pipe.
        */
ilBool ilWriteImage (
    ilPipe              pipe,
    ilObject            image
    )
{
ilImagePtr     pImage;
ilImagePrivPtr pPriv;
ilContext      context;
ilPipeInfo              info;
ilSrcElementData        srcData;

        /*  Validate that pipe and image are such, and that they have the same context. */
    pImage = (ilImagePtr)image;
    context = pipe->context;
    if (pipe->objectType != IL_PIPE) {
        context->error = IL_ERROR_OBJECT_TYPE;
        return;
        }

    if ((pImage->o.p.objectType != IL_INTERNAL_IMAGE) 
     && (pImage->o.p.objectType != IL_CLIENT_IMAGE))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_OBJECT_TYPE);

    if (pImage->o.p.context != context)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_CONTEXT_MISMATCH);

        /*  Get pipe info; if pipe not in IL_PIPE_FORMING state: error.
            If producerObject is the given image, error: cant read/write same image.
        */                          
    if (ilGetPipeInfo (pipe, FALSE, &info, 
                      (ilImageDes *)NULL, (ilImageFormat *)NULL) != IL_PIPE_FORMING) {
        if (!context->error)
            ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);
        return FALSE;
        }
    if (info.producerObject == (ilObject)pImage)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_CIRCULAR_PIPE);

        /*  Validate pipe image.  width, height and descriptor must be the same. */
    if ((info.width != pImage->i.width) || (info.height != pImage->i.height))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_IMAGE_SIZE);

        /*  If a writing to a compressed image, call separate function and exit. */
    if (pImage->des.compression != IL_UNCOMPRESSED)
        return ilWriteCompressedImage (pipe, image);        /* EXIT */

        /*  Convert des and/or format if not same as the images format; exit if error. */
    if (!ilConvert (pipe, pImage->i.pDes, pImage->i.pFormat, 0, NULL))
        return FALSE;
    ilGetPipeInfo (pipe, TRUE, &info, (ilImageDes *)NULL, (ilImageFormat *)NULL);

        /*  Add a consumer element which writes to the given image.
            If writing a palette image, supply an Init() function to copy the palette.
        */
    srcData.consumerImage = (ilObject)pImage;
    srcData.stripHeight = 0;
    srcData.constantStrip = FALSE;
    srcData.minBufferHeight = 0;
    pPriv = (ilImagePrivPtr)ilAddPipeElement (pipe, IL_CONSUMER, sizeof (ilImagePrivRec), 
        0, &srcData, (ilDstElementData *)NULL, ilWriteImageInit, IL_NPF, 
        ilWriteImageDestroy, ilWriteImageExecute, 0);
    if (!pPriv)
        return FALSE;

        /*  Element successfully added; setup private data.
            Increment the refCount in the image; see notes for ilImageDestroy().
        */
    pPriv->pImage = pImage;
    pPriv->pPalette = (pImage->des.type == IL_PALETTE) ? info.pPalette : 
                                                         (unsigned short *)NULL;
    pImage->o.refCount++;               /* see ilWriteImageDestroy() */

    context->error = IL_OK;
    return TRUE;
}

