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
 *   FUNCTIONS: descend_tree
 *		displayable
 *		fill_data1
 *		load_into_bintree
 *		main
 *		print_exit_code
 *		print_usage_msg
 *		put_addrs_2_dtbs_addr_file
 *		segregate_dicname
 *		traverse_tree
 *		user_args_processor
 *		write_2_dtbs_addr_file
 *		write_new_word_2_dtbs
 *		write_to_file
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1992,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/************************ DTSRINDEX.C *******************************
 * $XConsortium: dtsrindex.c /main/10 1996/09/23 21:02:54 cde-ibm $
 * CDE version of borodin.c
 * Formerly dtsrindex.c was cborodin.c.
 * 
 * INPUT FORMAT:
 * Text file in FZK format.
 * Each record contains 4 formatted 'lines' (text strings ending in \n):
 * 1. fzkey (not used in this program).
 * 2. abstract (not used in this program).
 * 3. unique database key for the record.  Used to find the database
 *    address of the record which is the reference for the inverted index.
 * 4. The record's date (not used in this program).
 * 
 * The rest of the record is unformatted text (not necessarily organized
 * into 'lines').  It is read a character at a time and parsed into
 * individual words by the parser function for the database's language.
 * Each record ends with a delimiter string specified by command line arg.
 *
 * $Log$
 * Revision 2.8  1996/04/10  19:50:38  miker
 * Deleted dangerous and unnecessary -a option.
 *
 * Revision 2.7  1996/03/25  18:54:15  miker
 * Changed FILENAME_MAX to _POSIX_PATH_MAX.
 *
 * Revision 2.6  1996/02/01  18:25:44  miker
 * AusText 2.1.11, DtSearch 0.3.  Pass 1 changed to accommodate
 * new single-character reading parser/stemmers.
 *
 * Revision 2.5  1995/12/29  17:16:04  miker
 * Bug fix: Opened wrong msg catalog.
 *
 * Revision 2.4  1995/12/27  21:18:40  miker
 * Msg bug: 'percent done' was negative number.
 *
 * Revision 2.3  1995/12/01  16:15:44  miker
 * Deleted unnecessary log2 var, conflict with Solaris function.
 * Added -r command line arg.
 *
 * Revision 2.2  1995/10/26  15:26:53  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  19:29:53  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/05  21:08:54  miker
 * Fixed bug: appeared as if 1 and 2 char 'words' were being indexed.
 * Added DEBUG_P switch.
 *
 * Revision 1.2  1995/09/01  22:17:02  miker
 * Fixed solaris segfault: too many args to printf in print_usage().
 *
 * Revision 1.1  1995/08/31  20:51:08  miker
 * Initial revision of dtsrindex.c, copied from cborodin.c.
 *
 * Log: cborodin.c,v
 * Revision 1.18  1995/05/30  18:58:54  miker
 * Correct bug introduced by previous fix (2.1.5c).
 *
 * Revision 1.17  1995/05/18  22:54:08  miker
 * 2.1.5b cborodin bug.  Segfault due to overflowing bitvector
 * after many deletions and no mrclean.
 */
#include "SearchP.h"
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>
#include <locale.h>
#include "vista.h"

extern void     find_keyword (char *cur_word, int vista_num);
extern void     read_wordstr (struct or_hwordrec * glob_word, int vista_num);
extern void     write_wordstr (struct or_hwordrec * glob_word, int vista_num);
extern void     fill_data1 (char *ch);

#define PROGNAME	"DTSRINDEX"

#define BATCH_SIZE	10000L
#define WORDS_PER_DOT	500
#define RECS_PER_DOT	20
#define INBUFSZ		1024	/* default input text header line size */
#define MS_misc		1
#define MS_cborodin	14

/******************* BIT VECTORS *****************/
DB_ADDR        *word_addrs_ii;		/* fread buf for d99 (= tot # dbas) */
DtSrINT32	*dbas_word_count;
char           *dbas_bits_batch;
DB_ADDR        *record_addr_word;
DtSrINT32	num_addrs_for_word;
DtSrINT32	or_reccount;
DtSrINT32	bit_vector_size;

/*-------------------------- GLOBALS ----------------------------*/
/* batch_size also used by fileman.c for allocating unused holes
 * in order to no go past end of 'record_addr_word' array.
 */
extern DtSrINT32  batch_size;

char            buf[1024];
static int      cache_size =		CACHE_SIZE;
static int      check_existing_addrs =	TRUE;
long            count_word_ii =		0L;
long            dbkey_seqno =		0L;
DBLK            dblk;
DBREC		dbrec;
static int	debugging =		0;
  #define DEBUG_I	0x01	/* P1 tree insertions */
  #define DEBUG_P	0x10	/* P1 parser/stemmer */
  #define DEBUG_T	0x02	/* P2 tree dump (words) */
  #define DEBUG_N	0x04	/* P2 NEW words, vista */
  #define DEBUG_O	0x08	/* P2 OLD words, vista)  */
  #define DEBUG_t	0x20	/* P2 tree dump (dbas) */
  #define DEBUG_n	0x40	/* P2 NEW d99 for new words */
  #define DEBUG_o	0x80	/* P2 OLD d99 updates for old words */
static unsigned long
		default_hashsize;
char            dicname [10];
char            dicpath [_POSIX_PATH_MAX];
static int      dotcount =		0;
char            dtbs_addr_file [_POSIX_PATH_MAX];
FILE           *dtbs_addr_fp;
long            dtbs_size_records =	0L;
static long     duplicate_recids =	0L;
struct stat     fstat_input;
FILE_HEADER     fl_hdr;
static char     fname_input [_POSIX_PATH_MAX];
struct or_hwordrec
		got_word;
static FILE    *instream;
char            *inbuf;
int             inbuf_overflowed =	FALSE;
size_t		inbufsz =		INBUFSZ;
int             is_pmr;
static DtSrINT32
		or_maxdba =		0;
static char	msg_374[] =	"\n%s Out of Memory!\n"
				"  Split the incoming file into several "
				"smaller files and try again.\n";
static char	msg_776[] =	"\n%s Write Failure d99 file: %s\n";
char            new_dtbs_file =		FALSE;
long            num_of_diff_words =	0L;
int             normal_retncode =	0;
static PARG	parg;
int             parsep_char =		END_RETAIN_PAGE;
char            rec_type;
unsigned long	record_count =		0UL;
int             record_lines;
static int      recs_per_dot =		RECS_PER_DOT;
static unsigned long
		seconds_left;
extern int      shutdown_now;
static DtSrINT32
		or_recslots;
char            *sprintbuffer =		NULL;
char            *temp =			NULL;
extern int	debugging_teskey;
time_t          timestart =		0;
time_t          totalstart =		0;
static int      words_per_dot =		WORDS_PER_DOT;

/************************************************/
/*						*/
/*		     DBALIST			*/
/*						*/
/************************************************/
typedef struct dba_str {
    DB_ADDR		dba;
    DtSrINT32		w_c;
    struct dba_str	*next_dba;
}               DBALIST;

/************************************************/
/*						*/
/*		     TREENODE			*/
/*						*/
/************************************************/
typedef struct _treen_ {
    char           *word;	/* ptr to word in stop list */
    struct _treen_ *llink;	/* left link in binary tree */
    struct _treen_ *rlink;	/* ptr to right link in binary tree */
    DBALIST        *dba_list;
}               TREENODE;

static TREENODE *root_node =		NULL;
static TREENODE *top_of_stack;
static TREENODE *stack;
static TREENODE *pres;
static TREENODE *prev;
static TREENODE *next;
static TREENODE *avail_node;



/************************************************/
/*						*/
/*		   displayable			*/
/*						*/
/************************************************/
/* Returns static string same as passed string except nonprintable
 * and nonascii chars replaced by '^' for display.
 */
static char	*displayable (char *passed_string)
{
    static char		*buf =		NULL;
    static size_t	buflen =	0;
    size_t		passed_len =	strlen (passed_string);
    char		*targ, *src;
    if (buflen < passed_len) {
	if (buf)
	    free (buf);
	buflen = passed_len;
	buf = austext_malloc (buflen + 4, PROGNAME"158", NULL);
    }
    targ = buf;
    for (src = passed_string;  *src != 0;  src++) {
	if (*src >= 32  && *src < 127)
	    *targ++ = *src;
	else
	    *targ++ = '^';
    }
    *targ = 0;
    return buf;
} /* displayable() */


