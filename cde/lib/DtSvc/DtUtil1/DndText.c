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
/* $XConsortium: DndText.c /main/5 1996/08/28 17:46:27 drk $ */
/*********************************************************************
 *
 *	File:		DndText.c
 *
 *	Description:	Implementation of the Text Transfer routines
 *			for the DND Convenience API.
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

#include <stdlib.h>
#include <locale.h>
#include <X11/Intrinsic.h>
#include <Xm/AtomMgr.h>
#include <Xm/DragC.h>
#include <Xm/DropSMgr.h>
#include <Xm/DropTrans.h>
#include "Dnd.h"
#include "DndP.h"
#include "DtSvcLock.h"

/* 
 * Text Transfer Function Prototypes
 */

static void	dndTextGetTargets(Atom**, Cardinal*);
static void	dndTextGetAvailTargets(DtDragInfo*, Atom**, Cardinal*);
static void	dndTextGetExportTargets(DtDragInfo*, Atom**, Cardinal*);
static void	dndTextGetImportTargets(DtDropInfo*, Atom**, Cardinal*);
static void	dndTextConvertInit(DtDragInfo*);
static Boolean	dndTextConvert(Widget, DtDragInfo*, Atom*, Atom*,
			Atom*, XtPointer*, unsigned long*, int*, 
			XSelectionRequestEvent*);
static void	dndTextConvertFinish(DtDragInfo*);
static void	dndTextTransferTargets(DtDropInfo*,
			Atom*, Cardinal, Atom**, Cardinal*);
static void	dndTextTransfer(Widget, DtDropInfo*, Atom*, Atom*,
			Atom*, XtPointer, unsigned long*, int*);
static void	dndTextTransferFinish(DtDropInfo*);

/* 
 * Text Transfer Selection Target
 */

static Atom 	XA_COMPOUND_TEXT;
static Boolean	dndPreferString;

/* 
 * Text Transfer ProtocolInfo
 */

static DtDndMethods dndTextTransferProtocol = {
	"DtDndTextTransfer",			/* name */
	(DtDndProtocol)DtDND_TEXT_TRANSFER,	/* protocol */
	DtDND_DRAG_SOURCE_TEXT,			/* sourceType */
	dndTextGetAvailTargets,			/* getAvailTargets */
	dndTextGetExportTargets,		/* getExportTargets */
	dndTextGetImportTargets,		/* getImportTargets */
	dndTextConvertInit,			/* convertInit */
	dndTextConvert,				/* convert */
	dndTextConvertFinish,			/* convertFinish */
	dndTextTransferTargets,			/* transferTargets */
	dndTextTransfer,			/* transfer */
	dndTextTransferFinish,			/* transferFinish */
};

/*
 * Text transfer protocol initialization
 */
DtDndMethods *
_DtDndTextTransferProtocolInitialize(
	Display *	display)
{
        _DtSvcProcessLock();
	if (XA_COMPOUND_TEXT == 0) {
		XA_COMPOUND_TEXT = DtGetAtom(display, "COMPOUND_TEXT");
		dndPreferString  = (strcmp(setlocale(LC_CTYPE,NULL),"C") == 0);
#ifdef DEBUG
	printf("locale = %s, dndPreferString = %d\n",
			setlocale(LC_CTYPE,NULL), dndPreferString);
#endif
	}
	_DtSvcProcessUnlock();

	return &dndTextTransferProtocol;
}

/*
 * Returns generic export/import targets for text transfers
 */
static void
dndTextGetTargets(
	Atom **		targets,
	Cardinal *	numTargets)
{
	int		ii = 0;

	*numTargets = 3;

	*targets = (Atom *)XtMalloc(*numTargets * sizeof(Atom));

	(*targets)[ii++] = XA_COMPOUND_TEXT;
	(*targets)[ii++] = XA_TEXT;
	(*targets)[ii++] = XA_STRING;
}

/*
 * Returns available targets for text transfers
 */
static void
dndTextGetAvailTargets(
	DtDragInfo *	dtDragInfo,
	Atom **		availTargets,
	Cardinal *	numAvailTargets)
{
	dndTextGetTargets(availTargets, numAvailTargets);
}

/*
 * Returns export targets for text transfers
 */
static void
dndTextGetExportTargets(
	DtDragInfo *	dtDragInfo,
	Atom **		exportTargets,
	Cardinal *	numExportTargets)
{
	dndTextGetTargets(exportTargets, numExportTargets);
}

/*
 * Returns import targets for text transfers
 */
static void
dndTextGetImportTargets(
	DtDropInfo *	dtDropInfo,
	Atom **		importTargets,
	Cardinal *	numImportTargets)
{
	dndTextGetTargets(importTargets, numImportTargets);
}

/*
 * Initialize protocol specific part of drag data
 */
static void
dndTextConvertInit(
	DtDragInfo *	dtDragInfo)
{
	DtDndContext *	dragData = dtDragInfo->dragData;

	dragData->data.strings = (XmString *)
		XtMalloc(dragData->numItems * sizeof(XmString));
}

