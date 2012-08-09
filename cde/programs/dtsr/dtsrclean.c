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
 *   FUNCTIONS: TERMINATE_LINE
 *		copy_new_d99
 *		copy_old_d2x_to_new
 *		end_of_job
 *		main
 *		open_all_files
 *		print_progress
 *		print_usage
 *		read_d2x
 *		signal_shutdown
 *		user_args_processor
 *		validation_error
 *		write_d2x
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1993,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*************************** DTSRCLEAN.C ****************************
 * $TOG: dtsrclean.c /main/9 1998/04/17 11:23:57 mgreess $
 * Does garbage collection (ie compression) of .d99 file.
 * Optionally verifies all database addresses in d99.
 * Modification of clndtbs.c and checkd99.c.
 * Does NOT use austext engine so this must be modified if schema changes.
 * 
 * INPUT FORMAT:
 * All command input is on command line.  Reads existing d2x and d99 files.
 * 
 * OUTPUT FORMAT:
 * New .d2x and .d99 files are placed into the directory specified by user.
 *
 * EXIT CODE STANDARDS:
 * 0 = normal.
 * 1 = warnings, but output should be ok.
 * 2 = failure in cmd line parse or other initialization; job never started.
 * 3 - 49 = fatal error, but output may be acceptable.
 * 50 - 99 = fatal error and output files are probably unusable.
 *	(In this program, even input may be corrupted).
 * 100+ = aborting due to asynchronous interrupt signal.
 *     Output files may or may not be unusable.
 *
 * $Log$
 * Revision 2.4  1996/05/08  16:20:50  miker
 * Added RENFILEs for new d2x files; austext_dopen no longer does.
 *
 * Revision 2.3  1996/02/01  18:13:06  miker
 * Deleted BETA definition.
 *
 * Revision 2.2  1995/10/26  14:51:08  miker
 * Renamed from mrclean.c.  Added prolog.
 *
 * Log: mrclean.c,v
 * Revision 2.1  1995/09/22  21:18:52  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.11  1995/09/05  18:16:46  miker
 * Name, msg, and other minor changes for DtSearch..
 * Print messages if austext_dopen() fails.
 *
 * Revision 1.10  1995/06/02  15:52:42  miker
 * Cleaned up -m and bit vector overflow msgs.
 *
 * Revision 1.9  1995/05/30  19:15:58  miker
 * Print beta char in startup banner msg.
 * Remove -m option and max_totrecs; select bit vector
 * size from maxdba, not reccount.
 */
#include "SearchP.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <locale.h>
#include "vista.h"
#include <sys/types.h>
#include <netinet/in.h>

#define MS_misc		1	/* msg catalog set number */
#define MS_dtsrclean	26	/* msg catalog set number */
#define DISCARD_FORMAT	"%s\t\"%s\"\t%s\t%s\n"	/* copied from oe.h */
#define RECS_PER_DOT	1000
#define DOTS_PER_MSG	50
#define DISK_BLKSIZE	512
#define MAX_CORRUPTION	100
#define	MAX_REC_READ	(DISK_BLKSIZE / sizeof(DB_ADDR))
 /*
  * Max number of addresses to be read from database addresses
  * file, ie the size of one block read from hard disk. 
  */
#define PROGNAME	"DTSRCLEAN"

#define SHOW_NOTHING	0	/* bit arguments for end_of_job() */
#define SHOW_USAGE	1
#define SHOW_EXITCODE	2
#define SHOW_PROGRESS	4

#define TERMINATE_LINE()   if(need_linefeed){fputc('\n',aa_stderr);need_linefeed=FALSE;}

/*-------------------------- GLOBALS ----------------------------*/
static char    *arg_dbname = NULL;
static char    *arg_newpath = NULL;
unsigned char  *bit_vector = NULL;
static size_t   bytes_in = 0L;
static size_t   corruption_count = 0L;
static struct or_swordrec
                d21new, d21old;
static struct or_lwordrec
                d22new, d22old;
static struct or_hwordrec
                d23new, d23old;
static char     datestr[32] = "";	/* "1946/04/17 13:03" */
static int      debug_mode = FALSE;
static size_t   dot_count = 0L;
char            fname_d99_new[1024];
char            fname_d99_old[1024];
FILE           *fp_d99_new = NULL;
FILE           *fp_d99_old = NULL;
static FILE    *frecids = NULL;
static int      is_valid_dba;
static size_t   max_corruption = MAX_CORRUPTION;
static int      normal_exitcode = 0;
static int      need_linefeed = FALSE;
static int      overlay_no = FALSE;
static int      overlay_yes = FALSE;
static DtSrINT32
		reccount =		0;
static DtSrINT32
		recslots;	/* dbrec.or_recslots promoted to INT32 */
static DtSrINT32
		dba_offset;
static DtSrINT32
		recs_per_dot =		RECS_PER_DOT;
static int      rewrite_reccount = FALSE;
static int      shutdown_now = 0;	/* = FALSE */
static size_t   size_d21_old = 0L;
static size_t   size_d22_old = 0L;
static size_t   size_d23_old = 0L;
static size_t   size_d99_old = 0L;
static time_t   timestart = 0L;
static DtSrINT32
		total_num_addrs =	0;
static int      validation_mode = FALSE;


/********************************************************/
/*                                                      */
/*                  signal_shutdown                     */
/*                                                      */
/********************************************************/
/* interrupt handler for SIGINT  */
static void     signal_shutdown (int sig)
{
    shutdown_now = 100 + sig;
    return;
}  /* signal_shutdown() */


