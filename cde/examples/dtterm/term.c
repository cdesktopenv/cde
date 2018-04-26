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
/* $XConsortium: term.c /main/3 1995/10/27 10:40:51 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company	
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */


/*
 * term.c
 *
 * Example code for DtTerm widget
 *
 * Create a terminal widget running a shell and provide
 * push button shortcuts for shell commands. Regular shell
 * commands can be entered also.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <Xm/MessageB.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Dt/Term.h>

static Widget toplevel;

static void CreateTerm(Widget, char *);
static void SendCommandCb(Widget, XtPointer, XtPointer);
static void AddCommandButton(Widget, char *, char *);

main(int argc, char **argv)
{
    XtAppContext appContext;
    Arg args[20];
    int n;
    XmString labelString;

    Widget mainWindow, termContainer, termTitle;
    
    /* Initialize DtTerm widget library */

    DtTermInitialize();

    toplevel = XtAppInitialize(&appContext, "Term", NULL, 0, &argc, argv,
				NULL, NULL, 0);

    n = 0;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_TEMPLATE); n++;
    mainWindow = XmCreateMessageBox(toplevel, "mainWindow", args, n);
    XtManageChild(mainWindow);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 10); n++;
    XtSetArg(args[n], XmNmarginHeight, 10); n++;
    termContainer = XmCreateFrame(mainWindow, "termContainer", args, n);
    XtManageChild(termContainer);

    labelString = XmStringCreateLocalized("DtTerm with date and time shortcuts");
    n = 0;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD); n++;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    termTitle = XmCreateLabel(termContainer, "termTitle", args, n);
    XtManageChild(termTitle);
    XmStringFree(labelString);

    /* Create the terminal widget */

    CreateTerm(termContainer, "/bin/sh");

    /* Add shortcut buttons to the message box */

    AddCommandButton(mainWindow, "Today", "date\n");
    AddCommandButton(mainWindow, "Month", "cal\n");
    AddCommandButton(mainWindow, "1994", "clear;cal 1994\n");
    AddCommandButton(mainWindow, "1995", "clear;cal 1995\n");
    AddCommandButton(mainWindow, "1996", "clear;cal 1996\n");
    AddCommandButton(mainWindow, "1997", "clear;cal 1997\n");
    AddCommandButton(mainWindow, "1998", "clear;cal 1998\n");
    AddCommandButton(mainWindow, "1999", "clear;cal 1999\n");
    AddCommandButton(mainWindow, "2000", "clear;cal 2000\n");

    XtRealizeWidget(toplevel);
    XtAppMainLoop(appContext);
}


/*
 * Create a DtTerm
 */

static void CreateTerm(Widget parent, char *cmd)
{
    Widget term;
    Arg args[20];
    int n;
    
    /*
     * Create a DtTerm widget.
     * Pass the command to execute.
     * Configure the window to fit a calendar year.
     */

    n = 0;
    XtSetArg(args[n], DtNsubprocessCmd, cmd); n++;
    XtSetArg(args[n], DtNrows, 46); n++;
    XtSetArg(args[n], DtNcolumns, 80); n++;
    term = DtCreateTerm(parent, "term", args, n);
    XtManageChild(term);
}

static void AddCommandButton(Widget parent, char *label, char *cmd)
{
    XmString labelString;
    Arg args[1];
    Widget button;

    /* Create a pushbutton which will send a command to the terminal */

    labelString = XmStringCreateLocalized(label);
    XtSetArg(args[0], XmNlabelString, labelString);
    button = XmCreatePushButton(parent, label, args, 1);
    XtManageChild(button);
    XmStringFree(labelString);
    XtAddCallback(button, XmNactivateCallback, SendCommandCb, (XtPointer)cmd);
}

static void SendCommandCb(Widget w, XtPointer cd, XtPointer cb)
{
    Widget term = XtNameToWidget(toplevel, "*term");
    unsigned char *cmd = (unsigned char*)cd;

    /* send the pushbutton command to the terminal widget */

    DtTermSubprocSend(term, cmd, strlen((char*)cmd));
}

