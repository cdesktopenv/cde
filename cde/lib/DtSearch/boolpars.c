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
/* $XConsortium: boolpars.c /main/5 1996/11/25 18:49:27 drk $
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
 *   FUNCTIONS: add_syntax_errmsg
 *		boolean_parse
 *		boolyac_AND
 *		boolyac_COLLOC
 *		boolyac_NOT
 *		boolyac_OR
 *		copy_final_truthtab
 *		copy_token
 *		creatett
 *		freett
 *		get_stem_truthtab
 *		main
 *		process_user_args
 *		yyerror
 *		yylex
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/********************* BOOLPARS.C ********************
 * $Id: boolpars.c /main/5 1996/11/25 18:49:27 drk $
 * February 1996.
 * AusText/DtSearch yacc-based boolean query parser.
 * Converts boolean query into stems array and truth table
 * for subsequent search.  Boolyac.y is the yacc source.
 * After processing by yacc, it becomes boolyac.c and boolyac.h.
 * This module contains all the related C source code: yylex,
 * yacc action functions, and the main AusText driver function, boolean_parse.
 * Additional information (format of TRUTHTAB) in header file boolpars.h.
 *
 * $Log$
 * Revision 1.4  1996/03/22  23:12:50  miker
 * Added string.h header and correctly cast strcspn() calls.
 *
 * Revision 1.3  1996/03/20  19:14:30  miker
 * Enable collocation expressions in stem (type 'S') searches.
 *
 * Revision 1.2  1996/03/13  22:35:59  miker
 * Changed char to UCHAR several places; similar typecasts.
 *
 * Revision 1.1  1996/03/05  15:52:06  miker
 * Initial revision
 */
#include "SearchE.h"
#include <stdlib.h>
#include <string.h>
#include "boolpars.h"
#include "boolyac.h"

#if (DtSrMAX_STEMCOUNT != 8)
#error DtSrMAX_STEMCOUNT is not defined to be 8.
#endif

#define PROGNAME	"BOOLPARS"
#define WORD_ENDERS	" \t\n\f()|@~&"
#define MAX_YYERRORS	4
#define MS_boolpars	28


/****************************************/
/*					*/
/*		 GLOBALS		*/
/*					*/
/****************************************/
int		qry_has_no_NOTs =	FALSE;
int		qry_is_all_ANDs =	FALSE;
TRUTHTAB	final_truthtab =	{ 0 };
int		parser_invalid_wordcount = 0;

static int	debugging_boolpars =	FALSE;
static unsigned char
		*final_permutes =	NULL;
static int	last_token_was_boolop =	TRUE;
static char	*msgbuf =		NULL;
static UCHAR	*next_lex_char =	NULL;
static int	paren_count =		0;
static TRUTHTAB	*ttlist =		NULL;
static int	yyerror_count =		0;
static size_t	yyleng;		/* same as in lex API */
static char	*yytext;	/* same as in lex API */


/****************************************/
/*					*/
/*	     add_syntax_errmsg		*/
/*					*/
/****************************************/
/* Action function called for yacc rules used to trap syntax errors.
 * Adds error message identified by msgno to user's msglist.
 */
void	add_syntax_errmsg (int msgno)
{
    switch (msgno) {
	case 1:
	    /* Message #2 is called in two places */
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 2,
		"%s Query field is empty."),
		PROGNAME"086");
	    DtSearchAddMessage (msgbuf);
	    break;

	case 2:
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 5,
		"%s Boolean operators must be positioned\n"
		"between words or expressions.  Two sequential words\n"
		"without an operator are interpreted as being separated\n"
		"by the AND operator (&)."),
		PROGNAME"091");
	    DtSearchAddMessage (msgbuf);
	    break;

	case 3:
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 6,
		"%s Expression in parentheses is missing."),
		PROGNAME"093");
	    DtSearchAddMessage (msgbuf);
	    break;

	case 4:
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 7,
		"%s NOT operator (~) must be positioned to\n"
		"the left of the word or expression it qualifies."),
		PROGNAME"098");
	    DtSearchAddMessage (msgbuf);
	    break;

	case 5:
	    /* Message #3 is called in two places */
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 3,
		"%s COLLOCATION operator (@) may\n"
		"only be positioned between two words."),
		PROGNAME"111");
	    DtSearchAddMessage (msgbuf);
	    break;

	case 6:
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 4,
		"%s One or more words in your\n"
		"query are not stored in database '%s'.") ,
		PROGNAME"089", usrblk.dblk->label);
	    DtSearchAddMessage (msgbuf);
	    break;

	default:
	    sprintf (msgbuf,  catgets(dtsearch_catd, MS_boolpars, 8,
		"%s Invalid boolean query.  Syntax Error #%d.") ,
		PROGNAME"100", msgno);
	    DtSearchAddMessage (msgbuf);
	    break;
    }
    return;
} /* add_syntax_errmsg() */


