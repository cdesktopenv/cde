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
/* $TOG: DmxPrintSetup.C /main/17 1997/09/03 17:34:59 mgreess $ */

/*
 *+SNOTICE
 *
 *	$:$
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1994 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

/*
 *		     Common Desktop Environment
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *   (c) Copyright 1995 Digital Equipment Corp.
 *   (c) Copyright 1995 Fujitsu Limited
 *   (c) Copyright 1995 Hitachi, Ltd.
 *                                                                   
 *
 *                     RESTRICTED RIGHTS LEGEND                              
 *
 *Use, duplication, or disclosure by the U.S. Government is subject to
 *restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 *Technical Data and Computer Software clause in DFARS 252.227-7013.  Rights
 *for non-DOD U.S. Government Departments and Agencies are as set forth in
 *FAR 52.227-19(c)(1,2).

 *Hewlett-Packard Company, 3000 Hanover Street, Palo Alto, CA 94304 U.S.A.
 *International Business Machines Corp., Route 100, Somers, NY 10589 U.S.A. 
 *Sun Microsystems, Inc., 2550 Garcia Avenue, Mountain View, CA 94043 U.S.A.
 *Novell, Inc., 190 River Road, Summit, NJ 07901 U.S.A.
 *Digital Equipment Corp., 111 Powdermill Road, Maynard, MA 01754, U.S.A.
 *Fujitsu Limited, 1015, Kamikodanaka Nakahara-Ku, Kawasaki 211, Japan
 *Hitachi, Ltd., 6, Kanda Surugadai 4-Chome, Chiyoda-ku, Tokyo 101, Japan
 */
#include <stdio.h>
#include <sys/param.h>

#include <Dt/Print.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>

#include "Dmx.h"
#include "DmxPrintOptions.h"
#include "DmxPrintSetup.h"
#include "DtMailHelp.hh"
#include "MailMsg.h"
#include "OptCmd.h"
#include "RoamApp.h"
#include "RoamMenuWindow.h"

static DtMailBoolean	DmxPrintSetup_print_separately = DTM_FALSE;
static DtMailBoolean	DmxPrintSetup_use_word_wrap = DTM_TRUE;
static DtMailBoolean	DmxPrintSetup_print_to_file = DTM_FALSE;
static char		*DmxPrintSetup_printer_name = NULL;
#ifdef REUSE_PRINT_SETUP_DIALOGS
static Widget		DmxPrintSetup_default_dtprint_setup = NULL;
#endif

DmxPrintSetup::DmxPrintSetup (
		Widget window,
		XtCallbackProc printCB, XtPointer printClosure,
		XtCallbackProc cancelCB, XtPointer cancelClosure,
		XtCallbackProc closeDisplayCB, XtPointer closeDisplayClosure,
		XtCallbackProc pdmSetupCB, XtPointer pdmSetupClosure)
{
    _parent = window;
    _dtprint_setup = (Widget) NULL;
    _widgets = (PrintSetupWidgets *) NULL;
    _use_word_wrap = DmxPrintSetup_use_word_wrap;
    _print_separately = DmxPrintSetup_print_separately;
    _print_to_file = DmxPrintSetup_print_to_file;
    if (NULL != DmxPrintSetup_printer_name)
      _printer_name = strdup(DmxPrintSetup_printer_name);
    else
      _printer_name = NULL;

    _filename = (char*) malloc(MAXPATHLEN+1);
    if (NULL != _filename)
      sprintf(_filename, "%s/dtmail_messages.ps", getenv("HOME"));

    _printCB = printCB;
    _cancelCB = cancelCB;
    _closeDisplayCB = closeDisplayCB;
    _pdmSetupCB = pdmSetupCB;

    _printClosure = printClosure;
    _cancelClosure = cancelClosure;
    _closeDisplayClosure = closeDisplayClosure;
    _pdmSetupClosure = pdmSetupClosure;

    attachPrintSetupDialog();
    XtRealizeWidget(_dtprint_setup);
}

DmxPrintSetup::~DmxPrintSetup (void)
{
    if (NULL != _printer_name)
      free(_printer_name);

    if (NULL != _filename)
      free(_filename);

    detachPrintSetupDialog();
}

