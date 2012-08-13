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
#ifndef NO_OL_COMPAT
/* 
 * (c) Copyright 1989 Sun Microsystems, Inc.
 * (c) Copyright 1993 HEWLETT-PACKARD COMPANY 
 * ALL RIGHTS RESERVED 
 */ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: WmOL.c /main/4 1995/11/01 11:45:08 rswiston $"
#endif
#endif

/*
 * Included Files:
 */
#include "WmGlobal.h"
#include "WmOL.h"
#include "WmProperty.h"
#include <Xm/AtomMgr.h>

#define ValidPropertyList(pcd) ((pcd)->paInitialProperties != NULL)



/*************************************<->*************************************
 *
 *  InitOLCompat ()
 *
 *  Description:
 *  -----------
 *  Interns the atoms necessary for OL protocols.
 *
 *  Inputs:
 *  ------
 *
 *  Outputs:
 *  --------
 * 
 *************************************<->***********************************/
void 
InitOLCompat(void)
{
    wmGD.xa_OL_WIN_ATTR =
	    XmInternAtom (DISPLAY, OL_WIN_ATTR, False);
    wmGD.xa_OL_DECOR_RESIZE =
	    XmInternAtom (DISPLAY, OL_DECOR_RESIZE, False);
    wmGD.xa_OL_DECOR_HEADER =
	    XmInternAtom (DISPLAY, OL_DECOR_HEADER, False);
    wmGD.xa_OL_DECOR_CLOSE =
	    XmInternAtom (DISPLAY, OL_DECOR_CLOSE, False);
    wmGD.xa_OL_DECOR_PIN =
	    XmInternAtom (DISPLAY, OL_DECOR_PIN, False);
    wmGD.xa_OL_DECOR_ADD =
	    XmInternAtom (DISPLAY, OL_DECOR_ADD, False);
    wmGD.xa_OL_DECOR_DEL =
	    XmInternAtom (DISPLAY, OL_DECOR_DEL, False);
    wmGD.xa_OL_WT_BASE =
	    XmInternAtom (DISPLAY, OL_WT_BASE, False);
    wmGD.xa_OL_WT_COMMAND =
	    XmInternAtom (DISPLAY, OL_WT_CMD, False);
    wmGD.xa_OL_WT_HELP =
	    XmInternAtom (DISPLAY, OL_WT_HELP, False);
    wmGD.xa_OL_WT_NOTICE =
	    XmInternAtom (DISPLAY, OL_WT_NOTICE, False);
    wmGD.xa_OL_WT_OTHER =
	    XmInternAtom (DISPLAY, OL_WT_OTHER, False);
    wmGD.xa_OL_PIN_IN =
	    XmInternAtom (DISPLAY, OL_PIN_IN, False);
    wmGD.xa_OL_PIN_OUT =
	    XmInternAtom (DISPLAY, OL_PIN_OUT, False);
    wmGD.xa_OL_MENU_LIMITED =
	    XmInternAtom (DISPLAY, OL_MENU_LIMITED, False);
    wmGD.xa_OL_MENU_FULL =
	    XmInternAtom (DISPLAY, OL_MENU_FULL, False);

} /* END OF FUNCTION InitOLCompat */



/*************************************<->*************************************
 *
 *  HasOpenLookHints (pCD)
 *
 *  Description:
 *  -----------
 *  Returns True if this client has OpenLook hints on it.  
 *
 *  Inputs:
 *  ------
 *  pCD  =  pointer to client data
 *
 *  Outputs:
 *  --------
 *  Returns True if client has the _OL_WIN_ATTR property on it.
 * 
 *************************************<->***********************************/
Boolean 
HasOpenLookHints(
        ClientData *pCD )
{
    Boolean rval = False;
    OLWinAttr *property = NULL;

    if (ValidPropertyList (pCD) &&
	HasProperty(pCD, wmGD.xa_OL_WIN_ATTR))
    {
	rval = True;
    }
    else if ((property=GetOLWinAttr (pCD)) != NULL)
    {
	XFree ((char *) property);
	rval = True;
    }

    return (rval);

} /* END OF FUNCTION HasOpenLookHints */



/*************************************<->*************************************
 *
 *  GetOLWinAttr (pCD)
 *
 *  Description:
 *  -----------
 *  Fetches the OLWinAttr property off of the client
 *
 *  Inputs:
 *  ------
 *  pCD  =  pointer to client data
 *
 *  Outputs:
 *  --------
 *  Returns a pointer to the OLWinAttr property if found and valid.
 *  (Returned data should be freed with XFree())
 *  Returns NULL pointer otherwise.
 * 
 *************************************<->***********************************/
