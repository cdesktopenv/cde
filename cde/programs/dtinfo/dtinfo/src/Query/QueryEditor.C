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
/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

/*
 * $XConsortium: QueryEditor.C /main/14 1996/10/11 17:11:51 cde-hal $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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

#define C_TOC_Element
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_QueryEditor
#define C_QueryGroup
#define C_QueryTerm
#define C_QueryGroupView
#define L_Query

#define C_HelpAgent
#define L_Agents

#define C_SearchMgr
#define C_SearchScopeMgr
#define C_MessageMgr
#define L_Managers

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Prelude.h"

#include "Registration.hh"

#include <WWL/WXmMenu.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmLabel.h>
#include <WWL/WXmCascadeButton.h>
#include <WWL/WXmSeparator.h>
#include <WWL/WXmPanedWindow.h>

#include <string.h>
#include <iostream>
using namespace std;

#define CLASS QueryEditor
#include "../Agents/create_macros.hh"

#include "utility/mmdb_exception.h"

//static bool g_allow_query_text_change;
  
QueryEditor *QueryEditor::f_query_editor;

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

QueryEditor::QueryEditor(UAS_SearchEngine& search_engine)
: f_shell (NULL),
  f_query (NULL),
  f_query_view (NULL),
  f_null_terms (0)
{
  f_query_editor = this;

  // NOTE: QueryTerm::f_caps is global. It needs to be term-wise
  //       to get terms to have varied capabilities.
  QueryTerm::avail_caps(search_engine.avail_caps());

#ifdef DEBUG
  fprintf(stderr, "(DEBUG) search engine capabilities = 0x%x\n",
						QueryTerm::avail_caps());
#endif
}


// /////////////////////////////////////////////////////////////////
// display - display the query editor
// /////////////////////////////////////////////////////////////////

void
QueryEditor::display()
{
  static int count = 0;
  if (count++ == 0) {
    if (f_shell == 0)
      create_ui();

    if (f_query == NULL)
      edit_query (NULL);

    f_shell.Popup();
    f_shell.DeIconify();
  }
  count--;
}


// /////////////////////////////////////////////////////////////////
// new_query - set up interface for a new query
// /////////////////////////////////////////////////////////////////

void
QueryEditor::edit_query (QueryGroup *query)
{
  static Boolean first_time = True;

//#ifdef UseFJMTF
#if 1
  // Fujitsu Motif seems to be buggy for HaL's Restraint widget.
  // You must not destroy and re-create the Restraint widget.
  // Instead keep the same Restraint widget forever. - 10/22/94 kamiya

  // Create a group with a single term if none provided.
  if (query == NULL)
    {
      query = new QueryGroup();
      // This QueryTerm is pointed to by the QueryGroup.
      new QueryTerm (query, NULL, NULL);
    }

  f_query = query;

  // re-load the query terms
  if (f_query_view != NULL)
	f_query_view->re_load_terms(f_query);

#else
  // Clean up old junk.
  // The query isn't destroyed sometimes??
  // Need to nuke only if it isn't saved on the history list.  DJB 
  if (f_query_view != NULL)
    {
      QueryGroup *group = f_query_view->query_group();
      f_query_view->destroy_widgets();
      delete f_query_view;
      delete f_query;
    }

  // NOTE: old query probably leaked!! 15:06 01/29/93 DJB
  // This is going to need to be fixed up to edit pre-existing querys.
  // NOTE: Another problem if this is called before the UI is created,
  // because thar's widgets created below.  17:19 02/03/93 DJB
  f_query = query;

  // Create a group with a single term if none provided.
  if (f_query == NULL)
    {
      f_null_terms = 0;
      f_query = new QueryGroup();
      // This QueryTerm is pointed to by the QueryGroup.
      new QueryTerm (f_query, NULL, NULL);
    }

  // QueryGroupView holds the pointer to the QueryGroup. 
  f_query_view =
    new QueryGroupView (f_query, WXmForm ((WObject &) f_query_area));
#endif

  if (first_time)
    {
//#ifdef UseFJMTF
#if 1
      f_null_terms = 0;

      // QueryGroupView holds the pointer to the QueryGroup. 
      f_query_view = new QueryGroupView (f_query, f_query_area);
#endif
      // Only let pane get small enough to show two query entries.
      // Start with the whole scrolled window, then take out the scrolling
      // area to leave the borders, then add the term height * 2 back in.
      // NOTE: This really needs to use the height of one row.  If this
      // dialog comes up with an existing query at this point, the
      // height will be wrong.  We need to get the height of a single
      // line somehow.  11:59 02/04/93 DJB 
      f_query_area.
	PaneMinimum (f_query_area.Height() -
		     WCore (XtParent(XtParent(*f_query_view))).Height() +
		     (2 * f_query_view->Height()));

      // Take the height of each pane - the min size of each pane.
      // That is the excess height of the dialog.
      // This needs to equal the current height - min height.
      // So subtract it from the current height to get the new min height.
      WXmForm qform (XtParent(f_query_text.Parent()));
#ifdef DEBUG
      printf ("qform height = %d, scrolled height = %d\n",
	      qform.Height(), f_query_area.Height());
      printf ("qform min = %d, scrolled min = %d\n",
	      qform.PaneMinimum(), f_query_area.PaneMinimum());
#endif
      Dimension excess_height = qform.Height() + f_query_area.Height() -
	qform.PaneMinimum() - f_query_area.PaneMinimum();

      ON_DEBUG(printf ("Query Editor excess height = %d\n", excess_height));

      f_shell.MinHeight (f_shell.Height() - excess_height);
      
      first_time = False;
    }

  //g_allow_query_text_change = TRUE;
  f_query_text.Value ((char*)"");
  //g_allow_query_text_change = FALSE;

  // Move the input focus to the term view.
  f_query_area.InitialFocus (*f_query_view);
  //  f_query_view->traverse_here();

  display();
}


// /////////////////////////////////////////////////////////////////
// create_ui - create the query editor user interface
// /////////////////////////////////////////////////////////////////

#define AM WAutoManage

void
QueryEditor::create_ui()
{
  Wait_Cursor bob;

  XmStringLocalized mtfstring;
  String	    string;
  KeySym	    mnemonic;

  f_shell = WTopLevelShell(window_system().toplevel(), WPopup, "query_editor");
  window_system().register_shell (&f_shell);

  string = CATGETS(Set_QueryEditor, 1, "Dtinfo: Query Editor");
  XtVaSetValues((Widget)f_shell, XmNtitle, string, NULL);

  // Main form and menu bar. 
  WXmForm form                      (f_shell,    "form"                );
  WXmMenuBar menu_bar               (form,       (char*)"menu_bar"     );

  WXmPulldownMenu scope_menu        (form,       (char*)"scope_menu"   );
  Arg args[1];
  int n = 0;
  XtSetArg(args[n], XmNsubMenuId, (Widget) scope_menu); n++;
  f_scope_option = WXmOptionMenu    (form,       (char*)"scope_option", AM, args, n);

  mtfstring = CATGETS(Set_AgentLabel, 212, "Scope Name");
  XtVaSetValues(f_scope_option, XmNlabelString, (XmString)mtfstring, NULL);

  // Menu definitions.   how about using AddPushButton (name, obj, fun)??
  WXmCascadeButton edit_cascade     (menu_bar,   "edit",             AM);
  WXmPulldownMenu edit_menu         (menu_bar,   (char*)"edit_menu"    );
  f_cut_btn = WXmPushButton         (edit_menu,  "cut",              AM);
  f_copy_btn = WXmPushButton        (edit_menu,  "copy",             AM);
  f_paste_btn = WXmPushButton       (edit_menu,  "paste",            AM);
  f_paste_btn = WXmPushButton       (edit_menu,  "new_term",         AM);
  WXmSeparator group_sep            (edit_menu,  "group_sep",        AM);
  f_group_btn = WXmPushButton       (edit_menu,  "group",            AM);
  f_ungroup_btn = WXmPushButton     (edit_menu,  "ungroup",          AM);
  WXmSeparator undo_sep             (edit_menu,  "undo_sep",         AM);
  f_undo_btn = WXmPushButton        (edit_menu,  "undo",             AM);
  f_redo_btn = WXmPushButton        (edit_menu,  "redo",             AM);

  mtfstring =  CATGETS(Set_AgentLabel, 16, "Edit");
  mnemonic  = *CATGETS(Set_AgentLabel, 17, "E");
  XtVaSetValues(edit_cascade, XmNlabelString, (XmString)mtfstring,
			      XmNmnemonic, mnemonic, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 224, "Cut");
  XtVaSetValues(f_cut_btn, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 18, "Copy");
  XtVaSetValues(f_copy_btn, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 225, "Paste");
  XtVaSetValues(f_paste_btn, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 226, "Group");
  XtVaSetValues(f_group_btn, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 227, "Ungroup");
  XtVaSetValues(f_ungroup_btn, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 228, "Undo");
  XtVaSetValues(f_undo_btn, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 229, "Redo");
  XtVaSetValues(f_redo_btn, XmNlabelString, (XmString)mtfstring, NULL);

  // Button area at the bottom 
  WXmForm hform                     (form,       "hform"               );
  f_hist_prev = WXmArrowButton      (hform,      "hist_prev",        AM);
  f_hist_next = WXmArrowButton      (hform,      "hist_next",        AM);
  WXmLabel history                  (hform,      "history",          AM);
  f_search_btn = WXmPushButton      (form,       "search",           AM);
  WXmPushButton cancel              (form,       "cancel",           AM);
  WXmPushButton clear               (form,       "clear",            AM);
  WXmPushButton scope		    (form,	 "scope",	     AM);
  WXmPushButton help                (form,       "help",             AM);
  WXmSeparator separator            (form,       "separator",        AM);

  mtfstring =  CATGETS(Set_AgentLabel, 92, "History");
  XtVaSetValues(history, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 102, "Search");
  XtVaSetValues(f_search_btn, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 231, "Clear All");
  XtVaSetValues(clear, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 46, "Scope Editor");
  XtVaSetValues(scope, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 12, "Close");
  XtVaSetValues(cancel, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring =  CATGETS(Set_AgentLabel, 48, "Help");
  XtVaSetValues(help, XmNlabelString, (XmString)mtfstring, NULL);

  help_agent().add_activate_help (help, (char*)"query_editor_help");

  // Main "work" area 
  WXmPanedWindow pane               (form,       "pane"                );
  WXmForm qform                     (pane,       "qform"               );
  WXmLabel qlabel                   (qform,      "qlabel",           AM);

  mtfstring =  CATGETS(Set_AgentLabel, 230, "Query");
  XtVaSetValues(qlabel, XmNlabelString, (XmString)mtfstring, NULL);
  
  f_query_text = WXmScrolledText    (qform,      (char*)"qtext",     AM);
//  f_query_text.SetEditable(False);
  f_query_area = WXmScrolledWindow  (pane,       "query_area",       AM);
  XtUnmanageChild (f_query_area.HorizontalScrollBar());

  edit_cascade.SubMenuId (edit_menu);

  // Callbacks
  ON_ACTIVATE (f_search_btn, search_activate);
  ON_ACTIVATE (cancel, cancel);
  ON_ACTIVATE (clear, clear);
  ON_ACTIVATE (scope, scope);

//  f_query_text.SetFocusCallback (this,
//			    (WWL_FUN) &QueryEditor::modify_verify);
//  f_query_text.SetModifyVerifyCallback (this,
//			    (WWL_FUN) &QueryEditor::modify_verify);

  // Set minimum sizes.
  qform.Manage();
  pane.Manage();
  //  hform.Manage();
  form.DefaultButton (f_search_btn);
  form.ShadowThickness (0);

  form.InitialFocus (pane);
  pane.InitialFocus (f_query_area);
  qform.PaneMinimum (f_query_text.Height());
  form.Manage();

  f_shell.Realize();
  f_shell.MinWidth (f_shell.Width());
  f_shell.MinHeight (f_shell.Height());
  ON_DEBUG(printf ("Query for height = %d\n", qform.Height()));
  //cerr << "Query for height = " << qform.Height() << endl;

//#ifndef UseFJMTF
#if 1
  // Swap the entries in the pane around.  Have to do this now
  // to get initial sizes right.
  qform.PositionIndex (1);
#endif

  fill_menu();

  UAS_SearchScope::request ((UAS_Receiver<ScopeCreated> *) this);
  UAS_SearchScope::request ((UAS_Receiver<ScopeDeleted> *) this);
  UAS_SearchScope::request ((UAS_Receiver<ScopeRenamed> *) this);

  search_scope_mgr().option_menu(this);
}


// /////////////////////////////////////////////////////////////////
// search_activate
// /////////////////////////////////////////////////////////////////

void
QueryEditor::search_activate()
{
  Wait_Cursor bob;

  // Need to duplicate string returned, because it is in static data space. 
  char *human_readable =
    strdup (f_query_view->query_group()->
	    generate_query (QueryGroup::QUERY_TYPE_INFIX));

#ifdef DEBUG
  fprintf(stderr, "(DEBUG) human_readable=\"%s\"\n", human_readable);
#endif

  UAS_SearchScope *scope =
    (UAS_SearchScope *) WXmPushButton (f_scope_option.MenuHistory()).UserData();

   mtry {
      // Search manager owns query from this point on.  Don't delete it here. 
      search_mgr().parse_and_search (human_readable, scope);
   }
   mcatch (demoException &, demo) {
      message_mgr().demo_failure(demo);
   }
   end_try;

   free(human_readable);

}


// /////////////////////////////////////////////////////////////////
// cancel - close the window
// /////////////////////////////////////////////////////////////////

void
QueryEditor::cancel()
{
  f_shell.Popdown();
}


// /////////////////////////////////////////////////////////////////
// empty term tracking routines
// /////////////////////////////////////////////////////////////////

void
QueryEditor::increment_null_terms()
{
  if (f_null_terms == 0)
    f_search_btn.SetSensitive (False);
  f_null_terms++;
  //  printf ("QueryEditor setting null_terms to %d\n", f_null_terms); 
}

void
QueryEditor::decrement_null_terms()
{
  f_null_terms--;
  //  printf ("QueryEditor setting null_terms to %d\n", f_null_terms); 
  if (f_null_terms == 0)
    f_search_btn.SetSensitive (True);
}


// /////////////////////////////////////////////////////////////////
// clear - clear the current query
// /////////////////////////////////////////////////////////////////

void
QueryEditor::clear()
{
  edit_query (NULL);
}


// /////////////////////////////////////////////////////////////////////////
// scope - activate search scope dialog
// /////////////////////////////////////////////////////////////////////////
void
QueryEditor::scope()
{
  search_scope_mgr().display();
}


// /////////////////////////////////////////////////////////////////
// query_changed - regenerate the text version
// /////////////////////////////////////////////////////////////////

void
QueryEditor::query_changed()
{
  // Need to duplicate string returned, because it is in static data space. 
#ifdef UseQSearch
  const char *human_readable =
    f_query_view->query_group()->generate_readable();
#else
  const char *human_readable =
    f_query_view->query_group()->
      generate_query (QueryGroup::QUERY_TYPE_INFIX);
#endif

  //g_allow_query_text_change = TRUE;
  f_query_text.Value ((char *) human_readable);
  //g_allow_query_text_change = FALSE;
}


// /////////////////////////////////////////////////////////////////
// modify_verify
// /////////////////////////////////////////////////////////////////


void
QueryEditor::modify_verify (WCallback *wcb)
{
//  if (g_allow_query_text_change)
//    return;

//  XmTextVerifyPtr tvp = (XmTextVerifyPtr) wcb->CallData();

  // Editing never allowed. 
//  tvp->doit = False;

//  message_mgr().warning_dialog (
//	(char*)UAS_String(CATGETS(Set_Messages, 47,
//				"This is a display-only field.")));
}


// /////////////////////////////////////////////////////////////////
// fill_menu - initially file in the scope option menu
// /////////////////////////////////////////////////////////////////

void
QueryEditor::fill_menu()
{
  int position = 0;
  // Create push buttons for each scope.
  xList<UAS_SearchScope *> &scope_list = search_scope_mgr().scope_list();
  List_Iterator<UAS_SearchScope *> s (scope_list);
  bool old_read_only = TRUE;

  for (; s != 0; s++)
    {
      // Add a separator when they change from read only to changable. 
      if (old_read_only != s.item()->read_only())
	{
	  DECLM (WXmSeparator, sep1, f_scope_option.SubMenuId(), "separator");
          sep1.PositionIndex (position++);
	  old_read_only = FALSE;
	}
      DECLM (WXmPushButton, scope,f_scope_option.SubMenuId(),s.item()->name());
      scope.UserData (s.item());
      scope.PositionIndex (position++);
    }

  // Do not want first item (Current Section)

#ifdef __osf__
#if 1
  Widget w = f_scope_option.SubMenuId();
  WComposite temp_composite(w);
  WXmPushButton(temp_composite.Children()[0]).Unmanage();
#else
  int i = 0;
  Widget w = f_scope_option.SubMenuId();;
  Arg args[1];
  WidgetList wl;
  
  XtSetArg(args[i], XmNchildren, &wl); i++;
  XtGetValues(w, args, i);
  XtUnmanageChild(wl[0]);
#endif
#else
  WXmPushButton(WComposite(f_scope_option.SubMenuId()).Children()[0]).Unmanage();
#endif

  f_scope_option.
      MenuHistory (WComposite(f_scope_option.SubMenuId()).Children()[1]);
  
  //UAS_SearchScope::request ((UAS_Receiver<ScopeCreated> *) this);
  //UAS_SearchScope::request ((UAS_Receiver<ScopeDeleted> *) this);
  //UAS_SearchScope::request ((UAS_Receiver<ScopeRenamed> *) this);
//
  //search_scope_mgr().option_menu(this);
}


// /////////////////////////////////////////////////////////////////
// receive* - handle updates to the list of scopes
// /////////////////////////////////////////////////////////////////

void
QueryEditor::receive (ScopeCreated &msg, void* /*client_data*/)
{
  int position = 0;
  const char *scope_name = msg.f_search_scope->name();
  xList<UAS_SearchScope *> &scope_list = search_scope_mgr().scope_list();
  List_Iterator<UAS_SearchScope *> s (scope_list);
  bool need_sep = TRUE;

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
    UAS_SearchScope *scope;
    for (; s != 0; s++, position++)
    {
      scope = s.item();
      if (scope->read_only() != need_sep)
      {
        position++;  // skip separator
        need_sep = FALSE;
      }
      if (scope->read_only())
	continue;
      // Find the first item that the new entry belongs after.
      ON_DEBUG (printf ("Scope Edit strcmp to <%s>\n", s.item()->name()));
      if (strcmp (scope_name, scope->name()) < 0)
	break;
    }
  }

  ON_DEBUG (printf ("Final SE position = %d\n", position));

  // Add a separator if this is the first user-defined entry. 
  if (need_sep == TRUE)
  {
    DECLM (WXmSeparator, separator, f_scope_option.SubMenuId(), "separator");
    separator.PositionIndex (position);
    position++;
  }

  // Create the new toggle button. 
  DECLM (WXmPushButton, scope, f_scope_option.SubMenuId(), scope_name);
  scope.PositionIndex (position);
  scope.UserData (msg.f_search_scope);
}


