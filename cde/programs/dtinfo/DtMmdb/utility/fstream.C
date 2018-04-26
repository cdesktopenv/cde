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


