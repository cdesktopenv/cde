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
/* $XConsortium: PrefsP.c /main/4 1995/11/02 14:43:38 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           PrefsP.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Processing functions for the preferences dialog.
 *
 *   FUNCTIONS: PreferencesChange
 *		PreferencesClose
 *		ShowPreferencesDialog
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>

#include <Dt/FileM.h>
#include "Encaps.h"
#include "SharedProcs.h"

#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Prefs.h"


/********    Static Function Declarations    ********/

static void PreferencesChange(
                        XtPointer client_data,
                        DialogData *old_dialog_data,
                        DialogData *new_dialog_data,
                        XtPointer call_data) ;
static void PreferencesClose(
                        XtPointer client_data,
                        DialogData *old_dialog_data,
                        DialogData *new_dialog_data) ;

/********    End Static Function Declarations    ********/


/************************************************************************
 *
 *  ShowPreferencesDialog
 *	Callback functions invoked from the Change Directory... menu
 *	item.  This function displays the change directory dialog.
 *
 ************************************************************************/

void
ShowPreferencesDialog(
        Widget w,
        XtPointer client_data,
        XtPointer callback )
{
   FileMgrRec  * file_mgr_rec;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   PreferencesRec *preferences_rec;
   Arg args[1];
   Widget mbar;
   char *tmpStr, *tempStr;



   /*  Set the menu item to insensitive to prevent multiple  */
   /*  dialogs from being posted and get the area under the  */
   /*  menu pane redrawn.                                    */

   if (w)
   {
      mbar = XmGetPostedFromWidget(XtParent(w));
      XmUpdateDisplay (w);
      XtSetArg(args[0], XmNuserData, &file_mgr_rec);
      XtGetValues(mbar, args, 1);

      /* Ignore accelerators when we're insensitive */
      if ((file_mgr_rec->menuStates & PREFERENCES) == 0)
      {
         XSetInputFocus(XtDisplay(w),
                        XtWindow(file_mgr_rec->preferencesBtn_child),
                        RevertToParent, CurrentTime);
         return;
      }
   }
   else
   {
      /* Done only during a restore session */
      file_mgr_rec = (FileMgrRec *)client_data;
   }


   /* Ignore accelerators received after we're unposted */
   if ((dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec)) == NULL)
      return;
   file_mgr_data = (FileMgrData *) dialog_data->data;

   file_mgr_rec->menuStates &= ~ PREFERENCES;

   _DtShowDialog (file_mgr_rec->shell, NULL, (XtPointer)file_mgr_rec, 
               file_mgr_data->preferences, PreferencesChange,
	       (XtPointer)file_mgr_rec, PreferencesClose,
	       (XtPointer)file_mgr_rec, NULL, False, False, NULL, NULL);

   /* set the title */
   preferences_rec = (PreferencesRec *)_DtGetDialogInstance(
                                        file_mgr_data->preferences);

   if(file_mgr_data->title != NULL &&
           (strcmp(file_mgr_data->helpVol, DTFILE_HELP_NAME) != 0 ||
            strcmp(file_mgr_data->title, "Trash Can") == 0))
   {
      tmpStr = GETMESSAGE(23, 1, "Set View Options");
      tempStr = (char *)XtMalloc(strlen(tmpStr) +
                                 strlen(file_mgr_data->title) + 5);
      sprintf(tempStr, "%s - %s", file_mgr_data->title, tmpStr);
   }
   else
   {
      tmpStr = (GETMESSAGE(23,22, "File Manager - Set View Options"));
      tempStr = XtNewString(tmpStr);
   }
   XtSetArg (args[0], XmNtitle, tempStr);
   XtSetValues (preferences_rec->shell, args, 1);
   XtFree(tempStr);
   file_mgr_rec->preferencesBtn_child=preferences_rec->shell;
}




/************************************************************************
 *
 *  PreferencesChange
 *	Callback functions invoked from the current directory dialog's
 *	apply button being pressed.  This function updates and redisplays
 *	the current directory information.
 *
 ************************************************************************/

