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
 *   FUNCTIONS: build_tree
 *		char_label
 *		huffman_code
 *		init_treebase
 *		main
 *		next_sorted_node
 *		print_usage
 *		strrev
 *		user_args_processor
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1990,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/************************** HUFFCODE.C ******************************
 * $XConsortium: huffcode.c /main/9 1996/11/14 15:31:05 rcs $
 * 12/90.
 * Counts frequency of occurrance of every possible byte value of input text.
 * Creates Huffman Code Table based on byte frequencies and writes it
 * in 2 formats to 2 different output files.
 * The encode table (.huf) maintains the frequency counts and explicitly
 * includes the huffman code strings.  Generally speaking, the .huf file
 * is intended for humans to read.  The decode table (.c) is an array
 * of integers meant to be compiled into an object module, then linked
 * into the decode program.  The .c format closely resembles the original 
 * huffman code tree in this program.
 * By keeping the tree as an obscure array of integers,
 * the huffman code can double as an encryption technique,
 * and the decoding method kept somewhat proprietary.
 * 
 * For a good discussion of Huffman codes and related algorithms,
 * see "Data Compression Techniques and Applications Hardware and
 * Software Considerations" by Gilbert Held and Thomas R. Marshall.
 * The tree itself is balanced to minimize longest bitstring length
 * per Eugene Schwartz, Information and Control 7, 37-44 (1964).
 * 
 * At beginning of each new execution, the program tries to
 * open the .huf table file and continue byte frequency counting
 * from the last run.
 * If the .huf file doesn't exist, the table's counts are
 * initialized to zeroes.  The .c decode table is recomputed fresh
 * each run, whether it existed before or not.
 * 
 * If the input file is not specified then the frequencies in the table
 * are not changed, and the huffman codes are recomputed with the
 * existing frequencies.
 * 
 * THIS PROGRAM DOES NOT CHECK .HUF FILE FORMAT!--it had better be correct.
 * 
 * HUFFMAN ENCODE TABLE (.huf) FILE FORMAT:
 * Each line represents each possible byte value (0 - 255),
 * the huffman 'literal' character (#256), or comments.
 * There are exactly 257 lines sorted by decreasing count.
 * There are four fields, each separated by one or more tabs (\t).
 * 
 * 1.  CHARACTER.  a number from 0 to 256.
 * 
 * 	The 'character' represented by the number 256 is the literal.
 * 	it represents all characters whose frequency is so low that
 * 	there is no huffman code translation--this reduces the max
 * 	length of the coded bit string when there are lots of zero
 * 	or low frequency bytes in the input.  For example,
 *	pure ascii text files only occasionally have byte values
 *	less than 32 (control chars) and rarely greater than 127
 *	(high order bit turned on).
 * 
 * 2.  HUFFMAN CODE.  a string of binary digits (0's and 1's).
 * 	Each string is unique to that character.
 * 	This field will consist of a single blank, when the character
 * 	will be coded by the huffman literal.  If the code of
 * 	the literal itself is blank, then literal coding is
 * 	not used in this table--all characters are represented
 * 	by complete huffman code strings.
 * 
 * 3.  COUNT.  The number of times this character appeared in the text.
 * 	The literal's count equals the sum of the counts of all the
 * 	real characters which are represented by the literal.
 * 	The literal's count may be 0 if all the characters it
 * 	represents have zero frequencies.
 * 
 * 4.  COMMENTS.  A label depicting the printable char or its description.
 * 
 * HUFFMAN DECODE TABLE (.c) FILE FORMAT:
 * A sequence of integers formatted as a C array of integer pairs
 * and intended to be compiled and linked into the decode program.
 * Each huffman tree node contains two integers.
 * The root of the tree is the LAST integer pair.
 * The first (left) integer contains the array index down the '0' branch,
 * the right integer points down the '1' branch.
 * However if an integer is negative,  the decoding ceases and the
 * resulting plaintext character is the negative integer + 257,
 * and will always be in the range 0 - 255, or 256 for the literal code.
 *
 * $Log$
 * Revision 2.3  1996/03/25  18:55:04  miker
 * Changed FILENAME_MAX to _POSIX_PATH_MAX.
 *
 * Revision 2.2  1995/10/25  17:50:34  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  20:46:28  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.4  1995/09/19  22:04:11  miker
 * Print out nonascii chars in .huf file comments.
 *
 * Revision 1.3  1995/09/05  18:08:00  miker
 * Name changes for DtSearch.
 */
