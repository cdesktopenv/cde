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
/* $XConsortium: DndBuff.c /main/5 1996/06/21 17:30:55 ageorge $ */
/*********************************************************************
 *
 *	File:		DndBuff.c
 *
 *	Description:	Implementation of the Buffer Transfer routines
 *			FOR the DND Convenience API.
 *
 *********************************************************************
 *
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
 */

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <Xm/AtomMgr.h>
#include <Xm/DragC.h>
#include <Xm/DropSMgr.h>
#include <Xm/DropTrans.h>
#include "Dnd.h"
#include "DndP.h"
#include "DtSvcLock.h"

/* 
 * Buffer Transfer Protocol Function Prototypes
 */

static void	dndBuffGetTargets(Boolean, Boolean, Atom**, Cardinal*);
static void	dndBuffGetAvailTargets(DtDragInfo*, Atom**, Cardinal*);
static void	dndBuffGetExportTargets(DtDragInfo*, Atom**, Cardinal*);
static void	dndBuffGetImportTargets(DtDropInfo*, Atom**, Cardinal*);
static void	dndBuffConvertInit(DtDragInfo*);
static Boolean	dndBuffConvert(Widget, DtDragInfo*, Atom*, Atom*,
			Atom*, XtPointer*, unsigned long*, int*, 
			XSelectionRequestEvent*);
static void	dndBuffConvertFinish(DtDragInfo*);
static void	dndBuffTransferTargets(DtDropInfo*,
			Atom*, Cardinal, Atom**, Cardinal*);
static void	dndBuffTransfer(Widget, DtDropInfo*, Atom*, Atom*, Atom*,
			XtPointer, unsigned long*, int*);
static void	dndBuffTransferFinish(DtDropInfo*);

/* 
 * Buffer Transfer Support Functions
 */

static void	dndBufferDataToSelectionValue(DtDndBuffer *, Cardinal,
			XtPointer *, unsigned long *, Boolean);
static void	dndBufferLengthsToSelectionValue(DtDndBuffer *, Cardinal,
			XtPointer *, unsigned long *);
static void	dndBufferNamesToSelectionValue(DtDndBuffer *, Cardinal,
			XtPointer *, unsigned long *);
static void	dndTextSelectionValueToBuffer(Display *, 
			Atom *, XtPointer, unsigned long *, int *, String,
			XtPointer *, unsigned long *,
			DtDndBuffer **, Cardinal *);
static void	dndSelectionValueToBuffer(XtPointer, unsigned long,
			XtPointer, unsigned long, XtPointer, unsigned long,
			DtDndBuffer **, Cardinal *);

/* 
 * Buffer Transfer Selection Targets
 */

static Atom	XA_DT_BUFFER_DATA;
static Atom	XA_DT_BUFFER_LENGTHS;
static Atom	XA_DT_BUFFER_NAMES;

/* 
 * Buffer Transfer Protocol Methods
 */

static DtDndMethods dndBuffTransferProtocol = {
	"DtDndBufferTransfer",			/* name */
	(DtDndProtocol)DtDND_BUFFER_TRANSFER,	/* protocol */
	DtDND_DRAG_SOURCE_DATA,			/* sourceType */
	dndBuffGetAvailTargets,			/* getAvailTargets */
	dndBuffGetExportTargets,		/* getExportTargets */
	dndBuffGetImportTargets,		/* getImportTargets */
	dndBuffConvertInit,			/* convertInit */
	dndBuffConvert,				/* convert */
	dndBuffConvertFinish,			/* convertFinish */
	dndBuffTransferTargets,			/* transferTargets */
	dndBuffTransfer,			/* transfer */
	dndBuffTransferFinish,			/* transferFinish */
};

typedef struct _TransferState {
	XtPointer	dataValue;
	unsigned long	dataLength;
	XtPointer	lengthsValue;
	unsigned long	lengthsLength;
	XtPointer	namesValue;
	unsigned long	namesLength;
	String		dataLabel;
        Atom            textTarget;
} TransferState;

/*
 * Buffer transfer protocol initialization
 */
DtDndMethods *
_DtDndBuffTransferProtocolInitialize(
	Display *	dpy)
{
        _DtSvcProcessLock();
	if (XA_DT_BUFFER_DATA == 0) {
		XA_DT_BUFFER_DATA	= DtGetAtom(dpy,"_DT_BUFFER_DATA");
		XA_DT_BUFFER_LENGTHS	= DtGetAtom(dpy,"_DT_BUFFER_LENGTHS");
		XA_DT_BUFFER_NAMES	= DtGetAtom(dpy,"_DT_BUFFER_NAMES");
	}
	_DtSvcProcessUnlock();
	return &dndBuffTransferProtocol;
}

