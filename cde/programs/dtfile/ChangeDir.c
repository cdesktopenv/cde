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
/* $XConsortium: ChangeDir.c /main/7 1996/10/30 11:09:35 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           ChangeDir.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   DESCRIPTION:    Source file for the change directory dialog.
 *
 *   FUNCTIONS: ChangeToNewDir
 *		CheckRestrictedDir
 *		Create
 *		Destroy
 *		DropOnChangeView
 *		FreeValues
 *		GetDefaultValues
 *		GetResourceValues
 *		GetValues
 *		GoToNewDir
 *		InstallChange
 *		InstallClose
 *		OkCallback
 *		SetFocus
 *		SetValues
 *		TextChange
 *		TryToChangeDir
 *		WriteResourceValues
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __hpux
#include <sys/inode.h>
#endif

#include <ctype.h>
#include <pwd.h>

#include <Xm/XmP.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>
#include <Xm/VendorSEP.h>
#include <Xm/List.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>

#include <X11/ShellP.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>

#include <Dt/Action.h>
#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/SharedProcs.h>

#include <Tt/tttk.h>

#include <Xm/DragIcon.h>
#include <Xm/DragC.h>
#include <Xm/TextF.h>
#include <Xm/XmPrivate.h> /* _XmStringUngenerate */
#include <Dt/Dnd.h>

#include "Encaps.h"
#include "SharedProcs.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "ChangeDir.h"
#include "Help.h"
#include "SharedMsgs.h"


#define MAX_PATH 1024

static char * CHANGEDIRECTORY = "ChangeDirectory";


static DialogResource resources[] =
{
   { "string_path", XmRXmString, sizeof(XmString),
     XtOffset(ChangeDirDataPtr, string_path), NULL, _DtXmStringToString },

   { "historyList", XmRXmStringTable, sizeof(XmStringTable),
      XtOffset(ChangeDirDataPtr, history_list), NULL, _DtXmStringTableToString },

   { "visibleCount", XmRInt, sizeof(int),
      XtOffset(ChangeDirDataPtr, visible_count), (XtPointer) 8, _DtIntToString },

   { "listCount", XmRInt, sizeof(int),
      XtOffset(ChangeDirDataPtr, list_count), NULL, _DtIntToString },
};


/********    Static Function Declarations    ********/

static void Create(     Display *display,
                        Widget parent,
                        Widget *shell,
                        XtPointer *dialog) ;
static void InstallChange( ChangeDirRec *change_dir_rec,
                           XtCallbackProc callback,
                           XtPointer client_data) ;
static void InstallClose( ChangeDirRec *change_dir_rec,
                          XtCallbackProc callback,
                          XtPointer client_data) ;
static void Destroy( ChangeDirRec *change_dir_rec) ;
static XtPointer GetValues( ChangeDirRec *change_dir_rec) ;
static XtPointer GetDefaultValues( void ) ;
static XtPointer GetResourceValues( XrmDatabase data_base,
                                  char **name_list) ;
static void SetValues( ChangeDirRec *change_dir_rec,
                       ChangeDirData *change_dir_data) ;
static void WriteResourceValues( DialogData *values,
                                 int fd,
                                 char **name_list) ;
static void FreeValues( ChangeDirData *change_dir_data) ;
static Boolean TryToChangeDir( Boolean isFromDialog,
                               char * incoming_string,
                               FileMgrData * file_mgr_data,
                               FileMgrRec * file_mgr_rec,
                               ChangeDirData * change_dir_data,
                               Widget selection_box,
                               ChangeDirApply * apply_data,
                               XmSelectionBoxCallbackStruct * callback_data,
                               void (*callback)()) ;
static void OkCallback( Widget selection_box,
                        ChangeDirApply *apply_data,
                        XmSelectionBoxCallbackStruct *callback_data) ;
static int CheckRestrictedDir ( FileMgrRec * file_mgr_rec,
                                FileMgrData * file_mgr_data,
                                char ** value,
                                Boolean relative_name);
static void GoToNewDir ( FileMgrData * file_mgr_data,
                         char * dir_name) ;
static void SetFocus(
                        ChangeDirRec *change_dir_rec,
                        ChangeDirData *change_dir_data) ;


/********    End Static Function Declarations    ********/

extern char *getwd(char *);

/*
 *  The Dialog Class structure.
 */

static DialogClass changeDirClassRec =
{
   resources,
   XtNumber(resources),
   Create,
   (DialogInstallChangeProc) InstallChange,
   (DialogInstallCloseProc) InstallClose,
   (DialogDestroyProc) Destroy,
   (DialogGetValuesProc) GetValues,
   GetDefaultValues,
   GetResourceValues,
   (DialogSetValuesProc) SetValues,
   WriteResourceValues,
   (DialogFreeValuesProc) FreeValues,
   (DialogMapWindowProc) _DtGenericMapWindow,
   (DialogSetFocusProc) SetFocus,
};

DialogClass * changeDirClass = (DialogClass *) &changeDirClassRec;




/************************************************************************
 *
 *  Create
 *
 ************************************************************************/

