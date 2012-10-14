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
 * $XConsortium: locator_hd.C /main/5 1996/10/26 18:18:44 cde-hal $
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


#include "oliasdb/locator_hd.h"

MMDB_BODIES(olias_locator)

olias_locator::olias_locator() : tuple(2, LOCATOR_CODE) 
{
}


// components: 
//  this_oid (ostring) parent (ostring), prev (ostring), 
//  next (ostring), children (list)

locator_smart_ptr::
locator_smart_ptr(info_lib* lib_ptr, 
                  const char* info_base_name, const char* locator) : 
smart_ptr(lib_ptr, info_base_name, LOCATOR_SET_POS, 
                 managers::query_mgr ->  form_pstring_handler(locator),
                 BASE_COMPONENT_INDEX, smart_ptr::SET
                )
{
}

locator_smart_ptr::
locator_smart_ptr(info_base* base_ptr, const char* locator) : 
smart_ptr(base_ptr, LOCATOR_SET_POS, 
                 managers::query_mgr ->  form_pstring_handler(locator),
                 BASE_COMPONENT_INDEX, smart_ptr::SET
                )
{
}

locator_smart_ptr::
locator_smart_ptr(info_base* base_ptr, const oid_t& id) : 
smart_ptr(base_ptr -> get_set(LOCATOR_SET_POS) -> its_store(), id)
{
}

const char* locator_smart_ptr::inside_node_locator_str()
{
  return get_string(BASE_COMPONENT_INDEX);
}

oid_t locator_smart_ptr::node_id()
{
  return get_oid(BASE_COMPONENT_INDEX+1);
}

const char* locator_smart_ptr::label()
{
  const char* rval;

  mtry
    {
      rval = get_string(BASE_COMPONENT_INDEX+2);
    }
  mcatch_any()
    {
      // catch exceptions to ensure backwards compatibility
      rval = 0;
    }
  end_try;

  return rval;
}

