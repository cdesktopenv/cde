/* $XConsortium: AttributeData.h /main/2 1996/07/18 16:36:49 drk $ */
#ifndef __attr_Data__
#define __attr_Data__

#include "BaseDataCollect.h"

class Token;

class AttributeData : public BaseData {

friend class FirstOf;
friend class OL_Data;
friend class Concat;  
  
private:
  const char *attribute_value( const Token &t, int attr_name );
  
protected:
  AttributeData( const Token &t, int attr_name, ActionType mode );

};

#endif
