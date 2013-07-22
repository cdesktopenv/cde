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
static char rcs_id[] = "$TOG: TermPrimRender.c /main/3 1999/07/20 17:34:54 mgreess $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994, 1996 Hewlett-Packard Company               *
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp. *
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.                *
 * (c) Copyright 1993, 1994, 1996 Novell, Inc.                          *
 * (c) Copyright 1996 Digital Equipment Corporation.			*
 * (c) Copyright 1996 FUJITSU LIMITED.					*
 * (c) Copyright 1996 Hitachi.						*
 */

#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermPrimP.h"
#include "TermPrimI.h"
#include "TermPrimData.h"
#include "TermPrimLineDraw.h"
#include "TermPrimBufferP.h"
#include "TermPrimRender.h"
#include "TermPrimRenderP.h"
#include "TermPrimSelect.h"
#include "TermPrimSelectP.h"
#include "TermPrimMessageCatI.h"
#include <limits.h>

#ifdef	DKS
void
_termSetRenderFont(Widget w, TermFont *termFont)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int i;

    /* DKS: ** alert ** alert ** alert ** alert ** alert **
     *
     *	    The following opens up a memory leak that needs to be closed.
     *	    we need to free up both the list and any created entries...
     */

    /* do initialization... */
    if (!tpd->renderTermFontsNum) {
	tpd->renderTermFontsNum = 4;
	tpd->renderTermFonts = (TermFont **) malloc((unsigned)
		tpd->renderTermFontsNum * sizeof(TermFont *));
	(void) memset(tpd->renderTermFonts, '\0',
		tpd->renderTermFontsNum * sizeof(TermFont *));
    }

    /* replace the font if we already have one defined, else add it... */
    for (i = 0; i < tpd->renderTermFontsNum; i++) {
	if (tpd->renderTermFonts[i] && (tpd->renderTermFonts[i]->id ==
		termFont->id))
	    /* found an existing font with this id... */
	    break;
    }

    if (i >= tpd->renderTermFontsNum) {
	/* we didn't find one, so let's add this one to the list... */
	for (i = 0; i < tpd->renderTermFontsNum; i++) {
	    if (!tpd->renderTermFonts[i])
		break;
	}

	/* check for list full... */
	if (i >= tpd->renderTermFontsNum) {
	    (void) fprintf(stderr,
		    "setRenderFont: list full can't add font to widget 0x%lx\n",
		    w);
	}

	/* we found an empty spot and need to malloc storage... */
	tpd->renderTermFonts[i] = (TermFont *) malloc(sizeof(TermFont));
	/* we don't need to initialize it as we will do a copy below... */
    }

    (void) memcpy(tpd->renderTermFonts[i], termFont, sizeof(TermFont));
    if ((0 == tpd->cellWidth) || ('@' == termFont->id)) {
	tpd->cellWidth = termFont->width;
	tpd->cellHeight = termFont->height;

	tw->term.widthInc = termFont->width;
	tw->term.heightInc = termFont->height;
	tw->term.ascent = termFont->ascent;
    }
}
#endif	/* DKS */

void
_DtTermPrimBell(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int i;

    if (tw->term.visualBell) {
	/* the speed of this operation is not critical, so we will just
	 * use the standard text rendering GC and restore it after we
	 * are done...
	 */
	if (tpd->renderGC.foreground !=
		(tw->primitive.foreground ^ tw->core.background_pixel)) {
	    tpd->renderGC.foreground = 
		    tw->primitive.foreground ^ tw->core.background_pixel;
	    (void) XSetForeground(XtDisplay(w), tpd->renderGC.gc,
		    tpd->renderGC.foreground);
	}
	(void) XSetFunction(XtDisplay(w), tpd->renderGC.gc, GXxor);
	for (i = 0; i < 2; i++) {
	    (void) XFillRectangle(XtDisplay(w),	/* Display		*/
		    XtWindow(w),		/* Drawable		*/
		    tpd->renderGC.gc,		/* GC			*/
		    tpd->offsetX,		/* x			*/
		    tpd->offsetY,		/* y			*/
		    tw->term.columns * tpd->cellWidth,
						/* width		*/
		    tw->term.rows * tpd->cellHeight);
						/* height		*/
	    (void) XSync(XtDisplay(w), 0);
	}
	/* restore the GC... */
	(void) XSetFunction(XtDisplay(w), tpd->renderGC.gc, GXcopy);
    } else {
	(void) XBell(XtDisplay(w), 0);
    }
}

void
_DtTermPrimRefreshText(Widget w, short startColumn, short startRow,
	short endColumn, short endRow)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    TermBuffer tBuffer = tpd->termBuffer;
    int lineWidth;
    unsigned char *linePtr;
    TermFont termFont;
    int currentColorPair = 0;
    int currentVideo = 0;
    short chunkStartColumn;
    short chunkWidth;
    enhValues enhancements;
    int i;
    int lineNum;
    unsigned long valueMask;
    GC gc;
    XGCValues values;
    TermEnhInfoRec enhInfo;
    Boolean checkSelection = False;
    int selectionEnd;
    Pixel tmpPixel;
    XmTextPosition  begin, end;
    
    DebugF('t', 0, fprintf(stderr, ">>_DtTermPrimRefreshText() starting\n"));
    DebugF('t', 0, fprintf(stderr,
	    ">>_DtTermPrimRefreshText() startCol=%hd  startRow=%hd  endCol=%hd  endRow=%hd\n",
	    startColumn, startRow, endColumn, endRow));

    if (tpd->mbCurMax > 1)
    {
        _DtTermPrimRefreshTextWc(w, startColumn, startRow, endColumn, endRow);
        return;
    }

    /* clip start/end x/y... */
    if (startColumn <= 0)
	startColumn = 0;
    if (startRow <= 0)
	startRow = 0;
    if (endColumn >= tw->term.columns)
	endColumn = tw->term.columns - 1;
    if (endRow >= tw->term.rows)
	endRow = tw->term.rows - 1;

    /*
    ** don't display if we are in jump scroll and in the process
    ** of scrolling and inside the scroll region...
    */
    if (tw->term.jumpScroll && tpd->scroll.jump.scrolled) {
	/* set all the scrollRefreshRows flags... */
	for (; startRow <= endRow; startRow++) {
	    tpd->scrollRefreshRows[startRow] = True;
	}

	DebugF('t', 0,
		fprintf(stderr,
		">>_DtTermPrimRefreshText() jump scroll in progress, no render\n"));
        return;
    }

    if (!tpd->renderGC.gc) {
	/* get a drawImageString GC... */
	int i;
	XGCValues values;

	/***********************************************************
	 * renderGC...
	 */
	/* set the GC fg and bg... */
	values.foreground = tw->term.reverseVideo ?
		tw->core.background_pixel : tw->primitive.foreground;
	values.background = tw->term.reverseVideo ?
		tw->primitive.foreground : tw->core.background_pixel;

	tpd->renderGC.gc = XCreateGC(XtDisplay(w), XtWindow(w),
		GCForeground | GCBackground, &values);

	/* set the GC cache values... */
	tpd->renderGC.foreground = values.foreground;
	tpd->renderGC.background = values.background;
	tpd->renderGC.fid = (Font) 0;

	/***********************************************************
	 * renderReverseGC...
	 */
	values.foreground = tw->term.reverseVideo ?
		tw->primitive.foreground : tw->core.background_pixel;
	values.background = tw->term.reverseVideo ?
		tw->core.background_pixel : tw->primitive.foreground;

	tpd->renderReverseGC.gc = XCreateGC(XtDisplay(w), XtWindow(w),
		GCForeground | GCBackground, &values);

	/* set the GC cache values... */
	tpd->renderReverseGC.foreground = values.foreground;
	tpd->renderReverseGC.background = values.background;
	tpd->renderReverseGC.fid = (Font) 0;

	/***********************************************************
	 * clearGC...
	 */
	values.foreground = tw->term.reverseVideo ?
		tw->primitive.foreground : tw->core.background_pixel;
	values.background = tw->term.reverseVideo ?
		tw->core.background_pixel : tw->primitive.foreground;

	tpd->clearGC.gc = XCreateGC(XtDisplay(w), XtWindow(w),
		GCForeground | GCBackground, &values);

	/* set the GC cache values... */
	tpd->clearGC.foreground = values.foreground;
	tpd->clearGC.background = values.background;
	tpd->clearGC.fid = (Font) 0;
    }

