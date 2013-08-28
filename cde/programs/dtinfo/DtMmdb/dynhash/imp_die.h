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
 * $XConsortium: imp_die.h /main/3 1996/06/11 17:19:33 cde-hal $
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



#ifndef _imp_die_h
#define _imp_die_h

#include "dstr/index_agent.h"
#include "dynhash/imp_bucket.h"

/***************************************************************/
//
// An improved version of Dietzfelbinger etc.'s algorithm.
//
//             Goal: reducing the space consumption.
//
//                            By
//
//                  QiFan Chen  (June 24, 1992)
//
/***************************************************************/


#define COLLISION_BIT 0x2

extern int steps[];
extern int no_steps;

class bucket_holder {
 
public:
   data_t* data_ptr;
   bucket_holder *next;

   bucket_holder() : data_ptr(0), next(0) {};
   virtual ~bucket_holder() {};
};

//extern data_t bad_record;

class imp_die : public index_agent 
{

protected:

   int k;        // parameter used in the 1st level hash function
   int p;        // prime number p

   int H;        // current hash table size
   int B;        // current bucket table size
   int n;        // current key set size

   imp_bucketPtr* bucket_array;    // bucket array
   data_tPtr* hash_table;   // the hash table

   bucket_holder* free_list_head ;   // free bucket holder list head
   bucket_holder* collected_records; // collected bucket list head

   pm_random rand_generator;         // rand generator

   int h(int key) const;                 // h_{sM}() function
   void alloc_table(int new_M); // expand the hash tabel and bucket array
   void init_table();           // init hash table and bucket array
   void collect_all_keys();     // collect all keys into bcuket_list_head
   Boolean rehash();            // rehash all keys 

   Boolean bucket_insert(int bucket_num, data_t&);
   Boolean bucket_remove(int bucket_num, data_t&);
   Boolean bucket_member(int bucket_num, data_t&) const;
   Boolean bucket_rehash(int bucket_num) ;

   Boolean bucket_fix_k(int bucket_num);
   Boolean bucket_rotate(int bucket_num);
   Boolean test_injective(imp_bucket& x);

public:
   imp_die(int prime = 32801, int expected_n = 100); 
                               // prime and expected
                               // key set size 
   virtual ~imp_die();

   void clean() ; // remove all keys 

   Boolean insert(data_t& v);  // insert a key
   Boolean remove(data_t& v);  // remove a key
   Boolean member(data_t& v) {
      return bucket_member(v.bucket_num(k, p, B), v);
   }; // member test


   int no_keys() const { return n; }; // return key set size

// WARNING:  -1 is the terminate condition!!!
   int first_bucket();
   imp_bucket* get_bucket(int&);
   void next_bucket(int&);

// output this with print_f handling the printing of whole data_t.
// pointer to data_t as voidd* is passed to print_f
   ostream& asciiOut(ostream& out, print_func_ptr_t print_f);

   ostream& asciiOut(ostream& out);
   istream& asciiIn(istream& in);
};

#endif

