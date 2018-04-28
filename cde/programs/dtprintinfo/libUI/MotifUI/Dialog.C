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
/* $XConsortium: Dialog.C /main/3 1996/01/19 15:09:18 lehors $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "Dialog.h"
#include "Prompt.h"
#include "Button.h"

#include <Xm/MessageB.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/FileSB.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>
#include <Xm/MwmUtil.h>

#include <Xm/BulletinBP.h>

// Constructor for a generic dialog
Dialog::Dialog(MotifUI *parent, char *title, DialogType dialog_type,
	       boolean has_resize_controls, DialogCallback help_callback, 
               void * help_callback_data, DialogCallback apply_callback, 
               void * apply_callback_data, DialogCallback reset_callback, 
               void * reset_callback_data, ValidationCallback CB,
               void * validation_data)
	: MotifUI(parent, title, NULL)
{
   Arg args[10];
   int n;
   XmString xm_string = StringCreate(title);

   _validation_callback = CB;
   _validation_callback_data = validation_data;
   _help_callback = help_callback;
   _help_callback_data = help_callback_data;
   _apply_callback = apply_callback;
   _apply_callback_data = apply_callback_data;
   _reset_callback = reset_callback;
   _reset_callback_data = reset_callback_data;
   _cancel_button = NULL;
   _cancel_widget = NULL;

   n = 0;
   if (has_resize_controls == false)
    {
      long mwmDecorations;
      mwmDecorations = (MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU);
      XtSetArg(args[n], XmNmwmDecorations, mwmDecorations); n++;
    }
   XtSetArg(args[n], XmNdialogTitle, xm_string); n++;
   XtSetArg(args[n], XmNmarginHeight, 5); n++;
   XtSetArg(args[n], XmNmarginWidth, 5); n++;
   XtSetArg(args[n], XmNallowShellResize, true); n++;
   XtSetArg(args[n], XmNautoUnmanage, false); n++;
   switch (_dialog_type = dialog_type)
    {
     default:
     case MODELESS:
     case INFORMATION:
     case ERROR:
     case WORK_IN_PROGRESS:
       XtSetArg(args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;
       _dialog_type = MODELESS;
       break;
     case MODAL:
     case FILE_SELECTION:
     case QUESTION:
     case WARNING:
     case PROMPT_DIALOG:
     case MODAL_WORK_IN_PROGRESS:
     case MODAL_ERROR:
     case MODAL_INFORMATION:
       XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
       _dialog_type = MODAL;
       break;
    };

   if (parent->UIClass() == APPLICATION)
    {
      XtSetArg(args[n], XmNdialogType, XmDIALOG_TEMPLATE); n++;
      _w = XmCreateMessageBox(parent->InnerWidget(), title, args, n);
      XtManageChild(_w);
    }
   else
      _w = XmCreateTemplateDialog(parent->InnerWidget(), title, args, n);

   _clientArea = XtVaCreateManagedWidget("form", xmFormWidgetClass, _w, NULL);

   StringFree(xm_string);
   XmAddWMProtocolCallback(XtParent(_w), 
      XmInternAtom(display, "WM_DELETE_WINDOW", False), 
      &Dialog::CloseCB, (XtPointer) this);
   InstallHelpCB();
}

// Constructor for info, error, work-in-progress
Dialog::Dialog(MotifUI *parent, char *title, char *message, 
	       DialogType dialog_type, char *ok_label, char *cancel_label, 
	       char *help_label, DialogCallback help,
	       void * help_callback_data, char * /*icon*/)
	: MotifUI(parent, title, NULL)
{
   Arg args[15];
   long mwmDecorations;
   mwmDecorations = (MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU);

   XmString xm_string = StringCreate(title);
   XmString xm_message = StringCreate(message);
   XmString ok_string = StringCreate(ok_label);
   XmString cancel_string = StringCreate(cancel_label);
   XmString help_string = StringCreate(help_label);

   XtSetArg(args[0], XmNdialogTitle, xm_string);
   XtSetArg(args[1], XmNmarginHeight, 5);
   XtSetArg(args[2], XmNmarginWidth, 5);
   XtSetArg(args[3], XmNallowShellResize, true);
   XtSetArg(args[4], XmNautoUnmanage, false);
   XtSetArg(args[5], XmNmessageString, xm_message);
   XtSetArg(args[6], XmNmwmDecorations, mwmDecorations);
   XtSetArg(args[7], XmNokLabelString, ok_string);
   XtSetArg(args[8], XmNcancelLabelString, cancel_string);
   XtSetArg(args[9], XmNhelpLabelString, help_string);
   _help_callback = help;
   _help_callback_data = help_callback_data;
   _apply_callback = NULL;
   _apply_callback_data = NULL;
   _reset_callback = NULL;
   _reset_callback_data = NULL;
   switch (_dialog_type = dialog_type)
    {
     case INFORMATION:
       XtSetArg(args[10], XmNdialogStyle, XmDIALOG_MODELESS);
       _w = XmCreateInformationDialog(parent->InnerWidget(), title, args, 11);
       break;

     case QUESTION:
       XtSetArg(args[10], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
       _w = XmCreateQuestionDialog(parent->InnerWidget(), title, args, 11);
       break;
     case WARNING:
       XtSetArg(args[10], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
       _w = XmCreateWarningDialog(parent->InnerWidget(), title, args, 11);
       break;

     case ERROR:
       XtSetArg(args[10], XmNdialogStyle, XmDIALOG_MODELESS);
       _w = XmCreateErrorDialog(parent->InnerWidget(), title, args, 11);
       break;

     case WORK_IN_PROGRESS:
       mwmDecorations = (MWM_DECOR_BORDER | MWM_DECOR_TITLE);
       XtSetArg(args[6], XmNmwmDecorations, mwmDecorations);
       XtSetArg(args[10], XmNdialogStyle, XmDIALOG_MODELESS);
       _w = XmCreateWorkingDialog(parent->InnerWidget(), title, args, 11);
       break;

     case MODAL_WORK_IN_PROGRESS:
       mwmDecorations = (MWM_DECOR_BORDER | MWM_DECOR_TITLE);
       XtSetArg(args[6], XmNmwmDecorations, mwmDecorations);
       XtSetArg(args[10], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
       _w = XmCreateWorkingDialog(parent->InnerWidget(), title, args, 11);
       break;

     case MODAL_ERROR:
       XtSetArg(args[10], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
       _w = XmCreateErrorDialog(parent->InnerWidget(), title, args, 11);
       break;

     case MODAL_INFORMATION:
       XtSetArg(args[10], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
       _w = XmCreateInformationDialog(parent->InnerWidget(), title, args, 11);
       break;

     case MODAL:
       XtSetArg(args[10], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
       _w = XmCreateMessageDialog(parent->InnerWidget(), title, args, 11);
       break;

     default:
       _dialog_type = MODELESS;
       XtSetArg(args[10], XmNdialogStyle, XmDIALOG_MODELESS);
       _w = XmCreateMessageDialog(parent->InnerWidget(), title, args, 11);
       break;
    }

   if (_dialog_type == QUESTION || _dialog_type == WARNING)
      XtAddCallback(_w, XmNcancelCallback, &Dialog::DialogCB, (XtPointer) this);
   else
      XtUnmanageChild(XmMessageBoxGetChild(_w, XmDIALOG_CANCEL_BUTTON));
   if (help)
      XtAddCallback(_w, XmNhelpCallback, &Dialog::DialogCB, (XtPointer) this);
   else
      XtUnmanageChild(XmMessageBoxGetChild(_w, XmDIALOG_HELP_BUTTON));
   if (_dialog_type == WORK_IN_PROGRESS ||
       _dialog_type == MODAL_WORK_IN_PROGRESS)
      XtUnmanageChild(XmMessageBoxGetChild(_w, XmDIALOG_OK_BUTTON));
   else
      XtAddCallback(_w, XmNokCallback, &Dialog::DialogCB, (XtPointer) this);
   XmAddWMProtocolCallback(XtParent(_w), 
      XmInternAtom(display, "WM_DELETE_WINDOW", False), 
      &Dialog::CloseCB, (XtPointer) this);
   InstallHelpCB();
   StringFree(xm_string);
   StringFree(xm_message);
   StringFree(cancel_string);
   StringFree(ok_string);
   StringFree(help_string);
   _clientArea = _w;
}

// Constructor for prompt
Dialog::Dialog(MotifUI *parent, char *title, char *caption, boolean editable,
	       PromptType prompt_type, char *default_value,
	       boolean echo_input, ValidationCallback validation_callback, 
               void * validation_callback_data, DialogCallback help_callback,
	       void * help_callback_data, DialogCallback apply_callback, 
               void * apply_callback_data, DialogCallback reset_callback, 
               void * reset_callback_data, char *message, char *icon)
	: MotifUI(parent, title, NULL)
{
   Arg      args[8];
   int      n;
   XmString xm_string = StringCreate(title);
   XmString xm_message;
   Pixmap   pixmap;

   _dialog_type = PROMPT_DIALOG;
   _validation_callback = validation_callback;
   _validation_callback_data = validation_callback_data;
   _help_callback = help_callback;
   _help_callback_data = help_callback_data;
   _apply_callback = apply_callback;
   _apply_callback_data = apply_callback_data;
   _reset_callback = reset_callback;
   _reset_callback_data = reset_callback_data;
   n = 0;
   XtSetArg(args[n], XmNdialogTitle, xm_string); n++;
   XtSetArg(args[n], XmNmarginHeight, 5); n++;
   XtSetArg(args[n], XmNmarginWidth, 5); n++;
   XtSetArg(args[n], XmNallowShellResize, true); n++;
   XtSetArg(args[n], XmNautoUnmanage, false); n++;
   XtSetArg(args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); n++;
   if (message && *message)
    {
      xm_message = StringCreate(message);
      XtSetArg(args[n], XmNmessageString, xm_message); n++;
      if (icon && *icon)
       {
	 pixmap = 0;
         // XtSetArg(args[n], XmNsymbolPixmap, pixmap); n++;
       }
    }

   _w = XmCreateMessageDialog(parent->InnerWidget(), title, args, n);
   StringFree(xm_string);
   if (xm_message && *message)
       StringFree(xm_message);
   if (help_callback)
    {
      XtAddCallback(_w, XmNhelpCallback, &Dialog::DialogCB, (XtPointer) this);
    }
   else
    {
      XtUnmanageChild(XmMessageBoxGetChild(_w, XmDIALOG_HELP_BUTTON));
    }
   _prompt = new Prompt(this, caption, editable, prompt_type, default_value,
		        validation_callback, validation_callback_data,
			echo_input);
   XmAddWMProtocolCallback(XtParent(_w), 
      XmInternAtom(display, "WM_DELETE_WINDOW", False), 
      &Dialog::CloseCB, (XtPointer) this);
   InstallHelpCB();
}

// Constructor for file selection
Dialog::Dialog(MotifUI *parent, char *title, char *base_directory, 
	       char *search_pattern, DialogType dialog_type,
	       DialogCallback help, void * /*help_callback_data*/)
	: MotifUI(parent, title, NULL)
{
   Arg args[9];
   XmString title_string = StringCreate(title);
   XmString directory_string = StringCreate(base_directory);
   XmString pattern_string = StringCreate(search_pattern);

   XtSetArg(args[0], XmNdialogTitle, title_string);
   XtSetArg(args[1], XmNmarginHeight, 5);
   XtSetArg(args[2], XmNmarginWidth, 5);
   XtSetArg(args[3], XmNallowShellResize, true);
   XtSetArg(args[4], XmNautoUnmanage, false);
   XtSetArg(args[5], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
   switch (_dialog_type = dialog_type)
    {
     default:
     case FILE_SELECTION:
       _dialog_type = FILE_SELECTION;
       XtSetArg(args[6], XmNfileTypeMask, XmFILE_REGULAR);
       break;
     case DIRECTORY_SELECTION:
       XtSetArg(args[6], XmNfileTypeMask, XmFILE_DIRECTORY);
       break;
     case FILE_DIRECTORY_SELECTION:
       XtSetArg(args[6], XmNfileTypeMask, XmFILE_ANY_TYPE);
       break;
    }
   XtSetArg(args[7], XmNdirectory, directory_string);
   XtSetArg(args[8], XmNpattern, pattern_string);

   _w = XmCreateFileSelectionDialog(parent->InnerWidget(), title, args, 9);
   StringFree(title_string);
   StringFree(directory_string);
   StringFree(pattern_string);
   if (help)
    {
      _help_callback_data = _help_callback_data;
      XtAddCallback(_w, XmNhelpCallback, &Dialog::DialogCB, (XtPointer) this);
    }
   else
    {
      XtUnmanageChild(XmMessageBoxGetChild(_w, XmDIALOG_HELP_BUTTON));
    }
   XmAddWMProtocolCallback(XtParent(_w), 
      XmInternAtom(display, "WM_DELETE_WINDOW", False), 
      &Dialog::CloseCB, (XtPointer) this);
   InstallHelpCB();
}

Dialog::~Dialog()
{
   // Empty
}

void Dialog::DefaultButton(MotifUI *button)
{
   XtVaSetValues(_w, XmNdefaultButton, button->BaseWidget(), NULL);
}

void Dialog::CancelButtonCB(Widget, XtPointer client_data, XtPointer)
{
   Dialog *obj = (Dialog *)client_data;
   if (obj->_cancel_button)
    {
       XtCallCallbacks(obj->_cancel_button->BaseWidget(), XmNactivateCallback,
		       obj->_cancel_button);
    }
}

void Dialog::CancelButton(MotifUI *button)
{
   if (!_cancel_widget)
    {
      _cancel_widget = XtVaCreateWidget("cancel_button",
					xmPushButtonWidgetClass, _w, NULL);
      XtAddCallback(_cancel_widget, XmNactivateCallback, CancelButtonCB, this);
      BB_CancelButton(_w) = _cancel_widget;
    }
   _cancel_button = (Button *)button;
}

void Dialog::Width(int w)
{
   XtVaSetValues(XtParent(_w), XmNallowShellResize, true, NULL);
   MotifUI::Width(w);
   XtVaSetValues(XtParent(_w), XmNallowShellResize, false, NULL);
}

void Dialog::Height(int w)
{
   XtVaSetValues(XtParent(_w), XmNallowShellResize, true, NULL);
   MotifUI::Height(w);
   XtVaSetValues(XtParent(_w), XmNallowShellResize, false, NULL);
}

void Dialog::WidthHeight(int w, int h)
{
   XtVaSetValues(XtParent(_w), XmNallowShellResize, true, NULL);
   MotifUI::WidthHeight(w, h);
   XtVaSetValues(XtParent(_w), XmNallowShellResize, false, NULL);
}

boolean Dialog::SetName(char *name)
{
   XmString xm_message = StringCreate(name);
   XtVaSetValues(_w, XmNmessageString, xm_message, NULL);
   StringFree(xm_message);
   return true;
}

boolean Dialog::SetVisiblity(boolean flag)
{
   MotifUI::SetVisiblity(flag);
   if (flag == false)
      return true;

   boolean is_modal;
   switch (_dialog_type)
    {
     case MODELESS:
     case INFORMATION:
     case ERROR:
     case WORK_IN_PROGRESS:
       is_modal = false;
       break;
     default:
       is_modal = true;
    }

   ToFront();
   XtVaSetValues(XtParent(_w), XmNallowShellResize, False, NULL);
   if (is_modal)
    {
      _status = -1;
      while (_status == -1 || XtAppPending(appContext))
         XtAppProcessEvent(appContext, XtIMAll);
      if (_status)
	 _rc = true;
      else
	 _rc = false;
    }
   return true;
}

const boolean Dialog::Answer(char **string)
{
   if (_rc)
    {
      if (_dialog_type == PROMPT_DIALOG ||
          _dialog_type == FILE_SELECTION ||
          _dialog_type == DIRECTORY_SELECTION ||
          _dialog_type == FILE_DIRECTORY_SELECTION)
       {
         *string = STRDUP(_string);
       }
    }
   else
      *string = NULL;
     
   return _rc;
}

void Dialog::OK()
{
   if (Apply() == true)
    {
      Visible(false);
      _rc = true;
      _status = 1;
    }
   if (_dialog_type != PROMPT_DIALOG &&
       _dialog_type != QUESTION &&
       _dialog_type != WARNING)
      delete this;
}

void Dialog::Cancel()
{
   Reset();
   Visible(false);
   _rc = false;
   _status = 0;
   if (_dialog_type != PROMPT_DIALOG &&
       _dialog_type != QUESTION &&
       _dialog_type != WARNING)
      delete this;
}

boolean Dialog::Apply()
{
   boolean rc = true;

   if (_dialog_type == PROMPT_DIALOG)
    {
      _string = _prompt->Value();
      if (_validation_callback)
         rc = (*_validation_callback)(_validation_callback_data, _string);
      else
         rc = true;
      if (rc == true)
       {
         if (_apply_callback)
            (*_apply_callback)(_apply_callback_data);
         _prompt->DefaultValue(_string);
       }
    }
   return rc;
}

void Dialog::Reset()
{
   if (_reset_callback)
      (*_reset_callback)(_reset_callback_data);
   if (_dialog_type == PROMPT_DIALOG)
      _prompt->Reset();
}

void Dialog::Help()
{
   if (_help_callback)
      (*_help_callback)(_help_callback_data);
}

void Dialog::CloseCB()
{
   Cancel();
}

void Dialog::CloseCB(Widget, XtPointer client_data, XtPointer /*call_data*/)
{
   Dialog *dialog = (Dialog *) client_data;
   dialog->CloseCB();
}

void Dialog::DialogCB(Widget, XtPointer client_data, XtPointer call_data)
{
   XmFileSelectionBoxCallbackStruct *cb;
   Dialog *dialog = (Dialog *) client_data;

   cb = (XmFileSelectionBoxCallbackStruct *) call_data;
   switch (cb->reason)
   {
      case XmCR_HELP: dialog->Help(); break;
      case XmCR_OK:
	 if (dialog->_dialog_type == FILE_SELECTION ||
             dialog->_dialog_type == DIRECTORY_SELECTION ||
             dialog->_dialog_type == FILE_DIRECTORY_SELECTION)
	    dialog->_string = dialog->StringExtract(cb->value);
         dialog->OK();
	 break;
      case XmCR_CANCEL: dialog->Cancel(); break;
      default: break;
   }
}

void Dialog::ResetCB(void *data)
{
   Dialog *dialog = (Dialog *) data;

   dialog->Reset();
}

void Dialog::ApplyCB(void *data)
{
   Dialog *dialog = (Dialog *) data;

   dialog->Apply();
}
