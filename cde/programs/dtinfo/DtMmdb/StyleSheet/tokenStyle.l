 /*
  * $TOG: tokenStyle.l /main/6 1998/04/17 11:50:07 mgreess $
  *
  * Copyright (c) 1993 HAL Computer Systems International, Ltd.
  * All rights reserved.  Unpublished -- rights reserved under
  * the Copyright Laws of the United States.  USE OF A COPYRIGHT
  * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
  * OR DISCLOSURE.
  * 
  * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
  * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
  * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
  * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
  * INTERNATIONAL, LTD.
  * 
  *                         RESTRICTED RIGHTS LEGEND
  * Use, duplication, or disclosure by the Government is subject
  * to the restrictions as set forth in subparagraph (c)(l)(ii)
  * of the Rights in Technical Data and Computer Software clause
  * at DFARS 252.227-7013.
  *
  *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
  *                  1315 Dell Avenue
  *                  Campbell, CA  95008
  * 
  */


%a 30000
%e 10000
%k 10000
%n 10000
%o 40000
%p 20000

%{
#include <string.h>
#include "ParserConst.h"
#include "Expression.h"
#include "FeatureValue.h"
#include "PathTable.h"
#include "SSPath.h"
#include "PathQualifier.h"
#include "StyleSheetExceptions.h"
#include "style.tab.h"
#include "Debug.h"
#include <iostream.h>

extern istream *g_stylein ;

#define YY_INPUT(buf,result,max_size)\
  {\
     if (g_stylein -> eof()) {\
        result=0;\
     } else {\
        g_stylein -> read((char*)buf, max_size-1); \
        result = g_stylein -> gcount(); \
        buf[result] = 0; \
     }\
  }

unsigned char* qstring_buf = new unsigned char[1024];
int qstring_buf_size = 1024;
int qstring_buf_content_size = 0;

char* commentBuffer = new char [1024];
int commentBufferSize = 1024;
int commentBufferContentSize = 0;

int yylineno=1;
		
void addToQstringBuf(const unsigned char* str, int size)
{
   if ( size <= 0 ) return;

   if ( qstring_buf_size - qstring_buf_content_size < size ) {
      qstring_buf_size = 2*(size+qstring_buf_content_size);
      unsigned char* x = new unsigned char[qstring_buf_size];
      memcpy(x, qstring_buf, qstring_buf_content_size);
      delete [] qstring_buf;
      qstring_buf = x;
   }

   memcpy(qstring_buf + qstring_buf_content_size, str, size);
   qstring_buf_content_size += size;
   qstring_buf[qstring_buf_content_size] = 0;
}


%}
unit ([Ii][Nn]|[Ii][Nn][Cc][Hh]|[Pp][Cc]|[Pp][Ii][Cc][Aa]|[Pp][Tt]|[Pp][Oo][Ii][Nn][Tt]|[Pp][Ii][Xx][Ee][Ll]|[Cc][Mm])

%x block sgmlgimode quoted_string

%%

"#|"	BEGIN(block);

^"#".*	{
	   if ( commentBufferSize < yyleng ) {
              delete [] commentBuffer;
              commentBufferSize = 2 * yyleng ;
              commentBuffer = new char [commentBufferSize];
           } 

	   commentBufferContentSize = yyleng-1;
           memcpy(commentBuffer, yytext+1, commentBufferContentSize); // copy everything except the #
           commentBuffer[commentBufferContentSize] = 0;
	}

"="	{
	   return(OPER_assign);
	}

"@"	{
	   return(OPER_attr);
	}

[+]	{
           yylval.charData = yytext[0];
	   return(OPER_plus);
	}

[-]	{
           yylval.charData = yytext[0];
	   return(OPER_minus);
	}

"/"	{
           yylval.charData = yytext[0];
	   return(OPER_div);
	}

"."	{
	   return(OPER_period);
	}

"*"	{
           yylval.charData = yytext[0];
	   return(OPER_star);
	}

":"	{
	   return(OPER_modify);
	}

"?"	{
	   return(OPER_oneof);
	}

"^"	{
	   return(OPER_parent);
	}

","	{
	   return(SEPARATOR);
	}

"{"	{
	   return(FSOPEN);
	}

"}"	{
	   return(FSCLOSE);
	}