/************************************************/
/*						*/
/*		   print_usage			*/
/*						*/
/************************************************/
/* Prints usage statement to stderr. */
static void     print_usage (void)
{
    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 1,
"\nUSAGE: %s [options] <dbname> <newpath>\n"
"       Compresses unused d99 space and validates d00-d99 links.\n"
"  -p<N>     Progress dots printed every <N> records (default %lu).\n"
"            Complete progress message printed every %d dots.\n"
"  -oy       Authorizes overlaying preexisting d99/d2<N> files in newpath.\n"
"  -on       Forces exit if preexisting d99/d2<N> files in newpath.\n"
"  -v        Validates d99 and d00 links, uncorrupts d99 file, and ensures\n"
"            accurate record count.  Also use -c0 to uncorrupt entire database.\n"
"  -v<fname> Same as -v but also writes all d00 recs unreferenced by d99\n"
"            to <fname> in format suitable to extract into .fzk file format.\n"
"  -c<N>     Exits if more than <N> corrupted/incomplete links (default %d).\n"
"            Corruption limit turned off by -c0.\n"
"  <dbname>  1 - 8 char database name = the old d99/d2<N> files to be updated.\n"
"            Files found in local directory or DBFPATH environment variable.\n"
"  <newpath> Specifies where the new d99/d2<N> files will be placed.\n"
"            If first char is not slash, path is relative to local directory.\n"
"EXIT CODES:\n"
"  0: Complete success.  1: Warning.  2: Job never started.\n"
"  3-49: Job ended prematurely, old files ok, new files unusable.\n"
"  50-99: Fatal Error, even old database may be corrupted.\n"
"  100+: Ctrl-C, kill, and all other signal interrupts cause premature\n"
"     end, new files may be unusable.  Signal = exit code - 100.\n")
	,aa_argv0, RECS_PER_DOT, DOTS_PER_MSG, MAX_CORRUPTION);
    return;
}  /* print_usage() */


/************************************************/
/*						*/
/*		  print_progress		*/
/*						*/
/************************************************/
/* Prints progress msg after dots or at end of job.
 * Label is "Final" or "Progress".
 */
static void     print_progress (char *label)
{
    long            seconds;
    int             compression;

    seconds = time (NULL) - timestart;	/* total seconds elapsed */
    if (seconds < 0L)
	seconds = 0L;

    if ((float) bytes_in / (float) size_d99_old >= 99.5)
	compression = 100;
    else {
	compression = (int) (100.* (float) bytes_in / (float) size_d99_old);
	if (compression < 0 || compression > 100)
	    compression = 0;
    }

    TERMINATE_LINE ();
    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 2,
	"%s: %s Compression %d%% (about %lu KB) in %ld:%02ld min:sec.\n") ,
	aa_argv0, label, compression, bytes_in / 1000L,
	seconds / 60UL, seconds % 60UL);
    if (*label == 'F')
	fprintf (aa_stderr,  catgets(dtsearch_catd, MS_dtsrclean, 3,
	    "%s: Counted %ld WORDS in %s.d99.\n") ,
	    aa_argv0, (long)reccount, arg_dbname);
    return;
}  /* print_progress() */


/************************************************/
/*						*/
/*		    end_of_job			*/
/*						*/
/************************************************/
/* Exits program.  Prints status messages before going down.
 * Should be called on even record boundaries whenever possible,
 * ie after record writes complete and shutdown_now > 0 (TRUE).
 */
static void     end_of_job (int exitcode, int show_flags)
{
    TERMINATE_LINE ();
    if (exitcode >= 100) {
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 66,
	    "%s Aborting after interrupt signal %d.\n"),
	    PROGNAME"66", exitcode - 100);
    }
    if (validation_mode && corruption_count == 0L)
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 4,
	    "%s: No corrupted links detected.\n") ,
	    aa_argv0);
    if (corruption_count > 0L) {
	if (max_corruption > 0L && corruption_count >= max_corruption)
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 193,
		"%s Aborting at %ld corrupted links.\n"),
		PROGNAME"193", corruption_count);
	else
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 194,
		"%s Detected%s %ld corrupted/incomplete link(s).\n"),
		PROGNAME"194",
		(validation_mode) ? " and corrected" : "",
		corruption_count);
    }
    if (show_flags & SHOW_PROGRESS) {
	print_progress ("Final");
    }
    if (show_flags & SHOW_USAGE)
	print_usage ();
    if (show_flags & SHOW_EXITCODE)
	fprintf (aa_stderr,  catgets(dtsearch_catd, MS_dtsrclean, 5,
	"%s: Exit code = %d.\n") , aa_argv0, exitcode);
    DtSearchExit (exitcode);
}  /* end_of_job() */


/************************************************/
/*						*/
/*	 	user_args_processor()		*/
/*						*/
/************************************************/
/* Reads and verifies users command line arguments and
 * converts them into internal switches and variables.
 * Some attempt is made to read as many errors as possible
 * before ending job for bad arguments.
 */
static void     user_args_processor (int argc, char **argv)
{
    char           *argptr;
    int             oops = FALSE;
    int             i;
    time_t          stamp;
    size_t          tempsize;

    if (argc < 3)
	end_of_job (2, SHOW_USAGE);

    /* parse all args that begin with a dash (-) */
    while (--argc > 0) {
	argv++;
	argptr = argv[0];
	if (argptr[0] != '-')
	    break;
	switch (tolower (argptr[1])) {
	    case 'r':
		if (strcmp (argptr, "-russell") == 0)	/* backdoor debug */
		    debug_mode = TRUE;
		else
		    goto UNKNOWN_ARG;

	    case 'm':
		fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 301,
		    "%s The -m argument is no longer necessary.\n"),
		    PROGNAME"301");
		break;

	    case 'o':
		i = tolower (argptr[2]);
		if (i == 'n')
		    overlay_no = TRUE;
		else if (i == 'y')
		    overlay_yes = TRUE;
		else {
	INVALID_ARG:
		    fprintf (aa_stderr,
			catgets(dtsearch_catd, MS_dtsrclean, 177,
			"%s Invalid %.2s argument.\n"),
			PROGNAME"177", argptr);
		    oops = TRUE;
		}
		break;

	    case 'v':
		validation_mode = TRUE;
		if (argptr[2] != '\0') {
		    if ((frecids = fopen (argptr + 2, "w")) == NULL) {
			fprintf (aa_stderr,
			    catgets(dtsearch_catd, MS_dtsrclean, 802,
			    "%s Unable to open '%s' to output"
			    " unreferenced d00 records:\n  %s\n"),
			    PROGNAME"802", argptr, strerror(errno));
			oops = TRUE;
		    }
		    time (&stamp);
		    strftime (datestr, sizeof (datestr),
			"%Y/%m/%d %H:%M", localtime (&stamp));
		}
		break;

	    case 'p':
		recs_per_dot = (DtSrINT32) atol (argptr + 2);
		if (recs_per_dot <= 0)
		    goto INVALID_ARG;
		break;

	    case 'c':
		tempsize = atol (argptr + 2);
		if (tempsize < 0L)
		    goto INVALID_ARG;
		max_corruption = tempsize;
		break;

	UNKNOWN_ARG:
	    default:
		fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 159,
		    "%s Unknown argument: '%s'.\n"),
		    PROGNAME"159", argptr);
		oops = TRUE;
		break;
	}	/* end switch */
    }	/* end parse of cmd line args */

    /* Test how we broke loop.
     * There should still be 2 args past the ones
     * beginning with a dash: dbname and newpath.
     */
    if (argc != 2) {
	if (argc <= 0)
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 210,
		"%s Missing required dbname argument.\n"),
		PROGNAME"210");
	if (argc <= 1)
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 211,
		"%s Missing required newpath argument.\n"),
		PROGNAME"211");
	if (argc > 2)
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 212,
		"%s Too many arguments.\n"),
		PROGNAME"212");
	oops = TRUE;
    }
    if (oops)
	end_of_job (2, SHOW_USAGE);

    /* DBNAME */
    arg_dbname = argv[0];
    if (strlen (arg_dbname) > 8) {
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 229,
	    "%s Invalid database name '%s'.\n"),
	    PROGNAME"229", arg_dbname);
	end_of_job (2, SHOW_USAGE);
    }

    /* NEWPATH:
     * Oldpath and newpath are validated when the files
     * are copied and the database is opened.
     */
    arg_newpath = argv[1];
    return;
}  /* user_args_processor() */


