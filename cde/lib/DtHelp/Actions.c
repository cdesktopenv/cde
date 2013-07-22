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
/* $XConsortium: Actions.c /main/8 1996/10/30 10:03:40 pascale $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Actions.c
 **
 **   Project:     Display Area Library
 **
 **   Description: This body of code handles the actions for the
 **                Display Area.
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

/*
 * system includes
 */
#include <stdlib.h>
#include <Xm/Xm.h>

/*
 * CanvasEngine
 */
#include "CanvasP.h"

/*
 * private includes
 */
#include "DisplayAreaP.h"
#include "DisplayAreaI.h"
#include "ActionsI.h"
#include "CallbacksI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
/********    End Private Function Declarations    ********/

/********    End Private Defines             ********/

/********    Private Variable Declarations    ********/

/********    End Private Variable Declarations    ********/

/******************************************************************************
 *                             Private Functions
 ******************************************************************************/
/******************************************************************************
 *                          Semi Public Functions
 *****************************************************************************/
/*****************************************************************************
 * Function: _DtHelpCopyAction
 *
 *    _DtHelpCopyAction - Copy the current info to the clipboard
 *
 *****************************************************************************/
void
_DtHelpCopyAction (
    Widget          widget,
    XEvent          *event,
    String          *params,
    Cardinal        *num_params)
{
    Arg    args[2];
    XtPointer userData;

    XtSetArg(args[0], XmNuserData, &userData);
    XtGetValues(widget, args, 1);

    _DtHelpInitiateClipboard(userData);;

}  /* End _DtHelpCopyAction */

/*****************************************************************************
 * Function: _DtHelpDeSelectAll
 *
 *    _DtHelpDeSelectAll - Deselects the information in the widget.
 *
 *****************************************************************************/
void
_DtHelpDeSelectAll (
    Widget          widget,
    XEvent          *event,
    String          *params,
    Cardinal        *num_params)
{
    Arg    args[2];
    XtPointer userData;

    XtSetArg(args[0], XmNuserData, &userData);
    XtGetValues(widget, args, 1);

    if (userData != NULL)
        _DtHelpClearSelection (userData);

}  /* End _DtHelpDeSelectAll */

/*****************************************************************************
 * Function: _DtHelpSelectAll
 *
 *    _DtHelpSelectAll - Selects all the information in the widget.
 *
 *****************************************************************************/
void
_DtHelpSelectAll (
    Widget          widget,
    XEvent          *event,
    String          *params,
    Cardinal        *num_params)
{
    Arg    args[2];
    XtPointer userData;

    XtSetArg(args[0], XmNuserData, &userData);
    XtGetValues(widget, args, 1);

    if (userData != NULL)
      {
        DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) userData;

	_DtCanvasMoveTraversal(pDAS->canvas, _DtCvTRAVERSAL_OFF, False, True,
						NULL, NULL, NULL, NULL, NULL);
	_DtHelpGetClearSelection (widget, userData);
	_DtCanvasProcessSelection(pDAS->canvas, 0, 0, _DtCvSELECTION_START);
	_DtCanvasProcessSelection(pDAS->canvas, pDAS->dispUseWidth,
					pDAS->maxYpos, _DtCvSELECTION_END);
	_DtCanvasMoveTraversal(pDAS->canvas, _DtCvTRAVERSAL_ON, False, True,
						NULL, NULL, NULL, NULL, NULL);
	pDAS->text_selected = True;
      }

}  /* End _DtHelpSelectAll */

/*****************************************************************************
 * Function: _DtHelpActivateLink
 *
 *    _DtHelpSelectAll - Selects all the information in the widget.
 *
 *****************************************************************************/
