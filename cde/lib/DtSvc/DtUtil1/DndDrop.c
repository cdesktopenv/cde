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
/* $XConsortium: DndDrop.c /main/6 1996/09/27 19:00:45 drk $ */
 /*********************************************************************
 *
 *	File:		DndDrop.c
 *
 *	Description:	Implementation of DND Drop Registration
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
#include <stdarg.h>
#include <stdint.h>
#include <X11/Intrinsic.h> 
#include <Xm/AtomMgr.h> 
#include <Xm/DragDrop.h>
#include <Xm/DropSMgr.h> 
#include <Xm/DropTrans.h> 
#include "Dnd.h"
#include "DndP.h"
#include "DtSvcLock.h"

/* 
 * Drop Receiver Callbacks
 */

static void		dndDropProc(Widget, XtPointer, XtPointer);
static void 		dndTransferStart(Widget, XmDropProcCallbackStruct*, 
				DtDropInfo*, DtDndTransfer*, Atom*, Cardinal);
static void		dndTransferProc(Widget, XtPointer, Atom*, Atom*, 
				XtPointer, unsigned long*, int*);
static void		dndAppTransfer(Widget, DtDropInfo*);
static void		dndDropAnimateCallback(Widget, XtPointer, XtPointer);

/*
 * Drop Support Functions
 */

static  void		dndDropSiteDestroy(Widget, XtPointer, XtPointer);
static 	XID		dndGetContextXID(Display*);
static 	Boolean		dndSaveDropInfo(Display*, Widget, DtDropInfo*);
static 	DtDropInfo *	dndFindDropInfo(Display*, Widget);
static	void		dndTransferStartFailed(Widget);
static	void		dndTransferFail(DtDropInfo*, Widget);
 
/*
 * Drop Register Resources
 */

typedef struct {
	XtCallbackList	dropAnimateCallback;
	Boolean		preserveRegistration;
	Boolean		registerChildren;
	Boolean		textIsBuffer;
} DropSettings;

#define Offset(field)	XtOffsetOf(DropSettings, field)

static XtResource dropResources[] = {
      { DtNdropAnimateCallback, DtCDropAnimateCallback, 
	XtRCallback, sizeof(XtCallbackList), Offset(dropAnimateCallback), 
	XtRImmediate, (XtPointer)NULL },
      { DtNpreserveRegistration, DtCPreserveRegistration, 
	XtRBoolean, sizeof(Boolean), Offset(preserveRegistration), 
	XtRImmediate, (XtPointer)True },
      { DtNregisterChildren, DtCRegisterChildren, 
	XtRBoolean, sizeof(Boolean), Offset(registerChildren), 
	XtRImmediate, (XtPointer)False },
      { DtNtextIsBuffer, DtCTextIsBuffer, 
	XtRBoolean, sizeof(Boolean), Offset(textIsBuffer), 
	XtRImmediate, (XtPointer)False },
};

#undef Offset

/*
 * DtDndVaDropRegister
 *
 *	Drop Site Registration - varargs version
 */

void
DtDndVaDropRegister(
	Widget		dropReceiver,
	DtDndProtocol	protocols,
	unsigned char	operations,
	XtCallbackList	transferCallback,
	...)
{
	va_list		vaList;
	ArgList		argList;
	Cardinal	argCount;
	_DtSvcWidgetToAppContext(dropReceiver);

	_DtSvcAppLock(app);
	va_start(vaList, transferCallback);
	argCount = _DtDndCountVarArgs(vaList);
	va_end(vaList);

	va_start(vaList, transferCallback);
	_DtDndArgListFromVarArgs(vaList, argCount, &argList, &argCount);
	va_end(vaList);

	DtDndDropRegister(dropReceiver, protocols, operations, 
				transferCallback, argList, argCount);

	XtFree((char *)argList);
	_DtSvcAppUnlock(app);
}

/*
 * DtDndDropRegister
 *
 *	Drop Site Registration - arglist version
 */