void
QueryEditor::receive (ScopeDeleted &msg, void* /*client_data*/)
{
  // find the associated button and nuke it
  ON_DEBUG (puts ("QueryEdit: handling delete message"));
  // First find renamed button in our list.
  WidgetList kids = WComposite(f_scope_option.SubMenuId()).Children();
  int num_kids = WComposite(f_scope_option.SubMenuId()).NumChildren();
  int separator_pos = -1;
  int i;
  for (i = 0; i < num_kids; i++)
    {
      if (XmIsSeparator (kids[i]))
	separator_pos = i + 1;
      if (msg.f_search_scope ==
	  ((UAS_SearchScope *) WXmPushButton (kids[i]).UserData()))
	break;
    }

  // It had better be in the list! 
  Xassert (i != num_kids);
  ON_DEBUG (printf ("  widget #%d is the button\n", i));

  // if it is selected, select first w/ callback called
  if (kids[i] == f_scope_option.MenuHistory())
    f_scope_option.MenuHistory (kids[1]);
  XtDestroyWidget (kids[i]);

  ON_DEBUG (printf ("QueryEditor: sep pos = %d, kids = %d (%d)\n",
		    separator_pos, num_kids, num_kids - 1));
  if (separator_pos == num_kids - 1)
    {
      ON_DEBUG (puts ("   destroying separator"));
      XtDestroyWidget (kids[separator_pos-1]);
    }
}


