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
/* $XConsortium: FindP.c /main/5 1996/10/15 10:07:22 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           FindP.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Processing functions for the find file dialog.
 *
 *   FUNCTIONS: FindClose
 *		ShowFindDialog
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>

#include <Dt/FileM.h>
#include "Encaps.h"
#include "SharedProcs.h"

#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Common.h"
#include "Find.h"
#include "SharedMsgs.h"



/********    Static Function Declarations    ********/

static void FindClose(  XtPointer client_data,
                        DialogData *old_dialog_data,
                        DialogData *new_dialog_data) ;

/********    End Static Function Declarations    ********/




/************************************************************************
 *
 *  ShowFindDialog
 *	Callback functions invoked from the Find File ... menu
 *	item.  This function displays the find file dialog.
 *
 ************************************************************************/

void
ShowFindDialog(
        Widget w,
        XtPointer client_data,
        XtPointer callback )
{
   FileMgrRec  * file_mgr_rec;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   FindRec     * find_rec;
   FindData * find_data;
   Arg args[2];
   Widget mbar;
   int i;
   XmString label_string;
   char *tempStr, *tmpStr;

   /*  Set the menu item to insensitive to prevent multiple  */
   /*  dialogs from being posted and get the area under the  */
   /*  menu pane redrawn.                                    */

   if (w)
   {
      if ( (XtArgVal) client_data == FM_POPUP )
        mbar = XtParent(w);
      else
        mbar = XmGetPostedFromWidget(XtParent(w));

      XmUpdateDisplay (w);
      XtSetArg(args[0], XmNuserData, &file_mgr_rec);
      XtGetValues(mbar, args, 1);

      /* Ignore accelerators when we're insensitive */
      if ((file_mgr_rec->menuStates & FIND) == 0)
      {
         XSetInputFocus(XtDisplay(w),
                        XtWindow(file_mgr_rec->findBtn_child),
                        RevertToParent, CurrentTime);
         return;
      }
   }
   else
   {
      /* Done only when restoring a session */
      file_mgr_rec = (FileMgrRec *)client_data;
   }


   /* Ignore accelerators received after we're unposted */
   if ((dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec)) == NULL)
      return;

   file_mgr_data = (FileMgrData *) dialog_data->data;

   file_mgr_rec->menuStates &= ~FIND;

   dialog_data = (DialogData *)file_mgr_data->find;
   find_data = (FindData *)dialog_data->data;
   find_data->file_mgr_data = file_mgr_data;
  
   if(find_data->content != NULL && w != NULL)
   {
      if(strcmp(find_data->content, "") != 0)
         XtFree(find_data->content);
      find_data->content = NULL;
   }

   if(find_data->filter != NULL && w != NULL)
   {
      XtFree(find_data->filter);
      find_data->filter = NULL;
   }

   if(find_data->directories != NULL && w != NULL)
   {
      XtFree(find_data->directories);
      find_data->directories = NULL;
   }

   if(w != NULL)
   {
      for (i = 0; i < find_data->num_matches; i++)
         XtFree (find_data->matches[i]);

      XtFree ((char *) find_data->matches);

      find_data->matches = NULL;
      find_data->num_matches = 0;
      find_data->selected_item = -1;
   }

   _DtShowDialog (file_mgr_rec->shell, NULL,
	       (XtPointer)file_mgr_rec, file_mgr_data->find, 
               NULL, (XtPointer)file_mgr_rec,
	       FindClose, (XtPointer)file_mgr_rec, NULL, False, 
               False, NULL, NULL);

   /* Save a ptr to file_mgr_rec in the find dialogs structure */
   find_rec = (FindRec *)_DtGetDialogInstance(file_mgr_data->find);
   find_rec->fileMgrRec = file_mgr_rec;

   if(file_mgr_data->title != NULL &&
               strcmp(file_mgr_data->helpVol, DTFILE_HELP_NAME) != 0)
   {
      tmpStr = (GETMESSAGE(15,46, "Find"));
      tempStr = (char *)XtMalloc(strlen(tmpStr) +
                                 strlen(file_mgr_data->title) + 5);
      sprintf(tempStr, "%s - %s", file_mgr_data->title, tmpStr);
   }
   else
   {
      tmpStr = (GETMESSAGE(15,49, "File Manager - Find"));
      tempStr = XtNewString(tmpStr);
   }
   XtSetArg (args[0], XmNtitle, tempStr);
   XtSetValues (find_rec->shell, args, 1);
   XtFree(tempStr);
   file_mgr_rec->findBtn_child=find_rec->shell;

#if defined(__hpux) || defined(sun)
   if(file_mgr_data->toolbox)
   {
      XtSetArg (args[0], XmNmenuHistory, find_rec->widgArry[ON]);
      XtSetValues(find_rec->followLink, args, 1);
   }
#endif

   label_string = XmStringCreateLocalized (GetSharedMessage(FILES_FOUND_LABEL));
   XtSetArg (args[0], XmNlabelString, label_string);
   XtSetValues (find_rec->listLabel, args, 1);
   XmStringFree (label_string);
}




/************************************************************************
 *
 *  FindClose
 *	Callback function invoked from the find file dialog's close
 *	button.  This function resensitizes the menu item, and saves the
 *      interim dialog values.
 *
 ************************************************************************/

static void
FindClose(
        XtPointer client_data,
        DialogData *old_dialog_data,
        DialogData *new_dialog_data )
{
   FileMgrRec * file_mgr_rec = (FileMgrRec *) client_data;
   XtPointer save_data;


   /* Free up the old interim values, and save the new ones */
   save_data = old_dialog_data->data;
   old_dialog_data->data = new_dialog_data->data;
   new_dialog_data->data = save_data;
   _DtFreeDialogData (new_dialog_data);


   /* Resensitize the associated menubutton */
   file_mgr_rec->menuStates |= FIND;
}
