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
/*
 *+SNOTICE
 *
 *      $XConsortium: Help.C /main/7 1996/10/11 20:04:16 cde-hp $
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <Xm/Xm.h>
#include <Dt/HelpDialog.h>
#include <Dt/HelpQuickD.h>
#include <Dt/Help.h>
#include <Dt/Editor.h>
#include "Help.hh"

#include <nl_types.h>
extern nl_catd catd;

#include "NLS.hh"

void DisplayMain( Widget, char *, char *);
static Widget helpMain = NULL;
static Widget helpError = NULL;
static Widget versionMain = NULL;

Widget
getErrorHelpWidget(void)
{
    return (helpError);
}

void
clearErrorHelpWidget(void)
{
    helpError = NULL;
}

char *
getHelpId (Widget w)
{
    char *helpId;
    char *buf;
    char *index;
    int i = 0, j = 0;
 
    if (XtParent(w) == NULL) {
        helpId = (char *) malloc(1000);
	index = helpId;
	buf = XtName(w);
	while(*buf) {
	    if (isalnum(*buf)) {
		*index++ = toupper(*buf);
	    } else if ('_' == *buf) {
		*index++ = '-';
	    }
	    buf++;
	}
	*index++ = '\0';
        return (helpId);
    } else {
        helpId = getHelpId (XtParent(w));
	i = strlen(helpId);
	buf = XtName(w);
	while(*buf) {
	    if (isalnum(*buf)) {
		helpId[i++] = toupper(*buf);
	    } else if ('_' == *buf) {
		helpId[i++] = '-';
	    }
	    buf++;
	}
	helpId[i++] = '\0';
        return (helpId);
    }
 
}


void
DisplayVersion (
	Widget parent,
	char *helpVolume,
	char *locationId )
{
    Arg		args[10];
    int		n;
    Widget	printWidget;
    Widget	helpWidget;

    if (versionMain != NULL) {
	n = 0;
	XtSetArg (args[n], XmNtitle, GETMSG(catd, 1, 4,
	  "DtMail Version Dialog")); n++;
	if (helpVolume != NULL) {
	    XtSetArg (args[n], DtNhelpVolume, helpVolume); n++;
	}
	XtSetArg (args[n], DtNlocationId, locationId); n++;
	XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
	XtSetValues(versionMain, args, n);
	XtManageChild(versionMain);
    } else {
	while (!XtIsSubclass(parent, applicationShellWidgetClass))
	    parent = XtParent(parent);

	// Build a new one in our cached list
	n = 0;
	XtSetArg (args[n], XmNtitle, GETMSG(catd, 1, 5,
	  "DtMail Version Dialog")); n++;
	if (helpVolume != NULL) {
	    XtSetArg (args[n], DtNhelpVolume, helpVolume); n++;
	}
	XtSetArg (args[n], DtNlocationId, locationId); n++;
	XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
	versionMain = DtCreateQuickHelpDialog(parent, "versionWidget", args, n);
	XtAddCallback(versionMain, XmNokCallback,
		    CloseMainCB, (XtPointer) versionMain);
	
	// We do not want a print button for now so we unmap it
	printWidget = DtHelpQuickDialogGetChild (versionMain,
					    DtHELP_QUICK_PRINT_BUTTON);
	XtUnmanageChild (printWidget);

	// We do not want a help button for now so we unmap it 

	helpWidget = DtHelpQuickDialogGetChild (versionMain,
					    DtHELP_QUICK_PRINT_BUTTON);
	XtUnmanageChild (helpWidget);
	XtManageChild(versionMain);
    }
}


#ifdef DEAD_WOOD
// The callback for the Help Menu in the combo window.

void
HelpMenuCB (
	Widget widget,
	XtPointer	clientdata,
	XtPointer)
{
    Widget selWidget = NULL;
    int	status = DtHELP_SELECT_ERROR;

    // Determine which help button was activated and display the
    // appropriate help information.

    switch ((long) clientdata) {
	case HELP_ON_ITEM:
	    while (!XtIsSubclass(widget, applicationShellWidgetClass))
		widget = XtParent(widget);
	    status = DtHelpReturnSelectedWidgetId(widget, NULL, &selWidget);

	    switch ((int) status) {
		case DtHELP_SELECT_ERROR:
		    printf(GETMSG(catd, 2, 1, "Selection Error, cannot continue\n"));
		    break;
		case DtHELP_SELECT_VALID:
		    while (selWidget != NULL) {
			if ((XtHasCallbacks(selWidget, XmNhelpCallback)
					== XtCallbackHasSome)) {
			    XtCallCallbacks((Widget)selWidget,
					XmNhelpCallback, NULL);
			    break;
			} else {
			    selWidget = XtParent(selWidget);
			}
		    }
		    break;
		case DtHELP_SELECT_ABORT:
		    printf(GETMSG(catd, 2, 2, "Selection aborted by user.\n"));
		    break;
		case DtHELP_SELECT_INVALID:
		    printf(GETMSG(catd, 1, 6, 
			   "You must select a component withing your app.\n"));
		    break;
	    }
	    break;
	case HELP_ON_TOPIC:
	    DisplayMain(widget, NULL, APP_MENU_ID);
	    break;
	case HELP_ON_VERSION:
	    DisplayVersion(widget, NULL, VER_MENU_ID);
	    break;
	default:
	    break;
    }
}
#endif /* DEAD_WOOD */

	
// Callback to process JUMP-NEW and APP-LINK hypertext requests in a
// given Help Dialog Window.
//
// This is the callback used for the DtNhyperLinkCallback
// on each of the help dialog widgets created.