#ifdef SUN_MOTIF_PERF
    /* use the clear GC... */
    gc = tpd->clearGC.gc;

    valueMask = (unsigned long) 0;
    if (tpd->clearGC.foreground !=
	    tw->term.reverseVideo ?
	    tw->core.background_pixel : tw->primitive.foreground) {
        values.foreground =
	        tw->term.reverseVideo ?
	        tw->primitive.foreground :
	        tw->core.background_pixel;
        tpd->clearGC.foreground = values.foreground;
        valueMask |= GCForeground;
    }
    if (tpd->clearGC.background !=
	    tw->term.reverseVideo ?
	    tw->primitive.foreground :
	    tw->core.background_pixel) {
        values.background =
	        tw->term.reverseVideo ?
	        tw->core.background_pixel :
	        tw->primitive.foreground;
        tpd->clearGC.background = values.background;
        valueMask |= GCBackground;
    }
    if (valueMask) {
        (void) XChangeGC(XtDisplay(w), tpd->clearGC.gc,
	        valueMask, &values);
    }
    (void) XFillRectangle(XtDisplay(w),         /* Display		*/
		    XtWindow(w),		/* Drawable		*/
		    gc,				/* GC			*/
		    startColumn * tpd->cellWidth + tpd->offsetX,
						/* x			*/
		    startRow * tpd->cellHeight + tpd->offsetY,
						/* y			*/
		    (endColumn - startColumn + 1) * tpd->cellWidth,
						/* width		*/
		    (endRow - startRow + 1) * tpd->cellHeight);		
						/* height		*/
