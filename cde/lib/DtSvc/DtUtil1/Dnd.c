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
/* $TOG: Dnd.c /main/7 1998/10/23 13:47:10 mgreess $ */
 /*********************************************************************
 *
 *	File:		Dnd.c
 *
 *	Description:	Implemenation of DND Convenience API.
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
#include <sys/utsname.h>
#include <X11/Intrinsic.h> 
#include <Xm/Xm.h>
#include <Xm/DragDrop.h>
#include <Xm/DropTrans.h>
#include <Xm/AtomMgr.h>
#include <Dt/UserMsg.h>
#include "Dnd.h"
#include "DndP.h"
#include "DtSvcLock.h"

/*********************************************************************
 *
 * Data Transfer Selection Targets
 *
 *********************************************************************/

Atom	XA_TARGETS;
Atom	XA_TIMESTAMP;
Atom	XA_MULTIPLE;
Atom	XA_DELETE;
Atom	XA_NULL;
Atom	XA_TEXT;
Atom	XA_HOST_NAME;
Atom	XA_SUN_FILE_HOST_NAME;
Atom	XA_SUN_ENUM_COUNT;
Atom	XA_SUN_DATA_LABEL;
Atom	XA_SUN_SELN_READONLY;
Atom	XA_SUN_ATM_FILE_NAME;
Atom	XA_SUN_ATM_METHODS;

static void
dndInitTargets(
	Display	*dpy)
{
        _DtSvcProcessLock();
	if (XA_TARGETS != 0) {
	        _DtSvcProcessUnlock();
		return;
	      }

	XA_TARGETS		= DtGetAtom(dpy,"TARGETS");
	XA_TIMESTAMP		= DtGetAtom(dpy,"TIMESTAMP");
	XA_MULTIPLE		= DtGetAtom(dpy,"MULTIPLE");
	XA_DELETE		= DtGetAtom(dpy,"DELETE");
	XA_NULL			= DtGetAtom(dpy,"NULL");
	XA_TEXT			= DtGetAtom(dpy,"TEXT");
	XA_HOST_NAME		= DtGetAtom(dpy,"HOST_NAME");
	XA_SUN_FILE_HOST_NAME	= DtGetAtom(dpy,"_SUN_FILE_HOST_NAME");
	XA_SUN_ENUM_COUNT 	= DtGetAtom(dpy,"_SUN_ENUMERATION_COUNT");
	XA_SUN_DATA_LABEL 	= DtGetAtom(dpy,"_SUN_DATA_LABEL");
	XA_SUN_SELN_READONLY 	= DtGetAtom(dpy,"_SUN_SELN_IS_READONLY");
	XA_SUN_ATM_FILE_NAME	= DtGetAtom(dpy,"_SUN_ATM_FILE_NAME");
	XA_SUN_ATM_METHODS	= DtGetAtom(dpy,
					"_SUN_ALTERNATE_TRANSPORT_METHODS");
	_DtSvcProcessUnlock();
}

/*********************************************************************
 *
 * Data Transfer Protocol Method Functions
 *
 *********************************************************************/

static	DtDndProtocol	dndProtocolList[] = {
	DtDND_FILENAME_TRANSFER,
	DtDND_BUFFER_TRANSFER,
	DtDND_TEXT_TRANSFER,
	DtDND_NOOP_TRANSFER
};

extern DtDndMethods *_DtDndBuffTransferProtocolInitialize(Display*);
extern DtDndMethods *_DtDndFileTransferProtocolInitialize(Display*);
extern DtDndMethods *_DtDndTextTransferProtocolInitialize(Display*);

/*
 * dndGetTransferMethods
 *
 *	Return the transfer methods for a particular protocol
 */
static DtDndMethods *
dndGetTransferMethods(
	Display *	display,
	DtDndProtocol	protocol)
{
	DtDndMethods *	methods = NULL;

	switch (protocol) {
	case DtDND_BUFFER_TRANSFER:
		methods = _DtDndBuffTransferProtocolInitialize(display);
		break;
	case DtDND_FILENAME_TRANSFER:
		methods = _DtDndFileTransferProtocolInitialize(display);
		break;
	case DtDND_TEXT_TRANSFER:
		methods = _DtDndTextTransferProtocolInitialize(display);
		break;
	}

	return methods;
}

/*
 * _DtDndCreateExportTransfer
 *
 *	Returns a transfer suitable for use by a drag initiator
 */
