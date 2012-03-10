 /*
  * $XConsortium: sgml.l /main/2 1996/07/18 14:28:02 drk $
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

#include "compression/trie.h"
#include "compression/abs_agent.h"


static unsigned char yybuf[LBUFSIZ];
static int yybuf_sz = LBUFSIZ;
static int yybuffed = 0;

              
#undef yywrap
int yywrap();

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
   { \
      result = (*fill_buf_func)((unsigned char*)buf,max_size); \
   }


%}

%%

"<"[0-9a-zA-Z_.]+">"|"</"[0-9a-zA-Z_.]+">" {
               (*lex_action_func)((unsigned char*)(yytext), yyleng, 1);
	}

.|\n	{
	   if ( yybuffed >= yybuf_sz ) {
              (*lex_action_func)(yybuf, yybuf_sz, 2);
              yybuffed = 0;
           }

           yybuf[yybuffed++] = yytext[0];
	}


%%

int yywrap()
{
   (*lex_action_func)(yybuf, yybuffed, 2);
   return 1;
}

