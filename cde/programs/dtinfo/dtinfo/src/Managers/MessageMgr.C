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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $TOG: MessageMgr.C /main/13 1998/07/27 14:20:49 mgreess $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

# include "UAS.hh"

#define C_MessageAgent
#define C_VersionAgent
#define C_Agent
#define L_Agents

#define C_MessageMgr
#define C_Manager
#define C_MarkMgr
#define L_Managers

#include "Prelude.h"

#include "Managers/CatMgr.hh"
#include "Registration.hh"
#include "Other/XmStringLocalized.hh"

#include <stdio.h>
#include <iostream>
using namespace std;

LONG_LIVED_CC (MessageMgr,message_mgr);

#define CLASS MessageMgr

STATIC_SENDER_CC(UAS_CancelRetrievalMsg);

//static int progressDisplayed = 0;

class DisplayDocumentError : public UAS_Receiver<UAS_ErrorMsg>
{
public:
  DisplayDocumentError()
    { UAS_Common::request (this); }
private:
  void receive (UAS_ErrorMsg &message, void *client_data);
};

void
DisplayDocumentError::receive (UAS_ErrorMsg &message, void *client_data)
{
  const char *text = message.fErrorMsg;

  if (text == NULL)
    text = (char*)UAS_String(CATGETS(Set_Messages, 35, "File a Bug"));

  message_mgr().error_dialog ((char *) text);
}

static DisplayDocumentError error_displayer;


class ReportSearchMsg : public UAS_Receiver<UAS_SearchMsg>
{
public:
  ReportSearchMsg() { UAS_SearchEngine::request (this); }
private:
  void receive (UAS_SearchMsg &message, void *client_data);
};

void
ReportSearchMsg::receive (UAS_SearchMsg &message, void *client_data)
{
  if (message.fSearchMsg) {
    bool cont = message_mgr().question_dialog(message.fSearchMsg);
    message.fContFlag = cont? 1 : 0;
  }
  else {
    message_mgr().error_dialog(
	(char*)UAS_String(CATGETS(Set_Messages, 36, "File a Bug")));
  }
}

static ReportSearchMsg g_search_msg_reporter;

class DisplayDocumentProgress: public UAS_Receiver<UAS_StatusMsg>,
			       public UAS_Receiver<UAS_DocumentRetrievedMsg> {
    public:
	DisplayDocumentProgress () {
	    UAS_Common::request ((UAS_Receiver<UAS_StatusMsg> *) this);
	    UAS_Common::request ((UAS_Receiver<UAS_DocumentRetrievedMsg> *) this);
	}

    public:
	void receive (UAS_StatusMsg &msg, void *client_data);
	void receive (UAS_DocumentRetrievedMsg &msg, void *client_data);

};

void
DisplayDocumentProgress::receive (UAS_StatusMsg &msg, void *client_data) {
    message_mgr().progress_dialog (msg.fText);
}

void
DisplayDocumentProgress::receive (UAS_DocumentRetrievedMsg &, void *client_data) {
    message_mgr().progress_dialog_done ();
}


static DisplayDocumentProgress progress_displayer;


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

MessageMgr::MessageMgr() :
 f_booklist_message (*(new MessageAgent())),
 f_nodeview_message (*(new MessageAgent())),
 f_booklist_process(0),
 f_nodeview_process(0),
 f_popped_up(False),
 f_pressed_ok(False),
 f_exit_flag(False),
 f_dialog_active(False)

