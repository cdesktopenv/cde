/* $XConsortium: Expression.C /main/3 1996/08/21 15:46:40 drk $ */
/* imported interfaces */

#include "Expression.h"
#include "ExprList.h"

OL_Expression::~OL_Expression()
{
  /* This is a hack right now to eliminate the memory leak
   */
  if ( data_type == CONCAT || data_type == FIRSTOF ) {
    ExprList *vlist = ( ExprList * )value_list;
    delete vlist;
  }
  else if ( data_type == LITERAL ) {
    char *vlist = (char *)value_list;
    delete vlist;
  }

}
