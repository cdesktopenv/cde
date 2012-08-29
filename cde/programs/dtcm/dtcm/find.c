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
/* $XConsortium: find.c /main/9 1996/11/21 19:42:36 drk $ */
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
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Dt/HourGlass.h>
#include "calendar.h"
#include "util.h"
#include "timeops.h"
#include "find.h"
#include "format.h"
#include "datefield.h"
#include "props.h"
#include "editor.h"
#include "select.h"
#include "help.h"
#include "getdate.h"

#define XOS_USE_XT_LOCKING
#define X_INCLUDE_TIME_H
#if defined(linux)
#undef SVR4
#endif
#include <X11/Xos_r.h>

static void layout_labels(Widget, Widget, Widget, Widget);

extern caddr_t
make_find(c)
Calendar *c;
{
	Find 		*f;
	Widget 		 separator1, separator2, button_form;
	XmString 	 xmstr;
	OrderingType 	 ot = get_int_prop((Props *)c->properties,
							CP_DATEORDERING);
	SeparatorType 	 sep = get_int_prop((Props *)c->properties, 
							CP_DATESEPARATOR);
	Tick		 cursor, begin_range, end_range;
	char		 buffer[50];
	int		 i;
	void 		 find_appts(), show_appt(), f_cancel_cb(), 
			 f_searchrange_cb(), f_searchall_cb();
	Arg		 args[3];
	Dimension	 highest;
	Widget	 	 highest_label;
	char		 *title;

	if (c->find == NULL) {
                c->find = (caddr_t)ckalloc(sizeof(Find));
                f = (Find*)c->find;
        }
        else
                f = (Find*)c->find;

	title = XtNewString(catgets(c->DT_catd, 1, 283, "Calendar : Find"));
	f->frame = XtVaCreatePopupShell("frame",
                xmDialogShellWidgetClass, 
		c->frame,
                XmNtitle, 		title,
                XmNallowShellResize, 	True,
		XmNmappedWhenManaged, 	False,
                NULL);
	XtFree(title);

        f->form = XtVaCreateWidget("form",
                xmFormWidgetClass, 
		f->frame,
                XmNautoUnmanage, 	False,
		XmNfractionBase, 	4,
		XmNmarginWidth, 	0,
		XmNmarginHeight, 	0,
                NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 284, "Find:"));
        f->apptstr_label = XtVaCreateWidget("label", 
		xmLabelWidgetClass, 
		f->form,
		XmNlabelString, 	xmstr,
                XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset,		10,
                XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset,		15,
                NULL);
	XmStringFree(xmstr);

        f->apptstr = XtVaCreateWidget("appt", 
		xmTextWidgetClass, 
		f->form,
                XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset,		10,
                XmNrightAttachment, 	XmATTACH_FORM,
		XmNrightOffset,		10,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		f->apptstr_label,
                NULL);

	f->search_rc_mgr = XtVaCreateWidget("search_rc_mgr",
                xmRowColumnWidgetClass, 
		f->form,
                XmNpacking, 		XmPACK_COLUMN,
                XmNorientation, 	XmVERTICAL,
                XmNradioBehavior, 	True,
                XmNisHomogeneous, 	True,
                XmNentryClass, 		xmToggleButtonGadgetClass,
		XmNmarginWidth,		0,
                XmNtopAttachment, 	XmATTACH_WIDGET,
                XmNtopWidget, 		f->apptstr,
                XmNleftAttachment, 	XmATTACH_OPPOSITE_WIDGET,
                XmNleftWidget, 		f->apptstr,
                XmNleftOffset, 		0,
                NULL);
 
        xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 973, "Search all"));
        f->search_all = XtVaCreateWidget("searchAll",
                xmToggleButtonGadgetClass, 
		f->search_rc_mgr,
		XmNlabelString, 	xmstr,
		XmNuserData, 		f,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(f->search_all, XmNvalueChangedCallback, f_searchall_cb, 
									NULL);
 
        xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 974, "Search from"));
        f->search_range = XtVaCreateWidget("searchrange",
                xmToggleButtonGadgetClass, 
		f->search_rc_mgr,
		XmNlabelString, 	xmstr,
		XmNuserData, 		f,
		XmNset, 		True,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(f->search_range, XmNvalueChangedCallback, 
							f_searchrange_cb, NULL);

	f->search_set = search_range;
 
	/* default beginning of range query to 6 months ago */

	cursor = now();
	for (i = 0; i < 6; i++)
		cursor = prevmonth_exactday(cursor);

	format_tick(cursor, ot, sep, buffer);

        f->search_from = XtVaCreateWidget("search_from", 
		xmTextWidgetClass, 
		f->form,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		f->search_rc_mgr,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		f->apptstr,
		XmNtopOffset, 		30, 
		XmNuserData, 		c,
		XmNvalue, 		buffer,
                NULL);
	XtSetSensitive(f->search_from, True);


	/* default end of range query to 6 from now */

	cursor = now();
	for (i = 0; i < 6; i++)
		cursor = nextmonth_exactday(cursor);

	format_tick(cursor, ot, sep, buffer);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 975, "to"));
        f->search_tolabel = XtVaCreateWidget("tolabel", 
		xmLabelWidgetClass, 
		f->form,
		XmNlabelString, 	xmstr,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		f->search_from,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		f->apptstr,
		XmNtopOffset, 		35, 
                NULL);
	XmStringFree(xmstr);

        f->search_to = XtVaCreateWidget("search_to", 
		xmTextWidgetClass, 
		f->form,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		f->search_tolabel,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		f->apptstr,
		XmNtopOffset, 		30, 
		XmNrightOffset,		10, 
		XmNuserData, 		c,
		XmNvalue, 		buffer,
                NULL);
	XtSetSensitive(f->search_to, True);

        separator1 = XtVaCreateWidget("separator1",
                xmSeparatorGadgetClass, 
		f->form,
                XmNleftAttachment, 	XmATTACH_FORM,
                XmNrightAttachment, 	XmATTACH_FORM,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		f->search_rc_mgr,
		XmNtopOffset, 		5,
                NULL);
 
	xmstr = XmStringCreateLocalized(
		catgets(c->DT_catd, 1, 848, "Date"));
        f->date_label = XtVaCreateWidget("finddatelabel", 
		xmLabelWidgetClass, 
		f->form,
		XmNlabelString, 	xmstr,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		separator1,
		XmNtopOffset, 		10,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		10,
                NULL);
	XmStringFree(xmstr);
	xmstr = XmStringCreateLocalized(
		catgets(c->DT_catd, 1, 849, "Time"));
        f->time_label = XtVaCreateWidget("findtimelabel", 
		xmLabelWidgetClass, 
		f->form,
		XmNlabelString, 	xmstr,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		separator1,
		XmNtopOffset, 		10,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		f->date_label,
                NULL);
	XmStringFree(xmstr);
	xmstr = XmStringCreateLocalized(
		catgets(c->DT_catd, 1, 850, "What"));
        f->what_label = XtVaCreateWidget("findwhatlabel", 
		xmLabelWidgetClass, 
		f->form,
		XmNlabelString, 	xmstr,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		separator1,
		XmNtopOffset, 		10,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		f->time_label,
                NULL);
	XmStringFree(xmstr);

	f->find_message = XtVaCreateWidget("message",
		xmLabelGadgetClass, 	f->form,
		XmNalignment, 		XmALIGNMENT_BEGINNING,
		XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset, 		10,
		XmNrightAttachment, 	XmATTACH_FORM,
		XmNbottomAttachment, 	XmATTACH_FORM,
		NULL);

        button_form = XtVaCreateWidget("print_button_form_mgr",
                xmFormWidgetClass,
                f->form,
                XmNautoUnmanage,        False,
                XmNfractionBase,        5,
		XmNhorizontalSpacing,	0,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNleftOffset,          5,
                XmNrightAttachment,     XmATTACH_FORM,
                XmNrightOffset,         5,
                XmNbottomAttachment,    XmATTACH_WIDGET,
                XmNbottomWidget,        f->find_message,
                XmNbottomOffset,        5,
                NULL);

        separator2 = XtVaCreateWidget("separator1",
                xmSeparatorGadgetClass, 
		f->form,
                XmNleftAttachment, 	XmATTACH_FORM,
                XmNrightAttachment, 	XmATTACH_FORM,
                XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	button_form,
		XmNbottomOffset, 	5,
                NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 285, "Find"));
        f->find_button = XtVaCreateWidget("findbutton", 
		xmPushButtonGadgetClass, 
		button_form,
		XmNlabelString, 	xmstr,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	0,
		XmNleftOffset, 		0,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	1,
		XmNrightOffset, 	0,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNshowAsDefault,	0,
                NULL);
	XtAddCallback(f->find_button, XmNactivateCallback, find_appts, NULL);
	XmStringFree(xmstr);
		
	xmstr = XmStringCreateLocalized(
				catgets(c->DT_catd, 1, 851, "Show Appointment"));
	f->show_button = XtVaCreateWidget("show", 
		xmPushButtonGadgetClass, 
		button_form,
		XmNlabelString, 	xmstr,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	1,
		XmNleftOffset, 		0,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	3,
		XmNrightOffset, 	0,
		XmNbottomAttachment,	XmATTACH_FORM,
                NULL);
	XmStringFree(xmstr);

	XtAddCallback(f->show_button, XmNactivateCallback, show_appt, NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 680, "Close"));
	f->cancel_button = XtVaCreateWidget("cancel", 
		xmPushButtonGadgetClass, 
		button_form,
		XmNlabelString, 	xmstr,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	3,
		XmNleftOffset, 		0,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	4,
		XmNrightOffset, 	0,
		XmNbottomAttachment,	XmATTACH_FORM,
                NULL);
	XmStringFree(xmstr);

	XtAddCallback(f->cancel_button, XmNactivateCallback, f_cancel_cb, NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
	f->help_button = XtVaCreateWidget("help", 
		xmPushButtonGadgetClass, 
		button_form,
		XmNlabelString, 	xmstr,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	4,
		XmNleftOffset, 		0,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	5,
		XmNrightOffset, 	0,
		XmNbottomAttachment,	XmATTACH_FORM,
                NULL);
        XtAddCallback(f->help_button, XmNactivateCallback, 
		(XtCallbackProc)help_cb, FIND_HELP_BUTTON);
        XtAddCallback(f->form, XmNhelpCallback, 
		(XtCallbackProc)help_cb, (XtPointer) FIND_HELP_BUTTON);
	XmStringFree(xmstr);

	XtSetArg(args[0], XmNlistSizePolicy, XmCONSTANT);
	XtSetArg(args[1], XmNvisibleItemCount, 5);
	XtSetArg(args[2], XmNscrollBarDisplayPolicy, XmSTATIC);

	f->find_list = (Widget)XmCreateScrolledList(f->form,
                "find_list", args, 3);
        f->find_list_sw = XtParent(f->find_list);

	_i18n_HighestWidget(3,&highest_label, &highest,
		f->date_label, f->time_label, f->what_label);

	XtVaSetValues(f->find_list_sw,
                XmNtopAttachment, 	XmATTACH_WIDGET,
                XmNtopWidget, 		highest_label,
                XmNleftAttachment, 	XmATTACH_FORM,
                XmNleftOffset, 		10,
                XmNrightAttachment, 	XmATTACH_FORM,
                XmNrightOffset, 	10,
                XmNbottomAttachment, 	XmATTACH_WIDGET,
		XmNbottomWidget, 	separator2,
		XmNbottomOffset, 	10,
                XmNlistSizePolicy, 	XmCONSTANT,
		NULL);

	XtManageChild(f->find_list);

	XtAddCallback(f->find_list, XmNdefaultActionCallback, show_appt, NULL);

	layout_labels(f->date_label, f->time_label, 
		      f->what_label, f->find_list);

        /* set default button */
        XtVaSetValues(button_form,
		XmNdefaultButton, 	f->find_button,
        	XmNcancelButton, 	f->cancel_button,
		NULL);
        XtVaSetValues(f->form,
		XmNdefaultButton, 	f->find_button,
        	XmNcancelButton, 	f->cancel_button,
		NULL);

/*
	XtManageChild(f->apptstr_label);
	XtManageChild(f->apptstr);
	XtManageChild(f->search_rc_mgr);
	XtManageChild(f->search_all);
	XtManageChild(f->search_range);
	XtManageChild(f->search_from);
	XtManageChild(f->search_to);
	XtManageChild(f->search_tolabel);
	XtManageChild(separator1);
	XtManageChild(f->date_label);
	XtManageChild(f->time_label);
	XtManageChild(f->what_label);
	XtManageChild(f->find_message);
	XtManageChild(button_form);
	XtManageChild(separator2);
	XtManageChild(f->find_list);
	XtManageChild(f->find_button);
	XtManageChild(f->show_button);
	XtManageChild(f->cancel_button);
	XtManageChild(f->help_button);
*/
        ManageChildren(f->search_rc_mgr);
	ManageChildren(button_form);
	ManageChildren(f->form);
	XtManageChild(f->form);
        XtVaSetValues(f->frame, XmNmappedWhenManaged, True, NULL);
	XtRealizeWidget(f->frame);

	XtPopup(f->frame, XtGrabNone);

	return(caddr_t)f;
}

