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
 * $XConsortium: stylesheet_hd.cc /main/4 1996/06/11 17:30:47 cde-hal $
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


#include "oliasdb/stylesheet_hd.h"
#include "api/transaction.h"

MMDB_BODIES(stylesheet)

stylesheet_smart_ptr::stylesheet_smart_ptr(info_lib* lib_ptr,
                               const char* base_name, 
                               const char* locator) : 
smart_ptr(lib_ptr, base_name, STYLESHEET_SET_POS, 
                 managers::query_mgr ->  form_pstring_handler(locator),
                 BASE_COMPONENT_INDEX, smart_ptr::SET
                )
{
}

stylesheet_smart_ptr::stylesheet_smart_ptr(info_base* base_ptr, 
                               const char* locator) : 
smart_ptr(base_ptr, STYLESHEET_SET_POS, 
                 managers::query_mgr ->  form_pstring_handler(locator),
                 BASE_COMPONENT_INDEX, smart_ptr::SET
                )
{
}

stylesheet_smart_ptr::stylesheet_smart_ptr(info_lib* lib_ptr,
                               const char* base_name, const oid_t& x):
smart_ptr(
    lib_ptr -> get_info_base(base_name) -> get_set(STYLESHEET_SET_POS) -> its_store(),
    x
         )
{
}

stylesheet_smart_ptr::stylesheet_smart_ptr(info_base* base_ptr, 
                               const oid_t& x):
smart_ptr(
    base_ptr -> get_set(STYLESHEET_SET_POS) -> its_store(),
    x
         )
{
}

/////////////////////////////////////////
//
//
/////////////////////////////////////////
const char* stylesheet_smart_ptr::name()
{
  return get_string(BASE_COMPONENT_INDEX);
}

const char* stylesheet_smart_ptr::online_data()
{
  return get_string(BASE_COMPONENT_INDEX+1);
}

const char* stylesheet_smart_ptr::hardcopy_data()
{
  return get_string(BASE_COMPONENT_INDEX+2);
}

int stylesheet_smart_ptr::online_data_size()
{
   return get_string_size(BASE_COMPONENT_INDEX+1);
}

int stylesheet_smart_ptr::hardcopy_data_size()
{
   return get_string_size(BASE_COMPONENT_INDEX+2);
}

void stylesheet_smart_ptr::update_online_data(istream& in)
{
   update_data(in, BASE_COMPONENT_INDEX+1);
}

void stylesheet_smart_ptr::update_hardcopy_data(istream& in)
{
   update_data(in, BASE_COMPONENT_INDEX+2);
}

void stylesheet_smart_ptr::update_data(istream& in, int index)
{
   transaction trans ;
   mtry {
      trans.begin();

      update_string(index, in);

      trans.end();
   }

   mcatch (beginTransException&, e)
      {
         rethrow;
      }
   mcatch (commitTransException&, e)
      {
         rethrow;
      }
   mcatch (mmdbException&, e)
      {
         trans.rollback();
         rethrow;
      }
   end_try;
}

void 
stylesheet_smart_ptr::update_online_data(const char* buf, int size)
{
   update_data(buf, size, BASE_COMPONENT_INDEX+1);
}

void 
stylesheet_smart_ptr::update_hardcopy_data(const char* buf, int size)
{
   update_data(buf, size, BASE_COMPONENT_INDEX+2);
}

void 
stylesheet_smart_ptr::update_data(const char* buf, int size, int index)
{
   transaction trans ;
   mtry {
      trans.begin();

      update_string(index, buf, size);

      trans.end();
   }

   mcatch (beginTransException&, e)
      {
         rethrow;
      }
   mcatch (commitTransException&, e)
      {
         rethrow;
      }
   mcatch (mmdbException&, e)
      {
         trans.rollback();
         rethrow;
      }
   end_try;
}

