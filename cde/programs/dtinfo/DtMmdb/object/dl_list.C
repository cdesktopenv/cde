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
 * $XConsortium: dl_list.cc /main/5 1996/06/11 17:24:12 cde-hal $
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


#include "object/dl_list.h"

dl_list::dl_list(c_code_t c_cd) : composite(c_cd)
{
   v_num_indices = 0;
   v_indices = 0;

   set_mode(SWAP_ALLOWED, false);
}

dl_list::dl_list(const dl_list& x) : composite(x)
{
   throw(stringException("dl_list(const dl_list&) not done yet"));
}

dl_list::~dl_list()
{
/*
MESSAGE(cerr, "in ~dl_list()");
debug(cerr, v_num_indices);
*/

   delete v_indices;
}

void dl_list::init_data_member(c_index_handlerPtr* index_arrray,
                            int iptrs
                           )
{
   v_indices = index_arrray;
   v_num_indices = iptrs;

   set_mode(HEALTH, true);

#ifdef MEMORY_MAPPED
   set_mode(UPDATE, true);
#endif
}

//
//Boolean dl_list::value_LS(root& x, Boolean safe) const
//{
//   if ( safe == true &&
//        ( my_oid().ccode() != DL_LIST_CODE ||
//          x.my_oid().ccode() != DL_LIST_CODE  
//        ) 
//      )
//      return false;
//
//   dl_list &y = *(dl_list*)&x;
//
//   if ( sz != y.sz )
//      return false;
//
///************************************************/
//// Note: the components are indexed from 1 to sz
///************************************************/
//
//   for ( int i=1; i<=sz; i++ ) {
//
//      handler* rootPtr1 = get_component(i);
//      handler* rootPtr2 = y.get_component(i);
//
//      if ( (*rootPtr1) -> value_LS( *rootPtr2 ) == true ) {
//         return true;
//      }
//
//      if ( (*rootPtr1) -> value_EQ( *rootPtr2 ) == false ) {
//         return false;
//      }
//   }
//
//   return false;
//}
//
//Boolean dl_list::value_EQ(root& x, Boolean safe) const
//{
//   if ( safe == true &&
//        ( my_oid().ccode() != DL_LIST_CODE ||
//          x.my_oid().ccode() != DL_LIST_CODE  
//        ) 
//      )
//      return false;
//
//   dl_list &y = *(dl_list*)&x;
//
//   if ( sz != y.sz )
//      return false;
//
///************************************************/
//// Note: the components are index from 1 to sz
///************************************************/
//
//   for ( int i=1; i<=sz; i++ ) {
//
//      handler* rootPtr1 = get_component(i);
//      handler* rootPtr2 = y.get_component(i);
//
//      if ( (*rootPtr1) -> value_EQ( *rootPtr2 ) == false ) {
//         return false;
//      }
//   }
//
//   return true;
//}


handler* dl_list::get_component(int index) 
{
   if ( v_sz == 0 ) 
      return 0;

   const oid_t* cell_ptr = &v_dl_list_head;

   for ( int i=1; i<index-1; i++ ) {

       if ( cell_ptr == 0 || cell_ptr -> icode() == 0 ) 
          throw(stringException("broken chain"));

       dl_list_cell_handler cell(*cell_ptr, storage_ptr);


       cell_ptr = &cell -> get_forward_ptr();

   }

   return new handler (*cell_ptr, storage_ptr);
}

oid_list_handler* dl_list::get_locs(handler& query, int index)
{
   if ( !INRANGE(index, 0, (int) v_num_indices-1) )
      throw(boundaryException(0, v_num_indices-1, index));

   if ( v_indices[index] == 0 ) 
      throw(stringException("NULL index handler ptr"));

   return (*v_indices[index]) -> get_loc_list(query);
}

oid_t dl_list::first_cell_oid() const 
{
   return v_dl_list_head;
}

oid_t dl_list::last_cell_oid() const 
{
   return v_dl_list_tail;
}


Boolean 
dl_list::insert_before(dl_list_cell_handler& new_cell,
                       dl_list_cell_handler& old_cell   
                      ) 
{
   oid_t prev_oid(old_cell -> get_backward_ptr());

   new_cell -> set_forward_ptr(old_cell.its_oid());
   old_cell -> set_backward_ptr(new_cell.its_oid());

   if ( prev_oid.icode() != 0 ) {

// NOTE: non-new-list-head case
      dl_list_cell_handler prev_cell_hd(prev_oid, storage_ptr);

      new_cell -> set_backward_ptr(prev_cell_hd.its_oid());
      prev_cell_hd -> set_forward_ptr(new_cell.its_oid());

   } else {

// NOTE: new-list-head case
      v_dl_list_head.become(new_cell.its_oid());

   }

   v_sz++;
   set_mode(HEALTH, true);

   insert_to_indices(new_cell);

   return true;
}

