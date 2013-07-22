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
static char rcs_id[] = "$XConsortium: TermPrimScroll.c /main/1 1996/04/21 19:19:17 drk $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermPrimP.h"
#include "TermPrimI.h"
#include "TermPrimData.h"
#include "TermPrimBuffer.h"

static void
waitOnCopyArea(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    XEvent event;
    XEvent *ev = &event;

    while (tpd->scrollInProgress) {
	(void) XWindowEvent(XtDisplay(w), XtWindow(w), ExposureMask, ev);
	switch (ev->type) {
	case Expose:
	    Debug('e', fprintf(stderr,
		    ">>_waitOnCopyArea() Expose event received\n"));

	    /* refresh the border... */
	    (void) _DtTermPrimDrawShadow(w);

	    /* just refresh the exposed area...
	     */
	    Debug('e', fprintf(stderr,
		    ">>_waitOnCopyArea() exposing Expose area\n"));
	    Debug('e', fprintf(stderr,
		    ">>                  x=%d  y=%d  width=%d  height=%d\n",
		    ev->xexpose.x, ev->xexpose.y,
		    ev->xexpose.width, ev->xexpose.height));
	    (void) _DtTermPrimExposeText(w, ev->xexpose.x, ev->xexpose.y,
		    ev->xexpose.width, ev->xexpose.height, True);
	    break;

	case GraphicsExpose:
	    /* refresh the exposed area and, if this is the last graphics
	     * exposure, we are done and can fall through to the noExpose
	     * code...
	     */
	    Debug('e', fprintf(stderr,
		    ">>_DtTermPrimScrollWait() GraphicsExpose event received, count=%d\n",
		    ev->xgraphicsexpose.count));
	    Debug('e', fprintf(stderr,
		    ">>_waitOnCopyArea() exposing GraphicsExpose area\n"));
	    Debug('e', fprintf(stderr,
		    ">>                  x=%d  y=%d  width=%d  height=%d\n",
		    ev->xgraphicsexpose.x, ev->xgraphicsexpose.y,
		    ev->xgraphicsexpose.width, ev->xgraphicsexpose.height));
	    (void) _DtTermPrimExposeText(w,
		    ev->xgraphicsexpose.x, ev->xgraphicsexpose.y,
		    ev->xgraphicsexpose.width, ev->xgraphicsexpose.height,
		    False);
	    if (ev->xgraphicsexpose.count > 0) {
		/* more to come... */
		break;
	    }

	case NoExpose:
	    /* we are done... */
	    Debug('e', fprintf(stderr,
		    ">>_waitOnCopyArea() NoExpose event received\n"));
	    tpd->scrollInProgress = False;
	    break;
	}
    }
}

