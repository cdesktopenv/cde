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
/* $XConsortium: Util.c /main/5 1996/09/30 11:29:05 cde-hp $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:         Util.c
 **
 **   Project:	   Cache Creek 
 **
 **   Description: Contains the Help Callback and Utility functions for or
 **                preview tool and DT 3.0 help browser tool.
 **		  
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/* System Include Files  */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/Text.h>

#include <Dt/Help.h>
#include <DtI/HelpP.h>
#include <Dt/HelpDialog.h>
#include <Dt/HelpQuickD.h>


#include <DtI/HelposI.h>
/* Local Includes */
#include "Main.h"
#include "UtilI.h"


/********    Static Function Declarations    ********/



static void CloseQuickHelpCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData);



 



/****************************************************************************
 * Function:         CloseAndExitCB(
 *                              Widget w,
 *                              XtPointer clientData, 
 *                      	XtPointer callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Closes the helpview process when the users
 *                  closes either a man page view or a ascii
 *                  text file view.
 *
 ***************************************************************************/
void CloseAndExitCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{

    /* We just want to go away here */
    exit (0);
}




/****************************************************************************
 * Function:         CloseQuickHelpCB(
 *                              Widget w,
 *                              XtPointer clientData, 
 *                      	XtPointer callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Closes the Quick Help dialog without exiting.
 *
 ***************************************************************************/
static void CloseQuickHelpCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{

    /* We just unmanage the dialog */
    XtUnmanageChild(viewWidget);
}




/****************************************************************************
 * Function:         CloseHelpCB(
 *                              Widget w,
 *                              XtPointer clientData, 
 *                      	XtPointer callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process close requests on all Help Dialog widgets 
 *                  created and managed by this application.
 *
 ***************************************************************************/
void CloseHelpCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{

  Widget helpDialog = (Widget) clientData;

  CacheListStruct *pTemp; 
  CacheListStruct *pTempCurrent; 
 
  pTemp = pCacheListHead;
  
  /* Search our Cache List for the closed help dialog */
  while ((pTemp->helpDialog != helpDialog) && (pTemp != NULL))
     pTemp = pTemp->pNext;
 

  if (pTemp == NULL)
    /* ERROR */
    printf("We did not find our help dialog widget in the cache list??? /n");

  /* Un Map and Clean up the help widget */
  XtUnmanageChild(helpDialog);
  pTemp->inUseFlag  = FALSE;
   

  /* Re-Assign our pTemp to point to our head so we can see 
   * if we have any in-use help dialog in our list.
   */
  pTemp = pCacheListHead;

  /* Check and see if we should exit  */
  while (pTemp != NULL && (pTemp->inUseFlag != TRUE))
     pTemp = pTemp->pNext;

  if (pTemp == NULL)  /* We have no dialog mapped so exit */
    {
      /* Lets clean up our cache list just for fun, prior to exiting */
      pTemp = pCacheListHead;
      while (pTemp != NULL)
	{
         pTempCurrent = pTemp;
         pTemp = pTemp->pNext;
         XtDestroyWidget(pTempCurrent->helpDialog);
        }

      exit (0);

    }
}




/****************************************************************************
 * Function:	    void ProcessLinkCB(
 *                              Widget w,
 *                              XtPointer  clientData, 
 *                      	XtPointer callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process JUMP-NEW and APP-LINK hypertext requests in a 
 *                  given Help Dialog Window.
 *                 
 *                  This is the callback used for the DtNhyperLinkCallback
 *                  on each of the help dialog widges created.
 *
 ****************************************************************************/
void ProcessLinkCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)