static Tick
f_get_searchdate(Widget widget, Props *p)
{
	Calendar *c = calendar;
        OrderingType ot;
        SeparatorType st;
	char *buf;
	Tick new_date;

	ot = get_int_prop(p, CP_DATEORDERING);
        st = get_int_prop(p, CP_DATESEPARATOR);
	buf = get_date_from_widget(c->view->date, widget, ot, st);

	new_date = (Tick) cm_getdate(buf, NULL);

	return(new_date);
}

void
f_searchall_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Find *f;
	XmToggleButtonCallbackStruct *state =
		(XmToggleButtonCallbackStruct *) call_data;

	XtVaGetValues(widget, XmNuserData, &f, NULL);

	if (state->set)
		f->search_set = search_all;
	XtSetSensitive(f->search_to, False);
	XtSetSensitive(f->search_from, False);
}

void
f_searchrange_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
	Find *f;
	XmToggleButtonCallbackStruct *state =
		(XmToggleButtonCallbackStruct *) call_data;

	XtVaGetValues(widget, XmNuserData, &f, NULL);

	if (state->set)
		f->search_set = search_range;
	XtSetSensitive(f->search_to, True);
	XtSetSensitive(f->search_from, True);
}

static void
fmt_time_what(
	Dtcm_appointment 	*appt,
	char 			*buf,
	DisplayType		 display)
{
        int 			 hr, mn;
	time_t			 tick;
        Lines 			*lines;
        Boolean 		 am;
        struct tm 		*tm;
	char			 tmp[16];
	_Xltimeparams		 localtime_buf;
 
        if(!appt || !buf) return;
        _csa_iso8601_to_tick(appt->time->value->item.string_value, &tick);
        tm = _XLocaltime(&tick, localtime_buf);
        hr = tm->tm_hour;
        mn = tm->tm_min;
        if (showtime_set(appt) && !magic_time(tick)) {
                if (display == HOUR12) {
                        am = adjust_hour(&hr);
                        sprintf(tmp, "%2d:%02d%s  ", hr, mn, am ? "a" : "p");
                }
                else
                        sprintf(tmp, "%02d%02d    ", hr, mn);

		sprintf(buf, "%8s", tmp);
        } else
		sprintf(buf, "%8s", "");

        lines = text_to_lines(appt->what->value->item.string_value, 1);
        if (lines != NULL && lines->s != NULL) {
                (void) cm_strcat(buf, lines->s);
                destroy_lines(lines);
        }
}

