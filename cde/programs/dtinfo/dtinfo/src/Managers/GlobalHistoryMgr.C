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
 * $XConsortium: GlobalHistoryMgr.cc /main/5 1996/07/10 09:38:04 rcs $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
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
#include <sstream>
using namespace std;
#include "UAS.hh"

#define C_GlobalHistoryMgr
#define C_PrefMgr
#define L_Managers

#define C_xList
#define L_Support

#define C_NodeHistoryAgent
#define L_Agents

#include <Prelude.h>

LONG_LIVED_CC(GlobalHistoryMgr,global_history_mgr);

class NodeRecorder : public UAS_Receiver<UAS_DocumentRetrievedMsg>
{
public:
  NodeRecorder()
    { UAS_Common::request ((UAS_Receiver<UAS_DocumentRetrievedMsg> *)this); }
private:
  void receive (UAS_DocumentRetrievedMsg &message, void *client_data);
};

  
void
NodeRecorder::receive (UAS_DocumentRetrievedMsg &message, void *client_data)
{
  extern bool g_style_sheet_update ;
  if (!g_style_sheet_update)
    global_history_mgr().add (message.fDoc);
}

static NodeRecorder node_recorder;

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

GlobalHistoryMgr::GlobalHistoryMgr()
: f_max_length (pref_mgr().get_int (PrefMgr::NodeHistSize)),
  f_history_agent (NULL)
{
  // It should default to 100 from the resource file, but just in case: 
  if (f_max_length <= 0)
    f_max_length = 50;
  UAS_Common::request ((UAS_Receiver<UAS_LibraryDestroyedMsg> *) this);
}


GlobalHistoryMgr::~GlobalHistoryMgr()
{
  List_Iterator<UAS_Pointer<UAS_Common> > hl (f_history_list);
  while (hl)
    f_history_list.remove (hl);
  delete f_history_agent;
}


// /////////////////////////////////////////////////////////////////
// display - display the agent
// /////////////////////////////////////////////////////////////////

void
GlobalHistoryMgr::display()
{
  if (f_history_agent == NULL)
    f_history_agent = new NodeHistoryAgent();
  f_history_agent->display();
}


// /////////////////////////////////////////////////////////////////
// set_max_length - reset the maximum length
// /////////////////////////////////////////////////////////////////

void
GlobalHistoryMgr::set_max_length (int new_length)
{
  f_max_length = new_length;

  // Need to remove some entries if the current list is too long. 
  if (f_history_list.length() > f_max_length)
    {
      List_Iterator<UAS_Pointer<UAS_Common> > hl (f_history_list);
      int count = f_history_list.length() - f_max_length;

      ON_DEBUG (printf ("GlobalHistoryMgr: removing %d excess\n", count));
      HistoryDelete delmsg;
      delmsg.f_index = 0;
      delmsg.f_count = count;
      UAS_Sender<HistoryDelete>::send_message (delmsg, 0);

      while (count > 0)
	{
	  f_history_list.remove (hl);
	  count--;
	}
    }
}


// /////////////////////////////////////////////////////////////////
// add - add a node to the history list
// /////////////////////////////////////////////////////////////////

void
GlobalHistoryMgr::add (UAS_Pointer<UAS_Common> &node_ptr)
{
  // First see if the node is already in the history list.
  List_Iterator<UAS_Pointer<UAS_Common> > hl (f_history_list);
  int i;

  for (i = 0; hl; hl++, i++)
    {
      ON_DEBUG (printf ("%2d: Checking <%s>\n", i, (char*)hl.item()->title()));
      if (hl.item() == node_ptr)
	  break;
    }

  if (hl)  // Item exists, so remove it first. 
    {
      HistoryDelete delmsg;
      delmsg.f_index = i;
      delmsg.f_count = 1;
      delmsg.f_moving = TRUE;
      UAS_Sender<HistoryDelete>::send_message (delmsg, 0);
      f_history_list.remove (hl);
    }
  else if (f_history_list.length() + 1 > f_max_length)
    {
      hl.reset();
      f_history_list.remove (hl);
      HistoryDelete delmsg;
      delmsg.f_index = 0;
      delmsg.f_count = 1;
      delmsg.f_moving = FALSE;
      UAS_Sender<HistoryDelete>::send_message (delmsg, 0);
    }

  f_history_list.append (node_ptr);

  HistoryAdd addmsg (node_ptr);
  addmsg.f_moving = hl ? TRUE : FALSE;
  UAS_Sender<HistoryAdd>::send_message (addmsg, 0);
}

void
GlobalHistoryMgr::receive (UAS_LibraryDestroyedMsg &msg, void *client_data) {
    //
    //  Go through each element in the list, sending history
    //  delete messages for all docs contained in the dead library
    //
    int curIndex = 0;
    List_Iterator<UAS_Pointer<UAS_Common> > hl (f_history_list);
    while (hl) {
	if (hl.item()->get_library() == msg.fLib) {
	    HistoryDelete delmsg;
	    delmsg.f_index = curIndex;
	    delmsg.f_count = 1;
	    delmsg.f_moving = FALSE;
	    UAS_Sender<HistoryDelete>::send_message (delmsg, 0);
	    f_history_list.remove (hl);
	} else {
	    curIndex ++;
	    hl ++;
	}
    }
}
