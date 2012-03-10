/* $XConsortium: Confirmed.h /main/4 1995/11/01 16:05:43 rswiston $ */

/*******************************************************************************
       Confirmed.h
       This header file is included by Confirmed.c

*******************************************************************************/

#ifndef _CONFIRMED_H_INCLUDED
#define _CONFIRMED_H_INCLUDED


#include <stdio.h>
#include <Xm/MessageB.h>

/*******************************************************************************
       Declarations of global functions.
*******************************************************************************/


void    display_confirmed_message (Widget parent, char *message);
void display_question_message (Widget parent, char *message,
                               char *button1, XtCallbackProc cb_button1,
                               char *button2, XtCallbackProc cb_button2,
                               char *button3, XtCallbackProc cb_button3);
void    cancelCB_QuestionDialog(Widget UxWidget,
                                XtPointer UxClientData,
                                XtPointer UxCallbackArg);
void    saveCB_QuestionDialog(Widget UxWidget,
                                XtPointer UxClientData,
                                XtPointer UxCallbackArg);
void    nosaveCB_QuestionDialog(Widget UxWidget,
                                XtPointer UxClientData,
                                XtPointer UxCallbackArg);


#endif  /* _CONFIRMED_H_INCLUDED */
