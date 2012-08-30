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
/* $XConsortium: Command.c /main/9 1996/10/30 11:09:42 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Command.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Command processing functions used by the File Browser.
 *
 *   FUNCTIONS: ActionCallback
 *		InvalidTrashDragDrop
 *		ProcessAction
 *		ProcessBufferDropOnFolder
 *		ProcessMoveCopyLink
 *		ProcessNewView
 *		RunCommand
 *		TimerEvent
 *		UpdateActionMenuPane
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
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/XmP.h>
#include <X11/Shell.h>
#include <X11/Intrinsic.h>

#include <Dt/Action.h>
#include <Dt/ActionP.h>
#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/HourGlass.h>
#include <Dt/Dnd.h>

#include "Encaps.h"
#include "SharedProcs.h"
#include "FileMgr.h"
#include "Desktop.h"
#include "Main.h"
#include "Help.h"
#include "SharedMsgs.h"
#include "Prefs.h"

/********    Static Function Declarations    ********/

static void ActionCallback( Widget w,
                            XtPointer client_data,
                            XtPointer call_data) ;
static void TimerEvent(
                        Widget widget,
                        XtIntervalId *id) ;
void ProcessBufferDropOnFolder (
                                char *command,
                                FileMgrData *file_mgr_data,
                                FileViewData *file_view_data,
                                DtDndDropCallbackStruct *drop_parameters,
                                Widget drop_window);


/********    End Static Function Declarations    ********/

extern int G_dropx,G_dropy;

/************************************************************************
 *
 *  UpdateActionMenuPane
 *      Build up a set of menu panes for the provided file manager rec
 *      that contains as items each of the commands for each file type.
 *
 ************************************************************************/

void
UpdateActionMenuPane(
        XtPointer client_data,
        FileMgrRec *file_mgr_rec,
        char *file_type,
        int type,
        int number,
        Widget widget,
        unsigned char physical_type)
{
   XmManagerWidget action_pane;
   FileMgrData *file_mgr_data = NULL;
   DialogData * dialog_data;
   FileViewData *file_view_data;
   DesktopRec *desktopWindow;
   Widget child;
   register int i, menu_offset;
   register int action_count;
   int count, del_count;
   int num_children;
   char ** command_list;
   XmString string;
   char *action_label;

   Arg args[2];


   if(type == DESKTOP)
      desktopWindow = (DesktopRec *)client_data;
   else if(type == FM_POPUP)
      file_view_data = (FileViewData *)client_data;

   if (file_mgr_rec)
   {
      dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
      file_mgr_data = (FileMgrData *) dialog_data->data;
   }

   /*  Count the number of actions defined for the the file type  */

   action_count = 0;
   command_list = _DtCompileActionVector(file_type);

   if(command_list != NULL)
      while (command_list[action_count] != NULL &&
                                    strlen (command_list[action_count]) != 0)
         action_count++;

   if (physical_type == DtDIRECTORY && type == DESKTOP)
       ++action_count;


   /*  If the menu pane is already set up for the file type then return.  */

   if(type == NOT_DESKTOP)
   {
      XtFree(file_mgr_rec->action_pane_file_type);
      file_mgr_rec->action_pane_file_type = XtNewString(file_type);

      action_pane = (XmManagerWidget) file_mgr_rec->action_pane;
      menu_offset = number + SELECTED_MENU_MAX;
   }
   else
   {
      action_pane = (XmManagerWidget) widget;
      menu_offset = number;
   }

   num_children = action_pane->composite.num_children;

   /*
    * This is so that we can determine which icon was responsible for
    * posting the menu, or requesting help.
    */
   if (type == FM_POPUP)
      file_mgr_data->popup_menu_icon = file_view_data;
/*
   else if (file_mgr_data)
      file_mgr_data->popup_menu_icon = NULL;
*/

   if (action_count + menu_offset > num_children)
   {
      if(type == FM_POPUP)
         for (i = number; i < num_children; i++)
            XtManageChild (action_pane->composite.children[i]);
      for (i = num_children; i < action_count + menu_offset; i++)
      {
         child = XmCreatePushButtonGadget ((Widget)action_pane,
                                            "action_button", args, 0);
         if(type == DESKTOP)
         {
            XtAddCallback (child, XmNactivateCallback, DTActionCallback,
                                                    (XtPointer)desktopWindow);
            XtAddCallback(child, XmNhelpCallback,
                          (XtCallbackProc)DTHelpRequestCB, NULL);
         }
         else if(type == FM_POPUP)
         {
            XtAddCallback (child, XmNactivateCallback, ActionCallback, NULL);
            XtAddCallback(child, XmNhelpCallback,
                          (XtCallbackProc)HelpRequestCB, NULL);
         }
         else
         {
            XtAddCallback (child, XmNactivateCallback, ActionCallback,
                                                    (XtPointer)file_mgr_rec);
            XtAddCallback(child, XmNhelpCallback,
                          (XtCallbackProc)HelpRequestCB, NULL);
         }
         XtManageChild (child);
      }
   }
   else
   {
     for (i = menu_offset; i < num_children; i++)
     {
       if (i < action_count + menu_offset)
       {
          XtRemoveAllCallbacks(action_pane->composite.children[i],
                                                   XmNactivateCallback);
          if(type == DESKTOP)
             XtAddCallback (action_pane->composite.children[i],
                                     XmNactivateCallback, DTActionCallback,
                                     (XtPointer)desktopWindow);
          else if(type == FM_POPUP)
             XtAddCallback (action_pane->composite.children[i],
                                  XmNactivateCallback, ActionCallback, NULL);
          else
             XtAddCallback (action_pane->composite.children[i],
                                        XmNactivateCallback, ActionCallback,
                                        (XtPointer)file_mgr_rec);
          XtManageChild (action_pane->composite.children[i]);
        }
        else
          XtUnmanageChild (action_pane->composite.children[i]);
     }
   }

   /*  For each action, set the label of the menu button  */
   /*  and the user data.                                 */

   num_children =  menu_offset + action_count;
   del_count = 0;
   for (i = 0; i < action_count; i++)
   {
      char *oldCommand;
      Arg argsTmp[2];

      if(type != DESKTOP)
      {
         char* strp;

         strp = XtNewString(command_list[i]);
         XtSetArg (args[1], XmNuserData, strp);

         action_label = DtActionLabel(command_list[i]);

         if(action_label != NULL)
             string = XmStringCreateLocalized(action_label);
         else
             string = XmStringCreateLocalized(command_list[i]);

         count = i + menu_offset;
      }
      else
      {
         if (physical_type == DtDIRECTORY && i == 0)
         {
            if((action_label = DtActionLabel(openNewView)) != NULL)
                string = XmStringCreateLocalized(action_label);
            else
                string = XmStringCreateLocalized(openNewView);
            XtSetArg (args[1], XmNuserData, openNewView);
            count = i + number;
         }
         else
         {
            if (physical_type == DtDIRECTORY)
            {
               if(strcmp(command_list[i - 1], openNewView) == 0  ||
                  strcmp(command_list[i - 1], openInPlace) == 0)
               {
                  del_count++;
                  XtUnmanageChild (action_pane->composite.children[
                                              num_children - del_count]);
                  continue;
               }
               action_label = DtActionLabel(command_list[i - 1]);

               if(action_label != NULL)
                   string = XmStringCreateLocalized(action_label);
               else
                   string = XmStringCreateLocalized(command_list[i-1]);

               XtSetArg (args[1], XmNuserData, XtNewString
                                                 (command_list[i - 1]));
               count = i + number - del_count;
            }
            else
            {
               action_label = DtActionLabel(command_list[i]);

               if(action_label != NULL)
                   string = XmStringCreateLocalized(action_label);
               else
                   string = XmStringCreateLocalized(command_list[i]);

               XtSetArg (args[1], XmNuserData, XtNewString(command_list[i]));
               count = i + number;
            }
         }
      }

      /* first we need to get the userData from the push button and
         free it up */
      XtSetArg (argsTmp[0], XmNuserData, &oldCommand);
      XtGetValues (action_pane->composite.children[count], argsTmp, 1);

      if(oldCommand != NULL && strcmp(oldCommand, openNewView) != 0)
         XtFree(oldCommand);


      XtSetArg (args[0], XmNlabelString, string);
      XtSetValues (action_pane->composite.children[count], args, 2);
      if(type != NOT_DESKTOP)
      {
         XtRemoveAllCallbacks(action_pane->composite.children[count],
                                                   XmNactivateCallback);

         if(type == DESKTOP)
         {
            XtAddCallback (action_pane->composite.children[count],
                           XmNactivateCallback, DTActionCallback,
                           (XtPointer)desktopWindow);
         }
         else
            XtAddCallback (action_pane->composite.children[count],
                           XmNactivateCallback, ActionCallback,
                           (XtPointer)NULL);
      }
      XmStringFree (string);
      XtFree(action_label);
   }

   _DtFreeStringVector(command_list);
}




