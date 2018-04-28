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
/* $XConsortium: dtsrkdump.c /main/3 1996/09/23 21:03:37 cde-ibm $
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
 *   FUNCTIONS: count_words
 *		main
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1994,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*********************** DTSRKDUMP.C *************************
 * $Id: dtsrkdump.c /main/3 1996/09/23 21:03:37 cde-ibm $
 * April 1994.
 * Dumps a DtSearch/AusText keyfile to stdout.
 * Renamed from auskdump for DtSearch.
 *
 * $Log$
 * Revision 2.3  1996/04/10  21:19:28  miker
 * Program renamed from auskdump with minor cleanup.
 *
 *
 * *** Log: auskdump.c,v ***
 * Revision 2.2  1995/10/19  20:29:37  miker
 * Permit accessing of read-only databases.
 * Revision 2.1  1995/09/22  18:55:59  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 * Revision 1.11  1995/09/19  21:47:26  miker
 * Added explanation of '*' in report.
 * Revision 1.10  1995/09/06  14:18:33  miker
 * Fixed bug: -p value incorrectly converted to double because
 * atof() function prototype was not provided from stdlib.h.
 * Revision 1.9  1995/09/01  23:58:57  miker
 * Minor name changes for DtSearch.
 * Print err msgs when databases fail to open.
 * Revision 1.8  1995/05/30  18:40:12  miker
 * Print progress dots and some additional dbrec info.
 */
#include "SearchP.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <locale.h>
#include "vista.h"

#define PROGNAME	"DTSRKDUMP"
#define MIN_THRESHOLD	100L
#define KEYS_PER_DOT	1000
#define MS_dtsrkdump	25

/*----------------- GLOBALS -------------------*/
char            buf[2048];
static long    *counters = NULL;	/* allocated array */
static int      do_verbose = FALSE;
static DB_ADDR  dba;
static long     min_threshold = MIN_THRESHOLD;
static long     maxdba = 0L;

static struct or_dbrec
		dbrec;

/****************************************/
/*					*/
/*		count_words		*/
/*					*/
/****************************************/
void            count_words (int index)
{
    long	vista_field;
    UCHAR	*ptr;
    DtSrINT32	offset, free, addrs;
    int		tabstop;
    long	keycount = 0;
    int		dotcount = 0;

    if (index == 0)
	vista_field = OR_SWORDKEY;
    else if (index == 2)
	vista_field = OR_LWORDKEY;
    else if (index == 4)
	vista_field = OR_HWORDKEY;
    else {
	printf (catgets (dtsearch_catd, MS_dtsrkdump, 1,
	    "%s Program Error Abort.\a\n"),
	    PROGNAME"030");
	DtSearchExit (4);
    }

    KEYFRST (PROGNAME"36", vista_field, 0);
    while (db_status == S_OKAY) {
	KEYREAD (PROGNAME"48", buf);
	if (buf[0] == STEM_CH)
	    (counters[index])++;
	else
	    (counters[index + 1])++;

	if (do_verbose) {
	    CRGET (PROGNAME"58", &dba, 0);

	    switch (index) {
		case 0:
		    CRREAD (PROGNAME"66", OR_SWOFFSET, &offset, 0);
		    CRREAD (PROGNAME"67", OR_SWFREE, &free, 0);
		    CRREAD (PROGNAME"68", OR_SWADDRS, &addrs, 0);
		    break;
		case 2:
		    CRREAD (PROGNAME"76", OR_LWOFFSET, &offset, 0);
		    CRREAD (PROGNAME"77", OR_LWFREE, &free, 0);
		    CRREAD (PROGNAME"78", OR_LWADDRS, &addrs, 0);
		    break;
		case 4:
		    CRREAD (PROGNAME"86", OR_HWOFFSET, &offset, 0);
		    CRREAD (PROGNAME"87", OR_HWFREE, &free, 0);
		    CRREAD (PROGNAME"88", OR_HWADDRS, &addrs, 0);
		    break;
	    }
	    NTOHL (offset);
	    NTOHL (free);
	    NTOHL (addrs);
	    if (addrs >= min_threshold) {
		printf (" \"");
		tabstop = 0;
		for (ptr = (UCHAR *) buf;  *ptr != 0;  ptr++) {
		    putchar ((*ptr >= 32) ? *ptr : '~');
		    tabstop++;
		}
		printf ("\" ");
		while (tabstop++ < 22)
		    putchar (' ');
		printf (catgets(dtsearch_catd, MS_dtsrkdump, 2,
		    "%c dba=%d:%-7ld ofs=%-9ld adr=%-6ld fre=%ld\n"),
		    (addrs >= dbrec.or_reccount) ? '*' : ' ',
		    dba >> 24, dba & 0xffffff, offset, addrs, free);
	    }
	}	/* end verbose */

	else {	/* !verbose */
	    if (++keycount % KEYS_PER_DOT == 0) {
		putchar ('.');
		if (++dotcount % 10 == 0)
		    putchar (' ');
		if (dotcount % 50 == 0) {
		    putchar ('\n');
		    dotcount = 0;
		}
		fflush (stdout);
	    }
	}	/* end !verbose dot printing */

	KEYNEXT (PROGNAME"98", vista_field, 0);
    }	/* end object key read loop */

    if (dotcount)
	putchar ('\n');
    return;
}  /* count_words() */


