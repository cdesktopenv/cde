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
/* $XConsortium: DndP.h /main/4 1996/06/21 17:28:31 ageorge $ */
/*********************************************************************
 *
 *	File:		DndP.h
 *
 *	Description:	Private include file for DND Convenience API.
 *
 *********************************************************************
 *
 *+SNOTICE
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement bertween
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel wihtout
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

#ifndef _Dt_DndP_h
#define _Dt_DndP_h

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Drag and Drop selection targets
 */

extern Atom	XA_TARGETS;
extern Atom	XA_TIMESTAMP;
extern Atom	XA_MULTIPLE;
extern Atom	XA_DELETE;
extern Atom	XA_NULL;
extern Atom	XA_TEXT;
extern Atom	XA_HOST_NAME;
extern Atom	XA_SUN_FILE_HOST_NAME;
extern Atom	XA_SUN_ENUM_COUNT;
extern Atom	XA_SUN_DATA_LABEL;
extern Atom	XA_SUN_SELN_READONLY;
extern Atom	XA_SUN_ATM_FILE_NAME;
extern Atom	XA_SUN_ATM_METHODS;

#define DtGetAtom(display, atomname) \
	XmInternAtom((display),(atomname),False)


/*
 * Drag Icon Styles
 */
typedef enum {
	DtDND_DRAG_SOURCE_DEFAULT,
	DtDND_DRAG_SOURCE_TEXT,
	DtDND_DRAG_SOURCE_DATA,
	DtDND_DRAG_SOURCE_MULTIPLE
} DtDndDragSource;

/*
 *  Drag-n-Drop Data Transfer Protocol
 */
typedef struct _DtDndTransfer {
	Atom *			targets;
	Cardinal		numTargets;
	struct _DtDndMethods * 	methods;
} DtDndTransfer;

/*
 * Drag Initiator Structure
 */

typedef struct _DtDragInfo {
        Widget          	dragInitiator;
	Widget			dragContext;
	DtDndProtocol		protocol;
	Cardinal		numItems;
	unsigned char		operations;
        XtCallbackList  	dragConvertCallback;
        XtCallbackList  	dragFinishCallback;
        XtCallbackList  	dropOnRootCallback;
        XtCallbackList  	dragDropFinishCallback;
        XtCallbackList  	dropFinishCallback;
        XtCallbackList  	topLevelEnterCallback;
        XtCallbackList  	topLevelLeaveCallback;
        XtCallbackList  	dropStartCallback;
	Widget			sourceIcon;
	Boolean			bufferIsText;
	DtDndTransfer *	 	transfer;
        DtDndContext *		dragData;
        Boolean         	inRoot;
	Window	        	backdropWindow;
	DtDndStatus		status;
	XtPointer		clientData;
} DtDragInfo;
 
/*
 * Drop Receiver Existing Registration Structure
 */
 
typedef struct _DtDropSiteInfo {
        XtCallbackProc		dropProc;
        unsigned char		operations;
        Atom *			importTargets;
        int             	numImportTargets;
} DtDropSiteInfo;

/*
 * Drop Receiver Data Transfer Structure
 */

typedef struct _DtTransferInfo {
	Widget			dragContext;
	DtDndProtocol		protocol;
	unsigned char		operation;
	struct _DtDndMethods *	methods;
	Atom *			transferTargets;
	Cardinal		numTransferTargets;
	Cardinal		currentTransfer;
	Boolean			appTransferCalled;
        XEvent *		event;
        Position		x, y;
	XtPointer		clientData;
        XtCallbackList  	dropAnimateCallback;
} DtTransferInfo;

/*
 * Drop Receiver Registration Structure
 */

typedef struct _DtDropInfo {
        Widget          	dropReceiver;
	DtDndProtocol		protocols;
	unsigned char		operations;
        XtCallbackList  	dropTransferCallback;
        XtCallbackList  	dropAnimateCallback;
	Boolean			textIsBuffer;
	DtDropSiteInfo *	dropSiteInfo;
	DtDndTransfer *		transfers;
	Cardinal		numTransfers;
	DtTransferInfo *	transferInfo;
        DtDndContext * 		dropData;
        DtDndStatus		status;
} DtDropInfo;

/*
 * Drag-n-Drop Data Transfer Protocol Function Prototypes
 */

