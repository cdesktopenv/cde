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
/* $XConsortium: page_cache.cc /main/4 1996/06/11 17:44:22 cde-hal $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#include "utility/debug.h"
#include "storage/page.h"
#include "storage/heap_comp_funcs.h"
#include "storage/page_rep.h"
#include "storage/page_storage.h"

page_cache_local_part::page_cache_local_part(unsigned int exp_cached_page) :
 f_num_cached_pages(0),
 f_free_space(fbytes_t_eq, fbytes_t_ls, exp_cached_page),
 f_page_pool_index(page_rep_eq, page_rep_ls),
 f_current_page_num(0),
 f_current_page_ptr(0)
{
}

page_cache_local_part::~page_cache_local_part()
{
   f_page_pool_index.del_elements(page_rep_del);
}

Boolean page_cache_local_part::init_heap(page_storage* st)
{
////////////////////////////////////////////////////////////////////////////
// We use a different strategy. Since pages are most of time read-only and 
// grows linearly when loaded, we assume the 1st to pages()-1 th page are 
// full. 
////////////////////////////////////////////////////////////////////////////

   if ( st -> pages() > 0 ) {

      page* z = (*st)(st -> pages(), page_storage::READ);

      f_free_space.insert(
		new fbytes_t(z -> free_bytes(), z -> page_id())
			      ); 

      f_free_space.heapify();
   }

   return true;
}

Boolean page_cache_local_part::quit_heap(page_storage* st)
{
   int ind = f_free_space.first();
	
   while ( ind ) {
      fbytes_t *x = (fbytes_t*)f_free_space(ind);

/*
debug(cerr, x -> page_num);
debug(cerr, x -> free_bytes);
*/

      delete x;
      f_free_space.next(ind);
   }

   return true;
}

page* page_cache_local_part::in_cache(page_storage* st, int page_num)
{

#ifdef DEBUG
st -> total_page_access++;
#endif

   if ( f_current_page_num == page_num ) {
      return f_current_page_ptr;
   }

   page_rep data(page_num, 0);
   page_rep *result = 0;

/*
cerr << "member(): " << st -> my_name() << " ";
cerr << page_num ;
cerr << " " << (void*)&f_page_pool_index;
cerr << "\n";
*/

   if ( (result = (page_rep*)f_page_pool_index.member(&data)) ) {
//cerr << "In Cache: page " << page_num << " of " << st -> my_name() << "\n";

     lru_page* p = result -> f_page_ptr;

     if ( p -> page_id() != page_num ) {
        throw(stringException("corrupted store"));
     }

     if ( p -> get_position() == ACTIVE ) {
#ifndef C_API
        (p -> f_store -> f_global_pcache).f_replace_policy.promote(*p);
#else
        (p -> f_store -> f_global_pcache_ptr -> f_replace_policy).promote(*p);
#endif
//cerr << "Promoting:" << (long)p << endl;
     }

     return p;

   } else {

#ifdef DEBUG
st -> pagings++;
#endif

      return 0;
   }
}

void page_cache_local_part::adjust_heap(fbytes_t* v, Boolean new_page)
{
    if ( new_page == false ) {
      if ( v -> free_bytes < 10 ) {
         f_free_space.delete_max();
         delete v;
      } else
         f_free_space.adjust_max_to(v);
   } else {
      f_free_space.insert_heapify(v);
   }
}

fbytes_t* 
page_cache_local_part::find_host_page(
	page_storage* st, Boolean& new_page, int len
				     )
{

   fbytes_t* x = (fbytes_t*)f_free_space.max_elm();

//if ( x )
//  debug(cerr, x -> free_bytes);
//else
//  MESSAGE(cerr, "no cached page");

   if ( x != 0 && x -> free_bytes > len ) {

      new_page = false;

   } else {
      st -> add_page_frames(1);
      new_page = true;
      x = new fbytes_t((*st)(st->pages(), page_storage::READ) -> free_bytes(), st->pages());
   }

   return x;
}



////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////


page_cache_global_part::page_cache_global_part(unsigned int allowed_pages) :
f_total_allowed_pages(allowed_pages),
f_replace_policy(allowed_pages, 0, true)
{
   if ( allowed_pages == 0 ) {
     char* s = getenv("MMDB_CACHED_PAGES");
     if ( s ) {
       f_total_allowed_pages = atoi(s);
       if ( f_total_allowed_pages < MIN_MMDB_CACHED_PAGES ) // minimal value
          f_total_allowed_pages = MIN_MMDB_CACHED_PAGES;
     } else
       f_total_allowed_pages = MMDB_CACHED_PAGES;

     f_replace_policy.set_params(f_total_allowed_pages, 0);
   } else
   if ( allowed_pages < MIN_MMDB_CACHED_PAGES ) {
     f_total_allowed_pages = MIN_MMDB_CACHED_PAGES;
     f_replace_policy.set_params(f_total_allowed_pages, 0);
   }
}

page_cache_global_part::~page_cache_global_part() 
{
}

