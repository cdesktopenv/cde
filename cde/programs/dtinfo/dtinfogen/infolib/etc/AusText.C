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
/* $XConsortium: AusText.C /main/3 1996/09/10 16:04:23 cde-hal $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/* imported interfaces */
#include "OL-Data.h"
#include "Task.h"
#include "FlexBuffer.h"
#include "Token.h"
#include "OLAF.h"
#include "BookCaseDB.h"
#include "BookTasks.h"
#include "NodeData.h"
#include "NodeTask.h"
#include "GraphicsTask.h"
#include "BookTasks.h"
#include "Dispatch.h"
#include "SGMLName.h"
#include "DataRepository.h"
#include "AusTextStorage.h"

/* exported interfaces */
#include "AusText.h"

const unsigned char WORD_SEP = ' ';

// charater strings
const int OLIAS_SCOPE_TITLE   = OLAF::Title;
const int OLIAS_SCOPE_EXAMPLE = OLAF::Example;
const int OLIAS_SCOPE_INDEX   = OLAF::Index;
const int OLIAS_SCOPE_TABLE   = OLAF::Table;
const int OLIAS_SCOPE_GRAPHIC = OLAF::Graphic;
    
//-----------------------------------------------------------------------
AusText::AusText(NodeData *f_node, const Token &t):SearchEngine(f_node, t)
{
  store = new DataRepository;
  f_search_store = NULL;

  // NOTE: We need to activate default zone here because AusText::markup
  //       is not being called for the top element in the section.
  //       Do not forget to deactivate it on the other end!
  store->ActivateZone ( DataRepository::Default, t.level() );
}

//-----------------------------------------------------------------------
AusText::~AusText()
{
  store->deActivateZone ( f_base );

  if ( store ) { delete store; }
  if ( f_search_store ) { delete f_search_store; }
}

//-----------------------------------------------------------------------
void
AusText::write_start_tag ( const Token &t)
{

  const AttributeRec *a;
  
  int level = t.level();
  
  if ( (a = t.LookupAttr( OLAF::OL_scope )) ) {

    switch ( a->getAttValue() ) {

      case OLIAS_SCOPE_EXAMPLE :
	store->ActivateZone( DataRepository::Example, level );
	break;

      case OLIAS_SCOPE_GRAPHIC :
	store->ActivateZone( DataRepository::Graphic, level );
	break;

      case OLIAS_SCOPE_INDEX   :
	store->ActivateZone( DataRepository::Index, level );
	break;

      case OLIAS_SCOPE_TABLE   :
	store->ActivateZone( DataRepository::Table, level  );
	break;

      case OLIAS_SCOPE_TITLE   :
	store->ActivateZone( DataRepository::Head, level );
	break;
			   
      default            : throw(Unexpected("Not a valid zone"));


    }

  }
#if 0
  else {
    store->ActivateZone ( DataRepository::Default, level );
  }
#endif

  store->put( WORD_SEP );
  
}


//------------------------------------------------------------------------
void
AusText::write_end_tag( const Token & t)
{
  store->deActivateZone( t.level() );
  store->put( WORD_SEP );
}
  
//------------------------------------------------------------------------
void 
AusText::write_terms( FlexBuffer *termsbuf )
{
  int bufsize = termsbuf->GetSize();
  const char *buffer = termsbuf->GetBuffer();

  store->write( buffer, bufsize );
  termsBuffer = termsbuf;
}

//------------------------------------------------------------------------
void 
AusText::write_buffer()
{
  BookCaseTask *f_bc = f_parent->node()->book()->bookcase();
  const char *BookCaseName = f_bc->bookcasename();
  const char *pathname     = f_bc->database()->path();

  f_search_store = new AusTextStore( pathname, BookCaseName );
  assert(f_search_store != NULL);

  const char *book_short_title = f_parent->node()->book()->book_short_title();
  const char *book_id          = f_parent->node()->book()->locator();
  const char *nodelocator = f_parent->node()->locator();
  const char *node_title  = f_parent->node()->title();

  f_search_store->insert(book_short_title,
			 book_id,
			 nodelocator,
			 node_title,
			 store
    );
}

//------------------------------------------------------------------------
void
AusText::markup( const Token & t )
{
  if ( f_base > 0 ) {
    
    ComplexTask::markup(t);
    
    if ( t.type() == START ) {


      if ( Dispatch::OutsideIgnoreScope() ) {
	if ( t.LookupAttr( OLAF::OL_Graphic ) ) {
	  CollectObject = t.level();
	  
	  f_graphics = new GraphicsTask ( this, t );
	  assert ( f_graphics != NULL );
	  addSubTask( f_graphics );
	  
	}

#if TABLES
	else if( t.LookupAttr( OLAF::OL_Table ) ) {
	  CollectObject = t.level();
	  addSubTask(new SearchableTableTask(this, t));
	}
#endif
	
	write_start_tag ( t );

      }
    }
    else if ( t.type() == END ) {

      if ( Dispatch::OutsideIgnoreScope() ) {
	if ( CollectObject >= 0 ) {

	  if ( CollectObject == t.level() ) {
	  
	    CollectObject = -1;

	    if ( f_graphics ) {
	      if ( f_graphics->HasSearchTerms() ) {
		hasTerms = 1;
		FlexBuffer *buf = (FlexBuffer *)f_graphics->GetTerms();
		write_terms ( buf );
	      }
	      KILLSUBTASK( f_graphics );
	    }

	  }
	  
	}
	
	
	write_end_tag( t );
      
	if ( f_base == t.level() ) {
	  f_base = -1;
	  write_buffer();
	}
      } /* OutsideIgnoreScope */
      
    } /* if ( t.type() == END ) */

  } /* if ( f_base > 0 ) */
}

//---------------------------------------------------------------------
void 
AusText::data  ( const char *str, size_t sz )
{

  if ( f_base > 0 ) {
    ComplexTask::data( str, sz );

    if ( Dispatch::OutsideIgnoreScope() ) {

      /* @@ for tables, this isn't quite right... */
      if ( CollectObject < 0 ) {


	if ( sz == 0 ) { return; }
	store->write( str, sz );
      }
    }
  }
}
