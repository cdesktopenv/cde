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
/* $TOG: MainWindow.C /main/13 1998/04/06 13:22:40 mgreess $ */
/*
 *+SNOTICE
 *
 *      $TOG: MainWindow.C /main/13 1998/04/06 13:22:40 mgreess $
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
 *	Copyright 1993, 1994, 1995 Sun Microsystems, Inc.  All rights reserved.
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


////////////////////////////////////////////////////////////////////
// MainWindow.C: Support a toplevel window
////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <Xm/Protocols.h>
#include <Xm/AtomMgr.h>
#include <Xm/MainW.h>
#include <Dt/Wsm.h>
#include <Dt/Session.h>
#include <DtMail/IO.hh>
#include <X11/Shell.h>
#include <X11/Xmu/Editres.h>
#include "Application.h"
#include "MainWindow.h"
#include "Help.hh"

// The following headers are private to CDE and should NOT be required
// but unfortunately are.
//
extern "C" {
#include <Dt/HourGlass.h>
}
#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/IconFile.h>

// This is a private CDE function that should be public, but is not,
// and does not even have a prototype in a header. Yes, it is required.
//
extern "C" Pixmap _DtGetMask(Screen * screen, char * image_name);

#if 0
static const char * DefaultIcon = "Dtablnk";
#endif
static const unsigned long FLASH_INTERVAL = 250; // milliseconds

MainWindow::MainWindow( char *name, Boolean allowResize ) : UIComponent ( name )
{
    _workArea = NULL;
    _flashing = 0;
    _icon_invert = NULL;
    _window_invert = NULL;
    _icon = 0;
    _allow_resize = allowResize;

    assert ( theApplication != NULL ); // Application object must exist
    // before any MainWindow object
    theApplication->registerWindow ( this );
}

void
MainWindow::initialize( )
{
    char *appWorkspaces;

    // All toplevel windows in the MotifApp framework are 
    // implemented as a popup shell off the Application's
    // base widget.
    //
    // XmNdeleteResponse is being set to DO_NOTHING so 
    // that the user can Cancel their close request.

    _w = XtVaCreatePopupShell ( _name, 
				topLevelShellWidgetClass,
				theApplication->baseWidget(),
				XmNdeleteResponse, XmDO_NOTHING,
				XmNallowShellResize, _allow_resize,
				NULL, NULL );

#ifdef USE_EDITRES
    XtAddEventHandler(
		_w, (EventMask) 0, True,
		(XtEventHandler) _XEditResCheckMessages, NULL);
#endif

    installDestroyHandler();
    
    // Use a Motif XmMainWindow widget to handle window layout
    
    _main = XtCreateManagedWidget ( "mainWindow", 
				   xmMainWindowWidgetClass,
				   _w, 
				   NULL, 0 );
    printHelpId("_main", _main);
    /* install callback */
    // XtAddCallback(_main, XmNhelpCallback, HelpCB, helpId);
    XtAddCallback(_main, XmNhelpCallback, 
			    HelpCB, (void *)"_HOMETOPIC");
    
    // Called derived class to create the work area
    
    _workArea = createWorkArea ( _main );  
    assert ( _workArea != NULL );
    
    // Designate the _workArea widget as the XmMainWindow
    // widget's XmNworkWindow widget
    
    XtVaSetValues ( _main, 
		   XmNworkWindow, _workArea,
		   NULL );

    Atom WM_DELETE_WINDOW=XmInternAtom( XtDisplay( _w ),
					"WM_DELETE_WINDOW",
					False );

    XmAddWMProtocolCallback( _w,
			     WM_DELETE_WINDOW,
			     ( XtCallbackProc ) quitCallback,
			     this );

#if 0
    // Why are we setting the icon to Dtablnk.  This is simply going to
    // be overriden by some other function setting it to the appropriate 
    // icon.
    setIconName(DefaultIcon);
#endif

    _window_invert = NULL;
    _last_state = 0;
    _flash_owin = (Window) NULL;
    _flash_iwin = (Window) NULL;
    memset((char*) &(this->_window_attributes), 0, sizeof(XWindowAttributes));

    // Manage the work area if the derived class hasn't already.
    
    if ( !XtIsManaged ( _workArea ) )
	XtManageChild ( _workArea ); 

    XtRealizeWidget(_w);
    appWorkspaces = theApplication->getAppWorkspaceList();
    setWorkspacesOccupied(appWorkspaces);
}

