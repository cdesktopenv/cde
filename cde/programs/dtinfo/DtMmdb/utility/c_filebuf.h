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


