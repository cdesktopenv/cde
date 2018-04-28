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
/* $XConsortium: ErrorDialog.c /main/5 1996/10/15 14:46:17 rswiston $ */

/*******************************************************************************
        ErrorDialog.c

       Associated Header file: ErrorDialog.h
*******************************************************************************/

#include <stdio.h>

#include <Xm/Xm.h>
#include "UxXt.h"
#include "dtcreate.h"

#define CONTEXT_MACRO_ACCESS 1
#include "ErrorDialog.h"
#undef CONTEXT_MACRO_ACCESS

/******************************************************************************/
/* activateCB_ErrorDialog_OkButton                                            */
/******************************************************************************/
void    activateCB_ErrorDialog_OkButton( Widget  UxWidget,
                                        XtPointer UxClientData,
                                        XtPointer UxCallbackArg)

{
  XtDestroyWidget(XtParent(UxWidget));
}

/******************************************************************************/
/* display_error_message - display desired message in error dialog            */
/* INPUT:  char *message - message to display in dialog box                   */
/* OUTPIT: none                                                               */
/******************************************************************************/
void display_error_message (Widget parent, char *message)
{

  ErrorDialog = XmCreateErrorDialog(parent, "errorDialog", NULL, 0);
  XtUnmanageChild (XmMessageBoxGetChild (ErrorDialog, XmDIALOG_CANCEL_BUTTON));
  XtUnmanageChild (XmMessageBoxGetChild (ErrorDialog, XmDIALOG_HELP_BUTTON));

  XtAddCallback( ErrorDialog, XmNokCallback,
          (XtCallbackProc) activateCB_ErrorDialog_OkButton,
          (XtPointer) NULL );

  XtVaSetValues (ErrorDialog,
                 RES_CONVERT(XmNdialogTitle, GETMESSAGE(6, 31, "Create Action - Error")),
                 RES_CONVERT(XmNmessageString, message),
                 NULL);

  XtManageChild (ErrorDialog);
  return;
}
/*******************************************************************************
       END OF FILE
*******************************************************************************/

