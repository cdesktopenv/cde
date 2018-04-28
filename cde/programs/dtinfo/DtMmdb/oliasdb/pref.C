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
 * $XConsortium: pref.cc /main/4 1996/06/11 17:30:27 cde-hal $
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




#include "oliasdb/pref.h"

MMDB_BODIES(pref)

pref_smart_ptr::pref_smart_ptr(pref_base* uptr,
                               const char* name) :  
	pbase(uptr), pref_set_hd_ptr(uptr -> pref_set_hd),
        v_name_hd(0), v_cnfg_hd(0) 
{

   abs_storage *pref_store = pref_set_hd_ptr -> its_store();

   oid_list_handler* pref_list = (*pref_set_hd_ptr) -> 
         get_locs(managers::query_mgr -> form_pstring_handler(name), 
                  BASE_COMPONENT_INDEX
                 );


   if ( pref_list == 0 ) {

MESSAGE(cerr, "NEW RECORD");
      try {
         pbase -> trans().begin();
   
         tuple_handler* w =
             new tuple_handler(oid_t(USER_CONFIG_CODE, 0), pref_store);
   
         v_name_hd = new pstring_handler(name, strlen(name), pref_store);
         v_cnfg_hd = new pstring_handler("", 0, pref_store);
   
         (*w) -> pinned_insert(BASE_COMPONENT_INDEX, v_name_hd -> its_oid());
         (*w) -> pinned_insert(BASE_COMPONENT_INDEX+1, v_cnfg_hd -> its_oid());
   
   /////////////////////
   // init smart_ptr
   /////////////////////
         smart_ptr::_init(w->its_oid(), pref_store);
   
         (*pref_set_hd_ptr) -> insert_object( *w );
   
         pbase -> trans().end();
      }

      catch (mmdbException&, e)
      {
         smart_ptr::_init(ground, 0); // mark the pref obsolete
         pbase -> trans().rollback();
         rethrow;
      }
      end_try;


   } else {
MESSAGE(cerr, "OLD RECORD");

      if ( (*pref_list) -> count() == 0 ) {
         throw(stringException("zero prefs in the list"));
      }

      smart_ptr::_init((*pref_list) -> operator()(1), pref_store);

      v_name_hd = (pstring_handler*)
                    get_handler(BASE_COMPONENT_INDEX, STRING_CODE);
      v_cnfg_hd = (pstring_handler*)
                    get_handler(BASE_COMPONENT_INDEX+1, STRING_CODE);;
   }

}

pref_smart_ptr::~pref_smart_ptr()
{
   delete v_name_hd;
   delete v_cnfg_hd;
}

Boolean pref_smart_ptr::update_value(const pstring& new_uc)
{
   return update_value(((pstring&)new_uc).get(), ((pstring&)new_uc).size());
}

Boolean pref_smart_ptr::update_value(const char* new_pref, 
                                     int new_pref_sz)
{
   try {
      pbase -> trans().begin();

      (*v_cnfg_hd) -> update(new_pref, new_pref_sz);

      pbase -> trans().end();
   }

   catch (mmdbException&, e)
      {
         pbase -> trans().rollback();
         return false;
      }
   end_try;


   return true;
}

Boolean pref_smart_ptr::remove_from_db()
{
   try {
      pbase -> trans().begin();

      (*pref_set_hd_ptr) -> remove_component(its_oid());

      pbase -> trans().end();
   }

   catch (mmdbException&, e)
      {
         pbase -> trans().rollback();
         return false;
      }
   end_try;


   return true;
}

char* pref_smart_ptr::name()
{
   return (*v_name_hd) -> get();
}

pstring* pref_smart_ptr::pref_value()
{
   return v_cnfg_hd -> operator->();
}

