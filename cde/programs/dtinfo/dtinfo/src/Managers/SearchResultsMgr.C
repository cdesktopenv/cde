/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: SearchResultsMgr.cc /main/5 1996/06/11 16:27:43 cde-hal $
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

#define C_ResultID
#define L_OliasSearch

#define C_NodeListMgr
#define C_SearchResultsMgr
#define L_Managers

#define C_NodeListAgent
#define C_SearchResultsAgent
#define L_Agents

#include "Prelude.h"

LONG_LIVED_CC(SearchResultsMgr,search_results_mgr);

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

SearchResultsMgr::SearchResultsMgr()
{
}


SearchResultsMgr::~SearchResultsMgr()
{
  g_search_results_mgr = NULL;
}


// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
SearchResultsMgr::display (ResultID *results)
{
  SearchResultsAgent *sra = get_agent();

  // NOTE: error checking
  // NOTE: save in f_active_agents
  sra->display (results);

  setStatus (eSuccess);
}

// /////////////////////////////////////////////////////////////////
// get_agent
// /////////////////////////////////////////////////////////////////

SearchResultsAgent *
SearchResultsMgr::get_agent ()
{
  AgentListEntry *ale;
  
  // First try a non-retained visible window.
  ale =  (AgentListEntry *)
    f_active_agents.iterate (&SearchResultsMgr::check_entry, NULL);
    
  // If none are available, try the inactive list.
  if (ale == NULL)
    ale = (AgentListEntry *) next_inactive ();

  // Finally, if none are available for reuse we have to create one.
  if (ale == NULL)
    {
      ON_DEBUG (printf ("Creating a new results agent...take cover!\n");)
      SearchResultsAgent *sra = new SearchResultsAgent();
      sra->init ();
      ale = new AgentListEntry (sra);
      sra->set_agent_list_entry (ale);
      activate (ale);
    }

  return ((SearchResultsAgent *) ale->agent());
}


// /////////////////////////////////////////////////////////////////
// check_entry
// /////////////////////////////////////////////////////////////////

bool
SearchResultsMgr::check_entry (ListEntry *le, void *)
{
  AgentListEntry *ale = (AgentListEntry *) le;
  SearchResultsAgent *sra = (SearchResultsAgent *) ale->agent();

  if (sra->retain ())
    return (TRUE);
  else
    return (FALSE);
}
