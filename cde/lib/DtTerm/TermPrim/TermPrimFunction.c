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
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermPrimP.h"
#include "TermPrimData.h"
#include "TermPrimFunction.h"
#include "TermPrimSetPty.h"

/******************************************************************************
 *
 *   ####   #####                   #       ######
 *  #    #  #    #                  #       #
 *  #       #    #   ###            #       #####    ###
 *  #       #####    ###            #       #        ###
 *  #    #  #   #     #             #       #         #
 *   ####   #    #   #              ######  #        #
 * 
 * 
 *  #####    ####                    #####    ##    #####
 *  #    #  #                          #     #  #   #    #
 *  #####    ####    ###               #    #    #  #####
 *  #    #       #   ###               #    ######  #    #
 *  #    #  #    #    #                #    #    #  #    #
 *  #####    ####    #                 #    #    #  #####
 */

void
_DtTermPrimFuncLF(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget          tw = (DtTermPrimitiveWidget)w;
    struct termData    *tpd = tw->term.tpd;
    
    /*
    ** move the insert point...
    */
    if (++tpd->cursorRow >= tw->term.rows)
    {
	/*
        ** scroll one line...
        */
        (void) _DtTermPrimScrollText(w, 1);
        tpd->cursorRow = tw->term.rows - 1;
    }
    (void) _DtTermPrimFillScreenGap(w);
    return;
}

void
_DtTermPrimFuncBackspace(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget          tw = (DtTermPrimitiveWidget)w;
    struct termData    *tpd = tw->term.tpd;

    if (tpd->cursorColumn > 0) {
	(void) tpd->cursorColumn--;
    }
    return;
}

void
_DtTermPrimFuncCR(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget)w;
    struct termData *tpd = tw->term.tpd;

    tpd->cursorColumn = tpd->leftMargin;
    return;
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
_DtTermPrimFuncNextLine(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    /* move down...
     */
    (void) _DtTermPrimCursorOff(w);
    while (count-- > 0) {
	if (tpd->cursorRow < tw->term.rows - 1) {
	    (void) tpd->cursorRow++;
	} else {
	    tpd->cursorRow = 0;
	}
    }
    return;
}


void
_DtTermPrimFuncPreviousLine(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    /* move up...
     */
    (void) _DtTermPrimCursorOff(w);
    while (count-- > 0) {
	if (tpd->cursorRow > 0) {
	    tpd->cursorRow--;
	} else {
	    tpd->cursorRow = tw->term.rows - 1;
	}
    }
    return;
}

void
_DtTermPrimFuncBackwardCharacter(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    /* move left...
     */
    (void) _DtTermPrimCursorOff(w);
    while (count-- > 0) {
	if (tpd->cursorColumn > 0) {
	    (void) tpd->cursorColumn--;
	} else {
	    /* past left edge -- wrap up... */
	    tpd->cursorColumn = tw->term.columns - 1;
	    if (tpd->cursorRow > 0) {
		(void) tpd->cursorRow--;
	    } else {
		/* past first line -- wrap to bottom of screen... */
		tpd->cursorRow = tw->term.rows - 1;
	    }
	}
    }
    return;
}

void
_DtTermPrimFuncForwardCharacter(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    /* move right...
     */
    (void) _DtTermPrimCursorOff(w);
    while (count-- > 0) {
	if (tpd->cursorColumn < tw->term.columns - 1) {
	    (void) tpd->cursorColumn++;
	} else {
	    /* past right edge -- wrap... */
	    tpd->cursorColumn = 0;
	    if (tpd->cursorRow < tw->term.rows - 1) {
		(void) tpd->cursorRow++;
	    } else {
		/* past last line -- wrap to top of screen... */
		tpd->cursorRow = 0;
	    }
	}
    }
    return;
}


void
_DtTermPrimFuncReturn(Widget w, int count, FunctionSource functionSource)
{
    Debug('i', fprintf(stderr,
	    ">>_DtTermPrimFuncReturn: not yet implemented\n"));
}


void
_DtTermPrimFuncTab(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget)w;
    struct termData *tpd = tw->term.tpd;
    TermBuffer tBuffer = tpd->termBuffer;
    short nextTab;

    (void) _DtTermPrimCursorOff(w);

    if (tpd->cursorColumn < tpd->leftMargin) {
	/* move to left margin on current line... */
	tpd->cursorColumn = tpd->leftMargin;
    } else {
	/* Move to the next tab stop.  Note that this cursor motion is
	 * similar to the right arrow cursor motion in that
	 * it doesn't set the line length until a character is entered.
	 */
	/* DKS: this code currently enforces tabs at every 8 character
	 * positions.  It needs to be worked on to support the margins/tabs
	 * buffer...
	 */
	nextTab = _DtTermPrimBufferGetNextTab(tBuffer, tpd->cursorColumn);

	/* check to see if we are past the end of the line... */
	if ((nextTab <= 0) || (nextTab >= tpd->rightMargin)) {
	    /* wrap to next line...
	     */
	    tpd->cursorColumn = tpd->leftMargin;

	    /* check to see if we scrolled off the bottom of the screen... */
	    if (++tpd->cursorRow >= tw->term.rows) {
		/* scroll one line...
		 */
		(void)_DtTermPrimScrollText(w, 1);
		tpd->cursorRow = tw->term.rows - 1;
	    }
	    /* fill any screen gap... */
	    if (tpd->topRow + tpd->cursorRow >= tpd->lastUsedRow) {
		(void) _DtTermPrimFillScreenGap(w);
	    }
	} else {
	    tpd->cursorColumn = nextTab;
	}
    }
    return;
}



