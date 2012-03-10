/* $XConsortium: dstr_test.C /main/6 1996/08/21 15:56:38 drk $ */

#include "dstr/dstr_test.h"
#include "utility/pm_random.h"
#include "utility/funcs.h"

#if defined(__uxp__) 
#include <sys/times.h>
#elif __osf__
#include <sys/time.h>
#endif

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

#ifdef __uxp__
      int seed;
      struct tms tp;
      if ((seed = (int)times(&tp)) < 0)
	seed = 19;
#else
      struct timeval tp;
      struct timezone tzp;

      int seed = ( gettimeofday(&tp, &tzp) == 0 ) ? int(tp.tv_sec) : 19;
#endif
      pm_random rand_gen;
      rand_gen.seed(seed);

      return bset_test(in_cts, out_cts, rand_gen, cycles);
   } else
     return 2;
}

#endif
