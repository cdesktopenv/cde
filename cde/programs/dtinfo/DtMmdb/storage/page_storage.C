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
 * $XConsortium: page_storage.C /main/8 1996/10/04 10:49:57 drk $
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


#include "storage/page.h"
#include "storage/page_storage.h"
#include "storage/version.h"
#include "utility/db_version.h"

#define db_type "MMDB"
#define db_type_sz strlen(db_type)

#define db_order_sz sizeof(char)

int page_storage::dv_sz = 0;
int page_storage::abs_off = 0;

#ifndef C_API
page_cache_global_part page_storage::f_global_pcache;
#else
page_cache_global_part* page_storage::f_global_pcache_ptr = 0;
#endif


static char* db_version = 0;
static char* data_version = 0;

str_index_record_t::str_index_record_t(int offset, mmdb_pos_t lc) 
: str_offset(offset), loc(lc)
{
}

void delete_str_index_record(const void* t)
{
   delete ((str_index_record_t*)t);
}

extern void_ptr_array g_store_array;
extern Boolean g_transaction_on;

//////////////////////////////
//
//////////////////////////////
store_trans::store_trans(char* p, char* n, int psz) : 
path(p), name(n), status(DISABLED), 
log_store(0), log_index(0), max_pages(0), page_sz(psz)
{
}

store_trans::~store_trans()
{
/*
MESSAGE(cerr, "dstr store_trans object");
debug(cerr, status);
debug(cerr, DISABLED);
debug(cerr, int(log_store));
*/

   if ( status == DISABLED && log_store ) {
      delete log_store;
      del_file(form("%s.log", name), path);
   }
   delete log_index;
}

void store_trans::init(rep_policy* policy)
{
   if ( log_store == 0 ) {
      log_store = new unixf_storage(path, form("%s.log", name), policy);

      if ( ! (*log_store) )
         throw(streamException(log_store -> rdstate()));
   }

   if ( log_index == 0 ) {
      log_index = new imp_die;
   }
}

void store_trans::quit()
{
   if ( log_store )
      log_store -> truncate(0);

   if ( log_index )
      log_index -> clean();
}

///////////////////////////////////
// Constructor 
///////////////////////////////////
page_storage::page_storage( char* _path, char* _name,
                            unixf_storage* store, int pg_sz, 
                            int/*no_pages*/, mmdb_byte_order_t create_order) :  
	abs_storage(_path, _name, PAGE_STORAGE_CODE),
	trans_info(page_storage::path, page_storage::name, pg_sz),
	page_sz(pg_sz), 
	f_local_pcache(30),
	v_db_order(create_order), v_buf(0),
	pagings(0),
	total_page_access(0)
{

//debug(cerr, my_name());
//debug(cerr, (void*)this);
//#ifdef C_API
   if ( dv_sz == 0 ) {
      dv_sz = db_type_sz + mm_version::version_bytes() + db_order_sz;
      abs_off = dv_sz;
   }
//#endif


   storage_ptr = store;

   int bts = ((unixf_storage*)storage_ptr) -> bytes();

   v_server_order = byte_order();
//debug(cerr, v_server_order);

   if ( bts == 0 ) {
//////////////////////
// empty store file
//////////////////////

       if ( db_version == 0 ) {
          db_version = new char[dv_sz];

          memcpy(db_version, db_type, db_type_sz);

          mm_version x(MAJOR, MINOR);

          x.to_byte_string(db_version+db_type_sz, 
                           (v_db_order != v_server_order) ? true : false
                          );

          char z = v_db_order;
          memcpy(db_version+db_type_sz+mm_version::version_bytes(), 
                 &z, db_order_sz);
       } 

       storage_ptr -> updateString( 0, db_version, dv_sz, 0, true);

       v_swap_order = (v_db_order != v_server_order) ? true : false;

   } else {
///////////////////
// store exists.
///////////////////

      bts -= abs_off;

      if ( data_version == 0 ) {
          data_version = new char[dv_sz];
      }

      storage_ptr -> readString(0, data_version, dv_sz);

/////////////////////////////////
// "MMDB" magic number testing
/////////////////////////////////
      if ( memcmp(data_version, db_type, db_type_sz) != 0 ) {
         throw(stringException(
            form("bad magic number. corrupted store %s/%s", _path, _name)
                              )
              );
      }

///////////////////////////////
// get db byte order
///////////////////////////////
      char z;
      memcpy(&z, 
             data_version + db_type_sz + mm_version::version_bytes(), 
             db_order_sz
            );
      
      v_db_order = int(z);

      if ( v_db_order != mmdb_big_endian && v_db_order != mmdb_little_endian ) {
         debug(cerr, v_db_order);
         throw(stringException(
              "data is in neither big endian nor little endian format"
                              ));
      }


///////////////////////////////
// test version
///////////////////////////////
      v_swap_order = (v_db_order != v_server_order) ? true : false;

      mm_version dv(data_version+db_type_sz, v_swap_order);

      f_version = dv;

      mm_version cv(MAJOR, MINOR);

      if ( cv < dv ) {
         cerr << "code major = " << cv.major_version() << "\n";
         cerr << "code minor = " << cv.minor_version() << "\n";
         cerr << "data major = " << dv.major_version() << "\n";
         cerr << "data minor = " << dv.minor_version() << "\n";
         throw(stringException(
            form("data and code version mismatch on store %s/%s", _path, _name)
                              )
              );
      }

   }

//debug(cerr, v_db_order);


   total_pages = bts / page_sz;

   f_local_pcache.init_heap(this);

   set_mode(HEALTH, true);
}

