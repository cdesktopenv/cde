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
// $XConsortium: c_api_test.cc /main/4 1996/06/11 17:28:13 cde-hal $

#include "oliasdb/DtMmdb.h"
#include "oliasdb/mmdb.h"
#include "oliasdb/c_api_common.h"
#include <stdio.h>
#include <stdlib.h>


void showBookCaseInfo(int i);

int showInfoLibInfo(int i)
{
   int n = 0;
   int j;
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

void showBookCaseInfo(int i)
{
   if ( i <= -1 ) {
      fprintf(stderr, "bad bookcase descriptor %d\n", i);
   } else {
      DtMmdbBookCaseInfo* info = DtMmdbBookCaseGetInfo(i);
      if ( info ) {
         if ( info -> name )
           fprintf(stderr, "	bookcaseInfo: name= %s\n", info -> name);
         fprintf(stderr, "	bookcaseInfo: num_bookcases= %d\n", info -> num_books);
         DtMmdbBookCaseFreeInfo(info);
      } else
         fprintf(stderr, "no bookcase info\n");
   }
}

// automatica test cases 

extern int auto_test(int argc, char** argv, OLIAS_DB& db);

extern "C"
int auto_test_c_api(int argc, char** argv)
{
   OLIAS_DB db;
   return auto_test(argc, argv, db);
}

// test cases that require manual inspection
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
       fprintf(stderr, "bad argment list\n");
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
       fprintf(stderr, "bad argment list\n");
   }

   fprintf(stderr, "infolib desc=%d\n", i);
   return 0;
}

DtMmdbInfoRequest* newDtMmdbInfoRequestWithLoc(int bc_id, char* loc)
{
   DtMmdbInfoRequest* x = 
 	(DtMmdbInfoRequest*) malloc(sizeof(DtMmdbInfoRequest));

   
   x -> bookcase_descriptor = bc_id ;
   x -> locator_ptr = loc;
   return x;
}

DtMmdbInfoRequest* newDtMmdbInfoRequestWithPrimaryOid(int bc_id, char* oid_str)
{
   DtMmdbInfoRequest* x = 
 	(DtMmdbInfoRequest*) malloc(sizeof(DtMmdbInfoRequest));

   
   x -> bookcase_descriptor = bc_id ;
   x -> primary_oid = newDtMmdbHandle(oid_t(oid_str, true, false));
   return x;
}

char* DtMmdbHandleToString(DtMmdbHandle* x)
{
   static char buf[100];
   if ( x -> oid_ptr) {
      oid_t *z = (oid_t*)(x -> oid_ptr);
      snprintf(buf, sizeof(buf), "%d.%d", z -> ccode(), z -> icode());
   } else
     buf[0] = 0;

   return buf;
}

void showSection(DtMmdbInfoRequest* req)
{
   if ( req == 0 ) return ;

    const char* str = DtMmdbSectionGetLongTitle(req, 0);
    if (str) {
    fprintf(stderr, "        SectionInfo: LongTitle=%s\n", str);
    }

    str = DtMmdbSectionGetShortTitle(req, 0);
    if (str) {
    fprintf(stderr, "        SectionInfo: ShortTitle=%s\n", str);
    }

    str = DtMmdbSectionGetData(req, 0);
    if (str) {
    fprintf(stderr, "        SectionInfo: data=%s\n", str);
    }

    int size = DtMmdbSectionGetDataSize(req);
    fprintf(stderr, "        SectionInfo: dataSize=%d\n", size);


    str = DtMmdbSectionGetTocLoc(req);
    if (str) {
    fprintf(stderr, "        SectionInfo: TocLoc=%s\n", str);
    }

    DtMmdbHandle* id = DtMmdbSectionGetBookId(req);
    if (id) {
    fprintf(stderr, "        SectionInfo: BookId=%s\n", DtMmdbHandleToString(id));
    }

    id = DtMmdbSectionGetStyleSheetId(req);
    if (id) {
    fprintf(stderr, "        SectionInfo: StyleSheetId=%s\n", DtMmdbHandleToString(id));
    }
}

void showTOC(DtMmdbInfoRequest* req)
{
   if ( req == 0 ) return ;

   DtMmdbHandle* id = DtMmdbTocGetParentId(req);
   if (id) 
      fprintf(stderr, "        TOC: parentId=%s\n", DtMmdbHandleToString(id));
   

   unsigned int ct = 0;
   DtMmdbHandle** ids = DtMmdbTocGetChildIds(req, &ct);
   if ( ids ) {
     fprintf(stderr, "ct=%d\n", ct);
     for ( int i=0; i<ct; i++ )
        fprintf(stderr, "        TOC: childid[%d]=%s\n", i, DtMmdbHandleToString(ids[i]));
   }

   int i = DtMmdbTocGetNumOfChildren(req);
   fprintf(stderr, "        TOC: NumOfChildren=%d\n", i);
}

extern "C"
int test2(int argc, char** argv)
{
   int j = 0;
   int n = 0;
   int i = -1;
   const char* str;
   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByLoc(i, argv[4]);

       DtMmdbInfoRequest* req = newDtMmdbInfoRequestWithLoc(j, argv[4]);
       showSection(req);
       break;
      }
     default:
       fprintf(stderr, "invalid arguments\n");
   }

   fprintf(stderr, "infolib desc=%d\n", i);
   return 0;
}

extern "C"
int test3(int argc, char** argv)
{
   int j = 0;
   int n = 0;
   int i = -1;
   const char* str;
   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       DtMmdbInfoRequest* req = newDtMmdbInfoRequestWithPrimaryOid(j, argv[4]);
       showSection(req);
       break;
      }
      default:
       fprintf(stderr, "invalid arguments\n");
   }

   fprintf(stderr, "infolib desc=%d\n", i);
   return 0;
}

extern "C"
int test4(int argc, char** argv)
{
   int j = 0;
   int i = 0;
   const char* str;
   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       DtMmdbInfoRequest* req = newDtMmdbInfoRequestWithPrimaryOid(j, argv[4]);
       showTOC(req);
       break;
      }
      default:
       fprintf(stderr, "invalid arguments\n");
   }

   fprintf(stderr, "infolib desc=%d\n", i);
   return 0;
}




