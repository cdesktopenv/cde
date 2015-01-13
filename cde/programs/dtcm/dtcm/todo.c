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
/* $TOG: todo.c /main/12 1999/07/01 09:24:05 mgreess $ */
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
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/LabelG.h>
#include <Xm/ScrolledW.h>
#include <Dt/HourGlass.h>
#include <Dt/Dt.h>
#include <Dt/Dnd.h>
#include <Dt/Icon.h>
#include "todo.h"
#include "datefield.h"
#include "deskset.h"
#include "getdate.h"
#include "format.h"
#include "timeops.h"
#include "props_pu.h"
#include "util.h"
#include "help.h"
#include "dnd.h"
#ifdef SVR4
#include <sys/param.h>
#endif /* SVR4 */

static void t_build_expand(ToDo *);

/* Absolute value macro */
#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#endif
static void set_todo_msg_defaults();
static void clear_flag_on_modify(Widget, XtPointer, XtPointer);

static CSA_return_code
t_execute_change(ToDo *, CSA_entry_handle , Dtcm_appointment *, Widget );
extern boolean_t compare_repeat_info(Dtcm_appointment *, RFP *, CSA_session_handle, int);
extern void change_rule_for_this_one_only(Calendar *, Dtcm_appointment *, Dtcm_appointment *);
extern Dimension ComputeMaxWidth(Widget, Widget, Widget, Widget);

/*******************************************************************************
**
**  Functions static to todo.c
**
*******************************************************************************/
/*
**  Return the nth appointment
*/
extern CSA_entry_handle
t_nth_appt(ToDo *t, int idx) {
	if (idx >= 0 && idx < t->todo_count)
		return t->todo_head[idx];
	return 0;
}

static void
clear_flag_on_modify(Widget w, XtPointer uData, XtPointer cbData)
{
  int *flagP = (int *)uData;

  *flagP = 0;
}

/*
**  This function will take todo values and stuff them into a form.
*/
static void
appt_to_form(ToDo *t, CSA_entry_handle a) {
	char			buf[MAXNAMELEN];
	CSA_return_code		stat;
	Dtcm_appointment	*appt;

	if (!t->reminders.bfpm_form_mgr) {
		t_build_expand(t);
		set_rfp_defaults(&t->rfp);
		set_reminders_defaults(&t->reminders);
	}

	if (!dssw_appt_to_form(&t->dssw, a))
		return;
	if (!rfp_appt_to_form(&t->rfp, a))
		return;
	t->rfpFlags = 0;
	if (!reminders_appt_to_form(&t->reminders, a))
		return;

	appt = allocate_appt_struct(appt_read,
				    t->cal->general->version,
				    CSA_ENTRY_ATTR_STATUS_I,
				    CSA_ENTRY_ATTR_ORGANIZER_I,
				    NULL);
	stat = query_appt_struct(t->cal->cal_handle, a, appt);
	backend_err_msg(t->frame, t->cal->view->current_calendar, stat,
			((Props_pu *)t->cal->properties_pu)->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return;
	}

	if (appt->state->value->item.sint32_value == CSA_STATUS_COMPLETED)
		t->completed_val = True;
	else
		t->completed_val = False;
	XmToggleButtonGadgetSetState(t->completed_toggle,
		t->completed_val, True);
	sprintf(buf, "%s:  %s",	catgets(t->cal->DT_catd, 1, 565, "Author"),
		appt->author->value->item.calendar_user_value->user_name);
	set_message(t->message_text, buf);
	free_appt_struct(&appt);
}

/*
**  List selection procedure will get the correct todo, then call
**  appt_to_form to load it into the UI.
*/
static void
t_list_select_proc(Widget w, XtPointer client_data, XtPointer data) {
	CSA_entry_handle		a;
	ToDo			*t = (ToDo *)client_data;
	XmListCallbackStruct	*cbs = (XmListCallbackStruct *)data;

	if (a = t_nth_appt(t, cbs->item_position - 1))
		appt_to_form(t, a);
	XtSetSensitive(t->delete_button, True);
        XtSetSensitive(t->change_button, True);
}

/*
**  This function creates the expando stuff on a form manager.
*/
static void
t_build_expand(ToDo *t) {
	Props		*p = (Props *)t->cal->properties;
	Widget		widgets[20];
	WidgetList	children;
	int		i = 0,
			j = 0,
			n;
	Dimension highest;
	Widget curr;

	/*
	**  Build the rfp "widget" then unmanage the privacy stuff ...
	**  Since none of the other widgets in the rfp composite widget don't
	**  depend on the privacy stuff for geometry mgmt, this is all we need
	**  to do.
	*/
	t->rfpFlags = 0;
	build_rfp(&t->rfp, t->cal, t->base_form_mgr);
	XtVaSetValues(t->rfp.rfp_form_mgr,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		t->separator1,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		NULL);
	XtVaGetValues(t->rfp.rfp_form_mgr,
		XmNchildren,            &children,
		XmNnumChildren,         &n,
		NULL);

	/* We don't want to manage the privacy widgets */
	for (i = 0; i < n; i++) {
		if ((children[i] == t->rfp.privacy_label) ||
		    (children[i] == t->rfp.privacy_menu))
		    continue;
		widgets[j++] = children[i];
	}
	XtManageChildren(widgets, n - 2);

	XtAddCallback(t->rfp.repeat_menu, XmNselectionCallback,
		      clear_flag_on_modify, (XtPointer)&t->rfpFlags);

	/*
	**  Build the reminders widget, unmanage everything except the mail
	**  stuff and make sure to reset the attachments so the form behaves
	**  correctly.  Use the reminders widget (though it seems wasteful) to
	**  share code and make future HIE decision changes easy to implement.
	*/
	build_reminders(&t->reminders, t->cal, t->base_form_mgr);
	widgets[0] = t->reminders.alarm_label;
	widgets[1] = t->reminders.mail_toggle;
	widgets[2] = t->reminders.mail_text;
	widgets[3] = t->reminders.mail_menu;
	widgets[4] = t->reminders.mailto_label;
	widgets[5] = t->reminders.mailto_text;

	_i18n_HighestWidget( 3, &curr, &highest, t->reminders.mail_toggle,
			t->reminders.mail_text, t->reminders.mail_menu );
	XtVaSetValues( curr,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, t->reminders.alarm_label,
			NULL );
	XtVaSetValues(t->reminders.bfpm_form_mgr,
		XmNtopAttachment, 	XmATTACH_OPPOSITE_WIDGET,
		XmNtopWidget, 		t->rfp.rfp_form_mgr,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNrightOffset, 	5,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		t->rfp.rfp_form_mgr,
		NULL);

	XtManageChildren(widgets, 6);
}

static void
clear_view_changes(ToDo *t){
	TodoView		*step, *last = NULL;

	step = t->view_list;

	if (t->view_list_modified == False)
		return;

	_DtTurnOnHourGlass(t->view_frame);

	while (step) {
		if (step->modified) {
			step->modified = False;
		}
		step = step->next;
	}
	_DtTurnOffHourGlass(t->view_frame);

	t->view_list_modified = False;
}

static void
write_view_changes(ToDo *t){
	Boolean			toggle_state;
	TodoView		*step = t->view_list;
	CSA_return_code		stat;
	Dtcm_appointment	*appt;
	CSA_entry_handle	updated_a;
	CSA_enum		scope;
	Calendar		*c = t->cal;
	Props_pu		*p = (Props_pu *)t->cal->properties_pu;

	if (t->view_list_modified == False)
		return;

	if (t->cal->general->version <= DATAVER3) {
		appt = allocate_appt_struct(appt_write, 
	    				    t->cal->general->version,
					    CSA_ENTRY_ATTR_STATUS_I, 
					    CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
					    NULL);
		appt->repeat_type->value->item.sint32_value = 
						CSA_X_DT_REPEAT_ONETIME;
	}
	else {
		appt = allocate_appt_struct(appt_write, 
	    				    t->cal->general->version,
					    CSA_ENTRY_ATTR_STATUS_I, 
					    CSA_ENTRY_ATTR_RECURRENCE_RULE_I, 
					    NULL); 
		appt->recurrence_rule->value->item.string_value = 
						strdup("D1 #1");
	}
		
	_DtTurnOnHourGlass(t->view_frame); 

	scope = CSA_SCOPE_ONE;

	while (step) { 
		if (step->modified) {

			toggle_state = XmToggleButtonGadgetGetState(
					step->view_item_toggle);
			appt->state->value->item.sint32_value = 
				(toggle_state) ?
				  CSA_STATUS_COMPLETED : CSA_X_DT_STATUS_ACTIVE;

			stat = csa_update_entry_attributes(c->cal_handle, 
					step->appt, scope, 
					CSA_FALSE, appt->count, 
					appt->attrs, &updated_a, NULL);
			backend_err_msg(t->view_frame, 
					c->view->current_calendar, stat,
					p->xm_error_pixmap);
			if (stat != CSA_SUCCESS)
				break;

			csa_free((CSA_buffer) step->appt);
			step->appt = updated_a;

			if (stat != CSA_SUCCESS)
				XtVaSetValues(step->view_item_toggle,
					XmNset, !toggle_state,
					NULL);
			step->modified = False;
		}
		step = step->next;
	}

	free_appt_struct(&appt);
		
	if (todo_showing(t)) {
		set_todo_msg_defaults(t);
		add_all_todo(t);
	}
	reset_alarm(c);
	invalidate_cache(t->cal);

	_DtTurnOffHourGlass(t->view_frame);

	t->view_list_modified = False;
}
static Boolean
flush_view_changes(ToDo *t){
	Calendar 	*c = t->cal;
	Props_pu	*pu = (Props_pu *) c->properties_pu;
	int		answer;

	if (t->view_list_modified) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 778, "Calendar : To Do List - Help"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 451, "You have made unsaved changes.\nYou may save your changes, discard your changes, \nor return to your previous place in the dialog."));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 452, "Save"));
	  	char *ident2 = XtNewString(catgets(c->DT_catd, 1, 700, "Discard"));
	  	char *ident3 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
		answer = dialog_popup(t->view_frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			BUTTON_IDENT, 2, ident2,
			BUTTON_IDENT, 3, ident3,
			DIALOG_IMAGE, pu->xm_warning_pixmap,
			NULL);
		XtFree(ident3);
		XtFree(ident2);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		if (answer == 1) {
			write_view_changes(t);
			return(True);
		}
		else if (answer == 2) {
			clear_view_changes(t);
			return(True);
		}
		else
			return(False);
	}

	return(True);
}

