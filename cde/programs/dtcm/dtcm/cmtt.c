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
/*******************************************************************************
**
**  cmtt.c
**
**  $TOG: cmtt.c /main/4 1999/09/20 10:29:41 mgreess $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef lint
static 	char sccsid[] = "@(#)cmtt.c 1.13 95/05/01 Copyr 1993 Sun Microsystems, Inc.";
#endif
#include <EUSCompat.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Xm/XmAll.h>
#include <Tt/tttk.h>
#include <Dt/UserMsg.h>
#include "calendar.h"
#include "props.h"
#include "misc.h"
#include "cmfns.h"
#include "debug.h"


static Tt_message load_cb();

#ifdef FNS_DEMO
static const char *ptype = "Sun_DtCm";

static void
cm_iconic(
	Widget	win,
	int	iconic)

{
	if (iconic) {
		XIconifyWindow(XtDisplay(win),
				XtWindow(win),
				XScreenNumberOfScreen(XtScreen(win)));
	} else {
		XtPopup(win, XtGrabNone);
		XMapWindow(XtDisplay(win), XtWindow(win));
	}

	return;
}
#endif

static void
OKCB (Widget dialog, XtPointer client_data, XtPointer call_data)
{
    XtUnmanageChild((Widget) client_data);
}

void
DieFromToolTalkError(Calendar *c, char *errfmt, Tt_status status)
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

    xms_ok = XmStringCreateLocalized(catgets(c->DT_catd, 2, 3, "OK"));
    xms_errmsg = XmStringCreateLocalized(errmsg);
    xms_title = XmStringCreateLocalized(catgets(c->DT_catd, 2, 1,
			"Calendar - Warning"));

    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNokLabelString, xms_ok); n++;
    XtSetArg(args[n], XmNdialogTitle, xms_title); n++;
    XtSetArg(args[n], XmNmessageString, xms_errmsg); n++;
    XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;

    dialog = XmCreateWarningDialog(c->frame, "IconEditorError", args, n);
    XtAddCallback(dialog, XmNokCallback, OKCB, (XtPointer) dialog);
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

    _DtSimpleError("Dtcm", DtWarning, NULL, errmsg);

    XtFree(errmsg);
    XmStringFree(xms_ok);
    XmStringFree(xms_errmsg);
    XmStringFree(xms_title);
}


/* 
 * Initialize tooltalk.  Can be called multiple times: the first call
 * initializes tooltalk, subsequent calls are no-ops.
 *
 * Returns
 *		-1	Error.  Tooltalk not initialized
 *		0	Tooltalk already initialized
 *		1	Tooltalk succussfully intialized
 */
Tt_status
cmtt_init(
	char		*toolname,
	Calendar	*c,
	XtAppContext	context,
	Widget		 shell)

{
	int		ttfd;
	Tt_status	status;
	char		*ttenv;
	char		*session;

	if (c->tt_procid) {
		return 0;
	}

	ttenv = (char *)getenv("TT_SESSION");
	if (!ttenv || strlen(ttenv) == 0) {
		session = tt_X_session(XDisplayString(c->xcontext->display));
		tt_default_session_set(session);
		tt_free(session);
	}

	c->tt_procid = ttdt_open(&ttfd, toolname, "SunSoft", "%I", 1);
	status = tt_ptr_error(c->tt_procid);
	if (tt_is_err(status)) {
		c->tt_procid = NULL;
		return status;
	}

#ifdef FNS_DEMO
	/*
	 * Declare our ptype, and register the callback to handle
	 * Edit/Display/Compose requests
	 */
	status = ttmedia_ptype_declare(ptype, 0, load_cb, (void *)c, 1);
	if (tt_is_err(status)) {
		fprintf(stderr, "cmtt_init could not declare ptype: %s\n",
			tt_status_message(status));
		return status;
	}
#endif

	ttdt_session_join(0, NULL, shell, c, 1);

	XtAppAddInput(context, ttfd, (XtPointer)XtInputReadMask,
			tttk_Xt_input_handler, c->tt_procid);

	/* Process the message that started us, if any */
	tttk_Xt_input_handler( 0, 0, 0 );

	return TT_OK;
}

#ifdef FNS_DEMO
static Tt_message
contract_cb(
	Tt_message	msg,
	Tttk_op		op,
	Widget		shell,
	void		*clientdata,
	Tt_message	contract)
{

	DP(("contract_cb(msg=%d, op=%d, shell=%d, data=%d, cntrct=%d)\n",
		msg, op, shell, clientdata, contract));

	switch (op) {
	case TTDT_QUIT:
	case TTDT_GET_STATUS:
	case TTDT_PAUSE:
	case TTDT_RESUME:
		break;
	}
	/* For now do nothing */
	return msg;
}

/*
 * Handle Edit, Display and Compose requests
 */
static Tt_message
load_cb(
	Tt_message	msg,
	void		*clientdata,
	Tttk_op		op,
	Tt_status	diagnosis,
	unsigned char	*contents,
	int		len,
	char 		*file,
	char 		*docname
)

{
	Tt_status status;
	char	*p;
	char	addr_buf[256];
	Calendar	*c;
	Tt_pattern	*contract_pats;

	DP(("load_cb(msg=%d, op=%d, diag=%d, contents=%d, len=%d, file=%s, \
docname=%s, data=%d)\n",
		msg, op, diagnosis, contents, len, file ? file : "<nil>",
		docname ? docname : "<nil>", clientdata));

	c = (Calendar *)clientdata;

	if (diagnosis != TT_OK) {
		if (tt_message_status(msg) == TT_WRN_START_MESSAGE) {
			/*
			 * Error in start message!  we may want to exit
			 * here, but for now let toolkit handle error
			 */
			 return msg;
		}

		/* Let toolkit handle the error */
		return msg;
	}

	contract_pats = ttdt_message_accept(msg, contract_cb, clientdata,
						c->frame, 1, 1);

	if ((status = tt_ptr_error(contract_pats)) != TT_OK) {
		fprintf(stderr, "dtcm: load_cb could not accept message: %s\n",
			tt_status_message(status));
	} else {
	/*
		tttk_patterns_destroy(contract_pats);
	*/
	}

	if (file != NULL) {
		p = file;
	} else if (len > 0) {
		p = (char *)contents;
	} else {
		tttk_message_fail( msg, TT_DESKTOP_ENODATA, 0, 1 );
		p = NULL;
	}

	if (p) {
		if (cmfns_lookup_calendar(p, addr_buf, sizeof(addr_buf)) == 1) {
			cm_iconic(c->frame, 0);
			switch_it(c, addr_buf, main_win);
		}
	}

	tt_message_reply(msg);

	tt_free((caddr_t)contents);
	tt_free(file);
	tt_free(docname);

	return 0;
}
#endif
