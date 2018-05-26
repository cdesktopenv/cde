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
/* $XConsortium: c_api_infolib_test.C /main/5 1996/08/21 15:53:07 drk $ */

#include "oliasdb/DtMmdb.h"
#include <stdio.h>


extern "C" void showBookCaseInfo(int i);

extern "C"
int showInfoLibInfo(int i)
{
   int n = 0;
   DtMmdbInfoLibInfo* info = DtMmdbInfoLibGetInfo(i);
   if ( info ) {
      if ( info -> path )
        fprintf(stderr, "infolibInfo: path= %s\n", info -> path);
      if ( info -> name )
        fprintf(stderr, "infolibInfo: name= %s\n", info -> name);
      fprintf(stderr, "infolibInfo: num_bookcases= %d\n", info -> num_bookcases);

      n = info -> num_bookcases;

      DtMmdbInfoLibFreeInfo(info);
   } else
      fprintf(stderr, "no infolibInfo\n");

   return n;
}

extern "C"
int test0(int argc, char** argv)
{
   int j = 0;
   int n = 0;
   int i = -1;
   switch ( argc ) {
     case 4:
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       n = showInfoLibInfo(i);
       for ( j=0; j<n; j++ )
          showBookCaseInfo(DtMmdbGetBookCaseByIndex(i, j));
       break;
     case 3:
       i = DtMmdbOpenInfoLib(argv[2], 0, DtMmdbFalse);
       showInfoLibInfo(i);
       n = showInfoLibInfo(i);
       for ( j=0; j<n; j++ )
          showBookCaseInfo(DtMmdbGetBookCaseByIndex(i, j));
       break;
     default:
       fprintf(stderr, "bad argument list\n");
   }

   fprintf(stderr, "infolib desc=%d\n", i);
   return 0;
}

extern "C"
int test1(int argc, char** argv)
{
   int j = 0;
   int n = 0;
   int i = -1;
   switch ( argc ) {
     case 5:
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       n = showInfoLibInfo(i);
       for ( j=0; j<n; j++ )
          showBookCaseInfo(DtMmdbGetBookCaseByLoc(i, argv[4]));
       break;
     default:
       fprintf(stderr, "bad argument list\n");
   }

   fprintf(stderr, "infolib desc=%d\n", i);
   return 0;
}