/************************************************************************
 *
 *  ActionCallback
 *      Callback function invoked upon the an actions menu button
 *      being selected.
 *
 ************************************************************************/

static void
ActionCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
   FileMgrRec   * file_mgr_rec;
   DialogData   * dialog_data;
   FileMgrData  * file_mgr_data;
   FileViewData  * file_view_data;
   char * command;
   Arg args[1];
   Widget mbar;
   Boolean popup = False;
   XmAnyCallbackStruct * callback;


   XmUpdateDisplay (w);
   mbar = XtParent(w);

   callback = (XmAnyCallbackStruct *) call_data;
   if(client_data == NULL)
   {
      popup = True;
      XtSetArg(args[0], XmNuserData, &file_mgr_rec);
      XtGetValues(mbar, args, 1);
      dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
      file_mgr_data = (FileMgrData *) dialog_data->data;
      file_view_data = file_mgr_data->popup_menu_icon;
      if(!file_view_data) /* The object would have probably been delete */
         return;
      file_mgr_data->popup_menu_icon = NULL;  /* Just to make it unuseful */
   }
   else
   {
      file_mgr_rec = (FileMgrRec *) client_data;
      dialog_data = _DtGetInstanceData ((XtPointer)file_mgr_rec);
      file_mgr_data = (FileMgrData *) dialog_data->data;
   }



   /*  Find the file data for the file that is selected  */

   XtSetArg (args[0], XmNuserData, (XtPointer) &command);
   XtGetValues (w, args, 1);

   if(strcmp(command, openNewView) == 0)
   {
      XButtonEvent *event = (XButtonEvent *)callback->event;
      unsigned int modifiers;

      modifiers = event->state;
      if(popup)
         RunCommand (command, file_mgr_data, file_view_data, NULL, NULL, NULL);
      else
         RunCommand (command, file_mgr_data, file_mgr_data->selection_list[0],
                     NULL, NULL, NULL);

      if((modifiers != 0) && ((modifiers & ControlMask) != 0))
      {
         DialogData  *dialog_data;

         dialog_data = _DtGetInstanceData(file_mgr_data->file_mgr_rec);
         CloseView(dialog_data);
      }
   }
   else
   {
      if(popup)
         RunCommand (command, file_mgr_data, file_view_data, NULL, NULL, NULL);
      else
         RunCommand (command, file_mgr_data, file_mgr_data->selection_list[0],
                     NULL, NULL, NULL);
   }
}


