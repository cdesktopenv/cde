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
static char rcs_id[] = "$TOG: TermFunction.c /main/2 1998/03/16 14:41:27 mgreess $";
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
#include "TermPrimI.h"
#include "TermP.h"
#include "TermBuffer.h"
#include "TermData.h"
#include "TermPrimData.h"
#include "TermFunction.h"
#include "TermPrimSetPty.h"
#include "TermPrimMessageCatI.h"
#include "TermPrimParserP.h" 
#include "TermParseTable.h"

static void
termFuncErase
(
    Widget         w,
    int            count,
    DtEraseMode    eraseSwitch,
    FunctionSource functionSource
);



/*** SCROLL *******************************************************************
 * 
 *   ####    ####   #####    ####   #       #
 *  #       #    #  #    #  #    #  #       #
 *   ####   #       #    #  #    #  #       #
 *       #  #       #####   #    #  #       #
 *  #    #  #    #  #   #   #    #  #       #
 *   ####    ####   #    #   ####   ######  ######
 */
#define EXIT_IF_MARGINS_SET(w) if( (w)->term.tpd->scrollLockTopRow != 0 ||  \
                                   (w)->term.tpd->scrollLockBottomRow !=    \
                                             (w)->term.rows-1 )   \
                                    return;

static void
doScroll(Widget w, short lines)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    short newtop;

    EXIT_IF_MARGINS_SET(tw)
    newtop = tpd->topRow + lines;
    /* don't go past top of active buffer or past end of history buffer */
    if (newtop>0) newtop=0;  
    if (newtop<-tpd->lastUsedHistoryRow) newtop=-tpd->lastUsedHistoryRow;         
    /* perform scroll... */
    (void) _DtTermPrimScrollTextTo(w, newtop);  
    (void) _DtTermPrimScrollComplete((Widget) tw, True);
}


void
_DtTermFuncScroll(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    /* make sure the cursor is off... */
    (void) _DtTermPrimCursorOff(w);

    /* and scroll the text... */
    (void) doScroll(w, count);
    (void) _DtTermPrimCursorOn(w);
    return;
}

void
_DtTermFuncBeginningOfBuffer(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermWidget vtw = (DtTermWidget) w;
    struct termData *tpd = tw->term.tpd;

    EXIT_IF_MARGINS_SET(tw)
    /* make sure the cursor is off... */
    (void) _DtTermPrimCursorOff(w);
    (void) _DtTermPrimScrollTextTo(w, -tpd->lastUsedHistoryRow);
    (void) _DtTermPrimScrollComplete((Widget) tw, True);
    (void) _DtTermPrimCursorOn(w);
}

void
_DtTermFuncEndOfBuffer(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    EXIT_IF_MARGINS_SET(tw)
    (void) _DtTermPrimCursorOff(w);
    (void) _DtTermPrimScrollTextTo(w, 0);
    (void) _DtTermPrimScrollComplete((Widget) tw, True);
    (void) _DtTermPrimCursorOn(w);
}


/*** CLEAR MEMORY / LINE ******************************************************
 * 
 *   ####   #       ######    ##    #####
 *  #    #  #       #        #  #   #    #
 *  #       #       #####   #    #  #    #
 *  #       #       #       ######  #####
 *  #    #  #       #       #    #  #   #
 *   ####   ######  ######  #    #  #    #
 * 
 *                               #
 *  #    #  ######  #    #      #   #          #    #    #  ######
 *  ##  ##  #       ##  ##     #    #          #    ##   #  #
 *  # ## #  #####   # ## #    #     #          #    # #  #  #####
 *  #    #  #       #    #   #      #          #    #  # #  #
 *  #    #  #       #    #  #       #          #    #   ##  #
 *  #    #  ######  #    # #        ######     #    #    #  ######
 */

static void
clearToEndOfMemory(Widget w)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int i;

    /* clear from current cursor to end of memory... */
    if (tpd->cursorRow + tpd->topRow < tpd->lastUsedRow) {
	/* clear the current line to end of memory... */
	(void) _DtTermPrimBufferClearLine(tpd->termBuffer,
		tpd->topRow + tpd->cursorRow, tpd->cursorColumn);
	
	/* clear the next line to the end of memory... */
	for (i = tpd->topRow + tpd->cursorRow + 1; i < tpd->lastUsedRow; i++)
	    (void) _DtTermPrimBufferClearLine(tpd->termBuffer, i, 0);

	/* render the current line from cursor to end of line... */
	(void) _DtTermPrimRefreshText(w, tpd->cursorColumn, tpd->cursorRow,
		tw->term.columns, tpd->cursorRow);

	/* render the rest of the lines... */
	if (tpd->cursorRow + 1 < tw->term.rows) {
	    (void) _DtTermPrimRefreshText(w, 0, tpd->cursorRow + 1,
		    tw->term.columns, tw->term.rows - 1);
	}

	/* set the lastUsedRow... */
	if (tpd->cursorRow > tpd->memoryLockRow) {
	    if (tpd->cursorColumn == 0) {
		/* don't count the current line -- lastUsedRow is the
		 * line above...
		 */
		tpd->lastUsedRow = tpd->topRow + tpd->cursorRow;
	    } else {
		tpd->lastUsedRow = tpd->topRow + tpd->cursorRow + 1;
	    }
	} else  {
	    tpd->lastUsedRow = tpd->topRow + tpd->memoryLockRow + 1;
	}
    }
    return;
}
	    
