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
/* $XConsortium: ilint.h /main/4 1996/01/08 12:14:49 lehors $ */
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

#ifndef ILINT_H
#define ILINT_H

        /*  General internal definitions for Image Library (IL).
        */

#include <stddef.h>        /* for size_t declaration */
#ifndef IL_H
#include "il.h"
#endif
#ifndef ILPIPELEM_H
#include "ilpipelem.h"
#endif

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0
#define TRUE 1

#ifndef __STDC__
#ifdef NULL
#undef NULL
#endif
#define NULL 0
#endif

    /*  IL_EXTERN is declared in place of "extern" for functions / data
        that are external (in other another source file) but not public
        (meant to be accessed by callers).  ilall.c redefines to null
        so that external references are not generated.
    */
#ifndef IL_EXTERN
#define IL_EXTERN extern
#endif

    /*  IL_PRIVATE is declared for functions / data that are private
        to the IL but need to be referenced by other source modules.
        ilall.c redefines to "static" so external symbol is not generated.
    */
#ifndef IL_PRIVATE
#define IL_PRIVATE
#endif

    /*  New definitions for JPEG.  These may be moved to il.h and exposed
        to users in the future ...
    */

        /*  ilImageDes.compInfo.JPEG.flags masks.  All other bits must be 0 */
#define IL_JPEGM_RAW         (1<<0)

typedef struct {
    short               QTableIndex;
    short               DCTableIndex;
    short               ACTableIndex;
    short               reserved;           /* must be zero (0) */
    } ilJPEGSampleData;

typedef struct {
    ilPtr               QTables[4];
    ilPtr               DCTables[4];
    ilPtr               ACTables[4];
    int                 restartInterval;
    ilJPEGSampleData    sample[IL_MAX_SAMPLES];
    } ilJPEGData;


        /*  Maximums that correspond to defines in /ilinc/il.h .
            WARNING: MUST CHANGE THESE IF IL.H DEFINES CHANGE.
            (These defines are here rather than in the public file to discourage
            public use of "max" defines, which would break if more codes were added
            (for example if application has a table of size MAX_?).
        */
#define IL_MAX_TYPE         4           /* IL_BITONAL, etc. */
#define IL_MAX_COMPRESSION  5           /* IL_UNCOMPRESSED, etc. */
#define IL_MAX_OBJECT_TYPE  7           /* IL_NULL_OBJECT, etc. */
#define IL_MAX_BYTE_ORDER   1           /* IL_MSB_FIRST, etc. */
#define IL_MAX_SAMPLE_ORDER 2           /* IL_SAMPLE_PIXELS, etc. */


        /*  First part of all IL objects.  More object-specific data may follow.  Notes:

            refCount        reference count.  Code which "attaches" to an object (e.g.
                            when an object is referenced in a pipe) should increment
                            refCount, and should call ilDestroyObject() when detaching
                            from the object.  The caller must have the same number of
                            calls to ilDestroyObject() as inc's of refCount for object.
        */

typedef struct {
    ilObjectPublicRec   p;                  /* /ilinc/il.h , e.g. "context" */
    unsigned long       refCount;           /* see notes */
    ilPtr               pNext, pPrev;       /* private to /ilc/ilobject.c */
    void                (*Destroy)();       /* private to /ilc/ilobject.c */
    } ilObjectRec, *ilObjectPtr;


        /*  From /ilc/ilobject.c : */
        /*  Creates an object of size "sizeInBytes" and returns a ptr to it, or null
            if failure (the error code in "context" is set in either case).  The object
            is added to the given "context". "sizeInBytes" must be a minimum of
            "sizeof(ilObjectRec)".
                The function "Destroy" will be called by ilDestroyObject(), as:
                    Destroy (ilObjectPtr pObject);
            Before destroying this object.  The Destroy() function must free any 
            object-specific data etc.  It MUST not touch the fields in the object header
            (ilObjectRec) or free *pObject.
        */

IL_EXTERN ilObjectPtr _ilCreateObject (
    ilContext           context,                /* context to add object to */
    int                 objectType,             /* code for object, e.g. IL_PIPE */
    void                (*Destroy)(),           /* destroy function; see above */
/* Use the portable(correct) type for sizeof() operator, bug report OSF_QAR#32082 */
    size_t              sizeInBytes             /* size of object to create */
    );

        /*  From /ilc/ilimage.c : */
        /*  Validate the given image descriptor at "pDes" and the image format at 
            "pFormat" if it is non-null and validate that they are compatible 
            with each other.
                If "allowPrivateTypes" is true, private image type codes are allowed;
            otherwise an error is declared.  Should be true for client images.
                Return 0 (IL_OK) if valid, else error code.
            NOTE: this function defines valid compressions and nBitsPerSample.
        */
IL_EXTERN ilError _ilValidateDesFormat (
    ilBool                  allowPrivateTypes,
    const ilImageDes     *pDes,
    const ilImageFormat  *pFormat
    );


        /*  From /ilc/ilcontext.c : */
        /*  Intersect the rectangle "*pSrcRect" with the rect "*pDstRect",
            storing the result in "*pDstRect".
        */
IL_EXTERN void _ilIntersectRect (
    ilRect             *pSrcRect,
    ilRect             *pDstRect
    );


        /*  Table of shift values, indexed by YCbCr subsample values (1, 2 or 4) */
IL_EXTERN const int _ilSubsampleShift [];


        /*  Allocate _nBytes from heap and return a ptr to it.
            If IL_GARBAGE_MALLOC defined, init the alloc'd space with garbage
            to help find unit'd vars/image errors.
        */
#ifdef IL_GARBAGE_MALLOC

    IL_EXTERN void *_ilMallocAndInitWithGarbage (unsigned long nBytes);
#   define IL_MALLOC(_nBytes)  (_ilMallocAndInitWithGarbage (_nBytes))

#else

#   define IL_MALLOC(_nBytes)  (malloc (_nBytes))

#endif

        /*  Allocate _nBytes from heap, zero it, and return a ptr to it.
        */
#define IL_MALLOC_ZERO(_nBytes)  (calloc ((_nBytes), 1))

        /*  Reallocate the given _ptr to be of size _nBytes.
        */
#define IL_REALLOC(_ptr, _nBytes)  (realloc ((_ptr), (_nBytes)))

        /*  Free given block (*_ptr), allocated by IL_MALLOC[_ZERO]().
        */
#define IL_FREE(_ptr)       (free (_ptr))

        /*  Print the given message if debug msgs enabled.
        */
#ifdef IL_DEBUG_MSGS
#   define IL_DEBUG(_msg)                (printf (stderr, _msg))
#   define IL_DEBUG_DEC(_msg, _decValue) (printf (stderr, _msg, _decValue))
#else
#   define IL_DEBUG(_msg)
#   define IL_DEBUG_DEC(_msg, _decValue)
#endif


#endif
