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
/* $XConsortium: MainWindow.c /main/3 1996/05/09 03:39:51 drk $ */
#include "PrintDemo.h"

/*
 * static function declarations
 */
static void MainWindow_createMenuBar(MainWindow* me);
static void MainWindow_createToolBar(MainWindow* me);

/*
 * ------------------------------------------------------------------------
 * Name: MainWindow_new
 *
 * Description:
 *
 *     Creates the main window of the DtPrint demo program.
 *
 * Return value:
 *
 *     A pointer to a new MainWindow structure.
 *
 */
MainWindow*
MainWindow_new(Widget parent)
{
    MainWindow* me =  (MainWindow*)XtCalloc(1, sizeof(MainWindow));
    /*
     * create the main window
     */
    me->widget =
	XtVaCreateWidget("MainWindow",
			 xmMainWindowWidgetClass,
			 parent,
			 NULL);
    /*
     * create the menu bar and tool bar
     */
    MainWindow_createMenuBar(me);
    MainWindow_createToolBar(me);
    /*
     * return
     */
    return me;
}

/*
 * ------------------------------------------------------------------------
 * Name: MainWindow_createMenuBar
 *
 * Description:
 *
 *     Creates the menu bar for the main window of the DtPrint demo
 *     program.
 *
 */
static void
MainWindow_createMenuBar(MainWindow* me)
{
    Widget file_menu;
    Widget menu_bar;
    XmString label;
    Widget w;
    /*
     * create the menu bar
     */
    menu_bar = XmCreateMenuBar(me->widget, "MenuBar", NULL, 0);
    /*
     * create the file menu
     */
    file_menu = XmCreatePulldownMenu(menu_bar, "FileMenu", NULL, 0);
    /*
     * create the file menu cascade button
     */
    label = XmStringCreateLocalized("File");
    XtVaCreateManagedWidget("FileCascade",
			    xmCascadeButtonGadgetClass,
			    menu_bar,
			    XmNsubMenuId, file_menu,
			    XmNlabelString, label,
			    NULL);
    XmStringFree(label);
    /*
     * create the "Print..." file menu item
     */
    label = XmStringCreateLocalized("Print...");
    me->print_menu_button =
	XtVaCreateManagedWidget("PrintButton",
				xmPushButtonWidgetClass,
				file_menu,
				XmNlabelString, label,
				NULL);
    XmStringFree(label);
    /*
     * separator
     */
    XtVaCreateManagedWidget("", xmSeparatorGadgetClass, file_menu, NULL);
    /*
     * "Exit" button
     */
    label = XmStringCreateLocalized("Exit");
    me->exit_button =
	XtVaCreateManagedWidget("ExitButton",
				xmPushButtonWidgetClass,
				file_menu,
				XmNlabelString, label,
				NULL);
    XmStringFree(label);
    /*
     * manage the menu bar and return it
     */
    XtManageChild(menu_bar);
}

/*
 * ------------------------------------------------------------------------
 * Name: MainWindow_createToolBar
 *
 * Description:
 *
 *     Creates a simple tool bar for the main window of the DtPrint demo
 *     program.
 *
 */
static void
MainWindow_createToolBar(MainWindow* me)
{
    Widget row;
    XmString label;

    row = XtVaCreateManagedWidget(
                                  "ToolBarRow",
                                  xmRowColumnWidgetClass,
                                  me->widget,
                                  XmNorientation, XmHORIZONTAL,
				  XmNscrolledWindowChildType, XmCOMMAND_WINDOW,
                                  NULL);
    /*
     * create just the "quick print" button
     */
    label = XmStringCreateLocalized("Quick Print");
    me->quick_print_button =
	XtVaCreateManagedWidget("QuickPrintButton",
				xmPushButtonWidgetClass,
				row,
				XmNlabelString, label,
				NULL);
    XmStringFree(label);
}
