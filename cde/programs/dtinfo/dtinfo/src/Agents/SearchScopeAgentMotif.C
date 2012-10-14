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
/* $TOG: SearchScopeAgentMotif.C /main/27 1998/04/17 11:35:37 mgreess $ */
/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

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

#include "UAS.hh"
#include "Support/UtilityFunctions.hh"

#define C_EnvMgr
#define C_LibraryMgr
#define C_MessageMgr
#define C_SearchMgr
#define C_SearchScopeMgr
#define L_Managers

#define C_WindowSystem
#define L_Other

#define C_OutlineList
#define C_OutlineString
#define C_InfoBase
#define C_TOC_Element
#define L_Basic

#define C_xList
#define L_Support

#define C_StringPref
#define L_Preferences

#define C_HelpAgent
#define C_OutlineListView
#define C_SearchScopeList
#define C_SearchScopeAgent
#define C_ScopePopup
#define L_Agents

#define C_QueryEditor
#define L_Query

#include "Other/XmStringLocalized.hh"
#include "Managers/CatMgr.hh"

#include "Exceptions.hh"
#include "Prelude.h"

#include "Registration.hh"

#include <WWL/WXmForm.h>
#include <WWL/WXmLabel.h>
#include <WWL/WXmPushButton.h>
#include <WWL/WXmRowColumn.h>
#include <WWL/WXmCascadeButton.h>
#include <WWL/WXmSeparator.h>
#include <WWL/WXmMenu.h>

#include <WWL/WXmDialogShell.h>
#include <WWL/WXmTextField.h>
#include <WWL/WXmMessageBox.h>

#ifdef SVR4
#ifndef USL
#include <libintl.h>        // 1/19/94 yuji
#endif
#endif

#include <string.h>
#include <iostream>
using namespace std;
#include <stdio.h>

#define CLASS SearchScopeAgent
STATIC_SENDER_CC (UpdateMenu);



#if defined(i386) || defined(_IBMR2)

extern "C" 
{
    extern int strcasecmp(const char *s1, const char *s2);
}

#endif

#ifdef USL

