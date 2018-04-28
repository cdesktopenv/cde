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
/* $TOG: file.c /main/4 1999/07/20 14:49:49 mgreess $ */
/*****************************************************************************
 *****************************************************************************
 **
 **   File:         file.c
 **
 **   Description:  File transfer functions for the CDE Drag & Drop Demo.
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
 **      Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <X11/Intrinsic.h>

#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/Frame.h>
#include <Xm/Protocols.h>

#include <Dt/Dt.h>
#include <Dt/Dnd.h>

#include "icon.h"
#include "demo.h"
#include "file.h"

 /*************************************************************************
 *
 *       Data Structures & Private Declarations For Appointment Buffers
 *
 **************************************************************************/

/*
 * Specification of drag or drop directory
 */

typedef enum {
	DragDirectory,
	DropDirectory
} DragOrDrop;

/*
 * File names and contents. The contents are the minimal required by the
 * data typing service to correctly type the file via content-based typing.
 * The file names are appropriate to the type of the contents as well. This
 * demo uses content-based typing to get the appropriate icons for these files.
 */

#define FILE_NAME_CSH   "runit"
#define FILE_DATA_CSH   "#! /bin/csh"

#define FILE_NAME_PS    "map.ps"
#define FILE_DATA_PS    "%!PS-Adobe-2.0"

#define FILE_NAME_TEXT  "tasks"
#define FILE_DATA_TEXT  "Nothing"

/*
 * Private file function declarations
 */

static XtActionProc	fileCheckForDragProc(Widget, XEvent*, String*,
						Cardinal*);
static void		fileConvertCallback(Widget, XtPointer, XtPointer);
static void     	fileCreateDirectory(char*);
static void     	fileCreateFile(char*, char*, char*);
static void		fileCreateFiles(Widget);
static void		fileDragStart(Widget, XEvent*, IconInfo*, int);
static char*		fileGetContents(char*);
static char*		fileGetDemoDirectory();
static char*		fileGetDirectoryName(DragOrDrop);
static void     	fileRemoveDirectory(char*);
static void		fileShutdown(Widget, XtPointer, XtPointer);

 /*************************************************************************
 *
 *       File Name Drag & Drop
 *
 **************************************************************************/

/*
 * fileConvertCallback
 *
 * When converting the data, fills in the file structure with the name(s) of
 * the file(s). When converting DELETE, removes the given file(s) and icon(s)
 * from the filesystem and drawing area respectively.
 */
static void
fileConvertCallback(
        Widget          dragContext,
        XtPointer       clientData,
        XtPointer       callData)
{
        DtDndConvertCallbackStruct *convertInfo =
                                        (DtDndConvertCallbackStruct *) callData;
        IconInfo        *iconArray = (IconInfo *) clientData;
	char		filePath[MAXPATHLEN + 1],
			command[MAXPATHLEN + 4];
        int             ii;
	Widget		fileDraw;

	if (convertInfo == NULL) {
		return;
	}

	/*
	 * Verify the protocol and callback reason
	 */

	if (convertInfo->dragData->protocol != DtDND_FILENAME_TRANSFER ||
	    (convertInfo->reason != DtCR_DND_CONVERT_DATA &&
	     convertInfo->reason != DtCR_DND_CONVERT_DELETE)) {
		return;
	}

        switch (convertInfo->reason) {
        case DtCR_DND_CONVERT_DATA:

		/*
	 	 * Supply the file names of the dragged files
		 */

                for (ii = 0; ii < convertInfo->dragData->numItems; ii++) {
			sprintf(filePath, "%s/%s",
				fileGetDirectoryName(DragDirectory),
				iconArray[ii].name);
                        convertInfo->dragData->data.files[ii] =
				XtNewString(filePath);
                }
                break;
        case DtCR_DND_CONVERT_DELETE:

		/*
		 * Delete dragged files as second part of a move operation
		 */

                fileDraw = XtNameToWidget(demoTopLevel, "*fileDraw");

                for (ii = 0; ii < convertInfo->dragData->numItems; ii++) {

			/* Remove file(s) */

                        sprintf(filePath, "%s/%s",
                                fileGetDirectoryName(DragDirectory),
                                iconArray[ii].name);
                        sprintf(command, "rm %s", filePath);
                        if (system(command) != 0) {
                                printf("Unable to remove file \"%s\".\n",
                                         filePath);
                        }

                	/* Remove icon(s) */

                	if (fileDraw != NULL) {
                        	IconDelete(fileDraw, &iconArray[ii]);

                        	XClearWindow(XtDisplayOfObject(fileDraw),
                                	(XtWindow(fileDraw)));
                        	XtCallCallbacks(fileDraw, XmNexposeCallback,
					NULL);
                	}
		}
                break;
        }
}