void
DtDndDropRegister(
	Widget		dropReceiver,
	DtDndProtocol	protocols,
	unsigned char	operations,
	XtCallbackList	dropTransferCallback,
	ArgList		argList,
	Cardinal	argCount)
{
	Display *	display		= XtDisplayOfObject(dropReceiver);
	DropSettings	settings;
	DtDropInfo *	dtDropInfo;
        DtDropSiteInfo* dsInfo = NULL;
	DtDndTransfer *	transfers;
	Cardinal	numTransfers;
	Atom	        *importTargets;
	Cardinal	numImportTargets;
	unsigned char	mergedOperations;
	Arg *		args;
	int		ii, jj, kk, nn;
	_DtSvcWidgetToAppContext(dropReceiver);

	_DtSvcAppLock(app);

	/*
	 * Reject a noop registration
	 */
	if (protocols == DtDND_NOOP_TRANSFER) {
	        _DtSvcAppUnlock(app);
		return;
	}

	/*
         * Parse resources into dropResources
         */
	XtGetSubresources(dropReceiver, &settings,
				(String)NULL, (String)NULL,
				dropResources, XtNumber(dropResources),
				argList, argCount);
	/*
	 * Initialize DropInfo
	 */

	dtDropInfo = (DtDropInfo *) XtMalloc(sizeof(DtDropInfo));

	dtDropInfo->dropReceiver 		= dropReceiver;
	dtDropInfo->protocols 			= protocols;
	dtDropInfo->operations 			= operations;
	dtDropInfo->textIsBuffer 		= settings.textIsBuffer;
	dtDropInfo->dropSiteInfo		= NULL;
	dtDropInfo->transferInfo		= NULL;
	dtDropInfo->status 			= DtDND_SUCCESS;

	dtDropInfo->dropTransferCallback 
			= _DtDndCopyCallbackList(dropTransferCallback);
	dtDropInfo->dropAnimateCallback
			= _DtDndCopyCallbackList(settings.dropAnimateCallback);

	/*
	 * Initialize Transfer Methods
	 */

	transfers = _DtDndCreateImportTransfers(dtDropInfo, &numTransfers);

	dtDropInfo->transfers		= transfers;
	dtDropInfo->numTransfers	= numTransfers;

#ifdef DEBUG
	printf("DtDndDropRegister: registering widget 0x%p\n", dropReceiver);
	_DtDndPrintTransfers(display,transfers,numTransfers);
#endif

	/*
	 * Preserve existing drop registration info if requested
	 */

	if (settings.preserveRegistration) {
		Arg		pArgs[4];

		dsInfo = (DtDropSiteInfo *)XtMalloc(sizeof(DtDropSiteInfo));

		dsInfo->dropProc 		= (XtCallbackProc)NULL;
		dsInfo->operations		= XmDROP_NOOP;
		dsInfo->importTargets		= NULL;
		dsInfo->numImportTargets	= 0;

		nn = 0;

		XtSetArg(pArgs[nn], XmNimportTargets,
				&(dsInfo->importTargets));		nn++;
		XtSetArg(pArgs[nn], XmNdropSiteOperations,
				&(dsInfo->operations));			nn++;
		XtSetArg(pArgs[nn], XmNnumImportTargets,
				&(dsInfo->numImportTargets));		nn++;
		XtSetArg(pArgs[nn], XmNdropProc,
				&(dsInfo->dropProc));			nn++;

		XmDropSiteRetrieve(dropReceiver, pArgs, nn);

		if ( (dsInfo->dropProc != NULL) && 
		     (dsInfo->numImportTargets > 0)  &&
		     (dsInfo->operations != XmDROP_NOOP) ){

			dtDropInfo->dropSiteInfo = dsInfo;
		} else {
			XtFree((char *)dsInfo);
			dsInfo = NULL;
		}
	}
	
	/*
	 * Create list of import targets based on the requested transfer
	 * protocols and any preserved drop site registration.
	 */

	numImportTargets = 0;

	for (ii = 0; ii < numTransfers; ii++) {
		numImportTargets += transfers[ii].numTargets;
	}

	if (dsInfo != NULL) { 
		numImportTargets += dsInfo->numImportTargets;
	}

	importTargets = (Atom *) XtMalloc(numImportTargets * sizeof(Atom));

	kk = 0;
	for (ii = 0; ii < numTransfers; ii++) {
		for (jj = 0; jj < transfers[ii].numTargets; jj++) {
			importTargets[kk++] = transfers[ii].targets[jj];
		}
	}

	if (dsInfo != NULL) {
		for (jj = 0; jj < dsInfo->numImportTargets; jj++) {
			importTargets[kk++] = dsInfo->importTargets[jj];
		}
	}

        /*
         * Merge operations
         */
 
	if (dsInfo != NULL) {
        	mergedOperations = dsInfo->operations | operations;
	} else {
        	mergedOperations = operations;
	}

	/*
	 * Create an argument list
	 */

#define NUM_DROP_ARGS	5
        args = (Arg *) XtMalloc(sizeof(Arg) * (NUM_DROP_ARGS + argCount));
#undef  NUM_DROP_ARGS

	/*
	 * Copy in passed arguments
	 */

	nn = 0;

	for (ii = 0; ii < argCount; ii++) {
		XtSetArg(args[nn], argList[ii].name, argList[ii].value); nn++;
	}

	/*
	 * Set argument list for drop site registration
	 */

	XtSetArg(args[nn], XmNimportTargets,      importTargets);    nn++;
	XtSetArg(args[nn], XmNnumImportTargets,   numImportTargets); nn++;
	XtSetArg(args[nn], XmNdropSiteOperations, mergedOperations);	   nn++;
	XtSetArg(args[nn], XmNdropProc, 	  dndDropProc);  	   nn++;

	if (settings.registerChildren) {
	       XtSetArg(args[nn], XmNdropSiteType, XmDROP_SITE_COMPOSITE); nn++;
	}

	/*
	 * Register the drop site
	 */

	if (dsInfo != NULL) {
		XmDropSiteUnregister(dropReceiver);
		XmDropSiteRegister(dropReceiver, args, nn);
	} else {
		XmDropSiteRegister(dropReceiver, args, nn);
	}

	/*
	 * Add a destroy callback to unregister the drop site
	 * Store the dropInfo using the dropReceiver as the context
	 * Free locally allocated memory
	 */

	XtAddCallback(dropReceiver, XtNdestroyCallback, 
			dndDropSiteDestroy, NULL);

	(void)dndSaveDropInfo(display, dropReceiver, dtDropInfo);

	XtFree((char *)importTargets);
	XtFree((char *)args);
	_DtSvcAppUnlock(app);
}

