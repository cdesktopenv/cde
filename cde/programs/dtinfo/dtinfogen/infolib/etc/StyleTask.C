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
/* $XConsortium: StyleTask.cc /main/7 1996/07/18 15:20:02 drk $ */
/* $XConsortium: StyleTask.cc /main/7 1996/07/18 15:20:02 drk $ */
/* $XConsortium: StyleTask.cc /main/7 1996/07/18 15:20:02 drk $ */
/* $XConsortium: StyleTask.cc /main/7 1996/07/18 15:20:02 drk $ */
/* $XConsortium: StyleTask.cc /main/7 1996/07/18 15:20:02 drk $ */
/* $XConsortium: StyleTask.cc /main/7 1996/07/18 15:20:02 drk $ */
/* $XConsortium: StyleTask.cc /main/7 1996/07/18 15:20:02 drk $ */
/* export... */
#include "StyleTask.h"

/* import... */
#include <assert.h>

#include "AttributeRec.h"
#include "DataBase.h"
#include "FlexBuffer.h"
#include "OLAF.h"
#include "OL-Data.h"
#include "SGMLName.h"
#include "StringList.h"
#include "Token.h"

#ifdef FISH_DEBUG
#include "dbug.h" /* Fred Fish's dbug.h */
#endif

/*****************
 *
 * StyleTask
 *
 *****************/

static unsigned hash_func(const CC_String &str) 
{
  return str.hash();
}

inline
static
void write_tabs( FlexBuffer *buf, Stack<int> *fstack, char ch )
{
  int num_ch = fstack->entries() + 1;

  for ( int i=0; i < num_ch; i++ ) {
    buf->put(ch);
  }
}

    
StyleTask::StyleTask()
{
   f_base = f_select = -1;
   onlineSS = new FlexBuffer();
   printSS = new FlexBuffer();
   viewset = new hashTable<CC_String,int>(hash_func);
   f_buffer = NULL;
   f_pathbuf = NULL;
   f_locator = NULL;

   feature_depth = new Stack<int>;

}


StyleTask::~StyleTask()
{
   delete viewset;
   delete f_pathbuf;
   delete onlineSS;
   delete printSS;
   delete feature_depth;

   KILLSUBTASK(f_locator);

   ComplexTask::removeAllSubTasks();
}

void StyleTask::reset()
{
   f_base = f_select = -1;
   delete f_pathbuf;		f_pathbuf = NULL;
   delete onlineSS;		onlineSS = new FlexBuffer();
   delete printSS;		printSS = new FlexBuffer();
   delete feature_depth;	
   feature_depth = new Stack<int>;
   f_buffer = NULL;
   
   KILLSUBTASK( f_locator );
   
   ComplexTask::removeAllSubTasks();
     
}


static void
report_position(FlexBuffer *buffer, const char *file, int line)
{
  char info[200]; //MAGIC

  snprintf(info, sizeof(info), "#file: %.150s line: %d\n", file, line);
  buffer->writeStr(info);
}


