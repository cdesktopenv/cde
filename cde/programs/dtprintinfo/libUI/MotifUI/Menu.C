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
/* $XConsortium: Menu.C /main/3 1995/11/06 09:42:58 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "Menu.h"
#include "MainWindow.h"

#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <Xm/CascadeB.h>

Menu::Menu(MotifUI *parent, char *name, char *mnemonic, MenuType menu_type)
	: MotifUI(parent, name, NULL)
{
   CreateMenu(parent->InnerWidget(), name, NULL, mnemonic, menu_type);
}

Menu::Menu(MotifUI *parent, boolean /*has_title*/, char *title,
	   char *name, char *mnemonic, MenuType menu_type)
	: MotifUI(parent, name, NULL)
{
   CreateMenu(parent->InnerWidget(), name, NULL, mnemonic, menu_type);
   CreateTitle(parent->InnerWidget(), NULL, title);
}

Menu::Menu(char *category, MotifUI *parent, char *name, char *mnemonic, 
	   MenuType menu_type)
	: MotifUI(parent, name, category)
{
   CreateMenu(parent->InnerWidget(), name, category, mnemonic, menu_type);
}

Menu::Menu(char *category, MotifUI *parent, boolean /*has_title*/,
	   char *title, char *name, char *mnemonic, MenuType menu_type)
	: MotifUI(parent, name, category)
{
   CreateMenu(parent->InnerWidget(), name, category, mnemonic, menu_type);
   CreateTitle(parent->InnerWidget(), category, title);
}

void Menu::SetRadio(boolean flag)
{
   XtVaSetValues(_w, XmNradioBehavior, flag, NULL);
}

boolean Menu::SetActivity(boolean flag)
{
   XtSetSensitive(_cascadeButton, flag);
   return true;
}

void Menu::CreateMenu(Widget parent, char *name, char * /*category*/,
		      char *mnemonic, MenuType menu_type)
{
   XmString xm_string = StringCreate(name);

   _title = NULL;
   _sep = NULL;
   switch (_menu_type = menu_type)
   {
   case PULLDOWN_MENU:
      _w = XmCreatePulldownMenu(parent, "pulldown_menu", NULL, 0);
      break;
   case OPTION_MENU:
      _w = XmCreateOptionMenu(parent, "option_menu", NULL, 0);
      break;
   case POPUP_MENU:
      _w = XmCreatePopupMenu(parent, "popup_menu", NULL, 0);
      XtVaSetValues(_w, XmNwhichButton, MotifUI::bMenuButton, NULL);
      break;
   }
   if (_menu_type != POPUP_MENU)
    {
      _cascadeButton = XtVaCreateManagedWidget("cascadeButton",
					       xmCascadeButtonWidgetClass,
					       parent,
					       XmNlabelString, xm_string,
					       XmNsubMenuId, _w, NULL);
      if (depth == 1)
       {
	 Pixel bg;
	 XtVaGetValues(_cascadeButton, XmNbackground, &bg, NULL);
	 if (bg == white)
	  {
	    XtVaSetValues(_w, XmNuserData, (XtPointer)this, NULL);
	    XtAddCallback(_w, XmNmapCallback, &(Menu::MapCB), NULL);
	    XtAddCallback(_w, XmNunmapCallback, &(Menu::MapCB), NULL);
	  }
       }
    }
   StringFree(xm_string);
   if (mnemonic)
      XtVaSetValues(_cascadeButton,
		    XmNmnemonic, XStringToKeysym(mnemonic), NULL);
   InstallHelpCB();
}

void Menu::CreateTitle(Widget /*parent*/, char *title, char * /*category*/)
{
   XmString xm_string = StringCreate(title);

   if (_title)
      XtVaSetValues(_title, XmNlabelString, xm_string, NULL);
   else
    {
      _title = XtVaCreateManagedWidget("title", xmLabelWidgetClass, _w, 
			               XmNlabelString, xm_string, NULL);
      _sep = XtVaCreateManagedWidget("separator", xmSeparatorWidgetClass, _w, 
			             XmNseparatorType, XmDOUBLE_LINE, NULL);
    }
   StringFree(xm_string);
}

boolean Menu::SetName(char *name)
{
   CreateTitle(_w, name, (char *) Category());
   return true;
}

void Menu::MapCB(Widget w, XtPointer /*client_data*/, XtPointer /*call_data*/)
{
   Menu *obj;
   XtVaGetValues(w, XmNuserData, &obj, NULL);
   Pixel fg;
   XtVaGetValues(obj->_cascadeButton, XmNforeground, &fg, NULL);
   if (fg == white)
      fg = black;
   else
      fg = white;
   XtVaSetValues(obj->_cascadeButton, XmNforeground, fg, NULL);
}
