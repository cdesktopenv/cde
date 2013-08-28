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
/* $XConsortium: c_api_toc_test.C /main/5 1996/08/21 15:53:29 drk $ */

#include "oliasdb/DtMmdb.h"
#include <stdio.h>

extern "C"
void showTOC(DtMmdbInfoRequest* req)
{
   DtMmdbHandle* id = 0;
   unsigned int ct = 0;
   unsigned int i = 0;
   DtMmdbHandle** ids = 0;

   if ( req == 0 ) return ;

   id = DtMmdbTocGetParentId(req);
   if (id)
      fprintf(stderr, "        TOC: parentId=%s\n", DtMmdbHandleToString(id));
   DtMmdbFreeHandle(id);


   ids = DtMmdbTocGetChildIds(req, &ct);
   if ( ids ) {
     fprintf(stderr, "ct=%d\n", ct);
     for ( i=0; i<ct; i++ )
        fprintf(stderr, "        TOC: childid[%d]=%s\n", i, DtMmdbHandleToString(ids[i]
));
   }
   DtMmdbFreeHandleList(ids);

   i = DtMmdbTocGetNumOfChildren(req);
   fprintf(stderr, "        TOC: NumOfChildren=%d\n", i);
}

extern "C"
int test4(int argc, char** argv)
{
   int j = 0;
   int i = 0;
   DtMmdbInfoRequest* req = 0;

   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       req = newDtMmdbInfoRequestWithPrimaryOid(j, argv[4]);
       showTOC(req);

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


