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
/* $XConsortium: iltiff.c /main/6 1996/10/04 11:29:48 rcs $ */
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

    /*  /ilc/iltiff.c : General TIFF file access functions, including
        those exposed in /ilc/iltiffint.h to other /ilc/iltiff*.c code.
    */

#include <stdlib.h>

#include "iltiffint.h"
#include "ilerrors.h"

#ifdef DTLIB
/* Used when this module is compiled by the DtHelp library build */
#include "GraphicsP.h"
#endif /* DTLIB */

    /*  Size in bytes of one item (# items = TIFF "length"), for each TIFF tag type.
        Also, # of entries of that type which fit in one long.
        Indexed by tag type = 1..IL_MAX_TAG_TYPE (0 entry unused).
    */
IL_PRIVATE int _ilTagTypeItemSizes [IL_MAX_TAG_TYPE+1] = {
    0,                          /* 0th entry unused; invalid tag type */
    1,                          /* IL_TAG_BYTE */
    1,                          /* IL_TAG_ASCII */
    2,                          /* IL_TAG_SHORT */
    4,                          /* IL_TAG_LONG */
    8 };                        /* IL_TAG_RATIONAL */

IL_PRIVATE int _ilTagTypeItemsThatFit [IL_MAX_TAG_TYPE+1] = {
    0,                          /* 0th entry unused; invalid tag type */
    4,                          /* IL_TAG_BYTE */
    4,                          /* IL_TAG_ASCII */
    2,                          /* IL_TAG_SHORT */
    1,                          /* IL_TAG_LONG */
    0 };                        /* IL_TAG_RATIONAL */

    /*  Structure of the first 8 bytes of a TIFF file: see ilConnectFile() */
typedef struct {
    ilByte              byteOrder[2];
    ilByte              version[2];
    ilByte              IFDHeadOffset[4];
    } ilTIFFHeader;


        /*  ------------------------- ilFreeFileList -------------------------------- */
        /*  Free the file list in the given file "*pFile".
            Note: ilFreeFileList() is also the Destroy() function for ilFile object.
        */

static void ilFreeFileList (
    ilFilePtr           pFile
    )
{
register ilFileImagePtr pFileImage, pAltFileImage, pFileImageTemp;

        /*  Run thru the list of file images; for each: if a mask image destroy it
            (only one); if child images, destroy them - they are linked by pChild.
        */
    pFileImage = pFile->pFileImageHead;
    while (pFileImage) {

            /*  Destroy the mask image if present */
        pAltFileImage = (ilFileImagePtr)pFileImage->p.pMask;
        if (pAltFileImage)
            IL_FREE (pAltFileImage);

            /*  Destroy the child images if present.  They are linked
                by pNext, and each one can have a mask image.
            */
        pAltFileImage = (ilFileImagePtr)pFileImage->p.pChild;
        while (pAltFileImage) {
            if (pAltFileImage->p.pMask)
                IL_FREE (pAltFileImage->p.pMask);
            pFileImageTemp = pAltFileImage;
            pAltFileImage = (ilFileImagePtr)pAltFileImage->p.pNext;
            IL_FREE (pFileImageTemp);
            }

            /*  Destroy the main image */
        pFileImageTemp = pFileImage;
        pFileImage = (ilFileImagePtr)pFileImage->p.pNext;
        IL_FREE (pFileImageTemp);
        }

        /*  Make the list empty and declare it invalid.
        */
    pFile->pFileImageHead = pFile->pFileImageTail = (ilFileImagePtr)NULL;
    pFile->haveImageList = FALSE;
}


        /*  ----------------------- ilReadFileTags --------------------------------- */
        /*  Public function: see spec.
        */
