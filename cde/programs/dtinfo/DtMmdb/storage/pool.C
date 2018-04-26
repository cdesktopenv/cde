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
/*
 * $XConsortium: pool.cc /main/3 1996/06/11 17:34:18 cde-hal $
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


#include "storage/memory_pool.h"
#include "utility/xtime.h"

#define NUMS 100000
#define SIZE 20

pool_test1()
{
MESSAGE(cerr, "pool alloc/free test1");
   memory_pool x;

   char* ptrs[NUMS];

   for ( int i=0; i<NUMS; i++ ) {
       ptrs[i] = x.alloc(SIZE);
   }

MESSAGE(cerr, "alloc done!");
  
   for ( i=0; i<NUMS; i++ ) {
       x.free(ptrs[i]);
   }
MESSAGE(cerr, "free done!");
}

pool_test2()
{
MESSAGE(cerr, "pool alloc/free test2");
   memory_pool x;

   char* ptrs[100];

   for ( int i=0; i<NUMS/100; i++ ) {

      for ( int j=0; j<100; j++ ) {
          ptrs[j] = x.alloc(SIZE);
      }


      for ( j=0; j<100; j++ ) {
          x.free(ptrs[j]);
      }
   }

}


new_delete_test1()
{
MESSAGE(cerr, "new/delete test1");
   char* ptrs[NUMS];

   for ( int i=0; i<NUMS; i++ ) {
       ptrs[i] = new char[SIZE];
   }

MESSAGE(cerr, "alloc done!");
 
   for ( i=NUMS-1; i>=0; i-- ) {
       delete ptrs[i];
   }
MESSAGE(cerr, "free done!");
}

new_delete_test2()
{
MESSAGE(cerr, "new/delete test2");
   char* ptrs[100];

   for ( int i=0; i<NUMS/100; i++ ) {

      for ( int j=0; j<100; j++ ) {
          ptrs[j] = new char[SIZE];
      }


      for ( j=0; j<100; j++ ) {
          delete ptrs[j];
      }
   }
}


main( int argc, char** argv )
{
   assert ( argc == 2 );

   int ok;
   float cpul
   long elap;
   xtime tm;

   tm.snap_shot();

   if ( strcmp(argv[1], "-pool1") == 0 ) {
      pool_test1();
      ok = 0;
   } else
   if ( strcmp(argv[1], "-pool2") == 0 ) {
      pool_test2();
      ok = 0;
   } else
   if ( strcmp(argv[1], "-new1") == 0 ) {
      new_delete_test1();
      ok = 0;
   } else
   if ( strcmp(argv[1], "-new2") == 0 ) {
      new_delete_test2();
      ok = 0;
   }
   else {
      MESSAGE(cerr, "apply a param: -pool[1|2] or -new[1|2]");
      ok = -1;
   }

   tm.duration(cpu, elap);
   debug(cerr, cpu);
   debug(cerr, elap);

   return ok;
}