/*
**  Quit handler for the view popup
*/
static void
t_view_quit_handler(Widget w, XtPointer cdata, XtPointer data) {
	ToDo *t = (ToDo *)cdata;

	if (flush_view_changes(t) == False)
		return;

	XtPopdown(t->view_frame);
	t->todo_view_is_up = False;
}

static void
t_view_cancel_cb(
        Widget  widget,
        XtPointer client_data,
        XmPushButtonCallbackStruct *cbs)
{
	ToDo		*t = (ToDo *)client_data;

	if (flush_view_changes(t) == False)
		return;

        XtPopdown(t->view_frame);
}

static void
t_view_apply_cb(
        Widget  widget,
        XtPointer client_data,
        XmPushButtonCallbackStruct *cbs)
{
	ToDo		*t = (ToDo *)client_data;

	write_view_changes(t);

	build_todo_view(t, t->view_list_glance, False); 
}

static void
t_view_ok_cb(
        Widget  widget,
        XtPointer client_data,
        XmPushButtonCallbackStruct *cbs)
{
	ToDo		*t = (ToDo *)client_data;

	write_view_changes(t);

        XtPopdown(t->view_frame);
}

/*
**  Callbacks for the todo filter popup
*/
static void
t_view_filter_proc(Widget w, XtPointer client_data, XtPointer cbs) {
	ToDo		*t = (ToDo *)client_data; 
	int		op;

	if (!t)
		return; 

	XtVaGetValues(w, XmNuserData, &op, NULL);

	if (op == t->view_filter)
		return;

	t->view_filter = op;

	build_todo_view(t, t->view_list_glance, True); 
}

extern Widget
create_filter_menu(Widget parent, XtCallbackProc cb_func, XtPointer data) {
	int			i;
	Widget			menuitem;
	Widget			menu;
	Widget			cascade;
	XmString		label;
	XmString		option1;
	XmString		option2;
	XmString		option3;
	extern			Calendar *calendar;
	int			ac = 0;
	Arg			args[5];

	label = XmStringCreateLocalized(catgets(calendar->DT_catd, 1, 40, "View"));
	option1 = XmStringCreateLocalized(catgets(calendar->DT_catd, 1, 272, "All"));
	option2 = XmStringCreateLocalized(catgets(calendar->DT_catd, 1, 783, "Pending"));
	option3 = XmStringCreateLocalized(catgets(calendar->DT_catd, 1, 784, "Completed"));

	menu = XmCreatePulldownMenu(parent, "_pulldown", NULL, 0);

	XtSetArg(args[ac], XmNsubMenuId, menu); ac++;

	cascade = XmCreateOptionMenu(parent, "option_m", args, ac);

        menuitem = XtVaCreateWidget("all",
                       xmPushButtonGadgetClass, menu,
                       XmNlabelString, option1,
                       XmNuserData, VIEW_ALL,
                       NULL);

	XtAddCallback(menuitem, XmNactivateCallback, cb_func, data);

        menuitem = XtVaCreateWidget("pending",
                       xmPushButtonGadgetClass, menu,
                       XmNlabelString, option2,
                       XmNuserData, VIEW_PENDING,
                       NULL);

	XtAddCallback(menuitem, XmNactivateCallback, cb_func, data);

        menuitem = XtVaCreateWidget("completed",
                       xmPushButtonGadgetClass, menu,
                       XmNlabelString, option3,
                       XmNuserData, VIEW_COMPLETED,
                       NULL);

	XtAddCallback(menuitem, XmNactivateCallback, cb_func, data);


	XmStringFree(label);
	XmStringFree(option1);
	XmStringFree(option2);
	XmStringFree(option3);

	ManageChildren(menu);

	return cascade;
}

/*
**  Build the popup to display, in list fashion, the todos for a given period.
*/
static void
t_build_view_popup(ToDo *t) {

	Calendar	*c = t->cal;
	XmString	xmstr;
	Widget 		label, separator;
	char		*title;

	/*
	**  Dialog shell and form
	*/
	title = XtNewString(catgets(c->DT_catd, 1, 1012, "Calendar : To Do List"));
	t->view_frame = XtVaCreatePopupShell("todo_list",
		xmDialogShellWidgetClass, c->frame,
		XmNtitle, title,
		XmNdeleteResponse, XmDO_NOTHING,
		XmNallowShellResize, True,
		XmNmappedWhenManaged, False,
		XmNautoUnmanage, False,
		NULL);
	XtFree(title);
	setup_quit_handler(t->view_frame, t_view_quit_handler, (caddr_t)t);

	t->view_form = XtVaCreateWidget("apptform",
      			xmFormWidgetClass, t->view_frame,
      			XmNfractionBase, 4,
			XmNautoUnmanage, False,
      			NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 655, "OK"));


	t->view_ok_button = XtVaCreateWidget("ok",
      				xmPushButtonWidgetClass, t->view_form,
      				XmNlabelString, xmstr,
      				XmNleftAttachment, XmATTACH_POSITION,
      				XmNleftPosition, 0,
      				XmNleftOffset, 10,
      				XmNrightAttachment, XmATTACH_POSITION,
      				XmNrightPosition, 1,
      				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset,         5,
      				NULL);

	XmStringFree(xmstr);


	XtAddCallback(t->view_ok_button, XmNactivateCallback, (XtCallbackProc) t_view_ok_cb, t);


	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 460, "Apply"));

	t->view_apply_button = XtVaCreateWidget("apply",
      				xmPushButtonWidgetClass, t->view_form,
      				XmNlabelString, xmstr,
      				XmNleftAttachment, XmATTACH_POSITION,
      				XmNleftPosition, 1,
      				XmNrightAttachment, XmATTACH_POSITION,
      				XmNrightPosition, 2,
      				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset,         5,
      				NULL);
	XmStringFree(xmstr);

	XtAddCallback(t->view_apply_button, XmNactivateCallback, (XtCallbackProc) t_view_apply_cb, t);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 923, "Cancel"));

	t->view_cancel_button = XtVaCreateWidget("cancel",
      				xmPushButtonWidgetClass, t->view_form,
      				XmNlabelString, xmstr,
      				XmNleftAttachment, XmATTACH_POSITION,
      				XmNleftPosition, 2,
      				XmNrightAttachment, XmATTACH_POSITION,
      				XmNrightPosition, 3,
      				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset,         5,
      				NULL);

	XmStringFree(xmstr);

	XtAddCallback(t->view_cancel_button, XmNactivateCallback, (XtCallbackProc) t_view_cancel_cb, t);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));

	t->view_help_button = XtVaCreateWidget("help",
      				xmPushButtonWidgetClass, t->view_form,
      				XmNlabelString, xmstr,
      				XmNleftAttachment, XmATTACH_POSITION,
      				XmNleftPosition, 3,
      				XmNrightAttachment, XmATTACH_POSITION,
      				XmNrightPosition, 4,
      				XmNrightOffset, 10,
      				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset,         5,
      				NULL);
	XmStringFree(xmstr);


	separator = XtVaCreateWidget("separator",
                xmSeparatorGadgetClass,
                t->view_form,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
                XmNbottomAttachment,   	XmATTACH_WIDGET,
                XmNbottomWidget,        t->view_ok_button,
		XmNbottomOffset,	5,
                NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 789, "To Do Type:"));
	label= XtVaCreateWidget("type_label",
                xmLabelGadgetClass, 	t->view_form,
		XmNtopAttachment, 	XmATTACH_FORM,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		XmNtopOffset, 		8,
		XmNlabelString, 	xmstr,
		NULL);
	XmStringFree(xmstr);


	t->view_filter_menu = create_filter_menu(t->view_form, t_view_filter_proc, (XtPointer) t);

	XtVaSetValues(t->view_filter_menu,
		XmNuserData, 		REPEAT_DAYS,
		XmNtopAttachment, 	XmATTACH_FORM,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		label,
		XmNtopOffset, 		3,
		XmNleftOffset, 		5,
		NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 790, "context"));
	t->view_list_label= XtVaCreateWidget("type_label",
                xmLabelGadgetClass, 	t->view_form,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		t->view_filter_menu, 
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		XmNtopOffset, 		5,
		XmNlabelString, 	xmstr,
		NULL);
	XmStringFree(xmstr);

	t->view_sw_mgr = XtVaCreateWidget("view_sw_mgr",
		xmScrolledWindowWidgetClass, t->view_form,
		XmNscrollingPolicy, 	XmAUTOMATIC,
		XmNlistSizePolicy, 	XmCONSTANT,
		XmNscrolledWindowMarginHeight, 5,
		XmNscrolledWindowMarginWidth, 5,
		XmNwidth, 		400,
		XmNheight, 		200,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset,          5,
		XmNrightAttachment, 	XmATTACH_FORM, 
		XmNrightOffset,         5,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		t->view_list_label, 
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	separator,
		XmNbottomOffset,	5,
		NULL);
	t->view_form_mgr = XtVaCreateWidget("view_form_mgr",
		xmFormWidgetClass, t->view_sw_mgr,
		NULL);

        XtAddCallback(t->view_help_button, XmNactivateCallback,
                (XtCallbackProc)help_cb, TODO_LIST_HELP_BUTTON);
        XtAddCallback(t->view_form, XmNhelpCallback,
                (XtCallbackProc)help_cb, (XtPointer) TODO_LIST_HELP_BUTTON);

	ManageChildren(t->view_form); 
	XtManageChild(t->view_sw_mgr);
	XtManageChild(t->view_form_mgr); 
	XtManageChild(t->view_form);

	t->view_list = NULL;
	t->view_filter = VIEW_ALL;
	t->todo_view_is_up = False;
	t->view_list_modified = False;
}

