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
/* $XConsortium: WorkingDialogManager.C /main/4 1996/04/21 19:40:36 drk $ */
/*
 *+SNOTICE
 *
 *      $XConsortium: WorkingDialogManager.C /main/4 1996/04/21 19:40:36 drk $
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
// WorkingDialogManager.C: 
//////////////////////////////////////////////////////////
#include "WorkingDialogManager.h"
#include "Application.h"
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/Scale.h>
#include "BusyPixmap.h"
#include <assert.h>

WorkingDialogManager *theWorkingDialogManager =
        new WorkingDialogManager ( "WorkingDialog" );

WorkingDialogManager::WorkingDialogManager ( char   *name ) : 
                           DialogManager ( name )
{
    _intervalId  = 0;
    _busyPixmaps = NULL;
}

Widget WorkingDialogManager::createDialog ( Widget parent )
{
    Widget dialog = XmCreateWorkingDialog ( parent, _name, NULL, 0 );
    
    XtVaSetValues ( dialog,
		   XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL,
		   NULL );
    
    XtAddCallback ( dialog, 
		   XmNokCallback, 
		   &WorkingDialogManager::unpostCallback,
		   (XtPointer) this );
    
    XtAddCallback ( dialog, 
		   XmNcancelCallback, 
		   &WorkingDialogManager::unpostCallback,
		   (XtPointer) this );
    
//    if ( !_busyPixmaps )
//	_busyPixmaps = new BusyPixmap ( dialog );    
    
    return dialog;
}

Widget WorkingDialogManager::post (char *title,
				   char          *text, 
				   void          *clientData,
				   DialogCallback ok,
				   DialogCallback cancel,
				   DialogCallback help )
{
    // The the dialog already exists, and is currently in use,
    // just return this dialog. The WorkingDialogManager
    // only supports one dialog.
    
    if ( _w && XtIsManaged ( _w ) )
	return _w;
    
    // Pass the message on to the base class
    
    DialogManager::post (title, text, clientData, ok, cancel, help );
    
    // Call timer to start an animation sequence for this dialog
    
    timer();
    forceUpdate( _w );
    return _w;
}

Widget 
WorkingDialogManager::post (char *title,
			   char          *text, 
			   Widget wid,
			   void          *clientData,
			   DialogCallback ok,
			   DialogCallback cancel,
			   DialogCallback help )
{
    // The the dialog already exists, and is currently in use,
    // just return this dialog. The WorkingDialogManager
    // only supports one dialog.
    
    if ( _w && XtIsManaged ( _w ) )
	return _w;
    
    // Pass the message on to the base class
    
    DialogManager::post (title, text, wid, clientData, ok, cancel, help );
    
    // Call timer to start an animation sequence for this dialog
    
    timer();
    forceUpdate( _w );
    return _w;
}

void WorkingDialogManager::timerCallback ( XtPointer clientData,
					  XtIntervalId * )
{
    WorkingDialogManager *obj = ( WorkingDialogManager * ) clientData;
    
    obj->timer();
}

void WorkingDialogManager::timer ()
{
    if ( !_w )
	return;

    
    // Reinstall the time-out callback to be called again
    
    _intervalId = 
      XtAppAddTimeOut ( XtWidgetToApplicationContext ( _w ),
			250, 
			&WorkingDialogManager::timerCallback,
			( XtPointer ) this );    

    // Get the next pixmap in the animation sequence and display
    // it in the dialog's symbol area.
    
//    XtVaSetValues ( _w, 
//		    XmNsymbolPixmap, _busyPixmaps->next(),
//		    NULL );
    forceUpdate( _w );
    
}

void WorkingDialogManager::unpostCallback ( Widget , 
					   XtPointer clientData, 
					   XtPointer )
{
    WorkingDialogManager *obj = ( WorkingDialogManager* ) clientData;
    
    obj->unpost();
}

void WorkingDialogManager::unpost ()
{
    assert ( _w != NULL );
    
    // Remove the dialog from the screen
    
    XtUnmanageChild ( _w );
    
    // Stop the animation
    
    if ( _intervalId )
	XtRemoveTimeOut ( _intervalId );
}

void WorkingDialogManager::updateMessage ( char *text )
{
    if ( _w )
    {
    
	// Just change the string displayed in the dialog
    
	XmString xmstr = XmStringCreateLocalized ( text ); 
	XtVaSetValues ( _w, XmNmessageString, xmstr, NULL );
	XmStringFree ( xmstr );
    }
    forceUpdate( _w );
}





