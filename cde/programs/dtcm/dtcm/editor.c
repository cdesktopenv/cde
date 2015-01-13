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
/* $TOG: editor.c /main/13 1999/07/01 11:44:34 mgreess $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/LabelG.h>
#include <Xm/DragDrop.h>
#include <Xm/Screen.h>
#include <Dt/HourGlass.h>
#include <Dt/Dt.h>
#include <Dt/Dnd.h>
#include <Dt/Icon.h>
#include "editor.h"
#include "calendar.h"
#include "datefield.h"
#include "deskset.h"
#include "getdate.h"
#include "format.h"
#include "timeops.h"
#include "props.h"
#include "props_pu.h"
#include "group_editor.h"
#include "browser.h"
#include "util.h"
#include "dnd.h"
#include "help.h"
#ifdef SVR4
#include <sys/param.h>
#endif /* SVR4 */

static void e_build_expand(Editor *);
static void clear_flag_on_modify(Widget, XtPointer, XtPointer);

/* Absolute value macro */
#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#endif

static void set_editor_msg_defaults();

/*******************************************************************************
**
**  Functions static to editor.c
**
*******************************************************************************/

static void
clear_flag_on_modify(Widget w, XtPointer uData, XtPointer cbData)
{
  int *flagP = (int *)uData;

  *flagP = 0;
}

/*
**  This function will take appointment values and stuff them into a form.
*/
static void
appt_to_form(Editor *e, CSA_entry_handle a) {
	char			buf[MAXNAMELEN];
	CSA_return_code		status;
	Dtcm_appointment	*appt;

	if (!e->reminders.bfpm_form_mgr) {
		e_build_expand(e);
		set_rfp_defaults(&e->rfp);
		set_reminders_defaults(&e->reminders);
	}

	if (!dssw_appt_to_form(&e->dssw, a))
		return;
	e->dsswFlags = 0;
	if (!rfp_appt_to_form(&e->rfp, a))
		return;
	e->rfpFlags = 0;
	if (!reminders_appt_to_form(&e->reminders, a))
		return;

	appt = allocate_appt_struct(appt_read, 
				        e->cal->general->version,
					CSA_ENTRY_ATTR_ORGANIZER_I, 
					NULL);
	status = query_appt_struct(e->cal->cal_handle,a, appt);
	backend_err_msg(e->frame, e->cal->view->current_calendar, status,
			((Props_pu *)e->cal->properties_pu)->xm_error_pixmap);
	if (status != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return;
	}
	sprintf(buf, "%s:  %s",	catgets(e->cal->DT_catd, 1, 251, "Author"),
		appt->author->value->item.calendar_user_value->user_name);
	set_message(e->message_text, buf);
	free_appt_struct(&appt);
}

/*
**  List selection procedure will get the correct appointment, then call
**  appt_to_form to load it into the UI.
*/
static void
e_list_select_proc(Widget w, XtPointer client_data, XtPointer data) {
	CSA_entry_handle		a;
	Editor			*e = (Editor *)client_data;
	XmListCallbackStruct	*cbs = (XmListCallbackStruct *) data;

	if (a = editor_nth_appt(e, cbs->item_position - 1))
		appt_to_form(e, a);

	XtSetSensitive(e->delete_button, True);
	XtSetSensitive(e->change_button, True);
}
/*
**  Quit handler for the editor and the editor view popup
*/
static void
e_quit_handler(Widget w, XtPointer cdata, XtPointer data) {
	Editor *e = (Editor *)cdata;
	Widget popMeDown;

	if (w == e->frame) {
		editor_clean_up(e);
		e->editor_is_up = False;
		popMeDown = e->base_form_mgr;
	} else if (w == e->view_frame)
	{
		e->editor_view_is_up = False;
		popMeDown = e->view_form;
	}

	XtUnmanageChild(popMeDown);
}


static void
e_view_cancel_cb(
        Widget  widget,
        XtPointer client_data,
        XmPushButtonCallbackStruct *cbs)
{
	Editor		*e = (Editor *)client_data;

        XtUnmanageChild(e->view_form);
}

/*
**  Build the popup to display, in list fashion, the appts for a given period.
*/
static void
e_build_view_popup(Editor *e) {
	int		ac = 0;
	Arg		args[15];
	XmString 	xmstr;
	Calendar	*c = e->cal;
	Widget		separator;
	char		*title;

	/*
	**  Dialog shell and stuff
	*/
	title = XtNewString(catgets(c->DT_catd, 1, 1086, 
				    "Calendar : Appointment List"));
	e->view_frame = XtVaCreatePopupShell("appt_list",
		xmDialogShellWidgetClass, 	e->cal->frame,
		XmNtitle,			title,
		XmNdeleteResponse, 		XmDO_NOTHING,
		XmNautoUnmanage, 		False,
		XmNallowShellResize, 		True,
		NULL);
	XtFree(title);
	setup_quit_handler(e->view_frame, e_quit_handler, (caddr_t)e);

	e->view_form = XtVaCreateWidget("apptform",
      			xmFormWidgetClass, 	e->view_frame,
      			XmNfractionBase, 	2,
			XmNmarginWidth,		2,
			XmNautoUnmanage, 	False,
      			NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 680, "Close"));
	e->view_cancel_button = XtVaCreateWidget("cancel",
			xmPushButtonWidgetClass, 	e->view_form,
      			XmNlabelString, 		xmstr,
      			XmNleftAttachment, 		XmATTACH_POSITION,
      			XmNleftPosition, 		0,
      			XmNrightAttachment, 		XmATTACH_POSITION,
      			XmNrightPosition, 		1,
      			XmNbottomAttachment, 		XmATTACH_FORM,
      			XmNleftOffset, 			10,
      			NULL);
	XmStringFree(xmstr);

	XtAddCallback(e->view_cancel_button, XmNactivateCallback, (XtCallbackProc) e_view_cancel_cb, e);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
	e->view_help_button = XtVaCreateWidget("help",
      			xmPushButtonWidgetClass, 	e->view_form,
      			XmNlabelString, 		xmstr,
      			XmNleftAttachment, 		XmATTACH_POSITION,
      			XmNleftPosition, 		1,
      			XmNrightAttachment, 		XmATTACH_POSITION,
      			XmNrightPosition, 		2,
      			XmNbottomAttachment, 		XmATTACH_FORM,
      			XmNrightOffset, 		10,
      			NULL);
	XmStringFree(xmstr);

	separator = XtVaCreateWidget("separator",
                xmSeparatorGadgetClass,
                e->view_form,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
                XmNbottomAttachment,   	XmATTACH_WIDGET,
                XmNbottomWidget,        e->view_cancel_button,
		XmNbottomOffset,	5,
                NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 703, "context"));
        e->view_list_label= XtVaCreateWidget("type_label",
                xmLabelGadgetClass, 	e->view_form,
                XmNtopAttachment, 	XmATTACH_FORM,
                XmNleftAttachment, 	XmATTACH_FORM,
                XmNleftOffset, 		5,
                XmNtopOffset, 		5,
                XmNlabelString, 	xmstr,
                NULL);
        XmStringFree(xmstr);


	ac = 0;
	XtSetArg(args[ac], XmNwidth, 300); ac++;
	XtSetArg(args[ac], XmNheight, 200); ac++;
	XtSetArg(args[ac], XmNscrolledWindowMarginHeight, 5); ac++;
	XtSetArg(args[ac], XmNscrolledWindowMarginWidth, 5); ac++;
	XtSetArg(args[ac], XmNlistSpacing, 5); ac++;
	XtSetArg(args[ac], XmNlistSizePolicy, XmCONSTANT); ++ac;
	XtSetArg(args[ac], XmNrightAttachment, XmATTACH_FORM); ++ac;
	XtSetArg(args[ac], XmNrightOffset, 2); ++ac;
	XtSetArg(args[ac], XmNleftAttachment, XmATTACH_FORM); ++ac;
	XtSetArg(args[ac], XmNtopAttachment, XmATTACH_WIDGET); ++ac;
	XtSetArg(args[ac], XmNtopWidget, e->view_list_label); ++ac;
	XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_WIDGET); ++ac;
	XtSetArg(args[ac], XmNbottomWidget, separator); ++ac;
	XtSetArg(args[ac], XmNbottomOffset, 5); ++ac;
	XtSetArg(args[ac], XmNdoubleClickInterval, 5); ++ac;
	e->view_list = XmCreateScrolledList(e->view_form, "view_list",
					    args, ac);

        XtAddCallback(e->view_help_button, XmNactivateCallback, 
		(XtCallbackProc)help_cb, APPT_LIST_HELP_BUTTON);
        XtAddCallback(e->view_form, XmNhelpCallback, 
		(XtCallbackProc)help_cb, (XtPointer) APPT_LIST_HELP_BUTTON);

	XtManageChild(e->view_list);
	ManageChildren(e->view_form);
	XtManageChild(e->view_form);
	e->editor_view_is_up = False;
}

