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
/* $XConsortium: iltiffwrite.c /main/4 1996/01/08 12:17:15 lehors $ */
    /*  /ilc/iltiffwrite.c : Code for ilWriteFileImage(); add a consumer to the pipe
        to write to a TIFF file.  See also /ilc/iltiff.c and /ilc/iltiffread.c .
    */

#include "iltiffint.h"
#include "ilpipelem.h"
#include "iljpgencode.h"
#include "ilerrors.h"

    /*  Tag info summary, private to ilWriteFileImage().  Encapsulates the user and std
        tag info.  Contains ptrs to data which is created when the pipe element is added
        and freed when pipe emptied (Destroy() function).
            "pTagOffsets" points to an array of offsets, in the same order as the tags at
        "pTagWriteData" + 2 (bytes).  For each tag, if the offset is zero (0), the tag
        data is in the tag, already byte-flipped.  If non-zero, it is the offset of the
        tag data from pTagWriteData.  The file offset where the data at pTagWriteData 
        will be written must be added to this offset and inserted into the tag byte-flipped.
            See ilMergeWriteTags() for details.
    */
typedef struct {
    ilPtr           pAlloc;                 /* ptr to whole mess to deallocate */
    CARD32         *pTagOffsets;            /* see above */
    ilPtr           pTagWriteData;          /* ptr to data to write to file */
    long            tagWriteSize;           /* size in bytes of data to write */
    ilFileOffset   *pStripOffsets;          /* ptr to strip offset list in memory */
    INT32          *pStripByteCounts;       /* ptr to strip byte count list in memory */
    ilFileOffset   *pJIF;                   /* (JIF only): offset in memory */
    long           *pJIFLength;             /* (JIF only): length in memory */
    unsigned short *pJPEGRestart;           /* (JPEG only): restart interval in memory */
    long           *pJPEGQTables;           /* (JPEG only): Q tables in memory */
    long           *pJPEGDCTables;          /* (JPEG only): DC tables in memory */
    long           *pJPEGACTables;          /* (JPEG only): AC tables in memory */
    unsigned short *pPalette;               /* ptr to pal; null if not palette image */
    int             nPaletteColors;         /* # of RGB triplets at pPalette */
    int             tagCount;               /* # of tags to write */
    } ilTagInfoRec, *ilTagInfoPtr;


typedef struct {
            /*  Data set into private when element added */
    ilFilePtr       pFile;                  /* ptr to file being written */
    ilTagInfoRec    tagInfo;                /* see above */
    ilBool          bigEndian;              /* true if MM byte order file */
    unsigned int    compression;            /* type of compression being written */
    long            nStrips;                /* # of strips which should be written */
    ilFileOffset    tailPtrOffset;          /* "next ptr" to point to this image */
    ilFileImageRec  fileImage;              /* data for file image being added */

            /*  Data set/inited by ilWriteFileInit() */
    ilFileOffset   *pStripOffset;           /* next in-memory strip offset to store */
    INT32          *pStripByteCount;        /* same for byte count (# bytes written) */
    long            nStripsSoFar;           /* # of strips written so far */
    long            nLinesSoFar;            /* # of lines written so far */
    long            nBytesSoFar;            /* # of bytes written so far (JPEG only) */
    } ilWriteFilePrivRec, *ilWriteFilePrivPtr;


    /*  Copy 4 bytes of a long byte-flipped based on given "bigEndian" flag, to
        the given ptr which must be of type ilPtr.  Same for a short.
    */
#define IL_WRITE_FLIP_LONG(_bigEndian, _long, _ptr) {  \
    if (_bigEndian) {                           \
        *(_ptr)++ = (_long) >> 24;              \
        *(_ptr)++ = ((_long) >> 16) & 0xff;     \
        *(_ptr)++ = ((_long) >> 8) & 0xff;      \
        *(_ptr)++ = (_long) & 0xff;             \
        }                                       \
    else {                                      \
        *(_ptr)++ = (_long) & 0xff;             \
        *(_ptr)++ = ((_long) >> 8) & 0xff;      \
        *(_ptr)++ = ((_long) >> 16) & 0xff;     \
        *(_ptr)++ = (_long) >> 24;              \
        }                                       \
    }

#define IL_WRITE_FLIP_SHORT(_bigEndian, _short, _ptr) {  \
    if (_bigEndian) {                           \
        *(_ptr)++ = ((_short) >> 8) & 0xff;     \
        *(_ptr)++ = (_short) & 0xff;            \
        }                                       \
    else {                                      \
        *(_ptr)++ = (_short) & 0xff;            \
        *(_ptr)++ = ((_short) >> 8) & 0xff;     \
        }                                       \
    }

        /*  --------------------- ilWriteFileInit -------------------------- */
        /*  Init() function for ilWriteFileImage().
        */
static ilError ilWriteFileInit (
    ilWriteFilePrivPtr pPriv,
    ilImageInfo        *pSrcImage,
    ilImageInfo        *pDstImage
    )
{
        /*  Point to beginning of strip offset/byte count memory buffers. */
    pPriv->pStripOffset = pPriv->tagInfo.pStripOffsets;
    pPriv->pStripByteCount = pPriv->tagInfo.pStripByteCounts;

    pPriv->nStripsSoFar = 0;
    pPriv->nLinesSoFar = 0;
    pPriv->nBytesSoFar = 0;

    return IL_OK;
}


        /*  --------------------- ilWriteFileCleanup -------------------------- */
        /*  Cleanup() function for ilWriteFileImage().
        */
static ilError ilWriteFileCleanup (
    ilWriteFilePrivPtr pPriv,
    ilBool                  aborting
    )
{
ilFilePtr          pFile;
ilFileOffset                position;
ilByte                      fourBytes[4], nextBytes[4];
ilPtr                       pBytes, pTag;
CARD32            *pTagOffsets, l;
int                         i;

        /*  If aborting pipe, just exit.  The net effect is that if
            some image strips are written and the pipe aborted before completion,
            the image strips are there but not pointed to by anything.
        */
    if (aborting)
        return IL_OK;                       /* EXIT */

        /*  Make sure the proper # of lines and strips have been written or error. */
    if ((pPriv->nStripsSoFar != pPriv->nStrips) 
     || (pPriv->nLinesSoFar != pPriv->fileImage.p.height))
        return IL_ERROR_MALFORMED_FILE_WRITE;
    pFile = pPriv->pFile;

        /*  Read "offset to next" that will be pointed to this image; set as "offset to
            next" for this image (IFDOffset), following tag info; see ilMergeWriteTags().
        */
    if (!IL_SEEK (pFile, pPriv->tailPtrOffset) || !IL_READ (pFile, 4, nextBytes))
        return IL_ERROR_FILE_IO;
    pBytes = pPriv->tagInfo.pTagWriteData + pPriv->tagInfo.tagCount * IL_TAG_SIZE + 2;
    *pBytes++ = nextBytes[0]; 
    *pBytes++ = nextBytes[1];
    *pBytes++ = nextBytes[2];
    *pBytes++ = nextBytes[3];

        /*  Position to EOF; save file position in "position".  Update tags whose values
            did not fit in the tag; they have a non-zero value in the array @pTagOffsets;
            the value is the offset from the beginning of the data to be written to the
            tag data.
        */
    if (fseek (pFile->stream, 0, 2))        /* seek to EOF (2); return != 0 is error */
        return IL_ERROR_FILE_IO;
    position = ftell (pFile->stream) - pFile->offset;
    
    pTagOffsets = pPriv->tagInfo.pTagOffsets;
    pTag = pPriv->tagInfo.pTagWriteData + 2;
    for (i = 0; i < pPriv->tagInfo.tagCount; i++, pTagOffsets++, pTag += IL_TAG_SIZE)
        if (*pTagOffsets) {
            l = position + *pTagOffsets;
            pBytes = pTag + 8;              /* point to offset field of the tag */
            IL_WRITE_FLIP_LONG (pPriv->bigEndian, l, pBytes)
            }

        /*  Write out the tag data to add the image to the file.  Patch location at
            "tailPtrOffset" in file to point to this image to link image into list.
        */
    if (fwrite ((char *)pPriv->tagInfo.pTagWriteData, pPriv->tagInfo.tagWriteSize, 1, 
                pFile->stream) != 1)
        return IL_ERROR_FILE_IO;
    pBytes = fourBytes;
/* compatibility problem with long or unsigned long data fields */
    IL_WRITE_FLIP_LONG (pPriv->bigEndian, (CARD32)position, pBytes)
    if (!IL_SEEK (pFile, pPriv->tailPtrOffset) || !IL_WRITE (pFile, 4, fourBytes))
        return IL_ERROR_FILE_IO;
    fflush (pFile->stream);                 /* flush all buffers; image now in file */

        /*  If this image is now last in file (its next offset is null), update file's 
            offset to last image's offset to next, and copy of same in private.
        */
    if (!nextBytes[0] && !nextBytes[1] && !nextBytes[2] && !nextBytes[3]) {
        pFile->IFDTailPtrOffset = position + pPriv->tagInfo.tagCount * IL_TAG_SIZE + 2;
        pPriv->tailPtrOffset = pFile->IFDTailPtrOffset;
        }

        /*  Mark file image list as invalid, so this image will show up next time. */
    pFile->haveImageList = FALSE;

    return IL_OK;
}


        /*  ------------------------ ilWriteFileDestroy ----------------------------- */
        /*  Destroy() function for ilWriteFileImage().
        */
