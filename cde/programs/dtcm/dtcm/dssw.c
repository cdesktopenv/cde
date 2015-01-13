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
**  dssw.c
**
**  $XConsortium: dssw.c /main/7 1996/10/14 16:06:20 barstow $
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
static  char sccsid[] = "@(#)dssw.c 1.58 95/08/07 Copyr 1993 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include "dssw.h"
#include "misc.h"
#include "getdate.h"
#include "props_pu.h"
#include "util.h"
#ifdef SVR4
#include <sys/param.h>
#endif /* SVR4 */

extern Dimension ComputeMaxWidth(Widget, Widget, Widget, Widget);


/*******************************************************************************
**
**  Static functions visible to dssw.c only
**
*******************************************************************************/
static void
dssw_get_non_times(DSSW *dssw, time_t t) {
	char		*str;
	Props		*p = (Props *)dssw->cal->properties;
	OrderingType	ot = get_int_prop(p, CP_DATEORDERING);
	SeparatorType	st = get_int_prop(p, CP_DATESEPARATOR);

	cm_strcpy(dssw->date_val,
		get_date_from_widget(t, dssw->date_text, ot, st));

	str = XmTextGetString(dssw->what_text);
	strcpy(dssw->what_val, str);
	XtFree(str);
}

static void
dssw_set_non_times(DSSW *dssw, time_t t) {
	Props		*p = (Props *)dssw->cal->properties;
	OrderingType	ot = get_int_prop(p, CP_DATEORDERING);
	SeparatorType	st = get_int_prop(p, CP_DATESEPARATOR);

	set_date_in_widget(t, dssw->date_text, ot, st);
	XmTextSetString(dssw->what_text, dssw->what_val);
}

/*
**  Callback from start time and stop time to set text fields accordingly
*/
static void
dssw_set_start_hour(Widget w, XtPointer client_data, XtPointer cbs) {
	int		start_hrs, stop_hrs, dur, start_mins, stop_mins;
	long		user_data;
	DSSW		*dssw = (DSSW *)client_data;
	Props		*p = (Props *)dssw->cal->properties;
	DisplayType	dt = get_int_prop(p, CP_DEFAULTDISP);

	XtVaGetValues(w, XmNuserData, &user_data, NULL);

	if (user_data == NO_TIME) {
		dssw->start_val.val[0] = '\0';
		dssw->stop_val.val[0]  = '\0';
		dssw->start_val.block = dssw->stop_val.block = TIME_AM;
        } else if (user_data == ALL_DAY) {
		if (dt == HOUR12) {
			sprintf(dssw->start_val.val, "12:00");
			sprintf(dssw->stop_val.val, "11:59");
		} else {
			sprintf(dssw->start_val.val, "0000");
			sprintf(dssw->stop_val.val, "2359");
		}
		dssw->start_val.block = TIME_AM;
		dssw->stop_val.block = TIME_PM;
	} else {
		start_hrs = user_data / hrsec;
		dur = get_int_prop(p, CP_APPTDURATION) * minsec;
		stop_hrs = (user_data + dur) / hrsec;
		start_mins = (user_data - (start_hrs * hrsec)) / minsec;
		stop_mins = ((user_data + dur) - (stop_hrs * hrsec)) / minsec;
		stop_hrs = stop_hrs % 24;

		if (dt == HOUR12) {
			dssw->start_val.block =
				(adjust_hour(&start_hrs)) ? TIME_AM : TIME_PM;
			dssw->stop_val.block =
				(adjust_hour(&stop_hrs)) ? TIME_AM : TIME_PM;
			sprintf(dssw->start_val.val, "%2d:%02d",
				start_hrs, start_mins);
			sprintf(dssw->stop_val.val, "%2d:%02d",
				stop_hrs, stop_mins);
		} else {
			dssw->start_val.block =
				(start_hrs > 12) ? TIME_PM : TIME_AM;
			dssw->stop_val.block =
				(stop_hrs > 12) ? TIME_PM : TIME_AM;
			sprintf(dssw->start_val.val, "%02d%02d",
				start_hrs, start_mins);
			sprintf(dssw->stop_val.val, "%02d%02d",
				stop_hrs, stop_mins);
		}
	}
	set_dssw_times(dssw);
}

