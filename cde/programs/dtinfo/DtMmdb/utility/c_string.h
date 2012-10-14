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
/* $XConsortium: c_string.h /main/5 2012/09/18 22:54:47 xxx $ */

#ifndef _string_h
#define _string_h

#include <string.h>
#include "utility/c_iostream.h"
#include "utility/c_charbuf.h"

class string
{
public:
   string(char* str);
   string(char* str, int size);
   ~string() ;

   char* c_str();
   int size();

protected:
   streambuf*	sbuf; // buffer that provides char sequence read/write
};

#endif
