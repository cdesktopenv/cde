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
/* $TOG: demo.c /main/5 1999/07/20 14:48:53 mgreess $ */
/*****************************************************************************
 *****************************************************************************
 **
 **   File:         demo.c
 **
 **   Description:  The Drag & Drop Demo program demonstrates the
 **		    CDE DnD functions in the Desktop Services library:
 **		  
 **		    	DtDndDragStart.3x
 **		    	DtDndDropRegister.3x
 **		    	DtDndCreateSourceIcon.3x
 **		  
 **		    The demo consists of a row of three different sources
 **		    for text, filename and appointment buffer drags.
 **		    It also has a text field that can accept either
 **		    text or filename drops.  Finally there is a data
 **		    area that accepts filename or buffer drops.
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <X11/Intrinsic.h>

#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/Frame.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MwmUtil.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/DragDrop.h>
#include <Xm/Screen.h>

#include <Dt/Dt.h>
#include <Dt/Dnd.h>

#include "icon.h"
#include "text.h"
#include "file.h"
#include "buff.h"

 /*************************************************************************
 *
 *	Data Structures & Declarations For Drag & Drop Demo
 *
 *************************************************************************/

/*
 * The Drag Threshold is the distance, measured in pixels, over which the
 * pointer must travel while the BSelect button (first mouse button) is held
 * down in order to start a drag. CDE defines this to be 10 pixels.
 */

#define DRAG_THRESHOLD 10

/*
 * Absolute value macro
 */

#ifndef ABS
#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#endif

/*
 * Global variables
 */

Widget		demoTopLevel;
XtAppContext	demoAppContext;
Boolean		demoDoingDrag = False;

/*
 * Private Drag & Drop Demo Function Declarations
 */

void		demoTransferCallback(Widget, XtPointer, XtPointer);

 /*************************************************************************
 *
 *	General-Purpose Drag & Drop Functions
 *
 *************************************************************************/

/*
 * demoDragFinishCallback
 *
 * Resets drag state to indicate the drag is over.
 */
void
demoDragFinishCallback(
	Widget		widget,
	XtPointer	clientData,
	XtPointer	callData)
{
	demoDoingDrag = False;
}

/*
 * demoDragMotionHandler
 *
 * Determine if the pointer has moved beyond the drag threshold while button 1
 * was being held down.
 */
void
demoDragMotionHandler(
	Widget		dragInitiator,
	XtPointer	clientData,
	XEvent	       *event)
{
	static int	initialX = -1;
	static int	initialY = -1;
	int		diffX, diffY;
	long		dragProtocol = (long)clientData;

	if (!demoDoingDrag) {

		/*
	 	 * If the drag is just starting, set initial button down coords
		 */

		if (initialX == -1 && initialY == -1) {
			initialX = event->xmotion.x;
			initialY = event->xmotion.y;
		}

		/*
		 * Find out how far pointer has moved since button press
		 */

		diffX = initialX - event->xmotion.x;
		diffY = initialY - event->xmotion.y;
		
		if ((ABS(diffX) >= DRAG_THRESHOLD) ||
		    (ABS(diffY) >= DRAG_THRESHOLD)) {
			demoDoingDrag = True;
			switch (dragProtocol) {
			case DtDND_TEXT_TRANSFER:
				textDragStart(dragInitiator, event);
				break;
			case DtDND_FILENAME_TRANSFER:
				fileCheckForDrag(dragInitiator, event,
					initialX, initialY);
				break;
			case DtDND_BUFFER_TRANSFER:
				apptDragStart(dragInitiator, event);
				break;
			}
			initialX = -1;
			initialY = -1;
		}
	}
}

/*
 * demoLookForButton
 */