/*
 * DtDndDropUnregister
 *
 *	Unregister the drop site
 */
void
DtDndDropUnregister(
	Widget		dropReceiver)
{
	Display	       *display 	= XtDisplayOfObject(dropReceiver);
	DtDropInfo     *dtDropInfo;
	DtDropSiteInfo *dsInfo;
	_DtSvcWidgetToAppContext(dropReceiver);

	_DtSvcAppLock(app);

	/*
	 * Retrieve dropInfo.  If there isn't one then the drop site
	 * was not registered by DtDndDropRegister() so return.
	 */

	if ((dtDropInfo = dndFindDropInfo(display, dropReceiver)) == NULL) {
	        _DtSvcAppUnlock(app);
		return;
	}
	dsInfo = dtDropInfo->dropSiteInfo;

	/*
	 * Unregister the drop site
	 */

	XmDropSiteUnregister(dropReceiver);

	/*
	 * If we have preserved drop site registration
	 * then restore the drop site to it's original state.
	 */

	if (dsInfo != NULL) {
        	Arg             args[4];
		Cardinal	nn = 0;
 
		XtSetArg(args[nn], XmNimportTargets, 
			dsInfo->importTargets);	 		nn++;
		XtSetArg(args[nn], XmNnumImportTargets,
			dsInfo->numImportTargets);	 	nn++;
		XtSetArg(args[nn], XmNdropProc,
			dsInfo->dropProc);		 	nn++;
		XtSetArg(args[nn], XmNdropSiteOperations,
			dsInfo->operations);		 	nn++;

		XmDropSiteRegister(dropReceiver, args, nn);

		XtFree((char *)dsInfo);
	}
 
        /*
	 * Free callback, context and memory associated with DtDndDropRegister
	 */

	XtRemoveCallback(dropReceiver, XtNdestroyCallback, 
			dndDropSiteDestroy, NULL);

	XDeleteContext(display, dndGetContextXID(display), 
		(XContext)(intptr_t)dropReceiver);

	_DtDndDestroyTransfers(dtDropInfo->transfers, dtDropInfo->numTransfers);

	XtFree((char *)dtDropInfo->dropTransferCallback);
	XtFree((char *)dtDropInfo->dropAnimateCallback);
	XtFree((char *)dtDropInfo);
	_DtSvcAppUnlock(app);
}


