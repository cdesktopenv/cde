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
/*******************************************************************************
**
**  reminders.c
**
**  $XConsortium: reminders.c /main/7 1996/08/06 11:41:54 barstow $
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

#include <EUSCompat.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/param.h>
#include <ctype.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include "reminders.h"
#include "props.h"
#include "help.h"
#include "props_pu.h"
#include "util.h"

extern	int _csa_duration_to_iso8601(int, char *);
extern	int _csa_iso8601_to_duration(char *, int*);

/*******************************************************************************
**
**  Static functions local to reminders.c only
**
*******************************************************************************/

static Reminder_val_op
r_validate_advance(char *advance_string) {

	char 	*c_p = advance_string;
	char 	*end_ptr;
	Boolean scanned_sign = False;

        /* crush out leading white space for the name
           comparison/insert process */
 
        while ((*c_p == ' ') || (*c_p == '\t'))
                c_p++;
 
        /* compress off trailing whitespace */
 
        end_ptr = c_p;
        while (*end_ptr)
                end_ptr++;
        while ((end_ptr > c_p) &&
               ((*(end_ptr - 1) == ' ') || (*(end_ptr - 1) == '\t')))
                end_ptr--;
 
        *end_ptr = 0;


	if (blank_buf(c_p)) {
		return(ADVANCE_BLANK);
	}

	if (embedded_blank(c_p)) {
		return(ADVANCE_CONTAINS_BLANK);
	}

	while (c_p && *c_p) {
		if (isdigit(*c_p) || ((*c_p == '-') && (scanned_sign == False))) {
			if ((*c_p == '-') && (scanned_sign == False))
				scanned_sign = True;
			c_p++;
		} else {
			return(ADVANCE_NONNUMERIC);

		}
	}

	return(REMINDERS_OK);
}

static Reminder_val_op
r_get_bfpm_vals(Widget toggle, Widget text, Reminders_val *val,
		Boolean convert_to_secs) {
	char			*cval;
	Time_scope_menu_op	scope;
	Reminder_val_op		status;

	if (!XmToggleButtonGetState(toggle)) {
		val->selected = False;
		val->scope_val = -1;
		return(REMINDERS_OK);
	}

	val->selected = True;
	cval = XmTextGetString(text);

	if ((status = r_validate_advance(cval)) != REMINDERS_OK)
		return(status);

	val->scope_val = atoi(cval);
	XtFree(cval);

	switch(val->scope) {
	case TIME_DAYS:
		if (convert_to_secs)
			val->scope_val = days_to_seconds(val->scope_val);
		break;
	case TIME_HRS:
		if (convert_to_secs)
			val->scope_val = hours_to_seconds(val->scope_val);
		break;
	case TIME_MINS:
	default:
		if (convert_to_secs)
			val->scope_val = minutes_to_seconds(val->scope_val);
		break;
	}

	return(REMINDERS_OK);
}

static void
r_get_mailto_val(Reminders *r) {
	char	*c;

	c = XmTextGetString(r->mailto_text);
	if (!blank_buf(c))
		cm_strcpy(r->mailto_val, c);
	else
		cm_strcpy(r->mailto_val, cm_get_credentials());
	XtFree(c);
}

