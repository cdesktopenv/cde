/* $XConsortium: Confirmed.c /main/5 1996/10/15 14:44:45 rswiston $ */
/*******************************************************************************
        Confirmed.c

       Associated Header file: Confirmed.h
*******************************************************************************/

#include <stdio.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <sys/signal.h>

#include "dtcreate.h"
#include "UxXt.h"
#include "CreateActionAppShell.h"

#define CONTEXT_MACRO_ACCESS 1
#include "Confirmed.h"
#undef CONTEXT_MACRO_ACCESS

/*******************************************************************************
       The following are callback functions for Confirmed dialog.
*******************************************************************************/

/******************************************************************************/
/* activateCB_Confirmed_OkButton                                              */
/******************************************************************************/
void    activateCB_Confirmed_OkButton( Widget  UxWidget,
                                        XtPointer UxClientData,
                                        XtPointer UxCallbackArg)

{
  XtDestroyWidget(XtParent(UxWidget));
}
/******************************************************************************/
/* display_confirmed_message                                                  */
/*                                                                            */
/* INPUT:  Widget parent - parent to position dialog on.                      */
/*         char *message - message to display in dialog box                   */
/* OUTPIT: none                                                               */
/*                                                                            */
/******************************************************************************/
void display_confirmed_message (Widget parent, char *message)
{

  Confirmed = XmCreateInformationDialog(parent, "confirmedDialog", NULL, 0);
  XtUnmanageChild (XmMessageBoxGetChild (Confirmed, XmDIALOG_CANCEL_BUTTON));
  XtUnmanageChild (XmMessageBoxGetChild (Confirmed, XmDIALOG_HELP_BUTTON));

  XtAddCallback( Confirmed, XmNokCallback,
          (XtCallbackProc) activateCB_Confirmed_OkButton,
          (XtPointer) NULL );

  XtVaSetValues (Confirmed,
                 RES_CONVERT(XmNdialogTitle, GETMESSAGE(6, 41, "Create Action - Confirmation")),
                 RES_CONVERT(XmNmessageString, message),
                 NULL);
  XtRealizeWidget(Confirmed);

  XtManageChild (Confirmed);
  return;
}
/*******************************************************************************
       The following are callback functions for Question dialog
*******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* cancelCB_QuestionDialog                                                    */
/*                                                                            */
/******************************************************************************/
void    cancelCB_QuestionDialog(Widget UxWidget,
                                   XtPointer UxClientData,
                                   XtPointer UxCallbackArg)
{
  XtDestroyWidget(XtParent(UxWidget));
}

/******************************************************************************/
/*                                                                            */
/* nosaveCB_QuestionDialog                                                    */
/*                                                                            */
/******************************************************************************/
void    nosaveCB_QuestionDialog(Widget UxWidget,
                                   XtPointer UxClientData,
                                   XtPointer UxCallbackArg)
{
  XtDestroyWidget(XtParent(UxWidget));
  FreeResources();
  exit(0);
}

/******************************************************************************/
/*                                                                            */
/* saveCallbackB_QuestionDialog                                               */
/*                                                                            */
/******************************************************************************/
void    saveCB_QuestionDialog(Widget UxWidget,
                                     XtPointer  UxClientData,
                                     XtPointer UxCallbackArg)

{
  XtDestroyWidget(XtParent(UxWidget));

  /********************************************************************/
  /* Activate the save routine.                                       */
  /********************************************************************/
  activateCB_CA_MB_FileSave((Widget)NULL, (XtPointer)NULL, (XtPointer)NULL);

}

/******************************************************************************/
/* display_question_message                                                   */
/*                                                                            */
/* INPUT:  Widget parent - parent to position dialog on.                      */
/*         char *message - message to display in dialog box                   */
/* OUTPIT: none                                                               */
/*                                                                            */
/******************************************************************************/
void display_question_message (Widget parent, char *message,
                               char *button1, XtCallbackProc cb_button1,
                               char *button2, XtCallbackProc cb_button2,
                               char *button3, XtCallbackProc cb_button3)
{
  Widget tmpw;

  QuestionDialog = XmCreateQuestionDialog(parent, "questionDialog", NULL, 0);

  tmpw = XmMessageBoxGetChild(QuestionDialog, XmDIALOG_OK_BUTTON);
  if (button1) {
     XtVaSetValues(tmpw,
                   RES_CONVERT( XmNlabelString, button1),
                   NULL);
     XtAddCallback( QuestionDialog, XmNokCallback,
             (XtCallbackProc) cb_button1,
             (XtPointer) NULL );
  } else {
     XtUnmanageChild (tmpw);
  }

  tmpw = XmMessageBoxGetChild(QuestionDialog, XmDIALOG_CANCEL_BUTTON);
  if (button2) {
     XtVaSetValues(tmpw,
                   RES_CONVERT( XmNlabelString, button2),
                   NULL);
     XtAddCallback( QuestionDialog, XmNcancelCallback,
             (XtCallbackProc) cb_button2,
             (XtPointer) NULL );
  } else {
     XtUnmanageChild (tmpw);
  }

  tmpw = XmMessageBoxGetChild(QuestionDialog, XmDIALOG_HELP_BUTTON);
  if (button3) {
     XtVaSetValues(tmpw,
                   RES_CONVERT( XmNlabelString, button3),
                   NULL);
     XtAddCallback( QuestionDialog, XmNhelpCallback,
             (XtCallbackProc) cb_button3,
             (XtPointer) NULL );
  } else {
     XtUnmanageChild (tmpw);
  }

  XtVaSetValues (QuestionDialog,
                 RES_CONVERT(XmNdialogTitle, GETMESSAGE(6, 51, "Create Action - Question")),
                 RES_CONVERT(XmNmessageString, message),
                 NULL);

  XtManageChild (QuestionDialog);
  return;
}
/*******************************************************************************
       END OF FILE
*******************************************************************************/

