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
/* $XConsortium: iltiffint.h /main/4 1995/12/19 16:55:04 cde-dec $ */
#ifndef ILTIFFINT_H
#define ILTIFFINT_H

     /* PRIVATE definitions shared between /ilc/iltiff*.c files.
        NO OTHER CODE SHOULD INCLUDE THIS FILE !
     */
#ifndef ILINT_H
#include "ilint.h"
#endif
#ifndef ILFILE_H
#include "ilfile.h"
#endif

#ifdef DTLIB
/* Used when this header is compiled into the DtHelp library */
#include "GraphicsP.h"
#endif /* DTLIB */

    /*  Defines for values of tag IL_TAG_PRIVATE_0 (34209).  This is a private tag
        read and (sometimes) written by the IL.  The tag is extensible; it is variable
        length, and the data in the tag depends on the flags.  The values are always
        an array of shorts, as follows:
            IL_TAG_P0_FLAGS         array of 16 bits.  Only bit 0 is defined as follows;
                                    all other bits must be zero:
            IL_TAG_P0_FLAG_DITHER_LEVELS ignored if the image is not a palette image; else
                                    image was dithered / error diffused.  The red/green/blue
                                    levels are stored in offsets 1..3; tag must be at least 
                                    IL_TAG_P0_DITHER_LEVELS_LENGTH shorts long.
    */
#define IL_TAG_P0_FLAGS                 0
#define IL_TAG_P0_FLAG_DITHER_LEVELS    (1<<0)
#define IL_TAG_P0_DITHER_LEVELS_LENGTH  4

    /*  Size in bytes of one item (# items = TIFF "length"), for each TIFF tag type.
        Also, # of entries of that type which fit in one long.
        Indexed by tag type = 1..IL_MAX_TAG_TYPE (0 entry unused).
    */
IL_EXTERN int _ilTagTypeItemSizes [];

IL_EXTERN int _ilTagTypeItemsThatFit [];

    /*  How a file image relates to its brethren. */
typedef enum {mainImage, childImage, maskImage} ilFileImageRelation;

    /*  One TIFF file tag, as it appears in a TIFF file. */
typedef struct {
    unsigned short      number;
    unsigned short      type;
/* compatibility problem with long and unsigned long data fields */
    CARD32              length;
    union {
        char            chars[4];
        unsigned short  shorts[2];
/* compatibility problem with long and unsigned long data fields */
        INT32           aLong;
        CARD32          offset;
        } data;
    } ilTIFFTagRec, *ilTIFFTagPtr;

#define IL_TAG_SIZE (sizeof (ilTIFFTagRec))


typedef CARD32        ilFileOffset;         /* a file offset, i.e. byte location */

    /*  Private definition of object ilFileImage.
        This identifies one "IFD" = Image File Directory - not a directory at all,
        it is the header for a file image: # of tags, list of tags, offset to next IFD.
    */
typedef struct {
    ilFileImagePublicRec p;                 /* public part: MUST BE FIRST */
    ilContext           context;            /* context for this file image */
    ilFileImageRelation imageType;          /* main/child/maskImage */
    short               tagCount;           /* # of tags for this image */
    ilFileOffset        tagOffset;          /* file offset to first tag */
    } ilFileImageRec, *ilFileImagePtr;

    /*  Private definition of object ilFile.
        The file exists at byte offset "offset" within "*file".
        See the TIFF spec for a description of TIFF tiles.
        "haveImageList" is true iff there is a *valid* image list;
        there may be a non-null list if false.  The list is invalidated
        when a write to the file is done.
    */
typedef struct _ilFileRec {
    ilObjectRec         o;                  /* std header: MUST BE FIRST */
#ifdef DTLIB
    /* Used when this header is compiled into the DtHelp library */
    _DtGrStream         *stream;             /* stream handle */
#else
    FILE                *stream;             /* stdio file handle */
#endif /* DTLIB */
    ilFileOffset        offset;             /* offset to start of TIFF data within file */
    ilBool              bigEndian;          /* true: "MM" file; false: "II" file */
    ilFileOffset        IFDTailPtrOffset;   /* offset of last IFD ptr */
    ilBool              haveImageList;      /* see above */
    ilFileImagePtr      pFileImageHead,
                        pFileImageTail;     /* ptr to head/tail of file image list */
    } ilFileRec, *ilFilePtr;


    /*  Converts the 4 TIFF bytes at "_4b" to a long, returned to the long "_long",
        based on the byte order specified by "_bigEndian" (TRUE == Motorola order).
        Note that the 4 bytes are not necessarily on a long boundary,
        so they must be shifted up in case long alignment is required.
    */
/* compatibility problem with long and unsigned long data fields */
#define IL_FLIP_LONG(_bigEndian, _4b, _long) {  \
    CARD32 _l;                  \
    if (_bigEndian) {                           \
        _l = *((ilPtr)(_4b)+0); _l <<= 8;       \
        _l |= *((ilPtr)(_4b)+1); _l <<= 8;      \
        _l |= *((ilPtr)(_4b)+2); _l <<= 8;      \
        _l |= *((ilPtr)(_4b)+3);                \
        }                                       \
    else {                                      \
        _l = *((ilPtr)(_4b)+3); _l <<= 8;       \
        _l |= *((ilPtr)(_4b)+2); _l <<= 8;      \
        _l |= *((ilPtr)(_4b)+1); _l <<= 8;      \
        _l |= *((ilPtr)(_4b)+0);                \
        }                                       \
    (_long) = _l;                               \
    }

    /*  Same for a short.
    */
#define IL_FLIP_SHORT(_bigEndian, _2b, _short) {\
    unsigned short _s;                 \
    if (_bigEndian) {                           \
        _s = *((ilPtr)(_2b)+0); _s <<= 8;       \
        _s |= *((ilPtr)(_2b)+1);                \
        }                                       \
    else {                                      \
        _s = *((ilPtr)(_2b)+1); _s <<= 8;       \
        _s |= *((ilPtr)(_2b)+0);                \
        }                                       \
    (_short) = _s;                              \
    }

    /*  Seek to the given offset within the given file, taking into
        account the offset at which the logical file begins.
        Returns true if seek successful, else false.
    */
#ifdef DTLIB
/* Used when this header is compiled into the DtHelp library */
#define IL_SEEK(_pFile, _offset) \
    ( _DtGrSeek ((_pFile)->stream, (_pFile)->offset + (_offset), 0) == 0 )
#else
#define IL_SEEK(_pFile, _offset) \
    ( fseek ((_pFile)->stream, (_pFile)->offset + (_offset), 0) == 0 )
#endif /* DTLIB */

    /*  Read the given number of bytes into the given buffer.
        All read macros return true if read successful, else false.
    */
#ifdef DTLIB
/* Used when this header is compiled into the DtHelp library */
#define IL_READ(_pFile, _nBytes, _pBuffer) \
    ( (_DtGrRead ((char *)(_pBuffer), _nBytes, 1, (_pFile)->stream) == 1) ? \
      TRUE : FALSE )
#else
#define IL_READ(_pFile, _nBytes, _pBuffer) \
    ( (_DtGrRead ((char *)(_pBuffer), _nBytes, 1, (_pFile)->stream) == 1) ? \
      TRUE : FALSE )
#endif /* DTLIB */

    /*  Write the given number of bytes into the given buffer.
        Returns true if read successful, else false.
    */
#define IL_WRITE(_pFile, _nBytes, _pBuffer) \
    ( (fwrite ((char *)(_pBuffer), _nBytes, 1, (_pFile)->stream) == 1) ? \
      TRUE : FALSE )


#endif