/*
**  This function creates the expando stuff on a form manager.
*/
static void
e_build_expand(
	Editor 		*e)
{
	Props		*p = (Props *)e->cal->properties;

	/*
	**  Build the rfp "widget"
	*/
	e->rfpFlags = 0;
	build_rfp(&e->rfp, e->cal, e->base_form_mgr);
	XtVaSetValues(e->rfp.rfp_form_mgr,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		3,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		e->separator1,
		NULL);
	ManageChildren(e->rfp.rfp_form_mgr);

	XtAddCallback(e->rfp.repeat_menu, XmNselectionCallback,
		      clear_flag_on_modify, (XtPointer)&e->rfpFlags);

	/*
	**  Call the build_reminders function to build that widget
	*/
	build_reminders(&e->reminders, e->cal, e->base_form_mgr);
	XtVaSetValues(e->reminders.bfpm_form_mgr,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget,		e->rfp.rfp_form_mgr,
		XmNrightOffset, 	3,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		e->separator1,
		NULL);
	ManageChildren(e->reminders.bfpm_form_mgr);
}

/*
**  Button action procedures manage and unmanage the "extra" stuff in the UI to
**  make it visible and invisible to the user.
*/
static void
e_expand_ui_proc(Widget w, XtPointer client_data, XtPointer data) {
	Editor		*e = (Editor *)client_data;
	Props_pu	*p = (Props_pu *)e->cal->properties_pu;
	XmString	xmstr;
	Dimension       h, height, width;
	static Boolean	expand_state_closed = True;

	XtVaGetValues(e->appt_list_sw, 
			XmNheight, 	&height, 
			XmNwidth, 	&width, 
			NULL);

	if (expand_state_closed) {
		Widget	children[2];

		if (!e->reminders.bfpm_form_mgr) {
			e_build_expand(e);
			set_rfp_defaults(&e->rfp);
			set_reminders_defaults(&e->reminders);
		}

		XtRealizeWidget(e->rfp.rfp_form_mgr);
		XtRealizeWidget(e->reminders.bfpm_form_mgr);

		xmstr = XmStringCreateLocalized(catgets(e->cal->DT_catd, 1, 625,
							"Less"));
		XtVaSetValues(e->expand_ui_button, XmNlabelString, xmstr,
			      NULL);
		XmStringFree(xmstr);

		XtVaGetValues(e->reminders.bfpm_form_mgr, XmNheight, &h, NULL);
                XtVaSetValues(e->separator1, XmNbottomOffset, h + 10, NULL);

		children[0] = e->rfp.rfp_form_mgr;
		children[1] = e->reminders.bfpm_form_mgr;

		XtManageChildren(children, 2);

		expand_state_closed = False;
	} else {
		xmstr = XmStringCreateLocalized(catgets(e->cal->DT_catd, 1, 626,
							"More"));
		XtVaSetValues(e->expand_ui_button, XmNlabelString, xmstr,
			      NULL);
		XmStringFree(xmstr);
		XtUnmanageChild(e->rfp.rfp_form_mgr);
		XtUnmanageChild(e->reminders.bfpm_form_mgr);
		XtVaSetValues(e->separator1, XmNbottomOffset, 0, NULL);
		expand_state_closed = True;
	}
	XtVaSetValues(e->appt_list_sw, 
		XmNheight, height, 
		XmNwidth, width, 
		NULL);
}


/*
**  This function will consume form values and stuff them into an appointment.
*/
static Dtcm_appointment*
form_to_appt(Editor *e) {
	boolean_t		all_ok;
	Dtcm_appointment	*a;

	/* This code needs to conditionally decide whether to write out
	   the repeating event specification as an old style set of
	   specifications, or a new style recurrence rule.  In the case
	   of data versions 1-3, it needs to be the old style.  In the
	   case of version 4 it needs to be the new style. */

	a = allocate_appt_struct(appt_write, e->cal->general->version, NULL);

	all_ok = dssw_form_to_appt(&e->dssw, a, e->cal->view->current_calendar,
				   e->cal->view->date);
	if (all_ok)
		all_ok = rfp_form_to_appt(&e->rfp, a,
					  e->cal->view->current_calendar);
	if (all_ok)
		all_ok = reminders_form_to_appt(&e->reminders, a, e->cal->view->current_calendar);

	if (!all_ok) {
		free_appt_struct(&a);
		return NULL;
	}
	a->type->value->item.sint32_value = CSA_TYPE_EVENT;
	a->subtype->value->item.string_value = strdup(CSA_SUBTYPE_APPOINTMENT);
	a->state->value->item.sint32_value = CSA_X_DT_STATUS_ACTIVE;

	return a;
}

/*
**  Action procedures
*/
static void
e_insert_proc(Widget w, XtPointer client_data, XtPointer data) {
	Editor			*e = (Editor *)client_data;
	CSA_entry_handle	new_a = 0;
	Dtcm_appointment	*a;
	Calendar		*c = e->cal;

	if (strcmp(c->calname, c->view->current_calendar) != 0) {
		char buf[BUFSIZ], buf2[BUFSIZ];
		int answer = 0;
		char *title;
		char *ident;
		/*
		 * Make sure user really meant to insert appointment
		 * into somebody elses calendar.
		 */
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 210, "The appointment will be scheduled in the calendar\nyou are currently browsing.  Do you still want to schedule it?"));
		sprintf(buf2, "%s %s", catgets(c->DT_catd, 1, 211, "Schedule in"),
			c->view->current_calendar);
		title = XtNewString(catgets(c->DT_catd, 1, 212,
					    "Calendar : Schedule Appointment"));
		ident = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
		answer = dialog_popup(e->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, buf,
			BUTTON_IDENT, 1, ident,
			BUTTON_IDENT, 2, buf2,
			NULL);
		XtFree(ident);
		XtFree(title);
		if (answer == 1)
			return;
	}

	_DtTurnOnHourGlass(e->frame);
	if ((a = form_to_appt(e)) != NULL) {
		if ((editor_insert(a, &new_a, e->cal)) == True)
			csa_free((CSA_buffer)new_a);
		free_appt_struct(&a);
	}
	_DtTurnOffHourGlass(e->frame);
}

