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
/* $XConsortium: PrintDemo.h /main/6 1996/05/09 03:40:22 drk $ */
#include <Xm/XmAll.h>
#include <Xm/Print.h>
#include <Dt/Print.h>
#include "AppSpecific.h"

/*
 * ------------------------------------------------------------------------
 * Module: Print
 *
 */
/*
 * app specific print data holder 
 */
typedef struct _AppPrintData
{
    Widget print_dialog;
    Widget print_shell;
    Widget pr_button;
    Boolean print_only;
    DtPrintSetupData *print_data;
    AppObject* app_object;

    /* used during pagesetup callback logic */
    int printed_lines ;
    int total_lines ;
    short lines_per_page ;
    Widget ptext, pform;
} AppPrintData;

extern AppPrintData* AppPrintData_new();

/*
 * Print module exported functions
 */
extern void CreatePrintSetup(Widget parent, AppPrintData* p);
extern void PrintMenuCB(Widget, XtPointer, XtPointer);
extern void QuickPrintCB(Widget, XtPointer, XtPointer);

/*
 * ------------------------------------------------------------------------
 * Module: Main
 *
 */
extern void CloseProgramCB(Widget, XtPointer, XtPointer);

/*
 * ------------------------------------------------------------------------
 * Module: MainWindow
 *
 */
/*
 * MainWindow structure
 */
typedef struct _MainWindow
{
    Widget widget;
    Widget print_menu_button;
    Widget quick_print_button;
    Widget exit_button;
} MainWindow;

extern MainWindow* MainWindow_new(Widget parent);


