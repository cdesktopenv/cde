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
/* $TOG: group_editor.c /main/10 1999/02/23 09:42:27 mgreess $ */
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
#include <Xm/SeparatoG.h>
#include <Xm/ToggleBG.h>
#include <Xm/Text.h>
#include <Xm/LabelG.h>
#include <Xm/DragDrop.h>
#include <Dt/Dt.h>
#include <Dt/Dnd.h>
#include <Dt/HourGlass.h>
#include <Dt/Icon.h>
#include <Tt/tttk.h>
#include "group_editor.h"
#include "editor.h"
#include "calendar.h"
#include "browser.h"
#include "blist.h"
#include "datefield.h"
#include "deskset.h"
#include "getdate.h"
#include "format.h"
#include "timeops.h"
#include "props.h"
#include "props_pu.h"
#include "util.h"
#include "dnd.h"
#include "help.h"
#include "cm_tty.h"
#ifdef SVR4
#include <sys/param.h>
#endif /* SVR4 */

#define ACCESS_NAME_LEN 25
extern	int _csa_duration_to_iso8601(int, char *);
extern void scrub_attr_list(Dtcm_appointment *);
extern boolean_t compare_repeat_info(Dtcm_appointment *, RFP *, CSA_session_handle, int);
extern void change_rule_for_this_one_only(Calendar *, Dtcm_appointment *, Dtcm_appointment *);
static void ge_build_expand(GEditor *);


/* Absolute value macro */
#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#endif

static void
ge_mail_proc(Widget w, XtPointer client_data, XtPointer data);
static Dtcm_appointment*
form_to_appt(GEditor *ge, Boolean no_reminders, int version);

extern int debug;

/*******************************************************************************
**
**  Functions static to group_editor.c
**
*******************************************************************************/
static void
ge_add_to_gappt_list(Access_data *ad, int idx, GEditor *ge, Boolean reset) {
	int			cnt = 1;
	char			buf[DEFAULT_GAPPT_LEN], *name1, *name2 = NULL;
	Props			*p = (Props *)ge->cal->properties;
	XmString		str;
	List_data		*step = NULL, *last = NULL;
	CSA_return_code		stat;
	DisplayType		dt = get_int_prop(p, CP_DEFAULTDISP);
	Dtcm_appointment	*appt;
	Tick			start_tick;

	/*
	**  If the reset flag is true, the items have been deleted from the
	**  scrolling list, so clear our internal buffer - otherwise prime the
	**  linked list position pointer to find the correct entry point for
	**  this appointment.
	*/
	if (reset)
		CmDataListDeleteAll(ge->list_data, True);
	else
		step = (List_data *)CmDataListGetData(ge->list_data, cnt);

	/*
	**  Get the necessary entry attributes
	*/
	appt = allocate_appt_struct(appt_read,
				    ad->version,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_SUMMARY_I,
				    CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
				    NULL);
	stat = query_appt_struct(ge->cal->cal_handle, ad->appt_head[idx], appt);
	backend_err_msg(ge->frame, ge->cal->view->current_calendar, stat,
			((Props_pu *)ge->cal->properties_pu)->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return;
	}

	/*
	**  Find the correct position for this appointment in the scrolling
	**  list (mirrored by our internal linked list) and add the necessary
	**  stuff to our internal list.
	*/
	_csa_iso8601_to_tick(appt->time->value->item.date_time_value, &start_tick);
	while (step && start_tick >= step->tick)
		step = (List_data *)CmDataListGetData(ge->list_data, ++cnt);

	step = (List_data *)ckalloc(sizeof(List_data));
	step->entry_idx = idx;
	step->tick = start_tick;
	step->ad = ad;
	CmDataListAdd(ge->list_data, (void *)step, cnt);

	/*
	**  Now determine the formatting for the appointment - if it has the
	**  same time as the last appt, leave off the time, and if it has the
	**  same owner as the last one, leave the the owner.
	*/
	step = (cnt <= 1) ?
		NULL : (List_data *)CmDataListGetData(ge->list_data, cnt - 1);
	name1 = cm_target2name(ad->name);
	if (step && step->ad)
		name2 = cm_target2name(step->ad->name);
	if (!step || start_tick != step->tick || reset)
		format_gappt(appt, ad->name, buf, dt, DEFAULT_GAPPT_LEN);
	else {


		/* Potentially nasty memory bug here.  The assumption is 
		   that the buffer supplied for the time format is the 
		   right size for the new format. */

		_csa_tick_to_iso8601(0, appt->time->value->item.date_time_value);
		if (!step || strcmp(ad->name, name2) != 0)
			format_gappt(appt, ad->name, buf, dt, DEFAULT_GAPPT_LEN);
		else
			format_gappt(appt, NULL, buf, dt, DEFAULT_GAPPT_LEN);
	}

	/*
	**  Create the string, add it to the list, and clean up
	*/
	str = XmStringCreateLocalized(buf);
	XmListAddItem(ge->appt_list, str, cnt);
	if (name1)
		free(name1);
	if (name2)
		free(name2);
	XmStringFree(str);
	free_appt_struct(&appt);
}

/*
**  This function will take appointment values and stuff them into a form.
*/
static void
appt_to_form(GEditor *ge, CSA_entry_handle a, char *name, int version) {
	char			buf[MAXNAMELEN];
	Props_pu		*pu = (Props_pu *)ge->cal->properties_pu;
	CSA_return_code		stat;
	Dtcm_appointment	*appt;

	if (!ge->rfp.rfp_form_mgr) {
		ge_build_expand(ge);
		set_rfp_defaults(&ge->rfp);
	}

	if (!dssw_appt_to_form(&ge->dssw, a))
		return;
	ge->dsswFlags = 0;
	if (!rfp_appt_to_form(&ge->rfp, a))
		return;
	ge->rfpFlags = 0;

	appt = allocate_appt_struct(appt_read, 
				        version,
					CSA_ENTRY_ATTR_ORGANIZER_I, 
					NULL);
	stat = query_appt_struct(ge->cal->cal_handle, a, appt);
	backend_err_msg(ge->frame, name, stat, pu->xm_error_pixmap);
	if (stat == CSA_SUCCESS) {
		sprintf(buf, "%s:  %s",
			catgets(ge->cal->DT_catd, 1, 300, "Author"),
			appt->author->value->item.calendar_user_value->user_name);
		set_message(ge->message_text, buf);
	} else
		set_message(ge->message_text, " ");

	free_appt_struct(&appt);
}

/*
**  List selection procedure will get the correct appointment, then call
**  appt_to_form to load it into the UI.
*/
static void
ge_list_select_proc(Widget w, XtPointer client_data, XtPointer data) {
	int			cnt;
	GEditor			*ge = (GEditor *)client_data;
	CSA_entry_handle	a;
	List_data		*ld;
	Access_data		*ad, *step_ad;
	XmListCallbackStruct	*cbs = (XmListCallbackStruct *)data;

	if (a = geditor_nth_appt(ge, cbs->item_position, &ad))
		appt_to_form(ge, a, (ad && ad->name) ? ad->name : "\0", ad->version);
	XmListDeselectAllItems(ge->access_list);
	if (!ad || !ad->name)
		return;

	cnt = 1;
	step_ad = (Access_data *)CmDataListGetData(ge->access_data, cnt);
	while (step_ad && strcmp(step_ad->name, ad->name) != 0)
		step_ad = (Access_data *)CmDataListGetData(ge->access_data,
							   ++cnt);
	if (step_ad)
		XmListSelectPos(ge->access_list, cnt, True);

	XtVaSetValues(ge->change_button, XmNsensitive, True, NULL);
	XtVaSetValues(ge->delete_button, XmNsensitive, True, NULL);
}

static void
ge_set_modify_buttons(GEditor *ge, int cnt) {
	char	buf[MAXNAMELEN];
	Boolean	val;

	if (cnt == 1)
		sprintf(buf, "%d %s.", cnt,
			catgets(ge->cal->DT_catd, 1, 633, "Calendar Selected"));
	else
		sprintf(buf, "%d %s.", cnt,
			catgets(ge->cal->DT_catd, 1, 634, "Calendars Selected"));
	set_message(ge->message_text, buf);

}

