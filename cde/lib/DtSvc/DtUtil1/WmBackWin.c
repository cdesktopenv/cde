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
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     WmBackWin.c
 **
 **   RCS:	$XConsortium: WmBackWin.c /main/4 1995/10/26 15:12:20 rswiston $
 **   Project:  DT Workspace Manager
 **
 **   Description: Identify backdrop windows
 **
 **   (c) Copyright 1993 by Hewlett-Packard Company
 **
 ****************************************************************************
 ************************************<+>*************************************/
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Dt/Wsm.h> 
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>


/********    Public Function Declarations    ********/

extern Window DtWsmGetCurrentBackdropWindow( 
                        Display *display,
                        Window root);
extern Boolean _DtWsmIsBackdropWindow(
        Display *display,
        int screen_num,
        Window window );

/********    End Public Function Declarations    ********/



/*************************************<->*************************************
 *
 *  Window DtWsmGetCurrentBackdropWindow (display, root)
 *
 *
 *  Description:
 *  -----------
 *  Returns the window used for the backdrop for the current workspace
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  root	- root window
 *
 *  Return	- window used for current workspace
 *                None if no backdrop window or error
 *
 *  Comments:
 *  --------
 *  
 * 
 *************************************<->***********************************/
Window
DtWsmGetCurrentBackdropWindow(
        Display *display ,
        Window root )
{
    Window wReturn = None;
    Atom aWS;
    DtWsmWorkspaceInfo *pWsInfo;
    Status status;

    status = DtWsmGetCurrentWorkspace (display, root, &aWS);

    if (status == Success)
    {
        status = DtWsmGetWorkspaceInfo(display, root, aWS, &pWsInfo);
	if (status == Success)
	{
	    if (pWsInfo->numBackdropWindows > 0)
	    {
		/* copy backdrop window (there should be at most one) */
		wReturn = pWsInfo->backdropWindows[0];
	    }

	    DtWsmFreeWorkspaceInfo (pWsInfo);
	}
    }

    return (wReturn);

}  /* END OF FUNCTION DtWsmGetCurrentBackdropWindow */


/*************************************<->*************************************
 *
 *  Boolean _DtWsmIsBackdropWindow (display, screen_num, window)
 *
 *
 *  Description:
 *  -----------
 *  Returns true if the window passed in is a backdrop window.
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  screen_num	- number of screen we're interested in
 *  window	- window we want to test
 *
 *  Outputs:
 *  -------
 *  Return	- True if window is a backdrop window
 *                False otherwise.
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
Boolean 
_DtWsmIsBackdropWindow(
        Display *display,
        int screen_num,
        Window window )
{
    Boolean rval = False;
    Status status;
    Atom *pWorkspaceList;
    int ix, iw, numWorkspaces;
    DtWsmWorkspaceInfo *pWsInfo;
    Window root;

    root = XRootWindow (display, screen_num);

    status = DtWsmGetWorkspaceList (display, root, 
			    &pWorkspaceList, &numWorkspaces);

    if ((status == Success) && 
	(numWorkspaces > 0) &&
	(pWsInfo = (DtWsmWorkspaceInfo *) 
	    malloc (numWorkspaces * sizeof(DtWsmWorkspaceInfo))))
    {
	for (ix=0;
		(!rval) && (ix < numWorkspaces) && (status == Success); 
		    ix++)
	{
	    status = DtWsmGetWorkspaceInfo (display,
					 root,
					 pWorkspaceList[ix],
					 &pWsInfo);
	    if (status == Success)
	    {
		for (iw = 0; iw < pWsInfo->numBackdropWindows; iw++)
		{
		    if (pWsInfo->backdropWindows[iw] == window)
		    {
			rval = True;
		    }
		}

		DtWsmFreeWorkspaceInfo(pWsInfo);
	    }
	}
	XFree ((char *)pWorkspaceList);
    }

    return (rval);
}  /* END OF FUNCTION _DtWsmIsBackdropWindow */
