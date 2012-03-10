// $XConsortium: tst_filebuf.C /main/4 1996/08/21 15:55:25 drk $

#include <iostream.h>

#include "utility/debug.h"
#include "utility/c_fstream.h"


fstream_test1(char* nm)
{
   fstream f(nm, ios::in|ios::out)
}

main()
{
   streambuf_test1();
   streambuf_test2();
   streambuf_test3();
   streambuf_test4();
}
