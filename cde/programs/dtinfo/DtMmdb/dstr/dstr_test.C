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
/* $XConsortium: dstr_test.C /main/6 1996/08/21 15:56:38 drk $ */

#include "dstr/dstr_test.h"
#include "utility/pm_random.h"
#include "utility/funcs.h"

#ifdef REGRESSION_TEST

extern
int
bset_test(unsigned int in_cts, unsigned int out_cts, 
	  pm_random& rand_gen, unsigned int cycles);

int dstr_test(int argc, char** argv)
{
   if ( strcmp(argv[1], "bset_test") == 0 ) {
      if ( argc != 5 ) {
         cerr << "usage: bset_test in_count out_count num_cycles \n";
         cerr << "	where: \n";
         cerr << "	in_count: number of elements originally in the set\n";
         cerr << "	out_count: number of elements originally not in the set\n";
         cerr << "	num_cycles: number of tests\n";
         return 1;
      }   

      int in_cts = atoi(argv[2]);
      int out_cts = atoi(argv[3]);
      int cycles = atoi(argv[4]);

      struct timeval tp;
      struct timezone tzp;

      int seed = ( gettimeofday(&tp, &tzp) == 0 ) ? int(tp.tv_sec) : 19;

      pm_random rand_gen;
      rand_gen.seed(seed);

      return bset_test(in_cts, out_cts, rand_gen, cycles);
   } else
     return 2;
}

#endif
