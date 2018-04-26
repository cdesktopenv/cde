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
 *	$XConsortium: Dialog.C /main/4 1996/04/21 19:41:15 drk $
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

#include "Dialog.h"
#include "Application.h"
#include "RoamMenuWindow.h"
#include <Xm/DialogS.h>
#include <assert.h>
#include "Help.hh"

// The following header is private to CDE and should NOT be required
// but unfortunately is.
//
extern "C" {
#include <Dt/HourGlass.h>
}


Dialog::Dialog(char *name, RoamMenuWindow *parent) : UIComponent(name)
{
  _parent = parent;
  _workArea = NULL;
    
  _w = XtCreatePopupShell(_name,
			  xmDialogShellWidgetClass,
			  parent->baseWidget(),
			  NULL, 0 );

  XtVaSetValues(_w, XmNdefaultPosition, False, NULL);

  assert( theApplication != NULL );
}

Dialog::Dialog(RoamMenuWindow *parent) : UIComponent("")
{
  _parent = parent;
  _workArea=NULL;
}

Dialog::~Dialog()
{
}

void
Dialog::initialize()
{
    
    

    XtVaSetValues( _w,
		   XmNdefaultPosition, False, NULL );
    
    XtAddCallback( _w,
		   XmNpopupCallback,
		   ( XtCallbackProc ) &Dialog::popupCallback,
		   XtPointer( this ) );
    
    XtAddCallback( _w,
		   XmNpopdownCallback,
		   ( XtCallbackProc ) &Dialog::popdownCallback,
		   XtPointer( this ) );
    
    _workArea = createWorkArea ( _w );  
    assert ( _workArea != NULL );
    printHelpId("_workArea", _workArea);
    /* add help callback */
    // XtAddCallback(_workArea, XmNhelpCallback, HelpCB, helpId);

    if ( !XtIsManaged ( _workArea ) )
	XtManageChild ( _workArea ); 
}


void
Dialog::title(
    char *text
)
{
    XtVaSetValues ( _w, XmNtitle, text, NULL );
}


void
Dialog::popupCallback( Widget ,
		       XtPointer clientData,
		       XmAnyCallbackStruct *
)
{
    Dialog *window=( Dialog * ) clientData;
    
    window->popped_up();
    
}


void
Dialog::popdownCallback( Widget ,		// w
			 XtPointer clientData,
			 XmAnyCallbackStruct *
)
{
    Dialog *window=( Dialog * ) clientData;
    
    window->popped_down();
    
}

void
Dialog::manage()
{
    if ( !XtIsManaged ( _workArea ) )
	XtManageChild ( _workArea ); 
    UIComponent::manage();
}

void
Dialog::busyCursor()
{
    // Do nothing if the widget has not been realized.

    if (XtIsRealized(_w)) {
	_DtTurnOnHourGlass(_w);
    }
}

void
Dialog::normalCursor()
{
    // Do nothing if the widget has not been realized

    if (XtIsRealized(_w)) {
	_DtTurnOffHourGlass(_w);
    }
}