/***********************************************************/
// Destructor 
/***********************************************************/
page_storage::~page_storage()
{
//MESSAGE(cerr, "dstr page_storage");
//debug(cerr, my_name());

   delete v_buf;
   delete storage_ptr;

   delete data_version;
   data_version = 0;

   delete db_version;
   db_version = 0;

   f_local_pcache.quit_heap(this);

   f_global_pcache.remove_pages(this);

/*
MESSAGE(cerr, my_name());
debug(cerr, total_page_access);
debug(cerr, pagings);
debug(cerr, float(pagings)/total_page_access);
*/
}

void page_storage::remove()
{
   storage_ptr -> remove();
}

void page_storage::sync()
{

/*
MESSAGE(cerr, "page_storage::sync()");
debug(cerr, my_path());
debug(cerr, my_name());
debug(cerr, pages());
debug(cerr, int(this));
*/


  long ind = f_global_pcache.f_replace_policy.first();

  while ( ind != 0 ) {
     lru_page *p= (lru_page*)(f_global_pcache.f_replace_policy)(ind, ACTIVE);
     if  ( p -> f_store == this ) 
        sync( p );
     f_global_pcache.f_replace_policy.next(ind);
   }

//MESSAGE(cerr, "page_storage::sync() done");
}

void page_storage::sync( int page_num )
{
   sync((*this)(page_num, READ));
}

void page_storage::sync( page* p )
{
   if ( p == 0 || p -> page_id() <= 0 ) {
      throw(stringException("null page pointer"));
   }

   if ( p -> dirty == true ) {


//cerr << "dirty PAGE swapped out " << p -> page_id();
//cerr << " " << my_name() << "\n";

      int offset = ( p -> page_id() - 1 ) * page_sz ;

#ifdef PORTABLE_DB
      p -> _swap_order(false);
#endif

//debug(cerr, *p);

#ifdef DEBUG
      fprintf(stderr, "purging page at %p size=%d into %s/%s @ %s:%d\n",
			p -> page_base(), page_sz,
			storage_ptr->my_path(), storage_ptr->my_name(),
			__FILE__, __LINE__);
#endif
      storage_ptr -> updateString( abs_off + mmdb_pos_t(offset),
                                   p -> page_base(),
                                   page_sz, 0, true
                                 );
#ifdef PORTABLE_DB
//////////////////////////////////////////////////////////////
// flip back to original. as the server will not quit after
// the sync.   
//////////////////////////////////////////////////////////////
      p -> _swap_order(true); 
#endif

      p -> dirty = false;
   }
}

