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
 * $XConsortium: TermPrimLineDraw.h /main/1 1996/04/21 19:17:51 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimLineDraw_h
#define	_Dt_TermPrimLineDraw_h

typedef struct _LineDrawFontRec *LineDrawFont;

LineDrawFont _DtTermPrimLineDrawCreateFont(Widget w, GlyphInfo glyphInfo,
	int numGlyphs,  int width, int ascent, int descent);
void _DtTermPrimLineDrawFreeFont(LineDrawFont lineDrawFont);
void _DtTermPrimLineDrawImageString(Display *display, Drawable d,
	LineDrawFont lineDrawFont,
	GC gc, GC clearGC, int x, int y, unsigned char *string, int length);
#endif	/* _Dt_TermPrimLineDraw_h */