page* 
page_cache_global_part::load_new_page(
	page_storage* st, int new_page_num, Boolean byte_order)
{
   lru_pagePtr p = 0;

   if ( f_replace_policy.active_elmts() < (int) f_total_allowed_pages ) {

/*
debug(cerr, page_cache -> active_elmts());
debug(cerr, num_cached_pages);
MESSAGE(cerr, "new a page");
*/
      p = new lru_page(st, st -> page_sz, new_page_num, byte_order);

//cerr << "New page " << new_page_num << " of " << st -> my_name() << "\n";

   } else {

      long ind = f_replace_policy.last(ACTIVE);
//cerr << "Getting last:" << (long)ind << endl;

      if ( ind == 0 )
         throw(stringException("corrupted store"));

      p = (lru_page*)f_replace_policy(ind, ACTIVE);

      int pid = p -> page_id();
      page_rep key(pid, 0);
  
      page_storage* pst = p -> f_store;

//cerr << "Removing " << pid ;
//cerr <<  " for " << pst -> my_name() << " ";

      delete (page_rep*)((pst->f_local_pcache).f_page_pool_index.remove(&key));

////////////////////////////////////////////////////
// Clean the current_page entry in the local cache.
////////////////////////////////////////////////////
      int & local_pid = (pst->f_local_pcache).f_current_page_num;
      if ( local_pid  == pid ) local_pid = 0;

      if ( p -> dirty == true ) {

#ifdef PORTABLE_DB
////////////////////////////////////////////////////////////
// swap the count field last.
// no need to flip back to the original as the page frame
// is reused.
////////////////////////////////////////////////////////////
         p -> _swap_order(false);
#endif

//cerr << " swapout";

         int page_aoff = ( pid - 1 ) * (pst -> page_sz);
         pst -> storage_ptr -> updateString(
		page_storage::abs_off + mmdb_pos_t(page_aoff), 
		p -> page_base(), 
		pst->page_sz
                                    	  );
      }
//cerr << "\n";

      p -> expand_chunk(st -> page_size());
      p -> reset();
      p -> clean_all();
      p -> pageid = new_page_num;
      p -> f_store = st;
   }

   p -> dirty = false;
   f_replace_policy.promote(*p);

/////////////////////////
// read in the content
/////////////////////////

   page_rep* data = new page_rep(new_page_num, p);
  
//cerr << "Inserting " << new_page_num << " for " << st -> my_name() << " to page index.";
//cerr << (void*)&(st->f_local_pcache.f_page_pool_index);

   st->f_local_pcache.f_page_pool_index.insert(data);

   int offset = (new_page_num-1) * (st->page_sz);

   if ( offset < 
	(((unixf_storage*)(st->storage_ptr))->bytes()-page_storage::abs_off)) 
   {
  

//cerr <<  " swap in the page";
//debug(cerr, offset);
//debug(cerr, int(p -> page_base()));

   ((unixf_storage*)(st->storage_ptr)) ->
        readString( page_storage::abs_off + mmdb_pos_t(offset),
                    p -> page_base(),
                    st->page_sz
                  );

#ifdef PORTABLE_DB
             p -> _swap_order(true); // swap count field first
#endif
   }
//cerr << "\n";

   (st->f_local_pcache).f_current_page_num = new_page_num;
   (st->f_local_pcache).f_current_page_ptr = p;

   return p;
}

void remove_from_global_cache(const void* x)
{
   page_rep* y = (page_rep*)x;
   lru_page* p = y -> f_page_ptr;
   page_storage* st = p -> f_store;
//cerr << "removing "  << y -> f_page_num << "\n";
#ifndef C_API
   ((st -> f_global_pcache).f_replace_policy).remove(*p);
#else
   (st -> f_global_pcache_ptr -> f_replace_policy).remove(*p);
#endif
   delete p;
}

void page_cache_global_part::remove_pages(page_storage* st)
{
//cerr << "remove_pages(): for "  << st -> my_name() << "\n";
   (st -> f_local_pcache).f_page_pool_index.apply(remove_from_global_cache);

/*
   lru_pagePtr* temps = new lru_pagePtr[f_replace_policy.active_elmts()];
   int ind = f_replace_policy.first();

   int i=0;
   while ( ind != 0  ) {
      lru_page* p = (lru_page*)f_replace_policy(ind, ACTIVE);
      if ( p -> f_store == st )
        temps[i++] = p;
      f_replace_policy.next(ind, ACTIVE);
   }

   for ( int j=0; j<i; j++ )
     f_replace_policy.remove(*temps[j]);
   delete temps;
*/
}


ostream& page_cache_global_part::print_cached_pages(ostream& s)
{
   MESSAGE(s, "cached pages:");
   long ind = f_replace_policy.first();
   while ( ind != 0  ) {
      lru_page* p = (lru_page*)f_replace_policy(ind, ACTIVE);
      debug(s, long(p));

      if ( p == 0 )
         throw(stringException("corrupted store"));

      debug(s, *p);
      f_replace_policy.next(ind, ACTIVE);
      debug(s, ind);
   }
   MESSAGE(s, "print cached pages done");
   return s;
}

