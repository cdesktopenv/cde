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
 * $XConsortium: fast_mphf.cc /main/5 1996/07/18 14:35:57 drk $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
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


#define NUM_BITS_INCREASES 5

#include "index/fast_mphf.h"

#ifdef C_API
tbl_cache *fast_mphf::v_tbl_cache_ptr = 0;
#define v_tbl_cache (*v_tbl_cache_ptr)
#else
tbl_cache fast_mphf::v_tbl_cache;
#endif

tbl_record::~tbl_record()
{
   delete v_tbl0;
   delete v_tbl1;
}

tbl_cache::tbl_cache() : f_array(10)
{
}

tbl_cache::~tbl_cache()
{
   for ( int v=0; v<f_array.no_elmts(); v++ ) {
      delete (tbl_record*)f_array[v];
   }
}


void tbl_cache::init_table(int hash_tbl_sz, int seed, atoi_pearson*& t1, atoi_pearson*& t2)
{
   int x = f_array.no_elmts();
   tbl_record *y = 0;

   for ( int v=0; v<x; v++ ) {
      y = (tbl_record*)f_array[v];

      if ( y -> v_tbl0 != 0 && y -> v_seed == seed ) 
      {

       t1 = y -> v_tbl0;
       t2 = y -> v_tbl1;

/*
MESSAGE(cerr, "USE cached TABLE!");
debug(cerr, int(v_tbl0));
debug(cerr, int(v_tbl1));
*/
       return ;
     }
   }
   
   pm_random z(seed);
   t1 = new atoi_pearson(hash_tbl_sz, 128, z);
   t2 = new atoi_pearson(hash_tbl_sz, 128, z);

   y = new tbl_record(seed, t1, t2);
   f_array.insert(y, x);
   f_array.reset_elmts(x+1);
}
	
fast_mphf::fast_mphf(c_code_t c_cd): long_pstring(c_cd),
   v_long_string_core_indexed(false),
   v_no_ps(0), v_p1(0), v_p2(0), 
   r(0), v_seed(0), t(0)
{
#ifdef C_API
   if ( v_tbl_cache_ptr == 0 ) {
     v_tbl_cache_ptr = new tbl_cache;
   }
#endif

   v_tbl0 = 0;
   v_tbl1 = 0;
}

void fast_mphf::init_persistent_info(persistent_info* x)
{
   long_pstring::init_persistent_info(x);
   if ( get_mode(OLD_OBJECT) == false ) {
      v_hash_tbl_sz = 0;
      v_no_ps = 0;
      v_p1 = 0;
      v_p2 = 0;
      r = 0;
      v_seed = 0;
      t = 0;
      hash::init_data_member();
   }
}

fast_mphf::~fast_mphf()
{
/*
   delete v_tbl0;
   delete v_tbl1;

#ifdef C_API
   if ( v_tbl_cache_ptr ) {
      for (int i=0; i<10; i++ )
           delete v_tbl_cache_ptr[i];
      delete v_tbl_cache_ptr;
      v_tbl_cache_ptr = 0;
   }
#endif
*/
}

io_status fast_mphf::asciiIn(istream& in)
{
   in >> v_hash_tbl_sz;
   in >> v_no_ps;
   in >> v_p1;
   in >> v_p2;
   in >> r;
   in >> v_seed;

/*
MESSAGE(cerr, "in fast_mphf::asciiIn()");
debug(cerr, my_oid());
debug(cerr, v_hash_tbl_sz);
debug(cerr, v_no_ps);
debug(cerr, v_p1);
debug(cerr, v_p2);
debug(cerr, r);
debug(cerr, v_seed);
*/

   v_key_set_sz = v_hash_tbl_sz ;

   in.get(); // skip the '\n' after seed

   long_pstring::asciiIn(in);

   if ( v_key_set_sz > 0 ) {
      t = (int)(flog2(v_key_set_sz)) + 1; // bits of each g value.
      if ( floor(flog2(v_key_set_sz)) < flog2(v_key_set_sz) )
         t++;

//MESSAGE(cerr, "compacted array:");
//debug(cerr, long_pstring::size());

//for (int z=0; z<long_pstring::size()/4; z++) {
// cerr << ((unsigned*)long_pstring::get())[z] << " ";
//}
//cerr << "\n";


      v_hash_func_sz = v_no_ps*sizeof(unsigned)+128*2+6*sizeof(int);

      init_map_tbls();

   } else {
      v_hash_func_sz = 0;
      t = 0;
   }

   set_mode(UPDATE, true);

   return done;
}

