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
static char rcs_id[] = "$XConsortium: TermPrimRenderMb.c /main/1 1996/04/21 19:19:05 drk $";
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

/* 
** This file contains the multi-byte character versions of the render routines.
*/

#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermPrimP.h"
#include "TermPrimData.h"
#include "TermPrimLineDraw.h"
#include "TermPrimOSDepI.h"
#include "TermPrimBufferP.h"
#include "TermPrimRenderP.h"
#include "TermPrimSelect.h"
#include "TermPrimSelectP.h"
#include <limits.h>
#include <wchar.h>

void
_DtTermPrimRefreshTextWc(Widget w, short startColumn, short startRow,
	short endColumn, short endRow)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    TermBuffer tBuffer = tpd->termBuffer;
    short lineWidth;
    short chunkLineWidth;
    wchar_t *linePtr;
    wchar_t *wc;
    TermFont termFont;
    int currentColorPair = 0;
    int currentVideo = 0;
    short chunkStartColumn;
    short chunkWidth;
    short chunkLength;
    short thisStartColumn;
    short thisEndColumn;
    enhValues enhancements;
    int i1;
    int lineNum;
    unsigned long valueMask;
    GC gc;
    XGCValues values;
    TermEnhInfoRec enhInfo;
    Boolean checkSelection = False;
    Boolean inSelection;
    int selectionEnd;
    Pixel tmpPixel;
    XmTextPosition  begin, end;
    TermCharInfoRec startCharInfo;
    
    DebugF('t', 0, fprintf(stderr, ">>_DtTermPrimRefreshTextWc() starting\n"));
    DebugF('t', 0, fprintf(stderr,
	    ">>_DtTermPrimRefreshTextWc() startCol=%hd  startRow=%hd  endCol=%hd  endRow=%hd\n",
	    startColumn, startRow, endColumn, endRow));

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
		">>_DtTermPrimRefreshTextWc() jump scroll in progress, no render\n"));
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
	values.foreground = tw->primitive.foreground;
	values.background = tw->core.background_pixel;

	tpd->renderGC.gc = XCreateGC(XtDisplay(w), XtWindow(w),
		GCForeground | GCBackground, &values);

	/* set the GC cache values... */
	tpd->renderGC.foreground = values.foreground;
	tpd->renderGC.background = values.background;
	tpd->renderGC.fid = (Font) 0;

	/***********************************************************
	 * renderReverseGC...
	 */
	values.foreground = tw->core.background_pixel;
	values.background = tw->primitive.foreground;

	tpd->renderReverseGC.gc = XCreateGC(XtDisplay(w), XtWindow(w),
		GCForeground | GCBackground, &values);

	/* set the GC cache values... */
	tpd->renderReverseGC.foreground = values.foreground;
	tpd->renderReverseGC.background = values.background;
	tpd->renderReverseGC.fid = (Font) 0;

	/***********************************************************
	 * clearGC...
	 */
	values.foreground = tw->core.background_pixel;
	values.background = tw->primitive.foreground;
	tpd->clearGC.gc = XCreateGC(XtDisplay(w), XtWindow(w),
		GCForeground | GCBackground, &values);

	/* set the GC cache values... */
	tpd->clearGC.foreground = values.foreground;
	tpd->clearGC.background = values.background;
	tpd->clearGC.fid = (Font) 0;
    }

    /* clip start/end x/y... */
    if (endColumn >= tw->term.columns)
	endColumn = tw->term.columns - 1;
    if (endRow >= tw->term.rows)
	endRow = tw->term.rows - 1;

    for (; startRow <= endRow; startRow++) {
	/* if we are refreshing a full line, then we can clear the
	 * scrollRefreshRows flag for this line...
	 */
	if ((startColumn == 0) && (endColumn >= tw->term.columns - 1)) {
	    tpd->scrollRefreshRows[startRow] = False;
	}

	lineNum = startRow + tpd->topRow;

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

	if (startColumn > endColumn) {
	    /* nothing to render on this line... */
	    continue;
	}

	if (lineNum >= tpd->lastUsedRow) {
	    /* we are pointing to empty screen space below the last used
	     * line of the display...
	     */
	    lineWidth       = 0;
	    linePtr         = (wchar_t) 0;
	    thisStartColumn = startColumn;
	    thisEndColumn   = endColumn;
	} else if (lineNum < 0) {
	    if ((tpd->useHistoryBuffer) &&
		    (-lineNum <= tpd->lastUsedHistoryRow)) {
		/* get a line out of the history buffer... */
		lineWidth = _DtTermPrimBufferGetLineWidth(tpd->historyBuffer,
			tpd->lastUsedHistoryRow + lineNum);
		if ((startColumn < lineWidth) &&
			_DtTermPrimGetCharacterInfo(tpd->historyBuffer,
			tpd->lastUsedHistoryRow + lineNum,
			startColumn, &startCharInfo)) {
		    thisStartColumn = startCharInfo.startCol;
		} else {
		    /* past the last character in this line... */
		    thisStartColumn = startColumn;
		}

		/* get the line width and a pointer to the data... */
		thisEndColumn = endColumn;
		lineWidth = MAX(0, MIN(thisEndColumn - thisStartColumn + 1,
			lineWidth - startColumn));
		linePtr = startCharInfo.u.pwc;
	    } else {
		/* we are above the history buffer.  Should not happen, but...
		 */
		lineWidth = 0;
		linePtr = (wchar_t) 0;
	    }
	} else {
	    /* adjust startColumn to point to the first column for the
	     * first character...
	     */
	    lineWidth = _DtTermPrimBufferGetLineWidth(tBuffer, lineNum); 
	    if ((startColumn < lineWidth) &&
		    _DtTermPrimGetCharacterInfo(tBuffer, lineNum, startColumn,
		    &startCharInfo)) {
		thisStartColumn = startCharInfo.startCol;
	    } else {
		/* past the last character in this line... */
		thisStartColumn = startColumn;
	    }

	    /* get the line width and a pointer to the data... */
	    thisEndColumn = endColumn;
	    lineWidth = MAX(0, MIN(thisEndColumn - thisStartColumn + 1,
		    lineWidth - thisStartColumn));
	    linePtr = startCharInfo.u.pwc;
	}

	chunkStartColumn = thisStartColumn;
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
			&chunkWidth,		/* width		*/
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

	    /* Are we in the selection area?
	     * _DtTermPrimSelectIsInSelection clips the chunkWidth to
	     * the end of the selection if we are...
	     */
	    if (checkSelection && 
                _DtTermPrimSelectIsInSelection(w, lineNum, chunkStartColumn,
		                         chunkWidth, &chunkWidth)) {
		inSelection = True;
	    } else {
		inSelection = False;
	    }

	    /* walk through the linePtr and figure out how many
	     * wide characters we need to render to fill out this
	     * chunk, and if the end of this chunk is in the middle
	     * of a wide character, bump the chunkWidth one column.
	     */
	    for (chunkLineWidth = 0, wc = linePtr;
		    chunkLineWidth < chunkWidth;
		    chunkLineWidth += i1, wc++) {
		if ((i1 = wcwidth(*wc)) <= 0) {
		    /* take care of null characters... */
		    i1 = 1;
		}
	    }
	    /* adjust for a multi-column character that spans the line
	     * boundary...
	     */
	    if (chunkLineWidth > chunkWidth) {
		chunkWidth = chunkLineWidth;
	    }
	    /* set reasonable defaults for our render info... */
	    enhInfo.fg = tw->primitive.foreground;
	    enhInfo.bg = tw->core.background_pixel;
	    enhInfo.font = tpd->defaultTermFont;
	    enhInfo.flags = (unsigned long) 0;

	    /* set our font and color from the enhancements... */
	    if (ENH_PROC(tBuffer)) {
                (void) (*(ENH_PROC(tBuffer)))(w, enhancements, &enhInfo);
	    }

	    /* are we in the selection area?... */
	    if (inSelection) {
		/* flip fg and bg... */
		tmpPixel = enhInfo.fg;
		enhInfo.fg = enhInfo.bg;
		enhInfo.bg = tmpPixel;
	    }
			
	    /* if secure, we will use a XFillRectangle, and we need
	     * foreground set to the background...
	     */

	    /* set the renderGC... */
	    valueMask = (unsigned long) 0;
	    if (TermIS_SECURE(enhInfo.flags)) {
		/* render secure video locally...
		 */
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
		    /*DKS: this probably should be changed... */
		    (termChar *) linePtr,	/* string		*/
		    wc - linePtr);		/* width		*/
	    }
	    chunkStartColumn += chunkWidth;
	    lineWidth -= chunkWidth;
	    linePtr = wc;
	}

	/* clear any extra space in the line.  chunkStartColumn now points to
	 * the end of the line, and lineWidth == 0...
	 */
	while (thisEndColumn - chunkStartColumn >= 0) {
	    chunkWidth = thisEndColumn - chunkStartColumn + 1;
	    if (checkSelection && 
                _DtTermPrimSelectIsInSelection(w, lineNum, chunkStartColumn,
		                         chunkWidth, &chunkWidth)) {
		/* use the render gc set to the fg color... */
		gc = tpd->renderReverseGC.gc;

		valueMask = (unsigned long) 0;
		if (tpd->renderReverseGC.foreground !=
			tw->primitive.foreground) {
		    tpd->renderReverseGC.foreground = tw->primitive.foreground;
		    values.foreground = tw->primitive.foreground;
		    valueMask |= GCForeground;
		}
		if (valueMask) {
		    (void) XChangeGC(XtDisplay(w), tpd->renderReverseGC.gc,
			    valueMask, &values);
		}
	    } else {
		/* use the clear GC... */
		gc = tpd->clearGC.gc;
	    }

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
	    chunkStartColumn += chunkWidth;
	}
    }
    DebugF('t', 0, fprintf(stderr, ">>_DtTermPrimRefreshTextWc() finished\n"));
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
_DtTermPrimExposeTextMb(Widget w, int x, int y, int width, int height,
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
	    (void) _DtTermPrimRefreshTextWc(w,
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
    DebugF('t', 0, fprintf(stderr, ">>exposeText() calling _DtTermPrimRefreshTextWc()\n"));
    (void) _DtTermPrimRefreshTextWc(w, (x - tpd->offsetX) / tpd->cellWidth,
	    (y - tpd->offsetY)  / tpd->cellHeight,
	    (x + width - 1 - tpd->offsetX) / tpd->cellWidth,
	    (y + height - 1 - tpd->offsetY) / tpd->cellHeight);

    DebugF('e', 0, fprintf(stderr, ">>exposeText() finished\n"));
}

static short
DoInsertWc(Widget w, wchar_t *wcBuffer, int wcBufferLen, Boolean *wrapped)
{
    DtTermPrimitiveWidget   tw = (DtTermPrimitiveWidget) w;    
    struct termData        *tpd = tw->term.tpd;
    TermBuffer              tBuffer = tpd->termBuffer;
    short                   newWidth;
    wchar_t                *returnChars;
    short                   returnCount;
    
    /*
    ** before we insert any text, we need to insure that the cursor is on
    ** a valid buffer row...
    */
    if (tpd->topRow + tpd->cursorRow >= tpd->lastUsedRow)
    {
	(void) _DtTermPrimFillScreenGap(w);
    }

    /* insert the text... */
    returnChars = (wchar_t *) XtMalloc(BUFSIZ * sizeof (wchar_t));
    newWidth = _DtTermPrimBufferInsertWc(tBuffer,       /* TermBuffer	    */
	    tpd->topRow + tpd->cursorRow,	        /* row		    */
	    tpd->cursorColumn,			        /* column	    */
	    wcBuffer,	                	        /* newChars	    */
	    wcBufferLen,			        /* numChars         */
	    tpd->insertCharMode != DtTERM_INSERT_CHAR_OFF,  /* insert flag	    */
	    &returnChars,			        /* return char ptr  */
	    &returnCount);			        /* return count ptr */

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

    /*
    ** Copy the returned characters to a temporary buffer so we don't have to
    ** worry about _DtTermPrimBufferInsertWc tromping over its overflow buffer...
    */
    wcBufferLen = returnCount;
    wcBuffer    = (wchar_t *)XtMalloc(wcBufferLen);
    (void) memcpy(wcBuffer, returnChars, wcBufferLen * sizeof(wchar_t));

    /*
    ** insert the text into the next line...
    */
    newWidth = _DtTermPrimBufferInsertWc(tBuffer,       /* TermBuffer       */
	    tpd->topRow + tpd->cursorRow + 1,           /* row              */
	    0,				                /* column	    */
	    wcBuffer,                 	                /* newChars	    */
	    wcBufferLen,		                /* numChars	    */
	    tpd->insertCharMode != DtTERM_INSERT_CHAR_OFF,  /* insert flag	    */
	    &returnChars,		                /* return char ptr  */
	    &returnCount);		                /* return count ptr */
    (void) XtFree((char *) wcBuffer);
    (void) XtFree((char *) returnChars);
    return(newWidth);
}

/* 
** Insert the supplied text into the TermBuffer, and return a count of the
** number of characters actually inserted.
*/
int
_DtTermPrimInsertTextWc
(
    Widget      w, 
    wchar_t    *wcBuffer,   /* buffer of wide chars                    */
    int         wcBufLen    /* # of wide chars in the buffer           */
)
{
    DtTermPrimitiveWidget   tw      = (DtTermPrimitiveWidget) w;    
    struct termData        *tpd     = tw->term.tpd;
    TermBuffer              tBuffer = tpd->termBuffer;

    int         i;
    short       renderStartX;
    short       renderEndX;
    short       insertStartX;
    short       insertCharCount;
    short       insertCharWidth; /* column width of characters to insert */
    short	thisCharWidth;
    short       newWidth;
    Boolean     needToRender = False;
    Boolean     wrapped = False;

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

    renderStartX    = tpd->cursorColumn;
    renderEndX      = tpd->cursorColumn;
    insertStartX    = tpd->cursorColumn;
    insertCharCount = 0;
    insertCharWidth = 0;

    for (i = 0; (i < wcBufLen) && tpd->ptyInputId; i++) {
	thisCharWidth = MAX(1, wcwidth(wcBuffer[i]));

	/* the following code performs two functions.  If we are in
	 * autowrap, it performs a sanity check on the insert position.
	 * if we are not in autowrap, it will insure that characters
	 * inserted after the last position will replace the last
	 * character...
	 */
	if (((tpd->cursorColumn + thisCharWidth) > tw->term.columns) &&
		!(tpd->autoWrapRight && !tpd->wrapRightAfterInsert)) {
	    /* blow away the previous character...
	     */
	    tpd->cursorColumn = tw->term.columns - thisCharWidth;
	    renderStartX = MIN(renderStartX, tpd->cursorColumn);
	    tpd->wrapState = WRAPpastRightMargin;
	}

	/* for emulations that wrap after inserting the character, we
	 * will insert the character and then check for wrap...
	 */
	if (tpd->wrapRightAfterInsert) {
	    if (insertCharCount == 0) {
		insertStartX    = i;
                insertCharWidth = 0;
	    }
	    (void) insertCharCount++;
            insertCharWidth += thisCharWidth;
	}

	if (((tpd->cursorColumn + insertCharWidth +
		(tpd->wrapRightAfterInsert ? 0: thisCharWidth)) >
		tw->term.columns) ||
		((tpd->cursorColumn + insertCharWidth +
		(tpd->wrapRightAfterInsert ? 0 : (thisCharWidth - 1))) ==
		tpd->rightMargin + 1)) {
	    if (tpd->autoWrapRight) {
		/* perform an auto wrap...
		 */
		/* we need to insert any pending characters, and
		 * render them...
		 */
		if (insertCharCount) {
		    newWidth  = DoInsertWc(w, &wcBuffer[insertStartX],
			                   insertCharCount, &wrapped);
		    tpd->cursorColumn += insertCharWidth;
		    insertCharCount = 0;
		    if (tpd->insertCharMode == DtTERM_INSERT_CHAR_OFF) {
			renderEndX = MAX(renderEndX, tpd->cursorColumn);
		    } else {
			renderEndX = newWidth;
		    }
		    needToRender = True;
		}
		if (needToRender) {
		    DebugF('t', 0, fprintf(stderr,
			    ">>termInsertText() calling[2] _DtTermPrimRefreshTextWc()\n"));
		    (void) _DtTermPrimRefreshTextWc(w, renderStartX, tpd->cursorRow,
			    wrapped ? tw->term.columns : MAX(renderEndX, 0),
			    tpd->cursorRow);
		    if (wrapped && (tpd->cursorRow + 1 < tw->term.rows)) {
			(void) _DtTermPrimRefreshTextWc(w, 0, tpd->cursorRow + 1,
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
		    newWidth  = DoInsertWc(w, &wcBuffer[insertStartX],
			                   insertCharCount, &wrapped);
		    tpd->cursorColumn += insertCharWidth;
		    insertCharCount = 0;
		    if (tpd->insertCharMode == DtTERM_INSERT_CHAR_OFF) {
			renderEndX = MAX(renderEndX, tpd->cursorColumn);
		    } else {
			renderEndX = newWidth;
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
		insertStartX    = i;
                insertCharWidth = 0;
	    }
	    (void) insertCharCount++;
            insertCharWidth += thisCharWidth;
	}
    }
    /* insert and render any remaining text... */
    if (insertCharCount > 0) {
	/*
	** The following code performs two functions.  If we are in
	** autowrap, it performs a sanity check on the insert position.
	** if we are not in autowrap, it will insure that characters
	** inserted after the last position will replace the last
	** character...
	**
	** NOTE: 
	**    This is only required in the case that we are trying to
        **    overwrite the last character on the line with a two column
        **    character...
	*/
	if (insertCharCount == 1 &&
	    thisCharWidth   == 2 && 
	    ((tpd->cursorColumn + thisCharWidth) > tw->term.columns) &&
	    !(tpd->autoWrapRight && !tpd->wrapRightAfterInsert))
	{
	    /*
	    ** blow away the previous character...
	    */
	    tpd->cursorColumn = tw->term.columns - thisCharWidth;
	    renderStartX = MIN(renderStartX, tpd->cursorColumn);
	    tpd->wrapState = WRAPpastRightMargin;
	}
	newWidth  = DoInsertWc(w, &wcBuffer[insertStartX],
		               insertCharCount, &wrapped);
        tpd->cursorColumn += insertCharWidth;
	insertCharCount = 0;
	if (tpd->insertCharMode == DtTERM_INSERT_CHAR_OFF) {
	    renderEndX = MAX(renderEndX, tpd->cursorColumn);
	} else {
	    renderEndX = newWidth;
	}
	needToRender = True;
    }
    if (needToRender) {
	renderEndX = MAX(renderEndX, tpd->cursorColumn);
	DebugF('t', 0, fprintf(stderr,
		">>termInsertText() calling _DtTermPrimRefreshTextWc()\n"));
	(void) _DtTermPrimRefreshTextWc(w, renderStartX - 1, tpd->cursorRow,
		wrapped ? tw->term.columns : MAX(renderEndX + 1, 0),
		tpd->cursorRow);
	if (wrapped && (tpd->cursorRow + 1 < tw->term.rows)) {
	    (void) _DtTermPrimRefreshTextWc(w, 0, tpd->cursorRow + 1,
		    renderEndX + 1, tpd->cursorRow + 1);
	}
	wrapped = False;
	needToRender = False;
    }
    return(i);
}
