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
// $XConsortium: c_api_graphic.cc /main/3 1996/06/11 17:27:16 cde-hal $

#include "oliasdb/c_api_common.h"

static graphic_smart_ptr*
getGraphicUsingLocAndProid(DtMmdbInfoRequest* request, Boolean checkLocOnly = false)
{
   mtry {
      info_base* x = getBookCase(request -> bookcase_descriptor);
      if ( x == 0 ) return 0;

      const char* str = getLocator(request);

      if ( str ) {
         return new graphic_smart_ptr(x, str);
      }

      if ( checkLocOnly == true ) return 0;

      oid_t * id = getPrimiaryOid(request);

      if ( id == 0 ) return 0;

      return new graphic_smart_ptr(x, *id);
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char*
DtMmdbGraphicGetData(DtMmdbInfoRequest* request, unsigned int* data_length)
{
   mtry {
      graphic_smart_ptr* x = getGraphicUsingLocAndProid(request, true);

      if ( x == 0 ) return 0;

#ifdef DEBUG
      fprintf(stderr, " graphicInfo: mmdb_oid = %d.%d\n", (int)(*x).its_oid().ccode(), (int)(*x).its_oid().icode());
#endif

      const char* y = x -> data();
      if ( data_length ) *data_length = x -> data_size();

      delete x;
      return y;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

DtMmdbGraphicInfo*
DtMmdbGraphicGetInfo(DtMmdbInfoRequest* request)
{

   mtry {
      graphic_smart_ptr* x = getGraphicUsingLocAndProid(request, true);

      if ( x == 0 ) return 0;



      DtMmdbGraphicInfo* z = newDtMmdbGraphicInfo();

      if ( z == 0 ) return 0;

      z -> type =  x -> coding();
      z -> width = x -> width();
      z -> height = x -> height();
      z -> llx =  x -> llx();
      z -> lly =  x -> lly();
      z -> urx =  x -> urx();
      z -> ury =  x -> ury();

      delete x;

      return z;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char* DtMmdbGraphicGetLoc(DtMmdbInfoRequest* request)
{
   mtry {
      graphic_smart_ptr* x = getGraphicUsingLocAndProid(request, true);

      if ( x == 0 ) return 0;

      const char* y = x -> locator();

      delete x;
      return y;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

void DtMmdbFreeGraphicInfo(DtMmdbGraphicInfo* x)
{
   free((void*)x);
}
