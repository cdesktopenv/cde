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

/*
 *      $XConsortium: ui_msg.c /main/3 1995/11/06 17:56:14 rswiston $
 *
 * @(#)ui_msg.c	1.17 09 Mar 1995      cde_app_builder/src/ab
 *
 *      RESTRICTED CONFIDENTIAL INFORMATION:
 *
 *      The information in this document is subject to special
 *      restrictions in a confidential disclosure agreement between
 *      HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *      document outside HP, IBM, Sun, USL, SCO, or Univel without
 *      Sun's specific written approval.  This document and all copies
 *      and derivative works thereof must be returned or destroyed at
 *      Sun's request.
 *
 *      Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 ***********************************************************************
 * ui_msg.c - User-Interface MessageDialog support functions
 *	
 *
 ***********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <ab_private/ab.h>
#include <ab_private/objxm.h>
#include <ab_private/ui_util.h>


/*************************************************************************
**                                                                      **
**       Private Function Declarations                                  **
**                                                                      **
**************************************************************************/

static Widget	create_message_dlg(
		    Widget	parent,
		    unsigned char dlg_type,
		    STRING	title,
		    STRING	msg,
		    STRING	ok_label,
		    STRING	cancel_label,
		    STRING	help_label,
		    unsigned char default_button_type
		);

static void	destroyCB(
		    Widget	widget,
		    XtPointer	client_data,
		    XtPointer	call_data
		);
static void     modal_dlgCB(
                    Widget      widget,
                    XtPointer   client_data,
                    XtPointer   call_data
                );

static Widget	create_prompt_dlg(
		    Widget	parent,
		    STRING	title,
		    STRING	label,
		    STRING	ok_label,
		    STRING	cancel_label,
		    STRING	help_label,
		    unsigned char default_button_type
		);

static void     prompt_ok_modal_dlgCB(
                    Widget      		  widget,
                    XtPointer   		  client_data,
                    XmSelectionBoxCallbackStruct  *call_data
                );
static void     prompt_cancel_modal_dlgCB(
                    Widget      		  widget,
                    XtPointer   		  client_data,
                    XmSelectionBoxCallbackStruct  *call_data
                );
static void 	prompt_close_modal_dlg(
		    Widget      widget,
		    XtPointer   client_data,
		    XtPointer   call_data
		);

/*************************************************************************
**                                                                      **
**       Data				                                **
**                                                                      **
**************************************************************************/

/*************************************************************************
**                                                                      **
**       Function Definitions                                           **
**                                                                      **
**************************************************************************/
/*
 * Popup a simple InfoMessage Dialog with "OK" & "Help" 
 * buttons (the dialog will be destroyed on pop-down)
 */
void
ui_popup_info_message(
    Widget	parent,
    STRING	title,
    STRING	msg
)
{
    ui_popup_message(parent, XmDIALOG_INFORMATION, title, msg, 
			"OK", NULL, 
			NULL, NULL,
			"Help", NULL,
			XmDIALOG_OK_BUTTON);
}

/*
 * Popup a Message Dialog
 * (the dialog will be destroyed after popdown)
 */
void
ui_popup_message(
    Widget	   parent,
    unsigned char  msg_type,
    STRING	   title,
    STRING         msg,
    STRING	   ok_label,
    XtCallbackProc ok_callback,
    STRING	   cancel_label,
    XtCallbackProc cancel_callback,
    STRING	   help_label,
    XtCallbackProc help_callback,
    unsigned char  default_button_type
)
{
    Widget	msg_dlg;

    msg_dlg = create_message_dlg(parent, msg_type, title, msg, 
				 ok_label, cancel_label, help_label,
				 default_button_type);

    /* Add Callbacks if necessary */
    if (ok_callback != NULL)
	XtAddCallback(msg_dlg, XmNokCallback, ok_callback, NULL);
    if (cancel_callback != NULL)
	XtAddCallback(msg_dlg, XmNcancelCallback, cancel_callback, NULL);
    if (help_callback != NULL)
	XtAddCallback(msg_dlg, XmNhelpCallback, help_callback, NULL);

    XtManageChild(msg_dlg);
    ui_win_front(msg_dlg);
 
}

/*
 * Popup an Application-MODAL MessageDialog. 
 * Return an "answer" corresponding to the Dialog Button
 * that the user pressed:
 *	UI_ANSWER_OK, UI_ANSWER_CANCEL, UI_ANSWER_HELP
 */
