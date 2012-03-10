/*
 * $XConsortium: node_hd.C /main/5 1996/08/21 15:53:33 drk $
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


#include "oliasdb/node_hd.h"

#ifdef C_API
#include "utility/c_strstream.h"
#else
#include <strstream.h>
#endif

MMDB_BODIES(olias_node)

node_smart_ptr::node_smart_ptr(info_lib* lib_ptr,
                               const char* base_name, 
                               const char* locator) : 
smart_ptr(lib_ptr, base_name, NODE_SET_POS, 
                 managers::query_mgr ->  form_pstring_handler(locator),
                 BASE_COMPONENT_INDEX, smart_ptr::SET
                )
{
}

node_smart_ptr::node_smart_ptr(info_base* base_ptr, 
                               const char* locator) : 
smart_ptr(base_ptr, NODE_SET_POS, 
                 managers::query_mgr ->  form_pstring_handler(locator),
                 BASE_COMPONENT_INDEX, smart_ptr::SET
                )
{
}

node_smart_ptr::node_smart_ptr(info_lib* lib_ptr,
                               const char* base_name, const oid_t& x):
smart_ptr(
    lib_ptr -> get_info_base(base_name) -> get_set(NODE_SET_POS) -> its_store(),
    x
         )
{
}

node_smart_ptr::node_smart_ptr(info_base* base_ptr, 
                               const oid_t& x):
smart_ptr(
    base_ptr -> get_set(NODE_SET_POS) -> its_store(),
    x
         )
{
}

/////////////////////////////////////////
//
//
/////////////////////////////////////////
const char* node_smart_ptr::locator()
{
  return get_string(BASE_COMPONENT_INDEX);
}

const char* node_smart_ptr::long_title()
{
  return get_string(BASE_COMPONENT_INDEX+1);
}

const char* node_smart_ptr::short_title()
{
  return get_string(BASE_COMPONENT_INDEX+2);
}

const char* node_smart_ptr::data()
{
  return get_string(BASE_COMPONENT_INDEX+3);
}

int node_smart_ptr::data_size()
{
  return get_string_size(BASE_COMPONENT_INDEX+3);
}

const char* node_smart_ptr::toc_node_loc()
{
  return get_string(BASE_COMPONENT_INDEX+4);
}

oid_t node_smart_ptr::doc_id()
{
  return get_oid(BASE_COMPONENT_INDEX+5);
}

oid_t node_smart_ptr::stylesheet_id()
{
  return get_oid(BASE_COMPONENT_INDEX+6);
}


void node_smart_ptr::update_doc_id(const oid_t& x)
{
   update_oid(BASE_COMPONENT_INDEX+5, x);
}

Boolean node_smart_ptr::update_data(istream& in)
{
  update_string(BASE_COMPONENT_INDEX+3, in);
  return true;
}
