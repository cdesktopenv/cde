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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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