DtDndTransfer *
_DtDndCreateExportTransfer(
	DtDragInfo *	dtDragInfo)
{
	Display *	dpy = XtDisplayOfObject(dtDragInfo->dragInitiator);
	DtDndTransfer *	transfer;

	dndInitTargets(dpy);

	transfer = (DtDndTransfer *)XtMalloc(sizeof(DtDndTransfer));

	transfer->methods = dndGetTransferMethods(dpy,dtDragInfo->protocol);

	(*transfer->methods->getExportTargets)(dtDragInfo,
			&(transfer->targets), &(transfer->numTargets));

	return transfer;
}

/*
 * _DtDndCreateImportTransfers
 *
 *	Create the list of transfers specified by the protocols.
 */
DtDndTransfer *
_DtDndCreateImportTransfers(
	DtDropInfo *	dtDropInfo,
	Cardinal *	numTransfers)
{
	Display *	dpy = XtDisplayOfObject(dtDropInfo->dropReceiver);
	DtDndTransfer *	transfers;
	DtDndTransfer *	transfer;
	Cardinal	ii, jj;

	dndInitTargets(dpy);

	*numTransfers = 0;

	for (ii = 0; dndProtocolList[ii] != DtDND_NOOP_TRANSFER; ii++) {
		if (dtDropInfo->protocols & dndProtocolList[ii]) {
			(*numTransfers)++;
		}
	}

	if (*numTransfers == 0)
		return (DtDndTransfer *)NULL;
	
	transfers = (DtDndTransfer *)XtMalloc(
				*numTransfers * sizeof(DtDndTransfer));

	for (ii = 0, jj = 0; dndProtocolList[ii] != DtDND_NOOP_TRANSFER; ii++) {
		if (dtDropInfo->protocols & dndProtocolList[ii]) {
			transfer = &transfers[jj++];
			transfer->methods = 
				dndGetTransferMethods(dpy,dndProtocolList[ii]);
			(*transfer->methods->getImportTargets)(dtDropInfo,
				&(transfer->targets), &(transfer->numTargets));
		}
	}

	return transfers;
}

/*
 * _DtDndDestroyTransfers
 *
 *	Destroy the transfer list created by
 *	_DtDndCreateExportTransfer() or _DtDndCreateImportTransfers()
 */
void
_DtDndDestroyTransfers(
	DtDndTransfer *	transfers,
	Cardinal	numTransfers)
{
	Cardinal	ii;

	
	for (ii = 0; ii < numTransfers; ii++) {
		XtFree((char *)transfers[ii].targets);
	}
	XtFree((char *)transfers);
}

/*
 * _DtDndTransferFromTargets
 *
 *	Returns the transfer method that matches the target list
 */
DtDndTransfer *	
_DtDndTransferFromTargets(
	DtDndTransfer *	transfers,
	Cardinal	numTransfers,
	Atom *		targets,
	Cardinal	numTargets)
{
	Cardinal	ii, jj, kk;

	for (ii = 0; ii < numTransfers; ii++) {
		for (jj = 0; jj < transfers[ii].numTargets; jj++) {
			for (kk = 0; kk < numTargets; kk++) {
				if (transfers[ii].targets[jj] == targets[kk]) {
					return &transfers[ii];
				}
			}
		}
	}
	return (DtDndTransfer *)NULL;
}

/*
 * _DtDndTransferAdd
 *
 *	Add the targets to the requested transfer targets list
 *	and call XmDropTransferAdd() to request them
 */
void
_DtDndTransferAdd(
	Widget		dropTransfer,
	DtDropInfo *	dtDropInfo,
	Atom *		transferTargets,
	Cardinal	numTransferTargets)
{
	DtTransferInfo *transferInfo = dtDropInfo->transferInfo;
	XmDropTransferEntryRec * transferEntries;
	Cardinal	numTransfers;
	int		ii, jj;

	transferEntries = (XmDropTransferEntryRec *)
		XtMalloc(numTransferTargets * sizeof(XmDropTransferEntryRec));

	numTransfers = transferInfo->numTransferTargets + numTransferTargets;

	transferInfo->transferTargets = (Atom *)XtRealloc(
			(char *)transferInfo->transferTargets,
			numTransfers * sizeof(Atom));

	jj = transferInfo->numTransferTargets;

	for (ii = 0; ii < numTransferTargets; ii++) {
		transferEntries[ii].target	= transferTargets[ii];
		transferEntries[ii].client_data	= (XtPointer)dtDropInfo;

		transferInfo->transferTargets[ii+jj] = transferTargets[ii];
	}

	transferInfo->numTransferTargets = numTransfers;

	XmDropTransferAdd(dropTransfer, transferEntries, numTransferTargets);

	XtFree((char *)transferEntries);
}

