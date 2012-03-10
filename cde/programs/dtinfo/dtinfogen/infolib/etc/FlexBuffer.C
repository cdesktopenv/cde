/* $XConsortium: FlexBuffer.cc /main/2 1996/07/18 16:13:49 drk $ */

#include <iostream.h>
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
  if(needed + 1 > maxSize){
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
