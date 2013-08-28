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
/* $XConsortium: BookTasks.cc /main/5 1996/05/29 12:36:58 rcs $ 
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */


#include <sstream>
using namespace std;

/* import... */
#include <assert.h>
#include <string.h>
#include "Dispatch.h"
#include "Token.h"
#include "OLAF.h"
#include "NodeTask.h"
#include "TOCTask.h"
#include "BookCaseDB.h"
#include "OL-Data.h"
#include "StringList.h"
#include "StyleTask.h"
#include "api/utility.h"

/* export... */
#include "BookTasks.h"

#ifdef LICENSE_MANAGEMENT
#include "cryptlib/lterms.h"
#endif

#ifdef FISH_DEBUG
#include "dbug.h"  /* ala Fred Fish's dbug package from uunet */
#endif

const int A_FEATURE = OLAF::Feature;
const int A_VEN_CODE = OLAF::VenCode;
const int A_VERSION  = OLAF::Version;
const int A_GROUPING = OLAF::Grouping;
const int A_DEMO_TERMS = OLAF::DemoTerms;
const int A_DEFAULT_SECTION = OLAF::DefaultSection;

/***********************************
 *
 * BookCaseTask
 *
 ***********************************/
  
BookCaseTask::BookCaseTask(const char *infolib)
{
  int len = strlen(infolib);
  library = new char[len + 1];
  *((char *) memcpy(library, infolib, len) + len) = '\0';

  f_base = -1;

  bookCaseName = NULL;
  bookCaseDesc = NULL;
  
  f_db = NULL;
  
  if ( !Dispatch::RunTocGenOnly() ) {
    style = new StyleTaskDB(this);
    addSubTask(style);
  }

  book = new BookTask(this);
  addSubTask(book);

  f_style = NULL;
}


//--------------------------------------------------------------------
BookCaseDB*
BookCaseTask::database()
{
  if(!f_db){
    f_db = new BookCaseDB(library);
  }

  return f_db;
}


//--------------------------------------------------------------------
DBTable *
BookCaseTask::table(int tid)
{
  return database()->table(tid);
}

//--------------------------------------------------------------------
const char *
BookCaseTask::bookcasename()
{
  if ( !bookCaseName ) {
    throw(Unexpected("BookCase name not available yet."));
  }

  return ( bookCaseName->content() );
}

/*
//--------------------------------------------------------------------
void
BookCaseTask::write_full_text_record( const char *str,
				      int sz,
				      const char *nodelocator,
                                      const char *node_title
 )
{
  const char *BookCaseName = bookcasename();
  
  if ( !f_search_storage ) {
    const char *pathname = database()->path();

    f_search_storage = new FulcrumStore( pathname , BookCaseName);
    assert ( f_search_storage );
  }

  const int   BookNum      = book->sequencenum();
  const char  *BookShortTitle = book->book_short_title();

  f_search_storage->insert( BookCaseName,
			    BookNum,
			    BookShortTitle,
			    nodelocator,
                            node_title,
			    str,
			    sz );
}
*/
/*
//--------------------------------------------------------------------
void
BookCaseTask::write_full_text_record(DataRepository *store,
				     const char *nodelocator,
				     const char *node_title
)
{

  const char *BookCaseName = bookcasename();

  if ( !f_search_storage ) {
    const char *pathname = database()->path();
    f_search_storage = new AusTextStore( pathname, BookCaseName );
    assert( f_search_storage );
  }

  const char  *BookShortTitle = book->book_short_title();
  f_search_storage->insert( BookShortTitle,
			    nodelocator,
			    node_title,
			    store
                          );
}
  
*/  
  
