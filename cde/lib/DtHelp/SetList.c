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
/* $XConsortium: SetList.c /main/16 1996/11/18 16:35:22 cde-hp $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	SetList.c
 **
 **   Project:     Text Graphic Display Library
 **
 **   Description: This body of code creates the Line and Graphic Tables for
 **		a Display Area.
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992,
                 1993, 1994, 1996 Hewlett-Packard Company.
 * (c) Copyright 1993, 1994, 1996 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1996 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/*
 * system includes
 */
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>


/*
 * Canvas Engine
 */
#include "CanvasP.h"
#include "CanvasSegP.h"

/*
 * private includes
 */
#include "Access.h"
#include "CleanUpI.h"
#include "DisplayAreaP.h"
#include "CallbacksI.h"
#include "FontAttrI.h"
#include "FormatI.h"
#include "SetListI.h"
#include "XInterfaceI.h"
#include "XUICreateI.h"

#ifdef NLS16
#include <nl_types.h>
#endif

/********    Private Function Declarations    ********/
/********    End Public Function Declarations    ********/

/******************************************************************************
 *
 * Private defines and variables
 *
 *****************************************************************************/

#define	GROW_SIZE	10
#define	ALL_LINES	-1

/******************************************************************************
 *
 * Private Macros
 *
 *****************************************************************************/
#define	NeedVertScrollbar(x) \
	_DtHelpIS_AS_NEEDED((x)->neededFlags,_DtHelpVERTICAL_SCROLLBAR)
#define	NeedHorzScrollbar(x) \
	_DtHelpIS_AS_NEEDED((x)->neededFlags,_DtHelpHORIZONTAL_SCROLLBAR)
/******************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************/
static void
SetMaxPositions (
	DtHelpDispAreaStruct	*pDAS,
	_DtCvUnit	 max_x,
	_DtCvUnit	 max_y)
{
    pDAS->maxX    = max_x;
    pDAS->maxYpos = max_y + pDAS->marginHeight;
}

/******************************************************************************
 *
 * Semi Public Functions - these routines called by other modules within
 *			   the library.
 *
 *****************************************************************************/
/******************************************************************************
 * Function: _DtHelpSetScrollBars
 *
 * Sets/Resets the vertical scroll bar values.
 *****************************************************************************/
