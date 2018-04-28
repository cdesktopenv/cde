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
/* $XConsortium: DialogManager.C /main/5 1996/04/21 19:32:05 drk $ */
/*
 *+SNOTICE
 *
 *      $XConsortium: DialogManager.C /main/5 1996/04/21 19:32:05 drk $
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


//////////////////////////////////////////////////////////
// DialogManager.C: Support cached dialog widgets
//////////////////////////////////////////////////////////
#include "DialogManager.h"
#include "Application.h"
#include <Xm/MessageB.h>
#include <assert.h>

#include <nl_types.h>
extern nl_catd catd;

#include "NLS.hh"

DialogManager::DialogManager ( char   *name ): UIComponent ( name )
{
    // Empty
}

Widget DialogManager::getDialog()
{
    Widget newDialog = NULL;
    
    // If the permanent widget exists and is not in use,
    // just return it
    
    if ( _w && !XtIsManaged ( _w ) )
	return _w;
    
    // Get a widget from the derived class
    
    newDialog = createDialog ( theApplication->baseWidget() ) ;
    
    // If this is a temporary dialog, install callbacks to 
    // destroy it when the user pops it down.
    
    if ( _w )
    {
	XtAddCallback ( newDialog, 
		       XmNokCallback, 
		       &DialogManager::destroyTmpDialogCallback,
		       (XtPointer) this );
	
	XtAddCallback ( newDialog, 
		       XmNcancelCallback, 
		       &DialogManager::destroyTmpDialogCallback,
		       (XtPointer) this );
    }
    else                 // If this is the first dialog to be 
	_w = newDialog;  // created, save it to be used again.
    
    return newDialog;
}

Widget DialogManager::getDialog(
    Widget w
)
{
    Widget newDialog = NULL;
    
    // If the permanent widget exists and is not in use,
    // just return it
    
    if ( _w && !XtIsManaged ( _w ) )
	return _w;
    
    // Get a widget from the derived class
    // Parent the dialog to the widget passed in
    
    newDialog = createDialog (w);
    
    // If this is a temporary dialog, install callbacks to 
    // destroy it when the user pops it down.
    
    if ( _w )
    {
	XtAddCallback ( newDialog, 
		       XmNokCallback, 
		       &DialogManager::destroyTmpDialogCallback,
		       (XtPointer) this );
	
	XtAddCallback ( newDialog, 
		       XmNcancelCallback, 
		       &DialogManager::destroyTmpDialogCallback,
		       (XtPointer) this );
    }
    else                 // If this is the first dialog to be 
	_w = newDialog;  // created, save it to be used again.
    
    return newDialog;
}

void DialogManager::destroyTmpDialogCallback ( Widget     w, 
					      XtPointer,
					      XtPointer clientData)
{
    XtDestroyWidget ( w );

    // We must set the wiget handle to NULL to prevent multiple
    // destroys.
    ((DialogManager *)clientData)->_w = NULL;
}

Widget DialogManager::post (char	  *title,
			    char          *text,
			    Widget	   wid,
			    void          *clientData,
			    DialogCallback ok,
			    DialogCallback cancel,
			    DialogCallback help)
{
    // Get a dialog widget from the cache
    
    Widget dialog = getDialog(wid);
    
    // Make sure the dialog exists, and that it is an XmMessageBox
    // or subclass, since the callbacks assume this widget type
    
    assert ( dialog != NULL );
//    assert ( XtIsSubclass ( dialog, xmMessageBoxWidgetClass ) );
	
	// Convert the text string to a compound string and 
	// specify this to be the message displayed in the dialog.
	
    XmString titleStr = XmStringCreateLocalized (title);
    XmString xmstr = XmStringCreateLocalized ( text );
    XtVaSetValues ( dialog, 
		    XmNmessageString, xmstr,
		    XmNdialogTitle, titleStr,
		    NULL );
    XmStringFree ( xmstr );
    XmStringFree ( titleStr );
    
    // Create an object to carry the additional data needed
    // to cache the dialogs.
    
    DialogCallbackData *dcb = new DialogCallbackData( this, 
						     clientData,
						     ok, cancel, 
						     help );
    // Install callback function for each button 
    // support by Motif dialogs. If there is no help callback
    // unmanage the corresponding button instead, if possible.

    if ( ok )
      XtAddCallback ( dialog, 
		      XmNokCallback, 
		      &DialogManager::okCallback,
		      (XtPointer) dcb );
    else
      {
	Widget w = XmMessageBoxGetChild ( dialog,
					  XmDIALOG_OK_BUTTON );
        XtUnmanageChild ( w );
      }


    if ( cancel )
      XtAddCallback ( dialog, 
		      XmNcancelCallback, 
		      &DialogManager::cancelCallback,
		      (XtPointer) dcb );
    else
      {
	Widget w = XmMessageBoxGetChild ( dialog,
					  XmDIALOG_CANCEL_BUTTON );
        XtUnmanageChild ( w );
      }
    
    
    if ( help )	    
	XtAddCallback ( dialog, 
		       XmNhelpCallback, 
		       &DialogManager::helpCallback,
		       (XtPointer) dcb );
    else
    {
	Widget w = XmMessageBoxGetChild ( dialog,
					 XmDIALOG_HELP_BUTTON );
        XtUnmanageChild ( w );
    }
    
    // Post the dialog.
    
    XtManageChild ( dialog );

    
    return dialog;
}

Widget DialogManager::post (char	  *title,
			    char          *text,
			    void          *clientData,
			    DialogCallback ok,
			    DialogCallback cancel,
			    DialogCallback help)
{
    // Get a dialog widget from the cache
    
    Widget dialog = getDialog();
    
    // Make sure the dialog exists, and that it is an XmMessageBox
    // or subclass, since the callbacks assume this widget type
    
    assert ( dialog != NULL );
//    assert ( XtIsSubclass ( dialog, xmMessageBoxWidgetClass ) );
	
	// Convert the text string to a compound string and 
	// specify this to be the message displayed in the dialog.
	
    XmString titleStr = XmStringCreateLocalized (title);
    XmString xmstr = XmStringCreateLocalized ( text );
    XtVaSetValues ( dialog, 
		    XmNmessageString, xmstr,
		    XmNdialogTitle, titleStr,
		    NULL );
    XmStringFree ( xmstr );
    XmStringFree ( titleStr );
    
    // Create an object to carry the additional data needed
    // to cache the dialogs.
    
    DialogCallbackData *dcb = new DialogCallbackData( this, 
						     clientData,
						     ok, cancel, 
						     help );
    // Install callback function for each button 
    // support by Motif dialogs. If there is no help callback
    // unmanage the corresponding button instead, if possible.

    if ( ok )
      XtAddCallback ( dialog, 
		      XmNokCallback, 
		      &DialogManager::okCallback,
		      (XtPointer) dcb );
    else
      {
	Widget w = XmMessageBoxGetChild ( dialog,
					  XmDIALOG_OK_BUTTON );
        XtUnmanageChild ( w );
      }


    if ( cancel )
      XtAddCallback ( dialog, 
		      XmNcancelCallback, 
		      &DialogManager::cancelCallback,
		      (XtPointer) dcb );
    else
      {
	Widget w = XmMessageBoxGetChild ( dialog,
					  XmDIALOG_CANCEL_BUTTON );
        XtUnmanageChild ( w );
      }
    
    
    if ( help )	    
	XtAddCallback ( dialog, 
		       XmNhelpCallback, 
		       &DialogManager::helpCallback,
		       (XtPointer) dcb );
    else
    {
	Widget w = XmMessageBoxGetChild ( dialog,
					 XmDIALOG_HELP_BUTTON );
        XtUnmanageChild ( w );
    }
    
    // Post the dialog.
    
    XtManageChild ( dialog );

    
    return dialog;
}

void DialogManager::okCallback ( Widget    w, 
				XtPointer clientData,
				XtPointer )
{
    DialogCallbackData *dcd = (DialogCallbackData *) clientData;
    DialogManager      *obj = (DialogManager *) dcd->dialogManager();
    DialogCallback      callback;
    
    // If caller specified an ok callback, call the function
    
    if ( ( callback = dcd->ok() ) != NULL )
	( *callback )( dcd->clientData() );
    
    // Reset for the next time
    
    obj->cleanup ( w, dcd );
}

void DialogManager::cancelCallback ( Widget    w, 
				    XtPointer clientData,
				    XtPointer )
{
    DialogCallbackData *dcd = (DialogCallbackData *) clientData;
    DialogManager      *obj = (DialogManager *) dcd->dialogManager();
    DialogCallback      callback;
    
    if ( ( callback = dcd->cancel() ) != NULL )
	( *callback )( dcd->clientData() );
    
    obj->cleanup ( w, dcd );
}

void DialogManager::helpCallback ( Widget    w, 
				  XtPointer clientData,
				  XtPointer )
{
    DialogCallbackData *dcd = (DialogCallbackData *) clientData;
    DialogManager      *obj = (DialogManager *) dcd->dialogManager();
    DialogCallback      callback;
    
    if ( ( callback = dcd->help() ) != NULL )
	( *callback )( dcd->clientData() );
    
    obj->cleanup ( w, dcd );
}

void DialogManager::cleanup ( Widget w, DialogCallbackData *dcd )
{
    // Remove all callbacks to avoid having duplicate 
    // callback functions installed.
    
    XtRemoveCallback ( w, 
		      XmNokCallback, 
		      &DialogManager::okCallback,
		      (XtPointer) dcd );
    
    XtRemoveCallback ( w, 
		      XmNcancelCallback, 
		      &DialogManager::cancelCallback,
		      (XtPointer) dcd );
    
    XtRemoveCallback ( w, 
		      XmNhelpCallback, 
		      &DialogManager::helpCallback,
		      (XtPointer) dcd );
    
    // Delete the DialogCallbackData instance for this posting
    
    delete dcd;
}

void
DialogManager::forceUpdate( Widget w )
{
  Widget diashell, topshell;
  Window diawindow, topwindow;

  Display		*dpy;
  XWindowAttributes	xwa;
  XEvent		event;
  if ( !w )
    return;
  XtAppContext cxt=XtWidgetToApplicationContext( w );
  for (diashell=w;!XtIsShell(diashell);diashell=XtParent(diashell));
  for ( topshell=diashell;XtIsTopLevelShell( topshell );
	topshell = XtParent( topshell ) );

//  if (XtIsRealized(diashell) && XtIsRealized(topshell)){
    dpy=XtDisplay(diashell);
    diawindow=XtWindow(diashell);
    topwindow=XtWindow(topshell);
    while ( XGetWindowAttributes(dpy,diawindow,&xwa) && 
	    xwa.map_state != IsViewable && XEventsQueued(dpy,QueuedAlready)){
//	if ( XGetWindowAttributes( dpy, topwindow, &xwa ) &&
//	   xwa.map_state != IsViewable )
//	  break;
      XtAppNextEvent( cxt, &event );
      XtDispatchEvent( &event );
    }
//  }
  XmUpdateDisplay(topshell);
}



// Added this extra functionality

void
my_okCallback( int *data )
{
    *data=1;
}

void
my_cancelCallback( int *data )
{
    *data=2;
}

int
DialogManager::post_and_return(
	char *title_str, 
	char *text_str,
	Widget wid
)
{
    int answer = 0;
    XmString okLabel, cancelLabel;

    // They may have been set via the overloaded post_and_return()
    // method before. Reset them to their default values...

    okLabel = XmStringCreateLocalized(GETMSG(catd, 1, 2, "OK"));
    cancelLabel = XmStringCreateLocalized(GETMSG(catd, 1, 3, "Cancel"));

    Widget dlg = this->getDialog(wid);

    // Make sure the dialog exists, and that it is an XmMessageBox
    // or subclass, since the callbacks assume this widget type
    
    assert ( dlg != NULL );

    XtVaSetValues(dlg,
		  XmNokLabelString, okLabel,
		  XmNcancelLabelString, cancelLabel,
		  NULL);

    Widget dialog =
	this->post(title_str,
		   text_str,
		   wid,
		   (void *) &answer,
		   ( DialogCallback ) &my_okCallback,
		   ( DialogCallback ) &my_cancelCallback);

    forceUpdate( dialog );
    while ( answer==0 ) 
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog), XtIMAll );

    // Process just one more event to pop down dialog.
    XtAppProcessEvent(XtWidgetToApplicationContext(dialog), XtIMAll );

    return(answer);

}

int
DialogManager::post_and_return(
	char *title_str, 
	char *text_str,
	char *okLabelString,
        Widget wid
)
{
    int answer = 0;
    XmString okLabel;

    okLabel = XmStringCreateLocalized(okLabelString);

    Widget dlg = this->getDialog(wid);

    // Make sure the dialog exists, and that it is an XmMessageBox
    // or subclass, since the callbacks assume this widget type
    
    assert ( dlg != NULL );

    XtVaSetValues(dlg,
		  XmNokLabelString, okLabel,
		  NULL);

    Widget dialog = this->post(title_str,
			       text_str,
			       wid,
			       (void *) &answer,
			       ( DialogCallback ) &my_okCallback);

    forceUpdate( dialog );
    while ( answer==0 ) 
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog), XtIMAll );

    // Process just one more event to pop down dialog.
    XtAppProcessEvent(XtWidgetToApplicationContext(dialog), XtIMAll );

    return(answer);

}

int
DialogManager::post_and_return(
	char *title_str, 
	char *text_str,
	char *okLabelString,
	char *cancelLabelString,
        Widget wid

)
{
    int answer = 0;
    XmString okLabel, cancelLabel;

    okLabel = XmStringCreateLocalized(okLabelString);
    cancelLabel = XmStringCreateLocalized(cancelLabelString);

    Widget dlg = this->getDialog(wid);

    // Make sure the dialog exists, and that it is an XmMessageBox
    // or subclass, since the callbacks assume this widget type
    
    assert ( dlg != NULL );

    XtVaSetValues(dlg,
		  XmNokLabelString, okLabel,
		  XmNcancelLabelString, cancelLabel,
		  NULL);

    Widget dialog = this->post(title_str,
			       text_str,
			       wid,
			       (void *) &answer,
			       ( DialogCallback ) &my_okCallback,
			       ( DialogCallback ) &my_cancelCallback);

    forceUpdate( dialog );
    while ( answer==0 ) 
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog), XtIMAll );

    // Process just one more event to pop down dialog.
    XtAppProcessEvent(XtWidgetToApplicationContext(dialog), XtIMAll );

    return(answer);

}



