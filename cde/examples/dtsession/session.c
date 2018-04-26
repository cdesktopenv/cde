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
/* $XConsortium: session.c /main/3 1995/10/27 10:40:33 rswiston $ */
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company	
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 */


/*
 * session.c
 *
 * Example code for Dt Session Manager conventions and API
 */

#include <stdio.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/Protocols.h>
#include <Dt/Session.h>


/*
 * Define '-session' command line option
 */

typedef struct _ApplicationArgs {
	String session;
} ApplicationArgs;

static XtResource applicationResources[] = {
	{
	"session",
	"Session",
	XmRString,
	sizeof(String),
	XtOffsetOf(ApplicationArgs,session),
	XmRImmediate,
	NULL
	},
};

static XrmOptionDescRec commandLineOpts[] = {
	{ "-session", "session", XrmoptionSepArg, NULL },
};

static ApplicationArgs applicationArgs;

/*
 * Simple application state to be preserved across sessions
 */

static int lightState = False;

/*
 * miscellaneous global data
 */

static XtAppContext appContext;
static Widget toplevel;
static int savedArgc;
static char **savedArgv;

static void PreserveCommandLine(int, char **);
static void SetWmCommand(char *);
static void SaveSessionCb(Widget, XtPointer, XtPointer);
static void RestoreSession(Widget, char*);
static void SaveApplicationState(char *);
static void RestoreApplicationState(char *);


main(int argc, char **argv)
{
    Widget mainWindow, toggle;
    XmString labelString;
    Arg args[1];
    
    /* Save the command line before Xt parses out the standard options */
    PreserveCommandLine(argc, argv);

    /* Create the application UI */

    toplevel = XtAppInitialize(&appContext, "Session",
				commandLineOpts, XtNumber(commandLineOpts),
				&argc, argv,
				NULL,
				NULL, 0);

    XtGetApplicationResources(toplevel, &applicationArgs,
				applicationResources,
				XtNumber(applicationResources),
				NULL, 0);

    mainWindow = XmCreateWorkArea(toplevel, "mainWindow", NULL, 0);
    XtManageChild(mainWindow);

    labelString = XmStringCreateLocalized("Lights");
    XtSetArg(args[0], XmNlabelString, labelString);
    toggle = XmCreateToggleButton(mainWindow, "lightsToggle", args, 1);
    XtManageChild(toggle);
    XmStringFree(labelString);

    /* Add callback to detect session manager messages */

    XmAddWMProtocolCallback(toplevel,
		XInternAtom(XtDisplay(toplevel), "WM_SAVE_YOURSELF", False),
		SaveSessionCb, (XtPointer)toplevel);

    /* Restore state if application was restarted by session manager */

    if (applicationArgs.session != NULL) {
	RestoreSession(toplevel, applicationArgs.session);
    }

    XtRealizeWidget(toplevel);
    XtAppMainLoop(appContext);
}


/*
 * Save session state
 */

static void SaveSessionCb(Widget w, XtPointer cd, XtPointer cb)
{
    Widget toplevel = (Widget)cd;
    char *savePath = NULL;
    char *saveFile = NULL;

    DtSessionSavePath(toplevel, &savePath, &saveFile);

    if (savePath != NULL) {
	SaveApplicationState(savePath);
	SetWmCommand(saveFile);
	XtFree(savePath);
	XtFree(saveFile);
    }
}

static void SaveApplicationState(char *path)
{
    Widget toggle = XtNameToWidget(toplevel, "*lightsToggle");
    FILE *fp;

    lightState = XmToggleButtonGetState(toggle);

    if ((fp = fopen(path, "w")) != NULL) {
	fprintf(fp, "%d", lightState);
	fclose(fp);
    }
}

static void PreserveCommandLine(int argc, char **argv)
{
    int i;

    savedArgv = (char **)XtMalloc(argc*sizeof(char *));
    savedArgc = argc;
    for (i=0; i < argc; i++) savedArgv[i] = XtNewString(argv[i]);
}


static void SetWmCommand(char *sessionId)
{
    char **wm_command;
    int i, j;

    wm_command = (char **) XtMalloc((savedArgc+2) * sizeof(char*));
    wm_command[0] = XtNewString(savedArgv[0]);
    wm_command[1] = XtNewString("-session");
    wm_command[2] = XtNewString(sessionId);

    for (i = 1, j = 3; i < savedArgc; i++) {
	    if (strcmp(savedArgv[i], "-session") == 0) { i++; continue; }
	    wm_command[j] = XtNewString(savedArgv[i]);
	    j++;
    }

    XSetCommand(XtDisplay(toplevel), XtWindow(toplevel), wm_command, j);

    for (i=0; i < j; i++) XtFree(wm_command[i]);
    XtFree((char*)wm_command);
}

/*
 * Restore previously saved state
 */

static void RestoreSession(Widget w, char *restoreFile)
{
    char *restorePath = NULL;

    DtSessionRestorePath(w, &restorePath, restoreFile);

    if (restorePath != NULL) {
	RestoreApplicationState(restorePath);
	XtFree(restorePath);
    }
}

static void RestoreApplicationState(char *path)
{
    Widget toggle = XtNameToWidget(toplevel, "*lightsToggle");
    FILE *fp;

    if ((fp = fopen(path, "r")) != NULL) {
	fscanf(fp, "%d", &lightState);
	fclose(fp);
    }

    XmToggleButtonSetState(toggle, lightState, False);
}