/*
 * Returns export/import targets for buffer transfers
 */
static void
dndBuffGetTargets(
	Boolean		includeText,
	Boolean		owCompat,
	Atom **		targets,
	Cardinal *	numTargets)
{
	int  		ii = 0;

	*numTargets = 3 + (includeText ? 2 : 0) + (owCompat ? 1 : 0);

	*targets = (Atom *)XtMalloc(*numTargets * sizeof(Atom));

	(*targets)[ii++] 	= XA_DT_BUFFER_DATA;
	(*targets)[ii++] 	= XA_DT_BUFFER_LENGTHS;
	(*targets)[ii++] 	= XA_DT_BUFFER_NAMES;

	if (includeText) {
		(*targets)[ii++] = XA_TEXT;
		(*targets)[ii++] = XA_STRING;
	}

	if (owCompat) {
		(*targets)[ii++] = XA_SUN_DATA_LABEL;
	}
}

/*
 * Returns available targets for buffer transfers
 */
static void
dndBuffGetAvailTargets(
	DtDragInfo *	dtDragInfo,
	Atom **		availTargets,
	Cardinal *	numAvailTargets)
{
	dndBuffGetTargets(True, True, availTargets, numAvailTargets);
}

/*
 * Returns export targets for buffer transfers
 */
static void
dndBuffGetExportTargets(
	DtDragInfo *	dtDragInfo,
	Atom **		exportTargets,
	Cardinal *	numExportTargets)
{
	dndBuffGetTargets(dtDragInfo->bufferIsText, True,
			exportTargets, numExportTargets);
}

/*
 * Returns import targets for buffer transfers
 */
static void
dndBuffGetImportTargets(
	DtDropInfo *	dtDropInfo,
	Atom **		importTargets,
	Cardinal *	numImportTargets)
{
	dndBuffGetTargets(dtDropInfo->textIsBuffer, True,
			importTargets, numImportTargets);
}

/*
 * Initialize protocol specific part of drag data
 */
static void
dndBuffConvertInit(
	DtDragInfo *	dtDragInfo)
{
	DtDndContext *	dragData = dtDragInfo->dragData;

	dragData->data.buffers = (DtDndBuffer *)
		XtCalloc(dragData->numItems, sizeof(DtDndBuffer));
}

/*
 * Convert the buffers into selection data
 */
static Boolean
dndBuffConvert(
        Widget          dragContext,
        DtDragInfo *	dtDragInfo,
        Atom *		selection,
        Atom *		target,  
        Atom *		returnType,
        XtPointer *	returnValue,
        unsigned long *	returnLength,
        int *		returnFormat,
        XSelectionRequestEvent * selectionRequestEvent)
{
	DtDndContext *	dragData = dtDragInfo->dragData;

	/*
	 * TEXT	or STRING
	 *	Convert the buffer data to a text selection
	 */
        if (*target == XA_TEXT || *target == XA_STRING) {
		*returnType 	= XA_STRING;
		*returnFormat	= 8;
                dndBufferDataToSelectionValue(
			dragData->data.buffers, dragData->numItems,
			returnValue, returnLength, True);
	/*
	 * _SUN_DATA_LABEL
	 *	Convert the first buffer name to a selection
	 */
        } else if (*target == XA_SUN_DATA_LABEL) {
		*returnType 	= XA_STRING;
		*returnFormat	= 8;
		*returnValue	= XtNewString(dragData->data.buffers[0].name);
		*returnLength	= strlen(*returnValue)+1;
	/*
	 * _DT_BUFFER_DATA
	 *	Convert the buffer data to a selection
	 */
        } else if (*target == XA_DT_BUFFER_DATA) {
		*returnType 	= XA_DT_BUFFER_DATA;
		*returnFormat	= 8;
                dndBufferDataToSelectionValue(
			dragData->data.buffers, dragData->numItems,
			returnValue, returnLength, False);
	/*
	 * _DT_BUFFER_LENGTHS
	 * 	Convert the buffer lengths to a selection
	 */
	} else if (*target == XA_DT_BUFFER_LENGTHS) {
		*returnType 	= XA_INTEGER;
		*returnFormat	= 32;
		dndBufferLengthsToSelectionValue(
			dragData->data.buffers, dragData->numItems,
			returnValue, returnLength);
	/*
	 * _DT_BUFFER_NAMES
	 * 	Convert the buffer names to a selection
	 */
	} else if (*target == XA_DT_BUFFER_NAMES) {
		*returnType 	= XA_STRING;
		*returnFormat	= 8;
                dndBufferNamesToSelectionValue(
			dragData->data.buffers, dragData->numItems,
			returnValue, returnLength);
	} else {
                return False;
	}

	return True;
}

