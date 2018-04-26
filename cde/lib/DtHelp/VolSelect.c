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
/* $XConsortium: VolSelect.c /main/5 1996/08/28 16:48:11 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	VolSelect.c
 **
 **   Project:  Cde DtHelp
 **
 **   Description: Displays and manages a dialog to select volumes
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xresource.h>

#include <Xm/MwmUtil.h>   /* for MWM_ consts */
#include <Xm/Protocols.h>

#include <Xm/BulletinB.h>
#include <Xm/DialogS.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/SelectioB.h>
#include <Xm/SeparatoG.h>

#include <Dt/Help.h>

/*
 * private includes
 */
#include "HelpI.h"
#include "HelposI.h"        /* DTGETMESSAGE */
#include "HelpDialogP.h"
#include "HelpAccessI.h"
#include "FileListUtilsI.h"
#include "FileUtilsI.h"
#include "HelpUtilI.h"
#include "VolSelectI.h"

#ifdef NLS16
#endif

/******** constants *********/

/* message catalog set for VolSelect.c */
#define FUSET     10
/******** types *********/
typedef struct FLSelDlgRec
{
   _DtHelpFileList     fileList;
   Boolean             modalDialog;
   _DtHelpFileDlgChildren dlgWidgets;
} FLSelDlgRec;

/******** variables *********/
/* Setup for the Retrun Translation set for the text field */
/* static char defaultTranslations[] = "<Key>Return: Activate()"; */

/******** functions *********/

/*****************************************************************************
 * Function:        void DestroyDialogCB(Widget w,
 *                                  XtPointer clientData,
 *                                  XtPointer callData);
 *
 *
 *
 * Parameters:
 *
 * Return Value:    Void.
 *
 * Purpose:         Free selection dialog memory
 *
 *****************************************************************************/
static void DestroyDialogCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   FLSelDlgRec * selDlgData = (FLSelDlgRec *) clientData;

   /* recall that the file list is not owned by the dialog */
   XtFree((String)selDlgData);
}


/*****************************************************************************
 * Function:        void CloseDialogCB(Widget w,
 *                                  XtPointer clientData,
 *                                  XtPointer callData);
 *
 *
 *
 * Parameters:
 *
 * Return Value:    Void.
 *
 * Purpose:         Closes the dialog
 *
 *****************************************************************************/
static void CloseDialogCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   FLSelDlgRec * selDlgData = (FLSelDlgRec *) clientData;

   /* unmanage the dialog; cause the close dialog callbacks to be called */
   XtUnmanageChild(selDlgData->dlgWidgets.form);
}



/*****************************************************************************
 * Function:        void SetSelectionCB(Widget w,
 *                                  XtPointer clientData,
 *                                  XtPointer callData);
 *
 *
 *
 * Parameters:
 *
 * Return Value:    Void.
 *
 * Purpose:         Set the selections in the file list
 *
 *****************************************************************************/
static void SetSelectionCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   FLSelDlgRec * selDlgData = (FLSelDlgRec *) clientData;
   int *         selList = NULL;
   int           selCnt = 0;
   int           setCnt = 0;
   int           pos = 0;
   Boolean       freeMem = True;
   _DtHelpFileEntry file;

   /* get the list */

   /* get the list of selected list items */
   XmListGetSelectedPos(selDlgData->dlgWidgets.list, &selList,&selCnt);
   if (NULL == selList)
   {  /* no more items selected ==> deselected last item */
      selCnt = 0;          /* 0 < 1, which is minimum pos */
      selList = &selCnt;
      freeMem = False;     /* not owned by me */
   }

   /* set the files */
   for ( file = selDlgData->fileList, pos = 1, setCnt = 0;
         NULL != file && NULL != selList && setCnt <= selCnt;
         file = _DtHelpFileListGetNext(NULL,file), pos++ )
   {
        if (   setCnt < selCnt  /* fixes bug due to valid value at selList[selCnt] */
            && pos == selList[setCnt] ) 
        {
           file->fileSelected = True;
           setCnt++;
        }
        else
        {
           file->fileSelected = False;
        }
   }

   if (freeMem) XtFree((String)selList);
}



/*****************************************************************************
 * Function:	    Widget CreateFileSelDialog()
 *
 *
 * Return Value:    dialog widget
 *
 * Purpose: 	    Create and display an instance of a file sel dialog.
 *
 *****************************************************************************/