strcasecmp(register const char *s1,
	   register const char *s2)
{
    register int c1, c2;

    while (*s1 && *s2) {
	c1 = isupper(*s1) ? tolower(*s1) : *s1;
	c2 = isupper(*s2) ? tolower(*s2) : *s2;
	if (c1 != c2)
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}

#endif

#define CLASS SearchScopeAgent
#include "create_macros.hh"

// /////////////////////////////////////////////////////////////////////////
// class ScopeOutlineListView
//
//  This is a subclass of OutlineListView
//  The difference between this class and the parent class is that this class
//  will deselect all parents and children of a selected item. This
//  functionality is to show that a selected item includes its children
//    -11:22 07/ 8/93 - jbm  
// /////////////////////////////////////////////////////////////////////////


class ScopeOutlineListView : public OutlineListView
{
public:
  ScopeOutlineListView(const WComposite &parent,
                       const char *name,
                       bool automanage = FALSE,
                       bool enable_activate = FALSE)
    : OutlineListView(parent, name, automanage, enable_activate)
    {}
  virtual void select(WCallback *wcb);
  virtual void clear();


  OutlineElement *find_parent(int, OutlineElement *);

  void unset_parent(OutlineElement *p);
  int lid_to_index(const UAS_String &lid);
  void select_infolib (UAS_String &lid, BitHandle handle);
  UAS_String name_to_lid(UAS_String& name);
  UAS_String lid_to_name(UAS_String& lid);

protected:
  
  OutlineElement *find_parent(OutlineElement *possible_parent,
                              OutlineElement *child);
 
};

void
ScopeOutlineListView::clear()
{
  // empty list like usual
  OutlineListView::clear();

  // set first element
  ((OutlineElement *)(*list())[0])->set_selected(data_handle());
  // NOTE: slow, set directly ???
  u_int starting_pos = 1;
  update_highlighting(list(), starting_pos);
}

void
ScopeOutlineListView::unset_parent(OutlineElement *parent)
{
  parent->unset_selected(data_handle());
}

void
ScopeOutlineListView::select(WCallback *wcb)
{
  bool update = FALSE ;

  // do selection work in base class 
  OutlineListView::select(wcb);

  // now do some special handling 

  if (f_selected_item_count == 0)
    {
      clear();
    } else {

      // first get item that was selected 
      XmStringTable items = Items();
      XmListCallbackStruct *lcs = (XmListCallbackStruct *)wcb->CallData();
      OutlineElement *oe ;

#ifdef JBM
      extract_pointer(items[lcs->item_position-1], &oe);
#else
      oe = item_at(lcs->item_position-1);
#endif

      // we want to deselect all children 
      if (oe->has_children() && oe->children_cached())
        {
          oe->children()->deselect_all(data_handle());
          update = TRUE ;
        }

      // now de-select parent (if have one)
      OutlineElement *parent;
      if (strcmp("infobases", XtName(wcb->GetWidget())) == 0)
        parent = find_parent(1, oe);
      else
        parent = find_parent(0, oe);

      if (parent != NULL)
        {
          parent->unset_selected(data_handle());
          update = TRUE ;
        }

      if (update)
        {
          u_int starting_pos = 1;
          update_highlighting(list(), starting_pos);
        }
    }
}

OutlineElement *
ScopeOutlineListView::find_parent(int infolib, OutlineElement *element)
{
  OutlineElement *top;

  if (infolib)
  {
    UAS_Pointer<UAS_Common> ilib = ((TOC_Element*)element)->toc();

    // need to find out which infolib we're dealing with
    int infolib_index = lid_to_index(ilib->lid());
    top = (OutlineElement*)(*list())[infolib_index] ;
  }
  else
    top = (OutlineElement*)(*list())[0] ;

  return find_parent(top, element);

}

OutlineElement *
ScopeOutlineListView::find_parent(OutlineElement *parent,
                                  OutlineElement *element)
{
  OutlineElement *current, *value ;

  if (parent->has_children() && parent->children_cached())
    {
      OutlineList *olist = parent->children() ;
      for (int i = 0 ; i < olist->length(); i++)
        {
          current = (OutlineElement*)(*olist)[i] ;
          if (current == element)
            return parent ;     // NOTE: procedure EXIT 

          value = find_parent(current, element);
          if (value != NULL)
            {
              // NOTE: this action should not really be here
		((*this).unset_parent)(parent);
              return value ;    // NOTE: procedure EXIT 
            }
        }
    }
  return NULL ;
}

void
ScopeOutlineListView::select_infolib (UAS_String &lid, BitHandle handle)
{
  int index = lid_to_index(lid);
  OutlineElement *oe = ((OutlineElement*)(*list())[index]);
  //((OutlineElement*)(*f_infolib_list->list())[index])->set_selected (handle);

  // select infolib
  oe->set_selected (handle);

  // deselect all children
  oe->children()->deselect_all(handle);

  // udpate highlighting
  u_int starting_pos = 1;
  update_highlighting(list(), starting_pos);
}


// Give a infolib id, return its index into the f_infolib_list
// list.
int
ScopeOutlineListView::lid_to_index(const UAS_String &lid)
{
  OutlineList *ol = list();
  OutlineElement *oe;
  for (int i = 0; i < ol->length(); i++)
  {
    oe = ((OutlineElement *) (*ol)[i]);
    UAS_Pointer<UAS_Common> infolib = ((TOC_Element *)oe)->toc();
    if( infolib->lid() == lid)
      return i;
  }

  return -1; // lid not found
}

UAS_String
ScopeOutlineListView::lid_to_name(UAS_String &lid)
{
  OutlineList *ol = list();
  OutlineElement *oe;
  for (int i = 0; i < ol->length(); i++)
  {
    oe = ((OutlineElement *) (*ol)[i]);
    UAS_Pointer<UAS_Common> infolib = ((TOC_Element *)oe)->toc();
    if( infolib->lid() == lid)
      return infolib->name();
  }
  // There has to be an infolib for a given lid--if
  // not, something has gone wrong.
  throw (CASTEXCEPT Exception());
}

UAS_String
ScopeOutlineListView::name_to_lid(UAS_String &name)
{
  OutlineList *ol = list();
  OutlineElement *oe;
  for (int i = 0; i < ol->length(); i++)
  {
    oe = ((OutlineElement *) (*ol)[i]);
    UAS_Pointer<UAS_Common> infolib = ((TOC_Element *)oe)->toc();
    if( infolib->name() == name)
      return infolib->lid();
  }
  // There has to be an infolib for a given lid--if
  // not, something has gone wrong.
  throw (CASTEXCEPT Exception());
}

// **********************************************************************
// **********************************************************************

SearchScopeAgent::SearchScopeAgent()
: f_shell (NULL),
  f_scope_list (this),
  f_first_time (True),
  f_option_menu (NULL),
  f_current_scope (NULL)
{
  f_auto_expand = True;
  create_ui();
  f_scope_list.restore();
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

SearchScopeAgent::~SearchScopeAgent()
{
#if 0
  // dont need to delete these because not called anyway - jbm

  // NOTE: Destroy should destroy & delete! 
  f_shell->Destroy();
  delete f_infolib_list;
  delete f_component_list;
  // LEAK: need to delete the scope objects 
  delete f_scope_list;

#endif
}
  

// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
SearchScopeAgent::display()
{
  Wait_Cursor bob;
  if (f_shell == 0)
    {
      create_ui();
    }
  if (f_first_time)
    {
      f_first_time = False;
      fill_option_menu();
    }

  select_initial();

  f_shell.Popup();
  f_shell.DeIconify();

  if (search_scope_mgr().show_warning())
  {
    // dialog is not real useful in a multi-infolib environment
    if (env().debug())
    {
       message_mgr().warning_dialog (
        (char*)UAS_String(CATGETS(Set_Messages, 31,
        "Ignoring invalid scopes in your profile.")));
    }
    search_scope_mgr().show_warning(False);
  }

}

// /////////////////////////////////////////////////////////////////
// fill_option_menu - fills the option menu with named scopes
// /////////////////////////////////////////////////////////////////
void
SearchScopeAgent::fill_option_menu()
{
  int position = 1; // skip past unnamed scope button
  List_Iterator<UAS_SearchScope *> i (f_scope_list);
  WComposite menu (f_scope_option.SubMenuId());

  ON_DEBUG (printf ("There are %d user scopes:\n", f_scope_list.length()));
  for (; i != 0; i++)
  {
    if (i.item()->read_only())
      continue;
    //DECLM (WXmPushButton, scope_btn, f_scope_option.SubMenuId(),
           //i.item()->name());
    DECLM (WXmPushButton, scope_btn, menu, i.item()->name());
    ON_ACTIVATE (scope_btn,select_scope);
    scope_btn.UserData (i.item());
    scope_btn.PositionIndex (position++);
  }
  f_first_time = False;
}

// /////////////////////////////////////////////////////////////////
// create_ui
// /////////////////////////////////////////////////////////////////

void
SearchScopeAgent::create_ui()
{
  XmStringLocalized mtfstring;
  String	    string;

  f_shell = WTopLevelShell (window_system().toplevel(),WPopup,"scope_editor");
  window_system().register_shell(&f_shell);

  string = CATGETS(Set_SearchScopeAgent, 1, "Dtinfo: Search Scope Editor");
  XtVaSetValues((Widget)f_shell, XmNtitle, string, NULL);

  DECL  (WXmForm,         form,           f_shell,     "form");
  DECLC (WXmPulldownMenu, scope_menu,     form,        "scope_menu");
  Arg args[1];
  int n = 0;
  XtSetArg(args[n], XmNsubMenuId, (Widget) scope_menu); n++;
  f_scope_option = WXmOptionMenu (form, (char*)"scope_option", WAutoManage, args, n);
  ASSN  (WXmPushButton,   f_unnamed,      scope_menu,  "unnamed");

  mtfstring = CATGETS(Set_AgentLabel, 212, "Scope Name");
  XtVaSetValues(f_scope_option, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 213, "Unnamed");
  XtVaSetValues(f_unnamed, XmNlabelString, (XmString)mtfstring, NULL);
  
  ASSNM (WXmPushButton,   f_new,          form,        "new");
  ASSNM (WXmPushButton,   f_save,         form,        "save");
  ASSNM (WXmPushButton,   f_rename,       form,        "rename");
  ASSNM (WXmPushButton,   f_delete,       form,        "delete");
  ASSNM (WXmPushButton,   f_reset,        form,        "reset");
  DECLM (WXmPushButton,   close,          form,        "close");
  DECLM (WXmPushButton,   help,           form,        "help");

  DECLM (WXmSeparator,     separator,     form,        "separator");

  DECLM (WXmLabel,         bookshelf,     form,        "books");
  DECLM (WXmLabel,         components,    form,        "components");

  mtfstring = CATGETS(Set_AgentLabel, 214, "Books");
  XtVaSetValues(bookshelf, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 215, "Components");
  XtVaSetValues(components, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 216, "New");
  XtVaSetValues(f_new, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 217, "Save");
  XtVaSetValues(f_save, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 218, "Rename");
  XtVaSetValues(f_rename, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 183, "Delete");
  XtVaSetValues(f_delete, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 202, "Reset");
  XtVaSetValues(f_reset, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 12, "Close");
  XtVaSetValues(close, XmNlabelString, (XmString)mtfstring, NULL);
  mtfstring = CATGETS(Set_AgentLabel, 48, "Help");
  XtVaSetValues(help, XmNlabelString, (XmString)mtfstring, NULL);

  // WARNING: the widget name, infobases, is used else where--do not alter.
  f_infolib_list = new ScopeOutlineListView (form, "infobases", WAutoManage);
  f_infolib_list->max_level (2);

  f_component_list = new ScopeOutlineListView (form, "components",WAutoManage);

  ON_ACTIVATE (f_new,new_scope);
  ON_ACTIVATE (f_save,save_scope);
  ON_ACTIVATE (f_rename,rename_scope);
  ON_ACTIVATE (f_delete,delete_scope);
  ON_ACTIVATE (f_reset,reset);
  ON_ACTIVATE (close,close);
  help_agent().add_activate_help (help, (char*)"scope_editor_help");
  
  form.ShadowThickness (0);
  form.Manage();

  // set up callback when item selected from list 
  Observe (f_infolib_list, OutlineListView::ENTRY_SELECTED,
           &SearchScopeAgent::scope_modified);
  
  Observe(f_component_list, OutlineListView::ENTRY_SELECTED,
          &SearchScopeAgent::scope_modified);

  // NOTE: May want to create dialogs on the fly.
  // Types: prompt: save & rename, yes/no: delete & close

  /* -------- Fill in lists -------- */

#if 0
  OutlineList *ol = new OutlineList (1);
  // NOTE: hack ahead, direct reference to mmdb for wedged searching: 
  ol->append (new TOC_Element (new TOC_mmdb_lib (the_mmdb())));
#else
#if 0
  UAS_Pointer<UAS_Common> d = UAS_Common::create ("mmdb:/t/");
  UAS_Pointer<UAS_Collection> cd = (UAS_Collection *) ((UAS_Common *) d);
  ol->append (new TOC_Element (cd->root()));
#else
  UAS_List<UAS_String> rootList = UAS_Common::rootLocators ();
  OutlineList *ol = new OutlineList(rootList.length());
  for (int i = 0; i < rootList.length(); i ++) {
      UAS_Pointer<UAS_Common> d = UAS_Common::create(*(UAS_String *)rootList[i]);
      UAS_Pointer<UAS_Collection> cd = (UAS_Collection *) ((UAS_Common *) d);
      ol->append (new TOC_Element (cd->root()));
  }


#endif
#endif
  BitHandle handle = ol->get_data_handle();
  ((OutlineElement *) (*ol)[0])->set_expanded (handle);
  f_infolib_list->set_list (ol, handle);
  f_infolib_list->clear();

  // Generate a components list 
  OutlineList *component_list = generate_component_list();
  BitHandle component_handle = component_list->get_data_handle();
  ((OutlineElement*)(*component_list)[0])->set_expanded (component_handle);
  f_component_list->set_list (component_list, component_handle);
  f_component_list->clear();

  ON_DEBUG (printf ("IB handle = %ld, CP handle = %ld\n",
                    handle, component_handle));
}


// /////////////////////////////////////////////////////////////////
// new_scppe - create a new scope 
// /////////////////////////////////////////////////////////////////

void
SearchScopeAgent::new_scope()
{
  if (f_reset.Sensitive())
    {
      bool dosave = message_mgr().question_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 17,
			"Do you want to save changes to the current scope?")));
      if (dosave)
        save_scope();
    }

  if (! XtIsManaged((Widget)f_unnamed)) {
      f_unnamed.Manage();
      // re-computation needed, weird - 11/2/94 kamiya
      f_scope_option.Unmanage();
      f_scope_option.Manage();
  }
  f_scope_option.MenuHistory (f_unnamed);

  f_infolib_list->clear();
  f_component_list->clear();

  ON_DEBUG (printf("new_scope: handle = %ld\n",f_infolib_list->data_handle()));

  f_new.SetSensitive (False);
  f_save.SetSensitive (True);
  f_rename.SetSensitive (False);
  f_delete.SetSensitive (False);
  f_reset.SetSensitive (False);

  f_current_scope = NULL;
}