/****************************************/
/*					*/
/*	        creatett		*/
/*					*/
/****************************************/
/* Constructor for new truth table.
 * Allocates it, inits it, and links it into ttlist.
 */
static TRUTHTAB	*creatett (int stemno, int pmsz, unsigned char *permutes)
{
    TRUTHTAB *newtt = austext_malloc (sizeof(TRUTHTAB) + pmsz + 4,
	PROGNAME"140", NULL);
    memset (newtt, 0, sizeof(TRUTHTAB));
    newtt->stemno = stemno;
    newtt->pmsz = pmsz;
    newtt->permutes = (unsigned char *) (newtt + 1);
    memcpy (newtt->permutes, permutes, pmsz);
    newtt->next = ttlist;
    ttlist = newtt;
    return newtt;
} /* creatett() */


/****************************************/
/*					*/
/*		 freett			*/
/*					*/
/****************************************/
/* Destructor of passed truth table.
 * Unlinks it from ttlist and frees it.
 */
static void	freett (TRUTHTAB *argtt)
{
    TRUTHTAB	*tt;
    TRUTHTAB	**lastlink = &ttlist;
    for (tt = ttlist;  tt;  tt = tt->next) {
	if (tt == argtt) {
	    *lastlink = tt->next;
	    free (tt);
	    break;
	}
	lastlink = &tt->next;
    }
    return;
} /* freett() */

 
/****************************************/
/*					*/
/*	   copy_final_truthtab		*/
/*					*/
/****************************************/
/* Copys passed truth table into global final_truthtab.
 * Returns final_truthtab.
 */
TRUTHTAB	*copy_final_truthtab (TRUTHTAB *tt)
{
    memset (&final_truthtab, 0, sizeof(TRUTHTAB));
    if (!final_permutes)
	final_permutes = austext_malloc (300, PROGNAME"788", NULL);
    final_truthtab.pmsz =	tt->pmsz;
    final_truthtab.permutes =	final_permutes;
    memcpy (final_permutes, tt->permutes, final_truthtab.pmsz);
    return &final_truthtab;
} /* copy_final_truthtab() */
 

/****************************************/
/*					*/
/*	    get_stem_truthtab		*/
/*					*/
/****************************************/
/* Subroutine of yylex.  Also used in yacc action functions.
 * Creates and returns truth table for passed stem.
 * If stem is new, adds it to saveusr.stems array, and adds
 * the original query word string to usrblk.stems for msgs.
 * Returns NULL and posts err msg if array is full
 * or has other error.
 */
static TRUTHTAB	*get_stem_truthtab (char *newstem, char *origword)
{
    int			i, stemno;
    unsigned char	bitmask;
    unsigned char	*pmp;
    unsigned char	new_permutes [128];
    TRUTHTAB		*newtt;

    /* Check if stem is already in array */
    for (stemno = 0;  stemno < saveusr.stemcount;  stemno++)
	if (strcmp (newstem, saveusr.stems[stemno]) == 0)
	    break;

    /* Add new stem to array */
    if (stemno == saveusr.stemcount) {
	if (++saveusr.stemcount > DtSrMAX_STEMCOUNT) {
	    sprintf (msgbuf, catgets (dtsearch_catd, MS_boolpars, 9,
		"%s Too many terms in boolean query."),
		PROGNAME"1513");
	    DtSearchAddMessage (msgbuf);
	    saveusr.stemcount--;
	    return NULL;
	}
	strncpy (saveusr.stems[stemno], newstem, DtSrMAXWIDTH_HWORD);
	saveusr.stems [stemno] [DtSrMAXWIDTH_HWORD - 1] = 0;
	if (origword) {
	    strncpy (usrblk.stems[stemno], origword, DtSrMAXWIDTH_HWORD);
	    usrblk.stems [stemno] [DtSrMAXWIDTH_HWORD - 1] = 0;
	}
    }

    /* Stemno now indicates correct term in saveusr.stems.
     * Truth table for a single term has 128 8-bit permutes,
     * the 1/2 of all 256 possible permutations that have
     * that term's bit switched on.
     */
    bitmask = 1 << stemno;	/* mask with only newstem's bit on */
    pmp = new_permutes;
    for (i=0; i<256; i++)
	if ((i & bitmask) != 0) {
	    *pmp = i;
	    pmp++;
	}
    newtt = creatett (stemno, 128, new_permutes);
    if (debugging_boolpars) {
	fprintf (aa_stderr, "   WORD: stem[%d]='%c%s' expr=%p pmsz=%d\n",
	    stemno,
	    (saveusr.stems[stemno][0] == STEM_CH) ?
		'~'  :  saveusr.stems[stemno][0],
	    &saveusr.stems[stemno][1],
	    (void *) newtt, newtt->pmsz);
	fflush (aa_stderr);
    }
    return newtt;
} /* get_stem_truthtab() */


