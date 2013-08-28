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
 * $XConsortium: handler.h /main/4 1996/07/18 14:41:47 drk $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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


#ifndef _handler_h
#define _handler_h 1

#include "dstr/memory_pool.h"
#include "storage/page_storage.h"
#include "object/root.h"
#include "mgrs/template_mgr.h"

class handler : public Destructable
{

public:
   handler() ;
   handler(const oid_t& id, abs_storage* store = 0);
   handler(c_code_t class_code, abs_storage* store);
   handler(rootPtr ptr, abs_storage* store);

   virtual ~handler();

// a light-weight way to resue handler resouce
   void set(rootPtr ptr, abs_storage* store);

   virtual void sync();
   virtual void commit();
   virtual void destroy();

   void* operator new( size_t ); 
   void operator delete( void* ); 

   root* operator ->();
   operator root&();

#ifdef A16_BROWSER
   oid_t& its_oid() { return obj_id; } ;
#else
   const oid_t& its_oid() const { return obj_id; } ;
#endif

   abs_storage* its_store() const { return store; } ;

protected:
   //static memory_pool handler_space_pool;

protected:
   abs_storage* store;
   oid_t obj_id;
   rootPtr obj_ptr;
};


#define HANDLER_SIGNATURES(class_name) \
class class_name ## _handler : public handler \
{ \
 \
public: \
   class_name ## _handler(const oid_t&, storagePtr = 0); \
   virtual ~class_name ## _handler(); \
 \
   class_name * operator ->(); \
}; \
\
typedef class_name ## _handler* class_name ## _handlerPtr; 


#define HANDLER_BODIES(class_name) \
\
class_name ## _handler::class_name ## _handler(const oid_t& v_oid, storagePtr _store): \
handler(v_oid, _store) \
{ \
} \
 \
class_name ## _handler::~class_name ## _handler() \
{ \
} \
 \
class_name* class_name ## _handler::operator ->() \
{ \
   return (class_name*)handler::operator->(); \
} 
 
#endif