static void
r_set_bfpm_vals(
	Widget 		toggle, 
	Widget 		text, 
	Widget 		menu, 
	Reminders_val  *val,
	Boolean 	compute_best_fit) 
{
	char		buf[128];
	Calendar       *c = calendar;
	Widget		submenu;
	int		n;
	WidgetList	option_items;
	Time_scope_menu_op      scope = val->scope;
	XmString                xmstr;


	if (!XtIsManaged(toggle))
		return;

	XtVaGetValues(menu,
		XmNsubMenuId, 	&submenu,
		NULL);
	
	XtVaGetValues(submenu,
		XmNchildren,	&option_items,
		XmNnumChildren,	&n,
		NULL);

	if (!val->selected) {
		XmToggleButtonSetState(toggle, False, True);
		XtVaSetValues(text, XmNvalue, "\0",
			NULL);

		switch ( scope ) {
		  case TIME_MINS :
		      xmstr = XmStringCreateLocalized(
		catgets( calendar->DT_catd, 1, 877, time_scope_str(scope) ) );
		      break;
		  case TIME_HRS :
		      xmstr = XmStringCreateLocalized(
		catgets( calendar->DT_catd, 1, 878, time_scope_str(scope) ) );
		      break;
		  case TIME_DAYS :
		      xmstr = XmStringCreateLocalized(
		catgets( calendar->DT_catd, 1, 879, time_scope_str(scope) ) );
		      break;
		  default :
		      xmstr = XmStringCreateLocalized(time_scope_str(scope));
		}
		XtVaSetValues(XmOptionButtonGadget(menu), XmNlabelString, xmstr,
			NULL);
		XmStringFree(xmstr);

		return;
	}

	if (compute_best_fit) {
		if (val->scope_val % daysec == 0) {
			val->scope = TIME_DAYS;
                	sprintf(buf, "%d", seconds_to_days(val->scope_val));
		} else if (val->scope_val % hrsec == 0) {
			val->scope = TIME_HRS;
                	sprintf(buf, "%d", seconds_to_hours(val->scope_val));
		} else {
			val->scope = TIME_MINS;
               		sprintf(buf, "%d", seconds_to_minutes(val->scope_val));
		}
	} else
		sprintf(buf, "%d", val->scope_val);

	switch ( scope ) {
	  case TIME_MINS :
	      xmstr = XmStringCreateLocalized(
		catgets( calendar->DT_catd, 1, 877, time_scope_str(scope) ) );
	      break;
	  case TIME_HRS :
	      xmstr = XmStringCreateLocalized(
		catgets( calendar->DT_catd, 1, 878, time_scope_str(scope) ) );
	      break;
	  case TIME_DAYS :
	      xmstr = XmStringCreateLocalized(
		catgets( calendar->DT_catd, 1, 879, time_scope_str(scope) ) );
	      break;
	  default :
	      xmstr = XmStringCreateLocalized(time_scope_str(scope));
	}
	XtVaSetValues(XmOptionButtonGadget(menu), XmNlabelString, xmstr,
		NULL);
	XmStringFree(xmstr);

	XmToggleButtonSetState(toggle, True, True);
	XtVaSetValues(text, XmNvalue, buf,
		NULL);
}

static void
r_set_mailto_val(Reminders *r) {
	XtVaSetValues(r->mailto_text, XmNvalue, r->mailto_val,
		NULL);
}

/*
**  The function ensures the user data is set properly for the time scope
**  menus.  This way, when the r_get_bfpm_vals function is called, it knows
**  how it should convert the value found in the text field.
*/
static void
r_scope_menu_proc(Widget item, XtPointer client_data, XtPointer cbs) {
	Reminders_val		*val = (Reminders_val *)client_data;
	Time_scope_menu_op	scope;

	XtVaGetValues(item, XmNuserData, &val->scope,
		NULL);
}

/*
**  Function sets the alarm text fields and menus sensitive/insensitive
**  depending on the state of the toggle button.
*/
static void
r_alarm_toggle_proc(Widget toggle_w, XtPointer client_data, XtPointer cbs) {

	Boolean		status = XmToggleButtonGetState(toggle_w);
	Reminders	*r = (Reminders *)client_data;
	Widget		text_w, menu_w;

	if (!r)
		return;

	if (toggle_w == r->beep_toggle) {
		XtVaSetValues(r->beep_text, XmNsensitive, status,
			NULL);
		XtVaSetValues(XmOptionButtonGadget(r->beep_menu),
			XmNsensitive, status,
			NULL);
	} else if (toggle_w == r->flash_toggle) {
		XtVaSetValues(r->flash_text, XmNsensitive, status,
			NULL);
		XtVaSetValues(XmOptionButtonGadget(r->flash_menu),
			XmNsensitive, status,
			NULL);
	} else if (toggle_w == r->popup_toggle) {
		XtVaSetValues(r->popup_text, XmNsensitive, status,
			NULL);
		XtVaSetValues(XmOptionButtonGadget(r->popup_menu),
			XmNsensitive, status,
			NULL);
	} else if (toggle_w == r->mail_toggle) {
		XtVaSetValues(r->mail_text, XmNsensitive, status,
			NULL);
		XtVaSetValues(XmOptionButtonGadget(r->mail_menu),
			XmNsensitive, status,
			NULL);
		XtVaSetValues(r->mailto_label, XmNsensitive, status,
			NULL);
		XtVaSetValues(r->mailto_text, XmNsensitive, status,
			NULL);
	}
}

