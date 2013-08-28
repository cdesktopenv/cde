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
 * $XConsortium: oid_list.cc /main/5 1996/07/18 14:43:28 drk $
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


#include "object/oid_list.h"

void oid_list::init_persistent_info(persistent_info* x)
{
   ostring* u = list_ptr.p; 

   root::init_persistent_info(x);

   if ( get_mode(OLD_OBJECT) == false ) {

//MESSAGE(cerr, "oid_list::init_persistent_info");
//debug(cerr, (void*)this);
//debug(cerr, my_oid());

     char* w = 0;
     int   v= 0;

//MESSAGE(cerr, "oid_list::init_persistent_info(), new object case");
//debug(cerr, (void*)this);
//debug(cerr, (void*)list_ptr.p);

     if (u) {
       w = u -> get();
       v = u -> size();
     } 

     if ( storage_ptr )
       storage_ptr -> insertString(list_ptr.loc, w, v);


//debug(cerr, list_ptr.loc);
//debug(cerr, v_sz);
   }

   delete u;
}

oid_list::oid_list(c_code_t c_id) : composite(c_id),
   f_internal_index(false), chk_index(0)
{
   list_ptr.loc = 0;
   init_data_member(0);
}

oid_list::oid_list(int num_oids, c_code_t c_id) : composite(c_id),
   f_internal_index(false), chk_index(0)
{
   list_ptr.loc = 0;
   init_data_member(num_oids);
}

oid_list::oid_list(oid_list& x) : 
   composite(x), chk_index(0)
{
   list_ptr.loc = 0;
   init_data_member(x.v_sz);

   for ( unsigned int i=1; i<=v_sz; i++ )
      update_component(i, x(i));
}

oid_list::~oid_list()
{
   if ( get_mode(PERSISTENT) == false ) {
      delete list_ptr.p;
   }

   delete chk_index;
}

void oid_list::init_data_member(int leng) 
{
   int char_leng = leng * OID_T_SZ ;
   char* ptr = new char[char_leng];

   oid_t x(ground);
  
   char* y = ptr;
   for ( int i=0; i<leng; i++ ) {
      x.to_char_string(y, false);
      y += OID_T_SZ;
   }

   list_ptr.loc = 0; // to please purify
   list_ptr.p = new ostring(ptr, char_leng); 

//MESSAGE(cerr, "oid_list::init_data_member(int leng)");
//debug(cerr, (void*)this);
//debug(cerr, (void*)list_ptr.p);

   delete ptr;
   v_sz = leng;
   set_mode(UPDATE, true);
}

void oid_list::build_internal_index() 
{
   if ( get_mode(OLD_OBJECT) == true &&
        get_mode(PERSISTENT) == true &&
        v_sz > 1000 
      ) 
      chk_index = new chunks_index(storage_ptr, list_ptr.loc);
   else
      chk_index = 0;
}

void oid_list::reqest_build_internal_index() 
{
   f_internal_index = true;
}

Boolean oid_list::expand_space(int extra_oids) 
{
/*
MESSAGE(cerr, "in expand_space()");
debug(cerr, my_oid());
debug(cerr, extra_oids);
debug(cerr, v_sz);
debug(cerr, int(list_ptr.loc));
debug(cerr, int(&list_ptr.loc));
*/

   Boolean ok;

   int extra_char_leng = extra_oids * OID_T_SZ;

   if ( get_mode(PERSISTENT) == true ) {

      int total_char_leng = extra_char_leng + v_sz * OID_T_SZ;

      char* ptr = new char[total_char_leng];

      memset(ptr, (char)0, total_char_leng);

      if ( list_ptr.loc == 0  ) {
         storage_ptr -> insertString(list_ptr.loc, ptr, total_char_leng);
      } else {
         storage_ptr -> appendString(list_ptr.loc, ptr, total_char_leng);
      }

      delete ptr;

      ok = true;

   } else {
      ok = list_ptr.p -> expand( extra_char_leng );
   }

   v_sz += extra_oids;

   set_mode(UPDATE, true);

   return ok;
}


