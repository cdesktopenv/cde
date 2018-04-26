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
 * $XConsortium: agent_desc.cc /main/4 1996/06/11 17:31:35 cde-hal $
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


#include "schema/agent_desc.h"
#include "index/fast_mphf.h"
#include "compression/abs_agent.h"


mphf_desc::mphf_desc() : stored_object_desc(FAST_MPHF_CODE, "index_agent	mphf")
{
}

handler* mphf_desc::init_handler(object_dict& dict) 
{
   page_storage* store = (page_storage*)dict.get_store(get_store_nm());

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
   return 0;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

smphf_desc::smphf_desc() : stored_object_desc(DYN_DISK_HASH_CODE, "index_agent	smphf")
{
}

handler* smphf_desc::init_handler(object_dict& dict) 
{
   return 0;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

huffman_desc::huffman_desc() : 
   stored_object_desc(HUFFMAN_AGENT_CODE, "compress	huffman")
{
}

handler* huffman_desc::init_handler(object_dict& dict) 
{
   page_storage* store = (page_storage*)dict.get_store(get_store_nm());

   if ( v_oid.icode() == 0 ) {
      v_handler_ptr = new handler(HUFFMAN_AGENT_CODE, store);
      desc::set_oid(v_handler_ptr -> its_oid());
   } else
      v_handler_ptr = new compress_agent_handler(v_oid, store);

   return v_handler_ptr;
}

/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

dict_desc::dict_desc() : 
   stored_object_desc(DICT_AGENT_CODE, "compress	dict")
{
}

handler* dict_desc::init_handler(object_dict& dict) 
{
   page_storage* store = (page_storage*)dict.get_store(get_store_nm());

   if ( v_oid.icode() == 0 ) {
      v_handler_ptr = new handler(DICT_AGENT_CODE, store);
      desc::set_oid(v_handler_ptr -> its_oid());
   } else
      v_handler_ptr = new compress_agent_handler(v_oid, store);

   return v_handler_ptr;
}

