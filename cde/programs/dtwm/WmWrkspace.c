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
#ifdef WSM
/* 
 * (c) Copyright 1987,1988,1989,1990,1992,1993,1994 HEWLETT-PACKARD COMPANY 
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 * ALL RIGHTS RESERVED 
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: WmWrkspace.c /main/7 1996/10/23 17:26:33 rswiston $"
#endif
#endif

/*
 * Included Files:
 */

#include "WmGlobal.h"
#include "WmHelp.h"
#include "WmResNames.h"
#include "WmIPlace.h"
#include <X11/Xutil.h>
#include "WmICCC.h"
#include <Xm/Xm.h>
#include <Dt/DtP.h>
#include <Dt/WsmM.h>
#include <stdio.h>
#ifdef PANELIST
#include "WmPanelP.h"
#endif /* PANELIST */
#include "WmIPC.h"		/* must be after DtP.h */
#ifdef WSM
#include "WmPresence.h"
#endif /* WSM */

/* local macros */
#ifndef MIN
#define MIN(a,b) ((a)<=(b)?(a):(b))
#endif 

#ifndef MAX
#define MAX(a,b) ((a)>=(b)?(a):(b))
#endif 


/* internally defined functions */

#include "WmWrkspace.h"

/********    Static Function Declarations    ********/

static void InsureUniqueWorkspaceHints( 
                        ClientData *pCD) ;

/********    End Static Function Declarations    ********/

/* FindDtSessionMatch () put in WmResParse.h */

/* external functions */
#include "WmBackdrop.h"
#include "WmError.h"
#include "WmFunction.h"
#include "WmIDecor.h"
#include "WmIconBox.h"
#include "WmMenu.h"
#include "WmProperty.h"
#include "WmResParse.h"
#include "WmWinInfo.h"
#include "WmWinList.h"
#include "WmWinState.h"
#include "WmXSMP.h"

/*
 * Global Variables:
 */

/* a dynamically allocated list of workspaces used
 * by F_AddToAllWorkspaces
 */
static int numResIDs = 0;
static WorkspaceID *pResIDs = NULL;



/*************************************<->*************************************
 *
 *  ChangeToWorkspace (pNewWS)
 *
 *
 *  Description:
 *  -----------
 *  This function changes to a new workspace.
 *
 *  Inputs:
 *  ------
 *  pNewWS =  pointer to workspace data
 *
 * 
 *************************************<->***********************************/

void 
ChangeToWorkspace(
        WmWorkspaceData *pNewWS )

{
    ClientData *pCD;
    int i;
    WmScreenData *pSD = pNewWS->pSD;

    ClientData *pWsPCD;
    Context   wsContext = F_CONTEXT_NONE;

    if (pNewWS == pSD->pActiveWS)
	return;				/* already there */

    pSD->pLastWS = pSD->pActiveWS;

    /*
     * Go through client list of old workspace and hide windows
     * that shouldn't appear in new workspace.
     */

    if (pSD->presence.shellW && 
	pSD->presence.onScreen &&
	pSD->presence.contextForClient == F_CONTEXT_ICON)
    {
	pWsPCD = pSD->presence.pCDforClient;
	wsContext = pSD->presence.contextForClient;
	HidePresenceBox (pSD, False);
    }

    for (i = 0; i < pSD->pActiveWS->numClients; i++)
    {
	pCD = pSD->pActiveWS->ppClients[i];
	if (!ClientInWorkspace (pNewWS, pCD))
	{
	   SetClientWsIndex(pCD);
	   SetClientState (pCD, pCD->clientState | UNSEEN_STATE,
		 CurrentTime);
	}
    }

    /*
     * Hide active icon text label
     */
     if ((pSD->iconDecoration & ICON_ACTIVE_LABEL_PART) &&
	 wmGD.activeIconTextDisplayed)
     {
	 HideActiveIconText(pSD);
     }
    
    /*
     * Unmap old icon box
     */
    if (pSD->useIconBox)
    {
	UnmapIconBoxes (pSD->pLastWS);
    }
    
    /* 
     * Set new active workspace 
     */
    pSD->pActiveWS = pNewWS;
    ChangeBackdrop (pNewWS);

    /*
     * Go through client list of new workspace and show windows
     * that should appear.
     */
    for (i = 0; i < pNewWS->numClients; i++)
    {
	pCD = pNewWS->ppClients[i];
	SetClientWsIndex(pCD);
        if (pCD->clientState & UNSEEN_STATE)
	{
	    SetClientState (pCD, 
		(pCD->clientState & ~UNSEEN_STATE), CurrentTime);
	}
	if ((pCD->clientState == MINIMIZED_STATE) &&
			 ((!pCD->pSD->useIconBox) || 
			  (!P_ICON_BOX(pCD))))
	{
	    XMoveWindow (DISPLAY, ICON_FRAME_WIN(pCD), 
			ICON_X(pCD), ICON_Y(pCD));
	}

	if (pCD->iconWindow)
	{
	    unsigned int xOffset, yOffset;

	    /*
	     * Adjust for icons in the box
	     */

	    if (pNewWS->pIconBox)
	    {
		xOffset = IB_MARGIN_WIDTH;
		yOffset = IB_MARGIN_HEIGHT;
	    }
	    else
	    {
		xOffset = 0;
		yOffset = 0;
	    }

	    /*
	     * reparent icon window to frame in this workspace
	     */
	    if ((ICON_DECORATION(pCD) & ICON_IMAGE_PART) && 
		(pCD->iconWindow))
	    {
		ReparentIconWindow (pCD, xOffset, yOffset);
	    }
	}
    }

    if ( (wsContext == F_CONTEXT_ICON &&
	  ClientInWorkspace (ACTIVE_WS, pWsPCD)) ||
	
	 (pSD->presence.shellW && 
	  ! pSD->presence.userDismissed &&
	  ClientInWorkspace (ACTIVE_WS, pSD->presence.pCDforClient) &&
	  pSD->presence.contextForClient == F_CONTEXT_ICON))
    {
	ShowPresenceBox(pSD->presence.pCDforClient, F_CONTEXT_ICON);
    }

#ifdef HP_VUE
    /* sync up workspace info property with current state */
    UpdateWorkspaceInfoProperty (pSD);
#endif /* HP_VUE */
    SetCurrentWorkspaceProperty (pSD);

    /* send workspace change broadcast message */
    dtSendWorkspaceModifyNotification(pSD, (Atom) pNewWS->id,
				DtWSM_REASON_CURRENT);

} /* END OF FUNCTION ChangeToWorkspace */

#ifdef PANELIST

/******************************<->*************************************
 *
 *  ChangeWorkspaceTitle (pWS, pchTitle)
 *
 *  Description:
 *  -----------
 *  Set the title for a workspace.
 *
 *  Inputs:
 *  ------
 *  pWS = pointer to workspace data
 *  pchTitle = new title to assign to this workspace
 *
 *  Outputs:
 *  -------
 *  none
 * 
 *  Comments:
 *  --------
 *  
 ******************************<->***********************************/

void 
ChangeWorkspaceTitle(
        WmWorkspaceData *pWS,
	char * pchTitle)
{
    XmString xmstr;

    /*
     * Convert string to XmString
     */
    xmstr = XmStringCreateLocalized (pchTitle);

    /*
     * Validate title ?
     */

    /*
     * Replace title in workspace data
     */
    XmStringFree (pWS->title);
    pWS->title = xmstr;

    /*
     * Save changes to resource database
     */
    SaveWorkspaceResources (pWS, (WM_RES_WORKSPACE_TITLE));

    /*
     * Replace old workspace in info property
     */
    SetWorkspaceInfoProperty (pWS);
#ifdef HP_VUE
    UpdateWorkspaceInfoProperty (pWS->pSD);
#endif /* HP_VUE */
    XFlush (DISPLAY);

    /*
     * Inform the world of the new workspace title
     */
    dtSendWorkspaceModifyNotification(pWS->pSD, pWS->id, DtWSM_REASON_TITLE);

} /* END OF FUNCTION ChangeWorkspaceTitle */
#endif /* PANELIST */


/*************************************<->*************************************
 *
 *  UpdateWorkspacePresenceProperty (pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function updates the _DT_WORKSPACE_PRESENCE property for a
 *  client window
 *
 *  Inputs:
 *  ------
 *  pCD  =  pointer to client data
 *
 * 
 *************************************<->***********************************/

void 
UpdateWorkspacePresenceProperty(
        ClientData *pCD )

{
    static Atom 	*pPresence = NULL;
    static unsigned long   cPresence = 0;
    unsigned long i;

    if (wmGD.useStandardBehavior)
    {
	/*
	 * Don't change any workspace properties in standard behavior
	 * mode.
	 */
	return;
    }

    if (!pPresence)
    {
	/* allocate initial list */
	if (!(pPresence = (Atom *) 
		    XtMalloc (pCD->pSD->numWorkspaces * sizeof(Atom))))
	{
	    Warning (((char *)GETMESSAGE(76, 1, "Insufficient memory for workspace presence property")));
	}
	else
	{
	    cPresence = pCD->pSD->numWorkspaces;
	}
    }

    if (cPresence < pCD->numInhabited)
    {
	/* allocate bigger list */
	if (!(pPresence = (Atom *) 
		XtRealloc ((char *)pPresence, pCD->numInhabited * sizeof(Atom))))
	{
	    Warning (((char *)GETMESSAGE(76, 2, "Insufficient memory for workspace presence property")));
	}
	else
	{
	    cPresence = pCD->numInhabited;
	}
    }

    for (i = 0; (i < pCD->numInhabited) && (i < cPresence) ; i++)
    {
	pPresence[i] = pCD->pWsList[i].wsID;
    }

    SetWorkspacePresence (pCD->client, pPresence,
				MIN(pCD->numInhabited, cPresence));

} /* END OF FUNCTION UpdateWorkspacePresenceProperty */

#ifdef HP_VUE

/*************************************<->*************************************
 *
 *  UpdateWorkspaceInfoProperty (pSD)
 *
 *
 *  Description:
 *  -----------
 *  This function updates the _DT_WORKSPACE_INFO property for the
 *  screen
 *
 *  Inputs:
 *  ------
 *  pSD  =  pointer to screen data
 *
 * 
 *************************************<->***********************************/

void 
UpdateWorkspaceInfoProperty(
        WmScreenData *pSD )

{
    WorkspaceInfo *pWsInfo;
    WmWorkspaceData *pws;
    int count;

    if (wmGD.useStandardBehavior)
    {
	/*
	 * Don't change any workspace properties in standard behavior
	 * mode.
	 */
	return;
    }

    if (pWsInfo = (WorkspaceInfo *) 
		  XtMalloc (pSD->numWorkspaces * sizeof(WorkspaceInfo)))
    {
	/* put current workspace at top of list */
	pWsInfo[0].workspace = pSD->pActiveWS->id;
	pWsInfo[0].backgroundWindow = pSD->pActiveWS->backdrop.window;
	pWsInfo[0].bg = pSD->pActiveWS->backdrop.background;
	pWsInfo[0].fg = pSD->pActiveWS->backdrop.foreground;
	pWsInfo[0].backdropName = pSD->pActiveWS->backdrop.nameAtom;

	/* add in the rest of the workspaces */
	pws = pSD->pWS;
	for (count = 1; count < pSD->numWorkspaces; count++)
	{
	    if (pWsInfo[0].workspace == pws->id)
		pws++;	/* already at top, skip this one */
	    
	    pWsInfo[count].workspace = pws->id;
	    pWsInfo[count].backgroundWindow = pws->backdrop.window;
	    pWsInfo[count].bg = pws->backdrop.background;
	    pWsInfo[count].fg = pws->backdrop.foreground;
	    pWsInfo[count].backdropName = pws->backdrop.nameAtom;
	    pws++;
	}

	/* set the property */
	SetWorkspaceInfo (pSD->wmWorkspaceWin, pWsInfo,
					    pSD->numWorkspaces);

	XtFree ((char *)pWsInfo);
    }
    else
    {
	Warning (((char *)GETMESSAGE(76, 3, "Insufficient memory to update workspace info")));
    }

} /* END OF FUNCTION UpdateWorkspaceInfoProperty */
#endif /* HP_VUE */


/*************************************<->*************************************
 *
 *  AddPersistentWindow (pWS)
 *
 *
 *  Description:
 *  -----------
 *  This function adds windows that want to be in all workspaces to
 *  the workspace passed in.
 *
 *  Inputs:
 *  ------
 *  pWS  =  pointer to workspace data
 *
 *  Outputs:
 *  --------
 * 
 *************************************<->***********************************/

