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
/* $XConsortium: errordialog.c /main/6 1996/10/17 09:36:14 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           errordialog.c
 *
 *
 *   DESCRIPTION:    Routines to create dialogs for error messages
 *
 *   FUNCTIONS: create_error_dialog
 *		error_cancel_callback
 *		ok_callback
 *		toggle_callback
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/XmP.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/BulletinB.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>

#include "dtcopy.h"
#include "sharedFuncs.h"

#define VERTICAL_SPACING  5
#define HORIZONTAL_SPACING  8
#define TOP_SPACING  (VERTICAL_SPACING + 3)
#define INDENTED_SPACING (HORIZONTAL_SPACING + 5)

/*--------------------------------------------------------------------
 * callbacks
 *------------------------------------------------------------------*/

static void
ok_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
  XtDestroyWidget(G_error_dialog);
  G_wait_on_input = FALSE;
  G_error_status = 0;
  G_error_dialog = NULL;
  return;

}


static void
error_cancel_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
   display_cancel_warning(G_error_dialog);

   G_pause_copy = TRUE;
}


/*-------------------------------------------------
 *
 *  The toggle shows up in more than one dialog
 *  thus the state of the two widgets must be
 *  synchronized.
 *
 *------------------------------------------------*/
void
toggle_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{


  G_ignore_errors = !G_ignore_errors;

  if (w == G_toggle_main)
  {
     if (G_error_dialog != NULL)
        XtVaSetValues(G_toggle_error,
                      XmNset, !G_ignore_errors,
                      NULL);
  }
  else
  {
     XtVaSetValues(G_toggle_main,
                   XmNset, !G_ignore_errors,
                   NULL);
  }
  return;
}


/*--------------------------------------------------------------------
 * create error dialog
 *------------------------------------------------------------------*/