static void
ProcessLinkCB (
	Widget,
	XtPointer,
	XtPointer callData)
{
//    Arg 	args[20];
//    Position 	xPos, yPos;
    int 	appLinkNum = 0;

    DtHelpDialogCallbackStruct * hyperData =
		(DtHelpDialogCallbackStruct *) callData;
    
}


void
DisplayMain(
	Widget parent,
	char *helpVolume,
	char *locationId)
{
    Arg	args[10];
    int n;

    if (helpMain != NULL) {
	n = 0;
#ifdef undef
	XtSetArg (args[n], XmNtitle, GETMSG(catd, 1, 7, "DtMail Help")); n++;
#endif
	XtSetArg (args[n], XmNtitle, GETMSG(catd, 1, 12, "Mailer : Help")); n++;
	if (helpVolume != NULL) {
	    XtSetArg (args[n], DtNhelpVolume, helpVolume); n++;
	}
	XtSetArg (args[n], DtNlocationId, locationId); n++;
	XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
	XtSetValues (helpMain, args, n);
	XtUnmanageChild(helpMain);
	XtManageChild(helpMain);
    } else {
	while (!XtIsSubclass(parent, applicationShellWidgetClass))
	    parent = XtParent(parent);
	
	// Build a new one in our cached list
	n = 0;
	XtSetArg (args[n], XmNtitle, GETMSG(catd, 1, 12, "Mailer : Help")); n++;
	if (helpVolume != NULL) {
	    XtSetArg (args[n], DtNhelpVolume, helpVolume); n++;
	}
	XtSetArg (args[n], DtNlocationId, locationId); n++;
	XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
	helpMain = DtCreateHelpDialog(parent, "Mailer", args, n);

	XtAddCallback(helpMain, DtNhyperLinkCallback, ProcessLinkCB, NULL);
	XtAddCallback(
		helpMain,
		DtNcloseCallback, CloseMainCB, (XtPointer) helpMain);
	
	XtManageChild(helpMain);
    }
}

void
DisplayErrorHelp(
	Widget parent,
	char *helpVolume,
	char *locationId)
{
    Arg	args[10];
    int n;

    if (helpError) {
	n = 0;
#ifdef undef
	XtSetArg (args[n], XmNtitle, GETMSG(catd, 1, 7, "DtMail Help")); n++;
#endif
	XtSetArg (args[n], XmNtitle, GETMSG(catd, 1, 12, "Mailer : Help")); n++;
	if (helpVolume != NULL) {
	    XtSetArg (args[n], DtNhelpVolume, helpVolume); n++;
	}
	XtSetArg (args[n], DtNlocationId, locationId); n++;
	XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
	//XtSetArg (args[n], DtNlocationId, "DTMAILVIEWMAINWINDOWWORK-AREA"); n++;
	XtSetValues (helpError, args, n);
	XtManageChild(helpError);
    } else {
    
	// Create a new help on the error dialogs each time, destroy it
	// when done.
	n = 0;
#ifdef undef
	XtSetArg (args[n], XmNtitle, GETMSG(catd, 1, 8, "DtMail Help")); n++;
#endif
	XtSetArg (args[n], XmNtitle, GETMSG(catd, 1, 12, "Mailer : Help")); n++;
	if (helpVolume != NULL) {
	XtSetArg (args[n], DtNhelpVolume, helpVolume); n++;
	}
	//XtSetArg (args[n], DtNlocationId, "DTMAILVIEWMAINWINDOWWORK-AREAPANEDWFORM2ROWCOLUMNPREVIOUS"); n++;
	XtSetArg (args[n], DtNlocationId, locationId); n++;
	XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
	helpError = DtCreateHelpDialog(parent, "Mailer", args, n);

	XtAddCallback(helpError, DtNhyperLinkCallback, ProcessLinkCB, NULL);

	XtAddCallback(helpError, DtNcloseCallback, 
		CloseMainCB, (XtPointer) helpError);

	XtUnmanageChild(helpError);
	XtManageChild(helpError);
    }
}