// /////////////////////////////////////////////////////////////////
// scope_name_prompt - prompt for a scope name and validate it
// /////////////////////////////////////////////////////////////////

const char *
SearchScopeAgent::scope_name_prompt()
{
  char *default_name = new char [31];
  int   default_name_len = 20 ;
  const char *scope_name = "";
  bool valid = FALSE;

  // Input validation loop. 
  do
    {
      int len = strlen(scope_name);
      if (len > default_name_len)
        {
          delete [] default_name ;
          default_name = new char[len + 1] ;
          default_name_len = len ;
        }
      strcpy (default_name, scope_name);
      message_mgr().set_max_length(default_name_len);
      scope_name = message_mgr().get_string(
	(char*)UAS_String(CATGETS(Set_SearchScopeAgent, 2,
					"Enter the scope name to save as:")),
	(char*)UAS_String(CATGETS(Set_SearchScopeAgent, 3,
					"Dtinfo: Save Scope")),
	default_name);

      // look for canceled operation 
      if (scope_name == NULL)
        {
          delete [] default_name ;
          message_mgr().set_max_length(40);
          return (NULL);
        }

      scope_name = truncate ((char *) scope_name);
      // look for bogus characters 
      if (strpbrk (scope_name, ":,;") != NULL)
        {
          message_mgr().error_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 18,
		"You cannot use a colon, comma or semicolon in a search scope name.")));
        }
      // look for empty scope name 
      else if (*scope_name == '\0')
        {
          message_mgr().error_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 19,
				"Please choose a non-empty scope name.")));
        }
      // look for name too long
      else if (strlen (scope_name) > 20)
        {
          // this dialog should never popup, since we don't allow
          // the user to enter more than the max. But, since the
          // max char length has changed to 20, the message should
          // be changed to reflect that--or just removed.
          message_mgr().error_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 20,
		"A search scope name may not exceed 30 characters.")));
        }
      // look for duplicate scope name 
      else
        {
          // Check for name in use.
          List_Iterator<UAS_SearchScope *> s (f_scope_list);
          if (strcasecmp (scope_name, "unnamed") != 0)
            for (; s != 0; s++)
              if (strcmp (scope_name, s.item()->name()) == 0)
                break;
          if (s != 0)
            {
              message_mgr().error_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 21,
		"The name you entered is already in use.")));
            }
          else
            valid = TRUE;
        }
    }
  while (!valid);

  delete [] default_name;
  message_mgr().set_max_length(40);
  return (scope_name);
}


// /////////////////////////////////////////////////////////////////
// save_scope - save the current scope, prompting if necessary
// /////////////////////////////////////////////////////////////////

void
SearchScopeAgent::save_scope()
{
  // get a name and create a new scope 
  const char *name;
  if (f_current_scope == NULL)  // ie: unnamed 
    {
      name = scope_name_prompt();
      if (name == NULL)
        return;

      save_unnamed_scope (name);
      if (XtIsManaged(f_unnamed)) {
        f_unnamed.Unmanage();
        // re-computation needed, weird - 11/2/94 kamiya
        f_scope_option.Unmanage();
        f_scope_option.Manage();
      }

      f_new.SetSensitive (True);
      f_rename.SetSensitive (True);
      f_delete.SetSensitive (True);
    }
  // just save it otherwise 
  else
    {
      BitHandle data_handle = f_scope_list.lookup_handle(f_current_scope);
      BitHandle visible_handle = f_infolib_list->data_handle();
  
      // save the bits from the applied handle
      ON_DEBUG (printf("Copy handle %ld to %ld\n", visible_handle, data_handle));
      f_infolib_list->list()->copy_selected (visible_handle, data_handle);
      if(f_auto_expand)
        f_infolib_list->list()->copy_expanded (visible_handle, data_handle);
      f_component_list->list()->copy_selected (visible_handle, data_handle);

      // Update the component mask value in the scope. 
      f_current_scope->search_zones().zones(component_mask (data_handle));

      UAS_PtrList<UAS_BookcaseEntry>bcases = bookcase_list(data_handle);
      f_current_scope->bookcases(bcases);

      // Save the scope to disk. 
      f_scope_list.save();
    }
  f_save.SetSensitive (False);
  f_reset.SetSensitive (False);
}


// /////////////////////////////////////////////////////////////////
// create_scope - create a new scope given various info
// /////////////////////////////////////////////////////////////////

UAS_SearchScope *
SearchScopeAgent::create_scope (const char *name,
                                UAS_PtrList<UAS_BookcaseEntry> &base_list,
                                unsigned int component_mask, bool ro)
{
  // this flag should be defined elsewhere-this could
  // become a command-line option
  if (f_shell == 0)
    {
      create_ui();
    }

  // Get a new bit handle for the new scope. 
  BitHandle handle = f_infolib_list->list()->get_data_handle();

  // Create the new scope object. 
  UAS_SearchScope *scope = new UAS_SearchScope (name, base_list, component_mask, ro);
  // append handle and scope to scope list
  if(scope->read_only())
  {
    List_Iterator<UAS_SearchScope *> s (f_scope_list);
    s++;
    // put all read only scopes at beginning, but
    // after the "Current Section" and "All Libraries" scopes
    // which should always be the first 2 on the list.
    f_scope_list.insert_after (s, scope);
    f_scope_list.add_handle(handle, 1);
  }
  else
  {
    f_scope_list.append (scope);
    f_scope_list.add_handle(handle, -1);
  }


  // Select the infobase level if the scope contains no infolibs.
  if (base_list.numItems() == 0)
    ((OutlineElement*)(*f_infolib_list->list())[0])->set_selected (handle);

  UAS_BookcaseEntry *bce;
  for (int i = 0; i < base_list.numItems(); i++)
    {
      bce = base_list[i];

      // Select the correct entries in the list.
      int index = lid_to_index(bce->lid());
      UAS_String bce_lid(bce->lid());
      UAS_String bce_bid(bce->bid());
      int bcindex = bid_to_index(bce_lid, bce_bid);
      List *iblist = 
	((OutlineElement*)(*f_infolib_list->list())[index])->children();

      ON_DEBUG (printf ("Using base %d of %d\n",
                        bce->base_num(), iblist->length()));

      UAS_ObjList<int> book_list = bce->book_list();
      if (book_list.numItems() == 0)
      {
        ((OutlineElement*)(*iblist)[bcindex])->set_selected (handle);
//        ((OutlineElement*)(*iblist)[bce->base_num()])->set_selected (handle);

        // since the bookcase is selected, need to expand parent
        if(f_auto_expand)
        {
          ((OutlineElement*)(*f_infolib_list->list())[index])->set_expanded (handle);
        }
      }
      else
        {
          List *books =
            ((OutlineElement*)(*iblist)[bcindex])->children();
//            ((OutlineElement*)(*iblist)[bce->base_num()])->children();

          if (f_auto_expand)
          {
            // need to expand bookcase and infolib
            if(book_list.numItems() > 0)
            {
              ((OutlineElement*)(*f_infolib_list->list())[index])->set_expanded (handle);
              ((OutlineElement*)(*iblist)[bcindex])->set_expanded (handle);
            }
          }
          for (int j = 0; j < book_list.numItems(); j++)
            ((OutlineElement*)(*books)[book_list[j]-1])->set_selected (handle);
        }
      //book_list.clear(); // old code reset this list... but why? -rtp
    }

  return (scope);
}

