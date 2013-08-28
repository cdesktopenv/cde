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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: OL-Data.C /main/4 1996/08/21 15:47:10 drk $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/* exported interfaces */
#include "OL-Data.h"

/* imported interfaces */
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <misc/unique_id.h>
#include "Expression.h"
#include "AttributeRec.h"
#include "OLAF.h"
#include "SGMLName.h"
#include "Token.h"
#include "Task.h"
#include "Dispatch.h"
#include "SGMLDefn.h"


#include "AttributeData.h"
#include "Content.h"
#include "FirstOf.h"
#include "GenericId.h"
#include "Literal.h"
#include "ConcatTask.h"


// Debugging macro
#ifdef DEBUG
#define DBG(level) if ( dbgLevel >= level)
#else
#define DBG(level) if (0)
#endif

static int dbgLevel=-1;

//---------------------------------------------------------------------------
static char *
FilteredSpaces( const char *str )
{
  
  char *head;
  char *tail = (char *)str + strlen( str ) - 1;
  char *ptr = (char *)str;

  while (1) {
    if ( *ptr != ' ' && *ptr != '\n' && *ptr != '\t' ) {
      head = ptr;
      break;
    }
    else {
      ptr++;
    }
  }

  while (1) {
    if ( *tail != ' ' && *tail != '\n' && *tail != '\t' ) {
      *(tail + 1) = '\0';
      break;
    }
    else { tail--; }
  }

  return ( head );
}
//-------------------------------------------------------------------------
OL_Data::OL_Data( const Token &t,
		  int DataType,
		  ActionType mode ):BaseData(t,mode) 
{

  
  char *dbgStr;
  dbgStr = getenv("OL_DEBUG");
  dbgLevel = ( dbgStr ? atoi ( dbgStr ) : 0 );

  /* first generate grep all the mode info */
  istat = mode & IGNORE_ON;
  removeSpaces = mode & REMOVE_SPACES;

  f_name = strdup( t.file() );
  line_num = t.line();

  if ( istat && !Dispatch::OutsideIgnoreScope() ) {
    data_avail = 0;
    return;
  }
  
  const AttributeRec *attRec;
  if ( !(attRec = t.LookupAttr( DataType )) ) {

    if ( mode & GENERATE_ID ) {
      const char *str = unique_id();
      
      ValueBuffer.writeStr( str );
      data_avail = 1;
      data_complete = 1;
    }
    else {

      /*
       * The default rule kicks in, i.e use #CONTENT
       */

      addSubTask( new Content( t, mode) );
    }

    return;
  }
    
  tokContent.Parse( ( char *)attRec->getAttrValueString() );


  const OL_Expression *eptr = tokContent.exprlist;
  assert(eptr != NULL);
  
  switch ( eptr->type() ) {

    case REFERENCE:
      {
	BaseData *sub_data = new AttributeData(t, eptr->name(), mode );
	if ( sub_data ) {
	  if ( (data_avail= sub_data->DataWillBeAvailable()) ) {
	    ValueBuffer.write( sub_data->content(),
			       sub_data->content_size() );
	    data_complete = 1;
	  }
	  delete sub_data;
	}
      }

      break;
	
    case CONTENT:
      addSubTask( new Content(t, mode) );
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
      data_avail = 1;
      data_complete = 1;
      ValueBuffer.writeStr( ( const char *)eptr->data_list() );
      break;
    
    default:
      abort();
  }

}

//-------------------------------------------------------------------------
void
OL_Data::markup( const Token &t )
{

  DBG(80) cerr << "(DEBUG) OL_Data::markup() " << endl;
  DBG(80) cerr << "        t.giName() = " << t.giName() << endl;

  if ( istat && !Dispatch::OutsideIgnoreScope() ) {
    return;
  }

  if ( f_base > 0 ) {

    ComplexTask::markup( t );
    if ( t.type() == END ) {

      if ( f_base == t.level() ) {

	data_complete = 1;

	for ( int i=0; i < ComplexTask::used; i++ ) {
	  BaseData *task = ( BaseData *)ComplexTask::subtask(i);
	  ValueBuffer.write( task->content(), task->content_size() );
	}


	if ( removeSpaces && ValueBuffer.GetSize() > 0 ) {
	  const char *filtered_string = FilteredSpaces( ValueBuffer.GetBuffer() );
	  /* rewrite ValueBuffer with the filtered string */
	  ValueBuffer.reset();
	  ValueBuffer.writeStr( filtered_string );
	}
	  
	  
	DBG(10) printf("Data for <%s> = %s\n", t.giName(),
		       ValueBuffer.GetBuffer() );

	reset();

      }
    }

  }  /* if level > 0 */
}

//-------------------------------------------------------------------------
void
OL_Data::data( const char *str, size_t t )
{

  if ( istat && !Dispatch::OutsideIgnoreScope() ) {
    return;
  }

  if ( f_base > 0 ) {
    ComplexTask::data( str, t );
  }
}

//-------------------------------------------------------------------------
void
OL_Data::reset()
{
  f_base = -1;
  istat = removeSpaces = 0;
  removeAllSubTasks();
}
  