/*
 * Clean up from the convert init proc
 */
static void
dndBuffConvertFinish(
	DtDragInfo *	dtDragInfo)
{
	DtDndContext *	dragData = dtDragInfo->dragData;

	if (dragData->data.buffers) {
		XtFree((char *)dragData->data.buffers);
		dragData->data.buffers = NULL;
	}
}

/*
 * Returns the transfer targets selected from the export targets
 * Prefer buffers over TEXT/STRING
 */
static void
dndBuffTransferTargets(
	DtDropInfo *	dtDropInfo,
	Atom *		exportTargets,
	Cardinal 	numExportTargets,
	Atom **		transferTargets,
	Cardinal *	numTransferTargets)
{
 	Boolean		foundData, foundLengths, foundNames, 
			foundDataLabel, foundText, foundString;
	int		ii;
	TransferState *	xferState; 

        xferState = (TransferState *)XtCalloc(1,sizeof(TransferState));
        dtDropInfo->transferInfo->clientData = (XtPointer)xferState;


	foundData = foundLengths = foundNames = False;
	foundDataLabel = foundText = foundString = False;

	for (ii = 0; ii < numExportTargets; ii++) {
		if (exportTargets[ii] == XA_DT_BUFFER_DATA) {
			foundData = True;
		} else if (exportTargets[ii] == XA_DT_BUFFER_LENGTHS) {
			foundLengths = True;
		} else if (exportTargets[ii] == XA_DT_BUFFER_NAMES) {
			foundNames = True;
		} else if (exportTargets[ii] == XA_SUN_DATA_LABEL) {
			foundDataLabel = True;
		} else if (exportTargets[ii] == XA_TEXT) {
			foundText = True;
		} else if (exportTargets[ii] == XA_STRING) {
			foundString = True;
		}
	}

        *numTransferTargets = 2;        /* max number of transfer targets */
   
	*transferTargets = (Atom *)XtMalloc(*numTransferTargets * sizeof(Atom));

	ii = 0;

	if (foundData && foundLengths && foundNames) {
                *numTransferTargets = 2;
                (*transferTargets)[ii++] = XA_DT_BUFFER_NAMES;
		(*transferTargets)[ii++] = XA_DT_BUFFER_LENGTHS;
	} else if (foundDataLabel && (foundText || foundString)) {
		*numTransferTargets = 2;
                (*transferTargets)[ii++] = XA_SUN_DATA_LABEL;
                (*transferTargets)[ii++] = foundText ? XA_TEXT : XA_STRING;
		xferState->textTarget     = foundText ? XA_TEXT : XA_STRING;
	} else if (foundText || foundString) {
		*numTransferTargets = 1;
                (*transferTargets)[ii++] = foundText ? XA_TEXT : XA_STRING;
	} else {
		*numTransferTargets	= 0;
		*transferTargets	= NULL;
		return;
	}
}

/*
 * Transfer the selection data into buffers
 */
