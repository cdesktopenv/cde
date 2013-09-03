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
/*****************************************************************************
 **
 **   File:        Main.c
 **
 **   Project:     DT 3.0
 **
 **   Description: main Dtstyle program
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*$TOG: Main.c /main/16 1998/07/30 12:32:34 mgreess $*/
/* 
 * (c) Copyright 1997, The Open Group 
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1993, 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include "signal.h"
#ifdef TIME
# include <time.h>
#endif
#ifndef NO_MESSAGE_CATALOG
# include <nl_types.h>
# define TRUE 1
# define FALSE 0
#endif
#include <locale.h>

/* #include <sys/dir.h> */
#include <X11/Xlib.h>
#include <Xm/MwmUtil.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/MessageB.h>

#include <Dt/GetDispRes.h>
#include <Dt/EnvControlP.h>
#include <Dt/Message.h>
#include <Dt/SessionM.h>
#include <Dt/UserMsg.h>
#include <Dt/Wsm.h>
#include <Dt/DtNlUtils.h>
#include <Dt/DtosP.h>

#include "MainWin.h"
#include "ColorMain.h"
#include "ColorFile.h"
#include "Resource.h"
#include "Protocol.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "Main.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define STYLE_LOCK "STYLE_LOCK"
#define SYS_FILE_SEARCH_PATH "DTPMSYSFILESEARCHPATH"
#define MAX_STR_LEN 128

/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/


static int ErrorHandler(
                        Display *disp,
                        XErrorEvent *event ) ;
static int IOErrorHandler(
                        Display *disp ) ;

static void ToolkitErrorHandler(
                        char *message) ;
static void errParentMap( 
                        Widget w,
                        XtPointer client_data,
                        XEvent *event) ;
static void UnmanageCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void DestroyCB( 
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static Boolean NewCreateD( XtPointer shell) ;
static Boolean NewAddTo( XtPointer shell) ;
static Boolean NewCreateTop1( XtPointer shell) ;
static Boolean NewCreateTop2( XtPointer shell) ;
static Boolean NewAddSysPath( XtPointer shell) ;
static Boolean NewAddDirectories( XtPointer shell) ;
static Boolean NewAddHomePath( XtPointer shell) ;
static Boolean NewReadPal( XtPointer shell) ;
static Boolean NewInitPal( XtPointer shell) ;
static Boolean NewAllocColor( XtPointer shell) ;
static Boolean NewBottomColor( XtPointer shell) ;
static Boolean NewCreateButtons( XtPointer shell) ;

extern void WaitChildDeath(void);

/*++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                   */
/*++++++++++++++++++++++++++++++++++++++*/
static XrmOptionDescRec option_list[] =
{
   {  "-session",  "session",    XrmoptionSepArg,  NULL  },
};

/*++++++++++++++++++++++++++++++++++++++*/
/* Global Variables                     */
/*++++++++++++++++++++++++++++++++++++++*/
Style style;
char  *progName;

/*+++++++++++++++++++++++++++++++++++++++++++*/
/*Misc functions all the dialogs use         */
/*+++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++*/
/* raise a dialog window above peer dialogs  */
/* Needs the SHELL of the dialog, not the    */
/* dialog box.                               */
/*+++++++++++++++++++++++++++++++++++++++++++*/
void 
raiseWindow(
        Window dialogWin )
{
  static int changeMask = CWStackMode;
  static XWindowChanges windowChanges = {0,0,0,0,0,0,Above};

    XReconfigureWMWindow(style.display, dialogWin,
      style.screenNum, changeMask, &windowChanges);
}


/************************************************************************
 * CenterMsgCB 
 *    - to be used with message dialogs (assumptions are being made that 
 *      parent is dialog shell, and child is bb, due to Xm hacks for them)
 *      (eg. it sets bb x,y to 0,0 and parents x,y to x,y set for bb)
 *    - parent for positioning only (may not be real parent)
 *    - use client_data for parent... if NULL, use style.errParent if ok,
 *    - or main style.shell (makes this routine more generally usefull)
 *
 ************************************************************************/
void 
CenterMsgCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    int n;
    Position newX, newY;
    Arg args[4];
    Widget   shell;

    /* figure out what to use as "visual" parent */
    shell = (Widget)client_data;
    if (shell == NULL)
    {
        if (!style.errParent || !XtParent(style.errParent) || 
	    !XtIsManaged(style.errParent))
            shell = style.shell;
        else
            shell = XtParent(style.errParent);
    }
    else
	shell = XtParent(shell);

    /* calculate new x,y to be centered in visualParent */
    newX = XtX(shell) + XtWidth(shell)/2 - XtWidth(w)/2;
    newY = XtY(shell) + XtHeight(shell)/2 - XtHeight(w)/2;

    if (newX < 0) newX = 0;
    if (newY < 0) newY = 0;

    n = 0;
    XtSetArg(args[n], XmNx, newX); n++;
    XtSetArg(args[n], XmNy, newY); n++;
    XtSetValues(w, args, n);
}