Boolean fast_mphf::init_map_tbls()
{
/*
MESSAGE(cerr, "in fast_mphf::init_map_tbls()");
debug(cerr, (void*)this);
debug(cerr, my_oid());
debug(cerr, v_key_set_sz);
debug(cerr, v_hash_tbl_sz);
debug(cerr, int(this));
debug(cerr, v_no_ps);
debug(cerr, v_p1);
debug(cerr, v_p2);
debug(cerr, r);
debug(cerr, v_seed);
debug(cerr, t);
//debug(cerr, (void*)&v_tbl_cache);
*/



  if ( v_key_set_sz > 0 ) {
     v_tbl_cache.init_table(v_hash_tbl_sz, v_seed, v_tbl0, v_tbl1);
  }

   //long_pstring::init_run_data();

   return true;
}

int fast_mphf::hashTo(const key_type& k)
{
   unsigned int i;

   if ( v_long_string_core_indexed == false ) {
      v_long_string_core_indexed = true;
   }
/*
cerr << "\n";
MESSAGE(cerr, "fast_mphf:: hashTO()");
debug(cerr, k);
*/


   if ( v_hash_tbl_sz == 0 ) {
      throw(stringException("hash table empty"));
   }

   i = v_tbl0 -> atoi(k.get(), k.size(), r, v_key_set_sz); // for halmphf


   if ( i < v_p1 ) {
     i %= v_p2;
   } else {
     i %= v_no_ps - v_p2;
     i += v_p2;
   }

   int gv, c_bit;
   gValue(i, gv, c_bit);



   //i = v_tbl1 -> atoi(k, c_bit+r+1) + gv;
//debug(cerr, c_bit+r);

   i = v_tbl1 -> atoi(k.get(), k.size(), c_bit+r+1, v_hash_tbl_sz) + gv; // for halmphf


   return i % v_hash_tbl_sz;
}

int fast_mphf::gValue(int i, int& gvalue, int& ctl_bit) 
{
   if ( !INRANGE(i, 0, (int) v_no_ps-1) ) {
      throw(boundaryException(0, v_no_ps-1, i));
   }

   int a, b;
   unsigned un_compacted, un_compacted1;
	
   a = b = t * i ;
   a /=  BITS_IN(unsigned);
   b %=  BITS_IN(unsigned);

   unsigned value_at_a, value_at_a_plus; 
/*
debug(cerr, a);
debug(cerr, b);
debug(cerr, t);
*/

   char x_buf[10];

   long_pstring::extract(a*sizeof(a), (a+1)*sizeof(a), x_buf);

   memcpy((char*)&value_at_a, x_buf, sizeof(value_at_a));


//cerr << "Extract: at " << a << "; the int =" << value_at_a << endl;
#ifdef PORTABLE_DB
   if ( swap_order() == true )
      ORDER_SWAP_UINT(value_at_a);
#endif
//cerr << "after swap " << a << "; the int =" << value_at_a << endl;

//debug(cerr, hex(value_at_a));

	
   if ( BITS_IN(unsigned) - b >= t ) {

       un_compacted = getbits(value_at_a, BITS_IN(unsigned) - b, t);

   } else {

      long_pstring::extract((a+1)*sizeof(a), (a+2)*sizeof(a), x_buf);
	
      memcpy((char*)&value_at_a_plus, x_buf, sizeof(value_at_a_plus));

//cerr << "Extract+1: at " << (a+1) << "; the int =" << value_at_a_plus << endl;
#ifdef PORTABLE_DB
      if ( swap_order() == true )
         ORDER_SWAP_UINT(value_at_a_plus);
#endif
//cerr << "after swap " << (a+1) << "; the int =" << value_at_a_plus << endl;

//debug(cerr, hex(value_at_a_plus));

       un_compacted1 = 
         getbits(value_at_a, BITS_IN(unsigned) - b, BITS_IN(unsigned) - b);
       un_compacted = 
         getbits(value_at_a_plus, BITS_IN(unsigned), t - BITS_IN(unsigned) + b);
       un_compacted1 <<= ( t - BITS_IN(unsigned) + b);
       un_compacted |= un_compacted1;
   }
//debug(cerr, hex(un_compacted));

   ctl_bit = un_compacted & (unsigned)1;
   gvalue = un_compacted >> 1;
   return 0;
}

