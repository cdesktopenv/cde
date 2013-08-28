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
/* $XConsortium: c_api_common.cc /main/5 1996/07/18 16:01:11 drk $ */

#include "dstr/void_ptr_array.h"
#include "storage/page_storage.h"
#include "api/info_lib.h"
#include <stdlib.h>
#include <limits.h>
#include "schema/store_desc.h"
#include "dynhash/data_t.h"
#include "index/index.h"
#include "object/compressed_pstring.h"
#include "object/pstring.h"

#include "oliasdb/DtMmdb.h"
#include "oliasdb/olias_funcs.h"
#include "oliasdb/c_api_common.h"
#include "oliasdb/graphic_hd.h"

#include "exception_init.h"

void_ptr_array* infolib_array = 0;
void_ptr_array* bookcase_array = 0;
OLIAS_DB *mmdb_ptr = 0;

DtMmdbHandle* DtMmdbGroundId;

void initialize_MMDB()
{
   initialize_exception();

   INIT_EXCEPTIONS();

   cin_ptr  = new fstream(0);
   cout_ptr = new fstream(1);
   cerr_ptr = new fstream(2);

   g_memory_pool_ptr = new memory_pool;

   page_storage::f_global_pcache_ptr = new page_cache_global_part;

   ostring::input_buf = new char[LBUFSIZ];

   data_t::larson_convertor_ptr = new atoi_larson;
   data_t::pearson_convertor_ptr = new atoi_pearson(SHRT_MAX, 256);

   ground_ptr = new oid_t(c_code_t(0), i_code_t(0));

   //root::io_info_buffer_ptr = new struct persis_t;

   pstring::v_io_buf_ptr = new buffer(LBUFSIZ);
   graphic_smart_ptr::local_graphic_buffer_ptr = new buffer(LBUFSIZ);

   compressed_pstring::working_buffer_ptr = new buffer(LBUFSIZ);
   compressed_pstring::v_cp_io_buf_ptr = new buffer(LBUFSIZ);

   store_desc::v_unixf_pool_ptr = 
	new lru(ACTIVE_UNIXF_SZ, INACTIVE_UNIXF_SZ, false);

   c_index::v_static_key_ptr = new key_type(LBUFSIZ);

   infolib_array = new void_ptr_array(10);
   bookcase_array = new void_ptr_array(10);
}

void quit_MMDB()
{
   delete c_index::v_static_key_ptr;

   delete store_desc::v_unixf_pool_ptr;

   delete compressed_pstring::working_buffer_ptr;
   delete compressed_pstring::v_cp_io_buf_ptr;

   delete pstring::v_io_buf_ptr;
   delete graphic_smart_ptr::local_graphic_buffer_ptr;

   //delete root::io_info_buffer_ptr;

   delete ground_ptr;

   delete data_t::larson_convertor_ptr;
   delete data_t::pearson_convertor_ptr;

   delete ostring::input_buf;

   delete page_storage::f_global_pcache_ptr;

   delete g_memory_pool_ptr;

   delete cin_ptr  ;
   delete cout_ptr ;
   delete cerr_ptr ;

   delete infolib_array;
   delete bookcase_array;

   quit_exception();

}

///////////////////////////////////////////////////////////////


extern "C" {
void DtMmdbInit()
{
   initialize_MMDB();
}

void DtMmdbQuit()
{
   quit_MMDB();
}

}

void* operator new( size_t sz )
{
   void* p = (void*)malloc(sz);
   //printf("a::operator new called(). sz= %d, allo=%x\n", sz, p);
   return p;
}

void operator delete( void* p )
{
   //printf("a::operator free called(). alloc= %x\n", p);
   if ( p )
      free((char*)p);
}

#ifdef DEBUG
extern "C" {
void* centerline_new(int n, char*, int)
{
    return :: operator new(n);
}
}
#endif

/*
info_lib* getInfoLib(int infolib_descriptor)
{
   if ( 0 <= infolib_descriptor && infolib_array &&
        infolib_descriptor < infolib_array -> no_elmts()
         )
   {
      return (info_lib*)(*infolib_array)[infolib_descriptor];
   } else
      return 0;
}
*/

info_base* getBookCase(int bookcase_descriptor)
{
   if ( 0 <= bookcase_descriptor && bookcase_array &&
        bookcase_descriptor < bookcase_array -> no_elmts()
      ) {
      return (info_base*)(*bookcase_array)[bookcase_descriptor];
   } else
      return 0;
}

oid_t* getPrimiaryOid(DtMmdbInfoRequest* x)
{
   if ( x == 0 ) return 0;

   if ( x -> primary_oid == 0 ) return 0;

   return (oid_t*)(x -> primary_oid -> oid_ptr);
}