void
AddPersistentWindows(
	WmWorkspaceData *pWS)

{
    WmScreenData *pSD = pWS->pSD;
    int i;
    ClientListEntry *pCLE;

    /*
     * For all the windows managed for this screen, see if they
     * want to be in all workspaces and add them to this workspace.
     */
    pCLE = pSD->clientList;

    while (1)
    {
	/*
	 * Process all the non-icon client list entries 
	 */
	if ((pCLE->type == NORMAL_STATE) &&
	    (pCLE->pCD->putInAll))
	{
	    AddClientToWorkspaces( pCLE->pCD, &(pWS->id), 1 );
	}
	
	/*
	 * Test for exit condition and advance client list pointer
	 */
	if (pCLE == pSD->lastClient) 
	    break;
	else
	    pCLE = pCLE->nextSibling;
    }

} /* END OF FUNCTION AddPersistentWindows */


/*************************************<->*************************************
 *
 *  CreateWorkspace (pSD, pchTitle)
 *
 *
 *  Description:
 *  -----------
 *  This function creates a new workspace.
 *
 *  Inputs:
 *  ------
 *  pSD  =  pointer to screen data
 *  pchTitle = user-visible title for the workspace (may be NULL)
 *
 *  Outputs:
 *  --------
 *  Returns pointer to workspace data if successful.
 * 
 *************************************<->***********************************/

WmWorkspaceData * 
CreateWorkspace(
        WmScreenData *pSD,
        unsigned char *pchTitle )

{
    WmWorkspaceData *pWS = NULL;
    String string;
    int iActiveWS;

    /*
     * Allocate more workspace datas if we have no spares
     */
    if (pSD->numWsDataAllocated <= pSD->numWorkspaces)
    {
	iActiveWS = (pSD->pActiveWS - pSD->pWS) / sizeof (WmWorkspaceData);
	pSD->numWsDataAllocated += WS_ALLOC_AMOUNT;
	pSD->pWS = (WmWorkspaceData *) XtRealloc ((char *)pSD->pWS,
		    pSD->numWsDataAllocated * sizeof(WmWorkspaceData));
	pSD->pActiveWS = &(pSD->pWS[iActiveWS]);
    }

    /*
     * Give this workspace a name
     */
    pWS = &pSD->pWS[pSD->numWorkspaces];
    string = (String) GenerateWorkspaceName (pSD, pSD->numWorkspaces);
    pWS->name = XtNewString (string);

    /*
     * Initialize the workspace data structure
     */
    InitWmWorkspace (pWS, pSD);
    if (pchTitle) 
    {
	if (pWS->title)
	    XmStringFree (pWS->title);
	pWS->title = XmStringCreateLocalized ((char *)pchTitle);
    }

    /*
     * bump workspace count
     */
    pSD->numWorkspaces++;

    /*
     * update the properties that announce workspace info
     */
    SetWorkspaceInfoProperty (pWS);
    SetWorkspaceListProperty (pSD);

    SaveWorkspaceResources(pWS, (WM_RES_WORKSPACE_LIST  | 
				 WM_RES_WORKSPACE_COUNT |
				 WM_RES_WORKSPACE_TITLE));
    dtSendWorkspaceModifyNotification(pSD, pWS->id, DtWSM_REASON_ADD);

    /*
     * Insure there's an iconbox for this workspace
     */
    if (pSD->useIconBox)
    {
	AddIconBoxForWorkspace (pWS);
    }

    /*
     * Add windows to this workspaces that want to be in "all"
     * workspaces.
     */
    AddPersistentWindows (pWS);

    /*
     * Update workspace presence dialog data
     */
    UpdatePresenceWorkspaces(pSD);

    return (pWS);
} /* END OF FUNCTION CreateWorkspace */


/*************************************<->*************************************
 *
 *  DeleteWorkspace (pWS)
 *
 *
 *  Description:
 *  -----------
 *  This function deletes a workspace.
 *
 *  Inputs:
 *  ------
 *  pWS  =  pointer to screen data
 *
 *  Outputs:
 *  --------
 *  Returns pointer to workspace data if successful.
 * 
 *************************************<->***********************************/

void 
DeleteWorkspace(
        WmWorkspaceData *pWS )

{
    WmWorkspaceData *pWSdest;		/* destination WS */
    int i, iNextWs;
    ClientData *pCD;
    WmScreenData *pSD = pWS->pSD;
    Atom aOldId;

    if (pSD->numWorkspaces > 1)
    {
	/*
	 * Find index for "next" workspace
	 */
	for (iNextWs = 0; iNextWs < pSD->numWorkspaces; iNextWs++)
	{
	    if (pSD->pWS[iNextWs].id == pWS->id)
	    {
		iNextWs++;
		break;
	    }
	}

	/* check bounds and wrap */
	if (iNextWs >= pSD->numWorkspaces)
	    iNextWs = 0;

	/*
	 * Determine default destination for clients that exist
	 * only in the workspace being deleted.
	 */
	if (pWS == ACTIVE_WS)
	{
	    pWSdest = &(pSD->pWS[iNextWs]);
	}
	else
	{
	    /*
	     * Use the "current" workspace as the default destination
	     */
	    pWSdest = ACTIVE_WS;
	}

	/*
	 * Move all clients out of this workspace
	 */
	while (pWS->numClients > 0)
	{
	    /* repeatedly remove the first one until all are gone */
	    pCD = pWS->ppClients[0];


	    if (pCD->numInhabited == 1)
	    {
		if (!(pCD->clientFlags & (ICON_BOX)))
		{
		    AddClientToWorkspaces (pCD, &(pWSdest->id), 1);
		}
	    }

	    RemoveClientFromWorkspaces (pCD, &(pWS->id), 1);
	}

	/*
	 * If we're deleting the current workspace, 
	 * then change to another workspace.
	 */
	if (pWS == ACTIVE_WS)
	{
	    ChangeToWorkspace (pWSdest);
	}

	/*
	 * Save the workspace ID for the notification message.
	 */
	aOldId = pWS->id;

	/*
	 * Destroy the icon box for the workspace if one was used
	 */
	if (pSD->useIconBox)
	{
	    DestroyIconBox (pWS);
	}

	/*
	 * Delete the property containing information on this workspace
	 */
	DeleteWorkspaceInfoProperty (pWS);

	/*
	 * Delete the workspace data structures
	 */
	if (pWS->backdrop.imagePixmap)
	{
	    if (!XmDestroyPixmap (XtScreen(pWS->workspaceTopLevelW),
			    pWS->backdrop.imagePixmap))
	    {
		/* not in Xm pixmap cache */
	    }
	}

	/* free pWS->backdrop.image */
	if ((pWS->backdrop.flags & BACKDROP_IMAGE_ALLOCED) &&
	    (pWS->backdrop.image))
	{
	    free (pWS->backdrop.image);
	}

        /* 
         * Free up icon placement data
         */
        if (wmGD.iconAutoPlace)
        {
           if (pWS->IPData.placeList != NULL)
               XtFree ((char *) pWS->IPData.placeList);
           if (pWS->IPData.placementRowY != NULL)
               XtFree ((char *) pWS->IPData.placementRowY);
           if (pWS->IPData.placementColX != NULL)
               XtFree ((char *) pWS->IPData.placementColX);
        }

	XtFree ((char *) pWS->name);
	XmStringFree (pWS->title);
	XtFree ((char *) pWS->ppClients);
	if (pWS->iconBoxGeometry) XtFree ((char *) pWS->iconBoxGeometry);
	XtDestroyWidget (pWS->workspaceTopLevelW);

	/*
	 * Compress the list of workspaces if we're not deleting
	 * the last one. (Do piece-wise to avoid overlapping copy
	 * problems).
	 */
	if (iNextWs > 0)
	{
	    WmWorkspaceData *pWSdest;
	    WmWorkspaceData *pWSsrc;
	    int j;

	    pWSdest = pWS;
	    pWSsrc = &(pSD->pWS[iNextWs]);

	    for (j=iNextWs; j < pSD->numWorkspaces; j++)
	    {
		memcpy (pWSdest, pWSsrc, sizeof(WmWorkspaceData));
		if (pSD->pActiveWS == pWSsrc)
		{
		    pSD->pActiveWS = pWSdest;
		}
		pWSdest++;
		pWSsrc++;
	    }
	}

	/*
	 * We now have one less workspace.
	 */
	pSD->numWorkspaces--;

	/*
	 * Update the properties that announce workspace info.
	 */
	SetWorkspaceListProperty (pSD);

	SaveWorkspaceResources(pWSdest,
		(WM_RES_WORKSPACE_LIST | WM_RES_WORKSPACE_COUNT));

	dtSendWorkspaceModifyNotification(pSD, aOldId, DtWSM_REASON_DELETE);

	/*
	 * Update workspace presence dialog data
	 */
	UpdatePresenceWorkspaces(pSD);
    }
} /* END OF FUNCTION DeleteWorkspace */


/*************************************<->*************************************
 *
 *  ProcessDtWmHints (pCD)
 *
 *
 *  Description:
 *  -----------
 *  Process the _DT_WM_HINTS property on the window (if any).  
 *
 *
 *  Inputs:
 *  ------
 *  pCD	= pointer to client data
 *
 * 
 *  Outputs:
 *  -------
 *  pCD	= may be changed.
 *
 *************************************<->***********************************/

void 
ProcessDtWmHints (ClientData *pCD)
{
    DtWmHints *pHints;
    Atom	property;
#ifdef HP_VUE
    Atom	propertyVUE;
#endif /* HP_VUE */
#ifdef PANELIST
    long	saveFunctions;
#endif /* PANELIST */

    /*
     * Retrieve the _DT_WM_HINTS property if it exists.
     */

    property = XmInternAtom(DISPLAY, _XA_DT_WM_HINTS, False);
#ifdef HP_VUE
    propertyVUE = XmInternAtom(DISPLAY, _XA_VUE_WM_HINTS, False);
#endif /* HP_VUE */

    if (
#ifdef HP_VUE
	((HasProperty (pCD, property)) || (HasProperty (pCD, propertyVUE))) 
#else /* HP_VUE */
	(HasProperty (pCD, property)) 
#endif /* HP_VUE */
        && (_DtWsmGetDtWmHints (DISPLAY, pCD->client, &pHints) == Success))
    {
	pCD->clientFlags |= GOT_DT_WM_HINTS;
	if (pHints->flags & DtWM_HINTS_FUNCTIONS)
	{
	    if (pHints->functions & DtWM_FUNCTION_ALL)
	    {
		/* client indicating inapplicable functions */
		pCD->dtwmFunctions &= ~(pHints->functions);
	    }
	    else
	    {
		/* client indicating applicable functions */
		pCD->dtwmFunctions &= pHints->functions;
	    }
	}

	if (pHints->flags & DtWM_HINTS_BEHAVIORS)
	{
	    /* set applicable behaviors */
	    pCD->dtwmBehaviors = pHints->behaviors;
	    if (pCD->dtwmBehaviors & DtWM_BEHAVIOR_SUB_RESTORED)
	    {
		/*
		 * if this is a restored subpanel, remove the
		 * DtWM_BEHAVIOR_SUB_RESTORED bit so the next
		 * time through the subpanel will behave as an
		 * existing subpanel
		 */
		pHints->behaviors &= ~DtWM_BEHAVIOR_SUB_RESTORED;
		_DtWsmSetDtWmHints (DISPLAY, pCD->client, pHints);
	    }
	}

	XFree ((char*)pHints);
    }
#ifdef PANELIST

    if (pCD->dtwmBehaviors & DtWM_BEHAVIOR_PANEL)
    {
	/* put it in all workspaces */
	saveFunctions = pCD->dtwmFunctions;
	pCD->dtwmFunctions |= DtWM_FUNCTION_OCCUPY_WS;

	F_AddToAllWorkspaces (NULL, pCD, NULL);

	pCD->dtwmFunctions = saveFunctions;
	pCD->clientFlags |= FRONT_PANEL_BOX ;
    }
#endif /* PANELIST */
} /* END OF ProcessDtWmHints */


/*************************************<->*************************************
 *
 *  GetClientWorkspaceInfo (pCD, manageFlags);
 *
 *
 *  Description:
 *  -----------
 *  This function sets up the portion of client data that has to
 *  do with workspaces 
 *
 *  Inputs:
 *  ------
 *  pCD  =  pointer to client data (only partly initialized!!)
 *  manageFlags = tells us, in particular, if we're restarting.
 *
 *  Outputs:
 *  --------
 *  pCD  = updated client data
 *
 *************************************<->***********************************/

