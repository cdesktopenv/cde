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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: TermPrimRender.h /main/1 1996/04/21 19:18:43 drk $";
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifndef	_Dt_TermPrimRender_h
#define	_Dt_TermPrimRender_h

typedef struct _TermFontRec *TermFont;

extern void _DtTermPrimRenderText(
    Widget		  w,
    TermFont		  font,
    Pixel		  fg,
    Pixel		  bg,
    unsigned long	  flags,
    int			  x,
    int			  y,
    unsigned char	 *string,
    int			  len
);

extern void _DtTermPrimDestroyFont(
    Widget		  w,
    TermFont		  font
);

extern void _DtTermPrimBell(Widget w);
extern void _termSetRenderFont(Widget w, TermFont *termFont);
extern void _termSetBufferSize(Widget w, int width, int height,
	int xOffset, int yOffset);
extern void _termFreeBuffer(Widget w);
extern void _DtTermPrimRefreshText(Widget w,
	short startColumn, short startRow,
	short endColumn, short endRow);
extern void _DtTermPrimExposeText(Widget w, int startX, int startY, int endX,
	int endY, Boolean isExposeEvent);
extern int _DtTermPrimInsertText(Widget w, unsigned char *buffer, int length);
extern int _DtTermPrimInsertTextWc(Widget w, wchar_t *buffer, int length);
extern void _DtTermPrimRefreshTextWc(Widget w, short startColumn,
	short startRow, short endColumn, short endRow);

extern Boolean
_DtTermPrimParseInput
(
    Widget		  w,
    unsigned char	 *buffer,
    int			  len,
    unsigned char	**dangleBuffer,
    int			 *dangleBufferLen
);

extern void _DtTermPrimCursorOff(Widget w);
extern void _DtTermPrimCursorOn(Widget w);
extern void _DtTermPrimCursorChangeFocus(Widget w);
extern XtInputCallbackProc _termReadPty(XtPointer client_data, int *source,
	XtInputId *id);
extern void _termCompleteScroll(Widget w);
extern void _DtTermPrimFillScreenGap(Widget w);
extern void _DtTermPrimRenderPadLine(Widget w);
#endif	/* _Dt_TermPrimRender_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