/*
**  List selection procedure will ensure that when more than one calendar name
**  is selected the change and delete buttons will be greyed.
*/
static void
ge_access_select_proc(Widget w, XtPointer client_data, XtPointer data) {
	GEditor			*ge = (GEditor *)client_data;
	XmListCallbackStruct	*cbs = (XmListCallbackStruct *)data;

	ge_set_modify_buttons(ge, cbs->selected_item_count);
}

static Tt_message
reply_cb(Tt_message m, void *c_data, Tttk_op op, unsigned char *contents, int len, char *file)
{
        char *client_procID = tt_message_handler(m);
        if ( debug && (client_procID != NULL) ) {
                fprintf(stderr, "DEBUG: reply_cb():client_procID = %s\n", client_procID);
                fprintf(stderr, "DEBUG: reply_cb():message_op = %s\n", tt_message_op(m));
        }
        return(m);
}

static char *
get_mail_address_list(Calendar *c) {
	int		i, *pos_list = NULL, pos_cnt;
        GEditor		*ge = (GEditor *)c->geditor;
	Access_data	*ad;
	int		address_len;
	char		*address;


	XmListGetSelectedPos(ge->access_list, &pos_list, &pos_cnt);
	for (i = 0, address_len = 0; i < pos_cnt; i++) {
		ad = (Access_data *)CmDataListGetData(ge->access_data,
						    pos_list[i]);
		if (ad)
			address_len += strlen(ad->name) + 1;
	}

	address = calloc(address_len+1, 1);
	memset(address, 0, address_len);

	for (i = 0; i < pos_cnt; i++) {
		ad = (Access_data *)CmDataListGetData(ge->access_data,
						    pos_list[i]);
		if (ad) {
			strcat(address, ad->name);
			strcat(address, " ");
		}
	}
	if (pos_list)
		XtFree((XtPointer)pos_list);

	return(address);
}

/*
**  Callback from the Mail... button will popup the compose window
*/
static void
ge_mail_proc(Widget w, XtPointer client_data, XtPointer data) {
	GEditor		*ge = (GEditor *)client_data;
	Calendar	*c = ge->cal;
	Props_pu	*p = (Props_pu *)ge->cal->properties_pu;
        Tt_message      msg;
        Tt_status       status;
        char            *appointment_buf;
        char            *mime_buf;
	Dtcm_appointment        *appt;
	char		*address = get_mail_address_list(c);
	char		*address_list[1];
 
        /* Send ToolTalk message to bring up compose GUI with buffer as attachme
nt */

	appt = form_to_appt(ge, False, DATAVER4);
 
        appointment_buf = parse_attrs_to_string(appt, (Props *) c->properties, attrs_to_string(appt->attrs, appt->count));


	free_appt_struct(&appt);

	address_list[0] = appointment_buf;

	mime_buf = create_rfc_message(address, "message", address_list, 1);
 
        msg = ttmedia_load(0, (Ttmedia_load_msg_cb)reply_cb, NULL, TTME_MAIL_EDIT, "RFC_822_MESSAGE", (unsigned char *)mime_buf, strlen(mime_buf), NULL, "dtcm_appointment_attachment", 0);
 
	status = tt_ptr_error(msg);
	if (tt_is_err(status))
	{
	    	fprintf(stderr, "dtcm: ttmedia_load: %s\n",
			tt_status_message(status));
	}
	else
	{
            status = tt_message_send(msg);
	    if (tt_is_err(status))
		fprintf(stderr, "dtcm: tt_message_send: %s\n",
			tt_status_message(status));
	}

	free(appointment_buf);
	free(mime_buf);
	free(address);
}

/*
**  This function creates the expando stuff on a form manager.
*/
static void
ge_build_expand(GEditor *ge) {
	Props		*p = (Props *)ge->cal->properties;
	XmString	xmstr;
	Calendar	*c = ge->cal;
	Widget		widgets[20];
	WidgetList	children;
	int		i = 0,
			j = 0,
			n;
	/*
	**  Build the rfp "widget"
	*/
	ge->rfpFlags = 0;
	build_rfp(&ge->rfp, ge->cal, ge->base_form_mgr);
	XtVaSetValues(ge->rfp.rfp_form_mgr,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, ge->separator2,
		XmNbottomOffset, 5,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, 5,
		NULL);
        XtVaGetValues(ge->rfp.rfp_form_mgr,
                XmNchildren,            &children,
                XmNnumChildren,         &n,
                NULL);

        /* We don't want to manage the privacy widgets */
        for (i = 0; i < n; i++) {
                if ((children[i] == ge->rfp.privacy_label) ||
                    (children[i] == ge->rfp.privacy_menu))
                    continue;
                widgets[j++] = children[i];
        }
        XtManageChildren(widgets, n - 2);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 304, "Mail..."));
        ge->mail_button = XtVaCreateWidget("mail",
		xmPushButtonWidgetClass, ge->base_form_mgr,
		XmNlabelString, xmstr,
		XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNbottomWidget, ge->rfp.rfp_form_mgr,
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, 5,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(ge->mail_button, XmNactivateCallback, ge_mail_proc, ge);

	XtSetSensitive(ge->mail_button, c->tt_procid == NULL ? False : True);
}

/*
**  Button action procedures manage and unmanage the "extra" stuff in the UI to
**  make it visible and invisible to the user.
*/
static void
ge_expand_ui_proc(Widget w, XtPointer client_data, XtPointer data) {
	GEditor		*ge = (GEditor *)client_data;
	XmString	xmstr;
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

	XtVaGetValues(ge->appt_list_sw, XmNheight, &height, NULL);
	XtVaGetValues(ge->access_list_sw, XmNwidth, &width, NULL);

	if (expand_state_closed) {
		Widget children[2];

		if (!ge->rfp.rfp_form_mgr) {
			ge_build_expand(ge);
			set_rfp_defaults(&ge->rfp);
		}

		XtSetSensitive(ge->rfp.rfp_form_mgr, False);
		XtRealizeWidget(ge->rfp.rfp_form_mgr);

		xmstr = XmStringCreateLocalized(
				catgets(ge->cal->DT_catd, 1, 625, "Less"));
		XtVaSetValues(ge->expand_ui_button, 
			XmNlabelString, xmstr,
			NULL);
		XmStringFree(xmstr);

		XtVaGetValues(ge->rfp.rfp_form_mgr, XmNheight, &h, NULL);
		XtVaSetValues(ge->separator1, XmNbottomOffset, h + 10, NULL);
		
		children[0] = ge->rfp.rfp_form_mgr;
		children[1] = ge->mail_button;

		XtManageChildren(children, 2);

		expand_state_closed = False;
	} else {
		XtSetSensitive(ge->rfp.rfp_form_mgr, False);
		xmstr = XmStringCreateLocalized(catgets(ge->cal->DT_catd, 1, 626,
							"More"));
		XtVaSetValues(ge->expand_ui_button, XmNlabelString, xmstr,
			NULL);
		XmStringFree(xmstr);
		XtVaSetValues(ge->separator1, XmNbottomOffset, 0, NULL);
		XtUnmanageChild(ge->rfp.rfp_form_mgr);
		XtUnmanageChild(ge->mail_button);
		expand_state_closed = True;
	}
	XtVaSetValues(ge->appt_list_sw, XmNheight, height, NULL);
	XtVaSetValues(ge->access_list_sw, XmNwidth, width, NULL);
	XtSetSensitive(ge->rfp.rfp_form_mgr, True);
}

static Tick
ge_reminder_val(Props *p, Props_op scope, Props_op unit) {
	int	i_val = get_int_prop(p, unit);
	char	*s_val = get_char_prop(p, scope);

	switch (convert_time_scope_str(s_val)) {
	case TIME_DAYS:
		return days_to_seconds(i_val);
	case TIME_HRS:
		return hours_to_seconds(i_val);
	case TIME_MINS:
	default:
		return minutes_to_seconds(i_val);
	}
}

