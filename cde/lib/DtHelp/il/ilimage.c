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
/* $XConsortium: ilimage.c /main/3 1995/10/23 15:48:58 rswiston $ */
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

        /*  /ilc/ilimage.c : General image handling code.
        */

#include <stdlib.h>

#include "ilint.h"
#include "ilpipelem.h"
#include "ilimage.h"
#include "ilcontext.h"
#include "ilerrors.h"
#include "ilutiljpeg.h"

        /*  Table of supported compression modes for each image type.
        */
static unsigned long    ilSupportedCompressions [IL_MAX_TYPE+1] = {
    /* IL_BITONAL */   ((1 << IL_UNCOMPRESSED) |
                        (1 << IL_G3)           |
                        (1 << IL_G4)           |
                        (1 << IL_LZW)          |
                        (1 << IL_PACKBITS)),
    /* IL_GRAY */      ((1 << IL_UNCOMPRESSED) |
                        (1 << IL_LZW) |
                        (1 << IL_PACKBITS) |
                        (1 << IL_JPEG)),
    /* IL_PALETTE */   ((1 << IL_UNCOMPRESSED) |
                        (1 << IL_PACKBITS) |
                        (1 << IL_LZW)),
    /* IL_RGB */       ((1 << IL_UNCOMPRESSED) |
                        (1 << IL_LZW) |
                        (1 << IL_JPEG)),
    /* IL_YCRCB */     ((1 << IL_UNCOMPRESSED) |
                        (1 << IL_LZW) |
                        (1 << IL_JPEG)) };

        /*      (ilBool) IL_VALID_COMPRESSION(comp, imageType) 
            returns TRUE if "comp" is a valid compression for "imageType", a valid image
            type (e.g. IL_BITONAL).
        */
#define IL_VALID_COMPRESSION(_comp, _imageType) \
    ((01L << (_comp)) & ilSupportedCompressions[_imageType])



        /*  ------------------------ ilValidateDesFormat ---------------------------- */
        /*  Validate the given image descriptor at "pDes" and the image format at 
            "pFormat" if it is non-null and validate that they are compatible 
            with each other.
                If "allowPrivateTypes" is true, private image type codes are allowed;
            otherwise an error is declared.  Should be true for client images.
                Return 0 (IL_OK) if valid, else error code.
            NOTE: this function defines valid compressions and nBitsPerSample.
        */