Boolean 
GetClientWorkspaceInfo(
        ClientData *pCD,
        long manageFlags )

{
    Atom *pIDs;
    int i;
    unsigned int numIDs = 0;
    Boolean bAll;

    /* 
     * Allocate initial workspace ID list 
     * fill with NULL IDs
     */
    if ((pCD->pWsList = (WsClientData *) 
	    XtMalloc(pCD->pSD->numWorkspaces * sizeof(WsClientData))) == NULL)
    {
	Warning (((char *)GETMESSAGE(76, 4, "Insufficient memory for client data")));
	return (False);
    }
    pCD->currentWsc = 0;
    pCD->pWorkspaceHints = NULL;
    pCD->sizeWsList = pCD->pSD->numWorkspaces;
    pCD->numInhabited = 0;		/* no valid ones yet */
    for (i = 0; i < pCD->pSD->numWorkspaces; i++)
    {
	pCD->pWsList[i].wsID = NULL;
	pCD->pWsList[i].iconPlace = NO_ICON_PLACE;
	pCD->pWsList[i].iconX = 0;
	pCD->pWsList[i].iconY = 0;
	pCD->pWsList[i].iconFrameWin = NULL;
	pCD->pWsList[i].pIconBox = NULL;
    }
    pCD->putInAll = bAll = False;

    /* 
     * Determine initial workspace set.
     *
     * If this is a secondary window, use the hints from the
     * transient tree leader.
     *
     * Else if we're restarting, then use our own workspace presence.
     *
     * Else if a command line option is specified, use that.
     *
     * Else, if workspace hints are on the window, then use them.
     *
     * If none of the above work out, the window will be put into
     * the current workspace.
     */
    if (pCD->client &&
	((pCD->transientLeader && GetLeaderPresence(pCD, &pIDs, &numIDs)) ||
	 ((manageFlags & MANAGEW_WM_RESTART) && 
	   GetMyOwnPresence (pCD, &pIDs, &numIDs)) ||
	 (WorkspaceIsInCommand (DISPLAY, pCD, &pIDs, &numIDs)) ||
	 (
#ifdef HP_VUE
	  (HasProperty (pCD, wmGD.xa_DT_WORKSPACE_HINTS) ||
	   HasProperty (pCD, 
			XmInternAtom (DISPLAY, _XA_VUE_WORKSPACE_HINTS,
			False)))
#else /* HP_VUE */
	  HasProperty (pCD, wmGD.xa_DT_WORKSPACE_HINTS) 
#endif /* HP_VUE */
	  && (GetWorkspaceHints (DISPLAY, pCD->client, &pIDs, &numIDs, &bAll) == 
	         Success))) &&
	 numIDs)
    {
	/*
	 * Got some workspace hints! 
	 */
	pCD->putInAll = bAll;
	ProcessWorkspaceHintList (pCD, pIDs, numIDs);
    }

    if (pCD->numInhabited == 0)
    {
	/*
	 * If not in any workspaces, then put the client into
	 * the current one.
	 */
	PutClientIntoWorkspace (pCD->pSD->pActiveWS, pCD);
    }

    return (True);

} /* END OF FUNCTION GetClientWorkspaceInfo */


/*************************************<->*************************************
 *
 *  WorkspaceIsInCommand (dpy, pCD, ppIDs, pNumIDs)
 *
 *
 *  Description:
 *  -----------
 *  Determine if workspace specification is in command line for client
 *
 *
 *  Inputs:
 *  ------
 *  dpy		- pointer to display structure
 *  pCD		- ptr to client data
 *  ppIDs	- pointer for returning list of IDs
 *  pNumIDs     - number of IDs being returned
 * 
 *  Outputs:
 *  -------
 *  ppIDs	- returned list of IDs
 *  pNumIDs     - number of IDs being returned
 *
 *  Return 	- True if workspace option found, false otherwise
 *
 *
 *  Comments:
 *  --------
 *  Malloc's memory that must be freed
 * 
 *************************************<->***********************************/

Boolean 
WorkspaceIsInCommand(
        Display *dpy,
        ClientData *pCD,
        WorkspaceID **ppIDs,
        unsigned int *pNumIDs )
{
    int wmcArgc;
    char **wmcArgv = NULL;
    Boolean rval = False;
    unsigned char *pch = NULL;
    XTextProperty clientMachineProp;

    if (FindClientDBMatch(pCD, (char **)&pch))
    {
        if (pch)
	{
	    if (ConvertNamesToIDs (pCD->pSD, pch, ppIDs, pNumIDs))
	    {
	        rval = True;
	    }
	    XtFree((char *)pch);
	}
    }
    else if (HasProperty (pCD, XA_WM_COMMAND) &&
	     XGetCommand (dpy, pCD->client, &wmcArgv, &wmcArgc) &&
	     (wmcArgv != NULL))
    {
	if (pCD->pSD->remainingSessionItems)
	{
	    if(!(XGetWMClientMachine(dpy, pCD->client, &clientMachineProp)))
	    {
		clientMachineProp.value = NULL;
	    }
	    if (FindDtSessionMatch(wmcArgc, wmcArgv, pCD, pCD->pSD, 
				    (char **)&pch, 
				    (char *)clientMachineProp.value))
	    {
		/*
		 * If we found a match to a client description
		 * in the DtSessionHints, use the information from
		 * the Hints instead of the command line
		 */
		if (pch)
		{
		    if (ConvertNamesToIDs (pCD->pSD, pch, ppIDs, pNumIDs))
		    {
			rval = True;
		    }
		    /*
		     * free malloced memory containing workspace list
		     */
		    XtFree ((char *)pch);
		}
		    
	    }
	    if (clientMachineProp.value)
	    {
		XFree ((char*)clientMachineProp.value);
	    }
	}

	if (!rval && FindWsNameInCommand (wmcArgc, wmcArgv, &pch))
	{
	    if (ConvertNamesToIDs (pCD->pSD, pch, ppIDs, pNumIDs))
	    {
		rval = True;
	    }
	}

	if (wmcArgv != NULL)
	{
	    XFreeStringList (wmcArgv);
	}
    }

    return (rval);
} /* END OF FUNCTION WorkspaceIsInCommand */


/*************************************<->*************************************
 *
 *  ConvertNamesToIDs (pSD, pch, ppAtoms, pNumAtoms)
 *
 *
 *  Description:
 *  -----------
 *  Takes a string containing a list of names separated by white space
 *  and converts it to a list of workspace IDs.
 * 
 *  Inputs:
 *  ------
 *  pSD		- pointer to screen data
 *  pchIn	- pointer to original string
 *  ppAtoms	- pointer to an atom pointer (for returning list pointer)
 *  pNumAtoms	- pointer to the number of atoms being returned.
 * 
 *  Outputs:
 *  -------
 *  *ppAtoms	- points to a list of atoms returned.
 *  *pNumAtoms	- the number of atoms being returned.
 *
 *  Return 	- True if some Atoms are being returned
 *
 *  Comments:
 *  --------
 *  Processes local copy of string so that pch is not modified.
 *
 *  The list of atoms returned has been dynamically allocated. 
 *  Please XtFree() it when you're done.
 *
 *************************************<->***********************************/

Boolean 
ConvertNamesToIDs(
        WmScreenData *pSD,
        unsigned char *pchIn,
        WorkspaceID **ppAtoms,
        unsigned int *pNumAtoms )

{
    unsigned char *pchLocal, *pch, *pchName;
    int num = 0;
    int numLocalIDs;
    WorkspaceID *pLocalIDs;

    if ((pLocalIDs = (WorkspaceID *) XtMalloc (WS_ALLOC_AMOUNT *
	sizeof(WorkspaceID))) == NULL)
    {
	Warning (((char *)GETMESSAGE(76, 5, "Insufficient Memory (ConvertNamesToIDs)")));
	ExitWM (WM_ERROR_EXIT_VALUE);
    }
    numLocalIDs = WS_ALLOC_AMOUNT;

   if (pchIn && (pchLocal = (unsigned char *) XtMalloc(1+strlen((char *)pchIn))))
   {
        strcpy ((char *)pchLocal, (char *)pchIn);
	pch = pchLocal;

	while (pchName = GetSmartString (&pch))
	{
	    int iwsx;
	    XmString xms;

	    /*
	     * Check workspace for workspace titles; map to 
	     * workspace names.
	     */
            xms = XmStringCreateLocalized ((char *)pchName);
	    for (iwsx = 0; iwsx < pSD->numWorkspaces; iwsx++)
	    {
		if (XmStringCompare (xms, pSD->pWS[iwsx].title))
		{
		    break;
		}
	    }
	    XmStringFree (xms);

	    if (iwsx < pSD->numWorkspaces)
	    {
	       /*
		* Found a workspace title we've got,
		* use id for workspace name
		*/
		pLocalIDs[num] = pSD->pWS[iwsx].id;
		num++;
	    }
	    else 
	    {
		/*
		 * Try for match on workspace name
		 */
		pLocalIDs[num] = (WorkspaceID) 
			    XInternAtom (DISPLAY, (char *)pchName, False);
		num++;
	    }

	    if (num >= numLocalIDs)
	    {
		/* list too small */
		numLocalIDs += WS_ALLOC_AMOUNT;
		if ((pLocalIDs = (WorkspaceID *) XtRealloc ((char *)pLocalIDs,
			    numLocalIDs * sizeof(WorkspaceID))) == NULL)
		{
		    Warning (((char *)GETMESSAGE(76, 6, "Insufficient Memory (ConvertNamesToIDs)")));
		    ExitWM (WM_ERROR_EXIT_VALUE);
		}
	    }
	}

	XtFree ((char *)pchLocal);
    }

    *ppAtoms = pLocalIDs;
    *pNumAtoms = num;
    return (num != 0);
    
} /* END OF FUNCTION ConvertNamesToIDs */


/*************************************<->*************************************
 *
 *  CheckForPutInAllRequest (pCD, pIDs, numIDs)
 *
 *
 *  Description:
 *  -----------
 *  Tests for the presence of the "all" atom in the atom list
 *  and sets the "putInAll" flag on the client.
 * 
 *  Inputs:
 *  ------
 *  pCD		- pointer to client data
 *  pIDs	- pointer to ID list
 *  numIDs	- number of IDs in list
 * 
 *  Outputs:
 *  -------
 *  pCD		- putInAll member may be set
 *
 *************************************<->***********************************/

void 
CheckForPutInAllRequest(
        ClientData *pCD,
        Atom *pIDs,
        unsigned int numIDs )

{
    unsigned int i;

    for (i = 0; (i < numIDs) && !(pCD->putInAll); i++)
    {
	if (pIDs[i] == wmGD.xa_ALL_WORKSPACES)
	{
	    pCD->putInAll = True;
	    break;
	}
    }
    
} /* END OF FUNCTION CheckForPutInAllRequest */


/*************************************<->*************************************
 *
 *  FindWsNameInCommand (argc, argv, ppch)
 *
 *
 *  Description:
 *  -----------
 *  Finds and returns the workspace name option in the command line 
 *  (if any)
 *
 *  Inputs:
 *  ------
 *  argc	- argument count
 *  argv	- argument list
 *  ppch	- string pointer to return
 *
 * 
 *  Outputs:
 *  -------
 *  *ppch	- points to ws name (if found)
 *
 *  Return 	- True if wsname found
 *
 *
 *  Comments:
 *  --------
 *************************************<->***********************************/

Boolean 
FindWsNameInCommand(
        int argc,
        char *argv[],
        unsigned char **ppch )

{

#define XRM_OPT		"-xrm"
#define WSLIST		"*workspaceList:"
#define WSLIST_LEN	14

    int i = 1;
    Boolean rval = False;
    unsigned char *pch, *pchTmp, *pch0;
    unsigned char *pchRes, *pchValue;
    
    if (argc > 0)
    {
	while (--argc && !rval)
	{
	    if (!strcmp(argv[i], XRM_OPT) && (argc > 1))
	    {
		/* 
		 * found "-xrm", now look at resource spec
		 */
		pch0 = (unsigned char *) strdup (argv[i+1]);
		if (!pch0)
		{
		    Warning (((char *)GETMESSAGE(76, 7, "Insufficient memory")));
		    ExitWM (WM_ERROR_EXIT_VALUE);
		}
		pch = pch0;

		/* strip off quotes ,
		 * separate two halve of resource spec
		 */
		pchRes = GetSmartString (&pch);
		pchValue = pch;

		if ((*pchRes) && (*pch))
		{
		    /* Erase colon at end of resource name */

		    pch = (unsigned char *) strrchr((char *)pchRes, ':');
		    if (pch)
		    {
			*pch = '\0';
		    }

		    /* find beginning of last component of resource
		     * spec
		     */
		    pch = (unsigned char *) strrchr ((char *)pchRes, '*');
		    pchTmp = (unsigned char *) strrchr ((char *)pchRes, '.');
		    if (pchTmp > pch)
		    {
			pch = pchTmp;
		    }

		    if (pch && *pch && *(pch+1))
		    {
			pch += 1;
		    }

		    /* compare resource with our resource */

		    if ( (!strcmp ((char *)pch, WmNworkspaceList)) ||
			 (!strcmp ((char *)pch, WmCWorkspaceList)))
		    {
			/* match, compute return position in
			   passed in string */

			*ppch = (unsigned char *) 
				(argv[i+1] + (pchValue - pch0));
			rval = True;
			XtFree ((char *)pch0);
			pch0 = NULL;
		    }
		}

		i++;		/* skip next arg */
		argc--;

		if (pch0)
		{
		    XtFree ((char *)pch0);
		}
	    }
	    i++;
	}
    }
    return (rval);
} /* END OF FUNCTION FindWsNameInCommand */