/*******************************************************************************
**
**  External functions
**
*******************************************************************************/
extern void
build_reminders(Reminders *r, Calendar *c, Widget parent) {

	Widget		widest_toggle;
	Dimension	widest, highest;
	Widget		prev, curr;
	Dimension	_toText, _toOption;
	XmString	xmstr;

	r->cal = c;
	r->parent = parent;

	/*
	**  Base form and label
	*/
	r->bfpm_form_mgr = XtVaCreateWidget("bfpm_form_mgr",
		xmFormWidgetClass, parent,
		XmNautoUnmanage, False,
		NULL);

	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 985, "Reminders"));
        r->alarm_label = XtVaCreateWidget("Reminders",
		xmLabelGadgetClass, r->bfpm_form_mgr,
		XmNlabelString,         xmstr,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 5,
		XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, 5,
                NULL);
	XmStringFree(xmstr);

	/* Create the 4 toggle items to see which one is the longest */

	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 986, "Beep"));
	r->beep_toggle = XtVaCreateWidget("Beep",
		xmToggleButtonGadgetClass, r->bfpm_form_mgr,
		XmNlabelString,         xmstr,
		XmNnavigationType, XmTAB_GROUP,
		NULL);
	XmStringFree(xmstr);


	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 987, "Flash"));
	r->flash_toggle = XtVaCreateWidget("Flash",
		xmToggleButtonGadgetClass, r->bfpm_form_mgr,
		XmNlabelString,         xmstr,
		XmNnavigationType, XmTAB_GROUP,
		NULL);
	XmStringFree(xmstr);


	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 988, "Popup"));
	r->popup_toggle = XtVaCreateWidget("Popup",
		xmToggleButtonGadgetClass, r->bfpm_form_mgr,
		XmNlabelString,         xmstr,
		XmNnavigationType, XmTAB_GROUP,
		NULL);
	XmStringFree(xmstr);


	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 989, "Mail"));
	r->mail_toggle = XtVaCreateWidget("Mail",
		xmToggleButtonGadgetClass, r->bfpm_form_mgr,
		XmNlabelString,         xmstr,
		XmNnavigationType, XmTAB_GROUP,
		NULL);
	XmStringFree(xmstr);


	xmstr = XmStringCreateLocalized(
                           catgets(c->DT_catd, 1, 990, "Mail To:" ) );
        r->mailto_label = XtVaCreateManagedWidget("MailTo",
                xmLabelGadgetClass, r->bfpm_form_mgr,
                XmNlabelString, xmstr,
                XmNsensitive, False,
                NULL);
	XmStringFree(xmstr);


	/*
	**  Beep toggle, text, and menu
	*/
	XtAddCallback(r->beep_toggle, XmNvalueChangedCallback,
		r_alarm_toggle_proc, r);

        r->beep_text = XtVaCreateWidget("beep_text",
		xmTextWidgetClass, r->bfpm_form_mgr,
                XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNsensitive, False,
		XmNmaxLength, REMINDER_SCOPE_LEN - 1,
                NULL);

        r->beep_menu = (Widget)create_time_scope_menu(r->bfpm_form_mgr,
		NULL, r_scope_menu_proc, (XtPointer)&r->beep_val);
	XtVaSetValues(r->beep_menu,
		XmNnavigationType, XmTAB_GROUP,
		NULL);

	/*
	**  Flash toggle, text, and menu
	*/
	XtAddCallback(r->flash_toggle, XmNvalueChangedCallback,
		r_alarm_toggle_proc, r);

        r->flash_text = XtVaCreateWidget("flash_text",
		xmTextWidgetClass, r->bfpm_form_mgr,
                XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNsensitive, False,
		XmNmaxLength, REMINDER_SCOPE_LEN - 1,
                NULL);

        r->flash_menu = (Widget)create_time_scope_menu(r->bfpm_form_mgr,
		NULL, r_scope_menu_proc, (XtPointer)&r->flash_val);
	XtVaSetValues(r->flash_menu,
		XmNnavigationType, XmTAB_GROUP,
		NULL);

	/*
	**  Popup toggle, text, and menu
	*/
	XtAddCallback(r->popup_toggle, XmNvalueChangedCallback,
		r_alarm_toggle_proc, r);

        r->popup_text = XtVaCreateWidget("popup_text",
		xmTextWidgetClass, r->bfpm_form_mgr,
                XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNsensitive, False,
		XmNmaxLength, REMINDER_SCOPE_LEN - 1,
                NULL);

        r->popup_menu = (Widget)create_time_scope_menu(r->bfpm_form_mgr,
		NULL, r_scope_menu_proc, (XtPointer)&r->popup_val);
	XtVaSetValues(r->popup_menu,
		XmNnavigationType, XmTAB_GROUP,
		NULL);

	/*
	**  Mail toggle, text, and menu
	*/
	XtAddCallback(r->mail_toggle, XmNvalueChangedCallback,
		r_alarm_toggle_proc, r);

        r->mail_text = XtVaCreateWidget("mail_text",
		xmTextWidgetClass, r->bfpm_form_mgr,
                XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNsensitive, False,
		XmNmaxLength, REMINDER_SCOPE_LEN - 1,
                NULL);

        r->mail_menu = (Widget)create_time_scope_menu(r->bfpm_form_mgr,
		NULL, r_scope_menu_proc, (XtPointer)&r->mail_val);
	XtVaSetValues(r->mail_menu,
		XmNnavigationType, XmTAB_GROUP,
		NULL);

	/*
        **  Mailto text
	*/
	r->mailto_text = XtVaCreateWidget("mailto_text",
		xmTextWidgetClass, r->bfpm_form_mgr,
		XmNeditMode, XmSINGLE_LINE_EDIT,
		XmNsensitive, False,
		XmNmaxLength, MAILTO_LEN - 1,
		NULL);
	/*
	 * Here, we do a layout.
	 */

	_i18n_WidestWidget( 5, &prev, &widest, r->beep_toggle, r->flash_toggle,
			r->popup_toggle, r->mail_toggle, r->mailto_label );
	_toText = 5 + 10 + widest + 10;

	_i18n_WidestWidget( 4, &prev, &widest, r->beep_text, r->flash_text,
			r->popup_text, r->mail_text );
	_toOption = _toText + widest + 5;

	/*
	 * r->beep_toggle,r->beep_text,r->beep_menu
	 */
	prev = r->alarm_label;
	_i18n_HighestWidgetAdjust( 3, &curr, &highest, r->beep_toggle,
			 r->beep_text, r->beep_menu );
	XtVaSetValues( curr,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNtopOffset, 10,
			NULL );
	XtVaSetValues( r->beep_toggle,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 5 + 10,
			NULL );
	XtVaSetValues( r->beep_text,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toText,
			NULL );
	XtVaSetValues( r->beep_menu,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toOption,
			NULL );
	prev = curr;

	/*
	 * r->flash_toggle,r->flash_text,r->flash_menu
	 */
	_i18n_HighestWidgetAdjust( 3, &curr, &highest, r->flash_toggle,
			r->flash_text, r->flash_menu );
	XtVaSetValues( curr,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNtopOffset, 10,
			NULL );
	XtVaSetValues( r->flash_toggle,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 5 + 10,
			NULL );
	XtVaSetValues( r->flash_text,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toText,
			NULL );
	XtVaSetValues( r->flash_menu,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toOption,
			NULL );
	prev = curr;

	/*
	 * r->popup_toggle,r->popup_text,r->popup_menu
	 */
	_i18n_HighestWidgetAdjust( 3, &curr, &highest, r->popup_toggle,
			r->popup_text, r->popup_menu );
	XtVaSetValues( curr,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNtopOffset, 10,
			NULL );
	XtVaSetValues( r->popup_toggle,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 5 + 10,
			NULL );
	XtVaSetValues( r->popup_text,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toText,
			NULL );
	XtVaSetValues( r->popup_menu,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toOption,
			NULL );
	prev = curr;

	/*
	 * r->mail_toggle,r->mail_text,r->mail_menu
	 */
	_i18n_HighestWidgetAdjust( 3, &curr, &highest, r->mail_toggle,
				r->mail_text, r->mail_menu );
	XtVaSetValues( curr,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNtopOffset, 10,
			NULL );
	XtVaSetValues( r->mail_toggle,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 5 + 10,
			NULL );
	XtVaSetValues( r->mail_text,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toText,
			NULL );
	XtVaSetValues( r->mail_menu,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toOption,
			NULL );
	prev = curr;

	/*
	 * r->mailto_label,r->mailto_text
	 */
	_i18n_HighestWidgetAdjust( 2, &curr, &highest, r->mailto_label,
					r->mailto_text );
	XtVaSetValues( curr,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNtopOffset, 20,
			NULL );
	XtVaSetValues( r->mailto_label,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 5 + 10,
			NULL );
	XtVaSetValues( r->mailto_text,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toText,
			NULL );
}

