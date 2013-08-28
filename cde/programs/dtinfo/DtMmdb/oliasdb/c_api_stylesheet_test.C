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
/* $XConsortium: c_api_stylesheet_test.C /main/5 1996/08/21 15:53:25 drk $ */

#include "oliasdb/DtMmdb.h"
#include <stdio.h>

extern "C"
void showStyleSheet(DtMmdbInfoRequest* req)
{
   const char* str ;
   unsigned int l;

   if ( req == 0 ) return ;

   str = DtMmdbStylesheetGetName(req); 
   if ( str )
     fprintf(stderr, "  StylesheetInfo: name =%s\n", str);

   str = DtMmdbStylesheetGetata(req, &l);
   if ( str ) {
     fprintf(stderr, "  StylesheetInfo: data =%s\n", str);
     fprintf(stderr, "  StylesheetInfo: length =%d\n", l);
   }
}

extern "C"
int test_ss1(int argc, char** argv)
{
   int j = 0;
   int i = 0;
   DtMmdbInfoRequest* req ;

   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       req = newDtMmdbInfoRequestWithLoc(j, argv[4]);
       showStyleSheet(req);

       DtMmdbFreeInfoRequest(req);
       DtMmdbCloseInfoLib(i);

       break;
      }
      default:
       fprintf(stderr, "invalid arguments\n");
   }

   fprintf(stderr, "infolib desc=%d\n", i);
   return 0;
}


extern "C"
int test_ss2(int argc, char** argv)
{
   int j = 0;
   int i = 0;
   DtMmdbInfoRequest* req ;

   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       req = newDtMmdbInfoRequestWithPrimaryOid(j, argv[4]);
       showStyleSheet(req);

       DtMmdbFreeInfoRequest(req);
       DtMmdbCloseInfoLib(i);

       break;
      }
      default:
       fprintf(stderr, "invalid arguments\n");
   }

   fprintf(stderr, "infolib desc=%d\n", i);
   return 0;
}



