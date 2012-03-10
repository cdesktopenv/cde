/* $XConsortium: GetVWmHint.c /main/5 1996/05/20 16:07:19 drk $
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
 **   File:     GetVwmHint.c
 **
 **   Project:  DT Workspace Manager
 **
 **   Description: Get Dt Window manager hints
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
 *  int _DtWsmGetDtWmHints (display, window, ppDtWmHints)
 *
 *
 *  Description:
 *  -----------
 *  Get the contents of the _DT_WM_HINTS property on a window
 *
 *
 *  Inputs:
 *  ------
 *  display	- display 
 *  window	- window to get hints from
 *  ppDtWmHints- pointer to a pointer to return
 *
 *  Outputs:
 *  --------
 *  *ppDtWmHints-points to the DtWmHints structure retrieved from
 *		  the window (NOTE: This should be freed using XFree)
 *
 *  Comments:
 *  ---------
 * 
 *************************************<->***********************************/
int
_DtWsmGetDtWmHints(
        Display *display,
        Window window,
        DtWmHints **ppDtWmHints)
{
    Atom actualType;
    int actualFormat;
    unsigned long leftover, items, length;
    int rcode;
    Atom property;
#ifdef HP_VUE
    unsigned long oldlength;
#endif /* HP_VUE */


    property = XmInternAtom(display, _XA_DT_WM_HINTS, False);
    length = sizeof (DtWmHints) / sizeof (long);

    *ppDtWmHints = NULL;
    if ((rcode=XGetWindowProperty(
			display,
			window,
			property,
			0L, 				/* offset */
			length,
			False,				/* delete */
			property,			/* req_type */
			&actualType,
			&actualFormat,
			&items,				/* items returned */
			&leftover,
			(unsigned char **)ppDtWmHints))==Success)
    {

        if ((actualType != property) || (items < length))
	{
	    /* wrong type, force failure */
	    rcode = BadValue;
	    if (actualType != None)
	    {
		XFree ((char *)*ppDtWmHints);
	    }
	    *ppDtWmHints = NULL;
	}
    }
#ifdef HP_VUE
    else {
	/*
	 * Didn't get the Dt hints, try to get the Vue hints.
	 * Not that the Vue hints had the same structure, just
	 * under a different property name.
	 */
	property = XmInternAtom(display, _XA_VUE_WM_HINTS, False);
	/* 
	 * Property previously existed without attachWindow.
	 */
	oldlength = length - (sizeof(Window)/sizeof(long));

	*ppDtWmHints = NULL;
	if ((rcode=XGetWindowProperty(
			display,
			window,
			property,
			0L, 				/* offset */
			length,
			False,				/* delete */
			property,			/* req_type */
			&actualType,
			&actualFormat,
			&items,				/* items returned */
			&leftover,
			(unsigned char **)ppDtWmHints))==Success)
	{

	    if ((actualType != property) ||
		(items < oldlength))
	    {
		/* wrong type, force failure */
		rcode = BadValue;
		if (actualType != None)
		{
		    XFree ((char *)*ppDtWmHints);
		}
		*ppDtWmHints = NULL;
	    }

	    if (*ppDtWmHints && (items < length))
	    {
		DtWmHints *pvh;

		/* assume old property, return full sized
		   property with empty values */
		pvh = (DtWmHints *) malloc (length * sizeof (long));

		pvh->flags = (*ppDtWmHints)->flags;
		pvh->functions = (*ppDtWmHints)->functions;
		pvh->behaviors = (*ppDtWmHints)->behaviors;
		pvh->attachWindow = NULL;

		XFree ((char *) *ppDtWmHints);
		
		*ppDtWmHints = pvh;
	    }
	}
    }
#endif /* HP_VUE */
	
    return(rcode);

} /* END OF FUNCTION _DtWsmGetDtWmHints */