void
find_appts(widget, client_data, cbs)
Widget widget;
XtPointer client_data;
XmPushButtonCallbackStruct *cbs;
{
	Calendar *c = calendar;
	Props *p = (Props*)c->properties;
	OrderingType ot = get_int_prop(p, CP_DATEORDERING);
	DisplayType dt = get_int_prop(p, CP_DEFAULTDISP);
	SeparatorType sep = get_int_prop(p, CP_DATESEPARATOR);
	Find *f = (Find*)c->find;
	int mos, i, j, range_count;
	char *location;
	Dimension w, h;
        char what_buf[WHAT_LEN+1], buf[WHAT_LEN+1], buf2[WHAT_LEN+1], message[40], *astr;
	XmString buf_str;
        int num_items, mo, last_match_total = 0, match_total = 0;
        pr_pos xy;
	Tick end_of_time, start, stop;
	Tick_list *ptr, *next_ptr, *tail_ptr, *new_tick;
	CSA_session_handle cal = NULL;
	CSA_return_code stat;
        CSA_entry_handle *entries = NULL;
	CSA_enum *ops;
        CSA_attribute *range_attrs;
	CSA_uint32 num_entries;
	Dtcm_appointment *appt;
	int comparison_length;
	Tick	real_eot = get_eot();
	_Xltimeparams localtime_buf;

	astr = XmTextGetString(f->apptstr);

        if (astr == NULL || *astr == NULL) {
		sprintf(message, "%s", catgets(c->DT_catd, 1, 290, "Specify Appt String to Match."));
		set_message(f->find_message, message);
                return;
        }
	XmListDeleteAllItems(f->find_list);
	set_message(f->find_message, " ");
	ptr = f->ticks;
	while (ptr != NULL) {
		next_ptr = ptr->next;
		free(ptr);
		ptr = next_ptr;
	}
	f->ticks = NULL;

	if (f->search_set == search_all) {
		end_of_time = real_eot;
		start = get_bot();
	}
	else {

		start = f_get_searchdate(f->search_from, p);

		if (start == DATE_BBOT)
			start = get_bot();
		else if (start == DATE_AEOT) {
                        sprintf(message, "%s", catgets(c->DT_catd, 1, 810, "Please enter a start date after 1/1/1970"));
                        set_message(f->find_message, message);
                        return;
                }
		else if (start <= 0) {
                        sprintf(message, "%s", catgets(c->DT_catd, 1, 811, "Malformed start date"));
                        set_message(f->find_message, message);
                        return;
                }


		end_of_time = f_get_searchdate(f->search_to, p);

		if (end_of_time == DATE_AEOT)
			end_of_time = real_eot;
		else if (end_of_time == DATE_BBOT) {
                        sprintf(message, "%s", catgets(c->DT_catd, 1, 812, "Please enter an end date before 1/1/2038"));
                        set_message(f->find_message, message);
                        return;
                }
		else if (end_of_time <= 0) {
                        sprintf(message, "%s", catgets(c->DT_catd, 1, 813, "Malformed end date"));
                        set_message(f->find_message, message);
                        return;
                }

		if (start >= end_of_time) {
			sprintf(message, "%s", catgets(c->DT_catd, 1, 713, "You must choose a begin date before the end date."));
                	set_message(f->find_message, message);
                	return;
        	}
		
		if ((end_of_time < 0) || (end_of_time > real_eot))
			end_of_time = real_eot;
	}

	stop = start + (4 * wksec);

	if (stop > end_of_time)
		stop = end_of_time;

	comparison_length = cm_strlen(astr);

	appt = allocate_appt_struct(appt_read,
				    c->general->version,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_SUMMARY_I,
                                    CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
				    NULL);

	_DtTurnOnHourGlass(f->frame);
        for (; stop <= end_of_time;) {
		setup_range(&range_attrs, &ops, &range_count, start, stop,
			    CSA_TYPE_EVENT, NULL, B_FALSE, c->general->version);
	        stat = csa_list_entries(c->cal_handle, range_count, range_attrs, ops, &num_entries, &entries, NULL);

        	if (stat != CSA_SUCCESS) {
			free_range(&range_attrs, &ops, range_count);
			_DtTurnOffHourGlass(f->frame);
                	return;
        	}

		for (i = 0; i < num_entries; i++) {
			stat = query_appt_struct(c->cal_handle, entries[i], appt);
                	if (stat != CSA_SUCCESS) {
				free_appt_struct(&appt);
				csa_free(entries);
				_DtTurnOffHourGlass(f->frame);
                        	return;
                	}

			for (j = 0; appt->what->value->item.string_value[j] != NULL; j++)
				if (strncasecmp(astr, &(appt->what->value->item.string_value[j]),
			     		comparison_length) == 0) {
					new_tick = (Tick_list *) ckalloc(sizeof(Tick_list));
					if (new_tick == NULL) {
						free_appt_struct(&appt);
						free_range(&range_attrs, &ops, range_count);
						csa_free(entries);
						_DtTurnOffHourGlass(f->frame);
						return;
					}
					new_tick->next = NULL;
					_csa_iso8601_to_tick(appt->time->value->item.date_time_value, &(new_tick->tick));
					if (f->ticks == NULL)
						f->ticks = new_tick;
					else
						tail_ptr->next = new_tick;
					tail_ptr = new_tick;
					match_total++;
					strcpy(buf, "");
					strcpy(buf2, "");
					strcpy(what_buf, "");
					strftime(buf, WHAT_LEN, "%h %e, %Y", 
					    _XLocaltime(
					      (const time_t *)&new_tick->tick,
					      localtime_buf));
                                	fmt_time_what(appt, what_buf, dt);
					sprintf (buf2, "%10s  %s", 
								buf, what_buf);
					buf_str = XmStringCreateLocalized(buf2);
					XmListAddItem(f->find_list, buf_str, 0);
					XmStringFree(buf_str);

					break;
                        	}  /* end if stmt */
		}  /* end for i = 0 loop */

		if (match_total != last_match_total) {
			if (match_total == 1)
				sprintf(message, catgets(c->DT_catd, 1, 631, "%d match found"), match_total);
			else if (match_total > 1)
				sprintf(message, catgets(c->DT_catd, 1, 292, "%d matches found"), match_total);

			set_message(f->find_message, message);
		}

		last_match_total = match_total;


		csa_free(entries);
		free_range(&range_attrs, &ops, range_count);

		if (stop == real_eot)
			break;

		start = stop + 1;
	
		if (start > end_of_time)
			break;

		stop = start + (4 * wksec);
		if ((stop > end_of_time) || (stop < 0))
			stop = end_of_time;
		

        }  /* end for range.end loop */

	if (match_total == 0)
		sprintf(message, "%s", catgets(c->DT_catd, 1, 291, "Appointment Not Found."));
	else if (match_total == 1)
		sprintf(message, catgets(c->DT_catd, 1, 631, "%d match found"), match_total);
	else
		sprintf(message, catgets(c->DT_catd, 1, 292, "%d matches found"), match_total);
	set_message(f->find_message, message);
	free_appt_struct(&appt);
	_DtTurnOffHourGlass(f->frame);

        XtFree(astr);
        return;
}

