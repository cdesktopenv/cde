/*
 * $XConsortium: SearchScopeMgr.hh /main/5 1996/09/14 13:09:26 cde-hal $
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

class List;
class SearchScopeAgent;
class ScopePopup;
class QueryEditor;
class UAS_SearchScope;
template <class T> class xList;

class SearchScopeMgr : public Long_Lived
{
public: // functions
  SearchScopeMgr();
  void display();
  void insert(UAS_Pointer<UAS_Common> &lib);
  void remove(UAS_Pointer<UAS_Common> &lib);

  xList<UAS_SearchScope *> &scope_list();

  bool infolib_selected (BitHandle handle);

  void option_menu(QueryEditor *menu);

  void show_warning(bool show) { f_show_warning = show; }
  bool show_warning() { return f_show_warning; }

private: // functions

private: //variables
  SearchScopeAgent *f_search_scope_agent;

  bool f_show_warning; // show invalid scope warning dialog?

private:
  LONG_LIVED_HH(SearchScopeMgr,search_scope_mgr);
};

LONG_LIVED_HH2(SearchScopeMgr,search_scope_mgr);