/************************************************************************
 *
 *  RunCommand
 *
 *    WARNING: when desktop links are passed in, this function will NOT
 *             expect the links to have already been mapped to their
 *             real files.
 *
 ************************************************************************/

void
RunCommand(
        char *command,
        FileMgrData *file_mgr_data,
        FileViewData *file_view_data,
        WindowPosition *position,
        DtDndDropCallbackStruct *drop_parameters,
        Widget drop_window )

{
   if ((strcmp (command, openInPlace) == 0) ||
       (strcmp (command, openNewView) == 0))
   {
      /* If the folder is locked, don't allow user to go into it */
      if( strcmp( file_view_data->file_data->logical_type, LT_FOLDER_LOCK ) == 0 )
      {
        char *tmpStr, *title, *msg;

        tmpStr = GETMESSAGE(9, 6, "Action Error");
        title = XtNewString(tmpStr);
        msg = (char *)XtMalloc(
                   strlen( GETMESSAGE(30, 1, "Cannot read from %s") )
                 + strlen( file_view_data->file_data->file_name )
                 + 1 );
        sprintf( msg, GETMESSAGE(30, 1, "Cannot read from %s"),
                 file_view_data->file_data->file_name );
        _DtMessage(((FileMgrRec*)file_mgr_data->file_mgr_rec)->file_window,
                   title, msg, NULL, HelpRequestCB );
        XtFree(title);
        XtFree(msg);
        return;
      }

      /* this statement applies to the case where a user traverses down the *
       * part of the directory tree containing the application manager      *
       * directories or the trash directory                                 */
      if( ((strcmp(file_view_data->file_data->logical_type, LT_AGROUP) == 0) &&
           (!(file_mgr_data->toolbox)))
          ||
          (strcmp(file_view_data->file_data->logical_type, LT_TRASH) == 0) )
      {
         ProcessAction(command,
                       file_view_data,
                       drop_parameters,
                       file_mgr_data->host,
                       file_mgr_data->current_directory,
                       file_mgr_data->restricted_directory,
                       ((FileMgrRec *) file_mgr_data->file_mgr_rec)->shell);
      }
      else
      {
         ProcessNewView(command, file_mgr_data, file_view_data, position);
      }
   }

   else if ((strcmp (command, "FILESYSTEM_MOVE") == 0) ||
            (strcmp (command, "FILESYSTEM_COPY") == 0) ||
            (strcmp (command, "FILESYSTEM_LINK") == 0))
   {
      /* Check to see what was dropped (files or buffers) */
      /* Call the appropriate routine to handle the drop  */
      if (drop_parameters->dropData->protocol == DtDND_FILENAME_TRANSFER)
         ProcessMoveCopyLink(command,
                             file_mgr_data,
                             file_view_data,
                             drop_parameters,
                             drop_window);
      else
         if (drop_parameters->dropData->protocol == DtDND_BUFFER_TRANSFER)
           ProcessBufferDropOnFolder(command,
                                     file_mgr_data,
                                     file_view_data,
                                     drop_parameters,
                                     drop_window);

   }

   else
   {
      ProcessAction(command,
                    file_view_data,
                    drop_parameters,
                    file_mgr_data->host,
                    file_mgr_data->current_directory,
                    file_mgr_data->restricted_directory,
                    ((FileMgrRec *) file_mgr_data->file_mgr_rec)->shell);
   }
}


/************************************************************************
 *
 *  ProcessNewView
 *
 ************************************************************************/

