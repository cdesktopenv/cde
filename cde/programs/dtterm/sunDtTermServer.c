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
/* $TOG: sunDtTermServer.c /main/6 1999/09/17 13:29:51 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "TermHeader.h"
#include "TermPrimDebug.h"
#include "TermView.h"
#include "TermPrimMessageCatI.h"
#include "TermPrimSetPty.h"
#ifdef  LOG_USAGE
#include "DtTermLogit.h"
#endif  /* LOG_USAGE */
#include <signal.h>
#include <errno.h>
#include <Dt/Service.h>
#include <Dt/UserMsg.h>
#include <Dt/DtP.h>
#include <Tt/tttk.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <Xm/XmAll.h>

#define GETXMSTRING(s, m, d)	XmStringCreateLocalized(GETMESSAGE(s,m,d))

#define VENDOR                "CDE"
#define VERSION               "1.0"

static char * procId;
static Tt_pattern * DtTermToolTalkPattern = NULL;
static Boolean sendStopped = False;
XtInputId ProcessToolTalkInputId = 0;
static int ttFd;
char DTTERM_CLASS_NAME[] = "Dtterm";
char *displayString = NULL;
Widget refWidget;

#ifdef  TIMEOUT
#define STICKAROUND     15              /* 15 minutes... */
static XtIntervalId waitId = (XtIntervalId) 0;
#endif  /* TIMEOUT */

extern void
sunSetupIA(Widget w);

void
DieFromToolTalkError(Widget, char*, Tt_status);

Tt_message 
SessionCallback( Tt_message msg, void * client_data, Tt_message contract);

Tt_callback_action
HandleTtRequest(Tt_message msg, Tt_pattern pat);

static void
dttermNewHandler(Tt_message msg);

#ifdef  TIMEOUT
static void TimeOut(XtPointer clientData, XtIntervalId *id);
#endif  /* TIMEOUT */

Boolean
FinalizeToolTalkSession( )
{
    Tt_status 	ttRc;
    int 	i;

    if (DtTermToolTalkPattern && tt_ptr_error(DtTermToolTalkPattern) == TT_OK) {
        ttRc = ttdt_session_quit(NULL, DtTermToolTalkPattern, 1);
        if (ProcessToolTalkInputId)
            XtRemoveInput(ProcessToolTalkInputId);
        return(True);
    }
    ttRc = ttdt_close(NULL, NULL, sendStopped);
    return(True);
}

Tt_message
SessionCallback( Tt_message msg, void * client_data, Tt_message contract)
{
    char 	*opString = tt_message_op(msg);
    Tttk_op 	op = tttk_string_op(opString);

    tt_free(opString);

    switch (op) {
        int i;

        default:
            break;
        case TTDT_QUIT:
            if (contract == 0) {
                tt_message_reply(msg);
                FinalizeToolTalkSession( );
                exit(0);
            }
          /* Should we send some type of tt failure message? */
            return 0;
    }
    return msg;
}

int
InitializeToolTalkSession(Widget topLevel, Boolean sendStarted)
{
    Tt_status 	ttstat;
    char	*errfmt;

    sendStopped = sendStarted;
    procId = ttdt_open(&ttFd,
                       DTTERM_CLASS_NAME,
                       VENDOR,
                       VERSION,
                       sendStarted);

    ttstat = tt_ptr_error(procId);
    errfmt =
      GETMESSAGE(10, 2, "Could not connect to ToolTalk:\n%s\nExiting ...");
    DieFromToolTalkError( topLevel, errfmt, ttstat );

    ttstat = tt_ptype_declare("DT_Terminal");
    DieFromToolTalkError( topLevel, "tt_ptype_declare", ttstat );

    ttstat = tt_ptype_opnum_callback_add("DT_Terminal", 0, HandleTtRequest);
    DieFromToolTalkError( topLevel, "tt_ptype_opnum_callback_add", ttstat );

    ttstat = tt_ptype_declare("SDT_Terminal");
    DieFromToolTalkError( topLevel, "tt_ptype_declare", ttstat );

    ttstat = tt_ptype_opnum_callback_add("SDT_Terminal", 0, HandleTtRequest);
    DieFromToolTalkError( topLevel, "tt_ptype_opnum_callback_add", ttstat );

    /*
     * If we were started by a message, the following call to
     * tttk_Xt_input_handler will process it.  Otherwise,
     * tttk_Xt_input_handler will just return.
     */
    tttk_Xt_input_handler(NULL, 0, 0);

    return(TT_OK);
}

int
FinishToolTalkInit(Widget topLevel)
{
    ProcessToolTalkInputId =
            XtAppAddInput(XtWidgetToApplicationContext(topLevel),
                          ttFd, (XtPointer)XtInputReadMask,
                          tttk_Xt_input_handler, procId);
    DtTermToolTalkPattern = ttdt_session_join(tt_default_session( ),
                                              SessionCallback,
                                              topLevel,
                                              NULL,
                                              1);
    if (tt_is_err(tt_ptr_error(DtTermToolTalkPattern))) {
        ttdt_close(NULL, NULL, sendStopped);
        return(0);
    }
    refWidget = topLevel;
    if (!displayString) {
	/* This dtterm -sdtserver must have been started up from either a
	 * session startup or the command line.  */
	displayString = DisplayString(XtDisplay(refWidget));
    }


}

