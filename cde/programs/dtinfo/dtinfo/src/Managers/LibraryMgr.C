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
 * $TOG: LibraryMgr.C /main/12 1998/04/17 11:36:33 mgreess $
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
#include <sstream>
#include <iostream>
using namespace std;

#define C_LibraryMgr
#define C_NodeMgr
#define L_Managers

#define C_OutlineList
#define C_TOC_Element
#define L_Basic

#define C_LibraryAgent
#define L_Agents

#define C_xList
#define L_Support

#include "Prelude.h"

#include "utility/mmdb_exception.h"

LONG_LIVED_CC(LibraryMgr,library_mgr);

class DisplayTOC : public UAS_Receiver<UAS_CollectionRetrievedMsg >
{
public:
  DisplayTOC()
    { UAS_Collection::request ((UAS_Receiver<UAS_CollectionRetrievedMsg> *)
			this); }
private:
  void receive (UAS_CollectionRetrievedMsg &message, void *client_data);
};

void
DisplayTOC::receive (UAS_CollectionRetrievedMsg &message, void *client_data)
{
  ON_DEBUG (printf ("Got TOC display message!\n"));
  if (!(window_system().videoShell()->print_only)) {
      UAS_Pointer<UAS_Common> toc(message.fCol->root());
      library_mgr().display (toc);
  }
}

static DisplayTOC toc_displayer;

class LibraryDestroyed: public UAS_Receiver<UAS_LibraryDestroyedMsg> {
    public:
	LibraryDestroyed () {
	    UAS_Common::request ((UAS_Receiver<UAS_LibraryDestroyedMsg> *)this);
	}
    private:
	void receive (UAS_LibraryDestroyedMsg &msg, void *client_data) {
	    library_mgr().remove (msg.fLib);
	}
};

static LibraryDestroyed lib_destroyed;


// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

