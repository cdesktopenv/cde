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
/* $XConsortium: Callbacks.c /main/21 1996/10/22 12:22:33 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        Callbacks.c
 **
 **   Project:     Display Area Library
 **
 **  
 **   Description: This body of code handles the callbacks for the
 **                Display Area.
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1987-1994, 1996 Hewlett-Packard Company.
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*
 * system includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/DrawnB.h>
#include <Xm/CutPaste.h>
#include <Xm/XmPrivate.h>


/*
 * Canvas Engine
 */
#include "CanvasP.h"

/*
 * private includes
 */
#include "Access.h"
#include "DisplayAreaP.h"
#include "CallbacksI.h"
#include "FontI.h"
#include "FontAttrI.h"
#include "HyperTextI.h"
#include "SetListI.h"
#include "HelposI.h"
#include "XInterfaceI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
static	Boolean	 ConvertSelectionCB (
			Widget		 widget,
			Atom		*selection,
			Atom		*target,
			Atom		*type,
			XtPointer	*value,
			unsigned long	*length,
			int		*format );
static	void	 ScrollTimerCB (
			XtPointer	 client_data,
			XtIntervalId	*id );
static	void	 StartSelection (
			Widget		widget,
			XtPointer	client_data );
/********    End Private Function Declarations    ********/

/********    Private Defines                 ********/
#define	SCROLL_BAR_FLAGS 0x03
#define	HORIZONTAL	 0
#define	VERTICAL	 1

/********    End Private Defines             ********/

/********    Private Variable Declarations    ********/

/********    End Private Variable Declarations    ********/

/******************************************************************************
 *                             Private Functions
 ******************************************************************************/
/*****************************************************************************
 * Function: ConvertSelectionCB
 *
 *    ConvertSelectionCB - this routine is called when someone asks for
 *				our selection.
 *
 *****************************************************************************/
static	Boolean
ConvertSelectionCB (
	Widget		 widget,
	Atom		*selection,
	Atom		*target,
	Atom		*type,
	XtPointer	*value,
	unsigned long	*length,
	int		*format )
{
    Atom  TARGETS    = XmInternAtom(XtDisplay(widget), "TARGETS"  , False);
    Atom  TIMESTAMP  = XmInternAtom(XtDisplay(widget), "TIMESTAMP", False);
    Atom  TEXT       = XmInternAtom(XtDisplay(widget), "TEXT"     , False);
    Atom  CMP_TEXT   = XmInternAtom(XtDisplay(widget), "COMPOUND_TEXT",False);
    Atom  LOCALE;
    int   retStatus  = 0;
    char *testString = "ABC";  /* these are characters in XPCS, so... safe */
    char *tmpString  = NULL;
    Arg    args[2];
    DtHelpDispAreaStruct *pDAS;

    XTextProperty  tmpProp;

    XtSetArg(args[0], XmNuserData, &pDAS);
    XtGetValues(widget, args, 1);

    if (pDAS == NULL || pDAS->primary == False || *selection != XA_PRIMARY)
	return False;

    retStatus = XmbTextListToTextProperty(XtDisplay(widget), &testString, 1,
				(XICCEncodingStyle)XTextStyle, &tmpProp);

    LOCALE = (Atom) 9999; /* XmbTextList... should always be able
				     * to convert XPCS characters; but in
				     * case its broken, this prevents a core
				     * dump.
				     */
    if (retStatus == Success)
      {
	LOCALE = tmpProp.encoding;
	XFree(tmpProp.value);
      }

    /*
     * List the targets understood
     */
    if (*target == TARGETS)
      {
	Atom *targs = (Atom *)XtMalloc((unsigned) (5 * sizeof(Atom)));

	*value   = (char *) targs;
	*targs++ = LOCALE;
	*targs++ = TIMESTAMP;
	*targs++ = TEXT;
	*targs++ = CMP_TEXT;
	*targs++ = XA_STRING;

	*type   = XA_ATOM;
	*length = (5 * sizeof(Atom)) >> 2;
	*format = 32;
      }
    else if (*target == TIMESTAMP)
      {
	Time *timestamp;
	timestamp  = (Time *) XtMalloc(sizeof(Time));
	*timestamp = pDAS->anchor_time;

	*value  = (char *) timestamp;
	*type   = XA_INTEGER;
	*length = sizeof(Time);
	*format = 32;
      }
    else if (*target == XA_STRING)
      {
	/*
	 * initialize the return type here in case we fail
	 */
	*type   = (Atom) XA_STRING;
	*format = 8;

	/*
	 * get the string
	 */
	_DtCanvasGetSelection (pDAS->canvas,
				(_DtCvSELECTED_TEXT | _DtCvSELECTED_REGION),
						(_DtCvPointer *)(&tmpString));
	retStatus = -1;
	if (tmpString != NULL && *tmpString != '\0')
	    retStatus = XmbTextListToTextProperty(XtDisplay(widget),
				&tmpString, 1,
				(XICCEncodingStyle)XStringStyle, &tmpProp);

	/*
	 * free the original copy of the string and check the results
	 */
	if (tmpString != NULL)
	    free(tmpString);

	if (retStatus == Success || retStatus > 0)
	  {
	    *value  = (XtPointer) tmpProp.value;
	    *length = tmpProp.nitems;
	  }
	else
	  {
	    *value = NULL;
	    *length = 0;
	    return False;
	  }
      }
    else if (*target == TEXT)
      {
	/*
	 * set the type and format to those calculated
	 */
	*type   = TEXT;
	*format = 8;

	/*
	 * get the string
	 */
	_DtCanvasGetSelection (pDAS->canvas, 
				(_DtCvSELECTED_TEXT | _DtCvSELECTED_REGION),
						(_DtCvPointer *)(&tmpString));

	retStatus = -1;
	if (tmpString != NULL && *tmpString != '\0')
	    retStatus = XmbTextListToTextProperty(XtDisplay(widget),
				&tmpString, 1,
				(XICCEncodingStyle)XStdICCTextStyle, &tmpProp);

	/*
	 * free the original copy of the string and check the results
	 */
	if (tmpString != NULL)
	    free(tmpString);

	if (retStatus == Success || retStatus > 0)
	  {
	    *value  = (XtPointer) tmpProp.value;
	    *length = tmpProp.nitems;
	  }
	else
	  {
	    *value = NULL;
	    *length = 0;
	    return False;
	  }
      }
    else if (*target == LOCALE)
      {
	/*
	 * pass the string straight through
	 */
	*type   = LOCALE;
	*format = 8;

	_DtCanvasGetSelection (pDAS->canvas, 
				(_DtCvSELECTED_TEXT | _DtCvSELECTED_REGION),
						(_DtCvPointer *)(value));
	*length = 0;
	if (*value != NULL)
	    *length = strlen((char *)(*value));
      }
    else if (*target == CMP_TEXT)
      {
	*type   = CMP_TEXT;
	*format = 8;

	/*
	 * get the selected text.
	 */
	_DtCanvasGetSelection (pDAS->canvas, 
				(_DtCvSELECTED_TEXT | _DtCvSELECTED_REGION),
						(_DtCvPointer *)(&tmpString));

	retStatus = -1;
	if (tmpString != NULL && *tmpString != '\0')
	    retStatus = XmbTextListToTextProperty(XtDisplay(widget),
			&tmpString, 1,
			(XICCEncodingStyle)XCompoundTextStyle, &tmpProp);

	/*
	 * free the original copy of the string and check the results
	 */
	if (tmpString != NULL)
	    free(tmpString);

	if (retStatus == Success || retStatus > 0)
	  {
	    *value  = (XtPointer) tmpProp.value;
	    *length = tmpProp.nitems;
	  }
	else
	  {
	    *value = NULL;
	    *length = 0;
	    return False;
	  }
      }
    else
	return False;

    return True;

} /* End ConvertSelectionCB */