/*********************************************************************
 *
 * Drop Receiver Callbacks
 *
 *********************************************************************/


/*
 * dndDropProc
 *
 * 	Determine transfer target and operation and initiate transfer.
 */
static void
dndDropProc(
	Widget		dropReceiver,
	XtPointer	clientData,
	XtPointer	callData)
{
	XmDropProcCallbackStruct *xmDropInfo =
			(XmDropProcCallbackStruct *) callData;
	Display	*	display 	= XtDisplayOfObject(dropReceiver);
	Boolean		compatible;
	Atom *		exportTargets	 = NULL;
	Cardinal	numExportTargets = 0;
	DtDropInfo *	dtDropInfo;
	DtDropSiteInfo* dsInfo;
	DtDndTransfer *	transfer;
 
	/*
	 * Reject invalid drops
	 */

	if ( (xmDropInfo->dropSiteStatus != XmDROP_SITE_VALID) || 
	     (xmDropInfo->operation == XmDROP_NOOP) || 
	     (xmDropInfo->dropAction != XmDROP) ) {

#ifdef DEBUG
		printf("dndDropProc: failed drop status or operation\n");
#endif
		dndTransferStartFailed(xmDropInfo->dragContext);
		return;
	}

	/*
	 * Get the cached DropInfo
	 */

	if ((dtDropInfo = dndFindDropInfo(display, dropReceiver)) == NULL) {
		dndTransferStartFailed(xmDropInfo->dragContext);
		return;
	}

	/*
	 * Get the export targets from the dragContext
	 */

	XtVaGetValues(xmDropInfo->dragContext,
		XmNexportTargets, &exportTargets,
		XmNnumExportTargets, &numExportTargets,
		NULL);

	if (exportTargets == NULL) {
		dndTransferStartFailed(xmDropInfo->dragContext);
		dtDropInfo->status = DtDND_FAILURE;
		return;
	}

#ifdef DEBUG
	printf("dndDropProc: Export Targets: ");
	_DtDndPrintTargets(display,exportTargets,numExportTargets);
#endif

	/*
	 * Search the DnD protocol import targets list to see
	 * if it's a target we handle.  If it is then start our transfer.
	 */

	transfer = _DtDndTransferFromTargets(
			dtDropInfo->transfers, dtDropInfo->numTransfers,
			exportTargets, numExportTargets);

	if (transfer != NULL) {

		dndTransferStart(dropReceiver, xmDropInfo, dtDropInfo,
			transfer, exportTargets, numExportTargets);
		return;
	}

	/*
	 * If there isn't any alternate drop registration
	 * then fail the transfer
	 */

	dsInfo = dtDropInfo->dropSiteInfo;

	if (dsInfo == NULL) {
		dndTransferStartFailed(xmDropInfo->dragContext);
		dtDropInfo->status = DtDND_FAILURE;
		return;
	}

	/*
	 * Determine if the exportTargets are compatible with
	 * the alternate drop registration targets
	 * If they are not compatible then fail the transfer
	 * Otherwise call the original dropProc.
	 */

	compatible = XmTargetsAreCompatible(display,
			dsInfo->importTargets, dsInfo->numImportTargets,
			exportTargets, numExportTargets);

	if (!compatible) {
		dndTransferStartFailed(xmDropInfo->dragContext);
		dtDropInfo->status = DtDND_FAILURE;
		return;
	}

	if (dsInfo->dropProc != NULL) {
		(*(dsInfo->dropProc))(dropReceiver, clientData, callData);
	}
}

