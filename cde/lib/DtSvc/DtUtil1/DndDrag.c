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
/* $XConsortium: DndDrag.c /main/5 1996/09/27 19:00:40 drk $ */
 /*********************************************************************
 *
 *	File:		DndDrag.c
 *
 *	Description:	Implemenation of DND Drag Initator
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <X11/Intrinsic.h> 
#include <Xm/AtomMgr.h> 
#include <Xm/DragDrop.h>
#include <Xm/DragC.h> 
#include <Xm/DragCP.h>
#include <Xm/DragOverSP.h>
#include <Dt/Wsm.h>
#include "Dnd.h"
#include "DndP.h"
#include "DtSvcLock.h"

/* 
 * Drag Initiator Callbacks
 */
static Boolean  dndConvertProc(Widget, Atom*, Atom*, Atom*, XtPointer *,
                        	unsigned long*, int*);
static void	dndAppConvert(Widget, int, XEvent*, DtDragInfo*);
static void	dndDropStartCallback(Widget, XtPointer, XtPointer);
static void	dndDropFinishCallback(Widget, XtPointer, XtPointer);
static void	dndDragDropFinishCallback(Widget, XtPointer, XtPointer);
static void	dndTopLevelEnterCallback(Widget, XtPointer, XtPointer);
static void	dndTopLevelLeaveCallback(Widget, XtPointer, XtPointer);

extern int _DtDndCountVarArgs(va_list vaList);
extern void _DtDndArgListFromVarArgs(va_list vaList,
                                     Cardinal maxArgs,
                                     ArgList *argListReturn,
                                     Cardinal *argCountReturn);
extern void _XmDragOverChange(Widget w,
                              unsigned char dropSiteStatus);

/*
 * Drag Initiator Resources
 */
typedef struct {
	XtCallbackList	dropOnRootCallback;
	Widget		sourceIcon;
	Boolean		bufferIsText;
} DragSettings;

#define Offset(field)	XtOffsetOf(DragSettings, field)

static XtResource dragResources[] = {
      { DtNdropOnRootCallback, DtCDropOnRootCallback, 
	XtRCallback, sizeof(XtCallbackList), Offset(dropOnRootCallback), 
	XtRImmediate, (XtPointer)NULL},
      { DtNsourceIcon, DtCSourceIcon, 
	XtRWidget, sizeof(Widget), Offset(sourceIcon), 
	XtRImmediate, (XtPointer)NULL },
      { DtNbufferIsText, DtCBufferIsText, 
	XtRBoolean, sizeof(Boolean), Offset(bufferIsText), 
	XtRImmediate, (XtPointer)False },
};

#undef Offset

/*
 * DtDndVaDragStart
 *
 *	Drag Start - varargs version
 */

Widget
DtDndVaDragStart(
	Widget		dragInitiator,
	XEvent*		event,
	DtDndProtocol	protocol,
	Cardinal	numItems,
	unsigned char	operations,
	XtCallbackList	dragConvertCallback,
	XtCallbackList	dragFinishCallback,
	...)
{
	Widget		dragContext;
	va_list		vaList;
	ArgList		argList;
	Cardinal	argCount;
	_DtSvcWidgetToAppContext(dragInitiator);

	_DtSvcAppLock(app);

	va_start(vaList, dragFinishCallback);
	argCount = _DtDndCountVarArgs(vaList);
	va_end(vaList);

	va_start(vaList, dragFinishCallback);
	_DtDndArgListFromVarArgs(vaList, argCount, &argList, &argCount);
	va_end(vaList);

	dragContext = DtDndDragStart(dragInitiator, event, protocol, 
					numItems, operations,
					dragConvertCallback, dragFinishCallback,
					argList, argCount);

	XtFree((char *)argList);

	_DtSvcAppUnlock(app);
	return dragContext;
}

/*
 * DtDndVaDragStart
 *
 *	Drag Start - arglist version
 */

