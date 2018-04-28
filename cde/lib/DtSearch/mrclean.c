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
 *   IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 *   combined with the aggregated modules for this product)
 *   OBJECT CODE ONLY SOURCE MATERIALS
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*************************** MRCLEAN.C ****************************
 * $TOG: mrclean.c /main/7 1998/04/17 11:25:42 mgreess $
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
#include <signal.h>
#include <sys/stat.h>
#include "vista.h"

#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#include <X11/Xos_r.h>

#define MS_misc		1	/* msg catalog set number */
#define DISCARD_FORMAT	"%s\t\"%s\"\t%s\t%s\n"	/* copied from oe.h */
#define RECS_PER_DOT	1000L
#define DOTS_PER_MSG	50L
#define DISK_BLKSIZE	512
#define MAX_CORRUPTION	100L
#define	MAX_REC_READ	(DISK_BLKSIZE / sizeof(DB_ADDR))
	/* Max number of addresses to be read from
	 * database addresses file, ie the size
	 * of one block read from hard disk.
	 */ 
#define PROGNAME	"MRCLEAN"
#define READBUFSIZE	(1024 + 32)

#define SHOW_NOTHING	0	/* bit arguments for end_of_job() */
#define SHOW_USAGE	1
#define SHOW_EXITCODE	2
#define SHOW_PROGRESS	4

#define TERMINATE_LINE()   if(need_linefeed){fputc('\n',aa_stderr);need_linefeed=FALSE;}


/*-------------------------- GLOBALS ----------------------------*/
static char		*arg_dbname =		NULL;
static char		*arg_newpath =		NULL;
long			batch_size =		0;	/* (to fileman.c) */
int			beta =
			    #ifdef BETA
				BETA;
			    #else
				0;
			    #endif
char			betabuf[8];
unsigned char		*bit_vector =		NULL;
static size_t		bytes_in =		0L;
static size_t		corruption_count =	0L;
static struct or_swordrec
			d21new, d21old;
static struct or_lwordrec
			d22new, d22old;
static struct or_hwordrec
			d23new, d23old;
static char		datestr [32] =		"";	/* "1946/04/17 13:03" */
static int		debug_mode =		FALSE;
static size_t		dot_count =		0L;
char			fname_d99_new [1024];
char			fname_d99_old [1024];
FILE			*fp_d99_new =		NULL;
FILE			*fp_d99_old =		NULL;
static FILE		*frecids =		NULL;
static int		is_valid_dba;
static size_t		max_corruption =	MAX_CORRUPTION;
/***static long		max_totrecs = 0L;****/
static int		normal_exitcode =	0;
static int		need_linefeed =		FALSE;
static int		overlay_no =		FALSE;
static int		overlay_yes =		FALSE;
static size_t		reccount =		0L;
static short		recslots;
static int		dba_offset;
static size_t		recs_per_dot =	RECS_PER_DOT;
static int		rewrite_reccount =	FALSE;
static int		shutdown_now =		0;	/* = FALSE */
static size_t		size_d21_old =		0L;
static size_t		size_d22_old =		0L;
static size_t		size_d23_old =		0L;
static size_t		size_d99_old =		0L;
static time_t		timestart =		0L;
static long		total_num_addrs =	0L;
static int		validation_mode =	FALSE;

/*****************************************************************************
	structure words: from opera.h file (just FYI...)
	char	word[DtSrMAXWIDTH_WORD]	- unique word.
	long	word_offset - offset in a database addresses file for
			      a given word. the first address starts
			      at this position.
	int	num_free_slots - number of free slots in a database
	 			 addresses file for a given word.
	long	number_of_addrs - total number of addresses for a given
				  word. 
*****************************************************************************/


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
} /* signal_shutdown() */


/************************************************/
/*						*/
/*		   print_usage			*/
/*						*/
/************************************************/
/* Prints usage statement to stderr. */
static void	print_usage (void)
{
fprintf (aa_stderr,
"\nUSAGE: %s [options] dbname newpath\n"
"  Compresses unused d99 space and validates d00-d99 links.\n"
"  dbname: 1 - 8 char database name = the old d99/d2x files to be updated.\n"
"     Files found in local directory or DBFPATH environment variable.\n"
"  newpath: Specifies where the new d99/d2x files will be placed.\n"
"     If first char is not slash, path is relative to local directory.\n"
"OPTIONS:\n"
"  -pN: Progress dots printed every N records (default %lu).\n"
"     Complete progress message printed every %d dots.\n"
"  -oy: Authorizes overlaying preexisting d99/d2x files in newpath.\n"
"  -on: Forces exit if preexisting d99/d2x files in newpath.\n"
"  -v:  Validates d99 and d00 links, uncorrupts d99 file, and ensures\n"
"     accurate record count.  Also use -c0 to uncorrupt entire database.\n"
"  -v<fname>:  Same as -v but also writes all d00 recs unreferenced by d99\n"
"     to fname in format suitable for albeniz to extract into fzk format.\n"
/********
"  -mN: Changes max # database documents from curr count in database to N.\n"
*********/
"  -cN: Exits if more than N corrupted/incomplete links (default %d).\n"
"     Corruption limit turned off by -c0.\n"
"EXIT CODES:\n"
"  0: Complete success.  1: Warning.  2: Job never started.\n"
"  3-49: Job ended prematurely, old files ok, new files unusable.\n"
"  50-99: Fatal Error, even old database may be corrupted.\n"
"  100+: Ctrl-C, kill, and all other signal interrupts cause premature\n"
"     end, new files may be unusable.  Signal = exit code - 100.\n"

, aa_argv0, RECS_PER_DOT, DOTS_PER_MSG, MAX_CORRUPTION);
return;
} /* print_usage() */