/*
 * dndTransferStart
 *
 *	Start the transfer using protocol specific transfer entries
 *
 */
static void
dndTransferStart(
	Widget		dropReceiver,
	XmDropProcCallbackStruct *xmDropInfo,
	DtDropInfo *	dtDropInfo,
	DtDndTransfer *	transfer,
	Atom *		exportTargets,
	Cardinal	numExportTargets)
{
        XtCallbackRec	dropAnimateCbRec[] = { {dndDropAnimateCallback,
							NULL}, {NULL, NULL} };
	Display	*	display 	   = XtDisplayOfObject(dropReceiver);
	DtDndContext *	dropData;
	DtTransferInfo *transferInfo;
	Atom *		transferTargets;
	Cardinal	numTransferTargets;
	Boolean		owCompat;
	XmDropTransferEntryRec * transferEntries;
	Cardinal	numTransferEntries;
	int		posOffsetX, posOffsetY;
	Boolean		status;
	Arg		args[10];
	Cardinal	ii, nn;

#ifdef DEBUG
	printf("dndTransferStart: transfer method = %s\n",
					transfer->methods->name);
#endif
	/*
	 * If the operation is a move but not registered for a move
	 * then force it to a copy drop.
	 */

	if ((xmDropInfo->operation == XmDROP_MOVE) && 
	   !(dtDropInfo->operations & XmDROP_MOVE)) {
		xmDropInfo->operation = XmDROP_COPY;
	}

	/*
	 * Determine icon adjustment to drop position
	 */

	_DtDndGetIconOffset(xmDropInfo->dragContext,
			transfer->methods->sourceType,
			&posOffsetX, &posOffsetY);

	/*
	 * Initialize drop transfer info
	 */

	transferInfo = (DtTransferInfo *)XtMalloc(sizeof(DtTransferInfo));

	transferInfo->dragContext		= xmDropInfo->dragContext;
	transferInfo->protocol			= transfer->methods->protocol;
	transferInfo->operation			= xmDropInfo->operation;
	transferInfo->methods			= transfer->methods;
	transferInfo->transferTargets		= NULL;
	transferInfo->numTransferTargets	= 0;
	transferInfo->currentTransfer		= 0;
	transferInfo->appTransferCalled		= False;
	transferInfo->event			= xmDropInfo->event;
	transferInfo->x				= xmDropInfo->x + posOffsetX;
	transferInfo->y				= xmDropInfo->y + posOffsetY;
	transferInfo->clientData		= NULL;

	/*
	 * Initialize drop data
	 */

	dropData = (DtDndContext *)XtCalloc(1,sizeof(DtDndContext));

	dropData->protocol		= transferInfo->protocol;

	/*
	 * Initialize drop transfer fields of DtDropInfo
	 */

	dtDropInfo->transferInfo	= transferInfo;
	dtDropInfo->dropData 		= dropData;
	dtDropInfo->status 		= DtDND_SUCCESS;

	/* 
	 * Get protocol specific transfer targets 
	 */

	(*transferInfo->methods->transferTargets)(dtDropInfo,
			exportTargets, numExportTargets,
			&transferTargets, &numTransferTargets);

	if (transferTargets == NULL) {

		XtFree((char *)dtDropInfo->transferInfo);
		XtFree((char *)dtDropInfo->dropData);

		dtDropInfo->transferInfo	= NULL;
		dtDropInfo->dropData 		= NULL;
		dtDropInfo->status 		= DtDND_FAILURE;

		dndTransferStartFailed(xmDropInfo->dragContext);
		return;
	}

	/*
	 * Convert _SUN_ENUMERATION_COUNT if available
	 * Insert into transfer targets list
	 */
	owCompat = XmTargetsAreCompatible(display, 
			exportTargets, numExportTargets,
			&XA_SUN_ENUM_COUNT, 1);

	if (owCompat) {
		Cardinal	jj, numTargets;
		Atom		*targets;

		numTargets 	= numTransferTargets + 1;
		targets 	= (Atom *)XtMalloc(numTargets * sizeof(Atom));

		jj = 0;
		targets[jj++] = XA_SUN_ENUM_COUNT;

		for (ii = 0; ii < numTransferTargets; ii++, jj++) {
			targets[jj] = transferTargets[ii];
		}

		XtFree((char *)transferTargets);

		transferTargets		= targets;
		numTransferTargets 	= numTargets;
	}

	/*
 	 * Create a transfer entries list from the transfer targets
	 */

	numTransferEntries = numTransferTargets;
	transferEntries = (XmDropTransferEntryRec *)
		XtMalloc(numTransferEntries * sizeof(XmDropTransferEntryRec));

	for (ii = 0; ii < numTransferEntries; ii++) {
		transferEntries[ii].target	= transferTargets[ii];
		transferEntries[ii].client_data	= (XtPointer)dtDropInfo;
	}

	transferInfo->transferTargets		= transferTargets;
	transferInfo->numTransferTargets	= numTransferTargets;

#ifdef DEBUG
	printf("Requesting transfers: ");
	_DtDndPrintTargets(display,transferTargets,numTransferTargets);
#endif

	/*
	 * Start the drop transfer
	 */

	dropAnimateCbRec[0].closure = (XtPointer) dtDropInfo;
	transferInfo->dropAnimateCallback
			= _DtDndCopyCallbackList(dropAnimateCbRec);
	nn = 0;
	XtSetArg(args[nn], XmNdropTransfers, 	transferEntries); nn++;
	XtSetArg(args[nn], XmNnumDropTransfers,	numTransferEntries);nn++;
	XtSetArg(args[nn], XmNtransferProc, 	dndTransferProc); nn++;
	XtSetArg(args[nn], XmNdestroyCallback, 	transferInfo->dropAnimateCallback); nn++;

	(void)XmDropTransferStart(xmDropInfo->dragContext, args, nn);

        XtFree((char *)transferEntries);
}

