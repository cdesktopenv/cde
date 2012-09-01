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
/* $TOG: Action.c /main/28 1999/09/16 14:55:25 mgreess $ */
/* 
 * (c) Copyright 1997, The Open Group 
 */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:         Action.c
 **
 **   Project:	    DT
 **
 **   Description:  This file contains the action library source code.
 **		  
 **
 ** (c) Copyright 1993, 1994 Hewlett-Packard Company
 ** (c) Copyright 1993, 1994 International Business Machines Corp.
 ** (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 ** (c) Copyright 1993, 1994 Novell, Inc. 
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*LINTLIBRARY*/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

#ifdef _SUN_OS /* Need this for the strtod () call */
#include <floatingpoint.h>
#endif /* _SUN_OS */

#define X_INCLUDE_STRING_H
#define XOS_USE_XT_LOCKING
#include <X11/Xos_r.h>

#include <stdlib.h>
#include <limits.h>

#include <X11/Intrinsic.h>

#include <Dt/DtP.h>
#include <Dt/Dts.h>
#include <Dt/Help.h>
#include <Dt/Message.h>
#include <Dt/Connect.h>
#include <Dt/Indicator.h>
#include <Dt/DtNlUtils.h>
#include <Dt/CommandM.h>
#include <Dt/Utility.h>
#include <Dt/Service.h>
#include <Dt/UserMsg.h>

#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/MessageB.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>

#include <Dt/ActionP.h>
#include <Dt/ActionUtilP.h>
#include <Dt/ActionDb.h>
#include <Dt/ActionFind.h>
#include <Tt/tttk.h>

#include <Dt/Action.h>

#include "myassertP.h"
#include "DtSvcLock.h"

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/opt/dt"
#endif

extern char * _DtStripSpaces( 
                        char * string) ;
extern char * _DtDbPathIdToString( DtDbPathId pathId) ;

#define _MAX_MAP_ATTEMPTS	100	/* Maximum nuber of "MAPS" that will
					   be done. */
#define _DT_ACTION_MAX_CLOSE_TRIES	5

/********    Public Function Declarations    ********/

void _DtCreateErrorDialog( 
                        Widget w,
                        char * actionName,
                        XmString msg) ;
Boolean _DtCompileMessagePiece(
                        Widget w,
                        ActionRequest *request,
                        char * relPathHost,
                        char * relPathDir,
	                parsedMsg * piece,
	                Boolean initialize,
	                unsigned long processingMask,
			Boolean ** paramUsed,
			int * promptDataIndex ) ;
ActionRequest * _DtCloneRequest (
                        ActionRequest * request) ;
void _DtFreeRequest( 
                        register ActionRequest *request) ;

char * _DtFindCwd( void ) ;

char * _DtActMapFileName(
                        const char * curHost,
                        const char * dir,
                        const char * file,
                        const char * newHost ) ;

extern void _DtProcessTtRequest(
                        Widget w,
                        ActionRequest *request,
                        char * relPathHost,
                        char * relPathDir ) ;
extern Tt_status _DtInitializeToolTalk(Widget w);

extern Boolean _DtEmptyString(
                        String str) ;

/********    End Public Function Declarations    ********/


/********    Static Function Declarations    ********/


static void FreeErrorDialog( 
                        Widget w,
                        XtPointer user_data,
                        XtPointer call_data) ;
static void InvalidFilename( 
                        Widget w,
                        char * actionName,
                        char * filename) ;
static void HostAccessError( 
                        Widget w,
                        char * actionName,
                        char * hostName) ;
static void MultiHostAccessError(
                        Widget w,
                        char * actionName,
                        char * hostList) ;
static void NoActionError( 
                        Widget w,
			DtShmBoson  origNameQuark,
                        char * actionName,
			char * type,
                        char * host,
                        char * dir,
                        char * file) ;
static void MapError( 
                        Widget w,
                        char * actionName ) ;
static void CommandInvokerError( 
                        Widget w,
                        char * actionName,
                        char * errorString) ;
static void NoToolTalkConnectionError(
                        Widget w,
                        String actionName,
			Tt_status status) ;
static void TmpFileCreateError(
			Widget w,
			char *actionName,
			char *dirName) ;
static void TmpFileOpenError(
			Widget w,
			char *actionName,
			char *fileName) ;
static void TmpFileWriteError(
			Widget w,
			char *actionName,
			char *fileName) ;
static void UnSupportedObject(
			Widget w,
			char *actionName,
			int  objClass);
static void SetExecHost(
                        ActionRequest * request) ;
static void ParseHostList (
                        char * hostString,
                        char *** hostListPtr,
                        int * hostListSizePtr,
                        int * hostCountPtr) ;
static void RemoveDuplicateHostNames (
                        char ** hostList,
                        int   * hostCountPtr ) ;
static void AddFailedHostToList (
                        ActionRequest * request,
                        char * badHost) ;
static int _DtAddEntry( 
                        register char * string,
                        register char * **arrayPtr,
                        register int *sizePtr) ;
static void TryToTypeFile( 
                        ObjectData *obj,
                        char * host,
                        char * dir,
                        char * file,
			char ** resolvedPath);
static ActionRequest * CreateActionRequest( 
                        Widget w,
                        char * actionName,
                        DtActionArg *aap,
			int numArgs,
                        char * termOpts,
			char * execHost,
                        char * cwdHost,
                        char * cwdDir,
                        _DtActInvRecT *invp);
static _DtActInvRecT   *CreateInvocationRecord(
			char		*actionName,
                        Widget		w,
			DtActionArg	*aap,
			int		numArgs);
static Boolean ParseFileArgument(
                        Widget w,
                        ActionRequest * request,
                        ObjectData * objectData,
                        char * hostname,
                        char * filename,
                        char * filetype,
                        Boolean typeFile) ;
static void AddPrompt( 
                        register int argNum,
                        char * prompt,
                        register int *numPrompts,
                        register PromptEntry **prompts) ;
static int MatchParamsToAction( 
                        ActionRequest *request,
                        int *numPrompts,
                        PromptEntry **prompts) ;
static void ProcessOneSegment(
	                ActionRequest * request,
                        parsedMsg * msg,
                        PromptEntry **prompts,
                        int *numPrompts,
	                Boolean * argsOptionFound,
	                int * lastArgReferenced,
	                int * unused,
	                Boolean * paramUsed) ;
static ActionPtr CloneActionDBEntry( 
                        register ActionPtr action) ;
static void CloneParsedMessage(
                        register parsedMsg * old_pmsg,
                        register parsedMsg * new_pmsg ) ;
static void FreeParsedMessage(
                        register parsedMsg * parsedMessage) ;
static parsedMsg * CloneParsedMessageArray(
                        register parsedMsg * pmsgArray,
                        register int count ) ;
static void FreeParsedMessageArray(
                        register parsedMsg * parsedMessageArray,
                        int count ) ;
static Boolean InsertArgumentString(
                        Widget w,
                        register char **bufPtr,
                        int * bufSizePtr,
                        ActionRequest *request,
                        register ObjectData *object,
                        unsigned long mask,
                        char * relPathHost,
                        char * relPathDir,
                        Boolean addLeadingSpace,
                        unsigned long processingMask ) ;
static void InsertUnmappedArgumentString(
                        register char **bufPtr,
                        int * bufSizePtr,
                        register ObjectData *object,
                        Boolean addLeadingSpace ) ;
static char * GrowMsgBuffer( 
                        char * buffer,
                        int *size,
                        int count) ;
static void CmdInvSuccessfulRequest( 
                        char *message,
                        void *data2) ;
static void CmdInvFailedRequest( 
                        char *message,
                        void *data2) ;
static void InitiateDtRequest(
                        Widget w,
                        ActionRequest *request) ;
static Boolean ResolveDtNotifyMessagePieces(
                        Widget w,
                        ActionRequest *request,
                        char * relPathHost,
                        char * relPathDir ) ;
static void InitiateDtNotifyMessage(
                        Widget w,
                        ActionRequest *request ) ;
static void PrepareAndExecuteAction( 
                        Widget w,
                        register ActionRequest *request);
static void __ExtractCWD(
                        register ActionRequest *request,
                        char ** hostPtr,
                        char ** dirPtr,
                        Boolean useObjectInfo) ;
static void ContinueRequest( 
                        Widget widget,
                        XtPointer user_data,
                        XtPointer call_data) ;
static void CancelRequest( 
                        Widget widget,
                        XtPointer user_data,
                        XtPointer call_data) ;
static void CreateContinueDialog( 
                        Widget w,
                        ActionRequest *request,
                        int numPrompts,
                        register PromptEntry *prompts) ;
static void CancelPromptDialog( 
                        Widget widget,
                        PromptDialog *dialog,
                        XtPointer call_data) ;
static void ProcessPromptDialog( 
                        Widget widget,
                        register PromptDialog *dialog,
                        XtPointer call_data) ;
static void ChangePromptTraversal( 
                        Widget widget,
                        register PromptDialog *dialog,
                        XtPointer call_data) ;
static void CreatePromptDialog( 
                        Widget w,
                        ActionRequest *request,
                        register int numPrompts,
                        register PromptEntry *prompts) ;
static Boolean MoreArgumentsToProcess( 
                        register ActionRequest *request) ;
static Boolean ProcessRequest( 
                        Widget w,
                        register ActionRequest *request) ;
static void InitLocalizedStrings( void ) ;
static int LinkToTypeQuark(
                        char * host,
                        char * dir,
                        char * file,
			char **resolvedPath) ;
static void CancelOut(
			Widget w,
			XEvent *event,
			XtPointer params,
			XtPointer num_params);
static void InitiateCommandInvokerRequest(
                        Widget w,
                        ActionRequest *request,
                        char * host,
                        char * dir) ;
static void ProcessCommandInvokerRequest(
                        Widget w,
                        ActionRequest *request,
                        char * relPathHost,
                        char * relPathDir) ;
static Boolean ResolveCommandInvokerMessagePieces(
                        Widget w,
                        ActionRequest *request,
                        char * relPathHost,
                        char * relPathDir) ;
static Tt_callback_action _DbReloadCB(Tt_message m, Tt_pattern p);
static void _DtActTimerCB( XtPointer clientData, 
		XtIntervalId timerId);
static void _DtActIndicatorCB( XtPointer clientData, 
		XtIntervalId timerId);

/********    End Static Function Declarations    ********/


/* Pointers to localizable strings */
static String PromptDialogTitle;
static String ErrorPostfix;
static String PromptDialogLabel;
static String ContinueMessage;
static String HostErrorMsg;
static String HostErrorMsg2;
static String NoActionMsg;
static String NoActionMsg2;
static String NoActionMsg3;
static String MapErrorMsg;
static String InvalidFileMsg;
static String MultiHostErrorMsg;
static String IcccmReqErrorMsg;
static String NoToolTalkConnMsg;
static String UnSupportedObjMsg;
static String TmpFileCreateErrorMsg;
static String TmpFileOpenErrorMsg;
static String TmpFileWriteErrorMsg;

/*
 * RWV:
 * These error messages are used in the ActionTt.c file
 * but were declared static to this file.  -- For the
 * time being I made them global to get things to work.
 */
String ToolTalkErrorMsg;
String ToolTalkErrorMsg2;
String TtFileArgMapErr;


/*
 * Variables needed to make the "Escape" key remove the prompt dialog.
 */
static XtActionsRec actionTable [] = {
   {"Escape", (XtActionProc) CancelOut},
};
static char translations_escape[] = "<Key>osfCancel:Escape()";


/* Help files */
#define PROMPT_HELP     "vg_act"


/* Maximum Indicator activation duration (in milliseconds) */

#define INDICATOR_TIME 		(120 * 1000)
#define MIN_INDICATOR_TIME	  (5 * 1000)



/******************************************************************************
 ******************************************************************************
 *
 *	Public API Functions
 *
 ******************************************************************************
 *****************************************************************************/

/*******************************************************************************
 * DtActionInvoke	-- invoke an action
 *      Widget 		w;		( widget for UI needs)
 *      char   		*action;	( action name )
 *      int        	aac;         	( action arg count )
 *      ActionArgp 	aap;		( action argument pointer )
 *      char		*termOpts;	( (opt) terminal options)
 *      char		*execHost;	( (opt) execution host )
 *      char 		*cwd;           ( (opt) cwd for this action )
 *      int  		useIndicator;	( 1 ==> use indicator, 0 ==> not )
 *      DtActionCallbackProc statusUpdateCb;  (user supplied fcn)
 *      XtPointer       client_data     (user supplied client data)
 *****************************************************************************/
DtActionInvocationID
DtActionInvoke(
	Widget 		w,	
	char   		*action,
	DtActionArg 	*aap,
	int        	aac,   
	char		*termOpts,
	char		*execHost,
        char 		*cwd,
	int  		useIndicator,
        DtActionCallbackProc statusUpdateCb,
        XtPointer       client_data)
{
    register int	i;
    ActionRequest	*request;
    char 		*contextHost= NULL;/* dummy to replace old parameter */
    _DtActInvRecT	*invp;		/* pointer to invocation record */
    Tt_status		 status = TT_OK;
    static Boolean initialized = False;
    extern XtAppContext *_DtInitAppContextp;
    _DtSvcWidgetToAppContext(w);

    _DtSvcAppLock(app);
    _DtSvcAppLock(*_DtInitAppContextp);

    /* We can't handle gadgets; use the parent, if necessary */
    if (XmIsGadget(w))
      w = XtParent(w);
    
    _DtSvcProcessLock();
    if ( !initialized )
    {
      mode_t mode;
      char *tmpDir;

     InitLocalizedStrings();

     /*
      * Make sure Tooltalk is initialized
      */
      status = _DtInitializeToolTalk(w);
      if (TT_OK != status)
      {
          NoToolTalkConnectionError(w, action, status);
          _DtSvcProcessUnlock();    
	  return 0;
      }

     /*
      * Create the DtTmp directory, if necessary.
      */
      tmpDir = _DtGetDtTmpDir();
      /* mode == 0755 */
      mode = (S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH );
      if ( mkdir(tmpDir,mode) )
      {
          /* 
           * Tmp directory creation failure.
           *
           * Make one attempt to create the parent directory if the error
           * was because of a missing path component -- It may be because
           * the "$HOME/.dt" directory hasn't yet been created.
           */
          if ( errno == ENOENT )
          {
              char *parentDir = _DtDirname(tmpDir);
	      if (parentDir && 0 == mkdir(parentDir,mode))
                  mkdir(tmpDir,mode);
              if (parentDir) XtFree(parentDir);
          }
      }
      XtFree(tmpDir);

      _DtInitializeCommandInvoker( XtDisplay(w),
	NULL,	/* bms tool class -- ignored */
	NULL,   /* application class -- ignored */
	(DtSvcMsgContext)NULL, /* reloadDBHandler -- none here */
        (_DtInitAppContextp ? 
	    *_DtInitAppContextp : XtWidgetToApplicationContext(w)));
    }
    initialized = True;
    _DtSvcProcessUnlock();    

    /* Start the activity indicator */
    if ( useIndicator ) {
        _DtSendActivityNotification(INDICATOR_TIME);
    }
 
    if ( (invp = CreateInvocationRecord(action,w,aap,aac)) == NULL)
    {
	myassert( 0 );  /* no request structure --should never happen */
        /* give up -- cannot allocate record */

        if ( useIndicator ) _DtSendActivityDoneNotification();
	_DtSvcAppUnlock(*_DtInitAppContextp);
	_DtSvcAppUnlock(app);
        return 0;
    }
    
    myassert(invp->id);

    if ( useIndicator ) {
      /* Start timer for minimum blink time */
	XtAppAddTimeOut(XtWidgetToApplicationContext(w),
	    MIN_INDICATOR_TIME,  
	    (XtTimerCallbackProc) _DtActIndicatorCB,
	    (XtPointer) invp->id );
    }

    /*
     * Add user callback info to the new invocation record.
     */
    invp->client_data = client_data;
    invp->cb = statusUpdateCb;

    /* Create and fill in the request structure */
    if ( !IS_INV_FINISHED(invp->state) && (request = CreateActionRequest ( 
           w,action,aap,aac,termOpts,execHost,contextHost,cwd,invp)) != NULL)
    {
        if (ProcessRequest(w, request))
        {
	     /* all done invoking ? */
	     RESET_INV_PENDING(invp->state);

	      /* We should only get here if all requests have been honored */
	     SET_INV_COMPLETE(invp->state);

	     /*
	      * Evaluate whether we are done with this invocation. 
	      * We may have to return  values  to the caller.  
	      */
             _DtActExecutionLeafNodeCleanup(invp->id,NULL,0,True);
	     _DtFreeRequest(request);
        }
	/* Otherwise, a dialog was posted; request will be freed later */
    }

    /*
     * Set the indicator that the invocation Id has been returned
     * add a timer so that we can return status info once the
     * caller has gotten the invocation Id.
     */
    SET_INV_ID_RETURNED(invp->state);
    XtAppAddTimeOut(XtWidgetToApplicationContext(w),
	0 /* call back immediately */, 
	(XtTimerCallbackProc) _DtActTimerCB,
	(XtPointer) invp->id );

    _DtSvcAppUnlock(*_DtInitAppContextp);
    _DtSvcAppUnlock(app);
    return invp->id;
}


void
DtDbReloadNotify( DtDbReloadCallbackProc proc, XtPointer client_data)
{
    Tt_status	status;
    Tt_pattern	pattern;
    char *	sessId;
    extern XtAppContext *_DtInitAppContextp;


    if (NULL == proc) return;

    _DtSvcAppLock(*_DtInitAppContextp);

    /* 
     *  Check if we need to initialize tooltalk
     */
    status = _DtInitializeToolTalk(NULL);
    if (TT_OK != status) {
          _DtSvcAppUnlock(*_DtInitAppContextp);
	  return;
    }

    /*
     * This function register a ToolTalk pattern for every
     * callback added.
     */
    pattern = tt_pattern_create();
    if (tt_ptr_error(pattern) != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }

    if (tt_pattern_scope_add(pattern, TT_SESSION) != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }
    if (tt_pattern_category_set(pattern, TT_OBSERVE) != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }
    if (tt_pattern_class_add(pattern, TT_NOTICE) != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }
    if (tt_pattern_state_add(pattern, TT_SENT) != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }
    sessId = tt_default_session();
    if (tt_pattern_session_add(pattern, sessId) != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }
    tt_free( sessId );
    if (tt_pattern_op_add(pattern, "DtTypes_Reloaded") != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }

    /*
     * Store information needed by the callback in the user data
     * fields of the pattern.
     */
    status = tt_pattern_user_set(pattern, 0, (void *)proc);
    if (status != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }
    status = tt_pattern_user_set(pattern, 1, (void *)client_data);
    if (status != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }

    /*
     * _DbReloadCB is the ToolTalk callback which will call
     * the user callback.
     */
    if (tt_pattern_callback_add(pattern, _DbReloadCB) != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }

    if (tt_pattern_register(pattern) != TT_OK) {
        _DtSvcAppUnlock(*_DtInitAppContextp);
	return;
    }

    _DtSvcAppUnlock(*_DtInitAppContextp);
}


/******************************************************************************
 ******************************************************************************
 *
 *	Private API Functions
 *
 ******************************************************************************
 *****************************************************************************/

static void
_DtActTimerCB( XtPointer clientData, XtIntervalId IntId)
{
        _DtActExecutionLeafNodeCleanup((unsigned long)clientData,
           NULL,0,True);
}

static void
_DtActIndicatorCB( XtPointer clientData, XtIntervalId IntId )
{
	unsigned long invocId  = (unsigned long) clientData;
	_DtActInvRecT *invRecP = _DtActFindInvRec( invocId );

	if ( !invRecP || IS_INV_FINISHED(invRecP->state) )
        {
	    /* Turn off the activity indicator */
            _DtSendActivityDoneNotification();
        }
	else
        {
	    /* 
             * Let the action turn off the indicator when invocation
             * is complete.
             */
	    SET_INV_INDICATOR_ON(invRecP->state);
        }
}

/***************************************************************************
*
*  Routines and static data to support DtDbReloadNotify which supplies
*  the user with transparent access to the messaging system for 
*  notification of action/datatypes database changes.
*
****************************************************************************/

/*
 * _DbReloadCB 
 * A ToolTalk callback function used to map callback arguments to
 * the callback function specified by the user.  This function invokes the
 * user-defined DtReloadNotifyProc callback with the desired client_data.
 */

static Tt_callback_action
_DbReloadCB(Tt_message m, Tt_pattern p)
{
    DtDbReloadCallbackProc	proc;
    XtPointer			client_data;

    /*
     * user data 0: DtDbReloadCallbackProc	proc;
     * user data 1: XtPointer			client_data;
     */
    proc = (DtDbReloadCallbackProc)tt_pattern_user(p, 0);
    client_data = (XtPointer)tt_pattern_user(p, 1);

    /*
     * Call registered callback function.
     */
    if (proc) (*proc)(client_data);

    return TT_CALLBACK_PROCESSED;
}


/***************************************************************************/
/***************************************************************************/
/*                        Error Dialog Code                                */
/***************************************************************************/
/***************************************************************************/

/*
 * 'Ok' callback for the generic error dialogs.  It will simply destroy
 * the dialog.
 */

static void 
FreeErrorDialog(
        Widget w,
        XtPointer user_data,
        XtPointer call_data )

{
   XtDestroyWidget(XtParent(w));
}


/*
 * Generic function used to create an error dialog.
 */

void 
_DtCreateErrorDialog(
        Widget w,
        String actionName,
        XmString msg )

{
   String title;
   int n;
   Arg args[10];
   Widget dialog;
   XmString ok;
   XWindowAttributes xwa;
   Status status;
   Boolean is_mapped = False;
   char *fmt;

   fmt = XtNewString((char *)Dt11GETMESSAGE(2, 1, "%1$s%2$s%3$s"));

   /* Create the title string for the dialog */
   title = (char *)XtMalloc((Cardinal)(strlen(PromptDialogTitle) + 
                                       strlen(actionName) +
                                       strlen(ErrorPostfix) +
				       strlen(fmt) + 1));

   (void)sprintf(title, fmt, PromptDialogTitle, actionName, ErrorPostfix);

   XtFree(fmt);

   ok = XmStringCreateLocalized((String)_DtOkString);

   if (XtIsRealized(w))
   {
     status = XGetWindowAttributes (XtDisplay (w), XtWindow (w), &xwa);
     if (status && (xwa.map_state == IsViewable))
       is_mapped = True;
   }

   /* Create the error dialog */
   n = 0;
   XtSetArg(args[n], XmNmessageString, msg); n++;
   XtSetArg(args[n], XmNtitle, title); n++;
   XtSetArg(args[n], XmNokLabelString, ok); n++;
   XtSetArg(args[n], XmNuseAsyncGeometry, True); n++;
   if (!is_mapped) 
   {
      XtSetArg (args[n], XmNdefaultPosition, False);
      n++;
   }
   dialog = XmCreateErrorDialog(w, "errorDialog", args, n);
   XmStringFree(ok);
   XtFree(title);

   /* Set up callbacks */
   XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
   XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
   XtAddCallback(dialog, XmNokCallback, FreeErrorDialog,
		 (XtPointer)NULL);

   /*
    * If the widget is not mapped, center this dialog.
    */
   if (!is_mapped) 
   {
      Dimension dialogWd, dialogHt;
      Widget dialogShell = XtParent(dialog);

      XtSetArg(args[0], XmNmappedWhenManaged, False);
      XtSetValues(dialogShell, args, 1);

      XtManageChild(dialog);
      XtRealizeWidget (dialogShell);

      XtSetArg(args[0], XmNwidth, &dialogWd);
      XtSetArg(args[1], XmNheight, &dialogHt);
      XtGetValues(dialog, args, 2);

      XtSetArg (args[0], XmNx,
		(WidthOfScreen(XtScreen(dialog)) - dialogWd) / 2U);
      XtSetArg (args[1], XmNy,
		(HeightOfScreen(XtScreen(dialog)) - dialogHt) / 2U);
      XtSetValues (dialog, args, 2);

      XtSetArg(args[0], XmNmappedWhenManaged, True);
      XtSetValues(dialogShell, args, 1);
   }

   /* Display the dialog */
   XtManageChild(dialog);
}


/*
 * Error handler for when the user supplied a file name which cannot
 * be accessed.  Displays an error dialog.  Most often, this is caused
 * when a filename with an embedded space is received in the object list.
 */

static void 
InvalidFilename(
        Widget w,
        String actionName,
        String filename )

{
   XmString pt1, pt2, msg;

   /* Construct the error message */
   pt1 = XmStringCreateLocalized(InvalidFileMsg);
   pt2 = XmStringCreateLocalized(filename);
   msg = XmStringConcat(pt1, pt2);

   _DtCreateErrorDialog(w, actionName, msg);
   XmStringFree(pt1);
   XmStringFree(pt2);
   XmStringFree(msg);
}


/*
 * Error handler for when the user supplied a host name which cannot
 * be accessed.  Displays an error dialog.
 */

static void 
HostAccessError(
        Widget w,
        String actionName,
        String hostName )

{
   XmString pt1, pt2, pt3, msg, msg2;

   /* Construct the error message */
   pt1 = XmStringCreateLocalized(HostErrorMsg);
   pt2 = XmStringCreateLocalized(hostName);
   pt3 = XmStringCreateLocalized(HostErrorMsg2);

   msg  = XmStringConcat(pt1, pt2);
   msg2 = XmStringConcat(msg, pt3);

   _DtCreateErrorDialog(w, actionName, msg2);

   XmStringFree(pt1);
   XmStringFree(pt2);
   XmStringFree(pt3);
   XmStringFree(msg);
   XmStringFree(msg2);
}


/*
 * Error handler for when the user supplied a collection of host names 
 * which cannot be accessed.  Displays an error dialog.
 */

static void 
MultiHostAccessError(
        Widget w,
        String actionName,
        String hostList )

{
   XmString msg;
   char * buf = XtMalloc(strlen(MultiHostErrorMsg) + strlen(hostList) + 10);

   sprintf(buf, MultiHostErrorMsg, hostList);
   msg = XmStringCreateLocalized(buf);
   _DtCreateErrorDialog(w, actionName, msg);

   XmStringFree(msg);
   XtFree(buf);
}

/***************************************************************************
 * 
 * MapError - this function creates an error message when an action
 *   cannot be executed because of too many "MAPs". 
 *
 * PARAMETERS:
 *
 *   Widget w;		- Widget needed for posting the error dialog.
 *
 *   String actionName; - The name of the action.
 *
 * RETURN:	void
 *
 ****************************************************************************/

static void 
MapError(
        Widget w,
        String actionName )
{

   XmString msg = XmStringCreateLocalized(MapErrorMsg);
   _DtCreateErrorDialog(w, actionName, msg);
   XmStringFree(msg);

}

/*
 * Error handler for when an action definition cannot be found to
 * match an object of a particular type.  Displays an error dialog.
 * A different message is displayed when no objects is supplied.
 */

/* 
* RWV: 
* Since we use tmp files for buffers and do not support
* strings; we need not add special code for buffer and
* string support.
*/
/* fdt: Will need to also handle a string or a buffer ... eventually */

static void 
NoActionError(
        Widget w,
	DtShmBoson origNameQuark,
        char * actionName,
	char * type,
        char * host,
        char * dir,
        char * file )

{
   char     *msgbuf = XtMalloc(2*MAXPATHLEN);
   XmString msg;
   char *   name = NULL;

   /* Construct the error message */
   if ((host == NULL) && (dir == NULL) && (file == NULL) && (type == NULL) )
   {
      (void)sprintf(msgbuf,NoActionMsg2,actionName);
   }
   else if ( (type != NULL ) && (file == NULL) && (dir == NULL))
   {
      /*
       * We are dealing with a buffer object for which an action couldn't 
       * be located.
       */
      (void)sprintf(msgbuf,NoActionMsg3,actionName,type);
   }
   else
   {
      name = (char *)XtMalloc((Cardinal)((host ? strlen(host) : 0) +
                                         (dir ? strlen(dir) : 0) +
                                         (file ? strlen(file) : 0) + 10));
      name[0] = '\0';
      
      /* Construct the file name */
      if (host)
      {
         (void)strcat(name, host);
         (void)strcat(name, ":");
      }

      if (dir)
      {
         (void)strcat(name, dir);
         if (strcmp(dir, "/") != 0)
            (void)strcat(name, "/");
      }

      if (file)
         (void)strcat(name, file);

      (void)sprintf(msgbuf,NoActionMsg,actionName,name,type);

   }
   msg  = XmStringCreateLocalized(msgbuf);

   _DtCreateErrorDialog(w, actionName, msg);
   XmStringFree(msg);
   if (msgbuf) XtFree(msgbuf);
   XtFree(name);
}


/*
 * Error handler for when the Command Invoker detects an error, and
 * send us a failure response to our action request.
 * Display an error dialog.
 */

static void 
CommandInvokerError(
        Widget w,
        String actionName,
        String errorString )

{
   XmString msg;

   msg = XmStringCreateLocalized(errorString);
   _DtCreateErrorDialog(w, actionName, msg);
   XmStringFree(msg);
}


/*
 * If an action requires a ToolTalk connection, and we were unable to get
 * one, then we will fail and post an error dialog.
 */

static void 
NoToolTalkConnectionError(
        Widget w,
        String actionName,
	Tt_status status)

{
	XmString msg;
	char *errmsg, *statmsg;
                   
	if (TT_OK == status)
	  statmsg = "";
	else
	  statmsg = tt_status_message(status);
	errmsg = XtMalloc(strlen(NoToolTalkConnMsg) + strlen(statmsg) + 2);
	sprintf(errmsg, NoToolTalkConnMsg, statmsg);

	msg = XmStringCreateLocalized(errmsg);
	_DtCreateErrorDialog(w, actionName, msg);

	XtFree(errmsg);
	XmStringFree(msg);
}

static void
TmpFileCreateError( Widget w, char *actionName, char *dirName)
{
        XmString    msg;
        char        *msgbuf = XtMalloc(_DtAct_MAX_BUF_SIZE);
        
        sprintf(msgbuf,TmpFileCreateErrorMsg,_DtActNULL_GUARD(dirName),
             actionName);

	msg  = XmStringCreateLocalized(msgbuf);
        _DtCreateErrorDialog(w, actionName, msg);
        XmStringFree(msg);
        if (msgbuf) XtFree(msgbuf);
}

static void
TmpFileOpenError( Widget w, char *actionName, char *fileName)
{
        XmString    msg;
        char        *msgbuf = XtMalloc(_DtAct_MAX_BUF_SIZE);
        
        sprintf(msgbuf,TmpFileOpenErrorMsg,_DtActNULL_GUARD(fileName),
             actionName);

	msg  = XmStringCreateLocalized(msgbuf);
        _DtCreateErrorDialog(w, actionName, msg);
        XmStringFree(msg);
        if (msgbuf) XtFree(msgbuf);
}

static void
TmpFileWriteError( Widget w, char *actionName, char *fileName)
{
        XmString    msg;
        char        *msgbuf = XtMalloc(_DtAct_MAX_BUF_SIZE);
        
        sprintf(msgbuf,TmpFileWriteErrorMsg,_DtActNULL_GUARD(fileName),
            actionName);

	msg  = XmStringCreateLocalized(msgbuf);
        _DtCreateErrorDialog(w, actionName, msg);
        XmStringFree(msg);
        if (msgbuf) XtFree(msgbuf);
}

static void
UnSupportedObject( Widget w, char *actionName, int objClass)
{
        XmString    msg;
        char        *msgbuf = XtMalloc(_DtAct_MAX_BUF_SIZE);
        
        sprintf(msgbuf,UnSupportedObjMsg,objClass,actionName);

	msg  = XmStringCreateLocalized(msgbuf);
        _DtCreateErrorDialog(w, actionName, msg);
        XmStringFree(msg);
        if (msgbuf) XtFree(msgbuf);
}




/***************************************************************************/
/***************************************************************************/
/*              Main Work Functions For _DtActionInvoke()                   */
/***************************************************************************/
/***************************************************************************/


/*
 * Load the globals pointing to any localizable strings.
 */

static void 
InitLocalizedStrings( void )

{
   PromptDialogTitle = XtNewString(((char *)Dt11GETMESSAGE(2, 3, "Action:  ")));
   ErrorPostfix = XtNewString(((char *)Dt11GETMESSAGE(2, 4, "   [Error]")));
   PromptDialogLabel = XtNewString(
         ((char *)Dt11GETMESSAGE(2, 5, "Please enter the following information:")));
   ContinueMessage = XtNewString(
    ((char *)Dt11GETMESSAGE(2, 6, "You have supplied more parameters than the selected action requires.\n\nSelect 'Ok' to ignore extra parameters.\n\nSelect 'Cancel' to terminate the action.")));
   HostErrorMsg =XtNewString(((char *)Dt11GETMESSAGE(2, 7, "The following host was not accessible:\n\n        ")));

#ifdef _SUN_OS
   HostErrorMsg2 =XtNewString(((char *)Dt11GETMESSAGE(2, 8, "\n\nThis may be because the remote host's file\nsystem is not properly mounted.\n\n")));
#else
   HostErrorMsg2 =XtNewString(((char *)Dt11GETMESSAGE(2, 8, "\n\nCheck that the appropriate remote data access connection\nhas been made.\n\n(See \"The Common Desktop Environment User's Guide\"\nfor more information.)\n")));
#endif
   NoActionMsg =XtNewString(
        ((char *)Dt11GETMESSAGE(2, 9, "Either action \"%s\" was not found\n         or\nthis action does not apply to the file:\n    \"%s\"\nwith data attribute:  \"%s%\"\n\n")));
   NoActionMsg2 = XtNewString(((char *)Dt11GETMESSAGE(2, 10, "Action \"%s\" was not found.\n")));
   InvalidFileMsg = XtNewString(((char *)Dt11GETMESSAGE(2, 11, "The following file was not found:\n\n       ")));
   MapErrorMsg = XtNewString(((char *)Dt11GETMESSAGE(2, 12, "This action cannot be executed because it contains too\nmany levels of MAPs, or the mapping is \"circular\".")));
   MultiHostErrorMsg =XtNewString(((char *)Dt11GETMESSAGE(2,13, "Unable to invoke the requested action.\n\nAre the following hosts accessible?\n     (%s)\nDoes the corresponding program exist?\n(Run " CDE_INSTALLATION_TOP "/bin/dttypes to match actions and programs.)\n\nHas your system run out of room to execute new processes?")));
   IcccmReqErrorMsg = XtNewString(((char *)Dt11GETMESSAGE(2,14,"The request to service this action has failed")));
   NoToolTalkConnMsg = XtNewString(((char *)Dt11GETMESSAGE(2,15,"The request to service this action has failed.\nA ToolTalk connection could not be established:\n\n%s")));
   ToolTalkErrorMsg = XtNewString(((char *)Dt11GETMESSAGE(2,16, "The request to service this action has failed")));
   ToolTalkErrorMsg2 = XtNewString(((char *)Dt11GETMESSAGE(2,17, "The request to service this action has failed for the following reason:\n\n    %s")));
   TtFileArgMapErr = XtNewString((char *)Dt11GETMESSAGE(2,18,"An error occurred while attempting to map one of\nthe file arguments."));
   NoActionMsg3 =XtNewString(
        ((char *)Dt11GETMESSAGE(2, 19, "Either action \"%s\" was not found\n         or\nthis action does not apply to buffers of type:\n  \"%s\"\n\n")));
   UnSupportedObjMsg = XtNewString(
        ((char *)Dt11GETMESSAGE(2, 21, "Unsupported input object class: \"%d\"\nfor action: \"%s\".")));
   TmpFileCreateErrorMsg = XtNewString(
        ((char *)Dt11GETMESSAGE(2, 22, "Unable to create a temporary file in directory: \"%s\"\nfor the action named: \"%s\"")));
   TmpFileOpenErrorMsg = XtNewString(
        ((char *)Dt11GETMESSAGE(2, 23, "Unable to open a temporary file: \"%s\"\nfor the action named: \"%s\"")));
   TmpFileWriteErrorMsg = XtNewString(
        ((char *)Dt11GETMESSAGE(2, 24, "Unable to write a temporary file: \"%s\"\nfor the action named: \"%s\"")));
}

/*
 * This function takes the information supplied by the caller of
 * _DtActionInvoke(), and turns it into an internal format.  This
 * includes parsing out each of the file names, and converting the
 * type from a string to an integer.
 *
 * The structure returned must be freed up eventually.
 */

static ActionRequest * 
CreateActionRequest(
        Widget w,
        String actionName,
	DtActionArg  *aap,
	int aac,
        String termOpts,
	String execHost,
        String cwdHost,
        String cwdDir,
        _DtActInvRecT *invp )

{
   register int i, j;
   int numObjects = 0;
   ObjectData * objectDataArray;
   ObjectData objectData;
   register ActionRequest * request;


   /* Allocate a new request structure -- zero filled */
   request = (ActionRequest *) XtCalloc(1,(Cardinal)sizeof(ActionRequest));

   request->actionName = XtNewString(actionName);

   if (termOpts)
      request->termOpts = XtNewString(termOpts);

   if (execHost) 
      request->execHost = XtNewString(execHost);

   if (cwdHost)
      request->cwdHost = XtNewString(cwdHost);

   if (cwdDir)
      request->cwdDir = XtNewString(cwdDir);

   request->objsUsed = -1;	/* -1 => not yet determined */

   if ( invp )
       request->invocId = invp->id;

   /* If there are no objects, then there's no reason to continue */
   if ((aac <= 0) || (aap == NULL))
      return(request);

   /*
    * Allocate space for all the object data at once
    */
   objectDataArray = (ObjectData *) XtCalloc(aac,(sizeof(ObjectData)));

   /*
    * process object names -- assume all file names are of the form
    *    /path/file  (do NOT allow host:/path/file)
    */
   for ( i = 0; i < aac ; i++ ) 
   {
       memset((void *)&objectData,0,sizeof(ObjectData));
       if ( (aap+i)->argClass == DtACTION_FILE ) 
       {
          if (ParseFileArgument(w, request, &objectData, NULL ,
                            aap[i].u.file.name, NULL , True))
          {
	     XtFree((char *)objectDataArray);
             return(NULL);
          }
       } 
       else if ( (aap+i)->argClass == DtACTION_BUFFER )
       {
           /*
            * Check if we've already created a tmp file for this buffer
            * if so fill in the request structure as if this were a file
            * object.
            */
           if ( invp->info[i].name )
           {
              /*
               * Use the tmp file name and type stored in the invocation rec.
               * The FILE bit will be set in the object mask -- we will also
               * set the BUFFER bit to indicate that this is a tmp file 
               * representing a buffer.
               */
	      if (ParseFileArgument(w, request, &objectData, NULL ,
				invp->info[i].name, invp->info[i].type , True))
	      {
		 XtFree((char *)objectDataArray);
		 return(NULL);
	      }
              /*
               * Set the buffer object bit as well -- and check whether
	       * this buffer is intended to be writable, if not reset the
               * writable bit set in ParseFileArgument().
               */
              SET_BUFFER_OBJ(objectData.mask);
	       if ( !(aap[i].u.buffer.writable) )
		    RESET_WRITE_OBJ(objectData.mask);
               /*
                * Save the buffer type info if we have it
                */
	       if ( aap[i].u.buffer.type )
		    objectData.type = _DtDtsMMStringToBoson(aap[i].u.buffer.type);

               /*
                * Save the original buffer pointer and size for this
                * pseudo-file.
                */
               if ( aap[i].u.buffer.bp )
               {
		   objectData.u.file.bp = aap[i].u.buffer.bp;
		   objectData.u.file.sizebp = aap[i].u.buffer.size;
               }
              
           }
           else
           {
	       /* Do buffer stuff here */
	       SET_BUFFER_OBJ(objectData.mask);
	       if ( aap[i].u.buffer.writable )
		    SET_WRITE_OBJ(objectData.mask);

	       /*
		* If the buffer type has been passed in to us save its quark 
		* in the object structure now.  When/if the type is determined
		* later this object record should be filled in with the
		* necessary quark.
		*/
	       if ( aap[i].u.buffer.type )
		    objectData.type = _DtDtsMMStringToBoson(aap[i].u.buffer.type);
	       else
	       {
		    /*
		     * We have already determined the buffer type when creating
		     * the invocation record. So get the type string
		     * from the invocation record.
		     */
		     myassert(invp->info[i].type);
		     if (invp->info[i].type)
		     {
			objectData.type = _DtDtsMMStringToBoson(invp->info[i].type); 
		     }
		}
		
	       /*
		* Save buffer contents
		*/
	       if ( aap[i].u.buffer.bp )
	       {
		   objectData.u.buffer.size = aap[i].u.buffer.size;
		   objectData.u.buffer.bp = aap[i].u.buffer.bp;
	       } else
	       {
		   myassert(0 /* null buffer pointer */ );
		   objectData.u.buffer.bp = NULL;
		   objectData.u.buffer.size = 0;
	       }
           }
       }

        /* structure assignment */
        objectDataArray[i] = objectData;
	numObjects++;
   }

   request->numObjects = numObjects;
   request->objects    = objectDataArray;

   return(request);
}


/******************************************************************************
 *
 * static _DtActInvRecT *
 * CreateInvocationRecord(actionName,w,aap,aac)
 *	Create an invocation record and fill in argument information
 *      return a pointer to the newly allocated invocation record.
 *
 *****************************************************************************/
static _DtActInvRecT *
CreateInvocationRecord(
        char		*actionName,
        Widget		w,
	DtActionArg	*aap,
	int		aac)
{
    register int	i;
    _DtActInvRecT	*invp;	/* pointer to invocation record */
    char		*tmp;

	/*
	 * allocate invocation record and ID to return to caller
	 */
      invp =  _DtActAllocInvRec();
      if ( !invp )
      {
		/*
                 * RWV --> ideally we would need error message here 
                 *         but if we are unable to allocate the record we
                 *         would in all likelyhood be unable to allocate the
                 *         error message as well.  We will essentially assume
                 *         allocation does not fail as is done throughout the
                 *         library.
                 */
		return NULL;
      }
      myassert(invp->id != 0);

      SET_INV_PENDING(invp->state);
      invp->w = w;
      invp->numChildren = 0;
      invp->childRec = NULL;

      /*
       * Fill in argument information
       */
      invp->ac = aac;

      if (aac == 0) {
 	 invp->info = NULL;

	 return invp;
      }

      invp->info = (_DtActArgInfo *)XtCalloc(aac,sizeof(_DtActArgInfo));

      for ( i=0; i < aac; i++ )
      {
	 if ( aap[i].argClass == DtACTION_BUFFER )
	 {
            int fd;		/* tmp file descriptor */
            char *format;	/* name template (printf format) */
            char *is_executable;	/* IS_EXECUTABLE attribute */
            mode_t mode; 
            int bytesToWrite, bytesWritten;
            int closeAttempts;

	    SET_BUFFER_OBJ(invp->info[i].mask);
	    if ( aap[i].u.buffer.writable )
		SET_WRITE_OBJ(invp->info[i].mask);

            /* save original buffer size */
            invp->info[i].size = aap[i].u.buffer.size;

            /*
             * Determine the type of the buffer object.
             * Typing based on the object name takes precedence
             * over the type "hint".
             */
            if ( aap[i].u.buffer.name 
                 || (aap[i].u.buffer.type == NULL) ) 
            {
		tmp = DtDtsBufferToDataType(
			  aap[i].u.buffer.bp,aap[i].u.buffer.size,
			  aap[i].u.buffer.name);
		/*
		 * Malloc our own copy of the type string so we won't
		 * have to worry about when to call DtDtsFreeDataType() later.
		 */
		invp->info[i].type = XtNewString(tmp);
		DtDtsFreeDataType(tmp);
            } else
            {
		invp->info[i].type = XtNewString(aap[i].u.buffer.type);
            }
	
	    /* 
             * Simply create tmp files for ALL buffers.
             * 
             * This allows us to work around problems related to client
             * programs making subsequent changes to or freeing the memory
             * associated with the buffer before we are through with it.
             *
             * For actions of type CMD we need to have files anyway.
             *
	     * Be sure to create tmp files with
	     * a suffix proper for the buffer type. 
	     */

	   /* first determine the permissions for the new tmp file */
	   is_executable = 
	        DtDtsDataTypeToAttributeValue(invp->info[i].type,
	             _DtActIS_EXECUTABLE,NULL);
	   /*
	    * The tmp file should at LEAST be readable
	    */
	   mode=( S_IRUSR  |  S_IRGRP | S_IROTH );
	   if ( aap[i].u.buffer.writable )
	       mode |= ( S_IWUSR | S_IWGRP | S_IWOTH );
	   if ( is_executable 
	           && DtDtsIsTrue(is_executable) )
	        mode |= ( S_IXUSR | S_IXGRP | S_IXOTH );

	   DtDtsFreeAttributeValue(is_executable);


           if ( aap[i].u.buffer.name )
           {
               /*
                * Attempt to use the name supplied for the buffer.
                */
	       invp->info[i].name = _DtActGenerateTmpFile(NULL,
                        aap[i].u.buffer.name,mode,&fd);
           }
           if ( !invp->info[i].name )
           { 
               /*
                * Generate tmp file based on format supplied for the
                * file type.
                */
	       format = DtDtsDataTypeToAttributeValue(invp->info[i].type,
			 _DtActNAME_TEMPLATE,NULL);

	       invp->info[i].name = _DtActGenerateTmpFile(NULL,format,mode,&fd);
	       DtDtsFreeAttributeValue(format);
           }
           if ( !invp->info[i].name )
           {
                /*
                 * Unable to generate usable tmp file name.
                 */
	        /* 
		 * Error message makes assertion message redundant.
		 * myassert(invp->info[i].name);
		 */
	        TmpFileCreateError(w,actionName,_DtGetDtTmpDir());

		RESET_INV_PENDING(invp->state);
		SET_INV_ERROR(invp->state);
		SET_INV_CANCEL(invp->state);

                return invp;
           }    

	   /*
	    * Write contents of buffer to temp file
	    */
	   myassert( fd >= 0 );
	   for ( bytesToWrite = aap[i].u.buffer.size, bytesWritten = 0;
		  bytesToWrite > 0;
		  bytesToWrite -= bytesWritten)
	   {
		bytesWritten = write(fd,aap[i].u.buffer.bp,bytesToWrite);
		if ( bytesWritten < 0 ) 
		{
		    if (errno == EINTR )
		    {
			    bytesWritten = 0;
			    continue;
		    } 
		    else
		    {
			myassert(0  /* Unrecoverable Write Error */);
			TmpFileWriteError(w,actionName,invp->info[i].name);

			close(fd);
			(void) unlink(invp->info[i].name);

			RESET_INV_PENDING(invp->state);
			SET_INV_ERROR(invp->state);
			SET_INV_CANCEL(invp->state);

			return invp;
		    }
		}
	    }
	    
	    closeAttempts = 0;
	    while ( close(fd) )
	    {
	     /* error closing fd */
		if ( closeAttempts > _DT_ACTION_MAX_CLOSE_TRIES )
		    break;

		switch ( errno )
		{
		    case EBADF:  /* invalid fd */
			myassert( 0 );
			break;
		    case EINTR:  /* interrupted sys call */
			closeAttempts++;
			continue;	/* try again */
		    case ENOSPC: /* Not enough space on NFS-mounted dev */
			TmpFileWriteError(w,actionName,
				       invp->info[i].name);

			unlink(invp->info[i].name);
			RESET_INV_PENDING(invp->state);
			SET_INV_ERROR(invp->state);
			SET_INV_CANCEL(invp->state);

			/* try another close */
			if ( close(fd) ) 
			{
			    /* It should have worked this time */
			    myassert(0);
			}
			return invp;
		    default: /* anything else */
			myassert(0);
			break;
		}
		break;  /* only try again for conditions with continue */
	    }
	    

	   /*
	    * Now that we have created a tmp file for this buffer
	    * object set the FILE_OBJ flag as well as the buffer flag.
	    * Objects with both the BUFFER and FILE flags set will be
	    * recognized as buffers which have been written to tmp files.
	    */
	    
	    SET_FILE_OBJ(invp->info[i].mask);

	 }
	 else if ( aap[i].argClass == DtACTION_FILE )
	 {
	    invp->info[i].name = XtNewString(aap[i].u.file.name);

	    SET_FILE_OBJ(invp->info[i].mask);
	    SET_WRITE_OBJ(invp->info[i].mask);
	 }
	 else
         {
	    myassert( 0 /* unsupported object */ );
            UnSupportedObject(w, actionName, aap[i].argClass);

	    RESET_INV_PENDING(invp->state);
	    SET_INV_ERROR(invp->state);
	    SET_INV_CANCEL(invp->state);

            return invp;
         }
      }

      return invp;
}



static Boolean 
ParseFileArgument(
        Widget w,
        ActionRequest * request,
        ObjectData * objectData,
        String hostname,
        String filename,
        String filetype,
        Boolean typeFile )

{
   register int i, j;
   String dirName;
   String host;
   String dir;
   int hostId;
   char *resolvedPath=NULL;

/********************************************************************
	WE NO LONGER ACCEPT host:/path FORMAT
   if (host = _DtHostString(filename))
   {
      hostId = _DtAddEntry(host, &request->hostNames, &request->numHostNames);
      XtFree(host);
   }
   else 
********************************************************************/
   if ( hostname )
   {
      hostId = _DtAddEntry(hostname, &request->hostNames, 
                           &request->numHostNames);
   }
   else
   {
      if ( request->cwdHost != NULL ) 
      {
         hostId = _DtAddEntry(request->cwdHost, &request->hostNames, 
                              &request->numHostNames);
      }
      else
      {
         /* if all else fails use local host */
         host = _DtGetLocalHostName();
         hostId = _DtAddEntry(host, &request->hostNames, 
                              &request->numHostNames);
         XtFree(host);
      }
   }

   objectData->u.file.origFilename = XtNewString(filename);
   objectData->u.file.origHostname = XtNewString(hostname);
   objectData->u.file.hostIndex = hostId;
   objectData->u.file.baseFilename = _DtBasename(filename);
   objectData->type = -1;

   /* Hash the directory name */
   if ( (dirName  = _DtDirname(filename)) == NULL )
   {
      if ( request->cwdDir )
         dirName = XtNewString(request->cwdDir);
      else
      {
         /* Default to current directory */
         dirName = _DtFindCwd();
      }
   }
   else if ( dirName[0] != '/' )
   {
      /*
       * We have been provided with a relative path name
       * interpret it relative to the context directory.
       */
      String tmpName;

      if ( request->cwdDir )
         tmpName=XtNewString(request->cwdDir);
      else
         tmpName=_DtFindCwd();
		
      tmpName=XtRealloc(tmpName,strlen(tmpName)+strlen(dirName) +2);
      (void)strcat(tmpName,"/");
      (void)strcat(tmpName,dirName);
      XtFree(dirName);
      dirName=tmpName;
   }
	  
   if ( objectData->u.file.baseFilename == NULL || dirName == NULL )
   {
      /* Invalidly formed file name */
      InvalidFilename(w, request->clonedAction->label, filename);
      _DtFreeRequest (request);
      XtFree(dirName);
      XtFree(objectData->u.file.origFilename);
      XtFree(objectData->u.file.origHostname);
      XtFree(objectData->u.file.baseFilename);
      return(True);
   }

   objectData->u.file.dirIndex = _DtAddEntry(dirName, &request->dirNames,
					     &request->numDirNames);
   SET_UNKNOWN_IF_DIR(objectData->mask);
   SET_FILE_OBJ(objectData->mask);
   /*
    * default file objects are treated as writable/returned objects.
    */
   SET_WRITE_OBJ(objectData->mask);

   /*
    * If a type has been provided for this file -- use it.
    * otherwise -- look up the type.
    */
   if (typeFile)
   {
      if ( filetype && *filetype )
         objectData->type = _DtDtsMMStringToBoson(filetype);
      else
      {
         TryToTypeFile(objectData,request->hostNames[hostId],
 		      dirName, objectData->u.file.baseFilename,
		      &resolvedPath);

         if ( resolvedPath )
         {
		struct stat sbuf;
		if ( !stat(resolvedPath,&sbuf) )
		{
			/* successful stat of file -- check permissions */
                      if ( !( sbuf.st_mode&S_IWOTH 
                           || sbuf.st_mode&S_IWGRP 
                           || sbuf.st_mode&S_IWUSR) )
	              {
			     RESET_WRITE_OBJ(objectData->mask);
                      }
		}
	 }
	
      }
   }
   XtFree(resolvedPath);
   XtFree(dirName);
   return(False);
}


/*
 * Returns a string representing the current working directory
 * for this process. This string must be freed up by the caller.
 * NOTE:This function does not replace sym_links with real paths.
 *      This may be useful on networks where nfs mounts and symbolic
 *	consistently named symbolic links are used to give the
 *	impression of a single large network file system.
 */

String 
_DtFindCwd( void )

{
	String tmp = 0;
	char buf[MAXPATHLEN + 1];

	if ((tmp = getcwd(buf, MAXPATHLEN)) == NULL)
	{
		_DtSimpleError(
			DtProgName, DtError, NULL,
			"getcwd(): unable to get current directory", NULL);
		tmp = "/";
	}
	return (XtNewString(tmp));
}


/*
 * Generic function which checks to see if the specified string is
 * already entered in the passed-in array; if so, then it will return
 * the index of the existing entry within the array; if not, then it
 * will grow the array, add the string into it, and then return the
 * new index.
 */

static int 
_DtAddEntry(
        register String string,
        register String **arrayPtr,
        register int *sizePtr )

{
   register int i;

   /* See if the string is already in the array */
   for (i = 0; i < *sizePtr; i++)
   {
      if (strcmp(string, (*arrayPtr)[i]) == 0)
         return(i);
   }

   /* Add the string */
   i = *sizePtr;
   (*sizePtr)++;
   (*arrayPtr) = (String *)XtRealloc((String)*arrayPtr, 
                           (Cardinal)(sizeof(String) * (*sizePtr)));
   (*arrayPtr)[i] = XtNewString(string);
   return(i);
}


/*
 * This function will type the indicated file, only if it is the first
 * parameter file; this is to improve performance, since in many cases,
 * only the first argument is used to 'type' the action, and the others
 * never need to be 'typed'.
 */

static void 
TryToTypeFile(
        ObjectData *obj,
        char * host,
        char * dir,
        char * file,
        char **resolvedPath )

{
   /* Follow the link when typing files */
   obj->type = LinkToTypeQuark(host, dir, file, resolvedPath);
}


/*
 * Given a file, follow any links, and base the filetype off of the
 * final file, not the link we are passed.
 */

static int 
LinkToTypeQuark(
        char * host,
        char * dir,
        char * file,
	char **resolvedPath )

{
   char * path;
   char link_path[MAXPATHLEN + 1];
   char file_name[MAXPATHLEN + 1];
   int link_len;
   char * end;
   int history_count;
   int history_size;
   char ** history;
   int i;
   char * dtype;
   DtShmBoson dquark;

   /* Used to check for symbolic link loops */
   history_count = 0;
   history_size = 100;
   history = (char **)XtMalloc(sizeof(char *) * history_size);

   path = _DtActMapFileName(host, dir, file, NULL);
   if (path == NULL)
   {
      *resolvedPath=NULL;
      return(-1);
   }
   strcpy(file_name, path);
   XtFree(path);

   while ((link_len = readlink(file_name, link_path, MAXPATHLEN)) > 0)
   {
      link_path[link_len] = '\0';

      /* Force the link to be an absolute path, if necessary */
      if (link_path[0] != '/')
      {
         /* Relative paths are relative to the current directory */
         end = DtStrrchr(file_name, '/') + 1;
         *end = '\0';
         strcat(file_name, link_path);
      }
      else
         strcpy(file_name, link_path);

      /* Check for a recursive loop; abort if found */
      for (i = 0; i < history_count; i++)
      {
         if (strcmp(file_name, history[i]) == 0)
         {
            /* Drop back to last non-recursive portion of the path */
            strcpy(file_name, history[history_count-1]);
            for (i = 0; i < history_count; i++)
               XtFree(history[i]);
            XtFree((char *)history);
            dtype = DtDtsFileToDataType(file_name);
            dquark = _DtDtsMMStringToBoson(dtype);
            DtDtsFreeDataType(dtype);
            *resolvedPath = XtNewString(file_name);
            return(dquark);
         }
      }

      /* Add to the history list */
      if (history_count >= history_size)
      {
         history_size += 100;
         history = (char **)XtRealloc((char *)history, 
                                      sizeof(char *) * history_size);
      }
      history[history_count++] = XtNewString(file_name);
   }

   /* Free up the history list */
   for (i = 0; i < history_count; i++)
      XtFree(history[i]);
   XtFree((char *)history);

   dtype = DtDtsFileToDataType(file_name);
   dquark = _DtDtsMMStringToBoson(dtype);
   DtDtsFreeDataType(dtype);
   *resolvedPath = XtNewString(file_name);
   return(dquark);
}


/*
 * Given a request, find the action to which it maps, and see if enough
 * parameters were supplied to allow the action to be started.  It's
 * possible we may need to bring up a dialog to collect more data, or
 * we may need to invoke multiple actions.
 *
 * The first time an action request is processed, we will check the
 * parameter situation, and will prompt the user, if necessary.  The
 * second time the action request is processed (typically when the
 * user closes the parameter collecting dialog), we will simply invoke
 * the action with whatever we have; the user will not be prompted a
 * second time for any missing parameters.
 *
 * If the request is processed (True is returned), then it is up to the
 * caller to free up the request structure.
 */

static Boolean 
ProcessRequest(
        Widget w,
        register ActionRequest *request )

{
   int unused;
   register ActionPtr action;
   int numPrompts;
   PromptEntry * prompts;
   DtShmBoson actionQuark;
   Tt_status status = TT_OK;

   /* See if this is the first pass for the request */
   if (request->clonedAction == NULL)
   {

      /* Always start with the first host, when processing a request */
      request->hostIndex = 0;

      /* Find the action DB entry which we map to */
      actionQuark = _DtDtsMMStringToBoson(request->actionName);
      RESET_TOO_MANY_MAPS(request->mask);

      if (actionQuark == -1 || (action = _DtActionFindDBEntry(request, actionQuark)) == NULL) 
      {
          /*
           * No action label is available here for error dialogs
           */
         if (IS_TOO_MANY_MAPS(request->mask))
	 {
	    MapError (w, request->actionName);

	 }
         else if (request->numObjects > 0)
         {
            if (IS_FILE_OBJ(request->objects[0].mask))
            {
               NoActionError(w, actionQuark, 
		 request->actionName, 
                 (char *)_DtDtsMMBosonToString(request->objects[0].type),
                 request->hostNames[request->objects[0].u.file.hostIndex],
                 request->dirNames[request->objects[0].u.file.dirIndex],
                 request->objects[0].u.file.baseFilename);
            }
            else if ( IS_BUFFER_OBJ(request->objects[0].mask) )
            {
               /*
                * RWV -- may have to modify this call to generate a 
                * message more suitable for buffer objects.
                */
               NoActionError(w, actionQuark,
		 request->actionName, 
                 (char *)_DtDtsMMBosonToString(request->objects[0].type),
                 NULL,	/* host */
                 NULL,	/* dir */
                 "Memory Object"   /* filename */);
            } else
                myassert(0 /* should never get here */ );

            /* fdt: add code for  strings
             * else if (IS_STRING_OBJ(request->objects[0].mask))
            */
         }
         else
            NoActionError(w, actionQuark, request->actionName, 
		NULL, NULL, NULL, NULL);

         /*
          * If we are in the middle of reprocessing a single argument
          * action, then continue with the next parameter.  Otherwise,
          * this error terminates the request, so return.
          */
         if (IS_REPROCESSING(request->mask) && MoreArgumentsToProcess(request))
            return(ProcessRequest(w, request));

	 /*
	  * We were never able to start this action.
	  */
	 {
	    _DtActInvRecT *invRecP = _DtActFindInvRec(request->invocId);
	    if (invRecP) SET_INV_ERROR(invRecP->state);
	 }
         return(True);
      }

      request->clonedAction = action;

      /*
       * If this is a ToolTalk message, then before proceeding any further,
       * make sure we can get connected to a ToolTalk session.  If we can't,
       * then we need to bail out.
       */
      if (IS_TT_MSG(action->mask) &&
	  (status = _DtInitializeToolTalk(NULL)) != TT_OK)
      {
         NoToolTalkConnectionError(w, request->clonedAction->label, status);
	 {
	    _DtActInvRecT *invRecP = _DtActFindInvRec(request->invocId);
	    if (invRecP) SET_INV_ERROR(invRecP->state);
	 }
         return(True);
      }

      /* Determine how we are sitting with parameters */
      unused = MatchParamsToAction(request, &numPrompts, &prompts);
      request->objsUsed = request->numObjects - unused;
      myassert(request->objsUsed >= 0);

      /* 
       * Do we need to create a prompt dialog? 
       * NOTE: if the action requires the user to be prompted, but the
       *       user has supplied extra parameters, so he will be asked
       *       to abort or continue, do the abort/continue dialog BEFORE
       *       the prompt dialog; there's little sense in collecting
       *       additional input if the user is going to abort the action!
       */
      if ((prompts != NULL) &&
          ((unused == 0) || IS_ARG_SINGLE_ARG(action->mask) || 
            IS_ARG_NONE_FOUND(action->mask)))
      {
         CreatePromptDialog(w, request, numPrompts, prompts);
         XtFree((char *)prompts);
         return(False);
      }

      /* Were too many parameters supplied? */
      else if (unused > 0)
      {
         /* 
          * If the action only needs a single parameter, then we need
          * to fire off multiple instances of the action; otherwise,
          * prompt the user to continue or abort.  An action requiring
          * no parameters is also treated like a single parameter action.
          */
         if (IS_ARG_SINGLE_ARG(action->mask) || IS_ARG_NONE_FOUND(action->mask))
         {

            PrepareAndExecuteAction(w, request);

            /* See if there are still more parameters to be processed */
            if (MoreArgumentsToProcess(request))
               return(ProcessRequest(w, request));
         }
         else
         {
            /* 
             * Postpone any further processing until the user either
             * tells us to continue, or abort.
             */
            CreateContinueDialog(w, request, numPrompts, prompts);
            XtFree((char *)prompts);
            return(False);
         }
      }
      else
      {
           PrepareAndExecuteAction(w, request);
       }
   }
   else
   {

      PrepareAndExecuteAction(w, request);
      action = request->clonedAction;

      /* 
       * If this is a single argument action, and we have more parameters
       * waiting to be processed, then continue processing them.
       */
      if ((IS_ARG_SINGLE_ARG(action->mask) || IS_ARG_NONE_FOUND(action->mask))
          && (MoreArgumentsToProcess(request)))
      {
         return(ProcessRequest(w, request));
      }
   }

   return(True);
}


/*
 * This function is called at the point where we have collected all of the
 * information needed to actually initiate the action.  We will use the
 * set of arguments passed into _DtActionInvoke(), along with any values
 * supplied through the prompt dialog.  It is also at this point that
 * the thread of control will split, dependent upon the type of action
 * being executed (Command Invoker, Tooltalk).
 */
static void 
PrepareAndExecuteAction(
        Widget w,
        register ActionRequest *request )

{
   char * relPathHost;
   char * relPathDir;
   register int i;
   register ActionPtr action = request->clonedAction;
   int     argNum;
   _DtActInvRecT *invp;	/* pointer to invocation record */
   _DtActChildRecT *childrecp;	/* pointer to child record */

  /*
   * We have gathered all the information necessary to invoke
   * this action  all dialogs have been posted and processed.
   * Now  create the action invocation record -- unless we are
   * in the midst of reprocessing an already invoked action.
   */
   invp = _DtActFindInvRec(request->invocId);
   myassert(invp);
   SET_INV_WORKING(invp->state);

   /*
    * Allocate a child rec -- fill it in
    */
   if ( (childrecp = _DtActAllocChildRec(invp)) != NULL )
   {
	request->childId = childrecp->childId;

	childrecp->childState = _DtActCHILD_PENDING_START;
        childrecp->mask = action->mask;

        childrecp->numObjects = request->objsUsed;
   } 
   else
       myassert( 0 /* Unable to allocate childRec */ );


   /*
    * Before proceeding, we need to determine what host and directory
    * will be used when resolving relative pathnames.
    */
   __ExtractCWD(request, &relPathHost, &relPathDir, False);
   if (IS_CMD(action->mask))
   {
      /*
       * All buffer objects must be placed into temporary files for
       * command actions. This has already been done when the
       * request structure was created.
       */
      if (childrecp && childrecp->numObjects > 0)
      {
	   childrecp->argMap =
               (_DtActArgMap *)XtCalloc(childrecp->numObjects,
                     sizeof(_DtActArgMap));

           for ( i = 0; i < childrecp->numObjects && i < invp->ac; i++ )
           {
	        childrecp->argMap[i].argN = i+1; /*  ignored for CMD actions */
	        childrecp->argMap[i].argIdx = 
                    i + request->objOffset;      /* idx into invp->info[] */
           }
      }
      ProcessCommandInvokerRequest(w, request, relPathHost, relPathDir);
   }
   else if (IS_TT_MSG(action->mask))
   {
      if (childrecp)
      {
	 /*
          * create argmap for returnable arguments -- 
          * i.e. those appearing in TT_ARGn_VALUE fields.
          *
          * The requirement is that one and only one action argument may
          * appear in a TT_ARGn_VALUE field.
          *
          * argMap is a sparse array which maps TT_ARGn_VALUEs to input
          * parameters.  If a TT_ARGn_VALUE does not have an input parameter
          * as a value then the sentinel value "-1" is provided as the index.
          * Allocate enough space for all the TT_ARGn_VALUEs plus one for
          * TT_FILE.
          *
          * The elements of the argMap array then represent:
          * argMap[ TT_ARG0, TT_ARG1, ...,TT_ARGN, TT_FILE]
          */

	  childrecp->argMap =
	      (_DtActArgMap *)XtCalloc( action->u.tt_msg.value_count + 1,
		   sizeof(_DtActArgMap));
   
	  for ( i = 0; i < action->u.tt_msg.value_count; i++)
	  {
              /*
               * Set index value to "-1".  This value will indicate 
               * TT_ARGn_VALUES which are NOT associated with input
               * parameters (action arguments).  If there is an action
               * argument associated with this TT_ARGn_VALUE we will set
               * it below.
               */
	      childrecp->argMap[i].argIdx = -1; 
	      childrecp->argMap[i].argN = i; 

		/* null argn value is valid -- so check MsgParts*/
	      if (!action->u.tt_msg.tt_argn_value[i].numMsgParts)
		  continue;

	      if (action->
		      u.tt_msg.tt_argn_value[i].parsedMessage[0].keyword 
			  != ARG)
		  continue;

	      /*
	       * TT_ARGn_VALUE fields should have only one arg keyword.
	       */
	      myassert(action->u.tt_msg.tt_argn_value[i].numMsgParts == 1);
	      argNum =action->
			 u.tt_msg.tt_argn_value[i].parsedMessage[0].argNum;

	      if ( ( argNum > 0 ) && ( argNum <= invp->ac ) )
	      {
		  /* The ith message part must be returned */
		  childrecp->argMap[i].argIdx = 
			argNum + request->objOffset - 1;
                  myassert( childrecp->argMap[i].argIdx >= 0 );
	      }
	   }
           /*
            * Add an argMap entry for the value of the TT_FILE field.
            * Tooltalk (e.g. media messages) sometimes uses this field
            * to pass values such as file names to the message receipient.
            * If the TT_FILE field has a single ARG keyword
            * then record that parameter number otherwise record "-1" as
            * was done for the value arguments above.
            */
	   childrecp->argMap[i].argIdx = -1; 
	   childrecp->argMap[i].argN = -1;   /* Use "-1" as TT_FILE entry idx */
           if (action->u.tt_msg.tt_file.numMsgParts
                  && action->u.tt_msg.tt_file.parsedMessage[0].keyword == ARG )
           {
	      argNum =action->
			 u.tt_msg.tt_file.parsedMessage[0].argNum;
	      if ( ( argNum > 0 ) && ( argNum <= invp->ac ) )
	      {
		  /* The ith message part should be the last argMap entry */
		  childrecp->argMap[i].argIdx = 
			argNum + request->objOffset - 1;
                  myassert( childrecp->argMap[i].argIdx >= 0 );
	      }
           }

      }
      _DtProcessTtRequest(w, request, relPathHost, relPathDir);
   }

   /*
    * For now we are through invoking this child.
    * There may still be more children to invoke or we may have to
    * re-invoke this child (e.g. multi-host processing for commands).
    */
   SET_INV_DONE(invp->state);

   /* Free up the path information */
   XtFree(relPathHost);
   XtFree(relPathDir);
}


/*
 * Determine the CWD to use; this information can be used to both
 * resolve relative filepaths, and to set the CWD used when executing
 * a command invoker request.  When resolving relative paths, the
 * information specified for the first argument is not used (see case 2
 * below).  It is determined using the following algorithm:
 *
 *     1) Use the CWD specified in the action (if a cmd invoker action).
 *     2) If told to use the objects, then use the directory where the
 *        object lives (if a regular file), or the object itself (if
 *        it's a directory.
 *     3) Use the CWD passed into _) by the application.
 *     4) Use the physical CWD of the application.
 *
 * Both the host and directory paths must by freed by the caller.
 */
static void 
__ExtractCWD(
        register ActionRequest *request,
        char ** hostPtr,
        char ** dirPtr,
        Boolean useObjectInfo )

{
   String msg;
   String lastCh;
   int lastChLen;
   register ActionPtr action = request->clonedAction;

   /* Only dropped objects will have been 'typed' at this point */
   if (useObjectInfo && (IS_CMD(action->mask)) && (request->numObjects > 0) && 
       (request->objects[0].type >= 0) && 
        IS_FILE_OBJ(request->objects[0].mask))
   {
      if (action->u.cmd.contextHost != NULL)
	 *hostPtr = XtNewString(action->u.cmd.contextHost);
      else
      {
	 *hostPtr = XtNewString(
                     request->hostNames[request->objects[0].u.file.hostIndex]);
      }

      if (IS_UNKNOWN_IF_DIR(request->objects[0].mask))
      {
         String nfsPath;
	 char *theHost, *theDir;
         struct stat statInfo;

         RESET_UNKNOWN_IF_DIR(request->objects[0].mask);

         /* 
          * The file may not have been checked yet, if it was never
          * referenced in the execution string; so .. we'll check 
          * here.
          */
	 theHost = request->hostNames[request->objects[0].u.file.hostIndex];
	 theDir = request->dirNames[request->objects[0].u.file.dirIndex];
         nfsPath = _DtActMapFileName(theHost, theDir, 
                               request->objects[0].u.file.baseFilename, NULL);

         if (nfsPath && (stat(nfsPath, &statInfo) == 0) &&
             ((statInfo.st_mode & S_IFMT) == S_IFDIR))
         {
            SET_DIR_OBJ(request->objects[0].mask);
         }
         XtFree(nfsPath);
      }

      if (IS_DIR_OBJ(request->objects[0].mask))
      {
	 if (action->u.cmd.contextDir != NULL)
            *dirPtr = XtNewString(action->u.cmd.contextDir);
	 else 
	 {
	    *dirPtr = XtMalloc((Cardinal)
	       (strlen(request->dirNames[request->objects[0].u.file.dirIndex]) +
		strlen(request->objects[0].u.file.baseFilename) + 2));
	    strcpy(*dirPtr, 
                   request->dirNames[request->objects[0].u.file.dirIndex]);

	    DtLastChar(*dirPtr, &lastCh, &lastChLen);
	    if ((lastChLen != 1) || (*lastCh != '/'))
	       (void)strcat(*dirPtr, "/");

	    (void)strcat(*dirPtr, request->objects[0].u.file.baseFilename);
	 }
      }
      else
      {
	 if (action->u.cmd.contextDir != NULL)
	    *dirPtr = XtNewString(action->u.cmd.contextDir);
	 else
         {
	    *dirPtr = XtNewString(
                        request->dirNames[request->objects[0].u.file.dirIndex]);
         }
      }
   }
   else 
   {
      /* Use specified context, or get process context, if necessary */
      if (IS_CMD(action->mask) && (action->u.cmd.contextHost != NULL))
	 *hostPtr = XtNewString(action->u.cmd.contextHost);
      else if (request->cwdHost)
         *hostPtr = XtNewString(request->cwdHost);
      else
      {
	 *hostPtr = _DtGetLocalHostName();
      }

      if (IS_CMD(action->mask) && (action->u.cmd.contextDir != NULL))
	 *dirPtr = XtNewString(action->u.cmd.contextDir);
      else if (request->cwdDir)
         *dirPtr = XtNewString(request->cwdDir);
      else
         *dirPtr = _DtFindCwd();
   }
}


/*
 * This function is used to prepare for the continued processing of
 * the parameters, when the action is a single argument action.  It
 * free up any data which was associated with the previous parameter,
 * and cascades up any remaining parameters in the object array.
 */

static Boolean 
MoreArgumentsToProcess(
        register ActionRequest *request )

{
   register int i;
   char * path;
   char * dtype;

   if (request->numObjects <= 1)
   {
      return(False);
   }
   else
   {
      /* Repeat processing for the next argument */

      /* Cascade up the remaining, unprocess parameters */
      if (IS_FILE_OBJ(request->objects[0].mask))
      {
         XtFree(request->objects[0].u.file.origFilename);
         XtFree(request->objects[0].u.file.origHostname);
         XtFree(request->objects[0].u.file.baseFilename);
      }
     /* 
      * RWV: 
      * Since we use tmp files for buffers and do not support
      * strings; we need not add special code for buffer and
      * string support.
      */
      /* fdt: Add support for strings and buffers here
       * else if (IS_BUFFER_OBJ(request->objects[0].mask))
       *    XtFree(request->objects[0].u.buffer.buffer);
       * else if (IS_STRING_OBJ(request->objects[0].mask))
       *    XtFree(request->objects[0].u.string.string);
       */
      for (i = 0; i < (request->numObjects - 1); i++)
      {
         request->objects[i] = request->objects[i+1];
      }
      request->numObjects--;
      request->objOffset++; 
      request->objsUsed = 0;
      request->childId = 0;

      /* Free up our previously cloned action */
      _DtFreeActionStruct(request->clonedAction);
      request->clonedAction = NULL;

      /* Free up any leftover prompt strings */
      for (i = 0; i < request->numPromptInputs; i++)
         XtFree(request->promptInputs[i]);
      XtFree((char *)request->promptInputs);
      request->promptInputs = NULL;
      request->numPromptInputs = 0;
      SET_REPROCESSING(request->mask);
      XtFree(request->badHostList);
      request->badHostList = NULL;
      XtFree(request->currentHost);
      request->currentHost = NULL;
      request->hostIndex = 0;

      /* Type the object, if possible */
      if (IS_FILE_OBJ(request->objects[0].mask))
      {
         if ((request->objects[0].u.file.hostIndex >= 0) &&
             (request->objects[0].u.file.dirIndex >= 0))
         {
            path = _DtActMapFileName(
                 request->hostNames[request->objects[0].u.file.hostIndex], 
                 request->dirNames[request->objects[0].u.file.dirIndex], 
                 request->objects[0].u.file.baseFilename, NULL);
            dtype = DtDtsFileToDataType(path);
            request->objects[0].type = _DtDtsMMStringToBoson(dtype);
            DtDtsFreeDataType(dtype);
            XtFree(path);
         }
      }
     /* 
      * RWV: 
      * Since we use tmp files for buffers and do not support
      * strings; we need not add special code for buffer and
      * string support.
      */
      /* fdt: add support for buffers and strings here
       * else if (IS_BUFFER_OBJ(request->objects[0].mask))
       * {
       * }
       * else if (IS_STRING_OBJ(request->objects[0].mask))
       * {
       * }
       */

      return(True);
   }
}


/***************************************************************************/
/***************************************************************************/
/*              Functions For Cloning And Free Structures                  */
/***************************************************************************/
/***************************************************************************/


/*
 * At the point that a request is sent, we need to save a copy of the 
 * request, for future reference.  We need to clone the request, since 
 * the original request structure may get modified between the time we 
 * send the message, and the time we need to reference it in the future.
 */

ActionRequest * 
_DtCloneRequest (
   ActionRequest * request)

{
   ActionRequest * newRequest;
   int i;

   newRequest = (ActionRequest *)XtMalloc((Cardinal)sizeof(ActionRequest));

   /*  
    * Structure assignment to clone all scalar values 
    * If a value is not explicitly set then it defaults to the same value
    * as in the original request.  Pointers to malloc-ed memory should all
    * be replaced with pointers to a copy of the region.
    */
   (*newRequest) = (*request);

   newRequest->actionName = XtNewString(request->actionName);

   if (request->numObjects > 0) {
     newRequest->objects = (ObjectData *)XtMalloc(sizeof(ObjectData) *
						  request->numObjects);

     for (i = 0; i < request->numObjects; i++)
     {
       newRequest->objects[i] = request->objects[i];
       if (IS_FILE_OBJ(request->objects[i].mask))
       {
	 newRequest->objects[i].u.file.origFilename = 
	   XtNewString(request->objects[i].u.file.origFilename);
	 newRequest->objects[i].u.file.origHostname = 
	   XtNewString(request->objects[i].u.file.origHostname);
	 newRequest->objects[i].u.file.baseFilename = 
	   XtNewString(request->objects[i].u.file.baseFilename);
       } 
       else if ( IS_BUFFER_OBJ(request->objects[i].mask) )
       {
	 /* 
	  * RWV:
	  * Since we are creating tmp files for all buffers
	  * we should never have to copy a buffer's contents.
	  *
	  * We should never reach this code because the FILE_OBJ
	  * bit is set when we create tmp files for buffers.
	  */
	 myassert(0);
	 /*  
	  * RWV:
	  * Can we get by without copying buffer object contents?
	  * if so  -- how do we avoid freeing it twice OR
	  * not freeing it at all?
	  */
	 /* make a copy of the buffer */
	 if ( request->objects[i].u.buffer.bp )
         {
	   myassert(newRequest->objects[i].u.buffer.size == request->objects[i].u.buffer.size);
	   newRequest->objects[i].u.buffer.bp =
	     XtMalloc( request->objects[i].u.buffer.size );
	   memcpy(newRequest->objects[i].u.buffer.bp,
		  request->objects[i].u.buffer.bp,
		  newRequest->objects[i].u.buffer.size);
	 }
       }
       else
	 myassert(0 /* no other object types supported */ );
     }
   }

   newRequest->numPromptInputs = request->numPromptInputs;
   if (request->numPromptInputs > 0) {
     newRequest->promptInputs = (char **)XtMalloc(sizeof(char *) * 
						  request->numPromptInputs);
     for (i = 0; i < request->numPromptInputs; i++)
       newRequest->promptInputs[i] = XtNewString(request->promptInputs[i]);
   }

   newRequest->numHostNames = request->numHostNames;
   if (request->numHostNames > 0) {
     newRequest->hostNames = (char **)XtMalloc(sizeof(char *) * 
					       request->numHostNames);
     for (i = 0; i < request->numHostNames; i++)
       newRequest->hostNames[i] = XtNewString(request->hostNames[i]);
   }

   newRequest->numDirNames = request->numDirNames;
   if (request->numDirNames > 0) {
     newRequest->dirNames = (char **)XtMalloc(sizeof(char *) * 
					      request->numDirNames);
     for (i = 0; i < request->numDirNames; i++)
       newRequest->dirNames[i] = XtNewString(request->dirNames[i]);
   }

   newRequest->termOpts = XtNewString(request->termOpts);
   newRequest->cwdHost = XtNewString(request->cwdHost);
   newRequest->cwdDir = XtNewString(request->cwdDir);

   if (request->clonedAction)
      newRequest->clonedAction = CloneActionDBEntry(request->clonedAction);
   else
      newRequest->clonedAction = NULL;

   newRequest->badHostList = XtNewString(request->badHostList);
   newRequest->currentHost = XtNewString(request->currentHost);
   newRequest->execHost = XtNewString(request->execHost);


   return(newRequest);
}


/*
 * Free up the contents of a request structure
 */

void 
_DtFreeRequest(
        register ActionRequest *request )

{
   register int i;

   XtFree(request->actionName);

   for (i = 0; i < request->numObjects; i++)
   {
      if (IS_FILE_OBJ(request->objects[i].mask))
      {
         XtFree(request->objects[i].u.file.origFilename);
         XtFree(request->objects[i].u.file.origHostname);
         XtFree(request->objects[i].u.file.baseFilename);
      }
     /* 
      * RWV: 
      * Since we use tmp files for buffers and do not support
      * strings; we need not add special code for buffer and
      * string support.
      */
      /* fdt: Add support for buffers and strings here
       * else if (IS_BUFFER_OBJ(request->objects[i].mask)
       *   XtFree(request->objects[i].u.buffer.buffer);
       * else if (IS_STRING_OBJ(request->objects[i].mask)
       *   XtFree(request->objects[i].u.string.string);
       */
   }

   /*
    * Since the objectDataArray was malloced at once
    * we can free it at once.
    */
   if (request->objects) XtFree((char *)request->objects);

   for (i = 0; i < request->numPromptInputs; i++)
      XtFree(request->promptInputs[i]);
   if (request->promptInputs) XtFree((char *)request->promptInputs);

   for (i = 0; i < request->numHostNames; i++)
      XtFree(request->hostNames[i]);
   if (request->hostNames) XtFree((char *)request->hostNames);

   for (i = 0; i < request->numDirNames; i++)
      XtFree(request->dirNames[i]);
   if (request->dirNames) XtFree((char *)request->dirNames);

   XtFree(request->termOpts);
   XtFree(request->cwdHost);
   XtFree(request->cwdDir);
   _DtFreeActionStruct(request->clonedAction);
   XtFree(request->badHostList);
   XtFree(request->currentHost);
   XtFree(request->execHost);

   XtFree ((char *)request);
}


/*
 * Create a clone of an action DB entry
 */

static ActionPtr 
CloneActionDBEntry(
        register ActionPtr action )

{
   register ActionPtr newAction = (ActionPtr)XtMalloc((Cardinal)sizeof(Action));
   register int i;

   /* Clone each field */
   newAction->action = action->action;
   newAction->file_name_id = action->file_name_id;
   newAction->label = XtNewString(action->label);
   newAction->description = XtNewString(action->description);

   newAction->type_count = action->type_count;
   if (action->type_count > 0) {
     newAction->arg_types = (DtShmBoson *)XtMalloc(sizeof(DtShmBoson) *
						   newAction->type_count);
     for (i = 0; i < newAction->type_count; i++)
       newAction->arg_types[i] = action->arg_types[i];
   }
   else {
     newAction->arg_types = NULL;
   }

   newAction->arg_count = action->arg_count;
   newAction->mask = action->mask;

   if (IS_CMD(action->mask))
   {
      cmdAttr * newCmd = &(newAction->u.cmd);
      cmdAttr * oldCmd = &(action->u.cmd);

      CloneParsedMessage(&(oldCmd->execString), &(newCmd->execString));
      CloneParsedMessage(&(oldCmd->termOpts), &(newCmd->termOpts));
      newCmd->contextDir = XtNewString(oldCmd->contextDir);
      newCmd->contextHost = XtNewString(oldCmd->contextHost);
      CloneParsedMessage(&(oldCmd->execHosts), &(newCmd->execHosts));
      newCmd->execHostCount = oldCmd->execHostCount;
      if (oldCmd->execHostCount > 0) {
	newCmd->execHostArray = (char **)XtMalloc(sizeof(char *) *
						  newCmd->execHostCount);
	for (i = 0; i < newCmd->execHostCount; i++)
	  newCmd->execHostArray[i] = XtNewString(oldCmd->execHostArray[i]);
      }
      else {
	newCmd->execHostArray = NULL;
      }
   }
   else if (IS_MAP(action->mask))
   {
      newAction->u.map.map_action = action->u.map.map_action;
   }
   else if (IS_TT_MSG(action->mask))
   {
      tt_msgAttr * newMsg = &(newAction->u.tt_msg);
      tt_msgAttr * oldMsg = &(action->u.tt_msg);

      newMsg->tt_class = oldMsg->tt_class;
      newMsg->tt_scope = oldMsg->tt_scope;
      CloneParsedMessage(&(oldMsg->tt_op), &(newMsg->tt_op));
      CloneParsedMessage(&(oldMsg->tt_file), &(newMsg->tt_file));

      newMsg->mode_count = oldMsg->mode_count;
      if (oldMsg->mode_count > 0) {
	newMsg->tt_argn_mode =
	  (int *)XtMalloc(sizeof(int) * newMsg->mode_count);
	for (i = 0; i < newMsg->mode_count; i++)
	  newMsg->tt_argn_mode[i] = oldMsg->tt_argn_mode[i];
      }
      else {
	newMsg->tt_argn_mode = NULL;
      }

      newMsg->vtype_count = oldMsg->vtype_count;
      newMsg->tt_argn_vtype = CloneParsedMessageArray(oldMsg->tt_argn_vtype,
                                                      oldMsg->vtype_count);

      newMsg->value_count = oldMsg->value_count;
      newMsg->tt_argn_value = CloneParsedMessageArray(oldMsg->tt_argn_value,
                                                      oldMsg->value_count);

      newMsg->rep_type_count = oldMsg->rep_type_count;
      if (oldMsg->rep_type_count > 0) {
	newMsg->tt_argn_rep_type = (int *)XtMalloc(sizeof(int) * 
						   newMsg->rep_type_count);
	for (i = 0; i < newMsg->rep_type_count; i++)
	  newMsg->tt_argn_rep_type[i] = oldMsg->tt_argn_rep_type[i];
      }
      else {
	newMsg->tt_argn_rep_type = NULL;
      }
   }

   return(newAction);
}


/*
 * Free up the contents of a request structure 
 */

void 
_DtFreeActionStruct(
        register ActionPtr action )

{
   register int i;

   if (action == NULL)
      return;

   XtFree(action->label);
   XtFree(action->description);
   if (action->arg_types) XtFree((char *)action->arg_types);

   if (IS_CMD(action->mask))
   {
      FreeParsedMessage(&(action->u.cmd.execString));
      FreeParsedMessage(&(action->u.cmd.termOpts));
      XtFree(action->u.cmd.contextDir);
      XtFree(action->u.cmd.contextHost);
      FreeParsedMessage(&(action->u.cmd.execHosts));
      for (i = 0; i < action->u.cmd.execHostCount; i++)
         XtFree(action->u.cmd.execHostArray[i]);
      if (action->u.cmd.execHostArray) {
	XtFree((char *)action->u.cmd.execHostArray);
      }
   }
   else if (IS_TT_MSG(action->mask))
   {
      FreeParsedMessage(&(action->u.tt_msg.tt_op));
      FreeParsedMessage(&(action->u.tt_msg.tt_file));
      if (action->u.tt_msg.tt_argn_mode) {
	 XtFree((char *)action->u.tt_msg.tt_argn_mode);
      }
      FreeParsedMessageArray(action->u.tt_msg.tt_argn_vtype,
                             action->u.tt_msg.vtype_count);
      FreeParsedMessageArray(action->u.tt_msg.tt_argn_value,
                             action->u.tt_msg.value_count);
      if (action->u.tt_msg.tt_argn_rep_type) {
	 XtFree((char *)action->u.tt_msg.tt_argn_rep_type);
      }
   }

   XtFree((char *)action);
}


static void
CloneParsedMessage(
        register parsedMsg * old_pmsg,
        register parsedMsg * new_pmsg )

{
   int i;
   register MsgComponent * piece;
   register MsgComponent * newPiece;

   new_pmsg->numMsgParts = old_pmsg->numMsgParts;
   if (old_pmsg->compiledMessage)
   {
      /*
       * Some day these may not always be null-terminated strings
       */
      new_pmsg->compiledMessage = (char *)XtMalloc(old_pmsg->msgLen);
      memcpy(new_pmsg->compiledMessage,
          old_pmsg->compiledMessage,
          old_pmsg->msgLen);
      new_pmsg->msgLen = old_pmsg->msgLen;
   }
   else
   {
      new_pmsg->compiledMessage = NULL;
      new_pmsg->msgLen = 0;
   }


   /* Clone the message components */
   if (old_pmsg->numMsgParts > 0)
   {
      new_pmsg->parsedMessage = (MsgComponent *)
           XtMalloc((Cardinal)(sizeof(MsgComponent) * old_pmsg->numMsgParts));

      for (i = 0; i < old_pmsg->numMsgParts; i++)
      {
         piece = &(old_pmsg->parsedMessage[i]);
         newPiece = &(new_pmsg->parsedMessage[i]);

         /* Clone each subcomponent of this message */
         if (piece->precedingText)
            newPiece->precedingText = XtNewString(piece->precedingText);
         else
            newPiece->precedingText = NULL;

         if (piece->prompt)
            newPiece->prompt = XtNewString(piece->prompt);
         else
            newPiece->prompt = NULL;

         newPiece->keyword = piece->keyword;
         newPiece->argNum = piece->argNum;
         newPiece->mask = piece->mask;
      }
   }
   else
      new_pmsg->parsedMessage = NULL;
}


/*
 * Free up the contents of a parsedMsg structure, but not the structure
 * itself (since many of our structures contain in-line instances of
 * the parsedMsg structure).
 */
static void 
FreeParsedMessage(
        register parsedMsg * parsedMessage )

{
   int i;

   /* Free up the message components */
   if (parsedMessage->numMsgParts > 0)
   {
      for (i = 0; i < parsedMessage->numMsgParts; i++)
      {
         XtFree(parsedMessage->parsedMessage[i].precedingText);
         XtFree(parsedMessage->parsedMessage[i].prompt);
      }
      XtFree((char *)parsedMessage->parsedMessage);
   }

   XtFree(parsedMessage->compiledMessage);
}


/*
 * Allocate an array to hold a copy of all of the parsedMsg structures.
 * This array must be freed eventually by the caller.
 */
static parsedMsg *
CloneParsedMessageArray(
        register parsedMsg * pmsgArray,
        register int count )

{
   parsedMsg * newArray;
   int i;

   if (count == 0)
      return(NULL);

   newArray = (parsedMsg *)XtMalloc(sizeof(parsedMsg) * count);

   for (i = 0; i < count; i++)
      CloneParsedMessage(pmsgArray + i, newArray + i);

   return(newArray);
}


/*
 * Free up the counted array of parsedMsg structures.
 * The array pointing to them also needs to be freed.
 */
static void 
FreeParsedMessageArray(
        register parsedMsg * parsedMessageArray,
        int count )

{
   int i;

   for (i = 0; i < count; i++)
      FreeParsedMessage(parsedMessageArray + i);

   XtFree((char *)parsedMessageArray);
}


/***************************************************************************/
/***************************************************************************/
/*         Functions For Placing Arguments Into A Message String           */
/***************************************************************************/
/***************************************************************************/


/*
 * This function takes a 'parsedMsg' structure, and compiles all of its
 * pieces into a single string, replacing keywords as they are encountered.
 * Since a given action request can be made up of multiple pieces, this
 * function uses some static variables to maintain state information between
 * calls for the same action request; passing in 'True' for the 'initialize'
 * parameter for the first call for a given action request will clear out
 * any old static values.
 */

Boolean 
_DtCompileMessagePiece(
        Widget w,
        ActionRequest *request,
        char * relPathHost,
        char * relPathDir,
	parsedMsg * piece,
	Boolean initialize,
	unsigned long processingMask,
	Boolean ** paramUsed,
	int * promptDataIndex )

{
   register int i, j;
   Boolean firstParmUsed;
   register MsgComponent * segment;
   char * compiledMsg = NULL;
   int    compiledMsgSize = 0;
   ObjectData tmpObjData;
   static char *sessionHostName= NULL;
   static char *displayHostName = NULL;
   static char *localHostName = NULL;

   XtFree(piece->compiledMessage);
   piece->compiledMessage = NULL;
   piece->msgLen = 0;

   if (initialize)
   {
      /* 
       * Keep track of which parameters have been used, so that when
       * a %Args% keyword is encountered, we know which parameters
       * should be substituted.
       */
      *promptDataIndex = 0;

      if (request->numObjects > 0) {
  	 *paramUsed = (Boolean *)XtMalloc((Cardinal)(sizeof(Boolean) *
						     request->numObjects));
	 for (i = 0; i < request->numObjects; i++)
	   (*paramUsed)[i] = False;
      }
   }

   _DtSvcProcessLock();
   /* We need to query our hostname the first time only */
   if ( ! localHostName )
      localHostName = _DtGetLocalHostName();

   /*
    * Determine the display host name -- default to localHostName for
    * degenerate display names (i.e. :0, unix:0, local:0, ...)
    */
    if ( ! displayHostName )
	displayHostName = _DtGetDisplayHostName(XtDisplay(w));

    if ( ! sessionHostName )
	sessionHostName = _DtGetSessionHostName();
   _DtSvcProcessUnlock();

   /*
    * The message is constructed by taking each of the
    * action segments, replacing any keywords, and then adding the
    * information to the end of the buffer.
    */
   for (i = 0; i < piece->numMsgParts; i++)
   {
      segment = piece->parsedMessage + i;

      /* Add any text preceding the keyword */
      if (segment->precedingText)
      {
         compiledMsg = GrowMsgBuffer(compiledMsg, &compiledMsgSize, 
                                 (int)strlen(segment->precedingText));
         (void)strcat(compiledMsg, segment->precedingText);
      }

      /* Process the keyword */
      switch (segment->keyword)
      {
         case LOCAL_HOST:
         {
            /* Add in the local host name */
            compiledMsg = GrowMsgBuffer(compiledMsg, &compiledMsgSize, 
                                      (int)strlen(localHostName));
            (void)strcat(compiledMsg, localHostName);
            break;
         }

         case DATABASE_HOST:
         {
            /* 
             * Add in the host associated with the DB file from which this
             * action was loaded.
             */
            char * fullPath;
            char * host;

            fullPath = _DtDbPathIdToString(request->clonedAction->file_name_id);
            host = _DtHostString(fullPath);
	    if (host)
	    {
	      compiledMsg = GrowMsgBuffer(compiledMsg, &compiledMsgSize,
					  host ? (int)strlen(host) : 0);
	      (void)strcat(compiledMsg, host);
	      XtFree(host);
	    }
            XtFree(fullPath);
            break;
         }

         case DISPLAY_HOST:
         {
	    /*
	     * Use the displayHostName determined the first time thru
	     */
	    compiledMsg = GrowMsgBuffer(compiledMsg, &compiledMsgSize, 
				  (int)strlen(displayHostName));
	    (void)strcat(compiledMsg, displayHostName);
	    break;
         }

	 case SESSION_HOST:
	 {
            /* 
             * Add in the session server host where providing the
	     * display management.  (i.e. the host where the login client
	     * is running.) 
             */
	    compiledMsg = GrowMsgBuffer(compiledMsg, &compiledMsgSize, 
				  (int)strlen(sessionHostName));
	    (void)strcat(compiledMsg, sessionHostName);
	    break;

	 }

         case NO_KEYWORD:
         {
            /* 
             * If this is an entry which simply collected some user input,
             * then add the user's input to the message buffer.
             * This corresponds to the keywords: 
             *
             *         %"prompt"% 
             *         %(String)"prompt"%
             */
            if (segment->prompt)
            {
               /* Create dummy object; makes processing easier */
               if (ParseFileArgument(w, request, &tmpObjData,
                             NULL, request->promptInputs[*promptDataIndex],
                             NULL, False))
               {
		  XtFree(compiledMsg);
                  return(False);
               }

               if (!InsertArgumentString(w, &compiledMsg, &compiledMsgSize, 
                          request, &tmpObjData, segment->mask, relPathHost, 
                          relPathDir, False, 0))
               {
		  XtFree(compiledMsg);
                  return(False);
               }

               /* Signal that this prompt has been used */
               (*promptDataIndex)++;
            }
            break;
         }

         case ARG:
         {
            if (segment->argNum == ALL_ARGS)
            {
               /* Insert all currently unused parameters */
               for (j = 0, firstParmUsed = False; j < request->numObjects; j++)
               {
                  /* Used or empty objects are skipped */
                  if ((*paramUsed)[j] == False)
                  {
                     if (IS_FILE_OBJ(request->objects[j].mask) &&
                         request->objects[j].u.file.origFilename)
                     {
                        if (!InsertArgumentString(w, &compiledMsg, 
                                      &compiledMsgSize, 
                                      request, request->objects+j,
                                      segment->mask, relPathHost, relPathDir,
                                      firstParmUsed, processingMask))
                        {
			   XtFree(compiledMsg);
                           return(False);
                        }
                        firstParmUsed = True;
                     }
                     /* 
                      * RWV: 
                      * Since we use tmp files for buffers and do not support
                      * strings; we need not add special code for buffer and
                      * string support.
                      */
                     /* fdt: add support for buffers and strings
                      * else if (IS_BUFFER_OBJ(request->objects[i].mask) &&
                      *          request->objects[i].u.buffer.buffer)
                      * else if (IS_STRING_OBJ(request->objects[i].mask) &&
                      *          request->objects[i].u.string.string)
                      */
                  }
               }
            }
            else if (segment->argNum <= request->numObjects)
            {
               if (IS_FILE_OBJ(request->objects[segment->argNum-1].mask) &&
                   request->objects[segment->argNum-1].u.file.origFilename)
               {
                    /* Replace only with the specified argument */
                   (*paramUsed)[segment->argNum-1] = True;
                   /*
                    * All buffer objects have been written to tmp files.
                    * This code replaces a reference to an object with its
                    * (tmp) file name.
                    *      Tooltalk processing code elsewhere
                    * (ActionTt.c) detects the conditions under which a buffer
                    * object reference should be replaced by the buffer contents
                    * instead of the tmp file name.  (i.e. a value field with a
                    * single argument reference with no additional text).  In such
                    * cases the compiled message string will be ignored.
                    */
                   if (!InsertArgumentString(w, &compiledMsg, &compiledMsgSize, 
                             request, request->objects + segment->argNum - 1, 
                             segment->mask, relPathHost, relPathDir, False,
                             processingMask))
                  {
		     XtFree(compiledMsg);
                     return(False);
                  }
               }
	     /* 
	      * RWV: 
	      * Since we use tmp files for buffers and do not support
	      * strings; we need not add special code for buffer and
	      * string support.
	      */
               /* fdt: add support for buffers and strings
                * else if (IS_BUFFER_OBJ(request->objects[i].mask) &&
                *          request->objects[i].u.buffer.buffer)
                * else if (IS_STRING_OBJ(request->objects[i].mask) &&
                *          request->objects[i].u.string.string)
                */
            }
            break;
         }
      }
   }

   if ((piece->compiledMessage = compiledMsg) == NULL)
	piece->msgLen = 0;
   else
       piece->msgLen = compiledMsg ? strlen(compiledMsg) + 1: 0;
   return(True);
}

/*
 * Given an object, add it to the end of the message buffer.  The
 * object may refer to a file, thus possibly requiring that it be
 * converted to another format.
 */

static Boolean 
InsertArgumentString(
        Widget w,
        register char **bufPtr,
        int * bufSizePtr,
        ActionRequest *request,
        register ObjectData *object,
        unsigned long mask,
        char * relPathHost,
        char * relPathDir,
        Boolean addLeadingSpace,
        unsigned long processingMask )

{
   int len;
   String lastCh;
   int lastChLen;
   char * path;
   char * value;
   char * dataType;
   char * mediaAttr;

   if (processingMask & _DTAct_TT_VTYPE)
      SET_TREAT_AS_FILE(mask);

   if (IS_TREAT_AS_FILE(mask))
   {
      if (object->type == -1)
      {
         /* Object still needs to be typed */
         if (IS_FILE_OBJ(object->mask))
         {
            char * origInfo = object->u.file.origFilename;

            ParseFileArgument(w, request, object, NULL, origInfo, NULL, True);
            XtFree(origInfo);
         }
	 /* 
	  * RWV: 
	  * Since we use tmp files for buffers and do not support
	  * strings; we need not add special code for buffer and
	  * string support.
	  */
         /* fdt: add support for buffers and strings
          * else if (IS_BUFFER_OBJ(object->mask))
          * else if (IS_STRING_OBJ(object->mask))
          */
      }

      if (IS_FILE_OBJ(object->mask))
      {
         if (processingMask & _DTAct_TT_VTYPE)
         {
            /* 
             * Instead of inserting the object referred to by "Arg_n",
             * we need to instead insert the MEDIA attribute for the
             * object.  If the MEDIA attribute is not defined for the
             * datatype associated with this object, then use the
             * datatype name itself.  If the thing can't be defined, then
             * do nothing.
             */
            if (object->type != (-1))
            {
               dataType = (char *)_DtDtsMMBosonToString(object->type);

               if ((path = _DtActMapFileName(
                  request->hostNames[object->u.file.hostIndex], 
                  request->dirNames[object->u.file.dirIndex], 
                  object->u.file.baseFilename, 
                  NULL)) == NULL)
               {
                  path = NULL;
               }

               mediaAttr = DtDtsDataTypeToAttributeValue(dataType, "MEDIA", 
                                                         path);
               XtFree(path);

               if (mediaAttr)
               {
                  value = XtNewString(mediaAttr);
                  DtDtsFreeAttributeValue(mediaAttr);
               }
               else
                  value = XtNewString(dataType);

               *bufPtr = GrowMsgBuffer(*bufPtr, bufSizePtr, strlen(value) + 1);
               if (addLeadingSpace)
                  strcat(*bufPtr, " ");
               strcat(*bufPtr, value);
               XtFree(value);
            }
            return(True);
         }

         if (IS_CMD(request->clonedAction->mask))
         {
            /* Map into a real path, relative to the execution host */
            if ((path = _DtActMapFileName(
               request->hostNames[object->u.file.hostIndex], 
               request->dirNames[object->u.file.dirIndex], 
               object->u.file.baseFilename, 
               request->currentHost)) == NULL)
            {
               AddFailedHostToList(request, request->currentHost);
               return(False);
            }

            *bufPtr = GrowMsgBuffer(*bufPtr, bufSizePtr, strlen(path) + 1);
            if (addLeadingSpace)
               (void)strcat(*bufPtr, " ");
            strcat(*bufPtr, path);
            XtFree(path);
         }
         else if (IS_TT_MSG(request->clonedAction->mask))
         {
            /*
             * ToolTalk automatically translates the 'filename' field within
             * a message, and expects the incoming name to be relative to
             * the local host.  So ... we simply need to map the name to
             * be relative to the local host.  However, if this is not the
             * filename, but is instead one of the 'args', then we must
             * insert it in a 'neutral' form.
             */
            if (processingMask & _DTAct_TT_ARG)
            {
               /* Map into "host:/path" */
               /* fdt: May need to instead map into 'network indep' form */
               InsertUnmappedArgumentString(bufPtr, bufSizePtr, object, 
                                            addLeadingSpace);
            }
            else
            {
               if ((path = _DtActMapFileName(
                  request->hostNames[object->u.file.hostIndex], 
                  request->dirNames[object->u.file.dirIndex], 
                  object->u.file.baseFilename, NULL)) == NULL)
               {
                  return(False);
               }

               *bufPtr = GrowMsgBuffer(*bufPtr, bufSizePtr, strlen(path) + 1);
               if (addLeadingSpace)
                  (void)strcat(*bufPtr, " ");
               strcat(*bufPtr, path);
               XtFree(path);
            }
         }
      }
     /* 
      * RWV: 
      * Since we use tmp files for buffers and do not support
      * strings; we need not add special code for buffer and
      * string support.
      */
      /* fdt: add support for buffers and strings
       * else if (IS_BUFFER_OBJ(object->mask))
       * else if (IS_STRING_OBJ(object->mask))
       */
   }
   else
      InsertUnmappedArgumentString(bufPtr, bufSizePtr, object, addLeadingSpace);
   return(True);
}


/*
 * This function knows how to insert a string in "host:/path" format;
 * this is essentually an 'unmapped' filename.  File arguments which
 * have been preceded by the "(String)" qualifier will be saved in
 * this fashion.  Likewise, any filenames (either in "String" or "File"
 * form) for an message will be saved in this format, due to the
 * fact that we don't know the execution host, and thus cannot properly
 * map the filename using the ToolTalk filename mapping functions.
 */

static void 
InsertUnmappedArgumentString(
        register char **bufPtr,
        int * bufSizePtr,
        register ObjectData *object,
        Boolean addLeadingSpace )

{
   char * host = NULL;
   int size;

   /* No mapping is necessary here. */
   if (IS_FILE_OBJ(object->mask))
   {
      size = strlen(object->u.file.origFilename) + 4;
      *bufPtr = GrowMsgBuffer(*bufPtr, bufSizePtr, size);
      if (addLeadingSpace)
          (void)strcat(*bufPtr, " ");
      strcat(*bufPtr, object->u.file.origFilename);
   }
 /* 
  * RWV: 
  * Since we use tmp files for buffers and do not support
  * strings; we need not add special code for buffer and
  * string support.
  */
   /* fdt: add support for buffers and strings
    * else if (IS_BUFFER_OBJ(object->mask))
    * else if (IS_STRING_OBJ(object->mask))
    */
}


/*
 * This function checks to see if the message buffer is large enough
 * to hold the current contents + 'count' more bytes.  If it is not
 * large enough, then the buffer will be grown.  The buffer MUST BE
 * NULL terminated.
 */

static String 
GrowMsgBuffer(
        String buffer,
        int *size,
        int count )

{
   int currentBufUsed = buffer ? strlen(buffer) : 0;

   if ((currentBufUsed + count + 1) >= *size)
   {
      (*size) += (count+1 > 1024) ? count + 1 : 1024;
      buffer = (char *)XtRealloc(buffer, (Cardinal)*size);

      /* If this is the first alloc for the buffer, then terminate the buffer */
      if(currentBufUsed == 0)
         buffer[0] = '\0';
   }

   return(buffer);
}



/***************************************************************************/
/***************************************************************************/
/*           Functions For Matching Arguments To A Message String          */
/***************************************************************************/
/***************************************************************************/


/*
 * If the specified prompt has not already been added to the array of
 * prompt strings, then add it.  The exception is for stand-alone
 * prompt strings, which always are added.
 */

static void 
AddPrompt(
        register int argNum,
        String prompt,
        register int *numPrompts,
        register PromptEntry **prompts )

{
   register int i;

   /*
    * Standard arguments only want their prompts entered once.
    * Stand-alone prompts all have argNum == NO_ARG, and each one
    * must be saved.  It's a special case.
    */
   if (argNum != NO_ARG)
   {
      for (i = 0; i < *numPrompts; i++)
      {
         if ((*prompts)[i].argIndex == argNum)
            return;
      }
   }

   (*numPrompts)++;
   *prompts = (PromptEntry *)XtRealloc((char *)*prompts, 
              (Cardinal)(sizeof(PromptEntry) * *numPrompts));
   (*prompts)[(*numPrompts) - 1].argIndex = argNum;
   (*prompts)[(*numPrompts) - 1].prompt = prompt;
}


/*
 * This function takes an action DB entry and an action request, and
 * determines if enough information was supplied to create the message
 * needed to get the work done.  If information was missing, then this
 * function will return an array of prompt strings, which can be used
 * to create a dialog for collecting the missing information.  This 
 * function also returns an indication of how many of the parameters
 * were left unused.
 *
 * The caller is responsible for freeing up the prompt array, but the
 * entries in the array MUST NOT be freed up.
 */

static int 
MatchParamsToAction(
        ActionRequest *request,
        int *numPrompts,
        PromptEntry **prompts )
{
   Boolean * paramUsed = NULL;
   int unused;
   Boolean argsOptionFound;
   register int i;
   int lastArgReferenced;
   ActionPtr action = request->clonedAction;

   /* Initialize things */
   *numPrompts = 0;
   *prompts = NULL;
   argsOptionFound = False;
   lastArgReferenced = -1;

   /* 
    * This array lets us know which parameters can be used when we
    * encounter the %Args% keyword.
    */
   unused = request->numObjects;
   if (unused > 0) {
     paramUsed = (Boolean *)XtMalloc((Cardinal)(sizeof(Boolean) * unused));
     for (i = 0; i < unused; i++)
       paramUsed[i] = False;
   }

   if (IS_CMD(action->mask))
   {
	/*
	 * NOTE: The current implementation of prompt strings requires that
	 *	 the segments be evaluated in the same order in which the
	 *	 message fields were parsed.
	 *      (See ResolveCommandInvokerMessagePieces() )
	 *       This order is currently "execHost", "execString" and
	 *	 "termOpts".  This situation arises because
	 * 	 the existing prompt data structures do NOT identify the
	 *	 location of the prompt and hence where to put the
	 *	 user-supplied value; except by order of occurance.  
	 */
      ProcessOneSegment(request, &(action->u.cmd.execHosts), prompts, 
                        numPrompts, &argsOptionFound, &lastArgReferenced,
                        &unused, paramUsed);
      ProcessOneSegment(request, &(action->u.cmd.execString), prompts, 
                        numPrompts, &argsOptionFound, &lastArgReferenced, 
                        &unused, paramUsed);
      ProcessOneSegment(request, &(action->u.cmd.termOpts), prompts, 
                        numPrompts, &argsOptionFound, &lastArgReferenced, 
                        &unused, paramUsed);
   }
   else if (IS_TT_MSG(action->mask))
   {
      ProcessOneSegment(request, &(action->u.tt_msg.tt_op), prompts, 
                        numPrompts, &argsOptionFound, &lastArgReferenced, 
                        &unused, paramUsed);
      ProcessOneSegment(request, &(action->u.tt_msg.tt_file), prompts, 
                        numPrompts, &argsOptionFound, &lastArgReferenced, 
                        &unused, paramUsed);

      for (i = 0; i < action->u.tt_msg.vtype_count; i++)
      {
         ProcessOneSegment(request, &(action->u.tt_msg.tt_argn_vtype[i]), 
                           prompts, numPrompts, &argsOptionFound, 
                           &lastArgReferenced, &unused, paramUsed);
      }

      for (i = 0; i < action->u.tt_msg.value_count; i++)
      {
	/*
	 * We require that at most ONE argument be consumed by a
         * tt_argn_value field.
	 */
         ProcessOneSegment(request, &(action->u.tt_msg.tt_argn_value[i]), 
                           prompts, numPrompts, &argsOptionFound, 
                           &lastArgReferenced, &unused, paramUsed);
         
      }
   }

   /*
    * Now that we have processed all of the pieces which will ultimately
    * used to construct our message, determine if any of the arguments
    * passed to _DtActionInvoke were not used; this allows us to tell
    * the user that there were unused arguments, so they can choose
    * to continue or abort the request.
    * If we ever encountered a %Args% keyword, then ultimately all of
    * the parameters will be used.
    */
   if (argsOptionFound)
      unused = 0;
   else
   {
      /*
       * Determine how many arguments were actually unused; only count
       * those arguments AFTER the last referenced one.  i.e. if arg2
       * is referenced, but arg1 and arg3 are not, then only count arg3
       * as an unused (and thus extra) parameter.
       */
      for (i = 0; ((i < lastArgReferenced - 1) && (i < request->numObjects));
           i++)
      {
         if (!paramUsed[i])
            unused--;
      }

      /* This should never happen, but ... */
      if (unused < 0)
         unused = 0;
   }

   if (paramUsed) XtFree(paramUsed);

   return(unused);
}


static void 
ProcessOneSegment(
	ActionRequest * request,
        parsedMsg * msg,
        PromptEntry **prompts,
        int *numPrompts,
	Boolean * argsOptionFound,
	int * lastArgReferenced,
	int * unused,
	Boolean * paramUsed )
{
   MsgComponent * piece;
   int i;

   /* 
    * Check each piece of this message component, to see if the parameter
    * it expects has been supplied.  If the parameter is missing, and
    * a prompt was supplied, then add the prompt to the prompt array.
    */
   for (i = 0; i < msg->numMsgParts; i++)
   {
      piece = msg->parsedMessage + i;

      /* 
       * We only care about %Args% and %Arg_<n>% keywords, and
       * entries which have no keyword, but do have a prompt.
       */ 
      if (piece->keyword == ARG)
      {
         if (piece->argNum == ALL_ARGS)
         {
            /*
             * When a %Args% keyword is found, this implies that there
             * will ultimately be no unused parameters, because this
             * keyword is replaced by all unused parameters.
             */
            *argsOptionFound = True;
         }
         else if (piece->argNum > 0)
         {
            /* Keep track of the largest arg index referenced */
            if (piece->argNum > *lastArgReferenced)
               *lastArgReferenced = piece->argNum;

            /* See if a parameter was supplied for this argNum */
            if (piece->argNum > request->numObjects)
            {
               /* Parameter is missing; see if a prompt was given */
               if (piece->prompt)
                  AddPrompt(piece->argNum, piece->prompt, numPrompts, prompts);
            }
            else
            {
               /* Mark this parameter as having been used */
               if (!paramUsed[piece->argNum - 1])
               {
                  paramUsed[piece->argNum - 1] = True;
                  (*unused)--;
               }
            }
         }
      }
      else if ((piece->keyword == NO_KEYWORD) && (piece->prompt))
      {
         /* Entries may be nothing but a prompt */
         AddPrompt(NO_ARG, piece->prompt, numPrompts, prompts);
      }
   }
}

/***************************************************************************/
/***************************************************************************/
/*                          Prompt Dialog Support                          */
/***************************************************************************/
/***************************************************************************/


/*
 * This is the event handler which catches the 'escape' key when typed
 * into the prompt.  It will unpost the dialog.
 */

static void
CancelOut(
        Widget w,
        XEvent *event,
        XtPointer params,
        XtPointer num_params)
{
   Arg args[10];
   Widget cancel;

   /* Get the cancel button widget id */
   XtSetArg(args[0], XmNuserData, &cancel);
   XtGetValues(w, args, 1);

   /* Unpost the text annotation dialog */
   XtCallCallbacks(cancel, XmNactivateCallback, NULL);
}


/*
 * 'Cancel' callback for the dialog used to collect missing parameters
 * from the user.  It will free up the memory holding the cancelled
 * request and will destroy the dialog.
 */

static void 
CancelPromptDialog(
        Widget widget,
        PromptDialog *dialog,
        XtPointer call_data )

{
   unsigned long evalStatus;
   unsigned long userStatus;
   _DtActInvRecT *invp;

   /* Destroy the dialog */
   XtDestroyWidget(XtParent(dialog->topLevel));

   /* Free up the prompt sub-structure */
   XtFree((char *)dialog->prompts);

   
   invp = _DtActFindInvRec(dialog->request->invocId);
   myassert(invp);  /* There should always be an invocation record */

   /* Free up the original request structure */
   _DtFreeRequest(dialog->request);

   /* Free up the callback structure */
   XtFree((char *)dialog);

   if ( !invp )
       return;	/* This should never happen */

   SET_INV_CANCEL(invp->state);

   /*
    * Evaluate whether we are done with this invocation -- are there
    * uncompleted children? There should not be any subsequent invocations
    * to worry about since this cancel effectively aborts further processing.
    *
    * We may have to return  values  to the caller.  
    */
   
   _DtActExecutionLeafNodeCleanup(invp->id,NULL,0,True);

}

/*
 * This function changes the focus from the given "widget's" 
 * tab group to the next tab group.
 */

static void 
ChangePromptTraversal(
        Widget widget,
        register PromptDialog *dialog,
        XtPointer call_data )
{
   XmProcessTraversal (widget, XmTRAVERSE_NEXT_TAB_GROUP);
}

/*
 * 'Ok' callback for the dialog used to collect missing parameters
 * from the user.  It will redo the array of parameter strings, and
 * then execute the command, given whatever the user has supplied.
 * It will also destroy the dialog box.
 */

static void 
ProcessPromptDialog(
        Widget widget,
        register PromptDialog *dialog,
        XtPointer call_data )

{
   register int i, j;
   String value;

   /* Unpost the dialog */
   XtUnmanageChild(dialog->topLevel);

   /*
    * Given the set of strings supplied by the user, update the
    * object array which is part of the original request.
    */
   for (i = 0; i < dialog->numPrompts; i++)
   {
      value = XmTextFieldGetString(dialog->prompts[i].promptWidget);

      /* Do we need to grow the object array? */
      if (dialog->prompts[i].argIndex > 0)
      {
         if (_DtEmptyString(value))
         {
            XtFree(value);
            continue;
         }

         if (dialog->prompts[i].argIndex > dialog->request->numObjects)
         {
            dialog->request->objects = (ObjectData *)
                XtRealloc((char *)dialog->request->objects, 
                (Cardinal)(sizeof(ObjectData) * (dialog->prompts[i].argIndex)));

            /* Initialize the new array entries */
            for (j = dialog->request->numObjects; 
                 j < dialog->prompts[i].argIndex; 
                 j++)
            {
               dialog->request->objects[j].mask = 0;
               SET_FILE_OBJ(dialog->request->objects[j].mask);
               dialog->request->objects[j].type = -1;
               dialog->request->objects[j].u.file.hostIndex = -1;
               dialog->request->objects[j].u.file.dirIndex = -1;
               dialog->request->objects[j].u.file.origFilename = NULL;
               dialog->request->objects[j].u.file.origHostname = NULL;
               dialog->request->objects[j].u.file.baseFilename = NULL;
               dialog->request->objects[j].u.file.bp = 0;
               dialog->request->objects[j].u.file.sizebp = 0;
               dialog->request->objects[j].u.buffer.bp = 0;
               dialog->request->objects[j].u.buffer.size = 0;
               dialog->request->objects[j].u.string.string = 0;
               SET_UNKNOWN_IF_DIR(dialog->request->objects[j].mask);
            }
            dialog->request->numObjects = dialog->prompts[i].argIndex;
         }
         /*
          * These values cannot be broken up into host/dir/file components,
          * nor can they be typed, until we know it they refer to a file.
          * This can't be determined until we construct the action message.
          */
         dialog->request->objects[dialog->prompts[i].argIndex-1].u.file.
                  origFilename = value;
      }
      else /* Prompt-only input */
      {
         /* 
          * Prompt-only input can't fit in our ordered object array,
          * since they don't have a unique argIndex which can be used
          * as the index into the object array.
          */
         dialog->request->numPromptInputs++;
         dialog->request->promptInputs = (String *)
            XtRealloc((char *)dialog->request->promptInputs,
              (Cardinal)(dialog->request->numPromptInputs * sizeof(String)));

         dialog->request->promptInputs[dialog->request->numPromptInputs-1] =
              value;
      }
   }

   /* Destroy the dialog */
   XtDestroyWidget(XtParent(dialog->topLevel));
   XmUpdateDisplay(widget);
   
   /* 
    * Invoke the action using the information we've collected.
    * If this was a single argument action, then the reprocessing
    * of it may have generated another dialog, so we can only free
    * up the request, when all processing is done.
    */
   if (ProcessRequest(dialog->associatedWidget, dialog->request))
   {
       _DtActInvRecT	*invp;
       unsigned long    evalStatus;
       unsigned long    userStatus;

       if ( (invp = _DtActFindInvRec(dialog->request->invocId)) )
       {
	   /* all done invoking ? */
	   RESET_INV_PENDING(invp->state);

	   /* We should only get here if all requests have been honored */
	   SET_INV_COMPLETE(invp->state);

	   /*
	    * evaluate whether all child actions have been completed
	    * and if its time to call the user callback.
	    */
           _DtActExecutionLeafNodeCleanup(invp->id,NULL,0,True);
       }
       myassert(invp); /* there should always be one to find */
       _DtFreeRequest(dialog->request);
   }

   /* Free up the prompt sub-structure */
   XtFree((char *)dialog->prompts);

   /* Free up the callback structure */
   XtFree((char *)dialog);
}


/*
 * This function takes the array of prompt strings, and creates a
 * dialog box, using these prompt strings as the labels for a set
 * of text widgets.
 */

static void 
CreatePromptDialog(
        Widget w,
        ActionRequest *request,
        register int numPrompts,
        register PromptEntry *prompts )

{
   register PromptDialog * dialog;
   register DialogPromptEntry * promptDes;
   XmString pt1;
   String title;
   Widget shell, bboard, frame, form, label;
   Widget promptLabel, topAttach;
   Widget separator, ok, cancel;
   register int count;
   int n, i;
   Arg args[20];
   XmString labelString;
   XWindowAttributes xwa;
   Status status;
   Boolean is_mapped = False;
   static XtTranslations trans_table;
   static Boolean first = True;
   Atom xa_WM_DELETE_WINDOW;

   /*
    * Want to set up the Escape key so that it will unpost the dialog.
    */
   _DtSvcProcessLock();
   if (first)
   {
      XtAppAddActions(XtWidgetToApplicationContext(w), actionTable, 1);
      trans_table = XtParseTranslationTable(translations_escape);
      first = False;
   }
   _DtSvcProcessUnlock();

   /* Allocate the structures we'll be needing */
   dialog = (PromptDialog *)XtMalloc((Cardinal)sizeof(PromptDialog));
   promptDes = (DialogPromptEntry *)XtMalloc((Cardinal)
              (sizeof(DialogPromptEntry) * numPrompts));


   /*  Create the shell, frame and form used for the dialog.  */

   title = (char *)XtMalloc((Cardinal)
           (strlen(PromptDialogTitle)+ strlen(request->clonedAction->label) + 1));
   (void)sprintf(title, "%1$s%2$s", PromptDialogTitle, request->clonedAction->label);
   n = 0;
   XtSetArg (args[n], XmNallowShellResize, True);		n++;
   XtSetArg (args[n], XmNtitle, title);		n++;
   shell = XmCreateDialogShell (w, "promptDialog", args, n);
   XtFree(title);

   if (XtIsRealized(w))
   {
     status = XGetWindowAttributes (XtDisplay (w), XtWindow (w), &xwa);
     if (status && (xwa.map_state == IsViewable))
       is_mapped = True;
   }

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 0);		n++;
   XtSetArg (args[n], XmNmarginHeight, 0);		n++;
   if (!is_mapped)
   {
      XtSetArg (args[n], XmNdefaultPosition, False);	
      n++;
   }
   bboard = XmCreateBulletinBoard (shell, "bboard", args, n);

   n = 0;
   XtSetArg (args[n], XmNshadowThickness, 1);		n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);	n++;
   frame = XmCreateFrame (bboard, "frame", args, n);
   XtManageChild (frame);

   n = 0;
   XtSetArg (args[n], XmNautoUnmanage, False);			n++;
   XtSetArg (args[n], XmNtextTranslations, trans_table);	n++;
   form = XmCreateForm (frame, "form", args, n);
   XtManageChild (form);

   /* Create the dialog description label */

   pt1 = XmStringCreateLocalized(PromptDialogLabel);
   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);       n++;
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
   XtSetArg(args[n], XmNleftOffset, 20);                       n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
   XtSetArg(args[n], XmNrightOffset, 20);                       n++;
   XtSetArg(args[n], XmNtopOffset, 15);                        n++;
   XtSetArg(args[n], XmNlabelString, pt1);                        n++;
   label = XmCreateLabelGadget(form, "label", args, n);
   XtManageChild (label);
   XmStringFree(pt1);

   /* Create each of the needed prompts */
   topAttach = label;
   for (count = 0; count < numPrompts; count++)
   {
      promptDes[count].argIndex = prompts[count].argIndex;

      pt1 = XmStringCreateLocalized(prompts[count].prompt);
      n = 0;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNtopWidget, topAttach);       n++;
      XtSetArg(args[n], XmNtopOffset, 10);                        n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNleftOffset, 30);                       n++;
      XtSetArg(args[n], XmNlabelString, pt1);                       n++;
      promptLabel = XmCreateLabelGadget(form, "promptLabel", args, n);
      XtManageChild(promptLabel);
      XmStringFree(pt1);

      n = 0;
      XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
      XtSetArg(args[n], XmNtopWidget, topAttach);       n++;
      XtSetArg(args[n], XmNtopOffset, 8);                        n++;
      XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
      XtSetArg(args[n], XmNrightOffset, 30);                       n++;
      XtSetArg(args[n], XmNtraversalOn, True);                       n++;
      XtSetArg(args[n], XmNleftAttachment,XmATTACH_WIDGET );    n++;
      XtSetArg(args[n], XmNleftWidget, promptLabel);        n++;
      XtSetArg(args[n], XmNleftOffset, 15);                     n++;
      promptDes[count].promptWidget = XmCreateTextField(form, "text", args, n);

      XtManageChild(promptDes[count].promptWidget);

      XmAddTabGroup(promptDes[count].promptWidget);
      topAttach = promptDes[count].promptWidget;
   }


   /*  Create a separator between the buttons  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopWidget, topAttach);	n++;
   XtSetArg (args[n], XmNtopOffset, 20);			n++;
   separator =  XmCreateSeparatorGadget (form, "separator", args, n);
   XtManageChild (separator);


   /*  Create the ok and cancel buttons  */

   n = 0;
   labelString = XmStringCreateLocalized((String)_DtOkString);
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 5 + 10);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNrightPosition, 31 + 10);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopWidget, separator);			n++;
   XtSetArg (args[n], XmNtopOffset, 16);			n++;
   XtSetArg (args[n], XmNbottomOffset, 16);			n++;
   XtSetArg (args[n], XmNmarginHeight, 4);			n++;
   XtSetArg (args[n], XmNshowAsDefault, True);                  n++;
   XtSetArg (args[n], XmNlabelString, labelString);             n++;
   ok = XmCreatePushButtonGadget (form, "ok", args, n);
   XtManageChild(ok);
   XtAddCallback(ok, XmNactivateCallback, (XtCallbackProc)ProcessPromptDialog, 
		 (XtPointer)dialog);
   XmStringFree(labelString);

   /* Set the default action */
   
   if (numPrompts <= 1) 
   {
      n = 0;
      XtSetArg (args[n], XmNdefaultButton, ok);    n++;
      XtSetValues(bboard, args, n);
   }
   else 
   {
      int i;
      /*
       * Want to set the traversal so that if "return" is hit in the
       * last prompt, the "ProcessPromptDialog" callback is invoked.  
       * Otherwise, the "return" should move the focus to the next 
       * which is in a different (tab group).
       */
      for (i = 0; i < numPrompts; i++) 
      {
	 if (i <= (numPrompts - 2))
	    XtAddCallback(promptDes[i].promptWidget, XmNactivateCallback, 
			  (XtCallbackProc)ChangePromptTraversal, 
			  (XtPointer)dialog);
	 else
	    XtAddCallback(promptDes[i].promptWidget, XmNactivateCallback, 
			  (XtCallbackProc)ProcessPromptDialog, 
			  (XtPointer)dialog);
      }
   }

   n = 0;
   labelString = XmStringCreateLocalized((String)_DtCancelString);
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 37 + 22);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNrightPosition, 63 + 22);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopWidget, separator);			n++;
   XtSetArg (args[n], XmNtopOffset, 21);			n++;
   XtSetArg (args[n], XmNbottomOffset, 21);			n++;
   XtSetArg (args[n], XmNmarginHeight, 4);			n++;
   XtSetArg (args[n], XmNlabelString, labelString);             n++;
   cancel = XmCreatePushButtonGadget (form, "cancel", args, n);
   XtManageChild(cancel);
   XtAddCallback(cancel, XmNactivateCallback, (XtCallbackProc)CancelPromptDialog, 
                 (XtPointer)dialog);
   XmStringFree(labelString);

   /*
    * For each prompt, must set up the Escape key to be equivalent
    * to the "Cancel button.
    */
   for (i = 0; i < numPrompts; i++)  {
      n = 0;
      XtSetArg(args[n], XmNuserData, cancel);  n++;
      XtSetValues(promptDes[i].promptWidget, args, n);
   }

   /*
    * If the widget is not mapped, center this dialog.
    */
   if (!is_mapped) 
   {
      Dimension dialogWd, dialogHt;

      XtSetArg(args[0], XmNmappedWhenManaged, False);
      XtSetValues(shell, args, 1);

      XtManageChild(bboard);
      XtRealizeWidget(shell);

      XtSetArg(args[0], XmNwidth, &dialogWd);
      XtSetArg(args[1], XmNheight, &dialogHt);
      XtGetValues(bboard, args, 2);

      XtSetArg (args[0], XmNx,
		(WidthOfScreen(XtScreen(bboard)) - dialogWd) / 2U);
      XtSetArg (args[1], XmNy,
		(HeightOfScreen(XtScreen(bboard)) - dialogHt) / 2U);
      XtSetValues (bboard, args, 2); 
   }

   /*  Adjust the decorations for the dialog shell of the dialog  */

   n = 0;
   XtSetArg (args[n], XmNmwmDecorations, 
             MWM_DECOR_BORDER | MWM_DECOR_MENU | MWM_DECOR_TITLE);	n++;
   XtSetValues(shell, args, n);

   xa_WM_DELETE_WINDOW = 
     XInternAtom(XtDisplay(shell), "WM_DELETE_WINDOW", False);
   XmAddWMProtocolCallback(
		shell, xa_WM_DELETE_WINDOW,
		(XtCallbackProc) CancelPromptDialog, (XtPointer) dialog);

   /* Fill in our instance structure */
   dialog->request = request;
   dialog->topLevel = bboard;
   dialog->numPrompts = count;
   dialog->prompts = promptDes;
   dialog->associatedWidget = w;

   /* Post the dialog */
   XtSetArg(args[0], XmNmappedWhenManaged, True);
   XtSetValues(shell, args, 1);
   XtManageChild(bboard);

   /* Make the first prompt automatically get the focus. */
   if (numPrompts >= 0)
      XmProcessTraversal(promptDes[0].promptWidget, XmTRAVERSE_CURRENT);
}