void
DmxPrintSetup::setPrintToFileName (char *filename)
{
    if (NULL != _filename)
      free(_filename);
    
    _filename = strdup(filename);

    if (NULL != _filename)
      XtVaSetValues(_dtprint_setup, DtNfileName, _filename, NULL);
}

void
DmxPrintSetup::display (void)
{
    if (_dtprint_setup == NULL) return;

#ifdef REUSE_PRINT_SETUP_DIALOGS
    XtArgVal /* Position */ x1, y1, x2, y2;
    XtArgVal /* Dimension */ w1, h1, w2, h2;
    XtVaGetValues(
		XtParent(_dtprint_setup),
		XmNx, &x1, XmNy, &y1,
		XmNwidth, &w1, XmNheight, &h1,
		NULL);
    XtVaGetValues(
		_parent,
		XmNx, &x2, XmNy, &y2,
		XmNwidth, &w2, XmNheight, &h2,
		NULL);
    XtVaSetValues(
		XtParent(_dtprint_setup),
		XmNx, x2 + (w2 - w1) / 2, XmNy, y2 + (h2 - h1) / 2,
		NULL);
#endif

    XtManageChild(_dtprint_setup);

    if (NULL != _printer_name)
      XtVaSetValues(_widgets->printer_name_tf, XmNvalue, _printer_name, NULL);
}

DtMailBoolean
DmxPrintSetup::getDefaultPrintData (DtPrintSetupData *print_data)
{
    if (_dtprint_setup == NULL || print_data == NULL) return DTM_FALSE;
    if (DtPRINT_SUCCESS == DtPrintFillSetupData(_dtprint_setup, print_data))
      return DTM_TRUE;
    else
      return DTM_FALSE;
}

DtMailBoolean
DmxPrintSetup::printSeparately (void)
{
    if (DTM_TRUE != _print_to_file)
      return _print_separately;

    return DTM_FALSE;
}

DtMailBoolean
DmxPrintSetup::useWordWrap (void)
{
    return _use_word_wrap;
}

void
DmxPrintSetup::attachPrintSetupDialog (void)
{
    unsigned char is_set;
    Widget dialog;

#ifdef REUSE_PRINT_SETUP_DIALOGS
    if (DmxPrintSetup_default_dtprint_setup != NULL)
    {
	//
	// Attempt to reuse the last print setup and print display connection.
	//
	_dtprint_setup = DmxPrintSetup_default_dtprint_setup;
	DmxPrintSetup_default_dtprint_setup = NULL;
    }
    else
      _dtprint_setup = createPrintSetupDialog(theRoamApp.baseWidget());
#else
    _dtprint_setup = createPrintSetupDialog(_parent);
#endif

    //
    // Copy the information about the widgets into the setup.
    //
    XtVaGetValues(_dtprint_setup, XmNuserData, &_widgets, NULL);

    //
    // Add the setup-specific callbacks
    //
    XtAddCallback(
		_dtprint_setup,
		DtNprintCallback,
		DmxPrintSetup::printCB,
		(XtPointer) this);
    XtAddCallback(
		_dtprint_setup,
		DtNcancelCallback,
		_cancelCB,
		(XtPointer) _cancelClosure);
    XtAddCallback(
    		_dtprint_setup,
		DtNclosePrintDisplayCallback,
		_closeDisplayCB,
		(XtPointer) _closeDisplayClosure);
    XtAddCallback(
    		_dtprint_setup,
		DtNsetupCallback,
		_pdmSetupCB,
		(XtPointer) _pdmSetupClosure);

    is_set = (_print_separately) ? XmSET : XmUNSET;
    XtVaSetValues(_widgets->print_separately_tb, XmNset, is_set, NULL);

    is_set = (_use_word_wrap) ? XmSET : XmUNSET;
    XtVaSetValues(_widgets->use_word_wrap_tb, XmNset, is_set, NULL);

    if (DTM_TRUE == _print_to_file)
      XtVaSetValues(
		_dtprint_setup,
		DtNprintDestination, DtPRINT_TO_FILE,
		NULL);
    else
      XtVaSetValues(
		_dtprint_setup,
		DtNprintDestination, DtPRINT_TO_PRINTER,
		NULL);

    if (NULL != _printer_name)
      XtVaSetValues(_dtprint_setup, DtNprinterName, _printer_name, NULL);

    if (NULL != _filename)
      XtVaSetValues(_dtprint_setup, DtNfileName, _filename, NULL);

    dialog = XtParent(_dtprint_setup);
    if (NULL != dialog && XtIsShell(dialog))
    {
	char *title = (char*) GETMSG(DT_catd, 21, 21, "Mailer - Print Setup");
        XtVaSetValues(dialog, XmNtitle, title, NULL);
    }
    else
    {
        fprintf(stderr, "Print Setup box is not parented to a shell\n");
    }
}