IL_PRIVATE ilError _ilValidateDesFormat (
    ilBool                  allowPrivateTypes,
    register const ilImageDes     *pDes,
    register const ilImageFormat  *pFormat
    )
{
#define MAX_BITS_PER_SAMPLE     24       /* maximum # of bits/sample supported */
register unsigned int           i, j, nBits, nLevels, nSamplesPerPixel;
const ilYCbCrSampleInfo              *pSample;

        /*  reserved must = 0; # samples must be 1 .. IL_MAX_SAMPLES.
        */
    for (i = 0; i < IL_DES_RESERVED_SIZE; i++)
        if (pDes->reserved[i] != 0)
            return IL_ERROR_RESERVED_NOT_ZERO;

    nSamplesPerPixel = pDes->nSamplesPerPixel;
    if ((nSamplesPerPixel <= 0) || (nSamplesPerPixel > IL_MAX_SAMPLES))
        return IL_ERROR_SAMPLES_PER_PIXEL;

        /*  unused bits in flags must = 0; if dithered palette bit set, must be a 
            palette image with levels whose product is <= # levels.
        */
    if (pDes->flags & ~(IL_DITHERED_PALETTE))
        return IL_ERROR_RESERVED_NOT_ZERO;
    if (pDes->flags & IL_DITHERED_PALETTE) {
        if ((pDes->type != IL_PALETTE)
         || ((pDes->typeInfo.palette.levels[0] * pDes->typeInfo.palette.levels[1] *
              pDes->typeInfo.palette.levels[2]) > pDes->nLevelsPerSample[0]))
            return IL_ERROR_PALETTE_INFO;
        }

        /*  If validating format: all values must be as documented in spec.
            For each sample: bits must be 1..MAX_BITS_PER_SAMPLE, and levels
            be >= 2 and fit in those bits.
                If no format, levels must be 1..2**MAX_BITS_PER_SAMPLE.
        */
    if (pFormat) {
        if (pFormat->byteOrder > IL_MAX_BYTE_ORDER) 
            return IL_ERROR_BYTE_ORDER;
        if (pFormat->sampleOrder > IL_MAX_SAMPLE_ORDER)
            return IL_ERROR_SAMPLE_ORDER;

            /*  NOTE: currently only pixel sample order supported if compressed */
        if ((pDes->compression != IL_UNCOMPRESSED)
         && (pFormat->sampleOrder != IL_SAMPLE_PIXELS))
            return IL_ERROR_SAMPLE_ORDER;

        nBits = pFormat->rowBitAlign;
        if ((nBits != 8) && (nBits != 16) && (nBits != 32))
            return IL_ERROR_ROW_BIT_ALIGN;

        for (i = 0; i < IL_FORMAT_RESERVED_SIZE; i++)
            if (pFormat->reserved[i] != 0)
                return IL_ERROR_RESERVED_NOT_ZERO;

        for (i = 0; i < nSamplesPerPixel; i++) {
            nBits = pFormat->nBitsPerSample[i];
            if ((nBits <= 0) || (nBits > MAX_BITS_PER_SAMPLE))
                return IL_ERROR_BITS_PER_SAMPLE;
            nLevels = pDes->nLevelsPerSample[i];
            if ((nLevels < 2) || (nLevels > (1<<nBits)))
                return IL_ERROR_LEVELS_PER_SAMPLE;
            }
        }
    else {
        for (i = 0; i < nSamplesPerPixel; i++) {
            nLevels = pDes->nLevelsPerSample [i];
            if ((nLevels < 2) || (nLevels > (1<<MAX_BITS_PER_SAMPLE)))
                return IL_ERROR_LEVELS_PER_SAMPLE;
            }
        }

        /*  Format and des are valid, excluding image type considerations.
            Validated based on type; allow user type only if caller said to.
                Note that no check is made on nBitsPerSample based on type.
            That is, it is allowed to create a bitonal image with 8 bits/pixel.
            It's just that the IL (probably) will never provide a conversion
            to 1 bit/pixel, so IL intrinsic functions are used on this image
            an error will occur - but client pipe elements could work on that image.
        */
    switch (pDes->type) {
      case IL_BITONAL:
        if (nSamplesPerPixel != 1)
            return IL_ERROR_SAMPLES_PER_PIXEL;
        break;

      case IL_PALETTE:
        if (pDes->nLevelsPerSample[0] > 256)
            return IL_ERROR_LEVELS_PER_SAMPLE;
      case IL_GRAY:
        if (nSamplesPerPixel != 1)
            return IL_ERROR_SAMPLES_PER_PIXEL;
        break;

      case IL_RGB:
        if (nSamplesPerPixel != 3)
            return IL_ERROR_SAMPLES_PER_PIXEL;
        break;

        /*  Validate YCbCr data; subsamples must be 1, 2 or 4, refWhite > refBlack.
            Image must be planar, 8 bits per sample.
        */
      case IL_YCBCR:
        if (nSamplesPerPixel != 3)
            return IL_ERROR_SAMPLES_PER_PIXEL;
        pSample = pDes->typeInfo.YCbCr.sample;
        i = 3;
        while (i-- > 0) {
            j = pSample->subsampleHoriz;
            if ((j != 1) && (j != 2) && (j != 4))
                return IL_ERROR_YCBCR_DATA;
            j = pSample->subsampleVert;
            if ((j != 1) && (j != 2) && (j != 4))
                return IL_ERROR_YCBCR_DATA;
            if (pSample->refBlack >= pSample->refWhite)
                return IL_ERROR_YCBCR_DATA;
            pSample++;
            }
        if (pDes->typeInfo.YCbCr.positioning != 0)
            return IL_ERROR_YCBCR_DATA;

        if (pFormat) {
            if ((pFormat->nBitsPerSample[0] != 8)
             || (pFormat->nBitsPerSample[1] != 8)
             || (pFormat->nBitsPerSample[2] != 8))
                return IL_ERROR_YCBCR_FORMAT;
            }
        break;

            /*  Unknown type: error or ok if private types allowed.
            */
      default:
        return (allowPrivateTypes) ? IL_OK : IL_ERROR_IMAGE_TYPE;
        break;
        }

        /*  Validate compression: not a user image type (exited above)
        */
    if (!IL_VALID_COMPRESSION (pDes->compression, pDes->type))
        return IL_ERROR_COMPRESSION;

    return IL_OK;
}

        /*  ----------------------- ilGetBytesPerRow ------------------------------ */
        /*  Public function: see spec.
            Returns the # bytes / row into "pnBytesPerRow" (for each plane of image),
            for an image of size "width", with "nSamplesPerPixel" (from des) and the
            format "*pFormat".
        */
