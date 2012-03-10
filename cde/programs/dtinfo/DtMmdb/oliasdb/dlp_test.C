/* $XConsortium: dlp_test.cc /main/4 1996/07/18 16:02:00 drk $ */

#ifdef REGRESSION_TEST

#include "oliasdb/dlp_test.h"

int dlp_test( info_lib* infolib_ptr, const char* base_name, const char* oid_str )
{
   oid_t id((char*)oid_str, true, false);

   dlp_smart_ptr* y = new dlp_smart_ptr( infolib_ptr, base_name, id );

   dlp_smart_ptr* z;
   cerr << form("run backward from %s:\n", oid_str);

   do {
      y -> node_id().asciiOut(cerr); cerr <<  "\n";
      z = y->prev();
      delete y;
      y = z;
   } while ( y );


   cerr << form("run forward from %s:\n", oid_str);

   y = new dlp_smart_ptr( infolib_ptr, base_name, id );
  
   do {
      y -> node_id().asciiOut(cerr); cerr <<  "\n";
      z = y->prev();
      delete y;
      y = z;
   } while ( z );


   return 0;
}
#endif