enum   DoSomething  { DoNothing, DoManage, DoUnmanage };
Boolean
_DtHelpSetScrollBars (
	XtPointer	client_data,
	Dimension	 new_width,
	Dimension	 new_height)
{
    int    n = 0;
    int    dispN = 0;

    int    changeWidth = 0;
    int    changeHeight = 0;
    int    oldMaxYpos;

    int    slideSize;
    int    maximum;
    int    pageInc;
    int    oldScrollPercent = 10000;

    _DtCvUnit   ret_width;
    _DtCvUnit   ret_height;

    Arg    dispArgs[10];
    Arg    args[10];

    Dimension	 marginWidth;
    Dimension	 marginHeight;
    Dimension	 horzHeight;
    Dimension	 vertWidth;
    Dimension	 oldHeight;
    Dimension	 oldWidth;

    XRectangle	 rectangle[1];

    enum DoSomething doVert = DoNothing;
    enum DoSomething doHorz = DoNothing;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * remember the old first line.
     */
    oldMaxYpos = pDAS->maxYpos;
    if (pDAS->firstVisible && pDAS->maxYpos)
        oldScrollPercent = pDAS->firstVisible * 10000 / pDAS->maxYpos;

    /*
     * get the width and height of the scroll bars.
     */
    if (pDAS->vertScrollWid)
      {
        XtSetArg (args[0], XmNwidth , &vertWidth);
        XtGetValues (pDAS->vertScrollWid, args, 1);
      }

    if (pDAS->horzScrollWid)
      {
        XtSetArg (args[0], XmNheight , &horzHeight);
        XtGetValues (pDAS->horzScrollWid, args, 1);
      }

    /*
     * remember our old width and height
     */
    oldWidth  = pDAS->dispWidth;
    oldHeight = pDAS->dispHeight;

    /*
     * Settle the sizing issue before we remap the scroll bars.
     */
    do
      {
	/*
	 * set the height.
	 */
	if (new_height != pDAS->dispHeight)
	  {
	    pDAS->dispHeight    = new_height;
	    pDAS->dispUseHeight = new_height - 2 * pDAS->decorThickness;
	    if (((int) new_height) < 2 * ((int) pDAS->decorThickness))
	        pDAS->dispUseHeight = 0;

	    pDAS->visibleCount  = ((int) pDAS->dispUseHeight) /
				   pDAS->lineHeight;

	    if (pDAS->visibleCount < 0)
	        pDAS->visibleCount = 0;
	  }

	/*
	 * check to see if we've changed width
	 */
	if (new_width != pDAS->dispWidth)
	  {
	    /*
	     * set the new width
	     */
	    pDAS->dispWidth    = new_width;
	    pDAS->dispUseWidth = new_width - 2 * pDAS->decorThickness;
	    if (((int)new_width) < 2 * ((int) pDAS->decorThickness))
		pDAS->dispUseWidth = 0;

	    /*
	     * re-layout the information.
	     */
	    if (_DtCvSTATUS_OK ==
			_DtCanvasResize(pDAS->canvas, _DtCvFALSE,
						&ret_width, &ret_height))
	        SetMaxPositions (pDAS, ret_width, ret_height);

	  }

	/*
	 * Check to see if we have a vertical scrollbar and whether it
	 * is as_needed.  If static, it is always mapped.
	 */
	if (NULL != pDAS->vertScrollWid && NeedVertScrollbar(pDAS))
	  {
	    /*
	     * will the information fit on one page?
	     */
	    if (pDAS->maxYpos <= ((int) pDAS->dispUseHeight))
	      {
		/*
		 * If the window has not already been 'stretched' by
		 * the width of the scrollbar and if the scrollbar is
		 * mapped, grow the window width.
		 */
		if (0 == changeWidth && pDAS->vertIsMapped)
		  {
		    /*
		     * set the flag to indicate that the window width
		     * has grown. This can occur only if the window
		     * has its vertical scrollbar already mapped.
		     */
		    changeWidth = 1;
		    new_width += vertWidth;
		  }
		/*
		 * Else check to see if the window has been shrunk.
		 * If so, then stretch it back out to its original
		 * width.
		 *
		 * This can occur only if
		 *    a) The vertical scrollbar is not mapped.
		 *    b) The previous pass through this code indicates
		 *       that the window could not hold the length of
		 *       the information, and therefore calculations and
		 *       flags were set to map the vertical scrollbar.
		 *       But a _DtCanvasResize() now indicates that a
		 *       vertical scrollbar is not needed.  So we don't
		 *       need the vertical scrollbar and have to nuliify
		 *       the calculation to include it.
		 */
		else if (-1 == changeWidth)
		  {
		    changeWidth = 0;
		    new_width += vertWidth;
		  }
	      }
	    /*
	     * The information will not fit on one page.
	     */
	    else
	      {
		/*
		 * If the window has not already been shrunk to
		 * accomidate the vertical scrollbar (and the scrollbar
		 * is not already mapped) then set the flags and
		 * shrink the width.
		 */
	        if (False == pDAS->vertIsMapped && 0 == changeWidth)
		  {
		    /*
		     * set the flag to indicate that the window width
		     * has shrunk (and that consequently, the vertical
		     * scrollbar is not mapped).
		     */
		    changeWidth = -1;
		    if (new_width > vertWidth)
		        new_width -= vertWidth;
		    else
		        new_width = 0;
	          }
		/*
		 * Else check to see if the window has been streched.
		 * If so, then shrink it back out to its original
		 * width.
		 *
		 * This can occur only if
		 *    a) The vertical scrollbar is mapped.
		 *    b) The previous pass through this code indicates
		 *       that the window could hold the length of
		 *       the information, and therefore calculations and
		 *       flags were set to unmap the vertical scrollbar.
		 *       But a _DtCanvasResize() now indicates that a
		 *       vertical scrollbar is needed.  So we need
		 *       the vertical scrollbar and have to nuliify
		 *       the calculation to eliminate it.
		 */
	        else if (1 == changeWidth)
	          {
		    changeWidth = 0;
		    new_width  -= vertWidth;
	          }
	      }
	  }

	/*
	 * Check to see if we have a horizontal scrollbar and whether it
	 * is as_needed.  If static, it is always mapped.
	 */
	if (NULL != pDAS->horzScrollWid && NeedHorzScrollbar(pDAS))
	  {
	    /*
	     * will the information fit within the right border?
	     */
	    if (pDAS->maxX <= ((int) pDAS->dispUseWidth))
	      {
		/*
		 * If the window has not already been 'stretched' by
		 * the height of the scrollbar and if the scrollbar is
		 * mapped, grow the window.
		 */
	        if (0 == changeHeight && pDAS->horzIsMapped)
		  {
		    /*
		     * set the flag to indicate that the window has
		     * grown. This can occur only if the window has
		     * its horizontal scrollbar already mapped.
		     */
		    changeHeight = 1;
		    new_height += horzHeight;
		  }
		/*
		 * Else check to see if the window has been shrunk.
		 * If so, stretch it back to its original height.
		 *
		 * This can occur only if
		 *   a) The horizontal scrollbar is not mapped.
		 *   b) The previous pass through the code indicated
		 *      that the information exceeded the right border
		 *      and therefore calculations and flags were set
		 *      to map the horizontal scrollbar. But a
		 *      _DtCanvasResize() now indicates that the
		 *      horizontal scrollbar is not needed. So we
		 *      have to nullify the calculations and flags
		 *      set to include it.
		 */
		else if (-1 == changeHeight)
		  {
		    changeHeight = 0;
		    new_height += horzHeight;
		  }
	      }
	    /*
	     * The information exceeds the right border of the window.
	     */
	    else
	      {
		/*
	         * If the window has no already been shrunk to
		 * accomidate the horizontal scrollbar (and the
		 * scrollbar is not already mapped), then set the
		 * flags and shrink the height.
		 */
	        if (False == pDAS->horzIsMapped && 0 == changeHeight)
		  {
		    /*
		     * set the flag to indicate that the window height
		     * has shrunk (and that the scrollbar needs mapping).
		     */
	            changeHeight = -1;
		    if (new_height > horzHeight)
		        new_height -= horzHeight;
		    else
		        new_height = 0;
	          }
		/*
		 * Else check to see if the window has been streched.
		 * If so, then shrink it back out to its original
		 * height.
		 *
		 * This can occur only if
		 *   a) The horizontal scrollbar is mapped.
		 *   b) The previous pass through the code indicated
		 *      that the information did not exceeded the right
		 *      border and therefore calculations and flags were
		 *      set to unmap the horizontal scrollbar. But a
		 *      _DtCanvasResize() now indicates that the
		 *      horizontal scrollbar is needed. So we
		 *      have to nullify the calculations and flags
		 *      set to exclude it.
		 */
	        else if (1 == changeHeight)
	          {
		    changeHeight = 0;
		    new_height  -= horzHeight;
	          }
	      }
          }

      } while (new_height != pDAS->dispHeight || new_width != pDAS->dispWidth);

    /*
     * reset the first visible line, if needed.
     */
    if (oldScrollPercent != 10000 && pDAS->maxYpos != oldMaxYpos)
	pDAS->firstVisible = pDAS->maxYpos * oldScrollPercent / 10000;

    if (pDAS->firstVisible + ((int)pDAS->dispUseHeight) > pDAS->maxYpos)
	pDAS->firstVisible = pDAS->maxYpos - pDAS->dispUseHeight;

    if (pDAS->firstVisible < 0)
        pDAS->firstVisible = 0;

    /*
     * Reset the virtual X value, if needed
     */
    if (pDAS->virtualX &&
		(pDAS->maxX - pDAS->virtualX < ((int) pDAS->dispUseWidth)))
      {
	pDAS->virtualX = pDAS->maxX - pDAS->dispUseWidth;
	if (pDAS->virtualX < 0)
	    pDAS->virtualX = 0;
      }

    /*
     * If I've changed my size, I've got to change my clip masks.
     */
    if (oldWidth != pDAS->dispWidth || oldHeight != pDAS->dispHeight)
      {
	/*
	 * If the display area hasn't been realized, the resetting of
	 * the bottom right corner offsets will not cause a resize
	 * event to occur. Rather the overall window will grow by
	 * the width or height of the scroll bar and not cause the
	 * display area to shrink. Therefore when the exposure event
	 * is called the width or height will be not be equal to what
	 * I think it should be causing the code to believe a resize
	 * is in process. The short of the matter is that the exposure
	 * will not happen and we end up with a blank window.
	 *
	 * So for this case, resize the display area to what we think it
	 * should be and the exposure event will then happen.
	if (!XtIsRealized (pDAS->dispWid))
	 */
          {
	    XtSetArg(dispArgs[dispN], XmNwidth, pDAS->dispWidth);    ++dispN;
	    XtSetArg(dispArgs[dispN], XmNheight, pDAS->dispHeight);  ++dispN;
          }

	rectangle[0].x      = pDAS->decorThickness;
	rectangle[0].y      = pDAS->decorThickness;
	rectangle[0].width  = pDAS->dispUseWidth;
	rectangle[0].height = pDAS->dispUseHeight;

	XSetClipRectangles(XtDisplay(pDAS->dispWid), pDAS->normalGC, 0, 0,
					rectangle, 1, Unsorted);
	XSetClipRectangles(XtDisplay(pDAS->dispWid), pDAS->invertGC, 0, 0,
					rectangle, 1, Unsorted);

	if (pDAS->resizeCall)
	    (*(pDAS->resizeCall)) (pDAS->clientData);
      }

    /*
     * Has the width changed? If so, then we are mapping or unmapping
     * the vertical scrollbar.
     */
    if (-1 == changeWidth)
      {
	/*
	 * manage the scrollbar, the window has shrunk.
	 */
	doVert = DoManage;

	XtSetArg(dispArgs[dispN], XmNrightAttachment, XmATTACH_WIDGET);
				++dispN;
	XtSetArg(dispArgs[dispN], XmNrightWidget, pDAS->vertScrollWid);
				++dispN;
      }
    else if (1 == changeWidth)
      {
	/*
	 * unmanage the scrollbar, the window has grown.
	 */
	doVert = DoUnmanage;

	XtSetArg(dispArgs[dispN], XmNrightAttachment, XmATTACH_FORM);
				++dispN;
      }

    /*
     * Has the height changed? If so, then we are mapping or unmapping
     * the horizontal scrollbar.
     */
    if (-1 == changeHeight)
      {
	/*
	 * manage the scrollbar, the window has shrunk.
	 */
	doHorz = DoManage;

        XtSetArg(dispArgs[dispN], XmNbottomAttachment,XmATTACH_WIDGET);
				++dispN;
        XtSetArg(dispArgs[dispN], XmNbottomWidget,pDAS->horzScrollWid);
				++dispN;
      }
    else if (1 == changeHeight)
      {
	/*
	 * unmanage the scrollbar, the window has grown.
	 */
	doHorz = DoUnmanage;

	XtSetArg(dispArgs[dispN], XmNbottomAttachment, XmATTACH_FORM);
				++dispN;
      }

    /*
     * get the margins, widths and height.
     */
    XtSetArg (args[0], XmNmarginWidth , &marginWidth);
    XtSetArg (args[1], XmNmarginHeight, &marginHeight);
    XtGetValues (XtParent (pDAS->dispWid), args, 2);

    /*
     * have to map the scroll bars before the drawn button can be
     * attached to them and can't unmap the scroll bars until
     * the drawn button has been unattached from them. But we
     * want to do the re-attachments all at once to minimize the
     * resizing.
     */
    if (doHorz == DoManage)
      {
	if (!XtIsRealized(pDAS->horzScrollWid))
	    XtSetMappedWhenManaged (pDAS->horzScrollWid, True);
	else
	    XtMapWidget (pDAS->horzScrollWid);
	pDAS->horzIsMapped = True;
      }

    if (doVert == DoManage)
      {
	if (!XtIsRealized(pDAS->vertScrollWid))
	    XtSetMappedWhenManaged (pDAS->vertScrollWid, True);
	else
	    XtMapWidget (pDAS->vertScrollWid);
	pDAS->vertIsMapped = True;
      }

    if (pDAS->vertScrollWid && pDAS->vertIsMapped && doVert != DoUnmanage)
      {
	if (pDAS->horzScrollWid && pDAS->horzIsMapped && doHorz != DoUnmanage)
	    XtSetArg(args[0], XmNbottomOffset, (marginHeight + horzHeight));
	else
	    XtSetArg(args[0], XmNbottomOffset, marginHeight);

	n = 1;

	/*
	 * set the slider size.
	 */
	slideSize = pDAS->dispUseHeight;
	if (slideSize < 1)
    	slideSize = 1;
    
	/*
	 * determine the maximum size
	 */
	maximum = pDAS->maxYpos;

	if (maximum < slideSize)
    	maximum = slideSize;

	if (maximum < 1)
    	maximum = 1;
    
	/*
	 * determine the page increment.
	 */
	pageInc = 1;
	if (((int)pDAS->dispUseHeight) - pDAS->lineHeight > 2)
    	pageInc = pDAS->dispUseHeight - pDAS->lineHeight;

	XtSetArg (args[n], XmNvalue        , pDAS->firstVisible);	++n;
	XtSetArg (args[n], XmNsliderSize   , slideSize);		++n;
	XtSetArg (args[n], XmNmaximum      , maximum);			++n;
	XtSetArg (args[n], XmNpageIncrement, pageInc);			++n;
	XtSetValues (pDAS->vertScrollWid, args, n);
      }

    if (pDAS->horzScrollWid && pDAS->horzIsMapped && doHorz != DoUnmanage)
      {
	if (pDAS->vertScrollWid && pDAS->vertIsMapped && doVert != DoUnmanage)
	    XtSetArg(args[0], XmNrightOffset, (marginWidth + vertWidth));
	else
	    XtSetArg(args[0], XmNrightOffset, marginWidth);

	n = 1;
	/*
	 * determine the slider size.
	 */
	slideSize = pDAS->dispUseWidth - 1;
	if (slideSize < 1)
    	slideSize = 1;

	/*
	 * determine the maximum size
	 */
	maximum = slideSize;
	if (maximum < pDAS->maxX)
    	maximum = pDAS->maxX - 1;

	if (maximum < 1)
    	maximum = 1;

	/*
	 * determine the page increment.
	 */
	pageInc = 1;
	if (((int) pDAS->dispUseWidth) > 2)
    	pageInc = pDAS->dispUseWidth - 1;

	XtSetArg (args[n], XmNvalue        , pDAS->virtualX);   n++;
	XtSetArg (args[n], XmNsliderSize   , slideSize);        n++;
	XtSetArg (args[n], XmNmaximum      , maximum);          n++;
	XtSetArg (args[n], XmNpageIncrement, pageInc);		++n;
	XtSetValues (pDAS->horzScrollWid, args, n);
      }

    /*
     * set the display area args
     */
    if (dispN)
      XtSetValues (pDAS->dispWid, dispArgs, dispN);

    /*
     * unmap those we don't want.
     */
    if (doHorz == DoUnmanage)
      {
	if (!XtIsRealized(pDAS->horzScrollWid))
	    XtSetMappedWhenManaged (pDAS->horzScrollWid, False);
	else
	    XtUnmapWidget (pDAS->horzScrollWid);
	pDAS->horzIsMapped = False;
      }

    if (doVert == DoUnmanage)
      {
	if (!XtIsRealized(pDAS->vertScrollWid))
	    XtSetMappedWhenManaged (pDAS->vertScrollWid, False);
	else
	    XtUnmapWidget (pDAS->vertScrollWid);
	pDAS->vertIsMapped = False;
      }

    /*
     * return whether or not we generated a resize (and hence an expose
     * event) by resizing the display area.
     */
    return (dispN ? True : False);

} /* End _DtHelpSetScrollBars */

