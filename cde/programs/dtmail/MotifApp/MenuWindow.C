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
	XtVaGetValues (_menuBar->baseWidget(),
		       XmNforeground, &fore,
		       XmNbackground, &back,
		       NULL);
    }
    else {
	MainWindow::getIconColors(fore, back);
    }
}