void ilGetBytesPerRow (
    register const ilImageDes    *pDes,
    register const ilImageFormat *pFormat,
    long                    width,
    long                   *pnBytesPerRow       /* RETURNED */
    )
{
register const unsigned short    *pNBits;
register int                rowBitAlign, nBits, nSamplesPerPixel, i;

    pNBits = pFormat->nBitsPerSample;
    rowBitAlign = pFormat->rowBitAlign;
    if (rowBitAlign < 8)
        rowBitAlign = 8;
    nSamplesPerPixel = pDes->nSamplesPerPixel;

        /*  Calculate nBytesPerRow: # bits * width, rounded up by rowBitAlign.
            For 8 < bits/pixel <= 16, use word/pixel; <= 32, use long (TIFF std).
            Allocation size for each plane is then nBytesPerRow * height of image.
            If a YCBCrImage, take horizontal subsampling into account.
        */
    if (pFormat->sampleOrder == IL_SAMPLE_PIXELS) {

            /*  Pixel order: one row contains all of the bits for each pixel, in sample 
                order.  Sum the bits for one pixel; always start on a byte boundary for
                samples > 8 bits.
            */
        nBits = 0;
        while (nSamplesPerPixel-- > 0) {
            i = *pNBits++;
            if (i > 8) {
                nBits = ((nBits + 7) / 8) * 8;      /* round nBits to byte boundary */
                if (i <= 16)
                    i = 16;
                else i = 32;
                }
            nBits += i;
            }
        *pnBytesPerRow = ((nBits * width + rowBitAlign - 1) / rowBitAlign)
                        * (rowBitAlign / 8);
        }
    else  {
        register const ilYCbCrSampleInfo *pYCbCr;

            /*  Planar order: each plane is independent; return nSamplesPerPixel rowbytes.
                Take into account subsampling if YCbCr image.
            */
        pYCbCr = (pDes->type == IL_YCBCR) ? 
                   pDes->typeInfo.YCbCr.sample : (ilYCbCrSampleInfo *)NULL;

        while (nSamplesPerPixel-- > 0) {
            nBits = *pNBits++;
            if (nBits > 8) {
                if (nBits <= 16)
                    nBits = 16;
                else nBits = 32;
            }
            if (pYCbCr) {
                i = width >> _ilSubsampleShift[pYCbCr->subsampleHoriz];
                if (i <= 0) i = 1;
                pYCbCr++;
                }
            else i = width;
            *pnBytesPerRow++ = ((nBits * i + rowBitAlign - 1) / rowBitAlign)
                              * (rowBitAlign / 8);
            }
        }
}


        /*  ------------------------ ilMallocImagePixels ---------------------------- */
        /*  For uncompressed images: create a buffer for the pixels of image "*pImage", 
            based on pImage->des, pImage->format, and pImage->height.
                For compressed images: just null pPixels and bufferSize; code that
            writes to a compressed image must check the bufferSize and malloc (first
            time) or realloc as necessary. Set ownPixels to true in either case.
                Returns: zero if success, else error code.
        */
