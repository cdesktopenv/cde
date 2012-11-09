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
/* $XConsortium: ilmap.c /main/5 1996/06/19 12:21:21 ageorge $ */
        /*  /ilc/ilmap.c : Contains ilMap(), which adds a pipe element to remap
            the pipe image given a LUT image.
        */

#include "ilint.h"
#include "ilpipelem.h"
#include "ilerrors.h"


        /*  Private data for map pipe functions.
        */
typedef struct {

        /*  Data inited by ilMap() when pipe element added. */
    ilClientImage   mapImage;               /* map (lookup table) image */
    ilPtr           pTranslate;             /* ptr to map image pixels */

        /*  Rest of the data is inited by the Init() function */
    long            width;                  /* width of src/dst images */
    long            srcRowBytes;            /* bytes/row of src image */
    ilPtr           pSrcPixels;             /* ptr to start of src pixels */
    long            dstRowBytes;            /* bytes/row of dst image */
    ilPtr           pDstPixels;             /* ptr to start of dst pixels */
    } ilMapPrivRec, *ilMapPrivPtr;


        /*  --------------------- ilMapInit -------------------------- */
        /*  Init() function: copy values from given images to private for fast reference.
        */
static ilError ilMapInit (
    ilPtr              pPrivate,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
    ilMapPrivPtr        pPriv = (ilMapPrivPtr)pPrivate;
    pPriv->width = pSrcImage->width;
    pPriv->srcRowBytes = pSrcImage->plane[0].nBytesPerRow;
    pPriv->pSrcPixels = pSrcImage->plane[0].pPixels;
    pPriv->dstRowBytes = pDstImage->plane[0].nBytesPerRow;
    pPriv->pDstPixels = pDstImage->plane[0].pPixels;
    return IL_OK;
}

        /*  --------------------- ilMapByteExecute -------------------------- */
        /*  Execute() pipe element function for case where the source is an image
            of bytes.  Lookup each byte in the 256-wide translate image and pass to dst.
        */
static ilError ilMapByteExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilMapPrivPtr                pPriv;
long                        srcRowBytes, dstRowBytes, nRowsM1, nLinesM1;
ilPtr                       pSrcLine, pDstLine;
register long               rowCount;
register ilPtr              pSrc, pDst, pTranslate;

        /*  Get data from private, exit if no rows or lines to do.
        */
    pPriv = (ilMapPrivPtr)pData->pPrivate;
    nRowsM1 = pPriv->width;
    if (nRowsM1 <= 0)
        return;
    nRowsM1--;
    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return;
    nLinesM1--;

    srcRowBytes = pPriv->srcRowBytes;
    pSrcLine = pPriv->pSrcPixels + pData->srcLine * srcRowBytes;
    dstRowBytes = pPriv->dstRowBytes;
    pDstLine = pPriv->pDstPixels + dstLine * dstRowBytes;
    pTranslate = pPriv->pTranslate;

        /*  Lookup each src byte in table (256-wide byte image) and copy to dst.
        */
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes;
        rowCount = nRowsM1;
        do {
            *pDst++ = (ilByte)pTranslate[*pSrc++];
            } while (--rowCount >= 0);
        } while (--nLinesM1 >= 0);

    return IL_OK;
}

        /*  --------------------- ilMap3ByteExecute -------------------------- */
        /*  Execute() pipe element function for 3 byte/pixel, pixel order case.
        */
static ilError ilMap3ByteExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilMapPrivPtr                pPriv;
long                        srcRowBytes, dstRowBytes, nRowsM1, nLinesM1;
ilPtr                       pSrcLine, pDstLine;
register long               rowCount;
register ilPtr              pSrc, pDst, pTranslate;

        /*  Get data from private, exit if no rows or lines to do.
        */
    pPriv = (ilMapPrivPtr)pData->pPrivate;
    nRowsM1 = pPriv->width;
    if (nRowsM1 <= 0)
        return;
    nRowsM1--;
    nLinesM1 = *pNLines;
    if (nLinesM1 <= 0)
        return;
    nLinesM1--;

    srcRowBytes = pPriv->srcRowBytes;
    pSrcLine = pPriv->pSrcPixels + pData->srcLine * srcRowBytes;
    dstRowBytes = pPriv->dstRowBytes;
    pDstLine = pPriv->pDstPixels + dstLine * dstRowBytes;
    pTranslate = pPriv->pTranslate;

        /*  For each pixel, lookup the samples independently.  The src, dst and map images
            are all 3 components in sample order - must multiply * 3 to index.
        */
    do {
        pSrc = pSrcLine;
        pSrcLine += srcRowBytes;
        pDst = pDstLine;
        pDstLine += dstRowBytes;
        rowCount = nRowsM1;
        do {
            *pDst++ = (pTranslate + 0)[*pSrc++ * 3];
            *pDst++ = (pTranslate + 1)[*pSrc++ * 3];
            *pDst++ = (pTranslate + 2)[*pSrc++ * 3];
            } while (--rowCount >= 0);
        } while (--nLinesM1 >= 0);

    return IL_OK;
}


        /*  ------------------------ ilMapDestroy -------------------------------- */
        /*  Destroy() function for ilWriteMap().  Calls ilDestroyObject() with
            element's pObject, which should point to the image.  The image's refCount
            is inc'd when pipe element added.  ilDestroyObject() will dec it and free
            the image if the refCount is 0.  Using refCount this way prevents the user
            from destroying a map image still attached to a pipe.
        */
