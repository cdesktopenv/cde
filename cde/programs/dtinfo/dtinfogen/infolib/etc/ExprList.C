/* $XConsortium: ExprList.C /main/3 1996/08/21 15:46:33 drk $ */
/* imported interfaces */

/* exported interfaces */
#include "Expression.h"
#include "ExprList.h"

//------------------------------------------------------------------
ExprList::ExprList()
{
  head = 0;
}

//------------------------------------------------------------------
ExprList::ExprList( OL_Expression *elist )
{
  head = elist;
}

//------------------------------------------------------------------
ExprList::~ExprList()
{
  OL_Expression *ptr = head;
  while ( ptr ) {
    OL_Expression *tmp = ptr;
    ptr = ptr->next;
    delete tmp;
  }
}
  
//------------------------------------------------------------------
OL_Expression *
ExprList::first()
{
  return ( head );
}

//------------------------------------------------------------------
OL_Expression *
ExprList::next( OL_Expression *ptr )
{
  if ( ptr ) { return(ptr->next); }
  else { return 0; }
}
