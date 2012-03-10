/* $XConsortium: Handler.cc /main/2 1996/07/18 16:15:12 drk $ */
#include <stdio.h>
#include <stdlib.h>

//---------------------------------------------------------------------
void FreeStoreException()
{
  fprintf(stderr,"(ERROR) Memory exhausted\n");
  exit(1);
}
