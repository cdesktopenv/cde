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
/*
 * $XConsortium: TermPrimLineDrawP.h /main/1 1996/04/21 19:17:54 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimLineDrawP_h
#define	_Dt_TermPrimLineDrawP_h

#include "TermPrimLineDraw.h"

typedef short cellPositionType;
typedef char segNumType;

#ifdef	USE_PIXMAPS
typedef struct _ScaledBitmapInfoRec {
    Boolean scaled;
    cellPositionType cellX;
    cellPositionType cellY;
} ScaledBitmapInfoRec, *ScaledBitmapInfo;
#else	/* USE_PIXMAPS */
typedef struct _ScaledCharInfoRec {
    Boolean scaled;
    XSegment *segs;
    segNumType numSegs;
    XRectangle *rects;
    segNumType numRects;
    XRectangle *stipples;
    segNumType numStipples;
} ScaledCharInfoRec, *ScaledCharInfo;
#endif	/* USE_PIXMAPS */

/* we will create a linked list of font Data... */
typedef struct _LineDrawFontDataRec {
   GlyphInfo glyphInfo;
   int numGlyphs;
   short lineDrawIndex[256];
   struct _LineDrawFontDataRec *next;
} LineDrawFontDataRec, *LineDrawFontData;

typedef struct _LineDrawFontRec {
    int refCount;
    int width;
    int height;
    int ascent;
    Boolean fontValid;
    Pixmap pixmap;
    Display *display;
    GlyphInfo glyphInfo;
    int numGlyphs;
    short *glyphIndex;
#ifdef	USE_PIXMAPS
    ScaledBitmapInfo scaledBitmapInfo;
#else	/* USE_PIXMAPS */
    ScaledCharInfo scaledCharInfo;
#endif	/* USE_PIXMAPS */
    struct _LineDrawFontRec *next;
} LineDrawFontRec;

#endif	/* _Dt_TermPrimLineDrawP_h */
