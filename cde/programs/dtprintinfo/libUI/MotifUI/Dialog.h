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
/* $XConsortium: Dialog.h /main/3 1995/11/06 09:40:07 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef DIALOG_H
#define DIALOG_H

#include "MotifUI.h"

class Button;
class Prompt;

class Dialog : public MotifUI {

   friend void DialogCB(Widget, XtPointer, XtPointer);
   friend void CloseCB(Widget, XtPointer, XtPointer);
   friend void CancelButtonCB(Widget, XtPointer, XtPointer);
   friend void ResetCB(void *);
   friend void ApplyCB(void *);

 private:

   Widget _clientArea;
   Widget _cancel_widget;
   DialogType _dialog_type;
   boolean _rc;
   char *_string;
   Prompt *_prompt;
   Button *_reset_button;
   Button *_apply_button;
   Button *_cancel_button;
   void * _cancel_callback_data;
   void * _help_callback_data;
   void * _reset_callback_data;
   void * _apply_callback_data;
   void * _validation_callback_data;
   DialogCallback _cancel_callback;
   DialogCallback _help_callback;
   DialogCallback _reset_callback;
   DialogCallback _apply_callback;
   ValidationCallback _validation_callback;

   static void DialogCB(Widget, XtPointer, XtPointer);
   static void CloseCB(Widget, XtPointer, XtPointer);
   static void CancelButtonCB(Widget, XtPointer, XtPointer);
   static void ResetCB(void *);
   static void ApplyCB(void *);

   int _status;

   void OK();
   void Cancel();
   void Reset();
   boolean Apply();
   void Help();

 protected:

   virtual boolean SetVisiblity(boolean);
   boolean SetName(char *);
   void Width(int w);
   int Width() { return MotifUI::Width(); }
   void Height(int h);
   int Height() { return MotifUI::Height(); }
   void WidthHeight(int w, int h);
   void WidthHeight(int *w, int *h) { MotifUI::WidthHeight(w, h); }
   const Widget InnerWidget()       { return _clientArea; }

 public:

   // Constructor for a generic MODELESS or MODAL dialog
   Dialog(MotifUI *parent,
	  char *name, 
	  DialogType dialog_type = MODELESS,
	  boolean has_resize_controls = true,
          DialogCallback cancel_callback = NULL,
          void * help_callback_data = NULL,
          DialogCallback apply_callback = NULL,
          void * apply_callback_data = NULL,
          DialogCallback reset_callback = NULL,
          void * reset_callback_data = NULL,
          ValidationCallback CB = NULL,
          void * validation_data = NULL);

   // Constructor for info, error, work-in-progress, question, warning
   Dialog(MotifUI *parent,
	  char *title, 
	  char *message, 
	  DialogType dialog_type = INFORMATION,
	  char *ok_label = "OK", 
	  char *cancel_label = "Cancel", 
	  char *help_label = "Help", 
	  DialogCallback helpCB = NULL, // If NULL, no help button is created
	  void *help_data = NULL,
	  char *icon = NULL);           // If NULL, use default icon

   // Constructor for prompt
   Dialog(MotifUI *parent,
	  char *title, 
	  char *caption,                  // This appears left of a input field
	  boolean editable,
	  PromptType prompt_type,
	  char *default_value,            // If NULL, no reset button is created
	  boolean echo_input = true,
          ValidationCallback CB = NULL,
          void * validation_data = NULL,
	  DialogCallback helpCB = NULL,   // If NULL, no help button is created
	  void * help_data = NULL,
          DialogCallback applyCB = NULL,
          void * apply_data = NULL,
          DialogCallback resetCB = NULL,
          void * reset_data = NULL,
	  char *message = NULL,          // The message above the input field
	  char *icon = NULL);            // If NULL, no icon will be shown

   // Constructor for file and directory selection
   Dialog(MotifUI *parent,
	  char *title, 
	  char *base_directory, 
	  char *search_pattern, 
	  DialogType dialog_type = FILE_SELECTION,
	  DialogCallback help = NULL,  // If NULL, no help button is created
	  void * help_data = NULL);
   virtual ~Dialog();

   virtual void CloseCB();
   void DefaultButton(MotifUI *);
   void CancelButton(MotifUI *);
   const boolean Answer(char **string); // user must delete string
   const boolean Answer()           { return _rc; }

   const UI_Class UIClass()         { return DIALOG; }
   const int UISubClass()           { return _dialog_type; }
   const char *const UIClassName()  { return "Dialog"; }
};

#endif /* DIALOG_H */
