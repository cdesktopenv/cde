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
/* $XConsortium: help.c /main/4 1995/11/02 14:05:18 rswiston $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
/******************************************************************************
 **  Program:           dticon
 **
 **  Description:       X11-based multi-color icon editor
 **
 **  File:              help.c, which contains the following subroutines or
 **                     functions:
 **                       AssignHelpCallbacks()
 **                       HelpTriggerCB()
 **                       HelpHyperlinkCB()
 **                       HelpCloseCB()
 **                       GetHelpDialog()
 **                       HelpSetVolAndLocation()
 **                       DisplayHelp()
 **                       DisplayNewHelp()
 **                       DisplayHelpDialog()
 **                       CreateHelpDialog()
 **
 ******************************************************************************
 **
 **  Copyright 1991 by Hewlett-Packard Company, 1990, 1991, 1992.
 **  All rights are reserved.  Copying or reproduction of this program,
 **  except for archival purposes, is prohibited without prior written
 **  consent of Hewlett-Packard Company.
 **
 **  Hewlett-Packard makes no representations about the suitibility of this
 **  software for any purpose.  It is provided "as is" without express or
 **  implied warranty.
 **
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/MwmUtil.h>
#include <Dt/HelpDialog.h>
#include "help.h"
#include "main.h"

extern Widget circleButton, editMenu_addHS_pb, editMenu_clear_pb;
extern Widget editMenu_copy_pb, editMenu_cut_pb, editMenu_deleteHS_pb;
extern Widget editMenu_flip_pb, editMenu_paste_pb, editMenu_resize_pb;
extern Widget editMenu_rotate_pb, editMenu_scale_pb, editMenu_top_pb;
extern Widget editMenu_undo_pb, ellipseButton, eraseButton, floodButton;
extern Widget fileMenu_new_pb, fileMenu_open_pb, fileMenu_quit_pb;
extern Widget fileMenu_saveAs_pb, fileMenu_save_pb, fileMenu_top_pb;
extern Widget lineButton, menu1, optionsMenu_format, optionsMenu_grid;
extern Widget optionsMenu_magnify, optionsMenu_top_pb, pointButton;
extern Widget polygonButton, polylineButton, queryDialog, rectangleButton;
extern Widget selectButton, fillToggle;
extern Widget editMenu_grabImage_pb;
extern Widget helpMenu_top_pb, helpMenu_intro_pb, helpMenu_tasks_pb,
              helpMenu_ref_pb, helpMenu_item_pb, helpMenu_using_pb,
              helpMenu_version_pb;
extern Widget rotateMenu, flipMenu, formatMenu, magnificationMenu;
extern Widget staticColorsBox, symbolicColorsBox, staticGreysBox;
extern Widget viewport;

void HelpTriggerCB(Widget w, caddr_t client_data, caddr_t call_data);
void HelpOnItemCB(Widget w, caddr_t client_data, caddr_t call_data);
void HelpSetVolAndLocation(int topic);
void DisplayNewHelp(char *helpVolume, char *locationId);
void DisplayHelp(char *helpVolume, char *locationId);
void DisplayHelpDialog(Widget dialog, char *helpVolume, char *locationId);
Widget CreateHelpDialog(HelpStruct *pHelp);


void
AssignHelpCallbacks( void )
{
#ifdef DEBUG
  if (debug)
    stat_out("Entering AssignHelpCallbacks\n");
#endif

/*** Set Help callbacks for the graphic tools ***/
  XtAddCallback(pointButton, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_POINT_BUTTON);
  XtAddCallback(floodButton, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_FLOOD_BUTTON);
  XtAddCallback(lineButton, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_LINE_BUTTON);
  XtAddCallback(polylineButton, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_POLYLINE_BUTTON);
  XtAddCallback(rectangleButton, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_RECTANGLE_BUTTON);
  XtAddCallback(polygonButton, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_POLYGON_BUTTON);
  XtAddCallback(circleButton, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_CIRCLE_BUTTON);
  XtAddCallback(ellipseButton, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_ELLIPSE_BUTTON);
  XtAddCallback(eraseButton, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_ERASER_BUTTON);
  XtAddCallback(selectButton, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_SELECT_BUTTON);
  XtAddCallback(fillToggle, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_FILL_BUTTON);

