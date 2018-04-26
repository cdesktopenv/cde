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
/* $TOG: about.c /main/7 1999/02/03 15:35:27 mgreess $ */
/*
 *  (c) Copyright 1993, 1994 Hewlett-Packard Company
 *  (c) Copyright 1993, 1994 International Business Machines Corp.
 *  (c) Copyright 1993, 1994 Novell, Inc.
 *  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <Xm/Frame.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/Label.h>
#include <Xm/DialogS.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/SeparatoG.h>
#include <Dt/Dt.h>
#include <EUSCompat.h>
#include "calendar.h"
#include "props_pu.h"
#include "revision.h"

#include "icon_pixmap_xbm"

#ifdef CREDITS
char *main_credits[] = {
	"Mary Hamilton",
	"Chris Kasso",
	"Martin Knutson",
	"Terre Layton",
	"Yvonne Tso",
	"J.F. Unson",
	"Janice Winsor",
	NULL
};

char *also_credits[] = {
	"Brent Ayers",
	"David Curley",
	"Betty Deluco",
	"Lin-Chuan Lee",
	NULL
};
#endif /* CREDITS */

#define GAP		10
#define TOG_GAP		10
#define MAX_LABEL_SPACE	55
#define XmUNSPECIFIED_ICON_SIZE 0
#define DTCM_ABOUT	"DtCMa"

extern Dimension ComputeMaxWidth(Widget, Widget, Widget, Widget);

static void close_about(Widget, XtPointer, XtPointer);
static void destroy_about(Widget, XtPointer, XtPointer);
static void do_credits(Widget, XtPointer, XtPointer);

extern String XmGetIconFileName( 
                        Screen *screen,
                        String imageInstanceName,
                        String imageClassName,
                        String hostPrefix,
                        unsigned int size) ;

/*
 * Create is popup the Calendar About dialog.
 */
void
show_about(
	Widget		w,
	XtPointer	clientData,
	XtPointer	cbs)
{
	Calendar	*c = calendar;
        XmString    	 xstr = NULL;
	Dimension	 max_width, form_width;
	Pixmap		 image = 0;
	char		 buf[64],
			*image_filename;
	XmbTextItem	 text[1];
	Pixel		 fg, bg;
	Widget		 icon_label, main_form, sub_form, name_label, 
			 version_label, version_date, copyright_label,
			 hp_label, ibm_label, novell_label, sun_label,
			 separator, close_button, more_button, dialog,
			 data_version_label, server_version_label; 
	Display		*dpy = XtDisplay(c->frame);
	Screen          *screen = XtScreen(c->frame);
	struct tm	 tm;
	char		*title;
#if DTCM_INTERNAL_REV > 0
	char		 buf2[64];
#endif

	if (c->about_dialog) {
		XtPopup(c->about_dialog, XtGrabNone);
		XMapRaised(dpy, XtWindow(c->about_dialog));
		return;
	}

	title = XtNewString(catgets(c->DT_catd, 1, 922, "About Calendar"));
	dialog = XtVaCreatePopupShell("about_dialog",
		xmDialogShellWidgetClass,
		c->frame,
		XmNtitle, 			title,
		XmNdeleteResponse,		XmDESTROY,
		XmNallowShellResize, 		True,
		NULL);
	XtFree(title);

	main_form =
	    XtVaCreateWidget("about_form_mgr",
		xmFormWidgetClass,
		dialog,
		XmNautoUnmanage,		False,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNbottomAttachment, 		XmATTACH_FORM,
		XmNfractionBase,		4,
		XmNnoResize,			True,
		NULL);

	XtVaGetValues(main_form,
		XmNbackground,		&bg,
		XmNforeground,		&fg,
		NULL);

	if ((image_filename = XmGetIconFileName(screen, NULL, 
						DTCM_ABOUT, NULL, 
						XmUNSPECIFIED_ICON_SIZE))) {
		image = XmGetPixmap(screen, image_filename, fg, bg); 
		free(image_filename);
	}


	icon_label =
	    XtVaCreateWidget("icon_label",
		xmLabelWidgetClass,
		main_form,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			GAP,
		NULL);

	if (!image || image == XmUNSPECIFIED_PIXMAP) {
        	xstr = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 917, "Missing\nGraphics"));
		XtVaSetValues(icon_label,
			XmNlabelType, 			XmSTRING,
			XmNlabelString, 		xstr,
			XmNborderWidth,			1,
			NULL);
		XmStringFree(xstr);
	} else {
		XtVaSetValues(icon_label,
			XmNlabelType, 			XmPIXMAP,
			XmNlabelPixmap, 		image,
			NULL);
	}

	sub_form =
	    XtVaCreateWidget("sub_form",
		xmFormWidgetClass,
		main_form,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		XmNleftAttachment, 		XmATTACH_WIDGET,
		XmNleftWidget, 			icon_label,
		XmNleftAttachment, 		XmATTACH_WIDGET,
                XmNshadowThickness,             0,
                XmNshadowType,                  XmSHADOW_IN,
		NULL);

        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 340, "Calendar"));
	name_label =
	    XtVaCreateWidget("about_name_label",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			0,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);

	sprintf(buf, "%s %d.%d.%d", 
				catgets(c->DT_catd, 1, 671, "Version"), 
				DtVERSION, DtREVISION, DtUPDATE_LEVEL); 
