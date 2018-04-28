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
/* $TOG: fileIODialog.c /main/6 1999/06/01 10:55:30 mgreess $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
/*******************************************************************************
        fileIODialog.c

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include <Xm/DialogS.h>
#include <Xm/MwmUtil.h>
#include "main.h"
#include "externals.h"
#include <Dt/DtPStrings.h>

#include <Xm/FileSB.h>

/*******************************************************************************
        Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/

extern void Do_FileIO();

Widget  fileIODialog;

/*******************************************************************************
        Forward declarations of functions that are defined later in this file.
*******************************************************************************/

Widget  create_fileIODialog();

/*******************************************************************************
        The following are callback functions.
*******************************************************************************/

static void
cancelCB_fileIODialog(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{
    XtUnmanageChild(fileIODialog);
}

/*******************************************************************************
        The following is the 'Interface function' which is the
        external entry point for creating this interface.
        This function should be called from your application or from
        a callback function.
*******************************************************************************/
Widget
create_fileIODialog( void )
{
    Widget    fileIODialog_shell, wid;
    XmString tmpXmStr, tmpXmStr1, tmpXmStr2, tmpXmStr3, tmpXmStr4,
             tmpXmStr6, tmpXmStr7, tmpXmStr8;
    Arg  args[10];
    int  n;
    char *userIconPath, *s;
    struct stat sbuf;

    userIconPath = getenv("XMICONSEARCHPATH");
    if (NULL == userIconPath)
      userIconPath = getenv("XMICONBMSEARCHPATH");
    if (NULL == userIconPath)
      userIconPath = getenv("HOME");

    if (NULL != userIconPath)
    {
	char *s;

	userIconPath = XtNewString(userIconPath);
	s = strchr(userIconPath, ':');
	if (NULL != s) *s = '\0';
	s = strrchr(userIconPath, '/');
	if (NULL != s) *s = '\0';
    }

    fileIODialog_shell = XtVaCreatePopupShell( "fileIODialog_shell",
                        xmDialogShellWidgetClass, mainWindow,
                        XmNtitle, GETSTR(2,2, "Icon Editor - File I/O"),
                        NULL );

    tmpXmStr  = GETXMSTR(2,6,  "Icon Editor - Save As");
    tmpXmStr1 = GETXMSTR(2,10, "Filter");
    tmpXmStr2 = GETXMSTR(2,12, "Folders");
    tmpXmStr3 = GETXMSTR(2,14, "Files");
    tmpXmStr4 = GETXMSTR(2,16, "Enter file name:");
    tmpXmStr6 = GETXMSTR(4,8,  "Cancel");
    tmpXmStr7 = GETXMSTR(4,10, "Help");

    fileIODialog = XtVaCreateWidget( "fileIODialog",
                        xmFileSelectionBoxWidgetClass, fileIODialog_shell,
                        XmNdialogTitle, tmpXmStr,
                        XmNfilterLabelString, tmpXmStr1,
                        XmNdirListLabelString, tmpXmStr2,
                        XmNfileListLabelString, tmpXmStr3,
                        XmNselectionLabelString, tmpXmStr4,
                        XmNapplyLabelString, tmpXmStr1,
                        XmNcancelLabelString, tmpXmStr6,
                        XmNhelpLabelString, tmpXmStr7,
                        NULL );

    userIconPath = getenv("XMICONSEARCHPATH");
    if (NULL != userIconPath)
    {
	userIconPath = XtNewString(userIconPath);
	s = strchr(userIconPath, ':');
	if (NULL != s) *s = '\0';
	s = strrchr(userIconPath, '/');
	if (NULL != s && userIconPath != s) *s = '\0';

        if (0 != stat(userIconPath, &sbuf) ||
	    0 == (S_IFDIR & sbuf.st_mode) ||
	    0 == (S_IWUSR & sbuf.st_mode))
	{
	    XtFree(userIconPath);
	    userIconPath = NULL;
	}
    }
    if (NULL == userIconPath)
    {
	userIconPath = getenv("XMICONBMSEARCHPATH");
        if (NULL != userIconPath)
        {
	    userIconPath = XtNewString(userIconPath);
	    s = strchr(userIconPath, ':');
	    if (NULL != s) *s = '\0';
	    s = strrchr(userIconPath, '/');
	    if (NULL != s && userIconPath != s) *s = '\0';

            if (0 != stat(userIconPath, &sbuf) ||
	        0 == (S_IFDIR & sbuf.st_mode) ||
	        0 == (S_IWUSR & sbuf.st_mode))
	    {
	        XtFree(userIconPath);
	        userIconPath = NULL;
	    }
	}
    }
    if (NULL == userIconPath)
    {
	char	buf[MAXPATHLEN+1];
	char	*home;

	home = getenv("HOME");
	snprintf(buf, sizeof(buf), "%s/%s/icons", home, DtPERSONAL_CONFIG_DIRECTORY);

        if (0 == stat(buf, &sbuf) &&
	    S_IFDIR & sbuf.st_mode &&
	    S_IWUSR & sbuf.st_mode)
	{
	    userIconPath = XtNewString(buf);
	}
    }
    if (NULL != userIconPath)
    {
        tmpXmStr8 = XmStringCreateLocalized(userIconPath);
	XtVaSetValues(fileIODialog, XmNdirectory, tmpXmStr8, NULL);
	XmStringFree(tmpXmStr8);
    }

    XmStringFree(tmpXmStr);
    XmStringFree(tmpXmStr1);
    XmStringFree(tmpXmStr2);
    XmStringFree(tmpXmStr3);
    XmStringFree(tmpXmStr4);
    XmStringFree(tmpXmStr6);
    XmStringFree(tmpXmStr7);

    XtAddCallback(fileIODialog, XmNcancelCallback, cancelCB_fileIODialog, NULL);
    XtAddCallback(fileIODialog, XmNokCallback, Do_FileIO, NULL);

    n = 0;
    XtSetArg (args[n], XmNuseAsyncGeometry, True);                          n++;
    XtSetArg (args[n], XmNmwmInputMode,MWM_INPUT_PRIMARY_APPLICATION_MODAL);n++;
    XtSetValues (fileIODialog_shell, args, n);


    /* check resources to get rid of unwanted children :-) */
    if (!xrdb.useFileFilter)
    {
        /* remove file filter label and text, and filter button */
        wid = XmFileSelectionBoxGetChild(fileIODialog, XmDIALOG_FILTER_LABEL);
        XtUnmanageChild (wid);
        wid = XmFileSelectionBoxGetChild(fileIODialog, XmDIALOG_FILTER_TEXT);
        XtUnmanageChild (wid);

        /* if no File Lists, unmanage it... otherwise rename it */
        if (!xrdb.useFileLists)
        {
            XtUnmanageChild (XmFileSelectionBoxGetChild(fileIODialog,
                                                        XmDIALOG_APPLY_BUTTON));
        }
        else
        {
            n = 0;
            tmpXmStr = GETXMSTR(2,18, "Update");
            XtSetArg (args[n], XmNapplyLabelString, tmpXmStr);         n++;
            XtSetValues (fileIODialog, args, n);
            XmStringFree(tmpXmStr);
        }
    }
    if (!xrdb.useFileLists)
    {
        /* remove label and scrolled lists for directories and files */
        wid = XmFileSelectionBoxGetChild(fileIODialog, XmDIALOG_DIR_LIST_LABEL);
        XtUnmanageChild (wid);
        wid = XmFileSelectionBoxGetChild(fileIODialog, XmDIALOG_LIST_LABEL);
        XtUnmanageChild (wid);
        wid = XmFileSelectionBoxGetChild(fileIODialog, XmDIALOG_DIR_LIST);
        XtUnmanageChild (XtParent(wid));
        wid = XmFileSelectionBoxGetChild(fileIODialog, XmDIALOG_LIST);
        XtUnmanageChild (XtParent(wid));
    }

    return ( fileIODialog );
}

/*******************************************************************************
        END OF FILE
*******************************************************************************/