/************************************************/
/*						*/
/*		 validation_error()		*/
/*						*/
/************************************************/
/* Subroutine of validation_mode in main().
 * Prints d2x and d99 data at location of error.
 * Adjusts d2x counts for number of good addrs and free slots.
 */
static void     validation_error (DB_ADDR dbaorig)
{
    DB_ADDR         slot;
    is_valid_dba = FALSE;

    slot = dbaorig >> 8;

    /* now efim retranslates back to real dba */
    if (dbaorig != -1)
	slot = ((slot + 1) * recslots - dba_offset)
	    | (OR_D00 << 24);

    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 6,
	"  DBA = %d:%ld (x%02x:%06lx),  orig addr val = x%08lx\n"
	"  Word='%c%s' offset=%ld addrs=%ld free=%d\n") ,
	OR_D00, slot, OR_D00, slot, dbaorig,
	(!isgraph (d23old.or_hwordkey[0])) ? '^' : d23old.or_hwordkey[0],
	d23old.or_hwordkey + 1, d23old.or_hwoffset,
	d23old.or_hwaddrs, d23old.or_hwfree);
    if (--d23new.or_hwaddrs < 0L)
	d23new.or_hwaddrs = 0L;
    /* (should never occur) */
    d23new.or_hwfree++;

    return;
}  /* validation_error() */


/************************************************/
/*						*/
/*		  open_all_files		*/
/*						*/
/************************************************/
static void     open_all_files
                (FILE ** fp, char *fname, char *mode, size_t * size, int *oops) {
    struct stat     fstatbuf;

    if ((*fp = fopen (fname, mode)) == NULL) {
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 439,
	    "%s Can't open %s: %s\n"),
	    PROGNAME"439", fname, strerror (errno));
	*oops = TRUE;
	return;
    }
    if (fstat (fileno (*fp), &fstatbuf) == -1) {
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 440,
	    "%s Can't access status of %s: %s\n"),
	    PROGNAME"440", fname, strerror (errno));
	*oops = TRUE;
	return;
    }
    if (size)
	if ((*size = fstatbuf.st_size) <= 0L) {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 499,
		"%s %s is empty.\n"),
		PROGNAME"499", fname);
	    *oops = TRUE;
	}
    return;
}  /* open_all_files() */


/************************************************/
/*						*/
/*	      copy_old_d2x_to_new		*/
/*						*/
/************************************************/
static void     copy_old_d2x_to_new
                (char *fname_old, char *fname_new, FILE * fp_old, FILE * fp_new) {
    char            readbuf[1024 + 32];
    int             i, j;

    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 7,
	"%s: Copying from old d2x files to %s...\n") ,
	aa_argv0, fname_new);
    for (;;) {	/* loop ends when eof set on input stream */
	errno = 0;
	i = fread (readbuf, 1, sizeof (readbuf), fp_old);
	/* byte swap not required on pure copy operation */
	if (errno) {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 517,
		"%s Read error on %s: %s.\n"),
		PROGNAME"517", fname_old, strerror (errno));
	    end_of_job (3, SHOW_EXITCODE);
	}
	j = fwrite (readbuf, 1, i, fp_new);
	if (i != j) {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 489,
		"%s Write error on %s: %s.\n"),
		PROGNAME"489", fname_new, strerror (errno));
	    end_of_job (3, SHOW_EXITCODE);
	}
	if (shutdown_now)
	    end_of_job (shutdown_now, SHOW_EXITCODE);
	if (feof (fp_old))
	    break;
    }
    TERMINATE_LINE ();
    fclose (fp_old);
    fclose (fp_new);
    return;
}  /* copy_old_d2x_to_new() */


/********************************/
/*				*/
/*	   read_d2x		*/
/*				*/
/********************************/
/* Performs vista RECREAD on curr word record.
 * CALLER SHOULD CHECK DB_STATUS.
 */
void            read_d2x (struct or_hwordrec * glob_word, long field)
{
    if (field == OR_SWORDKEY) {
	RECREAD (PROGNAME "061", &d21old, 0);
	if (db_status != S_OKAY)
	    return;
	strncpy (glob_word->or_hwordkey, d21old.or_swordkey,
	    DtSrMAXWIDTH_HWORD);
	glob_word->or_hwordkey[DtSrMAXWIDTH_HWORD - 1] = 0;
	glob_word->or_hwoffset =	ntohl (d21old.or_swoffset);
	glob_word->or_hwfree =		ntohl (d21old.or_swfree);
	glob_word->or_hwaddrs =		ntohl (d21old.or_swaddrs);
    }
    else if (field == OR_LWORDKEY) {
	RECREAD (PROGNAME "069", &d22old, 0);
	if (db_status != S_OKAY)
	    return;
	strncpy (glob_word->or_hwordkey, d22old.or_lwordkey,
	    DtSrMAXWIDTH_HWORD);
	glob_word->or_hwordkey[DtSrMAXWIDTH_HWORD - 1] = 0;
	glob_word->or_hwoffset =	ntohl (d22old.or_lwoffset);
	glob_word->or_hwfree =		ntohl (d22old.or_lwfree);
	glob_word->or_hwaddrs =		ntohl (d22old.or_lwaddrs);
    }
    else {
	RECREAD (PROGNAME "078", glob_word, 0);
	glob_word->or_hwordkey[DtSrMAXWIDTH_HWORD - 1] = 0;
	NTOHL (glob_word->or_hwoffset);
	NTOHL (glob_word->or_hwfree);
	NTOHL (glob_word->or_hwaddrs);
    }
    return;
}  /* read_d2x() */