#endif /* SUN_MOTIF_PERF */

    for (; startRow <= endRow; startRow++) {
	/* if we are refreshing a full line, then we can clear the
	 * scrollRefreshRows flag for this line...
	 */
	if ((startColumn == 0) && (endColumn >= tw->term.columns - 1)) {
	    tpd->scrollRefreshRows[startRow] = False;
	}

	lineNum = startRow + tpd->topRow;
	if (!tw->term.jumpScroll && tpd->scroll.nojump.pendingScroll) {
            if (lineNum != tpd->scrollLockBottomRow)
            {
                lineNum -= tpd->scroll.nojump.pendingScrollLines;
            }
    	}

	/* are we in the selected area?... */
        if (tpd->useHistoryBuffer)
        {
            if (_DtTermPrimSelectGetSelection(w, &begin, &end) &&
                (begin < end) &&
                (lineNum >= (begin / (tpd->selectInfo->columns + 1)) - 
                            tpd->lastUsedHistoryRow) &&
                (lineNum <= (end   / (tpd->selectInfo->columns + 1)) - 
                            tpd->lastUsedHistoryRow)) {
                checkSelection = True;
            } else {
                checkSelection = False;
            }
        }
        else
        {
            if (_DtTermPrimSelectGetSelection(w, &begin, &end) &&
                (begin < end) &&
                (lineNum >= (begin / (tpd->selectInfo->columns + 1))) &&
                (lineNum <= (end   / (tpd->selectInfo->columns + 1))))  {
                checkSelection = True;
            } else {
                checkSelection = False;
            }
        }

	chunkStartColumn = startColumn;
	if (startColumn > endColumn) {
	    /* nothing to render on this line... */
	    continue;
	}

	if (lineNum >= tpd->lastUsedRow) {
	    /* we are pointing to empty screen space below the last used
	     * line of the display...
	     */
	    lineWidth = 0;
	    linePtr = NULL;
	} else if (lineNum < 0) {
	    if ((tpd->useHistoryBuffer) &&
		    (-lineNum <= tpd->lastUsedHistoryRow)) {
		/* get a line out of the history buffer... */
		lineWidth = MAX(0, MIN(endColumn - startColumn + 1,
		    _DtTermPrimBufferGetLineWidth(tpd->historyBuffer,
		    tpd->lastUsedHistoryRow + lineNum) - startColumn));
		linePtr =
		    _DtTermPrimBufferGetCharacterPointer(tpd->historyBuffer,
		    tpd->lastUsedHistoryRow + lineNum, startColumn);
	    } else {
		/* we are above the history buffer.  Should not happen, but...
		 */
		lineWidth = 0;
		linePtr = NULL;
	    }
	} else {
	    /* get the line width and a pointer to the data... */
	    lineWidth = MAX(0, MIN(endColumn - startColumn + 1,
		    _DtTermPrimBufferGetLineWidth(tBuffer, lineNum) -
		    startColumn));
	    linePtr = _DtTermPrimBufferGetCharacterPointer(tBuffer, lineNum,
		    startColumn);
	}

	while (lineWidth > 0) {
	    /* get the enhancement values for the first chunk of the
	     * string...
	     */
	    if (lineNum >= 0) {
		(void) _DtTermPrimBufferGetEnhancement(tBuffer,
						/* TermBuffer		*/
			lineNum,		/* row			*/
			chunkStartColumn,	/* col			*/
			&enhancements,		/* enhancements		*/
			&chunkWidth,		/* width        	*/
			countAll);		/* countWhich		*/
	    } else {
		/* get it from the history buffer... */
		(void) _DtTermPrimBufferGetEnhancement(tpd->historyBuffer,
						/* TermBuffer		*/
			tpd->lastUsedHistoryRow + lineNum,
						/* row			*/
			chunkStartColumn,	/* col			*/
			&enhancements,		/* enhancements		*/
			&chunkWidth,		/* width		*/
			countAll);		/* countWhich		*/
	    }

	    /* clip chunkWidth... */
	    if (chunkWidth > lineWidth)
		chunkWidth = lineWidth;

	    /* set reasonable defaults for our render info... */
	    enhInfo.fg = tw->primitive.foreground;
	    enhInfo.bg = tw->core.background_pixel;
	    enhInfo.font = tpd->defaultTermFont;
	    enhInfo.flags = (unsigned long) 0;

	    /* set our font and color from the enhancements... */
	    if (ENH_PROC(tBuffer)) {
                (void) (*(ENH_PROC(tBuffer)))(w, enhancements, &enhInfo);
	    }

	    /* if we are in reverse video mode... */
	    if (tw->term.reverseVideo) {
		/* flip fg and bg... */
		tmpPixel = enhInfo.fg;
		enhInfo.fg = enhInfo.bg;
		enhInfo.bg = tmpPixel;
	    }

	    /* are we in the selection area?... */
	    if (checkSelection && 
                _DtTermPrimSelectIsInSelection(w, lineNum, chunkStartColumn,
		                         chunkWidth, &chunkWidth)) {

		/* flip fg and bg... */
		tmpPixel = enhInfo.fg;
		enhInfo.fg = enhInfo.bg;
		enhInfo.bg = tmpPixel;
	    }
			
	    /* if secure, we will use a XFillRectangle, and we need
	     * foreground set to the background...
	     */

	    if (TermIS_SECURE(enhInfo.flags)) {
		/* render secure video locally...
		 */
		/* set the renderReverseGC... */
		valueMask = (unsigned long) 0;
		if (tpd->renderReverseGC.foreground != enhInfo.bg) {
		    tpd->renderReverseGC.foreground = enhInfo.bg;
		    values.foreground = enhInfo.bg;
		    valueMask |= GCForeground;
		}
		if (valueMask) {
		    (void) XChangeGC(XtDisplay(w), tpd->renderReverseGC.gc,
			    valueMask, &values);
		}
		(void) XFillRectangle(XtDisplay(w),
			XtWindow(w),
			tpd->renderReverseGC.gc,
			chunkStartColumn * tpd->cellWidth + tpd->offsetX,
			startRow * tpd->cellHeight + tpd->offsetY,
			tpd->cellWidth *  chunkWidth,
			tpd->cellHeight);

		/* underline as well... */
		if (TermIS_UNDERLINE(enhInfo.flags)) {
		    valueMask = (unsigned long) 0;
		    if (tpd->renderGC.foreground != enhInfo.fg) {
			tpd->renderGC.foreground = enhInfo.fg;
			values.foreground = enhInfo.fg;
			valueMask |= GCForeground;
		    }
		    if (valueMask) {
			(void) XChangeGC(XtDisplay(w), tpd->renderGC.gc,
				valueMask, &values);
		    }
		    (void) XDrawLine(XtDisplay(w),
						/* Display		*/
			    XtWindow(w),	/* Drawable		*/
			    tpd->renderGC.gc,	/* GC			*/
			    chunkStartColumn * tpd->cellWidth + tpd->offsetX,
						/* X1			*/
			    startRow * tpd->cellHeight + tpd->offsetY +
			    tpd->cellHeight - 1,
						/* Y1			*/
			    (chunkStartColumn + chunkWidth) * tpd->cellWidth +
			    tpd->offsetX,	/* X2			*/
			    startRow * tpd->cellHeight + tpd->offsetY +
			    tpd->cellHeight - 1);
						/* Y2			*/
		}
	    } else {
		(void) _DtTermPrimRenderText(
		    w,				/* Widget		*/
		    enhInfo.font,		/* TermFont		*/
		    enhInfo.fg,			/* fg Pixel		*/
		    enhInfo.bg,			/* bg Pixel		*/
		    enhInfo.flags,		/* flags		*/
		    chunkStartColumn * tpd->cellWidth + tpd->offsetX,
						/* x			*/
		    startRow * tpd->cellHeight + tpd->offsetY,
						/* y			*/
		    linePtr,			/* string		*/
		    chunkWidth);		/* width		*/
	    }
	    chunkStartColumn += chunkWidth;
	    lineWidth -= chunkWidth;
	    linePtr += chunkWidth;
	}

	/* clear any extra space in the line.  chunkStartColumn now points to
	 * the end of the line, and lineWidth == 0...
	 */
	while (endColumn - chunkStartColumn >= 0) {
	    chunkWidth = endColumn - chunkStartColumn + 1;
	    if (checkSelection && 
                _DtTermPrimSelectIsInSelection(w, lineNum, chunkStartColumn,
		                         chunkWidth, &chunkWidth)) {
		/* use the render gc set to the fg color... */
		gc = tpd->renderReverseGC.gc;

		valueMask = (unsigned long) 0;
		if (tpd->renderReverseGC.foreground !=
			tw->term.reverseVideo ?
			tw->core.background_pixel : tw->primitive.foreground) {
		    values.foreground =
			    tw->term.reverseVideo ?
			    tw->core.background_pixel :
			    tw->primitive.foreground;
		    tpd->renderReverseGC.foreground = values.foreground;
		    valueMask |= GCForeground;
		}
		if (valueMask) {
		    (void) XChangeGC(XtDisplay(w), tpd->renderReverseGC.gc,
			    valueMask, &values);
		}
#ifndef SUN_MOTIF_PERF
	    } else {
		/* use the clear GC... */
		gc = tpd->clearGC.gc;

		valueMask = (unsigned long) 0;
		if (tpd->clearGC.foreground !=
			tw->term.reverseVideo ?
			tw->core.background_pixel : tw->primitive.foreground) {
		    values.foreground =
			    tw->term.reverseVideo ?
			    tw->primitive.foreground :
			    tw->core.background_pixel;
		    tpd->clearGC.foreground = values.foreground;
		    valueMask |= GCForeground;
		}
		if (tpd->clearGC.background !=
			tw->term.reverseVideo ?
			tw->primitive.foreground :
			tw->core.background_pixel) {
		    values.background =
			    tw->term.reverseVideo ?
			    tw->core.background_pixel :
			    tw->primitive.foreground;
		    tpd->clearGC.background = values.background;
		    valueMask |= GCBackground;
		}
		if (valueMask) {
		    (void) XChangeGC(XtDisplay(w), tpd->clearGC.gc,
			    valueMask, &values);
		}
	    }
#endif /* not SUN_MOTIF_PERF */

	    if (isDebugFSet('t', 1)) {
#ifdef	BBA
#pragma BBA_IGNORE
#endif	/*BBA*/
		/* Fill in the clear area so we can see what is going to
		 * be displayed...
		 */
		(void) XFillRectangle(XtDisplay(w),
			XtWindow(w),
			tpd->renderGC.gc,
			chunkStartColumn * tpd->cellWidth + tpd->offsetX,
			startRow * tpd->cellHeight + tpd->offsetY,
			chunkWidth * tpd->cellWidth,
			tpd->cellHeight);
		(void) XSync(XtDisplay(w), False);
		(void) shortSleep(100000);
	    }
	    (void) XFillRectangle(XtDisplay(w),
						/* Display		*/
		    XtWindow(w),		/* Drawable		*/
		    gc,				/* GC			*/
		    chunkStartColumn * tpd->cellWidth + tpd->offsetX,
						/* x			*/
		    startRow * tpd->cellHeight + tpd->offsetY,
						/* y			*/
		    chunkWidth * tpd->cellWidth,
						/* width		*/
		    tpd->cellHeight);		/* height		*/
#ifdef SUN_MOTIF_PERF
	    }
#endif /* SUN_MOTIF_PERF */
	    chunkStartColumn += chunkWidth;
	}
    }
    DebugF('t', 0, fprintf(stderr, ">>_DtTermPrimRefreshText() finished\n"));
}