//--------------------------------------------------------------------
void BookCaseTask::markup(const Token &t)
{
  ComplexTask::markup(t);

  if (t.type() == START){

    if(t.olaf() == OLAF::Bookcase){
      if(f_base >= 0){
	throw(Unexpected("second (illegal) BookCase element found"));
      }
      
      f_base = t.level();
    }

    if(f_base >= 0){
      switch(t.olaf()){
	
      case OLAF::BcName:
	if(bookCaseName){
	  throw(Unexpected("second (illegal) bookcase name element found"));
	}
	
	bookCaseName = new OL_Data(t, OLAF::OL_data);
	addSubTask(bookCaseName);
	break;

      case OLAF::BcDesc:
	if(bookCaseDesc){
	  throw(Unexpected("second (illegal) bookcase description element found"));
	}
	
	bookCaseDesc = new OL_Data(t, OLAF::OL_data);
	addSubTask(bookCaseDesc);
	break;

      }
    
      if(!f_style && t.LookupAttr(OLAF::OL_style)){
	OL_Data *tmp_style = new OL_Data(t, OLAF::OL_style);
	if ( tmp_style->DataWillBeAvailable() ) {
	  f_style = tmp_style;
	  addSubTask(f_style);
	}
	else {
	  delete tmp_style;
	}
      }
    }
  }

  else if(t.type() == END){

    if(t.level() == f_base){
      const char *name;
      const char *desc;
      
      if(bookCaseName){
	name = bookCaseName->content();
      }else{
	throw(Unexpected("No bookcase name element in Bookcase element."));
      }

      if(bookCaseDesc){
	desc = bookCaseDesc->content();
      }else{
	desc = "";
      }
      
      printf("BookCase name: `%s' desc: `%s'\n", name, desc);
    }

    /* @# warn if no bookcase ever found? */
  }
}


const char *
BookCaseTask::styleName()
{
  const char *ret;

  if ( Dispatch::RunTocGenOnly() ) {
    return("");
  }

  if(!f_style) {
    throw(Unexpected ("No style architectural form defined for bookcase."));
  }

  ret = f_style->content();

#ifdef FISH_DEBUG
  DBUG_PRINT("Style", ("bookcase style is: %s", ret));
#endif
  
  if(!style->exist(ret)){
#ifdef FISH_DEBUG
    DBUG_PRINT("Error", ("style `%s' not found", ret));
#endif
    Token::signalError(Token::User, Token::Fatal, NULL, 0,
		       "An undeclared style sheet name `%s' was found in the bookcase specification.\n", ret);
  }
  
  return ret;
}
    

/*****************
 *
 * BookTask
 *
 *****************/

BookTask::BookTask(BookCaseTask *bc)
{
  f_base = 0;
  f_seq_no = 1;

  f_toc = NULL;
  f_bookcase = bc;
  
  tocLocator = NULL;

  shortTitle = NULL;
  title = NULL;

  tabName = NULL;
  tabLocator = NULL;
  
  tabNames = new StringList();
  tabLocators = new StringList();
  tabLines = new StringList();
  tabFiles = new StringList();
  
  f_node = new NodeTask(this, NULL);
  addSubTask(f_node);

  f_style = NULL;

  e_string = NULL;
  e_len    = 0;
}


BookTask::~BookTask()
{
  KILLSUBTASK(f_node);

  delete tabLocators;

  KILLSUBTASK(title);
  KILLSUBTASK(shortTitle);
  KILLSUBTASK(tabName);
  KILLSUBTASK(tabLocator);
  
}

