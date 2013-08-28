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
 * $XConsortium: NodeMgr.C /main/10 1996/08/30 18:53:32 cde-hal $
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

#ifdef DUMP_NODES
#include <sys/fcntl.h>
#endif
#include <X11/Xlib.h>

#define C_TOC_Element
#define L_Basic

#define C_Stack
#define L_Support

#define C_NodeViewInfo
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_NodeWindowAgent
#define L_Agents

#define C_PrefMgr
#define C_NodeMgr
#define C_NodeWindowMgr
#define C_StyleSheetMgr
#define C_LibraryMgr
#define C_GraphicsMgr
#define L_Managers

#define C_xList
#define L_Support

#include "Prelude.h"

#include "Managers/CatMgr.hh"
#include "Registration.hh"

#ifdef JBM
#include <tml/tml_objects.hxx>
#endif

#include "DocParser.h"
#include "Resolver.h" 
#include "StyleSheet.h"
#include "StyleSheetExceptions.h"
#ifdef JBM
#include "../OnlineRender/TmlRenderer.hh"
#else
#include "../OnlineRender/CanvasRenderer.hh"
#endif

#include <sstream>

class NodeWindowAgent;
class NodeHandle;

LONG_LIVED_CC(NodeMgr,node_mgr);

class DisplayNode : public UAS_Receiver<UAS_DocumentRetrievedMsg>
{
public:
  DisplayNode()
    { UAS_Common::request ((UAS_Receiver<UAS_DocumentRetrievedMsg> *) this); }
private:
  void receive (UAS_DocumentRetrievedMsg &message, void *client_data);
};

void
DisplayNode::receive (UAS_DocumentRetrievedMsg &message, void *client_data)
{
    ON_DEBUG (cerr <<"Got Node display message!" << endl);
    size_t cd = (size_t)client_data;
    
    //  0 == display request

    if (cd == 0) {
	node_mgr().display(message.fDoc);
    }
    else {
	return;
    }
}

static DisplayNode node_displayer;

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

NodeMgr::NodeMgr()
: f_force_new_window (FALSE),
  f_preferred_window (NULL),
  f_font_scale(0)
{
  set_font_scale(pref_mgr().get_int(PrefMgr::FontScale));
  g_node_mgr = this;  // don't remove this line or you will suffer!! (11036) 
} 

// /////////////////////////////////////////////////////////////////
// display - display a node
// /////////////////////////////////////////////////////////////////

void
NodeMgr::display (UAS_Pointer<UAS_Common> &node_ptr)
{
  Wait_Cursor bob;
  f_last_displayed = node_ptr;
  NodeWindowAgent *nwa = NULL;

  // Find an unlocked agent.  Implement other agent choosing policy here.
  if (!f_force_new_window)
    {
      //
      //  SWM: Sanity check. Make sure f_preferred_window, if set,
      //  is set to some existing window.
      //
      if (f_preferred_window) {
	  int found = 0;
	  List_Iterator<NodeWindowAgent *> li (f_agent_list);
	  for ( ; li; li ++) {
	    if (li.item() == f_preferred_window) {
		found = 1;
		break;
	    }
	  }
	  if (!found)
	    f_preferred_window = 0;
      }
      if (f_preferred_window != NULL && !f_preferred_window->locked())
	{
	  nwa = f_preferred_window;
	}
      else
	{
	  List_Iterator<NodeWindowAgent *> i (f_agent_list);
	  for (; i; i++)
	    if (!i.item()->locked())
	      {
		nwa = i.item();
		break;
	      }
	}
    }
  else
    f_force_new_window = FALSE;   // reset 

  // Didn't find one, so create a new one. 
  if (nwa == NULL)
    nwa = create_agent();

  f_preferred_window = NULL;

  nwa->display (node_ptr);
}


// /////////////////////////////////////////////////////////////////
// create_agent - make a new agent
// /////////////////////////////////////////////////////////////////

NodeWindowAgent *
NodeMgr::create_agent()
{
  // take care of the case where we only had one node agent
  if (f_agent_list.length() == 1 && !library_mgr().visible_windows()) {
    List_Iterator<NodeWindowAgent *> i (f_agent_list);

    i.item()->close_set_sensitive(True);
  }

  NodeWindowAgent *nwa = new NodeWindowAgent();
  f_agent_list.append (nwa);

  // take care of the new agent
  if (f_agent_list.length() > 1 || library_mgr().visible_windows())
    nwa->close_set_sensitive(True);

  // notify the library manager when state has just changed
  if (f_agent_list.length() == 1)
    library_mgr().windows_notify(True);

  return (nwa);
}

// /////////////////////////////////////////////////////////////////
// agent_delete - an agent has been deleted
// /////////////////////////////////////////////////////////////////