/******************************************************************************
 *
 * Public Functions - those an application developer can call.
 *
 *****************************************************************************/

/*********************************************************************
 * Function: SetList
 *
 *    SetList creates a Text Graphic area with the appropriate scroll bars.
 *
 *********************************************************************/
void
_DtHelpDisplayAreaSetList (
    XtPointer	client_data,
    XtPointer	topic_handle,
    Boolean	append_flag,
    int		scroll_percent)
{
    _DtCvUnit         width;
    _DtCvUnit	      height;
    _DtCvUnit	      scrollY;
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    /*
     * clear the selection
     */
    _DtHelpClearSelection (client_data);

    /*
     * If there is anything to do
     */
    _DtCanvasSetTopic (pDAS->canvas, (_DtCvTopicPtr) topic_handle,
						pDAS->honor_size,
						&width, &height, &scrollY);

    /*
     * free the old topic and remember the new
     */
    _DtHelpDestroyTopicData(pDAS->lst_topic, _DtHelpDADestroyRegion,
						(_DtCvPointer) pDAS);

    pDAS->lst_topic = (_DtCvTopicPtr) topic_handle;

    /*
     * do we want to scroll?
     */
    if (scroll_percent != -1)
	scrollY = scroll_percent * height / 1000;

    /*
     * Reset the scroll bars and perhaps do an expose.
     */
    SetMaxPositions (pDAS, width, height);

    pDAS->firstVisible = scrollY;

    if (pDAS->firstVisible + ((int)pDAS->dispUseHeight) > pDAS->maxYpos)
	pDAS->firstVisible = pDAS->maxYpos - pDAS->dispUseHeight;

    if (pDAS->firstVisible < 0)
        pDAS->firstVisible = 0;

    (void) _DtHelpSetScrollBars (client_data, pDAS->dispWidth, pDAS->dispHeight);
    if (XtIsRealized (pDAS->dispWid))
	_DtHelpCleanAndDrawWholeCanvas(client_data);

}  /* End _DtHelpDisplayAreaSetList */