void
_DtTermPrimScrollWait(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int i;
    int exposeY;
    int exposeHeight;
    int scrolledLines;

    Debug('s', fprintf(stderr, ">>_DtTermPrimScrollWait() starting\n"));
    Debug('s', fprintf(stderr,
	    ">>_DtTermPrimScrollWait() scrollLines=%d, scrollsPending=%d\n",
	    tpd->scroll.jump.scrollLines, tpd->scroll.jump.scrollsPending));

    /* make sure the cursor is off... */
    (void) _DtTermPrimCursorOff(w);

    if (tpd->scroll.jump.scrollLines != 0) {
	/* flush so that we can be sure the output was visible before we
	 * scroll it off...
	 */
	(void) XFlush(XtDisplay(w));

	/* figure out the height of the copy area... */
	if (tpd->scroll.jump.scrollLines > 0) {
	    /* scrolling memory up... */
	    Debug('s', fprintf(stderr,
		    ">>_DtTermPrimScrollWait() up,  scrollLines=%d  cellHeight=%d\n",
		    tpd->scroll.jump.scrollLines, tpd->cellHeight));
	    if (tpd->scroll.jump.scrollLines >=
		    tpd->scrollBottomRow - tpd->scrollTopRow + 1) {
		/* a full screen or more -- we can avoid the copy area... */
		tpd->scrollHeight = 0;
		exposeY = tpd->offsetY + tpd->cellHeight *
			(tpd->scrollTopRow);
		exposeHeight = tpd->cellHeight *
			(tpd->scrollBottomRow - tpd->scrollTopRow + 1);
		Debug('s', fprintf(stderr,
			">>_DtTermPrimScrollWait()  height=%d    expose Y=%d  height=%d\n",
			tpd->scrollHeight, exposeY, exposeHeight));
	    } else {
		tpd->scrollHeight = tpd->cellHeight *
			(tpd->scrollBottomRow - tpd->scrollTopRow + 1 -
			tpd->scroll.jump.scrollLines);
		tpd->scrollSrcY = tpd->offsetY + tpd->cellHeight *
			(tpd->scrollTopRow + tpd->scroll.jump.scrollLines);
		tpd->scrollDestY = tpd->offsetY + tpd->cellHeight *
			(tpd->scrollTopRow);
		exposeY = tpd->offsetY + tpd->cellHeight *
			(tpd->scrollBottomRow + 1 -
			tpd->scroll.jump.scrollLines);
		exposeHeight = tpd->cellHeight *
			(tpd->scroll.jump.scrollLines);
		Debug('s', fprintf(stderr,
			">>_DtTermPrimScrollWait()  height=%d  SrcY=%d  DestY=%d    expose Y=%d  height=%d\n",
			tpd->scrollHeight, tpd->scrollSrcY, tpd->scrollDestY,
			exposeY, exposeHeight));
	    }
	} else {
	    /* scrolling memory down... */
	    Debug('s', fprintf(stderr,
		    ">>_DtTermPrimScrollWait() down,  scrollLines=%d  cellHeight=%d\n",
		    tpd->scroll.jump.scrollLines, tpd->cellHeight));
	    if (tpd->scroll.jump.scrollLines <=
		    -(tpd->scrollBottomRow - tpd->scrollTopRow + 1)) {
		/* a full screen or more -- we can avoid the copy area... */
		tpd->scrollHeight = 0;
		exposeY = tpd->offsetY + tpd->cellHeight *
			(tpd->scrollTopRow);
		exposeHeight = tpd->cellHeight *
			(tpd->scrollBottomRow - tpd->scrollTopRow + 1);
		Debug('s', fprintf(stderr,
			">>_DtTermPrimScrollWait()  height=%d    expose Y=%d  height=%d\n",
			tpd->scrollHeight, exposeY, exposeHeight));
	    } else {
		/* remember: scrollLines is **NEGATIVE**...
		 */
		tpd->scrollHeight = tpd->cellHeight * (tpd->scrollBottomRow -
			tpd->scrollTopRow + 1 - -tpd->scroll.jump.scrollLines);
		tpd->scrollSrcY = tpd->offsetY + tpd->cellHeight *
			(tpd->scrollTopRow);
		tpd->scrollDestY = tpd->offsetY + tpd->cellHeight *
			(tpd->scrollTopRow + -tpd->scroll.jump.scrollLines);
		exposeY = tpd->offsetY + tpd->cellHeight *
			(tpd->scrollTopRow);
		exposeHeight = tpd->cellHeight *
			(-tpd->scroll.jump.scrollLines);
		Debug('s', fprintf(stderr,
			">>_DtTermPrimScrollWait()  height=%d  SrcY=%d  DestY=%d    expose Y=%d  height=%d\n",
			tpd->scrollHeight, tpd->scrollSrcY, tpd->scrollDestY,
			exposeY, exposeHeight));
	    }
	}

	/* we need to do a copy area... */
	if (tpd->scrollHeight > 0) {
	    /* calculate scroll area... */
	    tpd->scrollWidth = tpd->cellWidth * tw->term.columns;
	    tpd->scrollSrcX = tpd->offsetX;
	    tpd->scrollDestX = tpd->offsetX;

	    (void) XCopyArea(XtDisplay(w),
						/* Display		*/
			XtWindow(w),		/* Source		*/
			XtWindow(w),		/* Destination		*/
			tpd->renderGC.gc,	/* GC			*/
			tpd->scrollSrcX,		/* source X		*/
			tpd->scrollSrcY,		/* source Y		*/
			tpd->scrollWidth,	/* width		*/
			tpd->scrollHeight,	/* height		*/
			tpd->scrollDestX,	/* destination X	*/
			tpd->scrollDestY);	/* destination Y	*/
	    tpd->scrollInProgress = True;
	}

	/* expose the exposed area.  We need to expose the exposed area
	 * as well as any lines that have been changed.  While we really
	 * could just expose the exposed area and then go back and fill
	 * in any remaining lines that have the flag set, we will first
	 * fill in any lines above the expose area with their flag set,
	 * fill in the expose area, and then fill in any lines below
	 * the expose area with their flag set.  This fills things in
	 * from top to bottom which is much more pleasing visually.
	 */
	Debug('s', fprintf(stderr,
		">>_DtTermPrimScrollWait() exposing scroll area\n"));
	Debug('s', fprintf(stderr,
		">>             x=%d  y=%d  width=%d  height=%d\n",
		tpd->offsetX,
		exposeY,
		tpd->cellWidth * tw->term.columns,
		exposeHeight));

	/* set scrollLines == 0, or renderText will not render them... */
	scrolledLines = tpd->scroll.jump.scrollLines;
	tpd->scroll.jump.scrollLines = 0;
	/* clear the scrolled flag... */
	tpd->scroll.jump.scrolled = False;

	/* refresh any lines above the expose zone that have their
	 * scrollRefreshRows flag set...
	 */
	for (i = 0; i < (exposeY - tpd->offsetX) / tpd->cellHeight; i++) {
	    if (tpd->scrollRefreshRows[i]) {
		(void) _DtTermPrimRefreshText(w, 0, i, tw->term.columns, i);
	    }
	}
	/* expose the refresh expose area... */
	(void) _DtTermPrimExposeText(w,
		tpd->offsetX,
		exposeY,
		tpd->cellWidth * tw->term.columns,
		exposeHeight,
		False);

	/* refresh any lines below the expose zone that have their
	 * scrollRefreshRows flag set...
	 */
	for (i = (exposeY - tpd->offsetX + exposeHeight) / tpd->cellHeight;
		i < tw->term.rows; i++) {
	    if (tpd->scrollRefreshRows[i]) {
		(void) _DtTermPrimRefreshText(w, 0, i, tw->term.columns, i);
	    }
	}

	if (tpd->scrollHeight > 0)
	    tpd->scroll.jump.scrollsPending++;

	(void) _DtTermPrimCursorUpdate(w);
    } else {
	/* scrollLines == 0...
	 */

	/* flush so that we can be sure the output was visible before we
	 * scroll it off...
	 */
	(void) XFlush(XtDisplay(w));

	/* clear the scrolled flag... */
	tpd->scroll.jump.scrolled = False;

	/* refresh any lines below the expose zone that have their
	 * scrollRefreshRows flag set...
	 */

	for (i = 0; i < tw->term.rows; i++) {
	    if (tpd->scrollRefreshRows[i]) {
		(void) _DtTermPrimRefreshText(w, 0, i, tw->term.columns, i);
	    }
	}
    }

    while (tpd->scroll.jump.scrollsPending > 0) {
	(void) waitOnCopyArea(w);
	tpd->scroll.jump.scrollsPending--;
    }

    Debug('s', fprintf(stderr, ">>_DtTermPrimScrollWait() finished\n"));
}
    
