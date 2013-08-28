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
/* $TOG: LibraryAgentMotif.C /main/50 1998/07/27 14:20:02 mgreess $ */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1994, 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
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

#include <sys/stat.h>

#include <sstream>
#include <iostream>
using namespace std;

#define C_xList
#define L_Support

#define C_OutlineString
#define C_OutlineElement
#define C_OutlineList
#define C_TOC_Element
#define L_Basic

#define C_PrintPanelAgent
#define C_HelpAgent
#define C_LibraryAgent
#define C_NodeWindowAgent
#define C_NodeViewInfo
#define C_OutlineListView
#define C_AddLibraryAgent
#define C_ScopeMenu
#define C_BookmarkEdit
#define L_Agents

#define C_ListView
#define C_NodeMgr
#define C_MapMgr
#define C_LibraryMgr
#define C_GlobalHistoryMgr
#define C_PrintMgr
#define C_SearchMgr
#define C_SearchScopeMgr
#define C_MessageMgr
#define C_PrefMgr
#define C_EnvMgr
#define C_GraphicsMgr
#define L_Managers


#define USES_OLIAS_FONT

#include "UAS.hh"
#include "Managers/CatMgr.hh"
#include "Other/XmStringLocalized.hh"

#include "Prelude.h"

#include "Registration.hh"

#include <WWL/WTopLevelShell.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/ToggleBG.h>
#include <Xm/CascadeBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>
extern "C" {
#include <Xm/TransferP.h>
}
#include <Dt/Dnd.h>
#include <Tt/tt_c.h>
#include <WWL/WXmPushButtonGadget.h>
#include <WWL/WXmFrame.h>
#include <WWL/WXmCascadeButtonGadget.h>
#include <WWL/WXmCascadeButton.h>

static BitHandle g_handle;
static bool g_ignore_wm_delete;


struct TrackingEntry
{
  TrackingEntry (UAS_Pointer<UAS_Common> &toc, TrackingEntry *child)
    : f_toc (toc), f_child (child) { }

  UAS_Pointer<UAS_Common>    f_toc;
  TrackingEntry  *f_child;
};


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

LibraryAgent::~LibraryAgent()
{
  if (f_shell != NULL)
  {
    //f_shell->Destroy();
    XtDestroyWidget(f_shell);
  }
  delete f_doc_tree_view;
  delete f_wm_delete_callback;
  delete f_scope_menu;
  free_tracking_hierarchy();
}
  

// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::display (OutlineList *f_outline_list)
{
  if (f_shell == NULL)
  {
    create_ui();
  }

  // NOTE: More of the evil hack... 00:20 01/14/93 DJB
  // g_handle is the handle of the list that this one was detached from.
  // We want the new list to have the same state as the new one, so we
  // copy the expanded state bits to the position that will be allocated
  // when the list is created. 
  if (g_handle != 0)
    {
      ON_DEBUG (puts (">> Copying expanded entries for DETACH!"));
      BitHandle handle = library_mgr().library()->get_data_handle();
      f_outline_list->copy_expanded (g_handle, handle);
      f_doc_tree_view->set_list (f_outline_list, handle);
    }
  else
    {
      f_doc_tree_view->set_list (f_outline_list);
    }

  //f_wm_delete_callback =
    //new WCallback (*f_shell, window_system().WM_DELETE_WINDOW(),
		   //this, (WWL_FUN) &LibraryAgent::close_window);

  popup();

  // moved to EnvMgr()--also, don't need to support AutoHelp since
  // only the script sent this message to dtinfo< ie this is not
  // a user option.
  // Display some initial help for newbie users if AutoHelp is on. 
#ifdef EAM
  static bool initial_help = True;
  if (initial_help && pref_mgr().get_boolean (PrefMgr::AutomaticHelp))
    {
      initial_help = False;
      help_agent().display_help ("doc_list_help");
    }
#endif
}

void
LibraryAgent::popup()
{
  f_popped_down = FALSE;
  track_to (node_mgr().last_displayed());
  //f_shell->Popup();
  //f_shell->DeIconify();
  XtPopup(f_shell, XtGrabNone);
  // insure booklist gets raised when asynchronous requests require
  // it in a new workspace...
  //XRaiseWindow (XtDisplay ((Widget)f_shell), XtWindow ((Widget)f_shell));
  XMapRaised (XtDisplay (f_shell), XtWindow (f_shell));
}

// /////////////////////////////////////////////////////////////////
// file Callbacks
// /////////////////////////////////////////////////////////////////

static void
exitCB(Widget, XtPointer client_data, XtPointer)
{
  Widget f_shell = (Widget) client_data;

  if (BookmarkEdit::modified_count() > 0)
    {
      if (! message_mgr().question_dialog(
		(char*)UAS_String(CATGETS(Set_Messages, 5,
			"There are unsaved bookmarks open.\n"
			"Quit without saving them?")),
		f_shell))
	  return;
    }
  
  //XtPopup(f_shell, XtGrabNone);
  //if (XtIsRealized(f_shell)) XMapRaised(XtDisplay(f_shell), XtWindow(f_shell));

  message_mgr().quit_dialog(
      (char*)UAS_String(CATGETS(Set_Messages, 6, "Quit Dtinfo?")), f_shell);
}

// /////////////////////////////////////////////////////////////////
// edit Callbacks
// /////////////////////////////////////////////////////////////////

static void
copyCB(Widget, XtPointer client_data, XtPointer)
{
  LibraryAgent *agent = (LibraryAgent *)client_data;
  agent->copy_to_clipbd();
}

// /////////////////////////////////////////////////////////////////
// options Callbacks
// /////////////////////////////////////////////////////////////////

static void
prefsCB(Widget, XtPointer, XtPointer)

{
  pref_mgr().display();
}

static void
tool_bar_toggleCB(Widget, XtPointer client_data, XtPointer call_data)
{
  Widget tool_bar = (Widget) client_data;
  XmToggleButtonCallbackStruct *cdata =
      (XmToggleButtonCallbackStruct *) call_data;

  if (cdata->set)
      XtManageChild(tool_bar);
  else
      XtUnmanageChild(tool_bar);
}

static void
search_area_toggleCB(Widget, XtPointer client_data, XtPointer call_data)
{
  Widget search_area = (Widget) client_data;
  XmToggleButtonCallbackStruct *cdata =
      (XmToggleButtonCallbackStruct *) call_data;

  if (cdata->set)
      XtManageChild(search_area);
  else
      XtUnmanageChild(search_area);
}

// /////////////////////////////////////////////////////////////////
// windows Callbacks
// /////////////////////////////////////////////////////////////////

static void
mark_listCB(Widget, XtPointer, XtPointer)
{
  mark_mgr().display_mark_list();
}

static void
section_historyCB(Widget, XtPointer, XtPointer)
{
  global_history_mgr().display();
}

static void
search_historyCB(Widget, XtPointer, XtPointer)
{
  search_mgr().search_history_list_view().display();
}

static void
query_editorCB(Widget, XtPointer, XtPointer)
{
  search_mgr().display_editor();
}

static void
scope_editorCB(Widget, XtPointer, XtPointer)
{
  search_scope_mgr().display();
}

// /////////////////////////////////////////////////////////////////
// create_ui
// /////////////////////////////////////////////////////////////////

#define ON_ACTIVATE(WOBJ,FUNC) \
  (WOBJ).SetActivateCallback (this, (WWL_FUN) &LibraryAgent::FUNC)

