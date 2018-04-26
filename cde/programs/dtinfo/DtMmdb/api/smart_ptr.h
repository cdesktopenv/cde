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
 * $XConsortium: smart_ptr.h /main/5 1996/06/11 17:11:38 cde-hal $
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



#ifndef _smart_ptr_h
#define _smart_ptr_h 1

#include "object/cset.h"
#include "object/dl_list.h"
#include "object/integer.h"
#include "api/info_lib.h"
#include "mgrs/query_mgr.h"

class smart_ptr : public composite_handler 
{

public:

   enum composite_tag { SET, LIST };

   smart_ptr(info_lib* lib_ptr,
             const char* infobase_name, int composite_position,
             const handler& query_hd, int index_selector, composite_tag);

   smart_ptr(info_base* base_ptr, int composite_position,
             const handler& query_hd, int index_selector, composite_tag);

   smart_ptr(abs_storage* st, const oid_t& x) 
     /*: composite_handler(c)*/
   {
      _init(*(oid_t*)&x, st);
   };

   smart_ptr() {};
   virtual ~smart_ptr() {};

   int get_int(int component_index);
   const char* get_string(int component_index);
   const char* get_string(int component_index, buffer&);
   int get_string_size(int component_index);
   oid_t get_oid(int component_index);

   void update_oid(int component_index, const oid_t& x);
   void update_string(int component_index, istream& in);
   void update_string(int component_index, const char* buf, int size);


protected:
   void _init(const oid_t& id, const abs_storage*);

   handler* get_handler(int component_index, c_code_t code);

};

typedef smart_ptr* smart_ptrPtr;

#endif
