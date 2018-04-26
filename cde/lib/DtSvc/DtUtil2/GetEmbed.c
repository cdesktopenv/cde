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
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:     GetEmbed.c
 **
 **   RCS:	$XConsortium: GetEmbed.c /main/5 1996/05/20 16:09:08 drk $
 **   Project:  DT Workspace Manager
 **
 **   Description: Get workspace embedded clients property.
 **
 **   (c) Copyright 1990, 1993 by Hewlett-Packard Company
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
 *  int _DtGetEmbeddedClients (display, root, ppEmbeddedClients, 
 *					pNumEmbeddedClients)
 *
 *
 *  Description:
 *  -----------
 *  Get the contents of the _DT_WORKSPACE_EMBEDDED_CLIENTS property 
 *  from a root window. This is a array of top-level windows that are
 *  embedded in the front panel of the window manager. They would
 *  not be picked up ordinarily by a session manager in a normal
 *  search for top-level windows because they are reparented to 
 *  the front panel which itself is a top-level window.
 *
 *
 *  Inputs:
 *  ------
 *  display		- display 
 *  root		- root window to get info from
 *  ppEmbeddedClients	- pointer to a pointer (to be returned)
 *  pNumEmbeddedClients	- pointer to a number (to be returned)
 *
 *  Outputs:
 *  -------
 *  *ppEmbeddedClients	- pointer to a array of window IDs (top-level
 *			  windows for embedded clients)
 *                 	  (NOTE: This should be freed using XFree)
 *  *pNumEmbeddedClients- number of window IDs in array
 *  Return		- Success if property fetched ok.
 *		  	  Failure returns are from XGetWindowProperty
 * 
 *  Comments:
 *  --------
 *  Use XFree to free the returned data.
 * 
 *************************************<->***********************************/
int 
_DtGetEmbeddedClients(
        Display *display,
        Window root,
        Atom **ppEmbeddedClients,
        unsigned long *pNumEmbeddedClients )
{
    Atom actualType;
    int actualFormat;
    unsigned long leftover;
    int rcode;
    Atom property;

    *ppEmbeddedClients = NULL;
    property = XmInternAtom (display,
			    _XA_DT_WORKSPACE_EMBEDDED_CLIENTS, False);

    if ((rcode=XGetWindowProperty(display,
    			 root,
			 property,
			 0L, 
			 (long)BUFSIZ,
			 False,
			 property,
			 &actualType,
			 &actualFormat,
			 pNumEmbeddedClients,
			 &leftover,
			 (unsigned char **)ppEmbeddedClients))==Success)
    {

        if (actualType != property)
	{
	    /* wrong type, force failure */
	    *pNumEmbeddedClients = 0;
	    rcode = BadValue;
	    if (actualType != None)
	    {
		XFree ((char *) *ppEmbeddedClients);
	    }
	}
    }
	
    return(rcode);
}