UI_MODAL_ANSWER
ui_popup_modal_message(
    Widget         parent,
    unsigned char  msg_type,
    STRING	   title,
    STRING         msg,
    STRING         ok_label,
    STRING         cancel_label,
    STRING         help_label,
    unsigned char  default_button_type,
    Widget	   *modal_dlg_pane_out_ptr
)
{
    XtAppContext           app;
    Widget		   modal_dlg_pane = NULL;
    UI_MODAL_ANSWER        answer;
    XtCallbackRec ok_callback[] = {
            {(XtCallbackProc)modal_dlgCB, (XtPointer) UI_ANSWER_OK},
            {(XtCallbackProc) NULL, (XtPointer) NULL}
    };
    XtCallbackRec cancel_callback[] = { 
            {(XtCallbackProc)modal_dlgCB, (XtPointer) UI_ANSWER_CANCEL}, 
            {(XtCallbackProc) NULL, (XtPointer) NULL} 
    };
    XtCallbackRec help_callback[] = {
            {(XtCallbackProc)modal_dlgCB, (XtPointer) UI_ANSWER_HELP},
            {(XtCallbackProc) NULL, (XtPointer) NULL}
    };


    modal_dlg_pane = create_message_dlg(parent, msg_type, title, msg, 
				ok_label, cancel_label, help_label,
				default_button_type);

    XtVaSetValues(modal_dlg_pane,
		XmNdialogStyle,    XmDIALOG_FULL_APPLICATION_MODAL,
		XmNokCallback,     &ok_callback,
		XmNcancelCallback, &cancel_callback,
		XmNhelpCallback,   &help_callback,
		XmNuserData,	   &answer,
		NULL);

    answer = UI_ANSWER_NONE;

    /* Popup Modal MessageDialog and wait for answer */
    XtManageChild(modal_dlg_pane);
    ui_win_front(modal_dlg_pane);
    app = XtDisplayToApplicationContext(XtDisplay(modal_dlg_pane));
    while (answer == UI_ANSWER_NONE)
        XtAppProcessEvent(app, XtIMAll);


    if (modal_dlg_pane_out_ptr != NULL)
    {
	(*modal_dlg_pane_out_ptr) = modal_dlg_pane;
    }
    return(answer);
}


static void
modal_dlgCB(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    UI_MODAL_ANSWER	op = (UI_MODAL_ANSWER)client_data;
    UI_MODAL_ANSWER	*answerp = NULL;

    XtVaGetValues(widget, XmNuserData, &answerp, NULL);

    /* Will cause Modal dialog to return */
    *answerp = op;

}

static Widget
create_message_dlg(
    Widget	parent,
    unsigned char  msg_type,
    STRING	   title,
    STRING         msg,
    STRING         ok_label,
    STRING         cancel_label,
    STRING         help_label,
    unsigned char  default_button_type
)
{
    Widget	msg_dlg;
    Widget 	shell;
    Widget	button;
    XmString	c_xmlabel, ok_xmlabel, h_xmlabel;
    Arg		arg[12];
    int		n = 0;

    /* We want to parent the Dialog off of a Shell
     * so walk up the tree to find the parent's shell
     * ancestor...
     */  
    shell = ui_get_ancestor_shell(parent);

    msg_dlg = XmCreateMessageDialog(shell,
                        "ab_message_dialog",
                        NULL, 0);

    /* We don't want a "Cancel" button */
    if (cancel_label == NULL)
    {
        button = XmMessageBoxGetChild(msg_dlg, XmDIALOG_CANCEL_BUTTON);
        XtUnmanageChild(button);
    }   
    else
    {   
        c_xmlabel = XmStringCreateLocalized(cancel_label);
        XtSetArg(arg[n], XmNcancelLabelString, c_xmlabel); n++;
    }   
    if (ok_label == NULL)
    {
        button = XmMessageBoxGetChild(msg_dlg, XmDIALOG_OK_BUTTON);
        XtUnmanageChild(button);
    }   
    else
    {   
        ok_xmlabel = XmStringCreateLocalized(ok_label);
        XtSetArg(arg[n], XmNokLabelString, ok_xmlabel); n++;
    }   
    if (help_label == NULL)
    {
        button = XmMessageBoxGetChild(msg_dlg, XmDIALOG_HELP_BUTTON);
        XtUnmanageChild(button);
    }
    else
    {
        h_xmlabel = XmStringCreateLocalized(help_label);
        XtSetArg(arg[n], XmNhelpLabelString, h_xmlabel); n++;
    }
    XtSetArg(arg[n], XmNmessageAlignment, XmALIGNMENT_CENTER);  n++;
    XtSetArg(arg[n], XmNdialogType,       msg_type);            n++;
    XtSetValues(msg_dlg, arg, n);
 
    /* Free XmStrings if they were allocated */
    if (cancel_label != NULL)
        XmStringFree(c_xmlabel);
    if (ok_label != NULL)
        XmStringFree(ok_xmlabel);
    if (help_label != NULL)
        XmStringFree(h_xmlabel);
 
    /* In case there are newlines in message, we must use the built-in
     * Motif converter available in Varargs.
     */
    XtVaSetValues(msg_dlg,
	    XtVaTypedArg,  XmNdialogTitle,
			XtRString, title, strlen(title)+1,
            XtVaTypedArg,  XmNmessageString,
                        XtRString, msg, strlen(msg)+1,
            XmNmessageAlignment,  XmALIGNMENT_CENTER,
	    XmNdefaultButtonType, default_button_type,
            NULL);
 
    XtAddCallback(XtParent(msg_dlg), XtNpopdownCallback, destroyCB, (XtPointer)1);
 
    return(msg_dlg);

}


