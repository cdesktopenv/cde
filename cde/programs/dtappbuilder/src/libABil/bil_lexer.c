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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */

/*
 *	$XConsortium: bil_lexer.c /main/3 1995/11/06 18:23:43 rswiston $
 *
 *	@(#)bil_lexer.c	1.9 02 Apr 1995	cde_app_builder/src/libABil
 *
 * 	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 */


/*
 * bil_lexer.c
 */
#include <string.h>
#include <ctype.h>
#include <ab_private/abio.h>
#include <ab_private/util.h>
#include "load.h"
#include "bil_parse.h"
#include "bilP.h"
#undef DEBUG
/* #define DEBUG */


/*
 * Public symbols
 */
FILE	*AByyin = NULL;

#define MAX_TOKEN_LEN	1023
#define MAX_TOKEN_SIZE	(MAX_TOKEN_LEN + 1)	/* len+1 for NULL */

static int	line_number= 1;
static int	last_token= AB_BIL_UNDEF;
static char*	last_token_value= NULL;
static int	last_value_type= AB_BIL_UNDEF;
static char	tokenText[MAX_TOKEN_SIZE] = "";
static int	tokenTextLen = 0;

#define save_token(x)		(last_token = (x))
#define save_type(x)		(save_token(last_value_type = (x)))

#ifdef DEBUG
#define retkey(x)	{  int keyword = (x); \
			   printf("lex-k:/%s/%d/%s/\n", \
			   tokenText, \
			   keyword, \
			   util_strsafe(bilP_token_to_string(keyword))); \
			   return save_token(keyword);}

#define retval(x)	{  int value = (x); \
			   printf("lex-v:/%s/%d/%s/\n", \
			   tokenText, \
			   value, \
			   util_strsafe(bilP_token_to_string(value))); \
			   return save_type(value);}

#define retchar(x)	{   int value = (x); \
			    printf("lex:'%c'\n", (value)); \
			    return (value);}

#else
#define retkey(x)		return save_token(x)
#define retval(x)		return save_type(x)
#define retchar(x)		return (x)
#endif /* DEBUG */

static int	get_token(FILE *file);
static int	get_keyword(FILE *file, int lastChar);
static int	get_comment(FILE *file, int lastChar);
static int	get_string(FILE *file, int lastChar);
static int	get_ident(FILE *file, int lastChar);
static int	get_number(FILE *file, int lastChar);
static int	get_(FILE *file, int lastChar);


int
AByylex(void)
{
    return get_token(AByyin);
}


/*
 * Returns the token
 */
static int
get_token(FILE *file)
{
    int		c;	/* static for speed */
    
    while (TRUE)
    {
	c = fgetc(file);
	switch (c)
	{
	case EOF:
	    return 0;
	break;

        case ':':
	    retkey(get_keyword(file, c));
	break;

        case 'a': case 'b': case 'c': case 'd': case 'e':
        case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y':
        case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E':
        case 'F': case 'G': case 'H': case 'I': case 'J':
        case 'K': case 'L': case 'M': case 'N': case 'O':
        case 'P': case 'Q': case 'R': case 'S': case 'T':
        case 'U': case 'V': case 'W': case 'X': case 'Y':
        case 'Z':
	    retval(get_ident(file, c));
	break;

        case '0': case '1': case '2': case '3': case '4': 
        case '5': case '6': case '7': case '8': case '9': 
	case '-':
	    retval(get_number(file, c));
	break;

        case '\"':
	    retval(get_string(file, c));
	break;

        case '/':
	    if ((c = fgetc(file)) == '/')
	    {
		get_comment(file, c);
	    }
	    else
	    {
		ungetc(c, file);
                retval(get_ident(file, '/'));
            }
	break;

        case '(': case ')':
	    retchar(c);
	break;

        case '\n':
	    ++line_number;
	break;

        case '.':
	    c = fgetc(file);
            if (isdigit(c))
            {
		ungetc(c,file);
		retval(get_number(file, '.'));
            }
            else
            {   
                ungetc(c, file);
                retval(get_ident(file, '.'));
            }
        break;
	}
    }

    return 0;
}


static int
get_ident(FILE *file, int lastChar)
{
    int		c = 0;

    tokenTextLen = 0;
    tokenText[tokenTextLen++] = lastChar;

    while ( ((c = fgetc(file)) != EOF) && ( isalnum(c) 
	    || (c == '_') || (c == '.') || (c == '/')) )
    {
        tokenText[tokenTextLen++] = c;
    }
    tokenText[tokenTextLen] = 0;
    if (c != EOF)
    {
	ungetc(c, file);
    }

    return AB_BIL_VALUE_IDENT;
}


/*
 * Gets a keyword and returns its BIL_TOKEN
 */
