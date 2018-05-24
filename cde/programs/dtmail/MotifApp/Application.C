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
/* $TOG: Application.C /main/15 1998/10/01 12:10:26 mgreess $ */
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


////////////////////////////////////////////////////////////
// Application.C: 
////////////////////////////////////////////////////////////
#include "Application.h"
#include "MainWindow.h"
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <nl_types.h>
#include <string.h>

#include "EUSDebug.hh"


XtResource
Application::_appResources[] = {
  {
    "workspaceList", "WorkspaceList", XtRString, sizeof(XtRString),
    XtOffset(Application *, _appWorkspaceList), XtRString, (XtPointer) NULL
  }
};

// XPG3 compatible. NL_CAT_LOCALE is set to 1 (non-zero) in XPG4. Use NL_CAT_LOCALE
// for all catopen() calls. This is also defined in include/DtMail/Common.h for dtmail
// and libDtMail catopen calls, if later on we have a common include file for
// dtmail, libDtMail and MotifApp, we can move this define over there.

#if defined(sun) && (_XOPEN_VERSION == 3)
#undef NL_CAT_LOCALE
#define NL_CAT_LOCALE 0

// If NL_CAT_LOCALE is not defined in other platforms, set it to 0
#elif !defined(NL_CAT_LOCALE)	
#define NL_CAT_LOCALE 0
#endif

#ifdef hpV4
/*
 * Wrapper around catgets -- this makes sure the message string is saved
 * in a safe location; so repeated calls to catgets() do not overwrite
 * the catgets() internal buffer.  This has been a problem on HP systems.
 */
char *catgets_cache2(nl_catd catd, int set, int num, char *dflt)
{

#define MSGS_PER_SET_MAX        12
#define NUM_SETS_MAX            2

  /* array to hold messages from catalog */
  static  char *MsgCat[NUM_SETS_MAX][MSGS_PER_SET_MAX];
  
  /* convert to a zero based index */
  int setIdx = set - 1;
  int numIdx = num - 1;
  
  if ( ! MsgCat[setIdx][numIdx] ) {
    MsgCat[setIdx][numIdx] = strdup( catgets(catd, set, num, dflt));
  }
  
  return MsgCat[setIdx][numIdx];
}

#endif

Application *theApplication = NULL;

nl_catd catd = (nl_catd) -1;    // catgets file descriptor

extern String ApplicationFallbacks[];

Application::Application ( char *appClassName ) : 
                    UIComponent ( appClassName )
{
    // Set the global Application pointer
    DebugPrintf(2, "Application::Application(%p \"%s\")\n", appClassName, appClassName);
    
    theApplication = this;  
    
    // Initialize data members
    
    _display    = NULL;
    _appContext = NULL;
    _bMenuButton = 0;
    _windows    = NULL;
    _numWindows = 0;
    _shutdownEnabled = 1;
    _applicationClass = strdup ( appClassName );
    _appWorkspaceList = NULL;
    _lastInteractiveEventTime = 0;
    _originalEgid = 0;
    _originalRgid = 0;
}

void Application::initialize ( int *argcp, char **argv )
{
    DebugPrintf(2, "Application::initialize(%p %d, %p)\n", argcp, *argcp, argv);

    DebugPrintf(3, "Application::initialize - Initializing privileges.\n");

    // The Solaris sendmail operates differently than the HP/IBM sendmail. 
    // sendmail on Solaris runs as 'root' and so has access permissions 
    // to any file on the system. sendmail on HP/IBM runs as set-group-id 
    // 'mail' and so requires that all mailboxes that it may deliver e-mail 
    // to be writable either by being group mail group writable, or by being 
    // writable by the world. On those platforms, then, dtmail is required 
    // to always run with set-group-id mail otherwise, when mailboxes are 
    // saved, they will loose their group ownership and sendmail will no 
    // onger be able to deliver to those mailboxes.

    // we have to be set-gid to group "mail" when opening and storing
    // folders.  But we don't want to do everything as group mail.
    // here we record our original gid, and set the effective gid
    // back the the real gid.  We'll set it back when we're dealing
    // with folders...
    //
    _originalEgid = getegid();	// remember effective group ID
    _originalRgid = getgid();	// remember real group ID
    disableGroupPrivileges();	// disable group privileges from here on

    DebugPrintf(3, "Application::initialize - Initializing Xt.\n");

    _w = XtOpenApplication (
			&_appContext, 
			_applicationClass, 
			(XrmOptionDescList) NULL, 0, 
			argcp, argv, ApplicationFallbacks,
			sessionShellWidgetClass, (ArgList) NULL, 0 );
    
    // Extract and save a pointer to the X display structure
    DebugPrintf(3, "Application::initialize - Extracting display.\n");
    _display = XtDisplay ( _w );

    // Set virtual BMenu mouse binding
    int numButtons = XGetPointerMapping(_display, (unsigned char *)NULL, 0);
    _bMenuButton = (numButtons < 3) ? Button2 : Button3;
    
    // The Application class is less likely to need to handle
    // "surprise" widget destruction than other classes, but
    // we might as well install a callback to be safe and consistent
    DebugPrintf(3, "Application::initialize - Installing destroy handler.\n");
    installDestroyHandler();
    
    // Center the shell, and make sure it isn't visible
    DebugPrintf(3, "Application::initialize - Setting window size.\n");
    XtVaSetValues ( _w,
		   XmNmappedWhenManaged, FALSE,
		   XmNx, DisplayWidth ( _display, 0 ) / 2,
		   XmNy, DisplayHeight ( _display, 0 ) / 2,
		   XmNwidth,  1,
		   XmNheight, 1,
		   NULL );
    
    // The instance name of this object was set in the UIComponent 
    // constructor, before the name of the program was available
    // Free the old name and reset it to argv[0]
    DebugPrintf(3, "Application::initialize - Deleting name %p\n", _name);
    free(_name);
    _name = strdup ( argv[0] );

    // Force the shell window to exist so dialogs popped up from
    // this shell behave correctly
    DebugPrintf(3, "Application::initialize - Realizing shell window.\n");
    XtRealizeWidget ( _w );
    
    getResources(_appResources, XtNumber(_appResources));

    // Initialize and manage any windows registered
    // with this application.
    
    for ( int i = 0; i < _numWindows; i++ )
    {
	DebugPrintf(3, "Application::initialize - Initializing windows[%d]\n", i);
	_windows[i]->initialize();
	DebugPrintf(3, "Application::initialize - Managing windows[%d]\n", i);
	_windows[i]->manage();
    }
}

