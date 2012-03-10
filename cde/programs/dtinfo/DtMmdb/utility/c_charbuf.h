/* $XConsortium: c_charbuf.h /main/4 1996/08/21 15:54:16 drk $ */


#ifndef _charbuf_h
#define _charbuf_h

#include "utility/c_streambuf.h"

class charbuf : public streambuf
{
protected:

protected:

public:
   charbuf(char* p, int l, int pHasContent = 0);
   ~charbuf();
};

#endif