static void
e_delete_proc(Widget w, XtPointer client_data, XtPointer data) {
	int		*item_list = NULL, item_cnt = 0;
	CSA_entry_handle	a;
	Editor		*e = (Editor *)client_data;
	Calendar	*c = e->cal;
	Props_pu	*p = (Props_pu *)e->cal->properties_pu;
	static int	answer;

	if (strcmp(c->calname, c->view->current_calendar) != 0) {
		char buf[BUFSIZ], buf2[BUFSIZ];
		char *title;
		char *ident;
		/*
		 * Make sure user really meant to delete appointment
		 * from somebody elses calendar.
		 */
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 1004, "This appointment is in a calendar owned by someone else.\nDo you want to delete it anyway ?"));
		sprintf(buf2, "%s %s", catgets(c->DT_catd, 1, 1005, "Delete from"),
			c->view->current_calendar);
		title = XtNewString(catgets(c->DT_catd, 1, 252,
				"Calendar : Appointment Editor - Delete"));
		ident = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
		answer = dialog_popup(e->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, buf,
			BUTTON_IDENT, 1, ident,
			BUTTON_IDENT, 2, buf2,
			NULL);
		XtFree(ident);
		XtFree(title);
		if (answer == 1)
			return;
	}

	_DtTurnOnHourGlass(e->frame);
	if (!XmListGetSelectedPos(e->appt_list, &item_list, &item_cnt)) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 252,
				"Calendar : Appointment Editor - Delete"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 253,
				"Select an appointment and DELETE again."));
		char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(e->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident);
		XtFree(text);
		XtFree(title);
		_DtTurnOffHourGlass(e->frame);
		return;
	} 

	if (!(a = editor_nth_appt(e, item_list[0] - 1))) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 252,
				"Calendar : Appointment Editor - Delete"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 256,
				"Internal error selecting appointment.\nAppointment was not deleted."));
		char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(e->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident,
			BUTTON_HELP, RESELECT_ERROR_HELP,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident);
		XtFree(text);
		XtFree(title);
		XtFree((XtPointer)item_list);
		_DtTurnOffHourGlass(e->frame);
		return;
	}
	XtFree((XtPointer)item_list);

	editor_delete(a, e->cal);
	_DtTurnOffHourGlass(e->frame);
}

static void
e_change_proc(Widget w, XtPointer client_data, XtPointer data) {
	int			*item_list = NULL, item_cnt = 0;
	Editor			*e = (Editor *)client_data;
	Calendar		*c = e->cal;
	CSA_entry_handle	old_a, updated_a;
	Props_pu		*p = (Props_pu *)e->cal->properties_pu;
	static int		answer;
	Dtcm_appointment	*new_a;

	if (strcmp(c->calname, c->view->current_calendar) != 0) {
		char buf[BUFSIZ], buf2[BUFSIZ];
		char *title = XtNewString(catgets(c->DT_catd, 1, 258,
				"Calendar : Appointment Editor - Change"));
		char *ident = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
		/*
		 * Make sure user really meant to insert appointment
		 * into somebody elses calendar.
		 */
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 1003, "This appointment is in a calendar owned by someone else.\nDo you want to change it anyway ?"));
		sprintf(buf2, "%s %s", catgets(c->DT_catd, 1, 1006, "Change in"),
			c->view->current_calendar);
		answer = dialog_popup(e->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, buf,
			BUTTON_IDENT, 1, ident,
			BUTTON_IDENT, 2, buf2,
			NULL);
		XtFree(ident);
		XtFree(title);
		if (answer == 1)
			return;
	}

	_DtTurnOnHourGlass(e->frame);
	if (!XmListGetSelectedPos(e->appt_list, &item_list, &item_cnt)) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 258,
				"Calendar : Appointment Editor - Change"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 259,
				"Select an appointment and CHANGE again."));
		char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(e->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident);
		XtFree(text);
		XtFree(title);
		_DtTurnOffHourGlass(e->frame);
		return;
	} 

	if (!(old_a = editor_nth_appt(e, item_list[0] - 1))) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 1075,
				"Calendar : Error - Change Appointment"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 1007,
				"Internal error selecting appointment.\nAppointment was not changed."));
		char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(e->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident);
		XtFree(text);
		XtFree(title);
		XtFree((XtPointer)item_list);
		_DtTurnOffHourGlass(e->frame);
		return;
	}
	XtFree((XtPointer)item_list);

	if ((new_a = form_to_appt(e)) == NULL) {
		_DtTurnOffHourGlass(e->frame);
		return;
	}

	if (editor_change(new_a, old_a, &updated_a, e->cal))
		csa_free((CSA_buffer)updated_a);

	free_appt_struct(&new_a);
	_DtTurnOffHourGlass(e->frame);
}

static void
e_clear_proc(Widget w, XtPointer client_data, XtPointer data) {
	Editor *e = (Editor *)client_data;

	set_editor_defaults(e, 0, 0, False);
	add_all_appt(e);
}

static void
e_close_proc(Widget w, XtPointer client_data, XtPointer data) {
	Editor *e = (Editor *)client_data;

	XtUnmanageChild(e->base_form_mgr);
	e->editor_is_up = False;
}

/*
 * dragMotionHandler
 *
 * Determine if the pointer has moved beyond the drag threshold while button 1
 * was being held down.
 */
void
FormApptDragMotionHandler(
        Widget          dragInitiator,
        XtPointer       clientData,
        XEvent         *event)
{
        int             diffX, diffY;
        Calendar	*c = (Calendar *) clientData;
	Editor		*e = (Editor *) c->editor;
	Dimension	source_height, source_width;
	Position	source_x, source_y;
 
        if (!e->doing_drag) {

		/* check to see if the iniital value was within the 
		   bounds for the drag source icon. */

		XtVaGetValues(e->drag_source, 
				XmNx, &source_x,
				XmNy, &source_y,
				XmNheight, &source_height,
				XmNwidth, &source_width,
				NULL);

		if ((event->xmotion.x < source_x) ||
		    (event->xmotion.y < source_y) ||
		    (event->xmotion.x > (int) (source_x + source_width)) ||
		    (event->xmotion.y > (int) (source_y + source_height)))
			return;

                /*
                 * If the drag is just starting, set initial button down coords
                 */
                if (e->initialX == -1 && e->initialY == -1) {
                        e->initialX = event->xmotion.x;
                        e->initialY = event->xmotion.y;
                }
                /*
                 * Find out how far pointer has moved since button press
                 */
                diffX = e->initialX - event->xmotion.x;
                diffY = e->initialY - event->xmotion.y;
 
                if ((ABS(diffX) >= DRAG_THRESHOLD) ||
                    (ABS(diffY) >= DRAG_THRESHOLD)) {
                        e->doing_drag = True;
			ApptDragStart(dragInitiator, event, c, SingleEditorIcon);
                        e->initialX = -1;
                        e->initialY = -1;
                }
        }
}

