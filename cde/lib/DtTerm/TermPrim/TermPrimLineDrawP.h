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
