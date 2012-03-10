/* $XConsortium: c_api_pure_virtual.cc /main/5 1996/07/18 16:01:43 drk $ */

#include <stdio.h>
#include <stdlib.h>


extern "C" {

#if defined(hpux)
char __pure_virtual_called()
{
   fprintf(stderr, "DtMmdb: pure virtual called\n");
   exit (-1);
   return 0;
}
#else
#if defined(SVR4) && defined(SC3)
void _pure_error_()
{
   fprintf(stderr, "DtMmdb: pure virtual called\n");
   exit (-1);
}
#else
#ifdef _AIX
void __PureVirtualCalled()
{
   fprintf(stderr, "DtMmdb: pure virtual called\n");
   exit (-1);
}
#else

void __pure_virtual_called()
{
   fprintf(stderr, "DtMmdb: pure virtual called\n");
   exit (-1);
}

#endif
#endif
#endif

}
