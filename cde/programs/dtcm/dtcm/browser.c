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
/* $TOG: browser.c /main/10 1999/02/23 09:42:01 mgreess $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <EUSCompat.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <csa.h>
#include <X11/Xlib.h>
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/DialogS.h>
#include <Xm/PanedW.h>
#include <Xm/Protocols.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/SashP.h>
#include <Dt/HourGlass.h>
#include <Tt/tttk.h>
#include "getdate.h"
#include "calendar.h"
#include "util.h"
#include "misc.h"
#include "timeops.h"
#include "browser.h"
#include "blist.h"
#include "format.h"
#include "datefield.h"
#include "props.h"
#include "props_pu.h"
#include "editor.h"
#include "group_editor.h"
#include "select.h"
#include "help.h"
#ifdef FNS
#include "cmfns.h"
#endif

#define DATESIZ 40

extern int debug;

static void mb_resize_proc(Widget, XtPointer, XtPointer);
static void cancel_cb(Widget, XtPointer, XtPointer);
static void popup_cb(Widget, XtPointer, XtPointer);
static void init_browser(Calendar *);
static void bcanvas_repaint(Widget, XtPointer, XtPointer);
static void bcanvas_event(Widget, XtPointer, XtPointer);
static void goto_date_cb(Widget, XtPointer, XtPointer);
static void browselist_from_browser(Widget, XtPointer, XtPointer);
static void gotomenu_cb(Widget, XtPointer, XtPointer);
static void schedule_cb(Widget, XtPointer, XtPointer);
static void mail_cb(Widget, XtPointer, XtPointer);
static void mb_box_notify(Widget, XtPointer, XtPointer);
static void mb_update_handler(CSA_session_handle, CSA_flags, CSA_buffer,
			      CSA_buffer, CSA_extension *);

extern void scrub_attr_list(Dtcm_appointment *);

static void
mb_init_array(Browser *b, int begin, int end) {
        b->segs_in_array = BOX_SEG * (end - begin) * 7;
        b->multi_array = (char*)ckalloc(b->segs_in_array);
}

static void
reset_ticks(Calendar *c, Boolean use_sel_idx) {
	int	beg, end;
	Props	*p = (Props *)c->properties;
        Browser	*b = (Browser*)c->browser;

	beg = get_int_prop(p, CP_DAYBEGIN);
	end = get_int_prop(p, CP_DAYEND);

	if (b->date <= get_bot()) {
		b->date = get_bot();
		if (b->col_sel > 0)
			b->col_sel = b->col_sel - 4;
	}
	if ((b->begin_week_tick = first_dow(b->date)) < get_bot())
		b->begin_week_tick = get_bot();
	if (use_sel_idx) {
		b->begin_day_tick =
			next_ndays(b->begin_week_tick, b->col_sel);
		b->begin_hr_tick =
			next_nhours(b->begin_day_tick, beg + b->row_sel);
	} else {
		b->begin_day_tick = lowerbound(b->date);
		b->begin_hr_tick = next_nhours(b->begin_day_tick, beg);
	}
        b->end_day_tick = upperbound(b->begin_day_tick);
        b->end_hr_tick = next_nhours(b->begin_hr_tick, 1);
}

extern void
br_display(Calendar *c) {
	int		i, *pos_list = NULL, pos_cnt;
        Browser		*b = (Browser *)c->browser;
	BlistData	*bd;
	Browselist	*bl = (Browselist *)c->browselist;
	void		mb_update_array();

	if (!b)
		return;

        for (i = 0; i < b->segs_in_array; i++)
		b->multi_array[i] = 0;
        b->add_to_array = True;

	XmListGetSelectedPos(b->browse_list, &pos_list, &pos_cnt);
	for (i = 0; i < pos_cnt; i++) {
		bd = (BlistData *)CmDataListGetData(bl->blist_data,
						    pos_list[i]);

		destroy_paint_cache(bd->cache, bd->cache_size);
		bd->cache = NULL;
		bd->cache_size = 0;

		if (bd)
			mb_update_array(bd->name, c);
	}
	if (pos_list)
		XtFree((XtPointer)pos_list);

        mb_refresh_canvas(b, c);
}

static void
invalid_date_msg(Calendar *c, Widget widget)
{
	Browser *b = (Browser*)c->browser;
	char *title = XtNewString(catgets(c->DT_catd, 1, 1070, 
				  "Calendar : Error - Compare Calendars"));
	char *text = XtNewString(catgets(c->DT_catd, 1, 20,
					 "Invalid Date In Go To Field."));
	char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));

	dialog_popup(b->frame,
		DIALOG_TITLE, title,
		DIALOG_TEXT, text,
		BUTTON_IDENT, 1, ident,
		DIALOG_IMAGE, ((Props_pu *)c->properties_pu)->xm_error_pixmap,
		NULL);

	XtFree(ident);
	XtFree(text);
	XtFree(title);
}

void 
set_entry_date(Calendar *c) {
        char *date = NULL;
        Browser *b;
        Props *p;
        OrderingType ot;
        SeparatorType st;
	Tick tick;

        b = (Browser*)c->browser;
        p = (Props*)c->properties;
        ot = get_int_prop(p, CP_DATEORDERING);
        st = get_int_prop(p, CP_DATESEPARATOR);

        date = get_date_from_widget(c->view->date, b->datetext, ot, st);
    	if (date == NULL ) {
		invalid_date_msg(c, b->message_text);
                return;
        }

        tick = cm_getdate(date, NULL);
	if (!timeok(tick)) {
		invalid_date_msg(c, b->message_text);
		return;
	}

        b->date = tick;
        reset_ticks(c, False);
        br_display(c);
}

/*
 * A note about the browser:
 * This custom dialog is built using two nested PanedWindowWidgets.
 * The first child of the outer pane is itself paned (the other
 * child is the dialog's action area).  The inner pane is movable
 * to allow users to reproportion the calendar list versus the free
 * time chart.  Most of the useful widget handles are stored in the
 * Browser structure allocated and returned from here.
 */