ilPtr ilReadFileTags (
    ilFileImage         fileImage,
    int                 nTagsInit,
    unsigned short     *pTagNumbers,
    ilFileTag         **ppTags,
    unsigned long       mustBeZero
    )
{
#define RFT_FATAL_ERROR(_error) {error = (_error);  goto RFTFatalError; }
ilFileImagePtr          pFileImage;
ilTIFFTagPtr            pFileTag, pFileTags;
ilError                 error;
ilBool                  tagFound;
unsigned long           readSize, mallocSize;
CARD32                  offset;
ilPtr                   pTagData, pTagAlloc;
ilFilePtr               pFile;
ilFileTag              *pTag, **ppTag;
int                     tagNumber, nFileTags, nItems, nTags, tagType, i;
register ilBool         bigEndian;

    pFileImage = (ilFileImagePtr)fileImage;
    if (mustBeZero != 0) {
        pFileImage->context->error = IL_ERROR_PAR_NOT_ZERO;
        return (ilPtr)NULL;                                     /* EXIT */
        }
    if (nTagsInit <= 0) {
        pFileImage->context->error = IL_OK;
        return (ilPtr)NULL;                                     /* no tags; EXIT */
        }

    pFile = (ilFilePtr)pFileImage->p.file;
    bigEndian = pFile->bigEndian;

        /*  Allocate space and read in all the file tags to "*pFileTags".
            Flip the bytes of number, type and length, but not the data/offset.
        */
    pTagAlloc = (ilPtr)NULL;                    /* in case of fatal error */
    readSize = pFileImage->tagCount * IL_TAG_SIZE;
    pFileTags = (ilTIFFTagPtr)IL_MALLOC (readSize);
    if (!pFileTags)
        RFT_FATAL_ERROR (IL_ERROR_MALLOC)
    if (!IL_SEEK (pFile, pFileImage->tagOffset)
     || !IL_READ (pFile, readSize, pFileTags))
        RFT_FATAL_ERROR (IL_ERROR_FILE_IO)

    pFileTag = pFileTags;
    pTagData = (ilPtr)pFileTag;
    nFileTags = pFileImage->tagCount;
    while (nFileTags-- > 0) {
        IL_FLIP_SHORT (bigEndian, pTagData, pFileTag->number)
        pTagData += 2;
        IL_FLIP_SHORT (bigEndian, pTagData, tagType)

            /*  NOTE: declare fatal error if tag type out of range, BUT TIFF 6.0 says 
                we should ignore these tags; ALSO, there are new tag types for 6.0.
            */
        if ((tagType == 0) || (tagType > IL_MAX_TAG_TYPE))
            RFT_FATAL_ERROR (IL_ERROR_FILE_TAG_TYPE)
        pFileTag->type = tagType;
        pTagData += 2;
        IL_FLIP_LONG (bigEndian, pTagData, pFileTag->length)
        pTagData += 8;                      /* skip length and data */
        pFileTag++;
        }

        /*  For each of caller's tags: find *pTagNumber in the pFileTags list.
            If not found, set corresponding "ppTags" entry to null, else point it
            at the found entry in pFileTags list, add its data size (rounded up to 
            multiple of 4 - long-align), and the size of one ilFileTag, to "mallocSize".
        */
    mallocSize = 0;
    ppTag = ppTags;
    nTags = nTagsInit;
    while (nTags-- > 0) {
        tagNumber = *pTagNumbers++;
        tagFound = FALSE;
        pFileTag = pFileTags;
        nFileTags = pFileImage->tagCount;
        while (nFileTags-- > 0) {
            if (pFileTag->number == tagNumber) {
                tagFound = TRUE;
                break;
                }
            else pFileTag++;
            }
        if (tagFound) {
            mallocSize += sizeof (ilFileTag) + 
                (pFileTag->length * _ilTagTypeItemSizes [pFileTag->type] + 3) & ~3;
            *ppTag++ = (ilFileTag *)pFileTag;
            }
        else *ppTag++ = (ilFileTag *)NULL;
        }

        /*  If no tags found; exit, else allocate "mallocSize" bytes, and for each found
            tag, point corresponding "ppTags" to spot in pTagAlloc that has the ilFileTag
            data, followed by the data itself, then long aligned.
                What is returned will look like:
                    ppTags [0] == null if tag not present, else ->:  
                                    tag data:                               byte offset
                                      number (same value as corresponding       0
                                              entry in pTagsNumbers)
                                      type                                      2
                                      nItems                                    4
                                      pItems (points to <items>, i.e. one       8
                                        long past this spot, or null if 
                                        "nItems" is zero (0).
                                      <items> - "nItems" occurrences of data   12
                                        depending on "type".
                    ppTags [1]->      <next tag, at offset 12 + ppTag[0]->nItems
                                        * size in bytes of one ppTag[0] item,
                                      as determined by "type".
        */
    if (mallocSize == 0) {
        IL_FREE (pFileTags);
        pFileImage->context->error = IL_OK;
        return (ilPtr)NULL;                                     /* EXIT */
        }
    pTagAlloc = (ilPtr)malloc (mallocSize);     /* caller uses free() to free this */
    if (!pTagAlloc)
        RFT_FATAL_ERROR (IL_ERROR_MALLOC)

    pTag = (ilFileTag *)pTagAlloc;
    nTags = nTagsInit;
    ppTag = ppTags;
    while (nTags-- > 0) {
        if (!*ppTag)
            ppTag++;                            /* tag not found, already null, next */
        else {                                  /* tag found */
            pFileTag = (ilTIFFTagPtr)*ppTag;    /* points to internal tag data */
            *ppTag++ = pTag;                    /* point to converted data, next tag */
            pTag->number = pFileTag->number;    /* copy from pFileTags to pTagAlloc */
            tagType = pTag->type = pFileTag->type;
            nItems = pTag->nItems = pFileTag->length;
            pTagData = (ilPtr)pTag + sizeof(ilFileTag);

            if (nItems == 0) {
                pTag->pItems = (ilPtr)NULL;
                pTag = (ilFileTag *)pTagData;   /* point pTag to spot for next tag */
                }
            else {
                pTag->pItems = pTagData;        /* data follows the tag */
                pTag = (ilFileTag *)(pTagData + /* next spot = past data, long-aligned */
                    ((nItems * _ilTagTypeItemSizes [tagType] + 3) & ~3));

                    /*  Byte flip the tag data to the spot pointed to by "pTagData",
                        i.e. right after the 12 bytes for the tag.  In TIFF, the data
                        is in the "offset" field tag if it fits in 4 bytes, else offset
                        points to the data in the file - seek and read it in that case.
                    */
                switch (tagType) {
                  case IL_TAG_BYTE:
                  case IL_TAG_ASCII:
                    if (nItems <= 4)
                        bcopy (pFileTag->data.chars, (char *)pTagData, nItems);
                    else {
                        IL_FLIP_LONG (bigEndian, pFileTag->data.chars, offset)
                        if (!IL_SEEK (pFile, offset) 
                         || !IL_READ (pFile, nItems, pTagData))
                            RFT_FATAL_ERROR (IL_ERROR_FILE_IO)
                        }
                    break;

                  case IL_TAG_SHORT:
                    if (nItems <= 2) {
                        for (i = 0; nItems > 0; nItems--, i += 2, pTagData += 2)
                            IL_FLIP_SHORT (bigEndian, pFileTag->data.chars + i,
                                           *((unsigned short *)pTagData))
                        }
                    else {
                        IL_FLIP_LONG (bigEndian, pFileTag->data.chars, offset)
                        if (!IL_SEEK (pFile, offset)
                         || !IL_READ (pFile, nItems<<1, pTagData))
                            RFT_FATAL_ERROR (IL_ERROR_FILE_IO)
                        for (; nItems > 0; nItems--, pTagData += 2)
                            IL_FLIP_SHORT (bigEndian, pTagData, *((unsigned short *)pTagData))
                        }
                    break;

                  case IL_TAG_LONG:
                    if (nItems == 1)
                        IL_FLIP_LONG (bigEndian, pFileTag->data.chars, *((INT32 *)pTagData))
                    else {
                        IL_FLIP_LONG (bigEndian, pFileTag->data.chars, offset)
                        if (!IL_SEEK (pFile, offset)
                         || !IL_READ (pFile, nItems<<2, pTagData))
                            RFT_FATAL_ERROR (IL_ERROR_FILE_IO)
                        for (; nItems > 0; nItems--, pTagData += 4)
                            IL_FLIP_LONG (bigEndian, pTagData, *((INT32 *)pTagData))
                        }
                    break;

                  case IL_TAG_RATIONAL:
                    IL_FLIP_LONG (bigEndian, pFileTag->data.chars, offset)
                    if (!IL_SEEK (pFile, offset) || !IL_READ (pFile, nItems<<3, pTagData))
                        RFT_FATAL_ERROR (IL_ERROR_FILE_IO)
                    while (nItems-- > 0) {
                        IL_FLIP_LONG (bigEndian, pTagData, *((INT32 *)pTagData))
                        pTagData += 4;
                        IL_FLIP_LONG (bigEndian, pTagData, *((INT32 *)pTagData))
                        pTagData += 4;
                        }
                    break;
                    }
                }   /* END nItems > 0 */
            }       /* END tag found */
        }           /* END each caller's tag */

        /*  Tags read succesfully: free pFileTags (local buffer); return pTagAlloc.
        */
    IL_FREE (pFileTags);
    pFileImage->context->error = IL_OK;
    return pTagAlloc;                                           /* EXIT */

        /*  Fatal error: free pFileTags, pTagAlloc if not null, return error code, null.
        */
RFTFatalError:
    if (pFileTags)
        IL_FREE (pFileTags);
    if (pTagAlloc)
        free (pTagAlloc);
    pFileImage->context->error = error;
    return (ilPtr)NULL;                                         /* EXIT */
}


        /*  --------------------- ilAddImageToList --------------------------------- */
        /*  Called when forming file image list.  "pPrevFileImage" is a pointer to the
            previous non-mask image in the file; null if pFileImage first non-mask read.
            pFileImage->imageType indicates whether image being added is 
            child/mask/mainImage; if child or mask, pPrevFileImage must be non-null.
        */
