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
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: display_jstate
 *		jpn_parser
 *		kanji_compounder
 *		load_jpn_language
 *		load_jpntree
 *		parse_substring
 *		read_jchar
 *		search_kanjitree
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/******************** JPN.C ********************
 * $TOG: jpn.c /main/7 1999/10/14 14:11:33 mgreess $
 * September 1995.
 * Includes functions and data for parsing Japanese,
 * supported languages DtSrLaJPN and DtSrLaJPN2.
 * Currently only supports EUC packed format,
 * but should be easily extendable to Shift-JIS.
 * JIS can be supported if half-width katakana are excluded
 * (no SI or SO chars to conflict with the ^O stemming char,
 * and engine must decide never to balk at ESCape sequences).
 * Will not support Unicode or other fixed width, n-wide
 * encodings that would conflict with ascii in either byte.
 * Does not require wide char or multibyte char functions.
 * There is no Japanese stemmer(), ie standard null_stemmer() is used.
 *
 * Code Set 0 can be either 7-bit ASCII or 7-bit JIS-Roman.
 * The parser() for ASCII is the full teskey_parser()
 * used for European languages with an ascii char set.
 * Min/max word size, stoplists, and include lists may be
 * used if provided, as in European languages.
 *
 * Code Set 1 is JIS X 0208-1990.
 * Symbols and line drawing elements are not indexed.
 * Hirigana strings are discarded as equivalent to stoplist words.
 * Contiguous strings of katakana, Roman, Greek, or cyrillic
 * are parsed as single words.
 *
 * Individual kanji chars are parsed as single words.
 * In addition, for language DtSrLaJPN, all kanji compounds
 * (pairs, triplets, etc) found in any contiguous string of
 * kanjis will be parsed up to a maximum word size
 * defined in MAX_KANJI_CLEN (see caveat below).
 * For language DtSrLaJPN2, only kanji substrings listed
 * in a .knj file are parsed as additional compound words.
 * Characters from unassigned kuten rows are presumed to be
 * user-defined kanji and are parsed as such.
 *
 * Code Set 2 is 1/2 width katakana.
 * Contiguous strings are parsed as single words.
 *
 * Code Set 3 is JIS X 0212-1990.
 * Parsing is similar to Code Set 1: discard symbols, etc,
 * contiguous strings of related foreign characters equal words,
 * and individual kanji and unassigned chars equal single words,
 * with additional kanji compounding depending on language.
 * Row 5 has 4 new katakana (not yet officially approved)
 * so it is treated here as katakana.
 *
 * $Log$
 * Revision 2.8  1996/04/10  20:24:33  miker
 * Fixed bug in kanji tree loader.
 *
 * Revision 2.7  1996/03/25  18:55:15  miker
 * Changed FILENAME_MAX to _POSIX_PATH_MAX.
 *
 * Revision 2.6  1996/03/13  22:57:40  miker
 * Added prolog.  Changed char to UCHAR several places.
 *
 * Revision 2.5  1996/03/05  16:09:58  miker
 * Made jchar array of unsigned chars for compat with Sun compilers.
 * Added test of PA_MSGS for yacc-based boolean queries.
 *
 * Revision 2.4  1996/02/01  19:08:10  miker
 * AusText 2.1.11, DtSearch 0.3:  Major rewrite for new parsers.
 * Made optional power series kanji compounding (KANJI_COMPOUNDS)
 * into a new DtSrLaJPN language.  Old version now DtSrLaJPN2.
 *
 * Revision 2.3  1995/12/01  16:20:17  miker
 * Changed read_jchar arg to unsigned to fix Solaris bug.
 *
 * Revision 2.2  1995/10/26  15:08:31  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  20:57:13  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.1  1995/09/19  21:24:57  miker
 * Initial revision
 */
#include "SearchP.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define PROGNAME	"JPN"
#define SS2_CHAR	0x8E	/* Single Shift char for Code Set 2 */
#define SS3_CHAR	0x8F	/* Single Shift char for Code Set 3 */
#define EXT_KATAKANA	".ktk"
#define EXT_KANJI	".knj"
#define SUBSTRBUFSZ	100
#define MS_misc		1
#define MS_lang		15

/* In addition to single kanji chars parsed as individual words,
 * Language DtSrLaJPN will also blindly consider all contiguous kanji
 * substrings up to MAX_KANJI_CLEN as separate compound words. 
 * For example if MAX_KANJI_CLEN were 3, the 4 kanjis "ABCD"
 * would parse as "A B C D AB BC CD ABC BCD".
 * The number of parsed words = the number of
 * ordered permutations of n things taken r! times!
 * This is can be very wasteful of indexing time and file space.
 * The alternative is language DtSrLaJPN2 which only considers
 * strings listed in jpn.knj as valid kanji compounds.
 * The kanji compounds in jpn.knj are the statistically significant
 * kanji substrings found in a large corpus of natural language Japanese.
 */
#define MAX_KANJI_CLEN	6

/************************************************/
/*						*/
/*		      JSTATE			*/
/*						*/
/************************************************/
/* EUC text substring types.
 * Used to switch states in parser's automaton.
 * Coded as bit positions for efficient boolean comparisons.
 */
#define JS_STX		0x0001	/* Start of text blk, initial state */
#define JS_KANJI	0x0002	/* Set 1, Set 3 */
#define JS_KATAKANA	0x0004	/* Set 1 */
#define JS_ASCII	0x0008	/* Set 0 */
#define JS_ROMAN	0x0010	/* Set 1 */
#define JS_GREEK	0x0020	/* Set 1, Set 3 */
#define JS_CYRILLIC	0x0040	/* Set 1 */
#define JS_ALPHA	0x0080	/* Set 3 */
#define JS_HALFKATA	0x0100	/* Set 2 */
#define JS_DISCARD	0x0200	/* Set 1, Set 3, any char not in EUC */
#define JS_ETX		0x0300	/* End of text block */
#define JS_ALPHA_COMPATIBLE  (JS_ROMAN | JS_GREEK | JS_CYRILLIC)

