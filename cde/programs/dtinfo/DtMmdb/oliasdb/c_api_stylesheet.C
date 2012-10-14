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
// $XConsortium: c_api_stylesheet.cc /main/3 1996/06/11 17:28:03 cde-hal $

#include "oliasdb/c_api_common.h"

static stylesheet_smart_ptr*
getStylesheetUsingLocAndProid(DtMmdbInfoRequest* request, Boolean checkPIDOnly = false)
{
   mtry {
      info_base* x = getBookCase(request -> bookcase_descriptor);
      if ( x == 0 ) return 0;

      oid_t * id = getPrimiaryOid(request);

      if ( id )
         return new stylesheet_smart_ptr(x, *id);

      if ( checkPIDOnly == true ) return 0;

      const char* str = getLocator(request);

      if ( str ) {
         return new stylesheet_smart_ptr(x, str);
      } else
         return 0;


   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char*
DtMmdbStylesheetGetName(DtMmdbInfoRequest* request)
{
   mtry {
      stylesheet_smart_ptr* x = getStylesheetUsingLocAndProid(request, true);

      if ( x == 0 ) return 0;
      const char* y = x -> name();
      delete x;
      return y;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char*
DtMmdbStylesheetGetata(DtMmdbInfoRequest* request, unsigned int* data_length)
{
   mtry {
      stylesheet_smart_ptr* x = getStylesheetUsingLocAndProid(request, false);

      if ( x == 0 ) return 0;

#ifdef DEBUG
      fprintf(stderr, " stylesheetInfo: mmdb_oid = %d.%d\n", (int)(*x).its_oid().ccode()
, (int)(*x).its_oid().icode());
#endif

      const char* y = x -> online_data();

      if ( data_length ) *data_length = strlen(y);

      delete x;
      return y;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