extern void
e_make_editor(Calendar *c)
{
	int		cnt = 0;
	Arg		args[15];
        Editor		*e = (Editor *)c->editor;
	Props_pu	*p = (Props_pu *)c->properties_pu;
	XmString	xmstr;
	Dimension	label_height;
	XtTranslations	new_translations;
	XFontSetExtents listfontextents;
	static char 	translations[] = "\
		~c ~s ~m ~a <Btn1Down>:\
		 dtcm-process-press(ListBeginSelect,TranslationDragStart)\n\
		c ~s ~m ~a <Btn1Down>:\
		 dtcm-process-press(ListBeginToggle,TranslationDragStart)";
	static char 	btn2_translations[] = "\
		~c ~s ~m ~a <Btn2Down>:\
		 dtcm-process-press(ListBeginSelect,TranslationDragStart)\n\
		c ~s ~m ~a <Btn2Down>:\
		 dtcm-process-press(ListBeginToggle,TranslationDragStart)\n\
		<Btn2Motion>:ListButtonMotion()\n\
		~c ~s ~m ~a <Btn2Up>:ListEndSelect()\n\
		c ~s ~m ~a <Btn2Up>:ListEndToggle()";
	Boolean 	btn1_transfer; 

	new_translations = XtParseTranslationTable(translations);
			 
	e->cal = c;

	/*
	**  Dialog shell and the base form
	*/
	e->frame = XtVaCreatePopupShell("frame",
		xmDialogShellWidgetClass, e->cal->frame,
		XmNdeleteResponse, 	XmDO_NOTHING,
		XmNallowShellResize, 	True,
		NULL);
	set_editor_title(e, c->view->current_calendar);
	setup_quit_handler(e->frame, e_quit_handler, (caddr_t)e);

	e->base_form_mgr = XtVaCreateWidget("base_form_mgr",
		xmFormWidgetClass, 	e->frame,
		XmNautoUnmanage, 	False,
		XmNfractionBase, 	8,
		NULL);

	/*
	**  Build the stuff in the upper portion of the form - the dssw widget,
	**  the scrolling appointment list, the expand/contract button, and the
	**  first separator.
	*/
	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 628, "Time  What"));
	e->list_label = XtVaCreateWidget("label",
		xmLabelGadgetClass, 	e->base_form_mgr,
		XmNlabelString, 	xmstr,
		XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset, 		5,
		NULL);
	XmStringFree(xmstr);

	XtVaGetValues(e->list_label, XmNheight, &label_height, NULL);

	e->message_text = XtVaCreateWidget("message",
		xmLabelGadgetClass, 	e->base_form_mgr,
		XmNalignment, 		XmALIGNMENT_BEGINNING,
		XmNbottomAttachment, 	XmATTACH_FORM,
		XmNbottomOffset, 	2,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		2,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNrightOffset, 	2,
		NULL);

	/*
	**  Create insert, delete, change, and clear buttons
	*/
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 342, "Insert"));
        e->insert_button = XtVaCreateWidget("Insert",
		xmPushButtonWidgetClass, e->base_form_mgr,
		XmNlabelString,		xmstr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	1,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	2,
                NULL);
	XtAddCallback(e->insert_button, XmNactivateCallback, e_insert_proc, e);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 343, "Change"));
        e->change_button = XtVaCreateWidget("Change",
		xmPushButtonWidgetClass, e->base_form_mgr,
		XmNlabelString,		xmstr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	2,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	3,
		XmNsensitive, 		False,
                NULL);
	XtAddCallback(e->change_button, XmNactivateCallback, e_change_proc, e);
	XmStringFree(xmstr);
 
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 419, "Delete"));
        e->delete_button = XtVaCreateWidget("Delete",
		xmPushButtonWidgetClass, e->base_form_mgr,
		XmNlabelString,		xmstr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	3,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	4,
		XmNsensitive, 		False,
                NULL);
	XtAddCallback(e->delete_button, XmNactivateCallback, e_delete_proc, e);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 803, "Clear"));
        e->clear_button = XtVaCreateWidget("Clear",
		xmPushButtonWidgetClass, e->base_form_mgr,
		XmNlabelString,		xmstr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	4,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	5,
                NULL);
	XtAddCallback(e->clear_button, XmNactivateCallback, e_clear_proc, e);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 923, "Cancel"));
        e->close_button = XtVaCreateWidget("Cancel",
		xmPushButtonWidgetClass, e->base_form_mgr,
		XmNlabelString,		xmstr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	5,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	6,
                NULL);
	XtAddCallback(e->close_button, XmNactivateCallback, e_close_proc, e);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
        e->help_button = XtVaCreateWidget("Help",
		xmPushButtonWidgetClass, e->base_form_mgr,
		XmNlabelString,		xmstr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	6,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	7,
                NULL);
	XmStringFree(xmstr);

	e->separator2 = XtVaCreateWidget("separator2",
		xmSeparatorGadgetClass, e->base_form_mgr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->insert_button,
		XmNbottomOffset, 	5,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNrightOffset, 	5,
		NULL);

	e->separator1 = XtVaCreateWidget("separator1",
		xmSeparatorGadgetClass, e->base_form_mgr,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNrightOffset, 	5,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->separator2,
		NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 626, "More"));
        e->expand_ui_button = XtVaCreateWidget("expand_ui_button",
		xmPushButtonWidgetClass, e->base_form_mgr,
		XmNlabelString, 	xmstr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->separator1,
		XmNbottomOffset, 	3,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		XmNnavigationType, 	XmTAB_GROUP,
                NULL);
	XtAddCallback(e->expand_ui_button, XmNactivateCallback,
		e_expand_ui_proc, e);
	XmStringFree(xmstr);

	e->dsswFlags = 0;
	build_dssw(&e->dssw, c, e->base_form_mgr, True, True);
	XtVaSetValues(e->dssw.dssw_form_mgr,
		XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset, 		label_height + 5,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->expand_ui_button,
		XmNbottomOffset, 	3,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		NULL);

	XtAddCallback(e->dssw.start_text, XmNvalueChangedCallback,
		      clear_flag_on_modify, (XtPointer)&e->dsswFlags);
	XtAddCallback(e->dssw.start_am, XmNvalueChangedCallback,
		      clear_flag_on_modify, (XtPointer)&e->dsswFlags);
	XtAddCallback(e->dssw.start_pm, XmNvalueChangedCallback,
		      clear_flag_on_modify, (XtPointer)&e->dsswFlags);
	XtAddCallback(e->dssw.stop_text, XmNvalueChangedCallback,
		      clear_flag_on_modify, (XtPointer)&e->dsswFlags);
	XtAddCallback(e->dssw.stop_am, XmNvalueChangedCallback,
		      clear_flag_on_modify, (XtPointer)&e->dsswFlags);
	XtAddCallback(e->dssw.stop_pm, XmNvalueChangedCallback,
		      clear_flag_on_modify, (XtPointer)&e->dsswFlags);

	/*
	 * Add a drag source icon inside the dssw, lower right
	 */
	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 627, "Drag Appt"));
	e->drag_source = XtVaCreateWidget("drag_source",
		dtIconGadgetClass, 	e->dssw.dssw_form_mgr,
		XmNpixmapPosition, 	XmPIXMAP_TOP,
		XmNstringPosition, 	XmSTRING_BOTTOM,
		XmNalignment, 		XmALIGNMENT_CENTER,
		XmNstring, 		xmstr,
		XmNbottomAttachment, 	XmATTACH_FORM,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNtraversalOn, 	False,
		XmNbehavior, 		XmICON_DRAG,
		NULL);
	XmStringFree(xmstr);

        XtAddEventHandler(XtParent(e->drag_source), Button1MotionMask, False,
                (XtEventHandler)FormApptDragMotionHandler, (XtPointer) c);

	XtVaGetValues((Widget)XmGetXmDisplay(XtDisplay(c->frame)), 
		"enableBtn1Transfer", 	&btn1_transfer, 
		NULL); 

	/* btn1_transfer is a tri-state variable - see 1195846 */
	if ((Boolean)btn1_transfer != True) {
        	XtAddEventHandler(XtParent(e->drag_source), 
				Button2MotionMask, False, 
				(XtEventHandler)FormApptDragMotionHandler, 
				(XtPointer) c);
	}

	if (p->drag_icon_xbm)
		XtVaSetValues(e->drag_source, 
				XmNpixmap, p->drag_icon_xbm, 
				NULL);

	XtVaSetValues(e->dssw.what_scrollwindow, 
		XmNrightAttachment, 	XmATTACH_WIDGET,
		XmNrightWidget, 	e->drag_source, 
		NULL);

	ManageChildren(e->dssw.dssw_form_mgr);

	CalFontExtents(c->fonts->labelfont, &listfontextents);

	cnt = 0;
	XtSetArg(args[cnt], XmNlistSizePolicy, XmCONSTANT); 		++cnt;
	XtSetArg(args[cnt], XmNwidth, 15 * 
		listfontextents.max_logical_extent.width); 		++cnt;
	XtSetArg(args[cnt], XmNscrollBarDisplayPolicy, XmSTATIC); 	++cnt;
	XtSetArg(args[cnt], XmNdoubleClickInterval, 5); 		++cnt;
        e->appt_list = XmCreateScrolledList(e->base_form_mgr, "appt_list",
					    args, cnt);
	e->appt_list_sw = XtParent(e->appt_list);

	XtOverrideTranslations(e->appt_list, new_translations);

	/* Make btn 2 do dnd of appts */
	/* btn1_transfer is a tri-state variable - see 1195846 */
	if ((Boolean)btn1_transfer != True) {
		new_translations = XtParseTranslationTable(btn2_translations);
		XtOverrideTranslations(e->appt_list, new_translations);
	}

        XtVaSetValues(e->appt_list_sw,
		XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset, 		label_height + 11,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		e->dssw.dssw_form_mgr,
		XmNleftOffset, 		10,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNrightOffset, 	5,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	e->expand_ui_button,
		XmNbottomOffset, 	3,
                NULL);

	XtManageChild(e->appt_list);
	XtAddCallback(e->appt_list,
		XmNbrowseSelectionCallback, e_list_select_proc, e); 

	XtVaSetValues(e->list_label,
		XmNleftAttachment, 	XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 		e->appt_list_sw,
		NULL);

        XtAddCallback(e->help_button, XmNactivateCallback,
		(XtCallbackProc)help_cb, APPT_EDITOR_HELP_BUTTON);
        XtAddCallback(e->base_form_mgr, XmNhelpCallback,
                (XtCallbackProc)help_cb, (XtPointer) APPT_EDITOR_HELP_BUTTON);

       	XtVaSetValues(e->base_form_mgr, 
		XmNdefaultButton, 	e->insert_button, 
		NULL);

       	XtVaSetValues(e->base_form_mgr, 
		XmNcancelButton, 	e->close_button, 
		NULL);

       	XmProcessTraversal(e->dssw.what_text, XmTRAVERSE_CURRENT);
       	XtVaSetValues(e->base_form_mgr, 
		XmNinitialFocus, 	e->dssw.what_text, 
		NULL);

	ManageChildren(e->base_form_mgr);
	XtManageChild(e->base_form_mgr);

	/*
	**  Set up editor variables.  The drag threshold variables are set to
	**  -1, so we are ready for first dnd operation.
	*/
	rfp_init(&e->rfp, c, e->base_form_mgr);
	reminders_init(&e->reminders, c, e->base_form_mgr);
	e->appt_count = 0;
	e->appt_head = NULL;
	e->initialX = -1;
	e->initialY = -1;
	e->doing_drag = False;
}