Widget
DmxPrintSetup::createPrintSetupDialog (Widget parent)
{
    PrintSetupWidgets	*widgets;
    XmString		xms;

    //
    // Create the app-specific widgets for the Setup Dialog.
    widgets = (PrintSetupWidgets *) XtMalloc(sizeof(PrintSetupWidgets));

    //
    // Create the DtPrintSetupDialog and specify that the application
    // specific area be located below the generic area.
    // Save the PrintSetupWidgets record as UserData so it can be
    // retrieved later.
    //
    widgets->dtprint_setup =
      DtCreatePrintSetupDialog(parent, "Setup", NULL, 0);

    XtVaSetValues(
		widgets->dtprint_setup,
		DtNworkAreaLocation, DtWORK_AREA_BOTTOM,
		XmNuserData, widgets,
		NULL);

    XtAddCallback(
		widgets->dtprint_setup,
		XmNhelpCallback,
		HelpCB,
		(void *)DTMAILPRINTSETUPDIALOG);

    XtAddCallback(
		widgets->dtprint_setup,
		XmNdestroyCallback,
		&DmxPrintSetup::destroyPrintSetupDialogCB,
		(XtPointer) widgets);

    widgets->form = XtVaCreateWidget(
		"PrintSetupForm",
		xmFormWidgetClass,
		widgets->dtprint_setup,
		NULL);
		 
    xms = XmStringCreateLocalized(GETMSG(DT_catd, 21, 12, "Print Separately"));
    widgets->print_separately_tb = XtVaCreateManagedWidget(
				"PrintSeparatelyTB",
				xmToggleButtonWidgetClass,
				widgets->form,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNlabelString, xms,
				XmNleftAttachment, XmATTACH_FORM,
				XmNtopAttachment, XmATTACH_FORM,
				NULL);
    XmStringFree(xms);

    xms = XmStringCreateLocalized(GETMSG(DT_catd, 21, 13, "Use Word Wrap"));
    widgets->use_word_wrap_tb = XtVaCreateManagedWidget(
				"UseWordWrapTB",
			  	xmToggleButtonWidgetClass,
				widgets->form,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNlabelString, xms,
				XmNleftAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, widgets->print_separately_tb,
				NULL);
    XmStringFree(xms);

    xms = XmStringCreateLocalized(GETMSG(DT_catd, 21, 20, "More ..."));
    widgets->more_options_pb = XtVaCreateManagedWidget(
				"PrintOptionsPB",
			  	xmPushButtonWidgetClass,
				widgets->form,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNlabelString, xms,
				XmNleftAttachment, XmATTACH_NONE,
				XmNrightAttachment, XmATTACH_FORM,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, widgets->use_word_wrap_tb,
				NULL);
    XmStringFree(xms);

    XtAddCallback(
		widgets->more_options_pb,
		XmNactivateCallback,
		&DmxPrintSetup::moreOptionsCB,
		(XtPointer) NULL);

    widgets->checkbox_tb =
      XtNameToWidget(widgets->dtprint_setup, "DestRadioBox.button_1");
    widgets->printer_name_tf = XtNameToWidget(widgets->dtprint_setup, "Name");

    if (NULL != widgets->checkbox_tb)
      XtAddCallback(
		widgets->checkbox_tb,
		XmNvalueChangedCallback,
		&DmxPrintSetup::destinationChangedCB,
		(XtPointer) widgets);

    XtManageChild(widgets->form);
    return(widgets->dtprint_setup);
}


