// $XConsortium: c_api_locator.cc /main/3 1996/06/11 17:27:38 cde-hal $

#include "oliasdb/c_api_common.h"

static locator_smart_ptr*
getLocatorUsingLocAndProid(DtMmdbInfoRequest* request, Boolean checkLocOnly = false)
{
   try {
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

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char*
DtMmdbLocatorGetSectionLoc(DtMmdbInfoRequest* request)
{
   try {
      locator_smart_ptr* x = getLocatorUsingLocAndProid(request, false);

      if ( x == 0 ) return 0;
      const char* y = x -> inside_node_locator_str();
      delete x;
      return y;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

DtMmdbHandle*
DtMmdbLocatorGetSectionObjectId(DtMmdbInfoRequest* request)
{
   try {
      locator_smart_ptr* x = getLocatorUsingLocAndProid(request, true);
      if ( x == 0 ) return 0;
      DtMmdbHandle *z = newDtMmdbHandle(x -> node_id());
      delete x;
      return z;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

