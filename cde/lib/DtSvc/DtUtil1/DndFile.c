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
/* $TOG: DndFile.c /main/5 1998/04/09 17:48:19 mgreess $ */
/*********************************************************************
 *
 *	File:		DndFile.c
 *
 *	Description:	Implementation of the File Transfer routines
 *			for the DND Convenience API.
 *
 *********************************************************************
 *
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
 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <X11/Intrinsic.h>
#include <Xm/AtomMgr.h>
#include <Xm/DragC.h>
#include <Xm/DropSMgr.h>
#include <Xm/DropTrans.h>
#include <Tt/tt_c.h>
#include "Dnd.h"
#include "DndP.h"
#include "DtSvcLock.h"

/* 
 * File Transfer Protocol Function Prototypes
 */

static void	dndFileGetTargets(Boolean, Boolean, Atom**, Cardinal*);
static void	dndFileGetAvailTargets(DtDragInfo*, Atom**, Cardinal*);
static void	dndFileGetExportTargets(DtDragInfo*, Atom**, Cardinal*);
static void	dndFileGetImportTargets(DtDropInfo*, Atom**, Cardinal*);
static void	dndFileConvertInit(DtDragInfo*);
static Boolean	dndFileConvert(Widget, DtDragInfo*, Atom*, Atom*,
			Atom*, XtPointer*, unsigned long*, int*, 
			XSelectionRequestEvent*);
static void	dndFileConvertFinish(DtDragInfo*);
static void	dndFileTransferTargets(DtDropInfo*,
			Atom*, Cardinal, Atom**, Cardinal*);
static void	dndFileTransfer(Widget, DtDropInfo*, Atom*, Atom*, Atom*,
			XtPointer, unsigned long*, int*);
static void	dndFileTransferFinish(DtDropInfo*);

/* 
 * File Transfer Support Functions
 */

static Boolean	dndFileContentsToSelectionValue(String,
			Atom*, XtPointer*, unsigned long*, int*);
static Boolean	dndFileListToSelectionValue(String*, Cardinal, Boolean,
			Atom*, XtPointer*, unsigned long*, int*);
static Boolean	dndSelectionValueToFileList(Atom, XtPointer, unsigned long, int,
			Boolean, String**, Cardinal*);
static String	dndFileEncode(String, Boolean);
static String	dndFileDecode(String, Boolean);

/* 
 * File Transfer Selection Targets
 */

static Atom	XA_FILE_NAME;
static Atom	XA_DT_NETFILE;

/* 
 * File Transfer Protocol Methods
 */

static DtDndMethods dndFileTransferProtocol = {
	"DtDndFileNameTransfer",		/* name */
	(DtDndProtocol)DtDND_FILENAME_TRANSFER,	/* protocol */
	DtDND_DRAG_SOURCE_DATA,			/* sourceType */
	dndFileGetAvailTargets,			/* getAvailTargets */
	dndFileGetExportTargets,		/* getExportTargets */
	dndFileGetImportTargets,		/* getImportTargets */
	dndFileConvertInit,			/* convertInit */
	dndFileConvert,				/* convert */
	dndFileConvertFinish,			/* convertFinish */
	dndFileTransferTargets,			/* transferTargets */
	dndFileTransfer,			/* transfer */
	dndFileTransferFinish,			/* transferFinish */
};

typedef struct _ConvertState {
	Boolean		owCompat;
} ConvertState;

/*
 * File transfer protocol initialization
 */
DtDndMethods *
_DtDndFileTransferProtocolInitialize(
	Display *	dpy)
{
        _DtSvcProcessLock();
	if (XA_FILE_NAME == 0) {
		XA_FILE_NAME  	     = DtGetAtom(dpy,"FILE_NAME");
		XA_DT_NETFILE 	     = DtGetAtom(dpy,"_DT_NETFILE");
	}

        _DtSvcProcessUnlock();
	return &dndFileTransferProtocol;
}

/*
 * Returns generic export/import targets for filename transfers
 */
