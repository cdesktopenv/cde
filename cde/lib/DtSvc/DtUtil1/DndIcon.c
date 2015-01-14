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
/* $XConsortium: DndIcon.c /main/5 1996/06/21 17:28:34 ageorge $ */
/*********************************************************************
 *
 *	File:		DndIcon.c
 *
 *	Description:	Implemenation of DND drag icons
 *
 *********************************************************************
 *
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This documment and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company
 * (c) Copyright 1993, 1994 International Business Machines Corp.
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994 Novell, Inc.
 *
 *+ENOTICE
 */

#include <stdlib.h>
#include <X11/Xlib.h> 
#include <X11/Intrinsic.h> 
#include <Xm/Screen.h> 
#include <Xm/DragIcon.h> 
#include "Dnd.h"
#include "DndP.h"
#include "DndIconI.h"
#include "DtSvcLock.h"

/*
 * Component types of a drag icon (source/state/operation)
 */

enum {
	DtDND_DRAG_SOURCE,
	DtDND_DRAG_STATE,
	DtDND_DRAG_OPERATION
};

/*
 * The XmDragIcon widgets for the state & operation components
 * of a Drag Icon
 */

typedef struct _DragIcons {
	Widget		validIcon, invalidIcon, noneIcon;
	Widget		moveIcon, copyIcon, linkIcon;
} DragIcons;

/*
 * Collection of drag icons for text & data drags
 */

typedef struct _DragCollection {
	DtDndDragSource	sourceType;
	DragIcons	text;
	DragIcons	data;
	Widget		textSource;
	Widget		dataSource;
	Widget		multipleSource;
} DragCollection;

/*
 * The context that the DragCollection is stored against.
 * Used in getDragCollection() and destroyDragCollection().
 */

static	XContext	dragCollectionContext;

/*
 * Creates an XmDragIcon for use as the source component of the drag icon.
 */
Widget
DtDndCreateSourceIcon(
	Widget		widget,
	Pixmap		pixmap,
	Pixmap		mask)
{
	Widget		dragIcon;
	Window	        rootWindow;
	int		pixmapX, pixmapY;
	unsigned int	pixmapWidth, pixmapHeight, pixmapBorder, pixmapDepth;
	Arg		args[20];
	Cardinal	nn = 0;
	_DtSvcWidgetToAppContext(widget);

	_DtSvcAppLock(app);

	while (XmIsGadget(widget)) {
		widget = XtParent(widget);
	}
 
	XGetGeometry (XtDisplayOfObject(widget), pixmap, &rootWindow,
		&pixmapX, &pixmapY, &pixmapWidth, &pixmapHeight,
		&pixmapBorder, &pixmapDepth);
 
	XtSetArg(args[nn], XmNwidth, pixmapWidth);  nn++;
	XtSetArg(args[nn], XmNheight, pixmapHeight);  nn++;
	XtSetArg(args[nn], XmNmaxWidth, pixmapWidth);  nn++;
	XtSetArg(args[nn], XmNmaxHeight, pixmapHeight);  nn++;
	XtSetArg(args[nn], XmNpixmap, pixmap);  nn++;
	XtSetArg(args[nn], XmNmask, mask);  nn++;
	XtSetArg(args[nn], XmNdepth, pixmapDepth);  nn++;
	dragIcon = XmCreateDragIcon(widget, "sourceIcon", args, nn);
 
	_DtSvcAppUnlock(app);
	return(dragIcon);
}

/*
 * Calculates the x,y offsets for the state cursor relative to the source
 */
static void
calcStateIconOffset(
	Widget		sourceIcon,
	int		*stateOffsetX,
	int		*stateOffsetY)
{
	Display		*dpy 	= XtDisplayOfObject(sourceIcon);
	Pixmap		pixmap;
	Window		root;
	int		pixmapX, pixmapY;
	unsigned int	pixmapW, pixmapH, junk;

	XtVaGetValues(sourceIcon, XmNpixmap, &pixmap, NULL);

	if (pixmap == None)
		return;

	XGetGeometry(dpy, pixmap, &root,
		&pixmapX, &pixmapY, &pixmapW, &pixmapH,
		&junk, &junk);

	if (pixmapH == 16) {
		*stateOffsetX = *stateOffsetY = 8;
	} else {
		*stateOffsetX = *stateOffsetY = 16;
	}
}

/*
 * Selects or activates the request type of source by setting
 * the default drag icon resources on the XmScreen widget.
 */