#include "SearchP.h"
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <sys/stat.h>
#include <stdlib.h>

#define MS_huff		30	/* message catalog set number */
#define DELIMITERS	"\t\n"	/* betw fields in .huf file */
#define LAST_BIT	'-'
#define MAX_BITLEN	24
#define MAX_NODES	514
 /*
  * 256 chars + 'literal' char = max leaves, therefore max treesize
  * = 2n - 1 = 513 
  */

/*----------------------- HCTREE --------------------------*/
/* tree is also a table so tree ptrs are table indexes:
 * 0 - 255 =	characters themselves (leaves at base of tree).
 *     256 =	literal code (special char/leaf).
 *   > 256 =	higher nodes.
 * 		Global 'last_node' = highest actual node alloc so far.
 * 		When tree completed, last_node = root of tree.
 *      -1 =	null links.
 */
typedef struct {
    char            bit;	/* '0' or '1' (assoc with link to
				 * father) */
    long            count;	/* freq of occurrance of char */
    int             sort;	/* specifies output sort order */
    int             father;	/* index points UP toward root of
				 * tree */
    int             son0;	/* index of '0' (left) subnode */
    int             son1;	/* index of '1' (right) subnode */
}               HCTREE;

/*------------------------ GLOBALS ---------------------------*/
static int      last_node = 256;
long            total_count;
long            literal_threshold = 0L;
int             debug_switch = FALSE;
int             literal_coding_on = TRUE;
int             input_file_specified;	/* TRUE if user enters
					 * filename */
int             no_huffcode_file;	/* TRUE if table file not
					 * found */
HCTREE          hctree1[MAX_NODES];
char            filename_input[_POSIX_PATH_MAX];
char            filename_huf[_POSIX_PATH_MAX];
char            filename_huc[_POSIX_PATH_MAX];

#ifndef TURBO_COMPILER
/****************************************/
/*					*/
/*	         strrev			*/
/*					*/
/****************************************/
static char    *strrev (char *string)
{
    int             i;
    int             j;
    char            temp;

    for (i = 0, j = strlen (string) - 1; i < j; i++, j--) {
	temp = string[i];
	string[i] = string[j];
	string[j] = temp;
    }
    return string;
}

#endif	/* !TURBO_COMPILER */

/****************************************/
/*					*/
/*	       Build Tree		*/
/*					*/
/****************************************/
/* Each call joins the two nodes with smallest count
 * into a single higher level node.  If there are more than 2 nodes with
 * similar 'smallest' counts, then within that group the 2 nodes with the
 * shortest current bitstring length are joined.
 * Returns TRUE for each successful lower level join.
 * Returns FALSE when final join is made at highest level (root).
 */
