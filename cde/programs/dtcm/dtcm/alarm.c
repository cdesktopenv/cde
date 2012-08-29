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
**  alarm.c
**
**  $XConsortium: alarm.c /main/4 1995/12/04 14:15:43 rswiston $
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
static  char sccsid[] = "@(#)alarm.c 1.50 95/07/27 Copyr 1991 Sun Microsystems, Inc.";
#endif

#include <EUSCompat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/LabelG.h>
#include <Xm/SeparatoG.h>
#include <Xm/MwmUtil.h>
#include <Dt/Wsm.h>
#include <Dt/Session.h>
#include "calendar.h"
#include "alarm.h"
#include "misc.h"
#include "props_pu.h"
#include "timeops.h"
#include "util.h"

#ifdef AIX
#include <sys/select.h>
#endif

/*  Structure used on a save session to see if a dt is iconic  */
typedef struct
{
   int state;
   Window icon;
} WM_STATE;
 


extern int debug;
extern int submit_mail(
		const char * to, const char * subject, const char * body);

/*******************************************************************************
**
**  Functions external to alarm.c
**
*******************************************************************************/

extern void
view_flasher(XtPointer client_data, XtIntervalId *interval_id) {
    Calendar        *c = (Calendar *)client_data;
    Atom 	wmStateAtom, actualType;
    int 	actualFormat;
    unsigned long nitems, leftover;
    WM_STATE *wmState;
 
    /*  Getting the WM_STATE property to see if iconified or not */
    wmStateAtom = XInternAtom(XtDisplay(c->frame), "WM_STATE", False);
 
    XGetWindowProperty (XtDisplay(c->frame), XtWindow(c->frame), 
			wmStateAtom, 0L,
                        (long)BUFSIZ, False, wmStateAtom, &actualType,
                        &actualFormat, &nitems, &leftover,
                        (unsigned char **) &wmState);
 
    if (wmState->state == IconicState) {

	XtVaSetValues(c->frame, XmNiconPixmap, NULL, NULL);
	if (c->view->flashes % 2) {
		if (c->icon && c->icon->icon && c->icon->icon_mask)
			XtVaSetValues(c->frame, 
				XmNiconPixmap, c->icon->icon, NULL);
	} else {
		if (c->icon_inverted && c->icon_inverted->icon 
				     && c->icon_inverted->icon_mask)
			XtVaSetValues(c->frame, 
				XmNiconPixmap, c->icon_inverted->icon, NULL);
	}
	XFlush(c->xcontext->display);
    }
    else {
	XWindowAttributes	window_attributes;

	XGetWindowAttributes(c->xcontext->display, 
				XtWindowOfObject(c->form), 
				&window_attributes);

       	XFillRectangle(c->xcontext->display, 
				XtWindowOfObject(c->form), 
				c->xcontext->invert_gc, 
				0, 0, 
				window_attributes.width, 
				window_attributes.height);
	XFlush(c->xcontext->display);
    }

    c->view->flashes -= 1;
    if (c->view->flashes != 0)
        XtAppAddTimeOut(c->xcontext->app, 250, view_flasher, c);
    else {
        /* really nasty kludge code to force a complete repaint of 
           the form.  It is possible to have damage occur during 
           the flashing feedback, and this can clean it up. */

        XUnmapWindow(c->xcontext->display, XtWindowOfObject(c->form));
        XMapWindow(c->xcontext->display, XtWindowOfObject(c->form));

	/* Make sure the correct icon is restored. */
	if (wmState->state == IconicState) {
		if (c->icon && c->icon->icon && c->icon->icon_mask)
			XtVaSetValues(c->frame, 
				XmNiconPixmap, c->icon->icon, NULL);
	}
    }
}

extern void 
flash_it(XtPointer client_data, XtIntervalId *interval_id) {
    int		i, j;
    Calendar	*c = (Calendar *)client_data;

    if (c->view->flashes == 0) {
        c->view->flashes = 10;
        XtAppAddTimeOut(c->xcontext->app, 250, view_flasher, c);
    }

    /* if the application is busy delivering a flash reminder, 
       don't deliver another one. */
}

