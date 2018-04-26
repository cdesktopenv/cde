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
/* $XConsortium: PromptDialogManager.C /main/4 1996/04/21 19:40:30 drk $ */
/*
 *+SNOTICE
 *
 *      $XConsortium: PromptDialogManager.C /main/4 1996/04/21 19:40:30 drk $
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


///////////////////////////////////////////////////////////
// PromptDialogManager.C: 
//////////////////////////////////////////////////////////
#include "PromptDialogManager.h"
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <assert.h>
// Define an instance to be available throughout the framework.

PromptDialogManager *thePromptDialogManager = 
    new PromptDialogManager ( "PromptDialog" );

PromptDialogManager::PromptDialogManager ( char   *name ) : 
                                 DialogManager ( name )
{
    // Empty
}

Widget PromptDialogManager::createDialog ( Widget parent )
{
  Widget dialog = XmCreatePromptDialog ( parent, _name, NULL, 0);
    
  XtVaSetValues ( dialog,
		  XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
		  NULL );
  
  return dialog;
}


Widget
PromptDialogManager::post( char *title,
			   char		*text,
			   void		*clientData,
			   DialogCallback ok,
			   DialogCallback cancel,
			    DialogCallback help )
{

    // Get a dialog widget from the cache
    
    Widget dialog = getDialog();
    
    // Make sure the dialog exists, and that it is an XmMessageBox
    // or subclass, since the callbacks assume this widget type
    
    assert ( dialog != NULL );
    assert ( XtIsSubclass ( dialog, xmSelectionBoxWidgetClass ) );
	
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
	Widget w = XmSelectionBoxGetChild ( dialog,
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
	Widget w = XmSelectionBoxGetChild ( dialog,
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
	Widget w = XmSelectionBoxGetChild ( dialog,
					 XmDIALOG_HELP_BUTTON );
        XtUnmanageChild ( w );
    }
    
    // Post the dialog.
    
    XtManageChild ( dialog );

    
    return dialog;
}

Widget
PromptDialogManager::post( char *title,
			   char		*text,
			   Widget wid,
			   void		*clientData,
			   DialogCallback ok,
			   DialogCallback cancel,
			    DialogCallback help )
{

    // Get a dialog widget from the cache
    
    Widget dialog = getDialog(wid);
    
    // Make sure the dialog exists, and that it is an XmMessageBox
    // or subclass, since the callbacks assume this widget type
    
    assert ( dialog != NULL );
    assert ( XtIsSubclass ( dialog, xmSelectionBoxWidgetClass ) );
	
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
	Widget w = XmSelectionBoxGetChild ( dialog,
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
	Widget w = XmSelectionBoxGetChild ( dialog,
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
	Widget w = XmSelectionBoxGetChild ( dialog,
					 XmDIALOG_HELP_BUTTON );
        XtUnmanageChild ( w );
    }
    
    // Post the dialog.
    
    XtManageChild ( dialog );

    
    return dialog;
}
