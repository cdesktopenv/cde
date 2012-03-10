/* $XConsortium: c_api_locator_test.C /main/5 1996/08/21 15:53:11 drk $ */

#include "oliasdb/DtMmdb.h"
#include <stdio.h>


extern "C"
void showLocator(DtMmdbInfoRequest* req)
{
   DtMmdbHandle* id = 0;
   const char* str = 0;

   if ( req == 0 ) return ;

   str = DtMmdbLocatorGetSectionLoc(req);
   if ( str )
      fprintf(stderr, "        LOC: sectionLoc=%s\n", str);

   id = DtMmdbLocatorGetSectionObjectId(req);
   if ( id )
      fprintf(stderr, "        LOC: sectionId=%s\n", DtMmdbHandleToString(id));
   DtMmdbFreeHandle(id);
}

extern "C"
int test_loc1(int argc, char** argv)
{
   int j = 0;
   int i = 0;
   const char* str;
   DtMmdbInfoRequest* req;

   switch ( argc ) {
     case 5:
      {
       i = DtMmdbOpenInfoLib(argv[2], argv[3], DtMmdbFalse);
       j = DtMmdbGetBookCaseByName(i, argv[3]);

       req = newDtMmdbInfoRequestWithLoc(j, argv[4]);
       showLocator(req);

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


extern "C"
int test_loc2(int argc, char** argv)
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
       showLocator(req);

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