void
ProcessNewView (
     char *command,
     FileMgrData *file_mgr_data,
     FileViewData *file_view_data,
     WindowPosition *position)
{
   DirectorySet * directory_set;
   char host_name[MAX_PATH];
   char directory_name[MAX_PATH];
   char *tmpStr, *title, *msg;

   /* we don't want to execute the default action if in trash ...  */
   if( trashFileMgrData != NULL
       && file_mgr_data == trashFileMgrData)
   {
      tmpStr = GETMESSAGE(27, 3, "Trash Can Error");
      title = XtNewString(tmpStr);
      tmpStr = GETMESSAGE(27, 87, "Object in the Trash cannot be opened.\n\nTo open an object use 'Put Back' to return it to the\nFile Manager then open it there.");
      msg = XtNewString(tmpStr);

      _DtMessage( ((FileMgrRec *)file_mgr_data->file_mgr_rec)->file_window,
                  title, msg, NULL, HelpRequestCB);
      XtFree(title);
      XtFree(msg);
      return;
   }

   strcpy (host_name, file_mgr_data->host);

   directory_set = (DirectorySet *) (file_view_data->directory_set);
   strcpy (directory_name, directory_set->name);

   if (strcmp (directory_name, "/") != 0)
      strcat (directory_name, "/");

   strcat (directory_name, file_view_data->file_data->file_name);
   DtEliminateDots (directory_name);

   if (strcmp (directory_name, "/..") == 0)
      strcpy (directory_name, "/");

   if (strcmp (command, openInPlace) == 0)
   {
      FileMgrRec *file_mgr_rec;
      Arg args[1];
      Widget vb;
      int value, size, increment, page;

      file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;
      ShowNewDirectory (file_mgr_data, host_name, directory_name);

      XtSetArg (args[0], XmNverticalScrollBar, &vb);
      XtGetValues (file_mgr_rec->scroll_window, args, 1);

         /* get scroll bar values */
      (void)XmScrollBarGetValues(vb, &value, &size, &increment, &page);

         /* set scroll bar values changing its position */
      if(value != 0)
        (void)XmScrollBarSetValues(vb, (int)0, size, increment, page, True);

      if(strcmp(file_mgr_data->current_directory,
                file_mgr_data->restricted_directory) == 0)
      {
         XtSetSensitive(*upBarBtn, False);
         currentMenuStates &= ~(MOVE_UP);
         file_mgr_rec->menuStates &= ~(MOVE_UP);
      }
      else
      {
         file_mgr_rec->menuStates |= MOVE_UP;
         XtSetSensitive(*upBarBtn, True);
         currentMenuStates &= ~(MOVE_UP);
      }
   }
   else
   {
      initiating_view = (XtPointer) file_mgr_data;
      if(file_mgr_data->restricted_directory == NULL)
      {
         GetNewView (host_name, directory_name, NULL, position, 0);
      }
      else
      {
         special_view = True;
         special_treeType = file_mgr_data->show_type;
         special_viewType = file_mgr_data->view;
         special_orderType = file_mgr_data->order;
         special_directionType = file_mgr_data->direction;
         special_randomType = file_mgr_data->positionEnabled;
         special_restricted =
            XtNewString(file_mgr_data->restricted_directory);
         if(file_mgr_data->title == NULL)
            special_title = NULL;
         else
            special_title = XtNewString(file_mgr_data->title);
         special_helpVol = XtNewString(file_mgr_data->helpVol);
         if(file_mgr_data->toolbox)
            GetNewView (file_mgr_data->host, directory_name,
                        file_mgr_data->restricted_directory, position, 0);
         else
            GetNewView (file_mgr_data->host, directory_name, NULL, position, 0);
      }

      initiating_view = (XtPointer) NULL;
   }
}


/************************************************************************
 *
 *  ProcessMoveCopyLink
 *
 ************************************************************************/

void
ProcessMoveCopyLink (
     char *command,
     FileMgrData *file_mgr_data,
     FileViewData *file_view_data,
     DtDndDropCallbackStruct *drop_parameters,
     Widget drop_window)

{
   unsigned int modifiers = 0;
   int numFiles, i;
   char ** file_set = NULL;
   char ** host_set = NULL;
   Boolean trashFile;


     /***************************************************/
     /* if no drop_parameters, there is nothing to move */
     /***************************************************/
   if (!drop_parameters)
     return;


     /**************************/
     /* are these trash files? */
     /**************************/
   trashFile = FileFromTrash(drop_parameters->dropData->data.files[0]);


     /***********************************************/
     /* if trying to copy or link from trash return */
     /***********************************************/
   if (trashFile)
   {
     if (file_mgr_data != trashFileMgrData)
        if (InvalidTrashDragDrop(drop_parameters->operation,
              FROM_TRASH,
              ((FileMgrRec *)file_mgr_data->file_mgr_rec)->file_window))
           return;
   }


     /***************************************************/
     /* extract file and host sets from drop parameters */
     /***************************************************/
   numFiles = drop_parameters->dropData->numItems;
   _DtSetDroppedFileInfo(drop_parameters, &file_set, &host_set);


      /******************************/
      /* set movement modifier mask */
      /******************************/
   if( (initiating_view != NULL) &&
       (((FileMgrData *)initiating_view)->toolbox) )
   {
      /* if initiating_view is a toolbox, the transfer must be */
      /* a copy                                                */
      modifiers = ControlMask;
   }
   else
   {
      if (strcmp(command, "FILESYSTEM_COPY") == 0)
         modifiers = ControlMask;
      else if (strcmp(command, "FILESYSTEM_LINK") == 0)
         modifiers = ShiftMask;
      else
         modifiers = 0;
   }


      /*****************************/
      /* Files dropped on a window */
      /*****************************/
   if (drop_window)
   {
        /****************************************************************/
        /* Files dropped in the trash -- move files  to trash directory */
        /****************************************************************/
      if(file_mgr_data == trashFileMgrData && !trashFile)
      {
         DPRINTF(("DropOnFileWindow:Dragging File(s) to Trash Can from NonTrash Window\n"));

         DropOnTrashCan(numFiles, host_set, file_set, drop_parameters);
      }

        /****************************************************************/
        /* Files dragged in the trash -- do nothing                     */
        /****************************************************************/
      else if(file_mgr_data == trashFileMgrData && trashFile)
      {
         DPRINTF(("DropOnFileWindow: Drag from Within Trash Can\n"));
      }

        /****************************************************************/
        /* Files dragged from the trash -- move the files to their new  */
        /* location                                                     */
        /****************************************************************/
      else if(trashFile && file_mgr_data != trashFileMgrData)
      {
         DPRINTF(("DropOnFileWindow: Dragging from Trash to Folder Window\n"));

         MoveOutOfTrashCan(file_mgr_data,
                           (FileMgrRec *)file_mgr_data->file_mgr_rec,
                           XtWindow(drop_window),  numFiles, host_set,
                           file_set, drop_parameters->x, drop_parameters->y);
      }


        /****************************************************************/
        /* Files dropped on a non-trash window -- move files to new     */
        /* location.                                                    */
        /*                                                              */
        /* Droppable windows must be handled like the desktop; i.e.     */
        /* positioning is supported.                                    */
        /****************************************************************/
      else
      {
         FileMgrRec *file_mgr_rec = (FileMgrRec *)file_mgr_data->file_mgr_rec;

/*
         if ((file_mgr_data->show_type == SINGLE_DIRECTORY) &&
             (file_mgr_data->view != BY_ATTRIBUTES))
*/
         {
            if (file_mgr_data == (FileMgrData *) initiating_view)
            {
               DPRINTF(("DropOnFileWindow: Dragging and Dropping File within same window\n"));

                  /* Simple reposition in the same window */
               XmDropSiteStartUpdate(file_mgr_rec->file_window);
               RepositionIcons(file_mgr_data,
                               file_set,
                               numFiles,
                               drop_parameters->x, drop_parameters->y,
                               False);
               LayoutFileIcons(file_mgr_rec, file_mgr_data, False, True);
               XmDropSiteEndUpdate(file_mgr_rec->file_window);
            }
            else
            {
               DPRINTF (("DropOnFileWindow: Dragging file(s) and dropping from other folders\n"));

               CheckMoveType(file_mgr_data, (FileViewData *)NULL,
                             (DirectorySet *)NULL, (DesktopRec *)NULL,
                             file_set, host_set, modifiers,
                             numFiles,
                             drop_parameters->x, drop_parameters->y,
                             NOT_DESKTOP);
            }
         }
/*
         else
         {
            DPRINTF(("DropOnFileWindow: Not Single Directory View\n"));

            if (FileMoveCopy (file_mgr_data,
                              NULL, file_mgr_data->current_directory, file_mgr_data->host,
                              host_set, file_set, numFiles,
                              modifiers, NULL, NULL))
            {
               DirectorySet * directory_data;
               char * directory_name;
               FileViewData * file_view_data;
               int j;

               DeselectAllFiles (file_mgr_data);

               directory_data = file_mgr_data->directory_set[0];
               for (i = 0; i < numFiles; i++)
               {
                  directory_name = DName (file_set[i]);
                  for (j = 0; j < directory_data->file_count; j++)
                  {
                     file_view_data = directory_data->file_view_data[j];
                     if ( (file_view_data->filtered != True) &&
                          (strcmp(directory_name,
                                  file_view_data->file_data->file_name) == 0) )
                     {
                        SelectFile (file_mgr_data, file_view_data);
                        break;
                     }
                  }
               }

               PositionFileView(file_view_data, file_mgr_data);
            }
         }
*/

         if (file_mgr_data->selected_file_count == 0)
            ActivateNoSelect (file_mgr_rec);
         else if (file_mgr_data->selected_file_count == 1)
            ActivateSingleSelect
               (file_mgr_rec,
                file_mgr_data->selection_list[0]->file_data->logical_type);
         else
            ActivateMultipleSelect (file_mgr_rec);
      }
   }

      /*****************************/
      /* Files dropped on an icon  */
      /*****************************/
   else
   {
      CheckMoveType(file_mgr_data, file_view_data,
                    (DirectorySet *) file_view_data->directory_set,
                    (DesktopRec *)NULL,
                    file_set, host_set, modifiers,
                    numFiles, drop_parameters->x, drop_parameters->y,
                    NOT_DESKTOP_DIR);
   }

      /***************************/
      /* free file and host sets */
      /***************************/
   _DtFreeDroppedFileInfo(numFiles, file_set, host_set);

}