{
   
  DtHelpDialogCallbackStruct * hyperData = 
                         (DtHelpDialogCallbackStruct *) callData;
  

  switch (hyperData->hyperType)
    {
      case DtHELP_LINK_JUMP_NEW:

        DisplayTopic (topLevel, hyperData->helpVolume,
                      hyperData->locationId, 0, NULL);
        
      break;

  
      case DtHELP_LINK_MAN_PAGE:
        /* Add support for man links */
        DisplayMan(topLevel, hyperData->specification, NO_EXIT_ON_CLOSE);

      break;

      case DtHELP_LINK_APP_DEFINE:
        /* Send out the proper warning (e.g. message 14) */
      break;

    
      default:  

        /* ERROR */
        /* Send out warning (e.g. message 15) */
      break;


    }  /* End Switch Statement */

 
}





/****************************************************************************
 * Function:	    void DisplayTopic(
 *                              Widget parent,
 *                              char *helpVolume,
 *                              char *locationId)
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Displays a new Cache Creek help topic in a new/cached
 *                  help dialog widget.
 *
 ****************************************************************************/
void DisplayTopic (
    Widget  parent,
    char    *helpVolume,
    char    *locationId,
    int     argc,
    char    **argv)

{
  Arg	 	   args[15];	
  int              n;
  CacheListStruct  *pCurrentNode = NULL;
  Boolean          cachedNode = FALSE;
  char             *titleString;

  /* If we have a null location id within helpview we want to force
   * it to the default: _HOMETOPIC.
   */
   if (locationId == NULL)
      locationId = XtNewString("_HOMETOPIC");

  /* Get a used or new node form our cache if we have one */
  cachedNode = GetFromCache(parent, &pCurrentNode);


  titleString = XtNewString((char*)_DTGETMESSAGE (7,20,"Help Viewer"));

  /* If we got a free one from the Cache, use it */
  /* Set Values on current free one, then map it */
  if (cachedNode)
    {

       n = 0;
       XtSetArg (args[n], XmNtitle, titleString);          n++;
       if (helpVolume != NULL)
         {
           XtSetArg (args[n],DtNhelpVolume,helpVolume);     n++; 
         }
       XtSetArg (args[n], DtNlocationId,locationId);        n++;
       XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC); n++;

       XtSetValues(pCurrentNode->helpDialog, args, n);
  
       XtManageChild(pCurrentNode->helpDialog);    
      }
   else
     {
        /* Build a new one in our cached list */
        n = 0;
        XtSetArg (args[n], XmNuseAsyncGeometry, True);      n++;
        XtSetArg (args[n], XmNtitle, titleString);          n++;
        XtSetArg (args[n], DtNshowTopLevelButton, TRUE);    n++;
        XtSetArg (args[n], DtNshowNewWindowButton, TRUE);    n++;
       if (helpVolume != NULL)
         {
           XtSetArg (args[n],DtNhelpVolume,helpVolume);     n++; 
         }
        XtSetArg (args[n], DtNlocationId,locationId);        n++;
        pCurrentNode->helpDialog =  
                   DtCreateHelpDialog(parent, "helpWidget", args, n);


        XtAddCallback(pCurrentNode->helpDialog, DtNhyperLinkCallback,
                       ProcessLinkCB, NULL);
             
        XtAddCallback(pCurrentNode->helpDialog, DtNcloseCallback,
                      CloseHelpCB,(XtPointer) pCurrentNode->helpDialog);

        XtManageChild(pCurrentNode->helpDialog); 

        /* Set the wm_command property on the help dialog window */
       if (argc != 0)
         XSetCommand(appDisplay, 
                  XtWindow(XtParent(pCurrentNode->helpDialog)), argv, argc);
   
      }

   XtFree(titleString);
  

}




/****************************************************************************
 * Function:	    CacheListStruct GetFromCache(
 *                                  Widget parent);
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Gets a free help node form our cache list.  If none are
 *                  free, it will return fallse and the calling routine will
 *                  create a new help dialog widget.
 *
 ****************************************************************************/
Boolean GetFromCache(
    Widget parent,
    CacheListStruct **pCurrentNode)