/*
 * Convert the motif strings into selection data
 */
static Boolean
dndTextConvert(
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
	Display *	dpy	 = XtDisplayOfObject(dragContext);
	XmString *	stringList;
	Cardinal	numStrings;
	XTextProperty	textProp;
	XmICCEncodingStyle encStyle;
	int		status;

	/*
 	 * Select the text encoding style; reject unknown targets
	 */

	if (*target == XA_COMPOUND_TEXT) {
		encStyle = XmSTYLE_COMPOUND_TEXT;
	} else if (*target == XA_STRING) {
		encStyle = XmSTYLE_STRING;
 	} else if (*target == XA_TEXT) {
		encStyle = XmSTYLE_TEXT;
	} else {
		return False;
	}

	/*
	 * Convert the XmString list into a string list
	 */

	numStrings	= dragData->numItems;
	stringList	= dragData->data.strings;

	status = XmCvtXmStringTableToTextProperty(dpy, stringList, numStrings,
						  encStyle, &textProp);
	if (status != Success)
	    return False;

	/*
	 * Return the text property
	 */

	*returnType 	= textProp.encoding;
	*returnValue	= (XtPointer)textProp.value;
	*returnLength	= textProp.nitems;
	*returnFormat	= textProp.format;

	return True;
}

/*
 * Clean up from the convert init proc
 */
static void
dndTextConvertFinish(
	DtDragInfo *	dtDragInfo)
{
	DtDndContext *	dragData = dtDragInfo->dragData;

	if (dragData->data.strings) {
		XtFree((char *)dragData->data.strings);
		dragData->data.strings = NULL;
	}
}

/*
 * Returns the transfer targets selected from the export targets
 */
static void
dndTextTransferTargets(
	DtDropInfo *	dtDropInfo,
	Atom *		exportTargets,
	Cardinal	numExportTargets,
	Atom **		transferTargets,
	Cardinal *	numTransferTargets)
{
	Boolean		foundCT, foundText, foundString;
	Atom		target;
	int		ii;

	foundCT = foundText = foundString = False;

	for (ii = 0; ii < numExportTargets; ii++) {
		if (exportTargets[ii] == XA_COMPOUND_TEXT) {
			foundCT = True;
		} else if (exportTargets[ii] == XA_TEXT) {
			foundText = True;
		} else if (exportTargets[ii] == XA_STRING) {
			foundString = True;
		}
	}

	if (dndPreferString && foundString) {
		target = XA_STRING;
	} else if (foundCT) {
		target = XA_COMPOUND_TEXT;
	} else if (foundText) {
		target = XA_TEXT;
	} else if (foundString) {
		target = XA_STRING;
	} else {
		*numTransferTargets	= 0;
		*transferTargets	= NULL;
		return;
	}
 
	*numTransferTargets = 1;

	*transferTargets = (Atom *)XtMalloc(*numTransferTargets * sizeof(Atom));

	(*transferTargets)[0] = target;
}

/*
 * Transfer the selection data into motif strings
 */
static void
dndTextTransfer(
        Widget          dropTransfer,
        DtDropInfo *	dtDropInfo,
        Atom *		selection,
	Atom *		target,
        Atom *		type,
        XtPointer       value,
        unsigned long *	length,
        int *		format)
{
	Display *	display = XtDisplayOfObject(dropTransfer);
	DtDndContext *	dropData = dtDropInfo->dropData;
	XmString *	stringList;
	XTextProperty	textProp;
	char **		text;
	int		ii, status, textCount;

	/*
	 * Ignore transfers we don't understand or if we've already transferred
	 */

	if (value == NULL || dropData->data.strings || 
	   (*target != XA_COMPOUND_TEXT && 
	    *target != XA_TEXT && 
	    *target != XA_STRING) ) {
		if (value != NULL)
			XtFree(value);
		return;
	}

	/*
	 * Convert the text property to a text list
	 */

	textProp.value 		= (unsigned char *)value;
	textProp.encoding 	= *type;
	textProp.format 	= *format;
	textProp.nitems 	= *length;

	status = XmbTextPropertyToTextList(display, &textProp,
			&text, &textCount);

	XtFree((char *)value);

	if (status != Success) {
		dtDropInfo->status 	= DtDND_FAILURE;
		return;
	}

	/*
	 * Convert the text list into a XmString list
	 */

	stringList = (XmString *)XtMalloc(textCount * sizeof(XmString));

	for (ii = 0; ii < textCount; ii++) {
		stringList[ii] = XmStringCreateLocalized(text[ii]);
	}

	XFreeStringList(text);

	dropData->numItems	= textCount;
	dropData->data.strings	= stringList;
}

/*
 * Clean up from the transfer proc
 */
static void
dndTextTransferFinish(
	DtDropInfo *	dtDropInfo)
{
	DtDndContext *	dropData = dtDropInfo->dropData;
	int		ii;

	for (ii = 0; ii < dropData->numItems; ii++) {
		XmStringFree(dropData->data.strings[ii]);
	}
	XtFree((char *)dropData->data.strings);
}
