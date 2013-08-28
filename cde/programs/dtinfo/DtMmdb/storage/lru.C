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
 * $XConsortium: lru.cc /main/4 1996/07/18 14:53:29 drk $
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


#include "storage/lru.h"

lru::lru(int a_sz, int i_sz, Boolean remove_cells) : 
	rep_policy(a_sz, i_sz), active_list(remove_cells),
	inactive_list(remove_cells)
{
}

lru::~lru()
{
}

// Move the last cell in the active list to the end of the 
// inactive list.
//	Out: replaced points at the last cell from the active list;
//           it is zero if the active list is empty.
void lru::lower_last(rep_cell*& replaced)
{
// if the active list is empty, just return
   if ( active_list.count() == 0 ) 
      return ;

// delete the last cell from the active list
   dlist_cell *atl = active_list.get_tail();
   active_list.delete_cell(atl);


// insert the removed last cell into the inactive list (last position)
   dlist_cell *itl = inactive_list.get_tail();
   inactive_list.insert_after(itl, atl);

   replaced = (rep_cell*)atl;
   replaced -> set_position(INACTIVE);
}


Boolean 
lru::promote(rep_cell& x, rep_cell*& replaced)
{
/*
MESSAGE(cerr, "enter promote");
debug(cerr, int(this));
debug(cerr, active_list.count());
debug(cerr, inactive_list.count());
*/


   replaced = 0;

   switch (x.get_position()) {
     case ACTIVE:
//MESSAGE(cerr, "active");
       active_list.delete_cell(&x);
       break;

     case INACTIVE:
//MESSAGE(cerr, "inactive");
       inactive_list.delete_cell(&x);
       lower_last(replaced);
       break;

     case NOWHERE:
//MESSAGE(cerr, "nowhere");
       if ( active_sz > active_list.count() )
          break;
       else
       if ( active_sz == active_list.count() &&
            inactive_sz == inactive_list.count() ) {
          throw(stringException("lru::promote(): pool too small"));
        } else 
          lower_last(replaced);
       break;
   }
//debug(cerr, "to merge");

   active_list.insert_before(active_list.get_head(), &x);
   x.set_position(ACTIVE);
   return true;
}

Boolean lru::promote(rep_cell& x)
{
  rep_cell* dummy = 0;
  return this -> promote(x, dummy);
}

Boolean lru::derank(rep_cell& x, position_t opt)
{
   switch (x.get_position()) {
     case ACTIVE:
        //MESSAGE(cerr, "active status to derank");
        active_list.delete_cell(&x);
        break;

     case INACTIVE:
        //MESSAGE(cerr, "derand: an inactive cell");
        inactive_list.delete_cell(&x);
        opt = INACTIVE;
        break;

     case NOWHERE:
        throw(stringException("lru::derand(): nowhere status"));
   }

   switch ( opt ) {
     case ACTIVE:
        active_list.insert_as_tail(&x);
        x.set_position(ACTIVE);
        break;
     case INACTIVE:
        inactive_list.insert_as_tail(&x);
        x.set_position(INACTIVE);
        break;
     default:
        break;
   }

   return true;
}

long lru::first(position_t option)
{
   switch (option) {
      case ACTIVE:
         return active_list.first();
      case INACTIVE:
         return inactive_list.first();
      default:
         throw(stringException("lru::first(): bad option"));
   }
}

rep_cell* lru::operator()(long index, position_t option)
{
   switch (option) {
      case ACTIVE:
         return (rep_cell*)(index);
      case INACTIVE:
         return (rep_cell*)(index);
      default:
         throw(stringException("lru::operator(): bad option"));
   }
}

void lru::next(long& index, position_t option)
{
   switch (option) {
      case ACTIVE:
         active_list.next(index);
         return;
      case INACTIVE:
         inactive_list.next(index);
         return;
      default:
         throw(stringException("lru::next(): bad option"));
   }
}

long lru::last(position_t option)
{
   switch (option) {
      case ACTIVE:
         return active_list.last();
      case INACTIVE:
         return inactive_list.last();
      default:
         throw(stringException("lru::last(): bad option"));
   }
}

Boolean lru::remove(rep_cell& x)
{
  switch (x.get_position()) {
     case ACTIVE:
       active_list.delete_cell(&x);
       return true;

     case INACTIVE:
       inactive_list.delete_cell(&x);
       return true;

     case NOWHERE:
       throw(stringException("lru::last(): bad option"));
   }

   return false;
}

void lru::remove()
{
   active_list.remove();
   inactive_list.remove();
}

void lru::forget()
{
   active_list.empty_list();
   inactive_list.empty_list();
}