/****************************************/
/*					*/
/*	        boolyac_AND		*/
/*					*/
/****************************************/
/* Action function for AND expression rule.
 * Returns set INTERSECTION of passed truth tables,
 * ie only the permutes they have in common.
 * Any truth table, input or output, can be the empty or
 * the universal set.  For example: "(A & B) & ~A" is empty.
 */
TRUTHTAB	*boolyac_AND (TRUTHTAB *tt1, TRUTHTAB *tt2) {
    TRUTHTAB		*newtt;
    unsigned char	new_permutes [256];
    int			pm1, pm2, newpm;

    pm1 = pm2 = newpm = 0;
    while (pm1 < tt1->pmsz  &&  pm2 < tt2->pmsz) {
	if (tt1->permutes[pm1] < tt2->permutes[pm2])
	    pm1++;
	else if (tt1->permutes[pm1] > tt2->permutes[pm2])
	    pm2++;
	else {
	    new_permutes [newpm++] = tt1->permutes [pm1];
	    pm1++;
	    pm2++;
	}
    }

    /* Free old truthtabs, create new one. */
    freett (tt1);
    freett (tt2);
    newtt = creatett (-1, newpm, new_permutes);
    if (debugging_boolpars) {
	fprintf (aa_stderr, "    AND: exprs=%p,%p-->expr=%p pmsz=%d\n",
	    (void *) tt1, (void *) tt2, (void *) newtt, newtt->pmsz);
	fflush (aa_stderr);
    }
    return newtt;
} /* boolyac_AND() */


/****************************************/
/*					*/
/*	        boolyac_OR		*/
/*					*/
/****************************************/
/* Action function for OR expression rule.
 * Returns set UNION of passed truth tables.
 * Any truth table, input or output, can be the empty or
 * the universal set.  For example: "A | ~A" is universal.
 */
TRUTHTAB	*boolyac_OR (TRUTHTAB *tt1, TRUTHTAB *tt2) {
    TRUTHTAB		*newtt;
    unsigned char	new_permutes [256];
    unsigned char	*permutes1 = tt1->permutes;
    unsigned char	*permutes2 = tt2->permutes;
    int			pm1, pm2, newpm;

    pm1 = pm2 = newpm = 0;

    /* While neither permutes array is exhausted... */
    while (pm1 < tt1->pmsz  &&  pm2 < tt2->pmsz) {
	if (permutes1[pm1] < permutes2[pm2])
	    new_permutes [newpm++] = permutes1[pm1++];
	else if (permutes2[pm2] < permutes1[pm1])
	    new_permutes [newpm++] = permutes2[pm2++];
	else {
	    new_permutes [newpm++] = permutes1[pm1++];
	    pm2++;
	}
    }
    /* After one or both permutes arrays are exhausted... */
    while (pm1 < tt1->pmsz)
	new_permutes [newpm++] = permutes1[pm1++];
    while (pm2 < tt2->pmsz)
	new_permutes [newpm++] = permutes2[pm2++];

    /* Free old truthtabs, create new one. */
    freett (tt1);
    freett (tt2);
    newtt = creatett (-1, newpm, new_permutes);
    if (debugging_boolpars) {
	fprintf (aa_stderr, "     OR: exprs=%p,%p-->expr=%p pmsz=%d\n",
	    (void *) tt1, (void *) tt2, (void *) newtt, newtt->pmsz);
	fflush (aa_stderr);
    }
    return newtt;
} /* boolyac_OR() */


/****************************************/
/*					*/
/*	        boolyac_NOT		*/
/*					*/
/****************************************/
/* Action function for NOT expression rule.
 * Returns set COMPLEMENT of passed truth table,
 * ie the universal set minus the passed set,
 * ie all possible permutes except those passed.
 * Either the old or the new truth table can be
 * the empty or the universal set.
 */
