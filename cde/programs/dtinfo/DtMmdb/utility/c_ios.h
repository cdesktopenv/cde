/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: c_ios.h /main/4 1996/08/21 15:54:50 drk $ */

#ifndef _ios_h
#define _ios_h

#include <sys/types.h>
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
  operator void*() { return (void*)(size_t)good(); };

};

#endif
