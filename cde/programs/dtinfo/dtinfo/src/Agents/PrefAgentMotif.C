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
//	Copyright (c) 1994,1995,1996 FUJITSU LIMITED
//	All Rights Reserved

/*
 * $XConsortium: PrefAgentMotif.C /main/14 1996/11/23 14:13:16 cde-hal $
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

#define C_MapMgr
#define C_PrefMgr
#define C_StyleSheetMgr
#define C_NodeWindowMgr
#define C_GlobalHistoryMgr
#define C_SearchMgr

#define L_Managers

#define C_HelpAgent
#define C_PrefAgent
#define L_Agents

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include <Prelude.h>

#include "Registration.hh"

#include <WWL/WXmForm.h>
#include <WWL/WXmMenu.h>
#include <WWL/WXmLabel.h>
#include <WWL/WXmFrame.h>
#include <WWL/WXmSeparator.h>
#include <WWL/WXmArrowButton.h>
#include <WWL/WXmDialogShell.h>
#include <WWL/WXmMessageBox.h>

#include <X11/cursorfont.h>

#ifdef SVR4
#ifndef USL
#include <libintl.h>	// 1/21/94 yuji
#endif
#endif

/* Update time in milliseconds when arrow button held down. */
#define CHANGE_SPEED 300

#define CLASS PrefAgent
#include "create_macros.hh"

// All the stuff below is semi-bogus.  There really needs to be classes
// for displaying values of certain types, and there needs to be preference
// classes which take care of the modified/unmodified BS.
// 14:03 11-Apr-93 DJB

// Modes of operation:
// 1) Set field from preference (initialize only!).
// 2) Reset field from preference. 
// 3) Set preference from field (when field is modified).
// 4) Update other UI parts when preference changes.  

  
class ChangeData {
public:
  ChangeData (WXmTextField &f, Widget up, Widget dn, int c, int l)
    : field (f), up_arrow(up), dn_arrow(dn), change(c), limit(l)
    { }

  WXmTextField field;		// Field to change 
  Widget       up_arrow;
  Widget       dn_arrow;
  int          change;		// Amount of change
  int          limit;		// Limit of change 
};


class Value {
public:
  Value()
    { modified();  }
  void modified()
    { changed = TRUE; num_changed++; }
  void unmodified()
    { changed = FALSE; num_changed--; }
  bool changed;
  static int num_changed;
};

int Value::num_changed = 0;

class IntegerValue : public Value {
public:
  IntegerValue (int value)
    : original (value), current (value) { }
  void set_value (int value)
    {
      if (value == original)  // ie: reset 
	unmodified();
      else if (!changed)      // ie: first change
	modified();
      current = value;
    }
  void apply (PrefSymbol sym)
    {
      if (changed) {
        unmodified(); original = current;
	pref_mgr().set_int (sym, current);
      }
    };

  int original;
  int current;
};

class BooleanValue : public Value {
public:
  BooleanValue (bool value)
    : original (value), current (value) { }
  void set_value (int value)
    {
      if (value == original)  // ie: reset 
	unmodified();
      else if (!changed)      // ie: first change
	modified();
      current = value;
    }
  void apply (PrefSymbol sym)
    {
      if (changed) {
        unmodified(); original = current;
	pref_mgr().set_boolean (sym, current);
      }
    };

  bool original;
  bool current;
};

// NOTE: String currently unused, will probably have to strdup() values. 
class StringValue : public Value {
public:
  StringValue (String value)
    : original (value), current (value) { }
  void set_value (String value)
    {
      if (value == original)  // ie: reset 
	unmodified();
      else if (!changed)      // ie: first change
	modified();
      current = value;
    }
  void apply (PrefSymbol sym)
    {
      if (changed) {
        unmodified(); original = current;
	pref_mgr().set_string (sym, current);
      }
    };

  String original;
  String current;
};

class WindowGeometryValue : public Value {
public:
  WindowGeometryValue (const WindowGeometry &value)
    : original (value), current (value) { }
  void set_value (WindowGeometry &value)
    {
      if (value == original)  // ie: reset 
	unmodified();
      else if (!changed)      // ie: first change
	modified();
      current = value;
    }
  void apply (PrefSymbol sym)
    {
      if (changed) {
	  unmodified(); original = current;
	  pref_mgr().set_geometry (sym, current);
	}
    };

  WindowGeometry original;
  WindowGeometry current;
};

#define IS_CHANGED(X) (((Value *) (X).UserData())->changed)
#define SET_CHANGED(X,V) ((Value *) (X).UserData())->changed = V

