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
 *   FUNCTIONS: euro_lstrupr
 *		free_wordtree
 *		is_concordable
 *		language_name
 *		load_include_list
 *		load_language
 *		load_paice_suffixes
 *		load_stop_list
 *		load_wordtree
 *		null_lstrupr
 *		null_stemmer
 *		paice_stemmer
 *		search_wordtree
 *		teskey_parser
 *		unload_language
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
/******************** LANG.C ********************
 * $XConsortium: lang.c /main/11 1996/11/25 18:47:29 drk $
 * July 1995.
 * Includes load_language(), unload_language(), and functions and data for
 * parsing and stemming European languages in DtSearch/AusText.
 * Incorporates p/o socrates.c, p/o proctext.c, parser.c
 * delsfx.c, loadchr.c, stop.c, inclist.c, convneg.c, isendwrd.c
 * Related to similar semantic modules repackaged into semantic.c.
 * Paice suffix removal algorithm from C. Paice, 1990,
 * "Another Stemmer", ACM SIGIR Forum, 24(3), 56-61.
 *
 * $Log$
 * Revision 2.13  1996/03/25  18:55:26  miker
 * Changed FILENAME_MAX to _POSIX_PATH_MAX.
 *
 * Revision 2.12  1996/03/25  17:00:19  miker
 * Cleanup compiler warning.
 *
 * Revision 2.11  1996/03/13  22:58:13  miker
 * Changed char to UCHAR several places.
 *
 * Revision 2.10  1996/03/05  16:49:58  miker
 * Move COMMENT_CHARS to SearchP.h.
 *
 * Revision 2.9  1996/03/05  16:31:20  miker
 * Added test of PA_MSGS for yacc-based boolean queries.
 * Made comment chars in linguistic files independent of locale.
 * Changed several char ptrs to unsigned char so parser will
 * work when compiled under default signed char compilers.
 * Simplified several statements with LHS *var++ for same reason.
 *
 * Revision 2.8  1996/02/05  16:16:05  miker
 * Restore prolog.
 *
 * Revision 2.7  1996/02/05  16:10:54  miker
 * load_paice_suffixes: discard .sfx lines beginning with all numeric
 * first token for compatibility with older file formats.
 *
 * Revision 2.6  1996/02/01  19:11:43  miker
 * AusText 2.1.11, DtSearch 0.3:  Major rewrite for new parsers.
 * Moved charmaps to new module langmap.c.  Removed hard coded
 * paice stemmer values--now dynamic from .sfx file.
 *
 * Revision 2.5  1995/10/26  14:55:28  miker
 * Added prolog.
 *
 * Revision 2.4  1995/10/19  20:54:36  miker
 * Increased msg buf sizes to accommodate larger database file names.
 *
 * Revision 2.3  1995/10/06  14:39:45  miker
 * Bug fix: coredump loading multiple databases
 * on Solaris.
 *
 * Revision 2.2  1995/10/03  21:39:10  miker
 * Changed teskey_parser, paice_stemmer, and null_stemmer
 * to return number of words parsed/stemmed, not just boolean.
 *
 * Revision 2.1  1995/09/22  21:00:19  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/19  22:08:28  miker
 * Added support for loading and parsing Japanese language DtSrLaJPN.
 *
 * Revision 1.2  1995/09/05  21:34:52  miker
 * Fixed bug: search engine wouldn't parse words of exactly
 * 3 or 15 chars.
 *
 * Revision 1.1  1995/08/31  21:03:44  miker
 * Initial revision
 */
#include "SearchP.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define X_INCLUDE_STRING_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>

#define PROGNAME	"LANG"
#define EXT_SUFFIX	".sfx"	/* standard paice suffix file format */
#define OUTBUFSZ	6140
#define SFX_DELIMS	" \t\n"
#define MS_misc		1
#define MS_lang		15
#define IS_VOWEL(c)	((paice_charmap [(UCHAR)c] & VOWEL) != 0)

/************************************************/
/*						*/
/*		      PRULE			*/
/*						*/
/************************************************/
/* List of Paice suffix removal rules from .sfx files */
typedef struct prule_t {
    struct prule_t   *link;	/* Ptr to next list node */
    UCHAR   *suffix;		/* Applicable suffix string, backwards */
    UCHAR   suflen;		/* Length of suffix */
    char    must_be_intact;	/* Optional '*'.  Rule only applies
				 * to intact words */
    UCHAR   remove_count;	/* Number of suffix chars to remove */
    UCHAR   aplen;		/* Length of apndstr */
    UCHAR   *apndstr;		/* Optional append string */
    char    is_last_rule;	/* '$' terminate or '>' continue algorithm */
    }	PRULE;

char *ensure_end_slash (char *pathstr);
void  unload_jpn_language (DBLK *dblk);

/************************************************/
/*						*/
/*		      GLOBALS			*/
/*						*/
/************************************************/
int		debugging_loadlang =		FALSE;
int		debugging_loadword =		FALSE;
int		debugging_search_wordtree =	FALSE;
int             debugging_teskey =		FALSE;
int             debugging_paice =		FALSE;
static int	*paice_charmap;
static UCHAR	paicebuf [DtSrMAXWIDTH_HWORD + 2];
static int	paicelen;
static int	word_is_intact;

/* Language strings correspond to DtSrLa.. constants.  */
static char	*lang_fnames[] = {
			"eng",		/*  0 */
			"eng",		/*  1  ('eng2' same files as 'eng') */
			"esp",		/*  2 */
			"fra",		/*  3 */
			"ita",		/*  4 */
			"deu",		/*  5 */
			"jpn",		/*  6 */
			"jpn",		/*  7  ('jpn2' same files as 'jpn' */
			NULL
		};


/************************************************/
/*						*/
/*                language_name			*/
/*						*/
/************************************************/
/* Returns language name string given language number */
static char	*language_name (DtSrINT16 langno)
{
    static char	*language_names[] = {
		"English-ASCII",	/*  0 = DtSrLaENG */
		"English-Latin1",	/*  1 = DtSrLaENG2 */
		"Spanish",		/*  2 = DtSrLaESP */
		"French",		/*  3 = DtSrLaFRA */
		"Italian",		/*  4 = DtSrLaITA */
		"German",		/*  5 = DtSrLaDEU */
		"Japanese-comp"		/*  6 = DtSrLaJPN */
		"Japanese-.knj"		/*  7 = DtSrLaJPN2 */
		};
#if TRACY
    static char *language_names[10];

    language_names[0] = catgets(dtsearch_catd, MS_lang, 50, "English-ASCII");
    language_names[1] = catgets(dtsearch_catd, MS_lang, 51, "English-Latin1");
    language_names[1] = catgets(dtsearch_catd, MS_lang, 52, "Spanish");
    language_names[1] = catgets(dtsearch_catd, MS_lang, 53, "French");
    language_names[1] = catgets(dtsearch_catd, MS_lang, 54, "Italian");
    language_names[1] = catgets(dtsearch_catd, MS_lang, 54, "German");
    language_names[1] = catgets(dtsearch_catd, MS_lang, 54, "Japanese-comp");
    language_names[1] = catgets(dtsearch_catd, MS_lang, 54, "Japanese-.knj");

#endif
    if (langno < 0)
	return "INVALID!";
    else if (langno > DtSrLaLAST)
	return "(Custom Language)";
    else
	return language_names [langno];
} /* language_name() */


