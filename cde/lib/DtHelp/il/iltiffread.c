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
/* $XConsortium: iltiffread.c /main/5 1996/06/19 12:20:39 ageorge $ */
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

    /*  /ilc/iltiffread.c : Code for ilReadFileImage(); add a producer to the pipe
        to read from a TIFF file.  See also /ilc/iltiff.c and /ilc/iltiffwrite.c .
    */

#include <stdlib.h>

#include "iltiffint.h"
#include "ilpipelem.h"
#include "ilcodec.h"
#include "ilerrors.h"
#include "ilutiljpeg.h"

typedef struct {
            /*  Data set into private when element added */
    ilFilePtr           pFile;              /* ptr to IL file being read */
    ilFileImagePtr      pFileImage;         /* ptr to file image being read */
    unsigned int        compression;        /* compression type of data being read */
    long                stripHeight;        /* # lines per strip except for last strip */
    unsigned short     *pPalette;           /* ptr to palette; filled in by Init() */
    ilPtr               pCompData;          /* ptr to comp data; filled in by Init() */
    ilBool              initDone;           /* if true, Init() has been called */

            /*  Data set into private by ilReadFileInit(). */
    ilPtr               pTagAlloc;          /* ptr to allocate file tags */
    ilImagePlaneInfo   *pDstImagePlane;     /* ptr to plane 0 of dst image */
    long                nStrips;            /* total # of strips to read */
    long                nStripsLeft;        /* # of strips left to read */
    long                lastStripHeight;    /* # lines in last strip */
    CARD32             *pOffsetsInit;       /* ptr to strip offsets */
    CARD32             *pOffsets;           /* ptr to next strip offset to read */
    ilBool              shortOffsets;       /* offsets are short (else long) */
    CARD32             *pByteCountsInit;    /* ptr to strip byte counts or null */
    CARD32             *pByteCounts;        /* ptr to next count to read or null */
    ilBool              shortByteCounts;    /* byte counts short (else long) */
    unsigned long       nStripBytes;        /* if !pByteCounts: # bytes per strip */
    unsigned long       nLastStripBytes;    /* if !pByteCounts: # bytes in last strip */
    } ilReadFilePrivRec, *ilReadFilePrivPtr;



        /*  --------------------- ilReadJPEGTags -------------------------- */
        /*  Called by ilReadFileInit() to read JPEG tag data, for JPEG-compressed file.
        */
static ilError ilReadJPEGTags (
    ilFilePtr           pFile,              /* file to read */
    int                 nComps,             /* # of samples/components in this image */
    long               *pQOffsets,          /* array of nComps file offsets to tables */
    long               *pDCOffsets,
    long               *pACOffsets,
    register ilJPEGData *pCompData          /* ptr to structure to fill in */
    )
{
    register int        i, c, nBytes;
    register ilJPEGSampleData *pSample;
    register ilPtr      pTable;
    ilByte              buffer[16];         /* buffer for DC/AC table prefix (# codes) */

        /*  Use sample/component index as table index; load tables based on sample */
    for (c = 0, pSample = pCompData->sample; c < IL_MAX_SAMPLES; c++, pSample++) {
        pSample->QTableIndex = c;
        pSample->DCTableIndex = c;
        pSample->ACTableIndex = c;
        pSample->reserved = 0;
        }

        /*  Load "nComps" of each type of table into slot indexed by sample #. */
    for (c = 0; c < nComps; c++, pQOffsets++, pDCOffsets++, pACOffsets++) {

            /*  Read Q table into 64 byte malloc'd block and point to it */
        if (!(pTable = (ilPtr)IL_MALLOC (64)))
            return IL_ERROR_MALLOC;
        pCompData->QTables[c] = pTable;
        if (!IL_SEEK (pFile, *pQOffsets) || !IL_READ (pFile, 64, pTable))
            return IL_ERROR_FILE_IO;

            /*  Read DC table: first 16 bytes are # of codes for each length (1..16);
                the codes follow in bytes, "size" = sum of values of first 16 bytes.
                Malloc space and read in "size" + 16 bytes.
            */
        if (!IL_SEEK (pFile, *pDCOffsets) || !IL_READ (pFile, 16, buffer))
            return IL_ERROR_FILE_IO;
        for (i = 0, nBytes = 16; i < 16; i++)
            nBytes += buffer[i];
        if (!(pTable = (ilPtr)IL_MALLOC (nBytes)))
            return IL_ERROR_MALLOC;
        pCompData->DCTables[c] = pTable;
        if (!IL_SEEK (pFile, *pDCOffsets) || !IL_READ (pFile, nBytes, pTable))
            return IL_ERROR_FILE_IO;

            /*  Read AC table: same as DC table */
        if (!IL_SEEK (pFile, *pACOffsets) || !IL_READ (pFile, 16, buffer))
            return IL_ERROR_FILE_IO;
        for (i = 0, nBytes = 16; i < 16; i++)
            nBytes += buffer[i];
        if (!(pTable = (ilPtr)IL_MALLOC (nBytes)))
            return IL_ERROR_MALLOC;
        pCompData->ACTables[c] = pTable;
        if (!IL_SEEK (pFile, *pACOffsets) || !IL_READ (pFile, nBytes, pTable))
            return IL_ERROR_FILE_IO;
        }

    return IL_OK;
}


        /*  --------------------- ilReadFileInit -------------------------- */
        /*  Init() function for ilReadFileImage().  Read the strip offsets (required)
            and strip byte counts (required if compressed) tags.
        */
