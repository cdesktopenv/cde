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
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: hc_decode
 *		main
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1990,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/************************* HDECODE.C ***************************
 * $XConsortium: hdecode.c /main/8 1996/11/21 19:50:15 drk $
 * 1990.
 * Decode function that reads in a Huffman Code bitstring and returns the
 * original plaintext characters.  Intended use of Huffman Code is
 * both compression and encryption of text lines in Opera System.
 * Decode (decryption) is optimized for high speed, online use.
 * 
 * Global Arguments:
 * 'hctree' is decode table in form of array of integers.  
 *     Each integer pair forms a node in a huffman code tree.
 *     See huffcode.c for full discussion of its organization.
 *     If 'hctree' is NULL, the tree has not been linked into
 *     current object module, and this function must read the
 *     source file and create the tree at execution time.  
 *     This flexibility allows internal use of decode function 
 *     using different trees without having to recompile/relink.
 *     The linked-in version is more secure for external customer use.
 * 'hctree_name' is filename prefix for both encode (.huf) and 
 *     encode (.c) source files.  If 'hctree' is NULL, this is
 *     the file from which the tree will be built.
 * 'hctree_id' is unique unix timestamp indicating when hctree was 
 *     create.  It is compared with caller's (encoder's) passed stamp
 *     to ensure encode/decode compatibility.
 * 'hctree_root' is the integer index of the root node in hctree.
 *     By luck, this is always the LAST node so it also gives an 
 *     indication of the storage needed when the tree has to be 
 *     allocated at execution time.
 * 
 * Passed Arguments:
 * 'bitstring' is the address of the huffman encoded cyphertext.  
 *     It begins on byte boundary, but may end in middle of
 *     a byte depending on charcount.
 * 'charbuf' is the output buffer where the plaintext characters 
 *     will be assembled.
 * 'charcount' is the number of plaintext bytes that were encoded into
 *     the passed bitstring, and will be assembled into charbuf.
 *
 * $Log$
 * Revision 2.3  1996/03/13  22:56:26  miker
 * Changed char to UCHAR several places.
 *
 * Revision 2.2  1995/10/25  17:51:46  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  20:50:00  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/05  18:02:21  miker
 * Name changes for DtSearch.
 */
#include "SearchP.h"
#include <errno.h>

#define X_INCLUDE_STRING_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>

#define HDEC_FBUFSZ	128
#define PROGNAME	"HDECODE"
#define MS_huff		30	/* message catalog set number */

extern char    *hctree_name;
extern time_t   hctree_id;
extern int      hctree_root;
extern int     *hctree;

/*---------------- TREENODE structure ---------------------*/
typedef struct {
    int             branch0;
    int             branch1;
}               TREENODE;

/************************************************/
/*						*/
/*		   HC Decode			*/
/*						*/
/************************************************/
void	hc_decode (
		UCHAR	*bitstring,	/* input: compressed data */
		UCHAR	*charbuf,	/* output: uncompressed data */
		int	charcount,	/* input: length uncompressed data */
		time_t	encode_id)
{  /* input: compression table to use */
#ifdef DEBUG_DECODE
    static int      first_time = TRUE;
#endif
    int    bitreg;
    int             i;
    int             bitcount;
    int             tree_index;
    TREENODE       *tree_addr;

#ifdef EXTERNAL_TREE
    char           *ptr;
    char           *hdecode_filebuf;
    FILE           *hdecode_file;
    _Xstrtokparams  strtok_buf;
#endif

#ifdef EXTERNAL_TREE
    /* Create hctree from external file? */
    if (hctree == NULL) {
	if ((hdecode_filebuf = malloc (HDEC_FBUFSZ)) == NULL) {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_huff, 10,
		"%s Out of Memory.\n"),
		PROGNAME"076");
	    DtSearchExit (2);
	}
	if ((hdecode_file = fopen (hctree_name, "r")) == NULL) {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_huff, 11,
		"%s Cannot open tree file '%s': %s\n"),
		PROGNAME"082", hctree_name, strerror (errno));
	    DtSearchExit (2);
	}

	/* read first few lines to load global variables */
	for (i = 0; i < 3; i++)
	    fgets (hdecode_filebuf, HDEC_FBUFSZ, hdecode_file);
	ptr = strchr (hdecode_filebuf, '=');
	hctree_id = atol (ptr + 1);

	fgets (hdecode_filebuf, HDEC_FBUFSZ, hdecode_file);
	ptr = strchr (hdecode_filebuf, '=');
	hctree_root = atoi (ptr + 1);

	fgets (hdecode_filebuf, HDEC_FBUFSZ, hdecode_file);	/* throwaway */

	/* allocate space for the hctree and read in the values */
	if ((hctree = (int *) malloc (
		sizeof (int) * 2 * (hctree_root + 2))) == NULL) {
	    fprintf (aa_stderr, "\n" PROGNAME "100 Out of Memory.\7\n");
	    DtSearchExit (2);
	}
	for (i = 0; i <= hctree_root; i++) {
	    if ((fgets (hdecode_filebuf, HDEC_FBUFSZ, hdecode_file)) == NULL) {
		fprintf (aa_stderr, catgets(dtsearch_catd, MS_huff, 12,
		    "%s Invalid format hctree '%s'.\n"),
		    PROGNAME"106", hctree_name);
		DtSearchExit (2);
	    }
	    hctree[2 * i] = atoi (_XStrtok (hdecode_filebuf, " \t,", strtok_buf));
	    hctree[2 * i + 1] = atoi (_XStrtok (NULL, " \t,", strtok_buf));
	}
	free (hdecode_filebuf);
	fclose (hdecode_file);
    }	/* endif where hctree created from external file */
