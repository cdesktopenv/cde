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
// $XConsortium: c_api_dlp.cc /main/4 1996/06/11 17:27:07 cde-hal $

#include "oliasdb/c_api_common.h"

static dlp_smart_ptr*
getDlpUsingProid(DtMmdbInfoRequest* request)
{
   mtry {
      info_base* x = getBookCase(request -> bookcase_descriptor);
      if ( x == 0 ) return 0;

      oid_t * id = getPrimiaryOid(request);

      if ( id ) {
         return new dlp_smart_ptr(x, *id);
      }

      return 0;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

DtMmdbHandle*
DtMmdbDlpGetPrevSectionId(DtMmdbInfoRequest* request)
{
   mtry {
      dlp_smart_ptr* x = getDlpUsingProid(request);
      if ( x == 0 ) return 0;
      DtMmdbHandle *z = 0;

      if ( x -> prev_node_oid() == ground )
        z = DtMmdbGroundId;
      else
        z = newDtMmdbHandle(x -> prev_node_oid());

      delete x;
      return z;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

DtMmdbHandle*
DtMmdbDlpGetNextSectionId(DtMmdbInfoRequest* request)
{
   mtry {
      dlp_smart_ptr* x = getDlpUsingProid(request);
      if ( x == 0 ) return 0;

      DtMmdbHandle *z = 0;

      if ( x -> next_node_oid() == ground)
        z = DtMmdbGroundId;
      else
        z = newDtMmdbHandle(x -> next_node_oid());

      delete x;
      return z;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

