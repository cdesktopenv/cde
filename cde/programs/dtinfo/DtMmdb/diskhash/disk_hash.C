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
 * $XConsortium: disk_hash.cc /main/5 1996/07/18 14:28:19 drk $
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


#include "diskhash/disk_hash.h"
#include "api/transaction.h"

extern transaction* g_transac;

int dsteps[] = { 2, 3, 5, 7, 11, 13, 17, 21, 23, 29, 31 };
int no_dsteps;

#define KPB 	20	// keys per bucket
#define REHASHS  5 	// number of rehashs in a row with 
 			// fixed bucket size
#define TOP_LEVEL_PARAMS (sizeof(k) + sizeof(p) + sizeof(M) + \
                          sizeof(n) + sizeof(v))

//static buffer buf(LBUFSIZ);

disk_hash::disk_hash(page_storage* store, int prime, int expected_n) : 
index_agent(), key_store(store), buf(store -> aux_buf())
{
   if ( g_transac ) {
      g_transac -> book(store);
   }

   buf.set_swap_order(store -> swap_order());

   rand_generator.seed();
   no_dsteps = sizeof(dsteps) / sizeof(int);

   init_params(prime, expected_n);

   bucket_vector = new bucket_array(M+v, store);
   hash_vector = new void_ptr_array(2*MAX(expected_n, (int) n));

   k_vector = new void_ptr_array(M+v);
   r_vector = new void_ptr_array(M+v);

   k_vector -> reset_vptr(voidPtr(1));

}

disk_hash::~disk_hash()
{
   delete bucket_vector; 
   delete hash_vector;

   delete k_vector;
   delete r_vector;
}

void disk_hash::init_params(int prime, int expected_n)
{
   int pgs = key_store -> pages();

   if ( pgs > 0 ) {
      if ( (*key_store)(1, page_storage::READ) -> count() != 2 ) {
         throw(stringException("corruptted primary bucket"));
      }

/////////////////////////////////
// read in params from the store
/////////////////////////////////
      buf.reset();

      (*key_store)(1, page_storage::READ) -> get(1, buf);

      buf.get(k).get(p).get(M).get(n).get(v);
      
    } else {
///////////////////////////////////////
// init the store. bucket 1 is reserved
// for top level function params
///////////////////////////////////////
      set_p(prime);
      set_k(rand_generator.rand());
      set_M(MAX(1, expected_n/KPB));
      set_v(MAX(1, M/4));
      set_n(0);

      key_store -> add_page_frames(1);

      int slot_num; char* z;
      (*key_store)(1, page_storage::WRITE) -> 
         alloc_slot(slot_num, TOP_LEVEL_PARAMS, z);

      if ( slot_num != 1 ) {
         throw(stringException("corruptted primary bucket"));
      }

      sync_params();
   }
}

void disk_hash::sync_params()
{
   buf.reset();
   buf.put(k).put(p).put(M).put(n).put(v);
   (*key_store)(1, page_storage::WRITE) -> update_slot(1, buf);
}

void disk_hash::clean()
{
   throw(stringException("void disk_hash::clean(): not implemented yet"));
}

///////////////////////////////////////////////////////////////
// rehash all keys
///////////////////////////////////////////////////////////////
Boolean disk_hash::rehash(data_t& w)
{
//MESSAGE(cerr, "REHASH:");
   char tmp_name[PATHSIZ];
   snprintf(tmp_name, sizeof(tmp_name), "%s.tmp", key_store -> my_name());

   fstream pool(form("%s/%s", key_store -> my_path(), tmp_name),
                ios::in | ios::out
               );

   for ( int i=0; i<bucket_vector -> count(); i++ ) {
      pool << bucket_vector -> get_bucket(i);
   }
   pool << w;


   Boolean ok = false;

   for ( int rehashs=0; rehashs<REHASHS; rehashs++ ) {

/////////////////////
// adjust params
/////////////////////
      int old_M = M;
      int old_v = v;

      set_M(MAX(n/KPB, 2*M));
      set_v(M/4);

      int delta = M + v - old_M - old_v;

      k_vector -> expandWith(delta);
      r_vector -> expandWith(delta);

//////////////////////////////////////////
// expand the buckets and the hash table
//////////////////////////////////////////
      bucket_vector -> expandWith(MAX(0, M+v - bucket_vector -> count()));
      hash_vector -> expandWith(MAX(0, 2*n - hash_vector -> count()));
      
/////////////////////
// clean buckets
/////////////////////
      for ( int i=0; i<bucket_vector -> count(); i++ ) {
         bucket_vector -> get_bucket(i).remove_all();
      }

      if ( _rehash(pool) == true ) {
         ok = true;
         break;
      }
   }

   if ( ok == false ) 
      throw(stringException("rehash() failed"));

   del_file(tmp_name, key_store -> my_path());

   return true;
}

Boolean disk_hash::_rehash(fstream& pool)
{
   pool.clear(); pool.seekg(0, ios::beg);

   hash_vector -> reset_vptr(voidPtr(0));
   k_vector -> reset_vptr(voidPtr(1));
   r_vector -> reset_vptr(voidPtr(0));

   set_k(rand_generator.rand());

   data_t x((char*)0, 0, 0);

   while ( pool >> x ) {
      if ( _insert(x, false) == false )
         return false;
   }

   return true;
}