/***************************************************************************/
/***************************************************************************/
/*                         Continue Dialog Support                         */
/***************************************************************************/
/***************************************************************************/


/*
 * 'Ok' callback for the abort/continue dialog.  It will continue with the
 * processing of the request, ignoring any unused parameters.
 */

static void 
ContinueRequest(
        Widget widget,
        XtPointer user_data,
        XtPointer call_data )

{
   register int i;
   register ContinueDialog *dialog = (ContinueDialog *)user_data;

   /* Destroy the dialog */
   XtDestroyWidget(XtParent(dialog->topLevel));
   XmUpdateDisplay(widget);

   /*
    * If we need to collect some prompt input from the user, then
    * post the prompt dialog; otherwise, send the action request.
    */
   if (dialog->numPrompts == 0)
   {
      if (ProcessRequest(dialog->associatedWidget, dialog->request))
      {
	   _DtActInvRecT	*invp;

	   if((invp=_DtActFindInvRec(dialog->request->invocId))!=NULL)
	   {
	       /* all done invoking ? */
	       RESET_INV_PENDING(invp->state);

	       /* We should only get here if all requests have been honored */
	       SET_INV_COMPLETE(invp->state);

	       /*
		* evaluate whether all child actions have been completed
		* and if its time to call the user callback.
		*/
               _DtActExecutionLeafNodeCleanup(invp->id,NULL,0,True);
	   }

	   myassert(invp); /* there should always be one to find */
         _DtFreeRequest(dialog->request);
      }
   }
   else
   {
      CreatePromptDialog(dialog->associatedWidget, dialog->request,
                         dialog->numPrompts, 
                         dialog->prompts);
   }


   /* Free up the prompt sub-structure */
   for (i = 0; i < dialog->numPrompts; i++)
      XtFree(dialog->prompts[i].prompt);
   XtFree((char *)dialog->prompts);

   /* Free up the callback structure */
   XtFree((char *)dialog);
}