void
QueryEditor::receive (ScopeRenamed &msg, void* /*client_data*/)
{
  ON_DEBUG (puts ("QueryEdit: handling rename message"));
  // First find renamed button in our list.
  WidgetList kids = WComposite(f_scope_option.SubMenuId()).Children();
  int num_kids = WComposite(f_scope_option.SubMenuId()).NumChildren();
  int i;
  for (i = 0; i < num_kids; i++)
    {
      if (msg.f_search_scope ==
	  ((UAS_SearchScope *) WXmPushButton (kids[i]).UserData()))
	break;
    }

  // It had better be in the list! 
  Xassert (i != num_kids);
  ON_DEBUG (printf ("QueryEdit: widget #%d is the button\n", i));

  // Now find the new insertion position in the list.
  int position = 0;
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
      ON_DEBUG (printf ("QueryEdit: strcmp <%s>\n", s.item()->name()));
      if (strcmp (msg.f_search_scope->name(), s.item()->name()) < 0)
	break;
    }

  ON_DEBUG (printf ("QueryEdit: Rename position = %d\n", position));
  WXmPushButton scope_btn (kids[i]);
  scope_btn.LabelString (msg.f_search_scope->name());
  scope_btn.PositionIndex (position);
}

void
QueryEditor::update_option_menu()
{
  WidgetList kids = WComposite(f_scope_option.SubMenuId()).Children();
  int num_kids = WComposite(f_scope_option.SubMenuId()).NumChildren();

  // destroy all toggle buttons in menu
  for (int i = 0; i < num_kids; i++)
  {
    XtUnmanageChild (kids[i]);
    XtDestroyWidget (kids[i]);
  }

  fill_menu();
}