"["	{
	   return(ARRAYOPEN);
	}

"]"	{
	   return(ARRAYCLOSE);
	}

"("	{
	   return(OPER_parenopen);
	}

")"	{
	   return(OPER_parenclose);
	}

"||"	{
	   return(OPER_or);
	}

"&&"	{
	   return(OPER_and);
	}

"=="|"!="	{
           if ( strcmp((const char*)yytext, "==") == 0 )
              yylval.intData = EQUAL;
 	   else
              yylval.intData = NOT_EQUAL;

	   return(OPER_equality);
	}

"!"	{
	   return(OPER_logicalnegate);
	}

"<="|"<"|">="|">"	{
           if ( strcmp((const char*)yytext, "<=") == 0 )
              yylval.intData = LESS_OR_EQUAL;
 	   else
           if ( strcmp((const char*)yytext, "<") == 0 )
              yylval.intData = LESS;
 	   else
           if ( strcmp((const char*)yytext, ">=") == 0 )
              yylval.intData = GREATER_OR_EQUAL;
 	   else
              yylval.intData = GREATER;

	   return(OPER_relational);
	}

"GICaseSensitive"	{
		return(GI_CASE_SENSITIVE);
			}

[Tt][Rr][Uu][Ee]	{
                yylval.boolData = true;
		return(BOOLVAL);
			}

[Ff][Aa][Ll][Ss][Ee]	{
                yylval.boolData = false;
		return(BOOLVAL);
			}

[On][Nn]	{
                yylval.boolData = true;
		return(BOOLVAL);
		}

[Oo][Ff][Ff]	{
                yylval.boolData = false;
		return(BOOLVAL);
		}

[0-9]+("."[0-9]+)?{unit} 	{
		yylval.charPtrData = 
                  (unsigned char*)strdup((const char*)yytext);
		return(DIMENSION);
		}

[0-9]+		{
		yylval.intData = atoi((char*)yytext);
		return(INTEGER);
		}

[0-9]+"."[0-9]+	{
		yylval.realData = atof((char*)yytext);
		return(REAL);
		}

\"		{
		BEGIN quoted_string;
		}

<quoted_string>\"	{

		yylval.charPtrData = 
			new unsigned char[qstring_buf_content_size+1];
		memcpy( yylval.charPtrData, 
			qstring_buf, 
			qstring_buf_content_size+1
		      );

   		qstring_buf_content_size = 0;
		BEGIN 0;

		return(QUOTED_STRING);
		}

<quoted_string>\\	{
		int c = styleinput();
		switch (c) {
		   case '"':
		     addToQstringBuf((unsigned char*)"\"", 1);
		     break;
		   case '\\':
		     addToQstringBuf((unsigned char*)"\\", 1);
		     break;
		   default:
                     throw(CASTSSEXCEPT StyleSheetException());
		}
		}

<quoted_string>[^\\\"]*	{
		addToQstringBuf((unsigned char*)yytext, yyleng);
		}

<quoted_string>.	{
		addToQstringBuf((unsigned char*)yytext, yyleng);
		}

{unit}		{
		yylval.charPtrData = 
                  (unsigned char*)strdup((const char*)yytext);
		  return(UNIT_STRING);
		}

[^ \t\n\".=@+*\/\.\*:?\^,{}\[\]()!]+	{
		yylval.charPtrData = 
                  (unsigned char*)strdup((const char*)yytext);
		return(NORMAL_STRING);
		}

<sgmlgimode>[0-9a-zA-Z\.\-]+ {
		yylval.charPtrData = 
                  (unsigned char*)strdup((const char*)yytext);
		BEGIN 0;
		return(SGMLGI_STRING);
	}

[\t]	{
	}

[\n]	{
	   yylineno++;
	}

.	{
	}



<block>"|#"	{ BEGIN(0); }
<block>.	;
<block>\n	;
%%

void enter_sgmlgi_context()
{
   BEGIN sgmlgimode;
}


void report_error_location()
{
   if ( commentBufferContentSize > 0 ) {
      cerr << commentBuffer << "\n";
   }
}

void yyerror(char* msg)
{
#ifdef DEBUG
   cerr << "line " << yylineno << ": " << msg << "\n";
#endif
   throw(CASTSSSEEXCEPT StyleSheetSyntaxError());
}

