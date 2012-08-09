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
/* $TOG: WmHelp.c /main/8 1997/06/18 17:30:51 samborn $ */
#ifdef WSM
/* 
 * (c) Copyright 1987, 1988, 1989, 1990, 1992, 1993 HEWLETT-PACKARD COMPANY 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = ""
#endif
#endif

/*
 * Included Files:
 */
#if defined(sun)
#ifndef XK_MISCELLANY
#define XK_MISCELLANY
#endif
#endif
 
#include "WmGlobal.h"
#include "WmError.h"
#include "WmFunction.h"
#include "WmHelp.h"
#include "WmCDecor.h"
#include "WmResParse.h"
#ifdef PANELIST
#include "WmPanelP.h"
#endif /* PANELIST */
#include "WmResNames.h"
#include "WmWrkspace.h"
#include "DataBaseLoad.h"
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/MessageB.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/Shell.h>
#include <Xm/MwmUtil.h>

#include <Dt/HelpDialog.h>
#include <Dt/HelpQuickD.h>

#include <Dt/DtNlUtils.h>

#include <X11/cursorfont.h>


/****************************************************************
 *
 *  Cache List Info Structure Definition.
 *
 ****************************************************************/
typedef struct _CacheListStruct {
    Widget                    helpDialog;
    Boolean                   inUseFlag;
    Boolean                   hLink;
    Boolean                   restored;
    char                      *workspaces;
    Position                  xPos;
    Position                  yPos;
    struct _CacheListStruct   *pNext;
    struct _CacheListStruct   *pPrevious;
} CacheListStruct;

typedef struct _DisplayTopicInfo {
    char                      *workspaces;
    Position                  xPos;
    Position                  yPos;
} DisplayTopicInfo;

/*
 * Function Declarations:
 */

/********    Static Function Declarations    ********/
static int LocateTheControl( 
                        Display *dpy,
                        int screen,
			Widget *pWidget);
static WmScreenData *WmScreenDataFromWidget (
					     Widget theWidget) ;

static int WmDtReturnSelectedWidget (
			 Widget theWidget,
			 Cursor cursor,
			 Widget *pWidget);
static void WmDtDisplayTopic (
			       Widget  parent,
			       char    *accessPath,
			       char    *idString,
			       int     helpType,
			       Widget  wmClient,
			       Boolean wmHelp,
			       ArgList argList,
			       int     argCount,
			       ClientData *pCData,
			       Boolean hyperLink,
			       DisplayTopicInfo *displayTopicInfo);
static void WmDtCloseHelpCB (
			      Widget w,
			      XtPointer clientData,
			      XtPointer callData);
static void WmDtWmCloseHelpCB (
			      Widget w,
			      XtPointer clientData,
			      XtPointer callData);
static void WmDtVersionCloseCB (
				 Widget w,
				 XtPointer clientData,
				 XtPointer callData);
static void WmDtVersionPopupCB (
				 Widget w,
				 XtPointer clientData,
				 XtPointer callData);

static void WmDtProcessLinkCB (
				Widget w,
				XtPointer clientData,
				XtPointer callData);
static Boolean WmDtGetFromCache(
				 CacheListStruct **pCurrentNode,
				 WmScreenData *pSD);
static void ShowHelpDialog(
		      ClientData *pClient);

static void HideHelpDialog(
		      WmScreenData *pSD,
		      Boolean userDismissed );
static void wmDtHelpSetPosition(
				 PtrWsDtHelpData pHelp,
				 WmScreenData *pSD,
				 Boolean restorePos);
static void wmDtHelpMapEventHandler (
				 Widget, 
				 XtPointer, 
				 XEvent *,
				 Boolean *);
static void wmDtErrorDialog(
		WmScreenData *pSD,
		String sMessage,
		String sTitle);
static void wmDtErrorDialogOkCB(
		Widget w,
		XtPointer client_data,
		XtPointer call_data );
static void wmDtErrorDialogPopupCB(
		Widget w,
		XtPointer client_data,
		XtPointer call_data );
static void wmDtErrorDialogHide( 
		WmScreenData *pSD );

/********    End Static Function Declarations    ********/


/*
 * Global Variables:
 */


static  DisplayTopicInfo displayTopicInfo;
static  Widget      versionWidget = (Widget)NULL;

#define  PT_IN_CHILD(X, Y, CHILD) \
         (((X) >=  (CHILD)->core.x) && \
          ((X) <= ((CHILD)->core.x + (Position) (CHILD)->core.width)) && \
          ((Y) >=  (CHILD)->core.y) && \
          ((Y) <= ((CHILD)->core.y + (Position) (CHILD)->core.height)))

#define MAX_WIDGET_PATH 100


/**********************************************************************
 * Function: LocateTheControl()
 *
 *   
 *
 **********************************************************************/
static int
LocateTheControl(
        Display *dpy,
        int screen,
	Widget *pWidget )
{

    Widget           child;
    CompositeWidget  comp_widget;
    int              status;
    Cursor           cursor;
    XEvent           event;
    Window           target_win = None;
    int              buttons = 0;
    int              x,y;
    int              i;
    Window           parent;
    Window           sub;
    int              new_x, new_y;
    int              offset;
    KeySym           keySym;
    Widget           widget = NULL;

    Boolean          foundOne = False;
    int  clientNum, scr;
    WmFpEmbeddedClientData *pECD;
    WmScreenData     *pSD = NULL;



    /* Make the target cursor */
    cursor = XCreateFontCursor (wmGD.display, XC_question_arrow);
    
    /* Grab the pointer using target cursor, letting it roam all over */
    status = XGrabPointer (wmGD.display, RootWindow(wmGD.display, screen), 
			   False,
                           ButtonPressMask|ButtonReleaseMask, GrabModeAsync,
                           GrabModeAsync, None, cursor, CurrentTime);
    if (status != GrabSuccess)
    {
        Warning (((char *)GETMESSAGE(29, 1, 
	"Help on Item failed. Another program may have grabbed the mouse.")));
	return(DtHELP_SELECT_ERROR);
    }
    
    
    /* Grab the Keyboard so we can catch the ESC button press */
    status = XGrabKeyboard(wmGD.display, RootWindow(wmGD.display, screen), 
			   False,
                           GrabModeAsync, GrabModeAsync, CurrentTime);
    if (status != GrabSuccess)
    {
	XUngrabPointer (wmGD.display, CurrentTime);
        Warning (((char *)GETMESSAGE(29, 2, 
	"Help on Item failed. Another program may have grabbed the mouse.")));
	return(DtHELP_SELECT_ERROR);
    }
    
    /* Let the user select a window... */
    while ((target_win == None) || (buttons != 0)) 
    {
	XtAppNextEvent(wmGD.mwmAppContext, &event);
	
	switch (event.type) 
	{
	  case ButtonPress:
	    if (target_win == None) 
	    {
		target_win = event.xbutton.subwindow; /* window selected */
		if (target_win == None)
		    target_win = RootWindow (wmGD.display, screen);
	    }
	    buttons++;
	    break;
	  case ButtonRelease:
	    /* there may have been some down before we started */
	    if (buttons > 0)
		buttons--;
	    break;
	  case KeyPress:
	    /* Look for ESC key press and stop if we get one */
	    if (event.xkey.state & ShiftMask)
		offset = 1;
	    else
		offset = 0;
	    
	    keySym = XLookupKeysym((XKeyEvent *)&event, offset);
	    if (keySym == XK_Escape)
	    {
		XUngrabKeyboard (wmGD.display, CurrentTime);
		XUngrabPointer (wmGD.display, CurrentTime);
		return(DtHELP_SELECT_ABORT);
	    }
        }
	
	XtDispatchEvent(&event);
    } 
    
    XUngrabKeyboard (wmGD.display, CurrentTime);      /* Done with keyboard */
    XUngrabPointer (wmGD.display, CurrentTime);      /* Done with pointer */
    
#ifdef  DEBUGGER
    printf("RootWindow (wmGD.display, screen) = %ld\n", RootWindow (wmGD.display, screen));
    printf("event.xbutton.root       = %ld\n", event.xbutton.root);
    printf("event.xbutton.window     = %ld\n", event.xbutton.window);
    printf("event.xbutton.subwindow  = %ld\n", event.xbutton.subwindow);
    printf("event.xbutton.x          = %d\n",  event.xbutton.x);
    printf("event.xbutton.y          = %d\n",  event.xbutton.y);
    printf("event.xbutton.x_root     = %d\n",  event.xbutton.x_root);
    printf("event.xbutton.y_root     = %d\n",  event.xbutton.y_root);
#endif  /*  DEBUGGER  */
    
    if (target_win == RootWindow (wmGD.display, screen))
        return (DtHELP_SELECT_INVALID);
    
    for (scr = 0; scr < wmGD.numScreens; scr++)
    {
	if (wmGD.Screens[scr].managed &&
	    wmGD.Screens[scr].screen == screen)
	{
	    pSD = (&(wmGD.Screens[scr]));
	    break;
	}
    }
    if (!pSD)
    {
	pSD=wmGD.pActiveSD;
    }

    /* check for subwindows */
    parent = RootWindow (wmGD.display, screen);
    x      = event.xbutton.x_root;
    y      = event.xbutton.y_root;
    
    while (!foundOne) 
    {
        XTranslateCoordinates(wmGD.display, parent, target_win, x, y,
                              &new_x, &new_y, &sub);
        if (!sub) 
	{
            x = new_x;
            y = new_y;
            break;
        }
        else 
	{
	    pECD = (WmFpEmbeddedClientData *) pSD->pECD;
	    for (clientNum = 0; clientNum < pSD->numEmbeddedClients;
		 clientNum++, pECD++)
	    {
		if ((pECD->pCD) &&
		    (target_win == pECD->pCD->client))
		{
		    widget = pECD->wControl;
		    foundOne = True;
		    break;
		}
	    }
            parent = target_win;
            target_win = sub;
            x = new_x;
            y = new_y;
        }
    }
    
    
    if (widget == NULL)
    {
	widget = XtWindowToWidget (wmGD.display, target_win);
    }
    
    if (widget == NULL)
    {
	widget = XtWindowToWidget (wmGD.display1, target_win);
	
	if (widget == NULL)
	{
	    return (DtHELP_SELECT_INVALID);
	}
    }
    
    if (!XtIsComposite (widget))
    {
	*pWidget = widget;
	return (DtHELP_SELECT_VALID);
    }
    comp_widget = (CompositeWidget)widget;
    
    /*  look for gadgets at this point  */
    for (i = 0; i < comp_widget->composite.num_children; i++) 
    {
        child = comp_widget->composite.children[i];

        if (PT_IN_CHILD (x, y, child) && XtIsManaged(child))
	{
	    *pWidget = child;
	    return (DtHELP_SELECT_VALID);
        }
    }

    if (widget != NULL)
    {
       *pWidget = widget;
	return (DtHELP_SELECT_VALID);
    }
    
    return (DtHELP_SELECT_INVALID);
    
} /* END OF FUNCTION LocateTheControl */