/**************************************************************************
 *  Function:
 *  	_DtTermPrimExposeText(): expose (refresh) the text in rectangular area
 *  
 *  Parameters:
 *  	Widget w: widget to expose
 *  	int x: starting x pixel in window
 *  	int y: starting y pixel in window
 *  	int width: width in pixels
 *  	int height: height in pixels
 *	Boolean isExposeEvent: true, deal with copyarea / expose overlap
 *  
 *  Returns:
 *  	<nothing>
 *  
 *  Notes:
 *  	This function will redisplay the text in a rectangular exposure
 *  	area.  The x, y, width, and height are in absolute pixels.  The
 *  	internal x and y offsets will be accounted for, and the minimum
 *  	number of characters will be displayed.  The algorithm has been
 *  	tuned (somewhat) and no longer exposes an extra character at the
 *  	end of the refresh lines and an extra line at the bottom of the
 *  	refresh area.  This can be verified by using the 't' and 'e'
 *  	debug flags.
 */

void
_DtTermPrimExposeText(Widget w, int x, int y, int width, int height,
	Boolean isExposeEvent)
{
    struct termData *tpd = ((DtTermPrimitiveWidget) w)->term.tpd;

    DebugF('e', 0, fprintf(stderr, ">>exposeText() starting\n"));
    DebugF('e', 0, fprintf(stderr,
	    ">>exposeText() startX=%d  startY=%d  width=%d  height=%d\n",
	    x, y, width, height));
    DebugF('e', 0, fprintf(stderr,
	    ">>             offsetX=%d  offsetY=%d  cellHeight=%d  cellWidth=%d\n",
	    tpd->offsetX, tpd->offsetY, tpd->cellHeight, tpd->cellWidth));

    /* The following "hack" takes care of the problem of an exposure event
     * from the server and a copy area from the client crossing.  The
     * combination of these two events can cause a race condition which
     * manifests itself by leaving a hole in the terminal window.  What
     * happens is this:
     * 
     * A window is partially obscured.  The terminal emulator does a copy
     * area (scroll) which includes part of obscured area.  Before the
     * server processes the copy area, the window is unobscured, and the
     * server sends an exposure event back to the client.
     * 
     * - The window is partially obscured.
     * 
     * - The terminal emulator does a copy area (scroll) which includes a
     *   portion of the obscured area.  Normally, the server will generate
     *   a graphics exposure event for the obscured portion that it can't
     *   copy which will allow the terminal emulator to update the area.
     * 
     * - Before the server receives the copy area request, the server
     *   unobscures the window and sends an exposure event for the exposed
     *   area.  (This is where the hack comes into play and refreshes the
     *   scrolled portion of this area (and possibly some extra space as
     *   well)).
     * 
     * - The server processes the copy area.  Since the area in question is
     *   no longer obscured, the server will copy blank space and not
     *   generate a graphics exposure event.
     * 
     * - The terminal emulator processes the exposure event and refreshes
     *   the exposed area.  (This is the hack extends the exposure to cover
     *   the gap).
     * 
     * - You now have a blank chunk of the terminal window where the
     *   obscured area was scrolled (without the hack).
     * 
     * Our fix is similar to the one used in xterm.  The Motif text widget
     * uses a more brute force method and simply extends the exposure event
     * to the full height (or width) of the screen in the direction of the
     * copy area.
     */
    if (isExposeEvent && tpd->scrollInProgress) {
	int bothX1;
	int bothX2;
	int bothY1;
	int bothY2;

	bothX1 = MAX(tpd->scrollSrcX, x);
	bothY1 = MAX(tpd->scrollSrcY, y);
	bothX2 = MIN(tpd->scrollSrcX + tpd->scrollWidth, x + width - 1);
	bothY2 = MIN(tpd->scrollSrcY + tpd->scrollHeight, y + height - 1);

	if ((bothX2 > bothX1) && (bothY2 > bothY1)) {
	    (void) _DtTermPrimRefreshText(w,
		    (x - tpd->offsetX + tpd->scrollDestX - tpd->scrollSrcX) /
		    tpd->cellWidth,
		    (y - tpd->offsetY + tpd->scrollDestY - tpd->scrollSrcY) /
		    tpd->cellHeight,
		    (x + width - 1 - tpd->offsetX + tpd->scrollDestX -
		    tpd->scrollSrcX) / tpd->cellWidth,
		    (y + height - 1 - tpd->offsetY + tpd->scrollDestY -
		    tpd->scrollSrcY) / tpd->cellHeight);
	}
    }

    /* render the text... */
    DebugF('t', 0, fprintf(stderr, ">>exposeText() calling _DtTermPrimRefreshText()\n"));
    (void) _DtTermPrimRefreshText(w, (x - tpd->offsetX) / tpd->cellWidth,
	    (y - tpd->offsetY)  / tpd->cellHeight,
	    (x + width - 1 - tpd->offsetX) / tpd->cellWidth,
	    (y + height - 1 - tpd->offsetY) / tpd->cellHeight);

    DebugF('e', 0, fprintf(stderr, ">>exposeText() finished\n"));
}