MainWindow::~MainWindow( )
{
    // Unregister this window with the Application object

    if (_w) {
	Atom WM_DELETE_WINDOW=XmInternAtom( XtDisplay( _w ),
					    "WM_DELETE_WINDOW",
					    False );
	XmRemoveWMProtocolCallback( _w,
				    WM_DELETE_WINDOW,
				    ( XtCallbackProc ) quitCallback,
				    NULL );

	if (_icon_invert) XFreeGC(XtDisplay(_w), _icon_invert);
	if (_window_invert) XFreeGC(XtDisplay(_w), _window_invert);
	if (_flash_iwin != (Window) NULL)
	  XDestroyWindow( XtDisplay(_w), _flash_iwin );
	if (_flash_owin != (Window) NULL)
	  XDestroyWindow( XtDisplay(_w), _flash_owin );
    }
    
    theApplication->unregisterWindow ( this );
}

void
MainWindow::enableWorkAreaResize()
{
    XtVaSetValues(_workArea, XmNresizePolicy, XmRESIZE_ANY, NULL);
}

void
MainWindow::disableWorkAreaResize()
{
    XtVaSetValues(_workArea, XmNresizePolicy, XmRESIZE_NONE, NULL);
}

void
MainWindow::manage()
{
    assert ( _w != NULL );
    XtPopup ( _w, XtGrabNone );
    
    // Map the window, in case the window is iconified

    if ( XtIsRealized ( _w ) ) 
	XMapRaised ( XtDisplay ( _w ), XtWindow ( _w ) );
}

void
MainWindow::unmanage()
{
    assert ( _w != NULL );
    XtPopdown ( _w );
}

void
MainWindow::iconify()
{
    assert ( _w != NULL );
    
    // Set the widget to have an initial iconic state
    // in case the base widget has not yet been realized
    
    XtVaSetValues ( _w, XmNiconic, TRUE, NULL );
    
    // If the widget has already been realized,
    // iconify the window
    
    if ( XtIsRealized ( _w ) )
	XIconifyWindow ( XtDisplay ( _w ), XtWindow ( _w ), 0 );
}

void
MainWindow::setIconTitle(const char * title)
{
    XtVaSetValues(_w, XmNiconName, title, NULL);
}

void
MainWindow::setIconName(const char * path)
{
    char * icon_filename = XmGetIconFileName(XtScreen(_w),
					     NULL,
					     (char *)path, // Bug!
					     NULL,
					     DtLARGE);

    if (icon_filename == NULL) {
	return;
    }

    Pixel fg = 0, bg = 0;

    getIconColors(fg, bg);

    _icon = XmGetPixmap(XtScreen(_w),
			icon_filename,
			fg, bg);

    Pixmap icon_mask_map = _DtGetMask(XtScreen(_w), icon_filename);

    if (!_icon || !icon_mask_map) {
	return;
    }

    XtVaSetValues(_w,
		  XmNiconPixmap, _icon,
		  XmNiconMask, icon_mask_map,
		  NULL);

    // Build the inverted icon mask for flashing.
    //
    if (_icon_invert) {
	XFreeGC(XtDisplay(_w), _icon_invert);
    }

    XGCValues	gc_vals;

    gc_vals.foreground = bg;
    gc_vals.function = GXxor;
    _icon_invert = XCreateGC(XtDisplay(_w), _icon, GCForeground | GCFunction,
			     &gc_vals);

    XtFree(icon_filename);
}

void
MainWindow::busyCursor()
{
    // Do nothing if the widget has not been realized

    if (XtIsRealized(_w)) {
	_DtTurnOnHourGlass(_w);
    }
}

void
MainWindow::normalCursor()
{
    // Do nothing if the widget has not been realized
    
    if (XtIsRealized ( _w ))
    {
	_DtTurnOffHourGlass(_w);
    }
}

void
MainWindow::setStatus(const char *)
{
    // Noop in our case.
}

void
MainWindow::clearStatus(void)
{
    // Noop in our case.
}

void
MainWindow::title(const char *text )
{
    XtVaSetValues ( _w, XmNtitle, (char *)text, NULL );
}

void
MainWindow::quitCallback( Widget,
			  XtPointer clientData,
			  XmAnyCallbackStruct *)
{
    MainWindow *window=( MainWindow *) clientData;
    window->quit();
}

void
MainWindow::getIconColors(Pixel & fore, Pixel & back)
{
    XtVaGetValues (_w,
		   XmNforeground, &fore,
		   XmNbackground, &back,
		   NULL);
}

struct WM_STATE {
    int		state;
    Window	icon;
};