Widget
DtDndDragStart(
	Widget		dragInitiator,
	XEvent*		event,
	DtDndProtocol	protocol,
	Cardinal	numItems,
	unsigned char	operations,
	XtCallbackList	dragConvertCallback,
	XtCallbackList	dragFinishCallback,
	ArgList		argList,
	Cardinal	argCount)
{
        XtCallbackRec	dragDropFinishCbRec[] = { {dndDragDropFinishCallback,
						   NULL}, {NULL, NULL} };
	XtCallbackRec	topLevelEnterCbRec[]  = { {dndTopLevelEnterCallback,
						   NULL},  {NULL, NULL} };
	XtCallbackRec	topLevelLeaveCbRec[]  = { {dndTopLevelLeaveCallback,
						   NULL},  {NULL, NULL} };
	XtCallbackRec	dropStartCbRec[]      = { {dndDropStartCallback,
					    	   NULL}, {NULL, NULL} };
	XtCallbackRec	dropFinishCbRec[]     = { {dndDropFinishCallback,
						   NULL}, {NULL, NULL} };
	Display *	display 	= XtDisplayOfObject(dragInitiator);
	Screen *	screen 		= XtScreenOfObject(dragInitiator);
	Window		rootWindow 	= RootWindowOfScreen(screen);
	DtDragInfo *	dtDragInfo;
	DragSettings	settings;
	DtDndDragSource	sourceType;
	DtDndTransfer *	transfer;
	Arg *		args;
	int		ii, nn, savedEventType;
	Atom *		exportTargets;
	Cardinal	numExportTargets;
	_DtSvcWidgetToAppContext(dragInitiator);

	_DtSvcAppLock(app);
	/*
	 * Reject the drag if noop or multiple protocols specified
	 */

	switch (protocol) {
	case DtDND_BUFFER_TRANSFER:
	case DtDND_FILENAME_TRANSFER:
	case DtDND_TEXT_TRANSFER:
		break;
	case DtDND_NOOP_TRANSFER:
	default:
		_DtSvcAppUnlock(app);
		return (Widget)NULL;
	}

	/*
	 * Parse resources into dragResources
	 */

	XtGetSubresources(dragInitiator, &settings, 
				(String)NULL, (String)NULL, 
				dragResources, XtNumber(dragResources),
				argList, argCount);

	/*
	 * Initialize DragInfo
	 */

	dtDragInfo = (DtDragInfo *) XtMalloc(sizeof(DtDragInfo));

	dtDragInfo->dragInitiator 		= dragInitiator;
	dtDragInfo->dragContext			= NULL;
	dtDragInfo->protocol			= protocol;
	dtDragInfo->numItems			= numItems;
	dtDragInfo->operations			= operations;
	dtDragInfo->sourceIcon 			= settings.sourceIcon;
	dtDragInfo->bufferIsText		= settings.bufferIsText;
	dtDragInfo->dragData 			= NULL;
	dtDragInfo->inRoot 			= False;
	dtDragInfo->status 			= DtDND_SUCCESS;
	dtDragInfo->clientData 			= NULL;
	dtDragInfo->backdropWindow 
			= DtWsmGetCurrentBackdropWindow(display, rootWindow);

	dtDragInfo->dragConvertCallback
			 = _DtDndCopyCallbackList(dragConvertCallback);
	dtDragInfo->dragFinishCallback
			 = _DtDndCopyCallbackList(dragFinishCallback);
	dtDragInfo->dropOnRootCallback
			 = _DtDndCopyCallbackList(settings.dropOnRootCallback);

	dtDragInfo->dragData = (DtDndContext *)XtCalloc(1,sizeof(DtDndContext));
	dtDragInfo->dragData->protocol		= dtDragInfo->protocol;
	dtDragInfo->dragData->numItems		= 0;

	/*
	 * Get data transfer method
	 * Use the transfer targets as export targets
	 */

	dtDragInfo->transfer = _DtDndCreateExportTransfer(dtDragInfo);

	exportTargets 		= dtDragInfo->transfer->targets;
	numExportTargets 	= dtDragInfo->transfer->numTargets;

#ifdef DEBUG
	printf("DtDndDragStart: drag from widget 0x%p\n", dragInitiator);
	_DtDndPrintTransfers(display,dtDragInfo->transfer,1);
#endif

	/*
	 * Set up drag icon
	 */

	if (numItems > 1) {
		sourceType = DtDND_DRAG_SOURCE_MULTIPLE;
	} else {
		sourceType = dtDragInfo->transfer->methods->sourceType;
	}
	
	_DtDndSelectDragSource(dragInitiator, sourceType, 
				dtDragInfo->sourceIcon);

	/*
	 * Construct argument list
	 */

#define NUM_DRAG_ARGS	30
	args = (Arg *) XtMalloc(sizeof(Arg) * (NUM_DRAG_ARGS + argCount));
#undef  NUM_DRAG_ARGS

	/*
	 * Copy in passed arguments
	 */
	nn = 0;

	for (ii = 0; ii < argCount; ii++) {
        	XtSetArg(args[nn], argList[ii].name, argList[ii].value); nn++;
	}

	/*
	 * Set basic drag start arguments
	 */

        XtSetArg(args[nn], XmNexportTargets,	exportTargets);
	nn++;
        XtSetArg(args[nn], XmNnumExportTargets, numExportTargets);
	nn++;
        XtSetArg(args[nn], XmNdragOperations, 	operations);
	nn++;
	XtSetArg(args[nn], XmNblendModel, 	XmBLEND_ALL);
	nn++;
	XtSetArg(args[nn], XmNcursorBackground, WhitePixelOfScreen(screen));
	nn++;
	XtSetArg(args[nn], XmNcursorForeground, BlackPixelOfScreen(screen));
	nn++;
	XtSetArg(args[nn], XmNclientData, 	dtDragInfo);
					nn++;

	if (dtDragInfo->sourceIcon != NULL) {
		XtSetArg(args[nn],XmNsourcePixmapIcon, dtDragInfo->sourceIcon);
		nn++;
		XtSetArg(args[nn],XmNsourceCursorIcon, dtDragInfo->sourceIcon);
		nn++;
	}

	/*
	 * Set up DnD callbacks for Motif
	 */

	XtSetArg(args[nn], XmNconvertProc, 	dndConvertProc);
	nn++;
   
	dragDropFinishCbRec[0].closure	= (XtPointer) dtDragInfo;
	dropFinishCbRec[0].closure 	= (XtPointer) dtDragInfo;
	dtDragInfo->dragDropFinishCallback 
			 = _DtDndCopyCallbackList(dragDropFinishCbRec);
	dtDragInfo->dropFinishCallback
			 = _DtDndCopyCallbackList(dropFinishCbRec);

	XtSetArg(args[nn], XmNdragDropFinishCallback, dtDragInfo->dragDropFinishCallback);
	nn++;
	XtSetArg(args[nn], XmNdropFinishCallback, dtDragInfo->dropFinishCallback);
	nn++;

	/*
	 * Only use top-level-enter/leave callbacks if also doing drop-on-root
	 */

	if (dtDragInfo->dropOnRootCallback != NULL) {

		topLevelEnterCbRec[0].closure 	= (XtPointer) dtDragInfo;
		topLevelLeaveCbRec[0].closure 	= (XtPointer) dtDragInfo;
		dropStartCbRec[0].closure 	= (XtPointer) dtDragInfo;
		dtDragInfo->topLevelEnterCallback
			 = _DtDndCopyCallbackList(topLevelEnterCbRec);
		dtDragInfo->topLevelLeaveCallback
			 = _DtDndCopyCallbackList(topLevelLeaveCbRec);
		dtDragInfo->dropStartCallback
			 = _DtDndCopyCallbackList(dropStartCbRec);

		XtSetArg(args[nn], XmNtopLevelEnterCallback, dtDragInfo->topLevelEnterCallback);
		nn++;
		XtSetArg(args[nn], XmNtopLevelLeaveCallback, dtDragInfo->topLevelLeaveCallback);
		nn++;
		XtSetArg(args[nn], XmNdropStartCallback, dtDragInfo->dropStartCallback);
		nn++;
	}

	/*
	 * Fake a button press. This is necessary because Motif requires
	 * a drag to start on a button press. We need to be able to start
	 * a drag on a mouse motion event when Bselect is held down. Since
	 * the motion event has the fields necessary for Motif this works.
	 */

	savedEventType = event->type;

	if (event->type == MotionNotify) {
		event->type = ButtonPress;
	}

	/*
	 * Start the drag
	 */

	dtDragInfo->dragContext = XmDragStart(dragInitiator, event, args, nn);

	XtFree((char *)args);

	event->type = savedEventType;

	_DtSvcAppUnlock(app);
	return (dtDragInfo->dragContext);
}

