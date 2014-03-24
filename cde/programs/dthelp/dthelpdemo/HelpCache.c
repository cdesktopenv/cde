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
/* $XConsortium: HelpCache.c /main/4 1995/11/08 09:17:54 rswiston $ */
/*************************************<+>*************************************
 *****************************************************************************
 **
 **  File:        HelpCache.c
 **
 **  Project:	   dthelpdemo demo program
 **
 **  Description: Contains the Help Callbacks and Utility functions for our
 **               demo tool dthelpdemo.
 **
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992, 1993, 1994
 **      Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/* System Include Files  */
#include <stdlib.h>
#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/XmP.h>


#include <Dt/Help.h>
#include <Dt/HelpDialog.h>
#include <Dt/HelpQuickD.h>

/* Local Includes */
#include "Main.h"
#include "HelpCacheI.h"
#include "HourGlassI.h"


/********    Static Function Declarations    ********/


static void CloseHelpCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData);
static Boolean GetFromCache(
    Widget parent,
    CacheListStruct **pCurrentNode);



/* Global Main Help Dialog Widget */
static Widget helpMain=NULL;
static Widget versionMain=NULL;


/*****************************************************************************
 * Function:	    void HelpMapCB()
 *                   
 *                            
 *
 * Parameters:      clientData is the widget in reference to
 *                  which widget w is placed
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Determins where a new child dialog should be mapped in
 *                  relation to its parent.
 *
 * Algorithm:       1. attempt left or right placement with no overlap
 *                  2. if fails, attempt up or down placement with no overlap
 *                  3. if fails, determines location with least
 *                     amount of overlap, and places there.
 *
 *****************************************************************************/
XtCallbackProc HelpMapCB(
    Widget w,
    XtPointer clientData,
    XtPointer callData)