/************************************************/
/*                                              */
/*               print_exit_code                */
/*                                              */
/************************************************/
/* Called from inside DtSearchExit() at (*austext_exit_last)() */
static void     print_exit_code (int exit_code)
{
    if(dotcount) {
	putchar ('\n');
	dotcount = 0;
    }
    /* Put total seconds into totalstart */
    if (totalstart > 0)
	totalstart = time (NULL) - totalstart;
    printf (catgets (dtsearch_catd, MS_cborodin, 206,
	"%s: Exit Code = %d, Total elapsed time %ldm %lds.\n"),
	aa_argv0, exit_code, totalstart / 60L, totalstart % 60L);
    return;
}	/* print_exit_code() */


/****************************************/
/*					*/
/*	     write_to_file()		*/
/*					*/
/****************************************/
/* This is the 'visit node' point for the tree traversal
 * functions of Pass 2 (traverse_tree() and descend_tree()).
 *
 * Each tree node = word or stem + linked list of dbas.
 * When called, each dba list member just contains the number
 * of times the token appears in that document.  This function
 * chains through the list, builds a statistical 'weight'
 * for each doc/word pair, and stores it as a reformatted 'dba'
 * in array 'record_addr_word[]', in 'host' byte swap order.
 * The count of the current number of addrs
 * in the array is stored in 'num_addrs_for_word'.
 * Fill_data1() is then called to update or write a new
 * vista record and d99 data for the token.
 *
 * The weight stored for each doc-word instance is 1 byte.
 * It's the ratio of log of number of times given word occurs in doc,
 * divided by log of total count of all words in doc,
 * scaled to range 0 to 255.
 * Fundamentally it's a word count of that word in the doc,
 * but adjusted as follows:
 * 1) Large occurrances in small documents weigh more than 
 *    the same number of occurrances in large documents.
 * 2) Taking the log skews the ratio to be more linear,
 *    ie take advantage of higher ranges of the 'weight'.
 *    For example a word that occurs in 10% of the document,
 *    will have a weight of .5 (50%).
 * 3) The scaling changes the ratio, a float between 0. and .9999,
 *    to an integer between 0 and 255.
 */
void            write_to_file (TREENODE * output_node)
{
    DBALIST	*print_dba;
    DB_ADDR	mydba;

    /* 'record_addr_word[]' was permanently allocated
     * with a size = max batch size so it can hold
     * all the addrs for a single word node in the tree.
     * In effect it will replace the dba linked list.
     * Note: word_addrs_ii (io buffer for d99 file) != record_addr_word[].
     */

    if (debugging & (DEBUG_T | DEBUG_t)) {	/* Print out tree node */
	printf (" node '%s' %c%c%c",
	    displayable(output_node->word),
	    (output_node->llink)? 'L' : '.',
	    (output_node->rlink)? 'R' : '.',
	    (debugging & DEBUG_t)? '\n' : ' ');
    }

    num_addrs_for_word = 0;	/* DtSrINT32 */
    print_dba = output_node->dba_list;
    while (print_dba != NULL) {

	mydba = print_dba->dba;
	if (debugging & DEBUG_t)
	    printf ("    dba #%ld: node adr=%ld cnt=%ld",
		(long)num_addrs_for_word, (long)mydba, (long)print_dba->w_c);

	record_addr_word [num_addrs_for_word] =
	    mydba << 8;  /* rec# in hi 3 bytes */
	record_addr_word [num_addrs_for_word] +=
	    (log ((double) (print_dba->w_c) + 0.5) /
	    log ((double) (dbas_word_count[mydba] + 1))) * 256;

	if (debugging & DEBUG_t)
	    printf ("  -> x%lx (%ld:%ld)\n",
		(long)record_addr_word [num_addrs_for_word],
		(long)record_addr_word [num_addrs_for_word] >> 8,
		(long)record_addr_word [num_addrs_for_word] & 0xffL);

	print_dba = print_dba->next_dba;
	num_addrs_for_word++;
	if (num_addrs_for_word >= batch_size) {
	    printf (catgets (dtsearch_catd, MS_cborodin, 280,
		"\n%s num_addrs_for_word (%ld) >= batchsz (%ld).\n"),
		PROGNAME"280", (long)num_addrs_for_word, (long)batch_size);
	    DtSearchExit (91);
	}
    }
    if ((debugging & DEBUG_T)  && !(debugging & DEBUG_t))
	printf (" dbacnt=%ld\n", (long)num_addrs_for_word);

    fill_data1 (output_node->word);

    return;
} /* write_to_file() */


/****************************************/
/*					*/
/*	     descend_tree()		*/
/*					*/
/****************************************/
/* Coroutine of traverse_tree(), Pass 2 Robson tree traversal.
 * The write_to_file() function is the 'preorder visit' point.
 */
void            descend_tree (void)
{
    int             not_done = TRUE;

    while (not_done) {
	if ((pres->llink == NULL) && (pres->rlink == NULL)) {
	    write_to_file (pres);
	    avail_node = pres;
	    return;
	}
	if (pres->llink != NULL) {
	    next = pres->llink;
	    pres->llink = prev;
	    prev = pres;
	    pres = next;
	}
	else {
	    write_to_file (pres);
	    next = pres->rlink;
	    pres->rlink = prev;
	    prev = pres;
	    pres = next;
	}
    }
    return;
} /* descend_tree() */


/********************************/
/*				*/
/*	  traverse_tree		*/
/*				*/
/********************************/
/* This is the actual Pass 2 function, a tree traversal
 * of Pass 1's word-dba binary tree.
 * The algorithm is based on the J. M. ROBSON link inversion traversal
 * algorithm for binary trees. Ref. Thomas A. STANDISH  pp. 77-78.
 * The write_to_file() function is the 'preorder visit' point.
 */
void            traverse_tree (void)
{
    int             not_done = TRUE;
    int             descend = TRUE;

    /* Dheck for the empty tree */
    if (root_node == NULL) {
	printf (catgets (dtsearch_catd, MS_cborodin, 288,
	    "%s Abort. There are no words in the input file %s.\n"),
	    PROGNAME"288", fname_input);
	DtSearchExit (34);
    }
    /* Initialize the variables */
    pres = root_node;
    prev = pres;
    top_of_stack = NULL;
    stack = NULL;

    while (not_done) {
	if (descend) {
	    descend_tree ();
	}
	if (pres == root_node) {
	    return;
	}
	if (prev->rlink == NULL) {
	    write_to_file (prev);
	    next = prev->llink;
	    prev->llink = pres;
	    pres = prev;
	    prev = next;
	    descend = FALSE;
	}
	else {
	    if (prev->llink == NULL) {
		next = prev->rlink;
		prev->rlink = pres;
		pres = prev;
		prev = next;
		descend = FALSE;
	    }
	    else {
		if (prev == top_of_stack) {
		    next = stack;
		    top_of_stack = stack->rlink;
		    stack = stack->llink;
		    next->llink = NULL;
		    next->rlink = NULL;
		    next = prev->llink;
		    prev->llink = prev->rlink;
		    prev->rlink = pres;
		    pres = prev;
		    prev = next;
		    descend = FALSE;
		}
		else {
		    write_to_file (prev);
		    avail_node->llink = stack;
		    avail_node->rlink = top_of_stack;
		    stack = avail_node;
		    top_of_stack = prev;
		    next = prev->rlink;
		    prev->rlink = pres;
		    pres = next;
		    descend = TRUE;
		}
	    }
	}
    }
} /* traverse_tree() */



/********************************************************/
/*							*/
/*	           print_usage_msg			*/
/*							*/
/********************************************************/
static void     print_usage_msg (void)
{
                    printf (catgets (dtsearch_catd, MS_cborodin, 17,
"\n"
"USAGE: %s -d<dbname> [options] <infile>\n"
"       Listed default file name extensions can be overridden.\n"
"  -d<dbname>  1 - 8 character database name, include optional path prefix.\n"
"  -t<etxstr>  End of text document delimiter string.  Default '\\f\\n'.\n"
"  -r<N>       Change Pass 1 records-per-dot from %d to <N>.\n"
"  -b<N>       Change max batch size from %ld to <N>.\n"
"  -c<N>       Change database paging cache from %ld 1K pages to <N> 1K pages.\n"
"              <N> >= 16 by powers of 2.  Initially try only small changes.\n"
"  -i<N>       Change (i)nput buffer size from default %d to <N>.\n"
"  -h<N>       Change duplicate record id hash table size from %ld to <N>.\n"
"              -h0 means there are no duplicates, do not check for them.\n"
"  <infile>    Input [path]file name.  Default extension %s.\n"),
	aa_argv0,
	(int) RECS_PER_DOT,
	(long) BATCH_SIZE,  (long) CACHE_SIZE,
	(int) INBUFSZ,  default_hashsize,  EXT_FZKEY);
    return;
} /* print_usage_msg() */


