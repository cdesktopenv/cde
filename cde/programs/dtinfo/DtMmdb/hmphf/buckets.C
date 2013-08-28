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
 * $XConsortium: buckets.cc /main/3 1996/06/11 17:19:53 cde-hal $
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



#include "hmphf/buckets.h"

bucket::bucket(char* key, int orig_position, Boolean copy) :
   v_no_keys(1),
   v_count(0),
   v_control_bit(-1),
   v_g_value(0),
   v_orig_pos(orig_position)
{

   char* x = 0;
   int len;

   switch (copy) {
     case true:
       len = strlen(key);
       x = new char[len + 1];
       *((char *) memcpy(x, key, len) + len) = '\0';
       break;
     case false:
       x = key;
       break;
   }
   key_ptr = new slist_void_ptr_cell(x);
}

bucket::~bucket() 
{
   slist_void_ptr_cell *lp = key_ptr;
   slist_void_ptr_cell* tmp_lp = 0;

   while ( lp ) {

      tmp_lp = lp;
      lp = (slist_void_ptr_cell*)(lp -> v_succ);
      delete tmp_lp;
   }
}

int bucket::add_key(char* key, Boolean copy)
{
   char *x = 0;
   int len;

   switch (copy) {
     case true:
       len = strlen(key);
       x = new char[len + 1];
       *((char *) memcpy(x, key, len) + len) = '\0';
       break;
     case false:
       x = key;
       break;
   }

   slist_void_ptr_cell* z = new slist_void_ptr_cell(x);

    z -> v_succ = key_ptr;
    key_ptr = z;

    v_no_keys++;
    return v_no_keys;
}

ostream& operator<<(ostream& out, bucket& bt)
{
   slist_void_ptr_cell *lp = bt.key_ptr;

   while ( lp ) {

      out << ((char*)(lp -> void_ptr())) << " ";
      lp = (slist_void_ptr_cell*)(lp -> v_succ);

   }
   out << "\n";
   return out;
}

////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////

//buckets::buckets(char* key_file, params& pms) :

buckets::buckets(char** keys, params& pms) :
v_no_buckets(pms.v_b), v_max_bucket_sz(0), 
rnd(pms.v_seed),
b_convertor(pms.v_n, 128, rnd), 
h_convertor(pms.v_n, 128, rnd)
{
   v_bucket_array = new bucketPtr[v_no_buckets]; 

   unsigned int i;
   for ( i=0; i < (unsigned int) v_no_buckets; v_bucket_array[i++] = 0);

//debug(cerr, pms);

   int hash, k;
   for ( i=0; i<pms.v_n; i++ ) {

//debug(cerr, keys[i]);
      hash = bucket_num(keys[i], pms);

      if ( v_bucket_array[hash] == 0 ) {
         v_bucket_array[hash] = new bucket(keys[i], hash, false);
         k = 1;
      } else {
         k = v_bucket_array[hash] -> add_key(keys[i], false);
      }

      v_max_bucket_sz = MAX(v_max_bucket_sz, k);
   }

   sort_by_size();
}

buckets::~buckets()
{
   for ( int i=0; i<v_no_buckets; i++ ) 
      delete v_bucket_array[i];
   
   delete v_bucket_array;
}

void buckets::set_control_bit(int cb)
{
   for ( int i=0; i<v_no_buckets; i++ ) {
      if ( (*this)[i] )
         (*this)[i] -> set_control_bit(cb);
   }
}

int buckets::bucket_num(char* k, params& pms)
{
//MESSAGE(cerr, "bucket_num");
//debug(cerr, k);
   int sum = b_convertor.atoi(k, strlen(k), pms.v_r, pms.v_n);

//debug(cerr, sum);

   if ( sum < (int) pms.v_p1 ) {
      sum %= pms.v_p2;
   } else {
      sum %= (pms.v_b - pms.v_p2);
      sum += pms.v_p2;
   }

//debug(cerr, sum);

   return sum;
}
      
//int buckets::hash_value(char* k, int offset, int range)
//{
///*
//MESSAGE(cerr, "hash_value");
//debug(cerr, k);
//*/
////debug(cerr, strlen(k));
//
//
//
///*
//debug(cerr, offset+1);
//debug(cerr, range);
//*/
//   int hv = h_convertor.atoi(k, strlen(k), offset+1, range);
//
////debug(cerr, hv);
//
///*
//if ( strcmp(k, "mphf_funcs.h") == 0 || 
//     strcmp(k, "mmdb_fast_mphf") == 0 ) {
//debug(cerr, k);
//debug(cerr, offset); 
//debug(cerr, range); 
//debug(cerr, hv); 
//}
//*/
//
//   return hv;
//}

