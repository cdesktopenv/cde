// $XConsortium: XpmLib.hh /main/3 1996/06/11 16:30:35 cde-hal $
/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/


#ifndef XPMLIB_HH
#define XPMLIB_HH

#include <dlfcn.h>
#include <X11/xpm.h>

class XpmLib
{
public:

  XpmLib(const char* = 0, int = RTLD_LAZY);
  ~XpmLib();

  bool is_open() { return f_handle ? TRUE : FALSE; } // if library opened properly

  int CreatePixmapFromData(Display*, Drawable, char**, Pixmap*, Pixmap*, XpmAttributes*);

  int CreatePixmapFromBuffer(Display*, Drawable, char*, Pixmap*, Pixmap*, XpmAttributes*);

  int ReadFileToPixmap(Display*, Drawable, char*, Pixmap*, Pixmap*, XpmAttributes*);

  static XpmLib& xpm_lib() { return *f_xpm_lib; }

private:

  void* f_handle;

  static XpmLib* f_xpm_lib;

  void* symbol(const char*);
};

inline XpmLib& xpm_lib()
{
  return XpmLib::xpm_lib();
}

#endif