static ilError ilWriteFileDestroy (
    ilWriteFilePrivPtr      pPriv
    )
{
        /*  Free the tag data malloc'd by ilMergeWriteTags() when element added. */
    IL_FREE (pPriv->tagInfo.pAlloc);
    return IL_OK;
}

        /*  --------------------- ilCopyPalette -------------------------- */
        /*  Called by Execute() function before writing first strip to copy the palette
            into the tag memory buffer, so it will be written out by ilWriteFileCleanup().
        */
static void ilCopyPalette (
    ilWriteFilePrivPtr pPriv,
    unsigned short    *pSrcPalette
    )
{
unsigned short *pDstPalette;
ilPtr          pBytes;
/* Since using IL_WRITE_FLIP_SHORT, palEntry should be unsigned short */
unsigned short palEntry;
unsigned long           greenOffset, blueOffset;
int                     i;

        /*  Copy palette into the in-memory tag area, byte-flipped.
            It will be written out along with all the tags and tag data during Cleanup().
            TIFF palettes are the same as IL: all the red, then green, then blue values,
            each a short, except in TIFF each is "2 ** nBits" entries 
            (pPriv->nPaletteColors) in length; in the IL they are always 256 entries.
        */
    pDstPalette = pPriv->tagInfo.pPalette;
    if (pDstPalette) {
        greenOffset = pPriv->tagInfo.nPaletteColors;
        blueOffset = pPriv->tagInfo.nPaletteColors * 2;
        for (i = 0; i < pPriv->tagInfo.nPaletteColors; i++, pSrcPalette++, pDstPalette++) {
            palEntry = *pSrcPalette;
            pBytes = (ilPtr)pDstPalette;
            IL_WRITE_FLIP_SHORT (pPriv->bigEndian, palEntry, pBytes)
            palEntry = *(pSrcPalette + 256);
            pBytes = (ilPtr)(pDstPalette + greenOffset);
            IL_WRITE_FLIP_SHORT (pPriv->bigEndian, palEntry, pBytes)
            palEntry = *(pSrcPalette + 512);
            pBytes = (ilPtr)(pDstPalette + blueOffset);
            IL_WRITE_FLIP_SHORT (pPriv->bigEndian, palEntry, pBytes)
            }
        }
}


        /*  --------------------- ilWriteFileExecute -------------------------- */
        /*  Execute() for ilWriteFileImage().  
        */
static ilError ilWriteFileExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
ilWriteFilePrivPtr pPriv;
ilFilePtr                   pFile;
unsigned long               nBytes, rowBytes;
ilPtr                       pBytes;
long                        position;

    if (*pNLines <= 0)
        return IL_OK;
    pPriv = (ilWriteFilePrivPtr)pData->pPrivate;
    pFile = pPriv->pFile;

        /*  If first strip and a palette image copy the palette into tag data */
    if ((pPriv->nStripsSoFar <= 0) && pPriv->tagInfo.pPalette)
        ilCopyPalette (pPriv, pData->pSrcImage->pPalette);

        /*  Check for too many lines/strips or inconstant strips (except last). */
    pPriv->nLinesSoFar += *pNLines;
    pPriv->nStripsSoFar++;
    if ((pPriv->nStripsSoFar > pPriv->nStrips) 
     || (pPriv->nLinesSoFar > pPriv->fileImage.p.height)
     || ((pPriv->nLinesSoFar != pPriv->fileImage.p.height)  /* not last strip */
             && (*pNLines != pPriv->fileImage.p.stripHeight)))
        return IL_ERROR_MALFORMED_FILE_WRITE;

        /*  Write one strip of data to the end of the file, after saving "position". */
    if (fseek (pFile->stream, 0, 2))        /* seek to EOF (2); return != 0 is error */
        return IL_ERROR_FILE_IO;            /* EXIT */
    position = ftell (pFile->stream) - pFile->offset;
    if (pPriv->compression == IL_UNCOMPRESSED) {
        rowBytes = pData->pSrcImage->plane[0].nBytesPerRow;
        nBytes = rowBytes * *pNLines;
        pBytes = pData->pSrcImage->plane[0].pPixels + pData->srcLine * rowBytes;
        }
    else {
        nBytes = pData->compressed.nBytesToRead;
        pBytes = pData->pSrcImage->plane[0].pPixels + pData->compressed.srcOffset;
        }
    if (fwrite ((char *)pBytes, nBytes, 1, pFile->stream) != 1) 
        return IL_ERROR_FILE_IO;            /* EXIT */

        /*  Store byte-flipped the offset within the file (position) and size
            (nBytes) of the strip just written.
        */
    pBytes = (ilPtr)pPriv->pStripOffset;
    pPriv->pStripOffset++;
/* compatibility problem with long or unsigned long data fields */
    IL_WRITE_FLIP_LONG (pPriv->bigEndian, (CARD32)position, pBytes)
    pBytes = (ilPtr)pPriv->pStripByteCount;
    pPriv->pStripByteCount++;
/* compatibility problem with long or unsigned long data fields */
    IL_WRITE_FLIP_LONG (pPriv->bigEndian, (CARD32)nBytes, pBytes)

    return IL_OK;
}


    /*  --------------------------- ilWriteJIFData -------------------------------- */
    /*  Called by ilWriteJPEGExecute() when first strip encountered, to encode
        and write a JIF header into the TIFF file.
    */
