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
/* $XConsortium: ilefstiff.h /main/3 1995/10/23 15:47:21 rswiston $ */
/**---------------------------------------------------------------------
***	
***    file:           ilefstiff.h
***
***    description:    Public include file for TIFF (*fileTypeInfo.Escape)()
***                    Image Library (IL) EFS routine.
***
***	
***    (c)Copyright 1993 Hewlett-Packard Co.
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



#ifndef ILEFSTIFF_H
#define ILEFSTIFF_H 

#ifndef IL_H
#include "il.h"
#endif

#ifndef ILFILE_H
#include "ilfile.h"
#endif

#ifndef ILEFS_H
#include "ilefs.h"
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


    /* option to (*fileTypeInfo.Escape)() */
#define IL_EFS_TIFF_READ_TAGS               0
#define IL_EFS_TIFF_WRITE_IMAGE_WITH_TAGS   1


typedef struct {
    ilEFSFile           file;
    int                 nTags;
    unsigned short     *pTagNumbers;
    ilFileTag         **ppTags;
    unsigned long       mustBeZero;
    ilPtr               tagData;               /* RETURNED */
    } ilEFSTIFFReadTagsInfo;

typedef struct {
    ilPipe              pipe;
    ilEFSFile           file;
    long                xRes;
    long                yRes;
    ilClientImage       maskImage;
    void               *pOptions;
    int                 nTags;
    ilFileTag          *pTags;
    } ilEFSTIFFWriteImageWithTagsInfo;

#ifdef __cplusplus
}                                /* for C++ V2.0 */
#endif

#endif
