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
/* $TOG: OverWrite.c /main/8 1998/10/26 12:40:12 mgreess $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           OverWrite.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Contains routines for collision dialog during
 *                   move/copy/link operations.
 *
 *   FUNCTIONS: Create_Action_Area
 *		buffer_multicollide_ok_callback
 *		buffer_replace_rename_ok_callback
 *		create_multicollide_dialog
 *		create_replace_merge_dialog
 *		create_replace_rename_dialog
 *		getVariableMessage
 *		multiRenameError_ok_callback
 *		multiReplaceError_ok_callback
 *		multicollide_cancel_callback
 *		multicollide_ok_callback
 *		renameError_ok_callback
 *		replaceError_ok_callback
 *		replace_merge_cancel_callback
 *		replace_merge_ok_callback
 *		replace_rename_cancel_callback
 *		replace_rename_ok_callback
 *		replace_rename_radio_callback
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

/* includes */
#include <errno.h>

#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/TextF.h>
#include <Xm/Separator.h>


#include <Dt/DtNlUtils.h>
#include <Dt/SharedProcs.h>

#include "Encaps.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "SharedProcs.h"
#include "Help.h"

#include "sharedFuncs.h"
#include "fsrtns.h"


/* defines */
#define VERTICAL_SPACING  5
#define HORIZONTAL_SPACING  10
#define TOP_SPACING  (VERTICAL_SPACING + 3)

/* types of messages sent through the pipe */
/* these messages are also defined & used in FileOp.c */
#define PIPEMSG_CANCEL      101
#define PIPEMSG_PROCEED     102
#define PIPEMSG_MERGE       103
#define PIPEMSG_REPLACE_BUFFER  104
#define PIPEMSG_RENAME_BUFFER   105
#define PIPEMSG_MULTI_PROCEED   106

#define RADIO_REPLACE 0
#define RADIO_RENAME  1


/* global variables */
Boolean G_initImages          = True;
int     G_filop_confirm_fd    = -1;
char    G_directory[MAX_PATH];
char    G_file[MAX_PATH];
String  *G_fileList           = NULL;
int     G_nCollisions         = 0;
Widget  G_dialogShell         = NULL;
Widget  G_rename_text         = NULL;
Widget  G_rename_toggle       = NULL;
Widget  G_merge_toggle        = NULL;
Widget  G_multi_rename_toggle = NULL;
int     G_mode;

/* types */
typedef enum {
  vm_replaceCannotDeleteFolder,         /* 80 81 */
  vm_replaceCannotDeleteFile,           /* 82 */
  vm_multiCannotRenameMany,             /* 83 84 */
  vm_multiCannotRenameOne,              /* 85 84 */
  vm_multiCannotDeleteFolderMany,       /* 86 */
  vm_multiCannotDeleteFolderOne,        /* 87 */
  vm_multiCannotDeleteFileMany,         /* 88 */
  vm_multiCannotDeleteFileOne          /* 89 */
} VariableMessage;

/* local functions */
static void      replaceError_ok_callback(
	                            Widget w,
                                    XtPointer client_data,
                                    XtPointer call_data);
static void      renameError_ok_callback(
	                            Widget w,
                                    XtPointer client_data,
                                    XtPointer call_data);
static void      multiReplaceError_ok_callback(
	                            Widget w,
                                    XtPointer client_data,
                                    XtPointer call_data);
static void      multiRenameError_ok_callback(
	                            Widget w,
                                    XtPointer client_data,
                                    XtPointer call_data);
static char *    getVariableMessage(
                                    VariableMessage message,
                                    int             mode);
static Boolean   IsReplaceable(
                                    char *name);


/*--------------------------------------------------------------------
 * Callback routines
 *------------------------------------------------------------------*/


void
replace_rename_ok_callback(
	Widget w,
	XtPointer client_data,
        XtPointer call_data)
{
   const int    rc = PIPEMSG_PROCEED;
   String       newFile, renameNewName, destinationName;
   struct stat  buf;
   int          saveError = 0;
   static char  msg[30+(3*MAX_PATH)];
   String       title;
   char         *newFileDir = NULL;
   char         *newFileName = NULL;
   int          newFileDirLen;
   int          rename_rc;
   Boolean      same = (Boolean)(XtArgVal)client_data;

   /* if same is true, then the object is tring to be copied into the same
    * folder as itself.  So the existing files stay the same and the new files
    * being created take on the new names.
    */

   /* Rename was selected */
   if (XmToggleButtonGetState(G_rename_toggle))
   {
      newFile         = XmTextFieldGetString(G_rename_text);
      renameNewName   = build_path(G_directory,newFile);
      destinationName = build_path(G_directory,G_file);
      XtFree(newFile);
      /* make sure renameNewName does not exist */
      if ( (lstat(renameNewName, &buf)) == 0)
      {
         sprintf(msg,
                 GETMESSAGE(9, 38, "%s\nalready exists: choose another name."),
                 renameNewName);
         title = XtNewString(GETMESSAGE(9, 37, "Object Rename - Error"));
         _DtMessageDialog (G_dialogShell, title, msg, 0, FALSE, NULL,
                           renameError_ok_callback, NULL, NULL, False, ERROR_DIALOG);
         XtFree(title);
         XtFree(renameNewName);
         XtFree(destinationName);
         return;

      }
      if(!same)
      {
          /* attempt the rename ... since rename function is being used, this*/
          /* only allows renames within the same filesystem */

          DirectoryFileModified(home_host_name, G_directory, G_file);
          if (DtStrchr(newFile, '/') == NULL)
              DirectoryFileModified(home_host_name, G_directory, newFile);
          else
          {
              newFileName = strrchr(renameNewName, '/');
              newFileDirLen = newFileName - renameNewName;
              newFileName++;

              newFileDir = XtMalloc(newFileDirLen + 1);
              strncpy(newFileDir, renameNewName, newFileDirLen);
              newFileDir[newFileDirLen] = '\0';

              DirectoryBeginModify(home_host_name, newFileDir);
              DirectoryFileModified(home_host_name, newFileDir, newFileName);
          }

          errno = 0;
          rename_rc = rename(destinationName, renameNewName);

          if (newFileDir != NULL)
          {
              DirectoryEndModify(home_host_name, newFileDir);
              XtFree(newFileDir);
          }

          if (rename_rc != 0)
          {
              saveError = errno;
              sprintf(msg,
                      GETMESSAGE(9, 39, "%s\ncannot be renamed to\n%s\n%s"),
                      destinationName, renameNewName, strerror(saveError));
              title = XtNewString(GETMESSAGE(9, 37, "Object Rename - Error"));
              _DtMessageDialog (G_dialogShell, title, msg, 0, FALSE, NULL,
                                renameError_ok_callback, NULL, NULL, False, ERROR_DIALOG);
              XtFree(title);
              XtFree(renameNewName);
              XtFree(destinationName);
              return;
          }
      }
      XtFree(renameNewName);
      XtFree(destinationName);
   }

   /* Replace was selected ... remove the offending target */
   else
   {
      destinationName = build_path(G_directory,G_file);
      DirectoryFileModified(home_host_name, G_directory, G_file);
      if(IsReplaceable(destinationName))
        fsErase(destinationName,&saveError,False);
      else
        saveError = 1;
      if ( saveError == 0 )
      {
         /* removal succeeded */
         XtFree(destinationName);
      }
      else
      {
         /* removal failed */

         stat(destinationName,&buf);
         title = XtNewString(GETMESSAGE(9, 40, "Object Replace - Error"));

         if (S_ISDIR(buf.st_mode))
         {
            strcpy(msg,getVariableMessage(vm_replaceCannotDeleteFolder, G_mode));
         }
         else
         {
            sprintf(msg,
                    getVariableMessage(vm_replaceCannotDeleteFile, G_mode),
                    destinationName);
         }

         _DtMessageDialog (G_dialogShell, title, msg, 0, FALSE, NULL,
                           replaceError_ok_callback, NULL, NULL, False, ERROR_DIALOG);
         XtFree(title);
         XtFree(destinationName);
         return;
      }
   }

   /* close the dialog */
   XtDestroyWidget(G_dialogShell);
   G_dialogShell = NULL;

   /* send return code through the pipe to the background proc */
   write(G_filop_confirm_fd, &rc, sizeof(int));
   G_filop_confirm_fd = -1;

}  /*  end replace_rename_ok_callback */