static void
destroyCB(
    Widget	widget,
    XtPointer	client_data,
    XtPointer	call_data
)
{
    int    yes = (int)(intptr_t) client_data;

   /* REMIND: aim,1/29/94 - temp. fix for prompt dialog crashing */
    if (yes == 1)
    	XtDestroyWidget(widget);
}

/* note: the caller has to destroy the dialog when it's not needed anymore */
static Widget
create_prompt_dlg(
    Widget      parent,
    STRING      title,
    STRING      label,
    STRING      ok_label,
    STRING      cancel_label,
    STRING      help_label,
    unsigned char default_button_type
)
{
    Widget      prompt_dlg;
    Widget      shell;
    Widget      button, default_button;
    XmString    c_xmlabel, ok_xmlabel, h_xmlabel;
    Arg         arg[12];
    int         n = 0;
 
    /* We want to parent the Dialog off of a Shell
     * so walk up the tree to find the parent's shell
     * ancestor...
     */
    shell = ui_get_ancestor_shell(parent);
 
    XtSetArg(arg[n], XmNautoUnmanage, FALSE); n++;
    prompt_dlg = XmCreatePromptDialog(shell,
                        "ab_prompt_dialog",
                        arg, n);
 
    n = 0;
    /* We don't want a "Cancel" button */
    if (cancel_label == NULL)
    {
        button = XmSelectionBoxGetChild(prompt_dlg, XmDIALOG_CANCEL_BUTTON);
        XtUnmanageChild(button);
    }
    else
    {
        c_xmlabel = XmStringCreateLocalized(cancel_label);
        XtSetArg(arg[n], XmNcancelLabelString, c_xmlabel); n++;
    }
    if (ok_label == NULL)
    {
        button = XmSelectionBoxGetChild(prompt_dlg, XmDIALOG_OK_BUTTON);
        XtUnmanageChild(button);
    }
    else
    {
        ok_xmlabel = XmStringCreateLocalized(ok_label);
        XtSetArg(arg[n], XmNokLabelString, ok_xmlabel); n++;
    }
    if (help_label == NULL)
    {    
        button = XmSelectionBoxGetChild(prompt_dlg, XmDIALOG_HELP_BUTTON);
        XtUnmanageChild(button);
    }
    else
    {
        h_xmlabel = XmStringCreateLocalized(help_label);
        XtSetArg(arg[n], XmNhelpLabelString, h_xmlabel); n++;
    }
    XtSetValues(prompt_dlg, arg, n);

    /* Free XmStrings if they were allocated */
    if (cancel_label != NULL)
        XmStringFree(c_xmlabel);
    if (ok_label != NULL)
        XmStringFree(ok_xmlabel);
    if (help_label != NULL)
        XmStringFree(h_xmlabel);
 
    switch (default_button_type)
    {
	case XmDIALOG_CANCEL_BUTTON:
	    default_button = XmSelectionBoxGetChild(prompt_dlg,
				XmDIALOG_CANCEL_BUTTON);
	    break;
	case XmDIALOG_OK_BUTTON:
	    default_button = XmSelectionBoxGetChild(prompt_dlg,
				XmDIALOG_OK_BUTTON);
	    break;
	case XmDIALOG_HELP_BUTTON:
	    default_button = XmSelectionBoxGetChild(prompt_dlg,
				XmDIALOG_HELP_BUTTON);
	    break;
	default:
	    default_button = NULL;
	    break;
    }

    /* In case there are newlines in message, we must use the built-in
     * Motif converter available in Varargs.
     */
    XtVaSetValues(prompt_dlg,
            XtVaTypedArg,  XmNdialogTitle,
                        XtRString, title, strlen(title)+1,
	    XtVaTypedArg, XmNselectionLabelString,
			XtRString, label, strlen(label),
            XmNdefaultButton, default_button,
            NULL);

    return(prompt_dlg);
}

