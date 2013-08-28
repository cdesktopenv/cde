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
/* $XConsortium: NodeTask.C /main/6 1996/10/26 18:18:31 cde-hal $ */
/* $Id */

#include <sstream>
using namespace std;

/* exported interfaces... */
#include "NodeTask.h"

/* imported interfaces... */
#include <assert.h>
#include <assert.h>
#include <ctype.h>
#include "Dispatch.h"
#include "Token.h"
#include "LcfTask.h"
#include "OLAF.h"
#include "BookTasks.h"
#include "BookCaseDB.h"
#include "DataBase.h"
#include "OL-Data.h"
#include "NodeData.h"
#include "StyleTask.h"
#include "api/utility.h"

#ifdef FISH_DEBUG
#include "dbug.h" /* Fred Fish's dbug.h */
#endif

#define NUM_FIELDS 5

#define NULLTOEMPTY(x) ((x) ? (x) : "")

static int findDigits( int num ) 
{
  char str[NUM_FIELDS+1];
  snprintf( str, NUM_FIELDS+1, "%d", num );
  return( strlen(str) );
}

//--------------------------------------------------------------------

NodeTask::NodeTask(BookTask *book, NodeTask *parent)
{
  f_book = book;
  f_parent = parent;

  f_base = -1;
  section_element_name = NULL;
  ord = 1;
  
  f_title = NULL;
  f_shortTitle = NULL;

  f_locator = NULL;

  /* LCF task is managed explicitly instead of being pushed on the ComplexTask
   * list. It is to ensure that f_locator is available whenever f_lcf needs
   * it.
   */
  f_lcf = NULL;

  subnode_pending = 0;
  f_subnode = NULL;

  f_data = NULL;
  f_style = NULL;

}

//--------------------------------------------------------------------

NodeTask::~NodeTask()
{
  KILLSUBTASK(f_title);
  KILLSUBTASK(f_shortTitle);
  KILLSUBTASK(f_locator);

  if ( f_lcf ) {
    delete f_lcf;
    f_lcf = 0;
  }

  ComplexTask::removeAllSubTasks();
}


int NodeTask::checkNodeAF(const Token& t)
{
  int ret = 0;
  
  if(t.LookupAttr( OLAF::OL_Section )
     || t.LookupAttr( OLAF::OL_ToC ) ){
#ifdef FISH_DEBUG
    DBUG_PRINT("NodeTask", ("%lx <SECTION> found", this));
#endif

    ret = 1;
    
    if(f_base <= 0){ /* are we "idle"? */
      f_base = t.level();
      section_element_name = strdup(t.giName()); /*@# could be NULL! */

      if ( !Dispatch::RunTocGenOnly() ) {
	f_lcf = new LcfTask( this , t );
      }

      /*
       * first time we see OL-ID="...", spawn an OL_Data to collect the id
       */
      if(f_base > 0 && f_locator == NULL
	 && t.LookupAttr(OLAF::OL_id) ){
	f_locator = new OL_Data(t, OLAF::OL_id, REMOVE_SPACES);
	if ( f_locator->DataWillBeAvailable() ) {
	  addSubTask( f_locator );
	}
	else {
	  delete f_locator;
	  f_locator = NULL;
	}
      }

      if ( t.LookupAttr( OLAF::OL_Title ) && f_title == NULL ) {
	f_title = new OL_Data(t, OLAF::OL_Title, IGNORE_ON );
	if ( f_title->DataWillBeAvailable() ) {
	  addSubTask( f_title );
	}
	else {
	  delete f_title;
	  f_title = 0;
	}
      }

      if ( !Dispatch::RunTocGenOnly() ) {
	addSubTask ( f_data = new NodeData ( this, t ) );
      }

    }else{ /* not idle... must be in the middle of a node */
      if(!f_subnode){
	f_subnode = new NodeTask(f_book, this);
#ifdef FISH_DEBUG
	DBUG_PRINT("NodeTask", ("%lx spawned subnode %lx\n",
				  this, f_subnode));
#endif
      }
      subnode_pending = 1;
      f_subnode->markup(t);
	
    }
  }
      
  return ret;
}


