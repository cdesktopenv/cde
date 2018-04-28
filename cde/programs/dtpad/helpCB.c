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
/* $XConsortium: helpCB.c /main/4 1996/04/16 16:42:13 mgreess $ */
/**********************************<+>*************************************
***************************************************************************
**
**  File:        helpCB.c
**
**  Project:     DT dtpad, a memo maker type editor based on the Dt Editor
**               widget.
**
**  Description:
**  -----------
**
**  This file contains the all the Text Editor help related callbacks:
**
**    o 'XmNhelpCallback' callbacks for:
**	- the menu bar and the individual menus within the bar
**	- file selection boxes, dialogs and dialog components relating to
**	  the [File] menu
**	- the DtEditor widget
**    o 'XmNActivateCallback' callbacks for each item in the [Help]
**	 pulldown menu
**    o 'DtNhyperLinkCallback' and 'DtNcloseCallback' callbacks passed to
**	DtCreateHelpDialog() (via helpDlg.c:CreateHelpDialog()).
**
**  All 'XmNhelpCallback' and 'XmNActivateCallback' callbacks, except for
**  [Help] menu [On Item...], simply set the help location id to correspond
**  to help text on the widget for which the callback was set.  The On Item
**  'XmNActivateCallback' callback determines the widget on which it was
**  invoked and then calls the 'XmNhelpCallback' callback directly.  If no
**  'XmNhelpCallback' is installed, it searches up the widget's parentage
*   until it finds one and then calls it.
**
**  NOTE: All dialogs relating to the [Edit] and [Format] menus are
**	  controlled directly by the DtEditor widget.  Consequently,
**	  all of the help location ids for the "Find/Change", "Check
**	  Spelling" and "Format Settings" dialogs are set in the
**	  in the DtEditor widget's 'XmNhelpCallback' callback
**	  (i.e. in, HelpEditorWidgetCB).
**
*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1991, 1992, 1993.  All rights are
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
#include "dtpad.h"
#include "help.h"
#include <Dt/HelpDialog.h>


/************************************************************************
 * The following callbacks are set as the 'XmNhelpCallback' for the menu
 * menu bar and for the individual pulldown menus within the menu bar.
 ************************************************************************/

void
HelpMenubarCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, MENUBAR_HELP,
			     (char *) NULL);
}

void
HelpFileCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, FILE_MENU_HELP,
			     (char *) NULL);
}

void
HelpEditCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, EDIT_MENU_HELP,
			     (char *) NULL);
}

void
HelpFormatCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, FORMAT_MENU_HELP,
			     (char *) NULL);
}

void
HelpOptionsCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data)
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, OPTIONS_MENU_HELP,
			     (char *) NULL);
}

void
HelpHelpCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, HELP_MENU_HELP,
			     (char *) NULL);
}



/************************************************************************
 * The following callbacks are set as the 'XmNhelpCallback' on file
 * selection boxes and prompt dialogs relating to the [File] menu.
 ************************************************************************/

void
HelpOpenDialogCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, FILE_OPEN_DIALOG_HELP,
			     (char *) NULL);
}

void
HelpIncludeDialogCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, FILE_INCLUDE_DIALOG_HELP,
			     (char *) NULL);
}

void
HelpSaveAsDialogCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *) client_data;

    if (pPad->xrdb.nameChange) {
	SetHelpVolAndDisplayHelp(pPad, FILE_SAVEAS_DIALOG_HELP,
				 (char *) NULL);
    } else {
	SetHelpVolAndDisplayHelp(pPad, FILE_COPYTOFILE_DIALOG_HELP,
				 (char *) NULL);
    }
}

void
HelpAskIfSaveDialogCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, FILE_ASKIFSAVE_DIALOG_HELP,
			     (char *) NULL);
}

void
HelpFileAlreadyExistsCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, FILE_EXISTS_DIALOG_HELP,
			     (char *) NULL);
}


/************************************************************************
 * The following callback is set as the 'XmNhelpCallback' on the DtEditor
 * widget.  It controls help for the "Find/Change", "Check Spelling" and
 * "Format Settings" dialogs and dialog components.
 ************************************************************************/

void
HelpEditorWidgetCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{

    DtEditorHelpCallbackStruct *editorHelp = 
			(DtEditorHelpCallbackStruct *) call_data;

    switch (editorHelp->reason) {

	/* -----> edit area */
	case DtEDITOR_HELP_EDIT_WINDOW:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     EDIT_AREA_HELP,
				     (char *) NULL);
	    break;

	/* -----> status line area and fields */
	case DtEDITOR_HELP_STATUS_LINE:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     STATUS_LINE_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_STATUS_CURRENT_LINE:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     STATUS_CURRENT_LINE_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_STATUS_TOTAL_LINES:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     STATUS_TOTAL_LINES_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_STATUS_MESSAGE:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     STATUS_MESSAGE_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_STATUS_OVERSTRIKE:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     STATUS_OVERSTRIKE_HELP,
				     (char *) NULL);
	    break;

	/* -----> Format Settings dialog and dialog fields */
	case DtEDITOR_HELP_FORMAT_DIALOG:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     FORMAT_SETTINGS_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_FORMAT_LEFT_MARGIN:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     FORMAT_LEFT_MARGIN_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_FORMAT_RIGHT_MARGIN:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     FORMAT_RIGHT_MARGIN_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_FORMAT_ALIGNMENT:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     FORMAT_ALIGNMENT_HELP,
				     (char *) NULL);
	    break;

	/* -----> Find/Change dialog and dialog fields */
	case DtEDITOR_HELP_CHANGE_DIALOG:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     FINDCHANGE_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_CHANGE_FIND:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     FINDCHANGE_FIND_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_CHANGE_CHANGE:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     FINDCHANGE_CHANGETO_HELP,
				     (char *) NULL);
	    break;

	/* -----> Check Spelling dialog and dialog fields */
	case DtEDITOR_HELP_SPELL_DIALOG:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     SPELL_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_SPELL_MISSPELLED_WORDS:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     SPELL_MISSPELLED_WORDS_HELP,
				     (char *) NULL);
	    break;
	case DtEDITOR_HELP_SPELL_CHANGE:
	    SetHelpVolAndDisplayHelp((Editor *)client_data,
				     SPELL_CHANGETO_HELP,
				     (char *) NULL);
	    break;

 	default:
	    ;

    } /* switch (editorHelp->reason) */

}


