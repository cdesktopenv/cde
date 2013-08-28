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
/* $TOG: dtinfo_start.c /main/9 1999/09/20 13:26:59 mgreess $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 * This file contains the main program for: dtinfo_start
 */

#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>

#include <Xm/XmP.h>
#include <Xm/XmAll.h>

#include <Dt/EnvControlP.h>
#include <Dt/Action.h>
#include <Dt/DtGetMessageP.h>
#include <Dt/MsgLog.h>
#include <Dt/Dt.h>

#include <Tt/tt_c.h>

#include "dtinfo_start.h"
#include "dtinfo_start.opnums"

/*
 * Operation names, number of arguments and default action names
 */
static const char	* LOAD_INFO_LIB_STRING = "DtInfo_LoadInfoLib";
static const int	LOAD_INFO_LIB_ARGS = 3;
static const char	* LOAD_INFO_LIB_ACTION = "DtInfoStart";

static const char	* SHOW_INFO_AT_LOC_STRING = "DtInfo_ShowInfoAtLoc";
static const int	SHOW_INFO_AT_LOC_ARGS = 4;
static const char	* SHOW_INFO_LIB_SECTION_ACTION = "DtInfoStartAtLoc";

/*
 * Static global vars
 */
static Widget 		top_level;

static XtAppContext 	app_context;

static const int	time_out = 1000;	/* milliseconds */

static const int	SET_NUM = 1;		/* message cat set number */

static const char	* name = "DtInfo"; /* ptype name and Xt name */

/*
 * Forward declarations for static functions
 */
static int InvokeAction (
	char			* prog_name, 
	int			op_num,
	const char		* action_name, 
	char			* exec_host, 
	char			* info_lib, 
	char			* topic);

static void ActionDoneCallback (
	DtActionInvocationID	id,
	XtPointer		client_data,
	DtActionArg		* args,
	int			num_args,
	DtActionStatus		status);

static void Exit (
	XtPointer		client_data,
	XtIntervalId		id);

static Tt_status ConnectToMessageServer (
	char			* prog_name);

static void ReceiveMessage (
	XtPointer 		client_data,
	int 			* fd,
	XtInputId 		* id);

static void LogToolTalkError (
	DtMsgLogType		msg_type,
	char			* prog_name,
	char			* function_name,
	Tt_status		error_num);
static void DieFromToolTalkError (
	Widget			parent,
	char			* errfmt,
	Tt_status		status);


int
main ( 
	int 			argc,
	char 			** argv) 
{
	Display			* display;
	Arg 			args[10];
	int 			i = 0;
	Tt_status		status;
  
	XtSetLanguageProc (NULL, NULL, NULL);

	_DtEnvControl (DT_ENV_SET);

	XtToolkitInitialize ();

	app_context = XtCreateApplicationContext ();

	if (!(display = XtOpenDisplay (app_context, NULL, NULL, name,
				       NULL, 0, &argc, argv))) {
		DtMsgLogMessage (argv[0], DtMsgLogError, 
			(char *) GETMESSAGE (SET_NUM, 7, "XtOpenDisplay() failed.  Perhaps the DISPLAY environment\nvariable is not set or is invalid."));
		exit (1);
	}
  
	/*
	 * Create a minimalist application shell - needed by
	 * DtActionInvoke.
	 */
	XtSetArg (args[i], XtNallowShellResize, 	True); 	i++;
	XtSetArg (args[i], XtNmappedWhenManaged, 	False);	i++;
	XtSetArg (args[i], XtNheight, 			1); 	i++;
	XtSetArg (args[i], XtNwidth, 			1); 	i++;

	top_level = XtAppCreateShell (argv[0], 
				      name, 
				      topLevelShellWidgetClass, 
				      display, 
				      args, 
				      i);

	XtRealizeWidget (top_level);

	/*  
	 * Initialize the desktop
	 */
	if (DtAppInitialize (app_context, display, top_level,
			     argv[0], (char *)name) == False) {
		DtMsgLogMessage (argv[0], DtMsgLogError, 
			(char *) GETMESSAGE (SET_NUM, 8, "DtAppInitialize() failed.  Perhaps the desktop environment\nis not properly installed."));
   		exit (1);
	}

	/*
	 * Connect to the message server
	 */
	status = ConnectToMessageServer (argv[0]);
	if (TT_OK != status) {
		char *errfmt;

		/*
		 * An error Message has already been logged.
		 */
		errfmt = GETMESSAGE (2, 2,
			  "Could not connect to ToolTalk:\n%s\nExiting ...");
		DieFromToolTalkError (top_level, errfmt, status);
	}

	/* 
	 * Load the datatypes and actions 
	 */
	DtDbLoad();

	XtAppMainLoop (app_context);

	return (0);
}