/************************************************/
/*						*/
/*		     JPNTREE			*/
/*						*/
/************************************************/
/* Similar to standard binary WORDTREE.
 * Each tree node distinguished by first 4 bytes
 * (usually 2 jchars), which is minimum compound word size.
 * All compounds beginning with those 4 bytes are chained
 * in a linked list off of that node.
 */
typedef struct _jpntree_tag {
    struct _jpntree_tag  *rlink;  /* ptr to right binary node */
    struct _jpntree_tag  *llink;  /* ptr to left binary node */
    struct _jpntree_tag  *next;   /* ptr to next compound in linked list */
    int                  len;     /* length of word in bytes */
    void                 *word;
    } JPNTREE;

/************************************************/
/*						*/
/*		      JPNBLK			*/
/*						*/
/************************************************/
typedef struct {
	JPNTREE	*katatree;
	JPNTREE	*kanjitree;
	}	JPNBLK;

/************************************************/
/*						*/
/*		      GLOBALS			*/
/*						*/
/************************************************/
int		debugging_jpn =		FALSE;
extern int	debugging_loadlang;
extern int	debugging_loadword;

/* Used in jpn_parser() and parse_substr().  Made global for speed. */
static int	do_compounding =	FALSE;
static int	is_new_substring =	TRUE;
static int	jstate, last_jstate;
static UCHAR	jchar [8];
static int	jcharlen =		0;
static DBLK	*jpn_dblk;
static JPNTREE	*jpn_kanjitree =	NULL;
static JPNTREE	*jpn_katatree =		NULL;
static JPNTREE	*kanjitree =		NULL;
static int	language;
static long	*offsetp;
static long	readcount =		0;
static READCFP	readchar;
static void	*readchar_arg;
static UCHAR	*outbuf =		NULL;
static UCHAR	*save_parg_string =	NULL;
static UCHAR	*substrbuf =		NULL;
static long	substr_offset;

char *ensure_end_slash (char *pathstr);

/************************************************/
/*						*/
/*                display_jstate		*/
/*						*/
/************************************************/
/* for debugging and error msgs */
static char	*display_jstate (int js)
{
    switch (js) {
	case JS_KANJI:		return "KANJI";
	case JS_KATAKANA:	return "KATAKANA";
	case JS_DISCARD:	return "DISCARD";
	case JS_ROMAN:		return "ROMAN";
	case JS_ASCII:		return "ASCII";
	case JS_ALPHA:		return "ALPHA";
	case JS_ETX:		return "ETX";
	case JS_STX:		return "STX";
	case JS_GREEK:		return "GREEK";
	case JS_CYRILLIC:	return "CYRILLIC";
	case JS_HALFKATA:	return "HALFKATA";
	default:		return "(UNKNOWN)";
    }
} /* display_jstate() */


/************************************************/
/*						*/
/*                  read_jchar			*/
/*						*/
/************************************************/
/* Subroutine of jpn_parser().
 * Using global character reading 'readchar' cofunction,
 * returns (1) next multibyte Japanese character in global jchar,
 * (2) length of jchar in global jcharlen, and
 * (3) next state of state machine in global jstate.
 * Function itself returns jstate.
 * Rows in the KUTEN tables which are officially 'unassigned'
 * are treated as user-defined kanji, so all jstates
 * are presumed JS_KANJI except those specifically marked
 * otherwise at the beginning of each array below.
 */
static int	read_jchar (void)
{
    /* Jstates table for EUC Set 1 (JIS 0208) */
    static int  jstates_set1 [] = {
	JS_DISCARD,	JS_DISCARD,	JS_DISCARD,	/* 0 - 2 */
	JS_ROMAN,	JS_DISCARD,	JS_KATAKANA,	/* 3 - 5 */
	JS_GREEK,	JS_CYRILLIC,	JS_DISCARD	/* 6 - 8 */
    };

    /* Jstates table for EUC Set 3 (JIS 0212).
     * Row 5 is presumed to be katakana because
     * of four new unapproved katakana characters.
     */
    static int  jstates_set3 [] = {
	JS_DISCARD,	JS_DISCARD,	JS_DISCARD,	/* 0 - 2 */
	JS_DISCARD,	JS_DISCARD,	JS_KATAKANA,	/* 3 - 5 */
	JS_GREEK,	JS_CYRILLIC,	JS_DISCARD,	/* 6 - 8 */
	JS_ALPHA,	JS_ALPHA,	JS_ALPHA	/* 9 - 11 */
    };

    if (readchar_arg) {
	jchar[0] = readchar (readchar_arg);
	readchar_arg = NULL;
    }
    else
	jchar[0] = readchar (NULL);
    if (jchar[0] == 0)
	return (jstate = JS_ETX);
    readcount++;

    /* Set 1 (JIS 0208) */
    if (jchar[0] >= 0xA1 && jchar[0] <= 0xFE) {
	jcharlen = 2;
	if (jchar[0] > 0xA8)
	    jstate = JS_KANJI;
	else
	    jstate = jstates_set1 [(jchar[0] & 0x7F) - 32];
	if ((jchar[1] = readchar (NULL)))
	    readcount++;
	else
	    jstate = JS_ETX;
	return jstate;
    } 

    /* Set 0 (ASCII) */
    if (jchar[0] < 0x80) {
	jcharlen = 1;
	return (jstate = JS_ASCII);
    }

    /* Set 3 (JIS 0212) */
    if (jchar[0] == SS3_CHAR) {
	jcharlen = 3;
	/*
	 * Hop over the single shift char to get the first JIS byte.
	 * Make sure first JIS byte is in proper
	 * range to avoid indexing outside of table.
	 */
	if ((jchar[1] = readchar (NULL)) == 0)
	    return (jstate = JS_ETX);
	readcount++;
	if (jchar[1] < 0xA1)
	    return (jstate = JS_DISCARD);
	if (jchar[1] > 0xAA)
	    jstate = JS_KANJI;
	else
	    jstate = jstates_set3 [(*jchar & 0x7F) - 32];

	if ((jchar[2] = readchar (NULL)) == 0)
	    return (jstate = JS_ETX);
	readcount++;
	/* JS_ALPHA chars ('miscellaneous alphabetic chars' of
	 * rows 9 - 11) are compatible with several other jstates,
	 * so adjust as necessary.
	 */
	if (jstate == JS_ALPHA  &&
		((last_jstate & JS_ALPHA_COMPATIBLE) != 0))
	    jstate = last_jstate;
	else if (last_jstate == JS_ALPHA  &&
		((jstate & JS_ALPHA_COMPATIBLE) != 0))
	    last_jstate = jstate;
	return jstate;
    }

    /* Set 2 (half-width katakana) */
    if (jchar[0] == SS2_CHAR) {
	jcharlen = 2;
	jstate = JS_HALFKATA;
	if ((jchar[1] = readchar (NULL)))
	    readcount++;
	else
	    jstate = JS_ETX;
	return jstate;
    }

    /* If first jchar doesn't match expected EUC coding,
     * discard it until we get back into sync.
     */
    jcharlen = 1;
    return (jstate = JS_DISCARD);
} /* read_jchar() */