IL_PRIVATE ilError _ilMallocImagePixels (
    register ilImagePtr pImage
    )
{
register int            plane, nPlanes;
int                     MCUMinus1;
register long           nBytesToAlloc, nBytesPerRow, allocHeight;
ilPtr                   pPixels;
long                    rowBytesPerPlane [IL_MAX_SAMPLES];
register ilYCbCrSampleInfo *pYCbCr;


        /*  Get the bytes per row, set into the image. */
    ilGetBytesPerRow (&pImage->des, &pImage->format, pImage->i.width, rowBytesPerPlane);

            /*  Alloc each plane; on failure, backtrack and free those already created.
                Don't allocate buffer for compressed image!
                If YCbCr image point pYCbCr and calc maxVertSubsample (1,2,4) and
                MCU (Minimum Coded Unit) = 8 * maxVertSubsample.
            */
    nPlanes = (pImage->format.sampleOrder == IL_SAMPLE_PIXELS) ? 1 :
               pImage->des.nSamplesPerPixel;
    if (pImage->des.type == IL_YCBCR) {
        pYCbCr = pImage->des.typeInfo.YCbCr.sample;
        MCUMinus1 = pYCbCr[0].subsampleVert;
        if (pYCbCr[1].subsampleVert > MCUMinus1)
            MCUMinus1 = pYCbCr[1].subsampleVert;
        if (pYCbCr[2].subsampleVert > MCUMinus1)
            MCUMinus1 = pYCbCr[2].subsampleVert;
        MCUMinus1 = MCUMinus1 * 8 - 1;  /* e.g. if max = 4 MCUMinus1 is 31 */
        }
    else pYCbCr = (ilYCbCrSampleInfo *)NULL;

    for (plane = 0; plane < nPlanes; plane++) {
        nBytesPerRow = rowBytesPerPlane [plane];
        if (pImage->des.compression != IL_UNCOMPRESSED)
             nBytesToAlloc = 0;
        else {
                /*  If YCbCr image, alloc smaller buffer (divide by subsample) but
                    first rounded up to the MCU.  For example, if max subsample is 2,
                    an make unsubsampled buffer a mult of 16; a 2 subsampled mult of 8.
                    "allocHeight" is height rounded up to MCU / vertSubsample.
                */
            if (pYCbCr) {
                allocHeight = ((pImage->i.height + MCUMinus1) & ~MCUMinus1)
                                             >> _ilSubsampleShift[pYCbCr->subsampleVert];
                pYCbCr++;
                }
            else allocHeight = pImage->i.height;
            nBytesToAlloc = nBytesPerRow * allocHeight;
            }
        if (!nBytesToAlloc)
            pPixels = (ilPtr)NULL;
        else {
            pPixels = (ilPtr)IL_MALLOC (nBytesToAlloc);
            if (!pPixels) {                     /* error; free those already created */
                while ((plane != 0)) {
                    plane--;
                    if ((pPixels = pImage->i.plane[plane].pPixels))
                        IL_FREE (pPixels);
                    }
                pImage->i.clientPixels = TRUE;      /* so IL does not deallocate them */
                return IL_ERROR_MALLOC;             /* EXIT */
                }
            }
        pImage->i.plane[plane].pPixels = pPixels;
        pImage->i.plane[plane].nBytesPerRow = nBytesPerRow;
        pImage->i.plane[plane].bufferSize = nBytesToAlloc;
        }   /* END each plane */

    pImage->i.clientPixels = FALSE;
    return IL_OK;
}


        /*  ------------------------ ilFreeImagePixels ----------------------------- */
        /*  Destroy() function for client and internal image objects.
            Also used to discard pixels when writing images or loading client images.
            Free the pixels of the image only if the IL owns them.
        */
IL_PRIVATE void _ilFreeImagePixels (
    register ilImagePtr pImage
    )
{
int                     plane;
ilPtr                   pPixels;

        /*  If the IL owns the pixels (it does not if "clientPixels" true to 
            ilCreateClientImage()), then free them if non-null.  
            If pixel sample order, just one plane, else nSamplesPerPixel planes.
        */
    if (!pImage->i.clientPixels) {
        for (plane = 0; plane < IL_NPLANES(pImage); plane++) {
            pPixels = pImage->i.plane[plane].pPixels;
            if (pPixels)
                IL_FREE (pPixels);
            }
        pImage->i.clientPixels = TRUE;      /* so IL does not deallocate them */
        }

        /*  If own palette and it is non-null free it. */
    if (!pImage->i.clientPalette && pImage->i.pPalette) {
        IL_FREE (pImage->i.pPalette);
        pImage->i.clientPalette = TRUE;
        }

        /*  If own comp data and it is non-null free it and associated (JPEG) tables */
    if (!pImage->i.clientCompData && pImage->i.pCompData) {
        if (pImage->des.compression == IL_JPEG)
            _ilJPEGFreeTables ((ilJPEGData *)pImage->i.pCompData);
        IL_FREE (pImage->i.pCompData);
        pImage->i.pCompData = (ilPtr)NULL;
        pImage->i.clientCompData = TRUE;
        }

        /*  If strip offset array non-null free it */
    if (pImage->pStripOffsets) {
        IL_FREE (pImage->pStripOffsets);
        pImage->pStripOffsets = (long *)NULL;
        }
}


    /*  ------------------------ ilAllocStripOffsets ----------------------------- */
    /*  For compressed images only: allocate strip offsets array (pStripOffsets)
        using the given "stripHeight", or use the default strip height if "stripHeight"
        is zero (0).  Locks image to that strip height.
    */
