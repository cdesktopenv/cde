/* $XConsortium: BaseDataCollect.h /main/2 1996/07/18 16:40:12 drk $ */
#ifndef __BaseDataCollect_h
#define __BaseDataCollect_h

#include "Task.h"
#include "Token.h"
#include "FlexBuffer.h"

enum ActionType {
  DEFAULT_ACTION=0,
  IGNORE_ON=1,
  REMOVE_SPACES=2,
  GENERATE_ID=4
};

class BaseData : public ComplexTask {

protected:
  int f_base;
  FlexBuffer ValueBuffer;
  int data_avail;
  int data_complete;
  int ignore_status;

protected:
  BaseData( const Token &t, ActionType istat) { 
    f_base = t.level();
    data_avail = 1;
    data_complete = 0;
    ignore_status = istat & IGNORE_ON;
  }

public:  
  int   ContentIsEmpty()    { return ( ValueBuffer.GetSize() == 0 ); }
  const char  *content()    { return( ValueBuffer.GetBuffer() ); }
  int   content_size()      { return( ValueBuffer.GetSize() );   }
  int   DataWillBeAvailable()   { return( data_avail ); }
  int   DataIsComplete()        { return( data_complete ); }

};

#endif