static void
doActualScroll(Widget w, int lines)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int exposeY;
    int exposeHeight;
    int i;

    /* make sure the cursor is off... */
    (void) _DtTermPrimCursorOff(w);

    /* figure out the height of the copy area... */
    if (lines > 0) {
	/* scrolling memory up... */
	if (lines >= tpd->scrollBottomRow - tpd->scrollTopRow + 1) {
	    /* we are scrolling a full screen or more, so there is nothing
	     * to copy...
	     */
	    tpd->scrollHeight = 0;
	    exposeY = tpd->offsetY + tpd->cellHeight * tpd->scrollTopRow;
	    exposeHeight = tpd->cellHeight *
		    (tpd->scrollBottomRow - tpd->scrollTopRow + 1);
	} else {
	    tpd->scrollHeight = tpd->cellHeight *
		    (tpd->scrollBottomRow - tpd->scrollTopRow + 1 - lines);
	    tpd->scrollSrcY = tpd->offsetY + tpd->cellHeight *
		    (tpd->scrollTopRow + lines);
	    tpd->scrollDestY = tpd->offsetY + tpd->cellHeight *
		    (tpd->scrollTopRow);

	    /* expose bottom lines lines... */
	    exposeY = tpd->offsetY + tpd->cellHeight *
		    (tpd->scrollBottomRow + 1 - lines);
	    exposeHeight = tpd->cellHeight * lines;
	}
    } else {
	/* scrolling memory down... */
	/* lines are negative -- make positive... */
	lines = -lines;
	if (lines >= tw->term.rows) {
	    /* we are scrolling a full screen or more, so there is nothing
	     * to copy...
	     */
	    tpd->scrollHeight = 0;
	    exposeY = tpd->offsetY + tpd->cellHeight * tpd->scrollTopRow;
	    exposeHeight = tpd->cellHeight *
		    (tpd->scrollBottomRow - tpd->scrollTopRow + 1);
	} else {
	    tpd->scrollHeight = tpd->cellHeight *
		    (tpd->scrollBottomRow - tpd->scrollTopRow + 1 - lines);
	    tpd->scrollSrcY = tpd->offsetY + tpd->cellHeight *
		    (tpd->scrollTopRow);
	    tpd->scrollDestY = tpd->offsetY + tpd->cellHeight *
		    (tpd->scrollTopRow + lines);

	    /* expose top lines lines... */
	    exposeY = tpd->offsetY + tpd->cellHeight *
		    (tpd->scrollTopRow);
	    exposeHeight = tpd->cellHeight * lines;
	}
    }

    /* scroll the display... */
    if (tpd->scrollHeight > 0) {
	/* calculate scroll area... */
	tpd->scrollWidth = tpd->cellWidth * tw->term.columns;
	tpd->scrollSrcX = tpd->offsetX;
	tpd->scrollDestX = tpd->offsetX;

	/* copy the area... */
	XCopyArea(XtDisplay(w),			/* Display		*/
		XtWindow(w),			/* Source		*/
		XtWindow(w),			/* Destination		*/
		tpd->renderGC.gc,		/* GC			*/
		tpd->scrollSrcX,			/* source X		*/
		tpd->scrollSrcY,			/* source Y		*/
		tpd->scrollWidth,		/* width		*/
		tpd->scrollHeight,		/* height		*/
		tpd->scrollDestX,		/* destination X	*/
		tpd->scrollDestY);		/* destination Y	*/
	tpd->scrollInProgress = True;
    }

    /* clear the old area... */
    (void) XFillRectangle(XtDisplay(w),		/* Display		*/
	    XtWindow(w),			/* Drawable		*/
	    tpd->clearGC.gc,			/* GC			*/
	    tpd->offsetX,			/* x			*/
	    exposeY,				/* y			*/
	    tw->term.columns * tpd->cellWidth,
						/* width		*/
	    exposeHeight);			/* height		*/

    /* expose the old area... */
    (void) _DtTermPrimExposeText(w,
	    tpd->offsetX,
	    exposeY,
	    tw->term.columns * tpd->cellWidth,
	    exposeHeight, False);

    (void) _DtTermPrimCursorUpdate(w);
}