Boolean 
dl_list::insert_after(dl_list_cell_handler& new_cell,
                      dl_list_cell_handler& old_cell   
                      ) 
{
   oid_t next_oid(old_cell -> get_forward_ptr());

   new_cell -> set_backward_ptr(old_cell.its_oid());
   old_cell -> set_forward_ptr(new_cell.its_oid());

   if ( next_oid.icode() != 0 ) {

// NOTE: non-new-list-tail case
      dl_list_cell_handler next_cell_hd(next_oid, storage_ptr);

      new_cell -> set_forward_ptr(next_cell_hd.its_oid());
      next_cell_hd -> set_backward_ptr(new_cell.its_oid());

   } else {

// NOTE: new-list-tail case
      new_cell -> get_forward_ptr().become(ground);
      v_dl_list_tail.become(new_cell.its_oid());

   }

   v_sz++;
   set_mode(HEALTH, true);

   insert_to_indices(new_cell);

   return true;
}

Boolean 
dl_list::insert_as_head(const dl_list_cell_handler& new_cell)
{
#ifdef A16_BROWSER
   dl_list_cell_handler* z = 0;
#endif

   if ( v_dl_list_head.icode() != 0 ) {

      dl_list_cell_handler first_cell(v_dl_list_head, 
                                      storage_ptr
                                     );

#ifdef A16_BROWSER
      z = (dl_list_cell_handler*)&new_cell;
      first_cell -> set_backward_ptr(z -> its_oid());
#else
      first_cell -> set_backward_ptr(new_cell.its_oid());
#endif

   } 

   ((dl_list_cell_handler&)new_cell) -> set_forward_ptr(v_dl_list_head);

#ifdef A16_BROWSER
   v_dl_list_head.become(z -> its_oid());
#else
   v_dl_list_head.become(new_cell.its_oid());
#endif

   if ( v_dl_list_tail.icode() == 0 )
      v_dl_list_tail.become(v_dl_list_head);

   v_sz++;
   set_mode(HEALTH, true);

   insert_to_indices(new_cell);

   return true;
}

Boolean 
dl_list::insert_as_tail(dl_list_cell_handler& new_cell)
{
   if ( v_dl_list_tail.icode() != 0 ) {

      dl_list_cell_handler last_cell(v_dl_list_tail, 
                                     storage_ptr
                                     );

      last_cell -> set_forward_ptr(new_cell.its_oid());
   } 

   new_cell -> set_backward_ptr(v_dl_list_tail);
   v_dl_list_tail.become(new_cell.its_oid());

   if ( v_dl_list_head.icode() == 0 )
      v_dl_list_head.become(v_dl_list_tail);

   v_sz++;
   set_mode(HEALTH, true);

   new_cell -> set_mode(UPDATE, true);

   insert_to_indices(new_cell);

   return true;
}

io_status dl_list::asciiOut(ostream& out) 
{
/*   
MESSAGE(cerr, "HEAD");
v_dl_list_head.asciiOut(out); cerr << "\n";

MESSAGE(cerr, "TAIL");
v_dl_list_tail.asciiOut(out); cerr << "\n";
*/
   out << "OID_T:\n";
   my_oid().asciiOut(out); 
   out << "\n";

   if ( v_sz == 0 ) return done;

   oid_t* cell_ptr = &v_dl_list_head;

   for ( unsigned int i=1; i<=v_sz; i++ ) {

       if ( cell_ptr == 0 ) {
          throw(stringException("broken chain"));
       }

       if ( cell_ptr -> icode() == 0 ) {
          debug(cerr, i);
          debug(cerr, v_sz);
          throw(stringException("dl_list::get_component(): broken chain"));
       }

       dl_list_cell_handler cell(*cell_ptr, storage_ptr);


       cell -> asciiOut(cerr); cerr << "\n";
       cell_ptr = &cell -> get_forward_ptr();

   }

   return done;
}

void dl_list::batch_index_begin()
{
   for ( unsigned int i=0; i<v_num_indices; i++ ) {
      if ( v_indices[i] != 0 ) {
        (*v_indices[i]) -> batch_index_begin();
      }
   }
}

