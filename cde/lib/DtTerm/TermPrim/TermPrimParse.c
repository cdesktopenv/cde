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
#include "TermPrimBuffer.h"
#include "TermPrimParseTable.h"
#include "TermPrimFunction.h"


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
_DtTermPrimParseLF
(
    Widget  w
)
{
    (void) _DtTermPrimFuncLF(w, 1, fromParser);
    return;
}

void
_DtTermPrimParseBackspace(Widget w)
{
    (void) _DtTermPrimFuncBackspace(w, 1, fromParser);
    return;
}

void
_DtTermPrimParseTab(Widget w)
{
    (void) _DtTermPrimFuncTab(w, 1, fromParser);
    return;
}

void
_DtTermPrimParseCR
(
    Widget w
)
{
    (void) _DtTermPrimFuncCR(w, 1, fromParser);
    return;
}


void
_DtTermPrimParseBell(Widget w)
{
    (void) _DtTermPrimBell(w);
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
_DtTermPrimParseCursorRight(Widget w)
{
    (void) _DtTermPrimFuncForwardCharacter(w, 1, fromParser);
    return;
}

void
_DtTermPrimParseCursorLeft(Widget w)
{
    (void) _DtTermPrimFuncBackwardCharacter(w, 1, fromParser);
    return;
}

void
_DtTermPrimParseCursorDown(Widget w)
{
    (void) _DtTermPrimFuncNextLine(w, 1, fromParser);
    return;
}

void
_DtTermPrimParseCursorUp(Widget w)
{
    (void) _DtTermPrimFuncPreviousLine(w, 1, fromParser);
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
_DtTermPrimParseTabSet(Widget w)
{
    (void) _DtTermPrimFuncTabSet(w, 1, fromParser);
    return;
}

void
_DtTermPrimParseTabClear(Widget w)
{
    (void) _DtTermPrimFuncTabClear(w, 1, fromParser);
    return;
}

void
_DtTermPrimParseTabClearAll(Widget w)
{
    (void) _DtTermPrimFuncTabClearAll(w, 1, fromParser);
    return;
}


/*** MARGIN FUNCTION **********************************************************
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
_DtTermPrimParseMarginSetLeft(Widget w)
{
    (void) _DtTermPrimFuncMarginSetLeft(w, 1, fromParser);
    return;
}

void
_DtTermPrimParseMarginSetRight(Widget w)
{
    (void) _DtTermPrimFuncMarginSetRight(w, 1, fromParser);
    return;
}

void
_DtTermPrimParseMarginClear(Widget w)
{
    (void) _DtTermPrimFuncMarginClear(w, 1, fromParser);
    return;
}

/* 
** _DtTermPrimParseWriteChar
** 
** this should never be called
*/
void
_DtTermPrimParseWriteChar
(
    Widget w
)
{
    fprintf(stdout, "Oops, somehow _DtTermPrimParseWriteChar got called.\n");
}