#if DTCM_INTERNAL_REV > 0
	strcpy(buf2, buf);
	sprintf(buf, "%s (%s %d)", 
				buf2,
				catgets(c->DT_catd, 1, 672, "Revision"), 
				DTCM_INTERNAL_REV);
#endif

        xstr = XmStringCreateLocalized(buf);
	version_label =
	    XtVaCreateWidget("version_label",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			GAP,
		XmNtopWidget, 			name_label,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			0,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);

	sprintf(buf, "%s %d", catgets(c->DT_catd, 1, 967, 
				"Calendar Server Version"), 
			        get_server_version(c->my_cal_handle));

        xstr = XmStringCreateLocalized(buf);
	server_version_label =
	    XtVaCreateWidget("server_version",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			0,
		XmNtopWidget, 			version_label,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			0,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);

	sprintf(buf, "%s %d", catgets(c->DT_catd, 1, 968, 
				"Calendar Data Version"), 
			        c->my_cal_version);

        xstr = XmStringCreateLocalized(buf);
	data_version_label =
	    XtVaCreateWidget("data_version",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			0,
		XmNtopWidget, 			server_version_label,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			0,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);

#if defined(DTCM_SHOW_ABOUT_COPYRIGHT)

	tm.tm_sec = 0;
	tm.tm_min = 0;
	tm.tm_hour = 0;
	tm.tm_mday = 1;
	tm.tm_mon = DTCM_VERSION_MONTH - 1;
	tm.tm_year = DTCM_VERSION_YEAR - 1900;
	tm.tm_isdst = -1;

#ifdef _AIX
	sprintf( buf, "%s %d", months[tm.tm_mon + 1 ], tm.tm_year + 1900 );
#else /* _AIX */
	strftime(buf, 64, "%B %Y", &tm);
#endif /* _AIX */

        xstr = XmStringCreateLocalized(buf);
	version_date =
	    XtVaCreateWidget("version_date",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			0,
		XmNtopWidget, 			data_version_label,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			0,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);
        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 674,
					   "Copyright (c) 1993, 1994, 1995:"));
	copyright_label =
	    XtVaCreateWidget("copyright_label",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			GAP,
		XmNtopWidget, 			version_date,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			0,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);
        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 675, 
						"Hewlett-Packard Company"));
	hp_label =
	    XtVaCreateWidget("hp_label",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			GAP,
		XmNtopWidget, 			copyright_label,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			GAP * 2,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);
        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 676, 
				"International Business Machines Corp."));
	ibm_label =
	    XtVaCreateWidget("ibm_label",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			0,
		XmNtopWidget, 			hp_label,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			GAP * 2,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);
        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 677, 
						"Novell, Inc."));
	novell_label =
	    XtVaCreateWidget("novell_label",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			0,
		XmNtopWidget, 			ibm_label,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			GAP * 2,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);
        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 678, 
						"Sun Microsystems, Inc."));
	sun_label =
	    XtVaCreateWidget("sun_label",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			0,
		XmNtopWidget, 			novell_label,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			GAP * 2,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);
#endif

#ifdef CREDITS
	xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 679, "More..."));
	more_button = XtVaCreateWidget("more_button",
		xmPushButtonGadgetClass, 
		main_form,
		XmNlabelString, 		xstr,
		XmNmarginWidth,			5,
		XmNtopAttachment, 		XmATTACH_NONE,
		XmNleftAttachment, 		XmATTACH_WIDGET,
		XmNleftOffset,			GAP,
		XmNleftWidget,			sub_form,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);
	XtAddCallback(more_button, XmNactivateCallback, do_credits, 
			(XtPointer)image);
