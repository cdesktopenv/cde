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
/* $XConsortium: HelpAccess.c /main/6 1996/08/28 16:46:12 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        HelpAccess.c
 **
 **   Project:     Cache Creek (Rivers) Project
 **
 **   Description: This module handles all the help requests within a given
 **                help dialog widget.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <signal.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/RowColumnP.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>

/* Help Library  Includes */
#include "DisplayAreaI.h"
#include "Access.h"
#include "StringFuncsI.h"
#include "HelpDialogI.h"
#include "HelpDialogP.h"

#include <Dt/Help.h>
#include "HelpI.h"
#include "HelposI.h"
#include "HelpUtilI.h"
#include "HelpQuickD.h"
#include "HelpQuickDP.h"
#include "HelpQuickDI.h"



/********    Static Function Declarations    ********/
static void  CloseOnHelpCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data);
static void SetupHelpDialog(
    Widget parent,
    char *helpOnHelpVolume,
    Widget *helpWidget,
    char *locationId);

/********    End Static Function Declarations    ********/




/*****************************************************************************
 * Function:	   static void CloseOnHelpCB(
 *                            Widget w,   
 *                            XtPointer client_data,
 *                            XtPointer call_data);
 *       
 * Parameters:   
 *
 * Return Value:    
 *
 * Purpose: 	   This routine closes the onhelp dialog Widget that we 
 *                 created for our help on help stuff.
 *
 ****************************************************************************/
static void  CloseOnHelpCB(
    Widget w,
    XtPointer client_data,
    XtPointer call_data )
{

  XtUnmanageChild(w);


}



/****************************************************************************
 * Function:          void  _DtHelpCB ()
 * 
 * Parameters:           widget
 *                       clientData
 *                       callData
 *   
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Called when any "F1" key is pressed in a help dialog 
 *                  widget, or a user selects any of the help buttons in 
 *                  any of the help dialogs dialogs.
 *
 ***************************************************************************/
void  _DtHelpCB(
    Widget widget,
    XtPointer clientData,
    XtPointer callData)
{
  char *errorStr;
  
  DtHelpListStruct * pHelpInfo = (DtHelpListStruct *) clientData;
  _DtHelpCommonHelpStuff * help = pHelpInfo->help;

  /* If helpOnHelp resource is set use it, else post an error dialog */
  if (help->helpOnHelpVolume != NULL)
    {
       SetupHelpDialog((Widget)pHelpInfo->widget, help->helpOnHelpVolume,
                     &(help->onHelpDialog), pHelpInfo->locationId);
    }
  else
    {
       /* We have no help available for the help stuff so generate an error */
       /* Do NOT EVER provide a help button */
       /* ??? ERROR */
       errorStr = XtNewString(((char *) _DTGETMESSAGE(6, 4,
                          "Help On Help not provided for this application!")));
       _DtHelpErrorDialog(XtParent(pHelpInfo->widget), errorStr);
       XtFree(errorStr);
    }

}




/*****************************************************************************
 * Function:	    void SetupHelpDialog (); 
 *
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Creates or re-uses a help on help quick help widget.
 *
 *****************************************************************************/
