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
 * $XConsortium: object_dict.h /main/4 1996/06/11 17:32:47 cde-hal $
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



#ifndef _object_dict
#define _object_dict 1

#include "dstr/bset.h"
#include "utility/ostring.h"
#include "object/handler.h"

#include "mgrs/template_mgr.h"
#include "schema/store_desc.h"
#include "schema/stored_object_desc.h"

#define SCHEMA_FILE             "schema.mmdb"
#define SCHEMA_FILE_SUFFIX      "sch"

class object_dict
{

public:
   object_dict();
   virtual ~object_dict();

   handler* get_handler(const char* obj_name);
   abs_storage* get_store(const char* store_name);

   desc* init_a_base(char* db_path, char* db_name);
   desc* init_a_base(char* define_desc_path, char* db_path, char* db_name);

   const char* db_path() { return v_db_path; };

   friend class server;

protected:
   desc* parse(char* define_desc_path);
   desc* parse(buffer& desc_buffer);
   void _init(desc*);

   void quit_a_base(desc* start_ptr, desc* end_ptr = 0, Boolean sync = true);

   void _quit_stores(desc* start_ptr, desc* end_ptr = 0, Boolean sync = true);
   void _quit_stored_objects(desc* start_ptr, desc* end_ptr = 0);
   void _quit_descs(desc* start_ptr, desc* end_ptr = 0);

protected:
   bset v_dict;             // name to oid mapping
   desc * v_desc_ptr;       // list of stored object desc cells
   desc * v_last_desc_ptr;  // tail cell 
   char v_db_path[PATHSIZ]; // db_path
};

#endif
