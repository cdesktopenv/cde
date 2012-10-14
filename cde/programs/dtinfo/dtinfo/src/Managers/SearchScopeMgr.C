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
 * $XConsortium: SearchScopeMgr.C /main/9 1996/09/14 13:08:50 cde-hal $
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

#define C_xList
#define C_Support

#define C_SearchScopeMgr
#define L_Managers

#define C_SearchScopeAgent
#define C_OutlineListView
#define L_Agents

#define C_InfoBase
#define C_List
#define L_Basic


#include "Prelude.h"
#include <iostream>
using namespace std;

LONG_LIVED_CC(SearchScopeMgr,search_scope_mgr);

class LibraryNotify : public UAS_Receiver<UAS_CollectionRetrievedMsg >,
                      public UAS_Receiver<UAS_LibraryDestroyedMsg>
{
public:
  LibraryNotify();

private:
  void receive (UAS_CollectionRetrievedMsg &message, void *client_data);
  void receive (UAS_LibraryDestroyedMsg &message, void *client_data);
};

LibraryNotify::LibraryNotify()
{
  UAS_Collection::request((UAS_Receiver<UAS_CollectionRetrievedMsg> *)
                        this);
  UAS_Common::request ((UAS_Receiver<UAS_LibraryDestroyedMsg> *)this);
}

void
LibraryNotify::receive (UAS_CollectionRetrievedMsg &message, void *client_data)
{
  ON_DEBUG (printf ("Got Library Notify message!\n"));
  UAS_Pointer<UAS_Common> lib(message.fCol->root());
  search_scope_mgr().insert (lib);
}

void
LibraryNotify::receive (UAS_LibraryDestroyedMsg &message, void *client_data)
{
  ON_DEBUG (printf ("Got Library Notify message!\n"));
  search_scope_mgr().remove (message.fLib);
}

static LibraryNotify lib_notify;


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

SearchScopeMgr::SearchScopeMgr()
: f_search_scope_agent (NULL),
f_show_warning(False)
{
}


// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
SearchScopeMgr::display()
{
  if (f_search_scope_agent == NULL)
    f_search_scope_agent = new SearchScopeAgent();

  f_search_scope_agent->display();
}

// /////////////////////////////////////////////////////////////////
// scope_list - insert a new library scope into the scope list
// /////////////////////////////////////////////////////////////////
void
SearchScopeMgr::insert(UAS_Pointer<UAS_Common> &lib)
{
  if (f_search_scope_agent == NULL)
    f_search_scope_agent = new SearchScopeAgent();

  f_search_scope_agent->add_infolib(lib);
}

// /////////////////////////////////////////////////////////////////
// scope_list - remove a library scope from the scope list
// /////////////////////////////////////////////////////////////////
void
SearchScopeMgr::remove(UAS_Pointer<UAS_Common> &lib)
{
  if (f_search_scope_agent == NULL)
    f_search_scope_agent = new SearchScopeAgent();

  f_search_scope_agent->remove_infolib(lib);

}


// /////////////////////////////////////////////////////////////////
// scope_list - return a list of existing scopes
// /////////////////////////////////////////////////////////////////

xList<UAS_SearchScope *> &
SearchScopeMgr::scope_list()
{
  if (f_search_scope_agent == NULL)
    f_search_scope_agent = new SearchScopeAgent();
  return (f_search_scope_agent->scope_list());
}


bool
SearchScopeMgr::infolib_selected (BitHandle handle)
{
  return (f_search_scope_agent->infolib_selected (handle));
}

// /////////////////////////////////////////////////////////////////
// option_menu - give access for query editor option menu to
// SearchScopeAgent. This is so that menu items can be managed more
// readily.
// /////////////////////////////////////////////////////////////////
void
SearchScopeMgr::option_menu(QueryEditor *menu)
{
  f_search_scope_agent->option_menu (menu);
}
