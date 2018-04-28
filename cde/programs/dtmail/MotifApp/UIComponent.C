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
/* $TOG: UIComponent.C /main/9 1998/07/23 17:57:36 mgreess $ */
/*
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//         This example code is from the book:
//
//           Object-Oriented Programming with C++ and OSF/Motif
//         by
//           Douglas Young
//           Prentice Hall, 1992
//           ISBN 0-13-630252-1	
//
//         Copyright 1991 by Prentice Hall
//         All Rights Reserved
//
//  Permission to use, copy, modify, and distribute this software for 
//  any purpose except publication and without fee is hereby granted, provided 
//  that the above copyright notice appear in all copies of the software.
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// UIComponent.C: Base class for all C++/Motif UI components
///////////////////////////////////////////////////////////////
#include <Dt/Wsm.h>
#include "UIComponent.h"
#include <assert.h>
#include <stdio.h>

UIComponent::UIComponent ( const char *name ) : BasicComponent ( name )
{
    // Empty
    _numPendingTasks = 0;
}


void
UIComponent::widgetDestroyedCallback( Widget, 
				      XtPointer clientData, 
				      XtPointer )
{
    UIComponent * obj = (UIComponent *) clientData;	
    
    obj->widgetDestroyed();
}

void
UIComponent::widgetDestroyed()
{
    _w = NULL;
}

void
UIComponent::installDestroyHandler()
{
    assert ( _w != NULL );
    XtAddCallback ( _w, 
		   XmNdestroyCallback,
		   &UIComponent::widgetDestroyedCallback, 
		   (XtPointer) this );
}

void
UIComponent::manage()
{
    assert ( _w != NULL );
    assert ( XtHasCallbacks ( _w, XmNdestroyCallback ) ==
	    XtCallbackHasSome );
    XtManageChild ( _w );
}

void
UIComponent::displayInCurrentWorkspace()
{
    Widget w = baseWidget();
    while (w && !XtIsShell(w)) w = XtParent(w);
    if (w && XtIsShell(w)) displayInCurrentWorkspace(w);
    manage();
}

void
UIComponent::displayInCurrentWorkspace(Widget shell)
{
    while (shell && !XtIsShell(shell)) shell = XtParent(shell);

    // Make sure the shell is popped up and occupying the current workspace.
    if (NULL != shell && XtIsShell(shell))
    {
	Atom     pCurrent;
	Display	*display = XtDisplay(shell);
	Window	window = XtWindow(shell);

        XtVaSetValues(shell, XmNiconic, False, NULL);
	XRaiseWindow(display, window);

	/* Get the current Workspace */
	if (Success == DtWsmGetCurrentWorkspace(
					display,
					XRootWindowOfScreen(XtScreen(shell)),
					&pCurrent))
	{
	    Atom *ws = NULL;
	    unsigned long num = 0;
	    int k;

	    if (Success==DtWsmGetWorkspacesOccupied(display, window, &ws, &num))
	    {
		/* Already in this workspace? */
		for (k = 0; k < num; k++)
		  if (ws[k] == pCurrent) break;

		/* Add to the workspace */
		if (k >= num)
		{
		    size_t nbytes = sizeof(Atom) * (num+1);
		    ws = (Atom*) XtRealloc((char*) ws, nbytes);
		    ws[num] = pCurrent;
		    DtWsmSetWorkspacesOccupied(display, window, ws, num + 1);
		}

		XFree((char *)ws);
	    }
	    else
	      /* Change the hints to reflect the current workspace */
	      DtWsmSetWorkspacesOccupied(display, window, &pCurrent, 1);
	}
    }
}

UIComponent::~UIComponent()
{
    // Make sure the widget hasn't already been destroyed
    
    if ( _w ) 
    {
	// Remove destroy callback so Xt can't call the callback
	// with a pointer to an object that has already been freed
	
	XtRemoveCallback ( _w, 
			  XmNdestroyCallback,
			  &UIComponent::widgetDestroyedCallback,
			  (XtPointer) this );	
    }
}

void
UIComponent::getResources( const XtResourceList resources, 
			   const int numResources )
{
    // Check for errors
    
    assert ( _w != NULL );
    assert ( resources != NULL );
    
    // Retrieve the requested resources relative to the 
    // parent of this object's base widget
    // Added support for doing getResources on the Application
    
    if ( XtParent( _w ) ) 
	XtGetSubresources ( XtParent( _w ), 
			    (XtPointer) this, 
			    _name,
			    className(),
			    resources, 
			    numResources,
			    NULL, 
			    0 );
    else 
	XtGetSubresources ( _w , 
			    (XtPointer) this, 
			    _name,
			    className(),
			    resources, 
			    numResources,
			    NULL, 
			    0 );
}


#ifdef DEAD_WOOD
void
UIComponent::setDefaultResources( const Widget w, 
				  const String *resourceSpec )
{
    int         i;	
    Display    *dpy = XtDisplay ( w );	// Retrieve the display pointer
    XrmDatabase rdb = NULL;		// A resource data base
    
    // Create an empty resource database

    rdb = XrmGetStringDatabase ( "" );

    // Add the Component resources, prepending the name of the component

    i = 0;
    while ( resourceSpec[i] != NULL )
	{
	    char *buf = new char[1000];
	    sprintf(buf, "*%s%s", _name, resourceSpec[i++]);
	    XrmPutLineResource( &rdb, buf );
	    delete [] buf;
	}

    // Merge them into the Xt database, with lowest precendence
    
    if ( rdb )
	{
	    XrmDatabase db = XtDatabase(dpy);
	    XrmCombineDatabase(rdb, &db, FALSE);
	}
}
#endif /* DEAD_WOOD */

#ifndef CAN_INLINE_VIRTUALS
const char *const
UIComponent::className(void)
{
    return "UIComponent";
}
#endif /* ! CAN_INLINE_VIRTUALS */