static ilError ilWriteJIFData (
    ilWriteFilePrivPtr pPriv,
    ilFilePtr               pFile,
    ilImageInfo            *pImage
    )
{
    iljpgJIFOffsetsRec      offsets;
    iljpgDataRec            data;
    long                    JIFOffset;
    long                    position, nBytes, i, j;
    ilJPEGEncodeStream      streamRec;
    ilJPEGData             *pCompData;
    ilError                 error;
    long                    mcuWidth, mcuHeight;
    ilPtr          pBytes;


        /*  Init streamRec so output is written into malloc'd memory, and encode
            a JIF image into that memory.  For restartInterval value: if a single strip
            use value from raw data (== 0 if no restart markers), otherwise calculate
            it from strip size.  If multi-strip and non-zero restartMarker in raw data
            a JIF image cannot be created - see below.
        */
    pCompData = (ilJPEGData *)pImage->pCompData;
    _ilJPEGDataIn (pImage->pDes, pPriv->fileImage.p.width, pPriv->fileImage.p.height, 
                   &data);
    _ilJPEGTablesIn (pCompData, &data);

    if (pPriv->nStrips != 1) {              /* else data.restartInterval = raw already */
        mcuWidth = 8 * data.maxHoriFactor;
        mcuHeight = 8 * data.maxVertFactor;
        data.restartInterval =              /* # of mcus across * # down */
            ((pPriv->fileImage.p.width + mcuWidth - 1) / mcuWidth) *
            ((pPriv->fileImage.p.stripHeight + mcuHeight - 1) / mcuHeight);
        }

    streamRec.pBuffer = streamRec.pDst = streamRec.pPastEndBuffer = (ilPtr)NULL;
    if (error = iljpgEncodeJIF (&streamRec, &data, &offsets))
        return error;

        /*  Write JIF header to end of file, at "position". */
    if (fseek (pFile->stream, 0, 2))        /* seek to EOF (2); != 0 is error */
        return IL_ERROR_FILE_IO;
    position = ftell (pFile->stream) - pFile->offset;
    nBytes = streamRec.pDst - streamRec.pBuffer;
    if (fwrite ((char *)streamRec.pBuffer, nBytes, 1, pFile->stream) != 1) 
        return IL_ERROR_FILE_IO;
    IL_FREE (streamRec.pBuffer);            /* free malloc'd buffer space */
    pPriv->nBytesSoFar = nBytes;            /* JIF data size so far */

        /*  Update in-memory values for tags that point to JIF data and to
            the Q/DC/AC tables, returned by iljpgEncodeJIF() above.
                Write restartInterval: if 0, no restart markers.  If non-zero,
            then there must be only one strip in image or it cannot be encoded as
            as a JIF image, because restart markers need to be written between the 
            strips and would not mesh with restart markers already in the data; 
            so in that case write JIF pointer marker = 0 => not JIF.
            The resulting file will be able to be read by IL2.1 and later but not by
            earlier IL versions or by other software that relies on JIF.
        */
    pBytes = (ilPtr)pPriv->tagInfo.pJPEGRestart;
/* type cast from int to short before FLIP */
    IL_WRITE_FLIP_SHORT (pPriv->bigEndian, (short)pCompData->restartInterval, pBytes)

    if (pCompData->restartInterval && (pPriv->nStrips != 1))
         JIFOffset = 0;                     /* not valid JIF; write 0 ptr */
    else JIFOffset = position;
    pBytes = (ilPtr)pPriv->tagInfo.pJIF;
/* compatibility problem with long or unsigned long data fields */
    IL_WRITE_FLIP_LONG (pPriv->bigEndian, (CARD32)position, pBytes)

        /*  Write offsets to Q tables for each component: table offset for each 
            table, indexed by component table index, plus file to offset to JIF hdr.
        */
    pBytes = (ilPtr)pPriv->tagInfo.pJPEGQTables;
    for (i = 0; i < pPriv->fileImage.p.des.nSamplesPerPixel; i++) {
        j = offsets.QTables[data.comp[i].QTableIndex];
        if (!j)                             /* no Q table defined for that index */
            return IL_ERROR_COMPRESSED_DATA;
        j += position;                      /* now offset from start of "file" */
/* compatibility problem with long or unsigned long data fields */
        IL_WRITE_FLIP_LONG (pPriv->bigEndian, (INT32)j, pBytes)
        }

        /*  Same thing for DC and AC tables */
    pBytes = (ilPtr)pPriv->tagInfo.pJPEGDCTables;
    for (i = 0; i < pPriv->fileImage.p.des.nSamplesPerPixel; i++) {
        j = offsets.DCTables[data.comp[i].DCTableIndex];
        if (!j)
            return IL_ERROR_COMPRESSED_DATA;
        j += position;
/* compatibility problem with long or unsigned long data fields */
        IL_WRITE_FLIP_LONG (pPriv->bigEndian, (INT32)j, pBytes)
        }

    pBytes = (ilPtr)pPriv->tagInfo.pJPEGACTables;
    for (i = 0; i < pPriv->fileImage.p.des.nSamplesPerPixel; i++) {
        j = offsets.ACTables[data.comp[i].ACTableIndex];
        if (!j)
            return IL_ERROR_COMPRESSED_DATA;
        j += position;
/* compatibility problem with long or unsigned long data fields */
        IL_WRITE_FLIP_LONG (pPriv->bigEndian, (INT32)j, pBytes)
        }

    return IL_OK;
}

        /*  --------------------- ilWriteJPEGExecute -------------------------- */
        /*  Execute() for ilWriteFileImage() when writing JPEG-compressed data.
        */
static ilError ilWriteJPEGExecute (
    ilExecuteData          *pData,
    long                    dstLine,
    long                   *pNLines
    )
{
    ilWriteFilePrivPtr pPriv;
    ilFilePtr               pFile;
    ilError                 error;
    long                    position, nBytes, i, j;
    ilPtr          pBytes;
    ilByte                  marker[2];

        /*  The overall approach is to write a JIF image into the file plus strips.
            The data written will be the JIF "header", followed by strips of JPEG
            data, separated by restart markers.  Each strip will pointed to by
            the normal TIFF strip offset/byte counts.  The resulting data can then
            be read either as a JIF image (using the JPEGInterchangeFormat/Length
            tags - how IL2.0 and earlier versions read it) or as a "raw" image using
            the other JPEG tags (how later versions read it).
        */
    pPriv = (ilWriteFilePrivPtr)pData->pPrivate;
    pFile = pPriv->pFile;

        /*  Check for too many lines/strips or inconstant strips (except last). */
    pPriv->nLinesSoFar += *pNLines;
    pPriv->nStripsSoFar++;
    if ((pPriv->nStripsSoFar > pPriv->nStrips) 
     || (pPriv->nLinesSoFar > pPriv->fileImage.p.height)
     || ((pPriv->nLinesSoFar != pPriv->fileImage.p.height)  /* not last strip */
             && (*pNLines != pPriv->fileImage.p.stripHeight)))
        return IL_ERROR_MALFORMED_FILE_WRITE;

        /*  If first strip: encode a JIF header and write it to file.
            Init streamRec so output is written into malloc'd memory, and encode
            a JIF image into that memory.
        */
    if (pPriv->nStripsSoFar <= 1)                   /* first strip; inc'd above */
        if (error = ilWriteJIFData (pPriv, pFile, pData->pSrcImage))
            return error;

        /*  Write one strip of data to end of the TIFF file, after saving "position". */
    if (fseek (pFile->stream, 0, 2))        /* seek to EOF (2); return != 0 is error */
        return IL_ERROR_FILE_IO;
    position = ftell (pFile->stream) - pFile->offset;
    nBytes = pData->compressed.nBytesToRead;
    pBytes = pData->pSrcImage->plane[0].pPixels + pData->compressed.srcOffset;
    if (fwrite ((char *)pBytes, nBytes, 1, pFile->stream) != 1) 
        return IL_ERROR_FILE_IO;
    pPriv->nBytesSoFar += nBytes + 2;       /* # bytes in strip + 2 for marker below */

        /*  Store byte-flipped the offset within the file (position) and size
            (nBytes) of the strip just written.
        */
    pBytes = (ilPtr)pPriv->pStripOffset;
    pPriv->pStripOffset++;
/* compatibility problem with long or unsigned long data fields */
    IL_WRITE_FLIP_LONG (pPriv->bigEndian, (CARD32)position, pBytes)
    pBytes = (ilPtr)pPriv->pStripByteCount;
    pPriv->pStripByteCount++;
/* compatibility problem with long or unsigned long data fields */
    IL_WRITE_FLIP_LONG (pPriv->bigEndian, (INT32)nBytes, pBytes)

        /*  If last strip, add an EOI marker to end and store JIF length tag value;
            otherwise add a restart marker, with strip count modulo 8 in it.
        */
    marker[0] = 0xff;
    if (pPriv->nStripsSoFar >= pPriv->nStrips) {
        marker[1] = ILJPGM_EOI;
        pBytes = (ilPtr)pPriv->tagInfo.pJIFLength;
/* compatibility problem with long or unsigned long data fields */
        IL_WRITE_FLIP_LONG (pPriv->bigEndian, (INT32)pPriv->nBytesSoFar, pBytes)
        }
    else marker[1] = ILJPGM_RST0 | ((pPriv->nStripsSoFar - 1) & 7);

        /*  Write two byte restart or end of image (EOI) marker */
    if (fseek (pFile->stream, 0, 2))
        return IL_ERROR_FILE_IO;
    if (fwrite ((char *)marker, 2, 1, pFile->stream) != 1) 
        return IL_ERROR_FILE_IO;

    return IL_OK;
}


        /*  --------------------- ilMergeWriteTags ----------------------------------- */
        /*  Called by ilWriteFileImage().  Merge the user and std tag info, copy the
            tag data (flipped) and return into to *pInfo.
        */
