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
/* $XConsortium: MainWindow.C /main/3 1995/11/06 09:42:36 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "MainWindow.h"
#include "Menu.h"
#include "IconObj.h"
#include "Button.h"
#include "Sep.h"
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include "Icon.h"


XtActionsRec MainWindow::actions[] =
{
  {"PopupMenu", (XtActionProc) &MainWindow::KeyboardPopupMenu}
};

class PopupMenuContainer : public MotifUI
{
  public:

   PopupMenuContainer(MotifUI *);
};

PopupMenuContainer::PopupMenuContainer(MotifUI *parent) : 
      MotifUI(parent, "PopupMenuContainer", NULL)
{
   _w = parent->BaseWidget();
}

MainWindow::MainWindow(MotifUI *parent,
		       char *name,
		       char *widgetName,
		       char *icon,
		       char *icon_name)
	: MotifUI(parent, name, NULL, widgetName)
{
   CreateMainWindow(parent, name, widgetName, icon, icon_name);
}

MainWindow::MainWindow(char *category,
		       MotifUI *parent,
		       char *name,
		       char *widgetName,
		       char *icon,
		       char *icon_name)
	: MotifUI(parent, name, category, widgetName)
{
   CreateMainWindow(parent, name, widgetName, icon, icon_name);
}

void MainWindow::CreateMainWindow(MotifUI *parent, char * /*name*/,
				  char *widgetName, char *_icon,
				  char *_icon_name)
{
   _w = XtVaCreateManagedWidget(widgetName, xmMainWindowWidgetClass, 
			        parent->BaseWidget(), NULL);
   XtAppAddActions(appContext, actions, XtNumber(actions));
   trans = XtParseTranslationTable("#override\n <Key> osfMenu: PopupMenu()");
   XtAddEventHandler(_w, ButtonPressMask, FALSE, PopupMenu, (XtPointer) this);
   XtOverrideTranslations(_w, trans);
   popups = new PopupMenuContainer(this);
   XtVaSetValues(_w, XmNuserData, this, NULL);
   LastPopupMenu = NULL;
   icon = NULL;
   icon_name = NULL;
   IconFile(_icon);
   IconName(_icon_name);
   InstallHelpCB();
}

MainWindow::~MainWindow()
{
   delete icon;
   delete icon_name;
}

void MainWindow::IconFile(char *_icon)
{
   if (_icon)
    {
      Pixmap pixmap, mask;
      GetPixmaps(_w, _icon, &pixmap, &mask);
      delete icon;
      if (pixmap != XmUNSPECIFIED_PIXMAP)
       {
         if (depth == 1)
            XtVaSetValues(XtParent(_w), XmNiconPixmap, pixmap, NULL);
         else
            XtVaSetValues(XtParent(_w), XmNiconPixmap, pixmap,
		          (mask ? XmNiconMask : NULL), mask, NULL);
         icon = STRDUP(_icon);
       }
      else
	 icon = NULL;
    }
}

void MainWindow::IconName(char *_icon_name)
{
   delete icon_name;
   if (!_icon_name)
      icon_name = STRDUP(Name());
   else
      icon_name = STRDUP(_icon_name);
   if (icon_name)
      XtVaSetValues(XtParent(_w), XmNiconName, icon_name, NULL);
}

void MainWindow::SetWorkWindow(MotifUI *obj)
{
   XtVaSetValues(_w, XmNworkWindow, obj->BaseWidget(), NULL);
}

Button *MainWindow::AddAction(char *name, char *category,
			      ButtonCallback callback,
			      void *callback_data, char *mnemonic,
			      char *acceleratorText, char *accelerator)
{
   Menu *menu = (Menu *)popups->FindByCategory(category);
   if (!menu)
    {
      menu = new Menu(category, popups, true, "", "", NULL, POPUP_MENU);
      XtVaSetValues(menu->InnerWidget(), XmNpopupEnabled, False, NULL);
    }
   return new Button(category, menu, name, PUSH_BUTTON, callback, callback_data,
	             mnemonic, acceleratorText, accelerator);
}

