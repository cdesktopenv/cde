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
 *	$TOG: SelectFileCmd.h /main/5 1997/05/30 17:47:20 mgreess $
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


///////////////////////////////////////////////////////////////////
// SelectFileCmd.h:  Allow the user to select a file interactively
///////////////////////////////////////////////////////////////////
#ifndef SELECTFILECMD_H
#define SELECTFILECMD_H

#ifndef I_HAVE_NO_IDENT
#endif

#include "NoUndoCmd.h"
#include <Xm/Xm.h>
#include <Xm/ToggleB.h>

typedef void (*FileCallback) ( void *, char * );

class SelectFileCmd : public NoUndoCmd {
    
  private:
    
    static void fileSelectedCB ( Widget, XtPointer, XtPointer );
    static void fileCanceledCB ( Widget, XtPointer, XtPointer );
    static void hiddenCB(Widget, XtPointer, XtPointer);
    void	doHidden(int);
    
  protected:
    
    void doit();              // Called by base class
    char * 	_ok_label;
    char *	_title;

    FileCallback _ok_callback;       // Function called when user selects file.
    void        *_ok_clientData;     // Data provided for ok callback.
    FileCallback _cancel_callback;   // Function called when user cancels.
    void        *_cancel_clientData; // Data provided for cancel callback.
    
    Widget      _fileBrowser; // The Motif widget used to get file

    Widget	_parentWidget;  // Need it to parent fileBrowser.
    Widget	_hidden_button;

    XmString	_directory;	// The directory pointed to.
    
    virtual void fileSelected (char *);
    virtual void fileCanceled ();
    
  public:
    
    SelectFileCmd (const char * name,
		   const char * label,
		   const char * title,
		   const char * ok_label,
		   int          active,
		   FileCallback callback,		// ok callback
		   void *       clientData,		// ok data
		   Widget       parent);

    SelectFileCmd (const char * name,
		   const char * label,
		   const char * title,
		   const char * ok_label,
		   int          active,
		   FileCallback ok_callback,		// ok callback
		   void *       ok_clientData,		// ok data
		   FileCallback cancel_callback,	// cancel callback
		   void *       cancel_clientData,	// cancel data
		   Widget       parent);
    ~SelectFileCmd ();

    char	*getDirectory();
    char	*getSelected();
    int		getHidden();

    void	setDirectory(char *);
    void	setSelected(char *);
    void	setHidden(int);

    Widget	fileBrowser(void) { return _fileBrowser; }
    Widget	hiddenButton(void) { return _hidden_button; }
};
#endif