io_status oid_list::asciiOut(ostream& out) 
{
   if ( !out ) 
      return fail;

   out << v_sz << '\n';

   int ind = first();

   while (ind ) {

      oid_t x = (*this)(ind);

      if ( x.eq(ground) == false ) {
         x.asciiOut(out);
         out << "\n";
      }
    
     next(ind);
   }

   return done;
}

oid_t oid_list::operator()(int ind) 
{
/*
MESSAGE(cerr, "oid_list::operator()");
debug(cerr, my_oid());

debug(cerr, ind);
debug(cerr, v_sz);
debug(cerr, int(this));
debug(cerr, int(list_ptr.p));
*/

   if ( !INRANGE(ind, 1, (int) v_sz) ) {
      MESSAGE(cerr, "oid_list::opeartor(): out of range");
      throw(boundaryException(1, v_sz, ind));
   }

   char z[OID_T_SZ];
   int offset = (ind - 1) * OID_T_SZ; 

   if ( get_mode(PERSISTENT) == true ) 
   {

      if ( list_ptr.loc == 0 )
         throw(stringException("zero loc value"));

      if ( f_internal_index == true && chk_index == 0 ) {
         build_internal_index();
      }

      if ( chk_index ) {
         str_index_record_t* anchor = 
            chk_index -> chunk_location(offset);

         storage_ptr ->
            readString(anchor -> loc, z, OID_T_SZ, 
                       offset - anchor -> str_offset);

      } else
         storage_ptr ->
               readString(list_ptr.loc, z, OID_T_SZ, offset);

   } else {

      if ( list_ptr.p == 0 )
         throw(stringException("zero list_ptr.p value"));

      memcpy(z, list_ptr.p -> get() + offset, OID_T_SZ);

   }
//MESSAGE(cerr, "oid_list::operator() done");

#ifdef PORTABLE_DB
   return oid_t(z, false, swap_order());
#else
   return oid_t(z, false, false);
#endif

}

Boolean 
oid_list::insert_component(const oid_t& new_comp_oid)
{
/*
MESSAGE(cerr, "oid_list::insert_component");
debug(cerr, v_sz);
debug(cerr, (void*)this);
debug(cerr, int(list_ptr.p));
new_comp_oid.asciiOut(cerr); cerr << "\n";
*/

   char z[OID_T_SZ];
   new_comp_oid.to_char_string(z, swap_order());

   if ( get_mode(PERSISTENT) == true ) {

      if ( list_ptr.loc == 0 ) {

         storage_ptr ->
              insertString(list_ptr.loc, z, OID_T_SZ);

      } else {
         storage_ptr ->
               appendString(list_ptr.loc, z, OID_T_SZ);
      }

   } else {
      list_ptr.p -> append(z, OID_T_SZ);
   } 

   v_sz++;

   set_mode(UPDATE, true);

   return true;
}

Boolean 
oid_list::update_component(int index, const oid_t& new_oid)
{
//MESSAGE(cerr, "oid_list::update_component()");
//debug(cerr, my_oid());
   if ( !INRANGE(index, 1, (int) v_sz) ) {
      MESSAGE(cerr, "oid_list update(): out of range");
      throw(boundaryException(1, v_sz, index));
   }

   char z[OID_T_SZ];

   Boolean swap;

   if ( get_mode(PERSISTENT) == true && swap_order() == true )
      swap = true;
   else
      swap = false;

   new_oid.to_char_string(z, swap);

   if ( get_mode(PERSISTENT) == true ) {

      if ( list_ptr.loc == 0 )
         throw(stringException("zero list_ptr.p value"));

      storage_ptr ->
         updateString(list_ptr.loc, z,
                      OID_T_SZ, (index-1) * OID_T_SZ
                     );

   } else {

//MESSAGE(cerr, "oid_list::update_component(): vm update");

      if ( list_ptr.p == 0 )
         throw(stringException("zero list_ptr.p value"));

      list_ptr.p -> update(z, OID_T_SZ, (index-1)*OID_T_SZ);

   } 
//MESSAGE(cerr, "oid_list::update_component() done");
   return true;
}

