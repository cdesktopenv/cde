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
 * $XConsortium: dlist.C /main/5 1996/08/21 15:51:48 drk $
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


#include "dstr/dlist.h"

#ifdef C_API
#include "utility/c_stream.h"
#else
#include <sstream>
using namespace std;
#endif

   
dlist::dlist(Boolean x) : 
	v_ct(0), v_head(0), v_tail(0), remove_cells_when_done(x)
{
}

dlist::~dlist() 
{
   if ( remove_cells_when_done == true )
      remove();
}

void dlist::remove() 
{
   long ind = first();
   while ( ind ) {
      dlist_cell *p = (dlist_cell*)ind;
      next(ind);
      delete p;
   }
}

void dlist::append(dlist* tail_list)
{
   if ( tail_list == 0 ) return;

   if ( v_tail != 0 )
      v_tail -> v_succ = tail_list -> v_head;

   if ( tail_list -> v_head )
      tail_list -> v_head -> v_pred = v_tail;

   if ( v_head == 0 )
      v_head = tail_list -> v_head;

   v_tail = tail_list -> v_tail;
   v_ct += tail_list -> v_ct;

   tail_list -> v_head = tail_list -> v_tail = 0;
   tail_list -> v_ct = 0;
}

void dlist::insert_before(dlist_cell* ref, dlist_cell* x) 
{
/*
debug(cerr, "insert before");
debug(cerr, int(x));
debug(cerr, int(head));
debug(cerr, int(tail));
*/
   if ( v_head == 0 ) {
      x -> v_pred = x -> v_succ = 0;
      v_head = v_tail = x;
   } else
   if ( ref == v_head ) {
      x -> v_succ = v_head ;
      x -> v_pred = 0;
      v_head = x;
      ref -> v_pred = x;
   } else {
      ref -> v_pred -> v_succ = x;
      x -> v_pred = ref -> v_pred;
      x -> v_succ = ref;
      ref -> v_pred = x;
   }
   v_ct++;
/*
debug(cerr, "insert before done");
debug(cerr, int(head));
debug(cerr, int(tail));
*/
}

void dlist::insert_after(dlist_cell* ref, dlist_cell* x) 
{
   if ( v_head == 0 ) {
      x -> v_pred = x -> v_succ = 0;
      v_head = v_tail = x;
   } else
   if ( ref == v_tail ) {
      ref -> v_succ = x;
      x -> v_succ = 0;
      v_tail = x;
      x -> v_pred = ref;
   } else {
      ref -> v_succ -> v_pred = x;
      x -> v_succ = ref -> v_succ;
      x -> v_pred = ref;
      ref -> v_succ = x;
   }
   v_ct++;
}

void dlist::insert_as_head(dlist_cell* x)
{
   if ( v_head == 0 ) {
      x -> v_pred = x -> v_succ = 0;
      v_head = v_tail = x;
   } else {
      v_head -> v_pred = x;
      x -> v_pred = 0;
      x -> v_succ = v_head;
      v_head = x;
   }
   v_ct++;
}

void dlist::insert_as_tail(dlist_cell* x)
{
   if ( v_head == 0 ) {
      x -> v_pred = x -> v_succ = 0;
      v_head = v_tail = x;
   } else {
      v_tail -> v_succ = x;
      x -> v_succ = 0;
      x -> v_pred = v_tail;
      v_tail = x;
   } 
   v_ct++;
}

void dlist::delete_cell(dlist_cell* x) 
{
/*
MESSAGE(cerr, "delete_cell()");
debug(cerr, int(x));
debug(cerr, int(head));
debug(cerr, int(tail));
debug(cerr, ct);
*/

   if ( x == 0 ) 
      return ;

   if ( x == v_head ) {
      if ( v_ct == 1 )
         v_head = v_tail = 0;
      else {
         v_head = v_head -> v_succ;
         v_head -> v_pred = 0;
      } 
   } else
      if ( x == v_tail ) {
         v_tail = v_tail -> v_pred;
         v_tail -> v_succ = 0;
      } else {
         x -> v_pred -> v_succ = x -> v_succ ;
         x -> v_succ -> v_pred = x -> v_pred ;
      } 

   v_ct--;

   x -> v_succ = x -> v_pred = 0;
}

long dlist::first()
{
   return long(v_head);
}

long dlist::last()
{
   return long (v_tail);
}

void dlist::next(long& index)
{
   if ( index == long(v_tail) )
      index = 0;
   else
      index = long( ((dlist_cell*)(index)) -> v_succ );
}