static void
dssw_set_stop_hour(Widget w, XtPointer client_data, XtPointer cbs) {
	long		user_data;
	int             hrs, mins;
	DSSW		*dssw = (DSSW *)client_data;
	Props		*p = (Props *)dssw->cal->properties;
	DisplayType	dt = get_int_prop(p, CP_DEFAULTDISP);

	get_dssw_times(dssw);
	XtVaGetValues(w, XmNuserData, &user_data, NULL);

	if (user_data == NO_TIME) {
		dssw->stop_val.val[0] = '\0';
		dssw->stop_val.block = TIME_AM;
	} else {
		hrs = user_data / hrsec;
		mins = (user_data - (hrs * hrsec)) / minsec;

		if (dt == HOUR12) {
			dssw->stop_val.block =
				(adjust_hour(&hrs)) ? TIME_AM : TIME_PM;
			sprintf(dssw->stop_val.val, "%2d:%02d", hrs, mins);
		} else {
			dssw->stop_val.block = (hrs > 12) ? TIME_PM : TIME_AM;
			sprintf(dssw->stop_val.val, "%02d%02d", hrs, mins);
		}
	}
	set_dssw_times(dssw);
}

/*******************************************************************************
**
**  External functions
**
*******************************************************************************/

extern void
set_dssw_menus(DSSW *dssw, Props *p) {

	set_time_submenu(dssw->dssw_form_mgr, dssw->start_menu, p, 
			 dssw_set_start_hour, (XtPointer)dssw, 
			 dssw->show_notime_selection, dssw->show_allday_selection,
			 &dssw->start_menu_widget_list, &dssw->start_menu_widget_count);

	set_time_submenu(dssw->dssw_form_mgr, dssw->stop_menu, p, 
			 dssw_set_stop_hour, (XtPointer)dssw, dssw->show_notime_selection, False,
			 &dssw->stop_menu_widget_list, &dssw->stop_menu_widget_count);

	set_dssw_defaults(dssw, calendar->view->date, True);

}

#define GAP 	5

