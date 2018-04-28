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
/* $XConsortium: getbdffn.c /main/5 1996/10/14 14:44:32 barstow $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */

#include "excutil.h"
#include <Xm/FileSB.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * There is no public header file for this function (only an
 * internal header XmStringI.h).
 */
extern XtPointer _XmStringUngenerate (XmString string,
                        XmStringTag tag,
                        XmTextType tag_type,
                        XmTextType output_type);

void filesbokCB();
void filesbcancelCB();

extern char *maintitle;

void getbdffn(Exc_data * ed)
{
    Widget	filesb;
    Arg		args[20];
    Cardinal	n;
    char	*selectlabel;
    XmString	xms;

    selectlabel = GETMESSAGE(2, 2, "BDF file selection");
    n = 0;
    XtSetArg(args[n], XmNtitle, maintitle); n++;
    xms = XmStringCreateLocalized(selectlabel);
    XtSetArg(args[n], XmNselectionLabelString, xms); n++;
    filesb = XmCreateFileSelectionDialog(ed->toplevel, "filesb", args, n);
    XtAddCallback(filesb, XmNokCallback,
		  (XtCallbackProc) filesbokCB,
		  (XtPointer) ed);
    XtAddCallback(filesb, XmNcancelCallback,
		  (XtCallbackProc) filesbcancelCB,
		  (XtPointer) ed);
    XtUnmanageChild(XmFileSelectionBoxGetChild(filesb, XmDIALOG_HELP_BUTTON));
    XtManageChild(filesb);
}

int fopencheck(char *file, char *dir, char *mode)
/*
 *  when mode = "r", if fopen can open the file with read only mode, return 0
 *  when mode = "w", if fopen can open the file with write mode, return 0
 *                   but if the file exists, return 1
 *  otherwise return -1
 */
{
    struct stat	buf;

    if (strcmp(mode, "r") == 0) {
	if (stat(file, &buf) == 0)
	    if ((buf.st_mode & S_IFMT) == S_IFREG)
		return 0; /* readable regular file */
    } else if (strcmp(mode, "w") == 0) {
	if (stat(file, &buf) == 0) {
	    if (((buf.st_mode & S_IFMT) == S_IFREG) &&
		(access(file, W_OK) == 0))
		return 1; /* writable existing file */
	} else {
	    if (stat(dir, &buf) == 0) {
		if (((buf.st_mode & S_IFMT) == S_IFDIR) &&
		    (access(dir, W_OK) == 0)) {
		    return 0; /* writable new file */
		}
	    }
	}
    } else
	fprintf(stderr, "Unanticipatable error occurred in fopencheck.\n");
    return -1;
}

void filesbcancelCB(Widget widget, Exc_data * ed, XtPointer call_data)
{
    excexit(ed);
}

void freeStrings(char * dir, char * file)
{
    if (dir != NULL)
	XtFree(dir);
    if (file != NULL)
	XtFree(file);
}

void filesbokCB(Widget widget, Exc_data * ed, XtPointer call_data)
{
    XmFileSelectionBoxCallbackStruct	*ptr;
    char				*file = NULL, *dir = NULL, *tmpfile;
    int					r, ans = 0;
    char				*msg1;
    char				*msg2;

    msg1 = GETMESSAGE(2, 4, "The selected file exists. Overwrite?");
    msg2 = GETMESSAGE(2, 6, "Failed to open the selected file.");

    ptr = (XmFileSelectionBoxCallbackStruct *) call_data;

    file = (char *)  _XmStringUngenerate((XmString) ptr->value, NULL,
					  XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
    if (!file) {
	return;
    }

    dir = (char *)  _XmStringUngenerate((XmString) ptr->dir, NULL,
					  XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
    if (!dir) {
	return;
    } else {
	if (*file != '/') {
	    if ((tmpfile = XtMalloc(strlen(dir) + 1 + strlen(file) + 1))
		== NULL) {
		excerror(ed, EXCERRMALLOC, "filesbokCB", "exit");
	    }
	    sprintf(tmpfile, "%s/%s", dir, file);
	    XtFree(file);
	    file = tmpfile;
	}
    }

    r = fopencheck(file, dir, ed->bdfmode);
    if (r == 0) {/* no problem */
	/*fprintf(stderr, "%s will be opened\n", file);*/
    } else if (r == 1) { /* file exist at export function */
	AskUser(widget, ed, msg1, &ans, "warning");
	if (ans != 1) { /* overwrite cancel */
	    freeStrings(dir, file);
	    return;
	}
    } else { /* file will not be opened */
	AskUser(widget, ed, msg2, &ans, "error");
	freeStrings(dir, file);
	return;
    }
    ed->bdffile = (char *) malloc(strlen(file) + 1);
    strcpy(ed->bdffile, file);
    freeStrings(dir, file);
    XtUnmanageChild(widget);
    if (ed->function == EXPORT)
    {
	createbdf(ed);
    } else if (ed->function == IMPORT)
    {
	PopupSelectXLFD(ed->toplevel);
    }
}
