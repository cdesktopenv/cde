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
/* $XConsortium: History.c /main/10 1996/11/22 12:25:49 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        History.c
 **
 **   Project:     Cache Creek (Rivers) Project
 **
 **   Description: Creates an instance of a Cache Creek History Dialog.
 ** 
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>

/* These includes work in R4 and R5 */
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/ScrolledW.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/List.h>
#include <Xm/DialogS.h>

/* Dt  Includes */
#include "Access.h"
#include "bufioI.h"

#include <Dt/Help.h>
#include "DisplayAreaI.h"
#include "DisplayAreaP.h"
#include "StringFuncsI.h"
#include "HelposI.h"
#include "HistoryI.h"
#include "HelpDialogI.h"
#include "HelpDialogP.h"
#include "HelpUtilI.h"
#include "HelpAccessI.h"
#include "FormatI.h"


/*
 * Local Includes
 */


static void CloseHistoryCB (
    Widget w,
    XtPointer client_data,
    XtPointer call_data);
static void ProcessTopicSelectionCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data);
static void ProcessVolumeSelectionCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data);
static void CreateHistoryDialog(
    Widget nw);
static DtHistoryListStruct *AddItemToHistoryList(
     DtHistoryListStruct **pHead,
     XmString  title,
     int topicType,
     Boolean *duplicateItem);
static DtTopicListStruct *PullTopicListFromSelVolumeList(
     Widget nw);
static void UpdateTopicList(
    DtHistoryListStruct *pHistoryList,
    Widget nw,
    int topicType);    
static Boolean IsTopicInHistoryList(
    DtTopicListStruct *pTopicList,
    XmString topicTitle);

/************************************************************************
 * Function: CloseHistoryCB()
 *
 *	Close the History Dialog
 *
 ************************************************************************/
static void CloseHistoryCB (
    Widget w,
    XtPointer client_data,
    XtPointer call_data)
{
  DtHelpDialogWidget hw = (DtHelpDialogWidget) client_data;
 
  /* We unmap the history dialog */
  XtUnmanageChild(hw->help_dialog.history.historyWidget);
  
  /* Re-sensatize the search button so the user can select it agan */
  XtSetSensitive(hw->help_dialog.menu.historyBtn, TRUE);
  XtSetSensitive(hw->help_dialog.browser.btnBoxHistoryBtn, TRUE);
}


/*****************************************************************************
 * Function:	    Boolean _DtHelpDisplayHistoryInfo(Widget nw);
 *                             
 * 
 * Parameters:      nw        Specifies the name of the current help dialog 
 *                            widget.
 *
 * Return Value:
 *
 * Purpose: 	    Displays the pre-created history dialog.
 *
 *****************************************************************************/
void _DtHelpDisplayHistoryInfo(
    Widget nw)
{

  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw;
 
  if (hw->help_dialog.history.historyWidget == NULL)
     CreateHistoryDialog((Widget) hw);
  
   /* if its not managed, manage it */
   if ( XtIsManaged(hw->help_dialog.history.historyWidget) == False )
   {
      /* manage and map the History Dialog */
      XtManageChild(hw->help_dialog.history.historyWidget);
      XtMapWidget(XtParent((Widget)hw->help_dialog.history.historyWidget));
   }
   else  /* if it is managed, bring it forward */
   {
     Widget parent = XtParent(hw->help_dialog.history.historyWidget);
     XRaiseWindow ( XtDisplay(parent), XtWindow(parent) );
   }
}



/*****************************************************************************
 * Function:	    Boolean IsTopicInHistoryList(nw)
 *                             
 * 
 * Parameters:      nw        Specifies the name of the current help dialog 
 *                            widget.
 *
 * Return Value:
 *
 * Purpose: 	    checks the current topic list for a matching value, and
 *                  returns true if found, false if not.
 *
 *****************************************************************************/
static Boolean IsTopicInHistoryList(
    DtTopicListStruct *pTopicList,
    XmString topicTitle)
{

  Boolean done=FALSE;
  
  while (!done && (pTopicList != NULL))
    {
      if (XmStringCompare (topicTitle, pTopicList->topicTitleLbl) == True)
         done= TRUE;
      pTopicList = pTopicList->pNext;
    }

  return(done);

}