/*************************************<->*************************************
 *
 *  PutClientIntoWorkspace (pWS, pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function updates the data for the client and workspace to
 *  reflect the presence of the client in the workspace.
 *
 *  Inputs:
 *  ------
 *  pWS = pointer to workspace data
 *  pCD = pointer to client data
 *
 *  Outputs:
 *  --------
 *
 *************************************<->***********************************/

void 
PutClientIntoWorkspace(
        WmWorkspaceData *pWS,
        ClientData *pCD )

{
    int i = pCD->numInhabited;
    int iAdded, j, k;

    /* insure the client's got enough workspace data */
    if (pCD->sizeWsList < pCD->pSD->numWorkspaces)
    {
	iAdded = pCD->pSD->numWorkspaces - pCD->sizeWsList;

	pCD->sizeWsList = pCD->pSD->numWorkspaces;
	pCD->pWsList = (WsClientData *) 
		XtRealloc((char *)pCD->pWsList, 
		    (pCD->pSD->numWorkspaces * sizeof(WsClientData)));

	/* intialized new data */
	j = pCD->sizeWsList - 1;
	for (j=1; j <= iAdded; j++)
	{
	    k = pCD->sizeWsList - j;
	    pCD->pWsList[k].iconPlace = NO_ICON_PLACE;
	    pCD->pWsList[k].iconX = 0;
	    pCD->pWsList[k].iconY = 0;
	    pCD->pWsList[k].iconFrameWin = (Window) 0;
	    pCD->pWsList[k].pIconBox = NULL;
	}
    }


    /* update the client's list of workspace data */
    pCD->pWsList[i].wsID = pWS->id; 
    pCD->numInhabited++;

    if (!(pCD->clientFlags & WM_INITIALIZATION))
    {
	/* 
	 * Make sure there's an icon 
	 * (Don't do this during initialization, the pCD not
	 * ready for icon making yet).
	 */
	InsureIconForWorkspace (pWS, pCD);
    }

    /* update the workspace list of clients */
    AddClientToWsList (pWS, pCD);

} /* END OF FUNCTION PutClientIntoWorkspace */


/*************************************<->*************************************
 *
 *  TakeClientOutOfWorkspace (pWS, pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function updates the data for the client and the workspace
 *  to reflect the removal of the client from the workspace.
 *
 *  Inputs:
 *  ------
 *  pWS = pointer to workspace data
 *  pCD = pointer to client data
 *
 *  Outputs:
 *  --------
 *
 *************************************<->***********************************/

void 
TakeClientOutOfWorkspace(
        WmWorkspaceData *pWS,
        ClientData *pCD )

{
    int ixA;
    Boolean Copying = False;
    WsClientData *pWsc;

    if (pWS && pCD && ClientInWorkspace(pWS, pCD))
    {
	/*
	 * Clean up icon
	 */
	if (!pCD->transientLeader)
	{
	    pWsc = GetWsClientData (pWS, pCD);

	    if ((pCD->pSD->useIconBox) && 
		(pWsc->pIconBox) &&
		(pCD->clientFunctions & MWM_FUNC_MINIMIZE))
	    {
		DeleteIconFromBox (pWS->pIconBox, pCD);
	    }
	    else if (wmGD.iconAutoPlace)
	    {
		/* 
		 * Free up root icon spot 
		 */

	        if ((pWsc->iconPlace != NO_ICON_PLACE) &&
		    (pWS->IPData.placeList[pWsc->iconPlace].pCD == pCD))
		{
		    pWS->IPData.placeList[pWsc->iconPlace].pCD = NULL;
		    pWsc->iconPlace = NO_ICON_PLACE;
		}
	    }
	}

	/* 
	 *  Remove the selected workspace and copy the remaining ones
	 *  up. (Do piece-wise to avoid overlapping copy.)
	 */
	for (ixA = 0; ixA < pCD->numInhabited; ixA++)
	{
	    if (Copying)
	    {
		memcpy (&pCD->pWsList[ixA-1], &pCD->pWsList[ixA], 
			sizeof(WsClientData));
	    }
	    else if (pCD->pWsList[ixA].wsID == pWS->id)
	    {
		/* 
		 *  This is the one we're removing, start copying here.
		 */
		Copying = True;
	    }
	}

	/* 
	 * Decrement the number of workspaces inhabited.
	 */
	pCD->numInhabited--;

	/* update the workspaces list of clients */
	RemoveClientFromWsList (pWS, pCD);
    }
#ifdef DEBUG
    else
    {
	Warning("TakeClientOutOfWorkspace: null workspace passed in.");
    }
#endif /* DEBUG */


} /* END OF FUNCTION TakeClientOutOfWorkspace */


/*************************************<->*************************************
 *
 *  GetWorkspaceData (pSD, wsID)
 *
 *
 *  Description:
 *  -----------
 *  This function finds the data that is associated with a workspace ID.
 *
 *  Inputs:
 *  ------
 *  pSD   = pointer to screen data
 *  wsID  =  workspace ID
 *
 *  Outputs:
 *  --------
 *  Function returns a pointer to the workspace data if successful,
 *  or NULL if unsuccessful.
 *
 *************************************<->***********************************/

WmWorkspaceData * 
GetWorkspaceData(
        WmScreenData *pSD,
        WorkspaceID wsID )

{
    WmWorkspaceData *pWS = NULL;
    int i;

    for (i=0; i < pSD->numWorkspaces; i++)
    {
	if (pSD->pWS[i].id == wsID) 
	{
	    pWS = &pSD->pWS[i];
	    break;
	}
    }

#ifdef DEBUG
    if (!pWS)
    {
	/* failed to find one */
        Warning ("Failed to find workspace data");
    }
#endif

    return (pWS);

}  /* END OF FUNCTION GetWorkspaceData */



/*************************************<->*************************************
 *
 *  GenerateWorkspaceName (pSD, wsnum)
 *
 *
 *  Description:
 *  -----------
 *  This function generates and returns a workspace string name from
 *  a small number passed in.
 *
 *  Inputs:
 *  ------
 *  pSD = pointer to screen data
 *  wsNum  =  number for workspace
 *
 * 
 *  Outputs:
 *  -------
 *  returns pointer to statically allocated data. You must copy it
 *  to your local buffer.
 *
 *  Comments:
 *  ---------
 *  Name is of the form ws<n> where <n> is a number.
 * 
 *************************************<->***********************************/

unsigned char * 
GenerateWorkspaceName(
        WmScreenData *pSD,
        int wsnum )

{
    static unsigned char nameReturned[10];
    int i, j;

    /*
     * Nice n-squared algorithm...
     * (This should be OK for small number of workspaces)
     */
    for (i=0; i <= pSD->numWorkspaces; i++)
    {
	/* generate a name */
	sprintf ((char *)nameReturned, "ws%d", i);
	if (!DuplicateWorkspaceName (pSD, nameReturned, wsnum))
	    break;
    }

    return (nameReturned);

}  /* END OF FUNCTION GenerateWorkspaceName */



/*************************************<->*************************************
 *
 *  InWindowList (w, wl, num)
 *
 *
 *  Description:
 *  -----------
 *  This function determines if a window is in a list of windows
 *
 *  Inputs:
 *  ------
 *  w = window of interest
 *  wl = list of windows
 *  num = number of windows in wl
 *
 * 
 *  Outputs:
 *  -------
 *  The function returns "True" if "w" appears in "wl"
 *
 *************************************<->***********************************/

Boolean 
InWindowList(
        Window w,
        Window wl[],
        int num )

{
    int i;
    Boolean rval = False;

    for (i = 0; (i < num) && !rval; i++)
    {
	if (w == wl[i])
	{
	    rval = True;
	}
    }

    return (rval);

}   /* END OF FUNCTION InWindowList */


/*************************************<->*************************************
 *
 *  ClientInWorkspace (pWS, pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function determines if a client is in a workspace
 *
 *  Inputs:
 *  ------
 *  pWS = pointer to workspace data
 *  pCD = pointer to client data
 * 
 *  Outputs:
 *  -------
 *  The function returns "True" if client pCD is in workspace pWS
 *
 *************************************<->***********************************/

Boolean 
ClientInWorkspace(
        WmWorkspaceData *pWS,
        ClientData *pCD )

{
    int i;
    Boolean rval = False;

    for (i = 0; (i < pCD->numInhabited) && !rval; i++)
    {
	if (pWS->id == pCD->pWsList[i].wsID)
	{
	    rval = True;
	}
    }

    return (rval);

}   /* END OF FUNCTION ClientInWorkspace */


/*************************************<->*************************************
 *
 *  GetWsClientData (pWS, pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function returns a pointer to the client's specific data for
 *  this workspace
 *
 *  Inputs:
 *  ------
 *  pWS = pointer to workspace data
 *  pCD = pointer to client data
 * 
 *  Outputs:
 *  -------
 *  The function returns a pointer to the client's data for this
 *  workspace. If the client isn't in the workspace, an error is 
 *  printed and the first datum in the workspace list is returned.
 *
 *************************************<->***********************************/

WsClientData * 
GetWsClientData(
        WmWorkspaceData *pWS,
        ClientData *pCD )

{
    int i;
    WsClientData *pWsc = NULL;

    for (i = 0; (i < pCD->numInhabited) && !pWsc; i++)
    {
	if (pWS->id == pCD->pWsList[i].wsID)
	{
	    pWsc = &pCD->pWsList[i];
	}
    }

    if (!pWsc)
    {
	pWsc = &pCD->pWsList[0];
    }

    return (pWsc);

}   /* END OF FUNCTION GetWsClientData */


/*************************************<->*************************************
 *
 *  SetClientWsIndex (pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function sets the index into the client's array of workspace
 *  specific data. This index points to the data to be used for the
 *  currently active workspace.
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 * 
 *  Outputs:
 *  -------
 *  The function returns an index as described above. If the client is
 *  not in the currently active workspace, then the index returned is 0.
 *
 *************************************<->***********************************/

void 
SetClientWsIndex(
        ClientData *pCD )

{
    int i;
    WmWorkspaceData *pWS = pCD->pSD->pActiveWS;

    for (i = 0; (i < pCD->numInhabited); i++)
    {
	if (pWS->id == pCD->pWsList[i].wsID)
	{
	    break;
	}
    }

    if (i >= pCD->numInhabited)
    {
	i = 0;
    }

    pCD->currentWsc = i;

}   /* END OF FUNCTION SetClientWsIndex */



/*************************************<->*************************************
 *
 *  ProcessWmWorkspaceHints (pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function processes a change to the _DT_WORKSPACE_HINTS property
 *  on a window that we manage.
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 * 
 *  Outputs:
 *  -------
 *  Returns False if we ran out of memory or no hints.
 *  Returns True on success.
 *
 *************************************<->***********************************/

Boolean 
ProcessWorkspaceHints(
        ClientData *pCD )

