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
 *   FUNCTIONS: UPDATE_MAXDBA
 *		count_all_records
 *		create_object
 *		load_next_miscrec
 *		main
 *		print_exit_code
 *		print_progress
 *		read_dbrec
 *		segregate_dicname
 *		update_object
 *		user_args_processor
 *		write_dbrec
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
/*********************** DTSRLOAD.C ***************************
 * $XConsortium: dtsrload.c /main/8 1996/09/23 21:04:17 cde-ibm $
 * October 1993.
 * Formerly dtsrload.c was cravel.c.
 * Input: Standard AusText .fzk file.
 * Function: Adds to or updates corresponding DtSearch-
 * AusText database records.
 *
 * $Log$
 * Revision 2.7  1996/03/25  18:54:44  miker
 * Changed FILENAME_MAX to _POSIX_PATH_MAX.
 *
 * Revision 2.6  1996/03/13  22:53:47  miker
 * Changed char to UCHAR several places.
 *
 * Revision 2.5  1996/02/01  18:46:02  miker
 * AusText 2.1.11, DtSearch 0.3.  Changed document text reads from fgets
 * to new single character reading functions to match dtsrindex.
 * Added -t etx delimiter string command line arg.
 *
 * Revision 2.4  1995/12/01  16:18:22  miker
 * Added fflush for stdout and stderr for clean printing to AusBuild log.
 *
 * Revision 2.3  1995/10/26  17:48:45  miker
 * Fixed duplicate msgs catopen().
 *
 * Revision 2.2  1995/10/25  18:39:52  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  19:31:48  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/20  22:52:47  miker
 * Fixed bug: DtSrFlNOTAVAIL was being set in wrong obj field.
 *
 * Revision 1.2  1995/09/19  21:59:53  miker
 * Set DtSrFlNOTAVAIL when appropriate for doc.
 * If DtSearch, use DtSrVERSION instead of AUSAPI_VERSION in banner.
 *
 * Revision 1.1  1995/08/31  20:52:34  miker
 * Initial revision
 *
 * Revision 1.12  1995/06/08  19:42:44  miker
 * 2.1.5f: Removed -w option.  It no longer had an effect.
 */
#include "SearchP.h"
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <sys/stat.h>
#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include "vista.h"
#include <sys/types.h>
#include <netinet/in.h>

#define PROGNAME        "DTSRLOAD"
#define RECS_PER_DOT    20
#define TERMINATE_LINE  if (dotcount>0) { putchar('\n'); }
#define EXIT_NORMAL	0	/* perfect return code */
#define EXIT_WARNING	1	/* functioned ok, but with warnings */
#define EXIT_VANISH	3	/* input file effectively empty */
#define MS_misc		1
#define MS_cravel	11

/*--------------- EXTERNS ------------------*/
extern volatile int
		shutdown_now;
extern void     gen_vec (char *fname_huffcode_tab);
extern long     gen_vec_hufid;

/*--------------- GLOBALS ------------------*/
static char    *abstrbuf = NULL;
static int      blobs_are_used;	/* boolean */
static long     created_reccount = 0L;
static long     dbrec_hufid = 1L;
unsigned long   default_hashsize;
int             debug_mode = FALSE;
int             debug_encode = FALSE;
static char     dicname[10];	/* 1 - 8 char database name */
char            dicpath[_POSIX_PATH_MAX];
static int      dotcount = 0;
static long     duplicate_recids = 0L;
char            fname_huffcode_tab[_POSIX_PATH_MAX];
char            fname_input[_POSIX_PATH_MAX];
struct stat     fstat_input;
static FILE    *infile = NULL;
static long     input_reccount = 0L;
static DtSrINT32
		maxdba = 0;
static int      need_final_progress_msg = TRUE;
static int      normal_exitcode = EXIT_NORMAL;
static DtSrINT32
		objsize =	0;
static DtSrObjdate
		objdate = 0;
static DB_ADDR  objdba = NULL_DBA;
static PARG	parg;
static int      recs_per_dot = RECS_PER_DOT;
static time_t   starttime = 0L;
static DtSrObjdate
		starttimeobjd = 0;
char            sprintbuf[1024 + _POSIX_PATH_MAX];
static int	sumblobs =	0;
static int	sumlines =	0;
static DtSrINT32
		system_reccount = 0;
static long	updated_reccount = 0L;

struct or_dbrec dbrec;
struct or_objrec objrec;
struct or_miscrec miscrec;
struct or_blobrec blobrec;

/********************************************************/
/*							*/
/*		    UPDATE_MAXDBA			*/
/*							*/
/********************************************************/
/* Ensures global var 'maxdba' always contains highest D00 slot number */
#define UPDATE_MAXDBA(dba)  {if((dba&0xffffff)>maxdba)maxdba=dba&0xffffff;}


/********************************************************/
/*                                                      */
/*                segregate_dicname                     */
/*                                                      */
/********************************************************/
/* Separates dictionary name from pathname and loads
 * them into the globals 'dicname' and 'dicpath'.
 * Returns TRUE if dicname is valid, else returns FALSE.
 */
static int      segregate_dicname (char *string)
{
    char           *ptr;
    int             i;

    strncpy (dicpath, string, sizeof (dicpath));
    dicpath[sizeof (dicpath) - 1] = 0;

    /* Set 'ptr' to just the 8 char dictionary name by moving
     * it backwards until first non-alphanumeric character
     * (such as a ":" in the dos drive id or a slash between directories),
     * or to the beginning of string.
     */
    for (ptr = dicpath + strlen (dicpath) - 1; ptr >= dicpath; ptr--)
	if (!isalnum (*ptr)) {
	    ptr++;
	    break;
	}
    if (ptr < dicpath)
	ptr = dicpath;

    /* test for valid dictionary name */
    i = strlen (ptr);
    if (i < 1 || i > 8)
	return FALSE;

    strcpy (dicname, ptr);
    *ptr = 0;	/* truncate dicname off of full path/dicname */
    return TRUE;
}  /* segregate_dicname() */


