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
 * $XConsortium: NodeHistoryAgentMotif.hh /main/5 1996/11/18 19:09:47 cde-hal $
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

# include "UAS.hh"

#include <WWL/WTopLevelShell.h>
#include <WWL/WXmList.h>
#include <WWL/WXmPushButton.h>


class NodeHistoryAgent : public WWL,
			 public UAS_Receiver<HistoryAdd>,
			 public UAS_Receiver<HistoryDelete>
{
public: // functions
  NodeHistoryAgent()
    : f_shell (NULL), f_selected_item (0), f_popped_up(FALSE)
    { }
  virtual ~NodeHistoryAgent();
  void display();

private: // functions
  void create_ui();
  void refresh_list();

  // Callbacks: 
  void select (WCallback *);
  void view();
  void close();
  void popdown();

  void free_history_list();

  void receive (HistoryAdd &, void *client_data);
  void receive (HistoryDelete &, void *client_data);

  XmString compose_entry(UAS_String book, UAS_String section);

private:  // variables
  WTopLevelShell f_shell;
  WXmList        f_list;
  WXmPushButton  f_display;

  int                   f_selected_item;
  bool                  f_popped_up;
};