/************************************************************************
 *
 * ProcessBufferDropOnFolder
 *
 ************************************************************************/

void
ProcessBufferDropOnFolder (
     char *command,
     FileMgrData *file_mgr_data,
     FileViewData *file_view_data,
     DtDndDropCallbackStruct *drop_parameters,
     Widget drop_window)

{
   unsigned int modifiers = 0;
   int num_of_buffers, i;
   char ** file_set = NULL;
   char ** host_set = NULL;
   BufferInfo *buffer_set = NULL;
   char  directory[MAX_PATH];




   /***************************************************/
   /* if no drop_parameters, or invalid params        */
   /* then disallow the drop                          */
   /***************************************************/
   if (!drop_parameters)
     return;

   /* if dropped on file window and file_mgr_data is null */
   if (drop_window && (file_mgr_data == NULL))
     return;

   /* if dropped on a folder icon and file_view_data */
   /* is NULL, disallow the drop                     */
   if (!drop_window && (file_view_data == NULL))
     return;





  /****************************************************/
  /* extract file and host sets from drop parameters  */
  /* @@@...need to check with Linda about how host_set*/
  /* is being handled                                 */
  /****************************************************/

   num_of_buffers = drop_parameters->dropData->numItems;

   /* Allocate memory for file and buffer structures */
   file_set = (char **)XtMalloc(sizeof(char **) * num_of_buffers );
   host_set = (char **)XtMalloc(sizeof(char **) * num_of_buffers);
   buffer_set = (BufferInfo * )XtMalloc (sizeof (BufferInfo) * num_of_buffers);


   _DtSetDroppedBufferInfo(file_set, buffer_set, host_set, drop_parameters);




  /*****************************************************/
  /* If buffers were dropped on the window, determine  */
  /* which MODE (AS PLACED, GRID, TREE VIEW) and call  */
  /* the appropriate routines to handle the creation   */
  /* of the buffers into files. Assuming dropping      */
  /* on non-trash windows.                             */
  /*****************************************************/

  if (drop_window)
  {

    /* Single directory view */
    if ((file_mgr_data->show_type == SINGLE_DIRECTORY ) &&
        (file_mgr_data->view != BY_ATTRIBUTES))

    {

      DPRINTF (("ProcessDropOnBufferFolder: Dropping buffers on single directory view: %s\n", file_mgr_data->current_directory));

      G_dropx = drop_parameters->x;
      G_dropy = drop_parameters->y;

      /* Reposition Icons if in in "AS PLACED" Mode */
      if (file_mgr_data -> positionEnabled == RANDOM_ON)
      {
/*
        RepositionIcons (file_mgr_data,
                         file_set,
                         num_of_buffers,
                         drop_parameters->x, drop_parameters->y,
                         True);
*/
      }

      /* Call MakeFileFromBuffer */
      MakeFilesFromBuffers(file_mgr_data, file_mgr_data->current_directory,
			   file_mgr_data->host, file_set,
			   host_set, buffer_set, num_of_buffers,
			   NULL, NULL);

    }
    else
    {
      DPRINTF (("ProcessDropOnBufferFolder: Dropping buffers in Tree View\n"));

      MakeFilesFromBuffers(file_mgr_data, file_mgr_data->current_directory,
			   file_mgr_data->host, file_set,
			   host_set, buffer_set, num_of_buffers,
			   NULL, NULL);

      /* Do Tree View Stuff      */
      {
         DirectorySet * directory_data;
         char * directory_name;
         FileViewData * file_view_data;
         int j;

         DeselectAllFiles (file_mgr_data);

         directory_data = file_mgr_data->directory_set[0];
         for (i = 0; i < num_of_buffers; i++)
         {
            directory_name = DName (file_set[i]);
            for (j = 0; j < directory_data->file_count; j++)
            {
               file_view_data = directory_data->file_view_data[j];
               if ( (file_view_data->filtered != True) &&
                     (strcmp(directory_name,
                     file_view_data->file_data->file_name) == 0) )
               {
                  SelectFile (file_mgr_data, file_view_data);
                  break;
               }
            }
         }
         PositionFileView(file_view_data, file_mgr_data);
      }
   } /* endif for Tree View */

  } /* endif drop buffers on window */
  else
  {
    /* Buffers were dropped on a Folder icon */
    /* Call MakeFileFromBuffer */


    DPRINTF(("ProcessBufferDropOnFolder...Buffers dropped on Folder icon %s\n",
             file_view_data ->file_data -> file_name));

    if (file_mgr_data->show_type != SINGLE_DIRECTORY &&
        file_mgr_data->tree_root == file_view_data)
    {
        /* dropped on the top level folder in the tree view */
        sprintf (directory,"%s",file_mgr_data->current_directory);
    }
    else
        sprintf (directory,"%s/%s",file_mgr_data->current_directory,
                 file_view_data->file_data->file_name);
    DtEliminateDots(directory);

    DPRINTF (("Copying buffer to %s\n", directory));
    MakeFilesFromBuffers(file_mgr_data, directory,
			 file_mgr_data->host, file_set,
			 host_set, buffer_set, num_of_buffers,
			 NULL, NULL);

  }


  /***********************************/
  /* free file_set + buffer_set      */
  /***********************************/
  _DtFreeDroppedBufferInfo (file_set, buffer_set, host_set, num_of_buffers);


}


