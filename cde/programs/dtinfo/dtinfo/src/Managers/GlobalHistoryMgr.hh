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
 * $XConsortium: GlobalHistoryMgr.hh /main/5 1996/07/10 09:38:13 rcs $
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

#include "UAS.hh"

struct HistoryDelete;
struct HistoryAdd;
class NodeHistoryAgent;

class GlobalHistoryMgr : public Long_Lived,
			 public UAS_Sender<HistoryAdd>,
			 public UAS_Sender<HistoryDelete>,
			 public UAS_Receiver<UAS_LibraryDestroyedMsg>
{
public:
  GlobalHistoryMgr();
  ~GlobalHistoryMgr();

  void display();

  xList<UAS_Pointer<UAS_Common> > &history_list()
    { return (f_history_list); }
  void add (UAS_Pointer<UAS_Common> &);
  void set_max_length (int);
  void receive (UAS_LibraryDestroyedMsg &msg, void *client_data);

private:
  xList<UAS_Pointer<UAS_Common> >     f_history_list;
  unsigned int              f_max_length;

  NodeHistoryAgent         *f_history_agent;

  LONG_LIVED_HH (GlobalHistoryMgr,global_history_mgr);
};

LONG_LIVED_HH2 (GlobalHistoryMgr,global_history_mgr);


struct HistoryDelete : public Destructable
{
  int  f_index;    // Index of starting entry to nuke.
  int  f_count;    // Number of entries to remove.
  bool f_moving;   // True if the entry is about to be re-inserted. 
};

struct HistoryAdd : public Destructable
{
  HistoryAdd (UAS_Pointer<UAS_Common> &node_ptr)
    : f_new_entry(node_ptr), f_moving(false) { }

  UAS_Pointer<UAS_Common> f_new_entry;      // UAS_Pointer to UAS_Common displayed.
  bool f_moving;                  // True if a moved entry.
};