/*********************************************************************
 *
 * 	Drag Initiator Callbacks
 *
 *********************************************************************/

/*
 * dndDropStartCallback
 *
 *	
 */
static void
dndDropStartCallback(
	Widget		dragContext,
	XtPointer	clientData,
	XtPointer	callData)
{
	DtDragInfo     *dtDragInfo	= (DtDragInfo *) clientData;
	DtDndContext   *dragData;
	XmDragContext   xmDragContext = (XmDragContext)dtDragInfo->dragContext;
	XmDropStartCallbackStruct *xmDropInfo = (XmDropStartCallback) callData;	
	DtDndTransferCallbackStruct	dropCallData;
	int		posOffsetX, posOffsetY;

	/*
	 * If the user has cancelled the drop, or the drop isn't on the
	 * root, or there are no dropOnRoot or convert callbacks
	 * then reject the drop.
	 */  

	if (xmDragContext->drag.dragCompletionStatus == XmDROP_CANCEL ||
	    dtDragInfo->inRoot == False ||
	    dtDragInfo->dropOnRootCallback == NULL ||
	    dtDragInfo->dragConvertCallback == NULL ) {
 
		xmDropInfo->dropSiteStatus 	= XmINVALID_DROP_SITE;
		xmDropInfo->dropAction 		= XmDROP_CANCEL;

		return;
	}

	/*
	 * The following is to handle the dropOnRoot situation.
	 * We handle both the convert and transfer sides of the
	 * transaction here.  First we get the application drag data
	 * and then we call the application dropOnRoot callback.
	 */
 
	/*
	 * Initialize protocol specific dragData
	 */

	dtDragInfo->dragData->numItems	= dtDragInfo->numItems;

	(*dtDragInfo->transfer->methods->convertInit)(dtDragInfo);

	/*
	 * Invoke the application convert callback
	 */

	dndAppConvert(dragContext, DtCR_DND_CONVERT_DATA, 
			xmDropInfo->event, dtDragInfo);

	if (dtDragInfo->status == DtDND_FAILURE) {
		return;
	}

	/*
	 * Setup dropOnRootcall data and invoke the dropOnroot callback
	 */

	_DtDndGetIconOffset(dtDragInfo->dragContext,
			dtDragInfo->transfer->methods->sourceType,
			&posOffsetX, &posOffsetY);

	dropCallData.reason 		= DtCR_DND_ROOT_TRANSFER;
	dropCallData.event		= xmDropInfo->event;
	dropCallData.x 			= xmDropInfo->x + posOffsetX;
	dropCallData.y 			= xmDropInfo->y + posOffsetY;
	dropCallData.operation		= xmDropInfo->operation;
	dropCallData.dropData		= dtDragInfo->dragData;
	dropCallData.completeMove	= False;
	dropCallData.status 		= DtDND_SUCCESS;

	_DtDndCallCallbackList(dragContext, dtDragInfo->dropOnRootCallback,
		(XtPointer)&dropCallData);

	/*
	 * Tell Motif that the root is a valid drop site
	 */

	xmDropInfo->dropSiteStatus	= XmVALID_DROP_SITE;
	xmDropInfo->dropAction		= XmDROP;
}