/*****************************************************************************
 * Function: ScrollTimerCB
 *
 *    ScrollTimerCB - This routine is called when we have a timer
 *		go off with the mouse outside the Display Area during a
 *		selection.
 *
 *****************************************************************************/
static	void
ScrollTimerCB (
	XtPointer	 client_data,	/*  data from applicaiton   */
	XtIntervalId	*id )		/*  timer id                */
{
    int    diffY = 0;
    int    diffX = 0;
    int    x;
    int    y;
    int    scrollTimeOut;
    int    maxY;
    int    dispY;
    Arg    args[2];
    XmScrollBarCallbackStruct callData;
    DtHelpDispAreaStruct  *pDAS = (DtHelpDispAreaStruct *) client_data;

    if (*id != pDAS->scr_timer_id)
      return;

    pDAS->scr_timer_id = 0;

    maxY  = pDAS->maxYpos;
    dispY = pDAS->firstVisible + pDAS->dispUseHeight;

    if ((pDAS->scr_timer_data.vertical_reason == XmCR_INCREMENT && maxY <= dispY)
		||
	(pDAS->scr_timer_data.vertical_reason == XmCR_DECREMENT &&
		!pDAS->firstVisible))
	pDAS->scr_timer_data.vertical_reason = XmCR_NONE;

    if ((pDAS->scr_timer_data.horizontal_reason == XmCR_INCREMENT &&
	pDAS->maxX <= pDAS->virtualX + ((int)pDAS->dispUseWidth))
		||
	(pDAS->scr_timer_data.horizontal_reason == XmCR_DECREMENT &&
		!pDAS->virtualX))
	pDAS->scr_timer_data.horizontal_reason = XmCR_NONE;

    if ( pDAS->scr_timer_data.vertical_reason == XmCR_NONE &&
		pDAS->scr_timer_data.horizontal_reason == XmCR_NONE)
	return;

    y = pDAS->firstVisible;
    if (pDAS->scr_timer_data.vertical_reason == XmCR_NONE)
	y = pDAS->scr_timer_y - pDAS->decorThickness;
    else if (pDAS->scr_timer_data.vertical_reason == XmCR_INCREMENT)
      {
        y = y + pDAS->dispUseHeight + pDAS->lineHeight;
	if (y > pDAS->maxYpos)
	    y = pDAS->maxYpos;

        diffY = y - pDAS->firstVisible - pDAS->dispUseHeight;
      }
    else if (pDAS->scr_timer_data.vertical_reason == XmCR_DECREMENT)
      {
	y -= pDAS->lineHeight;
	if (y < 0)
	    y = 0;

        diffY = y - pDAS->firstVisible;
      }

    if (pDAS->scr_timer_data.horizontal_reason == XmCR_NONE)
	x = pDAS->scr_timer_x - pDAS->decorThickness;
    else
      {
	if (pDAS->scr_timer_data.horizontal_reason == XmCR_INCREMENT)
          {
	    diffX = (int) (pDAS->charWidth / 10);
	    x = pDAS->dispUseWidth;

	    if (x + pDAS->virtualX + diffX > pDAS->maxX)
		diffX = pDAS->maxX - x - pDAS->virtualX;
          }
	else if (pDAS->scr_timer_data.horizontal_reason == XmCR_DECREMENT)
          {
	    diffX = -((int)(pDAS->charWidth / 10));
	    x = 0;

	    if (pDAS->virtualX + diffX < 0)
		diffX = -(pDAS->virtualX);
          }
      }
   
    _DtCanvasProcessSelection (pDAS->canvas,
			(x + diffX + pDAS->virtualX - pDAS->decorThickness),
			y, _DtCvSELECTION_UPDATE);

    callData.event = NULL;
    if (diffX)
      {
	scrollTimeOut   = pDAS->horz_rep_scr;
	callData.reason = pDAS->scr_timer_data.horizontal_reason;
	callData.value  = pDAS->virtualX + diffX;
	_DtHelpHorzScrollCB (pDAS->horzScrollWid, client_data,
					(XtPointer) &callData);

	if (pDAS->horzScrollWid)
	  {
	    XtSetArg (args[0], XmNvalue, pDAS->virtualX);
	    XtSetValues (pDAS->horzScrollWid, args, 1);
	  }

	if (diffY != 0 && pDAS->vertScrollWid)
	  {
	    XtSetArg (args[0], XmNvalue, y);
	    XtSetValues (pDAS->vertScrollWid, args, 1);
	    if (pDAS->vScrollNotify)
	      (pDAS->vScrollNotify)(pDAS->clientData, y);
	  }
      }
    else
      {
	scrollTimeOut   = pDAS->vert_rep_scr;
	callData.reason = pDAS->scr_timer_data.vertical_reason;
	callData.value  = pDAS->firstVisible + diffY;
	_DtHelpVertScrollCB (pDAS->vertScrollWid, client_data,
					(XtPointer) &callData);

	if (pDAS->vertScrollWid)
	  {
	    XtSetArg (args[0], XmNvalue, pDAS->firstVisible);
	    XtSetValues (pDAS->vertScrollWid, args, 1);
	    if (pDAS->vScrollNotify)
	      (pDAS->vScrollNotify)(pDAS->clientData, pDAS->firstVisible);
	  }
      }

    pDAS->scr_timer_id = XtAppAddTimeOut (
		XtWidgetToApplicationContext (pDAS->dispWid),
		((unsigned long) scrollTimeOut),
		ScrollTimerCB, client_data);

} /* End ScrollTimerCB */

