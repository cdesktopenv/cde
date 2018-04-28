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
/* $TOG: DtActions.C /main/4 1998/08/03 16:30:18 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "stdlib.h"

#include "DtActions.h"
#include "DtMainW.h"
#include "Button.h"
#include "Sep.h"

DtActions::DtActions(AnyUI *parent,
	             char *name,
		     char *mnemonic)
	: Menu(parent, name, mnemonic)
{
   // Empty
}

class DtAction : public Button {

 public:

   static void ActionCB(void *);
   char *actionReferenceName;
   ActionCallback actionCallback;
   void *actionCallbackData;

   ~DtAction();

   DtAction(char *category, AnyUI *parent, char *name, char *mnemonic,
	    char *acceleratorText, char *accelerator, char *actionReferenceName,
	    ActionCallback callbackCB, void *callback_data);
};

DtAction::DtAction(char *category,
		   AnyUI *parent,
		   char *name,
		   char *mnemonic,
	           char *acceleratorText,
	           char *accelerator,
		   char *actionName,
	           ActionCallback callbackCB,
		   void *callback_data)
	: Button(category, parent, name, PUSH_BUTTON, NULL, NULL, 
                 mnemonic, acceleratorText, accelerator)
{
   actionReferenceName = strdup(actionName);
   actionCallback = callbackCB;
   actionCallbackData = callback_data;
   Callback(ActionCB);
}

DtAction::~DtAction()
{
   free(actionReferenceName);
}

void DtAction::ActionCB(void *callback_data)
{
   DtAction *action = (DtAction *) callback_data;
   BaseUI *obj = ((DtActions *)action->Parent())->SelectedObject();
   if (obj && action->actionCallback)
      (*action->actionCallback)(action->actionCallbackData, obj,
			        action->actionReferenceName);
}

void DtActions::AddSep(char *objectClassName)
{
   new Sep(objectClassName, this);
}

void DtActions::AddAction(char *name, char *category, char *actionReferenceName,
			  ActionCallback callback, void *callback_data,
			  char *mnemonic, char *acceleratorText,
			  char *accelerator)
{
   new DtAction(category, this, name, mnemonic, acceleratorText, accelerator,
		actionReferenceName, callback, callback_data);
}

void DtActions::UpdateActions(int n_items, BaseUI *obj)
{
   if (n_items == 1)
    {
      Active(true);
      int i;
      for (i = 0; i < _numChildren; i++)
       {
	 if (!_children[i]->Category() ||
	     !STRCMP(_children[i]->Category(), obj->Category()))
	    _children[i]->Visible(true);
	 else
	    _children[i]->Visible(false);
       }
    }
   else
      Active(false);
   selected_object = obj;
}

boolean DtActions::HandleHelpRequest()
{
   DtMainW *window = (DtMainW *)Parent()->Parent();
   if (Active())
    {
      if (strcmp(selected_object->Category(), "Queue"))
         window->DisplayHelp("SelectedPrintJobMenuDE");
      else
         window->DisplayHelp("SelectedPrinterMenuDE");
    }
   else if (window->PrinterAppMode() == PRINT_MANAGER)
      window->DisplayHelp("SelectedMenuDE");
   else
      window->DisplayHelp("PJSelectedMenuDE");
   return true;
}
