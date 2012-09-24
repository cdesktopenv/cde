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
/* $XConsortium: HelpP.c /main/4 1995/11/02 14:40:13 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           HelpP.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Processing functions for the help dialog.
 *
 *   FUNCTIONS: DTHelpClose
 *		DTHyperLink
 *		MainWinHyperLink
 *		MainWindowHelpClose
 *		ShowDTHelpDialog
 *		ShowHelpDialog
 *		ShowTrashHelpDialog
 *		TrashHelpClose
 *		TrashHyperLink
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
#include <unistd.h>
#include <time.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>

#include <Dt/Connect.h>
#include <Dt/HourGlass.h>
#include "Encaps.h"
#include "SharedProcs.h"

#include <Dt/HelpDialog.h>

#include "Desktop.h"
#include "Main.h"
#include "Help.h"
#include "FileMgr.h"


/********    Static Function Declarations    ********/

static void MainWindowHelpClose( 
                        XtPointer client_data,
                        DialogData *old_dialog_data,
                        DialogData *new_dialog_data) ;
static void MainWinHyperLink(
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData) ;
static void TrashHelpClose( 
                        XtPointer client_data,
                        DialogData *old_dialog_data,
                        DialogData *new_dialog_data) ;
static void TrashHyperLink(
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData) ;
static void DTHelpClose(
                        XtPointer client_data,
                        DialogData *old_dialog_data,
                        DialogData *new_dialog_data) ;
static void DTHyperLink(
                        Widget w,
                        XtPointer clientData,
                        XtPointer callData) ;

/********    End Static Function Declarations    ********/



/************************************************************************
 *
 *  ShowHelpDialog
 *      Used to display a primary or a hyperlink help dialog
 *
 ************************************************************************/

void
ShowHelpDialog(
        Widget parentShell,
        XtPointer topLevelRec,
        int dialogType,
        DialogData *dialogData,
        char *idString,
        char *volString,
        char *topicTitle,
        char *fileType,
        int helpType)

{
   HelpRec * helpRec;
   HelpData * helpData;
   int count;
   char *helpName;
   char *tmpStr;
   DialogData * fmDialogData;
   FileMgrData * fileMgrData;
   Arg args[5];


   /* 
    * DialogData is not NULL, if doing a restore session */
   if (dialogData == NULL)
   {
      /* We are not restoring a session, so get the default values */
      dialogData = _DtGetDefaultDialogData(help_dialog);

      /* Add to the array of visible help annotation dialogs for this view */
      fmDialogData = _DtGetInstanceData(topLevelRec);
      fileMgrData = (FileMgrData *) fmDialogData->data;

      if (dialogType == MAIN_HELP_DIALOG)
      {
         fileMgrData->primaryHelpDialog = dialogData;
      }
      else
      {
         /* Hyperlink help window */
         _DtAddOneSubdialog (dialogData,
                          &fileMgrData->secondaryHelpDialogList,
                          &fileMgrData->secondaryHelpDialogCount);
      }

      helpData = (HelpData *)dialogData->data;
      helpData->idString = XtNewString(idString);
      helpData->volString = XtNewString(volString);
      helpData->topicTitle = XtNewString(topicTitle);
      helpData->fileType = XtNewString(fileType);
      helpData->helpType = helpType;
   }

   _DtShowDialog(parentShell, NULL, topLevelRec, dialogData, 
              NULL, NULL, MainWindowHelpClose, topLevelRec, NULL, False,
              False, NULL, NULL);

   /* lets set the title appropriately */
   fmDialogData = _DtGetInstanceData(topLevelRec);
   fileMgrData = (FileMgrData *) fmDialogData->data;
   helpData = (HelpData *)dialogData->data;
   if(fileMgrData->title == NULL || fileMgrData->helpVol == NULL ||
           strcmp(fileMgrData->helpVol, DTFILE_HELP_NAME) == 0 ||
                     strcmp(helpData->volString, "Help4Help") == 0)
   {
      if(fileMgrData->title)
      {
        tmpStr = GETMESSAGE(29,2, "%s Help"); 
        helpName = (char *)XtMalloc(strlen(fileMgrData->title) + 
                                  strlen(tmpStr) + 1); 
        sprintf(helpName, tmpStr, fileMgrData->title); 
        XtSetArg(args[0], XmNtitle, helpName);
      }
      else
      {
        XtSetArg(args[0], XmNtitle, (GETMESSAGE(29,1, "File Manager Help")));
        helpName = NULL;
      }
   }
   else
   {
      tmpStr = GETMESSAGE(29,2, "%s Help"); 
      helpName = (char *)XtMalloc(strlen(fileMgrData->title) + 
                                  strlen(tmpStr) + 1); 
      sprintf(helpName, tmpStr, fileMgrData->title); 
      XtSetArg(args[0], XmNtitle, helpName);
   }
   helpRec = (HelpRec *)_DtGetDialogInstance(dialogData);
   XtSetValues(XtParent(helpRec->helpDialog), args, 1);
   if(helpName != NULL)
      XtFree(helpName);

   /* Add hyperlink callback */
   XtRemoveAllCallbacks(helpRec->helpDialog, DtNhyperLinkCallback);
   XtAddCallback(helpRec->helpDialog, DtNhyperLinkCallback,
                 MainWinHyperLink, topLevelRec);
}