/************************************************/
/*						*/
/*		 search_wordtree		*/
/*						*/
/************************************************/
/* Sept 1991.
 * Formerly search_inclist() in inclist.c and search_stoplist() in stop.c.
 * Searches a word list in a binary WORDTREE.
 * Passed wordstring is presumed to be a clean,
 * uppercase word token string terminated by \0.
 * Variables are static for speeeeed.
 * Returns TRUE if successful search, else FALSE.
 * See also search_wordtree_jpn() in jpn.c
 */
static int	search_wordtree (WORDTREE *wordtree, UCHAR *wordstring)
{
    static int     	direction;
    static WORDTREE	*node;

    if (debugging_search_wordtree)
	fprintf (aa_stderr, PROGNAME"196 search wordtree for '%s':\n",
	    wordstring);
    /* MAIN SEARCH LOOP: binary tree search */
    for (node = wordtree;  node != NULL;  ) {
	if ((direction = strcmp ((char *) wordstring, node->word)) == 0) {
	    if (debugging_search_wordtree)
		fprintf (aa_stderr, "  HIT!\n");
	    return TRUE;
	}
	/* Descend left or right depending on word */
	if (debugging_search_wordtree)
	    fprintf (aa_stderr, "  %c '%s'\n",
		(direction < 0) ? 'L' : 'R', (char *) node->word);
	if (direction < 0)
	    node = node->llink;
	else
	    node = node->rlink;
    }
    if (debugging_search_wordtree)
	fprintf (aa_stderr, "  MISS.\n");
    return FALSE;
}  /* search_wordtree() */


/************************************************/
/*						*/
/*                 teskey_parser		*/
/*						*/
/************************************************/
/* 1989.
 * Teskey_parser() is derived from the former Socrates() in socrates.c.
 * Returns next teskey-parsed word token from a character stream.
 * Called from (1) dtsrindex, where readchar_ftext() cofunction
 * reads the .fzk file document 'stream', or (2) search engine
 * query parsers, where readchar_string() cofunction 'reads'
 * from the query string.
 * (The word hiliting parser does not directly call teskey_parser; it has
 * its own simplified equivalent to the parsing algorithms herein.)
 *
 * First call passes args in PARG structure.  This resets end of
 * text block (ETX) flag, resets 'offset' counter to zero, etc.
 * Subsequent calls should pass NULL, and parser returns
 * next token in block, until reader cofunction reads ETX,
 * ie special ETX char ('\0').  Subsequent calls to parser
 * return NULL meaning "no tokens left in current stream".  
 * Reader cofunctions tolerate repeated calls after
 * the first ETX, still returning '\0'.
 * 
 * This parser presumes all incoming text is unformatted.
 * Since parser accesses streams a char at a time it does
 * not require periodic line feeds or anything else.
 * 
 * Parser also returns offset information: number of bytes
 * since beginning of text block.
 * 
 * Variables are static for speeeeeeed.
 * 
 * OUTPUT FORMAT:  NULL or a static C string containing a single
 * parsed word token.  Word buffer reused at next call.
 * Each word is translated as follows:
 * 	All alphas TO UPPERCASE.
 *	Teskey algorithm used to find word boundaries.
 *	Always keeps include-list words.
 * 	Throws away stoplist words, very short words, and very long words.
 *	All intervening nonconcordables discarded.
 * 
 * There is a slight mod to the published Teskey algorithm.
 * Words can begin with optionally concordable chars
 * but not end with them.  For example if '-' is optionally
 * concordable, '-foo-' will be parsed into '-foo'.
 */
char	*teskey_parser (PARG *parg)
{
    static READCFP	cofunction;
    static void		*cofunction_arg;
    static DBLK		*dblk =		NULL;
    static UCHAR	*outbuf =	NULL;
    static size_t	outbufsz =	0;
    static UCHAR	*endmaxword;	/* end largest possible output word */
    static UCHAR	*outp;		/* next loc in outbuf */
    static int		*charmap;
    static int		minwordsz, maxwordsz;
    static int		wordlen;
    static enum	{BETW_WORDS, IN_WORD, TOO_LONG}
			tpstate;
    static long		*offsetp, readcount, candidate_offset;
    static int		is_hiliting;
    static int		add_msgs;

    /* If first call for current text block... */
    if (parg) {
	dblk = parg->dblk;
	minwordsz = dblk->dbrec.or_minwordsz;
	maxwordsz = dblk->dbrec.or_maxwordsz;
	charmap = dblk->charmap;
	offsetp = parg->offsetp;
	is_hiliting = (parg->flags & PA_HILITING);
	add_msgs = (parg->flags & PA_MSGS);
	if (charmap == NULL) {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_lang, 4,
		"%s dblk not initialized.\n"),
		PROGNAME"801");
	    DtSearchExit (55);
	}

	if (parg->string) {
	    cofunction_arg = parg->string;
	    cofunction = (READCFP) readchar_string;
	}
	else if (parg->ftext) {
	    cofunction_arg = parg;
	    cofunction = (READCFP) readchar_ftext;
	}
	else {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_lang, 5,
		"%s Program Error: parg contains neither file nor string.\n"),
		PROGNAME"327");
	    DtSearchExit (27);
	}

	if (outbufsz <= maxwordsz) {
	    if (outbuf)
		free (outbuf);
	    outbufsz = maxwordsz + 8;
	    outbuf = austext_malloc (outbufsz + 8, PROGNAME"807", NULL);
	}
	endmaxword = outbuf + maxwordsz;
	if (debugging_teskey)
	    fprintf (aa_stderr,
		"teskey: start of text block, maxwsz=%d outbufsz=%lu\n",
		maxwordsz, (unsigned long) outbufsz);
	readcount = 0L;
    }

    /* CANDIDATE WORD LOOP:  Read text chars into outbuf.
     * Exit loop when outbuf contains one candidate token or at ETX.
     */