{
    Atom *pIDs;
    int i, j;
    unsigned int numIDs;
    WmScreenData *pSD = PSD_FOR_CLIENT(pCD);
    Boolean rval = False;
    Boolean InBoth;
    Boolean bAll;
    int numOld;
    WorkspaceID *pDiff = NULL;
    int numDiff;

    numOld = pCD->numInhabited;
    ReserveIdListSpace (numOld);
    for (i = 0; i < numOld; i++)
    {
	pResIDs[i] = pCD->pWsList[i].wsID;
    }

    if ((pCD->client) && 
	(GetWorkspaceHints (DISPLAY, pCD->client, 
			    &pIDs, &numIDs, &bAll) == Success) &&
        (bAll || (numIDs && (pDiff = (WorkspaceID *) 
	          XtMalloc (sizeof(WorkspaceID) * MAX(numIDs, numOld))))))
    {
	/*
	 *  Process request to put window in all workspaces
	 */
	pCD->putInAll = bAll;
	CheckForPutInAllRequest (pCD, pIDs, numIDs);

	if (!pCD->putInAll)
	{
	    /*
	     * Compute the ids to be removed.
	     */
	    numDiff = 0;

	    for (i=0; i < numOld; i++)
	    {
		InBoth = False;
		for (j=0; j < numIDs; j++)
		{
		    if (pIDs[j] == pResIDs[i])
		    {
			InBoth = True;
			break;
		    }
		}

		if (!InBoth)
		{
		    pDiff[numDiff++] = pResIDs[i];
		}
	    }

	    /*
	     * Remove the client from the set of workspaces
	     */
	    if (numDiff)
	    {
		RemoveClientFromWorkspaces (pCD, pDiff, numDiff);
	    }
	}

	/*
	 *  Process request to put window in all workspaces
	 */

	if (pCD->putInAll)
	{
	    for (i=0; i<pCD->pSD->numWorkspaces; i++)
	    {
		if (!ClientInWorkspace(&pCD->pSD->pWS[i], pCD))
		{
		    AddClientToWorkspaces (pCD, &pCD->pSD->pWS[i].id, 1);
		}
	    }
	}
	else
	{
	    /*
	     * Compute the ids to be added.
	     */

	    numDiff = 0;
	    for (i=0; i < numIDs; i++)
	    {
		InBoth = False;
		for (j=0; j < numOld; j++)
		{
		    if (pResIDs[j] == pIDs[i])
		    {
			InBoth = True;
			break;
		    }
		}

		if (!InBoth)
		{
		    pDiff[numDiff++] = pIDs[i];
		}
	    }

	    /*
	     * Add the client to the set of workspaces
	     */
	    if (numDiff) 
	    {
		AddClientToWorkspaces (pCD, pDiff, numDiff);
	    }
	}

	/*
	 * If the client is not in any workspaces, then
	 * put it in the current one 
	 *
	 * !!! Is this right? !!!
	 */
	if (pCD->numInhabited == 0)
	{
	    AddClientToWorkspaces (pCD, &pSD->pActiveWS->id, 1);
	}

	/*
	 * Free up the old list of hints
	 */
	if (pCD->pWorkspaceHints)
	{
	    XFree ((char *)pCD->pWorkspaceHints);
	}
	if (pDiff)
	{
	    XtFree ((char *)pDiff);
	}

	/* 
	 * Save the new hints
	 */
	pCD->pWorkspaceHints = pIDs;
	pCD->numWorkspaceHints = numIDs;

	/* 
	 * Update the presence property
	 */
	UpdateWorkspacePresenceProperty (pCD);

	rval = True;
    }
    return (rval);

}   /* END OF FUNCTION ProcessWorkspaceHints */


/*************************************<->*************************************
 *
 *  InsureUniqueWorkspaceHints (pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function processes the workspace hints and removes duplicates.
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 * 
 *  Outputs:
 *  -------
 *  May modify *pWorkspaceHints and numWorkspaceHints
 *
 *************************************<->***********************************/

static void 
InsureUniqueWorkspaceHints(
        ClientData *pCD )

{
    int next, trail, i;
    WorkspaceID *pID;
    Boolean  duplicate;


    if (pCD->numWorkspaceHints < 2) return;

    pID = pCD->pWorkspaceHints;

    trail = 0;
    next = 1;

    while (next < pCD->numWorkspaceHints)
    {
	duplicate = False;
	for (i = 0; i < next; i++)
	{
	    if (pID [next] == pID [i])
	    {
		/* duplicate found! */
		duplicate = True;
		break;
	    }
	}

	if (duplicate)
	{
	    /* skip duplicates */
	    next++;
	}
	else
	{
	    /* not a duplicate */
	    trail++;
	    if (next > trail)
	    {
		/*
		 * We need to copy up over an old duplicate
		 */
		pID [trail] = pID [next];
	    }
	}
	next++;
    }

    pCD->numWorkspaceHints = trail+1;

}   /* END OF FUNCTION InsureUniqueWorkspaceHints */



/*************************************<->*************************************
 *
 *  ProcessWorkspaceHintList (pCD, pIDs, numIDs)
 *
 *
 *  Description:
 *  -----------
 *  This function processes a list of workspace hints for a client.
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 *  pIDs = pointer to a list of workspace IDs
 *  numIDs = number of IDs in the list
 * 
 *  Outputs:
 *  -------
 *
 *************************************<->***********************************/

void 
ProcessWorkspaceHintList(
        ClientData *pCD,
        WorkspaceID *pIDs,
        unsigned int numIDs )

{
    int i;
    WmWorkspaceData *pWS;


    if (numIDs > 0)
    {
	/*
	 * Keep these hints; make sure there are no duplicate
	 * workspace requests.
	 */
	pCD->pWorkspaceHints = pIDs;
	pCD->numWorkspaceHints = numIDs;
	InsureUniqueWorkspaceHints (pCD);
	numIDs = pCD->numWorkspaceHints;

	if (pCD->pWorkspaceHints)
	{
	    /*
	     *  Process request to put window in all workspaces
	     */
	    CheckForPutInAllRequest (pCD, pIDs, numIDs);

	    if (pCD->putInAll)
	    {
		for (i=0; i<pCD->pSD->numWorkspaces; i++)
		{
		    PutClientIntoWorkspace (&pCD->pSD->pWS[i], pCD);
		}
	    }
	    else
	    {
		for (i=0; i<numIDs; i++)
		{
		    /*
		     * Put the client into requested workspaces that
		     * exist.
		     */
		    if (pWS = GetWorkspaceData (pCD->pSD, 
						pCD->pWorkspaceHints[i]))
		    {
			PutClientIntoWorkspace (pWS, pCD);
		    }
		}
	    }
	}
    }

}   /* END OF FUNCTION ProcessWorkspaceHintList */


/*************************************<->*************************************
 *
 *  RemoveSingleClientFromWorkspaces (pCD, pIDs, numIDs)
 *
 *
 *  Description:
 *  -----------
 *  This function removes a single client from a list of workspaces
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 *  pIDs = pointer to a list of workspace IDs
 *  numIDs = number of workspace IDs
 * 
 *  Outputs:
 *  -------
 *
 *************************************<->***********************************/

void 
RemoveSingleClientFromWorkspaces(
        ClientData *pCD,
        WorkspaceID *pIDs,
        unsigned int numIDs )

{
    int i;
    WmWorkspaceData *pWS;

    for (i=0; i < numIDs; i++)
    {
	/*
	 *  Remove the client from the specified workspaces 
	 */
	if ((pWS = GetWorkspaceData (pCD->pSD, pIDs[i])) &&
	    (ClientInWorkspace (pWS, pCD)))
	{
	    /*
	     * If this workspace is active, then make the
	     * window unseen.  We only need to call
	     * SetClientState on the main window, the
	     * transients will get taken care of in there.
	     */
	    if ((pWS == pCD->pSD->pActiveWS) &&
		(pCD->transientLeader == NULL) &&
		!(pCD->clientState & UNSEEN_STATE))
	    {
		SetClientState (pCD, 
		    (pCD->clientState | UNSEEN_STATE), CurrentTime);
	    }
	    TakeClientOutOfWorkspace (pWS, pCD);

	    /* 
	     * Update the presence property
	     */
	    UpdateWorkspacePresenceProperty (pCD);
	}
    }

}   /* END OF FUNCTION RemoveSingleClientFromWorkspaces */

/*************************************<->*************************************
 *
 *  RemoveSubtreeFromWorkspaces (pCD, pIDs, numIDs)
 *
 *
 *  Description:
 *  -----------
 *  This function removes a transient subtree from a list of workspaces
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 *  pIDs = pointer to a list of workspace IDs
 *  numIDs = number of workspace IDs
 * 
 *  Outputs:
 *  -------
 *
 *************************************<->***********************************/

void 
RemoveSubtreeFromWorkspaces(
        ClientData *pCD,
        WorkspaceID *pIDs,
        unsigned int numIDs )

{
    ClientData *pNext;

    pNext = pCD->transientChildren;
    while (pNext)
    {
	/* process all children first */
	if (pNext->transientChildren)
	{
	    RemoveSubtreeFromWorkspaces (pNext, pIDs, numIDs);
	}
	else
	{
	    RemoveSingleClientFromWorkspaces (pNext, pIDs, numIDs);
	}
	pNext = pNext->transientSiblings;
    }

    /* process the primary window */
    RemoveSingleClientFromWorkspaces (pCD, pIDs, numIDs);


}   /* END OF FUNCTION RemoveSubtreeFromWorkspaces */


#ifdef PANELIST

/******************************<->*************************************
 *
 *  pIDs = GetListOfOccupiedWorkspaces (pCD, numIDs)
 *
 *
 *  Description:
 *  -----------
 *  This function creates a list of occupied workspaces of a particular
 *  client, EXCLUDING the current workspace.
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 * 
 *  Outputs:
 *  -------
 *  pIDs = pointer to a list of workspace IDs
 *  numIDs = number of workspace IDs
 *
 *  Comment
 *  -------
 *  memory for pIDs is allocated with XtMalloc and should be 
 *  freed with XtFree.
 *
 *
 ******************************<->***********************************/
WorkspaceID * 
GetListOfOccupiedWorkspaces(
        ClientData *pCD,
        int *numIDs )
{
    int i;

    WorkspaceID *pLocalIDs = NULL;

    WorkspaceID activeWsID = pCD->pSD->pActiveWS->id;

    *numIDs = 0;

    if ((pLocalIDs = (WorkspaceID *) XtMalloc (pCD->numInhabited *
	sizeof(WorkspaceID))) == NULL)
    {
	Warning (((char *)GETMESSAGE(76, 7, "Insufficient memory")));
	return (NULL);
    }

    for (i = 0; i < pCD->numInhabited; i++)
    {
	if (activeWsID != pCD->pWsList[i].wsID)
	{
	      pLocalIDs[(*numIDs)++] = pCD->pWsList[i].wsID;
	}
    }

    return(pLocalIDs);

}   /* END OF FUNCTION GetListOfOccupiedWorkspaces */
#endif /* PANELIST */


/******************************<->*************************************
 *
 *   HonorAbsentMapBehavior(pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function adds a client to the current workspace and
 *   if (pCD->absentMapBehavior == AMAP_BEHAVIOR_MOVE)
 *  removes the client from the other  workspaces 
 *
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 * 
 *  Outputs:
 *  -------
 *
 ******************************<->***********************************/

void 
HonorAbsentMapBehavior(
        ClientData *pCD)
{
    int inWorkspace = 0;

    if (pCD->absentMapBehavior == AMAP_BEHAVIOR_MOVE)
    {
	int wsCnt;

	/* 
	 * Remove from other workspaces
	 */
	for (wsCnt = 0; wsCnt < pCD->numInhabited; wsCnt = inWorkspace)
	{
	    if (pCD->pWsList[wsCnt].wsID != pCD->pSD->pActiveWS->id)
	    {
		RemoveClientFromWorkspaces (pCD, 
					    &pCD->pWsList[wsCnt].wsID, 1);
	    }
	    else inWorkspace++;
	}
    }

    if (inWorkspace == 0)
	AddClientToWorkspaces (pCD, &ACTIVE_WS->id, 1);

}   /* END OF FUNCTION HonorAbsentMapBehavior */



/******************************<->*************************************
 *
 *  RemoveClientFromWorkspaces (pCD, pIDs, numIDs)
 *
 *
 *  Description:
 *  -----------
 *  This function removes a client from a list of workspaces
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 *  pIDs = pointer to a list of workspace IDs
 *  numIDs = number of workspace IDs
 * 
 *  Outputs:
 *  -------
 *
 ******************************<->***********************************/

void 
RemoveClientFromWorkspaces(
        ClientData *pCD,
        WorkspaceID *pIDs,
        unsigned int numIDs )

{
    ClientData *pcdLeader;

    pcdLeader = (pCD->transientLeader) ? FindTransientTreeLeader (pCD) : pCD;

    RemoveSubtreeFromWorkspaces (pcdLeader, pIDs, numIDs);


}   /* END OF FUNCTION RemoveClientFromWorkspaces */


/*************************************<->*************************************
 *
 *  AddSingleClientToWorkspaces (pCD, pIDs, numIDs)
 *
 *
 *  Description:
 *  -----------
 *  This function adds a single client to a list of workspaces
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 *  pIDs = pointer to a list of workspace IDs
 *  numIDs = number of workspace IDs
 * 
 *  Outputs:
 *  -------
 *
 *************************************<->***********************************/

