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
 * $XConsortium: btree_index.cc /main/4 1996/06/11 17:21:02 cde-hal $
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




#include "index/btree_index.h"

#ifdef C_API
#define v_static_key (*v_static_key_ptr)
#endif


btree_index::btree_index() : dyn_index(BTREE_INDEX_CODE) 
{
}

Boolean
btree_index::init_data_member( inv_lists_handler* y, const char* btree_store ) 
{
   v_inv_lists_hd = y;
   v_idx_agent_ptr = new btree(btree_store);

   set_mode(HEALTH, true);
   return true;
}

btree_index::~btree_index()
{
   delete v_idx_agent_ptr;
}

size_t btree_index::handler_to_inv_idx(const handler& query)
{
   get_key_string(query);

   data_t k(v_static_key.get(), v_static_key.size());

   if ( v_idx_agent_ptr -> member(k) == false )
     throw(stringException("first_of_invlist(): key is not in btree"));

   return size_t(k.dt);
}

 
MMDB_BODIES(btree_index)
HANDLER_BODIES(btree_index)