static void
Create(
        Display *display,
        Widget parent,
        Widget *shell,
        XtPointer *dialog )
{
   ChangeDirRec * change_dir_rec;
   XmString prompt;
   Widget w;
   Arg args[8];
   int n;
   XtTranslations trans_table;
   Widget change_dir;


   /*  Allocate the change directory dialog instance record.  */

   change_dir_rec = (ChangeDirRec *) XtMalloc (sizeof (ChangeDirRec));

   prompt = XmStringCreateLocalized (((char *)GETMESSAGE(2,14, "Destination Folder:")));

   trans_table = XtParseTranslationTable(translations_space);
   n = 0;
   XtSetArg (args[n], XmNselectionLabelString, prompt);		n++;
   XtSetArg (args[n], XmNokLabelString, okXmString);		n++;
   XtSetArg (args[n], XmNcancelLabelString, cancelXmString);	n++;
   XtSetArg (args[n], XmNhelpLabelString, helpXmString);	n++;
   XtSetArg (args[n], XmNautoUnmanage, False);			n++;
   XtSetArg (args[n], XmNtextTranslations, trans_table);	n++;
   change_dir = change_dir_rec->change_dir =
      XmCreateSelectionDialog (parent, "change_directory", args, n);
   change_dir_rec->shell = XtParent(change_dir);

   XmStringFree (prompt);

   /* Add the help callback to each of the important sub-components */
   XtAddCallback(change_dir, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_CHANGEDIR_DIALOG_STR);
   w = XmSelectionBoxGetChild(change_dir, XmDIALOG_APPLY_BUTTON);
   XtAddCallback(w, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_CHANGEDIR_DIALOG_STR);
   w = XmSelectionBoxGetChild(change_dir, XmDIALOG_CANCEL_BUTTON);
   XtAddCallback(w, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_CHANGEDIR_DIALOG_STR);
   w = XmSelectionBoxGetChild(change_dir, XmDIALOG_HELP_BUTTON);
   XtAddCallback(w, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_CHANGEDIR_DIALOG_STR);
   XtAddCallback(w, XmNactivateCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_CHANGEDIR_DIALOG_STR);
   w = XmSelectionBoxGetChild(change_dir, XmDIALOG_OK_BUTTON);
   XtAddCallback(w, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_CHANGEDIR_DIALOG_STR);
   w = XmSelectionBoxGetChild(change_dir, XmDIALOG_LIST);
   XtAddCallback(XtParent(w), XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_CHANGEDIR_DIALOG_STR);
   w = XmSelectionBoxGetChild(change_dir, XmDIALOG_LIST_LABEL);
   XtAddCallback(w, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_CHANGEDIR_DIALOG_STR);
   w = XmSelectionBoxGetChild(change_dir, XmDIALOG_SELECTION_LABEL);
   XtAddCallback(w, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_CHANGEDIR_DIALOG_STR);
   w = XmSelectionBoxGetChild(change_dir, XmDIALOG_TEXT);
   XtAddCallback(w, XmNhelpCallback, (XtCallbackProc)HelpRequestCB,
                 HELP_CHANGEDIR_DIALOG_STR);


   /*  Adjust the margin widths and heights of each of the buttons  */

   XtSetArg (args[0], XmNmarginWidth, 6);
   XtSetArg (args[1], XmNmarginHeight, 2);

   w = XmSelectionBoxGetChild (change_dir_rec->change_dir, XmDIALOG_OK_BUTTON);
   XtSetValues (w, args, 2);

   w = XmSelectionBoxGetChild (change_dir_rec->change_dir,
                                                         XmDIALOG_HELP_BUTTON);
   XtSetValues (w, args, 2);

   w = XmSelectionBoxGetChild (change_dir_rec->change_dir,
                                                       XmDIALOG_CANCEL_BUTTON);
   XtSetValues (w, args, 2);

   XtUnmanageChild(XmSelectionBoxGetChild (change_dir_rec->change_dir,
                   XmDIALOG_APPLY_BUTTON));


   /*  Adjust the decorations for the dialog shell of the dialog  */

   XtSetArg(args[0], XmNmwmFunctions, MWM_FUNC_MOVE | MWM_FUNC_CLOSE);
   XtSetArg(args[1], XmNmwmDecorations,
             MWM_DECOR_BORDER | MWM_DECOR_TITLE);
   XtSetValues (change_dir_rec->shell, args, 2);

   XtSetArg(args[0], XmNcancelButton, w);
   XtSetValues (change_dir_rec->change_dir, args, 1);

   /*  Set the return values for the dialog widget and dialog instance.  */

   *shell = change_dir_rec->change_dir;
   *dialog = (XtPointer) change_dir_rec;

}




/************************************************************************
 *
 *  InstallChange
 *
 ************************************************************************/

static void
InstallChange(
        ChangeDirRec *change_dir_rec,
        XtCallbackProc callback,
        XtPointer client_data )
{
   Widget selection_box = change_dir_rec->change_dir;
   ChangeDirApply * apply_data;

   /*  Setup the callback data to be sent to the Ok callback.   */
   /*  This contains the encapsulation callback to invoke upon  */
   /*  the data within the dialog being changed.                */

   apply_data = (ChangeDirApply *) XtMalloc (sizeof (ChangeDirApply));
   apply_data->callback = callback;
   apply_data->client_data = client_data;
   apply_data->change_dir_rec = (XtPointer) change_dir_rec;
   change_dir_rec->apply_data = apply_data;

   /*  Add the callbacks for list item insertion and help.  */

   XtAddCallback (selection_box, XmNokCallback, (XtCallbackProc) OkCallback,
                                                      (XtPointer) apply_data);
}




