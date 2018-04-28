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
/* $TOG: Button.C /main/3 1998/07/24 16:14:59 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "Button.h"

#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/ArrowB.h>
#include "WorkArea.h"

Button::Button(MotifUI *parent, char *name, ButtonType button_type,
	       ButtonCallback callback, void * callback_data,
	       char *mnemonic, char *acceleratorText,
	       char *accelerator, char *iconFile)
	: MotifUI(parent, name, NULL)
{
   CreateButton(parent, name, NULL, button_type, callback, callback_data,
	        mnemonic, acceleratorText, accelerator, iconFile);
}

Button::Button(char *category, MotifUI *parent, char *name,
	       ButtonType button_type, ButtonCallback callback,
	       void * callback_data, char *mnemonic, char *acceleratorText,
	       char *accelerator, char *iconFile)
	: MotifUI(parent, name, category)
{
   CreateButton(parent, name, category, button_type, callback, callback_data,
	        mnemonic, acceleratorText, accelerator, iconFile);
}

Button::~Button()
{
   delete _iconFile;
}

void Button::IconFile(char *iconFile)
{
   delete _iconFile;
   _iconFile = STRDUP(iconFile);
   if (_iconFile)
    {
      Pixmap pixmap;
      char *s = new char[strlen(iconFile) + 6];
      if (depth == 1)
       {
	 Pixel bg;
	 XtVaGetValues(_w, XmNbackground, &bg, NULL);
	 if (bg == black)
            sprintf(s, "%s.m.pm", iconFile);
	 else
            sprintf(s, "%s.m.bm", iconFile);
       }
      else
         sprintf(s, "%s.m.pm", iconFile);
      GetPixmaps(_w, s, &pixmap);
      delete [] s;
      XtVaSetValues(_w, XmNlabelType, XmPIXMAP, XmNlabelPixmap, pixmap, NULL);
    }
   else
      XtVaSetValues(_w, XmNlabelType, XmSTRING, NULL);
}

void Button::CreateButton(MotifUI *parent, char *name, char * /*category*/,
	       ButtonType button_type, ButtonCallback callback,
	       void * callback_data, char *mnemonic,
	       char *acceleratorText, char *accelerator, char *iconFile)
{
   int IsArrow = false;
   XmString xm_string = StringCreate(name);
   Widget parentW;
   Widget super_node = NULL;

   _iconFile = NULL;
   _button_type = button_type;

   if (parent->UIClass() == DIALOG)
      parentW = parent->BaseWidget();
   else if (parent->UIClass() == ICON)
    {
      parentW = XtParent(parent->InnerWidget());
      super_node = parent->InnerWidget();
    }
   else
      parentW = parent->InnerWidget();
   if (button_type == PUSH_BUTTON)
      _w = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass, parentW, 
                                   XmNlabelString, xm_string,
		                   XmNaccelerator, accelerator,
				   XmNmultiClick, XmMULTICLICK_DISCARD,
                                   GuiNsuperNode, super_node, NULL);
   else if (button_type == TOGGLE_BUTTON)
      _w = XtVaCreateManagedWidget("toggle", xmToggleButtonWidgetClass, parentW,
				   XmNlabelString, xm_string,
		                   XmNaccelerator, accelerator,
				   XmNmultiClick, XmMULTICLICK_DISCARD,
                                   GuiNsuperNode, super_node, NULL);
   else
    {
      int dir;

      switch (button_type)
       {
	case UP_ARROW_BUTTON: dir = XmARROW_UP;
	case DOWN_ARROW_BUTTON: dir = XmARROW_DOWN;
        case LEFT_ARROW_BUTTON: dir = XmARROW_LEFT;
        case RIGHT_ARROW_BUTTON: dir = XmARROW_RIGHT;
       }
      _w = XtVaCreateManagedWidget("arrow", xmArrowButtonWidgetClass, parentW, 
				   XmNmultiClick, XmMULTICLICK_DISCARD,
				   XmNarrowDirection, dir, NULL);
      IsArrow = true;
    }
   StringFree(xm_string);
   if (!IsArrow)
    {
      if (mnemonic)
         XtVaSetValues(_w, XmNmnemonic, XStringToKeysym(mnemonic), NULL);
      if (acceleratorText)
       {
         xm_string = StringCreate(acceleratorText);
         XtVaSetValues(_w, XmNacceleratorText, xm_string, NULL);
         StringFree(xm_string);
       }
    }
   _callback = callback;
   _callback_data = callback_data;

   if (button_type == TOGGLE_BUTTON)
      XtAddCallback(_w, XmNvalueChangedCallback, &Button::ActivateCB, 
                    (XtPointer) this);
   else
      XtAddCallback(_w, XmNactivateCallback, &Button::ActivateCB, 
                    (XtPointer) this);
   InstallHelpCB();
   IconFile(iconFile);
}

void Button::ActivateCB(Widget w, 
			XtPointer client_data,
			XtPointer)
{
   Button *obj = (Button *) client_data;
   ButtonCallback callback;

   if (XmIsToggleButton(w))
    {
      if (obj->Selected())
	 obj->Selected(false);
      else
	 obj->Selected(true);
    }
   if ((callback = obj->_callback))
    {
      if (obj->_callback_data)
         (*callback)(obj->_callback_data);
      else
         (*callback)(obj);
    }
}