/*
 * fileDragFinishCallback
 *
 * Free the file names allocated in fileConvertCallback()
 */
void
fileDragFinishCallback(
        Widget          widget,
        XtPointer       clientData,
        XtPointer       callData)
{
	DtDndDragFinishCallbackStruct *dragFinishInfo =
				(DtDndDragFinishCallbackStruct *)callData;
	DtDndContext	*dragData = dragFinishInfo->dragData;
	int		ii;

	for (ii = 0; ii < dragData->numItems; ii++) {
		XtFree(dragData->data.files[ii]);
	}
}

/*
 * fileTransferCallback
 *
 * Handles the transfer of a file or appointment to the draw area.
 * Adds the appropriate icon to the list of icons on the draw area.
 */
void
fileTransferCallback(
        Widget          widget,
        XtPointer       clientData,
        XtPointer       callData)
{
        DtDndTransferCallbackStruct *transferInfo =
                                (DtDndTransferCallbackStruct *) callData;
        IconInfo       *iconList = NULL, *iconPtr;
        char           *filePath, *name, *contents,
			command[2*MAXPATHLEN + 5];
        int             ii;

	if (transferInfo == NULL) {
		return;
	}

	/*
	 * Verify the protocol and callback reasons
	 */

        if (transferInfo->dropData->protocol != DtDND_FILENAME_TRANSFER ||
	    transferInfo->reason != DtCR_DND_TRANSFER_DATA) {
		return;
	}

	if (widget != NULL) {
		XtVaGetValues(widget, XmNuserData, &iconList, NULL);
        }

	/*
	 * Copy the dropped file(s) to the drop directory
	 */

        for (ii = 0; ii < transferInfo->dropData->numItems; ii++) {

		/* Copy the file(s) */

        	filePath = transferInfo->dropData->data.files[ii];

		contents = fileGetContents(filePath);

        	if ((name = strrchr(filePath,'/')) == NULL) {
        		name = filePath;
        	} else {
        		name++;
        	}
                sprintf(command, "cp %s %s", filePath,
                        fileGetDirectoryName(DropDirectory));
                if (system(command) != 0) {
                        printf("Could not copy file \"%s\" to \"%s\".\n",
                                filePath, fileGetDirectoryName(DropDirectory));
                        transferInfo->status = DtDND_FAILURE;
                        return;
                }

		/* Create icon(s) for new file(s) at the drop site */
	
        	iconPtr = IconNew();
        	IconInitialize(widget, iconPtr,
        		transferInfo->x + ii * 10,
        		transferInfo->y + ii * 10,
        		contents, strlen(contents), name, IconByData);
        	iconPtr->next = iconList;
		if (iconList != NULL) {
			iconList->prev = iconPtr;
		}
		iconList = iconPtr;
        	XtVaSetValues(widget, XmNuserData, iconList, NULL);
		XtFree(contents);
        }
}

/*
 * fileDragSetup
 *
 * Prepares the file draw area to be a drag source.
 */
void
fileDragSetup(
        Widget          fileDraw)
{
        char            translations[] = "<Btn2Down>: fileCheckForDragProc()";
        XtTranslations  newTranslations;
        XtActionsRec    actionTable[] = {
                {"fileCheckForDragProc", (XtActionProc)fileCheckForDragProc},
        };

        XtAppAddActions(demoAppContext, actionTable, 1);
        newTranslations = XtParseTranslationTable(translations);
        XtVaSetValues(fileDraw, XmNtranslations, newTranslations, NULL);

        XtAddEventHandler(fileDraw, Button1MotionMask, False,
                (XtEventHandler)demoDragMotionHandler,
                (XtPointer)DtDND_FILENAME_TRANSFER);

	fileCreateFiles(fileDraw);
}

