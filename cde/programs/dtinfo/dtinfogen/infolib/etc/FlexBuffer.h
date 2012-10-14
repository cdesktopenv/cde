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
/* $XConsortium: FlexBuffer.h /main/2 1996/07/18 16:45:28 drk $ */
#ifndef FLEX_BFR_HDR
#define FLEX_BFR_HDR

#include <stddef.h>
#include <iostream>
using namespace std;

class FlexBuffer {
  
friend ostream &operator<< ( ostream &s, FlexBuffer &);

private:
  int pos;
  int maxSize;
  char *HeadPtr;

  void grow(size_t);
  
public:
  void write( const char *ch, size_t n );
  void writeStr ( const char *ch );
  void put( char );

  void reset() { pos = 0; };
  
  int GetSize()          { return( pos ); }
  const char *GetBuffer()  { grow(pos); HeadPtr[pos] = 0; return(HeadPtr); }

  FlexBuffer();
  ~FlexBuffer() { delete [] HeadPtr; }
 
  FlexBuffer &operator+ ( FlexBuffer & );

};  
  
inline  void FlexBuffer::put( char c )
{
  grow( pos + 1 );
  *(HeadPtr + pos) = c;
  pos += 1;
} 

#endif  
