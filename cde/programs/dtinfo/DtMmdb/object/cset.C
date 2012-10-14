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
 * $XConsortium: cset.cc /main/5 1996/06/11 17:23:58 cde-hal $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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


#include "object/cset.h"
#include "utility/xtime.h"

cset::cset(c_code_t c_cd): composite(c_cd)
{
   num_indices = 0;
   indices = 0;
  
   set_mode(SWAP_ALLOWED, false);
}

void cset::init_data_member(c_index_handlerPtr* index_arrray, 
                         int iptrs
                        ) 
{
   indices = index_arrray;
   num_indices = iptrs;
   set_mode(HEALTH, true);

#ifdef MEMORY_MAPPED
   set_mode(UPDATE, true);
#endif
}

cset::~cset()
{
/*
MESSAGE(cerr, "~cset() called.");
debug(cerr, my_oid());
debug(cerr, num_indices);
debug(cerr, int(this));
MESSAGE(cerr, "~cset() called.");
debug(cerr, num_indices);
*/

   delete indices;
}

handler* cset::get_component(int index) 
{
   if ( indices[0] == 0 )
      return 0;

   oid_t id_ptr = (*indices[0]) -> first_of_invlist(index);

   handler* y = new handler(id_ptr, storage_ptr);

   return y;
}

oid_list_handler* cset::get_locs(handler& query, int index)
{
   if ( !INRANGE(index, 0, num_indices-1) ) {
      MESSAGE(cerr, "cset::get_locs(): invalid index");
      throw(boundaryException(0, num_indices-1, index));
   }

   if ( indices[index] == 0 ) {
      MESSAGE(cerr, "cset::get_locs(): invalid index");
      throw(stringException("NULL index ptr"));
   }

   return (*indices[index]) -> get_loc_list(query);
}

oid_t cset::get_first_oid(const handler& query, int index)
{
   if ( !INRANGE(index, 0, num_indices-1) ) {
      MESSAGE(cerr, "cset::get_first_oid(): invalid index");
      throw(boundaryException(0, num_indices-1, index));
   }

   if ( indices[index] == 0 ) {
      MESSAGE(cerr, "cset::get_first_oid(): invalid index");
      throw(stringException("NULL index ptr"));
   }

   return (*indices[index]) -> first_of_invlist(query);
}

c_index_handler* cset::get_index_ptr(int index)
{
   if ( !INRANGE(index, 0, num_indices-1) ) {
      MESSAGE(cerr, "cset::get_index_ptr(): invalid index");
      throw(boundaryException(0, num_indices-1, index));
   }

   return indices[index];
}

void cset::batch_index_begin() 
{
   for ( int i=0; i<num_indices; i++ ) {
      if ( indices[i] != 0 ) {
         (*indices[i]) -> batch_index_begin();
      }
   }
}

void cset::batch_index_end() 
{
   for ( int i=0; i<num_indices; i++ ) {
      if ( indices[i] != 0 ) {
         (*indices[i]) -> batch_index_end();
      }
   }
}

io_status cset::asciiIn(istream& in) 
{
   batch_index_begin();
   io_status ok = batch_asciiIn(in);
   batch_index_end();

   return ok;
}

