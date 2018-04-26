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
 * $XConsortium: chunks_index.cc /main/4 1996/07/18 14:52:47 drk $
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



#include "storage/chunks_index.h"

/****************************************************/
// record and comparison functions for string index
/****************************************************/

Boolean str_index_record_ls(const void* o1, const void* o2)
{
   const str_index_record_t* x = (str_index_record_t*)o1;
   const str_index_record_t* y = (str_index_record_t*)o2;

/*
MESSAGE(cerr, "LS");
debug(cerr, x -> str_offset);
debug(cerr, x -> loc);
debug(cerr, y -> str_offset);
debug(cerr, y -> loc);
MESSAGE(cerr, "");
*/

   return ( x -> str_offset < y -> str_offset ) ? true : false;
}

Boolean str_index_record_eq(const void* o1, const void* o2)
{
   const str_index_record_t* x = (str_index_record_t*)o1;
   const str_index_record_t* y = (str_index_record_t*)o2;

/*
MESSAGE(cerr, "EQ");
debug(cerr, x -> str_offset);
debug(cerr, x -> loc);
debug(cerr, y -> str_offset);
debug(cerr, y -> loc);
MESSAGE(cerr, "");
*/

   return ( x -> str_offset == y -> str_offset ) ? true : false;
}

///////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////

chunks_index::chunks_index(abs_storage* store, 
                           mmdb_pos_t loc) : 
v_index_imp(str_index_record_eq, str_index_record_ls),
v_storage_ptr(store), v_initial_loc(loc)
{
   str_index_record_tPtr* vector = 0;
   int vector_sz = 0;

   if ( v_storage_ptr == 0 || v_initial_loc == 0 )
      return ;

   ((page_storage*)v_storage_ptr) -> get_str_locs(loc, vector, vector_sz);

/*
MESSAGE(cerr, "vector:");
debug(cerr, vector_sz);
for ( int i=0; i<vector_sz; i++ ) {
debug(cerr, i);
debug(cerr, vector[i] -> loc);
debug(cerr, vector[i] -> str_offset);
}
*/


   binary_insert(vector, 0, vector_sz-1);
   delete [] vector;
}

chunks_index::~chunks_index() 
{
    v_index_imp.del_elements(delete_str_index_record);
}

/***************************************************/
// Note: given that the keys in vector are non-
//       decreasing, it is optimal to binarilly
//       insert keys to obtained a
//       balanced binary search tree.
//
//       get_str_locs() returns a non-decreasing
//       key vector
/***************************************************/

Boolean
chunks_index::binary_insert(str_index_record_tPtr* vector,
                            int left, int right)
{
   if ( left > right )
     return true;

   int middle = (right + 1 - left) / 2 + left;
//debug(cerr, middle);
   v_index_imp.insert(vector[middle]);

   Boolean ok1 = binary_insert(vector, left, middle-1);
   Boolean ok2 = binary_insert(vector, middle+1, right);

   return  ( ok1 == true && ok2 == true ) ? true : false;
}


str_index_record_t* 
chunks_index::chunk_location( int offset )
{
   str_index_record_t key(offset);

   str_index_record_t* anchor = (str_index_record_t*)
        v_index_imp.smaller_member((root*)&key);

   if ( anchor == 0 ) {
      throw(stringException("chunk_location(): no smaller member"));
   }

   return anchor;
}

