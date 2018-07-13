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
/* $TOG: timezone.c /main/10 1999/07/01 16:36:43 mgreess $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdio.h>
#include <stdlib.h>
#include <csa.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/DialogS.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/MwmUtil.h>
#include <Xm/ComboBox.h>
#include <Dt/HourGlass.h>
#include "calendar.h"
#include "util.h"
#include "timeops.h"
#include "timezone.h"
#include "format.h"
#include "datefield.h"
#include "props.h"
#include "editor.h"
#include "todo.h"
#include "group_editor.h"
#include "select.h"
#include "help.h"

extern caddr_t
make_timezone(Calendar *c)
{
	Timezone *t;
	Widget text, separator, button_form;
	XmString xmstr;
	int ac;
	Arg args[10];
	char buf[BUFSIZ], tmp_buf[BUFSIZ] = "";
	char *title;
	void tz_customtime_cb(), tz_mytime_cb(), 
			tz_apply_cb(), tz_cancel_cb(), tz_ok_cb();

	if (c->timezone == NULL) {
                c->timezone = (caddr_t)ckalloc(sizeof(Timezone));
                t = (Timezone*)c->timezone;
        }
        else
                t = (Timezone*)c->timezone;

	title = XtNewString(catgets(c->DT_catd, 1, 649, "Calendar : Time Zone"));
	t->frame = XtVaCreatePopupShell("frame",
                xmDialogShellWidgetClass, c->frame,
                XmNtitle, 		title,
		XmNallowShellResize, 	False,
		XmNmwmFunctions, 	MWM_FUNC_CLOSE | MWM_FUNC_MOVE,
		XmNmappedWhenManaged, 	False,
                NULL);
	XtFree(title);

        t->form = XtVaCreateWidget("timezone_form",
                xmFormWidgetClass, 	t->frame,
                XmNautoUnmanage, 	False,
                XmNhorizontalSpacing, 	5,
                XmNverticalSpacing, 	5,
		XmNmarginWidth,         0,
		XmNmarginHeight,        0,
		XmNfractionBase, 	4,
                NULL);

	t->timezone_rc_mgr = XtVaCreateWidget("search_rc_mgr",
                xmRowColumnWidgetClass, t->form,
                XmNpacking, 		XmPACK_COLUMN,
                XmNorientation, 	XmVERTICAL,
                XmNradioBehavior, 	True,
                XmNisHomogeneous, 	True,
                XmNentryClass, 		xmToggleButtonGadgetClass,
                XmNleftAttachment, 	XmATTACH_FORM,
                XmNleftOffset, 		5,
                XmNtopAttachment, 	XmATTACH_FORM,
                XmNtopOffset, 		5,
                NULL);
 
        xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 650, "My Time"));
        t->mytime = XtVaCreateWidget("myTime",
                xmToggleButtonGadgetClass, t->timezone_rc_mgr,
		XmNlabelString, xmstr,
		XmNset, True,
		XmNuserData, t,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(t->mytime, XmNvalueChangedCallback, tz_mytime_cb, NULL);
 
        xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 652, "Custom Time"));
        t->customtime = XtVaCreateWidget("customTime",
                xmToggleButtonGadgetClass, t->timezone_rc_mgr,
		XmNlabelString, xmstr,
		XmNuserData, t,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(t->customtime, XmNvalueChangedCallback, tz_customtime_cb, NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 653, "GMT"));
        t->gmtlabel = XtVaCreateWidget("gmtLabel", xmLabelWidgetClass, t->form,
		XmNlabelString, xmstr,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, t->customtime,
		XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNbottomWidget, t->timezone_rc_mgr,
		XmNbottomOffset, 7,
                NULL);
	XmStringFree(xmstr);
	XtSetSensitive(t->gmtlabel, False);

        ac = 0;  
        XtSetArg(args[ac], XmNcomboBoxType, XmDROP_DOWN_COMBO_BOX); ++ac;
        XtSetArg(args[ac], XmNleftAttachment, XmATTACH_WIDGET); ++ac;
        XtSetArg(args[ac], XmNleftWidget, t->gmtlabel); ++ac;
        XtSetArg(args[ac], XmNleftOffset, 8); ++ac;
        XtSetArg(args[ac], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); ++ac;
        XtSetArg(args[ac], XmNbottomOffset, 5); ++ac;
        XtSetArg(args[ac], XmNbottomWidget, t->timezone_rc_mgr); ++ac;
	XtSetArg(args[ac], XmNpositionMode, XmONE_BASED); ++ac;
        t->gmtcombo = XmCreateComboBox(t->form, "gmtcombo", args, ac);

        XtVaSetValues(t->gmtcombo, XmNwidth, 75, NULL);
	XtSetSensitive(t->gmtcombo, False);

        for(ac = -12; ac <= 12; ac += 1) {
                if (ac >= 0)
                        sprintf(buf, "+%d", ac);  
		else
                        sprintf(buf, "%d", ac);  
		xmstr = XmStringCreateLocalized(buf);
		XmComboBoxAddItem(t->gmtcombo, xmstr, 0, False);
		XmStringFree(xmstr);
        }
	/* Remind: remove this kludge after timezone attribute is available */
	/* Remind: don't do this because of daylight savings time */
	/*
	XtVaGetValues(t->gmtcombo, XmNtextField, &text, NULL);
	XmTextSetString(text, "-6");
	*/

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 654, "Hours"));
        t->hourlabel = XtVaCreateWidget("gmtHours", xmLabelWidgetClass, t->form,
		XmNlabelString, xmstr,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, t->gmtcombo,
		XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, 10,
		XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNbottomWidget, t->timezone_rc_mgr,
		XmNbottomOffset, 7,
                NULL);
	XmStringFree(xmstr);
	XtSetSensitive(t->hourlabel, False);

	separator = XtVaCreateWidget("separator",
                xmSeparatorGadgetClass,
                t->form,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		t->timezone_rc_mgr,
		XmNtopOffset, 		1,
                NULL);

        button_form = XtVaCreateWidget("button_form",
                xmFormWidgetClass,      t->form,
                XmNautoUnmanage,        False,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNleftOffset,          5,
                XmNrightAttachment,     XmATTACH_FORM,
                XmNrightOffset,         5,
                XmNtopAttachment,       XmATTACH_WIDGET,
                XmNtopWidget,           separator,
                XmNhorizontalSpacing,   3,
                XmNfractionBase,        4,
                NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 655, "OK"));
       t->okbutton = XtVaCreateWidget("OK", 
		xmPushButtonWidgetClass, button_form,
		XmNlabelString, 	xmstr,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		separator,
		XmNtopOffset, 		8,
               	XmNleftAttachment, 	XmATTACH_POSITION,
                XmNleftPosition, 	0,
                XmNrightAttachment, 	XmATTACH_POSITION,
                XmNrightPosition, 	1,
		XmNnavigationType, 	XmTAB_GROUP,
		XmNuserData, 		c,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(t->okbutton, XmNactivateCallback, tz_ok_cb, NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 460, "Apply"));
       t->applybutton = XtVaCreateWidget("Apply", 
		xmPushButtonWidgetClass, button_form,
		XmNlabelString, 	xmstr,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		separator,
		XmNtopOffset, 		8,
               	XmNleftAttachment, 	XmATTACH_POSITION,
                XmNleftPosition, 	1,
                XmNrightAttachment, 	XmATTACH_POSITION,
                XmNrightPosition, 	2,
		XmNnavigationType, 	XmTAB_GROUP,
		XmNuserData, 		c,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(t->applybutton, XmNactivateCallback, tz_apply_cb, NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 923, "Cancel"));
	t->cancelbutton = XtVaCreateWidget("Cancel", 
		xmPushButtonWidgetClass, button_form,
		XmNlabelString, 	xmstr,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		separator,
		XmNtopOffset, 		8,
               	XmNleftAttachment, 	XmATTACH_POSITION,
                XmNleftPosition, 	2,
                XmNrightAttachment, 	XmATTACH_POSITION,
                XmNrightPosition, 	3,
		XmNnavigationType, 	XmTAB_GROUP,
		XmNuserData, 		c,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(t->cancelbutton, XmNactivateCallback, tz_cancel_cb, NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
	t->helpbutton = XtVaCreateWidget("Help", 
		xmPushButtonWidgetClass, button_form,
		XmNlabelString, 	xmstr,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		separator,
		XmNtopOffset, 		8,
               	XmNleftAttachment, 	XmATTACH_POSITION,
                XmNleftPosition, 	3,
                XmNrightAttachment, 	XmATTACH_POSITION,
                XmNrightPosition, 	4,
		XmNnavigationType, 	XmTAB_GROUP,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(t->helpbutton, XmNactivateCallback,
				(XtCallbackProc)help_cb, TIMEZONE_HELP_BUTTON);


	t->timezone_message = XtVaCreateWidget("message",
                xmLabelGadgetClass, t->form,
                XmNalignment, XmALIGNMENT_BEGINNING,
                XmNtopAttachment, XmATTACH_WIDGET,
                XmNtopWidget, t->helpbutton,
		XmNtopOffset, 8,
                XmNleftAttachment, XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNbottomAttachment, XmATTACH_FORM,
                NULL);

	/* set timezone */
	/* Remind - get this from the structure after it's available */
	cm_strcpy(t->gmttimezone, "");
	cm_strcpy(t->mytimezone, "US/Pacific");
	cm_strcpy(tmp_buf, (char*)getenv("TZ"));
	cm_strcpy(t->mytimezone, tmp_buf);

	sprintf(buf, "%s %s", catgets(c->DT_catd, 1, 659, "Time Zone:"),
		t->mytimezone);
	set_message(t->timezone_message, buf);
	set_message(c->message_text, buf);

	XtVaSetValues(t->form, 
			XmNdefaultButton, 	t->applybutton,
			XmNcancelButton, 	t->cancelbutton,
			NULL);
	XtVaSetValues(button_form, 
			XmNdefaultButton, 	t->applybutton,
			XmNcancelButton, 	t->cancelbutton,
			NULL);
        ManageChildren(t->timezone_rc_mgr);
        ManageChildren(button_form);
	ManageChildren(t->form);
	XtManageChild(t->form);
        XtVaSetValues(t->frame, XmNmappedWhenManaged, True, NULL);
	XtRealizeWidget(t->frame);

	XtPopup(t->frame, XtGrabNone);

	return(caddr_t)t;
}

extern void
refresh_timezone(Timezone *t)
{
    XmToggleButtonSetState(t->mytime, t->timezone_type == mytime, FALSE);
    XmToggleButtonSetState(t->customtime, t->timezone_type == customtime,FALSE);
    XtSetSensitive(t->gmtlabel, t->timezone_type == customtime);
    XtSetSensitive(t->gmtcombo, t->timezone_type == customtime);
    XtSetSensitive(t->hourlabel, t->timezone_type == customtime);

    if (t->timezone_type == mytime)
    {
	Widget text;
	XtVaGetValues(t->gmtcombo, XmNtextField, &text, NULL);
        XmTextSetString(text, "");
    }
}

void
tz_mytime_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Timezone *t;
        XmToggleButtonCallbackStruct *state =
                (XmToggleButtonCallbackStruct *) call_data;

	XtVaGetValues(widget, XmNuserData, &t, NULL);

	if (state->set)
		t->edit_timezone_type = mytime;
	XtSetSensitive(t->gmtlabel, !state->set);
	XtSetSensitive(t->gmtcombo, !state->set);
	XtSetSensitive(t->hourlabel, !state->set);
}

void
tz_customtime_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Timezone *t;
        XmToggleButtonCallbackStruct *state =
                (XmToggleButtonCallbackStruct *) call_data;

	XtVaGetValues(widget, XmNuserData, &t, NULL);

	if (state->set)
		t->edit_timezone_type = customtime;
	XtSetSensitive(t->gmtlabel, state->set);
	XtSetSensitive(t->gmtcombo, state->set);
	XtSetSensitive(t->hourlabel, state->set);
}