/*************************************************************/
/* ErrDialog                                                 */
/* Put up an error dialog and block until the user clicks OK */
/* by default, there is no cancel or help button, but the    */
/* dialog is created with autoUnmanage false, and ok/cancel  */
/* callbacks to do the unmanage, so that a help button can   */
/* be used                                                   */
/*************************************************************/
void 
ErrDialog(
        char *errString,
        Widget visualParent )
{
    int           n;
    Arg           args[10];
    XmString      ok;

    /* create the compound string */
    style.tmpXmStr = CMPSTR(errString);

    style.errParent = visualParent;

    if (style.errDialog == NULL)     /* create it */
    {
        ok = XmStringCreateLocalized((String) _DtOkString);

        n = 0;
        XtSetArg(args[n], XmNokLabelString, ok); n++;
        XtSetArg(args[n], XmNmessageString, style.tmpXmStr);                n++;
        XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC);                n++;
        XtSetArg (args[n], XmNautoUnmanage, False);                         n++;
        XtSetArg (args[n], XmNdefaultPosition, False);                      n++;
        style.errDialog = XmCreateErrorDialog(style.shell,"ErrorNotice",args,n);

        XtAddCallback (style.errDialog, XmNokCallback, UnmanageCB, NULL);
        XtAddCallback (style.errDialog, XmNcancelCallback, UnmanageCB, NULL);
        XtAddCallback (style.errDialog, XmNmapCallback, CenterMsgCB, NULL);
        XtUnmanageChild ( XmMessageBoxGetChild (style.errDialog,
                                                XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild ( XmMessageBoxGetChild (style.errDialog,
                                                XmDIALOG_HELP_BUTTON));

        /* set the dialog shell parent title */
        n=0;
        XtSetArg (args[n], XmNmwmInputMode,
                        MWM_INPUT_PRIMARY_APPLICATION_MODAL); n++;
        XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
        XtSetArg (args[n], XmNtitle, ((char *)GETMESSAGE(2, 3, "Error")));   n++;
        XtSetValues (XtParent(style.errDialog), args, n);
    }
    else                 /* change the string */
    {
        n = 0;
        XtSetArg(args[n], XmNmessageString, style.tmpXmStr); n++;
        XtSetValues (style.errDialog, args, n);
    }

    /* free the compound string */
    XmStringFree (style.tmpXmStr);

    if (XtIsManaged(style.errParent) || XtParent(style.errParent) == NULL)
    {
        XtManageChild(style.errDialog);
        /* ring the bell (PM behavior) */
        XBell(style.display, 0);
    }
    else
    {
        XtAddEventHandler(XtParent(style.errParent), StructureNotifyMask, 0,
                          (XtEventHandler)errParentMap, NULL);
    }

}



static void 
errParentMap(
        Widget w,
        XtPointer client_data,
        XEvent *event )
{
    if (event->type == MapNotify)
    {
        XtManageChild(style.errDialog);
        /* ring the bell (PM behavior) */
        XBell(style.display, 0);
        XtRemoveEventHandler(XtParent(style.errParent), StructureNotifyMask,
                             0, (XtEventHandler)errParentMap, NULL);
    }
}

/*********************************************************/
/* InfoDialog                                            */
/* Put up a modeless info dialog.                        */
/* There is no cancel or help button.                    */
/* Dialog is created with autoUnmanage true.             */
/* An ok callback is added which will destroy the dialog */
/* and optionally unmap the parent.                      */
/*********************************************************/
void
InfoDialog(
        char *infoString,
        Widget parent,
        Boolean unmapParent )
{
    int             n;
    Arg             args[10];
    static XmString ok = NULL;
    Widget          w;

    /* create the compound string */
    style.tmpXmStr = CMPSTR(infoString);

    if (ok == NULL)
	ok = CMPSTR((String) _DtOkString);

    /* create it */
    n = 0;
    XtSetArg(args[n], XmNokLabelString, ok);                            n++;
    XtSetArg(args[n], XmNmessageString, style.tmpXmStr);                n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS);               n++;
    XtSetArg(args[n], XmNmwmFunctions, DIALOG_MWM_FUNC);                n++;
    w = XmCreateInformationDialog(parent, "Notice", args, n);

    if (unmapParent)
        XtAddCallback (w, XmNokCallback, DestroyCB, parent);
    else
        XtAddCallback (w, XmNokCallback, DestroyCB, NULL);
    XtUnmanageChild ( XmMessageBoxGetChild(w, XmDIALOG_CANCEL_BUTTON) );
    XtUnmanageChild ( XmMessageBoxGetChild(w, XmDIALOG_HELP_BUTTON) );

    /* set the dialog shell parent title */
    n=0;
    XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
    XtSetArg (args[n], XmNtitle, ((char *)GETMESSAGE(2, 2, "Notice")));   n++;
    XtSetValues (XtParent(w), args, n);

    /* free the compound string */
    XmStringFree (style.tmpXmStr);

    /* manage the info dialog */
    XtManageChild(w);

}


