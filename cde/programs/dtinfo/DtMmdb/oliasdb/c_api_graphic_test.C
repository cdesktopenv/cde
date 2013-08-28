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
/* $XConsortium: c_api_graphic_test.C /main/5 1996/08/21 15:53:03 drk $ */


#include "oliasdb/DtMmdb.h"
#include <stdio.h>

extern "C"
void showGraphic(DtMmdbInfoRequest* req)
{
   const char* str;
   unsigned int l;
   unsigned int i;
   DtMmdbGraphicInfo* info ;

   if ( req == 0 ) return ;

   str = DtMmdbGraphicGetData( req, &l);
   if ( str ) {
      fprintf(stderr, "  GraphicInfo: data=");
      for (i=0; i<l; i++ )
        fprintf(stderr, "%c", str[i]);
      fprintf(stderr, "\n");
   }

   info = DtMmdbGraphicGetInfo(req);
   if ( info ) {
     fprintf(stderr, "	GraphicInfo: type=%d\n", info -> type);
     fprintf(stderr, "	GraphicInfo: width=%d\n", info -> width);
     fprintf(stderr, "	GraphicInfo: height=%d\n", info -> height);
     fprintf(stderr, "	GraphicInfo: llx=%d\n", info -> llx);
     fprintf(stderr, "	GraphicInfo: lly=%d\n", info -> lly);
     fprintf(stderr, "	GraphicInfo: urx=%d\n", info -> urx);
     fprintf(stderr, "	GraphicInfo: ury=%d\n", info -> ury);
     DtMmdbFreeGraphicInfo(info);
   }

/*
   str =  DtMmdbGraphicGetLoc(req);
     fprintf(stderr, "	GraphicInfo: graphic's locator=%s\n", str);
*/

}

extern "C"
int test_graphic1(int argc, char** argv)
{
   int j = 0;
   int i = 0;
   DtMmdbInfoRequest* req;

   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       req = newDtMmdbInfoRequestWithLoc(j, argv[4]);
       showGraphic(req);

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
int test_graphic2(int argc, char** argv)
{
   int j = 0;
   int i = 0;
   DtMmdbInfoRequest* req;

   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       req = newDtMmdbInfoRequestWithPrimaryOid(j, argv[4]);
       showGraphic(req);

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
