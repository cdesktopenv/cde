// $XConsortium: strstream.C /main/5 1996/08/21 15:55:17 drk $

#include "utility/c_strstream.h"
#include "utility/c_charbuf.h"
#include <string.h>

istrstream::istrstream(char* str) : 
   //ios(new charbuf(str, strlen(str))), istream(0)
   istream(0)
{
   sbuf = new charbuf(str, strlen(str), 1);
}

istrstream::istrstream(char* str, int size ) : 
   //istream(new charbuf(str, size))
   istream(0)
{
   sbuf = new charbuf(str, size, 1);
}

istrstream::~istrstream() 
{
}

ostrstream::ostrstream(char* str, int size, int) :
   //ios(new charbuf(str, size)), ostream(0)
   ostream(0)
{
   sbuf = new charbuf(str, size);
}

ostrstream::~ostrstream() 
{
}

char* ostrstream::str() 
{
   char* x = sbuf -> get_buf();
   x[pcount()] = 0;
   return x;
}

int ostrstream::pcount() 
{
   int x = sbuf -> pcount();
   return x;
}