void
LibraryAgent::create_ui()
{
  Widget widget;
  Arg args[5];
  int n;

  String	    string;
  UAS_Pointer<UAS_String> help_text;

#if 0
  f_shell = (WTopLevelShell*)(Widget)
      WTopLevelShell (toplevel(), WPopup, "doc_tree");
#endif

  f_shell = window_system().toplevel();
  window_system().register_shell ((WTopLevelShell*)f_shell);

  // set title of doc_tree
  string = CATGETS(Set_LibraryAgent, 3, "Dtinfo: Book List");
  XtVaSetValues(f_shell, XmNtitle, string, NULL);

  // just use "delete_response" instead
  // add a destroy callback to nuke the agent when widget is destroyed.

  XtAddCallback(f_shell, XmNdestroyCallback, ui_destroyedCB, this);


  Widget mainw = XtCreateWidget("mainw", xmMainWindowWidgetClass,
				f_shell, 0, 0);


  n = 0;
  XtSetArg(args[n], XmNscrolledWindowChildType, XmMENU_BAR); n++;
  Widget menu_bar = XmCreateMenuBar(mainw, (char*)"menu_bar", args, n);

  n = 0;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg(args[n], XmNscrolledWindowChildType, XmCOMMAND_WINDOW); n++;
  Widget tool_bar = XtCreateWidget("tool_bar", xmRowColumnWidgetClass,
				   mainw, args, n);

  n = 0;
  XtSetArg(args[n], XmNscrolledWindowChildType, XmMESSAGE_WINDOW); n++;
  Widget search_area = XtCreateWidget("search_area", xmFormWidgetClass,
				      mainw, args, n);

  // Menus
  Widget fileM = XmCreatePulldownMenu(menu_bar, (char*)"file_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, fileM); n++;
  widget = XtCreateManagedWidget("file", xmCascadeButtonGadgetClass, menu_bar, args, n);
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 1, "File")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 2, ""),
	NULL);
  help_agent().add_help_cb(widget);

  f_view = XtCreateManagedWidget("view",
				 xmPushButtonGadgetClass, fileM, 0, 0);
  XtAddCallback(f_view, XmNactivateCallback, display_nodeCB, this);

  // add labelString to f_view

  XtVaSetValues(f_view,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 4, "View")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 5, ""),
	NULL);

#ifdef AllowDetach
  f_detach = XtCreateManagedWidget("detach",
				   xmPushButtonGadgetClass, fileM, 0, 0);
  XtAddCallback(f_detach, XmNactivateCallback, detach_subtreeCB, this);

  // add labelString to f_detach
  XtVaSetValues(f_detach,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 6, "Detach")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 7, ""),
	NULL);
#endif

  XtCreateManagedWidget("sep1", xmSeparatorGadgetClass, fileM, 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsensitive, FALSE); n++;
  f_print_as = XtCreateManagedWidget("print_as",
				     xmPushButtonGadgetClass, fileM, args, n);
  XtAddCallback(f_print_as, XmNactivateCallback, print_asCB, this);
  // add labelString to f_print_as
  XtVaSetValues(f_print_as,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 10, "Print...")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 9, "P"),
	NULL);

  XtCreateManagedWidget("sep2", xmSeparatorGadgetClass, fileM, 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsensitive, f_close_sensitive); n++;
  f_close = XtCreateManagedWidget("close",
				  xmPushButtonGadgetClass, fileM, args, n);
  XtAddCallback(f_close, XmNactivateCallback, close_windowCB, this);
  // add labelString to f_close
  XtVaSetValues(f_close,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 12, "Close")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 13, ""),
	NULL);

  widget = XtCreateManagedWidget("exit", xmPushButtonGadgetClass, fileM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, exitCB, f_shell);
  // add labelString to "exit"
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 14, "Exit")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 15, ""),
	NULL);

  // edit menu
  Widget editM = XmCreatePulldownMenu(menu_bar, (char*)"edit_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, editM); n++;
  widget = XtCreateManagedWidget("edit", xmCascadeButtonGadgetClass,
				menu_bar, args, n);
  // add labelString to "edit"
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 16, "Edit")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 17, ""),
	NULL);
  help_agent().add_help_cb(widget);

  f_copy = XtCreateManagedWidget("copy", xmPushButtonGadgetClass, editM, 0, 0);
  //XtSetSensitive(f_copy, True);
  XtAddCallback(f_copy, XmNactivateCallback, copyCB, this);
  // add labelString to "copy"
  XtVaSetValues(f_copy,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 18, "Copy")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 19, ""),
	NULL);

  widget = XtCreateManagedWidget("add", xmPushButtonWidgetClass, editM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, add_libraryCB, this);
  // add labelString to "add"
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 20, "Add...")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 21, ""),
	NULL);

  f_remove = XtCreateManagedWidget("remove",
				   xmPushButtonGadgetClass, editM, 0, 0);
  XtAddCallback(f_remove, XmNactivateCallback, remove_libraryCB, this);
  // add labelString to "remove"
  XtVaSetValues(f_remove,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 22, "Remove")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 23, ""),
	NULL);

  // options menu
  Widget optionsM = XmCreatePulldownMenu(menu_bar, (char*)"options_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, optionsM); n++;
  widget = XtCreateManagedWidget("options", xmCascadeButtonGadgetClass,
			menu_bar, args, n);
  // add labelString to "options"
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 24, "Options")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 25, ""),
	NULL);
  help_agent().add_help_cb(widget);

  widget = XtCreateManagedWidget("preferences", xmPushButtonGadgetClass,
			      optionsM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, prefsCB, 0);
  // add labelString to "preferences"
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 26, "Preferences...")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 27, ""),
	NULL);

  XtCreateManagedWidget("sep1", xmSeparatorGadgetClass, optionsM, 0, 0);

  f_auto_track = XtCreateManagedWidget("auto_track",
				       xmToggleButtonGadgetClass,
				       optionsM, 0, 0);
  XtAddCallback(f_auto_track, XmNvalueChangedCallback,
		auto_track_toggleCB, this);
  // add labelString to "auto_track"
  XtVaSetValues(f_auto_track,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 28, "Auto Track")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 29, ""),
	NULL);

  Widget tool_barT = XtCreateManagedWidget("show_tool_bar",
					   xmToggleButtonGadgetClass,
					   optionsM, 0, 0);
  XtAddCallback(tool_barT, XmNvalueChangedCallback,
		tool_bar_toggleCB, tool_bar);
  // add labelString to "show_tool_bar"
  XtVaSetValues(tool_barT,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 30, "Show ToolBar")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 31, ""),
	NULL);

  Widget search_areaT = XtCreateManagedWidget("show_search_area",
					      xmToggleButtonGadgetClass,
					      optionsM, 0, 0);
  XtAddCallback(search_areaT, XmNvalueChangedCallback,
		search_area_toggleCB, search_area);
  // add labelString to "show_search_area"
  XtVaSetValues(search_areaT,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 32, "Show Search Area")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 33, ""),
	NULL);

  // windows menu
  Widget windowsM = XmCreatePulldownMenu(menu_bar, (char*)"windows_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, windowsM); n++;
  widget = XtCreateManagedWidget("windows", xmCascadeButtonGadgetClass,
				menu_bar, args, n);
  // add labelString to "windows"
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 34, "Windows")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 35, ""),
	NULL);
  help_agent().add_help_cb(widget);

  widget = XtCreateManagedWidget("marks_list", xmPushButtonGadgetClass,
			      windowsM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, mark_listCB, 0);
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 40, "Marks List")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 41, ""),
	NULL);

  widget = XtCreateManagedWidget("section_history", xmPushButtonGadgetClass,
			      windowsM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, section_historyCB, 0);
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 38, "Section History")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 39, ""),
	NULL);

  widget = XtCreateManagedWidget("search_history", xmPushButtonGadgetClass,
			      windowsM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, search_historyCB, 0);
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 42, "Search History")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 43, ""),
	NULL);

  widget = XtCreateManagedWidget("query", xmPushButtonGadgetClass,
			      windowsM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, query_editorCB, 0);
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 44, "Query Editor")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 45, ""),
	NULL);

  widget = XtCreateManagedWidget("scope", xmPushButtonGadgetClass,
			      windowsM, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, scope_editorCB, 0);
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 46, "Scope Editor")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 47, ""),
	NULL);

  Widget helpM = XmCreatePulldownMenu(menu_bar, (char*)"help_menu", 0, 0);

  // create help menu
  n = 0;
  XtSetArg(args[n], XmNsubMenuId, helpM); n++;
  widget = XtCreateManagedWidget("help", xmCascadeButtonGadgetClass,
			      menu_bar, args, n);
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 48, "Help")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 49, ""),
	NULL);
  help_agent().add_help_cb(widget);
  n = 0;
  XtSetArg(args[n], XmNmenuHelpWidget, widget); n++;
  XtSetValues(menu_bar, args, n);