#if defined(__STDC__) || defined(hpux) || defined(__osf__)
#define ATTACH_VALUE(X,T,V) (X).UserData ((void *) new T##Value(V))
#define SET_VALUE(X,T,V) ((T##Value *) (X).UserData())->set_value(V)
#define ORIGINAL_VALUE(X,T) ((T##Value *) (X).UserData())->original
#define CURRENT_VALUE(X,T) ((T##Value *) (X).UserData())->current
#define APPLY_VALUE(X,T,S) ((T##Value *) (X).UserData())->apply(S)
#else
#define ATTACH_VALUE(X,T,V) (X).UserData ((void *) new T/**/Value(V))
#define SET_VALUE(X,T,V) ((T/**/Value *) (X).UserData())->set_value(V)
#define ORIGINAL_VALUE(X,T) ((T/**/Value *) (X).UserData())->original
#define CURRENT_VALUE(X,T) ((T/**/Value *) (X).UserData())->current
#define APPLY_VALUE(X,T,S) ((T/**/Value *) (X).UserData())->apply(S)
#endif

// /////////////////////////////////////////////////////////////////
// Constructor
// /////////////////////////////////////////////////////////////////


PrefAgent::PrefAgent()
: f_shell(NULL)
{
}


// /////////////////////////////////////////////////////////////////
// Destructor
// /////////////////////////////////////////////////////////////////

PrefAgent::~PrefAgent()
{
  delete f_timeout;
  f_timeout = NULL;
  f_shell.Destroy();
}


// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
PrefAgent::display()
{
  Wait_Cursor bob;
  if (f_shell == 0)
    create_ui();

  // Always reset on new popup. 
  reset();

  f_shell.Popup();
  f_shell.DeIconify();
}


// /////////////////////////////////////////////////////////////////
// create_ui
// /////////////////////////////////////////////////////////////////