static void ilAddImageToList (
    ilFilePtr                   pFile,
    register ilFileImagePtr     pFileImage,
    ilFileImagePtr              pPrevFileImage
    )
{
    pFileImage->p.pNext = pFileImage->p.pPrev = 
        pFileImage->p.pChild = pFileImage->p.pMask = (ilFileImage)NULL;

    switch (pFileImage->imageType) {
      case childImage:
        if (pPrevFileImage->imageType == childImage)
             pPrevFileImage->p.pNext = (ilFileImage)pFileImage;
        else pPrevFileImage->p.pChild = (ilFileImage)pFileImage;
        break;

      case maskImage:
        pPrevFileImage->p.pMask = (ilFileImage)pFileImage;
        break;

        /*  Main image: if page # present, add to list in page order, else add to end */
      case mainImage:
        if (pFileImage->p.nPages < 0) {     /* no page #; add to end of list */
            if (pFileImage->p.pPrev = (ilFileImage)pFile->pFileImageTail)
                pFileImage->p.pPrev->pNext = (ilFileImage)pFileImage;
            else pFile->pFileImageHead = pFileImage;
            }
        else {
                /*  Have page #.  If list is empty or page # <= that of first element in
                    list, add pFileImage to front of list; otherwise find place in list
                    based on page # and insert image in that place.
                */
            pPrevFileImage = pFile->pFileImageHead;
            if (!pPrevFileImage || (pFileImage->p.page <= pPrevFileImage->p.page)) {
                pFileImage->p.pNext = (ilFileImage)pPrevFileImage;  /* add to front */
                pFileImage->p.pPrev = (ilFileImage)NULL;
                pFile->pFileImageHead = pFileImage;
                }
            else {      /* Add after pPrevFileImage w/ highest page # <= image's page # */
                while (pPrevFileImage->p.pNext
                 && (pPrevFileImage->p.pNext->page <= pFileImage->p.page)) {
                    pPrevFileImage = (ilFileImagePtr)pPrevFileImage->p.pNext;
                    }
                pFileImage->p.pNext = pPrevFileImage->p.pNext;
                pFileImage->p.pPrev = (ilFileImage)pPrevFileImage;
                pPrevFileImage->p.pNext = (ilFileImage)pFileImage;
                }
            }

            /*  Point next in list, or tail if this image is last, back to this image */
        if (pFileImage->p.pNext)
            pFileImage->p.pNext->pPrev = (ilFileImage)pFileImage;
        else pFile->pFileImageTail = pFileImage;
        break;
        }
}


        /*  --------------------- ilReadTagsAndAddImageToList ------------------------ */
        /*  Look at the tag values in the given file image "*pFileImage", and if the tags
            indicate a supported image, add it to the list of file images in "*pFile".
            If image is not supported (e.g unknown compression type) the image is freed.
                ppPrevFileImage must point to the ptr to the previous image read from the 
            file, or null if this is the first image; used for child and mask images.
                Return IL_OK if image valid, even if not supported; an error means
            the image is malformed (e.g. missing required tag).
        */