/*
**  Button action procedures manage and unmanage the "extra" stuff in the UI to
**  make it visible and invisible to the user.
*/
static void
t_expand_ui_proc(Widget w, XtPointer client_data, XtPointer data) {
	ToDo		*t = (ToDo *)client_data;
	Props_pu	*p = (Props_pu *)t->cal->properties_pu;
	XmString	xmstr;
	Dimension	h1, h2;
	Dimension       h, height, width;
	static Boolean	expand_state_closed = True;

	/* This is really hokey.  There is a problem in the Motif code 
	   that figures out traversals.  In the case of the appointment 
	   editor, when the widgets are traversed, that the appointment 
	   editor is them expanded, the traversal list is left in an 
	   incorrect state.  The only way to straighten this out is 
	   to trick the traversal code into re-evaluating the traversal 
	   list.  We do this by setting one of the forms insensitive, 
	   and then back to sensitive.  There is no visual impact, and 
	   it seems to work.  Do *not* remove these calls to 
	   XtSetSensitive(), of the synlib tests will stop working. */

	XtVaGetValues(t->todo_list_sw, XmNwidth, &width, NULL);
	if (expand_state_closed) {
		Widget	children[2];

		if (!t->reminders.bfpm_form_mgr)
			t_build_expand(t);

		XtRealizeWidget(t->rfp.rfp_form_mgr);
		XtRealizeWidget(t->reminders.bfpm_form_mgr);

		XtSetSensitive(t->rfp.rfp_form_mgr, False);

		xmstr = XmStringCreateLocalized(catgets(t->cal->DT_catd, 1, 625,
							"Less"));
		XtVaSetValues(t->expand_ui_button, XmNlabelString, xmstr,
			      NULL);
		XmStringFree(xmstr);

		XtVaGetValues(t->rfp.rfp_form_mgr, XmNheight, &h1, NULL);
		XtVaGetValues(t->reminders.bfpm_form_mgr, XmNheight, &h2, NULL);
		h = ( h1 > h2 ) ? h1 : h2;
                XtVaSetValues(t->separator1, XmNbottomOffset, h + 10, NULL);

		children[0] = t->rfp.rfp_form_mgr;
		children[1] = t->reminders.bfpm_form_mgr;

		XtManageChildren(children, 2);
		if (t->reminders.bfpm_form_mgr) {
			set_rfp_defaults(&t->rfp);
			set_reminders_defaults(&t->reminders);
		}

		expand_state_closed = False;
	} else {
		XtSetSensitive(t->rfp.rfp_form_mgr, False);
		xmstr = XmStringCreateLocalized(catgets(t->cal->DT_catd, 1, 626,
							"More"));
		XtVaSetValues(t->expand_ui_button, XmNlabelString, xmstr,
			      NULL);
		XmStringFree(xmstr);
		XtUnmanageChild(t->rfp.rfp_form_mgr);
		XtUnmanageChild(t->reminders.bfpm_form_mgr);
		XtVaSetValues(t->separator1, XmNbottomOffset, 0, NULL);
		expand_state_closed = True;
	}
	XtVaSetValues(t->todo_list_sw, XmNwidth, width, NULL);
	XtSetSensitive(t->rfp.rfp_form_mgr, True);
}

static Boolean
first_line_contains_text(char *str) {

	if (!str)
		return(False);

	while (*str && *str != '\n') {
		if (*str != ' ' && *str != '\t')
			return(True);

		str++;
	}

	return(False);
}
/*
**  This function will consume form values and stuff them into an appointment.
*/
static Dtcm_appointment*
t_form_to_appt(ToDo *t) {
	boolean_t		all_ok;
	Dtcm_appointment	*appt;
	char			*str;
        Calendar 		*c = t->cal;
	Props_pu		*p = (Props_pu *)t->cal->properties_pu;

	if (t->cal->general->version < DATAVER4)
		appt = allocate_appt_struct(appt_write, t->cal->general->version, 
				CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I,
	                        CSA_ENTRY_ATTR_LAST_UPDATE_I,
	                        CSA_ENTRY_ATTR_ORGANIZER_I,
	                        CSA_ENTRY_ATTR_START_DATE_I,
	                        CSA_ENTRY_ATTR_TYPE_I,
	                        CSA_ENTRY_ATTR_CLASSIFICATION_I,
	                        CSA_ENTRY_ATTR_END_DATE_I,
	                        CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
	                        CSA_ENTRY_ATTR_SUMMARY_I,
	                        CSA_ENTRY_ATTR_STATUS_I,
	                        CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
	                        CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
	                        CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I,
	                        CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I,
	                        CSA_ENTRY_ATTR_AUDIO_REMINDER_I,
	                        CSA_ENTRY_ATTR_FLASHING_REMINDER_I,
	                        CSA_ENTRY_ATTR_MAIL_REMINDER_I,
	                        CSA_ENTRY_ATTR_POPUP_REMINDER_I,
				NULL);
	else
		appt = allocate_appt_struct(appt_write, t->cal->general->version, 
				CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I,
	                        CSA_ENTRY_ATTR_LAST_UPDATE_I,
	                        CSA_ENTRY_ATTR_ORGANIZER_I,
	                        CSA_ENTRY_ATTR_START_DATE_I,
	                        CSA_ENTRY_ATTR_TYPE_I,
	                        CSA_ENTRY_ATTR_CLASSIFICATION_I,
	                        CSA_ENTRY_ATTR_END_DATE_I,
	                        CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
	                        CSA_ENTRY_ATTR_SUMMARY_I,
	                        CSA_ENTRY_ATTR_STATUS_I,
	                        CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
	                        CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
	                        CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I,
	                        CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I,
	                        CSA_ENTRY_ATTR_AUDIO_REMINDER_I,
	                        CSA_ENTRY_ATTR_FLASHING_REMINDER_I,
	                        CSA_ENTRY_ATTR_MAIL_REMINDER_I,
	                        CSA_ENTRY_ATTR_POPUP_REMINDER_I,
	                        CSA_ENTRY_ATTR_RECURRENCE_RULE_I,
				NULL);

	/* We have a special case in the "todo" department here.  The 
	   dialog only has the concept of a start time.  The backing 
	   widget structure dssw.* has an end time defined in it, and 
	   the generic routine dssw_form_to_appt() wants to check it's 
	   value.  Not good, as the unused widget may have an illegal 
	   value left around in it.  To get around this problem, before 
	   grabbing the results out of the form, we will copy the 
	   "start time" value into the "end time" value.  This will 
	   make the check come out OK, and the duration appear to be 0. */

	str = XmTextGetString(t->dssw.start_text);
	XmTextSetString(t->dssw.stop_text, str);
	XtFree(str);
	XmToggleButtonSetState(t->dssw.stop_am, XmToggleButtonGetState(t->dssw.start_am), True);
	XmToggleButtonSetState(t->dssw.stop_pm, XmToggleButtonGetState(t->dssw.start_pm), True);

	/* For todo items, since no time shows up in the todo dialog 
	   item chooser dialog, we really need the first line of the 
	   summary to contain some text, otherwise there isn't anything 
	   visible to choose.  Check that here. */

	str = XmTextGetString(t->dssw.what_text);
	if (!first_line_contains_text(str)) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 837, "Calendar : Error - To Do Editor"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 913,
				"A To Do item must have text in the first line of the What item."));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		dialog_popup(t->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		XtFree(str);
		return(NULL);
	}
	XtFree(str);

	/* 
	 * Type needs to be set before dssw_form_to_appt() to insure
	 * correct error message is used.
	 */
	appt->type->value->item.sint32_value = CSA_TYPE_TODO;


	all_ok = dssw_form_to_appt(&t->dssw, appt,
		t->cal->view->current_calendar, t->cal->view->date);
	if (all_ok)
		all_ok = rfp_form_to_appt(&t->rfp, appt,
					  t->cal->view->current_calendar);
	if (all_ok)
		all_ok = reminders_form_to_appt(&t->reminders, appt,
					  t->cal->view->current_calendar);

	if (!all_ok) {
		free_appt_struct(&appt);
		return(NULL);
	}

	/* for todo appointments, we want to make sure that any reminder 
	   other than the mail reminder is not set up.  This can happen 
	   if the users default reminders have been set in this way. */


	if (appt->beep->name) {
		free(appt->beep->name);
		appt->beep->name = NULL;
	}

	if (appt->flash->name) {
		free(appt->flash->name);
		appt->flash->name = NULL;
	}

	if (appt->popup->name) {
		free(appt->popup->name);
		appt->popup->name = NULL;
	}

	t->completed_val = XmToggleButtonGadgetGetState(t->completed_toggle);

	appt->state->value->item.sint32_value = (t->completed_val) ?
		CSA_STATUS_COMPLETED : CSA_X_DT_STATUS_ACTIVE;
	appt->show_time->value->item.sint32_value = True;

	return appt;
}

