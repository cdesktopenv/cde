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
/* $TOG: MainWindow.h /main/11 1998/08/21 15:57:56 mgreess $
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


////////////////////////////////////////////////////////////////////
// MainWindow.h: Support a toplevel window
////////////////////////////////////////////////////////////////////
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "UIComponent.h"

class MainWindow : public UIComponent {
    
protected:
    
    Widget   _main;        // The XmMainWindow widget
    Widget   _workArea;    // Widget created by derived class
    Boolean	_allow_resize;
    Pixmap	_icon;
    GC		_icon_invert;
    GC		_window_invert;
    int		_last_state;
    Window	_flash_owin;
    Window	_flash_iwin;
    XWindowAttributes
		_window_attributes;
    
    // Derived classes must define this function to 
    // create the application-specific work area.
    
    virtual Widget createWorkArea ( Widget ) = 0;

    virtual void getIconColors(Pixmap & fore, Pixmap & back);
    
  public:
    
    MainWindow ( char *name, Boolean allowResize=FALSE );
    virtual ~MainWindow();
    
    // The Application class automatically calls initialize() 
    // for all registered main window objects
    
    virtual void initialize();
    virtual void disableWorkAreaResize();
    virtual void enableWorkAreaResize();
    virtual void manage();   // popup the window
    virtual void unmanage(); // pop down the window
    virtual void iconify();
    virtual void setIconTitle(const char * title);
    virtual void setIconName(const char * name);
    virtual void title(const char *);
    virtual void flash(const int count);
    virtual void quit(Boolean delete_win = FALSE)=0;
    virtual void panicQuit()=0;
    virtual Boolean isIconified();

    // Functions to control session management.
    virtual int  smpSaveSessionGlobal(void) = 0;
    virtual void smpSaveSessionLocal(void) = 0;


    virtual void busyCursor(void);
    virtual void normalCursor(void);

    virtual void setStatus(const char *);
    virtual void clearStatus(void);

    virtual void propsChanged(void) = 0;

    void	 setWorkspacesOccupied(char *workspaces);
    char	 *MbStrchr(char *str, int ch);

  private:
    static void quitCallback( Widget, XtPointer, XmAnyCallbackStruct * );

    static void flashCallback(XtPointer, XtIntervalId *);
    void doFlash(XtIntervalId *);

    int		_flashing;
};
#endif