/************************************************************************
 * The following callbacks are set as the 'XmNhelpCallback' on the print
 * setup dialogs accessible from the [File] [Print...] menu button.
 ************************************************************************/

void
HelpPrintSetupDialogCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, PRINT_SETUP_DIALOG_HELP,
			     (char *) NULL);
}

void
HelpPrintSetupAppSpecificCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, PRINT_APP_SPECIFIC_OPTIONS,
			     (char *) NULL);
}

void
HelpPrintSetupGenericCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, PRINT_GENERIC_OPTIONS,
			     (char *) NULL);
}

void
HelpPrintSetupPageHeadersFootersCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, PRINT_PAGE_HEADERS_FOOTERS,
			     (char *) NULL);
}

void
HelpPrintSetupPageMargins(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, PRINT_PAGE_MARGINS,
			     (char *) NULL);
}



/************************************************************************
 * The following callbacks are set (in dtpad.c:CreateHelpMenu) as the
 * 'XmNactivateCallback' for each of the items in the Help menu:
 *
 *	Overview...
 *	----------
 *	Table of Contents...
 *	Tasks...
 *	Reference...
 *	On Item
 *	----------
 *	Using Help...
 *	----------
 *	About Text Editor...
 *
 ************************************************************************/

void
HelpOverviewCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, HELP_MENU_OVERVIEW_HELP,
			     (char *) NULL);
}

void
HelpTasksCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, HELP_MENU_TASKS_HELP,
			     (char *) NULL);
}

void
HelpTOCCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, HELP_MENU_TOC_HELP,
			     (char *) NULL);
}

void
HelpReferenceCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, HELP_MENU_REFERENCE_HELP,
			     (char *) NULL);
}

void
HelpOnItemCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    Widget widget;

   switch(DtHelpReturnSelectedWidgetId(pPad->app_shell, (Cursor)NULL, &widget))
   {
	/*
	 * There are additional cases (e.g. user aborts request) but I
	 * don't feel they warrant an error dialog.
	 */
	case DtHELP_SELECT_VALID:
	    while (!XtIsShell(widget)) {
		if (XtHasCallbacks(widget, XmNhelpCallback) == XtCallbackHasSome) {
		    XtCallCallbacks(widget, XmNhelpCallback, (XtPointer)pPad);
			return;
		}
		widget = XtParent(widget);
	    }
	    break;
	case DtHELP_SELECT_INVALID:
	    Warning(pPad, 
	      GETMESSAGE(12, 1, "You must select an item within the Text Editor."),
	    XmDIALOG_INFORMATION);
	    break;
    }
}

void
HelpUsingHelpCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *)client_data, HELP_MENU_USING_HELP_HELP,
			     HELP_ON_HELP_VOLUME);
}

void
HelpAboutCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    SetHelpVolAndDisplayHelp((Editor *) client_data, HELP_MENU_ABOUT_HELP,
			     (char *) NULL);
}



/************************************************************************
 * HelpHyperlinkCB - is the 'DtNhyperLinkCallback' passed to
 *	DtCreateHelpDialog (in CreateHelpDialog).
 ************************************************************************/
void
HelpHyperlinkCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    DtHelpDialogCallbackStruct *pHyper = 
	    (DtHelpDialogCallbackStruct *) call_data;

    switch(pHyper->hyperType)
    {
	case DtHELP_LINK_JUMP_NEW:
	    DisplayNewHelpWindow((Editor *)client_data, pHyper->helpVolume,
			pHyper->locationId);
	    break;
	default:
            ;
	    /* application defined link code goes here */
    }
}



/************************************************************************
 * HelpCloseCB - is the 'DtNcloseCallback' passed to DtCreateHelpDialog
 *	(in CreateHelpDialog) to close/unmap all help dialogs, both cached
 *	and uncached.  It checks to see if the referenced dialog is in the
 *	uncached help for the current pad.
 ************************************************************************/
void
HelpCloseCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Editor *pPad = (Editor *)client_data;
    HelpStruct *pHelp;
    
    if(w != pPad->MainHelp)
    {
        /*
         * empty for loop just marches pHelp to the right place
         */
        for(pHelp = pPad->pHelpCache; 
	    pHelp != (HelpStruct *)NULL && pHelp->dialog != w;
	    pHelp = pHelp->pNext)
	    ;

        if(pHelp != (HelpStruct *)NULL)
        {
	    /* this should always happen */
            pHelp->inUse = False;
        }
    }

    XtUnmanageChild(w);
}