extern void
build_dssw(
	DSSW 		*dssw, 
	Calendar 	*c, 
	Widget 		 parent, 
	Boolean 	 show_notime, 
	Boolean 	 show_allday)
{
	Props		*p;
	DisplayType	 dt;
	Arg		 args[20];
	int		 n;
	Dimension	max_left_label_width;
	Widget		child[2];
	XmString	label_str;
	Dimension	highest, widest;
	Widget		prev, curr;
	Dimension	_toLabel, _toText, _toMenu, _toRC;

	dssw->cal = c;
	dssw->parent = parent;
	p = (Props *)c->properties;

	dt = get_int_prop(p, CP_DEFAULTDISP);

	dssw->show_notime_selection = show_notime;
	dssw->show_allday_selection = show_allday;

	/*
	**  One form manager for the widget
	*/
	dssw->dssw_form_mgr = XtVaCreateWidget("dssw_form_mgr",
		xmFormWidgetClass, 	parent,
		XmNautoUnmanage, 	False,
		NULL);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 828, "Date:"));
        dssw->date_label = XtVaCreateWidget("date_label",
		xmLabelGadgetClass, 	dssw->dssw_form_mgr,
		XmNlabelString,		label_str,
                NULL);
	XmStringFree(label_str);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 829, "Start:"));
        dssw->start_label = XtVaCreateWidget("start_label",
		xmLabelGadgetClass, 	dssw->dssw_form_mgr,
		XmNlabelString,		label_str,
                NULL);
	XmStringFree(label_str);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 832, "End:"));
        dssw->stop_label = XtVaCreateWidget("stop_label",
		xmLabelGadgetClass, 	dssw->dssw_form_mgr,
		XmNlabelString,		label_str,
                NULL);
	XmStringFree(label_str);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 835, "What:"));
        dssw->what_label = XtVaCreateWidget("what_label",
		xmLabelGadgetClass, 	dssw->dssw_form_mgr,
		XmNlabelString,		label_str,
                NULL);
	XmStringFree(label_str);
 
        dssw->date_text = XtVaCreateWidget("date_text",
		xmTextWidgetClass, 	dssw->dssw_form_mgr,
                XmNeditMode, 		XmSINGLE_LINE_EDIT,
		XmNmaxLength, 		DATE_LEN - 1,
                NULL);
 
	/*
	**  Radio button behavior for AM/PM selection for start time
	*/
	dssw->start_text = XtVaCreateWidget("start_text",
		xmTextWidgetClass, 	dssw->dssw_form_mgr,
		XmNeditMode, 		XmSINGLE_LINE_EDIT,
		XmNmaxLength, 		START_STOP_LEN - 1,
		NULL);

        dssw->start_menu = create_start_stop_time_menu(dssw->dssw_form_mgr,
		NULL, dssw_set_start_hour, (XtPointer)dssw, p, show_notime, 
		show_allday, &dssw->start_menu_widget_list, 
		&dssw->start_menu_widget_count);

	XtVaSetValues(dssw->start_menu,
		XmNnavigationType, 	XmTAB_GROUP,
		NULL);

        dssw->start_ampm_rc_mgr = XtVaCreateWidget("start_ampm_rc_mgr",
		xmRowColumnWidgetClass, dssw->dssw_form_mgr,
		XmNpacking, 		XmPACK_COLUMN,
		XmNorientation, 	XmHORIZONTAL,
		XmNradioBehavior, 	True,
		XmNisHomogeneous, 	True,
		XmNentryClass, 		xmToggleButtonGadgetClass,
		NULL);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 830, "AM"));
        child[0] = dssw->start_am = XtVaCreateWidget("start_am",
		xmToggleButtonGadgetClass, dssw->start_ampm_rc_mgr,
		XmNlabelString,		label_str,
		XmNsensitive, 		(dt == HOUR12) ? True : False,
		NULL);      
	XmStringFree(label_str);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 831, "PM"));
        child[1] = dssw->start_pm = XtVaCreateWidget("start_pm",
		xmToggleButtonGadgetClass, dssw->start_ampm_rc_mgr,
		XmNlabelString,		label_str,
		XmNsensitive, 		(dt == HOUR12) ? True : False,
		NULL);      
	XmStringFree(label_str);

	XtManageChildren(child, 2);

	/*
	**  Last and near the end, the stop stuff
	**  Radio button behavior for AM/PM selection for stop time
	*/
	dssw->stop_text = XtVaCreateWidget("stop_text",
		xmTextWidgetClass, 	dssw->dssw_form_mgr,
		XmNeditMode, 		XmSINGLE_LINE_EDIT,
		XmNmaxLength, 		START_STOP_LEN - 1,
		NULL);

        dssw->stop_menu = create_start_stop_time_menu(dssw->dssw_form_mgr,
		NULL, dssw_set_stop_hour, (XtPointer)dssw, p, show_notime, 
		False, &dssw->stop_menu_widget_list, 
		&dssw->stop_menu_widget_count);

	XtVaSetValues(dssw->stop_menu,
		XmNnavigationType, 	XmTAB_GROUP,
		NULL);

        dssw->stop_ampm_rc_mgr = XtVaCreateWidget("stop_ampm_rc_mgr",
		xmRowColumnWidgetClass, dssw->dssw_form_mgr,
		XmNpacking, 		XmPACK_COLUMN,
		XmNorientation, 	XmHORIZONTAL,
		XmNradioBehavior, 	True,
		XmNisHomogeneous, 	True,
		XmNentryClass, 		xmToggleButtonGadgetClass,
		NULL);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 833, "AM"));
        child[0] = dssw->stop_am = XtVaCreateWidget("stop_am",
		xmToggleButtonGadgetClass, dssw->stop_ampm_rc_mgr,
		XmNlabelString,		label_str,
		XmNsensitive, 		(dt == HOUR12) ? True : False,
		NULL);      
	XmStringFree(label_str);

	label_str = XmStringCreateLocalized(catgets(c->DT_catd, 1, 834, "PM"));
        child[1] = dssw->stop_pm = XtVaCreateWidget("stop_pm",
		xmToggleButtonGadgetClass, dssw->stop_ampm_rc_mgr,
		XmNlabelString,		label_str,
		XmNsensitive, 		(dt == HOUR12) ? True : False,
		NULL);      
	XmStringFree(label_str);

	XtManageChildren(child, 2);


	/*
	**  Finally the what text field
	*/
	n = 0;
        XtSetArg(args[n], XmNeditMode, 		XmMULTI_LINE_EDIT), n++;
