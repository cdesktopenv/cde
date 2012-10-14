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
/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/
/*
 * $TOG: MessageAgentMotif.C /main/20 1998/04/17 11:34:04 mgreess $
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

#define C_TOC_Element
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_MessageAgent
#define L_Agents

#define C_MarkMgr
#define L_Managers

#include "Prelude.h"

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Registration.hh"

#include <WWL/WXmDialogShell.h>
#include <WWL/WXmMessageBox.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmTextField.h>
#include <X11/X.h>
#include <sstream>
using namespace std;

#define CLASS MessageAgent
#include "create_macros.hh"

static bool g_active;
# if 0
static int  g_minWidth = -1;
# endif


STATIC_SENDER_CC(CancelOperation);

// /////////////////////////////////////////////////////////////////
// create_ui
// /////////////////////////////////////////////////////////////////

void
MessageAgent::create_ui(Widget parent)
{
  f_popped_up = False;
  f_dialog = (WXmMessageDialog *)(Widget)
             WXmMessageDialog(parent, (char*)"MessageDialog");

  f_text = (WXmTextField *)
           XtCreateWidget ("text", xmTextFieldWidgetClass, *f_dialog, NULL, 0);
  
  WXmDialogShell shell (XtParent(*f_dialog));

  window_system().register_full_modal_shell(&shell);

  f_dialog->SetOkCallback (this,
                           (WWL_FUN) &MessageAgent::ok_activate);
  f_dialog->SetMapCallback (this,
                            (WWL_FUN) &MessageAgent::position_dialog);
  f_dialog->SetCancelCallback (this,
			    (WWL_FUN) &MessageAgent::cancelOperation);
  shell.SetPopdownCallback (this,
			     (WWL_FUN) &MessageAgent::pop_down);

  //  help_agent().add_activate_help (f_dialog->HelpPB, "quitbox_help");
  f_dialog->HelpPB().Unmanage();

  XtRealizeWidget (shell);
}

void
MessageAgent::ok_activate (WCallback *)
{
  if (f_exit_flag) {
    mark_mgr().finalize ();
    UAS_Common::finalize ();
    ::exit (0);
  }

  f_pressed_ok = True;
}

#define BORDER_FUDGE     6
#define TITLEBAR_FUDGE   18

// /////////////////////////////////////////////////////////////////
// position_dialog - position a dialog appropriately
// /////////////////////////////////////////////////////////////////

void
MessageAgent::position_dialog (WCallback *)
{
  Window        root, child;
  int           root_x, root_y;
  int           win_x, win_y;
  unsigned int  mask;

  static int	sizesSet = 0;

#ifdef BOGUS
  if (!sizesSet) {
      WXmDialogShell theShell (XtParent (*f_dialog));
      theShell.MinHeight (theShell.Height());
      sizesSet = 1;
  }
#endif

  XQueryPointer(window_system().onlineDisplay(), RootWindow(window_system().onlineDisplay(), DefaultScreen(window_system().onlineDisplay())),
		&root, &child, &root_x, &root_y, &win_x, &win_y, &mask);

  // Bogosity alert: The fudge factors are to compensate for the window
  //                 manager title bar height and border width. I don't
  //                 know how to query for their actual values. So, since
  //                 these values are currently hardwired, they'll center
  //                 the dialog OkPB close to pointer location, but will
  //                 be off slightly (...depending on the actual value of
  //                 these dimensions).
  // -rtp

  Position x = root_x - (f_dialog->OkPB().X() +
                         f_dialog->OkPB().Width() / 2) - BORDER_FUDGE;
  Position y = root_y - (f_dialog->OkPB().Y() +
                         f_dialog->OkPB().Height() / 2) -
                         TITLEBAR_FUDGE - BORDER_FUDGE;

  // Adjust position so that the dialog is not off the screen.
  if (x < 0) x = 0;
  if (y < 0) y = 0;

  Position lrx = x + f_dialog->Width() + BORDER_FUDGE * 2;
  Position lry = y + f_dialog->Height() + TITLEBAR_FUDGE + BORDER_FUDGE * 2;

  static Dimension screen_width =
    DisplayWidth (window_system().onlineDisplay(),
		  DefaultScreen (window_system().onlineDisplay()));
  static Dimension screen_height =
    DisplayHeight (window_system().onlineDisplay(),
		  DefaultScreen (window_system().onlineDisplay()));

  if (lrx > screen_width) x = x - (lrx - screen_width);
  if (lry > screen_height) y = y - (lry - screen_height);

  f_dialog->X(x);
  f_dialog->Y(y);

  XRaiseWindow (window_system().onlineDisplay(), XtWindow (*f_dialog));
}

// /////////////////////////////////////////////////////////////////
// Configure and display as message dialog
// /////////////////////////////////////////////////////////////////

void
MessageAgent::displayMessage (char *message_text)
{
    char *message_string = strdup(message_text);
  if(f_dialog == NULL)
    return;

  XmStringLocalized mtfstring;

  // Set labels on buttons
  mtfstring = CATGETS(Set_AgentLabel, 161, "OK");
  XtVaSetValues((Widget)f_dialog->OkPB(), XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 162, "Cancel");
  XtVaSetValues((Widget)f_dialog->CancelPB(), XmNlabelString, (XmString)mtfstring, NULL);

  // Unmanage the cancel and help buttons
  f_dialog->CancelPB().Unmanage();
  f_dialog->HelpPB().Unmanage();
  f_dialog->OkPB().Manage();

  // Set the message string
  f_dialog->MessageString (message_string);

  // Display it
  display_it();
  free(message_string);
}

void
MessageAgent::cancelOperation (WCallback *) {
    undisplayProgress ();
    CancelOperation cancelMsg;
    cancelMsg.fDummy = 0;
    MessageAgent::send_message (cancelMsg);
}


// /////////////////////////////////////////////////////////////////
// Configure and display as an error dialog
// /////////////////////////////////////////////////////////////////

void
MessageAgent::displayError (char *message_text, Widget parent)
{
    char *message_string = strdup(message_text);
  if (f_dialog == NULL)
    create_ui(parent);
  if (g_active)
    return;
  g_active = TRUE;

  // Set the window title
  WXmDialogShell *shell = (WXmDialogShell *)XtParent(*f_dialog);

  String string = CATGETS(Set_MessageAgent, 1, "Dtinfo: Error");
  XtVaSetValues((Widget)shell, XmNtitle, string, NULL);

  // Set the dialog type
  f_dialog->DialogType (XmDIALOG_ERROR);


  f_dialog->DialogStyle(XmDIALOG_FULL_APPLICATION_MODAL);
  // Set message alignment
  f_dialog->MessageAlignment (XmALIGNMENT_BEGINNING);
  f_dialog->CancelPB().Unmanage();
  f_dialog->HelpPB().Unmanage();
  f_dialog->OkPB().Manage();

  // Display the dialog with message text
  displayMessage (message_string);
  g_active = FALSE;
  free(message_string);
}

// /////////////////////////////////////////////////////////////////
// Configure and display as a warning dialog
// /////////////////////////////////////////////////////////////////

void
MessageAgent::displayWarning (char *message_text, Widget parent)
{
    char *message_string = strdup(message_text);
  if (f_dialog == NULL)
    create_ui(parent);
  if (g_active)
    return;
  g_active = TRUE;

  // Set the window title
  WXmDialogShell *shell = (WXmDialogShell *)XtParent(*f_dialog);

  String string = CATGETS(Set_MessageAgent, 2, "Dtinfo: Warning");
  XtVaSetValues((Widget)shell, XmNtitle, string, NULL);

  // Set the dialog type
  f_dialog->DialogType (XmDIALOG_WARNING);

  // Set message alignment
  f_dialog->MessageAlignment (XmALIGNMENT_BEGINNING);
  //  f_dialog->MessageAlignment (XmALIGNMENT_CENTER);
  f_dialog->CancelPB().Unmanage();
  f_dialog->HelpPB().Unmanage();
  f_dialog->OkPB().Manage();

  // Display the dialog with message text
  displayMessage (message_string);
  g_active = FALSE;
  free(message_string);
}

// /////////////////////////////////////////////////////////////////
// Configure and display as an information dialog
// /////////////////////////////////////////////////////////////////

void
MessageAgent::displayInformation (char *message_text, Widget parent)
{
    char *message_string = strdup(message_text);

  if (f_dialog == NULL)
    create_ui(parent);
  if (g_active)
    return;
  g_active = TRUE;

  // Set the window title
  WXmDialogShell *shell = (WXmDialogShell *)XtParent(*f_dialog);

  String string = CATGETS(Set_MessageAgent, 3, "Dtinfo: Information");
  XtVaSetValues((Widget)shell, XmNtitle, string, NULL);

  // Set the dialog type
  f_dialog->DialogType (XmDIALOG_INFORMATION);

  // Set message alignment
  f_dialog->MessageAlignment (XmALIGNMENT_CENTER);
  f_dialog->CancelPB().Unmanage();
  f_dialog->HelpPB().Unmanage();
  f_dialog->OkPB().Manage();

  // Display the dialog with message text
  displayMessage (message_string);
  free(message_string);
  g_active = FALSE;
}

// /////////////////////////////////////////////////////////////////
// Configure and display as a progress dialog
// /////////////////////////////////////////////////////////////////

void
MessageAgent::displayProgress (char *message_text, Widget parent) {

    char *message_string = strdup(message_text);

    if (f_dialog == NULL)
	create_ui (parent);
    if (f_popped_up) {
	f_dialog->MessageString (message_string);
    } else {
	//  Set the window title and message alignment.
	WXmDialogShell *shell = (WXmDialogShell *) XtParent (*f_dialog);
#if 0
	if (g_minWidth == -1) {
	      g_minWidth = window_system().get_int_default ("messageDialogMinWidth");
	}
        shell->MinWidth (g_minWidth);
#endif
	f_dialog->NoResize(False);
#if 0
	f_dialog->ResizePolicy(XmRESIZE_NONE);
#endif
	String string = CATGETS(Set_MessageAgent, 4, "Dtinfo: Progress");
	XtVaSetValues((Widget)shell, XmNtitle, string, NULL);

	f_dialog->DialogType (XmDIALOG_INFORMATION);
	f_dialog->MessageAlignment (XmALIGNMENT_BEGINNING);

	// Set labels on buttons
	XmStringLocalized label_str = CATGETS(Set_AgentLabel, 162, "Cancel");
	XtVaSetValues((Widget)f_dialog->CancelPB(),
				XmNlabelString, (XmString)label_str, NULL);

	// Unmanage the ok and help buttons
	f_dialog->OkPB().Unmanage();
	f_dialog->HelpPB().Unmanage();
	f_dialog->CancelPB().Manage();

	// Set the message string
	f_dialog->MessageString(message_string);

	// Display it
	display_it();

    }
    // free storage allocated from strdup
    free(message_string);
}

void
MessageAgent::undisplayProgress () {
    if (f_popped_up) {
	f_dialog->Unmanage ();
	pop_down ();
	//
	// Fix for DTS#14434: Update the display when
	// we pop this puppy down.
	//
	XmUpdateDisplay ((Widget) *f_dialog);
    }
    //
    //  Put things back the way we found them...
    //
    f_dialog->OkPB().Manage ();
    f_dialog->HelpPB().Manage ();
#if 0
    f_dialog->ResizePolicy (XmRESIZE_ANY);
#endif
    WXmDialogShell *shell = (WXmDialogShell *) XtParent (*f_dialog);
    shell->MinWidth (XtUnspecifiedShellInt);
}

// /////////////////////////////////////////////////////////////////
// display_question - ask a question, return user's answer
// /////////////////////////////////////////////////////////////////

bool
MessageAgent::displayQuestion (char *message_text, Widget parent)
{
    char *message_string = strdup(message_text);

  XmStringLocalized mtfstring;

  if (f_dialog == NULL)
    create_ui(parent);
  if (g_active)
    return (FALSE);
  g_active = TRUE;

  // Set the window title
  WXmDialogShell *shell = (WXmDialogShell *)XtParent(*f_dialog);

  String string = CATGETS(Set_MessageAgent, 6, "Dtinfo: Question");
  XtVaSetValues((Widget)shell, XmNtitle, string, NULL);

  // Set the dialog type
  f_dialog->DialogType (XmDIALOG_QUESTION);

  // Set message alignment
  f_dialog->MessageAlignment (XmALIGNMENT_CENTER);

  // Set labels on buttons
  mtfstring = CATGETS(Set_AgentLabel, 244, "Yes");
  XtVaSetValues((Widget)f_dialog->OkPB(), XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 245, "No");
  XtVaSetValues((Widget)f_dialog->CancelPB(), XmNlabelString, (XmString)mtfstring, NULL);

  // Manage the cancel and help buttons
  f_dialog->CancelPB().Manage();
  f_dialog->OkPB().Manage ();
  f_dialog->HelpPB().Unmanage();

  // Set the message and display the dialog
  // Note: this is a special case dialog
  f_dialog->MessageString (message_string);
  display_it();
  g_active = FALSE;

  free(message_string);
  
  return (f_pressed_ok);
}


// /////////////////////////////////////////////////////////////////
// Configure and display as a quit dialog
// NOTE: Special case
// /////////////////////////////////////////////////////////////////

void
MessageAgent::displayQuit (char *message_text, Widget parent)
{
    char *message_string = strdup(message_text);
  XmStringLocalized mtfstring;

  if (f_dialog == NULL)
    create_ui(parent);
  if (g_active)
    return;
  g_active = TRUE;

  // Set the window title
  WXmDialogShell *shell = (WXmDialogShell *)XtParent(*f_dialog);

  String string = CATGETS(Set_MessageAgent, 5, "Dtinfo: Confirm Quit");
  XtVaSetValues((Widget)shell, XmNtitle, string,
                               XmNminWidth, 200,  NULL);

  // Set the dialog type
  f_dialog->DialogType (XmDIALOG_QUESTION);

  // Set message alignment
  f_dialog->MessageAlignment (XmALIGNMENT_CENTER);

  // Set the flag for the OK button
  f_exit_flag = True;

  // Set labels on buttons
  mtfstring = CATGETS(Set_AgentLabel, 244, "Yes");
  XtVaSetValues((Widget)f_dialog->OkPB(), XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 245, "No");
  XtVaSetValues((Widget)f_dialog->CancelPB(), XmNlabelString, (XmString)mtfstring, NULL);

  // Manage the cancel and help buttons
  f_dialog->CancelPB().Manage();
  f_dialog->OkPB().Manage();
  f_dialog->HelpPB().Unmanage();

  // Set the message and display the dialog
  // Note: this is a special case dialog
  f_dialog->MessageString (message_string);
  display_it();
  g_active = FALSE;
  free(message_string);

  // Reset the exit flag IF we ever return. 
  f_exit_flag = False;
}


// /////////////////////////////////////////////////////////////////
// display_it - display dialog, processing events locally
// /////////////////////////////////////////////////////////////////

// NOTE: We might update in the future to return answer for some. 

void
MessageAgent::display_it()
{
  
  f_popped_up = True;
  f_pressed_ok = False;
  f_dialog->Manage();

  // Enter into a local event loop that processes events until the
  // dialog is popped down.
  // XtAppContext app_context = window_system().app_context();
  XEvent event;
  
  while (f_popped_up)
    {
      XtAppNextEvent (window_system().f_application_context, &event);
      XtDispatchEvent (&event);
    }
}

// /////////////////////////////////////////////////////////////////
// pop_down - reset f_popped_up flag on popdown
// /////////////////////////////////////////////////////////////////

void
MessageAgent::pop_down()
{
  f_popped_up = False;
}


// /////////////////////////////////////////////////////////////////
// get_integer - ask a question, get an integer
// /////////////////////////////////////////////////////////////////

int
MessageAgent::get_integer (const char *message, const char* title,
			   int default_value, Widget parent)
{
  if (f_dialog == NULL)
    create_ui(parent);
    if (g_active)
    return (100);
  g_active = TRUE;

  char buffer[256];

  // Set the dialog type and message.
  f_dialog->MessageString (message? message : "File a Bug");
  f_dialog->DialogType (XmDIALOG_QUESTION);

  // Set the dialog title.
  WXmDialogShell shell (XtParent(*f_dialog));
  shell.Title (title? (char*)title : (char *)"File a Bug");

  // Set the dialog buttons.
  f_dialog->CancelPB().Manage();
  f_dialog->HelpPB().Unmanage();

  XmStringLocalized mtfstring;

  mtfstring = CATGETS(Set_AgentLabel, 161, "OK");
  XtVaSetValues((Widget)f_dialog->OkPB(), XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 162, "Cancel");
  XtVaSetValues((Widget)f_dialog->CancelPB(), XmNlabelString, (XmString)mtfstring, NULL);

  sprintf (buffer, "%d", default_value);
  f_text->Value (buffer);
  f_text->Manage();
 
  f_dialog->InitialFocus (*f_text);
  SET_CALLBACK (*f_text,ModifyVerify,verify_integer);
  SET_CALLBACK (*f_text,ValueChanged,value_changed);

  f_dialog->Realize(); // enforce geometry computation - 10/17/94 haya

  display_it();
  g_active = FALSE;

  f_text->RemoveAllModifyVerifyCallback();
  f_text->Unmanage();
  int value = atoi (f_text->Value());
  ON_DEBUG (printf ("User entered %d\n", value));

  f_dialog->OkPB().SetSensitive(True);

  if (f_pressed_ok)
    return (value);
  else
    return (-1);
}


// /////////////////////////////////////////////////////////////////
// value_changed - sensitize the ok button only when there is text
// /////////////////////////////////////////////////////////////////

void
MessageAgent::value_changed (WCallback *wcb)
{

  char *value = f_text->Value();
  if(strcmp(value, "") == 0)
    f_dialog->OkPB().SetSensitive(False);
  else
    f_dialog->OkPB().SetSensitive(True);
  
}

// /////////////////////////////////////////////////////////////////
// verify_integer - only allow integer type-in values
// /////////////////////////////////////////////////////////////////

void
MessageAgent::verify_integer (WCallback *wcb)
{
  CALL_DATA (XmTextVerifyCallbackStruct,tvp);

  ON_DEBUG (printf ("verify_integer: text = %p, ci = %ld, ni = %ld, sp = %ld, ep = %ld\n",
		    tvp->text->ptr,
		    tvp->currInsert, tvp->newInsert,
		    tvp->startPos, tvp->endPos));
  // Check for non-digit and string length. 
  char *p;
  if (tvp->text->ptr != NULL)
    {
      // No more than three characters allowed. 
      if (f_text->GetLastPosition() + tvp->text->length -
	  (tvp->endPos  - tvp->startPos) > 3)
	tvp->doit = False;
      else
	{
	  // Make sure all entries are digits 
	  p = tvp->text->ptr;
	  while (isdigit (*p))
	    p++;
	  if (*p != '\0')
	    tvp->doit = False;
	}
    }
  // NOTE: Should desensitize Ok button here when not
  // enough characters are in the text field (or better
  // yet, in the ValueChanged callback).  19:14 14-Dec-93 DJB
}



// /////////////////////////////////////////////////////////////////
// get_string - ask a question, get a string
// /////////////////////////////////////////////////////////////////

const char *
MessageAgent::get_string (const char *msg, const char* title,
			  const char *default_value, Widget parent)
{
  if (msg == NULL)
    return NULL;

  if (f_dialog == NULL)
    create_ui(parent);
  if (g_active)
    return ("error");
  g_active = TRUE;

  if (title == NULL)
    title = (char*)UAS_String(
		CATGETS(Set_MessageAgent, 7, "Dtinfo: File a Bug"));

  static char buffer[256];

  // Set the dialog type and message.
  f_dialog->MessageString (msg);
  f_dialog->DialogType (XmDIALOG_QUESTION);

  // Set the dialog title.
  WXmDialogShell shell (XtParent(*f_dialog));
  shell.Title ((char*)title);

  // Set the dialog buttons.
  f_dialog->CancelPB().Manage();

  XmStringLocalized mtfstring;

  mtfstring = CATGETS(Set_AgentLabel, 161, "OK");
  XtVaSetValues((Widget)f_dialog->OkPB(), XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 162, "Cancel");
  XtVaSetValues((Widget)f_dialog->CancelPB(), XmNlabelString, (XmString)mtfstring, NULL);

  if (default_value)
    f_text->Value ((char *) default_value);
  f_text->Manage();
 
  f_dialog->InitialFocus (*f_text);
  f_dialog->Realize(); // enforce geometry computation - 7/12/94 kamiya

  display_it();

  g_active = FALSE;
  f_text->Unmanage();
  char *value = f_text->Value();
  strcpy (buffer, value);

  ON_DEBUG (printf ("User entered <%s>\n", value));

  XtFree (value);

  if (f_pressed_ok)
    return (buffer); 
  else
    return (NULL);
}

// /////////////////////////////////////////////////////////////////
// setMaxLength - Set the maximum number of characters to be entered
// /////////////////////////////////////////////////////////////////

void
MessageAgent::setMaxLength (int length, Widget parent)
{
  if (f_dialog == NULL)
    create_ui(parent);
  XtVaSetValues((Widget)*f_text, XmNmaxLength, length, NULL);
}
