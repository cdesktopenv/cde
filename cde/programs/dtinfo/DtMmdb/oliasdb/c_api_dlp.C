// $XConsortium: c_api_dlp.cc /main/4 1996/06/11 17:27:07 cde-hal $

#include "oliasdb/c_api_common.h"

static dlp_smart_ptr*
getDlpUsingProid(DtMmdbInfoRequest* request)
{
   try {
      info_base* x = getBookCase(request -> bookcase_descriptor);
      if ( x == 0 ) return 0;

      oid_t * id = getPrimiaryOid(request);

      if ( id ) {
         return new dlp_smart_ptr(x, *id);
      }

      return 0;
   }

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

DtMmdbHandle*
DtMmdbDlpGetPrevSectionId(DtMmdbInfoRequest* request)
{
   try {
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

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

DtMmdbHandle*
DtMmdbDlpGetNextSectionId(DtMmdbInfoRequest* request)
{
   try {
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

   catch (mmdbException &,e)
   {
     return 0;
   } end_try;
     return 0;
}

