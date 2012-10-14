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
/* $XConsortium: c_stringstream.h /main/5 1996/08/21 15:55:22 drk $ */

#ifndef _strstream_h
#define _strstream_h

#include <string.h>
#include "utility/c_iostream.h"
#include "utility/c_string.h"

#define BUF_INITSZ 4096

class istringstream : public istream
{
public:
   istringstream();
   istringstream(char* str);
   ~istringstream() ;
};

class ostringstream : public ostream
{
public:
   ostringstream();
   ostringstream(char* str, int=ios::out);
   ~ostringstream() ;

   string str();
};

class stringstream : public istringstream, public ostringstream
{
public:
   stringstream();
   stringstream(char* str);
   virtual ~stringstream() {};
};


#endif