/*** Set Help callbacks for the pen groups ***/
  XtAddCallback(staticColorsBox, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_STATIC_COLOR_BUTTON);
  XtAddCallback(staticGreysBox, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_STATIC_GREY_BUTTON);
  XtAddCallback(symbolicColorsBox, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_DYNAMIC_COLOR_BUTTON);

/*** Set Help callbacks for the File menu ***/
  XtAddCallback(fileMenu_top_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_FILE_BUTTON);
  XtAddCallback(fileMenu_new_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_NEW_BUTTON);
  XtAddCallback(fileMenu_open_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_OPEN_BUTTON);
  XtAddCallback(fileMenu_save_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_SAVE_BUTTON);
  XtAddCallback(fileMenu_saveAs_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_SAVE_AS_BUTTON);
  XtAddCallback(fileMenu_quit_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_QUIT_BUTTON);

/*** Set Help callbacks for the Edit menu ***/
  XtAddCallback(editMenu_top_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_EDIT_BUTTON);
  XtAddCallback(editMenu_undo_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_UNDO_BUTTON);
  XtAddCallback(editMenu_cut_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_CUT_BUTTON);
  XtAddCallback(editMenu_copy_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_COPY_BUTTON);
  XtAddCallback(editMenu_paste_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_PASTE_BUTTON);
  XtAddCallback(editMenu_rotate_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_ROTATE_BUTTON);
  XtAddCallback(rotateMenu, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_ROTATE_BUTTON);
  XtAddCallback(editMenu_flip_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_FLIP_BUTTON);
  XtAddCallback(flipMenu, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_FLIP_BUTTON);
  XtAddCallback(editMenu_scale_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_SCALE_BUTTON);
  XtAddCallback(editMenu_resize_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_RESIZE_ICON_BUTTON);
  XtAddCallback(editMenu_addHS_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_ADD_HOTSPOT_BUTTON);
  XtAddCallback(editMenu_deleteHS_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_DELETE_HOTSPOT_BUTTON);
  XtAddCallback(editMenu_grabImage_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_GRAB_BUTTON);
  XtAddCallback(editMenu_clear_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_CLEAR_ICON_BUTTON);

/*** Set Help callbacks for the Options menu ***/
  XtAddCallback(optionsMenu_top_pb, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_OPTIONS_BUTTON);
  XtAddCallback(optionsMenu_grid, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_VISIBLE_GRID_BUTTON);
  XtAddCallback(optionsMenu_format, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_OUTPUT_FORMAT_BUTTON);
  XtAddCallback(formatMenu, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_OUTPUT_FORMAT_BUTTON);
  XtAddCallback(optionsMenu_magnify, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_MAGNIFICATION_BUTTON);
  XtAddCallback(magnificationMenu, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_MAGNIFICATION_BUTTON);

/*** Set Help callbacks for the Help menu (menu items activated) ***/
  XtAddCallback(helpMenu_intro_pb,
                        XmNactivateCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_INTRODUCTION);
  XtAddCallback(helpMenu_tasks_pb,
                        XmNactivateCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_TASKS);
  XtAddCallback(helpMenu_ref_pb,
                        XmNactivateCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_REFERENCE);
  XtAddCallback(helpMenu_item_pb,
                        XmNactivateCallback,
                        (XtCallbackProc) HelpOnItemCB,
                        (XtPointer)HELP_ON_ITEM);
  XtAddCallback(helpMenu_using_pb,
                        XmNactivateCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_USING_HELP);
  XtAddCallback(helpMenu_version_pb,
                        XmNactivateCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_VERSION);