/*****************************************************************************
 * Function: void _DtHelpDisplayAreaDimensionsReturn (DtHelpDispAreaStruct *pDAS,
 *					int *ret_width,	int *ret_height)
 *
 * Parameters	pDAS		Specifies the Display Area.
 *		ret_rows	Returns the number of rows in
 *				the Display Area.
 *		ret_columns	Returns the number of columns in
 *				the Display Area.
 *
 * Returns:	nothing
 *
 * Purpose:	Allows access to the height and width of a Display Area.
 *
 *****************************************************************************/
void
_DtHelpDisplayAreaDimensionsReturn (
    XtPointer	 client_data,
    short	*ret_rows,
    short	*ret_columns )
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    *ret_rows = ((int)pDAS->dispUseHeight) / pDAS->lineHeight;
    *ret_columns = ((int) pDAS->dispUseWidth) / (pDAS->charWidth / 10);

}  /* End _DtHelpDisplayAreaDimensionsReturn */

/*****************************************************************************
 * Function: int _DtHelpGetScrollbarValue (DtHelpDispAreaStruct *pDAS)
 *
 * Parameters	pDAS		Specifies the Display Area.
 *
 * Returns:	a value from -1 to 100.
 *		-1 means the vertical scrollbar was not created when
 *		the display area was created.
 *		0 to 100 is the percentage the vertical scroll bar is
 *		scrolled displaying the current topic.
 *
 * Purpose:	Allows the system to query the location of the scrollbar
 *		and remember for smart-backtracking.
 *
 *****************************************************************************/
