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
 * $XConsortium: mark_base.cc /main/7 1996/07/18 14:47:29 drk $
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



#include "oliasdb/mark_base.h"
#include "oliasdb/mark.h"
#include "utility/debug.h"
#include "misc/unique_id.h"

mark_base::mark_base(user_base::rw_flag_t rw) : 
	user_base(MARK_SPEC, rw)
{
   mark_set_hd = NULL;
}


mark_base::mark_base( const char* base_dir, 
                      const char* base_nm,
                      const char* base_ds,
		      user_base::rw_flag_t rw 
                    ) : 
	user_base(base_dir, base_nm, base_ds, MARK_SPEC, rw) 
{
   if ( checking_status != SUCC ) {
      mark_set_hd = NULL;
      return;
   }

   desc* ptr = first_desc_ptr;

   if ( ptr == 0 ) 
      throw(stringException("empty mark specification"));
     
   while ( ptr ) {
     if ( strcmp( ptr -> get_type(), "container	set") == 0 ) {
        mark_set_hd = (cset_handler*)
            (f_obj_dict -> get_handler(ptr -> get_nm()));
        return;
     }
     ptr = ptr -> get_next_desc();
   }

   throw(stringException("can't find mark set description record"));
}

mark_base::~mark_base()
{
}

oid_list_handler* mark_base::get_mark_list(const char* node_locator)
{
#ifdef INTERCEPT_MARK_BASE_CALLS
MESSAGE(cerr, "mark_base::get_mark_list()");
debug(cerr, node_locator);
#endif

   pstring* x = new pstring(node_locator, strlen(node_locator));
   handler y(x, 0);

   oid_list_handler*z = (*mark_set_hd) -> get_locs(y, 1);

#ifdef INTERCEPT_MARK_BASE_CALLS
if (z) 
  debug(cerr, (*z) -> asciiOut(cerr));
#endif

   return z;
}
    
mmdb_pos_t mark_base::first()
{
   page_storage *s = (page_storage*)((*mark_set_hd) -> get_store());

   mmdb_pos_t ind = s -> first_loc();

   if ( managers::template_mgr -> peek_slot(s, ind) != USER_MARK_CODE ) {
      this -> next(ind);
   }

   return ind;
}
   
oid_t mark_base::get_mark_oid(mmdb_pos_t& ind)
{
   page_storage *s = (page_storage*)((*mark_set_hd) -> get_store());

   root *r = 0;
   managers::template_mgr -> init_obj(s, ind, r);

   if (r==0)
      throw(stringException("null root object pointer"));

   oid_t x = r -> my_oid();
   delete r;

   return x;
}

void mark_base::next(mmdb_pos_t& ind)
{
   page_storage *s = (page_storage*)((*mark_set_hd) -> get_store());

   while ( s -> seek_loc( ind, positive, spointer_t::IS_OBJECT ) == true ) {
     if ( managers::template_mgr -> peek_slot(s, ind) == USER_MARK_CODE ) {
        return;
     }
   }

   ind = 0;
}