void
PrefAgent::create_ui()
{
  // Create main preferences window.

  XmStringLocalized mtfstring;
  String	    string;
  int		    decorations=MWM_DECOR_BORDER  |
                                MWM_DECOR_RESIZEH |
                                MWM_DECOR_TITLE   |
                                MWM_DECOR_MENU;

  f_shell = WTopLevelShell (window_system().toplevel(), WPopup, "preferences");
  window_system().register_shell(&f_shell);

  string = CATGETS(Set_PrefAgent, 1, "Dtinfo: Preferences");
  XtVaSetValues((Widget)f_shell,
                XmNtitle, string,
                XmNmwmDecorations, decorations,
                NULL);

  DECL  (WXmForm,         form,         f_shell,      "form");
  DECLMC(WXmOptionMenu,   options,      form,         "options");
  DECLC (WXmPulldownMenu, options_menu, form,         "options_menu");

  mtfstring = CATGETS(Set_AgentLabel, 186, "Preferences for");
  XtVaSetValues(options, XmNlabelString, (XmString)mtfstring, NULL);

  ASSNM (WXmPushButton,   f_ok,         form,         "ok");
  ASSNM (WXmPushButton,   f_apply,      form,         "apply");
  ASSNM (WXmPushButton,   f_reset,      form,         "reset");
  DECLM (WXmPushButton,   cancel,       form,         "cancel");
  DECLM (WXmPushButton,   help,         form,         "help");

  mtfstring = CATGETS(Set_AgentLabel, 161, "OK");
  XtVaSetValues(f_ok, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 201, "Apply");
  XtVaSetValues(f_apply, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 202, "Reset");
  XtVaSetValues(f_reset, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 162, "Cancel");
  XtVaSetValues(cancel, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 48, "Help");
  XtVaSetValues(help, XmNlabelString, (XmString)mtfstring, NULL);

  SET_CALLBACK (f_ok,Activate,ok);
  SET_CALLBACK (f_apply,Activate,apply);
  SET_CALLBACK (f_reset,Activate,reset);
  SET_CALLBACK (cancel,Activate,cancel);
  help_agent().add_activate_help (help, (char*)"preferences_help");

  DECLM (WXmFrame,        frame,        form,         "frame");
  DECL  (WXmForm,         container,    frame,        "container");

  // Create browsing preferences.
  DECLM (WXmPushButton,   browse,          options_menu,  "browse");

  DECL  (WXmForm,         browse_form,     container,     "browse_prefs");
  DECLM (WXmLabel,        browse_label,    browse_form,   "browse_label");
  DECLM (WXmPushButton,   get_browse_geo,  browse_form,   "get_browse_geo");
  ASSNM (WXmTextField,    f_browse_geo,    browse_form,   "browse_geo");
  DECLM (WXmLabel,        fs_label,        browse_form,   "fs_label");
  DECL  (WXmForm,         fs_form,         browse_form,   "fs_form");
  ASSNM (WXmTextField,    f_fs_field,      fs_form,       "scale_field");
  DECLM (WXmArrowButton,  scale_up,        fs_form,       "scale_up");
  DECLM (WXmArrowButton,  scale_down,      fs_form,       "scale_down");
  DECLM (WXmLabel,        lock_label,      browse_form,   "lock_label");
  ASSNM (WXmToggleButton, f_lock_toggle,   browse_form,   "lock_toggle");

  mtfstring = CATGETS(Set_AgentLabel, 187, "Browsing");
  XtVaSetValues(browse, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 190, "Browser Geometry");
  XtVaSetValues(browse_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 270, "...");
  XtVaSetValues(get_browse_geo, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 192, "Font Scale");
  XtVaSetValues(fs_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 193, "Pin Window");
  XtVaSetValues(lock_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 194, "File a Bug");
  XtVaSetValues(f_lock_toggle, XmNlabelString, (XmString)mtfstring, NULL);

  Dimension height;
  ChangeData *inc, *dec;

  // Assume that the fields are taller than the labels.  If the user
  // changes the font resources and violates this assumtion he's screwed
  // and that's the way we like it.

  height = f_fs_field.Height();

  browse_label.Height (height);
  fs_label.Height (height);
  scale_up.Height (height/2);
  scale_down.Height (height/2);

  f_lock_toggle.LabelType (XmPIXMAP);
  lock_label.Height (height);

  fs_form.Manage();
  browse_form.Manage();
  f_top_panel = browse_form;

  SET_CALLBACK_D (browse,Activate,switch_to_window,(Widget)browse_form);
  SET_CALLBACK_D (get_browse_geo,Activate,get_geometry,(Widget)f_browse_geo);
  SET_CALLBACK (f_browse_geo,ValueChanged,something_changed);
  SET_CALLBACK (f_fs_field,ValueChanged,something_changed);
  SET_CALLBACK (f_lock_toggle,ValueChanged,something_changed);
  SET_CALLBACK (f_lock_toggle,ValueChanged,lock_toggle);

  inc = new ChangeData(f_fs_field, scale_up, scale_down, 1, 5);
  dec = new ChangeData(f_fs_field, scale_up, scale_down, -1, -2);
  SET_CALLBACK_D (scale_up,Activate,change_cb,inc);
  SET_CALLBACK_D (scale_up,Arm,arm_arrow,inc);
  SET_CALLBACK  (scale_up,Disarm,disarm_arrow);
  SET_CALLBACK_D (scale_down,Activate,change_cb,dec);
  SET_CALLBACK_D (scale_down,Arm,arm_arrow,dec);
  SET_CALLBACK  (scale_down,Disarm,disarm_arrow);

  // Create map window preferences
  DECLM (WXmPushButton,   map,             options_menu,  "map");
  DECLM (WXmForm,         map_form,        container,     "map_prefs");
  DECLM (WXmLabel,        map_label,       map_form,      "map_label");
  DECLM (WXmPushButton,   get_map_geo,     map_form,      "get_map_geo");
  ASSNM (WXmTextField,    f_map_geo,       map_form,      "map_geo");
  DECLM (WXmLabel,        update_label,    map_form,      "update_label");
  ASSNM (WXmToggleButton, f_update_toggle, map_form,      "update_toggle");

  mtfstring = CATGETS(Set_AgentLabel, 188, "Map");
  XtVaSetValues(map, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 195, "Map Geometry");
  XtVaSetValues(map_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 270, "...");
  XtVaSetValues(get_map_geo, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 196, "Auto Update");
  XtVaSetValues(update_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 194, "File a Bug");
  XtVaSetValues(f_update_toggle, XmNlabelString, (XmString)mtfstring, NULL);

  height = f_map_geo.Height();
  map_label.Height (height);
  update_label.Height (height);
  
  SET_CALLBACK_D (get_map_geo,Activate,get_geometry,(Widget)f_map_geo);
  SET_CALLBACK_D (map,Activate,switch_to_window,(Widget)map_form);
  SET_CALLBACK (f_map_geo,ValueChanged,something_changed);
  SET_CALLBACK (f_update_toggle,ValueChanged,something_changed);
  SET_CALLBACK (f_update_toggle,ValueChanged,update_toggle);

  map_form.Manage();

  // Create history preferences 
  DECLM (WXmPushButton,   history,      options_menu, "history");

  mtfstring = CATGETS(Set_AgentLabel, 260, "History");
  XtVaSetValues(history, XmNlabelString, (XmString)mtfstring, NULL);

  DECL  (WXmForm,         hist_form,     container,     "history_prefs");
  DECLM (WXmLabel,        nh_label,      hist_form,     "nh_label");
  DECL  (WXmForm,         nh_form,       hist_form,     "nh_form");
  ASSNM (WXmTextField,    f_nh_field,    nh_form,       "nh_field");
  DECLM (WXmArrowButton,  nh_up,         nh_form,       "nh_up");
  DECLM (WXmArrowButton,  nh_down,       nh_form,       "nh_down");
  DECLM (WXmLabel,        sh_label,      hist_form,     "sh_label");
  DECL  (WXmForm,         sh_form,       hist_form,     "sh_form");
  ASSNM (WXmTextField,    f_sh_field,    sh_form,       "sh_field");
  DECLM (WXmArrowButton,  sh_up,         sh_form,       "sh_up");
  DECLM (WXmArrowButton,  sh_down,       sh_form,       "sh_down");

  mtfstring = CATGETS(Set_AgentLabel, 197, "Section History Size");
  XtVaSetValues(nh_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 198, "Search History Size");
  XtVaSetValues(sh_label, XmNlabelString, (XmString)mtfstring, NULL);

  height = f_nh_field.Height();
  nh_label.Height (height);
  nh_up.Height (height/2);
  nh_down.Height (height/2);

  height = f_sh_field.Height();
  sh_label.Height (height);
  sh_up.Height (height/2);
  sh_down.Height (height/2);

  nh_form.Manage();
  sh_form.Manage();
  hist_form.Manage();
  
  SET_CALLBACK_D (history,Activate,switch_to_window,(Widget)hist_form);
  SET_CALLBACK (f_nh_field,ValueChanged,something_changed);
  SET_CALLBACK (f_sh_field,ValueChanged,something_changed);

  inc = new ChangeData (f_nh_field, nh_up, nh_down, 10, 1000);
  dec = new ChangeData (f_nh_field, nh_up, nh_down, -10, 10);
  SET_CALLBACK_D (nh_up,Activate,change_cb,inc);
  SET_CALLBACK_D (nh_up,Arm,arm_arrow,inc);
  SET_CALLBACK (nh_up,Disarm,disarm_arrow);
  SET_CALLBACK_D (nh_down,Activate,change_cb,dec);
  SET_CALLBACK_D (nh_down,Arm,arm_arrow,dec);
  SET_CALLBACK (nh_down,Disarm,disarm_arrow);

  inc = new ChangeData (f_sh_field, sh_up, sh_down, 10, 1000);
  dec = new ChangeData (f_sh_field, sh_up, sh_down, -10, 10);
  SET_CALLBACK_D (sh_up,Activate,change_cb,inc);
  SET_CALLBACK_D (sh_up,Arm,arm_arrow,inc);
  SET_CALLBACK (sh_up,Disarm,disarm_arrow);
  SET_CALLBACK_D (sh_down,Activate,change_cb,dec);
  SET_CALLBACK_D (sh_down,Arm,arm_arrow,dec);
  SET_CALLBACK (sh_down,Disarm,disarm_arrow);

  // Create Search preferences.

  DECLM (WXmPushButton,   search,            options_menu,  "search");

  mtfstring = CATGETS(Set_AgentLabel, 189, "Searching");
  XtVaSetValues(search, XmNlabelString, (XmString)mtfstring, NULL);

  DECL  (WXmForm,         search_form,       container,     "search_prefs");
  DECLM (WXmLabel,        max_hits_label,    search_form,   "max_hits");
  DECL  (WXmForm,         hits_form,         search_form,   "hits_form");
  ASSNM (WXmTextField,    f_max_hits_field,  hits_form,     "hits_field");
  DECLM (WXmArrowButton,  hits_up,           hits_form,     "hits_up");
  DECLM (WXmArrowButton,  hits_down,         hits_form,     "hits_down");
  DECLM (WXmLabel,        adisplay_label,    search_form,   "adisplay_label");
  ASSNM (WXmToggleButton, f_adisplay_toggle, search_form,   "adisplay_toggle");

  mtfstring = CATGETS(Set_AgentLabel, 199, "Maximum Search Hits");
  XtVaSetValues(max_hits_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 200, "Auto Display First Hit");
  XtVaSetValues(adisplay_label, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 194, "File a Bug");
  XtVaSetValues(f_adisplay_toggle, XmNlabelString, (XmString)mtfstring, NULL);

  height = f_max_hits_field.Height();
  max_hits_label.Height (height);
  hits_up.Height (height/2);
  hits_down.Height (height/2);
  adisplay_label.Height (height);

  hits_form.Manage();
  search_form.Manage();

  SET_CALLBACK_D (search,Activate,switch_to_window,(Widget)search_form);
  SET_CALLBACK (f_max_hits_field,ValueChanged,something_changed);

  inc = new ChangeData (f_max_hits_field, hits_up, hits_down, 10, 500);
  dec = new ChangeData (f_max_hits_field, hits_up, hits_down, -10, 10);
  SET_CALLBACK_D (hits_up,Activate,change_cb,inc);
  SET_CALLBACK_D (hits_up,Arm,arm_arrow,inc);
  SET_CALLBACK (hits_up,Disarm,disarm_arrow);
  SET_CALLBACK_D (hits_down,Activate,change_cb,dec);
  SET_CALLBACK_D (hits_down,Arm,arm_arrow,dec);
  SET_CALLBACK (hits_down,Disarm,disarm_arrow);
  SET_CALLBACK (f_adisplay_toggle,ValueChanged,something_changed);
  SET_CALLBACK (f_adisplay_toggle,ValueChanged,adisplay_toggle);

  /* -------- node history size, search history size -------- */

  options.SubMenuId (options_menu);

  container.Manage();
  form.DefaultButton (f_ok);
  form.ShadowThickness (0);
  form.Manage();

  f_shell.Realize();
  Dimension width;
  f_shell.Get (WArgList (XmNheight, (XtArgVal) &height,
			 XmNwidth, &width,
			 NULL));
  f_shell.Set (WArgList (XmNminHeight, height,
			 XmNmaxHeight, height,
			 XmNminWidth,  width,
			 XmNmaxWidth,  width,
			 NULL));

  map_form.Unmanage();
  hist_form.Unmanage();
  search_form.Unmanage();

  // Values get displayed when reset() is called on dialog display. 
  init_values();
}


