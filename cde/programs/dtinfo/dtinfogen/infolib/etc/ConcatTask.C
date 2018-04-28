/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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

  