/*	XtSetArg(args[n], XmNscrollVertical, 	True), n++;  */
	XtSetArg(args[n], XmNscrollHorizontal,	False), n++; 
	XtSetArg(args[n], XmNscrollingPolicy, 	XmAUTOMATIC), n++; 
	XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED), n++;
	XtSetArg(args[n], XmNrows, 		3), n++;
	XtSetArg(args[n], XmNmaxLength, 	WHAT_LEN - 1), n++;

	dssw->what_text = XmCreateScrolledText(dssw->dssw_form_mgr, 
					       "what_text", args, n);

	dssw->what_scrollwindow = XtParent(dssw->what_text);
	XtManageChild(dssw->what_text);

	/*
	 * Do a layout
	 */
	_i18n_WidestWidget( 4, &prev, &max_left_label_width,
		dssw->date_label, dssw->start_label, dssw->stop_label,
		dssw->what_label );
	_toText = max_left_label_width + 2 * GAP;

	/*
	 * dssw->date_label, dssw->date_text
	 */
	_i18n_WidestWidget( 1, &curr, &widest, dssw->date_label );
	_toLabel = _toText - GAP - widest;
	_i18n_HighestWidgetAdjust( 2, &curr, &highest, dssw->date_label,
					dssw->date_text );
	XtVaSetValues( curr,
			XmNtopAttachment, XmATTACH_FORM,
			XmNtopOffset, GAP,
			NULL );
	XtVaSetValues( dssw->date_label,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toLabel,
			NULL );
	XtVaSetValues( dssw->date_text,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toText,
			XmNrightAttachment, XmATTACH_FORM,
			NULL );
	prev = curr;

	/*
	 * dssw->start_label, dssw->start_text, dssw->start_menu,
	 * dssw->start_ampm_rc_mgr
	 */
	_i18n_WidestWidget( 1, &curr, &widest, dssw->start_label );
	_toLabel = _toText - GAP - widest;
	_i18n_WidestWidget( 1, &curr, &widest, dssw->start_text );
	_toMenu = _toText + widest + GAP;
	_i18n_WidestWidget( 1, &curr, &widest, dssw->start_menu );
	_toRC = _toMenu + widest + GAP;
	_i18n_HighestWidgetAdjust( 4, &curr, &highest, dssw->start_label,
		dssw->start_text, dssw->start_menu, dssw->start_ampm_rc_mgr );
	XtVaSetValues( curr,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNtopOffset, GAP,
			NULL );
	XtVaSetValues( dssw->start_label,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toLabel,
			NULL );
	XtVaSetValues( dssw->start_text,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toText,
			NULL );
	XtVaSetValues( dssw->start_menu,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toMenu,
			NULL );
	XtVaSetValues( dssw->start_ampm_rc_mgr,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toRC,
			NULL );
	prev = curr;

	/*
	 * dssw->stop_label, dssw->stop_text, dssw->stop_menu,
	 * dssw->stop_ampm_rc_mgr
	 */
	_i18n_WidestWidget( 1, &curr, &widest, dssw->stop_label );
	_toLabel = _toText - GAP - widest;
	_i18n_WidestWidget( 1, &curr, &widest, dssw->stop_text );
	_toMenu = _toText + widest + GAP;
	_i18n_WidestWidget( 1, &curr, &widest, dssw->stop_menu );
	_toRC = _toMenu + widest + GAP;
	_i18n_HighestWidgetAdjust( 4, &curr, &highest, dssw->stop_label,
		dssw->stop_text, dssw->stop_menu, dssw->stop_ampm_rc_mgr );
	XtVaSetValues( curr,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNtopOffset, GAP,
			NULL );
	XtVaSetValues( dssw->stop_label,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toLabel,
			NULL );
	XtVaSetValues( dssw->stop_text,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toText,
			NULL );
	XtVaSetValues( dssw->stop_menu,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toMenu,
			NULL );
	XtVaSetValues( dssw->stop_ampm_rc_mgr,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toRC,
			NULL );
	prev = curr;

	/*
	 * dssw->what_label, dssw->what_scrollwindow
	 */
	_i18n_WidestWidget( 1, &curr, &widest, dssw->what_label );
	_toLabel = _toText - GAP - widest;
	XtVaSetValues( dssw->what_label,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNtopOffset, GAP,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toLabel,
			NULL );
	XtVaSetValues( dssw->what_scrollwindow,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, prev,
			XmNtopOffset, GAP,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, _toText,
			XmNrightAttachment, XmATTACH_FORM,
			XmNbottomAttachment, XmATTACH_FORM,
			NULL );
}

