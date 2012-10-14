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
// $XConsortium: c_api_section.cc /main/3 1996/06/11 17:27:53 cde-hal $

#include "oliasdb/c_api_common.h"

const char* 
DtMmdbSectionGetLoc(DtMmdbInfoRequest* request)
{
   mtry {
      info_base* x = getBookCase(request -> bookcase_descriptor);
      if ( x == 0 ) return 0;
   
      oid_t * id = getPrimiaryOid(request); 
   
      if ( id == 0 ) return 0;
   
      node_smart_ptr y(x, *id);
   
      return y.locator();
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

static node_smart_ptr* 
getSectionUsingLocAndProid(DtMmdbInfoRequest* request)
{
   mtry {
      info_base* x = getBookCase(request -> bookcase_descriptor);
      if ( x == 0 ) return 0;

      const char* str = getLocator(request);

      if ( str ) {
         return new node_smart_ptr(x, str);
      }

      oid_t * id = getPrimiaryOid(request);

      if ( id == 0 ) return 0;

      return new node_smart_ptr(x, *id);
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char* 
DtMmdbSectionGetLongTitle(
	DtMmdbInfoRequest* request,
	unsigned int* title_length
			 )
{
   mtry {
      node_smart_ptr* x = getSectionUsingLocAndProid(request);

      if ( x == 0 ) return 0;

#ifdef DEBUG
      fprintf(stderr, "	mmdb_oid = ");
      (*x).its_oid().asciiOut(cerr); cerr << "\n";
      cerr.flush();
#endif

      const char* y = x -> long_title();

      if ( title_length ) *title_length = strlen(y);
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
DtMmdbSectionGetShortTitle(
	DtMmdbInfoRequest* request,
	unsigned int* title_length
			  )
{
   mtry {
      node_smart_ptr* x = getSectionUsingLocAndProid(request);

      if ( x == 0 ) return 0;
      const char* y = x -> short_title();

      if ( title_length ) *title_length = strlen(y);
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
DtMmdbSectionGetData
	(
	DtMmdbInfoRequest* request,
	unsigned int* data_length
	)
{
   mtry {
      node_smart_ptr* x = getSectionUsingLocAndProid(request);

      if ( x == 0 ) return 0;
      const char* y = x -> data();

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

int DtMmdbSectionGetDataSize(DtMmdbInfoRequest* request)
{
   mtry {
      node_smart_ptr* x = getSectionUsingLocAndProid(request);

      if ( x == 0 ) return -1;

      int y = x -> data_size();

      delete x;
      return y;
   }

   mcatch (mmdbException &,e)
   {
     return -1;
   } end_try;

     return -1;
}
	
const char* DtMmdbSectionGetTocLoc(DtMmdbInfoRequest* request)
{
   mtry {
      node_smart_ptr* x = getSectionUsingLocAndProid(request);

      if ( x == 0 ) return 0;
      const char* y = x -> toc_node_loc();

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
DtMmdbSectionGetBookId(DtMmdbInfoRequest* request)
{
   mtry {
      node_smart_ptr* x = getSectionUsingLocAndProid(request);

      if ( x == 0 ) return 0;

      DtMmdbHandle *z = newDtMmdbHandle(x -> doc_id());

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
DtMmdbSectionGetStyleSheetId(DtMmdbInfoRequest* request)
{
   mtry {
      node_smart_ptr* x = getSectionUsingLocAndProid(request);

      if ( x == 0 ) return 0;

      DtMmdbHandle *z = newDtMmdbHandle(x -> stylesheet_id());

      delete x;
      return z;
   }

   mcatch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