void
tz_set_timezone(Calendar *c, Timezone *t)
{
    char *tmp_buf, buf[BUFSIZ], gmt[BUFSIZ];
    Widget text;
    Dtcm_calendar *cal_handle;
    int time_diff;

    t->timezone_type = t->edit_timezone_type;
    if (t->timezone_type == mytime)
    {
	set_timezone(t->mytimezone);
	sprintf(buf, "%s %s", catgets(c->DT_catd, 1, 659, "Time Zone:"),
		t->mytimezone);

	/* get utc time */
	/* Remind: this won't work because of daylight savings time */
	/* Remind: add this after we can get more info from backend */
	/*
	  cal_handle = allocate_cal_struct(
					appt_read, 
			    		c->general->version,
					DtCM_ATTR_UTC_OFFSET_I, 
					NULL);
          if (query_cal_struct(c->my_cal_handle, cal_handle) == CSA_SUCCESS)
	    time_diff =
	      seconds_to_hours(cal_handle->attrs->value->item.number_value);
	  if (time_diff >= 0) sprintf(gmt, "GMT+%d", time_diff);
	  else sprintf(gmt, "GMT%d", time_diff);
	  set_timezone(gmt);
	*/

    }
    else if (t->timezone_type == customtime)
    {
	XtVaGetValues(t->gmtcombo, XmNtextField, &text, NULL);
        tmp_buf = XmTextGetString(text);
	cm_strcpy(t->gmttimezone, tmp_buf);
	sprintf(gmt, "GMT%s", tmp_buf);
	set_timezone(gmt);
	sprintf(buf, "%s %s", catgets(c->DT_catd, 1, 659, "Time Zone:"), gmt);
    }
    set_message(t->timezone_message, buf);
    paint_canvas(c, NULL, RENDER_CLEAR_FIRST);
    set_message(c->message_text, buf);
    if (editor_showing((Editor*)c->editor))
      add_all_appt((Editor*)c->editor);
    if (geditor_showing((GEditor*)c->geditor))
      add_all_gappt((GEditor*)c->geditor);
    if (todo_showing((ToDo*)c->todo))
      add_all_todo((ToDo*)c->todo);
}

void
tz_apply_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Calendar *c;
	Timezone *t;

	XtVaGetValues(widget, XmNuserData, &c, NULL);
	t = (Timezone*)c->timezone;
	tz_set_timezone(c, t);
}

 
void
tz_ok_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Calendar *c;
	Timezone *t;

	XtVaGetValues(widget, XmNuserData, &c, NULL);
	t = (Timezone*)c->timezone;
	tz_set_timezone(c, t);

	XtPopdown(t->frame);
}

void
tz_cancel_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Calendar *c;
	Timezone *t;

	XtVaGetValues(widget, XmNuserData, &c, NULL);
	t = (Timezone*)c->timezone;

	XtPopdown(t->frame);
}