// /////////////////////////////////////////////////////////////////
// apply
// /////////////////////////////////////////////////////////////////

void
PrefAgent::ok()
{
  apply();
  f_shell.Popdown();
}


// NOTE:
// All the functionaly in the `if' statements below in appy and reset
// really should be part of the class.  No time right now, though. 


// /////////////////////////////////////////////////////////////////
// apply
// /////////////////////////////////////////////////////////////////

void
PrefAgent::apply()
{
  // Save browse preferences.
  bool update_fs = False;
  if (IS_CHANGED (f_fs_field))
    update_fs = True;
  APPLY_VALUE (f_browse_geo, WindowGeometry, PrefMgr::BrowseGeometry);
  APPLY_VALUE (f_fs_field, Integer, PrefMgr::FontScale);
  APPLY_VALUE (f_lock_toggle, Boolean, PrefMgr::BrowseLock);

  // Save map preferences.
  APPLY_VALUE (f_map_geo, WindowGeometry, PrefMgr::MapGeometry);
  APPLY_VALUE (f_update_toggle, Boolean, PrefMgr::MapAutoUpdate);
  map_mgr().set_auto_update(f_update_toggle.Set());

  // Save history preferences.
  ON_DEBUG (printf ("Section hist changed = %d\n", IS_CHANGED (f_nh_field)));
  if (IS_CHANGED (f_nh_field))
    global_history_mgr().set_max_length (CURRENT_VALUE (f_nh_field, Integer));
  APPLY_VALUE (f_nh_field, Integer, PrefMgr::NodeHistSize);
  ON_DEBUG (printf ("Search hist changed = %d\n", IS_CHANGED (f_nh_field)));
  if (IS_CHANGED (f_sh_field))
    search_mgr().set_history_length (CURRENT_VALUE (f_sh_field, Integer));
  APPLY_VALUE (f_sh_field, Integer, PrefMgr::SearchHistSize);

  // Save search preferences. 
  APPLY_VALUE (f_max_hits_field, Integer, PrefMgr::MaxSearchHits);
  APPLY_VALUE (f_adisplay_toggle, Boolean, PrefMgr::DisplayFirstHit);

  // Desensitize appropriate controls. 
  f_ok.SetSensitive (False);
  f_apply.SetSensitive (False);
  f_reset.SetSensitive (False);

  pref_mgr().sync();

  if (update_fs)
    {
      Wait_Cursor bob;
      style_sheet_mgr().font_preference_modified();
    }
}