static int      build_tree (void)
{
    int             i, j;
    int             low0 = -1;
    int             low1 = -1;
    int             len0 = 0;
    int             len1 = 0;
    int             curr;

    /* find 2 lowest counts */
    for (i = 0; i < 257; i++) {
	/* skip over real chars with counts <= 'literal' threshold */
	if (literal_coding_on
	    && i != 256
	    && hctree1[i].count <= literal_threshold) {
	    hctree1[i].sort = MAX_BITLEN + 1;
	    continue;
	}

	/* skip over literal if literal coding turned off */
	if (i == 256 && !literal_coding_on) {
	    hctree1[256].sort = MAX_BITLEN + 1;
	    continue;
	}

	/*
	 * Ascend to highest tree level for current table entry,
	 * putting length of bitstring into sort field. Save
	 * highest tree level in curr. 
	 */
	hctree1[i].sort = 0;
	for (j = i; j != -1; j = hctree1[j].father) {
	    hctree1[i].sort++;
	    curr = j;
	}

	/*
	 * sanity checks after ascending tree: 1. if bit strings
	 * have grown too large, quit. 2. if curr points to top
	 * tree level, quit. 
	 */
	if (hctree1[i].sort > MAX_BITLEN) {
	    fprintf (stderr, "%s", catgets(dtsearch_catd, MS_huff, 30,
		"\n183 Bit strings have grown too large.  You probably "
		"have literals\n  turned off with grossly unbalanced "
		"character counts.\n\7"));
	    exit (2);
	}
	if (hctree1[curr].count >= total_count) {
	    fprintf (stderr, "%s", catgets(dtsearch_catd, MS_huff, 31,
		"\n191 Programming Error:  Still trying to build\n"
		"  Huffman Code Tree after root created.\n\7"));
	    exit (2);
	}

	/*
	 * if curr ptr already joins low0 or low1, try the next
	 * table entry 
	 */
	if (curr == low0 || curr == low1)
	    continue;

	/*
	 * If curr count is less than low0, or if curr count = low0
	 * but curr bitstring length is less, replace both low0 and
	 * low1. (that way, we keep low0 always <= low1) 
	 */
	if (low0 == -1 || hctree1[curr].count < hctree1[low0].count ||
	    (hctree1[curr].count == hctree1[low0].count && hctree1[i].sort < len0)) {
	    low1 = low0;
	    len1 = len0;
	    low0 = curr;
	    len0 = hctree1[i].sort;
	    continue;
	}

	/*
	 * At this point curr count is 'greater' than low0. If curr
	 * count is less than low1, or if curr count = low1 but
	 * curr bitstring length is less, replace only low1 
	 */
	if (low1 == -1 || hctree1[curr].count < hctree1[low1].count ||
	    (hctree1[curr].count == hctree1[low1].count && hctree1[i].sort < len1)) {
	    low1 = curr;
	    len1 = hctree1[i].sort;
	    continue;
	}

	/*
	 * default: curr count is greater than BOTH low0 and low1,
	 * try next table entry 
	 */
    }	/* end loop to find two lowest counts */

    /* low0 and low1 now point to two lowest count nodes.
     * link in low0 and low1 to next available new node.
     */
    last_node++;
    hctree1[low0].bit = '0';
    hctree1[low0].father = last_node;
    hctree1[low1].bit = '1';
    hctree1[low1].father = last_node;
    hctree1[last_node].bit = LAST_BIT;
    hctree1[last_node].father = -1;
    hctree1[last_node].count = hctree1[low0].count + hctree1[low1].count;
    hctree1[last_node].son0 = low0;
    hctree1[last_node].son1 = low1;

    if (debug_switch)
	printf ("%3d: low0=%6ld\tlow1=%6ld\tsum=%6ld\t(%ld)\n",
	    last_node, hctree1[low0].count, hctree1[low1].count,
	    hctree1[last_node].count, total_count);

    if (hctree1[last_node].count < total_count)
	return TRUE;
    else
	return FALSE;
}  /* end of function build_tree */

