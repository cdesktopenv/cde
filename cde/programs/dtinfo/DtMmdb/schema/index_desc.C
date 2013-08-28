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
 * $XConsortium: index_desc.cc /main/4 1996/06/11 17:32:17 cde-hal $
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


#include "schema/index_desc.h"

#include "index/inv_lists.h"
#include "index/mphf_index.h"
#include "index/dyn_disk_index.h"
#include "index/btree_index.h"


index_desc::index_desc(int class_code, const char* comment) :
	stored_object_desc(class_code, comment), position(0)
{
   inv_nm = strdup("");
   agent_nm = strdup("");
}

index_desc::~index_desc()
{
   delete inv_nm;
   delete agent_nm;
}

void index_desc::set_inv_nm(const char* str)
{
   delete inv_nm;
   inv_nm = strdup(str);
}

void index_desc::set_agent_nm(const char* str)
{
   delete agent_nm;
   agent_nm = strdup(str);
}

ostream& index_desc::asciiOut(ostream& out, Boolean last)
{
   desc::asciiOut(out, false);
   desc_print(out, inv_nm);
   desc_print3(out, "store_nm", get_store_nm());
   desc_print(out, position);

   if ( last == true )
      desc_print_end(out, agent_nm);
   else
      desc_print(out, agent_nm);

   if ( ! out )
     throw(stringException("index_desc::asciiOut() failed"));

   return out;
}

//////////////////////////////////////////////////
//
//////////////////////////////////////////////////
mphf_index_desc::mphf_index_desc() : 
	index_desc(MPHF_INDEX_CODE, "index	mphf_index")
{
}

handler* mphf_index_desc::init_handler(object_dict& dict)
{
   page_storage* store = (page_storage*)dict.get_store(get_store_nm());

   if ( v_oid.icode() == 0 ) {
      v_handler_ptr = new handler(MPHF_INDEX_CODE, store);
      desc::set_oid(v_handler_ptr -> its_oid());
   } else
      v_handler_ptr = new mphf_index_handler(v_oid, store);

/////////////////////////////
// get inv list component
/////////////////////////////
   handler* inv_handler = dict.get_handler(get_inv_nm());

/////////////////////////////
// init index agent 
/////////////////////////////
   handler* mphf_handler = dict.get_handler(get_agent_nm());

   (*(mphf_index_handler*)v_handler_ptr) ->
            init_data_member(
                  (fast_mphf_handler*)mphf_handler,
                  (inv_lists_handler*)inv_handler
                         );

   (*(mphf_index_handler*)v_handler_ptr) -> set_selector(position) ;

   return v_handler_ptr;
}

//////////////////////////////////////////////////
//
//////////////////////////////////////////////////
btree_index_desc::btree_index_desc() : 
	index_desc(MPHF_INDEX_CODE, "index	btree_index")
{
}

handler* btree_index_desc::init_handler(object_dict& dict)
{
   page_storage* store = (page_storage*)dict.get_store(get_store_nm());

   if ( v_oid.icode() == 0 ) {
      v_handler_ptr = new handler(BTREE_INDEX_CODE, store);
      desc::set_oid(v_handler_ptr -> its_oid());
   } else
      v_handler_ptr = new btree_index_handler(v_oid, store);

/////////////////////////////
// get inv list component
/////////////////////////////
   handler* inv_handler = dict.get_handler(get_inv_nm());

   (*(btree_index_handler*)v_handler_ptr) ->
            init_data_member(
                  (inv_lists_handler*)inv_handler,
                  form("%s/%s", get_agent_nm(), dict.db_path())
                         );

   (*(btree_index_handler*)v_handler_ptr) -> set_selector(position) ;

   return v_handler_ptr;
}

//////////////////////////////////////////////////
//
//////////////////////////////////////////////////
smphf_index_desc::smphf_index_desc() : 
	index_desc(DYN_DISK_INDEX_CODE, "index	smphf_index")
{
}

handler* smphf_index_desc::init_handler(object_dict& dict)
{
   page_storage* store = (page_storage*)dict.get_store(get_store_nm());

   if ( v_oid.icode() == 0 ) {
      v_handler_ptr = new handler(DYN_DISK_INDEX_CODE, store);
      desc::set_oid(v_handler_ptr -> its_oid());
   } else
      v_handler_ptr = new dyn_disk_index_handler(v_oid, store);

   handler* inv_handler = dict.get_handler(get_inv_nm());

   page_storage* idx_store = (page_storage*)dict.get_store(get_agent_nm());

   (*(dyn_disk_index_handler*)v_handler_ptr) -> 
       init_data_member( (inv_lists_handler*)inv_handler, idx_store );

   (*(dyn_disk_index_handler*)v_handler_ptr) -> set_selector(position) ;

   return v_handler_ptr;
}

