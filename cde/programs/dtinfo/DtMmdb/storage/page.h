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
 * $XConsortium: page.h /main/10 1996/07/18 14:54:33 drk $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */



#ifndef _page_h
#define _page_h 1

#include "utility/config.h"
#include "utility/funcs.h"
#include "utility/buffer.h"
#include "storage/rep_cell.h"
#include "storage/spointer.h"


// do not change to short! (for alignment reason)
typedef int count_t; 


class page : private buffer 
{

public:
   page(int buf_sz = PAGSIZ, int pid = 0, Boolean swap_order = false);
   virtual ~page() ;

// wipe the page clean
   void clean_all();

// get and put functions
   Boolean get( int slot_num, buffer& target,  
                int offset = 0, int bytes_to_read = 0
              ) const;  
                            // get the byte string at slot 'slot_num' from buf.

   Boolean put( int& slot_num, buffer& source ); 
                                       // put buf into the page into next 
            			       // available slot. the index of the slot
				       // is returned in slot_num.

   Boolean alloc_slot( int& slot_num, int size, char*& string_ptr ); 

   Boolean del_slot( int slot_num );
                               // delete the slot at postiion 'slot_num'

   Boolean update_slot( int slot_num, buffer& source, int offset = 0 );
   Boolean get_str_ptr( int slot_num, char*& str, int& len );

   virtual void reset() { buffer::reset(); };  

   spointer_t* get_spointer(int slot_num) const;

// return the number of objects on the page
   int count() const {  return *((count_t*)v_base); } ;          

   int page_id() { return pageid; };// page id
   int free_bytes() const;          // free bytes on the page
   int free_slots() const;          // free slots on the page
   char* page_base() { return v_base; };

// iteration functions

// go over all strings on the page
   int first() const { return ( count() <= 1 ) ? 0 : 1; };      

   void next(int &) const;
   void prev(int &) const;

// new operator
   void* operator new( size_t );
   void operator delete( void*, size_t );

// IO function and friend class
   friend ostream& operator<<(ostream&, page&);

   friend class page_storage;
   friend class page_cache_global_part;
   friend class dyn_hash;


protected:

// slot management funcs
   void set_count(int i) { 
      count_t& count = *(count_t*)(v_base);
      count = i;           // reset count
   };

   Boolean _alloc_slot( int slot_num, int size, char*& string_ptr ); 

   void _swap_order(Boolean swap_count_field_first);

protected:
   unsigned int pageid ;   // page id
   Boolean dirty ;         // = false: page has not been written;
   int num_locks;          // no. of locks
   int v_memalign_offset;  // align offset of the memory
                           // chunk for this page

   Boolean v_swap_order;

};

typedef page* pagePtr;

class page_storage;

class lru_page : public page, public rep_cell 
{

public:
   lru_page(page_storage* st, int buf_sz = PAGSIZ, 
	    int pid = 0, Boolean swap_order = false
           ) : 
       page(buf_sz, pid, swap_order), f_store(st) {};
   virtual ~lru_page() {};

   page_storage* f_store;
};

typedef lru_page* lru_pagePtr;

#endif