/************************************************/
/*						*/
/*		 kanji_compounder		*/
/*						*/
/************************************************/
/* Subroutine of parse_substring() of jpn_parser().
 * Used only for language DtSrLaJPN (power series compounding).
 * Called repeatedly when the substring is a sequence of kanji chars.
 * For each call writes to outbuf and returns a single kanji
 * compound word, using every possible compound in the substring
 * from length 1 to length MAX_KANJI_CLEN.
 * Updates offsetp for each word returned.
 * Returns NULL when substring exhausted.  First call for
 * a new substring indicated by global is_new_substring.
 */

static UCHAR	*kanji_compounder (void)
{
    static int		all_done =	TRUE;
    static int		clen =		MAX_KANJI_CLEN + 1;
    static UCHAR	*mysubstrp =	NULL;
    static UCHAR	*mysubstrend =	NULL;
    static UCHAR	*op, *ss;
    static int		i;

    if (is_new_substring) {
	is_new_substring = FALSE;
	all_done = FALSE;
	clen = 1;
	mysubstrp = substrbuf;
	mysubstrend = substrbuf + strlen ((char*)substrbuf);
    }

    /* Advance compound length by 1.
     * If max compound length exceeded, reset it
     * to 1 and increment substring pointer by 1 jchar.
     */
    else {
	if (all_done)
	    return NULL;
	if (++clen > MAX_KANJI_CLEN) {
	    clen = 1;
	    mysubstrp += (*mysubstrp == SS3_CHAR)? 3 : 2;
	}
    }

    /* Assemble one word into outbuf, of length clen,
     * beginning at current substring ptr.
     * If there aren't enough jchars left in string,
     * reset clen to 1 and advance substrp by 1 jchar.
     * We're all done when substring exhausted.
     */
    while (mysubstrp < mysubstrend) {
	op = outbuf;
	ss = mysubstrp;
	for (i = 0;  i < clen;  i++) {
	    /* Are there enough jchars left in substring? */
	    if (ss >= mysubstrend) {
		clen = 1;
		mysubstrp += (*mysubstrp == SS3_CHAR)? 3 : 2;
		i = 0;		/* indicates assembly failure */
		break;		/* breaks the for loop */
	    }
	    /* Assemble one jchar into outbuf */
	    if (*ss == SS3_CHAR)
		*op++ = *ss++;
	    *op++ = *ss++;
	    *op++ = *ss++;
	}
	/* Did word assembly succeed? */
	if (i >= clen) {
	    *op = 0;
	    if (offsetp)
		*offsetp = substr_offset + (mysubstrp - substrbuf);
	    if (debugging_jpn)
		fprintf (aa_stderr,
		    "knjcompdr: subofs=%2ld totofs=%3ld \"%s\"\n",
		    (long) (mysubstrp - substrbuf), *offsetp, outbuf);
	    return outbuf;
	}
    }

    all_done = TRUE;
    return NULL;
} /* kanji_compounder() */


/************************************************/
/*						*/
/*		search_kanjitree		*/
/*						*/
/************************************************/
/* Subroutine of parse_substring() of jpn_parser().
 * Used only for language DtSrLaJPN2; DtSrLaJPN calls
 * kanji_compounder() to generate compounds algorithmically.
 * First call for a new substring of kanjis is indicated
 * when is_new_substring is TRUE.  Each call, then and thereafter,
 * returns a token (1) for each individual kanji char in string,
 * and (2) for each sequence of kanjis found in the kanji
 * compounds JPNTREE which begins with each char in string.
 * Also returns offset of returned token in offsetp.
 * Returns NULL when string is exhausted.
 * Variables are static for speeeeed.
 */
