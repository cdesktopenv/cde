/*
 * $XConsortium: index.cc /main/6 1996/07/18 14:36:52 drk $
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


#include "index/index.h"

#ifdef C_API
key_type* c_index::v_static_key_ptr;
#define v_static_key (*v_static_key_ptr)
#else
key_type c_index::v_static_key(LBUFSIZ);
#endif

void c_index::init_persistent_info(persistent_info* x)
{
   root::init_persistent_info(x);
   set_mode(SWAP_ALLOWED, false);
}

c_index::c_index(c_code_t c_cd) : composite(c_cd)
{ 
   v_cmp_selector = 0;
}

oid_list_handler* c_index::operator()(int ind)
{
   return (*v_inv_lists_hd) -> get_list( ind );
}

ostream& operator <<(ostream& s, c_index&) 
{ 
   return s;
}

oid_t c_index::first_of_invlist(const handler& q)
{
   return first_of_invlist(handler_to_inv_idx(q));
}

oid_t c_index::first_of_invlist(int ind)
{
   oid_t x;
   oid_list_handler* y = (*this)(ind);

   if ( y == 0 ) {
     throw(stringException("NULL oid list handler ptr"));
   }

   if ( (*y) -> count() == 0 )
     throw(stringException("index::first_of_invlist(): empty list"));
   
   x = (*y) -> operator()((*y) -> first()); 

   delete y;

//MESSAGE(cerr, "c_index::first_of_invlist() returns: ");
//debug(cerr, x);
   return x;
}


Boolean c_index::get_key_string(const handler& t) const
{
   ostrstream out(v_static_key.get(), v_static_key.alloc_size());

   ((handler&)t) -> asciiOut(out);
   v_static_key.set_size(out.pcount());

   return true;
}

Boolean c_index::get_key_string(const oid_t& t) const
{
   v_static_key.reset();

   ostrstream out(v_static_key.get(), v_static_key.alloc_size());
   t.asciiOut(out);

   v_static_key.set_size(out.pcount());

   return true;
}


int c_index::cdr_sizeof()
{
   return composite::cdr_sizeof() + sizeof(v_cmp_selector);
}

io_status c_index::cdrOut(buffer& buf)
{
   composite::cdrOut(buf);
   buf.put(v_cmp_selector);
   return done;
}

io_status c_index::cdrIn(buffer& buf)
{
   composite::cdrIn(buf);
   buf.get(v_cmp_selector);
   return done;
}

void c_index::commit()
{
   if ( v_inv_lists_hd )
     v_inv_lists_hd -> commit();
}

HANDLER_BODIES(c_index)