static Widget CreateFileSelDialog(
        DtHelpDialogWidget hw,
        Widget         parent,
        Boolean        modalDialog,
        char *         dlgTitle,
        _DtHelpFileList   in_out_list,
        _DtHelpFileDlgChildren * out_struct)
{
  FLSelDlgRec *  selDlgData;       /* selection Dialog Data */
  XmString       labelString;
  DtHelpListStruct *pHelpInfo;
  int            n;
  Arg            args[20];

#define FSWIDGETS  selDlgData->dlgWidgets             /* short hand */

   /* create and init the dlg data */
   selDlgData = (FLSelDlgRec *) XtCalloc(1,sizeof(FLSelDlgRec));
   if (NULL == selDlgData) return NULL;
   selDlgData->fileList = in_out_list;
   selDlgData->modalDialog = modalDialog;

   /*  Create the shell and form used for the dialog.  */
   if (NULL == dlgTitle)
      dlgTitle = (char *)_DTGETMESSAGE (FUSET, 1, "Help - Volume Selection");

   dlgTitle = XtNewString(dlgTitle);
   n = 0;
   XtSetArg (args[n], XmNtitle, dlgTitle); 	 			n++;
   XtSetArg (args[n], XmNallowShellResize, TRUE);			n++;
   FSWIDGETS.shell = (Widget) XmCreateDialogShell(parent, "fileSelectShell", args, n);
   XtFree(dlgTitle);
   XtAddCallback(FSWIDGETS.shell, XmNdestroyCallback, 
                             DestroyDialogCB,(XtPointer)selDlgData);

   /* Set the useAsyncGeo on the shell */
   n = 0;
   XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
   XtSetValues (XtParent(FSWIDGETS.shell), args, n);

   /* put the form in the shell */
   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);				n++;
   XtSetArg (args[n], XmNmarginHeight, 1);				n++;
   XtSetArg (args[n], XmNshadowThickness, 1);				n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);			n++;
   XtSetArg (args[n], XmNautoUnmanage, False);				n++;
   XtSetArg (args[n], XmNuserData, selDlgData);				n++;
/* XtSetArg (args[n], XmNdialogStyle, (modalDialog ? 
         XmDIALOG_PRIMARY_APPLICATION_MODAL : XmDIALOG_MODELESS));	n++;*/
   FSWIDGETS.form = (Widget) XmCreateForm (FSWIDGETS.shell, "selectForm", args, n);

   /* put the list label in the form */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                       (FUSET, 2,"Help Volumes")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);			n++;
   XtSetArg (args[n], XmNtopOffset, 10);                                n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 5);                               n++;
   XtSetArg (args[n], XmNalignment, XmALIGNMENT_BEGINNING);             n++;
   FSWIDGETS.label = (Widget)  
         XmCreateLabelGadget (FSWIDGETS.form, "listLabel", args, n);
   XtManageChild (FSWIDGETS.label);
   XmStringFree (labelString);

   /* Create the select scrolled list */
   n = 0;
   XtSetArg (args[n], XmNlistSizePolicy, XmCONSTANT);	                n++;
   XtSetArg (args[n], XmNselectionPolicy, XmMULTIPLE_SELECT);           n++;
   XtSetArg (args[n], XmNdoubleClickInterval, 10);/*basically disables*/n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True);                       n++;
   FSWIDGETS.list = (Widget) XmCreateScrolledList (FSWIDGETS.form, "selList", args, n);
   XtManageChild(FSWIDGETS.list);

   XtAddCallback(FSWIDGETS.list, XmNsingleSelectionCallback,
                   SetSelectionCB,(XtPointer) selDlgData);
   XtAddCallback(FSWIDGETS.list, XmNmultipleSelectionCallback,
                   SetSelectionCB,(XtPointer) selDlgData);
   XtAddCallback(FSWIDGETS.list, XmNextendedSelectionCallback,
                   SetSelectionCB,(XtPointer) selDlgData);