OLWinAttr * 
GetOLWinAttr(
        ClientData *pCD )
{
    Boolean rval = False;
    OLWinAttr *property = NULL;
    OLWinAttr *prop_new;
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long leftover;
    int ret_val;

    ret_val = XGetWindowProperty (DISPLAY, pCD->client, wmGD.xa_OL_WIN_ATTR, 
		  0L, ENTIRE_CONTENTS,
		  False, wmGD.xa_OL_WIN_ATTR, 
		  &actual_type, &actual_format, 
		  &nitems, &leftover, (unsigned char **)&property);

    if (ret_val != Success)
    {
	    property = NULL;
    }
    else if ((actual_format != 32) || 
	(actual_type != wmGD.xa_OL_WIN_ATTR)) 
    {
	    if (property) 
		XFree((char *)property);
	    property = NULL;
    }

    if (property && (nitems == OLDOLWINATTRLENGTH))
    {
	/* Old size, convert to new size */
	/* 
	 * !!! Should use XAlloc() here, but Xlib doesn't
	 *     define an inverse function of XFree(). !!!
	 */
	prop_new = (OLWinAttr *) malloc (sizeof(OLWinAttr));

	prop_new->flags = WA_WINTYPE | WA_MENUTYPE | WA_PINSTATE;
	prop_new->win_type = ((old_OLWinAttr *)property)->win_type;
	prop_new->menu_type = ((old_OLWinAttr *)property)->menu_type;
	prop_new->pin_initial_state = 
			((old_OLWinAttr *)property)->pin_initial_state;

	XFree ((char *) property);
	property = prop_new;
    }

    /* convert pin state for old clients */
    if (property && (property->flags & WA_PINSTATE))
    {
	if (property->pin_initial_state == wmGD.xa_OL_PIN_IN)
	{
	    property->pin_initial_state = PIN_IN;
	}
	else if (property->pin_initial_state == wmGD.xa_OL_PIN_OUT)
	{
	    property->pin_initial_state = PIN_OUT;
	}
    }

    return (property);

} /* END OF FUNCTION GetOLWinAttr */


/*************************************<->*************************************
 *
 *  GetOLDecorFlags (pCD, property, pDecor)
 *
 *  Description:
 *  -----------
 *  Fetches the _OL_DECOR_ADD or _OL_DECOR_DEL property off of the 
 *  client and returns OL flavored decor flags.
 *
 *  Inputs:
 *  ------
 *  pCD  =  pointer to client data
 *  property = property to fetch
 *  pDecor = pointer to OL decor flags word
 *
 *  Outputs:
 *  --------
 *  Return =  True if property found, False otherwise
 *  *pDecor = OL decor flags if valid property found, 
 *	      undefined if property not found.
 * 
 *************************************<->***********************************/
Boolean
GetOLDecorFlags(
        ClientData *pCD,
	Atom property,
	unsigned long *pDecor)
{
    int status, i;
    Boolean rval;
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long leftover;
    Atom *pAtoms = NULL;

    status = XGetWindowProperty (DISPLAY, pCD->client, property,
		  0L, ENTIRE_CONTENTS,
		  False, XA_ATOM, 
		  &actual_type, &actual_format, 
		  &nitems, &leftover, (unsigned char **)&pAtoms);

    if ((status != Success) || 
	!pAtoms || 
	(nitems == 0) ||
	(actual_type != XA_ATOM) ||
	(actual_format != 32)) 
    {
	    if (pAtoms)
		    XFree((char *)pAtoms);
	    rval = False;
    }
    else
    {
	*pDecor = 0;

	/*
	 * We only look for the ones we might be interested in.
	 * Several OL decoration types are ignored.
	 */
	for (i = 0; i < nitems; i++) {
		if (pAtoms[i] == wmGD.xa_OL_DECOR_RESIZE)
			*pDecor |= OLDecorResizeable;
		else if (pAtoms[i] == wmGD.xa_OL_DECOR_HEADER)
			*pDecor |= OLDecorHeader;
		else if (pAtoms[i] == wmGD.xa_OL_DECOR_CLOSE)
			*pDecor |= OLDecorCloseButton;
		else if (pAtoms[i] == wmGD.xa_OL_DECOR_PIN)
			*pDecor |= OLDecorPushPin;
	}

	XFree((char *)pAtoms);
	rval = True;

    }
    return (rval);

} /* END OF FUNCTION GetOLDecorFlags */



/*************************************<->*************************************
 *
 *  ProcessOLDecoration (pCD)
 *
 *  Description:
 *  -----------
 *
 *  Inputs:
 *  ------
 *  pCD  =  pointer to client data
 *
 *  Outputs:
 *  --------
 *  pCD = possibly modified with new decoration info
 * 
 *************************************<->***********************************/