/* ARGSUSED */
static Bool demoLookForButton(Display *d, XEvent *event, XPointer arg)
{
#define DAMPING 5
#define ABS_DELTA(x1, x2) (x1 < x2 ? x2 - x1 : x1 - x2)

    if( event->type == MotionNotify) 
    {
        XEvent * press = (XEvent *) arg;

        if (ABS_DELTA(press->xbutton.x_root, event->xmotion.x_root) > DAMPING ||
            ABS_DELTA(press->xbutton.y_root, event->xmotion.y_root) > DAMPING)
          return(True);
    }
    else if (event->type == ButtonRelease) return(True);

    return(False);
}

/*
 * demoProcessPress
 */

void demoProcessPress(Widget w, XEvent *event, String *parms, Cardinal *nparms)
{
#define SELECTION_ACTION        0
#define TRANSFER_ACTION         1

   int i, action, cur_item;
   int *selected_positions, nselected_positions;

   /* 
    * This action happens when Button1 is pressed and the Selection
    * and Transfer are integrated on Button1.  It is passed two
    * parameters: the action to call when the event is a selection,
    * and the action to call when the event is a transfer.
    */

    if (*nparms != 2 || !XmIsList(w)) return;

    action = SELECTION_ACTION;
    cur_item = XmListYToPos(w, event->xbutton.y);
    if (cur_item > 0)
    {
        XtVaGetValues(w,
		XmNselectedPositions, &selected_positions,
		XmNselectedPositionCount, &nselected_positions,
		NULL);

	for (i=0; i<nselected_positions; i++)
	{
	    if (cur_item == selected_positions[i])
	    {
                /*
		 * The determination of whether this is a transfer drag
		 * cannot be made until a Motion event comes in.  It is
		 * not a drag as soon as a ButtonUp event happens.
		 */
                XEvent new_event;
                
		XPeekIfEvent(
			XtDisplay(w),
			&new_event,
			&demoLookForButton,
			(XPointer) event);
                switch (new_event.type)
                {
                    case MotionNotify:
      	               action = TRANSFER_ACTION;
                       break;
                    case ButtonRelease:
        	       action = SELECTION_ACTION;
                       break;
                }
		break;
	    }
	}
    }

    XtCallActionProc(w, parms[action], event, parms, *nparms);
}

/*
 * demoDrawAnimateCallback
 *
 * Expands the icon melted into the draw area by Motif.
 */
void
demoDrawAnimateCallback(
	Widget		dragContext, /* WARNING: This is being destroyed. */
	XtPointer	clientData,
	XtPointer	callData)
{
	DtDndDropAnimateCallbackStruct *animateInfo =
				(DtDndDropAnimateCallbackStruct *) callData;
	Widget		dropDraw = (Widget)clientData;
	Display         *display = XtDisplayOfObject(dropDraw);
	Screen	        *screen	= XtScreen(dropDraw);
	Window          window = XtWindow(dropDraw);
	int		expandWidth, expandHeight,
			sourceX, sourceY;
	static GC	graphicsContext = NULL;
	XGCValues	gcValues;
	IconInfo        *iconPtr;
	
	/*
	 * Create graphics context if it doesn't yet exist
	 */

	if (graphicsContext == NULL) {
		gcValues.foreground = BlackPixelOfScreen(screen);
		gcValues.background = WhitePixelOfScreen(screen);
		graphicsContext = XCreateGC(display, window,
				GCForeground | GCBackground, &gcValues);
	}

	/*
	 * Get the dragged icon from the dropDraw area
	 */

	XtVaGetValues(dropDraw, XmNuserData, &iconPtr, NULL);

	if (iconPtr == NULL) {
		return;
	}

	/*
	 * Set clip mask and coordinates for this icon in the graphics context
	 */

	gcValues.clip_mask = iconPtr->mask;
	gcValues.clip_x_origin = iconPtr->icon.x;
	gcValues.clip_y_origin = iconPtr->icon.y;
	XChangeGC(display, graphicsContext,
			GCClipMask | GCClipXOrigin | GCClipYOrigin,
			&gcValues);

	/*
	 * Reconstitute the icon after Motif melts it
	 */

	for (expandWidth = expandHeight = 0;
	     expandWidth < (int)iconPtr->icon.width &&
	     	expandHeight < (int)iconPtr->icon.height;
	     expandWidth += 2, expandHeight += 2) {

		sourceX = ((int)iconPtr->icon.width - expandWidth)/2;
		sourceY = ((int)iconPtr->icon.height - expandHeight)/2;

		XCopyPlane(display, iconPtr->bitmap, window, graphicsContext,
			sourceX, sourceY, expandWidth, expandHeight,
			iconPtr->icon.x + sourceX, iconPtr->icon.y + sourceY,
			1L);

		XmeMicroSleep(25000L);
		XFlush(display);
	}
}