/*
** Erase characters on the specified row (and clear the enhancements) with
** the appropriate semantics.  For VT class emulators, there are several
** ways to erase characters:
**      - from the active position to the end of line
**      - erase the specified number of characters starting at the current
**        cursor position
**      - from the start of the line to the active position
**      - erase the entire line
**      - from the active position to the end of the buffer
**      - erase the specified number of lines starting at the current
**        cursor position
**      - from the start of the buffer to the active position
**      - erase the entire buffer
**
** We handle the easy stuff here, and pass the rest to _DtTermBufferErase.
*/
static void
termFuncErase
(
    Widget         w,
    int            count,
    DtEraseMode    eraseSwitch,
    FunctionSource functionSource
)
{
    DtTermPrimitiveWidget tw  = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd = tw->term.tpd;
    TermBuffer            tb  = tpd->termBuffer;
    short row;
    short col;

    row = tpd->cursorRow + tpd->topRow;
    col = tpd->cursorColumn;
    
    switch(eraseSwitch)
    {
      case eraseToEOL:
	/*
	** erase from the current cursor position to the end of the line
	*/
	_DtTermFuncClearToEndOfLine(w, 1, functionSource);
	break;

      case eraseLine:
	/*
	** erase the current line
	*/
	_DtTermFuncClearLine(w, 1, functionSource);
	break;

      case eraseToEOB:
	(void) _DtTermFuncClearToEndOfBuffer(w, 1, functionSource);
	break;

      case eraseFromRow0Col0:
	/*
	** first clear all lines from the top of the screen upto the
	** current line...
	*/
	row = tpd->cursorRow;
	col = tpd->cursorColumn;

	tpd->cursorColumn = 0;
	for (tpd->cursorRow = tpd->topRow;
	     tpd->cursorRow < row;
	     tpd->cursorRow++)
	{
	    _DtTermFuncClearLine(w, 1, functionSource);
	}
	tpd->cursorRow    = row;
	tpd->cursorColumn = col;

	/*
	** now clear all characters from col 0 to the current cursor position
	** on the current line...
	*/
	_DtTermBufferErase(((DtTermPrimitiveWidget)w)->term.tpd->termBuffer, 
			   tpd->cursorRow + tpd->topRow, tpd->cursorColumn,
                           count, eraseFromCol0);

	(void) _DtTermPrimRefreshText(w, 0, tpd->cursorRow,
		    tpd->cursorColumn + 1 + (tpd->mbCurMax > 1 ? 1:0),
                    tpd->cursorRow);
	break;

      case eraseBuffer:
	(void) _DtTermFuncClearBuffer(w, 0, functionSource);
	break;

      case eraseFromCol0:
	_DtTermBufferErase(((DtTermPrimitiveWidget)w)->term.tpd->termBuffer, 
			   tpd->cursorRow + tpd->topRow, tpd->cursorColumn,
			   count, eraseSwitch);
	(void) _DtTermPrimRefreshText(w, 0, tpd->cursorRow,
		    tpd->cursorColumn + 1 + (tpd->mbCurMax > 1 ? 1:0),
                    tpd->cursorRow);
      case eraseCharCount:
	_DtTermBufferErase(((DtTermPrimitiveWidget)w)->term.tpd->termBuffer, 
			   tpd->cursorRow + tpd->topRow, tpd->cursorColumn,
			   count, eraseSwitch);
	(void) _DtTermPrimRefreshText(w, 
		    tpd->cursorColumn - (tpd->mbCurMax > 1 ? 1:0),
		    tpd->cursorRow,
		    tpd->cursorColumn + count + (tpd->mbCurMax > 1 ? 1:0),
		    tpd->cursorRow);
	break;
    }
}

void
_DtTermFuncEraseInDisplay
(
    Widget         w,
    int            count,
    FunctionSource functionSource
)
{
    termFuncErase(w, 0, (DtEraseMode)count, functionSource);
}