static void
dndFileGetTargets(
        Boolean         doHost,
	Boolean		owCompat,
	Atom **		targets,
	Cardinal *	numTargets)
{
	int		ii = 0;

        *numTargets = 2 + (doHost ? 1 : 0) + (owCompat ? 4 : 0);

	*targets = (Atom *)XtMalloc(*numTargets * sizeof(Atom));

	(*targets)[ii++] = XA_DT_NETFILE;
	(*targets)[ii++] = XA_FILE_NAME;

        if (doHost) {
                (*targets)[ii++] = XA_HOST_NAME;
        }

	if (owCompat) {
		(*targets)[ii++] = XA_TEXT;
		(*targets)[ii++] = XA_STRING;
		(*targets)[ii++] = XA_SUN_DATA_LABEL;
		(*targets)[ii++] = XA_SUN_ATM_METHODS;
	}
}

/*
 * Returns available targets for filename transfers
 */
static void
dndFileGetAvailTargets(
	DtDragInfo *	dtDragInfo,
	Atom **		availTargets,
	Cardinal *	numAvailTargets)
{
	dndFileGetTargets(True, True, availTargets, numAvailTargets);
}

/*
 * Returns export targets for filename transfers
 */
static void
dndFileGetExportTargets(
	DtDragInfo *	dtDragInfo,
	Atom **		exportTargets,
	Cardinal *	numExportTargets)
{
	dndFileGetTargets(True, False, exportTargets, numExportTargets);
}

/*
 * Returns import targets for filename transfers
 */
static void
dndFileGetImportTargets(
	DtDropInfo *	dtDropInfo,
	Atom **		importTargets,
	Cardinal *	numImportTargets)
{
	dndFileGetTargets(False, False, importTargets, numImportTargets);
}

/*
 * Initialize protocol specific part of drag data
 */
static void
dndFileConvertInit(
	DtDragInfo *	dtDragInfo)
{
	DtDndContext *	dragData = dtDragInfo->dragData;
	ConvertState *	cvtState;

	dragData->data.files = (String *)
		XtMalloc(dragData->numItems * sizeof(String));

	cvtState = (ConvertState *)XtCalloc(1,sizeof(ConvertState));
	dtDragInfo->clientData = (XtPointer)cvtState;
}

/*
 * Convert the file names into selection data
 */
static Boolean
dndFileConvert(
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
	ConvertState *	cvtState = (ConvertState *)dtDragInfo->clientData;

	*returnType 	= XA_NULL;
	*returnValue	= (XtPointer)NULL;
	*returnLength	= 0;
	*returnFormat	= 8;

	/*
	 * Determine file encoding style.
	 * Handle Sun ATM file name.
	 * Reject unknown targets.
	 */

	if (*target == XA_FILE_NAME || *target == XA_DT_NETFILE) {

		if (!dndFileListToSelectionValue(
				dragData->data.files, dragData->numItems,
				(*target == XA_DT_NETFILE), 
				returnType, returnValue, 
				returnLength, returnFormat)) {
			return False;
		}

	} else if (*target == XA_TEXT || *target == XA_STRING) {

		if (dragData->numItems > 1 || !cvtState->owCompat)
			return False;

		if (!dndFileContentsToSelectionValue(dragData->data.files[0],
				returnType, returnValue,
				returnLength, returnFormat)) {
			return False;
		}
		
	} else if (*target == XA_SUN_SELN_READONLY) {

		cvtState->owCompat = True;

	} else if (*target == XA_SUN_DATA_LABEL) {
		String		name;

		if (dragData->numItems > 1)
			return False;

		if ((name = strrchr(dragData->data.files[0],'/')) != NULL)
			name++;
		else
			name = dragData->data.files[0];

		*returnType	= XA_STRING;
		*returnValue	= (XtPointer)XtNewString(name);
		*returnLength	= strlen(*returnValue);
		*returnFormat	= 8;

		cvtState->owCompat = True;

	} else if (*target == XA_SUN_ATM_METHODS) {

		Atom *atmAtom	= XtNew(Atom);

		atmAtom[0] 	= XA_SUN_ATM_FILE_NAME;
	
		*returnType 	= XA_ATOM;
		*returnValue	= (XtPointer)atmAtom;
		*returnLength	= 1;
		*returnFormat	= 32;

		cvtState->owCompat = True;

	} else {
		return False;
	}

	return True;
}