/*
**  Action procedures
*/
static void
t_insert_proc(Widget w, XtPointer client_data, XtPointer data) {
	int			wk;
	ToDo			*t = (ToDo *)client_data;
	Props_pu		*p = (Props_pu *)t->cal->properties_pu;
	CSA_entry_handle	new_a;
	CSA_return_code		stat;
	Dtcm_appointment	*appt;
	CSA_enum		scope;
	static int		answer;
	Repeat_menu_op	 op = ONE_TIME;
	RFP		*rfp = &t->rfp;
	Calendar	*c = t->cal;

	_DtTurnOnHourGlass(t->frame);
	if ((appt = t_form_to_appt(t)) == NULL) {
		_DtTurnOffHourGlass(t->frame);
		return;
	}

	/* the gui does not support specifying the sequence end date */
	if (appt->sequence_end_date && appt->sequence_end_date->name) {
		free(appt->sequence_end_date->name);
		appt->sequence_end_date->name = NULL;
	}
	/*
	**  First, get the value on the repeat menu
	*/
	answer = 2;
	if (rfp->repeat_menu)
	  op = rfp->repeat_type;
	else
	  op = ONE_TIME;

	if (op != ONE_TIME) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 1102,
					"Insert To Do"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 1103,
					"This To Do is part of a repeating series.\nDo you want to insert the item ...?"));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
	  	char *ident4 = XtNewString(catgets(c->DT_catd, 1, 272, "All"));
		answer = dialog_popup(c->frame,
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
		_DtTurnOffHourGlass(t->frame);
		return;
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

	stat = csa_add_entry(t->cal->cal_handle, appt->count, appt->attrs, &new_a, NULL);
	backend_err_msg(t->frame, t->cal->view->current_calendar, stat,
			p->xm_error_pixmap);
	free_appt_struct(&appt);
	if (stat != CSA_SUCCESS) {
		_DtTurnOffHourGlass(t->frame);
		return;
	}
	csa_free((CSA_buffer) new_a);

	set_todo_msg_defaults(t);
	add_all_todo(t);
	if (todo_view_showing(t))
		build_todo_view(t, t->cal->view->glance, False);
	reset_alarm(t->cal);
	invalidate_cache(t->cal);
	_DtTurnOffHourGlass(t->frame);
}

static void
t_delete_proc(Widget w, XtPointer client_data, XtPointer data) {
	int			*item_list = NULL, item_cnt = 0;
	ToDo			*t = (ToDo *)client_data;
        Calendar 		*c = t->cal;
	Props_pu		*p = (Props_pu *)t->cal->properties_pu;
	CSA_entry_handle	entry;
	CSA_return_code		stat;
	CSA_enum		scope;
	Dtcm_appointment	*appt;
	static int		answer;

	_DtTurnOnHourGlass(t->frame);
	if (!XmListGetSelectedPos(t->todo_list, &item_list, &item_cnt)) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 566, "Calendar : Error - To Do Editor"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 567,
				"Select a To Do and DELETE again."));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(t->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		_DtTurnOffHourGlass(t->frame);
		return;
	} 

	if (!(entry = t_nth_appt(t, item_list[0] - 1))) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 566, "Calendar : Error - To Do Editor"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 570,
				"Internal error selecting To Do.\nTo Do was not deleted."));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(t->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		XtFree((XtPointer)item_list);
		_DtTurnOffHourGlass(t->frame);
		return;
	}
	XtFree((XtPointer)item_list);

	answer = 0;
	appt = allocate_appt_struct(appt_read, 
				    	c->general->version,
					CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I, 
					NULL);
	stat = query_appt_struct(c->cal_handle, entry, appt);
	backend_err_msg(t->frame, c->view->current_calendar, stat,
			p->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		_DtTurnOffHourGlass(t->frame);
		return;
	}

	if (appt->repeat_type->value) {
		if (appt->repeat_type->value->item.sint32_value !=
		    CSA_X_DT_REPEAT_ONETIME) {
		  char *title = XtNewString(catgets(c->DT_catd, 1, 591, "Calendar : To Do Editor"));
		  char *text = XtNewString(catgets(c->DT_catd, 1, 573,
					"This To Do is part of a repeating series.\nDo you want to delete ...?"));
		  char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
		  char *ident2 = XtNewString(catgets(c->DT_catd, 1, 270,
					"This One Only"));
		  char *ident3 = XtNewString(catgets(c->DT_catd, 1, 271, "Forward"));
		  char *ident4 = XtNewString(catgets(c->DT_catd, 1, 272, "All"));
		answer = dialog_popup(t->frame,
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
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 591,
                                        "Calendar : To Do Editor"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 791,
                                        "This To Do repeats in an unknown fashion.  All occurrences will be changed\nDo you still wish to delete it?"));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
	  	char *ident4 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
                answer = dialog_popup(c->frame,
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
		_DtTurnOffHourGlass(t->frame);
		return;
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

	stat = csa_delete_entry(t->cal->cal_handle, entry, scope, NULL);
	backend_err_msg(t->frame, t->cal->view->current_calendar, stat,
			p->xm_error_pixmap);	

	set_todo_msg_defaults(t);
	add_all_todo(t);
	if (todo_view_showing(t))
		build_todo_view(t, t->cal->view->glance, False);
	reset_alarm(t->cal);
	invalidate_cache(t->cal);
	_DtTurnOffHourGlass(t->frame);
}

/*
**  Static function actually does the change - used by the editor and the view
**  list.
*/
static CSA_return_code
t_execute_change(ToDo *t, CSA_entry_handle old_a, Dtcm_appointment *new_a,
		 Widget frame) {
        Calendar 		*c = t->cal;
	Props_pu		*p = (Props_pu *)t->cal->properties_pu;
	CSA_entry_handle	updated_a;
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
	backend_err_msg(t->frame, c->view->current_calendar, stat,
			p->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return stat;
	}

        if ((appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER) ||
            (appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER_WEEKLY) ||
            (appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER_MONTHLY) ||
            (appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER_YEARLY)) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 591,
                                        "Calendar : To Do Editor"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 708,
                                        "This appointment repeats in an unknown fashion.  All occurrences will be changed\nDo you still wish to change it?"));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
	  	char *ident4 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
                answer = dialog_popup(c->frame,
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
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 591, "Calendar : To Do Editor"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 579,
				"This To Do is part of a repeating series.\nDo you want to change ...?"));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
	  	char *ident2 = XtNewString(catgets(c->DT_catd, 1, 270,
				"This One Only"));
	  	char *ident3 = XtNewString(catgets(c->DT_catd, 1, 271, "Forward"));
	  	char *ident4 = XtNewString(catgets(c->DT_catd, 1, 272, "All"));
		answer = dialog_popup(frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			BUTTON_IDENT, 2, ident2,
			BUTTON_IDENT, 3, ident3,
			(compare_repeat_info(appt, &(t->rfp), c->cal_handle, c->general->version) ?
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
		return CSA_E_FAILURE;
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

	stat = csa_update_entry_attributes(c->cal_handle, old_a, scope, CSA_FALSE, new_a->count, new_a->attrs, &updated_a, NULL);
	backend_err_msg(t->frame, c->view->current_calendar, stat,
			p->xm_error_pixmap);
	if (stat != CSA_SUCCESS)
		return stat;
	csa_free((CSA_buffer) updated_a);

	if (todo_showing(t)) {
		set_todo_msg_defaults(t);
		add_all_todo(t);
	}
	if (todo_view_showing(t))
		build_todo_view(t, c->view->glance, False);
	reset_alarm(c);
	invalidate_cache(t->cal);

	return stat;
}

static void
t_view_change_proc(Widget w, XtPointer cdata, XtPointer cbs) {
	ToDo			*t = (ToDo *)cdata;
	TodoView		*step = t->view_list;

	while (step) {
		if (step->view_item_toggle == w) {
			step->modified = True;
			t->view_list_modified = True;
			return;
		}
		step = step->next;
	}
}

static void
t_change_proc(Widget w, XtPointer client_data, XtPointer data) {
	int			*item_list = NULL, item_cnt = 0;
	ToDo			*t = (ToDo *)client_data;
        Calendar 		*c = t->cal;
	Props_pu		*p = (Props_pu *)t->cal->properties_pu;
	CSA_entry_handle	old_a;
	Dtcm_appointment	*new_a;
	static int		answer;

	_DtTurnOnHourGlass(t->frame);
	if (!XmListGetSelectedPos(t->todo_list, &item_list, &item_cnt)) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 566, "Calendar : Error - To Do Editor"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 585,
				"Select a To Do and CHANGE again."));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(t->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		_DtTurnOffHourGlass(t->frame);
		return;
	} 

	if (!(old_a = t_nth_appt(t, item_list[0] - 1))) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 566, "Calendar : Error - To Do Editor"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 1009,
				"Internal error selecting To Do.\nTo Do was not changed."));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(t->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		XtFree((XtPointer)item_list);
		_DtTurnOffHourGlass(t->frame);
		return;
	}
	XtFree((XtPointer)item_list);

	if ((new_a = t_form_to_appt(t)) != NULL) {
		t_execute_change(t, old_a, new_a, t->frame);
		free_appt_struct(&new_a);
	}

	_DtTurnOffHourGlass(t->frame);
}

