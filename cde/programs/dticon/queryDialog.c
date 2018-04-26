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
/* $XConsortium: queryDialog.c /main/4 1995/11/02 14:06:32 rswiston $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
/*******************************************************************************
        queryDialog.c

*******************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#include <Xm/MwmUtil.h>
#include <Xm/MessageB.h>
#include "main.h"
#include "externals.h"

#define    RES_CONVERT( res_name, res_value) \
    XtVaTypedArg, (res_name), XmRString, (res_value), strlen(res_value) + 1

/*******************************************************************************
        Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/

Widget  queryDialog;

/*******************************************************************************
        Forward declarations of functions that are defined later in this file.
*******************************************************************************/

Widget  create_queryDialog();

/*******************************************************************************
        The following are callback functions.
*******************************************************************************/

static void
okCallback_queryDialog(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{
        Process_Query_OK();
}

static void
cancelCB_queryDialog(
        Widget w,
        XtPointer clientData,
        XtPointer callbackArg )
{
        Process_Query_Cancel();
}

/*******************************************************************************
        The 'build_' function creates all the widgets
        using the resource values specified in the Property Editor.
*******************************************************************************/

static Widget
build_queryDialog( void )
{
    Widget      queryDialog_shell;
    XmString tmpXmStr, tmpXmStr2, tmpXmStr3, tmpXmStr4, tmpXmStr5;
    Arg  args[10];
    int  n;


    queryDialog_shell = XtVaCreatePopupShell( "queryDialog_shell",
                        xmDialogShellWidgetClass, mainWindow,
                        XmNtitle, GETSTR(6,2, "Icon Editor - Warning"),
                        NULL );

    tmpXmStr = GETXMSTR(6,2, "Icon Editor - Warning");
    tmpXmStr2= GETXMSTR(6,6, "OK");
    tmpXmStr3= GETXMSTR(6,8, "Do ya really wanna?");
    tmpXmStr4= GETXMSTR(6,10, "Cancel");
    tmpXmStr5= GETXMSTR(4,10, "Help");
    queryDialog = XtVaCreateWidget( "queryDialog",
                        xmMessageBoxWidgetClass, queryDialog_shell,
                        XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON,
                        XmNdialogTitle, tmpXmStr,
                        XmNokLabelString, tmpXmStr2,
                        XmNmessageString, tmpXmStr3,
                        XmNcancelLabelString, tmpXmStr4,
                        XmNhelpLabelString, tmpXmStr5,
                        XmNdialogType, XmDIALOG_WARNING,
                        NULL );
    XmStringFree(tmpXmStr);
    XmStringFree(tmpXmStr2);
    XmStringFree(tmpXmStr3);
    XmStringFree(tmpXmStr4);
    XmStringFree(tmpXmStr5);

    XtAddCallback( queryDialog, XmNokCallback, okCallback_queryDialog, NULL);
    XtAddCallback( queryDialog, XmNcancelCallback, cancelCB_queryDialog, NULL);

    n = 0;
    XtSetArg (args[n], XmNuseAsyncGeometry, True);                          n++;
    XtSetArg (args[n], XmNmwmInputMode,MWM_INPUT_PRIMARY_APPLICATION_MODAL);n++;
    XtSetValues (queryDialog_shell, args, n);


    return ( queryDialog );
}

/*******************************************************************************
        The following is the 'Interface function' which is the
        external entry point for creating this interface.
        This function should be called from your application or from
        a callback function.
*******************************************************************************/

Widget
create_queryDialog( void )
{
        Widget                  rtrn;

        rtrn = build_queryDialog();

        return(rtrn);
}

/*******************************************************************************
        END OF FILE
*******************************************************************************/

