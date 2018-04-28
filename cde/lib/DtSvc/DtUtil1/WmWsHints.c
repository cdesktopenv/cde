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
/* $XConsortium: WmWsHints.c /main/5 1996/05/20 16:08:33 drk $
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
 **   File:     WmWsHints.c
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Set and Get workspace hints.
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
 *  _DtWsmSetWorkspaceHints (display, window, pWsHints)
 *
 *
 *  Description:
 *  -----------
 *  Set the contents of the _DT_WORKSPACE_HINTS property on a window
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  window	- window to get hints from
 *  pWsHints	- pointer to workspace hints
 *
 *  Comments:
 *  ---------
 *  The internal form of the property is a structure. The structure
 *  must be unwound and turned into a simple array of "long"s before
 *  being written out.
 *
 *  This function currently only deals with version 1 of the property
 *  structure. The passed in pWsHints->version is ignored.
 * 
 *************************************<->***********************************/
void 
_DtWsmSetWorkspaceHints(
        Display *display,
        Window window,
        DtWorkspaceHints *pWsHints)
{
    unsigned int iSizeWsHints;
    Atom property;
    long *pPropValue, *pP;
    int i;

    /*
     * Unwind structure into a simple array of longs.
     *
     * Compute size of property to write (don't count
     * pointer to workspaces, it will be unwound).
     */
    iSizeWsHints = sizeof(DtWorkspaceHints) - sizeof(long);
    if (pWsHints->flags & DT_WORKSPACE_HINTS_WORKSPACES)
    {
	iSizeWsHints += pWsHints->numWorkspaces * sizeof(long);
    }
    pP = pPropValue = (long *) XtMalloc (iSizeWsHints);

    *pP++ = 1;		/* only deals with version 1 !! */
    *pP++ = pWsHints->flags;

    if (pWsHints->flags & DT_WORKSPACE_HINTS_WSFLAGS)
	*pP++ = pWsHints->wsflags;
    else
	*pP++ = 0L;

    if (pWsHints->flags & DT_WORKSPACE_HINTS_WORKSPACES)
    {
	*pP++ = pWsHints->numWorkspaces;
	for (i=0; i<pWsHints->numWorkspaces; i++)
	{
	    *pP++ = pWsHints->pWorkspaces[i];
	}
    }
    else
    {
	*pP++ = 0L;	/* numWorkspaces */
    }

    property  = XmInternAtom (display, _XA_DT_WORKSPACE_HINTS, False);
    XChangeProperty (display, window, property, 
	property, 32, PropModeReplace, 
	(unsigned char *)pPropValue, (iSizeWsHints/sizeof(long)));
}


/*************************************<->*************************************
 *
 *  int _DtWsmGetWorkspaceHints (display, window, ppWsHints)
 *
 *
 *  Description:
 *  -----------
 *  Get the contents of the _DT_WORKSPACE_HINTS property from a window
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  window	- window to get hints from
 *  ppWsHints	- pointer to pointer to workspace hints
 *
 *  Outputs:
 *  *ppWsHints	- allocated workspace hints data.
 *
 *  Comments:
 *  ---------
 *  The internal form of the property is a structure. The property
 *  is read in and packed into data allocated for the structure.
 *  Free the workspace hints by calling _DtWsmFreeWorkspaceHints.
 *
 *  This function currently only deals with version 1 of the property
 *  structure. 
 * 
 *************************************<->***********************************/
int
_DtWsmGetWorkspaceHints(
        Display *display,
        Window window,
        DtWorkspaceHints **ppWsHints)
{
    unsigned int iSizeWsHints;
    Atom property;
    DtWorkspaceHints *pWsH;
    long *pP = NULL;
    long *pProp = NULL;
    int i;
    Atom actualType;
    int actualFormat;
    unsigned long leftover, lcount;
    int rcode;

    property  = XmInternAtom (display, _XA_DT_WORKSPACE_HINTS, False);
    if ((rcode = XGetWindowProperty(
			    display,
			    window,
                            property,
			    0L, 
			    (long)BUFSIZ,
                            False,
			    property,
                            &actualType,
			    &actualFormat,
                            &lcount,
			    &leftover,
                            (unsigned char **)&pProp))==Success)
    {
	if (actualType != property)
        {
            /* wrong type, force failure */
            rcode = BadValue;
        }
	else
	{
	    pP = pProp;
	    pWsH = (DtWorkspaceHints *) 
		XtMalloc (sizeof(DtWorkspaceHints) * sizeof(long));
	    
	    pWsH->version = *pP++;

	    /* 
	     * Only handles version 1 
	     *
	     * (Fudge the test a little so that newer versions
	     *  won't be treated as older versions. This assumes
	     *  that customers will migrate their software after
	     *  a couple of revisions of these hints.)
	     */
	    if (pWsH->version > 5)
	    {
		/* 
		 * Assume old version of hints which was a simple
		 * list of atoms.
		 */
		pWsH->version = 1; 
		pWsH->flags = DT_WORKSPACE_HINTS_WORKSPACES;
		pWsH->wsflags = 0;
		pWsH->numWorkspaces = lcount;

		/* reset pointer to first atom in list */
		pP--;
	    }
	    else 
	    {
		pWsH->flags = *pP++;
		pWsH->wsflags = *pP++;
		pWsH->numWorkspaces = *pP++;
	    }

	    if (pWsH->flags & DT_WORKSPACE_HINTS_WORKSPACES)
	    {
		pWsH->pWorkspaces = (Atom *) 
		    XtMalloc (pWsH->numWorkspaces * sizeof (Atom));
		for (i=0; i<pWsH->numWorkspaces; i++)
		{
		    pWsH->pWorkspaces[i] = *pP++;
		}
	    }
	    else
	    {
		pWsH->pWorkspaces = NULL;
	    }
	    *ppWsHints = pWsH;
	}

	if (pProp && (actualType != None))
	{
	    XFree ((char *)pProp);
	}
    }

    return (rcode);
}


/*************************************<->*************************************
 *
 *  _DtWsmFreeWorkspaceHints (pWsHints)
 *
 *
 *  Description:
 *  -----------
 *  Free  a workspace hints structure returned from _DtWsmGetWorkspaceHints
 *
 *  Inputs:
 *  ------
 *  pWsHints	- pointer to workspace hints
 *
 *  Outputs:
 *
 *  Comments:
 *  ---------
 *************************************<->***********************************/
void 
_DtWsmFreeWorkspaceHints(
        DtWorkspaceHints *pWsHints)
{
    if (pWsHints)
    {
	if (pWsHints->pWorkspaces)
	{
	    XtFree ((char *) pWsHints->pWorkspaces);
	}
	XtFree ((char *)pWsHints);
    }
}