static ilError ilReadTagsAndAddImageToList (
    ilFilePtr                   pFile,
    ilFileImagePtr             *ppPrevFileImage,
    register ilFileImagePtr     pFileImage
    )
{
    /*  Indexes into "tagNumbers" below. NOTE: order is important, as tags are checked
        in the below order, and some tags default to values which must be before them.
            Also NOTE: check for type other than short/long and # items done  below!
    */
#define NEW_SUBFILE_TYPE     0
#define IMAGE_WIDTH          1
#define IMAGE_LENGTH         2
#define RESOLUTION_UNIT      3
#define X_RESOLUTION         4          /* follow RESOLUTION_UNIT */
#define Y_RESOLUTION         5          /* follow RESOLUTION_UNIT */
#define ROWS_PER_STRIP       6          /* follow IMAGE_WIDTH */
#define SAMPLES_PER_PIXEL    7
#define PLANAR_CONFIGURATION 8          /* follow SAMPLES_PER_PIXEL */
#define BITS_PER_SAMPLE      9          /* follow SAMPLES_PER_PIXEL */
#define FILL_ORDER           10
#define GROUP_3_OPTIONS      11         /* follow FILL_ORDER */
#define GROUP_4_OPTIONS      12         /* follow FILL_ORDER */
#define COMPRESSION          13         /* follow GROUP_3/4_OPTIONS, FILL_ORDER */
#define PREDICTOR            14         /* follow COMPRESSION */
#define PHOTOMETRIC_INTERPRETATION 15   /* follow COMP, BITS_PS, SAMPLES_PP, NEW_SFT */
#define PAGE_NUMBER          16         /* follow NEW_SFT */
#define JPEG_PROC            17         /* follow COMPRESSION */
#define YCBCR_COEFFICIENTS   18         /* follow PHOTO_INT */
#define YCBCR_SUBSAMPLING    19         /* follow PHOTO_INT */
#define YCBCR_POSITIONING    20         /* follow PHOTO_INT */
#define REFERENCE_BLACK_WHITE 21        /* follow PHOTO_INT */
#define PRIVATE_0            22         /* follow PHOTO_INT */

static unsigned short tagNumbers [] = {
    IL_TAG_NEW_SUBFILE_TYPE,
    IL_TAG_IMAGE_WIDTH,
    IL_TAG_IMAGE_LENGTH,
    IL_TAG_RESOLUTION_UNIT,
    IL_TAG_X_RESOLUTION,
    IL_TAG_Y_RESOLUTION,
    IL_TAG_ROWS_PER_STRIP,
    IL_TAG_SAMPLES_PER_PIXEL,
    IL_TAG_PLANAR_CONFIGURATION,
    IL_TAG_BITS_PER_SAMPLE,
    IL_TAG_FILL_ORDER,
    IL_TAG_GROUP_3_OPTIONS,
    IL_TAG_GROUP_4_OPTIONS,
    IL_TAG_COMPRESSION,
    IL_TAG_PREDICTOR,
    IL_TAG_PHOTOMETRIC_INTERPRETATION,
    IL_TAG_PAGE_NUMBER,
    IL_TAG_JPEG_PROC,
    IL_TAG_YCBCR_COEFFICIENTS,
    IL_TAG_YCBCR_SUBSAMPLING,
    IL_TAG_YCBCR_POSITIONING,
    IL_TAG_REFERENCE_BLACK_WHITE,
    IL_TAG_PRIVATE_0 };

#define AI_NTAGS            (sizeof(tagNumbers) / sizeof (unsigned short))
#define AIL_FATAL_ERROR(_error) {error = (_error);  goto AILFatalError; }

ilFileImageRelation        imageType;
ilFileTag                  *tag [AI_NTAGS];
ilBool                      present;
ilError                     error;
ilPtr                       pTagAlloc;
register unsigned long      value;
int                         i, tagIndex, nBits, resolutionUnit, fillOrder;
unsigned long               group3CompData;
unsigned long               group4CompData;
register ilFileTag         *pTag;
ilYCbCrInfo                *pYCbCr;     /* null if not YCbCr ; else -> des...YCbCr */

        /*  Init/zero des and format in *pFileImage.
        */
    IL_INIT_IMAGE_DES (&pFileImage->p.des)
    IL_INIT_IMAGE_FORMAT (&pFileImage->p.format)

        /*  Read the above file tags; fatal error if error returned.
        */
    pTagAlloc = ilReadFileTags ((ilFileImage)pFileImage, AI_NTAGS, tagNumbers, tag, 0);
    if (!pTagAlloc)
        return (pFileImage->context->error) ? 
            pFileImage->context->error : IL_ERROR_FILE_MISSING_TAG;

        /*  Loop thru the tags; set "present" true if tag found, set "value" to integer
            value of tag (long or short allowed - exceptions checked explicitly - else
            fatal error).
                Fix bug #0169: ignore tags with # items <= 0.
        */
    for (tagIndex = 0; tagIndex < AI_NTAGS; tagIndex++) {
        if ((pTag = tag [tagIndex]) && (pTag->nItems > 0)) {
            if (pTag->type == IL_TAG_SHORT)
                value = *((unsigned short *)pTag->pItems);
            else if (pTag->type == IL_TAG_LONG)
/* compatibility problem with long and unsigned long data fields */
                value = *((CARD32 *)pTag->pItems);
            else if ((tagIndex != X_RESOLUTION) 
                  && (tagIndex != Y_RESOLUTION)
                  && (tagIndex != YCBCR_COEFFICIENTS)
                  && (tagIndex != REFERENCE_BLACK_WHITE))
                    AIL_FATAL_ERROR (IL_ERROR_FILE_MALFORMED_TAG);
            if (pTag->nItems != 1) {
                if ((tagIndex != BITS_PER_SAMPLE) 
                 && (tagIndex != PAGE_NUMBER)
                 && (tagIndex != YCBCR_COEFFICIENTS)
                 && (tagIndex != YCBCR_SUBSAMPLING)
                 && (tagIndex != REFERENCE_BLACK_WHITE)
                 && (tagIndex != PRIVATE_0))
                    AIL_FATAL_ERROR (IL_ERROR_FILE_MALFORMED_TAG);
                }
            present = TRUE;
            }
        else present = FALSE;

            /*  Parse the tags and fill in pFileImage->format, des.  Init des first.
                See page 8 in the TIFF v5.0 spec for info on each tag type.
            */
        switch (tagIndex) {
          case NEW_SUBFILE_TYPE:
            if (present && (value & 4)) imageType = maskImage;
            else if (present && (value & 1)) imageType = childImage;
            else imageType = mainImage;
            break;

          case IMAGE_WIDTH:
            if (!present) AIL_FATAL_ERROR (IL_ERROR_FILE_MISSING_TAG);
            pFileImage->p.width = value;
            break;

          case IMAGE_LENGTH:
            if (!present) AIL_FATAL_ERROR (IL_ERROR_FILE_MISSING_TAG);
            pFileImage->p.height = value;
            break;

                /*  Resolution returned as dpi * 2, or "0" if not present.  The TIFF
                    spec says should default to 300 dpi if not present, but we let
                    the caller make that assumption.
                */
          case RESOLUTION_UNIT:
            if (!present) 
                resolutionUnit = 2;             /* default to inches */
            else resolutionUnit = value;
            break;

          case X_RESOLUTION:
          case Y_RESOLUTION:
            { long      denom;
            if (!present)
                value = 0;
            else {
                if (pTag->type != IL_TAG_RATIONAL)
                    AIL_FATAL_ERROR (IL_ERROR_FILE_MALFORMED_TAG);
/* compatibility problem with long and unsigned long data fields */
                value = *((INT32 *)pTag->pItems);
                denom = *((INT32 *)pTag->pItems + 1);
                if (resolutionUnit == 3)            /* centimeters */
                    value = (denom == 0) ? 0 : (double)value / 2.54 / (double)denom + 0.5;
                else                                /* not centimeters: assume inches */
                    value = (denom == 0) ? 0 : (double)value / (double)denom + 0.5;
                }
            if (tagIndex == X_RESOLUTION)
                 pFileImage->p.xRes = value;
            else pFileImage->p.yRes = value;
            }
            break;

          case ROWS_PER_STRIP:
            if (!present || (((long)value) < 0) || (value > pFileImage->p.height))
                 pFileImage->p.stripHeight = pFileImage->p.height;
            else pFileImage->p.stripHeight = value;
            break;

          case SAMPLES_PER_PIXEL:
            if (!present) value = 1;
            if (value <= 0) 
                AIL_FATAL_ERROR (IL_ERROR_FILE_TAG_VALUE);
            if (value > IL_MAX_SAMPLES)
                goto UnsupportedImage;
            pFileImage->p.des.nSamplesPerPixel = value;
            break;

                /*  Planar configuration: default to pixels unless more than one
                    sample, and value is present and is 2.
                */
          case PLANAR_CONFIGURATION:
            pFileImage->p.format.sampleOrder = ((pFileImage->p.des.nSamplesPerPixel != 1) 
                && present && (value == 2)) ? IL_SAMPLE_PLANES : IL_SAMPLE_PIXELS;
            break;

                /*  bits per sample: default to 1, or to first item (in "value") if
                    fewer items than samples per pixel.
                */
          case BITS_PER_SAMPLE:
            for (i = 0; i < pFileImage->p.des.nSamplesPerPixel; i++)
                pFileImage->p.format.nBitsPerSample [i] = (!present || (pTag->nItems < 1)) ?
                    1 : ((i > pTag->nItems - 1) ? value : *((unsigned short *)pTag->pItems + i));
            break;

                /*  FillOrder: used for class F only (TIFF compression "3")
                */
          case FILL_ORDER:
            fillOrder = (present && (value == 2)) ? IL_LSB_FIRST : IL_MSB_FIRST;
            break;

                /*  Parse group 3 TIFF options, assumed to be 0 if not present.
                    As per TIFF spec, dont support if any unrecognized bits are not 0.
                    Note that "group3CompData" is set to 0 below if comp = 2 (TIFF G3).
                */
          case GROUP_3_OPTIONS:
            group3CompData = IL_G3M_EOL_MARKERS;    /* EOL present if not "TIFF" G3 */
            if (fillOrder == IL_LSB_FIRST)
                group3CompData |= IL_G3M_LSB_FIRST;
            if (present) {
                if (value & ~0x7)
                    goto UnsupportedImage;      /* non-zero unhandled bits; unsupported */
                if (value & 1)
                    group3CompData |= IL_G3M_2D;
                if (value & 2)
                    group3CompData |= IL_G3M_UNCOMPRESSED;
                if (!(value & 4))
                    group3CompData |= IL_G3M_EOL_UNALIGNED;
                }
            break;

                /*  Group 4: set LSB first if FillOrder tag specifies it.  If 
                    Group4Options tag defined and any bit other than zero set (bit 0
                    is undefined) than consider image unsupported.  Bit one (1) specifies
                    "uncompressed" mode which is not supported.
                */
          case GROUP_4_OPTIONS:
            group4CompData = 0;
            if (fillOrder == IL_LSB_FIRST)
                group4CompData |= IL_G4M_LSB_FIRST;
            if (present) {
                if (value & ~1)
                    goto UnsupportedImage;      /* non-zero unhandled bits; unsupported */
                }
            break;

          case COMPRESSION:
            if (!present) value = 1;
            switch (value) {
              case 1: pFileImage->p.des.compression = IL_UNCOMPRESSED; break;

                /*  Comp codes 2 and 3 are both Group 3 - the difference is only in
                    what *pCompData describes.  For code 2 ("TIFF" group 3), the comp
                    data is 0 - the bits were defined that way.  For code 3, use what
                    was deciphered/defaulted by the group 3 options tag.
                */
              case 2: 
                group3CompData = 0;     /* "TIFF" G3 - default options, set to 0 */
              case 3:                   /* "true" G3 */
                pFileImage->p.des.compression = IL_G3;
                pFileImage->p.des.compInfo.g3.flags = group3CompData;
                break;

              case 4: pFileImage->p.des.compression = IL_G4;         
                pFileImage->p.des.compInfo.g4.flags = group4CompData;
                break;
              case 5: pFileImage->p.des.compression = IL_LZW; break;
              case 6: 
                pFileImage->p.des.compression = IL_JPEG;
                pFileImage->p.des.compInfo.JPEG.reserved = IL_JPEGM_RAW;
                pFileImage->p.des.compInfo.JPEG.process = 0;
                break;
              case 32773: pFileImage->p.des.compression = IL_PACKBITS; break;
              default: goto UnsupportedImage;
                }
            break;

                /*  NOTE: predictive LZW not supported yet !!!!
                */
          case PREDICTOR:
            if (present && (value != 1) && (pFileImage->p.des.compression == IL_LZW))
                goto UnsupportedImage;
            break;

                /*  If no photometric interpration, default based on (assumed) image
                    type: 1 sample: 1 bit/pixel: bitonal, white=0 if fax comp, else 1;
                                  > 1 bit/pixel: gray, black=0.
                          3 samples: RGB; else error.
                */
          case PHOTOMETRIC_INTERPRETATION:
            pYCbCr = (ilYCbCrInfo *)NULL;       /* unless a YCbCr image */
            if (!present) {
                if (pFileImage->p.des.nSamplesPerPixel == 1) {
                    if (pFileImage->p.format.nBitsPerSample[0] == 1) {
                        pFileImage->p.des.type = IL_BITONAL;
                        switch (pFileImage->p.des.compression) {
                          case IL_G3: case IL_G4:
                            pFileImage->p.des.blackIsZero = FALSE;
                            break;
                          default:
                            pFileImage->p.des.blackIsZero = TRUE;
                            break;
                            }
                        }
                    else {
                        pFileImage->p.des.type = IL_GRAY;
                        pFileImage->p.des.blackIsZero = TRUE;
                        }
                    }
                else if (pFileImage->p.des.nSamplesPerPixel == 3)
                    pFileImage->p.des.type = IL_RGB;
                else AIL_FATAL_ERROR (IL_ERROR_FILE_MISSING_TAG);
                }
            else switch (value) {   /* photo interpretation present; handle it */

                    /*  Bitonal or gray; 0 = white is 0, else black is zero. */
              case 0:
              case 1: 
                if (pFileImage->p.des.nSamplesPerPixel != 1)
                    AIL_FATAL_ERROR (IL_ERROR_FILE_TAG_VALUE);
                pFileImage->p.des.blackIsZero = (value == 1);
                pFileImage->p.des.type = (pFileImage->p.format.nBitsPerSample[0] == 1) ?
                    IL_BITONAL : IL_GRAY;
                break;

                    /*  RGB. */
              case 2:
                if (pFileImage->p.des.nSamplesPerPixel != 3)
                    AIL_FATAL_ERROR (IL_ERROR_FILE_TAG_VALUE);
                pFileImage->p.des.type = IL_RGB;
                break;

                    /*  Palette images: max of 8 bits */
              case 3:
                if (pFileImage->p.des.nSamplesPerPixel != 1)
                    AIL_FATAL_ERROR (IL_ERROR_FILE_TAG_VALUE);
                if (pFileImage->p.format.nBitsPerSample[0] > 8)
                    goto UnsupportedImage;
                pFileImage->p.des.type = IL_PALETTE;
                break;

                    /*  Mask image: is bitonal */
              case 4:
                if (pFileImage->p.des.nSamplesPerPixel != 1)
                    AIL_FATAL_ERROR (IL_ERROR_FILE_TAG_VALUE);
                pFileImage->p.des.type = IL_BITONAL;
                pFileImage->p.des.blackIsZero = FALSE;
                imageType = maskImage;
                break;

                    /*  YCbCr: set pYCbCr to non-null */
              case 6:
                if (pFileImage->p.des.nSamplesPerPixel != 3)
                    AIL_FATAL_ERROR (IL_ERROR_FILE_TAG_VALUE);
                pFileImage->p.des.type = IL_YCBCR;
                pYCbCr = &pFileImage->p.des.typeInfo.YCbCr;     /* point to YCbCr data */
                break;

                    /*  Others not supported */
              default: goto UnsupportedImage;
                }
            break;

          case PAGE_NUMBER:
            if (present) {
                if ((pTag->nItems != 2) || (pTag->type != IL_TAG_SHORT))
                    AIL_FATAL_ERROR (IL_ERROR_FILE_MALFORMED_TAG);
                pFileImage->p.page = value;
                pFileImage->p.nPages = *((unsigned short *)pTag->pItems + 1);
                }
            else pFileImage->p.page = pFileImage->p.nPages = -1;
            break;

                /*  JPEG tags: ignore if not JPEG compression */
          case JPEG_PROC:
            if (pFileImage->p.des.compression == IL_JPEG) {
                if (!present) AIL_FATAL_ERROR (IL_ERROR_FILE_MISSING_TAG);
                if (value != 1)         /* only baseline sequential supported! */
                    goto UnsupportedImage;
                }
            break;

                /*  YCbCr tags: ignored if not YCbCr image */
          case YCBCR_COEFFICIENTS:
            if (pYCbCr) {
                if (!present) {
                    pYCbCr->lumaRed = 2990;
                    pYCbCr->lumaGreen = 5870;
                    pYCbCr->lumaBlue = 1140;
                    }
                else {
/* compatibility problem with long or unsigned long data fields */
                    register INT32 *pLong = (INT32 *)pTag->pItems;
                    double          num, denom;
                    if ((pTag->nItems != 3) || (pTag->type != IL_TAG_RATIONAL))
                        AIL_FATAL_ERROR (IL_ERROR_FILE_MALFORMED_TAG);
                    num = *pLong++;  denom = *pLong++;
                    pYCbCr->lumaRed = num/denom * 10000;
                    num = *pLong++;  denom = *pLong++;
                    pYCbCr->lumaGreen = num/denom * 10000;
                    num = *pLong++;  denom = *pLong++;
                    pYCbCr->lumaBlue = num/denom * 10000;
                    }
                }
            break;

          case YCBCR_SUBSAMPLING:
            if (pYCbCr) {
                if (!present)
                    pYCbCr->sample[1].subsampleHoriz = pYCbCr->sample[1].subsampleVert = 2;
                else {
                    if ((pTag->nItems != 2) || (pTag->type != IL_TAG_SHORT))
                        AIL_FATAL_ERROR (IL_ERROR_FILE_MALFORMED_TAG);
                    pYCbCr->sample[1].subsampleHoriz = *((short *)pTag->pItems);
                    pYCbCr->sample[1].subsampleVert = *((short *)pTag->pItems + 1);
                    }
                        /* Set Y subsampling to (1,1); copy Cr to Cb: defined by TIFF */
                pYCbCr->sample[0].subsampleHoriz = pYCbCr->sample[0].subsampleVert = 1;
                pYCbCr->sample[2].subsampleHoriz = pYCbCr->sample[1].subsampleHoriz;
                pYCbCr->sample[2].subsampleVert = pYCbCr->sample[1].subsampleVert;
                }
            break;

                /*  Ignore positioning for now; set to zero (0) */
          case YCBCR_POSITIONING:
            if (pYCbCr)
                pYCbCr->positioning = 0;
            break;

            /*  ReferenceBlackWhite: if present, allow tag type to be "long" or "rational".
                "rational" is the correct tag type; "long" was written by Image 1 (the
                first Developer's Kit that supported JPEG), so support the old files.
            */
          case REFERENCE_BLACK_WHITE:
            if (pYCbCr) {
                register ilYCbCrSampleInfo  *pSample = pYCbCr->sample;
                int                         i;
                if (!present) {
                    pSample->refBlack = 0; pSample->refWhite = 255; pSample++;
                    pSample->refBlack = 0; pSample->refWhite = 255; pSample++;
                    pSample->refBlack = 0; pSample->refWhite = 255; pSample++;
                    }
                else {
/* compatibility problem with long and unsigned long data fields */
                    register INT32 *pLong = (INT32 *)pTag->pItems;
                    if (pTag->nItems != 6)
                        AIL_FATAL_ERROR (IL_ERROR_FILE_MALFORMED_TAG);
                    if (pTag->type == IL_TAG_LONG) {            /* old type */
                        for (i = 0; i < 3; i++, pSample++) {
                            pSample->refBlack = *pLong++; 
                            pSample->refWhite = *pLong++;
                            }
                        }
                    else if (pTag->type == IL_TAG_RATIONAL) {   /* correct TIFF 6.0 type */
                        long    num, denom;
                        for (i = 0; i < 3; i++, pSample++) {
                            num = *pLong++;
                            denom = *pLong++;
                            pSample->refBlack = (denom == 1) ? num :
                                (double)num / (double)denom + 0.5;
                            num = *pLong++;
                            denom = *pLong++;
                            pSample->refWhite = (denom == 1) ? num :
                                (double)num / (double)denom + 0.5;
                            }
                        }
                    else AIL_FATAL_ERROR (IL_ERROR_FILE_MALFORMED_TAG); /* bad tag type */
                    }
                }
            break;

                /*  HP IL private tag.  See /ilc/iltiffint.h for definition.
                    If "dithered palette" mask on, set levels and bit in flags in des.
                */
          case PRIVATE_0:
            if (present) {
                if (value & IL_TAG_P0_FLAG_DITHER_LEVELS) {
                    if (pFileImage->p.des.type == IL_PALETTE) {
                        register short *pLevel = ((short *)pTag->pItems + 1);
                        if ((pTag->nItems < 4) || (pTag->type != IL_TAG_SHORT))
                            AIL_FATAL_ERROR (IL_ERROR_FILE_MALFORMED_TAG);
                        pFileImage->p.des.flags |= IL_DITHERED_PALETTE;
                        pFileImage->p.des.typeInfo.palette.levels[0] = *pLevel++;
                        pFileImage->p.des.typeInfo.palette.levels[1] = *pLevel++;
                        pFileImage->p.des.typeInfo.palette.levels[2] = *pLevel++;
                        }
                    }
                }
            break;

            }   /* END switch tag index */
        }       /* END each tag */

        /*  All the tags are parsed and valid so far.  Finish filling in *pFileImage;
            # levels is inferred from # bits; only supporting byte/sample for now.
        */
    for (i = 0; i < pFileImage->p.des.nSamplesPerPixel; i++) {
        nBits = pFileImage->p.format.nBitsPerSample [i];
        if (nBits <= 0)
            AIL_FATAL_ERROR (IL_ERROR_FILE_TAG_VALUE);      /* error; invalid # bits */
        if (nBits > 8)
            goto UnsupportedImage;                          /* not error, not supported */
        pFileImage->p.des.nLevelsPerSample [i] = 1 << nBits;
        }
    pFileImage->p.format.byteOrder = IL_MSB_FIRST;          /* arbitrary! */
    pFileImage->p.format.rowBitAlign = 8;                   /* by TIFF definition */

        /*  If mask image: must be bitonal or discard it. */
    if (imageType == maskImage) {
        register ilFileImagePtr pPrevFileImage = *ppPrevFileImage;
        if (!pPrevFileImage                                     /* no image before it */
         || pPrevFileImage->p.pMask                             /* already has mask */
         || (pFileImage->p.des.nSamplesPerPixel != 1)
         || (pFileImage->p.format.nBitsPerSample[0] != 1))
            goto UnsupportedImage;                              /* reject image */
        }

        /*  If a child image and no previous image, make a main image and add it. */
    if ((imageType == childImage) && !*ppPrevFileImage)
        imageType = mainImage;

        /*  Add image to list, make previous image unless a mask image */
    pFileImage->imageType = imageType;                      /* child/mask/mainImage */
    ilAddImageToList (pFile, pFileImage, *ppPrevFileImage);
    if (imageType != maskImage)
        *ppPrevFileImage = pFileImage;

    free (pTagAlloc);                                       /* free() as client would */
    return IL_OK;                                           /* success; EXIT */

        /*  Error: free pTagAlloc, but not pFileImage - caller does that. Return error.
            Use free(), not IL_FREE(), to free pTagAlloc, just like caller should.
        */
AILFatalError:
    if (pTagAlloc)
        free (pTagAlloc);
    return error;

        /*  Image is not supported; not an error, but don't add to list - free image.
        */
UnsupportedImage:
    IL_FREE (pFileImage);
    return IL_OK;
}


        /*  ------------------------ ilListFileImages ------------------------------- */
        /*  Public function: see spec.
        */