void
SearchScopeAgent::select_infolib (UAS_String &lid, BitHandle handle)
{
  ((ScopeOutlineListView *)f_infolib_list)->select_infolib(lid, handle);
}

// /////////////////////////////////////////////////////////////////
// select_initial - select the first scope and display it
// /////////////////////////////////////////////////////////////////

void
SearchScopeAgent::select_initial (Widget exception)
{
  // Select the Unnamed scope if it is the only (non-exception) one left.
  WComposite menu (f_scope_option.SubMenuId());

  int num_kids = menu.NumChildren();
  if ((exception != NULL  && num_kids == 2) ||  num_kids == 1)
    {
      f_reset.SetSensitive (False);
      new_scope();
    }
  // Select the first (non-exception) scope in the list otherwise. 
  else
    {
      WXmPushButton btn (menu.Children()[1]);
      if (btn == exception)
        btn = WXmPushButton (menu.Children()[2]);

      set_current_scope(btn);
#if EAM
      f_scope_option.MenuHistory (btn);
      f_current_scope = (UAS_SearchScope *) btn.UserData();
      f_infolib_list->list()->copy_selected(f_scope_list.lookup_handle(f_current_scope),
                                             f_infolib_list->data_handle());
      if (f_auto_expand)
      {
        f_infolib_list->list()->copy_expanded(f_scope_list.lookup_handle(f_current_scope),
                                             f_infolib_list->data_handle());
        f_infolib_list->update_list(f_infolib_list->list(),
                                    f_infolib_list->data_handle());
      }
      set_components (f_current_scope->search_zones().zones());
      // visually update 
      f_infolib_list->data_handle (f_infolib_list->data_handle());
      f_component_list->data_handle (f_component_list->data_handle());

      f_new.SetSensitive (True);
      f_save.SetSensitive (False);
      f_delete.SetSensitive (True);
      f_rename.SetSensitive (True);
      f_reset.SetSensitive (False);
#endif
    }
}


// /////////////////////////////////////////////////////////////////
// rename - rename the current scope
// /////////////////////////////////////////////////////////////////

void
SearchScopeAgent::rename_scope()
{
  WXmPushButton scope_btn (f_scope_option.MenuHistory());
  // Rename must be insensitive for Unnamed scope.  Use save instead.
  Xassert (scope_btn != f_unnamed);
  Xassert (f_current_scope != NULL);
  Xassert (f_current_scope == (UAS_SearchScope *) scope_btn.UserData());

  const char *name = scope_name_prompt();
  if (name == NULL)
    return;

  scope_btn.LabelString (name);

  f_save.SetSensitive (False);
  f_reset.SetSensitive (False);

  int position = 1, old_position = -1;
  List_Iterator<UAS_SearchScope *> s (f_scope_list);
  for (; s != 0; s++)
    {
      if (s.item()->read_only())
        continue;
      if (s.item() == f_current_scope)
        {
          old_position = position;
          continue;
        }
      ON_DEBUG (printf ("ScopeAgent: strcmp against <%s>\n",
                        s.item()->name()));
      if (strcmp (name, s.item()->name()) < 0)
        break;
      position++;
    }

  // Move the item to the correct alphabetical placement in the list.
  ON_DEBUG (printf ("ScopeAgent: old position = %d, new position = %d\n",
                    old_position, position));
  if (position != old_position)
    {
      // look up handle for scope and save it
      BitHandle data_handle = f_scope_list.lookup_handle(f_current_scope);
      // remove handle and scope from scope list
      f_scope_list.remove_handle(f_current_scope);
      f_scope_list.remove (f_current_scope);
      ON_DEBUG (printf ("ScopeAgent: Inserting before <%s>\n",
                        s != 0 ? s.item()->name() : "LAST"));
      // add handle and scope back in appropriate location
      f_scope_list.add_handle(data_handle, position-1);
      f_scope_list.insert_before (s, f_current_scope);
  
      scope_btn.PositionIndex (position);
    }

  // Make sure old scope name preference isn't saved by NULLing out
  // it's value.  This should really happen in a ScopeListMgr object.  DJB
  char scratch[64];
  sprintf (scratch, "Scope.%s", f_current_scope->name());
  StringPref (scratch).value ("");
  f_current_scope->set_name (name);
  f_scope_list.save();
}


// /////////////////////////////////////////////////////////////////////////
// reset - resets window to values at last save
// /////////////////////////////////////////////////////////////////////////

void
SearchScopeAgent::reset()
{
  WXmPushButton scope_btn (f_scope_option.MenuHistory());
  // Handle Unnamed scope as a special case.
  if (scope_btn == f_unnamed)
    {
      f_infolib_list->clear();
      f_component_list->clear();
    }
  else
    {
      UAS_SearchScope *scope = (UAS_SearchScope *) scope_btn.UserData();
      f_infolib_list->list()->copy_selected(f_scope_list.lookup_handle(scope),
                                             f_infolib_list->data_handle());
//      f_infolib_list->list()->copy_expanded(f_scope_list.lookup_handle(scope),
//                                             f_infolib_list->data_handle());
      set_components (scope->search_zones().zones());

      // update visually by resetting flag (to itself)
      f_infolib_list->data_handle(f_infolib_list->data_handle());
      f_component_list->data_handle(f_component_list->data_handle());
  }

  if (scope_btn != f_unnamed)
    f_save.SetSensitive (False);
  f_reset.SetSensitive (False);
}


// /////////////////////////////////////////////////////////////////////////
// scope_modified - called when selection made in sublist
// /////////////////////////////////////////////////////////////////////////

void
SearchScopeAgent::scope_modified()
{
  f_save.SetSensitive (True);
  f_reset.SetSensitive (True);
}


// /////////////////////////////////////////////////////////////////
// close
// /////////////////////////////////////////////////////////////////

void
SearchScopeAgent::close()
{
  if (f_reset.Sensitive())
    {
      bool dosave = message_mgr().question_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 17,
		"Do you want to save changes to the current scope?")));
      if (dosave)
        save_scope();
    }

  if (f_current_scope == NULL)
    f_unnamed.Unmanage();

  f_shell.Popdown();
}


// /////////////////////////////////////////////////////////////////////////
// save_unnamed_scope
// /////////////////////////////////////////////////////////////////////////

