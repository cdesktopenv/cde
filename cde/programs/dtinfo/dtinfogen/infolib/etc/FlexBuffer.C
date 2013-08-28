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
/* $XConsortium: FlexBuffer.cc /main/2 1996/07/18 16:13:49 drk $ */

#include <iostream>
using namespace std;
#include <memory.h>
#include <string.h>
#include "FlexBuffer.h"

//---------------------------------------------------------
FlexBuffer::FlexBuffer()
{
  HeadPtr = 0;
  maxSize = pos = 0;
}

//---------------------------------------------------------
//FlexBuffer::~FlexBuffer()
//{
//  delete HeadPtr;
//}

//---------------------------------------------------------
void
FlexBuffer::grow(size_t needed)
{
  if(needed + 1 > (size_t) maxSize){
    char *born = new char[maxSize = needed * 3 / 2 + 10];
    
    if(pos){
      memcpy(born, HeadPtr, pos);
      delete [] HeadPtr;
    }

    HeadPtr = born;
  }
}

//---------------------------------------------------------
void
FlexBuffer::write(const char *d, size_t n)
{
  grow(pos + n);
  memcpy(HeadPtr + pos, d, n);
  pos += n;
}

//---------------------------------------------------------
void
FlexBuffer::writeStr(const char *str )
{
  int n = strlen(str);
  grow( pos + n );
  memcpy ( HeadPtr + pos, str, n );
  pos += n;
}

//---------------------------------------------------------
FlexBuffer &
FlexBuffer::operator+( FlexBuffer & fb )
{

  grow( pos + fb.GetSize() );
  memcpy(HeadPtr + pos , fb.HeadPtr, fb.GetSize() );
  pos += fb.GetSize();
  return ( *this );
}

//---------------------------------------------------------
ostream &
operator<< ( ostream &os, FlexBuffer &fb )
{
  os << fb.GetBuffer();
  return os;
}