{

    Arg         args[2];
    Widget      parent;
    Position    centeredY, bestX, bestY, pX, pY; 
    Dimension   pHeight, myHeight, pWidth, myWidth;
    Dimension   maxX, maxY;
    int	        rhsX, lhsX, topY, botY;   /* needs to be int, not Dimension */
    Display *   display;
    Screen *    screen;
    int         screenNumber;

    parent = (Widget)clientData;
    display = XtDisplay(w);
    screen = XtScreen(w);
    screenNumber = XScreenNumberOfScreen(screen);
    pX = XtX(parent);
    pY = XtY(parent);
    if (pX < 0) pX = 0;
    if (pY < 0) pY = 0;
    pHeight = XtHeight(parent);
    pWidth = XtWidth(parent);
    myHeight = XtHeight(w);
    myWidth = XtWidth(w);
    maxX = XDisplayWidth(display,screenNumber);
    maxY = XDisplayHeight(display,screenNumber);

    /* algorithm 
     * 1. attempt left or right placement with no overlap
     * 2. if fails, attempt up or down placement with no overlap
     * 3. if fails, places on the right in the middle
     */
    
    /* first try left right placement */
    bestY = pY + pHeight/2 - myHeight/2;
    centeredY = bestY;
    rhsX = pX + pWidth;
    lhsX = pX - myWidth - 8;     /* 8: account for border */
    if ( (rhsX + myWidth) < maxX ) bestX = rhsX;
    else if ( lhsX > 0 ) bestX = lhsX;
    else
    {
          /* then try up down placement */
        bestX = pX + pWidth/2 - myWidth/2;
        botY = pY + pHeight;
        topY = pY - myHeight - 44;     /* 44: account for menu border */
        if ( (botY + myWidth) < maxY ) bestY = botY;
        else if ( topY > 0 ) bestY = topY;
        else
        {
            /* otherwise, center vertically and on the right */
            bestX = maxX - myWidth;
            bestY = centeredY;
        }
    }

    XtSetArg(args[0], XmNx, bestX);
    XtSetArg(args[1], XmNy,  bestY);
    XtSetValues(w, args, 2);

    return((XtCallbackProc) NULL);

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
static void CloseHelpCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{

  Widget helpDialog = (Widget) clientData;

  CacheListStruct *pTemp; 
 
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
   

}



/****************************************************************************
 * Function:         CloseMainCB(
 *                              Widget w,
 *                              XtPointer clientData, 
 *                      	XtPointer callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process close requests on our main help dialog.
 *
 ***************************************************************************/
static void CloseMainCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
   Widget currentDialog = (Widget) clientData;
 


  /* Un Map and Clean up the help widget */
   XtUnmanageChild(currentDialog);

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
  Arg 		    args[20];
  int 		    n;
  Position          xPos, yPos;
  int               appLinkNum=0;
  int               count;
  static Dimension  width=0;
  static Dimension  height=0;
  static Boolean    goBigger=TRUE;

  DtHelpDialogCallbackStruct * hyperData = 
                         (DtHelpDialogCallbackStruct *) callData;
  

  switch (hyperData->hyperType)
    {
      case DtHELP_LINK_JUMP_NEW:

        DisplayTopic (XtParent(w), hyperData->helpVolume,
                      hyperData->locationId);
        
        break;

       case  DtHELP_LINK_MAN_PAGE:

         /* Create and display the requested man page */
        DisplayMan(XtParent(w), hyperData->specification);

        
         break;

      case  DtHELP_LINK_TEXT_FILE:

         /* Create a quick help dialog and display the text file in it */
        
         break;

       case  DtHELP_LINK_APP_DEFINE:

       appLinkNum = atoi(hyperData->specification);

      if (appLinkNum == 100)     /* Move the window */
        {

          /* First Place the window in the upper left */
          n = 0;
          XtSetArg(args[n], XmNx, 0); ++n; 
          XtSetArg(args[n], XmNy, 0); ++n;
          XtSetValues(topLevel, args, n);
        
          /* Now move it down to the center of the display */

          for (count = 1;count < 500; count= count+5)
            {
              n = 0;
              XtSetArg(args[n], XmNx, count); ++n; 
              XtSetArg(args[n], XmNy, count); ++n;
              XtSetValues(topLevel, args, n);
              XmUpdateDisplay(topLevel);
            }

        }


      if (appLinkNum == 101)      /* Resize the window */
        {

        if (width == 0)
          {
            /* Get the current dialog size */
            n =0;
            XtSetArg (args[n], XmNheight, &height);  n++;
            XtSetArg (args[n], XmNwidth, &width);   n++;
            XtGetValues(topLevel, args, n);
          }

        if (goBigger)
          {
            n =0;
            XtSetArg (args[n], XmNheight, height+100);  n++;
            XtSetArg (args[n], XmNwidth, width+50);   n++;
            XtSetValues(topLevel, args, n);
            goBigger = FALSE;
          }
        else
	  {
            /* Go smaller */
            n =0;
            XtSetArg (args[n], XmNheight, height);  n++;
            XtSetArg (args[n], XmNwidth, width);   n++;
            XtSetValues(topLevel, args, n);
            goBigger = TRUE;
          }

        }
 

      if (appLinkNum == 102)  
        {

	}

      if (appLinkNum == 103)  
        {

        }

      break;

      default:  /* Catches any other  applicaion definded link types */

         printf("We some how got a bogus hyptertext link type/n");


    }  /* End Switch Statement */
 
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
void DisplayMan(
    Widget  parent,
    char    *man)

{

  Arg  args[20];
  int    n;
  Widget helpWidget;
  char *title;

  XmUpdateDisplay(topLevel);
 


  if (manWidget == NULL)
    {    
      /* Create the QuickHelpDialog widget for help on help */
      title = XtNewString(man);
 
      n =0;
      XtSetArg (args[n], XmNuseAsyncGeometry, True);         n++;
      XtSetArg (args[n], XmNtitle, title);                   n++;
      XtSetArg (args[n], DtNhelpType,DtHELP_TYPE_MAN_PAGE); n++; 
      XtSetArg (args[n], DtNmanPage, man);                   n++;
      manWidget = DtCreateHelpQuickDialog(topLevel,"manBox", args, n);
      XtFree((char*) title);


      XtAddCallback(manWidget, DtNcloseCallback,
                    CloseMainCB, (XtPointer) manWidget);
 
      /* Add the popup position callback to our man dialog */
      XtAddCallback (XtParent(manWidget), XmNpopupCallback,
                    (XtCallbackProc)HelpMapCB, 
                    (XtPointer)topLevel);
 



      /* We do not want a help button for now so we unmap it */     
      helpWidget = DtHelpQuickDialogGetChild (manWidget,
                              DtHELP_QUICK_HELP_BUTTON);
      XtUnmanageChild (helpWidget);

      XtManageChild(manWidget);  
    }
  else
    {
       TurnOnHourGlass(manWidget);
  
       /* We already have a quick help dialog so re-use it */
       n = 0;
       XtSetArg (args[n], DtNhelpType,DtHELP_TYPE_MAN_PAGE); n++; 
       XtSetArg (args[n], DtNmanPage, man);                   n++;
     
       XtSetValues(manWidget, args, n);
       

       title = XtNewString(man);
       n = 0;
       XtSetArg (args[n], XmNtitle, title);                   n++;
       XtSetValues(XtParent(manWidget), args, n);
       XtFree((char*) title);

       XtManageChild(manWidget); 
       XtMapWidget(XtParent(manWidget));    
       XRaiseWindow(XtDisplay(parent), XtWindow(XtParent(manWidget)));
       TurnOffHourGlass(manWidget);       

     }

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
 * Purpose: 	    Creats and displays a new help dialog w/the requested help
 *                  volume and topic.
 *
 ****************************************************************************/
void DisplayTopic(
    Widget  parent,
    char    *helpVolume,
    char    *locationId)

{
  Arg	 	args[10];	
  int           n;

 CacheListStruct *pCurrentNode = NULL;
  Boolean       cachedNode = FALSE;


  /* Get a inuse node if we have one or a Cached one */
  cachedNode = GetFromCache(parent, &pCurrentNode);


  /* If we got a free one from the Cache, use it */
  /* Set Values on current free one, then map it */
  if (cachedNode)
    {
       n = 0;
       XtSetArg (args[n], XmNtitle, "HelpDemo Help");          n++;
       if (helpVolume != NULL)
         {
           XtSetArg (args[n],DtNhelpVolume,helpVolume);     n++; 
         }
       XtSetArg (args[n], DtNlocationId,locationId);        n++;
       XtSetValues(pCurrentNode->helpDialog, args, n);
  
       XtManageChild(pCurrentNode->helpDialog);    
       XtMapWidget(XtParent(pCurrentNode->helpDialog));
     }
   else
     {
       while (!XtIsSubclass(parent, applicationShellWidgetClass))
         parent = XtParent(parent);
     


        /* Build a new one in our cached list */
        n = 0;
        XtSetArg (args[n], XmNtitle, "Helpdemo Help");          n++;
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
                      CloseHelpCB, (XtPointer) pCurrentNode->helpDialog);

        XtManageChild(pCurrentNode->helpDialog);    
        XtMapWidget(XtParent(pCurrentNode->helpDialog));
     }
}





/****************************************************************************
 * Function:	    void DisplayMain(
 *                              Widget parent,
 *                              char *helpVolume,
 *                              char *locationId)
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Displays help for helpdemo in the one helpDialog window 
 *                  created for the applicaiton.
 *
 ****************************************************************************/
void DisplayMain (
    Widget  parent,
    char    *helpVolume,
    char    *locationId)

{
  Arg	 	args[10];	
  int           n;
 
  if (helpMain != NULL)
    {
       n = 0;
       XtSetArg (args[n], XmNtitle, "hemodemo Help");          n++;
       if (helpVolume != NULL)
         {
           XtSetArg (args[n],DtNhelpVolume,helpVolume);     n++; 
         }
       XtSetArg (args[n], DtNlocationId,locationId);        n++;
       XtSetValues(helpMain, args, n);
  
       XtManageChild(helpMain);    
       
     }
   else
     {
       while (!XtIsSubclass(parent, applicationShellWidgetClass))
         parent = XtParent(parent);
     

        /* Build a new one in our cached list */
        n = 0;
        XtSetArg (args[n], XmNtitle, "Helpdemo Help");       n++;
       if (helpVolume != NULL)
         {
           XtSetArg (args[n],DtNhelpVolume,helpVolume);     n++; 
         }
        XtSetArg (args[n], DtNlocationId,locationId);        n++;
        helpMain = DtCreateHelpDialog(parent, "helpWidget", args, n);

        XtAddCallback(helpMain, DtNhyperLinkCallback,
                       ProcessLinkCB, NULL);
             
        XtAddCallback(helpMain, DtNcloseCallback,
                      CloseMainCB, (XtPointer) helpMain);

        /* Add the popup position callback to our main help dialog */
        XtAddCallback (XtParent(helpMain), XmNpopupCallback,
                        (XtCallbackProc)HelpMapCB, 
                        (XtPointer)parent);
 
        XtManageChild(helpMain);    
        
     }
}






/****************************************************************************
 * Function:	    void DisplayVersion(
 *                              Widget parent,
 *                              char *helpVolume,
 *                              char *locationId)
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Displays the version dialog for the helpdemo program.
 *
 ****************************************************************************/
void DisplayVersion (
    Widget  parent,
    char    *helpVolume,
    char    *locationId)

{
  Arg	 	args[10];	
  int           n;
  Widget        printWidget;
  Widget        helpWidget;
  Widget        backWidget;

 
  if (versionMain != NULL)
    {
       n = 0;
       XtSetArg (args[n], XmNtitle, "Helpdemo Version Dialog");          n++;
       if (helpVolume != NULL)
         {
           XtSetArg (args[n],DtNhelpVolume,helpVolume);     n++; 
         }
       XtSetArg (args[n], DtNlocationId,locationId);        n++;
       XtSetValues(versionMain, args, n);
  
       XtManageChild(versionMain);    
       
     }
   else
     {
       while (!XtIsSubclass(parent, applicationShellWidgetClass))
         parent = XtParent(parent);
     

        /* Build a new one in our cached list */
        n = 0;
        XtSetArg (args[n], XmNtitle, "Helpdemo Version Dialog");       n++;
       if (helpVolume != NULL)
         {
           XtSetArg (args[n],DtNhelpVolume,helpVolume);     n++; 
         }
        XtSetArg (args[n], DtNlocationId,locationId);        n++;
	XtSetArg (args[n], DtNhelpType, DtHELP_TYPE_TOPIC);  n++;
        versionMain = DtCreateHelpQuickDialog(parent,"versionWidget",args,n);

        XtAddCallback(versionMain, DtNcloseCallback,
                      CloseMainCB, (XtPointer) versionMain);


        /* We do not want a print button for now so we unmap it */     
        printWidget = DtHelpQuickDialogGetChild (versionMain, 
                                         DtHELP_QUICK_PRINT_BUTTON);
        XtUnmanageChild (printWidget);
  

        /* We do not want a help button for now so we unmap it */     
        helpWidget = DtHelpQuickDialogGetChild (versionMain, 
                                        DtHELP_QUICK_HELP_BUTTON);
        XtUnmanageChild (helpWidget);
  
        backWidget = DtHelpQuickDialogGetChild (versionMain, 
                                        DtHELP_QUICK_BACK_BUTTON);
        XtUnmanageChild (backWidget);

        XtManageChild(versionMain);    
        
     }
}






/****************************************************************************
 * Function:	    static CacheListStruct GetFromCache(
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
static Boolean GetFromCache(
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
       /* We have need for an in-use help dialog or a new one, so look */
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



















