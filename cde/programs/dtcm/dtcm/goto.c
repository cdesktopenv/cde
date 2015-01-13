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
**  goto.c
**
**  $XConsortium: goto.c /main/4 1995/12/04 14:18:24 rswiston $
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
static  char sccsid[] = "@(#)goto.c 1.40 95/03/28 Copyr 1993 Sun Microsystems, Inc.";
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
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>
#include <Xm/MwmUtil.h>
#include "calendar.h"
#include "util.h"
#include "timeops.h"
#include "find.h"
#include "datefield.h"
#include "getdate.h"
#include "dayglance.h"
#include "select.h"
#include "props.h"
#include "editor.h"
#include "goto.h"
#include "help.h"

extern caddr_t
make_goto(Calendar *c)
{
	Goto 		*g;
	XmString 	xmstr;
	Widget		separator, button_form;
	void 		goto_date(), g_cancel_cb();
	char		*title;

	if (c->goTo == NULL) {
                c->goTo = (caddr_t)ckalloc(sizeof(Goto));
                g = (Goto*)c->goTo;
        }
        else
                g = (Goto*)c->goTo;

	title = XtNewString(catgets(c->DT_catd, 1, 715, "Calendar : Go to Date"));
	g->frame = XtVaCreatePopupShell("frame",
                xmDialogShellWidgetClass, c->frame,
                XmNtitle, title,
                XmNallowShellResize, True,
		XmNmappedWhenManaged, False,
                NULL);
	XtFree(title);

        g->form = XtVaCreateWidget("form",
                xmFormWidgetClass, 	g->frame,
                XmNautoUnmanage, 	False,
		XmNfractionBase,        3,
                XmNhorizontalSpacing, 	4,
                XmNverticalSpacing, 	4,
		XmNmarginWidth,         0,
		XmNmarginHeight,        0,
                NULL);

        xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 293, "Date:"));
        g->datelabel = XtVaCreateWidget("Date:", 
		xmLabelWidgetClass, 	g->form,
                XmNleftAttachment, 	XmATTACH_FORM,
		XmNleftOffset,          5,
                XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopOffset, 		11,
		XmNlabelString, 	xmstr,
                NULL);
	XmStringFree(xmstr);

        g->datetext = XtVaCreateWidget("text", 
		xmTextWidgetClass, 	g->form,
                XmNtopAttachment, 	XmATTACH_FORM,
		XmNtopWidget, 		g->datelabel,
		XmNtopOffset, 		7,
		XmNleftAttachment, 	XmATTACH_WIDGET,
		XmNleftWidget, 		g->datelabel,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	3,
		XmNrightOffset, 	10,
		XmNcolumns,		32,
                NULL);
        XtAddCallback(g->datetext, XmNactivateCallback, goto_date, NULL);


	separator = XtVaCreateWidget("separator",
                xmSeparatorGadgetClass,
                g->form,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
		XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		g->datetext,
		XmNtopOffset, 		5,
                NULL);

        button_form = XtVaCreateWidget("form",
                xmFormWidgetClass,      g->form,
                XmNautoUnmanage,        False,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNleftOffset,          5,
                XmNrightAttachment,     XmATTACH_FORM,
                XmNrightOffset,         5,
                XmNtopAttachment,       XmATTACH_WIDGET,
                XmNtopWidget,           separator,
                XmNhorizontalSpacing,   4,
                XmNfractionBase,        3,
                NULL);

        xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 294, "Go To"));
        g->datebutton = XtVaCreateWidget("goTo", 
		xmPushButtonWidgetClass, button_form,
                XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		separator,
		XmNlabelString, 	xmstr,
		XmNnavigationType, 	XmTAB_GROUP,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	0,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	1,
                NULL);
        XtAddCallback(g->datebutton, XmNactivateCallback, goto_date, NULL);
	XmStringFree(xmstr);

        xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 680, "Close"));
        g->cancelbutton = XtVaCreateWidget("cancel", 
		xmPushButtonWidgetClass, button_form,
                XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		separator,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	1,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	2,
		XmNlabelString, 	xmstr,
                NULL);
	XtAddCallback(g->cancelbutton, XmNactivateCallback, g_cancel_cb, NULL);
	XmStringFree(xmstr);


        xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 77, "Help"));
        g->helpbutton = XtVaCreateWidget("help", 
		xmPushButtonWidgetClass, button_form,
                XmNtopAttachment, 	XmATTACH_WIDGET,
		XmNtopWidget, 		separator,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNleftPosition, 	2,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNrightPosition, 	3,
		XmNlabelString, 	xmstr,
                NULL);
	XtAddCallback(g->helpbutton, XmNactivateCallback, 
				(XtCallbackProc)help_cb, GOTO_HELP_BUTTON);
        XtAddCallback(g->form, XmNhelpCallback,
                (XtCallbackProc)help_cb, (XtPointer) GOTO_HELP_BUTTON);
	XmStringFree(xmstr);

	g->goto_message = XtVaCreateWidget("message",
		xmLabelGadgetClass, g->form,
		XmNalignment, XmALIGNMENT_BEGINNING,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, g->cancelbutton,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
		NULL);

        /* set default button */
        XtVaSetValues(g->form, XmNdefaultButton, g->datebutton, NULL);
        XtVaSetValues(g->form, XmNcancelButton, g->cancelbutton, NULL);
        XtVaSetValues(button_form, XmNdefaultButton, g->datebutton, NULL);
        XtVaSetValues(button_form, XmNcancelButton, g->cancelbutton, NULL);
        XmProcessTraversal(g->datetext, XmTRAVERSE_CURRENT);
        XtVaSetValues(g->form, XmNinitialFocus, g->datetext, NULL);

	ManageChildren(button_form);
	ManageChildren(g->form);
	XtManageChild(g->form);
        XtVaSetValues(g->frame, XmNmappedWhenManaged, True, NULL);
        XtRealizeWidget(g->frame);

	XtPopup(g->frame, XtGrabNone);

	return(caddr_t)g;
}