#endif /* CREDITS */

	separator = XtVaCreateWidget("separator",
		xmSeparatorGadgetClass,
		main_form,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			GAP * 2,
		XmNtopWidget, 			sub_form,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			0,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			0,
		NULL);

	xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 680, "Close"));
	close_button = XtVaCreateWidget("close_button",
		xmPushButtonGadgetClass, main_form,
		XmNlabelString, 		xstr,
		XmNmarginWidth,			5,
		XmNbottomAttachment, 		XmATTACH_FORM,
		XmNbottomOffset,		GAP,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			separator,
		XmNtopOffset, 			GAP,
		NULL);

#ifdef CREDITS
	max_width = ComputeMaxWidth(more_button, close_button, NULL, NULL);
#else
	max_width = ComputeMaxWidth(close_button, close_button, NULL, NULL);
#endif /* CREDITS */

	XmStringFree(xstr);
	XtAddCallback(close_button, XmNactivateCallback, close_about, NULL);
	XtAddCallback(close_button, XmNdestroyCallback, destroy_about, NULL);

#ifdef CREDITS
	XtVaSetValues(more_button,
		XmNwidth,			max_width,
		XmNbottomAttachment, 		XmATTACH_WIDGET,
		XmNbottomWidget, 		separator,
		XmNbottomOffset,		GAP,
		NULL);
#endif /* CREDITS */

	ManageChildren(sub_form);
	ManageChildren(main_form);
	XtManageChild(main_form);

	XtVaGetValues(main_form,
		XmNwidth,			&form_width,
		NULL);

	XtVaSetValues(close_button,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			form_width/2 - max_width/2,
		XmNwidth,			max_width,
		NULL);

        XtVaSetValues(main_form, 
		XmNdefaultButton, 		close_button,
        	XmNcancelButton, 		close_button,
		NULL);

	XtPopup(dialog, XtGrabNone);

	c->about_dialog = dialog;
}

static void
close_about(
        Widget                   w,
        XtPointer                client_data,
        XtPointer                callback_struct)
{
	XtPopdown(calendar->about_dialog);
	XtDestroyWidget(calendar->about_dialog);
	calendar->about_dialog = NULL;
}

static void
destroy_about(
        Widget                   w,
        XtPointer                client_data,
        XtPointer                callback_struct)
{
	calendar->about_dialog = NULL;
}

static void
destroy_credits(
        Widget                   w,
        XtPointer                client_data,
        XtPointer                callback_struct)
{
	calendar->credits_dialog = NULL;
}

static void
close_credits(
        Widget                   w,
        XtPointer                client_data,
        XtPointer                callback_struct)
{
	XtPopdown(calendar->credits_dialog);
	XtDestroyWidget(calendar->credits_dialog);
	calendar->credits_dialog = NULL;
}

#ifdef CREDITS

/*
 * Create and popup the Credits dialog.
 */