static ilError ilReadFileInit (
    ilPtr              pPrivate,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
register ilReadFilePrivPtr pPriv = (ilReadFilePrivPtr)pPrivate;
ilFileImagePtr          pFileImage;
ilError                 error;
register ilFileTag     *pTag;

    /*  Data for tags to read: "numbers" is the array of tag ids, "nTags" in length.
        "?Index" vars are indices into "numbers" and returned data "tags".
    */
#define                 MAX_NTAGS 10        /* max # of tags read at once (plus slop) */
int                     nTags;              /* # of tags to read */
unsigned short          numbers[MAX_NTAGS]; /* tag ids to read */
ilFileTag              *tags [MAX_NTAGS];   /* ptr to returned data */
int                     stripOffsetsIndex, stripByteCountsIndex, colorMapIndex,
                        QTablesIndex, DCTablesIndex, ACTablesIndex, softwareIndex,
                        restartIndex;
#define ADD_TAG(_index, _tagid) { \
    _index = nTags++;             \
    numbers[_index] = _tagid;     \
    }


        /*  If this function has already been called, exit.  All of the tag data is
            read in here and kept around, so if pipe is being re-executed there is no
            need to read it again - merely reset current strip ptrs to start of data.
        */
    if (pPriv->initDone) {
        pPriv->pOffsets = pPriv->pOffsetsInit;
        pPriv->pByteCounts = pPriv->pByteCountsInit;
        pPriv->nStripsLeft = pPriv->nStrips;
        return IL_OK;
        }

        /*  Read the strip tags, which reads in the actual strips.  Offsets (file offsets
            to strips) are required; read the byte counts only if the image is compressed.
            Read ColorMap tag if a palette image; read JPEG tags if JPEG compressed.
        */
    pFileImage = pPriv->pFileImage;
    nTags = 0;
    ADD_TAG (stripOffsetsIndex, IL_TAG_STRIP_OFFSETS)

    if (pPriv->pPalette)
        ADD_TAG (colorMapIndex, IL_TAG_COLOR_MAP)

    if (pPriv->compression != IL_UNCOMPRESSED)
        ADD_TAG (stripByteCountsIndex, IL_TAG_STRIP_BYTE_COUNTS)

    if (pPriv->compression == IL_JPEG) {
        ADD_TAG (stripByteCountsIndex, IL_TAG_STRIP_BYTE_COUNTS)
        ADD_TAG (QTablesIndex, IL_TAG_JPEG_Q_TABLES)
        ADD_TAG (DCTablesIndex, IL_TAG_JPEG_DC_TABLES)
        ADD_TAG (ACTablesIndex, IL_TAG_JPEG_AC_TABLES)
        ADD_TAG (softwareIndex, IL_TAG_SOFTWARE)            /* for bug; see below */
        ADD_TAG (restartIndex, IL_TAG_JPEG_RESTART_INTERVAL)
        }

    pPriv->pTagAlloc = ilReadFileTags ((ilFileImage)pFileImage, nTags, numbers, tags, 0);
    if (pFileImage->context->error)
        return pFileImage->context->error;

    pPriv->pDstImagePlane = pDstImage->plane;

        /*  Set pOffsets, ptr to strip offsets (required); determine if short/long. */
    if (!(pTag = tags [stripOffsetsIndex]))
        return IL_ERROR_FILE_STRIP_TAGS;
    pPriv->nStripsLeft = pPriv->nStrips = pTag->nItems;
    pPriv->pOffsets = pPriv->pOffsetsInit = (CARD32 *)pTag->pItems;
    if (pTag->type == IL_TAG_SHORT)
        pPriv->shortOffsets = TRUE;
    else if (pTag->type == IL_TAG_LONG)
        pPriv->shortOffsets = FALSE;
    else return IL_ERROR_FILE_STRIP_TAGS;

        /*  Validate nStrips with stripHeight and height of image. */
    if (pPriv->nStrips <= 0)
        return IL_ERROR_FILE_STRIP_TAGS;
    pPriv->lastStripHeight = pFileImage->p.height - 
                                (pPriv->nStrips-1) * pFileImage->p.stripHeight;
    if ((pPriv->lastStripHeight <= 0) 
     || (pPriv->lastStripHeight > pFileImage->p.stripHeight))
        return IL_ERROR_FILE_STRIP_TAGS;

        /*  If image is uncompressed, ignore strip byte counts - can calculate them,
            and it is safer (have seen uncompressed files with bad strip byte counts, 
            and perhaps they should be rejected, but we can read them, so why not?):
            set nStripBytes based on dst row bytes, nLastStripBytes for last strip.
            If compress: Strip byte counts must exist and have same # items as offsets.
        */
    if (pPriv->compression == IL_UNCOMPRESSED) {
        unsigned long dstRowBytes = pPriv->pDstImagePlane->nBytesPerRow;
        pPriv->pByteCounts = pPriv->pByteCountsInit = (CARD32 *)NULL;
        pPriv->nStripBytes = pPriv->stripHeight * dstRowBytes;
        pPriv->nLastStripBytes = pPriv->lastStripHeight * dstRowBytes;
        }
    else {
        pTag = tags [stripByteCountsIndex];
        if (!pTag)
            return IL_ERROR_FILE_STRIP_TAGS;
        pPriv->pByteCounts = pPriv->pByteCountsInit = (CARD32 *)pTag->pItems;
        if (pTag->nItems != pPriv->nStrips)
            return IL_ERROR_FILE_STRIP_TAGS;
        if (pTag->type == IL_TAG_SHORT)
            pPriv->shortByteCounts = TRUE;
        else if (pTag->type == IL_TAG_LONG)
            pPriv->shortByteCounts = FALSE;
        else return IL_ERROR_FILE_STRIP_TAGS;
        }

        /*  ColorMap (palette) must be present if a palette image (pPriv->pPalette).
            Copy TIFF palette (size = 2 ** nBits)  to pPriv->pPalette (size = 3 * 256).
        */
    if (pPriv->pPalette) {
        register unsigned short *pPalette, *pFilePalette;
        register int             nPaletteEntries, i;

        pTag = tags [colorMapIndex];
        if (!pTag)
            return IL_ERROR_FILE_MISSING_TAG;
        nPaletteEntries = 1 << pPriv->pFileImage->p.format.nBitsPerSample[0];
        if (pTag->nItems != (3 * nPaletteEntries))
            return IL_ERROR_FILE_MALFORMED_TAG;
        pPalette = pPriv->pPalette;
        pFilePalette = (unsigned short *)pTag->pItems;
        for (i = 0; i < nPaletteEntries; i++, pPalette++, pFilePalette++) {
            pPalette[0]   = pFilePalette [0];
            pPalette[256] = pFilePalette [nPaletteEntries];
            pPalette[512] = pFilePalette [nPaletteEntries << 1];
            }
        }

        /*  If a JPEG file read JPEG tags and load Q/DC/AC tables into pCompData.
            JPEG table tags must be present and # items = # components/samples.
        */
    if (pPriv->compression == IL_JPEG) {
        int             nSamples = pFileImage->p.des.nSamplesPerPixel;
        ilFileTag      *pQTag, *pDCTag, *pACTag, *pTag;

        pQTag  = tags[QTablesIndex];
        pDCTag = tags[DCTablesIndex];
        pACTag = tags[ACTablesIndex];
        if (!pQTag || !pDCTag || !pACTag)
            return IL_ERROR_FILE_MISSING_TAG;
        if ((pQTag->nItems != nSamples)  || (pQTag->type != IL_TAG_LONG)
         || (pDCTag->nItems != nSamples) || (pDCTag->type != IL_TAG_LONG)
         || (pACTag->nItems != nSamples) || (pACTag->type != IL_TAG_LONG))
            return IL_ERROR_FILE_MALFORMED_TAG;

        if (error = ilReadJPEGTags (pPriv->pFile, nSamples, (long *)pQTag->pItems, 
                                    (long *)pDCTag->pItems, (long *)pACTag->pItems, 
                                    (ilJPEGData *)pPriv->pCompData))
            return error;

            /*  If restartInterval tag present, store value else default to 0 */
        if ((pTag = tags[restartIndex]) && (pTag->type == IL_TAG_SHORT))
            ((ilJPEGData *)pPriv->pCompData)->restartInterval = *((short *)pTag->pItems);
        else ((ilJPEGData *)pPriv->pCompData)->restartInterval = 0;

            /*  Handle 2 bugs in older versions of IL (v2.0 and earlier).  These versions
                always wrote in "JIF mode", as a single strip.  The read code always
                read using the JIF tag instead of the strip offsets/byte counts, because
                the decompression code could only handle JIF data.  
                    "count" bug: in versions 2.0 and earlier. The (single) strip byte 
                count was too small; usually by 8 for a single sample image (gray) or 14 
                for multi-sample (RGB or YCbCr); larger for some baselevels.
                Then work around the count bug by reading until the EOI marker is found.
                The data length is then all bytes up but not including the EOI marker.
                    "offset" bug: the strip offset is 3 bytes too small.  Fix by adding
                3 to the strip offset.
                    The bugs are in files written by IL versions 1.1 (Image Developer's
                Kit release; first to support JPEG) or 2.0 (Image 2).
                So if the first "n" chars of the software tag are:
                    "HP IL v1.1"  (n = 10)  count and offset bugs
                    "HP IL v2.0"  (n = 10)  count bug
                    "HP IL v 2.0" (n = 11)  count bug
            */
        {   ilBool          hasCountBug, hasOffsetBug;
            ilFileTag      *pSoftwareTag;
            char           *pName;

            hasCountBug = FALSE;
            hasOffsetBug = FALSE;
            if (pSoftwareTag = tags[softwareIndex]) {
                pName = (char *)pSoftwareTag->pItems;
                if ((pSoftwareTag->nItems >= 7) && (pName[0] == 'H') && (pName[1] == 'P') 
                 && (pName[2] == ' ') && (pName[3] == 'I') && (pName[4] == 'L') 
                 && (pName[5] == ' ') && (pName[6] == 'v')) {
                    if ((pSoftwareTag->nItems >= 10) && (pName[7] == '1') 
                     && (pName[8] == '.') && (pName[9] == '1'))
                        hasCountBug = hasOffsetBug = TRUE;
                    else if ((pSoftwareTag->nItems >= 10)
                     && (pName[7] == '2') && (pName[8] == '.') && (pName[9] == '0'))
                        hasCountBug = TRUE;
                    else if ((pSoftwareTag->nItems >= 11) && (pName[7] == ' ') 
                     && (pName[8] == '2') && (pName[9] == '.') && (pName[10] == '0'))
                        hasCountBug = TRUE;
                    }
                }

            if (hasOffsetBug)
                *pPriv->pOffsets += 3;

            if (hasCountBug) {
#               define JPEGM_FIRST_BYTE 0xFF    /* first byte of all JPEG markers */
#               define JPEGM_EOI        0xD9    /* End Of Image */
                ilByte                  value;
                long                    nStripBytes;

                nStripBytes = *pPriv->pByteCounts;
                if (!IL_SEEK (pPriv->pFile, *pPriv->pOffsets + nStripBytes))
                    return (IL_ERROR_COMPRESSED_DATA);
                do {                        /* get a marker */
                    do {
                        if (!IL_READ (pPriv->pFile, 1, &value))
                            return IL_ERROR_COMPRESSED_DATA;
                        nStripBytes++;
                        } while (value != JPEGM_FIRST_BYTE);
                    do {
                        if (!IL_READ (pPriv->pFile, 1, &value))
                            return IL_ERROR_COMPRESSED_DATA;
                        nStripBytes++;
                        } while (value == JPEGM_FIRST_BYTE);
                    } while (!value);          /* skip "0" after ff */

                if (value != JPEGM_EOI)
                    return IL_ERROR_COMPRESSED_DATA;

                    /*  nStripBytes now includes marker; subtract 2 to remove marker */
                nStripBytes -= 2;
                *pPriv->pByteCounts = nStripBytes;
                }
        }

        }   /* END JPEG */

    pPriv->initDone = TRUE;             /* avoid re-reading; e.g. palette data */
    return IL_OK;
}


        /*  ------------------------ ilReadFileDestroy -------------------------------- */
        /*  Destroy() function for ilReadFileImage().  Calls ilDestroyObject() on the
            file, whose refCount was inc'd when element added.  This prevents the file
            from being destroyed until the pipe element is destroyed.
                Free the palette space, compression data and tag malloc space if present.
        */