// Calling _exit() now to work around a problem with threads
// deadlocking if exit() is called.
// Need to fix the threads deadlocking bug and then replace
// _exit() with exit().

Application::~Application()
{
    //
    // Allocated using strdup, so free using free.
    //
    free((void*) _applicationClass);
    delete []_windows;

#ifdef CDExc21492
  #if defined(__hpux)
    this->BasicComponent::~BasicComponent();
  #else
    BasicComponent::~BasicComponent();
  #endif
#endif

    catclose(catd);

    // In an MT environment, calling exit() causes threads to
    // hang and a deadlock results.
    // Call _exit() instead

    _exit(0);
}

// ApplicationExtractEventTime - extract the time the
// current event happened if it is one we are interested
// in - this is used to delay actions that can lock the application
// while the user is being interactive with the application
//

inline void Application::extractAndRememberEventTime(XEvent *event)
{
  switch (((XAnyEvent *)event)->type)
    {
    case KeyPress:	// press any key on the keyboard
    case ButtonPress:	// press any botton on the screen
    case MotionNotify:	// motion events
      _lastInteractiveEventTime = time((time_t *)0);
      break;
    }
}

void Application::handleEvents()
{
    // Just loop forever
#if 0    
    XtAppMainLoop ( _appContext );
#else
    XEvent	event;

    _lastInteractiveEventTime = time((time_t *)0);
    
    for (;;) {
      XtAppNextEvent( _appContext, &event );
      extractAndRememberEventTime( &event );
      XtDispatchEvent( &event );
    }
#endif
}

void Application::registerWindow ( MainWindow *window )
{
    int i;
    MainWindow **newList;
    
    // Allocate a new list large enough to hold the new
    // object, and copy the contents of the current list 
    // to the new list
    
    newList = new MainWindow*[_numWindows + 1];
    
    for ( i = 0; i < _numWindows; i++ )
	newList[i] = _windows[i];
    
    // Install the new list and add the window to the list
    
    if (_numWindows > 0) delete []_windows;
    _windows =  newList;
    _windows[_numWindows] = window;
    
    _numWindows++;
}

void Application::unregisterWindow ( MainWindow *window )
{
    int i, index;
    
    // If this is the last window bye bye.

    if (isEnabledShutdown() && _numWindows == 1)
    {
	_numWindows--;

	// Call derived class's shutdown method.
	shutdown();
	return;
    }

    // Copy all objects, except the one to be removed, to a new list

    MainWindow **newList = new MainWindow*[_numWindows - 1];
    
    for (i=0, index=0; i<_numWindows; i++)
      if (_windows[i] != window)
	newList[index++] = _windows[i];
    
    delete []_windows;
    _windows = newList;
    _numWindows--;
}

void Application::manage()
{
    // Manage all application windows. This will pop up
    // iconified windows as well.
    
    for ( int i = 0; i < _numWindows; i++ )
	_windows[i]->manage();
}

void Application::unmanage()
{
    // Unmanage all application windows
    
    for ( int i = 0; i < _numWindows; i++ )
	_windows[i]->unmanage();
}

void Application::iconify()
{
    // Iconify all top-level windows.
    
    for ( int i = 0; i < _numWindows; i++ )
	_windows[i]->iconify();
}


void
Application::open_catalog()
{
    // open message catalog file
    catd = catopen("MotifApp", NL_CAT_LOCALE);
}

void
Application::setAppWorkspaceList(char *workspaceList)
{
    // open message catalog file
    if (NULL != _appWorkspaceList)
      free(_appWorkspaceList);

    _appWorkspaceList = strdup(workspaceList);
}
