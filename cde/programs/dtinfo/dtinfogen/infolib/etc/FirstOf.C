/* $XConsortium: FirstOf.C /main/3 1996/08/21 15:46:47 drk $ */
/* exported interfaces */
#include "FirstOf.h"

/* imported interfaces */
#include "Dispatch.h"
#include "OLAF.h"
#include "ExprList.h"
#include "Expression.h"
#include "Token.h"
#include "OL-Data.h"
#include "ConcatTask.h"
#include "Content.h"
#include "AttributeData.h"
#include "GenericId.h"
#include "Literal.h"


//---------------------------------------------------------------------
FirstOf::FirstOf( const Token &t, 
		  ExprList *el, 
		  ActionType mode ):BaseData(t,mode)
{
  f_base = t.level();

  for ( OL_Expression *local_list = el->first();
	local_list;
	local_list = el->next(local_list) ) {

    switch ( local_list->type() ) {

      case CONTENT :
        addSubTask( new Content( t,mode) );
        break;
      
      case CONCAT :
        addSubTask( new Concat ( t, 
				 (ExprList *) local_list->data_list(),
				 mode) );
	break;

      case FIRSTOF :
	addSubTask( new FirstOf( t, 
				 (ExprList *)local_list->data_list(), 
				 mode) );
	break;

      case GENERIC_ID :
	addSubTask( new GenericId(t, 
				  local_list->name(),
				  mode) );
	break;

      case LITERAL :
	addSubTask( new Literal( t, 
				 (const char *)local_list->data_list(),
				 mode) );
	break;

      case REFERENCE :
	addSubTask( new AttributeData( t, 
				       local_list->name(),
				       mode ));
	break;

      default:
	abort();
   }

  }
}

//--------------------------------------------------------------------------
void
FirstOf::markup( const Token &t )
{

  if ( ignore_status && !Dispatch::OutsideIgnoreScope() ) {
    return;
  }

  if ( f_base > 0 ) {

    ComplexTask::markup( t );

    if ( t.type() == END ) {

      if ( t.level() == f_base ) {

	data_complete = 1;
	
	for ( int i = 0; i < ComplexTask::used; i++ ) {
	  BaseData *task = ( BaseData *)ComplexTask::subtask(i);
	  if ( task->DataIsComplete() ) {
	    ValueBuffer.writeStr( task->content() );
	    break;
	  }
	}

	f_base = -1;
	ComplexTask::removeAllSubTasks();

      }
    }
  }
}
    
	    


    

    
    

