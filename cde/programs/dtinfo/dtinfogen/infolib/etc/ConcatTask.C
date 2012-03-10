/* $XConsortium: ConcatTask.C /main/3 1996/08/21 15:46:12 drk $ */
/* exported interfaces */
#include "ConcatTask.h"

/* imported interfaces */
#include "Expression.h"
#include "ExprList.h"
#include "Token.h"
#include "AttributeData.h"
#include "Content.h"
#include "FirstOf.h"
#include "GenericId.h"
#include "Literal.h"

Concat::Concat( const Token &t, 
		ExprList *elist,
		ActionType mode):OL_Data(t, mode)
{
  
  for ( OL_Expression *eptr = elist->first();
	eptr;
	eptr = elist->next(eptr) ) {
    switch ( eptr->type() ) {

      case REFERENCE:
        addSubTask( new AttributeData( t, eptr->name(), mode ));
	break;
	
      case CONTENT:
        addSubTask( new Content(t,mode) );
	break;

      case CONCAT:
        addSubTask( new Concat( t, 
				(ExprList *)eptr->data_list(),
				mode) );
	break;

      case FIRSTOF:
	addSubTask( new FirstOf( t,
				 (ExprList *)eptr->data_list(),
				 mode) );
	break;

      case GENERIC_ID:
	addSubTask( new GenericId( t,
				   eptr->name(),
				   mode) );
	break;

      case LITERAL:
        addSubTask( new Literal( t, 
				 ( const char *)eptr->data_list(),
				 mode) );
        break;
    
      default:
	abort();
    }
  }
}

  