/********************************/
/*				*/
/*	   write_d2x		*/
/*				*/
/********************************/
/* performs vista RECWRITE on curr word record.
 * CALLER MUST CHECK DB_STATUS.
 */
static void     write_d2x (struct or_hwordrec * glob_word, long field)
{
    if (field == OR_SWORDKEY) {
	strcpy (d21new.or_swordkey, glob_word->or_hwordkey);
	d21new.or_swoffset =	htonl (glob_word->or_hwoffset);
	d21new.or_swfree =	htonl (glob_word->or_hwfree);
	d21new.or_swaddrs =	htonl (glob_word->or_hwaddrs);
	RECWRITE (PROGNAME "102", &d21new, 0);
    }
    else if (field == OR_LWORDKEY) {
	strcpy (d22new.or_lwordkey, glob_word->or_hwordkey);
	d22new.or_lwoffset =	htonl (glob_word->or_hwoffset);
	d22new.or_lwfree =	htonl (glob_word->or_hwfree);
	d22new.or_lwaddrs =	htonl (glob_word->or_hwaddrs);
	RECWRITE (PROGNAME"112", &d22new, 0);
    }
    else {
	HTONL (glob_word->or_hwoffset);
	HTONL (glob_word->or_hwfree);
	HTONL (glob_word->or_hwaddrs);
	RECWRITE (PROGNAME "115", glob_word, 0);
    }
    return;
}  /* write_d2x() */



/************************************************/
/*						*/
/*		    copy_new_d99()		*/
/*						*/
/************************************************/
/* The garbage collection/compression process itself.
 * For very large databases, there will be appx 3 million word records,
 * so the loop should be coded for ***EFFICIENCY***.
 */
