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
 *+SNOTICE
 *
 *	$TOG: UIComponent.h /main/6 1998/01/29 15:18:31 mgreess $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
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
// UIComponent.h: Base class for all C++/Motif UI components
///////////////////////////////////////////////////////////////
#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#ifndef I_HAVE_NO_IDENT
#endif

#include <Xm/Xm.h>
#include "BasicComponent.h"

class UIComponent : public BasicComponent {
    
  private:

    // Interface between XmNdestroyCallback and this class
    
    static void widgetDestroyedCallback ( Widget, 
					 XtPointer, 
					 XtPointer );
    
  protected:
    
    //
    // Number of pending tasks.
    // UIComponents with pending tasks should not be terminated.
    // Used in Application and MainWindow classes to block termination.
    //
    int			_numPendingTasks;
    
    // Protect constructor to prevent direct instantiation
    UIComponent ( const char * );
    
    void installDestroyHandler(); // Easy hook for derived classes
    
    // Called by widgetDestroyedCallback() if base widget is destroyed
    virtual void widgetDestroyed(); 
    
#ifdef DEAD_WOOD
    // Loads component's default resources into database
    void setDefaultResources ( const Widget , const String *);
#endif /* DEAD_WOOD */
    
    // Retrieve resources for this clsss from the resource manager
    void getResources ( const XtResourceList, const int );
    
  public:
    
    virtual ~UIComponent();
    
    // Manage the entire widget subtree represented
    // by this component. Overrides BasicComponent method
    virtual void manage();
    virtual void displayInCurrentWorkspace();
    virtual void displayInCurrentWorkspace(Widget);
    
    // Public access functions
#ifdef CAN_INLINE_VIRTUALS
    virtual const char *const className() { return "UIComponent"; }
#else
    virtual const char *const className();
#endif
	
    //
    // Functions for registering and unregistering tasks
    // which will block termination.
    //
    void registerPendingTask() { _numPendingTasks++; }
    void unregisterPendingTask() { _numPendingTasks--; }
};

#endif