/*
 * fileDropSetup
 *
 * Such a function is not needed since the demoDropSetup() in demo.c registers
 * the draw area as a drop site for drops of file names.
 */

/*
 * fileDragStart
 *
 * Initiates a file drag. The function fileCheckForDrag() first determines
 * if the pointer is over a file icon before calling this function.
 */
static void
fileDragStart(
        Widget          widget,
        XEvent         *event,
        IconInfo       *iconArray,
        int             numFiles)
{
        static XtCallbackRec convertCBRec[] = { {fileConvertCallback, NULL},
                                                {NULL, NULL} };
        static XtCallbackRec dragFinishCBRec[] =
					      { {demoDragFinishCallback, NULL},
					        {fileDragFinishCallback, NULL},
                                                {NULL, NULL} };
        Widget          dragIcon;
        Arg             arg[1];

        convertCBRec[0].closure    = (XtPointer) iconArray;

	/*
	 * Set up drag icon
	 */

        if (numFiles == 1) {
        	if (iconArray[0].dragIcon == NULL) {
                	iconArray[0].dragIcon = DtDndCreateSourceIcon(widget,
                        	iconArray[0].bitmap, iconArray[0].mask);
        	}
                dragIcon = iconArray[0].dragIcon;
        } else {
                dragIcon = NULL; /* Use default multiple provided by library */
        }

        XtSetArg(arg[0], DtNsourceIcon, (XtArgVal)dragIcon);

	/*
	 * Start the drag
	 */

        if (DtDndDragStart(widget, event, DtDND_FILENAME_TRANSFER, numFiles,
                        XmDROP_COPY | XmDROP_MOVE,
                        convertCBRec, dragFinishCBRec, arg, 1)
            == NULL) {

                printf("DragStart returned NULL.\n");
        }
}

/*
 * fileCheckForDragProc
 *
 * Called when button 2 is pressed in the file drag area. Calls
 * fileCheckForDrag() to determine if the button was pressed over a file
 * icon in which case a drag is started.
 */
static XtActionProc
fileCheckForDragProc(
        Widget          widget,
        XEvent		*event,
        String		*params,
        Cardinal	*numParams)
{
	fileCheckForDrag(widget, event, event->xbutton.x, event->xbutton.y);
}

/*
 * fileCheckForDrag
 *
 * Determine if the pointer is over a file icon (within the drag threshold)
 * when button 2 is pressed or when button 1 was pressed and the drag
 * threshold has been exceeded.
 */ 
void
fileCheckForDrag(
        Widget          widget,
        XEvent         *event,
	int		initialX,
	int		initialY)
{
        IconInfo       *iconList, *iconPtr, *iconArray;

        XtVaGetValues(widget, XmNuserData, &iconList, NULL);

        if (iconList == NULL) {
                printf("Unable to locate icon list.\n");
        }

        for (iconPtr = iconList; iconPtr != NULL; iconPtr = iconPtr->next) {
                if ((initialX > (int)iconPtr->icon.x &&
                     initialX < (int)(iconPtr->icon.x + iconPtr->icon.width)) &&
                    (initialY > (int)iconPtr->icon.y &&
                     initialY < (int)(iconPtr->icon.y + iconPtr->icon.height))){

			/*
			 * This starts a single file drag. To start a multiple
			 * file drag add elements to the icon array here. The
			 * convert and transfer callbacks are already written
			 * to handle multiple file transfers. 
			 */

                        iconArray = (IconInfo *)XtCalloc(1,sizeof(IconInfo));
                        iconArray[0] = *iconPtr;

                        fileDragStart(widget, event, iconArray, 1);
                }
        }
}

 /*************************************************************************
 *
 *      File Creation, Initialization & Destruction
 *
 *************************************************************************/

/*
 * fileCreateDragSource
 *
 * Create draw area with a frame to serve as the drag source for files.
 */
Widget
fileCreateDragSource(
	Widget		parent)
{
	Widget		fileFrame,
			fileDraw;

        fileFrame = XtVaCreateManagedWidget("fileFrame",
                xmFrameWidgetClass, parent,
                NULL);

        fileDraw = XtVaCreateManagedWidget("fileDraw",
                xmDrawingAreaWidgetClass, fileFrame,
                NULL);
        XtAddCallback(fileDraw, XmNexposeCallback, demoDrawExposeCallback,NULL);

	return fileDraw;
}

