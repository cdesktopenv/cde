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
 * $XConsortium: sorter.cc /main/3 1996/06/11 17:20:47 cde-hal $
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




#include "hmphf/sorter.h"

/*#define NUM_BUCKETS 10 */
#define NUM_BUCKETS 5000

sorter::sorter(char* key_file) :
   v_max_bucket_sz(0),
   v_no_unique_keys(0),
   v_unique_keys(0),
   b_convertor(NUM_BUCKETS, 128)
{
   fstream in(key_file, ios::in);
   _init(in);
}

sorter::sorter(istream& in) :
   v_max_bucket_sz(0),
   v_no_unique_keys(0),
   v_unique_keys(0),
   b_convertor(NUM_BUCKETS, 128)
{
   _init(in);
}

void sorter::_init(istream& in) 
{
   v_bucket_array = new bucketPtr[NUM_BUCKETS];

   int i;
   for ( i=0; i<NUM_BUCKETS; v_bucket_array[i++] = 0);

   char key_buf[LBUFSIZ];

   int k;
   while ( in.getline(key_buf, LBUFSIZ) ) {
      i = b_convertor.atoi(key_buf, strlen(key_buf), 0, NUM_BUCKETS);

      if ( v_bucket_array[i] == 0 ) {
         v_bucket_array[i] = new bucket(key_buf, i, true);
         k = 1;
      } else {
         k = v_bucket_array[i] -> add_key(key_buf, true);
      }

      v_max_bucket_sz = MAX(v_max_bucket_sz, k);
  
      in.getline(key_buf, LBUFSIZ); // skip the next info field
   }

   filter_by_hash();

   assemble_unique_keys();
}

sorter::~sorter() 
{
   int i;
   for ( i=0; i<NUM_BUCKETS; delete v_bucket_array[i++] );
   delete v_bucket_array;

   for ( i=0; i<v_no_unique_keys; delete v_unique_keys[i++] );
   delete v_unique_keys;
}

void sorter::filter_by_hash()
{
// a small hash table for keys to map to
   v_map_table = new charPtr[2*v_max_bucket_sz];

   int i;
   for ( i=0; i<2*v_max_bucket_sz; v_map_table[i++] = 0 );

// an int table remembering slots in the v_map_table
// that have been mapped.
   v_index_table =  new int[v_max_bucket_sz];

// a charPtr table remembering possibly duplicated keys
   v_check_table = new charPtr[v_max_bucket_sz];

// a charPtr table remembering dupcated keys
   v_dup_table = new charPtr[v_max_bucket_sz];

   for ( i=0; i<NUM_BUCKETS; i++ ) {
      if ( v_bucket_array[i] )
         filter_a_bucket(v_bucket_array[i]);
   }

   delete v_map_table;
   delete v_check_table;
   delete v_index_table;
   delete v_dup_table;
}

void sorter::filter_a_bucket(bucketPtr bkt)
{
   slist_void_ptr_cell *lp = bkt -> key_ptr;

   while ( lp ) {
      remove_keys(bkt, (char*)(lp -> void_ptr()), lp);
      lp = (slist_void_ptr_cell*)(lp -> v_succ);
   }
}

void sorter::remove_keys(bucketPtr bkt, char* key, slist_void_ptr_cell* lp)
{
//debug(cerr, key);

   slist_void_ptr_cell *llp = lp;

   while ( llp && llp -> v_succ ) {

      slist_void_ptr_cell *next_lp = 
          (slist_void_ptr_cell*)llp -> v_succ;
//debug(cerr, (char*)(next_lp -> void_ptr()));
      
      if ( strcmp( key, (char*)(next_lp -> void_ptr()) ) == 0 ) {
//MESSAGE(cerr, "rmove key:");
//cerr <<  (char*)(next_lp -> void_ptr()) << "\n";
         llp -> v_succ = next_lp -> v_succ;
      
         delete (char*)(next_lp -> void_ptr());
         delete next_lp;

         bkt -> v_no_keys --;
      } else
         llp = (slist_void_ptr_cell*)(llp -> v_succ);

   }
}