/*
 * dndConvertProc
 *
 *
 */
static Boolean
dndConvertProc(
	Widget		dragContext,
	Atom	       *selection,
	Atom	       *target,
	Atom	       *returnType,
	XtPointer      *returnValue,
	unsigned long  *returnLength,
	int	       *returnFormat)
{
	Atom 		realSelectionAtom; /* Motif hides the selection atom */
	DtDragInfo     *dtDragInfo = NULL;
	XSelectionRequestEvent *selectionRequestEvent;
	Boolean 	status;

#ifdef DEBUG
	{
	Display *display = XtDisplayOfObject(dragContext);
	char   *atomname = XGetAtomName(display,*target);
	printf("dndConvertProc:  target = %s\n",(atomname ? atomname : "Null"));
	if (atomname) XFree(atomname);
	}
#endif
	/*
	 * Get the DtDragInfo
	 */

	XtVaGetValues(dragContext, XmNclientData, &dtDragInfo, NULL);

	if (dtDragInfo == NULL || dtDragInfo->status == DtDND_FAILURE) {
		return False;
	}

	/*
	 * Get selection request event
	 */

	XtVaGetValues(dragContext, XmNiccHandle, &realSelectionAtom, NULL);
	selectionRequestEvent = XtGetSelectionRequest(dragContext,
		realSelectionAtom, NULL); /* REMIND: NULL for atomic transfer */

	/*
	 * Get the application drag data if necessary
	 */

	if (dtDragInfo->dragData->numItems == 0) {

		dtDragInfo->dragData->numItems	= dtDragInfo->numItems;
		
		(*dtDragInfo->transfer->methods->convertInit)(dtDragInfo);

		dndAppConvert(dragContext, DtCR_DND_CONVERT_DATA,
			(XEvent *)selectionRequestEvent, dtDragInfo);

		if (dtDragInfo->status == DtDND_FAILURE) {
			return False;
		}
	}

	/*
	 * Handle transfer protocol independent target conversions
	 */

	if (*target == XA_TARGETS) {
	/*
	 * TARGETS	Construct a list of targets consisting of those
	 *		the dnd library supports plus those supported by
	 *		the drag initiator.
	 */
		int	ii, LIBRARY_TARGETS = 6;
		Atom *	availTargets;
		Atom *	allTargets;
		Cardinal numAvailTargets;
		Cardinal numAllTargets;

		(*dtDragInfo->transfer->methods->getAvailTargets)(dtDragInfo, 
				&availTargets, &numAvailTargets);

		numAllTargets = numAvailTargets + LIBRARY_TARGETS;
		allTargets    = (Atom *)XtMalloc(sizeof(Atom) * numAllTargets);

		for (ii = 0; ii < numAvailTargets; ii++) {
			allTargets[ii] = availTargets[ii];
		}

		XtFree((char *)availTargets);

		ii = numAvailTargets;

		allTargets[ii++] = XA_TARGETS;
		allTargets[ii++] = XA_TIMESTAMP;
		allTargets[ii++] = XA_MULTIPLE;
		allTargets[ii++] = XA_HOST_NAME;
		allTargets[ii++] = XA_SUN_FILE_HOST_NAME;
		allTargets[ii++] = XA_DELETE;

		*returnType 	= XA_ATOM;
		*returnFormat 	= 32;
		*returnValue 	= (XtPointer)allTargets;
		*returnLength 	= numAllTargets * sizeof(Atom)/4;

		status = True;

	} else if (*target == XA_TIMESTAMP || *target == XA_MULTIPLE) {
	/*
	 * TIMESTAMP and MULTIPLE are handled by the Intrinsics
	 */
		status = True;

	} else if (*target == XA_HOST_NAME ||
		   *target == XA_SUN_FILE_HOST_NAME) {
	/*
	 * HOST_NAME, _SUN_FILE_HOST_NAME	The name of this host
	 */
                *returnType 	= XA_STRING;
                *returnValue	= (XtPointer)XtNewString(_DtDndGetHostName());
                *returnLength	= strlen((char *)*returnValue) + 1;
                *returnFormat	= 8;

		status = True;

	} else if (*target == XA_DELETE) {
	/*
	 * DELETE	Set up convert callback data to specify
  	 * 		deletion and invoke the application-defined
	 *		convertCallback() to perform the delete.
	 */

		*returnType 	= XA_NULL;
		*returnFormat 	= 32;
		*returnValue 	= (XtPointer) NULL;
		*returnLength 	= 0;

		dndAppConvert(dragContext, DtCR_DND_CONVERT_DELETE,
			(XEvent *)selectionRequestEvent, dtDragInfo);

		status = True;

	} else if (*target == XA_SUN_ENUM_COUNT) {
	/*
	 * _SUN_ENUMERATION_COUNT	The number of items available
	 */
                int *count	= XtNew(int);

		if (dtDragInfo->dragData->numItems == 1) {
			count[0] = 1;
		} else {
			count[0] = 0;
			dtDragInfo->status = DtDND_FAILURE;
		}

                *returnType 	= XA_INTEGER;
                *returnValue	= (XtPointer)count;
                *returnLength	= 1;
                *returnFormat	= 32;

		status = True;
	} else {
	/*
	 * Invoke protocol specific convert method
	 */
		status = (*dtDragInfo->transfer->methods->convert)(
				dragContext, dtDragInfo,
				selection, target, 
				returnType, returnValue,
				returnLength, returnFormat, 
				selectionRequestEvent);
	}

	return status;
}

