/* $XConsortium: ContentType.h /main/3 1996/08/21 15:46:16 drk $ */
#ifndef __Content_Type_h__
#define __Content_Type_h__

#include <assert.h>

class OL_Expression;

class ContentType {

friend class OL_Data;
  
private:
  OL_Expression     *exprlist;

protected:
  void Parse( char *str );
  ContentType();
  ~ContentType();

public:
  void init ( OL_Expression *elist );  // Construct a ExprList object 

};

#endif
  
  
