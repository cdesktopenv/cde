/* $XConsortium: ConcatTask.h /main/2 1996/07/18 16:40:43 drk $ */
#ifndef __Concat_h__
#define __Concat_h__

#include "Task.h"
#include "OL-Data.h"

class ExprList;
class Token;

class Concat : public OL_Data {
  
friend class FirstOf;  
friend class OL_Data;  

protected:
  Concat( const Token &t, ExprList *elist, ActionType mode );
  void markup( const Token &t ) { OL_Data::markup(t); }
  void data( const char *str, size_t sz ) { OL_Data::data(str, sz); }

};

#endif