/****************************************/
/*					*/
/*	       Char Label		*/
/*					*/
/****************************************/
static char    *char_label (int x)
{
    static char     buf[64];

    switch (x) {
	case 0:
	    return "NULL";
	case 8:
	    return "\\b (backspace)";
	case 9:
	    return "\\t (tab)";
	case 10:
	    return "\\n (linefeed)";
	case 11:
	    return "\\v (vert tab)";
	case 12:
	    return "\\f (form feed)";
	case 13:
	    return "\\r (carr retn)";
	case 26:
	    return "CTRL-Z (EOF)";
	case 27:
	    return "CTRL-[ (ESC)";
	case 31:
	    return "CTRL-dash";
	case 32:
	    return "SPACE (blank)";
	case 45:
	    return "- (dash)";
	case 95:
	    return "_ (underscore)";
	case 127:
	    return "DEL";
	case 256:
	    return "*** LITERAL CODE ***";
	default:
	    if (x > 256)
		return "";
	    else if (x < 32) {
		sprintf (buf, "'CTRL-%c'", 0x40 | x);
		return buf;
	    }
	    else if (x >= 128) {
		strcpy (buf, catgets(dtsearch_catd, MS_huff, 32,
		    "(nonascii char, high bit set)"));
		return buf;
	    }
	    else {
		sprintf (buf, "'%c'", x);
		return buf;
	    }
    }
}  /* end of function char_label */

/****************************************/
/*					*/
/*	     Next Sorted Node		*/
/*					*/
/****************************************/
/* Called repeatedly, returns the next treebase node in sorted order.
 * Sort order is by length of Huffman Code String.
 * Caller must pass index of last node returned (neg at first call).
 * Lasti should never be larger than treebase.
 */
static int      next_sorted_node (int lasti)
{
    int             i;
    int             nexti = -1;
    long            nextsortval = MAX_BITLEN + 2;

    /* permanently mark last returned node as unavailable */
    if (lasti >= 0)
	hctree1[lasti].sort = MAX_BITLEN + 2;

    /* find next shortest string length */
    for (i = 0; i < 257; i++)
	if (hctree1[i].sort < nextsortval) {
	    nextsortval = hctree1[i].sort;
	    nexti = i;
	}
    return nexti;
}  /* end of function next_sorted_node */

/****************************************/
/*					*/
/*	   Initialize Treebase		*/
/*					*/
/****************************************/
/* 'Treebase' is original 257 character nodes (including literal code).
 * If huffcode table file exists, initializes treebase with its values,
 * else initializes treebase with zero counts.
 */
static void     init_treebase (void)
{
    int             i;
    FILE           *instream_huf;
    char            filebuf[128];

    total_count = 0L;

    /* .huf table file does not exist--zero all counts */
    if ((instream_huf = fopen (filename_huf, "r")) == NULL) {
	no_huffcode_file = TRUE;
	for (i = 0; i < 257; i++) {
	    hctree1[i].bit = LAST_BIT;
	    hctree1[i].count = 0L;
	    hctree1[i].father = -1;
	    hctree1[i].son0 = -1;
	    hctree1[i].son1 = -1;
	}
    }

    /* Table file exists--init treebase with values from file.
     * We are only interested in the character itself (i),
     * and its current count.  All other fields will be recreated
     * at output time.  FILE FORMAT IS NOT CHECKED--IT HAD BETTER BE CORRECT!
     */
    else {
	no_huffcode_file = FALSE;
	fgets (filebuf, sizeof (filebuf) - 1, instream_huf);
	/* discard this first line (don't need id stamp) */
	while (fgets (filebuf, sizeof (filebuf) - 1, instream_huf)
	    != NULL) {
	    i = atoi (strtok (filebuf, DELIMITERS));	/* char */
	    if (i < 0 || i > 256) {
		fprintf (stderr, catgets(dtsearch_catd, MS_huff, 33,
		    "366 Invalid file format for %s.\n"),
		    filename_huf);
		exit (2);
	    }
	    strtok (NULL, DELIMITERS);	/* skip over current huff
					 * code */
	    hctree1[i].count = (i == 256) ?
		0L : atol (strtok (NULL, DELIMITERS));
	    hctree1[i].bit = LAST_BIT;
	    hctree1[i].father = -1;
	    hctree1[i].son0 = -1;
	    hctree1[i].son1 = -1;
	    if (i != 256)
		total_count += hctree1[i].count;
	}	/* endwhile loop that reads each table line */
	fclose (instream_huf);
    }
    return;
}  /* end of function init_treebase */

