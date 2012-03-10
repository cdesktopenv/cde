%{ /* -*- c++ -*- */
/* $XConsortium: ReplaceIdIdref.l /main/3 1996/11/19 16:54:45 drk $ */
  
/* exported interfaces... */
#include "NodeData.h"
  
/* imported interfaces... */
#include <iostream.h>
#include <stream.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "misc/unique_id.h"
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

// Debugging macro
#ifdef DEBUG
#define DBG(level) if ( dbgLevel >= level)
#else
#define DBG(level) if (0)
#endif

static int dbgLevel=-1;

/*
 * Forward declaration for my_input
 */
static int my_input ( char *, int );

#undef YY_INPUT
#define YY_INPUT(b, r, ms ) ( r=my_input( ( char *)b,ms) )
  
static char *myinput;
static char *myinputptr;
static char *myinputlim;

NodeData *CurrentNodeData;

extern void replace_entity ( FlexBuffer *, const char *);

%}

%x OLID OLIDREF GRAPHIC XREF

%%

"<#OL-XREF>"		{
			   BEGIN ( XREF );
			}

<XREF>[0-9]+		{
                          const char *str = ( const char *)yytext;
                          OL_Data *idref =
			    (OL_Data *)CurrentNodeData->subtask( atoi(str) );
			  if ( !idref->ContentIsEmpty() ) {
			   const char *idrefval = idref->content();
			   
			   FlexBuffer *db_buf =  CurrentNodeData->DbBuffer; 

			   db_buf->writeStr("<#OL-XREF>");
			   replace_entity ( db_buf, idrefval );
			   db_buf->writeStr("</#OL-XREF>");
			  }
			}

<XREF>"</#OL-XREF>"	{
                           BEGIN ( 0 );
			}

"<#OL-ID>"              {
                           BEGIN ( OLID );
			}

<OLID>[0-9]+            {
                           /*
			    * query the array for the real no.
			    */

                          const char *str = ( const char *)yytext;
                          OL_Data *id =
			    (OL_Data *)CurrentNodeData->subtask( atoi(str) );
			  if ( !id->ContentIsEmpty() ) {
			    const char *idval = id->content();
			    FlexBuffer *db_buf = CurrentNodeData->DbBuffer;

			    db_buf->writeStr("<#OL-ID>");
			    replace_entity( db_buf, idval );
			    db_buf->writeStr("</#OL-ID>");

			  }
			}
<OLID>"</#OL-ID>"       {
                           BEGIN ( 0 );
			}

"<#OL-IDREF>"           {
                           BEGIN( OLIDREF );
			}
<OLIDREF>[0-9]+         {
                          const char *str = (const char *)yytext;
                          OL_Data *idref =
			    (OL_Data *)CurrentNodeData->subtask(atoi(str));
			  if ( !idref->ContentIsEmpty() ) {

			   const char *idrefval = idref->content();
			   int line_num = idref->line_no();
			   const char *filename = idref->filename();
			   
			   FlexBuffer *db_buf =  CurrentNodeData->DbBuffer; 

			   db_buf->writeStr("<#OL-IDREF>");
			   db_buf->writeStr(form("<#L>%d</#L>", line_num));
			   db_buf->writeStr(form("<#F>%s</#F>", filename));
			   replace_entity ( db_buf, idrefval );
			   db_buf->writeStr("</#OL-IDREF>");
			  }
			 }
<OLIDREF>"</#OL-IDREF>"  {
                           BEGIN( 0 );
			 }
"<#GRAPHIC>"             {
                           BEGIN( GRAPHIC );
                         }
<GRAPHIC>[0-9]+          {
                           const char *str = (const char *)yytext;
                           OL_Data *graphic_id =
			     (OL_Data *)CurrentNodeData->subtask(atoi(str));
			   if ( !graphic_id->ContentIsEmpty() ) {

			     const char *graphic_id_val =
			       graphic_id->content();

			     CurrentNodeData->DbBuffer->writeStr("<#GRAPHIC>");
			     replace_entity (CurrentNodeData->DbBuffer, 
					     graphic_id_val);
			     CurrentNodeData->DbBuffer->writeStr("</#GRAPHIC>");
			   }
			 }
<GRAPHIC>"</#GRAPHIC>"   {
                           BEGIN(0);
			 }
.                        |
\n                       {
                           CurrentNodeData->DbBuffer->put( yytext[0] );
			 }


%%

//-----------------------------------------------------------------------
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

//----------------------------------------------------------------------
void ReplaceIdIdRef( NodeData *nd , char *buffer, int sz )
{
  CurrentNodeData = nd;

  myinput = buffer;
  myinputptr = buffer;
  myinputlim = buffer + sz;

  yylex();

  BEGIN INITIAL;
  yyrestart(NULL);
}