/*
**  This function will consume form values and stuff them into an appointment.
*/
extern Boolean
dssw_form_to_appt(DSSW *dssw, Dtcm_appointment *a, char *name, Tick t)
{
  return dssw_form_flags_to_appt(dssw, a, name, t, (int *)NULL);
}

extern Boolean
dssw_form_flags_to_appt(DSSW *dssw, Dtcm_appointment *a, char *name, Tick t, int *flagsP)
{
	time_t		start_tick, stop_tick;
	char		ampm_buf[BUFSIZ], buf[BUFSIZ];
	Props		*p = (Props *)dssw->cal->properties;
	Props_pu	*pu = (Props_pu *)dssw->cal->properties_pu;
	DisplayType	dt = get_int_prop(p, CP_DEFAULTDISP);
	int		flags = 0;

	if (flagsP == (int *)NULL)
	  flagsP = &flags;

	/*
	**  If neither start nor end times exist, then the start time is set
	**  to 3:41am (magic time) and the end time to 3:41 plus one minute.
	*/
	get_dssw_vals(dssw, t);
	if (blank_buf(dssw->date_val)) {
		editor_err_msg(dssw->parent, name, MISSING_DATE,
			       pu->xm_error_pixmap);
		return False;
	}

	if (!blank_buf(dssw->start_val.val)) {
		if (!valid_time(p, dssw->start_val.val)) {
			if (a->type->value->item.sint32_value == CSA_TYPE_TODO)
				editor_err_msg(dssw->parent, name, 
					       INVALID_TIME_DUE,
					       pu->xm_error_pixmap);
			else
				editor_err_msg(dssw->parent, name, 
					       INVALID_START,
					       pu->xm_error_pixmap);
			return False;
		}

		if (dt == HOUR12) {
			/* am and pm should not be translated.  They are only
			 * used in the date parsing code and never shown to
			 * the user.
			 */
			if (dssw->start_val.block == TIME_AM)
				sprintf(ampm_buf, "am");
			else
				sprintf(ampm_buf, "pm");
		} else
			ampm_buf[0] = '\0';

		sprintf(buf, "%s %s%s",
			dssw->date_val, dssw->start_val.val, ampm_buf); 

		if (!blank_buf(dssw->stop_val.val)
			&& !valid_time(p, dssw->stop_val.val)) {
			editor_err_msg(dssw->parent, name, INVALID_STOP,
				pu->xm_error_pixmap);
               		return False;
		}
	} else if (!blank_buf(dssw->stop_val.val)) {
		editor_err_msg(dssw->parent, name, MISSING_START,
			pu->xm_error_pixmap);
		return False;
	} else
		sprintf(buf, "%s 3:41am", dssw->date_val);

	start_tick = cm_getdate(buf, NULL);
	if(start_tick < 0) {
		editor_err_msg(dssw->parent, name, INVALID_DATE,
			pu->xm_error_pixmap);
		return False;
	}

	if (blank_buf(dssw->what_val) && blank_buf(dssw->start_val.val)
		&& blank_buf(dssw->stop_val.val)) {
		editor_err_msg(dssw->parent, name, INVALID_NOTIME_APPT,
			pu->xm_error_pixmap);
		return False;
	}

	a->time->value->item.date_time_value = (char *) malloc(BUFSIZ);
	_csa_tick_to_iso8601(start_tick, a->time->value->item.date_time_value);
	a->what->value->item.string_value = (char *)cm_strdup(dssw->what_val);
	a->show_time->value->item.sint32_value = True;

	if (!blank_buf(dssw->stop_val.val)) {
		if (dt == HOUR12) {
			/* am and pm should not be translated.  They are only
			 * used in the date parsing code and never shown to
			 * the user.
			 */
			if (dssw->stop_val.block == TIME_AM)
				sprintf(ampm_buf, "am");
			else
				sprintf(ampm_buf, "pm");
		} else
			ampm_buf[0] = '\0';

		sprintf(buf, "%s %s%s", dssw->date_val,
			dssw->stop_val.val, ampm_buf); 
		if ((stop_tick = cm_getdate(buf, NULL)) <= 0) {
			editor_err_msg(dssw->parent, name, INVALID_DATE,
				       pu->xm_error_pixmap);
			return False;
		}
		if (stop_tick < start_tick) {
		    if (*flagsP == 0)
		    {
		  	char *title = XtNewString(catgets(calendar->DT_catd, 1, 248,
					"Calendar : Schedule Appointment"));
		  	char *ident1 = XtNewString(catgets(calendar->DT_catd, 1,
					923, "Cancel"));
		  	char *ident2 = XtNewString(catgets(calendar->DT_catd, 1,
					250, "Next Day"));
			sprintf(buf, "%s", catgets(calendar->DT_catd, 1, 247,
				"This appointment has an end time earlier than\nits begin time.  Do you want to\nschedule it into the next day?"));
			*flagsP = dialog_popup(dssw->parent,
				DIALOG_TITLE, title,
				DIALOG_TEXT, buf,
				BUTTON_IDENT, -1, ident1,
				BUTTON_IDENT, DSSW_NEXT_DAY, ident2,
				DIALOG_IMAGE, pu->xm_question_pixmap,
				NULL);
			XtFree(ident2);
			XtFree(ident1);
			XtFree(title);
		    }
		    switch (*flagsP) {
		    case DSSW_NEXT_DAY:
			stop_tick += daysec;
			break;

		    default:
			*flagsP = 0;
		        return False;
		    }
		}

		a->end_time->value->item.date_time_value = (char *) malloc(BUFSIZ);
		_csa_tick_to_iso8601(stop_tick, a->end_time->value->item.date_time_value);
	} else if (blank_buf(dssw->start_val.val)) {
		a->end_time->value->item.date_time_value = (char *) malloc(BUFSIZ);
		_csa_tick_to_iso8601(start_tick + minsec, a->end_time->value->item.date_time_value);
		a->show_time->value->item.sint32_value = False;
	} else {
		free(a->end_time->value);
		a->end_time->value = NULL;
	}

	return True;
}
/*
**  This function will consume form values and stuff them into an appointment.
*/
extern Boolean
dssw_form_to_todo(DSSW *dssw, Dtcm_appointment *a, char *name, Tick t)
{
	time_t		start_tick, stop_tick;
	char		ampm_buf[BUFSIZ], buf[BUFSIZ];
	Props		*p = (Props *)dssw->cal->properties;
	Props_pu	*pu = (Props_pu *)dssw->cal->properties_pu;
	DisplayType	dt = get_int_prop(p, CP_DEFAULTDISP);

	get_dssw_vals(dssw, t);
	/*
	 * Todo does not have end time.  So to distinguish between this
	 * and the editor, zero this out.
	 */
	dssw->stop_val.val[0] = '\0';
	if (blank_buf(dssw->date_val)) {
		editor_err_msg(dssw->parent, name, MISSING_DATE,
			       pu->xm_error_pixmap);
		return False;
	}

	if (!blank_buf(dssw->start_val.val)) {
		if (!valid_time(p, dssw->start_val.val)) {
			editor_err_msg(dssw->parent, name, INVALID_TIME,
				pu->xm_error_pixmap);
			return False;
		}

		if (dt == HOUR12) {
			if (dssw->start_val.block == TIME_AM)
				sprintf(ampm_buf, "am");
			else
				sprintf(ampm_buf, "pm");
		} else
			ampm_buf[0] = '\0';

		sprintf(buf, "%s %s%s",
			dssw->date_val, dssw->start_val.val, ampm_buf); 

		/* 
		 * No check here for stop time.
		 */
	} else {
		editor_err_msg(dssw->parent, name, MISSING_TIME,
			pu->xm_error_pixmap);
		return False;
	}

	start_tick = cm_getdate(buf, NULL);
	if(start_tick < 0) {
		editor_err_msg(dssw->parent, name, INVALID_DATE,
			pu->xm_error_pixmap);
		return False;
	}

	a->time->value->item.date_time_value = (char *) malloc(BUFSIZ);
	_csa_tick_to_iso8601(start_tick, a->time->value->item.date_time_value);
	a->what->value->item.string_value = (char *)cm_strdup(dssw->what_val);
	a->show_time->value->item.sint32_value = True;

	free(a->end_time->value);
	a->end_time->value = NULL;

	return True;
}