/******************************************************************************
 * Function: DrawWholeCanvas
 *
 *****************************************************************************/
static void
DrawWholeCanvas(
    DtHelpDispAreaStruct  *pDAS)
{
    _DtCvUnit    top;
    _DtCvUnit    bottom;
    _DtCvUnit    right;
    _DtCvUnit    next_y;

    _DtCanvasMoveTraversal(pDAS->canvas,_DtCvTRAVERSAL_OFF, False,
				(XtIsRealized(pDAS->dispWid) ? True : False),
				NULL, NULL, NULL, NULL, NULL);

    top     = pDAS->firstVisible;
    bottom  = top + pDAS->dispUseHeight;
    right   = pDAS->virtualX + pDAS->dispUseWidth;

    _DtCanvasRender (pDAS->canvas, pDAS->virtualX, top, right, bottom,
		     pDAS->render_type, _DtCvTRUE, NULL, &next_y);
    pDAS->nextNonVisible =
	(pDAS->render_type == _DtCvRENDER_PARTIAL) ? bottom : next_y;

    /*
     * If we have a hypertext link boxed, draw it
     */
    _DtCanvasMoveTraversal(pDAS->canvas, _DtCvTRAVERSAL_ON, False,
				(XtIsRealized(pDAS->dispWid) ? True : False),
					NULL, NULL, NULL, NULL, NULL);
    /*
     * if the toc exists within this area, draw it.
     */
    if ((pDAS->toc_flag & _DT_HELP_TOC_ON) &&
		pDAS->toc_y + pDAS->toc_height >= top && pDAS->toc_y <  bottom)
	_DtHelpDATocMarker((XtPointer) pDAS, True);

} /* End DrawWholeCanvas */

/******************************************************************************
 *                          Semi Public Functions
 *****************************************************************************/
/******************************************************************************
 * Function: _DtHelpCleanAndDrawWholeCanvas
 *
 *****************************************************************************/
void
_DtHelpCleanAndDrawWholeCanvas(
    XtPointer  client_data)
{
    DtHelpDispAreaStruct  *pDAS = (DtHelpDispAreaStruct *) client_data;

    XClearArea (XtDisplay(pDAS->dispWid), XtWindow(pDAS->dispWid),
				pDAS->decorThickness, pDAS->decorThickness,
				pDAS->dispUseWidth,
				pDAS->dispUseHeight,
				False);
    DrawWholeCanvas (pDAS);

}

void
_DtHelpSearchMoveTraversal(XtPointer client_data, int search_hit_index)
{
    _DtCvUnit baseline, ascend, descend;

    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    if (_DtCvGetSearchLineMetrics(pDAS->canvas, search_hit_index,
					&baseline, &descend, &ascend) == 0) {

	_DtCvUnit top, hitY, height;

	top    = pDAS->firstVisible;
	hitY   = baseline - ascend - pDAS->lineThickness;
	height = ascend + descend + 2 * pDAS->lineThickness;

	if (hitY < top)
	    top = hitY;
	else if (hitY + height > top + ((int) pDAS->dispUseHeight))
	    top = hitY + height - ((int) pDAS->dispUseHeight);

	if (top != pDAS->firstVisible) {

	    _DtCvUnit dispUseHeightHalf =
			pDAS->dispUseHeight / 2 + pDAS->dispUseHeight % 2;

	    /* put search hit in the middle of the viewport */
	    if (top == hitY)
		top -= dispUseHeightHalf - height;
	    else {
		top += dispUseHeightHalf + height;
		if (top + ((int)pDAS->dispUseHeight) > pDAS->maxYpos)
		    top = pDAS->maxYpos - pDAS->dispUseHeight;
	    }
	    if (top < 0)
		top = 0;

	    if (top != pDAS->firstVisible) {

		Arg arg;

		pDAS->firstVisible = top;

		XtSetArg(arg, XmNvalue, pDAS->firstVisible);
		XtSetValues(pDAS->vertScrollWid, &arg, 1);

		if (pDAS->vScrollNotify)
		  (pDAS->vScrollNotify)(pDAS->clientData, pDAS->firstVisible);

		_DtHelpCleanAndDrawWholeCanvas(pDAS);
	    }
	}
    }
}

/******************************************************************************
 * Function: _DtHelpCancelSelection
 *
 * Returns : True    if a selection was active and cancelled.
 *           False   if a selection was not active.
 *
 *****************************************************************************/
Boolean
_DtHelpCancelSelection(
    XtPointer  client_data)
{
    Boolean selActive = False;
    DtHelpDispAreaStruct  *pDAS = (DtHelpDispAreaStruct *) client_data;

    if (pDAS->select_state == _DtHelpSelectingText && pDAS->primary == True)
      {
	selActive = True;
	if (pDAS->scr_timer_id)
	  {
	    XtRemoveTimeOut (pDAS->scr_timer_id);
	    pDAS->scr_timer_id = 0;
	  }
	_DtHelpClearSelection (client_data);
      }

    return selActive;
}

/******************************************************************************
 *                          Public Functions
 *****************************************************************************/
/******************************************************************************
 * Function: _DtHelpExposeCB
 *
 *    _DtHelpExposeCB handles the exposure events for a Text Graphic Area.
 *
 *****************************************************************************/
