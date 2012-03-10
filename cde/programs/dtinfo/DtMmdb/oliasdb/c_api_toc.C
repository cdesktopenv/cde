// $XConsortium: c_api_toc.cc /main/4 1996/06/11 17:28:18 cde-hal $

#include "oliasdb/c_api_common.h"

static toc_smart_ptr*
getTocUsingProid(DtMmdbInfoRequest* request)
{
   try {
      info_base* x = getBookCase(request -> bookcase_descriptor);
      if ( x == 0 ) return 0;

      oid_t * id = getPrimiaryOid(request);

      if ( id == 0 ) return 0;

      return new toc_smart_ptr(x, *id);
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}


DtMmdbHandle*
DtMmdbTocGetParentId(DtMmdbInfoRequest* request)
{
   try {
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

   catch (mmdbException &,e)
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
   try {
      toc_smart_ptr* x = getTocUsingProid(request);
      if ( x == 0 ) return 0;

      oid_list_handler* y = x -> children();
      if ( y == 0 ) return 0;

      int count = (*y) -> count();
//fprintf(stderr, "count=%d\n", count);

      DtMmdbHandle** z = (DtMmdbHandle**)malloc(sizeof(DtMmdbHandle*) * (count+1));

//fprintf(stderr, "z=%x\n", (void*)z);
      if ( z == 0 ) return 0;
    
      for (int i=0; i<count; i++) { 
         z[i] = newDtMmdbHandle(oid_t(OLIAS_NODE_CODE, ((*y) -> operator()(i+1)).icode()));
      }

      z[i] = 0;

      delete y;
      delete x;

      if ( list_length ) 
         *list_length = count;

      return z;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

int DtMmdbTocGetNumOfChildren( DtMmdbInfoRequest* request)
{
   try {
      toc_smart_ptr* x = getTocUsingProid(request);
      if ( x == 0 ) return -1;
      int y = x -> subtree_size();
      delete x;
      return y; 
   }

   catch (mmdbException &,e)
   {
     return -1;
   } end_try;
     return -1;
}