void buckets::sort_by_size()
{
//MESSAGE(cerr, "sort()");
   int* links = new int[v_no_buckets];
   int i;
   for ( i=0; i<v_no_buckets; links[i++]=-1 );

   int* sizes = new int[v_max_bucket_sz+1];
   for ( i=0; i<v_max_bucket_sz+1; sizes[i++]=-1 );

// arrage buckets according to their size
   int x;
   for ( i = 0; i < v_no_buckets; i++ ) {

      if ( v_bucket_array[i] == 0 ) 
         continue;

      x = v_bucket_array[i] -> v_no_keys;

      links[i] = sizes[x];

      sizes[x] = i;
   }

   bucketPtr* new_bkt_array = new bucketPtr[v_no_buckets];

   int j=0;
   int idx;
   for ( i = v_max_bucket_sz; i >= 0; i-- ) {

      if ( sizes[i] == -1 )
        continue;

      idx = sizes[i];

//debug(cerr, i);
      while ( idx != -1 ) {

         new_bkt_array[j++] = v_bucket_array[idx];

//debug(cerr, new_bkt_array[j-1] -> v_no_keys);
//debug(cerr, *new_bkt_array[j-1]);

         v_bucket_array[idx] = 0;

         idx = links[idx];
      }
   }

   for ( ; j<v_no_buckets; new_bkt_array[j++] = 0 );

   delete sizes;
   delete links;

   delete v_bucket_array;
   v_bucket_array = new_bkt_array;
}

      
/*************************************************/
/* return -1 if no more pattern can be generated */
/* return 0 if a pattern is generated            */
/*************************************************/
int 
buckets::get_pattern(int idx, int_pattern& pat, params& pms)
{

   v_bucket_array[idx] -> v_control_bit++;
   for ( ; v_bucket_array[idx] -> v_control_bit<2 ;
           v_bucket_array[idx] -> v_control_bit ++
   ) {

      if ( use_current_params(idx, pat, pms) == 0 )
          return 0;
      
   }
   return -1;
}

int 
buckets::use_current_params(int idx, int_pattern& pat, 
                            params& pms, Boolean use_g_value)
{
   int i = 0;
   slist_void_ptr_cell *lp = v_bucket_array[idx] -> key_ptr;

   while ( lp ) {

            
//debug(cerr, (char*)(lp -> void_ptr()));
//cerr << (char*)(lp -> void_ptr()) << "\n";

     if ( use_g_value == false ) {
        pat.insert(
            hash_value( ((char*)(lp -> void_ptr())),
                 pms.v_r + v_bucket_array[idx] -> v_control_bit,
                 pms.v_n
                      ),
         i++
                  );
     } else {

//cerr << (char*)(lp -> void_ptr()) << "\n";
//debug(cerr, pms.r + v_bucket_array[idx] -> v_control_bit);
/*
debug(cerr, ((ostring*)(lp -> void_ptr())) -> get());
int from_tbl1 =  hash_value(
                 ((char*)(lp -> void_ptr())),
                 pms.r + v_bucket_array[idx] -> v_control_bit,
                 pms.n
                 );

debug(cerr, from_tbl1);
debug(cerr, v_bucket_array[idx] -> v_g_value);
*/

         pat.insert(
           (
               hash_value(
                 ((char*)(lp -> void_ptr())),
                 pms.v_r + v_bucket_array[idx] -> v_control_bit,
                 pms.v_n
                         ) 
               + 
               v_bucket_array[idx] -> v_g_value
           ) % pms.v_n,
           i++
          );
      }
      lp = (slist_void_ptr_cell*)(lp -> v_succ);
   }

//debug(cerr, v_bucket_array[idx] -> no_keys());
   pat.reset_elmts(v_bucket_array[idx] -> no_keys());

//debug(cerr, pat);

   return pat.duplicate();
}
   
ostream& operator<<(ostream& out, buckets& bts)
{
   for ( int i=0; i<bts.no_buckets(); i++ )
      if ( bts[i] ) {
         debug(cerr, bts[i] -> orig_pos());
         debug(out, *bts[i]);
      }

   return out;
}

