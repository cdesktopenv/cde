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
/* $XConsortium: NodeData.C /main/4 1996/09/24 16:55:25 cde-hal $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

/* exported interfaces... */
#include "NodeData.h"
  
/* imported interfaces... */
#include <iostream>
#include <sstream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#include "Token.h"
#include "FlexBuffer.h"
#include "NodeTask.h"
#include "SearchEng.h"
#include "OLAF.h"
#include "SGMLName.h"
#include "OL-Data.h"
#include "BookTasks.h"
#include "DataBase.h"
#include "BookCaseDB.h"
#include "GraphicsTask.h"
#include "Dispatch.h"  
#include "api/utility.h"
#ifdef UseQSearch
#include "QSearch.h"
#else
#ifdef FULCRUM
#include "Fulcrum.h"
#else
#ifdef DTSEARCH
#include "AusText.h"
#endif
#endif
#endif

// Debugging macro
#ifdef DEBUG
#define DBG(level) if ( dbgLevel >= level)
#else
#define DBG(level) if (0)
#endif

static int dbgLevel=-1;


extern void ReplaceIdIdRef( NodeData *, char *, int );

//----------------------------------------------------------------
void 
replace_entity( FlexBuffer *buf , const char *str) 
{
  int len = strlen( str );
  int i;
  const char *ptr;

  for ( i = 0, ptr = str;
        i < len;
	i++, ptr++ ) {
    switch ( *ptr ) 
      {
      case '\n':
	buf->writeStr("&lnfeed;");
	break;

      case '&':
	buf->writeStr("&amp;");
	break;

      case '\240':
	buf->writeStr("&nbsp;");
	break;

      case '<':
	buf->writeStr("&lt;");
	break;

      default:
	buf->put( *ptr );
      }
  }
}
  

//--------------------------------------------------------------------
void
NodeData::write_start_tag( const Token &t, FlexBuffer *buffer )
{

  const AttributeRec *LinkRec = t.LookupAttr( OLAF::OL_idref );
  const AttributeRec *IdRec   = t.LookupAttr( OLAF::OL_id    );
  const AttributeRec *XRefRec = t.LookupAttr( OLAF::OL_XRef  );

  const AttributeRec *GraphicExist = t.LookupAttr( OLAF::OL_Graphic );

  if ( t.LookupAttr( OLAF::OL_Ignore ) ) {
    if ( IdRec ) {

      buffer->writeStr("<%BOGUS><#><#OL-ID>");
      
      char *str = form("%d", seq_no );
      buffer->writeStr( str );
      
      buffer->writeStr( "</#OL-ID><#LAST>0</#LAST></#></%BOGUS>" );
      
      seq_no++;
      addSubTask( new OL_Data ( t, OLAF::OL_id, REMOVE_SPACES ) );

      return;
      
    }
  }

  if ( Dispatch::OutsideIgnoreScope() ) {
    
    /* write the start tag */
    char *str = form("<%s>", t.giName() );
    buffer->writeStr( str );

    /* write out all the attribute name and value */
//    if ( t.GetFirstAttr() || LinkRec || IdRec || GraphicExist ) {
// Always output the attribute tag container because we'll always have
// position data.
    buffer->writeStr ( "<#>" );
//    }
  
    for ( const AttributeRec *a = t.GetFirstAttr();
	  a;
	  a = t.GetNextAttr(a) ) {

      /* write the start tag */

      const char *attName = SGMLName::lookup( a->getAttrName() );

      /* write the attribute start tag */
      char *start_tag = form("<%s>",attName);
      buffer->writeStr( start_tag );

      /* write the attribute value */
      replace_entity ( buffer, a->getAttrValueString() );
      
      /* write the attribute end tag */
      char *end_tag = form("</%s>", attName );
      buffer->writeStr( end_tag );

    }

    if ( LinkRec ) {
      char *str = form ( "<#OL-IDREF>%d</#OL-IDREF>", seq_no );
      buffer->writeStr(str);
      seq_no++;
      addSubTask( new OL_Data ( t, OLAF::OL_idref, REMOVE_SPACES ) );
    }

    if ( GraphicExist ) {
      char *str = form("<#GRAPHIC>%d</#GRAPHIC>", seq_no );
      buffer->writeStr( str );
      seq_no++;
      current_graphics_id = new OL_Data(t, OLAF::OL_id, GENERATE_ID);
      addSubTask( current_graphics_id );
    }
    
    if ( IdRec ) {
      char *str = form("<#OL-ID>%d</#OL-ID>", seq_no );
      buffer->writeStr( str );
      seq_no++;

      addSubTask( new OL_Data ( t, OLAF::OL_id, REMOVE_SPACES ) );
    }

    if ( XRefRec ) {
      char *str = form("<#OL-XREF>%d</#OL-XREF>", seq_no );
      buffer->writeStr( str );
      seq_no++;

      addSubTask( new OL_Data ( t, OLAF::OL_XRef, REMOVE_SPACES ) );
    }


    if ( GraphicExist ) {
      /*
       * delay the writing of </#> until potential search terms are generated
       */
      return;
    }
  
//    if ( t.GetFirstAttr() || LinkRec || IdRec ) {
      buffer->writeStr("<#LAST>0</#LAST></#>");
//    }
    
  }
}

//---------------------------------------------------------------------
static void
write_end_tag( const Token &t, FlexBuffer *buffer)
{

  if ( Dispatch::OutsideIgnoreScope() ) {
    char *str = form("</%s>", t.giName() );
    buffer->writeStr( str );
  }
	
}
  