/**********************************************************************
 *
 * Cache Creek related help functions
 *
 **********************************************************************/

/*************************************<->*************************************
 *
 *  WmDtStringHelpCB
 *
 *
 *  Description:
 *  -----------
 *
 *
 *************************************<->***********************************/

void WmDtStringHelpCB (Widget theWidget, 
			XtPointer client_data, 
			XtPointer call_data)
{
    WmScreenData *pSD;
    char * theHelpString = (char *)client_data;
    Widget theRealWidget = theWidget;
    if( XmIsGadget(theWidget))
    {
	theRealWidget = XtParent(theWidget);
    }
    pSD = WmScreenDataFromWidget(theWidget);

/*    WmDtDisplayTopic(theRealWidget, NULL, theHelpString, */
    WmDtDisplayTopic(pSD->screenTopLevelW1, NULL, theHelpString, 
		      DtHELP_TYPE_DYNAMIC_STRING, (Widget) NULL, True,
		      NULL, 0, NULL, False, NULL);



} /* END OF FUNCTION WmDtStringHelpCB */


/*************************************<->*************************************
 *
 *  WmDtWmStringHelpCB
 *
 *
 *  Description:
 *  -----------
 *
 *
 *************************************<->***********************************/

void WmDtWmStringHelpCB (Widget theWidget, 
			XtPointer client_data, 
			XtPointer call_data)
{
    WmScreenData *pSD;
    char * theHelpString = (char *)client_data;
    pSD = WmScreenDataFromWidget(theWidget);

#ifdef OLD
    if (XtDisplay(theWidget) == DISPLAY)
    {
	WmDtDisplayTopic(pSD->screenTopLevelW1, 
			  NULL, theHelpString, 
			  DtHELP_TYPE_STRING, theWidget, True,
			  NULL, 0, NULL, False, NULL);
    }
    else
    {
	WmDtDisplayTopic(theWidget, NULL, theHelpString, 
			  DtHELP_TYPE_STRING, theWidget, True,
			  NULL, 0, NULL, False, NULL);

    }
#endif /* OLD */
	WmDtDisplayTopic(pSD->screenTopLevelW1, 
			  NULL, theHelpString, 
			  DtHELP_TYPE_DYNAMIC_STRING, theWidget, True,
			  NULL, 0, NULL, False, NULL);


} /* END OF FUNCTION WmDtWmStringHelpCB */



/*************************************<->*************************************
 *
 *  WmDtDisplayTopicHelp
 *
 *  Description:
 *  -----------
 *  External entry procedure used to display help topics.
 *
 *************************************<->***********************************/

void WmDtDisplayTopicHelp (Widget widget, 
    			char * helpVolume,
    			char * locationID)
{
    WmScreenData *pSD = WmScreenDataFromWidget(widget);

    WmDtDisplayTopic(pSD->screenTopLevelW1, 
		      helpVolume, locationID,
		      DtHELP_TYPE_TOPIC, (Widget) NULL, True,
		      NULL, 0, NULL, False, NULL);
}

/*************************************<->*************************************
 *
 *  WmDtDisplayStringHelp
 *
 *  Description:
 *  -----------
 *  External entry procedure used to display help topics.
 *
 *************************************<->***********************************/

void WmDtDisplayStringHelp (Widget widget, 
    			char * helpString)
{
    WmScreenData *pSD = WmScreenDataFromWidget(widget);

    WmDtDisplayTopic(pSD->screenTopLevelW1, 
			  NULL, helpString, 
			  DtHELP_TYPE_DYNAMIC_STRING, widget, True,
			  NULL, 0, NULL, False, NULL);
}

/*************************************<->*************************************
 *
 *  WmDtWmTopicHelpCB
 *
 *
 *  Description:
 *  -----------
 *  for each icon in the icon box
 *
 *************************************<->***********************************/

void WmDtWmTopicHelpCB (Widget theWidget, 
			XtPointer client_data, 
			XtPointer call_data)
{
    Widget shellWidget;
    WmScreenData *pSD;
    char *helpVolume = 	WM_DT_HELP_VOLUME;
    char *locationID = (char *)client_data;
    pSD = WmScreenDataFromWidget(theWidget);

    shellWidget = theWidget;
    while (!(XtIsSubclass(XtParent(shellWidget), shellWidgetClass)))
    {
        shellWidget = XtParent(shellWidget);
    }

    WmDtDisplayTopic(pSD->screenTopLevelW1, helpVolume, 
		      locationID, 
		      DtHELP_TYPE_TOPIC, shellWidget, True,
		      NULL, 0, NULL, False, NULL);

} /* END OF FUNCTION WmDtWmTopicHelpCB */

/****************************************************************************
 * Function:         WmScreenDataFromWidget(
 *                              Widget theWidget)
 *
 * Parameters:      
 *
 * Return Value:    pSD
 *
 * Purpose: 	   
 *
 ***************************************************************************/
static WmScreenData *
WmScreenDataFromWidget (Widget theWidget) 
{

    WmScreenData *pSD = NULL;
    int scr;
    
    CacheListStruct *pTemp; 

    if (theWidget)
    {
	for (scr = 0; scr < wmGD.numScreens; scr++)
	{
	    if (wmGD.Screens[scr].managed &&
		wmGD.Screens[scr].screen == 
		XScreenNumberOfScreen(XtScreen(theWidget)))
	    {
		pSD = (&(wmGD.Screens[scr]));
		break;
	    }
	} 
	if (!pSD)
	{
	    pSD=wmGD.pActiveSD;
	}
    }
    else
    {
	pSD=wmGD.pActiveSD;
    }
    return(pSD);

} /* END OF FUNCTION WmScreenDataFromWidget */

/****************************************************************************
 * Function:         WmDtCloseHelpCB(
 *                              Widget w,
 *                              caddr_t clientData, 
 *                      	caddr_t callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process close requests on all Help Dialog widgets 
 *                  created and managed by this application.
 *
 ***************************************************************************/
static void WmDtCloseHelpCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
    WmScreenData *pSD = NULL;
    int scr;
    Widget helpDialog = (Widget) clientData;
    
    CacheListStruct *pTemp; 
    
    pSD = WmScreenDataFromWidget(w);
    pTemp = pSD->cachedHelp;

    /* Search our Cache List for the closed help dialog */
    while ((pTemp->helpDialog != helpDialog) && (pTemp != NULL))
    {
	pTemp = pTemp->pNext;
    }
    
    if (pTemp == NULL)
    {
	/* ERROR */
     /* printf("Didn't find our help dialog widget in the cache list??? /n");*/
    }

    XtPopdown (XtParent(helpDialog));

    pTemp->inUseFlag  = FALSE;
    pSD->cachedHelp->hLink  = False;    

    /* Re-Assign our pTemp to point to our head so we can see 
     * if we have any in-use help dialog in our list.
     */
    pTemp = pSD->cachedHelp;
    
} /* END OF FUNCTION WmDtCloseHelpCB */


/****************************************************************************
 * Function:         WmDtWmCloseHelpCB(
 *                              Widget w,
 *                              caddr_t clientData, 
 *                      	caddr_t callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process close requests on all Help Dialog widgets 
 *                  created and managed by this application.
 *
 ***************************************************************************/
static void WmDtWmCloseHelpCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
    WmScreenData *pSD;
    Widget helpDialog = (Widget) clientData;
    pSD = WmScreenDataFromWidget(w);
    HideHelpDialog (pSD, True); 

} /* END OF FUNCTION WmDtWmCloseHelpCB */



/****************************************************************************
 * Function:         WmDtWmPopupHelpCB(
 *                              Widget w,
 *                              caddr_t clientData, 
 *                      	caddr_t callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process 
 *
 *
 ***************************************************************************/
static void WmDtWmPopupHelpCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
    PtrWsDtHelpData  pHelp;
    WmScreenData *pSD;

    pSD = WmScreenDataFromWidget(w);

    pHelp = &pSD->dtHelp;  

    if (pSD->dtHelp.restored)
    {
	/*  set position of dialog to restored position  */
	wmDtHelpSetPosition (pHelp, pSD, True);
	pSD->dtHelp.restored = False;
    }
    else
    {
	/*  set position of dialog relative to client window  */
	wmDtHelpSetPosition (pHelp, pSD, False); 
    }

} /* END OF FUNCTION WmDtWmPopupHelpCB */


/****************************************************************************
 * Function:         WmDtVersionPopupCB(
 *                              Widget w,
 *                              caddr_t clientData, 
 *                      	caddr_t callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process 
 *
 *
 ***************************************************************************/
static void WmDtVersionPopupCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
    Arg	 	setArgs[10];	
    Arg	 	getArgs[10];	
    Dimension   width, height;
    int         x, y;
    int         n;

    if (w)
    {
#ifdef OLD
	n = 0;
	XtSetArg (setArgs[n], XmNx, 0);  n++;
	XtSetArg (setArgs[n], XmNy, 0);  n++;
#endif /* OLD */

        /*
         * Center the Version Dialog on the screen
         */

        n = 0;
        XtSetArg(getArgs[n], XmNheight, &height); n++;
        XtSetArg(getArgs[n], XmNwidth, &width); n++;
        XtGetValues (w, (ArgList) getArgs, n);

        x = (DisplayWidth 
	     (DISPLAY, XScreenNumberOfScreen(XtScreen(w))) - ((int) width))/2;
        y = (DisplayHeight 
	     (DISPLAY, XScreenNumberOfScreen(XtScreen(w))) - ((int) height))/2;
        n = 0;
        XtSetArg(setArgs[n], XmNx, (XtArgVal) x); n++;
        XtSetArg(setArgs[n], XmNy, (XtArgVal) y); n++;
	XtSetValues (w, setArgs, n);
    }

} /* END OF FUNCTION WmDtVersionPopupCB */



/****************************************************************************
 * Function:         WmDtPopupHelpCB(
 *                              Widget helpShellW,
 *                              caddr_t clientData, 
 *                      	caddr_t callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process 
 *
 *
 ***************************************************************************/
