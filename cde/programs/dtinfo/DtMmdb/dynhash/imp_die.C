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
 * $XConsortium: imp_die.C /main/5 1996/08/21 15:52:00 drk $
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


#include "dynhash/imp_die.h"

#ifdef C_API
#include "utility/c_iostream.h"
#else
#include <iostream>
using namespace std;
#endif

int steps[] = { 2, 3, 5, 7, 11, 13, 17, 21, 23, 29, 31, 37, 41, 43, 47, 51 };
int no_steps;

#define PRIME_LEVLE_2 2147483647

imp_die::imp_die(int prime, int expected_n) : 
p(prime), H(0), B(0), n(0), bucket_array(0), hash_table(0),
free_list_head(0), collected_records(0)
{
   alloc_table(int(2.5*expected_n));
   init_table();

   rand_generator.seed();
   k = rand_generator.rand();

   no_steps = sizeof(steps) / sizeof(int);
}

imp_die::~imp_die()
{
//debug(cerr, H);
//debug(cerr, B);
//debug(cerr, n);

   for ( int i = 0; i<B; i++ ) {
      delete bucket_array[i];
   }

   bucket_holder* x = free_list_head;
   while ( x ) {
      bucket_holder* y = x -> next;
      delete x;
      x = y ;
   }

   x = collected_records;
   while ( x ) {
      bucket_holder* y = x -> next;
      delete x -> data_ptr; 
      delete x;
      x = y ;
   }

   delete hash_table;
   delete bucket_array ;
}

//**********************************************************
// allocate new memory for the bucket table and hash table. 
// H is the size of the bucket table before 
// expansion. new\_H is the new size.
//**********************************************************
void imp_die::alloc_table(int new_H)
{
   if ( bucket_array ) {
      for ( int i = 0; i<B; delete bucket_array[i++] );
      delete bucket_array ;
   }

   B = new_H/2;
   bucket_array = new imp_bucketPtr[B];

   if ( hash_table ) {
      delete hash_table;
   }

   hash_table = new data_tPtr[new_H];

   H = new_H;
}

void imp_die::init_table()
{
   int i;
   for ( i = 0; i < B; i++ ) {
      bucket_array[i] = 0 ; 
   }
   for ( i = 0; i < H; i++ ) {
      hash_table[i] = 0; 
   }
}

void imp_die::clean()
{
   n = 0;
   collect_all_keys();

   int i;
   for ( i=0; i<B; i++ ) {
      if ( bucket_array[i] ) {
         delete bucket_array[i];
         bucket_array[i] = 0 ;
      }
   }

   for ( i=0; i<H; i++ ) 
      hash_table[i] = 0;
}

/*****************************/
// collect all keys into 
// bcuket_list_head
/*****************************/
void imp_die::collect_all_keys()
{
   for ( int i = 0; i < B; i++ ) 
      if ( bucket_array[i] ) {
 
         bucket_holder *x ;
         if ( free_list_head ) {
            x = free_list_head ;
//debug(cerr, "get from free list");
            free_list_head = free_list_head -> next;
         } else {
//debug(cerr, "get from new");
            x = new bucket_holder;
         }
//debug(cerr, int(x));

         x -> data_ptr = bucket_array[i] -> remove_all();

         delete bucket_array[i];
         bucket_array[i] = 0;

         x -> next = collected_records;
         collected_records = x;
      }
}

/*****************************/
// rehash all keys
/*****************************/
Boolean imp_die::rehash()
{
   while ( 1 ) {

      collect_all_keys();
   
      if ( 2*n > H ) alloc_table(2*H);  
      
      init_table();
   
      k = rand_generator.rand();
   
      bucket_holder *x = collected_records;
      while ( x ) {
   
         data_t* y = x -> data_ptr;
   
         while ( y ) {
   
            data_t *z = (data_t*)(y -> v_succ);
   
            y -> v_pred = 0;
            y -> v_succ = 0;
   
            int hash = y -> bucket_num(k, p, B);
      
            if ( bucket_array[hash] == 0 )
               bucket_array[hash] = new imp_bucket;
      
            bucket_array[hash] -> insert(y); 
   
            y = z;
         }
         
         x = x -> next;
      }
   
      free_list_head = collected_records; 
      collected_records = 0; 

      Boolean rehash_done = true;
   
      for ( int i = 0; i < B; i++ ) 
         if ( bucket_array[i] ) {
            if ( bucket_rehash(i) == false ) {
                debug(cerr, i);
                debug(cerr, *bucket_array[i]);
		rehash_done = false;
		break;
                //throw(stringException("rehash() failed"));
            }
         }

      if ( rehash_done == true )
        return true;
   }
}