extern Reminder_val_op
get_reminders_vals(Reminders *r, Boolean convert_to_secs) {

	Reminder_val_op	status;

	if ((status = r_get_bfpm_vals(r->beep_toggle, r->beep_text, &r->beep_val,
			convert_to_secs)) != REMINDERS_OK)
		return(status);

	if ((status = r_get_bfpm_vals(r->flash_toggle, r->flash_text, &r->flash_val,
			convert_to_secs)) != REMINDERS_OK)
		return(status);

	if ((status = r_get_bfpm_vals(r->mail_toggle, r->mail_text, &r->mail_val,
			convert_to_secs)) != REMINDERS_OK)
		return(status);

	if ((status = r_get_bfpm_vals(r->popup_toggle, r->popup_text, &r->popup_val,
			convert_to_secs)) != REMINDERS_OK)
		return(status);

	r_get_mailto_val(r);

	return(REMINDERS_OK);
}

static void
reminder_err_msg(Widget frame, char *name, Reminder_val_op op, Pixmap p) 
{
        Calendar        *c = calendar;
        char            buf[MAXNAMELEN], buf2[MAXNAMELEN * 2];
	char		*title;
	char		*ident1;

        switch(op) {
        case ADVANCE_BLANK:
                sprintf(buf, "%s", catgets(c->DT_catd, 1, 775, "You must specify a reminder advance\nOperation was cancelled.  Calendar"));
                break;
        case ADVANCE_CONTAINS_BLANK:
                sprintf(buf, "%s", catgets(c->DT_catd, 1, 776, "A reminder advance may not contain an embedded blank\nOperation was cancelled.  Calendar"));
                break;
        case ADVANCE_NONNUMERIC:
                sprintf(buf, "%s", catgets(c->DT_catd, 1, 777, "Advance values must be a number with an optional sign\nOperation was cancelled.  Calendar"));
                break;
        case REMINDERS_OK:
        default:
                return;
        }

 
        sprintf(buf2, "%s %s", buf, name);
	title = XtNewString(catgets(c->DT_catd, 1, 364,
                        "Calendar : Error - Editor"));
	ident1 = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
        dialog_popup(frame, DIALOG_TITLE, title,
                DIALOG_TEXT, buf2,
                BUTTON_IDENT, 1, ident1,
                BUTTON_HELP, ADVANCE_ERROR_HELP,
                DIALOG_IMAGE, p,
                NULL);
	XtFree(ident1);
	XtFree(title);
}


