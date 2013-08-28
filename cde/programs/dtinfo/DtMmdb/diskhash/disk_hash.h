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
 * $XConsortium: disk_hash.h /main/3 1996/06/11 17:16:15 cde-hal $
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



#ifndef _disk_hash_h
#define _disk_hash_h

#include "diskhash/bucket_array.h"
#include "diskhash/disk_bucket.h"
#include "dstr/void_ptr_array.h"
#include "dstr/index_agent.h"
#include "storage/page_storage.h"

#define COLLISION_BIT 0x2

extern int steps[];
extern int no_steps;

class disk_hash : public index_agent 
{

public:
   disk_hash(page_storage* key_oid_store, 
             int prime = 32801, 
             int expected_n = 100
            ); 
                               // prime and expected
                               // key set size 
   virtual ~disk_hash();

   Boolean insert(data_t& v);  // insert a key
   Boolean remove(data_t& v);  // remove a key
   Boolean member(data_t& v);  // member test

   void clean();  // remove all keys

   //int no_keys() const { return n; }; // return key set size

// WARNING:  -1 is the terminate condition!!!
   int first_bucket();
   disk_bucket* get_bucket(int&);
   void next_bucket(int&);

// output this with print_f handling the printing of whole data_t.
// pointer to data_t as void* is passed to print_f
   ostream& asciiOut(ostream& out, print_func_ptr_t print_f);

   ostream& asciiOut(ostream& out);
   istream& asciiIn(istream& in);


protected:
   void init_params(int prime, int expected_n);
   void sync_params();

   void set_M(unsigned int newM) { M = newM; };
   void set_v(unsigned int newv) { v = newv; };
   void set_k(unsigned int newk) { k = newk; };
   void set_p(unsigned int newp) { p = newp; };
   void set_n(unsigned int newn) { n = newn; };

   Boolean rehash(data_t& w);   // rehash all keys in buckets + w
   Boolean _rehash(fstream& pool);   // rehash keys from pool

   Boolean _insert(data_t& v, Boolean rehash_if_fail);  
   Boolean member(data_t& v, disk_bucket*& b, int& slot_num) const;  

   void caching(disk_bucket& b, data_t& w, int slot_num);

   void out_params();

protected:
   unsigned int M;        // number buckets 
   unsigned int v;        // number of overflow buckets

   unsigned int k;        // parameter used in the 1st level hash function
   unsigned int p;        // prime number p

   unsigned int n;        // current key set size

   bucket_array* bucket_vector;  // bucket array

   void_ptr_array* hash_vector;  // hash array
   void_ptr_array* k_vector;     // k param array
   void_ptr_array* r_vector;     // r param array

   page_storage* key_store; // key_recordOID store

   buffer& buf; 

   pm_random rand_generator;   // rand generator
};

#endif

