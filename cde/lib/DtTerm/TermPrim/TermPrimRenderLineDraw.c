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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$XConsortium: TermPrimRenderLineDraw.c /main/1 1996/04/21 19:18:59 drk $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include <wchar.h>

#include "TermHeader.h"
#include "TermPrimP.h"
#include "TermPrimDebug.h"
#include "TermPrimRenderP.h"
#include "TermPrimRenderLineDraw.h"
#include "TermPrimLineDrawP.h"

static void
LineDrawRenderFunction(
    Widget		  w,
    TermFont		  font,
    Pixel		  fg,
    Pixel		  bg,
    unsigned long	  flags,
    int			  x,
    int			  y,
    unsigned char	 *rawString,
    int			  len
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    XGCValues values;
    unsigned long valueMask;
    LineDrawFont lineDrawFont = (LineDrawFont) font->fontInfo;
    unsigned char *string;

    if (tpd->mbCurMax > 1) {
	/* we have a string of wide chars that need to be converted to
	 * chars...
	 */
	wchar_t			 *wPtr;
	unsigned char		 *ptr;
	int			  i1;
	unsigned char		  mbChar[MB_LEN_MAX];

	/* we will need to convert 2 column characters to 2 spaces in
	 * order to preserve character positions...
	 */
	string = (unsigned char *) XtMalloc(2 * len);
	for (ptr = string, wPtr = (wchar_t *) rawString, i1 = 0; i1 < len;
		i1++, wPtr++) {
	    switch (wcwidth(*wPtr)) {
	    case 1:
		if (wctomb((char *) mbChar, *wPtr) == 1) {
		    *ptr++ = *mbChar;
		} else {
		    *ptr++ = ' ';
		}
		break;

	    case 2:
		*ptr++ = ' ';
		*ptr++ = ' ';
		break;

	    default:
		*ptr++ = ' ';
		break;
	    }
	}

	len = ptr - string;
    } else {
	string = rawString;
    }

    /* set the renderGC... */
    valueMask = (unsigned long) 0;

    /* set the foreground... */
    if (TermIS_SECURE(flags)) {
	if (tpd->renderGC.foreground != bg) {
	    tpd->renderGC.foreground = bg;
	    values.foreground = bg;
	    valueMask |= GCForeground;
	}
    } else {
	if (tpd->renderGC.foreground != fg) {
	    tpd->renderGC.foreground = fg;
	    values.foreground = fg;
	    valueMask |= GCForeground;
	}
    }

    /* set background... */
    if (tpd->renderGC.background != bg) {
	tpd->renderGC.background = bg;
	values.background = bg;
	valueMask |= GCBackground;
    }

    if (valueMask) {
	(void) XChangeGC(XtDisplay(w), tpd->renderGC.gc, valueMask,
		&values);
    }

    /* we need a clear GC as well...
     */
    valueMask = (unsigned long) 0;
    if (tpd->renderReverseGC.foreground != bg) {
	tpd->renderReverseGC.foreground = bg;
	values.foreground = bg;
	valueMask |= GCForeground;
    }
    if (valueMask) {
	(void) XChangeGC(XtDisplay(w),
		tpd->renderReverseGC.gc, valueMask, &values);
    }

    /* line draw a line of text... */
    if (isDebugFSet('t', 1)) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
	/* Fill in the text area so we can see what is going to
	 * be displayed...
	 */
	(void) XFillRectangle(XtDisplay(w),
		XtWindow(w),
		tpd->renderGC.gc,
		x,
		y,
		tpd->cellWidth * len,
		tpd->cellHeight);
	(void) XSync(XtDisplay(w), False);
	(void) shortSleep(100000);
    }
			
    (void) _DtTermPrimLineDrawImageString(
	    XtDisplay(w),			/* Display		*/
	    XtWindow(w),			/* Drawable		*/
	    lineDrawFont,			/* LineDrawFont		*/
	    tpd->renderGC.gc,			/* GC			*/
	    tpd->renderReverseGC.gc,		/* clearGC		*/
	    x,					/* x			*/
	    y + lineDrawFont->ascent,		/* y			*/
	    string,				/* string		*/
	    len);				/* length		*/

    /* we don't support overstrike for line draw... */

    /* handle the underline enhancement... */
    /* draw the underline... */
    if (TermIS_UNDERLINE(flags)) {
	XDrawLine(XtDisplay(w),			/* Display		*/
		XtWindow(w),			/* Window		*/
		tpd->renderGC.gc,		/* GC			*/
		x,				/* X1			*/
		y + tpd->cellHeight - 1,		/* Y1			*/
		x + len * tpd->cellWidth,	/* X2			*/
		y + tpd->cellHeight - 1);	/* Y2			*/
    }

    if (rawString != string) {
	(void) XtFree((char *) string);
    }
}

static void
LineDrawDestroyFunction(
    Widget		  w,
    TermFont		  font
)
{
    LineDrawFont lineDrawFont = (LineDrawFont) font->fontInfo;

    (void) _DtTermPrimLineDrawFreeFont(lineDrawFont);
    (void) XtFree((char *) font);
}

static void
LineDrawExtentsFunction(
    Widget		  w,
    TermFont		  font,
    unsigned char	 *string,
    int			  len,
    int			 *widthReturn,
    int			 *heightReturn,
    int			 *ascentReturn
)
{
    LineDrawFont lineDrawFont = (LineDrawFont) font->fontInfo;

    if (widthReturn) {
	*widthReturn = len * lineDrawFont->width;
    }
    if (heightReturn) {
	*heightReturn = lineDrawFont->height;
    }
    if (ascentReturn) {
	*ascentReturn = lineDrawFont->ascent;
    }
    return;
}

TermFont
_DtTermPrimRenderLineDrawCreate(
    Widget		  w,
    GlyphInfo		  glyphInfo,
    int			  numGlyphs,
    int			  width,
    int			  ascent,
    int			  descent
)
{
    TermFont termFont;

    termFont = (TermFont) XtMalloc(sizeof(TermFontRec));
    termFont->renderFunction = LineDrawRenderFunction;
    termFont->destroyFunction = LineDrawDestroyFunction;
    termFont->extentsFunction = LineDrawExtentsFunction;
    termFont->fontInfo =
      (XtPointer)_DtTermPrimLineDrawCreateFont(w, glyphInfo, numGlyphs,
					       width, ascent, descent);
    return(termFont);
}