/**************************************************************************
 *  Function:
 *	_DtTermPrimScrollText(): scroll the terminal window
 *
 *  Parameters:
 *	Widget w: widget to scroll
 *	int lines: lines to scroll
 *
 *  Returns:
 *	<nothing>
 *
 *  Notes:
 *
 *	This function will scroll the terminals window.  It supports
 *	both jump scroll and non-jump scroll (single line at a time).
 *	In jump scroll, the maximum number of lines that will be
 *	scrolled at a time is the length of the display.  This insures
 *	that all text will be displayed for at least an instant.
 *
 *	In jump scroll mode, scrolling is performed as follows:
 *
 *	- Scroll request is made:
 *
 *	    + Number of lines to scroll is incremented.
 *
 *	    + If the number of lines is < the length of the screen,
 *	      rendering is discontinued and the actual scroll request is
 *	      delayed until the the cursor is turned back on (i.e.,
 *	      select(2) says there is no more input on the pty).
 *
 *	    + If the number of lines is >= the length of the screen, the
 *	      copy area is performed, and the world blocks on exposure
 *	      events on the window.  When a no-expose request, or the
 *	      last graphics expose request is received, any remaining
 *	      lines will be queue'ed up.
 *
 *	- The code that processes data coming over the pty determines
 *	  via select(2) that there is no data to be read.
 *
 *	    + The copy area is performed, and the world blocks on
 *	      exposure events on the window.  When a no-expose request,
 *	      or the last graphics expose request is received, the
 *	      scroll is complete.
 *
 *
 *	In non-jump scroll mode, the following actions are performed:
 *
 *	- If there is no scroll in progress, the copy area is performed.
 *	  Any additional input is processed as normal.
 *
 *	- When the generated no-expose or final graphics expose event is
 *	  processed, the pending scroll flag is cleared.
 *
 *	- If there is a scroll in progress, the new scroll is queued up.
 *	  Any remaining pty input will be queued and the pty will be
 *	  dropped from Xt input select.
 *
 *	- When the generated no-expose or final graphics expose event is
 *	  processed, the queued copy area will be performed.  Any queued
 *	  pty input will be processed and the pty will be added to the
 *	  Xt input select.
 */