/********************************************************/
/*							*/
/*	          segregate_dicname			*/
/*							*/
/********************************************************/
/* Separates dictionary name from pathname and loads
 * them into the globals 'dicname' and 'dicpath'.
 * Returns TRUE if dicname is valid, else returns FALSE.
 */
static int      segregate_dicname (char *string)
{
    char            mybuf[_POSIX_PATH_MAX];
    char           *ptr;
    int             i;

    strncpy (mybuf, string, sizeof (mybuf));
    mybuf[sizeof (mybuf) - 1] = 0;

    /*
     * Set 'ptr' to just the 8 char dictionary name by moving
     * it backwards until first non-alphanumeric character
     * (such as a ":" in the dos drive id or a slash between directories),
     * or to the beginning of string.
     */
    for (ptr = mybuf + strlen (mybuf) - 1; ptr >= mybuf; ptr--)
	if (!isalnum (*ptr)) {
	    ptr++;
	    break;
	}
    if (ptr < mybuf)
	ptr = mybuf;

    /* test for valid dictionary name */
    i = strlen (ptr);
    if (i < 1 || i > 8)
	return FALSE;

    strcpy (dicname, ptr);
    *ptr = 0;
    strncpy (dicpath, mybuf, sizeof (dicpath));
    dicpath[sizeof (dicpath) - 1] = 0;
    return TRUE;
} /* segregate_dicname() */


/********************************************************/
/*							*/
/*	           USER_ARGS_PROCESSOR			*/
/*							*/
/********************************************************/
/* handles command line arguments for 'main' */
void            user_args_processor (int argc, char **argv)
{
    char           *argptr;
    char           *targ, *src;
    int             i;

    if (argc <= 1) {
	print_usage_msg ();
	DtSearchExit (2);
    }
    /* Initialize some variables prior to parsing command line */
    dicname[0] = 0;
    dicpath[0] = 0;

    /* Each pass grabs new parm of "-xxx" format */
    while (--argc > 0 && (*++argv)[0] == '-') {
	argptr = argv[0];
	switch (argptr[1]) {

	    case 't':		/* ETX delimiter string */
		/* Replace any "\n" string with real linefeed */
		targ = parg.etxdelim = malloc (strlen (argptr + 2) + 4);
		src = argptr + 2;
		while (*src) {
		    if (src[0] == '\\' && src[1] == 'n') {
			*targ++ = '\n';
			src += 2;
		    }
		    else
			*targ++ = *src++;
		}
		*targ = 0;
		break;

	    case 'r':
		if ((recs_per_dot = atoi (argptr + 2)) <= 0) {
		    printf (catgets (dtsearch_catd, MS_cborodin, 577,
			"%s Invalid arg '%s'.  Using default -r%d.\n"),
			PROGNAME"577", argptr, RECS_PER_DOT);
		    recs_per_dot = RECS_PER_DOT;
		}
		break;

	    case 'h':
		duprec_hashsize = atol (argptr + 2);
		if (duprec_hashsize == 0UL)
		    printf (catgets (dtsearch_catd, MS_cborodin, 539,
			"%s Duplicate record id checking disabled.\n"),
			PROGNAME"539");
		break;

	    case 'b':
		batch_size = atol (argptr + 2);
		if (batch_size <= 0L) {
		    printf (catgets (dtsearch_catd, MS_cborodin, 595,
			"%s Invalid batch size argument '%s'.\n"),
			PROGNAME"595", argptr);
		    goto BADPARM;
		}
		break;

	    case 'c':
		cache_size = atoi (argptr + 2);
		if (cache_size < 16) {
		    /* minimum size is 16 */
		    if (cache_size > 0)
			cache_size = 16;
		    /* on error reset size to default */
		    else
			cache_size = CACHE_SIZE;
CACHE_ADJUSTED:
		    printf (catgets (dtsearch_catd, MS_cborodin, 600,
			    "%sCache size readjusted to %d.\n"),
			PROGNAME "600 ", cache_size);
		    break;
		}
		/* If necessary, round up to nearest power of 2 */
		for (i = 4; i < 12; i++)
		    if (1 << i >= cache_size)
			break;
		i = 1 << i;
		if (i != cache_size) {
		    cache_size = i;
		    goto CACHE_ADJUSTED;
		}
		break;

	    case 'D':		/* unadvertised debugging feature */
		for (i = 2;  argptr[i] != 0;  i++) {
		    switch (argptr[i]) {
			case 'I':	debugging |= DEBUG_I;  break;
			case 'P':	debugging |= DEBUG_P;
				/******* debugging_teskey = TRUE; ******/
					break;
			case 'N':	debugging |= DEBUG_N;  break;
			case 'n':	debugging |= DEBUG_n;  break;
			case 'O':	debugging |= DEBUG_O;  break;
			case 'o':	debugging |= DEBUG_o;  break;
			case 'T':	debugging |= DEBUG_T;  break;
			case 't':	debugging |= DEBUG_t;  break;
			default:	goto BADPARM;
		    }
		}
		break;

	    case 'd':
		/* May include both dicname and dicpath */
		if (!segregate_dicname (argptr + 2)) {
		    printf (catgets (dtsearch_catd, MS_cborodin, 550,
			"%s '%s' is invalid path/database name.\n"),
			PROGNAME"550", argptr);
		    goto BADPARM;
		}
		break;

	    case 'i':		/* (I)nput buffer size */
		if ((inbufsz = atol (argptr + 2)) <= 0) {
		    printf (catgets (dtsearch_catd, MS_cborodin, 558,
			"%s Invalid input buffer size '%s'.\n"),
			PROGNAME"558", argptr);
		    goto BADPARM;
		}
		break;

	    default:
		printf (catgets (dtsearch_catd, MS_cborodin, 567,
		    "%s Unknown command line argument '%s'.\n"),
		    PROGNAME"567", argptr);
BADPARM:
		print_usage_msg ();
		DtSearchExit (2);	/* abort */

	}			/* endswitch */
    }				/* endwhile for cmd line '-'processing */

    /* Validate input file name */
    if (argc-- <= 0) {
	printf (catgets (dtsearch_catd, MS_cborodin, 580,
	    "%s Missing required input file name.\n"),
	    PROGNAME"580");
	goto BADPARM;
    }
    /* Don't incr argv yet--save input file name */
    else
	append_ext (fname_input, _POSIX_PATH_MAX, argv[0], EXT_FZKEY);

    /* Check for missing database name */
    if (dicname[0] == 0) {
	printf (catgets (dtsearch_catd, MS_cborodin, 589,
	    "%s No database name specified (-d argument).\a\n"),
	    PROGNAME"589");
	goto BADPARM;
    }
    strcpy (dblk.name, dicname);
    dblk.path = dicpath;
    return;
} /* user_args_processor() */


/****************************************/
/*					*/
/*	put_addrs_2_dtbs_addr_file	*/
/*					*/
/****************************************/
/* Suboutine of write_2_dtbs_addr_file() from Pass 2.
 * That function has used a bit vector to determine
 * the total change in old d99 addrs for preexisting words,
 * and prepared for writing an array of old dbas that
 * are not in the current words tree node (globally named
 * word_addrs_ii [num_addrs]).
 * The addrs that ARE in the Pass 1 node fzk file were previously
 * prepared in a similar array of dbas, globally named
 * record_addr_word [num_addrs_for_word] but passed here as
 * 'addrs_array' and 'nitems'.
 * Both arrays will be byte swapped from 'host' to
 * 'network' order in this function.
 * This function does the actual fwrite of both arrays to the d99.
 * If the number of new addrs can fit in the available free slots,
 * it rewrites to original offset, otherwise appends to end of d99.
 */