/************************************************************************
 *
 *  InstallClose
 *
 ************************************************************************/

static void
InstallClose(
        ChangeDirRec *change_dir_rec,
        XtCallbackProc callback,
        XtPointer client_data )
{
   Widget close_widget;
   Atom delete_window_atom;

   close_widget =
      XmSelectionBoxGetChild (change_dir_rec->change_dir,
                                             XmDIALOG_CANCEL_BUTTON);

   XtAddCallback (close_widget, XmNactivateCallback, callback, client_data);

   delete_window_atom = XmInternAtom (XtDisplay(change_dir_rec->shell),
                                      "WM_DELETE_WINDOW", True);
   XmRemoveWMProtocols( change_dir_rec->shell, &delete_window_atom, 1 );
   XmAddWMProtocolCallback( change_dir_rec->shell, delete_window_atom, callback,
                            (XtPointer)client_data );
}




/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/

static void
Destroy(
        ChangeDirRec *change_dir_rec )
{
   XtDestroyWidget (change_dir_rec->change_dir);
   XtFree ((char *) change_dir_rec->apply_data);
   XtFree ((char *) change_dir_rec);
}




/************************************************************************
 *
 *  GetValues
 *
 ************************************************************************/

static XtPointer
GetValues(
        ChangeDirRec *change_dir_rec )
{
   ChangeDirData * change_dir_data;
   Arg args[8];
   register int n;
   XmStringTable history_list;
   XmString      string_path;


   /*  Allocate and initialize the change dir dialog data.  */

   change_dir_data = (ChangeDirData *) XtMalloc (sizeof (ChangeDirData));

   change_dir_data->displayed = True;
   change_dir_data->host_name = NULL;
   change_dir_data->file_mgr_rec = NULL;

   n = 0;
   XtSetArg (args[n], XmNx, &change_dir_data->x);		n++;
   XtSetArg (args[n], XmNy, &change_dir_data->y);		n++;
   XtSetArg (args[n], XmNwidth, &change_dir_data->width);		n++;
   XtSetArg (args[n], XmNheight, &change_dir_data->height);		n++;
   XtSetArg (args[n], XmNlistItemCount, &change_dir_data->list_count);	n++;
   XtSetArg (args[n], XmNlistVisibleItemCount, &change_dir_data->visible_count);	n++;
   XtSetArg (args[n], XmNtextString, &string_path);		n++;
   XtSetArg (args[n], XmNlistItems, &history_list);		n++;
   XtGetValues (change_dir_rec->change_dir, args, n);


   /*  Extract and make local copies of the XmString data.  */

   change_dir_data->history_list = (XmStringTable)
      XtMalloc (sizeof(XmStringTable) * (change_dir_data->list_count + 1));

   for (n = 0; n < change_dir_data->list_count; n++)
      change_dir_data->history_list[n] = XmStringCopy (history_list[n]);
   change_dir_data->history_list[n] = NULL;

   if(change_dir_data->string_path)
      XmStringFree (change_dir_data->string_path);
   change_dir_data->string_path = XmStringCopy (string_path);

   return ((XtPointer) change_dir_data);
}




/************************************************************************
 *
 *  GetDefaultValues
 *
 ************************************************************************/

static XtPointer
GetDefaultValues( void )
{
   ChangeDirData * change_dir_data;


   /*  Allocate and initialize the default change dir dialog data.  */

   change_dir_data = (ChangeDirData *) XtMalloc (sizeof (ChangeDirData));

   change_dir_data->displayed = False;
   change_dir_data->x = 0;
   change_dir_data->y = 0;
   change_dir_data->height = 0;
   change_dir_data->width = 0;
   change_dir_data->string_path = NULL;

/*
   This will cause users_home_dir to be displayed in
   Application Manager go to dialog.

   if(restrictMode)
      change_dir_data->string_path =
              XmStringCreateLocalized(users_home_dir, XmFONTLIST_DEFAULT_TAG);
   else
      change_dir_data->string_path = NULL;
*/
   change_dir_data->history_list = NULL;
   change_dir_data->visible_count = 8;
   change_dir_data->list_count = 0;
   change_dir_data->host_name = NULL;
   change_dir_data->file_mgr_rec = NULL;


   return ((XtPointer) change_dir_data);
}




/************************************************************************
 *
 *  GetResourceValues
 *
 ************************************************************************/

