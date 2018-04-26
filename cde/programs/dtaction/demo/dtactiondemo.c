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
/* $TOG: dtactiondemo.c /main/4 1998/04/20 12:47:18 mgreess $ */
/*****************************************************************************
 *****************************************************************************
 **
 **   File:         dtactiondemo.c
 **
 **   Description:  This file contains the dtactiondemo program that
 **		    demonstrates the use of the Action Library functions.
 **		    It executes actions from the action database.  It's
 **		    usage is as follows:
 **
 **			Usage: dtactiondemo ACTION [file1 file2 ...]
 **
 **                 where ACTION is the name of an action and file1, ...  
 **		    are the action's file arguments.
 **   
 **		    This program does not support the "host:" semantics.
 **
 **                 Use the datatyping demo to display the supported
 **                 actions for a file.
 **		  
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <limits.h>
#include <stddef.h>
#include <nl_types.h>
#include <signal.h>

#include <errno.h>

#include <Xm/XmP.h>
#include <Xm/Text.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <Xm/Protocols.h>
#include <Xm/MwmUtil.h>

#include <Dt/EnvControlP.h>
#include <Dt/DbUtil.h>
#include <Dt/Action.h>

#include <time.h>

#ifndef PATH_MAX
#	define	PATH_MAX	511
#endif
#ifndef HOSTNAME_MAX_LEN
#	define	HOSTNAME_MAX_LEN	127
#endif


/* Command line options */
XrmOptionDescRec option_list[] =
{
   {  "-timeout",  "timeout",       XrmoptionSepArg,   NULL},
   {  "-contextHost", "contextHost",XrmoptionSepArg,   NULL},
   {  "-contextDir", "contextDir",  XrmoptionSepArg,   NULL},
   {  "-execHost", "execHost",      XrmoptionSepArg,   NULL},
   {  "-termParms", "termParms",    XrmoptionSepArg,   NULL},
};

/* Fallback Resources */
static char *fallback_resources[] = {
    "*timeout: 1",
    (char *) NULL
};

typedef struct {
   int    timer;
   char * contextHost;
   char * contextDir;
   char * execHost;
   char * termParms;
} ApplArgs, *ApplArgsPtr;



/* Dtaction resources */
XtResource resources[] =
{
   {
    "timeout", "Timeout", XmRInt, sizeof(int),
    XtOffsetOf(ApplArgs, timer), XmRImmediate, (XtPointer) 1,
   },
   {
    "contextHost", "ContextHost", XmRString, sizeof(char *),
    XtOffsetOf(ApplArgs, contextHost), XmRImmediate, (XtPointer) NULL,
   },
   {
    "contextDir", "ContextDir", XmRString, sizeof(char *),
    XtOffsetOf(ApplArgs, contextDir), XmRImmediate, (XtPointer) NULL,
   },
   {
    "execHost", "ExecHost", XmRString, sizeof(char *),
    XtOffsetOf(ApplArgs, execHost), XmRImmediate, (XtPointer) NULL,
   },
   {
    "termParms", "TermParms", XmRString, sizeof(char *),
    XtOffsetOf(ApplArgs, termParms), XmRImmediate, (XtPointer) NULL,
   },
};


void CheckForDone( 
                        XtPointer clientData,
                        XtIntervalId id) ;


/********    End Forward Function Declarations    ********/

/****************************************************************************/

static Widget toplevel;
static ApplArgs appArgs;
static DtDirPaths  *dirPaths;
static XtAppContext appContext;


/* ARGSUSED */
void
CheckForDone(
   XtPointer clientData,
   XtIntervalId id)
{
    if ( toplevel->core.num_popups ==  0 ) 
	exit(0);

    XtAppAddTimeOut(appContext,
	appArgs.timer * 1000, (XtTimerCallbackProc)CheckForDone, NULL);
}


void
main( 
     int argc,
     char **argv ) 
{
    Display *display;
    Arg args[20];
    int n=0;
    char *actionName;
    int numArgs = 0;
    char contextHost[HOSTNAME_MAX_LEN+1];
    char contextDir[PATH_MAX+1];
    DtActionFileArgp ap = NULL;
  
    if (argc < 2) exit(0);

    XtSetLanguageProc(NULL, NULL, NULL);

    /*  Initialize the toolkit and open the display  */
   (void) signal(SIGCLD, (void (*)())SIG_IGN);
   toplevel = XtAppInitialize(&appContext , "Dtaction", option_list, 
	sizeof(option_list)/sizeof(XrmOptionDescRec), &argc, argv,
	fallback_resources, (ArgList) NULL, (Cardinal) NULL);  
  
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetArg(args[n], XmNmappedWhenManaged, False); n++;
    XtSetArg(args[n], XmNheight, 1); n++;
    XtSetArg(args[n], XmNwidth, 1); n++;
    XtSetValues(toplevel, args, n);
    XtRealizeWidget(toplevel);

    display = XtDisplay (toplevel);
    XtGetApplicationResources(toplevel, &appArgs, 
	resources, XtNumber(resources), NULL, 0);

    if (appArgs.timer < 1)
      appArgs.timer = 1;

    _DtEnvControl(DT_ENV_SET);

   /*  Get Dt initialized  */
   if (DtInitialize (display, toplevel, argv[0], "Dtaction") == False)
   {
      /* Fatal Error: could not connect to the messaging system. */
      /* DtInitialize() has already logged an appropriate error msg */
      exit(-1);
   }


   /* Load the filetype/action dbs; DtInvokeAction() requires this */

    DtDbLoad();

   /*
    * Get the requested action name
    */
    actionName = argv[1];

    if ( argc > 2 ) 
    {
	/*
	 * create an action arg array for the file objects for
	 * this action.  This number of objects should be one
	 * less than the argument count. The returned vector will
	 * be terminated by a null pointer.
	 */
	numArgs= argc - 2;
	ap = (DtActionFileArgp) XtCalloc(numArgs,sizeof(DtActionFileArg));
    }

    for ( n = 0; n < numArgs; n++) {
        ap[n].type = NULL;
        ap[n].host = NULL;
	ap[n].name = argv[n+2];
    }

    _DtActionInvokeOnFiles(toplevel, actionName, numArgs, ap,
	appArgs.termParms,appArgs.execHost,appArgs.contextHost,
	appArgs.contextDir,True);
    
    /*
     * Set up a timer for when it is safe to exit.
     * We must invoke XtMainLoop() at least once, to force any prompt or
     * error dialogs to get posted.
     */
    XtAppAddTimeOut(appContext,
	appArgs.timer * 1000, (XtTimerCallbackProc)CheckForDone,
                   NULL);
    XtAppMainLoop(appContext);

}


