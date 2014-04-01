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
 * $XConsortium: misc.h /main/4 1996/06/11 17:22:54 cde-hal $
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


#ifndef _misc_h
#define _misc_h

#include "object/root.h"
#include "utility/ostring.h"
#include "storage/abs_storage.h"
#include "object/handler.h"

Boolean name_oid_ls(const void* o1, const void* o2);
Boolean name_oid_eq(const void* o1, const void* o2);

Boolean oid_storage_ls(const void* o1, const void* o2);
Boolean oid_storage_eq(const void* o1, const void* o2);

//Boolean oid_ls(const void* o1, const void* o2);
//Boolean oid_eq(const void* o1, const void* o2);

class name_oid_t 
{

public:
   name_oid_t(const char* nm, abs_storage* store = 0) : v_store(store) 
   { 
     v_name.set(nm);
   }
   name_oid_t(const char* nm, const oid_t& id, abs_storage* store = 0) : 
      v_oid(id), v_store(store) 
   { 
     v_name.set(nm);
   };

   ~name_oid_t() {};

public:
   ostring v_name;
   oid_t v_oid;
   abs_storage* v_store;
};

void delete_name_oid_rec_f(const void* name_oid_ptr);

class mark_t : private ostring
{
public:
   mark_t(char* marks = (char*)"\t\n ");
   virtual ~mark_t() {};
   friend istream& operator >>(istream&, mark_t&);
   friend ostream& operator <<(ostream&, mark_t&);
};


#endif