extern void 
mail_it(XtPointer client_data, XtIntervalId *interval_id, CSA_reminder_reference *r) {
	Calendar		*c = (Calendar *)client_data;
	int			hr;
	Lines			*lines = NULL, *l = NULL;
	Props			*p = (Props *)c->properties;
	Boolean			pm;
	DisplayType		dt = get_int_prop(p, CP_DEFAULTDISP);
	Dtcm_appointment	*appt;
	char			subbuf[BUFSIZ], bodybuf[BUFSIZ];
	char			datebuf[200], startbuf[100], stopbuf[100];
	char			whatbuf[BUFSIZ];
	char			*to;
	CSA_return_code		stat;
	Tick			start, stop;
	char			*addr_data = NULL;

	appt = allocate_appt_struct(appt_read,
				    c->general->version,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_SUMMARY_I,
				    CSA_ENTRY_ATTR_ORGANIZER_I,
				    CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
				    CSA_ENTRY_ATTR_END_DATE_I,
				    CSA_ENTRY_ATTR_MAIL_REMINDER_I,
				    NULL);

	stat = query_appt_struct(c->cal_handle, r->entry, appt);

	backend_err_msg(c->frame, c->calname, stat,
			((Props_pu *)c->properties_pu)->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return;
	}

	/* compose to field */
	if (appt->mail->value->item.reminder_value->reminder_data.data == NULL ||
	    (appt->mail->value->item.reminder_value->reminder_data.size == 0)) {
		/* empty recipient */
		if (debug)
			fprintf(stderr, "%s", catgets(c->DT_catd, 1, 1,
				"dtcm: empty recipient in mail reminder\n"));
		to = appt->author->value->item.calendar_user_value->user_name;
	} else {
		addr_data = calloc(appt->mail->value->item.reminder_value->reminder_data.size + 1, 1);
		strncpy(addr_data, (char *) appt->mail->value->item.reminder_value->reminder_data.data, appt->mail->value->item.reminder_value->reminder_data.size);
		to = addr_data;
	}

	/* compose subject field */
	lines = text_to_lines(appt->what->value->item.string_value, 5);
	sprintf(subbuf, catgets(c->DT_catd, 1, 2, "Reminder- %s"),
		(lines) ? lines->s : "\0");

	/* compose message body */
	_csa_iso8601_to_tick(appt->time->value->item.date_time_value, &start);
	format_tick(start, ORDER_MDY, SEPARATOR_SLASH, datebuf);

	if (showtime_set(appt) && !magic_time(start)) {
		hr = hour(start);
		pm = (dt == HOUR12 && !adjust_hour(&hr)) ? True : False;
		if (dt == HOUR12) {
			sprintf(startbuf, "%2d:%02d %s", hr, minute(start),
				pm ? "pm" :
				"am");
		} else {
			sprintf(startbuf, "%02d%02d", hr, minute(start));
		}
	} else
		startbuf[0] = '\0';

	if (showtime_set(appt) && !magic_time(start) && appt->end_time) {
		_csa_iso8601_to_tick(appt->end_time->value->\
			item.date_time_value, &stop);
		hr = hour(stop);
		pm = (dt == HOUR12 && !adjust_hour(&hr)) ? True : False;
		if (dt == HOUR12) {
			sprintf(stopbuf, "%2d:%02d %s", hr, minute(stop),
				pm ? catgets(c->DT_catd, 1, 3, "pm") :
				catgets(c->DT_catd, 1, 4, "am"));
		} else {
			sprintf(stopbuf, "%02d%02d", hr, minute(stop));
		}
	} else
		stopbuf[0] = '\0';

	if (l = lines) {
		sprintf(whatbuf, "%s\n", l->s);
		l = l->next;
	} else
		whatbuf[0] = '\0';

	while(l != NULL) {
		strcat(whatbuf, "\t\t");
		strcat(whatbuf, l->s);
		strcat(whatbuf, "\n");
		l = l->next;
	}
	if (lines)
		destroy_lines(lines);

	if (stopbuf[0] != '\0') {
		sprintf(bodybuf, catgets(c->DT_catd, 1, 7, "\n\n\t** Calendar Appointment **\n\n\tDate:\t%s\n\tStart:\t%s\n\tEnd:\t%s\n\tWhat:\t%s"),
			datebuf, startbuf, stopbuf, whatbuf);
	} else {
		sprintf(bodybuf, catgets(c->DT_catd, 1, 1100, "\n\n\t** Calendar To Do Item **\n\n\tDue Date:\t%s\n\tTime Due:\t%s\n\tWhat:\t\t%s"),
			datebuf, startbuf, whatbuf);
	}

	if (debug) {
		fprintf(stderr, "to = `%s`\n", to);
		fprintf(stderr, "subject = `%s`\n", subbuf);
		fprintf(stderr, "body = `%s`\n", bodybuf);
	}

	(void)submit_mail(to, subbuf, bodybuf);

	free_appt_struct(&appt);

	if (addr_data)
		free(addr_data);
}