void
_DtTermPrimFillScreenGap(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;    
    struct termData *tpd = tw->term.tpd;
    TermBuffer tBuffer = tpd->termBuffer;
    int linesNeeded;
    int linesCopied;
    int historyLinesNeeded;
    int i1;
    TermLineSelection selectionFlag;

    /* before we insert any text, we need to insure that the cursor is
     * on a valid buffer row.  If not, we will need to fill in any gap
     * between the lastUsedRow and the current cursor postion with 0
     * width lines, and get a buffer row for the current cursor row...
     */
    if (tpd->topRow + tpd->cursorRow >= tpd->lastUsedRow) {
	/* if there are lines left in the buffer, just use them...
	 */
	/* this does not effect the position of the selection... */
	for (; (tpd->lastUsedRow < tpd->bufferRows) &&
		(tpd->topRow + tpd->cursorRow >= tpd->lastUsedRow); ) {
	    /* all we need to do is clear the line... */
	    _DtTermPrimBufferClearLine(tBuffer, tpd->lastUsedRow++, 0);
	}

	/* if we still need more lines, then we will need to steal from
	 * the top of the buffer...
	 */
	if (tpd->topRow + tpd->cursorRow >= tpd->lastUsedRow) {
	    if (tpd->memoryLockMode == SCROLL_LOCKprotect) {
		if (!tpd->warningDialogMapped) {
		    (void) _DtTermPrimWarningDialog(w,
			    GETMESSAGE(NL_SETN_PrimRend, 1,"MEMORY FULL\nPress OK to clear"));
		}
	    } else {
		/* figure out how many lines are needed... */
		linesNeeded = tpd->topRow + tpd->cursorRow -
			(tpd->lastUsedRow - 1);
		linesCopied = 0;
		/* before we drop them off of the top, if we are
		 * using a history buffer, we need to transfer them 
		 * into the history buffer...
		 */
		if (tpd->useHistoryBuffer &&
			(tpd->scrollLockTopRow == 0) &&
			(tpd->scrollLockBottomRow >= (tw->term.rows - 1))) {
		    /* do we have enough lines in the history buffer, or
		     * do we need to steal some from the top?...
		     */
		    historyLinesNeeded = linesNeeded -
			    (tpd->historyBufferRows - tpd->lastUsedHistoryRow);
		    if ((historyLinesNeeded > 0) &&
			    (tpd->historyBufferRows > 0)) {
			/* take them from the top...  */
			/* we are effectively deleting the lines from the
			 * top of the combined buffers...
			 */
			(void) _DtTermPrimSelectDeleteLines(w, 0,
				historyLinesNeeded);
			/* move the lines from the top to the bottom... */
			(void) _DtTermPrimBufferInsertLineFromTB(
				tpd->historyBuffer, tpd->historyBufferRows - 1,
				historyLinesNeeded, insertFromTop);
			/* the lines are now freed, adjust the used count... */
			tpd->lastUsedHistoryRow -= historyLinesNeeded;
			if (tpd->lastUsedHistoryRow < 0) {
			    historyLinesNeeded += tpd->lastUsedHistoryRow;
			    tpd->lastUsedHistoryRow = 0;
			}
		    }

		    /* copy the lines over... */
		    for (i1 = 0; (i1 < linesNeeded) &&
			    (tpd->lastUsedHistoryRow < tpd->historyBufferRows);
			    i1++) {
			termChar *c1;
			short length;
			termChar *overflowChars;
			short overflowCount;

			/* get the line from the active buffer... */
			length = _DtTermPrimBufferGetLineLength(tBuffer,
				i1);

			/*
			** stuff it into the history buffer...
			** (but only if there is something to copy)
			*/
			if (length > 0) {
			    unsigned char eIndex;
			    short eCol;
			    short eCount;
			    enhValue *eValues = (enhValue *)NULL;

			    overflowChars = (termChar *) XtMalloc(BUFSIZ * sizeof (termChar));
			    c1 = _DtTermPrimBufferGetCharacterPointer(
				    tBuffer, i1, 0);

			    /* Perpetuate the enhancements. */
			    for (eCol = 0; eCol < length; eCol += eCount)
			    {
				if (_DtTermPrimBufferGetEnhancement(tBuffer,
					i1, eCol, &eValues,
					&eCount, countAll))
				{
				    if ((eValues == (enhValue *)NULL) ||
					(eCount <= 0))
					break;

				    for (eIndex = 0;
					 eIndex < NUM_ENH_FIELDS(tBuffer);
					 eIndex++)
				    {
					_DtTermPrimBufferSetEnhancement(
					    tpd->historyBuffer,
					    tpd->lastUsedHistoryRow,
					    eCol, eIndex, eValues[eIndex]);
				    }

				    (void) _DtTermPrimBufferInsert(
					    tpd->historyBuffer,
					    tpd->lastUsedHistoryRow,
					    eCol, c1, eCount,
					    False, &overflowChars,
					    &overflowCount);
				}
				else break;
			    }

			    if (eCol < length)
			    {
				/* Clear out enhancement values if necessary */
				if (eValues != (enhValue *)NULL)
				{
				    for (eIndex = 0;
					 eIndex < NUM_ENH_FIELDS(tBuffer);
					 eIndex++)
				    {
					_DtTermPrimBufferSetEnhancement(
					    tpd->historyBuffer,
					    tpd->lastUsedHistoryRow,
					    eCol, eIndex, 0);
				    }
				}
				(void) _DtTermPrimBufferInsert(
					tpd->historyBuffer,
					tpd->lastUsedHistoryRow,
					eCol, c1,
					length - eCol,
					False, &overflowChars,
					&overflowCount);
			    }

			    (void) XtFree((char *) overflowChars); 
			} else {
			    (void) _DtTermPrimBufferClearLine(
				    tpd->historyBuffer,
				    tpd->lastUsedHistoryRow, 0);
			}
			/* perpetuate the state of the wrap flag... */
			(void) _DtTermPrimBufferSetLineWrapFlag(
			    tpd->historyBuffer,
			    tpd->lastUsedHistoryRow,
			    _DtTermPrimBufferTestLineWrapFlag(tBuffer, i1));
			/* perpetuate the state of the inSelection flag...
			 */

			selectionFlag =
				_DtTermPrimBufferGetInSelectionFlag(
					tBuffer, i1);
			(void) _DtTermPrimBufferSetInSelectionFlag(
				tpd->historyBuffer,
				tpd->lastUsedHistoryRow,
				selectionFlag);
		
			/* clear the inselection flag before we scroll
			 * the lines off or we will end up releasing the
			 * selection...
			 */
			(void) _DtTermPrimBufferSetInSelectionFlag(
			    tBuffer, i1, (TermLineSelection) 0);
			(void) tpd->lastUsedHistoryRow++;
			(void) linesCopied++;
		    }
		}
		    
		/* take them from the top.  If we are about to take lines
		 * from the top without first copying them into the
		 * history buffer, we need to adjust the selection
		 * position...
		 */
		if (linesNeeded > linesCopied) {
		    (void) _DtTermPrimSelectDeleteLines(w,
			    tpd->lastUsedHistoryRow + linesCopied,
			    linesNeeded - linesCopied);
		}
		(void) _DtTermPrimBufferInsertLineFromTB(tBuffer,
			tpd->bufferRows - 1,
			linesNeeded, insertFromTop);
		/* adjust everything... */
		tpd->topRow -= linesNeeded;
	    }
	}
    }
}