int
_DtHelpGetScrollbarValue (XtPointer client_data)
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

    int  percent = -1;

    if (pDAS->vertScrollWid != NULL)
      {
	percent = 0;
	if (pDAS->firstVisible && pDAS->maxYpos)
	    percent = (pDAS->firstVisible * 1000) / pDAS->maxYpos;
      }

    return percent;

}  /* End _DtHelpGetScrollbarValue */

/*****************************************************************************
 * Function: int _DtHelpUpdatePath (DtHelpDispAreaStruct *pDAS,
 *
 * Parameters	pDAS		Specifies the Display Area.
 *
 * Returns:	a value from -1 to 100.
 *		-1 means the vertical scrollbar was not created when
 *		the display area was created.
 *		0 to 100 is the percentage the vertical scroll bar is
 *		scrolled displaying the current topic.
 *
 * Purpose:	Allows the system to query the location of the scrollbar
 *		and remember for smart-backtracking.
 *
 *****************************************************************************/
int
_DtHelpUpdatePath (
    DtHelpDispAreaStruct	*pDAS,
    _DtHelpVolumeHdl		 volume_handle,
    char			*loc_id)
{
    int		 result;
    char	*realId;
    _DtCvUnit	width;
    _DtCvUnit	height;
    _DtCvUnit	tocY;
    _DtCvUnit	tocHeight;
    XtPointer	topic_handle;

    /*
     * turn off the toc marker
     */
    _DtHelpDATocMarker(pDAS, False);
    (void) _DtCanvasMoveTraversal(pDAS->canvas, _DtCvTRAVERSAL_OFF, False,
			((_DtCvValue) XtIsRealized(pDAS->dispWid)),
			NULL, NULL, NULL, NULL, NULL);

    /*
     * now create and set the topic
     */
    result = _DtHelpFormatToc(pDAS,volume_handle,loc_id,&realId,&topic_handle);
    if (-1 == result)
	return -1;

    _DtCanvasSetTopic (pDAS->canvas, (_DtCvTopicPtr) topic_handle,
						pDAS->honor_size,
						&width, &height, NULL);

    /*
     * free the old topic and remember the new
     */
    _DtHelpDestroyTopicData(pDAS->lst_topic, _DtHelpDADestroyRegion,
						(_DtCvPointer) pDAS);

    pDAS->lst_topic = (_DtCvTopicPtr) topic_handle;

    /*
     * Reset the scroll bars and perhaps do an expose.
     */
    SetMaxPositions (pDAS, width, height);

    /*
     * now turn the traversal on - but only if there is a path in
     * the TOC.
     */
    if (1 != result)
      {
        (void) _DtCanvasMoveTraversal(pDAS->canvas, _DtCvTRAVERSAL_ID, False,
		((_DtCvStatus) XtIsRealized(pDAS->dispWid)), realId,
		NULL, &(pDAS->toc_y), &(pDAS->toc_base), &(pDAS->toc_height));

        /*
         * figure out where the first line should be...
         */
        tocY      = pDAS->toc_y - pDAS->lineThickness;
        tocHeight = pDAS->toc_height + (2 * pDAS->lineThickness);

        if (tocY < pDAS->firstVisible ||
	    tocY + tocHeight > pDAS->firstVisible + ((int)pDAS->dispUseHeight))
          {
	    pDAS->firstVisible  = tocY;
	    if (pDAS->firstVisible + ((int)pDAS->dispUseHeight) > pDAS->maxYpos)
	        pDAS->firstVisible = pDAS->maxYpos - pDAS->dispUseHeight;

	    if (pDAS->firstVisible < 0)
	        pDAS->firstVisible = 0;
          }
      }

    /*
     * Reset the scroll bars and perhaps do an expose.
     */
    (void) _DtHelpSetScrollBars((XtPointer) pDAS, pDAS->dispWidth, pDAS->dispHeight);
    if (XtIsRealized (pDAS->dispWid))
	_DtHelpCleanAndDrawWholeCanvas((XtPointer) pDAS);

    if (1 != result)
	_DtHelpDATocMarker((XtPointer) pDAS, True);

    return 0;

}  /* End _DtHelpUpdatePath */

/*****************************************************************************
 * Function:    Widget _DtHelpDisplayAreaWidget ();
 *
 * Parameters:
 *              client_data     Specifies the display area pointer.
 *
 * Returns: the widget associated with the display area.
 *
 * Purpose:
 *
 *****************************************************************************/
Widget
_DtHelpDisplayAreaWidget (
    XtPointer    client_data)
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

   return (pDAS->dispWid);
}

/*****************************************************************************
 * Function:    XtPointer _DtHelpDisplayAreaData ();
 *
 * Parameters:
 *              client_data     Specifies the display area pointer.
 *
 * Returns: the client data associated with the display area.
 *
 * Purpose:
 *
 *****************************************************************************/
XtPointer
_DtHelpDisplayAreaData (
    XtPointer    client_data)
{
    DtHelpDispAreaStruct *pDAS = (DtHelpDispAreaStruct *) client_data;

   return (pDAS->clientData);
}