{

  CacheListStruct *pTemp; 

  if (pCacheListHead == NULL)
    {
       /* We have a new list so lets create one and pass it back */
       pCacheListHead = 
                (CacheListStruct *) XtMalloc((sizeof(CacheListStruct)));

       /* Assign the default values to our node */
       pCacheListHead->helpDialog = NULL;
       pCacheListHead->inUseFlag  = TRUE;
       pCacheListHead->pNext      = NULL;
       pCacheListHead->pPrevious = NULL;

       /* Assign our tale pointer */
       pCacheListTale = pCacheListHead;

       /* Make sure or totalNodes counter is correct, e.g. force it to 1 */
       totalCacheNodes = 1;
    
       /* Return our head pointer because it's our first and only node */
       *pCurrentNode = pCacheListHead;
       return (FALSE);

    }
  else
    { 
       /* We have some nodes so search for a free one first */
       pTemp = pCacheListHead;

       while (pTemp != NULL)
         {
           if (pTemp->inUseFlag == FALSE)
             {
                pTemp->inUseFlag = TRUE;
                *pCurrentNode = pTemp;
                return (TRUE);
	      }
           else
             pTemp = pTemp->pNext;
	 }
      

       /* If we did not find a free nod then we must add a new one to the
        * top of the list, and return it.
        */

       pTemp =  (CacheListStruct *) XtMalloc((sizeof(CacheListStruct)));

       /* Assign the default values to our node */
       pTemp->helpDialog = NULL;
       pTemp->inUseFlag  = TRUE;
     
       pTemp->pNext      = pCacheListHead;
       pTemp->pPrevious  = NULL;

       pCacheListHead->pPrevious = pTemp;

       /* Re-Assign our head pointer to point to the new head of the list */
       pCacheListHead = pTemp;

       /* Make sure or totalNodes counter is correct, e.g. force it to 1 */
       totalCacheNodes = totalCacheNodes + 1;

       /* Return our head pointer because it's our new node */
       *pCurrentNode = pCacheListHead;
       return (FALSE);

     }
  

}




/****************************************************************************
 * Function:	    void DisplayFile(
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Displays a ascii file in a quick help dialog.
 *
 ****************************************************************************/
void DisplayFile (
    Widget  parent,
    char    *file)

{

  Arg  args[20];
  int    n;
  Widget helpWidget;
  Widget backWidget;
  char *title;
  XmString printString;
  XmString exitString;

      
  /* Create the QuickHelpDialog widget for help on help */
  title = XtNewString(file);
  printString = XmStringCreateLocalized((char *)_DTGETMESSAGE(7,19,"Print ..."));
  exitString = XmStringCreateLocalized((char *)_DTGETMESSAGE(7, 16, "Exit"));

  n =0;
  XtSetArg (args[n], XmNuseAsyncGeometry, True);         n++;
  XtSetArg (args[n], XmNtitle, title);                   n++;
  XtSetArg (args[n], DtNprintLabelString, printString);  n++;
  XtSetArg (args[n], XmNokLabelString, exitString);      n++;
  XtSetArg (args[n], DtNhelpType,DtHELP_TYPE_FILE);     n++; 
  XtSetArg (args[n], DtNhelpFile, file);                 n++;
  XtSetArg (args[n], XmNscrollBarPlacement,
                         DtHELP_AS_NEEDED_SCROLLBARS);       n++;
  viewWidget = DtCreateHelpQuickDialog(parent,"fileBox", args, n);
  XmStringFree(printString);
  XmStringFree(exitString);
  XtFree((char*) title);

  /* Catch the close callback so we can destroy the widget */
  XtAddCallback(viewWidget, DtNcloseCallback,
                CloseAndExitCB, (XtPointer) NULL);

  /* We do not want a help button for now so we unmap it */     
  helpWidget = DtHelpQuickDialogGetChild (viewWidget,DtHELP_QUICK_HELP_BUTTON);
  XtUnmanageChild (helpWidget);
 
  /* We do not want a backtrack button for now so we unmap it */     
  backWidget = DtHelpQuickDialogGetChild (viewWidget,DtHELP_QUICK_BACK_BUTTON);
  XtUnmanageChild (backWidget);
  
  /* Display the dialog */
  XtManageChild(viewWidget);     

}





