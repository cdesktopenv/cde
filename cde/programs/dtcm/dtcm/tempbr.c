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
**  tempbr.c
**
**  $TOG: tempbr.c /main/5 1999/02/23 09:42:53 mgreess $
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
static  char sccsid[] = "@(#)tempbr.c 1.48 95/03/28 Copyr 1991 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/DialogS.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/XmStrDefs.h>
#include <Xm/MwmUtil.h>
#include <Dt/HourGlass.h>
#ifdef FNS_DEMO
#include <Tt/tt_c.h>
#include <Tt/tttk.h>
#endif
#include "util.h"
#include "timeops.h"
#include "tempbr.h"
#include "datefield.h"
#include "props.h"
#include "editor.h"
#include "deskset.h"
#include "help.h"
#ifdef FNS
#include "cmfns.h"
#endif

static void tb_cancel_cb();
static void tb_close_cb();
static void tempbr_show_cb(Widget w, XtPointer data, XtPointer cbs);
#if defined(__linux__)
#include <string.h>
#else
extern char *strdup(const char *);
#endif

#ifdef FNS_DEMO
/* This stuff is currently broken */
Tt_message
tempbr_sundex_ttedit_cb(
	Tt_message	m,
	void		*c_data,
	Tttk_op		op, 
	unsigned char	*contents,
	int		len,
	char		*file)

{
	char	*user_str = "user:";
	char	*p;

	Tempbr	*tb = (Tempbr *)c_data;
	/*
	cmfns_name_to_namepanel(contents, tb->np);
	*/
	/* XXX Yech, but hell it is just a demo */
	if (strncmp(user_str, (char *)contents, strlen(user_str)) == 0) {
		p = strchr((char *)contents, ':');
		p++;
	} else {
		p = (char *)contents;
	}
		
	XmTextFieldSetString(tb->name_text, p);
	return m;
}

static void
tempbr_sundex_cb(Widget w, XtPointer data, XtPointer cbs)
{
	Tempbr	*tb = (Tempbr *)data;
	Tt_message	msg = NULL;
	Tt_status	status;

	msg = ttmedia_load(0, (Ttmedia_load_msg_cb)tempbr_sundex_ttedit_cb,
			tb, TTME_EDIT, "Sun_Info", data, strlen(data) + 1,
			NULL, "calendar-address", 0);

	status = tt_ptr_error(msg);
	if (tt_is_err(status)) {
		fprintf(stderr, "dtcm: ttmedia_load: %s\n",
			tt_status_message(status));
		return;
	}

	status = tt_message_send(msg);
	if (tt_is_err(status)) {
		fprintf(stderr, "dtcm: tt_message_send: %s\n",
			tt_status_message(status));
	}

	return;
}
#endif /* FNS_DEMO */

static void
tempbr_show_cb(Widget w, XtPointer data, XtPointer cbs)
{
        Calendar *c = calendar;
	char *name;
	Tempbr	*tb = (Tempbr *)data;
	char msg_buf[256];
	char buf[256];

	if ((name = XmTextGetString(tb->name_text)) == NULL) {
		return;
	}

	_DtTurnOnHourGlass(tb->frame);

#ifdef FNS
	if (cmfns_use_fns((Props *)c->properties)) {
		sprintf(msg_buf, catgets(c->DT_catd, 1, 647,
			"Looking up %s..."), name);
		set_message(tb->show_message, msg_buf);
		XmUpdateDisplay(tb->show_message);
		XFlush(XtDisplay(w));
		/* Translate name to a calendar address */
		cmfns_lookup_calendar(name, buf, sizeof(buf));
		name = buf;
	}
#endif

	sprintf(msg_buf, catgets(c->DT_catd, 1, 559, "Browsing %s..."),
		name);
	set_message(tb->show_message, msg_buf);
	XFlush(XtDisplay(w));
        switch_it(tb->cal, name, tempbrowser);
	_DtTurnOffHourGlass(tb->frame);
	return;
}