#if EAM

  widget = XtCreateManagedWidget("help", xmPushButtonGadgetClass,
                              helpM, 0, 0);
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 50, "On Item...")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 51, ""),
	NULL);

#endif

  Widget on_overview, on_tasks, on_reference;
  Widget on_item, on_help, on_about;

  on_overview = XtCreateManagedWidget("on_overview", xmPushButtonGadgetClass,
			      helpM, 0, 0);
  XtVaSetValues(on_overview,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 257, "Overview")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 258, ""),
	NULL);

  XtCreateManagedWidget("sep", xmSeparatorGadgetClass,
			      helpM, 0, 0);
  on_tasks = XtCreateManagedWidget("on_tasks", xmPushButtonGadgetClass,
			      helpM, 0, 0);
  XtVaSetValues(on_tasks,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 249, "Tasks")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 250, ""),
	NULL);

  on_reference = XtCreateManagedWidget("on_reference", xmPushButtonGadgetClass,
			      helpM, 0, 0);
  XtVaSetValues(on_reference,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 251, "Reference")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 252, ""),
	NULL);
  on_item = XtCreateManagedWidget("on_item", xmPushButtonGadgetClass,
			      helpM, 0, 0);
  XtVaSetValues(on_item,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 50, "On Item...")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 51, ""),
	NULL);
  XtCreateManagedWidget("sep", xmSeparatorGadgetClass,
			      helpM, 0, 0);
  on_help = XtCreateManagedWidget("on_help", xmPushButtonGadgetClass,
			      helpM, 0, 0);
  XtVaSetValues(on_help,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 253, "Using Help")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 254, ""),
	NULL);
  XtCreateManagedWidget("sep", xmSeparatorGadgetClass,
			      helpM, 0, 0);
  on_about = XtCreateManagedWidget("on_about", xmPushButtonGadgetClass,
			      helpM, 0, 0);
  XtVaSetValues(on_about,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 255, "About Information Manager")),
	XmNmnemonic,
	*CATGETS(Set_AgentLabel, 256, ""),
	NULL);

  // WXmPushButtonGadget tmp_pbg(tmp);
  // help_agent().add_activate_help (tmp_pbg, "doc_list_help");
  WXmPushButtonGadget on_overview_pbg(on_overview);
  help_agent().add_activate_help (on_overview, (char*)"on_overview");
  WXmPushButtonGadget on_tasks_pbg(on_tasks);
  help_agent().add_activate_help (on_tasks, (char*)"on_tasks");
  WXmPushButtonGadget on_reference_pbg(on_reference);
  help_agent().add_activate_help (on_reference, (char*)"on_reference");
  WXmPushButtonGadget on_item_pbg(on_item);
  help_agent().add_context_help (on_item);
  WXmPushButtonGadget on_help_pbg(on_help);
  help_agent().add_activate_help (on_help,  (char*)"on_help");
  WXmPushButtonGadget on_about_pbg(on_about);
  help_agent().add_activate_help (on_about, (char*)"on_about");

  // Tools
  f_view2 = XtCreateManagedWidget("view", xmPushButtonWidgetClass,
				  tool_bar, 0, 0);
  XtAddCallback(f_view2, XmNactivateCallback, display_nodeCB, this);
  help_text = new UAS_String(
		CATGETS(Set_AgentQHelp, 52, "Display selected section"));
  add_quick_help(f_view2, (char*)*help_text);
  f_help_text.insert_item(help_text);
  help_agent().add_help_cb(f_view2);



#ifdef AllowDetach
  // decision made to remove detach capabilities from CDE 2.1, rCs 6-14-96
  f_detach2 = XtCreateManagedWidget("detach", xmPushButtonWidgetClass,
				    tool_bar, 0, 0);
  XtAddCallback(f_detach2, XmNactivateCallback, detach_subtreeCB, this);
  help_text = new UAS_String(CATGETS(Set_AgentQHelp, 53, "Display selected section in a separate Book List"));
  add_quick_help(f_detach2, (char*)*help_text);
  f_help_text.insert_item(help_text);
#endif

  f_print2 = XtCreateManagedWidget("print", xmPushButtonWidgetClass,
				   tool_bar, 0, 0);
  XtAddCallback(f_print2, XmNactivateCallback, printCB, this);
  help_text = new UAS_String(CATGETS(Set_AgentQHelp, 54,
				     "Print selected section"));
  add_quick_help(f_print2, (char*)*help_text);
  f_help_text.insert_item(help_text);
  help_agent().add_help_cb(f_print2);

  widget = XtCreateManagedWidget("add", xmPushButtonWidgetClass, tool_bar, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, add_libraryCB, this);
  help_text = new UAS_String(CATGETS(Set_AgentQHelp, 55,
				"Open an additional information library"));
  add_quick_help(widget, (char*)*help_text);
  f_help_text.insert_item(help_text);
  help_agent().add_help_cb(widget);

  f_remove2 = XtCreateManagedWidget("remove", xmPushButtonWidgetClass,
				    tool_bar, 0, 0);
  XtAddCallback(f_remove2, XmNactivateCallback, remove_libraryCB, this);
  help_text = new UAS_String(CATGETS(Set_AgentQHelp, 56,
				"Close selected information library"));
  add_quick_help(f_remove2, (char*)*help_text);
  f_help_text.insert_item(help_text);
  help_agent().add_help_cb(f_remove2);

  widget = XtCreateManagedWidget("query", xmPushButtonWidgetClass,
			      tool_bar, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, query_editorCB, 0);
  help_text = new UAS_String(CATGETS(Set_AgentQHelp, 57,
				"Perform a search using the Query Editor"));
  add_quick_help(widget, (char*)*help_text);
  f_help_text.insert_item(help_text);
  help_agent().add_help_cb(widget);


  widget = XtCreateManagedWidget("marks_list", xmPushButtonWidgetClass,
			      tool_bar, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, mark_listCB, 0);
  help_text = new UAS_String(CATGETS(Set_AgentQHelp, 58,
				"Display the bookmarks and notes list"));
  add_quick_help(widget, (char*)*help_text);
  f_help_text.insert_item(help_text);
  help_agent().add_help_cb(widget);

  n = 0;
  XtSetArg(args[n], XmNscrolledWindowChildType, XmWORK_AREA); n++;
  Widget form = XtCreateWidget("form", xmFormWidgetClass, mainw, args, n);

  // this needs to be here for reference through the resources (attachments)
  Widget message = XtCreateManagedWidget("message", xmFrameWidgetClass,
					 form, 0, 0);

  // Booklist
  // we need a WXmFrame to pass to the following list
  WXmFrame frame = WXmFrame(form, "frame", WAutoManage, 0, 0);

  f_doc_tree_view = new OutlineListView(frame, "doc_list", WAutoManage, True);
  help_agent().add_help_cb((Widget)*f_doc_tree_view);