/************************************/
// insert 
/************************************/
Boolean imp_die::insert(data_t& v)
{
   n++;

   int hash = v.bucket_num(k, p, B);

   if ( bucket_insert(hash, v) == false ) 
      rehash();
   
   return true;
}

/******************************************/
// remove operation
/******************************************/
Boolean imp_die::remove(data_t& v)
{
MESSAGE(cerr, "imp_die::remove(data_t& v)");
   int hash = v.bucket_num(k, p, B);

/*********************************/ 
// assure the bucket is not empty
/*********************************/ 
   if ( bucket_remove(hash, v) == false )
       return false;

   n--;

/*********************************/ 
// delete the bucket if it becomes
// empty
/*********************************/ 
   if ( bucket_array[hash] -> empty() == true ) {
      delete bucket_array[hash];
      bucket_array[hash] = 0;
   }

   return true;
}

/*******************************************************/ 
// first level hash function 
/*******************************************************/ 
int imp_die::h(int key) const
{
   return abs( k * key ) % p % B;
}

//static to_print = false;
/****************************************/
// select a proper value for parameter k
/****************************************/
Boolean imp_die::bucket_fix_k(int bucket_num)
{
   int loops = 0;

   Boolean injective = false;

   imp_bucket& x = *bucket_array[bucket_num];

/*
MESSAGE(cerr, "bucket_fix_k()");
debug(cerr, x);

if ( bucket_num == 412 )
  to_print = true;
else
  to_print = false;
*/

   while ( injective == false && loops < H ) {

      injective = test_injective(x);
      if ( injective == false ) {
         loops ++;
         x.k = abs(rand_generator.rand()) % ( p - 1 ) + 1;
      } 
   }

   return true;
}

Boolean imp_die::test_injective(imp_bucket& x)
{
//MESSAGE(cerr, "test_injective()");
   long ind_out = x.first();
   while ( ind_out != 0 ) {

      data_t* out = x(ind_out);

/*
if ( to_print == true ) {
debug(cerr, x.k);
debug(cerr, x.rotate);
debug(cerr, p);
debug(cerr, M);
}
*/
      int hash_out = out -> slot_num(x.k, x.rotate, PRIME_LEVLE_2, H);
   
      long ind_in = ind_out;
      x.next(ind_in);
   
      while ( ind_in != 0 ) {

         data_t* in = x(ind_in);

         int hash_in = in -> slot_num(x.k, x.rotate, PRIME_LEVLE_2, H);
      
         if ( hash_out == hash_in ) {
      
/*
if ( to_print == true )
{
      debug(cerr, x);
      debug(cerr, ind_in);
      debug(cerr, ind_out);
      debug(cerr, hash_in);
      debug(cerr, hash_out);
}
*/
      
            return false;
         }
   
         x.next(ind_in);
      }

      x.next(ind_out);
   }

   return true;
}

Boolean imp_die::bucket_rotate(int bucket_num)
{
   imp_bucket& x = *bucket_array[bucket_num];

   int loops = 0;

   int z = rand_generator.rand();

   x.rotate = z % (H - 1) + 1;
   int step = steps[z % no_steps];

   Boolean all_fit = false;
   int hash;

   while ( all_fit == false && loops < H ) {

      long ind = x.first();
      while ( ind != 0 ) {

         data_t* data_ptr = x(ind);
         hash = data_ptr -> slot_num(x.k, x.rotate, PRIME_LEVLE_2, H);

         if ( hash_table[hash] != 0 ) {
            long ind1 = x.first();
            while ( ind1 != ind ) {

               hash = x(ind1) -> slot_num(x.k, x.rotate, PRIME_LEVLE_2, H);

               hash_table[hash] = 0;

               x.next(ind1);
            }
            loops ++;
            x.rotate += step;
            break;
         }
         hash_table[hash] = data_ptr;
         x.next(ind);
      }
      all_fit = ( ind == 0 ) ? true : false;
   }

   return all_fit ;
}