// /////////////////////////////////////////////////////////////////
// reset
// /////////////////////////////////////////////////////////////////

void
PrefAgent::reset()
{
  // Reset browse preferences.
  if (IS_CHANGED (f_browse_geo))
    set_geometry (f_browse_geo, ORIGINAL_VALUE (f_browse_geo, WindowGeometry));
  if (IS_CHANGED (f_fs_field))
    set_integer (f_fs_field, ORIGINAL_VALUE (f_fs_field, Integer));
  if (IS_CHANGED (f_lock_toggle)) {
    set_lock (ORIGINAL_VALUE (f_lock_toggle, Boolean));
    f_lock_toggle.Unmap();
    f_lock_toggle.Map();
  }

  // Reset map preferences.
  if (IS_CHANGED (f_map_geo))
    set_geometry (f_map_geo, ORIGINAL_VALUE (f_map_geo, WindowGeometry));
  if (IS_CHANGED (f_update_toggle))
    set_update (ORIGINAL_VALUE (f_update_toggle, Boolean));

  // Reset history preferences.
  if (IS_CHANGED (f_nh_field))
    set_integer (f_nh_field, ORIGINAL_VALUE (f_nh_field, Integer));
  if (IS_CHANGED (f_sh_field))
    set_integer (f_sh_field, ORIGINAL_VALUE (f_sh_field, Integer));

  // Reset Search preferences. 
  if (IS_CHANGED (f_max_hits_field))
    set_integer (f_max_hits_field, ORIGINAL_VALUE (f_max_hits_field, Integer));
  if (IS_CHANGED (f_adisplay_toggle))
    set_auto_display (ORIGINAL_VALUE (f_adisplay_toggle, Boolean));

  // Desensitize appropriate controls. 
  f_ok.SetSensitive (False);
  f_apply.SetSensitive (False);
  f_reset.SetSensitive (False);
}