static void WmDtPopupHelpCB (
    Widget helpShellW,
    XtPointer clientData,
    XtPointer callData)
{
    char * ptr;
    Atom * workspace_atoms = NULL;
    int num_workspaces=0;

    CacheListStruct *pTemp = (CacheListStruct *)clientData;
    Arg args[10];
    int n;
    
    n = 0;
    XtSetArg (args[n], XmNgeometry, NULL);   n++;
    XtSetArg (args[n], XmNwaitForWm, False); n++;

    if (pTemp)
    {
	if (pTemp->workspaces) 
	{
	    char **atom_names = NULL;
	    do
	    {
		ptr = DtStrchr (pTemp->workspaces, '*');
		if (ptr != NULL) *ptr = NULL;
		
		atom_names = 
		    (char **) XtRealloc ((char *)atom_names,
					 sizeof(char*) * (num_workspaces + 1));
		atom_names[num_workspaces] = XtNewString(pTemp->workspaces);
		num_workspaces++;
		
		if (ptr != NULL)
		{
		    *ptr = '*';
		    pTemp->workspaces = ptr + 1;
		}
	    } while (ptr != NULL);

	    workspace_atoms = (Atom *) XtMalloc(sizeof(Atom) * num_workspaces);
	    XInternAtoms(wmGD.display, atom_names, num_workspaces,
			 True, workspace_atoms); 
	    
	    DtWsmSetWorkspacesOccupied (XtDisplay(helpShellW), 
					XtWindow (helpShellW), 
					workspace_atoms,
					num_workspaces);
	    
	    while (num_workspaces-- > 0)
	      XtFree(atom_names[num_workspaces]);
	    XtFree ((char *) atom_names);
	    XtFree ((char *) workspace_atoms);
	}
	
	XtSetArg (args[n], XmNx, pTemp->xPos);	     n++;
	XtSetArg (args[n], XmNy, pTemp->yPos);	     n++;
	pTemp->restored = False;
	XtRemoveCallback(helpShellW, XmNpopupCallback, 
			 WmDtPopupHelpCB, clientData);
	XtAddCallback(helpShellW, XmNpopupCallback, WmDtPopupHelpCB, NULL);
    }
    else
    {
	XtSetArg (args[n], XmNx, 0);	     n++;
	XtSetArg (args[n], XmNy, 0);	     n++;
    }

    XtSetValues (helpShellW, args, n);


} /* END OF FUNCTION WmDtPopupHelpCB */



/****************************************************************************
 * Function:         WmDtVersionCloseCB(
 *                              Widget w,
 *                              caddr_t clientData, 
 *                      	caddr_t callData
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Process close requests on all Help Dialog widgets 
 *                  created and managed by this application.
 *
 ***************************************************************************/
static void WmDtVersionCloseCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)
{
    XtUnmanageChild (w);

} /* END OF FUNCTION WmDtWmVersionCloseHelpCB */




/****************************************************************************
 * Function:	    void WmDtProcessLinkCB(
 *                              Widget w,
 *                              caddr_t  clientData, 
 *                      	caddr_t callData
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
static void WmDtProcessLinkCB (
    Widget w,
    XtPointer clientData,
    XtPointer callData)

{
    DtHelpDialogCallbackStruct * hyperData = 
	(DtHelpDialogCallbackStruct *) callData;
    
    switch (hyperData->hyperType)
    {
      case DtHELP_LINK_JUMP_NEW:
        WmDtDisplayTopic (XtParent(w), hyperData->helpVolume,
			   hyperData->locationId,
			   /* DtHELP_LINK_JUMP_NEW */ DtHELP_TYPE_TOPIC,
			   (Widget)NULL, False,
			   NULL, 0, NULL, True, NULL);
	break;
	
      default:  /* This catches the application definded link types */
        if (hyperData->hyperType >= 100)
	{  
            /* The application would put any code here to handle their
             * Applicaiton-defined-links.  
             */
	}
        else
	{
	    /* ERROR */
	    /* printf("We some how got a bogus hyptertext link type/n");*/
	}	    
    }  /* End Switch Statement */
    
} /* END OF FUNCTION WmDtProcessLinkCB */


/****************************************************************************
 * Function:	    void WmDtHelpOnVersion(
 *                              Widget parent)
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Displays a new Cache Creek help topic in a new/cached
 *                  help dialog widget.
 *
 ****************************************************************************/
void WmDtHelpOnVersion (
			 Widget  parent)

{
#ifndef NO_DT

    Arg	 	setArgs[10];	
    Arg	 	setArgs2[5];	
    int         n, n2;
    Widget printButton = NULL;
    Widget helpButton = NULL;
    Widget backButton = NULL;
    Widget closeButton = NULL;
    XmString    label = NULL;

   if((versionWidget != NULL) &&
       (XtIsManaged(versionWidget)))
    {
        return ;
    }

    n = 0;
    if (!(versionWidget))
    {
#ifndef NO_MESSAGE_CATALOG
	XtSetArg (setArgs[n], XmNtitle, wmNLS.defaultVersionTitle);     n++;
#else
	XtSetArg (setArgs[n], XmNtitle, "About Workspace Manager");     n++;
#endif


	XtSetArg (setArgs[n], DtNhelpType, DtHELP_TYPE_TOPIC);    n++;
	XtSetArg (setArgs[n], DtNlocationId, "_COPYRIGHT");  n++;
	XtSetArg (setArgs[n], DtNhelpVolume, WM_DT_HELP_VOLUME);     n++; 

	versionWidget = DtCreateHelpQuickDialog (parent, "VersionDialog",
						  setArgs, n);

	XtAddCallback(versionWidget, DtNcloseCallback,
		      WmDtVersionCloseCB, 
		      (XtPointer) versionWidget);

	XtAddCallback(XtParent(versionWidget), XmNpopupCallback,
		      WmDtVersionPopupCB, NULL);

	closeButton = DtHelpQuickDialogGetChild( versionWidget, 
					     DtHELP_QUICK_CLOSE_BUTTON );
	n2=0;

#ifndef NO_MESSAGE_CATALOG
	XtSetArg (setArgs2[n2], DtNcloseLabelString, wmGD.okLabel);    n2++;
#else
	XtSetArg (setArgs2[n2], DtNcloseLabelString, 
	      label=XmStringCreateLocalized ("OK")); n2++;
#endif

	XtSetValues(closeButton, setArgs2, n2);
	if(label)XmStringFree(label);

	printButton = DtHelpQuickDialogGetChild( versionWidget, 
					     DtHELP_QUICK_PRINT_BUTTON );
	helpButton = DtHelpQuickDialogGetChild( versionWidget, 
					     DtHELP_QUICK_HELP_BUTTON );
	backButton = DtHelpQuickDialogGetChild( versionWidget, 
					     DtHELP_QUICK_BACK_BUTTON );

	XtUnmanageChild(printButton);
	XtUnmanageChild(helpButton);
	XtUnmanageChild(backButton);
    }
    else
    {
	XtSetArg (setArgs[n], DtNlocationId, "_COPYRIGHT");  n++;
	XtSetArg (setArgs[n], DtNhelpVolume, WM_DT_HELP_VOLUME);     n++; 
	XtSetValues(versionWidget, setArgs, n);
    }
    XtManageChild(versionWidget);

#endif /* NO_DT */

} /* END OF FUNCTION WmDtHelpOnVersion  */



/******************************<->*************************************
 *
 *  Boolean WmDtHelp (String args)
 *
 *  Description:
 *  -----------
 *  Invoke item help 
 *
 *  Inputs:
 *  ------
 *  args - 
 * 
 *  Outputs:
 *  -------
 *  Return - True if the find a help callback; false otherwise.
 *
 *  Comments:
 *  --------
 *
 ******************************<->***********************************/

Boolean
WmDtHelp (String args)
{
    WmScreenData *pSD;
    Widget theWidget;
    WmPanelistObject  pPanelist;
    char *theHelpVolume = WM_DT_HELP_VOLUME;
    char *theHelpTopic  = WM_DT_HELP_TOPIC;
    char volume[MAXWMPATH + 1];
    char topic[MAXWMPATH + 1];
    int  argCount = 0;


    /* get right screen data */
/*    pSD = WmScreenDataFromWidget(w);*/
    pSD = wmGD.pActiveSD;


    pPanelist = (WmPanelistObject) pSD->wPanelist;
    
    if (theWidget = O_Panel(pPanelist))
    {
	if (args )
	{
	    /*
	     * parse args for volume and topic 
	     */
	    WmDtGetHelpArgs((unsigned char*)args, &volume, &topic, &argCount);
	    if (argCount == 1)
	    {
		WmDtDisplayTopic(pSD->screenTopLevelW1, 
				  theHelpVolume, 
				  topic, 
				  DtHELP_TYPE_TOPIC, theWidget, True,
				  NULL, 0, NULL, False, NULL);
	    }
	    else
	    {
		WmDtDisplayTopic(pSD->screenTopLevelW1, 
				  volume, 
				  topic, 
				  DtHELP_TYPE_TOPIC, theWidget, False,
				  NULL, 0, NULL, False, NULL);
	    }
	    
	}
	else
	{
	    WmDtDisplayTopic(pSD->screenTopLevelW1, 
			      theHelpVolume, 
			      theHelpTopic, 
			      DtHELP_TYPE_TOPIC, theWidget, True,
			      NULL, 0, NULL, False, NULL);
	}
	return(True);
    }
    else
    {
	return (False);
    }
}  /* END OF FUNCTION WmDtHelp */


/******************************<->*************************************
 *
 *  Boolean WmDtHelpMode (void)
 *
 *  Description:
 *  -----------
 *  Invoke item help mode
 *
 *  Inputs:
 *  ------
 *  args - 
 * 
 *  Outputs:
 *  -------
 *  Return - True if the find a help callback; false otherwise.
 *
 *  Comments:
 *  --------
 *
 ******************************<->***********************************/

Boolean
WmDtHelpMode (void)
{
    Widget selectedWidget = NULL;
    int iStatus;
    String sTmp, sMessage, sTitle;

    iStatus = WmDtReturnSelectedWidget(wmGD.topLevelW, NULL, 
			&selectedWidget);

    switch (iStatus)
    {
	case DtHELP_SELECT_VALID:
	    if (selectedWidget != (Widget) NULL)
	    {
		do
		{
		    if ((XtHasCallbacks(selectedWidget, 
			    XmNhelpCallback) == XtCallbackHasSome))
		    {
			XtCallCallbacks(selectedWidget, XmNhelpCallback, 
			    (XtPointer)NULL);
			return(True);
		    }
		    else
		    {
			selectedWidget = XtParent(selectedWidget);
		    }
		}
		while (selectedWidget != NULL);
	    }
	    break;

	case DtHELP_SELECT_INVALID:
	    /* invalid selection error (window not of this process) */
#ifndef NO_MESSAGE_CATALOG
	    sTmp = (char *)GETMESSAGE(29, 3, 
		 "You must select a Workspace Manager\n\
dialog, an item in the Front Panel,\n\
or an item in a subpanel.");
            sMessage = XtNewString (sTmp);
	    sTmp = (char *)GETMESSAGE(29, 4, 
		"Workspace Manager - Help On Item Error");
            sTitle = XtNewString (sTmp);
#else
	    sMessage = "You must select a Workspace Manager\n\
dialog, an item in the Front Panel,\n\
or an item in a subpanel.";
	    sTitle = "Workspace Manager - Help On Item Error";
#endif
            wmDtErrorDialog (ACTIVE_PSD, sMessage, sTitle);
#ifndef NO_MESSAGE_CATALOG
	    if (sMessage) XtFree (sMessage);
	    if (sTitle) XtFree (sTitle);
#endif
	    break;

	case DtHELP_SELECT_ERROR:
	    /* error (like grab failed) */
	    break;

	case DtHELP_SELECT_ABORT:
	    /* user aborted this selection */
	    break;
    } /* end switch */

    return (False);
}  /* END OF FUNCTION WmDtHelpMode */



