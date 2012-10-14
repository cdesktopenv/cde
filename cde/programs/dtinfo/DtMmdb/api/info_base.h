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
 * $XConsortium: info_base.h /main/6 1996/09/04 01:37:17 cde-hal $
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


#ifndef _info_base_h
#define _info_base_h 1

#include "utility/macro.h"
#include "storage/version.h"
#include "api/base.h"
#include "mgrs/managers.h"
#include "schema/object_dict.h"
#include "object/cset.h"
#include "object/dl_list.h"

class Iterator
{
public:
   Iterator( handler*, c_code_t );
   ~Iterator();

   operator void*();

protected:
   handler* collection_hd;
   c_code_t instance_c_code;
   mmdb_pos_t ind;

   friend class info_base;
};

/*************************************/
// The info_base class
/*************************************/

class info_base : public base
{

public:
   info_base( object_dict& dict, 
	      char** set_nms, char** list_nms, 
              const char* base_dir, const char* base_name,
              const char* base_desc, const char* base_uid, 
	      const char* base_locale,
	      const mm_version& v
            );
   virtual ~info_base();

// export funcs
   cset_handlerPtr get_set( const char* set_name );
   cset_handlerPtr get_set( int set_position );

   dl_list_handlerPtr get_list( const char* list_name );
   dl_list_handlerPtr get_list( int list_position );

   void add_composite(char* new_db_path, char* def_spec_path);

// query functions
   int num_of_docs();
   mm_version& data_version() { return f_v; };

// iterate over all instances with 'c_code' in a set or a list
   Iterator* first(char* col_nm, c_code_t c_code);
   oid_t get_oid(const Iterator& ind);
   void next(Iterator&);

#ifdef C_API
// an integer id within the infolib
   int index_id() { return f_index_id; };
   void set_index_id(int x) { f_index_id = x; };
#endif

// build compression dicts. data is from stdin
   void build_dict(char* compress_agent_name);

   const char* get_info_base_locale() const { return info_base_locale; }

protected:

   cset_handlerPtr* info_base_set_ptrs;
   dl_list_handlerPtr* info_base_list_ptrs;

#ifdef C_API
   int f_index_id;
#endif

   mm_version f_v;

   int get_set_pos( const char* set_name );
   int get_list_pos( const char* list_name );

   friend class info_lib;

   char info_base_locale[PATHSIZ];
};

typedef info_base* info_basePtr;

#endif
