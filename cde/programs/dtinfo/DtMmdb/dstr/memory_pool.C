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
 * $XConsortium: memory_pool.cc /main/4 1996/07/18 14:29:52 drk $
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


#include "dstr/memory_pool.h"

#define NUM_CHUNKS 50

#ifdef C_API
memory_pool* g_memory_pool_ptr = 0;
#endif

///////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////

int g_carrier = 0;

chunk_carrier::chunk_carrier(int chk_sz, int chks ) :
chunk_sz(ll4(chk_sz)), max_chunks(chks)
{
/*
debug(cerr, chunk_sz);
debug(cerr, max_chunks);
debug(cerr, int(this));
*/

   alloc_sz = max_chunks * (chunk_sz + sizeof(chunk_manage_record_t));
g_carrier += alloc_sz;
   carrier_ptr = new char[alloc_sz];
}

chunk_carrier::~chunk_carrier()
{
   delete carrier_ptr;
}

dlist* chunk_carrier::init_ptrs()
{
   dlist* w = new dlist;

   int offset = 0;
   for ( int i=0; i<max_chunks; i++ ) {

      chunk_manage_record_t* x = 
        (chunk_manage_record_t*)(carrier_ptr + offset);

      x -> chunk_carrier_ptr = this;

      w -> insert_as_tail(x);

      offset += (chunk_sz + sizeof(chunk_manage_record_t));
   }

   return w;
}


///////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////

fix_chunk_pool::fix_chunk_pool(int x) : chunk_sz(x), chunks(1)
{
   init_one_chunk_carrier();
}

void fix_chunk_pool::init_one_chunk_carrier()
{
   chunk_carrier* x = new chunk_carrier(chunk_sz, NUM_CHUNKS);

   chunk_carrier_list.insert_as_tail(new dlist_void_ptr_cell(x));

   dlist* z = x -> init_ptrs() ;

   free_chunk_list.append( z );

   delete z;
}

fix_chunk_pool::~fix_chunk_pool()
{
   free_chunk_list.empty_list();

   long x = chunk_carrier_list.first();

   while ( x ) {
      chunk_carrier *y = (chunk_carrier*)
              (((dlist_void_ptr_cell*)x) -> void_ptr());
      delete y;
      chunk_carrier_list.next(x); 
   }

//debug(cerr, chunk_sz);
//debug(cerr, chunks);
}

char* fix_chunk_pool::alloc()
{
   if ( free_chunk_list.count() == 0 ) {
//chunks++;
      init_one_chunk_carrier();
   }

   chunk_manage_record_t *x = 
      (chunk_manage_record_t*)free_chunk_list.get_head();

   free_chunk_list.delete_cell(x);

   return (char*)( ((char*)x) + sizeof(chunk_manage_record_t) );
}

void fix_chunk_pool::free(char* x)
{
/*
   chunk_manage_record_t* chk_mng_ptr = 
      (chunk_manage_record_t*)
         (x - sizeof(chunk_manage_record_t));
*/

   free_chunk_list.insert_as_tail(
      (chunk_manage_record_t*)
         (x - sizeof(chunk_manage_record_t))
   );


//   chunk_carrier* z = chk_mng_ptr -> chunk_carrier_ptr;
//
//
//   int delta = x -  z -> carrier_ptr;
//
//   if ( INRANGE(delta, 0, z -> alloc_sz) )
///*
//   if ( ((char*)chk_mng_ptr - z -> carrier_ptr) % 
//        (z -> chunk_sz + sizeof(chunk_manage_record_t)) 
//           == 
//        0 
//      )
//*/
//     free_chunk_list.insert_as_tail(chk_mng_ptr);
//   else {
//     MESSAGE(cerr, "fix_chunk_pool::free(): bad ptr");
//     debug(cerr, int(x));
///*
//     debug(cerr, int((char*)chk_mng_ptr - z -> carrier_ptr));
//     debug(cerr, z ->chunk_sz + sizeof(chunk_manage_record_t));
//*/
//     debug(cerr, delta);
//     debug(cerr, int(z -> carrier_ptr));
//     abort();
//   }
}


///////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////

memory_pool::memory_pool(int x) : 
max_alloc_size_from_pool(x), vm_pool_vector(MAX_CHUNK_SZ)
{
//MESSAGE(cerr, "memory_pool cstr");
//debug(cerr, int(this));
}

int g_memory_size = 0;

memory_pool::~memory_pool()
{
   for ( int i=4; i<MAX_CHUNK_SZ; i++ ) {
      delete (fix_chunk_pool*)vm_pool_vector[i];
   }
//cerr << "g_memory_size = " << g_memory_size << "\n";
//cerr << "g_carrier= " << g_carrier << "\n";
}


char* memory_pool::alloc(size_t sz)
{
/*
MESSAGE(cerr, "=======");
debug(cerr, int(this));
debug(cerr, sz);
*/

//g_memory_size += sz;

   if ( INRANGE(sz, 4, MAX_CHUNK_SZ) ) {

       fix_chunk_pool *x = 
          (fix_chunk_pool*)vm_pool_vector[sz];

       if ( x == 0 ) {
//MESSAGE(cerr, "calling new fix_chunk_pool");
          x = new fix_chunk_pool(sz);
          vm_pool_vector.insert(x, sz);
       }
       
       return x -> alloc();
      
   } else {
       return new char[sz];
   }
//MESSAGE(cerr, "=======");
}

void memory_pool::free(char* str)
{
   int sz = ((chunk_manage_record_t*)
      (str - sizeof(chunk_manage_record_t))) ->
          chunk_carrier_ptr -> chunk_sz;

   if ( INRANGE(sz, 4, MAX_CHUNK_SZ) ) {

       fix_chunk_pool* x = (fix_chunk_pool*)vm_pool_vector[sz];

       if ( x == 0 ) {
          debug(cerr, sz);
          throw(stringException(
                "memory_pool::free(): fix_chunk_pool missing"
                               )
               );
       } else
          x -> free(str);
      
   } else {
       delete str;
   }
}

