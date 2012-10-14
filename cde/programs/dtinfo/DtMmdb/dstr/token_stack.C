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
 * $XConsortium: token_stack.C /main/5 1996/08/21 15:51:52 drk $
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


#include "dstr/token_stack.h"

#ifdef C_API
#include "utility/c_stream.h"
#else
#include <sstream>
using namespace std;
#endif

token_stack::token_stack() : curr_token_start(0)
{
   v_curr_token_buf = new buffer(LBUFSIZ);
   v_curr_list_cell = new slist_void_ptr_cell(v_curr_token_buf);
   chunk_list.insert_as_tail(v_curr_list_cell);
}

token_stack::~token_stack() 
{
   long ind = chunk_list.first();
   while (ind) {
      slist_void_ptr_cell *p = (slist_void_ptr_cell*)ind;
      buffer *w = (buffer*)(p -> void_ptr());
      delete w;
      chunk_list.next(ind);
   }
}

void token_stack::clear()
{
   long ind = chunk_list.first();
   while (ind) {
      slist_void_ptr_cell *p = (slist_void_ptr_cell*)ind;
      buffer *w = (buffer*)(p -> void_ptr());
      w -> reset();
      chunk_list.next(ind);
   }

   v_curr_list_cell = (slist_void_ptr_cell*)(chunk_list.get_head());
   v_curr_token_buf = (buffer*)(v_curr_list_cell -> void_ptr());
}

void token_stack::new_token()
{
   if ( curr_token_start ) 
      v_curr_token_buf -> put(char(0));

   curr_token_start = 0;
}

void token_stack::add_partial_token(char* x) 
{
   if ( v_curr_token_buf -> remaining_sz() < strlen(x) + 1) {

      int partial_str_len = curr_token_start ? strlen(curr_token_start) : 0;

      buffer* new_buf = 0;

      slist_void_ptr_cell* next_cell = 
          (slist_void_ptr_cell*)(v_curr_list_cell -> v_succ); 

      if ( next_cell ) {
         new_buf = (buffer*)(next_cell -> void_ptr());

      } else {
         new_buf = new buffer(MAX(partial_str_len + 1, LBUFSIZ));
         next_cell = new slist_void_ptr_cell(new_buf);
         chunk_list.insert_as_tail(next_cell);
      }

      if ( curr_token_start ) {
         v_curr_token_buf -> put(char(0));
         new_buf -> put(curr_token_start, strlen(curr_token_start));
         v_curr_token_buf -> set_content_sz(
                    int(curr_token_start - v_curr_token_buf->get_base())
                                           );
      } 

      v_curr_list_cell = next_cell;
      v_curr_token_buf = new_buf;
   }

   if ( curr_token_start == 0 ) {
      curr_token_start = 
         v_curr_token_buf -> get_base() + v_curr_token_buf -> content_sz();
   }

   v_curr_token_buf -> put(x, strlen(x));
/*
MESSAGE(cerr, "add_partial_token():");
debug(cerr, last_buf -> content_sz());
*/
}

ostream& operator <<(ostream& out, token_stack& ts)
{
   long ind = ts.chunk_list.first();
   while (ind) {
      slist_void_ptr_cell *p = (slist_void_ptr_cell*)ind;
      buffer *w = (buffer*)(p -> void_ptr());
 
      int offset = 0; 
      while ( offset < w -> content_sz() ) {

         char* str = (char*)(w -> get_base() + offset);
         out << str << "\n";

         offset += strlen(str) + 1;
      }

      ts.chunk_list.next(ind);
   }
   return out;
}

