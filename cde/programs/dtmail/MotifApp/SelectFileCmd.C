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
/* $TOG: SelectFileCmd.C /main/9 1998/10/26 17:57:37 mgreess $ */
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


//////////////////////////////////////////////////////////
// SelectFileCmd.C: 
//////////////////////////////////////////////////////////
#include <unistd.h>
#include <stdlib.h>
#include "SelectFileCmd.h"
#include "Application.h"
#include <Xm/FileSB.h>
#include <X11/Intrinsic.h>
#include "Help.hh"

#include <nl_types.h>
extern nl_catd catd;

#include "NLS.hh"

extern "C" {
#include <Dt/HourGlass.h>
}

extern "C" {
extern XtPointer _XmStringUngenerate (
				XmString string,
				XmStringTag tag,
				XmTextType tag_type,
				XmTextType output_type);
}


extern int forceUpdate( Widget );

SelectFileCmd::SelectFileCmd (const char * name, 
			      const char * label,
			      const char * title,
			      const char * ok_label,
			      int          active,
			      FileCallback ok_callback,
			      void        *ok_clientData,
			      Widget       parent) :
			      NoUndoCmd ((char *)name, (char *)label, active )
{
    _ok_label = (ok_label ? strdup(ok_label) : strdup("OK"));
    _title = (title ? strdup(title) : strdup(name));
    _ok_callback       = ok_callback;
    _ok_clientData     = ok_clientData;
    _cancel_callback   = NULL;
    _cancel_clientData = NULL;
    _fileBrowser       = NULL;
    _parentWidget      = parent;
}

SelectFileCmd::SelectFileCmd (const char * name, 
			      const char * label,
			      const char * title,
			      const char * ok_label,
			      int          active,
			      FileCallback ok_callback,
			      void        *ok_clientData,
			      FileCallback cancel_callback,
			      void        *cancel_clientData,
			      Widget       parent) :
			      NoUndoCmd ((char *)name, (char *)label, active )
{
    _ok_label = (ok_label ? strdup(ok_label) : strdup("OK"));
    _title = (title ? strdup(title) : strdup(name));
    _ok_callback       = ok_callback;
    _ok_clientData     = ok_clientData;
    _cancel_callback   = cancel_callback;
    _cancel_clientData = cancel_clientData;
    _fileBrowser       = NULL;
    _parentWidget      = parent;
}

SelectFileCmd::~SelectFileCmd()
{
    free(_ok_label);
    free(_title);
}


void SelectFileCmd::doit()
{
    // Create a FileSelectionBox widget
    
    Arg args[1];
    Cardinal n = 0;
    XmString title;

    // If the FSB already exists and is managed, raise it.

    if (_fileBrowser) {
        XtManageChild ( _fileBrowser );
        XtPopup (XtParent(_fileBrowser), XtGrabNone );
	XRaiseWindow(XtDisplay(_fileBrowser), XtWindow(XtParent(_fileBrowser)));
	forceUpdate(_fileBrowser);
	return;
    }

    // If there is no FSB, create it and manage it.
    // If there is one, just manage it.

    // Creating one is different from the book. cast required.
    // Also, its the "new CDE" FSB!

    if (!_fileBrowser) {
	_DtTurnOnHourGlass(_parentWidget);

	_fileBrowser =
	    XmCreateFileSelectionDialog (_parentWidget,
					 (char *) name(), 
					 args, n );       
	// Set the title right...
	title = XmStringCreateLocalized(_title);
	XmString ok_str = XmStringCreateLocalized(_ok_label);
	XtVaSetValues(_fileBrowser,
		      XmNdialogTitle, title,
		      XmNokLabelString, ok_str,
		      NULL);
	XmStringFree(title);
	XmStringFree(ok_str);
	XmString hidden_str = XmStringCreateLocalized(
			GETMSG(catd, 1, 11, "Show hidden folders and files"));
	_hidden_button = XtVaCreateManagedWidget(
				"hidden", xmToggleButtonWidgetClass,
				 _fileBrowser,
				 XmNlabelString, hidden_str,
				 XmNalignment, XmALIGNMENT_BEGINNING,
				 XmNnavigationType, XmSTICKY_TAB_GROUP,
				 XmNsensitive, TRUE,
				 NULL);
	XmStringFree(hidden_str);

	printHelpId("_fileBrowser", _fileBrowser);
	/* add help callback */
	// XtAddCallback(_fileBrowser, XmNhelpCallback, HelpCB, helpId);
	
	// Set up the callback to be called when the user chooses a file
	    
	XtAddCallback ( _fileBrowser, 
	    XmNokCallback,
	    &SelectFileCmd::fileSelectedCB, 
	    (XtPointer) this );
	XtAddCallback ( _fileBrowser,
	    XmNcancelCallback,
	    &SelectFileCmd::fileCanceledCB,
	    (XtPointer) this );
	XtAddCallback(_hidden_button,
		      XmNvalueChangedCallback,
		      &SelectFileCmd::hiddenCB,
		      this);

	XtSetSensitive(
	    XmFileSelectionBoxGetChild(_fileBrowser, XmDIALOG_HELP_BUTTON),
	    False);

	_DtTurnOffHourGlass(_parentWidget);
    }
    
    // Display the dialog
    
    XtManageChild ( _fileBrowser );

    // Raise it, because it might be buried.
    //
    XRaiseWindow(XtDisplay(_fileBrowser), XtWindow(XtParent(_fileBrowser)));
}

