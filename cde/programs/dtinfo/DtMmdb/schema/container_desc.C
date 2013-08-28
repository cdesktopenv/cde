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
 * $XConsortium: container_desc.cc /main/5 1996/07/18 14:49:46 drk $
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


#include "schema/container_desc.h"

container_desc::container_desc(int code, const char* comment) : 
	stored_object_desc(code, comment), index_nms(10)
{
}

container_desc::~container_desc()
{
   int ind = index_nms.first();
   char* idx_nm = 0;

   while ( ind != -1 ) {

      idx_nm = (char*)index_nms[ind];

      delete idx_nm;

      index_nms.next(ind);
   }
}

void container_desc::set_index_nm(const char* str)
{
   if ( index_nms.count() <= index_nms.no_elmts() ) {
      index_nms.expandWith(2*index_nms.count());
   }

   int i = index_nms.no_elmts() + 1;

   int l = strlen(str);
   char *x = new char[l+1];
   memcpy(x, str, l);
   x[l] = 0;

//cerr << "container_desc::set_index_nm(): this = " << this << ", str=(long)" << (long)str << " i=" << i << endl;
   index_nms.insert(x, i);

   index_nms.reset_elmts(i);
}

ostream& container_desc::asciiOut(ostream& out, Boolean last)
{
   if ( index_nms.no_elmts() == 0 ) {
      stored_object_desc::asciiOut(out, last);
      return out;
   } 

   stored_object_desc::asciiOut(out, false);

   int ind = index_nms.first();
   char* index_nm = 0;
   int num_printed = 1;

   for ( int i=0; i<index_nms.count(); i++ ) {
//cerr << "ind=" << ind << endl;
     index_nm = (char*)index_nms[ind];
     index_nms.next(ind);

     if ( index_nm ) {
//cerr << "this=" << this << "; index_nm=(long)" << (long)index_nm << " ind=" << ind << endl;
       if ( num_printed < index_nms.no_elmts() )
         desc_print(out, index_nm);
       else {
         if ( last == true ) {
           desc_print_end(out, index_nm);
           break;
         } else
           desc_print(out, index_nm);
       }
       num_printed++;
     }
   }

   return out;

/*
   char* index_nm = 0;
   int ind = index_nms.first();
   int last_ind = 0;

cerr << "this=" << this << "; index_nms.no_elmts()=" << index_nms.no_elmts();
cerr << "first()=" << ind << endl;

   do {
      last_ind = ind;

      index_nms.next(ind);

      if ( index_nms[ind] == 0 )
         break;

   } while ( ind != -1 );

////////////////////////////
   stored_object_desc::asciiOut(out, false);

cerr << "last_ind=" << last_ind << endl;
cerr << "ind=" << ind << endl;

   while ( ind != last_ind ) {

     index_nm = (char*)index_nms[ind];
     index_nms.next(ind);

     if ( index_nm ) {
cerr << "this=" << this << "; index_nm=(long)" << (long)index_nm << " ind=" << ind << endl;
       desc_print(out, index_nm);
     }
   }

   index_nm = (char*)index_nms[last_ind];
 
   if ( last == true)
     desc_print_end(out, index_nm);
   else
     desc_print(out, index_nm);


   return out;
*/
}

void 
container_desc::init_index_array(object_dict& dict, 
                                 c_index_handlerPtr*& index_array, 
                                 int& idx_array_sz)
{
   c_index_handlerPtr* tmp_index_array = 
        new c_index_handlerPtr[index_nms.no_elmts()];

   idx_array_sz = 0;

   int i = 0;
   int ind = index_nms.first();
   char* idx_nm = 0;

   while ( ind != -1 ) {

      idx_nm = (char*)index_nms[ind];

      if ( idx_nm ) {

         c_index_handler* idx_handler = 
            (c_index_handler*)dict.get_handler(idx_nm);

         idx_array_sz = MAX(idx_array_sz, (*idx_handler) -> bound_to());

         tmp_index_array[i++] = idx_handler;
      }

      index_nms.next(ind);
//cerr << "ind=" << ind << endl;
   }

   idx_array_sz++;

   index_array = new c_index_handlerPtr[idx_array_sz];

   for ( ind=0; ind<idx_array_sz; ind++ ) 
      index_array[ind] = 0;

   for ( ind=0; ind<i; ind++ ) {
      int pos = (*tmp_index_array[ind]) -> bound_to();
      index_array[pos] = tmp_index_array[ind];
   }

   delete tmp_index_array;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

set_desc::set_desc() : container_desc(SET_CODE, "container	set")
{
}

handler* set_desc::init_handler(object_dict& dict) 
{
   page_storage* store = (page_storage*)dict.get_store(get_store_nm());

   if ( v_oid.icode() == 0 ) {
      v_handler_ptr = new handler(SET_CODE, store);
      desc::set_oid(v_handler_ptr -> its_oid());
   } else
      v_handler_ptr = new cset_handler(v_oid, store);

/////////////////////////////
// init index array 
/////////////////////////////
   c_index_handlerPtr* idx_array = 0; int sz = 0;

   init_index_array(dict, idx_array, sz) ;

//debug(cerr, v_handler_ptr -> its_oid());
//debug(cerr, int(v_handler_ptr -> operator ->()));

   (*(cset_handler*)v_handler_ptr) -> init_data_member(idx_array, sz);

   return v_handler_ptr;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

list_desc::list_desc() : container_desc(DL_LIST_CODE, "container	list")
{
}

handler* list_desc::init_handler(object_dict& dict) 
{
   page_storage* store = (page_storage*)dict.get_store(get_store_nm());

   if ( v_oid.icode() == 0 ) {
      v_handler_ptr = new handler(DL_LIST_CODE, store);
      desc::set_oid(v_handler_ptr -> its_oid());
   } else
      v_handler_ptr = new dl_list_handler(v_oid, store);

/////////////////////////////
// init index array 
/////////////////////////////
   c_index_handlerPtr* idx_array = 0; int sz = 0;

   init_index_array(dict, idx_array, sz) ;

//debug(cerr, v_handler_ptr -> its_oid());
   (*(dl_list_handler*)v_handler_ptr) -> init_data_member(idx_array, sz);

   return v_handler_ptr;
}