void
_DtTermFuncEraseInLine
(
    Widget         w,
    int            count,
    FunctionSource functionSource
)
{
    termFuncErase(w, 0, (DtEraseMode)count, functionSource);
}

void
_DtTermFuncEraseCharacter
(
    Widget         w,
    int            count,
    FunctionSource functionSource
)
{
    termFuncErase(w, count, eraseCharCount, functionSource);
}

void
_DtTermFuncClearToEndOfBuffer(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget      tw        = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd        = tw->term.tpd;
    short trow ;

    /* turn the cursor off... */
    (void) _DtTermPrimCursorOff(w);

    (void) clearToEndOfMemory(w);
}


void
_DtTermFuncClearBuffer(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    DtTermWidget vtw = (DtTermWidget) w;
    struct termData *tpd = tw->term.tpd;
    int i;

    /* turn the cursor off... */
    (void) _DtTermPrimCursorOff(w);

    /* move the cursor to the first line after locked memory... */
    /* move to cusor column 0 for now so that we will clear the entire
     * line...
     */
    tpd->cursorColumn = 0;
    tpd->cursorRow = 0;

    /* and clear from the current cursor position to end of memory... */
    (void) clearToEndOfMemory(w);

    /* and set the final cursor position if origin mode    */
    if ( vtw->vt.td->originMode) {
        tpd->cursorRow    = vtw->term.tpd->scrollLockTopRow;
      }
}


void
_DtTermFuncClearLine(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int lineWidth;

    if (tpd->topRow + tpd->cursorRow < tpd->lastUsedRow) {
	/* get the current width... */
	lineWidth = _DtTermPrimBufferGetLineWidth(tpd->termBuffer,
		        tpd->topRow + tpd->cursorRow);
	/* clear line... */
	(void) _DtTermPrimBufferClearLine(tpd->termBuffer,
		tpd->topRow + tpd->cursorRow, 0);
	/* render the line.  We only need to refresh the old line
	 * width...
	 */
	if (lineWidth > 0)
	    (void) _DtTermPrimRefreshText(w, 0, tpd->cursorRow, lineWidth,
		    tpd->cursorRow);
    }
}


void
_DtTermFuncClearToEndOfLine(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int lineWidth;

    if (tpd->topRow + tpd->cursorRow < tpd->lastUsedRow) {
	/* turn the cursor off... */
	(void) _DtTermPrimCursorOff(w);

	/* get the current width... */
	lineWidth = _DtTermPrimBufferGetLineWidth(tpd->termBuffer,
		        tpd->topRow + tpd->cursorRow);
	/* clear line... */
	if (lineWidth > tpd->cursorColumn) {
	    (void) _DtTermPrimBufferClearLine(tpd->termBuffer,
		    tpd->topRow + tpd->cursorRow, tpd->cursorColumn);
	    /* render the line.  We only need to refresh the old line
	     * width...
	     */
	    (void) _DtTermPrimRefreshText(w, 
		    tpd->cursorColumn - (tpd->mbCurMax > 1 ? 1 : 0),
		    tpd->cursorRow,
		    lineWidth, tpd->cursorRow);
	}
	/* if this is the last row, we are not in memory lock land,
	 * and we are in column 0, it is no longer the last used row...
	 */
	if ((tpd->topRow + tpd->cursorRow == tpd->lastUsedRow - 1) &&
		(tpd->cursorColumn == 0) &&
		(tpd->cursorRow > tpd->memoryLockRow)) {
	    tpd->lastUsedRow = tpd->topRow + tpd->cursorRow;
	}
    }
}

/*
** Delete "count" characters from the buffer, VT class terminals do not
** support delete with wrap, so no need to worry about it.
*/
void
_DtTermFuncDeleteChar
(
    Widget         w,
    int            count,
    FunctionSource functionSource
)
{
    DtTermPrimData tpd = ((DtTermPrimitiveWidget) w)->term.tpd;
    short          deleteWidth;
    short          row;
    short          col;

    row = tpd->topRow + tpd->cursorRow;
    col = tpd->cursorColumn;
    
    if (row < tpd->lastUsedRow && count > 0)
    {
	deleteWidth = count;
	(void) _DtTermPrimCursorOff(w);
	_DtTermBufferDelete(tpd->termBuffer, &row, &col, &deleteWidth);
	(void) _DtTermPrimRefreshText(w, col, row,
				      ((DtTermPrimitiveWidget) w)->term.columns,
				      row);

    }
}

