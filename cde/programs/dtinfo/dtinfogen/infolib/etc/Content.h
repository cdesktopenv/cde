/* $XConsortium: Content.h /main/2 1996/07/18 16:41:08 drk $ */
#ifndef __Cont_h__
#define __Cont_h__

#include "BaseDataCollect.h"

class Token;

class Content : public BaseData {
  
friend class FirstOf;
friend class OL_Data;
friend class Concat;  
  
protected:
  Content( const Token &t , ActionType mode);
  void markup( const Token &t );
  void data( const char *str, size_t sz );
};

inline
Content::Content( const Token &t, ActionType mode ):BaseData(t, mode) {}

#endif