/************************************************************************
 *
 *  MainWindowHelpClose
 *	Callback function invoked from the help dialog's close button.
 *
 ************************************************************************/

static void
MainWindowHelpClose(
        XtPointer client_data,
        DialogData *old_dialog_data,
        DialogData *new_dialog_data )

{
   FileMgrRec  * fileMgrRec = (FileMgrRec *) client_data;
   FileMgrData * fileMgrData;
   DialogData  * dialog_data;
   int count;
   int i, j;

   dialog_data = _DtGetInstanceData((XtPointer)fileMgrRec);
   fileMgrData = (FileMgrData *)dialog_data->data;

   /*  Remove the dialog data from the top level window's list  */
   if (old_dialog_data == fileMgrData->primaryHelpDialog)
   {
      fileMgrData->primaryHelpDialog = NULL;
   }
   else
   {
      _DtHideOneSubdialog( old_dialog_data,
                        &fileMgrData->secondaryHelpDialogList,
                        &fileMgrData->secondaryHelpDialogCount);
   }

   _DtHideDialog(old_dialog_data, False);
   _DtFreeDialogData(old_dialog_data);
   _DtFreeDialogData(new_dialog_data);
}



/************************************************************************
 *
 *  MainWinHyperLink
 *	Callback function responsible for handling a hyperlink help
 *      request for a help window associated with a main window view.
 *
 ************************************************************************/

static void 
MainWinHyperLink(
        Widget w,
        XtPointer clientData,
        XtPointer callData )

{
   FileMgrRec * fileMgrRec = (FileMgrRec *)clientData;
   DtHelpDialogCallbackStruct * hyperData;

   hyperData = (DtHelpDialogCallbackStruct *)callData;
   if (hyperData->hyperType == DtHELP_LINK_JUMP_NEW)
   {
      ShowHelpDialog(fileMgrRec->shell, (XtPointer)fileMgrRec,
		     HYPER_HELP_DIALOG, 
                     NULL, hyperData->locationId,
                     hyperData->helpVolume, NULL, NULL, DtHELP_TYPE_TOPIC);
   }
}


/************************************************************************
 *
 *  ShowTrashHelpDialog
 *      Used to display a primary or a hyperlink help dialog for trash
 *      window.
 *
 ************************************************************************/

void
ShowTrashHelpDialog(
        Widget parentShell,
        int dialogType,
        DialogData *dialogData,
        char *idString,
        char *volString  )

{
   HelpRec * helpRec;
   HelpData * helpData;
   int count;
   Arg args[3];

   /* Get the default values */
   dialogData = _DtGetDefaultDialogData(help_dialog);

   if (dialogType == MAIN_HELP_DIALOG)
   {
      primaryTrashHelpDialog = dialogData;
   }
   else
   {
      /* Hyperlink help window */
      _DtAddOneSubdialog (dialogData,
                       &secondaryTrashHelpDialogList,
                       &secondaryTrashHelpDialogCount);
   }

   helpData = (HelpData *)dialogData->data;
   helpData->idString = XtNewString(idString);
   helpData->volString = XtNewString(volString);
   helpData->helpType  = DtHELP_TYPE_TOPIC;

   _DtShowDialog(parentShell, NULL, NULL, dialogData, NULL, NULL, TrashHelpClose, 
              NULL, NULL, False, False, NULL, NULL);

   /* lets set the title appropriately */
   XtSetArg(args[0], XmNtitle, (GETMESSAGE(29,9, "Trash Can Help")));
   helpRec = (HelpRec *)_DtGetDialogInstance(dialogData);
   XtSetValues(XtParent(helpRec->helpDialog), args, 1);

   /* Add hyperlink callback */
   helpRec = (HelpRec *)_DtGetDialogInstance(dialogData);
   XtRemoveAllCallbacks(helpRec->helpDialog, DtNhyperLinkCallback);
   XtAddCallback(helpRec->helpDialog, DtNhyperLinkCallback,
                 TrashHyperLink, NULL);
}




/************************************************************************
 *
 *  TrashHelpClose
 *	Callback function invoked from the help dialog's close button,
 *      for the Trash help dialogs.
 *
 ************************************************************************/

static void
TrashHelpClose(
        XtPointer client_data,
        DialogData *old_dialog_data,
        DialogData *new_dialog_data )

{
   int count;
   int i, j;

   /*  Remove the dialog data from the trash window's list  */
   if (old_dialog_data == primaryTrashHelpDialog)
   {
      primaryTrashHelpDialog = NULL;
   }
   else
   {
      _DtHideOneSubdialog( old_dialog_data,
                        &secondaryTrashHelpDialogList,
                        &secondaryTrashHelpDialogCount);
   }

   _DtHideDialog(old_dialog_data, False);
   _DtFreeDialogData(old_dialog_data);
   _DtFreeDialogData(new_dialog_data);
}



