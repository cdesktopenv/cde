// $XConsortium: fstream.C /main/5 1996/08/21 15:54:34 drk $

#include "utility/c_fstream.h"

istream* cin_ptr  = 0;
ostream* cout_ptr = 0;
ostream* cerr_ptr = 0;

fstream::fstream () :  iostream(new filebuf(-1))
{
}

fstream::fstream (int fd) : iostream(new filebuf(fd))
{
}

fstream::fstream (const char* name, int mode, int protect) : 
   iostream(new filebuf(name, mode, protect))
{
}

fstream::~fstream()
{
   this -> close();
}

void fstream::open(const char* name, int mode, int protect)
{
   ((filebuf*)sbuf) -> open(name, mode, protect);
}

void fstream::close()
{
   ((filebuf*)sbuf) -> close();
}

filebuf* fstream::rdbuf()
{
   return (filebuf*)sbuf;
}