#ifdef DtinfoClient
  Atom atoms[4];
  atoms[0] = XInternAtom(XtDisplay(*f_doc_tree_view), "_DT_NETFILE", False);
  atoms[1] = XInternAtom(XtDisplay(*f_doc_tree_view), "FILE_NAME",   False);
  atoms[2] = XA_TEXT(XtDisplay(*f_doc_tree_view));
  atoms[3] = None;
  n = 0;
  XtSetArg(args[n], XmNimportTargets, atoms); n++;
  XtSetArg(args[n], XmNnumImportTargets, 3); n++;
  // becoming a sink of drag and drop
  XmeDropSink((Widget)*f_doc_tree_view, args, n);
  // add destination callback
  XtAddCallback((Widget)*f_doc_tree_view, XmNdestinationCallback, destCB, this);
#endif

  //  XtAddCallback(f_print, XmNactivateCallback, PrintButtonCB, window_system().GetAppPrintData());

  f_doc_tree_view->set_agent (this);

  Observe (f_doc_tree_view, OutlineListView::ENTRY_SELECTED,
	   &LibraryAgent::entry_selected);
  UAS_Common::request ((UAS_Receiver<UAS_DocumentRetrievedMsg> *) this);

  // Status line - force its colors to the same as its parent's ones
  Pixel fg, bg;
  n = 0;
  XtSetArg(args[n], XmNforeground, &fg); n++;
  XtSetArg(args[n], XmNbackground, &bg); n++;
  XtGetValues(message, args, n);
  n = 0;
  XtSetArg(args[n], XmNforeground, fg); n++;
  XtSetArg(args[n], XmNbackground, bg); n++;
  f_status_text = XtCreateManagedWidget("text", xmTextFieldWidgetClass,
					message, args, n);
  string = CATGETS(Set_LibraryAgent, 63, "Welcome to Dtinfo!");
  XtVaSetValues(f_status_text, XmNvalue, string, NULL);

  // Search area
  widget = XmCreatePulldownMenu(search_area, (char*)"scope_menu", 0, 0);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, widget); n++;
  XtManageChild(widget = XmCreateOptionMenu(search_area, (char*)"scope", args, n));
  XtVaSetValues(widget,
	XmNlabelString,
	(XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 59, "Search:")),
	NULL);
  help_text = new UAS_String(CATGETS(Set_AgentQHelp, 60,
				     "Specify search scope"));
  add_quick_help(widget, (char*)*help_text);
  f_help_text.insert_item(help_text);
  help_agent().add_help_cb(widget);

  // hookup the ScopeMenu object here
  f_scope_menu = new ScopeMenu (widget);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNleftWidget, widget); n++;

  // print callback pass a pointer to the AppPrintData class as client data
  
  //  f_print_button->SetActivateCallback(this, 
  //				     (WWL_FUN) &LibraryAgent::print_panel_activate,
  //				     );

  WXmTextField *search_text = (WXmTextField*)(Widget)
      WXmTextField (search_area, "text", WAutoManage, args, n);
  XtAddEventHandler (*search_text, EnterWindowMask, False,
		     (XtEventHandler) search_help, (XtPointer) this);
  XtAddEventHandler (*search_text, LeaveWindowMask, False,
		     (XtEventHandler) search_help, (XtPointer) this);
  XtAddEventHandler (*search_text, FocusChangeMask, False,
		     (XtEventHandler) search_help, (XtPointer) this);
  ON_ACTIVATE (*search_text,		text_callback);
  help_agent().add_help_cb((Widget)search_text);


  widget = XtCreateManagedWidget("scope_editor", xmPushButtonWidgetClass,
			      search_area, 0, 0);
  XtAddCallback(widget, XmNactivateCallback, scope_editorCB, 0);
  XtVaSetValues(widget,
    XmNlabelString,
    (XmString)XmStringLocalized(CATGETS(Set_AgentLabel, 61, "Scope Editor")),
    NULL);
  help_text = new UAS_String(CATGETS(Set_AgentQHelp, 62,
			     "Specify search scope using the Scope Editor"));
  add_quick_help(widget, (char*)*help_text);
  f_help_text.insert_item(help_text);
  help_agent().add_help_cb(widget);


  // set right attachment here since it cannot be set at creation time
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
  XtSetArg(args[n], XmNrightWidget, widget); n++;
  XtSetValues(*search_text, args, n);

  XtManageChild(form);
  XtManageChild(menu_bar);
  if (XmToggleButtonGadgetGetState(tool_barT))
      XtManageChild(tool_bar);
  if (XmToggleButtonGadgetGetState(search_areaT))
      XtManageChild(search_area);
  XtManageChild(frame);
  XtManageChild(mainw);

  f_wm_delete_callback =
    new WCallback (f_shell, window_system().WM_DELETE_WINDOW(),
		   this, (WWL_FUN) &LibraryAgent::close_window);
}

//////////////////////////////////////////////////////////////////
// close_window
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::close_windowCB(Widget, XtPointer client_data, XtPointer)
{
  LibraryAgent *agent = (LibraryAgent *)client_data;
  agent->close_window();
}

void
LibraryAgent::close_window()
{
  if (g_ignore_wm_delete)
    return;
  if (!XtIsSensitive(f_close)) {
      exitCB(0, f_shell, 0);
      return;
  }
  if (f_keep_forever)
    {
      f_popped_down = TRUE;
      f_doc_tree_view->untrack();
      //f_shell->Popdown();
      XtPopdown(f_shell);
      library_mgr().agent_unmapped(this);
    }
  else
    {
      delete this;
    }
}

// /////////////////////////////////////////////////////////////////
// add_quick_help
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::add_quick_help (Widget w, const char* help_text)
{
  if (help_text == NULL || *help_text == '\0') {
    XtVaSetValues(w, XmNuserData, NULL, NULL);
    return;
  }

  XtVaSetValues(w, XmNuserData, (void*)this, NULL);

  // Now add the event handlers to display the quick help.
  XtAddEventHandler (w, EnterWindowMask, False, quick_helpEH, (XtPointer)help_text);
  XtAddEventHandler (w, LeaveWindowMask, False, quick_helpEH, (XtPointer)help_text);
  XtAddEventHandler (w, FocusChangeMask, False, quick_helpEH, (XtPointer)help_text);
}

// /////////////////////////////////////////////////////////////////
// quick_help
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::quick_helpEH (Widget w, XtPointer client_data,
			  XEvent *event, Boolean *)
{
  // display the help text. 
  Arg args[2];
  int n;
  LibraryAgent *agent;

  n = 0;
  XtSetArg(args[n], XmNuserData, &agent); n++;
  XtGetValues(w, args, n);

  // display quick help if moving into widget or clear quick help
  // if moving out of widget.
  if (event->type == EnterNotify || event->type == FocusIn)
    XmTextFieldSetString(agent->f_status_text, (char*)client_data);
  else if (event->type == LeaveNotify || event->type != FocusOut)
    XmTextFieldSetString(agent->f_status_text, (char*)"");
}


// /////////////////////////////////////////////////////////////////
// search text callbacks
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::text_callback(WCallback *wcb)
{
#if 0
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*)wcb->CallData();
#endif
    char *text = XmTextGetString(wcb->GetWidget());

    if (*text != '\0')
    {
      Wait_Cursor bob;
      search_mgr().parse_and_search (text, f_scope_menu->current_scope());
      search_mgr().search_section ("");
    }
    XtFree (text);
}