static UCHAR	*search_kanjitree (void)
{
    static int		all_done =	TRUE;
    static JPNTREE	*node, *last_node;
    static UCHAR	*substrp, *substrend;
    static int     	direction;
    static int		jcharlen;

    if (is_new_substring) {
	is_new_substring = FALSE;
	all_done = FALSE;
	substrend = substrbuf + strlen ((char*)substrbuf);
	substrp = substrbuf;

	/* Return first substr jchar as next token */
	last_node = NULL;	/* NULL = tree not searched yet */
	jcharlen = (*substrp == SS3_CHAR)? 3 : 2;
	strncpy ((char*)outbuf, (char*)substrp, jcharlen);
	outbuf [jcharlen] = 0;
	if (offsetp)
	    *offsetp = substr_offset;
	return outbuf;
    }
    else if (all_done)
	return NULL;

    /* If not enough chars left in substring to search tree,
     * treat it as an exhausted tree search.  In other words,
     * reset tree search, increment to next jchar, and return it.
     */
    if (strlen ((char*)substrp) < 4) {
	if (debugging_jpn)
	    fputs ("knjtree: ...remaining substring too short", aa_stderr);
EXHAUSTED_TREE:
	if (debugging_jpn)
	    fputs (".\n", aa_stderr);
	last_node = NULL;
	substrp += jcharlen;
	if (substrp >= substrend) {
	    all_done = TRUE;
	    return NULL;
	}
	jcharlen = (*substrp == SS3_CHAR)? 3 : 2;
	strncpy ((char*)outbuf, (char*)substrp, jcharlen);
	outbuf [jcharlen] = 0;
	if (offsetp)
	    *offsetp = substr_offset + (substrp - substrbuf);
	return outbuf;
    }

    /* If last call resulted in a tree hit, the node was saved.
     * Continue the linked list search directly from the last hit.
     */
    if (last_node) {
	last_node = last_node->next;
	if (debugging_jpn)
	    fputs ("knjtree: ...continue tree search: ", aa_stderr);
LINKED_LIST_SEARCH:
	for (node = last_node;  node;  node = node->next) {
	    if ((strncmp ((char*)substrp, node->word, node->len)) == 0) {
		/* HIT on linked list search */
		last_node = node;
		strcpy ((char*)outbuf, node->word);
		if (debugging_jpn)
		    fprintf (aa_stderr, "* '%s'\n", outbuf);
		if (offsetp)
		    *offsetp = substr_offset + (substrp - substrbuf);
		return outbuf;
	    }
	    else if (debugging_jpn)
		fputc ('-', aa_stderr);
	}
	goto EXHAUSTED_TREE;
    }

    /* Start new binary tree search at curr jchar.
     * If hit, commence linked list search.
     */
    if (debugging_jpn)
	fprintf (aa_stderr,
	    "knjtree: \"%.4s...\" ", substrp);
    for (node = kanjitree;  node != NULL;  ) {
	if ((direction = strncmp ((char*)substrp, node->word, 4)) == 0) {
	    /* HIT on binary search */
	    last_node = node;
	    goto LINKED_LIST_SEARCH;
	}
	/* Descend left or right depending on word */
	if (debugging_jpn)
	    fputc ((direction < 0) ? 'L' : 'R', aa_stderr);
	if (direction < 0)
	    node = node->llink;
	else
	    node = node->rlink;
    }

    /* No match on first 4 bytes of substrp in binary tree.
     * Tree exhausted without a hit, so increment to next
     * jchar in substring and return it as a word.
     */
    goto EXHAUSTED_TREE;
}  /* search_kanjitree() */


/************************************************/
/*						*/
/*		  parse_substring		*/
/*						*/
/************************************************/
/* Subroutine of jpn_parser().
 * Returns next Japanese multibyte word token from current
 * substring of jchars, or NULL when out of tokens.
 * Returned token is valid until next call.
 * Static args initialized at first call for a new substring.
 * Provides optional kanji compounding depending on PA_  flags.
 * We usually compound at index time (dtsrindex) or when query
 * is Query-By-Example (statistical searches), and usually don't
 * compound boolean queries.
 */