/*****************************************************************************
 * Function:	    void UpdateTopicList(
 *                     DtHistoryListStruct *pTopics,
 *                     Widget topicList
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Cleans and recreates a new topic list
 *
 *****************************************************************************/
static void UpdateTopicList(
    DtHistoryListStruct *pHistoryList,
    Widget nw,
    int topicType)    
{
   int i;
   int n;
   Arg args[5];
   XmString labelString;
   XmString *items;
   DtTopicListStruct *pTopicList=NULL;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) nw;
   

   /* Flush the current history topic list */
   XmListDeselectAllItems(hw->help_dialog.history.topicList);
   XmListDeleteAllItems(hw->help_dialog.history.topicList);

   /* Grab the top of our current topic list from our history list struct */
   pTopicList = pHistoryList->pTopicHead;
   
   /* Loop through and build up a new visible topics list */
   items = (XmString *) XtMalloc(sizeof(XmString) * pHistoryList->totalNodes);
   for (i = 0; i < pHistoryList->totalNodes; i++)
     {
       items[i]   = pTopicList->topicTitleLbl;
       pTopicList = pTopicList->pNext;
     }

   XtSetArg(args[0], XmNitems,items);
   XtSetArg(args[1], XmNitemCount, pHistoryList->totalNodes);
   XtSetValues(hw->help_dialog.history.topicList, args, 2);

   XtFree((char *)items);

   /* Now, modify the label if we need to */

   switch (topicType)
     {
       case DtHELP_TYPE_TOPIC:
         labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (3, 8,"Help Topics Visited:")));
         break;  


       case DtHELP_TYPE_FILE:
         labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (3, 9,"Help Files Visited:")));
         break;


       case DtHELP_TYPE_MAN_PAGE:
         labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (3, 10,"Man Pages Visited:")));
         break;


      case DtHELP_TYPE_STRING:
      case DtHELP_TYPE_DYNAMIC_STRING:
        labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (3, 11,"Help Messages Visited:")));
         break;
 
      default:  
        /* Bogus type, how did we ever get here ??? */
        break;

    }  /* End Switch Statement */

   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetValues (hw->help_dialog.history.topicsListLabel, args, n);
   XmStringFree(labelString);


}


/*****************************************************************************
 * Function:	    void ProcessVolumeSelectionCB(Widget w,
 *                                              XtPointer client_data,
 *                                              XtPointer call_data);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process user selection of an item in the Volume List
 *                  of the history dialo.
 *
 *****************************************************************************/
static void ProcessVolumeSelectionCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data)
{
   XmListCallbackStruct *selectedItem = (XmListCallbackStruct *) call_data;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) client_data;
   DtHistoryListStruct *pTemp= NULL;
   int topicPosition=0;  
   int i;   
   XmString labelString;
   int * topicPosList = NULL;
   int   topicCnt = 0;
   Boolean mod = False;
   Arg          args[2];
   XmFontList	fontList;

   /* Determin the item selected in the volume list */
   topicPosition = selectedItem->item_position;

   /* To fix the bug of having no volume selected, check
      whether user clicked again on the currently selected item,
      deselecting it and leaving no other selected, and just
      reselect that item and do nothing else */
   /* for reasons unknown, selectedItem->selected_item_positions
      doesnt seem properly initialized and leads to a core dump;
      so use XmListGetSelectedPos() instead */
   if (   False == XmListGetSelectedPos(w, &topicPosList, &topicCnt)
       && NULL == topicPosList )
   {
      XmListSelectPos(w,topicPosition,False); /* False=dont notify */
      return;                               /* RETURN */
   }
   XtFree((char *)topicPosList);

   /* and find that item in our list */
   pTemp = hw->help_dialog.history.pHistoryListHead;
   for (i=1;i < topicPosition; i++)
      pTemp = pTemp->pNext;
  
  
   UpdateTopicList(pTemp, (Widget)hw, pTemp->topicType);
      
   /* Look to see if we have the current visible topic matches something in
    * in the topic list. If so, highlight the item in the topic list
    */
   XtSetArg(args[0], XmNfontList, &fontList);
   XtGetValues(hw->help_dialog.history.topicList, args, 1);

   if (   _DtHelpFormatTopicTitle (hw->help_dialog.help.pDisplayArea,
				   hw->help_dialog.display.volumeHandle,
				   hw->help_dialog.display.locationId,
				   &labelString, &fontList, &mod) != 0 
       || NULL == labelString)
   {
       labelString = XmStringCreateLocalized(hw->help_dialog.display.locationId);
   }

   if (True == mod)
   {
       /* must set the fontlist, otherwise this will cause a dangle later. */
       XtSetArg(args[0], XmNfontList, fontList);
       XtSetValues(hw->help_dialog.history.topicList, args, 1);
       XmFontListFree(fontList);
   }

   XmListSelectItem(hw->help_dialog.history.topicList,labelString, FALSE);
   XmListSetBottomItem(hw->help_dialog.history.topicList,labelString);
   XmStringFree(labelString);
}





