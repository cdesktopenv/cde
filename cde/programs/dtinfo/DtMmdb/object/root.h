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
 * $XConsortium: root.h /main/6 1996/07/18 14:46:10 drk $
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


#ifndef _root_h
#define _root_h 1

#include "utility/funcs.h"
#include "utility/buffer.h"
#include "storage/rep_cell.h"
#include "storage/page.h"
#include "object/oid_t.h"
#include "object/c_codes.h"
#include "object/new_delete.h"
//#include "Exceptions.hh"

class abs_storage;


/************************************/
// constants used to name status bits
/************************************/
enum obj_mode_t { HEALTH, 
                  SWAP_ALLOWED, 
                  PERSISTENT, 
                  UPDATE, 
                  OLD_OBJECT, 
                  CDR 
                };

/************************************/
// structure recording various status 
// of an object
/************************************/
struct status_t 
{
   unsigned reserved: 	10;   // 
   unsigned swap: 	1;   // swapped out allowed?   (true, or false)
   unsigned cdr: 	1;   // compacted disk rep?   (true, or false)
   unsigned ok: 	1;   // in good shape ?   (true, or false)
   unsigned persistent: 1;   // persistent ?      (true, or false)
   unsigned update:     1;   // updated ?         (true, or false)
   unsigned old_object :1;    // old object? (true, or false)
   unsigned ref_count: 	16;   // reference count [0, 2^16 - 1];
};

/************************************/
// structure recording io information
/************************************/
class persistent_info
{
public:

   persistent_info(abs_storage* s = 0, c_code_t = 0,
       mmdb_pos_t = 0, Boolean =0, Boolean = 0);
   ~persistent_info() {};
   
   Boolean cdr;
   abs_storage* storage;
   c_code_t  class_code;
   mmdb_pos_t position;
   Boolean persistent; // true -> on persisent store
                       // false -> on memory 
   Boolean old_object; // true -> object has been created on store
                       
};

extern persistent_info transient_info;

/*************************************/
// The root class
/*************************************/

#define THIS		     0 // const to name this object
#define BASE_COMPONENT_INDEX 1 // const to name the 1st component

class root : public Destructable, public rep_cell
{
protected:

/***************************/
// instance-wise data 
/***************************/
   oid_t f_oid;
   status_t status;
   abs_storage* storage_ptr;

   virtual void init_persistent_info(persistent_info* = &transient_info);
   void set_c_code(c_code_t); // set class code

/***************************/
// static data and function
/***************************/
   static void* heap_alloc( size_t sz );

public:
   root(c_code_t c_id = ROOT_CODE);
   root(const oid_t& );
   root(const root&);
   virtual ~root() ;

#ifdef C_API
   NEW_AND_DELETE_SIGNATURES(root);
#endif

   MMDB_SIGNATURES(root);

// status set and get functions
   virtual void set_mode(obj_mode_t, Boolean);
   Boolean get_mode(obj_mode_t) const;

// reference count set and get functions
   void reset_ref_count();
   void set_ref_count(int delta);
   int get_ref_count();

   abs_storage* get_store() { return storage_ptr; };

// status inquiry functions
   virtual Boolean OK() const ;        // in good shape ? 
   virtual ostream& memory_layout(root*, ostream& = cerr);

/*
// value comparison functions 
   virtual Boolean value_EQ(root&, Boolean safe = true) const ;
   virtual Boolean value_LS(root&, Boolean safe = true) const ;
*/

// export functions 
   const oid_t& my_oid() const ;      // get oid

// ascii In and Out functions
   virtual io_status asciiOut(ostream&) ; // output (ASCII)
   virtual io_status asciiIn(istream&) ;  // init from istream (ASCII)  

   friend ostream& operator<<(ostream&, const root&) ;  

// compacted disk representation In and Out functions
   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);
   virtual void commit() {}; // for commit all components


   friend class template_mgr_t;
   friend class service_mgr_t;
   friend class oid;

#ifdef C_API
   friend void initialize_MMDB();
   friend void quit_MMDB();
#endif

};

typedef root* rootPtr;


#endif