static short
DoInsert(Widget w, unsigned char *buffer, int length, Boolean *wrapped)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;    
    struct termData *tpd = tw->term.tpd;
    TermBuffer tBuffer = tpd->termBuffer;
    short newWidth;

    termChar *returnChars;
    short returnCount;

    /* before we insert any text, we need to insure that the cursor is on
     * a valid buffer row...
     */
    if (tpd->topRow + tpd->cursorRow >= tpd->lastUsedRow) {
	(void) _DtTermPrimFillScreenGap(w);
    }

    /* insert the text... */
    returnChars = (termChar *) XtMalloc(BUFSIZ * sizeof (termChar));
    newWidth = _DtTermPrimBufferInsert(tBuffer,	/* TermBuffer		*/
	    tpd->topRow + tpd->cursorRow,		/* row			*/
	    tpd->cursorColumn,			/* column		*/
	    (termChar *) buffer,		/* newChars		*/
	    length,				/* numChars		*/
	    tpd->insertCharMode != DtTERM_INSERT_CHAR_OFF,
						/* insert flag		*/
	    &returnChars,			/* return char ptr	*/
	    &returnCount);			/* return count ptr	*/

    if ((tpd->insertCharMode != DtTERM_INSERT_CHAR_ON_WRAP) || (returnCount <= 0)) {
        (void) XtFree((char *) returnChars);
	return(newWidth);
    }

    /* we are in insert char mode with wrap and we wrapped text off of
     * the line...
     */
    *wrapped = True;

    /* wrap the inserted characters into the following line... */
    if (tpd->topRow + tpd->cursorRow + 1 >= tpd->lastUsedRow) {
	/* fake cursorRow... */
	(void) tpd->cursorRow++;
	(void) _DtTermPrimFillScreenGap(w);
	(void) tpd->cursorRow--;
    }

    /* we will allocate a temporary buffer so we don't have to worry
     * about _DtTermPrimBufferInsert tromping over its overflow buffer...
     */
    length = returnCount;
    buffer = (unsigned char *) XtMalloc(length);
    (void) memcpy(buffer, returnChars, length);

    /* insert the text into the next line... */
    newWidth = _DtTermPrimBufferInsert(tBuffer,	/* TermBuffer		*/
	    tpd->topRow + tpd->cursorRow + 1,
					/* row			*/
	    0,				/* column		*/
	    (termChar *) buffer,	/* newChars		*/
	    length,			/* numChars		*/
	    tpd->insertCharMode != DtTERM_INSERT_CHAR_OFF,
					/* insert flag		*/
	    &returnChars,		/* return char ptr	*/
	    &returnCount);		/* return count ptr	*/

    (void) XtFree((char *) buffer);
    (void) XtFree((char *) returnChars);
    return(newWidth);
}