void
SearchScopeAgent::save_unnamed_scope (const char *name)
{
  Wait_Cursor bob;

  // Allocate a bit handle for the new scope 
  BitHandle data_handle = f_infolib_list->list()->get_data_handle();
  BitHandle visible_handle = f_infolib_list->data_handle();
  
  // save the bits from the applied handle
  ON_DEBUG (printf("Copying handle %ld to %ld\n", visible_handle, data_handle));
  f_infolib_list->list()->copy_selected (visible_handle, data_handle);
  if(f_auto_expand)
    f_infolib_list->list()->copy_expanded (visible_handle, data_handle);
  f_component_list->list()->copy_selected (visible_handle, data_handle);

  unsigned int mask = component_mask (data_handle);
  assert (mask != 0);

  UAS_PtrList<UAS_BookcaseEntry>bcases = bookcase_list(data_handle);
  f_current_scope = new UAS_SearchScope (name, bcases, mask);

  int position = 0;

  // Scan the current menu to find the correct insertion position. 
  List_Iterator<UAS_SearchScope *> s (f_scope_list);
  for (; s != 0; s++, position++)
    {
      if (s.item()->read_only())
        continue;

      // Find the first item that the new entry belongs before.
      ON_DEBUG (printf ("Editor strcmp to <%s>\n", s.item()->name()));
      if (strcmp (name, s.item()->name()) < 0)
        break;
    }

  f_scope_list.add_handle(data_handle, position);
  f_scope_list.insert_before (s, f_current_scope);
  ON_DEBUG (printf ("Final editor position = %d\n", position));

  // Create the new button. 
  DECLM (WXmPushButton, scope_btn, f_scope_option.SubMenuId(), name);
  ON_ACTIVATE (scope_btn,select_scope);
  scope_btn.PositionIndex (position);
  scope_btn.UserData (f_current_scope);
  f_scope_option.MenuHistory (scope_btn);

  // Flush the changes to disk. 
  ON_DEBUG (puts (">> About to save scope"));
  f_scope_list.save();
}


unsigned int
SearchScopeAgent::component_mask (BitHandle handle)
{
  unsigned int return_mask = 0;

  OutlineList *scope_list = f_component_list->list();
  // BitHandle handle = f_component_list->data_handle();

  if (((OutlineElement*)(*scope_list)[0])->is_selected(handle)){
    return_mask = f_all;
    ON_DEBUG(cerr << "ALL COMPONENTS selected" << endl);
  } else { 
    List *top = ((OutlineElement*)(*scope_list)[0])->children();
    if (((OutlineElement*)(*top)[0])->is_selected(handle)){
      return_mask |= f_titles;
      ON_DEBUG(cerr << "TITLES selected" << endl);
    }
    if (((OutlineElement*)(*top)[1])->is_selected(handle)){
      return_mask |= f_bodies;
      ON_DEBUG(cerr << "BODY selected" << endl);
    }
    if (((OutlineElement*)(*top)[2])->is_selected(handle)){
      return_mask |= f_examples;
      ON_DEBUG(cerr << "EXAMPLES selected" << endl);
    }
    if (((OutlineElement*)(*top)[3])->is_selected(handle)){
      return_mask |= f_indexes;
      ON_DEBUG(cerr << "INDEX selected" << endl);
    }
    if (((OutlineElement*)(*top)[4])->is_selected(handle)){
      return_mask |= f_tables;
      ON_DEBUG(cerr << "TABLES selected" << endl);
    }
#ifndef DtinfoClient
    if (((OutlineElement*)(*top)[5])->is_selected(handle)){
      return_mask |= f_graphics;
      ON_DEBUG(cerr << "GRAPHICS selected" << endl);
    }
#endif
  }
  return return_mask ;
}


void
SearchScopeAgent::set_components (u_int mask)
{
#define SELECT(list, item, handle) \
  ((OutlineElement*)(*list)[item])->set_selected(handle)
#define DESELECT(list, item, handle) \
  ((OutlineElement*)(*list)[item])->unset_selected(handle)

  OutlineList *scope_list = f_component_list->list();
  BitHandle handle = f_component_list->data_handle();
  List *top = ((OutlineElement*)(*scope_list)[0])->children();

  if ((mask & f_all) == f_all)
    {
      SELECT(scope_list, 0, handle);
      DESELECT(top,0,handle);
      DESELECT(top,1,handle);
      DESELECT(top,2,handle);
      DESELECT(top,3,handle);
      DESELECT(top,4,handle);
#ifndef DtinfoClient
      DESELECT(top,5,handle);
#endif
    }
  else
    {
      DESELECT(scope_list, 0, handle);

      if (mask & f_titles)
        SELECT(top,0,handle);
      else
        DESELECT(top,0,handle);

      if (mask & f_bodies)
        SELECT(top,1,handle);
      else
        DESELECT(top,1,handle);

      if (mask & f_examples)
        SELECT(top,2,handle);
      else
        DESELECT(top,2,handle);

      if (mask & f_indexes)
        SELECT(top,3,handle);
      else
        DESELECT(top,3,handle);

      if (mask & f_tables)
        SELECT(top,4,handle);
      else
        DESELECT(top,4,handle);

#ifndef DtinfoClient
      if (mask & f_graphics)
        SELECT(top,5,handle);
      else
        DESELECT(top,5,handle);
#endif
    }
}
#undef SELECT
#undef DESELECT


// /////////////////////////////////////////////////////////////////
// generate_component_list
// /////////////////////////////////////////////////////////////////

#define ADD(LIST,STRING,CHILD,EXPANDED) \
  oe = new OutlineString (STRING); \
  if (CHILD) oe->set_children (CHILD); \
  if (EXPANDED) oe->set_expanded (0x1); \
  LIST->append (*oe);

OutlineList *
SearchScopeAgent::generate_component_list()
{
  OutlineList *components = new OutlineList (10);
  OutlineList *parts = new OutlineList (10);
  OutlineElement *oe;

  ADD (components, CATGETS(Set_Messages, 22, "Everything"), parts, True);
  ADD (parts, CATGETS(Set_Messages, 23, "Titles"), NULL, False);
  ADD (parts, CATGETS(Set_Messages, 24, "Body"), NULL, False);
  ADD (parts, CATGETS(Set_Messages, 25, "Examples"), NULL, False);
  ADD (parts, CATGETS(Set_Messages, 26, "Index"), NULL, False);
  ADD (parts, CATGETS(Set_Messages, 27, "Tables"), NULL, False);
#ifndef DtinfoClient
  ADD (parts, CATGETS(Set_Messages, 28, "Graphics"), NULL, False);
#endif
  
  return (components);
}
#undef ADD

// check to see if the first infolib is selected in the scope (handle).
bool
SearchScopeAgent::infolib_selected (BitHandle handle)
{
  OutlineElement *infolib = (OutlineElement *) (*f_infolib_list->list())[0];
  return (infolib->is_selected (handle));
}


// check to see if infolib is selected in the scope (handle).
bool
SearchScopeAgent::infolib_selected (UAS_String &lid, BitHandle handle)
{
  int index = lid_to_index(lid);
  OutlineElement *infolib = (OutlineElement *) (*f_infolib_list->list())[index];

  return (infolib->is_selected (handle));
}

// /////////////////////////////////////////////////////////////////
// bookcase_list - return a list of bookcases for a given handle
// /////////////////////////////////////////////////////////////////