static int
get_keyword(FILE *file, int lastChar)
{
    int		iChar= 0;
    BIL_TOKEN	keywordToken= AB_BIL_UNDEF;

    tokenTextLen = 0;
    tokenText[tokenTextLen++] = lastChar;
    while (   ((iChar= fgetc(file)) != EOF)
	   && (((!isspace(iChar)) && (iChar != ')') && (iChar != '(')))
	   )
    {
	tokenText[tokenTextLen++]= iChar;
    }
    tokenText[tokenTextLen]= 0;
    if (iChar != EOF)
    {
	ungetc(iChar, file);
    }

    keywordToken= bilP_string_to_token(tokenText);

    if (keywordToken == AB_BIL_UNDEF)
    {
	/*
	 * This will not be executed if the actual :undef keyword is seen
	 * in the BIL file.  :undef is AB_BIL_UNDEF_KEYWORD, and is a
	 * valid keyword.
	 *
	 * The token AB_BIL_UNDEF signifies that the keyword in the BIL
	 * file was invalid and could not be converted to a token.
	 */
	char	msg[256];
	sprintf(msg, 
	    catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 35,
	      "unknown keyword - %s"), 
	    tokenText);
	abil_print_custom_load_err(msg);
    }
    else
    {
        switch (keywordToken)
        {
	    /*
	     * If it's a type, save it
	     */
	    case AB_BIL_FALSE:
	    case AB_BIL_NIL:
	    case AB_BIL_TRUE:
		    save_type(keywordToken);
		    break;
	}
    }

    return keywordToken;
}


static int
get_number(FILE *file, int lastChar)
{
    int		c = -1;
    BOOL	dotSeen = FALSE;

    tokenTextLen = 0;
    tokenText[tokenTextLen++] = lastChar;

    while (   ((c = fgetc(file)) != EOF)
	   && (isalnum(c) || (c == '.')) )
    {
	if (c == '.')
	{
	    /* only allow 1 decimal in number */
	    if (dotSeen)
	    {
		break;
	    }
	    dotSeen = TRUE;
	}
        tokenText[tokenTextLen++] = c;
    }
    tokenText[tokenTextLen] = 0;
    if (c != EOF)
    {
	ungetc(c, file);
    }

    return dotSeen? AB_BIL_VALUE_FLOAT:AB_BIL_VALUE_INT;
}


/*
 * Reads in and discards a comment
 */
static int
get_comment(FILE *file, int lastChar)
{
	int	c;
	while (((c= fgetc(file)) != EOF) && (c != '\n'))
	{
		/* fprintf(yyout, "%c", c); fflush(yyout); */
	}
	++line_number;
	return 0;
}

/*
 * Reads in and saves a string value.
 */
static int
get_string(FILE *file, int lastChar)
{
    ISTRING	istring = NULL;
    char	*string= NULL;

    ungetc(lastChar, file);
    abio_get_string(file, &istring);

    tokenText[0]= 0; tokenTextLen= 0;
    string = istr_string(istring);
    if (string != NULL)
    {
	int i;
	int	len = strlen(string);
	for (i= 0; i < len; ++i)
	{
	    if (string[i] == '\n')
	    {
		++line_number;
	    }
	}
        util_strncpy(tokenText, string, MAX_TOKEN_LEN);
	tokenTextLen= strlen(tokenText);
    }
    istr_destroy(istring);

    return AB_BIL_VALUE_STRING;
}


/*
 * Called by the lexical analyzer or parser whenever an error occurs.
 */
void 
AByyerror(const char *message)
{
    char        tokenMsg[1024] = "";
    char        errMsg[1024] = "";

    if (strlen(tokenText) > 0) 
    {
	sprintf(tokenMsg, 
	  catgets(ABIL_MESSAGE_CATD, ABIL_MESSAGE_SET, 37, ", near '%s'"),
	  tokenText);
    }
    sprintf(errMsg, "%s%s\n", message, tokenMsg);
    abil_print_custom_load_err(errMsg);
}

#ifdef BOGUS 
/*
 * Called by lexical analyzer at EOF.  Returning 1 ends parsing.
 */
int 
yywrap()
{
    return 1;
}
#endif /* BOGUS */


/*************************************************************************
 **									**
 **	Public functions						**
 **									**
 *************************************************************************/

int
bilP_load_reset(void)
{
	line_number= 1;
	last_token= AB_BIL_UNDEF;
	last_value_type= AB_BIL_UNDEF;
	last_token_value= NULL;
	return 0;
}

int
bilP_load_get_token(void)
{
	return last_token;
}

int
bilP_load_get_value_type(void)
{
	return last_value_type;
}

STRING
bilP_load_get_value(void)
{
	return tokenText;
}

int
bilP_load_get_length(void)
{
	return tokenTextLen;
}

int
bilP_load_get_line_number(void)
{
    return line_number;
}

int
bilP_load_set_line_number(int lineNumber)
{
    line_number= lineNumber;
    return 0;
}

void
allprint(char c)
{
   printf("'%c'", c);
}

void 
sprint(char *s)
{
    printf("\"%s\"", s);
}

void
bilP_reset_token_text(void)
{
    sprintf(tokenText, "%s", "");
}
