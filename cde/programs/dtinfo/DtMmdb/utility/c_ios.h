/* $XConsortium: c_ios.h /main/4 1996/08/21 15:54:50 drk $ */

#ifndef _ios_h
#define _ios_h

#include "utility/macro.h"
#include "utility/c_streambuf.h"

class ios
{
protected:
  int f_state;
  streambuf*      sbuf; // buffer that provides char sequence read/write

public:

  enum open_mode { in=1, out=2, app=4, trunc=8 };
  enum seek_dir   { beg=0, cur=1, end=2 } ;
  enum states { OK=0, BAD=1, FAIL=2 } ;

  ios(streambuf* sb = 0);
  virtual ~ios();

  int rdstate() { return f_state; };

  int fail() ;
  int bad() ;

  int good() { 
    return !(fail() || bad()) ;
  };

  void set_bad() { f_state |= BAD;};
  void set_fail() { f_state |= FAIL;};
  void clear() { f_state = OK; };

  int operator!() { return fail(); };
  operator void*() { return (void*)good(); };

};

#endif