/****************************************/
/*					*/
/*	      Huffman Code		*/
/*					*/
/****************************************/
/* determines correct huffman code based on current counts in tree,
 * writes out all to both files overlaying previous values if they existed.
 */
static void     huffman_code (time_t idstamp)
{
    int             i;	/* current char */
    int             lasti;
    int             j;	/* ascends tree from i to build bit_string */
    char            bit_string[MAX_BITLEN + 4];
    char            sprintbuf[128];
    char           *bitptr;
    FILE           *outstream_huc;
    FILE           *outstream_huf;

    /* establish the 'literal' node (char #256) count
     * equal to sum of all chars whose counts are less than threshold.
     */
    if (literal_coding_on) {
	hctree1[256].count = 0L;
	for (i = 0; i < 256; i++)
	    if (hctree1[i].count <= literal_threshold)
		hctree1[256].count += hctree1[i].count;
    }

    /* build the Huffman Code tree, and determine root (last_node) */
    while (build_tree ());

    /* now that we know the total number of tree nodes (last_node),
     * we are ready to write.  
     * Open both output files and verify they are not write protected.
     */
    if ((outstream_huc = fopen (filename_huc, "w")) == NULL) {
	fprintf (stderr, catgets(dtsearch_catd, MS_huff, 34,
	    "424 File '%s' failed to open for write.  Is it read-only?\n"),
	    filename_huc);
	exit (2);
    }
    if ((outstream_huf = fopen (filename_huf, "w")) == NULL) {
	fprintf (stderr, catgets(dtsearch_catd, MS_huff, 34,
	    "439 File '%s' failed to open for write.  Is it read-only?\n"),
	    filename_huf);
	exit (2);
    }

    /* create the .c decode file (tree as integer array) */
    fprintf (outstream_huc,
	"#include <time.h>\n"
	"char   *hctree_name =\t\"%s\";\n"
	"time_t hctree_id =\t%ldL;\n"
	"int    hctree_root =\t%d;\n"
	"static int hctree_array[] = {\n",
	filename_huc, idstamp, last_node - 257);
    for (i = 257; i <= last_node; i++) {
	fprintf (outstream_huc, "\t%4d,\t%4d%c\t/* %3d */\n",
	    hctree1[i].son0 - 257, hctree1[i].son1 - 257,
	    (i == last_node) ? ' ' : ',',	/* no comma after last
						 * one */
	    i - 257);	/* comment contains node number */
    }
    fprintf (outstream_huc, "\t};\nint *hctree =\thctree_array;\n");
    fclose (outstream_huc);

    /* write out the tree base (0-256) in sorted order to .huf file */
    fprintf (outstream_huf, "%ld\tHCTREE_ID\n", idstamp);
    for (lasti = -1; (i = next_sorted_node (lasti)) >= 0; lasti = i) {
	/*
	 * Create huffman code digit string. j ascends tree from i
	 * to build string in reverse order. 
	 */
	bitptr = bit_string;
	for (j = i; j != -1; j = hctree1[j].father)
	    *bitptr++ = hctree1[j].bit;
	*bitptr = '\0';	/* terminate reversed string */
	strrev (bit_string);	/* reverse the string order */
	if (bit_string[1] == 0)
	    strcpy (bit_string, "  ");
	if (strlen (bit_string) < 9)
	    strcat (bit_string, "\t");

	/* write out the line for this char */
	sprintf (sprintbuf, "%d\t%s\t%ld\t%s\n",
	    i,
	    bit_string + 1,	/* hop over LAST_BIT */
	    hctree1[i].count,
	    char_label (i));
	fprintf (outstream_huf, "%s", sprintbuf);

    }	/* end forloop printing out each tree base entry */

    fclose (outstream_huf);
    return;
}  /* end of function huffman_code */