/****************************************************************************
 * Function:	    void DisplayMan()
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Displays a UNIX man page in a quick help dialog.
 *
 ****************************************************************************/
void DisplayMan (
    Widget  parent,
    char    *man,
    int     closeOption)

{

  Arg  args[20];
  int    n;
  Widget helpWidget;
  char *title;
  XmString printString;
  XmString exitString;
  XmString backString;

  XmUpdateDisplay(topLevel);
 

  if (viewWidget == NULL)
    {    
      /* Create the QuickHelpDialog widget for help on help */
      title = XtNewString(man);
      printString = XmStringCreateLocalized((char *)_DTGETMESSAGE(
							7, 19, "Print ..."));

      backString = XmStringCreateLocalized((char *)_DTGETMESSAGE(
							7, 18, "Backtrack"));

      if (closeOption == EXIT_ON_CLOSE)
        {
          exitString = XmStringCreateLocalized((char *)_DTGETMESSAGE(
							7, 16, "Exit"));
        }
      else
        {
          exitString = XmStringCreateLocalized((char *)_DTGETMESSAGE(
							7, 17, "Close"));
        }

      n =0;
      XtSetArg (args[n], XmNuseAsyncGeometry, True);         n++;
      XtSetArg (args[n], XmNtitle, title);                   n++;
      XtSetArg (args[n], DtNbackLabelString, backString);    n++;
      XtSetArg (args[n], DtNprintLabelString, printString);  n++;
      XtSetArg (args[n], XmNokLabelString, exitString);      n++;
      XtSetArg (args[n], DtNhelpType,DtHELP_TYPE_MAN_PAGE); n++; 
      XtSetArg (args[n], DtNmanPage, man);                   n++;
      XtSetArg (args[n], XmNscrollBarPlacement,
                         DtHELP_AS_NEEDED_SCROLLBARS);           n++;
      viewWidget = DtCreateHelpQuickDialog(parent,"manBox", args, n);
      XmStringFree(printString);
      XmStringFree(exitString);
      XmStringFree(backString);
      XtFree((char*) title);

      if (closeOption == EXIT_ON_CLOSE)
        {
          XtAddCallback(viewWidget, DtNcloseCallback,
                       CloseAndExitCB, (XtPointer) NULL);
       
        }
      else
        {
           XtAddCallback(viewWidget, DtNcloseCallback,
                    CloseQuickHelpCB, (XtPointer) NULL);
        }

      /* We do not want a help button for now so we unmap it */     
      helpWidget = DtHelpQuickDialogGetChild (viewWidget,DtHELP_QUICK_HELP_BUTTON);
      XtUnmanageChild (helpWidget);

      XtManageChild(viewWidget);  
    }
  else
    {
       _DtHelpTurnOnHourGlass(viewWidget);
  
       /* We already have a quick help dialog so re-use it */
       n = 0;
       XtSetArg (args[n], DtNhelpType,DtHELP_TYPE_MAN_PAGE); n++; 
       XtSetArg (args[n], DtNmanPage, man);                   n++;
     
       XtSetValues(viewWidget, args, n);
       

       title = XtNewString(man);
       n = 0;
       XtSetArg (args[n], XmNtitle, title);                   n++;
       XtSetValues(XtParent(viewWidget), args, n);
       XtFree((char*) title);

       XtManageChild(viewWidget); 
       XtMapWidget(XtParent(viewWidget));    
       XRaiseWindow(appDisplay, XtWindow(XtParent(viewWidget)));
       _DtHelpTurnOffHourGlass(viewWidget);       

     }

 



}