void
ProcessOLDecoration(
        ClientData *pCD)
{
    OLWinAttr * pOLWinAttr;
    unsigned long OLdecor;
    long decorMask;

    if (HasOpenLookHints (pCD) && 
	((pOLWinAttr = GetOLWinAttr (pCD)) != NULL))
    {
	/*
	 * This window already has some decoration applied to
	 * it based on its ICCCM type (transient or not).
	 * Use the OL hints to construct a mask to further 
	 * modify these decorations.
	 */
	if ((pOLWinAttr->flags & WA_WINTYPE) == 0) 
        {
	    /*
	     * Window type not specified, assume all decorations
	     */
	    decorMask = WM_DECOR_ALL;
	} 
	else if (pOLWinAttr->win_type == wmGD.xa_OL_WT_BASE)
	{
	    /* 
	     * Base windows can have all decorations
	     */
	    decorMask = WM_DECOR_ALL;
	} 
	else if (pOLWinAttr->win_type == wmGD.xa_OL_WT_COMMAND) 
	{
	    /*
	     * Command windows have titles, pins (close button), and
	     * resize handles.
	     */
	    decorMask = WM_DECOR_TITLE | WM_DECOR_SYSTEM | WM_DECOR_RESIZEH;
	}
	else if (pOLWinAttr->win_type == wmGD.xa_OL_WT_HELP) 
	{
	    /*
	     * Help windows have titles and pins (close button).
	     * No resize, but give it a border to look nicer.
	     */
	    decorMask = (WM_DECOR_TITLE | WM_DECOR_SYSTEM | WM_DECOR_BORDER);
	}
	else if ((pOLWinAttr->win_type == wmGD.xa_OL_WT_NOTICE)  &&
		 (pOLWinAttr->win_type == wmGD.xa_OL_WT_OTHER))
	{
	    decorMask = WM_DECOR_NONE;
	} 
	else
	{
	    decorMask = WM_DECOR_ALL;
	}

	if (GetOLDecorAdd(pCD,&OLdecor))
	{
	    if (OLdecor & OLDecorResizeable)
	    {
		decorMask |= WM_DECOR_RESIZEH;
	    }
	    if (OLdecor & OLDecorHeader)
	    {
		decorMask |= WM_DECOR_TITLE;
	    }
	    if (OLdecor & OLDecorCloseButton)
	    {
		/* OL "close" is same as "Motif" minimize */
		decorMask |= MWM_DECOR_MINIMIZE;
	    }
	    if (OLdecor & OLDecorPushPin)
	    {
		/* 
		 * windows with pins can't be minimized 
		 */
		decorMask &= ~MWM_DECOR_MINIMIZE;
		decorMask |= MWM_DECOR_TITLE | MWM_DECOR_MENU;
	    }
	}

	if (GetOLDecorDel(pCD,&OLdecor))
	{
	    if (OLdecor & OLDecorResizeable)
	    {
		decorMask &= ~MWM_DECOR_RESIZEH;
	    }
	    if (OLdecor & OLDecorHeader)
	    {
		decorMask &= ~WM_DECOR_TITLEBAR;
	    }
	    if (OLdecor & OLDecorCloseButton)
	    {
		/* OL "close" is same as "Motif" minimize */
		decorMask &= ~MWM_DECOR_MINIMIZE;
	    }

	    /* push pin is ignored here */

	}

	/*
	 * If the window has a push pin or a limited menu,
	 * then consider it very similar to a secondary window.
	 */
	if (((pOLWinAttr->flags & WA_PINSTATE) &&
	     (pOLWinAttr->pin_initial_state == PIN_IN)) ||
	    ((pOLWinAttr->flags & WA_MENUTYPE) &&
	     (pOLWinAttr->menu_type == wmGD.xa_OL_MENU_LIMITED)))
	{
	    decorMask &= ~(MWM_DECOR_MINIMIZE | MWM_DECOR_MAXIMIZE);
	    pCD->bPseudoTransient = True;
	    pCD->dtwmFunctions &= ~DtWM_FUNCTION_OCCUPY_WS;
	}

	/* 
	 * Reduce decoration on this window according to OL hints
	 */
	pCD->clientDecoration &= decorMask;

	/*
	 * Reduce client functions if necessary.
	 */
	if (!(decorMask & MWM_DECOR_MINIMIZE))
	{
	    pCD->clientFunctions &= ~MWM_FUNC_MINIMIZE;
	}
	if (!(decorMask & MWM_DECOR_MAXIMIZE))
	{
	    pCD->clientFunctions &= ~MWM_FUNC_MAXIMIZE;
	}
	if (!(decorMask & MWM_DECOR_RESIZEH))
	{
	    pCD->clientFunctions &= ~MWM_FUNC_RESIZE;
	}

        /* 
	 * Set the clients secondariesOnTop value to false to allow
	 * open look transient behaviour (transients below primary).
	 */
        pCD->secondariesOnTop = False;
	
	if (pOLWinAttr)
	    XFree((char *)pOLWinAttr);
    }

} /* END OF FUNCTION ProcessOLDecoration */

#endif /* NO_OL_COMPAT */
