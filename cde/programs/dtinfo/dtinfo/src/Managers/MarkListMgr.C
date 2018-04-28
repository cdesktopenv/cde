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
 * $XConsortium: MarkListMgr.cc /main/3 1996/06/11 16:26:15 cde-hal $
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

#define C_MarkBase
#define C_Mark
#define L_Odb

#define C_MarkListMgr
#define L_Managers

#define C_ListView
#define L_Agents

#include "Prelude.h"


LONG_LIVED_CC(MarkListMgr,mark_list_mgr);


// /////////////////////////////////////////////////////////////////
// MarkListMgr - class constructor
// /////////////////////////////////////////////////////////////////

MarkListMgr::MarkListMgr()
: f_mark_list (NULL), f_mark_list_view (NULL)
{
  g_mark_list_mgr = this;
}


// /////////////////////////////////////////////////////////////////
// display - display the mark list
// /////////////////////////////////////////////////////////////////

void
MarkListMgr::display()
{
  List *old_list = f_mark_list;

  f_mark_list = user_mark_base().get_all_marks();

  if (f_mark_list_view == NULL)
    {
      f_mark_list_view = new ListView (f_mark_list, "mark_list");
      Observe (f_mark_list_view, ListView::ENTRY_ACTIVATE,
	       &MarkListMgr::display_mark);
    }
  else
    {
      f_mark_list_view->list (f_mark_list);
    }

  f_mark_list_view->display();

  if (old_list != NULL)
    {
      old_list->remove_all (TRUE);
      delete old_list;
    }
}


// /////////////////////////////////////////////////////////////////
// display_mark
// /////////////////////////////////////////////////////////////////

void
MarkListMgr::display_mark (FolioObject *, u_int, int which_item)
{
  // Beware:  The mark will not be selected, because a new one is going
  // to be created when the node is displayed.  18:19 01/14/93 DJB 
  ((Mark *) (*f_mark_list)[which_item])->display_and_select();
}
