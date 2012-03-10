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