/*
 * fileCreateDropSite
 *
 * Such a function is not needed since the drop site is the draw area which
 * is created in demoCreateDropSite() in demo.c
 */

/*
 * fileCreateDirectory
 *
 * Create the given directory.
 */
static void
fileCreateDirectory(
        char            *directory)
{
        char            command[MAXPATHLEN + 8];

        sprintf(command, "mkdir %s", directory);
        if (system(command) != 0) {
                printf("Unable to create directory \"%s\"\n", directory);
                exit(1);
        }
}

/*
 * fileCreateFile
 *
 * Given a path (partial or absolute), a file name and data create a file
 * containing the given data using the given path.
 */
static void
fileCreateFile(
        char            *filePath,
        char            *fileName,
	char		*fileData)
{
        FILE            *fp;
        char            filePathAndName[MAXPATHLEN];

        sprintf(filePathAndName, "%s/%s", filePath, fileName);

        if ((fp = fopen(filePathAndName, "w")) == NULL) {
                printf("Cannot create file \"%s\" in current directory.\n"
                        "Exiting...\n", filePathAndName);
                exit(1);
        }
        if (fwrite(fileData, strlen(fileData), 1, fp) != 1) {
                printf("Cannot write file \"%s\" in current directory.\n"
                        "Exiting...\n", filePathAndName);
                exit(1);
        }
        fclose(fp);
}

/*
 * fileCreateFiles
 *
 * Create drag and drop directories and the files to drag.
 */
static void
fileCreateFiles(
	Widget		fileDraw)
{
        IconInfo        *iconList,
                        *iconPtr;
        char            *dragDirectory,
                        *dropDirectory;
        Atom            WM_DELETE_WINDOW;

        WM_DELETE_WINDOW =
	  XmInternAtom(XtDisplay(demoTopLevel), "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback(demoTopLevel, WM_DELETE_WINDOW, fileShutdown,
                (XtPointer)NULL);

        dragDirectory = fileGetDirectoryName(DragDirectory);
        dropDirectory = fileGetDirectoryName(DropDirectory);

        fileRemoveDirectory(dragDirectory);
        fileCreateDirectory(dragDirectory);

        fileRemoveDirectory(dropDirectory);
        fileCreateDirectory(dropDirectory);

        fileCreateFile(dragDirectory, FILE_NAME_TEXT, FILE_DATA_TEXT);
        fileCreateFile(dragDirectory, FILE_NAME_CSH,  FILE_DATA_CSH);
        fileCreateFile(dragDirectory, FILE_NAME_PS,   FILE_DATA_PS);

        iconPtr = IconNew();
        IconInitialize(fileDraw, iconPtr, 40, 25,
		FILE_DATA_TEXT, strlen(FILE_DATA_TEXT),
		FILE_NAME_TEXT, IconByData);

        iconList = iconPtr;
        iconPtr = IconNew();
        iconPtr->next = iconList;
        iconList->prev = iconPtr;

        IconInitialize(fileDraw, iconPtr, 105, 25,
		FILE_DATA_CSH, strlen(FILE_DATA_CSH),
		FILE_NAME_CSH, IconByData);

        iconList = iconPtr;
        iconPtr = IconNew();
        iconPtr->next = iconList;
        iconList->prev = iconPtr;

        IconInitialize(fileDraw, iconPtr, 75, 95,
		FILE_DATA_PS, strlen(FILE_DATA_PS),
		FILE_NAME_PS, IconByData);

        iconList = iconPtr;

        XtVaSetValues(fileDraw, XmNuserData, iconList, NULL);
}

/*
 * fileRemoveDirectory
 *
 * Remove the given directory and its contents if the directory exists.
 */
static void
fileRemoveDirectory(
        char            *directory)
{
        struct stat     fileStatus;
        char            command[MAXPATHLEN + 8];

        if (stat(directory, &fileStatus) == 0) { /* directory exists */
                sprintf(command, "rm -rf %s", directory);
                if (system(command) != 0) {
                        printf("Unable to remove directory \"%s\"\n"
                                "Please remove this directory by hand "
                                "and try again.\n", directory);
                        exit(1);
                }
        }
}

/*
 * fileShutdown
 *
 * Remove the temporary file
 */