io_status oid_list::asciiIn(istream& in) 
{
//MESSAGE(cerr, "oid_list::asciiIn()");
   if ( in.get() != LIST_MARK_CHAR ) 
      throw(formatException("should be a LIST_MARK_CHAR"));

   if ( in.get() != '\n' ) 
      throw(formatException("should be a \n"));

   dlist temp_list;
   dlist_void_ptr_cell* y = 0;

   char c;
   while ( (c = in.get()) != LIST_MARK_CHAR ) {

      in.putback(c);

      oid_t *x = new oid_t(ground);
      x -> asciiIn(in);
      y = new dlist_void_ptr_cell(x);

      temp_list.insert_as_tail(y);

      v_sz++;
   }

   if ( in.get() != '\n' ) {
      throw(formatException("bad oid_list input stream"));
   }

   int oid_array_sz = v_sz * OID_T_SZ;
   char *oid_array = new char[oid_array_sz];

   long ind = temp_list.first();

   int i = 0;

   char z[OID_T_SZ];

   while ( ind ) {
      y = (dlist_void_ptr_cell*)(ind);

      ((oid_t*)(y -> void_ptr())) -> 
         to_char_string(z, swap_order());

      delete ((oid_t*)(y -> void_ptr()));

      memcpy(oid_array + i * OID_T_SZ, z, OID_T_SZ);
   
      i++;
      temp_list.next(ind);
   }
 
// save the list
   if ( get_mode(PERSISTENT) == true ) {

      if ( list_ptr.loc == 0 )
         storage_ptr -> insertString(list_ptr.loc, oid_array, oid_array_sz);
      else
         storage_ptr -> updateString(list_ptr.loc, oid_array, oid_array_sz);

      set_mode(UPDATE, true);

   } else {
      delete list_ptr.p;
      list_ptr.p = new ostring(0);
      list_ptr.p -> set(oid_array, v_sz);
   }

   delete oid_array;

   return done;
}

Boolean oid_list::remove_component(const oid_t& x)
{
   int ind = first();
   while ( ind ) {
   
      if ( (*this)(ind).eq(x) == true ) {

         update_component(ind, ground);

         set_mode(UPDATE, true);

         return true;
      }

     next(ind);
   }
   return false;
}



int oid_list::cdr_sizeof()
{
   return composite::cdr_sizeof() + sizeof(list_ptr.loc);
}

io_status oid_list::cdrOut(buffer& buf)
{
/*
MESSAGE(cerr, "oid_list::cdrOut()");
debug(cerr, (void*)this);
debug(cerr, (void*)list_ptr.loc);
debug(cerr, my_oid());
*/
   composite::cdrOut(buf);
   buf.put(list_ptr.loc);
   return done;
}

io_status oid_list::cdrIn(buffer& buf)
{
//MESSAGE(cerr, "oid_list::cdrIn()");
//debug(cerr, (void*)this);
   composite::cdrIn(buf);
   buf.get(list_ptr.loc);
//debug(cerr, (void*)list_ptr.loc);
//debug(cerr, my_oid());
   return done;
}

MMDB_BODIES(oid_list)

oid_list_handler::oid_list_handler(int num_oids, storagePtr store):
handler(OID_LIST_CODE, store)
{
   ((oid_list*)obj_ptr) -> init_data_member(num_oids);
}

oid_list_handler::oid_list_handler(const oid_t& v_oid, storagePtr store):
handler(v_oid, store)
{
}

oid_list_handler::~oid_list_handler()
{
}

oid_list* oid_list_handler::operator ->()
{
   return (oid_list*)handler::operator->();
}