void
_DtTermFuncDeleteLine(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    /* we might as well turn the cursor off now, since we will need to
     * change the cursor column to the left margin when we are done
     * anyway...
     */
    (void) _DtTermPrimCursorOff(w);
    if (tpd->topRow + tpd->cursorRow < tpd->lastUsedRow) {
	/* complete any pending scroll if the scroll area has changed... */
	(void) _DtTermPrimScrollCompleteIfNecessary(w, tpd->cursorRow,
		tw->term.rows - 1, count);

	/* clip the line count... */
        if (count > tpd->scrollLockBottomRow -tpd->scrollLockTopRow +1)
           count = tpd->scrollLockBottomRow -tpd->scrollLockTopRow +1 ;
#ifdef	BOGUS
	if (tpd->topRow + tpd->cursorRow + count > tpd->lastUsedRow) {
	    count = tpd->lastUsedRow - (tpd->topRow + tpd->cursorRow);
	}
#endif	/* BOBUS */
	if (tpd->topRow + tpd->cursorRow + count > tpd->scrollLockBottomRow) {
	    count = tpd->scrollLockBottomRow -
		    (tpd->topRow + tpd->cursorRow) + 1;
	}

	/* special case out when there is a single line and we are
	 * deleting the top line...
	 */
	if (((tpd->lastUsedRow == 1) && (tpd->cursorRow == 0)) ||
            (tpd->cursorRow + 1 == tpd->lastUsedRow)) {
	    /* we need to clear the line, re-render it, and we are done...
	     */
	    (void) _DtTermPrimBufferClearLine(tpd->termBuffer,
		    tpd->topRow + tpd->cursorRow, 0);
	    (void) _DtTermPrimRefreshText(w, 0, tpd->cursorRow, tw->term.columns,
		    tpd->cursorRow);
	} else {
	    /* delete the lines at the cursor...
	     */
	    (void) _DtTermPrimBufferDeleteLine(tpd->termBuffer,
		    tpd->topRow + tpd->cursorRow, count, tpd->lastUsedRow);
	    /* delete lines at the cursor from the selection... */
	    (void) _DtTermPrimSelectDeleteLines(w,
		    tpd->lastUsedHistoryRow + tpd->topRow + tpd->cursorRow,
		    count);

	    /* insert the lines we just deleted at the bottom of the
	     * scrolling region...
	     */
	    (void) _DtTermPrimBufferInsertLineFromTB(tpd->termBuffer,
		    tpd->topRow + tpd->scrollLockBottomRow - count + 1,
		    count, insertFromBottom);
	    /* insert lines at the bottom of the scrolling region into
	     * the selection...
	     */
	    (void) _DtTermPrimSelectInsertLines(w,
		    tpd->lastUsedHistoryRow + tpd->topRow +
		    tpd->scrollLockBottomRow - count + 1, count);

	    /* and scroll the text on the screen... */
            (void) _DtTermPrimScrollTextArea(w, tpd->cursorRow,
		    tpd->scrollLockBottomRow + 1 - tpd->cursorRow, count);
	}
    }

    /* move to the left margin... */
    tpd->cursorColumn = tpd->leftMargin;
}


/** HARD/SOFT RESET ***********************************************************
 *                                       #
 *  #    #    ##    #####   #####       #    ####    ####   ######   #####
 *  #    #   #  #   #    #  #    #     #    #       #    #  #          #
 *  ######  #    #  #    #  #    #    #      ####   #    #  #####      #
 *  #    #  ######  #####   #    #   #           #  #    #  #          #
 *  #    #  #    #  #   #   #    #  #       #    #  #    #  #          #
 *  #    #  #    #  #    #  #####  #         ####    ####   #          #
 * 
 * 
 *  #####   ######   ####   ######   #####
 *  #    #  #       #       #          #
 *  #    #  #####    ####   #####      #
 *  #####   #            #  #          #
 *  #   #   #       #    #  #          #
 *  #    #  ######   ####   ######     #
 */
void
_DtTermFuncHardReset(Widget w, int count, FunctionSource functionSource)
{
    DtTermWidget vtw = (DtTermWidget) w;
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    (void) _DtTermPrimScrollComplete(w, True);
    /* ring the bell... */
    (void) _DtTermPrimBell(w);
    (void) _DtTermPrimCursorOn(w);
     
    _DtTermFuncSoftReset(w, count, functionSource) ;

    /* Do this after soft reset */

    vtw->term.reverseVideo = False;
    tpd->topRow = 0 ;
    tpd->autoLineFeed=False ;
    vtw->term.jumpScroll = True ;
    /* auto key repeat is X server issue - do nothing here */
    _DtTermFuncClearBuffer(w, 0, (FunctionSource) NULL ) ;
    _DtTermFunctionKeyClear(w) ;

    (void) _DtTermPrimSetCursorVisible(w, True);
    (void) _DtTermPrimCursorOn(w);  
    
    return;
}