extern void
make_browser(Calendar *c)
{
	Browser 	*b;
	Props 		*p = (Props*) c->properties;
	Widget 		separator1;
	Dimension 	w, h, height;
	XmString 	xmstr;
	XmString goto_label, prev_week, this_week, next_week, prev_month, next_month;
	int 		num_children;
	Widget 		*children;
        OrderingType 	ord_t = get_int_prop(p, CP_DATEORDERING);
        SeparatorType 	sep_t = get_int_prop(p, CP_DATESEPARATOR);
	char 		buf[BUFSIZ];
	Widget		text_field_form;
	int		outpane_width, outpane_height;
	int		upform_min,item_count;
	char		*title;

	if (c->browser == NULL) {
                c->browser = (caddr_t)ckalloc(sizeof(Browser));
                b = (Browser*)c->browser;
        }
        else
                b = (Browser*)c->browser;

	b->date = c->view->date;

        mb_init_array(b, get_int_prop(p, CP_DAYBEGIN), get_int_prop(p, CP_DAYEND));
        b->current_selection = (caddr_t) ckalloc(sizeof(Selection));

	/* if the screen is small adjust the max size for width and height
	 * so the shell can be moved up and down using window facility
	 */
	if ((WidthOfScreen(XtScreen(c->frame)) < 360) ||
	    (HeightOfScreen(XtScreen(c->frame)) < 600 ))
	{
		outpane_width = 300;
		outpane_height = 430;
		item_count	= 3;
		upform_min	= 120;
	}
	else
	{
		outpane_width = 360;
		outpane_height = 600;
		item_count	= 8;
		upform_min	= 200;
	}

	title = XtNewString(catgets(c->DT_catd, 1, 1010, 
					"Calendar : Compare Calendars"));
	b->frame = XtVaCreatePopupShell("frame",
                xmDialogShellWidgetClass, c->frame,
                XmNtitle, title,
                XmNallowShellResize, True,
                XmNmappedWhenManaged, False, 
		XmNdeleteResponse, XmDO_NOTHING,
                NULL);
	XtFree(title);
	XtAddCallback(b->frame, XmNpopupCallback, popup_cb, (XtPointer)c);


	/*
	 * Create the outer pane, whose upper part will hold a
	 * nested pane, and whose lower part will hold the actions
	 * and message area
	 */
	b->outer_pane = XtVaCreateManagedWidget("outerPane",
		xmPanedWindowWidgetClass, b->frame,
		XmNsashHeight, 1,
		XmNsashWidth, 1,
		XmNwidth, outpane_width,
		XmNheight, outpane_height,
		NULL);

	b->inner_pane = XtVaCreateManagedWidget("innerPane",
		xmPanedWindowWidgetClass, b->outer_pane,
		XmNallowResize, True,
		NULL);

	b->upper_form = XtVaCreateManagedWidget("upperForm",
		xmFormWidgetClass, b->inner_pane,
		XmNallowResize, True,
		XmNpaneMinimum, upform_min,
		NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 22, "Browse Menu Items"));
        b->list_label = XtVaCreateWidget("browseMenuLabel", 
		xmLabelGadgetClass, b->upper_form,
		XmNlabelString, xmstr,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 10,
                NULL);
	XmStringFree(xmstr);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 21, "Edit List..."));
        b->edit_list = XtVaCreateWidget("editList",
		xmPushButtonGadgetClass, b->upper_form,
		XmNlabelString, xmstr,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, b->list_label,
		XmNleftOffset, 80,
                NULL);
	XmStringFree(xmstr);

	XtAddCallback(b->edit_list, XmNactivateCallback,
		browselist_from_browser, (XtPointer)c);

	b->browse_list = (Widget)XmCreateScrolledList(b->upper_form,
                "browseList", NULL, 0);
        XtAddCallback(b->browse_list,
		XmNmultipleSelectionCallback, mb_box_notify, (XtPointer)c);
        XtAddCallback(b->browse_list,
		XmNdefaultActionCallback, mb_box_notify, (XtPointer)c);
	XtVaSetValues(b->browse_list,
		XmNselectionPolicy, XmMULTIPLE_SELECT,
               	XmNvisibleItemCount, item_count,
		NULL);

        b->browse_list_sw = XtParent(b->browse_list);
	XtVaSetValues(b->browse_list_sw,
                XmNtopAttachment, XmATTACH_WIDGET,
                XmNtopWidget, b->edit_list,
                XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, 20,
                XmNrightAttachment, XmATTACH_FORM,
		XmNrightOffset, 20,
		XmNbottomAttachment, XmATTACH_FORM,
                XmNvisualPolicy, XmVARIABLE,
		NULL);


	/*
	 * Create the "go to" option menu for time navigation
	 */
	prev_week =
	   XmStringCreateLocalized(catgets(c->DT_catd, 1, 23, "Prev Week"));
	this_week =
	   XmStringCreateLocalized(catgets(c->DT_catd, 1, 24, "This Week"));
	next_week =
	   XmStringCreateLocalized(catgets(c->DT_catd, 1, 25, "Next Week"));
	prev_month =
	   XmStringCreateLocalized(catgets(c->DT_catd, 1, 26, "Prev Month"));
	next_month =
	   XmStringCreateLocalized(catgets(c->DT_catd, 1, 27, "Next Month"));
	goto_label =
	   XmStringCreateLocalized(catgets(c->DT_catd, 1, 28, "Go To:"));

	/*
	 * remember - this returns a RowColumn widget!
	 */
	b->gotomenu = XmVaCreateSimpleOptionMenu(b->upper_form,
                "goToOptionMenu", goto_label, 0, 0, gotomenu_cb,
                XmVaPUSHBUTTON, prev_week, NULL, NULL, NULL,
                XmVaPUSHBUTTON, this_week, NULL, NULL, NULL,
                XmVaPUSHBUTTON, next_week, NULL, NULL, NULL,
                XmVaPUSHBUTTON, prev_month, NULL, NULL, NULL,
                XmVaPUSHBUTTON, next_month, NULL, NULL, NULL,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 5,
                XmNleftAttachment, XmATTACH_FORM,
		XmNleftOffset, 20,
		XmNuserData, c,
		XmNnavigationType, XmTAB_GROUP,
                NULL);
	XmStringFree(prev_week);
	XmStringFree(this_week);
	XmStringFree(next_week);
	XmStringFree(prev_month);
	XmStringFree(next_month);
	XmStringFree(goto_label);

	text_field_form = XtVaCreateManagedWidget("text_field_form",
		xmFormWidgetClass, b->upper_form,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 5,
                XmNleftAttachment, XmATTACH_WIDGET,
                XmNleftWidget, b->gotomenu,
		NULL);

	b->datetext = XtVaCreateWidget("dateText",
		xmTextWidgetClass, text_field_form,
                NULL);
	XtAddCallback(b->datetext, XmNactivateCallback, goto_date_cb, 
								(XtPointer)c);

	/*
	 * We can now calc the proper offset for the bottom of scrolled
	 * list - allow for a small margin above and below the text field.
	 */
	XtVaGetValues(b->datetext, XmNheight, &height, NULL);
	XtVaSetValues(b->browse_list_sw, XmNbottomOffset, (height + 10), NULL);

	b->lower_form = XtVaCreateManagedWidget("lowerForm",
		xmFormWidgetClass, b->inner_pane,
		XmNallowResize, True,
		XmNpaneMinimum, 200,
		XmNtraversalOn, False,
		NULL);

	/*
	 * create drawing area for chart
	 */
        b->canvas = XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass,
                b->lower_form,
                XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNtraversalOn, False,
                NULL);
       	b->xcontext = gr_create_xcontext(c, b->canvas, gr_color,
		c->xcontext->app);

        XtVaSetValues(b->canvas, XmNheight, 300, NULL);

        XtAddCallback(b->canvas, XmNresizeCallback, mb_resize_proc, (XtPointer)c);
        XtAddCallback(b->canvas, XmNinputCallback, bcanvas_event, (XtPointer)c);
        XtAddCallback(b->canvas, XmNexposeCallback, bcanvas_repaint, (XtPointer)c);

	/*
	 * Create action area of the dialog
	 */
	b->action = XtVaCreateWidget("action",
		xmFormWidgetClass, b->outer_pane,
		NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 29, "Schedule..."));
       	b->schedule = XtVaCreateWidget("schedule",
		xmPushButtonGadgetClass, b->action,
		XmNlabelString, xmstr,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(b->schedule, XmNactivateCallback, schedule_cb, (XtPointer)c);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 30, "Mail..."));
       	b->mail = XtVaCreateWidget("mail",
		xmPushButtonGadgetClass, b->action,
		XmNlabelString, xmstr,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, b->schedule,
		XmNtopAttachment, XmATTACH_FORM,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(b->mail, XmNactivateCallback, mail_cb, (XtPointer)c);

	XtSetSensitive(b->mail, c->tt_procid == NULL ? False : True);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 923, "Cancel"));
       	b->cancel = XtVaCreateWidget("cancel",
		xmPushButtonGadgetClass, b->action,
		XmNlabelString, xmstr,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, b->mail,
		XmNtopAttachment, XmATTACH_FORM,
                NULL);
	XmStringFree(xmstr);
	XtAddCallback(b->cancel, XmNactivateCallback, cancel_cb, (XtPointer)c);

        xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
        b->helpbutton = XtVaCreateWidget("help",
		xmPushButtonGadgetClass, b->action,
                XmNlabelString, xmstr,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, b->cancel,
		XmNtopAttachment, XmATTACH_FORM,
                NULL);
        XmStringFree(xmstr);
        XtAddCallback(b->helpbutton, XmNactivateCallback,
                (XtCallbackProc)help_cb, COMPARE_CALS_HELP_BUTTON);
        XtAddCallback(b->action, XmNhelpCallback,
                (XtCallbackProc)help_cb, (XtPointer) COMPARE_CALS_HELP_BUTTON);

	b->message_text = XtVaCreateWidget("message",
		xmLabelGadgetClass, b->action,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, b->schedule,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNalignment, XmALIGNMENT_BEGINNING,
		NULL);

	/*
	 * Fix the size of action area
	 */
	XtVaGetValues(b->schedule, XmNheight, &height, NULL);
	XtVaSetValues(b->action,
		XmNpaneMaximum, (2*height),
		XmNpaneMinimum, (2*height),
		NULL);


	XtVaGetValues(b->outer_pane,
		XmNchildren, &children,
		XmNnumChildren, &num_children,
		NULL);

	while (num_children-- > 0) {
		if (XmIsSash(children[num_children])) {
				XtVaSetValues(children[num_children], XmNtraversalOn, False, NULL);
		}
	}

	XtManageChild(b->edit_list);
	XtManageChild(b->list_label);
	XtManageChild(b->browse_list);
	XtManageChild(b->gotomenu);
	XtManageChild(b->datetext);
	XtManageChild(b->schedule);
	XtManageChild(b->mail);
	XtManageChild(b->cancel);
	XtManageChild(b->helpbutton);
	XtManageChild(b->message_text);
	XtManageChild(b->action);

	gr_init(b->xcontext, b->canvas);

        format_tick(b->date, ord_t, sep_t, buf);
	XmTextSetString(b->datetext, buf);

	set_entry_date(c);

        /*
	 * set default button for dialog
	 */
        XtVaSetValues(b->action, XmNdefaultButton, b->schedule, NULL);
        XtVaSetValues(b->upper_form, XmNdefaultButton, b->schedule, NULL);

	/* We don't want a ``return'' in the text field to trigger the
	 * default action so we create a form around the text field and
	 * designate the text-field as the default button.
	 */
        XtVaSetValues(text_field_form, XmNdefaultButton, b->datetext, NULL);

        XtVaSetValues(b->action, XmNcancelButton, b->cancel, NULL);
        XtVaSetValues(b->upper_form, XmNcancelButton, b->cancel, NULL);
        XmProcessTraversal(b->schedule, XmTRAVERSE_CURRENT);
        XtVaSetValues(b->action, XmNinitialFocus, b->schedule, NULL);

        XtVaSetValues(b->frame, XmNmappedWhenManaged, True, NULL);
	XtRealizeWidget(b->frame);
	/*
	 * Enforce a "reasonable" size.
	 * too narrow and the buttons are erased
	 * too short and the chart will look smashed
	XtVaSetValues(b->frame,
		XmNminWidth, 300,
		XmNminHeight, 600,
		NULL);
	 */

	/*
	* Add a WM protocol callback to handle the
	* case where the window manager closes the dialog.
	* Pass the calendar ptr through client data to allow
	* the callback to get at the shell and destroy it.
	*/
	setup_quit_handler(b->frame, cancel_cb, (XtPointer)c);

	init_browser(c);
}