/*
 * demoDrawExposeCallback
 *
 * Draws all the icons associated with the drawing area.
 */
void
demoDrawExposeCallback(
	Widget		widget,
	XtPointer	clientData,
	XtPointer	callData)
{
	IconInfo       *iconPtr;

	XtVaGetValues(widget, XmNuserData, &iconPtr, NULL);

	while (iconPtr != NULL) {
		IconDraw(widget, iconPtr);
		iconPtr = iconPtr->next;
	}
}

/*
 * demoDropSetup
 *
 * Registers draw area to accept drops of files or buffers such as appointments.
 */
void
demoDropSetup(
	Widget		dropDraw)
{
	static XtCallbackRec transferCBRec[] = { {demoTransferCallback, NULL},
					         {NULL, NULL} };
	static XtCallbackRec animateCBRec[] = { {demoDrawAnimateCallback, NULL},
					        {NULL, NULL} };

	animateCBRec[0].closure = (XtPointer)dropDraw;

	DtDndVaDropRegister(dropDraw,
		DtDND_FILENAME_TRANSFER | DtDND_BUFFER_TRANSFER,
		XmDROP_COPY | XmDROP_MOVE, transferCBRec, 
		DtNdropAnimateCallback,		animateCBRec,
		DtNtextIsBuffer, 		True,
		DtNpreserveRegistration, 	False,
		NULL);
}

/*
 * demoTransferCallback
 *
 * Called when something is dropped on the drawing area drop site. If the
 * transfer protocol is DtDND_FILENAME_TRANSFER or DtDND_BUFFER_TRANSFER
 * the fileTransferCallback and apptTransferCallback are called respectively.
 */
void
demoTransferCallback(
        Widget          widget,
        XtPointer       clientData,
        XtPointer       callData)
{
        DtDndTransferCallbackStruct *transferInfo =
                                (DtDndTransferCallbackStruct *) callData;

	if (transferInfo == NULL) {
		return;
	}

	switch (transferInfo->dropData->protocol) {

	case DtDND_FILENAME_TRANSFER:
		fileTransferCallback(widget, clientData, callData);
		break;
	case DtDND_BUFFER_TRANSFER:
		apptTransferCallback(widget, clientData, callData);
		break;
	}
}

 /*************************************************************************
 *
 *	Demo Client Creation
 *
 *************************************************************************/

/*
 * Fallback resources are used if app-defaults file is not found
 */

