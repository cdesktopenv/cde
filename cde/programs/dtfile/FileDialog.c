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
/* $XConsortium: FileDialog.c /main/7 1996/10/23 16:25:18 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           FileDialog.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Simple Dialogs for file manipulation commands.
 *
 *   FUNCTIONS: ApplyFileDone
 *		DialogCancel
 *		FileTypePreview
 *		FileTypePreviewCB
 *		GetToPath
 *		MakeFileApply
 *		MakeFileDone
 *		MakeFileOk
 *		RenameDone
 *		RenameOk
 *		ShowCopyFileDialog
 *		ShowLinkFileDialog
 *		ShowMakeFileDialog
 *		ShowMoveFileDialog
 *		ShowRenameFileDialog
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#if defined(_AIX) || defined(hpux)
#include <sys/dir.h>
#else
#ifndef MAXNAMLEN
#define MAXNAMLEN 255
#endif
#endif

#ifdef __osf__
#include <unistd.h>
#endif

#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>

#include <X11/Shell.h>
#include <X11/Xatom.h>

#include <Dt/TitleBox.h>
#include <Dt/Icon.h>
#include <Dt/IconP.h>
#include <Dt/IconFile.h>
#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/FileM.h>
#include <Dt/HourGlass.h>
#include <Dt/Dts.h>
#include <Dt/SharedProcs.h>

#include <Tt/tttk.h>

#include "Encaps.h"
#include "SharedProcs.h"
#include "sharedFuncs.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Help.h"
#include "SharedMsgs.h"


#define FM_TYPE_LABEL  "TYPE_LABEL"


/********    Static Function Declarations    ********/

static void RenameOk(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
void RenameDone(
                        XtPointer client_data,
                        int rc) ;
static Boolean GetToPath(
                        char *to,
                        char *new_name,
                        char *host_in,
                        char *dir_in) ;
static void FileTypePreview(
                        Widget w,
                        String new_name,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void FileTypePreviewCB(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void MakeFileOk(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void MakeFileApply(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void MakeFileDone(
                        XtPointer client_data,
                        char *to,
                        int rc) ;
static void ApplyFileDone(
                        XtPointer client_data,
                        char *to,
                        int rc) ;
static void DialogCancel(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;

/********    End Static Function Declarations    ********/



/************************************************************************
 *
 *  ShowRenameFileDialog
 *	Callback function invoked from the File Manager menu.  This
 *	function creates and displays a dialog prompting for a new
 *	name for the selected file.
 *
 *  NOTE:  The Rename File... menu item can currently be reactivated
 *         by single select on another item.  Is this behavior correct?
 *
 ************************************************************************/

void
ShowRenameFileDialog(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
  FileMgrRec * file_mgr_rec;
  DialogData * dialog_data;
  FileViewData * file_view_data;
  Arg args[20];
  Widget mbar;
  FileMgrData * file_mgr_data;
  char * directory_name;
  int n;

   /*  Set the menu item to insensitive to prevent multiple  */
   /*  dialogs from being posted and get the area under the  */
   /*  menu pane redrawn.                                    */

  XmUpdateDisplay (w);

  if(client_data != 0)
  {
    file_view_data = (FileViewData *)client_data;
    mbar = XtParent(w);
  }
  else
  {
    file_view_data = NULL;
    mbar = XmGetPostedFromWidget(XtParent(w));
  }


  XtSetArg(args[0], XmNuserData, &file_mgr_rec);
  XtGetValues(mbar, args, 1);

  dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
  file_mgr_data = (FileMgrData *) dialog_data->data;


  /*  Get the selected file.  */

  if( file_view_data == NULL )
    file_view_data = file_mgr_data->selection_list[0];

  directory_name = ((DirectorySet *) file_view_data->directory_set)->name;

  /*  Create the widget hierarchy for the dialog  */

  n = 0;
  XtSetArg (args[n], XmNallowShellResize, True);  n++;

  /* Ignore accelerators when we're insensitive */
  if(client_data == 0)
  {
    if ((file_mgr_rec->menuStates & RENAME) == 0)
      return;
    file_mgr_rec->menuStates &= ~RENAME;
  }
  else
  {
    if(file_mgr_data->selected_file_count != 0)
      file_mgr_rec->menuStates |= RENAME;
  }

  UnpostTextField (file_mgr_data);

  CreateNameChangeDialog(file_view_data->widget, file_view_data,
                         (XtPointer)file_mgr_data, 0);
}


/************************************************************************
 *
 *  ShowCopyFileDialog
 *	Callback function invoked from the File Manager menu.  This
 *	function creates and displays a dialog prompting for a new
 *	name for the selected file.
 *
 *  NOTE:  The Rename File... menu item can currently be reactivated
 *         by single select on another item.  Is this behavior correct?
 *
 ************************************************************************/

void
ShowCopyFileDialog(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileMgrRec * file_mgr_rec;
   DialogData * dialog_data;
   FileMgrData * file_mgr_data;
   static FileViewData * file_view_data;    /* static because it is passed to RenameOk callback */

   char name_string[18+MAX_PATH];
   char * directory_name;
   char * tmpStr, *tempStr;

   XtArgVal width;
   Dimension f_width, d_width;

   Widget shell;
   Widget form, dir_form, file_form;
   Widget current_name;
   Widget dir_label, dir_text;
   Widget file_label, file_text;
   Widget file_type_box;
   Widget file_type;
   Widget separator;
   Widget ok;
   Widget cancel;
   Widget preview;
   Widget help;
   int flag = FLAG_RESET;

   DialogCallbackStruct * call_struct;
   XmString label_string;

   Arg args[20];
   int n;
   Widget mbar;
   XtTranslations trans_table;
   Atom delete_window_atom;

   /*  Set the menu item to insensitive to prevent multiple  */
   /*  dialogs from being posted and get the area under the  */
   /*  menu pane redrawn.                                    */

   XmUpdateDisplay (w);

   if(client_data != 0)
   {
      file_view_data = (FileViewData *)client_data;
      mbar = XtParent(w);
   }
   else
   {
      file_view_data = NULL;
      mbar = XmGetPostedFromWidget(XtParent(w));
   }


   XtSetArg(args[0], XmNuserData, &file_mgr_rec);
   XtGetValues(mbar, args, 1);

   /* Ignore accelerators when we're insensitive */

   if ((file_mgr_rec->menuStates & DUPLICATE) == 0)
     return;

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;


   /*  Get the selected file.  */

   if(file_view_data == NULL)
      file_view_data = file_mgr_data->selection_list[0];
   directory_name = ((DirectorySet *) file_view_data->directory_set)->name;


   /*  Create the widget hierarchy for the dialog  */

   n = 0;
   XtSetArg (args[n], XmNallowShellResize, True);  n++;

   /*  Adjust the decorations for the dialog shell of the dialog  */

   XtSetArg (args[n], XmNmwmFunctions, MWM_FUNC_MOVE | MWM_FUNC_CLOSE); ++n;
   XtSetArg (args[n], XmNmwmDecorations, MWM_DECOR_BORDER | MWM_DECOR_TITLE);                       ++n;

   file_mgr_rec->menuStates &= ~DUPLICATE;
   if(file_mgr_data->title != NULL &&
      strcmp(file_mgr_data->helpVol, DTFILE_HELP_NAME) != 0)
   {
     tmpStr = GETMESSAGE(10, 44, "Copy Object");
     tempStr = (char *)XtMalloc(strlen(tmpStr) +
                                strlen(file_mgr_data->title) + 5);
     sprintf(tempStr, "%s - %s", file_mgr_data->title, tmpStr);
   }
   else
   {
     tmpStr = (GETMESSAGE(10,45, "File Manager - Copy Object"));
     tempStr = XtNewString(tmpStr);
   }

   XtSetArg (args[n], XmNtitle, tempStr);      n++;
   shell = XmCreateDialogShell (file_mgr_rec->shell, "copy_file", args, n);
   XtFree(tempStr);

   /* Set the useAsyncGeo on the shell */
   XtSetArg (args[0], XmNuseAsyncGeometry, True);
   XtSetValues (XtParent(shell), args, 1);


   trans_table = XtParseTranslationTable(translations_space);

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);                n++;
   XtSetArg (args[n], XmNmarginHeight, 1);               n++;
   XtSetArg (args[n], XmNshadowThickness, 1);            n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);      n++;
   XtSetArg (args[n], XmNautoUnmanage, False);           n++;
#ifdef _CHECK_FOR_SPACES
   XtSetArg (args[n], XmNtextTranslations, trans_table); n++;
#endif
   XtSetArg (args[n], XmNuserData, file_mgr_rec);        n++;
   form = XmCreateForm (shell, "form", args, n);
   XtAddCallback(form, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);


   sprintf (name_string, "%s ", ((char *)GETMESSAGE(10, 37, "Selected object:")));
   strcat (name_string, file_view_data->file_data->file_name);
   label_string = XmStringCreateLocalized (name_string);

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);        n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);   n++;
   XtSetArg (args[n], XmNtopOffset, 5);                     n++;
   XtSetArg (args[n], XmNtraversalOn, False);               n++;
   current_name = XmCreateLabelGadget (form, "current_name", args, n);
   XtManageChild (current_name);
   XtAddCallback(current_name, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);

   XmStringFree (label_string);

   /* Create the field displaying the file type info */

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);            n++;
   XtSetArg(args[n], XmNtopWidget, current_name);                   n++;
   XtSetArg(args[n], XmNtopOffset, 3);                              n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);            n++;
   XtSetArg(args[n], XmNrightOffset, 5);                            n++;
   XtSetArg(args[n], XmNmarginHeight, 5);                           n++;
   XtSetArg(args[n], XmNmarginWidth, 5);                            n++;
   file_type_box = XmCreateFrame (form, "file_type_box", args, n);
   XtManageChild (file_type_box);
   XtAddCallback(file_type_box, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 8, "File Type")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);          n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                 n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_ETCHED_OUT);    n++;
   XtSetArg (args[n], XmNpixmapPosition, XmPIXMAP_TOP);       n++;
   XtSetArg (args[n], XmNtraversalOn, False);                 n++;
   file_type = _DtCreateIcon (file_type_box, "file_type", args, n);
   XtManageChild (file_type);
   XtAddCallback(file_type, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);

   XmStringFree (label_string);


   /* Adjust the colors of the icon */
   SetToNormalColors (file_type, form, file_mgr_rec->main, 0);


   /* ------------------------------------------------------------ */
   /*  Create a form to hold the destination label and TextField.  */
   /*  The form makes sure they will always be aligned together.   */
   /* ------------------------------------------------------------ */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);     n++;
   XtSetArg (args[n], XmNleftOffset, 5);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);    n++;
   XtSetArg (args[n], XmNtopWidget, current_name);           n++;
   XtSetArg (args[n], XmNtopOffset, 10);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
   XtSetArg (args[n], XmNrightWidget, file_type_box);        n++;
   XtSetArg (args[n], XmNrightOffset, 5);                    n++;

   dir_form = XmCreateForm (form, "dir_form", args, n);
   XtManageChild (dir_form);

   /* Create the Destination Label */
   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 34, "Destination Folder: ")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);         n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_END);        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                n++;
   dir_label = XmCreateLabelGadget (dir_form, "new_name", args, n);
   XtManageChild (dir_label);
   XtAddCallback(dir_label, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);

   XmStringFree (label_string);

   /* Create the Destination TextField */
   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);  n++;
   XtSetArg (args[n], XmNleftOffset, 5);                    n++;
   XtSetArg (args[n], XmNleftWidget, dir_label);            n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);   n++;
   XtSetArg (args[n], XmNtraversalOn, True);                n++;
   XtSetArg (args[n], XmNcursorPosition,
               strlen(file_view_data->file_data->file_name));  n++;
   dir_text = XmCreateTextField (dir_form, "new_text", args, n);
   XtManageChild (dir_text);
   XtAddCallback(dir_text, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);
   XmTextFieldInsert(dir_text, 0, directory_name);

   /* -------------------------------------------------------------- */
   /*  Create a form to hold the Name for Copy label and TextField.  */
   /*  The form makes sure they will always be aligned together.     */
   /* -------------------------------------------------------------- */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);     n++;
   XtSetArg (args[n], XmNleftOffset, 5);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);    n++;
   XtSetArg (args[n], XmNtopWidget, dir_form);               n++;
   XtSetArg (args[n], XmNtopOffset, 5);                      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
   XtSetArg (args[n], XmNrightWidget, file_type_box);        n++;
   XtSetArg (args[n], XmNrightOffset, 5);                    n++;

   file_form = XmCreateForm (form, "file_form", args, n);
   XtManageChild (file_form);

   /* Create the Name for Copy Label */
   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 38, "Name for copy : ")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);         n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_END);        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                n++;
   file_label = XmCreateLabelGadget (file_form, "new_name", args, n);
   XtManageChild (file_label);
   XtAddCallback(file_label, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);

   XmStringFree (label_string);

   /* Create the Name for Copy TextField */
   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);    n++;
   XtSetArg (args[n], XmNleftWidget, file_label);             n++;
   XtSetArg (args[n], XmNleftOffset, 5);                      n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);     n++;
   XtSetArg (args[n], XmNtraversalOn, True);                  n++;
   XtSetArg (args[n], XmNcursorPosition,
               strlen(file_view_data->file_data->file_name)); n++;
   file_text = XmCreateTextField (file_form, "new_text", args, n);
   XtManageChild (file_text);
   XtAddCallback(file_text, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);
   XmTextFieldInsert(file_text, 0, file_view_data->file_data->file_name);


   /* ------------------------------------------------ */
   /*  Make the two labels the same length - maximum.  */
   /* ------------------------------------------------ */

   XtVaGetValues(dir_label, XmNwidth, &width, NULL);
   d_width = (Dimension)width;
   XtVaGetValues(file_label, XmNwidth, &width, NULL);
   f_width = (Dimension)width;

   if (d_width > f_width)
       XtVaSetValues(file_label, XmNwidth, d_width, NULL);
   else
       XtVaSetValues(dir_label, XmNwidth, f_width, NULL);

   XtVaGetValues(dir_text, XmNwidth, &width, NULL);
   d_width = (Dimension)width;
   XtVaGetValues(file_text, XmNwidth, &width, NULL);
   f_width = (Dimension)width;

   if (d_width > f_width)
       XtVaSetValues(file_text, XmNwidth, d_width, NULL);
   else
       XtVaSetValues(dir_text, XmNwidth, f_width, NULL);


   /*  The data sent to the Text, Ok and Cancel callbacks  */

   call_struct =
      (DialogCallbackStruct *) XtMalloc (sizeof (DialogCallbackStruct));

   call_struct->dialog_widget = form;
   call_struct->menu_widget = w;
   call_struct->dir_text_widget = dir_text;
   call_struct->file_text_widget = file_text;
   call_struct->file_type_widget = file_type;
   call_struct->original_physical_type =
      file_view_data->file_data->physical_type;
   call_struct->file_mgr_rec = file_mgr_rec;
   call_struct->host_name = XtNewString (file_mgr_data->host);
   call_struct->directory_name = XtNewString (directory_name);
   call_struct->mode = COPY_FILE;
   call_struct->file_name = XtNewString (file_view_data->file_data->file_name);
   call_struct->original_logical_type =file_view_data->file_data->logical_type;
   call_struct->previous_file_type = NULL;
   call_struct->file_mgr_data = file_mgr_data;


   /*  Create a separator between the buttons  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopWidget, file_type_box);     	n++;
   XtSetArg (args[n], XmNtopOffset, 5); 			n++;
   separator =  XmCreateSeparatorGadget (form, "separator", args, n);
   XtManageChild (separator);



   /*  Create the ok, cancel, preview and help buttons  */

   n = 0;
   XtSetArg (args[n], XmNlabelString, okXmString);               n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);     n++;
   XtSetArg (args[n], XmNleftPosition, 1);                       n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNrightPosition, 21);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);        n++;
   XtSetArg (args[n], XmNtopWidget, separator);                  n++;
   XtSetArg (args[n], XmNtopOffset, 5);                          n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                       n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                        n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                       n++;
   XtSetArg (args[n], XmNshowAsDefault, True);                   n++;
   XtSetArg (args[n], XmNuserData, flag);                        n++;
   ok = XmCreatePushButtonGadget (form, "ok", args, n);
   XtManageChild (ok);

   XtAddCallback (ok, XmNactivateCallback, RenameOk, (XtPointer) call_struct);
   XtAddCallback(ok, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);


   /*  Set the ok button to the default for the bulletin board.  */
   /*  This causes the return key from the text widget to be     */
   /*  properly processed.                                       */

   XtSetArg (args[0], XmNdefaultButton, ok);
   XtSetValues (form, args, 1);


   n = 0;
   XtSetArg (args[n], XmNlabelString, cancelXmString);		n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 22);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNrightPosition, 42);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopWidget, separator);			n++;
   XtSetArg (args[n], XmNtopOffset, 5);				n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNbottomOffset, 5);			n++;
   XtSetArg (args[n], XmNmarginWidth, 4);			n++;
   XtSetArg (args[n], XmNmarginHeight, 4);			n++;
   XtSetArg (args[n], XmNuserData, flag);                       n++;
   cancel = XmCreatePushButtonGadget (form, "cancel", args, n);
   XtManageChild (cancel);

   XtAddCallback (cancel, XmNactivateCallback, DialogCancel,
                                                   (XtPointer) call_struct);
   XtAddCallback(cancel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 36, "Show Icon")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);      n++;
   XtSetArg (args[n], XmNleftPosition, 43);                       n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);     n++;
   XtSetArg (args[n], XmNrightPosition, 78);                      n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);         n++;
   XtSetArg (args[n], XmNtopWidget, separator);                   n++;
   XtSetArg (args[n], XmNtopOffset, 5);                           n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                        n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                         n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                        n++;
   preview = XmCreatePushButtonGadget (form, "show_file_type", args, n);
   XtManageChild (preview);

   XmStringFree (label_string);
   XtAddCallback (preview, XmNactivateCallback, FileTypePreviewCB,
                                                   (XtPointer) call_struct);
   XtAddCallback(preview, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);
   tmpStr = XtNewString(file_view_data->file_data->file_name);
   FileTypePreview (preview, tmpStr, (XtPointer) call_struct, NULL);

   n = 0;
   XtSetArg (args[n], XmNlabelString, helpXmString);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);      n++;
   XtSetArg (args[n], XmNleftPosition, 79);                       n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);     n++;
   XtSetArg (args[n], XmNrightPosition, 99);                      n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);         n++;
   XtSetArg (args[n], XmNtopWidget, separator);                   n++;
   XtSetArg (args[n], XmNtopOffset, 5);                           n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                        n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                         n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                        n++;
   help = XmCreatePushButtonGadget (form, "help", args, n);
   XtManageChild (help);

   XtAddCallback(help, XmNactivateCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);
   XtAddCallback(help, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_COPY_DIALOG_STR);



   XtSetArg (args[0], XmNcancelButton, cancel);
   XtSetValues (form, args, 1);

   XtAddCallback (shell, XmNpopupCallback, (XtCallbackProc)_DtmapCB,
                                             (XtPointer) XtParent(shell));

   delete_window_atom = XmInternAtom (XtDisplay(shell), "WM_DELETE_WINDOW", True);
   XmRemoveWMProtocols( shell, &delete_window_atom, 1 );
   XmAddWMProtocolCallback( shell, delete_window_atom, DialogCancel,
                            (XtPointer) call_struct );

   call_struct->Ok               = ok;
   call_struct->Cancel           = cancel;

   XtManageChild (form);

   /* Force the focus to the text field */
   XmProcessTraversal(dir_text, XmTRAVERSE_CURRENT);
}

