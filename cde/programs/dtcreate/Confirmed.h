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