/* XtAddCallback(FSWIDGETS.list, XmNdefaultActionCallback, 
                   SetSelectionCB, (XtPointer) selDlgData);*/

   /* Set the constraints on our scrolled list */
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, FSWIDGETS.label);			n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 10);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, 100);				n++;
   /* 100 is just a rough guess of the one that we'll calc & install later */
   XtSetValues (XtParent (FSWIDGETS.list), args, n);

   /* put the prompt label in the form */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                       (FUSET, 3,"Select one or more help volumes")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 20);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 20);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, XtParent(FSWIDGETS.list));		n++;
   XtSetArg (args[n], XmNtopOffset, 5);                                 n++;
   FSWIDGETS.prompt =(Widget)  
         XmCreateLabelGadget (FSWIDGETS.form, "prompt", args, n);
   XtManageChild (FSWIDGETS.prompt);
   XmStringFree (labelString);

   /*  Create a separator between the buttons  */
   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget,FSWIDGETS.prompt);                   n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   FSWIDGETS.separator =  (Widget) XmCreateSeparatorGadget (
                               FSWIDGETS.form, "separator", args, n);
   XtManageChild (FSWIDGETS.separator);

   /**  Create the action buttons along the bottom **/

   /*  Create the close button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (FUSET, 4,"Close")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 10);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 35);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, FSWIDGETS.separator);		n++;
   XtSetArg (args[n], XmNtopOffset, 4);					n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, 4);				n++;
   XtSetArg (args[n], XmNmarginHeight, 3);				n++;
   FSWIDGETS.closeBtn = (Widget) XmCreatePushButtonGadget (
                           FSWIDGETS.form, "closeBtn", args, n);
   XtManageChild (FSWIDGETS.closeBtn);
   XmStringFree (labelString);
   
   XtAddCallback(FSWIDGETS.closeBtn, XmNactivateCallback, 
                (XtCallbackProc)CloseDialogCB, (XtPointer) selDlgData);
  	     
   /* Build the Help button */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (FUSET, 5,"Help")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNleftPosition, 65);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightPosition, 90);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, FSWIDGETS.separator);		n++;
   XtSetArg (args[n], XmNtopOffset, 4);					n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, 4);				n++;
   XtSetArg (args[n], XmNmarginHeight, 3);				n++;
   FSWIDGETS.helpBtn = (Widget) XmCreatePushButtonGadget (
                          FSWIDGETS.form, "helpBtn", args, n);
   XtManageChild (FSWIDGETS.helpBtn);
   XmStringFree (labelString);

   pHelpInfo = _DtHelpListAdd(DtHELP_volSelectHelpBtn_STR,
                        (Widget) hw, &hw->help_dialog.help, 
			&hw->help_dialog.help.pHelpListHead);
   XtAddCallback(FSWIDGETS.helpBtn, XmNactivateCallback, 
                _DtHelpCB, (XtPointer) pHelpInfo);

   /* set the forms default and cancel button (for KCancel) */
   n = 0;
   XtSetArg (args[n], XmNcancelButton, FSWIDGETS.closeBtn); n++;
   XtSetArg (args[n], XmNdefaultButton, FSWIDGETS.closeBtn); n++;
   XtSetValues (FSWIDGETS.form, args, n);

   /* Set the traversal rules */
   XtSetArg (args[0], XmNnavigationType, XmSTICKY_TAB_GROUP); /* tab */
   XtSetValues(FSWIDGETS.list,args,1);
   XtSetValues(FSWIDGETS.closeBtn,args,1);
   XtSetValues(FSWIDGETS.helpBtn,args,1);

   /* set initial focus to list */
   XmProcessTraversal(FSWIDGETS.list,XmTRAVERSE_CURRENT);

#if 0
   /*  Adjust the decorations for the dialog shell of the dialog  */
   n = 0;
   XtSetArg(args[n], XmNmwmFunctions,  MWM_FUNC_MOVE);                 n++;
   XtSetArg(args[n], XmNmwmDecorations, 
             MWM_DECOR_BORDER | MWM_DECOR_TITLE);		       n++;
   XtSetValues (FSWIDGETS.shell, args, n);