/*
 * 'Cancel' callback for the dialog which prompts the user to continue
 * or abort, when too many parameters have been supplied.  This will
 * free up the dialog data and the request and will destroy the dialog.
 */

static void 
CancelRequest(
        Widget widget,
        XtPointer user_data,
        XtPointer call_data )

{
   register int i;
   register ContinueDialog *dialog = (ContinueDialog *)user_data;
   unsigned long evalStatus;
   unsigned long userStatus;
   _DtActInvRecT *invp;

   /* Destroy the dialog */
   XtDestroyWidget(XtParent(dialog->topLevel));

   /* Free up the prompt sub-structure */
   for (i = 0; i < dialog->numPrompts; i++)
      XtFree(dialog->prompts[i].prompt);
   XtFree((char *)dialog->prompts);

   /* get the invocation record */
   invp = _DtActFindInvRec(dialog->request->invocId);
   myassert(invp);	/* There should always be one available */
   
   /* Free up the original request structure */
   _DtFreeRequest(dialog->request);

   /* Free up the callback structure */
   XtFree((char *)dialog);

   if ( !invp )
       return;   /* should never happen */

   SET_INV_CANCEL(invp->state);
   /*
    * Evaluate whether we are done with this invocation -- are there
    * uncompleted children? There should not be any subsequent invocations
    * to worry about since this cancel effectively aborts further processing.
    *
    * We may have to return  values  to the caller.  
    */
   _DtActExecutionLeafNodeCleanup(invp->id,NULL,0,True);
}

