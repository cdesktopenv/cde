/*
 * $XConsortium: dyn_memory_index.cc /main/4 1996/06/11 17:21:31 cde-hal $
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



#include "index/dyn_memory_index.h"


dyn_memory_index::dyn_memory_index() : dyn_index(DYN_MEMORY_INDEX_CODE) 
{
}

Boolean
dyn_memory_index::init_data_member(inv_lists_handler* y, abs_storage* store ) 
{
   v_inv_lists_hd = y;
   storage_ptr = store;

   if ( v_inv_lists_hd && (*v_inv_lists_hd) -> OK() ) {
 
      v_idx_agent_ptr = new imp_die;

      if ( exist_file(storage_ptr -> my_name(), storage_ptr -> my_path()) == 
           true ) {

         fstream in(form("%s.%s", storage_ptr->my_path(), storage_ptr->my_name()), ios::in);
         v_idx_agent_ptr -> asciiIn(in);

      }

      set_mode(HEALTH, true);

      return true;
   } else {
      set_mode(HEALTH, false);
      return false;
   }
}

dyn_memory_index::~dyn_memory_index()
{
//MESSAGE(cerr, "~dyn_memory_index() called");

   if ( v_idx_agent_ptr ) {

/*
      char buf[PATHSIZ]; 
      for ( int i=0; i<PATHSIZ; buf[i++]=0 );

      ostrstream oid_t_out(buf, PATHSIZ);
      my_oid().asciiOut(oid_t_out);
*/

      abs_storage* x = get_store();
      fstream out(
           form("%s.%s", storage_ptr->my_path(), storage_ptr->my_name()), 
           ios::out
                 );
      v_idx_agent_ptr -> asciiOut(out);
   }
}

int dyn_memory_index::handler_to_inv_idx(const handler&)
{
   throw(stringException(
         "dyn_memory_index::handler_to_int(): not implemented yet"
                        )
        );
   return 0;
}

Boolean dyn_memory_index::remove_loc(handler&, const oid_t&)
{
   throw(stringException(
         "dyn_memory_index::remove_loc(): not implemented yet"
                        )
        );
   return false;
}

Boolean dyn_memory_index::remove_loc(const oid_t&, const oid_t&) 
{
   throw(stringException(
         "dyn_memory_index::remove_loc(): not implemented yet"
                        )
        );
   return false;
}
 
MMDB_BODIES(dyn_memory_index)
HANDLER_BODIES(dyn_memory_index)
