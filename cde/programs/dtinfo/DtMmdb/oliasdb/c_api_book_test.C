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
/* $XConsortium: c_api_book_test.C /main/5 1996/08/21 15:52:51 drk $ */

#include "oliasdb/DtMmdb.h"
#include <stdio.h>

extern "C"
void showBook(DtMmdbInfoRequest* req)
{
    const char* str = 0;
    DtMmdbHandle* id = 0;
    unsigned int l;
    int x ;
    DtMmdbHandle** ids;

    if ( req == 0 ) return ;

    str = DtMmdbBookGetShortTitle(req, &l);
    if ( str )
       fprintf(stderr, "        bookInfo: shortTitle=%s\n", str);

    str = DtMmdbBookGetLongTitle(req, &l);
    if ( str )
       fprintf(stderr, "        bookInfo: longTitle=%s\n", str);

    id = DtMmdbBookGetTocObjectId(req);
    if ( id )
        fprintf(stderr, "        bookInfo: TOC section id=%s\n", DtMmdbHandleToString(id));
    DtMmdbFreeHandle(id);

    x = DtMmdbBookGetSeqNum(req);
    fprintf(stderr, "        bookInfo: seqnum =%d\n", x);

    str =DtMmdbBookGetSeqLIcense(req, &l);
    if ( str )
       fprintf(stderr, "        bookInfo: license term=%s\n", str);

    ids = DtMmdbBookGetTabList(req, &l);
    if (ids) {
      for ( x=0; x<(int)l; x++ )
        fprintf(stderr, "        bookInfo: tab section id=%s\n", DtMmdbHandleToString(ids[x]));
    }
    DtMmdbFreeHandleList(ids);
}

/*  TOC section's oid ask key */
extern "C"
int test_book1(int argc, char** argv)
{
   int j = 0;
   int i = -1;
   DtMmdbInfoRequest* req = 0;

   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       req = newDtMmdbInfoRequestWithPrimaryOid(j, argv[4]);
       showBook(req);

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

/* book's oid as key */
extern "C"
int test_book2(int argc, char** argv)
{
   int j = 0;
   int i = -1;
   DtMmdbInfoRequest* req = 0;

   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       req = newDtMmdbInfoRequestWithSecondaryOid(j, argv[4]);
       showBook(req);

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

/* book's seq number as key */
extern "C"
int test_book3(int argc, char** argv)
{
   int j = 0;
   int i = -1;
   DtMmdbInfoRequest* req = 0;

   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       req = newDtMmdbInfoRequestWithSeqnum(j, argv[4]);
       showBook(req);

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

