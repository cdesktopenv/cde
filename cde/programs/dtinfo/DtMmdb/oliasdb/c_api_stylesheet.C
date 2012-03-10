// $XConsortium: c_api_stylesheet.cc /main/3 1996/06/11 17:28:03 cde-hal $

#include "oliasdb/c_api_common.h"

static stylesheet_smart_ptr*
getStylesheetUsingLocAndProid(DtMmdbInfoRequest* request, Boolean checkPIDOnly = false)
{
   try {
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

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char*
DtMmdbStylesheetGetName(DtMmdbInfoRequest* request)
{
   try {
      stylesheet_smart_ptr* x = getStylesheetUsingLocAndProid(request, true);

      if ( x == 0 ) return 0;
      const char* y = x -> name();
      delete x;
      return y;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

const char*
DtMmdbStylesheetGetata(DtMmdbInfoRequest* request, unsigned int* data_length)
{
   try {
      stylesheet_smart_ptr* x = getStylesheetUsingLocAndProid(request, false);

      if ( x == 0 ) return 0;

#ifdef DEBUG
      fprintf(stderr, " stylesheetInfo: mmdb_oid = %d.%d\n", (*x).its_oid().ccode()
, (*x).its_oid().icode());
#endif

      const char* y = x -> online_data();

      if ( data_length ) *data_length = strlen(y);

      delete x;
      return y;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