void
LibraryAgent::search_help (Widget, XtPointer client_data,
			      XEvent *event, Boolean *)
{
  // Make sure the event is what we expect.
  if (event->type != EnterNotify && event->type != FocusIn &&
      event->type != LeaveNotify && event->type != FocusOut)
    return;

  LibraryAgent* agent = (LibraryAgent*)client_data;
  if (agent == NULL)
    return;


  if (event->type == LeaveNotify || event->type == FocusOut)
  {
    XmTextFieldSetString(agent->f_status_text, (char*)"");
    return;
  }

  
  static String help_text     = NULL;
  static String default_scope = NULL;

  if (help_text == NULL) {
    UAS_Pointer<UAS_String> str;
    str = new UAS_String(CATGETS(Set_AgentQHelp, 64, "Search %s"));
    help_text = (char*)*str;
    agent->f_help_text.insert_item(str);

    str = new UAS_String(CATGETS(Set_LibraryAgent, 65, "Current Section"));
    default_scope = (char*)*str;
    agent->f_help_text.insert_item(str);
  }
  
  // Format the search help string. 
  UAS_SearchScope *scope = agent->f_scope_menu->current_scope();
  char buffer[128];
  if (scope != NULL)
    snprintf (buffer, sizeof(buffer), help_text, (char *) scope->name());
  else
    snprintf (buffer, sizeof(buffer), help_text, default_scope);

  // Finally, display it in the quick help field.
  XmTextFieldSetString(agent->f_status_text, buffer);
}


// /////////////////////////////////////////////////////////////////
// ui_destroyed - user nuked Motif window, so kill myself
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::ui_destroyed()
{
  // Set f_shell to NULL, since Xt is destroying it for us. 
  f_shell = NULL;
  delete this;
}

void
LibraryAgent::ui_destroyedCB(Widget, XtPointer client_data, XtPointer)
{
  LibraryAgent *agent = (LibraryAgent*)client_data;
  delete agent;
}

#ifdef AllowDetach
// /////////////////////////////////////////////////////////////////
// detach_subtree
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::detach_subtreeCB(Widget, XtPointer client_data, XtPointer)
{
  LibraryAgent *agent = (LibraryAgent *)client_data;
  agent->detach_subtree();
}