extern void 
open_it(XtPointer client_data, XtIntervalId *interval_id, CSA_reminder_reference *r) {
	Calendar	*c = (Calendar *)client_data;
	
	postup_show_proc(c, r);
}

extern void
reminder_driver(XtPointer client_data, XtIntervalId *interval_id) {
	Calendar		*c = (Calendar *)client_data;
	Tick			run_tick;
	int			i;

	if (c->view->next_alarm_id != *interval_id || !c->view->next_alarm) {
		c->view->next_alarm_id = 0;
		reset_timer(c);
		return;
	}


	/*
	 * do not handle the reminder if the time it's to happen
	 * is earlier than the current time
	 */
	_csa_iso8601_to_tick(c->view->next_alarm[0].run_time, &run_tick);
	if (now() >= run_tick) {

		for (i = 0; i < c->view->next_alarm_count; i++) {
			if (strcmp(c->view->next_alarm[i].attribute_name, CSA_ENTRY_ATTR_AUDIO_REMINDER)==0) 
				ring_it(client_data, interval_id);
			else if (strcmp(c->view->next_alarm[i].attribute_name, CSA_ENTRY_ATTR_FLASHING_REMINDER)==0) 
				flash_it(client_data, interval_id);
			else if (strcmp(c->view->next_alarm[i].attribute_name, CSA_ENTRY_ATTR_POPUP_REMINDER)==0) 
				open_it(client_data, interval_id, &c->view->next_alarm[i]);
			else if (strcmp(c->view->next_alarm[i].attribute_name, CSA_ENTRY_ATTR_MAIL_REMINDER)==0) 
				mail_it(client_data, interval_id, &c->view->next_alarm[i]);

		}
	}

	c->view->next_alarm_id = 0;

	reset_timer(c);
}

extern void
bell_ringer(XtPointer client_data, XtIntervalId *interval_id) {
	Calendar        *c = (Calendar *)client_data;
	XBell(c->xcontext->display, 50);
	c->view->rings -= 1;
	if (c->view->rings != 0)
		XtAppAddTimeOut(c->xcontext->app, 250, bell_ringer, c);
}

extern void
ring_it(XtPointer client_data, XtIntervalId *interval_id) {
	Calendar	*c = (Calendar *)client_data;

    if (c->view->rings == 0) {
        c->view->rings = 5;
        XtAppAddTimeOut(c->xcontext->app, 250, bell_ringer, c);
    }

    /* If the application is busy delivering a ring reminder
       don't deliver another one. */
}

/*
 * Close dialog: pop down and destroy the dialog, and free
 * associated memory.
 */
static void
close_cb(Widget w, XtPointer data, XtPointer ignore)
{
	Widget shell = (Widget) data;

   	XtPopdown(shell);
   	XtDestroyWidget(shell);
}