/*** Set Help callbacks for the Help menu (help on menu items themselves) ***/
  XtAddCallback(helpMenu_top_pb,
                        XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_HELP_BUTTON);
  XtAddCallback(helpMenu_intro_pb,
                        XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_HELP_INTRODUCTION);
  XtAddCallback(helpMenu_tasks_pb,
                        XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_HELP_TASKS);
  XtAddCallback(helpMenu_ref_pb,
                        XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_HELP_REFERENCE);
  XtAddCallback(helpMenu_item_pb,
                        XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_HELP_ON_ITEM);
  XtAddCallback(helpMenu_using_pb,
                        XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_HELP_USING_HELP);
  XtAddCallback(helpMenu_version_pb,
                        XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_HELP_VERSION);

/*** Set Help callbacks for other GUI components ***/
  XtAddCallback(menu1, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_MENUBAR);
  XtAddCallback(queryDialog, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_QUERY);
  XtAddCallback(stdErrDialog, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_ERROR);
  XtAddCallback(newIconDialog, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_NEW_ICON);
  XtAddCallback(fileIODialog, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_FILEIO_ICON);
  XtAddCallback(viewport, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_VIEWPORT);
  XtAddCallback(mainWindow, XmNhelpCallback,
                        (XtCallbackProc) HelpTriggerCB,
                        (XtPointer)HELP_MAIN_WINDOW);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving AssignHelpCallbacks\n");
#endif
}


void
HelpTriggerCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
#ifdef DEBUG
  if (debug)
    stat_out("Entering HelpTriggerCB: help-tag=%d\n", (int) client_data);
#endif

  HelpSetVolAndLocation((int) (intptr_t) client_data);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving HelpTriggerCB\n");
#endif
}


void
HelpOnItemCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data )
{
    Widget selWidget;
    int    status;

#ifdef DEBUG
  if (debug)
    stat_out("Entering HelpOnItemCB: help-tag=%d\n", (int) client_data);
#endif


    while (!XtIsSubclass(w, applicationShellWidgetClass))
        w = XtParent(w);

    status = DtHelpReturnSelectedWidgetId(w, 0, &selWidget);
    /* NULL value for cursor, uses default cursor value. */

    switch ((int)status)
    {
        case DtHELP_SELECT_ERROR:
            /* Display error dialog here, function call failed */
            break;
        case DtHELP_SELECT_VALID:
            while (selWidget != (Widget)NULL)
            {
                if ( (XtHasCallbacks(selWidget, XmNhelpCallback)
                      == XtCallbackHasSome) )
                {
                    XtCallCallbacks(selWidget, XmNhelpCallback, NULL);
                    break;
                }
                else
                    selWidget = XtParent(selWidget);
            }
            break;
        case DtHELP_SELECT_ABORT:
            /* Display error dialog here,
             * "Selection Aborted by user."
             */
            break;
        case DtHELP_SELECT_INVALID:
            DoErrorDialog( GETSTR(14,4,"You must select an item\nwithin the Icon Editor.") );
            break;

    }

#ifdef DEBUG
  if (debug)
    stat_out("Leaving HelpOnItemCB\n");
#endif
}


void
HelpHyperlinkCB(
        Widget w,
        caddr_t client_data_unused,
        caddr_t call_data )
{
  DtHelpDialogCallbackStruct *pHyper =
                (DtHelpDialogCallbackStruct *) call_data;

  switch(pHyper->hyperType) {
    case DtHELP_LINK_JUMP_NEW :
                DisplayNewHelp(pHyper->helpVolume, pHyper->locationId);
                break;
    default : ; /* application defined link code goes here */
   } /* switch */
}


void
HelpCloseCB(
        Widget w,
        caddr_t client_data,
        caddr_t call_data_unused )
{
    HelpStruct *pHelp = (HelpStruct *)client_data;

#ifdef DEBUG
  if (debug)
    stat_out("Entering HelpCloseCB\n");
#endif

    if(pHelp != (HelpStruct *)NULL)
    {
      /* this is a "cached" help dialog */
      pHelp->inUse = False;
    }

    XtUnmanageChild(w);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving HelpCloseCB\n");
#endif
}