/*
 * Popup a Prompt Dialog
 * (the dialog will be destroyed after popdown)
 *
 * does not participate with ab window protocol
 */
void
ui_popup_prompt(
    Widget      	parent,
    STRING      	title,
    STRING      	label,
    STRING		initial_text,
    STRING      	ok_label,
    XtCallbackProc 	ok_callback,
    XtPointer		ok_clientData,
    STRING      	cancel_label,
    XtCallbackProc 	cancel_callback,
    XtPointer		cancel_clientData,
    STRING      	help_label,
    XtCallbackProc 	help_callback,
    XtPointer		help_clientData,
    unsigned char 	default_button_type
)
{
    Widget      	prompt_dlg, textf;

    prompt_dlg = create_prompt_dlg(parent, title, label,
                                 ok_label, cancel_label, help_label,
                                 default_button_type);

    /* Add Callbacks if necessary */
    if (ok_callback != NULL)
        XtAddCallback(prompt_dlg, XmNokCallback, ok_callback, ok_clientData);
    if (cancel_callback != NULL)
        XtAddCallback(prompt_dlg, XmNcancelCallback, cancel_callback, cancel_clientData);
    if (help_callback != NULL)
        XtAddCallback(prompt_dlg, XmNhelpCallback, help_callback, help_clientData);
    /* destroy the dialog after popdown */
    XtAddCallback(XtParent(prompt_dlg), XtNpopdownCallback, destroyCB, (XtPointer)1);

    XtManageChild(prompt_dlg);
    ui_win_front(prompt_dlg);

    if ( initial_text != NULL )
    {
	textf = XmSelectionBoxGetChild(prompt_dlg, XmDIALOG_TEXT);
	XmTextFieldSetString(textf, initial_text);
	XmTextFieldSetSelection(textf, 0, strlen(initial_text), CurrentTime);
	XmProcessTraversal(textf, XmTRAVERSE_CURRENT);
    }
}

static void
prompt_ok_modal_dlgCB(
    Widget      		  widget,
    XtPointer   		  client_data,
    XmSelectionBoxCallbackStruct  *call_data
)
{
    UI_MODAL_ANSWER	*answerp = NULL;
    Prompt_Info_Rec	*prompt_info = NULL;
    Widget		textf = NULL;

    XtVaGetValues(widget, XmNuserData, &prompt_info, NULL);
    *(prompt_info->seln_str) =(STRING)objxm_xmstr_to_str(call_data->value);
    answerp = (UI_MODAL_ANSWER *)client_data;

    textf = XmSelectionBoxGetChild(widget, XmDIALOG_TEXT); 
    if ( prompt_info->verify_proc(textf, prompt_info) )
    {
        /* We want to popdown the prompt dialog first before setting
         * answerp, because that will cause ui_popup_modal_prompt() to
         * return to the routine which is calling it. 
         */
	ui_win_show(widget, False, XtGrabNone);
	*answerp = UI_ANSWER_OK;
    }
}

