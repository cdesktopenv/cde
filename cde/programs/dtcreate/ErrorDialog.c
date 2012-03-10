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

