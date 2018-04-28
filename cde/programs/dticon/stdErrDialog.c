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
/* $XConsortium: stdErrDialog.c /main/4 1995/11/02 14:06:41 rswiston $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
/*******************************************************************************
        stdErrDialog.c

*******************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#include <Xm/MwmUtil.h>
#include "main.h"
#include "externals.h"

#include <Xm/MessageB.h>

/*******************************************************************************
        Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/

Widget  stdErrDialog;

/*******************************************************************************
        Forward declarations of functions that are defined later in this file.
*******************************************************************************/

Widget  create_stdErrDialog();

/*******************************************************************************
        The following are callback functions.
*******************************************************************************/

static void
okCallback_stdErrDialog(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{
        Process_StdErr_OK();
}

/*******************************************************************************
        The 'build_' function creates all the widgets
        using the resource values specified in the Property Editor.
*******************************************************************************/

static Widget
build_stdErrDialog( void )
{
    Widget    stdErrDialog_shell;
    XmString tmpXmStr, tmpXmStr2;
    Arg  args[10];
    int  n;


    stdErrDialog_shell = XtVaCreatePopupShell( "stdErrDialog_shell",
                        xmDialogShellWidgetClass, mainWindow,
                        XmNtitle, GETSTR(8,2, "Icon Editor - Error"),
                        NULL );

    tmpXmStr = GETXMSTR(8,4, "Continue");
    tmpXmStr2= GETXMSTR(8,2, "Icon Editor - Error");
    stdErrDialog = XtVaCreateWidget( "stdErrDialog",
                        xmMessageBoxWidgetClass, stdErrDialog_shell,
                        XmNokLabelString, tmpXmStr,
                        XmNdialogTitle, tmpXmStr2,
                        XmNdialogType, XmDIALOG_ERROR,
                        NULL );
    XmStringFree(tmpXmStr);
    XmStringFree(tmpXmStr2);

    XtAddCallback( stdErrDialog, XmNokCallback, okCallback_stdErrDialog, NULL);

    n = 0;
    XtSetArg (args[n], XmNuseAsyncGeometry, True);                          n++;
    XtSetArg (args[n], XmNmwmInputMode,MWM_INPUT_PRIMARY_APPLICATION_MODAL);n++;
    XtSetValues (stdErrDialog_shell, args, n);

    return ( stdErrDialog );
}


/*******************************************************************************
        The following is the 'Interface function' which is the
        external entry point for creating this interface.
        This function should be called from your application or from
        a callback function.
*******************************************************************************/

Widget
create_stdErrDialog( void )
{
        Widget                  rtrn;

        rtrn = build_stdErrDialog();

        {
          Widget w0, w1, w2;
          Arg arg[10];
          int i;
          w0 = rtrn;
          w1 = XmMessageBoxGetChild(w0, XmDIALOG_CANCEL_BUTTON);
          w2 = XmMessageBoxGetChild(w0, XmDIALOG_HELP_BUTTON);
          XtUnmanageChild(w1);
          XtUnmanageChild(w2);
        }
        return(rtrn);
}

/*******************************************************************************
        END OF FILE
*******************************************************************************/