/*
 * Clean up from the convert init proc
 */
static void
dndFileConvertFinish(
	DtDragInfo *	dtDragInfo)
{
	DtDndContext *	dragData = dtDragInfo->dragData;

	if (dragData->data.files) {
		XtFree((char *)dragData->data.files);
		dragData->data.files = NULL;
	}
	if (dtDragInfo->clientData)
		XtFree((char *)dtDragInfo->clientData);
}

/*
 * Returns the transfer targets selected from the export targets
 * Prefer _DT_NETFILE over FILE_NAME; prefer local host file names
 */
static void
dndFileTransferTargets(
	DtDropInfo *	dtDropInfo,
	Atom *		exportTargets,
	Cardinal 	numExportTargets,
	Atom **		transferTargets,
	Cardinal *	numTransferTargets)
{
	Boolean		foundNetFile, foundFileName, foundHostName;
	Atom		target;
	int		ii;

	foundNetFile = foundFileName = foundHostName = False;

	for (ii = 0; ii < numExportTargets; ii++) {
		if (exportTargets[ii] == XA_DT_NETFILE) {
			foundNetFile = True;
		} else if (exportTargets[ii] == XA_FILE_NAME) {
			foundFileName = True;
		} else if (exportTargets[ii] == XA_HOST_NAME) {
			foundHostName = True;
		}
	}

	if (foundNetFile && foundFileName && foundHostName) {
		target = XA_HOST_NAME;
	} else if (foundNetFile) {
		target = XA_DT_NETFILE;
	} else if (foundFileName) {
		target = XA_FILE_NAME;
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
 * Transfer the selection data into file names
 */
static void
dndFileTransfer(
        Widget          dropTransfer,
        DtDropInfo *	dtDropInfo,
        Atom *		selection,
	Atom *		target,
        Atom *		type,
        XtPointer       value,
        unsigned long *	length,
        int *		format)
{
	DtDndContext *	dropData  = dtDropInfo->dropData;

	/*
	 * Ignore if we've already transferred
	 */

	if (dropData->data.files) {
		XtFree(value);
		return;
	}

	/*
	 * If hosts are the same then request FILE_NAME else request NETFILE
	 */
	if (*target == XA_HOST_NAME) {
		Atom		target;

		if (strcmp(_DtDndGetHostName(),(char *)value) == 0)
			target = XA_FILE_NAME;
		else
			target = XA_DT_NETFILE;

		_DtDndTransferAdd(dropTransfer, dtDropInfo, &target, 1);

	/*
	 * Convert NETFILE or FILE_NAME selection to file list
	 */
	} else if (*target == XA_DT_NETFILE || *target == XA_FILE_NAME) {

		if (!dndSelectionValueToFileList(*type, value, 
			*length, *format, (*target == XA_DT_NETFILE),
			&(dropData->data.files), &(dropData->numItems))) {

			dtDropInfo->status = DtDND_FAILURE;
		}
	}

	if (value != NULL)
		XtFree(value);
}

/*
 * Clean up from the transfer proc
 */
static void
dndFileTransferFinish(
	DtDropInfo *	dtDropInfo)
{
	DtDndContext *	dropData = dtDropInfo->dropData;
	int		ii;

	for (ii = 0; ii < dropData->numItems; ii++) {
		XtFree((char *)dropData->data.files[ii]);
	}
	XtFree((char *)dropData->data.files);
}

/*
 * Convert the file contents into a STRING selection
 */
static Boolean
dndFileContentsToSelectionValue(
	String		fileName,
	Atom *		returnType,
	XtPointer *	returnValue,
	unsigned long *	returnLength,
	int *		returnFormat)
{
	String		fullPath;
	struct stat	stBuf;
	int		fd;
	unsigned long	bufLen, bytesRead;
	char *		buf;

	fullPath = dndFileEncode(fileName, False);

	if (stat(fullPath,&stBuf) == -1)
		return False;

	if ((fd = open(fullPath, O_RDONLY)) == -1)
		return False;

	bufLen = stBuf.st_size;
	buf = (void *)XtMalloc(bufLen + 1);

	bytesRead = read(fd, buf, bufLen);
	close(fd);

	if (bytesRead == -1 || bytesRead != bufLen) {
		XtFree((char *)buf);
		return False;
	}

	buf[bufLen] = '\0';

	*returnType	= XA_STRING;
	*returnValue	= (XtPointer)buf;
	*returnLength	= bufLen;
	*returnFormat	= 8;

	XtFree(fullPath);

	return True;
}

/*
 * Convert the filename list into a STRING selection
 */
static Boolean
dndFileListToSelectionValue(
	String *	fileList,
	Cardinal	numFiles,
	Boolean		doNetFile,
	Atom *		returnType,
	XtPointer *	returnValue,
	unsigned long *	returnLength,
	int *		returnFormat)
{
	XTextProperty	textProp;
	Status		status;
	String *	tmpList;
	Cardinal	ii;

	/*
	 * Encode the file list
	 */

	tmpList = (String *)XtMalloc(numFiles * sizeof(String));

	for (ii = 0; ii < numFiles; ii++) {
		tmpList[ii] = dndFileEncode(fileList[ii], doNetFile);
	}

	/*
	 * Convert the encoded file list into a string property
	 */

	status = XStringListToTextProperty(tmpList, numFiles, &textProp);

	for (ii = 0; ii < numFiles; ii++) {
		XtFree(tmpList[ii]);
	}

	XtFree((char *)tmpList);

	if (status == 0) 
		return False;

	*returnType	= textProp.encoding;
	*returnValue	= (XtPointer)textProp.value;
	*returnLength	= textProp.nitems;
	*returnFormat	= textProp.format;

	return True;
}

/*
 * Convert the STRING selection into a filename list
 */
static Boolean
dndSelectionValueToFileList(
	Atom		type,
	XtPointer	value,
	unsigned long	length,
	int		format,
	Boolean		doNetFile,
	String **	returnFileList,
	Cardinal *	returnNumFiles)
{
	XTextProperty	textProp;
	Status		status;
	String *	tmpList;
	String *	fileList;
	int		ii, numFiles;

	/*
	 * Convert text prop to file list
	 */

	textProp.encoding	= type;
	textProp.value		= (unsigned char *)value;
	textProp.nitems		= length;
	textProp.format		= format;

	status = XTextPropertyToStringList(&textProp, &tmpList, &numFiles);

	if (status == 0)
		return False;

	/*
	 * Decode the file list
	 */

	fileList = (String *)XtMalloc(numFiles * sizeof(String));

	for (ii = 0; ii < numFiles; ii++) {
		fileList[ii] = dndFileDecode(tmpList[ii], doNetFile);
	}

	XFreeStringList(tmpList);

	*returnFileList	= fileList;
	*returnNumFiles	= numFiles;

	return True;
}

/*
 * Encodes a file name
 * Either into ToolTalk NetFile or into a full path if needed
 */
static String
dndFileEncode(
	String		fileName,
	Boolean		doNetFile)
{
	String		netFile;
	String		retFile;

	if (doNetFile) {
		netFile = tt_file_netfile(fileName);
		if (tt_ptr_error(netFile) == TT_OK) {
			retFile = XtNewString(netFile);
		} else {
			retFile = XtNewString(fileName);
		}
		tt_free(netFile);
	} else if (fileName[0] != '/') {
		char	cwd[MAXPATHLEN];
		char	*realPath;

		if (getcwd(cwd,MAXPATHLEN) == NULL) {
			strcpy(cwd,"/");
		}
		realPath = XtMalloc(strlen(cwd) + strlen(fileName) + 2);
		sprintf(realPath,"%s/%s",cwd,fileName);
		retFile = realPath;
	} else {
		retFile = XtNewString(fileName);
	}
	return retFile;
}

/*
 * Decodes a file name; possibly from a ToolTalk NetFile
 */
static String
dndFileDecode(
	String		fileName,
	Boolean		doNetFile)
{
	String		file;
	String		retFile;

	if (doNetFile) {
		file = tt_netfile_file(fileName);
		if (tt_ptr_error(file) == TT_OK) {
			retFile = XtNewString(file);
		} else {
			retFile = XtNewString(fileName);
		}
		tt_free(file);
	} else {
		retFile = XtNewString(fileName);
	}
	return retFile;
}