static void
t_clear_proc(Widget w, XtPointer client_data, XtPointer data) {
	ToDo *t = (ToDo *)client_data;

	set_todo_defaults(t);
	add_all_todo(t);
}

static void
t_close_proc(Widget w, XtPointer client_data, XtPointer data) {
	ToDo *t = (ToDo *)client_data;

	XtPopdown(t->frame);
	t->todo_is_up = False;
}

static void
t_quit_handler(Widget w, XtPointer cdata, XtPointer data) {
	ToDo *t = (ToDo *)cdata;

	todo_clean_up(t);
	XtPopdown(t->frame);
	t->todo_is_up = False;
}


/*
 * dragMotionHandler
 *
 * Determine if the pointer has moved beyond the drag threshold while button 1
 * was being held down.
 */
static void
FormApptDragMotionHandler(
        Widget          dragInitiator,
        XtPointer       clientData,
        XEvent         *event)
{
        int             diffX, diffY;
        Calendar	*c = (Calendar *) clientData;
	ToDo		*t = (ToDo *) c->todo;
	Dimension	source_height, source_width;
	Position	source_x, source_y;
 
        if (!t->doing_drag) {

		/* check to see if the iniital value was within the 
		   bounds for the drag source icon. */

		XtVaGetValues(t->drag_source, 
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
                if (t->initialX == -1 && t->initialY == -1) {
                        t->initialX = event->xmotion.x;
                        t->initialY = event->xmotion.y;
                }
                /*
                 * Find out how far pointer has moved since button press
                 */
                diffX = t->initialX - event->xmotion.x;
                diffY = t->initialY - event->xmotion.y;
 
                if ((ABS(diffX) >= DRAG_THRESHOLD) ||
                    (ABS(diffY) >= DRAG_THRESHOLD)) {
                        t->doing_drag = True;
			ApptDragStart(dragInitiator, event, c, TodoEditorIcon);
                        t->initialX = -1;
                        t->initialY = -1;
                }
        }
}

extern void
t_make_todo(Calendar *c)
{
	int		cnt = 0;
	Arg		args[15];
        ToDo		*t = (ToDo *)c->todo;
	Props_pu	*p = (Props_pu *)c->properties_pu;
	XmString	xmstr;
	Dimension	highest, widest;
	Widget		prev, curr;
	Dimension	_toLabel, _toText, _toMenu, _toRC;
	Dimension       label_height,
			max_left_label_width;
	Boolean		btn1_transfer;
        XtTranslations  new_translations;
	XFontSetExtents	listfontextents;
        static char     translations[] = "\
                ~c ~s ~m ~a <Btn1Down>:\
		 dtcm-process-press(ListBeginSelect,TranslationDragStart)\n\
                c ~s ~m ~a <Btn1Down>:\
		 dtcm-process-press(ListBeginToggle,TranslationDragStart)";
	static char     btn2_translations[] = "\
                ~c ~s ~m ~a <Btn2Down>:\
		 dtcm-process-press(ListBeginSelect,TranslationDragStart)\n\
                c ~s ~m ~a <Btn2Down>:\
		 dtcm-process-press(ListBeginToggle,TranslationDragStart)\n\
                <Btn2Motion>:ListButtonMotion()\n\
                ~c ~s ~m ~a <Btn2Up>:ListEndSelect()\n\
                c ~s ~m ~a <Btn2Up>:ListEndToggle()";
	WidgetList	children;
	Widget		widgets[20];
	int		i = 0,
			j = 0,
			n;
	

        new_translations = XtParseTranslationTable(translations);

	t->cal = c;

	/*
	**  Dialog shell and the base form
	*/
	t->frame = XtVaCreatePopupShell("frame",
		xmDialogShellWidgetClass, t->cal->frame,
		XmNdeleteResponse, 	XmDO_NOTHING,
		XmNallowShellResize, 	True,
		XmNmappedWhenManaged, 	False,
		NULL);
	set_todo_title(t, c->view->current_calendar);
	setup_quit_handler(t->frame, t_quit_handler, (caddr_t)t);

	t->base_form_mgr = XtVaCreateWidget("base_form_mgr",
		xmFormWidgetClass, 	t->frame,
		XmNautoUnmanage, 	False,
		XmNfractionBase, 	15,
		NULL);

	/*
	**  Build the stuff in the upper portion of the form
	*/

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 797, "To Do"));
	t->list_label = XtVaCreateWidget("todo_label",
                xmLabelGadgetClass, 	t->base_form_mgr,
		XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset, 		5,
		XmNlabelString, 	xmstr,
		NULL);
	XmStringFree(xmstr);

	XtVaGetValues(t->list_label, XmNheight, &label_height, NULL);

	t->message_text = XtVaCreateWidget("message",
                xmLabelGadgetClass,     t->base_form_mgr,
		XmNshadowThickness, 	0,
		XmNcursorPositionVisible, False,
		XmNresizeWidth, 	True,
		XmNalignment,           XmALIGNMENT_BEGINNING,
		XmNbottomAttachment,    XmATTACH_FORM,
		XmNbottomOffset,        2,
		XmNleftAttachment,      XmATTACH_FORM,
		XmNleftOffset,          2,
		XmNrightAttachment,     XmATTACH_FORM,
		XmNrightOffset,         2,
		NULL);

	/*
	**  Create insert, delete, change, and clear buttons
	*/
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 342, "Insert"));
        t->insert_button = XtVaCreateWidget("insert",
		xmPushButtonWidgetClass, t->base_form_mgr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	t->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	1,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	3,
		XmNlabelString, 	xmstr,
                NULL);
	XtAddCallback(t->insert_button, XmNactivateCallback, t_insert_proc, t);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 343, "Change"));
        t->change_button = XtVaCreateWidget("change",
		xmPushButtonWidgetClass, t->base_form_mgr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	t->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	3,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	5,
		XmNlabelString, 	xmstr,
		XmNsensitive, 		False,
                NULL);
	XtAddCallback(t->change_button, XmNactivateCallback, t_change_proc, t);
	XmStringFree(xmstr);
 
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 802, "Delete"));
        t->delete_button = XtVaCreateWidget("delete",
		xmPushButtonWidgetClass, t->base_form_mgr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	t->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	5,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	7,
		XmNlabelString, 	xmstr,
		XmNsensitive, 	False,
                NULL);
	XtAddCallback(t->delete_button, XmNactivateCallback, t_delete_proc, t);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 803, "Clear"));
        t->clear_button = XtVaCreateWidget("clear",
		xmPushButtonWidgetClass, t->base_form_mgr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	t->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	7,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	9,
		XmNlabelString, 	xmstr,
                NULL);
	XtAddCallback(t->clear_button, XmNactivateCallback, t_clear_proc, t);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 923, "Cancel"));
        t->close_button = XtVaCreateWidget("close",
		xmPushButtonWidgetClass, t->base_form_mgr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	t->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	9,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	11,
		XmNlabelString, 	xmstr,
                NULL);
	XtAddCallback(t->close_button, XmNactivateCallback, t_close_proc, t);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
        t->help_button = XtVaCreateWidget("help",
		xmPushButtonWidgetClass, t->base_form_mgr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	t->message_text,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	11,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	13,
		XmNlabelString, 	xmstr,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(t->help_button, XmNactivateCallback,
		(XtCallbackProc)help_cb, TODO_EDITOR_HELP_BUTTON);
        XtAddCallback(t->base_form_mgr, XmNhelpCallback,
                (XtCallbackProc)help_cb, (XtPointer) TODO_EDITOR_HELP_BUTTON);


	t->separator2 = XtVaCreateWidget("separator2",
		xmSeparatorGadgetClass, t->base_form_mgr,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	t->insert_button,
		XmNbottomOffset, 	5,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNrightOffset, 	5,
		NULL);

	t->separator1 = XtVaCreateWidget("separator1",
		xmSeparatorGadgetClass, t->base_form_mgr,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNrightOffset, 	5,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	t->separator2,
		NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 626, "More"));
        t->expand_ui_button = XtVaCreateWidget("expando",
		xmPushButtonWidgetClass, t->base_form_mgr,
		XmNlabelString, 	xmstr,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	t->separator1,
		XmNbottomOffset, 	3,
		XmNnavigationType, 	XmTAB_GROUP,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(t->expand_ui_button, 
			XmNactivateCallback, t_expand_ui_proc, t);

	/*
	**  First, manipulate the dssw widget.  Why, you ask, don't you just
	**  recreate the stuff you need instead of using the entire widget.
	**  Three reasons:  first, to re-use the methods associated with the
	**  dssw structure; second, in case HIE changes their mind, adjusting
	**  the UI is trivial; and third, if the dssw widget changes or needs
	**  to be fixed, the code is in one place.
	*/
	build_dssw(&t->dssw, c, t->base_form_mgr, True, False);

	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 590, "Due Date:"));
	XtVaSetValues(t->dssw.date_label,
		XmNlabelString, xmstr,
		NULL);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 798, "Time Due:"));
	XtVaSetValues(t->dssw.start_label,
		XmNlabelString, xmstr,
		NULL);
	XmStringFree(xmstr);

	_i18n_WidestWidget( 3, &curr, &max_left_label_width, t->dssw.date_label,
			 t->dssw.start_label, t->dssw.what_label );

	/*
	 * t->dssw.date_label, t->dssw.date_text
	 */
	_i18n_WidestWidget( 1, &curr, &widest, t->dssw.date_label );
	_toLabel = max_left_label_width + 2 * 5 - 5 - widest;
	_toText = max_left_label_width + 2 * 5;
	XtVaSetValues( t->dssw.date_label,
			XmNleftOffset, _toLabel,
			NULL );
	XtVaSetValues( t->dssw.date_text,
			XmNleftOffset, _toText,
			NULL );
	/*
	 * t->dssw.start_label, t->dssw.start_text, t->dssw.start.menu,
	 * t->dssw.start_ampm_rc_mgr
	 */
        _i18n_WidestWidget( 1, &curr, &widest, t->dssw.start_label );
        _toLabel = max_left_label_width + 2 * 5 - 5 - widest;
        _toText = max_left_label_width + 2 * 5;
        _i18n_WidestWidget( 1, &curr, &widest, t->dssw.start_text );
        _toMenu = _toText + widest + 5;
        _i18n_WidestWidget( 1, &curr, &widest, t->dssw.start_menu );
        _toRC = _toMenu + widest + 5;
	XtVaSetValues( t->dssw.start_label,
			XmNleftOffset, _toLabel,
			NULL );
	XtVaSetValues( t->dssw.start_text,
			XmNleftOffset, _toText,
			NULL );
	XtVaSetValues( t->dssw.start_menu,
			XmNleftOffset, _toMenu,
			NULL );
	XtVaSetValues( t->dssw.start_ampm_rc_mgr,
			XmNleftOffset, _toRC,
			NULL );

	_i18n_HighestWidget( 4, &prev, &highest, t->dssw.start_label,
	t->dssw.start_text, t->dssw.start_menu, t->dssw.start_ampm_rc_mgr );
        _i18n_WidestWidget( 1, &curr, &widest, t->dssw.what_label );
        _toLabel = max_left_label_width + 2 * 5 - 5 - widest;
        _toText = max_left_label_width + 2 * 5;
	XtVaSetValues( t->dssw.what_label,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNleftOffset, _toLabel,
			NULL );	

	/*
	 * Add a drag source icon inside the dssw, lower right
	 */
	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 906, "Drag To Do"));
	t->drag_source = XtVaCreateWidget("drag_source",
		dtIconGadgetClass, 	t->dssw.dssw_form_mgr,
		XmNpixmapPosition, 	XmPIXMAP_TOP,
		XmNstringPosition, 	XmSTRING_BOTTOM,
		XmNalignment, 		XmALIGNMENT_CENTER,
		XmNstring, 		xmstr,
		XmNbottomAttachment, 	XmATTACH_FORM,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNtraversalOn, 	False,
		NULL);
	XmStringFree(xmstr);

	XtVaSetValues( t->dssw.what_scrollwindow,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNleftOffset, _toText,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, t->drag_source,
			NULL );	

        XtAddEventHandler(XtParent(t->drag_source), Button1MotionMask, False,
                (XtEventHandler)FormApptDragMotionHandler, (XtPointer) c);
	
        XtVaGetValues((Widget)XmGetXmDisplay(XtDisplay(c->frame)), 
                "enableBtn1Transfer",   &btn1_transfer, 
                NULL); 

        /* btn1_transfer is a tri-state variable - see 1195846 */ 
	if ((Boolean)btn1_transfer != True)
                XtAddEventHandler(XtParent(t->drag_source), 
                                Button2MotionMask, False, 
                                (XtEventHandler)FormApptDragMotionHandler, 
                                (XtPointer) c);



	XtVaSetValues(t->drag_source, XmNpixmap, p->drag_icon_xbm, NULL);

	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 784, "Completed"));

	t->completed_toggle = XtVaCreateManagedWidget("completed",
		xmToggleButtonGadgetClass, t->dssw.dssw_form_mgr,
		XmNlabelString, 	xmstr,
                XmNbottomAttachment,    XmATTACH_FORM,
		XmNnavigationType, 	XmTAB_GROUP,
		NULL);
	XmStringFree(xmstr);

        _toText = max_left_label_width + 2 * 5 - 4;
	XtVaSetValues( t->completed_toggle,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toText,
			NULL );

	_i18n_HighestWidget( 2, &prev, &highest, t->dssw.what_label,
				t->dssw.what_scrollwindow );
	XtVaSetValues( prev,
			XmNbottomAttachment, XmATTACH_WIDGET,
			XmNbottomWidget, t->completed_toggle,
			XmNbottomOffset, 10,
			NULL );

	XtVaSetValues(t->dssw.dssw_form_mgr,
                XmNtopAttachment, 	XmATTACH_FORM,
                XmNtopOffset, 		label_height + 5,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		5,
		XmNbottomAttachment, 	XmATTACH_WIDGET,
                XmNbottomWidget, 	t->expand_ui_button,
                XmNbottomOffset, 	3,
		NULL);

        XtVaGetValues(t->dssw.dssw_form_mgr,
                XmNchildren,            &children,
                XmNnumChildren,         &n,
                NULL);

        /* We don't want to manage some of the widgets */
	for (i = 0; i < n; i++) {
		if ((children[i] == t->dssw.stop_label) ||
		    (children[i] == t->dssw.stop_menu) ||
		    (children[i] == t->dssw.stop_ampm_rc_mgr) ||
		    (children[i] == t->dssw.stop_text))
			continue;
		widgets[j++] = children[i];
	}
        XtManageChildren(widgets, n - 4);

	/*
	**  Build the list and separators
	*/
	CalFontExtents(c->fonts->labelfont, &listfontextents);;

	cnt = 0;
	XtSetArg(args[cnt], XmNlistSizePolicy, XmCONSTANT); 		++cnt;
	XtSetArg(args[cnt], XmNwidth, 15 * 
		listfontextents.max_logical_extent.width); 		++cnt;
	XtSetArg(args[cnt], XmNscrollBarDisplayPolicy, XmSTATIC); 	++cnt;
	XtSetArg(args[cnt], XmNdoubleClickInterval, 5); 		++cnt; 
        t->todo_list = (Widget)XmCreateScrolledList(t->base_form_mgr,
						"todo_list", args, cnt);
	t->todo_list_sw = XtParent(t->todo_list);

        XtOverrideTranslations(t->todo_list, new_translations);

        /* Make btn 2 do dnd of appts */
	/* btn1_transfer is a tri-state variable - see 1195846 */ 
	if ((Boolean)btn1_transfer != True) {   
                new_translations = XtParseTranslationTable(btn2_translations);
                XtOverrideTranslations(t->todo_list, new_translations);
        }

        XtVaSetValues(t->todo_list_sw,
                XmNtopAttachment, 	XmATTACH_FORM,
                XmNtopOffset, 		label_height + 11,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNrightOffset, 	5,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		t->dssw.dssw_form_mgr,
		XmNleftOffset, 		10,
		XmNbottomAttachment, 	XmATTACH_OPPOSITE_WIDGET,
		XmNbottomWidget, 	t->dssw.dssw_form_mgr,
                NULL);

	XtManageChild(t->todo_list);
	XtAddCallback(t->todo_list,
		XmNbrowseSelectionCallback, t_list_select_proc, t); 

	XtVaSetValues(t->list_label,
		XmNleftAttachment, 	XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, 		t->todo_list_sw,
		NULL);

        /* set default button */
        XtVaSetValues(t->base_form_mgr, 
		XmNdefaultButton, 	t->insert_button, 
		NULL);

        XtVaSetValues(t->base_form_mgr, 
		XmNcancelButton, 	t->close_button, 
		NULL);

        XmProcessTraversal(t->dssw.what_text, XmTRAVERSE_CURRENT);
        XtVaSetValues(t->base_form_mgr, 
		XmNinitialFocus, 	t->dssw.what_text, 
		NULL);

	ManageChildren(t->base_form_mgr);
	XtManageChild(t->base_form_mgr);

        rfp_init(&t->rfp, c, t->base_form_mgr);
	reminders_init(&t->reminders, c, t->base_form_mgr);

	t->todo_count = 0;
	t->todo_head = NULL;
	t->todo_is_up = False;
	t->initialX = -1;
        t->initialY = -1;
        t->doing_drag = False;
}

