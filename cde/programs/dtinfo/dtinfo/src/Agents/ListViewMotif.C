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
/*	Copyright (c) 1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved				*/

/*
 * $XConsortium: ListViewMotif.C /main/9 1996/10/15 17:35:55 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
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

#include <WWL/WTopLevelShell.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmList.h>
#include <WWL/WXmForm.h>
#include <WWL/WXmLabelGadget.h>
#include <WWL/WXmSeparator.h>

#include <stdio.h>

#define C_WindowSystem
#define L_Other


#define C_TOC_Element
#define C_List
#define L_Basic

#define C_HelpAgent
#define C_ListView
#define L_Agents

#include "Prelude.h"

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Registration.hh"

// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

ListView::~ListView()
{
  if (f_shell != NULL)
    {
      f_shell->Destroy();
      delete f_shell;
      delete f_list;
      delete f_wm_delete_callback;
    }
}


// /////////////////////////////////////////////////////////////////
// list - set/get the list
// /////////////////////////////////////////////////////////////////

void
ListView::list (List *list)
{
  if (f_the_list != NULL)
    f_the_list->
      RemoveDependent ((notify_handler_t) &ListView::list_changed,
			List::APPENDED);
  f_the_list = list;
  f_the_list->AddDependent ((notify_handler_t) &ListView::list_changed,
			     List::APPENDED);
  display_list();
}

const List *
ListView::list()
{
  return (f_the_list);
}

// /////////////////////////////////////////////////////////////////
// display - display it on the screen
// /////////////////////////////////////////////////////////////////

void
ListView::display()
{
  if (f_shell == NULL)
    {
      create_ui_objects();
      display_list();
    }

  f_shell->Popup();
  f_shell->DeIconify();
}


// /////////////////////////////////////////////////////////////////
// create_ui_objects
// /////////////////////////////////////////////////////////////////

void
ListView::create_ui_objects()
{
  /* -------- Create ui components. -------- */
  f_shell = new WTopLevelShell (toplevel(), WPopup, f_name);
  window_system().register_shell (f_shell);
  WXmForm form (*f_shell, "form");

  /* -------- allow child to be creative -------- */
  child_create_ui_pre(form);


  WXmPushButton close (form, "close", WAutoManage);
  WXmPushButton help (form, "help", WAutoManage);

  XtVaSetValues(close, XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 12, "Close")), NULL);
  XtVaSetValues(help, XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 48, "Help")), NULL);

  // 7/30/93 rtp - bear with this hack, it's a little bogus
  int help_namelen = strlen(f_name) + strlen("_help") + 1;
  char *help_name = new char[help_namelen];
  snprintf(help_name, help_namelen, "%s%s", f_name, "_help");
  // What i've done is appended '_help' to the widget name and added
  // a help callback to the help button that will reference this name
  help_agent().add_activate_help (help, help_name);

  f_activate = WXmPushButton (form, "activate", WAutoManage);
  XtVaSetValues(f_activate, XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 165, "Display")), NULL);
  //  WXmLabelGadget label (form, "label", WAutoManage);
  WXmSeparator separator (form, "separator", WAutoManage);
  // NOTE: Fix WWL to have a scrolled list object!
  Widget scrolled_list = XmCreateScrolledList (form, (char*)"list", NULL, 0);
  f_list = new WXmList (scrolled_list);
  f_list->Manage();

  XmFontList defaultList;
  {
    XmFontList font;
    XtVaGetValues(scrolled_list, XmNfontList, &font, NULL);
    defaultList = XmFontListCopy(font);
  }
  if (window_system().dtinfo_space_font())
    defaultList = XmFontListAppendEntry(defaultList,
                                        window_system().dtinfo_space_font());
  XtVaSetValues(scrolled_list, XmNfontList, defaultList, NULL);

  /* -------- let child add anything else -------- */
  child_create_ui_post(form);

  /* -------- Callbacks -------- */
  f_wm_delete_callback =
    new WCallback (*f_shell, window_system().WM_DELETE_WINDOW(),
		   this, (WWL_FUN) &ListView::close_window);
  close.SetActivateCallback (this, (WWL_FUN) &ListView::close_window);
  f_list->SetSingleSelectionCallback (this, (WWL_FUN) &ListView::select);
  f_list->SetBrowseSelectionCallback (this, (WWL_FUN) &ListView::select);
  f_list->SetDefaultActionCallback (this, (WWL_FUN) &ListView::activate);
  f_activate.SetActivateCallback (this, (WWL_FUN) &ListView::activate);

  /* -------- Finalize -------- */
  form.DefaultButton (f_activate);
  form.ShadowThickness (0);  // get rid of border turds (motif bug)
  form.Manage();
}
  

// /////////////////////////////////////////////////////////////////
// display_list - put the list in the Motif list widget
// /////////////////////////////////////////////////////////////////

void
ListView::display_list()
{
  // NOTE: What does it mean to display a NULL list? 6/22/92 djb 
  if (f_shell == NULL || f_the_list == NULL)
    return;

  unsigned int i;

  // NOTE: There should be a wwl object for string tables!! 6/19/92 djb
  XmStringTable st = (XmStringTable)
    malloc (sizeof (XmStringTable) * f_the_list->length());

  for (i = 0; i < f_the_list->length(); i++)
    st[i] = display_value((*f_the_list)[i]);

  // Basically bogus, but best fix for now.  18:32 10-May-94 DJB
  f_list->DeselectAllItems();
  WArgList args;
  f_list->Items (st, args);
  f_list->ItemCount (f_the_list->length(), args);
  f_list->Set (args);

  for (i = 0; i < f_the_list->length(); i++)
    XmStringFree(st[i]);

  free ((char *) st);
}


// /////////////////////////////////////////////////////////////////
// select
// /////////////////////////////////////////////////////////////////

void
ListView::select (WCallback *wcb)
{
  XmListCallbackStruct *lcs = (XmListCallbackStruct *) wcb->CallData();

  f_activate.SetSensitive (True);
  f_selected_item = lcs->item_position - 1;
}


// /////////////////////////////////////////////////////////////////
// activate
// /////////////////////////////////////////////////////////////////

void
ListView::activate (WCallback *)
{
  notify (ENTRY_ACTIVATE, (void *)(size_t) f_selected_item);
}
  

// /////////////////////////////////////////////////////////////////
// close_window
// /////////////////////////////////////////////////////////////////

void
ListView::close_window (WCallback *)
{
  f_shell->Popdown();
}


// /////////////////////////////////////////////////////////////////
// list_changed - the list was altered somehow
// /////////////////////////////////////////////////////////////////

void
ListView::list_changed (FolioObject *, u_int, void *, void *)
{
  // NOTE: simplistic for now 6/23/92 djb
  display_list();
}

XmString
ListView::display_value(FolioObject *object)
{
  return XmStringCreateLocalized((char*)object->display_as());
}

void
ListView::child_create_ui_pre(WXmForm &)
{
  /* -------- empty -------- */
}

void
ListView::child_create_ui_post(WXmForm &)
{
  /* -------- empty -------- */
}
