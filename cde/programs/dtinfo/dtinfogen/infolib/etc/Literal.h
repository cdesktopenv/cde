/* $XConsortium: Literal.h /main/2 1996/07/18 16:47:09 drk $ */
#ifndef __Lithdr__
#define __Lithdr__

#include "BaseDataCollect.h"
#include "FlexBuffer.h"

class Token;

class Literal : public BaseData {

friend class FirstOf;
friend class OL_Data;
friend class Concat;  
  
protected:
  Literal( const Token &t, const char *str, ActionType mode );
};

inline
Literal::Literal( const Token &t, 
		  const char *str, 
		  ActionType mode ):BaseData(t, mode)
{
  data_complete = 1;
  if ( str ) {
    ValueBuffer.writeStr( str );
  }

}
  
#endif