/*
 * When an action is requested, and more parameters than are needed
 * are supplied, the user will be prompted to continue with the
 * operation (ignoring the extra parameters), or to abort the request.
 *
 * This function builds the dialog which will collect the user's response.
 */

static void 
CreateContinueDialog(
        Widget w,
        ActionRequest *request,
        int numPrompts,
        register PromptEntry *prompts )

{
   register ContinueDialog * dialog;
   String title;
   XmString label;
   register int i; 
   int n;
   Arg args[10];
   XmString ok, cancel;
   char *fmt;

   /* Allocate the structures we'll be needing */
   dialog = (ContinueDialog *)XtMalloc((Cardinal)sizeof(ContinueDialog));
   dialog->request = request;
   dialog->associatedWidget = w;
   dialog->numPrompts = numPrompts;

   /*
    * We need to make a clone of the prompt array, since the strings
    * it contains are not ones we can guarantee will be around when
    * the user finally responds to this dialog.
    */
   if (prompts)
   {
      dialog->prompts = (PromptEntry *)
              XtMalloc((Cardinal)(sizeof(PromptEntry) * numPrompts));
      for (i = 0; i < numPrompts; i++)
      {
         dialog->prompts[i].argIndex = prompts[i].argIndex;
         dialog->prompts[i].prompt = XtNewString(prompts[i].prompt);
      }
   }
   else
      dialog->prompts = NULL;

   ok = XmStringCreateLocalized((String)_DtOkString);
   cancel = XmStringCreateLocalized((String)_DtCancelString);

   /* Create the error dialog */
   fmt = XtNewString((char *)Dt11GETMESSAGE(2, 2, "%1$s%2$s"));
   title = (char *)XtMalloc((Cardinal)
             (strlen(PromptDialogTitle) +
	      strlen(request->clonedAction->label) +
	      strlen(fmt) + 1));
   (void)sprintf(title, fmt, PromptDialogTitle, request->clonedAction->label);
   label = XmStringCreateLocalized(ContinueMessage);
   XtFree(fmt);

   n = 0;
   XtSetArg(args[n], XmNmessageString, label); n++;
   XtSetArg(args[n], XmNtitle, title); n++;
   XtSetArg(args[n], XmNokLabelString, ok); n++;
   XtSetArg(args[n], XmNcancelLabelString, cancel); n++;
   dialog->topLevel = XmCreateWarningDialog(w, "continueDialog", args, n);
   XtFree(title);
   XmStringFree(ok);
   XmStringFree(cancel);
   XmStringFree(label);

   XtUnmanageChild(XmMessageBoxGetChild(dialog->topLevel,
					XmDIALOG_HELP_BUTTON));
   XtAddCallback(dialog->topLevel, XmNokCallback, ContinueRequest,
		 (XtPointer)dialog);
   XtAddCallback(dialog->topLevel, XmNcancelCallback, CancelRequest,
		 (XtPointer)dialog);
   XtManageChild(dialog->topLevel);
}