/*
**  This function will consume form values and stuff them into an appointment.
*/
static Dtcm_appointment*
form_to_appt(GEditor *ge, Boolean no_reminders, int version) {
	Props			*p = (Props *)ge->cal->properties;
	boolean_t		all_ok;
	Dtcm_appointment	*a;

	if (no_reminders) {

		if (version <= DATAVER2)
		a = allocate_appt_struct(appt_write,
				         version,
					 CSA_ENTRY_ATTR_START_DATE_I,
					 CSA_ENTRY_ATTR_TYPE_I,
					 CSA_ENTRY_ATTR_SUBTYPE_I,
					 CSA_ENTRY_ATTR_CLASSIFICATION_I,
					 CSA_ENTRY_ATTR_END_DATE_I,
					 CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
					 CSA_ENTRY_ATTR_SUMMARY_I,
					 CSA_ENTRY_ATTR_STATUS_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
					 NULL);
		else if (version == DATAVER3)
		a = allocate_appt_struct(appt_write,
				         version,
					 CSA_ENTRY_ATTR_START_DATE_I,
					 CSA_ENTRY_ATTR_TYPE_I,
					 CSA_ENTRY_ATTR_SUBTYPE_I,
					 CSA_ENTRY_ATTR_CLASSIFICATION_I,
					 CSA_ENTRY_ATTR_END_DATE_I,
					 CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
					 CSA_ENTRY_ATTR_SUMMARY_I,
					 CSA_ENTRY_ATTR_STATUS_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I,
					 CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I,
					 NULL);
		else if (version == DATAVER4)
		a = allocate_appt_struct(appt_write,
				         version,
					 CSA_ENTRY_ATTR_START_DATE_I,
					 CSA_ENTRY_ATTR_TYPE_I,
					 CSA_ENTRY_ATTR_SUBTYPE_I,
					 CSA_ENTRY_ATTR_CLASSIFICATION_I,
					 CSA_ENTRY_ATTR_END_DATE_I,
					 CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
					 CSA_ENTRY_ATTR_SUMMARY_I,
					 CSA_ENTRY_ATTR_STATUS_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I,
					 CSA_ENTRY_ATTR_RECURRENCE_RULE_I,
					 NULL);
	}
	else {

		if (version <= DATAVER2)
		a = allocate_appt_struct(appt_write,
				         version,
					 CSA_ENTRY_ATTR_START_DATE_I,
					 CSA_ENTRY_ATTR_TYPE_I,
					 CSA_ENTRY_ATTR_SUBTYPE_I,
					 CSA_ENTRY_ATTR_CLASSIFICATION_I,
					 CSA_ENTRY_ATTR_END_DATE_I,
					 CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
					 CSA_ENTRY_ATTR_SUMMARY_I,
					 CSA_ENTRY_ATTR_STATUS_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
					 CSA_ENTRY_ATTR_AUDIO_REMINDER_I,
					 CSA_ENTRY_ATTR_FLASHING_REMINDER_I,
					 CSA_ENTRY_ATTR_MAIL_REMINDER_I,
					 CSA_ENTRY_ATTR_POPUP_REMINDER_I,
					 NULL);
		else if (version == DATAVER3)
		a = allocate_appt_struct(appt_write,
				         version,
					 CSA_ENTRY_ATTR_START_DATE_I,
					 CSA_ENTRY_ATTR_TYPE_I,
					 CSA_ENTRY_ATTR_SUBTYPE_I,
					 CSA_ENTRY_ATTR_CLASSIFICATION_I,
					 CSA_ENTRY_ATTR_END_DATE_I,
					 CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
					 CSA_ENTRY_ATTR_SUMMARY_I,
					 CSA_ENTRY_ATTR_STATUS_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I,
					 CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I,
					 CSA_X_DT_ENTRY_ATTR_SEQUENCE_END_DATE_I,
					 CSA_ENTRY_ATTR_AUDIO_REMINDER_I,
					 CSA_ENTRY_ATTR_FLASHING_REMINDER_I,
					 CSA_ENTRY_ATTR_MAIL_REMINDER_I,
					 CSA_ENTRY_ATTR_POPUP_REMINDER_I,
					 NULL);
		else if (version == DATAVER4)
		a = allocate_appt_struct(appt_write,
				         version,
					 CSA_ENTRY_ATTR_START_DATE_I,
					 CSA_ENTRY_ATTR_TYPE_I,
					 CSA_ENTRY_ATTR_SUBTYPE_I,
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

		a->beep->value->item.reminder_value->lead_time = malloc(BUFSIZ);
		_csa_duration_to_iso8601(ge_reminder_val(p, CP_BEEPUNIT, CP_BEEPADV), 
				     a->beep->value->item.reminder_value->lead_time);
		a->beep->value->item.reminder_value->reminder_data.data = NULL;
		a->beep->value->item.reminder_value->reminder_data.size = 0;

		a->flash->value->item.reminder_value->lead_time = malloc(BUFSIZ);
		_csa_duration_to_iso8601(ge_reminder_val(p, CP_FLASHUNIT, CP_BEEPADV), 
				     a->flash->value->item.reminder_value->lead_time);
		a->flash->value->item.reminder_value->reminder_data.data = NULL;
		a->flash->value->item.reminder_value->reminder_data.size = 0;

		a->popup->value->item.reminder_value->lead_time = malloc(BUFSIZ);
		_csa_duration_to_iso8601(ge_reminder_val(p, CP_OPENUNIT, CP_BEEPADV), 
				     a->popup->value->item.reminder_value->lead_time);
		a->popup->value->item.reminder_value->reminder_data.data = NULL;
		a->popup->value->item.reminder_value->reminder_data.size = 0;

		a->mail->value->item.reminder_value->lead_time = malloc(BUFSIZ);
		_csa_duration_to_iso8601(ge_reminder_val(p, CP_MAILUNIT, CP_BEEPADV), 
				     a->mail->value->item.reminder_value->lead_time);
		a->mail->value->item.reminder_value->reminder_data.data =
			(CSA_uint8 *) cm_strdup(get_char_prop(p, CP_MAILTO));
		a->mail->value->item.reminder_value->reminder_data.size =
			strlen(get_char_prop(p, CP_MAILTO)) + 1;
	}

	all_ok = dssw_form_to_appt(&ge->dssw, a, ge->cal->view->current_calendar,
				   ge->cal->view->date);
	if (all_ok)
		all_ok = rfp_form_to_appt(&ge->rfp, a,
					  ge->cal->view->current_calendar);

	if (!all_ok) {
		free_appt_struct(&a);
		return NULL;
	}

	a->type->value->item.sint32_value = CSA_TYPE_EVENT;
	a->subtype->value->item.string_value = strdup(CSA_SUBTYPE_APPOINTMENT);
	a->state->value->item.sint32_value = CSA_X_DT_STATUS_ACTIVE;
	a->private->value->item.sint32_value = CSA_CLASS_PUBLIC;

	return(a);
}

/*
**  Action procedures
*/
static void
ge_insert_proc(Widget w, XtPointer client_data, XtPointer data) {
	int			i, c_cnt, *c_list;
	GEditor			*ge = (GEditor *)client_data;
	Props_pu		*p = (Props_pu *)ge->cal->properties_pu;
	Calendar		*c = ge->cal;
	CSA_entry_handle	new_a;
	CSA_return_code		stat;
	Access_data		*ad;
	Dtcm_appointment	*appt;
	XmListCallbackStruct	*lcb = (XmListCallbackStruct *)data;

	_DtTurnOnHourGlass(ge->frame);
	XmListGetSelectedPos(ge->access_list, &c_list, &c_cnt);
	if (c_cnt <= 0) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 305,
				"Calendar : Error - Group Appointment Editor"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 306,
				"Select a calendar and INSERT again."));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		dialog_popup(ge->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		_DtTurnOffHourGlass(ge->frame);
		return;
	} 


	for(i = 0; i < c_cnt; i++) {
		ad = (Access_data *)
			CmDataListGetData(ge->access_data, c_list[i]);


		/* it may seem a little odd to create this
		   template appointment over and over again, but this 
		   needs to be done because each connection may support 
		   a different data model, and thus need the appointments 
		   created differently. */

		if (same_user(ad->name, c->calname)) 
			appt = form_to_appt(ge, False, ad->version);
		else
	    		appt = form_to_appt(ge, True, ad->version);

		if (appt == NULL) {
			_DtTurnOffHourGlass(ge->frame);
			return;
		}

		if (!ad)
			stat = CSA_E_CALENDAR_NOT_EXIST;
		else {

			scrub_attr_list(appt);

			/* the gui does not support specifying the sequence
			 * end date
			 */
			if (appt->sequence_end_date &&
			    appt->sequence_end_date->name) {
				free(appt->sequence_end_date->name);
				appt->sequence_end_date->name = NULL;
			}

			stat = csa_add_entry(ad->cal_handle,
					       appt->count, 
					       appt->attrs,
					       &new_a, NULL);
		}
		backend_err_msg(ge->frame, ad->name, stat,
				p->xm_error_pixmap);
		if (stat != CSA_SUCCESS) {
			XtFree((XtPointer)c_list);
			free_appt_struct(&appt);
			add_all_gappt(ge);
			if (editor_showing((Editor *)c->editor))
               			add_all_appt((Editor*)c->editor);
			_DtTurnOffHourGlass(ge->frame);
			return;
		}
		csa_free((CSA_buffer)new_a);
		free_appt_struct(&appt);
	}
	XtFree((XtPointer)c_list);

	add_all_gappt(ge);
	if (editor_showing((Editor *)c->editor))
       		add_all_appt((Editor*)c->editor);

	reset_alarm(ge->cal);
	invalidate_cache(ge->cal);
	paint_canvas(ge->cal, NULL, RENDER_CLEAR_FIRST);
	br_display(ge->cal);
	_DtTurnOffHourGlass(ge->frame);
}

