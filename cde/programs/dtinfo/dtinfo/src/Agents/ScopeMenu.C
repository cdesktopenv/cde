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
/* $XConsortium: ScopeMenu.C /main/6 1996/10/14 17:59:18 cde-hal $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 * Copyright (c) 1994 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#include "UAS.hh"

#define C_TOC_Element
#define L_Basic

#define C_ScopeMenu
#define C_SearchScopeAgent
#define L_Agents

#define C_xList
#define L_Support

#define C_SearchScopeMgr
#define C_MessageMgr
#define L_Managers

#define C_WindowSystem
#define L_Other

#include "Prelude.h"

#define CLASS ScopeMenu
#include "create_macros.hh"

#include <WWL/WXmPushButtonGadget.h>
#include <WWL/WXmSeparator.h>
#include <WWL/WXmMenu.h>
#include <WWL/WXmCascadeButton.h>

ScopeMenu::ScopeMenu (Widget option_menu, bool require_current_section)
: f_current_scope (NULL),
  f_use_current_section(require_current_section),
  f_option_menu(option_menu)
{
  Arg args[1];
  int n;

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, &f_pull_menu); n++;
  XtGetValues(option_menu, args, n);

  fill_menu();

  UAS_SearchScope::request ((UAS_Receiver<ScopeCreated> *) this);
  UAS_SearchScope::request ((UAS_Receiver<ScopeDeleted> *) this);
  UAS_SearchScope::request ((UAS_Receiver<ScopeRenamed> *) this);

  SearchScopeAgent::request ((UAS_Receiver<UpdateMenu> *) this);
}

ScopeMenu::~ScopeMenu()
{
}


void
ScopeMenu::fill_menu()
{
  int position = 0;
  Wait_Cursor bob;

  // Create toggle buttons for each scope.
  xList<UAS_SearchScope *> &scope_list = search_scope_mgr().scope_list();
  List_Iterator<UAS_SearchScope *> s (scope_list);
  bool old_read_only = TRUE;

  // Set the current scope to the first entry
  //  (Current Section or Information Library) 
  f_current_scope = s.item();

  ON_DEBUG(cerr << "Scope Popup" << endl);

  int first = TRUE;
  for (; s != 0; s++)
    {
      
      // in case we are not using Current Section
      if (f_current_scope == NULL)
	f_current_scope = s.item();

      ON_DEBUG(cerr << "\t" << f_current_scope->name() << endl);
//    ON_DEBUG(f_current_scope->dump_items());

      // Add a separator when they change from read only to changable. 
      if (old_read_only != s.item()->read_only())
	{
	  DECLM (WXmSeparator, sep1, f_pull_menu, "separator");
          sep1.PositionIndex (position++);
	  old_read_only = FALSE;
	}
      DECLM (WXmPushButtonGadget, scope, f_pull_menu, s.item()->name());
      if (first && !f_use_current_section)
	{
	  first = FALSE;
	  scope.Unmanage();
	  // Reset current scope so that it is set to Library on next pass.
	  f_current_scope = NULL;
	}
      scope.UserData (s.item());
      scope.PositionIndex (position++);

      WCallback *cb = SET_CALLBACK(scope,Activate,set_scope);
      SET_CALLBACK_D (scope,Destroy,destroy_scope,cb);
      if (f_current_scope == s.item())
	{
	  Arg args[1];
	  int n = 0;
	  XtSetArg(args[n], XmNmenuHistory, (Widget)scope); n++;
	  XtSetValues(f_option_menu, args, n);
	}
    }

  //UAS_SearchScope::request ((UAS_Receiver<ScopeCreated> *) this);
  //UAS_SearchScope::request ((UAS_Receiver<ScopeDeleted> *) this);
  //UAS_SearchScope::request ((UAS_Receiver<ScopeRenamed> *) this);
}


void
ScopeMenu::set_scope (WCallback *wcb)
{
  f_current_scope =
	(UAS_SearchScope *) WXmPushButtonGadget(wcb->GetWidget()).UserData();
}


void
ScopeMenu::destroy_scope (WCallback *wcb)
{
  WCallback *cb = (WCallback *) wcb->ClientData();
  delete cb;
  delete wcb;
}

// /////////////////////////////////////////////////////////////////
// receive - handle scope created / deleted messages
// /////////////////////////////////////////////////////////////////

void
ScopeMenu::receive (ScopeCreated &msg, void *client_data)
{
  int position = 0;
  const char *scope_name = msg.f_search_scope->name();
  xList<UAS_SearchScope *> &scope_list = search_scope_mgr().scope_list();
  List_Iterator<UAS_SearchScope *> s (scope_list);
  bool need_sep = TRUE;

  int n;
  Arg args[2];
  WidgetList kids;
  Cardinal num_kids;

  n = 0;
  XtSetArg(args[n], XmNnumChildren, &num_kids); n++;
  XtSetArg(args[n], XmNchildren, &kids); n++;
  XtGetValues(f_pull_menu, args, n);

  UAS_SearchScope *scope;

  if (msg.f_search_scope->read_only())
  {
    // insert read-only scopes at the start; reserve position 0
    // for "Current Section" scope and position 1 for
    // the "All Libraries" scope
    position = 2;
    need_sep = FALSE;
  }
  else
  {
    // Scan the current menu to find the correct insertion position. 
    for (; s != 0; s++, position++)
    {
      scope = s.item();
      if (need_sep != scope->read_only())
      {
        position++;  // skip separator
        need_sep = FALSE;
      }
      if (scope->read_only())
	continue;
      // Find the first item that the new entry belongs after.
      ON_DEBUG (printf ("Scope strcmp to <%s>\n", scope->name()));
      if (strcmp (scope_name, scope->name()) < 0)
	break;
    }
  }

  ON_DEBUG (printf ("Final position = %d\n", position));

  // Add a separator if this is the first user-defined entry. 
  if (need_sep == TRUE)
  {
    DECLM (WXmSeparator, separator, f_pull_menu, "separator");
    separator.PositionIndex (position);
    position++;
  }

  // Create the new toggle button. 
  DECLM (WXmPushButtonGadget, scope_btn, f_pull_menu, scope_name);
  scope_btn.PositionIndex (position);
  scope_btn.UserData (msg.f_search_scope);
  WCallback *cb = SET_CALLBACK(scope_btn,Activate,set_scope);
  SET_CALLBACK_D(scope_btn,Destroy,destroy_scope,cb);
}

void
ScopeMenu::receive (ScopeDeleted &msg, void *client_data)
{
  // find the associated button and nuke it
  ON_DEBUG (puts ("ScopeMenu: handling delete message"));
  // First find renamed button in our list.
  Arg args[2];
  int n, theKid;
  Cardinal num_kids;
  WidgetList kids;
  int separator_pos = -1;
  Widget selected;

  n = 0;
  XtSetArg(args[n], XmNmenuHistory, &selected); n++;
  XtGetValues(f_option_menu, args, n);

  n = 0;
  XtSetArg(args[n], XmNnumChildren, &num_kids); n++;
  XtSetArg(args[n], XmNchildren, &kids); n++;
  XtGetValues(f_pull_menu, args, n);

  unsigned int i;
  for (i = 0; i < num_kids; i++)
    {
      if (XmIsSeparator (kids[i]))
      {
	separator_pos = i + 1;
        continue;
      }
      if (XmIsPushButtonGadget (kids[i]) &&
	  msg.f_search_scope ==
	  ((UAS_SearchScope *) WXmPushButtonGadget (kids[i]).UserData()))
	break;
    }

  // It had better be in the list! 
  theKid = i;
  Xassert (theKid != (int) num_kids);
  ON_DEBUG (printf ("widget #%d is the button\n", theKid));

  // if it is selected, select first w/ callback called
  if (kids[theKid] == selected)
    {
      n = 0;
      if (XtIsManaged (kids[0]))
	  XtSetArg(args[n], XmNmenuHistory, kids[0]);
      else
	  XtSetArg(args[n], XmNmenuHistory, kids[1]);
      n++;
      XtSetValues(f_option_menu, args, n);
    }

  XtUnmanageChild (kids[theKid]);
  XtDestroyWidget (kids[theKid]);

  // Get rid of the separator if no user scopes remain.
  ON_DEBUG (printf ("ScopeMenu: sep pos = %d, num_kids = %d (%d)\n",
		    separator_pos, num_kids, num_kids -1 -2));
  // - 1 for deleted widget
  if (separator_pos == (int) (num_kids - 1))
    {
      ON_DEBUG (puts ("   destroying separator"));
      XtDestroyWidget (kids[separator_pos-1]);
    }
}


void
ScopeMenu::receive (ScopeRenamed &msg, void *client_data)
{
  ON_DEBUG (puts ("ScopeMenu: handling rename message"));
  // First find renamed button in our list.
  Arg args[2];
  int n;
  Cardinal num_kids;
  WidgetList kids;

  n = 0;
  XtSetArg(args[n], XmNnumChildren, &num_kids); n++;
  XtSetArg(args[n], XmNchildren, &kids); n++;
  XtGetValues(f_pull_menu, args, n);

  unsigned int i;
  for (i = 0; i < num_kids; i++)
    {
      if (XmIsPushButtonGadget (kids[i]) &&
	  msg.f_search_scope ==
	  ((UAS_SearchScope *) WXmPushButtonGadget (kids[i]).UserData()))
	break;
    }

  // It had better be in the list! 
  Xassert (i != num_kids);
  ON_DEBUG (printf ("ScopeMenu: widget #%d is the button\n", i));

  // Now find the new insertion position in the list.
  int position = 0, old_position = i;
  xList<UAS_SearchScope *> &scope_list = search_scope_mgr().scope_list();
  List_Iterator<UAS_SearchScope *> s (scope_list);

  // find the new position in the list
  for (; s != 0; s++)
    {
      if (s.item() == msg.f_search_scope)
	continue;
      position++;
      if (s.item()->read_only())
	continue;
      ON_DEBUG (printf ("ScopeMenu: strcmp <%s>\n", s.item()->name()));
      if (strcmp (msg.f_search_scope->name(), s.item()->name()) < 0)
	break;
    }

  if (s == NULL)
    position++;

  ON_DEBUG (printf ("ScopeMenu: Rename position = %d, old = %d\n",
		    position, old_position));
  WXmPushButtonGadget scope_btn (kids[i]);
  scope_btn.LabelString (msg.f_search_scope->name());
  if (position != old_position)
    scope_btn.PositionIndex (position);
}

void
ScopeMenu::receive (UpdateMenu &msg, void *client_data)
{
  xList<UAS_SearchScope *> &scope_list = search_scope_mgr().scope_list();
  List_Iterator<UAS_SearchScope *> s (scope_list);

  int n;
  Arg args[2];
  WidgetList kids;
  Cardinal num_kids;

  n = 0;
  XtSetArg(args[n], XmNnumChildren, &num_kids); n++;
  XtSetArg(args[n], XmNchildren, &kids); n++;
  XtGetValues(f_pull_menu, args, n);

  // destroy all toggle buttons in menu
  for (unsigned int i = 0; i < num_kids; i++)
  {
    XtUnmanageChild (kids[i]);
    XtDestroyWidget (kids[i]);
  }

  // select current scope
  if(f_use_current_section)
    f_current_scope = s.item();
  else
  {
    s++;
    f_current_scope = s.item();
  }
  fill_menu();
  XtUnmanageChild(XtParent(f_option_menu));
  XtManageChild(XtParent(f_option_menu));
  XtUnmanageChild(f_option_menu);
  XtManageChild(f_option_menu);
}