static void     copy_new_d99 (long keyfield)
{
    int             is_odd_nibble;
    DtSrINT32       num_holes;
    DtSrINT32       slots_left;
    unsigned char  *bvptr;
    int             a;
    DB_ADDR         dba, dbaorig;
    DtSrINT32       x;
    DtSrINT32       swapx;
    int             done;
    DtSrINT32       good_addrs_left;
    DtSrINT32       good_addrs_this_block;
    DtSrINT32       num_reads, num_writes;
    DB_ADDR         word_addrs[MAX_REC_READ + 64];	/* d99 read buf */
    DB_ADDR         word_addrs_out[MAX_REC_READ + 64];	/* d99 write buf */

    KEYFRST (PROGNAME "179", keyfield, 0);
    while (db_status == S_OKAY) {
	read_d2x (&d23new, keyfield);
	if (validation_mode)	/* save for validation err msgs */
	    memcpy (&d23old, &d23new, sizeof (d23old));

	/*
	 * Read old d99 file at specified offset to get total num
	 * "holes". In the first portion of record holes are filled
	 * with representations of valid database addresses +
	 * statistical weights. In the second portion the holes are
	 * "free slots" for future expansion which are
	 * conventionally initialized with a -1. 
	 */
	/* force number of free slots to 0(ZERO) */
	d23new.or_hwfree = 0;
	fseek (fp_d99_old, d23new.or_hwoffset, SEEK_SET);
	num_holes = d23new.or_hwaddrs + d23new.or_hwfree;
	good_addrs_left = d23new.or_hwaddrs;
	bytes_in += sizeof (DB_ADDR) * num_holes;

	/* Update the offset in the d2x record buffer */
	d23new.or_hwoffset = ftell (fp_d99_new);

	/*
	 * Copy the array of holes in each disk block, reading the
	 * old and writing to the new.  Loop ends when the number
	 * of holes left will fit into one last block. 
	 */
	done = FALSE;
	while (!done) {	/* loop on each block in this word */
	    if (num_holes > MAX_REC_READ) {
		num_reads = MAX_REC_READ;
		num_holes -= MAX_REC_READ;
	    }
	    else {
		done = TRUE;
		num_reads = num_holes;
	    }
	    errno = 0;
	    fread (word_addrs, sizeof(DB_ADDR), (size_t)num_reads, fp_d99_old);
	    if (errno) {
		TERMINATE_LINE ();
		fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 657,
		    "%s Read error on %s: %s.\n"),
		    PROGNAME"657", fname_d99_old, strerror (errno));
		end_of_job (4, SHOW_PROGRESS + SHOW_EXITCODE);
	    }
	    /* Note BYTE_SWAP only needed for validation_mode.
	     * If not validating, we're just going to copy
	     * the network format dba's as is directly to
	     * the new d99 file.
	     */

	    /*
	     * Addrs on d99 are now 'record numbers' not dbas. A
	     * rec# is what the dba/slot# would be if records took
	     * up just one slot and there were no dbrec at start of
	     * file.  D99 rec#s start at #1, not #0. 
	     */

	    /*
	     * If user requested validation_mode, validate each
	     * 'good' rec# (not free slots) in word_addrs buffer. 
	     * If any d99 links are corrupt, skip them when copying
	     * to the new d99 file. Rewrite -1's to all free slots.
	     * ----> NOTE UNUSUAL FORMAT OF DBA HOLES IN D99! <----
	     * Record number is shifted to the high order 3 bytes.
	     * The statistical weight is in the low order byte. The
	     * vista file number is known from the #define constant
	     * OR_D00, and the vista dba/slot# is mapped from rec#
	     * by mult/div number of slots per rec, plus/minus
	     * dbrec offset. 
	     */
	    if (validation_mode) {
#ifdef BYTE_SWAP
		for (swapx = 0;  swapx < num_reads;  swapx++)
		    NTOHL (word_addrs[swapx]);
#endif
		/* set x to number of good addrs in this block */
		if (good_addrs_left > num_reads) {
		    x = num_reads;
		    good_addrs_left -= num_reads;
		}
		else {
		    x = good_addrs_left;
		    good_addrs_left = 0;
		}

		/*
		 * Validate the rec#'s in this block.  Note that
		 * the loop is skipped if the entire block is free
		 * slots. 
		 */
		good_addrs_this_block = 0;
		for (a = 0; a < x; a++) {	/* a = index to curr dba */
		    /*
		     * Get rec#.  Save original rec# for err msgs,
		     * then shift slot number to lower 3 bytes,
		     * discarding weight. 
		     */
		    dbaorig = word_addrs[a];	/* rec#,rec#,rec#:wt */
		    dba = dbaorig >> 8;	/* 0,rec#,rec#,rec# */
		    is_valid_dba = TRUE;	/* default */

		    /*
		     * If original rec# == -1 we've overrun the
		     * good rec#'s into the expansion area, which
		     * is filled with -1's.  This is real bad news
		     * because if the counts in d02 are bad, the
		     * online programs will quickly crash, and we
		     * can't continue this program. Advance to next
		     * rec# because we can't mark the bit vector. 
		     */
		    if (dbaorig == -1L) {
			TERMINATE_LINE ();
			fprintf (aa_stderr,
			    catgets(dtsearch_catd, MS_dtsrclean, 111,
			    "*** %s DBA in d99 = -1.  "
			    "Probable overrun into expansion\n"
			    "  area due to incorrect count values "
			    "in d2x file.\n"),
			    PROGNAME"111");
			validation_error (dbaorig);
			corruption_count++;
			if (max_corruption > 0L &&
			    corruption_count >= max_corruption)
			    end_of_job (91, SHOW_PROGRESS + SHOW_EXITCODE);
			continue;	/* skip the bit vector
					 * check */
		    }

		    /*
		     * If slot number > max totrecs, we have a
		     * corrupted d99-d00 link because we've already
		     * validated the d00 file and we know that it
		     * has no slots > max.  Also we have to advance
		     * to next slot because we can't mark the bit
		     * vector. 
		     */
		    /******if (dba >= max_totrecs)*******/
		    if (dba >= total_num_addrs) {
			TERMINATE_LINE ();
			fprintf (aa_stderr,
			    catgets(dtsearch_catd, MS_dtsrclean, 222,
			    "*** %s DBA in d99 not in d00,"
			    " slot > max num docs.\n"),
			    PROGNAME"222");
			validation_error (dbaorig);
			corruption_count++;
			if (max_corruption > 0L &&
			    corruption_count >= max_corruption)
			    end_of_job (92, SHOW_PROGRESS + SHOW_EXITCODE);
			continue;	/* skip the bit vector check */
		    }

		    /*
		     * Verify that dba exists in d00 file (test bit
		     * #1). If not, mark bit #3 (3rd lowest) in
		     * nibble and print error msg unless bit #3
		     * previously marked. 
		     */
		    bvptr = bit_vector + (dba >> 1);
		    is_odd_nibble = (dba & 1L);
		    if (!(*bvptr & ((is_odd_nibble) ? 0x01 : 0x10))) {
								/* bit #1 */
			if (!(*bvptr & ((is_odd_nibble) ? 0x04 : 0x40))) {
								/* bit #3 */
			    *bvptr |= (is_odd_nibble) ? 0x04 : 0x40;
			    TERMINATE_LINE ();
			    fprintf (aa_stderr,
				catgets(dtsearch_catd, MS_dtsrclean, 333,
				"*** %s DBA in d99 does not exist in d00.\n"),
				PROGNAME"333");
			    validation_error (dbaorig);
			    corruption_count++;
			    if (max_corruption > 0L &&
				corruption_count >= max_corruption)
				end_of_job (93, SHOW_PROGRESS + SHOW_EXITCODE);
			}	/* endif where corrupt link
				 * detected */
		    }

		    /*
		     * Mark bit #2 in bit vector indicating a d99
		     * reference. 
		     */
		    *bvptr |= (is_odd_nibble) ? 0x02 : 0x20;	/* bit #2 */

		    /*
		     * move good dba to curr output block, incr
		     * counter 
		     */
		    if (is_valid_dba)
			word_addrs_out[good_addrs_this_block++] = dbaorig;

		}	/* end validation loop for each good dba in
			 * the block */

		/*
		 * Write out only validated addrs in current block.
		 * If this was the last block, fill out all the
		 * free slots, if any, with -1 values, and exit the
		 * dba loop for this word. 
		 */
		if (good_addrs_this_block > 0) {
#ifdef BYTE_SWAP
		    for (swapx = 0;  swapx < good_addrs_this_block;  swapx++)
			NTOHL (word_addrs_out[swapx]);
#endif
		    num_writes = fwrite (word_addrs_out, sizeof (DB_ADDR),
			(size_t)good_addrs_this_block, fp_d99_new);
		    if (num_writes != good_addrs_this_block)
			goto WRITE_ERROR;
		}
		if (good_addrs_left <= 0) {
		    /*
		     * Write blocks of -1s until new d2x free slot
		     * count is exhausted.  The last block may be <
		     * MAX_REC_READ. 
		     */
		    slots_left = d23new.or_hwfree;
		    while (slots_left > 0) {
			/*
			 * set x to number of -1's to write for
			 * this block 
			 */
			if (slots_left > MAX_REC_READ) {
			    x = MAX_REC_READ;
			    slots_left -= MAX_REC_READ;
			}
			else {
			    x = slots_left;
			    slots_left = 0;
			}
			for (a = 0; a < x; a++)
			    word_addrs_out[a] = (DtSrINT32) -1;
			/* BYTE_SWAP not required for foxes */
			num_writes = fwrite (word_addrs_out,
			    sizeof(DB_ADDR), (size_t)x, fp_d99_new);
			if (num_writes != x)
			    goto WRITE_ERROR;
		    }	/* end while loop to write out all -1's */
		    done = TRUE;
		}
	    }	/* endif for validation_mode for this block */

	    /*
	     * If NOT in validation mode, just write out the new
	     * d99 block as an exact copy of the input block. 
	     * BYTE_SWAP not required because word_addrs is
	     * still in its original network order from the fread.
	     */
	    else {
		num_writes = fwrite (word_addrs, sizeof(DB_ADDR),
		    (size_t)num_reads, fp_d99_new);
		if (num_writes != num_reads) {
	    WRITE_ERROR:
		    fprintf (aa_stderr,
			catgets(dtsearch_catd, MS_dtsrclean, 665,
			"%s Write error on %s: %s.\n"),
			PROGNAME"665", fname_d99_new, strerror(errno));
		    end_of_job (4, SHOW_PROGRESS + SHOW_EXITCODE);
		}
	    }	/* endelse for NOT validation_mode for this block */

	}	/* end loop for all blocks for this entire word
		 * (done = TRUE) */

	/* write the updated d2x record */
	write_d2x (&d23new, keyfield);
	reccount++;

	/*
	 * Every now and then print a dot. Print complete progress
	 * msg after DOTS_PER_MSG dots. 
	 */
	if (!(reccount % recs_per_dot)) {
	    if (++dot_count > DOTS_PER_MSG) {
		dot_count = 0;
		print_progress ("Progress");
	    }
	    else {
		fputc ('.', aa_stderr);
		need_linefeed = TRUE;
		if (!(dot_count % 10L))
		    fputc (' ', aa_stderr);
	    }
	    fflush (aa_stderr);
	}	/* end of print-a-dot */

	if (shutdown_now)
	    end_of_job (shutdown_now, SHOW_PROGRESS + SHOW_EXITCODE);
	KEYNEXT (PROGNAME "196", keyfield, 0);
    }	/* end of main loop on each word in database */


    return;
}  /* copy_new_d99() */