/************************************************/
/*						*/
/*		  print_progress		*/
/*						*/
/************************************************/
/* Prints progress msg after dots or at end of job.
 * Label is "Final" or "Progress".
 */
static void	print_progress (char *label)
{
long	seconds;
int	compression;

seconds = time(NULL) - timestart;	/* total seconds elapsed */
if (seconds < 0L)
    seconds = 0L;

if ((float) bytes_in / (float) size_d99_old  >=  99.5)
    compression = 100;
else
    {
    compression = (int) (100. * (float) bytes_in / (float) size_d99_old);
    if (compression < 0 || compression > 100)
	compression = 0;
    }

TERMINATE_LINE();
fprintf (aa_stderr,
    "%s: %s Compression %d%% (about %lu KB) in %ld:%02ld min:sec.\n",
    aa_argv0, label, compression, bytes_in / 1000L,
    seconds / 60UL, seconds % 60UL);
if (*label == 'F')
    fprintf (aa_stderr, "%s: Counted %ld WORDS in %s.d99.\n",
	aa_argv0, reccount, arg_dbname);
return;
} /* print_progress() */


/************************************************/
/*						*/
/*		    end_of_job			*/
/*						*/
/************************************************/
/* Exits program.  Prints status messages before going down.
 * Should be called on even record boundaries whenever possible,
 * ie after record writes complete and shutdown_now > 0 (TRUE).
 */
static void	end_of_job (int exitcode, int show_flags)
{
TERMINATE_LINE();
if (exitcode >= 100)
    {
    fprintf (aa_stderr, PROGNAME"66 Aborting after interrupt signal %d.\n",
	exitcode - 100);
    }
if (validation_mode && corruption_count == 0L)
    fprintf (aa_stderr, "%s: No corrupted links detected.\n", aa_argv0);
if (corruption_count > 0L)
    {
    if (max_corruption > 0L  &&  corruption_count >= max_corruption)
	fprintf (aa_stderr, PROGNAME"193 Aborting at %ld corrupted links.\n",
	    corruption_count);
    else
	fprintf (aa_stderr, PROGNAME"194 "
	    "Detected%s %ld corrupted/incomplete link(s).\n",
	    (validation_mode)? " and corrected" : "",
	    corruption_count);
    }
if (show_flags & SHOW_PROGRESS)
    {
    print_progress ("Final");
    }
if (show_flags & SHOW_USAGE)
    print_usage();
if (show_flags & SHOW_EXITCODE)
    fprintf (aa_stderr, "%s: Exit code = %d.\n", aa_argv0, exitcode);
DtSearchExit (exitcode);
} /* end_of_job() */


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
static void	user_args_processor (int argc, char **argv)
{
char	*argptr;
int	oops = FALSE;
int	i;
time_t	stamp;
size_t	tempsize;

if (argc < 3) end_of_job (2, SHOW_USAGE);

/* parse all args that begin with a dash (-) */
while (--argc > 0)
    {
    argv++;
    argptr = argv[0];
    if (argptr[0] != '-') break;
    switch (tolower(argptr[1]))
	{
	case 'r':
	    if (strcmp(argptr, "-russell") == 0)	/* backdoor debug */
		debug_mode = TRUE;
	    else goto UNKNOWN_ARG;

	case 'm':
	    fprintf (aa_stderr,
		PROGNAME"301 The -m argument is no longer necessary.\n");
	    break;

	case 'o':
	    i = tolower (argptr[2]);
	    if (i == 'n') overlay_no = TRUE;
	    else if (i == 'y') overlay_yes = TRUE;
	    else
		{
		INVALID_ARG:
		fprintf (aa_stderr,
		    PROGNAME"177 Invalid %.2s argument.\n", argptr);
		oops = TRUE;
		}
	    break;

	case 'v':
	    validation_mode = TRUE;
	    if (argptr[2] != '\0')
		{
		  _Xltimeparams localtime_buf;
		  struct tm *time_ptr;

		if ((frecids = fopen (argptr+2, "w")) == NULL)
		    {
		    fprintf (aa_stderr, PROGNAME"802 Unable to open '%s' "
			"to output unreferenced d00 records: %s\n",
			argptr, strerror(errno));
		    oops = TRUE;
		    }
		time (&stamp);
		time_ptr = _XLocaltime(&stamp, localtime_buf);
		strftime (datestr, sizeof(datestr),
		    "%Y/%m/%d %H:%M", time_ptr);
		}
	    break;

	case 'p':
	    recs_per_dot = atol (argptr + 2);
	    if (recs_per_dot <= 0L)  goto INVALID_ARG;
	    break;

	case 'c':
	    tempsize = atol (argptr + 2);
	    if (tempsize < 0L)  goto INVALID_ARG;
	    max_corruption = tempsize;
	    break;

	UNKNOWN_ARG:
	default:
	    fprintf (aa_stderr, PROGNAME"159 Unknown argument: '%s'.\n", argptr);
	    oops = TRUE;
	    break;
	} /* end switch */
    } /* end parse of cmd line args */

/* Test how we broke loop.
 * There should still be 2 args past the ones
 * beginning with a dash: dbname and newpath.
 */
if (argc != 2)
    {
    if (argc <= 0)
	fputs (PROGNAME"210 Missing required dbname argument.\n", aa_stderr);
    if (argc <= 1)
	fputs (PROGNAME"211 Missing required newpath argument.\n", aa_stderr);
    if (argc > 2)
	fputs (PROGNAME"212 Too many arguments.\n", aa_stderr);
    oops = TRUE;
    }
if (oops) end_of_job (2, SHOW_USAGE);

/* DBNAME */
arg_dbname = argv[0];
if (strlen(arg_dbname) > 8)
    {
    fprintf (aa_stderr,
	 PROGNAME"229 Invalid database name '%s'.\n", arg_dbname);
    end_of_job (2, SHOW_USAGE);
    }

/* NEWPATH:
 * Oldpath and newpath are validated when the files
 * are copied and the database is opened.
 */
arg_newpath = argv[1];
return;
} /* user_args_processor() */