static UCHAR	*parse_substring (void)
{
    static int	is_substr_end =		TRUE;
    static int	substrlen =		0;
    static PARG	myparg;
    static UCHAR	*token;
    static long	myoffset;

    if (is_new_substring) {
	substrlen = strlen ((char*)substrbuf);

	/* A very common ascii substring is the final line-feed
	 * at the end of a line of text--discard it now.
	 */
	if (last_jstate == JS_ASCII
		&&  substrlen == 1
		&&  substrbuf[0] == '\n') {
	    is_substr_end = TRUE;
	    is_new_substring = FALSE;
	    return NULL;
	}

	is_substr_end = FALSE;

	if (!outbuf)
	    outbuf = austext_malloc (DtSrMAXWIDTH_HWORD + 8,
		PROGNAME"807", NULL);

	if (debugging_jpn) {
	    int		i;
	    fprintf (aa_stderr, "jpnsubstr: js=%s len=%ld str='",
		display_jstate(last_jstate), (long) substrlen);
	    for (i = 0;  i < substrlen;  i++)
		fputc ((substrbuf[i] < 32)? '~' : substrbuf[i],
		    aa_stderr);
	    fprintf (aa_stderr, "'\n");
	    if (last_jstate == JS_ROMAN) {
		fprintf (aa_stderr, "  (ascii equiv: '");
		for (i = 1;  i < substrlen;  i+=2)
		    fputc ((substrbuf[i] & 0x7f) + 32, aa_stderr);
		fprintf (aa_stderr, "')\n");
	    }
	    fflush (aa_stderr);
	}

    } /* endif is_new_substring */

    if (is_substr_end)
	return NULL;

    switch (last_jstate) {

	case JS_DISCARD:
	    /* Ignore discardable substrings */
	    is_new_substring = FALSE;
	    is_substr_end = TRUE;
	    return NULL;

	case JS_KATAKANA:
	case JS_ROMAN:
	case JS_CYRILLIC:
	case JS_GREEK:
	case JS_ALPHA:
	case JS_HALFKATA:
	    /* Treat entire substring as single parsed word */
ENTIRE_SUBSTR_IS_WORD:
	    if (debugging_jpn)
		fputs ("  token is entire substring.\n", aa_stderr);
	    strncpy ((char*)outbuf, (char*)substrbuf, DtSrMAXWIDTH_HWORD);
	    outbuf [DtSrMAXWIDTH_HWORD - 1] = 0;
	    is_new_substring = FALSE;
	    is_substr_end = TRUE;
	    if (offsetp)
		*offsetp = substr_offset;
	    return outbuf;

	case JS_ASCII:
	    /* Call the full teskey_parser() for European languages.
	     * Includes stoplist and include list processing.
	     */
	    if (is_new_substring) {
		is_new_substring = FALSE;
		if (debugging_jpn)
		    fputs ("  calling teskey parser.\n", aa_stderr);
		myparg.dblk =		jpn_dblk;
		myparg.string =		substrbuf;
		myparg.ftext =		NULL;
		myparg.offsetp =	&myoffset;
		token = (UCHAR *) teskey_parser (&myparg);
	    }
	    else
		token = (UCHAR *) teskey_parser (NULL);
	    if (token) {
		if (offsetp)
		    *offsetp = substr_offset + myoffset;
	    }
	    else
		is_substr_end = TRUE;
	    return token;

	case JS_KANJI:
	    /* If not compounding, treat entire substring
	     * as one query word, ie a single compound kanji word.
	     * If compounding, each individual kanji in the
	     * substring is returned as a word by itself.
	     * Each kanji can be 2 or 3 bytes depending on
	     * which code set it came from.  In addition,
	     * sequences of 2 or more kanjis ('compound kanji
	     * words') are returned as individual words.
	     * Method of kanji compounding depends on language:
	     * DtSrLaJPN does "power series" kanji compounding,
	     * DtSrLaJPN2 looks up kanji compounds in a word tree.
	     * Both functions test and reset is_new_substring,
	     * update offsetp as necessary, and return either NULL
	     * or a pointer to outbuf containing a valid token.
	     */
	    if (!do_compounding)
		goto ENTIRE_SUBSTR_IS_WORD;
	    token = (language == DtSrLaJPN)?
		kanji_compounder() : search_kanjitree();
	    if (!token)
		is_substr_end = TRUE;
	    return token;

	default:
	    break;

    } /* end state switch */

    /* Should never get here... */
    fprintf (aa_stderr, catgets(dtsearch_catd, MS_lang, 20,
	"%s Program Error: Unknown jstate %d.\n") ,
	PROGNAME"246", last_jstate);
    DtSearchExit (46);
} /* parse_substring() */


/************************************************/
/*						*/
/*                  jpn_parser			*/
/*						*/
/************************************************/
/* Returns next word token from text stream of packed EUC
 * Japanese text, languages DtSrLaJPN and DtSrLaJPN2.
 * Called from (1) dtsrindex, where readchar_ftext() cofunction
 * reads the .fzk file document 'stream', or (2) search engine
 * query parsers, where readchar_string() cofunction 'reads'
 * from the query string.
 *
 * First call passes args in PARG block.  This resets end of
 * text block (ETX) flag, resets 'offset' counter to zero, etc.
 * Subsequent calls should pass NULL, and parser returns
 * next token in block, until reader cofunction reads ETX
 * end returns special ETX char ('\0').  Subsequent call to parser
 * returns NULL meaning "no tokens left in current stream".
 * Reader cofunction tolerates repeated calls after
 * the first ETX, still returning '\0'.
 *
 * This parser presumes all incoming text is packed EUC multibyte
 * Japanese chars as described above, but is otherwise unformatted.
 * Since parser accesses streams a multibyte char at a time,
 * it does not require periodic line feeds, etc.
 *
 * To control kanji compounding, caller should set a PA_ switch
 * in parg.flags as desired before call.  Compounding is done
 * when indexing (dtsrindex) or for hiliting (comparing previous
 * search results against all possible words in document text).
 * But in a Query by Example (stat searches), parser might also
 * be asked to  generate compound words.  In boolean queries
 * (stems and exact words), parser should not generate compounds
 * because if user enters a compound string, he probably only wants
 * documents containing that exact token.
 *
 * Parser also returns offset information: number of bytes
 * since beginning of text block.  The returned offsets are
 * NOT NECESSARILY IN ASCENDING ORDER due to kanji compounding.
 *
 * Variables are static or global for speeeeeeed.
 * 
 * OUTPUT FORMAT:  NULL or a static C string containing a
 * single parsed word token.
 * The text in the buffer is valid until the next call.
 * Each word is translated as described above.
 */
