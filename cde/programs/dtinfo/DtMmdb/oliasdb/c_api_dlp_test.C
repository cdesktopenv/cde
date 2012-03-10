/* $XConsortium: c_api_dlp_test.C /main/5 1996/08/21 15:53:00 drk $ */

#include "oliasdb/DtMmdb.h"
#include <stdio.h>


extern "C"
void showDLPCell(DtMmdbInfoRequest* req)
{
   DtMmdbHandle* id ;

   if ( req == 0 ) return ;

   id = DtMmdbDlpGetPrevSectionId(req);
   if ( id )
      fprintf(stderr, "        DLP: prevSectionId=%s\n", DtMmdbHandleToString(id));
   DtMmdbFreeHandle(id);

   id = DtMmdbDlpGetNextSectionId(req);
   if ( id )
      fprintf(stderr, "        DLP: nextSectionId=%s\n", DtMmdbHandleToString(id));
   DtMmdbFreeHandle(id);
}

extern "C"
int test_dlp(int argc, char** argv)
{
   int j = 0;
   int i = 0;
   const char* str;
   DtMmdbInfoRequest* req ;

   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       req = newDtMmdbInfoRequestWithPrimaryOid(j, argv[4]);
       showDLPCell(req);

       DtMmdbFreeInfoRequest(req);
       DtMmdbCloseInfoLib(i);

       break;
      }
      default:
       fprintf(stderr, "invalid arguments\n");
   }

   fprintf(stderr, "infolib desc=%d\n", i);
   return 0;
}



