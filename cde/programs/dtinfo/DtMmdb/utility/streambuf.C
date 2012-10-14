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
/* $XConsortium: streambuf.C /main/8 1996/08/21 15:55:14 drk $ */
#include "utility/c_streambuf.h"

#if defined(USL) || defined(__osf__) || defined(linux) || defined(CSRG_BASED)
#include <stdlib.h>
#else
#include <libc.h>
#endif

#define DEF_BUF_SIZ 4096

streambuf::streambuf() : _size(0), _capacity(DEF_BUF_SIZ), _alloc(1),
   _pcount(0), _gcount(0)
{
   base = (char*)malloc(DEF_BUF_SIZ);
   end = base + DEF_BUF_SIZ;
   get_ptr = put_ptr = base;
}

streambuf::streambuf(char* p, int l, int bufferFull) :
    base(p), end(p+l), put_ptr(p), get_ptr(p), _size(0), _capacity(l), 
    _alloc(0), _pcount(0), _gcount(0)
{
   if (bufferFull) {
     _size = l;
   }
}

streambuf::~streambuf()
{
   if ( _alloc )
      free(base);
}

int streambuf::examine()
{
   notify(GET);

   if ( empty() && underflow() == EOF )
      return EOF;

   return (unsigned char)(*get_ptr);
}

int streambuf::get()
{
   notify(GET);

   if ( empty() && underflow() == EOF )
      return EOF;

   int x = (unsigned char)(*get_ptr);

   move_get_ptr(+1);

   _size--;

   _gcount++;

   return x;
}

int streambuf::putback(char c)
{
   if ( full() )
      return EOF;

   move_get_ptr(-1);

   _size++;

   *get_ptr = c;
   return 0;
}

int streambuf::put(char c)
{
   notify(PUT);

   if ( full() && overflow() == EOF )
      return EOF;

   *put_ptr = c;

   move_put_ptr(1);

   _size++;
   _pcount++;

   return 0;
}

int streambuf::move_get_ptr(int one)
{
   switch (one) {
     case 1:

       get_ptr++;  
       if ( get_ptr == end )
         get_ptr = base;

       return 0;

     case -1:

       get_ptr--;  
       if ( get_ptr == base-1 )
         get_ptr = end-1;

       return 0;

     default:
       return EOF;
   }

}

int streambuf::move_put_ptr(int one)
{
   switch (one) {
     case 1:

       if ( get_ptr == 0 )
          get_ptr = put_ptr;

       put_ptr++; 

       if ( put_ptr == end )
         put_ptr = base;

       return 0;

      default:
          return EOF;
   }
}