/*****************************************************************************
 * Function:	 WmDtReturnSelectedWidget (Widget, Cursor, Widget *) 
 *
 * Parameters:   Widget      Specifies a valid widget within the app's
 *                           widget hierarchy.
 *	         Cursor	     Cursor to grab pointer with (use NULL!!)
 *               Widget *    ptr to loc to receive returned widget
 *
 * Return Value:  DtHELP_SELECT_ERROR  - error in getting the widget
 *                DtHELP_SELECT_ABORT  - user aborted
 *                DtHELP_SELECT_INVALID - user selected invalid window
 *                DtHELP_SELECT_VALID  - valid widget returned
 *
 * Purpose: Allows developers to get the widget ID for any widget in their UI
 *          that the user has selected vai the pointer.  This function will
 *          cause the cursor to change and allow a user to select an item in 
 *          the UI.
 *
 *****************************************************************************/
static int
WmDtReturnSelectedWidget(
	Widget theWidget, 
	Cursor cursor, 
	Widget *pWidget )

{
    Display   *dpy;
    int       screen;  
    int	      iStatus;
 
    /* Setup some needed variables */
    dpy = XtDisplay(theWidget);
    screen = ACTIVE_SCREEN;
 
    /* refresh the display */
    XmUpdateDisplay(theWidget);

    /* Change the curser to let the user select the desired widget */
    iStatus = LocateTheControl(dpy, screen, pWidget);
    
    return (iStatus);
    
} /* END OF FUNCTION WmDtReturnSelectedWidget */


/****************************************************************************
 * Function:	    void WmDtDisplayTopic(
 *                              Widget parent,
 *                              char *helpVolume,
 *                              char *locationId,
 *                              Widget wmClient,
 *                              Boolean wmHelp,
 *                              ArgList argList,
 *                              int argCount,
 *			        ClientData *pCData,
 *                              Boolean  hyperlink,
 *                              DisplayTopicInfo   *displayTopicInfo)
 *
 * Parameters:      
 *
 * Return Value:    Void.
 *
 * Purpose: 	    Displays a new Cache Creek help topic in a new/cached
 *                  help dialog widget.
 *
 ****************************************************************************/
static void
WmDtDisplayTopic (
    Widget  parent,
    char    *helpVolume,
    char    *locationId,
    int      helpType,
    Widget  wmClient,
    Boolean wmHelp ,
    ArgList argList,
    int argCount,
    ClientData *pCData,
    Boolean hyperLink,
    DisplayTopicInfo *displayTopicInfo)

{
#ifndef NO_DT
    Arg	 	setArgs[10]; 
    Arg	 	smallArgs[2];	
    ArgList  argsNew;
    int         n;
    int         ac;
    Widget      helpWidget;
    Widget      shellWidget;
    char *      helpTitle;
    WmPanelistObject  pPanelist;
    char *      defaultTitle="Help";
    ClientData *pCD = NULL;
    WmScreenData *pSD = NULL;
    CacheListStruct *pCurrentNode = NULL;
    Boolean       cachedNode = FALSE;


    pSD = WmScreenDataFromWidget (parent);


    pPanelist = (WmPanelistObject) pSD->wPanelist;
    
    if (argCount)
    {
	/*argsNew = (ArgList) XtMalloc( sizeof( Arg) * (argCount + 1)) ;*/
	argsNew = (ArgList) XtMalloc( sizeof( Arg) * (argCount)) ;
	memcpy( argsNew, argList, (sizeof( Arg) * argCount)) ;
    }

    shellWidget = parent;
    while (!(XtIsSubclass(shellWidget, shellWidgetClass)))
    {
	shellWidget = XtParent(shellWidget);
    }

    n = 0;
    switch (helpType)
    {
      case  DtHELP_TYPE_TOPIC:
	if (helpVolume != NULL)
	{
	    XtSetArg (setArgs[n],DtNhelpVolume,helpVolume);     n++; 
	}
	XtSetArg (setArgs[n], DtNlocationId,locationId);        n++;
	helpTitle=locationId;
	break;
	
      case DtHELP_TYPE_DYNAMIC_STRING:
	XtSetArg (setArgs[n], DtNstringData, locationId);        n++;
/*	helpTitle=locationId;*/
	helpTitle=defaultTitle;
	break;
	
      case DtHELP_TYPE_MAN_PAGE:
	break;
	
      case DtHELP_TYPE_FILE:
	
	break;
	
      default:
	/* error condition, don't try to display help */
	break;
    }

    if (n)
    {
	XtSetArg (setArgs[n], DtNhelpType, helpType);        n++;    

	if (wmHelp)
	{
	    if (pSD->dtHelp.shell)
	    {
		helpWidget = pSD->dtHelp.dialog;
		XtSetValues(helpWidget, setArgs, n);
	    }
	    else
	    {
#ifndef NO_MESSAGE_CATALOG
		XtSetArg (setArgs[n], XmNtitle, 
			  wmNLS.defaultDtwmHelpTitle);     n++;
#else
		XtSetArg (setArgs[n], XmNtitle, 
				"Workspace Manager - Help");     n++;
#endif

		pSD->dtHelp.dialog =  
		    DtCreateHelpDialog(shellWidget, "helpWidget", setArgs, n);
		helpWidget = pSD->dtHelp.dialog;
		pSD->dtHelp.shell = XtParent(helpWidget);
		pSD->dtHelp.bMapped = False;

		n = 0;
		XtSetArg (setArgs[n], XmNname, "wmHelp");     n++;
		if (argCount)
		{
		    XtSetValues(pSD->dtHelp.shell, 
				argList, argCount);
		}

		XtAddCallback(helpWidget, 
			      DtNhyperLinkCallback,
			      WmDtProcessLinkCB, NULL);
		
		XtAddCallback(helpWidget, DtNcloseCallback,
                              WmDtWmCloseHelpCB,
                              (XtPointer) helpWidget);

		XtAddCallback(pSD->dtHelp.shell, XmNpopupCallback,
			      WmDtWmPopupHelpCB, NULL);

		XtAddEventHandler(pSD->dtHelp.shell, 
			StructureNotifyMask, 
			False, 
			wmDtHelpMapEventHandler, 
			(XtPointer) pSD);

	    }
	    if(pCData)
	    {
		ShowHelpDialog(pCData);
	    }
	    else
	    {
		if (wmClient)
		{
		    XFindContext (DISPLAY, XtWindow(XtParent(wmClient)),
				  wmGD.windowContextType, (caddr_t *)&pCD);
		}
		else
		{
		    XFindContext (DISPLAY, XtWindow(O_Shell(pPanelist)),
				  wmGD.windowContextType, (caddr_t *)&pCD);
		}
		ShowHelpDialog(pCD);
	    }		
	}     /* if (wmHelp) */
	else
	{
	    /* Get a used or new node form our cache if we have one */
	    cachedNode = WmDtGetFromCache(&pCurrentNode, pSD);

	    /* If we got a free one from the Cache, use it */
	    /* Set Values on current free one, then map it */
	    if (cachedNode)
	    {
		XtSetValues(pCurrentNode->helpDialog, setArgs, n);
	    }
	    else
	    {
#ifndef NO_MESSAGE_CATALOG
		XtSetArg (setArgs[n], XmNtitle, 
			  wmNLS.defaultHelpTitle);     n++;
#else
		XtSetArg (setArgs[n], XmNtitle, 
				"Workspace Manager - Help");     n++;
#endif

		XtSetArg (setArgs[n], XmNgeometry, NULL); n++;
                XtSetArg (setArgs[n], XmNx, 0); n++;
                XtSetArg (setArgs[n], XmNy, 0); n++;

		pCurrentNode->helpDialog =  
		    DtCreateHelpDialog(shellWidget, "helpWidget", setArgs, n);
		n = 0;
		XtSetArg (setArgs[n], XmNname, "cachedHelp");     n++;
		if (argCount)
		{
		    XtSetValues(shellWidget, argList, argCount);
		}

		XtAddCallback(pCurrentNode->helpDialog, 
			      DtNhyperLinkCallback,
			      WmDtProcessLinkCB, NULL);
		
		XtAddCallback(pCurrentNode->helpDialog, DtNcloseCallback,
			      WmDtCloseHelpCB, 
			      (XtPointer) pCurrentNode->helpDialog);

		if (hyperLink)
		{
		    pCurrentNode->hLink = True;		    
		}
		else
		{
		    if (displayTopicInfo)
		    {
			pCurrentNode->workspaces = 
			    displayTopicInfo->workspaces;
			pCurrentNode->xPos = displayTopicInfo->xPos;
			pCurrentNode->yPos = displayTopicInfo->yPos;
			pCurrentNode->restored = True;
			XtAddCallback(XtParent(pCurrentNode->helpDialog), 
				      XmNpopupCallback,
				      WmDtPopupHelpCB, 
				      (XtPointer)pCurrentNode);
		    }
		    else
		    {
			XtAddCallback(XtParent(pCurrentNode->helpDialog), 
				      XmNpopupCallback,
				      WmDtPopupHelpCB, NULL);
		    }
		}

	    }
	    helpWidget = pCurrentNode->helpDialog;
	    XtManageChild(helpWidget);    
	    XtPopup(XtParent(helpWidget), XtGrabNone);
	}

    } /* if n is still 0, we did not find a useful help type */
#endif /* NO_DT */
} /* END OF FUNCTION WmDtDisplayTopic  */