void
buffer_replace_rename_ok_callback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data)
{
   int          rc = 0;
   String       newFile, renameNewName, destinationName;
   struct stat  buf;
   int          saveError = 0;
   static char  msg[38+MAX_PATH];
   String       title;


   /* Rename was selected */
   if (XmToggleButtonGetState(G_rename_toggle))
   {
      newFile         = XmTextFieldGetString(G_rename_text);
      renameNewName   = build_path(G_directory,newFile);
      destinationName = build_path(G_directory,G_file);

      /* make sure renameNewName does not exist */
      if ( (lstat(renameNewName, &buf)) == 0)
      {
         sprintf(msg,
                 GETMESSAGE(9, 38, "%s\nalready exists: choose another name."),
                 renameNewName);
         title = XtNewString(GETMESSAGE(9, 37, "Object Rename - Error"));
         _DtMessageDialog (G_dialogShell, title, msg, 0, FALSE, NULL,
                           renameError_ok_callback, NULL, NULL, False, ERROR_DIALOG);
         XtFree(title);
         XtFree(renameNewName);
         XtFree(destinationName);
         return;

      }

      /* Perform the actual rename in the child process */
      /* set up pipe message */
      rc = PIPEMSG_RENAME_BUFFER;


      XtFree(renameNewName);
      XtFree(destinationName);
   }
   /* Replace was selected ... remove the offending target */
   else
   {
      destinationName = build_path(G_directory,G_file);
      if(IsReplaceable(destinationName))
        fsErase(destinationName,&saveError,False);
      else
        saveError = 1;
      if ( saveError == 0 )
      {

         rc = PIPEMSG_REPLACE_BUFFER;
         /* removal succeeded */
         XtFree(destinationName);

      }
      else
      {
         /* removal failed */

         stat(destinationName,&buf);
         title = XtNewString(GETMESSAGE(9, 40, "Object Replace - Error"));

         if (S_ISDIR(buf.st_mode))
         {
            strcpy(msg,getVariableMessage(vm_replaceCannotDeleteFolder, COPY_FILE));
         }
	 else
	 {
            sprintf(msg,
                    getVariableMessage(vm_replaceCannotDeleteFile, COPY_FILE),
                    destinationName);
         }


         _DtMessageDialog (G_dialogShell, title, msg, 0, FALSE, NULL,
                           replaceError_ok_callback, NULL, NULL, False, ERROR_DIALOG);
         XtFree(title);
         XtFree(destinationName);
         return;
      }
   }

   /* close the dialog */
   XtDestroyWidget(G_dialogShell);
   G_dialogShell = NULL;

   /* send return code through the pipe to the background proc */
   write(G_filop_confirm_fd, &rc, sizeof(int));

   /* write the the new name on the pipe if rename is selected */
   if (XmToggleButtonGetState(G_rename_toggle))
   {
     DPRINTF(("Writing %s to %d\n", newFile ,G_filop_confirm_fd));
     PipeWriteString(G_filop_confirm_fd, newFile);
     XtFree(newFile);
   }
   G_filop_confirm_fd = -1;

}  /*  end buffer_replace_rename_ok_callback */


void
replace_rename_cancel_callback(
	Widget w,
	XtPointer client_data,
        XtPointer call_data)
{
   const int rc = PIPEMSG_CANCEL;

   /* close the dialog */
   XtDestroyWidget(G_dialogShell);
   G_dialogShell = NULL;

   /* send negative return code through the pipe to the background proc */
   write(G_filop_confirm_fd, &rc, sizeof(int));
   G_filop_confirm_fd = -1;

}  /*  end replace_rename_cancel_callback */


static void
replace_rename_radio_callback(
	Widget w,
	XtPointer client_data,
        XtPointer call_data)
{
   XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *)call_data;


   if (state->set)
      switch ((int)(XtArgVal) client_data)
      {
        case RADIO_REPLACE:
           XtVaSetValues (G_rename_text,
                          XmNsensitive, FALSE,
                          NULL);
        break;

        case RADIO_RENAME:
           XtVaSetValues (G_rename_text,
                          XmNsensitive, TRUE,
                          NULL);
           XmProcessTraversal(G_rename_text, XmTRAVERSE_CURRENT);
        break;


      }   /* end switch */

  return;

}  /* end replace_rename_radio_callback */



static void
renameError_ok_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
  XtDestroyWidget((Widget)client_data);
  XmProcessTraversal(G_rename_text, XmTRAVERSE_CURRENT);

  return;
}  /* end renameError_ok_callback */


void
replace_merge_ok_callback(
	Widget w,
	XtPointer client_data,
        XtPointer call_data)
{
   int          rc;
   String       newFile=NULL, renameNewName, renameOldName;
   struct stat  buf;
   int          saveError = 0;
   char         msg[128];
   String       title;
   char         *destinationPath =  build_path(G_directory,G_file);

   if (XmToggleButtonGetState(G_merge_toggle))
      rc = PIPEMSG_MERGE;
   else
   {
      rc = PIPEMSG_PROCEED;
      if(DirectoryBusy(destinationPath))
      {
         char *msg,*errmsg;

         msg = GETMESSAGE(11,30, "Cannot move or rename the folder %s.\n\
All File Manager views displayed for a folder or its sub-folders\n\
must be closed before a folder can be moved or renamed.");

         errmsg = XtMalloc(strlen(msg)+strlen(destinationPath)+1);
         sprintf(errmsg,msg,destinationPath);
         title = XtNewString(GETMESSAGE(9, 40, "Object Replace - Error"));
         _DtMessage(toplevel, title, errmsg, NULL, HelpRequestCB);
         XtFree(errmsg);
         XtFree(title);
         XtFree(destinationPath);
         rc = PIPEMSG_CANCEL;
      }
   }

   /* close the dialog */
   XtDestroyWidget(G_dialogShell);
   G_dialogShell = NULL;

   /* send return code through the pipe to the background proc */
   write(G_filop_confirm_fd, &rc, sizeof(int));
   G_filop_confirm_fd = -1;

}  /*  end replace_merge_ok_callback */



void
replace_merge_cancel_callback(
	Widget w,
	XtPointer client_data,
        XtPointer call_data)
{
   const int rc = PIPEMSG_CANCEL;

   /* close the dialog */
   XtDestroyWidget(G_dialogShell);
   G_dialogShell = NULL;

   /* send negative return code through the pipe to the background proc */
   write(G_filop_confirm_fd, &rc, sizeof(int));
   G_filop_confirm_fd = -1;

}  /*  end replace_merge_cancel_callback */



void
multicollide_cancel_callback(
	Widget w,
	XtPointer client_data,
        XtPointer call_data)
{
   const int rc = PIPEMSG_CANCEL;
   int       i;

   /* close the dialog */
   XtDestroyWidget(G_dialogShell);
   G_dialogShell = NULL;

   /* de-allocate the file list */
   for ( i = 0; i < G_nCollisions; i++)
      XtFree((char *)G_fileList[i]);
   XtFree((char *)G_fileList);
   G_fileList = NULL;

   /* send return code through the pipe to the background proc */
   write(G_filop_confirm_fd, &rc, sizeof(int));
   G_filop_confirm_fd = -1;

}  /*  end multicollide_cancel_callback */