/*++++++++++++++++++++++++++++++++++++++*/
/* UnmanageCB                           */
/*++++++++++++++++++++++++++++++++++++++*/
static void 
UnmanageCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  XtUnmanageChild(w);
}


/*++++++++++++++++++++++++++++++++++++++*/
/* DestroyCB                            */
/*++++++++++++++++++++++++++++++++++++++*/
static void 
DestroyCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  XtDestroyWidget(XtParent(w));
  if (client_data != NULL)
      XtUnmanageChild((Widget)client_data); 
}


/**********************************************************/
/* putDialog                                              */
/* move a dialog up so it isn't covering the main window  */
/* Or down if there is no room up.                        */
/* note: "parent" needs to have valid x,y information...  */
/*       ex: a child of dialog shell doesn't, its parent  */
/*       does, so the parent shell would be passed in     */
/**********************************************************/
void 
putDialog(
        Widget parent,
        Widget dialog )
{
  int n;
  Position newX, newY, pY, pX;
  Dimension pHeight, myHeight, pWidth, myWidth;
  Arg args[4];

    pX = XtX(parent);
    pY = XtY(parent);
    pHeight = XtHeight(parent);
    pWidth = XtWidth(parent);
    myHeight = XtHeight(dialog);
    myWidth = XtWidth(dialog);

    if ((newY = pY - myHeight +5) < 0) 
        newY = pY + pHeight;
    newX = pX + pWidth/2 - myWidth/2;

    n = 0;
    XtSetArg(args[n], XmNx, newX); n++;
    XtSetArg(args[n], XmNy, newY); n++;
    XtSetValues(dialog,args,n);

#ifdef PutDDEBUG
  printf("newX, newY, pY, pX;\n");
  printf("%d    %d    %d  %d\n",newX, newY, pY, pX);
  printf("pHeight, myHeight, pWidth, myWidth;\n");
  printf("%d       %d        %d      %d\n", pHeight, myHeight, pWidth, myWidth);
#endif
}


