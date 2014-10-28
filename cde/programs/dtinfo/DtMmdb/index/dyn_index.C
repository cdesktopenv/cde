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
 * $XConsortium: dyn_index.cc /main/5 1996/07/18 14:34:38 drk $
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



#include "index/dyn_index.h"

#ifdef C_API
#define v_static_key (*v_static_key_ptr)
#endif


dyn_index::dyn_index(c_code_t x) : c_index(x)
{
   v_inv_lists_hd = 0;
   v_idx_agent_ptr = 0;
}


dyn_index::~dyn_index()
{
////////////////////////////////////////////////////////
// Need not to delete index object handlers, as they 
// are handled by the object cache.  qfc 6-17-93
////////////////////////////////////////////////////////
}
 
data_t* dyn_index::hd_to_key(const handler& t)
{
   get_key_string(t);
/*
MESSAGE(cerr, "dyn_index::hd_to_key():");
if ( v_static_key.get() )
   debug(cerr, v_static_key.get());
MESSAGE(cerr, "************");
*/

   return new data_t(v_static_key.get(), v_static_key.size());
}

Boolean 
dyn_index::insert_key_loc(const handler& t, const oid_t& id) 
{

//MESSAGE(cerr, "dyn_index::insert_key_loc()");
//id.asciiOut(cerr); cerr << "\n";


   data_t* intKey = hd_to_key(t);

   _insert_loc( *intKey, id );

   delete intKey;
   return true;
}
   
Boolean 
dyn_index::insert_key_loc(const oid_t& t, const oid_t& id) 
{
    data_t intKey(int(t.icode()), voidPtr(-1));
    return _insert_loc( intKey, id );
}

Boolean 
dyn_index::_insert_loc(data_t& intKey, const oid_t& id) 
{
/*
MESSAGE(cerr, "in dyn_index insert()");
debug(cerr, intKey);
debug(cerr, id);
debug(cerr, p);
*/

    if ( v_idx_agent_ptr -> member( intKey ) == true ) {

       int hash = int(long((intKey.dt)));

//MESSAGE(cerr, "update inv list");
//debug(cerr, hash);

/**************************/
// just update the inv list
/**************************/
       oid_list_handler* list_hd = 
            (*v_inv_lists_hd) -> get_list(hash);

       if ( list_hd == 0 ) 
          throw(stringException("NULL oidlist ptr"));

/*
MESSAGE(cerr, "insert to list before");
(*list_hd) -> asciiOut(cerr); cerr << "\n";
*/

       (*list_hd) -> insert_component(id);
       //list_hd -> commit();

/*
MESSAGE(cerr, "insert to list after");
(*list_hd) -> asciiOut(cerr); cerr << "\n";
*/
      
       delete list_hd;


    } else {
//MESSAGE(cerr, "create inv list");

/**************************/
// create a new inv list
/**************************/
       oid_list list;
       list.insert_component(id);

       (*v_inv_lists_hd) -> append_list(list);

       intKey.dt = (voidPtr)(size_t)(*v_inv_lists_hd) -> count();
//debug(cerr, int(intKey.dt));

       v_idx_agent_ptr -> insert( intKey );

    }
/*
MESSAGE(cerr, "dyn_index::_insert_loc(): ");
v_idx_agent_ptr -> asciiOut(cerr);
MESSAGE(cerr, "=========================");
*/

    return true;
}
   
Boolean 
dyn_index::remove_loc(handler& t, const oid_t& id)
{
   data_t* intKey = hd_to_key(t);

   if ( intKey == 0 ) 
      return false;
   else {
      Boolean ok = _remove_loc(*intKey, id);
      delete intKey;
      return ok;
   }
}

Boolean 
dyn_index::remove_loc(const oid_t& t, const oid_t& id) 
{
   data_t intKey(int(t.icode()), voidPtr(-1));
   return _remove_loc(intKey, id);
}

Boolean dyn_index::_remove_loc(data_t& intKey, const oid_t& id) 
{
//MESSAGE(cerr, "_remove_loc()");

   Boolean ok = v_idx_agent_ptr -> member( intKey );

   if ( ok == false ) return true; // no key in the hash table.

//MESSAGE(cerr, "in hash table");
   int hash = int(long(intKey.dt));

   oid_list_handler *list_hd = 
        (*v_inv_lists_hd) -> get_list(hash);

   if ( list_hd == 0 )
       throw(stringException("NULL oid list ptr"));

//MESSAGE(cerr, "remove_component():");
   (*list_hd) -> remove_component(id);

   //list_hd -> commit();

//MESSAGE(cerr, "remove_component() done");

   delete list_hd;
    
//MESSAGE(cerr, "_remove_loc() done");
   return true;
}

Boolean dyn_index::update_streampos(handler& /* old_obj */,     
                                    handler& /* new_obj */,
                                    mmdb_pos_t /* pos */
                                   ) 
{
   return false;
}

oid_list_handler* dyn_index::get_loc_list(const oid_t& id)
{
   data_t intKey((int)id.icode());

   if ( v_idx_agent_ptr -> member( intKey ) == true ) {

      return  (*v_inv_lists_hd) -> get_list(int(long(intKey.dt))); 

   } else {

      return 0;

   }
}

oid_list_handler* dyn_index::get_loc_list(const handler& t)
{
//MESSAGE(cerr, "dyn_index::get_loc_list()");
//debug(cerr, (*v_inv_lists_hd) -> get_store() -> my_path());
//debug(cerr, (*v_inv_lists_hd) -> get_store() -> my_name());

   if ( OK() == false ) {
      MESSAGE(cerr, 
              "dyn_index::get_loc_list(): bad index object status");
      return 0;
   }

   data_t* intKey = 0;

   if ( ( intKey = hd_to_key(t)) != 0 &&
        v_idx_agent_ptr -> member( *intKey ) == true 
      ) {
//MESSAGE(cerr, "in hash table");

      int p = int(long(intKey -> dt));
//debug(cerr, p);
      delete intKey;
//MESSAGE(cerr, "dyn_index::get_loc_list() done");

      return  (*v_inv_lists_hd) -> get_list(p);

   } else {
//MESSAGE(cerr, "not in hash table");
      delete intKey;
      return 0;
   }
}

int dyn_index::invlist_length(handler& t) 
{
   oid_list_handler* locs = get_loc_list(t);

   if ( locs == 0 )
      throw(stringException("NULL oid list ptr"));

   return (*locs) -> count();
}

io_status dyn_index::asciiOut(ostream& )
{
   return done;
}