void
DmxPrintSetup::detachPrintSetupDialog (void)
{
    XtRemoveCallback(
		_dtprint_setup,
		DtNprintCallback,
		DmxPrintSetup::printCB,
		(XtPointer) this);
    XtRemoveCallback(
		_dtprint_setup,
		DtNcancelCallback,
		_cancelCB,
		(XtPointer) _cancelClosure);
    XtRemoveCallback(
		_dtprint_setup,
		DtNclosePrintDisplayCallback,
		_closeDisplayCB,
		(XtPointer) _closeDisplayClosure);
    XtRemoveCallback(
		_dtprint_setup,
		DtNsetupCallback,
		_pdmSetupCB,
		(XtPointer) _pdmSetupClosure);

    //
    // Leave the Print Setup Dialog for the next print job.
    //
#ifdef REUSE_PRINT_SETUP_DIALOGS
    if (DmxPrintSetup_default_dtprint_setup != NULL)
    {
	XtDestroyWidget(DmxPrintSetup_default_dtprint_setup);
	DmxPrintSetup_default_dtprint_setup = NULL;
    }
    DmxPrintSetup_default_dtprint_setup = _dtprint_setup;
#else
    XtDestroyWidget(_dtprint_setup);
#endif
}


void
DmxPrintSetup::savePrintSetupOptions(DtPrintSetupData *print_data)
{
    XtArgVal /* unsigned char */	is_set;

    XtVaGetValues(_widgets->print_separately_tb, XmNset, &is_set, NULL);
    _print_separately = (is_set == XmSET) ? DTM_TRUE : DTM_FALSE;
    DmxPrintSetup_print_separately = _print_separately;

    XtVaGetValues(_widgets->use_word_wrap_tb, XmNset, &is_set, NULL);
    _use_word_wrap = (is_set == XmSET) ? DTM_TRUE : DTM_FALSE;
    DmxPrintSetup_use_word_wrap = _use_word_wrap;
    
    _print_to_file =
	(DtPRINT_TO_FILE == print_data->destination) ? DTM_TRUE : DTM_FALSE;
    DmxPrintSetup_print_to_file = _print_to_file;

    if (NULL != _printer_name)
      free(_printer_name);
    _printer_name = strdup(print_data->printer_name);

    if (NULL != DmxPrintSetup_printer_name)
      free(DmxPrintSetup_printer_name);
    DmxPrintSetup_printer_name = strdup(_printer_name);

    if (NULL != _filename)
      free(_filename);
    _filename = strdup(print_data->dest_info);
}


void
DmxPrintSetup::destinationChangedCB(
				Widget checkbox_tb,
				XtPointer client_data,
				XtPointer)
{
    PrintSetupWidgets	*widgets = (PrintSetupWidgets*) client_data;
    XtArgVal /* Boolean */ toggleFlag;

    if (NULL == checkbox_tb) return;

    XtVaGetValues(checkbox_tb, XmNset, &toggleFlag, NULL);
    if(toggleFlag)
      XtVaSetValues(
		widgets->print_separately_tb,
		XmNsensitive, False,
		XmNset, False,
		NULL);
    else
      XtVaSetValues(widgets->print_separately_tb, XmNsensitive, True, NULL);
}


void
DmxPrintSetup::destroyPrintSetupDialogCB(Widget, XtPointer widgets, XtPointer)
{
    XtFree((char *) widgets);
}


void
DmxPrintSetup::moreOptionsCB(Widget, XtPointer, XtPointer)
{
    OptCmd		*optCmd = (OptCmd *) theRoamApp.mailOptions();

    optCmd->displayPrintingOptionsPane();
}

void
DmxPrintSetup::printCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    DmxPrintSetup		*thisSetup = (DmxPrintSetup*) client_data;
    DtPrintSetupCallbackStruct	*pbs = (DtPrintSetupCallbackStruct*) call_data;

    thisSetup->_printCB(w, thisSetup->_printClosure, call_data);
    thisSetup->savePrintSetupOptions(pbs->print_data);
}