/*++++++++++++++++++++++++++++++++++++++*/
/* main                                 */
/*++++++++++++++++++++++++++++++++++++++*/
int 
main(
        int argc,
        char **argv )
{
    int             n;
    Arg             args[MAX_ARGS];
    XEvent          event;
    XPropertyEvent *pEvent=(XPropertyEvent *)&event;
    long            mwmFunc;
    Boolean         useMaskRtn, useIconFileCacheRtn;    
    char           *dirs = NULL;
    char           *string;

#ifdef USERHELP
malloc_check(1);
malloc_trace(0);
#endif

    XtSetLanguageProc(NULL, NULL, NULL);
    _DtEnvControl(DT_ENV_SET); 
    

    /* Initialize the toolkit and open the display */
    style.shell = 
        XtInitialize(argv[0], XMCLASS, option_list, 1, (int *)&argc, argv);

#ifdef __osf__
    _XmColorObjCreate(style.shell, NULL, NULL);
#endif

    /* Allow all WS manipulation functions except resize and maximize */
    mwmFunc = MWM_FUNC_ALL ^ (MWM_FUNC_RESIZE | MWM_FUNC_MAXIMIZE); 

    n = 0;
    XtSetArg(args[n], XmNmwmFunctions, mwmFunc); n++;
    XtSetArg(args[n], XmNuseAsyncGeometry, True); n++;
    XtSetValues(style.shell, args, n);

    /* initialize global style data */

    style.display    = XtDisplay(style.shell);
    style.screen     = DefaultScreenOfDisplay(style.display);
    style.screenNum  = DefaultScreen(style.display);
    style.colormap   = DefaultColormap(style.display, style.screenNum);
    style.root       = DefaultRootWindow(style.display);
    style.execName   = argv[0];
    style.errDialog  = NULL;
    style.tmpXmStr   = NULL;
    style.home = (char *) XtMalloc(strlen((char *) getenv("HOME")) + 1);
    strcpy(style.home, (char *) getenv("HOME"));
    style.colorDialog = NULL;
    style.backdropDialog = NULL;
    style.fontDialog = NULL;
    style.kbdDialog = NULL;
    style.mouseDialog = NULL;
    style.audioDialog = NULL;
    style.screenDialog = NULL;
    style.startupDialog = NULL;
    style.dtwmDialog = NULL;
    style.i18nDialog = NULL;
    
    if (progName = DtStrrchr(argv[0], '/')) progName++;
    else progName = argv[0];

    /* Get the lock established to ensure only one dtstyle process
     * is running per screen .. first malloc enough space*/

    if (_DtGetLock (style.display, STYLE_LOCK) == 0)
    {
        _DtSimpleError (progName, DtError, NULL, "%s",
	     ((char *)GETMESSAGE(2, 5, "Style Manager is already running, second attempt aborted.")));
        exit(1);
    }

    InitDtstyleProtocol();
    SetWindowProperties();

    /* Register error handlers */
    XSetErrorHandler(ErrorHandler);
    XSetIOErrorHandler(IOErrorHandler);
    XtAppSetErrorHandler(XtWidgetToApplicationContext(style.shell),
                         ToolkitErrorHandler);
    XtAddEventHandler(style.shell, StructureNotifyMask, 0,
                         (XtEventHandler)MwmReparentNotify, NULL);

    /* set up resolution dependent layout variables */
    switch (_DtGetDisplayResolution(style.display, style.screenNum))
    {
        case LOW_RES_DISPLAY:
            style.horizontalSpacing = 
            style.verticalSpacing = 3;
            break;
            
        case MED_RES_DISPLAY:
            style.horizontalSpacing = 
            style.verticalSpacing = 5;
            break;
            
        case HIGH_RES_DISPLAY:
            style.horizontalSpacing = 
            style.verticalSpacing = 8;
            break;
    }
    
    GetApplicationResources();
    
    XmeGetIconControlInfo(style.screen, &useMaskRtn,
			  &style.useMultiColorIcons, &useIconFileCacheRtn);
    
    
    /* add the directory $HOME/.dt/backdrops */
    
    string = (char *)XtMalloc(strlen(style.home) + strlen("/.dt/backdrops:") + 1);
    sprintf(string, "%s/.dt/backdrops:", style.home);

    dirs = (char *)XtCalloc(1, strlen("/etc/dt/backdrops:/usr/dt/backdrops") + 
			    (style.xrdb.backdropDir == NULL ? 2 :
			    strlen(style.xrdb.backdropDir)) + 
			    strlen(string) + 2);
  

    strcpy(dirs, string);
    if (style.xrdb.backdropDir) 
      {
	strcat(dirs, style.xrdb.backdropDir);
	strcat(dirs, ":"); 
      }
    strcat(dirs, "/etc/dt/backdrops:/usr/dt/backdrops");

    _DtWsmSetBackdropSearchPath(style.screen, dirs, style.useMultiColorIcons);

    if (string != NULL)
      XtFree((char *)string);
    
    if (dirs != NULL)
      XtFree((char *)dirs); 
    
    

    style.count = 0;
    /* if this is started from save session we need to set up the BMS
       first, otherwise do it after making the window. (for user perception
       for how long it takes for the dtstyle to come up) */
    if(style.xrdb.session != NULL) {
      DtInitialize (style.display, style.shell, progName, progName);
      /*Restore a session or build and display the main Window.*/
      if(!restoreSession(style.shell,style.xrdb.session))
	init_mainWindow(style.shell);
    }
    else {
      init_mainWindow(style.shell);
      DtInitialize (style.display, style.shell, progName, progName);
      InitializeAtoms();
      CheckMonitor(style.shell);
      GetDefaultPal(style.shell);
    }
    
    signal(SIGINT,(void (*)())activateCB_exitBtn); 
    signal(SIGTERM,(void (*)())activateCB_exitBtn); 

    /* to avoid defunct screen saver processes */    
    signal(SIGCHLD, (void (*)())WaitChildDeath);

    /* backdrop dialog  needs to know when the workspace changes to recolor 
       the bitmap displayed in the dialog */
    ListenForWorkspaceChange();

    /* if using COLOR builtin, style.workProcs is True */

    if ((XmeUseColorObj() != FALSE) && style.workProcs)

        XtAppAddWorkProc(XtWidgetToApplicationContext(style.shell), 
                        NewCreateD, style.shell);

    XtAppMainLoop(XtWidgetToApplicationContext(style.shell));

    return 0;
}