//static Boolean xflag = false;
/***********************************************************/
// readString(). 
/***********************************************************/
int 
page_storage::readString(mmdb_pos_t loc, char* base, int len, int str_offset)
{
/*
int xstring_ofst = str_offset;
int xloc = loc;
int xlen = len;

MESSAGE(cerr, "page_storage: readString");
debug(cerr, my_path());
debug(cerr, my_name());
debug(cerr, loc);
debug(cerr, int(base));
debug(cerr, len);
debug(cerr, str_offset);
*/

   buffer in_cache(0);
   in_cache.set_chunk(base, len);

   spointer_t *slot_info;

   while ( len > 0 ) {

      if ( loc == 0 ) {
         throw(stringException("damaged store."));
      }

      int page_num  = PAGE_ID( loc, page_sz );
      int page_slot  = PAGE_IDX( loc, page_sz );

//debug(cerr, page_num);
//debug(cerr, page_slot);
 
      page *y = (*this)(page_num, READ);

      slot_info = y -> get_spointer(page_slot);

      int str_leng = slot_info -> string_leng();
      loc  = slot_info -> forward_ptr();
/*
MESSAGE(cerr, "slot info:");
debug(cerr, str_leng);
debug(cerr, loc);
debug(cerr, slot_info -> string_ofst());
*/

      delete slot_info;

      if ( str_offset >= str_leng ) {

          str_offset -= str_leng;

      } else {
          int bytes_read = MIN(len, str_leng - str_offset);

/*
debug(cerr, len);
debug(cerr, bytes_read);
*/

          y -> get( page_slot, in_cache, str_offset, bytes_read );

          len -= bytes_read;

          str_offset = 0;
      }
   }

//MESSAGE(cerr, "page_storage:: readString done");

   return 0;
}

int 
page_storage::get_str_ptr(mmdb_pos_t loc, char*& str, 
                          int& len) 
{
   int page_num  = PAGE_ID( loc, page_sz );
   int page_slot  = PAGE_IDX( loc, page_sz );


//MESSAGE(cerr, "IN get_str_ptr()");
//debug(cerr, my_path());
//debug(cerr, my_name());
//debug(cerr, page_num);
//debug(cerr, page_slot);


   page *y = (*this)(page_num, READ);

   spointer_t* slot_info =  y -> get_spointer(page_slot);

//debug(cerr, int(slot_info -> get_mode(spointer_t::DELETED)));
/*
if ( page_num == 1 && page_slot == 1 ) {
debug(cerr, hex(slot_info -> header.int_view));
debug(cerr, hex(slot_info -> header.bit_view.spointer));
debug(cerr, hex(slot_info -> header.bit_view.length));
debug(cerr, hex(slot_info -> header.bit_view.deleted));
debug(cerr, hex(slot_info -> header.bit_view.first_recd));
debug(cerr, hex(slot_info -> header.bit_view.updated));
debug(cerr, hex(slot_info -> header.bit_view.is_object));
}
*/


   if ( slot_info -> get_mode(spointer_t::DELETED) == true ) {
      delete slot_info;
      throw(stringException("read deleted byte string"));
   }

   delete slot_info;
   y -> get_str_ptr(page_slot, str, len);

   return 0;
}

int page_storage::insertString(mmdb_pos_t& loc, const char* base, int len, Boolean flush_opt)
{
/*
cerr << "insert: len="  << len << "\n";
int u3 = len;
*/


#ifdef STORAGE_DEBUG
MESSAGE(cerr, "page_storage insertString");
debug(cerr, len);
#endif

   buffer temp_buf(0); 
   int page_num = 0;
   int page_oft = 0;

   int str_offset = len;
   int sec_len;

   Boolean first_recd = true;
   Boolean new_page;

   while ( str_offset > 0 ) {

      loc = (page_num != 0 ) ? 
             FORM_PAGE_REF(page_num, page_oft, page_sz) :
             0;

      fbytes_t* v = f_local_pcache.find_host_page(this, new_page) ;
      page_num = v -> page_num;

      page* y = (*this)(page_num, WRITE);

      sec_len = MIN(len, y -> free_bytes());

      str_offset -= sec_len;


      temp_buf.set_chunk((char*)(base + str_offset), sec_len);
      temp_buf.set_content_sz(sec_len);

      y -> put( page_oft, temp_buf );

      spointer_t *x = y -> get_spointer(page_oft);
      x -> set_mode(spointer_t::FIRST_RECD, first_recd);
      x -> set_forward_ptr(loc);
      delete x;

      v -> free_bytes = y -> free_bytes();
      f_local_pcache.adjust_heap(v, new_page);

      if ( flush_opt == true ) sync(page_num);

      first_recd = false;
      len -= sec_len;
   }


#ifdef STORAGE_DEBUG
MESSAGE(cerr, "FINAL LOC COMPONENT");
debug(cerr, page_oft);
debug(cerr, page_sz);
debug(cerr, page_num);
#endif

   loc = FORM_PAGE_REF(page_num, page_oft, page_sz);



/*
      int u1 = PAGE_ID( loc, page_sz );
      int u2 = PAGE_IDX( loc, page_sz );
cerr << "insert:" << page_num << "." << page_oft << " " << u3 << "\n";
*/



   return 0;
}