void
_DtTermPrimScrollText(Widget w, short lines)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    short oldTopRow;
    short newTopRow;
    int lines2;
    int i;

    /* figure out what our new top row will be.  It is limitied by the
     * beginning of memory (i.e., we can't scroll above line 0) and the
     * lastUsedRow (i.e., we can't scroll the lastUsedRow off the screen).
     * we will then use this value to clip lines...
     */
    oldTopRow = tpd->topRow;
    if (lines > 0) {
	newTopRow = tpd->topRow + lines;
	/* don't scroll past the lastUsedRow... */
	if (newTopRow > (tpd->lastUsedRow - tpd->scrollLockTopRow)) {
	    newTopRow = tpd->lastUsedRow - tpd->scrollLockTopRow;
	}
    } else {
	if (tpd->useHistoryBuffer) {
	    if ((tpd->topRow + tpd->lastUsedHistoryRow) >= -lines)
		newTopRow = tpd->topRow - -lines;
	    else
		newTopRow = -tpd->lastUsedHistoryRow;
	} else {
	    if (tpd->topRow >= -lines)
		newTopRow = tpd->topRow - -lines;
	    else
		newTopRow = 0;
	}
    }

    /* adjust lines to the clipped newTopRow... */
    lines = newTopRow - oldTopRow;

    /* we don't need to do any for 0... */
    if (0 == lines)
	return;

    if ((tw->term.jumpScroll &&
	    ((lines + tpd->scroll.jump.scrollLines) >
	    (tw->term.rows -
		tpd->scrollLockTopRow -
		(tw->term.rows - 1 - tpd->scrollLockBottomRow)))) ||
	    ((lines + tpd->scroll.jump.scrollLines) <
	    -(tw->term.rows -
		tpd->scrollLockTopRow -
		(tw->term.rows - 1 - tpd->scrollLockBottomRow))) || 
	    (tpd->scrollTopRow != tpd->scrollLockTopRow) ||
	    (tpd->scrollBottomRow != tpd->scrollLockBottomRow)) {
	/* scroll out the queued up jump scroll lines... */
	if (tpd->scroll.jump.scrollLines != 0) {
	    (void) _DtTermPrimScrollWait(w);
	}
    }

    /* move the memory locked region in screen memory...
     */
    /* Assuming a 24 line buffer with lock on 10 and 20 and 30 lines
     * of screen buffer, with one below and scrolling 3 lines, we do
     * the following:
     *
     *  oooooLLLLLLLLLL          LLLLo
     *  ||||||||||||||||||||||||||||||
     *  000000000011111111112222222222
     *  012345678901234567890123456789
     *
     * move lines 15-17 to the top of the buffer:
     *
     *  ooooooooLLLLLLLLLL       LLLLo
     *  ||||||||||||||||||||||||||||||
     *  111000000000011111112222222222
     *  567012345678901234890123456789
     *
     * move line 29 to the bottom of the unlocked area:
     * 
     *  ooooooooLLLLLLLLLL        LLLL
     *  ||||||||||||||||||||||||||||||
     *  111000000000011111112222222222
     *  567012345678901234890123495678
     *
     * clear the top2 lines and move it to the bottom of the
     * unlocked area:
     * 
     *  ooooooLLLLLLLLLL          LLLL
     *  ||||||||||||||||||||||||||||||
     *  100000000001111111222221122222
     *  701234567890123489012345695678
     * 
     */

    /* if we have a history buffer and we are going to be scrolling lines
     * off of the buffer, we should move them into the history buffer.
     */
    if ((tpd->scrollLockTopRow == 0) &&
	    (tpd->scrollLockBottomRow >= (tw->term.rows - 1))) {
	/*DKS*/
    }

    /* first move the top lock area... */
    if (tpd->scrollLockTopRow > 0) {
	(void) _DtTermPrimBufferMoveLockArea(tpd->termBuffer,
		tpd->topRow + lines, tpd->topRow, tpd->scrollLockTopRow);
    }

    /* now move the bottom lock area... */
    if (tpd->scrollLockBottomRow < (tw->term.rows - 1)) {
	/* if we are moving up, let's move as many rows as we can from
	 * bottom offscreen memory...
	 */
	lines2 = 0;

	if ((lines > 0) && (tpd->bufferRows > (tpd->topRow + tw->term.rows))) {
	    /* clip lines2 to the number of lines that we can obtain
	     * from the bottom of the buffer...
	     */
	    lines2 = lines;
	    if (lines2 > (tpd->bufferRows - (tpd->topRow + tw->term.rows))) {
		lines2 = tpd->bufferRows - (tpd->topRow + tw->term.rows);
	    }

	    if (lines2 > 0) {
		/* move them... */
		(void) _DtTermPrimBufferMoveLockArea(tpd->termBuffer,
			tpd->topRow + tpd->scrollLockBottomRow - lines2,
			tpd->topRow + tpd->scrollLockBottomRow,
			(tw->term.rows - 1) - tpd->scrollLockBottomRow);
	    }
	}

	/* figure out how many lines we will need to take from the
	 * top...
	 */
	lines2 = lines - lines2;
	if (lines2 > 0) {
	    for (i = 0; i < lines2; i++) {
		(void) _DtTermPrimBufferClearLine(tpd->termBuffer, i, 0);
	    }
	    (void) _DtTermPrimBufferMoveLockArea(tpd->termBuffer,
		    0,
		    lines2,
		    tpd->topRow + tpd->scrollLockBottomRow + 1 - lines);
	    newTopRow -= lines2;
	}
    }

    /* set topRow... */
    tpd->topRow = newTopRow;

    if (tw->term.jumpScroll) {
	/* jump scroll...
	 */
	/* queue up the lines for scrolling... */
	tpd->scroll.jump.scrollLines += lines;
	tpd->scroll.jump.scrolled = True;
	tpd->scrollTopRow = tpd->scrollLockTopRow;
	tpd->scrollBottomRow = tpd->scrollLockBottomRow;

	/* scroll out the scrollRefreshRows flags now... */
	/* NOTE: we loose the refresh flag for all rows that are scrolled
	 * off.  The result of this is that if we do a scroll up followed
	 * by a scroll down, we will (at a minimum) refresh the top and
	 * bottom lines.  One workaround would be to tripple the buffer
	 * and keep the lines that get scrolled off the top or bottom.
	 * This would probably break something, since there are times
	 * that the scrolled off line gets modified or even cleared (such
	 * as delete line off of the top of the display), so this might
	 * not be a very good idea.
	 */
	if (lines > 0) {
	    /* scroll them up... */
	    for (i = tpd->scrollTopRow;
		    i <= tpd->scrollBottomRow - lines; i++) {
		tpd->scrollRefreshRows[i] =
			tpd->scrollRefreshRows[i + lines];
	    }
	    /* set the rest... */
	    for (; i <= tpd->scrollBottomRow; i++) {
		tpd->scrollRefreshRows[i] = True;
	    }
	} else {
	    /* remember, lines is negative... */
	    for (i = tpd->scrollBottomRow;
		    i >= tpd->scrollTopRow + -lines; i--) {
		tpd->scrollRefreshRows[i] =
			tpd->scrollRefreshRows[i - -lines];
	    }
	    for (; i >= tpd->scrollTopRow; i--) {
		tpd->scrollRefreshRows[i] = True;
	    }
	}
    } else {
	/* non jump-scroll...
	 */
	if (tpd->scrollInProgress) {
	    /* let's queue up the scroll and bail out...
	     */
	    tpd->scroll.nojump.pendingScrollLines += lines;
	    /*DKS!!! v v v v v v v v v v v v v v v v v v v v v v */
	    if (tpd->scroll.nojump.pendingScrollLines > 1)
		(void) fprintf(stderr,
			"tell Dave _DtTermPrimScrollText has pendingScrollLines=%d\n",
			tpd->scroll.nojump.pendingScrollLines);
	    /*DKS!!! ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ */
	    tpd->scroll.nojump.pendingScroll = True;
	    tpd->scroll.nojump.pendingScrollTopRow = tpd->scrollLockTopRow;
	    tpd->scroll.nojump.pendingScrollBottomRow = tpd->scrollLockBottomRow;
	    (void) _DtTermPrimStartOrStopPtyInput(w);
	    return;
	}

	/* no scroll in progress, let's scroll it... */
	tpd->scrollTopRow = tpd->scrollLockTopRow;
	tpd->scrollBottomRow = tpd->scrollLockBottomRow;
	(void) doActualScroll(w, lines);
    }

    /*  This case:    (tpd->scrollLockTopRow > 0)
        is handled elsewhere.
     */
    if ( lines > 0 && tpd->scrollLockBottomRow < (tw->term.rows-1)) {
             _DtTermPrimSelectMoveLines(w, tpd->scrollLockTopRow + lines, 
                         tpd->scrollLockTopRow,
                         tpd->scrollLockBottomRow - tpd->scrollLockTopRow);
    }

    return;
}