/************************************************/
/*						*/
/*		 validation_error()		*/
/*						*/
/************************************************/
/* Subroutine of validation_mode in main().
 * Prints d2x and d99 data at location of error.
 * Adjusts d2x counts for number of good addrs and free slots.
 */
static void	validation_error (DB_ADDR dbaorig)
{
DB_ADDR	slot;
is_valid_dba = FALSE;

slot = dbaorig >> 8;

 /* now retranslate back to real dba */
if (dbaorig != -1)
              slot = ((slot + 1) * recslots - dba_offset)
                      | (OR_D00 << 24);

fprintf (aa_stderr,
    "  DBA = %d:%ld (x%02x:%06lx),  orig addr val = x%08lx\n"
    "  Word='%c%s' offset=%ld addrs=%ld free=%d\n",
    OR_D00, slot, OR_D00, slot, dbaorig,
    (!isgraph(d23old.or_hwordkey[0]))? '^' : d23old.or_hwordkey[0],
    d23old.or_hwordkey+1, d23old.or_hwoffset,
    d23old.or_hwaddrs, d23old.or_hwfree);
if (--d23new.or_hwaddrs < 0L) d23new.or_hwaddrs = 0L;
				/* (should never occur) */
d23new.or_hwfree++;

return;
} /* validation_error() */


/************************************************/
/*						*/
/*		  open_all_files		*/
/*						*/
/************************************************/
static void	open_all_files
    (FILE **fp, char *fname, char *mode, size_t *size, int *oops)
{
struct stat	fstatbuf;

if ((*fp = fopen (fname, mode)) == NULL)
    {
    fprintf (aa_stderr, PROGNAME"439 Can't open %s: %s\n",
	fname, strerror(errno));
    *oops = TRUE;
    return;
    }
if (fstat (fileno(*fp), &fstatbuf) == -1)
    {
    fprintf (aa_stderr, PROGNAME"440 Can't access status of %s: %s\n",
	fname, strerror(errno));
    *oops = TRUE;
    return;
    }
if (size)
  if ((*size = fstatbuf.st_size) <= 0L)
    {
    fprintf (aa_stderr, PROGNAME"499 %s is empty.\n", fname);
    *oops = TRUE;
    }
return;
} /* open_all_files() */


/************************************************/
/*						*/
/*	      copy_old_d2x_to_new		*/
/*						*/
/************************************************/
static void	copy_old_d2x_to_new
    (char *fname_old, char *fname_new, FILE *fp_old, FILE *fp_new)
{
char	readbuf [READBUFSIZE];
int	i, j;

fprintf (aa_stderr, "%s: Copying from old d2x files to %s...\n",
    aa_argv0, fname_new );
for (;;)	/* loop ends when eof set on input stream */
    {
    errno = 0;
    i = fread (readbuf, 1, sizeof(readbuf), fp_old);
    if (errno)
	{
	fprintf (aa_stderr, PROGNAME"517 Read error on %s: %s.\n", 
	    fname_old, strerror(errno));
	end_of_job (3, SHOW_EXITCODE);
	}
    j = fwrite (readbuf, 1, i, fp_new);
    if (i != j)
	{
	fprintf (aa_stderr, PROGNAME"489 Write error on %s: %s.\n", 
	    fname_new, strerror(errno));
	end_of_job (3, SHOW_EXITCODE);
	}
    if (shutdown_now)
	end_of_job (shutdown_now, SHOW_EXITCODE);
    if (feof(fp_old))
	break;
    }
TERMINATE_LINE();
fclose (fp_old);
fclose (fp_new);
return;
} /* copy_old_d2x_to_new() */


/********************************/
/*				*/
/*	   read_d2x		*/
/*				*/
/********************************/
/* Performs vista RECREAD on curr word record.
 * CALLER SHOULD CHECK DB_STATUS.
 */
void	read_d2x (struct or_hwordrec *glob_word, long field)
{
if (field == OR_SWORDKEY)
	{
	RECREAD (PROGNAME"61", &d21old, 0);
	if (db_status != S_OKAY)
		return;
	strncpy (glob_word->or_hwordkey, d21old.or_swordkey,
		DtSrMAXWIDTH_HWORD);
	glob_word->or_hwordkey [DtSrMAXWIDTH_HWORD-1] = 0;
	glob_word->or_hwoffset = d21old.or_swoffset;
	glob_word->or_hwfree = d21old.or_swfree;
	glob_word->or_hwaddrs = d21old.or_swaddrs;
	}
else if (field == OR_LWORDKEY) {
	RECREAD (PROGNAME"69", &d22old, 0);
	if (db_status != S_OKAY)
		return;
	strncpy (glob_word->or_hwordkey, d22old.or_lwordkey,
		DtSrMAXWIDTH_HWORD);
	glob_word->or_hwordkey [DtSrMAXWIDTH_HWORD-1] = 0;
	glob_word->or_hwoffset = d22old.or_lwoffset;
	glob_word->or_hwfree = d22old.or_lwfree;
	glob_word->or_hwaddrs = d22old.or_lwaddrs;
	}
else	{
	RECREAD (PROGNAME"78", glob_word, 0);
	glob_word->or_hwordkey [DtSrMAXWIDTH_HWORD-1] = 0;
	}
return;
} /* read_d2x() */


