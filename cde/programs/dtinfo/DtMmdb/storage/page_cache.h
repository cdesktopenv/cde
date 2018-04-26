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
/* $XConsortium: page_cache.h /main/3 1996/06/11 17:44:27 cde-hal $ */

#ifndef _page_cache_h
#define _page_cache_h 1

class page;
class page_storage;

#define MMDB_CACHED_PAGES 100
#define MIN_MMDB_CACHED_PAGES 10

#include "dstr/bset.h"

class page_cache_local_part
{

protected:
   int f_num_cached_pages ;  // number of cached pages

   heap f_free_space;        // a heap recording free
                             // space on each page

   bset f_page_pool_index;    // index to pages in the pool

   int f_current_page_num;
   page* f_current_page_ptr;

public:
   page_cache_local_part(unsigned int exp_cached_page);
   virtual ~page_cache_local_part();

// init the heap
   Boolean init_heap(page_storage*);
   Boolean quit_heap(page_storage*);

// does the cache hold page 'page_num'?
   page* in_cache(page_storage* st, int page_num);

// get a non-empty page
   fbytes_t* find_host_page(page_storage*, Boolean& new_page, int len = 0);
   void adjust_heap(fbytes_t* v, Boolean new_page);

   friend class page_cache_global_part ;
};

class lru;

class page_cache_global_part 
{

public:
//////////////////////////////////////////////////////////////////////
// The default value (0) triggers the constructor to 
// search for the value in the shell variable MMDB_CACHED_PAGES.
// If the varialble is undefind, the value set by const MMDB_CACHED_PAGES
// will be used.
//////////////////////////////////////////////////////////////////////
   page_cache_global_part(unsigned int total_allowed_pages = 0);
   virtual ~page_cache_global_part() ;

   void remove_pages(page_storage* st);

// return a free page frame from cache
   page* load_new_page(page_storage*, int page_num, Boolean byteOrder);

   ostream& print_cached_pages(ostream&); // show cached pages.

   friend class page_cache_local_part;
   friend class page_storage;
   friend void remove_from_global_cache(const void*);

protected:
   unsigned int    f_total_allowed_pages;
   lru             f_replace_policy;
};

#endif