#endif	/* for EXTERNAL_TREE */

#ifdef DEBUG_DECODE
    if (first_time) {
	first_time = FALSE;
	printf ("\n***** created hctree from '%s' ******\n"
	    "hctree_id = %ld\nhctree_root = %d\n",
	    hctree_name, hctree_id, hctree_root);
    }
#endif


    if (encode_id != hctree_id) {
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_huff, 13,
	    "%s Incompatible hctree_ids.\n"),
	    PROGNAME"118");
	DtSearchExit (2);
    }
    tree_addr = (TREENODE *) hctree;
    bitcount = 0;
    while (charcount-- > 0) {	/****** MAIN OUTPUT CHARACTER LOOP ******/
	tree_index = hctree_root;
	while (tree_index >= 0) {	/****** TREE TRAVERSAL LOOP ******/
	    /* retrieve next bit */
	    if (bitcount <= 0) {	/* read next input char? */
		bitcount = 8;
		bitreg = *bitstring++;
	    }
	    bitreg <<= 1;
	    bitcount--;
	    if (bitreg & 0x0100)
		tree_index = tree_addr[tree_index].branch1;
	    else
		tree_index = tree_addr[tree_index].branch0;
	}	/* end tree traversal loop */

	/******** DECODE CHARACTER ********/
	/* if literal code, retrieve next 8 bits as char itself */
	if ((tree_index += 257) == 256) {
	    tree_index = 0;
	    for (i = 8; i > 0; i--) {
		if (bitcount <= 0) {	/* read next input char? */
		    bitcount = 8;
		    bitreg = *bitstring++;
		}
		bitreg <<= 1;
		bitcount--;
		tree_index <<= 1;
		if (bitreg & 0x0100)
		    tree_index |= 1;
	    }	/* end 8-bit for loop */
	}	/* endif to process literal coding */
	*charbuf = tree_index;
	charbuf++;
    }	/* end main output character loop */

    return;
}  /* end of function hc_decode */

#ifdef DEBUG_DECODE
/************************************************/
/*						*/
/*		     Main			*/
/*						*/
/************************************************/
void            main (int argc, char *argv[])
{
#define	BITSTR_BUFSIZE	140
    FILE           *instream;
    FILE           *aa_stderr = stderr;
    char            stringbuf[BITSTR_BUFSIZE + 2];
    char            charbuf[9 * BITSTR_BUFSIZE];
    char            fname_tree[80];
    int             mychar;
    int             oops;
    int             i;
    union {
	INT             integer;
	char            chars[2];
    }               charcount;

    if (argc <= 1) {
	puts ("Usage:  hdecode [hucfile] cypherfile");
	return;
    }
    if (argc >= 3) {
	hctree = NULL;
	append_ext (fname_tree, sizeof (fname_tree), argv[1], EXT_HDECODE);
	hctree_name = fname_tree;
	argv++;
    }
    if ((instream = fopen (argv[1], "rb")) == NULL) {
	fprintf (aa_stderr, "Cannot open cypherfile '%s'.\n", argv[1]);
	exit (2);
    }
MAINLOOP:
    /**************/
    if ((mychar = fgetc (instream)) == EOF)
	return;
    charcount.chars[0] = mychar;
    if ((mychar = fgetc (instream)) == EOF)
	return;
    charcount.chars[1] = mychar;
    if (charcount.integer > sizeof (charbuf) - 2) {
	oops = TRUE;
	charcount.integer = sizeof (charbuf) - 2;
    }
    else
	oops = FALSE;
/*    printf("\n\n***** charcount = %d %s*****\n",
	charcount.integer, (oops) ? "(reduced)" : "");*/
    for (i = 0; i < BITSTR_BUFSIZE; i++) {
	if ((mychar = fgetc (instream)) == EOF) {
	    fprintf (aa_stderr, "\n" PROGNAME "202 Unexpected EOF '%s'.\n",
		argv[1]);
	    exit (2);
	}
	stringbuf[i] = mychar;
    }
    hc_decode (stringbuf, charbuf, charcount.integer, hctree_id);
    for (i = 0; i < charcount.integer; i++)
	putchar (charbuf[i]);
    goto MAINLOOP;
    /************/
}  /* end of function main */

#endif

/************************* HDECODE.C ***************************/