/*****************************************************************************
 * Function:	    void ProcessTopicSelectionCB(Widget w,
 *                                              XtPointer client_data,
 *                                              XtPointer call_data);
 *
 *
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process user selection of an item in the History List.
 *
 *****************************************************************************/
static void ProcessTopicSelectionCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data)
{
   XmListCallbackStruct *selectedItem = (XmListCallbackStruct *) call_data;
   DtHelpDialogWidget hw = (DtHelpDialogWidget) client_data;
   
   DtTopicListStruct *pTemp= NULL;
   int topicPosition=0;  
   int i;


   /* First, find out what item is currently selected in our 
    * volume list.  From this, we can get the proper topic list to
    * travers.
    */

   pTemp = PullTopicListFromSelVolumeList((Widget)hw); 

   if (pTemp == NULL)
     {
       /*FIX: We have a problem, this should never happen; decide what to do*/
       return;
     }


   /* Determin the item selected and find that item in our list */
   topicPosition = selectedItem->item_position;
   
   for (i=1;i < topicPosition; i++)
      pTemp = pTemp->pNext;
  
   hw->help_dialog.display.helpType = pTemp->topicType;

   if (hw->help_dialog.display.helpType != DtHELP_TYPE_TOPIC)
     {
       if (hw->help_dialog.display.topicTitleLbl != NULL)
           XmStringFree(hw->help_dialog.display.topicTitleLbl);
       hw->help_dialog.display.topicTitleLbl =
					XmStringCopy(pTemp->topicTitleLbl);
     }
   

   switch (pTemp->topicType)
     {
       case DtHELP_TYPE_TOPIC:
         /* Look and see if we need to update our helpVolue to a new value */
         if (   pTemp->helpVolume != NULL
             && (   hw->help_dialog.display.helpVolume == NULL
                 || strcmp(hw->help_dialog.display.helpVolume,pTemp->helpVolume) != 0))
         {
           XtFree(hw->help_dialog.display.helpVolume);
           hw->help_dialog.display.helpVolume  = XtNewString(pTemp->helpVolume);
         
           /* Set our help volume flag so we open the proper volume */
           hw->help_dialog.ghelp.volumeFlag         = FALSE;
         }

         XtFree(hw->help_dialog.display.locationId);
         hw->help_dialog.display.locationId = XtNewString(pTemp->locationId);
        
         /* set the topicType flag to process correctly */
         hw->help_dialog.display.helpType = DtHELP_TYPE_TOPIC;

	 break;

       case DtHELP_TYPE_STRING:
         XtFree(hw->help_dialog.display.stringData);
         hw->help_dialog.display.stringData = XtNewString(pTemp->locationId);
        
         /* set the topicType flag to process correctly */
         hw->help_dialog.display.helpType = DtHELP_TYPE_STRING;

         break;
  
       case DtHELP_TYPE_DYNAMIC_STRING:
         XtFree(hw->help_dialog.display.stringData);
         hw->help_dialog.display.stringData = XtNewString(pTemp->locationId);
        
         /* set the topicType flag to process correctly */
         hw->help_dialog.display.helpType = DtHELP_TYPE_DYNAMIC_STRING;

         break;

       case DtHELP_TYPE_MAN_PAGE:
         XtFree(hw->help_dialog.display.manPage);
         hw->help_dialog.display.manPage = XtNewString(pTemp->locationId);
        
         /* set the topicType flag to process correctly */
         hw->help_dialog.display.helpType = DtHELP_TYPE_MAN_PAGE;

	 break;

       case DtHELP_TYPE_FILE:
         XtFree(hw->help_dialog.display.helpFile);
         hw->help_dialog.display.helpFile = XtNewString(pTemp->locationId);
        
         /* set the topicType flag to process correctly */
         hw->help_dialog.display.helpType = DtHELP_TYPE_FILE;

         break;

       default:  
 
        /* ERROR-MESSAGE */
        /* We should never get here, because we using the proper types */
        return;

    }  /* End Switch Statement */
  
    _DtHelpSetupDisplayType(hw, FALSE, DtHISTORY_AND_JUMP);   

}