static ilError ilMergeWriteTags (
    ilBool              bigEndian,              /* byte order to be written */
    int                 nStdTags,               /* # of std (IL) tags */
    ilFileTag          *pStdTags,               /* ptr to std tags */
    int                 nUserTags,              /* # of user (IL) tags */
    ilFileTag          *pUserTags,              /* ptr to user tags */
    ilTagInfoPtr        pInfo)                  /* returned tag info */
{
ilFileTag             **ppTagsInit, **ppLastTag;
ilFileTag    **ppTag, **ppTagTemp, *pTag;
ilPtr                   pTagData, pFileTags;
long                    tagArraySize, tagDataSize, l;
CARD32                 *pTagOffsets, *pLong;
int                     nTags, i;
unsigned short          tagType, *pShort;
int            tagNumber;

        /*  Allocate and null an array of ptrs to tags, size = # user + std tags, + 1
            so that there is at least one guaranteed null tag ptr, for "end of array".
        */
    tagArraySize = nStdTags + nUserTags + 1;
    ppTagsInit = (ilFileTag **)IL_MALLOC (tagArraySize * sizeof (ilFileTag **));
    if (!ppTagsInit)
        return IL_ERROR_MALLOC;
    ppLastTag = ppTagsInit + tagArraySize - 1;
    for (ppTag = ppTagsInit; ppTag <= ppLastTag; ppTag++)
        *ppTag = (ilFileTag *)NULL;

        /*  Fill in array with ptrs to tags, sorted in ascending numeric order (required
            by TIFF!), with duplicates eliminated.  Do the user tags, then the std tags,
            so that the std tag values overwrite the user if duplicates present.
                Sort by insertion sort: find tag with number >= this tag's number, or end
            of list (null ptr).  If greater, shift all values up one; then insert this tag
        */
    for (i = 0, pTag = pUserTags, nTags = nUserTags;  i < 2;
         i++, pTag = pStdTags, nTags = nStdTags) {
        while (nTags-- > 0) {
            ppTag = ppTagsInit;
            while (*ppTag && ((*ppTag)->number < pTag->number))
                ppTag++;
            if (*ppTag && ((*ppTag)->number > pTag->number)) {
                ppTagTemp = ppLastTag;
                while (ppTagTemp > ppTag) {
                    *ppTagTemp = *(ppTagTemp - 1);
                    ppTagTemp--;
                    }
                }
            *ppTag = pTag;
            pTag++;
            }
        }

        /*  Set "nTags" to the resulting number of tags, and "tagDataSize" to the size
            of tag data which does not fit in the tag itself (rounded to mult of 2).
        */
    for (tagDataSize = 0, nTags = 0, ppTag = ppTagsInit; *ppTag; nTags++, ppTag++) {
        pTag = *ppTag;
        tagType = pTag->type;
        if ((tagType == 0) || (tagType > IL_MAX_TAG_TYPE)) {
            IL_FREE (ppTagsInit);
            return IL_ERROR_FILE_TAG_TYPE;
            }
        if (pTag->nItems > _ilTagTypeItemsThatFit [tagType])
            tagDataSize += (_ilTagTypeItemSizes [tagType] * pTag->nItems + 1) & ~1;
        }

        /*  Alloc room on heap for: an array of offsets to tag data for each tag, or 0
            if tag data fits in tag; the # of tags (2 bytes); the tags themselves; the 
            next IFD offset (4 bytes); and the tag data that does not fit in the tags.
            pInfo->pAlloc will point to it all:
                    -----------------------------
                    | offset to data for each   |   <- pTagOffsets (not written)
                    | tag or 0 if data in tag   |  (rest is written to TIFF, and is
                    |                           |   "tagWriteSize" bytes in size
                    ----------------------------- 
                    | # of tags (short)         |   <- pTagWriteData
                    ----------------------------- 
                    | tag info for each tag     |   +2
                    | (of type ilTIFFTagRec)    |
                    ----------------------------- 
                    | ptr to next IFD           |   + # tags * IL_TAG_SIZE + 2
                    ----------------------------- 
                    | tag data external to tags |   + # tags * IL_TAG_SIZE + 6
                    ----------------------------- 
        */
    pInfo->tagCount = nTags;
    pInfo->tagWriteSize = nTags * IL_TAG_SIZE + 2 + tagDataSize + 4;
    pInfo->pAlloc = (ilPtr)IL_MALLOC_ZERO (nTags * sizeof(INT32) + pInfo->tagWriteSize);
    if (!pInfo->pAlloc) {
        IL_FREE (ppTagsInit);
        return IL_ERROR_MALLOC;
        }
    pInfo->pTagOffsets = (CARD32 *)pInfo->pAlloc;
    pInfo->pTagWriteData = pInfo->pAlloc + nTags * sizeof(INT32);
    pTagData = (ilPtr)pInfo->pTagWriteData + nTags * IL_TAG_SIZE + 6;

        /*  Write the # of tags byte-flipped, point "pFileTags" to tag data write area.
            For byte/ascii data: "nItems" is length of string, including trailing null
            for ascii.  Round up to even byte boundary (TIFF requirement).
        */
    pFileTags = (ilPtr)pInfo->pTagWriteData;
    IL_WRITE_FLIP_SHORT (bigEndian, (unsigned short)nTags, pFileTags)

    for (ppTag = ppTagsInit, pTagOffsets = pInfo->pTagOffsets; *ppTag; ppTag++) {
        ilPtr                   pWrite;

        pTag = *ppTag;
        tagType = pTag->type;
        IL_WRITE_FLIP_SHORT (bigEndian, pTag->number, pFileTags)
        IL_WRITE_FLIP_SHORT (bigEndian, tagType, pFileTags)
/* compatibility problem with long or unsigned long data fields */
        IL_WRITE_FLIP_LONG  (bigEndian, (CARD32)pTag->nItems, pFileTags)
        if (pTag->nItems > _ilTagTypeItemsThatFit [tagType]) {
            *pTagOffsets++ = pTagData - pInfo->pTagWriteData;
            pWrite = pTagData;
            pTagData += (_ilTagTypeItemSizes [tagType] * pTag->nItems + 1) & ~1UL;
            }
        else {
            *pTagOffsets++ = 0;
            pWrite = pFileTags;
            }
        pFileTags += 4;                 /* skip one TIFF long */

            /*  Copy the data at "*pTag->pItems" to "pWrite", with byte-flip.
                pWrite points to 4 byte area in tag if items fit, else to area in pTagData.
                NOTE: handle strip offsets/byte counts specially: pItems is ignored.
            */
        tagNumber = pTag->number;
        if (tagNumber == IL_TAG_STRIP_OFFSETS)
            pInfo->pStripOffsets = (ilFileOffset *)pWrite;
        else if (tagNumber == IL_TAG_STRIP_BYTE_COUNTS)
            pInfo->pStripByteCounts = (INT32 *)pWrite;
        else if (tagNumber == IL_TAG_COLOR_MAP)
            pInfo->pPalette = (unsigned short *)pWrite;
        else if (tagNumber == IL_TAG_JPEG_INTERCHANGE_FORMAT)
            pInfo->pJIF = (ilFileOffset *)pWrite;
        else if (tagNumber == IL_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH)
            pInfo->pJIFLength = (long *)pWrite;
        else if (tagNumber == IL_TAG_JPEG_RESTART_INTERVAL)
            pInfo->pJPEGRestart = (unsigned short *)pWrite;
        else if (tagNumber == IL_TAG_JPEG_Q_TABLES)
            pInfo->pJPEGQTables = (long *)pWrite;
        else if (tagNumber == IL_TAG_JPEG_DC_TABLES)
            pInfo->pJPEGDCTables = (long *)pWrite;
        else if (tagNumber == IL_TAG_JPEG_AC_TABLES)
            pInfo->pJPEGACTables = (long *)pWrite;
        else switch (tagType) {
          case IL_TAG_BYTE:
          case IL_TAG_ASCII:
            bcopy ((char *)pTag->pItems, (char *)pWrite, pTag->nItems);
            break;
          case IL_TAG_SHORT:
            for (i = 0, pShort = (unsigned short *)pTag->pItems; 
                                   i < pTag->nItems; i++, pShort++)
                IL_WRITE_FLIP_SHORT (bigEndian, *pShort, pWrite)
            break;
          case IL_TAG_LONG:
/* compatibility problem with long or unsigned long data fields */
            for (i = 0, pLong = (CARD32 *)pTag->pItems;
                                   i < pTag->nItems; i++, pLong++)
                IL_WRITE_FLIP_LONG (bigEndian, *pLong, pWrite)
            break;
          case IL_TAG_RATIONAL:
/* compatibility problem with long or unsigned long data fields */
            for (i = 0, pLong = (CARD32 *)pTag->pItems; i < pTag->nItems; i++) {
                IL_WRITE_FLIP_LONG (bigEndian, *pLong, pWrite)
                pLong++;
                IL_WRITE_FLIP_LONG (bigEndian, *pLong, pWrite)
                pLong++;
                }
            break;
            }
        }   /* END for one tag */

        /*  Fill in ptr to next IFD (= 0, null), free temp alloc space and exit.
        */
/* compatibility problem with long or unsigned long data fields */
    IL_WRITE_FLIP_LONG (bigEndian, 0, pFileTags)
    IL_FREE (ppTagsInit);
    return IL_OK;
}

        /*  ---------------------- ilFormatPipeImage --------------------------------- */
        /*  Reformat and compress the pipe image as necessary.  On entry, pStripHeight
            must point to the callers requested stripHeight; on return it is the new
            (constant) stripHeight to use to write.
                On entry, pInfo/pDes/pFormat must point to the current pipe values;
            on return they are the new pipe values.  Returns false if an error occurs.
        */