/****************************************************************************
 * Function:	    static Boolean WmDtGetFromCache(
 *				         CacheListStruct **pCurrentNode)
 *
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
static Boolean WmDtGetFromCache(
				 CacheListStruct **pCurrentNode,
				 WmScreenData       *pSD)
{
    CacheListStruct *pTemp; 
    
    if (pSD->cachedHelp == NULL)
    {
	/* We have a new list so lets create one and pass it back */
	pSD->cachedHelp = 
	    (CacheListStruct *) XtMalloc((sizeof(CacheListStruct)));
	
	/* Assign the default values to our node */
	pSD->cachedHelp->helpDialog = NULL;
	pSD->cachedHelp->inUseFlag  = TRUE;
	pSD->cachedHelp->restored  = False;
	pSD->cachedHelp->hLink  = False;
	pSD->cachedHelp->pNext      = NULL;
	pSD->cachedHelp->pPrevious = NULL;
	
	/* Return our head pointer because it's our first and only node */
	*pCurrentNode = pSD->cachedHelp;
	return (FALSE);
    }
    else
    { 
	/* We have some nodes so search for a free one first */
	pTemp = pSD->cachedHelp;
	
	while (pTemp != NULL)
	{
	    if (pTemp->inUseFlag == FALSE)
	    {
                pTemp->inUseFlag = TRUE;
		pTemp->hLink  = False;	
		pTemp->restored  = False;
                *pCurrentNode = pTemp;
                return (TRUE);
	    }
	    else
		pTemp = pTemp->pNext;
	}
	
	
	/* 
	 * If we did not find a free node then we must add a new one to the
	 * top of the list, and return it.
	 */
	
	pTemp =  (CacheListStruct *) XtMalloc((sizeof(CacheListStruct)));
	
	/* Assign the default values to our node */
	pTemp->helpDialog = NULL;
	pTemp->inUseFlag  = TRUE;
	pTemp->restored  = False;
	pTemp->hLink  = False;	
	pTemp->pNext      = pSD->cachedHelp;
	pTemp->pPrevious  = NULL;
	
	pSD->cachedHelp->pPrevious = pTemp;

	/* Re-Assign our head pointer to point to the new head of the list */
	pSD->cachedHelp = pTemp;
	
	/* Return our head pointer because it's our new node */
	*pCurrentNode = pSD->cachedHelp;
	return (FALSE);
    }
    
} /* END OF FUNCTION WmDtGetFromCache */

/* End of Dt utils */


/*************************************<->*************************************
 *
 *  void
 *  RestoreHelpDialogs (pSD)
 *
 *
 *  Description:
 *  -----------
 *  Saves the state of the help system
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  Return = none
 *
 *
 *  Comments:
 *  --------
 * 
 ******************************<->***********************************/

Boolean
RestoreHelpDialogs(
        WmScreenData *pSD)
{
#ifndef NO_DT
    XrmDatabase db;
    XrmName xrm_name[5];
    XrmRepresentation rep_type;
    XrmValue value;

    Position xLoc, yLoc;
    short columns, rows;
    int helpType;
    char geometry[40];
    int wsCnt;    
    int cCount;
    int cachedCount=0;		
    char dialogName[10];

    ClientData *pCD = NULL;
    char *tTitle = NULL;
    char *wsName = NULL;
    char *wmClientName = NULL;
    char *helpVolume = NULL;
    char *locationId = NULL;
    char *stringData = NULL;
    char *workspaces = NULL;

    Arg setArgs[20];
    int ac;

    WmPanelistObject  pPanelist = (WmPanelistObject) pSD->wPanelist;

    xLoc = 0;
    yLoc = 0;
    columns = 40;
    rows = 15;

    if (pSD->helpResources)
    {
	if ((db = XrmGetStringDatabase (pSD->helpResources)) == NULL) 
	{
	    return(False);
	}
	xrm_name [0] = XrmStringToQuark ("wsHelp");
	xrm_name [1] = XrmStringToQuark ("onScreen");
	xrm_name [2] = NULL;
	
	
	if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)) 
	{
	    /* Are we supposed to be mapped? */
	    if (strcmp(value.addr, "True") == 0)
	    {
		ac = 0;
		/* get x position */
		xrm_name [1] = XrmStringToQuark ("x");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    pSD->dtHelp.xPos = (Position)atoi((char *)value.addr);
		    pSD->dtHelp.restored = True;
		}
		
		/* get y position */
		xrm_name [1] = XrmStringToQuark ("y");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    pSD->dtHelp.yPos = (Position)atoi((char *)value.addr);
		    pSD->dtHelp.restored = True;
		}

		/* get columns */
		xrm_name [1] = XrmStringToQuark ("columns");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    XtSetArg (setArgs[ac], XmNcolumns, 
			      (short)atoi((char *)value.addr)); ac++;
		    columns = (short)atoi((char *)value.addr);
		}

		/* get rows */
		xrm_name [1] = XrmStringToQuark ("rows");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    XtSetArg (setArgs[ac], XmNrows, 
			      (short)atoi((char *)value.addr)); ac++;
		    rows = (short)atoi((char *)value.addr);
		}
		sprintf (geometry, "=+%d+%d", xLoc, yLoc);
		
		XtSetArg (setArgs[ac], XmNgeometry, 
			  XtNewString(geometry)); ac++;

		/* get visiblePathCount */
		xrm_name [1] = XrmStringToQuark ("vPCount");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    XtSetArg (setArgs[ac], DtNvisiblePathCount, 
			      (int)atoi((char *)value.addr)); ac++;
		}

		/* get topicTitle */
		xrm_name [1] = XrmStringToQuark ("tTitle");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    tTitle = XtNewString ((char *)value.addr);
		    XtSetArg (setArgs[ac], DtNtopicTitle, tTitle); ac++;
		}

		/* get help type */
		xrm_name [1] = XrmStringToQuark ("helpType");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    XtSetArg (setArgs[ac], DtNhelpType, 
			      (int)atoi((char *)value.addr)); ac++;
		    helpType = (int)atoi((char *)value.addr);
		}
		
		
		switch (helpType)
		{
		  case  DtHELP_TYPE_TOPIC:
		    /* get helpVolume */
		    xrm_name [1] = XrmStringToQuark ("helpVolume");
		    if (XrmQGetResource (db, xrm_name, xrm_name, 
					 &rep_type, &value)) 
		    {
			helpVolume = XtNewString ((char *)value.addr);
/*			XtSetArg (setArgs[ac], DtNhelpVolume, 
				  helpVolume); ac++;*/
		    }
		    /* get locationId */
		    xrm_name [1] = XrmStringToQuark ("locationId");
		    if (XrmQGetResource (db, xrm_name, xrm_name, 
					 &rep_type, &value)) 
		    {
			locationId = XtNewString ((char *)value.addr);
/*			XtSetArg (setArgs[ac], DtNlocationId, 
				  locationId); ac++;*/
		    }

		    break;
		    
		  case DtHELP_TYPE_DYNAMIC_STRING:
		    
		    /* get stringData */
		    xrm_name [1] = XrmStringToQuark ("stringData");
		    if (XrmQGetResource (db, xrm_name, xrm_name, 
					 &rep_type, &value)) 
		    {
			stringData = XtNewString ((char *)value.addr);
/*			XtSetArg (setArgs[ac], DtNstringData, 
				  stringData); ac++;*/
			locationId = stringData;
		    }
		    
		    break;
		    
		  case DtHELP_TYPE_MAN_PAGE:
		    
		    break;
		    
		  case DtHELP_TYPE_FILE:
		    
		    break;
		    
		  default:
		    /* error condition, don't try to display help */
		    break;
		}

		/* get name of wm client for which wmHelp is transient */

		xrm_name [1] = XrmStringToQuark ("windowGroup");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    wmClientName =  ((char *)value.addr);
		    if ((!strcmp(wmClientName, pSD->iconBoxName)) &&
			(pSD->useIconBox))
		    {
			xrm_name [1] = XrmStringToQuark ("wsName");
			if (XrmQGetResource (db, xrm_name, xrm_name, 
					     &rep_type, &value)) 
			{
			    wsName = (char *)value.addr;
			    for (wsCnt = 0; wsCnt < pSD->numWorkspaces; 
				 wsCnt++)
			    {
				if (!strcmp(pSD->pWS[wsCnt].name, wsName))
				{
				    pCD=pSD->pWS[wsCnt].pIconBox->pCD_iconBox;
				    break;
				}
			    }
			}

		    }
