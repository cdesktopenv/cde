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
 * $XConsortium: store_desc.cc /main/5 1996/07/18 14:50:53 drk $
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



#include "utility/config.h"
#include "schema/store_desc.h"
#include "api/transaction.h"

extern transaction* g_transac;

#ifdef C_API
lru* store_desc::v_unixf_pool_ptr;
#else
lru store_desc::v_unixf_pool(ACTIVE_UNIXF_SZ, INACTIVE_UNIXF_SZ, false);
#endif

store_desc::store_desc(const char* name) : desc(name)
{
   mode_str = strdup("");
   order_str = 0;

#ifdef MMDB_BIG_ENDIAN
   order_str = strdup("big_endian");
#endif

#ifdef MMDB_LITTLE_ENDIAN
   order_str = strdup("little_endian");
#endif

   if ( order_str == 0 )
      throw(stringException("machine type not supported"));
}

store_desc::store_desc(int tp, const char* comment): 
	desc(tp, comment)
{
   mode_str = strdup("");

   order_str = 0;

#ifdef MMDB_BIG_ENDIAN
   order_str = strdup("big_endian");
#endif

#ifdef MMDB_LITTLE_ENDIAN
   order_str = strdup("little_endian");
#endif

   if ( order_str == 0 )
      throw(stringException("machine type not supported"));
}

store_desc::~store_desc()
{ 
   delete mode_str; 
   delete order_str; 

/*
   store_desc_obj_ct-- ;

   if ( store_desc_obj_ct == 0 ) 
     delete v_unixf_pool;
*/
}

void store_desc::set_mode(const char* str)
{
   delete mode_str;
   mode_str = strdup(str);

   get_mode(); // check 
}

int store_desc::get_mode()
{
   int mode = 0;
   for ( unsigned int i=0; i<strlen(mode_str); i++ ) {
      switch ( mode_str[i] ) {
         case 'r':
           mode |= ios::in;
           break;
         
         case 'w':
           mode |= ios::out;
           break;

         default:
           throw(stringException("unknown mode char"));
      } 
   }
   return mode;
}

void store_desc::set_order(const char* str)
{
   delete order_str;
   order_str = strdup(str);

   get_order(); // check 
}

mmdb_byte_order_t store_desc::get_order()
{
   if ( strcmp(order_str, "big_endian") == 0 ) 
      return mmdb_big_endian;
   else
   if ( strcmp(order_str, "little_endian") == 0 ) 
      return mmdb_little_endian;
   else 
      throw(stringException("machin type not supported"));
   

   return mmdb_big_endian; 
}

ostream& store_desc::asciiOut(ostream& out, Boolean last)
{
   desc::asciiOut(out, false);
   char* mode = mode_str; desc_print(out, mode);

   char* byte_order = order_str;
   if ( last == true )
      desc_print_end(out, byte_order);
   else
      desc_print(out, byte_order);

   if ( ! out )
     throw(stringException("store_desc::asciiOut() failed"));

   return out;
}

page_store_desc::page_store_desc() : 
	store_desc(PAGE_STORAGE_CODE, "store	page_store"),
	page_sz(0), cached_pages(0)
{
}

void page_store_desc::set_page_sz(int x)
{
   page_sz = x;
}

void page_store_desc::set_cached_pages(int x)
{
   cached_pages = x;
}

ostream& page_store_desc::asciiOut(ostream& out, Boolean last)
{
   store_desc::asciiOut(out, false);
   desc_print(out, cached_pages);

   if ( last == true )
      desc_print_end(out, page_sz);
   else
      desc_print(out, page_sz);

   if ( ! out )
     throw(stringException("page_store_desc::asciiOut() failed"));

   return out;
}

abs_storage* page_store_desc::init_store(char* db_path)
{
   unixf_storage* unixf =
             new unixf_storage(
                               (char*)db_path,
                               (char*)get_nm(),
#ifdef C_API
                               v_unixf_pool_ptr,
#else
                               &v_unixf_pool,
#endif
                               get_mode()
                              );

   //cached_pages = MIN(cached_pages, 10);

   v_store_ptr =
           new page_storage((char*)db_path, (char*)get_nm(),
                            unixf, page_sz, cached_pages, get_order());


   if ( g_transac )
      g_transac -> book(v_store_ptr);

   return v_store_ptr;
}

void page_store_desc::sync_store()
{
   if ( v_store_ptr )
      ((page_storage*)v_store_ptr) -> sync();
}

void page_store_desc::quit_store()
{
   delete v_store_ptr;
}