/*
**  This function will consume form values and stuff them into an appointment.
*/
extern Boolean 
reminders_form_to_appt(Reminders *r, Dtcm_appointment *appt, char *name) {

	Reminder_val_op	status;
        Props_pu        *pu = (Props_pu *)r->cal->properties_pu;

	if (r->bfpm_form_mgr) {
		if ((status = get_reminders_vals(r, True)) != REMINDERS_OK) {
			reminder_err_msg(r->parent, name, status, 
							pu->xm_error_pixmap);
			return(False);
		}
	}

	if (!r->beep_val.selected) {
		free(appt->beep->value);
		appt->beep->value = NULL;
	} else {
		appt->beep->value->item.reminder_value->lead_time = (char *) malloc(BUFSIZ);
		_csa_duration_to_iso8601(r->beep_val.scope_val, appt->beep->value->item.reminder_value->lead_time);
		appt->beep->value->item.reminder_value->reminder_data.data = NULL;
		appt->beep->value->item.reminder_value->reminder_data.size = 0;
	}

	if (!r->flash_val.selected) {
		free(appt->flash->value);
		appt->flash->value = NULL;
	} else { 
		appt->flash->value->item.reminder_value->lead_time = (char *) malloc(BUFSIZ);
		_csa_duration_to_iso8601(r->flash_val.scope_val, appt->flash->value->item.reminder_value->lead_time);
		appt->flash->value->item.reminder_value->reminder_data.data = NULL;
		appt->flash->value->item.reminder_value->reminder_data.size = 0;
	}

	if (!r->popup_val.selected) {
		free(appt->popup->value);
		appt->popup->value = NULL;
	} else {
		appt->popup->value->item.reminder_value->lead_time = (char *) malloc(BUFSIZ);
		_csa_duration_to_iso8601(r->popup_val.scope_val, appt->popup->value->item.reminder_value->lead_time);
		appt->popup->value->item.reminder_value->reminder_data.data = NULL;
		appt->popup->value->item.reminder_value->reminder_data.size = 0;
	}

	if (!r->mail_val.selected) {
		free(appt->mail->value);
		appt->mail->value = NULL;
	} else {
		appt->mail->value->item.reminder_value->lead_time = (char *) malloc(BUFSIZ);
		_csa_duration_to_iso8601(r->mail_val.scope_val, appt->mail->value->item.reminder_value->lead_time);
		appt->mail->value->item.reminder_value->reminder_data.data = (CSA_uint8 *) cm_strdup(r->mailto_val);
		appt->mail->value->item.reminder_value->reminder_data.size = strlen(r->mailto_val);
	}

	return True;
}