String	fallbackResources[] = {
	"title:					CDE Drag & Drop Demo",

	"*outerRowColumn.orientation:		VERTICAL",
	"*outerRowColumn.spacing:		15",
	"*outerRowColumn.marginHeight:		15",
	"*outerRowColumn.marginWidth:		15",

	"*upperRowColumn.orientation:		HORIZONTAL",
	"*upperRowColumn.packing:		PACK_COLUMN",
	"*upperRowColumn.spacing:		15",
	"*upperRowColumn.marginHeight:		0",
	"*upperRowColumn.marginWidth:		0",

	"*fileDraw.height:			175",
	"*fileDraw.resizePolicy:		RESIZE_NONE",

	"*fruitList.listSizePolicy:		CONSTANT",
	"*fruitList.scrollBarDisplayPolicy:	STATIC",
	"*fruitList.selectionPolicy:		MULTIPLE_SELECT",

	"*apptList.listSizePolicy:		CONSTANT",
	"*apptList.scrollBarDisplayPolicy:	STATIC",
	"*apptList.selectionPolicy:		MULTIPLE_SELECT",

	"*textRowColumn.orientation:		HORIZONTAL",
	"*textRowColumn.packing:		PACK_TIGHT",
	"*textRowColumn*textLabel.labelString: Name:",
	"*textRowColumn*textField.width:	550",
	"*textRowColumn.marginWidth:		0",

	"*dropDraw.height:			100",
	"*dropDraw.resizePolicy:		RESIZE_NONE",
	NULL
};

/*
 * demoCreateDropSite
 *
 * Creates the drawing area at the bottom of the demo which is used as a drop
 * site for files and appointments.
 */
Widget
demoCreateDropSite(
	Widget		parent)
{
	Widget		dropFrame,
			dropDraw;

	dropFrame = XtVaCreateManagedWidget("dropFrame",
		xmFrameWidgetClass, parent,
		NULL);

	dropDraw = XtVaCreateManagedWidget("dropDraw",
		xmDrawingAreaWidgetClass, dropFrame,
		NULL);

	XtAddCallback(dropDraw,
		XmNexposeCallback, demoDrawExposeCallback,
		NULL);

	return dropDraw;
}

/*
 * main
 *
 * Create widgets for the demo program. Call protocol-specific functions
 * to initialize the drag sources and drop sites.
 */
void
main(
	int 		argc,
	String	       *argv)
{
	Widget		outerRowColumn,
			upperRowColumn,
			textDragSource,
			fileDragSource,
			apptDragSource,
			separator,
			textDropSite,
			drawDropSite;

	/*
	 * Create basic widgets and layout widgets
	 */

	XtSetLanguageProc(NULL, NULL, NULL);

	demoTopLevel = XtAppInitialize(&demoAppContext, "Dtdnddemo", 
		(XrmOptionDescList)NULL, 0, &argc, argv, 
		fallbackResources, (ArgList)NULL, 0);

	DtAppInitialize(demoAppContext, XtDisplay(demoTopLevel), 
		demoTopLevel, argv[0], "Dtdnddemo"); 

	outerRowColumn = XtVaCreateManagedWidget("outerRowColumn",
		xmRowColumnWidgetClass, demoTopLevel,
		NULL);

	upperRowColumn = XtVaCreateManagedWidget("upperRowColumn",
		xmRowColumnWidgetClass, outerRowColumn,
		NULL);

	/*
	 * Create the drag sources
	 */

	textDragSource = textCreateDragSource(upperRowColumn);
	fileDragSource = fileCreateDragSource(upperRowColumn);
	apptDragSource = apptCreateDragSource(upperRowColumn);

	/*
	 * Create a line separating the drag sources from the drop sites
	 */

	separator = XtVaCreateManagedWidget("separator",
		xmSeparatorWidgetClass, outerRowColumn,
		NULL);
	
	/*
	 * Create the drop sites
	 */

	textDropSite = textCreateDropSite(outerRowColumn);
	drawDropSite = demoCreateDropSite(outerRowColumn);

	/*
	 * Realize the widget tree
	 */

	XtRealizeWidget(demoTopLevel);

	/*
	 * Load the data typing database which is used to get icons
	 */

	DtDtsLoadDataTypes();

	/*
	 * Set up the drag sources
	 */

	textDragSetup(textDragSource);
	fileDragSetup(fileDragSource);
	apptDragSetup(apptDragSource);

	/*
	 * Set up the drop sites
	 */

	textDropSetup(textDropSite);
	demoDropSetup(drawDropSite);

	/*
	 * Start the event processing loop
	 */

	XtAppMainLoop(demoAppContext);
}
