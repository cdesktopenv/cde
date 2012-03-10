/* $XConsortium: OL-Data.h /main/2 1996/07/18 16:47:56 drk $ */
#ifndef OL_DATA_H
#define OL_DATA_H

#include <string.h>

#include "ContentType.h"
#include "Token.h"
#include "BaseDataCollect.h"

class OL_Data : public BaseData {

friend class FirstOf;

private:
  ContentType tokContent;
  char *f_name;
  int   line_num;
  int   istat;      /* These are status info */
  int   removeSpaces;
  void reset();

protected:
  /*
   * This constructor is used by the derived class of OL_Data only
   */
  OL_Data( const Token &t, ActionType ignore=DEFAULT_ACTION );

public:

  /* 
   * istat is controlled by the invoker to determine if OL_Data should really
   * ignore the OLIAS IGNORE attribute. The reason that such action is 
   * controlled by the invoker of the class is that the corresponding
   * behavior of IGNORE is controlled by the invoker, not by the OL_Data class
   */
  
  OL_Data( const Token &t, 
	   int aType, 
	   ActionType mode=DEFAULT_ACTION);
  /* no action takes place by default */


  ~OL_Data() { delete f_name; }
  
public:
  void markup( const Token & );
  void data( const char *, size_t );
  char *filename() { return ( f_name ); }
  int   line_no()  { return ( line_num ); }
  
};

inline
OL_Data::OL_Data(const Token &t, ActionType mode):BaseData(t,mode)
{
  istat = mode & IGNORE_ON;
  removeSpaces = mode & REMOVE_SPACES;
  
  f_name = strdup( t.file() );
  line_num = t.line();
}

#endif