static ilError ilReadFileDestroy (
    ilPtr  pPrivate
    )
{
    ilReadFilePrivPtr pPriv = (ilReadFilePrivPtr)pPrivate;
    ilDestroyObject ((ilObject)pPriv->pFile);

    if (pPriv->pPalette)
        IL_FREE (pPriv->pPalette);

    if (pPriv->pCompData && (pPriv->compression == IL_JPEG)) {
        _ilJPEGFreeTables ((ilJPEGData *)pPriv->pCompData);
        IL_FREE (pPriv->pCompData);
        }

    if (pPriv->pTagAlloc && pPriv->initDone)
        free (pPriv->pTagAlloc);        /* not IL_FREE(); act like client */

    return IL_OK;
}



        /*  --------------------- ilReadFileExecute -------------------------- */
        /*  Execute() for ilReadFileImage().  Read one strip (or whole image if no 
            strips) out to the dst image.  If compressed, special care required.
            This is a producer; signal last strip if no more strips to read.
        */
static ilError ilReadFileExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
register ilReadFilePrivPtr  pPriv;
unsigned long               nBytes, offset;
ilPtr                       pDst;
ilImagePlaneInfo           *pDstImagePlane;


        /*  Get "offset", position of strip in file, and "nBytes" to read: from strip
            byte counts if present (must be if compressed), else use calculated values.
        */
    pPriv = (ilReadFilePrivPtr)pData->pPrivate;
    pPriv->nStripsLeft--;
    if (pPriv->shortOffsets) {
        offset = *((unsigned short *)pPriv->pOffsets);
        pPriv->pOffsets = (CARD32 *)((ilPtr)pPriv->pOffsets + 2);
        }
    else offset = *pPriv->pOffsets++;
    if (pPriv->pByteCounts) {
        if (pPriv->shortByteCounts) {
            nBytes = *((unsigned short *)pPriv->pByteCounts);
            pPriv->pByteCounts = (CARD32 *)((ilPtr)pPriv->pByteCounts + 2);
            }
        else nBytes = *pPriv->pByteCounts++;
        }
    else nBytes = (pPriv->nStripsLeft <= 0) ? pPriv->nLastStripBytes : pPriv->nStripBytes;

        /*  If reading a compressed image, read into "pPixels + srcOffset" (ignore
            dstLine); make sure room for "nBytes"; (re)alloc buffer if not room.
                Pass "nBytes" written on to next pipe element.
        */
    pDstImagePlane = pPriv->pDstImagePlane;
    if (pPriv->compression != IL_UNCOMPRESSED) {
        long         dstOffset;
        dstOffset = *pData->compressed.pDstOffset;
        if ((dstOffset + nBytes) > pDstImagePlane->bufferSize)
            if (!_ilReallocCompressedBuffer (pData->pDstImage, 0, dstOffset + nBytes))
                return IL_ERROR_MALLOC;                 /* error, EXIT */
        pDst = pDstImagePlane->pPixels + dstOffset;
        *pData->compressed.pNBytesWritten = nBytes;
        }
    else pDst = pDstImagePlane->pPixels + dstLine * pDstImagePlane->nBytesPerRow;

        /*  Seek to offset and read nBytes into pDst. */
    if (!IL_SEEK (pPriv->pFile, offset)
     || !IL_READ (pPriv->pFile, nBytes, pDst))
        return (IL_ERROR_FILE_IO);

        /*  Set # of lines written - strip height unless last strip (return flag). */
    if (pPriv->nStripsLeft <= 0) {
        *pNLines = pPriv->lastStripHeight;
        return IL_ERROR_LAST_STRIP;
        }
    else {
        *pNLines = pPriv->stripHeight;
        return IL_OK;
        }
}


        /*  ---------------------- ilReadFileImage ---------------------------------- */
        /*  Public function: see spec.
            Adds a producer to the given pipe to read the given fileImage.
        */