static void
dndBuffTransfer(
        Widget          dropTransfer,
        DtDropInfo *	dtDropInfo,
        Atom *		selection,
	Atom *		target,
        Atom *		type,
        XtPointer       value,
        unsigned long *	length,
        int *		format)
{
	Display *	display  = XtDisplayOfObject(dropTransfer);
	DtDndContext *	dropData = dtDropInfo->dropData;
	TransferState *	xferState = 
			(TransferState *)dtDropInfo->transferInfo->clientData;

	/*
	 * Ignore if we've already transferred
	 */
	if (dropData->data.buffers) {
		XtFree(value);
		return;
	}

	/*
	 * TEXT or STRING
	 *
	 * Convert the text selection into buffers
	 */
        if (*target == XA_TEXT || *target == XA_STRING) {

		dndTextSelectionValueToBuffer(display, 
			type, value, length, format, xferState->dataLabel,
			&(xferState->dataValue), &(xferState->dataLength),
			&(dropData->data.buffers), &(dropData->numItems));

	/*
	 * _SUN_DATA_LABEL
	 *
	 * Save the buffer name selection for later conversion
         * Request the text data transfer
	 */
        } else if (*target == XA_SUN_DATA_LABEL &&
		   *type   == XA_STRING &&
		   *format == 8) {

		xferState->dataLabel	= (String)value;
   
                _DtDndTransferAdd(dropTransfer, dtDropInfo,
                                  &(xferState->textTarget), 1);
	/*
	 * _DT_BUFFER_DATA
	 *
	 * Save the buffer data selection for later conversion
	 */
        } else if (*target == XA_DT_BUFFER_DATA &&
		   *type   == XA_DT_BUFFER_DATA &&
		   *format == 8) {

		xferState->dataValue	= value;
		xferState->dataLength	= *length;
   
                dndSelectionValueToBuffer(
                        xferState->dataValue, xferState->dataLength,
                        xferState->lengthsValue, xferState->lengthsLength,
                        xferState->namesValue, xferState->namesLength,
                        &(dropData->data.buffers), &(dropData->numItems));
   

	/*
	 * _DT_BUFFER_LENGTHS
	 *
	 * Save the buffer lengths selection for later conversion
         * Request the buffer data transfer
	 */
	} else if (*target == XA_DT_BUFFER_LENGTHS &&
		   *type   == XA_INTEGER &&
		   *format == 32) {

		xferState->lengthsValue	 = value;
		xferState->lengthsLength = *length;
   
                _DtDndTransferAdd(dropTransfer, dtDropInfo,
                                  &XA_DT_BUFFER_DATA, 1);
	/*
	 * _DT_BUFFER_NAMES
	 *
	 * Save the buffer names selection for later conversion
	 */
	} else if (*target == XA_DT_BUFFER_NAMES &&
		   *type   == XA_STRING &&
		   *format == 8) {

		xferState->namesValue	= value;
		xferState->namesLength	= *length;

	/*
	 * Ignore transfers we don't understand
	 */
	} else {
		XtFree(value);
		return;
	}
}

/*
 * Clean up from the transfer proc
 */
static void
dndBuffTransferFinish(
	DtDropInfo *	dtDropInfo)
{
	DtDndContext *	dropData = dtDropInfo->dropData;
	TransferState *	xferState = 
			(TransferState *)dtDropInfo->transferInfo->clientData;

	dtDropInfo->transferInfo->clientData = NULL;

	if (xferState->dataValue)
		XtFree(xferState->dataValue);
	if (xferState->lengthsValue)
		XtFree(xferState->lengthsValue);
	if (xferState->namesValue)
		XtFree(xferState->namesValue);
	if (xferState->dataLabel)
		XtFree(xferState->dataLabel);
	XtFree((char *)xferState);
	XtFree((char *)dropData->data.buffers);
}

/*
 * Convert buffer data into a single chunk of memory
 */
static void
dndBufferDataToSelectionValue(
	DtDndBuffer *	buffers,
	Cardinal	numBuffers,
	XtPointer *	returnValue,
	unsigned long *	returnLength,
	Boolean		isText)
{
	int		 ii;
	char		*bufferPtr;

	*returnLength = 0;

	for (ii = 0; ii < numBuffers; ii++) {
		*returnLength += buffers[ii].size;
		if (isText) {
			*returnLength += 1;
		}
	}

	*returnValue = (XtPointer) XtMalloc(*returnLength);

	bufferPtr = (XtPointer)*returnValue;

	for (ii = 0; ii < numBuffers; ii++) {
		memcpy(bufferPtr, buffers[ii].bp, buffers[ii].size);
		bufferPtr += buffers[ii].size;
		if (isText) {
			*bufferPtr = '\0';
			bufferPtr++;
		}
	}

	if (isText) {
		*returnLength -= 1;
	}
}

/*
 * Convert buffer lengths into a list of integers
 */
static void
dndBufferLengthsToSelectionValue(
	DtDndBuffer *	buffers,
	Cardinal	numBuffers,
	XtPointer *	returnValue,
	unsigned long *	returnLength)
{
	int		ii;
	unsigned long *	lengths;

	*returnLength = numBuffers;

	*returnValue = (XtPointer) XtMalloc(*returnLength * sizeof(long));

	lengths = (unsigned long *)*returnValue;
 
	for (ii = 0; ii < numBuffers; ii++) {
		lengths[ii] = buffers[ii].size;
	}
}

/*
 * Convert buffer names into a single chunk of memory
 */