/*****************************************************************************
 * Function:	    void CreateHistoryDialog(
 *                             Widget nw);
 *
 *
 * Parameters:      helpDialogWidget   Specifies the current Help Dialog to 
 *                                     create the history dialog for.
 *
 *
 * Return Value:   
 *
 * Purpose: 	    Create and display an instance of a history dialog.
 *
 *****************************************************************************/
static void CreateHistoryDialog(
    Widget nw)
{
  Widget historyForm;
  Widget volumeListLabel;
  Widget separator;
  Widget cancelBtn, helpBtn;
  Widget historyShell;
  

  XmString labelString;
  Arg args[20];
  int n;
  char * title;
  DtHelpListStruct *pHelpInfo;
  int offset=0;
  Dimension btnHeight=0;
  Dimension widgetBorderHeight=0;

  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw ;

  /*  Create the shell and form used for the dialog.  */

   title = XtNewString(((char *)_DTGETMESSAGE(3, 1,"Help - History Browser")));
   n = 0;
   XtSetArg (args[n], XmNtitle, title);	 	 			n++;
   XtSetArg (args[n], XmNallowShellResize, TRUE);			n++;
   historyShell = XmCreateDialogShell((Widget) hw, "historyShell", args, n);
   XtFree(title);

   /* Set the useAsyncGeo on the shell */
   n = 0;
   XtSetArg (args[n], XmNuseAsyncGeometry, True); n++;
   XtSetValues (XtParent(historyShell), args, n);


   n = 0;
   XtSetArg (args[n], XmNmarginWidth, 1);				n++;
   XtSetArg (args[n], XmNmarginHeight, 1);				n++;
   XtSetArg (args[n], XmNshadowThickness, 1);				n++;
   XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);			n++;
   XtSetArg (args[n], XmNautoUnmanage, False);				n++;
   historyForm = XmCreateForm (historyShell, "historyForm", args, n);

 
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (3, 2,"Help Volumes Visited:")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 5);				n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);			n++;
   XtSetArg (args[n], XmNtopOffset, 10);                                n++;
   volumeListLabel = 
         XmCreateLabelGadget (historyForm, "volumeListLabel", args, n);
   XtManageChild (volumeListLabel);
   XmStringFree (labelString);


   /* Create our history scrolled list */
   n = 0;
   XtSetArg (args[n], XmNlistSizePolicy, XmCONSTANT);	                n++;
   XtSetArg (args[n], XmNselectionPolicy, XmSINGLE_SELECT);             n++;
   XtSetArg (args[n], XmNresizable, FALSE);                             n++;
   hw->help_dialog.history.volumeList =
            XmCreateScrolledList (historyForm, "historyVolumeList", args, n);
   XtManageChild (hw->help_dialog.history.volumeList);

   XtAddCallback (hw->help_dialog.history.volumeList, 
                 XmNsingleSelectionCallback, 
                 (XtCallbackProc)ProcessVolumeSelectionCB, (XtPointer) hw);
   
   /* Set the constraints on our scrolled list */
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, volumeListLabel);			n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 10);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   XtSetValues (XtParent (hw->help_dialog.history.volumeList), args, n);


   /* Create the Result Label and scrolled list */

   /* Create Result List Label */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                     (3, 8,"Help Topics Visited:")));
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, 
                  XtParent(hw->help_dialog.history.volumeList));        n++;
   XtSetArg (args[n], XmNtopOffset, 10);		        	n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 10);				n++;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   hw->help_dialog.history.topicsListLabel = 
         XmCreateLabelGadget (historyForm, "topicsListLabel", args, n);
   XtManageChild (hw->help_dialog.history.topicsListLabel);
   XmStringFree (labelString);


   /* Create our volume topics list */
   n = 0;
   XtSetArg (args[n], XmNlistSizePolicy, XmCONSTANT);		        n++;
   XtSetArg (args[n], XmNselectionPolicy, XmSINGLE_SELECT);             n++;
   XtSetArg (args[n], XmNresizable, FALSE);                             n++;
   hw->help_dialog.history.topicList =
            XmCreateScrolledList (historyForm, "historyTopicList", args, n);
   XtManageChild (hw->help_dialog.history.topicList);

   XtAddCallback (hw->help_dialog.history.topicList, 
                  XmNsingleSelectionCallback, 
                  (XtCallbackProc)ProcessTopicSelectionCB,
                  (XtPointer) hw);
  
   XtAddCallback (hw->help_dialog.history.topicList, 
                  XmNdefaultActionCallback, 
                  (XtCallbackProc)ProcessTopicSelectionCB,
                  (XtPointer) hw);

   /* Set the constraints on our scrolled list */
   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, hw->help_dialog.history.topicsListLabel);	n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNleftOffset, 10);				n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightOffset, 10);				n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, 70);				n++;
   XtSetValues (XtParent (hw->help_dialog.history.topicList), args, n);

  
   /*  Create a separator between the buttons  */

   n = 0;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget,
                        XtParent(hw->help_dialog.history.topicList));   n++;
   XtSetArg (args[n], XmNtopOffset, 8);				n++;
   separator =  XmCreateSeparatorGadget (historyForm, "separator", args, n);
   XtManageChild (separator);


   /*  Create the action buttons along the bottom */
   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (3, 3,"Close")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   XtSetArg (args[n], XmNmarginWidth, 6);				n++;
   /*XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
    *XtSetArg (args[n], XmNbottomOffset, 3);				n++;
    */
   cancelBtn = XmCreatePushButtonGadget (historyForm, "cancelBtn", args, n);
   
   XtAddCallback(cancelBtn, XmNactivateCallback, 
                (XtCallbackProc)CloseHistoryCB, (XtPointer) hw);
  	     
   XtManageChild (cancelBtn);
   XmStringFree (labelString);

   XtSetArg (args[0], XmNdefaultButton, cancelBtn);
   XtSetValues (historyForm, args, 1);

   /* set the cancel button (for KCancel) */
   n = 0;
   XtSetArg (args[n], XmNcancelButton, cancelBtn); n++;
   XtSetValues (historyForm, args, n);


   /* Build the Help button */

   labelString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (3, 4,"Help")));
   n = 0;
   XtSetArg (args[n], XmNlabelString, labelString);			n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);		n++;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
   XtSetArg (args[n], XmNtopWidget, separator);				n++;
   XtSetArg (args[n], XmNtopOffset, 5);					n++;
   XtSetArg (args[n], XmNmarginHeight, 4);				n++;
   XtSetArg (args[n], XmNmarginWidth, 6);				n++;
   helpBtn = XmCreatePushButtonGadget (historyForm, "helpBtn", args, n);
   XtManageChild (helpBtn);

   /* Setup and add our help callback for this button */
   pHelpInfo = _DtHelpListAdd(DtHELP_historyHelpBtn_STR,
                        (Widget) hw, &hw->help_dialog.help, 
			&hw->help_dialog.help.pHelpListHead);
   XtAddCallback (helpBtn, XmNactivateCallback,
                  _DtHelpCB, (XtPointer) pHelpInfo);
   
   XmStringFree (labelString);


   /*  Adjust the decorations for the dialog shell of the dialog  */
    n = 0;
    XtSetArg(args[n], XmNmwmFunctions,  
             MWM_FUNC_MOVE |MWM_FUNC_RESIZE);                           n++;
    XtSetArg (args[n], XmNmwmDecorations, 
             MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_RESIZEH);	n++;
    XtSetValues (historyShell, args, n);
    
   /* calculate and set the buttons' positions on the form */
   {
   Widget btnList[2];
   unsigned long avgWidth = 73; /* default size, in 10ths of pixel */
   Dimension formWidth;
   XmFontList fontList = NULL;
   Atom xa_ave_width;
#define BTN_MARGINS   4
#define BETW_BTN_SPC  5

   XtSetArg(args[0], XmNfontList, &fontList);
   XtGetValues(hw->help_dialog.history.topicList, args, 1);

   btnList[0] = cancelBtn;
   btnList[1] = helpBtn;

   /* get the average width of the topic list's font */
   xa_ave_width = XmInternAtom(XtDisplay(cancelBtn), "AVERAGE_WIDTH", False);
   _DtHelpXmFontListGetPropertyMax(fontList, xa_ave_width, &avgWidth);

   /* xa_ave_width is given in 10ths of a pixel; convert to pixels by div thru 10 */
   /* we want a 25 column minimum width */
   formWidth = avgWidth * 25 / 10;

   _DtHelpSetButtonPositions(btnList, 2, formWidth, BTN_MARGINS, BETW_BTN_SPC);
   }

   /* Perform the final form layout so our dialog resizes correctly. */
   /* Get height of: bottom buttons and separator */
   n = 0;
   XtSetArg(args[n], XmNheight, &btnHeight);		        n++;
   XtSetArg(args[n], XmNborderWidth, &widgetBorderHeight);	n++;
   XtGetValues(cancelBtn, args, n);
   offset = btnHeight + widgetBorderHeight + 15;
                    /* 20 =='s fudge factor */
  
   n = 0;
   XtSetArg(args[n], XmNborderWidth, &widgetBorderHeight);	n++;
   XtSetArg(args[n], XmNheight, &btnHeight);			n++;
   XtGetValues(separator, args, n);
   offset += btnHeight + widgetBorderHeight;
 


   /* make the bottom attachment for the seporator such that things will fit,
    * and the dialog will size properly.
    */
   n = 0;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomOffset, offset);				n++;
   XtSetValues (XtParent (hw->help_dialog.history.topicList), args, n); 
   
   /* We may have some history values already, so update our newlly 
    * created scrolled list widget.
    */
   
  
   /* Add the popup position callback to our history dialog */
   XtAddCallback (historyShell, XmNpopupCallback, (XtCallbackProc)_DtHelpMapCB,
                  (XtPointer) XtParent(hw));
 
   /* Add our help callback to the shell of the dialog  */
   pHelpInfo = _DtHelpListAdd(DtHELP_historyShell_STR,
                        (Widget) hw, &hw->help_dialog.help, 
			&hw->help_dialog.help.pHelpListHead);
   XtAddCallback(historyForm, XmNhelpCallback,
                 _DtHelpCB, (XtPointer) pHelpInfo);
   
  /* Assign our new search dialog to our widget instance */
  hw->help_dialog.history.historyWidget = historyForm;

}