/*
**  Functions to build and popup the postup reminder
*/
extern void
postup_show_proc(Calendar *c, CSA_reminder_reference *r) {
        int			start_hr, stop_hr;
	char			text[BUFSIZ];
	time_t			st, sp = 0;
	Lines			*lines, *l = NULL;
	Props			*p = (Props *)c->properties;
	Widget			pu_frame, pu_base_form, 
				pu_text_form, separator, 
				pu_form, button_form, pu_date,
				pu_range, pu_image, pu_close, line, last_line;
	Boolean			start_am, stop_am;
	XmString		xmstr;
	CSA_return_code		stat;
	DisplayType		dis_t = get_int_prop(p, CP_DEFAULTDISP);
	OrderingType		ord_t = get_int_prop(p, CP_DATEORDERING);
	SeparatorType		sep_t = get_int_prop(p, CP_DATESEPARATOR);
	Dtcm_appointment	*appt;
	char			*title;

	if (!c->postup)
		c->postup = XtAppCreateShell("calendar_postup", "Dtcm",
			xmDialogShellWidgetClass, c->xcontext->display,
			NULL, 0);

	/*
	**  Create the Motif objects
	*/
	title = XtNewString(catgets(c->DT_catd, 1, 839, "Calendar : Reminder"));
	pu_frame = XtVaCreatePopupShell("pu_frame",
		topLevelShellWidgetClass, c->postup,
		XmNtitle, title,
		XmNmwmFunctions, MWM_FUNC_MOVE | MWM_FUNC_CLOSE,
		XmNdeleteResponse,      XmDESTROY,
		NULL);
	XtFree(title);

	pu_base_form = XtVaCreateWidget("pu_base_form",
		xmFormWidgetClass, pu_frame,
		NULL);

	pu_text_form = XtVaCreateWidget("pu_test_form",
		xmFormWidgetClass, pu_base_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNleftOffset, 1,
		XmNrightOffset, 1,
		XmNtopOffset, 1,
		NULL);

	pu_image = XtVaCreateWidget("pu_image",
		xmLabelGadgetClass, pu_text_form,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 5,
		XmNlabelType, XmPIXMAP,
		XmNlabelPixmap, ((Props_pu *)c->properties_pu)->postup_pixmap,
		NULL);

	button_form = XtVaCreateWidget("pu_form",
		xmFormWidgetClass, pu_base_form,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 2,
		XmNleftOffset, 1,
		XmNrightOffset, 1,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNfractionBase, 3,
		NULL);

	xmstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 680, "Close"));
	pu_close = XtVaCreateManagedWidget("close", 
		xmPushButtonWidgetClass, button_form, 
                XmNtopAttachment, 	XmATTACH_FORM,
		XmNbottomAttachment, 	XmATTACH_FORM,
		XmNleftAttachment, 	XmATTACH_POSITION,
		XmNrightAttachment, 	XmATTACH_POSITION,
		XmNleftPosition,	1,
		XmNrightPosition,	2,
		XmNbottomOffset, 	1,
                XmNlabelString, 	xmstr,
                NULL);
	XmStringFree(xmstr);

	separator = XtVaCreateWidget("separator",
                xmSeparatorGadgetClass,
                pu_base_form,
                XmNleftAttachment,      XmATTACH_FORM,
                XmNrightAttachment,     XmATTACH_FORM,
                XmNbottomAttachment,   	XmATTACH_WIDGET,
                XmNbottomWidget,        button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, pu_text_form,
		XmNtopOffset, 1,
                NULL);


	pu_form = XtVaCreateWidget("pu_form",
		xmFormWidgetClass, pu_text_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 1,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, pu_image,
		XmNverticalSpacing, 10,
		NULL);

	pu_date = XtVaCreateWidget("pu_date",
		xmLabelGadgetClass, pu_form,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		NULL);

	pu_range = XtVaCreateWidget("pu_range",
		xmLabelGadgetClass, pu_form,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, pu_date,
		XmNtopOffset, 8,
		NULL);

	XtAddCallback(pu_close, XmNactivateCallback, close_cb, pu_frame);

	appt = allocate_appt_struct(appt_read,
				    c->general->version,
				    CSA_ENTRY_ATTR_START_DATE_I,
				    CSA_ENTRY_ATTR_END_DATE_I,
				    CSA_ENTRY_ATTR_SUMMARY_I,
				    CSA_X_DT_ENTRY_ATTR_SHOWTIME_I,
				    NULL);
	stat = query_appt_struct(c->cal_handle, r->entry, appt);
	backend_err_msg(c->frame, c->calname, stat,
			((Props_pu *)c->properties_pu)->xm_error_pixmap);
	if (stat != CSA_SUCCESS) {
		free_appt_struct(&appt);
		return;
	}

	_csa_iso8601_to_tick(appt->time->value->item.date_time_value, &st);
	if (appt->end_time)
		_csa_iso8601_to_tick(appt->end_time->value->\
			item.date_time_value, &sp);
	l = lines = text_to_lines(appt->what->value->item.string_value, 5);
	last_line = 0;
	while (l) {
		xmstr = XmStringCreateLocalized(l->s);
		line = XtVaCreateWidget("text_line",
			xmLabelGadgetClass, pu_text_form,
			XmNlabelString, xmstr,
			XmNleftAttachment, XmATTACH_WIDGET,
			XmNleftWidget, pu_form,
			XmNleftOffset, 15,
			NULL);
		XmStringFree(xmstr);

		if (last_line)
			XtVaSetValues(line, 
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, last_line,
				XmNtopOffset, 8,
				NULL);
		else
			XtVaSetValues(line, 
				XmNtopAttachment, XmATTACH_FORM,
				XmNtopOffset, 8,
				NULL);

		last_line = line;
		l = l->next;
	}
	if (lines)
		destroy_lines(lines);

	/*
	**  Now fill in the correct information
	*/
	set_date_in_widget(st, pu_date, ord_t, sep_t);

        if (!showtime_set(appt) || magic_time(st)) 
                text[0] = '\0';
	else {
		start_hr = hour(st);
		if (sp) stop_hr = hour(sp);
		if (dis_t == HOUR12) {
			start_am = adjust_hour(&start_hr);
			if (sp) {
				/* NL_COMMENT

				   Message 1087 : This message is used to form
				   the alarm string that appears in the
				   reminder popup.  In the C locale it would
				   look something like this:

				   ``From 11:00am to 1:00pm''

				   In the printf conversion string the $n are:

					$1 	-> start hour
					$2	-> start minute
					$3	-> am or pm
					$4	-> stop hour
					$5	-> stop minute
					$6	-> am or pm

				*/
			        char *am = XtNewString(catgets(c->DT_catd, 
							       1, 4, "am"));
			        char *pm = XtNewString(catgets(c->DT_catd, 
							       1, 3, "pm"));
				stop_am = adjust_hour(&stop_hr);
				sprintf(text, catgets(c->DT_catd, 1, 1087,
				   "From %1$2d:%2$02d%3$s to %4$2d:%5$02d%6$s"),
					start_hr, minute(st),
					(start_am) ? am : pm,
					stop_hr, minute(sp),
					(stop_am) ? am : pm
					);
				XtFree(am);
				XtFree(pm);
			} else {
				/* NL_COMMENT

				   Message 1088 : This message is used to form
				   the alarm string that appears in the
				   reminder popup.  It is used when an appt
				   does not have and ``end'' time.  In the 
				   C locale it would look something like this:

				   ``11:00am''

				   In the printf conversion string the $n are:

					$1 	-> start hour
					$2	-> start minute
					$3	-> am or pm

				*/
			        char *meridian = 
				  XtNewString ((start_am) ? 
					       catgets(c->DT_catd, 1, 4, "am"):
					       catgets(c->DT_catd, 1, 3, "pm"));
				
				sprintf(text, catgets(c->DT_catd, 1, 1088,
				   			"%1$2d:%2$02d%3$s"), 
					start_hr, minute(st), meridian
					);
				XtFree(meridian);
			}
		} else {
			if (sp) {
				/* NL_COMMENT

				   Message 1089 : This message is used to form
				   the alarm string that appears in the
				   reminder popup.  This string is used when
				   a user has asked that times be displayed
				   in 24 hour format.  In the C locale it 
				   would look something like this:

				   ``From 0100 to 1600''

				   In the printf conversion string the $n are:

					$1 	-> start hour
					$2	-> start minute
					$3	-> stop hour
					$4	-> stop minute

				*/
				sprintf(text, catgets(c->DT_catd, 1, 1089,
				   "From %1$02d%2$02d to %3$02d%4$02d"),
					start_hr, minute(st), stop_hr, 
					minute(sp));
			} else {
				/* NL_COMMENT

				   Message 1090 : This message is used to form
				   the alarm string that appears in the
				   reminder popup.  This string is used when
				   an appt does not have an end time and the
				   user has asked that times be displayed
				   in 24 hour format.  In the C locale it 
				   would look something like this:

				   ``1600''

				   In the printf conversion string the $n are:

					$1 	-> start hour
					$2	-> start minute

				*/
				sprintf(text, catgets(c->DT_catd, 1, 1090,
							"%1$02d%2$02d"), 
					start_hr, minute(st));
			}
		}
	}

	free_appt_struct(&appt);
	xmstr = XmStringCreateLocalized(text);
	XtVaSetValues(pu_range, XmNlabelString, xmstr,
		NULL);
	XmStringFree(xmstr);

	ManageChildren(pu_form);
	ManageChildren(pu_base_form);
	ManageChildren(pu_text_form);
	XtManageChild(pu_base_form);
	XtVaSetValues(button_form, XmNdefaultButton, pu_close, NULL);
        XtVaSetValues(button_form, XmNcancelButton, pu_close, NULL);
	XtPopup(pu_frame, XtGrabNone);
	DtWsmOccupyAllWorkspaces(c->xcontext->display, XtWindow(pu_frame));
}