/************************************************/
/*						*/
/*		      main()			*/
/*						*/
/************************************************/
int             main (int argc, char *argv[])
{
    FILE_HEADER     fl_hdr;
    int             a, i, j;
    unsigned char  *bvptr;
    DB_ADDR         dba, dba1, dbaorig;
    char            dbfpath[1024];
    char            fname_d21_new[1024];
    char            fname_d21_old[1024];
    char            fname_d22_new[1024];
    char            fname_d22_old[1024];
    char            fname_d23_new[1024];
    char            fname_d23_old[1024];
    FILE           *fp_d21_new = NULL;
    FILE           *fp_d21_old = NULL;
    FILE           *fp_d22_new = NULL;
    FILE           *fp_d22_old = NULL;
    FILE           *fp_d23_new = NULL;
    FILE           *fp_d23_old = NULL;
    char            full_dbname_old[1024];
    char            full_dbname_new[1024];
    DtSrINT32	    max_bitvec = 0L;
    int             oops;
    char           *ptr;
    char            readbuf[1024 + 32];
    unsigned long   reads_per_dot;
    char            recidbuf[DtSrMAX_DB_KEYSIZE + 4];
    time_t          starttime;
    DtSrINT32	    x;
    struct or_dbrec dbrec;

    aa_argv0 = argv[0];
    setlocale (LC_ALL, "");
    dtsearch_catd = catopen (FNAME_DTSRCAT, 0);

    time (&starttime);
    strftime (dbfpath, sizeof (dbfpath),	/* just use any ol' buffer */
	catgets (dtsearch_catd, MS_misc, 22, "%A, %b %d %Y, %I:%M %p"),
	localtime (&starttime));
    printf ( catgets(dtsearch_catd, MS_dtsrclean, 11,
	"%s Version %s.  Run %s.\n") ,
	aa_argv0, AUSAPI_VERSION, dbfpath);

    signal (SIGHUP, signal_shutdown);
    signal (SIGINT, signal_shutdown);
    signal (SIGQUIT, signal_shutdown);
    signal (SIGTRAP, signal_shutdown);
    signal (SIGKILL, signal_shutdown);	/* this cannot be trapped */
    signal (SIGALRM, signal_shutdown);
    signal (SIGTERM, signal_shutdown);
#ifdef SIGPWR
    signal (SIGPWR, signal_shutdown);
#endif
#ifdef _AIX
    signal (SIGXCPU, signal_shutdown);
    signal (SIGDANGER, signal_shutdown);
#endif

    user_args_processor (argc, argv);

    /* In order to find old files, we have to check if
     * DBFPATH environment variable has been set.
     * Load the fully constructed DBFPATH-dbname into its own buffer.
     */
    full_dbname_old[0] = '\0';
    dbfpath[0] = 0;
    if ((ptr = getenv ("DBFPATH")) != NULL) {
	if (*ptr == 0)
	    fprintf (aa_stderr,
		 catgets(dtsearch_catd, MS_dtsrclean, 12,
		"%s: Ignoring empty DBFPATH environment variable.\n") ,
		aa_argv0);
	else {
	    fprintf (aa_stderr,  catgets(dtsearch_catd, MS_dtsrclean, 13,
		"%s: Using DBFPATH = '%s'.\n") ,
		aa_argv0, ptr);
	    strcpy (full_dbname_old, ptr);

	    /* Ensure that DBFPATH ends in a slash. */
	    ptr = strchr (full_dbname_old, '\0');
	    if (*(ptr - 1) != LOCAL_SLASH) {
		*ptr++ = LOCAL_SLASH;
		*ptr = '\0';
	    }
	    strcpy (dbfpath, full_dbname_old);
	}
    }

    /* Currently full_dbname_old contains just the path.
     * Similarly, build just path name for the 2 new files
     * using full_dbname_new as a buffer.
     * Verify they don't both refer to the same directory.
     */
    strcpy (full_dbname_new, arg_newpath);
    ptr = strchr (full_dbname_new, '\0');
    if (*(ptr - 1) != LOCAL_SLASH) {
	*ptr++ = LOCAL_SLASH;
	*ptr = '\0';
    }
    if (strcmp (full_dbname_old, full_dbname_new) == 0) {
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 393,
	    "%s Old and new directories are identical: '%s'.\n"),
	    PROGNAME"393", full_dbname_old);
	end_of_job (2, SHOW_USAGE);
    }

    /* Complete full_dbname_old by appending dbname to the path prefix.
     * Then build full path/file names for all 4 files.
     */
    strcat (full_dbname_old, arg_dbname);
    strcat (full_dbname_new, arg_dbname);
    fprintf (aa_stderr,  catgets(dtsearch_catd, MS_dtsrclean, 14,
	"%s: Old files: '%s.d2x, .d99'.\n") ,
	aa_argv0, full_dbname_old);
    fprintf (aa_stderr,  catgets(dtsearch_catd, MS_dtsrclean, 15,
	"%s: New files: '%s.d2x, .d99'.\n") ,
	aa_argv0, full_dbname_new);

    strcpy (fname_d99_old, full_dbname_old);
    strcat (fname_d99_old, ".d99");
    strcpy (fname_d21_old, full_dbname_old);
    strcat (fname_d21_old, ".d21");
    strcpy (fname_d22_old, full_dbname_old);
    strcat (fname_d22_old, ".d22");
    strcpy (fname_d23_old, full_dbname_old);
    strcat (fname_d23_old, ".d23");
    strcpy (fname_d99_new, full_dbname_new);
    strcat (fname_d99_new, ".d99");
    strcpy (fname_d21_new, full_dbname_new);
    strcat (fname_d21_new, ".d21");
    strcpy (fname_d22_new, full_dbname_new);
    strcat (fname_d22_new, ".d22");
    strcpy (fname_d23_new, full_dbname_new);
    strcat (fname_d23_new, ".d23");

    /* If the user hasn't already authorized overwriting preexisting files,
     * check new directory and if new files already exist,
     * ask permission to overwrite.
     */
    if (!overlay_yes) {
	oops = FALSE;	/* TRUE forces a user prompt */
	if ((fp_d99_new = fopen (fname_d99_new, "r")) != NULL) {
	    fclose (fp_d99_new);
	    oops = TRUE;
	}
	if ((fp_d21_new = fopen (fname_d21_new, "r")) != NULL) {
	    fclose (fp_d21_new);
	    oops = TRUE;
	}
	if ((fp_d22_new = fopen (fname_d22_new, "r")) != NULL) {
	    fclose (fp_d22_new);
	    oops = TRUE;
	}
	if ((fp_d23_new = fopen (fname_d23_new, "r")) != NULL) {
	    fclose (fp_d23_new);
	    oops = TRUE;
	}
	if (oops) {
	    fprintf (aa_stderr,  catgets(dtsearch_catd, MS_dtsrclean, 24,
		"%s: One or more new files already exist.\n") ,
		aa_argv0);
	    if (overlay_no) {
		fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 463,
		    "%s Command line argument disallows file overlay.\n"),
		    PROGNAME"463");
		end_of_job (2, SHOW_EXITCODE);
	    }
	    fputs (catgets(dtsearch_catd, MS_dtsrclean, 45,
		"    Is it ok to overlay files in new directory? [y/n] "),
		aa_stderr);

            *readbuf = '\0';
            fgets (readbuf, sizeof(readbuf), stdin);
            if (strlen(readbuf) && readbuf[strlen(readbuf)-1] == '\n')
              readbuf[strlen(readbuf)-1] = '\0';

	    if (tolower (*readbuf) != 'y')
		end_of_job (2, SHOW_NOTHING);
	}
    }	/* end of check for overlaying new files */

    /* Open all files.  The d2x's are opened so that the old ones
     * can be copied into the new directory before starting
     * the garbage collection process proper.
     * The d99's are opened now just to verify permissions.
     */
    oops = FALSE;  /* TRUE ends job, but only after trying all 4 files */
    open_all_files (&fp_d21_old, fname_d21_old, "rb", &size_d21_old, &oops);
    open_all_files (&fp_d22_old, fname_d22_old, "rb", &size_d22_old, &oops);
    open_all_files (&fp_d23_old, fname_d23_old, "rb", &size_d23_old, &oops);
    open_all_files (&fp_d99_old, fname_d99_old, "rb", &size_d99_old, &oops);
    open_all_files (&fp_d21_new, fname_d21_new, "wb", NULL, &oops);
    open_all_files (&fp_d22_new, fname_d22_new, "wb", NULL, &oops);
    open_all_files (&fp_d23_new, fname_d23_new, "wb", NULL, &oops);
    open_all_files (&fp_d99_new, fname_d99_new, "wb", NULL, &oops);

    if (shutdown_now)
	end_of_job (shutdown_now, SHOW_EXITCODE);
    if (oops)
	end_of_job (2, SHOW_EXITCODE);

    /* Copy old d2x files to new directory.
     * Database will open using new files so only they will be changed.
     */
    copy_old_d2x_to_new (fname_d21_old, fname_d21_new, fp_d21_old, fp_d21_new);
    copy_old_d2x_to_new (fname_d22_old, fname_d22_new, fp_d22_old, fp_d22_new);
    copy_old_d2x_to_new (fname_d23_old, fname_d23_new, fp_d23_old, fp_d23_new);

    /* Open database, but use new d2x files for updates. */
    RENFILE (PROGNAME"1102", arg_dbname, OR_D21, fname_d21_new);
    RENFILE (PROGNAME"1104", arg_dbname, OR_D22, fname_d22_new);
    RENFILE (PROGNAME"1106", arg_dbname, OR_D23, fname_d23_new);
    if (!austext_dopen (arg_dbname, (dbfpath[0] == 0) ? NULL : dbfpath,
	    NULL, 0, &dbrec)) {
	puts (DtSearchGetMessages ());
	end_of_job (3, SHOW_EXITCODE);
    }

    /* This is where efim changed real dba to
     * record number (still called dba)
     */
    RECFRST (PROGNAME "1067", OR_OBJREC, 0);
    CRGET (PROGNAME "1068", &dba, 0);	/* dba of first real obj
					 * record */
    recslots = dbrec.or_recslots;	/* vista slots per obj
					 * record */
    dba_offset = recslots - (dba & 0xffffff);	/* accounts for dbrec */

    /* total_num_addrs = what reccount would be if
     * all holes were filled with good records.
     */
    total_num_addrs = (dbrec.or_maxdba - (dba & 0xffffff) + 1) / recslots + 1;
    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 25,
	"%s: curr reccnt=%ld, mxdba=%ld, sl/rec=%ld, tot#adr=%ld.\n") ,
	aa_argv0, (long)dbrec.or_reccount, (long)dbrec.or_maxdba,
	(long)dbrec.or_recslots, (long)total_num_addrs);

    /* Initialize validation_mode (checkd99) */
    if (validation_mode) {
	/*
	 * Allocate and initialize a bit vector: 4 bits for every
	 * possible d00 database address. 
	 */
	max_bitvec = (total_num_addrs >> 1) + 2;
	if ((bit_vector = malloc ((size_t)max_bitvec + 64)) == NULL) {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 465,
		"%s WARNING: Can't allocate memory for bit vector.\n"
		"  'Validate' mode switched off.\n"),
		PROGNAME"465");
	    validation_mode = FALSE;
	    normal_exitcode = 1;	/* warning */
	    goto EXIT_INIT_VALIDATION;
	}
	memset (bit_vector, 0, (size_t)max_bitvec);

	/*
	 * Read every d00 rec sequentially.  1 in bit #1 (lowest
	 * order) in bit vector means record (dba) exists in d00
	 * file. While we're at it, count the total number of
	 * records. 
	 */
	x = dbrec.or_reccount / 50 + 1;	/* x = recs per dot */
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 26,
	    "%s: Reading d00 file.  Each dot appx %ld database documents...\n"),
	    aa_argv0, (long)x);
	reccount = 0;
	dot_count = 0L;
	RECFRST (PROGNAME "534", OR_OBJREC, 0);
	while (db_status == S_OKAY) {
	    CRREAD (PROGNAME "617", OR_OBJKEY, recidbuf, 0);

	    /* print periodic progress dots */
	    if (!(++reccount % x)) {
		fputc ('.', aa_stderr);
		need_linefeed = TRUE;
		if (!(++dot_count % 10L))
		    fputc (' ', aa_stderr);
		fflush (aa_stderr);
	    }

	    /*
	     * Get dba and record number and confirm it will not
	     * overflow bit vector. 
	     */
	    CRGET (PROGNAME "537", &dba, 0);
	    dba &= 0x00ffffff;	/* mask out file number in high order byte */
	    dba1 = (dba + dba_offset) / recslots;  /* ="rec number", base 1 */
	    if (dba1 >= total_num_addrs) {
		TERMINATE_LINE ();
		fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 561,
		    "%s DBA '%d:%ld' (rec #%ld) in d00 exceeds "
		    "total num addrs %ld;\n"
		    "  Bit vector overflow because maxdba %ld"
		    " in dbrec is incorrect.\n"),
		    PROGNAME"561", OR_D00, (long)dba, (long)dba1,
		    (long)total_num_addrs, (long)dbrec.or_maxdba);
		end_of_job (7, SHOW_EXITCODE);
	    }
	    if (shutdown_now)
		end_of_job (shutdown_now, SHOW_EXITCODE);

	    /*
	     * Set bit #1 of even or odd nibble to indicate that
	     * this record *number* actually exists in d00 file. 
	     */
	    bit_vector[dba1 >> 1] |= (dba1 & 1L) ? 0x01 : 0x10;

	    RECNEXT (PROGNAME "541", 0);
	}	/* end of sequential read thru d00 file */

	TERMINATE_LINE ();	/* end the dots... */

	/* confirm that RECCOUNT record holds the correct number */
	if (dbrec.or_reccount == reccount) {
	    fprintf (aa_stderr,
		 catgets(dtsearch_catd, MS_dtsrclean, 27,
		"%s: Confirmed %ld DOCUMENTS in %s.d00.\n") ,
		aa_argv0, (long)dbrec.or_reccount, arg_dbname);
	}
	else {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 28,
		"%s: %ld DOCUMENTS actually in %s.d00 not ="
		" %ld count stored there.\n"
		"  Count will be corrected in new d00 file.\n") ,
		aa_argv0, (long)reccount, arg_dbname, (long)dbrec.or_reccount);
	    dbrec.or_reccount = reccount;
	    rewrite_reccount = TRUE;
	}