/*
		    else if(!strcmp(wmClientName, O_PanelName(pPanelist)))
		    {
			pCD=O_ClientData(pPanelist);
		    }
*/
		}

		/* 
		 *  Map the help dialog
		 */

		WmDtDisplayTopic(pSD->screenTopLevelW1, helpVolume, 
				  locationId, 
				  helpType, NULL, True,
				  setArgs, ac, pCD, False, NULL); 
	    }
	}/* if (XrmQGetResource for wsHelp) */

	xrm_name [0] = XrmStringToQuark ("cachedHelp");
	xrm_name [1] = XrmStringToQuark ("cachedCount");
	xrm_name [2] = NULL;
	
	
	if (XrmQGetResource (db, xrm_name, xrm_name, &rep_type, &value)) 
	{
	    /* restore cachedCount number of help dialogs */
	    cachedCount = (int)atoi((char *)value.addr);
	    for (cCount=1; cCount < (cachedCount + 1); cCount++)
	    {
		sprintf (dialogName,  "oWsHelp%d", cCount);
		xrm_name [0] = XrmStringToQuark (dialogName);
		xrm_name [1] = NULL;
		xrm_name [2] = NULL;

		displayTopicInfo.xPos = 0;
		displayTopicInfo.xPos = 0;
		displayTopicInfo.workspaces = NULL;

		ac = 0;
		/* get x position */
		xrm_name [1] = XrmStringToQuark ("x");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    /*		    XtSetArg (setArgs[ac], XmNx, 
				    atoi((char *)value.addr)); ac++;*/
		    xLoc = (Position)atoi((char *)value.addr);
		    displayTopicInfo.xPos = xLoc;
		}
		
		/* get y position */
		xrm_name [1] = XrmStringToQuark ("y");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    /*		    XtSetArg (setArgs[ac], XmNy, 
				    atoi((char *)value.addr)); ac++;*/
		    yLoc = (Position)atoi((char *)value.addr);
		    displayTopicInfo.yPos = yLoc;
		}
		
		/* get columns */
		xrm_name [1] = XrmStringToQuark ("columns");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    XtSetArg (setArgs[ac], XmNcolumns, 
			      (short)atoi((char *)value.addr)); ac++;
		    columns = (short)atoi((char *)value.addr);
		}
		
		/* get rows */
		xrm_name [1] = XrmStringToQuark ("rows");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    XtSetArg (setArgs[ac], XmNrows, 
			      (short)atoi((char *)value.addr)); ac++;
		    rows = (short)atoi((char *)value.addr);
		}
		sprintf (geometry, "=+%d+%d", xLoc, yLoc);
		
		XtSetArg (setArgs[ac], XmNgeometry, 
			  XtNewString(geometry)); ac++;
		
		/* get visiblePathCount */
		xrm_name [1] = XrmStringToQuark ("vPCount");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    XtSetArg (setArgs[ac], DtNvisiblePathCount, 
			      (int)atoi((char *)value.addr)); ac++;
		}

		/* get topicTitle */
		xrm_name [1] = XrmStringToQuark ("tTitle");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    tTitle = XtNewString ((char *)value.addr);
		    XtSetArg (setArgs[ac], DtNtopicTitle, tTitle); ac++;
		}

		/* get workspaces list */
		xrm_name [1] = XrmStringToQuark ("workspaces");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    workspaces = XtNewString ((char *)value.addr);
		    displayTopicInfo.workspaces = workspaces;
		}


		/* get help type */
		xrm_name [1] = XrmStringToQuark ("helpType");
		if (XrmQGetResource (db, xrm_name, xrm_name, 
				     &rep_type, &value)) 
		{
		    XtSetArg (setArgs[ac], DtNhelpType, 
			      (int)atoi((char *)value.addr)); ac++;
		    helpType = (int)atoi((char *)value.addr);
		}
		

		switch (helpType)
		{
		  case  DtHELP_TYPE_TOPIC:
		    /* get helpVolume */
		    xrm_name [1] = XrmStringToQuark ("helpVolume");
		    if (XrmQGetResource (db, xrm_name, xrm_name, 
					 &rep_type, &value)) 
		    {
			helpVolume = XtNewString ((char *)value.addr);
		    }
		    /* get locationId */
		    xrm_name [1] = XrmStringToQuark ("locationId");
		    if (XrmQGetResource (db, xrm_name, xrm_name, 
					 &rep_type, &value)) 
		    {
			locationId = XtNewString ((char *)value.addr);
		    }
		    
		    break;
		    
		  case DtHELP_TYPE_DYNAMIC_STRING:
		    
		    /* get stringData */
		    xrm_name [1] = XrmStringToQuark ("stringData");
		    if (XrmQGetResource (db, xrm_name, xrm_name, 
					 &rep_type, &value)) 
		    {
			stringData = XtNewString ((char *)value.addr);
			/* XtSetArg (setArgs[ac], DtNstringData, 
						stringData); ac++;*/
			locationId = stringData;
		    }
		    
		    break;
		    
		  case DtHELP_TYPE_MAN_PAGE:
		    
		    break;
		    
		  case DtHELP_TYPE_FILE:
		    
		    break;
		    
		  default:
		    /* error condition, don't try to display help */
		    break;
		    
		} /* switch */		

		WmDtDisplayTopic(pSD->screenTopLevelW1, helpVolume, 
				  locationId, 
				  helpType, NULL, False,
				  setArgs, ac, NULL, False, 
				  &displayTopicInfo); 
		
	    } /* for all cached help dialogs */


	} /* if (XrmQGetResource ) cachedHelp/cachedCount */ 

	XrmDestroyDatabase(db);
    }
#endif /* NO_DT */
} /* END OF FUNCTION  RestoreHelpDialogs */

/*************************************<->*************************************
 *
 *  void
 *  SaveHelpResources (pSD)
 *
 *
 *  Description:
 *  -----------
 *  Saves the state of the help system
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  Return = none
 *
 *
 *  Comments:
 *  --------
 * 
 ******************************<->***********************************/

void 
SaveHelpResources(
        WmScreenData *pSD)
{
#ifndef NO_DT
    PtrWsDtHelpData  pHelp;
    CacheListStruct *pTemp; 
    Arg getArgs[20];
    int ac;

    Window wmGroupWindow;
    ClientData *pCD;
    ClientData *pCDforHelp;
    int wsCnt;
    int thisCnt;
    WorkspaceID wsID;
    WmWorkspaceData *pWS;

    Position xLoc, yLoc;
    short columns, rows;
    unsigned char helpType;
    int vPCount;
    char *helpVolume;
    char *locationId;
    char *stringData;
    char *topicTitle;
    char *wsName;
    char workspaces[MAXWMPATH+1];

    char buffer[MAXWMPATH+1];
    char *res_class;
    char *data;
    int cum_len;

    char screenName[10];
    char dialogName[128];
    char resString[MAXWMPATH+1];
 
    int  cachedCount=0;


    pHelp = &pSD->dtHelp;  

    /* allocate initial data space */
    if ((data = (char *) XtMalloc (MAXWMPATH+1)) == NULL)
    {
        Warning (((char *)
                  GETMESSAGE(76,12,"Insufficient memory to save resources")));
        Do_Quit_Mwm (False);
    }
    cum_len = 1;
    *data = '\0';

    /* Get our current resource class */

    if (MwmBehavior)
    {
        res_class = WM_RESOURCE_CLASS;
    }
    else
    {
        res_class = DT_WM_RESOURCE_CLASS;
    }

    sprintf (screenName, "%d\0", pSD->screen);

    sprintf (buffer, "%s*%s*%s:  \\n ", res_class,  screenName, 
	     WmNhelpResources);
    AddStringToResourceData (buffer, &data, &cum_len);

    if (pHelp->onScreen)
    {
	sprintf (dialogName, "wsHelp");
	sprintf (buffer, "%s*onScreen: True\\n ", dialogName);
	AddStringToResourceData (buffer, &data, &cum_len);

	ac = 0; 
	XtSetArg(getArgs[ac], XmNx, &xLoc);   ac++;
	XtSetArg(getArgs[ac], XmNy, &yLoc);   ac++;
	XtGetValues(pSD->dtHelp.shell, getArgs, ac);
	if (!XFindContext (DISPLAY, XtWindow(pSD->dtHelp.shell),
			   wmGD.windowContextType, (caddr_t *)&pCDforHelp))
	{
	    if (wmGD.positionIsFrame)
	    {
		xLoc = xLoc - pCDforHelp->clientOffset.x;
		yLoc = yLoc - pCDforHelp->clientOffset.y;
	    }
	}

	ac = 0;
	XtSetArg(getArgs[ac], XmNcolumns, &columns); ac++;
	XtSetArg(getArgs[ac], XmNrows, &rows);   ac++;
	XtSetArg(getArgs[ac], DtNhelpType, &helpType);   ac++;
	XtSetArg(getArgs[ac], DtNvisiblePathCount, &vPCount);   ac++;
	XtSetArg(getArgs[ac], DtNtopicTitle, &topicTitle);   ac++;
	XtGetValues(pSD->dtHelp.dialog, getArgs, ac);

	sprintf (buffer, "%s*x: %d \\n ", dialogName, xLoc);
	AddStringToResourceData (buffer, &data, &cum_len);
	sprintf (buffer, "%s*y: %d \\n ", dialogName, yLoc);
	AddStringToResourceData (buffer, &data, &cum_len);
	sprintf (buffer, "%s*columns: %d \\n ", dialogName, columns);
	AddStringToResourceData (buffer, &data, &cum_len);
	sprintf (buffer, "%s*rows: %d \\n ", dialogName, rows);
	AddStringToResourceData (buffer, &data, &cum_len);
	sprintf (buffer, "%s*helpType: %d \\n ", dialogName, helpType);
	AddStringToResourceData (buffer, &data, &cum_len);
	sprintf (buffer, "%s*vPCount: %d \\n ", dialogName, vPCount);
	AddStringToResourceData (buffer, &data, &cum_len);
	sprintf (buffer, "%s*tTitle: %s\\n ", dialogName, topicTitle);
	AddStringToResourceData (buffer, &data, &cum_len);

	ac = 0;
	switch (helpType)
	{
	  case  DtHELP_TYPE_TOPIC:
	    XtSetArg (getArgs[ac],DtNhelpVolume, &helpVolume); ac++; 
	    XtSetArg (getArgs[ac], DtNlocationId, &locationId); ac++;
	    XtGetValues(pSD->dtHelp.dialog, getArgs, ac);
	    sprintf (buffer, "%s*helpVolume: %s\\n ", dialogName, helpVolume);
	    AddStringToResourceData (buffer, &data, &cum_len);
	    sprintf (buffer, "%s*locationId: %s\\n ", dialogName, locationId);
	    AddStringToResourceData (buffer, &data, &cum_len);

	    break;
	    
	  case DtHELP_TYPE_DYNAMIC_STRING:
	    XtSetArg (getArgs[ac], DtNstringData, &stringData); ac++;
	    XtGetValues(pSD->dtHelp.dialog, getArgs, ac);
	    sprintf (buffer, "%s*stringData: %s\\n ", dialogName, stringData);
	    AddStringToResourceData (buffer, &data, &cum_len);

	    break;
	    
	  case DtHELP_TYPE_MAN_PAGE:
	    
	    break;
	    
	  case DtHELP_TYPE_FILE:
	    
	    break;
	    
	  default:
	    /* error condition, don't try to display help */
	    break;
	}

	ac = 0;
	XtSetArg(getArgs[ac], XmNwindowGroup, &wmGroupWindow);   ac++;
	XtGetValues(pSD->dtHelp.shell, getArgs, ac);

	if (!XFindContext (DISPLAY, wmGroupWindow,
			   wmGD.windowContextType, (caddr_t *)&pCD))
	{
	    sprintf (buffer, "%s*windowGroup: %s\\n ", dialogName, 
		     pCD->clientName);
	    AddStringToResourceData (buffer, &data, &cum_len);

	    if ((!strcmp(pCD->clientName, pSD->iconBoxName)) &&
		(pSD->useIconBox))
	    {
		wsName = XGetAtomName(DISPLAY,
				      pCD->pWsList[pCD->currentWsc].wsID);
		sprintf (buffer, "%s*wsName: %s\\n ", dialogName, 
			 wsName);
		AddStringToResourceData (buffer, &data, &cum_len);
		XFree(wsName);
	    }
	}

    }
    
    if (pSD->cachedHelp != NULL)
    {
	/* 
	 * We have some nodes so search for 
	 * managed dialogs and save their state
	 */
	
	pTemp = pSD->cachedHelp;
	
	while (pTemp != NULL)
	{
	    if (pTemp->inUseFlag == FALSE)
	    {
		pTemp = pTemp->pNext;
	    }
	    else
	    {
		cachedCount++;		
		sprintf (dialogName,  "oWsHelp%d",cachedCount);

		ac = 0; 
		XtSetArg(getArgs[ac], XmNx, &xLoc);   ac++;
		XtSetArg(getArgs[ac], XmNy, &yLoc);   ac++;
		XtGetValues(XtParent(pTemp->helpDialog), getArgs, ac);
		if (!XFindContext (DISPLAY, 
				   XtWindow(XtParent(pTemp->helpDialog)),
				   wmGD.windowContextType, 
				   (caddr_t *)&pCDforHelp))
		{
		    if (wmGD.positionIsFrame)
		    {
			xLoc = xLoc - pCDforHelp->clientOffset.x;
			yLoc = yLoc - pCDforHelp->clientOffset.y;
		    }
		}
		else
		{
		    pCDforHelp = NULL;
		}


		ac = 0; 
		XtSetArg(getArgs[ac], XmNcolumns, &columns); ac++;
		XtSetArg(getArgs[ac], XmNrows, &rows);   ac++;
		XtSetArg(getArgs[ac], DtNhelpType, &helpType);   ac++;
		XtSetArg(getArgs[ac], DtNvisiblePathCount, &vPCount);   ac++;
		XtSetArg(getArgs[ac], DtNtopicTitle, &topicTitle);   ac++;
		XtGetValues(pTemp->helpDialog, getArgs, ac);

		sprintf (buffer, "%s*x: %d \\n ", dialogName, xLoc);
		AddStringToResourceData (buffer, &data, &cum_len);
		sprintf (buffer, "%s*y: %d \\n ", dialogName, yLoc);
		AddStringToResourceData (buffer, &data, &cum_len);
		sprintf (buffer, "%s*columns: %d \\n ", dialogName, columns);
		AddStringToResourceData (buffer, &data, &cum_len);
		sprintf (buffer, "%s*rows: %d \\n ", dialogName, rows);
		AddStringToResourceData (buffer, &data, &cum_len);
		sprintf (buffer, "%s*helpType: %d \\n ", dialogName, helpType);
		AddStringToResourceData (buffer, &data, &cum_len);
		sprintf (buffer, "%s*vPCount: %d \\n ", dialogName, vPCount);
		AddStringToResourceData (buffer, &data, &cum_len);
		sprintf (buffer, "%s*tTitle: %s\\n ", dialogName, topicTitle);
		AddStringToResourceData (buffer, &data, &cum_len);

		ac = 0;
		switch (helpType)
		{
		  case  DtHELP_TYPE_TOPIC:
		    XtSetArg (getArgs[ac],DtNhelpVolume, &helpVolume); ac++; 
		    XtSetArg (getArgs[ac], DtNlocationId, &locationId); ac++;
		    XtGetValues(pTemp->helpDialog, getArgs, ac);
		    sprintf (buffer, "%s*helpVolume: %s\\n ", 
			     dialogName, helpVolume);
		    AddStringToResourceData (buffer, &data, &cum_len);
		    sprintf (buffer, "%s*locationId: %s\\n ", 
			     dialogName, locationId);
		    AddStringToResourceData (buffer, &data, &cum_len);

		    break;
		    
		  case DtHELP_TYPE_DYNAMIC_STRING:
		    XtSetArg (getArgs[ac], DtNstringData, &stringData); ac++;
		    XtGetValues(pTemp->helpDialog, getArgs, ac);
		    sprintf (buffer, "%s*stringData: %s\\n ", 
			     dialogName, stringData);
		    AddStringToResourceData (buffer, &data, &cum_len);

		    break;
		    
		  case DtHELP_TYPE_MAN_PAGE:
		    
		    break;
		    
		  case DtHELP_TYPE_FILE:
	
		    break;
		    
		  default:
		    /* error condition, don't try to display help */
		    break;
		}

		if (pCDforHelp)
		{
		    thisCnt = 0;
		    sprintf(workspaces,"");
		    for (wsCnt = 0; wsCnt < pSD->numWorkspaces;
			 wsCnt++)
		    {
			wsID = XInternAtom (DISPLAY, pSD->pWS[wsCnt].name, 
					    False);
			pWS = GetWorkspaceData (pSD, wsID);

			if (ClientInWorkspace (pWS, pCDforHelp))
			{
			    if (thisCnt == 0)
			    {
				strcpy(workspaces, pSD->pWS[wsCnt].name); 

			    }
			    else
			    {
				strcat(workspaces, "*");
				strcat(workspaces, pSD->pWS[wsCnt].name);
			    }
			    thisCnt++;
			}
		    }
		    sprintf (buffer, "%s*workspaces: %s\\n ", dialogName, 
			     workspaces);
		    AddStringToResourceData (buffer, &data, &cum_len);
		}
		pTemp = pTemp->pNext;
	    }
	} /* While */
	if (cachedCount)
	{
	    sprintf (buffer, "cachedHelp*cachedCount: %d\\n ",  cachedCount);
	    AddStringToResourceData (buffer, &data, &cum_len);
	}
    }

    if (data)
    {
	/*
	 * Save help resources as a string 
	 */
	sprintf (buffer, " \n");
	AddStringToResourceData (buffer, &data, &cum_len);
	
	_DtAddToResource (DISPLAY, data);
	XtFree(data);
    }
#endif /*  NO_DT */
} /* END OF FUNCTION  SaveHelpResources */


 
/* Code stolen from WmPresence.c */