void
_DtTermFuncSoftReset(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    DtTermWidget vtw = (DtTermWidget) w;
    DtTermData td = vtw->vt.td;

    (void) _DtTermPrimScrollComplete(w, True);
    /* ring the bell... */
    (void) _DtTermPrimBell(w);
    /* the following locks don't make sense in the vt context, but
       I'll initialize them to make sure they're not corrupted in the 
       "prim" layer
    */
    tpd->keyboardLocked.xferPending = False;
    tpd->keyboardLocked.error       = False;
    tpd->keyboardLocked.record      = False;
 
    /* ANSI and vt220 resets  (from  RM) */
    tpd->keyboardLocked.escape  = False;
    _DtTermPrimInsertCharUpdate(w, DtTERM_INSERT_CHAR_OFF);

    /* vt 220 soft resets  (DECRST, DECSTBM, from spec) */
    tpd->autoWrapRight=True;   /* We are NOT following the vt spec here*/
    td->originMode=False;
    td->applicationMode=False;    /* Normal cursor mode */
    td->applicationKPMode=False;  /* Numeric keypad mode */
    td->originMode=False;         
    /* no scroll lock */
    tpd->memoryLockMode=SCROLL_LOCKoff ;
    tpd->scrollLockTopRow = 0;
    tpd->scrollLockBottomRow = tw->term.rows-1;
    /* cursor state stuff for next restore cursor (DECRC) */
    td->saveCursor.cursorRow = 0;
    td->saveCursor.cursorColumn = 0;
    td->saveCursor.enhFieldState = FIELD_UNPROTECT;
    td->saveCursor.enhVideoState = (char)0;
    td->saveCursor.originMode = False;
    td->saveCursor.enhFgColorState = 0;
    td->saveCursor.enhBgColorState = 0;
    td->saveCursor.GL = &td->G0; 
    td->saveCursor.GR = &td->G0;    
    td->saveCursor.G0 = RENDER_FONT_NORMAL;
    td->saveCursor.G1 = RENDER_FONT_LINEDRAW;   
    td->saveCursor.G2 = RENDER_FONT_NORMAL;
    td->saveCursor.G3 = RENDER_FONT_NORMAL;
    td->saveCursor.singleShiftFont = RENDER_FONT_NORMAL; 
    td->saveCursor.singleShiftPending = False;
    /* this is saved by termprim td->saveCursor.wrapMode  = False ; */

    /*
    ** xterm resets (xterm extensions to DECRST
    ** xterm does not reset this stuff so I won't either    
    **   td->allow80_132ColMode = False ;
    **   td->doCursesFixMode = False ; 
    **   td->marginBellMode  = False ; 
    **   tpd->logging = False ; 
    **  when implemented in Term, set "NOT alternate buffer"
    */
    vtw->vt.td->reverseWrapMode = False;  /* this is inconsistant with xterm*/

  
    /* Set the "other" state variables?
     */
    /* video... */
    td->enhVideoState &= ~VIDEO_MASK;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhVideo,
	      td->enhVideoState);

    /* field... */
    td->enhFieldState = FIELD_UNPROTECT;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhField,
	      td->enhFieldState);

    /* foreground... */
    td->enhFgColorState = 0;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhFgColor,
	      td->enhFgColorState);

    /* background... */
    td->enhBgColorState = 0;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhBgColor,
	      td->enhBgColorState);

    /* font... */
    td->GL = &td->G0;
    td->GR = &td->G0;
    td->G0 = FONT_NORMAL;
    td->G1 = FONT_NORMAL;
    td->G2 = FONT_NORMAL;
    td->G3 = FONT_NORMAL;
    td->enhFontState = *td->GL;
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhFont, 
	      td->enhFontState);

    /* reset the parser */
    {
      ParserContext context ;
      context = GetParserContext(w) ;
      context->stateTable =  _DtTermStateStart ;
      _DtTermClearParam(w) ;
    }

    (void) _DtTermPrimSetCursorVisible(w, True);
    (void) _DtTermPrimCursorOn(w);  

    /* flush any pending text that we have queued up to send down the
     * pty...
     */
    (void) _DtTermPrimPendingTextFlush(tpd->pendingWrite);
    return;
}


