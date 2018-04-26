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
/* $XConsortium: Test.C /main/2 1995/07/17 14:07:26 drk $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "Application.h"
#include "MainWindow.h"
#include "MenuBar.h"
#include "Menu.h"
#include "Button.h"
#include "Container.h"
#include "IconObj.h"
#include "Sep.h"

#include <stdlib.h>
#include <stdio.h>

static void ExitCB(void *data)
{
   delete ((BaseUI *)data)->Parent();
   exit (0);
}

static void PrintUICB(void *data)
{
   ((AnyUI *) data)->DumpUIHierarchy(true);
}

static void PrintObjectsCB(void *data)
{
   ((BaseUI *) data)->DumpHierarchy();
}

class TestWindow : public MainWindow
{

 public:

   Container *container;
   
   TestWindow(MotifUI *p,char *name) : MainWindow(p,name,NULL,"Fpprnt.l.pm") { }
   void Initialize();
};

void TestWindow::Initialize()
{
   MenuBar *mbar;
   Menu *menu;

   mbar = new MenuBar(this);
   container = new Container("PrinterSubSystem", this, "Printers");
   container->WidthHeight(600, 400);
   container->ContainerView(TREE);
   SetWorkWindow(container);

   // File menu
   menu = new Menu(mbar, "File", "F");
     new Button(menu, "Exit", PUSH_BUTTON, ExitCB, this, "x", "ALT+F4");

   // Edit menu
   menu = new Menu(mbar, "Edit", "E");
     new Button(menu, "Cut", PUSH_BUTTON, NULL, NULL, "t", "Shift+Del");
     new Button(menu, "Copy", PUSH_BUTTON, NULL, NULL, "C", "Ctrl+Ins");
     new Button(menu, "Paste", PUSH_BUTTON, NULL, NULL, "P", "Shift+Ins");
     new Button(menu, "Delete", PUSH_BUTTON, NULL, NULL, "D");

   // View menu
   menu = new Menu(mbar, "View", "V");
     new Button(menu, "New", PUSH_BUTTON, NULL, NULL, "N");
     new Sep(menu);
     new Button(menu, "Select All", PUSH_BUTTON, NULL, NULL, "S", "Ctrl+/");
     new Button(menu, "Unselect All", PUSH_BUTTON, NULL, NULL, "U", "Ctrl+\\");
     new Sep(menu);
     new Button(menu, "Set Preferences...", PUSH_BUTTON, NULL, NULL, "P");
     new Sep(menu);
     new Button(menu, "Save Settings...", PUSH_BUTTON, NULL, NULL, "t");

   // Actions Menu
   menu = new Menu(mbar, "Actions", "A");
     new Button(menu, "Monitor", PUSH_BUTTON, NULL, NULL, "M");
     new Button(menu, "Cancel Job", PUSH_BUTTON, NULL, NULL, "C");

   // Help Menu
   menu = new Menu(mbar, "Help", "H");
     new Button(menu, "Introduction", PUSH_BUTTON, NULL, NULL, "I");
     new Sep(menu);
     new Button(menu, "Tasks", PUSH_BUTTON, NULL, NULL, "T");
     new Button(menu, "Reference", PUSH_BUTTON, NULL, NULL, "R");
     new Button(menu, "On Item", PUSH_BUTTON, NULL, NULL, "O");
     new Sep(menu);
     new Button(menu, "Using Help", PUSH_BUTTON, NULL, NULL, "U");
     new Sep(menu);
     new Button(menu, "Version", PUSH_BUTTON, NULL, NULL, "V");
   mbar->SetHelpMenu(menu);

   // Debug menu
   menu = new Menu(mbar, "Debug", "G");
     new Button(menu, "Print Objects", PUSH_BUTTON, PrintObjectsCB, Parent());
     new Button(menu, "Print UI", PUSH_BUTTON, PrintUICB, Parent());

}

int main(int argc, 
         char **argv)
{
   Application *app = new Application("Printer", "Dtprinter", &argc, argv);
   TestWindow *window = new TestWindow(app, "Printer");
   window->Initialize();

   window->AddAction("Exit", "PrinterSubSystem", ExitCB, NULL, "x", "Alt+F4");
   window->AddAction("Properties...", "Queue", NULL, NULL, "p",
		     "Ctrl+Backspace");
   window->AddSep("Queue");
   window->AddAction("Start", "Queue", NULL, NULL, "S");
   window->AddAction("Stop", "Queue", NULL, NULL, "t");
   window->AddSep("Queue");
   window->AddAction("Monitor", "Queue", NULL, NULL, "M");
   window->AddAction("Cancel", "PrintJob", NULL, NULL, "C");

   window->RegisterPopup(window->container);

   IconObj *lp0 = new IconObj("Queue", window->container, "lp0", "Fpprnt");
   window->RegisterPopup(lp0);
   IconObj *tmp = new IconObj("PrintJob", lp0, "job1", "DtPrtjb");
   window->RegisterPopup(tmp);
   tmp = new IconObj("PrintJob", lp0, "job2", "DtPrtjb");
   window->RegisterPopup(tmp);
   tmp = new IconObj("PrintJob", lp0, "job3", "DtPrtjb");
   window->RegisterPopup(tmp);
   IconObj *lp1 = new IconObj("Queue", window->container, "lp1", "Fpprnt");
   window->RegisterPopup(lp1);
   tmp = new IconObj("PrintJob", lp1, "job4", "DtPrtjb");
   window->RegisterPopup(tmp);
   tmp = new IconObj("PrintJob", lp1, "job5", "DtPrtjb");
   window->RegisterPopup(tmp);

   app->Visible(true);
   app->Run();

   return 0;
}