/*****************************************************************************
 * Function:	    void _DtHelpUpdateHistoryList(char *locationId,
 *                                 int topicType,
 *                                 Widget nw);  
 *
 *
 * Parameters:      parent      Specifies the ID string for the new topic we
 *                              are going to display in the HelpDialog widget.
 *
 *                  helpDialogWidget  Specifies the current help dialog widget.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Updates the Path Display area on top of the help
 *                  dialog.
 *
 *****************************************************************************/
void _DtHelpUpdateHistoryList(
    char *locationId,
    int topicType,
    Boolean vol_changed,
    Widget nw)
{
  DtHistoryListStruct *pCurrentHistoryList=NULL;
  XmString   topicTitle       = NULL;
  XmString   currentItemTitle = NULL;
  Boolean allocTopic=FALSE;
  Boolean dupItem=FALSE;
  Boolean changedSelectedVolume=FALSE;
  Boolean mod=FALSE;
  XmFontList	fontList;
  Arg           args[2];

  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw;


   /* Lets just build a history dialog so we can start populating it. 
    * We won't manage it tell we need it 
    */
   if (hw->help_dialog.history.historyWidget == NULL)
     CreateHistoryDialog((Widget) hw);

  /* Find out what type of topic we are currently processing */
  switch (topicType)
    {
      case DtHELP_TYPE_TOPIC:

	/*
	 * get the font list for the volume list.
	 */
	XtSetArg(args[0], XmNfontList, &fontList);
	XtGetValues(hw->help_dialog.history.topicList, args, 1);

        /* First we get the current topics Title */
	allocTopic = True;
	mod        = False;
	if (_DtHelpFormatTopicTitle (hw->help_dialog.help.pDisplayArea,
					hw->help_dialog.display.volumeHandle,
					locationId, &topicTitle,
					&fontList, &mod) != 0
		|| NULL == topicTitle)
	    topicTitle = XmStringCreateLocalized(locationId);

	if (mod == True)
	  {
	    XtSetArg(args[0], XmNfontList, fontList);
	    XtSetValues(hw->help_dialog.history.topicList, args, 1);
	    XmFontListFree(fontList);
	  }

	/*
	 * get the font list for the volume list.
	 */
	XtSetArg(args[0], XmNfontList, &fontList);
	XtGetValues(hw->help_dialog.history.volumeList, args, 1);

        /* Second, we get the current volume title */
	mod = False;
        _DtHelpFormatVolumeTitle (hw->help_dialog.help.pDisplayArea,
                                 hw->help_dialog.display.volumeHandle,
                                 &currentItemTitle, &fontList, &mod);   

        if(currentItemTitle == NULL &&
				NULL != hw->help_dialog.display.helpVolume)
          currentItemTitle = XmStringCreateLocalized(
					hw->help_dialog.display.helpVolume);
	if (mod == True)
	  {
	    XtSetArg(args[0], XmNfontList, fontList);
	    XtSetValues(hw->help_dialog.history.volumeList, args, 1);
	    XmFontListFree(fontList);
	  }

        break;  


      case DtHELP_TYPE_FILE:
        currentItemTitle = XmStringCreateLocalized(
				((char *)_DTGETMESSAGE(3, 5, "Help Files")));
        topicTitle       = hw->help_dialog.display.topicTitleLbl;
        break;


      case DtHELP_TYPE_MAN_PAGE:
        currentItemTitle = XmStringCreateLocalized(
				((char *)_DTGETMESSAGE(3, 6, "Man Pages")));
        topicTitle       = hw->help_dialog.display.topicTitleLbl;
        break;


      case DtHELP_TYPE_STRING:
      case DtHELP_TYPE_DYNAMIC_STRING:
        currentItemTitle = XmStringCreateLocalized(
				((char *)_DTGETMESSAGE(3, 7, "Help Messages")));
        topicTitle       = hw->help_dialog.display.topicTitleLbl;
        break;
 
      default:  
        /* Bogus type, how did we ever get here ??? */
        break;

    }  /* End Switch Statement */


  /* Now add this item to the history list if necessary.  This fuction
   * will return a pointer to the current working history list item.
   */
   pCurrentHistoryList = AddItemToHistoryList(
                                &(hw->help_dialog.history.pHistoryListHead),
                                currentItemTitle, topicType, &dupItem);

   if (dupItem != TRUE)
     {
       /* We can skip this if it already exists in our history list,else
        * we add the new history item  to the top of the scrolled window
        */
        XmListAddItem(hw->help_dialog.history.volumeList,currentItemTitle, 1);
        XmListSelectPos(hw->help_dialog.history.volumeList,1, FALSE);
     }
   else /* dupItem == True */
     {
       /* We changed to a diferent volume or help type so force it selected */
       XmListSelectItem(hw->help_dialog.history.volumeList,
						currentItemTitle, FALSE);
       changedSelectedVolume = TRUE;
       if (NULL != currentItemTitle)
           XmStringFree(currentItemTitle);
     }

  /* Now add this item to the proper Topic List. */
  if (pCurrentHistoryList != NULL)
    {

      if (!IsTopicInHistoryList(pCurrentHistoryList->pTopicHead, topicTitle))
        {
          /* Add the new topic to the history top of the history list */
          _DtHelpTopicListAddToHead(locationId, topicTitle,topicType, 
                             DtHISTORY_LIST_MAX,
                             hw->help_dialog.display.helpVolume,
                             &(pCurrentHistoryList->pTopicHead),
                             &(pCurrentHistoryList->pTopicTale),
                             &(pCurrentHistoryList->totalNodes),
                             hw->help_dialog.backtr.scrollPosition);

          XmUpdateDisplay((Widget) hw);
          UpdateTopicList(pCurrentHistoryList, (Widget)hw, topicType);

      
          /* Force the top item to be selected */
          XmListSelectPos(hw->help_dialog.history.topicList,1, FALSE);
        }
      else
	{
          if (changedSelectedVolume)
            {
	      if (TRUE == vol_changed)
                  UpdateTopicList(pCurrentHistoryList, (Widget)hw, topicType);
              XmListSelectItem(hw->help_dialog.history.topicList,topicTitle,
                             FALSE);
              /* Make sure the item is visible */
            }
	}
    }
  if (allocTopic == True)
      XmStringFree(topicTitle);
}