// /////////////////////////////////////////////////////////////////
// cancel
// /////////////////////////////////////////////////////////////////

void
PrefAgent::cancel()
{
  f_shell.Popdown();
}


// /////////////////////////////////////////////////////////////////
// init_values
// /////////////////////////////////////////////////////////////////

void
PrefAgent::init_values()
{
  PrefMgr &m = pref_mgr();
  
  ATTACH_VALUE (f_browse_geo, WindowGeometry,
		m.get_geometry (PrefMgr::BrowseGeometry));
  ATTACH_VALUE (f_fs_field, Integer, m.get_int (PrefMgr::FontScale));
  ATTACH_VALUE (f_lock_toggle, Boolean, m.get_boolean (PrefMgr::BrowseLock));
  
  ATTACH_VALUE (f_map_geo, WindowGeometry,
		m.get_geometry (PrefMgr::MapGeometry));
  ATTACH_VALUE (f_update_toggle, Boolean,
		m.get_boolean (PrefMgr::MapAutoUpdate));

  ATTACH_VALUE (f_nh_field, Integer, m.get_int (PrefMgr::NodeHistSize));
  ATTACH_VALUE (f_sh_field, Integer, m.get_int (PrefMgr::SearchHistSize));

  ATTACH_VALUE (f_max_hits_field, Integer, m.get_int (PrefMgr::MaxSearchHits));
  ATTACH_VALUE (f_adisplay_toggle, Boolean,
		m.get_boolean (PrefMgr::DisplayFirstHit));
}


// /////////////////////////////////////////////////////////////////
// something_changed
// /////////////////////////////////////////////////////////////////

void
PrefAgent::something_changed()
{
  f_ok.SetSensitive (True);
  f_apply.SetSensitive (True);
  f_reset.SetSensitive (True);
}


// /////////////////////////////////////////////////////////////////
// switch_to_window
// /////////////////////////////////////////////////////////////////

void
PrefAgent::switch_to_window (WCallback *wcb)
{
  Widget w = (Widget) wcb->ClientData();
  if (w == f_top_panel)
    return;

  XtManageChild (w);
  XRaiseWindow (XtDisplay (w), XtWindow (w));
  XtUnmanageChild (f_top_panel);
  f_top_panel = w;
}


// Keep track if timeout called.  If so, don't change on active callback. 
static int timeout_called;

// /////////////////////////////////////////////////////////////////
// change_cb - update callback for field arrow buttons
// /////////////////////////////////////////////////////////////////

void
PrefAgent::change_cb (WCallback *wcb)
{
  if (timeout_called == 0)
    change_field ((ChangeData *) wcb->ClientData());
  // Get rid of the timeout if it is active. 
  if (f_timeout)
    {
      delete f_timeout;
      f_timeout = NULL;
    }
}


// /////////////////////////////////////////////////////////////////
// arm_arrow - arm callback for field arrow buttons
// /////////////////////////////////////////////////////////////////

void
PrefAgent::arm_arrow (WCallback *wcb)
{
  // Add a timeout to keep changing the value.
  timeout_called = 0;
  f_timeout =
    new WTimeOut (window_system().app_context(), CHANGE_SPEED, this,
		  (WTimeOutFunc) &PrefAgent::change_timeout,
		  wcb->ClientData());
}


// /////////////////////////////////////////////////////////////////
// disarm_arrow - arm callback for field arrow buttons
// /////////////////////////////////////////////////////////////////

void
PrefAgent::disarm_arrow()
{
  delete f_timeout;
  f_timeout = NULL;
}


// /////////////////////////////////////////////////////////////////
// change_timeout
// /////////////////////////////////////////////////////////////////

