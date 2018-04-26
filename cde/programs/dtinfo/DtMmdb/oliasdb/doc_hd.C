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
 * $XConsortium: doc_hd.cc /main/4 1996/06/11 17:28:47 cde-hal $
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


#include "oliasdb/doc_hd.h"


MMDB_BODIES(doc)

doc_smart_ptr::
doc_smart_ptr(info_lib* lib_ptr, const char* info_base_name, const int seq_num) : 
smart_ptr(lib_ptr, info_base_name, 
                  DOC_SET_POS, 
                  managers::query_mgr -> form_integer_handler(seq_num),
                  BASE_COMPONENT_INDEX +3,
                  smart_ptr::SET 
                 )
{
   if ( seq_num < 1 ) 
      throw(boundaryException(1, MAXINT, seq_num));
}

doc_smart_ptr::
doc_smart_ptr(info_base* base_ptr, const int seq_num) : 
smart_ptr(base_ptr, DOC_SET_POS,    
                  managers::query_mgr -> form_integer_handler(seq_num),
                  BASE_COMPONENT_INDEX + 3, smart_ptr::SET)
{
   if ( seq_num < 1 ) 
      throw(boundaryException(1, MAXINT, seq_num));
}

doc_smart_ptr::
doc_smart_ptr(info_lib* lib_ptr, const char* base_name, const oid_t& x_id) : 
smart_ptr(lib_ptr, base_name, DOC_SET_POS, 
          managers::query_mgr -> form_oid_handler(x_id),
          BASE_COMPONENT_INDEX,
          smart_ptr::SET
         )
{
}

doc_smart_ptr::
doc_smart_ptr(info_base* base_ptr, const oid_t& x_id) : 
smart_ptr(base_ptr, DOC_SET_POS, 
          managers::query_mgr -> form_oid_handler(x_id),
          BASE_COMPONENT_INDEX,
          smart_ptr::SET
         )
{
}

doc_smart_ptr::
doc_smart_ptr(const oid_t& doc_id, 
	      info_lib* lib_ptr, const char* base_name
	     ) :
smart_ptr(
    lib_ptr->get_info_base(base_name)->get_set(DOC_SET_POS) -> 
	its_store(),
    doc_id
         )
{
}

doc_smart_ptr::
doc_smart_ptr(const oid_t& doc_id, info_base* base_ptr) :
  smart_ptr(
      base_ptr -> get_set(DOC_SET_POS) -> its_store(),
      doc_id
           )
{
}

oid_t doc_smart_ptr::locator_id()
{
   return get_oid(BASE_COMPONENT_INDEX);
}

const char* doc_smart_ptr::short_title()
{
   return get_string(BASE_COMPONENT_INDEX+1);
}

const char* doc_smart_ptr::long_title()
{
   return get_string(BASE_COMPONENT_INDEX+2);
}

int doc_smart_ptr::seq_num()
{
   return get_int(BASE_COMPONENT_INDEX+3);
}

short_list_handler* doc_smart_ptr::tab_list()
{
   short_list_handler* y = (short_list_handler*)
          get_handler( BASE_COMPONENT_INDEX + 4, 
                          SHORT_LIST_CODE 
                        ); 

   return y;
}

const char* doc_smart_ptr::license_terms()
{
  return get_string(BASE_COMPONENT_INDEX+5);
}

unsigned int doc_smart_ptr::license_terms_size()
{
  return get_string_size(BASE_COMPONENT_INDEX+5);
}

