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
/* $XConsortium: WmGWsList.c /main/6 1996/06/21 17:24:26 ageorge $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1992-1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     WmGetWsLis.c
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Get Dt Workspace List
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
#include <X11/Xatom.h>
#include "DtSvcLock.h"

/*************************************<->*************************************
 *
 *  int DtWsmGetWorkspaceList (display, root, ppWorkspaceList,
 * 							pNumWorkspaces)
 *
 *
 *  Description:
 *  -----------
 *  Get the contents of the _DT_WORKSPACE_LIST property 
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  root	- root window of screen of interest
 *  pNumWorkspaces - the number of workspaces returned
 *  ppWorkspaceList - pointer to a pointer to return
 *
 *  Outputs:
 *  --------
 *  *ppDtWmHints-points to the Workspace List
 *		  (NOTE: This should be freed using XFree)
 *
 *  Comments:
 *  ---------
 * 
 *************************************<->***********************************/
int
DtWsmGetWorkspaceList(
        Display *display,
        Window root,
        Atom **ppWorkspaceList,
	int *pNumWorkspaces)
{
    Atom actualType;
    int actualFormat;
    unsigned long leftover, items, length, oldlength;
    int rcode;
    Atom property;
    Window wmWindow;
    _DtSvcDisplayToAppContext(display);

    _DtSvcAppLock(app);

    if ((rcode=_DtGetMwmWindow (display, root, &wmWindow)) == Success)
    {

	property = XmInternAtom(display, _XA_DT_WORKSPACE_LIST, False); 
	length = BUFSIZ;

	*ppWorkspaceList = NULL;
	if ((rcode=XGetWindowProperty(
			display,
			wmWindow,
			property,
			0L, 				/* offset */
			length,
			False,				/* delete */
			XA_ATOM,			/* req_type */
			&actualType,
			&actualFormat,
			&items,				/* items returned */
			&leftover,
			(unsigned char **)ppWorkspaceList))==Success)
	{

	    if (actualType != XA_ATOM)
	    {
		/* wrong type, force failure */
		rcode = BadValue;
		if (actualType != None)
		{
		    XFree ((char *)*ppWorkspaceList);
		}
		*ppWorkspaceList = NULL;
	    }
	    else
	    {
		*pNumWorkspaces = (int) items;
	    }
	}
    }
	
    _DtSvcAppUnlock(app);
    return(rcode);

} /* END OF FUNCTION DtWsmGetWorkspaceList */

/*************************************<->*************************************
 *
 *  int DtWsmGetCurrentWorkspace (display, root, paWorkspace)
 *
 *
 *  Description:
 *  -----------
 *  Get the contents of the _DT_WORKSPACE_CURRENT property 
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  root	- root window of screen of interest
 *  paWorkspace  - pointer to a Atom of current workspace (returned)
 *
 *  Outputs:
 *  --------
 *  *paWorkspace - atom for current workspace (represents "name"
 *                 of workspace). 
 *
 *  Comments:
 *  ---------
 *  The Atom is copied to the caller's space. Nothing needs to be
 *  freed.
 * 
 *************************************<->***********************************/
int
DtWsmGetCurrentWorkspace (
        Display *display,
        Window root,
        Atom *paWorkspace)
{
    Atom actualType;
    int actualFormat;
    unsigned long leftover, items, length;
    int rcode;
    Atom property;
    Window wmWindow;
    Atom *paTemp;
    _DtSvcDisplayToAppContext(display);

    _DtSvcAppLock(app);
    if ((rcode=_DtGetMwmWindow (display, root, &wmWindow)) == Success)
    {

	property = XmInternAtom(display, _XA_DT_WORKSPACE_CURRENT, False); 
	length = 1;

	if ((rcode=XGetWindowProperty(
			display,
			wmWindow,
			property,
			0L, 				/* offset */
			length,
			False,				/* delete */
			XA_ATOM,			/* req_type */
			&actualType,
			&actualFormat,
			&items,				/* items returned */
			&leftover,
			(unsigned char **)&paTemp))==Success)
	{

	    if (actualType != XA_ATOM)
	    {
		/* wrong type, force failure */
		rcode = BadValue;
		if (actualType != None)
		{
		    XFree ((char *)paTemp);
		}
	    }
	    else
	    {
		*paWorkspace = *paTemp;
		XFree((char *) paTemp);
	    }
	}
    }
	
    _DtSvcAppUnlock(app);
    return(rcode);

} /* END OF FUNCTION DtWsmGetCurrentWorkspace */