/****************************************/
/*					*/
/*	       Print Usage		*/
/*					*/
/****************************************/
static void     print_usage (void)
{
    fprintf (stderr, catgets(dtsearch_catd, MS_huff, 35,
"USAGE: huffcode [-lN | -l-] [-o] <huffname> [<infile>]\n"
"  -l<N> specifies the 'literal' threshold count.  Any character occurring\n"
"       <= <N> times will be coded with the Huffman literal.  Default is -l0,\n"
"       literal coding only for bytes with counts of zero.\n"
"  -l-  turns off literal coding.  Turning off literal coding in unbalanced\n"
"       trees leads to EXTREMELY LONG bit string codes--don't do it unless\n"
"       the input is known to be a well balanced binary file.\n"
"  -o   preauthorizes overwriting any currently existing decode file.\n"
"  <huffname> is the filename prefix for the Huffman Code files.\n"
"       If the encode file (%s) already exists, byte counts from infile will\n"
"       be added to it, otherwise it will be newly created.\n"
"       The decode file (%s) is always newly created each run.\n"
"  <infile>   is an input file containing bytes to be counted.\n"
"       It may be omitted if the encode file already exists.\n"),
	EXT_HUFFCODE, EXT_HDECODE);
    return;
}  /* end of function print_usage */

/********************************************************/
/*							*/
/*	           USER_ARGS_PROCESSOR			*/
/*							*/
/********************************************************/
/* handles command line arguments for 'main' */
static void     user_args_processor (int argc, char **argv)
{
    char           *argptr;
    int             OK_to_overwrite = FALSE;
    FILE           *stream;

    if (argc <= 1) {	/* user just wants to see usage msg */
	print_usage ();
	exit (1);
    }

    /* each pass grabs new parm of "-xxx" format */
    while (--argc > 0 && (*++argv)[0] == '-') {
	argptr = argv[0];
	argptr[1] = tolower (argptr[1]);
	switch (argptr[1]) {
	    case 'l':	/* literal threshold */
		if (argptr[2] == 0)
		    goto BADARG;
		else if (argptr[2] == '-')
		    literal_coding_on = FALSE;
		else
		    literal_threshold = atoi (argptr + 2);
		break;

	    case 'o':	/* OK_to_overwrite .c file if it already
			 * exists */
		OK_to_overwrite = TRUE;
		break;

	    case 'v':	/* verbose mode = debug switch */
		debug_switch = TRUE;
		break;

	BADARG:
	    default:
		fprintf (stderr, catgets(dtsearch_catd, MS_huff, 36,
		    "'%s' is invalid argument.\n"), argptr);
		print_usage ();
		exit (2);	/* ABORT program */

	}	/* endswitch */
    }	/* endwhile for cmd line '-'processing */

    /* test for required tree file name */
    if (argc <= 0) {
	fprintf (stderr, "%s", catgets(dtsearch_catd, MS_huff, 37,
	    "576 Missing Huffman Code file names prefix.\n"));
	print_usage ();
	exit (2);
    }
    /* create 2 output file names from passed argument */
    strncpy (filename_huf, argv[0], _POSIX_PATH_MAX);
    filename_huf[_POSIX_PATH_MAX - 6] = 0;
    strcat (filename_huf, EXT_HUFFCODE);
    strncpy (filename_huc, argv[0], _POSIX_PATH_MAX);
    filename_huc[_POSIX_PATH_MAX - 6] = 0;
    strcat (filename_huc, EXT_HDECODE);

    /* Since the decode file is a C source code file (.c extension),
     * we want to be sure not to erase somebody's source program.
     * So if the .c file already exists, and the user didn't specify
     * overwrite in a command line argument, ask him now if it's OK to
     * blow away the old file.
     */
    if (!OK_to_overwrite)
	if ((stream = fopen (filename_huc, "r")) != NULL) {
	    fclose (stream);
	    printf (catgets(dtsearch_catd, MS_huff, 38,
		"Decode file '%s' already exists.  "
		"Is it OK to overwrite it? [y/n] "),
		filename_huc);
	    if (toupper (getchar ()) != 'Y')
		exit (2);
	}

    /* test for optional input file name */
    if (--argc <= 0)
	input_file_specified = FALSE;
    else {
	input_file_specified = TRUE;
	strncpy (filename_input, argv[1], _POSIX_PATH_MAX);
	filename_input[_POSIX_PATH_MAX - 1] = 0;
    }

    return;
}  /* end of function user_args_processor */