void
show_appt(widget, client_data, cbs)
Widget widget;
XtPointer client_data;
XmPushButtonCallbackStruct *cbs;
{
	Calendar *c = calendar;

	Props *p = (Props*)c->properties;
	Find *f = (Find*)c->find;
	int *pos_list;
	int pos_cnt;
	int i;
	Dimension w, h;
	Tick_list *ptr;
	char buf[BUFSIZ];
	int list_cnt;

	XtVaGetValues(f->find_list, XmNitemCount, &list_cnt, NULL);
	if (list_cnt == 0) {
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 714, "There are no appointments in the list.  You must find one before showing an appointment."));
		set_message(f->find_message, buf);
		return;
	}
	if (!XmListGetSelectedPos(f->find_list, &pos_list, &pos_cnt)) {
		sprintf(buf, "%s", catgets(c->DT_catd, 1, 632, "Please select an appointment from the list to show"));
		set_message(f->find_message, buf);
		return;
	}
	set_message(f->find_message, " ");
	XtVaGetValues(c->canvas, XmNheight, &h, XmNwidth, &w, NULL);
	gr_clear_area(c->xcontext, 0, 0, w, h);

	c->view->olddate = c->view->date;
	for (ptr = (Tick_list*)f->ticks, i = 1; ptr != NULL && i < *pos_list; i++)
		ptr = ptr->next;

	c->view->date = ptr->tick;
	c->view->nwks = numwks(c->view->date);
	XtFree((XtPointer)pos_list);
	invalidate_cache(c);
	paint_canvas(c, NULL, RENDER_UNMAP);
	calendar_deselect(c);
	paint_selection(c);

}