/************************************************************************
 *
 *  InvalidTrashDragDrop
 *
 ************************************************************************/

Boolean
InvalidTrashDragDrop (
     int drag_op,
     int trash_context,
     Widget w)
{
   Boolean rc = False;

   if ( (drag_op == XmDROP_COPY) ||
        (drag_op == XmDROP_LINK) )
   {
      char *tmpStr, *title, *msg;

      tmpStr = GETMESSAGE(18, 22, "Drag Error");
      title = XtNewString(tmpStr);

      switch(trash_context)
      {
         case TO_TRASH:
         case WITHIN_TRASH:
            tmpStr = (GETMESSAGE(18,36, "You can't copy or link a file or folder out of the Trash Can.\nMove the object out of the Trash and put it into the File Manager.\nYou can then copy or link it from there."));
            break;
         case FROM_TRASH:
            tmpStr = (GETMESSAGE(18,37, "You can't copy or link a file or folder out of the Trash Can.\nMove the object out of the Trash and put it into the File Manager.\nYou can then copy or link it from there."));
            break;
         default:
            tmpStr = NULL;
            break;
      }

      if (tmpStr)
      {
         msg = XtNewString(tmpStr);
         _DtMessage (w, title, msg, NULL, HelpRequestCB);
         XtFree(msg);
      }

      XtFree(title);

      rc = True;
   }

   return(rc);
}


/************************************************************************
 *
 *  ProcessAction
 *
 ************************************************************************/

void
ProcessAction (
     char *action,
     FileViewData *file_view_data,
     DtDndDropCallbackStruct *drop_parameters,
     char *cur_host,
     char *cur_dir,
     char *restricted_dir,
     Widget w)

