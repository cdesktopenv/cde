/* $XConsortium: DataRepository.C /main/3 1996/08/21 15:46:21 drk $ */

#include "dti_cc/CC_Stack.h"
#include "dti_cc/CC_Slist.h"

/* Imported Interfaces */
#include "Task.h"
#include "FlexBuffer.h"

#include "DataRepository.h"


//-------------Initializer for the zone_name mapping ----------------  
static char *zone_name[] = {
  "D",
  "H",
  "G",
  "E",
  "I",
  "T"
};


//------------------------------------------------------------------
DataRepository::DataRepository()
{
  for ( int i = 0; i < Total; i++ ) {
    table[i] = NULL;
  }
  zone_stack = new Stack<Rec>;
  current_buf = NULL;

}

//------------------------------------------------------------------
DataRepository::~DataRepository()
{
  delete zone_stack;

  // destroy the table
  for ( int i = 0; i < Total; i++ ) {
    delete table[i];
  }
}

//------------------------------------------------------------------
void
DataRepository::ActivateZone( int zt, int level )
{
  if ( !table[zt] ) {
    table[zt] = new FlexBuffer;
  }
  
  // prepare a buffer to be written 
  Rec rec( level, table[zt] );
  zone_stack->push ( rec );
  current_buf = table[zt];
}

//------------------------------------------------------------------
void
DataRepository::deActivateZone( int level )
{
  if ( zone_stack->empty() ) {
    return;
  }

  if ( zone_stack->top().level != level ) {
    return;
  }
  
  Rec ZoneRec = zone_stack->pop();

  // update the current_buf 
  if ( !zone_stack->empty() ) {
    current_buf = zone_stack->top().Buf;
  }

}

//------------------------------------------------------------------
void 
DataRepository::put(char ch)
{
  if ( current_buf ) {
    current_buf->put( ch );
  }
}

//------------------------------------------------------------------
void
DataRepository::write( const char *str, size_t n )
{
  current_buf->write( str, n );
}

//-------------------------------------------------------------------
const char *
DataRepository::get_zone_name( int zt ) 
{
  if ( zt >= Total ) {
    return NULL;
  }
  return ( zone_name[zt] );
}
