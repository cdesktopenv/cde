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
 * $TOG: mphf_hash_table.C /main/4 1998/04/17 11:50:23 mgreess $
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


#include "hmphf/mphf_hash_table.h"

#define FULL 1

mphf_hash_table::mphf_hash_table(params& params_ptr) :
v_no_slots(params_ptr.v_n), v_num_filled_slots(0)
{
   v_rep = new char[v_no_slots];
   v_random_table = new int[v_no_slots];
   v_map_table = new int[v_no_slots];

   clear();
}

mphf_hash_table::~mphf_hash_table()
{
   delete v_rep ;
   delete v_random_table ;
   delete v_map_table ;
}

void mphf_hash_table::clear()
{
   int i;
   //, right;

   pm_random pm(19);

   for ( i=0; i<v_no_slots; i++ ) {
      v_rep[i] = 0;
   }

   for ( i=0; i<v_no_slots; i++ ) {
      v_random_table[i] = i;
   }

   for ( i = 0; i < v_no_slots; i++) {
      //right = pm.rand() % ( v_no_slots - i) + i;
      int_swap( v_random_table[pm.rand() % ( v_no_slots - i) + i],  
                v_random_table[i]
              );
   }

   for ( i = 0; i < v_no_slots; i++)
       v_map_table[v_random_table[i]] = i;

   v_num_filled_slots = 0;
}


int mphf_hash_table::fast_fit(int_pattern& pat)
{
   int ok;
   int i, j, alignment, landing_slot;
   int right_rdm_tbl_index, left_rdm_tbl_cnt;

   for ( i = v_num_filled_slots; i < v_no_slots; i++ ) {
      ok = 0;

/**************************/
/* compute the alignment  */
/**************************/
      alignment = (v_no_slots + v_random_table[i] - pat[0]) 
                    % v_no_slots;

/**************************/
/* test fit the pattern   */
/**************************/
      for ( j=1; j<pat.no_elmts(); j++ ) {
         landing_slot = (pat[j] + alignment) % v_no_slots;
         if ( v_rep[landing_slot] == FULL ) {
            ok = -1;
            break; // try another alignment
         }
      }

/**************************/
/* really fit the pattern */
/**************************/
      if ( ok == 0 ) {
         for ( j=0; j < pat.no_elmts(); j++ ) {
            landing_slot = (pat[j] + alignment) % v_no_slots;
            v_rep[landing_slot] = FULL ;

            right_rdm_tbl_index = v_map_table[landing_slot];
            left_rdm_tbl_cnt = v_random_table[v_num_filled_slots + j];

            v_random_table[right_rdm_tbl_index] = left_rdm_tbl_cnt;
            v_map_table[left_rdm_tbl_cnt] = right_rdm_tbl_index;
            v_random_table[v_num_filled_slots + j] = landing_slot;

            v_map_table[landing_slot] = v_num_filled_slots + j;
         }
         v_num_filled_slots += pat.no_elmts();
         return alignment;
      }
   }
   return -1;
}

int mphf_hash_table::fit_hash_table(int_pattern& pat)
{
   int i, j;
   for ( i=0; i<pat.no_elmts(); i++ ) {
      if ( v_rep[int(pat[i])] != (int) 0 ) {

         for ( j=0; j<=i; j++ ) 
           v_rep[int(pat[j])] = (int) 0;
     
         v_num_filled_slots -= i+1;
         return -1;
      } else {
         v_rep[int(pat[i])] = FULL; 
         v_num_filled_slots++;
      }
   }
   return 0;
}
