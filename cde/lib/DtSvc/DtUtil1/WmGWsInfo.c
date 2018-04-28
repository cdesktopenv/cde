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
/* $TOG: WmGWsInfo.c /main/7 1997/11/13 14:03:00 bill $
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
 **   File:     WmGWsInfo.c
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Get Dt Workspace Info
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
 *  int DtWsmGetWorkspaceInfo (display, root, aWS, ppWsInfo)
 *
 *
 *  Description:
 *  -----------
 *  Get the contents of the _DT_WORKSPACE_INFO_<name> property 
 *  for workspace <name>.
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  root	- root window of screen of interest
 *  aWS		- atom for workspace
 *  ppWsInfo	- ptr to WS info buffer ptr (to be returned)
 *
 *  Outputs:
 *  --------
 *  *ppWsInfo	- returned ptr to WS Info. (free with XtFree).
 *
 *
 *  Comments:
 *  ---------
 *  ptr to WorkspaceInfo should be freed by calling
 *  DtWsmFreeWorkspaceInfo.
 * 
 *************************************<->***********************************/
int
DtWsmGetWorkspaceInfo(
        Display *display,
        Window root,
	Atom aWS,
        DtWsmWorkspaceInfo **ppWsInfo)
{
    int rcode;
    Atom aProperty;
    Window wmWindow;
    DtWsmWorkspaceInfo *pWsInfo;
    char *pchName, *pch;
    int  iLen;
    int  i;
    Window *pWin;
    XTextProperty tp;
    char **ppchList;
    int count, item;
    _DtSvcDisplayToAppContext(display);

    _DtSvcAppLock(app);
    /* 
     * Construct atom name
     */
    pchName = (char *) XGetAtomName (display, aWS);
    iLen = strlen(pchName) + strlen (_XA_DT_WORKSPACE_INFO) + 4;

    pch = (char *) XtMalloc (iLen);
    strcpy (pch, _XA_DT_WORKSPACE_INFO);
    strcat (pch, "_");
    strcat (pch, pchName);

    aProperty = XInternAtom (display, pch, FALSE);

    XFree ((char *) pchName);
    XtFree ((char *) pch);
    pch = NULL;

    /* 
     * Get window where property is 
     */
    if ((rcode=_DtGetMwmWindow (display, root, &wmWindow)) == Success)
    {
	if ((rcode=XGetTextProperty(
			display,
			wmWindow,
			&tp,
			aProperty))>=Success)
	{
	    if (rcode=XmbTextPropertyToTextList (
				display,
				&tp,
				&ppchList,
				&count) >= Success)
	    {
		pWsInfo = (DtWsmWorkspaceInfo *)
			XtCalloc(1, sizeof(DtWsmWorkspaceInfo));

		pWsInfo->workspace = aWS;
		item = 0;

		/* title */
		if (item < count)
		{
		    pWsInfo->pchTitle = (char *) 
				XtNewString ((String) ppchList[item]);
		    item++;
		}

		/* pixel set id */
		if (item < count)
		{
		    pWsInfo->colorSetId = atoi (ppchList[item]);
		    item++;
		}

		/* backdrop window (moved to end!) */

		/* backdrop background */
		if (item < count)
		{
		    pWsInfo->bg = (unsigned long) 
			    strtol (ppchList[item], (char **) NULL, 0);
		    item++;
		}

		/* backdrop foreground */
		if (item < count)
		{
		    pWsInfo->fg = (unsigned long) 
			    strtol (ppchList[item], (char **) NULL, 0);
		    item++;
		}

		/* backdrop name (atom) */
		if (item < count)
		{
		    pWsInfo->backdropName = 
			    (Atom) strtol (ppchList[item], (char **) NULL, 0);
		    item++;
		}

		/* number of backdrop windows */
		if (item < count)
		{
		    pWsInfo->numBackdropWindows = 
			    (int) strtol (ppchList[item], (char **) NULL, 0);
		    item++;
		}

		/* list of backdrop windows */
		if (pWsInfo->numBackdropWindows > 0 &&
			(item + pWsInfo->numBackdropWindows) <= count)
		{
		    pWin = (Window *)
			XtMalloc (pWsInfo->numBackdropWindows *
				  sizeof (Window));
		    for (i=0; i<pWsInfo->numBackdropWindows; i++)
		    {
			pWin[i] = (Window) 
			    strtol (ppchList[item], (char **) NULL, 0);
			item++;
		    }
		    pWsInfo->backdropWindows = pWin;
		}
		else
		{
		    /* Bogus backdrop windows info */
		    pWsInfo->numBackdropWindows = 0;
		}

		/* pass back ptr to filled in structure */
		*ppWsInfo = pWsInfo;

		/* free the converted data */
		XFreeStringList (ppchList);
	    }
	    else 
	    {
		/* conversion failed */
		*ppWsInfo = NULL;
	    }
	    /* free the property data */
	    if (tp.value) 
		XFree (tp.value);
	}
    }
	
    if (rcode >= Success) rcode=Success;

    _DtSvcAppUnlock(app);
    return(rcode);

} /* END OF FUNCTION DtWsmGetWorkspaceInfo */



/*************************************<->*************************************
 *
 *  void DtWsmFreeWorkspaceInfo (pWsInfo)
 *
 *
 *  Description:
 *  -----------
 *  Free a WorkspaceInfo buffer
 *
 *
 *  Inputs:
 *  ------
 *  pWsInfo	- ptr to WS info buffer
 *
 *  Outputs:
 *  --------
 *  None
 *
 *  Comments:
 *  ---------
 * 
 *************************************<->***********************************/
void
DtWsmFreeWorkspaceInfo(
        DtWsmWorkspaceInfo *pWsInfo)
{
    if (pWsInfo)
    {
	if (pWsInfo->pchTitle)
	    XtFree (pWsInfo->pchTitle);
        if (pWsInfo->backdropWindows)
	    XtFree ((char *)pWsInfo->backdropWindows);
	XtFree ((char *)pWsInfo);
    }

} /* END OF FUNCTION DtWsmFreeWorkspaceInfo */