/************************************************************************
 *
 *  ErrorHandler
 *
 ************************************************************************/
static int
ErrorHandler(
        Display *disp,
        XErrorEvent *event )

{
  #define _DTSTYLE_BUFSIZE 1024
  char errmsg[_DTSTYLE_BUFSIZE];

  _DtPrintDefaultErrorSafe(disp, event, errmsg, _DTSTYLE_BUFSIZE);
  _DtSimpleError(progName, DtWarning, NULL, errmsg, NULL);

   /* We do not want to exit here lets try to continue... */
  return 1;
}


/************************************************************************
 *
 *  IOErrorHandler
 *
 ************************************************************************/
static int
IOErrorHandler (display)
 Display *display;

{
#ifdef DEBUG
    Warning ("X IO error occurred during generic operation");
#endif /* DEBUG */

    exit (1);
    return 1;

} 


/************************************************************************
 *
 *  ToolkitErrorHandler
 * 
 *  All Xt memory allocation errors should fall through to this routine.
 *  There is no need to check for Xtmalloc errors where they are used.
 *
 ************************************************************************/
static void
ToolkitErrorHandler(
        char *message )

{
    _DtSimpleError (progName, DtError, NULL, 
        GETMESSAGE(2, 6, "An X Toolkit error occurred... Exiting.\n"));
    exit (1);
}