void 
AddSingleClientToWorkspaces(
        ClientData *pCD,
        WorkspaceID *pIDs,
        unsigned int numIDs )

{
    int i;
    WmWorkspaceData *pWS;

    for (i=0; i < numIDs; i++)
    {
	/*
	 *  Add the client to the specified workspaces if 
	 *  it is not already there.
	 */
	if ((pWS = GetWorkspaceData (pCD->pSD, pIDs[i])) &&
	    (!ClientInWorkspace (pWS, pCD)))
	{
	    PutClientIntoWorkspace (pWS, pCD);

	    if ((pWS == PSD_FOR_CLIENT(pCD)->pActiveWS) &&
		(pCD->transientLeader == NULL) &&
		(pCD->clientState & UNSEEN_STATE))
	    {
		SetClientState (pCD, 
		    (pCD->clientState & ~UNSEEN_STATE), CurrentTime);
	    }

	    /* 
	     * Update the presence property (only on transient leader)
	     */
	    UpdateWorkspacePresenceProperty (pCD);

	}
    }
} /* END OF FUNCTION AddSingleClientToWorkspace */


/*************************************<->*************************************
 *
 *  AddSubtreeToWorkspaces (pCD, pIDs, numIDs)
 *
 *
 *  Description:
 *  -----------
 *  This function adds a client subtree to a list of workspaces
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data (head of subtree)
 *  pIDs = pointer to a list of workspace IDs
 *  numIDs = number of workspace IDs
 * 
 *  Outputs:
 *  -------
 *
 *************************************<->***********************************/

void 
AddSubtreeToWorkspaces(
        ClientData *pCD,
        WorkspaceID *pIDs,
        unsigned int numIDs )

{
    ClientData *pNext;

    pNext = pCD->transientChildren;
    while (pNext)
    {
	/* process all children first */
	if (pNext->transientChildren)
	{
	    AddSubtreeToWorkspaces (pNext, pIDs, numIDs);
	}
	else
	{
	    AddSingleClientToWorkspaces (pNext, pIDs, numIDs);
	}
	pNext = pNext->transientSiblings;
    }

    /* process the primary window */
    AddSingleClientToWorkspaces (pCD, pIDs, numIDs);


}   /* END OF FUNCTION AddSubtreeToWorkspaces */


/*************************************<->*************************************
 *
 *  AddClientToWorkspaces (pCD, pIDs, numIDs)
 *
 *
 *  Description:
 *  -----------
 *  This function adds a transient tree to a list of workspaces
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 *  pIDs = pointer to a list of workspace IDs
 *  numIDs = number of workspace IDs
 * 
 *  Outputs:
 *  -------
 *
 *************************************<->***********************************/

void 
AddClientToWorkspaces(
        ClientData *pCD,
        WorkspaceID *pIDs,
        unsigned int numIDs )

{
    ClientData *pcdLeader;

    pcdLeader = (pCD->transientLeader) ? FindTransientTreeLeader (pCD) : pCD;

    AddSubtreeToWorkspaces (pcdLeader, pIDs, numIDs);

}   /* END OF FUNCTION AddClientToWorkspaces */



/*************************************<->*************************************
 *
 *  AddClientToWsList (pWS, pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function adds a client to a list of clients in a workspace
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 *  pWS = pointer to workspace data
 * 
 *  Outputs:
 *  -------
 *  none
 *
 *************************************<->***********************************/

void 
AddClientToWsList(
        WmWorkspaceData *pWS,
        ClientData *pCD )

{
    if (pWS->numClients >= pWS->sizeClientList)
    {
	if (pWS->sizeClientList == 0)
	{
	    pWS->ppClients = (ClientData **) 
		XtMalloc (WINDOW_ALLOC_AMOUNT * sizeof(ClientData *));
	}
	else
	{
	    pWS->ppClients = (ClientData **) 
		XtRealloc ((char *)pWS->ppClients, 
			 (pWS->sizeClientList + WINDOW_ALLOC_AMOUNT) * 
			 sizeof(ClientData *));
	}

	if (!pWS->ppClients)
	{
	    Warning (((char *)GETMESSAGE(76, 9, "Insufficient memory to add window to workspace")));
	    ExitWM(WM_ERROR_EXIT_VALUE);
	}

	pWS->sizeClientList += WINDOW_ALLOC_AMOUNT;
    }

    if (pWS->numClients < pWS->sizeClientList)
    {
	pWS->ppClients[pWS->numClients] = pCD;
	pWS->numClients++;
    }
}   /* END OF FUNCTION AddClientToWsList */


/*************************************<->*************************************
 *
 *  RemoveClientFromWsList (pWS, pCD)
 *
 *
 *  Description:
 *  -----------
 *  This function removes a client from a list of clients in a workspace
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 *  pWS = pointer to workspace data
 * 
 *  Outputs:
 *  -------
 *  none
 *
 *************************************<->***********************************/

void 
RemoveClientFromWsList(
        WmWorkspaceData *pWS,
        ClientData *pCD )

{
    int src, dest;

    for (dest = 0; dest < pWS->numClients; dest++)
    {
	if (pWS->ppClients[dest] == pCD)
	{
	    break;
	}
    }

    for (src = dest+1; src < pWS->numClients; src++, dest++)
    {
	pWS->ppClients[dest] = pWS->ppClients[src];
    }

    pWS->numClients--;

}   /* END OF FUNCTION RemoveClientFromWsList */


/*************************************<->*************************************
 *
 *  Boolean
 *  F_CreateWorkspace (args, pCD, event)
 *
 *  Description:
 *  -----------
 *
 *  Inputs:
 *  ------
 *  args = ...
 *  pCD = ...
 *  event = ...
 * 
 *  Outputs:
 *  -------
 *  Return = ...
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

Boolean 
F_CreateWorkspace(
        String args,
        ClientData *pCD,
        XEvent *event )

{
    WmScreenData *pSD = ACTIVE_PSD;

    if (pSD->numWorkspaces >= MAX_WORKSPACE_COUNT)
    {
	char buffer[MAXWMPATH];
	/*
	 * At the maximum number of allowed workspaces.
	 */
	sprintf (buffer, 
	((char *)GETMESSAGE(76, 14, "Maximum number of workspaces is %d. New workspace was not created.")), MAX_WORKSPACE_COUNT);
	Warning (buffer);
    }
    else
    {
	CreateWorkspace (ACTIVE_PSD, (unsigned char *)args);
    }

    return (TRUE);

} /* END OF FUNCTION F_CreateWorkspace */


/*************************************<->*************************************
 *
 *  Boolean
 *  F_DeleteWorkspace (args, pCD, event)
 *
 *  Description:
 *  -----------
 *
 *  Inputs:
 *  ------
 *  args = ...
 *  pCD = ...
 *  event = ...
 * 
 *  Outputs:
 *  -------
 *  Return = ...
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

Boolean 
F_DeleteWorkspace(
        String args,
        ClientData *pCD,
        XEvent *event )

{
    WmScreenData *pSD = ACTIVE_PSD;
    WmWorkspaceData *pWS = NULL;
    int i;

    if (args == NULL)
    {
	pWS= ACTIVE_WS;
    } 
    else
    {
	for (i=0; i<pSD->numWorkspaces; i++)
	{
	    if (!strcmp(pSD->pWS[i].name, args))
	    {
		pWS = &(pSD->pWS[i]);
		break;
	    }
	}
    }

    if (pWS)
	DeleteWorkspace (pWS);

    return (TRUE);

} /* END OF FUNCTION F_DeleteWorkspace */


/*************************************<->*************************************
 *
 *  Boolean
 *  F_GotoWorkspace (args, pCD, event)
 *
 *  Description:
 *  -----------
 *
 *  Inputs:
 *  ------
 *  args = ...
 *  pCD = ...
 *  event = ...
 * 
 *  Outputs:
 *  -------
 *  Return = ...
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

Boolean 
F_GotoWorkspace(
        String args,
        ClientData *pCD,
        XEvent *event )

{
    WorkspaceID wsID;
    WmWorkspaceData *pWS;

    wsID = XInternAtom (DISPLAY, args, False);
    pWS = GetWorkspaceData (ACTIVE_PSD, wsID);

    if (pWS)
    {
	ChangeToWorkspace (pWS);
    }
    return (TRUE);

} /* END OF FUNCTION F_GotoWorkspace */




/*************************************<->*************************************
 *
 *  Boolean
 *  F_AddToAllWorkspaces (args, pCD, event)
 *
 *
 *  Description:
 *  -----------
 *  Puts a client into all workspaces
 *
 *
 *  Inputs:
 *  ------
 *  args = ...
 *  pCD = pointer to client data
 *  event = ...
 *
 * 
 *  Outputs:
 *  -------
 *  Return = True
 *
 *
 *  Comments:
 *  --------
 *  The list of Ids returned has been privately allocated. Copy
 *  if you want to save or do anything with it.
 * 
 *************************************<->***********************************/

Boolean 
F_AddToAllWorkspaces(
        String args,
        ClientData *pCD,
        XEvent *event )

{
    WmScreenData *pSD;
    int i;

    if (pCD && (pCD->dtwmFunctions & DtWM_FUNCTION_OCCUPY_WS))
    {
	pSD = pCD->pSD;

	ReserveIdListSpace (pSD->numWorkspaces);

	for (i = 0; i < pSD->numWorkspaces; i++)
	{
	    pResIDs[i] = pSD->pWS[i].id;
	}

	AddClientToWorkspaces (pCD, pResIDs, pSD->numWorkspaces);

	pCD->putInAll = True;
    }

    return (True);

} /* END OF FUNCTION F_AddToAllWorkspaces */


/*************************************<->*************************************
 *
 *  Boolean
 *  F_Remove (args, pCD, event)
 *
 *
 *  Description:
 *  -----------
 *  Removes a client from the current workspace
 *
 *
 *  Inputs:
 *  ------
 *  args = ...
 *  pCD = pointer to client data
 *  event = ...
 *
 * 
 *  Outputs:
 *  -------
 *  Return = True
 *
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

Boolean 
F_Remove(
        String args,
        ClientData *pCD,
        XEvent *event )

{
    Boolean rval = False;

    /*
     * Only remove if in more than one workspace.
     */
    if ((pCD && (pCD->dtwmFunctions & DtWM_FUNCTION_OCCUPY_WS)) &&
	(pCD->numInhabited > 1))
    {
	if (ClientInWorkspace (ACTIVE_WS, pCD))
	{
	    RemoveClientFromWorkspaces (pCD, &ACTIVE_WS->id, 1);
            pCD->putInAll = False;
	}
    }

    return (rval);

} /* END OF FUNCTION F_Remove */



/*************************************<->*************************************
 *
 *  GetCurrentWorkspaceIndex (pSD)
 *
 *
 *  Description:
 *  -----------
 *  Returns an index into the screens array of workspace structures
 *  for the current workspace.
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *
 *
 *  Comments:
 *  --------
 *************************************<->***********************************/
int 
GetCurrentWorkspaceIndex(
        WmScreenData *pSD )
{

    int i;

    for (i = 0 ; i < pSD->numWorkspaces; i++)
    {
	if (pSD->pWS[i].id == pSD->pActiveWS->id)
	break;
    }

    if (i >= pSD->numWorkspaces)
    {
	/* failed to find workspace!!! How did that happen??? */
	i = 0;
#ifdef DEBUG
	Warning ("Failed to find workspace index");
#endif /* DEBUG */
    }

    return(i);
} /* END OF FUNCTION GetCurrentWorkspaceIndex */


