/* $XConsortium: Content.cc /main/2 1996/07/18 16:10:34 drk $ */

/* exported interfaces */
#include "Content.h"

/* imported interfaces */
#include "Dispatch.h"
#include "OL-Data.h"
#include "Token.h"
#include "SGMLName.h"

void
Content::markup( const Token &t )
{

  if ( ignore_status && !Dispatch::OutsideIgnoreScope() ) {
    return;
  }

  if ( f_base > 0 ) {

    if ( t.type() == END ) {
      if ( f_base == t.level() ) {
	f_base = -1;
	data_complete=1;
      }
    }
    else if ( t.type() == EXTERNAL_ENTITY ) {
      data_avail = 1;
      ValueBuffer.writeStr( t.getEntityFileName() );
    }
    else if ( t.type() == INTERNAL_ENTITY ) {
      data_avail = 1;
      ValueBuffer.put( '&' );
      ValueBuffer.writeStr( SGMLName::lookup( t.getEntityName() ) );
      ValueBuffer.put( ';' );
    }

  }
}

void
Content::data( const char *str, size_t sz )
{

  if ( ignore_status && !Dispatch::OutsideIgnoreScope() ) {
    return;
  }

  if ( f_base > 0 ) {
    ValueBuffer.write ( str, sz );
  }
}

	