static void
write_array(FlexBuffer *buffer, const char *tokens, int quotes)
{
  buffer->put('[');

  char *str = strdup ( tokens );

  if(str){
    char * token = strtok ( str, " \n\t");
    if ( token ) {
      if (quotes) buffer->put( '\"' );
      buffer->writeStr( token );
      if (quotes) buffer->put( '\"' );

      while ( (token = strtok ( NULL, " \n\t" )) ) {
	buffer->put(',');
        if (quotes) buffer->put( '\"' );
	buffer->writeStr( token );
        if (quotes) buffer->put( '\"' );
      }
    }
    delete str;
  } /*@# else out of memory... */
  
  buffer->put(']');
}

  
static void
autonumber(FlexBuffer *buffer, const Token& t)
{
  const AttributeRec *id = t.LookupAttr(SGMLName::intern("ID", 1));
  const AttributeRec *type = t.LookupAttr(SGMLName::intern("Type", 1));
  const AttributeRec *initial = t.LookupAttr(SGMLName::intern("Initial", 1));
  const AttributeRec *delta = t.LookupAttr(SGMLName::intern("Delta", 1));
  const AttributeRec *reset = t.LookupAttr(SGMLName::intern("Reset", 1));
  const AttributeRec *counter = t.LookupAttr(SGMLName::intern("Counter", 1));

  if(!id) throw(Unexpected("Autonumber: missing ID attribute"));
  if(!type) throw(Unexpected("Autonumber: missing Type attribute"));
  if(!initial) throw(Unexpected("Autonumber: missing Initial attribute"));
  if(!delta) throw(Unexpected("Autonumber: missing Delta attribute"));
  if(!reset) throw(Unexpected("Autonumber: missing Reset attribute"));
  if(!counter) throw(Unexpected("Autonumber: missing Counter attribute"));

  
  buffer->writeStr(id->getAttrValueString());
  buffer->writeStr(" = autonumber[\"");
  buffer->writeStr(type->getAttrValueString());
  buffer->writeStr("\", \"");
  buffer->writeStr(initial->getAttrValueString());
  buffer->writeStr("\", \"");
  buffer->writeStr(delta->getAttrValueString());
  buffer->writeStr("\", ");

  write_array(buffer,  counter->getAttrValueString(), 1);

  buffer->writeStr(", ");

  write_array(buffer, reset->getAttrValueString(), 1);

  buffer->writeStr("]\n\n");
}


static void
write_attr(FlexBuffer *f_buffer, const AttributeRec *arec)
{
  f_buffer->writeStr( SGMLName::lookup( arec->getAttrName() ) );
  f_buffer->writeStr( ":\t" );

  const char *val = arec->getAttrValueString();

  /* NAMES, NUMBERS convert to arrays for stylesheet lang. */
  if(arec->getAttrType() == SGMLName::TOKEN
     && strchr(val, ' ')){
    write_array(f_buffer, val, 0);
  }else{
    /* NASTY HACK!
     * The stylesheet internal language requires some feature
     * values to be quoted, and some not. It _seems_ that
     * quoting anything that doesn't start with a digit will
     * satisfy the constraints. This is highly artificial!
     *
     * Now for the exception that proves the hack!
     * TRUE and FALSE...
     *
     * One more to add to the heap, 
     * if an attribute value is being referenced
     */

    int quotes = !isdigit(val[0])
      && strcmp(val, "TRUE") != 0
      && strcmp(val, "FALSE") != 0
      && val[0] != '@';
  
    if(quotes) f_buffer->writeStr( "\"" );
    f_buffer->writeStr( val );
    if(quotes) f_buffer->writeStr( "\"" );
  }
}