static ilBool ilFormatPipeImage (
    ilPipe                  pipe,
    unsigned int            compression,
    ilPtr                   pCompData,
    long                   *pStripHeight,
    ilPipeInfo    *pInfo,
    ilImageDes    *pDes,
    ilImageFormat *pFormat
    )
{
#define IL_WRITE_TIFF_STRIP_SIZE (16 * 1024)    /* write 16KB strips as default */
long               stripHeight;
long                        i, nLevels;
ilBool                      mustConvert;
unsigned long               g3Flags, g4Flags;
ilJPEGEncodeControl         jpegEncode;

        /*  If compression is "use current compression" (which makes it easier for
            caller to copy images), then set compression to current pipe compression,
            and point pCompData to compression-specific current pipe data.
        */
    if (compression == IL_WRITE_CURRENT_COMPRESSION) {
        compression = pDes->compression;
        if (compression == IL_G3) {
            g3Flags = pDes->compInfo.g3.flags;
            pCompData = (ilPtr)&g3Flags;
            }
        else if (compression == IL_G4) {
            g4Flags = pDes->compInfo.g4.flags;
            pCompData = (ilPtr)&g4Flags;
            }
        }

        /*  Compare the levels/sample to bits/sample, and scale up if levels != 2**bits.
            This is because the IL separates bits and levels, but TIFF assumes # levels
            = 2**bits.  A 16 level 8 bit gray image must be level-scaled up to 256 or
            else the reader will assume 256 level and display it too dark.
            However, bitonal must be bit/pixel, and for palette, levels is ignored.
                NOTE: the right thing to do is to convert the format (e.g. change 16
            level 8 bit gray to 4 bit gray), but ilConvert() does not handle this yet!
        */
    switch (pDes->type) {
      case IL_BITONAL:
        if (pFormat->nBitsPerSample[0] != 1) {
            pFormat->nBitsPerSample[0] = 1;
            if (!ilConvert (pipe, (ilImageDes *)NULL, pFormat, 0, NULL))
                return FALSE;
            }
        break;
      case IL_PALETTE:                      /* ignore nLevels for palette images */
        break;
      default:                              /* continuous tone: match levels to bits */
        for (i = 0, mustConvert = FALSE; i < pDes->nSamplesPerPixel; i++) {
            nLevels = 1 << pFormat->nBitsPerSample[i];
            if (pDes->nLevelsPerSample[i] != nLevels) {
                pDes->nLevelsPerSample[i] = nLevels;
                mustConvert = TRUE;
                }
            }
        if (mustConvert)
            if (!ilConvert (pipe, pDes, pFormat, 0, (ilPtr)NULL))
                return FALSE;
        break;
        }

        /*  Convert to pixel order and reget pipe info if multi-sample and planar. 
            However, leave planar if JPEG compression - keep YCbCr subsampled if so.
        */
    if (pDes->nSamplesPerPixel == 1)
        pFormat->sampleOrder = IL_SAMPLE_PIXELS;
    else if ((pFormat->sampleOrder != IL_SAMPLE_PIXELS) && (compression != IL_JPEG)) {
        pFormat->sampleOrder = IL_SAMPLE_PIXELS;
        if (!ilConvert (pipe, (ilImageDes *)NULL, pFormat, 0, NULL))
            return FALSE;
        ilGetPipeInfo (pipe, FALSE, pInfo, pDes, pFormat);
        }

        /*  If writing uncompressed, force rowBitAlign of 8 (required by TIFF);
            else compress to stripHeight, using the given compression & data.
            If stripHeight is 0 use default TIFF write size, based on uncompressed size.
        */
    stripHeight = *pStripHeight;

    if (compression == IL_UNCOMPRESSED) {
        ilGetPipeInfo (pipe, TRUE, pInfo, pDes, pFormat);   /* force decompression */
        if (pFormat->rowBitAlign != 8) {
            pFormat->rowBitAlign = 8;
            if (!ilConvert (pipe, (ilImageDes *)NULL, pFormat, 0, (ilPtr)NULL))
                return FALSE;
            ilGetPipeInfo (pipe, FALSE, pInfo, pDes, pFormat);
            }
        if (stripHeight <= 0) {
            long          bytesPerRow [IL_MAX_SAMPLES];
            ilGetBytesPerRow (pDes, pFormat, pInfo->width, bytesPerRow);
            stripHeight = IL_WRITE_TIFF_STRIP_SIZE / bytesPerRow[0];
            }
        if (stripHeight <= 0)
            stripHeight = 1;
        else if (stripHeight > pInfo->height)
            stripHeight = pInfo->height;
        *pStripHeight = stripHeight;        /* write with desired strip height */
        }
    else {
            /*  If compressing JPEG, force "raw" format; get caller's Q (dflt = 0).
                Compress with desired strip height, unless already JPEG compressed,
                in which case use 0 strip height: accept current strip height.
            */
        if (compression == IL_JPEG) {
            jpegEncode.mustbezero = IL_JPEGM_RAW;
            if (pCompData)
                 jpegEncode.Q = ((ilJPEGEncodeControl *)pCompData)->Q;
            else jpegEncode.Q = 0;
            pCompData = (ilPtr)&jpegEncode;
            if (pDes->compression == IL_JPEG)
                stripHeight = 0;            /* accept current strip height */
            }
        if (!ilCompress (pipe, compression, pCompData, stripHeight, 0))
            return FALSE;
        ilGetPipeInfo (pipe, FALSE, pInfo, pDes, pFormat);
        *pStripHeight = pInfo->stripHeight; /* strip height determined by compression */
        }

    return TRUE;
}


        /*  ---------------------- ilWriteFileImage ---------------------------------- */
        /*  Public function: see spec.
            Adds a consumer to the given pipe to write the current pipe image.
        */