/*************************************<->*************************************
 *
 *  void
 *  InsureIconForWorkspace (pWS, pCD)
 *
 *
 *  Description:
 *  -----------
 *  Makes sure an icon exists for the workspace
 *
 *
 *  Inputs:
 *  ------
 *
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

void 
InsureIconForWorkspace(
        WmWorkspaceData *pWS,
        ClientData *pCD )

{
    WsClientData *pWsc;

    if (pCD->clientFunctions & MWM_FUNC_MINIMIZE)
    {
	pWsc = GetWsClientData (pWS, pCD);

	if ((pCD->pSD->useIconBox) && 
	    (!(pCD->clientFlags & (CLIENT_WM_CLIENTS | FRONT_PANEL_BOX))))
	{
	    /*
	     * Create a new widget for the icon box
	     */
	    if (MakeIcon (pWS, pCD)) 
	    {
		XSaveContext (DISPLAY, pWsc->iconFrameWin, 
			wmGD.windowContextType, (caddr_t)pCD);

		if (pCD->iconWindow && pWsc->iconFrameWin)
		{
		    XGrabButton (DISPLAY, AnyButton, AnyModifier, 
			pWsc->iconFrameWin, True,
			ButtonPressMask|ButtonReleaseMask|
			    ButtonMotionMask,
			GrabModeAsync, GrabModeAsync, None, 
			wmGD.workspaceCursor);
		}

		ShowClientIconState (pCD, (pCD->clientState & ~UNSEEN_STATE));
	    }
	}
	else
	{
	    /* 
	     * Reuse existing icon in new workspaces. Suggest
	     * icon position in current WS as position of icon
	     * in new WS.
	     */
	    pWsc->iconFrameWin = pCD->pWsList[0].iconFrameWin;
	    pWsc->iconX = ICON_X(pCD); 
	    pWsc->iconY = ICON_Y(pCD);

	    if ((pCD->clientState & ~UNSEEN_STATE) != MINIMIZED_STATE)
	    {
		pWsc->iconPlace = NO_ICON_PLACE;
	    }
	    else if (!wmGD.iconAutoPlace)
	    {
		if (wmGD.positionIsFrame)
		{
		    pWsc->iconX -= pCD->clientOffset.x;
		    pWsc->iconY -= pCD->clientOffset.y;
		}
                PlaceIconOnScreen (pCD, &pWsc->iconX, &pWsc->iconY);
	    }
	    else	/* icon auto placement */
	    {
		pWsc->iconPlace = 
		CvtIconPositionToPlace (&pWS->IPData,
					pWsc->iconX, pWsc->iconY);
		if (pWS->IPData.placeList[pWsc->iconPlace].pCD)
		{
		    /* The spot is already occupied!  Find a 
		       spot nearby. */
		    pWsc->iconPlace = 
		    FindIconPlace (pCD, &pWS->IPData, pWsc->iconX,
				pWsc->iconY);

		    if (pWsc->iconPlace == NO_ICON_PLACE)
		    {
			/* Can't find a spot close by. Use the
			   next available slot */
			pWsc->iconPlace = GetNextIconPlace (&pWS->IPData);
			if (pWsc->iconPlace == NO_ICON_PLACE)
			{
			    pWsc->iconPlace =
				CvtIconPositionToPlace (&pWS->IPData,
                                                pCD->clientX,
                                                pCD->clientY);
			}
		    }
		}
		CvtIconPlaceToPosition (&pWS->IPData, pWsc->iconPlace, 
					&pWsc->iconX, &pWsc->iconY);

		
		if (!(pWS->IPData.placeList[pWsc->iconPlace].pCD))
		{
		    pWS->IPData.placeList[pWsc->iconPlace].pCD = pCD;
		}
	    }
	}
    }
} /* END OF FUNCTION InsureIconForWorkspace */


/*************************************<->*************************************
 *
 *  Boolean
 *  GetLeaderPresence (pCD, pIDs, pnumIDs)
 *
 *
 *  Description:
 *  -----------
 *  Gets the workspace presence of the transient tree leader for a
 *  client.
 *
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 *  ppIDs = pointer to pointer to list of workspace ids
 *  pnumIDs = pointer to number of workspace ids
 *
 * 
 *  Outputs:
 *  -------
 *  *ppIDS = list of workspace IDs
 *  *pnumIDs = number of workspace IDs in list
 *
 *  Return = true on success
 *
 *
 *  Comments:
 *  --------
 *  ID list is dynamically allocated, please XtFree() it when you're
 *  done.
 * 
 *************************************<->***********************************/

Boolean 
GetLeaderPresence(
        ClientData *pCD,
        WorkspaceID **ppIDs,
        unsigned int *pnumIDs )

{
    ClientData *pcdLeader;
    int i;
    Boolean rval = False;
    WorkspaceID *pLocalIDs;

    if ((pLocalIDs = (WorkspaceID *) XtMalloc (pCD->pSD->numWorkspaces *
	sizeof(WorkspaceID))) == NULL)
    {
	Warning (((char *)GETMESSAGE(76, 10, "Insufficient Memory (GetLeaderPresence)")));
	ExitWM (WM_ERROR_EXIT_VALUE);
    }

    /*
     * Make up list of workspaces for primary window
     */
    if (pCD->transientLeader)
    {
	pcdLeader = FindTransientTreeLeader (pCD);

	for (i = 0; i < pcdLeader->numInhabited; i++)
	{
	    pLocalIDs[i] = pcdLeader->pWsList[i].wsID;
	}

	*ppIDs = pLocalIDs;
	*pnumIDs = pcdLeader->numInhabited;
	rval = True;
    }

    return (rval);

} /* END OF FUNCTION GetLeaderPresence */


/*************************************<->*************************************
 *
 *  Boolean
 *  GetMyOwnPresence (pCD, pIDs, pnumIDs)
 *
 *
 *  Description:
 *  -----------
 *  Returns the current workspace presence for the client
 *
 *
 *  Inputs:
 *  ------
 *  pCD = pointer to client data
 *  ppIDs = pointer to pointer to list of workspace ids
 *  pnumIDs = pointer to number of workspace ids
 *
 * 
 *  Outputs:
 *  -------
 *  *ppIDS = list of workspace IDs
 *  *pnumIDs = number of workspace IDs in list
 *
 *  Return = true on success
 *
 *
 *  Comments:
 *  --------
 *  ID list is dynamically allocated (by DtWsmGetWorkspacesOccupied).
 *  Please XtFree() it when you're done.
 * 
 *************************************<->***********************************/

Boolean 
GetMyOwnPresence(
        ClientData *pCD,
        WorkspaceID **ppIDs,
        unsigned int *pnumIDs )

{
    Boolean rval = False;
    unsigned long nIDs = (unsigned long)*pnumIDs;

    /*
     * Get the workspace presence property 
     */
    if (
#ifdef HP_VUE
	(HasProperty (pCD, wmGD.xa_DT_WORKSPACE_PRESENCE) ||
	 HasProperty (pCD, 
		      XmInternAtom (DISPLAY, _XA_VUE_WORKSPACE_PRESENCE, 
		      False)))
#else /* HP_VUE */
	HasProperty (pCD, wmGD.xa_DT_WORKSPACE_PRESENCE) 
#endif /* HP_VUE */
	&& (DtWsmGetWorkspacesOccupied (DISPLAY, pCD->client, ppIDs,
				       &nIDs) == Success))
    {
	if (nIDs)
	{
	    rval = True;
	}
    }
    *pnumIDs = (unsigned int)nIDs;

    return (rval);

} /* END OF FUNCTION GetMyOwnPresence */



/*************************************<->*************************************
 *
 *  void
 *  ReserveIdListSpace (numIDs)
 *
 *
 *  Description:
 *  -----------
 *  Insures that there is enough room in our privately allocated
 *  list of workspace IDs
 *
 *
 *  Inputs:
 *  ------
 *  numIDs = number of workspace ids
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 *************************************<->***********************************/

void 
ReserveIdListSpace(
        int numIDs )

{
    if (numResIDs == 0)
    {
	pResIDs = (WorkspaceID *) 
		    XtMalloc (numIDs * sizeof (WorkspaceID));
	if (pResIDs)
	{
	    numResIDs = numIDs;
	}
    }
    else if (numResIDs < numIDs)
    {
	pResIDs = (WorkspaceID *) XtRealloc ((char *)pResIDs, 
					numIDs * sizeof (WorkspaceID));

	numResIDs = (pResIDs)? numIDs : 0;
    }

    if (pResIDs == NULL)
    {
	Warning (((char *)GETMESSAGE(76, 11, "Insufficient memory")));
	ExitWM (WM_ERROR_EXIT_VALUE);
    }

} /* END OF FUNCTION ReserveIdListSpace */




/******************************<->*************************************
 *
 *  SaveResources (pSD)
 *
 *  Description:
 *  -----------
 *  Saves dtwm resources to restore session
 *
 *  Inputs:
 *  ------
 *  pSD = pointer to screen data
 *
 *  Outputs:
 *  -------
 *  None
 *
 *  Comments:
 *  ---------
 *
 *************************************<->***********************************/
void
SaveResources( WmScreenData *pSD)
{
    int wsCnt;
#ifdef PANELIST
    WmPanelistObject  pPanelist;
#endif /* PANELIST */

    if(pSD)
    {
	if (pSD->pActiveWS)
	{
	    SaveWorkspaceResources(pSD->pActiveWS, 
				   (WM_RES_INITIAL_WORKSPACE |
				    WM_RES_WORKSPACE_COUNT));
	}

#ifdef PANELIST
	pPanelist = (WmPanelistObject) pSD->wPanelist;
	if (pPanelist && O_Shell(pPanelist))
	{
	    /* This is the front panel for the screen */
	    SaveWorkspaceResources(pSD->pActiveWS, 
				   WM_RES_FP_POSITION);


            /*  Call the fronto panel function to save its resources  */
	    
	    WmFrontPanelSessionSaveData();
	}
#endif /*  PANELIST */



	for (wsCnt = 0; wsCnt < pSD->numWorkspaces; wsCnt++)
	{
	    if(pSD->useIconBox)
	    {
		SaveWorkspaceResources(&pSD->pWS[wsCnt], 
				       WM_RES_ICONBOX_GEOMETRY);
	    }
	} /* for wsCnt */

	SaveHelpResources(pSD);

    } /* if pSD */

} /* END OF FUNCTION SaveResources */


/******************************<->*************************************
 *
 *  SaveWorkspaceResource (pWS, flags)
 *
 *  Description:
 *  -----------
 *  Modifies the RESOURCE_MANAGER property to add update versions
 *  of the requested resources.
 *
 *  Inputs:
 *  ------
 *  pWS = pointer to workspace data
 *  
 *  Outputs:
 *  -------
 *  None
 *
 *  Comments:
 *  ---------
 *
 *************************************<->***********************************/
