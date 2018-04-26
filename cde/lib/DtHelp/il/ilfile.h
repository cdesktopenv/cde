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
/* $XConsortium: ilfile.h /main/4 1995/12/19 16:54:40 cde-dec $ */
/**---------------------------------------------------------------------
***	
***    file:           ilfile.h
***
***    description:    Public declarations and functions for accessing 
***                    TIFF files with the Image Library (IL).
***	
***
***    (c)Copyright 1991, 1993 Hewlett-Packard Co.
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



#ifndef ILFILE_H
#define ILFILE_H

#include <stdio.h>

#ifndef IL_H
#include "il.h"
#endif

#ifdef DTLIB
/* Used when this header is compiled into the DtHelp library */
#include "GraphicsP.h"
#endif /* DTLIB */

#ifndef NeedFunctionPrototypes
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#define NeedFunctionPrototypes 1
#else
#define NeedFunctionPrototypes 0
#endif /* __STDC__ */
#endif /* NeedFunctionPrototypes */

#ifndef NeedWidePrototypes
#if defined(NARROWPROTO)
#define NeedWidePrototypes 0
#else
#define NeedWidePrototypes 1      /* default to make interropt. easier */
#endif
#endif

#ifdef __cplusplus               /* do not leave open across includes */
extern "C" {                                    /* for C++ V2.0 */
#endif


        /*  ilFileTag.type */
#define IL_TAG_BYTE         1
#define IL_TAG_ASCII        2
#define IL_TAG_SHORT        3
#define IL_TAG_LONG         4
#define IL_TAG_RATIONAL     5
#define IL_MAX_TAG_TYPE     5

        /*  Tags not read or written by the IL - caller
            may read them with ilReadFileTags() or write them
            using ilWriteFileImage().
        */
#define IL_TAG_DOCUMENT_NAME        269
#define IL_TAG_IMAGE_DESCRIPTION    270
#define IL_TAG_MAKE                 271
#define IL_TAG_MODEL                272
#define IL_TAG_PAGE_NAME            285
#define IL_TAG_X_POSITION           286
#define IL_TAG_Y_POSITION           287
#define IL_TAG_TRANSFER_FUNCTION    301
#define IL_TAG_DATE_TIME            306
#define IL_TAG_ARTIST               315
#define IL_TAG_HOST_COMPUTER        316
#define IL_TAG_WHITE_POINT          318
#define IL_TAG_PRIMARY_CHROMATICITIES 319
#define IL_TAG_TRANSFER_RANGE       342

        /*  Tags explicitly read and written (when appropriate)
            by the IL.  These tags cannot be written by the caller
            using ilWriteFileImage(), because the IL-defined values
            will override those specified by the caller.
        */
#define IL_TAG_NEW_SUBFILE_TYPE     254
#define IL_TAG_IMAGE_WIDTH          256
#define IL_TAG_IMAGE_LENGTH         257
#define IL_TAG_BITS_PER_SAMPLE      258
#define IL_TAG_COMPRESSION          259
#define IL_TAG_PHOTOMETRIC_INTERPRETATION 262
#define IL_TAG_FILL_ORDER           266     /* TIFF class F only */
#define IL_TAG_STRIP_OFFSETS        273
#define IL_TAG_SAMPLES_PER_PIXEL    277
#define IL_TAG_ROWS_PER_STRIP       278
#define IL_TAG_STRIP_BYTE_COUNTS    279
#define IL_TAG_X_RESOLUTION         282
#define IL_TAG_Y_RESOLUTION         283
#define IL_TAG_PLANAR_CONFIGURATION 284
#define IL_TAG_GROUP_3_OPTIONS      292
#define IL_TAG_GROUP_4_OPTIONS      293
#define IL_TAG_RESOLUTION_UNIT      296
#define IL_TAG_PAGE_NUMBER          297
#define IL_TAG_PREDICTOR            317
#define IL_TAG_SOFTWARE             305
#define IL_TAG_COLOR_MAP            320
#define IL_TAG_JPEG_PROC            512
#define IL_TAG_JPEG_INTERCHANGE_FORMAT 513
#define IL_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH 514
#define IL_TAG_JPEG_RESTART_INTERVAL 515
#define IL_TAG_JPEG_Q_TABLES        519
#define IL_TAG_JPEG_DC_TABLES       520
#define IL_TAG_JPEG_AC_TABLES       521
#define IL_TAG_YCBCR_COEFFICIENTS   529
#define IL_TAG_YCBCR_SUBSAMPLING    530
#define IL_TAG_YCBCR_POSITIONING    531
#define IL_TAG_REFERENCE_BLACK_WHITE 532
#define IL_TAG_PRIVATE_0          34209

        /*  ilWriteFileImage (method) */
#define IL_WRITE_MAIN       0
#define IL_WRITE_MASK       1
#define IL_WRITE_CHILD      2

        /*  ilWriteFileImage (compression) */
#define IL_WRITE_CURRENT_COMPRESSION    (~0)

typedef ilObject ilFile;

typedef struct _ilFileImage {
    ilFile              file;
    struct _ilFileImage *pNext, *pPrev,
                        *pMask,
                        *pChild;
    long                width, height;
    long                xRes, yRes;
    short               page, nPages;
    ilImageDes          des;
    ilImageFormat       format;
    long                stripHeight;
    } ilFileImagePublicRec;
typedef ilFileImagePublicRec *ilFileImage;

        /*  TIFF file access types */
typedef struct {
    unsigned short      number;
    unsigned short      type;
    unsigned long       nItems;
    ilPtr               pItems;
    } ilFileTag;

extern ilBool ilInitFile (
#if NeedFunctionPrototypes
    ilContext           context,
    FILE               *stream,
    long                offset,
    ilBool              MSBIsFirst,
    unsigned long       mustBeZero
#endif
    );

extern ilFile ilConnectFile (
#if NeedFunctionPrototypes
    ilContext           context,
#ifdef DTLIB
    /* Used when this header is compiled into the DtHelp library */
    _DtGrStream         *stream,
#else
    FILE                *stream,
#endif /* DTLIB */
    long                offset,
    unsigned long       mustBeZero
#endif
    );

extern ilFileImage ilListFileImages (
#if NeedFunctionPrototypes
    ilFile              file,
    unsigned long       mustBeZero
#endif
    );

extern ilPtr ilReadFileTags (
#if NeedFunctionPrototypes
    ilFileImage         fileImage,
    int                 nTags,
    unsigned short     *pTagNumbers,
    ilFileTag         **ppTags,
    unsigned long       mustBeZero
#endif
    );

extern ilBool ilReadFileImage (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    ilFileImage         fileImage,
    ilRect             *pSrcRect,
    unsigned long       mustBeZero
#endif
    );

extern ilBool ilWriteFileImage (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    ilFile              file,
    unsigned int        method,
    ilFileImage         fileImage,
    unsigned int        compression,
    ilPtr               pCompressionData,
    long                stripHeight,
    long                xRes,
    long                yRes,
    short               page,
    short               nPages,
    int                 nTags,
    ilFileTag          *pTags,
    unsigned long       mustBeZero
#endif
    );

#ifdef __cplusplus
}                                /* for C++ V2.0 */
#endif

#endif