static int
InvokeAction (
	char			* prog_name, 
	int			op_num,
	const char		* action_name, 
	char			* exec_host, 
	char			* info_lib, 
	char			* topic)
{
	DtActionArg 		* args;
	int			num_args = 1;
	DtActionInvocationID	action_id;

	if (op_num == SHOW_INFO_AT_LOC)
		num_args = 2;

	args = (DtActionArg *) XtCalloc (num_args, sizeof (DtActionArg));
	if (!args) {
		DtMsgLogMessage (prog_name, DtMsgLogError, 
			(char *) GETMESSAGE (SET_NUM, 6,
			"Cannot invoke an action because malloc(1) failed."));
		return (0);
	}

	args[0].argClass = DtACTION_FILE;
	args[0].u.file.name = info_lib;

	if (op_num == SHOW_INFO_AT_LOC) {
		args[1].argClass = DtACTION_BUFFER;
		args[1].u.buffer.name = topic;
		args[1].u.buffer.size = strlen (topic) + 1;
		args[1].u.buffer.writable = False;
	}

	action_id = DtActionInvoke (top_level,
				    (char *) action_name,
				    args,
				    num_args,
				    NULL,
				    exec_host,
				    NULL,
				    True,
        			    (DtActionCallbackProc) ActionDoneCallback,
				    prog_name);

#if defined(DEBUG)
	DtMsgLogMessage (prog_name, DtMsgLogError, "Invoked %s\n", action_name);
#endif

	/*
	 * The following test, comment and code are from:
	 *   $TOP/dtaction/Main.c
	 *
	 * "Set up a timer if we didn't get a valid procId --
	 *  since there will be no invocation update in that case."
	 */
	if (!action_id) {
		XtAppAddTimeOut (app_context,
				 10, 
				 (XtTimerCallbackProc) Exit,
				 NULL);
	}

	return (1);
}


/* ARGSUSED */
static void 
ActionDoneCallback (
	DtActionInvocationID	id,
	XtPointer		client_data,
	DtActionArg		* args,
	int			num_args,
	DtActionStatus		status)
{
#if defined(DEBUG)
	char *status_name;

	switch (status) {
		case DtACTION_DONE:
		    status_name = "DtACTION_DONE";
		    break;
		case DtACTION_OK:
		    status_name = "DtACTION_DONE";
		    break;
        	case DtACTION_INVOKED:
		    status_name = "DtACTION_DONE";
		    break;
		case DtACTION_FAILED:
		    status_name = "DtACTION_DONE";
		    break;
		case DtACTION_CANCELED:
		    status_name = "DtACTION_DONE";
		    break;
		case DtACTION_INVALID_ID:
		    status_name = "DtACTION_DONE";
		    break;
		case DtACTION_STATUS_UPDATE:
		    status_name = "DtACTION_DONE";
		    break;
        	default:
		    status_name = "UNKNOWN";
		    break;
	}
#endif
	switch (status) {
		case DtACTION_DONE:
		case DtACTION_OK:
#if defined(DEBUG)
			DtMsgLogMessage ("dtinfo_start", DtMsgLogError,
				 	 "Action returned %s\n", status_name);
#endif
			XtAppAddTimeOut(app_context, 10 * time_out, 
					(XtTimerCallbackProc) Exit,
	      				NULL);

        	case DtACTION_INVOKED:
		case DtACTION_FAILED:
		case DtACTION_CANCELED:
		case DtACTION_INVALID_ID:
		case DtACTION_STATUS_UPDATE:
        	default:
#if defined(DEBUG)
			DtMsgLogMessage ("dtinfo_start", DtMsgLogError,
				 	 "Action returned %s\n", status_name);
#endif
			XtAppAddTimeOut(app_context, 10 * time_out, 
					(XtTimerCallbackProc) Exit,
	      				NULL);
	}
}


static Tt_status
ConnectToMessageServer (
	char			* prog_name)
{
	int			mark;
	char			* procid;
	int			ttfd;
	Tt_status		status;
  
	mark = tt_mark();

	procid = tt_open();
	status = tt_pointer_error (procid);
  	if (status != TT_OK) {
		LogToolTalkError (DtMsgLogError, prog_name,
				    "tt_open", status);
		tt_release (mark);
		return (status);
	}
  
	status = tt_ptype_declare (name);
	if (status != TT_OK) {
		LogToolTalkError (DtMsgLogError, prog_name, 
				    "tt_ptype_declare", status);
		tt_release (mark);
		return (status);
	}

	ttfd = tt_fd ();
	if ((tt_int_error (ttfd)) != TT_OK) {
		LogToolTalkError (DtMsgLogError, prog_name, 
				    "tt_fd", status);
		tt_release (mark);
		return (status);
	}

	XtAppAddInput (app_context, ttfd, (XtPointer) XtInputReadMask,
	     	       ReceiveMessage, (char *) prog_name);

	status = tt_session_join (tt_default_session());
	if ((tt_int_error (ttfd)) != TT_OK) {
		LogToolTalkError (DtMsgLogError, prog_name, 
				    "tt_session_join", status);
		tt_release (mark);
		return (status);
	}
  
	tt_release(mark);

	return (TT_OK);
}


