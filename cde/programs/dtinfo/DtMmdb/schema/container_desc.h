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
/* $XConsortium: container_desc.h /main/4 1996/06/11 17:31:53 cde-hal $ */

#ifndef _container_desc_h
#define _container_desc_h 1

#include "schema/stored_object_desc.h"
#include "schema/object_dict.h"
#include "dstr/void_ptr_array.h"
#include "object/dl_list.h"
#include "object/cset.h"
#include "index/index.h"

class container_desc : public stored_object_desc {

public:
   container_desc(int class_code, const char* comment = "");
   virtual ~container_desc();

   void set_index_nm(const char*);
   virtual ostream& asciiOut(ostream& out, Boolean last = true);

   void init_index_array(object_dict&, c_index_handlerPtr*&, int& sz) ;

protected:
   void_ptr_array index_nms;
};

class set_desc : public container_desc {

public:
   set_desc();
   virtual ~set_desc() {};

   handler* init_handler(object_dict&) ;

protected:
};

class list_desc : public container_desc {

public:
   list_desc();
   virtual ~list_desc() {};

   handler* init_handler(object_dict&) ;

protected:
};


#endif