extern caddr_t
make_std_tempbr(Calendar *c)
{
	Tempbr *tb;
	XmString	label_str;
	Widget		separator, button_form;
	char		*title;

	if (c->tempbr == NULL) {
                c->tempbr = (caddr_t)ckalloc(sizeof(Tempbr));
                tb = (Tempbr*)c->tempbr;
        }
        else
                tb = (Tempbr*)c->tempbr;

	title = XtNewString(catgets(c->DT_catd, 1, 560, "Calendar : Show Other Calendar"));
	tb->frame = XtVaCreatePopupShell("frame",
                xmDialogShellWidgetClass, c->frame,
                XmNtitle, 		title,
		XmNallowShellResize, 	True,
                XmNmappedWhenManaged, 	False,
                NULL);
	XtFree(title);

        tb->form = XtVaCreateWidget("form",
                xmFormWidgetClass, 	
		tb->frame,
                XmNautoUnmanage, 	False,
		XmNfractionBase, 	3,
                XmNhorizontalSpacing, 	4,
                XmNverticalSpacing, 	4,
		XmNmarginWidth,         0,
		XmNmarginHeight,        0,
                NULL);

	label_str = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 410, "User Name:"));
        tb->name_label = XtVaCreateWidget("name_label", 
		xmLabelWidgetClass, 
		tb->form,
		XmNlabelString,		label_str,
                XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
                XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset, 		11,
                NULL);
	XmStringFree(label_str);

        tb->name_text = XtVaCreateWidget("show_text",
		xmTextWidgetClass, 
		tb->form,
                XmNbottomAttachment, 	XmATTACH_OPPOSITE_WIDGET,
                XmNbottomWidget, 	tb->name_label,
		XmNbottomOffset, 	-4,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		tb->name_label,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	3,
		XmNrightOffset, 	10,
		XmNcolumns,		30,
                NULL);

	separator = XtVaCreateWidget("separator",
                xmSeparatorGadgetClass,
                tb->form,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		tb->name_text,
		XmNtopOffset, 		5,
                NULL);

        button_form = XtVaCreateWidget("form",
                xmFormWidgetClass, 	
		tb->form,
                XmNautoUnmanage, 	False,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNleftOffset,      	5,
                XmNrightAttachment,     XmATTACH_FORM,
                XmNrightOffset,      	5,
                XmNtopAttachment,       XmATTACH_WIDGET,
		XmNtopWidget, 		separator,
                XmNhorizontalSpacing, 	4,
		XmNfractionBase, 	3,
                NULL);
 
        label_str = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 562, "Show"));
        tb->show_button = XtVaCreateWidget("show_button",
		xmPushButtonWidgetClass, 
		button_form,
		XmNlabelString, label_str,
                XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, separator,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNleftPosition, 0,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNrightPosition, 1,
		XmNnavigationType, XmTAB_GROUP,
                NULL);
	XmStringFree(label_str);

        label_str = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 680, "Close"));
        tb->cancel_button = XtVaCreateWidget("cancelButton",
		xmPushButtonWidgetClass, 
		button_form,
		XmNlabelString, label_str,
                XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, separator,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNleftPosition, 1,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNrightPosition, 2,
		XmNuserData, c,
                NULL);
	XmStringFree(label_str);
        XtAddCallback(tb->cancel_button, 
			XmNactivateCallback, tb_cancel_cb, NULL);

        label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
        tb->help_button = XtVaCreateWidget("helpButton",
		xmPushButtonWidgetClass, button_form,
		XmNlabelString, label_str,
                XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, separator,
		XmNleftAttachment, XmATTACH_POSITION,
		XmNleftPosition, 2,
		XmNrightAttachment, XmATTACH_POSITION,
		XmNrightPosition, 3,
                NULL);
	XmStringFree(label_str);
        XtAddCallback(tb->help_button, XmNactivateCallback,
                (XtCallbackProc)help_cb, SHOW_OTHER_CAL_HELP_BUTTON);
        XtAddCallback(tb->form, XmNhelpCallback,
                (XtCallbackProc)help_cb, (XtPointer) SHOW_OTHER_CAL_HELP_BUTTON);

#ifdef FNS_SUNDEX_DEMO
	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 648,
					    "SunDex"));
        tb->show_button = XtVaCreateWidget("sundex_button",
		xmPushButtonWidgetClass, tb->form,
		XmNlabelString,		label_str,
                XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 9,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget,	tb->name_text,
		XmNleftOffset, 5,
                NULL);
	XmStringFree(label_str);
	XtAddCallback(tb->show_button, XmNactivateCallback,
		tempbr_sundex_cb, tb);
#endif

       	tb->show_message = XtVaCreateWidget("show_message",
		xmLabelGadgetClass, tb->form,
		XmNalignment, XmALIGNMENT_BEGINNING,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, tb->cancel_button,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
		NULL);

	set_message(tb->show_message, "");

	XtAddCallback(tb->show_button, XmNactivateCallback,
			tempbr_show_cb, tb);
        ds_position_popup(c->frame, tb->frame, DS_POPUP_LOR);

        /* set default button */
        XtVaSetValues(button_form, 
			XmNdefaultButton, 	tb->show_button, 
			XmNcancelButton, 	tb->show_button, 
			NULL);
        XtVaSetValues(tb->form, 
			XmNdefaultButton, 	tb->show_button, 
			XmNcancelButton, 	tb->show_button, 
			NULL);

        XmProcessTraversal(tb->name_text, XmTRAVERSE_CURRENT);
        XtVaSetValues(tb->form, XmNinitialFocus, tb->name_text, NULL);

	ManageChildren(button_form);
	ManageChildren(tb->form);
	XtManageChild(tb->form);
        XtVaSetValues(tb->frame, XmNmappedWhenManaged, True, NULL);
	XtRealizeWidget(tb->frame);

	XtPopup(tb->frame, XtGrabNone);

	return(caddr_t)tb;
}

caddr_t
make_tempbr(Calendar *c)
{
	caddr_t tb;

	tb = make_std_tempbr(c);
	((Tempbr *)tb)->cal = c;

	return tb;
}

static void
tb_cancel_cb(
        Widget  widget,
        XtPointer client_data,
        XmPushButtonCallbackStruct *cbs)
{
        Calendar *c;
        Tempbr      *tb;

        XtVaGetValues(widget, XmNuserData, &c, NULL);
        tb = (Tempbr *)c->tempbr;

	XmTextSetString(tb->name_text, "");
        XtPopdown(tb->frame);

        return;
}

static void
tb_close_cb(Widget w, XtPointer data, XtPointer cbs)
{
	Tempbr	*tb = (Tempbr *)data;

	XtPopdown(tb->frame);

	return;
}