static void 
browselist_from_browser(Widget w, XtPointer client_data, XtPointer call_data)
{
        Calendar *c = (Calendar *)client_data;
        Browselist	*bl;

        bl = (Browselist *)c->browselist;

	show_browselist(c);
}

static void
goto_date_cb(Widget w, XtPointer client_data, XtPointer call_data) 
{
        Calendar *c = (Calendar *)client_data;
	Browser *b;

	set_entry_date(c);
}

static void
goto_unit(Calendar *c, int item_no)
{
        char buf[DATESIZ];
        Browser *b;
        Props *p;
        OrderingType    ord_t;
        SeparatorType   sep_t;

        b = (Browser*)c->browser;
        p = (Props*)c->properties;

	switch (item_no + 1) {
	case MB_PREVWEEK:
		b->date = last_ndays(b->date, 7);
		break;
	case MB_THISWEEK:
		b->date = now();
		break;
	case MB_NEXTWEEK:
		b->date = next_ndays(b->date, 7);
		break;
	case MB_NEXTMONTH:
		b->date = next_ndays(b->date, 28);
		break;
	case MB_PREVMONTH:
		b->date = last_ndays(b->date, 28);
		break;
	default:
		b->date = now();
		break;
	}

        reset_ticks(c, False);

        ord_t = get_int_prop(p, CP_DATEORDERING);
        sep_t = get_int_prop(p, CP_DATESEPARATOR);

        format_tick(b->date, ord_t, sep_t, buf);
	XmTextSetString(b->datetext, buf);
        br_display(c);
}

static void
bcanvas_repaint(Widget w, XtPointer client_data, XtPointer call_data)
{
        Calendar *c = (Calendar *)client_data;
	Browser *b;
	XRectangle clip;
	XEvent ev;
        new_XContext    *xc;
	XmDrawingAreaCallbackStruct *cbs = (XmDrawingAreaCallbackStruct*)call_data;

	/* repair in full on final exposure (crude optimization scheme) */
	if (cbs->event->xexpose.count != 0)
	   return;

	b = (Browser *)c->browser;
        if ((xc = b->xcontext)==NULL)
                return;

	clip.x = 0;
	clip.y = 0;
	XtVaGetValues(w, XmNwidth, &clip.width, XmNy, &clip.height, NULL);

        gr_set_clip_rectangles(xc, 0, 0, &clip, 1, Unsorted);

        gr_clear_area(b->xcontext, 0, 0, b->canvas_w, b->canvas_h);
        gr_clear_clip_rectangles(xc);
	mb_refresh_canvas(b, c);
 
        XSync(XtDisplay(b->canvas), 0);
}