/************************************************************************
 *
 *  ShowMoveFileDialog
 *	Callback function invoked from the File Manager menu.  This
 *	function creates and displays a dialog prompting for a new
 *	name for the selected file.
 *
 *  NOTE:  The Rename File... menu item can currently be reactivated
 *         by single select on another item.  Is this behavior correct?
 *
 ************************************************************************/

void
ShowMoveFileDialog(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileMgrRec * file_mgr_rec;
   DialogData * dialog_data;
   static FileMgrData * file_mgr_data;   /* static because it is passed to RenameOk callback */
   FileViewData * file_view_data;

   char name_string[18+MAX_PATH];
   char * directory_name;
   char * tmpStr, *tempStr;

   Widget shell;
   Widget form;
   Widget current_name;
   Widget new_name;
   Widget new_text;
   Widget separator;
   Widget ok;
   Widget cancel;
   Widget help;
   int flag=FLAG_RESET;

   DialogCallbackStruct * call_struct;
   XmString label_string;

   Arg args[20];
   int n;
   Widget mbar;
   XtTranslations trans_table;
   Atom delete_window_atom;

   /*  Set the menu item to insensitive to prevent multiple  */
   /*  dialogs from being posted and get the area under the  */
   /*  menu pane redrawn.                                    */

   XmUpdateDisplay (w);

   if(client_data != 0)
   {
      file_view_data = (FileViewData *)client_data;
      mbar = XtParent(w);
   }
   else
   {
      file_view_data = NULL;
      mbar = XmGetPostedFromWidget(XtParent(w));
   }


   XtSetArg(args[0], XmNuserData, &file_mgr_rec);
   XtGetValues(mbar, args, 1);

   /* Ignore accelerators when we're insensitive */

   if ((file_mgr_rec->menuStates & MOVE) == 0)
     return;

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   /*  Get the selected file.  */

   if(file_view_data == NULL)
      file_view_data = file_mgr_data->selection_list[0];
   directory_name = ((DirectorySet *) file_view_data->directory_set)->name;


   /*  Create the widget hierarchy for the dialog  */

   n = 0;
   XtSetArg (args[n], XmNallowShellResize, True);  n++;

   /*  Adjust the decorations for the dialog shell of the dialog  */
   XtSetArg (args[n], XmNmwmFunctions, MWM_FUNC_MOVE | MWM_FUNC_CLOSE); ++n;
   XtSetArg (args[n], XmNmwmDecorations, MWM_DECOR_BORDER | MWM_DECOR_TITLE);                       ++n;

   file_mgr_rec->menuStates &= ~MOVE;
   if(file_mgr_data->title != NULL &&
      strcmp(file_mgr_data->helpVol, DTFILE_HELP_NAME) != 0)
   {
     tmpStr = GETMESSAGE(10, 42, "Move Object");
     tempStr = (char *)XtMalloc(strlen(tmpStr) +
                                strlen(file_mgr_data->title) + 5);
     sprintf(tempStr, "%s - %s", file_mgr_data->title, tmpStr);
   }
   else
   {
     tmpStr = (GETMESSAGE(10,43, "File Manager - Move Object"));
     tempStr = XtNewString(tmpStr);
   }
   XtSetArg (args[n], XmNtitle, tempStr);      n++;
   shell = XmCreateDialogShell (file_mgr_rec->shell, "move_file", args, n);
   XtFree(tempStr);

   /* Set the useAsyncGeo on the shell */
   XtSetArg (args[0], XmNuseAsyncGeometry, True);
   XtSetValues (XtParent(shell), args, 1);


   trans_table = XtParseTranslationTable(translations_space);

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);		 n++;
   XtSetArg (args[n], XmNmarginHeight, 1);		 n++;
   XtSetArg (args[n], XmNshadowThickness, 1);		 n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);	 n++;
   XtSetArg (args[n], XmNautoUnmanage, False);		 n++;