ilFileImage ilListFileImages (
    ilFile              file,
    unsigned long       mustBeZero
    )
{
ilError                 error;
long                    imageSequenceNumber;
ilByte                  fourBytes [4];
ilFileOffset            IFDOffset;
ilFileImagePtr          pFileImage, pFileImageHead, pPrevFileImage;
ilTIFFHeader            header;
register ilFilePtr      pFile;
register ilBool         bigEndian;

        /*  Check if image list already built - if so, return ptr to it.
        */
    pFile = (ilFilePtr)file;
    if (mustBeZero != 0) {
        pFile->o.p.context->error = IL_ERROR_PAR_NOT_ZERO;
        return (ilFileImage)NULL;                               /* EXIT */
        }
    if (pFile->haveImageList) {
        pFile->o.p.context->error = IL_OK;
        return (ilFileImage)pFile->pFileImageHead;              /* EXIT */
        }

    ilFreeFileList (pFile);                 /* remove potential invalid image list */

        /*  Dont have image list - read file and build the list.  When done, 
            pFile->IFDTailPtrOffset points to the "next IFD" ptr which is null, i.e.
            the next IFD ptr for the last image in the file.  For a file with no images
            that would be the ptr in the file header.
                First read header to get ptr to first image in file.
        */
#ifdef DTLIB
    /* Used when this module is compiled by the DtHelp library build */
    if (_DtGrSeek (pFile->stream, pFile->offset, 0)) {
#else
    if (fseek (pFile->stream, pFile->offset, 0)) {
#endif /* DTLIB */
        pFile->o.p.context->error = IL_ERROR_FILE_IO;
        return (ilFileImage)NULL;
        }
#ifdef DTLIB
    /* Used when this module is compiled by the DtHelp library build */
    if (!_DtGrRead ((char *)&header, sizeof (header), 1, pFile->stream)) {
#else
    if (!fread ((char *)&header, sizeof (header), 1, pFile->stream)) {
#endif /* DTLIB */
        pFile->o.p.context->error = IL_ERROR_FILE_IO;
        return (ilFileImage)NULL;
        }
    IL_FLIP_LONG (pFile->bigEndian, header.IFDHeadOffset, IFDOffset);

    pFile->haveImageList = TRUE;
    pFile->IFDTailPtrOffset = 4;            /* point to first IFD ptr in header */
    imageSequenceNumber = 0;
    bigEndian = pFile->bigEndian;
    pPrevFileImage = (ilFileImagePtr)NULL;

        /*  Overview: read the tag count, allocate room for tags (pBeginTagBuffer) and
            read the unflipped tags into it.  Allocate space for converted tags 
            (pFile->pTags) and copy/flip unflipped tags into it.
            After the tags comes the offset to next IFD; read and flip it.
        */
    while (IFDOffset) {
        imageSequenceNumber++;
        pFileImage = (ilFileImagePtr)IL_MALLOC_ZERO (sizeof (ilFileImageRec));
        if (!pFileImage) {
            error = IL_ERROR_MALLOC;
            goto LFIFatalError;
            }

        pFileImage->p.file = (ilFile)pFile;
        pFileImage->context = pFile->o.p.context;

            /*  Read and flip the tagCount, @IFDOffset.  Point tagOffset at next location.
            */
        if (!IL_SEEK (pFile, IFDOffset)
         || !IL_READ (pFile, 2, fourBytes)) {
            error = IL_ERROR_FILE_IO;
            goto LFIFatalError;
            }
        IL_FLIP_SHORT (bigEndian, fourBytes, pFileImage->tagCount);
        pFileImage->tagOffset = IFDOffset + 2;

            /*  Add file image to list, if supported.  An error is returned only if
                the file image is malformed; not if it is just unsupported.
            */
        if (error = ilReadTagsAndAddImageToList (pFile, &pPrevFileImage, pFileImage))
            goto LFIFatalError;             /* fatal error returned; EXIT */

            /*  Read and flip offset to next IFD, at end of tags; save in pFile. */
        IFDOffset += pFileImage->tagCount * IL_TAG_SIZE + 2;
        pFile->IFDTailPtrOffset = IFDOffset;
        if (!IL_SEEK (pFile, IFDOffset)
         || !IL_READ (pFile, 4, fourBytes)) {
            error = IL_ERROR_FILE_IO;
            goto LFIFatalError;
            }
        IL_FLIP_LONG (bigEndian, fourBytes, IFDOffset);
        }   /* END one IFD */

        /*  Images accessed successfully; return ok; */
    pFile->o.p.context->error = IL_OK;
    return (ilFileImage)pFile->pFileImageHead;                      /* EXIT */

        /*  Fatal error: dispose file list and pFileImage if non-null, return "error",
            and sequence # of image in file that provoked the error.
        */
LFIFatalError:
    if (pFileImage)
        IL_FREE (pFileImage);
    ilFreeFileList (pFile);
    pFile->o.p.context->error = error;
    pFile->o.p.context->errorInfo = imageSequenceNumber;
    return (ilFileImage)NULL;
}


        /*  ------------------------ ilConnectFile -------------------------------- */
        /*  Public function: see spec.
            Locate a TIFF file at "offset" within "file", and return a handle used
            to access this file, or return null and an error code.
        */
ilFile ilConnectFile (
    ilContext           context,
#ifdef DTLIB
    /* Used when this module is compiled by the DtHelp library build */
    _DtGrStream         *stream,
#else
    FILE                *stream,
#endif /* DTLIB */
    long                offset,
    unsigned long       mustBeZero
    )
{
ilBool                  bigEndian;
ilFilePtr               pFile;
ilTIFFHeader            header;

    if (mustBeZero != 0) {
        context->error = IL_ERROR_PAR_NOT_ZERO;
        return (ilPipe)NULL;
        }

        /*  Read what should be a TIFF file header at "offset" within "file".
            Verify that byte order is "MM" (bigEndian) or "II" (!bigEndian) or error.
            Verify the answer to life, the universe, and everything ...
        */
#ifdef DTLIB
    /* Used when this module is compiled by the DtHelp library build */
    if (_DtGrSeek (stream, offset, 0)) {   /* nonzero means error for fseek */
#else
    if (fseek (stream, offset, 0)) {       /* nonzero means error for fseek */
#endif /* DTLIB */
        context->error = IL_ERROR_FILE_IO;
        return (ilFile)NULL;
        }
#ifdef DTLIB
    /* Used when this module is compiled by the DtHelp library build */
    if (!_DtGrRead ((char *)&header, sizeof (header), 1, stream)) {
#else
    if (!fread ((char *)&header, sizeof (header), 1, stream)) {
#endif /* DTLIB */
        context->error = IL_ERROR_FILE_IO;
        return (ilFile)NULL;
        }
    if ((header.byteOrder[0] == 'M') && (header.byteOrder[1] == 'M'))
        bigEndian = TRUE;
    else if ((header.byteOrder[0] == 'I') && (header.byteOrder[1] == 'I'))
        bigEndian = FALSE;
    else {
        context->error = IL_ERROR_FILE_NOT_TIFF;
        return (ilFile)NULL;
        }
    if (!(bigEndian && (header.version[0] == 0) && (header.version[1] == 42))
     && !(!bigEndian && (header.version[0] == 42) && (header.version[1] == 0))) {
        context->error = IL_ERROR_FILE_NOT_TIFF;
        return (ilFile)NULL;
        }

        /*  File seems to be a valid TIFF file.  Create an ilFileRec object, init it
            and return ptr to it.
        */
    pFile = (ilFilePtr)_ilCreateObject (context, IL_FILE, ilFreeFileList, sizeof(ilFileRec));
    if (!pFile)
        return (ilFile)NULL;                        /* context->error already set */

    pFile->stream = stream;
    pFile->offset = offset;
    pFile->bigEndian = bigEndian;
    pFile->pFileImageHead = pFile->pFileImageTail = (ilFileImagePtr)NULL;
    pFile->haveImageList = FALSE;

    context->error = IL_OK;
    return (ilFile)pFile;
}