void
f_cancel_cb(
        Widget  widget,
        XtPointer client_data,
        XmPushButtonCallbackStruct *cbs)
{
        Calendar *c = calendar;
        Find      *f;

        f = (Find *)c->find;

        XmTextSetString(f->apptstr, "");
        set_message(f->find_message, " ");
	XmListDeleteAllItems(f->find_list);

        XtPopdown(f->frame);

        return;
}

static void
layout_labels(
	Widget	l1, 
	Widget  l2,
	Widget  l3,
	Widget  list)
{
	int		list_char_width;
	Dimension	label_width;
	XmString	xmstr;
	XmFontList	font_list;

	xmstr = XmStringCreateLocalized(" ");

	XtVaGetValues(list, XmNfontList, &font_list, NULL);

	list_char_width = XmStringWidth(font_list, xmstr);

	XtVaGetValues(l1, XmNwidth, &label_width, NULL);

	/*
	 * The ``Time'' label.
	 */
	XtVaSetValues(l2,
		XmNleftOffset,	(14 * list_char_width) - label_width,
		NULL);

	XtVaGetValues(l2, XmNwidth, &label_width, NULL);

	/*
	 * The ``What'' label.
	 */
	XtVaSetValues(l3,
		XmNleftOffset,	(8 * list_char_width) - label_width,
		NULL);

	XmStringFree(xmstr);
}