{
   FileViewData *first_arg = NULL;
   DtActionArg * action_args = NULL;
   int arg_count = 0;
   char * pwd_host = NULL;
   char * pwd_dir = NULL;
   DirectorySet *directory_set;
   FileMgrData *file_mgr_data;

   /* We don't want to execute the default action if in trash ...  */
   directory_set = (DirectorySet *) file_view_data->directory_set;
   file_mgr_data = (FileMgrData *) directory_set->file_mgr_data;
   if( trashFileMgrData != NULL
       && file_mgr_data == trashFileMgrData)
   {
      char *tmpStr, *title, *msg;

      /* we don't want to execute the default action if in trash ...  */
      tmpStr = GETMESSAGE(27, 3, "Trash Can Error");
      title = XtNewString(tmpStr);
      tmpStr = GETMESSAGE(27, 105, "Default action of a trash object will not be executed.\n\nTo execute the default action of this object\n use 'Put Back' to return it to the File Manager\nthen execute it there.");
      msg = XtNewString(tmpStr);

      _DtMessage(((FileMgrRec *)file_mgr_data->file_mgr_rec)->file_window,
                 title, msg, NULL, HelpRequestCB);
      XtFree(title);
      XtFree(msg);
      return;
   }

   /* Build action arguments:
    *   First, test for redundant action information --
    *   file_view_data contains information for the object that the user
    *       a) dropped files on
    *       b) activated a popup menu over (note that the Actions piece
    *          of both the Selected and popup menus is only active when
    *          a single file is selected; therefore, this function doesn't
    *          deal with cases where multiple files are selected and a
    *          popup menu is activated
    *       c) doubled-clicked
    *
    *   If file_view_data contains information for the action that we
    *   are processing, then this information is not included as an
    *   argument to DtActionInvoke; otherwise, the information is
    *   included as the first argument to DtActionInvoke.
    */
   if ( !DtDtsDataTypeIsAction(file_view_data->file_data->logical_type) ||
        (strcmp(action, file_view_data->file_data->logical_type) != 0) ||
        (strcmp(action, file_view_data->file_data->file_name)    != 0) )
      first_arg = file_view_data;

   if (drop_parameters)
   {
      if (drop_parameters->dropData->protocol == DtDND_FILENAME_TRANSFER)
         _DtBuildActionArgsWithDroppedFiles(first_arg, drop_parameters,
                                            &action_args, &arg_count);
      else
         _DtBuildActionArgsWithDroppedBuffers(first_arg, drop_parameters,
                                              &action_args, &arg_count);
   }
   else
   {
      if (first_arg)
         _DtBuildActionArgsWithSelectedFiles(&first_arg, 1,
                                             &action_args, &arg_count);
   }


   /* Retrieve context dir for action -- in the case of toolboxes, the
      root toolbox */
   SetPWD(cur_host, cur_dir, &pwd_host, &pwd_dir, restricted_dir);


   /* Turn on hour glass */
   _DtTurnOnHourGlass(w);


   /* Invoke action */
   DtActionInvoke(w, action, action_args, arg_count,
                  NULL, NULL, pwd_dir, True, NULL, NULL);


   /* Add timer event to turn off hour glass */
   XtAppAddTimeOut(XtWidgetToApplicationContext(w), 1500,
                   (XtTimerCallbackProc) TimerEvent, (XtPointer) w);


   XtFree(pwd_host);
   XtFree(pwd_dir);
   _DtFreeActionArgs(action_args, arg_count);
}


/************************************************************************
 *
 *  TimerEvent
 *      This function is called when dtfile does an _DtActionInvoke. All
 *      it does is turn off the Hourglass cursor.
 *
 ************************************************************************/

static void
TimerEvent(
        Widget widget,
        XtIntervalId *id )
{
   _DtTurnOffHourGlass (widget);
}

/************************************************************************
 *
 * This Handle's the FILESYSTEM_MOVE, FILESYSTEM_COPY, or FILESYSTEM_LINK
 * ToolTalk messages.  The operation type is passed in via opType. It is
 * either MOVE_FILE, COPY_FILE, or LINK_FILE.  Arg 0 of the ToolTalk message
 * contains the folder the operation is taking place TO and arg 1 contains
 * the files that the operation  is happening to.
 *
 ************************************************************************/