TRUTHTAB	*boolyac_NOT (TRUTHTAB *oldtt) {
    TRUTHTAB		*newtt;
    unsigned char	new_permutes [256];
    int			oldpm, newpm;
    int			candidate;

    oldpm = newpm = 0;
    for (candidate = 0;  candidate < 256;  candidate++) {
	if (oldpm >= oldtt->pmsz  ||  candidate < oldtt->permutes [oldpm]) {
	    new_permutes [newpm++] = candidate;
	}
	/*
	 * oldtt not done  &&  candidate == oldtt.
	 * (candidate > oldtt not possible).
	 */
	else {
	    oldpm++;
	}
    }
    freett (oldtt);
    newtt = creatett (-1, newpm, new_permutes);
    if (debugging_boolpars) {
	fprintf (aa_stderr, "    NOT: expr=%p-->expr=%p pmsz=%d\n",
	    (void *) oldtt, (void *) newtt, newtt->pmsz);
	fflush (aa_stderr);
    }
    return newtt;
} /* boolyac_NOT() */


/****************************************/
/*					*/
/*	      boolyac_COLLOC		*/
/*					*/
/****************************************/
/* Action function for COLLOCATION expression rule.
 * The record set satisfying a collocation expression is
 * generated dynamically.  At the parse level it is equivalent
 * to a separate 'word' with its own (undetermined) record set.
 * So it's given its own slot in saveusr.stems.  The word
 * in saveusr.stems is formated "@ssttv[v...]" where ss and tt are
 * ascii numbers that index the original collocated words
 * in saveusr.stems, and v... is the collocation value integer.
 * For example, "@03005" represents the collocation of stem
 * number 3 and stem number 0, with collocation value 5.
 *
 * Returns NULL and errmsg on msglist if any problems.
 */
TRUTHTAB	*boolyac_COLLOC (
		    TRUTHTAB	*word1tt,
		    int		colloc_val,
		    TRUTHTAB	*word2tt)
{
    TRUTHTAB	*newtt;
    char	wordbuf [DtSrMAXWIDTH_HWORD];

    if (word1tt->stemno < 0  ||  word2tt->stemno < 0) {
	/* Message #3 is called in two places */
	sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 3,
	    "%s COLLOCATION operator (@) may\n"
	    "only be positioned between two words."),
	    PROGNAME"371");
	DtSearchAddMessage (msgbuf);
	return NULL;
    }
    if (word1tt->stemno == word2tt->stemno) {
	sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 12,
	    "%s Collocation operator is not\n"
	    "permitted between identical words."),
	    PROGNAME"377");
	DtSearchAddMessage (msgbuf);
	return NULL;
    }
    sprintf (wordbuf, COLLOC_STEM_FORMAT,
	word1tt->stemno, word2tt->stemno, colloc_val);
    if ((newtt = get_stem_truthtab (wordbuf, wordbuf)) == NULL)
	return NULL;
    freett (word1tt);
    freett (word2tt);
    if (debugging_boolpars) {
	fprintf (aa_stderr, " COLLOC: exprs=%p,%p-->expr=%p pmsz=%d\n",
	    (void *) word1tt, (void *) word2tt, (void *) newtt, newtt->pmsz);
	fflush (aa_stderr);
    }
    return newtt;
} /* boolyac_COLLOC() */


/****************************************/
/*					*/
/*		 yyerror		*/
/*					*/
/****************************************/
/* Replaces standard yacc error routine. */
void	yyerror (char *msg) {
    if (strcmp (msg, "syntax error") == 0) {
	if (DtSearchHasMessages())
	    return;
	else if (parser_invalid_wordcount > 0)
	    add_syntax_errmsg(6);
	else {
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 1,
		"%s Your search string is an invalid\n"
		"boolean query.  Please reformulate and try again."),
		PROGNAME"001");
	    DtSearchAddMessage (msgbuf);
	}
    }
    else
	DtSearchAddMessage (msg);
    return;
} /* yyerror() */


/****************************************/
/*					*/
/*		copy_token		*/
/*					*/
/****************************************/
/* Subroutine of yylex().  Copies passed substring
 * Into a zero-terminated buffer of its own.
 * Static buffer good until next call.
 */
static char	*copy_token (UCHAR *tokenp, size_t toklen)
{
    static char		*buf =	NULL;
    static size_t	bufsz =	0;
    if (toklen > bufsz) {
	if (buf)
	    free (buf);
	bufsz = toklen + (toklen >> 1);	/* 1.5 times size needed */
	buf = austext_malloc (bufsz + 4, PROGNAME"182", NULL);
    }
    strncpy (buf, (char *) tokenp, toklen);
    buf [toklen] = 0;
    return buf;
} /* copy_token() */


