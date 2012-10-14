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
/* $XConsortium: c_api_bookcase.cc /main/5 1996/07/18 16:00:51 drk $ */

#include "oliasdb/c_api_common.h"

//extern void_ptr_array* infolib_array;
extern void_ptr_array* bookcase_array;
//extern olias_server* oserver_ptr;
extern OLIAS_DB* mmdb_ptr;


///////////////////////////////////////////////////////////////

int
DtMmdbGetBookCaseByName(int infolib_descriptor, const char* name)
{
   mtry {
      info_lib* x = mmdb_ptr -> getInfoLib(infolib_descriptor);

      if ( x == 0 ) return -1;

      info_base* base = x -> get_info_base(name);

      if ( base == 0 ) 
         return -1;
      else
         return base -> index_id();
   }

   mcatch (mmdbException &,e)
   {
      return -1;
   } end_try;
   return -1;
}

int
DtMmdbGetBookCaseByIndex(int infolib_descriptor, int index)
{
   mtry {
      info_lib* x = mmdb_ptr -> getInfoLib(infolib_descriptor);

      if ( x == 0 ) return -1;

      if ( 0 <= index && index < x -> num_of_bases() ) {

         info_base* base;

         long ind = x -> first();
         for ( int i=0; i<index; i++ )
            x -> next(ind);

         base = (*x)(ind);

         if ( base == 0 ) 
            return -1;
         else
            return base -> index_id();

      } else
        return -1;
   }
   mcatch (mmdbException &,e)
   {
      return -1;
   } end_try;
   return -1;
}

int
DtMmdbGetBookCaseByLoc(int infolib_descriptor, const char* locator)
{
   mtry {
      info_lib* x = mmdb_ptr -> getInfoLib(infolib_descriptor);

      if ( x == 0 ) return -1;

      //info_base* base = oserver_ptr -> get_infobase(locator, olias_server::LOC);
      info_base* base = x -> getInfobaseByComponent(locator, info_lib::LOC);

      if ( base == 0 ) 
            return -1;
      else
            return base -> index_id();
      
   }
   mcatch (mmdbException &,e)
   {
      return -1;
   } end_try;

   return -1;
}

int*
DtMmdbGetBookCaseByLocs(int infolib_descriptor, const char** locators, 
	int* count_ptr)
{

    mtry {
      info_lib* x = mmdb_ptr -> getInfoLib(infolib_descriptor);

      if ( x == 0 ) return 0;

      int count = 0;
      while ( locators[count] )
         count++;

      info_base** bases = 
         x -> getInfobasesByComponent((char**)locators, count, info_lib::LOC);

      count = 0;
      while ( bases[count] )
         count++;

      int* ds = (int*) malloc(sizeof(int)*(count+1));

      for ( int i=0; i<count; i++ )
         ds[i] = bases[i] -> index_id();

      delete bases;

      if (count_ptr) *count_ptr = count;

      return ds;

   }
   mcatch (mmdbException &,e)
   {
      if (count_ptr) *count_ptr = 0;
      return 0;
   } end_try;

   return 0;
}


DtMmdbBookCaseInfo* 
DtMmdbBookCaseGetInfo(int bookcase_descriptor)
{
   mtry {
        info_base* x = getBookCase(bookcase_descriptor);
  
        if ( x == 0 ) return 0;
  
        DtMmdbBookCaseInfo *y =
            (DtMmdbBookCaseInfo*)malloc(sizeof(DtMmdbBookCaseInfo));
  
        if ( y == 0 ) return 0;

        y -> name = x -> get_base_name();
        y -> num_books = x -> num_of_docs();

        return y;

   }
   mcatch (mmdbException &,e)
   {
      return 0;
   } end_try;

   return 0;
}

void DtMmdbBookCaseFreeInfo(DtMmdbBookCaseInfo* x)
{
   free((void*)x);
}