void
MoveCopyLinkHandler(
                    Tt_message ttMsg,
                    int opType)
{
   struct stat fileInfo;
   char title[256];
   int numArgs, i;
   char *ptr, *toName, *fileNames = NULL, *type = NULL, *fileList;
   char *files = NULL;
   char ** file_set = NULL;
   char ** host_set = NULL;
   unsigned int modifiers = 0;
   int file_count = 0;
   int file_set_size = 0;
   int errorCount = 0;

   toName = tt_message_file( ttMsg );
   fileNames = fileList = tt_message_arg_val( ttMsg, 1 );

   if( tt_is_err( tt_ptr_error( toName ) ) )
   { /* No file name */
      tt_message_reply( ttMsg );
      tttk_message_destroy( ttMsg );
      return;
   }

   /* let's loop through the fileName passed to get the files which the
    * operation is to happen on.  The file's are separated by spaces.  What
    * happens if a file has a space in it: We parse on the spaces and if the
    * next char after a space is a '/' then we assume that is the end of the
    * file name.  What this implies is that this won't work for files with
    * spaces at the end of the name
    */
   while(1)
   {
      /* build the arrary of char pointer's */
      if (file_count == file_set_size)
      {
         file_set_size += 10;
         file_set =
           (char **) XtRealloc ((char *)file_set,
                                sizeof (char **) * file_set_size);
      }

      /* find the next space */
      ptr = DtStrchr(fileList, ' ');

      /* if ptr is NULL, we have our last file name (no spaces found) */
      if(ptr == NULL)
      {
         file_set[file_count] = XtNewString(fileList);
         file_count++;
         break;
      }
      else
      {
         /* Let's check if the next char is a '/'. If it is then we know it's
          * the next file name, else the space found is part of a filename.
          */
         if(ptr[1] == '/')
         {
            *ptr = '\0';
            file_set[file_count] = XtNewString(fileList);
            file_count++;
         }
         fileList = ptr+1;
      }
   }


  /* go clean up all the '.' and '..' */
   for(i = 0; i < file_count; i++)
      DtEliminateDots( file_set[i] );
   DtEliminateDots( toName );

  /* Set up the modifier key's */
   if( opType == MOVE_FILE)
      modifiers = 0;
   else if ( opType == COPY_FILE)
      modifiers = ControlMask;
   else
      modifiers = ShiftMask;


   /*
    * Let's check make sure the Folder the operation is happening to exists.
    */
   if( stat( toName, &fileInfo ) != 0
       && lstat( toName, &fileInfo ) != 0 )
   { /* to directory does not exist */
      char *dialogTitle;

      if(opType == MOVE_FILE)
      {
         dialogTitle = XtNewString(GETMESSAGE(33, 3, "Move Object Error"));
         sprintf(title, GETMESSAGE(33, 6, "The location you are trying to Move to:\n\n   %s\n\ndoes not exist in the file system."), toName);
      }
      else if(opType == COPY_FILE)
      {
         dialogTitle = XtNewString(GETMESSAGE(33, 4, "Copy Object Error"));
         sprintf(title, GETMESSAGE(33, 7, "The location you are trying to Copy to:\n\n   %s\n\ndoes not exist in the file system."), toName);
      }
      else
      {
         dialogTitle = XtNewString(GETMESSAGE(33, 5, "Link Object Error"));
         sprintf(title, GETMESSAGE(33, 8, "The location you are trying to Link to:\n\n   %s\n\ndoes not exist in the file system."), toName);
      }
      _DtMessage( toplevel, dialogTitle, title, NULL, HelpRequestCB );

      tt_free( toName );
      tt_message_reply( ttMsg );
      tttk_message_destroy( ttMsg );
      return;
   }


   /*
    * Let's check make sure the object the operation is happening to is a
    * Folder.
    */
   if ((fileInfo.st_mode & S_IFMT) != S_IFDIR)     /* target not directory */
   { /* File that the user is doing the operation to is not a directory */
      char *dialogTitle;

      if(opType == MOVE_FILE)
      {
         dialogTitle = XtNewString(GETMESSAGE(33, 3, "Move Object Error"));
         sprintf(title, GETMESSAGE(33, 9, "The location you are trying to Move to:\n\n   %s\n\nis not a folder."), toName);
      }
      else if(opType == COPY_FILE)
      {
         dialogTitle = XtNewString(GETMESSAGE(33, 4, "Copy Object Error"));
         sprintf(title, GETMESSAGE(33, 10, "The location you are trying to Copy to:\n\n   %s\n\nis not a folder."), toName);
      }
      else
      {
         dialogTitle = XtNewString(GETMESSAGE(33, 5, "Link Object Error"));
         sprintf(title, GETMESSAGE(33, 11,"The location you are trying to Link to:\n\n   %s\n\nis not a folder."), toName);
      }
      _DtMessage( toplevel, dialogTitle, title, NULL, HelpRequestCB );

      tt_free( toName );
      tt_message_reply( ttMsg );
      tttk_message_destroy( ttMsg );
      return;
   }

   /*
    *
    *
    */
   for(i = 0; i < file_count; i++)
   {
      if( stat( file_set[i], &fileInfo ) != 0
          && lstat( file_set[i], &fileInfo ) != 0 )
      { /* File does not exist */
         char *dialogTitle;

         if(opType == MOVE_FILE)
         {
            dialogTitle = XtNewString(GETMESSAGE(33, 3, "Move Object Error"));
            if(file_count == 1)
               sprintf(title, GETMESSAGE(33, 12, "The object you are trying to Move:\n\n   %s\n\ndoes not exist in the file system."), file_set[i]);
            else
               sprintf(title, GETMESSAGE(33, 13, "One of the objects you are trying to Move:\n\n   %s\n\ndoes not exist in the file system.\nNot Moving any of them."), file_set[i]);
         }
         else if(opType == COPY_FILE)
         {
            dialogTitle = XtNewString(GETMESSAGE(33, 4, "Copy Object Error"));
            if(file_count == 1)
               sprintf(title, GETMESSAGE(33, 14, "The object you are trying to Copy:\n\n   %s\n\ndoes not exist in the file system."), file_set[i]);
            else
               sprintf(title, GETMESSAGE(33, 15, "One of the objects you are trying to Copy:\n\n   %s\n\ndoes not exist in the file system.\nNot Copying any of them."), file_set[i]);
         }
         else
         {
            dialogTitle = XtNewString(GETMESSAGE(33, 5, "Link Object Error"));
            if(file_count == 1)
               sprintf(title, GETMESSAGE(33, 16, "The object you are trying to Link:\n\n   %s\n\ndoes not exist in the file system."), file_set[i]);
            else
               sprintf(title, GETMESSAGE(33, 17, "One of the objects you are trying to Link:\n\n   %s\n\ndoes not exist in the file system.\nNot Linking any of them."), file_set[i]);
         }
         _DtMessage( toplevel, dialogTitle, title, NULL, HelpRequestCB );

         tt_free( toName );
         tt_message_reply( ttMsg );
         tttk_message_destroy( ttMsg );
         return;
      }
   }


   /* set all the hosts to the home host name since ToolTalk messages pass
    * the filenames in host relative paths (Is this correct?)
    */
   host_set = (char **)XtMalloc(sizeof(char *) * file_count);
   for(i = 0; i < file_count; i++)
      host_set[i] = home_host_name;

   /* Go do the Move/Copy/Link... this function will do proper error checking */
   FileMoveCopy(NULL, NULL, toName, home_host_name,
                host_set, file_set, file_count,
                modifiers, NULL, NULL);

   tt_free( toName );
   tt_message_reply( ttMsg );
   tttk_message_destroy( ttMsg );
   return;
}

