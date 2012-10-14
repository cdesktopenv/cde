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
 * $XConsortium: mphf_funcs.cc /main/4 1996/07/18 14:33:08 drk $
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



#include "hmphf/mphf_funcs.h"

#define NO_BACKTRACKS 50
#define NO_MAPPINGS 20
#define NO_SEARCHINGS 10

//compute_a_mphf(char* key_file, params& pms, char* mphf_spec_file)

int compute_a_mphf(char** keys, params& pms, buffer& mphf_buffer)
{
   mphf_hash_table ht(pms);      

   int i, k;

   for ( k=0; k<NO_MAPPINGS; k++ ) {

      //buckets bs(key_file, pms);
      buckets bs(keys, pms);

//MESSAGE(cerr, "buckets built");
     
/* search for a MPHF */
      for ( i = 0; i<NO_SEARCHINGS; i++) {

//MESSAGE(cerr, form("%dth search:", i+1)); 

         bs.set_control_bit(-1);
         ht.clear();

         if ( search(bs, ht, pms) == 0 ) {
//MESSAGE(cerr, "search done");
            
/* verify computed MPHF */
            if ( verify(bs, ht, pms) == 0 ) {

/* output the computed MPHF */
                return write_spec(bs, pms, mphf_buffer);

            } else {
                return -1;
            }
         }
      }
      pms.v_r++;
   }

   return 1;
}

int search(buckets& bs, mphf_hash_table& ht, params& pms)
{
   int i = 0,
       fails = 0,
       patternFit = 0;
        
   int num_backtracks  = 0;
   int no_search_fails = bs.no_buckets() / 2;

   int_pattern new_pattern(bs.max_bucket_sz());

   while ( i < bs.no_buckets() && fails < no_search_fails ) {

        if ( bs[i] == 0 || bs[i] -> no_keys() == 0 ) { 
           i++; 
           continue; 
        };
   
        patternFit = -1;

//MESSAGE(cerr, "before fit");
        while ( bs.get_pattern(i, new_pattern, pms) == 0 ) {

//debug(cerr, new_pattern);
//cerr << new_pattern.no_elmts();
           patternFit = ht.fast_fit(new_pattern);
   
           if ( patternFit >= 0 ) {
//MESSAGE(cerr, " fit");
              bs[i] -> set_g_value(patternFit);
              break;
           } else {
//MESSAGE(cerr, " fail");
              fails++;
           }

       }
           
       if ( patternFit == -1 ) {
   
//MESSAGE(cerr, "BACKTRACK");
           if ( i <= 0 ) break; 
   
           i--;
           //fails = 0;
   
           if ( num_backtracks > bs.no_buckets() ) {
              return -2;
           } else 
              num_backtracks++;
   
       } else {
//MESSAGE(cerr, "increment i");
            i++; 
       }
   }

   return ( patternFit >= 0 ) ? 0 : -1;
}

int verify(buckets& bs, mphf_hash_table& ht, params& pms)
{
   int i;
   int_pattern new_pattern(bs.max_bucket_sz());

   //debug(cerr, ht.num_filled_slots());
   //debug(cerr, ht.no_slots());
   //debug(cerr, pms.n);

   if ( ht.num_filled_slots() != ht.no_slots() ) {

      MESSAGE(cerr, "panic: hash table not full or 'too' full");
      MESSAGE(cerr, form("filled_slots = %d\n", ht.num_filled_slots()));
      MESSAGE(cerr, form("no_slots = %d\n", ht.no_slots()));
      return -1;

   } 

   ht.clear();

   for ( i = 0; i < bs.no_buckets(); i++ ) {

     if ( bs[i] == 0 || bs[i] -> no_keys() == 0 ) continue;

       bs.use_current_params(i, new_pattern, pms, true);
//debug(cerr, bs[i] -> orig_pos());
//debug(cerr, new_pattern);

     if ( ht.fit_hash_table(new_pattern) != 0 ) {
        MESSAGE(cerr, "panic: collision occurred");
        return -1 ;
     }
   }

      //debug(cerr, ht.num_filled_slots());
      //debug(cerr, ht.no_slots());

   if ( ht.num_filled_slots() != ht.no_slots() ) {

      MESSAGE(cerr, "panic: hash table not full after test insertion");
      MESSAGE(cerr, form("filled_slots = %d\n", ht.num_filled_slots()));
      MESSAGE(cerr, form("no_slots = %d\n", ht.no_slots()));
      return -1;
   } else {
      MESSAGE(cerr, "verifying OK");
      return 0;
   }
}