/*** INSERT CHAR/LINE *********************************************************
 * 
 *     #    #    #   ####   ######  #####    #####
 *     #    ##   #  #       #       #    #     #
 *     #    # #  #   ####   #####   #    #     #
 *     #    #  # #       #  #       #####      #
 *     #    #   ##  #    #  #       #   #      #
 *     #    #    #   ####   ######  #    #     #
 * 
 *                                       #
 *   ####   #    #    ##    #####       #   #          #    #    #  ######
 *  #    #  #    #   #  #   #    #     #    #          #    ##   #  #
 *  #       ######  #    #  #    #    #     #          #    # #  #  #####
 *  #       #    #  ######  #####    #      #          #    #  # #  #
 *  #    #  #    #  #    #  #   #   #       #          #    #   ##  #
 *   ####   #    #  #    #  #    # #        ######     #    #    #  ######
 */

void
_DtTermFuncInsertLine(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;
    int insertedRows = 0;
    int moveCount;
    int src;
    int i;

    /* as with delete line, turn the cursor off now since we will
     * pretty much need to turn it off later to change the cursor
     * column to the left margin...
     */
    (void) _DtTermPrimCursorOff(w);

    /* move the cursor to the left column... */
    tpd->cursorColumn = tpd->leftMargin;

    /* if the cursor is below used memory, we don't need to do anything...
     */
    if (tpd->topRow + tpd->cursorRow >= tpd->lastUsedRow) {
	return;
    }

    /* complete any pending scroll if the scroll area has changed... */
    (void) _DtTermPrimScrollCompleteIfNecessary(w, tpd->cursorRow,
	    tw->term.rows - 1, -count);

    /* first, insert any unused memory (from below lastUsedRow)... */
    if (tpd->lastUsedRow < tpd->bufferRows) {
	/* the most we can insert from the bottom is the number of lines
	 * not yet used...
	 */
	moveCount = tpd->bufferRows - tpd->lastUsedRow;
	if (moveCount > count) {
	    moveCount = count;
	}

	/* insert them...
	 */
	(void) _DtTermPrimBufferInsertLineFromTB(tpd->termBuffer,
		tpd->topRow + tpd->cursorRow,
		moveCount, insertFromBottom);
	/* inform the selection code that the selection has been moved... */
	(void) _DtTermPrimSelectInsertLines(w,
		tpd->lastUsedHistoryRow + tpd->topRow + tpd->cursorRow,
		moveCount);

	/* update screen info and counts... */
	count -= moveCount;
	insertedRows += moveCount;
	tpd->lastUsedRow += moveCount;
    }

    if (count > 0) {
	/* insert used memory...
	 */
	/* first insert as much as we can from above the screen... */
	if (count > tpd->topRow) {
	    moveCount = tpd->topRow;
	} else {
	    moveCount = count;
	}

	if (moveCount > 0) {
	    /* insert them into the buffer before we modify the selecction...
	     */
	    (void) _DtTermPrimBufferInsertLineFromTB(tpd->termBuffer,
		    tpd->topRow + tpd->cursorRow - moveCount,
		    moveCount, insertFromTop);

	    /* delete the lines at the top of the buffer from the selection...
	     */
	    (void) _DtTermPrimSelectDeleteLines(w, tpd->lastUsedHistoryRow,
		    moveCount);

	    /* insert the lines at the bottom of the buffer in the
	     * selection...
	     */
	    (void) _DtTermPrimSelectInsertLines(w,
		    tpd->lastUsedHistoryRow + tpd->topRow + tpd->cursorRow -
		    moveCount, moveCount);

	    /* update screen info and counts... */
	    count -= moveCount;
	    insertedRows += moveCount;
	    tpd->topRow -= moveCount;
	}

	/* now insert whatever else we need from the bottom... */
	if (count > 0) {
	    /* DKS: we need to clip here so we don't self destruct... */
	    moveCount = count;

	    /* insert them... */
	    if ((tpd->cursorRow >= tpd->scrollLockTopRow) &&
		    (tpd->cursorRow <= tpd->scrollLockBottomRow)) {
		/* scroll within the scroll region.  So, take memory
		 * from the bottom of the scroll region instead of the
		 * bottom of memory...
		 */
		/* insert lines into the selection... */
		(void) _DtTermPrimBufferInsertLine(tpd->termBuffer,
			tpd->topRow + tpd->cursorRow, moveCount,
			tpd->scrollLockBottomRow - moveCount + 1);
		/* then adjust the selection... */
		(void) _DtTermPrimSelectInsertLines(w,
			tpd->lastUsedHistoryRow + tpd->topRow + tpd->cursorRow,
			moveCount);
	    } else {
		/* just take the lines from the bottom... */
		/* we don't need to delete lines from the bottom since
		 * that doesn't move the selection...
		 */
		/* insert lines into the selection... */
		(void) _DtTermPrimBufferInsertLineFromTB(tpd->termBuffer,
			tpd->topRow + tpd->cursorRow, moveCount,
			insertFromBottom);
		/* then adjust the selection... */
		(void) _DtTermPrimSelectInsertLines(w,
			tpd->lastUsedHistoryRow + tpd->topRow + tpd->cursorRow,
			moveCount);
	    }

	    /* update screen info and counts... */
	    count -= moveCount;
	    insertedRows += moveCount;
	}
    }

    /* now we need to scroll below us.  Remember, in order to scroll text
     * down, the count needs to be *negative*...
     */
    if ((tpd->cursorRow >= tpd->scrollLockTopRow) &&
	    (tpd->cursorRow <= tpd->scrollLockBottomRow)) {
	(void) _DtTermPrimScrollTextArea(w, tpd->cursorRow,
		tpd->scrollLockBottomRow + 1 - tpd->cursorRow, -insertedRows);
    } else {
	(void) _DtTermPrimScrollTextArea(w, tpd->cursorRow,
		tw->term.rows - tpd->cursorRow, -insertedRows);
    }
}