static void
selectDragSource(
	DragCollection	*dc,
	Widget		xmScreen,
	DtDndDragSource	sourceType,
	Widget		sourceIcon)
{
	Display		*dpy 		= XtDisplayOfObject(xmScreen);
	Widget 		validIcon, invalidIcon, noneIcon,
			moveIcon, copyIcon, linkIcon,
			defSourceIcon;
	int		hotX, hotY,
			stateOffsetX, stateOffsetY,
			offsetDeltaX, offsetDeltaY;
	Arg		args[30];
	Cardinal	nn = 0;

	/*
	 * Choose the state/operation/default-source drag icons based
	 * on the requested drag-source-type
	 */
	switch (sourceType) {
	case DtDND_DRAG_SOURCE_TEXT:
		validIcon	= dc->text.validIcon;
		invalidIcon	= dc->text.invalidIcon;
		noneIcon	= dc->text.noneIcon;
		moveIcon	= dc->text.moveIcon;
		copyIcon	= dc->text.copyIcon;
		linkIcon	= dc->text.linkIcon;
		defSourceIcon	= dc->textSource;
		hotX		= text_x_hot;
		hotY		= text_y_hot;
		stateOffsetX	= text_x_offset_state;
		stateOffsetY	= text_y_offset_state;
		offsetDeltaX	= text_x_offset_delta;
		offsetDeltaY	= text_y_offset_delta;
		break;
	case DtDND_DRAG_SOURCE_DATA:
		validIcon	= dc->data.validIcon;
		invalidIcon	= dc->data.invalidIcon;
		noneIcon	= dc->data.noneIcon;
		moveIcon	= dc->data.moveIcon;
		copyIcon	= dc->data.copyIcon;
		linkIcon	= dc->data.linkIcon;
		defSourceIcon	= dc->dataSource;
		hotX		= data_x_hot;
		hotY		= data_y_hot;
		stateOffsetX	= data_x_offset_state;
		stateOffsetY	= data_y_offset_state;
		offsetDeltaX	= data_x_offset_delta;
		offsetDeltaY	= data_y_offset_delta;
		break;
	case DtDND_DRAG_SOURCE_MULTIPLE:
		validIcon	= dc->data.validIcon;
		invalidIcon	= dc->data.invalidIcon;
		noneIcon	= dc->data.noneIcon;
		moveIcon	= dc->data.moveIcon;
		copyIcon	= dc->data.copyIcon;
		linkIcon	= dc->data.linkIcon;
		defSourceIcon	= dc->multipleSource;
		hotX		= data_x_hot;
		hotY		= data_y_hot;
		stateOffsetX	= data_x_offset_state;
		stateOffsetY	= data_y_offset_state;
		offsetDeltaX	= data_x_offset_delta;
		offsetDeltaY	= data_y_offset_delta;
		break;
	default:
	case DtDND_DRAG_SOURCE_DEFAULT:
		validIcon	= NULL;
		invalidIcon	= NULL;
		noneIcon	= NULL;
		moveIcon	= NULL;
		copyIcon	= NULL;
		linkIcon	= NULL;
		defSourceIcon	= NULL;
		hotX		= motif_x_hot;
		hotY		= motif_y_hot;
		stateOffsetX	= motif_x_offset_state;
		stateOffsetY	= motif_y_offset_state;
		offsetDeltaX	= motif_x_offset_delta;
		offsetDeltaY	= motif_y_offset_delta;
		break;
	}

	/*
 	 * Calculate the offsets of the state icon from the 
	 * DtDND_DRAG_SOURCE_DATA source icon if specified
	 */
	if (sourceIcon && sourceType == DtDND_DRAG_SOURCE_DATA) {
		calcStateIconOffset(sourceIcon, &stateOffsetX, &stateOffsetY);
	}

	nn = 0;
	XtSetArg(args[nn], XmNhotX, 	hotX);			nn++;
	XtSetArg(args[nn], XmNhotY, 	hotY);			nn++;
	XtSetArg(args[nn], XmNoffsetX,	stateOffsetX);		nn++;
	XtSetArg(args[nn], XmNoffsetY,	stateOffsetY);		nn++;

	if (validIcon) 
		XtSetValues(validIcon, args, nn);
	if (invalidIcon)
		XtSetValues(invalidIcon, args, nn);
	if (noneIcon)
		XtSetValues(noneIcon, args, nn);

	nn = 0;
	XtSetArg(args[nn], XmNoffsetX,	stateOffsetX + offsetDeltaX);	nn++;
	XtSetArg(args[nn], XmNoffsetY,	stateOffsetY + offsetDeltaY);	nn++;

	if (moveIcon)
		XtSetValues(moveIcon, args, nn);
	if (copyIcon)
		XtSetValues(copyIcon, args, nn);
	if (linkIcon)
		XtSetValues(linkIcon, args, nn);

	/*
	 * Set the default drag cursor icons for the XmScreen widget
	 */
	XtVaSetValues(xmScreen,
		XmNdefaultValidCursorIcon, 	validIcon,
		XmNdefaultInvalidCursorIcon, 	invalidIcon,
		XmNdefaultNoneCursorIcon, 	noneIcon,
		XmNdefaultMoveCursorIcon, 	moveIcon,
		XmNdefaultCopyCursorIcon, 	copyIcon,
		XmNdefaultLinkCursorIcon, 	linkIcon,
		XmNdefaultSourceCursorIcon, 	defSourceIcon,
		NULL);

	dc->sourceType = sourceType;
}