/***********************************************************/
// updateString(). 
/***********************************************************/
int 
page_storage::updateString(mmdb_pos_t loc, const char* base, int len, 
                           int string_ofst, Boolean flush_opt)
{
/*
      int u1 = PAGE_ID( loc, page_sz );
      int u2 = PAGE_IDX( loc, page_sz );
cerr << "update:" << u1 << "." << u2 << " " << len << " " << string_ofst << "\n";
*/


/*
MESSAGE(cerr, "update string");
debug(cerr, name);
debug(cerr, loc);
*/

   int page_num =0;
   int slot_num =0;
   int offset = 0;
   page* y = 0;
   spointer_t *x = 0;

   while ( len > 0 && loc != 0 ) {

      page_num  = PAGE_ID( loc, page_sz );
      slot_num = PAGE_IDX( loc, page_sz );
//debug(cerr, page_num);
//debug(cerr, slot_num);

      y = (*this)(page_num, WRITE); 

      x = y -> get_spointer(slot_num);

/*
debug(cerr, x -> forward_ptr());
debug(cerr, x -> string_leng());
debug(cerr, x -> string_ofst());
*/

      loc = x -> forward_ptr();
      int this_len = x -> string_leng();
      delete x;

      if ( string_ofst >= this_len ) {
         string_ofst -= this_len;
         continue;
      }

      int update_len = MIN(len, this_len - string_ofst);

      buffer sbuf(0);
      sbuf.set_chunk((char*)(base+offset), update_len);
      sbuf.set_content_sz(update_len);

      y -> update_slot(slot_num, sbuf, string_ofst);

      if ( flush_opt == true ) sync(page_num);

      string_ofst = 0;
      offset += update_len;
      len -= update_len;
   }

   if ( len > 0 ) {

//debug(cerr, len);
//debug(cerr, offset);

/*****************************/
// the update is an expanding
/*****************************/
      mmdb_pos_t new_loc;
      insertString(new_loc, base+offset, len, flush_opt) ;

      y = (*this)(page_num, WRITE); 
      x = y -> get_spointer(slot_num);
      x -> set_forward_ptr(new_loc);
      delete x;

      page_num = PAGE_ID( new_loc, page_sz );
      slot_num = PAGE_IDX( new_loc, page_sz );
      y = (*this)(page_num, WRITE); 
      x = y -> get_spointer(slot_num);
      x -> set_mode(spointer_t::FIRST_RECD, false);
      delete x;

      y -> dirty = true;

      return 0;
   }

//   if ( loc != 0 ) {

/*****************************/
// the update is a shrinking 
/*****************************/
//      y = (*this)(page_num); 
//      x = y -> get_spointer(slot_num);
//      x -> set_forward_ptr(0);
//      return deleteString(loc);
//   }


   return 0;
}

/***********************************************************/
// deleteString(). 
/***********************************************************/
int page_storage::deleteString(mmdb_pos_t loc, Boolean flush_opt)
{
   while ( loc != 0 ) {
      int page_num  = PAGE_ID( loc, page_sz );
      int slot_num = PAGE_IDX( loc, page_sz );
//debug(cerr, page_num);
//debug(cerr, slot_num);

      page* y = (*this)(page_num, WRITE); 

      spointer_t *x = y -> get_spointer(slot_num);

      if ( x -> get_mode(spointer_t::DELETED) == true )
          return 0;

      loc = x -> forward_ptr();
//debug(cerr, loc);
      delete x;

      y -> del_slot(slot_num);

      if ( flush_opt == true ) sync(page_num);
   }

   return 0;
}