/*************************************<->*************************************
 *
 *  void
 *  ShowHelpDialog (pClient)
 *
 *
 *  Description:
 *  -----------
 *  Pops up (shows) the help dialog
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *  Return = none
 *
 *
 *  Comments:
 *  --------
 * 
 ******************************<->***********************************/

static void 
ShowHelpDialog(
        ClientData *pClient)
{
    PtrWsDtHelpData  pHelp;
    WmScreenData *pSD;
    ClientData *pOldCD;

    if (!(pClient))
    {
	pSD=wmGD.pActiveSD;
    }
    else
    {
	pSD = pClient->pSD;
    }
    pHelp = &pSD->dtHelp;  

    pOldCD = pHelp->pCDforClient;
    pHelp->pCDforClient = pClient;
/*    pHelp->contextForClient = wsContext;*/

    if (pSD->dtHelp.onScreen)
    {
	ClientData *pCD = NULL;

	XFindContext (DISPLAY, XtWindow (pSD->dtHelp.shell),
			wmGD.windowContextType, (XtPointer) &pCD);

	if ((!pSD->dtHelp.bMapped) ||
	    (pCD &&
	     (!ClientInWorkspace (ACTIVE_WS, pCD) ||
	     (pCD->clientState == MINIMIZED_STATE) ||
	     (pOldCD && pOldCD != pHelp->pCDforClient))))
	{
	    HideHelpDialog (pSD, True);
	}
    }
    
    /* 
     * pop it up 
     */

    XtManageChild(pSD->dtHelp.dialog);
    XtPopup (pSD->dtHelp.shell, XtGrabNone);

    pSD->dtHelp.onScreen = True;

} /* END OF FUNCTION  ShowHelpDialog */


/*************************************<->*************************************
 *
 *  void
 *  HideHelpDialog (pSD, userDismissed)
 *
 *
 *  Description:
 *  -----------
 *  Pops down (hides) the workspace presence dialog
 *
 *  Inputs:
 *  ------
 *  pSD = pointer to screen data
 *  userDismissed = did the user dismiss or did workspace switching 
 *                  unpost the workspace presence box ?
 * 
 *  Outputs:
 *  -------
 *  Return = none
 *
 *
 *  Comments:
 *  --------
 * 
 ******************************<->***********************************/

static void 
HideHelpDialog(
        WmScreenData *pSD,
        Boolean userDismissed )

{
    if (pSD->dtHelp.onScreen)
    {
	/* Pop down the shell */
	XtPopdown (pSD->dtHelp.shell);

	/* 
	 * Do a withdraw to make sure window gets unmanaged
	 * (popdown does nothing if its unmapped)
	 */
	XWithdrawWindow (DISPLAY, XtWindow (pSD->dtHelp.shell),
			 pSD->screen);
	/* must sync to insure event order */
	XSync (DISPLAY, False);
	
	
	pSD->dtHelp.onScreen = False;
	pSD->dtHelp.userDismissed = userDismissed;
    }

} /* END OF FUNCTION HideHelpDialog  */


/*************************************<->*************************************
 *
 *  wmDtHelpSetPosition (pHelp, pSD, restorePos)
 *
 *
 *  Description:
 *  -----------
 *  Sets the position of the workspace presence dialog
 *
 *  Inputs:
 *  ------
 *  pPres = pointer to workspace presence data
 *
 *  Outputs:
 *  --------
 * 
 ******************************<->***********************************/
static void 
wmDtHelpSetPosition(
		     PtrWsDtHelpData pHelp,
		     WmScreenData *pSD,
		     Boolean restorePos)
{
    Arg args[10];
    int n;
    XFontStruct *font;
    Dimension height;
    Window wGroup;
    int x, y;
    ClientData *pCDforHelp;

    if (!restorePos)
    {
	/*
	 * Get size of this dialog
	 */
	n = 0;
	XtSetArg (args[n], XmNheight, &height); n++;
	XtGetValues (pHelp->shell, args, n);
	
	if (wmGD.positionIsFrame)
	{
	    if (pSD->decoupleTitleAppearance)
	    {
		font = pSD->clientTitleAppearance.font;
	    }
	    else
	    {
		font = pSD->clientAppearance.font;
	    }
	    
/*	    height += TEXT_HEIGHT(font) + (2 * pSD->frameBorderWidth);*/
	    height += TEXT_HEIGHT(font) + (2 * pSD->resizeBorderWidth);
	}

	
	/* 
	 * set position of dialog relative to client window 
	 * (use system menu position)
	 * set this dialog to be transient for the client
	 * for which it is posted.
     */
	if (pHelp->pCDforClient)
	{
	    GetSystemMenuPosition (pHelp->pCDforClient, 
				   &x, &y, height,
				   (Context)0);
#ifdef NO
	    if(y < pHelp->pCDforClient->frameInfo.y)
	    {
		if (!XFindContext (DISPLAY, XtWindow(pHelp->shell),
				   wmGD.windowContextType, 
				   (caddr_t *)&pCDforHelp))
		{
		    if (wmGD.positionIsFrame)
		    {
/*			x = x - pCDforHelp->clientOffset.x;*/
			y = y - pCDforHelp->clientOffset.x;
		    }
		}
		else
		{
		    y = y - (2 * pSD->frameBorderWidth);
		}
	    }
#endif
	}
	else
	{
	    /*
	     * Don't have a client so map at 0,0
	 */
	    x = 0;
	    y = 0;
	}
    }
    else
    {
	x = pHelp->xPos;
	y = pHelp->yPos;
    }

    n = 0;
    XtSetArg (args[n], XmNgeometry, NULL); n++;
    XtSetArg (args[n], XmNx, x);				n++;
    XtSetArg (args[n], XmNy, y);				n++;
    XtSetArg (args[n], XmNtransientFor, NULL);			n++;

    if (pHelp->pCDforClient)
    {
	int ac;
	Arg al[5];

	ac = 0;
	XtSetArg (al[ac], XmNwindowGroup, &wGroup); ac++;
	XtGetValues (pHelp->shell, al, ac);

#ifdef FUTURE
	if (pHelp->contextForClient != F_CONTEXT_ICON)
	{
	    XtSetArg (args[n], XmNwindowGroup, 
		      pHelp->pCDforClient->client); 
	    n++;
	}
	else if (pSD->useIconBox && P_ICON_BOX(pHelp->pCDforClient))
	{
	    XtSetArg (args[n], XmNwindowGroup, 
		      P_ICON_BOX(pHelp->pCDforClient)->pCD_iconBox->client); 
	    n++;
	}
	else
	{
	    XtSetArg (args[n], XmNwindowGroup, 0); n++;
	}
#else /* FUTURE */
        if (wGroup != pHelp->pCDforClient->client)
	{
	    /*
	     * Changing who this is transient for, need to 
	     * make sure the window is unmapped so that the
	     * window manager moves it to the right place.
	     */
	    HideHelpDialog (pSD, True);
	}

	XtSetArg (args[n], XmNwindowGroup, pHelp->pCDforClient->client); n++;
#endif /* FUTURE */
    }
    else
    {
        if (wGroup != 0)
	{
	    HideHelpDialog (pSD, True);
	}
	XtSetArg (args[n], XmNwindowGroup, 0); n++;
    }

    XtSetArg (args[n], XmNwaitForWm, False);			n++;

    XtSetValues (pHelp->shell, args, n);

    
} /* END OF FUNCTION wmDtHelpSetPosition */