/****************************************/
/*					*/
/*		   yylex		*/
/*					*/
/****************************************/
/* Delivers tokens to yyparse() from usrblk.query */
int	yylex (void)
{
    int		retn_token;
    PARG	parg;
    char	*stembufp;
    char	mystembuf [DtSrMAXWIDTH_HWORD + 4];

GET_ANOTHER_TOKEN:

    /* Skip white space */
    while (ascii_charmap[*next_lex_char] & WHITESPACE)
	next_lex_char++;

    /* Terminating zero indicates end of query and end of parse.
     * Automatically close unbalanced parentheses.
     */
    if (*next_lex_char == 0) {
	if (paren_count > 0) {
	    paren_count--;
	    retn_token =	')';
	    yytext =		")";
	    yyleng =		1;
	    goto DELIVER_TOKEN;
	}
	retn_token =		0;
	yytext =		"";
	yyleng =		0;
	goto DELIVER_TOKEN;
    }

    switch (*next_lex_char) {
	case '|':	/* OR operator */
	    last_token_was_boolop =	TRUE;
	    retn_token =		'|';
	    yytext =			"|";
	    yyleng =			1;
	    next_lex_char++;
	    break;

	case '~':	/* NOT operator */
	    if (!last_token_was_boolop) {
		/* Generate implied AND between words
		 * and parenthesized expressions.
		 * A NOT is not itself boolean; it must 
		 * precede the next word or expression.
		 */
		last_token_was_boolop =	TRUE;
		retn_token =		'&';
		yytext =		"&";
		yyleng =		1;
		break;
	    }
	    last_token_was_boolop =	TRUE;
	    retn_token =		'~';
	    yytext =			"~";
	    yyleng =			1;
	    next_lex_char++;
	    break;

	case '&':	/* AND operator */
	    if (last_token_was_boolop && qry_is_all_ANDs) {
		/* Ignore multiple AND operators.
		 * These might occur if we silently
		 * discarded some invalid words.
		 */
		next_lex_char++;
		goto GET_ANOTHER_TOKEN;
	    }
	    last_token_was_boolop =	TRUE;
	    retn_token =		'&';
	    yytext =			"&";
	    yyleng =			1;
	    next_lex_char++;
	    break;

	case '(':	/* OPEN parentheses */
	    if (!last_token_was_boolop) {
		/* Generate implied AND between words
		 * and parenthesized expressions.
		 */
		last_token_was_boolop =	TRUE;
		retn_token =		'&';
		yytext =		"&";
		yyleng =		1;
		break;
	    }
	    paren_count++;
	    retn_token =		'(';
	    yytext =			"(";
	    yyleng =			1;
	    next_lex_char++;
	    break;

	case ')':	/* CLOSE parentheses */
	    /* Just discard excessive right parentheses */
	    if (--paren_count < 0) {
		paren_count = 0;
		next_lex_char++;
		goto GET_ANOTHER_TOKEN;
	    }
	    last_token_was_boolop =	FALSE;
	    retn_token =		')';
	    yytext =			")";
	    yyleng =			1;
	    next_lex_char++;
	    break;

	case '@':	/* COLLOCATION operator */
	    /* Collocation token:
	     * Token is defined as the collocation char followed
	     * by one or more numeric digits: "@#[#...]".
	     * Syntactically it's a kind of an AND operator.
	     * Semantically it's a pseudo word token
	     * (it will occupy a slot in the stems array).
	     * The yylval is the integer value following 
	     * the collocation character.
	     */
	    yyleng = strcspn ((char *) next_lex_char + 1, WORD_ENDERS) + 1;
	    yytext = copy_token (next_lex_char, yyleng);
	    next_lex_char += yyleng;

	    if ((usrblk.dblk->dbrec.or_dbaccess & ORA_BLOB) == 0) {
		retn_token = ERROR_TOKEN;
		sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 10,
		    "%s Collocation searches not available for database '%s'."),
		    PROGNAME"2567", usrblk.dblk->label);
		DtSearchAddMessage (msgbuf);
		break;
	    }
	    yylval.int_val = atoi (yytext + 1);
	    if (yylval.int_val <= 0) {
		retn_token = ERROR_TOKEN;
		sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 11,
		    "%s Collocation operator '%.*s' is invalid.\n"
		    "Correct format is '@n' where n is greater than zero.") ,
		    PROGNAME"294", DtSrMAXWIDTH_HWORD, yytext);
		DtSearchAddMessage (msgbuf);
		break;
	    }
	    last_token_was_boolop =	TRUE;
	    retn_token =		COLLOC_TOKEN;
	    break;


	default:
	    /* Presumed word token:
	     * Token is all text chars until next whitespace,
	     * next lex token, or end of string.
	     * Linguistically parse it and optionally stem it.
	     * The token value is the truth table for one
	     * word: all permutes with only that word's
	     * bits turned on.  If the word is already
	     * in the stems array, then the permutes
	     * position is the word's index in the array.
	     * If the word is not in the array, it's added.
	     * If the array is full, then an error is reported.
	     */
	    if (!last_token_was_boolop) {
		/* Generate implied AND between words
		 * and parenthesized expressions.
		 */
		last_token_was_boolop =	TRUE;
		retn_token =		'&';
		yytext =		"&";
		yyleng =		1;
		break;
	    }
	    yyleng = strcspn ((char *) next_lex_char, WORD_ENDERS);
	    yytext = copy_token (next_lex_char, yyleng);
	    next_lex_char += yyleng;
	    /*
	     * Linguistically parse the token.
	     * Failure can occur because word is too short
	     * or too long, it's on the stoplist, etc.
	     * Setting PA_MSGS causes parser to explain
	     * invalid words with a msg.
	     */
	    memset (&parg, 0, sizeof(PARG));
	    parg.dblk =		usrblk.dblk;
	    parg.string =	yytext;
	    /*****if (!qry_is_all_ANDs)********/
		parg.flags =	PA_MSGS;
	    stembufp = usrblk.dblk->parser (&parg);
	    if (debugging_boolpars) {
		fprintf (aa_stderr, "   lang: '%s' -> '%s'\n",
		    yytext, (stembufp)? stembufp : "<null>");
		fflush (aa_stderr);
	    }
	    /*
	     * If token is not a linguistically valid word,
	     * one of two things can happen.  If the query
	     * is all_ANDs (most common type) we silently
	     * ignore the token.
	     * Otherwise report error and quit now.
	     */
	    if (stembufp == NULL) {
		parser_invalid_wordcount++;
		if (qry_is_all_ANDs)
		    goto GET_ANOTHER_TOKEN;
		retn_token = ERROR_TOKEN;
		if (!DtSearchHasMessages()) {
		    sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 13,
			"%s Word '%.*s' is invalid.") ,
			PROGNAME"315", DtSrMAXWIDTH_HWORD, yytext);
		    DtSearchAddMessage (msgbuf);
		}
		break;
	    }
	    if (strlen(stembufp) != strlen(yytext)) {
		retn_token =		ERROR_TOKEN;
		sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 14,
		    "%s String '%.*s' is not a single word.") ,
		    PROGNAME"634", DtSrMAXWIDTH_HWORD, yytext);
		DtSearchAddMessage (msgbuf);
		break;
	    }
	    /*
	     * If stemming, we must prefix term with
	     * special stem char in the stems array.
	     */
	    if (usrblk.request == OE_SRCH_STEMS) {
		stembufp = usrblk.dblk->stemmer (stembufp, usrblk.dblk);
		if (debugging_boolpars) {
		    fprintf (aa_stderr, " stemer: -> '%s'\n", stembufp);
		    fflush (aa_stderr);
		}
		mystembuf[0] = STEM_CH;
		strncpy (mystembuf + 1, stembufp, DtSrMAXWIDTH_HWORD);
		mystembuf [DtSrMAXWIDTH_HWORD - 1] = 0;
		stembufp = mystembuf;
	    }

	    /* Load stem into stems arrays and return it's truth table. */
	    if ((yylval.truthtab = get_stem_truthtab (stembufp, yytext))) {
		retn_token =		WORD_TOKEN;
		last_token_was_boolop =	FALSE;
	    }
	    else
		retn_token =		ERROR_TOKEN;
	    break;

    } /* switch on *next_lex_char */

