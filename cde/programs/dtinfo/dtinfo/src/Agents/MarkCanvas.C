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
/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
* $XConsortium: MarkCanvas.cc /main/7 1996/07/10 09:35:53 rcs $
*
* Copyright (c) 1992 HAL Computer Systems International, Ltd.
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

#define C_OString
#define C_HashTbl
#define C_NodeViewInfo
#define C_NamedData
#define L_Basic

#define C_Mark
#define L_Marks

#define C_Anchor
#define L_Doc

#define C_MarkIcon
#ifdef UseTmlRenderer
#define C_ViewportAgent
#endif
#define C_MarkCanvas
#define L_Agents

#define C_MessageMgr
#define L_Managers

#include <Prelude.h>
#include <string.h>

#define CLASS MarkCanvas
STATIC_SENDER_CC (MarkSelectionChanged);

#define CURRENT_FORMAT "TML-1"

MarkCanvas *MarkCanvas::g_selected_mark;

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

MarkCanvas::MarkCanvas (UAS_Pointer<Mark> mark_ptr, Agent *agent,
			unsigned int offset, unsigned int ypos)
: f_mark_ptr (mark_ptr),
  f_mark_icon (NULL),
  f_full_offset (offset),
  f_y_position (ypos),
  f_agent (agent)
{
  // Request MarkChanged events.
  Mark::request ((UAS_Receiver<MarkChanged> *) this);
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

MarkCanvas::~MarkCanvas()
{
  if (selected())
    deselect();
}




// /////////////////////////////////////////////////////////////////
// select
// /////////////////////////////////////////////////////////////////

void
MarkCanvas::select(bool move_to)
{
  if (g_selected_mark != NULL)
    g_selected_mark->deselect();
  g_selected_mark = this;

  // re-computation is necessary after searching. for now let me
  // re-compute location every time - 10/31/94 kamiya
#ifdef JBM
  compute_location();

  if (f_draw_mark == NULL)
    {
      view_port &vp = f_viewport_agent->tml_viewport();
      f_draw_mark =
	vp.mark_region (f_model_range.get_start(), f_model_range.get_end(),
			vp.get_ftbl()->ptr_2_tml_dp_inv);
    }
#endif
  // Send a message to anyone listening that a Mark has been selected. 
  // Generally this will be the NodeWindowAgent since it needs to
  // senzitize some menu entries. 
  MarkSelectionChanged message;
  message.f_selection_type = MarkSelectionChanged::SELECT;
  message.f_move_to = move_to ;
  send_message (message);

  // Tell the MarkIcon so that it can hightlight itself.
  if (f_mark_icon != NULL)
    f_mark_icon->select (this);
}


// /////////////////////////////////////////////////////////////////
// deselect
// /////////////////////////////////////////////////////////////////

void
MarkCanvas::deselect()
{
  // Send a message indicating that this Mark is no longer selected.
  // Generally this will be the NodeWindowAgent since it needs to
  // desenzitize some menu entries. 
  MarkSelectionChanged message;
  message.f_selection_type = MarkSelectionChanged::DESELECT;
  send_message (message);

  g_selected_mark = NULL;

#ifdef JBM
  // Call the TML routine that removes the Mark highlighting. 
  if (f_draw_mark != NULL)
    {
      f_viewport_agent->tml_viewport().detach_mark (f_draw_mark);
      f_draw_mark = NULL;
    }
#endif

  // Tell the MarkIcon so that it can un-hightlight itself.
  if (f_mark_icon != NULL)
    f_mark_icon->deselect (this);
}


// /////////////////////////////////////////////////////////////////
// receive - handle mark chagned message
// /////////////////////////////////////////////////////////////////

void
MarkCanvas::receive (MarkChanged &message, void *client_data)
{
  if (message.f_mark_ptr != f_mark_ptr)
    return;
  // Let the icon know that something is different about this Mark. 
  if (f_mark_icon != NULL)
    f_mark_icon->update_icon (this);
}
