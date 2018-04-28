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
 * $XConsortium: bucket.cc /main/3 1996/06/11 17:18:55 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */


#include "dynhash/bucket.h"

int bucket::upper_limit;

bucket::bucket(int new_Mj, int owj) : wj(0), Mj(new_Mj), old_wj(owj)
{
   k = 19;
   data_array = new data_t[2*Mj*Mj];
}

bucket::~bucket()
{
   delete [2 * Mj * Mj] data_array;
}

/***********************************************/
// rehash the existing keys and the new key 
// 'data'
/***********************************************/
void bucket::rehash_all(data_t& data, shared_t& sh)
{
   int j = 0;
   int ind = first();
   while ( ind != -1 ) {
      sh.internal_L[j++] = (*this)(ind);
      data_array[ind].marked = false;
      next(ind);
   }
   sh.internal_L[j] = data;

   select_h_params(sh);

   for ( ind = 0; ind <= j; ind++ ) {
       int hash = h(sh.internal_L[ind].key, sh);
       data_array[hash] = sh.internal_L[ind];
       data_array[hash].marked = true;
   }
}

/****************************************/
// select a proper value for parameter k
// sh contains useful variables from the
// first level hash table.
/****************************************/
void bucket::select_h_params(shared_t& sh)
{
   Boolean injective = false;
   int loops = 0;

/******************************************/ 
// loop until an injective mapping is found
/******************************************/ 

   while ( injective == false ) {
      k = sh.rand_generator.rand() % (sh.p - 1) + 1;
      injective = true;
      for ( int i=0; i<wj; i++ ) {
          int hash = h( sh.internal_L[i].key, sh  );
          if ( data_array[hash].marked == false ) {
             data_array[hash].marked = true ;
          } else {
            for ( int j = 0; j<2*Mj*Mj; j++ )
               data_array[j].marked = false;
            injective = false;
            break;
          }
      }
      loops ++;
      if ( loops >= 20 ) { // supposedly loop twice
         debug(cerr, loops);
         debug(cerr, Mj);
         debug(cerr, wj);
         debug(cerr, k);
         for ( int i=0; i<wj; i++ ) {
            int hash = h( sh.internal_L[i].key, sh  );
            debug(cerr, sh.internal_L[i]);
            debug(cerr, hash);
         }
         throw(boundaryException(1, 20, loops));
      }
   }
}

/************************************************/
// insert 
/************************************************/
Boolean bucket::insert(data_t& data, shared_t& sh)
{
   wj++;
   int j = h(data.key, sh);

   if ( wj <= Mj ) {

/**********************/
// space is enough 
/**********************/

      if ( data_array[j].marked == false ) {
         data_array[j] = data;
         data_array[j].marked = true;
      } else 
      if ( data_array[j].key == data.key) {
         debug(cerr,  data_array[j]);
         debug(cerr,  data);
         MESSAGE(cerr, "key is in the set");
         return true;
      } else
         rehash_all(data, sh); 
      
      return true;
   } else {

/***************************/
// need to double the space 
// debug(cerr, "rehash bucket");
// debug(cerr, data.key);
/***************************/

      int old_contribute = 2 * old_wj * old_wj;
      int new_contribute = 2 * wj * wj;
      if ( sh.sum - old_contribute + new_contribute < sh.limit ) {

/**************************************/
// if condition 4 holds, we just rehash 
// keys in this table
/**************************************/
         sh.sum -= old_contribute;
         sh.sum += new_contribute;
         old_wj = wj;

         data_t *x = new data_t[8*Mj*Mj];

         int ind = first();

/********************/
// collect records
/********************/
         while ( ind != -1 ) {
            x[ind] = (*this)(ind);
            next(ind);
         }

/*****************************/
// allocate new space
/*****************************/
         delete [2*Mj*Mj] data_array;
         data_array = x;

         Mj *= 2;

/*****************************/
// re-hash keys in this bucket
/*****************************/
         rehash_all(data, sh); 

         return true;

      } else {
/*******************************/
// Condition 4 does not hold.
// Return false to signal a 
// entire set re-hash.
/*******************************/
         return false;
      }
   }
}

/*******************************/
// compute the hash value
/*******************************/
int bucket::h(int key, shared_t& sh)
{
   return ( (abs( k * key )) % (sh.p) ) % (2 * Mj * Mj);
}

/*******************************/
// membership test
/*******************************/
Boolean bucket::member(data_t& data, shared_t& sh)
{
   int pos = h( data.key, sh );
   if ( data_array[pos].marked == true ) {
      data.dt = data_array[pos].dt;

      if ( data_array[pos].key == data.key ) 
         return true;
      else 
         return false;

   } else {
      data.key = -1;
      return false;
   }
}

/*******************************/
// remove operation
/*******************************/
Boolean bucket::remove(data_t& data, shared_t& sh)
{
   int pos = h( data.key, sh );

   if ( data_array[pos].marked == true &&
        data_array[pos].key == data.key) {

      data_array[pos].marked = false;
      wj--;
      return true;

   } else {
      MESSAGE(cerr, "+++++++++++++++");
      debug(cerr, wj);
      debug(cerr, *this);
      debug(cerr, data);
      debug(cerr, data_array[pos]);
      MESSAGE(cerr, "data is not in the key set");
      MESSAGE(cerr, "+++++++++++++++");
      return false;
   }
}

/***********************************************/
// iteration operations
/***********************************************/
int bucket::first()
{
   upper_limit = 2 * Mj * Mj;
   if ( wj == 0 ) 
      return -1;
   else {
      int i=0;
      while ( data_array[i].marked == false ) {
         if ( i >= upper_limit ) {
            debug(cerr, i);
            debug(cerr, upper_limit);
            throw(stringException("hash table is in inconsistent status"));
         }
         i++;
      }
      return i;
   }
}

data_t& bucket::operator()(int ind)
{
   return data_array[ind];
}

void bucket::next(int& ind)
{
   for ( int j = ind+1; j < upper_limit; j ++ )
      if ( data_array[j].marked == true ) {
         ind = j;
         return;
      }
       
   ind = -1;
}

/***********************************************/
// print operation
/***********************************************/
ostream& operator<<(ostream& out, bucket& bt)
{
   int ind = bt.first();
   while ( ind != -1 ) {
      debug(out, bt(ind));
      bt.next(ind);
   }
   return out;
}