/* ARGSUSED */
static void
ReceiveMessage (
	XtPointer 		client_data,
	int 			* fd,
	XtInputId 		* id)
{
	static int		last_op_num = 0;
	int			op_num;
	int                     opstatus;
	int                     num_args;
	Tt_message		message;
	const char 		* action_name;
	char 			* exec_host;
	char 			* locale;
	char 			* topic = NULL;
	char			* prog_name = (char *) client_data;
	static			char lang[200];
	const char		* op_name;
	char			* info_lib = NULL;

	message = tt_message_receive();

	/* from tt_message_receive(3), and after examining other TT clients */
	if (message == 0) return;

	if (tt_pointer_error (message) != TT_OK) {
		LogToolTalkError (DtMsgLogError, prog_name, 
				    "tt_message_receive", 
				    tt_pointer_error (message));
		exit (1);
	}

	op_num = tt_message_opnum (message);
	switch (op_num) {

		case LOAD_INFO_LIB:
			op_name = LOAD_INFO_LIB_STRING;
    			break;

		case SHOW_INFO_AT_LOC:
			op_name = SHOW_INFO_AT_LOC_STRING;
			break;

		default: {
			/*
			 * Don't know how I got this message
			 */
			DtMsgLogMessage (prog_name, DtMsgLogError, 
				(char *) GETMESSAGE (SET_NUM, 11, "The message operation '%d' is not supported."),
				op_num);

			tt_message_reject (message);
			tt_message_destroy(message);
    			exit (1);
		}
  	}

	/*
	 * Check the number of arguments
	 */
	num_args =  tt_message_args_count (message);
	if ((op_num == LOAD_INFO_LIB &&    num_args != LOAD_INFO_LIB_ARGS) ||
	    (op_num == SHOW_INFO_AT_LOC && num_args != SHOW_INFO_AT_LOC_ARGS)) {

		DtMsgLogMessage (prog_name, DtMsgLogError, 
			(char *) GETMESSAGE (SET_NUM, 12, "Message '%s' does not have the required\nnumber of arguments '%d'."),
			op_name, 
			(op_num == LOAD_INFO_LIB) ?
				LOAD_INFO_LIB_ARGS : SHOW_INFO_AT_LOC_ARGS);
		tt_message_reject (message);
		tt_message_destroy(message);
		exit (1);
	}

	/*
	 * Check for an info lib
	 */
	info_lib = tt_message_file (message);
	if ((tt_ptr_error (info_lib) != TT_OK) || info_lib == NULL) {
		/*
		 * Use DTINFOLIBDEFAULT if it is defined
		 */
		if ((info_lib = getenv ("DTINFOLIBDEFAULT")) == NULL) {
			DtMsgLogMessage (prog_name, DtMsgLogError, 
				(char *) GETMESSAGE (SET_NUM, 10, "An InfoLib was not provided in the '%s' message\nand the environment variable DTINFOLIBDEFAULT was not defined."),
				op_name);
			exit (1);
		}
	}

	if ((action_name = tt_message_arg_val (message, 0)) == NULL) {

		action_name = (op_num == LOAD_INFO_LIB) ? 
			LOAD_INFO_LIB_ACTION : SHOW_INFO_LIB_SECTION_ACTION;

		DtMsgLogMessage (prog_name, DtMsgLogWarning,
			(char *) GETMESSAGE (SET_NUM, 13, "Message '%s' specified a NULL action name.\nAction '%s' will be invoked."),
			op_name,
			action_name);
	}

	if ((exec_host = tt_message_arg_val (message, 1)) == NULL) {

		DtMsgLogMessage (prog_name, DtMsgLogWarning, 
			(char *) GETMESSAGE (SET_NUM, 14, "Message '%s' specified a NULL execution host."),
			op_name);
	}

	if ((locale = tt_message_arg_val (message, 2)) == NULL) {

		DtMsgLogMessage (prog_name, DtMsgLogWarning,
			(char *) GETMESSAGE (SET_NUM, 15, "Message '%s' specified a NULL locale."),
			op_name);
	}
	else {
		/*
		 * Put locale into the environment so that DtActionInvoke
		 * will propagate it to the dtinfo process.
		 */
		(void) snprintf (lang, sizeof(lang), "LANG=%s", locale);
		(void) putenv (lang);
	}

	if (op_num == SHOW_INFO_AT_LOC) {
		if ((topic = tt_message_arg_val (message, 3)) == NULL) {

			DtMsgLogMessage (prog_name, DtMsgLogError, 
				(char *) GETMESSAGE (SET_NUM, 16, "Message '%s' specified a NULL topic."),
				op_name);
			exit (1);
		}
	}

        if (op_num == last_op_num)
	{
#if defined(DEBUG)
	    DtMsgLogMessage("dtinfo_start", DtMsgLogError,
			    "Received a duplicate tooltalk message:  %s",
			    (op_num == LOAD_INFO_LIB) ? 
			      "LOAD_INFO_LIB" : "SHOW_INFO_AT_LOC" );
#endif
	}
	else
	{
#if defined(DEBUG)
	    DtMsgLogMessage(
		"dtinfo_start", DtMsgLogError,
		"Received a tooltalk message:  %s\nInvoking action %s",
		(op_num==LOAD_INFO_LIB) ? "LOAD_INFO_LIB" : "SHOW_INFO_AT_LOC",
		action_name);
#endif
	    last_op_num = op_num;
    	    opstatus = InvokeAction (prog_name, 
				     op_num, 
				     action_name, 
			 	     exec_host, 
				     info_lib,
				     topic);
	    if (!opstatus) {
 		    tt_message_fail (message);
		    exit (1);
	    }
	}

 	tt_message_reply (message);

	tt_message_destroy(message);
}


