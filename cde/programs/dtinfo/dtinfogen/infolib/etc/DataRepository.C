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
/* $XConsortium: DataRepository.C /main/3 1996/08/21 15:46:21 drk $ */

#include "dti_cc/CC_Stack.h"
#include "dti_cc/CC_Slist.h"

/* Imported Interfaces */
#include "Task.h"
#include "FlexBuffer.h"

#include "DataRepository.h"


//-------------Initializer for the zone_name mapping ----------------  
static const char *zone_name[] = {
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