/*
 * dndTransferProc
 *
 * 	Process the transfers
 */
static void
dndTransferProc(
	Widget		dropTransfer,
	XtPointer	clientData,
	Atom *		selection,
	Atom *		type,
	XtPointer	value,
	unsigned long *	length,
	int *		format)
{
	DtDropInfo *	dtDropInfo 	= (DtDropInfo *)clientData;
	DtTransferInfo *transferInfo	= dtDropInfo->transferInfo;
	int		index;
	Atom		target;

	/*
	 * Ignore if no dtDropInfo or failed transfer
	 * Ignore null transfer (which are responses to DELETE)
	 */

	if (value == NULL || dtDropInfo == NULL || 
	    dtDropInfo->status == DtDND_FAILURE) {
		dndTransferFail(dtDropInfo, dropTransfer);
		XtFree(value);
		return;
	} else if (*type == XA_NULL) {
		XtFree(value);
		return;
	}

	/*
	 * Determine current transfer target
	 */

	index = transferInfo->currentTransfer;
	
	if (index < transferInfo->numTransferTargets) {
		target = transferInfo->transferTargets[index];
	} else {
		target = None;
	}
	transferInfo->currentTransfer++;

#ifdef DEBUG
	{
	Display *display = XtDisplayOfObject(dropTransfer);
	char *targetname = XGetAtomName(display,target);
	char *typename   = XGetAtomName(display,*type);
	printf("dndTransferProc: target = %s type = %s fmt = %d len = %ld\n",
			(targetname ? targetname : "Null"), 
			(typename ? typename : "Null"),
			 *format, *length);
	if (targetname) XFree(targetname);
	if (typename) XFree(typename);
	}
#endif

	/*
	 * Handle _SUN_ENUMERATION_COUNT request; reject multiple items
	 */
	if (target == XA_SUN_ENUM_COUNT) {
		int *	enumCount = (int *)value;
		if (enumCount[0] > 1) {
			dndTransferFail(dtDropInfo, dropTransfer);
		}
		XtFree(value);
		return;
	}

	/*
	 * Invoke protocol specific transfer proc
	 */

	(*transferInfo->methods->transfer)( dropTransfer, dtDropInfo, 
		selection, &target, type, value, length, format);

	/*
	 * If the transfer failed, set up to terminate unsuccessfully
	 */

	if (dtDropInfo->status == DtDND_FAILURE) {
		dndTransferFail(dtDropInfo, dropTransfer);
		return;
	}

	/*
	 * If the transfers are complete;
	 * If the dropData isn't ready then fail the transfer
	 * Otherwise call the application transfer callback
	 */

	if (transferInfo->currentTransfer == transferInfo->numTransferTargets) {

		if (dtDropInfo->dropData->numItems == 0) {
			dndTransferFail(dtDropInfo, dropTransfer);
			return;
		} else if (!transferInfo->appTransferCalled) {
			dndAppTransfer(dropTransfer, dtDropInfo);
			transferInfo->appTransferCalled = True;
		}
	}
}

