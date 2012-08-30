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
/* $TOG: Application.h /main/9 1997/09/05 14:40:30 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 *+SNOTICE
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


////////////////////////////////////////////////////////////
// Application.h: 
////////////////////////////////////////////////////////////
#ifndef APPLICATION_H
#define APPLICATION_H

#include "UIComponent.h"
#include <sys/types.h>
#include <unistd.h>
#include "MainWindow.h"

class Application : public UIComponent {
    
    // Allow main and MainWindow to access protected member functions
    friend int main ( int, char ** );
    friend class MainWindow;

  private:    
    
    // Functions for registering and unregistering toplevel windows
    void registerWindow ( MainWindow * );
    void unregisterWindow ( MainWindow * );

    
  protected:
    
    // Functions to handle Xt interface
    virtual void initialize ( int *, char ** );  
    virtual void handleEvents();

    virtual void open_catalog();  
    inline void extractAndRememberEventTime( XEvent * );

    char        *_applicationClass;
    XtAppContext _appContext;
    static XtResource
		_appResources[];
    char        *_appWorkspaceList;
    int		 _bMenuButton;
    Display     *_display;

    long	_lastInteractiveEventTime;
    gid_t	_originalEgid;	  // startup effective gid
    gid_t	_originalRgid;	  // startup real gid
    int		_shutdownEnabled;
    MainWindow  **_windows;       // top-level windows in the program
    int         _numWindows;

  public:
    
    Application ( char * );
    virtual ~Application();     
    
    // Functions to control session management.
    virtual int  smpSaveSessionGlobal() = 0;
    virtual void smpSaveSessionLocal() = 0;
    virtual void restoreSession() = 0;

    // Functions to control shutdown.
    void	disableShutdown()	{ _shutdownEnabled = 0; }
    void	enableShutdown()	{ _shutdownEnabled = 1; }
    int		isEnabledShutdown()	{ return _shutdownEnabled; }
    virtual void shutdown() = 0;
    
    // Functions to manipulate group execution privileges
#if defined(CSRG_BASED)
    void	disableGroupPrivileges(void) { (void) setegid(_originalRgid); }
    void	enableGroupPrivileges(void)  { (void) setegid(_originalEgid); }
#else
    void	disableGroupPrivileges(void) { (void) setgid(_originalRgid); }
    void	enableGroupPrivileges(void)  { (void) setgid(_originalEgid); }
#endif
    gid_t	originalEgid(void)	{ return _originalEgid; }
    gid_t	originalRgid(void)	{ return _originalRgid; }

    // Functions to manipulate application's top-level windows
    void	iconify();
    void	manage();
    void	unmanage();
    
    // Convenient access functions
    virtual const char *const
		className()		{ return "Application"; }
    Display     *display()		{ return _display; }
    XtAppContext appContext()		{ return _appContext; }
    const char  *applicationClass()	{ return _applicationClass; }
    int		 bMenuButton()		{ return _bMenuButton; }
    char	*getAppWorkspaceList()	{ return _appWorkspaceList; }
    long 	lastInteractiveEventTime(void)
					{ return _lastInteractiveEventTime; }
    int		num_windows()		{ return _numWindows; }
    void	setAppWorkspaceList(char *workspaceList);
};

// Pointer to single global instance

extern Application *theApplication; 


#endif
