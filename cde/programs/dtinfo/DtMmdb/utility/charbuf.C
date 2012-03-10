// $XConsortium: charbuf.C /main/4 1996/08/21 15:54:12 drk $


#include "utility/c_charbuf.h"

charbuf::charbuf(char* p, int l, int bufferFull) : streambuf(p, l, bufferFull)
{
}

charbuf::~charbuf()
{
}