IL_PRIVATE ilBool _ilAllocStripOffsets (
    register ilImagePtr pImage,
    register long       stripHeight
    )
{
register long           nStrips;

    if (stripHeight <= 0)
        stripHeight = ilRecommendedStripHeight (pImage->i.pDes, pImage->i.pFormat,
            pImage->i.width, pImage->i.height);

    if (stripHeight > pImage->i.height)
        stripHeight = pImage->i.height;

    nStrips = (pImage->i.height + stripHeight - 1) / stripHeight;
    pImage->pStripOffsets = (long *)IL_MALLOC_ZERO ((nStrips + 1) * sizeof(long));
    if (!pImage->pStripOffsets) {
        pImage->o.p.context->error = IL_ERROR_MALLOC;
        return FALSE;
        }
    pImage->nStrips = nStrips;
    pImage->stripHeight = stripHeight;

    pImage->o.p.context->error = IL_OK;
    return TRUE;
}


        /*  ---------------------- ilCreateInternalImage --------------------------- */
        /*  Public function: see spec.
        */
ilInternalImage ilCreateInternalImage (
    ilContext           context,
    long                width,
    long                height,
    ilImageDes         *pDes,
    unsigned long       mustBeZero
    )
{
register ilImagePtr     pImage;
ilError                 error;


    if (mustBeZero != 0) {
        context->error = IL_ERROR_PAR_NOT_ZERO;
        return (ilInternalImage)NULL;
        }

        /*  Null (zero-sized) images not allowed; exit w/ error if so. */
    if ((width <= 0) || (height <= 0)) {
        context->error = IL_ERROR_ZERO_SIZE_IMAGE;
        return (ilInternalImage)NULL;
        }

        /*  Validate pDes: don't allow user-defined types. */
    if ((context->error = _ilValidateDesFormat (FALSE, pDes, (ilImageFormat *)NULL)))
        return (ilInternalImage)NULL;

        /*  Create the object and add it to context. */
    pImage = (ilImagePtr)_ilCreateObject (context, IL_INTERNAL_IMAGE, _ilFreeImagePixels, 
                                         sizeof (ilImageRec));
    if (!pImage)
        return (ilInternalImage)NULL;

        /*  Fill in the image structure, excluding object header (.o).
            Init the format to std format, based on image type, which is not private type.
            For bits per sample (other than bitonal): use 4 bits if levels/sample <= 16,
            else use 8 bits.  Could use other values (e.g. min # bits for levels/sample),
            but at this point there are no conversions other than for 4<->8 bits.
        */
    pImage->des = *pDes;
    switch (pDes->type) {
      case IL_BITONAL:
        pImage->format = *IL_FORMAT_BIT;
        break;

      case IL_GRAY:
      case IL_PALETTE:
        pImage->format = *IL_FORMAT_BYTE;
        if (pDes->nLevelsPerSample[0] <= 16)
            pImage->format.nBitsPerSample[0] = 4;
        break;

      case IL_RGB:
        pImage->format = *IL_FORMAT_3BYTE_PIXEL;
        if ((pDes->nLevelsPerSample[0] <= 16)
         && (pDes->nLevelsPerSample[1] <= 16)
         && (pDes->nLevelsPerSample[2] <= 16))
            pImage->format.nBitsPerSample[0] = pImage->format.nBitsPerSample[1] = 
                pImage->format.nBitsPerSample[2] = 4;
        break;

        /*  YCbCr: always use 8 bits / pixel.  If uncompressed use planar format,
            but if compressed use pixel (e.g. JPEG interleaved format).
        */
      case IL_YCBCR:
        pImage->format = (pDes->compression == IL_UNCOMPRESSED) ? 
                              *IL_FORMAT_3BYTE_PLANE : *IL_FORMAT_3BYTE_PIXEL;
        break;

      default:
        context->error = IL_ERROR_IMAGE_TYPE;
        return (ilInternalImage)NULL; 
        }

        /*  Init image data, allocate pixels if uncompressed */
    pImage->nStrips = 0;            /* if compressed, strip height not set yet */
    pImage->stripHeight = 0;
    pImage->pStripOffsets = (long *)NULL;

    pImage->i.pDes = &pImage->des;
    pImage->i.pFormat = &pImage->format;
    pImage->i.width = width;
    pImage->i.height = height;
    pImage->i.pCompData = (ilPtr)NULL;
    pImage->i.clientCompData = TRUE;
    pImage->i.pPalette = (unsigned short *)NULL;
    pImage->i.clientPalette = TRUE;

    if ((error = _ilMallocImagePixels (pImage))) {
        ilDestroyObject ((ilObject)pImage);
        context->error = error;
        return (ilInternalImage)NULL;
        }

        /*  If a palette image, create an empty palette. */
    if (pDes->type == IL_PALETTE) {
        pImage->i.pPalette = (unsigned short *)
                                 IL_MALLOC_ZERO (sizeof (unsigned short) * (3 * 256));
        if (!pImage->i.pPalette) {
            ilDestroyObject ((ilObject)pImage);
            context->error = IL_ERROR_MALLOC;
            return (ilInternalImage)NULL;
            }
        pImage->i.clientPalette = FALSE;
        }

        /*  If raw JPEG compressed, point pCompData to allocated JPEG info block, else
            init pCompData to null, owned by client (i.e. not freed when destroyed). 
        */
    if ((pDes->compression == IL_JPEG) && (pDes->compInfo.JPEG.reserved & IL_JPEGM_RAW)) {
        if (!(pImage->i.pCompData = (ilPtr)IL_MALLOC_ZERO (sizeof (ilJPEGData)))) {
            ilDestroyObject ((ilObject)pImage);
            context->error = IL_ERROR_MALLOC;
            return (ilInternalImage)NULL;
            }
        pImage->i.clientCompData = FALSE;           /* IL must now destroy comp data */
        }

    context->error = IL_OK;
    return (ilInternalImage)pImage;
}


        /*  ------------------------ ilQueryInternalImage ----------------------------- */
        /*  Public function: see spec.
        */
