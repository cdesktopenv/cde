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
    } else {
      this->level = -1;
      this->Buf = NULL;
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
  

  



