/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: SearchResultsMgr.hh /main/3 1996/06/11 16:27:49 cde-hal $
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
 *"
 */

#include "UAS.hh"

class SearchResultsAgent;

class SearchResultsMgr : public NodeListMgr
{
public: //functions
  SearchResultsMgr();
  ~SearchResultsMgr();

  void display (ResultID *results);

protected: // functions
  SearchResultsAgent *get_agent ();
  static bool check_entry (ListEntry *, void *);

protected: //variables
  LONG_LIVED_HH(SearchResultsMgr,search_results_mgr);
};

LONG_LIVED_HH2(SearchResultsMgr,search_results_mgr);