//---------------------------------------------------------------------
NodeData::NodeData( NodeTask *parent, const Token &t)
{

  char *dbgStr;
  dbgStr = getenv ( "OL_DEBUG");
  dbgLevel = ( dbgStr ? atoi(dbgStr) : 0 );
  
  f_node  = parent;
  f_base = t.level();
  CollectObject = -1;
  NodeBuffer    = new FlexBuffer();
  DbBuffer      = new FlexBuffer();
  seq_no = 0;

  current_graphics_id = NULL;

  /* f_search is managed explicitly */
  /* This probably has to be #ifdef FULCRUM */
#ifdef FULCRUM
  f_search = new Fulcrum ( this, t );
#else
#ifdef UseQSearch
  f_search  = new QSearch ( this, t );
#else
#ifdef DTSEARCH
  f_search = new AusText( this, t );
#endif
#endif
#endif

  write_start_tag ( t, NodeBuffer );
}

//---------------------------------------------------------------------
NodeData::~NodeData()
{
  assert(f_base < 0); /* for debugging NodeTask/NodeData interaction */
  
  reset();

  delete NodeBuffer;
  delete DbBuffer;
}


//---------------------------------------------------------------------
void
NodeData::markup( const Token &t )
{

  if ( f_base > 0 ) {

    ComplexTask::markup(t);
    if ( f_search ) {
      f_search->markup( t );
    }
  
    if ( t.type() == START ) {
      
	if ( CollectObject >= 0 ) {
	  write_start_tag ( t, internal_buffer );
	}
	else {
	  
	  if ( t.LookupAttr( OLAF::OL_Graphic ) ) {
	    CollectObject = t.level();
	    internal_buffer = new FlexBuffer();
	    assert(internal_buffer != NULL);
	  }

	  write_start_tag ( t, NodeBuffer );
	}
      
    }
      
    else if ( t.type() == END ) {
      
      if ( Dispatch::OutsideIgnoreScope() ) {
	
	if ( CollectObject >= 0 ) {
	
	  if ( t.level() == CollectObject ) {
	    if ( f_search->HasSearchTerms() ) {
	      FlexBuffer *buffer = (FlexBuffer *)
		                      f_search->DumpSearchTerms();
	    
	      /*
	       * write out the searchable terms in attribute format
	       */

	      NodeBuffer->writeStr("<#TERMS>");
	      NodeBuffer->write ( buffer->GetBuffer(),
				  buffer->GetSize() );
	      NodeBuffer->writeStr("</#TERMS>");
	    }
	  
	    NodeBuffer->writeStr("<#LAST>0</#LAST></#>");

	    /* write out all the other data/tags that are found within the
	     * graphic tags
	     */
	    *NodeBuffer = *NodeBuffer + *internal_buffer;
	    
	    write_end_tag ( t, NodeBuffer );

	    current_graphics_id = NULL;
	    CollectObject = -1;
	    delete internal_buffer;
	  }
	  else {
	    write_end_tag ( t, internal_buffer );
	  }
	}
	else { /* ie not collecting oject */
	  write_end_tag( t , NodeBuffer);
	}
	
	if ( f_base == t.level() ) {
	  write_record();
	  reset();
	}
      }
    }   /* if ( t.type() == END ) */
    
  }     /* if ( f_base > 0 ) */
}

//---------------------------------------------------------------------
void
NodeData::reset()
{
  f_base = -1;

  NodeBuffer->reset();
  DbBuffer->reset();

  if ( f_search ) {
    delete f_search;
    f_search = NULL;
  }

  ComplexTask::removeAllSubTasks();
    
}

//---------------------------------------------------------------------
void
NodeData::data ( const char *str, size_t t )
{

  if ( f_base > 0 ) {
    
    ComplexTask::data( str, t );
    if ( f_search ) {
      f_search->data( str, t );
    }
    
    if ( Dispatch::OutsideIgnoreScope() ) {
      if (CollectObject < 0) {
	replace_entity( NodeBuffer, str );
      }
      else {
	replace_entity( internal_buffer, str );
      }
    }
  }
  
}

//---------------------------------------------------------------------
void
NodeData::write_record()
{

  DBG(20) printf("Node Before replacement is %s\n", NodeBuffer->GetBuffer());
  
  ReplaceIdIdRef( this, (char *)NodeBuffer->GetBuffer(),
		  NodeBuffer->GetSize() );
  /*
   * write out the whole record
   */
  const char *TocLocator  = f_node->book()->locator();
  const char *NodeLocator = f_node->locator();

  DBTable *tbl = f_node->book()->bookcase()->table(BookCaseDB::NodeSGML);
  tbl->insert(STRING_CODE,  TocLocator,
	      STRING_CODE,  NodeLocator,
	      -STRING_CODE, DbBuffer->GetBuffer(), (size_t)DbBuffer->GetSize(),
	      NULL);

  /*
  DBTable *ltab = f_node->book()->bookcase()->table(BookCaseDB::Link);
  for ( int i = 0; i < ComplexTask::used; i++ ) {
    OL_Data *Task = ( OL_Data * )ComplexTask::subtask(i);

    ltab->insert( STRING_CODE, NodeLocator,
		  INTEGER_CODE, StartingSeqNo + i,
		  STRING_CODE, Task->content(),
		  NULL
		  );

  }
  */
  
}  

//---------------------------------------------------------------------
const char *
NodeData::graphics_id()
{
  if ( !current_graphics_id ) {
    throw(Unexpected("Graphics ID not available"));
  }

  if ( current_graphics_id->ContentIsEmpty() ) {
    throw(Unexpected("An ID could not be found for the graphical object"));
  }
  
  return ( current_graphics_id->content() );
}