static Boolean 
NewCreateD(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /*  Create the Dialog Box Dialog */
    CreateDialogBoxD((Widget)shell);

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("CreateDialogBoxD elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewAddTo, shell);

    return(True);
}

static Boolean 
NewAddTo(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /*  Create the Dialog Box Dialog */
    AddToDialogBox();

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("AddToDialogBox elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewCreateTop1, shell);

    return(True);
}

static Boolean 
NewCreateTop1(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /* Create the top portion of the color dialog */
    CreateTopColor1();

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("CreateTopColor1 elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewCreateTop2, shell);
    return(True);
}

static Boolean 
NewCreateTop2(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /* Create the top portion of the color dialog */
    CreateTopColor2();

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("CreateTopColor2 elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewAddSysPath, shell);

    return(True);
}

static Boolean 
NewAddSysPath(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /* initialize the system directory */
    AddSystemPath();

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("AddSystemPath elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewAddDirectories, shell);

    return(True);
}

static Boolean 
NewAddDirectories(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /* initialize the directorys list in the resource */
    if(style.xrdb.paletteDir != NULL)
      AddDirectories(style.xrdb.paletteDir);
    else
      style.count++;

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("AddDirectories elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewAddHomePath, shell);

    return(True);
}

static Boolean 
NewAddHomePath(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /* initialize the home directory */
    AddHomePath();

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("AddHomePath elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewReadPal, shell);

    return(True);
}

static Boolean 
NewReadPal(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /* Read in the palettes one at a time */
    if (ReadPaletteLoop(True)) {

#ifdef TIME
       gettimeofday(&second, &tzp);
       if(first.tv_usec > second.tv_usec){
          second.tv_usec += 1000000;
          second.tv_sec--;
       }
       printf("ReadPaletteLoop DONE elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    /* Stop work procs if no palettes found - fatal color error.
     * When the color button is pressed, the regular processing
     * will pick up on NumOfPalettes == 0 and post an error
     * dialog */
     
       if (NumOfPalettes == 0)
            return(True);
       loadDatabase();
       XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewInitPal, shell);
       return(True);
    }

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("ReadPaletteLoop NOT DONE elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif
      
    return(False);

}

static Boolean 
NewInitPal(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /* initialize the palette list */
    if(InitializePaletteList((Widget)shell, paletteList, True)) {

#ifdef TIME
       gettimeofday(&second, &tzp);
       if(first.tv_usec > second.tv_usec){
          second.tv_usec += 1000000;
          second.tv_sec--;
       }
       printf("InitializePaletteList DONE elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

       XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewAllocColor, shell);
       return(True);
     }

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("InitializePaletteList NOT DONE elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    return(False);
}

static Boolean 
NewAllocColor(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /* initialize pixels to the right color */
    AllocatePaletteCells((Widget)shell);

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("AllocatePaletteCells elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewBottomColor, shell);

    return(True);
}

static Boolean 
NewBottomColor(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /* Create the bottom portion of the color dialog */
    CreateBottomColor();

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("CreateBottomColor elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    XtAppAddWorkProc(XtWidgetToApplicationContext((Widget) shell), NewCreateButtons, shell);

    return(True);
}

static Boolean 
NewCreateButtons(
        XtPointer shell )
{
#ifdef TIME
    struct timeval first, second, lapsed;
    struct timezone tzp;

    gettimeofday(&first, &tzp);
#endif

    /* initialize pixels to the right color */
    CreatePaletteButtons(style.buttonsForm);

#ifdef TIME
    gettimeofday(&second, &tzp);
    if(first.tv_usec > second.tv_usec){
       second.tv_usec += 1000000;
       second.tv_sec--;
    }
    printf("CreatePaletteButtons elapsed time is %ld seconds, %ld microseconds\n", second.tv_sec - first.tv_sec, second.tv_usec - first.tv_usec);
#endif

    return(True);
}

/*************************************<->*************************************
 *
 *  WaitChildDeath()
 *
 *
 *  Description:
 *  -----------
 *  When a SIGCHLD signal comes in, wait for all child processes to die.
 *
 *
 *  Inputs:
 *  ------
 * 
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
void
WaitChildDeath( void )
{
  int   stat_loc;
  pid_t pid;
  
  pid = wait(&stat_loc);
  signal(SIGCHLD,(void (*)())WaitChildDeath);

}