/* End of Code stolen from WmPresence.c */

/*************************************<->*************************************
 *
 *  wmDtHelpMapEventHandler (wHelp, pSD, pEvent)
 *
 *
 *  Description:
 *  -----------
 *  An XtEventHandler called on structure notify events on the screen's 
 *  help window.  (Only handles map and unmap events)
 *
 *  Inputs:
 *  ------
 *  wHelp = help widget
 *  pSD = ptr to screen data
 *  pEvent = event triggering this call
 *
 *  Outputs:
 *  --------
 * 
 ******************************<->***********************************/
static void
wmDtHelpMapEventHandler(
	Widget wHelp, 
	XtPointer ptr,
        XEvent  *pEvent,
	Boolean *pBoolean)	/* unused */
{
    WmScreenData *pSD = (WmScreenData *) ptr;

    switch (pEvent->type)
    {
	case MapNotify:
	    pSD->dtHelp.bMapped = True;
	    break;

	case UnmapNotify:
	    pSD->dtHelp.bMapped = False;
	    break;
    }
}
/* END OF FUNCTION wmDtHelpMapHandler */



/*************************************<->*************************************
 *
 *  wmDtErrorDialog (pSD, message, title)
 *
 *
 *  Description:
 *  -----------
 *  Called to show an error message
 *
 *  Inputs:
 *  ------
 *  pSD = ptr to screen data
 *  message = error message to display
 *  title = title for the error dialog
 *
 *  Outputs:
 *  --------
 * 
 ******************************<->***********************************/
static void
wmDtErrorDialog(
	WmScreenData *pSD,
	String sMessage,
	String sTitle
		)
{
    int ac;
    Arg al[20];
    XmString xmsMessage;
    XmString xmsTitle;
    Widget wTemp;
    Widget wParent;
    Widget wShell;
    Window wRoot, wChild; 
    int rootX, rootY, winX, winY;
    unsigned int mask;
    Dimension dWidth, dHeight;
    Position x, y;
#ifdef PANELIST
    WmPanelistObject  pPanelist;
#endif /* PANELIST */

    xmsMessage = 
	    XmStringCreateLocalized (sMessage);
    xmsTitle = 
	    XmStringCreateLocalized (sTitle);

    if (pSD->dtHelp.errorDialog == NULL)
    {
	XmString tmpXmString = (XmString)NULL;

	ac = 0;
	XtSetArg (al[ac], XmNmessageString, xmsMessage);	ac++;
	XtSetArg (al[ac], XmNdialogTitle, xmsTitle);		ac++;
#ifndef NO_MESSAGE_CATALOG
	XtSetArg (al[ac], XmNokLabelString, wmGD.okLabel);  	ac++;
#else
	tmpXmString = XmStringCreateLocalized ("OK");
	XtSetArg (al[ac], XmNokLabelString, tmpXmString); ac++;
#endif

#if 0
	if ((wmGD.dtSD == pSD) && pSD->wPanelist)
	{
	    pPanelist = (WmPanelistObject) pSD->wPanelist;
	    wParent = O_Shell (pPanelist);
	}
	else
	{
	    wParent = pSD->screenTopLevelW1;
	}
#else
        wParent = pSD->screenTopLevelW1;
#endif
        pSD->dtHelp.errorDialog =
	    XmCreateErrorDialog (wParent, "Error Dialog", al, ac);

	if (tmpXmString)
	    XmStringFree(tmpXmString);

	wTemp = XmMessageBoxGetChild (pSD->dtHelp.errorDialog, 
			XmDIALOG_HELP_BUTTON);
	XtUnmanageChild (wTemp);

	wTemp = XmMessageBoxGetChild (pSD->dtHelp.errorDialog, 
			XmDIALOG_CANCEL_BUTTON);
	XtUnmanageChild (wTemp);

	XtAddCallback (pSD->dtHelp.errorDialog, XmNokCallback, 
			wmDtErrorDialogOkCB, (XtPointer)pSD); 

	wShell = pSD->dtHelp.errorDialog;
	while (!(XtIsSubclass(wShell, shellWidgetClass)))
	{
	    wShell = XtParent(wShell);
	}
	XtAddCallback(wShell, XmNpopupCallback, wmDtErrorDialogPopupCB, 
			(XtPointer)pSD);

	XtRealizeWidget (pSD->dtHelp.errorDialog);

	/* remove some WM functions */
	ac = 0;
	XtSetArg (al[ac], XmNmwmFunctions, (MWM_FUNC_ALL|
	    MWM_FUNC_RESIZE|MWM_FUNC_MINIMIZE|MWM_FUNC_MAXIMIZE));
	    ac++;
	XtSetValues (wShell, al, ac);

	DtWsmRemoveWorkspaceFunctions (DISPLAY, XtWindow(wShell));
    }
    else
    {
	/* make sure it's unmanaged */
	wmDtErrorDialogHide (pSD);

	/* set new message and title */
	ac = 0;
	XtSetArg (al[ac], XmNmessageString, xmsMessage);	ac++;
	XtSetArg (al[ac], XmNdialogTitle, xmsTitle);		ac++;
	XtSetValues (pSD->dtHelp.errorDialog, al, ac);
    }

    if (pSD->dtHelp.errorDialog)
    {
	XtManageChild (pSD->dtHelp.errorDialog);
	XtPopup (XtParent(pSD->dtHelp.errorDialog), XtGrabNone);
    }

    XmStringFree (xmsMessage);
    XmStringFree (xmsTitle);
}
/* END OF FUNCTION wmDtErrorDialog */


/*************************************<->*************************************
 *
 *  void
 *  wmDtErrorDialogHide (pSD)
 *
 *
 *  Description:
 *  -----------
 *
 *
 *  Inputs:
 *  ------
 *  pSD = ptr to screen data
 *
 * 
 *  Outputs:
 *  -------
 *  Return = ...
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

static void 
wmDtErrorDialogHide( WmScreenData *pSD )
{
    Widget wShell;

    XtPopdown (XtParent(pSD->dtHelp.errorDialog));
    XtUnmanageChild (pSD->dtHelp.errorDialog);
    /* 
     * Do a withdraw to make sure window gets unmanaged
     * (popdown does nothing if its unmapped)
     */
    wShell = pSD->dtHelp.errorDialog;
    while (!(XtIsSubclass(wShell, shellWidgetClass)))
    {
	wShell = XtParent(wShell);
    }
    if (wShell)
	XWithdrawWindow (DISPLAY, XtWindow (wShell), pSD->screen);

    /* must sync to insure event order */
    XSync (DISPLAY, False);

} /* END OF FUNCTION wmDtErrorDialogHide */


/*************************************<->*************************************
 *
 *  void
 *  wmDtErrorDialogOkCB (w, client_data, call_data)
 *
 *
 *  Description:
 *  -----------
 *
 *
 *  Inputs:
 *  ------
 *  w = ...
 *  client_data = ...
 *  call_data = ...
 *
 * 
 *  Outputs:
 *  -------
 *  Return = ...
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

static void 
wmDtErrorDialogOkCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    WmScreenData *pSD = (WmScreenData *) client_data;

    wmDtErrorDialogHide (pSD);

} /* END OF FUNCTION wmDtErrorDialogOkCB */


/*************************************<->*************************************
 *
 *  void
 *  wmDtErrorDialogPopupCB (w, client_data, call_data)
 *
 *
 *  Description:
 *  -----------
 *
 *
 *  Inputs:
 *  ------
 *  w = ...
 *  client_data = ...
 *  call_data = ...
 *
 * 
 *  Outputs:
 *  -------
 *  Return = ...
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

static void 
wmDtErrorDialogPopupCB(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
    WmScreenData *pSD = (WmScreenData *) client_data;
    Window wRoot, wChild; 
    int rootX, rootY, winX, winY;
    unsigned int mask;
    Dimension dWidth, dHeight;
    Position x, y;
    Arg al[10];
    int ac;

    /* set new position near cursor position */
    XQueryPointer (DISPLAY, XtWindow(pSD->screenTopLevelW1),
	&wRoot, &wChild, &rootX, &rootY, &winX, &winY, &mask);

    ac = 0;
    XtSetArg (al[ac], XmNwidth, &dWidth);			ac++;
    XtSetArg (al[ac], XmNheight, &dHeight);			ac++;
    XtGetValues (w, al, ac);

    x = (Position) rootX - (dWidth / 2);
    y = (Position) rootY - (dHeight / 2);

    if ((x + ((Position) dWidth / 2)) > DisplayWidth (DISPLAY, pSD->screen))
	x = DisplayWidth (DISPLAY,pSD->screen) - (dWidth/2) - 1;
    if (x < 0) 
	x = 0;

    if ((y + ((Position) dHeight / 2)) > DisplayHeight (DISPLAY, pSD->screen))
	y = DisplayHeight (DISPLAY,pSD->screen) - (dHeight/2) - 1;
    if (y < 0) 
	y = 0;

    ac = 0;
    XtSetArg (al[ac], XmNx, x);			ac++; 
    XtSetArg (al[ac], XmNy, y);			ac++;
    XtSetArg (al[ac], XmNwindowGroup, 
			XtUnspecifiedWindowGroup);	ac++; 
    XtSetArg (al[ac], XmNwaitForWm, False);		ac++;
    XtSetValues (w, al, ac);


} /* END OF FUNCTION wmDtErrorDialogPopupCB */

/****************************   eof    ***************************/
#endif /* WSM */




