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
/* $XConsortium: Button.h /main/3 1995/11/06 09:39:10 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef BUTTON_H
#define BUTTON_H

#include "MotifUI.h"

class Button : public MotifUI {

 private:
   
   ButtonCallback _callback;
   void * _callback_data;
   static void ActivateCB(Widget, XtPointer, XtPointer);
   ButtonType _button_type;
   char *_iconFile;
   void CreateButton(MotifUI *, char *, char *, ButtonType, ButtonCallback,
	             void *, char *, char *, char *, char *);

 public:

   // If the callback_data is NULL, the this pointer will be the callback data
   Button(MotifUI *parent,
	  char *name,
	  ButtonType button_type = PUSH_BUTTON,
	  ButtonCallback callback = NULL,
	  void * callback_data = NULL,
	  char *mnemonic = NULL,
	  char *acceleratorText = NULL, 
	  char *accelerator = NULL, 
	  char *iconFile = NULL);

   Button(char *category,
	  MotifUI *parent,
	  char *name,
	  ButtonType button_type = PUSH_BUTTON,
	  ButtonCallback callback = NULL,
	  void * callback_data = NULL,
	  char *mnemonic = NULL,
	  char *acceleratorText = NULL, 
	  char *accelerator = NULL,
	  char *iconFile = NULL);

   ~Button();

   void Callback(ButtonCallback cb) { _callback = cb; }
   void CallbackData(void *data)    { _callback_data = data; }
   ButtonCallback Callback()        { return _callback; }
   void * CallbackData()            { return _callback_data; }
   const char *IconFile()	    { return _iconFile; }
   void IconFile(char *);

   const UI_Class UIClass()         { return BUTTON; }
   const int UISubClass()           { return _button_type; }
   const char *const UIClassName()  { return "Button"; }
};

#endif /* BUTTON_H */
