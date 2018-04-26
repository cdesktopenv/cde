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
/* $XConsortium: WmRestart.c /main/5 1996/05/20 16:08:04 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1991,1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     WmRestart.c
 **
 **   Project:  HP DT Workspace Manager
 **
 **   Description: Request restart of the HP DT workspace manager
 **
 ****************************************************************************
 ************************************<+>*************************************/
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <Dt/Wsm.h> 
#include <Dt/WsmP.h> 
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>



/*************************************<->*************************************
 *
 *  _DtWmRestart (display, root)
 *
 *
 *  Description:
 *  -----------
 *  Request the window manager to restart
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  root	- root window of screen 
 *
 *  Returns:
 *  --------
 *  Success if request sent 
 * 
 *************************************<->***********************************/
int 
_DtWmRestart (Display *display, Window root)
{
    int rval = BadAtom;
    Window wmWindow;

    /*
     * Get the workspace manager window
     */
    if ((rval=_DtGetMwmWindow (display, root, &wmWindow)) == Success)
    {
      
	/*
	 * Make the request by appending the restart request
	 * name to the _DT_WM_REQUEST property
	 */

	rval = XChangeProperty (display, wmWindow, 
	    XmInternAtom(display, _XA_DT_WM_REQUEST, False),
	    XA_STRING, 8, PropModeAppend, 
	    (unsigned char *)DTWM_REQ_RESTART, 
	    1+strlen(DTWM_REQ_RESTART));

	XFlush (display);	/* do it now */

    }

    return (rval);

} /* END OF FUNCTION _DtWmRestart */


/*************************************<->*************************************
 *
 *  int _DtWmRestartNoConfirm (display, root)
 *
 *
 *  Description:
 *  -----------
 *  Request the window manager to restart with no confirmation dialog
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  root	- root window of screen 
 *
 *  Returns:
 *  --------
 *  Success if request sent 
 * 
 *************************************<->***********************************/
int
_DtWmRestartNoConfirm (Display *display, Window root)
{
    int rval;
    Window wmWindow;
    char buffer[80];

    /*
     * Make the request by appending the restart request
     * name to the _DT_WM_REQUEST property
     */

    strcpy (&buffer[0], DTWM_REQ_RESTART);
    strcat (&buffer[0], " ");
    strcat (&buffer[0], DTWM_REQP_NO_CONFIRM);

    /*
     * Get the workspace manager window
     */
    if ((rval=_DtGetMwmWindow (display, root, &wmWindow)) == Success)
    {
	/*
	 * Make the request by appending the restart request
	 * name to the _DT_WM_REQUEST property
	 */

	rval = XChangeProperty (display, wmWindow, 
	    XmInternAtom(display, _XA_DT_WM_REQUEST, False),
	    XA_STRING, 8, PropModeAppend, 
	    (unsigned char *)buffer, 
	    1+strlen(buffer));

	XFlush (display); 	/* do it now */
    }

    return (rval);

} /* END OF FUNCTION _DtWmRestartNoConfirm */