/*****************************************************************************
 * Function:	    static DtHistoryListStruct *AddItemToHistoryList(
 *                                
 * Parameters:
 *              title   Specifies the XmString of the title.  If
 *			'duplicateItem' returns False, the caller
 *			must not modify or free this parameter.
 *
 *
 * Return Value:   Pointer to the current history list node that we are going
 *                 to add the item.
 *
 * Purpose: 	   Add or update our history list with a new item
 *
 *****************************************************************************/
static DtHistoryListStruct *AddItemToHistoryList(
     DtHistoryListStruct **pHead,
     XmString  title,
     int topicType,
     Boolean *duplicateItem)
{
  DtHistoryListStruct *pTemp=NULL;
  Boolean done=FALSE;

   /*
    * Walk the list.
    */
   pTemp = (*pHead); 
   *duplicateItem = False;
 
   while (!done && (pTemp != NULL))
     {
       if (XmStringCompare (title, pTemp->itemTitle) == True)
          {
            /* We have a match, so lets use it */
            *duplicateItem = TRUE;
            done = TRUE;
          }
       else
         pTemp = pTemp->pNext;

     }

   if (pTemp == NULL)
     {
       /* We did not find a match, so lets add one to the top of 
        * our current list.
        */
       pTemp =(DtHistoryListStruct *) XtMalloc((sizeof(DtHistoryListStruct)));
    
       
       pTemp->itemTitle        = title;
       pTemp->totalNodes       = 0;
       pTemp->topicType        = topicType;
       pTemp->pNext            = (*pHead);
       pTemp->pTopicHead       = NULL;
       pTemp->pTopicTale       = NULL;

       /* Re-Assign our head pointer to point to the new 
        * head of the list
        */
       (*pHead) = pTemp;
     }

  return (pTemp);

}


/*****************************************************************************
 * Function:	    static DtHistoryListStruct PullTopicListFromSelVolumeList
 *                                
 *
 *
 *
 * Return Value:   Pointer to the current history list node that we are going
 *                 to add the item.
 *
 * Purpose: 	   Add or update our history list with a new item
 *
 *****************************************************************************/
static DtTopicListStruct *PullTopicListFromSelVolumeList(
     Widget nw)
{
  DtHelpDialogWidget hw = (DtHelpDialogWidget) nw;
  DtHistoryListStruct *pTemp=NULL;
  Boolean itemsSelected=FALSE;
  int positionCount;
  int *positionList;  /* Should always be only one item */
  int i;

  
  pTemp = hw->help_dialog.history.pHistoryListHead;

  /* Find out what item is currently selected in our visited volumes list */

  itemsSelected = XmListGetSelectedPos(hw->help_dialog.history.volumeList,
                                            &positionList, &positionCount);

  if (itemsSelected)
    {
      /* We should get in here every time */
         
      for (i=1;i < positionList[0]; i++)
        pTemp = pTemp->pNext;

      free(positionList);  
      return (pTemp->pTopicHead);
    }
  else
    {
      free(positionList);
      return (NULL);
      /* error condition we must account for */
    }

 
}