void SelectFileCmd::fileSelectedCB(
				Widget w,
				XtPointer clientData,
				XtPointer callData)
{
    SelectFileCmd * obj = (SelectFileCmd *) clientData;
    XmFileSelectionBoxCallbackStruct *cb = 
	(XmFileSelectionBoxCallbackStruct *) callData;
    char     *name   = NULL;

    // XtUnmanageChild ( w );   // Bring the file selection dialog down.
    
    if (cb->value)
    {
	// Extract the first character string matching the default
	// character set from the compound string
        name = (char *) _XmStringUngenerate(
					cb->value, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
         // if the name is a null string ( no file is selected)
         // we should pop up an error dialog to let user know
         // that he/she did not specify file name. But because
         // the message cat file is frozen, we can not add any
         // new msg. Just free the name and return.
         // see aix defect 176761.
         if(NULL == name) return;
	 if (strlen(name)<1)
         {
           free(name);
           return;
         }

	// If a string was succesfully extracted, call
	// fileSelected to handle the file.

        XtUnmanageChild ( w );   // Bring the file selection dialog down.
	obj->fileSelected ( name );
    }
}

void SelectFileCmd::fileCanceledCB ( Widget w,
				     XtPointer clientData,
				     XtPointer )
{
    SelectFileCmd * obj = (SelectFileCmd *) clientData;

    XtUnmanageChild(w);   // Bring the file selection dialog down.
    obj->fileCanceled();
}

void SelectFileCmd::fileSelected ( char *filename )
{
    if ( _ok_callback )
	_ok_callback ( _ok_clientData, filename );
}

void SelectFileCmd::fileCanceled ()
{
    if ( _cancel_callback )
	_cancel_callback ( _cancel_clientData, NULL );
}

void
SelectFileCmd::hiddenCB(Widget,
			XtPointer client_data,
			XtPointer cb_data)
{
    SelectFileCmd * self = (SelectFileCmd *)client_data;
    XmToggleButtonCallbackStruct *cbs = (XmToggleButtonCallbackStruct*) cb_data;

    self->doHidden(cbs->set);
}

void
SelectFileCmd::doHidden(int on)
{
    XtEnum		style;
    style = (on) ? XmFILTER_NONE : XmFILTER_HIDDEN_FILES;
    XtVaSetValues(_fileBrowser, XmNfileFilterStyle, style, NULL);
    XmFileSelectionDoSearch(_fileBrowser, NULL);
}

char *
SelectFileCmd::getDirectory()
{
    XmString	directory;
    char	*path;

    if (NULL == _fileBrowser)
      return NULL;

    // Get the default selection.
    XtVaGetValues(_fileBrowser, XmNdirectory, &directory, NULL);
    path = (char *)
      _XmStringUngenerate(directory, NULL, XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
    XmStringFree(directory);

    return path;
}

char *
SelectFileCmd::getSelected()
{
    Widget	text;
    char	*path = NULL;

    if (NULL == _fileBrowser)
      return NULL;

    // Set the default selection.
    text = XtNameToWidget(_fileBrowser, "Text");
    if (NULL != text)
      XtVaGetValues(text, XmNvalue, &path, NULL);

    return path;
}

int
SelectFileCmd::getHidden()
{
    int			val;
    unsigned char	current_state;

    if (NULL == _fileBrowser || NULL == _hidden_button)
      return 0;
    
    XtVaGetValues(_hidden_button, XmNset, &current_state, NULL);
    val = (current_state == XmSET) ? 1 : 0;
    return val;
}

void
SelectFileCmd::setDirectory(char *path)
{
    XmString	directory;

    if (NULL == _fileBrowser)
      return;

    // Set the default directory where the file selection box points.
    directory = XmStringCreateLocalized(path);
    XtVaSetValues(_fileBrowser, XmNdirectory, directory, NULL);
    XmStringFree(directory);
}

void
SelectFileCmd::setSelected(char *path)
{
    Widget	text;

    if (NULL == _fileBrowser)
      return;

    // Set the default selection.
    text = XtNameToWidget(_fileBrowser, "Text");
    if (NULL != text)
      XtVaSetValues(text, XmNvalue, path, NULL);
}

void
SelectFileCmd::setHidden(int on)
{
    unsigned char	current_state;
    unsigned char	desired_state;

    if (NULL == _fileBrowser || NULL == _hidden_button)
      return;
    
    desired_state = (on) ? XmSET : XmUNSET;
    XtVaGetValues(_hidden_button, XmNset, &current_state, NULL);
    if (current_state == desired_state)
      return;

    XtVaSetValues(_hidden_button, XmNset, desired_state, NULL);
    doHidden(on);
}
