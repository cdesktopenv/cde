 /*
  * $XConsortium: token.l /main/2 1996/07/18 14:51:52 drk $
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
#include "store_desc.h"
#include "sheet.tab.h"
extern char replace_string[PATHSIZ];
extern int replace_string_len;

int linecount = 1;
%}

%%

"container"	{
         return(CONTAINER);
        }

"set"	{
         return(SET);
        }

"list"	{
         return(LIST);
        }

"index_nm"	{
         return(INDEX_NAME);
        }

"inv"	{
         return(INV);
        }

"compress"	{
         return(COMPRESS);
        }

"huffman"	{
         return(HUFFMAN);
        }

"dict"	{
         return(DICT);
        }

"index_agent"	{
         return(INDEX_AGENT);
        }

"mphf"	{
         return(MPHF);
        }

"btree"	{
         return(BTREE);
        }

"smphf"	{
         return(SMPHF);
        }

"index"	{
         return(INDEX);
        }

"mphf_index"	{
         return(MPHF_INDEX);
        }

"smphf_index"	{
         return(SMPHF_INDEX);
        }

"btree_index"	{
         return(BTREE_INDEX);
        }

"inv_nm"	{
         return(INV_NAME);
        }

"agent_nm"	{
         return(AGENT_NAME);
        }

"store_nm"	{
         return(STORE_NAME);
        }

"position"	{
         return(POSITION);
        }

"store"	{
         return(STORE);
        }

"page_store"	{
         return(PAGE_STORE);
        }

"nm"	{
         return(NM);
        }

"v_oid"	{
         return(V_OID);
        }

"mode"	{
         return(MODE);
        }

"page_sz"	{
         return(PAGE_SZ);
        }

"byte_order"	{
         return(BYTE_ORDER);
        }

"cached_pages"	{
         return(CACHED_PAGES);
        }

"="    {
         return(EQUAL);
	}

":"[\\]?[\n]?    {
         return(SEPARATOR);
	}

[\n]+    { 
	   linecount++; 
	 }

[\t ]+    {
	}

^"#".*    {
	}

[0-9]+ 	{
    	 yylval.integer = atoi((char*)yytext);
         return (NUMBER);
	}

[a-zA-Z0-9_\-.$]+ 	{
         if ( replace_string[0] != 0 && yytext[0] == '$' ) {
            strcpy(replace_string + replace_string_len, (char*)yytext+1);
    	    yylval.string = replace_string;
         } else
    	    yylval.string = (char*)yytext;
         return (TOKEN);
	}

%%