static void	put_addrs_2_dtbs_addr_file (
		    DB_ADDR	*addrs_array,
		    DtSrINT32	nitems)
{
    FREE_SPACE_STR	*free_slot;
    FREE_SPACE_STR	del_rec;
    DtSrINT32		int32;
    DtSrINT32		num_writes;
    DtSrINT32		num_addrs;

    if (nitems >= batch_size) {
	printf ( catgets(dtsearch_catd, MS_cborodin, 6,
	    "put_addrs_2_dtbs_addr_file() nitems=%d, batchsz=%ld\n") ,
	    (int)nitems, (long)batch_size);
	DtSearchExit (58);
    }

    num_addrs = got_word.or_hwaddrs;
    got_word.or_hwaddrs += nitems;  /** somehow, this can exceed total
	**** num addrs in database by 1 (!?) ******/
	/* (...only if prev 'overlay/compression' didn't delete all) */

#ifdef BYTE_SWAP
	/* Put both arrays in 'network' byte order */
	for (int32 = 0;  int32 < nitems;  int32++)
	    HTONL (addrs_array[int32]);
	for (int32 = 0;  int32 < num_addrs;  int32++)
	    HTONL (word_addrs_ii[int32]);
#endif

    /*
     * If number of new addresses greater than number of free holes,
     * find new free slot that is big enough to hold the data .
     */
    if (nitems > got_word.or_hwfree) {
	/* Discard old slot, find new one. */
	del_rec.hole_size = num_addrs + got_word.or_hwfree;
	del_rec.offset = got_word.or_hwoffset;
	free_slot = find_free_space (got_word.or_hwaddrs, &fl_hdr);
	add_free_space (&del_rec, &fl_hdr);
	if (free_slot == NULL) {
	    fseek (dtbs_addr_fp, 0L, SEEK_END);
	    got_word.or_hwoffset = ftell (dtbs_addr_fp);
	    got_word.or_hwfree = 0;
	}
	else {
	    fseek (dtbs_addr_fp, free_slot->offset, SEEK_SET);
	    got_word.or_hwoffset = free_slot->offset;
	    got_word.or_hwfree = free_slot->hole_size -
		got_word.or_hwaddrs;
	}
	/*----- Write new database addresses to a file -----*/
	num_writes = fwrite (addrs_array, sizeof(DB_ADDR),
		(size_t)nitems, dtbs_addr_fp);
	if (num_writes != nitems) {
	    DtSearchExit (98);
	}

	/* Copy the old addresses immediately after the new ones */
	num_writes = fwrite (word_addrs_ii, sizeof(DB_ADDR), (size_t)num_addrs,
	    dtbs_addr_fp);
	if (num_writes != num_addrs) {
	    printf (catgets (dtsearch_catd, MS_cborodin, 776, msg_776),
		PROGNAME"776", strerror(errno));
	    DtSearchExit (76);
	}

	/* Write foxes to the free holes, if any, no byte swap */
	for (int32 = 0;  int32 < got_word.or_hwfree;  int32++)
	    addrs_array [int32] = 0xFFFFFFFF;
	num_writes = fwrite (addrs_array, sizeof(DB_ADDR),
	    (size_t)got_word.or_hwfree, dtbs_addr_fp);
	if (num_writes != got_word.or_hwfree) {
	    printf (catgets (dtsearch_catd, MS_cborodin, 776, msg_776),
		PROGNAME"786", strerror(errno));
	    DtSearchExit (86);
	}
    } /* end if (nitems > got_word.or_hwfree), had to get bigger slot */

    /* Else can reuse existing slot.
     * Write the new addresses into free holes.
     * The remaining free holes should already have foxes. (?)
     */
    else {
	fseek (dtbs_addr_fp, got_word.or_hwoffset, SEEK_SET);
	num_writes = fwrite (addrs_array, sizeof(DB_ADDR),
		(size_t)nitems, dtbs_addr_fp);
	if (num_writes != nitems) {
	    printf (catgets (dtsearch_catd, MS_cborodin, 776, msg_776),
		PROGNAME"798", strerror(errno));
	    DtSearchExit (87);
	}
	/* Copy the old addresses immediately after the new ones */
	num_writes = fwrite (word_addrs_ii, sizeof(DB_ADDR),
		(size_t)num_addrs, dtbs_addr_fp);
	if (num_writes != num_addrs) {
	    printf (catgets (dtsearch_catd, MS_cborodin, 776, msg_776),
		PROGNAME"889", strerror(errno));
	    DtSearchExit (89);
	}
	got_word.or_hwfree -= nitems;
    }
} /* put_addrs_2_dtbs_addr_file() */


/****************************************/
/*					*/
/*	 write_2_dtbs_addr_file		*/
/*					*/
/****************************************/
/* Subroutine of fill_data1() from Pass 2.
 * Updates OLD (preexisting) word's d99 file.
 *
 * The vista word rec has already been read into global 'got_word'.
 * record_addr_word [num_addrs_for_word] is the array of dba's
 * for docs from this batch that contain the current word (built by
 * fill_data1 from the dba_list for the word's Pass 1 binary tree node,
 * and still in 'host' byte swap order).
 * This function freads all the old addresses for that word from
 * the d99 file.  It then deletes(!) d99 addrs that
 * are in the word's Pass 1 tree node.  It then calls
 * put_addrs_2_dtbs_addr_file() to fwrite out the  
 * dba's in the tree, which are either brand new,
 * or are 'updating' the deleted addrs.
 * Then it writes the modified old addrs.
 * Then rewrites vista word rec with new data.
 *
 * The bit vector dbas_bits_batch contains a 1 bit
 * for every dba for every doc in the fzk file.
 * got_word structure:
 * .or_hwordkey - the word. (always in a 'huge' word buffer).
 * .or_hwoffset - offset in a d99 inverted index file for
 * 		      a given word. the first address starts
 * 		      at this position.
 * .or_hwaddrs - total number of addresses for a given word. 
 * .or_hwfree - number of free slots in a database
 *  			 addresses file for a given word.
 */