/*
 * dndAppConvert
 *
 *	Call the application convert callback
 */
static void
dndAppConvert(
	Widget		dragContext,
	int		reason,
	XEvent *	event,
	DtDragInfo *	dtDragInfo)
{
	DtDndConvertCallbackStruct	convertCallData;

	convertCallData.reason 		= reason;
	convertCallData.event 		= event;
	convertCallData.dragData	= dtDragInfo->dragData;
	convertCallData.status 		= DtDND_SUCCESS;

	_DtDndCallCallbackList(dragContext, dtDragInfo->dragConvertCallback,
		(XtPointer)&convertCallData);

	dtDragInfo->status = convertCallData.status;

	if (reason == DtCR_DND_CONVERT_DATA &&
	    dtDragInfo->dragData->numItems <= 0) {
		dtDragInfo->status = DtDND_FAILURE;
	}

}

/*
 * dndDropFinishCallback
 *
 *	Handle drop-on-root case
 */
static void
dndDropFinishCallback(
	Widget		dragContext,
	XtPointer	clientData,
	XtPointer	callData)
{
	DtDragInfo     *dtDragInfo = (DtDragInfo *) clientData;
	XmDropFinishCallbackStruct *xmDropFinishCallData =
			(XmDropFinishCallbackStruct *) callData;

	if (dtDragInfo->dropOnRootCallback != NULL &&
	    dtDragInfo->inRoot &&
	    xmDropFinishCallData->dropSiteStatus == XmVALID_DROP_SITE) {

		xmDropFinishCallData->completionStatus = XmDROP_SUCCESS;

		XtVaSetValues(dtDragInfo->dragContext,
			XmNblendModel, XmBLEND_NONE, NULL);
	}
}

