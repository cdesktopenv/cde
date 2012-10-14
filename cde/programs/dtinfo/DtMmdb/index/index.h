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
 * $XConsortium: index.h /main/7 1996/08/21 15:52:20 drk $
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


#ifndef _index_h
#define _index_h 1

#ifdef C_API
#include "utility/c_stringstream.h"
#else
#include <sstream>
#endif

#include "object/oid_list.h"
#include "index/inv_lists.h"

class c_index : public composite
{
   
public:
   c_index(c_code_t = INDEX_CODE);
   virtual ~c_index() {} ;

   void init_persistent_info(persistent_info* x);


   void set_selector(int cmp) { v_cmp_selector = cmp; };

// iteration functions
   virtual oid_list_handler* operator()(int index);
   virtual oid_t first_of_invlist(int ind) ;
   virtual oid_t first_of_invlist(const handler&);

// insert index functions
   virtual Boolean batch_index_begin() { return true; };
   virtual Boolean batch_index_end() { return true; };
   virtual Boolean insert_key_loc(const handler&, const oid_t&) = 0;
   virtual Boolean insert_key_loc(const oid_t&, const oid_t&) = 0;
   virtual Boolean load(const char*) { return true; };

// remove index functions
   virtual Boolean remove_loc(handler&, const oid_t&) = 0;
   virtual Boolean remove_loc(const oid_t&, const oid_t&) = 0;

// update index function
   virtual Boolean update_streampos(handler& old_obj,     
                                    handler& new_obj,
                                    mmdb_pos_t pos
                                   ) = 0;

// retreval functions
   virtual oid_list_handler* get_loc_list(const oid_t&)=0;
   virtual oid_list_handler* get_loc_list(const handler&)=0;
   virtual int invlist_length(handler&) = 0;

// translate a query to an index of an inv_list
   virtual size_t handler_to_inv_idx(const handler& query) = 0;

// status inquiry functions
   int bound_to() const { return v_cmp_selector; } ;
   virtual Boolean sorted() const = 0;

   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);
   void commit();

// i/o function
   friend ostream& operator <<(ostream& s, c_index& t) ;

#ifdef C_API
   friend void initialize_MMDB();
   friend void quit_MMDB();
#endif

protected:
   Boolean get_key_string(const handler&) const;
   Boolean get_key_string(const oid_t&) const;

protected:

#ifdef C_API
   static key_type* v_static_key_ptr;
#else
   static key_type v_static_key;
#endif

   int v_cmp_selector;
   inv_lists_handler *v_inv_lists_hd;
};

typedef c_index* indexPtr;

HANDLER_SIGNATURES(c_index)

#endif