void            write_2_dtbs_addr_file (void)
{
    DtSrINT32		num_addrs_ii;
    DtSrINT32		num_reads;
    DtSrINT32		i_start, k, cur_ind;
    DtSrINT32		num_delete_addrs = 0;
    char		addrs_removed = FALSE;
    register DtSrINT32	i;
    register DtSrINT32	cur_byte;
    register char	bit_addrs;
    register DB_ADDR	temp1;

    if (debugging & DEBUG_O)
	printf ("  old vis '%s' ofs=%ld adr=%ld fre=%ld\n",
	    displayable(got_word.or_hwordkey),
	    (long) got_word.or_hwoffset,
	    (long) got_word.or_hwaddrs,
	    (long) got_word.or_hwfree);

    num_addrs_ii = got_word.or_hwaddrs;
    if (num_addrs_ii > or_reccount) {
	printf (catgets (dtsearch_catd, MS_cborodin, 713,
	    "\n%s Word '%s' occurs in %ld records,\n"
	    "  but there are only %ld records in database!\n"
	    "  (This may be a good candidate for the stoplist).\n"),
	    PROGNAME"713",
	    (long) got_word.or_hwordkey,
	    (long) num_addrs_ii,
	    (long) or_reccount);
	DtSearchExit (68);
    }

    if (fseek (dtbs_addr_fp, (long) got_word.or_hwoffset, SEEK_SET) != 0)
	{
	printf (catgets (dtsearch_catd, MS_cborodin, 875,
	    "\n%s Could not fseek d99 file to offset %ld.\n"),
	    PROGNAME"875", got_word.or_hwoffset);
	DtSearchExit (98);
	}
    num_reads = fread (word_addrs_ii, sizeof(DB_ADDR),
	(size_t)num_addrs_ii, dtbs_addr_fp);
    if (num_reads != num_addrs_ii) {
	printf (catgets (dtsearch_catd, MS_cborodin, 848,
	    "\n%s Could not fread %ld bytes (%ld dba's) of d99 file\n"
	    "  at offset %ld.  Number of dba's read (return code) = %ld.\n"),
	    PROGNAME"848", sizeof(DB_ADDR) * num_addrs_ii, (long)num_addrs_ii,
	    (long)got_word.or_hwoffset, (long)num_reads);
	DtSearchExit (98);
    }
#ifdef BYTE_SWAP
    for (i = 0; i < num_addrs_ii; i++)
	NTOHL (word_addrs_ii[i]);
    /* Now both addr arrays are in 'host' byte swap order */
#endif

    /* If there are only new docs,
     * this switch will prevent the checking for updates.
     */
    if (check_existing_addrs) {
	i_start = 0;

	/* Loop on every preexisting dba for word as read from d99 */
	for (i = 0; i < num_addrs_ii; i++) {
	    if (debugging & DEBUG_o)
		printf ("  old d99 %ld: x%lx(%ld:%ld)",
		    (long) i,
		    (long) word_addrs_ii[i],
		    (long) word_addrs_ii[i] >> 8,
		    (long) word_addrs_ii[i] & 0xffL);

	    /* Get 'record number' by shifting hi 3 bytes 1 byte (8 bits)
	     * to right over stat wt byte.  D99 rec#'s start at 1,
	     * so subtract 1 to start at 0 for bit vector.
	     */
	    temp1 = (*(word_addrs_ii + i) >> 8) - 1;	/* = rec#, base 0 */
	    cur_byte = temp1 >> 3;	/* get matching byte# in bit vector */
	    if (cur_byte >= bit_vector_size) {
		printf ( catgets(dtsearch_catd, MS_cborodin, 9,
		    "\n%s Corrupted d99 file for word '%s',\n"
		    " database address %ld @ file position %ld => bitvector[%ld],"
		    " but max bitvector allocation = %ld.\n") ,
		    PROGNAME"727", displayable(got_word.or_hwordkey),
		    (long)temp1, (long)i,
		    (long)cur_byte, (long)bit_vector_size);
		DtSearchExit (69);
	    }
	    bit_addrs = 0;
	    bit_addrs |= 1 << (temp1 % 8);	/* bit mask */
	    /*
	     * If this dba, which is on the current word's old d99
	     * addrs list, is also a doc in the fzk file (dbas_bits_batch),
	     * delete it from the d99 list by writing subsequent dba's
	     * over it.  Boy this recursive nested loop has gotta be slow.
	     * Faster algorithm?  Add 'good' addrs to the end of
	     * record_addr_word[].  No nested overlay loop, only one write!
	     */
	    if (bit_addrs & (*(dbas_bits_batch + cur_byte))) {
		addrs_removed = TRUE;
		num_delete_addrs++;
		if (i_start == 0) {
		    cur_ind = i;
		    i_start = i + 1;
		}
		else {
		    if (i_start < i) {
			/* compress: move good addrs over
			 * space of deleted ones */
			for (k = i_start; k < i; k++) {
			    word_addrs_ii[cur_ind] = word_addrs_ii[k];
			    cur_ind++;
			}
		    }
		    i_start = i + 1;
		}
	    } /* end if where dba is on both fzk list and curr d99 */ 
	} /* end loop on every d99 addr for this word */

	if (addrs_removed) {	/* final overlay compression */
	    if (i_start < i) {
		/* compress: move good addrs over
		 * space of deleted ones */
		for (k = i_start; k < i; k++) {
		    word_addrs_ii[cur_ind] = word_addrs_ii[k];
		    cur_ind++;
		}
	    }
	}
    } /* end if (check_existing_addrs) */

    got_word.or_hwaddrs -= num_delete_addrs;
    got_word.or_hwfree += num_delete_addrs;

    /* The old dba array word_addrs_ii[] is now 'compressed',
     * it contains only addrs not in fzk file.
     * And the vista rec 'got_word' now matches it.
     * And record_addr_word[] still contains
     * the new/updated addrs from the fzk file.
     * Now Efim calls a func to write them both back out to d99 file.
     */
    put_addrs_2_dtbs_addr_file (record_addr_word, num_addrs_for_word);
    write_wordstr (&got_word, 0);	/* update vista WORD rec */

    return;
} /*  write_2_dtbs_addr_file() */


/********************************/
/*				*/
/*	write_new_word_2_dtbs	*/
/*				*/
/********************************/
/* Subroutine of fill_data1() in Pass 2 for a NEW word.
 * Writes d99 data, and updates (empty) got_word vista record.
 * record_addr_word [num_addrs_for_word] is the array of addrs
 * for docs from this batch that contain the current word (built by
 * fill_data1 from the dba_list for the word's Pass 1 binary tree node).
 * It will be byte swapped from 'host' to 'network' order in this function.
 */
void            write_new_word_2_dtbs (void)
{
    FREE_SPACE_STR *free_slot;
    DtSrINT32	num_writes;
    int             ret_fseek;
    DtSrINT32	int32;

    if (debugging & (DEBUG_n  | DEBUG_N))
	printf ("  new word '%s', adrs=%ld,",
	    got_word.or_hwordkey, (long)num_addrs_for_word);

    free_slot = find_free_space (num_addrs_for_word, &fl_hdr);
    if (free_slot == NULL) {
	/* append addrs to end of d99 file */
	ret_fseek = fseek (dtbs_addr_fp, 0L, SEEK_END);
	got_word.or_hwoffset = ftell (dtbs_addr_fp);
	got_word.or_hwfree = 0;
	if (debugging & (DEBUG_n  | DEBUG_N))
	    printf ("APPEND ofs=%ld, fre=0\n", (long int) got_word.or_hwoffset);
    }
    else {
	ret_fseek = fseek (dtbs_addr_fp,
		(long)free_slot->offset, SEEK_SET);
	got_word.or_hwoffset = free_slot->offset;
	got_word.or_hwfree = free_slot->hole_size -
	    num_addrs_for_word;
	if (debugging & (DEBUG_n  | DEBUG_N))
	    printf (" REUSE slot ofs=%ld, fre=%ld\n",
		(long int) got_word.or_hwoffset, (long int) got_word.or_hwfree);
    }

    /***** Write new database addresses to d99 file *********/
    if (debugging & DEBUG_n) {
	for (int32 = 0;  int32 < num_addrs_for_word;  int32++) {
	    printf ("     dba #%ld: x%lx(%ld:%ld)\n",
		(long)int32,
		(long)record_addr_word[int32],
		(long)record_addr_word[int32] >> 8,
		(long)record_addr_word[int32] & 0xffL);
	}
    }
#ifdef BYTE_SWAP
	/* Put addr array in 'network' byte order */
	for (int32 = 0;  int32 < num_addrs_for_word;  int32++)
	    HTONL (record_addr_word[int32]);
#endif
    num_writes = fwrite (record_addr_word, sizeof(DB_ADDR),
	(size_t)num_addrs_for_word, dtbs_addr_fp);
    if (num_writes != num_addrs_for_word)
	DtSearchExit (97);

    got_word.or_hwaddrs = num_addrs_for_word;

    if (got_word.or_hwfree != 0) {
	/* Fill unused free holes with foxes for debugging.
	 * Note that byte swap is unnecessary for foxes.
	 * Note that record_addr_word is now available for this action.
	 */
	for (int32 = 0;  int32 < got_word.or_hwfree;  int32++)
	    *(record_addr_word + int32) = 0xFFFFFFFF;
	num_writes = fwrite (record_addr_word, sizeof(DB_ADDR),
	    (size_t)got_word.or_hwfree, dtbs_addr_fp);
	if (num_writes != got_word.or_hwfree) {
	    printf (catgets (dtsearch_catd, MS_cborodin, 776, msg_776),
		PROGNAME"960", strerror(errno));
	    DtSearchExit (96);
	}
    }

    /* Save changed word_info structure back to the vista database! */
    write_wordstr (&got_word, 0);
    return;
} /* write_new_word_2_dtbs() */


/************************/
/*			*/
/*	fill_data1	*/
/*			*/
/************************/
/* Called from write_to_file() in Pass 2.
 * Write_to_file() is 'visit node' function of tree traversal.
 * It has converted dbalist in each word node in tree to
 * array of dbas (record_addr_word [num_addrs_for_word])
 * with correct statistical weighting, still in 'host' byte swap order.
 * This function seeks word key in database.  If word is new,
 * it calls functions to write new vista rec and d99 data.
 * If word is old it calls functions to read word rec and update d99.
 */