/*
 * dndDragDropFinishCallback
 *
 *	Call the application dragFinishCallback
 */
static void
dndDragDropFinishCallback(
	Widget		dragContext,
	XtPointer	clientData,
	XtPointer	callData)
{
	XmDragDropFinishCallbackStruct *xmDndFinishInfo =
				(XmDragDropFinishCallbackStruct *)callData;
	DtDragInfo	       		*dtDragInfo = (DtDragInfo *)clientData;
	DtDndDragFinishCallbackStruct	dragFinishCallData;

	/*
	 * Invoke application dragFinishCallback
	 */

	dragFinishCallData.reason 	= DtCR_DND_DRAG_FINISH;
	dragFinishCallData.event 	= xmDndFinishInfo->event;
	dragFinishCallData.sourceIcon 	= dtDragInfo->sourceIcon;
	dragFinishCallData.dragData 	= dtDragInfo->dragData;

	_DtDndCallCallbackList(dragContext, dtDragInfo->dragFinishCallback, 
		(XtPointer)&dragFinishCallData);

	/*
	 * Restore motif default drag cursors
	 */

	_DtDndSelectDragSource(dragContext, DtDND_DRAG_SOURCE_DEFAULT, NULL);

	/*
	 * Invoke protocol specific convertFinish
	 */

	(*dtDragInfo->transfer->methods->convertFinish)(dtDragInfo);

	/*
	 * Free data structures allocated during the drag
	 */

	XtFree((char *)dtDragInfo->transfer->targets);
	XtFree((char *)dtDragInfo->transfer);
	XtFree((char *)dtDragInfo->dragConvertCallback);
	XtFree((char *)dtDragInfo->dragFinishCallback);
	XtFree((char *)dtDragInfo->dragDropFinishCallback);
	XtFree((char *)dtDragInfo->dropFinishCallback);
	if (dtDragInfo->dropOnRootCallback != NULL) {
	  XtFree((char *)dtDragInfo->topLevelEnterCallback);
	  XtFree((char *)dtDragInfo->topLevelLeaveCallback);
	  XtFree((char *)dtDragInfo->dropStartCallback);
	}
	XtFree((char *)dtDragInfo->dropOnRootCallback);
	XtFree((char *)dtDragInfo->dragData);
	XtFree((char *)dtDragInfo);
}