static void
do_credits(
	Widget		w,
	XtPointer	client_data,
	XtPointer	callback_struct)
{
	Calendar	*c = calendar;
	Pixmap		image = (Pixmap)client_data;
	Widget 		dialog, main_form, sub_form, separator, close_button,
			icon_label, title_label, label, parent, above_widget;
        XmString	xstr = NULL;
	int		i;
	Dimension	button_width, dialog_width;
	char		*title;

	if (c->credits_dialog) {
		XtPopup(c->credits_dialog, XtGrabNone);
		return;
	}

	title = XtNewString(catgets(c->DT_catd, 1, 681, "Calendar Contributors"));
	dialog = XtVaCreatePopupShell("contrib_dialog",
		xmDialogShellWidgetClass,
		c->frame,
		XmNtitle, 			title,
		XmNdeleteResponse,		XmDESTROY,
		XmNallowShellResize, 		True,
		NULL);
	XtFree(title);

	main_form =
	    XtVaCreateWidget("credit_main_form",
		xmFormWidgetClass,
		dialog,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNbottomAttachment, 		XmATTACH_FORM,
		XmNnoResize,			True,
		NULL);

	icon_label =
	    XtVaCreateWidget("icon_label",
		xmLabelWidgetClass,
		main_form,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			GAP,
		NULL);

	if (!image || image == XmUNSPECIFIED_PIXMAP) {
        	xstr = XmStringCreateLocalized(
			catgets(c->DT_catd, 1, 918, "Missing\nGraphics"));
		XtVaSetValues(icon_label,
			XmNlabelType, 			XmSTRING,
			XmNlabelString, 		xstr,
			XmNborderWidth,			1,
			NULL);
		XmStringFree(xstr);
	} else {
		XtVaSetValues(icon_label,
			XmNlabelType, 			XmPIXMAP,
			XmNlabelPixmap, 		image,
			NULL);
	}

	sub_form =
	    XtVaCreateWidget("credit_sub_form",
		xmFormWidgetClass,
		main_form,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		XmNleftAttachment, 		XmATTACH_WIDGET,
		XmNleftWidget, 			icon_label,
		XmNrightAttachment, 		XmATTACH_FORM,
		NULL);

        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 682, 
				"This application was brought to you by:"));
	title_label =
	    XtVaCreateWidget("title_label",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_FORM,
		XmNtopOffset, 			GAP,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			0,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);

	above_widget = title_label;

	for (i = 0; main_credits[i]; i++) {
        	xstr = XmStringCreateLocalized(main_credits[i]);
		label =
		    XtVaCreateWidget("main_credit_label",
			xmLabelWidgetClass,
			sub_form,
			XmNlabelType, 			XmSTRING,
			XmNlabelString, 		xstr,
			XmNalignment,			XmALIGNMENT_BEGINNING,
			XmNtopAttachment, 		XmATTACH_WIDGET,
			XmNtopWidget,			above_widget,
			XmNtopOffset, 			0,
			XmNleftAttachment, 		XmATTACH_FORM,
			XmNleftOffset, 			90,
			XmNrightAttachment, 		XmATTACH_FORM,
			XmNrightOffset,			GAP,
			NULL);

		above_widget = label;
		XmStringFree(xstr);
	}

        xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 1002, 
				"Also contributing:"));
	title_label =
	    XtVaCreateWidget("title_label",
		xmLabelWidgetClass,
		sub_form,
		XmNlabelType, 			XmSTRING,
		XmNlabelString, 		xstr,
		XmNalignment,			XmALIGNMENT_BEGINNING,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget,			above_widget,
		XmNtopOffset, 			10,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			0,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			GAP,
		NULL);

	XmStringFree(xstr);

	above_widget = title_label;

	for (i = 0; also_credits[i]; i++) {
        	xstr = XmStringCreateLocalized(also_credits[i]);
		label =
		    XtVaCreateWidget("credit_label",
			xmLabelWidgetClass,
			sub_form,
			XmNlabelType, 			XmSTRING,
			XmNlabelString, 		xstr,
			XmNalignment,			XmALIGNMENT_BEGINNING,
			XmNtopAttachment, 		XmATTACH_WIDGET,
			XmNtopWidget,			above_widget,
			XmNtopOffset, 			0,
			XmNleftAttachment, 		XmATTACH_FORM,
			XmNleftOffset, 			90,
			XmNrightAttachment, 		XmATTACH_FORM,
			XmNrightOffset,			GAP,
			NULL);

		above_widget = label;
		XmStringFree(xstr);
	}

	separator = XtVaCreateWidget("separator",
		xmSeparatorGadgetClass,
		main_form,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopOffset, 			GAP * 2,
		XmNtopWidget, 			sub_form,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			0,
		XmNrightAttachment, 		XmATTACH_FORM,
		XmNrightOffset,			0,
		NULL);

	xstr = XmStringCreateLocalized(catgets(c->DT_catd, 1, 680, "Close"));
	close_button = XtVaCreateWidget("close_button",
		xmPushButtonGadgetClass, main_form,
		XmNlabelString, 		xstr,
		XmNmarginWidth,			5,
		XmNbottomAttachment, 		XmATTACH_FORM,
		XmNbottomOffset,		GAP,
		XmNtopAttachment, 		XmATTACH_WIDGET,
		XmNtopWidget, 			separator,
		XmNtopOffset, 			GAP,
		NULL);

	XmStringFree(xstr);
	XtAddCallback(close_button, XmNactivateCallback, close_credits, NULL);
	XtAddCallback(close_button, XmNdestroyCallback, destroy_credits, NULL);

	ManageChildren(sub_form);
	ManageChildren(main_form);
	XtManageChild(main_form);

	XtVaGetValues(dialog,
		XmNwidth,	&dialog_width,
		NULL);
	XtVaGetValues(close_button,
		XmNwidth,	&button_width,
		NULL);

	XtVaSetValues(close_button,
		XmNleftAttachment, 		XmATTACH_FORM,
		XmNleftOffset, 			dialog_width/2 - button_width/2,
		NULL);

        XtVaSetValues(main_form, XmNdefaultButton, close_button, NULL);
        XtVaSetValues(main_form, XmNcancelButton, close_button, NULL);

	XtPopup(dialog, XtGrabNone);

	c->credits_dialog = dialog;
}

#endif /* CREDITS */
