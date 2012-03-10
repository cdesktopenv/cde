/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     WmRequest.c
 **
 **   RCS:	$XConsortium: WmRequest.c /main/4 1995/10/26 15:13:08 rswiston $
 **   Project:  HP DT Workspace Manager
 **
 **   Description: Send a function request to the window manager
 **
 **   (c) Copyright 1992, 1993 by Hewlett-Packard Company
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
 *  _DtWmRequestMultiple (display, root, char *pchRequest, int len)
 *
 *
 *  Description:
 *  -----------
 *  Send one or more function requests to the window manager.
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  root	- root window of screen 
 *  pchRequest  - string request  (possibly multiple)
 *		  (usu. of form "f.<func> [<arg>]")
 *  len		- length of request, counting terminating NULL
 *
 *  Returns:
 *  --------
 *  Success if request sent 
 * 
 *************************************<->***********************************/
Status 
_DtWmRequestMultiple (
	Display *display, 
	Window root, 
	char *pchRequest,
	int len)
{
    Status rval = BadAtom;
    Window wmWindow;

    /*
     * Get the workspace manager window
     */
    if ((rval=_DtGetMwmWindow (display, root, &wmWindow)) == Success)
    {
      
	/*
	 * Make the request by appending the request
	 * to the _DT_WM_REQUEST property
	 */

	rval = XChangeProperty (display, wmWindow, 
	    XmInternAtom(display, _XA_DT_WM_REQUEST, False),
	    XA_STRING, 8, PropModeAppend, 
	    (unsigned char *)pchRequest, 
	    len);

    }

    return (rval);

} /* END OF FUNCTION _DtWmRequestMultiple */

#if 0

/*************************************<->*************************************
 *
 *  DtWmRequest (display, root, char *pchRequest)
 *
 *
 *  Description:
 *  -----------
 *  Send a function request to the window manager.
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  root	- root window of screen 
 *  pchRequest  - NULL terminated string request 
 *		  (usu. of form "f.<func> [<arg>]")
 *
 *  Returns:
 *  --------
 *  Success if request sent 
 * 
 *************************************<->***********************************/
Status 
DtWmRequest (Display *display, Window root, char *pchRequest)
{
    return (_DtWmRequestMultiple (display, root, 
			pchRequest, 1+strlen(pchRequest)));

} /* END OF FUNCTION DtWmRequest */

#endif