/*
 * dndTopLevelEnterCallback -- Support for drop-on-root callback.
 *  	When a drop-on-root callback has been set, determines if 
 *	the drag has entered the root window (or equivalents)
 *	and sneakily changes Motif's idea that the root is an
 *	invalid drop site to think that it's really a valid one.
 *	Also updates dtDragInfo.inRoot as needed.
 */
static void
dndTopLevelEnterCallback(
	Widget		dragContext,
	XtPointer	clientData,
	XtPointer	callData)
{
	XmTopLevelEnterCallbackStruct  *xmEnterInfo =
		(XmTopLevelEnterCallbackStruct *) callData;
	DtDragInfo	       *dtDragInfo = (DtDragInfo *) clientData;
	XmDragContext		xmDragContext = (XmDragContext) dragContext;
	XmDragOverShellWidget	dragOverShell = xmDragContext->drag.curDragOver;

	
	if (xmEnterInfo->window == RootWindowOfScreen(xmEnterInfo->screen) ||
	    dtDragInfo->backdropWindow == xmEnterInfo->window ) {

		dragOverShell->drag.cursorState = XmVALID_DROP_SITE;
		_XmDragOverChange((Widget)dragOverShell,
			dragOverShell->drag.cursorState);

		dtDragInfo->inRoot = True;

	} else {
		dtDragInfo->inRoot = False;
	}
}

/*
 * dndTopLevelLeaveCallback -- Support for drop-on-root callback.
 *  	When a drop-on-root callback has been set, determines if 
 *	the drag is exiting the root window and restores Motif's
 *	internal state back to thinking that the root window is
 *	an invalid drop site.  We don't update dtDragInfo->inRoot
 *	here since the top-level-leave callback is called before
 *	the drop callback which needs to know if we're in the root
 *	or not.
 */
static void
dndTopLevelLeaveCallback(
	Widget		dragContext,
	XtPointer	clientData,
	XtPointer	callData)
{
	XmTopLevelLeaveCallbackStruct  *xmLeaveInfo =
		(XmTopLevelLeaveCallbackStruct *) callData;
	DtDragInfo	       *dtDragInfo = (DtDragInfo *) clientData;
	XmDragContext		xmDragContext = (XmDragContext) dragContext;
	XmDragOverShellWidget	dragOverShell = xmDragContext->drag.curDragOver;
	

	if (xmLeaveInfo->window == RootWindowOfScreen(xmLeaveInfo->screen) ||
	    dtDragInfo->backdropWindow == xmLeaveInfo->window ) {

		dragOverShell->drag.cursorState = XmINVALID_DROP_SITE;

		_XmDragOverChange((Widget)dragOverShell,
			dragOverShell->drag.cursorState);
	}
}