READ_ANOTHER_WORD:
    outp = outbuf;
    tpstate = BETW_WORDS;
    while ((*outp = cofunction (cofunction_arg))) {
	readcount++;
	cofunction_arg = NULL;

	/*------------- BETW_WORDS State ------------
	 * Reader is between word tokens.
	 */
	if (tpstate == BETW_WORDS) {
	    /*
	     * Discard nonconcordable chars between words.
	     */
	    if ((charmap[*outp] & NON_CONCORD) != 0)
		continue;
	    /*
	     * Fully concordable char is definite start of new word.
	     * Convert to uppercase and go get next char.
	     */
	    if ((charmap[*outp] & CONCORDABLE) != 0) {
		*outp = charmap[*outp] & 0x00ff;
		outp++;
		candidate_offset = readcount;
		tpstate = IN_WORD;
		continue;
	    }
	    /*
	     * Must be optionally concordable.  It can only
	     * start a new word if next char is concordable.
	     * If so, convert a fully concordable char
	     * to uppercase and go get next char.
	     * Otherwise discard just like non_concord.
	     */
	    outp++;
	    if ((*outp = cofunction(NULL)))
		readcount++;
	    if ((charmap[*outp] & CONCORDABLE) != 0) {
		*outp = charmap[*outp] & 0x00ff;
		outp++;
		candidate_offset = readcount - 1;
		tpstate = IN_WORD;
		continue;
	    }
	    else {
		outp--;
		continue;
	    }
	} /* endif BETW_WORDS */


	/*------------- IN_WORD State ------------
	 * Reader is in middle of a word.
	 * Convert all concordables to uppercase and append.
	 * Terminate word at first non_concord.
	 * Non_concords treatment depends on next char.
	 */
	else if (tpstate == IN_WORD) {
	    if ((charmap[*outp] & CONCORDABLE) != 0) {
		if (outp < endmaxword) {
		    *outp = charmap[*outp] & 0x00ff;
		    outp++;
		}
		else {
		    tpstate = TOO_LONG;
		    if (debugging_teskey)
			fprintf (aa_stderr,
				"teskey: ofs=%3ld \"%.15s...\", (TOO LONG)\n",
				candidate_offset-1, outbuf);
		    if (add_msgs) {
			char	msgbuf [DtSrMAXWIDTH_HWORD + 100];
			sprintf (msgbuf, catgets(dtsearch_catd, MS_lang, 8,
			    "%s '%.*s...' is larger\n"
			    "than the maximum word size of database '%s'.") ,
			    PROGNAME"449", maxwordsz,
			    parg->string, dblk->label);
			DtSearchAddMessage (msgbuf);
			return NULL;
		    }
		    outbuf[0] = 0;
		    outp = outbuf;
		}
		continue;
	    }
	    if ((charmap[*outp] & NON_CONCORD) != 0) {
		*outp = '\0';
		break;
	    }
	    /* Must be opt_concord... */
	    outp++;
	    if ((*outp = cofunction(NULL)))
		readcount++;
	    if ((charmap[*outp] & CONCORDABLE) != 0) {
		if (outp < endmaxword) {
		    *outp = charmap[*outp] & 0x00ff;	/* uppercase */
		    outp++;
		}
		else {
		    tpstate = TOO_LONG;
		    if (debugging_teskey)
			fprintf (aa_stderr,
				"teskey: ofs=%3ld \"%.15s...\", (TOO LONG)\n",
				candidate_offset-1, outbuf);
		    outbuf[0] = 0;
		    outp = outbuf;
		}
		continue;
	    }
	    else {	/* next char NOT concordable...*/
		*(--outp) = '\0';
		break;
	    }
	} /* endif IN_WORD */


	/*------------- TOO_LONG State ------------
	 * Reader is in middle of a word that exceeds max word size.
	 * Discard all concordables and opt_concords until we
	 * can get between words again with a clear non_concord.
	 */
	else if (tpstate == TOO_LONG) {
	    if ((charmap[*outp] & NON_CONCORD) != 0) {
		outp = outbuf;
		tpstate = BETW_WORDS;
	    }
	    continue;
	}

	/*------------- UNKNOWN State ------------*/
	else {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_lang, 10,
		"%s Program Error: Unknown parser state.\n"),
		PROGNAME"306");
	    DtSearchExit (26);
	}
    } /* end read loop for next CANDIDATE WORD */

    /*---------- TEST FOR ETX -------------*/
    if (outbuf[0] == 0) {
	if (debugging_teskey)
	    fprintf (aa_stderr, "teskey: etx\n");
	if (add_msgs) {
	    char	msgbuf [200];
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_lang, 12,
		"%s '%.120s' is not a valid word in database '%s'.") ,
		PROGNAME"506", parg->string, dblk->label);
	    DtSearchAddMessage (msgbuf);
	}
	return NULL;
    }

    wordlen = strlen ((char *) outbuf);
    candidate_offset--;	/* token offset is one less than number of reads */
    if (debugging_teskey)
	fprintf (aa_stderr, "teskey: ofs=%3ld \"%s\"",
	    candidate_offset, outbuf);

    if (is_hiliting) {
	if (debugging_teskey)
	    fprintf (aa_stderr, ", (hiliting, skip tree searches)");
	goto GOOD_WORD;
    }

    /*--------- INCLUDE LIST ----------
     * Search before testing for stoplist or minimum word length.
     */
    if (dblk->inclist != NULL) {
	if (search_wordtree (dblk->inclist, outbuf)) {
	    if (debugging_teskey)
		fprintf (aa_stderr, ", (INCLUDE LIST)");
	    goto GOOD_WORD;
        }
    }

    /*--------- TOO SHORT -----------*/
    if (wordlen < minwordsz) {
	if (debugging_teskey)
	    fprintf (aa_stderr, ", (TOO SHORT, min %d)\n", minwordsz);
	if (add_msgs) {
	    char	msgbuf [200];
	    sprintf (msgbuf, catgets(dtsearch_catd, MS_lang, 17,
		"%s '%s' is less than the\n"
		"minimum word size of database '%s'.") ,
		PROGNAME"543", parg->string, dblk->label);
	    DtSearchAddMessage (msgbuf);
	    return NULL;
	}
	goto READ_ANOTHER_WORD;
    }

    /*----------- STOP LIST -------------*/
    if (dblk->stoplist != NULL) {
	if (search_wordtree (dblk->stoplist, outbuf)) {
	    if (debugging_teskey)
		fprintf (aa_stderr, ", (STOP LIST)\n");
	    if (add_msgs) {
		char	msgbuf [200];
		sprintf (msgbuf, catgets(dtsearch_catd, MS_lang, 19,
		    "%s The word '%s' is not indexed in database '%s'.") ,
		    PROGNAME"558", parg->string, dblk->label);
		DtSearchAddMessage (msgbuf);
		return NULL;
	    }
	    goto READ_ANOTHER_WORD;
	}
    }

GOOD_WORD:
    /* Word is correctly parsed and passes all dblk filters. */
    if (debugging_teskey)
	fprintf (aa_stderr, ", ...good word\n");
    if (offsetp)
	*offsetp = candidate_offset;
    return (char *) outbuf;
} /* teskey_parser() */


/************************************************/
/*						*/
/*                 is_concordable		*/
/*						*/
/************************************************/
/* Verifies passed word token is teskey-concordable
 * in code page of passed charmap.  Used in validating
 * word files.  Returns TRUE if all chars concordable
 * or optionally concordable, else returns FALSE.
 */
int	is_concordable (char *word, int *charmap)
{
    UCHAR	*cptr;
    for (cptr = (UCHAR *)word;  *cptr != 0;  cptr++)
	if ((charmap[*cptr] & NON_CONCORD) != 0)
	    break;
    return (*cptr == 0);
} /* is_concordable() */