/***********************************************************/
// allocate a chunk on a page. 
/***********************************************************/
int 
page_storage::allocString(mmdb_pos_t& loc, int len, char*& string_ptr, int mode)
{
//MESSAGE(cerr, "AllocString:");
//debug(cerr, my_name());
//debug(cerr, len);

   if ( len > page_sz ) {
      MESSAGE(cerr, "allocString(): string too long");
      throw(boundaryException(0, page_sz, len));
   }

   Boolean new_page;

   fbytes_t* v = f_local_pcache.find_host_page(this, new_page, len) ;

   page* x = (*this)(v -> page_num, WRITE);

   int slot_num;

   x->alloc_slot(slot_num, len, string_ptr);

   spointer_t* slot_info = x -> get_spointer(slot_num);
   slot_info -> add_mode(mode);
   delete slot_info;


   v -> free_bytes = x -> free_bytes();
   f_local_pcache.adjust_heap(v, new_page);

   loc = FORM_PAGE_REF(v -> page_num, slot_num, page_sz);

#ifdef STORAGE_DEBUG
MESSAGE(cerr,  "in allocString(): final params");
debug(cerr,  int(x -> dirty));
debug(cerr,  page_num);
debug(cerr,  slot_num);
debug(cerr,  my_name());
debug(cerr,  loc);
debug(cerr,  int(string_ptr));
#endif


// int u1 = PAGE_ID( loc, page_sz );
// int u2 = PAGE_IDX( loc, page_sz );
// cerr << "Allocated ID=" << u1 << "." << u2 << " " << len << "\n";

   return 0;
}


int page_storage::appendString(mmdb_pos_t loc, const char* base, int len, Boolean)
{
/*
      int u1 = PAGE_ID( loc, page_sz );
      int u2 = PAGE_IDX( loc, page_sz );
cerr << "append:" << u1 << "." << u2 << " " << len << "\n";
*/



   int page_num = 0;
   int slot_num = 0;
   page* y;
   spointer_t *x;

   while ( loc != 0 ) {

      page_num  = PAGE_ID( loc, page_sz );
      slot_num = PAGE_IDX( loc, page_sz );

      y = (*this)(page_num, READ);
      x = y -> get_spointer(slot_num);

      loc = x -> forward_ptr();
      delete x;
   }

   insertString(loc, base, len) ;

   y = (*this)(page_num, WRITE);
   x = y -> get_spointer(slot_num);
   x -> set_forward_ptr(loc);
   delete x;

   page_num = PAGE_ID( loc, page_sz );
   slot_num = PAGE_IDX( loc, page_sz );
   y = (*this)(page_num, WRITE);
   x = y -> get_spointer(slot_num);
   x -> set_mode(spointer_t::FIRST_RECD, false);
   delete x;

   return 0;
}

/***********************************************************/
// create new page frame. 
/***********************************************************/
int page_storage::add_page_frames(int pages)
{
   total_pages += pages;

//////////////////////////////////////
// prepare the new page in the cache
//////////////////////////////////////
   for (int i=1; i<=pages; i++)
      operator()(total_pages+i-pages, WRITE);

   return 0;
}

/***********************************************************/
// return the mmdb_pos_t of the first page. 
/***********************************************************/
int page_storage::first() const
{
   return ( total_pages > 0 ) ? 1 : 0;
}

//////////////////////////////////////////
/* ind should be the page id            */
//////////////////////////////////////////
page* page_storage::operator()(int ind, enum access mode) 
{
/*
if ( mode == WRITE ) {
MESSAGE(cerr, "opera(): write mode");
debug(cerr, ind);
debug(cerr, name);
}
*/

/*
if ( mode == WRITE ) {
char* s = name + strlen(name) - 11;
debug(cerr, s);
if ( strcmp(s, "index.store") == 0 )
debug(cerr, name);
}
*/

//cerr << "Try to get page " << ind << " from " << my_name() << "\n";

   if ( ! INRANGE( ind,  1, pages() ) ) {
      debug(cerr, my_path());
      debug(cerr, my_name());
      MESSAGE(cerr, 
              form("page_storage:: operator(): mmdb_pos_t %d out of range.", ind)
             );
      throw(boundaryException(1, pages(), ind));
   } 

   page* p = f_local_pcache.in_cache(this, ind);
   
   if ( p == 0 ) {
   
//cerr << "swapping in a page " << ind << endl;
     p = f_global_pcache.load_new_page( this, ind, 
		(v_db_order==v_server_order) ? false : true
				      );
   
   }
   
/*
debug(cerr, my_name());
debug(cerr, p -> count());
debug(cerr, int(mode));
debug(cerr, int(WRITE));
*/
   if ( mode == WRITE ) {
      p -> dirty = true;
      save_to_log(p);
   }

   return p;
}