/*** CURSOR MOTION ************************************************************
 * 
 *   ####   #    #  #####    ####    ####   #####
 *  #    #  #    #  #    #  #       #    #  #    #
 *  #       #    #  #    #   ####   #    #  #    #
 *  #       #    #  #####        #  #    #  #####
 *  #    #  #    #  #   #   #    #  #    #  #   #
 *   ####    ####   #    #   ####    ####   #    #
 * 
 * 
 *  #    #   ####    #####     #     ####   #    #
 *  ##  ##  #    #     #       #    #    #  ##   #
 *  # ## #  #    #     #       #    #    #  # #  #
 *  #    #  #    #     #       #    #    #  #  # #
 *  #    #  #    #     #       #    #    #  #   ##
 *  #    #   ####      #       #     ####   #    #
 */


void
_DtTermFuncTab(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget)w;
    struct termData *tpd = tw->term.tpd;
    TermBuffer tBuffer = tpd->termBuffer;
    short nextTab;

    (void) _DtTermPrimCursorOff(w);

/* Move to the next tab stop.  Note that this cursor motion is
 * similar to the right arrow cursor motion in that
 * it doesn't set the line width until a character is entered.
 */
/* DKS: this code currently enforces tabs at every 8 character
 * positions.  It needs to be worked on to support the margins/tabs
 * buffer...
 */
   nextTab = _DtTermPrimBufferGetNextTab(tBuffer, tpd->cursorColumn);

   /* check to see if we are past the end of the line... */
   if ((nextTab <= 0) || (nextTab >= tpd->rightMargin)) {
       tpd->cursorColumn = tpd->rightMargin;
   } else {
       tpd->cursorColumn = nextTab;
   }
   return;
}



/*  The remainder of this file is new for ANSI and should be merged with 
    the above (some of which may be incorrect for ANSI)
*/

/*** VIDEO ENHANCEMENTS *******************************************************
 *
 *  #    #     #    #####   ######   ####
 *  #    #     #    #    #  #       #    #
 *  #    #     #    #    #  #####   #    #
 *  #    #     #    #    #  #       #    #
 *   #  #      #    #    #  #       #    #
 *    ##       #    #####   ######   ####
 * 
 * 
 *  ######  #    #  #    #    ##    #    #   ####   ######
 *  #       ##   #  #    #   #  #   ##   #  #    #  #
 *  #####   # #  #  ######  #    #  # #  #  #       #####
 *  #       #  # #  #    #  ######  #  # #  #       #        ###
 *  #       #   ##  #    #  #    #  #   ##  #    #  #        ###
 *  ######  #    #  #    #  #    #  #    #   ####   ######   ###
 */