/*
**  The next two functions will take appointment values and stuff them into
**  a form.
*/
extern Boolean
dssw_appt_to_form(DSSW *dssw, CSA_entry_handle entry) {
	Boolean			ret_val;
	CSA_return_code		stat;
	Dtcm_appointment	*appt;

	appt = allocate_appt_struct(appt_read,
				    dssw->cal->general->version,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_SUMMARY_I,
				    CSA_ENTRY_ATTR_END_DATE_I,
				    CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
				    NULL);
	stat = query_appt_struct(dssw->cal->cal_handle, entry, appt);
	backend_err_msg(dssw->cal->frame, dssw->cal->view->current_calendar,
		stat, ((Props_pu *)dssw->cal->properties_pu)->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return False;
	}

	ret_val = dssw_attrs_to_form(dssw, appt);
	free_appt_struct(&appt);

	return ret_val;
}

extern Boolean
dssw_attrs_to_form(DSSW *dssw, Dtcm_appointment *appt) {
	int		start_hr, stop_hr;
	char		*what;
	time_t		tick, end_tick = 0;
	Boolean		showtime;
	Props		*p = (Props *)dssw->cal->properties;
	DisplayType	dt = get_int_prop(p, CP_DEFAULTDISP);

	_csa_iso8601_to_tick(appt->time->value->item.date_time_value, &tick);
	showtime = showtime_set(appt);
	if (appt->end_time)
		_csa_iso8601_to_tick(appt->end_time->value->item.\
			date_time_value, &end_tick);
	what = appt->what->value->item.string_value;

	/*
	**  Set the date, start, stop, and what fields
	*/
	if (!showtime || magic_time(tick)) {
		dssw->start_val.val[0] = '\0';
		dssw->stop_val.val[0] = '\0';
	} else {
		if ((start_hr = hour(tick)) < 12)
			dssw->start_val.block = TIME_AM;
		else
			dssw->start_val.block = TIME_PM;

		if (dt == HOUR12) {
			if (start_hr == 0)
				start_hr = 12;
			else if (start_hr > 12)
				start_hr -= 12;
			sprintf(dssw->start_val.val, "%2d:%02d", start_hr,
				minute(tick));
		} else
			sprintf(dssw->start_val.val, "%02d%02d", start_hr,
				minute(tick));

		if (end_tick) {
			if ((stop_hr = hour(end_tick)) < 12)
				dssw->stop_val.block = TIME_AM;
			else
				dssw->stop_val.block = TIME_PM;

			if (dt == HOUR12) {
				if (stop_hr == 0)
					stop_hr = 12;
				else if (stop_hr > 12)
					stop_hr -= 12;
				sprintf(dssw->stop_val.val, "%2d:%02d", stop_hr,
					minute(end_tick));
			} else
				sprintf(dssw->stop_val.val, "%02d%02d", stop_hr,
					minute(end_tick));
		} else {
			/* no end time */
			dssw->stop_val.val[0] = '\0';
			dssw->stop_val.block = TIME_AM;
		}
	}

	/* limit the size of the text pulled out of the appointment. */

	if (what) {
		strncpy(dssw->what_val, what, WHAT_LEN - 1);
		dssw->what_val[WHAT_LEN-1] = '\0';
	}

	set_dssw_vals(dssw, tick);

	return True;
}