void            fill_data1 (char *node_word)
{
    char            miker[1024];
    strcpy (miker, node_word);

    count_word_ii++;
    if (shutdown_now) {
	printf (catgets (dtsearch_catd, MS_cborodin, 164,
	    "\n%s Abort due to signal %d.  Database %s\n"
	    "  probably corrupted.  Restore backup database.\n"),
	    PROGNAME"164", shutdown_now, dicname);
	DtSearchExit (10);
    }

    /* print occasional progress dots and msgs */
    if (!(count_word_ii % words_per_dot)) {
	putchar ('.');
	dotcount++;
	if (!(dotcount % 10))
	    putchar (' ');
	if (dotcount >= 50) {
	    dotcount = 0;
	    seconds_left = (unsigned long)
		(((float) num_of_diff_words /
		    (float) count_word_ii - 1.) *
		(float) (time (NULL) - timestart));
	    printf (catgets (dtsearch_catd, MS_cborodin, 849,
		"\n%s: Word #%ld, %.0f%% done.  Est %lum %02lus "
		"to completion.\n"),
		aa_argv0, count_word_ii,
		(float) count_word_ii / (float) num_of_diff_words * 100.0,
		/***(count_word_ii * 100L) / num_of_diff_words,***/
		seconds_left / 60L, seconds_left % 60L);
	}
	else
	    fflush (stdout);
    }	/* endif for progress dots and msgs */

    strncpy (got_word.or_hwordkey, node_word, DtSrMAXWIDTH_HWORD);
    got_word.or_hwordkey[DtSrMAXWIDTH_HWORD - 1] = 0;
    find_keyword (miker, 0);	/* vista KEYFIND for word rec */
    if (db_status == S_NOTFOUND) {	/* this is a NEW word */
	got_word.or_hwoffset = 0;
	got_word.or_hwfree = 0;
	got_word.or_hwaddrs = 0;
	fillnew_wordrec (&got_word, 0);	/* write (empty) vista word rec */
	if (db_status != S_OKAY)
	    vista_abort (PROGNAME"981");
	write_new_word_2_dtbs();	/* write NEW word's d99 entries
					 * and update vista word rec */
	return;
    }

    /* update previously existing word */
    read_wordstr (&got_word, 0);	/* read OLD word rec into got_word */
    if (db_status == S_OKAY)
	write_2_dtbs_addr_file();	/* update OLD word's d99 entries
					 * and update vista word rec */
    return;
}	/* fill_data1() */


/************************************************/
/*						*/
/*		load_into_bintree		*/
/*						*/
/************************************************/
/* Pass 1 function.
 * Loads parsed word token or stem token into
 * inverted index binary tree along with passed dba.
 * Token is allowed to be empty, ie first byte is \0.
 * Derived from Efim's original 'teskey_parse()'
 * and bin_tree() functions.
 * Variables static for speeeeeeed.
 */
static void	load_into_bintree (
			char	*parser_token,
			int	token_is_stem,
			DB_ADDR	dba)
{
    static DtSrINT16	or_maxwordsz;
    static char		*cptr;
    static int		i;
    static TREENODE	**this_link;
    static TREENODE	*newnode;
    static DBALIST	*newdba;
    static char		*tokbuf =	NULL;

    if (*parser_token == 0) {
	if (debugging & DEBUG_I)
	    printf (" bintr=<empty> dba=%ld\n", (long)dba);
	return;
    }

    /* Copy token to a buffer.
     * Stems have a special prefix character
     * to distinguish them from words.
     * Also increment total dba word count.
     */
    if (tokbuf == NULL) {
	or_maxwordsz = dblk.dbrec.or_maxwordsz;
	tokbuf = austext_malloc ((size_t) or_maxwordsz + 4,
	    PROGNAME"1152", NULL);
    }
    if (token_is_stem) {
	tokbuf[0] = STEM_CH;
	strncpy (tokbuf + 1, parser_token, (size_t)or_maxwordsz);
	dbas_word_count[dba]++;
    }
    else
	strncpy (tokbuf, parser_token, (size_t)or_maxwordsz);
    tokbuf [or_maxwordsz] = 0;
    if (debugging & DEBUG_I)
	printf (" bintr='%s' dba=%ld ", displayable(tokbuf), (long)dba);

    /* TREE TRAVERSAL.  Search binary tree to find either
     * insertion point or identical preexisting token.
     */
    for (this_link = &root_node; *this_link != NULL; ) {
	i = strcmp (tokbuf, (*this_link)->word);

	/* If identical word/stem token already exists... */
	if (i == 0) {
	    /* If token appears more than once in current
	     * document (dba already exists at top of dba list),
	     * just increment the word count in the list.
	     */
	    if ((*this_link)->dba_list->dba == dba)
		(*this_link)->dba_list->w_c++;
		
	    /* If this is first appearance of token for this doc
	     * (dba is not at start of token's dba list),
	     * insert dba at start of token's dba list.
	     */
	    else {
		if ((newdba = malloc (sizeof(DBALIST))) == NULL) {
		    printf (catgets (dtsearch_catd, MS_cborodin, 374,
			msg_374), PROGNAME"1150");
		    DtSearchExit (26);
		}
		newdba->dba =		  dba;
		newdba->w_c =		  1;
		newdba->next_dba =	  (*this_link)->dba_list;
		(*this_link)->dba_list =  newdba;
	    }
	    if (debugging & DEBUG_I)
		printf (" Old %ld=%ld\n",
		    (long)((*this_link)->dba_list->dba),
		    (long)((*this_link)->dba_list->w_c));
	    return;	/* done with token */

	} /* endif where token was found in binary tree */

	/* Increment link ptr by descending to correct subtree */
	if (i < 0) {
	    this_link = &(*this_link)->llink;
	    if (debugging & DEBUG_I)
		putchar ('L');
	}
	else {
	    this_link = &(*this_link)->rlink;
	    if (debugging & DEBUG_I)
		putchar ('R');
	}
    } /* end tree traversal */

    /* Tree traversal never found a preexisting token node.
     * Create a new node and insert it at the point
     * indicated by link ptr.
     */
    newnode = austext_malloc (sizeof(TREENODE) + strlen(tokbuf) + 4,
	PROGNAME"1234", NULL);
    newnode->llink =	NULL;
    newnode->rlink =	NULL;
    newnode->word = (char *) (newnode + 1);	/* use mem at end of node */
    strcpy (newnode->word, tokbuf);

    newdba = austext_malloc (sizeof(DBALIST), PROGNAME"1235", NULL);
    newnode->dba_list =	newdba;
    newdba->dba =	dba;
    newdba->w_c =	1;
    newdba->next_dba =	NULL;

    *this_link =	newnode;
    num_of_diff_words++;

    if (debugging & DEBUG_I)
        printf (" New %ld=%ld\n",
	    (long)((*this_link)->dba_list->dba),
	    (long)((*this_link)->dba_list->w_c));
    return;
} /* load_into_bintree() */