void
_DtTermVideoEnhancement(Widget w,int value)
{
    DtTermPrimitiveWidget      tw        = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd        = tw->term.tpd;
    DtTermWidget      vtw        = (DtTermWidget) w;
    DtTermData        td        = vtw->vt.td;
    ParserContext   context   = GetParserContext(w);

      switch(value) {
        case 0:
	      /* reset video */
              td->enhVideoState &= ~VIDEO_MASK ;

              /* reset color */
	      td->enhFgColorState = 0;
	      (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer, 
		      tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
		      enhFgColor, td->enhFgColorState);
	      td->enhBgColorState = 0;
	      (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer, 
		      tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
		      enhBgColor, td->enhBgColorState);
              break;

        case 1:  /* Bold... */
		 td->enhVideoState |= BOLD;
                 td->enhVideoState &= ~HALF_BRIGHT ;
                 break;
         
        case 2:  /* Half bright... */
		 td->enhVideoState |= HALF_BRIGHT ;
                 td->enhVideoState &= ~BOLD ;
                 break;

        case 4:  /* Underline... */
		 td->enhVideoState |= UNDERLINE;
                 break;
                 
        case 5:  /* we don't do blink, so use inverse */
        case 7:  /* Inverse Video... */
		 td->enhVideoState |= INVERSE ;
                 break;

        case 8:  /* Secure Video... */
		 td->enhVideoState |= SECURE ;
                 break;

        case 22: /* Half bright off... */
		 td->enhVideoState &= ~HALF_BRIGHT ;
		 td->enhVideoState &= ~BOLD ;
                 break;

        case 24: /* Underline off... */
		 td->enhVideoState &= ~UNDERLINE  ;
                 break;

        case 25: /* we don't do blink, so use inverse */

        case 27: /* Inverse Video off... */
		 td->enhVideoState &= ~INVERSE ;
                 break;

        case 28: /* Secure Video off... */
		 td->enhVideoState &= ~SECURE ;
                 break;

	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	      td->enhFgColorState = value - 30 + 1;
	      (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer, 
		      tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
		      enhFgColor, td->enhFgColorState);
	      return;
	      break;
	case 39:
	      td->enhFgColorState = 0;
	      (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer, 
		      tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
		      enhFgColor, td->enhFgColorState);
	      return;
	      break;

	case 40:
	case 41:
	case 42:
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:
	      td->enhBgColorState = value - 40 + 1;
	      (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer, 
		      tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
		      enhBgColor, td->enhBgColorState);
	      return;
	      break;
	case 49:
	      td->enhBgColorState = 0;
	      (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer, 
		      tpd->topRow + tpd->cursorRow, tpd->cursorColumn,
		      enhBgColor, td->enhBgColorState);
	      return;
	      break;

        default: break;
            /* fprintf(stderr,"wrong param in SGR esc seq\n") ;*/
      }

    /* set the enhancement */
    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer, 
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhVideo, 
              td->enhVideoState);
    return;
}

void
_DtTermFontEnhancement(Widget w,int value)
{
    DtTermPrimitiveWidget      tw        = (DtTermPrimitiveWidget) w;
    DtTermPrimData        tpd        = tw->term.tpd;
    DtTermWidget      vtw        = (DtTermWidget) w;
    DtTermData        td        = vtw->vt.td;
    ParserContext   context   = GetParserContext(w);

    (void)_DtTermPrimBufferSetEnhancement(tpd->termBuffer,
              tpd->topRow + tpd->cursorRow, tpd->cursorColumn, enhFont, value);
}

/* This are utility functions for the terminal dialog to use to effect the
   function key lock
*/
void _DtTermSetUserKeyLock(
         Widget         w,
         Boolean        lock_state)
{
  _DtTermWidgetToAppContext(w);

  _DtTermAppLock(app);
  ((DtTermWidget)w)->vt.td->userKeysLocked = lock_state ;
  _DtTermAppUnlock(app);
}

Boolean _DtTermGetUserKeyLock(
         Widget         w)
{
  Boolean lock_state;
  _DtTermWidgetToAppContext(w);

  _DtTermAppLock(app);
  lock_state = ((DtTermWidget)w)->vt.td->userKeysLocked;
  _DtTermAppUnlock(app);
  return(lock_state);
}

void _DtTermSetAutoLineFeed(
         Widget         w,
         Boolean        alf_state)
{
      _DtTermWidgetToAppContext(w);

      _DtTermAppLock(app);
      ((DtTermWidget)w)->vt.td->tpd->autoLineFeed = alf_state ;
      _DtTermAppUnlock(app);
}

Boolean _DtTermGetAutoLineFeed(
         Widget         w)
{
      Boolean alf_state;
      _DtTermWidgetToAppContext(w);

      _DtTermAppLock(app);
      alf_state = ((DtTermWidget)w)->vt.td->tpd->autoLineFeed;
      _DtTermAppUnlock(app);
      return(alf_state);
}

void _DtTermSetCurrentWorkingDirectory(
	Widget		w,
	char		*cwd)
{
    DtTermWidget      vtw        = (DtTermWidget) w;
    _DtTermWidgetToAppContext(w);

    _DtTermAppLock(app);
    vtw->term.subprocessCWD =
	    XtRealloc(vtw->term.subprocessCWD, strlen(cwd) + 1);
    (void) strcpy(vtw->term.subprocessCWD, cwd);
    _DtTermAppUnlock(app);
}

char *_DtTermGetCurrentWorkingDirectory(
	Widget		w)
{
    DtTermWidget      vtw        = (DtTermWidget) w;
    char *cwd;
    _DtTermWidgetToAppContext(w);

    _DtTermAppLock(app);
    cwd = vtw->term.subprocessCWD;
    _DtTermAppUnlock(app);
    return(cwd);
}
