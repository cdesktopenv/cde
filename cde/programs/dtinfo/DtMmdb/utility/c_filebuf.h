/* $XConsortium: c_filebuf.h /main/4 1996/08/21 15:54:26 drk $ */

#ifndef _x_filebuf_h
#define _x_filebuf_h

#include "utility/c_streambuf.h"
#include "utility/c_ios.h"

class  filebuf : public streambuf
{
protected:
   int _fd;
   int _mode;
   int _prev_action;
   char* _name;

   streampos current_pos;
   streampos new_pos;
   streampos default_new_pos;

protected:
   void notify(int action_t); 
   void _notify(int action_t);

   int overflow();
   int underflow() ;

   int _seek();
   int _seek(streampos pos, int whence);
   int _write(char* ptr, int size);

public:
   filebuf(int fd);
   filebuf(const char* name, int mode, int protect = 0644);
   ~filebuf();

   int fd() { return _fd; };
   int is_open();

   int open(const char* name, int mode, int protect = 0644);
   int close();

   int flush() ;
   int seekg(streampos delta) ;
};

#endif