/************************************************/
/*						*/
/*                 load_wordtree		*/
/*						*/
/************************************************/
/* Called by load_stop_list(), load_include_list(), etc,
 * to read an appropriate word list file into binary tree structures.
 *
 * INPUT FILE FORMAT:  One word per line, all chars teskey concordable.
 * Preferred order is frequency of occurrence in the corpus
 * to make searches efficient.  Otherwise the words should at least
 * be in random order or an order that will approximate a binary search.
 * If first char is any of COMMENT_CHARS, line is ignored as comments.
 * Ascii spaces, tabs, or newline delimits the first word token--
 * anything else on the line is ignored as comments.
 * Optionally characters in word token will be checked for teskey
 * concordability.
 *
 * RETURNS 0 if file successfully loaded, returns 1 if file missing,
 * returns 2 and messages in global msglist if file has fatal errors.
 */
int	load_wordtree (
		    WORDTREE	**treetop,
		    DBLK	*dblk,
		    char	*fname,
		    int		do_teskey_test)
{
    int		i;
    int		errcount;
    int		is_duplicate;
    long	linecount = 0;
    char	*token;
    char	readbuf [256];
    char	sprintbuf [_POSIX_PATH_MAX + 1024];
    FILE	*fileid;
    WORDTREE	*new;
    WORDTREE	**this_link;
    _Xstrtokparams	strtok_buf;

    if (debugging_loadlang)
	fprintf (aa_stderr, PROGNAME"1071 "
	    "load_wordtree: db=%s fname='%s'\n",
	    NULLORSTR(dblk->name), NULLORSTR(fname));

    if ((fileid = fopen (fname, "rt")) == NULL) {
	/* Not being able to find the file is not an error.
	 * We indicate that with the return code.
	 * But any other error (like permissions) is fatal.
 	 */
	if (errno == ENOENT) {
	    if (debugging_loadlang)
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
    errcount = 0;
    while (fgets (readbuf, sizeof(readbuf), fileid) != NULL) {
	linecount++;
	/*
	 * Ignore comment lines beginning with punctuation char.
	 * Ignore empty lines (strtok returns NULL, no tokens).
	 * Otherwise first or only word on line is the desired word. 
	 */
	if (strchr (COMMENT_CHARS, readbuf[0]))
	    continue;
	if ((token = _XStrtok(readbuf, " \t\n", strtok_buf)) == NULL)
	    continue;
	dblk->lstrupr (token, dblk);

	if (debugging_loadword)
	    fprintf (aa_stderr, "  WORD: '%s'  ", token);

	/* If requested confirm all chars are teskey-concordable. */
	if (do_teskey_test)
	    if (!is_concordable (token, dblk->charmap)) {
		sprintf (sprintbuf, catgets (dtsearch_catd, MS_misc, 400,
		    "%s: %s, line %ld: Invalid chars in word '%s'."),
		    PROGNAME"400", fname, linecount, token);
		DtSearchAddMessage (sprintbuf);
		errcount++;
		continue;
	    }

	/* Unless we've already detected some errors,
	 * allocate a new node and load its data fields.
	 */
	if (errcount)
	    continue;
	i = strlen (token);
	new = austext_malloc (sizeof(WORDTREE) + i + 4,
	    PROGNAME"104", NULL);
	new->llink = NULL;
	new->rlink = NULL;
	new->len = i;
	new->word = (void *) (new + 1);
	strcpy (new->word, token);

	/* Descend binary tree and insert in correct alphabetical place */
	is_duplicate = FALSE;
	for (this_link = treetop;  *this_link != NULL;  ) {
	    i = strcmp (new->word, (*this_link)->word);

	    /* test for duplicate word */
	    if (i == 0) {
		sprintf (sprintbuf, catgets (dtsearch_catd, MS_misc, 423,
		    "%s Word '%s' in '%s' is a duplicate."),
		    PROGNAME"423", token, fname);
		DtSearchAddMessage (sprintbuf);
		/* duplicates aren't fatal, just ignore the word */
		is_duplicate = TRUE;
		break;	/* no point in continuing descent */
	    }

	    /* Descend tree to find correct insertion point */
	    if (debugging_loadword)
		fputc(((i < 0)? 'L' : 'R'), aa_stderr);
	    this_link = (WORDTREE **) ((i < 0) ?
		&(*this_link)->llink : &(*this_link)->rlink);
	} /* end forloop to find tree insertion point */

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
	    fputs(" .\n", aa_stderr);
    }	/* end of read loop */

    fclose (fileid);

    if (errcount) {
	if (debugging_loadlang)
	    fprintf (aa_stderr,
		PROGNAME"1186 load word file '%s' failed.\n", fname);
	return 2;
    }
    else {
	if (debugging_loadlang)
	    fprintf (aa_stderr,
		PROGNAME"1193 load word file '%s' successful.\n", fname);
	return 0;
    }
}  /* load_wordtree() */


/************************************************/
/*						*/
/*		  free_wordtree			*/
/*						*/
/************************************************/
/* Formerly free_bintree() in msgutil.c.
 * Frees storage for all nodes in a WORDTREE and
 * sets its top-of-list pointer to NULL.
 * Works only for node structures where all memory
 * was allocated in a single call to malloc(). 
 * Uses link inversion traversal (eg, Data Structure Techniques,
 * Thomas A. Standish, Algorithm 3.6) where TAG is initialized
 * at preorder visit, and node is freed at postorder visit.
 */
static void	free_wordtree (WORDTREE ** wordtree_head)
{
    WORDTREE        *next;
    WORDTREE        *prev = NULL;
    WORDTREE        *pres = *wordtree_head;

    if (*wordtree_head == NULL)
	return;

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
    free (pres);
    if (prev == NULL) {	/* end of algorithm? */
	*wordtree_head = NULL;
	return;
    }
    if (prev->word == (void *) 0) {	/* go up left leg */
	next = prev->llink;
	pres = prev;
	prev = next;
	goto DESCEND_RIGHT;
    }
    else {	/* go up right leg */
	next = prev->rlink;
	prev->word = (void *) 0;	/* restore TAG = 0 */
	pres = prev;
	prev = next;
	goto POSTORDER_VISIT;
    }
}  /* free_wordtree() */


/************************************************/
/*						*/
/*              load_include_list		*/
/*						*/
/************************************************/
/* Builds include list by reading include file
 * into a binary tree structure.
 * Unlike stoplists, include-lists are optional.
 * Also unlike stoplists, there are no language default include-lists.
 * 'dblist' may be NULL.
 * RETURNS TRUE if no problems, else FALSE with msg in ausapi_msglist.
 */
static int	load_include_list (DBLK *dblk, DBLK *dblist)
{
    int		i;
    int		filename_was_null = (dblk->fname_inc == NULL);
    DBLK	*db;
    char	sprintbuf [512];

    dblk->inclist = NULL;	/* just to be sure */
    
    if (debugging_loadlang)
        fprintf (aa_stderr,
	    PROGNAME"1705 Load inclist: db='%s' lang=#%d,%s\n",
	    NULLORSTR(dblk->name), (int)dblk->dbrec.or_language,
	    language_name(dblk->dbrec.or_language));

    /* If file name not provided, generate one based on
     * dblk's path, database name, and default extension.
     */
    if (filename_was_null) {
	if (dblk->name[0] == 0) {
	    dblk->fname_inc = "";
	    dblk->inclist = NULL;
	    if (debugging_loadlang)
	        fprintf (aa_stderr, PROGNAME"1339 "
		    "No inclist because neither fname nor dbname provided.\n");
	    return TRUE;
	}
	if (dblk->path == NULL)
	    dblk->path = strdup("");
	dblk->fname_inc = austext_malloc (strlen(dblk->path) + 36,
	     PROGNAME"1187", NULL);
	strcpy (dblk->fname_inc, dblk->path);
	ensure_end_slash (dblk->fname_inc);
	strcat (dblk->fname_inc, dblk->name);
	strcat (dblk->fname_inc, EXT_INCLIST);
    }
    if (debugging_loadlang)
        fprintf (aa_stderr,
	    PROGNAME"1350 Include list file name = '%s'.\n",
	    dblk->fname_inc);

    /* Dont reload the same file if it's already
     * been loaded into a previous dblk in a list.
     * Code works just fine if dblist == NULL.
     */
    for (db = dblist;  db != NULL;  db = db->link) {
	if (db == dblk || db->fname_inc == NULL)
	    continue;
	if (strcmp (db->fname_inc, dblk->fname_inc) == 0) {
	    dblk->inclist = db->inclist;
	    dblk->lang_flags |= LF_DUP_INC;
	    if (debugging_loadlang)
	        fprintf (aa_stderr, PROGNAME"1363 "
		    "Using previously loaded inclist, db='%s'.\n",
		    dblk->name);
	    return TRUE;
	}
    }

    /* Include list is optional so missing file is
     * not an error unless caller named a specific file.
     */
    i = load_wordtree (&dblk->inclist, dblk, dblk->fname_inc, TRUE);
    switch (i) {
	case 0:
	    return TRUE;

	case 1:
	    if (filename_was_null) {
		dblk->fname_inc = "";
		dblk->inclist = NULL;
		return TRUE;
	    }
	    else {
		sprintf (sprintbuf,
		    catgets (dtsearch_catd, MS_misc, 362, "%s: %s: %s."),
		    PROGNAME"1218", dblk->fname_inc, strerror(ENOENT));
		DtSearchAddMessage (sprintbuf);
		return FALSE;
	    }
	    
	default:
	    return FALSE;
    }
} /* load_include_list() */


/************************************************/
/*						*/
/*               load_stop_list			*/
/*						*/
/************************************************/
/* Builds stoplist by reading stoplist file into a
 * binary tree structure.  File name can be
 *    (1) passed in dblk.fname_stp,
 *    (2) generated from dblk path, name, and '.stp',
 *    (3) default for dblk path, language, and '.stp'.
 * 'dblist' may be NULL.
 * RETURNS TRUE if no problems, else FALSE with msg in ausapi_msglist.
 */
static int	load_stop_list (DBLK *dblk, DBLK *dblist)
{
    int		i;
    DBLK	*db;
    char	sprintbuf [_POSIX_PATH_MAX + 512];
    struct stat	statbuf;

    dblk->stoplist = NULL;	/* just to be sure */
    
    if (debugging_loadlang)
        fprintf (aa_stderr,
	    PROGNAME"1700 Load stoplist: db='%s' lang=#%d,%s\n",
	    NULLORSTR(dblk->name), (int)dblk->dbrec.or_language,
	    language_name(dblk->dbrec.or_language));

    /* If file name not provided, generate one based on
     * dblk's path, database name, and default extension.
     * And if that doesn't work, generate one based on
     * dblk's path, language, and default extension.
     */
    if (dblk->fname_stp == NULL) {
	if (dblk->path == NULL)
	    dblk->path = strdup("");
	dblk->fname_stp = austext_malloc (strlen(dblk->path) + 36,
	     PROGNAME"919", NULL);

	strcpy (dblk->fname_stp, dblk->path);
	ensure_end_slash (dblk->fname_stp);
	strcat (dblk->fname_stp, dblk->name);
	strcat (dblk->fname_stp, EXT_STOPLIST);
	errno = 0;
	stat (dblk->fname_stp, &statbuf);
	if (errno == ENOENT) {
	    strcpy (dblk->fname_stp, dblk->path);
	    ensure_end_slash (dblk->fname_stp);
	    strcat (dblk->fname_stp, lang_fnames [dblk->dbrec.or_language]);
	    strcat (dblk->fname_stp, EXT_STOPLIST);
	}
    }
    if (debugging_loadlang)
        fprintf (aa_stderr,
	    PROGNAME"1448 Stoplist file name = '%s'.\n",
	    dblk->fname_stp);

    /* Dont reload the same file if it's already
     * been loaded into a previous dblk in a list.
     * Code works just fine if dblist == NULL.
     */
    for (db = dblist;  db != NULL;  db = db->link) {
	if (db == dblk || db->fname_stp == NULL)
	    continue;
	if (strcmp (db->fname_stp, dblk->fname_stp) == 0) {
	    dblk->stoplist = db->stoplist;
	    dblk->lang_flags |= LF_DUP_STP;
	    if (debugging_loadlang)
	        fprintf (aa_stderr, PROGNAME"1460 "
		    "Using previously loaded stoplist, db='%s'.\n",
		    dblk->name);
	    return TRUE;
	}
    }

    /* Stop lists are mandatory--a missing stoplist is fatal. */
    i = load_wordtree (&dblk->stoplist, dblk, dblk->fname_stp, TRUE);
    if (i == 1) {
	sprintf (sprintbuf,
	    catgets (dtsearch_catd, MS_misc, 362, "%s: %s: %s"),
	    PROGNAME"1270", dblk->fname_stp, strerror(ENOENT));
	DtSearchAddMessage (sprintbuf);
    }
    return (i == 0);
} /* load_stop_list() */


/************************************************/
/*						*/
/*		free_paice_rules		*/
/*						*/
/************************************************/
/* Frees all allocated storage for a set of paice rules, typically
 * loaded at dblk.stem_extra.  Called by REINIT routines and
 * by load_paice_suffixes() when cleaning up after an error.
 */
static void	free_paice_rules (PRULE ***rules_table_ptr)
{
    int		i;
    PRULE	*p, **linkp;
    PRULE	**rules_table;

    if (*rules_table_ptr == NULL)
	return;
    rules_table = *rules_table_ptr;
    for (i=0; i<256; i++) {
	if (rules_table[i] == NULL)
	    continue;
	p = rules_table[i];
	while (p) {
	    linkp = &p->link;
	    free (p->suffix);
	    if (p->apndstr)
		free (p->apndstr);
	    free (p);
	    p = *linkp;
	}
    }
    free (rules_table);
    *rules_table_ptr = NULL;
    return;
} /* free_paice_rules() */


/************************************************/
/*						*/
/*		load_paice_suffixes		*/
/*						*/
/************************************************/
/* Loads European language paice stemmer suffix rules
 * into dblk.stem_extra as an array of ptrs to linked lists.
 * Like stop lists, sfx files can be
 *    (1) passed in dblk.fname_sfx,
 *    (2) generated from dblk path, dbname, and '.sfx',
 *    (3) generated from dblk path, language, and '.sfx'.
 * Internal tables will be reused if file previously loaded.
 * Only uses single byte character sets (ascii, iso-latin-1).
 * Uses strtok().  dblk->charmap must already be loaded.
 * Will continue to parse entire file even if errors are found.
 * RETURNS TRUE if no problems, else FALSE with msg in ausapi_msglist.
 */
static int      load_paice_suffixes (DBLK *dblk, DBLK *dblist)
{
    FILE	*fp;
    DBLK	*db;
    PRULE	*prule, **prule_link;
    PRULE	**rules_table;
    struct stat	statbuf;
    UCHAR	*cptr, *token;
    char	readbuf [_POSIX_PATH_MAX + 1024];
    char	msgbuf [_POSIX_PATH_MAX + 1024];
    UCHAR	*suffix, *apndstr;
    int		must_be_intact, is_last_rule;
    UCHAR	remove_count;
    int		lineno, errcount;
    _Xstrtokparams	strtok_buf;

    dblk->stem_extra = NULL;	/* just to be sure */
    rules_table = NULL;

    if (debugging_loadlang)
        fprintf (aa_stderr,
	    PROGNAME"1715 Load paice suffixes: db='%s' lang=#%d,%s\n",
	    NULLORSTR(dblk->name), (int)dblk->dbrec.or_language,
	    language_name(dblk->dbrec.or_language));

    /* If file name not provided, generate one based on
     * dblk's path, database name, and default extension.
     * And if that doesn't work, generate one based on
     * dblk's path, language, and default extension.
     */
    if (dblk->fname_sfx == NULL) {
	if (dblk->path == NULL)
	    dblk->path = strdup("");
	dblk->fname_sfx = austext_malloc (strlen(dblk->path) + 36,
	     PROGNAME"1113", NULL);

	strcpy (dblk->fname_sfx, dblk->path);
	ensure_end_slash (dblk->fname_sfx);
	strcat (dblk->fname_sfx, dblk->name);
	strcat (dblk->fname_sfx, EXT_SUFFIX);
	errno = 0;
	stat (dblk->fname_sfx, &statbuf);
	if (errno == ENOENT) {
	    strcpy (dblk->fname_sfx, dblk->path);
	    ensure_end_slash (dblk->fname_sfx);
	    strcat (dblk->fname_sfx, lang_fnames [dblk->dbrec.or_language]);
	    strcat (dblk->fname_sfx, EXT_SUFFIX);
	}
    }
    if (debugging_loadlang)
        fprintf (aa_stderr,
	    PROGNAME"1740 Paice suffix file name = '%s'.\n",
	    dblk->fname_sfx);

    /* Dont reload the same file if it's already
     * been loaded into a previous dblk in a list,
     * but flag it so it won't be freed at unload_language/REINIT.
     * Code works just fine if dblist == NULL.
     */
    for (db = dblist;  db != NULL;  db = db->link) {
	if (db == dblk || db->fname_sfx == NULL)
	    continue;
	if (strcmp (db->fname_sfx, dblk->fname_sfx) == 0) {
	    dblk->stem_extra = db->stem_extra;
	    dblk->lang_flags |= LF_DUP_SFX;
	    if (debugging_loadlang)
	        fprintf (aa_stderr, PROGNAME"1145 "
		    "Using previously loaded suffixes, db='%s'.\n",
		    dblk->name);
	    return TRUE;
	}
    }

    fp = fopen (dblk->fname_sfx, "rt");
    if (fp == NULL) {
	sprintf (msgbuf,
	    catgets (dtsearch_catd, MS_misc, 362, "%s: %s: %s."),
	    PROGNAME"181", dblk->fname_sfx, strerror(errno));
	DtSearchAddMessage (msgbuf);
	dblk->fname_sfx = NULL;
	return FALSE;
    }

    /* Rules table will eventually be loaded at dblk.stem_extra.
     * It consists of 256 PRULE ptrs,
     * one for each possible single byte char.
     * Each ptr is the head of a rules list for that char.
     */
    rules_table = austext_malloc (256 * sizeof(PRULE*),
	PROGNAME"199", &ausapi_msglist);
    memset (rules_table, 0, 256 * sizeof(PRULE*));
    lineno =	0;
    errcount =	0;

    /*------- Main Read Loop -------*/
    while (fgets (readbuf, sizeof(readbuf), fp) != NULL) {
	lineno++;

	/* Ignore comment lines */
	if (strchr (COMMENT_CHARS, readbuf[0]))
	    continue;

	/* TOKEN #1: suffix string, backwards, all uppercase.
	 * If missing, ignore 'empty' line.
	 * If the first token is all numeric, ignore line
	 * (for compatibility with older versions of file).
	 */
	if ((suffix = (UCHAR *)_XStrtok(readbuf, SFX_DELIMS, strtok_buf)) == NULL)
	    continue;

	for (cptr = suffix;  cptr;  cptr++)
	    if ((dblk->charmap[*cptr] & NUMERAL) == 0)
		break;
	if (*cptr == '\0')
	    continue;

	/* OPTIONAL TOKEN #2: if next token '*', set 'intact' flag */
	if ((token = (UCHAR *)_XStrtok(NULL, SFX_DELIMS, strtok_buf)) == NULL) {
BAD_RULE:
	    sprintf (msgbuf,  catgets(dtsearch_catd, MS_lang, 51,
		"%s %s, Line %d: Invalid Paice Rule for suffix '%s'.") ,
		PROGNAME"898", dblk->fname_sfx, lineno, suffix);
	    DtSearchAddMessage (msgbuf);
	    errcount++;
	    continue;
	}
	must_be_intact = FALSE;
	if (token[0] == '*') {
	    must_be_intact = TRUE;
	    /* Read next token... */
	    if ((token = (UCHAR *)_XStrtok(NULL, SFX_DELIMS, strtok_buf)) == NULL)
		goto BAD_RULE;
	}

	/* TOKEN #3: remove-count */
	remove_count = (UCHAR) atoi ((char *) token);

	/* OPTIONAL TOKEN #4: if next token is NOT a continue
         * symbol ('>' or '$'), then it's an append string.
	 */
	apndstr = NULL;
	if ((token = (UCHAR *)_XStrtok(NULL, SFX_DELIMS, strtok_buf)) == NULL)
	    goto BAD_RULE;
	if (token[0] != '$'  &&  token[0] != '>') {
	    apndstr = token;
	    /* Read next token... */
	    if ((token = (UCHAR *)_XStrtok(NULL, SFX_DELIMS, strtok_buf)) == NULL)
		goto BAD_RULE;
	}

	/* TOKEN #5: continue symbol '$' (stop) or '>' (continue) */
	is_last_rule = (token[0] == '$');

	if (debugging_loadword) {
	    fprintf (aa_stderr,
		"  SFX: intact?=%d stop?=%d remv=%d '%s'",
		(int) must_be_intact,
		(int) is_last_rule,
		(int) remove_count,
		suffix);
	    if (apndstr)
		fprintf (aa_stderr, "\tapnd='%s'\n", apndstr);
	    else
		fputc ('\n', aa_stderr);
	}

	/* Good suffix.  If we haven't had any errors yet,
         * add it to rules list for the first char of the suffix.
	 */
	if (errcount)
	    continue;
	prule = austext_malloc (sizeof(PRULE), PROGNAME"1252", NULL);
	memset (prule, 0, sizeof(PRULE));
	prule->suffix =		(UCHAR *) strdup ((char*)suffix);
	prule->suflen =		strlen ((char*)suffix);
	prule->must_be_intact =	must_be_intact;
	prule->remove_count =	remove_count;
	prule->is_last_rule =	is_last_rule;
	if (apndstr) {
	    prule->apndstr =	(UCHAR *) strdup ((char*)apndstr);
	    prule->aplen =	strlen ((char*)apndstr);
	}

	prule_link = &rules_table[suffix[0]];
	while (*prule_link)
	    prule_link = &(*prule_link)->link;
	*prule_link = prule;

    } /* end Main Read Loop */

    fclose (fp);
    if (errcount) {
	free_paice_rules (&rules_table);
	return FALSE;
    }
    dblk->stem_extra = rules_table;

    /* Update last table entry */
    if (debugging_loadlang) {
        fprintf (aa_stderr,
	    PROGNAME"1654 Paice suffix file '%s' loaded ok.\n",
	    dblk->fname_sfx);
	fflush (aa_stderr);
    }
    return TRUE;
}  /* load_paice_suffixes() */


/************************************************/
/*						*/
/*		 is_matching_rule		*/
/*						*/
/************************************************/
/* Subroutine of paice_stemmer().
 * Returns TRUE if passed rule can be applied to stem in paicebuf.
 * Else returns FALSE.
 */
static int	is_matching_rule (PRULE *rule)
{
    static UCHAR	*ptr;
    static int		i, j;

    if (debugging_paice)
	fprintf (aa_stderr, "  test rule '%s':\t", rule->suffix);

    /* Skip rule if we've made at least one previous change
     * but the current rule requires an intact word.
     */
    if (rule->must_be_intact  &&  !word_is_intact) {
	if (debugging_paice)
	    fputs ("word not intact...\n", aa_stderr);
	return FALSE;
    }

    /* Do a backward strcmp on the suffix.
     * Skip rule if it doesn't match current paicebuf's ending chars.
     */
    j = rule->suflen;
    ptr = paicebuf + paicelen - 1;
    for (i = 0; i < j; i++) {
	if (*((rule->suffix) + i) != *ptr) {
	    if (debugging_paice)
		fputs ("no match...\n", aa_stderr);
	    return FALSE;
	}
	ptr--;
    }

    if (debugging_paice)
	fputs ("match", aa_stderr);

    /* Set i = paicebuf length after removing and appending suffixes.
     * Used to algorithmically test remaining stem length
     * after tentative application of rule.
     */
    i = paicelen - (rule->remove_count - rule->aplen);

    if (i <= 1) {
	if (debugging_paice)
	    fputs (", but stem too short...\n", aa_stderr);
	return FALSE;
    }

    if (i == 2) {
	if (IS_VOWEL (paicebuf[0])) {
	    if (debugging_paice)
		fputs (", and short vowel stem valid.\n", aa_stderr);
	    return TRUE;
	}
	else {
	    if (debugging_paice)
		fputs (", but consonant stem too short...\n", aa_stderr);
	    return FALSE;
	}
    }

    /* Remaining stem is at least 3 chars.
     * If it contains a vowel anywhere, it's valid.
     * (A 'Y' after the first char counts as a vowel).
     * Otherwise it's not.
     */
    for (j=0;  j<i;  j++) {
	if (IS_VOWEL (paicebuf[j])) {
GOOD_STEM:
	    if (debugging_paice)
		fputs (", and remaining stem valid.\n", aa_stderr);
	    return TRUE;
	}
	if (j > 0  &&  paicebuf[j] == 'Y')
	    goto GOOD_STEM;
    }

    if (debugging_paice)
	fputs (", but remaining stem all consonants.\n", aa_stderr);
    return FALSE;
}  /* is_matching_rule() */


/************************************************/
/*						*/
/*                 paice_stemmer		*/
/*						*/
/************************************************/
/* Given a word token (ALREADY UPPERCASE) in a single byte
 * language such as the output of teskey_parser,
 * generates 'stem' by repeated suffix removal.
 * Returns stem token in a static buffer valid
 * until next call to paice_stemmer or null_stemmer.
 * Returned stem might be the original unmodified word.
 * Returned stem might also be empty string.
 * Returned stem is *never* NULL, even if wordin == NULL.
 * Input buffer will not be modified; does not use strtok.
 * All variables are static for speeeeeeed.
 */
static char	*paice_stemmer (char *wordin, DBLK *dblk)
{
    UCHAR	finalc;
    PRULE	*rule, **rules_table;

    if (wordin == NULL)
	return "";
    if (*wordin == 0)
	return "";

    if ((rules_table = (PRULE **)dblk->stem_extra) == NULL) {
	fprintf (aa_stderr, catgets (dtsearch_catd, MS_lang, 31,
	    "%s Stemmer suffixes file never loaded.\n"),
	    PROGNAME"310");
	DtSearchExit (2);
    }

    /* The max length of a stem is bufsz - 2:
     * one for the terminating \0 and one for the
     * prefix ^O that identifies a stem.  (But this
     * stemmer doesn't actually insert the ^O now.)
     */
    strncpy ((char*)paicebuf, wordin, DtSrMAXWIDTH_HWORD);
    paicebuf [DtSrMAXWIDTH_HWORD - 2] = 0;
    paice_charmap =	dblk->charmap;
    word_is_intact = 	TRUE;

    for (;;) { /*-------- Main Stemming Loop ---------*/

	paicelen = strlen ((char*)paicebuf);
	finalc = *(paicebuf + paicelen - 1);
	if (debugging_paice) {
	    fprintf (aa_stderr,
		"paice: '%s', rules list '%c' for database '%s'\n",
		paicebuf, finalc, dblk->name);
	    fflush (aa_stderr);
	}

	/* Look for a matching rule */
	if ((rule = rules_table [finalc]) == NULL) {
	    if (debugging_paice)
		fputs ("  list is null, stop.\n", aa_stderr);
	    break;
	}
	while (rule) {
	    if (is_matching_rule (rule))
		break;
	    rule = rule->link;
	}
	if (rule == NULL) {
	    if (debugging_paice)
		fprintf (aa_stderr, "  rules list '%c' is exhausted, stop.\n",
		    finalc);
	    break;
	}

	/* Apply rule that matched */
	if (debugging_paice)
	    fputs ("    apply rule: ", aa_stderr);
	if (rule->remove_count == 0) {
	    if (debugging_paice)
		fputs ("remove_count = 0, stop.\n", aa_stderr);
	    break;
	}

	paicebuf [paicelen - rule->remove_count] = 0;
	if (rule->aplen)
	    strcat ((char*)paicebuf, (char*)rule->apndstr);
	paicelen = strlen ((char*)paicebuf);
	word_is_intact = FALSE;	 /* we've removed at least 1 suffix */
	if (debugging_paice)
	    fprintf (aa_stderr, "--> '%s'", paicebuf);

	/* Terminate algorithm if rule says so.
	 * Otherwise continue removing suffixes
	 * from this partially stemmed word.
	 */
	if (rule->is_last_rule) {
	    if (debugging_paice)
		fputs (", stop flag is set, stop.\n", aa_stderr);
	    break;
	}
	if (debugging_paice)
	    fputc ('\n', aa_stderr);

    } /* end Main Stemming Loop */

    if (debugging_paice) {
	fprintf (aa_stderr, "  final stem: '%s'\n", paicebuf);
	fflush (aa_stderr);
    }
    return (char *) paicebuf;
} /* paice_stemmer() */


/************************************************/
/*						*/
/*                 null_stemmer			*/
/*						*/
/************************************************/
/* Stemmer that just copies and returns passed word.
 * In effect, the passed word IS its own stem.
 * Output buffer valid until next call to null_stemmer
 * or paice_stemmer.
 */
char	*null_stemmer (char *word, DBLK *dblk)
{
    if (word == NULL)
	return "";
    if (*word == '\0')
	return "";
    strncpy ((char *)paicebuf, word, DtSrMAXWIDTH_HWORD);
    paicebuf [DtSrMAXWIDTH_HWORD-1] = 0;
    return (char *) paicebuf;
} /* null_stemmer() */


/************************************************/
/*						*/
/*                 euro_lstrupr			*/
/*						*/
/************************************************/
/* Converts passed string to uppercase in place.
 * Classic strupr() function using teskey charmaps.
 */
static char	*euro_lstrupr (char *string, DBLK *dblk)
{
    static int		*charmap;
    static UCHAR	*s;
    charmap = dblk->charmap;
    for (s=(UCHAR *)string;  *s;  s++)
	*s = charmap[*s] & 0xff;
    return string;
}


/************************************************/
/*						*/
/*                 null_lstrupr			*/
/*						*/
/************************************************/
/* Just returns passed string.  Used where uppercase
 * conversions are not required for a language.
 */
char	*null_lstrupr (char *s, DBLK *d)
{ return s; }


/************************************************/
/*						*/
/*                load_language			*/
/*						*/
/************************************************/
/* Loads a dblk with a specific language's
 * structures and function pointers.
 * Does not reload structures previously loaded in
 * other dblks on dblist if derived from identical files.
 * But always loads structures if passed dblist is NULL.
 * Presumes dblk already partially initialized with mandatory fields:
 *	name, path, language.
 * May also be preinitialized with optional fields:
 *	minwordsz, maxwordsz.
 * Returns TRUE if all successful.
 * Otherwise returns FALSE with err msgs on ausapi_msglist.
 */
int	load_language (DBLK *dblk, DBLK *dblist)
{
    int		oops =	FALSE;
    int		language = dblk->dbrec.or_language;

    if (debugging_loadlang)
        fprintf (aa_stderr,
	    "\n"PROGNAME"1920 Loading language #%d, %s, for dblk '%s'.\n",
	    (int)dblk->dbrec.or_language,
	    language_name (dblk->dbrec.or_language),
	    NULLORSTR(dblk->name));

    /*
     * Note: Load list functions must be called
     * AFTER charmap and lstrupr are loaded.
     */
    switch (language) {	
	case DtSrLaENG:
	case DtSrLaENG2:
	case DtSrLaESP:
	case DtSrLaFRA:
	case DtSrLaITA:
	case DtSrLaDEU:
	    dblk->charmap =	(language == DtSrLaENG)?
				    ascii_charmap : latin_charmap;
	    dblk->parser =	teskey_parser;
	    dblk->stemmer =	paice_stemmer;
	    dblk->lstrupr =	euro_lstrupr;
	    if (dblk->dbrec.or_maxwordsz == 0)
		dblk->dbrec.or_maxwordsz = (language == DtSrLaDEU)?
		    MAXWIDTH_LWORD - 1 : MAXWIDTH_SWORD - 1;
	    if (dblk->dbrec.or_minwordsz == 0)
		dblk->dbrec.or_minwordsz = MINWIDTH_TOKEN + 1;
	    oops = FALSE;
	    if (!load_stop_list (dblk, dblist))
		oops = TRUE;
	    if (!load_include_list (dblk, dblist))
		oops = TRUE;
	    if (!load_paice_suffixes (dblk, dblist))
		oops = TRUE;
	    if (oops)
	        return FALSE;
	    break;

	case DtSrLaJPN:
	case DtSrLaJPN2:
	    return load_jpn_language (dblk, dblist);

	default:
	    /* Try loading a custom 'user' language.
	     * If he failed to provide a loader function,
	     * the dummy custom loader will tell him so.
	     * If he provided one but it can't load this language,
	     * it should return it's own error msgs.
	     */
	    return load_custom_language (dblk, dblist);

    } /* end switch (language) */

    return TRUE;
} /* load_language() */


/************************************************/
/*						*/
/*                unload_language		*/
/*						*/
/************************************************/
/* Frees storage for structures allocated by load_language().
 * Called when engine REINITs due to change in site config file
 * or databases.
 * Duplicate wordtrees are not unloaded because they
 * will have already been unloaded in a previous dblk.
 */
void	unload_language (DBLK *dblk)
{
    switch (dblk->dbrec.or_language) {
	case DtSrLaENG:
	case DtSrLaENG2:
	case DtSrLaESP:
	case DtSrLaFRA:
	case DtSrLaITA:
	case DtSrLaDEU:
	    dblk->charmap = NULL;
	    if ((dblk->lang_flags & LF_DUP_STP) == 0)
		free_wordtree (&dblk->stoplist);
	    else {
		dblk->stoplist = NULL;
		dblk->lang_flags &= ~LF_DUP_STP;
	    }
	    if ((dblk->lang_flags & LF_DUP_INC) == 0)
	        free_wordtree (&dblk->inclist);
	    else {
		dblk->inclist = NULL;
		dblk->lang_flags &= ~LF_DUP_INC;
	    }
	    if ((dblk->lang_flags & LF_DUP_SFX) == 0)
	        free_paice_rules ((PRULE***)&dblk->stem_extra);
	    else {
		dblk->stem_extra = NULL;
		dblk->lang_flags &= ~LF_DUP_SFX;
	    }
	    break;

	case DtSrLaJPN:
	case DtSrLaJPN2:
	    unload_jpn_language (dblk);
	    break;

	default:
	    unload_custom_language (dblk);
	    break;
    }
    return;
} /* unload_language() */
/******************** LANG.C ********************/