{
    MessageAgent::request ((UAS_Receiver<CancelOperation> *) this);
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

MessageMgr::~MessageMgr()
{
  g_message_mgr = NULL;
}

void
okCB(Widget, XtPointer client_data, XtPointer)
{
  MessageMgr* mgr = (MessageMgr*)client_data;
  mgr->ok_cb();
}

void
cancelCB(Widget, XtPointer client_data, XtPointer)
{
  MessageMgr* mgr = (MessageMgr*)client_data;
  mgr->cancel_cb();
}

void
MessageMgr::ok_cb()
{
  if (f_exit_flag) {
    mark_mgr().finalize ();
    UAS_Common::finalize ();
    ::exit (0);
  }

  f_pressed_ok = True;
  f_popped_up = False;
}

void
MessageMgr::cancel_cb()
{
  f_pressed_ok = False;
  f_popped_up = False;
}

void
MessageMgr::show_it(Widget dialog)
{
  Widget parent = NULL;
  Widget shell = NULL;

  XtManageChild(dialog);

  if (dialog) parent = XtParent(dialog);
  if (parent) shell = XtParent(parent);
  if (NULL == shell) shell = parent;
  XtPopup(shell, XtGrabNone);
  XMapRaised(XtDisplay(shell), XtWindow(shell));

  f_popped_up = True;
  f_pressed_ok = False;
 
  XEvent event;
  while(f_popped_up)
  {
    XtAppNextEvent (window_system().app_context(), &event);
    XtDispatchEvent (&event);
  }
}

void
MessageMgr::destroy_it(Widget dialog)
{
  Widget shell = XtParent(dialog);
  XtPopdown(shell);
  XtUnmanageChild(dialog);
  XtDestroyWidget(dialog);
}

Widget
MessageMgr::create_dialog (unsigned char dialog_type,
                           char *message_text,
                           Widget parent)
{
  Widget dialog;

  // determine the parent of the dialog shell
  if(parent == NULL)
  {
    Window window;
    int revert;
    XGetInputFocus(window_system().display(), &window, &revert);
    parent = XtWindowToWidget(window_system().display(), window);
    if(parent == NULL)
      parent = window_system().toplevel();
  }

  dialog = XmCreateMessageDialog(parent, (char*)"dialog", NULL, 0);

  WXmString wxms = message_text;
  XtVaSetValues(dialog,
                XmNdialogType, dialog_type,
                XmNmessageAlignment, XmALIGNMENT_CENTER,
                XmNmessageString, (XmString)wxms,
                NULL);

  // add callbacks
  XtAddCallback(dialog, XmNokCallback, okCB, this);
  XtAddCallback(dialog, XmNcancelCallback, cancelCB, this);

  return dialog;
}

// /////////////////////////////////////////////////////////////////
// quit dialog
// /////////////////////////////////////////////////////////////////

void
MessageMgr::quit_dialog (char *message_text, Widget parent)
{
  //if(parent == NULL)
    //parent = (Widget)window_system().toplevel();
  //Widget dialog = XmCreateQuestionDialog(parent, "dialog", NULL, 0);
  Widget dialog = create_dialog(XmDIALOG_QUESTION, message_text, parent);

  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtVaSetValues(dialog,
                XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                XmNmessageAlignment, XmALIGNMENT_CENTER,
                NULL);

  // Set labels on buttons
  XmStringLocalized mtfstring;
  mtfstring = CATGETS(Set_AgentLabel, 244, "Yes");
  XtVaSetValues(XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON),
                XmNlabelString, (XmString)mtfstring,
                NULL);
  mtfstring = CATGETS(Set_AgentLabel, 245, "No");
  XtVaSetValues(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON),
                XmNlabelString, (XmString)mtfstring,
                NULL);

  // Set the window title
  Widget shell = XtParent(dialog);
  String string = CATGETS(Set_MessageAgent, 5, "Dtinfo: Confirm Quit");
  XtVaSetValues(shell,
                XmNminWidth, 200,
                XmNtitle, (XmString)string,
                NULL);

  // Set the flag for the OK button
  f_exit_flag = True;

  show_it(dialog);

  f_exit_flag = False;

  destroy_it(dialog);
#if 0
  if(parent == NULL)
    f_booklist_message.displayQuit(message_text, window_system().toplevel());
  else
    f_nodeview_message.displayQuit(message_text, parent);
  //f_message_agent.displayQuit (message_text, parent);
#endif
}


