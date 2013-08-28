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
/* $XConsortium: store_test.C /main/7 1996/08/21 15:56:52 drk $ */

#include <sys/time.h>
#include <sys/times.h>
#include "utility/pm_random.h"
#include "storage/page_storage.h"

////////////////////////////
// case: store pages exist
////////////////////////////
void real_page_cache_test_1(pm_random& rand_gen, page_storage** st, unsigned int ct, unsigned int no_access)
{
   unsigned int j, k;
   page_storage::access_t l;

   for ( unsigned int i=0; i< no_access; i++)
   {

      j = rand_gen.rand() % ct; // pick the store
      k = rand_gen.rand() % (st[j] -> pages()) + 1; // pick the page

// pick the READ/WRITE option

      l = (rand_gen.rand_01()>0.5) ? page_storage::READ : page_storage::WRITE;

      cerr << "store=" << st[j] -> my_name() << " ";
      cerr << "page=" << k << " ";
      if (l==page_storage::READ) 
         cerr << "read" ;
      else
         cerr << "write";
      cerr << "\n";

      (*st[j])(k, l);
   }
}

page_storage** 
prepare_store(char* path, lru& open_file_policy, 
	      pm_random& rand_gen, unsigned int ct, unsigned int low, unsigned int high)
{
   char name[256];

   page_storage** x = new page_storagePtr[ct];
   unixf_storage*  unix_file = 0;

   for ( unsigned int i=0; i<ct; i++) {
      int pages = rand_gen.rand() % (high-low) + low;

/*
if ( i == 0 )
   pages = 2;
if ( i == 1 )
   pages = 17;
*/

      snprintf(name, sizeof(name), "test.%d", i);

      if ( exist_file(name) == true )
         del_file(name);

      unix_file = new unixf_storage(path, name, &open_file_policy);
      x[i] = new page_storage(path, name, unix_file, 1024);
      x[i] -> add_page_frames(pages);
   }
   return x;
}
   
void quit_store(page_storage** st, unsigned int ct)
{
   for ( unsigned int i=0; i<ct; i++) {
     delete st[i];
   }
   delete st;
}

int page_cache_test_1(int argc, char** argv)
{
   if ( argc != 7 ) {
      cerr << "usage: page_cache_test1 db_path stores min_pages max_pages no_probes\n";
      cerr << "where \n";
      cerr << "	db_path: a path where the test dbs will be created;\n";
      cerr << "	stores: number of stores;\n";
      cerr << "	min_pages: min number of pages in these stores;\n";
      cerr << "	max_pages: max number of pages in these stores;\n";
      cerr << "	no_probes: number of probes to check the page cache.\n";
      return 1;
   }

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

   char* path = argv[2];

   if ( check_and_create_dir(path) != true )
     return -1;

   lru open_file_policy(20, 1000, false);

   unsigned int ct = atoi(argv[3]);
   unsigned int low = atoi(argv[4]); if ( low == 0 ) low = 1;

   unsigned int high = atoi(argv[5]);
   unsigned int no_access= atoi(argv[6]);

   page_storage** st =
        prepare_store(path, open_file_policy, rand_gen, ct, low, high);

   real_page_cache_test_1(rand_gen, st, ct, no_access);

   quit_store(st, ct);

   return 0;
}

int store_test(int argc, char** argv)
{
   if ( strcmp(argv[1], "page_cache_test_1") == 0 )
     return page_cache_test_1(argc, argv);
   else
     return 2;
}

