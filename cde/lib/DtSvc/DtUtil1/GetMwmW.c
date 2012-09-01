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
/* $XConsortium: GetMwmW.c /main/5 1996/05/20 16:07:08 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990,1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     GetMwmW.c
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Gets the mwm window id.
 **
 ****************************************************************************
 ************************************<+>*************************************/
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xm/MwmUtil.h>
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>



/********    Public Function Declarations    ********/

extern int _DtGetMwmWindow( 
                        Display *display,
                        Window root,
                        Window *pMwmWindow) ;

/********    End Public Function Declarations    ********/

/********    Static Function Declarations    ********/

static int _GetMwmWindow( 
                        Display *display,
                        Window root,
                        Window *pMwmWindow,
                        Atom property) ;

/********    End Static Function Declarations    ********/


/*************************************<->*************************************
 *
 *  int _GetMwmWindow (display, root, pMwmWindow, property)
 *
 *
 *  Description:
 *  -----------
 *  Get the Motif Window manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  root		- root window of screen
 *  pMwmWindow		- pointer to a window (to be returned)
 *  property		- the property atom
 *
 *  Outputs:
 *  --------
 *  *pMwmWindow		- mwm window id, if successful
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if mwm is not managing the screen for the root window
 *  passed in.
 * 
 *************************************<->***********************************/
static int 
_GetMwmWindow(
        Display *display,
        Window root,
        Window *pMwmWindow,
        Atom property )
{
    Atom actualType;
    int actualFormat;
    unsigned long nitems;
    unsigned long leftover;
    PropMotifWmInfo *pWmInfo = NULL;
    int rcode;
    Window wroot, wparent, *pchildren;
    unsigned int nchildren;

    *pMwmWindow = 0;
    if ((rcode=XGetWindowProperty(display,root,
			     property,0L, PROP_MWM_INFO_ELEMENTS,
			     False,property,
			     &actualType,&actualFormat,
			     &nitems,&leftover,(unsigned char **)&pWmInfo))==Success)
    {

        if (actualType != property)
	{
	    /* wrong type, force failure */
	    rcode = BadValue;
	}
	else
	{
	    rcode = BadWindow;	/* assume the worst */

	    /*
	     * The mwm window should be a direct child of root
	     */
	    if (XQueryTree (display, root, &wroot, &wparent,
			    &pchildren, &nchildren))
	    {
		int i;

		for (i = 0; (i < nchildren) && (rcode != Success); i++)
		{
		    if (pchildren[i] == pWmInfo->wmWindow)
		    {
			rcode = Success;
		    }
		}
	    }

	    if (rcode == Success);
	    {
		*pMwmWindow = pWmInfo->wmWindow;
	    }

	    if (pchildren)
	    {
		XFree ((char *)pchildren);
	    }

	}

	if (pWmInfo)
	{
	    XFree ((char *)pWmInfo);
	}
    }
	
    return(rcode);

} /* END OF FUNCTION _GetMwmWindow */


/*************************************<->*************************************
 *
 *  int _DtGetMwmWindow (display, root, pMwmWindow)
 *
 *
 *  Description:
 *  -----------
 *  Get the Motif Window manager window
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  root		- root window of screen
 *  pMwmWindow		- pointer to a window (to be returned)
 *
 *  Outputs:
 *  --------
 *  *pMwmWindow		- mwm window id, if successful
 *  Return		- status from XGetWindowProperty
 *
 *  Comments:
 *  --------
 *  This can fail if mwm is not managing the screen for the root window
 *  passed in.
 * 
 *************************************<->***********************************/
int 
_DtGetMwmWindow(
        Display *display,
        Window root,
        Window *pMwmWindow )
{
    Atom xa_MWM_INFO;

    xa_MWM_INFO = XmInternAtom (display, _XA_MWM_INFO, False);
    return (_GetMwmWindow (display, root, pMwmWindow, xa_MWM_INFO));
}