// /////////////////////////////////////////////////////////////////
// error dialog
// /////////////////////////////////////////////////////////////////
void
MessageMgr::error_dialog (char *message_text, Widget parent)
{
  Widget dialog = create_dialog(XmDIALOG_ERROR, message_text, parent);
  //Widget dialog = XmCreateErrorDialog(parent, "dialog", NULL, 0);

  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
  XtVaSetValues(dialog,
                XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                XmNmessageAlignment, XmALIGNMENT_BEGINNING,
                NULL);

  // Set the window title
  Widget shell = XtParent(dialog);
  String string = CATGETS(Set_MessageAgent, 1, "Dtinfo: Error");
  XtVaSetValues(shell, XmNtitle, string, NULL);

  show_it(dialog);

  destroy_it(dialog);

#if 0
  if(parent == NULL)
    f_booklist_message.displayError(message_text, window_system().toplevel());
  else
    f_nodeview_message.displayError(message_text, parent);
  //f_message_agent.displayError (message_text, parent);
#endif
}

// /////////////////////////////////////////////////////////////////
// warning dialog
// /////////////////////////////////////////////////////////////////

void
MessageMgr::warning_dialog (char *message_text, Widget parent)
{
  Widget dialog = create_dialog(XmDIALOG_WARNING, message_text, parent);
  //Widget dialog = XmCreateWarningDialog(parent, "dialog", NULL, 0);

  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
  XtVaSetValues(dialog,
                XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                XmNmessageAlignment, XmALIGNMENT_BEGINNING,
                NULL);

  // Set the window title
  Widget shell = XtParent(dialog);
  String string = CATGETS(Set_MessageAgent, 2, "Dtinfo: Warning");
  XtVaSetValues(shell, XmNtitle, string, NULL);

  show_it(dialog);

  XtPopdown(shell);
  XtUnmanageChild(dialog);
  XtDestroyWidget(dialog);

#if 0
  if(parent == NULL)
    f_booklist_message.displayWarning(message_text, window_system().toplevel());
  else
    f_nodeview_message.displayWarning(message_text, parent);
  //f_message_agent.displayWarning (message_text, parent);
#endif
}

// /////////////////////////////////////////////////////////////////
// information dialog
// /////////////////////////////////////////////////////////////////

void
MessageMgr::info_dialog (char *message_text, Widget parent)
{
  Widget dialog = create_dialog(XmDIALOG_INFORMATION, message_text, parent);
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));

  // Set the window title
  Widget shell = XtParent(dialog);
  String string = CATGETS(Set_MessageAgent, 3, "Dtinfo: Information");
  XtVaSetValues(shell, XmNtitle, string, NULL);

  show_it(dialog);
  destroy_it(dialog);

#if 0
  if(parent == NULL)
    f_booklist_message.displayInformation(message_text, window_system().toplevel());
  else
    f_nodeview_message.displayInformation(message_text, parent);
  //f_message_agent.displayInformation (message_text, parent);
#endif
}

// /////////////////////////////////////////////////////////////////
// progress dialog
// /////////////////////////////////////////////////////////////////

void
MessageMgr::progress_dialog (char *message_text, Widget parent) {
//    MessageAgent::request ((UAS_Receiver<CancelOperation> *) this);
  //progressDisplayed = 1;
  if(parent == NULL)
  {
    f_booklist_message.displayProgress (
                         message_text, window_system().toplevel());
    f_booklist_process = 1;
  }
  else
  {
    f_nodeview_message.displayProgress (message_text, parent);
    f_nodeview_process = 1;
  }
}

// /////////////////////////////////////////////////////////////////
// progress dialog done
// /////////////////////////////////////////////////////////////////

void
MessageMgr::progress_dialog_done () {
//    MessageAgent::unrequest ((UAS_Receiver<CancelOperation> *) this);
    if (f_booklist_process)
	f_booklist_message.undisplayProgress ();
    if (f_nodeview_process)
	f_nodeview_message.undisplayProgress ();
}

// /////////////////////////////////////////////////////////////////
// question dialog
// /////////////////////////////////////////////////////////////////