/***************************************************************************/
/***************************************************************************/
/*                  Command Invoker Specific Functions                     */
/***************************************************************************/
/***************************************************************************/

/*
 * This is the entry point into the command-specific world.  All of the code
 * before this has been written to handle any of the different transport
 * types.  The code from this point on will know specifically how to
 * interact with the command invoker layer.  We will start by taking each
 * of the pieces of information making up the command invoker request, and
 * resolving any of the keywords, by replacing them with the appropriate
 * information.  If this fails (which it only should do if we try to map
 * a file to a host which cannot be accessed), then we will either continue,
 * using the next exec host, or we will terminate, if no more hosts are left.
 */

static void 
ProcessCommandInvokerRequest(
        Widget w,
        ActionRequest *request,
        char * relPathHost,
        char * relPathDir )

{
   char * cwdHost;
   char * cwdDir;
   ActionPtr action = request->clonedAction;
   _DtActInvRecT *invp = NULL;
   _DtActChildRecT *childp = NULL;

   if (ResolveCommandInvokerMessagePieces(w, request, relPathHost, relPathDir))
   {
      /* 
       * Issue the request; the success/failure notification comes 
       * asynchronously; that's when everything gets cleaned up, or
       * tried again, for the next exec host.
       */
      __ExtractCWD(request, &cwdHost, &cwdDir, True);
      InitiateCommandInvokerRequest( w, request, cwdHost, cwdDir);
      XtFree(cwdHost);
      XtFree(cwdDir);
   }
   else
   {
       if (  !(invp = _DtActFindInvRec(request->invocId) ) )
	    myassert( 0 /* could not find invocation record */ );

       if ( !(childp=_DtActFindChildRec(request->invocId,request->childId)))
	    myassert( 0 /* could not find child record */ );

      /*
       * The only way we could have reached here is if the execution host
       * was not accessible, and we tried to map one of the data files to
       * be relative to this host.  If there are other hosts to be tried,
       * then we will retry the request on the next host; otherwise, we
       * will post an error dialog, and bail out.
       */

      request->hostIndex++;

      if (request->hostIndex >= action->u.cmd.execHostCount)
      {
         /* No more hosts to try; report an error, and bail out */

	   if ( invp && childp )
	   {
	     SET_INV_ERROR(invp->state); 
	     childp->childState = _DtActCHILD_FAILED;
	   }

         /* 
          * Cleanup should happen later when we return up the stack.
          */

         if (action->u.cmd.execHostCount <= 1)
         {
            /* Display error dialog listing just the one failed exec host */
            HostAccessError(w, request->clonedAction->label, request->badHostList);
         }
         else
         {
            /* Display error dialog listing all failed exec hosts */
            MultiHostAccessError(w, request->clonedAction->label, request->badHostList);
         }
      }
      else
      {
	   if ( invp && childp )
	   {
		/* 
		 * Delete child record for failed exec on this host
		 */
		_DtActDeleteChildRec(invp,childp);
		SET_INV_PENDING(invp->state);
		if ( ! invp->numChildren )
		    RESET_INV_WORKING(invp->state);
	   }

         /* Retry the request, using the next exec host */
         PrepareAndExecuteAction(w, request);
         return;
      }
   }
}