extern void
get_dssw_times(DSSW *dssw) {
	char		*str;

	str = XmTextGetString(dssw->start_text);
	strcpy(dssw->start_val.val, str);
	XtFree(str);
	dssw->start_val.block =
		XmToggleButtonGetState(dssw->start_am) ? TIME_AM : TIME_PM;

	str = XmTextGetString(dssw->stop_text);
	strcpy(dssw->stop_val.val, str);
	XtFree(str);
	dssw->stop_val.block =
		XmToggleButtonGetState(dssw->stop_am) ? TIME_AM : TIME_PM;
}

extern void
get_dssw_vals(DSSW *dssw, Tick t) {
	dssw_get_non_times(dssw, t);
	get_dssw_times(dssw);
}

extern void
set_dssw_times(DSSW *dssw) {
	XmString	xmstr;
	Props		*p = (Props *)dssw->cal->properties;
	DisplayType	dt = get_int_prop(p, CP_DEFAULTDISP);

	XtVaSetValues(dssw->start_text, XmNvalue, dssw->start_val.val,
		NULL);
	XtVaSetValues(dssw->stop_text, XmNvalue, dssw->stop_val.val,
		NULL);

	xmstr = XmStringCreateLocalized(dssw->start_val.val);
	XtVaSetValues(XmOptionButtonGadget(dssw->start_menu),
		XmNlabelString, xmstr,
		NULL);
	XmStringFree(xmstr);
	xmstr = XmStringCreateLocalized(dssw->stop_val.val);
	XtVaSetValues(XmOptionButtonGadget(dssw->stop_menu),
		XmNlabelString, xmstr,
		NULL);
	XmStringFree(xmstr);

	if (dt == HOUR12) {
		if (dssw->start_val.block == TIME_AM)
			XmToggleButtonSetState(dssw->start_am, True, True);
		else
			XmToggleButtonSetState(dssw->start_pm, True, True);
		if (dssw->stop_val.block == TIME_AM)
			XmToggleButtonSetState(dssw->stop_am, True, True);
		else
			XmToggleButtonSetState(dssw->stop_pm, True, True);
	}
}

