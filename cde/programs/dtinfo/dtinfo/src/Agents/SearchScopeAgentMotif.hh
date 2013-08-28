/*
 * $XConsortium: SearchScopeAgentMotif.hh /main/9 1996/09/27 10:15:46 cde-hal $
 *
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

#include <WWL/WTopLevelShell.h>
#include <WWL/WXmMenu.h>
#include <WWL/WXmPushButton.h>

class OutlineListView;
class OutlineList;
class ScopePopup;
class QueryEditor;


struct UpdateMenu
{
  bool update;
};


template <class T> class xList;

class SearchScopeAgent : public Agent,
			 public UAS_SearchZones
{
public: // functions 
  SearchScopeAgent();
  ~SearchScopeAgent();

  STATIC_SENDER_HH (UpdateMenu);

  void display();

  xList<UAS_SearchScope *> &scope_list()
    { return (f_scope_list); }

  UAS_PtrList<UAS_BookcaseEntry>bookcase_list (BitHandle handle);
  UAS_PtrList<UAS_BookcaseEntry>bookcase_list (UAS_String &lid);
  UAS_PtrList<UAS_BookcaseEntry>bookcase_list ();
  bool infolib_selected (UAS_String &lid, BitHandle handle);
  bool infolib_selected (BitHandle handle);

  UAS_SearchScope *create_scope (const char *name,
			         UAS_PtrList<UAS_BookcaseEntry> &base_list,
			         unsigned int component_mask, bool ro);

  // methods for updating the infobase list
  void add_infolib(UAS_Pointer<UAS_Common>&);
  void remove_infolib(UAS_Pointer<UAS_Common>&);

  // method for creating a scope for a specific infolib
  UAS_SearchScope * create_infolib_scope(UAS_String &lid);

  void select_infolib (UAS_String &lid, BitHandle handle);


  UAS_List<UAS_Common> list();
  UAS_List<UAS_Common> list(UAS_String &lid);

  int bid_to_index(UAS_String &lid, UAS_String &bid); // convert a bid into an index
  UAS_String lid_to_name(UAS_String& lid);
  UAS_String name_to_lid(UAS_String& name);

  void option_menu(QueryEditor *menu);
  void update_current_scope();


private: // functions
  void create_ui();

  void select_initial (Widget exception = NULL);
  void new_scope();
  void save_scope();
  void rename_scope();
  void reset();
  void delete_scope();
  void close();
  void select_scope (WCallback *);

  void scope_modified();

  const char *scope_name_prompt();
  void save_unnamed_scope (const char *name);

  char *truncate (char *);

  unsigned int component_mask (BitHandle handle);
  void set_components(u_int mask);
  OutlineList *generate_component_list();

  UAS_SearchScope *get_search_scope(const char *); // return scope
  UAS_SearchScope *get_search_scope(UAS_Pointer<UAS_Common>&); // return scope

  int lid_to_index(const UAS_String &lid); // convert an lid into an index

  void fill_option_menu();
  void rebuild_scope_list();
  void update_option_menu(UAS_String &scope_name);
  void set_current_scope(WXmPushButton& btn);


private:   // variables
  WTopLevelShell    f_shell;
  SearchScopeList   f_scope_list;

  bool              f_first_time;
  bool              f_auto_expand;
  QueryEditor      *f_option_menu;

  UAS_SearchScope  *f_current_scope;
  WXmOptionMenu     f_scope_option;
  WXmPushButton     f_unnamed;
  OutlineListView  *f_infolib_list;
  OutlineListView  *f_component_list;
  WXmPushButton     f_new, f_save, f_rename, f_delete, f_reset;

  
};