/*
**  This function will take appointment values and stuff them into a form.
*/
extern Boolean
reminders_appt_to_form(Reminders *r, CSA_entry_handle entry) {
	Boolean			ret_val;
	CSA_return_code		stat;
	Dtcm_appointment	*appt;

	appt = allocate_appt_struct(appt_read,
				    r->cal->general->version,
				    CSA_ENTRY_ATTR_AUDIO_REMINDER_I,
				    CSA_ENTRY_ATTR_FLASHING_REMINDER_I,
				    CSA_ENTRY_ATTR_POPUP_REMINDER_I,
				    CSA_ENTRY_ATTR_MAIL_REMINDER_I,
				    NULL);
	stat = query_appt_struct(r->cal->cal_handle, entry, appt);
	backend_err_msg(r->cal->frame, r->cal->view->current_calendar, stat,
		((Props_pu *)r->cal->properties_pu)->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return False;
	}

	ret_val = reminders_attrs_to_form(r, appt);
	free_appt_struct(&appt);

	return ret_val;
}

extern Boolean
reminders_attrs_to_form(Reminders *r, Dtcm_appointment *appt) {

	if (appt->beep && appt->beep->value) {
		r->beep_val.selected = True;
		_csa_iso8601_to_duration(appt->beep->value->item.reminder_value->lead_time, &r->beep_val.scope_val);
	} else {
		r->beep_val.selected = False;
		r->beep_val.scope_val = -1;
	}

	if (appt->flash && appt->flash->value) {
		r->flash_val.selected = True;
		_csa_iso8601_to_duration(appt->flash->value->item.reminder_value->lead_time, &r->flash_val.scope_val);
	} else {
		r->flash_val.selected = False;
		r->flash_val.scope_val = -1;
	}

	if (appt->popup && appt->popup->value) {
		r->popup_val.selected = True;
		_csa_iso8601_to_duration(appt->popup->value->item.reminder_value->lead_time, &r->popup_val.scope_val);;
	} else {
		r->popup_val.selected = False;
		r->popup_val.scope_val = -1;
	}
	if (appt->mail && appt->mail->value) {
		r->mail_val.selected = True;
		_csa_iso8601_to_duration(appt->mail->value->item.reminder_value->lead_time, &r->mail_val.scope_val);
		memset(r->mailto_val, 0, MAILTO_LEN);
		strncpy(r->mailto_val, (char *) appt->mail->value->item.reminder_value->reminder_data.data, appt->mail->value->item.reminder_value->reminder_data.size);
	} else {
		r->mail_val.selected = False;
		r->mail_val.scope_val = -1;
		r->mailto_val[0] = '\0';
	}

	set_reminders_vals(r, True);

	return True;
}