/*******************************************************************************
**
**  External functions
**
*******************************************************************************/
extern void
add_to_appt_list(CSA_entry_handle entry, Editor *e) {
	char			buf[DEFAULT_APPT_LEN];
	Props			*p = (Props *)e->cal->properties;
	XmString		str;
	CSA_return_code		status;
	DisplayType		dt = get_int_prop(p, CP_DEFAULTDISP);
	Dtcm_appointment	*appt;

	appt = allocate_appt_struct(appt_read,
				    e->cal->general->version,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_SUMMARY_I,
				    CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
				    NULL);
	status = query_appt_struct(e->cal->cal_handle, entry, appt);
	backend_err_msg(e->frame, e->cal->view->current_calendar, status,
			((Props_pu *)e->cal->properties_pu)->xm_error_pixmap);
	if (status != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return;
	}

	format_appt(appt, buf, dt, DEFAULT_APPT_LEN);
	str = XmStringCreateLocalized(buf);
	XmListAddItem(e->appt_list, str, 0);
	XmStringFree(str);
	free_appt_struct(&appt);
}

extern void
add_all_appt(Editor *e) {
	int		i;
	CSA_uint32	j;
	char		*date;
	Tick		tick;
	Props		*p;
	Calendar	*c = e->cal;
	CSA_entry_handle	*entry_list;
	OrderingType	o;
	SeparatorType	s;

	if (!editor_showing(e))
		return;

	p = (Props *)e->cal->properties;
	o = get_int_prop(p, CP_DATEORDERING);
	s = get_int_prop(p, CP_DATESEPARATOR);

	date = get_date_from_widget(e->cal->view->date,	e->dssw.date_text, o, s);
	if (!date || (tick = cm_getdate(date, NULL)) <= 0)
		return;
	build_editor_list(e, tick, dayGlance, &entry_list, &j);

	XmListDeleteAllItems(e->appt_list);
	XtSetSensitive(e->delete_button, False);
	XtSetSensitive(e->change_button, False);
	if (e->appt_head && e->appt_count >= 0)
		csa_free(e->appt_head); 
	e->appt_head = entry_list;
	e->appt_count = j;
	for (i = 0; i < j; i++)
		add_to_appt_list(entry_list[i], e);
	if (j <= 0)
		XtSetSensitive(e->appt_list, False);
	else
		XtSetSensitive(e->appt_list, True);
}


static void
set_list_title(Editor *e) {
        Calendar 	*c = e->cal;
	Props		*p = (Props *)c->properties;
	char		*header;
	char		buffer[BUFSIZ];
	char		buffer2[BUFSIZ];
	XmString        xmstr;

	switch (e->view_list_glance) {
		case yearGlance:
				header = catgets(c->DT_catd, 1, 704, "Year of %d");
				sprintf(buffer, header, year(e->view_list_date));
				break;
		case monthGlance:
				header = catgets(c->DT_catd, 1, 705, "%s");
				format_date(e->view_list_date+1, get_int_prop(p, CP_DATEORDERING), buffer2, 0, 0, 0);
				sprintf(buffer, header, buffer2);
				break;
		case weekGlance:
				header = catgets(c->DT_catd, 1, 706, "Week of %s");
				format_date(e->view_list_date+1, get_int_prop(p, CP_DATEORDERING), buffer2, 1, 0, 0);
				sprintf(buffer, header, buffer2);
				break;
		case dayGlance:
				header = catgets(c->DT_catd, 1, 707, "%s");
				format_date(e->view_list_date+1, get_int_prop(p, CP_DATEORDERING), buffer2, 1, 0, 0);
				sprintf(buffer, header, buffer2);
				break;
	}
	if (e->view_frame) {
		xmstr = XmStringCreateLocalized(buffer);
		XtVaSetValues(e->view_list_label, XmNlabelString, xmstr,
			NULL);
		XmStringFree(xmstr);
	}
}
extern void
build_editor_list(Editor *e, Tick date, Glance glance,
		  CSA_entry_handle **entry_list, CSA_uint32 *count) {
	int			range_count;
	time_t			start, stop;
	CSA_return_code		status;
	CSA_enum		*ops;
	CSA_attribute		*range_attr;

	switch(glance) {
	case yearGlance:
		start = lowerbound(jan1(date));
		stop = nextyear(start) - 1;
		break;
	case monthGlance:
		start = first_dom(date);
		stop = nextmonth(start) - 1;
		break;
	case weekGlance:
		start = first_dow(date);
		stop = nextweek(start) - 1;
		break;
	case dayGlance:
	default:
		start = lowerbound(date);
		stop = nextday(start) - 1;
		break;
	}

	setup_range(&range_attr, &ops, &range_count, start, stop,
		    CSA_TYPE_EVENT, 0, B_FALSE, e->cal->general->version);

	status = csa_list_entries(e->cal->cal_handle, range_count, range_attr, ops, count, entry_list, NULL);
	backend_err_msg(e->frame, e->cal->view->current_calendar, status,
			((Props_pu *)e->cal->properties_pu)->xm_error_pixmap);
	if (status != CSA_SUCCESS) {
		*entry_list = NULL;
		*count = 0;
	}
	free_range(&range_attr, &ops, range_count);
}