//------------------------------------------------------------------
void
BookTask::encrypt( const Token &t )
{
  /*
   * Grab all the strings that are required by the encryption API
   */

  char buf[ 256 ];

#ifdef LICENSE_MANAGEMENT
  LTerms lt;

  char *a_val;
      
  for ( const AttributeRec *arec = t.GetFirstAttr();
	arec;
	arec = t.GetNextAttr( arec ) ) {

    a_val = ( char * )arec->getAttrValueString();
    
#ifdef FISH_DEBUG
    DBUG_PRINT("BookTasks", ("access attribute value %s", a_val) );
#endif
	
    switch ( arec->getAttrName() ) {
      
      case A_FEATURE :

        if ( lt.add_feature( a_val ) != 0 ) {
	  throw(Unexpected("invalid access feature syntax"));
	}
	break;

      case A_VEN_CODE :
	if ( lt.add_ven_code( a_val ) != 0 ) {
	  throw(Unexpected("invalid access ven_code syntax") );
	}
	break;

      case A_VERSION :
	if ( lt.add_version( a_val ) !=  0 ) {
	  throw(Unexpected("invalid access version syntax"));
	}
	break;

      case A_GROUPING :
	if ( lt.add_grouping( a_val ) != 0 ) {
	  throw(Unexpected("invalid access grouping syntax"));
	}
	break;

      case A_DEMO_TERMS :

	if ( lt.add_demo_terms( a_val ) != 0 ) {
	  throw(Unexpected("invalid access demo_terms syntax"));
	}
	break;

      case A_DEFAULT_SECTION :

	if ( lt.add_noaccess_locator( a_val ) != 0 ) {
	  throw(Unexpected("invalid default section ID syntax"));
	}
	break;

      }
  }

  if ( lt.pack( buf, 256 ) ) {
    throw(Unexpected("Unable to pack the string for encryption"));
  }

  int len = 256;
  if ( e_terms( &lt, buf, len ) ) {
    throw(Unexpected("Unable to encrypt string for access control"));
  }
#else
  int len = 256;
  (void) memset(buf, '\0', len);
#endif

  e_string = (char *)malloc(len);
  (void)memcpy(e_string, buf, len);	// Cannot use strdup - embedded NULs
  e_len    = len;
  
}

//----------------------------------------------------------------------
void BookTask::markup(const Token &t)
{
  ComplexTask::markup(t);
  
  if(t.type() == START){
    
    if(t.olaf() == OLAF::Book){
      if(f_base > 0){
	throw(Unexpected("illegal nested BOOK architectural form"));
      }

      f_base = t.level();
    }

    if(f_base >= 0){
      switch(t.olaf()){
      case OLAF::BkSTitle:
	if(shortTitle){
	  throw(Unexpected("BkSTitle already found"));
	}

	shortTitle = new OL_Data(t, OLAF::OL_data);
	addSubTask(shortTitle);
	break;

      case OLAF::BkTitle:
	if(title){
	  throw(Unexpected("BkTitle already found"));
	}

	title = new OL_Data(t, OLAF::OL_data);
	addSubTask(title);
	break;

      case OLAF::Tab:
	if(tabName){
	  tabNames->append(tabName->content());
	  tabLocators->append(tabLocator->content());
	  tabLines->append( form("%d", tabLocator->line_no()) );
	  tabFiles->append( tabLocator->filename() );
	  
	  KILLSUBTASK(tabName);
	  KILLSUBTASK(tabLocator);
	}
      
	tabName = new OL_Data(t, OLAF::OL_data);
	tabLocator = new OL_Data(t, OLAF::OL_idref, REMOVE_SPACES);
	addSubTask(tabName);
	addSubTask(tabLocator);
	break;

      case OLAF::BookAccess:
	encrypt( t );
	break;
      }

      if ( t.LookupAttr( OLAF::OL_ToC ) ) {
	if ( f_toc ) {
	   throw(Unexpected("An illegal TOC was found.\n"));
	}
      
	f_toc = new TOCTask(t, this);
	addSubTask(f_toc);
      }

      if(f_base >= 0 && !f_style && t.LookupAttr(OLAF::OL_style)){
	OL_Data *tmp_style = new OL_Data(t, OLAF::OL_style);
	if ( tmp_style->DataWillBeAvailable() ) {
	  f_style = tmp_style;
	  addSubTask(f_style);
	}
	else {
	  delete tmp_style;
	}
      }
    }
  }

  else if(t.type() == END){
    if(t.level() == f_base){ /* found end of book... write out CCF data */
      if ( !Dispatch::RunTocGenOnly() ) {
	write_record();
      }

      reset();
    }
  }
}