static void
ge_delete_proc(Widget w, XtPointer client_data, XtPointer data) {
	int			*item_list = NULL, item_cnt = 0, c_cnt, i;
	GEditor			*ge = (GEditor *)client_data;
	Calendar		*c = ge->cal;
	Props_pu		*p = (Props_pu *)ge->cal->properties_pu;
	CSA_entry_handle	entry;
	CSA_return_code		stat;
	Access_data		*ad;
	CSA_enum		scope;
	Dtcm_appointment	*appt;
	static int		answer;

	_DtTurnOnHourGlass(ge->frame);
	if (!XmListGetSelectedPos(ge->appt_list, &item_list, &item_cnt)) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 305,
				"Calendar : Error - Group Appointment Editor"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 253,
				"Select an appointment and DELETE again."));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(ge->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		_DtTurnOffHourGlass(ge->frame);
		return;
	}

	if (!(entry = geditor_nth_appt(ge, item_list[0], &ad))) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 305,
				"Calendar : Error - Group Appointment Editor"));
	  	char *text = XtNewString(catgets(c->DT_catd, 1, 256,
				"Internal error selecting appointment.\nAppointment was not deleted."));
	  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95,
				"Continue"));
		answer = dialog_popup(ge->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			BUTTON_HELP, RESELECT_ERROR_HELP,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		XtFree((XtPointer)item_list);
		_DtTurnOffHourGlass(ge->frame);
		return;
	}
	XtFree((XtPointer)item_list);

	XtVaGetValues(ge->access_list, XmNselectedItemCount, &c_cnt,
		NULL);
	if (c_cnt <= 0) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 305,
				"Calendar : Error - Group Appointment Editor"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 315,
				"Select a calendar and DELETE again."));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(ge->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		_DtTurnOffHourGlass(ge->frame);
		return;
	} 

	answer = 0;
	appt = allocate_appt_struct(appt_read, 
				        ad->version,
					CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I, 
					NULL);
	stat = query_appt_struct(c->cal_handle, entry, appt);
	backend_err_msg(ge->frame, c->view->current_calendar, stat,
			p->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		_DtTurnOffHourGlass(ge->frame);
		return;
	}

	if (appt->repeat_type->value) {
		if (appt->repeat_type->value->item.sint32_value !=
		    CSA_X_DT_REPEAT_ONETIME) {
		  	char *title = XtNewString(catgets(c->DT_catd, 1, 314,
					"Calendar : Group Appointment Editor - Delete"));
		  	char *text = XtNewString(catgets(c->DT_catd, 1, 274,
					"This appointment is part of a repeating series.\nDo you want to delete ...?"));
		  	char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
		  	char *ident2 = XtNewString(catgets(c->DT_catd, 1, 270,
					"This One Only"));
		  	char *ident3 = XtNewString(catgets(c->DT_catd, 1, 271, "Forward"));
		  	char *ident4 = XtNewString(catgets(c->DT_catd, 1, 272, "All"));
			answer = dialog_popup(ge->frame,
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
		char *title = XtNewString(catgets(c->DT_catd, 1, 314,
                                        "Calendar : Group Appointment Editor - Delete"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 711,
                                        "This appointment repeats in an unknown fashion.  All occurrences will be deleted\nDo you still wish to delete it?"));
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
		_DtTurnOffHourGlass(ge->frame);
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

	stat = csa_delete_entry(c->cal_handle, entry, scope, NULL);
	backend_err_msg(ge->frame, c->view->current_calendar, stat,
			p->xm_error_pixmap);

	add_all_gappt(ge);
	reset_alarm(ge->cal);
	invalidate_cache(ge->cal);
	paint_canvas(ge->cal, NULL, RENDER_CLEAR_FIRST);
	br_display(ge->cal);
	_DtTurnOffHourGlass(ge->frame);
}

static void
ge_change_proc(Widget w, XtPointer client_data, XtPointer data) {
	int			*item_list = NULL, item_cnt = 0, c_cnt, i;
	GEditor			*ge = (GEditor *)client_data;
	Calendar		*c = ge->cal;
	Props_pu		*p = (Props_pu *)c->properties_pu;
	CSA_entry_handle	old_a, new_entry;
	CSA_return_code		stat;
	Access_data		*ad;
	CSA_enum		scope;
	Dtcm_appointment	*new_a = NULL, *appt;
	static int		answer;

	_DtTurnOnHourGlass(ge->frame);
	if (!XmListGetSelectedPos(ge->appt_list, &item_list, &item_cnt)) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 305,
				"Calendar : Error - Group Appointment Editor"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 259,
				"Select an appointment and CHANGE again."));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(ge->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		_DtTurnOffHourGlass(ge->frame);
		return;
	} 

	if (!(old_a = geditor_nth_appt(ge, item_list[0], &ad))) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 305,
				"Calendar : Error - Group Appointment Editor"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 1007,
				"Internal error selecting appointment.\nAppointment was not changed."));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(ge->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		XtFree((XtPointer)item_list);
		_DtTurnOffHourGlass(ge->frame);
		return;
	}
	XtFree((XtPointer)item_list);

	XtVaGetValues(ge->access_list, XmNselectedItemCount, &c_cnt,
		NULL);
	if (c_cnt <= 0) {
		char *title = XtNewString(catgets(c->DT_catd, 1, 329,
				"Calendar : Group Appointment Editor - Change"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 330,
				"Select a calendar and CHANGE again."));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		answer = dialog_popup(ge->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		_DtTurnOffHourGlass(ge->frame);
		XtFree(ident1);
		XtFree(text);
		XtFree(title);
		return;
	} 

	answer = 0;
	appt = allocate_appt_struct(appt_read,
				    ad->version,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_TYPE_I,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_TIMES_I,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_INTERVAL_I,
				    CSA_X_DT_ENTRY_ATTR_REPEAT_OCCURRENCE_NUM_I,
				    CSA_ENTRY_ATTR_REFERENCE_IDENTIFIER_I,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_RECURRENCE_RULE_I,
				    NULL);
	stat = query_appt_struct(c->cal_handle, old_a, appt);
	backend_err_msg(ge->frame, ad->name, stat, p->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		_DtTurnOffHourGlass(ge->frame);
		return;
	}

	if ((new_a = form_to_appt(ge, (same_user(ad->name, c->calname)) ? False : True, ad->version)) == NULL) {
		free_appt_struct(&appt);
		_DtTurnOffHourGlass(ge->frame);
		return;
	}

        if ((appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER) ||
            (appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER_WEEKLY) ||
            (appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER_MONTHLY) ||
            (appt->repeat_type->value->item.sint32_value == CSA_X_DT_REPEAT_OTHER_YEARLY)) {
	  	char *title = XtNewString(catgets(c->DT_catd, 1, 329,
                                        "Calendar : Group Appointment Editor - Change"));
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
		char *title = XtNewString(catgets(c->DT_catd, 1, 329,
				"Calendar : Group Appointment Editor - Change"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 268,
				"This appointment is part of a repeating series.\nDo you want to change ...?"));
		char *ident1 = XtNewString(catgets(c->DT_catd, 1, 923, "Cancel"));
		char *ident2 = XtNewString(catgets(c->DT_catd, 1, 270,
				"This One Only"));
		char *ident3 = XtNewString(catgets(c->DT_catd, 1, 271, "Forward"));
		char *ident4 = XtNewString(catgets(c->DT_catd, 1, 272, "All"));
		answer = dialog_popup(ge->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident1,
			BUTTON_IDENT, 2, ident2,
			BUTTON_IDENT, 3, ident3,
                        (compare_repeat_info(appt, &(ge->rfp), ad->cal_handle, ad->version) ?
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
		if (answer == 2) {
			change_rule_for_this_one_only(c, new_a, appt);
		}
	}

	switch(answer) {
	case 1:
		_DtTurnOffHourGlass(ge->frame);
		free_appt_struct(&appt);
		free_appt_struct(&new_a);
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

	/* We are not allowed to change the type of the entry, so we will
           remove that particular entry from the list for writing. */
 
	if (new_a->type) {
                if (new_a->type->name){
                        free(new_a->type->name);
                        new_a->type->name = NULL;
                }
        }

	/* if the repeat type/times is changed, reset the sequence end date */
	if (ad->version == DATAVER3 &&
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

	stat = csa_update_entry_attributes(ad->cal_handle, old_a, scope, CSA_FALSE, new_a->count, new_a->attrs, &new_entry, NULL);
	backend_err_msg(ge->frame, ad->name, stat, p->xm_error_pixmap);

	if (stat == CSA_SUCCESS)
		csa_free((CSA_buffer)new_entry);

	free_appt_struct(&appt);
	free_appt_struct(&new_a);

	add_all_gappt(ge);
	reset_alarm(c);
	invalidate_cache(ge->cal);
	paint_canvas(c, NULL, RENDER_CLEAR_FIRST);
	br_display(c);
	_DtTurnOffHourGlass(ge->frame);
}

static void
ge_clear_proc(Widget w, XtPointer client_data, XtPointer data) {
	GEditor *ge = (GEditor *)client_data;

	set_geditor_defaults(ge, 0, 0);
	XmListDeselectAllItems(ge->access_list);
	set_message(ge->message_text, " ");
	add_all_gappt(ge);
}

static void
ge_close_proc(Widget w, XtPointer client_data, XtPointer data) {
	GEditor *ge = (GEditor *)client_data;

	geditor_clean_up(ge);
	XtPopdown(ge->frame);
	ge->geditor_is_up = False;
}

static void
ge_quit_handler(Widget w, XtPointer cdata, XtPointer data) {
	ge_close_proc(w, cdata, data);
}

static void
FormGroupApptDragMotionHandler(Widget dragInitiator, XtPointer clientData,
			       XEvent *event) {
        int             diffX, diffY;
        Calendar        *c = (Calendar *) clientData;
        GEditor         *ge = (GEditor *) c->geditor;
 
        if (!ge->doing_drag) {
                /*
                 * If the drag is just starting, set initial button down coords
                 */
                if (ge->initialX == -1 && ge->initialY == -1) {
                        ge->initialX = event->xmotion.x;
                        ge->initialY = event->xmotion.y;
                }
                /*
                 * Find out how far pointer has moved since button press
                 */
                diffX = ge->initialX - event->xmotion.x;
                diffY = ge->initialY - event->xmotion.y;
 
                if ((ABS(diffX) >= DRAG_THRESHOLD) ||
                    (ABS(diffY) >= DRAG_THRESHOLD)) {
                        ge->doing_drag = True;
			ApptDragStart(dragInitiator, event, c, GroupEditorIcon);
                        ge->initialX = -1;
                        ge->initialY = -1;
                }
        }
}

static void
ge_make_editor(Calendar *c) {
	int		cnt;
	Arg		args[15];
	char		*buf;
        GEditor		*ge = (GEditor *)c->geditor;
	Props_pu	*p = (Props_pu *)c->properties_pu;
	XmString	xmstr;
	Dimension	label_height;
        XtTranslations  new_translations;
	Boolean		btn1_transfer;
	Widget		second_list_label;
	Widget		second_access_label;
	XFontSetExtents listfontextents;
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

        new_translations = XtParseTranslationTable(translations);

	ge->cal = c;

	CalFontExtents(c->fonts->userfont, &listfontextents);

	/*
	**  Dialog shell
	*/
	cnt = 0;
	XtSetArg(args[cnt], XmNdeleteResponse, XmDO_NOTHING); ++cnt;
	XtSetArg(args[cnt], XmNallowShellResize, True); ++cnt;
	XtSetArg(args[cnt], XmNmappedWhenManaged, False); ++cnt;
	ge->frame = XmCreateDialogShell(ge->cal->frame, "frame", args, cnt);

	/*
	**  Base form
	*/
	cnt = 0;
	XtSetArg(args[cnt], XmNautoUnmanage, False); ++cnt;
	XtSetArg(args[cnt], XmNfractionBase, 100); ++cnt;
	ge->base_form_mgr = XmCreateForm(ge->frame, "base_form_mgr", args, cnt);

	/*
	**  Set title and quit handler
	*/
	set_geditor_title(ge, c->view->current_calendar);
	setup_quit_handler(ge->frame, ge_quit_handler, (caddr_t)ge);

	/*
	**  Message widget
	*/
	cnt = 0;
	XtSetArg(args[cnt], XmNalignment, XmALIGNMENT_BEGINNING); ++cnt;
	XtSetArg(args[cnt], XmNbottomAttachment, XmATTACH_FORM); ++cnt;
	XtSetArg(args[cnt], XmNbottomOffset, 2); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_FORM); ++cnt;
	XtSetArg(args[cnt], XmNleftOffset, 2); ++cnt;
	XtSetArg(args[cnt], XmNrightAttachment, XmATTACH_FORM); ++cnt;
	XtSetArg(args[cnt], XmNrightOffset, 2); ++cnt;
	ge->message_text = XmCreateLabelGadget(ge->base_form_mgr, "message",
					       args, cnt);
	/*
	**  Create insert, delete, change, and clear buttons in rc manager
	*/

	ge->button_rc_mgr = XtVaCreateWidget("button_rc_mgr",
                xmFormWidgetClass, ge->base_form_mgr,
                XmNautoUnmanage, False,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, ge->message_text,
		XmNtopOffset, 10,
		XmNbottomOffset, 5,
                XmNhorizontalSpacing, 10,
		XmNfractionBase, 8,
                NULL);


	cnt = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 342, "Insert"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNleftPosition, 1); ++cnt;
	XtSetArg(args[cnt], XmNrightAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNrightPosition, 2); ++cnt;
        ge->insert_button = XmCreatePushButton(ge->button_rc_mgr, "insert",
					       args, cnt);
	XmStringFree(xmstr);
	XtAddCallback(ge->insert_button, XmNactivateCallback, ge_insert_proc,
		      ge);

	cnt = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 343, "Change"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNleftPosition, 2); ++cnt;
	XtSetArg(args[cnt], XmNrightAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNrightPosition, 3); ++cnt;
        ge->change_button = XmCreatePushButton(ge->button_rc_mgr, "change",
					       args, cnt);
	XmStringFree(xmstr);
	XtAddCallback(ge->change_button, XmNactivateCallback, ge_change_proc,
		      ge);

	cnt = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 419, "Delete"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNleftPosition, 3); ++cnt;
	XtSetArg(args[cnt], XmNrightAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNrightPosition, 4); ++cnt;
        ge->delete_button = XmCreatePushButton(ge->button_rc_mgr, "delete",
					       args, cnt);
	XmStringFree(xmstr);
	XtAddCallback(ge->delete_button, XmNactivateCallback, ge_delete_proc,
		      ge);

	cnt = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 803, "Clear"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNleftPosition, 4); ++cnt;
	XtSetArg(args[cnt], XmNrightAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNrightPosition, 5); ++cnt;
        ge->clear_button = XmCreatePushButton(ge->button_rc_mgr, "clear",
					       args, cnt);
	XmStringFree(xmstr);
	XtAddCallback(ge->clear_button, XmNactivateCallback, ge_clear_proc,
		      ge);

	cnt = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 923, "Cancel"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNleftPosition, 5); ++cnt;
	XtSetArg(args[cnt], XmNrightAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNrightPosition, 6); ++cnt;
        ge->close_button = XmCreatePushButton(ge->button_rc_mgr, "close",
					       args, cnt);
	XmStringFree(xmstr);
	XtAddCallback(ge->close_button, XmNactivateCallback, ge_close_proc,
		      ge);

	cnt = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNleftPosition, 6); ++cnt;
	XtSetArg(args[cnt], XmNrightAttachment, XmATTACH_POSITION); ++cnt;
	XtSetArg(args[cnt], XmNrightPosition, 7); ++cnt;
        ge->help_button = XmCreatePushButton(ge->button_rc_mgr, "help",
					       args, cnt);
	XmStringFree(xmstr);
        XtAddCallback(ge->help_button, XmNactivateCallback,
		      (XtCallbackProc)help_cb, GROUP_APPT_EDITOR_HELP_BUTTON);
        XtAddCallback(ge->base_form_mgr, XmNhelpCallback,
		      (XtCallbackProc)help_cb, GROUP_APPT_EDITOR_HELP_BUTTON);

	ManageChildren(ge->button_rc_mgr);

	/*
	**  The separators
	*/
	cnt = 0;
	XtSetArg(args[cnt], XmNbottomAttachment, XmATTACH_WIDGET); ++cnt;
	XtSetArg(args[cnt], XmNbottomWidget, ge->button_rc_mgr); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_FORM); ++cnt;
	XtSetArg(args[cnt], XmNleftOffset, 5); ++cnt;
	XtSetArg(args[cnt], XmNrightAttachment, XmATTACH_FORM); ++cnt;
	XtSetArg(args[cnt], XmNrightOffset, 5); ++cnt;
	ge->separator2 = XmCreateSeparatorGadget(ge->base_form_mgr,
						 "separator2", args, cnt);

	cnt = 0;
	XtSetArg(args[cnt], XmNbottomAttachment, XmATTACH_WIDGET); ++cnt;
	XtSetArg(args[cnt], XmNbottomWidget, ge->separator2); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_FORM); ++cnt;
	XtSetArg(args[cnt], XmNleftOffset, 5); ++cnt;
	XtSetArg(args[cnt], XmNrightAttachment, XmATTACH_FORM); ++cnt;
	XtSetArg(args[cnt], XmNrightOffset, 5); ++cnt;
	ge->separator1 = XmCreateSeparatorGadget(ge->base_form_mgr,
						 "separator1", args, cnt);

	/*
	**  The more button
	*/
	cnt = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 626, "More"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNbottomAttachment, XmATTACH_WIDGET); ++cnt;
	XtSetArg(args[cnt], XmNbottomWidget, ge->separator1); ++cnt;
	XtSetArg(args[cnt], XmNbottomOffset, 3); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_FORM); ++cnt;
	XtSetArg(args[cnt], XmNleftOffset, 5); ++cnt;
        ge->expand_ui_button = XmCreatePushButton(ge->base_form_mgr,
		"expand_ui_button", args, cnt);
	XmStringFree(xmstr);
	XtAddCallback(ge->expand_ui_button, XmNactivateCallback,
		ge_expand_ui_proc, ge);

	/*
	**  The appt list label
	*/
	cnt = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 971, "Time Calendar"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNtopAttachment, XmATTACH_FORM); ++cnt;
	XtSetArg(args[cnt], XmNtopOffset, 5); ++cnt;
	ge->appt_list_label = XmCreateLabelGadget(ge->base_form_mgr,
						  "appt_list_label", args, cnt);
	XmStringFree(xmstr);

	/*
	**  The appt list label
	*/
	cnt = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 972, "What"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNtopAttachment, XmATTACH_FORM); ++cnt;
	XtSetArg(args[cnt], XmNtopOffset, 5); ++cnt;
	second_list_label = XmCreateLabelGadget(ge->base_form_mgr,
						  "second_list_label", args, cnt);
	XmStringFree(xmstr);

	XtVaGetValues(ge->appt_list_label, XmNheight, &label_height, NULL);

	/*
	**  The dssw widget
	*/
	ge->dsswFlags = 0;
	build_dssw(&ge->dssw, c, ge->base_form_mgr, True, True);
	XtVaSetValues(ge->dssw.dssw_form_mgr,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, label_height + 5,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, ge->expand_ui_button,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, 5,
		NULL);

	/*
	 * Add a drag source icon inside the dssw, lower right
	 */
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 627, "Drag Appt"));
	ge->drag_source = XtVaCreateWidget("drag_source",
		dtIconGadgetClass, ge->dssw.dssw_form_mgr,
		XmNpixmapPosition, XmPIXMAP_TOP,
		XmNstringPosition, XmSTRING_BOTTOM,
		XmNalignment, XmALIGNMENT_CENTER,
		XmNstring, xmstr,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNtraversalOn, False,
		NULL);
	XmStringFree(xmstr);

        XtAddEventHandler(XtParent(ge->drag_source), Button1MotionMask, False,
                (XtEventHandler)FormGroupApptDragMotionHandler, (XtPointer) c);

	XtVaGetValues((Widget)XmGetXmDisplay(XtDisplay(c->frame)), 
			"enableBtn1Transfer",   &btn1_transfer, 
			NULL); 

        /* btn1_transfer is a tri-state variable - see 1195846 */ 
	if ((Boolean)btn1_transfer != True)
        	XtAddEventHandler(XtParent(ge->drag_source), 
				Button2MotionMask, False,
                		(XtEventHandler)FormGroupApptDragMotionHandler,
				(XtPointer) c);

        if (p->drag_icon_xbm)
                XtVaSetValues(ge->drag_source,
                                XmNpixmap, p->drag_icon_xbm,
                                NULL);

	XtVaSetValues(ge->dssw.what_scrollwindow, 
			XmNrightAttachment, 	XmATTACH_WIDGET,
			XmNrightWidget, 	ge->drag_source, 
			NULL);
	
	ManageChildren(ge->dssw.dssw_form_mgr);

	/*
	**  The access list
	*/

	cnt = 0;
	XtSetArg(args[cnt], XmNheight, 4 * listfontextents.max_logical_extent.height); ++cnt;
	XtSetArg(args[cnt], XmNlistSizePolicy, XmCONSTANT); ++cnt;
	XtSetArg(args[cnt], XmNselectionPolicy, XmMULTIPLE_SELECT); ++cnt;
	XtSetArg(args[cnt], XmNwidth, 23 * listfontextents.max_logical_extent.width); ++cnt;
	XtSetArg(args[cnt], XmNdoubleClickInterval, 5); ++cnt; 
        ge->access_list = XmCreateScrolledList(ge->base_form_mgr,
					       "access_list", args, cnt);
	XtAddCallback(ge->access_list, XmNmultipleSelectionCallback,
		      ge_access_select_proc, (XtPointer)ge); 

	ge->access_list_sw = XtParent(ge->access_list);

        XtVaSetValues(ge->access_list_sw,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, ge->dssw.dssw_form_mgr,
		XmNleftOffset, 5,
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, 5,
		XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNbottomWidget, ge->dssw.dssw_form_mgr,
                NULL);
        XtManageChild(ge->access_list);
	/*
	**  The access list label
	*/
	cnt = 0;
	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 340, "Calendar"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNbottomAttachment, XmATTACH_WIDGET); ++cnt;
	XtSetArg(args[cnt], XmNbottomWidget, ge->access_list_sw); ++cnt;
	XtSetArg(args[cnt], XmNbottomOffset, 5); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET); ++cnt;
	XtSetArg(args[cnt], XmNleftWidget, ge->access_list_sw); ++cnt;
	ge->access_list_label = XmCreateLabelGadget(ge->base_form_mgr,
		"access_list_label", args, cnt);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 341, "Access"));
	XtSetArg(args[cnt], XmNlabelString, xmstr); ++cnt;
	XtSetArg(args[cnt], XmNbottomAttachment, XmATTACH_WIDGET); ++cnt;
	XtSetArg(args[cnt], XmNbottomWidget, ge->access_list_sw); ++cnt;
	XtSetArg(args[cnt], XmNbottomOffset, 5); ++cnt;
	XtSetArg(args[cnt], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET); ++cnt;
	XtSetArg(args[cnt], XmNleftWidget, ge->access_list_sw); ++cnt;
	XtSetArg(args[cnt], XmNleftOffset, 23 * listfontextents.max_logical_extent.width); ++cnt;
	second_access_label = XmCreateLabelGadget(ge->base_form_mgr,
		"second_access_label", args, cnt);
	XmStringFree(xmstr);

	/*
	**  The appt list
	*/
	cnt = 0;
	XtSetArg(args[cnt], XmNlistSizePolicy, XmCONSTANT); ++cnt;
	XtSetArg(args[cnt], XmNscrollBarDisplayPolicy, XmSTATIC); ++cnt;
	XtSetArg(args[cnt], XmNwidth, 200); ++cnt;
	XtSetArg(args[cnt], XmNdoubleClickInterval, 5); ++cnt; 
	ge->appt_list = XmCreateScrolledList(ge->base_form_mgr,
					     "ge_appt_list", args, cnt);

        XtOverrideTranslations(ge->appt_list, new_translations);
        /* Make btn 2 do dnd of appts */
	/* btn1_transfer is a tri-state variable - see 1195846 */ 
	if ((Boolean)btn1_transfer != True) {   
                new_translations = XtParseTranslationTable(btn2_translations);
                XtOverrideTranslations(ge->appt_list, new_translations);
        }

	XtAddCallback(ge->appt_list, XmNbrowseSelectionCallback,
		      ge_list_select_proc, (XtPointer)ge); 

	ge->appt_list_sw = XtParent(ge->appt_list);

	XtVaSetValues(ge->appt_list_sw,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, label_height + 11,
		XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, ge->access_list_sw,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, ge->access_list_sw,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, ge->access_list_label,
		XmNbottomOffset, 10,
		NULL);
        XtManageChild(ge->appt_list);

	/*
	**  Move the label over to the top of the access list scrolling window
	*/
	XtVaSetValues(ge->appt_list_label,
		XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, ge->appt_list_sw,
		NULL);

	XtVaSetValues(second_list_label,
		XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNleftWidget, ge->appt_list_sw,
		XmNleftOffset, 17 * listfontextents.max_logical_extent.width,
		NULL);

        XtVaSetValues(ge->button_rc_mgr, XmNdefaultButton, ge->insert_button, NULL);
        XtVaSetValues(ge->button_rc_mgr, XmNcancelButton, ge->close_button, NULL);
        XmProcessTraversal(ge->dssw.what_text, XmTRAVERSE_CURRENT);
        XtVaSetValues(ge->base_form_mgr, XmNinitialFocus, ge->dssw.what_text, NULL);

	/*
	**  Managed the base form, reset flags and set up initial X and Y
	**  values for the first dnd operation.
	*/
	ManageChildren(ge->base_form_mgr);
	XtManageChild(ge->base_form_mgr);

	rfp_init(&(ge->rfp), c, ge->base_form_mgr);

	ge->access_data = CmDataListCreate();
	ge->list_data = CmDataListCreate();
	ge->initialX = -1;
	ge->initialY = -1;
        ge->doing_drag = False;
}