/***********************************************************/
// update ind to the mmdb_pos_t of next page.
/***********************************************************/
void page_storage::next(int& ind) const
{
   if ( ind >= pages() )
      ind = 0;
   else
      ind++;
}

io_status page_storage::asciiOut(ostream& out)  
{
   int ind = first();
   while ( ind != 0  ) {
#ifdef DEBUG
      page* p = (*this)(ind, READ);
      debug(out, *p);
#endif
      next(ind);
   }
   return done;

/*
   debug(cerr, pagings);
   pagings = 0;
*/
}

Boolean 
page_storage::seek_loc(mmdb_pos_t& loc, const direction d, int smd)
{
   if ( d == positive ) {
       return seek_loc_positive(loc, smd);
   } else {
       return seek_loc_negative(loc, smd);
   }
}

Boolean page_storage::seek_loc_positive(mmdb_pos_t& loc, int smd)
{
   int pgs = pages();

   int page_num = PAGE_ID( loc, page_sz );
   int page_slot = PAGE_IDX( loc, page_sz );

   while ( page_num <= pgs ) {

      page* x = (*this)(page_num, READ);

      if ( page_slot == 0 )
         page_slot = x -> first();
      else
         x -> next( page_slot );

      spointer_t *y;

      while ( page_slot && (y = x -> get_spointer(page_slot)) != 0 ) {
         if ( y -> get_mode(spointer_t::DELETED) == false &&
              y -> test_mode(smd) == true ) {
            loc = FORM_PAGE_REF(page_num, page_slot, page_sz);
            delete y;
            return true;
         } else {
            delete y;
            x -> next(page_slot);
         }
      }

      page_num++;
      page_slot = 0;
   }

   return false;
}

Boolean page_storage::seek_loc_negative(mmdb_pos_t& loc, int smd)
{
   int page_num = PAGE_ID( loc, page_sz );
   int page_slot = PAGE_IDX( loc, page_sz );

   while ( page_num > 0 ) {

      page* x = (*this)(page_num, READ);

      if ( page_slot == 0 )
         page_slot = x -> count() - 1;
      else
         x -> prev(page_slot);

      spointer_t *y;

      while ( page_slot && (y = x -> get_spointer(page_slot)) != 0 ) 
      {
         if ( y -> get_mode(spointer_t::DELETED) == false &&
              y -> test_mode(smd) == true ) {
            loc = FORM_PAGE_REF(page_num, page_slot, page_sz);
            delete y;
            return true;
         } else {
            delete y;
            x -> prev(page_slot);
         }
      }

      page_num--;
      page_slot = 0;
   }

   return false;
}

void page_storage::reset_paging_count()
{
/*
   if ( storage_ptr -> rdbuf() -> is_open() ) {
      cerr << storage_ptr -> my_name() << " has not been closed\n";
      storage_ptr -> close();
   } else {
      cerr << storage_ptr -> my_name() << " is already closed\n";
   }
*/
   pagings = 0;
   
}
   
int page_storage::paging_count() const
{
   return pagings;
}

mmdb_pos_t page_storage::first_loc()
{
   if ( pages() >= 1 && (*this)(1, READ) -> count() != 0 )
      return FORM_PAGE_REF(1, 1, page_sz);
   else
      return 0;
}

mmdb_pos_t page_storage::last_loc()
{
   int pgs = pages();
    
   if ( pgs == 0 ) 
      return 0;

   for ( int i=pgs; i>0; i-- ) {
      int ct = (*this)(i, READ) -> count();
      if ( ct > 0 )
         return FORM_PAGE_REF(i, ct, page_sz);
   }

   return 0;
}