IL_EXTERN char _ilVersionString[];    /* in /ilc/ilversion.c ; written for "software" tag */

ilBool ilWriteFileImage (
    ilPipe              pipe,
    ilFile              file,
    unsigned int        method,
    ilFileImage         fileImage,
    unsigned int        compression,
    ilPtr               pCompData,
    long                stripHeight,
    long                xRes,
    long                yRes,
    short               page,
    short               nPages,
    int                 nUserTags,
    ilFileTag          *pUserTags,
    unsigned long       mustBeZero
    )
{
#define MAX_WRITE_TAGS  50          /* max # of std (non-user) tags added here */

#define TW_ADD_TAG(_number, _type, _nItems, _pItems) { \
    pTag->number = (_number);                       \
    pTag->type = (_type);                           \
    pTag->nItems = (_nItems);                       \
    pTag->pItems = (ilPtr)(_pItems);                \
    pTag++;                                         \
    nTags++;                                        \
    }

long                    nStripsPerImage;
ilFileTag               tags [MAX_WRITE_TAGS];
ilFileTag     *pTag;
int                     nTags, i;
ilTagInfoRec            tagInfo;
ilFilePtr               pFile;
ilPipeInfo              info;
ilImageDes              des;
ilImageFormat           format;
ilContext               context;
ilError                 error;
ilWriteFilePrivPtr      pPriv;
ilSrcElementData        srcData;
unsigned long           group3Options, group4Options;
ilFileImageRelation     writeMethod;
ilFileOffset            tailPtrOffset;

    /*  NOTE: below data types (sizes) FIXED! - tied to TIFF type (short/long) */
short                   ditherLevels[IL_TAG_P0_DITHER_LEVELS_LENGTH];
short                   bitsPerSample [IL_MAX_SAMPLES], TIFFcompression, photo;
short                   nSamplesPerPixel, resolutionUnit, planarConfiguration;
short                   pageInfo[2], predictor, fillOrder;
short                   jpegProc, subsample[2], positioning;
long                    width, height, newSubFileType;
long                    xResolution[2], yResolution[2], coeff[6], refBW[12];