void
_DtHelpExposeCB(
        Widget  widget,
	XtPointer client_data,
	XtPointer call_data )
{
    Arg    args[4];

    Dimension  height;
    Dimension  width;

    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    XmDrawnButtonCallbackStruct *callback =
				(XmDrawnButtonCallbackStruct *) call_data;

    if (callback->reason != XmCR_EXPOSE ||
		(pDAS->neededFlags & (1 << (VisibilityFullyObscured + 3))))
	return;

    /*
     * get the width and height.
     */
    XtSetArg(args[0], XmNwidth, &width);
    XtSetArg(args[1], XmNheight, &height);
    XtGetValues(widget, args, 2);

    /*
     * if this exposure is a result of a resize,
     * wait for the resize to handle it.
     */
    if (width != pDAS->dispWidth || height != pDAS->dispHeight)
	return;

    if (!(callback->event) || callback->event->xexpose.count)
	return;

    /*
     * re-draw the information in the display area
     */
    DrawWholeCanvas (pDAS);

}  /* End _DtHelpExposeCB */

/*********************************************************************
 * Function: _DtHelpResizeCB
 *
 *    _DtHelpResizeCB handles the exposure events for a Text Graphic Area.
 *
 *********************************************************************/
void
_DtHelpResizeCB(
        Widget  widget,
	XtPointer client_data,
	XtPointer call_data )
{
    Arg  args[4];
    Dimension width;
    Dimension height;

    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;
    XmDrawnButtonCallbackStruct *callback =
				(XmDrawnButtonCallbackStruct *) call_data;

    if (callback->reason != XmCR_RESIZE)
	return;

    /*
     * get the width and height of the form.
     */
    XtSetArg(args[0], XmNwidth, &width);
    XtSetArg(args[1], XmNheight, &height);
    XtGetValues(XtParent(widget), args, 2);
    if (width == pDAS->formWidth && height == pDAS->formHeight)
	return;

    pDAS->formWidth  = width;
    pDAS->formHeight = height;

    /*
     * get the width and height.
     */
    XtSetArg(args[0], XmNwidth, &width);
    XtSetArg(args[1], XmNheight, &height);
    XtGetValues(widget, args, 2);

    if (width == pDAS->dispWidth && height == pDAS->dispHeight)
	return;

    /*

    _DtHelpClearSelection (pDAS);
     * reset the scroll bars and possibly reformat the text for the size.
     */
    (void) _DtHelpSetScrollBars (client_data, width, height);
    if (XtIsRealized (pDAS->dispWid))
        _DtHelpCleanAndDrawWholeCanvas (client_data);

    /*
     * I will get an expose event after the resize.
     */

}  /* End _DtHelpResizeCB */

/***************************************************************************
 * Function:  _DtHelpVertScrollCB
 *
 * _DtHelpVertScrollCB is called when the vertical scroll bar is changed.
 *
 **************************************************************************/
void 
_DtHelpVertScrollCB(
        Widget widget,
        XtPointer clientData,
        XtPointer callData )
{
    DtHelpDispAreaStruct      *pDAS     = (DtHelpDispAreaStruct *) clientData;
    XmScrollBarCallbackStruct *callBack =
					(XmScrollBarCallbackStruct *) callData;
    int         diff   = pDAS->lineHeight;
    int         srcY, dstY;
    int         clearY;
    int		reason = callBack->reason;
    _DtCvUnit   absTop;
    _DtCvUnit   absBot;
    Display    *dpy;
    Window      win;
  
    /*
     * if the policy is XmEXPLICIT, don't want the focus on the scrollbar
     */
    if (callBack->event != NULL && callBack->event->type == ButtonPress &&
	      _XmGetFocusPolicy(XtParent(XtParent(pDAS->dispWid))) != XmPOINTER)
	XmProcessTraversal(pDAS->dispWid, XmTRAVERSE_CURRENT);

    /*
     * check to make sure we don't do a rerender when we don't have to.
     */
    if (pDAS->firstVisible == callBack->value)
	return;

    /* If a drag occured, reset the reason to increment, decrement, page    */
    /* increment, or page decrement depending on the distance and direction */
    /* dragged. */
    if (callBack->reason == XmCR_DRAG || callBack->reason == XmCR_VALUE_CHANGED)
      {
	diff = callBack->value - pDAS->firstVisible;

	if (diff > 0 && diff <= ((int) pDAS->dispUseHeight))
            reason = XmCR_INCREMENT;
	else if (diff < 0 && -(diff) <= ((int) pDAS->dispUseHeight))
	  {
            reason = XmCR_DECREMENT;
	    diff   = -diff;
	  }
        else if (diff > ((int) pDAS->dispUseHeight))
            reason = XmCR_PAGE_DECREMENT;
        else
            reason = XmCR_PAGE_INCREMENT;
      }
    else if (callBack->reason == XmCR_INCREMENT ||
					callBack->reason == XmCR_DECREMENT)
      {
	diff = callBack->value - pDAS->firstVisible;
	if (diff < 0)
	    diff = -diff;
      }

    /* Reset first visible to the returned scrollbar value. */
    pDAS->firstVisible = callBack->value;

    /* For page increment and decrement, call _DtHelpCleanAndDrawWholeCanvas
     * to clear the view area and redisplay the text.
     *
     * For increment and decrement, 
     * use XCopyArea to move the visible lines and draw the cleared out line.
     */
    if (!pDAS->maxYpos ||
		(pDAS->neededFlags & (1 << (VisibilityFullyObscured + 3))))
        return;
  
    dpy = XtDisplay (widget);
    win = XtWindow (pDAS->dispWid);

    if (reason == XmCR_PAGE_INCREMENT || reason == XmCR_PAGE_DECREMENT ||
		reason == XmCR_TO_TOP || reason == XmCR_TO_BOTTOM)
	_DtHelpCleanAndDrawWholeCanvas (clientData);
    else
      {
        if (reason == XmCR_INCREMENT)
	  {
	    dstY   = pDAS->decorThickness;
	    srcY   = dstY + diff;
	    clearY = pDAS->dispHeight - pDAS->decorThickness - diff;
	  }
	else
	  {
	    srcY   = pDAS->decorThickness;
	    dstY   = srcY + diff;
	    clearY = srcY;
	  }
        XCopyArea(dpy, win, win, pDAS->normalGC, pDAS->decorThickness, srcY,
		pDAS->dispUseWidth, (pDAS->dispUseHeight - diff),
		pDAS->decorThickness, dstY);

        XClearArea(dpy, win, pDAS->decorThickness, clearY,
		 pDAS->dispUseWidth, ((unsigned int) diff), False);

	if (pDAS->neededFlags & (1 << (VisibilityPartiallyObscured + 3)))
	  {
	    /*
	     * redraw all the information
	     */
	    DrawWholeCanvas (pDAS);
	  }
	else
	  {
	    /*
	     * draw the line that sits on the cleared line
	     */
	    absTop = clearY + pDAS->firstVisible - pDAS->decorThickness;
	    absBot = absTop + diff;

	    _DtCanvasRender (pDAS->canvas, 0, absTop,
				pDAS->virtualX + pDAS->dispWidth, absBot,
				_DtCvRENDER_PARTIAL, _DtCvFALSE, NULL, NULL);
	    /*
	     * if the toc exists within this area, draw it.
	     */
	    if ((pDAS->toc_flag & _DT_HELP_TOC_ON)
				&& pDAS->toc_y + pDAS->toc_height >= absTop
				&& pDAS->toc_y < absBot)
		_DtHelpDATocMarker((XtPointer) pDAS, True);
	  }
      }

}  /* End _DtHelpVertScrollCB */