char	*jpn_parser (PARG *parg)
{
    static int		filling_substring =	TRUE;
    static int		was_discarding =	FALSE;
    static int		add_msgs = 		FALSE;
    static UCHAR	*endsubstrbuf =		NULL;
    static size_t	substrbufsz =		0;
    static UCHAR	*token;
    static UCHAR	*substrp;

    /* If first call for new text block... */
    if (parg) {
	jpn_dblk = parg->dblk;
	language = jpn_dblk->dbrec.or_language;
	kanjitree = ((JPNBLK *)(jpn_dblk->parse_extra))->kanjitree;
	offsetp = parg->offsetp;
	do_compounding = (parg->flags & (PA_HILITING | PA_INDEXING));
	add_msgs = (parg->flags & PA_MSGS);
	if (parg->string) {	/* text is query str from search engine */
	    save_parg_string = parg->string;
	    readchar_arg = parg->string;
	    readchar = (READCFP) readchar_string;
	}
	else {			/* text is from .fzk file in dtsrindex */
	    save_parg_string = NULL;
	    readchar_arg = parg;
	    readchar = (READCFP) readchar_ftext;
	}

	if (substrbufsz == 0) {
	    substrbufsz = SUBSTRBUFSZ;
	    substrbuf = austext_malloc (SUBSTRBUFSZ + 8, PROGNAME"680", NULL);
	}
	endsubstrbuf = substrbuf + substrbufsz;

	if (debugging_jpn) {
	    fprintf (aa_stderr,
		"jpnparser: start text block, substrbufsz=%ld.\n",
		(long) substrbufsz);
	    fflush (aa_stderr);
	}

	/* Seed the first substring */
	filling_substring = TRUE;
	readcount = 0L;
	last_jstate = JS_STX;
	read_jchar();

    } /* endif (parg != NULL) */

FILL_ANOTHER_SUBSTRING:
    /* Input text is presumed to contain substrings
     * of chars related by their EUC encoding.
     * Fill the substring buffer by reading in nonDISCARDable
     * multibyte jchars until jstate changes signaling
     * end of a substring.
     * Note last jchar read, the one that changes the jstate,
     * hangs around till we come back to this loop.
     */
    if (filling_substring) {
	if (debugging_jpn) {
	    if (jstate == JS_DISCARD) {
		fputs ("jpnparser: js=DISCARD:", aa_stderr);
		was_discarding = TRUE;
	    }
	    else
		was_discarding = FALSE;
	}
	while (jstate == JS_DISCARD) {
	    if (debugging_jpn)
		fprintf (aa_stderr, " %s", jchar);
	    read_jchar();
	}
	if (debugging_jpn && was_discarding)
	    fputc ('\n', aa_stderr);
	if (jstate == JS_ETX) {
	    if (debugging_jpn)
		fputs ("jpnparser: js=ETX\n", aa_stderr);
            if (add_msgs) {
                char    msgbuf [DtSrMAXWIDTH_HWORD + 100];
                sprintf (msgbuf, catgets(dtsearch_catd, MS_lang, 21,
		    "%s '%.*s' is not a valid Japanese word.") ,
                    PROGNAME"812", DtSrMAXWIDTH_HWORD, save_parg_string);
                DtSearchAddMessage (msgbuf);
            }
	    return NULL;
	}

	last_jstate = jstate;
	substrp = substrbuf;
	substr_offset = readcount - jcharlen;

	/* Fill the substring buffer.
	 * Ensure substring buffer is big enough.
	 */
	while (last_jstate == jstate) {
	    if (endsubstrbuf - substrp < 8) {
		size_t	curlen = substrp - substrbuf;
		if (debugging_jpn) {
		    fprintf (aa_stderr,
			"jpnparser: curr substr len %lu, "
			"new substrbufsz %lu.\n",
			(unsigned long) curlen, (unsigned long) substrbufsz<<1);
		    fflush (aa_stderr);
		}
		substrbufsz <<= 1;	/* double its size */
		substrbuf = realloc (substrbuf, substrbufsz);
		endsubstrbuf = substrbuf + substrbufsz;
		substrp = substrbuf + curlen;
	    }
	    strncpy ((char*)substrp, (char*)jchar, jcharlen);
	    substrp += jcharlen;
	    read_jchar();
	}
	*substrp = 0;
	filling_substring = FALSE;
	is_new_substring = TRUE;
    }

    /* Empty the substring buffer returning each token
     * one by one, ie parse and return word tokens from string,
     * including possible kanji compounds if switched on.
     */
    if ((token = parse_substring()))
	return (char *) token;

    /* When current substring is empty, go back and fill another one.
     * If we're parsing a string (eg hiliting text of a doc),
     * parse_substring() will have used readchar_string().
     * Since we now want to resume using it to parse the original
     * string, we have to reset it's string ptr.
     */
    filling_substring = TRUE;
    if (save_parg_string)
	readchar_arg = save_parg_string + readcount;
    goto FILL_ANOTHER_SUBSTRING;

} /* jpn_parser() */


/************************************************/
/*						*/
/*                load_jpntree			*/
/*						*/
/************************************************/
/* Subroutine of load_jpn_language.  Builds a JPNTREE
 * from a file of packed EUC compound words.
 * Basically a copy of load_wordtree() in lang.c.
 *
 * INPUT FILE FORMAT:  One word per line, min 4 bytes (2 jchars),
 * all words packed EUC.  Preferred order is frequency of
 * occurrence in the corpus to make searches efficient.
 * Otherwise the words should at least be in random order or
 * an order that will approximate a binary search.
 * If first char is ASCII (ie not packed EUC), line is
 * ignored as comments.  Any ascii chars after packed EUC,
 * such as whitespace and/or subsequent ascii comments,
 * delimits word token (ie anything else on the line is ignored).
 * "Line" ends in ascii linefeed (\n).
 *
 * RETURNS 0 if file successfully loaded, returns 1 if file missing,
 * returns 2 and messages in global msglist if file has fatal errors.
 */