//
//
//   int n_chk = 0;
//   int n_idx = 0;
//
//   slist_void_ptr_cell *lp = bkt -> key_ptr;
//
//   while ( lp ) {
//
//// maintaint the order of key chains in v_check_table[]!!!
//
//      char* key2 = ((char*)(lp -> void_ptr()));
//
//      int hash = b_convertor.atoi(
//                    key2, strlen(key2), 
//                    1, 2*v_max_bucket_sz 
//                                 );
//
//      if ( v_map_table[hash] != 0 ) {
//
//         if ( v_map_table[hash] != (charPtr)1 ) {
//            v_check_table[n_chk++] = v_map_table[hash];
//            v_map_table[hash] = (charPtr)1; 
//                                 // set to occupied. so that the same
//                                 // key will be in the v_check_table once.
//                                 // assume 1 will never be the address
//                                 // of any keys.
//         }
//
//         v_check_table[n_chk++] = key2;
//
//      } else {
//         //v_map_table[hash] = ((Ostring*)(lp -> void_ptr())) -> get();
//         v_map_table[hash] = ((char*)(lp -> void_ptr()));
//      }
//
//      v_index_table[n_idx++] = hash; // remember the slot being set
//
//      lp = (slist_void_ptr_cell*)(lp -> succ);
//   }
////debug(cerr, n_chk);
//
//// double check possiblly collided keys
//   int n_dup = 0;
//   for ( int u=0; u<n_chk-1; u++ ) {
//      for (int v=u+1; v<n_chk; v++ ) {
//         if ( v_check_table[u] && v_check_table[v] &&
//              strcmp(v_check_table[u], v_check_table[v]) == 0 ) 
//         {
//// mark v as a duplicated key
//            v_dup_table[n_dup++] = v_check_table[v];
//            v_check_table[v] = 0;
//         }
//           
//      }
//   }
//
////debug(cerr, n_dup);
//
//// remove the duplicates
//   if ( n_dup > 0 ) {
//      slist_void_ptr_cell *lp = bkt -> key_ptr;
//      slist_void_ptr_cell *prev_lp = lp;
//
//      int u = 0;
//      lp = (slist_void_ptr_cell*)(lp -> succ);
//
//      while ( lp && u<n_dup ) {
//
//
///*
//debug(cerr, v_dup_table[u]);
//debug(cerr, int(v_dup_table[u]));
//debug(cerr, (char*)(lp -> void_ptr()));
//debug(cerr, int((char*)(lp -> void_ptr())));
//*/
//
//
//         if ( v_dup_table[u] == ((char*)(lp -> void_ptr())) ) 
//         {
//
//MESSAGE(cerr, "key removed:");
//debug(cerr, v_dup_table[u]);
//
//             prev_lp -> succ = lp -> succ;
//             bkt -> v_no_keys --;
//             u++;
//         } else 
//             prev_lp = lp;
//
//         lp = (slist_void_ptr_cell*)(lp -> succ);
//         
//      }
//   }
//
//// clean v_map_table
//   for ( int k=0; k<n_idx; k++ ) { 
//      v_map_table[v_index_table[k]] = 0;
//   }
//
////MESSAGE(cerr, "SOS0");
//}

void sorter::assemble_unique_keys()
{
   int i;
   for ( i=0; i<NUM_BUCKETS; i++ ) {
      if ( v_bucket_array[i] ) {
         v_no_unique_keys += v_bucket_array[i] -> v_no_keys;
      }
   }

   v_unique_keys = new charPtr[v_no_unique_keys]; 

   int j = 0;
   slist_void_ptr_cell *lp = 0;

   for ( i=0; i<NUM_BUCKETS; i++ ) {

      if ( v_bucket_array[i] == 0 ) continue;

      lp = v_bucket_array[i] -> key_ptr;

      while ( lp ) {
   
         v_unique_keys[j++] = ((char*)(lp -> void_ptr()));
         lp -> set_vptr(0);
   
         lp = (slist_void_ptr_cell*)(lp -> v_succ);
      }
   }
}

ostream& operator<< (ostream& out, sorter& st)
{
   debug(out, st.v_no_unique_keys);

   for ( int i=0; i<st.v_no_unique_keys; i++ ) {
      out << st.v_unique_keys[i] << "\n";
   }

   return out;
}