/*
 * This function takes all of the pieces making up a command invoker request,
 * and resolves any references to keywords, using both the passed-in
 * arguments, and any information collected from the prompt dialog.
 */

static Boolean 
ResolveCommandInvokerMessagePieces(
        Widget w,
        ActionRequest *request,
        char * relPathHost,
        char * relPathDir )

{
   ActionPtr action = request->clonedAction;
   cmdAttr * cmd = &(action->u.cmd);
   char * termOpts;
   Boolean * paramUsed = NULL;
   int promptDataIndex = 0;
	/*
	 * NOTE: The current implementation of prompt strings requires that
	 *	 the segments be evaluated in the same order in which the
	 *	 action fields were parsed. (See MatchParamsToAction() )
	 *       This order is currently "execHost", "execString" and
	 *	 "termOpts".  This situation arises because
	 * 	 the existing prompt data structures do NOT identify the
	 *	 location of the prompt and hence where to put the
	 *	 user-supplied value; except by order of occurance.  
	 */

   /* Set up the next host to execute on */
   _DtCompileMessagePiece(w, request, relPathHost, relPathDir,
                           &(action->u.cmd.execHosts), True, 0, &paramUsed,
			   &promptDataIndex);
   SetExecHost(request);

   if ((_DtCompileMessagePiece(w, request, relPathHost, relPathDir,
                           &(cmd->execString), False, 0, &paramUsed,
			   &promptDataIndex) == False) ||
       (_DtCompileMessagePiece(w, request, relPathHost, relPathDir,
                           &(cmd->termOpts), False, 0, &paramUsed,
			   &promptDataIndex) == False))
   {
      /* Free up any intermediate work we've done here */
      XtFree(cmd->execString.compiledMessage);
      XtFree(cmd->termOpts.compiledMessage);
      XtFree(cmd->execHosts.compiledMessage);
      cmd->execString.compiledMessage = NULL;
      cmd->termOpts.compiledMessage = NULL;
      cmd->execHosts.compiledMessage = NULL;
      XtFree(paramUsed);
      return(False);
   }

   /* 
    * If term_opts were passed in to the _DtActionInvoke() function, then
    * append them to the term_opts derived from the action definition and
    * internal defaults.  This should give precedence to the last defined
    * options.
    */
   if ( request->termOpts )
   {
         termOpts = XtMalloc( strlen(cmd->termOpts.compiledMessage) + 
			strlen(request->termOpts) + 2 );
	strcpy(termOpts,cmd->termOpts.compiledMessage);
	strcat(termOpts," ");
	strcat(termOpts,request->termOpts);
	XtFree(cmd->termOpts.compiledMessage);
        cmd->termOpts.compiledMessage = termOpts;
   }

   XtFree(paramUsed);
   return(True);
}