static Widget
GetHelpDialog( void )
{
  HelpStruct *pHelp;
  Arg args[5];
  int i, done;

#ifdef DEBUG
  if (debug)
    stat_out("Entering GetHelpDialog\n");
#endif

  if(help_head == (HelpStruct *)NULL) {
    pHelp = (HelpStruct *)XtMalloc(sizeof(HelpStruct));
    pHelp->pNext = (HelpStruct *)NULL;
    pHelp->pPrev = (HelpStruct *)NULL;
    pHelp->inUse = True;
    pHelp->dialog = CreateHelpDialog(pHelp);
    help_head = pHelp;

#ifdef DEBUG
  if (debug)
    stat_out("Leaving GetHelpDialog: Created the 1st help dialog\n");
#endif

    return pHelp->dialog;
   }
  else {
    pHelp = help_head;
    done = False;
    while (!done) {
      if(pHelp->inUse == False) {
        pHelp->inUse = True;

#ifdef DEBUG
  if (debug)
    stat_out("Leaving GetHelpDialog: activated an existing help dialog\n");
#endif

        return pHelp->dialog;
       } /* if */
      if(pHelp->pNext != (HelpStruct *)NULL)
        pHelp = pHelp->pNext;
      else
        done = True;
     } /* while */
    pHelp->pNext = (HelpStruct *) XtMalloc(sizeof(HelpStruct));
    pHelp->pNext->pPrev = pHelp;
    pHelp = pHelp->pNext;
    pHelp->pNext = (HelpStruct *)NULL;
    pHelp->inUse = True;
    pHelp->dialog = CreateHelpDialog(pHelp);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving GetHelpDialog: Created a new help dialog\n");
#endif

    return pHelp->dialog;
   }
}