static int
getWindowState(Widget w)
{
    Atom 	wmStateAtom, actualType;
    int 	actualFormat;
    int 	retval;
    unsigned long nitems, leftover;
    WM_STATE *wmState;
 
    /*  Getting the WM_STATE property to see if iconified or not */
    wmStateAtom = XInternAtom(XtDisplay(w), "WM_STATE", False);
 
    XGetWindowProperty (XtDisplay(w), XtWindow(w), 
			wmStateAtom, 0L,
                        (long)BUFSIZ, False, wmStateAtom, &actualType,
                        &actualFormat, &nitems, &leftover,
                        (unsigned char **) &wmState);

    if (wmState)
	retval = wmState->state;
    else
	retval = 0;

    free((void*) wmState);
    return retval;
}

void
MainWindow::flash(const int count)
{
    XWindowAttributes	window_attributes;

    if (count == 0) return;
    if (_flashing > 0) return;

    if (_window_invert == NULL) {
	// Create a GC to flash the window.
	//
	XGCValues	gc_vals;
	Pixel	fg, bg;
	getIconColors(fg, bg);
	
	gc_vals.foreground = bg;
	gc_vals.function = GXxor;
	_window_invert = XCreateGC(XtDisplay(_w), XtWindow(_w),
				   GCForeground | GCFunction, &gc_vals);
	XSetSubwindowMode(XtDisplay(_w), _window_invert, IncludeInferiors);
    }

    _last_state = getWindowState(_w);

    //
    //  The original method here, to invert the window and timeout
    //  before inverting back to the original (pixels), breaks when
    //  the window is left with pixels XOR'd in the flash ON state.
    //
    //  One quick fix, uses a transparent window (or windows) on top
    //  of the window to be flashed.  The temp window(s) are used to
    //  prevent updates while flash is ON and/or cause a full update
    //  (expose) after each flash.
    //
    //  First, (this part optional) put an "InputOnly" window on top of
    //  the window to be flashed and ignore all events to this window.
    //  This has the effect of preventing user input (events) from
    //  causing application updates to the window.  This temp window
    //  can be left up (with the wait cursor) until flashing is done.  
    //
    //  Next, handle expose events by using a transparent "InputOutput"
    //  window on top of everything only when flash is ON.  This has
    //  the effect of preventing expose events from causing application
    //  updates to the real window when flash is ON.  It especially,
    //  ensures other problems (e.g. updates to the window caused by
    //  other application timeout events and overlapping window pixels
    //  from an expose event) are cleaned up by an expose event when
    //  this temp window is unmapped or destroyed (between each flash).
    //
    
    XGetWindowAttributes(XtDisplay(_w), XtWindow(_w), &window_attributes);

    if ((Window) NULL != _flash_owin &&
	(window_attributes.width != _window_attributes.width ||
	 window_attributes.height != _window_attributes.height ||
	 window_attributes.border_width != _window_attributes.border_width))
    {
        XDestroyWindow( XtDisplay(_w), _flash_iwin );
        XDestroyWindow( XtDisplay(_w), _flash_owin );
        _flash_iwin = (Window) NULL;
        _flash_owin = (Window) NULL;
    }

    if ((Window) NULL == _flash_owin)
    {
        XSetWindowAttributes	sw_attr;

	memcpy((char*) &(this->_window_attributes), 
	       (char*) &window_attributes,
	       sizeof(window_attributes));

        sw_attr.event_mask = 0;
        _flash_iwin = XCreateWindow(
			XtDisplay(_w), XtWindow(_w), 0, 0,
                        _window_attributes.width, _window_attributes.height,
                        _window_attributes.border_width, (int) CopyFromParent,
                        InputOnly, CopyFromParent,
			CWEventMask, &sw_attr );
        XMapWindow( XtDisplay(_w), _flash_iwin );
        _flash_owin = XCreateWindow(
			XtDisplay(_w), XtWindow(_w), 0, 0,
                        _window_attributes.width, _window_attributes.height,
                        _window_attributes.border_width, (int) CopyFromParent,
                        InputOutput, CopyFromParent,
			CWEventMask, &sw_attr );
    }

    _flashing = count * 2;
    XtAppAddTimeOut(
		XtWidgetToApplicationContext(_w),
		FLASH_INTERVAL, flashCallback, this);
}

void
MainWindow::flashCallback(XtPointer client_data, XtIntervalId * interval_id)
{
    MainWindow * mw = (MainWindow *)client_data;

    mw->doFlash(interval_id);
}

