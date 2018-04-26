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
 * $XConsortium: cset.h /main/5 1996/06/11 17:24:03 cde-hal $
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


#ifndef _cset_h
#define _cset_h 1

#include "utility/filter.h"
#include "object/composite.h"
#include "index/index.h"

/***************************************
* a set class.
****************************************/

class cset: public composite
{

protected:
   c_index_handlerPtr *indices; 
   unsigned int num_indices;

public:

// indices[] array elements:
//      [0]: oid index;
//      [1]: whole object index;
//      [2]: component 1 index;
//      [3]: component 2 index;
//         ... ...    
//      [n+1]: component n object index.

   cset( c_code_t c_id = SET_CODE );
   virtual ~cset();

   void init_data_member(c_index_handlerPtr* indices, 
                      int num_indices
                     );

   MMDB_SIGNATURES(cset);

/*
// value comparison functions
   virtual Boolean value_EQ(root&, Boolean safe = true) const ;
   virtual Boolean value_LS(root&, Boolean safe = true) const ;
*/

// iteration over components
// no delete to returned component if comp is zero when passed in. 
   virtual handler* get_component(int index) ;

// get a set of locs for element objects satisfying a 'query'
// 'index' indicates the index to be used. 
   virtual oid_list_handler* get_locs(handler& query, int index);

   virtual oid_t get_first_oid(const handler& query, int index);

// get a pointer to an index
   virtual c_index_handler* get_index_ptr(int index);

// insert/remove a component
   virtual Boolean insert_object(const handler&) ;
   virtual Boolean remove_component(const oid_t&) ;

// update a component 
   virtual Boolean update_index( handler* old_comp_obj, 
                                 handler* new_comp_obj, 
                                 int index,
                                 oid_t& main_obj_oid
                                );

// compacted disk representation In and Out functions
   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);

   void commit(); // for commit all components


// init/quit batch index 
   void batch_index_begin();
   void batch_index_end();

// read in a set of object instances. Used with 
// batch_index_begin() and batch_index_end() to 
// load a mixed object stream
   io_status batch_asciiIn(istream& s); 

// object instance in/out function
   io_status asciiIn(istream& s) ;
   io_status asciiOut(ostream& s) ;

};

typedef cset* csetPtr;


HANDLER_SIGNATURES(cset)
   
#endif