int 
page_storage::get_str_locs(mmdb_pos_t str_loc, 
                           str_index_record_tPtr*& vector, int& vector_len
                          )
{
   int vector_sz = 20;
   vector = new str_index_record_tPtr[vector_sz];

   int num_pieces = 0;
   int str_offset = 0;
   spointer_t *x = 0;

   while ( str_loc > 0 ) {

      if ( num_pieces >= vector_sz ) {
         vector_sz *= 2;
         vector = (str_index_record_tPtr*)realloc(
                        (char*)vector, 
                        vector_sz*sizeof(str_index_record_tPtr)
                  );
      }

      vector[num_pieces++] = 
          new str_index_record_t(str_offset, str_loc);

      int page_num  = PAGE_ID( str_loc, page_sz );
      int page_slot  = PAGE_IDX( str_loc, page_sz );

      page *y = (*this)(page_num, READ);

      x = y -> get_spointer(page_slot);

      str_offset += x -> string_leng();
      str_loc  = x -> forward_ptr();

      delete x;
   }

   vector_len = num_pieces;
   return 0;
}

int page_storage::set_page_dirty(mmdb_pos_t loc)
{
MESSAGE(cerr, "set page dirty: page loc is");
debug(cerr, loc);

   page* x = (*this)(PAGE_ID( loc, page_sz ), WRITE);
   x -> dirty = true;
   return 0;
}

Boolean page_storage::io_mode(int mode)
{
   if ( storage_ptr )
      return storage_ptr -> io_mode(mode);
   else
      return false;
}

void page_storage::begin_trans()
{
   mtry {
      if ( trans_info.status == store_trans::ENABLED )
         throw(stringException("trans is still in progress"));
   
      trans_info.init(policy);
      trans_info.set_max_pages(total_pages);
   
      trans_info.status = store_trans::ENABLED;
   
      int l_max_pages = trans_info.max_pages;
      if ( swap_order() == true )
          ORDER_SWAP_UINT(l_max_pages);
   
      trans_info.log_store -> 
        updateString(0, (char*)&l_max_pages, sizeof(l_max_pages), 0, true);
  }

  mcatch (mmdbException&, e) {
// workaround for solaris's /SUNWspro/bin/CC compiler.
     beginTransException x;
     throw(x);
     //throw(beginTransException());
  }
  end_try;
}

void page_storage::commit_trans()
{
//////////////////////////////
// sync the touched pages
//////////////////////////////
   mtry {
      int ind = trans_info.log_index -> first_bucket();
   
      while ( ind != -1 ) {
   
         imp_bucket* bucket = trans_info.log_index -> get_bucket(ind);
   
         if ( bucket ) {
             long b_ind = bucket -> first();
             while ( b_ind != 0 ) {
                data_t* z = (*bucket)(b_ind);
   //debug(cerr, ((page*)(z -> dt)) -> count());
                sync((page*)(z -> dt));
                bucket -> next(b_ind);
             }
         }
         trans_info.log_index -> next_bucket(ind);
      }
   
      trans_info.quit();
   
      trans_info.status = store_trans::DISABLED;
   }

   mcatch (mmdbException &,e) {
// workaround for solaris's /SUNWspro/bin/CC compiler.
      commitTransException x;
      throw(x);
   } end_try;
}