/***************************************************************************
 * Function:  _DtHelpHorzScrollCB
 *
 * _DtHelpHorzScrollCB is called when the horizontal scroll bar is changed.
 *
 **************************************************************************/
void 
_DtHelpHorzScrollCB(
        Widget widget,
        XtPointer clientData,
        XtPointer callData )
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) clientData;
    XmScrollBarCallbackStruct *callBack =
					(XmScrollBarCallbackStruct *) callData;
    int		 diff = (int)(pDAS->charWidth / 10);
    int		 srcX;
    int		 dstX;
    int		 clearX;
    int		 reason = callBack->reason;
    _DtCvUnit	 absLeft;
    _DtCvUnit	 absRight;
    _DtCvUnit	 absY;
    Display	*dpy;
    Window	 win;

    /*
     * if the policy is XmEXPLICIT, don't want the focus on the scrollbar
     */
    if (callBack->event != NULL && callBack->event->type == ButtonPress &&
	      _XmGetFocusPolicy(XtParent(XtParent(pDAS->dispWid))) != XmPOINTER)
	XmProcessTraversal(pDAS->dispWid, XmTRAVERSE_CURRENT);

    /*
     * check to make sure we don't do a rerender when we don't have to.
     */
    if (pDAS->virtualX == callBack->value)
	return;

    /* If a drag occured, reset the reason to increment, decrement, page    */
    /* increment, or page decrement depending on the distance and direction */
    /* dragged. */
    if (callBack->reason == XmCR_DRAG || callBack->reason == XmCR_VALUE_CHANGED)
      {
	diff = callBack->value - pDAS->virtualX;

	if (diff > 0 && diff <= ((int) pDAS->dispUseWidth))
            reason = XmCR_INCREMENT;
	else if (diff < 0 && -(diff) <= ((int) pDAS->dispUseWidth))
	  {
            reason = XmCR_DECREMENT;
	    diff   = -diff;
	  }
        else if (diff > ((int) pDAS->dispUseWidth))
            reason = XmCR_PAGE_DECREMENT;
        else
            reason = XmCR_PAGE_INCREMENT;
      }
    else if (callBack->reason == XmCR_INCREMENT ||
					callBack->reason == XmCR_DECREMENT)
      {
	diff = callBack->value - pDAS->virtualX;
	if (diff < 0)
	    diff = -diff;
      }

    /* Reset first visible to the returned scrollbar value. */
    pDAS->virtualX = callBack->value;

    /* For page increment and decrement, call _DtHelpCleanAndDrawWholeCanvas
     * to clear the view area and redisplay the text.
     *
     * For increment and decrement, 
     * use XCopyArea to move the visible lines and draw the cleared out line.
     */
    if (!pDAS->maxX || !pDAS->visibleCount ||
		(pDAS->neededFlags & (1 << (VisibilityFullyObscured + 3))))
        return;
  
    dpy = XtDisplay (widget);
    win = XtWindow (pDAS->dispWid);

    /* For page increment and decrement, clear the view area and call
     * View_DtHelpExposeCB to redisplay the text. For increment and decrement,
     * use XCopyArea to move the visible lines and draw the cleared out line.
     */
    if (reason == XmCR_PAGE_INCREMENT || reason == XmCR_PAGE_DECREMENT ||
		reason == XmCR_TO_TOP || reason == XmCR_TO_BOTTOM)
	_DtHelpCleanAndDrawWholeCanvas (clientData);
    else
      {
        if (reason == XmCR_INCREMENT)
	  {
	    dstX   = pDAS->decorThickness;
	    srcX   = dstX + diff;
	    clearX = pDAS->dispWidth - pDAS->decorThickness - diff;
	  }
	else
	  {
	    srcX   = pDAS->decorThickness;
	    dstX   = srcX + diff;
	    clearX = srcX;
	  }

        XCopyArea(dpy, win, win, pDAS->normalGC, srcX, pDAS->decorThickness,
			pDAS->dispUseWidth - diff, pDAS->dispUseHeight,
			dstX, pDAS->decorThickness);

        XClearArea(dpy, win, clearX, pDAS->decorThickness,
		((unsigned int) diff), pDAS->dispUseHeight, False);

	if (pDAS->neededFlags & (1 << (VisibilityPartiallyObscured + 3)))
	  {
	    /*
	     * redraw all the information
	     */
	    DrawWholeCanvas (pDAS);
	  }
	else
	  {
	    /*
	     * draw the line that sits on the cleared line
	     */
	    absLeft  = clearX + pDAS->virtualX - pDAS->decorThickness;
	    absRight = absLeft + diff;
	    absY     = pDAS->firstVisible - pDAS->decorThickness;

	    _DtCanvasRender (pDAS->canvas, absLeft, absY,
				absRight, absY + pDAS->dispHeight,
				_DtCvRENDER_PARTIAL, _DtCvFALSE, NULL, NULL);

	    /*
	     * if the toc exists within this area, draw it.
	     */
	    if ((pDAS->toc_flag & _DT_HELP_TOC_ON)
		&& ((int) (pDAS->toc_y + pDAS->toc_height)) >= ((int) absY)
		&& ((int) pDAS->toc_y) < ((int) (absY + pDAS->dispHeight)))
		_DtHelpDATocMarker((XtPointer) pDAS, True);

	  }
      }

}  /* End _DtHelpHorzScrollCB */

