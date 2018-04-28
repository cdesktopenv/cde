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
// $XConsortium: WXmOpButton.cc /main/3 1996/06/11 16:46:51 cde-hal $
#include "WXmOpButton.h"

/* Other activate options:
   Add another callback to a single function in the query editor
   which will snag new text and insert in the text field.  This
   means that we'll need to track what's to both sides of the cursor
   to be able to figure out how much text to grab.
*/

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

WXmOpButton::WXmOpButton (WComposite &father, const char *name,
			  PRoot *&root, long &position,
			  is_sensitive_func_t sfunc, insert_op_func_t ifunc)
: WXmPushButton (father, name, WAutoManage),
  f_root (root), f_position (position),
  f_is_sensitive (sfunc), f_insert (ifunc), f_sensitive (True)
{
  SetActivateCallback (this, (WWL_FUN) &WXmOpButton::insert_op);
}

// /////////////////////////////////////////////////////////////////
// set_sensitive - check sensitivity and set
// /////////////////////////////////////////////////////////////////

void
WXmOpButton::set_sensitive ()
{
  bool sense;
  if (f_position == -1)
    sense = FALSE;
  else
    sense = (*f_is_sensitive)(f_root, f_position);
  if (sense != f_sensitive)
    {
      SetSensitive (sense);
      f_sensitive = sense;
    }
}

// /////////////////////////////////////////////////////////////////
// insert_op - insert operation
// /////////////////////////////////////////////////////////////////

void
WXmOpButton::insert_op (WCallback *)
{
  (*f_insert)(f_root, f_position);
}
