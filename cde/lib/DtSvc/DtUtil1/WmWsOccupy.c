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
/* $XConsortium: WmWsOccupy.c /main/6 1996/06/21 17:24:10 ageorge $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     WmWsOccupy.c
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Get/Set workspace occupancy of a window
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
#include "DtSvcLock.h"

/********    Public Function Declarations    ********/

extern int DtGetWorkspacesOccupied( 
                        Display *display,
                        Window window,
                        Atom **ppaWs,
                        unsigned long *pNumWs) ;


extern void DtWsmSetWorkspacesOccupied( 
                        Display *display,
                        Window window,
                        Atom *pWsHints,
                        unsigned long numHints) ;

/********    End Public Function Declarations    ********/


/********    Static Function Declarations    ********/

static int _GetWorkspacesOccupied( 
                        Display *display,
                        Window window,
                        Atom **ppaWs,
                        unsigned long *pNumWs,
                        Atom property) ;

/********    End Static Function Declarations    ********/



/*************************************<->*************************************
 *
 *  int _GetWorkspacesOccupied (display, window, ppaWs, 
 *                                  pNumWs, property)
 *
 *
 *  Description:
 *  -----------
 *  Get the contents of a property on a window that is a list of atoms
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  window		- window to get hints from
 *  ppaWs	- pointer to a pointer to return
 *  pNumWs	- pointer to a number to return
 *  property		- the property atom
 *
 *  Outputs:
 *  --------
 *  *ppaWs	- points to the list of workspace info structures
 *                 	  (NOTE: This should be freed using XFree)
 *  *pNumWs  	- the number of workspace info structure in the list
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  ---------
 *  Assumes that less than BUFSIZ bytes will be returned. This code
 *  won't work for very large amounts of info (lots of workspaces).
 * 
 *************************************<->***********************************/
static int 
_GetWorkspacesOccupied(
        Display *display,
        Window window,
        Atom **ppaWs,
        unsigned long *pNumWs,
        Atom property )
{
    Atom actualType;
    int actualFormat;
    unsigned long leftover;
    int rcode;

    *ppaWs = NULL;
    if ((rcode=XGetWindowProperty(display,window,
			 property,0L, (long)BUFSIZ,
			 False,property,
			 &actualType,&actualFormat,
			 pNumWs,&leftover,(unsigned char **)ppaWs))==Success)
    {

        if (actualType != property)
	{
	    /* wrong type, force failure */
	    *pNumWs = 0;
	    rcode = BadValue;
	    if (actualType != None)
	    {
		XFree ((char *)*ppaWs);
	    }
	}
    }
	
    return(rcode);

} /* END OF FUNCTION GetWorkspacesOccupied */


/*************************************<->*************************************
 *
 *  int DtWsmGetWorkspacesOccupied (display, window, ppaWs, pNumWs)
 *
 *
 *  Description:
 *  -----------
 *  Get the list of workspaces that this window is in.
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  window	- window to get info from
 *  ppaWs	- pointer to an atom pointer (to be returned)
 *  pNumWs	- pointer to a number (to be returned)
 *
 *  Outputs:
 *  -------
 *  *ppaWs	- pointer to a list of workspace atoms
 *             	  (NOTE: This should be freed using XFree)
 *  *pNumWs	- number of workspace in the list
 *  Return	- Success if something returned
 *		  not Success otherwise.
 * 
 *  Comments:
 *  --------
 *  Use XFree to free the returned data.
 * 
 *************************************<->***********************************/
int 
DtWsmGetWorkspacesOccupied(
        Display *display,
        Window window,
        Atom **ppaWs,
        unsigned long *pNumWs)
{
    int rcode;
    _DtSvcDisplayToAppContext(display);

    _DtSvcAppLock(app);
    rcode =  _GetWorkspacesOccupied (display, window, ppaWs, 
			pNumWs, 
			XmInternAtom(display, _XA_DT_WORKSPACE_PRESENCE,
			    False));

    _DtSvcAppUnlock(app);
    return (rcode);
}

/*************************************<->*************************************
 *
 *  DtWsmSetWorkspacesOccupied (display, window, pWsHints, numHints)
 *
 *
 *  Description:
 *  -----------
 *  Set the set of workspaces to be occupied by this client.
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  window	- window to get hints from
 *  pWsHints	- pointer to a list of workspace atoms
 *  numHints	- number of atoms in list
 *
 *  Comments:
 *  ---------
 *  No error checking
 * 
 *************************************<->***********************************/
void 
DtWsmSetWorkspacesOccupied(
        Display *display,
        Window window,
        Atom *pWsHints,
        unsigned long numHints )
{
    DtWorkspaceHints wsh;
    _DtSvcDisplayToAppContext(display);

    _DtSvcAppLock(app);

    wsh.flags = DT_WORKSPACE_HINTS_WORKSPACES;
    wsh.pWorkspaces = pWsHints;
    wsh.numWorkspaces = numHints;

    _DtWsmSetWorkspaceHints(display, window, &wsh);
    _DtSvcAppUnlock(app);
}


/*************************************<->*************************************
 *
 *  DtWsmOccupyAllWorkspaces (display, window)
 *
 *
 *  Description:
 *  -----------
 *  Occupy all the workspaces on this screen (including new ones
 *  as they are created)
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  window	- window to get hints from
 *
 *  Comments:
 *  ---------
 * 
 *************************************<->***********************************/
void 
DtWsmOccupyAllWorkspaces(
        Display *display,
        Window window)
{
    DtWorkspaceHints wsh;
    _DtSvcDisplayToAppContext(display);

    _DtSvcAppLock(app);

    wsh.flags = DT_WORKSPACE_HINTS_WSFLAGS;
    wsh.wsflags = DT_WORKSPACE_FLAGS_OCCUPY_ALL;

    _DtWsmSetWorkspaceHints(display, window, &wsh);
    _DtSvcAppUnlock(app);
}