/*
 * dndAppTransfer
 *
 *	 Call the application transfer callback
 */
static void
dndAppTransfer(
	Widget		dropTransfer,
	DtDropInfo *	dtDropInfo)
{
	DtTransferInfo *transferInfo 	= dtDropInfo->transferInfo;
	DtDndTransferCallbackStruct	transferCallData;

	/*
	 * If the transfer failed or there isn't a callback simply return
	 */

	if (dtDropInfo->status == DtDND_FAILURE ||
	    dtDropInfo->dropTransferCallback == NULL) {
		return;
	}

	/*
	 * Fill in the callback structure and call the
 	 * application-defined dropTransferCallback.
	 */

	transferCallData.reason		= DtCR_DND_TRANSFER_DATA;
	transferCallData.event		= transferInfo->event;
	transferCallData.x		= transferInfo->x;
	transferCallData.y		= transferInfo->y;
	transferCallData.operation	= transferInfo->operation;
	transferCallData.dropData	= dtDropInfo->dropData;
	transferCallData.dragContext	= transferInfo->dragContext;
	transferCallData.status		= DtDND_SUCCESS;

	if (transferInfo->operation == XmDROP_MOVE) {
		transferCallData.completeMove = True;
	} else {
		transferCallData.completeMove = False;
	}

	_DtDndCallCallbackList(dtDropInfo->dropReceiver, 
		dtDropInfo->dropTransferCallback, 
		(XtPointer)&transferCallData);

	dtDropInfo->status = transferCallData.status;

	/*
	 * If the app requests it then fail the transfer
	 */
	if (transferCallData.status == DtDND_FAILURE) {
		dndTransferFail(dtDropInfo, dropTransfer);
		return;
	}

	/*
	 * If the transfer succeeded and this is a move operation
	 * then transfer DELETE to delete the original.
	 */

	if (transferCallData.status == DtDND_SUCCESS &&
	    transferCallData.completeMove &&
	    transferInfo->operation == XmDROP_MOVE) {

		XmDropTransferEntryRec	transferEntries[1];

		transferEntries[0].target	= XA_DELETE;
		transferEntries[0].client_data	= (XtPointer)dtDropInfo;

		XmDropTransferAdd(dropTransfer, transferEntries, 1);
	}
}

/*
 * dndDropAnimateCallback
 *
 * 	Call the application dropAnimateCallback
 */