UAS_PtrList<UAS_BookcaseEntry>
SearchScopeAgent::bookcase_list (BitHandle handle)
{
  ON_DEBUG (printf ("*** BUILDING SCOPE LIST, handle = %ld ***\n", handle));
  // Get the list of all infobases.

  // get the list of bookcase names
  UAS_List<UAS_Common> bcase_list = bookcaseList();
  OutlineList *ol = f_infolib_list->list();

  // NOTE: hardcoding - assuming 0 is only library


  UAS_PtrList<UAS_BookcaseEntry>  bcases;
  UAS_BookcaseEntry              *bce;
  OutlineElement                 *oe;
  UAS_Pointer<UAS_Common>         common;
  int                             infolib, bookcase;

  // Iterate over the infolibs looking for selected entries.
  assert (handle != 0);
  for (infolib = 0; infolib < ol->length(); infolib++)
  { 
    //
    // See if the infolib is selected.
    //
    oe = ((OutlineElement*)(*ol)[infolib]); 
    List *bclist = ((OutlineElement*)(*ol)[infolib])->children();
    if (oe->is_selected (handle))
    {
      // get all bookcases and append them to the selected list
      for (bookcase = 0; bookcase < bclist->length(); bookcase++)
      {
        oe = ((OutlineElement *)(*bclist)[bookcase]);
        common = ((TOC_Element*)oe)->toc();
        if (common->type() == UAS_BOOKCASE)
        {
          // note: copied into a scope obj; deleted in scope obj's dtor
          bce = new UAS_BookcaseEntry(common);
          bcases.append(bce);
          ON_DEBUG (printf("Adding base: %s\n",bce->name()));
        }
      }
    }
    else
    {
      // The infolib is not selected--check to see if any of its
      // children (bookcases) are selected.
      //
      // Get children of the infolib.
      // If the bookcase is selected, create a BookcaseEntry and 
      // append it to the list.
      for (bookcase = 0; bookcase < bclist->length(); bookcase++)
      {
        oe = ((OutlineElement *)(*bclist)[bookcase]);
        if (oe->is_selected (handle))
        {
          common = ((TOC_Element*)oe)->toc();
          if (common->type() == UAS_BOOKCASE)
          {
            // note: copied into a scope obj; deleted in scope obj's dtor
            bce = new UAS_BookcaseEntry(common);
            bcases.append(bce);
            ON_DEBUG (printf("Adding base: %s\n",bce->name()));
          }
        }
        else
        {
          // The bookcase is not selected, so check to see if any kids are.
          //
          // Check for any children book selections.  If found, add an
          // entry for the enclosing bookcase and add the books to it.
          UAS_ObjList<int> booklist;
          if (oe->children_cached())
          {
            //
            // build a list of books for a give bookcase
            //
            List *books = oe->children();
            // (1-based book, since that's how they're indexed in Fulcrum.)
            for (int book_num = 1; book_num <= books->length(); book_num++)
            {
                ON_DEBUG (printf ("Checking Book #%d: ", book_num));
              if (((OutlineElement*)(*books)[book_num-1])->is_selected (handle))
              {
                ON_DEBUG (puts ("selected"));
                booklist.append (book_num);
              }
              else
              {
                ON_DEBUG (puts ("NOT selected"));
              }
            }
            if (booklist.numItems() == 0)
              continue; // nothing was selected--go to next infolib
  
            common = ((TOC_Element*)oe)->toc();
            // note: copied into a scope obj; deleted in scope obj's dtor
            bce = new UAS_BookcaseEntry(common);
            bcases.append(bce);
            ON_DEBUG (printf("Adding base: %s\n",bce->name()));

            // Now add the booklist to that parent, which owns it now.
            bce->set_book_list (booklist);
            booklist.clear();
          }
        }
      }
    }
  }
  return (bcases);
}

// ////////////////////////////////////////////////////////////////
// return a list of bookcases from all infolibs currently installed
// ////////////////////////////////////////////////////////////////
UAS_PtrList<UAS_BookcaseEntry>
SearchScopeAgent::bookcase_list()
{
  UAS_PtrList<UAS_BookcaseEntry> bcases;
  UAS_BookcaseEntry *bce;
  OutlineList *ol = f_infolib_list->list();
  OutlineElement *oe;

  for (int i = 0; i < ol->length(); i++)
  {
    oe = ((OutlineElement *) (*ol)[i]);
    UAS_Pointer<UAS_Common> infolib = ((TOC_Element *)oe)->toc();
    UAS_List<UAS_Common> kids = infolib->children();
    for (int j = 0; j < kids.length(); j++)
    {
      if (kids[j]->type() == UAS_BOOKCASE)
      {
        // the uas scope object dups this list and will delete it
        bce = new UAS_BookcaseEntry(kids[j]);
        bce->set_base_num(j);
        bce->set_infolib_num(i);
        bcases.append(bce);
      }
    }
  }

  return (bcases);
}

// Return the list of bookcases for a given infolib.
//
UAS_PtrList<UAS_BookcaseEntry>
SearchScopeAgent::bookcase_list(UAS_String &lid)
{
  int index = lid_to_index(lid);
  OutlineList *ol = f_infolib_list->list();
  OutlineElement *oe = ((OutlineElement *) (*ol)[index]);
  UAS_Pointer<UAS_Common> infolib = ((TOC_Element *)oe)->toc();

  UAS_PtrList<UAS_BookcaseEntry> bcases;
  UAS_BookcaseEntry *bce;
  UAS_List<UAS_Common> kids = infolib->children();

  for (int j = 0; j < kids.length(); j++)
  {
    if (kids[j]->type() == UAS_BOOKCASE)
    {
      // the uas scope object dups this list and will delete it
      bce = new UAS_BookcaseEntry(kids[j]);
      bce->set_base_num(j);
      bcases.append(bce);
    }
  }
  return (bcases);
}

// /////////////////////////////////////////////////////////////////////////
// truncate - destructively removes leading and trailing blanks
// /////////////////////////////////////////////////////////////////////////

char *
SearchScopeAgent::truncate (char *string)
{
  // remove leading blanks 
  char *newstr = string;
  while (*newstr == ' ')
    newstr++;

  // remove trailing blanks
  u_int len = strlen (newstr);
  char *eptr = newstr + len - 1 ;
  while (*eptr == ' ')
    --eptr ;

  // terminate string
  *(eptr + 1) = '\0';

  // change newlines to spaces 
  for (char *s = newstr; *s != '\0'; s++)
    if (*s == '\n')
      *s = ' ';

  return (newstr);
}


// /////////////////////////////////////////////////////////////////////////
// select_scope
// /////////////////////////////////////////////////////////////////////////

void
SearchScopeAgent::select_scope (WCallback *wcb)
{
  WXmPushButton scope_btn (wcb->GetWidget());
  ON_DEBUG (fprintf (stderr,"ScopeEdit: selecting scope\n"));

  if (f_reset.Sensitive())
    {
      bool dosave = message_mgr().question_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 17,
		"Do you want to save changes to the current scope?")));
      if (dosave)
        save_scope();
      // Reset the menu history in case a new scope was created. 
      f_scope_option.MenuHistory (scope_btn);
    }
  set_current_scope(scope_btn);
}

void
SearchScopeAgent::set_current_scope (WXmPushButton& btn)
{
  // update current selection in option menu
  f_scope_option.MenuHistory (btn);
  f_unnamed.Unmanage();

  // re-computation needed. - 12/20/94 haya(addition 11/2/94 kamiya)
  f_scope_option.Unmanage();
  f_scope_option.Manage();

  f_current_scope = (UAS_SearchScope *) btn.UserData();

  // Copy the specified scope's selections in the list.
  f_infolib_list->list()->copy_selected(f_scope_list.lookup_handle(f_current_scope),
                                         f_infolib_list->data_handle());
  if (f_auto_expand)
  {
    f_infolib_list->list()->copy_expanded(f_scope_list.lookup_handle(f_current_scope),
                                         f_infolib_list->data_handle());
    f_infolib_list->update_list(f_infolib_list->list(), f_infolib_list->data_handle());
  }
  set_components (f_current_scope->search_zones().zones());

  // visually update 
  f_infolib_list->data_handle (f_infolib_list->data_handle());
  f_component_list->data_handle (f_component_list->data_handle());

  f_new.SetSensitive (True);
  f_save.SetSensitive (False);
  f_rename.SetSensitive (True);
  f_delete.SetSensitive (True);
  f_reset.SetSensitive (False);
}