void dl_list::batch_index_end()
{
   for ( unsigned int i=0; i<v_num_indices; i++ ) {
      if ( v_indices[i] != 0 ) {
         (*v_indices[i]) -> batch_index_end();
      }
   }
}

io_status dl_list::asciiIn(istream& in)
{
   batch_index_begin();
   io_status ok = batch_asciiIn(in);
   batch_index_end();
   return ok;
}

io_status dl_list::batch_asciiIn(istream& in)
{
   char ccode_buf[LBUFSIZ];
   int c;
   handler* hd_ptr = 0;

   while ( (c = in.get()) != EOF ) {

      in.putback(c);

      in.getline(ccode_buf, LBUFSIZ);
      char* x = strrchr(ccode_buf, 'L');
      if ( x ) *x = 0;

      int ccode; 
      sscanf(ccode_buf, "%u", &ccode);

      hd_ptr = new handler(ccode, storage_ptr);


      if ( filter::assigned() == false )
         (*hd_ptr) -> asciiIn(in);
      else
         (*hd_ptr) -> asciiIn(filter::filter_func()(in));

/*
MESSAGE(cerr, "dllist: ID read in");
hd_ptr -> its_oid().asciiOut(cerr); cerr << "\n";
*/


      insert_as_tail(*(dl_list_cell_handler*)hd_ptr) ;
 
      delete hd_ptr;
   }
   return done;
}

Boolean 
dl_list::insert_to_indices(const dl_list_cell_handler& new_object)
{
#ifdef A16_BROWSER
   dl_list_cell_handler* z = (dl_list_cell_handler*)&new_object;
#endif

   if ( v_indices[0] != 0 ) {

#ifdef A16_BROWSER
      (*v_indices[0]) -> insert_key_loc(new_object, z -> its_oid());
#else
      (*v_indices[0]) -> insert_key_loc(new_object, new_object.its_oid());
#endif

   }

   for ( unsigned int i = 1; i < v_num_indices; i++ ) {

      if ( v_indices[i] == 0 ) continue;

      handler* y = (*(composite_handler*)&new_object) 
                    -> get_component(i);

/*
MESSAGE(cerr, "dlist: comp id:");
y -> its_oid().asciiOut(cerr); cerr << "\n";
MESSAGE(cerr, "obj id: (its_oid and my_oid)");
new_object.its_oid().asciiOut(cerr); cerr << "\n";
(*(dl_list_cell_handler*)&new_object) -> my_oid().asciiOut(cerr); cerr << "\n";
*/


      if ( y == 0 ) continue;

// A16 backward compatible
#ifdef A16_BROWSER
      (*v_indices[i]) -> insert_key_loc(*y, z -> its_oid());
#else
      (*v_indices[i]) -> insert_key_loc(*y, new_object.its_oid());
#endif

//debug(cerr, y -> its_oid());
//debug(cerr, new_object.its_oid());
    
       if ( !(y -> its_oid() == new_object.its_oid()) )
          delete y;
   }
   set_mode(HEALTH, true);

   return true;
}


int dl_list::cdr_sizeof()
{
   return composite::cdr_sizeof() + 
          v_dl_list_head.cdr_sizeof() +
          v_dl_list_tail.cdr_sizeof() +
          sizeof(v_num_indices);
}

io_status dl_list::cdrOut(buffer& buf)
{
   composite::cdrOut(buf);
   v_dl_list_head.cdrOut(buf);
   v_dl_list_tail.cdrOut(buf);
   buf.put(v_num_indices);
   return done;
}

io_status dl_list::cdrIn(buffer& buf)
{
   composite::cdrIn(buf);
   v_dl_list_head.cdrIn(buf);
   v_dl_list_tail.cdrIn(buf);
   buf.get(v_num_indices);
   return done;
}

oid_t dl_list::get_first_oid(const handler& query, int index)
{
   if ( !INRANGE(index, 0, (int) v_num_indices-1) ) {
       MESSAGE(cerr, "cset::get_first_oid(): invalid index");
       throw(boundaryException(0, v_num_indices-1, index));
   }

   if ( v_indices[index] == 0 ) {
       throw(stringException("cset::get_first_oid(): NULL index ptr"));
   }

   return (*v_indices[index]) -> first_of_invlist(query);
}

void dl_list::commit()
{
   for ( unsigned int i = 1; i < v_num_indices; i++ )
   {
      if ( v_indices[i] ) 
        v_indices[i] -> commit() ;
   }
}


MMDB_BODIES(dl_list)
HANDLER_BODIES(dl_list)