/*******************************************************************************
**
**  External functions
**
*******************************************************************************/
extern void
add_to_todo_list(CSA_entry_handle entry, ToDo *t) {
	char			*buf;
	Lines			*lines;
	XmString		str;
	CSA_return_code		stat;
	Dtcm_appointment	*appt;

	appt = allocate_appt_struct(appt_read, 
				    	t->cal->general->version,
					CSA_ENTRY_ATTR_SUMMARY_I, 
					NULL);
	stat = query_appt_struct(t->cal->cal_handle, entry, appt);
	backend_err_msg(t->frame, t->cal->view->current_calendar, stat,
			((Props_pu *)t->cal->properties_pu)->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return;
	}

	lines = text_to_lines(appt->what->value->item.string_value, 1);
	if (lines && lines->s) {
		buf = (char *)ckalloc(cm_strlen(lines->s) + 1);
		strcpy(buf, lines->s);
		destroy_lines(lines);
	} else {
		buf = (char *)ckalloc(1);
		buf[0] = '\0';
	}

	str = XmStringCreateLocalized(buf);
	XmListAddItem(t->todo_list, str, 0);
	free_appt_struct(&appt);
	XmStringFree(str);
	free(buf);
}

extern void
add_all_todo(ToDo *t) {
	int		i;
	CSA_uint32	count;
	char		*date, date_str[MAXNAMELEN];
	Tick		tick;
	CSA_entry_handle	*entry_list;
	Props		*p;
	OrderingType	o;
	SeparatorType	s;

	if (!todo_showing(t))
		return;

	p = (Props *)t->cal->properties;
	o = get_int_prop(p, CP_DATEORDERING);
	s = get_int_prop(p, CP_DATESEPARATOR);
	date = get_date_from_widget(t->cal->view->date, t->dssw.date_text, o, s);
	if (!date)
		return;

	sprintf(date_str, "%s", date);
	if ((strcasecmp(date, "today") == 0)
		|| (strcasecmp(date, "tomorrow") == 0)
		|| (strcasecmp(date, "yesterday") == 0))
		sprintf(date_str, "12:00 am");
	if ((tick = cm_getdate(date_str, NULL)) <= 0)
		return;
	build_todo_list(t, tick, dayGlance, &entry_list, &count, VIEW_ALL);

	XmListDeleteAllItems(t->todo_list);

	XtSetSensitive(t->delete_button, False);
        XtSetSensitive(t->change_button, False);

	if (t->todo_head && t->todo_count > 0)
		csa_free((CSA_buffer) t->todo_head);
	t->todo_head = entry_list;
	t->todo_count = count;
	for (i = 0; i < count; i++)
		add_to_todo_list(entry_list[i], t);
	if (count <= 0)
		XtSetSensitive(t->todo_list, False);
	else
		XtSetSensitive(t->todo_list, True);
}