//----------------------------------------------------------------------
void BookTask::write_record(void)
{
  StringList tablines;

  /* finish any pending tab */
  if(tabName){
    tabNames->append(tabName->content());
    tabLocators->append(tabLocator->content());
    tabLines->append( form("%d", tabLocator->line_no()) );
    tabFiles->append(tabLocator->filename());
  }
      
  for(unsigned int i = 0; i < tabNames->qty(); i++){
    const char *name = tabNames->item(i);
    const char *loc = tabLocators->item(i);
    const char *line = tabLines->item(i);
    const char *file = tabFiles->item(i);

    int plen = strlen(name) + 1 + strlen(loc) + 1
			    + strlen(line) + 1 + strlen(file) + 1;
    char *p = new char [plen];

    snprintf(p, plen, "%s\t%s\t%s\t%s", name, loc, line, file);
    tablines.append(p);

    delete p;
  }

  const char *bk_title;

  if(title){
    bk_title = title->content();
  }else{
    throw(Unexpected("Required Title form missing from Book"));
  }
    
  const char *bk_stitle = bk_title;

  if ( shortTitle ) {
    bk_stitle = shortTitle->content();
  }
  
  DBTable *tbl = bookcase()->table(BookCaseDB::BookMeta);

  tbl->insert(STRING_CODE, locator(),
		STRING_CODE, bk_stitle,
		STRING_CODE, bk_title,
		INTEGER_CODE, f_seq_no,
		SHORT_LIST_CODE, tablines.qty(), STRING_CODE, tablines.array(),
		-STRING_CODE, e_string , (size_t)e_len,
		NULL);

#ifdef FISH_DEBUG
  DBUG_PRINT("Book", ("Book... title: `%s' short title: `%s'\n",
		      bk_title,
		      bk_stitle));
#endif
}


void BookTask::reset()
{
  KILLSUBTASK(shortTitle);
  KILLSUBTASK(title);
  KILLSUBTASK(f_toc);
  KILLSUBTASK(f_style);
 
  delete tabNames; tabNames = new StringList();
  delete tabLocators; tabLocators = new StringList();

  tabName = tabLocator = NULL;

  delete tabLines; tabLines = new StringList();
  delete tabFiles; tabFiles = new StringList();

  delete tocLocator; tocLocator = NULL;

 
  f_seq_no ++;
  f_base = 0;

}


const char *BookTask::locator()
{

  if ( Dispatch::RunTocGenOnly() ) {
    return ("");
  }

  if(!tocLocator){
    /* this is the first time anybody asked for the book's locator...
     * it must be the TOC node asking for the book locator, which
     * is the TOC node locator!
     */
    const char *l = f_node->locator();
    int len = strlen(l);
    tocLocator = new char[len + 1];
    *((char *) memcpy(tocLocator, l, len) + len) = '\0';
  }
  
  return tocLocator;
}


const char *
BookTask::styleName()
{
  if ( Dispatch::RunTocGenOnly() ) {
    return("");
  }

  const char *ret;
  if(f_style){
    ret = f_style->content();
    
    if(f_bookcase->styleTask()->exist(ret)){
#ifdef FISH_DEBUG
      DBUG_PRINT("Style", ("book style is: %s", ret));
#endif
    }else{
      Token::signalError(Token::User, Token::Continuable, NULL, 0,
			 "no such style `%s'\n", ret);
      ret = f_bookcase->styleName();
    }
  }else{
    ret = f_bookcase->styleName();
  }

  return ret;
}

//--------------------------------------------------------------
const char *
BookTask::book_title()
{
  if ( !title ) {
    throw(Unexpected("Book title is not available yet"));
  }
  
  return( title->content() );
}

//--------------------------------------------------------------
const char *    
BookTask::book_short_title()
{
  if ( !shortTitle ) { 
    return( book_title() );
  }

  return( shortTitle->content() );
}
  