Boolean imp_die::bucket_insert(int bucket_num, data_t& v)
{
   if ( bucket_array[bucket_num] == 0 ) {
      bucket_array[bucket_num] = new imp_bucket();
   }

   imp_bucketPtr x = bucket_array[bucket_num];

   data_t* y = 0;

   if ( collected_records ) {
     bucket_holder* first_list = collected_records; 
     y = first_list -> data_ptr; 

     first_list -> data_ptr = (data_t*)(y -> v_succ);

     if ( first_list -> data_ptr == 0 ) {
        collected_records = collected_records -> next;
        delete first_list;
     }

     *y = v;

   } else 
      y = new data_t(v);

   x -> insert(y);
   int hash = y -> slot_num(x->k, x->rotate, PRIME_LEVLE_2, H);
      
   if ( hash_table[hash] == 0 ) {
      hash_table[hash] = y;
      return true;
   }
   
//*******************************
// clear the hash table entries
//*******************************
   long ind = x -> first();
   while ( ind ) {
      int hash = (*x)(ind) -> slot_num(x->k, x->rotate, PRIME_LEVLE_2, H);

      if ( hash_table[hash] && *hash_table[hash] == *(*x)(ind) )
         hash_table[hash] = 0;
      x -> next(ind);
   }

   return bucket_rehash(bucket_num);
/*   
debug(cerr, bucket_num);
debug(cerr, int(x));
debug(cerr, int(bucket_array));
debug(cerr, int(bucket_array[bucket_num]));
*/

}


//*******************************
// rehash keys in the bucket 
//*******************************
Boolean imp_die::bucket_rehash(int bucket_num)
{
   bucket_fix_k(bucket_num);
   return bucket_rotate(bucket_num);
}

Boolean imp_die::bucket_remove(int bucket_num, data_t& v)
{
   imp_bucketPtr x = bucket_array[bucket_num];

   if ( x == 0 ) return false;

   int hash = v.slot_num(x->k, x->rotate, PRIME_LEVLE_2, H);

   if ( hash_table[hash] && *hash_table[hash] == v ) {

      x -> delete_cell(hash_table[hash]);
      delete hash_table[hash];
      hash_table[hash] = 0;

//MESSAGE(cerr, "afterremove the entry");
//debug(cerr, *x);

      return true;
   } else 
      return false;
}

Boolean imp_die::bucket_member(int bucket_num, data_t& v) const
{
/*
debug(cerr, bucket_num);
debug(cerr, v);
*/

   imp_bucketPtr x = bucket_array[bucket_num];

   if ( x == 0 ) {
//MESSAGE(cerr, "empty bucket");
//debug(cerr, int(bucket_array));
      return false;
   }

   int hash = v.slot_num(x->k, x->rotate, PRIME_LEVLE_2, H);

   if ( hash_table[hash] ) {
      v.dt = hash_table[hash] -> dt;
      if ( *hash_table[hash] == v ) {
/*
MESSAGE(cerr, "hash table entry match");
debug(cerr, int(this));
debug(cerr, v);
debug(cerr, *hash_table[hash]);
*/

         return true;
      } else {
         return false;
      }
   } else {
      return false;
   }
}

int imp_die::first_bucket()
{
   if ( B > 0 )
      return 0;
   else
      return -1;
}

void imp_die::next_bucket(int& ind)
{
   if ( ind >= B )
      ind = -1;
   else
      ind++;
}

imp_bucket* imp_die::get_bucket(int& ind)
{
   while ( ind < B && bucket_array[ind] == 0 )
     ind++;

   if ( ind >= B )
      return 0;
   else
      return bucket_array[ind];
}

/*******************************************************/
// print operation
/*******************************************************/
ostream& imp_die::asciiOut(ostream& out)
{
   int ind = first_bucket();

   while ( ind != -1 ) {
 
      imp_bucket* bucket = get_bucket(ind);

      if ( bucket )
         out << *bucket;

      next_bucket(ind);
   }

   return out;
}

istream& imp_die::asciiIn(istream& in)
{
   data_t actor;

   while ( in >> actor ) {
      insert(actor);
   }

   return in;
}

ostream& imp_die::asciiOut(ostream& out, print_func_ptr_t print_f)
{

   int ind = first_bucket();

   while ( ind != -1 ) {
 
      imp_bucket* bucket = get_bucket(ind);

      if ( bucket ) {
         bucket -> asciiOut(out, print_f);
      }

      next_bucket(ind);
   }

   return out;
}