void
multicollide_ok_callback(
	Widget w,
	XtPointer client_data,
        XtPointer call_data)
{
   const int    rc = PIPEMSG_MULTI_PROCEED;
   int          i;
   int          saveError;
   String       destinationPath;
   String       title;
   static char  msg[1024];
   struct stat  buf;


   Boolean      same = (Boolean)(XtArgVal)client_data;

   /* if same is true, then the object is tring to be copied into the same
    * folder as itself.  So the existing files stay the same and the new files
    * being created take on the new names.
    */

   if (XmToggleButtonGetState(G_multi_rename_toggle))
   {
      char *names = NULL;
      title = XtNewString(GETMESSAGE(9, 37, "Object Rename - Error"));

      write(G_filop_confirm_fd, &rc, sizeof(int));
      /* Auto rename was selected */
      for ( i = 0; i < G_nCollisions; i++)
      {
          if(!same)
          {

         destinationPath = build_path(G_directory,G_fileList[i]);
         DirectoryFileModified(home_host_name, G_directory, G_fileList[i]);
         if ( auto_rename(destinationPath) != 0)
         {
           saveError = -1;
           if(!names)
             names = (char *) XtNewString(destinationPath);
           else
           {
             names = (char *)XtRealloc(names,strlen(names)+
                              strlen(destinationPath)+2);
             strcat(names,"\n");
             strcat(names,destinationPath);
           }
         }
         else
           saveError = 0;
         write(G_filop_confirm_fd, &saveError, sizeof(int));
         XtFree(destinationPath);
      }

          else
          {
              saveError = 0;
              write(G_filop_confirm_fd, &saveError, sizeof(int));
          }
      }

      if(!same)
      {
          if ( names )
          {
              char *buf;
              int bufsize;
              buf = XtMalloc(strlen(GETMESSAGE(9,138, "Failed to rename the following object(s)\n\n%s\n\nThe most likely cause is that you do not have\npermission to rename these objects(s)"))+strlen(names)+10);
              sprintf(buf,GETMESSAGE(9,138, "Failed to rename the following object(s)\n\n%s\n\nThe most likely cause is that you do not have\npermission to rename these object(s)"),names);
              _DtMessage(toplevel, title, buf, NULL, HelpRequestCB);
              XtFree(buf);
          }
          XtFree(names);
      }
      XtFree(title);
   }
   else
   {
      char *names = NULL;

      title = XtNewString(GETMESSAGE(9, 40, "Object Replace - Error"));
      write(G_filop_confirm_fd, &rc, sizeof(int));

      /* Replace was selected ... remove the offending targets */
      for ( i = 0; i < G_nCollisions; i++)
      {
         destinationPath = build_path(G_directory,G_fileList[i]);
         if(IsReplaceable(destinationPath))
	 {
	   if(DirectoryBusy(destinationPath))
	   {
             char *msg,*errmsg;

             msg = GETMESSAGE(11,30, "Cannot move or rename the folder%s.\nAll File Manager views displayed for a folder or its sub-folders\nmust be closed before a folder can be moved or renamed.");

	     errmsg = XtMalloc(strlen(msg)+strlen(destinationPath)+1);
	     sprintf(errmsg,msg,destinationPath);
             _DtMessage(toplevel, title, errmsg, NULL, HelpRequestCB);
             XtFree(errmsg);
             XtFree(destinationPath);
	     continue;
	   }
	   else
             fsErase(destinationPath,&saveError,False);
	 }
         else
           saveError = -1;
         write(G_filop_confirm_fd, &saveError, sizeof(int));
	 if(saveError != 0)
	 {
           if(!names)
	     names = (char *) XtNewString(destinationPath);
	   else
	   {
	     names = (char *)XtRealloc(names,strlen(names)+
			      strlen(destinationPath)+2);
	     strcat(names,"\n");
	     strcat(names,destinationPath);
	   }
	 }
         XtFree(destinationPath);
      }
      if ( names )
      {
        char *buf;
	int bufsize;
        buf = XtMalloc(strlen(GETMESSAGE(9,139, "Failed to replace the following object(s)\n\n%s\n\nThe most likely cause is that you do not have\npermission to replace these object(s)"))+strlen(names)+10);
        sprintf(buf,GETMESSAGE(9,139, "Failed to replace the following object(s)\n\n%s\n\nThe most likely cause is that you do not have\npermission to replace these object(s)"),names);
        _DtMessage(toplevel, title, buf, NULL, HelpRequestCB);
	XtFree(buf);
      }
      XtFree(names);
      XtFree(title);
   }


   /* close the dialog */
   XtDestroyWidget(G_dialogShell);
   G_dialogShell = NULL;

   /* de-allocate the file list */
   for ( i = 0; i < G_nCollisions; i++)
      XtFree((char *)G_fileList[i]);
   XtFree((char *)G_fileList);
   G_fileList = NULL;

   /* send return code through the pipe to the background proc */
/*
   write(G_filop_confirm_fd, &rc, sizeof(int));
*/
   G_filop_confirm_fd = -1;
}  /*  end multicollide_ok_callback */


void
buffer_multicollide_ok_callback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data)
{
   int          rc = 0;
   int          i;
   int          saveError;
   String       destinationPath;
   String       title;
   char         newPath[MAX_PATH];
   char         *newFile;
   static char  msg[1024];
   struct stat  buf;


   if (XmToggleButtonGetState(G_multi_rename_toggle))
   {
     /* send return code through the pipe to the background proc */
      rc = PIPEMSG_RENAME_BUFFER;
      write(G_filop_confirm_fd, &rc, sizeof(int));

      /* Auto rename was selected */
      for ( i = 0; i < G_nCollisions; i++)
      {
	 /* automatically generated new name */
         destinationPath = build_path(G_directory,G_fileList[i]);
	 generate_NewPath(newPath, destinationPath);
	 newFile= strrchr(newPath,'/') + 1 ;

	 DPRINTF (("buffer_multicollide_ok_callback: new file is %s\n", newFile));

	 /* write new name on the pipe */

         PipeWriteString(G_filop_confirm_fd, newFile);

         XtFree(destinationPath);
      }
   } /* endif multi auto-rename */
   else
   {
      title = XtNewString(GETMESSAGE(9, 40, "Object Replace - Error"));

      /* Replace was selected ... remove the offending targets */
      for ( i = 0; i < G_nCollisions; i++)
      {
	 /* set the pipe msg */
	 rc = PIPEMSG_REPLACE_BUFFER;

         destinationPath = build_path(G_directory,G_fileList[i]);
         fsErase(destinationPath,&saveError,False);
         if ( saveError != 0 )
         {
            /* removal failed */
            stat(destinationPath,&buf);

            if ( S_ISDIR(buf.st_mode) && i != 1 )
            {
               sprintf(msg,
                       getVariableMessage(vm_multiCannotDeleteFolderMany, COPY_FILE),
                       destinationPath, i);
            }
            else if ( S_ISDIR(buf.st_mode) && i == 1)
            {
               sprintf(msg,
                       getVariableMessage(vm_multiCannotDeleteFolderOne, COPY_FILE),
                       destinationPath);
            }
            else if ( ! S_ISDIR(buf.st_mode) && i != 1 )
            {
               sprintf(msg,
                       getVariableMessage(vm_multiCannotDeleteFileMany, COPY_FILE),
                       destinationPath, i);
            }
            else  /*  ! S_ISDIR(buf.st_mode) && i == 1 ) */
            {
               sprintf(msg,
                       getVariableMessage(vm_multiCannotDeleteFileOne, COPY_FILE),
                       destinationPath);
            }
            _DtMessageDialog (G_dialogShell, title, msg, 0, FALSE, NULL,
                              multiReplaceError_ok_callback, NULL, NULL, False, ERROR_DIALOG);
            XtFree(destinationPath);
            return;
         }


         /* send return code through the pipe to the background proc */
         rc = PIPEMSG_REPLACE_BUFFER;
	 write(G_filop_confirm_fd, &rc, sizeof(int));

         XtFree(destinationPath);
      }
      XtFree(title);
   } /* end else */


   /* close the dialog */
   XtDestroyWidget(G_dialogShell);
   G_dialogShell = NULL;

   /* de-allocate the file list */
   for ( i = 0; i < G_nCollisions; i++)
      XtFree((char *)G_fileList[i]);
   XtFree((char *)G_fileList);
   G_fileList = NULL;

   G_filop_confirm_fd = -1;

}  /*  end multicollide_ok_callback */



static void
replaceError_ok_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
  XtDestroyWidget((Widget)client_data);

  return;
}  /* end replaceError_ok_callback */



static void
multiReplaceError_ok_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
  XtDestroyWidget((Widget)client_data);

  return;
}  /* end multiReplaceError_ok_callback */



static void
multiRenameError_ok_callback(
	Widget w,
	XtPointer client_data,
	XtPointer call_data)
{
  XtDestroyWidget((Widget)client_data);

  return;
}  /* end multiRenameError_ok_callback */



/*--------------------------------------------------------------------
 * create replace/rename dialog
 *------------------------------------------------------------------*/