EXIT_INIT_VALIDATION:;
    }	/* end of validation_mode initialization */

    /* initialize main loop */
    time (&timestart);
    reccount = 0;
    bytes_in = 0L;
    dot_count = DOTS_PER_MSG;	/* force initial msg after first
				 * blk of recs */
    TERMINATE_LINE ();
    fprintf (aa_stderr, catgets(dtsearch_catd, MS_dtsrclean, 29,
	"%s: Compressing into %s.  Each dot appx %lu words...\n") ,
	aa_argv0, arg_newpath, (unsigned long)recs_per_dot);

    /* write New Header Information to a new d99 file */
    init_header (fp_d99_new, &fl_hdr);

    /* Sequentially read each word key file in big loop.
     * For each word, read the d99.
     * In validation mode check the dbas.
     * If not validating, just blindly rewrite the old d99 to the new one.
     * If validating only write good dba's and mark the bit vector.
     */
    copy_new_d99 (OR_SWORDKEY);
    copy_new_d99 (OR_LWORDKEY);
    copy_new_d99 (OR_HWORDKEY);


    if (reccount == 0)
	end_of_job (50, SHOW_PROGRESS + SHOW_EXITCODE);
    else
	print_progress ("Final");

    /* If validation_mode requested, traverse bit vector and print out
     * table of each d00 record which cannot be accessed from any d99 word.
     * If a validation file name was provided, write out a line for each
     * bad reecord in alebeniz-compatible format.
     */
    if (validation_mode) {
	for (x = 0, bvptr = bit_vector;  x < max_bitvec;  x++, bvptr++) {
	    for (j = 0; j < 8; j += 4) {	/* j = 0 or 4, amount of
						 * bit shift */
		/* a = bits #1 and #2 of current nibble */
		a = 0x30 & (*bvptr << j);

		/* if dba is in d00 but not in d99... */
		if (a & 0x10 && !(a & 0x20)) {
		    /* ...construct valid vista dba */
		    dbaorig = x << 1;
		    if (j)
			dbaorig++;	/* slot number */
		    /***	dba = dbaorig | (OR_D00 << 24);	***//* r
		     * eal dba */

		    /* now efim retranslates back to real dba */
		    dba = ((dbaorig + 1) * recslots - dba_offset)
			| (OR_D00 << 24);

		    /* ...print out err msg */
		    CRSET (PROGNAME "734", &dba, 0);
		    CRREAD (PROGNAME "735", OR_OBJKEY, readbuf, 0);
		    fprintf (aa_stderr,
			catgets(dtsearch_catd, MS_dtsrclean, 444,
			"*** %s d00 record '%s' is not referenced in d99.\n"
			"  DBA = %d:%ld (x%02x:%06lx).\n") ,
			PROGNAME"444", readbuf, OR_D00,
			(long)dba, OR_D00, (long)dba);

		    /*...if albeniz compatible output requested, do it */
		    if (frecids) {
			fprintf (frecids, DISCARD_FORMAT, arg_dbname,
			    readbuf, "MrClean", datestr);
		    }

		    corruption_count++;
		    if (max_corruption > 0L  &&
				corruption_count >= max_corruption)
			end_of_job (94, SHOW_EXITCODE);
		}	/* endif where d00 is not referenced by d99 */
	    }	/* end forloop: every 2 bits in a bitvector byte */
	}	/* end forloop: every byte in bitvector */
    }

    /* Normal_exitcode currently will contain either a 0 or a 1.
     * If we were uncorrupting the d99 and found any corrupt links,
     * make sure it's 1 (warning).  If there were corrupt links and
     * we weren't trying to uncorrupt it, change it to a hard error.
     */
 /***by the way, corruption_count can be > 0 only if in validation_mode.**/
    if (corruption_count > 0L) {
	if (validation_mode)
	    normal_exitcode = 1;
	else
	    normal_exitcode = 90;
    }
    end_of_job (normal_exitcode, SHOW_EXITCODE);
}  /* main() */

/*************************** DTSRCLEAN.C ****************************/