/**********************************************/
/*                                            */
/*                    MAIN                    */
/*                                            */
/**********************************************/
main (int argc, char **argv)
{
    int			i;
    long		word_offset;	/* <-- PARG.offsetp */
    long		bytes_in;	/* ftell() */
    DtSrINT32		dba_offset;
    int			got_ETX;
    char		*cptr, *src;
    char		temp_buf[40];
    char		db_key [DtSrMAX_DB_KEYSIZE + 2];
    int			oops = FALSE;
    register DtSrINT32	cur_byte;
    struct tm		*tmptr;
    DB_ADDR		dba, temp_dba;
    time_t		elapsed;
    size_t		mallocsz;
    char		*parsebufp, *stembufp;

    /******************* INITIALIZE ******************/
    setlocale (LC_ALL, "");
    dtsearch_catd = catopen (FNAME_DTSRCAT, 0);

    aa_argv0 = strdup (argv[0]);
    time (&elapsed);
    tmptr = localtime (&elapsed);
    strftime (buf, sizeof(buf),
	catgets (dtsearch_catd, MS_misc, 22, "%A, %b %d %Y, %I:%M %p"),
	tmptr);
    printf (catgets (dtsearch_catd, MS_cborodin, 1, "%s.  Run %s.\n"),
	aa_argv0, buf);
    austext_exit_last = print_exit_code;
    batch_size = BATCH_SIZE;
    init_user_interrupt ();
    default_hashsize = duprec_hashsize;

    memset (&dblk, 0, sizeof(DBLK));

    memset (&parg, 0, sizeof(PARG));
    parg.dblk =		&dblk;
    parg.etxdelim =	ETXDELIM;	/* default, can be changed */
    parg.offsetp =	&word_offset;
    parg.flags |=	PA_INDEXING;	/* do compounding, if parser can */

    /* Read user specified command line arguments */
    user_args_processor (argc, argv);

    /* Finish init now that we know final values */
    inbuf = austext_malloc (inbufsz + 16, PROGNAME"1349", NULL);
    temp = austext_malloc (inbufsz + 16, PROGNAME"1285", NULL);
    sprintbuffer = austext_malloc (inbufsz + _POSIX_PATH_MAX + 16,
	PROGNAME"1286", NULL);
    record_addr_word = austext_malloc ((sizeof(DB_ADDR) * batch_size) + 16,
	PROGNAME "1133", NULL);

    /* Save dicname and path in dblk.  Save full name of d99 file. */
    strcpy (dblk.name, dicname);
    dblk.path = dicpath;
    strcpy (dtbs_addr_file, dicpath);
    strcat (dtbs_addr_file, dicname);
    strcat (dtbs_addr_file, EXT_DTBS);

    /* Open the database */
    if (!austext_dopen (dicname, dicpath, NULL, cache_size, &dbrec)) {
	fprintf (aa_stderr, "%s\n", DtSearchGetMessages());
	DtSearchExit (3);
    }
    memcpy (&dblk.dbrec, &dbrec, sizeof(DBREC));

    /* Load database's parser, stemmer, and linguistic files into dblk. */
    if (!load_language (&dblk, NULL)) {
	puts (DtSearchGetMessages());
	printf (catgets (dtsearch_catd, MS_cborodin, 1097,
	    "%s Aborting due to errors in loading language files.\n"),
	    PROGNAME"1097");
	DtSearchExit(3);
    }

    RECFRST (PROGNAME "1067", OR_OBJREC, 0);
    CRGET (PROGNAME "1069", &dba, 0);  /* byte swap already done in vista */

    or_reccount = dbrec.or_reccount;	/* DtSrINT32 */
    or_recslots = dbrec.or_recslots;	/* promoted to DtSrINT32 */
    or_maxdba = dbrec.or_maxdba;	/* DtSrINT32 lim of dbas_word_count */
    bit_vector_size = ((or_maxdba / or_recslots + 1) >> 3) + 1; /* DtSrINT32 */
    dba_offset = or_recslots - (dba & 0x00FFFFFF);	/* DtSrINT32 */

    if (debugging)
	printf (PROGNAME"1286 "
	    "realnumrec=%ld recslots=%ld bitvecsz=%ld"
	    " dbaoffset=%d maxdba=%ld\n",
	    (long)or_reccount, (long)or_recslots, (long)bit_vector_size,
	    (int)dba_offset, (long)or_maxdba);

    /* Allocate memory space for the arrays.
     * dbas_bits_batch = 'bit vector', one bit for every possible rec#.
     *   the 1 bits = only the dba's that are in this fzk batch.
     * word_addrs_ii = fread buffer for d99 file.
     * dbas_word_count = summing bkts for word count statistics.
     */
    dbas_bits_batch = (char *) austext_malloc ((size_t)bit_vector_size + 48,
	PROGNAME "1150", NULL);
    word_addrs_ii = (DB_ADDR *) austext_malloc (
	sizeof (DB_ADDR) * (or_reccount + 1) + 48,
	PROGNAME "1152", NULL);
    mallocsz = sizeof(DtSrINT32) * (or_maxdba + 1) + 48;
    dbas_word_count = (DtSrINT32 *) austext_malloc (mallocsz,
	PROGNAME "1154", NULL);
    memset (dbas_bits_batch, 0, (size_t)bit_vector_size + 48);
    memset (dbas_word_count, 0, mallocsz);

    root_node = NULL;

   /* Open the d99 file that contains database addresses.
    * If the file doesn't exist, it means the database
    * for keyword search is empty - open a new file.
    */
    if ((dtbs_addr_fp = fopen (dtbs_addr_file, "r+b")) == NULL) {
	dtbs_addr_fp = fopen (dtbs_addr_file, "w+b");
	check_existing_addrs = FALSE;
	new_dtbs_file = TRUE;
	if (dtbs_addr_fp == NULL) {
	    /* msg 1068 used multiple places */
	    printf (catgets (dtsearch_catd, MS_cborodin, 1068,
		"%s Can't open new inverted index file '%s': %s\n"),
		PROGNAME"1068", dtbs_addr_file, strerror(errno));
	    DtSearchExit (13);
	}
	/* write New Header Information to a file */
	init_header (dtbs_addr_fp, &fl_hdr);
    }
    else {
	/* read Header Information from d99 file */
	if (!fread_d99_header (&fl_hdr, dtbs_addr_fp)) {
	    /* msg 1068 used multiple places */
	    printf (catgets (dtsearch_catd, MS_cborodin, 1068,
		"%s Can't read header data for '%s': %s\n"),
		PROGNAME"1422", dtbs_addr_file, strerror(errno));
	    DtSearchExit (13);
	}
    }

    /* open input .fzk file */
    src = getcwd (sprintbuffer, _POSIX_PATH_MAX);
    if (!src && debugging)
	printf (PROGNAME"1336 Can't getcwd: %s.\n", strerror(errno));
    if (!src)
	src = getenv ("PWD");
    printf (catgets (dtsearch_catd, MS_misc, 24,
	"%s: current working directory = '%s', .fzk file = '%s'\n"),
	aa_argv0,
	(src) ? src : catgets (dtsearch_catd, MS_misc, 6, "<unknown>"),
	fname_input);
    if ((instream = fopen (fname_input, "rt")) == NULL) {
BAD_INPUT_FILE:
	printf (catgets (dtsearch_catd, MS_cborodin, 1083,
	    "%s Can't read input file '%s': %s\n"),
	    PROGNAME"1083", fname_input, strerror(errno));
	DtSearchExit (14);
    }
    if (fstat (fileno (instream), &fstat_input) == -1)
	goto BAD_INPUT_FILE;
    parg.ftext = instream;	/* for readchar_ftext(), discard_to_ETX() */

    time (&totalstart);		/* for total elapsed time */
    timestart = totalstart;	/* for Pass 1 elapsed time */

    /*------------ PASS 1:  ------------
     * Main Read Loop.  For each text record in input file,
     * parse and stem words, store them into binary tree
     * inverted index in memory.
     * The first few lines are database administrative values.
     * They are presumed ascii and read with fgets() as
     * 'lines' terminated with \n.  The text of the document
     * itself is presumed to be in the appropriate database
     * 'language', so it is *not* presumed to be lines
     * terminated with \n.  The document text is read by
     * the language's parser() a 'word' at a time, which
     * ultimately means a byte at a time.
     */
    printf (catgets (dtsearch_catd, MS_cborodin, 1108,
	"%s: Beginning Pass 1, reading records from '%s'.\n"
	"   Each dot = %d records.\n"),
	aa_argv0, fname_input, recs_per_dot);
    dotcount = 0;

    while (!feof(instream)) {

	/* 1. Read and discard the FZKEY line.
	 * 2. Read and discard the ABSTRACT line.
	 * 3. Read the UNIQUE KEY for the record.
	 *    Do some record initialization steps here.
	 * 4. Read and discard the DATE line.
	 * 5. Let the parser read and parse rest of record, ie doc text...
	 */

	/*----- READ LINE #1, fzkey -----*/
        if (fgets (inbuf, inbufsz, instream) == NULL)
	    break;
	inbuf [inbufsz] = 0;	/* just to be sure */

	if (shutdown_now) {
	    printf (catgets (dtsearch_catd, MS_cborodin, 164,
		"\n%s: %s Abort due to signal %d.  Database %s\n"
		"  possibly corrupted.  Restore backup database.\n"),
		aa_argv0, PROGNAME"1299", shutdown_now, dicname);
	    DtSearchExit (11);
	}

	/* Silently skip null records just like dtsrload */
	if (strcmp (inbuf, parg.etxdelim) == 0)
	    continue;

	record_count++;

	/*----- READ LINE #2, abstract -----*/
	if (fgets (inbuf, inbufsz, instream) == NULL) {
INVALID_FZK_FORMAT:
	    printf (catgets (dtsearch_catd, MS_cborodin, 1129,
		"%s: %s Invalid .fzk file format.\n"),
		fname_input, PROGNAME"1129");
	    DtSearchExit (22);
	}
	inbuf[inbufsz] = 0;	/* just to be sure */

	/*--- READ LINE #3, unique database key ---*/
	if (fgets (inbuf, inbufsz, instream) == NULL)
	    goto INVALID_FZK_FORMAT;
	inbuf[inbufsz] = 0;	/* just to be sure */

	if ((cptr = strtok (inbuf, " \t\n")) == NULL)
	    goto INVALID_FZK_FORMAT;

	/* If necessary, discard long keys exactly like cravel */
	if (strlen (cptr) >= DtSrMAX_DB_KEYSIZE) {
	    printf (catgets (dtsearch_catd, MS_cborodin, 659,
		"\n%s: %s Discarding record, key too long:\n  '%s'.\n"),
		aa_argv0, PROGNAME"659", cptr);
	    discard_to_ETX (&parg);
	    continue;
	}
	strcpy (db_key, cptr);

	/* Skip duplicate record ids in same order as dtsrload */
	i = is_duprec (db_key);
	if (i == 2) {	/* out of memory */
	    printf (catgets (dtsearch_catd, MS_cborodin, 374, msg_374),
		    PROGNAME"1317");
	    DtSearchExit (57);
	}
	else if (i == 1) {	/* duplicate record id */
	    duplicate_recids++;
	    if (dotcount > 0)
		    putchar ('\n');
	    printf (catgets (dtsearch_catd, MS_cborodin, 1402,
		"%s: Discarded duplicate rec #%lu '%s'.\n"),
		aa_argv0, record_count, db_key);
	    discard_to_ETX (&parg);
	    continue;
	}

	/****** FFFFFFFFFFFFFFFFFFFFF **********/
	/* Convert database address (slot #) to 'record number',
	 * what dba would have been if all records took up
	 * only one slot and there were no dbrec at top of file.
	 * Record numbers on d99, like dba's, start at #1,
	 * but rec numbers here (in bit vector) start at #0.
	 */
	KEYFIND (PROGNAME "222", OR_OBJKEY, (char *) db_key, 0);
	if (db_status != S_OKAY) {
	    normal_retncode = 1;	/* = 'warning' */
	    if (dotcount > 0)
		putchar ('\n');
	    printf (catgets (dtsearch_catd, MS_cborodin, 1168,
		"%s: %s Discarded '%s', key not in database.\n"),
		aa_argv0, PROGNAME"1168", displayable(db_key));
	    discard_to_ETX (&parg);
	    continue;
	}

	CRGET (PROGNAME "224", &temp_dba, 0); /* vista already byte swapped */
	temp_dba &= 0x00FFFFFF;	/* = slot# */
	dba = (temp_dba + dba_offset) / or_recslots; /* = rec#, base 1 */
	/*
	 * Don't change this 'dba'!--eventually it goes
	 * into d99 in this exact format!  It will also
	 * be used as an index into dbas_word_count[] in
	 * load_into_bintree() so do a sanity check
	 * to make sure that it hasn't exceeded the size
	 * of that array.  (The count increments have been
	 * reported as as 'uninitialized memory reads'
	 * by a debugger).  This might happen for example
	 * if user failed to run dtsrload before dtsrindex?
	 */
	if (dba < 1  ||  dba > or_maxdba) {
	    printf ( catgets(dtsearch_catd, MS_cborodin, 21,
		"\n%s '%s' record overflows word counter array.\n"
		"Record number %ld > maxdba %ld, dba=%ld, "
		"recslots=%ld, offs=%d.\n") ,
		PROGNAME"1526", displayable(db_key),
		(long)dba, (long)or_maxdba, (long)temp_dba,
		(long)or_recslots, (int)dba_offset);
	    DtSearchExit (68);
	}
	temp_dba = dba - 1;	/* = rec# starting at 0 */
	cur_byte = temp_dba >> 3;	/* bits to bytes: div by 8 */
	if (cur_byte >= bit_vector_size) {
	    printf ( catgets(dtsearch_catd, MS_cborodin, 22,
		"\n%s '%s' record in database (dba=%ld)\n"
		"  overflows bitvector allocation (%ld >= %ld).\n") ,
		PROGNAME"1475", displayable(db_key), (long)dba,
		(long)cur_byte, (long)bit_vector_size);
	    DtSearchExit (69);
	}
	dbas_bits_batch[cur_byte] |= 1 << (temp_dba % 8);

	/* Print occasional progress dots and msgs */
	if (!(record_count % recs_per_dot)) {
	    putchar ('.');
	    dotcount++;
	    if (!(dotcount % 10))
		putchar (' ');
	    if (dotcount >= 50) {
		dotcount = 0;
		bytes_in = ftell (instream);
		seconds_left = (unsigned long)
		    (((float) fstat_input.st_size /
		    (float) bytes_in - 1.) *
		    (float) (time (NULL) - timestart));
		printf (catgets (dtsearch_catd, MS_cborodin, 1190,
		    "\n%s: Rec #%lu, %.0f%% done.  "
		    "Est %lum %02lus to end Pass 1.\n"),
		    aa_argv0,
		    record_count,
		    (float) bytes_in / (float) fstat_input.st_size * 100.0,
		    seconds_left / 60UL,
		    seconds_left % 60UL);
	    }
	    fflush (stdout);
	}

	/*----- READ LINE #4, date -----*/
	if (fgets (inbuf, inbufsz, instream) == NULL)
	    goto INVALID_FZK_FORMAT;
	inbuf[inbufsz] = 0;	/* just to be sure */

	/* PARSE LOOP FOR CURRENT TEXT BLOCK.
	 * We must be in the middle of a record ('lines' #5 and beyond).
	 * From here to ETX, which is either the record delimiter string
	 * or the end of file, read the file a 'word' at a time
	 * using the parse() function for the language specified
	 * for the database.
	 * Load_into_bintree() stores each token into
	 * inverted index binary tree.
	 * Note: dba here MUST still be rec#, base 1.
	 * It's stored as is by load_into_bintree(),
	 * and will be moved as is into d99 file in Pass 2.
	 */
	if (debugging & DEBUG_P)
	    printf ("\nRecord #%lu '%s'\n"
		    "Offset Word----               Stem----\n",
		record_count, db_key);
	for (	cptr = dblk.parser (&parg);
		cptr;
		cptr = dblk.parser (NULL)) {

	    if (debugging & DEBUG_P) {
		printf ("%6ld %s %n", word_offset, cptr, &i);
		if (!(debugging & DEBUG_I))
		    while (i++ < 30)
			putchar (' ');
	    }
	    load_into_bintree (cptr, FALSE, dba);
	    cptr = dblk.stemmer (cptr, &dblk);
	    if (debugging & DEBUG_P) {
		printf ("%s\n", cptr);
		fflush (stdout);
	    }
	    load_into_bintree (cptr, TRUE, dba);
	}

    } /* end of PASS 1 Main read loop */

    elapsed = time(NULL) - timestart;
    if (dotcount > 0) {
	putchar ('\n');
	dotcount = 0;
    }
    if (duplicate_recids > 0L) {
	normal_retncode = 1;	/* 'warning' */
	sprintf (buf, catgets (dtsearch_catd, MS_cborodin, 40,
	    "Ignored %ld duplicate records"),
	    duplicate_recids);
    }
    else
	strcpy (buf, catgets (dtsearch_catd, MS_cborodin, 41,
	    "No duplicate records found"));
    printf (catgets (dtsearch_catd, MS_cborodin, 1225,
	"%s: Pass 1 completed in %lum %lus, read %lu records.\n"
	"  %s, parsed %lu words.\n"),
	aa_argv0, elapsed / 60L, elapsed % 60L, record_count,
	buf, num_of_diff_words);
    if (record_count > batch_size) {
	printf (catgets (dtsearch_catd, MS_cborodin, 33,
	    "\n%s Number of incoming records exceeded %d.\n"
	    "  This will usually result in 'Out of Paging Space' "
	    "error in Pass 2\n"
	    "  and corruption of database.  Either split the incoming file to\n"
	    "  reduce record count or use the -b option, and rerun.\n"),
	    PROGNAME"33", (int)batch_size);
	DtSearchExit (33);
    }

    /*----------------- PASS 2:  -----------------
     * Traverse completed binary tree and write it to d99 file.
     */
    printf (catgets (dtsearch_catd, MS_cborodin, 1233,
	"%s: Beginning Pass 2: batch index traversal and database update.\n"
	"  Each dot = %d words.\n"),
	aa_argv0, words_per_dot);
    dotcount = 0;
    time (&timestart);
    traverse_tree (); 	/* actual Pass 2 */
    if (dotcount) {
	putchar ('\n');
	dotcount = 0;
    }

    /* Write header information to the d99 file */
    if (!fwrite_d99_header (&fl_hdr, dtbs_addr_fp)) {
	printf (catgets (dtsearch_catd, MS_cborodin, 776, msg_776),
	    PROGNAME"1723", strerror(errno));
	DtSearchExit (13);
    }
    d_close ();
    fclose (dtbs_addr_fp);

    elapsed = time (NULL) - timestart;
    printf (catgets (dtsearch_catd, MS_cborodin, 1246,
	"%s: Pass 2 completed in %lum %lus, updated %lu words.\n"),
	aa_argv0, elapsed / 60L, elapsed % 60L, count_word_ii);
    if (normal_retncode == 1)
	printf (catgets (dtsearch_catd, MS_cborodin, 2,
	    "%s: Warnings were detected.\n"), aa_argv0);
    DtSearchExit (normal_retncode);

} /* main() */

/*************************** DTSRINDEX.C ****************************/