#ifdef _CHECK_FOR_SPACES
   XtSetArg (args[n], XmNtextTranslations, trans_table);n++;
#endif
   XtSetArg (args[n], XmNuserData, file_mgr_rec);        n++;
   form = XmCreateForm (shell, "form", args, n);
   XtAddCallback(form, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_MOVE_DIALOG_STR);


   sprintf (name_string, "%s ", ((char *)GETMESSAGE(10, 37, "Selected object:")));
   strcat (name_string, file_view_data->file_data->file_name);
   label_string = XmStringCreateLocalized (name_string);

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);		n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNleftOffset, 5);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopOffset, 5);				n++;
   XtSetArg (args[n], XmNtraversalOn, False);			n++;
   current_name = XmCreateLabelGadget (form, "current_name", args, n);
   XtManageChild (current_name);
   XtAddCallback(current_name, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_MOVE_DIALOG_STR);

   XmStringFree (label_string);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 34, "Destination Folder: ")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);		n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNleftOffset, 5);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopWidget, current_name);		n++;
   XtSetArg (args[n], XmNtopOffset, 12);			n++;
   XtSetArg (args[n], XmNtraversalOn, False);			n++;
   new_name = XmCreateLabelGadget (form, "new_name", args, n);
   XtManageChild (new_name);
   XtAddCallback(new_name, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_MOVE_DIALOG_STR);

   XmStringFree (label_string);


   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);   n++;
   XtSetArg (args[n], XmNleftWidget, new_name);              n++;
   XtSetArg (args[n], XmNleftOffset, 0);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);    n++;
   XtSetArg (args[n], XmNtopWidget, current_name);           n++;
   XtSetArg (args[n], XmNtopOffset, 5);                      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);    n++;
   XtSetArg (args[n], XmNrightOffset, 5);                    n++;
   XtSetArg (args[n], XmNtraversalOn, True);                 n++;
   XtSetArg (args[n], XmNcursorPosition,
               strlen(file_view_data->file_data->file_name));	n++;
   new_text = XmCreateTextField (form, "new_text", args, n);
   XtManageChild (new_text);
   XtAddCallback(new_text, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_MOVE_DIALOG_STR);

   /*  The data sent to the Text, Ok and Cancel callbacks  */

   call_struct =
      (DialogCallbackStruct *) XtMalloc (sizeof (DialogCallbackStruct));

   call_struct->dialog_widget = form;
   call_struct->menu_widget = w;
   call_struct->dir_text_widget = new_text;
   call_struct->file_text_widget = NULL;
   call_struct->file_type_widget = NULL;
   call_struct->original_physical_type =
      file_view_data->file_data->physical_type;
   call_struct->file_mgr_rec = file_mgr_rec;
   call_struct->host_name = XtNewString (file_mgr_data->host);
   call_struct->directory_name = XtNewString (directory_name);
   call_struct->file_name = XtNewString (file_view_data->file_data->file_name);
   call_struct->mode = MOVE_FILE;
   call_struct->original_logical_type =file_view_data->file_data->logical_type;
   call_struct->previous_file_type = NULL;
   call_struct->file_mgr_data = file_mgr_data;


   /*  Create a separator between the buttons  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);    n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);   n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);   n++;
   XtSetArg (args[n], XmNtopWidget, new_name);              n++;
   XtSetArg (args[n], XmNtopOffset, 10);                    n++;
   separator =  XmCreateSeparatorGadget (form, "separator", args, n);
   XtManageChild (separator);



   /*  Create the ok, cancel, preview and help buttons  */


   n = 0;
   XtSetArg (args[n], XmNlabelString, okXmString);               n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);     n++;
   XtSetArg (args[n], XmNleftPosition, 2);                       n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNrightPosition, 32);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);        n++;
   XtSetArg (args[n], XmNtopWidget, separator);                  n++;
   XtSetArg (args[n], XmNtopOffset, 5);                          n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                       n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                        n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                       n++;
   XtSetArg (args[n], XmNshowAsDefault, True);                   n++;
   XtSetArg (args[n], XmNuserData, flag);                        n++;
   ok = XmCreatePushButtonGadget (form, "ok", args, n);
   XtManageChild (ok);

   XtAddCallback (ok, XmNactivateCallback, RenameOk, (XtPointer) call_struct);
   XtAddCallback(ok, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_MOVE_DIALOG_STR);


   /*  Set the ok button to the default for the bulletin board.  */
   /*  This causes the return key from the text widget to be     */
   /*  properly processed.                                       */

   XtSetArg (args[0], XmNdefaultButton, ok);
   XtSetValues (form, args, 1);


   n = 0;
   XtSetArg (args[n], XmNlabelString, cancelXmString);           n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);     n++;
   XtSetArg (args[n], XmNleftPosition, 35);                      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNrightPosition, 65);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);        n++;
   XtSetArg (args[n], XmNtopWidget, separator);                  n++;
   XtSetArg (args[n], XmNtopOffset, 5);                          n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                       n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                        n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                       n++;
   XtSetArg (args[n], XmNuserData, flag);                        n++;
   cancel = XmCreatePushButtonGadget (form, "cancel", args, n);
   XtManageChild (cancel);

   XtAddCallback (cancel, XmNactivateCallback, DialogCancel,
                                                   (XtPointer) call_struct);
   XtAddCallback(cancel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_MOVE_DIALOG_STR);


   n = 0;
   XtSetArg (args[n], XmNlabelString, helpXmString);             n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);     n++;
   XtSetArg (args[n], XmNleftPosition, 68);                      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNrightPosition, 98);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);        n++;
   XtSetArg (args[n], XmNtopWidget, separator);                  n++;
   XtSetArg (args[n], XmNtopOffset, 5);                          n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                       n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                        n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                       n++;
   help = XmCreatePushButtonGadget (form, "help", args, n);
   XtManageChild (help);

   XtAddCallback(help, XmNactivateCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_MOVE_DIALOG_STR);
   XtAddCallback(help, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_MOVE_DIALOG_STR);



   XtSetArg (args[0], XmNcancelButton, cancel);
   XtSetValues (form, args, 1);

   XtAddCallback (shell, XmNpopupCallback, (XtCallbackProc)_DtmapCB,
                                             (XtPointer) XtParent(shell));

   delete_window_atom = XmInternAtom (XtDisplay(shell), "WM_DELETE_WINDOW", True);
   XmRemoveWMProtocols( shell, &delete_window_atom, 1 );
   XmAddWMProtocolCallback( shell, delete_window_atom, DialogCancel,
                            (XtPointer) call_struct );

   call_struct->Ok               = ok;
   call_struct->Cancel           = cancel;

   XtManageChild (form);

   /* Force the focus to the text field */
   XmProcessTraversal(new_text, XmTRAVERSE_CURRENT);

}

/************************************************************************
 *
 *  ShowLinkFileDialog
 *	Callback function invoked from the File Manager menu.  This
 *	function creates and displays a dialog prompting for a new
 *	name for the selected file.
 *
 *  NOTE:  The Rename File... menu item can currently be reactivated
 *         by single select on another item.  Is this behavior correct?
 *
 ************************************************************************/

void
ShowLinkFileDialog(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileMgrRec * file_mgr_rec;
   DialogData * dialog_data;
   static FileMgrData * file_mgr_data;   /* static because it is passed to RenameOk callback */
   FileViewData * file_view_data;

   char name_string[18+MAX_PATH];
   char * directory_name;
   char * tmpStr, *tempStr;

   Widget shell;
   Widget form;
   Widget current_name;
   Widget dir_label, dir_text;
   Widget file_label, file_text;
   Widget file_type_box;
   Widget file_type;
   Widget separator;
   Widget ok;
   Widget cancel;
   Widget preview;
   Widget help;
   int flag=FLAG_RESET;

   DialogCallbackStruct * call_struct;
   XmString label_string;

   Arg args[20];
   int n;
   Widget mbar;
   XtTranslations trans_table;
   Atom delete_window_atom;

   /*  Set the menu item to insensitive to prevent multiple  */
   /*  dialogs from being posted and get the area under the  */
   /*  menu pane redrawn.                                    */

   XmUpdateDisplay (w);

   if(client_data != 0)
   {
      file_view_data = (FileViewData *)client_data;
      mbar = XtParent(w);
   }
   else
   {
      file_view_data = NULL;
      mbar = XmGetPostedFromWidget(XtParent(w));
   }


   XtSetArg(args[0], XmNuserData, &file_mgr_rec);
   XtGetValues(mbar, args, 1);

   /* Ignore accelerators when we're insensitive */

   if ((file_mgr_rec->menuStates & LINK) == 0)
     return;

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;


   /*  Get the selected file.  */

   if(file_view_data == NULL)
      file_view_data = file_mgr_data->selection_list[0];
   directory_name = ((DirectorySet *) file_view_data->directory_set)->name;


   /*  Create the widget hierarchy for the dialog  */

   n = 0;
   XtSetArg (args[n], XmNallowShellResize, True);  n++;

   file_mgr_rec->menuStates &= ~LINK;
   if(file_mgr_data->title != NULL &&
      strcmp(file_mgr_data->helpVol, DTFILE_HELP_NAME) != 0)
   {
     tmpStr = GETMESSAGE(10, 46, "Link Object");
     tempStr = (char *)XtMalloc(strlen(tmpStr) +
                                strlen(file_mgr_data->title) + 5);
     sprintf(tempStr, "%s - %s", file_mgr_data->title, tmpStr);
   }
   else
   {
     tmpStr = (GETMESSAGE(10,47, "File Manager - Link Object"));
     tempStr = XtNewString(tmpStr);
   }
   XtSetArg (args[n], XmNtitle, tempStr);      n++;
   shell = XmCreateDialogShell (file_mgr_rec->shell, "link_file", args, n);
   XtFree(tempStr);

   /* Set the useAsyncGeo on the shell */
   XtSetArg (args[0], XmNuseAsyncGeometry, True);
   XtSetValues (XtParent(shell), args, 1);


   trans_table = XtParseTranslationTable(translations_space);

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);                n++;
   XtSetArg (args[n], XmNmarginHeight, 1);               n++;
   XtSetArg (args[n], XmNshadowThickness, 1);            n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);      n++;
   XtSetArg (args[n], XmNautoUnmanage, False);           n++;