DELIVER_TOKEN:
    if (debugging_boolpars) {
	fprintf (aa_stderr,
	    "  yylex: op?=%d parct=%d tok#=%d lval=%p%sYYTEXT='%s'\n",
	    last_token_was_boolop, paren_count,
	    retn_token, (void *) yylval.truthtab,
	    (retn_token == COLLOC_TOKEN)? "\t\t" : "\t",
	    yytext);
	fflush (aa_stderr);
    }
    return retn_token;

} /* yylex() */


/****************************************/
/*					*/
/*	       boolean_parse		*/
/*					*/
/****************************************/
/* Called from Opera_Engine for boolean searches.
 * Driver for yyparse().
 * Expects usrblk.request == OE_SRCH_STEMS or OE_SRCH_WORDS.
 * If parse is completely successful (query is valid), outputs
 *   saveusr.stemcount,
 *   saveusr.stems (stemmed if necessary with STEM_CH as first char,
 *	and phony colloc words with '@' as first char),
 *   usrblk.stems (original unstemmed query terms for err msgs),
 *   final_truthtab,
 *   qry_has_no_NOTs,
 *   qry_is_all_ANDs,
 * and returns TRUE.  Truthtab allocation good until next call.
 * If parse fails, returns FALSE and err msg(s) on msglist.
 */