static int	load_jpntree (
		    JPNTREE	**treetop,
		    char	*fname)
{
    int		i;
    int		comment_count = 0;
    int		node_count = 0;
    int		is_duplicate;
    long	linecount = 0;
    UCHAR	*cptr;
    UCHAR	readbuf [256];
    char	sprintbuf [_POSIX_PATH_MAX + 1024];
    FILE	*fileid;
    JPNTREE	*new;
    JPNTREE	**this_link;

    if (debugging_loadlang | debugging_loadword)
	fprintf (aa_stderr, PROGNAME"1071 "
	    "load_jpntree: fname='%s'\n", NULLORSTR(fname));

    if ((fileid = fopen (fname, "rt")) == NULL) {
	/* Not being able to find the file is not an error.
	 * We indicate that with the return code.
	 * But any other error (like permissions) is fatal.
 	 */
	if (errno == ENOENT) {
	    if (debugging_loadlang | debugging_loadword)
		fputs ("  ...file not found.\n", aa_stderr);
	    return 1;
	}
	else {
	    sprintf (sprintbuf,
		catgets (dtsearch_catd, MS_misc, 362, "%s: %s: %s."),
		PROGNAME"362", fname, strerror(errno));
	    DtSearchAddMessage (sprintbuf);
	    return 2;
	}
    }

    /*--------- Main Read Loop ----------*/
    while (fgets ((char*)readbuf, sizeof(readbuf), fileid) != NULL) {
	linecount++;
	/*
	 * Ignore lines beginning with any ascii char (comments).
	 * Otherwise first or only packed EUC token on line
	 * is the desired word. 
	 */
	if (readbuf[0] < 0x80) {
	    comment_count++;
	    continue;
	}
	for (cptr = readbuf;  *cptr >= 0x80;  cptr++)
	    ;
	*cptr = 0;
	if (debugging_loadword) {
	    fprintf (aa_stderr, "  JPNWORD: '%s' %n", readbuf, &i);
	    while (i++ < 28)
		fputc (' ', aa_stderr);
	}
	    
	/* Test for word too short */
	if (strlen((char*)readbuf) < 4) {
	    sprintf (sprintbuf, catgets(dtsearch_catd, MS_lang, 23,
		"%s Word '%s' on line %ld is too short.") ,
		PROGNAME"1074", readbuf, linecount);
	    DtSearchAddMessage (sprintbuf);
	    continue;
	}

	/* Allocate and populate a new node */
	i = strlen ((char*) readbuf);
	new = austext_malloc (sizeof(JPNTREE) + i + 4,
	    PROGNAME"104", NULL);
	new->llink = NULL;
	new->rlink = NULL;
	new->next = NULL;
	new->len = i;
	new->word = (void *) (new + 1);
	strcpy (new->word, (char *) readbuf);

	/* Search binary tree, comparing only first 4 bytes */
	is_duplicate = FALSE;
	for (this_link = treetop;  *this_link != NULL;  ) {
	    i = strncmp (new->word, (*this_link)->word, 4);

	    if (i == 0) {
		/* If first 4 bytes are similar, search
		 * linked list, comparing entire string.
		 */
		while (*this_link != NULL) {
		    i = strcmp (new->word, (*this_link)->word);

		    /* Test for duplicate word */
		    if (i == 0) {
			sprintf (sprintbuf,
			    catgets (dtsearch_catd, MS_misc, 423,
			    "%s Word '%s' in '%s' is a duplicate."),
			    PROGNAME"423", readbuf, fname);
			DtSearchAddMessage (sprintbuf);
			/* duplicates aren't fatal, just ignore the word */
			is_duplicate = TRUE;
			break;	/* discontinue list search */
		    }
		    if (debugging_loadword)
			fputc('-', aa_stderr);
		    this_link =  &(*this_link)->next;
		} /* end linked list search */

		break;		/* discontinue tree search */
	    } /* endif where first 4 bytes matched at a tree node */

	    /* First 4 bytes dissimilar.  Descend tree
	     * to find next possible insertion point.
	     */
	    if (debugging_loadword)
		fputc(((i < 0)? 'L' : 'R'), aa_stderr);
	    this_link = (JPNTREE **) ((i < 0) ?
		&(*this_link)->llink : &(*this_link)->rlink);
	} /* end binary tree search */

	/* Don't link anything if error found while descending tree */
	if (is_duplicate) {
	    if (debugging_loadword)
		fputs (" duplicate!\n", aa_stderr);
	    free (new);
	    continue;
	}

	/* Insert new node at current location in tree */
	*this_link = new;
	if (debugging_loadword)
	    fputs(".\n", aa_stderr);
	node_count++;
    }	/* end of read loop */

    fclose (fileid);

    if (node_count <= 0) {
	if (debugging_loadlang | debugging_loadword)
	    fprintf (aa_stderr,
		PROGNAME"1185 load '%s' unsuccessful, %d comments discarded.\n",
		fname, comment_count);
	sprintf (sprintbuf, catgets(dtsearch_catd, MS_lang, 24,
	    "%s No Japanese words in word file '%s'.") ,
	    PROGNAME"1186", fname);
	DtSearchAddMessage (sprintbuf);
	return 2;
    }
    else {
	if (debugging_loadlang | debugging_loadword)
	    fprintf (aa_stderr,
		PROGNAME"1193 load word file '%s' successful, %d words.\n",
		fname, node_count);
	return 0;
    }
}  /* load_jpntree() */


/************************************************/
/*						*/
/*              load_jpn_language		*/
/*						*/
/************************************************/
/* Loads a dblk with japanese (DtSrLaJPN, DtSrLaJPN2)
 * structures and function pointers.
 * Called from load_language(), with identical input and output.
 * Does not reload structures previously loaded in
 * other jpn dblks on dblist if derived from identical files.
 * But always loads structures if passed dblist is NULL.
 * Presumes dblk already partially initialized:
 *	name, path, language, flags.
 * Returns TRUE if all successful.  Otherwise
 * returns FALSE with err msgs on ausapi_msglist.
 */
int      load_jpn_language (DBLK *dblk, DBLK *dblist)
{
    extern int	ascii_charmap[];	/* in lang.c */
    int		i;
    int		errcount = 0;
    JPNBLK	*jpnblk;
    char	fname [_POSIX_PATH_MAX + 4];
    char	path [_POSIX_PATH_MAX + 4];
    char	msgbuf [_POSIX_PATH_MAX + 128];

    dblk->charmap =	ascii_charmap;	/* for teskey */
    dblk->parser =	jpn_parser;
    dblk->lstrupr =	null_lstrupr;
    dblk->stemmer =	null_stemmer;
    if (dblk->dbrec.or_maxwordsz == 0)	/* for teskey */
        dblk->dbrec.or_maxwordsz = MAXWIDTH_SWORD - 1;
    if (dblk->dbrec.or_minwordsz == 0)	/* for teskey */
        dblk->dbrec.or_minwordsz = MINWIDTH_TOKEN + 1;
    jpnblk = austext_malloc (sizeof(JPNBLK) + 4, PROGNAME"2107", NULL);
    memset (jpnblk, 0, sizeof(JPNBLK));
    dblk->parse_extra = (void *) jpnblk;

    /* Load optional katakana and kanji word lists.
     * If specific dblk version not found,
     * try the default language version.
     * If either has load errors, return a failure.
     * If both are missing, just forget it.
     */
    if (dblk->path == NULL)
	path[0] = 0;
    else {
	if (strlen (dblk->path) > _POSIX_PATH_MAX - 14) {
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_lang, 25,
		"%s Database '%s' path too long: '%s'.") ,
		PROGNAME"759", dblk->name, dblk->path);
	    DtSearchAddMessage (msgbuf);
	    return FALSE;
	}
	strcpy (path, dblk->path);
	ensure_end_slash (path);
    }