void
HelpSetVolAndLocation(
        int topic)
{
    char *helpVolume, *locationId;

#ifdef DEBUG
  if (debug)
    stat_out("Entering HelpSetVolAndLocation: topic = %d\n", topic);
#endif

    helpVolume = HELP_VOLUME;
    switch (topic) {

      /*** choices for the 'Help' pulldown menu ***/
      case HELP_INTRODUCTION:
            locationId = HELP_INTRODUCTION_STR;
            break;
      case HELP_TASKS:
            locationId = HELP_TASKS_STR;
            break;
      case HELP_REFERENCE:
            locationId = HELP_REFERENCE_STR;
            break;
      case HELP_ON_ITEM:
            locationId = HELP_ON_ITEM_STR;
            break;
      case HELP_USING_HELP:
            helpVolume = HELP_HELP_VOLUME;
            locationId = HELP_USING_HELP_STR;
            break;
      case HELP_VERSION:
            locationId = HELP_VERSION_STR;
            break;

      /*** choices for the top-level menus on the main menubar ***/
      case HELP_FILE_BUTTON:
            locationId = HELP_FILE_BUTTON_STR;
            break;
      case HELP_EDIT_BUTTON:
            locationId = HELP_EDIT_BUTTON_STR;
            break;
      case HELP_OPTIONS_BUTTON:
            locationId = HELP_OPTIONS_BUTTON_STR;
            break;
      case HELP_HELP_BUTTON:
            locationId = HELP_HELP_BUTTON_STR;
            break;

      /*** choices for the 'File' pulldown menu ***/
      case HELP_NEW_BUTTON:
            locationId = HELP_NEW_BUTTON_STR;
            break;
      case HELP_OPEN_BUTTON:
            locationId = HELP_OPEN_BUTTON_STR;
            break;
      case HELP_SAVE_BUTTON:
            locationId = HELP_SAVE_BUTTON_STR;
            break;
      case HELP_SAVE_AS_BUTTON:
            locationId = HELP_SAVE_AS_BUTTON_STR;
            break;
      case HELP_QUIT_BUTTON:
            locationId = HELP_QUIT_BUTTON_STR;
            break;

      /*** choices for the 'Edit' pulldown menu ***/
      case HELP_UNDO_BUTTON:
            locationId = HELP_UNDO_BUTTON_STR;
            break;
      case HELP_CUT_BUTTON:
            locationId = HELP_CUT_BUTTON_STR;
            break;
      case HELP_COPY_BUTTON:
            locationId = HELP_COPY_BUTTON_STR;
            break;
      case HELP_PASTE_BUTTON:
            locationId = HELP_PASTE_BUTTON_STR;
            break;
      case HELP_ROTATE_BUTTON:
            locationId = HELP_ROTATE_BUTTON_STR;
            break;
      case HELP_FLIP_BUTTON:
            locationId = HELP_FLIP_BUTTON_STR;
            break;
      case HELP_SCALE_BUTTON:
            locationId = HELP_SCALE_BUTTON_STR;
            break;
      case HELP_RESIZE_ICON_BUTTON:
            locationId = HELP_RESIZE_ICON_BUTTON_STR;
            break;
      case HELP_ADD_HOTSPOT_BUTTON:
            locationId = HELP_ADD_HOTSPOT_BUTTON_STR;
            break;
      case HELP_DELETE_HOTSPOT_BUTTON:
            locationId = HELP_DELETE_HOTSPOT_BUTTON_STR;
            break;
      case HELP_GRAB_BUTTON:
            locationId = HELP_GRAB_BUTTON_STR;
            break;
      case HELP_CLEAR_ICON_BUTTON:
            locationId = HELP_CLEAR_ICON_BUTTON_STR;
            break;

      /*** choices for the 'Options' pulldown menu ***/
      case HELP_VISIBLE_GRID_BUTTON:
            locationId = HELP_VISIBLE_GRID_BUTTON_STR;
            break;
      case HELP_OUTPUT_FORMAT_BUTTON:
            locationId = HELP_OUTPUT_FORMAT_BUTTON_STR;
            break;
      case HELP_MAGNIFICATION_BUTTON:
            locationId = HELP_MAGNIFICATION_BUTTON_STR;
            break;

      /*** choices for the 'Help' pulldown menu ***/
      case HELP_HELP_INTRODUCTION:
            locationId = HELP_HELP_INTRODUCTION_STR;
            break;

      case HELP_HELP_TASKS:
            locationId = HELP_HELP_TASKS_STR;
            break;

      case HELP_HELP_REFERENCE:
            locationId = HELP_HELP_REFERENCE_STR;
            break;

      case HELP_HELP_ON_ITEM:
            locationId = HELP_HELP_ON_ITEM_STR;
            break;

      case HELP_HELP_USING_HELP:
            locationId = HELP_HELP_USING_HELP_STR;
            break;

      case HELP_HELP_VERSION:
            locationId = HELP_HELP_VERSION_STR;
            break;

      /*** choices for each of the Graphics Tool pushbuttons ***/
      case HELP_POINT_BUTTON:
            locationId = HELP_POINT_BUTTON_STR;
            break;
      case HELP_FLOOD_BUTTON:
            locationId = HELP_FLOOD_BUTTON_STR;
            break;
      case HELP_LINE_BUTTON:
            locationId = HELP_LINE_BUTTON_STR;
            break;
      case HELP_POLYLINE_BUTTON:
            locationId = HELP_POLYLINE_BUTTON_STR;
            break;
      case HELP_RECTANGLE_BUTTON:
            locationId = HELP_RECTANGLE_BUTTON_STR;
            break;
      case HELP_POLYGON_BUTTON:
            locationId = HELP_POLYGON_BUTTON_STR;
            break;
      case HELP_CIRCLE_BUTTON:
            locationId = HELP_CIRCLE_BUTTON_STR;
            break;
      case HELP_ELLIPSE_BUTTON:
            locationId = HELP_ELLIPSE_BUTTON_STR;
            break;
      case HELP_ERASER_BUTTON:
            locationId = HELP_ERASER_BUTTON_STR;
            break;
      case HELP_SELECT_BUTTON:
            locationId = HELP_SELECT_BUTTON_STR;
            break;
      case HELP_FILL_BUTTON:
            locationId = HELP_FILL_BUTTON_STR;
            break;

      /*** choices for each of the Pen Color Group pushbuttons ***/
      case HELP_STATIC_COLOR_BUTTON:
            locationId = HELP_STATIC_COLOR_BUTTON_STR;
            break;
      case HELP_STATIC_GREY_BUTTON:
            locationId = HELP_STATIC_GREY_BUTTON_STR;
            break;
      case HELP_DYNAMIC_COLOR_BUTTON:
            locationId = HELP_DYNAMIC_COLOR_BUTTON_STR;
            break;

      /*** choice for other GUI components ***/
      case HELP_MENUBAR:
            locationId = HELP_MENUBAR_STR;
            break;
      case HELP_QUERY:
            locationId = HELP_QUERY_STR;
            break;
      case HELP_ERROR:
            locationId = HELP_ERROR_STR;
            break;
      case HELP_NEW_ICON:
            locationId = HELP_NEW_ICON_STR;
            break;
      case HELP_FILEIO_ICON:
            locationId = HELP_FILEIO_ICON_STR;
            break;
      case HELP_VIEWPORT:
            locationId = HELP_VIEWPORT_STR;
            break;
      case HELP_MAIN_WINDOW:
            locationId = HELP_MAIN_WINDOW_STR;
            break;
      default:
            locationId = HELP_MAIN_WINDOW_STR;
            break;
    }
    DisplayHelp(helpVolume, locationId);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving HelpSetVolAndLocation\n");
#endif
}