/*
 * Create a drag icon widget from the passed bits
 */
static Widget
makeDragIconFromData(
	Widget		widget,
	unsigned char	*bits,
	unsigned char	*maskbits,
	int		width,
	int		height,
	int		component) 
{
	Widget		icon;
	Pixmap		pixmap, mask;
	Display		*dpy 		= XtDisplayOfObject(widget);
	Screen		*screen		= XtScreenOfObject(widget);
	Window		root 		= RootWindowOfScreen(screen);
	String		name;
	Arg		args[30];
	Cardinal	nn = 0;

	pixmap = XCreatePixmapFromBitmapData(dpy, root,
		(char *)bits, width, height,
		1, 0, 1);

	mask = XCreatePixmapFromBitmapData(dpy, root,
		(char *)maskbits, width, height,
		1, 0, 1);

	switch (component) {
	case DtDND_DRAG_SOURCE:
		name = "sourceIcon";
		break;
	case DtDND_DRAG_STATE:
		name = "stateIcon";
		XtSetArg(args[nn], XmNattachment, XmATTACH_NORTH_WEST);	nn++;
		XtSetArg(args[nn], XmNoffsetX, 	  -6);			nn++;
		XtSetArg(args[nn], XmNoffsetY,    -6);			nn++;
		XtSetArg(args[nn], XmNhotX,       3);			nn++;
		XtSetArg(args[nn], XmNhotY,       3);			nn++;
		break;
	case DtDND_DRAG_OPERATION:
		name = "operationIcon";
		XtSetArg(args[nn], XmNattachment, XmATTACH_NORTH_WEST);	nn++;
		XtSetArg(args[nn], XmNoffsetX, 	  1);			nn++;
		XtSetArg(args[nn], XmNoffsetY,    1);			nn++;
		break;
	}

	XtSetArg(args[nn], XmNwidth,		width);			nn++;
	XtSetArg(args[nn], XmNheight,		height);		nn++;
	XtSetArg(args[nn], XmNmaxWidth,	 	width);  		nn++;
	XtSetArg(args[nn], XmNmaxHeight,	height);  		nn++;
	XtSetArg(args[nn], XmNpixmap, 		pixmap);  		nn++;
	XtSetArg(args[nn], XmNmask, 		mask);			nn++;
	XtSetArg(args[nn], XmNdepth, 		1);			nn++;

	icon = XmCreateDragIcon(widget, name, args, nn);

	return icon;
}

/*
 * Create the collection of text & data drag icons; state/operation
 * cursors and source icons.
 */
static DragCollection *
makeDragCollection(
	Widget		widget)
{
	DragCollection	*dc		= XtNew(DragCollection);
	Display		*dpy 		= XtDisplayOfObject(widget);
	Screen		*screen		= XtScreenOfObject(widget);
	Window		root 		= RootWindowOfScreen(screen);
	Widget		xmScreen 	= XmGetXmScreen(screen);

	/*
	 * Text & Data State Cursors
	 */

	dc->text.invalidIcon = makeDragIconFromData(widget, 
		text_invalid_bits, text_invalid_m_bits, 
		text_invalid_width, text_invalid_height, 
		DtDND_DRAG_STATE);

	dc->text.validIcon = makeDragIconFromData(widget, 
		text_valid_bits, text_valid_m_bits, 
		text_valid_width, text_valid_height, 
		DtDND_DRAG_STATE);

	dc->text.noneIcon = makeDragIconFromData(widget, 
		text_none_bits, text_none_m_bits, 
		text_none_width, text_none_height, 
		DtDND_DRAG_STATE);

	dc->data.invalidIcon = makeDragIconFromData(widget, 
		data_invalid_bits, data_invalid_m_bits, 
		data_invalid_width, data_invalid_height, 
		DtDND_DRAG_STATE);

	dc->data.validIcon = makeDragIconFromData(widget, 
		data_valid_bits, data_valid_m_bits, 
		data_valid_width, data_valid_height, 
		DtDND_DRAG_STATE);

	dc->data.noneIcon = makeDragIconFromData(widget, 
		data_none_bits, data_none_m_bits, 
		data_none_width, data_none_height, 
		DtDND_DRAG_STATE);

	/*
	 * Text & Data Operation Cursors
	 */

	dc->data.moveIcon = makeDragIconFromData(widget,
		data_move_bits, data_move_m_bits,
		data_move_width, data_move_height,
		DtDND_DRAG_OPERATION);

	dc->data.copyIcon = makeDragIconFromData(widget,
		data_copy_bits, data_copy_m_bits,
		data_copy_width, data_copy_height,
		DtDND_DRAG_OPERATION);

	dc->data.linkIcon = makeDragIconFromData(widget,
		data_link_bits, data_link_m_bits,
		data_link_width, data_link_height,
		DtDND_DRAG_OPERATION);

	dc->text.moveIcon = dc->data.moveIcon;
	dc->text.copyIcon = dc->data.copyIcon;
	dc->text.linkIcon = dc->data.linkIcon;

	/*
	 * Text & Data Source Cursors
	 */

	dc->textSource = makeDragIconFromData(widget, 
		text_source_bits, text_source_m_bits, 
		text_source_width, text_source_height, 
		DtDND_DRAG_SOURCE);

	dc->dataSource = makeDragIconFromData(widget, 
		data_single_bits, data_single_m_bits, 
		data_single_width, data_single_height, 
		DtDND_DRAG_SOURCE);

	dc->multipleSource = makeDragIconFromData(widget, 
		data_multiple_bits, data_multiple_m_bits, 
		data_multiple_width, data_multiple_height, 
		DtDND_DRAG_SOURCE);

	dc->sourceType = DtDND_DRAG_SOURCE_DEFAULT;

	return dc;
}