#endif

   { /* set the proper offset between the bottom of the list
        and the bottom of the form to maintain a constant sized
        button bar. */
   int offset = 0;
   Dimension widgetBorderHeight = 0;
   Dimension widgetHeight = 0;

   /* first calc offset of list to form bottom based on earlier sizes */
#define KNOWN_OFFSETS_BELOW_LIST 24   /* 2*(4+3) + 2*5 */
   offset = KNOWN_OFFSETS_BELOW_LIST;
   n = 0;
   XtSetArg(args[n], XmNborderWidth, &widgetBorderHeight);      n++;
   XtSetArg(args[n], XmNheight, &widgetHeight);                 n++;
   XtGetValues(FSWIDGETS.closeBtn, args, n);
   offset += widgetHeight + 2 * widgetBorderHeight;
   XtGetValues(FSWIDGETS.separator, args, n);
   offset += widgetHeight + 2 * widgetBorderHeight;
   XtGetValues(FSWIDGETS.prompt, args, n);
   offset += widgetHeight + 2 * widgetBorderHeight;
   XtGetValues(XtParent(FSWIDGETS.list), args, 1);  /* get only first arg */
   offset += widgetBorderHeight;

   /* then set the offset */
   n = 0;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);              n++;
   XtSetArg (args[n], XmNbottomOffset, offset);                         n++;
   XtSetValues (XtParent (FSWIDGETS.list), args, n);
   }

   /* Add the popup position callback to our select dialog */
   XtAddCallback (FSWIDGETS.shell, XmNpopupCallback, 
                  (XtCallbackProc)_DtHelpMapCB, (XtPointer) XtParent(parent));
 
   /* Add our help callback to the form of the dialog  */
   pHelpInfo = _DtHelpListAdd(DtHELP_volSelectShell_STR,
                        (Widget) hw, &hw->help_dialog.help, 
			&hw->help_dialog.help.pHelpListHead);
   XtAddCallback(FSWIDGETS.form, XmNhelpCallback, 
                        _DtHelpCB, (XtPointer) pHelpInfo);

   if (out_struct) *out_struct = FSWIDGETS;
   return FSWIDGETS.form;
}



/*****************************************************************************
 * Function:	    void _DtHelpFileListCreateSelectionDialog()
 *                             
 * 
 * Parameters:      parent      Specifies the parent widget
 *                  modelDialog should the dialog be modal or not
 *                  fileList    list of files from which to select
 *
 * Return Value:    widget for the modal dialog box
 *
 * Purpose: 	    Setup the selection dialog, 
 *                  displays the dialog, allow user selection, and return.
 *
 *****************************************************************************/
Widget _DtHelpFileListCreateSelectionDialog (
        DtHelpDialogWidget hw,
        Widget         parent,
        Boolean        modalDialog,
        char *         dlgTitle,
        XmFontList *   io_titlesFontList,
        _DtHelpFileList   in_out_list,
        _DtHelpFileDlgChildren * out_widgets)
{
   Widget    selDlg;     /* selection dialog */
   _DtHelpFileDlgChildren widgets;
   XmFontList fontList;
   _DtHelpFileEntry file;
   Arg       args[5];
 
   /* create the dialog */
   selDlg = CreateFileSelDialog(hw, parent, modalDialog, 
                              dlgTitle, in_out_list, &widgets);

   /* put the fonts and files in it */
   /** Set the fontlist of our scrolled list **/
   /* first get the font list */
   XtSetArg (args[0], XmNfontList, &fontList);
   XtGetValues (widgets.list, args, 1);

   /* merge with font list of volume titles */
   /* do this by stepping through the fontList's entries and
      appending them to the volume title's font list */
   {
      XmFontContext context;

      if (    (NULL != io_titlesFontList)
           && XmFontListInitFontContext(&context,fontList) == True)
      {
         XmFontListEntry entry;
         for ( entry = XmFontListNextEntry(context) ;
               NULL != entry ;
               entry = XmFontListNextEntry(context) )
         {
            *io_titlesFontList = 
                   XmFontListAppendEntry(*io_titlesFontList,entry);
         }
         XmFontListFreeFontContext(context);
         fontList = *io_titlesFontList;

         /* and install in place */
         XtSetArg (args[0], XmNfontList, fontList);
         XtSetValues (widgets.list, args, 1);
      }
   }  /* end of scope */

   /* populate the list with volumes */
   for ( file = in_out_list;
         NULL != file;
         file = _DtHelpFileListGetNext(NULL,file) )
   {
      XmListAddItem(widgets.list,file->fileTitleXmStr,0);

      /* select item if file is selected */
      if (file->fileSelected) XmListSelectPos(widgets.list,0,False);
   }

   /* manage the dialog and map it */
   XtManageChild(selDlg);
 
   /* pass widgets back to caller */
   if (out_widgets) *out_widgets = widgets;

   return selDlg;
}   

