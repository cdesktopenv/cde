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
 * $XConsortium: composite.h /main/4 1996/06/11 17:23:39 cde-hal $
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


#ifndef _composite_h
#define _composite_h 1

#include "object/root.h"
#include "storage/page_storage.h"
#include "object/handler.h"

/******************************************
* composite object class
*******************************************/
class composite: public root
{

public:
   NEW_AND_DELETE_SIGNATURES(composite);

   composite(c_code_t c_id = COMPOSITE_CODE);
   composite(const composite& x);
   virtual ~composite() {};

// iteration over components
   virtual int first() const;
   virtual handler* get_component(int) ;
   //virtual handler* get_component(oid_t&) ;
   virtual void next(int& index) const;
   
// number of components 
   virtual int count() const { return v_sz; };

   MMDB_SIGNATURES(composite);

// insert a component
   virtual Boolean insert_component(const oid_t&) {
     return true; };

// update a component
   virtual Boolean update_component(int, const oid_t&) {
     return true; 
   };

   void set_mode(obj_mode_t, Boolean);

   Boolean test_io_mode(int mode);

// compacted disk representation In and Out functions
   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);


   Boolean swap_order() {
     return (storage_ptr) ? storage_ptr -> swap_order() : false;
   };


protected:
   unsigned int v_sz;  // number of components
};

typedef composite* compositePtr;

class composite_handler : public handler
{

public:
   composite_handler()  {};
   composite_handler(const oid_t&, storagePtr = 0);
   virtual ~composite_handler();

   composite* operator ->();

   handler* _get_component(int i);
};

   

#endif