/********************************************************/
/*                                                      */
/*                 user_args_processor                  */
/*                                                      */
/********************************************************/
/* handles command line arguments for 'main' */
static void     user_args_processor (int argc, char **argv)
{
    char           *argptr;
    char           *src, *targ;

    if (argc <= 1) {
PRINT_USAGE:
	    printf (catgets (dtsearch_catd, MS_cravel, 1,
"\nUSAGE: %s -d<dbname> [options] infile\n"
"       Listed default file name extensions can be overridden.\n"
"  -d<dbname>  1 - 8 char database name, incl optional path prefix.\n"
"              File name extensions automatically appended.\n"
"  -t<etxstr>  End of text doc delimiter string.  Default '\\f\\n'.\n"
"  -c          Initialize database record count by counting records.\n"
"  -p<N>       Print a progress dot every <N> records (default %d).\n"
"  -h<N>       Change duplicate rec id hash table size from %ld to <N>.\n"
"              -h0 means there are no duplicates, don't check for them.\n"
"  -e<path>    Path-filename of huffman encode table (default %s).\n"
"  <infile>    Input [path]file name.  Default extension %s.\n"
	    ),
	    aa_argv0,
	    RECS_PER_DOT, default_hashsize,
	    FNAME_HUFFCODE_TAB, EXT_FZKEY);
	DtSearchExit (2);
    }

    /* Each pass grabs new parm of "-xxx" format */
    for (argc--, argv++; argc > 0 && ((*argv)[0] == '-' || (*argv)[0] == '+');
	argc--, argv++) {
	argptr = argv[0];

	if (strncmp (argptr, "-russell", 8) == 0) {
	    debug_mode = TRUE;
	    if (argptr[8] == '2')
		debug_encode = TRUE;
	    continue;
	}

	argptr[1] = tolower (argptr[1]);
	switch (argptr[1]) {
	    case 'd':	/* (D)ictionary */
		/* May include both dicname and dicpath */
		if (!segregate_dicname (argptr + 2)) {
		    printf (catgets (dtsearch_catd, MS_cravel, 246,
			"\n%s '%s' is invalid path/dictionary name.\n"),
			PROGNAME, argptr);
		    goto PRINT_USAGE;
		}
		break;

	    case 't':	/* ETX delimiter string */
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

	    case 'p':
		if ((recs_per_dot = atoi (argptr + 2)) <= 0) {
		    recs_per_dot = RECS_PER_DOT;
		    printf (catgets (dtsearch_catd, MS_cravel, 582,
			    "%sIgnored invalid progress dot argument '%s'.\n"),
			PROGNAME "582 ", argptr);
		}
		break;

	    case 'e':
		append_ext (fname_huffcode_tab, sizeof (fname_huffcode_tab),
		    argptr + 2, EXT_HUFFCODE);
		break;

	    case 'h':
		duprec_hashsize = atol (argptr + 2);
		if (duprec_hashsize == 0UL)
		    printf (catgets (dtsearch_catd, MS_cravel, 13,
			"%s Duplicate record id checking disabled.\n"),
			PROGNAME);
		break;

	    case 'c':	/* force correct initial reccount by counting
			 * records */
		system_reccount = -1;
		break;

	    default:
UNKNOWN_ARG:
		printf (catgets (dtsearch_catd, MS_cravel, 14,
		    "\n%s Unknown command line argument '%s'.\n"),
		    PROGNAME, argptr);
	}	/* endswitch */
    }	/* endwhile for cmd line '-'processing */

    /* validate input file name */
    if (argc <= 0) {
	puts (catgets (dtsearch_catd, MS_cravel, 15,
		"\nMissing required input file name.\a"));
	goto PRINT_USAGE;
    }
    else
	append_ext (fname_input, sizeof (fname_input), argv[0], EXT_FZKEY);

    /* check for missing database name */
    if (dicname[0] == 0) {
	puts (catgets (dtsearch_catd, MS_cravel, 16,
		"\nNo database name specified (-d argument).\a"));
	goto PRINT_USAGE;
    }
    return;
}  /* user_args_processor() */


/****************************************/
/*                                      */
/*          count_all_records           */
/*                                      */
/****************************************/
/* Initializes system_reccount and maxdba by
 * actually counting all records in database.
 * Must be called after dbrec has been read to ensure
 * maxdba accounts for last miscrec slot number.
 */
static void     count_all_records (void)
{
    char            keybuf[DtSrMAX_DB_KEYSIZE + 4];

    printf (catgets (dtsearch_catd, MS_cravel, 17,
	    "%s Initializing total record count "
	    "in database by actually counting...\n"),
	    PROGNAME);
    system_reccount = 0;
    maxdba = 0;
    KEYFRST (PROGNAME "286", OR_OBJKEY, 0);
    while (db_status == S_OKAY) {
	KEYREAD (PROGNAME "288", keybuf);
	if (db_status != S_OKAY)
	    vista_abort (PROGNAME "288");
	/* don't count records beginning with ctrl char */
	if (keybuf[0] >= 32) {
	    system_reccount++;
	    CRGET (PROGNAME "251", &objdba, 0);
	    UPDATE_MAXDBA (objdba);
	}
	KEYNEXT (PROGNAME "291", OR_OBJKEY, 0);
    }
    /* account for last record's misc record slots */
    maxdba += dbrec.or_recslots;
    return;
}  /* count_all_records() */


/****************************************/
/*                                      */
/*             read_dbrec               */
/*                                      */
/****************************************/
/* Read the database's dbrec and load global variables
 * system_reccount and maxdba with current values from db.
 */
