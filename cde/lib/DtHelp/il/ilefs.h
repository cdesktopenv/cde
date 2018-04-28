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
/* $XConsortium: ilefs.h /main/3 1995/10/23 15:47:11 rswiston $ */
/**---------------------------------------------------------------------
***	
***    file:           ilefs.h
***
***    description:    Public include file for Image Library (IL) EFS support.
***
***	
***    (c)Copyright 1992 Hewlett-Packard Co.
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



#ifndef ILEFS_H
#define ILEFS_H 

#ifndef IL_H
#include "il.h"
#endif

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
#define NeedWidePrototypes 1
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


    /* openMode to ilEFSOpen() */
#define IL_EFS_READ                 0
#define IL_EFS_READ_SEQUENTIAL      1
#define IL_EFS_WRITE                2

    /* readMode to ilEFSReadImage() */
#define IL_EFS_READ_MAIN            0
#define IL_EFS_READ_MASK            1

    /* masks for bits in ilEFSFileInfo/ilEFSFileTypeInfo.attributes */
#define IL_EFS_MULTI_PAGE_READS     (1<<0)
#define IL_EFS_MULTI_PAGE_WRITES    (1<<1)
#define IL_EFS_MASK_READS           (1<<2)
#define IL_EFS_MASK_WRITES          (1<<3)
#define IL_EFS_SCALEABLE_READS      (1<<4)

    /* masks for bits in ilEFSPageInfo.attributes */
#define IL_EFS_EOF                  (1<<0)
#define IL_EFS_MASKED               (1<<1)
#define IL_EFS_SCALEABLE            (1<<2)

    /* masks for bits in searchOptions to ilEFSOpen() */
#define IL_EFS_BY_TYPE_NAME         (1<<0)
#define IL_EFS_BY_EXTENSION         (1<<1)
#define IL_EFS_BY_CHECKING          (1<<2)
#define IL_EFS_READ_SEARCH          (IL_EFS_BY_EXTENSION | IL_EFS_BY_CHECKING)

    /* String lengths; all values include trailing null. */
#define IL_EFS_MAX_NAME_CHARS       12
#define IL_EFS_MAX_DISPLAY_NAME_CHARS 24
#define IL_EFS_MAX_EXTENSION_CHARS  8

#define IL_EFS_MAX_EXTENSIONS       8

#define IL_EFS_TYPE_RESERVED_SIZE   10


typedef ilObject ilEFSFile;
typedef ilObject ilEFSFileType;

typedef struct {
    ilEFSFileType       fileType;
    unsigned short      openMode;
    unsigned short      howFound;
    unsigned long       attributes;
    long                nPages;
    ilShortBool         inUse;
    short               reserved [21];
    } ilEFSFileInfo;

typedef struct {
    unsigned long       attributes;
    long                page;
    long                width, height;
    long                xRes, yRes;
    ilImageDes          des;
    ilImageFormat       format;
    } ilEFSPageInfo;

typedef struct {
    char                name [IL_EFS_MAX_NAME_CHARS];
    char                displayName [IL_EFS_MAX_DISPLAY_NAME_CHARS];
    unsigned short      checkOrder;
    short               nExtensions;
    char                extensions [IL_EFS_MAX_EXTENSIONS] 
                                   [IL_EFS_MAX_EXTENSION_CHARS];
    unsigned long       openModes;
    unsigned long       attributes;
    ilBool            (*Escape)();
    ilPtr             (*Open)();
    ilBool            (*Seek)();
    ilBool            (*GetPageInfo)();
    ilBool            (*ReadImage)();
    ilBool            (*WriteImage)();
    ilBool            (*Close)();
    ilBool            (*Destroy)();
    long                reserved [IL_EFS_TYPE_RESERVED_SIZE];
    } ilEFSFileTypeInfo;


extern ilEFSFile ilEFSOpen (
#if NeedFunctionPrototypes
    ilContext           context,
    char               *fileName,
    unsigned int        openMode,
    unsigned long       searchOptions,
    char               *typeName,
    void               *pOptions
#endif
    );

extern ilBool ilEFSGetFileInfo (
#if NeedFunctionPrototypes
    ilEFSFile          file,
    ilEFSFileInfo     *pInfo                   /* RETURNED */
#endif
    );

extern ilBool ilEFSSeek (
#if NeedFunctionPrototypes
    ilEFSFile           file,
    long                page,
    void               *pOptions
#endif
    );

extern ilBool ilEFSGetPageInfo (
#if NeedFunctionPrototypes
    ilEFSFile          file,
    ilEFSPageInfo     *pInfo                   /* RETURNED */
#endif
    );

extern ilBool ilEFSReadImage (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    ilEFSFile           file,
    unsigned int        readMode,
    long                width,
    long                height,
    void               *pOptions
#endif
    );

extern ilBool ilEFSWriteImage (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    ilEFSFile           file,
    long                xRes,
    long                yRes,
    ilClientImage       maskImage,
    void               *pOptions
#endif
    );

extern ilBool ilEFSListFileTypes (
#if NeedFunctionPrototypes
    ilContext           context,
    int                *pNFileTypes,            /* RETURNED */
    ilEFSFileType     **pFileTypes              /* RETURNED */
#endif
    );

extern ilBool ilEFSGetFileTypeInfo (
#if NeedFunctionPrototypes
    ilEFSFileType       fileType,
    ilEFSFileTypeInfo  *pInfo                   /* RETURNED */
#endif
    );

extern ilEFSFileType ilEFSAddFileType (
#if NeedFunctionPrototypes
    ilContext           context,
    ilEFSFileTypeInfo  *pInfo,
    void               *pOptions
#endif
    );

#ifdef __cplusplus
}                                /* for C++ V2.0 */
#endif

#endif