//--------------------------------------------------------------------
void NodeTask::markup(const Token& t)
{
  if(subnode_pending){
    f_subnode->markup(t);
  }else{

    ComplexTask::markup(t);
    
    if(t.type() == START){
      /*
       * Process Node element start tags...
       */
      if(checkNodeAF(t)){
	/* work done in above routine */
      }
      
      /*
       * Process Node title start tags...
       */
      else { 

	if ( t.LookupAttr( OLAF::OL_ShortTitle ) ) {
	  if(f_base > 0 && f_shortTitle == NULL){
	    f_shortTitle = new OL_Data(t, OLAF::OL_ShortTitle, IGNORE_ON );
	    if ( !f_shortTitle->DataWillBeAvailable() ) {
	      delete f_shortTitle;
	      f_shortTitle = 0;
	    }
	    else {
	      addSubTask(f_shortTitle);
	    }
	  }
	}

	if ( t.LookupAttr( OLAF::OL_Title ) ) {
	  /* only grab the first title in a node */
	  if(f_base > 0 && f_title == NULL){
	    f_title = new OL_Data(t, OLAF::OL_Title, IGNORE_ON );
	    if ( !f_title->DataWillBeAvailable() ) {
	      delete f_title;
	      f_title = 0;
	    }
	    else {
	      addSubTask(f_title);
	    }
	  }
	}

      }

      
      if(f_base >= 0 && !f_style && t.LookupAttr(OLAF::OL_style)
	 && !subnode_pending ){
	OL_Data *tmp_style = new OL_Data(t, OLAF::OL_style, IGNORE_ON);
	if ( tmp_style->DataWillBeAvailable() ) {
	  f_style = tmp_style;
	  addSubTask(f_style);
	}
	else {
	  delete tmp_style;
	}
      }

      /*
       * first time we see OL-ID="...", spawn an OL_Data to collect the id
       */
      if(f_base > 0 && f_locator == NULL
	 && t.LookupAttr( OLAF::OL_id ) && !subnode_pending ){
	f_locator = new OL_Data(t, OLAF::OL_id, REMOVE_SPACES);
	if ( f_locator->DataWillBeAvailable() ) {
	  addSubTask( f_locator );
	}
	else {
	  delete f_locator;
	  f_locator = NULL;
	}
	
      }
    }

    /*
     * Let LCF task do its thing
     */
    if ( f_base > 0 && !subnode_pending && !Dispatch::RunTocGenOnly() ) { 
      f_lcf->markup(t); 
    }
    
    /*
     * End Tags... track nesting level, ...
     */
    if(t.type() == END){
      if(f_base > 0){
	
	if(t.level() == f_base){ /* found end of node...
				  * write out node meta data */
	  write_record();
	  reset();

	  if(f_parent){
	    f_parent->endSubNode(t);
	  }
	}
      }
      
    }

  }

}


void NodeTask::endSubNode(const Token& t)
{
  // f_lcf->setNode(this);
  subnode_pending = 0;
  markup(t);
}


int NodeTask::formatOrd(char * buf, int max)
{
  int ret;
  int ord_len = findDigits( ord );

  if ( ord_len > NUM_FIELDS ) {
    throw(Unexpected(form("No. of sections = %d have exceeded"
			  " the maximum number of sections allowed"
			  " at one level\n", ord )));
  }
  
  if(f_parent == NULL){
    int buflen = strlen(buf);
    assert( buflen + NUM_FIELDS < max );

    snprintf(buf, NUM_FIELDS + 1, "%05d", ord);  /* we assume max is big
						  * enough to format one int
						  */
    ret = buflen + NUM_FIELDS;

  }else{

    ret = f_parent->formatOrd(buf, max);
    int buflen = ret;
    assert ( buflen + NUM_FIELDS + 1< max );

    snprintf(buf + buflen, NUM_FIELDS + 2, ".%05d", ord);
    ret = buflen + NUM_FIELDS + 1;

  }

  return ret;
}


