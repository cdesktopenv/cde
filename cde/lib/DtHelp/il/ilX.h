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
/* $XConsortium: ilX.h /main/3 1995/10/23 15:41:07 rswiston $ */
/**---------------------------------------------------------------------
***	
***    file:           ilX.h
***
***    description:    Public declarations and functions for accessing X drawables with IL
***
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



#ifndef ILX_H
#define ILX_H

#ifndef IL_H
#include "il.h"
#endif
#ifndef X_H
#include <X11/X.h>
#endif
#ifndef _XLIB_H_
#include <X11/Xlib.h>
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
#define NeedWidePrototypes 1      /* default to make interropt. easier */
#endif
#endif

#ifdef __cplusplus               /* do not leave open across includes */
extern "C" {                                    /* for C++ V2.0 */
#endif


    /*  Option codes to ilChangeXWC() */
#define IL_XWC_ALLOC_GRAYS              0       /* pData = null */
#define IL_XWC_FREE_GRAYS               1       /* pData = null */
#define IL_XWC_ALLOC_COLORS             2       /* pData = null */
#define IL_XWC_FREE_COLORS              3       /* pData = null */
#define IL_XWC_SET_DITHER_METHOD        4       /* pData -> unsigned int */
#define IL_XWC_SET_RAW_MODE             5       /* pData -> ilBool */

#define IL_XWC_ALLOC_DITHER_COLORS      IL_XWC_ALLOC_COLORS /* no longer used */
#define IL_XWC_FREE_DITHER_COLORS       IL_XWC_FREE_COLORS /* no longer used */

    /*  Values for ilQueryXWCInfo.visualType */
#define IL_XWC_BITONAL                  0
#define IL_XWC_GRAY_8                   1
#define IL_XWC_COLOR_8                  2
#define IL_XWC_COLOR_24                 3

    /*  Values for ilQueryXWCInfo.writeType */
#define IL_XWC_WRITE_BITONAL            0
#define IL_XWC_WRITE_GRAY               1
#define IL_XWC_WRITE_COLOR_DITHER       2
#define IL_XWC_WRITE_COLOR              3

    /*  Masks for "flags" to ilReadXDrawable */
#define IL_READ_X_RAW_MODE              (1<<0)

typedef struct {
    Display            *display;
    Visual             *visual;
    Colormap            colormap;
    GC                  gc;
    unsigned short      visualType;
    ilShortBool         staticVisual;
    unsigned int        ditherMethod;
    ilBool              rawMode;
    unsigned short      writeType;
    short               filler[19];
    } ilXWCInfo;

typedef ilObject ilXWC;


extern ilBool ilReadXDrawable (        
#if NeedFunctionPrototypes
    ilPipe              pipe,
    Display            *display,
    Drawable            drawable,
    Visual             *visual,
    Colormap            colormap,
    ilBool              blackIsZero,
    ilRect             *pSrcRect,
    ilBool              copyToPixmap,
    unsigned long       flags
#endif
    );

extern ilBool ilConvertForXWrite (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    ilXWC               XWC,
    ilBool              mapPixels,
    unsigned long       mustBeZero,
    void               *mustBeNull
#endif
    );

extern ilBool ilWriteXDrawable (
#if NeedFunctionPrototypes
    ilPipe              pipe,
    Drawable            drawable,
    ilXWC               XWC,
    ilRect             *pSrcRect,
    int                 dstX,
    int                 dstY,
    unsigned long       mustBeZero
#endif
    );

extern ilXWC ilCreateXWC (
#if NeedFunctionPrototypes
    ilContext           context,
    Display            *display,
    Visual             *visual,
    Colormap            colormap,
    GC                  gc,
    unsigned long       mustBeZero1,
    unsigned long       mustBeZero2
#endif
    );

extern ilBool ilChangeXWC (
#if NeedFunctionPrototypes
    ilXWC                   XWC,
    unsigned int            code,
    void                   *pData
#endif
    );

extern ilBool ilQueryXWC (
#if NeedFunctionPrototypes
    ilXWC                   XWC,
    ilXWCInfo              *pInfo
#endif
    );


#ifdef __cplusplus
}                                /* for C++ V2.0 */
#endif

#endif
