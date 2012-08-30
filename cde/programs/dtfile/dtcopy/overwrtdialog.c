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
/* $XConsortium: overwrtdialog.c /main/5 1996/10/17 09:36:29 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           overwrtdialog.c
 *
 *
 *   DESCRIPTION:    Routines to handle dialog for collision operations
 *
 *   FUNCTIONS: confirmtoggle_callback
 *		create_overwrite_dialog
 *		ok_callback
 *		overwrite_cancel_callback
 *		radio_callback
 *		renameError_ok_callback
 *		touch
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
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
#include <Xm/MessageB.h>
#include <Xm/Separator.h>

#include "dtcopy.h"
#include "sharedFuncs.h"

#define VERTICAL_SPACING  5
#define HORIZONTAL_SPACING  10
#define TOP_SPACING  (VERTICAL_SPACING + 3)



/*--------------------------------------------------------------------
 * Callback routines
 *------------------------------------------------------------------*/

static void
radio_callback(
	Widget w,
	XtPointer client_data,
        XtPointer call_data)
{
   XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *)call_data;


   G_overwrite_selection = (int) client_data;

   if (state->set)
      switch (G_overwrite_selection)
      {
        case G_OVERWRITE_EXISTING:
           XtVaSetValues (G_rename_text,
                          XmNsensitive, FALSE,
                          NULL);
        break;

        case G_SKIP:
           XtVaSetValues (G_rename_text,
                          XmNsensitive, FALSE,
                          NULL);
        break;

        case G_RENAME_EXISTING:
           XtVaSetValues (G_rename_text,
                          XmNsensitive, TRUE,
                          NULL);
           XmProcessTraversal(G_rename_text, XmTRAVERSE_CURRENT);
       break;


      }   /* end switch */

  return;

}


static void
renameError_ok_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
  XtDestroyWidget((Widget)client_data);
  XmProcessTraversal(G_rename_text, XmTRAVERSE_CURRENT);

  return;
}


static void
ok_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
   struct stat    buf;
   char           msg[128];
   int            saveError=0;
   String         title;
   char           tfolder[MAX_PATH], tobject[MAX_PATH];
   char          *newObject, *renameNewName;

   switch (G_overwrite_selection)
   {
      case G_OVERWRITE_EXISTING:
      case G_SKIP:
         break;

      case G_RENAME_EXISTING:
         /* get object (i.e. file) name from text field and build the complete path */
         /* this procedure restricts renaming to be within a folder (directory) */
         split_path(G_rename_oldname,tfolder,tobject);
         newObject = XmTextFieldGetString(G_rename_text);
         renameNewName = build_path(tfolder,newObject);
         XtFree(newObject);

         /* make sure renameNewName does not exist */
         if ( (lstat(renameNewName, &buf)) == 0)
         {
            title = GETMESSAGE(5, 11, "File Overwrite - Error");
            sprintf(msg,
                    GETMESSAGE(5, 10, "%s\nalready exists: choose another name."),
                    renameNewName);
            _DtMessageDialog (G_over_dialog, title, msg, 0, FALSE, NULL,
                              renameError_ok_callback, NULL, NULL, False, ERROR_DIALOG);
            free(renameNewName);
            return;

         }

         /* attempt the rename ... since rename function is being used, this */
         /* only allows renames within the same filesystem */
         errno = 0;
         if ( rename(G_rename_oldname, renameNewName) != 0)
         {
            saveError = errno;
            title = GETMESSAGE(5, 11, "File Overwrite - Error");
            sprintf(msg,
                    GETMESSAGE(5, 10, "%s\ncannot be renamed to\n%s\n%s"),
                    G_rename_oldname, renameNewName, strerror(saveError));
            _DtMessageDialog (G_over_dialog, title, msg, 0, FALSE, NULL,
                              renameError_ok_callback, NULL, NULL, False, ERROR_DIALOG);
            free(renameNewName);
            return;

         }
         /* the rename went ok ... create an empty file so SyncDirectory doesn't */
         /* generate an error trying to delete G_rename_oldname */
         else
         {
            free(renameNewName);
            touch(G_rename_oldname);
         }
         break;
   }

  XtDestroyWidget(G_over_dialog);
  G_over_dialog = NULL;
  G_wait_on_input = FALSE;

  return;
}


static void
confirmtoggle_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
  G_overwrite_confirm = !G_overwrite_confirm;
  return;
}


static void
overwrite_cancel_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
   display_cancel_warning(G_over_dialog);

   G_pause_copy = TRUE;
}


/*--------------------------------------------------------------------
 * create overwrite dialog
 *------------------------------------------------------------------*/