void
PrefAgent::change_timeout (WTimeOut *wto)
{
  timeout_called++;
  // Change the value.
  bool changed = change_field ((ChangeData *) wto->ClientData());
  // Add the callback again if the limit hasn't been reached. 
  if (changed)
    {
      int speed = CHANGE_SPEED;
      // After three times double the speed. 
      if (timeout_called > 3)
	speed /= 2;
      f_timeout =
	new WTimeOut (window_system().app_context(), speed, this,
		      (WTimeOutFunc) &PrefAgent::change_timeout,
		      wto->ClientData());
    }
  else
    f_timeout = NULL;
}


// /////////////////////////////////////////////////////////////////
// change_field
// /////////////////////////////////////////////////////////////////

bool
PrefAgent::change_field (ChangeData *cd)
{
  // Get the current field value and add the change to it. 
  char *value = cd->field.Value();
  int amount = atoi (value);
  free (value);
  amount += cd->change;

  // reached the limit, need to make the arrow go insensitive
  if (amount == cd->limit)
  {
     // see if incrementing or decrementing
     if(cd->change < 0)
     {
       // user is decrementing--make down arrow insensitive
       XtSetSensitive(cd->dn_arrow, False);
     }
     else
     {
       // user is incrementing--make down arrow insensitive
       XtSetSensitive(cd->up_arrow, False);
     }
  }
  else
  {
     // see if incrementing or decrementing
     if(cd->change < 0)
     {
       // user is decrementing--make up arrow sensitive if it is
       // insensitive
       if(!XtIsSensitive(cd->up_arrow))
         XtSetSensitive(cd->up_arrow, True);
     }
     else
     {
       // user is incrementing--make down arrow sensitive if it is
       // insensitive
       if(!XtIsSensitive(cd->dn_arrow))
         XtSetSensitive(cd->dn_arrow, True);
     }
  }

  // Make sure the change won't exceed any limits.
  // If change is negative, then limit is a lower limit (and vice-versa).
  if ((cd->change < 0 && amount < cd->limit) ||
      (cd->change > 0 && amount > cd->limit))
    {
      // Sorry, Charlie. 
      window_system().beep();
      return (FALSE);
    }

  // Update the field to display the new value.
  set_integer (cd->field, amount);

  return (TRUE);
}


// /////////////////////////////////////////////////////////////////
// lock_toggle
// /////////////////////////////////////////////////////////////////

void
PrefAgent::lock_toggle (WCallback *wcb)
{
  XmToggleButtonCallbackStruct &cbs =
    *(XmToggleButtonCallbackStruct*) wcb->CallData();

  set_lock (cbs.set);
}


// /////////////////////////////////////////////////////////////////
// update_toggle
// /////////////////////////////////////////////////////////////////

void
PrefAgent::update_toggle (WCallback *wcb)
{
  XmToggleButtonCallbackStruct &cbs =
    *(XmToggleButtonCallbackStruct*) wcb->CallData();

  set_update (cbs.set);
}


// /////////////////////////////////////////////////////////////////
// set_lock
// /////////////////////////////////////////////////////////////////

void
PrefAgent::set_lock (bool set)
{
    if (set) {
	f_lock_toggle.WObject::Set(WArgList(XmNselectPixmap,
					    window_system().locked_pixmap(f_lock_toggle.Parent()),
					    XmNlabelPixmap,
					    window_system().semilocked_pixmap(f_lock_toggle.Parent()),
					    NULL));
    }
    else {
	f_lock_toggle.WObject::Set(WArgList(XmNselectPixmap,
					    window_system().semilocked_pixmap(f_lock_toggle.Parent()),
					    XmNlabelPixmap,
					    window_system().unlocked_pixmap(f_lock_toggle.Parent()),
					    NULL));
    }
    
    f_lock_toggle.Set (set);
    SET_VALUE (f_lock_toggle, Boolean, set);
}  


// /////////////////////////////////////////////////////////////////
// set_update
// /////////////////////////////////////////////////////////////////

void
PrefAgent::set_update (bool set)
{
  XmStringLocalized mtfstring =
	CATGETS(Set_AgentLabel, set? 203 : 204, set ? "On" : "Off");

  XtVaSetValues(f_update_toggle, XmNlabelString, (XmString)mtfstring, NULL);

  f_update_toggle.Set (set);
  SET_VALUE (f_update_toggle, Boolean, set);
}


// /////////////////////////////////////////////////////////////////
// set_geometry
// /////////////////////////////////////////////////////////////////

void
PrefAgent::set_geometry (WXmTextField &field, WindowGeometry &wg)
{
  static char buf[16];
  sprintf (buf, "%dx%d", wg.width, wg.height);
  field.Value (buf);
  SET_VALUE (field, WindowGeometry, wg);
}


// /////////////////////////////////////////////////////////////////
// set_integer
// /////////////////////////////////////////////////////////////////

void
PrefAgent::set_integer (WXmTextField &field, int i)
{
  static char buf[16];
  sprintf (buf, "%d", i);
  field.Value (buf);
  SET_VALUE (field, Integer, i);
}