/***************************************************************************
 * Function:  _DtHelpClickOrSelectCB
 *
 * _DtHelpClickOrSelectCB is called when the vertical scroll bar is changed.
 *
 **************************************************************************/
void 
_DtHelpClickOrSelectCB(
        Widget widget,
        XtPointer clientData,
        XtPointer callData )
{
    XmDrawnButtonCallbackStruct *callBack =
				(XmDrawnButtonCallbackStruct *) callData;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct*) clientData;

    /*
     * If this is not an ARM call or entered through an Arm&Activate
     * (event-type will be keypress or keyrelease) throw it away.
     */
    if (callBack->reason != XmCR_ARM || callBack->event == NULL ||
	callBack->event->type == KeyPress ||
	callBack->event->type == KeyRelease)
	return;

    pDAS->timerX       = callBack->event->xbutton.x;
    pDAS->timerY       = callBack->event->xbutton.y;
    pDAS->select_state = _DtHelpCopyOrLink;

    if (NULL != pDAS->armCallback)
	(pDAS->armCallback)(pDAS->clientData);

}  /* End _DtHelpClickOrSelectCB */

/*****************************************************************************
 * Function: _DtHelpEndSelectionCB
 *
 *
 * Called by: Callback for the Selection mechanism
 *****************************************************************************/
void
_DtHelpEndSelectionCB (
	Widget		w,		/*  widget id           */
	XtPointer	client_data,	/*  data from applicaiton   */
	XtPointer	call_data )	/*  data from widget class  */
{
    XmDrawnButtonCallbackStruct *callback =
                                (XmDrawnButtonCallbackStruct *) call_data;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct*) client_data;
    int    newX;
    int    newY;

    if (callback->reason != XmCR_DISARM || callback->event == NULL ||
	callback->event->type == KeyPress ||
	callback->event->type == KeyRelease)
        return;

    /*
     * if a scroll timer is active, we are selecting text.
     * stop it.
     */
    if (pDAS->scr_timer_id)
      {
	XtRemoveTimeOut (pDAS->scr_timer_id);
	pDAS->scr_timer_id = 0;
      }

    newX = callback->event->xbutton.x;
    newY = callback->event->xbutton.y;
    if (pDAS->select_state == _DtHelpCopyOrLink)
      {
	if (abs (pDAS->timerX - newX) <= pDAS->moveThreshold &&
			abs (pDAS->timerY - newY) <= pDAS->moveThreshold)
	  {
	    _DtHelpClearSelection (client_data);

	    /*
	     * If this is null, we came the the Arm&Activate routine
	     * which means a key was pressed, so we ignore this call.
	     */
	    if (callback->event)
	        /*
	         * find the hypertext link and process it.
	         */
                _DtHelpProcessHyperSelection (client_data,
						pDAS->timerX, pDAS->timerY,
						callback->event);
	  }
	else
	    StartSelection (w, client_data);
        return;
      }

    /*
     * The user was doing a selection, finish it up.
     */
    if (pDAS->select_state != _DtHelpNothingDoing)
      {
	_DtCanvasProcessSelection(pDAS->canvas,
		(newX + pDAS->virtualX - pDAS->decorThickness),
		(newY + pDAS->firstVisible - pDAS->decorThickness),
		_DtCvSELECTION_END);
	_DtCanvasMoveTraversal(pDAS->canvas, _DtCvTRAVERSAL_ON, False, True,
				NULL, NULL, NULL, NULL, NULL);

      }

    pDAS->select_state = _DtHelpNothingDoing;
    return;

}  /* End _DtHelpEndSelectionCB */

/***************************************************************************
 * Function:  _DtHelpMouseMoveCB
 *
 * _DtHelpMouseMoveCB tracks the mouse movement for the Selection mechanism
 *
 **************************************************************************/
void 
_DtHelpMouseMoveCB(
        Widget	   widget,
        XtPointer  client_data,
        XEvent	  *event )
{
    _DtCvUnit   newX;
    _DtCvUnit   newY;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * If a selection is not in progress, don't do anything.
     */
    if (pDAS->select_state == _DtHelpNothingDoing || event->type != MotionNotify)
	return;

    if (pDAS->scr_timer_id)
      {
	XtRemoveTimeOut (pDAS->scr_timer_id);
	pDAS->scr_timer_id = 0;
      }

    newX = event->xmotion.x;
    newY = event->xmotion.y;

    if (pDAS->select_state == _DtHelpCopyOrLink)
      {
	if (abs (newX - pDAS->timerX) < pDAS->moveThreshold &&
			abs (newY - pDAS->timerY) < pDAS->moveThreshold)
	    return;

	StartSelection (widget, client_data);
	return;
      }

    if (newY < ((int) pDAS->decorThickness) && pDAS->firstVisible)
	pDAS->scr_timer_data.vertical_reason = XmCR_DECREMENT;

    else if ((newY >
	    (((int) pDAS->dispHeight)-((int)pDAS->decorThickness))) &&
	    (pDAS->maxYpos >
		(((int)pDAS->firstVisible)
				+ ((int)pDAS->dispUseHeight))))
	pDAS->scr_timer_data.vertical_reason = XmCR_INCREMENT;
    else
	pDAS->scr_timer_data.vertical_reason = XmCR_NONE;

    if (newX < ((int) pDAS->decorThickness) && pDAS->virtualX)
	pDAS->scr_timer_data.horizontal_reason = XmCR_DECREMENT;
    else if (newX > ((int) pDAS->dispWidth) &&
	    pDAS->maxX > pDAS->virtualX + ((int) pDAS->dispUseWidth))
	pDAS->scr_timer_data.horizontal_reason = XmCR_INCREMENT;
    else
	pDAS->scr_timer_data.horizontal_reason = XmCR_NONE;

    if (pDAS->scr_timer_data.vertical_reason != XmCR_NONE ||
	pDAS->scr_timer_data.horizontal_reason != XmCR_NONE)
      {
	int scrollTimeOut = pDAS->vert_init_scr;

	if (pDAS->scr_timer_data.horizontal_reason != XmCR_NONE)
	    scrollTimeOut = pDAS->horz_init_scr;

	pDAS->scr_timer_x  = newX;
	pDAS->scr_timer_y  = newY;
	pDAS->scr_timer_id =
			XtAppAddTimeOut(XtWidgetToApplicationContext(widget),
				((unsigned long) scrollTimeOut),
				ScrollTimerCB, (XtPointer) pDAS);
	return;
      }

    newX = newX + pDAS->virtualX - pDAS->decorThickness;
    if (newX < 0)
	newX = 0;

    newY = newY + pDAS->firstVisible - pDAS->decorThickness;
    if (newY < 0)
	newY = 0;

    _DtCanvasProcessSelection(pDAS->canvas, newX, newY, _DtCvSELECTION_UPDATE);

} /* End _DtHelpMouseMoveCB */