void
create_replace_rename_dialog(Widget         parent_widget,
                             int            mode,
                             String         directory,
                             String         file,
                             int            filop_confirm_fd,
                             ActionAreaDefn actions,
 			     Boolean        Same)
{
  Arg args[10];
  int n;
  Widget form;
  Widget icon;
  Widget action_label;
  Widget msg_label;
  Widget radio;
  Widget radioButton1;
  Widget actionArea;
  Widget separator;
  Pixmap px;
  Pixel background, foreground;
  char     path[MAX_PATH], newDir[MAX_PATH], newFile[MAX_PATH];
  String   s;
  XmString xs;
  char *title;
  char *orig_label;
  int orig_msg_num;
  Boolean set;

  if( G_dialogShell )
  {
     XBell( XtDisplay( G_dialogShell ), 100 );
     return;
  }

  /* make args available to callback functions */
  G_filop_confirm_fd = filop_confirm_fd;
  G_mode             = mode;
  strcpy(G_directory,directory);
  strcpy(G_file,file);

  /* Initialize images for error, warning, ... icons */
  if (G_initImages)
  {
     ImageInitialize(XtDisplay(toplevel));
     G_initImages = False;
  }

  /* Initialize mode-dependent things */
  n = 0;
  orig_label = actions.actionList[1].label;
  orig_msg_num = actions.actionList[1].msg_num;
  switch (mode) {
     case MOVE_FILE:
        title = XtNewString(GETMESSAGE(9, 34, "File Manager - Move Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        actions.actionList[1].label   = XtNewString(GETMESSAGE(9, 30, "Cancel Move"));
        actions.actionList[1].msg_num = 30;
        break;
     case COPY_FILE:
        title = XtNewString(GETMESSAGE(9, 35, "File Manager - Copy Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        actions.actionList[1].label   = XtNewString(GETMESSAGE(9, 31, "Cancel Copy"));
        actions.actionList[1].msg_num = 31;
        break;
     case LINK_FILE:
        title = XtNewString(GETMESSAGE(9, 36, "File Manager - Link Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        actions.actionList[1].label   = XtNewString(GETMESSAGE(9, 32, "Cancel Link"));
        actions.actionList[1].msg_num = 32;
        break;
     default:
        title = XtNewString(GETMESSAGE(9, 33, "File Manager - Move/Copy/Link Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        break;
  }


  /* Create the dialog shell for the dialog */
  G_dialogShell = XmCreateDialogShell (parent_widget, "replace/rename dialog", args, n);


  /* Create the Manager Widget, form, for the copy dialog */
  form  = XtVaCreateWidget ("dialog_form",
  			     xmFormWidgetClass,  G_dialogShell,
                             XmNverticalSpacing,   VERTICAL_SPACING,
                             XmNhorizontalSpacing, HORIZONTAL_SPACING,
                             XmNdialogStyle,       XmDIALOG_FULL_APPLICATION_MODAL,
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
                             xmLabelWidgetClass, form,
                             XmNlabelType,        XmPIXMAP,
                             XmNlabelPixmap,      px,
                             XmNtopAttachment,    XmATTACH_FORM,
                             XmNleftAttachment,   XmATTACH_FORM,
                             NULL);

  /* Create the message label */
  s = XtMalloc( 1
                + strlen( GETMESSAGE(9, 16, "An object named\n%s\nalready exists inside the folder\n%s") )
                + strlen( file )
                + strlen( directory ) );
  sprintf( s,
           GETMESSAGE(9, 16, "An object named\n%s\nalready exists inside the folder\n%s"),
           file,
           directory );

  xs = XmStringCreateLocalized(s);
  msg_label = XtVaCreateManagedWidget("msg_header",
                             xmLabelWidgetClass, form,
                             XmNlabelString,      xs,
                             XmNalignment,        XmALIGNMENT_BEGINNING,
                             XmNtopAttachment,    XmATTACH_FORM,
                             XmNleftAttachment,   XmATTACH_WIDGET,
                             XmNleftWidget,       icon,
                             XmNleftOffset,       HORIZONTAL_SPACING,
                             XmNrightAttachment,  XmATTACH_FORM,
                             NULL);
  XmStringFree(xs);
  XtFree(s);


  /* create the action-question */
  xs = XmStringCreateLocalized(GETMESSAGE(9, 20, "What do you want to do?"));
  action_label = XtVaCreateManagedWidget("action-question",
                             xmLabelWidgetClass, form,
                             XmNlabelString,      xs,
                             XmNalignment,        XmALIGNMENT_BEGINNING,
                             XmNtopAttachment,    XmATTACH_WIDGET,
                             XmNtopWidget,        msg_label,
                             XmNtopOffset,        3*VERTICAL_SPACING,
                             XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET,
                             XmNleftWidget,       msg_label,
                             XmNleftOffset,       0,
                             NULL);
  XmStringFree(xs);

  /* create radio box */

  n = 0;
  XtSetArg (args[n], XmNorientation,      XmVERTICAL);               n++;
  XtSetArg (args[n], XmNtopAttachment,    XmATTACH_WIDGET);          n++;
  XtSetArg (args[n], XmNtopWidget,        action_label);             n++;
  XtSetArg (args[n], XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget,       action_label);             n++;
  XtSetArg (args[n], XmNleftOffset,       2*HORIZONTAL_SPACING);     n++;
  radio = (Widget) XmCreateRadioBox(form, "radio_box", args, n);

  if( mode == MAKE_BUFFER )
    set = FALSE;
  else
  {
      /* if the object is have the operation done to the same folder (i.e.
         being done to itself, we want the default to be to create a new name
         for the new object.
         */
      if(Same)
          set = FALSE;
      else
    set = TRUE;
  }

  /* create the radio buttons ... default button is replace */
  xs = XmStringCreateLocalized(GETMESSAGE(9, 21, "Replace existing object"));
  radioButton1 = XtVaCreateManagedWidget("radio_toggle1",
                                         xmToggleButtonWidgetClass, radio,
                                         XmNalignment,   XmALIGNMENT_BEGINNING,
                                         XmNlabelString, xs,
                                         XmNset,         set,
                                         NULL);
  XmStringFree(xs);
  if(!Same)
  {
      XtSetSensitive(radioButton1, True);
      XtAddCallback(radioButton1, XmNvalueChangedCallback, replace_rename_radio_callback, (XtPointer) RADIO_REPLACE);
  }
  else
      XtSetSensitive(radioButton1, False);

  if( mode == MAKE_BUFFER )
  {
    xs = XmStringCreateLocalized(GETMESSAGE(10,48, "New Object Name:"));
    set = TRUE;
  }
  else
  {
      if(Same)
      {
          xs = XmStringCreateLocalized(GETMESSAGE(9, 140, "Rename new object to:"));
          set = TRUE;
      }
      else
      {
          xs = XmStringCreateLocalized(GETMESSAGE(9, 22, "Rename existing object to:"));
          set = FALSE;
      }
  }
  G_rename_toggle = XtVaCreateManagedWidget("radio_toggle2",
                                            xmToggleButtonWidgetClass, radio,
                                            XmNalignment,   XmALIGNMENT_BEGINNING,
                                            XmNlabelString, xs,
                                            XmNset,         set,
                                            NULL);
  XmStringFree(xs);
  XtAddCallback(G_rename_toggle, XmNvalueChangedCallback, replace_rename_radio_callback, (XtPointer) RADIO_RENAME);

  XtManageChild(radio);


  /* create text field for entering a new file name */
  /* a name is generated to use as the default */
  _DtBuildPath(path,directory,file);
  generate_NewPath(path,path);
  split_path(path, newDir, newFile);
  G_rename_text = XtVaCreateManagedWidget("rename_text",
                                       xmTextFieldWidgetClass, form,
                                       XmNrightAttachment,  XmATTACH_FORM,
                                       XmNleftAttachment,   XmATTACH_WIDGET,
                                       XmNleftWidget,       radio,
                                       XmNleftOffset,       HORIZONTAL_SPACING,
                                       XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                                       XmNbottomWidget,     radio,
                                       XmNsensitive,        set,
                                       XmNvalue,            newFile,
                                       XmNcursorPosition,   strlen(newFile),
                                       NULL);

  /* create the action area  */
  actionArea = Create_Action_Area(form, actions,  NULL);
  separator  =  XtVaCreateManagedWidget("separator",
                                        xmSeparatorWidgetClass, form,
                                        XmNtopAttachment,     XmATTACH_WIDGET,
                                        XmNtopWidget,         radio,
                                        XmNbottomAttachment,  XmATTACH_WIDGET,
                                        XmNbottomWidget,      actionArea,
                                        XmNleftAttachment,    XmATTACH_FORM,
                                        XmNrightAttachment,   XmATTACH_FORM,
                                        NULL);


  /* set initial keyborad focus to the action button area */
  XtVaSetValues(form,
                XmNinitialFocus, actionArea,
                NULL);

  /* Manage the dialog */
  XtManageChild(form);


  XtFree(title);
  XtFree(actions.actionList[1].label);
  actions.actionList[1].label = orig_label;
  actions.actionList[1].msg_num = orig_msg_num;
  return;

}  /* end create_replace_rename_dialog */



/*--------------------------------------------------------------------
 * create replace/merge dialog
 *------------------------------------------------------------------*/

void
create_replace_merge_dialog(Widget parent_widget,
                             int            mode,
                             String         directory,
                             String         file,
                             int            filop_confirm_fd,
                             ActionAreaDefn actions)
{
  Arg args[10];
  int n;
  Widget form;
  Widget icon;
  Widget action_label;
  Widget msg_label;
  Widget radio;
  Widget radioButton1;
  Widget actionArea;
  Widget separator;
  Pixmap px;
  Pixel background, foreground;
  char     path[MAX_PATH], newDir[MAX_PATH], newFile[MAX_PATH];
  String   s;
  XmString xs;
  char *title;
  char *orig_label;
  int orig_msg_num;

  if( G_dialogShell )
  {
     XBell( XtDisplay( G_dialogShell ), 100 );
     return;
  }

  /* make args available to callback functions */
  G_filop_confirm_fd = filop_confirm_fd;
  G_mode             = mode;
  strcpy(G_directory,directory);
  strcpy(G_file,file);

  /* Initialize images for error, warning, ... icons */
  if (G_initImages)
  {
     ImageInitialize(XtDisplay(toplevel));
     G_initImages = False;
  }

  /* Initialize mode-dependent things */
  n = 0;
  orig_label = actions.actionList[1].label;
  orig_msg_num = actions.actionList[1].msg_num;
  switch (mode) {
     case MOVE_FILE:
        title = XtNewString(GETMESSAGE(9, 34, "File Manager - Move Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        actions.actionList[1].label   = XtNewString(GETMESSAGE(9, 30, "Cancel Move"));
        actions.actionList[1].msg_num = 30;
        break;
     case COPY_FILE:
        title = XtNewString(GETMESSAGE(9, 35, "File Manager - Copy Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        actions.actionList[1].label   = XtNewString(GETMESSAGE(9, 31, "Cancel Copy"));
        actions.actionList[1].msg_num = 31;
        break;
     case LINK_FILE:
        title = XtNewString(GETMESSAGE(9, 36, "File Manager - Link Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        actions.actionList[1].label   = XtNewString(GETMESSAGE(9, 32, "Cancel Link"));
        actions.actionList[1].msg_num = 32;
        break;
     default:
        title = XtNewString(GETMESSAGE(9, 33, "File Manager - Move/Copy/Link Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        break;
  }


  /* Create the dialog shell for the dialog */
  G_dialogShell = XmCreateDialogShell (parent_widget, "replace/merge dialog", args, n);


  /* Create the Manager Widget, form, for the copy dialog */
  form  = XtVaCreateWidget ("dialog_form",
  			     xmFormWidgetClass,  G_dialogShell,
                             XmNverticalSpacing,   VERTICAL_SPACING,
                             XmNhorizontalSpacing, HORIZONTAL_SPACING,
                             XmNdialogStyle,       XmDIALOG_FULL_APPLICATION_MODAL,
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
                             xmLabelWidgetClass, form,
                             XmNlabelType,        XmPIXMAP,
                             XmNlabelPixmap,      px,
                             XmNtopAttachment,    XmATTACH_FORM,
                             XmNleftAttachment,   XmATTACH_FORM,
                             NULL);

  /* Create the message label */
  s = XtMalloc( 1
                + strlen( GETMESSAGE(9, 17, "A folder named\n%s\nalready exists inside the folder\n%s") )
                + strlen( file )
                + strlen( directory ) );

  sprintf(s, GETMESSAGE(9, 17, "A folder named\n%s\nalready exists inside the folder\n%s"), file, directory);

  xs = XmStringCreateLocalized(s);
  msg_label = XtVaCreateManagedWidget("msg_header",
                             xmLabelWidgetClass, form,
                             XmNlabelString,      xs,
                             XmNalignment,        XmALIGNMENT_BEGINNING,
                             XmNtopAttachment,    XmATTACH_FORM,
                             XmNleftAttachment,   XmATTACH_WIDGET,
                             XmNleftWidget,       icon,
                             XmNleftOffset,       HORIZONTAL_SPACING,
                             XmNrightAttachment,  XmATTACH_FORM,
                             NULL);
  XmStringFree(xs);
  XtFree(s);


  /* create the action-question */
  xs = XmStringCreateLocalized(GETMESSAGE(9, 20, "What do you want to do?"));
  action_label = XtVaCreateManagedWidget("action-question",
                             xmLabelWidgetClass, form,
                             XmNlabelString,      xs,
                             XmNalignment,        XmALIGNMENT_BEGINNING,
                             XmNtopAttachment,    XmATTACH_WIDGET,
                             XmNtopWidget,        msg_label,
                             XmNtopOffset,        3*VERTICAL_SPACING,
                             XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET,
                             XmNleftWidget,       msg_label,
                             XmNleftOffset,       0,
                             NULL);
  XmStringFree(xs);

  /* create radio box */

  n = 0;
  XtSetArg (args[n], XmNorientation,      XmVERTICAL);               n++;
  XtSetArg (args[n], XmNtopAttachment,    XmATTACH_WIDGET);          n++;
  XtSetArg (args[n], XmNtopWidget,        action_label);             n++;
  XtSetArg (args[n], XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget,       action_label);             n++;
  XtSetArg (args[n], XmNleftOffset,       2*HORIZONTAL_SPACING);     n++;
  radio = (Widget) XmCreateRadioBox(form, "radio_box", args, n);

  /* create the radio buttons ... default button is replace */
  xs = XmStringCreateLocalized(GETMESSAGE(9, 25, "Replace existing folder"));
  radioButton1 = XtVaCreateManagedWidget("radio_toggle1",
                                         xmToggleButtonWidgetClass, radio,
                                         XmNalignment,   XmALIGNMENT_BEGINNING,
                                         XmNlabelString, xs,
                                         XmNset,         TRUE,
                                         NULL);
  XmStringFree(xs);

  xs = XmStringCreateLocalized(GETMESSAGE(9, 24, "Merge contents of the two folders"));
  G_merge_toggle = XtVaCreateManagedWidget("radio_toggle2",
                                            xmToggleButtonWidgetClass, radio,
                                            XmNalignment,   XmALIGNMENT_BEGINNING,
                                            XmNlabelString, xs,
                                            NULL);
  XmStringFree(xs);

  XtManageChild(radio);


  /* create the action area  */
  actionArea = Create_Action_Area(form, actions, NULL);
  separator  =  XtVaCreateManagedWidget("separator",
                                        xmSeparatorWidgetClass, form,
                                        XmNtopAttachment,     XmATTACH_WIDGET,
                                        XmNtopWidget,         radio,
                                        XmNbottomAttachment,  XmATTACH_WIDGET,
                                        XmNbottomWidget,      actionArea,
                                        XmNleftAttachment,    XmATTACH_FORM,
                                        XmNrightAttachment,   XmATTACH_FORM,
                                        NULL);


  /* set initial keyborad focus to the action button area */
  XtVaSetValues(form,
                XmNinitialFocus, actionArea,
                NULL);

  /* Manage the dialog */
  XtManageChild(form);


  XtFree(title);
  XtFree(actions.actionList[1].label);
  actions.actionList[1].label = orig_label;
  actions.actionList[1].msg_num = orig_msg_num;
  return;

}  /* end create_replace_merge_dialog */




/*--------------------------------------------------------------------
 * create multicollide dialog
 *------------------------------------------------------------------*/

void
create_multicollide_dialog(Widget           parent_widget,
                           int              mode,
                           int              nSelected,
                           int              nCollisions,
                           String           destination,
                           String         * fileList,
                           int              filop_confirm_fd,
                           ActionAreaDefn   actions,
 			   Boolean          Same)
{
  Arg args[10];
  int n;
  Widget form;
  Widget icon;
  Widget action_label;
  Widget msg_label;
  Widget radio;
  Widget radioButton1;
  Widget actionArea;
  Widget separator;
  Pixmap px;
  Pixel background, foreground;
  String   s, s1, eMsgOne, eMsgMany;
  int nChanged;
  XmString xs;
  char *title;
  char *orig_label;
  int orig_msg_num;
  Boolean set;

  if( G_dialogShell )
  {
     XBell( XtDisplay( G_dialogShell ), 100 );
     return;
  }

  /* make args available to callback functions */
  G_filop_confirm_fd = filop_confirm_fd;
  strcpy(G_directory,destination);
  G_fileList    = fileList;
  G_nCollisions = nCollisions;
  G_mode        = mode;

  /* Initialize images for error, warning, ... icons */
  if (G_initImages)
  {
     ImageInitialize(XtDisplay(toplevel));
     G_initImages = False;
  }

  /* Initialize mode-dependent things */
  n = 0;
  orig_label = actions.actionList[1].label;
  orig_msg_num = actions.actionList[1].msg_num;
  switch (mode) {
     case MOVE_FILE:
        title = XtNewString(GETMESSAGE(9, 34, "File Manager - Move Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        actions.actionList[1].label   =
           XtNewString(GETMESSAGE(9, 30, "Cancel Move"));
        actions.actionList[1].msg_num = 30;
        eMsgOne  = XtNewString(GETMESSAGE(9, 127, "1 object moved."));
        eMsgMany = XtNewString(GETMESSAGE(9, 128, "%d objects moved."));
        break;
     case COPY_FILE:
        title = XtNewString(GETMESSAGE(9, 35, "File Manager - Copy Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        actions.actionList[1].label   =
           XtNewString(GETMESSAGE(9, 31, "Cancel Copy"));
        actions.actionList[1].msg_num = 31;
        eMsgOne  = XtNewString(GETMESSAGE(9, 129, "1 object copied."));
        eMsgMany = XtNewString(GETMESSAGE(9, 130, "%d objects copied."));
        break;
     case LINK_FILE:
        title = XtNewString(GETMESSAGE(9, 36, "File Manager - Link Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        actions.actionList[1].label   =
           XtNewString(GETMESSAGE(9, 32, "Cancel Link"));
        actions.actionList[1].msg_num = 32;
        eMsgOne  = XtNewString(GETMESSAGE(9, 131, "1 object linked."));
        eMsgMany = XtNewString(GETMESSAGE(9, 132, "%d objects linked."));
        break;
     default:
        title = XtNewString(GETMESSAGE(9, 33, "File Manager - Move/Copy/Link Warning"));
        XtSetArg (args[n], XmNtitle, title);
        n++;
        eMsgOne  = XtNewString(GETMESSAGE(9, 133, "1 object changed."));
        eMsgMany = XtNewString(GETMESSAGE(9, 134, "%d objects changed."));
        break;
  }


  /* Create the dialog shell for the dialog */
  G_dialogShell = XmCreateDialogShell (parent_widget, "multicollide dialog", args, n);


  /* Create the Manager Widget, form, for the copy dialog */
  form  = XtVaCreateWidget ("dialog_form",
  			     xmFormWidgetClass,  G_dialogShell,
                             XmNverticalSpacing,   VERTICAL_SPACING,
                             XmNhorizontalSpacing, HORIZONTAL_SPACING,
                             XmNdialogStyle,       XmDIALOG_FULL_APPLICATION_MODAL,
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
                             xmLabelWidgetClass, form,
                             XmNlabelType,        XmPIXMAP,
                             XmNlabelPixmap,      px,
                             XmNtopAttachment,    XmATTACH_FORM,
                             XmNleftAttachment,   XmATTACH_FORM,
                             NULL);

  /* Create the message label */

  nChanged = (nSelected - nCollisions);
  if ( nChanged == 0 )
    /* This is really stupid but Sun requires it
       For now just get it work.
     */
    s1 = (char *)XtCalloc( 1, 1 );
  else if ( nChanged == 1 )
  {
    s1 = eMsgOne;
  }
  else
  {
    s1 = (char *)XtMalloc( 20 + strlen(eMsgMany) );

    sprintf(s1, eMsgMany, nChanged);
  }

  if (nCollisions > 1)
  {
    int tmp_len = (s1)?strlen(s1):0;
    char *tmpmsg,*filemsg=NULL;
    int i;

    filemsg = XtMalloc(strlen(destination)+3);
    sprintf(filemsg,"%s\n",destination);
    for(i = 0;i < nCollisions; i++)
    {
       if(i > 7)
       {
	 char *addmsg;
	 tmpmsg = GETMESSAGE(27,98, "(Plus %d additional object(s))");
	 addmsg = XtMalloc(strlen(tmpmsg)+10);
	 sprintf(addmsg,tmpmsg,nCollisions-8);
         filemsg = XtRealloc(filemsg,strlen(filemsg)+strlen(addmsg)+4);
	 strcat(filemsg,"\n\n");
	 strcat(filemsg,addmsg);
	 XtFree(addmsg);
	 break;
       }
       filemsg = XtRealloc(filemsg,strlen(filemsg)+strlen(destination)+
                      strlen(fileList[i])+3);
       strcat(filemsg,"\n");
       strcat(filemsg,destination);
       strcat(filemsg,"/");
       strcat(filemsg,fileList[i]);
    }
    filemsg = XtRealloc(filemsg,strlen(filemsg)+2);

    tmpmsg = GETMESSAGE(9, 70,
              "%s\n%d objects have the same name as objects in the folder\n%s");
    s = (char *)XtMalloc( tmp_len + strlen(tmpmsg) + strlen(filemsg) + 10);
    sprintf(s,tmpmsg,s1, nCollisions, filemsg);
    XtFree(filemsg);
  }
  else
  {
    int tmp_len = (s1)?strlen(s1):0;
    s = (char *)XtMalloc( tmp_len + 10
                          + strlen( GETMESSAGE(9, 71, "%s\n1 object has the same name as an object in the folder\n%s") )
                          + strlen( destination ) );
    sprintf(s, GETMESSAGE(9, 71, "%s\n1 object has the same name as an object in the folder\n%s"), s1, destination);
  }

  xs = XmStringCreateLocalized(s);
  msg_label = XtVaCreateManagedWidget("msg_header",
                             xmLabelWidgetClass, form,
                             XmNlabelString,      xs,
                             XmNalignment,        XmALIGNMENT_BEGINNING,
                             XmNtopAttachment,    XmATTACH_FORM,
                             XmNleftAttachment,   XmATTACH_WIDGET,
                             XmNleftWidget,       icon,
                             XmNleftOffset,       HORIZONTAL_SPACING,
                             XmNrightAttachment,  XmATTACH_FORM,
                             NULL);
  XmStringFree(xs);
  XtFree(s);
  XtFree(s1);
  XtFree(eMsgOne);
  XtFree(eMsgMany);


  /* create the action-question */
  xs = XmStringCreateLocalized(GETMESSAGE(9, 20, "What do you want to do?"));
  action_label = XtVaCreateManagedWidget("action-question",
                             xmLabelWidgetClass, form,
                             XmNlabelString,      xs,
                             XmNalignment,        XmALIGNMENT_BEGINNING,
                             XmNtopAttachment,    XmATTACH_WIDGET,
                             XmNtopWidget,        msg_label,
                             XmNtopOffset,        3*VERTICAL_SPACING,
                             XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET,
                             XmNleftWidget,       msg_label,
                             XmNleftOffset,       0,
                             NULL);
  XmStringFree(xs);

  /* create radio box */

  n = 0;
  XtSetArg (args[n], XmNorientation,      XmVERTICAL);               n++;
  XtSetArg (args[n], XmNtopAttachment,    XmATTACH_WIDGET);          n++;
  XtSetArg (args[n], XmNtopWidget,        action_label);             n++;
  XtSetArg (args[n], XmNleftAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget,       action_label);             n++;
  XtSetArg (args[n], XmNleftOffset,       2*HORIZONTAL_SPACING);     n++;
  radio = (Widget) XmCreateRadioBox(form, "radio_box", args, n);

  /* create the radio buttons ... default button is replace */
  xs = XmStringCreateLocalized(GETMESSAGE(9, 56, "Replace existing objects"));

  if(Same)
      set = FALSE;
  else
      set = TRUE;

  radioButton1 = XtVaCreateManagedWidget("radio_toggle1",
                                         xmToggleButtonWidgetClass, radio,
                                         XmNalignment,   XmALIGNMENT_BEGINNING,
                                         XmNlabelString, xs,
                                         XmNset,         set,
                                         NULL);
  XmStringFree(xs);
  if(Same)
  {
      XtSetSensitive(radioButton1, False);
      set = TRUE;
      xs = XmStringCreateLocalized(GETMESSAGE(9, 141, "Rename new objects by appending a number"));
  }
  else
  {
      XtSetSensitive(radioButton1, True);
      set = FALSE;
      xs = XmStringCreateLocalized(GETMESSAGE(9, 57, "Rename existing objects by appending a number"));
  }

  G_multi_rename_toggle = XtVaCreateManagedWidget("radio_toggle2",
                                                  xmToggleButtonWidgetClass, radio,
                                                  XmNalignment,   XmALIGNMENT_BEGINNING,
                                                  XmNset,         set,
                                                  XmNlabelString, xs,
                                                  NULL);
  XmStringFree(xs);

  XtManageChild(radio);


  /* create the action area  */
  actionArea = Create_Action_Area(form, actions, NULL);
  separator  =  XtVaCreateManagedWidget("separator",
                                        xmSeparatorWidgetClass, form,
                                        XmNtopAttachment,     XmATTACH_WIDGET,
                                        XmNtopWidget,         radio,
                                        XmNbottomAttachment,  XmATTACH_WIDGET,
                                        XmNbottomWidget,      actionArea,
                                        XmNleftAttachment,    XmATTACH_FORM,
                                        XmNrightAttachment,   XmATTACH_FORM,
                                        NULL);


  /* set initial keyborad focus to the action button area */
  XtVaSetValues(form,
                XmNinitialFocus, actionArea,
                NULL);

  /* Manage the dialog */
  XtManageChild(form);


  XtFree(title);
  XtFree(actions.actionList[1].label);
  actions.actionList[1].label = orig_label;
  actions.actionList[1].msg_num = orig_msg_num;
  return;

}  /* end create_multicollide_dialog */



/*--------------------------------------------------------------------
 *
 * Create Action Area
 *
 * Use of this function requires #include "sharedFuncs.h"
 * The source for Create_Action_Area is in dtfile/OverWrite.c and
 * dtfile/dtcopy/utils.c. Because it uses GETMESSAGE, it could not
 * be placed in dtcopy/sharedFunc.c.
 *
 *------------------------------------------------------------------*/

Widget
Create_Action_Area(
	Widget parent_widget,
	ActionAreaDefn actions,
	Widget *pushbutton_array)
{
  Widget action_area, widget;
  int i;
  int fractbase_value;
  XmString xm_string;


  if (actions.defaultAction < 0  ||  actions.defaultAction > actions.numActions-1)
     actions.defaultAction = 0;

  fractbase_value = (TIGHTNESS * actions.numActions) - 1;
  action_area = XtVaCreateWidget("action_area", xmFormWidgetClass ,parent_widget,
                                XmNfractionBase, fractbase_value,
                                XmNleftAttachment, XmATTACH_FORM,
                                XmNrightAttachment, XmATTACH_FORM,
                                XmNbottomAttachment, XmATTACH_FORM,
                                NULL);

  for (i=0; i < actions.numActions ;i++ )
    {
      xm_string = XmStringCreateLocalized(GETMESSAGE(actions.actionList[i].msg_set,
                                                actions.actionList[i].msg_num,
                                                actions.actionList[i].label));
      widget = XtVaCreateManagedWidget(actions.actionList[i].label,
               xmPushButtonWidgetClass, action_area,
               XmNleftAttachment,     (i ? XmATTACH_POSITION: XmATTACH_FORM),
               XmNleftPosition,       (TIGHTNESS * i),
               XmNtopAttachment,      XmATTACH_FORM,
               XmNbottomAttachment,   XmATTACH_FORM,
               XmNrightAttachment,
                        ((i != (actions.numActions - 1)) ? XmATTACH_POSITION: XmATTACH_FORM),
               XmNrightPosition,      ((TIGHTNESS*i) + (TIGHTNESS - 1)),
               XmNshowAsDefault,      (i == actions.defaultAction),
               XmNdefaultButtonShadowThickness, 1,
               XmNlabelString,         xm_string,
               NULL);
      XmStringFree(xm_string);



       if (actions.actionList[i].callback)
         {
           XtAddCallback(widget, XmNactivateCallback,
                         actions.actionList[i].callback, actions.actionList[i].data);
         }

       if (i == actions.defaultAction)
         {
            XtArgVal heightptr;
            Dimension height, h;
            XtVaGetValues (action_area, XmNmarginHeight, &heightptr, NULL);
            height = (Dimension)heightptr;
            XtVaGetValues (widget, XmNheight, &heightptr, NULL);
            h = (Dimension)heightptr;

            height +=2 * h;
            XtVaSetValues (action_area,
                           XmNdefaultButton, widget,
                           XmNpaneMaximum,   height,
                           XmNpaneMinimum,   height,
                           NULL);

         }

       if (pushbutton_array != NULL)
           pushbutton_array[i] = widget;

    } /* endfor */

   XtManageChild(action_area);
   return action_area;

}  /* end Create_Action_Area */



/*--------------------------------------------------------------------
 *
 * Get Variable Message
 *
 * Return the appropriate message for the current mode (move, copy,
 * link, or other).
 *
 *------------------------------------------------------------------*/

static char *
getVariableMessage(
        VariableMessage message,
	int             mode)
{
   switch (message)
   {
      case vm_replaceCannotDeleteFolder:
           switch (mode)
           {
              case MOVE_FILE:
                   return GETMESSAGE(9, 95, "Move Canceled - the folder was not moved.\n\n\
The folder you want to replace could not be deleted\n\
because an object inside that folder could not be deleted.\n\
Some other objects inside that folder may have already been\n\
deleted before the move process was halted.\n\n\
The most probable cause of this problem is that you do not\n\
have Write permission for the object that could not be removed.\n\
Change Write permission or manually rename the folder you\n\
are moving so it no longer has the same name.");
                   break;
              case COPY_FILE:
                   return GETMESSAGE(9, 96, "Copy Canceled - the folder was not copied.\n\n\
The folder you want to replace could not be deleted\n\
because an object inside that folder could not be deleted.\n\
Some other objects inside that folder may have already been\n\
deleted before the copy process was halted.\n\n\
The most probable cause of this problem is that you do not\n\
have Write permission for the object that could not be removed.\n\
Change Write permission or manually rename the folder you\n\
are copying so it no longer has the same name.");
                   break;
              case LINK_FILE:
                   return GETMESSAGE(9, 97, "Link Canceled - the folder was not linked.\n\n\
The folder you want to replace could not be deleted\n\
because an object inside that folder could not be deleted.\n\
Some other objects inside that folder may have already been\n\
deleted before the link process was halted.\n\n\
The most probable cause of this problem is that you do not\n\
have Write permission for the object that could not be removed.\n\
Change Write permission or manually rename the folder you\n\
are linking so it no longer has the same name.");
                   break;
              default:
                   return GETMESSAGE(9, 98, "Operation Canceled - the folder was not changed.\n\n\
The folder you want to replace could not be deleted\n\
because an object inside that folder could not be deleted.\n\
Some other objects inside that folder may have already been\n\
deleted before the process was halted.\n\n\
The most probable cause of this problem is that you do not\n\
have Write permission for the object that could not be removed.\n\
Change Write permission or manually rename the folder you\n\
are changing so it no longer has the same name.");
                   break;
           }  /* end switch (mode) */

      case vm_replaceCannotDeleteFile:
           switch (mode)
           {
              case MOVE_FILE:
                   return  GETMESSAGE(9, 99, "Move Canceled\n\n\
%s\n\
cannot be removed.\n\n\
The most probable cause of this problem is that\n\
you do not have Write permission for the file.\n\
Change Write permission or manually rename the\n\
file you are moving so it no longer has the same name.");
                   break;
              case COPY_FILE:
                   return  GETMESSAGE(9, 100, "Copy Canceled\n\n\
%s\n\
cannot be removed.\n\n\
The most probable cause of this problem is that\n\
you do not have Write permission for the file.\n\
Change Write permission or manually rename the\n\
file you are copying so it no longer has the same name.");
                   break;
              case LINK_FILE:
                   return  GETMESSAGE(9, 101, "Link Canceled\n\n\
%s\n\
cannot be removed.\n\n\
The most probable cause of this problem is that\n\
you do not have Write permission for the file.\n\
Change Write permission or manually rename the\n\
file you are linking so it no longer has the same name.");
                   break;
              default:
                   return  GETMESSAGE(9, 102, "Operation Canceled\n\n\
%s\n\
cannot be removed.\n\n\
The most probable cause of this problem is that\n\
you do not have Write permission for the file.\n\
Change Write permission or manually rename the\n\
file you are changing so it no longer has the same name.");
                   break;
           }  /* end switch (mode) */

      case vm_multiCannotRenameMany:
           switch (mode)
           {
              case MOVE_FILE:
                   return GETMESSAGE(9, 103, "Move Canceled - None of the objects were moved.\n\n\
 The following object could not be automatically renamed\n\
%s\n\n\
%d objects were renamed before the move process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are moving so it no longer has the same name.");
                   break;
              case COPY_FILE:
                   return GETMESSAGE(9, 104, "Copy Canceled - None of the objects were copied.\n\n\
 The following object could not be automatically renamed\n\
%s\n\n\
%d objects were renamed before the copy process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are copying so it no longer has the same name.");
                   break;
              case LINK_FILE:
                   return GETMESSAGE(9, 105, "Link Canceled - None of the objects were linked.\n\n\
 The following object could not be automatically renamed\n\
%s\n\n\
%d objects were renamed before the link process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are linking so it no longer has the same name.");
                   break;
              default:
                   return GETMESSAGE(9, 106, "Operation Canceled - None of the objects were changed.\n\n\
 The following object could not be automatically renamed\n\
%s\n\n\
%d objects were renamed before the process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are changing so it no longer has the same name.");
                   break;
           }  /* end switch (mode) */

      case vm_multiCannotRenameOne:
           switch (mode)
           {
              case MOVE_FILE:
                   return GETMESSAGE(9, 107, "Move Canceled - None of the objects were moved.\n\n\
 The following object could not be automatically renamed\n\
%s\n\n\
1 object was renamed before the move process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are moving so it no longer has the same name.");
                   break;
              case COPY_FILE:
                   return GETMESSAGE(9, 108, "Copy Canceled - None of the objects were copied.\n\n\
 The following object could not be automatically renamed\n\
%s\n\n\
1 object was renamed before the copy process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are copying so it no longer has the same name.");
                   break;
              case LINK_FILE:
                   return GETMESSAGE(9, 109, "Link Canceled - None of the objects were linked.\n\n\
 The following object could not be automatically renamed\n\
%s\n\n\
1 object was renamed before the link process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are linking so it no longer has the same name.");
                   break;
              default:
                   return GETMESSAGE(9, 110, "Operation Canceled - None of the objects were changed.\n\n\
 The following object could not be automatically renamed\n\
%s\n\n\
1 object was renamed before the process was halted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above object. Change Write permission or manually\n\
rename the object you are changing so it no longer has the same name.");
                   break;
           }  /* end switch (mode) */

      case vm_multiCannotDeleteFolderMany:
           switch (mode)
           {
              case MOVE_FILE:
                   return GETMESSAGE(9, 111, "Move Canceled - none of the objects were moved.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\n\
Some objects inside this folder may have been deleted\n\
before the move process was halted.\n\n\
%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are moving so it no longer has the same name.");
                   break;
              case COPY_FILE:
                   return GETMESSAGE(9, 112, "Copy Canceled - none of the objects were copied.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\n\
Some objects inside this folder may have been deleted\n\
before the copy process was halted.\n\n\
%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are copying so it no longer has the same name.");
                   break;
              case LINK_FILE:
                   return GETMESSAGE(9, 113, "Link Canceled - none of the objects were linked.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\n\
Some objects inside this folder may have been deleted\n\
before the link process was halted.\n\n\
%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are linking so it no longer has the same name.");
                   break;
              default:
                   return GETMESSAGE(9, 114, "Operation Canceled - none of the objects were changed.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\n\
Some objects inside this folder may have been deleted\n\
before the process was halted.\n\n\
%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are changing so it no longer has the same name.");
                   break;
           }  /* end switch (mode) */

      case vm_multiCannotDeleteFolderOne:
           switch (mode)
           {
              case MOVE_FILE:
                   return GETMESSAGE(9, 115, "Move Canceled - none of the objects were moved.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\n\
Some objects inside this folder may have been deleted\n\
before the move process was halted.\n\n\
1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are moving so it no longer has the same name.");
                   break;
              case COPY_FILE:
                   return GETMESSAGE(9, 116, "Copy Canceled - none of the objects were copied.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\n\
Some objects inside this folder may have been deleted\n\
before the copy process was halted.\n\n\
1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are copying so it no longer has the same name.");
                   break;
              case LINK_FILE:
                   return GETMESSAGE(9, 117, "Link Canceled - none of the objects were linked.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\n\
Some objects inside this folder may have been deleted\n\
before the link process was halted.\n\n\
1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are linking so it no longer has the same name.");
                   break;
              default:
                   return GETMESSAGE(9, 118, "Operation Canceled - none of the objects were changed.\n\n\
A folder that you want to replace could not be deleted.\n\
   Folder: %s\n\n\
Some objects inside this folder may have been deleted\n\
before the process was halted.\n\n\
1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above folder. Change Write permission or manually\n\
rename the folder you are changing so it no longer has the same name.");
                   break;
           }  /* end switch (mode) */

      case vm_multiCannotDeleteFileMany:
           switch (mode)
           {
              case MOVE_FILE:
                   return GETMESSAGE(9, 119, "Move Canceled - none of the objects were moved.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n\
%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are moving so it no longer has the same name.");
                   break;
              case COPY_FILE:
                   return GETMESSAGE(9, 120, "Copy Canceled - none of the objects were copied.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n\
%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are copying so it no longer has the same name.");
                   break;
              case LINK_FILE:
                   return GETMESSAGE(9, 121, "Link Canceled - none of the objects were linked.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n\
%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are linking so it no longer has the same name.");
                   break;
              default:
                   return GETMESSAGE(9, 122, "Operation Canceled - none of the objects were changed.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n\
%d other objects have been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are changing so it no longer has the same name.");
                   break;
           }  /* end switch (mode) */

      case vm_multiCannotDeleteFileOne:
           switch (mode)
           {
              case MOVE_FILE:
                   return GETMESSAGE(9, 123, "Move Canceled - none of the objects were moved.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n\
1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are moving so it no longer has the same name.");
                   break;
              case COPY_FILE:
                   return GETMESSAGE(9, 124, "Copy Canceled - none of the objects were copied.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n\
1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are copying so it no longer has the same name.");
                   break;
              case LINK_FILE:
                   return GETMESSAGE(9, 125, "Link Canceled - none of the objects were linked.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n\
1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are linking so it no longer has the same name.");
                   break;
              default:
                   return GETMESSAGE(9, 126, "Operation Canceled - none of the objects were changed.\n\n\
A file that you want to replace could not be deleted.\n\
    File: %s\n\n\
1 other object has been deleted.\n\n\
The most probable cause of this problem is that you do not have Write\n\
permission for the above file. Change Write permission or manually\n\
rename the file you are changing so it no longer has the same name.");
                   break;
           }  /* end switch (mode) */

   }  /* end switch (message) */

}  /* end getVariableMessage */

static Boolean
IsReplaceable(
   char *name)
{
  struct stat s1;

  if(lstat(name,&s1) < 0)
    return False;
  if (S_ISDIR(s1.st_mode))
  {
    if(!access(name,02|01))
      return True;
    return False;
  }
  else
  {
    if(!access(name,02))
      return True;
    return False;
  }
}