static void
PreferencesChange(
        XtPointer client_data,
        DialogData *old_dialog_data,
        DialogData *new_dialog_data,
        XtPointer call_data )
{
   FileMgrRec  * file_mgr_rec = (FileMgrRec *) client_data;
   DialogData  * dialog_data;
   FileMgrData * file_mgr_data;
   XtPointer save_data;
   PreferencesData * preferences_data;
   unsigned char *viewP;
   FileViewData  *branch;
   int i;

   dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
   file_mgr_data = (FileMgrData *) dialog_data->data;

   save_data = file_mgr_data->preferences->data;
   file_mgr_data->preferences->data = new_dialog_data->data;
   new_dialog_data->data = save_data;
   _DtFreeDialogData (new_dialog_data);


   preferences_data = (PreferencesData *) (file_mgr_data->preferences->data);

   if (file_mgr_data->show_type != preferences_data->show_type	||
       file_mgr_data->tree_files != preferences_data->tree_files ||
       file_mgr_data->view_single != preferences_data->view_single ||
       file_mgr_data->view_tree != preferences_data->view_tree ||
       file_mgr_data->order != preferences_data->order		||
       file_mgr_data->direction != preferences_data->direction  ||
       file_mgr_data->positionEnabled != preferences_data->positionEnabled ||
       file_mgr_data->show_iconic_path != preferences_data->show_iconic_path ||
       file_mgr_data->show_current_dir != preferences_data->show_current_dir ||
       file_mgr_data->show_status_line != preferences_data->show_status_line)
   {
      /*
       * Depending upon which of the preferences values changed, positioning
       * may need to be disabled, and the preferences dialog updated.
       */
      if (((file_mgr_data->order != preferences_data->order) ||
          (file_mgr_data->direction != preferences_data->direction)) &&
          (file_mgr_data->positionEnabled == RANDOM_ON))
      {
         /* Discard positioning info */
         file_mgr_data->grid_height = 0;
         file_mgr_data->grid_width = 0;
         if(file_mgr_data->object_positions)
            FreePositionInfo(file_mgr_data);
      }

      /*
       * In tree mode, if we switch from showing files "always" to "choose",
       * change all branches from tsAll back to tsDirs.
       */
      if (file_mgr_data->tree_files == TREE_FILES_ALWAYS &&
          preferences_data->tree_files == TREE_FILES_CHOOSE)
      {
        for (i = 0; i < file_mgr_data->directory_count; i++)
        {
           branch = file_mgr_data->directory_set[i]->sub_root;
           if (branch->ts == tsAll)
           {
              branch->ts = tsDirs;
              UpdateBranchList(file_mgr_data, file_mgr_data->directory_set[i]);
           }
        }
      }

      if (preferences_data->show_type == MULTIPLE_DIRECTORY)
        viewP = &preferences_data->view_tree;
      else
        viewP = &preferences_data->view_single;
      if (file_mgr_data->view != *viewP)
      {
         file_mgr_data->newSize = True;
      }
      else
      { 
         if (file_mgr_data->show_type != preferences_data->show_type) 
            file_mgr_data->newSize = True;
         else
            file_mgr_data->newSize = False;
      }
      file_mgr_data->show_type = preferences_data->show_type;
      file_mgr_data->tree_files = preferences_data->tree_files;
      file_mgr_data->view = *viewP;
      file_mgr_data->view_single = preferences_data->view_single;
      file_mgr_data->view_tree = preferences_data->view_tree;
      file_mgr_data->order = preferences_data->order;
      file_mgr_data->direction = preferences_data->direction;
      file_mgr_data->show_iconic_path = preferences_data->show_iconic_path;
      file_mgr_data->show_current_dir = preferences_data->show_current_dir;
      file_mgr_data->show_status_line = preferences_data->show_status_line;
      if (file_mgr_data->positionEnabled == preferences_data->positionEnabled)
         FileMgrRedisplayFiles (file_mgr_rec, file_mgr_data, False);
      else
      {
         file_mgr_data->positionEnabled = preferences_data->positionEnabled;
         FileMgrRedisplayFiles (file_mgr_rec, file_mgr_data, True);
      }
   
      if(openDirType == NEW)
         ForceMyIconOpen(file_mgr_data->host, NULL);
   }
}




/************************************************************************
 *
 *  PreferencesClose
 *	Callback function invoked from the preferences dialog's close
 *	button.  This function resensitizes the menu item.
 *
 ************************************************************************/

static void
PreferencesClose(
        XtPointer client_data,
        DialogData *old_dialog_data,
        DialogData *new_dialog_data )
{
   FileMgrRec * file_mgr_rec = (FileMgrRec *) client_data;

   _DtFreeDialogData (new_dialog_data);

   /* Resensitize the associated menu button */
   file_mgr_rec->menuStates |= PREFERENCES;
}
