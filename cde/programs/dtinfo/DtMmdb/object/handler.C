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
 * $XConsortium: handler.cc /main/5 1996/07/18 14:41:25 drk $
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


#include "object/handler.h"
#include "mgrs/managers.h"

//memory_pool handler::handler_space_pool;
extern memory_pool g_memory_pool;

handler::handler() : 
store(0), obj_id(ground), obj_ptr(0)
{
}

handler::handler(const oid_t& id, abs_storage* s) : 
store(s), obj_id(id), obj_ptr(0)
{
}

handler::handler(c_code_t cod, abs_storage* s) : 
store(s), obj_id(cod, 0), obj_ptr(0)
{
   if ( s ) 
      operator->(); // to init the object from the store
}

handler::handler(rootPtr ptr, abs_storage* s) : 
store(s), obj_id(ptr -> my_oid()), obj_ptr(ptr)
{
}

void handler::set(rootPtr ptr, abs_storage* s)
{
   store = (abs_storage*)s;

   if ( ptr )
      obj_id.become(ptr -> my_oid());

   obj_ptr = ptr;
}

handler::~handler()
{
   if ( store ) {
      managers::template_mgr -> quit_obj(store, obj_ptr);
   } else
      delete obj_ptr;

//   commit();
//   delete obj_ptr;
}

void handler::destroy()
{
   if ( store )
      managers::template_mgr -> destroy_obj(store, obj_ptr);
   else
      delete obj_ptr;
}

void handler::sync()
{
   if ( store ) store -> sync();
}

void handler::commit()
{
   if ( store ) {
      if ( obj_ptr )
         obj_ptr -> commit(); // save all its handler components
      managers::template_mgr -> commit_obj(store, obj_ptr);
   }
}

root* handler::operator ->()
{
   if ( store ) {

      if ( obj_ptr == 0 ) {

/*
           (obj_ptr && obj_id.eq(obj_ptr -> my_oid()) == false )
MESSAGE(cerr, "handler::operator ->()");
debug(cerr, (void*)obj_ptr);
debug(cerr, obj_id);
        if ( obj_ptr -> get_mode(SWAP_ALLOWED) == true )
           //r_obj_cache.promote_object(store, obj_ptr);
*/

         //obj_ptr = r_obj_cache.init_object(store, obj_id);

         if ( obj_id.icode() ) {
           managers::template_mgr -> init_obj(store, obj_id.icode(), obj_ptr);
         } else {
           managers::template_mgr -> create_obj(store, obj_id.ccode(), obj_ptr);
         }


///////////////////////////////////////////////////////////////////
// update the handler side object id. Its class code may not
// be initialized as class code of a oid_t is not saved on the disk.
// We do not expect this check will degrade the performance as
// this init_object block is only called once for the operator->().
// Subsequent calls will bypass it as obj_ptr is not 0.
///////////////////////////////////////////////////////////////////
         obj_id.become(obj_ptr -> my_oid());

      } 
   }

   if ( store && obj_ptr == 0 ) {
       debug(cerr, long(store));
       debug(cerr, its_oid());
       throw(stringException("null obj ptr"));
   };

   return obj_ptr;
}


handler::operator root&()
{
   return *(this -> operator->());
}

void* handler::operator new( size_t x )
{
   //return (void*)g_memory_pool.alloc(x);

   return ::operator new(x);
}

void handler::operator delete( void* ptr )
{
   //g_memory_pool.free((char*)ptr);

   ::operator delete(ptr);
}
