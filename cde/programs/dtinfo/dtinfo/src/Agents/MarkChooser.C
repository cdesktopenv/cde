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
 * $XConsortium: MarkChooser.cc /main/8 1996/06/14 14:10:02 cde-hal $
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

#define C_WindowSystem
#define L_Other

#define C_MarkChooser
#define C_MarkCanvas
#define C_HelpAgent
#define L_Agents

#define C_MessageMgr
#define L_Managers

#include <Prelude.h>

#include "Managers/CatMgr.hh"
#include "Other/XmStringLocalized.hh"

#include "Registration.hh"

#define CLASS MarkChooser
#include "create_macros.hh"

#include <WWL/WXmDialogShell.h>
#include <WWL/WXmSeparator.h>

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

MarkChooser::MarkChooser (Widget parent, xList<MarkCanvas *> &marks,
			  const char *title_key, const char *ok_key)
: f_selected_item (-1),
  f_mark_list (&marks)
{
    create_ui (parent, title_key, ok_key);
    update_list();
}


MarkChooser::~MarkChooser()
{
  f_form.Unmanage();
  /*
     have to remove the callbacks first, otherwise the
     XtDestroyWidget() call will remove the callbacks from
     the widgets, then our destructor will have memory violations
     trying to remove them again - brad
     */
  removeCallbacks();		
  XtDestroyWidget (XtParent (f_form));
}


// /////////////////////////////////////////////////////////////////
// create_ui - create the user interface
// /////////////////////////////////////////////////////////////////

void
MarkChooser::create_ui (Widget parent,
			const char *title_key, const char *ok_key)
{
  XmStringLocalized mtfstring;

  WXmDialogShell shell (parent, True, "mark_chooser");
  window_system().register_full_modal_shell (&shell);

  ASSN  (WXmForm,         f_form,   shell,     "form");
  ASSNM (WXmPushButton,   f_ok,     f_form,    "ok");
  DECLM (WXmPushButton,   cancel,   f_form,    "cancel");
#if 0
  // do not provide help on full-modal window - 10/27/94 kamiya
  DECLM (WXmPushButton,   help,     f_form,    "help");
#endif
  DECLM (WXmSeparator,    sep,      f_form,    "separator");

  Widget list = XmCreateScrolledList (f_form, (char*)"list", NULL, 0);
  f_list = WXmList (list);
  f_list.Manage();

  XtVaSetValues(f_form, XmNdialogTitle, (XmString)XmStringLocalized((char*)title_key), NULL);
  XtVaSetValues(f_ok, XmNlabelString, (XmString)XmStringLocalized((char*)ok_key), NULL);

  mtfstring = CATGETS(Set_AgentLabel, 162, "Cancel");
  XtVaSetValues(cancel, XmNlabelString, (XmString)mtfstring, NULL);

  SET_CALLBACK (f_list,SingleSelection,select);
  SET_CALLBACK (f_list,BrowseSelection,select);
  SET_CALLBACK (f_list,DefaultAction,ok);
  ON_ACTIVATE (f_ok,ok);
  ON_ACTIVATE (cancel,cancel);
#if 0
  // do not provide help on full-modal window - 10/27/94 kamiya
  help_agent().add_activate_help (help, "mark_chooser");
#endif

  f_form.DefaultButton (f_ok);
  f_form.ShadowThickness (0);
  f_form.Manage();

  // Set the min size based on the current size.
  shell.Realize();
  shell.MinWidth (shell.Width());
  shell.MinHeight (shell.Height());
}


// /////////////////////////////////////////////////////////////////
// update_list - fill in the list with mark items
// /////////////////////////////////////////////////////////////////

void
MarkChooser::update_list()
{
  int length = f_mark_list->length();
  ON_DEBUG (printf ("Chooser list length = %d\n", length));
  XmStringTable table = new XmString [length];

  List_Iterator<MarkCanvas *> m (f_mark_list);

  int i = 0;
  while (m)
    {
      Xassert (i < length);
      table[i] = XmStringCreateLocalized((String)m.item()->mark_ptr()->name());
      m++; i++;
    }
  Xassert (i == length);

  f_list.Set (WArgList (XmNitemCount, (XtArgVal) i,
			XmNitems, table,
			NULL));

  for (i = 0; i < length; i++)
    XmStringFree (table[i]);
  delete [] table;
}


// /////////////////////////////////////////////////////////////////
// get_choice - return the user's choice from the list
// /////////////////////////////////////////////////////////////////

extern int g_blew_away_marks_too_bad_you_lose_dts_14590;

MarkCanvas *
MarkChooser::get_choice()
{
  g_blew_away_marks_too_bad_you_lose_dts_14590 = 0 ;

  f_form.Manage();

  f_done = FALSE;
  XtAppContext app_context = window_system().app_context();
  XEvent event;

  while (!f_done)
    {
      XtAppNextEvent (app_context, &event);
      XtDispatchEvent (&event);
    }

  if (g_blew_away_marks_too_bad_you_lose_dts_14590)
    {
      ON_DEBUG(cerr << "blew away marks" << endl);
      return NULL ;
    }

  if (f_selected_item > 0)
    {
      List_Iterator<MarkCanvas *> m (f_mark_list);
      int i = 1;
      while (i < f_selected_item)
	i++, m++;
      return (m.item());
    }
  else
    {
      return (NULL);
    }
}

// /////////////////////////////////////////////////////////////////
// select - process a list item selection
// /////////////////////////////////////////////////////////////////

void
MarkChooser::select (WCallback *wcb)
{
  CALL_DATA (XmListCallbackStruct, lcs);

  f_ok.SetSensitive (True);
  f_selected_item = lcs->item_position;
}


// /////////////////////////////////////////////////////////////////
// ok, cancel, help - other callbacks
// /////////////////////////////////////////////////////////////////


void
MarkChooser::ok()
{
  f_done = TRUE;
}

void
MarkChooser::cancel()
{
  f_selected_item = -1;
  f_done = TRUE;
}