Boolean fast_mphf::build(const char *from)
{
   fstream in(from, ios::in);
   return build(in);
}


Boolean fast_mphf::build(istream& in)
{
   int ok = -1;

   sorter stor(in);

   params pms;

   pms.v_n = stor.no_unique_keys();

   pms.select_value();

   buffer mphf_spec(LBUFSIZ);
   //buffer& mphf_spec = get_store() -> aux_buf();
   mphf_spec.set_swap_order(swap_order());

   int i=0;
   while ( i<NUM_BITS_INCREASES && ok != 0 ) {

      ok = compute_a_mphf(stor.unique_keys(), pms, mphf_spec);

      switch (ok) {

          case 0:
             break;
   
          case 1:
             pms.re_select_value();
             break;
   
          case -1:
             throw(stringException("finding a mphf failed"));
      }

      i++;
   }

   if ( ok != 0 ) {
      set_mode(HEALTH, false);
      throw(stringException("finding a mphf failed"));
   }
   
   stringstream strin;

   if ( !strin ) {
      throw(streamException(strin.rdstate()));
   }
   else {
      strin.write(mphf_spec.get_base(), mphf_spec.content_sz());
   }

   asciiIn(strin);

   set_mode(HEALTH, true);

   return true;
}

void
fast_mphf::print_mapping(const char *key_file, int option) 
{
//debug(cerr, option);
   MESSAGE(cerr, "print_mapping()");

   char string[LBUFSIZ];
   fstream in(key_file, ios::in);

   if ( !in ) {
     throw(streamException(in.rdstate()));
   }

   char *hash_table = new char[v_hash_tbl_sz];
   for (unsigned int i = 0; i < v_hash_tbl_sz; hash_table[i++] = 0 );

   ostring lbuf(LBUFSIZ);


   while ( in.getline(string, LBUFSIZ, '\n') ) {

//     string[strlen(string)-1] = '\0';

//debug(cerr, string);
//debug(cerr, strlen(string));

     lbuf.reset();
     lbuf.set(string, strlen(string));

     int hash  = hashTo(lbuf) ;

     if ( option == 1 ) {
        cout << " string = " << string;
        cout  << ", hash = " << hash << "\n";
     }

     if ( hash_table[hash] == 1 ) 
        MESSAGE(cerr, "mapping_print(): panic: mphf hash collision");
     else
        hash_table[hash] = 1;
    
    in.getline(string, LBUFSIZ, '\n'); 
   }
  
   MESSAGE(cerr, "print_mapping() done");
}

void fast_mphf::print_tbls(ostream& out) 
{
   debug(out, *v_tbl0);
   debug(out, *v_tbl1);
}

void fast_mphf::print_gvalues(ostream& out) 
{
   int gv, cbit;
   for (unsigned int i = 0; i<v_no_ps; i++ ) {
      out << i;
      gValue(i, gv, cbit);
      out << " " <<  gv << " " << cbit << "\n";
   }
}

int fast_mphf::print_bits(unsigned x, ostream& out)
{
   for ( unsigned int i=0; i<8*sizeof(unsigned); i++ ) {
      if ( BIT_TEST(x, 0x80000000) )
        out << "1";
      else
        out << "0";
      x = x << 1;
   }
   out << "\n";
   return 0;
}


int fast_mphf::cdr_sizeof()
{
   return long_pstring::cdr_sizeof() + hash::cdr_sizeof() +
          6*sizeof(unsigned int);
}

io_status fast_mphf::cdrOut(buffer& buf)
{
   long_pstring::cdrOut(buf);
   hash::cdrOut(buf);

   buf.put(v_no_ps);
   buf.put(v_p1);
   buf.put(v_p2);
   buf.put(r);
   buf.put(v_seed);
   buf.put(t);

   return done;
}

io_status fast_mphf::cdrIn(buffer& buf)
{
   long_pstring::cdrIn(buf);
   hash::cdrIn(buf);

   buf.get(v_no_ps);
   buf.get(v_p1);
   buf.get(v_p2);
   buf.get(r);
   buf.get(v_seed);
   buf.get(t);

   init_map_tbls();

   return done;
}


   
MMDB_BODIES(fast_mphf)
HANDLER_BODIES(fast_mphf)