void
StyleTask::markup( const Token &t )
{
  ComplexTask::markup(t);

   if (t.type() == START) {
      /*
       * Process Stylesheet start tags...
       */

      switch(t.olaf()){
       case OLAF::Stylesheet:

	 if (f_base >= 0) {
	    throw
	       (Unexpected
		("illegal nested STYLESHEET architectural form"));
	 }

	 f_base = t.level();
#ifdef FISH_DEBUG
	 DBUG_PRINT("Style", ("Style level=`%d'\n", f_base));
#endif
	 break;

       case OLAF::Path:

	 if ( f_pathbuf != NULL ) {
	    delete f_pathbuf;
	 }

	 f_pathbuf = new FlexBuffer();
	 f_buffer = f_pathbuf;
	 f_dataMode = inPath;
	 break;

       case OLAF::Select:
	 f_buffer->writeStr( "[" );
	 f_select = t.level();
	 break;
	
       case OLAF::Online:

	 f_buffer = onlineSS;
	 // feature_depth->clear();

	 if ( f_pathbuf == NULL ) {
	    throw(Unexpected("no path available for online feature."));
	 }

	 report_position(f_buffer, t.file(), t.line());
	 
	 f_buffer->writeStr( f_pathbuf->GetBuffer() );
	 f_buffer->writeStr( "\n" );
	 write_tabs( f_buffer, feature_depth, '\t');
	 f_buffer->writeStr( "{\n" );
	 feature_depth->push(t.level());

	 break;

       case OLAF::Print:

	 f_buffer = printSS;
	 // feature_depth->clear();

	 if ( f_pathbuf == NULL ) {
	    throw(Unexpected("no path available for print feature."));
	 }

	 report_position(f_buffer, t.file(), t.line());
	 f_buffer->writeStr( f_pathbuf->GetBuffer() );
	 f_buffer->writeStr( "\n" );
	 write_tabs( f_buffer, feature_depth, '\t');
	 f_buffer->writeStr( "{\n" );
	 feature_depth->push( t.level() );

	 break;

       case OLAF::AutoNumber:

	 report_position(onlineSS, t.file(), t.line());
	 autonumber(onlineSS, t);

	 report_position(printSS, t.file(), t.line());
	 autonumber(printSS, t);
	 break;
	
       case OLAF::FeatureText:

	 report_position(f_buffer, t.file(), t.line());
	 write_tabs( f_buffer, feature_depth, '\t');
	 
	 f_buffer->writeStr( t.giName() );
	 f_buffer->writeStr( ": " );
	 f_dataMode = startContent;
	 break;


      case OLAF::AutoRef:
	 {
	   const AttributeRec *arec;
	   int id = SGMLName::intern("ID");
	   if((arec = t.LookupAttr(id))){

	     if(f_dataMode == inContent){
	       f_buffer->writeStr(" + ");
	     }
	     
	     f_buffer->writeStr(arec->getAttrValueString());
	     f_dataMode = inContent;
	   } /* else document is not conforming... sgmls will report error */
	 }
	 break;
			       
       case OLAF::Feature:

	 report_position(f_buffer, t.file(), t.line());
	 write_tabs( f_buffer, feature_depth, '\t');
	 
	 f_buffer->writeStr( t.giName() );
	 f_buffer->writeStr( ": " );

	 const AttributeRec *arec;
   
	 /*
	  * Is it an enumeration feature?
	  */
	 if((arec = t.LookupAttr(OLAF::OL_Choice))){

           /* OL_Choice can only be applied to NAME attributes, hence
	      we don't neet to worryabout "'s in the attribute value.
	      */
	   /* except TRUE and FALSE....*/

	   const char *val = arec->getAttrValueString();

	   int quotes = !isdigit(val[0])
	     && strcmp(val, "TRUE") != 0
	     && strcmp(val, "FALSE") != 0
	     && val[0] != '@';

	   if (quotes) f_buffer->writeStr("\"");
	   f_buffer->writeStr(val);
	   if (quotes) f_buffer->writeStr("\" ");

	 }else{
	   f_buffer->writeStr( " {\n" );

	   for (arec = t.GetFirstAttr();
		arec != NULL;
		arec = t.GetNextAttr( arec )) {
	     
	     if (( arec->getAttrValueString() != NULL ) &&
		 ( arec->getAttrName() != OLAF::OLIAS )) {
	       
	       write_tabs( f_buffer, feature_depth, '\t');
	       f_buffer->put('\t');

	       write_attr(f_buffer, arec);

	       f_buffer->writeStr( ",\n" );
	     }
	   }
	   feature_depth->push(t.level());
	 }
	
	 break;
      }
      
      /*
       * first time we see OL-ID="...", spawn an OL_Data to collect the id
       */
      if ((f_base >= 0) && (f_locator == NULL) &&
          (t.LookupAttr(OLAF::OL_id))) {

#ifdef FISH_DEBUG
         DBUG_PRINT("Style", ("spawning locator collection subtask\n"));
#endif
         f_locator = new OL_Data(t, OLAF::OL_id, REMOVE_SPACES);
         addSubTask( f_locator );

      }
   }

   else if (t.type() == END) {

      if (f_base > 0) {

	int topelement;
	if ( !feature_depth->empty() ) {
	  if ( (topelement = feature_depth->top()) ) {
	    if ( topelement == t.level() ) {
	      topelement = feature_depth->pop();
	      write_tabs( f_buffer, feature_depth, '\t');
	      
	      if(feature_depth->empty()){
		f_buffer->writeStr("}\n");
	      }
	      else {
		f_buffer->writeStr("},\n");
	      }

	    }else if ( topelement + 1 == t.level() ) {
	      /* need a new-line at end of FeatureText data */
	      f_buffer->writeStr(",\n");
	    }
	  }
	}
	  
	if (t.level() == f_base) {
	  /* found end of stylesheet... write out StyleSheet data */
#ifdef FISH_DEBUG
	  DBUG_PRINT("Style", ("found end of stylesheet write out StyleSheet data\n"));
#endif
	  write_record();
	  
	}
	else if(t.level() == f_select){
	  f_buffer->writeStr( "]" );
	  f_select = -1;
	}
      }
    }
}