static void
mb_display_footermess(Browser *b, Calendar *c)
{
        int num_cals;
        char buf[BUFSIZ];

	XtVaGetValues(b->browse_list, XmNselectedItemCount, &num_cals, NULL);
        if (num_cals == 1)
                sprintf(buf, "%d %s", num_cals,
			catgets(c->DT_catd, 1, 31, "Calendar Displayed"));
        else
                sprintf(buf,  "%d %s", num_cals,
			catgets(c->DT_catd, 1, 32, "Calendars Displayed"));
	set_message(b->message_text, buf);
}

static void
browser_to_gaccess_list(Calendar *c) {
	int		i, pos_cnt, *pos_list = NULL;
	GEditor		*ge = (GEditor *)c->geditor;
	Browser		*b = (Browser *)c->browser;
	BlistData	*bd;
	Browselist	*bl = (Browselist *)c->browselist;

	/*
	 *  Add selected items to list box in group appt editor
	 */
	XmListGetSelectedPos(b->browse_list, &pos_list, &pos_cnt);
	for (i = 0; i < pos_cnt; i++) {
		if (bd = (BlistData *)CmDataListGetData(bl->blist_data,
							pos_list[i]))
			add_to_gaccess_list(bd->name, bd->cal_handle,
				bd->user_access, bd->version, ge, True);
	}
	add_all_gappt(ge);
	if (pos_cnt > 0)
		XtFree((XtPointer)pos_list);
}

void mb_update_busystatus(Browser *b, Calendar *c)
{
	int			i, j, r_cnt;
	CSA_uint32		num_entries;
	char			buf[BUFSIZ + 5];
	Boolean 		match = False;
	XmString		xmstr;
        Browselist 		*bl = (Browselist *)c->browselist;
	time_t			start, stop;
	CSA_entry_handle	*entries = NULL;
	BlistData 		*bd;
	CSA_return_code		stat;
	CSA_enum 		*ops;
	CSA_attribute 		*range_attrs;
	Tick			start_tick, end_tick;

	j = 1;
	while (bd = (BlistData *)CmDataListGetData(bl->blist_data, j)) {
		if (!XmListPosSelected(b->browse_list, j++))
			continue;

		sprintf(buf, "  %s", bd->name);
		start = b->begin_day_tick;
		stop = b->end_hr_tick;

		if (bd->cache == NULL) {
			setup_range(&range_attrs, &ops, &r_cnt, start, stop, 
					CSA_TYPE_EVENT, 0, B_FALSE, bd->version);
	        	stat = csa_list_entries(bd->cal_handle, r_cnt, range_attrs, ops, &num_entries, &entries, NULL);
        			free_range(&range_attrs, &ops, r_cnt);
        		backend_err_msg(b->frame, bd->name, stat,
                        		((Props_pu *)c->properties_pu)->xm_error_pixmap);
        		if (stat != CSA_SUCCESS) {
				csa_free(entries);
                		return;
			}

			allocate_paint_cache(entries, num_entries, &bd->cache);
			bd->cache_size = num_entries;
			csa_free(entries);
		}

        	for (i = 0; i < bd->cache_size; i++) {

			start_tick = bd->cache[i].start_time;
			end_tick = bd->cache[i].end_time;
			if ((start_tick+1 <= b->end_hr_tick) && 
		    	    (end_tick-1 
				>= b->begin_hr_tick)) {
				buf[0] = '*';
				break;
			}

		} /* end for */

		xmstr = XmStringCreateLocalized(buf);
		XmListDeletePos(b->browse_list, j-1);
		XmListAddItem(b->browse_list, xmstr, j-1);
		XmListSelectPos(b->browse_list, j-1, False);
		XmStringFree(xmstr);
	} /* end while */
}

static void
bcanvas_event(Widget w, XtPointer client_data, XtPointer call_data)
{
        Calendar *c = (Calendar *)client_data;
	Browser *b;
	XmDrawingAreaCallbackStruct *cbs = (XmDrawingAreaCallbackStruct*) call_data;
        XEvent  *event = cbs->event;
	Tick start;
        static XEvent lastevent;
	int x, y, boxw, boxh;
	pr_pos xy;

        if ((event->type == ButtonRelease) || (event == NULL))
                return;

	b = (Browser*)c->browser;

        x       = event->xbutton.x;
        y       = event->xbutton.y;

	switch(event->type) {
	case ButtonPress:
                if (x > b->chart_x && y > b->chart_y &&
                        x < (b->chart_x + b->chart_width)
                        && y < (b->chart_y + b->chart_height))
                {
                        browser_deselect(c, b);
                        b->col_sel = (x - b->chart_x) / b->boxw;
                        b->row_sel = (y - b->chart_y) / b->boxh;
                        xy.x = b->col_sel;
                        xy.y = b->row_sel;

			/*
			 *  Don't bring up multi-browser for an invalid date
			 */
			if ((last_ndays(b->date, 2) <= get_bot()) &&
			    (b->col_sel < 3))
				return;
			else if ((next_ndays(b->date, 1) > get_eot()) &&
				 (b->col_sel > 3))
				return;
 
                        reset_ticks(c, True);
                        browser_select(c, b, &xy);
                        if (ds_is_double_click(&lastevent, event)) {
				_DtTurnOnHourGlass(b->frame);
				show_geditor(c, b->begin_hr_tick,
					     b->end_hr_tick);
				browser_to_gaccess_list(c);
				_DtTurnOffHourGlass(b->frame);
                        }
			/* add busyicon */
			mb_update_busystatus(b, c);
		}
                c->general->last_canvas_touched = browser;
        };
        lastevent = *event;
}

static void
schedule_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
        Calendar *c = (Calendar *)client_data;
	Browser *b;
	Props *p;
        OrderingType    ord_t;
        SeparatorType   sep_t;
	XmPushButtonCallbackStruct *cbs = (XmPushButtonCallbackStruct*) call_data;

	b = (Browser *)c->browser;
	p = (Props *)c->properties;
	_DtTurnOnHourGlass(b->frame);
        ord_t = get_int_prop(p, CP_DATEORDERING);
        sep_t = get_int_prop(p, CP_DATESEPARATOR);
	show_geditor(c, b->begin_hr_tick, b->end_hr_tick);
	browser_to_gaccess_list(c);
	_DtTurnOffHourGlass(b->frame);
}


