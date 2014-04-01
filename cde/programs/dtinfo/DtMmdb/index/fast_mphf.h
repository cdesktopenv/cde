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
 * $XConsortium: fast_mphf.h /main/5 1996/07/18 14:36:28 drk $
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


#ifndef _fast_mphf_h
#define _fast_mphf_h 1

#include <sys/wait.h>
#include "index/hash.h"
#include "utility/atoi_pearson.h"
#include "object/long_pstring.h"
#include "utility/xtime.h"
#include "hmphf/mphf_funcs.h"
#include "hmphf/sorter.h"


///////////////////////////////////////////////////////////////
// A fast MPHF contruct method proposed by Chen Qi Fan in 4/91.
// Usually it will compute MPHFs using 2-3 bits/key.
// Random number table size is reduced to 128 chars.
// Computation is fast due to the pattern matching 
// used in the searching stage.
//
// Reference: VaTech Technical Report TR92-2, SIGIR92 paper.
//
// Modification: 
//		convert to mmdb version (task started on 9-15-92)
///////////////////////////////////////////////////////////////

class tbl_record 
{

public:
   int v_seed;
   atoi_pearson* v_tbl0;
   atoi_pearson* v_tbl1;

public:
   tbl_record(int sd = 0, atoi_pearson* t1 =0, atoi_pearson* t2 =0) : 
	v_seed(sd), v_tbl0(t1), v_tbl1(t2) {};
   ~tbl_record();
};

class tbl_cache
{
protected:
    void_ptr_array f_array;

public:
 
   tbl_cache();
   ~tbl_cache();

   void init_table(int hash_table_sz, int seed, atoi_pearson*&, atoi_pearson*&);

   friend class fast_mphf;
};

class fast_mphf : public long_pstring, public ihash
{

public:

   fast_mphf(c_code_t = FAST_MPHF_CODE);
   virtual ~fast_mphf();

   MMDB_SIGNATURES(fast_mphf);

   void init_persistent_info(persistent_info*);


// init the two ascii->integer map tables 
   Boolean init_map_tbls();

// load the MPHF from files
   virtual io_status asciiIn(istream&);

// compute a hash value for a key.
   virtual int hashTo(const key_type&);

// compute a MPHF
   virtual Boolean build(const char* data_path);
   virtual Boolean build(istream& data_stream);

// show the mapping from keys to hash values and verify the mphf.
// option = 0: no print, only check;
// option = 1: print and check.
   void print_mapping( const char*key_file, int option = 0) ;

   void print_gvalues(ostream&out = cerr) ;

   virtual int cdr_sizeof();
   virtual io_status cdrOut(buffer&);
   virtual io_status cdrIn(buffer&);

protected:

// return the ith g value from the g array (in packed form)
   int gValue(int, int& gvalue, int& ctl_bit) ;

   void print_tbls(ostream&out = cerr) ;
   int print_bits(unsigned, ostream& = cout);

protected:

#ifdef C_API
   static tbl_cache *v_tbl_cache_ptr;
#else
   static tbl_cache v_tbl_cache;
#endif

   atoi_pearson *v_tbl0 ,  // table1
                *v_tbl1 ;  // table2

   Boolean v_long_string_core_indexed;

   unsigned int v_no_ps,  // number of partitions (buckets)
                v_p1, v_p2, // parameters p1 and p2.
                r,
                v_seed,
                t;

};

HANDLER_SIGNATURES(fast_mphf)

#endif