void
goto_date(Widget widget, XtPointer client_data, XmPushButtonCallbackStruct *cbs)
{
        Calendar *c = calendar;
        Props *p;
        Goto *g;
        Tick gotodate, start, stop;
        char *date = NULL, today[DATESIZ];
	char message[40];
	Dimension w, h;
	OrderingType ot;
	SeparatorType st;
 
        p = (Props*)c->properties;
	ot = get_int_prop(p, CP_DATEORDERING);
	st = get_int_prop(p, CP_DATESEPARATOR);
        g = (Goto*)c->goTo;
 
	date = XmTextGetString(g->datetext);
	if ((date == NULL) || (*date == '\0')) {
		sprintf(message, "%s", catgets(c->DT_catd, 1, 297, "Please type in a date"));
		set_message(g->goto_message, message);
                return;
	}
        format_tick(c->view->date, ot, st, today);
	set_message(g->goto_message, "");
        date = get_date_from_widget(c->view->date, g->datetext, ot, st);
        if ( date == NULL ) {
		return;
        } else {
                gotodate = cm_getdate(date, NULL);
        }
	if (!strcmp(today, date)) {
		sprintf(message, catgets(c->DT_catd, 1, 298, "You are already viewing %s"), date);
		set_message(g->goto_message, message);
		return;
	}
 

	if (gotodate == DATE_BBOT) {
		sprintf(message, "%s", catgets(c->DT_catd, 1, 814, "You must enter a date after 1969 and before 2038"));
		set_message(g->goto_message, message);
                return;
	}
	else if (gotodate == DATE_AEOT) {
		sprintf(message, "%s", catgets(c->DT_catd, 1, 814, "You must enter a date after 1969 and before 2038"));
		set_message(g->goto_message, message);
                return;
	}
        else if (gotodate <= 0) {
		sprintf(message, "%s", catgets(c->DT_catd, 1, 299, "Invalid Date"));
		set_message(g->goto_message, message);
                return;
        }

	invalidate_cache(c);
         
       	get_range(c->view->glance, c->view->date, &start, &stop);
        if (in_range(start, stop, gotodate)) {
                /* date is in view; deselect and repaint new selection */
                calendar_deselect(c);
                c->view->olddate = c->view->date;
                c->view->date = gotodate;
                paint_selection(c);
        }
        else {
                /* repaint the entire canvas */
                c->view->olddate = c->view->date;
                c->view->date = gotodate;
		c->view->nwks = numwks(c->view->date);
        	XtVaGetValues(c->canvas, XmNwidth, &w, XmNheight, &h, NULL);
                gr_clear_area(c->xcontext, 0, 0, w, h);
                if (c->view->glance == dayGlance) {
                        init_mo(c);
                        init_dayview(c);
                }
                paint_canvas(c, NULL, RENDER_UNMAP);
        }        
}

void
g_cancel_cb(
        Widget  widget,
        XtPointer client_data,
        XmPushButtonCallbackStruct *cbs)
{
        Calendar *c = calendar;
        Goto      *g;

        g = (Goto *)c->goTo;

	XmTextSetString(g->datetext, "");

        XtPopdown(g->frame);

        return;
}
