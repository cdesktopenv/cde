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
 * $XConsortium: mphf_index.cc /main/5 1996/07/18 14:38:37 drk $
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


#include "index/mphf_index.h"

#ifdef C_API
#define v_static_key (*v_static_key_ptr) 
#endif


mphf_index::mphf_index() : c_index(MPHF_INDEX_CODE), f_key_file_name(0)
{
//MESSAGE(cerr, " mphf_index::mphf_index():");
//debug(cerr, (void*)this);

   v_mphf = 0;
   v_inv_lists_hd = 0;
}

void mphf_index::init_data_member(fast_mphf_handler* mp, 
                                  inv_lists_handler* inv) 
{
/*
MESSAGE(cerr, " mphf_index::init_data_member():");
debug(cerr, (void*)v_mphf);
debug(cerr, (void*)(v_mphf->operator->()));
*/
   v_mphf = mp;
   v_inv_lists_hd = inv;
   set_mode(HEALTH, true);
}

   
mphf_index::~mphf_index()
{
   delete f_key_file_name;

////////////////////////////////////////////////////////
// Need not to delete index object handlers, as they 
// are handled by the object cache.  qfc 6-17-93
////////////////////////////////////////////////////////
}

Boolean 
mphf_index::insert_key_loc(const oid_t& id, const oid_t& loc) 
{
    get_key_string(id);

    *v_key_loc_pair_out << v_static_key << "\n";
    loc.asciiOut(*v_key_loc_pair_out);
    *v_key_loc_pair_out << "\n";

    return true;
}

Boolean 
mphf_index::insert_key_loc(const handler& hd, const oid_t& loc) 
{
    get_key_string(hd);

    *v_key_loc_pair_out << v_static_key << "\n";
    loc.asciiOut(*v_key_loc_pair_out);
    *v_key_loc_pair_out << "\n";

    return true;
}

Boolean mphf_index::remove_loc(const oid_t&, const oid_t&)
{
   throw(stringException("mphf_index::remove_index(): not applicable"));
   return false;
}

Boolean mphf_index::remove_loc(handler&, const oid_t&)
{
   throw(stringException("remove_index(): not applicable"));
   return false;
}

Boolean mphf_index::update_streampos(handler& ,
                                     handler& ,
                                     mmdb_pos_t
				    ) 
{
   throw(stringException("update_index(): not applicable"));
   return false;
}


Boolean mphf_index::load(const char* key_set)
{
/*
MESSAGE(cerr, " mphf_index::load():");
debug(cerr, (void*)this);
debug(cerr, (void*)(v_mphf->operator->()));
*/

   (*v_mphf) -> build(key_set);

   fstream in(key_set, ios::in);
   
   v_sz = (*v_mphf) -> keysetsize();
   (*v_inv_lists_hd) -> expand_space(v_sz);
   
   key_type key(LBUFSIZ);
   
#ifdef DEBUG
   int i = 0;
#endif

   while ( in >> key ) {

      oid_t key_id(c_code_t(0), 0);
   
      key_id.asciiIn(in);

#ifdef DEBUG
if ( i % 1000 == 0 && i != 0 ) {
cerr << i;
MESSAGE(cerr, " keys processed");
MESSAGE(cerr, "========");
}
i++;
#endif


      int hash = (*v_mphf) -> hashTo(key);
   
      (*v_inv_lists_hd) -> insert_to_list(hash+1, key_id);

   }
   
#ifndef MPHF_DEBUG
   del_file(key_set, 0);
#endif
   
   set_mode(HEALTH, true);
   set_mode(UPDATE, true);
   
   return true;
}

oid_list_handler* mphf_index::get_loc_list(const oid_t& ) 
{
   MESSAGE(cerr, "get_loc_list(): not implemented yet"); 
   return 0;
}

size_t mphf_index::handler_to_inv_idx(const handler& t)
{
   get_key_string(t);
//debug(cerr, v_static_key);
   return (*v_mphf) -> hashTo(v_static_key);
}

oid_list_handler* mphf_index::get_loc_list(const handler& t) 
{
   return (*this)(handler_to_inv_idx(t) + 1) ;
}

int mphf_index::invlist_length(handler& t) 
{
   oid_list_handler* x = get_loc_list(t);
   return (*x) -> count();
}

ostream& operator << (ostream&s, mphf_index& v)
{
   int ind = v.first();
   while (ind != 0) {
     v.next(ind);
   }
   return s;
}

Boolean mphf_index::batch_index_begin()
{
/*
MESSAGE(cerr, "batch_index_begin()");
debug(cerr, v_cmp_selector);
debug(cerr, count());
debug(cerr, int(this));
*/

   if ( count() > 0 ) {
      throw(stringException("batch_index_begin(): index exists"));
   } 

   f_key_file_name = 
     strdup(form("%s/keys.%d_%d", 
		get_store() -> my_path(), f_oid.ccode(), f_oid.icode()));

   v_key_loc_pair_out = new fstream(f_key_file_name, ios::out);

   return (( *v_key_loc_pair_out ) ?  true : false );
}

Boolean mphf_index::batch_index_end()
{
   v_key_loc_pair_out -> close();
   delete v_key_loc_pair_out;

   return load(f_key_file_name);
}

oid_t mphf_index::first_of_invlist(int ind)
{
   oid_t x((*v_inv_lists_hd) -> operator()(ind+1));

   if ( x.ccode() == OID_LIST_CODE ) {
      oid_list_handler* y = (*this)(ind);
      if ( (*y) -> count() > 0 ) {
         return (*y) -> operator()(1);
      } else {
         throw(stringException("first_of_invlist(): empty list"));
      }
   } else {
      return x;
   }
}

oid_t mphf_index::first_of_invlist(const handler& t)
{
   int hash = handler_to_inv_idx(t);
/*
debug(cerr, hash);
*/


   oid_t x((*v_inv_lists_hd) -> operator()(hash+1));

//debug(cerr, x);

   if ( x.ccode() == OID_LIST_CODE ) {
      oid_list_handler* y = (*this)(hash);
      if ( (*y) -> count() > 0 ) {
         return (*y) -> operator()(1);
      } else {
         throw(stringException("first_of_invlist(): empty list"));
      }
   } else {
      return x;
   }
}

MMDB_BODIES(mphf_index)
HANDLER_BODIES(mphf_index)