static void
dndBufferNamesToSelectionValue(
	DtDndBuffer *	buffers,
	Cardinal	numBuffers,
	XtPointer *	returnValue,
	unsigned long *	returnLength)
{
	int		ii, len;
	char *		namePtr;

	*returnLength = 0;

	for (ii = 0; ii < numBuffers; ii++) {
		if (buffers[ii].name != NULL) {
			*returnLength += strlen(buffers[ii].name) + 1;
		} else {
			*returnLength += 1;
		}
	}

	*returnValue = (XtPointer) XtMalloc(*returnLength);

	namePtr = (char *)*returnValue;

	for (ii = 0; ii < numBuffers; ii++) {
		if (buffers[ii].name == NULL) {
			*namePtr = '\0';
			namePtr++;
		} else {
			len = strlen(buffers[ii].name) + 1;
			memcpy(namePtr, buffers[ii].name, len);
			namePtr += len;
		}
	}
}

/*
 * Split text selection data into multiple buffers
 */
static void
dndTextSelectionValueToBuffer(
	Display *	display,
	Atom *		type,
	XtPointer	value,
	unsigned long * length,
	int *		format,
	String		dataLabel,
	XtPointer *	returnValue,
	unsigned long * returnLength,
	DtDndBuffer **	buffers,
	Cardinal *	numBuffers)
{
	XTextProperty	textProp;
	char **		text;
	char *		bufPtr;
	int		ii, status, textCount, textLen;

	textProp.value		= (unsigned char *)value;
	textProp.encoding	= *type;
	textProp.format		= *format;
	textProp.nitems		= *length;

	status = XmbTextPropertyToTextList(display, &textProp, 
			&text, &textCount);

	if (status != Success) {
		*numBuffers 	= 1;
		(*buffers) 	= (DtDndBuffer *)XtMalloc(sizeof(DtDndBuffer));

		(*buffers)[0].bp	= value;
		(*buffers)[0].size	= *length;
		(*buffers)[0].name 	= dataLabel ? dataLabel : (String)NULL;

		*returnValue		= value;
		*returnLength		= *length;

		return;
	}

	*numBuffers = textCount;

	(*buffers) = (DtDndBuffer *)XtMalloc(*numBuffers * sizeof(DtDndBuffer));

	textLen = 0;
	for (ii = 0; ii < *numBuffers; ii++) {
		(*buffers)[ii].size 	= strlen(text[ii]);
		(*buffers)[ii].name 	= (String)NULL;
		textLen += (*buffers)[ii].size+1;
	}

	(*returnValue)  = XtMalloc(textLen * sizeof(char));
	(*returnLength) = textLen;

	bufPtr = (*returnValue);
	for (ii = 0; ii < *numBuffers; ii++) {
		(*buffers)[ii].bp 	= bufPtr;
		memcpy(bufPtr,text[ii],(*buffers)[ii].size+1);
		bufPtr += (*buffers)[ii].size+1;
	}

	if (dataLabel)
		(*buffers)[0].name	= dataLabel;

	XtFree(value);
	XFreeStringList(text);
}

/*
 * Split data/lengths/names selection data into multiple buffers
 */
static void
dndSelectionValueToBuffer(
	XtPointer	dataValue,
	unsigned long	dataLength,
	XtPointer	lengthsValue,
	unsigned long	lengthsLength,
	XtPointer	namesValue,
	unsigned long	namesLength,
	DtDndBuffer **	buffers,
	Cardinal *	numBuffers)
{
	int		ii;
	char *		bufPtr;
	char *		namePtr;
	unsigned long *	lenList;

	if (lengthsLength == 0 || lengthsValue == NULL) {

		*numBuffers = 1;

		(*buffers) = (DtDndBuffer *)XtMalloc(sizeof(DtDndBuffer));

		(*buffers)[0].bp	= dataValue;
		(*buffers)[0].size	= dataLength;
		(*buffers)[0].name	= (String)NULL;

		return;
	}

	*numBuffers = lengthsLength;

	(*buffers) = (DtDndBuffer *)XtMalloc(*numBuffers * sizeof(DtDndBuffer));

	lenList = (unsigned long *)lengthsValue;
	bufPtr  = dataValue;
	namePtr	= namesValue;

	for (ii = 0; ii < *numBuffers; ii++) {
		(*buffers)[ii].bp	= bufPtr;
		(*buffers)[ii].size 	= lenList[ii];
		(*buffers)[ii].name 	= namePtr;
		bufPtr += lenList[ii];
		namePtr = strchr(namePtr, '\0');
		namePtr++;
	}
}