void 
SaveWorkspaceResources(
        WmWorkspaceData *pWS,
        unsigned long flags)
{
    char *buffer = NULL;
    int bufferLength = 0;
    char *res_class;
    char *data;
    int cum_len;

    char screenName[1024];
    char tmpScreenName[10];

    Position clientX;
    Position clientY;
    Dimension clientWidth;
    Dimension clientHeight;
    int xoff, yoff;
#ifdef PANELIST
    WmPanelistObject  pPanelist = (WmPanelistObject) pWS->pSD->wPanelist;
    ClientData *pCD_Panel ;
    char tmpBuffer[MAXWMPATH+1];
#endif /* PANELIST */
    int iLen;

    /* allocate initial data space */
    if ((data = (char *) XtMalloc (MAXWMPATH+1)) == NULL)
    {
	Warning (((char *)
		  GETMESSAGE(76,12,"Insufficient memory to save resources")));
	Do_Quit_Mwm (False);
    }
    cum_len = 1;
    *data = '\0';

    if (bufferLength == 0)
    {
	buffer = (char *) XtMalloc (MAXWMPATH+1);
	bufferLength = MAXWMPATH;
    }
    *buffer = '\0';

    /* Get our current resource class */

    if (MwmBehavior)
    {
	res_class = WM_RESOURCE_CLASS;
    }
    else 
    {
	res_class = DT_WM_RESOURCE_CLASS;
    }

    strcpy(screenName, "*");
    strcat(screenName,XtName (pWS->pSD->screenTopLevelW)); 

    /* construct and write out the resources specification */

    if (flags & WM_RES_BACKDROP_IMAGE)
    {
        iLen = (strlen (res_class) + strlen (screenName) +
	     strlen (pWS->name) + strlen (WmNbackdrop) +
	     strlen (WmNimage) + strlen (pWS->backdrop.image) + 20);
	    
	if (iLen > bufferLength)
	{
	    bufferLength += iLen;
	    buffer = (char *) 
		XtRealloc (buffer, bufferLength * sizeof(char));
	}

	sprintf (buffer, "%s%s*%s*%s*%s:  %s\n", res_class,
		screenName, pWS->name, 
		WmNbackdrop, WmNimage, pWS->backdrop.image);

	AddStringToResourceData (buffer, &data, &cum_len);
    }


    if (flags & WM_RES_WORKSPACE_TITLE)
    {
	String asciiName;

	asciiName = WmXmStringToString (pWS->title);

	iLen = strlen (res_class) + strlen (screenName) +
	       strlen (pWS->name) + strlen (WmNtitle) +
	       strlen (asciiName) + 16;

	if (iLen > bufferLength)
	{
	    bufferLength += iLen;
	    buffer = (char *) 
		XtRealloc (buffer, bufferLength * sizeof(char));
	}

	sprintf (buffer, "%s%s*%s*%s:  %s\n", res_class,
		screenName, pWS->name, 
		WmNtitle, asciiName);

	AddStringToResourceData (buffer, &data, &cum_len);

	XtFree (asciiName);
    }

    if ((flags & WM_RES_INITIAL_WORKSPACE) &&
        (!wmGD.useStandardBehavior))
    {
	iLen = strlen (res_class) + strlen (screenName) +
	       strlen (WmNinitialWorkspace) + strlen (pWS->name) + 14;

	if (iLen > bufferLength)
	{
	    bufferLength += iLen;
	    buffer = (char *) 
		XtRealloc (buffer, bufferLength * sizeof(char));
	}

	sprintf (buffer, "%s%s*%s:  %s\n", res_class,
		screenName, 
		WmNinitialWorkspace, pWS->name);

	AddStringToResourceData (buffer, &data, &cum_len);
    }

    if ((flags & WM_RES_WORKSPACE_LIST) &&
        (!wmGD.useStandardBehavior))
    {
	WmWorkspaceData *pWSi;
	char *pchQname;
	int i;

	pWSi = pWS->pSD->pWS;

	pchQname = (char *) _DtWmParseMakeQuotedString (
					(unsigned char *)pWSi->name);
	strcpy ((char *)wmGD.tmpBuffer, pchQname);
	XtFree (pchQname);
	pWSi++;


	for (i=1; i<pWS->pSD->numWorkspaces; i++, pWSi++)
	{
	    strcat ((char *)wmGD.tmpBuffer, " ");
	    pchQname = (char *) _DtWmParseMakeQuotedString (
					(unsigned char *)pWSi->name);
	    strcat ((char *)wmGD.tmpBuffer, pchQname);
	    XtFree (pchQname);
	}

	sprintf (buffer, "%s%s*%s:  %s\n", res_class,
		screenName, 
		WmNworkspaceList, wmGD.tmpBuffer);

	AddStringToResourceData (buffer, &data, &cum_len);
    }

    if ((flags & WM_RES_WORKSPACE_COUNT) &&
        (!wmGD.useStandardBehavior))
    {
	char pchNumWs[20];

	sprintf (pchNumWs, "%d", pWS->pSD->numWorkspaces);

	iLen = strlen (res_class) + strlen (screenName) +
	       strlen (WmNworkspaceCount) + strlen (pchNumWs) + 14;

	if (iLen > bufferLength)
	{
	    bufferLength += iLen;
	    buffer = (char *) 
		XtRealloc (buffer, bufferLength * sizeof(char));
	}

	sprintf (buffer, "%s%s*%s:  %s\n", res_class,
		screenName, 
		WmNworkspaceCount, pchNumWs);

	AddStringToResourceData (buffer, &data, &cum_len);
    }

#ifdef PANELIST
    if ((flags & WM_RES_FP_POSITION) &&
	(O_Shell(pPanelist)) && 
        (!wmGD.useStandardBehavior) &&
	(!XFindContext (DISPLAY, XtWindow(O_Shell(pPanelist)),
					  wmGD.windowContextType, 
					  (XtPointer)&pCD_Panel)))
    {
	Position midX, midY, tmpX, tmpY;
	Dimension screenWidth, screenHeight;

	clientX = pCD_Panel->clientX;
	clientY = pCD_Panel->clientY;

	/*
	 *  Determine quadrant that the front panel midpoint is
	 *  in and save front panel with appropriate gravity.
	 */

	/* find panel midpoint */

	midX = clientX+(pCD_Panel->clientWidth >> 1);
	midY = clientY+(pCD_Panel->clientHeight >> 1);

	/* get screen dimensions */

	screenWidth = XDisplayWidth (DISPLAY, pCD_Panel->pSD->screen);
	screenHeight = XDisplayHeight (DISPLAY, pCD_Panel->pSD->screen);

	/*
	 * Determine midpoint quadrant and set up client geometry
	 * relative to that corner. Adjust if positionIsFrame 
	 * is being used.
	 */
	if (midX <= (Position) screenWidth/2)
	{
	    if(wmGD.positionIsFrame)
	    {
		clientX -= pCD_Panel->frameInfo.upperBorderWidth;
	    }

	    /* West */
	    if (midY <= (Position) screenHeight/2)
	    {
		/* NorthWest */
		if(wmGD.positionIsFrame)
		{
		    clientY -= (pCD_Panel->frameInfo.upperBorderWidth +
				pCD_Panel->frameInfo.titleBarHeight);
		}
		sprintf (tmpBuffer, "+%d+%d\0", clientX, clientY);
	    }
	    else
	    {
		/* SouthWest */
		clientY = screenHeight - clientY - pCD_Panel->clientHeight;
		if(wmGD.positionIsFrame)
		{
		    clientY -= pCD_Panel->frameInfo.lowerBorderWidth;
		}

		sprintf (tmpBuffer, "+%d-%d\0", clientX, clientY);
	    }
	}
	else
	{
	    clientX = screenWidth - clientX - pCD_Panel->clientWidth;
	    if (wmGD.positionIsFrame)
	    {
		clientX -= pCD_Panel->frameInfo.lowerBorderWidth;
	    }

	    /* East */
	    if (midY <= (Position) screenHeight/2)
	    {
		/* NorthEast */
		if(wmGD.positionIsFrame)
		{
		    clientY -= (pCD_Panel->frameInfo.upperBorderWidth +
				pCD_Panel->frameInfo.titleBarHeight);
		}
		sprintf (tmpBuffer, "-%d+%d\0", clientX, clientY);
	    }
	    else
	    {
		/* SouthEast */
		clientY = screenHeight - clientY - pCD_Panel->clientHeight;
		if(wmGD.positionIsFrame)
		{
		    clientY -= pCD_Panel->frameInfo.lowerBorderWidth;
		}
		sprintf (tmpBuffer, "-%d-%d\0", clientX, clientY);
	    }
	}

	iLen = strlen (res_class) + strlen (screenName) +
	       strlen (XtName(O_Shell(pPanelist))) + 
	       strlen (WmNgeometry) + strlen (tmpBuffer) + 18;

	if (iLen > bufferLength)
	{
	    bufferLength += iLen;
	    buffer = (char *) 
		XtRealloc (buffer, bufferLength * sizeof(char));
	}

	sprintf (buffer, "%s%s*%s*%s:  %s\n", res_class,
		 screenName, 
		 XtName (O_Shell(pPanelist)), 
		 WmNgeometry, tmpBuffer);

	AddStringToResourceData (buffer, &data, &cum_len);
    }
#endif /* PANELIST */

    if ((flags & WM_RES_ICONBOX_GEOMETRY) &&
        (!wmGD.useStandardBehavior))
    {
	/* update iconbox geometry string */

	if (pWS->iconBoxGeometry)
	{
	    XtFree((char *) (pWS->iconBoxGeometry));
	    pWS->iconBoxGeometry = NULL;
	}

	clientWidth = (pWS->pIconBox->pCD_iconBox->clientWidth - 
		       pWS->pIconBox->pCD_iconBox->baseWidth) /
			   pWS->pIconBox->pCD_iconBox->widthInc;

	clientHeight = (pWS->pIconBox->pCD_iconBox->clientHeight - 
			pWS->pIconBox->pCD_iconBox->baseHeight) /
			    pWS->pIconBox->pCD_iconBox->heightInc ;

	if(wmGD.positionIsFrame)
	{
	    CalculateGravityOffset (pWS->pIconBox->pCD_iconBox, &xoff, &yoff);
	    clientX = pWS->pIconBox->pCD_iconBox->clientX - xoff;
	    clientY = pWS->pIconBox->pCD_iconBox->clientY - yoff;
	}
	else
	{
	    clientX = pWS->pIconBox->pCD_iconBox->clientX;
	    clientY = pWS->pIconBox->pCD_iconBox->clientY;
	}

	sprintf (buffer, "%dx%d+%d+%d\0", clientWidth, clientHeight,
		 clientX, clientY);

	pWS->iconBoxGeometry  = strdup( buffer);

	iLen = strlen (res_class) + strlen (screenName) +
	       strlen (pWS->name) + strlen (WmNiconBoxGeometry) + 
	       strlen (pWS->iconBoxGeometry) + 18;

	if (iLen > bufferLength)
	{
	    bufferLength += iLen;
	    buffer = (char *) 
		XtRealloc (buffer, bufferLength * sizeof(char));
	}

	sprintf (buffer, "%s%s*%s*%s:  %s\n", res_class,
		screenName, pWS->name, 
		WmNiconBoxGeometry, pWS->iconBoxGeometry);

	AddStringToResourceData (buffer, &data, &cum_len);
    }


   if (data)
   {
	/*
	 * Merge in the resource(s)
	 */
	_DtAddToResource (DISPLAY, data);
       XtFree(data);
   }

   if (buffer)
   {
       XtFree(buffer);
   }

} /* END OF FUNCTION SaveWorkspaceResources */


/******************************<->*************************************
 *
 *  AddStringToResourceData (string, pdata, plen)
 *
 *  Description:
 *  -----------
 *  Adds a string to a growing buffer of strings. 
 *
 *  Inputs:
 *  ------
 *  string - string to add
 *  pdata - pointer to data pointer 
 *  plen - number of bytes used in *pdata already 
 * 
 *  Outputs:
 *  -------
 *  *pdata - data pointer  (may be changed by XtRealloc)
 *  *plen - number of bytes used in *pdata  (old value plus length 
 *          of string
 *
 *  Comments:
 *  ---------
 *
 *************************************<->***********************************/
void 
AddStringToResourceData(
        char *string,
        char **pdata,
        int *plen )
{
    if ((*pdata = (char *) XtRealloc(*pdata, *plen+strlen(string)+1)) == NULL)
    {
	Warning (((char *)GETMESSAGE(76, 13, "Insufficient memory to save resources.")));
	Do_Quit_Mwm (False);
    }

    strcat (*pdata, string);
    *plen += strlen(string);
} /* END OF FUNCTION AddStringToResourceData */


/*************************************<->*************************************
 *
 *  DuplicateWorkspaceName (pSD, name, num)
 *
 *
 *  Description:
 *  -----------
 *  This function searches the first "num" workspace names to see if the
 *  passed "name" duplicates any workspace name defined so far.
 *
 *
 *  Inputs:
 *  ------
 *  pSD = pointer to screen data
 *  name = potential string name for workspace
 *  num = number of workspaces to check against
 * 
 *  Outputs:
 *  -------
 *  Return = True if a dupicate was found
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/
Boolean
DuplicateWorkspaceName (WmScreenData *pSD, unsigned char *name, int num)
{
    int i;
    Boolean duplicate = False;

    if (pSD && pSD->pWS)
    {
	for (i = 0; (i < num) && !duplicate; i++)
	{
	    if (!strcmp (pSD->pWS[i].name, (char *)name))
	    {
		duplicate = True;
	    }
	}
    }

    return (duplicate);
}

#ifdef DEBUG
int PrintWorkspaceList (pSD)
    WmScreenData *pSD;
{
    int i, j, k;
    WmWorkspaceData *pWS;
    ClientData *pCD;
    ClientData *pClients[500];
    int numSaved = 0;
    Boolean Saved;

    fprintf (stderr, "Screen: %d\n", pSD->screen);

    for (i =0; i < pSD->numWorkspaces; i++)
    {
	pWS = &pSD->pWS[i];

	fprintf (stderr, "\nWorkspace %s contains: \n", pWS->name);

	for (j = 0; j < pWS->numClients; j++)
	{
	    pCD = pWS->ppClients[j];
	    fprintf (stderr, "\t%s\n", pCD->clientName);

	    Saved = False;
	    for (k = 0; k < numSaved; k++)
	    {
		if (pCD == pClients[k]) 
		{
		    Saved = True;
		    break;
		}
	    }

	    if (!Saved)
	    {
		pClients[numSaved++] = pCD;
	    }
	}
    }

    for (i = 0; i < numSaved; i++)
    {
	pCD = pClients[i];
	fprintf (stderr, "\nClient %s is in: \n", pCD->clientName);
	for (j = 0; j < pCD->numInhabited; j++)
	{
	    pWS = GetWorkspaceData (pCD->pSD, pCD->pWsList[j].wsID);
	    fprintf (stderr, "\t%s\n", pWS->name);
	}

    }
} /* END OF FUNCTION PrintWorkspaceList */
#endif /* DEBUG */

/* DO NOT ADD ANYTHING AFTER THE FOLLOWING #ENDIF !!! */
#endif /* WSM */