void
DisplayHelp(
        char *helpVolume,
        char *locationId)
{
  static Widget mainHelpDialog = NULL;

#ifdef DEBUG
  if (debug)
    stat_out("Entering DisplayHelp\n");
#endif

  if (mainHelpDialog == NULL)
      mainHelpDialog = CreateHelpDialog(NULL);

  DisplayHelpDialog (mainHelpDialog, helpVolume, locationId);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving DisplayHelp\n");
#endif
}


void
DisplayNewHelp(
        char *helpVolume,
        char *locationId)
{
  Widget helpDialog;

#ifdef DEBUG
  if (debug)
    stat_out("Entering DisplayNewHelp\n");
#endif

  helpDialog = GetHelpDialog();
  DisplayHelpDialog (helpDialog, helpVolume, locationId);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving DisplayNewHelp\n");
#endif
}

void
DisplayHelpDialog (
        Widget helpDialog,
        char  *helpVolume,
        char  *locationId)
{
  Arg args[10];
  int n;

#ifdef DEBUG
  if (debug)
    stat_out("Entering DisplayHelpDialog\n");
#endif

  n = 0;
  XtSetArg(args[n], DtNhelpVolume, helpVolume); n++;
  XtSetArg(args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;
  XtSetArg(args[n], DtNlocationId, locationId); n++;
  XtSetValues(helpDialog, args, n);

  XtManageChild(helpDialog);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving DisplayHelpDialog\n");
#endif
}


Widget
CreateHelpDialog (HelpStruct *pHelp)
{
    Widget helpDialog;
    Arg args[10];
    int i;

#ifdef DEBUG
  if (debug)
    stat_out("Entering CreateHelpDialog, pHelp is %x\n", pHelp);
#endif

    i = 0;
    XtSetArg(args[i], XmNtitle, GETSTR(14,2, "Icon Editor - Help")); i++;
    XtSetArg(args[i], XmNiconName,GETSTR(14,2, "Icon Editor - Help"));i++;
    helpDialog = DtCreateHelpDialog(dtIconShell, "helpDlg",
                                            args, i);
    XtAddCallback(helpDialog, DtNhyperLinkCallback,
                      (XtCallbackProc)HelpHyperlinkCB, NULL);
    XtAddCallback(helpDialog, DtNcloseCallback,
                      (XtCallbackProc)HelpCloseCB, pHelp);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving CreateHelpDialog\n");
#endif

    return helpDialog;
}

