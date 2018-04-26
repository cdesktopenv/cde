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
 * $XConsortium: disk_bucket.cc /main/4 1996/06/11 17:16:00 cde-hal $
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



#include "diskhash/disk_bucket.h"
#include "api/transaction.h"

//#define BKT_PARAMS_SIZE  (sizeof(char) + sizeof(v_k) + sizeof(v_r))
#define BKT_PARAMS_SIZE  (sizeof(char))

//static buffer buf(LBUFSIZ);

extern transaction* g_transac;

disk_bucket::disk_bucket(int bnum, page_storage* store) : 
v_bucket_num(bnum), v_key_store(store), v_overflowed(false),
buf(store -> aux_buf())
{
   buf.set_swap_order(store -> swap_order());
   init_params();
}

disk_bucket::~disk_bucket()
{
}

page* disk_bucket::bucket_page()
{
   if ( g_transac ) {
      g_transac -> book(v_key_store);
   }
   return (*v_key_store)(v_bucket_num+2, page_storage::READ);
}

void disk_bucket::init_params()
{
   page* p = bucket_page();
   buf.reset();

   if ( p -> count() > 1 ) {
      p -> get(1, buf);
      char c; buf.get(c); v_overflowed = c;
      //buf.get(v_k);
      //buf.get(v_r);
   } else {

      v_key_store -> save_to_log(p);

      int slot_num; char* z;
      p -> alloc_slot(slot_num, BKT_PARAMS_SIZE, z);
      if ( slot_num != 1 ) {
         debug(cerr, v_bucket_num+2);
         debug(cerr, slot_num);
         throw(stringException("corrupted disk bucket"));
      } 
      char c = v_overflowed; buf.put(c);
      p -> update_slot(1, buf);


/*
      MESSAGE(cerr, "bucket OK");
      debug(cerr, int(p));
      debug(cerr, v_bucket_num+2);
*/
   }
}

void disk_bucket::sync_params()
{
   page* p = bucket_page();

   v_key_store -> save_to_log(p);

   buf.reset();

   char c = v_overflowed; buf.put(c);
   //buf.put(v_k);
   //buf.put(v_r);

   p -> update_slot(1, buf);
}

Boolean disk_bucket::member(data_t& v, int& slot_num)
{
//debug(cerr, v);
   data_t *x = 0;

////////////////////////////////////
// use the hint, if slot_num != 0 
////////////////////////////////////

//debug(cerr, bucket_page() -> count());
//debug(cerr, slot_num);

   if ( slot_num != 0 && count() >= slot_num ) {

//MESSAGE(cerr, "probe the slot");
      x = (*this)(slot_num);

      if ( x && *x == v ) {

         v.dt = x -> dt;
         delete x;
         return true;
      }
      delete x;

   }

/////////////////////////
// do a linear search 
/////////////////////////
   int ind = first();
//debug(cerr, ind);
   while ( ind && ind != slot_num ) {
//MESSAGE(cerr, "linearly search the bucket");
      x = (*this)(ind);
//debug(cerr, *x);
      if ( x && *x == v ) {
         v.dt = x -> dt;
         delete x;
         slot_num = ind;
         return true;
      }
      delete x;
      next(ind);
   }
   return false;
}

int disk_bucket::insert(data_t* data)
{
//MESSAGE(cerr, "disk_bucket::insert()");
   buf.reset();
   data -> binaryOut(buf);
//debug(cerr, buf.content_sz());

   int slot_num = 0;
   page* p = bucket_page();

   v_key_store -> save_to_log(p);

   if (  p -> free_bytes() > buf.content_sz() ) {
      p -> put( slot_num, buf );
   } else {
      set_overflow(true); 
      sync_params();
   }

   return slot_num; 
}

Boolean disk_bucket::remove(int ind)
{
   page* p = bucket_page();
   v_key_store -> save_to_log(p);

   p -> del_slot(ind);

   return true;
}

void disk_bucket::remove_all()
{
   bucket_page() -> clean_all();
   init_params();
}

/*
int disk_bucket::first()
{
   int ind = 0;
   next(ind);
   return ind;
}
*/

data_t* disk_bucket::operator()(int ind)
{
   buf.reset();

   if ( bucket_page() -> get( ind+1, buf ) == false ) return 0;

   data_t *x = new data_t;
   x-> binaryIn(buf);

   return x;
}

/*
void disk_bucket::next(int& ind)
{
   ind = ( ind >= count() ) ? 0 : ind+1;
}

int disk_bucket::count()
{
   return MAX(0, bucket_page() -> count() - 2);
}

Boolean disk_bucket::empty()
{
   return ( count() == 0 ) ? true : false;
}
*/

ostream& disk_bucket::asciiOut(ostream& out, print_func_ptr_t print_f)
{
  int ind = first();
   while ( ind != 0 ) {

      data_t* x = (*this)(ind);

      if ( x )
        x -> asciiOut(out, print_f);

      delete x;

      next(ind);
   }
   return out;
}

ostream& operator<<(ostream& out, disk_bucket& bt)
{
//MESSAGE(cerr, "in disk_bucket::<<");
//debug(cerr, bt.bucket_page() -> count());

   int ind = bt.first();
   while ( ind != 0 ) {

      data_t* x = bt(ind);

      if ( x )
         out << *x << "\n";

      delete x;

      bt.next(ind);
   }
   return out;
}