/*******************************************************************************
**
**  External functions
**
*******************************************************************************/
extern void
add_to_gaccess_list(
	char			*name,
	CSA_session_handle	cal_handle,
	unsigned int		user_access,
	int			version,
	GEditor			*ge,
	Boolean			select_val)
{

	int		cnt = 1;
	char		access, *buf;
	XmString	xmstr;
	Calendar	*c = ge->cal;
	Props_pu	*p = (Props_pu *)ge->cal->properties_pu;
	Access_data	*new_data = NULL;

	if (!ge)
		return;

	while (ge->access_data && cnt <= ge->access_data->count) {
		new_data = (Access_data *)
			CmDataListGetData(ge->access_data, cnt);
		if (strcmp(new_data->name, name) == 0) {
			XmListDeselectPos(ge->access_list, cnt);
			if (select_val)
				XmListSelectPos(ge->access_list, cnt, True);
			return;
		}
		++cnt;
	}

	if (same_user(name, c->calname) ||  c->my_cal_handle == cal_handle) {
		user_access = CSA_OWNER_RIGHTS;
		access = 'Y';
	} else {

		if (version < DATAVER4) {
			access = (user_access & (CSA_X_DT_INSERT_ACCESS |
				CSA_OWNER_RIGHTS)) ? 'Y' : 'N';
		} else {
			access = (user_access & (CSA_INSERT_PUBLIC_ENTRIES |
				CSA_INSERT_CONFIDENTIAL_ENTRIES |
				CSA_INSERT_PRIVATE_ENTRIES |
				CSA_OWNER_RIGHTS)) ? 'Y' : 'N';
		}
	}

	new_data = (Access_data *)ckalloc(sizeof(Access_data));
	new_data->appt_count = 0;
	new_data->appt_head = NULL;
	new_data->name = cm_strdup(name);
	new_data->cal_handle = cal_handle;
	new_data->version = version;
	new_data->entry_access = user_access;
	CmDataListAdd(ge->access_data, (void *)new_data, 0);

	buf = (char *)ckalloc(ACCESS_NAME_LEN + cm_strlen(catgets(c->DT_catd,
		1, 348, "Insert Permission")) + 5);
	sprintf(buf, "%-*s %c", ACCESS_NAME_LEN, name, access);
	xmstr = XmStringCreateLocalized(buf);
	free(buf);
	XmListAddItemUnselected(ge->access_list, xmstr, 0);
	if (select_val)
		XmListSelectItem(ge->access_list, xmstr, True);
	XmStringFree(xmstr);

	ge_set_modify_buttons(ge, ge->access_data->count);
}

