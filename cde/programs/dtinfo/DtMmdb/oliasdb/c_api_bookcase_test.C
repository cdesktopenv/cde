/* $XConsortium: c_api_bookcase_test.C /main/5 1996/08/21 15:52:55 drk $ */

#include "oliasdb/DtMmdb.h"
#include <stdio.h>


extern "C"
void showBookCaseInfo(int i)
{
   if ( i <= -1 ) {
      fprintf(stderr, "bad bookcase descriptor %d\n", i);
   } else {
      DtMmdbBookCaseInfo* info = DtMmdbBookCaseGetInfo(i);
      if ( info ) {
         if ( info -> name )
           fprintf(stderr, "    bookcaseInfo: name= %s\n", info -> name);
         fprintf(stderr, "      bookcaseInfo: num_bookcases= %d\n", info -> num_books);
         DtMmdbBookCaseFreeInfo(info);
      } else
         fprintf(stderr, "no bookcase info\n");
   }
}