void page_storage::roll_back()
{
   mtry
   {
      if ( exist_file(form("%s.log", name), path) == false ) 
         return;
   
   MESSAGE(cerr, "roll_back() begins");
   
      trans_info.init(policy); // init the log store
                 
   //////////////////////////////////
   // verify the log is in good shape
   //////////////////////////////////
      int m;
      unsigned int log_bytes = trans_info.log_store -> bytes();
   
      if ( trans_info.log_store && 
           (*trans_info.log_store) &&
           log_bytes > sizeof(m) ) 
      {
   
   //////////////////////////////////
   // truncate the store to previous
   // length
   //////////////////////////////////
         trans_info.log_store -> readString(0, (char*)&m, sizeof(m));
   
         if ( swap_order() == true )
            ORDER_SWAP_UINT(m);
   
         trans_info.set_max_pages(m); // init the log store
   
         ((unixf_storage*)storage_ptr) -> truncate(abs_off + m*page_sz); 
   
   //debug(cerr, m);
   //////////////////////////////////
   // restore the store to previous
   // state
   //////////////////////////////////
         int l_pid = 0;
   
         if ( (log_bytes - sizeof(m)) % (page_sz+sizeof(l_pid)) != 0 )
            throw(stringException("corrupted transaction log"));
   
         int u = (log_bytes - sizeof(m)) / (page_sz+sizeof(l_pid));
   //debug(cerr, u);
   
   
         buffer log_buf(page_sz);
         for ( int i=0; i<u; i++ ) {
   
             int offset = sizeof(m) + i*(page_sz + sizeof(l_pid));
   
             trans_info.log_store -> 
                   readString(offset,                        // page id
                              (char*)&l_pid, sizeof(l_pid)
                             );
   
             if ( swap_order() == true ) // swap byte order if necessary
                   ORDER_SWAP_UINT(l_pid);
   
             trans_info.log_store -> 
                   readString(offset + sizeof(l_pid),        // page content
                              log_buf.get_base(),
                              page_sz
                             );
   
   
             storage_ptr -> updateString( abs_off + (l_pid-1)*page_sz, 
                                          log_buf.get_base(),
                                          page_sz,
                                          0, 
                                          true
                                        );
         }
      }
   
   ///////////////////////////////////////////////
   // make sure the cached pages are not synced
   ///////////////////////////////////////////////
   
      long ind = f_global_pcache.f_replace_policy.first();
   
      while ( ind != 0 ) {
        lru_page *p = (lru_page*)
		(f_global_pcache.f_replace_policy)(ind, ACTIVE);

        if ( p -> f_store == this )
           p -> dirty = false;
        f_global_pcache.f_replace_policy.next(ind);
      }
   
      trans_info.quit(); // remove the log store
   
      trans_info.status = store_trans::DISABLED;
  }

  mcatch (mmdbException &,e)
  {
// workaround for solaris's /SUNWspro/bin/CC compiler.
      rollbackTransException x;
      throw(x);
  } end_try;

MESSAGE(cerr, "roll_back() completes");
}

void page_storage::save_to_log(page* p)
{
/*
MESSAGE(cerr, "About to save to log");
debug(cerr, my_name());
debug(cerr, int(trans_info.status));
debug(cerr, int(store_trans::ENABLED));
debug(cerr, trans_info.max_pages);
*/

   if ( trans_info.status == store_trans::ENABLED &&
        INRANGE(p -> page_id(), 1, trans_info.max_pages)
      ) 
   {
      //assert ( trans_info.log_store );
      //assert ( trans_info.log_index );

      if ( trans_info.log_store  == 0 || trans_info.log_index == 0 )
         throw(stringException("corrupted store"));
   
      int l_pid = p -> page_id();
      data_t pkey(l_pid, voidPtr(p));

   
      if ( trans_info.log_index -> member(pkey) == false ) {
//MESSAGE(cerr, form("Save_to_log pid=%d, pcnt = %d, name=%s", 
//        l_pid, p -> count(), name));

         int log_bytes_before = trans_info.log_store -> bytes();
   
         mtry {
            if ( swap_order() == true ) // swap to desired order
               ORDER_SWAP_UINT(l_pid);

            trans_info.log_store -> 
               appendString( 0, (char*)&l_pid, sizeof(l_pid), false );

            if ( swap_order() == true ) // swap back
               ORDER_SWAP_UINT(l_pid);
             
            p -> _swap_order(false); 

            trans_info.log_store -> 
               appendString( 0, p -> page_base(), page_sz, true );

//debug(cerr,  trans_info.log_store -> bytes());
            p -> _swap_order(true); 
   
            trans_info.log_index -> insert(pkey);
         }

         mcatch (mmdbException&, e) {
            trans_info.log_store -> truncate(log_bytes_before);
            rethrow;
         }
         end_try;

      } 
//    else
//MESSAGE(cerr, form("Not save_to_log pid=%d, name=%s", l_pid, name));

   }
}

buffer& page_storage::aux_buf() 
{ 
   if ( v_buf == 0 )
      v_buf = new buffer(LBUFSIZ);
   return *v_buf; 
}