void
LibraryAgent::detach_subtree()
{
  // NOTE: EVIL hack here.  Save the bithandle for the view that will
  // be created by the display call.  00:14 01/14/93 DJB
  ON_DEBUG (puts ("Doing the DETACH thing"));
  g_handle = f_doc_tree_view->data_handle();
  library_mgr().display (f_doc_tree_view->selection());
  g_handle = 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////
//  add_library
///////////////////////////////////////////////////////////////////////////////

void
LibraryAgent::add_libraryCB(Widget w, XtPointer client_data, XtPointer)
{
  LibraryAgent *agent = (LibraryAgent *)client_data;
  // put up the add library application modal dialog...
  //AddLibraryAgent addLib (*agent->f_shell);
  AddLibraryAgent addLib (w);
  char *newLib = addLib.getInfolibName();

  if (newLib)
  {
    agent->add_library(newLib);
    XtFree (newLib);
  }
}


// status flags for return from add_library()
// (these should never be changed in value -- see TtIpcMgr)
#define ID_SUCCESS      1
#define BAD_ARG_FORMAT  2
#define LOAD_ERROR      3
#define NOT_FOUND       5

int
LibraryAgent::add_library(char* newLib, Widget parent)
{
    int   sts = ID_SUCCESS ;
    int   bufferlen;

    if(  (newLib == NULL) ||
        ((newLib != NULL) && ( *newLib == '\0' )) )
    {
        message_mgr().error_dialog(
                (char*)UAS_String(CATGETS( Set_AddLibraryAgent, 6,
                      "No Infolib specified." )), parent);
        return (sts = BAD_ARG_FORMAT);
    }

    UAS_Pointer<UAS_Common> d = NULL ;
    mtry {
        // (precise locator format parsing to be done in the UAS layer)

        if( strchr( newLib, ':' ) && strchr( newLib, '/' ) ) {
            // assume to have a fully-qualified locator

            d = UAS_Common::create( newLib ) ;
        }
        else if( *newLib == '/' ) {
            // assume given absolute path to an infolib.
            // construct the fully-qualified form and pass it on.
            bufferlen = strlen("mmdb:INFOLIB=") + strlen(newLib) + 1;
            char *buffer = new char[bufferlen];
            snprintf (buffer, bufferlen, "mmdb:INFOLIB=%s", newLib);
            d = UAS_Common::create (buffer);
            delete [] buffer;
        }
        else if( !strchr( newLib, '/' ) ) {

            char* pathname;
            // check environment variables for valid paths
            pathname = env().infolibNameToPath(newLib);
            if (pathname != NULL)
            {
                // construct the fully-qualified form and pass it on.
                bufferlen = strlen("mmdb:INFOLIB=") + strlen(pathname) + 1;
                char *buffer = new char[bufferlen];
                snprintf (buffer, bufferlen, "mmdb:INFOLIB=%s", pathname);
                XtFree(pathname);
                d = UAS_Common::create (buffer);
                delete [] buffer;
            }
            else
            {
                message_mgr().error_dialog (
                  (char*)UAS_String(CATGETS(Set_AddLibraryAgent, 5,
                       "Infolib specification format error.")),
		  parent);
                sts = BAD_ARG_FORMAT ;
            }

        }
        else {
            message_mgr().error_dialog (
                (char*)UAS_String(CATGETS(Set_AddLibraryAgent, 2,
                     "Please enter an absolute path to the infolib.")),
	        parent);
        }

	if (d != (const int)NULL) {
	    d->retrieve ();
	} else {
#ifdef DEBUG
            // if returned to reach here, error msg probably given by
            // MMDB interface:   "Invalid MMDB infolib path: <>"
	    message_mgr().error_dialog (
	      (char*)UAS_String(CATGETS(Set_AddLibraryAgent, 3,
                     "Infolib open failed. Check the path specified.")),
	      parent);
#endif
            sts = NOT_FOUND ;
	}
    } mcatch_any () {
        // one case that will reach here, but get no MMDB interface error
        // is if a (close to) fully qualified locator format is entered,
        // but with keyword error.

        // report error only if likely from retrieval; others handled already
        if( d != (const int)NULL )
        {
          message_mgr().error_dialog(
                (char*)UAS_String(CATGETS(Set_AddLibraryAgent, 4,
                       "Infolib creation failed.")),
		parent) ;
        }
        sts = LOAD_ERROR ;
    } end_try;
    return sts;
}

///////////////////////////////////////////////////////////////////////////////
//  remove_library
///////////////////////////////////////////////////////////////////////////////

void
LibraryAgent::remove_libraryCB(Widget, XtPointer client_data, XtPointer)
{
  LibraryAgent *agent = (LibraryAgent *)client_data;
  library_mgr().undisplay (agent->f_doc_tree_view->selection());
}


// /////////////////////////////////////////////////////////////////
// entry_selected - something selected in list, sensitize detach
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::entry_selected (void *, u_int notify_type)
{
  bool sensitize_print = False;
  int selected_item_count = f_doc_tree_view->selected_item_count();

  Xassert (notify_type == OutlineListView::ENTRY_SELECTED);
  Xassert (selected_item_count >= 0);

  // Need to disable print if any selected items are infobase
  // or info library level.

  if (selected_item_count == 0)
    {
      f_oe = NULL;

#ifdef AllowDetach
      XtSetSensitive(f_detach, False);
      XtSetSensitive(f_detach2, False);
#endif

      XtSetSensitive(f_view, False);
      XtSetSensitive(f_view2, False);
      XtSetSensitive(f_remove, False);
      XtSetSensitive(f_remove2, False);
      XtSetSensitive(f_print2, False);
      XtSetSensitive(f_print_as, False);
      XtSetSensitive(f_copy, False);
    }
  else if (selected_item_count == 1)
    {
      // Get the single selected item.
      // NOTE: This is sort of round-about.
      // Got a better idea?  20:42 17-Mar-93 DJB 
      List *select_list = f_doc_tree_view->selected_item_list();
      f_oe = (OutlineElement *) (*select_list)[0];
      delete select_list;

#ifdef AllowDetach
      if (f_oe->has_children())
      {
	  XtSetSensitive(f_detach, True);
	  XtSetSensitive(f_detach2, True);
      }
      else
      {
	  XtSetSensitive(f_detach, False);
	  XtSetSensitive(f_detach2, False);
      }
#endif

      if (((TOC_Element *) f_oe)->toc()->data_length() != 0)
	{
	  XtSetSensitive(f_view, True);
	  XtSetSensitive(f_view2, True);
	}
      else
	{
	  XtSetSensitive(f_view, False);
	  XtSetSensitive(f_view2, False);
	}
      //
      //  Only enable remove if the selected item is one
      //  of the list's roots (corresponding to a top level
      //  information library, etc)
      //
      OutlineList &rootList = *(f_doc_tree_view->list());
      int removeSensitive = 0;
      if (rootList.length() > 1) { // don't let 'em remove the last one...
	  if (((TOC_Element *) f_oe)->toc()->type() == UAS_LIBRARY) {
	      for (unsigned int i = 0; i < rootList.length(); i ++) {
		if (((TOC_Element *) f_oe)->toc() ==
				    ((TOC_Element *) rootList[i])->toc()) {
		    removeSensitive = 1;
		    break;
		}
	      }
	  }
      }
      XtSetSensitive(f_remove, removeSensitive);
      XtSetSensitive(f_remove2, removeSensitive);
      sensitize_print = True;
      XtSetSensitive(f_copy, True);
    }
  else // (selected_item_count > 1)
    {
      f_oe = NULL;

#ifdef AllowDetach
      XtSetSensitive(f_detach, False);
      XtSetSensitive(f_detach2, False);
#endif

      XtSetSensitive(f_view, False);
      XtSetSensitive(f_view2, False);
      XtSetSensitive(f_remove, False);
      XtSetSensitive(f_remove2, False);
      XtSetSensitive(f_copy, True);
      sensitize_print = True;
    }

  // Only allow print to be sensitive if selected list does NOT contain
  // InfoLibrary or InfoBase elements.

  if (sensitize_print)
    {
      // Get the list of selections. 
      List *select_list = f_doc_tree_view->selected_item_list();
      Xassert (selected_item_count == (int) select_list->length());

      for (unsigned int i = 0; i < select_list->length(); i++)
	{
	  if (((TOC_Element *) (*select_list)[i])->toc()->data_length() == 0)
	    {
	      sensitize_print = False;
	      // No need to continue if non-printable found. 
	      break;
	    }
	}
      delete select_list;
    }

    XtSetSensitive(f_print2, sensitize_print);
    XtSetSensitive(f_print_as, sensitize_print);
}

// /////////////////////////////////////////////////////////////////
// Display button pressed
// ////////////////////////////////////////////////////////////////
void
LibraryAgent::display_nodeCB(Widget, XtPointer client_data, XtPointer)
{
  LibraryAgent *agent = (LibraryAgent *)client_data;
  agent->display_node();
}

void
LibraryAgent::display_node ()
{
  Xassert (f_oe != NULL);
  if (f_oe)
    f_oe->display();
}


// /////////////////////////////////////////////////////////////////
// print
// /////////////////////////////////////////////////////////////////


/*
 * ------------------------------------------------------------------------
 * Name: LibraryAgent::print_asCB
 *
 * Description:
 *
 *     This is called for "Print..." and will always bring up the
 *     print setup dialog.
 *     
 */

void
LibraryAgent::print_asCB(Widget w, XtPointer client_data, XtPointer)
{
    LibraryAgent *agent = (LibraryAgent *)client_data;

    //  Get a handle to the AppPrintData allocated in the WindowSystem class
    
    AppPrintData* p = window_system().GetAppPrintData();

    // Get the list of selected items. 
    List *select_list = agent->f_doc_tree_view->selected_item_list();
    xList<UAS_Pointer<UAS_Common> > * print_list = new  xList<UAS_Pointer<UAS_Common> >;
    
    for (unsigned int i = 0; i < select_list->length(); i++) {
	print_list->append (((TOC_Element *) (*select_list)[i])->toc());
    }

    p->f_print_list = print_list;
    p->f_outline_element = agent->f_oe;
 
    CreatePrintSetup(w, p);
    
    XtManageChild(p->f_print_dialog); /* popup dialog each time */
    
    delete select_list;

}

/*
 * ------------------------------------------------------------------------
 * Name: LibraryAgent::printCB
 *
 * Description:
 *
 *     Called when the user hits "Print" quick button.  Prints without
 *     displaying the Print setup dialog.
 *     
 */
void
LibraryAgent::printCB(Widget w, XtPointer client_data, XtPointer)
{
    LibraryAgent *agent = (LibraryAgent *)client_data;

    //  Get a handle to the AppPrintData allocated in the WindowSystem class
    
    AppPrintData* p = window_system().GetAppPrintData();

    // Get the list of selected items. 
    List *select_list = agent->f_doc_tree_view->selected_item_list();
    xList<UAS_Pointer<UAS_Common> > * print_list = new  xList<UAS_Pointer<UAS_Common> >;
    
    for (unsigned int i = 0; i < select_list->length(); i++) {
	print_list->append (((TOC_Element *) (*select_list)[i])->toc());
    }

    p->f_print_list = print_list;
    p->f_outline_element = agent->f_oe;
 
    CreatePrintSetup(w, p);

    // check if the DtPrintSetupBox ("Print...") has been called yet 

    if(p->f_print_data->print_display == (Display*)NULL)
    {
	
	// first time thru print setup, so get default data 
	
        if (DtPrintFillSetupData(p->f_print_dialog, p->f_print_data)
	    != DtPRINT_SUCCESS) {

	    // NOTE: DtPrintFillSetupData() already posts an error
	    // dialog on failure - no need to post our own.
    
	    delete select_list;
	    return ;
	}
    }
 
    DoPrint(w, p) ;
    
    delete select_list;
    
}

OutlineList *LibraryAgent::outline_list()
{
  return (f_doc_tree_view->list());
}


// /////////////////////////////////////////////////////////////////
// receive - handle node display by auto-tracking to it
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::receive (UAS_DocumentRetrievedMsg &message, void *)
{
  extern bool g_style_sheet_update ;
  if (!g_style_sheet_update)
    track_to (message.fDoc);
}

void
LibraryAgent::track_to (UAS_Pointer<UAS_Common> &node_ptr)
{
  ON_DEBUG (printf ("LibraryAgent::track_to: popped_down = %d, track set = %d, node_ptr = %p\n", f_popped_down, XmToggleButtonGadgetGetState(f_auto_track),(UAS_Common *) node_ptr));
  if (f_popped_down || !XmToggleButtonGadgetGetState(f_auto_track) || node_ptr == (const int)NULL)
    return;
  Wait_Cursor bob;
  ON_DEBUG (puts ("TRYING to locate document in doc tree!"));

  // First track document back up to it's root, so that
  // we can find it in our list.  If our root isn't in
  // the path to the root for the displayed document it
  // can't possible be displayed in our current subtree.
  OutlineList &rootList = *(f_doc_tree_view->list());
  bool in_subtree = FALSE;
  UAS_Pointer<UAS_Common> doc_root = node_ptr;
#ifdef DEBUG
  int inum;
#endif

  // Trace up to the root.
  free_tracking_hierarchy();
  while (doc_root != (const int)NULL && !in_subtree)
    {
      f_tracking_hierarchy = new TrackingEntry(doc_root, f_tracking_hierarchy);
      for (unsigned int i = 0; i < rootList.length(); i ++) {
	if (doc_root == ((TOC_Element *) rootList[i])->toc()) {
	    in_subtree = TRUE;
#ifdef DEBUG
	    inum = i;
#endif
	    break;
	}
      }
      doc_root = doc_root->parent();
    }

  if (in_subtree)
    {
      ON_DEBUG (printf ("Found node <%s> in my doc list subtree <%s>\n",
		    (char*)node_ptr->title(),
		    (char*)((TOC_Element *) rootList[inum])->toc()->title()));
      // Track to the element, scrolling if necessary. 
      track (TRUE);
    }
  else
    {
      f_doc_tree_view->untrack();
      free_tracking_hierarchy();
    }
}


// /////////////////////////////////////////////////////////////////
// track - (re)compute tracking position
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::track (bool scroll)
{
  if (f_popped_down || f_tracking_hierarchy == NULL || !XmToggleButtonGadgetGetState(f_auto_track))
    return;

  // Now walk back down the tree and highlight the lowest visible
  // entry above the document we are tracking.
  OutlineList &rootList = *(f_doc_tree_view->list());
  BitHandle handle = f_doc_tree_view->data_handle();
  // We must keep track of the number of items linerally skipped so
  // that we can ultimately highlight the right entry in the list. 
  unsigned int list_location = 0;
  // Make sure the roots match up, just to be safe.
  TrackingEntry *t = NULL;
  OutlineElement *oe = NULL;
  for (unsigned int cnt = 0; cnt < rootList.length(); cnt ++) {
      list_location ++;
      t = f_tracking_hierarchy->f_child;
      oe = (OutlineElement *) rootList[cnt];

      if (! oe->has_children())
	continue;

      // unexpanded infolib needs special handling here since
      // the while loop below does not work for this case
      if (! oe->is_expanded (handle)) {
	if (f_tracking_hierarchy &&
	    f_tracking_hierarchy->f_toc != ((TOC_Element *)oe)->toc())
	  continue;
      }

      // Scan the child list of the outline element
      // for the next matching TrackingEntry.
      int found = 1;
      while (t != NULL && oe->is_expanded (handle))
	{
	  // If the entry is expanded, it MUST have children. 
	  Xassert (oe->has_children());
	  OutlineList &kids = *(oe->children());
	  unsigned int i;
	  for (i = 0; i < kids.length(); i++)
	    {
	      // Keep track of how many expanded items we skip over.
	      list_location++;
	      if (((TOC_Element *) kids[i])->toc() == t->f_toc)
		break;
	      // Not found, so we must add all expanded children to count.
	      oe = (OutlineElement *) kids[i];
	      if (oe->is_expanded (handle) && oe->has_children())
		list_location += oe->children()->count_expanded (handle);
	    }
	  if (i < kids.length()) {
	      // Move down to the next level. 
	      oe = (OutlineElement *) kids[i];
	      t = t->f_child;
	  } else {
	      found = 0;
	      break;
	  }
	}
      if (found)
	break;
    }

  // At this point the oe ponts to the closest expanded entry
  // to the document that has just been displayed and location
  // is it's index in the overall list.

  ON_DEBUG (printf ("target element = <%s>, list_location = %d\n",
		    oe->display_as(), list_location));
  int old_selection  = f_doc_tree_view->tracking_position();

  // Select the icon depending on wheter or not we're on the exact
  // entry that we were seeking. 
  if (t == NULL)
    f_doc_tree_view->track_to (oe, list_location, OLIAS_SOLID_TRACKER);
  else
    f_doc_tree_view->track_to (oe, list_location, OLIAS_HOLLOW_TRACKER);

  // Scroll the entry into view if necessary.
  // If the entry is not visible and is one position before
  // or after the currently selected entry, scroll the list
  // by one position in the correct position.

  if (scroll)
    {
      int visible_items = f_doc_tree_view->VisibleItemCount();
      int top_position = f_doc_tree_view->TopItemPosition();
      int bottom_position = top_position + visible_items - 1;
      ON_DEBUG (printf ("top = %d, bottom = %d, old = %d\n",
			top_position, bottom_position, old_selection));

      if ((int)list_location < top_position ||
	  (int)list_location > bottom_position)
	{
	  ON_DEBUG (puts ("* About to scroll list"));
	  if ((int)old_selection == top_position &&
	      (int)list_location == top_position - 1)
	    {
	      // Scroll up one item. 
	      f_doc_tree_view->TopItemPosition (top_position - 1);
	    }
	  else if ((int)old_selection == bottom_position &&
		   (int)list_location == bottom_position + 1)
	    {
	      // Scroll down one item. 
	      f_doc_tree_view->TopItemPosition (top_position + 1);
	    }
	  else
	    {
	      // Center the entry in the list.
	      int new_top = list_location - (visible_items - 1) / 2;
	      int new_bottom = new_top + visible_items - 1;
	      if (new_bottom > f_doc_tree_view->ItemCount())
		new_top = f_doc_tree_view->ItemCount() - visible_items + 1;
	      else if (new_top < 1)
		new_top = 1;
	      ON_DEBUG (printf ("Centering, new_top = %d\n", new_top));
	      f_doc_tree_view->TopItemPosition (new_top);
	    }
	}
    }
}


// /////////////////////////////////////////////////////////////////
// free_tracking_hierarchy
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::free_tracking_hierarchy()
{
  TrackingEntry *tmp;
  while (f_tracking_hierarchy != NULL)
    {
      tmp = f_tracking_hierarchy;
      f_tracking_hierarchy = tmp->f_child;
      delete tmp;
    }
}


// /////////////////////////////////////////////////////////////////
// auto_track_toggle - turn auto track on and off
// /////////////////////////////////////////////////////////////////

void
LibraryAgent::auto_track_toggleCB(Widget, XtPointer client_data, XtPointer)
{
  LibraryAgent *agent = (LibraryAgent *)client_data;
  if (XmToggleButtonGadgetGetState(agent->f_auto_track))
      agent->track_to(node_mgr().last_displayed());
  else
    {
      agent->f_doc_tree_view->untrack();
      agent->free_tracking_hierarchy();
    }
}

void
LibraryAgent::library_removed (UAS_Pointer<UAS_Common> lib)
{
    OutlineList &rootList = *(f_doc_tree_view->list());
    unsigned int i;
    for (i = rootList.length() - 1; i >= 0; i --) {
	TOC_Element *te = (TOC_Element *) rootList[i];
	if (te->toc()->get_library() == lib) {
	    if (f_tracking_hierarchy && f_tracking_hierarchy->f_toc == lib) {
		f_doc_tree_view->untrack();
		free_tracking_hierarchy ();
	    }
	    rootList.remove(i);
	    // Not sure if I delete this guy...
	    delete te;
	}
    }
    f_doc_tree_view->clear ();

    // don't need to generate a new handle, just use the old one
    //BitHandle handle = rootList.get_data_handle();
    BitHandle handle = f_doc_tree_view->data_handle();

    for (i = 0; i < rootList.length(); i ++)
    {
      ((OutlineElement *) rootList[i])->set_expanded (handle);

      // check bookcases to make sure they are not expanded.
      // if they are expanded, make them contract.
      OutlineElement *oe = (OutlineElement*)rootList[i];

      List *bclist = oe->children();
         
      for(unsigned int b = 0; b < bclist->length(); b++)
      {
        OutlineElement *coe = (OutlineElement *)(*bclist)[b];
        if (coe->is_expanded(handle))
          coe->set_contracted(handle);
      }
    }

    // for some reason set_list doesn't think that the outline
    // list needs to be updated--so force the update to happen
    f_doc_tree_view->update_list(&rootList, handle);
    f_doc_tree_view->set_list (&rootList, handle);
    entry_selected (NULL, OutlineListView::ENTRY_SELECTED);
}

void
LibraryAgent::destCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmDestinationCallbackStruct* cs = (XmDestinationCallbackStruct*)call_data;
    
#if 0
    Atom targets = XInternAtom(XtDisplay(w), "_MOTIF_EXPORT_TARGETS", False);
#endif

    XmTransferValue(cs->transfer_id, XA_TARGETS(XtDisplay(w)),
		    (XtCallbackProc)transferCB, client_data,
		    XtLastTimestampProcessed(XtDisplay(w)));
}

