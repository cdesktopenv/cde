/* $XConsortium: c_strstream.h /main/5 1996/08/21 15:55:22 drk $ */

#ifndef _strstream_h
#define _strstream_h

#include <string.h>
#include "utility/c_iostream.h"

class istrstream : public istream
{
public:
   istrstream(char* str, int size) ;
   istrstream(char* str);
   ~istrstream() ;
};

class ostrstream : public ostream
{
public:
   ostrstream(char* str, int size, int=ios::out);
   ~ostrstream() ;

   int pcount() ;

   char* str();
};


#endif