LibraryMgr::LibraryMgr()
: f_first_agent (NULL), f_visible_num(0)
{
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

LibraryMgr::~LibraryMgr()
{
}


// /////////////////////////////////////////////////////////////////
// create_library - just do it!
// /////////////////////////////////////////////////////////////////

void
LibraryMgr::create_library()
{
}

// /////////////////////////////////////////////////////////////////
// display
// /////////////////////////////////////////////////////////////////

void
LibraryMgr::display (UAS_Pointer<UAS_Common> &toc)
{
#ifdef AllowDetach
  cerr << "f_visible_num: " << f_visible_num << endl;
  for (int i = 0; i < fObjList.length(); i ++) {
    if (fObjList[i] == toc) {
	if (!f_first_agent)
	    throw (CASTEXCEPT Exception());
	// it only matters if the agent is not visible
	if (f_first_agent->popped_down()) {
	  // if we only had one visible agent find it and set it correctly
	  //if (f_visible_num == 1 && !node_mgr().visible_windows()) {

          // should not make the close button sensitive with only one
          // booklist window visible and no reading windows!
          if (f_visible_num > 1 && !node_mgr().visible_windows()) {
	    List_Iterator<LibraryAgent *> i (f_agent_list);
	    // we must walk down the list because we don't which one it is
	    for (; i; i++)
		if (!i.item()->popped_down())
		    i.item()->close_set_sensitive(True);
	  }
	  f_visible_num++;
	  // take care of the new visible agent
	  if (f_visible_num > 1 || node_mgr().visible_windows())
	      f_first_agent->close_set_sensitive(True);
	  // notify the node manager when state has just changed
	  if (f_visible_num == 1)
	      node_mgr().windows_notify(True);
	}
	f_first_agent->popup();
	return;
    }
  }
#endif

  //
  //  First, check to see if the object to display is already
  //  in our list.
  //
  unsigned int i;
  for (i = 0; i < fObjList.length(); i ++)
  {
    if (fObjList[i] == toc)
    {
      if (!f_first_agent)
        throw (CASTEXCEPT Exception());

      // if library agent is not visible, make it visible
      // then notify the node manager.
      if (f_first_agent->popped_down())
      {
        f_visible_num = 1;
        node_mgr().windows_notify(True);
      }
      f_first_agent->popup();
      return;
    }
  }

  // insert new infolib into list and if library agent
  // is not created, create it.
  fObjList.insert_item (toc);
  LibraryAgent *agent;
  if (f_first_agent) {
    agent = f_first_agent;
  } else {
    agent = new LibraryAgent();
    f_agent_list.append (agent);
    f_first_agent = agent;
    agent->keep_forever();
  }

#ifdef AllowDetach
  // if we only had one visible agent find it and set it correctly
  //if (f_visible_num == 1 && !node_mgr().visible_windows()) {

  // should not make the close button sensitive with only one booklist window
  // visible and no reading windows!
  if (f_visible_num > 1 && !node_mgr().visible_windows()) {
    List_Iterator<LibraryAgent *> i (f_agent_list);
    // we must walk down the list because we don't which one it is
    for (; i; i++)
	if (!i.item()->popped_down())
	    i.item()->close_set_sensitive(True);
  }
  f_visible_num++;
  // take care of the new visible agent
  if (f_visible_num > 1 || node_mgr().visible_windows())
      f_first_agent->close_set_sensitive(True);
  // notify the node manager when state has just changed
  if (f_visible_num == 1)
      node_mgr().windows_notify(True);
#endif

  //
  //  MAKE SURE WE RUN PURIFY TO CHECK FOR THE POSSIBLE
  //  MEMORY LEAK BELOW.
  //
  // make outlinelist to display in library agent
  //
  OutlineList *ol = new OutlineList(fObjList.length());
  for (i = 0; i < fObjList.length(); i ++) {
    ol->append (new TOC_Element (fObjList[i]));
    if (toc->parent() == (const int)0) {
      BitHandle handle = ol->get_data_handle();
      ((OutlineElement *) (*ol)[i])->set_expanded (handle);
      ol->free_data_handle (handle);
    }
  }
  f_visible_num = 1;
  // notify the node manager when state has just changed
  node_mgr().windows_notify(True);
  agent->display (ol);
}

// /////////////////////////////////////////////////////////////////
// display - display a detached part of a doc tree
// /////////////////////////////////////////////////////////////////

void
LibraryMgr::display (OutlineElement *subtree)
{
  //cerr << "f_visible_num: " << f_visible_num << endl;
  assert (subtree != NULL);
  // Check for existing agent
  List_Iterator<LibraryAgent *> l (f_agent_list);
  for (; l; l++)
    {
      if (((TOC_Element *)((*(l.item()->outline_list()))[0]))->toc() ==
	  ((TOC_Element *) subtree)->toc())
	{
	  l.item()->popup();
	  return;
	}
    }

  LibraryAgent *library_agent = new LibraryAgent();
  f_agent_list.append (library_agent);
  // if we only had one visible agent find it and set it correctly
  if (f_visible_num == 1 && !node_mgr().visible_windows()) {
    List_Iterator<LibraryAgent *> i (f_agent_list);
    // we must walk down the list because we don't which one it is
    for (; i; i++)
	if (!i.item()->popped_down())
	    i.item()->close_set_sensitive(True);
  }
  f_visible_num++;
  // take care of the new visible agent
  if (f_visible_num > 1 || node_mgr().visible_windows())
      library_agent->close_set_sensitive(True);
  // notify the node manager when state has just changed
  if (f_visible_num == 1)
      node_mgr().windows_notify(True);

  // Must create an OutlineList to hold the specified OutlineElement.
  // The OutlineListView needs an OutlineList at the toplevel, not
  // an OutlineElement.
  OutlineList *sublist = new OutlineList (1);
  sublist->append (subtree);

  library_agent->display (sublist);
  // Need to watch for destroy so we can nuke the sublist.
  Observed (library_agent, FolioObject::DESTROYED,
	    &LibraryMgr::agent_destroyed, library_agent);
}

///////////////////////////////////////////////////////////////////////////////
//  undisplay 
///////////////////////////////////////////////////////////////////////////////

void
LibraryMgr::undisplay (OutlineElement *root) {
    UAS_Pointer<UAS_Common> rootDoc = ((TOC_Element *) root)->toc();
    if (rootDoc->type() != UAS_LIBRARY)
	throw (CASTEXCEPT Exception());
    unsigned int i;
    for (i = 0; i < fObjList.length(); i ++) {
	if (fObjList[i] == rootDoc) {
	    break;
	}
    }
    if (i >= fObjList.length()) {
	// internal error -- this shouldn't happen.
	throw (CASTEXCEPT Exception());
    }
    UAS_Common::destroy (fObjList[i]);
}

void
LibraryMgr::remove (UAS_Pointer<UAS_Common> lib) {
  List_Iterator<LibraryAgent *> l (f_agent_list);
  for (; l; l++) {
    l.item()->library_removed (lib);
  }
  for (unsigned int i = 0; i < fObjList.length(); i ++) {
    if (fObjList[i] == lib) {
	fObjList.remove_item(i);
	break;
    }
  }
}


// /////////////////////////////////////////////////////////////////
// agent_destroyed
// /////////////////////////////////////////////////////////////////

void
LibraryMgr::agent_destroyed (LibraryAgent *, u_int, void *,
			     LibraryAgent *agent)
{
  ON_DEBUG (puts ("Removing library agent from cache:"));
  // Remove the destroyed agent from the cache list. 
  List_Iterator<LibraryAgent *> l (f_agent_list);
  for (; l; l++)
    {
      if (l.item() == agent)
	{
	  ON_DEBUG (puts ("   got it!"));
	  f_agent_list.remove (l);
	  agent_unmapped(agent);
	  return;
	}
    }
}

// /////////////////////////////////////////////////////////////////
// agent_unmapped
// /////////////////////////////////////////////////////////////////
void
LibraryMgr::agent_unmapped (LibraryAgent *)
{
  //f_visible_num--;
  f_visible_num = 0;
#ifdef AllowDetach
  // make sure the user can't just close the last visible window
  if (f_visible_num == 1 && !node_mgr().visible_windows()) {
    List_Iterator<LibraryAgent *> i (f_agent_list);
    // we must walk down the list because we don't which one it is
    for (; i; i++)
	i.item()->close_set_sensitive(False);
  }
#endif
  // notify the node manager when state has just changed
  //if (f_visible_num == 0)
  node_mgr().windows_notify(False);
}

// /////////////////////////////////////////////////////////////////
// windows_notify - the Node mgr visible windows state changes
// /////////////////////////////////////////////////////////////////

void
LibraryMgr::windows_notify (bool visible)
{
  if (f_visible_num == 1)
    f_first_agent->close_set_sensitive(visible);
  
#ifdef AllowDetach
  // it only matters if we have only one agent visible
  if (f_visible_num == 1) {
    List_Iterator<LibraryAgent *> i (f_agent_list);
    // we must walk down the list because we don't which one it is
    for (; i; i++)
	i.item()->close_set_sensitive(visible);
  }
#endif
}


OutlineList *
LibraryMgr::library()
{
  if (f_first_agent != NULL)
    return (f_first_agent->outline_list());
  else
    return (NULL);
}

void
LibraryMgr::init(UAS_List<UAS_String> &infolibpaths)
{
  unsigned int i;

  UAS_Common::initialize(infolibpaths);

  UAS_List<UAS_String> locList = UAS_Common::rootLocators ();

  for (i = 0; i < locList.length(); i ++) {
    mtry
    {
      UAS_Pointer<UAS_Common> d =
          UAS_Common::create (*(UAS_String*)locList[i]);
      if ((void *)d != NULL) {
#ifdef DO_NOT_RETRIEVE
        UAS_Pointer<UAS_Collection> col = (UAS_Collection*)(UAS_Common*)d;
        UAS_Pointer<UAS_Common> library = col->root();
        library_mgr().display(library);
#else
        d->retrieve();
#endif
      }
      else
        message_mgr().error_dialog(window_system().get_message ("NoDocument"));
    }
    mcatch(demoException &, demo)
    {
      message_mgr().demo_failure(demo);
    }
    end_try;
  }
}

bool
LibraryMgr::lib_exist(UAS_String& lid)
{
  for (unsigned int i = 0; i < fObjList.length(); i ++) {
    if (fObjList[i]->lid() == lid)
      return True;
  }
  return False;
}