static char *
get_mail_address_list(Calendar *c) {
	int		i, *pos_list = NULL, pos_cnt;
        Browser		*b = (Browser *)c->browser;
	BlistData	*bd;
	Browselist	*bl = (Browselist *)c->browselist;
	int		address_len;
	char		*address;


	XmListGetSelectedPos(b->browse_list, &pos_list, &pos_cnt);
	for (i = 0, address_len = 0; i < pos_cnt; i++) {
		bd = (BlistData *)CmDataListGetData(bl->blist_data,
						    pos_list[i]);
		if (bd)
			address_len += strlen(bd->name) + 1;
	}

	address = calloc(address_len+1, 1);
	address[0] = '\0';

	for (i = 0; i < pos_cnt; i++) {
		bd = (BlistData *)CmDataListGetData(bl->blist_data,
						    pos_list[i]);
		if (bd) {
			strcat(address, bd->name);
			strcat(address, " ");
		}
	}
	if (pos_list)
		XtFree((XtPointer)pos_list);

	return(address);
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

static void
mail_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
        Calendar 	*c = (Calendar *)client_data;
	Props		*p = (Props *) c->properties;
        Browser		*b = (Browser *)c->browser;
        Tt_message      msg;
        Tt_status       status;
        char            *appointment_buf;
	Dtcm_appointment        *appt;
	char		*address = get_mail_address_list(c);
	char		*address_list[1];
	char		*mime_buf;
 
        /* Send ToolTalk message to bring up compose GUI with buffer as attachme
nt */

	appt = allocate_appt_struct(appt_write, DATAVER_ARCHIVE, NULL);
	load_appt_defaults(appt, p);

	appt->time->value->item.date_time_value = malloc(BUFSIZ);
	_csa_tick_to_iso8601(b->begin_hr_tick, appt->time->value->item.date_time_value);
	appt->end_time->value->item.date_time_value = malloc(BUFSIZ);
	_csa_tick_to_iso8601(b->end_hr_tick, appt->end_time->value->item.date_time_value);

	/* set up the start time from the dialog */

	scrub_attr_list(appt);
 
        appointment_buf = parse_attrs_to_string(appt, p, attrs_to_string(appt->attrs, appt->count));

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

static void
gotomenu_cb(Widget w, XtPointer data, XtPointer cbs) 
{
	int	item_no = (int) (intptr_t) data;
	/* should really be getting this from the widget */
	Calendar *c = calendar;

	goto_unit(c, item_no);
}

extern void
mb_update_segs(Browser *b, Tick tick, Tick dur, int *start_index, int *end_index)
{
        int     num_segs, i, start, start_hour, duration, nday;
        Props *p;
 
        p = (Props*)calendar->properties;
 
        start_hour = hour(tick);
        
        if (start_hour >= get_int_prop(p, CP_DAYEND)) {
                *start_index = -1;
                *end_index = -1;
                return;
        }
 
        if (start_hour < get_int_prop(p, CP_DAYBEGIN)) {
                start = 0;
                duration = dur - ((double)(get_int_prop(p, CP_DAYBEGIN) -
                 ((double)start_hour + (double)minute(tick)/(double)60))
                        * (double)hrsec);
        } else{
                start = ((double)(start_hour - get_int_prop(p, CP_DAYBEGIN)) *
                        (double)60 + (double)minute(tick));
                duration = dur;
        }
 
        if (duration <= 0) {
                *start_index = -1;
                *end_index = -1;
                return;
        }
        nday = (nday=dow(tick))==0? 6: nday-1;
        num_segs = (double)start / (double)MINS_IN_SEG;
        *start_index = (double)start / (double)MINS_IN_SEG + (nday * (b->segs_in_array/7));
        if (start - (num_segs * MINS_IN_SEG) > 7)
                (*start_index)++;
        num_segs = ((double)duration / (double)60 / (double)MINS_IN_SEG);
        *end_index = num_segs + *start_index;
        if (((double)duration/(double)60-MINS_IN_SEG*num_segs) > 7)
                (*end_index)++;
 
        if (*end_index > (i = ((nday + 1) * (b->segs_in_array / 7))) )
                *end_index = i;
 
        for (i = *start_index; i < *end_index; i++)
                if (b->add_to_array)
                        b->multi_array[i]++;
                else if (b->multi_array[i] > 0)
                        b->multi_array[i]--;
}

void
mb_update_array(char *entry_text, Calendar *c)
{
	int			start_ind, end_ind, i, r_cnt;
	time_t			start, stop;
        Browser			*b = (Browser *)c->browser;
        CSA_entry_handle	*entries = NULL;
	BlistData		*bd;
        CSA_return_code		stat;
        Browselist		*bl = (Browselist *)c->browselist;
	CSA_enum		*ops;
	CSA_attribute		*range_attrs;
	CSA_uint32 	num_entries;
	Tick			start_tick, end_tick;

	/*
	 *  Search for the entry text in our list of calendar handles
	 */
	i = 1;
	while ((bd = (BlistData *)CmDataListGetData(bl->blist_data, i++))
	       && strcmp(bd->name, entry_text) != 0);
	if (!bd)
		return;

        start = b->begin_week_tick;
        stop = next_ndays(b->begin_week_tick, 7) - 1;

	if (bd->cache == NULL) {
		setup_range(&range_attrs, &ops, &r_cnt, start, 
		    	stop, CSA_TYPE_EVENT,
		    	0, B_FALSE, bd->version);
        	stat = csa_list_entries(bd->cal_handle, r_cnt, range_attrs, 
					ops, &num_entries, &entries, NULL);
		free_range(&range_attrs, &ops, r_cnt);
        	backend_err_msg(b->frame, bd->name, stat,
				((Props_pu *)c->properties_pu)->xm_error_pixmap);
		if (stat != CSA_SUCCESS)
                	return;

		allocate_paint_cache(entries, num_entries, &bd->cache);
		bd->cache_size = num_entries;
		csa_free(entries);

	}

        for (i = 0; i < bd->cache_size; i++) {

		start_tick = bd->cache[i].start_time;
		end_tick = bd->cache[i].end_time;

                mb_update_segs(b, start_tick,
			       end_tick - start_tick, &start_ind,
			       &end_ind);
        }
}

static Boolean
register_names(char *name, Calendar *c)
{
	int		i;
	char		*user, *location;
	Props_pu	*p = (Props_pu *)c->properties_pu;
	Browser		*b = (Browser*)c->browser;
	BlistData	*bd = NULL;
        Browselist	*bl = (Browselist *)c->browselist;
        CSA_return_code	stat;
	CSA_session_handle	cal = NULL;
	unsigned int		user_access;
	CSA_calendar_user	csa_user;
	CSA_flags		flags = NULL;
	CSA_extension   	cb_ext;
	CSA_extension		logon_ext;
	char			buf[BUFSIZ];

	if (blank_buf(name))
		return False;

	i = 1;
	while ((bd = (BlistData *)CmDataListGetData(bl->blist_data, i++))
	       && strcmp(bd->name, name) != 0);
	if (!bd) {
	        char *title = XtNewString(catgets(c->DT_catd, 1, 1070,
				"Calendar : Error - Compare Calendars"));
		char *text = XtNewString(catgets(c->DT_catd, 1, 607,
				"Internal error registering calendar name."));
		char *ident = XtNewString(catgets(c->DT_catd, 1, 95, "Continue"));
		dialog_popup(b->frame,
			DIALOG_TITLE, title,
			DIALOG_TEXT, text,
			BUTTON_IDENT, 1, ident,
			DIALOG_IMAGE, p->xm_error_pixmap,
			NULL);
		XtFree(ident);
		XtFree(text);
		XtFree(title);
		return False;
	}
	if (bd->cal_handle)
		return True;

	if (strcmp(c->calname, name) == 0) {
		cal = c->my_cal_handle;
		user_access = c->my_access;
        } else if (strcmp(c->view->current_calendar, name) == 0) {
		cal = c->cal_handle;
		user_access = c->user_access;
	} else {
		user = cm_target2name(name);
		location = cm_target2location(name);
		csa_user.user_name = name;
		csa_user.user_type = 0;
		csa_user.calendar_user_extensions = NULL;
		csa_user.calendar_address = name;
		logon_ext.item_code = CSA_X_DT_GET_USER_ACCESS_EXT;
		logon_ext.item_data = 0;
		logon_ext.item_reference = NULL;
		logon_ext.extension_flags = CSA_EXT_LAST_ELEMENT;
		stat = csa_logon(NULL, &csa_user, NULL, NULL, NULL, &cal,
			&logon_ext);
		free(user);
		free(location);
 
        	if (stat != CSA_SUCCESS) {
			backend_err_msg(b->frame, name, stat,
			       ((Props_pu *)c->properties_pu)->xm_error_pixmap);
			return False;
        	} else
			user_access = logon_ext.item_data;

	}

	/* register for activity notification */
	flags = CSA_CB_ENTRY_ADDED | CSA_CB_ENTRY_UPDATED |
		CSA_CB_ENTRY_DELETED;
	cb_ext.item_code = CSA_X_XT_APP_CONTEXT_EXT;
	cb_ext.item_data = (CSA_uint32)c->xcontext->app;
	cb_ext.extension_flags = CSA_EXT_LAST_ELEMENT;

	stat = csa_register_callback(cal, flags, mb_update_handler, NULL, &cb_ext);
	if (stat != CSA_SUCCESS) {
		backend_err_msg(b->frame, name, stat,
			((Props_pu *)c->properties_pu)->xm_error_pixmap);
		return False;
	}

	bd->cal_handle = cal;
	bd->user_access = user_access;

	/* squirrel away data model version for later */

	bd->version = get_data_version(cal);

        return True;
}

extern void
mb_deregister_names(char *name, Calendar *c)
{
	int		i;
	Browser		*b = (Browser *)c->browser;
	Props_pu	*p = (Props_pu *)c->properties_pu;
	BlistData	*bd;
	Browselist	*bl = (Browselist *)c->browselist;
        CSA_return_code	stat;

	i = 1;
	while ((bd = (BlistData *)CmDataListGetData(bl->blist_data, i))
	       && strcmp(bd->name, name) != 0)
		++i;
	if (!bd)
		return;

	destroy_paint_cache(bd->cache, bd->cache_size);
	bd->cache = NULL;
	bd->cache_size = 0;

	if (bd->cal_handle != c->my_cal_handle &&
	    bd->cal_handle != c->cal_handle && bd->cal_handle) {
		stat = csa_logoff(bd->cal_handle, NULL);


		if (stat != CSA_SUCCESS) {
			backend_err_msg(b->frame, bd->name, stat,
					p->xm_error_pixmap);
		}
		bd->cal_handle = '\0';
		blist_clean(bl, False);
        }
}

static void
mb_box_notify(Widget widget, XtPointer client_data, XtPointer call_data)
{
	XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
        Calendar *c = (Calendar *)client_data;
	int		i;
	char		*addr;
        Browser       	*b;
        GEditor        	*e;
	BlistData	*bd;
	Browselist	*bl;
	XmString 	xmstr;
	char		name[BUFSIZ+5];
#ifdef FNS
	int		rcode;
	char		*fns_name, buf[256];
#endif

        b = (Browser*)c->browser;
        bl = (Browselist*)c->browselist;
        e = (GEditor*)c->geditor;

	i = 0;
	while (i < cbs->selected_item_count &&
	       cbs->item_position != cbs->selected_item_positions[i])
		++i;
	b->add_to_array = (i < cbs->selected_item_count) ? True : False;

	if ((bd = (BlistData *)CmDataListGetData(bl->blist_data,
						   cbs->item_position)) == NULL)
		return;
	/* erase busy status if it was busy because it was deselected */
	if (!XmListPosSelected(b->browse_list, cbs->item_position)) {
		sprintf(name, "  %s", bd->name);
		xmstr = XmStringCreateLocalized(name);
		XmListDeletePos(b->browse_list, cbs->item_position);
		XmListAddItem(b->browse_list, xmstr, cbs->item_position);
		XmStringFree(xmstr);
	} 	

#ifdef FNS
	rcode = -1;
	if (cmfns_use_fns((Props *)c->properties)) {
		/* Yes!  Try to use it */
		rcode = cmfns_lookup_calendar(bd->name, buf, sizeof(buf));
	}

	if (rcode > 0)
		addr = buf;
	else 
#endif
		addr = bd->name;

	_DtTurnOnHourGlass(b->frame);
	if (b->add_to_array) {
		if (!register_names(addr, c)) {
			XmListDeselectPos(b->browse_list, cbs->item_position);
			_DtTurnOffHourGlass(b->frame);
			return;
		}
        	if (geditor_showing(e)) {
                	add_to_gaccess_list(addr, bd->cal_handle,
				bd->user_access, bd->version, e, True);
			add_all_gappt(e);
		}
		mb_update_array(addr, c);
	} else {
		/*
		 * Must update the array before we deregister names because we
		 * close the calendar handle when we deregister.
		 */
		mb_update_array(addr, c);
		mb_deregister_names(addr, c);
        	if (geditor_showing(e))
                	remove_from_gaccess_list(addr, e);
	}
        mb_refresh_canvas(b, c);
	_DtTurnOffHourGlass(b->frame);
}

extern void
mb_clear_selected_calendar(
	char		*name,
	Calendar	*c)
{
	GEditor		*e = (GEditor*)c->geditor;
	Browser         *b = (Browser *)c->browser;

	/*
	 * Must update the array before we deregister names because we
	 * close the calendar handle when we deregister.
	 */
	b->add_to_array = False;
	mb_update_array(name, c);
        if (geditor_showing(e))
               	remove_from_gaccess_list(name, e);
	mb_deregister_names(name, c);
        mb_refresh_canvas(c->browser, c);
}

extern void
mb_init_canvas(Calendar *c)
{
        Browser 	*b = (Browser*)c->browser;
	Browselist	*bl = (Browselist *)c->browselist;
	BlistData	*bd;
	int		i;

        b->add_to_array = True;
        gr_clear_area(b->xcontext, 0, 0, b->canvas_w, b->canvas_h);
	register_names(c->calname, c);
        mb_update_array(c->calname, c);

	/*
	 *  Search for the entry text in our list of calendar handles
	 */

	i = 1;
	while ((bd = (BlistData *)CmDataListGetData(bl->blist_data, i++))
	       && strcmp(bd->name, c->calname) != 0);

	if (!bd)
		return;

	XmListSelectPos(b->browse_list, i - 1, False);
}


extern void
mb_init_datefield(Browser *b, Calendar *c)
{
        char		*date;
        Props		*p = (Props *)c->properties;
	OrderingType	ot = get_int_prop(p, CP_DATEORDERING);
	SeparatorType	st = get_int_prop(p, CP_DATESEPARATOR);

	date = XmTextGetString(b->datetext);
	if (!date || *date == '\0') {
        	date = get_date_from_widget(c->view->date, b->datetext, ot, st);
		if (date != NULL)
			XmTextSetString(b->datetext, date);
	}
}
static void
mb_init_browchart(Browser *b, Calendar *c)
{
        int		char_width, char_height, day_len, day_of_week;
        int		label_height, label_width;
        Props		*p = (Props *)c->properties;
	Dimension	canvas_width, canvas_height;
	XFontSetExtents fontextents;
 
	mb_init_datefield(b, c);
	XtVaGetValues(b->canvas,
		      XmNwidth, &canvas_width,
		      XmNheight, &canvas_height,
		      NULL);
	b->canvas_w = (int)canvas_width;
	b->canvas_h = (int)canvas_height;
	CalFontExtents(c->fonts->labelfont, &fontextents);
        char_height = fontextents.max_ink_extent.height;
    	char_width = fontextents.max_ink_extent.width;
        label_height = char_height * 2;
        label_width = char_width + 2;
        b->chart_height =
		b->canvas_h - (c->view->outside_margin * 2) - label_height - 5;
        b->chart_width =
		b->canvas_w - (c->view->outside_margin * 2) - label_width;
        b->boxw = b->chart_width / 7;
        b->chart_width = b->boxw * 7;

        day_len = get_int_prop(p, CP_DAYEND) - get_int_prop(p, CP_DAYBEGIN);
        b->boxh = b->chart_height / day_len;

        /*
	 * Make sure boxh is evenly divisable by BOX_SEG
	 */
        b->boxh -= (b->boxh % BOX_SEG);
        b->chart_height = b->boxh * day_len;
        b->chart_x = c->view->outside_margin + label_width;
        b->chart_y = c->view->outside_margin + label_height + char_height;
}       

extern void
mb_draw_chartgrid(Browser *b, Calendar *c)
{
        int    		x, y;
        int     	n;
        Props   	*p = (Props*)c->properties;
	XFontSetExtents fontextents;
        int     	char_height, char_width;
        char    	label[5], buf[160];
        new_XContext 	*xc = b->xcontext;
        int 		dayy, dayweek;
        Tick 		daytick;
	DisplayType 	dt;
	int		nop;
	int		s_width;
 
 
	CalFontExtents(c->fonts->viewfont, &fontextents);
	char_height = fontextents.max_logical_extent.height;
	char_width = fontextents.max_logical_extent.width;

        /*      Draw chart. It'll be filled in later.
                Draw grid lines and hourly labels.      */
        x = b->chart_x;
        y = b->chart_y;
 
        /* clear header */
        gr_clear_area(xc, 0, 0, b->canvas_w, b->chart_y);
        label[0] = '\0';

        /* draw hour labels */
        for (n = get_int_prop(p, CP_DAYBEGIN); n <= get_int_prop(p, CP_DAYEND); n++) {
	
		dt = get_int_prop(p, CP_DEFAULTDISP);
                if (dt == HOUR12)
                        sprintf(label, "%2d", n > 12 ? n - 12 : n);
                else
                        sprintf(label, "%2d", n);
                gr_text(xc, c->view->outside_margin-char_width, y+3,
                        c->fonts->viewfont, label, NULL);
                gr_draw_line(xc, x, y, x + b->chart_width,
                         y, gr_solid, NULL);
                y += b->boxh;
        }
 
        /*
         * Draw vertical lines and day labels
         */
        y = b->chart_y;
        dayy = y - char_height - 4;
        dayweek = dow(b->date);
        daytick = last_ndays(b->date, dayweek == 0 ? 6 : dayweek-1);
 
        /* draw month */
        format_date(b->begin_week_tick+1, get_int_prop(p, CP_DATEORDERING), buf, 0, 0, 0);
        gr_text(xc, c->view->outside_margin+4,
                 dayy-char_height-4, c->fonts->labelfont, buf, NULL);
 
        for (n = 0; n < 7; n++) {
		if (daytick >= get_bot() && daytick < get_eot()) {

			CalTextExtents(c->fonts->viewfont, days3[n+1], strlen(days3[n+1]), &nop, &nop, &s_width, &nop);

                	gr_text(xc, b->chart_x + (b->boxw * n) + ((b->boxw - s_width)/2),
                        	dayy, c->fonts->viewfont, days3[n+1], NULL);

			CalTextExtents(c->fonts->viewfont, numbers[dom(daytick)], strlen(numbers[dom(daytick)]), &nop, &nop, &s_width, &nop);

                	gr_text(xc, b->chart_x + (b->boxw * n) + ((b->boxw - s_width)/2),
                        	y - char_height / 2, c->fonts->viewfont,
                        	numbers[dom(daytick)], NULL);
		}
                daytick += daysec;
                gr_draw_line(xc, b->chart_x + (b->boxw * n),
                        y, b->chart_x + (b->boxw * n),
                        y + b->chart_height, gr_solid, NULL);
        }
 
        /*
         * Draw box around the whole thing.
         */
        gr_draw_box(xc, b->chart_x, b->chart_y, b->chart_width, b->chart_height, NULL);
        gr_draw_box(xc, b->chart_x-1, b->chart_y-1, b->chart_width+2, b->chart_height+2, NULL);
}

extern void
mb_draw_appts(Browser *b, int start, int end, Calendar *c)
{
        int		x, y, h, i, end_of_day;
	Boolean		outofbounds = False;
	Colormap	cms;

	if (next_ndays(b->date, 1) > get_eot())
		outofbounds = True;
        XtVaGetValues(b->canvas, XmNcolormap, &cms, NULL);

        h = (b->boxh/BOX_SEG);
        end_of_day = (b->segs_in_array / 7);
 
        y = b->chart_y + (start % end_of_day) * h;
        x = b->chart_x + (start/end_of_day * b->boxw);
 
        i = start;
        while (i < end) {
                if (b->multi_array[i] <= 0) {
                        gr_clear_area(b->xcontext, x, y, b->boxw, h);
                        y += h;
                        i++;
                }
                else if (b->multi_array[i] == 1) {
                        /* batch up for one repaint */
                        if ( ((i+1) < b->segs_in_array)
                                 && b->multi_array[i+1] == 1 &&
                                ( ((i+1) % end_of_day) != 0)) {
                                h += (b->boxh/BOX_SEG);
                                if (++i < end)
                                        continue;
                        }
                        if ((c->xcontext->screen_depth < 8) || FAKE_MONOCHROME)
                                gr_make_gray(b->xcontext, x, y, b->boxw, h, 25);
                        else
                                gr_make_grayshade(b->xcontext, x, y, b->boxw, h,
                                                        LIGHTGREY);
                        y += h;
                        h = (b->boxh/BOX_SEG);
                        i++;
                }
                else if (b->multi_array[i] == 2) {
                        /* batch up for one repaint */
                        if ( ((i+1) < b->segs_in_array)
                                 && b->multi_array[i+1] == 2 &&
                                ( ((i+1) % end_of_day) != 0) ) {
                                h += (b->boxh/BOX_SEG);
                                if (++i < end)
                                        continue;
                        }
                        if ((c->xcontext->screen_depth < 8) || FAKE_MONOCHROME)
                                gr_make_gray(b->xcontext, x, y, b->boxw, h, 50);
                        else
                                gr_make_rgbcolor(b->xcontext, cms, x, y,
						 b->boxw, h, MIDGREY, MIDGREY,
						 MIDGREY);
                        y += h;
                        h = (b->boxh/BOX_SEG);
                        i++;
                }
                else if (b->multi_array[i] >= 3) {
                        /* batch up for one repaint */
                        if ( ((i+1) < b->segs_in_array)
                                && b->multi_array[i+1] >= 3 &&
                                ( ((i+1) % end_of_day) != 0) ) {
                                h += (b->boxh/BOX_SEG);
                                if (++i < end)
                                        continue;
                        }
                        if ((c->xcontext->screen_depth < 8) || FAKE_MONOCHROME)
                                gr_make_gray(b->xcontext, x, y, b->boxw, h, 75);
                        else
                                gr_make_rgbcolor(b->xcontext, cms, x, y,
						 b->boxw, h, DIMGREY, DIMGREY,
						 DIMGREY);
                        y += h;
                        h = (b->boxh/BOX_SEG);
                        i++;
                }
                if (i != 0 && ((i % end_of_day) == 0)) {
                        x += b->boxw;
                        y = b->chart_y;
                        h = (b->boxh/BOX_SEG);
                }
	if (outofbounds && i > 4)
		break;
        }

        browser_select(c, b, NULL);
}

extern void
mb_refresh_canvas(Browser *b, Calendar *c)
{
        mb_draw_appts(b, 0, b->segs_in_array, c);
        mb_draw_chartgrid(b, c);
        mb_display_footermess(b, c);
}

void
mb_resize_proc(Widget w, XtPointer client_data, XtPointer call)
{
        Dimension width, height;
        Calendar *c = (Calendar *)client_data;
	Browser *b;

        XtVaGetValues(w, XmNwidth, &width, XmNheight, &height, NULL);
        b = (Browser*)c->browser;
        gr_clear_area(b->xcontext, 0, 0, width, height);
        mb_init_browchart(b, c);
        mb_refresh_canvas(b, c);
}

void
mb_refigure_chart(Calendar *c) {
	mb_resize_proc(((Browser *)c->browser)->canvas, (XtPointer)c, NULL);
}

extern void
browser_reset_list(Calendar *c) {
	int		i;
	Browser		*b = (Browser *)c->browser;
	BlistData	*bd;
	Browselist	*bl = (Browselist *)c->browselist;
	XmStringTable	list_selected_items, selected_items;
	int		selected_items_count;

	XtVaGetValues(b->browse_list,
		XmNselectedItemCount,	&selected_items_count,
		XmNselectedItems,	&list_selected_items,
		NULL);

	selected_items = (XmStringTable)calloc(selected_items_count, 
							sizeof(XmStringTable));
	for (i = 0; i < selected_items_count; i++)
		selected_items[i] = XmStringCopy(list_selected_items[i]);

	XtVaSetValues(b->upper_form, XmNresizePolicy, XmRESIZE_NONE, NULL);
	/*
	 * When a user removes a calendar from the menu we remove it
	 * from the multi-browser.  If the calendar happens to be selected
	 * in the multi-browser then we must deselect it and clean up
	 * the browser.  That's what this first loops does.
	 */
	for (i = 1; i <= bl->blist_data->count; i++) {
		bd = (BlistData *)CmDataListGetData(bl->blist_data, i);
		if (bd && bd->tag != BLIST_ACTIVE) {
			mb_clear_selected_calendar(bd->name, c);
			/* We need to reset this to one because the blist_data
			 * has changed which may cause us to miss an item.
			 */
			i = 1;
		}
	}

	XmListDeleteAllItems(b->browse_list);
	for (i = 1; i <= bl->blist_data->count; i++) {
		bd = (BlistData *)CmDataListGetData(bl->blist_data, i);
		if (bd && bd->name) {
			char 		buf[BUFSIZ + 5];
			XmString	xmstr;

			sprintf(buf, "  %s", bd->name); 
			xmstr = XmStringCreateLocalized(buf);
			if (!XmListItemExists(b->browse_list, xmstr))
				XmListAddItem(b->browse_list, xmstr, 0);
			XmStringFree(xmstr);
		}
	}
	XtVaSetValues(b->upper_form, XmNresizePolicy, XmRESIZE_ANY, NULL);
	
	/*
	 * Reselect the items that were selected before we changed the
	 * contents of the mb.
	 */
	for (i = 0; i < selected_items_count; i++) {
		int 	*pos_list, 
			 pos_cnt;

		if (XmListGetMatchPos(b->browse_list, selected_items[i],
				      &pos_list, &pos_cnt))
			XmListSelectPos(b->browse_list, pos_list[0], False);
		XmStringFree(selected_items[i]);
	}

	if (selected_items)
		free(selected_items);
}

extern void
init_browser(Calendar *c)
{
	pr_pos	xy;
        Browser *b = (Browser*)c->browser;
 
        browser_reset_list(c);
        b->row_sel = b->col_sel = 0;
        mb_init_browchart(b, c);
        mb_init_canvas(c);
        mb_refresh_canvas(b, c);
	xy.x = dow(b->date) - 1;
	xy.y = 0;
	browser_select(c, b, &xy);
}

static void
cancel_cb(Widget w, XtPointer client, XtPointer call)
{
        Calendar *c = (Calendar *)client;
	Browser *b = (Browser *)c->browser;

	XtPopdown(b->frame);

	XtDestroyWidget(b->frame);
	XtFree(b->multi_array); 
	XtFree(c->browser); c->browser = NULL;
}

static void
popup_cb(Widget w, XtPointer client, XtPointer call)
{
        Calendar *c = (Calendar *)client;
	Browser *b = (Browser *)c->browser;
	Position x, y;

	XtVaGetValues(c->frame, XmNx, &x, XmNy, &y, NULL);
	XtVaSetValues(b->frame, XmNx, x+100, XmNy, y+100, NULL);
}

/*
 * This is the CSA_callback called from the CSA library when
 * an update occurs on a calendar to which we are logged on,
 * and have registered interest.  Registered in register_names.
 * When calendar is logged off, any registered callbacks for it
 * are destroyed automagically.
 */
static void
mb_update_handler(CSA_session_handle cal, CSA_flags reason,
               CSA_buffer call_data, CSA_buffer client_data, CSA_extension *ext)
{
        Calendar        *c = calendar;
	Browser         *b = (Browser *)c->browser;

        /* sync whatever needs sync'ing */
	if (b) {
		br_display(c);
		if (geditor_showing((GEditor *)c->geditor))
			add_all_gappt((GEditor *)c->geditor);
	}
 
}

