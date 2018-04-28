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
/* $XConsortium: help.h /main/4 1996/04/16 16:42:05 mgreess $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        help.h
**
**  Project:     DT dtpad, a memo maker type editor based on the Dt Editor
**               widget.
**
**  Description:
**  -----------
**  This is the header file for the help related portions of dtpad.
**  It provides the help volume name and location ids for the various
**  help volume entry points accessed via the help callbacks in helpCB.c.
**
**  For example, in the line
**
**	#define FILE_MENU_HELP			"fileMenu"
**
**  FILE_MENU_HELP is used in helpCB.c to display help for the [File] pulldown
**  menu which is found in the dtpad help volume immediately following the
**  "tag":
**
**	<location id=fileMenu>&empty;<\location>
**
**
*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1992.  All rights are
**  reserved.  Copying or other reproduction of this program
**  except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
**
********************************************************************
**  (c) Copyright 1993, 1994 Hewlett-Packard Company
**  (c) Copyright 1993, 1994 International Business Machines Corp.
**  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
**  (c) Copyright 1993, 1994 Novell, Inc.
********************************************************************
**
**
**************************************************************************
**********************************<+>*************************************/

/************************************************************************
 * Default Text Editor help volume name
 ************************************************************************/
#define TEXTEDITOR_HELP_VOLUME			"Textedit"
#define HELP_ON_HELP_VOLUME			"Help4Help"


/************************************************************************
 * Help location ids for the menubar
 ************************************************************************/
#define MENUBAR_HELP				"menubar"


/************************************************************************
 * Help location ids for pulldown menus within the menubar
 ************************************************************************/

#define FILE_MENU_HELP                          "TextEditorFileMenu"
#define EDIT_MENU_HELP                          "TextEditorEditMenu"
#define OPTIONS_MENU_HELP                       "TextEditorOptionsMenu"
#define HELP_MENU_HELP                          "TextEditorHelpMenu"
#define FORMAT_MENU_HELP			"formatMenu"


/************************************************************************
 * Help location ids for individual items within the [Help] pulldown menu
 ************************************************************************/
#define HELP_MENU_OVERVIEW_HELP			"_hometopic"
#define HELP_MENU_TASKS_HELP			"Tasks"
#define HELP_MENU_TOC_HELP			"TableOfContents"			
#define HELP_MENU_REFERENCE_HELP		"Reference"
/* 'XmNActivateCallback' for [On Item] does not set a specific location id */
/* the HELP_MENU_USING_HELP_HELP location id is for the HELP_ON_HELP_VOLUME */
#define HELP_MENU_USING_HELP_HELP		"_hometopic"
#define HELP_MENU_ABOUT_HELP			"_copyright"


/************************************************************************
 * Help location ids for dialogs posted directly or indirectly by [File]
 * menu items
 ************************************************************************/
#define FILE_OPEN_DIALOG_HELP			"openDialog"
#define FILE_INCLUDE_DIALOG_HELP		"includeDialog"
#define FILE_SAVEAS_DIALOG_HELP			"saveAsDialog"
#define FILE_COPYTOFILE_DIALOG_HELP		"copytofileDialog"
/* [Print...] causes a separate process to be invoked with its own help */

/* -----> dialogs conditionally posted as the result of a [File] menu action */
#define FILE_ASKIFSAVE_DIALOG_HELP		"askIfSaveDialog"
#define FILE_EXISTS_DIALOG_HELP			"fileExistsDialog"
#define FILE_CONFIRM_DIALOG_HELP		"fileConfirmDialog"


/************************************************************************
 * Help location ids for the dialog posted by [File] menu [Print...]
 * (this dialog is controlled by the DtPrintSetupBox widget)
 ************************************************************************/
#define PRINT_SETUP_DIALOG_HELP			"printSetupDialog"

/* -----> fields/buttons within the Print Setup dialog */
#define PRINT_APP_SPECIFIC_OPTIONS		"print-appSpecific"
#define PRINT_GENERIC_OPTIONS			"print-generic"
#define PRINT_PAGE_HEADERS_FOOTERS		"print-pageHeadersFooters"
#define PRINT_PAGE_MARGINS			"print-pageMargins"


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
