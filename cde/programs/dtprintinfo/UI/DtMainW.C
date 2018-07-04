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
/* $TOG: DtMainW.C /main/5 1998/07/24 16:12:56 mgreess $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "DtMainW.h"
#include "Application.h"
#include "Menu.h"
#include "MenuBar.h"
#include "LabelObj.h"
#include "Sep.h"
#include "Button.h"
#include "IconObj.h"
#include "DtWorkArea.h"
#include "DtActions.h"
#include "DtSetModList.h"
#include "DtFindD.h"
#include "HelpSystem.h"

#include "BaseObj.h"

#include "dtprintinfomsg.h"

#include <stdlib.h>
#include <stdio.h>

static char *GetIcon(AnyUI *obj, PrinterApplicationMode app_mode)
{
   if (app_mode == SINGLE_PRINTER)
    {
      if (obj->depth == 1)
         return "Fpprnt.l.bm";
      else
         return "Fpprnt.l.pm";
    }
   else
    {
      if (obj->depth == 1)
         return "FpPrtmg.l.bm";
      else
         return "FpPrtmg.l.pm";
    }
}

DtMainW::DtMainW(char *category,
                 AnyUI *p, 
	         char *name, 
		 ContainerType container_type,
		 SelectionType select_type,
		 OpenCallback openCB, 
		 void *openCallbackData,
		 PreferenceCallback prefCB, 
		 void *prefCallbackData,
		 char *fileMenuName,
		 char *fileMenuMnemonic,
		 PrinterApplicationMode _app_mode)
	: MainWindow(category, p, name, name, GetIcon(p, _app_mode))
{
   Visible(true);
   in_find = false;
   working_curs = 0;
   app_mode = _app_mode;
   _openClose = openCB;
   _prefCB = prefCB;
   if (openCallbackData)
      _openCallbackData = openCallbackData;
   else
      _openCallbackData = this;
   if (prefCallbackData)
      _prefCallbackData = prefCallbackData;
   else
      _prefCallbackData = this;
   _fileMenuName = fileMenuName;
   _fileMenuMnemonic = fileMenuMnemonic;
   _container_type = container_type;
   _select_type = select_type;
   setModList = NULL;
   setPrefD = NULL;
   findD = NULL;
   helpSystem = NULL;
   findSetD = NULL;
   action_data = (ActionData **) malloc(sizeof(ActionData *));
   n_action_data = 0;
}

DtMainW::~DtMainW()
{
   int i;
   for (i = 0; i < n_action_data; i++)
    {
      delete action_data[i]->actionReferenceName;
      delete action_data[i];
    }
   delete action_data;
}

void DtMainW::DisplayHelp(char *location_id, char *volume)
{
   char old_msg[200];

   snprintf(old_msg, sizeof(old_msg), "%s", status_line->Name());
   status_line->Name(MESSAGE(GettingHelpL));
   WorkingCursor(true);
   if (!helpSystem)
    {
      char *title = new char[120];
      sprintf(title, "%s - %s", Name(), MESSAGE(HelpL));

      helpSystem = new HelpSystem(this, title, volume, location_id);
      helpSystem->Visible(true);
      delete [] title;
    }
   else
    {
      helpSystem->Visible(true);
      helpSystem->Refresh();
      helpSystem->HelpVolume(volume, location_id);
    }
   status_line->Name(old_msg);
   WorkingCursor(false);
   helpSystem->ToFront();
}

void DtMainW::Initialize()
{
   mbar = new MenuBar(this);
   Container *form = new DtWorkArea((char *) Category(), this, (char *)Name(), 
				    FORM);
   container = new DtWorkArea((char *) Category(), form, (char *)Name(), 
                              _container_type, _select_type);

   status_line = new LabelObj(form, " ", LEFT_JUSTIFIED, true);
   status_line->AttachBottom();
   status_line->AttachLeft();
   status_line->AttachRight();
   container->AttachTop();
   container->AttachLeft();
   container->AttachRight();
   container->AttachBottom(status_line);
   if (app_mode == SINGLE_PRINTER)
      container->WidthHeight(600, 150);
   else
      container->WidthHeight(600, 400);
   SetWorkWindow(form);

   char *title = new char[100];
   sprintf(title, MESSAGE(SetOptionsTitleL), Name());
   setPrefD = new DtSetPref(this, title, container, _prefCB,
                            _prefCallbackData);
   delete [] title;

   fileMenu = new DtMenu(mbar, _fileMenuName, _fileMenuMnemonic,
			 "PrinterMenuDE");
   exitB = new Button(fileMenu, MESSAGE(ExitChoiceL), PUSH_BUTTON, ExitCB, this,
		      MESSAGE(ExitMnemonicL), MESSAGE(ExitAcceleratorL));

   actionsMenu = new DtActions(mbar, MESSAGE(SelectedMenuL),
			       MESSAGE(SelectedAcceleratorL));
   actionsMenu->Active(false);

   if (app_mode != CONFIG_PRINTERS)
    {
      viewMenu = new DtMenu(mbar, MESSAGE(ViewMenuL),
			    MESSAGE(ViewAcceleratorL), "ViewMenuDE");

      setPref = new Button(viewMenu, MESSAGE(SetOptionsChoiceL), PUSH_BUTTON,
                           SetPrefCB, setPrefD, MESSAGE(SetOptionsMnemonicL));
    }
   else
      viewMenu = NULL;
   helpMenu = new DtMenu(mbar, MESSAGE(HelpChoiceL), MESSAGE(HelpMnemonicL),
			 "HelpMenuDE");
   introduction = new Button(helpMenu, MESSAGE(OverviewChoiceL), PUSH_BUTTON,
                             HelpCB, this, MESSAGE(OverviewMnemonicL));
   new Sep(helpMenu);
   tasks = new Button(helpMenu, MESSAGE(TaskChoiceL), PUSH_BUTTON, TasksCB,
		      this, MESSAGE(TaskMnemonicL));
   reference = new Button(helpMenu, MESSAGE(ReferenceChoiceL), PUSH_BUTTON,
			  ReferenceCB, this, MESSAGE(ReferenceMnemonicL));
   onWindow = new Button(helpMenu, MESSAGE(OnItemChoiceL), PUSH_BUTTON,
			 OnItemCB, this, MESSAGE(OnItemMnemonicL));
   new Sep(helpMenu);
   usingHelp = new Button(helpMenu, MESSAGE(UsingHelpChoiceL), PUSH_BUTTON,
			  UsingHelpCB, this, MESSAGE(UsingHelpMnemonicL));
   new Sep(helpMenu);
   char *tmp;
   if (app_mode == PRINT_MANAGER)
      tmp = MESSAGE(AboutChoice1L);
   else if (app_mode == SINGLE_PRINTER)
      tmp = MESSAGE(AboutChoice2L);
   else
      tmp = MESSAGE(AboutChoice3L);
   version = new Button(helpMenu, tmp, PUSH_BUTTON, AboutCB,
			this, MESSAGE(AboutMnemonicL));

   mbar->SetHelpMenu(helpMenu);
}

void DtMainW::SetPrefCB(void *data)
{
   DtSetPref *obj = (DtSetPref *) data;

   obj->Visible(true);
}

void DtMainW::ExitCB(void *data)
{
   Application *app = (Application *)((BaseUI *)data)->Parent();
   app->SaveMe();
   delete (BaseObj *)((BaseUI *)data)->ApplicationData;
   delete ((BaseUI *)data)->Parent();
   exit (0);
}

void DtMainW::OpenClose(BaseUI *obj)
{
   if (_openClose)
      (*_openClose)(_openCallbackData, obj);
}

void DtMainW::DtAddAction(char *name, char *category, char *actionName,
			  ActionCallback callback, void *callback_data,
			  char *mnemonic, char *acceleratorText,
			  char *accelerator)
{
   BaseUI *action1;
   if (action1 = actionsMenu->FindByName(name))
      action1->Category(NULL);
   else
      actionsMenu->AddAction(name, category, actionName, callback, 
			     callback_data, mnemonic, acceleratorText,
			     accelerator);

   int size = sizeof(ActionData *) * (n_action_data + 1);
   action_data = (ActionData **) realloc(action_data, size);
   action_data[n_action_data] = (ActionData *)malloc(sizeof(ActionData));
   action_data[n_action_data]->actionReferenceName = strdup(actionName);
   action_data[n_action_data]->callback_data = callback_data;
   action_data[n_action_data]->actionCallback = callback;
   Button *action = AddAction(name, category, &DtMainW::ActionCB, NULL,
			      mnemonic, acceleratorText, accelerator); 
   action->ApplicationData = action_data[n_action_data];
   n_action_data++;
}

void DtMainW::DtAddSep(char *category)
{
   actionsMenu->AddSep(category);
   AddSep(category);
}

void DtMainW::ActionCB(void *callback_data)
{
   Button *action = (Button *) callback_data;
   DtMainW *window = (DtMainW *)action->Parent()->Parent()->Parent();
   ActionData *cb = (ActionData *) action->ApplicationData;
   if (cb->actionCallback)
    {
      if (window->container->ObjectExists(window->PopupObjectUniqueID))
       {
         (*cb->actionCallback)(cb->callback_data, window->PopupObject,
                               cb->actionReferenceName);
       }
      else
       {
	 Dialog *dialog = new Dialog(window, (char *)window->Name(), 
				     MESSAGE(NotFoundMessageL), INFORMATION,
				     MESSAGE(OKL));
	 dialog->Visible(true);
       }
    }
}

void DtMainW::HelpCB(void *data)
{
   DtMainW * obj = (DtMainW *) data;
   obj->DisplayHelp("_hometopic");
}

void DtMainW::ReferenceCB(void *data)
{
   DtMainW * obj = (DtMainW *) data;
   obj->DisplayHelp("Reference");
}

void DtMainW::TasksCB(void *data)
{
   DtMainW * obj = (DtMainW *) data;
   obj->DisplayHelp("Tasks");
}

void DtMainW::UsingHelpCB(void *data)
{
   DtMainW * obj = (DtMainW *) data;
   obj->DisplayHelp("_hometopic", "Help4Help");
}

void DtMainW::AboutCB(void *data)
{
   DtMainW * obj = (DtMainW *) data;
   obj->DisplayHelp("_copyright");
}

void DtMainW::OnItemCB(void *data)
{
   DtMainW * obj = (DtMainW *) data;

   obj->ContextualHelp();
}

boolean DtMainW::HandleHelpRequest()
{
   if (app_mode == PRINT_MANAGER)
      DisplayHelp("MainWindowDE");
   else
      DisplayHelp("PJMainWindowDE");
   return true;
}

void DtMainW::WorkingCursor(boolean flag)
{
   int i;
   BaseUI **children = Children();
   if (flag)
    {
      if (working_curs == 0)
         for (i = 0; i < NumChildren(); i++, children++)
	    (*children)->PointerShape(HOUR_GLASS_CURSOR);
      working_curs++;
    }
   else
    {
      working_curs--;
      if (working_curs == 0)
         for (i = 0; i < NumChildren(); i++, children++)
	    (*children)->PointerShape(LEFT_SLANTED_ARROW_CURSOR);
    }

}

boolean DtMenu::HandleHelpRequest()
{
   ((DtMainW *)Parent()->Parent())->DisplayHelp(location_id);
   return true;
}
