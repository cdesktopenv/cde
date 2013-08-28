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
 * $XConsortium: heap.cc /main/4 1996/07/18 14:29:27 drk $
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


#include "dstr/heap.h"

heap::heap(cmp_func_ptr_t eq,    cmp_func_ptr_t ls, int max) :
buffer(MAX(1, max)*sizeof(voidPtr)), f_cmp_func_eq(eq), f_cmp_func_ls(ls)
{
// the 0th element is never used.
   char z[16];

   memset(z, 0, 16);

   buffer::put(z, sizeof(voidPtr)); 
   v_updated = false;
}

heap::~heap()
{
/*
   voidPtr *heap_space = (voidPtr*)buffer::get_base();

   int ct = count(); 

   for ( int i=1; i<=ct; i++ )
      delete heap_space[i];
*/
}

/*
int heap::count()
{ 
// the 0th element is excluded
   return buffer::content_sz() / sizeof(voidPtr) - 1; 
} 
*/

Boolean heap::insert(voidPtr elm) 
{
   if ( buf_sz() < (int)(content_sz() + 2*sizeof(voidPtr)) )
      buffer::expand_chunk(2*buf_sz()) ;

   long x = long(elm);
   buffer::put(x);

   v_updated = true;

   return true;
}

Boolean heap::insert_heapify(voidPtr elm) 
{
   insert(elm);

   int j = count(); int i = j/2;

   voidPtr *heap_space = (voidPtr*)buffer::get_base();

   while ( i > 0 && (*f_cmp_func_ls)(heap_space[i], elm) == true ) {
      heap_space[j] = heap_space[i];
      j = i; i /= 2;
   }
   heap_space[j] = elm;

   return true;
}

void heap::adjust(int i, call_back_func_ptr_t call_back)
{
   voidPtr *heap_space = (voidPtr*)buffer::get_base();

   int j = 2*i;
   voidPtr item = heap_space[i];

   while ( j <= count() ) {
       if ( j < count() && 
            (*f_cmp_func_ls)(heap_space[j], heap_space[j+1]) == true 
          ) {
          j++;
       }
       if ( (*f_cmp_func_ls)(item, heap_space[j]) == false )
          break;
       else {
          heap_space[j/2] = heap_space[j];
          if ( call_back != 0 ) 
             (*call_back)(j/2, heap_space[j/2]); 
       }
    
       j *= 2;
   }
  
   heap_space[j/2] = item;

   if ( call_back != 0 ) 
      (*call_back)(j/2, item);
}

voidPtr heap::max_elm()
{
   voidPtr *heap_space = (voidPtr*)buffer::get_base();

   if ( count() > 0 )
      return heap_space[1];
   else
      return 0;
}

void heap::adjust_max_to(voidPtr new_max_item)
{
   voidPtr *heap_space = (voidPtr*)buffer::get_base();

   heap_space[1] = new_max_item;
   adjust(1);
   v_updated = true;
}

void heap::heapify()
{
   //for ( int i = (int)floor(double(count())/2); i>=1; adjust(i--) );

   for ( int i = count()/2; i>=1; adjust(i--) );
}

void heap::remove()
{
   set_content_sz(0);
   buffer::put((unsigned int)0);
   v_updated = false;
}

void heap::delete_max(call_back_func_ptr_t call_back)
{
//MESSAGE(cerr, "delete max");
   voidPtr *heap_space = (voidPtr*)buffer::get_base();

   int ct = count();

   heap_space[1] = heap_space[ct];
   set_content_sz(content_sz() - sizeof(voidPtr));

   ct--;

   adjust(1, call_back);
}

void heap::_delete_max(int i, voidPtr* heap_array)
{
   int target = ( heap_array[i+1] == 0 ) ? 2*i : 2*i+1;

   voidPtr tmp = heap_array[i];
   heap_array[i] = heap_array[target] ;
   heap_array[target] = tmp;

   if ( heap_array[target] != 0 )
      _delete_max(target, heap_array);

   return;
}

int heap::first()
{
   return ( count() >= 1 ) ? 1 : 0;
}

void heap::next(int& ind)
{
   ind = ( ind < count() ) ? (ind+1) : 0;
}

voidPtr heap::operator()(int ind)
{
   if ( INRANGE(ind, 1, count()) ) {
      voidPtr *heap_space = (voidPtr*)buffer::get_base();
      return heap_space[ind];
   } else
      return 0;
}

