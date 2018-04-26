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
 * $XConsortium: dyn_index.h /main/5 1996/07/18 14:35:04 drk $
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



#ifndef _dyn_index_h
#define _dyn_index_h 1

#include "index/index.h"
#include "dstr/index_agent.h"
#include "index/inv_lists.h"

class dyn_index : public c_index
{

public:
   dyn_index(c_code_t = DYN_INDEX_CODE);
   virtual ~dyn_index();

// insert index functions
   virtual Boolean insert_key_loc(const handler&, const oid_t&) ;
   virtual Boolean insert_key_loc(const oid_t&, const oid_t&) ;
  
// remove index functions
   virtual Boolean remove_loc(handler&, const oid_t&) ;
   virtual Boolean remove_loc(const oid_t&, const oid_t&);

// update index function
   virtual Boolean update_streampos(handler& old_obj,     
                                    handler& new_obj,
                                    mmdb_pos_t pos
                                   ) ;

// retrieval functions
   virtual oid_list_handler* get_loc_list(const oid_t&);
   virtual oid_list_handler* get_loc_list(const handler&);
   virtual int invlist_length(handler&) ;

// status inquiry functions
   virtual Boolean sorted() const { return false; };  

// io function
   virtual io_status asciiOut(ostream&);

protected:
   Boolean _insert_loc(data_t& intKey, const oid_t& id);
   Boolean _remove_loc(data_t& intKey, const oid_t& id);
   data_t* hd_to_key(const handler& t);

protected:
   index_agent *v_idx_agent_ptr;        // index agent (e.g. btree, hash) func

};

#endif
