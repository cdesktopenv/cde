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
/*
 *+SNOTICE
 *
 *	$TOG: DialogShell.C /main/10 1998/02/03 12:10:06 mgreess $
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

#include <assert.h>
#include <X11/Intrinsic.h>
#include <X11/Xmu/Editres.h>
#include <Xm/Protocols.h>
#include <Xm/AtomMgr.h>

#include <Dt/Wsm.h>
#include <Dt/Session.h>
#include <DtMail/IO.hh>

// The following headers are private to CDE and should NOT be required
// but unfortunately are.
//
extern "C" {
#include <Dt/HourGlass.h>
}
#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/IconFile.h>

#include "DialogShell.h"
#include "Application.h"
#include "RoamMenuWindow.h"


DialogShell::DialogShell(char *name, RoamMenuWindow *parent, WidgetClass wc)
: UIComponent(name)
{
    _parent=parent;
    _workArea=NULL;
    _widgetClass=wc;
    
    assert( theApplication != NULL );
}

DialogShell::~DialogShell()
{
    Atom WM_DELETE_WINDOW=XmInternAtom( XtDisplay( _w ),
					"WM_DELETE_WINDOW",
					False );
    XmRemoveWMProtocolCallback( _w,
				WM_DELETE_WINDOW,
				( XtCallbackProc ) quitCallback,
				NULL );

}


void
DialogShell::initialize()
{
    _w = XtVaCreatePopupShell(
			_name, _widgetClass, _parent->baseWidget(),
			XmNdefaultPosition, False,
			NULL, NULL );
#ifdef USE_EDITRES
    XtAddEventHandler(
		_w, (EventMask) 0, True,
		(XtEventHandler) _XEditResCheckMessages, NULL);
#endif

    installDestroyHandler();
    _workArea = createWorkArea ( _w );  
    assert ( _workArea != NULL );

    XtVaSetValues( _w, XmNdefaultPosition, False, NULL );
    XtAddCallback( _w,
		   XmNpopupCallback,
		   ( XtCallbackProc ) &DialogShell::popupCallback,
		   XtPointer( this ) );
    XtAddCallback( _w,
		   XmNpopdownCallback,
		   ( XtCallbackProc ) &DialogShell::popdownCallback,
		   XtPointer( this ) );

    Atom WM_DELETE_WINDOW=XmInternAtom( XtDisplay( _w ),
					"WM_DELETE_WINDOW",
					False );

    XmAddWMProtocolCallback( _w,
			     WM_DELETE_WINDOW,
			     ( XtCallbackProc ) quitCallback,
			     this );
    
//  if (!XtIsManaged(_workArea)) XtManageChild(_workArea); 
}


void
DialogShell::title(
    char *text
)
{
    XtVaSetValues ( _w, XmNtitle, text, NULL );
}


void
DialogShell::popupCallback( Widget ,
		       XtPointer clientData,
		       XmAnyCallbackStruct *
)
{
    DialogShell *obj=( DialogShell * ) clientData;
    obj->popped_up();
    obj->displayInCurrentWorkspace();
}


void
DialogShell::popdownCallback( Widget ,
			 XtPointer clientData,
			 XmAnyCallbackStruct *
)
{
    DialogShell *obj=( DialogShell * ) clientData;
    obj->popped_down();
}

void
DialogShell::manage()
{
    if (NULL == _workArea) return;
    if (!XtIsManaged(_workArea )) XtManageChild(_workArea); 
    UIComponent::manage();
}


void
DialogShell::quitCallback( Widget,
			  XtPointer clientData,
			  XmAnyCallbackStruct *)
{
  DialogShell *dlg = ( DialogShell *) clientData;
  dlg->quit();
}

void
DialogShell::busyCursor()
{
    // Do nothing if the widget has not been realized

    if (XtIsRealized(_w)) {
	_DtTurnOnHourGlass(_w);
    }
}

void
DialogShell::normalCursor()
{
    // Do nothing if the widget has not been realized
    
    if (XtIsRealized ( _w ))
    {
	_DtTurnOffHourGlass(_w);
    }
}
