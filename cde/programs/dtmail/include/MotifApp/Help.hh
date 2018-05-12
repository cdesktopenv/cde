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
/* $XConsortium: Help.hh /main/5 1996/10/11 20:05:13 cde-hp $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 *+SNOTICE
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
 
#ifndef HELP_HH
#define HELP_HH
 
#include <Dt/Help.h>

// Option defines for menubar help access
#define HELP_ON_ITEM     1
#define HELP_ON_TOPIC    2
#define HELP_ON_VERSION  3

#define DTMAILCONTAINERMENUID "DTMAILVIEWMAINWINDOWMENUBARFILE"
#define DTMAILEDITMENUID "DTMAILVIEWMAINWINDOWMENUBAREDIT"
#define DTMAILMESSAGEMENUID "DTMAILVIEWMAINWINDOWMENUBARMESSAGE"
#define DTMAILATTACHMENUID "DTMAILVIEWMAINWINDOWMENUBARATTACH"
#define DTMAILVIEWMENUID "DTMAILVIEWMAINWINDOWMENUBARVIEW"
#define DTMAILCOMPOSEMENUID "DTMAILVIEWMAINWINDOWMENUBARCOMPOSE"

#define APP_MENU_ID "onApplicationMenu"
#define VER_MENU_ID "_copyright"

/************************************************************************
* Help location ids for edit area and status line of the DtEditor widget
************************************************************************/
#define EDIT_AREA_HELP				"TextEditorWindow"


/* -----> fields/buttons within the status line */
#define STATUS_LINE_HELP			"statusLine"
#define STATUS_CURRENT_LINE_HELP		"status-currentLine"
#define STATUS_TOTAL_LINES_HELP			"status-totalLines"
#define STATUS_MESSAGE_HELP			"status-message"
#define STATUS_OVERSTRIKE_HELP			"status-overstrike"

/************************************************************************
 * Help location ids for the dialog posted by [Format] menu [Settings...]
 * (this dialog is controlled by the DtEditor widget)
 ************************************************************************/
#define FORMAT_SETTINGS_HELP			"formatDialog"

/* -----> fields/buttons within the Format Settings dialog */
#define FORMAT_LEFT_MARGIN_HELP			"format-leftmargin"
#define FORMAT_RIGHT_MARGIN_HELP		"format-rightmargin"
#define FORMAT_ALIGNMENT_HELP			"format-alignment"

/************************************************************************
 * Help location ids for the dialog posted by [Edit] menu [Find/Change...]
 * (this dialog is controlled by the DtEditor widget)
 ************************************************************************/
#define FINDCHANGE_HELP				"findchangeDialog"
/* -----> fields/buttons within the Find/Change dialog */
#define FINDCHANGE_FIND_HELP			"fc-find"
#define FINDCHANGE_CHANGETO_HELP		"fc-changeto"

/************************************************************************
 * Help location ids for the dialog posted by [Edit] menu [Find/Change...]
 * (this dialog is controlled by the DtEditor widget)
 ************************************************************************/
#define SPELL_HELP				"spellDialog"

/* -----> fields/buttons within the Check Spelling dialog */
#define SPELL_MISSPELLED_WORDS_HELP		"sp-spelllist"
#define SPELL_CHANGETO_HELP			"sp-changeto"


char *getHelpId(Widget);
void printHelpId(char *, Widget);
#ifdef DEAD_WOOD
void HelpMenuCB(Widget, XtPointer, XtPointer);
#endif /* DEAD_WOOD */
void HelpCB(Widget, XtPointer, XtPointer);
void HelpTexteditCB( Widget, XtPointer, XtPointer ) ;
extern void DisplayMain(Widget, char *, char *);
Widget getErrorHelpWidget(void);
void clearErrorHelpWidget(void);
extern void DisplayErrorHelp(Widget, char *, char *);
void HelpErrorCB(Widget, XtPointer, XtPointer);
extern void DisplayVersion(Widget, char *, char *);
static void CloseMainCB(Widget, XtPointer, XtPointer);

#endif