// /////////////////////////////////////////////////////////////////
// adisplay_toggle
// /////////////////////////////////////////////////////////////////

void
PrefAgent::adisplay_toggle (WCallback *wcb)
{
  XmToggleButtonCallbackStruct &cbs =
    *(XmToggleButtonCallbackStruct*) wcb->CallData();

  set_auto_display (cbs.set);
}


// /////////////////////////////////////////////////////////////////
// set_auto_display
// /////////////////////////////////////////////////////////////////

void
PrefAgent::set_auto_display (bool set)
{
  XmStringLocalized mtfstring =
	CATGETS(Set_AgentLabel, set? 203 : 204, set ? "On" : "Off");

  XtVaSetValues(f_adisplay_toggle, XmNlabelString, (XmString)mtfstring, NULL);

  f_adisplay_toggle.Set (set);
  SET_VALUE (f_adisplay_toggle, Boolean, set);
}

#ifdef UseWideChars
// /////////////////////////////////////////////////////////////////
// hyphen_toggle - 4/11/94 kamiya
// This will be obsolete because toggle is not adequate for selecting
// hyphenation processing method - 4/11/94 kamiya
// /////////////////////////////////////////////////////////////////
void
PrefAgent::hyphen_toggle (WCallback *)
{
// obsolete, remove this method right away.
}

// /////////////////////////////////////////////////////////////////
// set_hyphen - 4/11/94 kamiya
// /////////////////////////////////////////////////////////////////
void
PrefAgent::set_hyphen (XtPointer)
{
// obsolete, remove this method right away.
}
#endif

// /////////////////////////////////////////////////////////////////
// get_geometry - interactivly get a window geometry
// /////////////////////////////////////////////////////////////////

void
PrefAgent::get_geometry (WCallback *wcb)
{
  WXmTextField text_field ((Widget)wcb->ClientData());
  char *shell_name;
  if (text_field == f_map_geo)
    shell_name = (char*)"map";
  else if (text_field == f_browse_geo)
    shell_name = (char*)"nodeview";
    
  // Display an instructional dialog. 
  WXmMessageDialog
    info_dialog ((Widget)f_shell, (char*)"geometry_msg",
		 WArgList (XmNdialogType, XmDIALOG_INFORMATION, NULL));
  WXmDialogShell shell (info_dialog.Parent());
  // set window title
  String string = CATGETS(Set_PrefAgent, 2, "Dtinfo: Get Geometry");
  XtVaSetValues((Widget)shell, XmNtitle, string, NULL);
  //  window_system().register_full_modal_shell (&shell);
  info_dialog.MessageString (
	(char*)UAS_String(CATGETS(Set_Messages, 16, "File a Bug")));
  XtUnmanageChild (info_dialog.OkPB());
  XtUnmanageChild (info_dialog.HelpPB());

  XtVaSetValues((Widget)info_dialog, XmNdefaultPosition, False, NULL);

  // Wait for the dialog to appear. 
  int mapped = FALSE;
  info_dialog.Manage();
  XtAddEventHandler (info_dialog, ExposureMask, False,
		     dialog_mapped, (XtPointer) &mapped);
  XtAppContext app_context = window_system().app_context();
  XEvent event;
  while (!mapped)
    {
      XtAppNextEvent (app_context, &event);
      XtDispatchEvent (&event);
    }

  // Let the user pick an OLIAS window. 
  static Cursor pick_cursor =
    XCreateFontCursor (window_system().display(), XC_crosshair);
  Widget target;
  target = XmTrackingEvent (f_shell, pick_cursor, False, &event);

  // Get the geometry of the selected window.
  while (target != NULL && !XtIsShell (target))
    target = XtParent (target);

  if (target != (Widget) info_dialog.Parent())
    {
      // Make sure the user clicked on the right kind of window.
      if (target != NULL && strcmp (XtName(target), shell_name) != 0)
	target = NULL;

      if (target == NULL)
	{
	  XBell (window_system().display(), 100);
	}
      else
	{
	  WShell shell (target);
	  WindowGeometry wg;
	  wg.width = shell.Width();
	  wg.height = shell.Height();
	  set_geometry (text_field, wg);
	}
    }

  // Nuke the dialog.
  info_dialog.Unmanage();
  XtDestroyWidget (info_dialog.Parent());
}


// /////////////////////////////////////////////////////////////////
// dialog_mapped - wait for dialog to appear on-screen
// /////////////////////////////////////////////////////////////////

void
PrefAgent::dialog_mapped (Widget w, XtPointer client_data,
			  XEvent *, Boolean *)
{
  XmUpdateDisplay (w);

  *((int *) client_data) = TRUE;
}