#ifdef NO_KATAKANA_TREES_YET
    /* Load katakana wordtree */
    strcpy (fname, path);
    strcat (fname, dblk->name);
    strcat (fname, EXT_KATAKANA);
    i = load_jpntree (&jpnblk->katatree, fname);
    if (i == 1) { 	/* ...db specific file not found */
	if (jpn_katatree == NULL) {	/* load default... */
	    strcpy (fname, path);
	    strcat (fname, "jpn");
	    strcat (fname, EXT_KATAKANA);
	    i = load_jpntree (&jpn_katatree, fname);
	}
	else		/* default already loaded */
	    i == 0;
	jpnblk->katatree = jpn_katatree;
    }
    if (i > 1)
	errcount++;
#endif /* NO_KATAKANA_TREES_YET */

    /* Load kanji wordtree only if kanji compounds are derived
     * from list in file, ie for language DtSrLaJPN2 only.
     * If database specific list not found,
     * use language generic list.  If language generic
     * list also not found, ignore compounding.
     * Only one language generic list will
     * be loaded, at jpn_kanjitree.
     */
    if (dblk->dbrec.or_language == DtSrLaJPN2) {
	strcpy (fname, path);
	strcat (fname, dblk->name);
	strcat (fname, EXT_KANJI);
	i = load_jpntree (&jpnblk->kanjitree, fname);
	if (i == 1) { 	/* ...db specific file not found */
	    /* If the generic knj file (jpn.knj) was
	     * never loaded, try loading it now.
	     */
	    if (jpn_kanjitree == NULL) {
		strcpy (fname, path);
		strcat (fname, "jpn");
		strcat (fname, EXT_KANJI);
		load_jpntree (&jpn_kanjitree, fname);
		/* (it either worked or it didn't) */
	    }
	    /* Whether generic load successful or not,
	     * try to use it (eg it might still be NULL).
	     */
	    jpnblk->kanjitree = jpn_kanjitree;
	}
	if (i > 1)  /* error trying to open db specific file */
	    errcount++;
    }

    return (errcount > 0)? FALSE : TRUE;

} /* load_jpn_language() */


/************************************************/
/*						*/
/*		   free_jpntree			*/
/*						*/
/************************************************/
/* Identical to free_wordtree() in lang.c
 * (link inversion traversal, from Data Structure Techniques,
 * Thomas A. Standish, Algorithm 3.6),
 * except post order visit includes freeing
 * linked list at each tree node.
 */
static void	free_jpntree (JPNTREE ** jpntree_head)
{
    JPNTREE	*next, *prev, *pres;
    JPNTREE	*listp, *next_listp;

    if (*jpntree_head == NULL)
	return;
    pres = *jpntree_head;
    prev = NULL;

DESCEND_LEFT:
    pres->word = (void *) 0;	/* preorder visit:  TAG = 0 */
    next = pres->llink;
    if (next != NULL) {
	pres->llink = prev;
	prev = pres;
	pres = next;
	goto DESCEND_LEFT;
    }
DESCEND_RIGHT:
     next = pres->rlink;
    if (next != NULL) {
	pres->word = (void *) 1;	/* TAG = 1 */
	pres->rlink = prev;
	prev = pres;
	pres = next;
	goto DESCEND_LEFT;
    }
POSTORDER_VISIT:
    listp = pres;
    while (listp->next) {
	next_listp = listp->next;
	free (listp);
	listp = next_listp;
    }
    free (listp);

    if (prev == NULL) {			/* end of algorithm? */
	*jpntree_head = NULL;
	return;
    }
    if (prev->word == (void *) 0) {	/* go up left leg */
	next = prev->llink;
	pres = prev;
	prev = next;
	goto DESCEND_RIGHT;
    }
    else {				/* go up right leg */
	next = prev->rlink;
	prev->word = (void *) 0;	/* restore TAG = 0 */
	pres = prev;
	prev = next;
	goto POSTORDER_VISIT;
    }
}  /* free_jpntree() */


/************************************************/
/*						*/
/*             unload_jpn_language		*/
/*						*/
/************************************************/
/* Frees storage for structures allocated by load_jpn_language().
 * Called when engine REINITs due to change in site config file
 * or databases.
 * The global jpntrees are not currently unloaded because they
 * are presumed valid for the duration of the engine session.
 * Currently there are no teskey trees (inclist, stoplist) to free.
 */
void	unload_jpn_language (DBLK *dblk)
{
    /* free jpnblk and any database-associated jpntrees */
    if (dblk->parse_extra) {
	JPNBLK	*jpnblk = (JPNBLK *) dblk->parse_extra;
	if (jpnblk->katatree  &&  jpnblk->katatree != jpn_katatree)
	    free_jpntree (&jpnblk->katatree);
	if (jpnblk->kanjitree &&  jpnblk->kanjitree != jpn_kanjitree)
	    free_jpntree (&jpnblk->kanjitree);
	free (jpnblk);
	dblk->parse_extra = NULL;
    }
    return;
} /* unload_jpn_language() */

/******************** JPN.C ********************/

