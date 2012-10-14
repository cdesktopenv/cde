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
/*
 * $XConsortium: NodeListAgentMotif.C /main/8 1996/10/15 17:37:13 cde-hal $
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

#define C_TOC_Element
#define L_Basic

#define C_NodeListAgent
#define L_Agents

#define C_MessageMgr
#define L_Managers

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Prelude.h"

#include "Registration.hh"

#include <WWL/WTopLevelShell.h>
#include <WWL/WXmMenu.h>
#include <WWL/WXmFrame.h>
#include <WWL/WXmTextField.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmSeparator.h>
#include <WWL/WXmLabel.h>
#include <WWL/WXmList.h>
#include <WWL/WXmPanedWindow.h>

Cardinal NodeListAgent::f_pane_insert_position = 0;

// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

NodeListAgent::~NodeListAgent()
{
  // NOTE: Maybe this variable should be in the agent! 
  delete f_wm_delete_callback;
  // NOTE: How about deleting the widget hierarchy!! djb 
}


// /////////////////////////////////////////////////////////////////
// init - let's get started
// /////////////////////////////////////////////////////////////////

void
NodeListAgent::init()
{
  f_shell = NULL;
  f_list = NULL;
  setInitialized();
  setStatus (eSuccess);
}

// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
NodeListAgent::display()
{
  if (!initialized())
    {
      // Error
      return;
    }

  /* -------- Create Motif ui if necessary. -------- */
  Xassert (f_shell != NULL);
  // Call to create_window should be in subclass

  f_shell->Popup();
  f_shell->DeIconify();
  setStatus (eSuccess);
}

// /////////////////////////////////////////////////////////////////
// create_base_window
// /////////////////////////////////////////////////////////////////

/* This should be called from subclasses create_window method. */

void
NodeListAgent::create_base_window (char *name)
{
  f_shell = new WTopLevelShell (toplevel(), WPopup, name);
  window_system().register_shell (f_shell);
#define AM WAutoManage
//Widget Class       Var Name        Parent           Widget Name
//------------------ --------------- ---------------- ----------------
  f_form     = new WXmForm    (*f_shell, "form");
  f_menu_bar = new WXmMenuBar (*f_form, (char*)"menu_bar" /* , WAutoManage */);

  WXmCascadeButton   file_cascade    (*f_menu_bar,    "file",           AM);
  WXmPulldownMenu    file_menu       (*f_menu_bar,    (char*)"file_menu"  );
  WXmPushButton      close           (file_menu,      "close",          AM);
  WXmCascadeButton   edit_cascade    (*f_menu_bar,    "edit",           AM);
  WXmPulldownMenu    edit_menu       (*f_menu_bar,    (char*)"edit_menu"  );
  WXmPushButton      undo            (edit_menu,      "undo",           AM);
  /*
  WXmCascadeButton   view_cascade    (*f_menu_bar,    "view",           AM);
  WXmPulldownMenu    view_menu       (*f_menu_bar,    (char*)"view_menu"  );
  WXmCascadeButton   sort_cascade    (view_menu,      "sort",           AM);
  WXmPulldownMenu    sort_menu       (*f_menu_bar,    (char*)"sort_menu"  );
  WXmPushButton      ooo             (sort_menu,      "ooo",            AM);
  */

  WXmCascadeButton   help_cascade    (*f_menu_bar,    "help",           AM);
  WXmPulldownMenu    help_menu       (*f_menu_bar,    (char*)"help_menu"  );
  WXmPushButton      context         (help_menu,      "context",        AM);
  WXmPushButton      window          (help_menu,      "window",         AM);

  XmStringLocalized mtfstring;

  f_display = WXmPushButton (*f_form,       "display",        AM);
  f_help    = WXmPushButton (*f_form,       "help",        AM);
  WXmPushButton      closeb          (*f_form,       "close",          AM);

  mtfstring = CATGETS(Set_AgentLabel, 165, "Display");
  XtVaSetValues((Widget)f_display, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 12, "Close");
  XtVaSetValues((Widget)closeb, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 48, "Help");
  XtVaSetValues((Widget)f_help, XmNlabelString, (XmString)mtfstring, NULL);

  WXmSeparator       separator       (*f_form,        "separator",      AM);

  f_pane = (WXmPanedWindow*)(Widget)WXmPanedWindow (*f_form, "pane");
  f_pane->InsertPosition (&NodeListAgent::pane_insert_position);
  
  f_list_form  = (WXmForm*)(Widget)WXmForm(*f_pane, "listform", AM);

  Widget scrolled_list =  XmCreateScrolledList (*f_list_form, (char*)"list", NULL, 0);

  f_list = (WXmList*)scrolled_list ;
  f_list->Manage();

  XmFontList defaultList;
  {
    XmFontList font;
    XtVaGetValues(scrolled_list, XmNfontList, &font, NULL);
    defaultList = XmFontListCopy(font);
  }

  if (window_system().dtinfo_font())
    defaultList = XmFontListAppendEntry(defaultList,
					window_system().dtinfo_font());
  if (window_system().dtinfo_space_font())
    defaultList = XmFontListAppendEntry(defaultList,
					window_system().dtinfo_space_font());
  XtVaSetValues(scrolled_list, XmNfontList, defaultList, NULL);
  
#undef AM

  // NOTE: error checking

  /* -------- attach menus -------- */
  file_cascade.SubMenuId (file_menu);
  edit_cascade.SubMenuId (edit_menu);
  help_cascade.SubMenuId (help_menu);
  f_menu_bar->MenuHelpWidget (help_cascade);

  close.SetActivateCallback (this, (WWL_FUN)&NodeListAgent::close_window);
  closeb.SetActivateCallback (this, (WWL_FUN)&NodeListAgent::close_window);
  f_wm_delete_callback =
    new WCallback (*f_shell, window_system().WM_DELETE_WINDOW(),
		   this, (WWL_FUN) &NodeListAgent::close_window);

  f_form->ShadowThickness (0);   // gets rid of border turds
  f_form->DefaultButton (f_display);
}

// /////////////////////////////////////////////////////////////////
// insert_position
// /////////////////////////////////////////////////////////////////

Cardinal
NodeListAgent::pane_insert_position (Widget w)
{
  extern WidgetClass xmSashWidgetClass, xmSeparatorGadgetClass;
  Cardinal position;
  if (XtIsSubclass (w, xmSashWidgetClass) ||
      XtIsSubclass (w, xmSeparatorGadgetClass))
    {
      WComposite cw (XtParent(w));
      position = cw.NumChildren();
    }
  else
    position = NodeListAgent::f_pane_insert_position;

  /* Code can set this var to determine the insert position of widgets. */
  return (position);
  // NodeListAgent::f_pane_insert_position = 0;
}


// /////////////////////////////////////////////////////////////////
// close_window
// /////////////////////////////////////////////////////////////////

void
NodeListAgent::close_window (WCallback *)
{
  f_shell->Popdown();
}