extern void
add_all_gappt(GEditor *ge) {
	int		tick, i, j, range_count;
	CSA_uint32	entry_count = 0;
	char		*date;
	Props		*p;
	time_t		start, stop;
	Boolean		at_least_one;
	Props_pu	*pu;
	CSA_entry_handle*entry_list = NULL;
	CSA_return_code	stat;
	Access_data	*step;
	OrderingType	o;
	SeparatorType	s;
	CSA_enum	*ops;
	CSA_attribute	*range_attr;

	if (!geditor_showing(ge))
		return;

	/*
	 * Bug in list widget - if it's empty and you select it, it core dumps.
	 * So, make it insensitive - later, if we have any appointments, make
	 * it sensitive.
	 */
	XmListDeleteAllItems(ge->appt_list);
	XtSetSensitive(ge->appt_list, False);

	p = (Props *)ge->cal->properties;
	pu = (Props_pu *)ge->cal->properties_pu;
	o = get_int_prop(p, CP_DATEORDERING);
	s = get_int_prop(p, CP_DATESEPARATOR);
	date = get_date_from_widget(ge->cal->view->date, ge->dssw.date_text,
				    o, s);
	if (!date)
		return;

	if ((tick = cm_getdate(date, NULL)) <= 0)
		return;
	start = lowerbound(tick);
	stop = next_ndays(tick, 1) - 1;

	/*
	 * For each calendar in the list of calendars pointed to by
	 * access_data, look up that calendar's entries and add them to the
	 * Access_data structure.
	 */
	stat = CSA_SUCCESS;
	for (i = 1; i <= ge->access_data->count && stat == CSA_SUCCESS; i++) {
		/*
		 * Get the individual calendar information and make sure we
		 * have browse access before attempting to get stuff.
		 */
		step = (Access_data *)CmDataListGetData(ge->access_data, i);
		if (!step || !step->cal_handle ||
		    !((step->version < DATAVER4 &&
		     step->entry_access & (CSA_OWNER_RIGHTS |
		     CSA_X_DT_BROWSE_ACCESS)) ||
		     (step->version >= DATAVER4 &&
		     step->entry_access & (CSA_OWNER_RIGHTS |
		     CSA_VIEW_PUBLIC_ENTRIES | CSA_VIEW_CONFIDENTIAL_ENTRIES |
		     CSA_VIEW_PRIVATE_ENTRIES))))
			continue;

		setup_range(&range_attr, &ops, &range_count, start, stop,
		    	CSA_TYPE_EVENT, 0, B_FALSE, step->version);
	        csa_list_entries(step->cal_handle, range_count, range_attr,
			ops, &entry_count, &entry_list, NULL);
		free_range(&range_attr, &ops, range_count);

		backend_err_msg(ge->frame, step->name, stat,
				pu->xm_error_pixmap);
		if (stat != CSA_SUCCESS)
			continue;

		if (step->appt_head && step->appt_count >= 0)
			csa_free(step->appt_head);
		step->appt_head = entry_list;
		step->appt_count = entry_count;
	}
	if (stat != CSA_SUCCESS)
		return;

	/*
	 * Everything is cool, add _all_ the appointments to the list
	 */
	at_least_one = False;
	for (i = 1; i <= ge->access_data->count; i++) {
		step = (Access_data *)CmDataListGetData(ge->access_data, i);
		if (!step || step->appt_count <= 0 ||
		    !((step->version < DATAVER4 &&
		     step->entry_access & (CSA_OWNER_RIGHTS |
		     CSA_X_DT_BROWSE_ACCESS)) ||
		     (step->version >= DATAVER4 &&
		     step->entry_access & (CSA_OWNER_RIGHTS |
		     CSA_VIEW_PUBLIC_ENTRIES | CSA_VIEW_CONFIDENTIAL_ENTRIES |
		     CSA_VIEW_PRIVATE_ENTRIES))))
			continue;
		for (j = 0; j < step->appt_count; j++) {
			ge_add_to_gappt_list(step, j, ge, !at_least_one);
			at_least_one = True;
		}
	}

	/*
	 * Make sure we have at least one appointment before sensitizing the
	 * list widget (see comment above).
	 */
	if (at_least_one)
		XtSetSensitive(ge->appt_list, True);

	XtVaSetValues(ge->change_button, XmNsensitive, False, NULL);
	XtVaSetValues(ge->delete_button, XmNsensitive, False, NULL);
}