void MainWindow::RegisterPopup(MotifUI *obj)
{
   Menu *menu = (Menu *)popups->FindByCategory((char *)obj->Category());
   if (!menu)
    {
      menu = new Menu((char *)obj->Category(), popups, true, "", "", 
		      NULL, POPUP_MENU);
      XtVaSetValues(menu->InnerWidget(), XmNpopupEnabled, False, NULL);
    }
   XtAddEventHandler(obj->InnerWidget(), ButtonPressMask, FALSE,
		     PopupMenu, (XtPointer) this);
   XtOverrideTranslations(obj->InnerWidget(), trans);
   XtVaSetValues(obj->InnerWidget(), XmNuserData, obj, NULL);
   if (obj->UIClass() == CONTAINER)
    {
      switch (obj->UISubClass())
      {
      case SCROLLED_CANVAS:
      case SCROLLED_FORM:
      case SCROLLED_VERTICAL_ROW_COLUMN:
      case SCROLLED_HORIZONTAL_ROW_COLUMN:
      case SCROLLED_PANE:
      case SCROLLED_WORK_AREA:
      case SCROLLED_ICON_LIST:
         XtAddEventHandler(XtParent(obj->InnerWidget()), ButtonPressMask, FALSE,
		           PopupMenu, (XtPointer) this);
         XtOverrideTranslations(XtParent(obj->InnerWidget()), trans);
         XtVaSetValues(XtParent(obj->InnerWidget()), XmNuserData, obj, NULL);
      break;
      }
    }
}

void MainWindow::AddSep(char *category)
{
   Menu *menu = (Menu *)popups->FindByCategory(category);
   if (!menu)
    {
      menu = new Menu(category, popups, true, "", "", NULL, POPUP_MENU);
      XtVaSetValues(menu->InnerWidget(), XmNpopupEnabled, False, NULL);
    }
   new Sep(category, menu);
}

void MainWindow::KeyboardPopupMenu(Widget widget, XEvent *event, 
                                   String * /*params*/,
				   Cardinal * /*num_params*/)
{
   Widget focus_widget;

   if (!(focus_widget = XmGetFocusWidget(widget)))
      focus_widget = widget;

   MotifUI *obj;
   MainWindow *window;
   XtVaGetValues(focus_widget, XmNuserData, &obj, NULL);
   window = (MainWindow *) obj;
   while (window->UIClass() != MAIN_WINDOW)
      window = (MainWindow *) window->Parent();
   window->PostMenu(obj, event);
}

void MainWindow::PopupMenu(Widget widget, XtPointer client_data, 
                           XEvent *event, Boolean * /*continued*/)
{
   XRectangle          pixmap_rect, label_rect;
   XButtonEvent *      ev;
   Window              child, parent, root;
   int                 root_x, root_y, win_x, win_y;
   unsigned int        modMask;
   Display *           display = XtDisplay(widget);

   ev = (XButtonEvent *) event;
   if (ev->button != MotifUI::bMenuButton)
      return;
   child = XtWindow(widget);
   while (child)
    {
      parent = child;
      XQueryPointer(display, parent, &root, 
                    &child, &root_x, &root_y, &win_x, &win_y, &modMask);
    } 
   if (XtWindowToWidget(display, parent))
      widget = XtWindowToWidget(display, parent);

   MainWindow *window = (MainWindow *) client_data;
   MotifUI *obj;
   XtVaGetValues(widget, XmNuserData, &obj, NULL);
   if (!obj)
      obj = window;
   if (obj->UIClass() == ICON)
    { 
      GuiIconGetRects(widget, &pixmap_rect, &label_rect);
      if (!(PointInRect(pixmap_rect, win_x, win_y) ||
          PointInRect(label_rect, win_x, win_y)))
       {
          XtVaGetValues(XtParent(widget), XmNuserData, &obj, NULL);
       }
    }
   window->PostMenu(obj, event);
}

void MainWindow::PostMenu(MotifUI *obj, XEvent *event)
{
   BaseUI *popup_menu = popups->FindByCategory((char *)obj->Category());
   if (LastPopupMenu)
      XtVaSetValues(LastPopupMenu, XmNpopupEnabled, False, NULL);
   Widget widget = NULL;
   if (popup_menu)
    {
      popup_menu->Name((char *)obj->Name());
      PopupObject = obj;
      PopupObjectUniqueID = PopupObject->UniqueID();
      widget = ((MotifUI *) popup_menu)->BaseWidget();
      XtVaSetValues(widget, XmNpopupEnabled, True, NULL);
      if (event->type == ButtonPress)
        {
          XmMenuPosition(widget, (XButtonPressedEvent *) event);
          XtManageChild(widget);
        }
    } 
   else
    {
      PopupObject = NULL;
      PopupObjectUniqueID = 0;
    }
   LastPopupMenu = widget;
}