static void
fileShutdown(
	Widget		widget,
	XtPointer	clientData,
	XtPointer	callData)
{
        fileRemoveDirectory(fileGetDirectoryName(DragDirectory));
        fileRemoveDirectory(fileGetDirectoryName(DropDirectory));
}

 /*************************************************************************
 *
 *      File Utility Functions
 *
 *************************************************************************/

/*
 * fileGetContents
 *
 * Open the specified file and read the contents into a buffer which is
 * returned.
 */
static char*
fileGetContents(
	char		*filePath)
{
	char		*contents = NULL;
        struct stat     fileStatus;
	FILE		*fp;

        if (stat(filePath, &fileStatus) == 0) { /* file exists */
		contents = (char *) XtMalloc(fileStatus.st_size + 1);
        	if ((fp = fopen(filePath, "r")) == NULL) {
                	printf("Cannot open file \"%s\" for reading.\n",
				filePath);
			XtFree(contents);
			contents = NULL;
        	} else if (fread(contents, fileStatus.st_size, 1, fp) != 1) {
                	printf("Cannot read file \"%s\".\n", filePath);
			XtFree(contents);
			contents = NULL;
        	}
		if (contents != NULL) {
			contents[fileStatus.st_size] = NULL;
		}
        	fclose(fp);
	}
	return contents;
}
	
/*
 * fileGetDemoDirectory
 *
 * Return the directory where the demo directories reside.
 */
static char*
fileGetDemoDirectory()
{
        static char     *demoDirectory = NULL;
        char            currentDirectory[MAXPATHLEN];
        int             status;

        if (demoDirectory == NULL) {
                demoDirectory = (char *) getenv("DNDDEMODIR");
                if (demoDirectory == NULL) {
                        demoDirectory = (char *) getcwd(NULL, MAXPATHLEN);
                        if (demoDirectory == NULL) {
                            sprintf(
                                "getcwd() could not get current directory.\n"
                                "\tUsing \".\" instead.\n",
                                NULL);
                            demoDirectory = ".";
                        } else {
                            /* strip off the /tmp_mnt */
			    if (strncmp(demoDirectory, "/tmp_mnt/", 9) == 0) {
			        demoDirectory += 8;
			    }
                        }
                }
        }
        return demoDirectory;
}

/*
 * fileGetDirectoryName
 *
 * Gets the name of the directory where the files are dragged from or dropped
 * to depending on which is requested.
 */
static char*
fileGetDirectoryName(
        DragOrDrop      dragOrDrop)
{
        static char     *dragDirectory = NULL;
        static char     *dropDirectory = NULL;

        switch (dragOrDrop) {
        case DragDirectory:
                if (dragDirectory == NULL) {
                        dragDirectory = (char *) XtMalloc(MAXPATHLEN + 1);
                        sprintf(dragDirectory, "%s/FileDragDir",
				fileGetDemoDirectory());
                }
                return dragDirectory;
                break;
        case DropDirectory:
                if (dropDirectory == NULL) {
                        dropDirectory = (char *) XtMalloc(MAXPATHLEN + 1);
                        sprintf(dropDirectory, "%s/FileDropDir",
				fileGetDemoDirectory());
                }
                return dropDirectory;
                break;
        default:
                return NULL;
        }
}

/*
 * fileStoreBuffer
 *
 * Store a buffer into a file in the drop directory.
 * A temporary file may be created if required.
 */
char *
fileStoreBuffer(
	char		*name,
	void		*buf,
	int		 len)
{
        char            path[MAXPATHLEN];
	char		*dir = fileGetDirectoryName(DropDirectory);
	struct stat	statInfo;
        FILE            *fp;

	if (name == NULL)
		name = "unnamed";

        sprintf(path, "%s/%s", dir, name);

	if (stat(path, &statInfo) == 0) {
		char	*tPath;
		
		if ((tPath = tempnam(dir, name)) == NULL)
			return (char *)NULL;
		strcpy(path, tPath);
		free(tPath);
	}

        if ((fp = fopen(path, "w")) == NULL) {
                printf("Cannot create file \"%s\"\n", path);
		return (char *)NULL;
        }

        if (fwrite(buf, len, 1, fp) != 1) {
                printf("Cannot write to file \"%s\".\n", path);
		return (char *)NULL;
        }

        fclose(fp);

	return XtNewString(path);
}