int
_DtTermPrimInsertText(Widget w, unsigned char *buffer, int length)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;    
    struct termData *tpd = tw->term.tpd;
    TermBuffer tBuffer = tpd->termBuffer;
    int i;
    short renderStartX;
    short renderEndX;
    short insertStartX;
    short insertCharCount;
    short newWidth;
    Boolean needToRender = False;
    Boolean wrapped = False;


    if (tpd->mbCurMax > 1)
    {
        short    wcBufferLen;
        wchar_t *wcBuffer;
        wchar_t *pwc;
        int      i;
        int      mbLen;
        char    *pmb;        
#ifdef    NOCODE
        /* 
        ** It would be nice if the calling function supplied us with a count
        ** of the number of mb characters in the buffer, then we wouldn't
        ** have to count them again.
        */
        /* 
        ** we could use this if the multi-byte buffer was null terminated
        */
        wcBufferLen = mbstowcs((wchar_t *)NULL, (char *)buffer, length);
#else  /* NOCODE */
        i           = 0;
        pmb         = (char *)buffer;
        /* 
        ** we should never need more than length * sizeof(wchar_t)
        ** bytes to store the wide char equivalent of the incoming mb string
        */
        wcBuffer    = (wchar_t *)XtMalloc(length * sizeof(wchar_t));                                                
        pwc         = wcBuffer;
        wcBufferLen = 0;
        while (i < length)
        {
            switch (mbLen = mbtowc(pwc, pmb, MIN(((int)MB_CUR_MAX), length - i)))
            {
              case -1:
                if ((int)MB_CUR_MAX <= length - i) {
                    /* we have a bogus multi-byte character.  Throw away
                     * the first byte and rescan (TM 12/14/93)...
                     */
                    /* 
                    ** in this case, we move the remaining length - i - 1 
                    ** bytes one byte to the left (to overwrite the bogus
                    ** byte)
                    */
                    memmove(pmb, pmb + 1, length - i - 1);
                    length--;
                    continue;
                }
                break;
              case  0:
                /* 
                ** treat null character same as any other character...
                */
                mbLen = 1;
              default:
                i   += mbLen;
                pmb += mbLen;
                pwc++;
                wcBufferLen++;
            }
        }
#endif /* NOCODE */
        i = _DtTermPrimInsertTextWc(w, wcBuffer, wcBufferLen);
	/* convert back from a wide character count to a multibyte
	 * character count...
	 */
        pmb         = (char *)buffer;
        pwc         = wcBuffer;
        wcBufferLen = i;
	i           = 0;
        while (i < wcBufferLen)
        {
            switch (mbLen = mblen(pmb, MIN(((int)MB_CUR_MAX), length - i)))
            {
	      case -1:
              case  0:
                /* 
                ** treat null character same as any other character...
                */
                mbLen = 1;
              default:
                i   ++;
                pmb += mbLen;
            }
        }
        XtFree((char *)wcBuffer);
        return(pmb - (char *) buffer);
    }

    /* turn off the cursor... */
    if (CURSORoff != tpd->cursorState) {
	(void) _DtTermPrimCursorOff(w);
    }

    /* we support two different types of autowrap.  The HP style one is where
     * you display the character and then wrap if you are at the end of the
     * line.  The ANSI style one is where you insert the character at the end
     * of the line and don't autowrap until you try to insert another
     * character...
     */

    renderStartX = tpd->cursorColumn;
    renderEndX = tpd->cursorColumn;
    insertStartX = tpd->cursorColumn;
    insertCharCount = 0;

    for (i = 0; (i < length) && tpd->ptyInputId; i++) {
	/* the following code performs two functions.  If we are in
	 * autowrap, it performs a sanity check on the insert position.
	 * if we are not in autowrap, it will insure that characters
	 * inserted after the last position will replace the last
	 * character...
	 */
	if ((tpd->cursorColumn >= tw->term.columns) &&
		!(tpd->autoWrapRight && !tpd->wrapRightAfterInsert)) {
	    /* blow away the previous character...
	     */
	    tpd->cursorColumn = tw->term.columns - 1;
	    renderStartX = MIN(renderStartX, tpd->cursorColumn);
	    tpd->wrapState = WRAPpastRightMargin;
	}

	/* for emulations that wrap after inserting the character, we
	 * will insert the character and then check for wrap...
	 */
	if (tpd->wrapRightAfterInsert) {
	    if (insertCharCount == 0) {
		insertStartX = i;
	    }
	    (void) insertCharCount++;
	}

	if (((tpd->cursorColumn + insertCharCount) >= tw->term.columns) ||
		((tpd->cursorColumn + insertCharCount) ==
		tpd->rightMargin + 1)) {
	    if (tpd->autoWrapRight) {
		/* perform an auto wrap...
		 */
		/* we need to insert any pending characters, and
		 * render them...
		 */
		if (insertCharCount) {
		    newWidth = DoInsert(w, &buffer[insertStartX],
			    insertCharCount, &wrapped);
		    tpd->cursorColumn += insertCharCount;
		    insertCharCount = 0;
		    if (tpd->insertCharMode != DtTERM_INSERT_CHAR_OFF) {
			renderEndX = newWidth;
		    } else {
			renderEndX = MAX(renderEndX, tpd->cursorColumn);
		    }
		    needToRender = True;
		}
		if (needToRender) {
		    DebugF('t', 0, fprintf(stderr,
			    ">>termInsertText() calling[2] _DtTermPrimRefreshText()\n"));
		    (void) _DtTermPrimRefreshText(w, renderStartX, tpd->cursorRow,
			    wrapped ? tw->term.columns : MAX(renderEndX, 0),
			    tpd->cursorRow);
		    if (wrapped && (tpd->cursorRow + 1 < tw->term.rows)) {
			(void) _DtTermPrimRefreshText(w, 0, tpd->cursorRow + 1,
				renderEndX, tpd->cursorRow + 1);
		    }
		    wrapped = False;
		    needToRender = False;
		}
		tpd->cursorColumn = tpd->leftMargin;
		tpd->wrapState = WRAPbetweenMargins;
		renderEndX = 0;

		_DtTermPrimBufferSetLineWrapFlag(tBuffer,
						 tpd->topRow + tpd->cursorRow,
						 True);

		if (tpd->cursorRow == tpd->scrollLockBottomRow) {
		    /* scroll at the bottom of the lock area... */
		    (void) _DtTermPrimScrollText(w, 1);
		    (void) _DtTermPrimFillScreenGap(w);
		} else if (++tpd->cursorRow >= tw->term.rows) {
		    /* we are at the bottom row of the locked region.
		     * Wrap to the beginning of this line...
		     */
		    tpd->cursorRow = tw->term.rows - 1;
		} else {
		    /* we are not at the bottom row.  We already have
		     * wrapped down one line...
		     */
		}
		    
		renderStartX = tpd->cursorColumn;

		if (tpd->scroll.nojump.pendingScroll) {
		    /* If we wrapped the screen, bail out now and we
		     * will take care of this character when we
		     * finish the scroll.  If we are in wrap after,
		     * then we need to skip past this character so
		     * that it doesn't get processed twice...
		     */
		    if (tpd->wrapRightAfterInsert)
			(void) i++;
		    break;
		}
	    } else {
		/* overwrite the last character(s) on the line... */
		if (insertCharCount > 0) {
		    newWidth = DoInsert(w, &buffer[insertStartX],
			    insertCharCount, &wrapped);
		    tpd->cursorColumn += insertCharCount;
		    insertCharCount = 0;
		    if (tpd->insertCharMode != DtTERM_INSERT_CHAR_OFF) {
			renderEndX = newWidth;
		    } else {
			renderEndX = MAX(renderEndX, tpd->cursorColumn);
		    }
		    needToRender = True;
		}

		if (tpd->cursorColumn == tpd->rightMargin + 1)
		    tpd->cursorColumn = tpd->rightMargin;
		else 
		    tpd->cursorColumn = tw->term.columns - 1;
	    }
	}

	/* for emulations that wrap before inserting the character, we
	 * will insert the character and then check for wrap...
	 */
	if (!tpd->wrapRightAfterInsert) {
	    /* before we insert any text, we need to insure that the
	     * cursor is on a valid buffer row...
	     */
	    if (insertCharCount == 0) {
		insertStartX = i;
	    }
	    (void) insertCharCount++;
	}
    }
    /* insert and render any remaining text... */
    if (insertCharCount > 0) {
	newWidth = DoInsert(w, &buffer[insertStartX],
		insertCharCount, &wrapped);
	tpd->cursorColumn += insertCharCount;
	insertCharCount = 0;
	if (tpd->insertCharMode != DtTERM_INSERT_CHAR_OFF) {
	    renderEndX = newWidth;
	} else {
	    renderEndX = MAX(renderEndX, tpd->cursorColumn);
	}
	needToRender = True;
    }
    if (needToRender) {
	renderEndX = MAX(renderEndX, tpd->cursorColumn);
	DebugF('t', 0, fprintf(stderr,
		">>termInsertText() calling _DtTermPrimRefreshText()\n"));
	(void) _DtTermPrimRefreshText(w, renderStartX, tpd->cursorRow,
		wrapped ? tw->term.columns : MAX(renderEndX, 0),
		tpd->cursorRow);
	if (wrapped && (tpd->cursorRow + 1 < tw->term.rows)) {
	    (void) _DtTermPrimRefreshText(w, 0, tpd->cursorRow + 1,
		    renderEndX, tpd->cursorRow + 1);
	}
	wrapped = False;
	needToRender = False;
    }

    return(i);
}

static void
buildDangleBuffer
(
    unsigned char	 *buffer,
    int			  bufferLen,
    unsigned char	 *mbPartialChar,
    int			 *mbPartialCharLen,
    int			  writeLen,
    unsigned char	**dangleBuffer,
    int			 *dangleBufferLen
)
{
    /* malloc our dangle buffer... */
    *dangleBufferLen = bufferLen + *mbPartialCharLen - writeLen;
    *dangleBuffer = (unsigned char *) XtMalloc(*dangleBufferLen);

    /* copy over the unwritten part of the orignal buffer... */
    (void) memmove(*dangleBuffer, buffer + writeLen, bufferLen - writeLen);
    if (*mbPartialCharLen) {
	(void) memmove(*dangleBuffer + bufferLen - writeLen, mbPartialChar,
		*mbPartialCharLen);
	*mbPartialCharLen = 0;
    }
    return;
}