static void     read_dbrec (void)
{
    RECFRST (PROGNAME "285", OR_DBREC, 0);	/* seqtl retrieval */
    if (db_status != S_OKAY) {
	printf (catgets (dtsearch_catd, MS_misc, 13,
	    "%sNo DB record in database '%s'.\n"
	    "  The usual cause is failure to initialize "
	    "the database (run initausd).\n"),
	    PROGNAME"296 ", dicname);
	DtSearchExit (8);
    }
    RECREAD (PROGNAME "302", &dbrec, 0);
    if (db_status != S_OKAY)
	vista_abort (PROGNAME "303");
    swab_dbrec (&dbrec, NTOH);

    if (debug_mode) {
	printf (PROGNAME
	    " DBREC: reccount=%ld maxdba=%ld vers='%s' dbacc=%d\n"
	    "  fzkeysz=%d abstrsz=%d maxwordsz=%d otype=%d lang=%d\n"
	    "  hufid=%ld flags=x%x compflags=x%x uflags=x%lx sec=x%lx\n"
	    ,(long)dbrec.or_reccount
	    ,(long)dbrec.or_maxdba
	    ,dbrec.or_version
	    ,(int)dbrec.or_dbaccess
	    ,(int)dbrec.or_fzkeysz
	    ,(int)dbrec.or_abstrsz
	    ,(int)dbrec.or_maxwordsz
	    ,(int)dbrec.or_dbotype
	    ,(int)dbrec.or_language
	    ,(long)dbrec.or_hufid
	    ,(int)dbrec.or_dbflags
	    ,(int)dbrec.or_compflags
	    ,(long)dbrec.or_dbuflags
	    ,(long)dbrec.or_dbsecmask
	    );
    }

    dbrec_hufid = dbrec.or_hufid;

    /* Confirm compatible program-database version numbers */
    if (!is_compatible_version (dbrec.or_version, SCHEMA_VERSION)) {
	printf (catgets(dtsearch_catd, MS_misc, 14,
	    "%s Program schema version '%s' incompatible with "
	    "database '%s' version '%s'.\n") ,
	    PROGNAME"245", SCHEMA_VERSION, dicname, dbrec.or_version);
	DtSearchExit(4);
    }

    /* If blobs are specified for the database,
     * they must be compressed blobs.
     */
    switch (dbrec.or_dbaccess) {
	case ORA_VARIES:	/* use of blobs determined obj by obj */
	case ORA_BLOB:		/* objects stored directly in blobs */
	case ORA_REFBLOB:	/* refs to objects stored in blobs */
	    blobs_are_used = TRUE;
	    if (!(dbrec.or_compflags & ORC_COMPBLOB)) {
		/* = don't compress blobs */
		printf (catgets (dtsearch_catd, MS_cravel, 717,
		    "%s Aborting: Uncompressed blobs not yet supported.\n"),
		    PROGNAME"717");
		DtSearchExit (5);
	    }
	    break;
	default:
	    blobs_are_used = FALSE;
	    break;
    }

    /* Initialize global variable maxdba, which records largest slot number.
     * If requested, init tot reccount by actually counting records.
     */
    if (system_reccount == -1)
	count_all_records ();
    else {
	system_reccount = dbrec.or_reccount;
	maxdba = dbrec.or_maxdba;
    }

    printf (catgets (dtsearch_catd, MS_cravel, 18,
	"%s: '%s' schema ver = %s, rec count = %ld, last slot = %ld.\n"),
	aa_argv0, dicname, dbrec.or_version,
	(long)system_reccount, (long)maxdba);
    return;
}  /* read_dbrec() */


/****************************************/
/*                                      */
/*             write_dbrec              */
/*                                      */
/****************************************/
/* Write the database's updated reccount and maxdba fields */
static void     write_dbrec (void)
{
    int             i;
    DtSrINT32	int32;

    RECFRST (PROGNAME "355", OR_DBREC, 0);	/* seqtl retrieval */
    if (db_status != S_OKAY)
	vista_abort (PROGNAME "356");
    int32 = htonl (system_reccount);
    CRWRITE (PROGNAME "341", OR_RECCOUNT, &int32, 0);
    int32 = htonl (maxdba);
    CRWRITE (PROGNAME "342", OR_MAXDBA, &int32, 0);

    /* If this was the first load of a new database,
     * save the huffman encode table id.
     */
    if (blobs_are_used && dbrec_hufid == -1) {
	int32 = htonl ((DtSrINT32)gen_vec_hufid);
	CRWRITE (PROGNAME "343", OR_HUFID, &int32, 0);
    }
    if (db_status != S_OKAY)
	vista_abort (PROGNAME "344");
    printf (catgets (dtsearch_catd, MS_cravel, 19,
	    "%s: Final database record count = %ld, last slot = %ld.\n"),
	aa_argv0, (long)system_reccount, (long)maxdba);
    return;
}  /* write_dbrec() */


/************************************************/
/*                                              */
/*               print_progress                 */
/*                                              */
/************************************************/
/* prints complete progress message and statistics to stdout */
static void     print_progress (void)
{
    time_t	seconds = time (NULL) - starttime;
    long	bytes_in = ftell (infile);

    if (bytes_in <= 0L)
	bytes_in = fstat_input.st_size;  /* make final msg "100%" */
    TERMINATE_LINE
    printf (catgets (dtsearch_catd, MS_cravel, 20,
	"%s: %ld input records processed in %ldm %lds, (%ld%%).\n"
	"  %ld duplicates, %ld new, %ld updates.\n"),
	aa_argv0,
	input_reccount, seconds / 60L, seconds % 60L,
	(bytes_in * 100L) / fstat_input.st_size,
	duplicate_recids, created_reccount, updated_reccount);
    need_final_progress_msg = FALSE;
    return;
}  /* print_progress() */