oid_t* getSecondaryOid(DtMmdbInfoRequest* x)
{
   if ( x == 0 ) return 0;

   if ( x -> secondary_oid == 0 ) return 0;

   return (oid_t*)(x -> secondary_oid -> oid_ptr);
}

int getSeqNum(DtMmdbInfoRequest* x)
{
   if ( x == 0 ) return -1;
   return x -> sequence_num;
}

const char* getLocator(DtMmdbInfoRequest* x)
{
   if ( x == 0 ) return 0;

   return x -> locator_ptr;
}

DtMmdbHandle* newDtMmdbHandle(const oid_t& x)
{
   oid_t* y = new oid_t(x);
   DtMmdbHandle *z = (DtMmdbHandle*)malloc(sizeof(DtMmdbHandle));
   z -> oid_ptr = (void*)y;
   return z;
}

DtMmdbGraphicInfo* newDtMmdbGraphicInfo()
{
   DtMmdbGraphicInfo *z = 
      (DtMmdbGraphicInfo*)malloc(sizeof(DtMmdbGraphicInfo));
   return z;
}

void DtMmdbFreeHandle(DtMmdbHandle* x)
{
   if ( x ) {
//    delete x -> oid_ptr;
      free((void*)x);
   }
}

void DtMmdbFreeHandleList(DtMmdbHandle** list)
{
   if ( list ) {
      int i = 0;
      while ( list[i] ) {
        DtMmdbFreeHandle(list[i]);
        i++;
      }
      free((void*)list);
   }
}

#ifdef REGRESSION_TEST
extern int auto_test(int argc, char** argv, OLIAS_DB& db);
#endif

extern "C" {
int auto_test_c_api(int argc, char** argv)
{
   mtry
   {
#ifdef REGRESSION_TEST
      OLIAS_DB db;
      return auto_test(argc, argv, db);
#else
      return -1;
#endif
   }

   mcatch (mmdbException &,e)
   {
      cerr << "Exception msg: " << e << "\n";
      return -1;
   }
   end_try;

   return -1;
}


DtMmdbInfoRequest* newDtMmdbInfoRequestWithLoc(int bc_id, char* loc)
{
   DtMmdbInfoRequest* x =
        (DtMmdbInfoRequest*) malloc(sizeof(DtMmdbInfoRequest));

  
   x -> bookcase_descriptor = bc_id ;
   x -> primary_oid = 0;
   x -> secondary_oid = 0;
   x -> locator_ptr = loc;
   return x;
}

DtMmdbInfoRequest* newDtMmdbInfoRequestWithPrimaryOid(int bc_id, char* oid_str)
{
   DtMmdbInfoRequest* x =
        (DtMmdbInfoRequest*) malloc(sizeof(DtMmdbInfoRequest));

  
   x -> bookcase_descriptor = bc_id ;
   x -> locator_ptr = 0;
   x -> primary_oid = newDtMmdbHandle(oid_t(oid_str, true, false));
   x -> secondary_oid = 0;
   return x;
}

DtMmdbInfoRequest* newDtMmdbInfoRequestWithSecondaryOid(int bc_id, char* oid_str)
{
   DtMmdbInfoRequest* x =
        (DtMmdbInfoRequest*) malloc(sizeof(DtMmdbInfoRequest));

  
   x -> bookcase_descriptor = bc_id ;
   x -> locator_ptr = 0;
   x -> primary_oid = 0;
   x -> secondary_oid = newDtMmdbHandle(oid_t(oid_str, true, false));
   return x;
}

DtMmdbInfoRequest* newDtMmdbInfoRequestWithSeqnum(int bc_id, char* str)
{
   DtMmdbInfoRequest* x =
        (DtMmdbInfoRequest*) malloc(sizeof(DtMmdbInfoRequest));

  
   x -> bookcase_descriptor = bc_id ;
   x -> locator_ptr = 0;
   x -> primary_oid = 0;
   x -> secondary_oid = 0;
   x -> sequence_num = atoi(str);
   return x;
}

char* DtMmdbHandleToString(DtMmdbHandle* x)
{
   static char buf[100];
   if ( x -> oid_ptr) {
      oid_t *z = (oid_t*)(x -> oid_ptr);
      snprintf(buf, sizeof(buf), "%d.%d", z -> ccode(), (int)z -> icode());
   } else
     buf[0] = 0;

   return buf;
}

void DtMmdbFreeInfoRequest(DtMmdbInfoRequest* x)
{
   if ( x ) {
     DtMmdbFreeHandle(x-> primary_oid);
     DtMmdbFreeHandle(x-> secondary_oid);
     free(x);
   }
}
}

