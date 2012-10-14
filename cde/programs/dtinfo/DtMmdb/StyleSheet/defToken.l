
%a 30000
%e 10000
%k 10000
%n 10000
%o 40000
%p 20000

%{
/* $XConsortium: defToken.l /main/5 1996/11/19 16:54:10 drk $ */

#include <string.h>
#include "FeatureDefDictionary.h"
#include "defParser.tab.h"
#include "Debug.h"
#include <iostream>
using namespace std;

extern istream *g_yyin;

#define YY_INPUT(buf,result,max_size)\
  {\
     if (g_yyin -> eof()) {\
        result=0;\
     } else {\
        g_yyin -> read((char *)buf, max_size-1); \
        result = g_yyin -> gcount(); \
        buf[result] = 0; \
     }\
  }

int yylineno=1;

unsigned char* defToken_string_buf = new unsigned char[1024];
int defToken_string_buf_size = 1024;
int defToken_string_buf_content_size = 0;

unsigned char* new_copy(const unsigned char* str, int size)
{
   unsigned char* x = new unsigned char[ ( size <= 0 ) ? 1 : size + 1];
   memcpy(x, str, size);
   x[size] = 0;
   return x;
}

void addToDefTokenStringBuf(const unsigned char* str, int size)
{
   if ( size <= 0 ) return;

   if ( defToken_string_buf_size - defToken_string_buf_content_size < size ) {
      defToken_string_buf_size = 2*(size+defToken_string_buf_content_size);
      unsigned char* x = new unsigned char[defToken_string_buf_size];
      memcpy(x, defToken_string_buf, defToken_string_buf_content_size);
      delete defToken_string_buf;
      defToken_string_buf = x;
   }

   memcpy(defToken_string_buf + defToken_string_buf_content_size, str, size);
   defToken_string_buf_content_size += size;
   defToken_string_buf[defToken_string_buf_content_size] = 0;
}



%}

stringprefix ([Ss][Tt][Rr][Ii][Nn][Gg][_][Pp][Rr][Ee][Ff][Ii][Xx])
string ([Ss][Tt][Rr][Ii][Nn][Gg])
integer ([Ii][Nn][Tt][Ee][Gg][Ee][Rr])
array([Aa][Rr][Rr][Aa][Yy])
real ([Rr][Ee][Aa][Ll])
unit ([Dd][Ii][Mm][Ee][Nn][Ss][Ii][Oo][Nn])
unitpixel ([Dd][Ii][Mm][Ee][Nn][Ss][Ii][Oo][Nn][_][Pp][Ii][Xx][Ee][Ll])
boolean ([Bb][Oo][Oo][Ll][Ee][Aa][Nn])
%x quoted_string

%%

^"#".*	{
	}

","     {
           return(COMMA);
        }

"*"     {
           return(STAR);
        }

";"     {
           return(SEMI_COLON);
        }

":"     {
           return(COLON);
        }

"{"     {
           return(FSOPEN);
        }

"}"     {
           return(FSCLOSE);
        }

"("     {
           return(OPER_parenopen);
        }

")"     {
           return(OPER_parenclose);
        }


{stringprefix} {
		yylval.charPtrData = new_copy((unsigned char*)yytext, yyleng);
		return(TYPE);
	}

{string} {
		yylval.charPtrData = new_copy((unsigned char*)yytext, yyleng);
		return(TYPE);
	}

{integer} {
		yylval.charPtrData = new_copy((unsigned char*)yytext, yyleng);
		return(TYPE);
	}

{real} {
		yylval.charPtrData = new_copy((unsigned char*)yytext, yyleng);
		return(TYPE);
	}

{unit} {
		yylval.charPtrData = new_copy((unsigned char*)yytext, yyleng);
		return(TYPE);
	}

{unitpixel} {
		yylval.charPtrData = new_copy((unsigned char*)yytext, yyleng);
		return(TYPE);
	}

{array} {
		yylval.charPtrData = new_copy((unsigned char*)yytext, yyleng);
		return(TYPE);
	}

{boolean} {
		yylval.charPtrData = 
		   new_copy((unsigned char*)"INTEGER", strlen("INTEGER"));
		return(TYPE);
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
			new unsigned char[defToken_string_buf_content_size+1];
		memcpy( yylval.charPtrData, 
			defToken_string_buf, 
			defToken_string_buf_content_size+1
		      );

   		defToken_string_buf_content_size = 0;
		BEGIN 0;

		return(QUOTED_STRING);
		}

<quoted_string>.	{
		addToDefTokenStringBuf(yytext, yyleng);
		}

"&"[^ \t\n\";.=@+*\/\.\*:?\^,{}\[\]()]+	{
		yylval.charPtrData = 
                  (unsigned char*)strdup((const char*)(yytext+1));
		return(REF_NAME);
		}

[^ \t\n\";.=@+*\/\.\*:?\^,{}\[\]()]+	{
		yylval.charPtrData = 
                  (unsigned char*)strdup((const char*)yytext);
		return(NORMAL_STRING);
		}

[\t]	{
	}

[\n]	{
	   yylineno++;
	}

.	{
	}



%%

void yyerror(char* msg)
{
   cerr << "line " << yylineno << ": " << msg;
}