/********************************/
/*				*/
/*	   write_d2x	*/
/*				*/
/********************************/
/* performs vista RECWRITE on curr word record.
 * CALLER MUST CHECK DB_STATUS.
 */
static void	write_d2x (struct or_hwordrec *glob_word, long field)
{
if (field == OR_SWORDKEY) {
	strcpy (d21new.or_swordkey, glob_word->or_hwordkey);
	d21new.or_swoffset = glob_word->or_hwoffset;
	d21new.or_swfree = glob_word->or_hwfree;
	d21new.or_swaddrs = glob_word->or_hwaddrs;
	RECWRITE (PROGNAME"102", &d21new, 0);
	}
else if (field == OR_LWORDKEY) {
	strcpy (d22new.or_lwordkey, glob_word->or_hwordkey);
	d22new.or_lwoffset = glob_word->or_hwoffset;
	d22new.or_lwfree = glob_word->or_hwfree;
	d22new.or_lwaddrs = glob_word->or_hwaddrs;
	RECWRITE (PROGNAME"111", &d22new, 0);
	}
else    {
        RECWRITE (PROGNAME"115", glob_word, 0);
        }
return;
} /* write_d2x() */



/************************************************/
/*						*/
/*		    copy_new_d99()		*/
/*						*/
/************************************************/
/* The garbage collection/compression process itself.
 * For very large databases, there will be appx 3 million word records,
 * so the loop should be coded for ***EFFICIENCY***.
 */
    /* fool the read_wordstr() function by using a phony word
     * whose length tells the function which read to do.
     * This code really sucks but I don't have time to fix it.
     */