static void
set_list_title(ToDo *t) {
        Calendar 	*c = t->cal;
	Props		*p = (Props *)c->properties;
	char		*header;
	char		buffer[BUFSIZ];
	char		buffer2[BUFSIZ];
	XmString	xmstr;

	switch (t->view_list_glance) {
		case yearGlance:
				header = catgets(c->DT_catd, 1, 806, "Year of %d");
				sprintf(buffer, header, year(t->view_list_date));
				break;
		case monthGlance:
				format_date(t->view_list_date+1, get_int_prop(p, CP_DATEORDERING), buffer2, 0, 0, 0);
				header = catgets(c->DT_catd, 1, 807, "%s");
				sprintf(buffer, header, buffer2);
				break;
		case weekGlance:
				format_date(t->view_list_date+1, get_int_prop(p, CP_DATEORDERING), buffer2, 1, 0, 0);
				header = catgets(c->DT_catd, 1, 808, "Week of %s");
				sprintf(buffer, header, buffer2);
				break;
		case dayGlance:
				format_date(t->view_list_date+1, get_int_prop(p, CP_DATEORDERING), buffer2, 1, 0, 0);
				header = catgets(c->DT_catd, 1, 809, "%s");
				sprintf(buffer, header, buffer2);
				break;
	}

	if (t->view_frame) {
		xmstr = XmStringCreateLocalized(buffer);
		XtVaSetValues(t->view_list_label, XmNlabelString, xmstr,
			NULL);
		XmStringFree(xmstr);
	}

}

extern void
build_todo_list(ToDo *t, Tick date, Glance glance, CSA_entry_handle **a, CSA_uint32 *count, todo_view_op vf) {
	int		range_count;
	CSA_sint32	state;
	time_t		start, stop;
	boolean_t	use_state = B_FALSE;
	CSA_return_code	stat;
	CSA_enum	*ops;
	CSA_attribute	*range_attrs;

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

	if (vf == VIEW_PENDING) {
		state = CSA_X_DT_STATUS_ACTIVE;
		use_state = B_TRUE;
	}
	else if (vf == VIEW_COMPLETED) {
		state = CSA_STATUS_COMPLETED;
		use_state = B_TRUE;
	}
	else
		use_state = B_FALSE;

	setup_range(&range_attrs, &ops, &range_count, start, stop,
		    CSA_TYPE_TODO, state, use_state, t->cal->general->version);
        stat = csa_list_entries(t->cal->cal_handle, range_count, range_attrs, ops, count, a, NULL);

	backend_err_msg(t->frame, t->cal->view->current_calendar, stat,
			((Props_pu *)t->cal->properties_pu)->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		*a = NULL;
		*count = 0;
	}
	free_range(&range_attrs, &ops, range_count);
}