/*********************************************************************
 *
 * Misc Debugging Functions
 *
 *********************************************************************/

#ifdef DEBUG
void
_DtDndPrintTargets(
	Display		*display,
	Atom		*targets,
	Cardinal	numTargets)
{
	Cardinal	ii;
	char		*name;

	for (ii = 0; ii < numTargets; ii++) {
		name = XGetAtomName(display, targets[ii]);
		if (name) {
			printf("%s ", name);
			XFree(name);
		} else {
			printf("(null) ");
		}
	}
	printf("\n");
}

void
_DtDndPrintTransfers(
	Display		*display,
	DtDndTransfer	*transfers,
	Cardinal	numTransfers)
{
	Cardinal	ii;

	for (ii = 0; ii < numTransfers; ii++) {
		printf("%s\tTargets: ",transfers[ii].methods->name);
		_DtDndPrintTargets(display,
			transfers[ii].targets,transfers[ii].numTargets);
	}
}
#endif

/*********************************************************************
 *
 * Misc Utility Functions
 *
 *********************************************************************/

/*
 * Copy CallbackList
 */
XtCallbackList
_DtDndCopyCallbackList(
	XtCallbackList  callbacks)
{
	XtCallbackList	cl;
	int		ii, count;

	if (callbacks == NULL)
		return NULL;

	count = 1;
	for (cl = callbacks; cl->callback != NULL; cl++) {
		count++;
	}

	cl = (XtCallbackList)XtMalloc(count * sizeof(XtCallbackRec));

	for (ii = 0; ii < count; ii++) {
		cl[ii].callback = callbacks[ii].callback;
		cl[ii].closure  = callbacks[ii].closure;
	}
	
	return cl;
}

/*
 * Call CallbackList
 */
void
_DtDndCallCallbackList(
	Widget          widget,
	XtCallbackList  callbacks,
	XtPointer       calldata)
{
	XtCallbackList	cl;

	if (callbacks == NULL)
		return;

	for (cl = callbacks; cl->callback != NULL; cl++) {
		(*cl->callback)(widget, cl->closure, calldata);
	}
}

/*
 * Misc Varargs Utility Functions
 */

int
_DtDndCountVarArgs(
	va_list		vaList)
{
	XtPointer	argPtr;
	Cardinal	argCount;

	argCount = 0; 
	for (argPtr = va_arg(vaList,String);
	     argPtr != NULL; 
	     argPtr = va_arg(vaList,String)) {

		va_arg(vaList, XtArgVal);
		argCount++;
	}

	return argCount;
}

void
_DtDndArgListFromVarArgs(
	va_list		vaList,
	Cardinal	maxArgs,
	ArgList		*argListReturn,
	Cardinal	*argCountReturn)
{
	ArgList		argList;
	Cardinal	argCount;
	XtPointer	argPtr;

	if (0 == maxArgs)
	{
	    *argListReturn = NULL;
	    *argCountReturn = 0;
	    return;
	}

	argList = (ArgList)XtMalloc((unsigned)(maxArgs * sizeof(Arg)));

	argCount = 0;
	for (argPtr = va_arg(vaList,String);
	     argPtr != NULL;
	     argPtr = va_arg(vaList,String)) {

		XtSetArg(argList[argCount], argPtr, va_arg(vaList,XtArgVal));
		argCount++;
	}

	*argListReturn 		= argList;
	*argCountReturn 	= argCount;
}

/*
 * Returns the network node/host name.
 */
String
_DtDndGetHostName(void)
{
        static char *nodename;

	_DtSvcProcessLock();
	if (nodename == NULL) {
		struct utsname	un;

		if (uname(&un) == -1) {
			_DtSimpleError(DtProgName,DtError, NULL, "uname", NULL);
			nodename = XtNewString("nodename");
		} else {
			nodename = XtNewString(un.nodename);
		}
	}
	_DtSvcProcessUnlock();
	return nodename;
}

