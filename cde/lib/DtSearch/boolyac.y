%{
/* $XConsortium: boolyac.y /main/1 1996/03/29 17:04:13 cde-ibm $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: expr
 *		yyparse
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1996,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/********************* BOOLYAC.Y ********************
 * $Id: boolyac.y /main/1 1996/03/29 17:04:13 cde-ibm $
 * February 1996.
 * AusText/DtSearch yacc-based boolean query parser.
 * Converts boolean query into stems array and truth table
 * for subsequent search.  This is the yacc source.
 * After processing by yacc, it becomes boolyac.c and boolyac.h.
 * Boolpars.c contains all the related C source code: yylex,
 * action functions, and the main driver function, boolean_parse.
 *
 * $Log$
 * Revision 1.1  1996/03/05  15:52:05  miker
 * Initial revision
 *
 */
#include <stddef.h>
#include "boolpars.h"
%}

%union {
    TRUTHTAB	*truthtab;
    int		int_val;
}

%token	<truthtab> WORD_TOKEN
%left	'|'		/* left associative, lowest precedence */
%left	'&' 		/* left associative, higher precedence */
%right	'~' 		/* right associative */
%token	<int_val>  COLLOC_TOKEN
%token	ERROR_TOKEN	/* highest precedence */

%type	<truthtab> expr query
%%

/*********** Top Level Start Rule ****************/
query:  /* nothing */
		{
		    if (parser_invalid_wordcount > 0)
			add_syntax_errmsg(6);
		    else
			add_syntax_errmsg(1);
		    YYABORT;
		}

  |	expr	
		{ $$ = copy_final_truthtab ($1); }

  |	'&' expr
		{ add_syntax_errmsg(2);  YYABORT; }

  |	expr '&'
		{
		    if (qry_is_all_ANDs)
			$$ = copy_final_truthtab ($1);
		    else {
			add_syntax_errmsg(2);
			YYABORT;
		    }
		}

  |	'|' expr
		{ add_syntax_errmsg(2);  YYABORT; }

  |	expr '|'
		{ add_syntax_errmsg(2);  YYABORT; }

  |	'(' ')'
		{ add_syntax_errmsg(3);  YYABORT; }

  |	expr '~' 
		{ add_syntax_errmsg(4);  YYABORT; }

  |	COLLOC_TOKEN expr
		{ add_syntax_errmsg(5);  YYABORT; }

  |	')' COLLOC_TOKEN
		{ add_syntax_errmsg(5);  YYABORT; }

  |	ERROR_TOKEN	{ YYABORT; }
  ;

/*********** Valid Expressions  ************/
expr:	WORD_TOKEN
		{ $$ = $1; }

  |	expr '&' expr
		{ $$ = boolyac_AND ($1, $3); } 

  |	expr '|' expr
		{ $$ = boolyac_OR ($1, $3); }

  |	'(' expr ')'
		{ $$ = $2; }

  |	'~' expr
		{ $$ = boolyac_NOT ($2); }

  |	WORD_TOKEN COLLOC_TOKEN WORD_TOKEN %prec COLLOC_TOKEN
		{
		    $$ = boolyac_COLLOC ($1, $2, $3);
		    if ($$ == NULL)
			YYABORT;
		}
  ;

%%
/********************* BOOLYAC.Y ********************/

