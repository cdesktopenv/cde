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
// $XConsortium: hash_desc.cc /main/3 1996/06/11 17:32:08 cde-hal $

#include "schema/hash_desc.h"
#include "handler/fast_mphf_handler.h"


mphf_desc::mphf_desc() : stored_object_desc(FAST_MPHF_CODE, "index_agent	mphf")
{
}

handler* mphf_desc::init_handler(object_dict& dict) 
{
   page_storage* store = (page_storage*)dict.get_store(store_nm);

   if ( v_oid.icode() == 0 ) {
      v_handler_ptr = new handler(FAST_MPHF_CODE, store);
      desc::set_oid(v_handler_ptr -> its_oid());
   } else
      v_handler_ptr = new fast_mphf_handler(v_oid, store);

   return v_handler_ptr;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

btree_desc::btree_desc() : stored_object_desc(BTREE_CODE, "index_agent	btree")
{
}

handler* btree_desc::init_handler(object_dict& dict) 
{
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

smphf_desc::smphf_desc() : stored_object_desc(DYN_DISK_HASH_CODE, "index_agent	smphf")
{
}

handler* smphf_desc::init_handler(object_dict& dict) 
{
}