void
_DtHelpActivateLink (
    Widget          widget,
    XEvent          *event,
    String          *params,
    Cardinal        *num_params)
{
    Arg    args[2];
    DtHelpDispAreaStruct *pDAS;
    _DtCvLinkInfo	    ceHyper;
    DtHelpHyperTextStruct   callData;

    XtSetArg(args[0], XmNuserData, &pDAS);
    XtGetValues(widget, args, 1);

    if (pDAS == NULL)
	return;

    if (! pDAS->dtinfo)
      {
	if (_DtCvSTATUS_OK == _DtCanvasGetCurLink(pDAS->canvas, &ceHyper))
	  {
	    callData.reason        = XmCR_ACTIVATE;
	    callData.event         = event;
	    callData.window        = XtWindow (pDAS->dispWid);
	    callData.specification = ceHyper.specification;
	    callData.hyper_type    = ceHyper.hyper_type;
	    callData.window_hint   = ceHyper.win_hint;
	    (*(pDAS->hyperCall)) (pDAS, pDAS->clientData, &callData);
	  }
      }
    else /* dtinfo context */
      {
	_DtCvPointer mark_enclosure = NULL;

	_DtCvStatus status;

	status = _DtCanvasGetCurTraversal(pDAS->canvas, &ceHyper,
							&mark_enclosure);

	if (status == _DtCvSTATUS_LINK)
	  {
	    callData.reason        = XmCR_ACTIVATE;
	    callData.event         = event;
	    callData.window        = XtWindow (pDAS->dispWid);
	    callData.specification = ceHyper.specification;
	    callData.hyper_type    = ceHyper.hyper_type;
	    callData.window_hint   = ceHyper.win_hint;
	    (*(pDAS->hyperCall)) (pDAS, pDAS->clientData, &callData);
	  }
	else if (status == _DtCvSTATUS_MARK)
	  {
	    callData.reason        = XmCR_ACTIVATE;
	    callData.event         = event;
	    callData.window        = XtWindow (pDAS->dispWid);
	    callData.specification = mark_enclosure;
	    callData.hyper_type    = -1; /* signifies it's a mark */
	    callData.window_hint   = 0;
	    (*(pDAS->hyperCall)) (pDAS, pDAS->clientData, &callData);
	  }

	/* otherwise do nothing */
      }

}  /* End _DtHelpActivateLink */

/*****************************************************************************
 * Function: _DtHelpPageUpOrDown
 *
 *    _DtHelpPageUpOrDown - Selects all the information in the widget.
 *
 *****************************************************************************/
void
_DtHelpPageUpOrDown (
    Widget          widget,
    XEvent          *event,
    String          *params,
    Cardinal        *num_params)
{
    int        keyPressed;
    _DtCvUnit  newY;
    _DtCvUnit  diff;
    Arg        args[2];
    XtPointer  userData;

    XtSetArg(args[0], XmNuserData, &userData);
    XtGetValues(widget, args, 1);

    if (userData != NULL)
      {
        DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) userData;

        diff = pDAS->dispUseHeight - pDAS->lineHeight;

	keyPressed = atoi(*params);
	if (keyPressed == 0)
	    diff = -diff;

        newY = pDAS->firstVisible + diff;

	/*
	 * Is the new Y position too large?
	 * If so, adjust.
	 */
        if (newY + ((int)pDAS->dispUseHeight) > pDAS->maxYpos)
            newY = pDAS->maxYpos - pDAS->dispUseHeight;

	/*
	 * Is the new Y before the begining?
	 * If so, zero it.
	 */
        if (newY < 0)
            newY = 0;

        if (newY != pDAS->firstVisible)
          {
            pDAS->firstVisible = newY;
            XtSetArg(args[0], XmNvalue, newY);
            XtSetValues (pDAS->vertScrollWid, args, 1);

	    if (pDAS->vScrollNotify)
	      (pDAS->vScrollNotify)(pDAS->clientData, pDAS->firstVisible);

            _DtHelpCleanAndDrawWholeCanvas (userData);
          }
      }

}  /* End _DtHelpPageUpOrDown */

/*****************************************************************************
 * Function: _DtHelpPageLeftOrRight
 *
 *    _DtHelpPageLeftOrRight - Selects all the information in the widget.
 *
 *****************************************************************************/