extern int
build_editor_view(Editor *e, Glance glance, Boolean redisplay) {
	int			cnt;
	CSA_uint32		entry_count;
	char			*buf, *what_str, str1[MAXNAMELEN],
				str2[MAXNAMELEN];
	CSA_entry_handle	*entry_list;
	Lines			*lines;
	Props			*p = (Props *)e->cal->properties;
	XmString		str;
	CSA_return_code		status;
	OrderingType		o = get_int_prop(p, CP_DATEORDERING);
	SeparatorType		s = get_int_prop(p, CP_DATESEPARATOR);
	DisplayType		dt = get_int_prop(p, CP_DEFAULTDISP);
	Dtcm_appointment	*appt;
	Tick			start_time;

	/*
	**  First, get the list of appointments and delete all items from the
	**  list
	*/

        if (redisplay == True) {
 
                /* On a redisplay, rebuild the list based on the
                   parameters of the last query. */
 
                build_editor_list(e, e->view_list_date, e->view_list_glance, &entry_list,
                                &entry_count);
        }
        else {
                /* If this is a clean display of the appointment list,
                   save the context so that a redisplay can be done if
                   something changes, like the display format or something
                   like that. */
 
                build_editor_list(e, e->cal->view->date, glance, &entry_list,
                                &entry_count);
 
                e->view_list_glance = glance;
                e->view_list_date = e->cal->view->date;
        }

	set_list_title(e);

	XmListDeleteAllItems(e->view_list);

	/*
	**  Now loop through and add each appt to the list
	*/
	appt = allocate_appt_struct(appt_read,
				    e->cal->general->version,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_SUMMARY_I,
				    NULL);
	for (cnt = 0; cnt < entry_count; cnt++) {
		/*
		**  Create the text string describing the editor and set that
		**  value in the scrolling list
		*/
		status = query_appt_struct(e->cal->cal_handle, entry_list[cnt], appt);
		backend_err_msg(e->frame, e->cal->view->current_calendar, status,
			((Props_pu *)e->cal->properties_pu)->xm_error_pixmap);
		if (status != CSA_SUCCESS) {
			csa_free(entry_list);
			free_appt_struct(&appt);
			entry_count = cnt = 0;
			continue;
		}

		_csa_iso8601_to_tick(appt->time->value->item.date_time_value, &start_time);
		format_tick(start_time, o, s, str1);
		format_time(start_time, dt, str2);
		lines = text_to_lines(appt->what->value->item.string_value, 1);
		if (lines && lines->s) {
			buf = (char *)ckalloc(cm_strlen(str1) + cm_strlen(str2)
					      + cm_strlen(lines->s) + 5);
			what_str = lines->s;
		} else {
			buf = (char *)ckalloc(cm_strlen(str1)
					      + cm_strlen(str2) + 6);
			what_str = "\0";
		}

		sprintf(buf, "%s  %s  %s", str1, str2, what_str);
		str = XmStringCreateLocalized(buf);
		XmListAddItem(e->view_list, str, 0);

		XmStringFree(str);
		free(buf);
		destroy_lines(lines);
	}
	if (entry_list && entry_count > 0)
		csa_free(entry_list);
	free_appt_struct(&appt);

	if (entry_count <= 0)
		XtSetSensitive(e->view_list, False);
	else
		XtSetSensitive(e->view_list, True);

	return (cnt);
}

boolean_t
compare_repeat_info(
        Dtcm_appointment	*old_a,
        RFP			*rfp,
        CSA_session_handle	cal_handle,
	int			cal_version)
{
        Dtcm_appointment        *appt;
        CSA_return_code          status;
        CSA_enum                 ops[1];
        CSA_entry_handle        *entries;
        CSA_uint32               num_entries;

        /* Find the start date of the appointment.  If it matches the
         * current appt then we don't care if the rules match because we
         * allow changes to the rules when you start from the first
         * appt.
         */
        ops[0] = CSA_MATCH_EQUAL_TO;

        status = csa_list_entries(cal_handle, 1, old_a->identifier, ops,
			&num_entries, &entries, NULL);

        if (status != CSA_SUCCESS || num_entries == 0) {
                return FALSE;
        }

        appt = allocate_appt_struct(appt_read,
                                    cal_version,
                                    CSA_ENTRY_ATTR_START_DATE_I,
                                    NULL);
        status = query_appt_struct(cal_handle, entries[0], appt);
        if (status != CSA_SUCCESS) {
                free_appt_struct(&appt);
                return FALSE;
        }

        csa_free(entries);

        if (!strcmp(old_a->time->value->item.date_time_value,
                    appt->time->value->item.date_time_value)) {
                free_appt_struct(&appt);
                return TRUE;
        }
 
        free_appt_struct(&appt);
 
        /* We're not at the first event so we check to see if the rule
         * has changed.
         */
        if (!old_a->repeat_type->value)
                return FALSE;
 
        if (old_a->repeat_type->value->item.sint32_value !=
            rfp->repeat_type)
                return FALSE;

        if (!old_a->repeat_interval || !old_a->repeat_interval->value)
                return TRUE;

        if (rfp->repeat_type == CSA_X_DT_REPEAT_EVERY_NDAY ||
            rfp->repeat_type == CSA_X_DT_REPEAT_EVERY_NWEEK ||
            rfp->repeat_type == CSA_X_DT_REPEAT_EVERY_NMONTH)
                if (old_a->repeat_interval->value->item.sint32_value !=
                    rfp->repeat_nth)
                        return FALSE;

        return TRUE;                  
}

void
trim_end_date_from_rule(char *rule, char *newrule)
{
	char	*ptr;

	/* this is done with the assumption that 'Z' only appears
	 * in the end date of the rule and that there is at most
	 * one end date in the rule and that the end date is
	 * is always at the end of the rule
	 */
	if (ptr = strchr(rule, 'Z')) {
		while (*ptr != ' ')
			ptr--;
		*ptr = '\0';
	}
	strcpy(newrule, rule);

	if (ptr)
		*ptr = ' ';
}

/*
 * A single event in a repeating appt is beinging changed.  If the rule is
 * not being changed then we need to change it to D1 #1 since the change
 * is suppose to effect this appt only.
 */
void
change_rule_for_this_one_only(
	Calendar	 *c,
	Dtcm_appointment *new, 
	Dtcm_appointment *old)
{
	char	buf[BUFSIZ];

	if (c->general->version < DATAVER4) {
		if ((new->repeat_type->value->item.sint32_value ==
		     old->repeat_type->value->item.sint32_value) &&
		    (new->repeat_times->value->item.uint32_value ==
		     old->repeat_times->value->item.uint32_value)) {
			new->repeat_type->value->item.sint32_value =
						CSA_X_DT_REPEAT_ONETIME;
			new->repeat_times->value->item.uint32_value = 0;
		}
	} else {
		/* an end date might be added to the rule if the user
		 * has done a delete/change forward operation on this
		 * appointment before.  We need to get rid of the
		 * end date from the rule otherwise the 2 rules
		 * won't match
		 */
		trim_end_date_from_rule(old->recurrence_rule->value->\
			item.string_value, buf); 
		if (!strcmp(new->recurrence_rule->value->item.string_value,
			    buf)) {
			free (new->recurrence_rule->value->item.string_value);
			new->recurrence_rule->value->item.string_value =
						cm_strdup("D1 #1");
		}
	}
}

