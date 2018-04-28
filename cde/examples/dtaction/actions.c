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
/* $XConsortium: actions.c /main/3 1995/10/27 10:37:37 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */

#include <Xm/XmAll.h>
#include <Dt/Dt.h>
#include <Dt/Action.h>

#define ApplicationClass "Dtaction"

static Widget shell;
static XtAppContext appContext;
static Widget actionText;
static Widget fileText;

static void CreateWidgets(Widget);
static void InvokeActionCb(Widget, XtPointer, XtPointer);
static void InvokeAction(char*, char*);
static void DbReloadProc(XtPointer);

void main(int argc, char **argv) 
{
    Arg args[20];
    int n=0;
    int numArgs = 0;

    shell = XtAppInitialize(&appContext , ApplicationClass, NULL, 0, 
						&argc, argv, NULL, args, n);  

    CreateWidgets(shell);

    if (DtInitialize(XtDisplay(shell), shell, argv[0], ApplicationClass)==False) {
	/* DtInitialize() has already logged an appropriate error msg */
	exit(-1);
    }

    /* Load the filetype/action databases */
    DtDbLoad();

    /* Notice changes to the database without needing to restart application */
    DtDbReloadNotify(DbReloadProc, NULL);

    XtRealizeWidget(shell);
    XmProcessTraversal(actionText, XmTRAVERSE_CURRENT);

    XtAppMainLoop(appContext);
}

static void CreateWidgets(Widget shell)
{
    Widget messageBox, workArea, w;
    Arg args[20];
    int n;
    XmString labelString;

    labelString = XmStringCreateLocalized("Invoke");

    n = 0;
    XtSetArg(args[n], XmNdialogType, XmDIALOG_TEMPLATE); n++;
    XtSetArg(args[n], XmNokLabelString, labelString); n++;
    messageBox = XmCreateMessageBox(shell, "messageBox", args, n);
    XtManageChild(messageBox);
    XmStringFree(labelString);
    XtAddCallback(messageBox, XmNokCallback, InvokeActionCb, NULL);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_END); n++;
    workArea = XmCreateWorkArea(messageBox, "workArea", args, n);
    XtManageChild(workArea);

    labelString = XmStringCreateLocalized("Invoke Action:");
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    w = XmCreateLabel(workArea, "actionLabel", args, n);
    XtManageChild(w);
    XmStringFree(labelString);

    labelString = XmStringCreateLocalized("On File:");
    n = 0;
    XtSetArg(args[n], XmNlabelString, labelString); n++;
    w = XmCreateLabel(workArea, "fileLabel", args, n);
    XtManageChild(w);
    XmStringFree(labelString);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 12); n++;
    actionText = XmCreateTextField(workArea, "actionText", args, n);
    XtManageChild(actionText);

    n = 0;
    XtSetArg(args[n], XmNcolumns, 12); n++;
    fileText = XmCreateTextField(workArea, "fileText", args, n);
    XtManageChild(fileText);
}

static void DbReloadProc(XtPointer cd)
{
    /* Pick up any dynamic changes to the database files */
    DtDbLoad();
}

static void InvokeActionCb(Widget w, XtPointer cd, XtPointer cb)
{
    char *action;
    char *file;

    action = XmTextFieldGetString(actionText);

    if (action == NULL) return;
    if (strlen(action) == 0) {
	XtFree(action);
	return;
    }

    file = XmTextFieldGetString(fileText);

    InvokeAction(action, file);

    XtFree(action);
    XtFree(file);

    XmTextFieldSetString(actionText, "");
    XmTextFieldSetString(fileText, "");

    XmProcessTraversal(actionText, XmTRAVERSE_CURRENT);
}

static void InvokeAction(char *action, char *file)
{
    DtActionArg *ap = NULL;
    int nap = 0;
    DtActionInvocationID actionId;
  
    /* If a file was specified, build the file argument list */

printf("%s(%s)\n",action,file);
    if (file != NULL && strlen(file) != 0) {
	ap = (DtActionArg*) XtCalloc(1, sizeof(DtActionArg));
	ap[0].argClass = DtACTION_FILE;
	ap[0].u.file.name = file;
	nap = 1;
    }

    /* Invoke the specified action */

    actionId = DtActionInvoke(shell,action,ap,nap,NULL,NULL,NULL,True,NULL,NULL);
}

