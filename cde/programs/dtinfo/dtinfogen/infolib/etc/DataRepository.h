/* $XConsortium: DataRepository.h /main/2 1996/07/18 16:42:32 drk $ */
// DataRepository.h -- used to store all the full-text content

#ifndef DataRP_HEADER
#define DataRP_HEADER

#include <stddef.h>

/* Forward declaration  */
class FlexBuffer;
template <class T> class Stack;
template <class T> class CC_TPtrSlist;

class Rec {
friend class DataRepository;
private:
  int level;
  FlexBuffer *Buf;

public:
  Rec(int l, FlexBuffer *buf):level(l),Buf(buf){}
  Rec():level(-1),Buf(NULL){}
  Rec( const Rec &t ) { 
    if ( this != &t ) { 
      this->level = t.level;
      this->Buf = t.Buf;
    }
  }
  Rec & operator=( Rec &t ) {
    if ( this != &t ) { 
      this->level = t.level;
      this->Buf = t.Buf;
    }
    return *this;
  }

};
  
class DataRepository {

public:

typedef enum 
  { Default=0, Head, Graphic, Example, Index, Table, Total} ZoneType;

private:
  FlexBuffer *table[Total];  // zone buffer
  Stack<Rec> *zone_stack;
  FlexBuffer *current_buf;

public:
  
  DataRepository();
  ~DataRepository();

  void ActivateZone( int zone_type, int level );  // throw Exception
  void deActivateZone( int level );      // throw Exception

  void put( char );
  void write ( const char *, size_t );

  FlexBuffer **tabbuf() {  return ( table ); }
  const char *get_zone_name( int zone_type );

};

#endif  
  

  