void
_DtHelpPageLeftOrRight (
    Widget          widget,
    XEvent          *event,
    String          *params,
    Cardinal        *num_params)
{
    int        keyPressed;
    _DtCvUnit  newX;
    _DtCvUnit  diff;
    Arg        args[2];
    XtPointer  userData;

    XtSetArg(args[0], XmNuserData, &userData);
    XtGetValues(widget, args, 1);

    if (userData != NULL)
      {
        DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) userData;

        diff = pDAS->dispUseWidth - ((int) pDAS->charWidth / 10);

	keyPressed = atoi(*params);
	if (keyPressed == 0)
	    diff = -diff;

        newX = pDAS->virtualX + diff;

	/*
	 * Is the new X position too large?
	 * If so, adjust.
	 */
        if (newX + ((int)pDAS->dispUseWidth) > pDAS->maxX)
            newX = pDAS->maxX - pDAS->dispUseWidth;

	/*
	 * Is the new X before the begining?
	 * If so, zero it.
	 */
        if (newX < 0)
            newX = 0;

        if (newX != pDAS->virtualX)
          {
            pDAS->virtualX = newX;
            XtSetArg(args[0], XmNvalue, newX);
            XtSetValues (pDAS->horzScrollWid, args, 1);

            _DtHelpCleanAndDrawWholeCanvas (userData);
          }
      }

}  /* End _DtHelpPageLeftOrRight */

/*****************************************************************************
 * Function: _DtHelpNextLink
 *
 *    _DtHelpNextLink - Moves the traversal to the requested hypertext link.
 *
 *****************************************************************************/
void
_DtHelpNextLink (
    Widget          widget,
    XEvent          *event,
    String          *params,
    Cardinal        *num_params)
{
    _DtCvTraversalCmd cmd = _DtCvTRAVERSAL_NEXT;
    _DtCvUnit  diff;
    _DtCvUnit  newY;
    _DtCvUnit  newX;
    _DtCvUnit  height;
    _DtCvUnit  top;
    Arg        args[2];
    XtPointer  userData;

    XtSetArg(args[0], XmNuserData, &userData);
    XtGetValues(widget, args, 1);

    if (userData != NULL)
      {
        DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) userData;

	switch(atoi(*params))
	  {
	    case 0: cmd = _DtCvTRAVERSAL_PREV;
		    break;
	    case 2: cmd = _DtCvTRAVERSAL_TOP;
		    break;
	    case 3: cmd = _DtCvTRAVERSAL_BOTTOM;
		    break;
	  }

	if (_DtCvSTATUS_OK == _DtCanvasMoveTraversal(pDAS->canvas, cmd, False,
				(XtIsRealized(pDAS->dispWid) ? True : False),
				NULL, &newX, &newY, NULL, &height))
	  {
	    /*
	     * take into account our traversal indicator
	     */
	    newY   -= pDAS->lineThickness;
	    height += (2 * pDAS->lineThickness);
    
	    top  = pDAS->firstVisible;
	    diff = ((int) pDAS->dispUseHeight) * 2 / 3;
	    if (newY < top)
	      {
		top = newY;
		if (cmd == _DtCvTRAVERSAL_TOP && newY <= diff)
		    top = 0;
	      }
	    else if (newY + height > top + ((int) pDAS->dispUseHeight))
	      {
		top = newY + height - ((int) pDAS->dispUseHeight);
		if (cmd == _DtCvTRAVERSAL_BOTTOM &&
						newY >= (pDAS->maxYpos - diff))
		    top = pDAS->maxYpos - pDAS->dispUseHeight;
	      }
    
	    if (top != pDAS->firstVisible)
	      {
		pDAS->firstVisible = top;
    
		if (top + ((int)pDAS->dispUseHeight) > pDAS->maxYpos)
		  {
		    pDAS->firstVisible = pDAS->maxYpos - pDAS->dispUseHeight;
		    if (pDAS->firstVisible < 0)
			pDAS->firstVisible = 0;
		  }
    
		XtSetArg (args[0], XmNvalue, pDAS->firstVisible);
		XtSetValues (pDAS->vertScrollWid, args, 1);
    
		if (pDAS->vScrollNotify)
		  (pDAS->vScrollNotify)(pDAS->clientData, pDAS->firstVisible);

		/*
		 * re-draw the information
		 */
		_DtHelpCleanAndDrawWholeCanvas (userData);
	      }
	  }
      }

}  /* End _DtHelpNextLink */