ilBool ilReadFileImage (
    ilPipe              pipe,
    ilFileImage         fileImage,
    ilRect             *pSrcRect,
    unsigned long       mustBeZero
    )
{
ilDstElementData        dstData;
register ilFileImagePtr pFileImage;
register ilReadFilePrivPtr pPriv;
unsigned short          *pPalette;
ilPtr                   pCompData;

        /*  Validate that pipe and file image have the same context. */
    pFileImage = (ilFileImagePtr)fileImage;
    if (mustBeZero != 0)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_PAR_NOT_ZERO);
    if (pFileImage->context != pipe->context)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_CONTEXT_MISMATCH);

        /*  Get pipe state - must be IL_PIPE_EMPTY or declare an error. */
    if (ilGetPipeInfo (pipe, FALSE, (ilPipeInfo *)NULL, (ilImageDes *)NULL, 
                                    (ilImageFormat *)NULL) != IL_PIPE_EMPTY) {
        if (!pFileImage->context->error)
            ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);
        return FALSE;
        }

        /*  Planar sample order not yet supported. */
    if (pFileImage->p.format.sampleOrder != IL_SAMPLE_PIXELS)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_NOT_IMPLEMENTED);

        /*  If a palette image, allocate space for it (must pass ptr to it when adding
            pipe element), but don't read it until Init() called.
        */
    if (pFileImage->p.des.type == IL_PALETTE) {
        pPalette = (unsigned short *)IL_MALLOC (sizeof (unsigned short) * 3 * 256);
        if (!pPalette)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_MALLOC);
        }
    else pPalette = (unsigned short *)NULL;

        /*  Allocate and zero pCompData if JPEG (similar to pPalette) */
    if (pFileImage->p.des.compression == IL_JPEG) {
        pCompData = (ilPtr)IL_MALLOC_ZERO (sizeof (ilJPEGData));
        if (!pCompData)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_MALLOC);
        }
    else pCompData = (ilPtr)NULL;

        /*  Add a producer pipe element: output data is defined in *pFileImage. */
    dstData.producerObject = (ilObject)fileImage;
    dstData.pDes = &pFileImage->p.des;
    dstData.pFormat = &pFileImage->p.format;
    dstData.width = pFileImage->p.width;
    dstData.height = pFileImage->p.height;
    dstData.stripHeight = pFileImage->p.stripHeight;
    dstData.constantStrip = TRUE;
    dstData.pPalette = pPalette;
    dstData.pCompData = pCompData;

    pPriv = (ilReadFilePrivPtr)ilAddPipeElement (pipe, IL_PRODUCER, 
            sizeof (ilReadFilePrivRec), 0, (ilSrcElementData *)NULL, &dstData,
            ilReadFileInit, IL_NPF, ilReadFileDestroy, ilReadFileExecute, NULL, 0);
    if (!pPriv) {
        if (pPalette) IL_FREE (pPalette);
        if (pCompData) IL_FREE (pCompData);
        return FALSE;
        }

        /*  Init private. Inc refCount for this file - dec'd in ilReadFileDestroy(). */
    pPriv->pFile = (ilFilePtr)pFileImage->p.file;
    pPriv->pFileImage = pFileImage;
    pPriv->compression = pFileImage->p.des.compression;
    pPriv->stripHeight = pFileImage->p.stripHeight;
    pPriv->pPalette = pPalette;
    pPriv->pCompData = pCompData;
    pPriv->initDone = FALSE;
    ((ilObjectPtr)pFileImage->p.file)->refCount++;

        /*  If a src rect given, crop to it.
            NOTE: IMPLEMENT PARTIAL FILE READ IN THE FUTURE !!!!!
        */
    if (pSrcRect)
        return ilCrop (pipe, pSrcRect);

    pipe->context->error = IL_OK;
    return TRUE;
}

