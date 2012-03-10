/* $XConsortium: GenericId.cc /main/5 1996/07/18 16:14:18 drk $ */

/* exported interfaces */
#include "GenericId.h"

/* imported interfaces */
#include "Dispatch.h"
#include "SGMLName.h"
#include "OLAF.h"
#include "OL-Data.h"
#include "Token.h"

#ifdef FISH_DEBUG
#include "dbug.h"
#endif

//-------------------------------------------------------------------------
GenericId::GenericId( const Token &t, 
		      int name, 
		      ActionType mode ):BaseData(t,mode)
{
#ifdef FISH_DEBUG
  DBUG_PRINT("GenericId", (" name = %s", SGMLName::lookup(name)) );
#endif
  giname = name;
  f_data = NULL;
  done   = 0;
  f_base = -1;
}
		      
//-------------------------------------------------------------------------
void
GenericId::markup( const Token &t )
{
#ifdef FISH_DEBUG
  DBUG_PRINT( "GenericId", (" token t = %s",SGMLName::lookup(t.Gi()) ) );
#endif
  
  if ( ignore_status && !Dispatch::OutsideIgnoreScope() ) {
    return;
  }

  if ( f_data ) {
    f_data->markup( t );
  }
    
  if ( t.type() == START ) {

    /* first time we see the GI */
    if ( t.Gi() == giname && f_base < 0 && !done ) { 

      /* fork off the OL_Data class for this GI */
      f_data = new OL_Data ( t, OLAF::OL_data, (ActionType)ignore_status );
      assert ( f_data != NULL );

      f_base = t.level();
	
    }
  }
  else if ( t.type() == END ) {

    if ( f_base == t.level() && !done ) {

      if ( data_complete = f_data->DataIsComplete() ) {
	ValueBuffer.writeStr( f_data->content() );
      }

      delete f_data; f_data = NULL;

      done = 1;
	
    }
  }
    
}


//-------------------------------------------------------------------------
void
GenericId::data( const char *str, size_t sz )
{
  if ( ignore_status && !Dispatch::OutsideIgnoreScope() ) {
    return;
  }

  if ( f_base > 0 ) {
    if ( f_data ) {
      f_data->data( str, sz );
    }
  }
}
  