static void
prompt_cancel_modal_dlgCB(
    Widget      		  widget,
    XtPointer   		  client_data,
    XmSelectionBoxCallbackStruct  *call_data
)
{
    UI_MODAL_ANSWER     *answerp = NULL;
    Prompt_Info_Rec     *prompt_info = NULL;

    XtVaGetValues(widget, XmNuserData, &prompt_info, NULL);
    *(prompt_info->seln_str) =(STRING)objxm_xmstr_to_str(call_data->value);
    answerp = (UI_MODAL_ANSWER *)client_data;

    /* We want to popdown the prompt dialog first before setting
     * answerp, because that will cause ui_popup_modal_prompt() to
     * return to the routine which is calling it.
     */
    ui_win_show(widget, False, XtGrabNone);
    *answerp = UI_ANSWER_CANCEL;
}

/* This routine is called when "Close is selected from
 * the Motif window manager menu.  It behaves the same
 * way as if the user pressed the "Cancel" button.
 */ 
static void
prompt_close_modal_dlg(
    Widget      widget,
    XtPointer   client_data,
    XtPointer  	call_data
)
{
    UI_MODAL_ANSWER     *answerp = NULL;

    answerp = (UI_MODAL_ANSWER *)client_data;
    *answerp = UI_ANSWER_CANCEL;
}

UI_MODAL_ANSWER
ui_popup_modal_prompt(
    Widget		parent,
    STRING         	title,
    STRING	   	label,
    STRING	   	initial_text,
    STRING         	ok_label,
    STRING         	cancel_label,
    STRING         	help_label,
    DtbObjectHelpData	help_data,
    unsigned char  	default_button_type,
    STRING		*seln_str,
    ModalPromptVerifyProc verify_proc,
    XtPointer		client_data
)
{
    XtAppContext        	app;
    static UI_MODAL_ANSWER	answer;
    Prompt_Info_Rec		prompt_info = {NULL, NULL, NULL};
    Widget			modal_prompt = NULL;
    
    XtCallbackRec ok_callback[] = {
            {(XtCallbackProc)prompt_ok_modal_dlgCB, (XtPointer) NULL},
            {(XtCallbackProc) NULL, (XtPointer) NULL}
    };   
    XtCallbackRec cancel_callback[] = {
            {(XtCallbackProc)prompt_cancel_modal_dlgCB, (XtPointer) NULL},
            {(XtCallbackProc) NULL, (XtPointer) NULL}
    };
 
    modal_prompt = create_prompt_dlg(parent, title, label, ok_label, 
			cancel_label, help_label, 
			default_button_type);

    answer = UI_ANSWER_NONE;
    ok_callback[0].closure = (XtPointer) &answer;
    cancel_callback[0].closure = (XtPointer) &answer;

    /* participate with the ab window protocol */
    ab_register_window(modal_prompt, AB_WIN_MODAL, WindowUp, parent,
	AB_WPOS_STACK_CENTER, prompt_close_modal_dlg, (XtPointer) &answer);
    
    prompt_info.verify_proc = verify_proc;
    prompt_info.client_data = client_data;
    prompt_info.seln_str = seln_str;
 
    XtVaSetValues(modal_prompt,
                XmNdialogStyle,    XmDIALOG_PRIMARY_APPLICATION_MODAL,
                XmNokCallback,     &ok_callback,
                XmNcancelCallback, &cancel_callback,
                XmNuserData,       &prompt_info,
                NULL);
    if (!util_strempty(help_label) && (help_data != NULL))
    {
	XtAddCallback(modal_prompt, 
		XmNhelpCallback, dtb_help_dispatch, 
		help_data);
    }

    /* Popup Modal MessageDialog and wait for answer */
    ab_show_window(modal_prompt);
    ui_win_front(modal_prompt);

    /* Set the initial text for the prompt dialog */
    if ( initial_text != NULL )
    {
	Widget	textf;
	
        textf = XmSelectionBoxGetChild(modal_prompt, XmDIALOG_TEXT);
        XmTextFieldSetString(textf, initial_text);
        XmTextFieldSetSelection(textf, 0, strlen(initial_text), CurrentTime);
        XmProcessTraversal(textf, XmTRAVERSE_CURRENT);
    }

    app = XtDisplayToApplicationContext(XtDisplay(modal_prompt));
    while (answer == UI_ANSWER_NONE)
    {
        XtAppProcessEvent(app, XtIMAll);
    }

    /*
    ** Destroy the dialog, we are done with it. 
    */
    XtDestroyWidget(modal_prompt);
    return(answer);
}