/*
 * Process a command-invoker request.
 */

static void 
InitiateCommandInvokerRequest(
        Widget w,
        ActionRequest *request,
        String host,
        String dir)

{
   char procIdBuf[_DtAct_MAX_BUF_SIZE];
   char tmpFileBuf[_DtAct_MAX_BUF_SIZE];
   char *procId;		/* for dtexec command line */
   char *tmpFiles = NULL;	/* for dtexec command line */
   _DtActInvRecT *invp;
   _DtActChildRecT *childp;
   CallbackData *data=(CallbackData *)XtMalloc((Cardinal)sizeof(CallbackData));
   ActionPtr action = request->clonedAction;

   tmpFileBuf[0]='\0';	/* seed the buffer with a null string */

   /*
    * Generate the procId option string for dtexec
    */

   /* Get the default procId from toolTalk */
   switch ( tt_ptr_error(procId = tt_default_procid()) )
   {
       case TT_ERR_NOMP:
           ;	/* fall through */
       case TT_ERR_PROCID: /* Try to establish a connection */
           tt_free(procId) ;
           if ( !_DtInitializeToolTalk(NULL) )
               procId=NULL;
           else if ( tt_ptr_error(procId = tt_default_procid()) != TT_OK )
           {
               myassert( 0 ); /* we should never get here */
               procId = NULL;
           }
           break;
       case TT_OK:
           break;
       default:
           tt_free(procId);
	   procId = NULL;
           break;
   }
   /*
    * The string generated for procId should never exceed the procId buf size. 
    */
   sprintf(procIdBuf,"%s_%d_%lu",
       _DtActNULL_GUARD(procId),
       (int) request->invocId,
       request->childId );

   myassert( strlen(procIdBuf) < sizeof(procIdBuf) );

   if (procId)
       tt_free(procId);
   procId = procIdBuf;	/* no need to malloc */
   
   /*
    * Generate string of tmp file args  for dtexec.
    */
    
    if ( (invp = _DtActFindInvRec(request->invocId)) != NULL )
    {
        if ( (childp =
             _DtActFindChildRec(request->invocId,request->childId)) != NULL )
        {
	    int i;
            char *p;
            int len = 0;

	    for(i = 0; i < childp->numObjects; i++)
	    {

                if(!(IS_BUFFER_OBJ(invp->info[childp->argMap[i].argIdx].mask)))
                    continue;	/* not a buffer object */

		if ( !(p = invp->info[childp->argMap[i].argIdx].name) )
                    continue;	/* no tmp file name */
 
		/* Add up the string length of the file name */
                if((len += strlen(" -tmp ") + strlen(p)) < sizeof(tmpFileBuf))
                {
                    /* 
                     * Use the automatic tmpFileBuf if possible
                     */
                    strcat(tmpFileBuf," -tmp ");
                    strcat(tmpFileBuf,p);
                } 
                else
                {
                   /*
                    * Malloc more space if necessary
                    */
                    XtFree(tmpFiles);
                    tmpFiles = XtMalloc(len + 1);
                    strcpy(tmpFiles,tmpFileBuf);
                    strcpy(tmpFiles," -tmp ");
                    strcpy(tmpFiles,p);
                }
                
	    }
            if ( len > 0 && len <  sizeof(tmpFileBuf) )
                tmpFiles = tmpFileBuf;
        }
        else
        {
            myassert( 0 /* could not find child rec */ );
	    tmpFiles = NULL;
        }
    }
    else
        tmpFiles = NULL;

   /* Fill out the callback structure */
   data->actionLabel = XtNewString(request->clonedAction->label);
   data->associatedWidget = w;
   data->offset = 0;
   data->actionPtr = action;
   data->requestPtr = _DtCloneRequest(request);


   if ( _DtActionCommandInvoke(action->mask & _DtAct_WINTYPE_BITS, host, dir, 
                            action->u.cmd.execString.compiledMessage,
                            action->u.cmd.termOpts.compiledMessage,
                            request->currentHost,
                            procId,
                            tmpFiles,
			    CmdInvSuccessfulRequest, (XtPointer)data,
			    CmdInvFailedRequest, (XtPointer)data) ) 
        if (invp)
		SET_INV_CMD_QUEUED(invp->state);


   if ( tmpFiles != tmpFileBuf )
       XtFree(tmpFiles);
}


