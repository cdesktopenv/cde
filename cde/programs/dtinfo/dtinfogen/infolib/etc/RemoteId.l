%{
/* $XConsortium: RemoteId.l /main/3 1996/11/19 16:54:33 drk $ */

/* imported interfaces */
#include "FlexBuffer.h"

#include "BTCollectable.h"
#include "dti_cc/CC_String.h"
#include "dti_cc/cc_hdict.h"


static int my_input ( char *, int );

#undef YY_INPUT
#define YY_INPUT(b, r, ms ) ( r=my_input( ( char *)b,ms) )

static char *myinput;
static char *myinputptr;
static char *myinputlim;
static FlexBuffer *NodeBuffer;
static FlexBuffer *idref_buffer;
static hashTable<CC_String,BTCollectable> *hd;
static int NeedRemote = 0;
static int current_line_num = 0;
static char *current_file_name;

%}

%x OLIDREF OLIDREF_LINE OLIDREF_FILE

%%

"<#OL-IDREF>"  {
                  BEGIN( OLIDREF );
		  NodeBuffer->writeStr("<#OL-IDREF>");
               }

<OLIDREF>"<#L>" {
                  BEGIN(OLIDREF_LINE);
                } 

<OLIDREF_LINE>[^<]+ {
                      const char *line = (const char *)yytext;
                      current_line_num = atoi( line );
                    }

<OLIDREF_LINE>"</#L>" {
                         BEGIN( OLIDREF );
                      }

<OLIDREF>"<#F>"  {
                    BEGIN ( OLIDREF_FILE );
                 }

<OLIDREF_FILE>[^<]+ {
                      current_file_name = strdup( (const char *)yytext);
                    }

<OLIDREF_FILE>"</#F>"  {
                          BEGIN ( OLIDREF );
                       }

<OLIDREF>"&lt;" {
                  // Perform the entity resolution
                  idref_buffer->put('<');
                  NodeBuffer->writeStr( "&lt;" );
                }
<OLIDREF>"&amp;"  {
                    idref_buffer->put('&');
                    NodeBuffer->writeStr( "&amp;" );
                  }



<OLIDREF>"</#OL-IDREF>" {
		  /*
		   * test if the link value found in 
		   * #OL-IDREF is resolved in the bookcase
		   */

		  CC_String key( (const char *)idref_buffer->GetBuffer() );

		  CC_String *val = (CC_String *)hd->findValue( &key );
		  if ( !val ) {
		    NeedRemote = 1;
		    cerr << "(WARNING) Unresolved link  = " << (const char *)key << endl
			 << "                file       = " << current_file_name << endl
			 << "                line no.   = " << current_line_num << "\n\n";
		  }

		  // cleanup and reset
		  delete current_file_name; current_file_name = 0;
		  idref_buffer->reset();

		  NodeBuffer->writeStr("</#OL-IDREF>");
		  if ( NeedRemote ) {
		    NodeBuffer->writeStr("<#REMOTE></#REMOTE>");
		    NeedRemote = 0;
		  }
		  
		  BEGIN ( 0 );
		}

<OLIDREF>[^&<]+ {
                  const char *str = (const char *)yytext;
		  idref_buffer->writeStr( str );
		  NodeBuffer->writeStr( str );
                }

.               |
\n              {
                  NodeBuffer->put( yytext[0] );
		}


%%
static int
my_input ( char *buf, int max_size )
{

  int remain = myinputlim - myinputptr;
  int n = ( max_size > remain ? remain : max_size );

  if ( n > 0 ) {
    memcpy ( buf, myinputptr, n );
    myinputptr += n;
  }
  return n;
}

//------------------------------------------------------------
void insert_remotelink( hashTable<CC_String,BTCollectable> *dictionary, 
                        char *data,
			size_t datalen,
			FlexBuffer *result_buf)
{
  myinput = data;
  myinputptr = data;
  myinputlim = data + datalen;

  NodeBuffer = result_buf;
  hd         = dictionary;

  idref_buffer = new FlexBuffer();
  yylex();
  delete idref_buffer;

  BEGIN INITIAL;
  yyrestart(NULL);
}

void bogus(int)
{
  cout << "bogus\n";
}