extern Boolean
editor_change(Dtcm_appointment *new_a, CSA_entry_handle old_a, CSA_entry_handle *updated_a,
	      Calendar *c) {
	Editor			*e = (Editor *)c->editor;
	Props_pu		*p = (Props_pu *)c->properties_pu;
	CSA_return_code		stat;
	CSA_enum		scope;
	Dtcm_appointment	*appt;
	static int		answer;

	answer = 0;
	appt = allocate_appt_struct(appt_read,
				    c->general->version,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I,
				    CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_RECURRENCE_RULE_I,
				    NULL);
	stat = query_appt_struct(c->cal_handle, old_a, appt);
	backend_err_msg(e->frame, c->view->current_calendar, stat,
			p->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return FALSE;
	}

	if ((appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER) ||
	    (appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER_WEEKLY) ||
	    (appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER_MONTHLY) ||
	    (appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER_YEARLY)) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 258,
                                      "Calendar : Appointment Editor - Change"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 708,
                                        "This appointment repeats in an unknown fashion.  All occurrences will be changed\nDo you still wish to change it?"));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
	  	char *ident4 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
                answer = dialog_popup(e->frame,
                                DIALOG_TITLE, title,
                                DIALOG_TEXT, text,
				BUTTON_IDENT, 1, ident1,
                                BUTTON_IDENT, 4, ident4,
                                DIALOG_IMAGE, p->xm_question_pixmap,
                                NULL);
		XtFree(ident4);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
	}
	else if (appt->repeat_type->value->item.sint32_value != 
		CSA_X_DT_REPEAT_ONETIME) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 258,
				"Calendar : Appointment Editor - Change"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 268,
				"This appointment is part of a repeating series.\nDo you want to change ...?"));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
		char *ident2 = XtNewString(catgets(c->DT_catd, 1, 270,
				"This One Only"));
		char *ident3 = XtNewString(catgets(c->DT_catd, 1, 271, "Forward"));
		char *ident4 = XtNewString(catgets(c->DT_catd, 1, 272, "All"));
		answer = dialog_popup(e->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			BUTTON_IDENT, 2, ident2,
			BUTTON_IDENT, 3, ident3,
			(compare_repeat_info(appt, &(e->rfp), c->cal_handle, c->general->version) ?
				BUTTON_IDENT : BUTTON_INSENSITIVE), 
					4, ident4,
			DIALOG_IMAGE, p->xm_question_pixmap,
			NULL);
		XtFree(ident4);
		XtFree(ident3);
		XtFree(ident2);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		if (answer == 2)
			change_rule_for_this_one_only(c, new_a, appt);
	}

	switch(answer) {
	case 1:
		free_appt_struct(&appt);
		return False;
	case 2:
		scope = CSA_SCOPE_ONE;
		break;
	case 3:
		scope = CSA_SCOPE_FORWARD;
		break;
	case 4:
	default:
		scope = CSA_SCOPE_ALL;
		break;
	}

	/* We are not allowed to change the type of the entry, so we will 
	   remove that particular entry from the list for writing. */

	if (new_a->type) {
		if (new_a->type->name){
			free(new_a->type->name);
			new_a->type->name = NULL;
		}
	}

	/* if the repeat type/times is changed, reset the sequence end date */
	if (c->general->version == DATAVER3 &&
	    appt->repeat_type->value->item.sint32_value !=
	    CSA_X_DT_REPEAT_ONETIME &&
	    (appt->repeat_type->value->item.sint32_value !=
	     new_a->repeat_type->value->item.sint32_value ||
	     appt->repeat_times->value->item.uint32_value !=
	     new_a->repeat_times->value->item.uint32_value))
	{
		if (new_a->sequence_end_date && new_a->sequence_end_date->value)
		{
			if (new_a->sequence_end_date->value->item.date_time_value)
				free(new_a->sequence_end_date->value->\
					item.date_time_value);
			free(new_a->sequence_end_date->value);
			new_a->sequence_end_date->value = NULL;
		}
	} else {
		if (new_a->sequence_end_date && new_a->sequence_end_date->name)
		{
			free (new_a->sequence_end_date->name);
			new_a->sequence_end_date->name = NULL;
		}
	}

	free_appt_struct(&appt);

	stat = csa_update_entry_attributes(c->cal_handle, old_a, scope, CSA_FALSE, new_a->count, new_a->attrs, updated_a, NULL);
	backend_err_msg(e->frame, c->view->current_calendar, stat,
			p->xm_error_pixmap);
	if (stat != CSA_SUCCESS)
		return FALSE;

	set_editor_msg_defaults(e);
	add_all_appt(e);
	if (editor_view_showing(e))
		build_editor_view(e, dayGlance, False);

	if (geditor_showing((GEditor *)calendar->geditor))
                add_all_gappt((GEditor *)calendar->geditor);

	reset_alarm(c);
	invalidate_cache(c);
	paint_canvas(c, NULL, RENDER_CLEAR_FIRST);

	if (c->browser)
		br_display(c);

	if (geditor_showing((GEditor *)c->geditor))
		add_all_gappt((GEditor *)c->geditor);

	return True;
}

extern void
editor_clean_up(Editor *e) {
	if (e->appt_head && e->appt_count >= 0)
		csa_free(e->appt_head); 
	e->appt_head = NULL;
	e->appt_count = 0;
}

extern Boolean
editor_delete(CSA_entry_handle entry, Calendar *c) {
	Editor			*e = (Editor *)c->editor;
	Props_pu		*p = (Props_pu *)c->properties_pu;
	CSA_return_code		stat;
	CSA_enum		scope;
	Dtcm_appointment	*appt;
	static int	answer;

	answer = 0;
	appt = allocate_appt_struct(appt_read, 
				c->general->version,
				CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I, 
				NULL);
	stat = query_appt_struct(c->cal_handle, entry, appt);
	backend_err_msg(e->frame, c->view->current_calendar, stat,
			p->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return False;
	}

	if (appt->repeat_type->value) {
		if (appt->repeat_type->value->item.sint32_value != CSA_X_DT_REPEAT_ONETIME) {
			char *title = XtNewString(catgets(c->DT_catd, 1, 252,
				      "Calendar : Appointment Editor - Delete"));
			char *text = XtNewString(catgets(c->DT_catd, 1, 274,
					"This appointment is part of a repeating series.\nDo you want to delete ...?"));
			char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
			char *ident2 = XtNewString(catgets(c->DT_catd, 1, 270,
					"This One Only"));
			char *ident3 = XtNewString(catgets(c->DT_catd, 1, 271, "Forward"));
			char *ident4 = XtNewString(catgets(c->DT_catd, 1, 272, "All"));
			answer = dialog_popup(e->frame,
				DIALOG_TITLE, title,
				DIALOG_TEXT, text,
				BUTTON_IDENT, 1, ident1,
				BUTTON_IDENT, 2, ident2,
				BUTTON_IDENT, 3, ident3,
				BUTTON_IDENT, 4, ident4,
				DIALOG_IMAGE, p->xm_question_pixmap,
				NULL);
			XtFree(ident4);
			XtFree(ident3);
			XtFree(ident2);
			XtFree(ident1);
			XtFree(text);
			XtFree(title);
		}
	}
	else if (appt->recurrence_rule->value) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 252,
                                      "Calendar : Appointment Editor - Delete"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 711,
                                        "This appointment repeats in an unknown fashion.  All occurrences will be deleted\nDo you still wish to delete it?"));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
		char *ident4 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
                answer = dialog_popup(e->frame,
                                DIALOG_TITLE, title,
                                DIALOG_TEXT, text,
				BUTTON_IDENT, 1, ident1,
                                BUTTON_IDENT, 4, ident4,
                                DIALOG_IMAGE, p->xm_question_pixmap,
                                NULL);
		XtFree(ident4);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
	}
	free_appt_struct(&appt);

	switch(answer) {
	case 1:
		return False;
	case 2:
		scope = CSA_SCOPE_ONE;
		break;
	case 3:
		scope = CSA_SCOPE_FORWARD;
		break;
	case 4:
	default:
		scope = CSA_SCOPE_ALL;
		break;
	}

	stat = csa_delete_entry(c->cal_handle, entry, scope, NULL);
	backend_err_msg(e->frame, c->view->current_calendar, stat,
			p->xm_error_pixmap);
	if (stat != CSA_SUCCESS)
		return FALSE;

	set_editor_msg_defaults(e);
	add_all_appt(e);
	if (editor_view_showing(e))
		build_editor_view(e, dayGlance, False);
	reset_alarm(c);
	invalidate_cache(c);
	paint_canvas(c, NULL, RENDER_CLEAR_FIRST);

	if (c->browser)
		br_display(c);

	return True;
}