typedef void	
(*DtDndGetAvailTargetsProc)(
	DtDragInfo * 		dtDragInfo, 
	Atom **			returnAvailTargetsList, 
	Cardinal *		returnNumAvailTargets);

typedef void	
(*DtDndGetExportTargetsProc)(
	DtDragInfo * 		dtDragInfo, 
	Atom **			returnExportTargetsList, 
	Cardinal *		returnNumExportTargets);

typedef void	
(*DtDndGetImportTargetsProc)(
	DtDropInfo * 		dtDropInfo, 
	Atom **			returnImportTargetsList, 
	Cardinal *		returnNumImportTargets);

typedef void 
(*DtDndConvertInitProc)(
	DtDragInfo *		dtDragInfo);

typedef Boolean 
(*DtDndConvertProc)(
	Widget			dragContext,
	DtDragInfo *		dtDragInfo,
	Atom *			selection,
	Atom * 			target,
	Atom *			returnType,
	XtPointer *		returnValue,
	unsigned long *		returnLength,
	int *			returnFormat, 
	XSelectionRequestEvent *selectionRequestEvent);

typedef void
(*DtDndConvertFinishProc)(
	DtDragInfo *		dtDragInfo);

typedef void
(*DtDndTransferTargetsProc)(
	DtDropInfo *		dtDropInfo, 
	Atom *			exportTargets,
	Cardinal		numExportTargets,
	Atom **			returnTransferTargetsList, 
	Cardinal *		returnNumTransferTargets);

typedef void
(*DtDndTransferProc)(
	Widget			dropTransfer,
	DtDropInfo *		dtDropInfo,
	Atom *			selection,
	Atom * 			target,
	Atom * 			type,
	XtPointer		value,
	unsigned long *		length,
	int * 			format);

typedef void
(*DtDndTransferFinishProc)(
	DtDropInfo *		dtDropInfo);

/*
 * Drag-n-Drop Data Transfer Methods
 */

typedef struct _DtDndMethods {
	String				name;
	DtDndProtocol			protocol;
	DtDndDragSource			sourceType;
	DtDndGetAvailTargetsProc	getAvailTargets;
	DtDndGetExportTargetsProc	getExportTargets;
	DtDndGetImportTargetsProc	getImportTargets;
	DtDndConvertInitProc		convertInit;
	DtDndConvertProc		convert;
	DtDndConvertFinishProc		convertFinish;
	DtDndTransferTargetsProc	transferTargets;
	DtDndTransferProc		transfer;
	DtDndTransferFinishProc		transferFinish;
} DtDndMethods;

/*
 * Drag-n-Drop Private Utility Functions
 */

extern DtDndTransfer *
_DtDndCreateExportTransfer(
	DtDragInfo *	dtDragInfo);

extern DtDndTransfer *
_DtDndCreateImportTransfers(
	DtDropInfo *	dtDropInfo,
	Cardinal *	numTransfers);

extern void
_DtDndDestroyTransfers(
	DtDndTransfer *	transfers,
	Cardinal	numTransfers);

extern DtDndTransfer * 
_DtDndTransferFromTargets(
        DtDndTransfer * transfers,
        Cardinal        numTransfers,
        Atom *          targets,
        Cardinal        numTargets);

extern void
_DtDndTransferAdd(
	Widget		dropTransfer,
	DtDropInfo *	dtDropInfo,
	Atom *		transferTargets,
	Cardinal	numTransferTargets);

extern XtCallbackList
_DtDndCopyCallbackList(
	XtCallbackList	callbacks);

extern void	
_DtDndCallCallbackList(
	Widget		widget,
	XtCallbackList	callbacks,
	XtPointer	calldata);

extern void	
_DtDndSelectDragSource(
	Widget		anyWidget,
	DtDndDragSource	sourceType,
	Widget		sourceIcon);

extern void
_DtDndGetIconOffset(
	Widget		dragContext, 
	DtDndDragSource	sourceType,
	int *		offsetXReturn,
	int *		offsetYReturn);

extern String
_DtDndGetHostName(void);

#ifdef DEBUG
extern void _DtDndPrintTargets(Display*,Atom*,Cardinal);
extern void _DtDndPrintTransfers(Display*,DtDndTransfer*,Cardinal);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _Dt_DndP_h */