/************************************************************************
 *
 *  TrashHyperLink
 *	Callback function responsible for handling a hyperlink help
 *      request for a help window associated with the trash window.
 *
 ************************************************************************/

static void 
TrashHyperLink(
        Widget w,
        XtPointer clientData,
        XtPointer callData )

{
   DtHelpDialogCallbackStruct * hyperData;

   hyperData = (DtHelpDialogCallbackStruct *)callData;
   if (hyperData->hyperType == DtHELP_LINK_JUMP_NEW)
   {
      ShowTrashHelpDialog(trashShell, HYPER_HELP_DIALOG, NULL, 
                          hyperData->locationId, hyperData->helpVolume);
   }
}


/************************************************************************
 *
 *  ShowDTHelpDialog
 *      Used to display a primary or a hyperlink help dialog
 *
 ************************************************************************/

void
ShowDTHelpDialog(
        Widget parentShell,
        int workspaceNum,
        int dialogType,
        DialogData *dialogData,
        char *idString,
        char *volString,
        char *topicTitle,
        char *fileType,
        int helpType)

{
   HelpRec * helpRec;
   HelpData * helpData;
   int count;
   Arg args[3];

   /* 
    * DialogData is not NULL, if doing a restore session */
   if (dialogData == NULL)
   {
      /* We are not restoring a session, so get the default values */
      dialogData = _DtGetDefaultDialogData(help_dialog);

      /* Add to the array of visible help annotation dialogs for this view */
      if (dialogType == MAIN_HELP_DIALOG)
      {
         desktop_data->workspaceData[workspaceNum]->primaryHelpDialog =
                 dialogData;
      }
      else
      {
         WorkspaceRec * wsInfo;

         /* Hyperlink help window */
         wsInfo = desktop_data->workspaceData[workspaceNum];
         _DtAddOneSubdialog (dialogData,
                          &wsInfo->secondaryHelpDialogList,
                          &wsInfo->secondaryHelpDialogCount);
      }

      helpData = (HelpData *)dialogData->data;
      helpData->idString = XtNewString(idString);
      helpData->volString = XtNewString(volString);
      helpData->topicTitle = XtNewString(topicTitle);
      helpData->fileType = XtNewString(fileType);
      helpData->helpType = helpType;
   }

   _DtShowDialog(parentShell, NULL, NULL, dialogData, NULL, NULL, 
              DTHelpClose, (XtPointer)(XtArgVal)workspaceNum,
              desktop_data->workspaceData[workspaceNum]->name,
              False, False, NULL, NULL);

   /* lets set the title appropriately */
   XtSetArg(args[0], XmNtitle, (GETMESSAGE(29,1, "File Manager Help")));
   helpRec = (HelpRec *)_DtGetDialogInstance(dialogData);
   XtSetValues(XtParent(helpRec->helpDialog), args, 1);

   /* Add hyperlink callback */
   helpRec = (HelpRec *)_DtGetDialogInstance(dialogData);
   XtRemoveAllCallbacks(helpRec->helpDialog, DtNhyperLinkCallback);
   XtAddCallback(helpRec->helpDialog, DtNhyperLinkCallback,
                 DTHyperLink, (XtPointer)(XtArgVal)workspaceNum);
}




/************************************************************************
 *
 *  DTHelpClose
 *	Callback function invoked from the help dialog's close button.
 *
 ************************************************************************/

static void
DTHelpClose(
        XtPointer client_data,
        DialogData *old_dialog_data,
        DialogData *new_dialog_data )

{
   int  workspaceNum = (int)(XtArgVal) client_data;
   int count;
   int i, j;
   WorkspaceRec * wsInfo;

   wsInfo = desktop_data->workspaceData[workspaceNum];

   /*  Remove the dialog data from the top level window's list  */
   if (old_dialog_data == wsInfo->primaryHelpDialog)
   {
      wsInfo->primaryHelpDialog = NULL;
   }
   else
   {
      _DtHideOneSubdialog( old_dialog_data,
                        &wsInfo->secondaryHelpDialogList,
                        &wsInfo->secondaryHelpDialogCount);
   }

   _DtHideDialog(old_dialog_data, False);
   _DtFreeDialogData(old_dialog_data);
   _DtFreeDialogData(new_dialog_data);
}



/************************************************************************
 *
 *  DTHyperLink
 *	Callback function responsible for handling a hyperlink help
 *      request for a help window associated with a main window view.
 *
 ************************************************************************/

static void 
DTHyperLink(
        Widget w,
        XtPointer clientData,
        XtPointer callData )

{
   int  workspaceNum = (int)(XtArgVal) clientData;
   DtHelpDialogCallbackStruct * hyperData;

   hyperData = (DtHelpDialogCallbackStruct *)callData;
   if (hyperData->hyperType == DtHELP_LINK_JUMP_NEW)
   {
      ShowDTHelpDialog(NULL, workspaceNum, HYPER_HELP_DIALOG, 
                     NULL, hyperData->locationId,
                     hyperData->helpVolume, NULL, NULL, DtHELP_TYPE_TOPIC);
   }
}