extern Boolean
editor_insert(Dtcm_appointment *appt, CSA_entry_handle *new_a, Calendar *c) {
	CSA_return_code	stat;
	Editor		*e = (Editor *)c->editor;
	Props_pu	*p = (Props_pu *)c->properties_pu;
	CSA_enum		scope;
	static int		answer=0;

	/* the gui does not support specifying the sequence end date */
	if (appt->sequence_end_date && appt->sequence_end_date->name) {
		free(appt->sequence_end_date->name);
		appt->sequence_end_date->name = NULL;
	}

	if ((appt->repeat_type) && (appt->repeat_type->value) &&
           (appt->repeat_type->value->item.sint32_value != CSA_X_DT_REPEAT_ONETIME))
	{
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 1101,
					"Insert Appointment"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 984,
					"This appointment is part of a repeating series.\nDo you want to insert appointment ...?"));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
	  	char *ident4 = XtNewString(catgets(c->DT_catd, 1, 272, "All"));
			answer = dialog_popup(e->frame,
				DIALOG_TITLE, title,
				DIALOG_TEXT, text,
				BUTTON_IDENT, 1, ident1,
				BUTTON_IDENT, 4, ident4,
				DIALOG_IMAGE, p->xm_question_pixmap,
				NULL);
		XtFree(ident4);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
	}

	switch(answer) {
	case 1:
		/*
		 * Free the CSA_buffer here since before, this routine
		 * only returned false when failure to obtain the CSA_structure
		 * occurred.
		 */
		csa_free((CSA_buffer)new_a);
		return 2;
	case 4:
	default:
		/*
		 * scope is not used at this time.  However, to follow
		 * the change/delete style, this is here so in case
		 * later the same type of dialog is required.
		 */
		scope = CSA_SCOPE_ALL;
		break;
	}

	stat = csa_add_entry(c->cal_handle, appt->count, appt->attrs, new_a, NULL);
	backend_err_msg(c->frame, c->view->current_calendar, stat,
			p->xm_error_pixmap);
	if (stat != CSA_SUCCESS)
		return FALSE;

	set_editor_msg_defaults(e);
	add_all_appt(e);
	if (editor_view_showing(e))
		build_editor_view(e, dayGlance, False);

	if (geditor_showing((GEditor *)calendar->geditor))
                add_all_gappt((GEditor *)calendar->geditor);

	reset_alarm(c);
	invalidate_cache(c);
	paint_canvas(c, NULL, RENDER_CLEAR_FIRST);

	if (c->browser)
		br_display(c);
	return True;
}

extern Boolean
editor_created(Editor *e) {
	if (!e || !e->base_form_mgr)
		return False;
	return True;
}

/*
**  Return the nth appointment from the array
*/
extern CSA_entry_handle
editor_nth_appt(Editor *e, int idx) {
	if (idx >= 0 && idx < e->appt_count)
		return e->appt_head[idx];
	return 0;
}

extern Boolean
editor_showing(Editor *e) {
	if (e)
		return e->editor_is_up;
	return False;
}

extern Boolean
editor_view_showing(Editor *e) {
	if (e)
		return e->editor_view_is_up;
	return False;
}

/*
**  External function to set editor defaults
*/
extern void
set_editor_defaults(Editor *e, Tick start, Tick stop, Boolean show_notime) {
	if (start > 0 || stop > 0) {
		load_dssw_times(&e->dssw, start, stop, show_notime);
		set_dssw_defaults(&e->dssw, e->cal->view->date, False);
	} else
		set_dssw_defaults(&e->dssw, e->cal->view->date, True);
	if (e->reminders.bfpm_form_mgr) {
		set_rfp_defaults(&e->rfp);
		set_reminders_defaults(&e->reminders);
	}
	set_message(e->message_text, " ");

	e->dsswFlags = 0;
	e->rfpFlags = 0;
}

extern void
set_editor_title(Editor *e, char *name) {
	char		buf[MAXNAMELEN];
	Calendar	*c = e->cal;

	if (e->frame) {
		sprintf(buf, "%s - %s", catgets(c->DT_catd, 1, 279,
			"Calendar : Appointment Editor"), name);
		XtVaSetValues(e->frame, XmNtitle, buf,
			NULL);
	}
}

extern void
set_editor_vals(Editor *e, Tick start, Tick stop) {
	if (start > 0 || stop > 0)
		load_dssw_times(&e->dssw, start, stop, False);
	set_dssw_vals(&e->dssw, e->cal->view->date);
	set_rfp_vals(&e->rfp);
	set_reminders_vals(&e->reminders, True);
}

extern void
show_editor(Calendar *c, time_t start, time_t stop, Boolean show_notime) {
	Editor	*e = (Editor *)c->editor;

	if (!editor_created(e))
		e_make_editor(c);
	else if (!XtIsManaged(e->base_form_mgr))
		XtManageChild(e->base_form_mgr);
	else
		XRaiseWindow(XtDisplay(e->base_form_mgr),
			     XtWindow(XtParent(e->base_form_mgr)));
	if (!editor_showing(e)) {
		ds_position_popup(c->frame, e->frame, DS_POPUP_LOR);
        	XmProcessTraversal(e->dssw.what_text, XmTRAVERSE_CURRENT);
        	XtVaSetValues(e->base_form_mgr, 
				XmNinitialFocus, e->dssw.what_text, 
				NULL);
	}

	if (! e->editor_is_up)
	    {
	    e->editor_is_up = True;
	    set_editor_defaults(e, start, stop, show_notime);
	}
	add_all_appt(e);
	if (e->frame) XtPopup(e->frame, XtGrabNone);
	/* if (e->view_frame) XtPopup(e->view_frame, XtGrabNone); */
}

extern void
show_editor_view(Calendar *c, Glance glance) {
	Editor		*e = (Editor *)c->editor;
	Props_pu	*p = (Props_pu *)c->properties_pu;

	if (!e->view_frame) {
		e->cal = c;
		e_build_view_popup(e);
	}

        XtVaSetValues(e->view_form, XmNdefaultButton, e->view_cancel_button, NULL);
        XtVaSetValues(e->view_form, XmNcancelButton, e->view_cancel_button, NULL);


	if (build_editor_view(e, glance, False) <= 0) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 280,
				"Calendar : Error - Appointment List"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 281,
				"Sorry, no appointments to list.     "));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		dialog_popup(e->cal->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		XtUnmanageChild(e->view_form);
		e->editor_view_is_up = False;
	} else {
		if (!editor_view_showing(e))
			ds_position_popup(c->frame, e->view_frame,
					  DS_POPUP_LOR);
		XtManageChild(e->view_form);
		e->editor_view_is_up = True;
	}
	/* if (e->frame) XtPopup(e->frame, XtGrabNone); */
	if (e->view_frame) XtPopup(e->view_frame, XtGrabNone);
}
/*
**  Function to set some editor defaults
*/
static void
set_editor_msg_defaults(Editor *e) {
	set_dssw_defaults(&e->dssw, e->cal->view->date, False);
	set_message(e->message_text, " ");
}
