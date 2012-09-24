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
/* $TOG: MenuWindow.C /main/5 1997/06/04 18:42:40 mgreess $ */
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


//////////////////////////////////////////////////////////
// MenuWindow.C
//////////////////////////////////////////////////////////

#include "MenuWindow.h"
#include "MenuBar.h"


MenuWindow::MenuWindow( char *name, Boolean allowResize ) :
MainWindow ( name, allowResize ) 
{
    _menuBar = NULL;
}

void MenuWindow::initialize()
{
    
    // Call base class to create XmMainWindow widget
    // and set up the work area.
    
    MainWindow::initialize();
    
    // Specify the base widget of a MenuBar object 
    // the XmMainWindow widget's menu bar.
    
    _menuBar = new MenuBar ( _main, "menubar" );
    
    XtVaSetValues ( _main, 
		   XmNmenuBar, _menuBar->baseWidget(),
		   NULL);
    
    // Call derived class hook to add panes to the menu
    
    createMenuPanes();
    
    _menuBar->manage();



}

MenuWindow::~MenuWindow()
{


    delete _menuBar;
}

void
MenuWindow::getIconColors(Pixel & fore, Pixel & back)
{
    if (_menuBar) {
        XtArgVal fore0, back0;
	XtVaGetValues (_menuBar->baseWidget(),
		       XmNforeground, &fore0,
		       XmNbackground, &back0,
		       NULL);
        fore = (Pixel) fore0;
        back = (Pixel) back0;
    }
    else {
	MainWindow::getIconColors(fore, back);
    }
}
