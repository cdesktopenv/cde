/* $XConsortium: ExprList.h /main/3 1996/08/21 15:46:37 drk $ */
#ifndef __ExprList__
#define __ExprList__

class OL_Expression;

class ExprList {

private:
  OL_Expression *head;

public:
  ExprList();
  ExprList( OL_Expression *elist);
  ~ExprList();

  OL_Expression *first();
  OL_Expression *next( OL_Expression *elem );

};

#endif
