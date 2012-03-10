/* $XConsortium: GenericId.h /main/2 1996/07/18 16:45:48 drk $ */
#ifndef __gen_Id__
#define __gen_Id__

#include "BaseDataCollect.h"

class Token;
class OL_Data;

class GenericId : public BaseData {

friend class FirstOf;
friend class OL_Data;
friend class Concat;  
  
private:
  int giname;
  int done;
  OL_Data *f_data;
  
protected:
  GenericId( const Token &t, int giname,ActionType mode);
  void markup( const Token &t );
  void data( const char *str, size_t sz );

};
  
  
#endif