/************************************/
// insert 
/************************************/
Boolean disk_hash::insert(data_t& w)
{
   if ( _insert(w, true) == false )
      throw(stringException("disk_hash::insert() failed"));
 
   n++;
   sync_params();

   return true;
}

Boolean disk_hash::_insert(data_t& w, Boolean rehash_if_fail)
{
   int hash = w.bucket_num(k, p, M);

//int hash = w.key.int_key;
//debug(cerr, hash);

   disk_bucket& b = bucket_vector -> get_bucket(hash);

   int slot_num = b.insert(&w);

   if ( slot_num != 0 ) {
      caching(b, w, slot_num);
   } else {
///////////////////////////////////
// insert into the overflow bucket
///////////////////////////////////

//MESSAGE(cerr, "INSERT to overflow buckets");
//debug(cerr, hash);

      for ( hash %= v; hash < (int) v; hash++ ) {

         disk_bucket& overflowb = bucket_vector -> get_bucket(hash+M);

         slot_num = overflowb.insert(&w);

         if ( slot_num != 0 ) {
           caching(overflowb, w, slot_num);
           break;
         }
      }

      if ( slot_num == 0 && rehash_if_fail == true ) 
         return rehash(w);
   }

   if ( slot_num != 0 ) 
      return true;
   else
      return false;
}

void disk_hash::caching(disk_bucket& b, data_t& w, int slot_num)
{
//debug(cerr, b.bnum());
//debug(cerr, k_vector -> count());

   int k = int((long)(*k_vector)[b.bnum()]);
   int r = int((long)(*r_vector)[b.bnum()]);

///////////////////////////////////////////
// cache all keys in the bycket except w.
// In fact, only need to cache keys whose
// hash vector slots have been updated.
// It is to be enhanced.
///////////////////////////////////////////

   int ind = b.first();
   while ( ind != 0 && ind != slot_num ) {

      data_t* x = b(ind);

      if ( x ) {
         hash_vector -> insert(
                   (voidPtr)(size_t)ind,
                   x -> slot_num(k, r, p, hash_vector -> count())
                              );
      }

      delete x;

      b.next(ind);
   }

////////////////////////////////////////
// cache w. it is always in the cache.
// others may be overwritten. 
////////////////////////////////////////
   hash_vector -> insert(
         (voidPtr)(size_t)slot_num,
         w.slot_num(k, r, p, hash_vector -> count())
                        );
}
      
/******************************************/
// remove operation
/******************************************/
Boolean disk_hash::remove(data_t& w)
{
   int slot_num;
   disk_bucket* b = 0;
   if ( member(w, b, slot_num) == true ) {

      b -> remove(slot_num);

      n--;
      sync_params();

      return true;
   } else
      return false;
}

Boolean disk_hash::member(data_t& w) 
{
//MESSAGE(cerr, "disk_hash::member():");
//asciiOut(cerr);
//MESSAGE(cerr, "+++++++++++++");
   disk_bucket* b = 0;
   int slot_num;
   return member(w, b, slot_num);
}

Boolean disk_hash::member(data_t& w, disk_bucket*& b, int& slot_num) const
{
   int hash = w.bucket_num(k, p, M);

//int hash = w.key.int_key;
//debug(cerr, hash);

   b = &bucket_vector -> get_bucket(hash);

   int k = int((long)(*k_vector)[b -> bnum()]);
   int r = int((long)(*r_vector)[b -> bnum()]);

   slot_num = 
     int((long)(*hash_vector)[w.slot_num(k, r, p, hash_vector -> count())]);

//debug(cerr, slot_num);

   if ( b -> member(w, slot_num) == true )
      return true;

   if ( b -> overflow() == true ) {

      for ( hash %= v; hash < (int) v; hash++ ) {

         b = &bucket_vector -> get_bucket(hash+M);

         if ( b -> member(w, slot_num) == true ) {
            return true;
         }
      }
   } 

   return false;
}

int disk_hash::first_bucket()
{
   return ( M > 0 ) ? 0 : -1;
}

disk_bucket* disk_hash::get_bucket(int& ind)
{
   return &bucket_vector -> get_bucket(ind);
}

void disk_hash::next_bucket(int& ind)
{
   ind = ( ind >= (int)(M+v-1) ) ? -1 : (ind+1);
}


/*******************************************************/
// print operation
/*******************************************************/
ostream& disk_hash::asciiOut(ostream& out)
{
   int ind = first_bucket();

   while ( ind != -1 ) {
 
//MESSAGE(cerr, "New Bucket:");
//debug(cerr, ind);
      disk_bucket* bucket = get_bucket(ind);

      if ( bucket ) {
         out << *bucket;
      }

      next_bucket(ind);
   }

   return out;
}

istream& disk_hash::asciiIn(istream& in)
{
   data_t actor;

   while ( in >> actor ) {
      _insert(actor, true);
      n++;
   }

   sync_params();

   return in;
}

ostream& disk_hash::asciiOut(ostream& out, print_func_ptr_t print_f)
{

   int ind = first_bucket();

   while ( ind != -1 ) {
 
      disk_bucket* bucket = get_bucket(ind);

      if ( bucket ) {
         bucket -> asciiOut(out, print_f);
      }

      next_bucket(ind);
   }

   return out;
}

void disk_hash::out_params()
{
   debug(cerr, k);
   debug(cerr, p);
   debug(cerr, M);
   debug(cerr, v);
   debug(cerr, n);
}