static void SetupHelpDialog(
    Widget parent,
    char *helpOnHelpVolume,
    Widget *helpDialog,
    char *locationId)
{
  Arg  args[10];
  int    n;
  XmString printString;
  XmString closeString;
  XmString backString;
  XmString helpString;
  char *title;


      if (*helpDialog ==  NULL) 
        {
          /* Create the QuickHelpDialog widget for help on help */
          title = XtNewString(((char *)_DTGETMESSAGE(6, 1, "Help On Help")));
          printString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (6, 2,"Print ...")));
          closeString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (6, 3,"Close")));
          backString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (6, 5,"Backtrack")));

          helpString = XmStringCreateLocalized(((char *)_DTGETMESSAGE
                            (6, 6,"Help ...")));

          n =0;
          XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC);   n++; 
          XtSetArg (args[n], XmNtitle, title);                  n++;
          XtSetArg (args[n], DtNprintLabelString, printString); n++;
          XtSetArg (args[n], DtNcloseLabelString, closeString); n++;
          XtSetArg (args[n], DtNbackLabelString, backString);   n++;
          XtSetArg (args[n], DtNhelpLabelString, helpString);   n++;
          XtSetArg (args[n], DtNhelpVolume,helpOnHelpVolume);   n++; 
          XtSetArg (args[n], DtNlocationId, locationId);        n++;
          *helpDialog = DtCreateHelpQuickDialog((Widget)parent, 
                                                "onHelpDialog", args, n);
          XmStringFree(printString);
          XmStringFree(closeString);
          XmStringFree(backString);
          XmStringFree(helpString);
          XtFree((char*) title);

          /* Catch the close callback so we can destroy the widget */
          XtAddCallback(*helpDialog, DtNcloseCallback,
                      CloseOnHelpCB, (XtPointer) NULL);

          /* Add the popup position callback to our history dialog */
          XtAddCallback (XtParent(*helpDialog), XmNpopupCallback,
                        (XtCallbackProc)_DtHelpMapCB, 
                        (XtPointer)XtParent(parent));

 
        }
      else
        {
           n = 0;
           XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC);  n++; 
           XtSetArg (args[n], DtNhelpVolume,helpOnHelpVolume);   n++; 
           XtSetArg (args[n], DtNlocationId, locationId);        n++;
           XtSetValues(*helpDialog, args, n);
  
         }

      /* Display the dialog */
      XtManageChild(*helpDialog);     
      XtMapWidget(XtParent((Widget)*helpDialog));
}



/*****************************************************************************
 * Function:	    void _DtHelpListFree (DtHelpListStruct * * pHead);  
 *
 *
 * Parameters:      pHead   Specifies the head pointer to the help list.
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Frees all elements in a Help List.
 *
 *****************************************************************************/
void _DtHelpListFree(
    DtHelpListStruct * * pHead)
{
  DtHelpListStruct *pTemp = NULL;
  DtHelpListStruct *pTmpHead = *pHead;

   while (pTmpHead != NULL)
     {
       /* Free up each element in the current node */
       pTemp            = pTmpHead;
       pTmpHead         = pTmpHead->pNext;
       pTemp->pNext     = NULL;
      
       /* Free the id String */
       XtFree(pTemp->locationId);
      
       /* Now, free the whole node */
       XtFree((char *)pTemp);
    }
    *pHead = NULL;   /* reset pointer */
}










/*****************************************************************************
 * Function:	    DtHelpListStruct * _DtHelpListAdd(
 *                                       char *locationId,
 *                                       Widget new,
 *                                       DtHelpListStruct **pHead)
 *    
 *                            
 *
 * Parameters:  
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Adds an element to the top of the given topicList.
 *
 *****************************************************************************/
DtHelpListStruct * _DtHelpListAdd(
    char *locationId,
    Widget widget,
    _DtHelpCommonHelpStuff * help,
    DtHelpListStruct **pHead)
{
  DtHelpListStruct *pTemp=NULL;

  /* Add locationId as first element if pHead = NULL */
  if (*pHead == NULL)
    {
      *pHead = (DtHelpListStruct *) XtMalloc((sizeof(DtHelpListStruct)));
      
      /* Assign the passed in values to our first element */
      (*pHead)->locationId      = XtNewString(locationId);
      (*pHead)->widget          = widget;
      (*pHead)->help            = help;
      (*pHead)->pNext           = NULL;
      (*pHead)->pPrevious       = NULL;
       
    }
  else 
    {  /* We have a list so add the new Help item to the top */
     pTemp = (DtHelpListStruct *) XtMalloc((sizeof(DtHelpListStruct)));
    
     pTemp->pNext            = (*pHead);
     pTemp->pPrevious        = NULL;

     (*pHead)->pPrevious     = pTemp;

     pTemp->locationId       = XtNewString(locationId);
     pTemp->widget           = widget;
     pTemp->help             = help;

     /* Re-Assign our head pointer to point to the new head of the list */
     (*pHead) = pTemp;
    }

  return (*pHead);
}