Tt_callback_action
HandleTtRequest(Tt_message msg, Tt_pattern pat)
{
    char 	*op;
    Tt_status 	status;

    op = tt_message_op( msg );
    status = tt_ptr_error( op );
    if (tt_is_err(status) || (op == 0)) {
	/* Let tttk_Xt_input_handler() Do The Right Thing */
	return TT_CALLBACK_CONTINUE;
    }
    if ((!strcmp(op, "SDtTerminal_New")) || (!strcmp(op, "DtTerminal_New")))  {
	if ((getuid() == tt_message_uid(msg)) &&
	    (getgid() == tt_message_gid(msg))) {
	    dttermNewHandler( msg );
	} else {
	    tt_message_reject(msg);
	    tt_message_destroy(msg);
	    return TT_CALLBACK_PROCESSED;
	}
    } else {
	tt_free(op);
	return TT_CALLBACK_CONTINUE;
    }

    tt_free(op);
    return TT_CALLBACK_PROCESSED;
}

/*ARGSUSED*/
static void
dttermNewHandler(
    Tt_message msg)
{
    Widget                shellWidget;
    int                   pid = -1;
    Arg                   arglist[20];
    int                   argcnt = 0;
    char                  *msgFile;
    char                  numArgs;
    int                   i, j, k;
    char                  *displayEnv, *newDisplayString;

    msgFile = tt_message_file(msg);
    if (tt_is_err(tt_ptr_error(msgFile))) msgFile = 0;
    numArgs = tt_message_args_count(msg);
    if (tt_is_err( tt_int_error(numArgs))) numArgs = 0;
    for (i = 0; i < numArgs; i++) {
	char *vtype, *val;

	vtype = tt_message_arg_type(msg, i);
	if ((vtype == 0) || (tt_is_err(tt_ptr_error(vtype)))) {
	    continue;
	}
	val = tt_message_arg_val(msg, i);
	if(strcmp(vtype, "-display") == 0) {
	    newDisplayString = XtNewString(val);
	}
	tt_free( val );
	tt_free( vtype );
    }

    if (!displayString) {
	/* This tt message is part of an action dtterm -server startup.  */
	displayString = newDisplayString;
	displayEnv = (char *)malloc(strlen("DISPLAY=") +
				    strlen(displayString) + 2);
	displayEnv[0]=0;
	strcat(displayEnv, "DISPLAY=");
	strcat(displayEnv, displayString);
	putenv(displayEnv);
	tt_free(msgFile);
	tt_message_reply(msg);
	tttk_message_destroy(msg);
	return;
    } else {
	if (strcmp(displayString, newDisplayString)) {
	    tt_free(msgFile);
	    tt_message_reject(msg);
	    XtFree(newDisplayString);
	    return;
	}
	XtFree(newDisplayString);
    }
    
    argcnt = 0;
    (void) XtSetArg(arglist[argcnt], XmNallowShellResize, True);
	    argcnt++;
    shellWidget = XtAppCreateShell((char *) 0, "Dtterm",
	    applicationShellWidgetClass, XtDisplay((Widget) refWidget),
	    arglist, argcnt);

    /* parse off messageFields and build the dttermview arglist... */
    argcnt = 0;

    /* create the dtterm... */
    (void) CreateInstance(shellWidget, "dtTermView", arglist, argcnt, True);

    (void) XtRealizeWidget(shellWidget);
#ifdef sun
    sunSetupIA(shellWidget);
#endif

#ifdef  TIMEOUT
	    /* since we now have active instances, we can remove our
	     * wait timeout...
	     */
	    if (waitId) {
		(void) XtRemoveTimeOut(waitId);
		waitId = (XtIntervalId) 0;
	    }
#endif  /* TIMEOUT */
	    
    tt_free(msgFile);
    tt_message_reply(msg);
    tttk_message_destroy(msg);
}



Boolean
ServerStartSession(Widget topLevel, int argc, char **argv, Boolean loginShell, 
		   char **commandToExec)
{
    return(InitializeToolTalkSession(topLevel, True));
}

#ifdef  TIMEOUT
static void
TimeOut(
    XtPointer             clientData,
    XtIntervalId         *id
)
{
    /* if we have no instances active, go away... */
    if (InstanceCount <= 0) {
	(void) exit(0);
    }

    /* otherwise, clear the waitId... */
    if (*id == waitId) {
	waitId = (XtIntervalId) 0;
    }
}
#endif  /* TIMEOUT */


static void
ExitCB (Widget dialog, XtPointer client_data, XtPointer call_data)
{
    exit((int) client_data);
}

void
DieFromToolTalkError(Widget parent, char *errfmt, Tt_status status)
{
    Arg		 args[10];
    Widget	 dialog, dialogShell;
    char	*errmsg, *statmsg, *title;
    XmString	 xms_errmsg, xms_ok, xms_title;
    int		 n;

    if (! tt_is_err(status)) return;
    statmsg = tt_status_message(status);
    errmsg = XtMalloc(strlen(errfmt) + strlen(statmsg) + 2);
    sprintf(errmsg, errfmt, statmsg);

    xms_ok = GETXMSTRING(10, 3, "OK");
    xms_errmsg = XmStringCreateLocalized(errmsg);
    xms_title = GETXMSTRING(10, 1, "Terminal - Error");

    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNokLabelString, xms_ok); n++;
    XtSetArg(args[n], XmNdialogTitle, xms_title); n++;
    XtSetArg(args[n], XmNmessageString, xms_errmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;

    dialog = XmCreateErrorDialog(parent, "IconEditorError", args, n);
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

    _DtSimpleError("Dtterm", DtFatalError, NULL, errmsg);

    XtFree(errmsg);
    XmStringFree(xms_ok);
    XmStringFree(xms_errmsg);
    XmStringFree(xms_title);

    while (TRUE)
      XtAppProcessEvent(XtWidgetToApplicationContext(dialog), XtIMAll);
}
