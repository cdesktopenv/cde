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
 * $XConsortium: page_storage.h /main/7 1996/07/18 14:55:30 drk $
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


#ifndef _page_storage_h
#define _page_storage_h 1

#define NUM_PAGES 10

#include "utility/pm_random.h"
#include "dstr/heap.h"
#include "dstr/void_ptr_array.h"
#include "dynhash/imp_die.h"
#include "storage/unixf_storage.h"
#include "storage/heap_comp_funcs.h"
#include "storage/page_cache.h"
#include "storage/spointer.h"

enum direction { positive, negative };

class page;

class str_index_record_t : public root
{

public:
   int str_offset;
   mmdb_pos_t loc;

   str_index_record_t(int offset, mmdb_pos_t lc = 0);
   virtual ~str_index_record_t() {};
};

void delete_str_index_record(const void* str_index_record_ptr);

typedef str_index_record_t* str_index_record_tPtr;

////////////////////////////////////////////
////////////////////////////////////////////
class store_trans
{

protected:
   enum trans_t { ENABLED, DISABLED };

   char* path;
   char* name;

   trans_t status;
   unixf_storage* log_store;
   imp_die* log_index;
   int max_pages;
   int page_sz;

public:
   store_trans(char* path, char*name, int page_sz); 
   ~store_trans();

   void init(rep_policy*);
   void quit();

   void set_max_pages(int max_pgs) {
      max_pages = max_pgs;
   };

   friend class page_storage;
};

/******************************************************/
// cached page storage. Implemented on Unix file class.
/******************************************************/

#ifdef C_API
#define f_global_pcache (*f_global_pcache_ptr)
#endif


class page_storage : public abs_storage 
{

protected:
   store_trans trans_info;

   int page_sz ;           // page size

   static int dv_sz ;
   static int abs_off ;

#ifndef C_API
   static page_cache_global_part f_global_pcache;
#else
   static page_cache_global_part* f_global_pcache_ptr;
#endif

   page_cache_local_part f_local_pcache;

   int total_pages;

// byte order 
   int v_server_order;
   int v_db_order;

   buffer* v_buf; 	   // aux. buf.

   int pagings ;
   int total_page_access;

protected:

   Boolean seek_loc_negative(mmdb_pos_t& loc, int smd);
   Boolean seek_loc_positive(mmdb_pos_t& loc, int smd);

public:
   page_storage(char* path, char* name,
                unixf_storage* store, 
                int page_sz = PAGSIZ, 
                int num_cached_pages = NUM_PAGES,
		mmdb_byte_order_t db_order_when_create_store = mmdb_big_endian
               );
   virtual ~page_storage();

   void remove(); // remove all pages in the store

   void sync();
   void sync(int pagenum);
   void sync(page*);

   void begin_trans();
   void commit_trans();
   void roll_back();
   void save_to_log(page* page_ptr);

// get server and db order
   int server_order() { return v_server_order; } ;
   int db_order() { return v_db_order; };


// i/o functions
   int readString (mmdb_pos_t loc, char* base, int len, int str_offset = 0); 
   int get_str_ptr(mmdb_pos_t loc, char*&, int& len) ;

   int updateString(mmdb_pos_t loc, const char* base, int len, int string_ofst = 0, Boolean flush = false);
   int deleteString (mmdb_pos_t loc, Boolean flush = false);
   int insertString(mmdb_pos_t& loc, const char* base, int len, Boolean flush = false);
   int allocString (mmdb_pos_t& loc, int len, char*&, int mode = 0);
   int appendString(mmdb_pos_t loc, const char*, int len, Boolean flush_opt = false);

   int set_page_dirty(mmdb_pos_t loc);

// iteration functions
   typedef enum access { READ, WRITE } access_t;
   int first() const;             // first page's index in the store
   page* operator()(int page_num, enum access intent) ;    //get the page
   void next(int&) const;         // next page's index. 


// format the store to contain extra empty 'pages' pages
   int add_page_frames(int pages);   


// get locs of the pieces that a long string is broken into
// The array should be deleted after use.
   int get_str_locs(mmdb_pos_t str_loc, str_index_record_tPtr*& locs, int& vector_leng);

// seek loc to (loc +/- 1) position 
   Boolean seek_loc(mmdb_pos_t& loc, const direction = positive, int = spointer_t::FIRST_RECD );
   mmdb_pos_t first_loc(); // return first loc in the store
   mmdb_pos_t last_loc(); // return last loc in the store

// store status query functions
   int page_size() { return page_sz; }; 

//
   void set_page_size(int pgsz) { page_sz = pgsz; }; 

// how many pages in total in the store
   int pages() const { return total_pages; }; 

// paging counting function
   void reset_paging_count();
   int paging_count() const;

   Boolean io_mode(int mode) ;

// return an aux. buf.
   buffer& aux_buf();

// printing functions
   virtual io_status asciiOut(ostream&) ;
 
   friend void close_file(const void*);
   friend void remove_from_global_cache(const void*);

   friend class storage_mgr_t;
   friend class handler;
   friend class page_cache_local_part;
   friend class page_cache_global_part;

#ifdef C_API
   friend void initialize_MMDB();
   friend void quit_MMDB();
#endif
};

typedef page_storage* page_storagePtr;

#endif
