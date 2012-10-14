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
// $XConsortium: c_api_toc.cc /main/4 1996/06/11 17:28:18 cde-hal $

#include "oliasdb/c_api_common.h"

static toc_smart_ptr*
getTocUsingProid(DtMmdbInfoRequest* request)
{
   mtry {
      info_base* x = getBookCase(request -> bookcase_descriptor);
      if ( x == 0 ) return 0;

      oid_t * id = getPrimiaryOid(request);

      if ( id == 0 ) return 0;

      return new toc_smart_ptr(x, *id);
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}


DtMmdbHandle*
DtMmdbTocGetParentId(DtMmdbInfoRequest* request)
{
   mtry {
      toc_smart_ptr* x = getTocUsingProid(request);
      if ( x == 0 ) return 0;

      DtMmdbHandle *z = 0;

      if ( x -> parent() == ground )
        z = DtMmdbGroundId;
      else
        z = newDtMmdbHandle(x -> parent());

      delete x;
      return z; 
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

DtMmdbHandle**
DtMmdbTocGetChildIds(
        DtMmdbInfoRequest* request,
        unsigned int* list_length
        )
{
   mtry {
      toc_smart_ptr* x = getTocUsingProid(request);
      if ( x == 0 ) return 0;

      oid_list_handler* y = x -> children();
      if ( y == 0 ) return 0;

      int count = (*y) -> count();
//fprintf(stderr, "count=%d\n", count);

      DtMmdbHandle** z = (DtMmdbHandle**)malloc(sizeof(DtMmdbHandle*) * (count+1));

//fprintf(stderr, "z=%x\n", (void*)z);
      if ( z == 0 ) return 0;
    
      int i;
      for (i=0; i<count; i++) {
         z[i] = newDtMmdbHandle(oid_t(OLIAS_NODE_CODE, ((*y) -> operator()(i+1)).icode()));
      }

      z[i] = 0;

      delete y;
      delete x;

      if ( list_length ) 
         *list_length = count;

      return z;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

int DtMmdbTocGetNumOfChildren( DtMmdbInfoRequest* request)
{
   mtry {
      toc_smart_ptr* x = getTocUsingProid(request);
      if ( x == 0 ) return -1;
      int y = x -> subtree_size();
      delete x;
      return y; 
   }

   mcatch (mmdbException &,e)
   {
     return -1;
   } end_try;
     return -1;
}