void
NodeMgr::agent_deleted (NodeWindowAgent *agent)
{
  if (f_preferred_window == agent)
    f_preferred_window = NULL;

  f_agent_list.remove (agent);

  // make sure the user can't just close the last visible window
  if (f_agent_list.length() == 1 && !library_mgr().visible_windows()) {
    List_Iterator<NodeWindowAgent *> i (f_agent_list);

    i.item()->close_set_sensitive(False);
  }
  // notify the library manager when state has just changed
  if (f_agent_list.length() == 0)
      library_mgr().windows_notify(False);

  // f_last_displayed points to a UAS_Common object. When the UAS_Common
  // gets deleted, this pointer does not get cleared. This causes
  // a core dump when adding and removing infolibs after a Node Window
  // has been displayed.
  UAS_Pointer<UAS_Common> d = agent->node_view_info()->node_ptr();
  if (d == f_last_displayed)
    f_last_displayed = NULL;
}

// /////////////////////////////////////////////////////////////////
// windows_notify - the Library mgr visible windows state changes
// /////////////////////////////////////////////////////////////////

void
NodeMgr::windows_notify (bool visible)
{
  // it only matters if we have only one agent
  if (f_agent_list.length() == 1) {
    List_Iterator<NodeWindowAgent *> i (f_agent_list);

    i.item()->close_set_sensitive(visible);
  }
}

// /////////////////////////////////////////////////////////////////
// book_name - return the name of a book given a node
// /////////////////////////////////////////////////////////////////


void
NodeMgr::re_display_all()
{
  List_Iterator<NodeWindowAgent *> cursor(f_agent_list);
  for (; cursor; cursor++)
    cursor.item()->re_display();
}

void
NodeMgr::set_font_scale(int value)
{
  int real_value = value * 2 ;
  if (real_value < - MIN_DOWNSCALE)
    real_value = - MIN_DOWNSCALE ;

  int change_amount = real_value - f_font_scale ;
  if (change_amount == 0)
    return ;

  // save the new font scale
  f_font_scale = real_value ;

#ifdef JBM
  // 12/27/95 - this section was used to do live updates of font
  // scaling which we are not doing anymore

  tml_chg_ptsize(change_amount);
  // node_parser().spacing(node_parser().spacing() + change_amount);


  List_Iterator<NodeWindowAgent *> i (f_agent_list);
  for (; i; i++)
    i.item()->update_fonts();
#endif
}



NodeViewInfo *
NodeMgr::load(UAS_Pointer<UAS_Common> &node_ptr)
{
  extern int styleparse();
  extern void stylerestart(FILE *);
  extern NodeViewInfo *gNodeViewInfo;

#ifdef FILE_STYLE_SHEET
  static int first = 0;

  StyleSheet ss ;
  {
    extern istream *g_stylein;
    ifstream input("style");
    input.unsetf(ios::skipws);
    if (input.fail())
      {
	cerr << "Could not open Style Sheet ./style" << endl;
	throw(Exception());
      }

    g_stylein = &input ;
    if (first)
      stylerestart(0);
    first=1 ;
    styleparse();
  }
#else
  mtry
    {
      style_sheet_mgr().initOnlineStyleSheet(node_ptr);
    }
  mcatch_noarg (StyleSheetSyntaxError)
    {
      message_mgr().error_dialog(
		(char*)UAS_String(CATGETS(Set_Messages, 39, "File a Bug")));
    }
  end_try ;
#endif


#ifdef DUMP_NODES
  {
    // doing this to dump nodes for testing 
    ofstream output("outfile");
    output << (char *) node_ptr->data() ;
  }
#endif

  istringstream input((char *) node_ptr->data());

  mtry
    {
#ifdef FONT_SCALE_DEBUG
      cerr << "PrefMgr::FontScale: " <<
	pref_mgr().get_int(PrefMgr::FontScale) << endl; 
#endif
      // assign node_ptr to global variable that TmlRenderer can pick up 
      gNodeViewInfo = new NodeViewInfo(node_ptr);
      CanvasRenderer	renderer (pref_mgr().get_int(PrefMgr::FontScale)) ;
      Resolver resolver(*gPathTab, renderer);
      DocParser docparser(resolver);
      docparser.parse(input);
    }
  mcatch_any()
    {
      ON_DEBUG(cerr << "NodeMgr::load...exception thrown" << endl);
      delete gNodeViewInfo ;
      gNodeViewInfo = 0;
      rethrow;
    }
  end_try;

  // TmlRenderer set this up for us 
  return gNodeViewInfo ;
}

/*
void
styleerror(char *error)
{
  ON_DEBUG(cerr << "NodeMgr->styleerror: Style Sheet Error: " << error << endl);
  throw(StyleSheetSyntaxError());
}
*/