#ifdef _CHECK_FOR_SPACES
   XtSetArg (args[n], XmNtextTranslations, trans_table); n++;
#endif
   XtSetArg (args[n], XmNuserData, file_mgr_rec);        n++;
   form = XmCreateForm (shell, "form", args, n);
   XtAddCallback(form, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);


   sprintf (name_string, "%s ", ((char *)GETMESSAGE(10, 37, "Selected object:")));
   strcat (name_string, file_view_data->file_data->file_name);
   label_string = XmStringCreateLocalized (name_string);

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);        n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);    n++;

   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);     n++;
   XtSetArg (args[n], XmNtopOffset, 5);                     n++;
   XtSetArg (args[n], XmNtraversalOn, False);               n++;
   current_name = XmCreateLabelGadget (form, "current_name", args, n);
   XtManageChild (current_name);
   XtAddCallback(current_name, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);

   XmStringFree (label_string);

   /* Create the field displaying the file type info */

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);              n++;
   XtSetArg(args[n], XmNtopOffset, 15);                             n++;
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);            n++;
   XtSetArg(args[n], XmNrightOffset, 5);                            n++;
   XtSetArg(args[n], XmNmarginHeight, 5);                           n++;
   XtSetArg(args[n], XmNmarginWidth, 5);                            n++;
   file_type_box = XmCreateFrame (form, "file_type_box", args, n);
   XtManageChild (file_type_box);
   XtAddCallback(file_type_box, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 8, "File Type")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);          n++;
   XtSetArg (args[n], XmNshadowThickness, 0);                 n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_ETCHED_OUT);    n++;
   XtSetArg (args[n], XmNpixmapPosition, XmPIXMAP_TOP);       n++;
   XtSetArg (args[n], XmNtraversalOn, False);                 n++;
   file_type = _DtCreateIcon (file_type_box, "file_type", args, n);
   XtManageChild (file_type);
   XtAddCallback(file_type, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);

   XmStringFree (label_string);


   /* Adjust the colors of the icon */
   SetToNormalColors (file_type, form, file_mgr_rec->main, 0);

   n = 0;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);    n++;
   XtSetArg (args[n], XmNrightWidget, file_type_box);          n++;
   XtSetArg (args[n], XmNrightOffset, 5);                      n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);      n++;
   XtSetArg (args[n], XmNtopWidget, current_name);             n++;
   XtSetArg (args[n], XmNtopOffset, 5);                        n++;
   XtSetArg (args[n], XmNtraversalOn, True);                   n++;
   XtSetArg (args[n], XmNcursorPosition,
               strlen(file_view_data->file_data->file_name));  n++;
   dir_text = XmCreateTextField (form, "new_text", args, n);
   XtManageChild (dir_text);
   XtAddCallback(dir_text, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);
   XmTextFieldInsert(dir_text, 0, directory_name);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 34, "Destination Folder: ")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);         n++;
   XtSetArg (args[n], XmNrightWidget, dir_text);             n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
   XtSetArg (args[n], XmNrightOffset, 5);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);    n++;
   XtSetArg (args[n], XmNtopWidget, current_name);           n++;
   XtSetArg (args[n], XmNtopOffset, 12);                     n++;
   XtSetArg (args[n], XmNtraversalOn, False);                n++;
   dir_label = XmCreateLabelGadget (form, "new_name", args, n);
   XtManageChild (dir_label);
   XtAddCallback(dir_label, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);

   XmStringFree (label_string);

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);    n++;
   XtSetArg (args[n], XmNleftWidget, dir_text);                        n++;
   XtSetArg (args[n], XmNleftOffset, 0);                               n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);              n++;
   XtSetArg (args[n], XmNtopWidget, dir_label);                        n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                n++;
   XtSetArg (args[n], XmNtraversalOn, True);                           n++;
   XtSetArg (args[n], XmNcursorPosition,
               strlen(file_view_data->file_data->file_name));          n++;
   file_text = XmCreateTextField (form, "new_text", args, n);
   XtManageChild (file_text);
   XtAddCallback(file_text, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);
   XmTextFieldInsert(file_text, 0, file_view_data->file_data->file_name);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 38, "Name for copy : ")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNrightWidget, file_text);               n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);     n++;
   XtSetArg (args[n], XmNrightOffset, 5);                       n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, dir_label);                 n++;
   XtSetArg (args[n], XmNtopOffset, 12);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   file_label = XmCreateLabelGadget (form, "new_name", args, n);
   XtManageChild (file_label);
   XtAddCallback(file_label, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);

   XmStringFree (label_string);


   /*  The data sent to the Text, Ok and Cancel callbacks  */

   call_struct =
      (DialogCallbackStruct *) XtMalloc (sizeof (DialogCallbackStruct));

   call_struct->dialog_widget = form;
   call_struct->menu_widget = w;
   call_struct->dir_text_widget = dir_text;
   call_struct->file_text_widget = file_text;
   call_struct->file_type_widget = file_type;
   call_struct->original_physical_type =
      file_view_data->file_data->physical_type;
   call_struct->file_mgr_rec = file_mgr_rec;
   call_struct->host_name = XtNewString (file_mgr_data->host);
   call_struct->directory_name = XtNewString (directory_name);
   call_struct->mode = LINK_FILE;
   call_struct->file_name = XtNewString (file_view_data->file_data->file_name);
   call_struct->original_logical_type =file_view_data->file_data->logical_type;
   call_struct->previous_file_type = NULL;
   call_struct->file_mgr_data = file_mgr_data;


   /*  Create a separator between the buttons  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopWidget, file_label);			n++;
   XtSetArg (args[n], XmNtopOffset, 10);			n++;
   separator =  XmCreateSeparatorGadget (form, "separator", args, n);
   XtManageChild (separator);



   /*  Create the ok, cancel, preview and help buttons  */


   n = 0;
   XtSetArg (args[n], XmNlabelString, okXmString);               n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);     n++;
   XtSetArg (args[n], XmNleftPosition, 1);                       n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNrightPosition, 21);                     n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);        n++;
   XtSetArg (args[n], XmNtopWidget, separator);                  n++;
   XtSetArg (args[n], XmNtopOffset, 5);                          n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                       n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                        n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                       n++;
   XtSetArg (args[n], XmNshowAsDefault, True);                   n++;
   XtSetArg (args[n], XmNuserData, flag);                        n++;
   ok = XmCreatePushButtonGadget (form, "ok", args, n);
   XtManageChild (ok);

   XtAddCallback (ok, XmNactivateCallback, RenameOk, (XtPointer) call_struct);
   XtAddCallback(ok, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);


   /*  Set the ok button to the default for the bulletin board.  */
   /*  This causes the return key from the text widget to be     */
   /*  properly processed.                                       */

   XtSetArg (args[0], XmNdefaultButton, ok);
   XtSetValues (form, args, 1);


   n = 0;
   XtSetArg (args[n], XmNlabelString, cancelXmString);		n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNleftPosition, 22);			n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);	n++;
   XtSetArg (args[n], XmNrightPosition, 42);			n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
   XtSetArg (args[n], XmNtopWidget, separator);			n++;
   XtSetArg (args[n], XmNtopOffset, 5);				n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNbottomOffset, 5);			n++;
   XtSetArg (args[n], XmNmarginWidth, 4);			n++;
   XtSetArg (args[n], XmNmarginHeight, 4);			n++;
   XtSetArg (args[n], XmNuserData, flag);                        n++;
   cancel = XmCreatePushButtonGadget (form, "cancel", args, n);
   XtManageChild (cancel);

   XtAddCallback (cancel, XmNactivateCallback, DialogCancel,
                                                   (XtPointer) call_struct);
   XtAddCallback(cancel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 36, "Show Icon")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);      n++;
   XtSetArg (args[n], XmNleftPosition, 43);                       n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);     n++;
   XtSetArg (args[n], XmNrightPosition, 78);                      n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);         n++;
   XtSetArg (args[n], XmNtopWidget, separator);                   n++;
   XtSetArg (args[n], XmNtopOffset, 5);                           n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                        n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                         n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                        n++;
   preview = XmCreatePushButtonGadget (form, "show_file_type", args, n);
   XtManageChild (preview);

   XmStringFree (label_string);
   XtAddCallback (preview, XmNactivateCallback, FileTypePreviewCB,
                                                   (XtPointer) call_struct);
   XtAddCallback(preview, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);
   tmpStr = XtNewString(file_view_data->file_data->file_name);
   FileTypePreview (preview, tmpStr, (XtPointer) call_struct, NULL);

   n = 0;
   XtSetArg (args[n], XmNlabelString, helpXmString);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);      n++;
   XtSetArg (args[n], XmNleftPosition, 79);                       n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);     n++;
   XtSetArg (args[n], XmNrightPosition, 99);                      n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);         n++;
   XtSetArg (args[n], XmNtopWidget, separator);                   n++;
   XtSetArg (args[n], XmNtopOffset, 5);                           n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                        n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                         n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                        n++;
   help = XmCreatePushButtonGadget (form, "help", args, n);
   XtManageChild (help);

   XtAddCallback(help, XmNactivateCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);
   XtAddCallback(help, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_LINK_DIALOG_STR);



   XtSetArg (args[0], XmNcancelButton, cancel);
   XtSetValues (form, args, 1);

   XtAddCallback (shell, XmNpopupCallback, (XtCallbackProc)_DtmapCB,
                                             (XtPointer) XtParent(shell));

   delete_window_atom = XmInternAtom (XtDisplay(shell), "WM_DELETE_WINDOW", True);
   XmRemoveWMProtocols( shell, &delete_window_atom, 1 );
   XmAddWMProtocolCallback( shell, delete_window_atom, DialogCancel,
                            (XtPointer) call_struct );

   call_struct->Ok     = ok;
   call_struct->Cancel = cancel;

   XtManageChild (form);

   /* Force the focus to the text field */
   XmProcessTraversal(dir_text, XmTRAVERSE_CURRENT);
}