int	boolean_parse (void)
{
    int		i;
    char	*cptr;
    TRUTHTAB	*tt, *ttnext;

    debugging_boolpars = (usrblk.debug & USRDBG_BOOL);
    if (!msgbuf)
	msgbuf = austext_malloc (300 + DtSrMAXWIDTH_HWORD,
		PROGNAME"255", NULL);

    /* Test for empty query */
    if (usrblk.query == NULL) {
EMPTY_QUERY:
	/* Message #2 is called in two places */
	sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 2,
	    "%s Query is empty."), PROGNAME"289");
	DtSearchAddMessage (msgbuf);
	return FALSE;
    }
    for  (cptr = usrblk.query;  *cptr;  cptr++) {
	if ((ascii_charmap[*cptr] & WHITESPACE) == 0)
	    break;
    }
    if (*cptr == 0)
	goto EMPTY_QUERY;

    /* Init globals for yylex and yyparse */
    next_lex_char =		(UCHAR *) usrblk.query;
    paren_count =		0;
    yyerror_count =		0;
    last_token_was_boolop =	TRUE;
    saveusr.stemcount =		0;
    parser_invalid_wordcount =	0;

    /* Query "is all ANDS" if it has no ORs, NOTs, or COLLOCs.
     * Missing or linguistically invalid words will be silently
     * discarded for all_ANDs queries.
     * Query "has no NOTs" if it has no NOTs.
     * Results from queries without NOTs can be statistically sorted.
     */
    qry_has_no_NOTs = !strchr (usrblk.query, '~');
    qry_is_all_ANDs = !strpbrk (usrblk.query, "|~@");

    if (debugging_boolpars || (usrblk.debug & USRDBG_SRCHCMPL)) {
	fprintf (aa_stderr,
	    "start boolean_parse: stem?=%d allANDs?=%d noNOTs?=%d\n"
	    "  query: '%s'\n",
	    (usrblk.request == OE_SRCH_STEMS),
	    qry_is_all_ANDs, qry_has_no_NOTs, usrblk.query);
	fflush (aa_stderr);
    }

    if (yyparse() != 0)
	return FALSE;

    /* Free entire remaining ttlist.  Only you
     * can prevent forest fires and memory leaks.
     */
    tt = ttlist;
    while (tt) {
	ttnext = tt->next;
	free (tt);
	tt = ttnext;
    }
    ttlist = NULL;

    if (debugging_boolpars || (usrblk.debug & USRDBG_SRCHCMPL)) {
	print_stems (saveusr.stemcount, saveusr.stems,
	    PROGNAME"815 end boolean_parse, syntax ok,");
	fprintf (aa_stderr, "  permutes=%d:", final_truthtab.pmsz);
	for (i=0;  i<16;  i++) {
	    if (i >=  final_truthtab.pmsz)
		break;
	    fprintf (aa_stderr, " %02x", final_truthtab.permutes [i]);
	}
	fputc ('\n', aa_stderr);
	fflush (aa_stderr);
    }

    if (final_truthtab.pmsz <= 0) {
	sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 15,
	    "%s Your query cannot logically return\n"
	    "any records.  Please reformulate and try again."),
	    PROGNAME"334");
	DtSearchAddMessage (msgbuf);
	return FALSE;
    }
    if (final_truthtab.pmsz >= 256) {
	sprintf (msgbuf, catgets(dtsearch_catd, MS_boolpars, 16,
	    "%s Your query will return entire database\n" 
	    "'%s'.  Please reformulate and try again.") ,
	    PROGNAME"341", usrblk.dblk->label);
	DtSearchAddMessage (msgbuf);
	return FALSE;
    }
    return TRUE;
} /* boolean_parse() */


#ifdef TESTBOOL	/*-----------------------------------------------*/

USRBLK		usrblk = { 0 };
DBLK		dblk;
SAVEUSR		saveusr = { 0 };
extern int	debugging_teskey;
extern int	debugging_paice;
extern int	debugging_jpn;

/****************************************/
/*					*/
/*	    process_user_args		*/
/*					*/
/****************************************/
/* Subroutine of main().  Validates and loads global
 * variables with values from command line arguments.
 */
