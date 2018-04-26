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
/* $TOG: WmChBackD.c /main/7 1998/07/30 12:11:23 mgreess $
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
 **   File:     WmChBackD.c
 **
 **   Description: Request backdrop change of the workspace manager
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


#define SYS_FILE_SEARCH_PATH "DTICONSEARCHPATH"
    

/*************************************<->*************************************
 *
 *  DtWsmChangeBackdrop (display, root, path, pixmap)
 *
 *
 *  Description:
 *  -----------
 *  Request the CDE workspace manager to change the backdrop
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  root	- root window of screen
 *  path	- file path to bitmap file
 *  pixmap	- pixmap id of backdrop pixmap
 *
 *  Returns:
 *  --------
 *  Success if request sent 
 *
 *  Comments:
 *  ---------
 *  public interface to _DtWsmChangeBackdrop()
 *
 *************************************<->***********************************/
int 
DtWsmChangeBackdrop (
	Display *display, 
	Window root, 
	char *path, 
	Pixmap pixmap)
{
    return _DtWsmChangeBackdrop(display, root, path, pixmap);
}


/*************************************<->*************************************
 *
 *  _DtWsmChangeBackdrop (display, root, path, pixmap)
 *
 *
 *  Description:
 *  -----------
 *  Request the CDE workspace manager to change the backdrop
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  root	- root window of screen
 *  path	- file path to bitmap file
 *  pixmap	- pixmap id of backdrop pixmap
 *
 *  Returns:
 *  --------
 *  Success if request sent 
 *
 *  Comments:
 *  ---------
 * 
 *************************************<->***********************************/
int 
_DtWsmChangeBackdrop (
	Display *display, 
	Window root, 
	char *path, 
	Pixmap pixmap)
{
    int rval = BadAtom;
    Window wmWindow;

    if (!path)
    {
	rval = BadValue;
    }
    else
    /*
     * Get the workspace manager window
     */
    if ((rval=_DtGetMwmWindow (display, root, &wmWindow)) == Success)
    {
	char *pch;
	int len;
      
	/*
	 * Build up the NULL-terminated request string
	 */
	len = strlen (DTWM_REQ_CHANGE_BACKDROP) + 1;
	len += strlen (path) + 1;
	len += 30;  /* fudge for ascii-ized pixmap id */

	pch = (char *) XtMalloc (len * sizeof(char));

	sprintf (pch, "%s %s %lx", DTWM_REQ_CHANGE_BACKDROP, path,
		    pixmap);

	/*
	 * Make the request by appending the string to 
	 * the _DT_WM_REQUEST property
	 */
	rval = XChangeProperty (display, wmWindow, 
	    XmInternAtom(display, _XA_DT_WM_REQUEST, False),
	    XA_STRING, 8, PropModeAppend, 
	    (unsigned char *)pch, 
	    1+strlen(pch));

    }

    return (rval);

} /* END OF FUNCTION _DtWsmChangeBackdrop */


