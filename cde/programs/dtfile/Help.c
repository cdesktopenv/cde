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
/* $XConsortium: Help.c /main/6 1996/10/29 17:08:14 mustafa $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 *   FILE:           Help.c
 *
 *   COMPONENT_NAME: Desktop File Manager (dtfile)
 *
 *   Description:    Source file for the help dialog.
 *
 *   FUNCTIONS: Create
 *		FreeValues
 *		GetDefaultValues
 *		GetResourceValues
 *		GetValues
 *		InstallClose
 *		MapFileTypeToHelpString
 *		SetValues
 *		WriteResourceValues
 *
 *   (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 *   (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 *   (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 *   (c) Copyright 1993, 1994, 1995 Novell, Inc.
 *
 ****************************************************************************
 ************************************<+>*************************************/


#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>


#include <Xm/XmP.h>
#include <Xm/DialogS.h>
#include <Xm/VendorSEP.h>
#include <Xm/MwmUtil.h>

#include <X11/ShellP.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <Xm/Protocols.h>

#include <Dt/Action.h>
#include <Dt/Connect.h>
#include <Dt/DtNlUtils.h>
#include <Dt/FileM.h>
#include <Dt/Dts.h>
#include "Encaps.h"
#include "SharedProcs.h"

#include <Dt/HelpDialog.h>

#include "Desktop.h"
#include "FileMgr.h"
#include "Main.h"
#include "Help.h"
#include "SharedMsgs.h"


/*  Resource definitions for the help dialog  */

static DialogResource resources[] =
{
   { "idString", XmRString, sizeof(String),
     XtOffset(HelpDataPtr, idString),
     (caddr_t) NULL, _DtStringToString },

   { "volString", XmRString, sizeof(String),
     XtOffset(HelpDataPtr, volString),
     (caddr_t) NULL, _DtStringToString },

   { "topicTitle", XmRString, sizeof(String),
     XtOffset(HelpDataPtr, topicTitle),
     (caddr_t) NULL, _DtStringToString },

   { "fileType", XmRString, sizeof(String),
     XtOffset(HelpDataPtr, fileType),
     (caddr_t) NULL, _DtStringToString },

   { "helpType", XmRInt, sizeof(int),
      XtOffset(HelpDataPtr, helpType),
      (caddr_t) DtHELP_TYPE_TOPIC, _DtIntToString },
};


static char * HELP_DIALOG = "HelpDialog";

/********    Static Function Declarations    ********/

static void Create(
                        Display *display,
                        Widget parent,
                        Widget *return_widget,
                        XtPointer *dialog) ;
static void InstallClose(
                        HelpRec *helpRec,
                        XtCallbackProc callback,
                        XtPointer clientData) ;
static XtPointer GetValues(
                        HelpRec *helpRec) ;
static XtPointer GetDefaultValues( void ) ;
static XtPointer GetResourceValues(
                        XrmDatabase data_base,
                        char **name_list) ;
static void SetValues(
                        HelpRec *helpRec,
                        HelpData *helpData) ;
static void WriteResourceValues(
                        DialogData *values,
                        int fd,
                        char **name_list) ;
static void FreeValues(
                        HelpData *helpData) ;

/********    End Static Function Declarations    ********/

/*
 *  The Dialog Class structure.
 */

static DialogClass helpClassRec =
{
   resources,
   XtNumber(resources),
   (DialogCreateProc)Create,
   (DialogInstallChangeProc)NULL,
   (DialogInstallCloseProc)InstallClose,
   (DialogDestroyProc)_DtGenericDestroy,
   (DialogGetValuesProc)GetValues,
   (DialogGetDefaultValuesProc)GetDefaultValues,
   (DialogGetResourceValuesProc)GetResourceValues,
   (DialogSetValuesProc)SetValues,
   (DialogWriteResourceValuesProc)WriteResourceValues,
   (DialogFreeValuesProc)FreeValues,
   (DialogMapWindowProc)_DtGenericMapWindow,
   (DialogSetFocusProc)NULL,
};

DialogClass * helpClass = (DialogClass *) &helpClassRec;




/************************************************************************
 *
 *  Create
 *
 ************************************************************************/

static void
Create(
        Display *display,
        Widget parent,
        Widget *return_widget,
        XtPointer *dialog )