static void	process_user_args (int argc, char *argv[])
{
    int		i;
    char	*argptr;
    char	*cptr;
    char	*src, *targ;
    int		oops = FALSE;

    /* Each pass grabs new parm of "-xxx" format */
    argc--, argv++;
    while (argc > 0) {
	argptr = argv[0];
	if (*argptr != '-')
	    break;
	switch (argptr[1]) {
	    case 'm':
		if (argptr[2] == 'x')
		    dblk.dbrec.or_maxwordsz = atoi (argptr + 3);
		else if (argptr[2] == 'n')
		    dblk.dbrec.or_minwordsz = atoi (argptr + 3);
		else
		    goto BAD_ARG;
		break;

	    case 'l':
		dblk.dbrec.or_language = atoi (argptr + 2);
		break;

	    case 'd':
		for (cptr = argptr+2;  *cptr != 0;  cptr++) {
		    switch (*cptr) {
			case 't': debugging_teskey = TRUE; break;
			case 'p': debugging_paice = TRUE; break;
			case 'j': debugging_jpn = TRUE; break;
			default:
			    oops = TRUE;
			    fprintf (aa_stderr,
				"%s Invalid debug option %c.\a\n",
				PROGNAME"049", *cptr);
			    break;
		    }
		}
		break;

BAD_ARG:
	    default:
		oops = TRUE;
		fprintf (aa_stderr,
		    "%s Invalid command line argument '%s'.\a\n",
		    PROGNAME"059", argptr);
		break;
	} /* end switch */

	argc--, argv++;
    } /* main loop on each arg */


    if (oops) {
	fprintf (aa_stderr,
	    "\nUSAGE: %s [options]\n"
	    "  -mx#   maximum word size.\n"
	    "  -mn#   minimum word size.\n"
	    "  -dtpj  Debug: Teskey, Paice, Japanese.\n"
	    "  -l#    language number.  Default 0.\n",
	    aa_argv0);
	exit(2);
    }
    return;
}  /* process_user_args() */


/****************************************/
/*					*/
/*		   main			*/
/*					*/
/****************************************/
int	main	(int argc, char *argv[])
{
    int		i;
    int		valid_boolpars;
    char	*cptr;
    char	linebuf [1024];

    /* Init global variables */
    aa_argv0 = argv[0];

    memset (&usrblk, 0, sizeof(USRBLK));
    usrblk.dblk = &dblk;
    usrblk.debug |= USRDBG_BOOL;	/* set debugging_boolpars */

    memset (&dblk, 0, sizeof(DBLK));
    strcpy (dblk.name, "testbool");
    dblk.label = dblk.name;
    dblk.dbrec.or_dbaccess |= ORA_BLOB;	/* enable collocations */

    /* Read command line args */
    process_user_args (argc, argv);

    if (!load_language (&dblk, NULL)) {
	fprintf (aa_stderr,
	    PROGNAME"140 load_language() failed.  Msgs:\n%s\n",
	    DtSearchGetMessages());
	return 2;
    }
    fprintf (aa_stderr, "  lang=%d minwdsz=%d maxwdsz=%d.\n",
	dblk.dbrec.or_language,
	dblk.dbrec.or_minwordsz,
	dblk.dbrec.or_maxwordsz);

    /* Main loop.  Each line is a boolean query. */
    printf ("Enter an AusText boolean query.  'q' or '.' to quit.\n"
	"If first char is '$', words will be stemmed:\n> ");
    fflush (stdout);
    while (fgets (linebuf, sizeof(linebuf), stdin) != NULL) {

	linebuf [sizeof(linebuf) - 1] = 0;
	if (strcmp (linebuf, ".\n") == 0)
	    break;
	if (strcmp (linebuf, "q\n") == 0)
	    break;
	if (linebuf[0] == '\n')
	    break;
	linebuf [strlen(linebuf) - 1] = 0;	/* overlay \n */

	if (linebuf[0] == '$') {
	    usrblk.query = linebuf + 1;
	    usrblk.request = OE_SRCH_STEMS;
	}
	else {
	    usrblk.query = linebuf;
	    usrblk.request = OE_SRCH_WORDS;
	}

	if (!boolean_parse())
	    puts (PROGNAME"707 boolean_parse() returned FALSE (OE_BAD_QUERY).");
	if (DtSearchHasMessages()) {
	    printf ("mmmmm Messages returned to user mmmmmmmmmmmmmmmmmm\n"
		"%s\nmmmmm End of messages to user mmmmmmmmmmmmmmmmmmmm\n",
		DtSearchGetMessages());
	    DtSearchFreeMessages();
	}

    printf ("--------------------------------\n> ");
    fflush (stdout);
    } /* main read loop for each query line */
    return 0;
} /* main() */

#endif /* TESTBOOL */

/********************* BOOLPARS.C ********************/