ilBool ilQueryInternalImage (
    ilInternalImage     image,
    long               *pWidth,                 /* RETURNED */
    long               *pHeight,                /* RETURNED */
    ilImageDes         *pDes,                   /* RETURNED */
    unsigned long       mustBeZero
    )
{
register ilImagePtr     pImage;


        /*  Check that given image is an internal image.
        */
    pImage = (ilImagePtr)image;
    if (mustBeZero != 0) {
        pImage->o.p.context->error = IL_ERROR_PAR_NOT_ZERO;
        return;                                                 /* EXIT */
        }
    if (pImage->o.p.objectType != IL_INTERNAL_IMAGE) {
        pImage->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return;                                                 /* EXIT */
        }

    pImage->o.p.context->error = IL_OK;

        /*  Right type of image: return values to non-null ptrs. */
    if (pWidth)
        *pWidth = pImage->i.width;
    if (pHeight)
        *pHeight = pImage->i.height;
    if (pDes)
        *pDes = pImage->des;

    return TRUE;
}


        /*  ---------------------- ilCreateClientImage --------------------------- */
        /*  Public function: see spec.
        */
ilClientImage ilCreateClientImage (
    ilContext           context,
    ilImageInfo        *pInfo,
    unsigned long       mustBeZero
    )
{
register ilImagePtr     pImage;
ilError                 error;
int                     plane, nPlanes;


    if (mustBeZero != 0) {
        context->error = IL_ERROR_PAR_NOT_ZERO;
        return (ilClientImage)NULL;
        }

        /*  Null (zero-sized) images not allowed; exit w/ error if so. */
    if ((pInfo->width <= 0) || (pInfo->height <= 0)) {
        context->error = IL_ERROR_ZERO_SIZE_IMAGE;
        return (ilClientImage)NULL;
        }

        /*  Validate pDes and pFormat: do allow user-defined types. */
    if ((context->error = _ilValidateDesFormat (TRUE, pInfo->pDes, pInfo->pFormat)))
        return (ilClientImage)NULL;

        /*  Create the object and add it to context. */
    pImage = (ilImagePtr)_ilCreateObject (context, IL_CLIENT_IMAGE, _ilFreeImagePixels, 
                                         sizeof (ilImageRec));
    if (!pImage)
        return (ilClientImage)NULL;

        /*  Fill in the image structure, excluding object header (.o).
            Copy caller's des, and if "raw" JPEG force to non-raw.  Client must be
            able to access image, and non-raw (JIF) format only meaningful way to do so.
        */
    pImage->des = *pInfo->pDes;
    if (pImage->des.compression == IL_JPEG)
        pImage->des.compInfo.JPEG.reserved = 0;

    pImage->format = *pInfo->pFormat;
    pImage->nStrips = 0;            /* if compressed, no data present yet */
    pImage->stripHeight = 0;
    pImage->pStripOffsets = (long *)NULL;

    pImage->i.pDes = &pImage->des;
    pImage->i.pFormat = &pImage->format;
    pImage->i.width = pInfo->width;
    pImage->i.height = pInfo->height;
    pImage->i.pPalette = (unsigned short *)NULL;    /* assume no palette or comp data */
    pImage->i.clientPalette = TRUE;
    pImage->i.pCompData = (ilPtr)NULL;
    pImage->i.clientCompData = TRUE;
    pImage->i.clientPixels = pInfo->clientPixels;

        /*  If a palette image, point to the one from the caller if pPalette not null;
            if null, create one here and the IL owns it.
        */
    if (pImage->des.type == IL_PALETTE) {
        if (pInfo->pPalette && pInfo->clientPalette) {
            pImage->i.pPalette = pInfo->pPalette;
            pImage->i.clientPalette = TRUE;
            }
        else {
            pImage->i.pPalette = (unsigned short *)
                                    IL_MALLOC_ZERO (sizeof (unsigned short) * (3 * 256));
            if (!pImage->i.pPalette) {
                ilDestroyObject ((ilObject)pImage);
                context->error = IL_ERROR_MALLOC;
                return (ilClientImage)NULL;
                }
            pImage->i.clientPalette = FALSE;
            }
        }

        /* multi-plane planar compressed images not supported */
    nPlanes = IL_NPLANES(pImage);
    if ((pImage->des.compression != IL_UNCOMPRESSED) && (nPlanes > 1)) {
        ilDestroyObject ((ilObject)pImage);
        context->error = IL_ERROR_SAMPLE_ORDER;
        return (ilClientImage)NULL;
        }

        /*  If caller providing pixels (clientPixels == true), point to them,
            otherwise allocate buffer space; if failure delete object and return error.
        */
    if (pInfo->clientPixels) {
        for (plane = 0; plane < nPlanes; plane++)
            pImage->i.plane[plane] = pInfo->plane[plane];
        }
    else {      /*  no pixels from client; allocate them here if uncompressed image */
        if ((error = _ilMallocImagePixels (pImage))) {
            ilDestroyObject ((ilObject)pImage);
            context->error = error;
            return (ilClientImage)NULL;
            }
        }

        /*  Compressed client images stored as one big strip; allocate strip offset array.
            If client pixels, offset[0] = 0, offset[1] = nBytes - all data in 1 strip.
        */
    if (pImage->des.compression != IL_UNCOMPRESSED) {
        if (!_ilAllocStripOffsets (pImage, pImage->i.height)) {
            error = context->error;
            ilDestroyObject ((ilObject)pImage);
            context->error = error;
            return (ilClientImage)NULL;
            }
        if (pInfo->clientPixels) {
            pImage->pStripOffsets[0] = 0;
            pImage->pStripOffsets[1] = pImage->i.plane[0].bufferSize;
            }
        }

    context->error = IL_OK;
    return (ilClientImage)pImage;
}


        /*  ------------------------ ilQueryClientImage ----------------------------- */
        /*  Public function: see spec.
        */
ilBool ilQueryClientImage (
    ilClientImage       image,
    ilImageInfo       **ppInfo,                 /* RETURNED */
    unsigned long       mustBeZero
    )
{
register ilImagePtr     pImage;

        /*  Validate given image of correct type; return false if not realized.
        */
    pImage = (ilImagePtr)image;
    if (mustBeZero != 0) {
        pImage->o.p.context->error = IL_ERROR_PAR_NOT_ZERO;
        return FALSE;
        }
    if (pImage->o.p.objectType != IL_CLIENT_IMAGE) {
        pImage->o.p.context->error = IL_ERROR_OBJECT_TYPE;
        return FALSE;
        }

    if (ppInfo)
        *ppInfo = &pImage->i;

    pImage->o.p.context->error = IL_OK;
    return TRUE;
}



