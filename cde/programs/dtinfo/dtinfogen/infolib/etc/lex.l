%{ /* -*- c++ -*- */
/* $XConsortium: lex.l /main/5 1996/11/19 16:55:12 drk $ */

#if !defined(__osf__) && !defined(USL) && !defined(linux) && !defined(CSRG_BASED)
#include <osfcn.h>
#else
#include <unistd.h>
#endif

#include <iostream.h>
#include <stdio.h>
#include <stream.h>  
#include <string.h>

#include "Exceptions.hh"

#include "SGMLDefn.h"  
#include "Dispatch.h"
#include "AttributeRec.h"
#include "AttributeList.h"  
#include "FlexBuffer.h"
#include "Token.h"
#include "api/utility.h"

static SGMLDefn *defn = new SGMLDefn();
static FlexBuffer *DataBuffer = new FlexBuffer();

static
unsigned char oct2dec( const char *str )
{
  unsigned char  value = 0;
  const char *ptr = str;
  const char *endptr = str + strlen(str) -1 ;
  
  for ( ; *ptr != '\0' ; ptr++ ) {
    
    int power = endptr - ptr;
    int result     = 1;
    
    for ( int i = 1; i <= power; i++ ) {
      result = result * 8;
    }

    value = value + ( *ptr - '0' ) * result ;
  }

  
  return ( value );
}



%}
%x ProcessData
%%

^A[^\n]+                   {
                             char *name;
			     char *tstr;
			     char *value;
                             int type;
			     // Parse the attStr

			     name = strtok ( (char*)yytext+1, "\n\t ");
                             tstr = strtok ( NULL, "\n\t ");
                             value = strtok ( NULL, "\n\t");
                             type = SGMLName::intern(tstr);

                             if ( type != SGMLName::IMPLIED ){
			       if(!value) value = "";
			       Dispatch::tok->StoreAttribute( name, value, type);
			     }
			   }

^"("[^\n\t ]+              {
                             // An generic identifier is found
                             // Need to determine how to display/return the
                             // information associated with it

                             Dispatch::token(START, (unsigned char *)yytext+1 );
                            
                           }

^"L"[0-9]+" "[^ \n]+       {	/* file & line num info. */
				char *p = (char*)yytext+1;
				Dispatch::line(atoi(p));
				while(*p != ' ') p++;
				p++;
				Dispatch::file(p);
			   }
				
^"L"[0-9]+		   {	/* line num info. */
				char *p = (char*)yytext+1;
				Dispatch::line(atoi(p));
			   }
				
^"s"[^\n]+                 {
                              /* system id found */
                              defn->store_sys_id( (char *)yytext + 1 );
			    }
^"f"[^\n]+                  {
                              /* file name found */
                              defn->store_file_name( (char *)yytext + 1 );
			    }
^"E"[^\n]+                  {
                              /*
			       * For now, only entity definition is recorded
			       * Eventually Notation, Subdoc....
			       */
                              defn->store_defn( ENTITY_TYPE,
						(char *)yytext + 1 );
			      
                              Dispatch::entity_decl( defn );
			      
			    }
^"&"[^\n]+                  {
                              /*
			       * external entity reference
			       */
                              Dispatch::token(EXTERNAL_ENTITY, 
					      (unsigned char *)yytext+1 );
			    }

^"{"                        {
                              /*
			       * subdoc start event
			       */
                             Dispatch::subdoc_start();

			   }

^"}"                       {
                             /*
			      * subdoc end event
			      */
                             Dispatch::subdoc_end();
			   }

^-                         {
                             BEGIN ( ProcessData );
                           }

<ProcessData>"\\""\\"      {
                              // A slash
                              DataBuffer->put ('\\');
                           }
<ProcessData>"\\n"         {
                             // Replace new line with space
                             DataBuffer->put('\015');
			     Dispatch::newline();
                             
                           }
<ProcessData>"\\|"         {
                             // Bypass this entity
                           }

<ProcessData>"\\|lnfeed\\|"  {
                               DataBuffer->put('\n');
                             }

<ProcessData>"\\"[0-7][0-7][0-7] {
  
                                   unsigned char ch = oct2dec(
				      (const char *)yytext + 1 );
                                   DataBuffer->put ( ch );

                                 }
<ProcessData>\n            {
                             Dispatch::data( DataBuffer );
                             delete DataBuffer;
                             DataBuffer = new FlexBuffer;

			     BEGIN(0);
			   }

<ProcessData>[^\n\\]+      {
                             DataBuffer->write( (char*)yytext,
						strlen((char*)yytext) );
                           }

<ProcessData>.             {
                             throw(Unexpected(form("Bad character '%s' found",
						   (char *)yytext)));
			   }

^")"[^\n\t ]+              {
                             Dispatch::token (END, (unsigned char *)yytext+1);
                           }

.                          |
\n                         ;
			     
%%

#ifdef DEBUG
//---------------------------------------------------------------------
#include "OLAF.h"  
#include "SGMLName.h"

static void TestToken( Token *tok )
{
  cout << "(" << tok->giName() << endl;
  for ( const AttributeRec *a = tok->GetFirstAttr(); a; a=tok->GetNextAttr( a ) ) {
    cout << "AttributeName = " << SGMLName::lookup(a->getAttrName()) << endl;
    cout << "AttributeValueStr = " << a->getAttrValueString() << endl;
    cout << "AttributeValue = " << a->getAttValue() << endl;
    cout << "AttributeType = " << a->getAttrType() << endl;

    if ( !(strcmp( a->getAttrValueString(), "NODE" ) ) ){
      if ( tok->AttributeMatch ( OLAF::OLIAS, OLAF::OL_Section ) ) {
	cout << "AttrMatch Test 1 passed\n";
      }
      else {
	cout << "AttrMatch Test 1 failed\n";
      }

      if ( !tok->AttributeMatch ( OLAF::OLIAS, OLAF::OL_Graphic ) ) {
	cout << "AttrMatch Test 2 passed\n";
      }
      else {
	cout << "AttrMatch Test 2 failed\n";
      }
      
    }
  }
}
#endif /* DBUG_OFF */