/*****************************************************************************
 * Function: StartSelection
 *
 *    StartSelection - If this routine is called, the user has initiated a
 *                   selection.
 *
 *****************************************************************************/
static	void
StartSelection (
	Widget		widget,		/*  widget id               */
	XtPointer	client_data )	/*  data from applicaiton   */
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct*) client_data;

    /*
     * If this widget doesn't own the primary selection, get it.
     */
    _DtHelpGetClearSelection(widget, client_data);

    /*
     * check to see if we have the primary selection.
     */
    if (pDAS->primary == True)
      {
	_DtCanvasMoveTraversal(pDAS->canvas, _DtCvTRAVERSAL_OFF, False, True,
				NULL, NULL, NULL, NULL, NULL);
	_DtCanvasProcessSelection(pDAS->canvas,
			pDAS->timerX + pDAS->virtualX - pDAS->decorThickness,
			pDAS->timerY + pDAS->firstVisible -pDAS->decorThickness,
			_DtCvSELECTION_START);

	pDAS->select_state  = _DtHelpSelectingText;
	pDAS->text_selected = True;
      }

}  /* End StartSelection */

/*****************************************************************************
 * Function: _DtHelpLoseSelectionCB
 *
 *    _DtHelpLoseSelectionCB - This routine is called when we lose the selection
 *
 *****************************************************************************/
void
_DtHelpLoseSelectionCB (
	Widget	 widget,
	Atom	*selection )
{
    Arg    args[2];
    DtHelpDispAreaStruct *pDAS;

    XtSetArg(args[0], XmNuserData, &pDAS);
    XtGetValues(widget, args, 1);

    if (pDAS != NULL && pDAS->dispWid == widget && *selection == XA_PRIMARY)
      {
	_DtHelpClearSelection ((XtPointer) pDAS);
	pDAS->primary       = False;
	pDAS->text_selected = False;
      }
} /* End _DtHelpLoseSelectionCB */

/*****************************************************************************
 * Function: _DtHelpClearSelection
 *
 *    Clears the selection pointers and variables
 *
 *****************************************************************************/
void
_DtHelpClearSelection ( XtPointer client_data)
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct*) client_data;

    if (pDAS->primary == True)
      {
	if (NULL != pDAS->canvas)
	  {
	    _DtCanvasMoveTraversal(pDAS->canvas, _DtCvTRAVERSAL_OFF, False, True,
				   NULL, NULL, NULL, NULL, NULL);
	    _DtCanvasProcessSelection(pDAS->canvas, 0, 0, _DtCvSELECTION_CLEAR);
	    _DtCanvasMoveTraversal(pDAS->canvas, _DtCvTRAVERSAL_ON, False, True,
				   NULL, NULL, NULL, NULL, NULL);
	  }
        pDAS->select_state  = _DtHelpNothingDoing;
        pDAS->text_selected = False;
      }
}

/***************************************************************************
 * Function:  _DtHelpFocusCB
 *
 * _DtHelpFocusCB tracks the traversal of the hypertext.
 *
 **************************************************************************/
void 
_DtHelpFocusCB(
        Widget	   widget,
        XtPointer  client_data,
        XEvent	  *event )
{
    Boolean                oldFlag;
    Boolean                newFlag = False;
    DtHelpDispAreaStruct  *pDAS = (DtHelpDispAreaStruct *) client_data;

    if (pDAS->hyperCall == NULL ||
		(event->type != FocusIn && event->type != FocusOut) ||
		!event->xfocus.send_event)
	return;

    /*
     * get the old flag
     */
    oldFlag = (pDAS->neededFlags & _DT_HELP_FOCUS_FLAG) ? True : False;

    /*
     * get the new flag
     */
    if (event->type == FocusIn)
	newFlag = True;

    if (oldFlag != newFlag)
      {
	if (newFlag == False)
	  {
            _DtCanvasMoveTraversal (pDAS->canvas, _DtCvTRAVERSAL_OFF, False,
				(XtIsRealized(widget) ? True : False),
				NULL, NULL, NULL, NULL, NULL);
            pDAS->neededFlags = pDAS->neededFlags & ~(_DT_HELP_FOCUS_FLAG);
	  }
	else
	  {
            pDAS->neededFlags = pDAS->neededFlags | _DT_HELP_FOCUS_FLAG;
            _DtCanvasMoveTraversal (pDAS->canvas, _DtCvTRAVERSAL_ON, False,
				(XtIsRealized(widget) ? True : False),
				NULL, NULL, NULL, NULL, NULL);
	  }
      }

} /* End _DtHelpFocusCB */

/***************************************************************************
 * Function:  _DtHelpEnterLeaveCB
 *
 * _DtHelpEnterLeaveCB tracks the traversal of the hypertext.
 *
 **************************************************************************/