{
   HelpRec * helpRec;
   Arg args[2];
   Widget helpDialog;
   int mwmDecs;

   /*  Allocate the audio annotation dialog instance record.  */

   helpRec = (HelpRec *) XtMalloc(sizeof(HelpRec));


   /*  Create the canned help dialog.  */

   helpDialog = DtCreateHelpDialog(parent, "helpDialog", args, 0);

   /* Fill in our instance structure */
   helpRec->shell = XtParent(helpDialog);
   helpRec->helpDialog = helpDialog;

   /*  Set the return values for the dialog widget and dialog instance.  */
   *return_widget = helpDialog;
   *dialog = (XtPointer) helpRec;
}


/************************************************************************
 *
 *  InstallClose
 *
 ************************************************************************/

static void
InstallClose(
        HelpRec *helpRec,
        XtCallbackProc callback,
        XtPointer clientData )

{
   XtAddCallback(helpRec->helpDialog, DtNcloseCallback, callback,
                 clientData);
}


/************************************************************************
 *
 *  GetValues
 *
 ************************************************************************/

static XtPointer
GetValues(
        HelpRec *helpRec )

{
   DialogData * dialog_data;
   HelpData * helpData;
   HelpData * oldData;
   Arg args[4];
   int n;


   /*  Allocate and initialize the help dialog data.  */

   helpData = (HelpData *) XtMalloc(sizeof(HelpData));

   helpData->displayed = True;

   n = 0;
   XtSetArg(args[n], XmNx, &helpData->x);           n++;
   XtSetArg(args[n], XmNy, &helpData->y);           n++;
   XtSetArg(args[n], XmNwidth, &helpData->width);   n++;
   XtSetArg(args[n], XmNheight, &helpData->height); n++;
   XtGetValues(helpRec->helpDialog, args, n);

   dialog_data = _DtGetInstanceData((XtPointer)helpRec);
   oldData = (HelpData *) dialog_data->data;
   helpData->idString = XtNewString(oldData->idString);
   helpData->volString = XtNewString(oldData->volString);
   helpData->topicTitle = XtNewString(oldData->topicTitle);
   helpData->fileType = XtNewString(oldData->fileType);
   helpData->helpType = oldData->helpType;

   return((XtPointer)helpData);
}




/************************************************************************
 *
 *  GetDefaultValues
 *
 ************************************************************************/

static XtPointer
GetDefaultValues( void )