/************************************************/
/*                                              */
/*               print_exit_code                */
/*                                              */
/************************************************/
/* Called from inside DtSearchExit() at austext_exit_last */
static void     print_exit_code (int exit_code)
{
    if (dotcount) {
	putchar ('\n');
	dotcount = 0;
    }
    printf ( catgets(dtsearch_catd, MS_cravel, 2,
	"%s: Exit code = %d\n") ,
	aa_argv0, exit_code);
    fflush (aa_stderr);
    fflush (stdout);
    return;
}  /* print_exit_code() */


/************************************************/
/*                                              */
/*              load_next_miscrec		*/
/*                                              */
/************************************************/
/* Repeatedly called from create_object() or update_object()
 * to fill miscrec buffer with next FZKABSTR type miscrec
 * from input file data saved in fzkbuf and abstrbuf.
 * First call for a given object is signaled by passed arg.
 * Thereafter static pointers keep track of where we are
 * in the source bufs to correctly load the next miscrec.
 * Initial state = fill-with-fzkey, if there is a fzkey.
 * Second state = fill-with-abstract, if there is an abstract.
 * Last state = zero-fill balance of remaining misc records.
 * Returns TRUE until last state completed (no more miscrecs can be written).
 */
static int      load_next_miscrec (int first_call)
{
    static enum {
	FILL_FZKEY, FILL_ABSTR, FILL_ZEROS
    }
                    fill_state = FILL_ZEROS;
    static char    *src = NULL;
    static int      srclen = 0;
    static int      totbytes = 0;

    int             i;
    char           *targ;

    /* Initialize static variables at first call. */
    if (first_call) {
	/* If fzkey-abstract misc recs not used, return immediately. */
	if ((totbytes = dbrec.or_fzkeysz + dbrec.or_abstrsz) <= 0)
	    return FALSE;
	if (dbrec.or_fzkeysz > 0) {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_cravel, 522,
		"%s This version of %s does not support semantic databases.\n"),
		PROGNAME"522", aa_argv0);
	    DtSearchExit (13);
	}
	else {
	    fill_state = FILL_ABSTR;
	    src = abstrbuf;
	    srclen = dbrec.or_abstrsz;
	}
    }

    /* If NOT first call, but we've finished writing everything out,
     * then tell the caller there's nothing left to do.
     */
    else if (totbytes <= 0)
	return FALSE;

    /* Main loop is on each byte of the or_misc field of miscrec.
     * Depending on the fill state, the byte will be a fzkey byte,
     * an abstract byte, or a binary zero byte.
     */
    targ = (char *) miscrec.or_misc;
    for (i = 0; i < sizeof(miscrec.or_misc); i++, totbytes--) {
	switch (fill_state) {
	    case FILL_FZKEY:
		*targ++ = *src++;
		if (--srclen <= 0) {	/* end of fzkey? */
		    if (dbrec.or_abstrsz > 0) {
			fill_state = FILL_ABSTR;
			src = abstrbuf;
			srclen = dbrec.or_abstrsz;
		    }
		    else
			fill_state = FILL_ZEROS;
		}
		break;

	    case FILL_ABSTR:
		if (*src == 0 || --srclen <= 0)	/* end of abstract? */
		    fill_state = FILL_ZEROS;
		*targ++ = *src++;
		break;

	    case FILL_ZEROS:
		*targ++ = 0;
		break;

	    default:
		fprintf (aa_stderr, catgets (dtsearch_catd, MS_misc, 25,
			"%sAbort due to program error.\n"),
		    PROGNAME "549 ");
		DtSearchExit (54);
	}	/* end switch */
    }	/* end for-loop */

    miscrec.or_misctype = ORM_FZKABS;
    return TRUE;
}  /* load_next_miscrec() */


/************************************************/
/*                                              */
/*               create_object			*/
/*                                              */
/************************************************/
/* Creates new object rec and misc recs from current vista rec.
 * Sets global objdba to new rec's dba and updates maxdba if necessary.
 * 1  create fields in objrec buffer, and write it.
 *    (or_objsize will be rewritten after text size has been determined.)
 * 2  create fzkey-abstract rec as necessary.
 */
static void     create_object (char *key)
{
    int             i;
    char           *src, *targ;
    DB_ADDR         tempdba;

    memset (&objrec, 0, sizeof (objrec));

    /* Copy the key into the buffer.  The previous initialization
     * ensures that the key will be padded on the right with zero fill.
     * At this point, key length should never be too long because
     * it has been previously tested (when the line was first read in).
     */
    src = key;
    targ = objrec.or_objkey;
    for (i = 0; i < DtSrMAX_DB_KEYSIZE; i++) {
	if (*src == 0)
	    break;
	*targ++ = *src++;
    }

    /* Objdate will be updated later if line #4 has
     * valid DtSrObjdate format.  Otherwise current
     * date/time stamp will be the default.
     */
    objrec.or_objdate = starttimeobjd;

    /* If all objects in database are same type, mark approp obj flag */
    if (dbrec.or_dbotype != 0)
	objrec.or_objtype = dbrec.or_dbotype;

    /* If blobs are never used, mark each obj as 'unretrievable' */
    if (!blobs_are_used)
	objrec.or_objflags |= DtSrFlNOTAVAIL;
    swab_objrec (&objrec, HTON);
    FILLNEW (PROGNAME "487", OR_OBJREC, &objrec, 0);
    if (db_status != S_OKAY)
	vista_abort (PROGNAME "495");
    CRGET (PROGNAME "375", &objdba, 0);	/* save object's dba */
    UPDATE_MAXDBA (objdba);
    if (debug_mode)
	printf ("---> new rec: inrecno %6ld, slot %6ld, key '%s'\n",
	    (long int) input_reccount, (long int) objdba & 0xffffff, objrec.or_objkey);

    /* Make current object record the owner of all its sets */
    SETOR (PROGNAME "376", OR_OBJ_BLOBS, 0);
    SETOR (PROGNAME "377", OR_OBJ_MISCS, 0);

    /* If fzkeys and/or abstracts are used,
     * write out the misc record(s) now.
     */
    if (load_next_miscrec (TRUE))
	do {
	    HTONS (miscrec.or_misctype);
	    FILLNEW (PROGNAME "501", OR_MISCREC, &miscrec, 0);
	    CRGET (PROGNAME "503", &tempdba, 0);
	    UPDATE_MAXDBA (tempdba);
	    CONNECT (PROGNAME "505", OR_OBJ_MISCS, 0);
	} while (load_next_miscrec (FALSE));

    system_reccount++;	/* new obj rec, so incr tot num database recs */
    created_reccount++;
    return;
}  /* create_object() */