void
create_overwrite_dialog(
	Widget parent_widget,
	char *sname,
	char *tname,
	Boolean isdir)
{
  char  buf[2048];
  char  tfolder[MAX_PATH], tobject[MAX_PATH];                      /* t for target */
  char  gname[MAX_PATH], gfolder[MAX_PATH], gobject[MAX_PATH]; /* g for generated */

  Arg args[10];
  int n =0;
  int i;
  XmString msg;
  XmString xm_string[N_OVERWRITE_RADIO_BUTTONS];
  Widget radiobtn[N_OVERWRITE_RADIO_BUTTONS];
  Widget form, outerFrame;
  Widget action_label, icon, action_area, separator, msg_label;
  Pixel background, foreground;
  Pixmap px;
  Position x, y;

  static ActionAreaItem action_items[] = {
    {"Ok",          6, 1, ok_callback,               NULL},
    {"Cancel Copy", 6, 7, overwrite_cancel_callback, NULL},  /* possibly changed below to "Cancel Move" */
    {"Help",        6, 4, help_callback,             (XtPointer) HELP_OVERWRITE},
  };

  ActionAreaDefn overwriteActions = {
     XtNumber(action_items),
     0,                      /* Ok is default action */
     action_items
  };


  /* save the target name for possible rename in ok_callback */
  strcpy(G_rename_oldname,tname);


  if (G_move)
  {
     action_items[2].label   = "Cancel Move";
     action_items[2].msg_num = 8;
  }

  /* Create the dialog shell for the dialog */
  n = 0;
  XtSetArg (args[n], XmNallowShellResize,  True);   n++;
  XtSetArg (args[n], XmNmappedWhenManaged, FALSE); n++;
  if (G_move)
  {
     XtSetArg (args[n], XmNtitle, GETMESSAGE(5, 2, "Folder Move - File Overwrite"));
     n++;
  }
  else
  {
     XtSetArg (args[n], XmNtitle, GETMESSAGE(5, 9, "Folder Copy - File Overwrite"));
     n++;
  }
  G_over_dialog = XmCreateDialogShell (parent_widget, "dialog_shell", args, n);

   /* create a frame around the dialog for better separation from window border */
  outerFrame = XtVaCreateWidget("outerFrame",
                             xmFrameWidgetClass,
                             G_over_dialog,
                             XmNshadowThickness, 3,
                             XmNshadowType, XmSHADOW_ETCHED_IN,
                             NULL);

  /* Create the Manager Widget, form, for the copy dialog */
  form  = XtVaCreateWidget ("dialog_form",
  			     xmFormWidgetClass,
                             outerFrame,
                             XmNverticalSpacing,   VERTICAL_SPACING,
                             XmNhorizontalSpacing, HORIZONTAL_SPACING,
                             NULL);

  /* Create a question dialog icon */
  n = 0;
  XtSetArg (args[n], XmNbackground, &background); n++;
  XtSetArg (args[n], XmNforeground, &foreground); n++;
  XtGetValues (form, args, n);

  px = XmGetPixmapByDepth(XtScreen(form), "xm_question",
                          foreground, background, form->core.depth);
  if (px == XmUNSPECIFIED_PIXMAP)
    px = XmGetPixmapByDepth(XtScreen(form), "default_xm_question",
                     foreground, background, form->core.depth);

  icon = XtVaCreateManagedWidget("question_icon",
                             xmLabelWidgetClass,
                             form,
                             XmNlabelType,        XmPIXMAP,
                             XmNlabelPixmap,      px,
                             XmNtopAttachment,    XmATTACH_FORM,
                             XmNleftAttachment,   XmATTACH_FORM,
                             NULL);

  /* Create the message label */
  split_path(tname,tfolder,tobject);
  sprintf(buf,
          GETMESSAGE(5, 1, "A file named\n%s\nalready exists inside the folder\n%s"),
          tobject, tfolder);
  msg = XmStringCreateLocalized(buf);
  msg_label = XtVaCreateManagedWidget("msg_header",
                             xmLabelWidgetClass,
                             form,
                             XmNlabelString,      msg,
                             XmNalignment,        XmALIGNMENT_BEGINNING,
                             XmNtopAttachment,    XmATTACH_FORM,
                             XmNleftAttachment,   XmATTACH_WIDGET,
                             XmNleftWidget,       icon,
                             XmNleftOffset,       HORIZONTAL_SPACING,
                             XmNrightAttachment,  XmATTACH_FORM,
                             NULL);
  XmStringFree(msg);


  /* create the action-question */
  msg = XmStringCreateLocalized(GETMESSAGE(5, 8, "Action to take:"));
  action_label = XtVaCreateManagedWidget("action-question",
                             xmLabelWidgetClass,
                             form,
                             XmNlabelString,      msg,
                             XmNalignment,        XmALIGNMENT_BEGINNING,
                             XmNtopAttachment,    XmATTACH_WIDGET,
                             XmNtopWidget,        msg_label,
                             XmNtopOffset,        2*VERTICAL_SPACING,
                             XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET,
                             XmNleftWidget,       msg_label,
                             NULL);
  XmStringFree(msg);


  /* create radio box */

  n = 0;
  XtSetArg (args[n], XmNorientation,      XmVERTICAL);               n++;
  XtSetArg (args[n], XmNtopAttachment,    XmATTACH_WIDGET);          n++;
  XtSetArg (args[n], XmNtopWidget,        action_label);             n++;
  XtSetArg (args[n], XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget,       action_label);             n++;
  XtSetArg (args[n], XmNleftOffset,       2*HORIZONTAL_SPACING);     n++;
  G_overwrite_radio = (Widget) XmCreateRadioBox(form, "radio_box", args, n);

  /* create the radio buttons */
  xm_string[G_OVERWRITE_EXISTING] = XmStringCreateLocalized(
                        GETMESSAGE(5, 3, "Replace existing file"));
  xm_string[G_RENAME_EXISTING] = XmStringCreateLocalized(
                        GETMESSAGE(5, 4, "Rename existing file to:"));
  xm_string[G_SKIP] = XmStringCreateLocalized(
                        GETMESSAGE(5, 6, "Skip this file"));


  for (i=0; i < N_OVERWRITE_RADIO_BUTTONS ;i++ )
     {

       radiobtn[i] = XtVaCreateManagedWidget("radio_toggle",
                                            xmToggleButtonWidgetClass, G_overwrite_radio,
                                            XmNalignment,   XmALIGNMENT_BEGINNING,
                                            XmNlabelString, xm_string[i],
                                            NULL);
       XtAddCallback(radiobtn[i], XmNvalueChangedCallback, radio_callback, (XtPointer) i);
       XmStringFree(xm_string[i]);
     }

  /* initially set replace-file radio button to TRUE */
  G_overwrite_selection = G_OVERWRITE_EXISTING;
  n=0;
  XtSetArg (args[n], XmNset, TRUE); n++;
  XtSetValues(radiobtn[G_overwrite_selection], args, n);

  /* create text field for entering a new file name */
  generate_NewPath(gname,tname);
  split_path(gname,gfolder,gobject);
  G_rename_text = XtVaCreateManagedWidget("rename_text",
                              xmTextFieldWidgetClass,
                              form,
                              XmNleftAttachment,        XmATTACH_WIDGET,
                              XmNleftWidget,            G_overwrite_radio,
                              XmNleftOffset,       	HORIZONTAL_SPACING,
                              XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET,
                              XmNbottomWidget,          G_overwrite_radio,
                              XmNrightAttachment,      XmATTACH_FORM,
                              XmNrightOffset,          5,
                 	      XmNvalue, 		gobject,
                 	      XmNsensitive, 		FALSE,
                 	      XmNcursorPosition,        strlen(gobject),
                              NULL);


  /* create the toggle button */
  msg = XmStringCreateLocalized(GETMESSAGE(5, 7, "Apply this action to subsequent name conflicts"));
  G_over_confirm = XtVaCreateManagedWidget("over_confirm",
  			     xmToggleButtonWidgetClass,
  			     form,
                             XmNlabelString,      msg,
                             XmNset,              !G_overwrite_confirm,
                             XmNtopAttachment,    XmATTACH_WIDGET,
                             XmNtopWidget,        G_overwrite_radio,
                             XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET,
                             XmNleftWidget,       action_label,
                             XmNleftOffset,       0,
                             XmNrightAttachment,  XmATTACH_FORM,
                             XmNalignment,        XmALIGNMENT_BEGINNING,
                             NULL);
  XtAddCallback(G_over_confirm, XmNvalueChangedCallback, confirmtoggle_callback, NULL);
  XmStringFree(msg);

  /* create the action area of the Copy Directory Dialog */
  action_area = Create_Action_Area(form, overwriteActions, NULL);
  separator  =  XtVaCreateManagedWidget("separator",
                             xmSeparatorWidgetClass,
                             form,
                             XmNtopAttachment,     XmATTACH_WIDGET,
                             XmNtopWidget,         G_over_confirm,
                             XmNbottomAttachment,  XmATTACH_WIDGET,
                             XmNbottomWidget,      action_area,
                             XmNleftAttachment,    XmATTACH_FORM,
                             XmNrightAttachment,   XmATTACH_FORM,
                             NULL);


  /* set initial keyborad focus to the action button area */
  XtSetArg (args[0], XmNinitialFocus, action_area);
  XtSetValues(form, args, 1);

  /* Manage the overwrite Directory Dialog */
  XtManageChild(G_overwrite_radio);
  XtManageChild(form);
  XtManageChild(outerFrame);

  /* Choose a window position */
  _DtChildPosition(G_over_dialog, parent_widget, &x, &y);
  n=0;
  XtSetArg (args[n], XmNx, x); n++;
  XtSetArg (args[n], XmNy, y); n++;
  XtSetValues(G_over_dialog, args, n);

  /* Now that we have a window postion, map the dialog shell */
  XtMapWidget(G_over_dialog);

  return;
}


/*-------------------------------------
 *
 *  touch
 *
 *  create an empty file "path"
 *
 *  return 0 if ok
 *  return EOF and set errno if error
 *
 *------------------------------------*/

int
touch(String path)

{

FILE *stream;

errno = 0;
if ( ( stream = fopen(path, "w") ) == NULL)
   return ( EOF );

return ( fclose (stream) );

}  /* end touch */
