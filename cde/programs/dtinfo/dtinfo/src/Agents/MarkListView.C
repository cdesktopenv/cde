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
 * $XConsortium: MarkListView.C /main/8 1996/09/13 20:45:08 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
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

#define C_MarkListView
#define C_HelpAgent
#define L_Agents

#define C_WindowSystem
#define L_Other

#define C_MarkMgr
#define C_MessageMgr
#define L_Managers

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include <Prelude.h>

#include "Registration.hh"

#include <WWL/WXmForm.h>
#include <WWL/WXmSeparator.h>

#define CLASS MarkListView
#include "create_macros.hh"

#include "utility/mmdb_exception.h"

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

MarkListView::MarkListView()
: f_shell (NULL),
  f_popped_up (FALSE)
{
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

MarkListView::~MarkListView()
{
  if (f_shell != 0)
    f_shell.Destroy();
  free_mark_list();
}


// /////////////////////////////////////////////////////////////////
// display - display the dialog and update the mark list
// /////////////////////////////////////////////////////////////////

void
MarkListView::display()
{
  Wait_Cursor bob;
  if (f_shell == 0)
    create_ui();

  if (!f_popped_up)
    {
      refresh_mark_list();
      f_popped_up = TRUE;
    }

  f_shell.Popup();
  f_shell.DeIconify();
}

// /////////////////////////////////////////////////////////////////
// create_ui
// /////////////////////////////////////////////////////////////////

void
MarkListView::create_ui()
{
  XmStringLocalized mtfstring;
  String	    string;

  f_shell = WTopLevelShell (window_system().toplevel(), WPopup, "mark_list");
  window_system().register_shell(&f_shell);

  string = CATGETS(Set_MarkList, 1, "Dtinfo: User Marks List");
  XtVaSetValues((Widget)f_shell, XmNtitle, string, NULL);

  DECL  (WXmForm,         form,         f_shell,      "form");
  ASSNM (WXmPushButton,   f_view,       form,         "view");
  ASSNM (WXmPushButton,   f_edit,       form,         "edit");
  ASSNM (WXmPushButton,   f_delete,     form,         "delete"); // ross bug #14441
  DECLM (WXmPushButton,   close,        form,         "close");
  DECLM (WXmPushButton,   help,         form,         "help");
  DECLM (WXmSeparator,    sep,          form,         "separator");
  Widget list = XmCreateScrolledList (form, (char*)"list", NULL, 0);
  f_list = WXmList (list);
  f_list.Manage();

  mtfstring = CATGETS(Set_AgentLabel, 165, "Display");
  XtVaSetValues(f_view, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 16, "Edit");
  XtVaSetValues(f_edit, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 183, "Delete");
  XtVaSetValues(f_delete, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 12, "Close");
  XtVaSetValues(close, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 48, "Help");
  XtVaSetValues(help, XmNlabelString, (XmString)mtfstring, NULL);

  SET_CALLBACK (f_list,SingleSelection,select);
  SET_CALLBACK (f_list,BrowseSelection,select);
  SET_CALLBACK (f_list,DefaultAction,view);
  ON_ACTIVATE (f_view,view);
  ON_ACTIVATE (f_edit,edit);
  ON_ACTIVATE (f_delete,remove);
  ON_ACTIVATE (close,close);
  help_agent().add_activate_help (help, (char*)"mark_list_help");

  SET_CALLBACK (f_shell,Popdown,popdown);

  form.DefaultButton (f_view);
  form.ShadowThickness (0);
  form.Manage();

  f_shell.Realize();
  f_shell.MinWidth (f_shell.Width());
  f_shell.MinHeight (f_shell.Height());

  MarkMgr::request (this);
  Mark::request ((UAS_Receiver<MarkDeleted> *) this);
  Mark::request ((UAS_Receiver<MarkChanged> *) this);
}


// /////////////////////////////////////////////////////////////////
// get_list - get the list of user marks
// /////////////////////////////////////////////////////////////////

void
MarkListView::refresh_mark_list()
{
  mark_mgr().get_all_marks (f_mark_list);

  unsigned int length = f_mark_list.length();
  // Convert the list to XmString values.
  XmStringTable table = new XmString[length];
  
  List_Iterator<UAS_Pointer<Mark> > m (f_mark_list);

  int i = 0;
  while (m)
    {
      table[i] = XmStringCreateLocalized ((String) m.item()->name());
      m++;
      i++;
    }

  // Update the list to display those items. 
  f_list.Set (WArgList (XmNitemCount, (XtArgVal) length,
			XmNitems, table,
			NULL));

  // Free the strings since the list widget keeps its own copy. 
  while (i)
    XmStringFree (table[--i]);
  delete [] table;
}


// /////////////////////////////////////////////////////////////////
// free_list - release the mark ptrs in the list
// /////////////////////////////////////////////////////////////////

void
MarkListView::free_mark_list()
{
  List_Iterator<UAS_Pointer<Mark> > m (f_mark_list);

  while (m)
    f_mark_list.remove (m);
}


// /////////////////////////////////////////////////////////////////
// popdown
// /////////////////////////////////////////////////////////////////

void
MarkListView::popdown()
{
  f_popped_up = FALSE;
  // Forget about everything while the dialog isn't visible. 
  free_mark_list();
  f_list.DeleteAllItems();
  // Current we forget about the selected item.
  // This is fixable by remembering the selected mark pointer. 
  f_selected_item = 0;
  f_view.SetSensitive (False);
  f_edit.SetSensitive (False);
  f_delete.SetSensitive (False);	// ross for bug #14441
}

// /////////////////////////////////////////////////////////////////
// select - handle item selection
// /////////////////////////////////////////////////////////////////

void
MarkListView::select (WCallback *wcb)
{
  CALL_DATA (XmListCallbackStruct,lcs);

  f_selected_item = lcs->item_position;

  ON_DEBUG (printf ("Selected item #%d\n", f_selected_item);)

  f_view.SetSensitive (True);
  f_edit.SetSensitive (True);
  f_delete.SetSensitive (True);		// ross for bug #14441
}


// /////////////////////////////////////////////////////////////////
// view - view a mark
// /////////////////////////////////////////////////////////////////

void
MarkListView::view()
{
  Wait_Cursor bob_dobbs;

  mtry
    {

      Xassert (f_selected_item > 0);
      Xassert (f_selected_item <= f_mark_list.length());

      List_Iterator<UAS_Pointer<Mark> > m (f_mark_list);
      int i  = 1;

      // Skip ahead to the selected item in the list. 
      while (i < f_selected_item)
	i++, m++;

      // Now `m' points to the selected Mark.
      m.item()->view();
    }
  mcatch(demoException &, demo)
    {
      message_mgr().demo_failure(demo);
    }
  end_try;
}


// /////////////////////////////////////////////////////////////////
// remove - delete a mark
// /////////////////////////////////////////////////////////////////

void
MarkListView::remove()
{
  Xassert (f_selected_item > 0);
  Xassert (f_selected_item <= f_mark_list.length());

  List_Iterator<UAS_Pointer<Mark> > m (f_mark_list);
  int i  = 1;

  // Skip ahead to the selected item in the list. 
  while (i < f_selected_item)
    i++, m++;

  // Now `m' points to the selected Mark.
  m.item()->remove();		// ross bug #14441
}




// /////////////////////////////////////////////////////////////////
// edit - edit a mark
// /////////////////////////////////////////////////////////////////

void
MarkListView::edit()
{
  Xassert (f_selected_item > 0);
  Xassert (f_selected_item <= f_mark_list.length());

  List_Iterator<UAS_Pointer<Mark> > m (f_mark_list);
  int i  = 1;

  // Skip ahead to the selected item in the list. 
  while (i < f_selected_item)
    i++, m++;

  // Now `m' points to the selected Mark.
  m.item()->edit();
}


// /////////////////////////////////////////////////////////////////
// close - popdown the the dialog
// /////////////////////////////////////////////////////////////////

void
MarkListView::close()
{
  f_shell.Popdown();
}


// /////////////////////////////////////////////////////////////////
// receive - handle mark creation/deletion
// /////////////////////////////////////////////////////////////////

void
MarkListView::receive (MarkCreated &msg, void* /*client_data*/)
{
  if (!f_popped_up)
    return;

  f_list.AddItemUnselected (msg.f_mark_ptr->name(), 0);
  f_mark_list.append (msg.f_mark_ptr);
}


void
MarkListView::receive (MarkDeleted &msg, void* /*client_data*/)
{
  if (!f_popped_up)
    return;

  List_Iterator<UAS_Pointer<Mark> > m (f_mark_list);
  int i  = 1;

  // Skip ahead to the deleted item item in the list. 
  while (m && m.item() != msg.f_mark_ptr)
    {
      ON_DEBUG (printf ("Checking <%s> against <%s>\n",
			m.item()->name(), msg.f_mark_ptr->name()));
      i++, m++;
    }

  Xassert (m != 0);

  //
  //  XXXSWM: Need to check if this is the selected item.
  //  if so, we gotta clear f_selected_item and Desensitize
  //  the display and edit buttons.
  //
  if (XmListPosSelected (f_list, i)) {
      f_selected_item = 0;
      f_view.SetSensitive (False);
      f_edit.SetSensitive (False);
      f_delete.SetSensitive (False);
  }

  // Remove it from the list widget and our list. 
  f_list.DeletePos (i);
  f_mark_list.remove (m);
}


void
MarkListView::receive (MarkChanged &msg, void* /*client_data*/)
{
  if (!f_popped_up)
    return;

  List_Iterator<UAS_Pointer<Mark> > m (f_mark_list);
  int i  = 1;

  // Skip ahead to the changed item item in the list. 
  while (m && m.item() != msg.f_mark_ptr)
    {
      ON_DEBUG (printf ("Checking <%s> against <%s>\n",
			m.item()->name(), msg.f_mark_ptr->name()));
      i++, m++;
    }

  Xassert (m != 0);

  // Change the title of the entry, once it is found.
  WXmString title (msg.f_mark_ptr->name());
  XmString  table[1];
  table[0] = title;

  f_list.ReplaceItemsPosUnselected (table, 1, i);
  if (f_selected_item == i)
    f_list.SelectPos (i, False);
}
