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
// $XConsortium: c_api_locator.cc /main/3 1996/06/11 17:27:38 cde-hal $

#include "oliasdb/c_api_common.h"

static locator_smart_ptr*
getLocatorUsingLocAndProid(DtMmdbInfoRequest* request, Boolean checkLocOnly = false)
{
   mtry {
      info_base* x = getBookCase(request -> bookcase_descriptor);
      if ( x == 0 ) return 0;

      const char* str = getLocator(request);

      if ( str ) {
         return new locator_smart_ptr(x, str);
      } else
         return 0;

/*
      if ( checkLocOnly == true ) return 0;

      oid_t * id = getPrimiaryOid(request);

      if ( id == 0 ) return 0;

      return new locator_smart_ptr(x, *id);
*/
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char*
DtMmdbLocatorGetSectionLoc(DtMmdbInfoRequest* request)
{
   mtry {
      locator_smart_ptr* x = getLocatorUsingLocAndProid(request, false);

      if ( x == 0 ) return 0;
      const char* y = x -> inside_node_locator_str();
      delete x;
      return y;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

DtMmdbHandle*
DtMmdbLocatorGetSectionObjectId(DtMmdbInfoRequest* request)
{
   mtry {
      locator_smart_ptr* x = getLocatorUsingLocAndProid(request, true);
      if ( x == 0 ) return 0;
      DtMmdbHandle *z = newDtMmdbHandle(x -> node_id());
      delete x;
      return z;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

