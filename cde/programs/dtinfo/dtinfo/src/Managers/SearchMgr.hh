/*	Copyright (c) 1994,1995,1996 FUJITSU LIMITED	*/
/*	All Rights Reserved				*/

/*
 * $XConsortium: SearchMgr.hh /main/5 1996/06/11 16:27:38 cde-hal $
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

#include "UAS.hh"

class QueryEditor;
class ListView;

class SearchMgr : public Long_Lived
{

public: // functions
  SearchMgr();
  ~SearchMgr();

  UAS_SearchEngine &search_engine();
  void parse_and_search (char *query, UAS_SearchScope *);

  //  void edit_query();
  void display_editor();

  ListView &search_history_list_view();
  void history_entry_activate (FolioObject *object, u_int notify_type,
			       void *notify_data, void *dependent_data);
  enum SearchMessageType
    { NO_HITS, BAD_QUERY };
  void display_message(SearchMessageType, int error_number = 0);

  void set_history_length (int size)
    { f_search_history_list.set_length (size); }

  void search_section (const UAS_String &ss) { f_search_section = ss; }
  UAS_String search_section () { return f_search_section; }
  void current_hits (UAS_Pointer<UAS_List<UAS_TextRun> > l) {
    f_current_hits = l;
  }
  UAS_Pointer<UAS_List<UAS_TextRun> > current_hits ();

// friend SearchMgr &search_mgr();

private:
  friend class SeReconfigure;
  void init();

  void add_root   (UAS_Pointer<UAS_Common>& root);
  void remove_root(UAS_Pointer<UAS_Common>& root);

  List *ibase_list();

protected: // variables
  QueryEditor        *f_query_editor;
  UAS_Pointer<UAS_SearchEngine>      f_search_engine ;
  HistoryList		f_search_history_list;
  ListView           *f_search_history_list_view;
  UAS_String	      f_search_section;
  UAS_Pointer<UAS_List<UAS_TextRun> > f_current_hits;
  UAS_List<UAS_Common> f_roots;

private:
  LONG_LIVED_HH(SearchMgr,search_mgr);
};

LONG_LIVED_HH2(SearchMgr,search_mgr);