void NodeTask::write_record()
{
  if(f_title){
    const char *title = f_title->content();
    const char *stitle = title;
    char num[1024]; num[0]='\0';
    const char *style = styleName();
    
    if(f_shortTitle){
      stitle = f_shortTitle->content();
    }


    formatOrd(num, sizeof(num));
      
#ifdef FISH_DEBUG
    DBUG_PRINT("NodeTask", ("%lx loc=%s ord=%s title=`%s'\n",
			    this, locator(), num, title));
#endif

    DBTable *tbl = f_book->bookcase()->table(BookCaseDB::NodeMeta);
    tbl->insert(STRING_CODE, f_book->locator(),
		STRING_CODE, locator(),
		STRING_CODE, f_locator->filename(),
		INTEGER_CODE, f_locator->line_no(),
		STRING_CODE, num,
		STRING_CODE, title,
		STRING_CODE, stitle,
		STRING_CODE, style,
		NULL);
  }else{
    throw(Unexpected(form(
      "No section title available for the section element [%s]\n", 
    NULLTOEMPTY(section_element_name))));
  }
}


void NodeTask::reset()
{
  f_base = -1;
  delete section_element_name;
  ord++;

  KILLSUBTASK(f_title);
  KILLSUBTASK(f_locator);
  KILLSUBTASK(f_shortTitle);
  KILLSUBTASK(f_data);
  KILLSUBTASK(f_style);

  if ( f_lcf ) {
    delete f_lcf;
    f_lcf = 0;
  }

  if(f_subnode) f_subnode->ord = 1;
}


void NodeTask::data( const char *str, size_t sz)
{
  if(subnode_pending){
    f_subnode->data(str, sz);
  }else{
    ComplexTask::data(str, sz);
  
    if ( f_base > 0 && !Dispatch::RunTocGenOnly() ) { 
      f_lcf->data(str, sz); 
    }
  }
}


const char *NodeTask::locator()
{
  if(!f_locator) throw(Unexpected(form(
		       "No ID available for the section element [%s]\n",
				       NULLTOEMPTY(section_element_name)
    )));
    

  if ( !f_locator->DataIsComplete() ) {
     throw(Unexpected(form(
       "ID collection is not done yet for the section element [%s]\n",
			   NULLTOEMPTY(section_element_name)
       )));
  }

  return ( f_locator->content() );

}


const char *
NodeTask::styleName()
{

  if ( Dispatch::RunTocGenOnly() ) {
    return("");
  }

  const char *ret;
  
  if(f_style){
    ret = f_style->content();
    
    /* This is a hack to get the name of the style sheet right
     * Because sgmls can change the case sensitivity of the actual
     * style sheet name, I am going to use the case sensitive version
     * of the string first, if it doesn't exist, I will try the case
     * insensitive one, i.e. all uppercase
     */
    
    if(f_book->bookcase()->styleTask()->exist(ret)){
#ifdef FISH_DEBUG
      DBUG_PRINT("Style", ("node style is: %s", ret));
#endif
    }
    else {
      char *local_str = (char *)ret;
      for ( char *ptr = local_str; *ptr != '\0'; ptr++ ) {
	*ptr = toupper( *ptr );
      }

      /* try again */
      if ( f_book->bookcase()->styleTask()->exist(ret) ) {
#ifdef FISH_DEBUG
	DBUG_PRINT("Style", ("node style is: %s", ret));
#endif
      }
      else{
	Token::signalError(Token::User, Token::Continuable, 
			   f_style->filename(), f_style->line_no(),
			   form("Style `%s' not available\n", ret) );

	if ( f_parent ) { ret = f_parent->styleName(); }
	else { ret = f_book->styleName(); }
	
      }
    }
  }
  else{
    if ( f_parent ) { ret = f_parent->styleName(); }
    else { ret = f_book->styleName(); }
  }

  return ret;
}


//------------------------------------------------------------
const char *
NodeTask::title()
{
  if (!f_title) {
    throw(Unexpected(
      form(
	"No section title available for the section element [%s]\n", 
	   NULLTOEMPTY(section_element_name )
      )
      ));
  }

  return( f_title->content() );
}
