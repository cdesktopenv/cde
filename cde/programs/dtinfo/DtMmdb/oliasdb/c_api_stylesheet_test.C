/* $XConsortium: c_api_stylesheet_test.C /main/5 1996/08/21 15:53:25 drk $ */

#include "oliasdb/DtMmdb.h"
#include <stdio.h>

extern "C"
void showStyleSheet(DtMmdbInfoRequest* req)
{
   const char* str ;
   unsigned int l;

   if ( req == 0 ) return ;

   str = DtMmdbStylesheetGetName(req); 
   if ( str )
     fprintf(stderr, "  StylesheetInfo: name =%s\n", str);

   str = DtMmdbStylesheetGetata(req, &l);
   if ( str ) {
     fprintf(stderr, "  StylesheetInfo: data =%s\n", str);
     fprintf(stderr, "  StylesheetInfo: length =%d\n", l);
   }
}

extern "C"
int test_ss1(int argc, char** argv)
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

       req = newDtMmdbInfoRequestWithLoc(j, argv[4]);
       showStyleSheet(req);

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
int test_ss2(int argc, char** argv)
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
       showStyleSheet(req);

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