extern void
geditor_clean_up(GEditor *ge) {
	remove_all_gaccess(ge);
}

/*
**  Return the nth appointment from the array
*/
extern CSA_entry_handle
geditor_nth_appt(GEditor *ge, int idx, Access_data **ad) {
	List_data	*ld;

	ld = (List_data *)CmDataListGetData(ge->list_data, idx);
	if (ld && ld->entry_idx >= 0 && ld->entry_idx < ld->ad->appt_count) {
		*ad = ld->ad;
		return ld->ad->appt_head[ld->entry_idx];
	}

	*ad = NULL;
	return 0;
}

extern Boolean
geditor_showing(GEditor *ge) {
	if (ge)
		return ge->geditor_is_up;
	return False;
}

/*
extern void
get_geditor_vals(GEditor *ge) {
	get_dssw_vals(&ge->dssw, ge->cal->view->date);
	get_rfp_vals(&ge->rfp);
}
*/

extern void
remove_all_gaccess_data(GEditor *ge) {
	int		i;
	Access_data	*step;

	if (!ge->access_data)
		return;

	for (i = 1; i <= ge->access_data->count; i++) {
		step = (Access_data *)CmDataListGetData(ge->access_data, i);
		if (step && step->name)
			free(step->name);
		if (step->appt_head && step->appt_count >= 0)
			csa_free(step->appt_head);
		step->appt_head = NULL;
		step->appt_count = 0;
	}
	CmDataListDeleteAll(ge->access_data, B_TRUE);
}