/************************************************************************
 *
 *  RenameOk
 *	Get the new file name and rename the file.
 *
 ************************************************************************/

static void
RenameOk(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   DialogCallbackStruct * call_struct = (DialogCallbackStruct *)client_data;
   char *title;
   char *msg = NULL;
   char *new_name = NULL, *new_dir = NULL, new[MAX_PATH];
   unsigned int btnMask;
   RenameDoneData *data;
   char to_host[256];
   char to_dir[MAX_PATH];
   char to_file[256];
   unsigned int modifiers;
   char *host_set[1], *file_set[1];
   int  file_count = 1;

   if(RecheckFlag(call_struct->dialog_widget,w))   /* OK button */
     return;
   else if(RecheckFlag(NULL,call_struct->Cancel))
     return;

   /* currently, only one file can be moved/copied/linked using the menu */
   /* the following arrays can be expanded if this restriction is lifted */

   new_dir = XmTextFieldGetString (call_struct->dir_text_widget);
   new_dir = (char *) _DtStripSpaces (new_dir);

   if (new_dir[0] == '~')
      new_dir = _DtChangeTildeToHome (new_dir);

   switch (call_struct->mode)
   {
     case MOVE_FILE:
       title = XtNewString(GetSharedMessage(FILE_MOVE_ERROR_TITLE));
       btnMask = MOVE;
       break;
     case COPY_FILE:
       title = XtNewString(GetSharedMessage(FILE_COPY_ERROR_TITLE));
       btnMask = DUPLICATE;
       break;
     case LINK_FILE:
       title = XtNewString(GetSharedMessage(FILE_LINK_ERROR_TITLE));
       btnMask = LINK;
       break;
   }

   if (strlen (new_dir) == 0)
     msg = XtNewString(GETMESSAGE(10, 11,
                                  "A file or folder cannot have a blank name."));
#ifdef _CHECK_FOR_SPACES
   else if (DtStrchr (new_dir, ' ') != NULL ||
            DtStrchr (new_dir, '\t') != NULL)
     msg = XtNewString(GetSharedMessage(NO_SPACES_ALLOWED_ERROR));
#endif

   switch (call_struct->mode)
   {
     case COPY_FILE:
     case LINK_FILE:
       new_name = XmTextFieldGetString (call_struct->file_text_widget);
       new_name = (char *) _DtStripSpaces (new_name);
       if (strlen (new_name) == 0)
         msg = XtNewString(GETMESSAGE(10, 11,
                                      "A file or folder cannot have a blank name."));
#ifdef _CHECK_FOR_SPACES
       else if (DtStrchr (new_name, ' ') != NULL ||
                DtStrchr (new_name, '\t') != NULL)
         msg = XtNewString(GetSharedMessage(NO_SPACES_ALLOWED_ERROR));
#endif
       sprintf(new, "%s/%s", new_dir, new_name );
       break;
     case MOVE_FILE:
       sprintf(new, "%s/%s", new_dir, call_struct->file_name);
       break;
   }

   if (msg != NULL)
   {
      _DtMessage(XtParent (w), title, msg, NULL, HelpRequestCB);
      XtFree(title);
      XtFree(msg);
      if(new_name) XtFree(new_name);
      if(new_dir) XtFree(new_dir);
      ResetFlag(call_struct->dialog_widget,w);
      ResetFlag(call_struct->dialog_widget,call_struct->Cancel);
      return;
   }

   XtFree(title);
   XtFree(msg);
   if(new_name) XtFree(new_name);
   if(new_dir) XtFree(new_dir);

   /* set up data for RenameDone callback */
   data = (RenameDoneData *)XtMalloc(sizeof(RenameDoneData));
   data->w = w;
   data->call_struct = call_struct;
   data->mode = call_struct->mode;
   data->btnMask = btnMask;

   /*
    * Do the rename/copy/...
    * This operation is performed in a background process; when the
    * background process finishes, the RenameDone callback will be
    * called with a return code indicating success or failure.
    */

   /* get host & path of the target file */
   GetTarget(call_struct->host_name, call_struct->directory_name, new,
             to_host, to_dir, to_file);
   /* get host & path of source file */
   host_set[0] = call_struct->host_name;
   file_set[0] = build_path(call_struct->directory_name, call_struct->file_name);
   /* set up modifiers depending on operation */
   switch (call_struct->mode)
   {
     case MOVE_FILE:
       modifiers = 0;
       break;
     case COPY_FILE:
       modifiers = ControlMask;
       break;
     case LINK_FILE:
       modifiers = ShiftMask;
       break;
   }

   FileMoveCopy(call_struct->file_mgr_data,
                to_file, to_dir, to_host,
                host_set, file_set, file_count,
                modifiers, RenameDone, (XtPointer)data);

   XtFree(file_set[0]);
   file_set[0] = NULL;
}


/************************************************************************
 *
 *  RenameDone
 *      This routine is called when the file move/copy/link is finished.
 *
 ************************************************************************/

void
RenameDone(
        XtPointer client_data,
        int rc)
{
   RenameDoneData *data = (RenameDoneData *)client_data;
   DialogCallbackStruct *call_struct = data->call_struct;

   if (rc == 0)
   {
      XtUnmanageChild(call_struct->dialog_widget);
      XmUpdateDisplay(call_struct->dialog_widget);
      XtDestroyWidget(XtParent(call_struct->dialog_widget));
      call_struct->file_mgr_rec->menuStates |= data->btnMask;

      XtFree(call_struct->host_name);
      XtFree(call_struct->directory_name);
      XtFree(call_struct->file_name);
      if(call_struct->previous_file_type)
        XtFree(call_struct->previous_file_type);

      XtFree((char *) call_struct);
   }

   XtFree((char *)data);
}


/************************************************************************
 *
 *  GetToPath
 *	Build a directory,filename path to be used as the destination
 *	of a create, copy or move operation.
 *
 ************************************************************************/

static Boolean
GetToPath (
        char *to,
        char *new_name,
        char *host_in,
        char *dir_in)
{
   char *ptr;
   char *host_name;
   char *directory_name;
   char *path;
   Tt_status tt_status;

   if (strncmp (new_name, "/", 1) == 0)
   {
      if ((path = (char *) ResolveLocalPathName (host_in,
                                                 new_name,
                                                 NULL,
                                                 home_host_name,
                                                 &tt_status )) == NULL)
         return (False);
   }
   else
   {
      if (strcmp(dir_in, "/") == 0)
         sprintf(to, "%s%s", dir_in, new_name);
      else
         sprintf(to, "%s/%s", dir_in, new_name);

      if ((path = (char *) ResolveLocalPathName (host_in,
                                                 to,
                                                 NULL,
                                                 home_host_name,
                                                 &tt_status )) == NULL)
         return (False);
   }

   strcpy (to, path);

   XtFree(path);
   return (True);
}



/************************************************************************
 *
 *  FileTypePreviewCB
 *	Get the text field file name, find out what the file type is
 *	for the file name and display the name and icon for the
 *	file type.
 *
 ************************************************************************/

static void
FileTypePreviewCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   DialogCallbackStruct * call_struct;
   char * new_name;

   call_struct = (DialogCallbackStruct *) client_data;

   new_name = XmTextFieldGetString (call_struct->file_text_widget);
   FileTypePreview(w, new_name, client_data, call_data);
}


/************************************************************************
 *
 *  FileTypePreview
 *	Find out what the file type is for the specified file name and
 *      display the name and icon for the file type.
 *
 ************************************************************************/