static Atom
pickup_target(Widget w, Atom* targets, unsigned long length)
{
    if (targets == NULL || length == 0)
	return None;

    Atom best = None;

    Atom *iter;
    for (iter = targets; iter < targets + length; iter++) {
	if (*iter == XInternAtom(XtDisplay(w), "_DT_NETFILE", False)) {
	    best = *iter;
#ifdef DD_DEBUG
	    cerr << "(DEBUG) pickup _DT_NETFILE" << endl;
#endif
	    break;
	}
    }
    if (best != None)
	return best;

    for (iter = targets; iter < targets + length; iter++) {
	if (*iter == XInternAtom(XtDisplay(w), "FILE_NAME", False)) {
	    best = *iter;
#ifdef DD_DEBUG
	    cerr << "(DEBUG) pickup FILE_NAME" << endl;
#endif
	    break;
	}
    }
    if (best != None)
	return best;

#if 0
    for (iter = targets; iter < targets + length; iter++) {
	if (*iter == XA_COMPOUND_TEXT(XtDisplay(w))) {
	    best = *iter;
#ifdef DD_DEBUG
	    cerr << "(DEBUG) pickup XA_COMPOUND_TEXT" << endl;
#endif
	    break;
	}
    }
    if (best != None)
	return best;
#endif

    for (iter = targets; iter < targets + length; iter++) {
	if (*iter == XA_TEXT(XtDisplay(w))) {
	    best = *iter;
#ifdef DD_DEBUG
	    cerr << "(DEBUG) pickup XA_TEXT" << endl;
#endif
	    break;
	}
    }    
    if (best != None)
	return best;

    for (iter = targets; iter < targets + length; iter++) {
	if (*iter == XA_STRING) {
	    best = *iter;
#ifdef DD_DEBUG
	    cerr << "(DEBUG) pickup XA_STRING" << endl;
#endif
	    break;
	}
    }    

#ifdef DD_DEBUG
    if (best == None)
	cerr << "(DEBUG) no proper Atoms found" << endl;
#endif

    return best;
}

