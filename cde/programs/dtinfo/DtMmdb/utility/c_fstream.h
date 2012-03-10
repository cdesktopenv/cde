/* $XConsortium: c_fstream.h /main/5 1996/08/21 15:54:38 drk $ */

#ifndef _x_fstream_h
#define _x_fstream_h

#include "utility/c_filebuf.h"
#include "utility/c_iostream.h"

class fstream : public iostream
{
public:
   fstream ();
   fstream (int fd);
   fstream (const char* name, int mode, int protect = 0644);

   ~fstream();

   void open(const char* name, int mode, int protect = 0644);

   void close();

   filebuf* rdbuf();

};

extern fstream xcout;
extern fstream xcerr;
extern fstream xcin;


#endif
