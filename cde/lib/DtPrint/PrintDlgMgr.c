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
/* $XConsortium: PrintDlgMgr.c /main/7 1996/08/12 18:40:50 cde-hp $ */
/*
 * DtPrint/PrintDlgMgr.c
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#include <Dt/PrintDlgMgrP.h>
#include <Dt/PrintMsgsP.h>

#include <Xm/XmAll.h>

#define SEND_MAIL_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintDlgMgrSet, DTPDM_SEND_MAIL_LABEL, _DtPrMsgPrintDlgMgr_0000)
#define BANNER_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintDlgMgrSet, DTPDM_BANNER_LABEL, _DtPrMsgPrintDlgMgr_0001)
#define SPOOL_OPTS_LABEL DTPRINT_GETMESSAGE( \
    MS_PrintDlgMgrSet, DTPDM_SPOOL_OPTS_LABEL, _DtPrMsgPrintDlgMgr_0002)

Widget _DtCreatePDMJobSetup(Widget parent)
{
    Widget manager;
    Widget send_mail, banner_label, banner, spool_opts_label, spool_opts;
    XmString label;
    Dimension banner_label_width, banner_label_height;
    Dimension banner_height;
    Dimension spool_opts_label_width, spool_opts_label_height;
    Dimension spool_opts_height;
    Position offset; /* vertical offset between text fields and their labels */
    Dimension vspace = 10; /* space between the "rows" */
    /*
     * create the manager for the Job Setup Box
     */
    manager =
	XtVaCreateManagedWidget("JobSetup", xmFormWidgetClass, parent,
				XmNallowOverlap, False,
				NULL);
    /*
     * send mail control
     */
    label = XmStringCreateLocalized(SEND_MAIL_LABEL);
    send_mail =
	XmVaCreateSimpleCheckBox(manager, "SendMail", NULL,
				 XmVaCHECKBUTTON, label, NULL, NULL, NULL,
				 NULL);
    XmStringFree(label);
    XtManageChild(send_mail);
    /*
     * banner page title
     */
    label = XmStringCreateLocalized(BANNER_LABEL);
    banner_label =
	XtVaCreateManagedWidget("BannerLabel",
				xmLabelWidgetClass,
				manager,
				XmNlabelString, label,
				XmNalignment, XmALIGNMENT_END,
				NULL);
    XmStringFree(label);
    banner =
	XtVaCreateManagedWidget("Banner", 
				xmTextFieldWidgetClass,
				manager,
				NULL);
    /*
     * spooler command options
     */
    label = XmStringCreateLocalized(SPOOL_OPTS_LABEL);
    spool_opts_label =
	XtVaCreateManagedWidget("OptionsLabel",
				xmLabelWidgetClass,
				manager,
				XmNlabelString, label,
				XmNalignment, XmALIGNMENT_END,
				NULL);
    XmStringFree(label);
    spool_opts =
	XtVaCreateManagedWidget("Options", 
				xmTextFieldWidgetClass,
				manager,
				NULL);
    /*
     * retrieve various dimensions of form children
     */
    XtVaGetValues(banner_label,
		  XmNwidth, &banner_label_width,
		  XmNheight, &banner_label_height,
		  NULL);
    XtVaGetValues(banner,
		  XmNheight, &banner_height,
		  NULL);
    XtVaGetValues(spool_opts_label,
		  XmNwidth, &spool_opts_label_width,
		  XmNheight, &spool_opts_label_height,
		  NULL);
    XtVaGetValues(spool_opts,
		  XmNheight, &spool_opts_height,
		  NULL);
    /*
     * set the width of the shorter text field to match the longest
     */
    if(banner_label_width < spool_opts_label_width)
	XtVaSetValues(banner_label,
		      XmNwidth, spool_opts_label_width,
		      XmNrecomputeSize, False,
		      NULL);
    else
	XtVaSetValues(spool_opts_label,
		      XmNwidth, banner_label_width,
		      XmNrecomputeSize, False,
		      NULL);
    /*
     * layout the form children
     */
    XtVaSetValues(send_mail,
		  XmNleftAttachment,             XmATTACH_FORM,
		  XmNtopAttachment,              XmATTACH_FORM,
		  NULL);
    offset = ((Position)banner_height - (Position)banner_label_height) / 2;
    XtVaSetValues(banner_label,
		  XmNleftAttachment,             XmATTACH_FORM,
		  XmNtopAttachment,              XmATTACH_WIDGET,
		  XmNtopWidget,                  send_mail,
		  XmNtopOffset, vspace + (offset > 0 ? offset : 0),
		  NULL);
    XtVaSetValues(banner,
		  XmNleftAttachment,             XmATTACH_WIDGET,
		  XmNleftWidget,                 banner_label,
		  XmNrightAttachment,            XmATTACH_FORM,
		  XmNtopAttachment,              XmATTACH_OPPOSITE_WIDGET,
		  XmNtopWidget,                  banner_label,
		  XmNtopOffset,                  -offset,
		  NULL);
    offset =
	((Position)spool_opts_height - (Position)spool_opts_label_height) / 2;
    XtVaSetValues(spool_opts_label,
		  XmNleftAttachment,             XmATTACH_FORM,
		  XmNtopAttachment,              XmATTACH_WIDGET,
		  XmNtopWidget,                  banner_label,
		  XmNtopOffset, vspace + (offset > 0 ? offset : 0),
		  NULL);
    XtVaSetValues(spool_opts,
		  XmNleftAttachment,             XmATTACH_WIDGET,
		  XmNleftWidget,                 spool_opts_label,
		  XmNrightAttachment,            XmATTACH_FORM,
		  XmNtopAttachment,              XmATTACH_OPPOSITE_WIDGET,
		  XmNtopWidget,                  spool_opts_label,
		  XmNtopOffset,                  -offset,
		  NULL);
    /*
     * return the new Job Setup Box
     */
    return manager;
}