void StyleTask::data ( const char *str, size_t t )
{

  if ( f_base > 0 ) {
    
    ComplexTask::data( str, t );
    if (f_buffer != NULL) {

      switch(f_dataMode){
      case inPath:
	f_buffer->write ( str, t );
	break;

      case inContent:
	f_buffer->write ( " + ", 3 );
	/* fall through */

      case startContent:
	f_buffer->write ( "\"", 1 );
	f_dataMode = inContent;

	const char *p;
	for(p = str; *p; p++){
	  if(*p == '"' || *p == '\\')
	    f_buffer->put('\\');
	  f_buffer->put(*p);
	}
	
	f_buffer->write ( "\"", 1 );
	break;
      }
      

    }
  }
  
}


const char *
StyleTask::locator()
{
   const char *ret = 0;
   
   if (f_locator){
      ret = f_locator->content();
   }
   
   if (!(ret && *ret)){
      throw(Unexpected ("No name given to stylesheet."));
   }
   return ret;
}


void StyleTask::write_record( void )
{

   const char
      *localstr = locator();

#ifdef FISH_DEBUG
   DBUG_PRINT("Style",
	      ("Style Sheet: \nview=`%s'\nonlineSS=`%s'\n\nprintSS=`%s'\n",
	       localstr,
	       onlineSS->GetBuffer(),
	       printSS->GetBuffer() ));

   DBUG_PRINT("Style", ("View id...\n"));
   DBUG_PRINT("Style", ("\tis: `%s'\n", localstr ));
#endif

   CC_String *str = new CC_String( localstr );
   int *bogus = new int(0);
   if ( !viewset->contains( str ) ) {
      viewset->insertKeyAndValue( str,bogus );
   }
   else {
     Token::signalError(Token::User, Token::Continuable, 0, 0,
			"Duplicate stylesheet id `%s'", localstr);
     return;
   }
   
   int online_bufsize=onlineSS->GetSize();
   const char *online_buf = onlineSS->GetBuffer();

   int print_bufsize= printSS->GetSize();
   const char *print_buf = printSS->GetBuffer();

   done(localstr,
	online_buf, online_bufsize,
	print_buf, print_bufsize);
}


int StyleTask::exist( const char * str )
{
  CC_String tmp(str);
  return( viewset->contains( &tmp ) );
}

const char *
StyleTask::print()
{
  const char *ret = 0;
  
  if(printSS) ret = printSS->GetBuffer();

  return ret;
}

int
StyleTask::print_data_size()
{
  if (printSS) return ( printSS->GetSize() );
  else return 0;
}

const char *
StyleTask::online()
{
  return onlineSS->GetBuffer();
}

int 
StyleTask::online_data_size()
{
  if ( onlineSS ) return ( onlineSS->GetSize() );
  else return 0;
}

