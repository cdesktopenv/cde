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
