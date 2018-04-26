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
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     SetVWmHint.c
 **
 **   RCS:	$XConsortium: SetVWmHint.c /main/4 1995/10/26 15:11:34 rswiston $
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Set Dt Wm hints.
 **
 **   (c) Copyright 1991, 1993, 1994 by Hewlett-Packard Company
 **
 ****************************************************************************
 ************************************<+>*************************************/
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Dt/Wsm.h> 
#include <Dt/WsmP.h> 
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>


/*************************************<->*************************************
 *
 *  _DtWsmSetDtWmHints (display, window, pHints)
 *
 *
 *  Description:
 *  -----------
 *  Set the contents of the _DT_WM_HINTS property on a window
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  window	- window to set hints on
 *  pHints	- pointer the hints to set
 *
 *  Comments:
 *  ---------
 *  No error checking
 * 
 *************************************<->***********************************/
void 
_DtWsmSetDtWmHints(
        Display *display,
        Window window,
        DtWmHints *pHints)
{
    Atom property;
    DtWmHints vh;

    property = XmInternAtom (display, _XA_DT_WM_HINTS, False);

    /*
     * Copy hints to make sure we have one of the right size.
     * This is for backward compatibility.
     */
    vh.flags = pHints->flags;

    if (pHints->flags & DtWM_HINTS_FUNCTIONS) {
	vh.functions = pHints->functions;
    } else {
	vh.functions = 0L;
    }
  
    if (pHints->flags & DtWM_HINTS_BEHAVIORS) {
	vh.behaviors = pHints->behaviors;
    } else {
	vh.behaviors = 0L;
    }

    if (pHints->flags & DtWM_HINTS_ATTACH_WINDOW) {
	vh.attachWindow = pHints->attachWindow;
    } else {
	vh.attachWindow = None;
    }

    XChangeProperty (
		display, 
		window, 
		property,
		property, 
		32, 
		PropModeReplace, 
		(unsigned char *)&vh, 
		(sizeof (DtWmHints)/sizeof (long)));
}