// The Help Callback for when the F1 key is pressed or when On Item
// Help is selected from the Help menu.

void
HelpCB (
	Widget w,
	XtPointer clientData,
	XtPointer)
{
    char *locationId = (char *) clientData;

    // printf("locationId = %s\n", locationId);
    // Just display the proper help based on the id string passed in.

    // We pass in a NULL for our helpVolume and let the value defined
    // in the app-defaults file be used.

    DisplayMain (w, "Mailer", locationId);
}

void
HelpErrorCB (
	Widget w,
	XtPointer clientData,
	XtPointer)
{
    char *locationId = (char *) clientData;

    // printf("locationId = %s\n", locationId);
    // Just display the proper help based on the id string passed in.

    // We pass in a NULL for our helpVolume and let the value defined
    // in the app-defaults file be used.

    DisplayErrorHelp (w, "Mailer", locationId);
}

void
HelpTexteditCB (
	Widget w,
	XtPointer clientData,
	XtPointer callData )
{

     char *locationId = NULL;
     Widget wEditor = (Widget) clientData;
     DtEditorHelpCallbackStruct *editorHelp =
                         (DtEditorHelpCallbackStruct *) callData;
 
     switch (editorHelp->reason) {
 
 	/* -----> edit area */
 	case DtEDITOR_HELP_EDIT_WINDOW:
 	    locationId = EDIT_AREA_HELP;
 	    break;
 
 	/* -----> status line area and fields */
 	case DtEDITOR_HELP_STATUS_LINE:
 	    locationId = STATUS_LINE_HELP;
 	    break;
 	case DtEDITOR_HELP_STATUS_CURRENT_LINE:
 	    locationId = STATUS_CURRENT_LINE_HELP;
 	    break;
 	case DtEDITOR_HELP_STATUS_TOTAL_LINES:
 	    locationId = STATUS_TOTAL_LINES_HELP;
 	    break;
 	case DtEDITOR_HELP_STATUS_MESSAGE:
 	    locationId = STATUS_MESSAGE_HELP;
 	    break;
 	case DtEDITOR_HELP_STATUS_OVERSTRIKE:
 	    locationId = STATUS_OVERSTRIKE_HELP;
 
 	/* -----> Format Settings dialog and dialog fields */
 	case DtEDITOR_HELP_FORMAT_DIALOG:
 	    locationId = FORMAT_SETTINGS_HELP;
 	    break;
 	case DtEDITOR_HELP_FORMAT_LEFT_MARGIN:
 	    locationId = FORMAT_LEFT_MARGIN_HELP;
 	    break;
 	case DtEDITOR_HELP_FORMAT_RIGHT_MARGIN:
 	    locationId = FORMAT_RIGHT_MARGIN_HELP;
 	    break;
 	case DtEDITOR_HELP_FORMAT_ALIGNMENT:
 	    locationId = FORMAT_ALIGNMENT_HELP;
 	    break;
 
 	/* -----> Find/Change dialog and dialog fields */
 	case DtEDITOR_HELP_CHANGE_DIALOG:
 	    locationId = FINDCHANGE_HELP;
 	    break;
 	case DtEDITOR_HELP_CHANGE_FIND:
 	    locationId = FINDCHANGE_FIND_HELP;
 	    break;
 	case DtEDITOR_HELP_CHANGE_CHANGE:
 	    locationId = FINDCHANGE_CHANGETO_HELP;
 	    break;
 
 	/* -----> Check Spelling dialog and dialog fields */
 	case DtEDITOR_HELP_SPELL_DIALOG:
 	    locationId = SPELL_HELP;
 	    break;
 	case DtEDITOR_HELP_SPELL_MISSPELLED_WORDS:
 	    locationId = SPELL_MISSPELLED_WORDS_HELP;
 	    break;
 	case DtEDITOR_HELP_SPELL_CHANGE:
 	    locationId = SPELL_CHANGETO_HELP;
 	    break;
 
  	default:
 	    ;
 
     } /* switch (editorHelp->reason) */
 
   DisplayMain (w, "Textedit", locationId);
}
  

// Callback to process close requests on our main help dialog.

static void
CloseMainCB (
	Widget,
	XtPointer	clientData,
	XtPointer)
{
    Widget currentDialog = (Widget) clientData;

    // Unmap and clean up help widget

    XtUnmanageChild(currentDialog);
}

#if defined(PRINT_HELPIDS)

void
printHelpId (char *w_name, Widget w)
{
    char *helpId;
 
    helpId = getHelpId (w);
    printf("%s = %s\n", w_name, helpId);
    free(helpId);
 
}

#else

void
printHelpId (char *, Widget)
{
}

#endif