io_status cset::batch_asciiIn(istream& in) 
{
   handler* root_hd_ptr = 0;
/*
#ifdef DEBUG
xtime tmr;
float f1;
long f2;
tmr.start();
#endif
*/
   char ccode_buf[LBUFSIZ];
   int c;
//fprintf(stderr, "batch_asciiIn() : start looping \n");
   while ( (c = in.get()) != EOF ) {
      in.putback(c);

      in.getline(ccode_buf, LBUFSIZ);
//fprintf(stderr, "ccode_buf=%s\n", ccode_buf);

      char* x = strrchr(ccode_buf, 'L');
      if ( x ) *x = 0;

      int ccode; 
      sscanf(ccode_buf, "%u", &ccode);
      //in >> ccode ;

//fprintf(stderr, "ccode=%d\n", ccode);

      root_hd_ptr = new handler(ccode, storage_ptr);


      if ( filter::assigned() == false )
         (*root_hd_ptr) -> asciiIn(in);
      else
         (*root_hd_ptr) -> asciiIn(filter::filter_func()(in));

      root_hd_ptr->commit();

      insert_object(*root_hd_ptr);


/*
#ifdef DEBUG
if ( v_sz % 1000 == 0 && v_sz != 0 ) {
tmr.stop(f1, f2);
debug(cerr, f1);
debug(cerr, f2);
debug(cerr, v_sz);
tmr.start();
}
#endif
*/

//(*root_hd_ptr)-> asciiOut(cerr);

/*
(root_hd_ptr -> its_oid()).asciiOut(cerr);
storage_ptr -> asciiOut(cerr);
MESSAGE(cerr, "\n========");
*/
      delete root_hd_ptr;

   }

   return done;
}

io_status cset::asciiOut(ostream&) 
{
   return done;
}

Boolean 
cset::insert_object(const handler& new_object)
{
/*
MESSAGE(cerr, "in insert_object()");
new_object.its_oid().asciiOut(cerr); cerr << "\n";
*/
   if ( indices[0] != 0 ) {

#ifdef A16_BROWSER 
      handler* zz = (handler*)&new_object;
      (*indices[0]) -> insert_key_loc(new_object, zz -> its_oid()); 
#else
      (*indices[0]) -> insert_key_loc(new_object, new_object.its_oid()); 
#endif
   }

   for ( int i = 1; i < num_indices; i++ ) {

      if ( indices[i] == 0 )
          continue;

      handler* y = (*(composite_handler*)&new_object) 
                    -> get_component(i);

      if ( y == 0 ) continue;

#ifdef A16_BROWSER
      handler* ww = (handler*)&new_object;
      (*indices[i]) -> insert_key_loc(*y, ww -> its_oid());
#else
      (*indices[i]) -> insert_key_loc(*y, new_object.its_oid());
#endif

      if ( !(y -> its_oid() == new_object.its_oid()) )
         delete y;
   }

   v_sz++;
   set_mode(UPDATE, true);

   return true;
}


Boolean cset::remove_component(const oid_t& x_oid)
{
   handler* x = new handler(x_oid, storage_ptr);

   for ( int i = 1; i < num_indices; i++ ) {

      if ( indices[i] == 0 )
         continue;

      handler* y = (*(composite_handler*)x) -> get_component(i);

      (*indices[i]) -> remove_loc(*y, x_oid);

      y -> destroy();

      delete y;

   }

   x -> destroy();

   delete x;

   v_sz--;
   set_mode(UPDATE, true);

   return true;
}

// update old component object 'old_comp_obj' to new 
// component object 'new_comp_obj'. 
// variable 'index' holds the index of the index object 
// effected. 'obj_pos' is the id of the
// main object, of which old_comp_obj is a part.

Boolean 
cset::update_index(handler* old_comp_obj, handler* new_comp_obj, 
                   int index,
                   oid_t& main_obj_oid)
{

   if ( !INRANGE(index, 1, num_indices-1) ) {
      throw(boundaryException(1, num_indices-1, index));
   }

   if ( indices[index] ) {


      return (*indices[index]) -> update_streampos
                          (
                             *old_comp_obj, 
                             *new_comp_obj, 
                             main_obj_oid.icode()
                          );

   } else
      return true;
}


int cset::cdr_sizeof()
{
   return composite::cdr_sizeof() + sizeof(num_indices);
}

io_status cset::cdrOut(buffer& buf)
{
   composite::cdrOut(buf);
   buf.put(num_indices);
   return done;
}

io_status cset::cdrIn(buffer& buf)
{
   composite::cdrIn(buf);
   buf.get(num_indices);
   return done;
}

void cset::commit() 
{
   for ( int i = 1; i < num_indices; i++ ) {

      if ( indices[i] )
         indices[i] -> commit();
   }
}


MMDB_BODIES(cset)
HANDLER_BODIES(cset)