void
SearchScopeAgent::delete_scope()
{
  Xassert (f_current_scope != NULL);

  bool doit = message_mgr().question_dialog (
		(char*)UAS_String(CATGETS(Set_Messages, 29,
		"Are you sure you want to delete the scope?")));
  if (!doit)
    return;

  UAS_SearchScope *scope = f_current_scope;

  // remove the associated button
  Widget dead_meat = f_scope_option.MenuHistory();
  XtUnmanageChild (dead_meat);
  XtDestroyWidget (dead_meat);
  select_initial (dead_meat);

  // remove the scope from the scope list
  f_scope_list.remove_handle(scope);
  f_scope_list.remove (scope);

  // Make sure old scope name preference isn't saved by NULLing out
  // it's value.  This should really happen in a ScopeListMgr object.  DJB
  char scratch[64];
  sprintf (scratch, "Scope.%s", scope->name());
  StringPref (scratch).value ("");
  delete scope;

  // flush to disk. 
  f_scope_list.save();
}

// Add new infolib to list
void
SearchScopeAgent::add_infolib(UAS_Pointer<UAS_Common> &lib)
{
  // get infolib list and append new infolib to the list
  char scratch[256];
  OutlineList *ol = f_infolib_list->list();
  OutlineElement *oe;
  int infolib_num;
      
  UAS_String temp_lid;

  // See if infolib is already in list--if it's there, 
  // don't add it again.
  int i;
  for (i = 0; i < ol->length(); i++)
  {
    oe = ((OutlineElement *) (*ol)[i]);
    if (((TOC_Element *)oe)->toc() == lib)
      return;
  }

  // need to make our own copy of this infolib
  UAS_Pointer<UAS_Common> newLib = UAS_Common::create(lib->locator());
  UAS_Pointer<UAS_Collection> cd = (UAS_Collection *) ((UAS_Common *) newLib);
  newLib = cd->root();
  ol->append (new TOC_Element (cd->root()));

  // update list
  BitHandle handle = f_infolib_list->data_handle();
  ((OutlineElement *) (*ol)[0])->set_expanded (handle);
  f_infolib_list->set_list (ol, handle);
  f_infolib_list->clear();

  // find infolib insertion point in list; need the index number
  for (i = 0; i < ol->length(); i++)
  {
      oe = ((OutlineElement *) (*ol)[i]);
      if (((TOC_Element *)oe)->toc()->lid() == newLib->lid())  
	  break;
  }

  infolib_num = i; // next insertion point in list

  // Create search scope for new infolib
  UAS_String newLib_lid(newLib->lid());
  UAS_SearchScope *scope = create_infolib_scope(newLib_lid);
  
  //
  // update the "All Libraries" search scope
  //

  // find the scope
  scope = get_search_scope((char*)UAS_String(
		CATGETS(Set_SearchScopeAgent, 4, "All Libraries")));
  
  // get a list of all bookcases
  UAS_PtrList<UAS_BookcaseEntry>bcases = bookcase_list();
  
  // update list of bookcases in the scope
  scope->bookcases(bcases);

  if (f_first_time)
  {
    f_first_time = False;
    fill_option_menu();
    select_initial();
  }

  rebuild_scope_list();
}

// Remove infolib from infobase list
void
SearchScopeAgent::remove_infolib(UAS_Pointer<UAS_Common> &lib)
{
  // get infolib list and remove infolib from the list
  OutlineList *ol = f_infolib_list->list();
  OutlineElement *oe;
  int infolib_num;

  // Get index of infolib in infobase list
  infolib_num = lid_to_index(lib->lid());

  if (infolib_num < 0)
    return;  // infolib not in list

  // Remove infolib from list
  ol->remove (infolib_num);

  // update list
  BitHandle handle = f_infolib_list->data_handle();

  f_infolib_list->clear();
  int i;
  for (i = 0; i < ol->length(); i ++)
    ((OutlineElement *) (*ol)[i])->set_expanded (handle);

  f_infolib_list->set_list (ol, handle);
  for (i = 0; i < ol->length(); i ++)
    ((OutlineElement *) (*ol)[i])->set_expanded (handle);

  f_infolib_list->set_list (ol, handle);


#ifdef EAM

  // find the scope for deleted library.
  // TEMPORARY: The scope name is the id of the first
  // bookcase;
  UAS_List<UAS_Common> kids = lib->children();
  for (i = 0; i < kids.length(); i++)
  {
      if (kids[i]->type() == UAS_BOOKCASE)
        break;
  }
  char scratch[128];
  sprintf(scratch, "Infolib %s", (char *)kids[i]->id());
#endif

  // remove search scope associated with infolib
  UAS_SearchScope *scope = get_search_scope(lib);

  // remove the scope from the scope list
  f_scope_list.remove_handle(scope);
  f_scope_list.remove (scope);

#ifdef EAM
  // Make sure old scope name preference isn't saved by NULLing out
  // it's value.  This should really happen in a ScopeListMgr
  // object.  DJB
  char scratch[128];
  sprintf (scratch, "Scope.%s", scope->name());
  StringPref (scratch).value ("");
#endif

  delete scope;

  //
  // update the "All Libraries" search scope
  //

  // find the scope
  scope = get_search_scope((char*)UAS_String(
		CATGETS(Set_SearchScopeAgent, 4, "All Libraries")));
  
  // get a list of all bookcases
  UAS_PtrList<UAS_BookcaseEntry>bcases = bookcase_list();
  
  // update list of bookcases in the scope
  scope->bookcases(bcases);

  rebuild_scope_list();
}

/* work procedure */
// Set current scope after callback has finished
bool
current_scope(XtPointer client_data)
{
  SearchScopeAgent* agent = (SearchScopeAgent*)client_data;
  agent->update_current_scope();
  return True;
}

void
SearchScopeAgent::update_current_scope()
{
  int n;
  Arg args[2];
  WidgetList kids;
  Cardinal num_kids;
  UAS_SearchScope* scope;
  UAS_SearchScope* current_scope=NULL;

  n = 0;
  XtSetArg(args[n], XmNnumChildren, &num_kids); n++;
  XtSetArg(args[n], XmNchildren, &kids); n++;
  XtGetValues(f_scope_option.SubMenuId(), args, n);

  if(f_current_scope != NULL)
  {
    for (int i = 1; i < num_kids; i++)
    {
      WXmPushButton btn (kids[i]);
      scope = (UAS_SearchScope*)btn.UserData();
      if(scope->name() == f_current_scope->name())
      {
        set_current_scope(btn);
        return;
      }
    }
  }

  if (num_kids > 1)
  {
    WXmPushButton btn (kids[1]);
    set_current_scope(btn);
    return;
  }

  // There are no user scopes in list. Manage the unnamed button
  // and set current scope to NULL
  f_reset.SetSensitive (False);
  new_scope();
}

// Rebuild the scope list after an infolib is added or removed.
// Then update the option menu to reflect changes that may
// have occured in the scope list.
// 
void
SearchScopeAgent::rebuild_scope_list()
{
  UAS_SearchScope *scope;
  // Save the name of the current scope.
  UAS_String name;
  if (f_current_scope != NULL)
  {
    name = UAS_String(f_current_scope->name());
  }

  // Delete all named scopes so list can be rebuilt.
  List_Iterator<UAS_SearchScope *> si (f_scope_list);

  for (; si != 0; si++)
  {
    scope = si.item();

    // delete all named scopes
    if (!scope->read_only())
    {
      // remove the scope from the scope list
      f_scope_list.remove_handle(scope);
      f_scope_list.remove (scope);

      delete scope;
      si.reset(); // this really is necessary.
                  // see comment in List_base::remove()
    }
  }

  // Rebuild the named scope list.
  f_scope_list.create_named_scopes();

  // update query editor menu
  if (f_option_menu != NULL)
    f_option_menu->update_option_menu();

  update_option_menu(name);

  static UpdateMenu update_menu;
  update_menu.update = True;
  send_message(update_menu);
}