/****************************************/
/*					*/
/*		   Main			*/
/*					*/
/****************************************/
int             main (int argc, char *argv[])
{
    FILE           *instream;
    struct stat     fstat_input;
    long            bytes_in = 0L;
    int             mychar;
    time_t          now, start_stamp;

    setlocale (LC_ALL, "");
    dtsearch_catd = catopen (FNAME_DTSRCAT, 0);
    printf (catgets(dtsearch_catd, MS_huff, 40,
	"HUFFCODE Version %s\n"), AUSAPI_VERSION);

    /* validate user's command line arguments */
    user_args_processor (argc, argv);

    /* initialize tree table, using the table file if it exists */
    init_treebase ();
    if (total_count == 0L)
	printf ("%s", catgets(dtsearch_catd, MS_huff, 41,
	    "Huffman Code Tables will be newly created.\n"));
    else
	printf (catgets(dtsearch_catd, MS_huff, 42,
	    "Table '%s' already contains %ld Kbytes from previous runs.\n"),
	    filename_huf, total_count / 1000L);

    if (!input_file_specified && no_huffcode_file) {
	fprintf (stderr, catgets(dtsearch_catd, MS_huff, 43,
	    "645 Input file not specified and '%s' table file\n"
	    "   doesn't exist--nothing to do!\n"),
	    filename_huf);
	print_usage ();
	exit (2);
    }

    /* read the input file and count its bytes */
    if (input_file_specified) {
	if ((instream = fopen (filename_input, "rb")) == NULL) {
    BAD_INPUT_FILE:
	    fprintf (stderr, catgets(dtsearch_catd, MS_huff, 44,
		"Could not open input file '%s' or access status: %s\n"),
		filename_input, strerror (errno));
	    exit (2);
	}
	if (fstat (fileno (instream), &fstat_input) == -1)
	    goto BAD_INPUT_FILE;
	printf (catgets(dtsearch_catd, MS_huff, 45,
	    "Input file '%s' contains about %ld Kbytes.\n"),
	    filename_input, fstat_input.st_size / 1000L);

	time (&start_stamp);
	while ((mychar = getc (instream)) != EOF) {
	    hctree1[mychar].count++;
	    total_count++;

	    /* echo progress to user every so often */
	    if (!(++bytes_in % 10000L))
		printf (catgets(dtsearch_catd, MS_huff, 46,
		    "\r%ld%% done. %2ld Kbytes read.  "
		    "Estimate %3ld seconds to completion.   "),
		    (bytes_in * 100L) / fstat_input.st_size,
		    bytes_in / 1000L,
		    (fstat_input.st_size - bytes_in) *
		    (time (NULL) - start_stamp) / bytes_in);
	}	/* end read loop for each char in input file */

	putchar ('\n');
	fclose (instream);
    }	/* endif that processes input file */

    /* build huffman code tree, write out files */
    time (&now);	/* this will be the official tree id time
			 * stamp */
    printf (catgets(dtsearch_catd, MS_huff, 47,
	"Identifying timestamp will be '%ld'.\n"
	"%s Huffman Code Tables in '%s' and '%s'..."),
	now,
	(no_huffcode_file) ?
	    catgets(dtsearch_catd, MS_huff, 48, "Creating") :
	    catgets(dtsearch_catd, MS_huff, 49, "Rebuilding"),
	filename_huf,
	filename_huc);
    huffman_code (now);
    putchar ('\n');
    return 0;
}  /* end of function main */

/************************** HUFFCODE.C ******************************/