void
_DtTermPrimScrollTextTo(Widget w, short topRow)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int oldTopRow;

    if (topRow == tpd->topRow) {
	/* already there...  */
	return;
    }

    (void) _DtTermPrimScrollText(w, topRow - tpd->topRow);
    return;
}

void
_DtTermPrimScrollTextArea(Widget w, short scrollStart, short scrollLength,
	short scrollDistance)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int oldTopRow;
    int exposeY;
    int exposeHeight;
    int i;

#ifdef	NOTDEF
    if (scrollDistance > 0) {
	/* scrolling text up... */
	if (tpd->lastUsedRow <= tpd->topRow + scrollStart) {
	    /* scrolling past lastUsedRow -- nothing to scroll... */
	    return;
	}
    }
#endif	/* NOTDEF */
    /* clip the length... */
    if (scrollStart + scrollLength > tw->term.rows) {
	scrollLength = tw->term.rows - scrollStart;
    }

    /* we don't need to do anything for 0 distance... */
    if (0 == scrollDistance) {
	return;
    }

    /* if we are in jumpscroll mode and:
     *	    -our region differs (scrollTopRow and scrollBottomRow), or
     *	    -we are going to queue more lines than the scroll region
     * we need to complete the queued scroll...
     */
    if (tw->term.jumpScroll &&
	    ((scrollDistance + tpd->scroll.jump.scrollLines > scrollLength) ||
	    (scrollDistance + tpd->scroll.jump.scrollLines < -scrollLength) ||
	    (tpd->scrollTopRow != scrollStart) ||
	    (tpd->scrollBottomRow != scrollStart + scrollLength - 1))) {
	/* scroll out the queued up jump scroll lines... */
	if (tpd->scroll.jump.scrolled != 0) {
	    (void) _DtTermPrimScrollWait(w);
	}
    }

    if (tw->term.jumpScroll) {
	/* jump scroll...
	 */
	/* queue up the lines for scrolling... */
	tpd->scroll.jump.scrollLines += scrollDistance;
	tpd->scroll.jump.scrolled = True;
	tpd->scrollTopRow = scrollStart;
	tpd->scrollBottomRow = scrollStart + scrollLength - 1;

	/* scroll out the scrollRefreshRows flags now... */
	if (scrollDistance > 0) {
	    /* scroll them up... */
	    for (i = tpd->scrollTopRow;
		    i <= tpd->scrollBottomRow - scrollDistance; i++) {
		tpd->scrollRefreshRows[i] =
			tpd->scrollRefreshRows[i + scrollDistance];
	    }
	    /* set the rest... */
	    for (; i <= tpd->scrollBottomRow; i++) {
		tpd->scrollRefreshRows[i] = True;
	    }
	} else {
	    for (i = tpd->scrollBottomRow;
		    i >= tpd->scrollTopRow + -scrollDistance; i--) {
		tpd->scrollRefreshRows[i] =
			tpd->scrollRefreshRows[i - -scrollDistance];
	    }
	    for (; i >= tpd->scrollTopRow; i--) {
		tpd->scrollRefreshRows[i] = True;
	    }
	}
    } else {
	/* non jump scroll...
	 */
	if (tpd->scrollInProgress) {
	    /* let's queue up the scroll and bail out...
	     */
	    tpd->scroll.nojump.pendingScrollLines += scrollDistance;
	    /*DKS!!! v v v v v v v v v v v v v v v v v v v v v v */
	    if (tpd->scroll.nojump.pendingScrollLines > 1)
		(void) fprintf(stderr,
			"tell Dave _DtTermPrimScrollText has pendingScrollLines=%d\n",
			tpd->scroll.nojump.pendingScrollLines);
	    /*DKS!!! ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ */
	    tpd->scroll.nojump.pendingScroll = True;
	    tpd->scroll.nojump.pendingScrollTopRow = scrollStart;
	    tpd->scroll.nojump.pendingScrollBottomRow =
		    scrollStart + scrollLength - 1;
	    (void) _DtTermPrimStartOrStopPtyInput(w);
	    return;
	}

	/* no scroll in progress, let's scroll it... */
	tpd->scrollTopRow = scrollStart;
	tpd->scrollBottomRow = scrollStart + scrollLength - 1;
	(void) doActualScroll(w, scrollDistance);
	tpd->scrollInProgress = True;
    }
}

