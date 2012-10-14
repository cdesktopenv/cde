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
 * $XConsortium: Mark.C /main/4 1996/09/27 19:02:03 drk $
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


#define C_Mark
#define L_Marks

#define C_MessageMgr
#define L_Managers

#include <Prelude.h>

#define CLASS Mark

STATIC_SENDER_CC (EditMark);
STATIC_SENDER_CC (ViewMark);
STATIC_SENDER_CC (MarkDeleted);
STATIC_SENDER_CC (MarkChanged);


Mark::Mark()
: f_reference_count(0),
  f_deleted (FALSE)
{
}

Mark::~Mark()
{
  ON_DEBUG (printf ("Mark::~Mark() @ 0x%p\n", this));
  if (f_reference_count != 0)
   message_mgr().error_dialog((char*)"Attempt to delete mark with references!");
}


bool
Mark::operator== (const Mark &mark)
{
  // Base classes must override this method if two different Mark
  // objects can represent the same logical Mark. 
  if (this == &mark)
    return (TRUE);
  else
    return (FALSE);
}


void
Mark::save()
{
  if (f_deleted)
    {
      message_mgr().error_dialog((char*)"Cannot save changes to deleted mark.");
    }
  else
    {
      MarkChanged change_message;
      change_message.f_mark_ptr = this;
      ON_DEBUG (puts ("Mark::save(): sending message"));
      send_message (change_message);
      do_save();
    }
}

void
Mark::remove()
{
  // Anyone holding a non-temporary Pointer<Mark> damn well better
  // be handling this message! 
  MarkDeleted delete_message;
  delete_message.f_mark_ptr = this;
  // Underyling code might can exception in this call. 
  do_remove();
  send_message (delete_message);
  f_deleted = TRUE;
  // Mark will be deleted when all references are gone. 
}


// /////////////////////////////////////////////////////////////////
// edit & display
// /////////////////////////////////////////////////////////////////

void
Mark::edit()
{
  EditMark edit_message;

  edit_message.f_mark_ptr = this;

  ON_DEBUG (printf ("Mark::edit() sending message\n"));
  send_message (edit_message);
}


void
Mark::view()
{
  ViewMark display_message;

  display_message.f_mark_ptr = this;

  send_message (display_message);
}


void
Mark::reference()
{
  f_reference_count++;
}

void
Mark::unreference()
{
  if (--f_reference_count == 0)
    delete this;
}
