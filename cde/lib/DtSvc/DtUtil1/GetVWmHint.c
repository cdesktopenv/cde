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
    return(rcode);
} /* END OF FUNCTION _DtWsmGetDtWmHints */