int write_spec(buckets& bs, params& pms, buffer& mphf_buffer)
{
   unsigned int gv_bits = 0;

   if ( pms.v_n > 0 ) {
      gv_bits = (int)(flog2(pms.v_n)) + 1; /* bits of each g value.*/
      if ( floor(flog2(pms.v_n)) < flog2(pms.v_n) )
        gv_bits++;
   } 

   int uints_of_cmpat_gv = gv_bits * pms.v_b;

   if ( uints_of_cmpat_gv % BITS_IN(unsigned) > 0 ) 
     uints_of_cmpat_gv += BITS_IN(unsigned);

   uints_of_cmpat_gv /= BITS_IN(unsigned);


   unsigned int *c_array = new unsigned[uints_of_cmpat_gv];

   compact(bs, c_array, gv_bits, mphf_buffer.get_swap_order());

   unsigned int g_array_bytes = sizeof(unsigned int) * uints_of_cmpat_gv;

   int spec_bytes = 7 * ( sizeof(unsigned int) + 1) + g_array_bytes + 1;

   mphf_buffer.expand_chunk(spec_bytes);

/*
   mphf_buffer.put(pms.v_n);		mphf_buffer.put('\n');
   mphf_buffer.put(pms.v_b);		mphf_buffer.put('\n');
   mphf_buffer.put(pms.v_p1);		mphf_buffer.put('\n');
   mphf_buffer.put(pms.v_p2);		mphf_buffer.put('\n');
   mphf_buffer.put(pms.v_r);		mphf_buffer.put('\n');
   mphf_buffer.put(pms.v_seed);		mphf_buffer.put('\n');
   mphf_buffer.put(g_array_bytes);	mphf_buffer.put('\t');
   mphf_buffer.put((char*)c_array, g_array_bytes); mphf_buffer.put('\n');
*/

   ostringstream fout(mphf_buffer.get_base(), ios::out);

   fout << pms.v_n << "\n";
   fout << pms.v_b << "\n";
   fout << pms.v_p1 << "\n";
   fout << pms.v_p2 << "\n";

/*
   int new_v_r = pms.v_r; SET_BIT(new_v_r, 0x80000000);
   fout << new_v_r << "\n";
*/
   fout << pms.v_r << "\n";

   fout << pms.v_seed << "\n";

   fout << g_array_bytes << '\t';
   fout.write((char*)c_array, g_array_bytes);
   fout << '\n';


   mphf_buffer.set_content_sz(spec_bytes);
   memcpy(mphf_buffer.get_base(), fout.str().c_str(), spec_bytes);

   delete c_array;

   return 0;
}

int compact(buckets& bs, unsigned s[], int t, Boolean swap)
{
   int target, k, i, remaining_bits, branch;
   unsigned unsigned_g, high_part_bits, lower_part_bits;

   remaining_bits = BITS_IN(unsigned);
   k = target = 0;

   unsigned* y = new unsigned[bs.no_buckets()];
   for ( i = 0; i < bs.no_buckets(); y[i++] = 0 );

   for ( i = 0; i < bs.no_buckets(); i++ ) {

      if ( bs[i] && bs[i] -> no_keys() > 0 ) {
         y[bs[i] -> orig_pos()] = 
             ((bs[i] -> g_value()) << 1) + bs[i] -> control_bit();
/*
cerr << bs[i] -> orig_pos() << " ";
cerr << bs[i] -> g_value() << " ";
cerr << bs[i] -> control_bit() << " ";
cerr << y[bs[i] -> orig_pos()] << "\n";
*/
      } 
   }


/*
MESSAGE(cerr, "=======BIT ARRAY:");
debug(cerr, bs.no_buckets());
   for ( i = 0; i < bs.no_buckets(); i++ )
     cerr << i << " " << y[i] << "\n";
     cerr << "=======\n";
*/

//MESSAGE(cerr, "=======BIT ARRAY (before swap):");

   for ( i = 0; i < bs.no_buckets(); i++ ) {

     unsigned_g = y[i];

/*
debug(cerr, i);
debug(cerr, hex(unsigned_g));
*/
/*
debug(cerr, form("%x", c_bit));
debug(cerr, "=====");
*/

     if (remaining_bits >= t ) {
        unsigned_g <<= (remaining_bits -t);
        target |= unsigned_g;

        remaining_bits -= t;
	branch = 0;
     } else {
        high_part_bits = getbits(unsigned_g,t,remaining_bits);
        lower_part_bits  = unsigned_g & ~(~0 << t-remaining_bits);
        lower_part_bits <<= (BITS_IN(unsigned)- (t-remaining_bits));

        s[k++] =  target | high_part_bits;

#ifdef PORTABLE_DB
     if ( swap == true )
        ORDER_SWAP_UINT(s[k-1]);    
#endif

        target = lower_part_bits;
        remaining_bits = BITS_IN(unsigned) - ( t - remaining_bits );

	branch =1;
     }
   }

   if ( bs.no_buckets() > 0 ) {
      s[k] = ( branch == 0 ) ? target : lower_part_bits;
#ifdef PORTABLE_DB
     if ( swap == true )
        ORDER_SWAP_UINT(s[k]);    
#endif
   }

/*
MESSAGE(cerr, "=======BIT ARRAY (after swap):");
debug(cerr, k+1);
   for ( i = 0; i <= k; i++ )
     cerr << i << " " << s[i] << "\n";
cerr << "=======\n";
*/

   delete y;
   return 0;
}

int wc(char* file_name, unsigned int& lines, unsigned int& max_length)
{
   char	buf[BUFSIZ];

   fstream in(file_name, ios::in);

   if ( !in ) {
      MESSAGE(cerr, "can not open key file");
      throw(streamException(in.rdstate()));
   }

   lines = 0;
   max_length = 0;
   while ( in.getline(buf, BUFSIZ) )  {
     max_length = MAX(strlen(buf)-1, max_length);
     lines++;
   }

   if ( lines == 0 ) {
      MESSAGE(cerr, "empty key file");
      return -1;
   } else
      return 0;
}