bool
MessageMgr::question_dialog (char *message_text, Widget parent)
{
  if(parent == NULL)
    parent = (Widget)window_system().toplevel();
  Widget dialog = XmCreateInformationDialog(parent, (char*)"dialog", NULL, 0);
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  WXmString wxms = message_text;
  XtVaSetValues(dialog,
                XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                XmNmessageAlignment, XmALIGNMENT_CENTER,
                XmNmessageString, (XmString)wxms, 
                NULL);

  // Set labels on buttons
  XmStringLocalized mtfstring;
  mtfstring = CATGETS(Set_AgentLabel, 244, "Yes");
  XtVaSetValues(XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON),
                XmNlabelString, (XmString)mtfstring,
                NULL);
  mtfstring = CATGETS(Set_AgentLabel, 245, "No");
  XtVaSetValues(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON),
                XmNlabelString, (XmString)mtfstring,
                NULL);

  XtAddCallback(dialog, XmNokCallback, okCB, this);
  XtAddCallback(dialog, XmNcancelCallback, cancelCB, this);

  // Set the window title
  Widget shell = XtParent(dialog);
  String string = CATGETS(Set_MessageAgent, 3, "Dtinfo: Information");
  XtVaSetValues(shell, XmNtitle, string, NULL);

  show_it(dialog);

  XtPopdown(shell);
  XtUnmanageChild(dialog);
  XtDestroyWidget(dialog);

  return(f_pressed_ok);

#if 0
  if(parent == NULL)
    return(f_booklist_message.displayQuestion(
                                message_text, window_system().toplevel()));
  else
    return(f_nodeview_message.displayQuestion(message_text, parent));
  //return (f_message_agent.displayQuestion (message_text, parent));
#endif
}

int
MessageMgr::get_integer(const char *msg, const char* title,
                        int default_value, Widget parent)
{
  if(parent == NULL)
    return (f_booklist_message.get_integer (msg, title, default_value, window_system().toplevel()));
  else
    return (f_nodeview_message.get_integer (msg, title, default_value, parent));
}

const char *
MessageMgr::get_string (const char* msg, const char* title,
			const char *default_value, Widget parent)
{
  if(parent == NULL)
    return (f_booklist_message.get_string (msg, title, default_value, window_system().toplevel()));
  else
    return (f_nodeview_message.get_string (msg, title, default_value, parent));
}


// /////////////////////////////////////////////////////////////////
// assert_failed
// /////////////////////////////////////////////////////////////////

void
MessageMgr::assert_failed (char *statement, char *file,
                           int line, Widget parent)
{
  char buffer[512];

  snprintf (buffer, sizeof(buffer),
"An internal error has occured.\n\
The current operation cannot be completed.\n\
Please file a bug report with this information:\n\
Assertion failed: %s\n\
File \"%s,\" line %d.",
	   statement, file, line);
	      
  if(parent == NULL)
    f_booklist_message.displayError (buffer, window_system().toplevel());
  else
    f_nodeview_message.displayError (buffer, parent);
  quit_dialog ((char*)"We strongly suggest you quit now, Ok?", parent);
}

// /////////////////////////////////////////////////////////////////////////
// demo_failure
//
// put up error message. quit application when dialog returns
// 
// /////////////////////////////////////////////////////////////////////////

void
MessageMgr::demo_failure(demoException &, Widget parent)
{
  error_dialog(
	(char*)UAS_String(CATGETS(Set_Messages, 38, "File a Bug")),
        parent);
  ::exit(0);
}
void
MessageMgr::receive (CancelOperation &, void *client_data) {
//    MessageAgent::unrequest ((UAS_Receiver<CancelOperation> *) this);
//
//  SWM -- FOR NOW: Don't pass any document (just 0). We will need
//  to change this in the future.
//
    UAS_CancelRetrievalMsg cancelMsg(0);
    send_message(cancelMsg, client_data);
}
void
MessageMgr::set_max_length(int length, Widget parent)
{
  if(parent == NULL)
    f_booklist_message.setMaxLength (length, window_system().toplevel());
  else
    f_nodeview_message.setMaxLength (length, parent);
}
