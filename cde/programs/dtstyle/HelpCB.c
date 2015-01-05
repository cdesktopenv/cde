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
/* $XConsortium: HelpCB.c /main/5 1996/03/25 00:51:28 pascale $ */
/*
 ****************************************************************************
 **
 **   File:        HelpCB.c
 **
 **   Project:     DT 3.0
 **
 **   Description: Contains the callbacks for Help
 **
 **
 ****************************************************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1990, 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include <Dt/HelpDialog.h>

#include "Main.h"
#include "Help.h"

void
HelpRequestCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    char *helpVolume, *locationId;
    intptr_t topic;
    
    topic = (intptr_t) client_data;
    helpVolume = HELP_VOLUME;

    switch (topic)
    {
	case HELP_MAIN_WINDOW:
	    locationId = HELP_MAIN_WINDOW_STR;
	    break;
	case HELP_MENUBAR:
	    locationId = HELP_MENUBAR_STR;
	    break;
	case HELP_FILE_BUTTON:
	    locationId = HELP_FILE_BUTTON_STR;
	    break;
	case HELP_HELP_BUTTON:
	    locationId = HELP_HELP_BUTTON_STR;
	    break;
	case HELP_FILE_MENU:
	    locationId = HELP_FILE_MENU_STR;
	    break;
	case HELP_HELP_MENU:
	    locationId = HELP_HELP_MENU_STR;
	    break;
	case HELP_EXIT:
	    locationId = HELP_EXIT_STR;
	    break;
	case HELP_INTRODUCTION:
	    locationId = HELP_INTRODUCTION_STR;
	    break;
	case HELP_TASKS:        
	    locationId = HELP_TASKS_STR;
	    break;
	case HELP_REFERENCE:
	    locationId = HELP_REFERENCE_STR;
	    break;
	case HELP_ITEM:
	    locationId = HELP_ITEM_STR;
	    break;
	case HELP_USING:
	    locationId = HELP_USING_STR;
            helpVolume = HELP_USING_HELP_VOLUME;
	    break;
	case HELP_VERSION:
	    locationId = HELP_VERSION_STR;
	    break;
	case HELP_COLOR_BUTTON:
	    locationId = HELP_COLOR_BUTTON_STR;
	    break;
	case HELP_FONT_BUTTON:
	    locationId = HELP_FONT_BUTTON_STR;
	    break;
	case HELP_BACKDROP_BUTTON:
	    locationId = HELP_BACKDROP_BUTTON_STR;
	    break;
	case HELP_KEYBOARD_BUTTON:
	    locationId = HELP_KEYBOARD_BUTTON_STR;
	    break;
	case HELP_MOUSE_BUTTON:
	    locationId = HELP_MOUSE_BUTTON_STR;
	    break;
	case HELP_AUDIO_BUTTON:
	    locationId = HELP_AUDIO_BUTTON_STR;
	    break;
	case HELP_SCREEN_BUTTON:
	    locationId = HELP_SCREEN_BUTTON_STR;
	    break;
	case HELP_DTWM_BUTTON:
	    locationId = HELP_DTWM_BUTTON_STR;
	    break;
	case HELP_STARTUP_BUTTON:
	    locationId = HELP_STARTUP_BUTTON_STR;
	    break;
	case HELP_I18N_BUTTON:
	    locationId = HELP_I18N_BUTTON_STR;
	    break;
	case HELP_COLOR_DIALOG:
	    locationId = HELP_COLOR_DIALOG_STR;
	    break;
	case HELP_ADD_PALETTE_DIALOG:
	    locationId = HELP_ADD_PALETTE_DIALOG_STR;
	    break;
	case HELP_ADD_PALETTE_WARNING_DIALOG:
	    locationId = HELP_ADD_PALETTE_WARNING_DIALOG_STR;
	    break;
	case HELP_DELETE_PALETTE_DIALOG:
	    locationId = HELP_DELETE_PALETTE_DIALOG_STR;
	    break;
	case HELP_DELETE_PALETTE_WARNING_DIALOG:
	    locationId = HELP_DELETE_PALETTE_WARNING_DIALOG_STR;
	    break;
	case HELP_MODIFY_PALETTE_DIALOG:
	    locationId = HELP_MODIFY_PALETTE_DIALOG_STR;
	    break;
	case HELP_COLOR_USE_DIALOG:
	    locationId = HELP_COLOR_USE_DIALOG_STR;
	    break;
	case HELP_FONT_DIALOG:
	    locationId = HELP_FONT_DIALOG_STR;
	    break;
	case HELP_BACKDROP_DIALOG:
	    locationId = HELP_BACKDROP_DIALOG_STR;
	    break;
	case HELP_KEYBOARD_DIALOG:
	    locationId = HELP_KEYBOARD_DIALOG_STR;
	    break;
	case HELP_MOUSE_DIALOG:
	    locationId = HELP_MOUSE_DIALOG_STR;
	    break;
	case HELP_AUDIO_DIALOG:
	    locationId = HELP_AUDIO_DIALOG_STR;
	    break;
	case HELP_SCREEN_DIALOG:
	    locationId = HELP_SCREEN_DIALOG_STR;
	    break;
	case HELP_DTWM_DIALOG:
	    locationId = HELP_DTWM_DIALOG_STR;
	    break;
	case HELP_STARTUP_DIALOG:
	    locationId = HELP_STARTUP_DIALOG_STR;
	    break;
	case HELP_I18N_DIALOG:
	    locationId = HELP_I18N_DIALOG_STR;
	    break;
    }

    Help(helpVolume, locationId);
}

void
QuickHelpRequestCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{

}

void
HelpModeCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
   Widget widget;

   switch(DtHelpReturnSelectedWidgetId(style.shell, (Cursor)NULL, &widget))
   {
      /*
       * There are additional cases (e.g. user aborts request) but I
       * don't feel they warrant an error dialog.
       */
      case DtHELP_SELECT_VALID:

          while (!XtIsShell(widget))
          {
             if (XtHasCallbacks(widget, XmNhelpCallback) == XtCallbackHasSome)
             {
                XtCallCallbacks(widget, XmNhelpCallback, (XtPointer)NULL);
                return;
             }
             widget = XtParent(widget);
          }
          break;

      case DtHELP_SELECT_INVALID:

          ErrDialog((char *)GETMESSAGE(2, 8, "You must select an item\nwithin the Style Manager."), style.shell);
          break;

   }
}

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
	    DisplayHelp(pHyper->helpVolume, pHyper->locationId);
	    break;
	default:
            ;
	    /* application defined link code goes here */
    }
}

void
HelpCloseCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{

    HelpStruct *pHelpCache = (HelpStruct *)client_data;
    HelpStruct *pHelp;

    /*
     * empty for loop just marches pHelp to the right place
     */
    for(pHelp = pHelpCache; 
	pHelp != (HelpStruct *)NULL && pHelp->dialog != w;
	pHelp = pHelp->pNext);

    if(pHelp != (HelpStruct *)NULL)
    {
	/* this should always happen */
        pHelp->inUse = False;
    }

    XtUnmapWidget(XtParent(w));

}