static void
LogToolTalkError (
	DtMsgLogType		msg_type,
	char			* prog_name,
	char			* function_name,
	Tt_status		error_num)
{
	char			*message;
        message = tt_status_message(error_num);
	DtMsgLogMessage (prog_name, msg_type, "%s: %s", function_name, message);
}

#define GETXMSTRING(s, m, d)    XmStringCreateLocalized(GETMESSAGE(s,m,d))

static void
ExitCB (Widget dialog, XtPointer client_data, XtPointer call_data)
{
    exit((size_t) client_data);
}

static void
DieFromToolTalkError(Widget parent, char *errfmt, Tt_status status)
{
    Arg		 args[10];
    Widget	 dialog, dialogShell;
    char	*errmsg, *statmsg;
    XmString	 xms_errmsg, xms_ok, xms_title;
    int		 n, errmsglen;

    if (! tt_is_err(status)) return;

    statmsg = tt_status_message(status);
    errmsglen = strlen(errfmt) + strlen(statmsg) + 2;
    errmsg = XtMalloc(errmsglen);
    snprintf(errmsg, errmsglen, errfmt, statmsg);

    xms_ok = GETXMSTRING(2, 3, "OK");
    xms_errmsg = XmStringCreateLocalized(errmsg);
    xms_title = GETXMSTRING(2, 1, "Dtinfo: Error");

    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNokLabelString, xms_ok); n++;
    XtSetArg(args[n], XmNdialogTitle, xms_title); n++;
    XtSetArg(args[n], XmNmessageString, xms_errmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;

    dialog = XmCreateErrorDialog(parent, "DtinfoStartError", args, n);
    XtAddCallback(dialog, XmNokCallback, ExitCB, (XtPointer) status);
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));

    /*
     * Disable the frame menu from dialog since we don't want the user
     * to be able to close dialogs with the frame menu
     */
    dialogShell = XtParent(dialog);
    n = 0;
    XtSetArg(args[n], XmNmwmDecorations, MWM_DECOR_ALL | MWM_DECOR_MENU); n++;
    XtSetValues(dialogShell, args, n);
    XtManageChild(dialog);
    XtRealizeWidget(dialogShell);

    XtFree(errmsg);
    XmStringFree(xms_ok);
    XmStringFree(xms_errmsg);
    XmStringFree(xms_title);

    while (TRUE)
      XtAppProcessEvent(XtWidgetToApplicationContext(dialog), XtIMAll);
}

/* ARGSUSED */
static void
Exit (
	XtPointer		client_data,
	XtIntervalId		id)
{
	/*
	 * The following code to determine if DtActionInvoke
	 * resulted in the posting a dialog is from:
	 *   $TOP/dtaction/Main.c
	 */
	if (top_level->core.num_popups ==  0)
		exit(0);

	XtAppAddTimeOut (app_context, 
			 time_out, 
			 (XtTimerCallbackProc) Exit, 
			 NULL);

}