static void
FileTypePreview(
        Widget w,
        String new_name,
        XtPointer client_data,
        XtPointer call_data )
{
   DialogCallbackStruct * call_struct;
   char * file_type;
   XmString string;
   Arg args[2];
   int n;
   char whole_name[MAX_PATH];
   char * orig_name;
   char * link_path;
   char * new_link_path;
   PixmapData *pixmapData;
   Tt_status tt_status;
   Boolean Flag =  ((Boolean)(XtArgVal)call_data == True)?True:False;

   call_struct = (DialogCallbackStruct *) client_data;
   new_name = (char *) _DtStripSpaces (new_name);

   if (new_name[0] == '~')
      new_name = _DtChangeTildeToHome (new_name);

   if (GetToPath (whole_name, new_name, call_struct->host_name,
                                 call_struct->directory_name) == False)
      strcpy(whole_name, " ");

   orig_name = ResolveLocalPathName( call_struct->host_name,
                                     call_struct->directory_name,
                                     call_struct->file_name,
                                     home_host_name, &tt_status);

   /* Check for broken links so we get the proper file_type */
   if (call_struct->original_logical_type != NULL &&
       strcmp(call_struct->original_logical_type, LT_BROKEN_LINK) == 0)
      file_type = XtNewString(call_struct->original_logical_type);
   else if (call_struct->original_physical_type == DtDIRECTORY)
      file_type = XtNewString(LT_DIRECTORY);
   else if ( *new_name == 0x0 &&
             call_struct->original_physical_type == DtDATA )
      file_type = XtNewString(LT_DATA);
   else if (link_path = _DtFollowLink(whole_name))
   {
      if (strcmp(orig_name, whole_name) == 0)
        file_type = (char *) DtDtsDataToDataType(orig_name, NULL, 0, NULL,
                                                 NULL, NULL, NULL);
      else
      {
         if (call_struct->menu_widget == *renameBtn)
         {
            new_link_path = XtNewString(link_path);

            if ((link_path = _DtFollowLink(orig_name)) &&
                (strcmp(link_path, orig_name) == 0))
            {
               file_type = (char *) DtDtsDataToDataType(new_link_path, NULL, 0,
                                                   NULL, NULL, NULL, NULL);
            }
            else
            {
               /* Links are typed according to what they are linked to */
               file_type = call_struct->original_logical_type;
            }

            XtFree(new_link_path);
         }
         else
         {
           /* We want to find out the possible filetype of a file that has
              such name (stored in link_path). However, the file does not even
              existed yet. In order for the file typing code to work correctly,
              we have to pass in a dummy stat structure indicates that if the
              file is created, this is its mode.
           */
           struct stat fileStat;
           unsigned int creationMask = umask( DtFILE_DATA_CREATION_MASK );

           memset( &fileStat, 0, sizeof( struct stat ) );
           fileStat.st_mode = creationMask | S_IFREG;

           file_type = (char *) DtDtsDataToDataType( link_path, NULL, 0,
                                                     &fileStat,
                                                     NULL, NULL, NULL);
           umask( creationMask );
         }
      }
   }
   else
      file_type = XtNewString(LT_RECURSIVE_LINK);


   n = 0;
   string = NULL;

   if (file_type)
   {
      if( call_struct->previous_file_type
          && strcmp( call_struct->previous_file_type, file_type ) == 0 )
      {
        char *tmpStr, *title, *msg;

        if(Flag == False)
        {
          tmpStr = GETMESSAGE(10, 36, "Show Icon");
          title = XtNewString(tmpStr);
          tmpStr = GETMESSAGE(10, 41, "The correct icon for this datatype is already shown.");
          msg = XtNewString(tmpStr);

          _DtMessage(XtParent(w), title, msg, NULL, HelpRequestCB);
          XtFree(title);
          XtFree(msg);

          XtFree ((char *) file_type);
          XtFree ((char *) new_name);
          XtFree ((char *) orig_name);
        }
        return;
      }

      call_struct->previous_file_type = file_type;

      if (DtActionExists(file_type))
      {
         char *str = (char *)DtActionLabel(file_type);
         if(str != NULL)
             string = XmStringCreateLocalized(str);
         else
             string = XmStringCreateLocalized(file_type);
         XtFree(str);
      }
      else
      {
         char *str = (char *)DtDtsDataTypeToAttributeValue(file_type,
                                                           FM_TYPE_LABEL,
                                                           NULL);
         if (str)
         {
            string = XmStringCreateLocalized(str);
            DtDtsFreeAttributeValue(str);
         }
         else
         {
            string = XmStringCreateLocalized(file_type);
         }
      }

      XtSetArg (args[n], XmNstring, string);		n++;

      pixmapData = _DtRetrievePixmapData(file_type,
                                         call_struct->file_name,
                                         call_struct->directory_name,
                                         toplevel,
                                         LARGE);
      if (pixmapData)
        XtSetArg (args[n], XmNimageName, pixmapData->iconFileName);
      else
        XtSetArg (args[n], XmNimageName, NULL);                 n++;
      XtSetValues (call_struct->file_type_widget, args, n);

      _DtCheckAndFreePixmapData(file_type,
                                toplevel,
                                (DtIconGadget) (call_struct->file_type_widget),
                                pixmapData);
   }
   else
   {
      XtSetArg (args[n], XmNimageName, NULL);           n++;
      XtSetValues (call_struct->file_type_widget, args, n);
      if (call_struct->previous_file_type)
      {
        XtFree((char *)call_struct->previous_file_type );
        call_struct->previous_file_type = NULL;
      }
   }

   if (string != NULL)
      XmStringFree (string);

   XtFree ((char *) new_name);
   XtFree ((char *) orig_name);
}


/************************************************************************
 *
 *  MakeFileDialog
 *      Callback function invoked from the File Manager menu.  This
 *      function creates and displays a dialog prompting for a file name
 *      to be used to create a directory, executable, or normal file.
 *
 ************************************************************************/

void
ShowMakeFileDialog(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileMgrRec * file_mgr_rec;
   DialogData * dialog_data;
   FileMgrData * file_mgr_data;

   Widget shell;
   Widget form;
   Widget new_name;
   Widget new_text;
   Widget file_type_box;
   Widget file_type;
   Widget separator;
   Widget ok;
   Widget apply;
   Widget cancel;
   Widget preview;
   Widget help;
   int flag=FLAG_RESET;

   DialogCallbackStruct * call_struct;
   char * tmpStr, *tempStr;
   XmString label_string;

   Arg args[20];
   int n;
   Widget mbar = XmGetPostedFromWidget(XtParent(w));
   Atom delete_window_atom;



   /*  Set the menu item to insensitive to prevent multiple  */
   /*  dialogs from being posted and get the area under the  */
   /*  menu pane redrawn.                                    */

   XmUpdateDisplay (w);

   XtSetArg(args[0], XmNuserData, &file_mgr_rec);
   XtGetValues(mbar, args, 1);

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;


   n = 0;
   XtSetArg (args[n], XmNallowShellResize, True);  n++;
   /*  Adjust the decorations for the dialog shell of the dialog  */
   XtSetArg (args[n], XmNmwmFunctions, MWM_FUNC_MOVE | MWM_FUNC_CLOSE);         ++n;
   XtSetArg (args[n], XmNmwmDecorations, MWM_DECOR_BORDER | MWM_DECOR_TITLE);   ++n;

   if (w == *create_directoryBtn)
   {
      /* Ignore accelerators when we're insensitive */
      if (file_mgr_rec->create_directoryBtn_child)
      {
         XSetInputFocus(XtDisplay(w),
			XtWindow(file_mgr_rec->create_directoryBtn_child),
			RevertToParent, CurrentTime);
         return;
      }

      file_mgr_rec->menuStates &= ~CREATE_DIR;

      if(file_mgr_data->title != NULL &&
               strcmp(file_mgr_data->helpVol, DTFILE_HELP_NAME) != 0)
      {
         tmpStr = GETMESSAGE(10, 14, "New Folder");
         tempStr = (char *)XtMalloc(strlen(tmpStr) +
                                 strlen(file_mgr_data->title) + 5);
         sprintf(tempStr, "%s - %s", file_mgr_data->title, tmpStr);
      }
      else
      {
         tmpStr = (GETMESSAGE(10,28, "File Manager - New Folder"));
         tempStr = XtNewString(tmpStr);
      }

      XtSetArg (args[n], XmNtitle, tempStr);      n++;
      shell =XmCreateDialogShell(file_mgr_rec->shell, "new_folder", args, n);
      label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 16, "New Folder Name: ")));
   }
   else if (w == *create_dataBtn)
   {
      /* Ignore accelerators when we're insensitive */
      if ( file_mgr_rec->create_dataBtn_child )
      {
         XSetInputFocus(XtDisplay(w),
                        XtWindow(file_mgr_rec->create_dataBtn_child),
                        RevertToParent, CurrentTime);
         return;
      }

      file_mgr_rec->menuStates &= ~CREATE_FILE;

      if(file_mgr_data->title != NULL &&
               strcmp(file_mgr_data->helpVol, DTFILE_HELP_NAME) != 0)
      {
         tmpStr = (GETMESSAGE(10,26, "New File"));
         tempStr = (char *)XtMalloc(strlen(tmpStr) +
                                 strlen(file_mgr_data->title) + 5);
         sprintf(tempStr, "%s - %s", file_mgr_data->title, tmpStr);
      }
      else
      {
         tmpStr = (GETMESSAGE(10,29, "File Manager - New File"));
         tempStr = XtNewString(tmpStr);
      }
      XtSetArg (args[n], XmNtitle, tempStr);      n++;
      shell =XmCreateDialogShell(file_mgr_rec->shell, "new_data_file", args, n);
      label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 7, "New File Name: ")));
   }
   XtFree(tempStr);

   /* Set the useAsyncGeo on the shell */
   XtSetArg (args[0], XmNuseAsyncGeometry, True);
   XtSetValues (XtParent(shell), args, 1);

   /*  Create the widget hierarchy for the dialog  */

   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);               n++;
   XtSetArg (args[n], XmNmarginHeight, 1);              n++;
   XtSetArg (args[n], XmNshadowThickness, 1);           n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);     n++;
   XtSetArg (args[n], XmNautoUnmanage, False);          n++;
   XtSetArg (args[n], XmNuserData, file_mgr_rec);       n++;
   form = XmCreateForm (shell, "form", args, n);


   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 15);                     n++;
/*
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNleftOffset, 5);                        n++;
*/
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
   XtSetArg (args[n], XmNtopOffset, 30);                        n++;
   XtSetArg (args[n], XmNtraversalOn, False);			n++;
   new_name = XmCreateLabelGadget (form, "new_name", args, n);
   XtManageChild (new_name);

   XmStringFree (label_string);


   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);      n++;
   XtSetArg (args[n], XmNleftWidget, new_name);                 n++;
   XtSetArg (args[n], XmNleftOffset, 0);                        n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);         n++;
   XtSetArg (args[n], XmNtopOffset, 30);                        n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 70);                    n++;
   XtSetArg (args[n], XmNtraversalOn, True);                    n++;
   new_text = XmCreateTextField (form, "new_text", args, n);

   XtManageChild (new_text);

   /* Create the field displaying the file type info */

   n = 0;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);          n++;
   XtSetArg(args[n], XmNtopOffset, 5);                          n++;