// Update the option menu to reflect any changes
// that may have occured in the list of named scopes due
// to addition or removal of infolibs.
// Update current scope.
void
SearchScopeAgent::update_option_menu(UAS_String &scope_name)
{

  int n;
  Arg args[2];
  WidgetList kids;
  Cardinal num_kids;

  n = 0;
  XtSetArg(args[n], XmNnumChildren, &num_kids); n++;
  XtSetArg(args[n], XmNchildren, &kids); n++;
  XtGetValues(f_scope_option.SubMenuId(), args, n);

  // destroy all toggle buttons in menu except
  // the unnamed button.
  for (int i = 1; i < num_kids; i++)
  {
    XtUnmanageChild (kids[i]);
    XtDestroyWidget (kids[i]);
  }

  fill_option_menu();

  XtAppAddWorkProc(window_system().app_context(), (XtWorkProc)current_scope, this);
#if EAM
  // need to set current scope to a valid scope.
  // see if the former "current scope" is still around.
  // if not, set current scope to first scope in list.
  // if there are no scopes in the list, set current scope
  // to NULL and manage the UnNamed scope button.

  // iterate through search scope list to see if current
  // scope is still there
  List_Iterator<UAS_SearchScope *> si (f_scope_list);
  UAS_String name;
  UAS_SearchScope *scope;
  UAS_SearchScope *user_scope = NULL;

  f_current_scope = NULL;
  for (; si != NULL; si++)
  {
    scope = si.item();

    if (!scope->read_only())
    {
      if (user_scope == NULL)
        user_scope = scope; // keep track of first user scope
      if(scope_name == scope->name())
      {
        set_current_scope(scope);
        return;
      }
    }
  }

  // see if former current scope has been deleted
  // due to infolib removal.
  if (user_scope == NULL)
  {
    // There are no user scopes in list. Manage the unnamed button
    // and set current scope to NULL
    f_reset.SetSensitive (False);
    new_scope();
    return;
  }

  // set current scope to first user scope-need to get
  // associated menu button.

  n = 0;
  XtSetArg(args[n], XmNnumChildren, &num_kids); n++;
  XtSetArg(args[n], XmNchildren, &kids); n++;
  XtGetValues(f_scope_option.SubMenuId(), args, n);

  set_current_scope(kids[1]);
#endif

}

UAS_List<UAS_Common>
SearchScopeAgent::list()
{
  if (f_shell == 0)
  {
    create_ui();
  }

  UAS_List<UAS_Common> rval;
  OutlineList *ol = f_infolib_list->list();
  OutlineElement *oe;

  for (int i = 0; i < ol->length(); i++)
  {
    oe = ((OutlineElement *) (*ol)[i]);
    UAS_Pointer<UAS_Common> infolib = ((TOC_Element *)oe)->toc();
    rval.insert_item((UAS_Common *)(infolib));
  }
  return rval;
}

// return a list of bookcase objects for a given infolib
//
UAS_List<UAS_Common>
SearchScopeAgent::list(UAS_String &lid)
{
  // convert lid into index
  int index = lid_to_index(lid);
  OutlineList *ol = f_infolib_list->list();
  OutlineElement *oe = ((OutlineElement *) (*ol)[index]);

  // extract the infolib
  UAS_Pointer<UAS_Common> infolib = ((TOC_Element *)oe)->toc();

  UAS_List<UAS_Common> rval;

  // get infolibs children--only save bookcases
  UAS_List<UAS_Common> kids = infolib->children();
  for (int i = 0; i < kids.length(); i++)
    if (kids[i]->type() == UAS_BOOKCASE)
      rval.insert_item(kids[i]);
  return rval;
}

// Create a scope for an infolib based on the infolib id
//
UAS_SearchScope *
SearchScopeAgent::create_infolib_scope(UAS_String &lid)
{
  int index          = lid_to_index(lid);
  OutlineList *ol    = f_infolib_list->list();
  OutlineElement *oe = ((OutlineElement *) (*ol)[index]);
  UAS_Pointer<UAS_Common> infolib = ((TOC_Element *)oe)->toc();

  UAS_PtrList<UAS_BookcaseEntry> bcases;
  UAS_BookcaseEntry *bce;

#ifdef EAM
  int base_num = 0;
  int first_base_num = -1;
#endif

  if(env().debug())
  {
    cerr << endl;
    cerr << "locator: " << (char *)infolib->locator() << endl;
    cerr << "     id: " << (char *)infolib->id() << endl;
    cerr << "    lid: " << (char *)infolib->lid() << endl;
    cerr << "   name: " << (char *)infolib->name() << endl;
    cerr << " locale: " << (char *)infolib->locale() << endl;
  }

  // get list of all bookcases for infolib and create bookcase
  // entry for each one.
  //
  UAS_List<UAS_Common> kids = list(lid);
  for (int i = 0; i < kids.length(); i++)
  {
    bce = new UAS_BookcaseEntry(kids[i]);
    bcases.append(bce);
  }

#ifdef EAM
  char scratch[128];
  sprintf(scratch, "Infolib %s", (char *)kids[first_base_num]->id());
#endif

  // create search scope for infolib using the infolib name as
  // search scope name.
  //
  UAS_SearchScope *s;
  s = create_scope((char*)infolib->name(), bcases, f_all, TRUE);
  s->set_infolib(infolib);
  return s;
}

// Find the index into the f_infolib_list given a lid
//
int
SearchScopeAgent::lid_to_index(const UAS_String &lid)
{
  return ((ScopeOutlineListView *)f_infolib_list)->lid_to_index(lid);
}

UAS_String
SearchScopeAgent::lid_to_name(UAS_String &lid)
{
  return ((ScopeOutlineListView *)f_infolib_list)->lid_to_name(lid);
}

UAS_String
SearchScopeAgent::name_to_lid(UAS_String &name)
{
  return ((ScopeOutlineListView *)f_infolib_list)->name_to_lid(name);
}

// Given a bookcase id, return its index into the f_infolib_list
// list.
int
SearchScopeAgent::bid_to_index(UAS_String &lid, UAS_String &bid)
{
  int index =  ((ScopeOutlineListView *)f_infolib_list)->lid_to_index(lid);
  OutlineList *ol = f_infolib_list->list();
  OutlineElement *oe = ((OutlineElement *) (*ol)[index]);
  UAS_Pointer<UAS_Common> infolib = ((TOC_Element *)oe)->toc();
  UAS_List<UAS_Common> kids = infolib->children();
  for (int i = 0; i < kids.length(); i++)
  {
    if (kids[i]->bid() == bid)
      return i;
  }
  // There has to be an bookcase for a given bid--if
  // not, something has gone wrong.
  throw (CASTEXCEPT Exception());
}

// given a search scope name, return the search scope
UAS_SearchScope *
SearchScopeAgent::get_search_scope(const char* name)
{
  xList<UAS_SearchScope *> &scopes = f_scope_list;
  List_Iterator<UAS_SearchScope *> iter (scopes);
  UAS_SearchScope *scope;

  for(;iter;iter++)
  {
    scope = iter.item();
    if (strcmp(name, scope->name()) == 0)
      return scope;
  }
  throw (CASTEXCEPT Exception());
}

// Given an infolib, return the search scope.
//
UAS_SearchScope *
SearchScopeAgent::get_search_scope(UAS_Pointer<UAS_Common> &infolib)
{
  xList<UAS_SearchScope *> &scopes = f_scope_list;
  List_Iterator<UAS_SearchScope *> iter (scopes);
  UAS_SearchScope *scope;
  int scope_idx=0;
  int maxScope=0; // max scope index

  iter++; // skip "Current Section" scope
  iter++; // skip "All Libraries" scope

  // find scope for associated infolib
  for(;iter;iter++)
  {
    scope = iter.item();
    if(scope->get_infolib() == infolib)
      return scope;
  }
  throw (CASTEXCEPT Exception());
}

// Search scope agent needs access to query editor
void
SearchScopeAgent::option_menu(QueryEditor *menu)
{
  f_option_menu = menu;
}