/*
 * Destroys the cached DragCollection and context for it
 */
static void
destroyDragCollection(
	Widget		widget,
	XtPointer	clientData,
	XtPointer	callData)
{
	DragCollection	*dc	= (DragCollection *)clientData;

	XtFree((char *)dc);

	XDeleteContext(XtDisplayOfObject(widget), 
		RootWindowOfScreen(XtScreenOfObject(widget)),
		dragCollectionContext);
}

/*
 * Returns the cached DragCollection; will create one if needed
 */
static DragCollection *
getDragCollection(
	Widget		xmScreen)
{
	DragCollection	*dc     = NULL;
	Display		*dpy	= XtDisplayOfObject(xmScreen);
	Screen		*screen	= XtScreenOfObject(xmScreen);
	Window		root	= RootWindowOfScreen(screen);
	int		status;

	_DtSvcProcessLock();
	if (dragCollectionContext == (XContext)0) {
		dragCollectionContext = XUniqueContext();
	}
	_DtSvcProcessUnlock();

	status = XFindContext(dpy, root, dragCollectionContext, (XtPointer)&dc);

	if (status != 0) {
		dc = makeDragCollection(xmScreen);
		XSaveContext(dpy, root, dragCollectionContext, (XtPointer)dc);
		XtAddCallback(xmScreen, XmNdestroyCallback, 
				destroyDragCollection, (XtPointer)dc);
	}

	return dc;
}

/*
 * Module-public function to select/activate a source type drag icon
 */
void
_DtDndSelectDragSource(
	Widget		widget,
	DtDndDragSource	sourceType,
	Widget		sourceIcon)
{
	Widget		xmScreen = XmGetXmScreen(XtScreenOfObject(widget));
	DragCollection	*dc	 = getDragCollection(xmScreen);

	selectDragSource(dc, xmScreen, sourceType, sourceIcon);
}

void     
_DtDndGetIconOffset(
	Widget		dragContext, 
	DtDndDragSource	sourceType,
	int *		offsetX, 
	int *		offsetY)
{
	Widget		sourceIcon;
	int		stateOffsetX, stateOffsetY;

	switch (sourceType) {
	default:
	case DtDND_DRAG_SOURCE_DEFAULT:
		*offsetX = -(motif_x_hot + motif_x_offset_state);
		*offsetY = -(motif_y_hot + motif_y_offset_state);
		break;
	case DtDND_DRAG_SOURCE_TEXT:
		*offsetX = -(text_x_hot + text_x_offset_state);
		*offsetY = -(text_y_hot + text_y_offset_state);
		break;
	case DtDND_DRAG_SOURCE_DATA:
	case DtDND_DRAG_SOURCE_MULTIPLE:
		*offsetX = -(data_x_hot + data_x_offset_state);
		*offsetY = -(data_y_hot + data_y_offset_state);

		XtVaGetValues(dragContext,
			XmNsourcePixmapIcon,	&sourceIcon,
			NULL);

		if (sourceIcon != NULL) {
			calcStateIconOffset(sourceIcon,
				&stateOffsetX, &stateOffsetY);
			*offsetX = -(data_x_hot + stateOffsetX);
			*offsetY = -(data_y_hot + stateOffsetY);
		}
		break;
	}
}