static void	copy_new_d99 (long keyfield)
{
int		is_odd_nibble;
long		num_holes;
long		slots_left;
long		good_addrs_this_block;
unsigned char	*bvptr;
int		a;
DB_ADDR		dba, dbaorig;
long		x;
int		done;
long		good_addrs_left;
int		num_reads, num_writes;
DB_ADDR		word_addrs [MAX_REC_READ + 64];		/* d99 read buf */
DB_ADDR		word_addrs_out [MAX_REC_READ + 64];	/* d99 write buf */

KEYFRST (PROGNAME"179", keyfield, 0);
while (db_status == S_OKAY)
    {
     /********RECREAD (PROGNAME"182", &d02new, 0);********/
    read_d2x (&d23new, keyfield);
    if (validation_mode)	/* save for validation err msgs */
	memcpy (&d23old, &d23new, sizeof(d23old));

/****************
@@@@@ START HERE NEXT
@@@@@ USE OR_D2x instead of keyfiled OR_SWROD etc so we can
@@@@@ correctly call validation_error()
****************/

    /* Read old d99 file at specified offset to get total num "holes".
     * In the first portion of record holes are filled with
     * representations of valid database addresses + statistical weights.
     * In the second portion the holes are "free slots" for future
     * expansion which are conventionally initialized with a -1.
     */
    /* force number of free slots to 0(ZERO) */
    d23new.or_hwfree = 0L;
    fseek (fp_d99_old, d23new.or_hwoffset, SEEK_SET);
    num_holes = d23new.or_hwaddrs + (long )d23new.or_hwfree;
    good_addrs_left = d23new.or_hwaddrs;
    bytes_in += sizeof(DB_ADDR) * num_holes;

    /* Update the offset in the d2x record buffer */
    d23new.or_hwoffset = ftell (fp_d99_new);

    /* Copy the array of holes in each disk block,
     * reading the old and writing to the new.  Loop ends
     * when the number of holes left will fit into one last block.
     */
    done = FALSE;
    while (!done)	/* loop on each block in this word */
	{
	if (num_holes > MAX_REC_READ)
	    {
	    num_reads = MAX_REC_READ;
	    num_holes -= MAX_REC_READ;
	    }
	else
	    {
	    done = TRUE;
	    num_reads = num_holes;
	    }
	errno = 0;
	fread (word_addrs, sizeof(DB_ADDR), num_reads, fp_d99_old);
	if (errno)
	    {
	    TERMINATE_LINE();
	    fprintf (aa_stderr, PROGNAME"657 Read error on %s: %s.\n", 
		fname_d99_old, strerror(errno));
	    end_of_job (4, SHOW_PROGRESS + SHOW_EXITCODE);
	    }

	/* Addrs on d99 are now 'record numbers' not dbas.
	 * A rec# is what the dba/slot# would be if records
	 * took up just one slot and there were no dbrec at
	 * start of file.  D99 rec#s start at #1, not #0.
 	 */
	/* If user requested validation_mode, validate each 'good' rec#
	 * (not free slots) in word_addrs buffer.  If any d99 links
	 * are corrupt, skip them when copying to the new d99 file.
	 * Rewrite -1's to all free slots.
	 * ----> NOTE UNUSUAL FORMAT OF DBA HOLES IN D99! <----
	 * Record number is shifted to the high order 3 bytes.
	 * The statistical weight is in the low order byte.
	 * The vista file number is known from the #define constant OR_D00,
	 * and the vista dba/slot# is mapped from rec# by mult/div
	 * number of slots per rec, plus/minus dbrec offset.
	 */
	if (validation_mode)
	    {
	    /* set x to number of good addrs in this block */
	    if (good_addrs_left > num_reads)
		{
		x = num_reads;
		good_addrs_left -= num_reads;
		}
	    else
		{
		x = good_addrs_left;
		good_addrs_left = 0;
		}

	    /* Validate the rec#'s in this block.  Note that the loop
	     * is skipped if the entire block is free slots.
	     */
	    good_addrs_this_block = 0;
	    for (a=0; a<x; a++)		/* a = index to curr dba */
		{
		/* Get rec#.  Save original rec# for err msgs, then shift
		 * slot number to lower 3 bytes, discarding weight.
		 */
		dbaorig = word_addrs[a];	/* rec#,rec#,rec#:wt */
		dba = dbaorig >> 8;		/* 0,rec#,rec#,rec# */
		is_valid_dba = TRUE;	/* default */

		/* If original rec# == -1 we've overrun the good
		 * rec#'s into the expansion area, which is filled
		 * with -1's.  This is real bad news because if
		 * the counts in d02 are bad, the online programs
		 * will quickly crash, and we can't continue this program.
		 * Advance to next rec# because we can't mark the bit vector.
		 */
		if (dbaorig == -1L)
		    {
		    TERMINATE_LINE();
		    fprintf (aa_stderr, "*** "PROGNAME "111 DBA in d99 = -1.  "
			"Probable overrun into expansion area\n"
			"  due to incorrect count values in d2x file.\n");
		    validation_error (dbaorig);
		    corruption_count++;
		    if (max_corruption > 0L  &&
		      corruption_count >= max_corruption)
			end_of_job (91, SHOW_PROGRESS + SHOW_EXITCODE);
		    continue;	/* skip the bit vector check */
		    }

		/* If slot number > max totrecs, we have a
		 * corrupted d99-d00 link because we've
		 * already validated the d00 file and we know that
		 * it has no slots > max.  Also we have to advance
		 * to next slot because we can't mark the bit vector.
		 */
/******		if (dba >= max_totrecs)*******/
		if (dba >= total_num_addrs)
		    {
		    TERMINATE_LINE();
		    fprintf (aa_stderr, "*** "PROGNAME"222 "
			"DBA in d99 not in d00, slot > max num docs.\n");
		    validation_error (dbaorig);
		    corruption_count++;
		    if (max_corruption > 0L  &&
		      corruption_count >= max_corruption)
			end_of_job (92, SHOW_PROGRESS + SHOW_EXITCODE);
		    continue;	/* skip the bit vector check */
		    }

		/* Verify that dba exists in d00 file (test bit #1).
		 * If not, mark bit #3 (3rd lowest) in nibble and
		 * print error msg unless bit #3 previously marked.
		 */
		bvptr = bit_vector + (dba >> 1);
		is_odd_nibble = (dba & 1L);
		if (!(*bvptr & ((is_odd_nibble)? 0x01 : 0x10)))	/* bit #1 */
		    {
		    if (!(*bvptr & ((is_odd_nibble)? 0x04 : 0x40)))/* bit #3 */
			{
			*bvptr |= (is_odd_nibble)? 0x04 : 0x40;
			TERMINATE_LINE();
			fprintf (aa_stderr, "*** "PROGNAME"333 "
			    "DBA in d99 does not exist in d00.\n");
			validation_error (dbaorig);
			corruption_count++;
			if (max_corruption > 0L  &&
			  corruption_count >= max_corruption)
			    end_of_job (93, SHOW_PROGRESS + SHOW_EXITCODE);
			} /* endif where corrupt link detected */
		    }

		/* Mark bit #2 in bit vector indicating a d99 reference. */
		*bvptr |= (is_odd_nibble)? 0x02 : 0x20;		/* bit #2 */

		/* move good dba to curr output block, incr counter */
		if (is_valid_dba)
		    word_addrs_out [good_addrs_this_block++] = dbaorig;

		} /* end validation loop for each good dba in the block */

	    /* Write out only validated addrs in current block.
	     * If this was the last block, fill out all the free slots,
	     * if any, with -1 values, and exit the dba loop for this word.
	     */
	    if (good_addrs_this_block > 0L)
		{
		num_writes = fwrite (word_addrs_out, sizeof(DB_ADDR),
		    good_addrs_this_block, fp_d99_new);
		if (num_writes != good_addrs_this_block) goto WRITE_ERROR;
		}
	    if (good_addrs_left <= 0L)
		{
		/* Write blocks of -1s until new d2x free slot count
		 * is exhausted.  The last block may be < MAX_REC_READ.
		 */
		slots_left = d23new.or_hwfree;
		while (slots_left > 0L)
		    {
		    /* set x to number of -1's to write for this block */
		    if (slots_left > MAX_REC_READ)
			{
			x = MAX_REC_READ;
			slots_left -= MAX_REC_READ;
			}
		    else
			{
			x = slots_left;
			slots_left = 0L;
			}
		    for (a=0; a<x; a++) word_addrs_out[a] = -1L;
		    num_writes = fwrite
			(word_addrs_out, sizeof(DB_ADDR), x, fp_d99_new);
		    if (num_writes != x) goto WRITE_ERROR;
		    } /* end while loop to write out all -1's */
		done = TRUE;
		}
	    } /* endif for validation_mode for this block */

	/* If NOT in validation mode, just write out the new d99 block
	 * as an exact copy of the input block.
	 */
	else
	    {
	    num_writes =
		fwrite (word_addrs, sizeof(DB_ADDR), num_reads, fp_d99_new);
	    if (num_writes != num_reads)
		{
		WRITE_ERROR:
		fprintf (aa_stderr, PROGNAME"665 Write error on %s: %s.\n",
		fname_d99_new, strerror(errno));
		end_of_job (4, SHOW_PROGRESS + SHOW_EXITCODE);
		}
	    } /* endelse for NOT validation_mode for this block */

	} /* end loop for all blocks for this entire word (done = TRUE) */ 

    /* write the updated d2x record */
    /*****RECWRITE (PROGNAME"184", &d23new, 0);******/
    write_d2x (&d23new, keyfield);
    reccount++;

    /* Every now and then print a dot.
     * Print complete progress msg after DOTS_PER_MSG dots.
     */
    if (!(reccount % recs_per_dot))
	{
	if (++dot_count > DOTS_PER_MSG)
	    {
	    dot_count = 0;
	    print_progress ("Progress");
	    }
	else
	    {
	    fputc ('.', aa_stderr);
	    need_linefeed = TRUE;
	    if (!(dot_count % 10L)) fputc (' ', aa_stderr);
	    }
	fflush (aa_stderr);
	} /* end of print-a-dot */

    if (shutdown_now)
	end_of_job (shutdown_now, SHOW_PROGRESS + SHOW_EXITCODE);
    KEYNEXT (PROGNAME"196", keyfield, 0);
    } /* end of main loop on each word in database */


return;
} /* copy_new_d99() */