/************************************************/
/*                                              */
/*               update_object			*/
/*                                              */
/************************************************/
/* Reinitializes portions of preexisting object rec.
 * (Presumes vista 'current record' is desired object rec.)
 * Sets objdba to rec's dba and updates maxdba if necessary.
 * System_reccount is not altered because this is not a new record.
 * 1  reinit certain fields in objrec, and rewrite it.
 *    (or_objsize will be rewritten after text size has been determined.)
 * 2  delete all blobs (there should be no hyper recs,
 *    and existing user notes should not be changed).
 * 3  update fzkey-abstract rec(s) as necessary.
 * Important: misc rec updates should always be IN-PLACE.
 * If miscrecs are deleted first then readded,
 * there is no guarantee that their slots will be adjacent.
 * This will screw up bit vector calculations in the inverted
 * index word searches.  In-place updates are faster anyway,
 * and we know that the number of misc rec slots is constant.
 */
static void     update_object (char *key)
{
    int		i;
    int		first_fzkabstr = TRUE;
    DtSrINT16	misctype;
    DtSrINT32	int32;
    DB_ADDR	tempdba;
    DtSrINT32	zero_objsize = 0;

    /* Slot number is dba with high order byte (filenum) parsed out */
    CRGET (PROGNAME "467", &objdba, 0);	/* save object's dba */
    UPDATE_MAXDBA (objdba);
    if (debug_mode)
	printf ("----> update: inrecno %6ld, slot %6ld, key '%s'\n",
	    (long int) input_reccount, (long int) objdba & 0xffffff, key);

    /* Reinit certain fields.
     * Objsize will be rewritten after new text size determined.
     * Objdate will be rewritten if .fzk file has valid
     * DtSrObjdate format in line #4.
     */
    CRWRITE (PROGNAME "472", OR_OBJSIZE, &zero_objsize, 0);
    int32 = htonl (starttimeobjd);
    CRWRITE (PROGNAME "681", OR_OBJDATE, &int32, 0);

    /* Make current object record the owner of all its sets */
    SETOR (PROGNAME "475", OR_OBJ_BLOBS, 0);
    SETOR (PROGNAME "476", OR_OBJ_MISCS, 0);

    /* Delete all blobs in a loop */
    FINDFM (PROGNAME "480", OR_OBJ_BLOBS, 0);
    while (db_status == S_OKAY) {
	DISDEL (PROGNAME "482", 0);
	FINDFM (PROGNAME "483", OR_OBJ_BLOBS, 0);
    }

    /* Update all miscrecs in a loop.
     * User notes are left alone,
     * and fzkey-abstracts are updated.
     * Currently other types are not allowed.
     */
    first_fzkabstr = TRUE;
    FINDFM (PROGNAME "480", OR_OBJ_MISCS, 0);
    while (db_status == S_OKAY) {
	CRREAD (PROGNAME "496", OR_MISCTYPE, &misctype, 0);
	NTOHS (misctype);
	switch (misctype) {
	    case ORM_OLDNOTES:
	    case ORM_HUGEKEY:
		break;	/* do nothing */

	    case ORM_FZKABS:	/* combined fzkey-abstract rec */
		if (load_next_miscrec (first_fzkabstr)) {
		    HTONS (miscrec.or_misctype);
		    RECWRITE (PROGNAME "601", &miscrec, 0);
		    CRGET (PROGNAME "605", &tempdba, 0);
		    UPDATE_MAXDBA (tempdba);
		    first_fzkabstr = FALSE;
		}
		else {
		    DISDEL (PROGNAME "709", 0);
		}
		break;

	    default:
		DISDEL (PROGNAME "529", 0);
	}	/* end switch */

	FINDNM (PROGNAME "506", OR_OBJ_MISCS, 0);
    }	/* end update loop for all members of OBJ_MISCS set */

    updated_reccount++;
    return;
}  /* update_object() */


/************************************************/
/*						*/
/*		  call_encoder			*/
/*						*/
/************************************************/
/* Called from main while reading document text.
 * Calls huffman compression encoder at convenient
 * intervals and at ETX.
 */
static void	call_encoder (UCHAR *ucharbuf, int buflen)
{
    objsize += buflen;
    if (debug_encode) {
	sumlines += buflen;
	printf ("buflen = %d, sumlines = %d, cum objsize = %ld\n",
	    (int)buflen, (int)sumlines, (long)objsize);
    }
    if (hc_encode (&blobrec, ucharbuf, buflen, FALSE)) {
	if (debug_encode) {
	    sumblobs += blobrec.or_bloblen;
	    printf ("---> WRITE sumlines = %d, bloblen = %d, "
		"sumblobs = %d, objsize = %ld\n",
		sumlines, (int)blobrec.or_bloblen,
		(int)sumblobs, (long)objsize);
	    sumlines = 0;
	}
	HTONS (blobrec.or_bloblen);
	FILLNEW (PROGNAME "572", OR_BLOBREC, &blobrec, 0);
	CONNECT (PROGNAME "578", OR_OBJ_BLOBS, 0);
    }
    return;
} /* call_encoder() */