static int
IsInfolib(const char* path)
{
    if (path == NULL || *path == '\0')
	return False;

    struct stat stat_buf;
    if (stat(path, &stat_buf) < 0)
	return False;
    
    if ((stat_buf.st_mode & S_IFMT & S_IFDIR) == 0)
	return False;
	
    if (access((char*)path, R_OK) < 0)
	return False;

    UAS_String map = path;
    map = map + "/" + "bookcase.map";

    if (access((char*)map, R_OK) < 0)
	return False;

    return True;
}

void
LibraryAgent::transferCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmSelectionCallbackStruct* cs = (XmSelectionCallbackStruct*)call_data;

#ifdef DD_DEBUG
    cerr << "transferCB called." << endl;
#endif

    if (cs->target == XA_TARGETS(XtDisplay(w))) {

	if (cs->type != XA_ATOM || cs->length == 0) { // check integrity
#ifdef DD_DEBUG
	    cerr << "this guy doesn't understand XA_TARGETS." << endl;
#endif
	    XmTransferDone(cs->transfer_id, XmTRANSFER_DONE_FAIL);
	    return;
	}

	Atom best_target = None;
	best_target = pickup_target(w, (Atom *)cs->value, cs->length);

	if (best_target == None) {
#ifdef DD_DEBUG
	    cerr << "could not resolve the best target." << endl;
#endif
	    XmTransferDone(cs->transfer_id, XmTRANSFER_DONE_FAIL);
	    return;
	}

	XmTransferValue(cs->transfer_id, best_target,
			(XtCallbackProc)transferCB, client_data,
			XtLastTimestampProcessed(XtDisplay(w)));
	return;
    }
    else if (cs->target == XInternAtom(XtDisplay(w), "_DT_NETFILE", False) ||
	     cs->target == XInternAtom(XtDisplay(w), "FILE_NAME", False) ||
	     cs->target == XA_TEXT(XtDisplay(w))) {

	char* netfile = (char*)cs->value;
	if (netfile == NULL || *netfile == '\0') {
	    XmTransferDone(cs->transfer_id, XmTRANSFER_DONE_FAIL);
	    return;
	}

	char* file;
	if (cs->target == XInternAtom(XtDisplay(w), "_DT_NETFILE", False))
	    file = tt_netfile_file(netfile);
	else
	    file = strdup(netfile);

	if (file == NULL || *file == '\0') {
		XtFree(netfile);
		XmTransferDone(cs->transfer_id, XmTRANSFER_DONE_FAIL);
		return;
	}

	XtFree(netfile);

#ifdef DD_DEBUG
	fprintf(stderr, "(DEBUG) local filename=\"%s\"\n", file);
#endif
	if (! IsInfolib(file)) { // inaccesible from the host
	    tt_free(file);
	    XmTransferDone(cs->transfer_id, XmTRANSFER_DONE_FAIL);
	    return;
	}

	UAS_String url = "mmdb:INFOLIB=";
	url = url + file;

	UAS_Pointer<UAS_Common> pIL;
	mtry {
	    pIL = UAS_Common::create ((char*)url);
	    if (! (pIL == (UAS_Pointer<UAS_Common>)NULL)) {
		pIL->retrieve();
	    }
	}
	mcatch_any() {
	    pIL = NULL;
	}
	end_try;

	if (pIL ==  (UAS_Pointer<UAS_Common>)NULL) {
	    tt_free(file);
	    XmTransferDone(cs->transfer_id, XmTRANSFER_DONE_FAIL);
	    return;
	}

	tt_free(file);

	XmTransferDone(cs->transfer_id, XmTRANSFER_DONE_SUCCEED);
	return;
    }
}

void
LibraryAgent::copy_to_clipbd()
{
  List *select_list = f_doc_tree_view->selected_item_list();
  int status;
  UAS_String buf;
  XmString clip_label;
  static int private_id = 0;
  //Window window = XtWindowOfObject((Widget)f_shell);
  Window window = XtWindowOfObject(f_shell);
  long item_id = 0;
  clip_label = XmStringCreateLocalized((char*)"Data");
  Wait_Cursor bob;
  UAS_String nl("\n");
  
  for (unsigned int i = 0; i < select_list->length(); i++)
  {
    UAS_Pointer<UAS_Common> toc;
    toc = ((TOC_Element *) (*select_list)[i])->toc();

    buf = buf + toc->title() + nl;
  }

  private_id++;

  // start a copy--retry until unlocked
  do
  {
    status = XmClipboardStartCopy(
                window_system().display(),
                window,
                clip_label,
                CurrentTime,
                NULL, NULL, &item_id);
  } while(status == ClipboardLocked);
                
  do
  {
    status = XmClipboardCopy(
                window_system().display(),
                window,
                item_id,
                (char*)"STRING",
                (char*)buf, (long)strlen((char*)buf) + 1,
                private_id, NULL);
  } while(status == ClipboardLocked);
              
  do
  {
    status = XmClipboardEndCopy(
                window_system().display(),
                window,
                item_id);
  } while(status == ClipboardLocked);


  XmStringFree(clip_label);
  delete select_list;
}