#define                 VERSION_LENGTH  18      /* length includes trailing null */
char                    versionName [VERSION_LENGTH];


        /*  Get pipe state - must be IL_PIPE_FORMING or declare an error. */
    pFile = (ilFilePtr)file;
    context = pFile->o.p.context;
    if (ilGetPipeInfo (pipe, FALSE, &info, &des, &format) != IL_PIPE_FORMING) {
        if (!context->error)
            ilDeclarePipeInvalid (pipe, IL_ERROR_PIPE_STATE);
        return FALSE;
        }

    if (mustBeZero != 0)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_PAR_NOT_ZERO);
    if (context != pipe->context)
        return ilDeclarePipeInvalid (pipe, IL_ERROR_CONTEXT_MISMATCH);

        /*  Force the file image list to be formed if not already; exit on error. */
    ilListFileImages (file, 0);
    if (context->error)
        return ilDeclarePipeInvalid (pipe, context->error);

        /*  Handle "method" set "writeMethod", "newSubFileType". */
    switch (method) {
      case IL_WRITE_MAIN:                       /* no parent, add as last image */
        writeMethod = mainImage;
        newSubFileType = 0;
        fileImage = (ilFileImage)NULL;
        break;

        /*  Transparency mask: error if no parent, or parent already has or is a mask, or
            not bitonal image.
        */
      case IL_WRITE_MASK:
        if (!fileImage                          /* no parent */
         || (fileImage->file != file)           /* not from file being written */
         || fileImage->pMask                    /* parent already has a mask */
         || (((ilFileImagePtr)fileImage)->imageType == maskImage)
         || (des.nSamplesPerPixel != 1)         /* not bitonal */
         || (format.nBitsPerSample[0] != 1))
            return ilDeclarePipeInvalid (pipe, IL_ERROR_FILE_WRITE_METHOD);
        writeMethod = maskImage;
        newSubFileType = 4;                     /* TIFF transparency mask */
        break;

        /*  Child image: must have non-mask parent fileImage with same file or error.
            Point to last image associated with parent: find last child (its mask if
            it has one), so that child images are in order written, and masks "bind"
            closest to image that they are a mask of.
        */
      case IL_WRITE_CHILD:
        writeMethod = childImage;
        newSubFileType = 1;                     /* TIFF reduced resolution image */
        if (!fileImage                          /* no parent */
         || (fileImage->file != file)           /* not from file being written */
         || (((ilFileImagePtr)fileImage)->imageType == maskImage))
            return ilDeclarePipeInvalid (pipe, IL_ERROR_FILE_WRITE_METHOD);
        while (fileImage->pChild)
            fileImage = fileImage->pChild;
        if (fileImage->pMask)
            fileImage = fileImage->pMask;       /* write after the mask image */
        break;

      default:
        return ilDeclarePipeInvalid (pipe, IL_ERROR_FILE_WRITE_METHOD);
        break;
        }

        /*  Reformat and compress the pipe image as necessary.  Return the stripHeight
            to use, and the new pipe info/des/format.
        */
    if (!ilFormatPipeImage (pipe, compression, pCompData, &stripHeight, 
                            &info, &des, &format))
        return FALSE;

        /*  Form the tags to be written for this file image. */
    nTags = 0;
    pTag = tags;

        /*  Write the 1st part of the IL version string as the "software" tag, starting 4
            chars into string (past '@(#)') - remember length includes trailing null!
        */
    bcopy (&_ilVersionString[4], versionName, VERSION_LENGTH - 1);
    versionName [VERSION_LENGTH-1] = 0;
    TW_ADD_TAG (IL_TAG_SOFTWARE, IL_TAG_ASCII, VERSION_LENGTH, versionName)

    if (page >= 0)
        newSubFileType |= 2;        /* mark as multi-page image */
    TW_ADD_TAG (IL_TAG_NEW_SUBFILE_TYPE, IL_TAG_LONG, 1, &newSubFileType)

    width = info.width;
    TW_ADD_TAG (IL_TAG_IMAGE_WIDTH, IL_TAG_LONG, 1, &width)

    height = info.height;
    TW_ADD_TAG (IL_TAG_IMAGE_LENGTH, IL_TAG_LONG, 1, &height)

    for (i = 0; i < des.nSamplesPerPixel; i++)
        bitsPerSample [i] = format.nBitsPerSample [i];
    TW_ADD_TAG (IL_TAG_BITS_PER_SAMPLE, IL_TAG_SHORT, des.nSamplesPerPixel, bitsPerSample)

        /*  Determine TIFF compression tag based on pipe image compression and possibly
            des.compInfo.  Ensure pCompData null if not used.
        */
    switch (des.compression) {
      case IL_UNCOMPRESSED:  
        TIFFcompression = 1; 
        break;

        /*  Group 3: use des.compInfo.g3.flags.
            If 0, write TIFF G3 (code 2), else code 3: must have EOL_MARKERS; init
            group3Options to be written below, and write FillOrder tag (required for 
            Class F; not recommended for other uses in TIFF).
            Add group 3 options tag only if "true" (not TIFF) G3 (TIFFcompression == 3)
        */
      case IL_G3: 
      { unsigned long userOptions;
        userOptions = des.compInfo.g3.flags;
        if (userOptions == 0)
            TIFFcompression = 2;
        else {
            if (userOptions & ~0x3f)        /* unhandled bits set: error */
                return ilDeclarePipeInvalid (pipe, IL_ERROR_COMPRESSION);
            group3Options = 0;
            fillOrder = (userOptions & IL_G3M_LSB_FIRST) ? 2 : 1;
            TW_ADD_TAG (IL_TAG_FILL_ORDER, IL_TAG_SHORT, 1, &fillOrder)

            if (userOptions & IL_G3M_2D)
                group3Options |= 1;
            if (userOptions & IL_G3M_UNCOMPRESSED)
                group3Options |= 2;
            if (!(userOptions & IL_G3M_EOL_MARKERS))
                return ilDeclarePipeInvalid (pipe, IL_ERROR_COMPRESSION);
            if (!(userOptions & IL_G3M_EOL_UNALIGNED))
                group3Options |= 4;
            TW_ADD_TAG (IL_TAG_GROUP_3_OPTIONS, IL_TAG_LONG, 1, &group3Options)
            TIFFcompression = 3;
            }
      }
        break;

      case IL_G4:            
      {    unsigned long userOptions;
            userOptions = des.compInfo.g4.flags;
            fillOrder = (userOptions & IL_G4M_LSB_FIRST) ? 2 : 1;
            TW_ADD_TAG (IL_TAG_FILL_ORDER, IL_TAG_SHORT, 1, &fillOrder)
      }
        group4Options = 0;
        TW_ADD_TAG (IL_TAG_GROUP_4_OPTIONS, IL_TAG_LONG, 1, &group4Options)
        TIFFcompression = 4; 
        break;

      case IL_LZW:           
        if (pCompData)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_COMPRESSION);
        TIFFcompression = 5; 
        break;

        /*  JPEG: write JIF with restart markers between strips.
            For JIF-in-TIFF: add tags for a file ptr and length of the JIF image,
            and the restart interval, but don't know value until JIF image received.
        */
      case IL_JPEG:
        jpegProc = 1;           /* baseline sequential process */
        TW_ADD_TAG (IL_TAG_JPEG_PROC, IL_TAG_SHORT, 1, &jpegProc)
        TW_ADD_TAG (IL_TAG_JPEG_INTERCHANGE_FORMAT, IL_TAG_LONG, 1, NULL)
        TW_ADD_TAG (IL_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH, IL_TAG_LONG, 1, NULL)
        TW_ADD_TAG (IL_TAG_JPEG_RESTART_INTERVAL, IL_TAG_SHORT, 1, NULL)
        TW_ADD_TAG (IL_TAG_JPEG_Q_TABLES, IL_TAG_LONG, des.nSamplesPerPixel, NULL)
        TW_ADD_TAG (IL_TAG_JPEG_DC_TABLES, IL_TAG_LONG, des.nSamplesPerPixel, NULL)
        TW_ADD_TAG (IL_TAG_JPEG_AC_TABLES, IL_TAG_LONG, des.nSamplesPerPixel, NULL)

        TIFFcompression = 6;
        break;

      case IL_PACKBITS:      
        if (pCompData)
            return ilDeclarePipeInvalid (pipe, IL_ERROR_COMPRESSION);
        TIFFcompression = 32773; 
        break;

      default:
        return ilDeclarePipeInvalid (pipe, IL_ERROR_COMPRESSION);
        }
    TW_ADD_TAG (IL_TAG_COMPRESSION, IL_TAG_SHORT, 1, &TIFFcompression)

        /*  Image type-specific code: set PhotometricInterpretation: to 4 (transparency
            mask) if a maskImage, else based on image type.
        */
    if (writeMethod == maskImage)
        photo = 4;                      /* special code for transparency mask images */
    else switch (des.type) {
      case IL_BITONAL:
      case IL_GRAY:     photo = (des.blackIsZero) ? 1 : 0; break;
      case IL_PALETTE:  photo = 3; break;
      case IL_RGB:      photo = 2; break;

        /*  YCbCr subsampling currently supported only if JPEG/JIF compressed. */
      case IL_YCBCR:
            /* Add the YCbCr-specific tags: coefficients as fractions of 10000 */
        coeff[0] = des.typeInfo.YCbCr.lumaRed;
        coeff[2] = des.typeInfo.YCbCr.lumaGreen;
        coeff[4] = des.typeInfo.YCbCr.lumaBlue;
        coeff[1] = coeff[3] = coeff[5] = 10000;
        TW_ADD_TAG (IL_TAG_YCBCR_COEFFICIENTS, IL_TAG_RATIONAL, 3, coeff)

            /*  TIFF only allows subsampling as follows: Y = 1, Cr/Cb = 1,2,4 but
                Cr and CB equal to each other; vert <= horiz.
            */
        { ilYCbCrSampleInfo *pSample = des.typeInfo.YCbCr.sample;

        if ((pSample[0].subsampleHoriz != 1)
         || (pSample[0].subsampleVert != 1)
         || (pSample[1].subsampleHoriz != pSample[2].subsampleHoriz)
         || (pSample[1].subsampleVert != pSample[2].subsampleVert)
         || (pSample[1].subsampleVert > pSample[2].subsampleHoriz))
            return ilDeclarePipeInvalid (pipe, IL_ERROR_YCBCR_TIFF);
        subsample[0] = pSample[1].subsampleHoriz;
        subsample[1] = pSample[1].subsampleVert;
        TW_ADD_TAG (IL_TAG_YCBCR_SUBSAMPLING, IL_TAG_SHORT, 2, subsample)

            /*  Set positioning - ? is centered correct ? */
        positioning = 1;
        TW_ADD_TAG (IL_TAG_YCBCR_POSITIONING, IL_TAG_SHORT, 1, &positioning)

        refBW[0]  = pSample[0].refBlack; refBW[1] = 1;
        refBW[2]  = pSample[0].refWhite; refBW[3] = 1;
        refBW[4]  = pSample[1].refBlack; refBW[5] = 1;
        refBW[6]  = pSample[1].refWhite; refBW[7] = 1;
        refBW[8]  = pSample[2].refBlack; refBW[9] = 1;
        refBW[10] = pSample[2].refWhite; refBW[11] = 1;
        TW_ADD_TAG (IL_TAG_REFERENCE_BLACK_WHITE, IL_TAG_RATIONAL, 6, refBW)
        }
        photo = 6;
        break;

      default:
        return ilDeclarePipeInvalid (pipe, IL_ERROR_IMAGE_TYPE);
        }
    TW_ADD_TAG (IL_TAG_PHOTOMETRIC_INTERPRETATION, IL_TAG_SHORT, 1, &photo)

        /*  Write # strips stripOffsets/ByteCounts, but supply no data - called
            function handles these specially and creates blank space, filled in later.
        */
    nStripsPerImage = (info.height + stripHeight - 1) / stripHeight;
    TW_ADD_TAG (IL_TAG_STRIP_OFFSETS, IL_TAG_LONG, nStripsPerImage, NULL)
    TW_ADD_TAG (IL_TAG_STRIP_BYTE_COUNTS, IL_TAG_LONG, nStripsPerImage, NULL)

    nSamplesPerPixel = des.nSamplesPerPixel;
    TW_ADD_TAG (IL_TAG_SAMPLES_PER_PIXEL, IL_TAG_SHORT, 1, &nSamplesPerPixel)

    TW_ADD_TAG (IL_TAG_ROWS_PER_STRIP, IL_TAG_LONG, 1, &stripHeight);

        /*  Write x/yRes: specified in dpi * 2; default to 300 dpi if not given. */
    resolutionUnit = 2;
    TW_ADD_TAG (IL_TAG_RESOLUTION_UNIT, IL_TAG_SHORT, 1, &resolutionUnit)
    if (xRes <= 0) xRes = 600;
    xResolution [0] = xRes;
    xResolution [1] = 2;
    TW_ADD_TAG (IL_TAG_X_RESOLUTION, IL_TAG_RATIONAL, 1, xResolution)
    if (yRes <= 0) yRes = 600;
    yResolution [0] = yRes;
    yResolution [1] = 2;
    TW_ADD_TAG (IL_TAG_Y_RESOLUTION, IL_TAG_RATIONAL, 1, yResolution)

        /*  As per TIFF spec, write planarConfiguration tag only if # samples > 1. */
    if (des.nSamplesPerPixel > 1) {
        planarConfiguration = (format.sampleOrder == IL_SAMPLE_PIXELS) ? 1 : 2;
        TW_ADD_TAG (IL_TAG_PLANAR_CONFIGURATION, IL_TAG_SHORT, 1, &planarConfiguration)
        }

    if (page >= 0) {                        /* else no page tag written */
        pageInfo[0] = page;
        pageInfo[1] = nPages;
        TW_ADD_TAG (IL_TAG_PAGE_NUMBER, IL_TAG_SHORT, 2, pageInfo)
        }

    if (des.compression == IL_LZW) {
        predictor = 1;
        TW_ADD_TAG (IL_TAG_PREDICTOR, IL_TAG_SHORT, 1, &predictor)
        }

        /*  If a palette entry, add a colorMap tag; # colors = 2 ** # bits. 
            If dithered flags bit set, write private tag to mark it as such.
        */
    if (des.type == IL_PALETTE) {
        tagInfo.nPaletteColors = 1 << format.nBitsPerSample [0];
        TW_ADD_TAG (IL_TAG_COLOR_MAP, IL_TAG_SHORT, 3 * tagInfo.nPaletteColors, NULL)

       if (des.flags & IL_DITHERED_PALETTE) {
            ditherLevels[0] = IL_TAG_P0_FLAG_DITHER_LEVELS;
            ditherLevels[1] = des.typeInfo.palette.levels[0];
            ditherLevels[2] = des.typeInfo.palette.levels[1];
            ditherLevels[3] = des.typeInfo.palette.levels[2];
            TW_ADD_TAG (IL_TAG_PRIVATE_0, IL_TAG_SHORT, IL_TAG_P0_DITHER_LEVELS_LENGTH,
                     ditherLevels)
            }
        }
    else tagInfo.pPalette = (unsigned short *)NULL;

        /*  Merge the user tags with the tags formed above, convert the data to
            the proper byte order and return the allocated info to "tagInfo" or error.
        */
    if (error = ilMergeWriteTags (pFile->bigEndian, nTags, tags, nUserTags, pUserTags, 
                                  &tagInfo))
        return ilDeclarePipeInvalid (pipe, error);

        /*  Add consumer pipe element, forcing constant "stripHeight" - ignored if pipe
            image is compressed - stripHeight was forced above by call to ilCompress().
            Use separate function for when writing JPEG.
        */
    srcData.consumerImage = (ilObject)NULL;
    srcData.stripHeight = stripHeight;
    srcData.constantStrip = TRUE;
    srcData.minBufferHeight = 0;
    pPriv = (ilWriteFilePrivPtr)ilAddPipeElement (pipe, IL_CONSUMER, 
        sizeof(ilWriteFilePrivRec), 0, &srcData, (ilDstElementData *)NULL,
        ilWriteFileInit, ilWriteFileCleanup, ilWriteFileDestroy, 
        (des.compression == IL_JPEG) ? ilWriteJPEGExecute : ilWriteFileExecute, 0);
    if (!pPriv)
        return FALSE;

        /*  Fill in private (*pPriv), including template for file image to be added. */
    pPriv->pFile = pFile;
    pPriv->tagInfo = tagInfo;
    pPriv->bigEndian = pFile->bigEndian;
    pPriv->compression = des.compression;
    pPriv->nStrips = nStripsPerImage;

        /*  Point pPriv->tailPtrOffset to location in file to point to this image: "ptr 
            to next" of last image in file (if main image) else ptr to next of parent.
        */
    if (fileImage)                              /* have parent: child or mask image */
        pPriv->tailPtrOffset = ((ilFileImagePtr)fileImage)->tagOffset +
                               ((ilFileImagePtr)fileImage)->tagCount * IL_TAG_SIZE;
    else pPriv->tailPtrOffset = pFile->IFDTailPtrOffset;

    pPriv->fileImage.p.file = (ilFile)pFile;
    pPriv->fileImage.p.pNext = pPriv->fileImage.p.pPrev = 
        pPriv->fileImage.p.pChild = pPriv->fileImage.p.pMask = (ilFileImage)NULL;
    pPriv->fileImage.p.width = info.width;
    pPriv->fileImage.p.height = info.height;
    pPriv->fileImage.p.xRes = xRes;
    pPriv->fileImage.p.yRes = yRes;
    pPriv->fileImage.p.page = page;
    pPriv->fileImage.p.nPages = nPages;
    pPriv->fileImage.p.des = des;
    pPriv->fileImage.p.format = format;
    pPriv->fileImage.p.stripHeight = stripHeight;
    pPriv->fileImage.context = pFile->o.p.context;
    pPriv->fileImage.tagCount = tagInfo.tagCount;

    pipe->context->error = IL_OK;
    return TRUE;
}


        /*  ---------------------- ilInitFile ---------------------------------- */
        /*  Public function: see spec.
            Writes a TIFF file header at position "offset" of file "stream" using
            Motorola order (MM) if "MSBIsFirst", else Intel order (II).
        */
ilBool ilInitFile (
    ilContext           context,
    FILE               *stream,
    long                offset,
    ilBool              MSBIsFirst,
    unsigned long       mustBeZero
    )
{
ilBool                  bigEndian;
ilFilePtr               pFile;
ilByte                 *pHeader;
static ilByte           MSBFirstHeader[]  = { 'M', 'M', 0, 42, 0, 0, 0, 0 };
static ilByte           LSBFirstHeader[] = { 'I', 'I', 42, 0, 0, 0, 0, 0 };

    if (fseek (stream, offset, 0)) {        /* nonzero means error for fseek */
        context->error = IL_ERROR_FILE_IO;
        return FALSE;
        }

    pHeader = (MSBIsFirst) ? MSBFirstHeader : LSBFirstHeader;
    if (fwrite ((char *)pHeader, sizeof (MSBFirstHeader), 1, stream) != 1) {
        context->error = IL_ERROR_FILE_IO;
        return FALSE;
        }
    fflush (stream);                        /* flush header out to file */

    context->error = IL_OK;
    return TRUE;
}