extern void
remove_all_gaccess(GEditor *ge) {
	remove_all_gaccess_data(ge);
	XmListDeleteAllItems(ge->access_list);
}

extern void
remove_from_gaccess_list(char *name, GEditor *ge) {
	int		cnt;
	List_data	*ld;
	Access_data	*ad;

	for (cnt = ge->list_data->count; cnt > 0; cnt--) {
		ld = (List_data *)CmDataListGetData(ge->list_data, cnt);
		if (ld && strcmp(ld->ad->name, name) == 0) {
			CmDataListDeletePos(ge->list_data, cnt, True);
			XmListDeletePos(ge->appt_list, cnt);
		}
	}

	cnt = ge->access_data->count;
	ad = (Access_data *)CmDataListGetData(ge->access_data, cnt);
	while (ad && strcmp(ad->name, name) != 0)
		ad = (Access_data *)CmDataListGetData(ge->access_data, --cnt);
	if (ad) {
		if (ad->name)
			free(ad->name);
		if (ad->appt_head && ad->appt_count >= 0)
			csa_free(ad->appt_head);
		CmDataListDeletePos(ge->access_data, cnt, True);
		XmListDeletePos(ge->access_list, cnt);
		ge_set_modify_buttons(ge, ge->access_data->count);
	}
}

extern void
set_geditor_defaults(GEditor *ge, Tick start, Tick stop) {
	int	*list = NULL, cnt;

	if (start > 0 || stop > 0) {
		load_dssw_times(&ge->dssw, start, stop, False);
		set_dssw_defaults(&ge->dssw, start, False);
	} else
		set_dssw_defaults(&ge->dssw, ge->cal->view->date, True);

	if (ge->rfp.rfp_form_mgr) 
		set_rfp_defaults(&ge->rfp);

	XmListGetSelectedPos(ge->access_list, &list, &cnt);
	ge_set_modify_buttons(ge, cnt);
	if (list)
		XtFree((XtPointer)list);

	ge->dsswFlags = ge->rfpFlags = 0;
}

extern void
set_geditor_title(GEditor *ge, char *name) {
	char		buf[MAXNAMELEN];
	Calendar	*c = ge->cal;

	if (ge->frame) {
		sprintf(buf, "%s - %s", catgets(c->DT_catd, 1, 349,
			"Calendar : Group Appointment Editor"), name);
		XtVaSetValues(ge->frame, XmNtitle, buf,
			NULL);
	}
}

extern void
set_geditor_vals(GEditor *ge, Tick start, Tick stop) {
	if (start > 0 || stop > 0) {
		load_dssw_times(&ge->dssw, start, stop, False);
		set_dssw_vals(&ge->dssw, start);
	} else
		set_dssw_vals(&ge->dssw, ge->cal->view->date);

	set_rfp_vals(&ge->rfp);

	ge->dsswFlags = ge->rfpFlags = 0;
}

extern void
show_geditor(Calendar *c, Tick start, Tick stop) {
	GEditor		*ge = (GEditor *)c->geditor;

	if (!ge->frame)
		ge_make_editor(c);

	if (!geditor_showing(ge)) {
		ds_position_popup(c->frame, ge->frame, DS_POPUP_LOR);
        	XmProcessTraversal(ge->dssw.what_text, XmTRAVERSE_CURRENT);
        	XtVaSetValues(ge->base_form_mgr, 
			XmNinitialFocus, ge->dssw.what_text, 
			NULL);
	}

	XtPopup(ge->frame, XtGrabNone);
	ge->geditor_is_up = True;
	set_geditor_defaults(ge, start, stop);
	add_all_gappt(ge);
}
