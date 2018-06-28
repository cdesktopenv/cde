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
/* @(#)$XConsortium: action.c /main/3 1996/05/07 13:57:56 drk $ */

#include	<Xm/Xm.h>
#include	<Dt/Dt.h>
#include	<Dt/Action.h>

#include	"xims.h"

static void	DbReloadProc(/* cd */);
static bool	init_action_env(/*  */);
static void	action_cb(long unsigned id, XtPointer cd,
			  DtActionArg *args, int argcnt, DtActionStatus status);
static void	wait_action_done(/*  */);

static bool			actionEnabled = False;
static int			actionDone = True;
static int			actionErr = NoError;
static DtActionInvocationID	actionId = 0;


	/* *****  DtAction functions  ***** */

static void	DbReloadProc(XtPointer cd)
{
    DPR(("DbReloadProc(): action database updated\n"));
    DtDbLoad();	/* Pick up any dynamic changes to the database files */
}

static bool	init_action_env(void)
{
    init_window_env();

    if (actionEnabled != True) {

	if (DtAppInitialize(appC, Dpy, TopW, Wargv[0], DTIMS_CLASS) == False) {
		/* DtInitialize() has already logged an appropriate error msg */
	    return False;
	}
	DtDbLoad();	/* Load the filetype/action databases */
	DtDbReloadNotify(DbReloadProc, NULL);

	actionEnabled = True;
    }

    return actionEnabled;
}

static void	action_cb(long unsigned id, XtPointer cd /* unused */,
                          DtActionArg *args, int argcnt, DtActionStatus status)
{
    if (actionId == 0) {
	DPR(("action_cb(): invalid id %d != %d\n", id, actionId));
	return;
    }
    if (id != actionId) {
	DPR(("action_cb(): invalid id %d != %d\n", id, actionId));
	return;
    }

    switch ((DtActionStatus) status) {
	case DtACTION_INVOKED:
	    DPR(("  ACTION_INVOKED\n"));
	    break;

	case DtACTION_DONE:
	    DPR(("  ACTION_DONE\n"));
	    actionId = 0;
	    actionDone = True;
	    break;

	case DtACTION_CANCELED:
	case DtACTION_FAILED:
	    DPR(("  ACTION_%sED\n",
		(status == DtACTION_CANCELED) ? "CANCEL" : "FAIL"));
	    if (args && argcnt > 0) {
		int	i;
		for (i = 0; i < argcnt; i++) {
		    if (args[i].argClass == DtACTION_FILE) {
			XtFree(args[i].u.file.name);
		    } else if (args[i].argClass == DtACTION_BUFFER) {
			XtFree(args[i].u.buffer.bp);
			XtFree(args[i].u.buffer.type);
			XtFree(args[i].u.buffer.name);
		    }
		}
		XtFree((void *)args);
	    }
	    actionId = 0;
	    actionDone = False;
	    break;

	case DtACTION_STATUS_UPDATE:
	    DPR(("  ACTION_STATUS_UPDATE\n"));
	    actionDone = False;
	    break;

	default:	/* ignore */
	    DPR(("  unknown status (%d)\n", status));
	    break;
    }

    if (actionId == 0) {	/* action done */
	send_dtims_msg(WIN_ST_ACTION_DONE, actionDone ? NoError : ErrRemoteAction);
    }

    return;
}


static void	wait_action_done(void)
{
    XEvent event;

    DPR(("wait_action_done(): Begin\n"));

    if (actionId == 0)	return;

    XtSetSensitive(TopW, False);
    XtAddEventHandler(TopW, NoEventMask, True, dtims_msg_proc, 0);

    while (actionId != 0 || actionErr != NoError) {
	XtAppNextEvent(appC, &event);
	XtDispatchEvent(&event);
    }

    XtRemoveEventHandler(TopW, XtAllEvents, True, dtims_msg_proc, 0);
    XtSetSensitive(TopW, True);

    DPR(("wait_action_done():  End\n"));
    return;
}

int	invoke_action(char *action, char *host)
{
    if (init_action_env() != True)	return ErrInternal;
  
    DPR(("invoke_action(%s): host=%s\n", action, host));

    if (!action || !*action)	return ErrInternal;
    if (host && !*host)	host = NULL;

    setErrFunc(action);

#ifdef	DEBUG
    if (DtActionExists(action) != True) {
	DPR(("invoke_action(%s): action not exists\n", action));
    }
#endif

    /* Invoke the specified action */
    actionId = DtActionInvoke(TopW, action, NULL, 0,
		NULL, host, NULL, False, action_cb, NULL);

    DPR(("invoke_action(%s): actionId=%d\n", actionId));

    wait_action_done();

    return actionErr;
}


void	send_dtims_msg(int msg, int errcode)
{
    XEvent ev;

    ev.xclient.type = ClientMessage;
    ev.xclient.window = winEnv.atom_owner;
    ev.xclient.message_type = winEnv.atom_status;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = msg;
    ev.xclient.data.l[1] = errcode;
    ev.xclient.data.l[2] = ev.xclient.data.l[3] = ev.xclient.data.l[4] = 0L;

    XSendEvent(Dpy, XtWindow(TopW), False, NoEventMask, &ev);
}

void	dtims_msg_proc(Widget w, XtPointer cd, XEvent *event,
                       Boolean *continue_dispatch /* never changed */)
{
    XClientMessageEvent *ev = &event->xclient;
    int	msg, errcode;

    DPR(("dtimsMessageProc()\n"));

    if (event->type != ClientMessage)	return;
    if (ev->message_type != winEnv.atom_status && ev->format != 32) {
	DPR(("\tinvalid type([%d]%s) or format(%d) -- ignored\n",
	    ev->message_type, XGetAtomName(Dpy, ev->message_type), ev->format));
	return;
    }
    msg = (int)ev->data.l[0];
    errcode = (int)ev->data.l[1];

    switch (msg) {
	case WIN_ST_REMOTE_CONF: 
	case WIN_ST_REMOTE_RUN: 
		actionId = 0;

	case WIN_ST_ACTION_DONE:
	    DPR(("\tmsg=%d  errcode=%d\n", msg, errcode));
	    actionErr = errcode;
	    break;

	default:
	    DPR(("\tunknown msg value (%d) -- ignored\n", msg));
	    break;
    }
    return;
}