extern void
set_reminders_defaults(Reminders *r) {
	Props		*p = (Props *)r->cal->properties;
	Reminders_val	*val;

	val = &r->beep_val;
	val->selected =	convert_boolean_str(get_char_prop(p, CP_BEEPON));
	val->scope = convert_time_scope_str(get_char_prop(p, CP_BEEPUNIT));
	val->scope_val = get_int_prop(p, CP_BEEPADV);

	val = &r->flash_val;
	val->selected =	convert_boolean_str(get_char_prop(p, CP_FLASHON));
	val->scope = convert_time_scope_str(get_char_prop(p, CP_FLASHUNIT));
	val->scope_val = get_int_prop(p, CP_FLASHADV);

	val = &r->popup_val;
	val->selected =	convert_boolean_str(get_char_prop(p, CP_OPENON));
	val->scope = convert_time_scope_str(get_char_prop(p, CP_OPENUNIT));
	val->scope_val = get_int_prop(p, CP_OPENADV);

	val = &r->mail_val;
	val->selected =	convert_boolean_str(get_char_prop(p, CP_MAILON));
	val->scope = convert_time_scope_str(get_char_prop(p, CP_MAILUNIT));
	val->scope_val = get_int_prop(p, CP_MAILADV);
	strcpy(r->mailto_val, get_char_prop(p, CP_MAILTO));

	set_reminders_vals(r, False);
}

extern void
set_reminders_vals(Reminders *r, Boolean compute_best_fit) {

	r_set_bfpm_vals(r->beep_toggle, r->beep_text, r->beep_menu,
		&r->beep_val, compute_best_fit);
	r_set_bfpm_vals(r->flash_toggle, r->flash_text, r->flash_menu,
		&r->flash_val, compute_best_fit);
	r_set_bfpm_vals(r->mail_toggle, r->mail_text, r->mail_menu,
		&r->mail_val, compute_best_fit);
	r_set_bfpm_vals(r->popup_toggle, r->popup_text, r->popup_menu,
		&r->popup_val, compute_best_fit);

	r_set_mailto_val(r);
}

static int
convert_time_val(
	int			value,
	Time_scope_menu_op	type)
{
	switch(type) {
        case TIME_DAYS:
                return days_to_seconds(value);
        case TIME_HRS:
        	return hours_to_seconds(value);
        case TIME_MINS:
        default:
        	return minutes_to_seconds(value);
        }
}

extern void
reminders_init(
	Reminders	*r,
	Calendar	*c,
	Widget		 parent)
{
	Props           *p = (Props *)c->properties;
        Reminders_val   *val;

	r->parent = parent;
	r->cal = c;

        val = &r->beep_val;
        val->selected = convert_boolean_str(get_char_prop(p, CP_BEEPON));
        val->scope = convert_time_scope_str(get_char_prop(p, CP_BEEPUNIT));
        val->scope_val = convert_time_val(get_int_prop(p, CP_BEEPADV), 
					  val->scope);

        val = &r->flash_val;
        val->selected = convert_boolean_str(get_char_prop(p, CP_FLASHON));
        val->scope = convert_time_scope_str(get_char_prop(p, CP_FLASHUNIT));
        val->scope_val = convert_time_val(get_int_prop(p, CP_FLASHADV), 
					  val->scope);

        val = &r->popup_val;
        val->selected = convert_boolean_str(get_char_prop(p, CP_OPENON));
        val->scope = convert_time_scope_str(get_char_prop(p, CP_OPENUNIT));
        val->scope_val = convert_time_val(get_int_prop(p, CP_OPENADV), 
					  val->scope);

        val = &r->mail_val;
        val->selected = convert_boolean_str(get_char_prop(p, CP_MAILON));
        val->scope = convert_time_scope_str(get_char_prop(p, CP_MAILUNIT));
        val->scope_val = convert_time_val(get_int_prop(p, CP_MAILADV), 
					  val->scope);
        strcpy(r->mailto_val, get_char_prop(p, CP_MAILTO));

}