extern int
build_todo_view(ToDo *t, Glance glance, Boolean redisplay) {
	int			cnt; 
	CSA_uint32		entry_count;
	char			*buf, *what_str, str1[MAXNAMELEN];
	char			str2[MAXNAMELEN];
	Lines			*lines;
	Props			*p = (Props *)t->cal->properties;
	XmString		str;
	TodoView		*step, *last = NULL;
	CSA_entry_handle	*entry_list;
	CSA_return_code		stat;
	OrderingType		o = get_int_prop(p, CP_DATEORDERING);
	SeparatorType		s = get_int_prop(p, CP_DATESEPARATOR);
	Dtcm_appointment	*appt;
	Tick			start_tick;

	/*
	**  First, get the list of to-do appointments
	*/

        if (redisplay == True) {
 
                /* On a redisplay, rebuild the list based on the
                   parameters of the last query. */
 
		build_todo_list(t, t->view_list_date, t->view_list_glance, &entry_list,
				&entry_count, t->view_filter);
        }
        else {
                /* If this is a clean display of the appointment list,
                   save the context so that a redisplay can be done if
                   something changes, like the display format or something
                   like that. */
 
		build_todo_list(t, t->cal->view->date, glance, &entry_list,
				&entry_count, t->view_filter);
 
                t->view_list_glance = glance;
                t->view_list_date = t->cal->view->date;
        }

	set_list_title(t);

	step = t->view_list;
	appt = allocate_appt_struct(appt_read,
				    t->cal->general->version,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_SUMMARY_I,
				    CSA_ENTRY_ATTR_STATUS_I,
				    NULL);

	/*
	**  We're going to re-use the list of widgets we may have built
	**  previously.  This optimization saves the expensive creating
	**  and destroying of all the widgets that make up the "list"
	*/
	for (cnt = 1; cnt <= entry_count; cnt++) {
		/*
		**  If there isn't a TodoView object, we've reached the end
		**  of the current list, so build another
		*/
		if (!step) {
			step = (TodoView *)ckalloc(sizeof(TodoView));

			sprintf(str1, "%d", cnt);
			sprintf(str2, "%s.", str1);
			str = XmStringCreateLocalized(str2);
			step->view_item_number = XtVaCreateManagedWidget("cnt",
				xmLabelGadgetClass, t->view_form_mgr,
				XmNlabelString, str,
				NULL);
			XmStringFree(str);

			step->view_item_toggle = XtVaCreateManagedWidget(str1,
				xmToggleButtonGadgetClass, t->view_form_mgr,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, step->view_item_number,
				XmNleftOffset, 5,
				XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET,
				XmNtopWidget, step->view_item_number,
				XmNtopOffset, -3,
				NULL);
			XtAddCallback(step->view_item_toggle,
				XmNvalueChangedCallback, t_view_change_proc, t);

			step->next = NULL;
			step->appt = 0;
			if (last) {
				XtVaSetValues(step->view_item_number,
					XmNtopAttachment, XmATTACH_WIDGET,
					XmNtopWidget, last->view_item_number,
					XmNtopOffset, 10,
					XmNleftAttachment,
						XmATTACH_OPPOSITE_WIDGET,
					XmNleftWidget, last->view_item_number,
					NULL);
				last->next = step;
			} else
				t->view_list = step;
		} else {
			XtManageChild(step->view_item_number);
			XtManageChild(step->view_item_toggle);
		}
		if (step->appt != 0)
			csa_free((CSA_buffer) step->appt);
		step->appt = entry_list[cnt - 1];
		step->modified = False;

		/*
		**  Create the text string describing the todo and set that
		**  value in the label gadget.
		*/
		stat = query_appt_struct(t->cal->cal_handle, entry_list[cnt - 1], appt);
		backend_err_msg(t->frame, t->cal->view->current_calendar, stat,
			((Props_pu *)t->cal->properties_pu)->xm_error_pixmap);
		if (stat == CSA_SUCCESS) {
			_csa_iso8601_to_tick(appt->time->value->item.date_time_value, &start_tick);
			format_tick(start_tick, o, s, str1);
			lines = text_to_lines(appt->what->value->item.string_value, 1);
			if (lines && lines->s) {
				buf = (char *)ckalloc(cm_strlen(str1)
						      + cm_strlen(lines->s) + 3);
				what_str = lines->s;
			} else {
				buf = (char *)ckalloc(cm_strlen(str1) + 4);
				what_str = "\0";
			}
			sprintf(buf, "%s  %s", str1, what_str);
			str = XmStringCreateLocalized(buf);
			XtVaSetValues(step->view_item_toggle,
				XmNlabelString, str,
				XmNset, (appt->state->value->item.sint32_value ==
					CSA_STATUS_COMPLETED) ?
						True : False,
				NULL);

			XmStringFree(str);
			free(buf);
			destroy_lines(lines);
#ifdef API
			if (cnt < entry_count)
				DtCmFreeAttributeValues(appt->attrs,
							appt->count);
#endif
		}

		last = step;
		step = step->next;
	}
	free_appt_struct(&appt);

	/*
	**  If step still points to a value, we've got extra items in the list,
	**  possibly filled with old stuff -- unmanage these widgets.
	*/
	while (step) {
		if (step->appt != 0)
			csa_free((CSA_buffer) step->appt);
		XtUnmanageChild(step->view_item_number);
		XtUnmanageChild(step->view_item_toggle);
		step = step->next;
	}

	if (entry_count > 0) {
		XtManageChild(t->view_form); 
		XtManageChild(t->view_sw_mgr);
		XtManageChild(t->view_form_mgr); 
	}

	t->view_list_modified = False;

	return entry_count;
}

extern void
todo_clean_up(ToDo *t) {
	if (t->todo_head && t->todo_count > 0)
		csa_free((CSA_buffer) t->todo_head);
	t->todo_head = NULL;
	t->todo_count = 0;
}

extern Boolean
todo_showing(ToDo *t) {
	if (t)
		return t->todo_is_up;
	return False;
}

extern Boolean
todo_view_showing(ToDo *t) {
	if (t)
		return t->todo_view_is_up;
	return False;
}

/*
**  External function to set todo defaults
*/
extern void
set_todo_defaults(ToDo *t) {
       /* ToDo structure may not be valid */
        if (!t || !t->cal) return;
	set_dssw_defaults(&t->dssw, t->cal->view->date, True);

	if (t->reminders.bfpm_form_mgr) {
		set_rfp_defaults(&t->rfp);
		set_reminders_defaults(&t->reminders);
	}
	XmToggleButtonGadgetSetState(t->completed_toggle, False, False);
	set_message(t->message_text, " ");

	t->rfpFlags = 0;
}
extern void
set_todo_title(ToDo *t, char *name) {
        Calendar 	*c = t->cal;
	char		buf[MAXNAMELEN];

	if (t->frame) {
		sprintf(buf, "%s - %s", catgets(c->DT_catd, 1, 591, "Calendar : To Do Editor"), name);
		XtVaSetValues(t->frame, XmNtitle, buf,
			NULL);
	}
}

extern void
show_todo(Calendar *c) {
	int		dv = get_data_version(c->cal_handle);
	ToDo		*t = (ToDo *)c->todo;
	Props_pu	*p = (Props_pu *)c->properties_pu;

	if (dv <= CMS_VERS_2) {
		backend_err_msg(c->frame, c->view->current_calendar,
			CSA_E_NOT_SUPPORTED, p->xm_error_pixmap);
		return;
	}

	if (!t->frame)
		t_make_todo(c);
	if (!todo_showing(t)) {
		ds_position_popup(c->frame, t->frame, DS_POPUP_LOR);
        	/* set default button */
        	XmProcessTraversal(t->dssw.what_text, XmTRAVERSE_CURRENT);
        	XtVaSetValues(t->base_form_mgr, 
			XmNinitialFocus, t->dssw.what_text, 
			NULL);
	}

	XtPopup(t->frame, XtGrabNone);
	if (! t->todo_is_up)
	{
	    t->todo_is_up = True;
	    set_todo_defaults(t);
	}
	add_all_todo(t);
}

extern void
show_todo_view(Calendar *c, todo_view_op view_filter) {
	int		dv = get_data_version(c->cal_handle);
	ToDo		*t = (ToDo *)c->todo;
	Props_pu	*p = (Props_pu *)c->properties_pu;

	if (dv <= CMS_VERS_2) {
		backend_err_msg(c->frame, c->view->current_calendar,
			CSA_E_NOT_SUPPORTED, p->xm_error_pixmap);
		return;
	}

	if (!t->view_frame) {
		t->cal = c;
		t_build_view_popup(t);
	}


        XtVaSetValues(t->view_form, XmNdefaultButton, t->view_apply_button, NULL);
        XtVaSetValues(t->view_form, XmNcancelButton, t->view_cancel_button, NULL);


	build_todo_view(t, c->view->glance, False);
	if (!todo_view_showing(t))
		ds_position_popup(c->frame, t->view_frame,
				  DS_POPUP_LOR);
	XtVaSetValues(t->view_frame, XmNmappedWhenManaged, True,
		NULL);
	XtPopup(t->view_frame, XtGrabNone);
	XtManageChild(t->view_form);
	t->todo_view_is_up = True;
}
/*
**  Function to set todo defaults
*/
static void
set_todo_msg_defaults(ToDo *t) {
	set_dssw_defaults(&t->dssw, t->cal->view->date, False);
	set_message(t->message_text, " ");
}
/*
 * This todo_insert does not free the appt struct as
 * the t_insert code does.
 */
extern Boolean
todo_insert(Dtcm_appointment *appt, CSA_entry_handle *new_a, Calendar *c) {
	CSA_return_code	stat;
	ToDo		*t = (ToDo *)c->todo;
	Props_pu	*p = (Props_pu *)c->properties_pu;
	CSA_enum		scope;
	static int		answer=0;

	/* the gui does not support specifying the sequence end date */
	if (appt->sequence_end_date && appt->sequence_end_date->name) {
		free(appt->sequence_end_date->name);
		appt->sequence_end_date->name = NULL;
	}
        if ((appt->repeat_type->value) &&
           (appt->repeat_type->value->item.sint32_value != CSA_X_DT_REPEAT_ONETIME))
        {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 1102,
					"Insert To Do"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 1103,
					"This To Do is part of a repeating series.\nDo you want to insert the item ...?"));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
	  	char *ident4 = XtNewString(catgets(c->DT_catd, 1, 272, "All"));
			answer = dialog_popup(c->frame,
				DIALOG_TITLE, title,
				DIALOG_TEXT, text,
				BUTTON_IDENT, 1, ident1,
				BUTTON_IDENT, 4, ident4,
				DIALOG_IMAGE, p->xm_question_pixmap,
				NULL);
		XtFree(ident4);
		XtFree(ident4);
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
		_DtTurnOffHourGlass(t->frame);
		return 0;
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

	if (stat != CSA_SUCCESS) {
		_DtTurnOffHourGlass(t->frame);
		return 0;
	}
	csa_free((CSA_buffer) new_a);

	set_todo_msg_defaults(t);
	add_all_todo(t);

	if (todo_view_showing(t))
		build_todo_view(t, dayGlance, False);

	reset_alarm(c);
	invalidate_cache(c);


	return True;
}