/************************************************/
/*						*/
/*		      main()			*/
/*						*/
/************************************************/
int	main (int argc, char *argv[])
{
FILE_HEADER	fl_hdr;
int		a, i, j;
unsigned char	*bvptr;
DB_ADDR		dba, dba1, dbaorig;
char		dbfpath [1024];
char		fname_d21_new [1024];
char		fname_d21_old [1024];
char		fname_d22_new [1024];
char		fname_d22_old [1024];
char		fname_d23_new [1024];
char		fname_d23_old [1024];
FILE		*fp_d21_new = NULL;
FILE		*fp_d21_old = NULL;
FILE		*fp_d22_new = NULL;
FILE		*fp_d22_old = NULL;
FILE		*fp_d23_new = NULL;
FILE		*fp_d23_old = NULL;
char		full_dbname_old [1024];
char		full_dbname_new [1024];
long		max_bitvec = 0L;
int		oops;
char		*ptr;
char		readbuf [READBUFSIZE];
unsigned long	reads_per_dot;
char		recidbuf [DtSrMAX_DB_KEYSIZE + 4];
time_t		starttime;
long		x;
struct or_dbrec	dbrec;
struct tm 	*time_ptr;
_Xltimeparams	localtime_buf;

if (beta > 1)
    sprintf (betabuf, "%c", beta);
else
    betabuf[0] = 0;
aa_argv0 = argv[0];

    time (&starttime);
    time_ptr = _XLocaltime(&starttime, localtime_buf);
    strftime (dbfpath, sizeof (dbfpath),   /* just use any ol' buffer */
        catgets (dtsearch_catd, MS_misc, 22, "%A, %b %d %Y, %I:%M %p"),
        time_ptr);
    printf (catgets (dtsearch_catd, MS_misc, 23,
        "%s: Version %s%s.  Run %s.\n"),
        aa_argv0, AUSAPI_VERSION, betabuf, dbfpath);


signal (SIGHUP, signal_shutdown);
signal (SIGINT, signal_shutdown);
signal (SIGQUIT, signal_shutdown);
signal (SIGTRAP, signal_shutdown);
signal (SIGABRT, signal_shutdown);	/* ordinarily this causes core dump */
signal (SIGKILL, signal_shutdown);	/* this cannot be trapped */
signal (SIGALRM, signal_shutdown);
signal (SIGTERM, signal_shutdown);
signal (SIGPWR, signal_shutdown);
signal (SIGUSR1, signal_shutdown);	/* ordinarily this "pings" OE */
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
if ((ptr = getenv ("DBFPATH")) != NULL) 
    {
    if (*ptr == 0)
	fprintf (aa_stderr,
	"%s: Ignoring empty DBFPATH environment variable.\n",
	aa_argv0);
    else
	{
	fprintf (aa_stderr, "%s: Using DBFPATH = '%s'.\n",
	    aa_argv0, ptr);
	strcpy (full_dbname_old, ptr);

	/* Ensure that DBFPATH ends in a slash. */
	ptr = strchr (full_dbname_old, '\0');
	if (*(ptr-1) != LOCAL_SLASH)
	    {
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
if (*(ptr-1) != LOCAL_SLASH)
    {
    *ptr++ = LOCAL_SLASH;
    *ptr = '\0';
    }
if (strcmp (full_dbname_old, full_dbname_new) == 0)
    {
    fprintf (aa_stderr,
	PROGNAME"393 Old and new directories are identical: '%s'\n.",
	full_dbname_old);
    end_of_job (2, SHOW_USAGE);
    }

/* Complete full_dbname_old by appending dbname to the path prefix.
 * Then build full path/file names for all 4 files.
 */
strcat (full_dbname_old, arg_dbname);
strcat (full_dbname_new, arg_dbname);
fprintf (aa_stderr, "%s: Old files: '%s.d2x, .d99'.\n",
    aa_argv0, full_dbname_old);
fprintf (aa_stderr, "%s: New files: '%s.d2x, .d99'.\n",
    aa_argv0, full_dbname_new);

strcat (fname_d99_old, full_dbname_old);
strcat (fname_d99_old, ".d99");
strcpy (fname_d21_old, full_dbname_old);
strcat (fname_d21_old, ".d21");
strcpy (fname_d22_old, full_dbname_old);
strcat (fname_d22_old, ".d22");
strcpy (fname_d23_old, full_dbname_old);
strcat (fname_d23_old, ".d23");
strcat (fname_d99_new, full_dbname_new);
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
if (!overlay_yes)
    {
    oops = FALSE;	/* TRUE forces a user prompt */
    if ((fp_d99_new = fopen(fname_d99_new, "r")) != NULL)
	{
	fclose (fp_d99_new);
	oops = TRUE;
	}
    if ((fp_d21_new = fopen(fname_d21_new, "r")) != NULL)
	{
	fclose (fp_d21_new);
	oops = TRUE;
	}
    if ((fp_d22_new = fopen(fname_d22_new, "r")) != NULL)
	{
	fclose (fp_d22_new);
	oops = TRUE;
	}
    if ((fp_d23_new = fopen(fname_d23_new, "r")) != NULL)
	{
	fclose (fp_d23_new);
	oops = TRUE;
	}
    if (oops)
	{
	fprintf (aa_stderr, "%s: One or more new files already exist.\n",
	    aa_argv0);
	if (overlay_no)
	    {
	    fputs (PROGNAME"463 "
		"Command line argument disallows file overlay.\n",
		aa_stderr);
	    end_of_job (2, SHOW_EXITCODE);
	    }
	fputs ("    Is it ok to overlay files in new directory? [y/n] ",
	    aa_stderr);

        *readbuf = '\0';
        fgets (readbuf, sizeof(readbuf), stdin);
        if (strlen(readbuf) && readbuf[strlen(readbuf)-1] == '\n')
          readbuf[strlen(readbuf)-1] = '\0';

	if (tolower (*readbuf) != 'y') end_of_job (2, SHOW_NOTHING);
	}
    } /* end of check for overlaying new files */

/* Open all files.  The d2x's are opened so that the old ones
 * can be copied into the new directory before starting
 * the garbage collection process proper.
 * The d99's are opened now just to verify permissions.
 */
oops = FALSE;	/* TRUE ends job, but only after trying all 4 files */
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

/* Open database, but use new d02 file for updates. */
if (!austext_dopen (arg_dbname, (dbfpath[0] == 0)? NULL : dbfpath,
	arg_newpath, 0, &dbrec))
    {
    puts (DtSearchGetMessages());
    end_of_job (3, SHOW_EXITCODE);
    }

/* Use the number of records in the dbrec, unless
 * user specified a larger number on command line.
 * Get other constants from dbrec.
 */
/********************
if (max_totrecs == 0)
    {
    max_totrecs = dbrec.or_reccount + 24;
    fprintf (aa_stderr,
	"%s: %ld records in database.  Using this as default for -m.\n",
	aa_argv0, dbrec.or_reccount);
    }
**********************/

/* this is where real dba was changed to record number (still called dba) */
RECFRST (PROGNAME"1067", OR_OBJREC, 0);
CRGET (PROGNAME"1068", &dba, 0);	/* dba of first real obj record */
recslots = dbrec.or_recslots;	/* vista slots per obj record */
dba_offset = recslots - (dba & 0xffffff);	/* accounts for dbrec */

/* total_num_addrs = what reccount would be if
 * all holes were filled with good records.
 */
total_num_addrs = (dbrec.or_maxdba - (dba & 0xffffff) + 1) / recslots + 1;
fprintf (aa_stderr,
    "%s: curr reccnt=%ld, mxdba=%ld, sl/rec=%ld, tot#adr=%ld.\n",
    aa_argv0, dbrec.or_reccount, dbrec.or_maxdba,
    dbrec.or_recslots, total_num_addrs);

/* Initialize validation_mode (checkd99) */
if (validation_mode)
    {
    /* Allocate and initialize a bit vector:
     * 4 bits for every possible d00 database address.
     */
    max_bitvec = (total_num_addrs >> 1) + 2;
    if ((bit_vector = malloc (max_bitvec + 64L)) == NULL)
	{
	fprintf (aa_stderr, PROGNAME"465 WARNING: "
	    "Can't allocate memory for bit vector. "
	    "'Validate' mode switched off.\n");
	validation_mode = FALSE;
	normal_exitcode = 1;	/* warning */
	goto EXIT_INIT_VALIDATION;
	}
    memset (bit_vector, 0, max_bitvec);

    /* Read every d00 rec sequentially.  1 in bit #1 (lowest order)
     * in bit vector means record (dba) exists in d00 file.
     * While we're at it, count the total number of records.
     */
    x = dbrec.or_reccount / 50L + 1;	/* x = recs per dot */
    fprintf (aa_stderr,
	"%s: Reading d00 file.  Each dot appx %ld database documents...\n",
	aa_argv0, x);
    reccount = 0L;
    dot_count = 0L;
    RECFRST (PROGNAME"534", OR_OBJREC, 0);
    while (db_status == S_OKAY)
	{
	CRREAD (PROGNAME"617", OR_OBJKEY, recidbuf, 0);

	/* print periodic progress dots */
	if (!(++reccount % x))
	    {
	    fputc ('.', aa_stderr);
	    need_linefeed = TRUE;
	    if (!(++dot_count % 10L)) fputc (' ', aa_stderr);
	    fflush (aa_stderr);
	    }

	/* Get dba and record number and confirm
	 * it will not overflow bit vector.
	 */
	CRGET (PROGNAME"537", &dba, 0);
	dba &= 0x00ffffff;	/* mask out file number in high order byte */
	dba1 = (dba + dba_offset) / recslots;	/*  = "rec number", base 1 */
/*****	if (dba1 >= max_totrecs)*****/
	if (dba1 >= total_num_addrs)
	    {
	    TERMINATE_LINE();
	    fprintf (aa_stderr, PROGNAME"561 "
		"DBA '%d:%ld' (rec #%ld) in d00 exceeds total num addrs %ld;\n"
		"  Bit vector overflow because maxdba %ld"
		" in dbrec is incorrect.\n",
/******		OR_D00, dba, dba1, max_totrecs);******/
		OR_D00, dba, dba1, total_num_addrs, dbrec.or_maxdba);
	    end_of_job (7, SHOW_EXITCODE);
	    }
	if (shutdown_now)
	    end_of_job (shutdown_now, SHOW_EXITCODE);

	/* Set bit #1 of even or odd nibble to indicate that
	 * this record *number* actually exists in d00 file.
	 */
	bit_vector[dba1>>1] |= (dba1 & 1L)? 0x01 : 0x10;

	RECNEXT (PROGNAME"541", 0);
	} /* end of sequential read thru d00 file */

    TERMINATE_LINE();	/* end the dots... */

    /* confirm that RECCOUNT record holds the correct number */
    if (dbrec.or_reccount == reccount)
	    {
	    fprintf (aa_stderr,
		"%s: Confirmed %ld DOCUMENTS in %s.d00.\n",
		aa_argv0, dbrec.or_reccount, arg_dbname);
	    }
    else
	    {
	    fprintf (aa_stderr,
		"%s: %ld DOCUMENTS actually in %s.d00 not ="
		" %ld count stored there.\n"
		"  Count will be corrected in new d00 file.\n",
		aa_argv0, reccount, arg_dbname, dbrec.or_reccount);
	    dbrec.or_reccount = reccount;
	    rewrite_reccount = TRUE;
	    }

    EXIT_INIT_VALIDATION: ;
    } /* end of validation_mode initialization */

/* initialize main loop */
time (&timestart);
reccount = 0L;
bytes_in = 0L;
dot_count = DOTS_PER_MSG;    /* force initial msg after first blk of recs */
TERMINATE_LINE();
fprintf (aa_stderr, "%s: Compressing into %s.  Each dot appx %lu words...\n",
    aa_argv0, arg_newpath, recs_per_dot);

/* write New Header Information to a new d99 file */
init_header(fp_d99_new, &fl_hdr);

/* Sequentially read each word key file in big loop.
 * For each word, read the d99.
 * In validation mode check the dbas.
 * If not validating, just blindly rewrite the old d99 to the new one.
 * If validating only write good dba's and mark the bit vector.
 */
copy_new_d99 (OR_SWORDKEY);
copy_new_d99 (OR_LWORDKEY);
copy_new_d99 (OR_HWORDKEY);


if (reccount == 0L)
    end_of_job (50, SHOW_PROGRESS + SHOW_EXITCODE);
else
    print_progress ("Final");

/* If validation_mode requested, traverse bit vector and print out
 * table of each d00 record which cannot be accessed from any d99 word.
 * If a validation file name was provided, write out a line for each
 * bad reecord in alebeniz-compatible format.
 */
if (validation_mode)
    {
    for (x=0L, bvptr = bit_vector;  x<max_bitvec;  x++, bvptr++)
	{
	for (j=0; j<8; j+=4)	/* j = 0 or 4, amount of bit shift */
	    {
	    /* a = bits #1 and #2 of current nibble */
	    a = 0x30 & (*bvptr << j);

	    /* if dba is in d00 but not in d99... */
	    if (a & 0x10 && !(a & 0x20))
		{
		/* ...construct valid vista dba */
		dbaorig = x << 1;
		if (j) dbaorig++;		/* slot number */
	/***	dba = dbaorig | (OR_D00 << 24);	***/ /* real dba */

		/* now retranslate back to real dba */
                dba = ((dbaorig + 1) * recslots - dba_offset)
                        | (OR_D00 << 24);

		/* ...print out err msg */
		/***printf("DBAORIG = %ld DBA = %ld\n", dbaorig, dba);***/
		CRSET (PROGNAME"734", &dba, 0);
		CRREAD (PROGNAME"735", OR_OBJKEY, readbuf, 0);
		fprintf (aa_stderr, "*** "PROGNAME"444 "
		    "d00 record '%s' is not referenced in d99.\n"
		    "  DBA = %d:%ld (x%02x:%06lx).\n",
		    readbuf, OR_D00, dba, OR_D00, dba);
		    /***readbuf, OR_D00, dbaorig, OR_D00, dbaorig);****/

		/* ...if albeniz compatible output requested, do it */
		if (frecids)
		    {
		    fprintf (frecids, DISCARD_FORMAT, arg_dbname,
			readbuf, "MrClean", datestr);
		    }

		corruption_count++;
		if (max_corruption > 0L && corruption_count >= max_corruption)
		    end_of_job (94, SHOW_EXITCODE);
		} /* endif where d00 is not referenced by d99 */
	    } /* end forloop: every 2 bits in a bitvector byte */
	} /* end forloop: every byte in bitvector */
    }

/* Normal_exitcode currently will contain either a 0 or a 1.
 * If we were uncorrupting the d99 and found any corrupt links,
 * make sure it's 1 (warning).  If there were corrupt links and
 * we weren't trying to uncorrupt it, change it to a hard error.
 */
/***@@@@ by the way, corruption_count can be > 0 only if in validation_mode */
if (corruption_count > 0L)
    {
    if (validation_mode)
	normal_exitcode = 1;
    else
	normal_exitcode = 90;
    }
end_of_job (normal_exitcode, SHOW_EXITCODE);
} /* main() */

/*************************** MRCLEAN.C ****************************/