/*
 * Sets the 'currentHost' field within the request structure to
 * the name of the next exec host to try.  Before any of the exec hosts 
 * in the action definition are tried, we will try the host passed in as 
 * part of the request, if one was specified.
 */

static void
SetExecHost (
             ActionRequest * request )

{
   ActionPtr action = request->clonedAction;
   int hostCount = 0;
   int hostListSize = 0;
   char ** hostList = NULL;

   XtFree(request->currentHost);

   /* If this is the first call, we may need to parse the host list */
   if (action->u.cmd.execHostArray == NULL)
   {
      /* Explicitly specified execHost overrides action definition execHost */
      if (request->execHost)
         ParseHostList(request->execHost, &hostList, &hostListSize, &hostCount);
      else if (action->u.cmd.execHosts.compiledMessage)
      {
         ParseHostList(action->u.cmd.execHosts.compiledMessage, &hostList, 
                       &hostListSize, &hostCount);
      }

      RemoveDuplicateHostNames(hostList, &hostCount);

      action->u.cmd.execHostArray = hostList;
      action->u.cmd.execHostCount = hostCount;
   }

   if (action->u.cmd.execHostCount == 0)
   {
      /* 
       * Oh boy ... someone is trying to be nasty!  The only way we could
       * have gotten here was to have the action's 'EXEC_HOST' field set
       * to nothing but a prompt string, and then the user left the string
       * empty!  We'll default the local host, and hope it works.
       */
      /* 
       * fdt: we really should default to whatever has been configured
       *      as the default execution host, followed by the LocalHost,
       *      if they are not the same.
       */
      request->currentHost = _DtGetLocalHostName();
   }
   else
   {
      request->currentHost = 
           XtNewString(action->u.cmd.execHostArray[request->hostIndex]);
   }
}


/*
 * This function takes a string of comma-separated host names, and adds them
 * to the passed-in string array.
 */

static void
ParseHostList (
   char * hostString,
   char *** hostListPtr,
   int * hostListSizePtr,
   int * hostCountPtr )

{
   char * workString;
   char * nextHost;
   _Xstrtokparams	strtok_buf;

   workString = XtNewString(hostString);
   nextHost = _XStrtok(workString, ",", strtok_buf);

   while(nextHost)
   {
      nextHost = _DtStripSpaces(nextHost);

      if (strlen(nextHost) > 0)
      {
         if (*hostCountPtr >= *hostListSizePtr)
         {
            (*hostListSizePtr) += 5;
            (*hostListPtr) = (char **)XtRealloc((char *)(*hostListPtr),
                                         sizeof(char *) * (*hostListSizePtr));
         }

         (*hostListPtr)[*hostCountPtr] = XtNewString(nextHost);
         (*hostCountPtr)++;
      }

      nextHost = _XStrtok(NULL, ",", strtok_buf);
   }

   XtFree(workString);
}


/*
 * This function goes through the compiled list of exec hosts, and removes
 * any duplicate entries.  It is not very useful to attempt to execute on
 * a given host, more than once.
 */
static void
RemoveDuplicateHostNames (
             char ** hostList,
             int   * hostCountPtr )

{
   int i,j,k;

   for (i = 0; i < *hostCountPtr; i++)
   {
      for (j = i+1; j < *hostCountPtr; )
      {
         if (strcmp(hostList[i], hostList[j]) == 0)
         {
            /* Remove the second entry */
            XtFree(hostList[j]);
            for (k = j; k < (*hostCountPtr) - 1; k++)
               hostList[k] = hostList[k+1];
            (*hostCountPtr)--;
         }
         else
            j++;
      }
   }
}


/*
 * When one of the exec hosts fails, we add it to the list of failed
 * hostnames, so that if ultimately all of the hosts fail, we have a
 * list we can display within the error dialog.
 */

static void 
AddFailedHostToList (
   ActionRequest * request,
   String badHost )

{
   int curLen;

   if (request->badHostList)
      curLen = strlen(request->badHostList);
   else
      curLen = 0;

   request->badHostList = XtRealloc(request->badHostList,
                                    curLen + 10 + strlen(badHost));

   if (curLen > 0)
   {
      strcat(request->badHostList, ", ");
      strcat(request->badHostList, badHost);
   }
   else
      strcpy(request->badHostList, badHost);
}


/*
 *
 * This callback is invoked when the Command Invoker library has successfully 
 * exectued an action.  We need to free up everything associated with this
 * request.
 */

static void 
CmdInvSuccessfulRequest(
        char *message,
        void *data2)

{
   _DtActInvRecT *invp = NULL;
   _DtActChildRecT *childrecp = NULL;

   CallbackData *data = (CallbackData *) data2;

   /*
    * Mark this invocation step as done
    * The child process itself may not be done.
    */
   if ((invp = _DtActFindInvRec(data->requestPtr->invocId)) != NULL )
   {
       extern void *_DtCmdCheckQForId(DtActionInvocationID id);
       
       SET_INV_DONE(invp->state);
       RESET_INV_CMD_QUEUED(invp->state);
       /*
        * Are there still more commands queued for this request ?
        */
       if ( _DtCmdCheckQForId(invp->id) )
       {
                /*
                 * If so; set the command queued bit
                 */
                SET_INV_CMD_QUEUED(invp->state);
       }
	/*
	 * RWV:
	 * This may not be the right place to set the child state for
         * command actions.  The child process may already have communicated
         * its status via TT messaging OR it may already have exited.
	 * For now we set the state here -- till we find a better place.
	 */
       if (childrecp = _DtActFindChildRec(invp->id,data->requestPtr->childId))
           childrecp->childState = _DtActCHILD_ALIVE_UNKNOWN;
       else
	   myassert(0 /* could not find child record */ );
       
       _DtActExecutionLeafNodeCleanup(invp->id,NULL,0,True);
   }
   else
	myassert( 0 /* Couldn't find an invocation record */);

   _DtFreeRequest(data->requestPtr);
   XtFree(data->actionLabel);
   XtFree((char *)data);
}



/*
 * This callback is invoked when the Command Invoker library has failed 
 * to exectue an action.  It there are additional execHosts to be processed,
 * then try the command again, using the next host.  If there are no more
 * hosts, then post an error dialog, and give up (freeing all data 
 * associated with this request).
 */

static void 
CmdInvFailedRequest(
        char *error_message,
        void *data2)

{
   CallbackData * data = (CallbackData *) data2;
   String msg = error_message;
   ActionPtr action;
   ActionRequest * request;
   _DtActChildRecT *childp = NULL;
   _DtActInvRecT *invp = NULL;


   /*
    * If this was not the last host in the execHost list, then retry
    * the request, using the next host; if this was the last host,
    * then we failed, and it is time to post an error dialog.  If the
    * host list had only one item, then to be backwards compatible,
    * we will display the message returned by the command invoker.
    * Otherwise, we will simple display the list of execHosts, along
    * with a message saying they could not be accessed.
    */
   request = data->requestPtr;

   if (request->clonedAction)
      action  = request->clonedAction;
   else
      action  = data->actionPtr;
   request->hostIndex++;
   AddFailedHostToList(request, request->currentHost);

   if (  !(invp = _DtActFindInvRec(request->invocId) ) )
	myassert( 0 /* could not find invocation record */ );

   if ( !(childp=_DtActFindChildRec(request->invocId,request->childId)))
	myassert( 0 /* could not find child record */ );

   /*
    * Make sure the CMD_QUEUED bit is set correctly
    */
   if ( invp  )
   {
       extern void *_DtCmdCheckQForId(DtActionInvocationID id);
       
       SET_INV_DONE(invp->state);
       RESET_INV_CMD_QUEUED(invp->state);
       /*
        * Are there still more commands queued for this request ?
        */
       if ( _DtCmdCheckQForId(invp->id) )
       {
                /*
                 * If so; set the command queued bit
                 */
                SET_INV_CMD_QUEUED(invp->state);
       }
   }

   if (request->hostIndex < action->u.cmd.execHostCount)
   {
       /* 
	* Free up the child structure for the failed command request
	* We may be trying again on another host but a new child rec
	* will be allocated in PrepareAndExecute().
	*/

       if ( invp && childp )
       {
	    /* 
	     * Delete child record for failed exec on this host
             */
	    _DtActDeleteChildRec(invp,childp);
            SET_INV_PENDING(invp->state);
            if ( ! invp->numChildren )
		RESET_INV_WORKING(invp->state);
       }

      /* Retry, using the next host */
      PrepareAndExecuteAction(data->associatedWidget, request);
   }
   else
   {

       if ( invp && childp )
       {
	 /* 
	  * RWV ---  
	  * How can we tell if the Invocation COMPLETE bit
	  * needs to be set here?
          * How about if no invocation is pending or working?
	  */
	  SET_INV_ERROR(invp->state); 
	  childp->childState = _DtActCHILD_FAILED;
       }


      /* No more hosts (they all failed); put up error dialog */
      if (action->u.cmd.execHostCount <= 1)
      {
         /* Be backwards compatible */
         CommandInvokerError(data->associatedWidget,
                             action->label,
                             msg + data->offset);
      }
      else
      {
         MultiHostAccessError(data->associatedWidget, request->clonedAction->label, 
                              request->badHostList);
      }


      /* Cleanup */
      _DtActExecutionLeafNodeCleanup(invp->id,NULL,0,True);
      _DtFreeRequest(request);
      XtFree(data->actionLabel);
   }
   XtFree((char *)data);
}



/*
 * This function maps a filename relative to 'host' to be relative to
 * 'newHost'.  If newHost is NULL, then the local host is assumed.
 *
 * The returned string must be freed by the caller.
 */

char * 
_DtActMapFileName(
        const char * curHost,
        const char * dir,
        const char * file,
        const char * newHost )
{
   char buf[MAXPATHLEN];
   char *chp = NULL;
   int   clen = 0;
   char *netpath = NULL;
   char *path = NULL;

   /*
    * Create the full path name relative to curHost
    */
   
   buf[0]='\0';	/* empty string to start with */

   if ( dir )
	   strcpy(buf,dir);
   if ( file )
   {
	/* check if there is already a '/' separator */
       if ( *file != '/' ) 
       {
	   DtLastChar(buf,&chp,&clen);
	   if ( !( (clen == 1) && (*chp == '/')) )
	       strcat(buf,"/");
       }
       strcat(buf,file);
   }

   /* We should have constructed a file name string now */
   myassert(buf[0] != '\0');

   if (newHost)
   {
      if ( _DtIsSameHost(curHost,newHost) )
      {
         /*
          * The current host is the same as the new host 
          * so no file name translation is necessary
          */
         return XtNewString(buf);
      }
      /*
       * The current host is not the same as the new host -- find the
       * cannonical netfile name then reinterpret it on the new host.
       */
       switch ( tt_ptr_error(netpath = tt_host_file_netfile(curHost,buf)) )
       {
         case TT_OK:
            break;
         case TT_ERR_PATH:
            netpath = NULL;
            break;
	 case TT_ERR_DBAVAIL:
            netpath = NULL;
            break;
         case TT_ERR_DBEXIST:
            netpath = NULL;
            break;
         case TT_ERR_INTERNAL:
            netpath = NULL;
            break;
         default:
            netpath = NULL;
            break;
       }
       if ( netpath )
       {
          switch ( tt_ptr_error(path = tt_host_netfile_file(newHost,netpath)) )
          {
            case TT_OK:
               break;
            case TT_ERR_PATH:
               path = NULL;
               break;
            case TT_ERR_DBAVAIL:
               path = NULL;
               break;
            case TT_ERR_DBEXIST:
               path = NULL;
               break;
            case TT_ERR_INTERNAL:
               path = NULL;
               break;
            default:
               path = NULL;
               break;
          }
        }
   } else
   {
      /* 
       * Convert the file path which is relative to curHost to be
       * relative to the local host.
       */
      if ( _DtIsSameHost(curHost,NULL) )
      {
         /*
          * The current host is the same as the  local host 
          * so no file name translation is necessary
          */
         return XtNewString(buf);
      }
      /*
       * The current host is not the same as the local host -- find the
       * cannonical netfile name then reinterpret it on the local host.
       */
       switch ( tt_ptr_error(netpath = tt_host_file_netfile(curHost,buf)) )
       {
         case TT_OK:
            break;
         case TT_ERR_PATH:
            netpath = NULL;
            break;
	 case TT_ERR_DBAVAIL:
            netpath = NULL;
            break;
         case TT_ERR_DBEXIST:
            netpath = NULL;
            break;
         case TT_ERR_INTERNAL:
            netpath = NULL;
            break;
         default:
            netpath = NULL;
            break;
       }
       if ( netpath )
       {
          switch ( tt_ptr_error(path = tt_netfile_file(netpath)) ) 
          {
            case TT_OK:
               break;
            case TT_ERR_PATH:
               path = NULL;
               break;
            case TT_ERR_DBAVAIL:
               path = NULL;
               break;
            case TT_ERR_DBEXIST:
               path = NULL;
               break;
            case TT_ERR_INTERNAL:
               path = NULL;
               break;
            default:
               path = NULL;
               break;
          }
       }
   }
	
   /*
    * Free up the memory allocated by tooltalk filenaming code here so
    * downstream code need not worry about it.
    */
   if ( netpath )
	tt_free(netpath);
   if ( path )
   {
      char *s = path;
      path = XtNewString(s);
      tt_free(s);
   }
	
   return path;
}