extern void
load_dssw_times(DSSW *dssw, Tick start, Tick stop, Boolean set_no_time) {
	char		buf[10];
	Props		*p = (Props *)dssw->cal->properties;
	DisplayType	dt;

	/*
	 * If start and stop times were specified, do the right thing to the
	 * dssw widget
	 */
	dt = get_int_prop(p, CP_DEFAULTDISP);

	if (set_no_time == True){
		strcpy(dssw->start_val.val, " ");
		strcpy(dssw->stop_val.val, " ");
		return;
	}

	if (start > 0) {
		format_time(start, dt, buf);
		if (dt == HOUR12) {
			strncpy(dssw->start_val.val, buf, 5);
			dssw->start_val.val[5] = '\0';
			dssw->start_val.block = (buf[5] == 'a') ?
				TIME_AM : TIME_PM;
		} else {
			strcpy(dssw->start_val.val, buf);
			dssw->start_val.block = TIME_AM;
		}
	}
	if (stop > 0) {
		format_time(stop, dt, buf);
		if (dt == HOUR12) {
			strncpy(dssw->stop_val.val, buf, 5);
			dssw->stop_val.val[5] = '\0';
			dssw->stop_val.block = (buf[5] == 'a') ?
				TIME_AM : TIME_PM;
		} else {
			strcpy(dssw->stop_val.val, buf);
			dssw->stop_val.block = TIME_AM;
		}
	}
}

extern void
set_dssw_vals(DSSW *dssw, Tick t) {
	dssw_set_non_times(dssw, t);
	set_dssw_times(dssw);
}

extern void
set_dssw_defaults(DSSW *dssw, Tick t, Boolean set_times) {
	int			appt_beg, appt_end, beg_hr, end_hr;
	Props			*p = (Props *)dssw->cal->properties;
	DisplayType		dt;
	Time_scope_menu_op	dur_scope;

	if (set_times) {
		dt = get_int_prop(p, CP_DEFAULTDISP);
		appt_beg = get_int_prop(p, CP_APPTBEGIN);
		beg_hr = appt_beg / minsec;
		appt_end = get_int_prop(p, CP_APPTDURATION) + appt_beg;
		end_hr = appt_end / minsec;
		end_hr = end_hr % 24;

		if (dt == HOUR12) {
			dssw->start_val.block =	(adjust_hour(&beg_hr)) ?
				TIME_AM : TIME_PM;
			dssw->stop_val.block =	(adjust_hour(&end_hr)) ?
				TIME_AM : TIME_PM;
			sprintf(dssw->start_val.val, "%2d:%02d",
				beg_hr, appt_beg % minsec);
			sprintf(dssw->stop_val.val, "%2d:%02d",
				end_hr, appt_end % minsec);
		} else {
			sprintf(dssw->start_val.val, "%02d%02d",
				beg_hr, appt_beg % minsec);
			sprintf(dssw->stop_val.val, "%02d%02d",
				end_hr, appt_end % minsec);
			dssw->start_val.block = dssw->stop_val.block = TIME_AM;
		}
	}
	dssw->what_val[0] = '\0';

	set_dssw_vals(dssw, t);
}