static XtPointer
GetResourceValues(
        XrmDatabase data_base,
        char **name_list )
{
   ChangeDirData * change_dir_data;
   XmStringTable   new_table;
   int i;


   /*  Allocate and get the resources for change dir dialog data.  */

   change_dir_data = (ChangeDirData *) XtMalloc (sizeof (ChangeDirData));

   _DtDialogGetResources (data_base, name_list,
                       CHANGEDIRECTORY, (char *) change_dir_data,
                       resources, changeDirClass->resource_count);

   change_dir_data->host_name = NULL;


   /*  If the history list is Non-Null, realloc the pointer array  */
   /*  one element larger and NULL out the new last element        */

   if (change_dir_data->list_count != 0)
   {
      new_table = (XmStringTable) XtMalloc (sizeof (XmStringTable) *
                                            (change_dir_data->list_count + 1));

      for (i = 0; i < change_dir_data->list_count; i++)
         new_table[i] = change_dir_data->history_list[i];

      new_table[change_dir_data->list_count] = NULL;

/******************
  THIS WILL BE A MEMORY HOLE WHEN MOTIF FIXES THE XMSTRINGTABLE
  CONVERTER.  EITHER REALLOC THE RETURNED TABLE OR COPY AND FREE IT.
******************/
      change_dir_data->history_list = new_table;
   }

   return ((XtPointer) change_dir_data);
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static void
SetValues(
        ChangeDirRec *change_dir_rec,
        ChangeDirData *change_dir_data )
{
   Arg args[4];
   char *tmp_str, *host_name;
   XmString host_string;
   DialogData * dialog_data;
   FileMgrData * file_mgr_data;

   dialog_data = _DtGetInstanceData (change_dir_data->file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   if(change_dir_data->string_path)
      XmStringFree (change_dir_data->string_path);

   if( restrictMode
       && file_mgr_data->toolbox == False )
      change_dir_data->string_path =
              XmStringCreateLocalized( users_home_dir);
   else if( file_mgr_data->restricted_directory )
      change_dir_data->string_path =
        XmStringCreateLocalized( file_mgr_data->current_directory);
   else
      change_dir_data->string_path = NULL;

   XtSetArg (args[0], XmNlistItemCount, change_dir_data->list_count);
   XtSetArg (args[1], XmNlistVisibleItemCount, change_dir_data->visible_count);
   XtSetArg (args[2], XmNtextString, change_dir_data->string_path);
   XtSetArg (args[3], XmNlistItems, change_dir_data->history_list);

   XtSetValues (change_dir_rec->change_dir, args, 4);

   if( change_dir_data->string_path )
   {
      Widget text;

      text = XmSelectionBoxGetChild(change_dir_rec->change_dir, XmDIALOG_TEXT);
      XtAddCallback (text, XmNmodifyVerifyCallback,
                     (XtCallbackProc)TextChange, (XtPointer)file_mgr_data );
      XtAddCallback (text, XmNmotionVerifyCallback,
                     (XtCallbackProc)TextChange, (XtPointer)file_mgr_data );
      XtAddCallback (text, XmNvalueChangedCallback,
                     (XtCallbackProc)TextChange, (XtPointer)file_mgr_data );
   }

   tmp_str = GETMESSAGE(2, 16, "System Name: ");
   host_name = XtMalloc(strlen(tmp_str) + strlen(change_dir_data->host_name)+1);
   sprintf(host_name, "%s%s", tmp_str, change_dir_data->host_name);
   host_string = XmStringCreateLocalized (host_name);
   XtSetArg (args[0], XmNlabelString, host_string);
   XtSetValues (XmSelectionBoxGetChild (change_dir_rec->change_dir,
                                        XmDIALOG_LIST_LABEL), args, 1);

   XtFree(host_name);
   XmStringFree(host_string);
}




/************************************************************************
 *
 *  WriteResourceValues
 *
 ************************************************************************/

static void
WriteResourceValues(
        DialogData *values,
        int fd,
        char **name_list )
{
   ChangeDirData * change_dir_data = (ChangeDirData *) values->data;
   ChangeDirRec * change_dir_rec;

   /*  If the dialog is currently displayed, update the geometry  */
   /*  fields to their current values.                            */

   if (change_dir_data->displayed == True)
   {
      _DtGenericUpdateWindowPosition(values);
      change_dir_rec = (ChangeDirRec *) _DtGetDialogInstance (values);
   }

   _DtDialogPutResources (fd, name_list, CHANGEDIRECTORY, values->data,
                          resources, changeDirClass->resource_count);
}




/************************************************************************
 *
 *  FreeValues
 *
 ************************************************************************/

static void
FreeValues(
        ChangeDirData *change_dir_data )
{
   register int i;

   for (i = 0; i < change_dir_data->list_count; i++)
      XmStringFree (change_dir_data->history_list[i]);
   XtFree ((char *) change_dir_data->history_list);

   XmStringFree (change_dir_data->string_path);
   XtFree ((char *) change_dir_data->host_name);

   XtFree ((char *) change_dir_data);
}


/************************************************************************
 ************************************************************************
 *
 *   Internal functions used by the ChangeDir dialog
 *
 ************************************************************************
 ************************************************************************/

/*****************************************************************************
  TryToChangeDir
  INPUT:
  OUTPUT:
  DESCRIPTION:
    This function is used both by the ChangeDir dialog, and the FileMgr
    dialog's text field; it attempts to verify the incoming string, and
    then change to the indicated directory.
  NOTE:
 *****************************************************************************/
static Boolean
TryToChangeDir(
       Boolean isFromDialog,
       char * incoming_string,
       FileMgrData * file_mgr_data,
       FileMgrRec * file_mgr_rec,
       ChangeDirData * change_dir_data,
       Widget selection_box,
       ChangeDirApply * apply_data,
       XmSelectionBoxCallbackStruct * callback_data,
       void (*callback)())
{
  char * new_directory;
  char * host;
  char * path;
  struct stat stat_buf;
  Widget list;
  XmString path_string;
  XmString host_string;
  Arg args[1];
  char * message_buf;
  char * tmpStr;
  char * title;
  char * msg;
  Boolean rc = True;
  char *restricted = NULL;

  new_directory = XtNewString(incoming_string);

  if ((new_directory) && (strcmp(new_directory, "") != 0))
  {
     _DtPathFromInput(new_directory, file_mgr_data->current_directory,
                      &host, &path);
  }
  else
  {
     tmpStr = GetSharedMessage(CHANGE_DIR_ERROR_TITLE);
     title = XtNewString(tmpStr);
     tmpStr = GETMESSAGE(2,18, "Destination Folder name is missing.\nType in a folder name or select a folder from the list.");
     msg = XtNewString(tmpStr);

     if(isFromDialog)
       _DtMessage(selection_box, title, msg, NULL, HelpRequestCB);
     else
       _DtMessage(file_mgr_rec->current_directory_text, title, msg, NULL,
                  HelpRequestCB);

     XtFree(title);
     XtFree(msg);
     return False;
  }


   /* Don't allow access to the desktop directory */
   if (path)
   {
      char *ttpath = (char *) GetTTPath(path);
      char *dtpath = (char *) GetTTPath(desktop_dir);
      if( ttpath && strcmp(ttpath, dtpath) == 0)
      {
        restricted = ttpath;
        XtFree(path);
        path = NULL;
      }
   }

   /* Stat the file and see if it is a valid directory.  (If the current view
      is restricted, make sure that the new directory doesn't violate the
      directory restrictions. If so, refilter the displayed file set to show
      the files in this directory.
   */
   if ((path)
       &&(stat(path, &stat_buf) == 0)
       &&((stat_buf.st_mode & S_IFMT) == S_IFDIR)
       &&(CheckRestrictedDir(file_mgr_rec, file_mgr_data, &path, False) == 0))
   {
     /* Check for read/xcute permission */
     if (CheckAccess(path, R_OK | X_OK))
     {
       tmpStr = GETMESSAGE(9, 6, "Action Error");
       title = XtNewString(tmpStr);
       msg = (char *)XtMalloc(strlen(GETMESSAGE(30, 1, "Cannot read from %s"))
                            + strlen(new_directory)
                            + 1);
       sprintf(msg, GETMESSAGE(30, 1, "Cannot read from %s"), new_directory);

       if(isFromDialog)
         _DtMessage(selection_box, title, msg, NULL, HelpRequestCB);
       else
         _DtMessage(file_mgr_rec->current_directory_text, title,
                    msg, NULL, HelpRequestCB);

       XtFree(title);
       XtFree(msg);
       return False;
     }
     else
     {
       if (selection_box)
       {
         /* Adjust the selection box elements to add the text item
          into the list.
         */
         list = XmSelectionBoxGetChild(selection_box, XmDIALOG_LIST);

         path_string = XmStringCreateLocalized(path);

         if (XmListItemExists(list, path_string))
           XmListDeselectAllItems(list);
         else
           XmListAddItem(list, path_string, 0);

         XmListSelectItem(list, path_string, False);
         XmListSetBottomPos(list, 0);

         XmStringFree(path_string);
       }

       /*  Clear out the text string  */
       if (isFromDialog)
       {
         if(restrictMode)
           XtSetArg(args[0],
                    XmNtextString,
                    XmStringCreateLocalized(users_home_dir));
         else
           XtSetArg(args[0], XmNtextString, NULL);
         XtSetValues(selection_box, args, 1);
       }
       else
       {
         if(!file_mgr_data || !file_mgr_data->restricted_directory)
           XmTextFieldSetString(file_mgr_rec->current_directory_text,
                              incoming_string);

       }


       /* Update the change directory host name field
          and the host name indicator widget in the dialog.
       */
       XtFree((char *) change_dir_data->host_name);
       change_dir_data->host_name = XtNewString(host);

       if (selection_box)
       {
         tmpStr = GETMESSAGE(2, 16, "System Name: ");
         message_buf = XtMalloc(strlen(tmpStr) +
                                strlen(change_dir_data->host_name) + 1);
         sprintf(message_buf, "%s%s", tmpStr, change_dir_data->host_name);
         host_string =
           XmStringCreateLocalized(message_buf);
         XtSetArg(args[0], XmNlabelString, host_string);
         XtSetValues(XmSelectionBoxGetChild(selection_box,XmDIALOG_LIST_LABEL),
                     args, 1);
         XtFree(message_buf);
         XmStringFree(host_string);

         XmUpdateDisplay (selection_box);
       }

       /*  Call the encapsulation change data callback  */
       if (isFromDialog)
         (*callback)(selection_box, apply_data->client_data, path);
       else
         (*callback)(file_mgr_data, path);
     }
   }
   else
   {
     tmpStr = GetSharedMessage(CHANGE_DIR_ERROR_TITLE);
     title = XtNewString(tmpStr);
     if(restricted)
     {
       tmpStr = GETMESSAGE(2,20,"You cannot switch to folder:\n\n%s\n\nYou are not allowed to view  this folder\nbecause it is a restricted folder.");
       path = restricted;
     }
     else
       tmpStr = GETMESSAGE(2,19,"The following folder name is invalid.\n\n%s");
     if(path)
     {
       message_buf = XtMalloc(strlen(tmpStr) + strlen(path) + 1);
       sprintf(message_buf, tmpStr, path);
     }
     else
     {
       message_buf = XtMalloc(strlen(tmpStr) + strlen(new_directory) + 1);
       sprintf(message_buf, tmpStr, new_directory);
     }

     if (isFromDialog)
       _DtMessage(selection_box, title, message_buf, NULL, HelpRequestCB);
     else
       _DtMessage(file_mgr_rec->current_directory_text, title, message_buf,
                  NULL, HelpRequestCB);

     XtFree(title);
     XtFree(message_buf);
     rc = False;
   }

   if (path)
      XtFree((char *) path);
   XtFree((char *) host);
   XtFree((char *) new_directory);

   return rc;
}



/************************************************************************
 *
 *  OkCallback
 *	This is a callback function called when the Ok button (Apply)
 *	is pressed.  It adds the string contained within the call data
 *	into the selection boxes list.
 *
 ************************************************************************/

static void
OkCallback(
        Widget selection_box,
        ChangeDirApply *apply_data,
        XmSelectionBoxCallbackStruct *callback_data )
{
   DialogData * dialog_data;
   ChangeDirData * change_dir_data;
   FileMgrRec * file_mgr_rec;
   FileMgrData * file_mgr_data;
   char *strValue;
   char *value = NULL;
   int result;
   char message_buf[MAX_PATH + 100];
   char * tmpStr;
   char * title;
   char * msg;


   /*  Get the change dir data record  */

   dialog_data = _DtGetInstanceData (apply_data->change_dir_rec);
   change_dir_data = (ChangeDirData *) dialog_data->data;

   file_mgr_rec = (FileMgrRec *) change_dir_data->file_mgr_rec;
   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   value = (char *) _XmStringUngenerate((XmString)callback_data->value,
                                        XmFONTLIST_DEFAULT_TAG,
                                        XmCHARSET_TEXT, XmCHARSET_TEXT);

   if ( value == NULL)
   {
       tmpStr = GetSharedMessage(CHANGE_DIR_ERROR_TITLE);
      title = XtNewString(tmpStr);
      tmpStr = (char *) GETMESSAGE(2, 11, "Cannot get the new folder name.");
      msg = XtNewString(tmpStr);

      _DtMessage(selection_box, title, message_buf, NULL, HelpRequestCB);

      XtFree(title);
      XtFree(msg);
      return;
   }

   strValue = XtNewString(value);
   if(file_mgr_data->toolbox && strValue[0] != '/')
      result = CheckRestrictedDir(file_mgr_rec, file_mgr_data, &strValue, True);
   else
      result = CheckRestrictedDir(file_mgr_rec, file_mgr_data, &strValue,False);

   /*  Process the string representing a directory to verify  */
   /*  that it is a valid path.                               */
   if (result == 0)
   {
      TryToChangeDir(True, (char *)strValue, file_mgr_data,
                     file_mgr_rec, change_dir_data, selection_box, apply_data,
                     callback_data, apply_data->callback);
   }

   XtFree(value);
   XtFree(strValue);
}


/************************************************************************
 *
 *  CheckRestrictedDir:
 *    Make sure we stay inside a restricted directory
 *
 ************************************************************************/

static int
CheckRestrictedDir (
	FileMgrRec * file_mgr_rec,
	FileMgrData * file_mgr_data,
	char ** value,
	Boolean relative_name)
{
   char *tmpStr, directory[MAXPATHLEN];
   int len;
   char *tmpBuffer, *title, *msg;

   /*  If not in restricte mode, everything is ok.  */
   if (file_mgr_data->restricted_directory == NULL && !restrictMode)
     return 0;

   /* get real file name */
   if (relative_name)
   {
      tmpStr = (char *)XtMalloc(strlen(*value) +
                  strlen(file_mgr_data->restricted_directory) + 3);
      strcpy(tmpStr, file_mgr_data->restricted_directory);
      if(*value[0] != '/')
         strcat(tmpStr, "/");
      strcat(tmpStr, *value);
      XtFree(*value);
      *value = tmpStr;
   }
   *value = (char *) DtEliminateDots (*value);

   if( restrictMode
       && file_mgr_data->toolbox == False )
   {
      if( strcmp( users_home_dir, "/" ) == 0 )
         return 0;
      strcpy( directory, users_home_dir );
   }
   else
      strcpy( directory, file_mgr_data->restricted_directory );

   len = strlen(directory);

   if( len > 1 && directory[len-1] == '/' )
   {
      directory[len - 1] = 0x0;
      --len;
   }

   /* check if value is inside the restricted subdir */
   len = strlen(directory);
   if (strncmp(*value, directory, len) != 0 ||
       (*value)[len] != '/' && (*value)[len] != '\0')
   {
       tmpBuffer = GetSharedMessage(CHANGE_DIR_ERROR_TITLE);
       title = XtNewString(tmpBuffer);
       if(restrictMode)
          tmpBuffer = GETMESSAGE(2,8, "You cannot switch to this folder.  You are\nallowed to view only folders beneath your\ncurrent folder. You cannot specify an absolute\npath to the new folder.");
       else
          tmpBuffer = GETMESSAGE(2,9, "This view is in restricted mode.  You cannot go\nto the specified folder because it is not in the\nrelative path of the restricted folder.\n");
       msg = XtNewString(tmpBuffer);

       _DtMessage(file_mgr_rec->current_directory_text, title, msg,
                  NULL, HelpRequestCB);

       XtFree(title);
       XtFree(msg);
       return -1;
   }

   return 0;
}


/************************************************************************
 *
 *  ChangeToNewDir
 *	This is the activate callback for the FileMgr text field provided
 *	for quickly changing the current directory being viewed.
 *
 ************************************************************************/

void
ChangeToNewDir (
       Widget text_field,
       XtPointer client_data,
       XmAnyCallbackStruct * callback_data )
{
   DialogData * dialog_data;
   ChangeDirData * change_dir_data;
   ChangeDirRec * change_dir_rec;
   FileMgrRec * file_mgr_rec;
   FileMgrData * file_mgr_data;
   char * value;
   Widget selection_box;
   Arg args[1];
   char * tmpStr;
   int rc;

   /*  Get the change dir data record  */

   file_mgr_rec = (FileMgrRec *) client_data;
   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;
   change_dir_data = (ChangeDirData *) file_mgr_data->change_dir->data;
   change_dir_rec =(ChangeDirRec *)_DtGetDialogInstance(file_mgr_data->change_dir);

   if (change_dir_rec)
      selection_box = change_dir_rec->change_dir;
   else
      selection_box = NULL;

   value = (char *)XmTextFieldGetString(text_field);

   /* check if user attempts to go outside restricted directory */
   rc = CheckRestrictedDir (file_mgr_rec, file_mgr_data, &value, True);

/*
   This code will de-activate the text widget.
   HCI requests that if the switch is not successful, don't de-activate it
   so user has a change to go back and correct it without having to pop it
   up again.

   XmProcessTraversal(file_mgr_rec->file_window, XmTRAVERSE_CURRENT);
   XtSetArg (args[0], XmNallowShellResize, False);
   XtSetValues (file_mgr_rec->shell, args, 1);
   XtUnmanageChild(file_mgr_rec->current_directory_text);
   XtSetArg (args[0], XmNallowShellResize, True);
   XtSetValues (file_mgr_rec->shell, args, 1);
   file_mgr_data->fast_cd_enabled = False;
*/

   /*  Process the string representing a directory to verify  */
   /*  that it is a valid path.                               */
   if (rc == 0)
   {
     if( TryToChangeDir(False, value, file_mgr_data, file_mgr_rec,
                        change_dir_data, selection_box, NULL, NULL,
                        GoToNewDir) )
       /* de-activate the text widget */
       UnpostTextPath( file_mgr_data );
   }

   XtFree(value);
}


/************************************************************************
 *
 *  DropOnChangeView
 *	This is the drop callback for the change-view icon provided
 *	for quickly changing the current directory being viewed.
 *
 ************************************************************************/

void
DropOnChangeView (
     Widget w,
     XtPointer client_data,
     XtPointer call_data)
{
   DtDndDropCallbackStruct *fileList = (DtDndDropCallbackStruct *)call_data;
   FileMgrRec *file_mgr_rec;
   DialogData * dialog_data;
   ChangeDirData * change_dir_data;
   ChangeDirRec * change_dir_rec;
   FileMgrData * file_mgr_data;
   Widget selection_box;
   char *full_path_name;
   int  numFiles;

   numFiles = fileList->dropData->numItems;

   /* if multiple selection, ignore all but the last one */

   if (numFiles > 0 && fileList->reason == DtCR_DND_DROP_ANIMATE)
   {
      file_mgr_rec = (FileMgrRec *) client_data;
      dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
      file_mgr_data = (FileMgrData *) dialog_data->data;

      full_path_name = XtNewString(fileList->dropData->data.files[numFiles-1]);

      if (CheckRestrictedDir(file_mgr_rec, file_mgr_data,
                             &full_path_name, False) == 0)
      {
         change_dir_data = (ChangeDirData *) file_mgr_data->change_dir->data;
         change_dir_rec =
              (ChangeDirRec *)_DtGetDialogInstance(file_mgr_data->change_dir);
         if (change_dir_rec)
            selection_box = change_dir_rec->change_dir;
         else
            selection_box = NULL;

         TryToChangeDir(False, full_path_name, file_mgr_data, file_mgr_rec,
                        change_dir_data, selection_box, NULL, NULL,
                        GoToNewDir);
      }
      XtFree(full_path_name);
   }

   fileList->status = DtDND_FAILURE;
}


static void
GoToNewDir(
       FileMgrData * file_mgr_data,
       char * dir_name )
{
   ChangeDirData * change_dir_data;
   int i;
   Boolean found;
   XmString xm_dir_name;

   /* Clear out any selected portion of the old CWD string */
   if (file_mgr_data->cd_select != NULL)
   {
      XtFree(file_mgr_data->cd_select);
      file_mgr_data->cd_select = NULL;
   }

   /* Update the internal copy of the dialog history list */
   if (file_mgr_data->change_dir)
   {
      change_dir_data = (ChangeDirData *) file_mgr_data->change_dir->data;
      xm_dir_name = XmStringCreateLocalized(dir_name);

      /* Add the directory only if not already in the list */
      for (i = 0, found = False; i < change_dir_data->list_count; i++)
      {
         if (XmStringCompare(xm_dir_name, change_dir_data->history_list[i]))
         {
            found = True;
            XmStringFree(xm_dir_name);
            break;
         }
      }

      if (!found)
      {
         change_dir_data->list_count++;
         change_dir_data->history_list = (XmStringTable)
            XtRealloc((char *)change_dir_data->history_list,
                   sizeof(XmStringTable) * (change_dir_data->list_count + 1));
         change_dir_data->history_list[change_dir_data->list_count - 1] =
                xm_dir_name;
         change_dir_data->history_list[change_dir_data->list_count] = NULL;
      }

      /* Update view to display the new directory */
      ShowNewDirectory(file_mgr_data, change_dir_data->host_name, dir_name);
   }
}


/*
 * Class function for forcing the focus to the text field, each time
 * the Change Dir dialog is posted.
 */

static void
SetFocus(
        ChangeDirRec *change_dir_rec,
        ChangeDirData *change_dir_data )
{
   Widget text;

   /* Force the focus to the text field */
   text = XmSelectionBoxGetChild (change_dir_rec->change_dir, XmDIALOG_TEXT);
   XmProcessTraversal(text, XmTRAVERSE_CURRENT);
}


/***************************************************************************
 *
 * TextChange() - callback from editable text widgets when something is
 *        typed in the text widget.  Only used when in restricted mode.
 *        Used to make sure the users are not going out of their
 *        "restricted" space.
 *
 ****************************************************************************/

void
TextChange(
        Widget text,
        XtPointer client_data,
        XmTextVerifyCallbackStruct * callback_data )
{
   char *homeDir, *homeDirBackup;
   char *selection;

   if (callback_data->reason == XmCR_MODIFYING_TEXT_VALUE ||
       callback_data->reason == XmCR_MOVING_INSERT_CURSOR )
   {
      FileMgrData * file_mgr_data;

      file_mgr_data = (FileMgrData *) client_data;

      selection = XmTextFieldGetSelection(text);

      /* get whats in the text widget */
      homeDir = homeDirBackup = XmTextFieldGetString(text);

      if(selection != NULL)
      {
         if(callback_data->event == NULL)
            XmTextFieldClearSelection(text, CurrentTime);
         else
            XmTextFieldClearSelection(text, callback_data->event->xkey.time);
         callback_data->doit = False;
      }
      else if(callback_data->reason == XmCR_MODIFYING_TEXT_VALUE)
      {
         /* see if the user is trying to back over the 'restricted'
          * text, in this case users_home_dir
          */
         if( strcmp( homeDir, "" ) != 0
             && strcmp( homeDir, file_mgr_data->restricted_directory ) <= 0 )
         {
            if( callback_data->text->length != 0 )
            {
               if( strncmp( file_mgr_data->restricted_directory,
                            callback_data->text->ptr,
                            strlen(file_mgr_data->restricted_directory)) != 0)
               {
                  callback_data->doit = True;
               }
            }
            else
            {
               callback_data->doit = False;
            }
         }
      }
      else
      {
         int maxlength;

         maxlength = strlen( file_mgr_data->restricted_directory );

         if(callback_data->newInsert < maxlength)
         {
            if(callback_data->event == NULL)
               _XmTextFieldSetDestination((Widget)text,
                            (Position)callback_data->currInsert, CurrentTime);
            else
            {
               _XmTextFieldSetDestination((Widget)text,
                                          (Position)callback_data->currInsert,
                                          callback_data->event->xkey.time);
               callback_data->doit = False;
            }
         }
      }

      XtFree(homeDirBackup);
      XtFree(selection);
   }
}

void
ChangeDirectoryToParent(
      FileMgrRec * file_mgr_rec,
      char *directory)
{
   DialogData * dialog_data;
   ChangeDirData * change_dir_data;
   ChangeDirRec * change_dir_rec;
   FileMgrData * file_mgr_data;
   Widget selection_box;
   char * tmpStr;
   int rc;

   /*  Get the change dir data record  */

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;
   change_dir_data = (ChangeDirData *) file_mgr_data->change_dir->data;
   change_dir_rec =(ChangeDirRec *)_DtGetDialogInstance(file_mgr_data->change_dir);

   if (change_dir_rec)
      selection_box = change_dir_rec->change_dir;
   else
      selection_box = NULL;

   /* check if user attempts to go outside restricted directory */
   rc = CheckRestrictedDir (file_mgr_rec, file_mgr_data, &directory, True);

   /*  Process the string representing a directory to verify  */
   /*  that it is a valid path.                               */
   if (rc == 0)
   {
      TryToChangeDir(False, directory, file_mgr_data, file_mgr_rec,
                     change_dir_data, selection_box, NULL, NULL,
                     GoToNewDir);
   }

}