void
MainWindow::doFlash(XtIntervalId *)
{
    static int busy_cursor = 0;
    int state = getWindowState(_w);

    // We are going to make things flash an even number of times.
    // to do this, we will lie about the state, and leave it at the
    // old state for one iteration.
    if (state != _last_state && (_flashing % 2) != 0)
      state = _last_state;
 
    if (! busy_cursor) {
        busyCursor();
	busy_cursor = 1;
    }

    if (state == IconicState) {
	Pixmap	image = _icon;

	XFillRectangle(XtDisplay(_w), image, _icon_invert, 0, 0, 48, 48);
	XtVaSetValues(_w, XmNiconPixmap, NULL, NULL);
	XtVaSetValues(_w, XmNiconPixmap, image, NULL);
    }
    else if (state != 0) {

        // Map temp window to prevent expose updates and other
        // application event updates ... when flash is on.
        if ( (_flashing % 2) == 0 )
          XMapWindow( XtDisplay(_w), _flash_owin );

	XFillRectangle(
		XtDisplay(_w), XtWindow(_w),
		_window_invert, 0, 0,
		_window_attributes.width, _window_attributes.height);

        // Remove temp window to update display when flash is off.
        if ( (_flashing % 2) != 0 )
          XUnmapWindow( XtDisplay(_w), _flash_owin );
    }

    _flashing -= 1;

    if (_flashing > 0) {
	XtAppAddTimeOut(
			XtWidgetToApplicationContext(_w),
			FLASH_INTERVAL, flashCallback, this);
	_last_state = state;
    }
    else {
        XUnmapWindow( XtDisplay(_w), _flash_iwin );
        XUnmapWindow( XtDisplay(_w), _flash_owin );
        normalCursor();
	busy_cursor = 0;
    }
}

Boolean
MainWindow::isIconified()
{
 
    Atom 	wmStateAtom, actualType;
    int 	actualFormat;
    unsigned long nitems, leftover;
    WM_STATE *wmState;
    Boolean retval = FALSE;
 
    assert ( _w != NULL );

    /*  Getting the WM_STATE property to see if iconified or not */
    wmStateAtom = XInternAtom(XtDisplay(_w), "WM_STATE", False);
 
    XGetWindowProperty (XtDisplay(_w), XtWindow(_w), 
			wmStateAtom, 0L,
                        (long)BUFSIZ, False, wmStateAtom, &actualType,
                        &actualFormat, &nitems, &leftover,
                        (unsigned char **) &wmState);


    if (wmState && wmState->state == IconicState)
      retval = TRUE;
    
    free((void*) wmState);
    return retval;
}

/************************************************************************
 * MbStrchr -
 ************************************************************************/
char *
MainWindow::MbStrchr(char *str, int ch)
{
    size_t mbCurMax = MB_CUR_MAX;
    wchar_t targetChar, curChar;
    char tmpChar;
    int i, numBytes, byteLen;

    if(mbCurMax <= 1) return strchr(str, ch);

    tmpChar = (char)ch;
    mbtowc(&targetChar, &tmpChar, mbCurMax);
    for(i = 0, numBytes = 0, byteLen = strlen(str); i < byteLen; i += numBytes)
    {
        numBytes = mbtowc(&curChar, &str[i], mbCurMax);
        if(curChar == targetChar) return &str[i];
    }
    return (char *)NULL;
}

void
MainWindow::setWorkspacesOccupied(char *workspaces)
{
    char	*ptr;
    Atom	*workspace_atoms = NULL;
    int		nworkspaces=0;

    if (workspaces)
    {
        do
        {
            ptr = MbStrchr (workspaces, ' ');

            if (ptr != NULL) *ptr = '\0';

            workspace_atoms = (Atom*) XtRealloc(
						(char *) workspace_atoms,
                              			sizeof(Atom)*(nworkspaces+1));
            workspace_atoms[nworkspaces] = XmInternAtom(
						XtDisplay(_w),
						workspaces, True);
            nworkspaces++;

            if (ptr != NULL)
            {
                *ptr = ' ';
                workspaces = ptr + 1;
            }
        } while (ptr != NULL);

        DtWsmSetWorkspacesOccupied(
				XtDisplay(_w), XtWindow (_w), 
				workspace_atoms, nworkspaces);

        XtFree ((char *) workspace_atoms);
        workspace_atoms = NULL;
    }
    else
    {
        Window	rootWindow;
        Atom	pCurrent;
        Screen	*currentScreen;
        int	screen;

        screen = XDefaultScreen(XtDisplay(_w));
        currentScreen = XScreenOfDisplay(XtDisplay(_w), screen);
        rootWindow = RootWindowOfScreen(currentScreen);

        if (DtWsmGetCurrentWorkspace(
				XtDisplay(_w),
				rootWindow,
                                &pCurrent) == Success)
          DtWsmSetWorkspacesOccupied(
				XtDisplay(_w),
				XtWindow(_w), 
				&pCurrent, 1);
    }
}