{
   HelpData * helpData;

   /*  Allocate and initialize the default help dialog data.  */
   helpData = (HelpData *) XtMalloc(sizeof(HelpData));

   helpData->displayed = False;
   helpData->x = 0;
   helpData->y = 0;
   helpData->height = 0;
   helpData->width = 0;

   helpData->idString = NULL;
   helpData->volString = NULL;
   helpData->topicTitle = NULL;
   helpData->fileType = NULL;
   helpData->helpType = 0;

   return((XtPointer) helpData);
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
   HelpData * helpData;

   helpData = (HelpData *) XtMalloc(sizeof(HelpData));

   _DtDialogGetResources(data_base, name_list, HELP_DIALOG, (char *)helpData,
                      resources, helpClass->resource_count);

   /* We need copies of all strings returned by the resource manager */
   if (helpData->idString)
      helpData->idString = XtNewString(helpData->idString);
   if (helpData->volString)
      helpData->volString = XtNewString(helpData->volString);
   if (helpData->topicTitle)
      helpData->topicTitle = XtNewString(helpData->topicTitle);
   if (helpData->fileType)
      helpData->fileType = XtNewString(helpData->fileType);

   return((XtPointer) helpData);
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static void
SetValues(
        HelpRec *helpRec,
        HelpData *helpData )

{
   Arg args[10];
   int n;
   String helpString = NULL;
   DtHelpDialogWidget diaShell;

   diaShell = (DtHelpDialogWidget)helpRec->helpDialog;

   n = 0;
   XtSetArg(args[n], DtNhelpVolume, helpData->volString); n++;
   XtSetArg(args[n], DtNhelpType, (unsigned char) helpData->helpType); n++;
   if (helpData->helpType == DtHELP_TYPE_TOPIC)
   {
      XtSetArg(args[n], DtNlocationId, helpData->idString); n++;
   }
   else
   {
      helpString = MapFileTypeToHelpString(helpData->idString,
                                           helpData->fileType);
      XtSetArg(args[n], DtNstringData, helpString); n++;
      XtSetArg(args[n], DtNtopicTitle, helpData->topicTitle); n++;
   }
   XtSetValues(helpRec->helpDialog, args, n);
   XtFree(helpString);
   diaShell = (DtHelpDialogWidget)helpRec->helpDialog;
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
   Arg args[5];
   HelpRec * helpRec;
   HelpData * helpData = (HelpData *) values->data;
   String volString;
   String idString;
   String topicTitle;
   String stringData;
   unsigned char helpType;
   int n;


   /*  If the dialog is currently displayed, update the geometry  */
   /*  fields to their current values.                            */
   if (helpData->displayed == True)
   {
      _DtGenericUpdateWindowPosition(values);
      helpRec = (HelpRec *)_DtGetDialogInstance(values);

      /* Get the current help volume and locationId */
      n = 0;
      XtSetArg(args[n], DtNhelpVolume, &volString); n++;
      XtSetArg(args[n], DtNlocationId, &idString); n++;
      XtSetArg(args[n], DtNtopicTitle, &topicTitle); n++;
      XtSetArg(args[n], DtNstringData, &stringData); n++;
      XtSetArg(args[n], DtNhelpType, &helpType); n++;
      XtGetValues(helpRec->helpDialog, args, n);
      XtFree(helpData->volString);
      helpData->helpType = (int) helpType;
      helpData->volString = XtNewString(volString);
      if (helpData->helpType == DtHELP_TYPE_TOPIC)
      {
         XtFree(helpData->idString);
         XtFree(helpData->topicTitle);
         helpData->idString = XtNewString(idString);
         helpData->topicTitle = NULL;
      }

      _DtDialogPutResources(fd, name_list, HELP_DIALOG, values->data,
                          resources, helpClass->resource_count);
   }
}



/************************************************************************
 *
 *  FreeValues
 *
 ************************************************************************/

static void
FreeValues(
        HelpData *helpData )

{
   XtFree((char *) helpData->topicTitle);
   XtFree((char *) helpData->idString);
   XtFree((char *) helpData->volString);
   XtFree((char *) helpData->fileType);
   XtFree((char *) helpData);
}


/*
 * The application must free up the returned string.
 * If 'miscData' is not NULL, then we are attempting to get the description
 * for a specific action, as defined for a specific filetype.
 */

XtPointer
MapFileTypeToHelpString(
   String filetype,
   String miscData )

{
   char * buf;
   char * helpPattern;
   char * desc;
   Boolean isAction;
   char * tmpStr;
   char * label = NULL;

   /* Special case for the 2 built in directory commands */
   if (strcmp(filetype, openNewView) == 0)
   {
      tmpStr = (GETMESSAGE(29,7, "The `%s` action opens a new File Manager view window\nof the requested folder."));
      desc = XtNewString(tmpStr);

      buf = XtMalloc(strlen(desc) + strlen(filetype) + 100);
      sprintf(buf, desc, filetype);
      XtFree(desc);
      return(buf);
   }
   if (strcmp(filetype, openInPlace) == 0)
   {
      tmpStr =
       (GETMESSAGE(29,8, "The `%s` action displays the requested folder in\nthe current File Manager view window."));
      desc = XtNewString(tmpStr);

      buf = XtMalloc(strlen(desc) + strlen(filetype) + 100);
      sprintf(buf, desc, filetype);
      XtFree(desc);
      return(buf);
   }

   desc = DtDtsDataTypeToAttributeValue(filetype,
                                        DtDTS_DA_DESCRIPTION,
                                        NULL);

   isAction = DtDtsDataTypeIsAction(filetype);

   if (desc)
   {
      char *descptr = XtNewString(desc);
      XtFree((char *)desc);
      if (descptr)
         return descptr;
      else
         return NULL;
   }

   /* No description found for this filetype */
   if (isAction)
   {
      tmpStr=(GETMESSAGE(29,5, "There is no help available for the %s action"));
      helpPattern = XtNewString(tmpStr);
   }
   else
   {
      tmpStr =
         (GETMESSAGE(29,6, "There is no help available for the %s filetype"));
      helpPattern = XtNewString(tmpStr);
   }

   label = DtDtsDataTypeToAttributeValue(filetype,
                                         DtDTS_DA_LABEL,
                                         NULL);
   if(label)
   {
       buf = XtMalloc(strlen(label) + strlen(helpPattern) + 100);
       sprintf(buf, helpPattern, label);
       DtDtsFreeAttributeValue(label);
       label = NULL;
   }
   else
   {
       buf = XtMalloc(strlen(filetype) + strlen(helpPattern) + 100);
       sprintf(buf, helpPattern, filetype);
   }

   XtFree(helpPattern);
   DtDtsFreeAttributeValue(label);
   return (buf);
}