/*
   XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg(args[n], XmNleftWidget, new_text);                  n++;
   XtSetArg(args[n], XmNleftOffset, 20);                        n++;
*/
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 75);                     n++;
/*
   XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);        n++;
   XtSetArg(args[n], XmNrightOffset, 5);                        n++;
*/
   XtSetArg(args[n], XmNmarginHeight, 5);                       n++;
   XtSetArg(args[n], XmNmarginWidth, 5);                        n++;
   file_type_box = XmCreateFrame (form, "file_type_box", args, n);
   XtManageChild (file_type_box);

   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 8, "File Type")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg(args[n], XmNshadowThickness, 0);                    n++;
   XtSetArg(args[n], XmNshadowType, XmSHADOW_ETCHED_OUT);       n++;
   XtSetArg(args[n], XmNpixmapPosition, XmPIXMAP_TOP);          n++;
   XtSetArg (args[n], XmNtraversalOn, False);                   n++;
   file_type = _DtCreateIcon (file_type_box, "file_type", args, n);
   XtManageChild (file_type);

   XmStringFree (label_string);


   /* Adjust the colors of the icon */
   SetToNormalColors (file_type, form, file_mgr_rec->main, 0);

   /*  The data sent to the Text, Ok and Cancel callbacks  */

   call_struct =
      (DialogCallbackStruct *) XtMalloc (sizeof (DialogCallbackStruct));

   if(w == *create_directoryBtn)
   {
     file_mgr_rec->create_directoryBtn_child = form;
     call_struct->original_physical_type = DtDIRECTORY;
   }
   if(w == *create_dataBtn)
   {
     file_mgr_rec->create_dataBtn_child = form;
     call_struct->original_physical_type = DtDATA;
   }

   call_struct->dialog_widget = form;
   call_struct->menu_widget = w;
   call_struct->dir_text_widget = NULL;
   call_struct->file_text_widget = new_text;
   call_struct->file_type_widget = file_type;
   call_struct->file_mgr_rec = file_mgr_rec;
   call_struct->host_name = XtNewString (file_mgr_data->host);
   call_struct->directory_name =  XtNewString(file_mgr_data->current_directory);
   call_struct->file_name = NULL;
   call_struct->original_logical_type = NULL;
   call_struct->previous_file_type = NULL;
   call_struct->file_mgr_data = file_mgr_data;


   /*  Create a separator between the buttons  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);       n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, file_type_box);             n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   separator =  XmCreateSeparatorGadget (form, "separator", args, n);
   XtManageChild (separator);



   /*  Create the ok, apply, cancel, and help buttons  */

   n = 0;
   XtSetArg (args[n], XmNlabelString, okXmString);              n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 1);                      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 19);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                      n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   XtSetArg (args[n], XmNshowAsDefault, True);                  n++;
   XtSetArg (args[n], XmNuserData, flag);                        n++;
   ok = XmCreatePushButtonGadget (form, "ok", args, n);
   XtManageChild (ok);

   XtAddCallback (ok, XmNactivateCallback, MakeFileOk, (XtPointer) call_struct);

   /*  Set the ok button to the default for the bulletin board.  */
   /*  This causes the return key from the text widget to be     */
   /*  properly processed.                                       */

   XtSetArg (args[0], XmNdefaultButton, ok);
   XtSetValues (form, args, 1);

   n = 0;
   XtSetArg (args[n], XmNlabelString, applyXmString);           n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 20);                      n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 39);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                      n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   apply = XmCreatePushButtonGadget (form, "apply", args, n);
   XtManageChild (apply);

   XtAddCallback (apply, XmNactivateCallback, MakeFileApply, (XtPointer) call_struct);

   n = 0;
   XtSetArg (args[n], XmNlabelString, cancelXmString);          n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 40);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 59);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                      n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   XtSetArg (args[n], XmNuserData, flag);                       n++;
   cancel = XmCreatePushButtonGadget (form, "cancel", args, n);
   XtManageChild (cancel);

   XtAddCallback (cancel, XmNactivateCallback, DialogCancel,
                  (XtPointer) call_struct);


   label_string = XmStringCreateLocalized (((char *)GETMESSAGE(10, 36, "Show Icon")));

   n = 0;
   XtSetArg (args[n], XmNlabelString, label_string);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 60);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 79);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                      n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   preview = XmCreatePushButtonGadget (form, "show_file_type", args, n);
   XtManageChild (preview);

   XmStringFree (label_string);
   XtAddCallback (preview, XmNactivateCallback, FileTypePreviewCB,
                  (XtPointer) call_struct);

   FileTypePreviewCB (preview, (XtPointer) call_struct, NULL);

   n = 0;
   XtSetArg (args[n], XmNlabelString, helpXmString);            n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);    n++;
   XtSetArg (args[n], XmNleftPosition, 80);                     n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);   n++;
   XtSetArg (args[n], XmNrightPosition, 99);                    n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);       n++;
   XtSetArg (args[n], XmNtopWidget, separator);                 n++;
   XtSetArg (args[n], XmNtopOffset, 5);                         n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);      n++;
   XtSetArg (args[n], XmNbottomOffset, 5);                      n++;
   XtSetArg (args[n], XmNmarginWidth, 4);                       n++;
   XtSetArg (args[n], XmNmarginHeight, 4);                      n++;
   help = XmCreatePushButtonGadget (form, "help", args, n);
   XtManageChild (help);

   if (w == *create_directoryBtn)
   {
      XtAddCallback(form, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
      XtAddCallback(new_name, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
      XtAddCallback(new_text, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
      XtAddCallback(help, XmNactivateCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
      XtAddCallback(file_type_box,XmNhelpCallback,(XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
      XtAddCallback(file_type, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
      XtAddCallback(ok, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
      XtAddCallback(apply, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
      XtAddCallback(cancel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
      XtAddCallback(preview, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
      XtAddCallback(help, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_DIR_DIALOG_STR);
   }
   else
   {
      XtAddCallback(form, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
      XtAddCallback(new_name, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
      XtAddCallback(new_text, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
      XtAddCallback(help, XmNactivateCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
      XtAddCallback(file_type_box,XmNhelpCallback,(XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
      XtAddCallback(file_type, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
      XtAddCallback(ok, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
      XtAddCallback(apply, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
      XtAddCallback(cancel, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
      XtAddCallback(preview, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
      XtAddCallback(help, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                    HELP_CREATE_FILE_DIALOG_STR);
   }


   XtSetArg (args[0], XmNcancelButton, cancel);
   XtSetValues (form, args, 1);

   XtAddCallback (shell, XmNpopupCallback, (XtCallbackProc)_DtmapCB,
                  (XtPointer) XtParent(shell));

   delete_window_atom = XmInternAtom (XtDisplay(shell), "WM_DELETE_WINDOW", True);
   XmRemoveWMProtocols( shell, &delete_window_atom, 1 );
   XmAddWMProtocolCallback( shell, delete_window_atom, DialogCancel,
                            (XtPointer) call_struct );

   call_struct->Ok     = ok;
   call_struct->Cancel = cancel;

   XtManageChild (form);

   /* Force the focus to the text field */
   XmProcessTraversal(new_text, XmTRAVERSE_CURRENT);

}


/************************************************************************
 *
 *  MakeFileOk
 *      Get the new file name and create the file.
 *
 ************************************************************************/

static void
MakeFileOk(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   DialogCallbackStruct * call_struct = (DialogCallbackStruct *) client_data;
   char * new_name;
   unsigned char type;
   char * title;
   char * msg;
   int length;
   int dirNameLength = strlen (call_struct->directory_name);
   int maxFileNameLength = pathconf (call_struct->directory_name, _PC_NAME_MAX);
   unsigned int btnMask;
   MakeFileDoneData *data;

   if(RecheckFlag(call_struct->dialog_widget,w))
     return;
   else if(RecheckFlag(NULL,call_struct->Cancel))
     return;

   /* see if this is a create file or directory */
   if (call_struct->menu_widget == *create_directoryBtn)
   {
      call_struct->file_mgr_rec->create_directoryBtn_child = NULL;
      type = DtDIRECTORY;
      btnMask = CREATE_DIR;
   }
   else
   {
      call_struct->file_mgr_rec->create_dataBtn_child = NULL;
      type = DtDATA;
      btnMask = CREATE_FILE;
   }

   /* get the new name */
   new_name = XmTextFieldGetString (call_struct->file_text_widget);
   new_name = (char *) _DtStripSpaces (new_name);


   if (new_name[0] == '~')
      new_name = _DtChangeTildeToHome (new_name);

   length = strlen (new_name);
   msg = NULL;
   if (length == 0)
   {
      msg = XtNewString(GETMESSAGE(10, 22,
                      "Cannot create a file or folder with a blank name."));
   }
   /* Ensure the new name has length less than or equal to the maximum
      length that the system allows.
      If maxFileNameLength == -1 the file system is not supporting POSIX, use MAXNAMLEN
   */
   else if( maxFileNameLength < -1  || ( maxFileNameLength == -1 && ( length > MAXNAMLEN  || length + dirNameLength > MAX_PATH ) ) || ( maxFileNameLength > 0 && length > maxFileNameLength ) )
   {
     if (type == DtDIRECTORY)
     {
       title = XtNewString(GETMESSAGE(10, 20, "Create Folder Error"));
       msg = XtNewString(GETMESSAGE(10, 23, "Invalid folder name."));
     }
     else
     {
       title = XtNewString(GETMESSAGE(10, 21, "Create File Error"));
       msg = XtNewString(GETMESSAGE(10, 24, "Invalid file name."));
     }
   }
#ifdef _CHECK_FOR_SPACES
   else if (DtStrchr (new_name, ' ') != NULL ||
            DtStrchr (new_name, '\t') != NULL)
   {
      msg = XtNewString(GetSharedMessage(NO_SPACES_ALLOWED_ERROR));
   }
#endif

   if (msg != NULL)
   {
      if (type == DtDIRECTORY)
         title = XtNewString(GETMESSAGE(10, 20, "Create Folder Error"));
      else
         title = XtNewString(GETMESSAGE(10, 21, "Create File Error"));
      _DtMessage (XtParent (w), title, msg, NULL, HelpRequestCB);
      XtFree(title);
      XtFree(msg);
      XtFree(new_name);
      ResetFlag(call_struct->dialog_widget,w);
      ResetFlag(call_struct->dialog_widget,call_struct->Cancel);
      return;
   }

   /* set up data for MakeFileDone callback */
   data = (MakeFileDoneData *)XtMalloc(sizeof(MakeFileDoneData));
   data->w = w;
   data->call_struct = call_struct;
   data->type = type;
   data->btnMask = btnMask;
   data->new_name = new_name;

   /*
    * Create the file/directory.
    * This operation is performed in a background process; when the
    * background process finishes, the MakeFileDone callback will be
    * called with a return code indicating success or failure.
    */

   MakeFile (w, call_struct->host_name, call_struct->directory_name,
             new_name, type, MakeFileDone, (XtPointer)data);
}


/************************************************************************
 *
 *  MakeFileDone
 *      This routine is called when the file creation is finished.
 *
 ************************************************************************/

static void
MakeFileDone(
        XtPointer client_data,
        char *to,
        int rc)
{
   MakeFileDoneData *data = (MakeFileDoneData *)client_data;
   DialogCallbackStruct *call_struct = data->call_struct;
   FileMgrRec *file_mgr_rec = (FileMgrRec *) call_struct->file_mgr_rec;

   char *title;
   char *msg;
   FileViewData *file_view_data;
   FileMgrData *file_mgr_data;
   DialogData *dialog_data;
   DirectorySet *directory_set;
   int i;

   /*
    * If the file/directory create was successful, unmanage the dialog
    * and free up any allocated data space.  Otherwise, just return
    * to allow the user to try a different name or cancel the dialog.
    */

   if (rc == EINVAL)
   {
      /* invalid name */
      if (data->type == DtDIRECTORY)
      {
         title = XtNewString(GETMESSAGE(10, 20, "Create Folder Error"));
         msg = XtNewString(GETMESSAGE(10, 23, "Invalid folder name."));
      }
      else
      {
         title = XtNewString(GETMESSAGE(10, 21, "Create File Error"));
         msg = XtNewString(GETMESSAGE(10, 24, "Invalid file name."));
      }
      _DtMessage (XtParent(data->w), title, msg, NULL, HelpRequestCB);
      ResetFlag(call_struct->dialog_widget,call_struct->Ok);
      ResetFlag(call_struct->dialog_widget,call_struct->Cancel);
      XtFree(title);
      XtFree(msg);
   }
   else if (rc != 0)
   {
      /* some other error */
      if (rc == EEXIST)
         msg = XtNewString(GetSharedMessage(CANT_OVERWRITE_ERROR));
      else
         msg = XtNewString(GetSharedMessage(CANT_CREATE_ERROR));
      FileOperationError (XtParent(data->w), msg, to);
      ResetFlag(call_struct->dialog_widget,call_struct->Ok);
      ResetFlag(call_struct->dialog_widget,call_struct->Cancel);
      XtFree(msg);
   }
   else
   {
      /* create was successful: close the dialog */
      XtUnmanageChild (call_struct->dialog_widget);
      XtDestroyWidget (XtParent (call_struct->dialog_widget));
      file_mgr_rec->menuStates |= data->btnMask;
   }

   if (rc == 0)
   {
      /* free the call data */
      XtFree(call_struct->host_name);
      XtFree(call_struct->directory_name);
      if (call_struct->previous_file_type)
        XtFree((char *) call_struct->previous_file_type);
      XtFree((char *) call_struct);

#ifdef FOO
      /* @@@
       * The code below wants to make the file that was just created
       * be selected and scroll the view so the new file is visible.
       * But: since we made directory updates asynchronous, this code
       * won't work anymore, because at this point, we won't yet be able
       * to find the new file in the cached directory.
       *
       * Possible solutions:
       * Somewhere (e.g., in the file_mgr_data structure) log our
       * intention to select the new file; when it finally shows up,
       * select it them.
       */
      dialog_data = _DtGetInstanceData (file_mgr_rec);
      file_mgr_data = (FileMgrData *) dialog_data->data;
      for (i = 0; i < file_mgr_data->directory_count; i++)
      {
         directory_set = (DirectorySet *)file_mgr_data->directory_set[i];
         if (strcmp(directory_set->name, call_struct->directory_name) == 0)
            break;
      }

      file_view_data = NULL;
      for (i = 0; i < directory_set->file_count; i++)
      {
         if (strcmp(directory_set->order_list[i]->file_data->file_name,
                                                          data->new_name) == 0)
         {
            file_view_data = directory_set->order_list[i];
            break;
         }
      }

      if (file_view_data != NULL)
      {
         if (file_view_data->widget != NULL)
         {
            DeselectAllFiles(file_mgr_data);
            SelectFile (file_mgr_data, file_view_data);
            PositionFileView(file_view_data, file_mgr_data);
            ActivateSingleSelect ((FileMgrRec *) file_mgr_data->file_mgr_rec,
                    file_mgr_data->selection_list[0]->file_data->logical_type);
         }
      }
#endif /* FOO */
   }

   XtFree(data->new_name);
   XtFree((char *)data);
}



/************************************************************************
 *
 *  MakeFileApply
 *      Get the new file name and create the file.
 *
 ************************************************************************/

static void
MakeFileApply(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   DialogCallbackStruct * call_struct = (DialogCallbackStruct *) client_data;
   char * new_name;
   unsigned char type;
   char * title;
   char * msg;
   int length;
   int dirNameLength = strlen (call_struct->directory_name);
   int maxFileNameLength = pathconf (call_struct->directory_name, _PC_NAME_MAX);
   unsigned int btnMask;
   MakeFileDoneData *data;

   /* see if this is a create file or directory */
   if (call_struct->menu_widget == *create_directoryBtn)
   {
      call_struct->file_mgr_rec->create_directoryBtn_child = NULL;
      type = DtDIRECTORY;
      btnMask = CREATE_DIR;
   }
   else
   {
      call_struct->file_mgr_rec->create_dataBtn_child = NULL;
      type = DtDATA;
      btnMask = CREATE_FILE;
   }

   /* get the new name */
   new_name = XmTextFieldGetString (call_struct->file_text_widget);
   new_name = (char *) _DtStripSpaces (new_name);


   if (new_name[0] == '~')
      new_name = _DtChangeTildeToHome (new_name);

   length = strlen (new_name);
   msg = NULL;
   if (length == 0)
   {
      msg = XtNewString(GETMESSAGE(10, 22,
                      "Cannot create a file or folder with a blank name."));
   }
   /* Ensure the new name has length less than or equal to the maximum
      length that the system allows.
      If maxFileNameLength == -1 the file system is not supporting POSIX, use MAXNAMLEN
   */
   else if( maxFileNameLength < -1  || ( maxFileNameLength == -1 && ( length > MAXNAMLEN  || length + dirNameLength > MAX_PATH ) ) || ( maxFileNameLength > 0 && length > maxFileNameLength ) )
   {
     if (type == DtDIRECTORY)
     {
       title = XtNewString(GETMESSAGE(10, 20, "Create Folder Error"));
       msg = XtNewString(GETMESSAGE(10, 23, "Invalid folder name."));
     }
     else
     {
       title = XtNewString(GETMESSAGE(10, 21, "Create File Error"));
       msg = XtNewString(GETMESSAGE(10, 24, "Invalid file name."));
     }
   }
#ifdef _CHECK_FOR_SPACES
   else if (DtStrchr (new_name, ' ') != NULL ||
            DtStrchr (new_name, '\t') != NULL)
   {
      msg = XtNewString(GetSharedMessage(NO_SPACES_ALLOWED_ERROR));
   }
#endif

   if (msg != NULL)
   {
      if (type == DtDIRECTORY)
         title = XtNewString(GETMESSAGE(10, 20, "Create Folder Error"));
      else
         title = XtNewString(GETMESSAGE(10, 21, "Create File Error"));
      _DtMessage (XtParent (w), title, msg, NULL, HelpRequestCB);
      XtFree(title);
      XtFree(msg);
      XtFree(new_name);
      return;
   }

   /* set up data for MakeFileDone callback */
   data = (MakeFileDoneData *)XtMalloc(sizeof(MakeFileDoneData));
   data->w = w;
   data->call_struct = call_struct;
   data->type = type;
   data->btnMask = btnMask;
   data->new_name = new_name;

   /*
    * Create the file/directory.
    * This operation is performed in a background process; when the
    * background process finishes, the MakeFileDone callback will be
    * called with a return code indicating success or failure.
    */

   MakeFile (w, call_struct->host_name, call_struct->directory_name,
             new_name, type, ApplyFileDone, (XtPointer)data);
   FileTypePreviewCB (w, client_data, (XtPointer) True);
}


/************************************************************************
 *
 *  MakeFileDone
 *      This routine is called when the file creation is finished.
 *
 ************************************************************************/

static void
ApplyFileDone(
        XtPointer client_data,
        char *to,
        int rc)
{
   MakeFileDoneData *data = (MakeFileDoneData *)client_data;
   DialogCallbackStruct *call_struct = data->call_struct;
   FileMgrRec *file_mgr_rec = (FileMgrRec *) call_struct->file_mgr_rec;

   char *title;
   char *msg;
   FileViewData *file_view_data;
   FileMgrData *file_mgr_data;
   DialogData *dialog_data;
   DirectorySet *directory_set;
   int i;

   /*
    * If the file/directory create was successful, unmanage the dialog
    * and free up any allocated data space.  Otherwise, just return
    * to allow the user to try a different name or cancel the dialog.
    */

   if (rc == EINVAL)
   {
      /* invalid name */
      if (data->type == DtDIRECTORY)
      {
         title = XtNewString(GETMESSAGE(10, 20, "Create Folder Error"));
         msg = XtNewString(GETMESSAGE(10, 23, "Invalid folder name."));
      }
      else
      {
         title = XtNewString(GETMESSAGE(10, 21, "Create File Error"));
         msg = XtNewString(GETMESSAGE(10, 24, "Invalid file name."));
      }
      _DtMessage (XtParent(data->w), title, msg, NULL, HelpRequestCB);
      XtFree(title);
      XtFree(msg);
   }
   else if (rc != 0)
   {
      /* some other error */
      if (rc == EEXIST)
         msg = XtNewString(GetSharedMessage(CANT_OVERWRITE_ERROR));
      else
         msg = XtNewString(GetSharedMessage(CANT_CREATE_ERROR));
      FileOperationError (XtParent(data->w), msg, to);
      XtFree(msg);
   }
}



/************************************************************************
 *
 *  DialogCancel
 *	Unmanage and destroy the dialog
 *
 ************************************************************************/

static void
DialogCancel(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   DialogCallbackStruct * call_struct = (DialogCallbackStruct *) client_data;
   DialogData *dialog_data;
   FileMgrData *file_mgr_data;
   unsigned int btnMask = 0;

   if(RecheckFlag(NULL,w))
     return;

   dialog_data = _DtGetInstanceData ((XtPointer)call_struct->file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   XtUnmanageChild (call_struct->dialog_widget);
   XtDestroyWidget (XtParent (call_struct->dialog_widget));

   if (file_mgr_data->selected_file_count == 1 )
   {
     /* If the selection count is 1 then
        resensitize the appropriate menu button.
        Otherwise, ignore it.
     */
     if (call_struct->menu_widget == *renameBtn)
       btnMask = RENAME;
     else if (call_struct->menu_widget == *moveBtn)
       btnMask = MOVE;
     else if (call_struct->menu_widget == *duplicateBtn)
       btnMask = DUPLICATE;
     else if (call_struct->menu_widget == *linkBtn)
       btnMask = LINK;
   }

   if (call_struct->menu_widget == *create_directoryBtn)
   {
     btnMask = CREATE_DIR;
     call_struct->file_mgr_rec->create_directoryBtn_child = NULL;
   }
   else if (call_struct->menu_widget == *create_dataBtn)
   {
     btnMask = CREATE_FILE;
     call_struct->file_mgr_rec->create_dataBtn_child = NULL;
   }

   call_struct->file_mgr_rec->menuStates |= btnMask;

   /* Clean up
   */
   if (call_struct->host_name != NULL)
      XtFree ((char *) call_struct->host_name);

   if (call_struct->directory_name != NULL)
      XtFree ((char *) call_struct->directory_name);

   if (call_struct->file_name != NULL)
      XtFree ((char *) call_struct->file_name);

   if (call_struct->previous_file_type)
      XtFree ((char *) call_struct->previous_file_type);

   XtFree ((char *) call_struct);
}

int
RecheckFlag(
  Widget parent,
  Widget w)
{
   Arg args[2];
   XtArgVal flag;

   XtSetArg (args[0], XmNuserData, &flag);
   XtGetValues(w,args,1);
   if(flag == FLAG_SET)
      return True;
   else if(flag != FLAG_RESET)
      return False;
   flag = FLAG_SET;
   XtSetArg (args[0], XmNuserData, flag);
   XtSetValues(w,args,1);
   if(parent != NULL)
    _DtTurnOnHourGlass(parent);
   return False;
}

void
ResetFlag(
   Widget parent,
   Widget w)
{
   Arg args[2];
   int flag;
   XtSetArg (args[0], XmNuserData, &flag);
   XtGetValues(w,args,1);
   if(flag != FLAG_SET)
      return;
   flag = FLAG_RESET;
   XtSetArg (args[0], XmNuserData, flag);
   XtSetValues(w,args,1);
   if(parent != NULL)
    _DtTurnOffHourGlass(parent);
}
