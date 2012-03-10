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
// $XConsortium: strstream.C /main/5 1996/08/21 15:55:17 drk $

#include "utility/c_strstream.h"
#include "utility/c_charbuf.h"
#include <string.h>

istrstream::istrstream(char* str) : 
   //ios(new charbuf(str, strlen(str))), istream(0)
   istream(0)
{
   sbuf = new charbuf(str, strlen(str), 1);
}

istrstream::istrstream(char* str, int size ) : 
   //istream(new charbuf(str, size))
   istream(0)
{
   sbuf = new charbuf(str, size, 1);
}

istrstream::~istrstream() 
{
}

ostrstream::ostrstream(char* str, int size, int) :
   //ios(new charbuf(str, size)), ostream(0)
   ostream(0)
{
   sbuf = new charbuf(str, size);
}

ostrstream::~ostrstream() 
{
}

char* ostrstream::str() 
{
   char* x = sbuf -> get_buf();
   x[pcount()] = 0;
   return x;
}

int ostrstream::pcount() 
{
   int x = sbuf -> pcount();
   return x;
}

