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
/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED		*/
/*	All Rights Reserved					*/

/*
 * $TOG: SearchMgr.C /main/17 1998/04/17 11:37:13 mgreess $
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

#include <sstream>
using namespace std;

#include "Registration.hh"

#include "UAS.hh"

#define C_ResultID
#define L_OliasSearch

#define C_WindowSystem
#define L_Other

#define C_QueryEditor
#define L_Query

#define C_xList
#define L_Support

#define C_SearchHistoryListView
#define L_Agents

#define C_MessageMgr
#define C_SearchResultsMgr
#define C_SearchMgr
#define C_SearchScopeMgr
#define C_PrefMgr
#define L_Managers

#include <Prelude.h>

#include "Managers/CatMgr.hh"

LONG_LIVED_CC(SearchMgr,search_mgr)

// Reconfigure search engine
class SeReconfigure : public UAS_Receiver<UAS_CollectionRetrievedMsg >,
                      public UAS_Receiver<UAS_LibraryDestroyedMsg>
{
public:
  SeReconfigure();

private:
  void receive (UAS_CollectionRetrievedMsg &message, void *client_data);
  void receive (UAS_LibraryDestroyedMsg &message, void *client_data);
};

SeReconfigure::SeReconfigure()
{
  UAS_Collection::request((UAS_Receiver<UAS_CollectionRetrievedMsg> *)
                        this);
  UAS_Common::request ((UAS_Receiver<UAS_LibraryDestroyedMsg> *)this);
}

// infolib was added to system, need to reinit search engine
void
SeReconfigure::receive (UAS_CollectionRetrievedMsg &message, void *client_data)
{
  ON_DEBUG (printf ("Received UAS_CollectionRetrievedMsg message!\n"));
  UAS_Pointer<UAS_Common> root(message.fCol->root());
  search_mgr().add_root(root);
}

// infolib was removed from system, need to reinit search engine
void
SeReconfigure::receive (UAS_LibraryDestroyedMsg &message, void *client_data)
{
  ON_DEBUG (printf ("Received UAS_LibraryDestroyedMsg message!\n"));
  search_mgr().remove_root(message.fLib);
}

static SeReconfigure se_reconfigure;



// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

SearchMgr::SearchMgr()
: f_search_engine(NULL),
  f_query_editor (NULL),
  f_search_history_list (pref_mgr().get_int (PrefMgr::SearchHistSize)),
  f_search_history_list_view (NULL),
  f_search_section ("")
{
  init();
}

// /////////////////////////////////////////////////////////////////
// init
// /////////////////////////////////////////////////////////////////

void
SearchMgr::init()
{
    UAS_List<UAS_String> rootLocs = UAS_Common::rootLocators ();

    unsigned int i;
    for (i = 0; i < rootLocs.length(); i ++) {
	UAS_Pointer<UAS_Common> theDoc =
				UAS_Common::create(*(UAS_String*)rootLocs[i]);
	UAS_Pointer<UAS_Collection> theCol = (UAS_Collection *)
		((UAS_Common *) theDoc);
	theDoc = theCol->root();
	f_roots.insert_item (theDoc);
    }

  setInitialized();
  setStatus (eSuccess);
}

SearchMgr::~SearchMgr()
{
    while (f_search_history_list.length () > 0)
	f_search_history_list.remove_first();
}

UAS_SearchEngine &
SearchMgr::search_engine () {
    if (f_search_engine == 0) {
	if (f_roots.length() > 0)
	    f_search_engine = f_roots[0]->search_engine();
	else
	    throw (CASTEXCEPT Exception());
    }
    return *(UAS_SearchEngine*)f_search_engine;
}


// /////////////////////////////////////////////////////////////////
// history_entry_activate - user says display something from hist
// /////////////////////////////////////////////////////////////////

void
SearchMgr::history_entry_activate (FolioObject *, u_int,
				   void *notify_data, void *)
{
  Wait_Cursor bob;
  size_t which_item = (size_t) notify_data;

  ResultID *rid = (ResultID *) f_search_history_list[which_item];

  // Fix for DTS 8327. 
  if (rid->ndocs() > 0)
    search_results_mgr().display (rid);
  else
    display_message (NO_HITS);
}


// /////////////////////////////////////////////////////////////////
// search_history_list_view
// /////////////////////////////////////////////////////////////////

ListView &
SearchMgr::search_history_list_view()
{
  if (f_search_history_list_view == NULL)
    {
      f_search_history_list_view =
	new SearchHistoryListView (&f_search_history_list,
	 (char*)"search_history");
      f_search_history_list_view->AddDependent
	((notify_handler_t) &SearchMgr::history_entry_activate,
	 ListView::ENTRY_ACTIVATE);
    }

  return (*f_search_history_list_view);
}

// /////////////////////////////////////////////////////////////////
// display_editor
// /////////////////////////////////////////////////////////////////

void
SearchMgr::display_editor()
{
  if (f_query_editor == NULL)
    f_query_editor = new QueryEditor(search_engine());

  f_query_editor->display();
}

// /////////////////////////////////////////////////////////////////
// search - invoke search engine on raw query and display results
// /////////////////////////////////////////////////////////////////

void
SearchMgr::parse_and_search (char *query, UAS_SearchScope *scope) {
//
//  SWM -- code lifted from old SearchQuery object. If no scope
//  is passed, pass "current section" if f_search_section is set,
//  otherwise, pass "information library"
//
    if (scope == NULL) {
       xList<UAS_SearchScope *> &scope_list = search_scope_mgr().scope_list();
       List_Iterator<UAS_SearchScope *> s (scope_list);
       if (!f_search_section.length())
	    s ++;
       scope = s.item();
    }

  // Hand off to search engine.
  UAS_Pointer<UAS_SearchResults> tmp_results;
  ResultID *search_results = 0;
  if (f_search_section.length()) {
    scope->search_zones().section ((char*)f_search_section);
  }
  mtry
  {
      tmp_results = search_engine().search (query, *scope);
  }
  mcatch (UAS_Exception&, e)
  {
      message_mgr().error_dialog ((char*)e.message());
      return;
  }
  mcatch_any ()
  {

  }
  end_try;

  if (tmp_results == 0) {
    display_message(BAD_QUERY);
  } else {

    if (tmp_results->num_docs() > 0) {
	search_results = new ResultID (tmp_results);
	f_search_history_list.append (*search_results);

	if (!f_search_section.length()) {
	  search_results_mgr().display (search_results);
	}

	if (f_search_section.length() ||
	    pref_mgr().get_boolean (PrefMgr::DisplayFirstHit))
	  {
	    UAS_Pointer<UAS_List<UAS_SearchResultsEntry> > tmpPtr =
	      tmp_results->create_results(0, 1);

	    UAS_List<UAS_SearchResultsEntry> * tmpTmpPtr =
		 (UAS_List<UAS_SearchResultsEntry>*)tmpPtr;

	    UAS_List<UAS_SearchResultsEntry> tmp3Ptr =
		 *tmpTmpPtr;

	    UAS_Pointer<UAS_SearchResultsEntry> tmp4Ptr =  tmp3Ptr[0];

	    f_current_hits = tmp4Ptr->create_matches(); 
	    UAS_String target = tmp4Ptr->id();  
	    UAS_Pointer<UAS_Common> doc_ptr = UAS_Common::create (target);
	    doc_ptr->retrieve();
	  }
    } else if (tmp_results->num_docs() == 0) {
        display_message(NO_HITS);
    }
  } 
}

void
SearchMgr::display_message (SearchMessageType msg, int)
{
  switch( msg ){
  case NO_HITS:
    message_mgr().info_dialog (
	(char*)UAS_String(CATGETS(Set_Messages, 40, "File a Bug")));
    break;
  case BAD_QUERY:
    message_mgr().warning_dialog (
	(char*)UAS_String(CATGETS(Set_Messages, 41, "File a Bug")));
    break;
  }
}

UAS_Pointer<UAS_List<UAS_TextRun> >
SearchMgr::current_hits()
{
  UAS_Pointer<UAS_List<UAS_TextRun> > rval = NULL;
  if (f_search_section) {
    rval = f_current_hits;
    f_current_hits = NULL; // f_current_hits is volatile
  }
  return rval;
}

void
SearchMgr::add_root(UAS_Pointer<UAS_Common>& root)
{
    unsigned int i;
    for (i = 0; i < f_roots.length(); i++) {
	if (f_roots[i] == root)
	    break;
    }
    if (i < f_roots.length())
	return;

    f_roots.insert_item(root);

    f_search_engine = NULL;
}

void
SearchMgr::remove_root(UAS_Pointer<UAS_Common>& root)
{
    unsigned int i;
    for (i = 0; i < f_roots.length(); i++) {
	if (f_roots[i] == root)
	    break;
    }
    if (i == f_roots.length())
	return;
    
    f_roots.remove_item(i);

    f_search_engine = NULL;
}