/**************************************************************************
 *  Function:
 *	_DtTermPrimScrollComplete(): finish a scroll.  This is normall called
 *		to complete a non-jump scroll, or if it is necessary to
 *		complete and flush any scroll.
 *
 *  Parameters:
 *	Widget w: widget to scroll
 *
 *  Returns:
 *	<nothing>
 *
 *  Notes:
 *
 *	This function is used in non-jump scroll only!!!
 *
 * 	This function is called by the Term widget when a no-expose or
 * 	final graphics expose event is received (which was generated by
 *	the previous scroll's copy area).  It will invoke the scroll that
 *	was queued up and caused pty input to be blocked.
 */

void
_DtTermPrimScrollComplete(Widget w, Boolean flush)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    if (tw->term.jumpScroll) {
	/* jump scroll...
	 */
	if (flush) {
	    /* we need to finish any pending scrolls...
	     */
	    /* In jump scroll, all we need to do is to invoke the scroll.
	     * _DtTermPrimScrollWait() will perform the actual copy area and
	     * wait for the generated graphics expose / no expose events...
	     */
	    if (tpd->scroll.jump.scrolled) {
		Debug('s', fprintf(stderr,
			">>_DtTermPrimScrollComplete() calling _DtTermPrimScrollWait()\n"));
		(void) _DtTermPrimScrollWait(w);
	    }
	}
    } else {
	/* non-jump scroll...
	 */

	if (flush && tpd->scrollInProgress) {
	    /* if we have a pending scroll, we need to wait for it to
	     * complete.  Normally, we would service this out of the
	     * XtMainLoop() dispatch loop, but for some reason we need
	     * to do it now.
	     */
	    (void) waitOnCopyArea(w);
	    tpd->scrollInProgress = False;
	}

	if (!tw->term.jumpScroll && tpd->scroll.nojump.pendingScroll) {
	    /* do the queued scroll... */
	    tpd->scrollTopRow = tpd->scroll.nojump.pendingScrollTopRow;
	    tpd->scrollBottomRow = tpd->scroll.nojump.pendingScrollBottomRow;
	    (void) doActualScroll(w, tpd->scroll.nojump.pendingScrollLines);

	    /* no lines pending, but there is a scroll in progress... */
	    tpd->scroll.nojump.pendingScrollLines = 0;
	    tpd->scrollInProgress = True;
	    tpd->scroll.nojump.pendingScroll = False;

	    if (flush) {
		/* wait for it to complete...
		 */
		(void) waitOnCopyArea(w);
		tpd->scrollInProgress = False;

		/* If there is any pending input or if we need to reinstall
		 * the input select, force a pty read...
		 */
		(void) XtAppAddTimeOut(XtWidgetToApplicationContext(w),
			0, _DtTermPrimForcePtyRead, (XtPointer) w);
	    }
	}
    }
}

void
_DtTermPrimScrollCompleteIfNecessary(Widget w, short scrollTopRow,
	short scrollBottomRow, short lines)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    short maxJumpScrollLines;

    if ((scrollTopRow != tpd->scrollTopRow) ||
	    (scrollBottomRow != tpd->scrollBottomRow)) {
	(void) _DtTermPrimScrollComplete(w, True);
	return;
    }

    if (tw->term.jumpScroll) {
	maxJumpScrollLines = tpd->scrollBottomRow - tpd->scrollTopRow + 1;
	if ((lines + tpd->scroll.jump.scrollLines > maxJumpScrollLines) ||
		(lines + tpd->scroll.jump.scrollLines < -maxJumpScrollLines))
	    (void) _DtTermPrimScrollComplete(w, True);
	    return;
    } else {
	if (!tw->term.jumpScroll && tpd->scroll.nojump.pendingScroll) {
	    (void) _DtTermPrimScrollComplete(w, True);
	    return;
	}
    }
}