void
create_error_dialog(
	Widget parent_widget,
	char *op,
	char *filename,
	int errnum)
{
   char  buf[1024];
   char *msg;

   Arg args[10];
   int n;
   XmString xm_string;
   Widget form, outerFrame;
   Widget icon, error_header_label, error_text_label;
   Widget separator, action_area;
   Pixel background, foreground;
   Pixmap px;
   Position x, y;

   static ActionAreaItem copy_action_items[] = {
     {"Continue",    6, 6, ok_callback,           NULL},
     {"Cancel Copy", 6, 7, error_cancel_callback, NULL},
     {"Help",        6, 4, help_callback,         (XtPointer) HELP_DIR_ERROR},
   };

  ActionAreaDefn copyActions = {
     XtNumber(copy_action_items),
     0,                      /* Ok is default action */
     copy_action_items
  };

   static ActionAreaItem move_action_items[] = {
     {"Continue",    6, 6, ok_callback,           NULL},
     {"Cancel Move", 6, 8, error_cancel_callback, NULL},
     {"Help",        6, 4, help_callback,         (XtPointer) HELP_DIR_ERROR},
   };

  ActionAreaDefn moveActions = {
     XtNumber(move_action_items),
     0,                      /* Ok is default action */
     move_action_items
  };


   /* Create the dialog shell for the dialog */
   n=0;
   XtSetArg (args[n], XmNallowShellResize, True); n++;
   XtSetArg (args[n], XmNmappedWhenManaged, False); n++;
   if (G_move)
   {
      XtSetArg (args[n], XmNtitle, GETMESSAGE(4, 4, "Folder Move - Error")); n++;
   }
   else
   {
      XtSetArg (args[n], XmNtitle, GETMESSAGE(4, 3, "Folder Copy - Error")); n++;
   }

   G_error_dialog = XmCreateDialogShell (G_copy_dialog, "dialog_shell", args, n);

   /* create a frame around the dialog for better separation from window border */
   outerFrame = XtVaCreateManagedWidget("outerFrame",
                               xmFrameWidgetClass, G_error_dialog,
                               XmNshadowThickness, 3,
                               XmNshadowType, XmSHADOW_ETCHED_IN,
                               NULL);


   /* Create the Manager Widget, form, for the copy error dialog */
   form  = XtVaCreateManagedWidget ("dialog_form",
   			      xmFormWidgetClass,
                              outerFrame,
                              XmNverticalSpacing,   VERTICAL_SPACING,
                              XmNhorizontalSpacing, HORIZONTAL_SPACING,
                              NULL);

   /* create the action area of the Error Dialog */
   if (G_move)
      action_area = Create_Action_Area(form, moveActions, NULL);
   else
      action_area = Create_Action_Area(form, copyActions, NULL);

   /* create the separator widget */
   separator  =  XtVaCreateManagedWidget("separator",
                              xmSeparatorWidgetClass,
                              form,
                              XmNbottomAttachment,  XmATTACH_WIDGET,
                              XmNbottomWidget,      action_area,
                              XmNleftAttachment,    XmATTACH_FORM,
                              XmNrightAttachment,   XmATTACH_FORM,
                              NULL);



   /* Create the error dialog icon */
   n = 0;
   XtSetArg (args[n], XmNbackground, &background); n++;
   XtSetArg (args[n], XmNforeground, &foreground); n++;
   XtGetValues (form, args, n);
   px = XmGetPixmapByDepth(XtScreen(form), "xm_error",
                           foreground, background, form->core.depth);
   if (px == XmUNSPECIFIED_PIXMAP)
     px = XmGetPixmapByDepth(XtScreen(form), "default_xm_error",
                      foreground, background, form->core.depth);

   icon = XtVaCreateManagedWidget("error_icon",
                              xmLabelWidgetClass,
                              form,
                              XmNlabelType,        XmPIXMAP,
                              XmNlabelPixmap,      px,
                              XmNtopAttachment,    XmATTACH_FORM,
                              XmNbottomAttachment, XmATTACH_WIDGET,
                              XmNbottomWidget,     separator,
                              XmNleftAttachment,   XmATTACH_FORM,
                              NULL);

   /* Create the first error message label */
   sprintf(buf,GETMESSAGE(4, 1, "Error %s\n%s"), op, filename);
   xm_string = XmStringCreateLocalized(buf);
   error_header_label = XtVaCreateManagedWidget("msg_header",
                              xmLabelWidgetClass,
                              form,
                              XmNlabelString,     xm_string,
                              XmNtopAttachment,   XmATTACH_FORM,
                              XmNleftAttachment,  XmATTACH_WIDGET,
                              XmNleftWidget,      icon,
                              XmNrightAttachment, XmATTACH_FORM,
                              XmNalignment,       XmALIGNMENT_BEGINNING,
                              NULL);
   XmStringFree(xm_string);

   /* Create the second error message label */
   msg =  strerror(errnum);
   xm_string = XmStringCreateLocalized(msg);
   error_text_label = XtVaCreateManagedWidget("msg_text",
                              xmLabelWidgetClass,
                              form,
                              XmNlabelString,      xm_string,
                              XmNtopAttachment,    XmATTACH_WIDGET,
                              XmNtopWidget,        error_header_label,
                              XmNleftAttachment,   XmATTACH_WIDGET,
                              XmNleftWidget,       icon,
                              XmNrightAttachment,  XmATTACH_FORM,
                              XmNalignment,        XmALIGNMENT_BEGINNING,
                              NULL);
   XmStringFree(xm_string);

   /* create the toggle button */
   xm_string = XmStringCreateLocalized(GETMESSAGE(4, 2, "Pause for errors"));
   G_toggle_error = XtVaCreateManagedWidget("toggle",
   			      xmToggleButtonWidgetClass,
                              form,
                              XmNlabelString,      xm_string,
                              XmNset,              !G_ignore_errors,
                              XmNtopAttachment,    XmATTACH_WIDGET,
                              XmNtopWidget,        error_text_label,
                              XmNleftAttachment,   XmATTACH_WIDGET,
                              XmNleftWidget,       icon,
                              XmNbottomAttachment, XmATTACH_WIDGET,
                              XmNbottomWidget,     separator,
                              XmNalignment,        XmALIGNMENT_BEGINNING,
                              NULL);
   XtAddCallback(G_toggle_error, XmNvalueChangedCallback, toggle_callback, NULL);
   XmStringFree(xm_string);
   /* set initial keyborad focus to the action button area */
   XtSetArg (args[0], XmNinitialFocus, action_area);
   XtSetValues(form, args, 1);

   /* Manage the Dialog */
   XtManageChild(form);


   /* Choose a window position */
   _DtChildPosition(G_error_dialog, G_toplevel, &x, &y);
   n=0;
   XtSetArg (args[n], XmNx, x); n++;
   XtSetArg (args[n], XmNy, y); n++;
   XtSetValues(G_error_dialog, args, n);

   /* Now that we have a window postion, mapp the dialog shell */
   XtMapWidget(G_error_dialog);

   return;
}