void
_DtHelpEnterLeaveCB(
        Widget     widget,
        XtPointer  client_data,
        XEvent    *event )
{
    Boolean                oldFlag;
    Boolean                newFlag = False;
    DtHelpDispAreaStruct  *pDAS = (DtHelpDispAreaStruct *) client_data;

    if (pDAS->hyperCall == NULL ||
                (event->type != EnterNotify && event->type != LeaveNotify))
        return;

    /*
     * get the old flag
     */
    oldFlag = (pDAS->neededFlags & _DT_HELP_FOCUS_FLAG) ? True : False;

    /*
     * get the new flag
     */
    if (event->type == FocusIn)
	newFlag = True;

    if (oldFlag != newFlag)
      {
	if (oldFlag == True)
	  {
            _DtCanvasMoveTraversal (pDAS->canvas, _DtCvTRAVERSAL_OFF, False,
				(XtIsRealized(widget) ? True : False),
				NULL, NULL, NULL, NULL, NULL);
            pDAS->neededFlags = pDAS->neededFlags & ~(_DT_HELP_FOCUS_FLAG);
	  }
	else
	  {
            pDAS->neededFlags = pDAS->neededFlags | _DT_HELP_FOCUS_FLAG;
            _DtCanvasMoveTraversal (pDAS->canvas, _DtCvTRAVERSAL_ON, False,
				(XtIsRealized(widget) ? True : False),
				NULL, NULL, NULL, NULL, NULL);
	  }
      }
} /* End _DtHelpEnterLeaveCB */

/***************************************************************************
 * Function:  _DtHelpVisibilityCB
 *
 * _DtHelpVisibilityCB tracks whether the window becomes obscured.
 *
 **************************************************************************/
void 
_DtHelpVisibilityCB(
        Widget	   widget,
        XtPointer  client_data,
        XEvent	  *event )
{
    DtHelpDispAreaStruct  *pDAS = (DtHelpDispAreaStruct *) client_data;

    if (event->type != VisibilityNotify)
	return;

    /*
     * save the scrollbar and focus flags while clearing the visibility flags.
     */
    pDAS->neededFlags = pDAS->neededFlags &
				(_DT_HELP_FOCUS_FLAG | SCROLL_BAR_FLAGS);

    /*
     * set the visibility flag
     */
    pDAS->neededFlags = pDAS->neededFlags |
				(1 << (event->xvisibility.state + 3));

} /* End _DtHelpVisibilityCB */

/*****************************************************************************
 * Function: _DtHelpInitiateClipboard
 *
 *    _DtHelpInitiateClipboard
 *
 *****************************************************************************/
void
_DtHelpInitiateClipboard (
    XtPointer  client_data)
{
    DtHelpDispAreaStruct  *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * check to see if we have the primary selection
     * before trying for the clipboard.
     */
    if (pDAS->primary == True && pDAS->text_selected == True)
      {
        long      itemId = 0L;               	     /* clipboard item id */
        long      dataId = 0L;                       /* clipboard data id */
        int       status;                            /* clipboard status  */
        XmString  clipLabel;
        Display  *dpy = XtDisplay(pDAS->dispWid);
        Window    win = XtWindow(pDAS->dispWid);
        char     *atomName;
        char     *tmpString;
        XTextProperty tmpProp;

        /*
         * get the selected text
         */
	_DtCanvasGetSelection (pDAS->canvas, 
				(_DtCvSELECTED_TEXT | _DtCvSELECTED_REGION),
						(_DtCvPointer *)(&tmpString));

        /*
         * Using the Xm clipboard facilities,
         * copy the selected text to the clipboard
         */
        if (tmpString != NULL)
	  {
            clipLabel = XmStringCreateLocalized ("DT_HELP");

            /*
	     * start copy to clipboard
	     */
            status = XmClipboardStartCopy(dpy, win, clipLabel,
					XtLastTimestampProcessed(dpy),
					pDAS->dispWid, NULL, &itemId);

	    /*
	     * no longer need the label
	     */
	    XmStringFree(clipLabel);

            if (status != ClipboardSuccess)
	      {
	        free(tmpString);
                return;
              }
   
	    status = XmbTextListToTextProperty(dpy, &tmpString, 1,
			                    (XICCEncodingStyle)XStdICCTextStyle,
					    &tmpProp);
	    /*
	     * free the original copy of the string and check the results.
	     */
	    free(tmpString);

            if (status != Success && status <= 0)
	      {
	        XmClipboardCancelCopy(dpy, win, itemId);
	        return;
              }

            atomName = XGetAtomName(dpy, tmpProp.encoding);

            /* move the data to the clipboard */
            status = XmClipboardCopy(dpy, win, itemId, atomName,
			          (XtPointer)tmpProp.value, tmpProp.nitems,
				  0, &dataId);

            XtFree(atomName);

            if (status != ClipboardSuccess)
	      {
	        XmClipboardCancelCopy(dpy, win, itemId);
                XFree((char*)tmpProp.value);
	        return;
	      }

            /*
	     * end the copy to the clipboard
	     */
            status = XmClipboardEndCopy (dpy, win, itemId);

            XFree((char*)tmpProp.value);
          }
      }

}  /* End _DtHelpInitiateClipboard */

/***************************************************************************
 * Function:  _DtHelpMoveBtnFocusCB
 *
 * _DtHelpMoveBtnFocusCB tracks the mouse movement for the Selection mechanism
 *
 **************************************************************************/
void 
_DtHelpMoveBtnFocusCB(
        Widget	   widget,
        XtPointer  client_data,
        XEvent	  *event )
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * if the policy is XmEXPLICIT, don't want the focus on the scrollbar
     */
    if (event->type == ButtonPress &&
	      _XmGetFocusPolicy(XtParent(XtParent(pDAS->dispWid))) != XmPOINTER)
	XmProcessTraversal(pDAS->dispWid, XmTRAVERSE_CURRENT);

}

/*****************************************************************************
 * Function: _DtHelpGetClearSelection
 *
 *  _DtHelpGetClearSelection - If this routine is called,
 *            the user has initiated a selection.
 *
 *****************************************************************************/
 void
_DtHelpGetClearSelection (
	Widget		widget,		/*  widget id               */
	XtPointer	client_data )	/*  data from applicaiton   */
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * If this widget doesn't own the primary selection, get it.
     */
    if (pDAS->primary != True)
      {
        if (XtOwnSelection (widget, XA_PRIMARY,
			XtLastTimestampProcessed(XtDisplay(widget)),
			(XtConvertSelectionProc) ConvertSelectionCB,
			(XtLoseSelectionProc) _DtHelpLoseSelectionCB,
			(XtSelectionDoneProc) NULL))
	  {
	    pDAS->primary = True;
	    pDAS->anchor_time = XtLastTimestampProcessed(XtDisplay(widget));
	  }
      }
    else
	_DtHelpClearSelection (client_data);

}  /* End _DtHelpGetClearSelection */