/****************************************/
/*					*/
/*		   main			*/
/*					*/
/****************************************/
int             main (int argc, char *argv[])
{
    int		i;
    int		oops;
    int		dotcount;
    long	keycount;
    long	total;
    char	*ptr;
    int		do_objkeys =		FALSE;
    int		do_wordkeys =		FALSE;
    char	dbpath[2048];
    char	rcs_revision [8];
    char	dbname[12];
    time_t	now;
    double	percent =		0.0;
    int		listing_most_words =	FALSE;

    static char    *word_labels[6] =
    {
	"Short Stems = %8ld\n", "Short Words = %8ld\n",
	"Long Stems =  %8ld\n", "Long Words =  %8ld\n",
	"Huge Stems =  %8ld\n", "Huge Words =  %8ld\n"
    };

    aa_argv0 = argv[0];
    time (&now);
    sscanf ("$Revision: /main/3 $", "%*s %s", rcs_revision);

    setlocale (LC_ALL, "");
    dtsearch_catd = catopen (FNAME_DTSRCAT, 0);

    strftime (buf, sizeof (buf), "%m/%d/%Y, %I:%M %p",
	localtime (&now));
    printf (catgets(dtsearch_catd, MS_dtsrkdump, 3,
	"%s %s, engine %s.  %s.\n"),
	aa_argv0, rcs_revision, AUSAPI_VERSION, buf);

    if (argc <= 1) {
PRINT_USAGE:
	printf (catgets(dtsearch_catd, MS_dtsrkdump, 4,
	    "\nUSAGE: %s -o|w|ow [-v] [-t<N> | -p<N>] dbname\n"
	    "       Reads DtSearch key files and prints summary report.\n"
	    "  -o        Keys examined are OBJECT record keys.\n"
	    "  -w        Keys examined are inverted index WORDS.\n"
	    "  -v        VERBOSE mode, lists every key.\n"
	    "  -t<N>     Threshold.  Sets w and v options, and lists only words\n"
	    "            with >= <N> addresses.  All words will be listed if <N> = 1.\n"
	    "  -p<N>     Another threshold.  Same as -t except <N> is percent\n"
	    "            of the entire database (<N> may include a decimal point).\n"
	    "            For example -p99.9 prints out every word that occurs\n"
	    "            in 99.9%% or more of the records--an excellent way to find\n"
	    "            candidates for the stop list.\n"
	    "            If w and v are set without threshold, default is -t%d.\n"
	    "  <dbname>  1 - 8 character database name with optional path prefix.\n")
	    ,aa_argv0
	    ,MIN_THRESHOLD
	    );
	DtSearchExit (2);
    }

    /* parse options */
    else {	/* argc >= 2 */
	for (;;) {
	    /* each pass grabs new token with "-xxx" format */
	    --argc;
	    ++argv;
	    if (argc <= 0)
		break;	/* no more tokens of any kind */
	    ptr = argv[0];
	    if (*ptr != '-')
		break;	/* no more option tokens */

	    /* examine each char in this -xxx token */
	    while (*(++ptr) != 0) {
		switch (*ptr) {
		    case 'o':
			do_objkeys = TRUE;
			break;

		    case 'w':
			do_wordkeys = TRUE;
			break;

		    case 'v':
			do_verbose = TRUE;
			break;

		    case 'p':
			do_verbose = TRUE;
			do_wordkeys = TRUE;
			percent = atof (ptr + 1);
			if (percent <= 0.0 || percent > 100.0) {
			    fprintf (stderr,
				catgets (dtsearch_catd, MS_dtsrkdump, 5,
				"%s Invalid percent value %lf.\a\n"),
				PROGNAME"195", percent);
			    goto PRINT_USAGE;
			}
			ptr[1] = 0;	/* terminate parse */
			break;

		    case 't':
			do_verbose = TRUE;
			do_wordkeys = TRUE;
			if ((min_threshold = atol (ptr + 1)) <= 0L) {
			    fprintf (stderr,
				catgets (dtsearch_catd, MS_dtsrkdump, 53,
				"%s Invalid threshold value.\a\n"),
				PROGNAME"198");
			    goto PRINT_USAGE;
			}
			ptr[1] = 0;	/* terminate parse */
			break;

		    default:
			fprintf (stderr,
			    catgets (dtsearch_catd, MS_dtsrkdump, 55,
			    "%s Unknown command line argument '%c'.\a\n"),
			    PROGNAME"278", *ptr);
			goto PRINT_USAGE;
		}	/* end switch */
	    }	/* end while-loop for each char of -xxx token */
	}	/* end for-loop for each -xxx token */
    }	/* end of options parse altogether */

    oops = FALSE;
    if (argc <= 0) {
	printf (catgets (dtsearch_catd, MS_dtsrkdump, 56,
	    "%s Missing required database name.\a\n"),
	    PROGNAME"267");
	oops = TRUE;
    }
    if (!do_wordkeys && !do_objkeys) {
	printf (catgets (dtsearch_catd, MS_dtsrkdump, 57,
	    "%s Either -o or -w must be specified.\a\n"),
	    PROGNAME"271");
	oops = TRUE;
    }
    if (oops)
	goto PRINT_USAGE;

    /* Database name may have a long path prefix.
     * If so, we need to segregate the two.
     * Set 'ptr' to just the 8 char dictionary name by moving
     * it backwards until first non-alphanumeric character
     * (such as a ":" in the dos drive id or a slash between directories),
     * or to the beginning of string.
     */
    strncpy (dbpath, argv[0], sizeof (dbpath));
    dbpath[sizeof (dbpath) - 1] = 0;
    for (ptr = dbpath + strlen (dbpath) - 1; ptr >= dbpath; ptr--)
	if (!isalnum (*ptr)) {
	    ptr++;
	    break;
	}
    if (ptr < dbpath)
	ptr = dbpath;

    /* test for valid database name */
    i = strlen (ptr);
    if (i < 1 || i > 8) {
	fprintf (stderr, catgets (dtsearch_catd, MS_dtsrkdump, 58,
	    "%s Invalid database name '%s'.\a\n"),
	    PROGNAME"297", ptr);
	goto PRINT_USAGE;
    }
    strcpy (dbname, ptr);
    *ptr = 0;	/* truncate dbname off of full path/dbname */

    /* Open database in read-only mode. */
    db_oflag = O_RDONLY;
    if (!austext_dopen (dbname, dbpath, NULL, 0, &dbrec)) {
	fprintf (stderr, "%s\n", DtSearchGetMessages());
	DtSearchExit (3);
    }
    maxdba = dbrec.or_maxdba;

    printf (catgets(dtsearch_catd, MS_dtsrkdump, 60,
	"%s: '%s' reccount=%ld maxdba=%ld recslots=%hd minw=%hd maxw=%hd\n"),
	aa_argv0, dbname, dbrec.or_reccount,
	dbrec.or_maxdba, dbrec.or_recslots,
	dbrec.or_minwordsz, dbrec.or_maxwordsz);

    /* Adjust threshold if necessary */
    if (percent > 0.0)
	min_threshold = (long)
	    ((float) percent * (float) dbrec.or_reccount / 100.0);
    if (min_threshold > dbrec.or_reccount)
	min_threshold = dbrec.or_reccount;
    if (do_wordkeys && do_verbose) {



	if (min_threshold > 1 && min_threshold < dbrec.or_reccount) {
	    printf (catgets(dtsearch_catd, MS_dtsrkdump, 70,
		"%s Will only list words occurring "
		"in %ld or more records.\n"),
		aa_argv0, min_threshold);
	    listing_most_words =
		(float) min_threshold / (float) dbrec.or_reccount > .90;
	}
	else {
	    printf (catgets(dtsearch_catd, MS_dtsrkdump, 80,
		"%s: Listing all words in database.\n"),
		aa_argv0);
	    listing_most_words = TRUE;
	}
    }


    if (do_objkeys) {
	/*
	 * Allocate and initialize an array of keytype counters, one for
	 * each possible ascii keytype char (256). 
	 */
	counters = austext_malloc (258 * sizeof(long), PROGNAME"113", NULL);
	memset (counters, 0, 258 * sizeof(long));
	dotcount = 0;
	keycount = 0;

	KEYFRST (PROGNAME"111", OR_OBJKEY, 0);
	while (db_status == S_OKAY) {
	    KEYREAD (PROGNAME"288", buf);
	    (counters[buf[0]])++;

	    CRGET (PROGNAME"251", &dba, 0);
	    if (maxdba < (dba & 0xffffff))
		maxdba = dba;

	    if (do_verbose) {
		/* Mark control and nonascii chars with a period.  */
		i = 0;
		putchar ('\"');
		for (ptr = buf; *ptr != 0; ptr++) {
		    if (*ptr < 32 | *ptr >= 127) {
			putchar ('.');
			i++;
		    }
		    else {
			putchar (*ptr);
			i++;
		    }
		}
		printf ("\" ");
		while (i++ < DtSrMAX_DB_KEYSIZE)
		    putchar (' ');

		printf (catgets(dtsearch_catd, MS_dtsrkdump, 100,
		    "dba x%08lx, %6ld\n"), dba, dba);
	    }	/* end verbose */

	    else {	/* !verbose */
		if (++keycount % KEYS_PER_DOT == 0) {
		    putchar ('.');
		    if (++dotcount % 10 == 0)
			putchar (' ');
		    if (dotcount % 50 == 0) {
			putchar ('\n');
			dotcount = 0;
		    }
		    fflush (stdout);
		}
	    }	/* end !verbose dot printing */

	    KEYNEXT (PROGNAME"291", OR_OBJKEY, 0);
	}	/* end object key read loop */

	/* Print objkey summary report */
	if (dotcount)
	    putchar ('\n');
	if (dbpath[0] == 0)
	    buf[0] = 0;
	else
	    sprintf (buf, catgets(dtsearch_catd, MS_dtsrkdump, 110,
		" in %s"), dbpath);
	printf (catgets(dtsearch_catd, MS_dtsrkdump, 120,
	    "Object Summary for '%s'%s:\n"), dbname, buf);
	puts (catgets(dtsearch_catd, MS_dtsrkdump, 130,
	    "Object Count by Keytypes:"));
	total = 0L;
	for (i = 0; i < 256; i++) {
	    if (counters[i] > 0L) {
		total += counters[i];
		if (i > 32 && i < 127)
		    printf (" '%c' %6ld\n", i, counters[i]);
		else
		    printf (" x%02x %6ld\n", i, counters[i]);
	    }
	}
	printf (catgets(dtsearch_catd, MS_dtsrkdump, 160,
	    "TOTAL Objects Count = %ld\n"), total);
	printf (catgets(dtsearch_catd, MS_dtsrkdump, 170,
	    "Largest Object DBA  = %ld\n"), maxdba);
	free (counters);
    }	/* end do_objkeys */

    if (do_wordkeys) {
	if (listing_most_words)
	    printf (catgets(dtsearch_catd, MS_dtsrkdump, 180,
		"%s: * Words marked with asterisk occur in every record.\n"),
		aa_argv0);

	/*
	 * Allocate and initialize word and stem counters. First is for
	 * short stems (those beginning with STEM_CH), next is for short
	 * words (everything else). Next are for long stems, long words,
	 * huge stems, and huge words (6 in all). 
	 */
	counters = austext_malloc (8 * sizeof (long), PROGNAME"113", NULL);
	memset (counters, 0, 6 * sizeof(long));

	count_words (0);	/* short */
	count_words (2);	/* long */
	count_words (4);	/* huge */

	/* print wordkey summary report */
	if (do_objkeys)
	    putchar ('\n');	/* separate from last report */
	if (dbpath[0] == 0)
	    buf[0] = 0;
	else
	    sprintf (buf, catgets(dtsearch_catd, MS_dtsrkdump, 110,
		" in %s"), dbpath);
	printf (catgets(dtsearch_catd, MS_dtsrkdump, 200,
	    "Words Summary for '%s'%s:\n"), dbname, buf);
	total = 0L;
	for (i = 0; i < 6; i++) {
	    printf (word_labels[i], counters[i]);
	    total += counters[i];
	}
	printf (catgets(dtsearch_catd, MS_dtsrkdump, 210,
	    "TOTAL Words Count = %ld\n"), total);
	free (counters);
    }	/* end do_wordkeys */

    DtSearchExit (0);
}  /* main() */

/*********************** DTSRKDUMP.C *************************/