/************************************************/
/*						*/
/*		      main			*/
/*						*/
/************************************************/
int             main (int argc, char *argv[])
{
    static int	hufftab_never_loaded = TRUE;
    DBLK	dblk;
    int		i, linelen;
    DtSrINT32	int32;
    char	*cptr, *targ, *src;
    char	*db_key;
    char	uniqkey [DtSrMAX_DB_KEYSIZE + 4];
    char	linebuf [2048];
    struct tm	*tmptr;

    /* Init globals */
    setlocale (LC_ALL, "");
    dtsearch_catd = catopen (FNAME_DTSRCAT, 0);

    aa_argv0 = argv[0];
    time (&starttime);
    tmptr = localtime (&starttime);
    starttimeobjd = tm2objdate (tmptr);
    strftime (linebuf, sizeof (linebuf),
	catgets (dtsearch_catd, MS_misc, 22, "%A, %b %d %Y, %I:%M %p"),
	tmptr);
    printf (catgets (dtsearch_catd, MS_misc, 23,
	"%s: Version %s.  Run %s.\n"),
	aa_argv0,
	DtSrVERSION,
	linebuf);
    austext_exit_last = print_exit_code;
    init_user_interrupt ();	/* specify signal handlers */
    default_hashsize = duprec_hashsize;	/* deflt val in isduprec.c */
    strcpy (fname_huffcode_tab, FNAME_HUFFCODE_TAB);
    dicname[0] = 0;
    dicpath[0] = 0;

    memset (&dblk, 0, sizeof(DBLK));
    memset (&parg, 0, sizeof(PARG));
    parg.dblk = &dblk;
    parg.etxdelim = ETXDELIM;

    /* Parse user's command line args and maybe change global variables */
    user_args_processor (argc, argv);
    strcpy (dblk.name, dicname);

    /* Open the database */
    if (debug_mode)
	printf (PROGNAME "211 database OPEN string = '%s%s'\n",
	    dicpath, dicname);
    if (!austext_dopen (dicname, dicpath, NULL, 0, NULL)) {
	fprintf (aa_stderr, "%s\n", DtSearchGetMessages());
	DtSearchExit (3);
    }

    src = getcwd (linebuf, sizeof (linebuf));
    if (!src)
	src = getenv ("PWD");
    printf (catgets (dtsearch_catd, MS_misc, 24,
	"%s: cwd = '%s', fzkfile = '%s'\n"),
	aa_argv0,
	(src) ? src : catgets (dtsearch_catd, MS_misc, 6, "<unknown>"),
	fname_input);
    if ((infile = fopen (fname_input, "r")) == NULL) {
	fprintf (aa_stderr, catgets (dtsearch_catd, MS_misc, 12,
	    "%sUnable to open %s:\n  %s\n"),
	    PROGNAME "326 ", fname_input, strerror (errno));
	DtSearchExit (6);
    }
    parg.ftext = infile;	/* for discard_to_ETX() */

    /* Read in starting database record count and other db config/status data */
    read_dbrec ();

    /* If fzkeys and/or abstracts are used,
     * create correctly sized buffers for them.
     */
    if (dbrec.or_fzkeysz > 0) {
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_cravel, 522,
	    "%s This version of %s does not support semantic databases.\n"),
	    PROGNAME"523", aa_argv0);
	DtSearchExit (13);
    }

    if (dbrec.or_abstrsz > 0)
	abstrbuf = austext_malloc (dbrec.or_abstrsz + 16, PROGNAME "744", NULL);

    /* Get input file size for progress msgs */
    if (fstat (fileno (infile), &fstat_input) == -1) {
	fprintf (aa_stderr, catgets (dtsearch_catd, MS_cravel, 29,
	    "%s Unable to get status for %s: %s\n"),
	    PROGNAME"337", fname_input, strerror (errno));
	DtSearchExit (10);
    }
    if (fstat_input.st_size <= 0L) {
	fprintf (aa_stderr, catgets (dtsearch_catd, MS_cravel, 30,
	    "%s File %s is empty.\n"),
	    PROGNAME"343", fname_input);
	DtSearchExit (7);
    }

    printf (catgets (dtsearch_catd, MS_cravel, 31,
	"%s: Each dot = %d records processed.\n"),
	aa_argv0, recs_per_dot);

    /*-------------------- MAIN LOOP --------------------
     * Executed once for each new input record.
     * 1. Read and process the FZKEY line.
     * 2. Read and process the ABSTRACT line.
     * 3. Read the UNIQUE KEY line.
     *    Write out an object record at this point.
     * 4. Read and process the DATE line, update object rec.
     * 5. Use readchar_ftext to read document text until ETX.
     *    Either blob it or discard it as appropriate.
     */
    while (!feof(infile)) {

        /*----- READ LINE #1, fzkey -------------------------
	 * First line of new record.
	 * Abort now if a shutdown signal was sent.
	 * Skip null records (ETX str followed immediately by ETX str).
	 * If this database uses fzkeys, "pack" current fzkey
	 * and save it in the correct miscrec buffer.
	 * If fzkeys are combined with abstracts they share the same
	 * miscrec, otherwise they they reside in their own miscrec.
	 * WARNING! Presumes or_fzkeysz <= the space allocated
	 * for it in the correct miscrec.
	 *-----------------------------------------------------*/
	if (fgets (linebuf, sizeof(linebuf) - 1, infile) == NULL)
	    break;

	/* Got at least one line of a new record.  Print progress dots */
	if (!(input_reccount % recs_per_dot)) {
	    if (input_reccount) {
		putchar ('.');
		dotcount++;
		if (!(dotcount % 10))
		    putchar (' ');
		if (dotcount >= 50) {
		    print_progress ();
		    dotcount = 0;
		}
		else
		    fflush (stdout);
	    }
	}
	input_reccount++;
	need_final_progress_msg = TRUE;

	linebuf [sizeof(linebuf)-1] = 0;
	linelen = strlen (linebuf);
	objsize = 0;

	if (shutdown_now) {
	    TERMINATE_LINE
	    printf (catgets (dtsearch_catd, MS_misc, 15,
		"%sReceived abort signal %d.\n"),
		PROGNAME"373 ", shutdown_now);
	    write_dbrec ();	/* at least update reccount and maxdba */
	    DtSearchExit (100 + shutdown_now);
	}
	/* Skip null record */
	if (strcmp (linebuf, parg.etxdelim) == 0)
	    continue;

	/*----- READ LINE #2, abstract ------------------------
	 * Second line is abstract line.  Save it in record buffer,
	 * hopping over the first 10 chars ("ABSTRACT: ....").
	 *-----------------------------------------------------*/
	if (fgets (linebuf, sizeof (linebuf) - 1, infile) == NULL)
	    break;
	linebuf [sizeof(linebuf)-1] = 0;
	linelen = strlen (linebuf);

	if (strncmp (linebuf, "ABSTRACT: ", 10) != 0) {
	    cptr = PROGNAME"580";
    INVALID_FORMAT:
	    normal_exitcode = EXIT_WARNING;
	    TERMINATE_LINE
	    printf (catgets (dtsearch_catd, MS_cravel, 579,
		"%s Discarded rec #%ld: Invalid .fzk file format.\n"),
		cptr, input_reccount);
	    if (strcmp (linebuf, parg.etxdelim) != 0)
		discard_to_ETX (&parg);
	    continue;
	}

	/* If abstracts are used, save this one in the abstract buffer */
	if (dbrec.or_abstrsz > 0) {
	    linebuf[--linelen] = 0;	/* delete terminating \n */
	    strncpy (abstrbuf, linebuf + 10, dbrec.or_abstrsz);
	    abstrbuf[dbrec.or_abstrsz - 1] = 0;
	}

	/*--- READ LINE #3, unique database key ------------------
	 * Third line is 'unique record id'.
	 * If key is valid update old objrec
	 * or create new one as necessary.
	 * (There may be one more write required
	 * after we determine total blob size).
	 *-----------------------------------------------------*/
	if (fgets (linebuf, sizeof (linebuf) - 1, infile) == NULL)
	    break;
	linebuf [sizeof(linebuf)-1] = 0;
	linelen = strlen (linebuf);
	if (strcmp (linebuf, parg.etxdelim) == 0) {
	    cptr = PROGNAME"1068";
	    goto INVALID_FORMAT;
	}
	/*
	 * Isolate first token surrounded by whitespace
	 * (and parse out \n) 
	 */
	if ((db_key = strtok (linebuf, " \t\n")) == NULL) {
	    cptr = PROGNAME"1076";
	    goto INVALID_FORMAT;
	}
	if (strlen (db_key) > DtSrMAX_DB_KEYSIZE - 1) {
	    normal_exitcode = EXIT_WARNING;
	    TERMINATE_LINE
	    printf (catgets (dtsearch_catd, MS_cravel, 33,
		"%s Discarded rec #%ld:  Key too long:\n  '%s'.\n"),
		PROGNAME"606", input_reccount, db_key);
	    discard_to_ETX (&parg);
	    continue;
	}
	if (!isalnum (db_key[0])) {
	    normal_exitcode = EXIT_WARNING;
	    TERMINATE_LINE
	    printf (catgets (dtsearch_catd, MS_cravel, 927,
		"%s Discarded rec #%ld:  First char (keytype) of key\n"
		"  '%s' is not alphanumeric.\n"),
		PROGNAME"927", input_reccount, db_key);
	    discard_to_ETX (&parg);
	    continue;
	}

	/* If duplicate record in fzk file, discard it. */
	i = is_duprec (db_key);
	if (i == 2) {
	    TERMINATE_LINE
	    printf (catgets (dtsearch_catd, MS_cravel, 34,
		"%s Out of Memory!  "
		"Set -h arg to a smaller number,\n"
		"  or reduce the number of input records.\n"),
		PROGNAME"1096");
	    DtSearchExit (55);
	}
	else if (i == 1) {	/* skip duplicate record id */
	    normal_exitcode = EXIT_WARNING;
	    TERMINATE_LINE
	    printf (catgets (dtsearch_catd, MS_cravel, 35,
		"%s: Discarded duplicate rec #%ld '%s'.\n"),
		aa_argv0, input_reccount, db_key);
	    duplicate_recids++;
	    discard_to_ETX (&parg);
	    continue;
	}
	/*
	 * Try to read the object record from the database. If it
	 * already exists (UPDATE): delete all its blobs (there
	 * should be no hyper recs). create or update
	 * fzkey-abstract recs as necessary. dont change any
	 * existing user notes. update fields in objrec buffer,
	 * but don't write it yet-- objrec will be rewritten
	 * after text size has been determined. If it doesn't
	 * exist (CREATE): create fields in objrec buffer, and
	 * write it. create fzkey-abstract recs as necessary.
	 * objrec will be rewritten after text size has been
	 * determined. After update or create, objdba contains
	 * dba of curr obj record. 
	 */
	strcpy (uniqkey, db_key);
	KEYFIND (PROGNAME "489", OR_OBJKEY, uniqkey, 0);
	if (db_status == S_OKAY)
	    update_object (uniqkey);
	else
	    create_object (uniqkey);


	/*----- READ LINE #4, date -----------------------------
	 * Line #4 is object date/time string (OBJDATESTR format).
	 * It is no longer optional.  If invalid, the current
	 * run date that was preloaded into the record is used.
	 *-----------------------------------------------------*/
	if (fgets (linebuf, sizeof (linebuf) - 1, infile) == NULL)
	    break;
	linebuf [sizeof(linebuf)-1] = 0;
	linelen = strlen (linebuf);
	if (!is_objdatestr (linebuf, &objdate)) {
	    normal_exitcode = EXIT_WARNING;
	    if (strcmp (linebuf, parg.etxdelim) == 0) {
		cptr = PROGNAME"1155";
		goto INVALID_FORMAT;
	    }
	    TERMINATE_LINE
	    printf (catgets (dtsearch_catd, MS_cravel, 1086,
		"%s Record '%s' has invalid date format--"
		"using run date.\n"),
		PROGNAME"1086", uniqkey);
	}
	else {		/* objdate is valid */
	    KEYFIND (PROGNAME "1098", OR_OBJKEY, uniqkey, 0);
	    if (db_status != S_OKAY)
		vista_abort (PROGNAME "1101");
	    HTONL (objdate);	/* ready for record writes */
	    CRWRITE (PROGNAME "1102", OR_OBJDATE, &objdate, 0);
	}

	/*----- READ TO ETX, record text ---------------------
	 * Balance of record (after line 4 to end of record marker)
	 * is text.  It may or may not be formatted in neat ascii
	 * lines, ie it may not have periodic linefeeds (\n).
	 * If this database does not store compressed records (blobs)
	 * we just discard all chars to end of text delimiter (ETX).
	 * Otherwise we read it char by char using readchar_ftext()
	 * and fill linebuf to some convenient size.
	 *
	 * Repeated calls to hc_encode() build
	 * a compressed record in its own internal blobrec buffer.
	 * When the buffer is full, hc_encode copies it to
	 * the passed blobrec buffer and returns TRUE.
	 * The caller should then write out the blobrec.
	 * If hc_encode returns FALSE, its internal blobrec is not
	 * yet full so the caller should not yet write out his record.
	 *-----------------------------------------------------*/

	if (!blobs_are_used) {
	    discard_to_ETX (&parg);
	    continue;
	}
	/*
	 * Initialize blob compression by reading in huffman
	 * encode table (first execution only). Ensure table id
	 * is same as one used for previous compressions, if any. 
	 */
	if (hufftab_never_loaded) {
	    hufftab_never_loaded = FALSE;
	    gen_vec (fname_huffcode_tab);
	    if (dbrec_hufid != gen_vec_hufid && dbrec_hufid != -1L) {
		TERMINATE_LINE
		printf (catgets (dtsearch_catd, MS_cravel, 1153,
		    "%s Current data compression table id"
		    " in '%s' is %ld.\n"
		    "  Database '%s' previously compressed"
		    " with table %ld.\n"),
		    PROGNAME"1153 ", fname_huffcode_tab,
		    gen_vec_hufid, dicname, dbrec_hufid);
		DtSearchExit (53);
	    }
	}
	/*
	 * Compress document text.  Repeatedly load linebuf
         * with fixed number of chars and compress it.
	 */
	if (debug_encode) {
	    sumlines = 0;
	    sumblobs = 0;
	}
	if ((linebuf[0] = readchar_ftext (&parg)) == 0) {
	    normal_exitcode = EXIT_WARNING;
	    TERMINATE_LINE
	    printf ( catgets(dtsearch_catd, MS_cravel, 1215,
		"%s Warning.  Record '%s' has no text.\n"),
		PROGNAME"1215" , uniqkey);
	    continue;
	}
	linelen = 1;
	while (linebuf [linelen] = readchar_ftext (NULL)) {
	    if (++linelen >= 80) {
		call_encoder ((UCHAR *)linebuf, linelen);
		linelen = 0;
	    }
	}

	/*
	 * At ETX:  If a partial line remains, process it just like
	 * the full lines above.  Then write out total size to
	 * object record, and make the final call to hc_encode with
	 * empty line and TRUE flag to indicate 'no more text,
	 * flush your last partial buffer'.
	 */
	if (linelen)
	    call_encoder ((UCHAR *)linebuf, linelen);
	CRSET (PROGNAME "685", &objdba, 0);
	int32 = htonl (objsize);
	CRWRITE (PROGNAME "686", OR_OBJSIZE, &int32, 0);
	if (hc_encode (&blobrec, (UCHAR *)"", 0, TRUE)) {
	    if (debug_encode) {
		sumblobs += blobrec.or_bloblen;
		printf ("---> FINAL sumlines =%d, bloblen = %d, "
		    "sumblobs = %ld, objsize = %ld\n",
		    (int)sumlines, (int)blobrec.or_bloblen,
		    (long)sumblobs, (long)objsize);
	    }
	    HTONS (blobrec.or_bloblen);
	    FILLNEW (PROGNAME "624", OR_BLOBREC, &blobrec, 0);
	    CONNECT (PROGNAME "625", OR_OBJ_BLOBS, 0);
	}

    }	/* end main record loop */

    fclose (infile);
    if (need_final_progress_msg)
	print_progress ();
    write_dbrec ();

    /* If all input records were discarded, complete processing
     * but upgrade warning exit code to hard error code.
     */
    if (created_reccount <= 0L && updated_reccount <= 0L) {
	normal_exitcode = EXIT_VANISH;
	fprintf (stderr, catgets (dtsearch_catd, MS_cravel, 1048,
		"%sDatabase objects not changed because input "
		"file effectively empty.\n"),
	    PROGNAME "1048 ");
    }

    /* Close database and print return code via exits.
     * Return code is either 0 (perfect), 1 (warnings),
     * or 3 (input file effectively empty).
     */
    DtSearchExit (normal_exitcode);
}  /* main() */

/*********************** DTSRLOAD.C ***************************/
