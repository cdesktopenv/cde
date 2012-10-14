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
 * $XConsortium: inv_lists.cc /main/5 1996/07/18 14:37:46 drk $
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


#include "index/inv_lists.h"
#include "utility/funcs.h"


inv_lists::inv_lists(c_code_t c_cd) : oid_list(c_cd)
{
   //set_mode(HEALTH, true);
}

inv_lists::~inv_lists() 
{ 
}

Boolean inv_lists::insert_list_pinned(int index, oid_list& lst)
{
   if ( lst.count() == 0 ) {
      throw(boundaryException(0, lst.count(), index));
   } 

   oid_list_handler list_hd(lst.count(), storage_ptr);

   for ( int i=1; i<=lst.count(); i++ )
      list_hd -> update_component(i, lst(i)) ;

   oid_list::update_component(index, list_hd.its_oid()) ;

   set_mode(UPDATE, true);
   return true;
}

Boolean inv_lists::append_list(oid_list& lst)
{
/*
MESSAGE(cerr, "append a inv list");
lst.asciiOut(cerr); cerr << "\n";
*/

   handler hd(OID_LIST_CODE, storage_ptr);

   oid_list_handler& lst_handler = *(oid_list_handler*)&hd;


   lst_handler -> expand_space(lst.count());

   for ( int i=1; i<=lst.count(); i++ )
      lst_handler -> update_component(i, lst(i)) ;

   oid_list::insert_component(hd.its_oid()) ;

   set_mode(UPDATE, true);

   return true;
}

Boolean inv_lists::remove_list(int index)
{
   update_component(index, ground);
   return true;
}

oid_list_handler* inv_lists::get_list(int index) 
{
   oid_t x((*this)(index));

   if ( x.eq(ground) == true ) {
      return 0;
   } else {
      return new oid_list_handler(x, storage_ptr);
   }
}

/*
oid_list_handler* inv_lists::get_list(mmdb_pos_t) 
{
   throw( stringException("get_list(): not implemented yet"));
   return 0;
}
*/

ostream& operator <<(ostream&s, inv_lists& o) 
{
   for (int i=0; i < o.count(); i++) {
      oid_list_handler* x = o.get_list(i);
      (*x) -> asciiOut(s);
      delete x;
      s << "\n";
   }
   return s;
}

void inv_lists::insert_to_list(int index, oid_t& id)
{
   if ( !INRANGE(index, 1, v_sz) ) {
      throw(boundaryException(1, v_sz, index));
   }

   oid_t list_id = (*this)(index);

/*
MESSAGE(cerr, "inv::insert");
debug(cerr, index);
*/

   if ( INRANGE((short)list_id.ccode(), 0, 1) &&
        id.ccode() != OID_LIST_CODE 
   ) {

       update_component(index, id);

   } else {
       if ( list_id.ccode() != OID_LIST_CODE ) {
          oid_list_handler y(OID_LIST_CODE, storage_ptr);
          y -> insert_component(list_id);
          update_component(index, y.its_oid());
          y -> insert_component(id);
       } else {
          oid_list_handler y( list_id, storage_ptr);
          y -> insert_component(id);
       }
   }

}

MMDB_BODIES(inv_lists)
HANDLER_BODIES(inv_lists)