Boolean
_DtTermPrimParseInput
(
    Widget		  w,
    unsigned char	 *buffer,
    int			  len,
    unsigned char	**dangleBuffer,
    int			 *dangleBufferLen
)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;    
    struct termData *tpd = tw->term.tpd;
    DtTermPrimitiveClassPart	 *termClassPart = &(((DtTermPrimitiveClassRec *)
	    (tw->core.widget_class))->term_primitive_class);
    TermBuffer tBuffer = tpd->termBuffer;
    int i;
    short insertStart;
    short insertByteCount;
    short returnLen;
    Boolean turnCursorOn = False;
    unsigned char *tmpBuffer = (unsigned char *) 0;
    unsigned char mbChar[MB_LEN_MAX];
    int mbCharLen = 1;
    static Boolean *preParseTable = (Boolean *) 0;

    *dangleBuffer = (unsigned char *) 0;
    insertStart = 0;
    insertByteCount = 0;

    /* initialize the preParseTable if necessary... */
    _DtTermProcessLock();
    if (!preParseTable) {
	preParseTable = (Boolean *) XtMalloc(256 * sizeof(Boolean));
	(void) memset(preParseTable, '\0', 256 * sizeof(Boolean));

	for (i = 0x00; i <= 0x1f; i++) {
	    preParseTable[i] = True;
	}
	for (i = 0x80; i <= 0x9f; i++) {
	    preParseTable[i] = True;
	}
    }
    _DtTermProcessUnlock();

    /* check for partial multibyte character... */
    if (tpd->mbPartialCharLen > 0) {
	tmpBuffer = (unsigned char *) XtMalloc(len + tpd->mbPartialCharLen);
	(void) memcpy(tmpBuffer, tpd->mbPartialChar, tpd->mbPartialCharLen);
	(void) memcpy(tmpBuffer + tpd->mbPartialCharLen, buffer, len);
	buffer = tmpBuffer;
	len += tpd->mbPartialCharLen;
	tpd->mbPartialCharLen = 0;
    }

    /* turn off the cursor... */
    if (CURSORoff != tpd->cursorState) {
	(void) _DtTermPrimCursorOff(w);
	turnCursorOn = True;
    }


    for (i = 0; (i < len) && tpd->ptyInputId; ) {
	if (tpd->mbCurMax > 1) {
            switch (mbCharLen = 
                    mblen((char *) &buffer[i], MIN(((int)MB_CUR_MAX), len - i)))
            {
              case -1:
		if ((int)MB_CUR_MAX <= len - i)
                {
		    /* we have a bogus multi-byte character.  Throw away
		     * the first byte and rescan (TM 12/14/93)...
		     */
		    /* dump what we know we want to insert... */
		    if (insertByteCount > 0) {
			returnLen = (*(termClassPart->term_insert_proc))(w,
				&buffer[insertStart], insertByteCount);
			if (returnLen != insertByteCount) {
			    (void) buildDangleBuffer(buffer, len,
					tpd->mbPartialChar,
					&tpd->mbPartialCharLen,
					insertStart + returnLen,
					dangleBuffer, dangleBufferLen);

			    insertByteCount = 0;
			    break;
			}
			insertByteCount = 0;
		    }
		    /* skip over the bogus char's first byte... */
		    (void) i++;
		    insertStart = i;
		    continue;
		} else {
		    /* we have a dangling partial multi-byte character... */
		    (void) memmove(tpd->mbPartialChar, &buffer[i], len - i);
		    tpd->mbPartialCharLen = len - i;
		    /* remove the partial char from the buffer and adjust
		     * the buffer len...
		     */
		    len = i;
		    continue;
		}
                break;
              case 0:
                mbCharLen = 1;
                /* fall through */
              default:
                break;
            }
	}

	if (((mbCharLen == 1) && preParseTable[buffer[i]]) ||
		tpd->parserNotInStartState) {
            /* 
	    ** It's either a control code or we are not in the start state.
	    ** If we have any text to insert, insert
	    ** the text, display any added text, then send it down through
	    ** the parser.
            */
	    if (insertByteCount > 0) {
		returnLen = (*(termClassPart->term_insert_proc))(w,
			&buffer[insertStart], insertByteCount);
		if (returnLen != insertByteCount) {
		    (void) buildDangleBuffer(buffer, len,
				tpd->mbPartialChar,
				&tpd->mbPartialCharLen,
				insertStart + returnLen,
				dangleBuffer, dangleBufferLen);
		    insertByteCount = 0;
		    break;
		}
		insertByteCount = 0;
	    }
            
	    tpd->parserNotInStartState = _DtTermPrimParse(w, &buffer[i],
		    mbCharLen);
	    i += mbCharLen;
	    insertStart = i;
        } else {
	    /* queue up text to insert into the buffer... */
	    insertByteCount += mbCharLen;
	    i += mbCharLen;
	}
    }

    if (insertByteCount > 0)
    {
	returnLen = (*(termClassPart->term_insert_proc))(w,
							 &buffer[insertStart],
							 insertByteCount);
	if (returnLen != insertByteCount)
	{
	    (void) buildDangleBuffer(buffer, len,
				     tpd->mbPartialChar,
				     &tpd->mbPartialCharLen,
				     insertStart + returnLen,
				     dangleBuffer, dangleBufferLen);
	}
    }
    else
    {
	/*
	** insertByteCount <= 0, check to make sure we haven't
	** already saved any remaining text in the dangleBuffer...
	*/
	if (i < len && !*dangleBuffer)
	{
	    (void) buildDangleBuffer(buffer, len,
				     tpd->mbPartialChar,
				     &tpd->mbPartialCharLen,
				     i, dangleBuffer, dangleBufferLen);
	}
    }

    /* if we turned the cursor off, turn the cursor back on... */
    if (turnCursorOn) {
	(void) _DtTermPrimCursorOn(w);
    }

    if (tmpBuffer) {
	(void) XtFree((char *) tmpBuffer);
    }
    if (*dangleBuffer) {
	return(False);
    }
    return(True);
}

/* 
** Pad the current line from the current end of line up to (and
** including) the current cursor column.
*/
void
_DtTermPrimRenderPadLine
(
    Widget          w
)
{
    DtTermPrimitiveWidget          tw = (DtTermPrimitiveWidget) w;    
    struct termData    *tpd = tw->term.tpd;
    TermBuffer          tBuffer = tpd->termBuffer;
    short               currentWidth;

    /* before we pad this line, we need to insure that cursor is on a
     * valid buffer row...
     */
    (void) _DtTermPrimFillScreenGap(w);

    currentWidth = _DtTermPrimBufferGetLineWidth(tBuffer,
	    tpd->topRow + tpd->cursorRow);
    if (tpd->cursorColumn >= currentWidth)
    {
        /* 
        ** Pad the line and refresh it.
        */
	if (tpd->mbCurMax > 1) {
	    _DtTermPrimBufferPadLineWc(tBuffer, tpd->topRow + tpd->cursorRow,
				     tpd->cursorColumn + 1);
	} else {
	    _DtTermPrimBufferPadLine(tBuffer, tpd->topRow + tpd->cursorRow,
				     tpd->cursorColumn + 1);
	}
        _DtTermPrimRefreshText(w, currentWidth, tpd->cursorRow, tpd->cursorColumn,
		   tpd->cursorRow);
    }
}

void
_DtTermPrimDestroyFont(
    Widget		  w,
    TermFont		  font
)
{
    if (font) (void) (*(font->destroyFunction))(w, font);
    return;
}

void
_DtTermPrimRenderText(
    Widget		  w,
    TermFont		  font,
    Pixel		  fg,
    Pixel		  bg,
    unsigned long	  flags,
    int			  x,
    int			  y,
    unsigned char	 *string,
    int			  len
)
{
    (void) (*(font->renderFunction))(w, font, fg, bg, flags, x, y, string, len);
    return;
}
