/* $XConsortium: FlexBuffer.h /main/2 1996/07/18 16:45:28 drk $ */
#ifndef FLEX_BFR_HDR
#define FLEX_BFR_HDR

#include <stddef.h>
#include <iostream.h>

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
