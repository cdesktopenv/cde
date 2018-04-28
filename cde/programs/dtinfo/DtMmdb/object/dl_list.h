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
 * $XConsortium: dl_list.h /main/5 1996/06/11 17:24:17 cde-hal $
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


#ifndef _dl_list_h
#define _dl_list_h 1

#include "utility/filter.h"
#include "object/composite.h"
#include "index/index.h"
#include "object/oid_list.h"
#include "object/dl_list_cell.h"

/***************************************
* dl_list class.
****************************************/

class dl_list: public composite
{


public:
   dl_list(c_code_t = DL_LIST_CODE);
   dl_list(const dl_list&);
   virtual ~dl_list();

   void init_data_member(c_index_handlerPtr* indices, 
                      int num_indices
                     );

   MMDB_SIGNATURES(dl_list);

/*
// value comparison functions
   virtual Boolean value_EQ(root&, Boolean safe = true) const ;
   virtual Boolean value_LS(root&, Boolean safe = true) const ;
*/

   virtual oid_t get_first_oid(const handler& query, int index);

// insert component functions. new_cell and old_cell
// are handlers to dl_list_cells already existing on
// the store 
   Boolean insert_before(dl_list_cell_handler& new_cell,
                         dl_list_cell_handler& old_cell
                        );
   Boolean insert_after(dl_list_cell_handler& new_cell,
                        dl_list_cell_handler& old_cell
                       );

// insert as head/tail cell. new_cell is a handler to a 
// del_list_cell already existing on the store
   Boolean insert_as_head(const dl_list_cell_handler& new_cell);
   Boolean insert_as_tail(dl_list_cell_handler& new_cell);

// iteration over components
   virtual handler* get_component(int index) ;

// get a set of locs for element objects satisfying a 'query'
// 'index' indicates the index to be used. 
   virtual oid_list_handler* get_locs(handler& query, int index)
;

// first/last cell's oid.
   oid_t first_cell_oid() const ;
   oid_t last_cell_oid() const ;

// init/quit batch index
   void batch_index_begin();
   void batch_index_end();

// read in a set of object instances. Used with
// batch_index_begin() and batch_index_end() to
// load a mixed object stream
   io_status batch_asciiIn(istream& s);

// instance in/out function
   virtual io_status asciiOut(ostream&) ;
   virtual io_status asciiIn(istream&) ;

// compacted disk representation In and Out functions
   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);
   void commit();

protected:
   Boolean insert_to_indices(const dl_list_cell_handler& new_cell);

protected:
   oid_t v_dl_list_head;
   oid_t v_dl_list_tail;

   c_index_handlerPtr *v_indices;
   unsigned int v_num_indices;
};

HANDLER_SIGNATURES(dl_list)
   
#endif