static ilError ilMapDestroy (
    ilPtr     pPrivate
    )
{
    ilMapPrivPtr pPriv = (ilMapPrivPtr)pPrivate;
    ilDestroyObject ((ilObject)pPriv->mapImage);
    return IL_OK;
}

        /*  ------------------------ ilMap ---------------------------------- */
        /*  Public function: see spec.
            Use "mapImage" as a lookup table for each pixel in the pipe image.
        */
ilBool ilMap (
    ilPipe              pipe,
    ilClientImage       mapImage
    )
{
register ilMapPrivPtr   pPriv;
register ilContext      context;
ilPipeInfo              info;
ilImageDes              des;
ilImageFormat           format;
ilImageInfo            *pInfo;
int                     i;
ilError               (*executeProc) ();

        /*  Validate that pipe and image are such, and that they have the same context.
        */
    context = pipe->context;
    if (pipe->objectType != IL_PIPE) {
        context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }
    if (mapImage->objectType != IL_CLIENT_IMAGE) 
        return ilDeclarePipeInvalid (pipe, IL_ERROR_OBJECT_TYPE);
    if (mapImage->context != context)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_CONTEXT_MISMATCH);

        /*  Get pipe info; if pipe not in IL_PIPE_FORMING state: error.
        */                          
    if (ilGetPipeInfo (pipe, TRUE, &info, &des, &format) != IL_PIPE_FORMING) {
        if (!pipe->context->error)
            ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);
        return FALSE;
        }

        /*  Query mapImage; must be uncompressed, 256x1, # samples = same as pipe image,
            each 8 bits/sample, or error.
        */
    if (!ilQueryClientImage (mapImage, &pInfo, 0))
        return FALSE;
    if ((pInfo->width != 256) || (pInfo->height != 1) 
     || (pInfo->pDes->compression != IL_UNCOMPRESSED)
     || (pInfo->pDes->nSamplesPerPixel != des.nSamplesPerPixel))
        return ilDeclarePipeInvalid (pipe, IL_ERROR_MAP_IMAGE);
    for (i = 0; i < des.nSamplesPerPixel; i++)
        if (pInfo->pFormat->nBitsPerSample[i] != 8)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_MAP_IMAGE);

        /*  Handle 1 and 3 byte (pixel format) cases; set "executeProc" accordingly.
        */
    if (des.nSamplesPerPixel == 1) {
        if (format.nBitsPerSample[0] != 8)
            if (!ilConvert (pipe, (ilImageDes *)NULL, IL_FORMAT_BYTE, 0, NULL))
                return FALSE;
        executeProc = ilMapByteExecute;
        }
    else if (des.nSamplesPerPixel == 3) {
        if ((format.nBitsPerSample[0] != 8)
         || (format.nBitsPerSample[1] != 8)
         || (format.nBitsPerSample[2] != 8)
         || (format.sampleOrder != IL_SAMPLE_PIXELS))
            if (!ilConvert (pipe, (ilImageDes *)NULL, IL_FORMAT_3BYTE_PIXEL, 0, NULL))
                return FALSE;
        executeProc = ilMap3ByteExecute;
        }
    else return ilDeclarePipeInvalid (pipe, IL_ERROR_NOT_IMPLEMENTED);

        /*  Add a filter to do the map, copy data to private.  Inc refCount for mapImage;
            see notes for ilMapDestroy().
        */
    pPriv = (ilMapPrivPtr)ilAddPipeElement (pipe, IL_FILTER, sizeof (ilMapPrivRec), 
                         0, (ilSrcElementData *)NULL, (ilDstElementData *)NULL, 
                         ilMapInit, IL_NPF, ilMapDestroy, executeProc, NULL, 0);
    if (!pPriv)
        return FALSE;
    pPriv->mapImage = mapImage;
    ((ilObjectPtr)mapImage)->refCount++;
    pPriv->pTranslate = pInfo->plane[0].pPixels;

    context->error = IL_OK;
    return TRUE;
}