/*** TAB FUNCTIONS ************************************************************
 * 
 *   #####    ##    #####
 *     #     #  #   #    #
 *     #    #    #  #####
 *     #    ######  #    #
 *     #    #    #  #    #
 *     #    #    #  #####
 * 
 * 
 *  ######  #    #  #    #   ####    #####     #     ####   #    #   ####
 *  #       #    #  ##   #  #    #     #       #    #    #  ##   #  #
 *  #####   #    #  # #  #  #          #       #    #    #  # #  #   ####
 *  #       #    #  #  # #  #          #       #    #    #  #  # #       #
 *  #       #    #  #   ##  #    #     #       #    #    #  #   ##  #    #
 *  #        ####   #    #   ####      #       #     ####   #    #   ####
 */

void
_DtTermPrimFuncTabSet(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    (void) _DtTermPrimBufferSetTab(tpd->termBuffer, tpd->cursorColumn);
}

void
_DtTermPrimFuncTabClear(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    (void) _DtTermPrimBufferClearTab(tpd->termBuffer, tpd->cursorColumn);
}

void
_DtTermPrimFuncTabClearAll(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    (void) _DtTermPrimBufferClearAllTabs(tpd->termBuffer);
}


/*** REDRAW DISPLAY ***********************************************************
 * 
 *  #    #    ##    #####    ####      #    #    #
 *  ##  ##   #  #   #    #  #    #     #    ##   #
 *  # ## #  #    #  #    #  #          #    # #  #
 *  #    #  ######  #####   #  ###     #    #  # #
 *  #    #  #    #  #   #   #    #     #    #   ##
 *  #    #  #    #  #    #   ####      #    #    #
 * 
 * 
 *  ######  #    #  #    #   ####    #####     #     ####   #    #   ####
 *  #       #    #  ##   #  #    #     #       #    #    #  ##   #  #
 *  #####   #    #  # #  #  #          #       #    #    #  # #  #   ####
 *  #       #    #  #  # #  #          #       #    #    #  #  # #       #
 *  #       #    #  #   ##  #    #     #       #    #    #  #   ##  #    #
 *  #        ####   #    #   ####      #       #     ####   #    #   ####
 */
void
_DtTermPrimFuncMarginSetLeft(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    if (tpd->cursorColumn < tpd->rightMargin) {
	tpd->leftMargin = tpd->cursorColumn;
    } else {
	_DtTermPrimBell(w);
    }
    return;
}

void
_DtTermPrimFuncMarginSetRight(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    if (tpd->cursorColumn > tpd->leftMargin) {
	tpd->rightMargin = tpd->cursorColumn;
    } else {
	_DtTermPrimBell(w);
    }
    return;
}

void
_DtTermPrimFuncMarginClear(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget)w;
    struct termData *tpd = tw->term.tpd;

    tpd->leftMargin = 0;
    tpd->rightMargin = tw->term.columns - 1;
    return;
}


/*** REDRAW DISPLAY ***********************************************************
 * 
 *  #####   ######  #####   #####     ##    #    #
 *  #    #  #       #    #  #    #   #  #   #    #
 *  #    #  #####   #    #  #    #  #    #  #    #
 *  #####   #       #    #  #####   ######  # ## #
 *  #   #   #       #    #  #   #   #    #  ##  ##
 *  #    #  ######  #####   #    #  #    #  #    #
 * 
 * 
 *  #####      #     ####   #####   #         ##     #   #
 *  #    #     #    #       #    #  #        #  #     # #
 *  #    #     #     ####   #    #  #       #    #     #
 *  #    #     #         #  #####   #       ######     #
 *  #    #     #    #    #  #       #       #    #     #
 *  #####      #     ####   #       ######  #    #     #
 */

void
_DtTermPrimFuncRedrawDisplay(Widget w, int count, FunctionSource functionSource)
{
    DtTermPrimitiveWidget tw = (DtTermPrimitiveWidget) w;
    struct termData *tpd = tw->term.tpd;

    (void) XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True);
    return;
}
