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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $XConsortium: MapMgr.C /main/7 1996/09/17 13:17:11 cde-hal $
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

#define C_MapAgent
#define L_Agents

#define C_MapMgr
#define L_Managers

#include "Prelude.h"


class ClearNotify : public UAS_Receiver<UAS_LibraryDestroyedMsg >
{
public:
  ClearNotify();

private:
  void receive (UAS_LibraryDestroyedMsg &message, void *client_data);
};

ClearNotify::ClearNotify()
{
  UAS_Common::request ((UAS_Receiver<UAS_LibraryDestroyedMsg> *)this);
}

void
ClearNotify::receive (UAS_LibraryDestroyedMsg &message, void *client_data)
{
  map_mgr().clear_map (message.fLib);
}

static ClearNotify clear_notify;


LONG_LIVED_CC(MapMgr,map_mgr);

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

MapMgr::MapMgr()
: f_map_agent (NULL)
{
}


// /////////////////////////////////////////////////////////////////
// display_centered_on - display a localized map centered on NODE
// /////////////////////////////////////////////////////////////////

void
MapMgr::display_centered_on (UAS_Pointer<UAS_Common> &doc_ptr)
{
// eventual alg:
//  search the list of nodes for an available agent;
//  if no agents available, create one;
//   -- matching agent is unlocked and of the right mode
//  tell the agent to display the localized map;

// NOTE: display_centered_on is being called for creating or
//       raising the map agent. Therefore, it may always raise
//       the window.

  if (f_map_agent == NULL)
    f_map_agent = new MapAgent();

  f_map_agent->display (doc_ptr, TRUE);
}

// /////////////////////////////////////////////////////////////////////////
// set the preference toggle 
// /////////////////////////////////////////////////////////////////////////

void
MapMgr::set_auto_update(int value)
{
  if (f_map_agent != NULL)
    {
      if (value)
	f_map_agent->lock();
      else
	f_map_agent->unlock();
    }
}

void
MapMgr::clear_map(UAS_Pointer<UAS_Common> &lib)
{
  if (f_map_agent != NULL)
    f_map_agent->clear_map(lib);
}
