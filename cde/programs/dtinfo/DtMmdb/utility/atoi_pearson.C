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
 * $XConsortium: atoi_pearson.cc /main/6 1996/06/11 17:35:45 cde-hal $
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



#include "utility/config.h"
#include "utility/atoi_pearson.h"

#define MASK 0xff

atoi_pearson::atoi_pearson(int r, int l): v_shared(false)
{
   pm_random rdm_generator;
   init(r, l, rdm_generator);
}

atoi_pearson::atoi_pearson(int r, int l, pm_random& rdm_generator) :
   v_shared(false)
{
   init(r, l, rdm_generator);
}

atoi_pearson::atoi_pearson(int r, int, char* shared_tbl) :
   v_entries(256), v_range(r), v_mask(0xff), 
   v_no_bytes(4), v_tbl(shared_tbl), v_shared(true)
{
}

void atoi_pearson::init(int r, int, pm_random& rdm_generator) 
{
   v_entries = 256;
   v_range = r; 
   v_mask = 0xff;
   v_no_bytes = 4;

   v_tbl = new char[v_entries];

   unsigned int i;
   for ( i = 0; i < v_entries; i++ )
      v_tbl[i] = i;

/*
MESSAGE(cerr, "atoi_pearson::init()");
   for ( i = 0; i < v_entries;  i++ ) 
     cerr << int(v_tbl[i]) << " ";
   cerr << "\n";
*/

   int l;
   for ( i = 0; i < v_entries - 1;  i++ ) {
      l = rdm_generator.rand() % ( v_entries - i ) + i;
      char_swap(v_tbl[l], v_tbl[i]);
   }

/*
MESSAGE(cerr, "atoi_pearson::init()");
   for ( i = 0; i < v_entries;  i++ ) 
     cerr << int(v_tbl[i]) << " ";
   cerr << "\n";
*/
}

atoi_pearson::~atoi_pearson()
{
   if ( v_shared == false )
      delete v_tbl;
}

struct reg_t {
#ifdef MMDB_BIG_ENDIAN
   unsigned b4: 8;
   unsigned b3: 8;
   unsigned b2: 8;
   unsigned b1: 8;
#endif

#ifdef MMDB_LITTLE_ENDIAN
   unsigned b1: 8;
   unsigned b2: 8;
   unsigned b3: 8;
   unsigned b4: 8;
#endif
};

union u_tag {
   struct reg_t chars_val;
   unsigned int hash_val;
} ;

int atoi_pearson::atoi(const key_type& k, int offset) const
{
   char* string = k.get();
   int l = k.size();
   return atoi((const char*)string, l, offset, 0);
}

int atoi_pearson::atoi(const char* string, int l, int offset, int rang ) const 
{
   u_tag reg ;

   reg.hash_val = 0;

   int x = string[0] + offset;
   reg.chars_val.b1 = v_tbl[x & MASK];
   reg.chars_val.b2 = v_tbl[(x+1) & MASK];

   if ( v_range > 65535 ) {
      reg.chars_val.b3 = v_tbl[(x + 2) & MASK];
      reg.chars_val.b4 = v_tbl[(x + 3) & MASK];
   }

   for ( int j= 1; j<l; j++ ) {
      reg.chars_val.b1 = v_tbl[ reg.chars_val.b1 ^ string[j] ];
      reg.chars_val.b2 = v_tbl[ reg.chars_val.b2 ^ string[j] ];
      if ( v_range > 65535 ) {
         reg.chars_val.b3 = v_tbl[( reg.chars_val.b3 ^ string[j] ) ];
         reg.chars_val.b4 = v_tbl[( reg.chars_val.b4 ^ string[j] ) ];
      }

   }



   return (rang == 0 ) ? 
             ( reg.hash_val % v_range ) 
           : 
             ( reg.hash_val % rang );

}

int atoi_pearson::atoi(const char* str, int offset, int rang ) const 
{
   return atoi(str, strlen(str), offset, rang);
}

ostream& operator<<(ostream& s, atoi_pearson& p)
{
   for ( unsigned int i = 0; i < p.v_entries ; i++ )
      s << int(p.v_tbl[i]) << " ";
   return s;
}

/* ########################################### */
/*                                             */
/* ########################################### */

/*
int atoi_sum::atoi(const char* string, int l) 
{
}

atoi_sum::size()
{
}

ostream& operator<<(ostream& s, atoi_sum& t)
{
   return s;
}
*/
