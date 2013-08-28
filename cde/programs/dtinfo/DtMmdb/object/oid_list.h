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
 * $XConsortium: oid_list.h /main/5 1996/07/18 14:43:51 drk $
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


#ifndef _oid_list_h
#define _oid_list_h 1

#include "utility/ostring.h"
#include "dstr/dlist.h"
#include "dstr/dlist_void_ptr_cell.h"
#include "object/oid.h"
#include "object/composite.h"
#include "storage/page_storage.h"
#include "storage/chunks_index.h"

/***************************************
* Primitive string class.
****************************************/

class oid_list: public composite
{
protected:
   Boolean f_internal_index;
   chunks_index* chk_index;

   union {
      mmdb_pos_t loc;
      ostring* p;
   } list_ptr;

protected:
// init the list (memory resident) with sz dummy oid_ts
   void init_data_member(int sz);    

public:
   void init_persistent_info(persistent_info* x);


   oid_list(oid_list&);
   oid_list(c_code_t = OID_LIST_CODE);
   oid_list(int num_oids, c_code_t);
   virtual ~oid_list();

// expand the list to include extra x oids.
// handle both the memory and disk list.
   Boolean expand_space(int x);

   void build_internal_index(); // set up an index on the list
   void reqest_build_internal_index(); 
                               // request building an internal 
			       // index in the operator()

   MMDB_SIGNATURES(oid_list);

   virtual oid_t operator()(int);

// insert a component
   virtual Boolean insert_component(const oid_t&);

// update a component
   virtual Boolean update_component(int index, const oid_t&);

// remove a component
   virtual Boolean remove_component(const oid_t&);

// print function
   virtual io_status asciiOut(ostream&) ;
   virtual io_status asciiIn(istream&) ;

// compacted disk representation In and Out functions
   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);

   friend class oid_list_handler;
};

typedef oid_list* oid_listPtr;

/*
#endif
   
#ifndef _oid_list_handler_h
#define _oid_list_handler_h 1
*/

class oid_list_handler : public handler
{

public:
   oid_list_handler(int num_oids, storagePtr = 0);
   oid_list_handler(const oid_t&, storagePtr = 0);
   virtual ~oid_list_handler();

   oid_list* operator ->();
};

   
#endif
