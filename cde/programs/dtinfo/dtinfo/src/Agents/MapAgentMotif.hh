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
 * $XConsortium: MapAgentMotif.hh /main/5 1996/09/17 13:14:47 cde-hal $
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

# include "UAS.hh"

class MapButton;
class WTopLevelShell;
class WXawPorthole;
class WXawTree;
class WXawPanner;
class WXmToggleButton;
class WXmPushButton;


class MapAgent : public Agent, public UAS_Receiver<UAS_DocumentRetrievedMsg>
{
public: // types
  enum map_mode_t { LOCAL_MODE, GLOBAL_MODE };

public: // functions
  MapAgent();
  map_mode_t mode() const;
  void set_mode (const map_mode_t mode);

  bool can_display (UAS_Pointer<UAS_Common> &) const;
  void display (UAS_Pointer<UAS_Common> &, bool popup);
  bool can_update();

  // to be called when preferences change it
  void lock();
  void unlock();

  void clear_map(UAS_Pointer<UAS_Common>&);

private: // functions
  void create_ui();
  void porthole_resize (WCallback *);
  void panner_movement (WCallback *);
  void close_window (WCallback *wcb);
  void lock_toggle (WCallback *);
  void center_on (MapButton *);

  void receive (UAS_DocumentRetrievedMsg &message, void *client_data);

  void clear_it();

private: // variables
  bool         f_locked;
  map_mode_t      f_map_mode;
  
  WTopLevelShell        *f_shell;
  WXawPorthole          *f_porthole;
  WXawPanner            *f_panner;
  WXawTree              *f_tree;
  WCallback             *f_wm_delete_callback;
  WXmToggleButton       *f_lock;

  UAS_Pointer<UAS_Common>      f_doc_ptr;
  bool                   f_onscreen;
  Dimension              f_min_tree_width, f_min_tree_height;
};
