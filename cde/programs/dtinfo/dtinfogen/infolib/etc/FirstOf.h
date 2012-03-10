/* $XConsortium: FirstOf.h /main/3 1996/08/21 15:46:51 drk $ */
#ifndef __FirstOfHdr__
#define __FirstOfHdr__

#include "Task.h"
#include "BaseDataCollect.h"

class Token;
class OL_Expression;
class ExprList;

class FirstOf : public BaseData {

friend class OL_Data;
friend class Concat;  
  
private:
  OL_Expression *elist;

protected:
  FirstOf( const Token &t, ExprList *el,ActionType mode );
  void markup( const Token &t );
  void data( const char *str, size_t sz ) { ComplexTask::data( str, sz ); }
};


#endif