static void
dndDropAnimateCallback(
	Widget		dropTransfer,
	XtPointer	clientData,
	XtPointer	callData)
{
	DtDropInfo *	dtDropInfo 	= (DtDropInfo *)clientData;
	DtTransferInfo *transferInfo	= dtDropInfo->transferInfo;
	DtDndDropAnimateCallbackStruct	dropAnimateCallData;

	/*
	 * Fill in the callback structure and call the
 	 * application-defined dropAnimateCallback.
	 */

	if (dtDropInfo->status == DtDND_SUCCESS &&
	    dtDropInfo->dropAnimateCallback != NULL) {

		dropAnimateCallData.reason	= DtCR_DND_DROP_ANIMATE;
		dropAnimateCallData.event     	= transferInfo->event;
		dropAnimateCallData.x		= transferInfo->x;
		dropAnimateCallData.y		= transferInfo->y;
		dropAnimateCallData.operation	= transferInfo->operation;
		dropAnimateCallData.dropData	= dtDropInfo->dropData;

		_DtDndCallCallbackList(dtDropInfo->dropReceiver, 
			dtDropInfo->dropAnimateCallback, 
			(XtPointer)&dropAnimateCallData);
	}

	/*
	 * Invoke the protocol specific transfer finish proc
	 */

	(*transferInfo->methods->transferFinish)(dtDropInfo);

	/*
	 * Free transfer created memory 
	 */
	XtFree((char *)dtDropInfo->transferInfo->transferTargets);
	XtFree((char *)dtDropInfo->transferInfo->dropAnimateCallback); 
	XtFree((char *)dtDropInfo->transferInfo);
	XtFree((char *)dtDropInfo->dropData);
	
	dtDropInfo->transferInfo 	= NULL;
	dtDropInfo->dropData 		= NULL;
}

/*
 * dndDropSiteDestroy
 *
 * Destroy callback unregisters the widget being destroyed as a drop site
 */
static void
dndDropSiteDestroy(
	Widget		widget,
	XtPointer	clientData,
	XtPointer	callData)
{
	DtDndDropUnregister(widget);
}
	
/*
 * dndGetContextXID
 *
 *	Returns a pixmap to use as the XID for Save/Find Context
 */
static XID
dndGetContextXID(
	Display *	display)
{
        static XID contextXID;

	_DtSvcProcessLock();
	if (contextXID == 0) {
		contextXID = XCreatePixmap(display, 
					DefaultRootWindow(display), 1, 1, 1);
	}
	_DtSvcProcessUnlock();
	return contextXID;
}

/*
 * dndSaveDropInfo
 *
 *	Saves the DtDropInfo relative to the dropReceiver
 */
static Boolean 		
dndSaveDropInfo(
	Display *	display,
	Widget		dropReceiver,
	DtDropInfo *	dtDropInfo)
{
	int		status;

	status = XSaveContext(display, dndGetContextXID(display),
		(XContext)(intptr_t)dropReceiver, (XPointer)dtDropInfo);

	return (status == 0);
}

/*
 * dndFindDropInfo
 *
 *	Finds the DtDropInfo saved relative to the dropReceiver
 */
static DtDropInfo *
dndFindDropInfo(
	Display *	display,
	Widget		dropReceiver)
{
	int		status;
	DtDropInfo *	dtDropInfo;

	status = XFindContext(display, dndGetContextXID(display), 
		(XContext)(intptr_t)dropReceiver, (XPointer *)&dtDropInfo);

	if (status != 0)
		dtDropInfo = (DtDropInfo *)NULL;
	
	return dtDropInfo;
}

/*
 * dndTransferStartFailed
 *
 * 	Fail the transfer by starting a 'failure' transfer
 *	Calls XmDropTransferStart()
 */
static void
dndTransferStartFailed(
	Widget	dragContext)
{
	Arg 	args[2];
	int 	nn = 0;

	XtSetArg(args[nn], XmNtransferStatus, XmTRANSFER_FAILURE); nn++;
	XtSetArg(args[nn], XmNnumDropTransfers, 0); nn++;

	XmDropTransferStart(dragContext, args, nn);
}

/*
 * dndTransferFail
 *
 * 	Fail the transfer by setting the dropTransfer widget to failure
 *	Assumes XmDropTransferStart() already called.
 */
static void
dndTransferFail(
	DtDropInfo *	dtDropInfo,
	Widget		dropTransfer)
{
	if (dtDropInfo)
		dtDropInfo->status = DtDND_FAILURE;

	XtVaSetValues(dropTransfer,
		XmNtransferStatus,	XmTRANSFER_FAILURE,
		XmNnumDropTransfers,	0,
		NULL);
}

