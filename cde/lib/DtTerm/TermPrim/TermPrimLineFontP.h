/*
 * $XConsortium: TermPrimLineFontP.h /main/1 1996/04/21 19:18:00 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimLineFontP_h
#define	_Dt_TermPrimLineFontP_h

#include "TermPrimLineFont.h"

typedef char charType;

typedef struct {
    char x1;
    signed char x1Offset;
    char y1;
    signed char y1Offset;
    char x2;
    signed char x2Offset;
    char y2;
    signed char y2Offset;
    char width;
} line;

typedef struct {
    char x1;
    signed char x1Offset;
    char y1;
    signed char y1Offset;
    char x2;
    signed char x2Offset;
    char y2;
    signed char y2Offset;
} rect;

typedef struct _GlyphInfoRec {
    charType *chars;
    rect *rects;
    char numRects;
    line *lines;
    char numLines;
    line *stipples;
    char numStipples;
} GlyphInfoRec;

#endif	/* _Dt_TermPrimLineFontP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
