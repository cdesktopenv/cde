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
 *   FUNCTIONS: change_max_wordsize
 *		change_min_wordsize
 *		confirm_ok_to_overwrite
 *		create_new_dbd
 *		main
 *		print_usage
 *		remove_d9x_file
 *		user_args_processor
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1993,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/************************* DTSRCREATE.C **************************
 * $XConsortium: dtsrcreate.c /main/9 1996/09/23 21:02:04 cde-ibm $
 * October 1993.
 * Program formerly named initausd, in module initausd.c.
 * Essentially performs the same function as vista's initdb,
 * but uses only the dtsearch.dbd and renames the files during creation.
 * Also creates and initializes the first slot, the 'dbrec'.
 *
 * $Log$
 * Revision 2.8  1996/03/25  18:53:33  miker
 * Changed FILENAME_MAX to _POSIX_PATH_MAX.
 *
 * Revision 2.7  1996/02/01  18:16:16  miker
 * Changed some arg defaults depending on DTSEARCH definition.
 * Deleted BETA definition.
 *
 * Revision 2.6  1995/12/27  16:10:03  miker
 * Permit -wx before -wn on command line.
 *
 * Revision 2.5  1995/12/07  23:27:51  miker
 * Fixed bug: minwordsz was > max when max inited to -1.
 * Changed 'Engine Type' to 'Flavor' for AusBuild.
 *
 * Revision 2.4  1995/10/25  21:09:42  miker
 * Added prolog.
 *
 * Revision 2.3  1995/10/20  21:28:25  miker
 * Intelligently look for dtsearch.dbd in 3 places.
 *
 * Revision 2.2  1995/10/19  21:16:17  miker
 * Internally rename database files so it doesn't have to be
 * done at open time.  Ask permission to overwrite preexisting
 * files.  Always create databases from current model dtsearch.dbd
 * (elminate SECOND CASE).  Coincides with libDtvis enhancements.
 *
 * Revision 2.1  1995/09/22  19:32:18  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.2  1995/09/19  21:56:53  miker
 * Enabled Japanese language DtSrJPN.
 * If DtSearch, use DtSrVERSION instead of AUSAPI_VERSION in banner.
 *
 * Revision 1.1  1995/08/31  20:50:28  miker
 * Initial revision
 */
#include "SearchP.h"
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>
#include "vista.h"

#define PROGNAME		"DTSRCREATE"
#define	DEFAULT_MINWORD		(MINWIDTH_TOKEN + 1)
#define	STANDARD_MAXWORD	(DtSrMAXWIDTH_HWORD - 1)
#define MS_misc			1
#define MS_initausd		12
#define FNAME_MODEL		"dtsearch.dbd"

/* The following MUST MATCH vista's dbtype.h! */
#define SIZEOF_FILE_ENTRY	252	/* sizeof(FILE_ENTRY) */
#define DBD_COMPAT_LEN		6
#define START_OF_FT		(DBD_COMPAT_LEN + (8 * sizeof(INT)))

/* Values for 'flavor' global variable */
#define AUSTEXT_FLAVOR	'a'
#define DTSEARCH_FLAVOR	'd'

/*------------------ GLOBALS -------------------*/
static int	abstrsz =	-1;
static char	dbname [12] =	"<dbname>";
struct or_dbrec dbrec;
static int	debug_mode =	FALSE;
static char     default_cant_open_msg[] =
				"%s: %s: %s.\n";
static int	fzkeysz =	0;
static int	flavor =	DTSEARCH_FLAVOR;
static int	language =	DtSrLaENG;
static int	minwordsz =	DEFAULT_MINWORD;
static int	maxwordsz =	INT_MAX;
static int	max_ormisc_size;
static int	maxwidth_lword;
static int	maxwidth_sword;
static char	modelpath [_POSIX_PATH_MAX];
				/* path/name of model dbd file */
static char	newpath [_POSIX_PATH_MAX];
				/* path/name for each renamed file */
static char	*newextp;	/* loc where extension suffixes placed */
static int	ok_to_overwrite = FALSE;
static long	path_offset =	0;
static int	quiet_mode =	FALSE;

static char	*exttab[] = {
	/* Must be in same order as model .dbd file tables */
	".d00", ".d01", ".d21", ".d22", ".d23",
	".k00", ".k01", ".k21", ".k22", ".k23",
	NULL };

/* Same as MS_initausd, 213... */
static char	default_unable_to_open_msg[] =
	"%1$s Unable to open '%2$s':\n  %3$s.\a\n";


/************************************************/
/*						*/
/*	    confirm_ok_to_overwrite		*/
/*						*/
/************************************************/
/* Called whenever we are about to write a new file.
 * Checks to see if file preexists.  If it does,
 * and user has never said it's ok to overwrite,
 * prompts for permission to overlay all preexisting files.
 * If 'yes', never asks again.  If 'no', exits.
 * Returns if ok to overwrite, else exits.
 */
static void	confirm_ok_to_overwrite (char *fname)
{
    FILE	*fptr;
    int		i;

    if (ok_to_overwrite)
	return;
    if ((fptr = fopen (newpath, "r")) == NULL)
	return;
    fclose (fptr);

    printf ( catgets(dtsearch_catd, MS_initausd, 12,
	"\nFile '%s' already exists.\n"
	"Is it ok to overwrite it and other database files? [y,n] ") ,
	newpath);
    i = tolower (getchar());
    if (i == 'y')
	ok_to_overwrite = TRUE;
    else
	DtSearchExit (2);
    return;
} /* confirm_ok_to_overwrite() */


/************************************************/
/*						*/
/*		change_max_wordsize		*/
/*						*/
/************************************************/
/* Subroutine of user_args_processor().
 * Adjusts maxwordsz per user request and allowed sizes of schema.
 */
static int      change_max_wordsize (char *new_size)
{
    int		users_newsize;

    maxwordsz = users_newsize = atoi (new_size);

    /* error if min and max specifications incompatible */
    if (minwordsz > maxwordsz) {
	printf (catgets (dtsearch_catd, MS_initausd, 5,
	    PROGNAME" Minimum word size %d greater "
	    "than maximum word size %d.\n"),
	    minwordsz, maxwordsz);
	return FALSE;
    }

    /* If necessary, adjust to nearest logical maxwordsz */
    if (maxwordsz != maxwidth_sword &&
	maxwordsz != maxwidth_lword &&
	maxwordsz != DtSrMAXWIDTH_HWORD - 1) {
	if (maxwordsz < maxwidth_sword)
	    maxwordsz = maxwidth_sword;
	else if (maxwordsz < maxwidth_lword)
	    maxwordsz = maxwidth_lword;
	else
	    maxwordsz = DtSrMAXWIDTH_HWORD - 1;
    }

    if (maxwordsz != users_newsize)
	printf (catgets (dtsearch_catd, MS_initausd, 8,
	    PROGNAME " Adjusted maximum word size to %d.\n"),
	    maxwordsz);

    /* Give user a final warning about large word sizes */
    if (maxwordsz > STANDARD_MAXWORD && language != DtSrLaDEU && !quiet_mode)
	printf ("%s", catgets (dtsearch_catd, MS_initausd, 10,
		PROGNAME" Specifying large maximum word sizes may "
		"significantly\n  increase storage requirements.\n"));
    return TRUE;
}  /* change_max_wordsize() */


/************************************************/
/*                                              */
/*               change_min_wordsize            */
/*                                              */
/************************************************/
/* Subroutine of user_args_processor().
 * Adjusts minwordsz per user request.
 */
static int      change_min_wordsize (char *new_size)
{
    int		old_minwordsz = minwordsz;
    if ((minwordsz = atoi (new_size)) < 0)
	return FALSE;

    /* error if min and max specifications incompatible */
    if (minwordsz > maxwordsz) {
	printf (catgets (dtsearch_catd, MS_initausd, 5,
	    PROGNAME " Minimum word size %d greater than "
	    "maximum word size %d.\n"),
	    minwordsz, maxwordsz);
	return FALSE;
    }

    if (!quiet_mode) {
	if (minwordsz != old_minwordsz)
	    printf (catgets (dtsearch_catd, MS_initausd, 6,
		PROGNAME " Adjusted minimum word size to %d.\n"),
		minwordsz);

	/* give user a warning about short word sizes */
	if (minwordsz < DEFAULT_MINWORD)
	    printf ("%s", catgets (dtsearch_catd, MS_initausd, 9,
		    PROGNAME " Specifying small minimum word sizes"
		    " may require extensive\n"
		    "  editing of stopword file to prevent significantly\n"
		    "  increased index storage requirements.\n"));
    }
    return TRUE;
}  /* change_min_wordsize() */


/************************************************/
/*						*/
/*		   print_usage			*/
/*						*/
/************************************************/
static void	print_usage (void)
{
    int		i;

    printf (catgets (dtsearch_catd, MS_initausd,
	    3,
  "\nUSAGE: %s [-options] dbname\n"
  "       Creates and initializes DtSearch/AusText database files.\n"
  "  -q       Do not print information messages.\n"
  "  -o       Ok to overwrite preexisting database.\n"
  "  -a<n>    Set maximum abstract size to <N> (default per flavor).\n"
  "  -d<dir>  Dir containing "FNAME_MODEL" file if not in dbname dir.\n"
  "  -wn<n>   Change minimum word size to <N>.  Default is %d.\n"
  "  -wx<n>   Change maximum word size to <N>.  Default per language.\n"
  "  ---------- Database Flavor ----------\n"
  "  -fd      DtSearch flavor.  No documents, only document references\n"
  "           in abstracts (default).\n"
  "  -fa      AusText flavor.  Documents stored in central server repository.\n"
  "  ------------ Supported Languages ------------\n"
  "  -l<n>    Set language number to <N>.  Default is 0.  Supported values:\n"
  "           0 English-ASCII\n"
  "           1 English-Latin1\n"
  "           2 Spanish\n"
  "           3 French\n"
  "           4 Italian\n"
  "           5 German\n"
  "           6 Japanese-autoknj\n"
  "           7 Japanese-knjlist\n"
  "  <dbname> Optional path prefix, then 1 - 8 character\n"
  "           database name. Do not specify 'austext' or 'dtsearch'.\n"),
	    aa_argv0, DEFAULT_MINWORD);

    return;
}  /* print_usage() */


/************************************************/
/*						*/
/*		user_args_processor		*/
/*						*/
/************************************************/
/* Handles command line arguments for main().
 * Initializes global variables.
 */
static void     user_args_processor (int argc, char **argv)
{
    int		i;
    int		remaining_slot_space;
    char	*ptr;

    /* Initialize variables prior to parsing command line */
    newpath[0] = 0;
    modelpath[0] = 0;

    if (argc < 2) {
	print_usage();
	DtSearchExit (2);
    }

    /* Each pass grabs new parm of "-xxx" format */
    for (;;) {
	argc--;
	argv++;
	if (argc <= 0)
	    break;
	ptr = argv[0];
	if (ptr[0] != '-')
	    break;

	switch (ptr[1]) {
	    case 'r':	/* unadvertised debug mode */
		if (strcmp (ptr, "-russell") == 0) {
		    debug_mode = TRUE;
		    puts ("001*** debug mode.");
		}
		else {
BAD_ARG:
		    print_usage();
		    printf (catgets (dtsearch_catd, MS_misc, 9,
			    "%sInvalid command line argument '%s'.\a\n"),
			"\n"PROGNAME" ", ptr);
		    DtSearchExit (2);
		}
		break;

	    case 'a':
		/* zero length abstract may be explicity specified */
		abstrsz = atoi (ptr + 2);
		if (abstrsz < 0 || (abstrsz == 0 && ptr[2] != '0'))
		    goto BAD_ARG;
		break;

	    case 'q':
		quiet_mode = TRUE;
		break;

	    case 'o':
		ok_to_overwrite = TRUE;
		break;

	    case 'f':
		switch (ptr[2]) {
		    case AUSTEXT_FLAVOR:
		    case DTSEARCH_FLAVOR:
			flavor = ptr[2];
			break;
		    default:
			goto BAD_ARG;
		}
		break;

	    case 'w':	/* change min (-wn..) or max (-wx..) word size */
		switch (ptr[2]) {
		    case 'x':
			if (!change_max_wordsize (ptr + 3))
			    goto BAD_ARG;
			break;
		    case 'n':
			if (!change_min_wordsize (ptr + 3))
			    goto BAD_ARG;
			break;
		    default:
			goto BAD_ARG;
		}
		break;

	    case 'd':	/* special path name for model .dbd */
		strncpy (modelpath, ptr + 2, sizeof(modelpath));
		modelpath [sizeof(modelpath) - sizeof(FNAME_MODEL) - 4] = 0;
		ensure_end_slash (modelpath);
		strcat (modelpath, FNAME_MODEL);
		break;

	    case 'l':
		/* Note that custom, unsupported languages
		 * greater than DtSrLaLAST are permitted.
		 */
		language = atoi (ptr + 2);
		if (language < 0)
		    goto BAD_ARG;
		if (!quiet_mode  &&  language > DtSrLaLAST)
		    printf ( catgets(dtsearch_catd, MS_initausd, 13,
			"%s Warning! you have specified "
			"an unsupported, custom language.\n"
			"  You will have to provide your own "
			"language loaders at run time\n"
			"  in user function 'load_custom_language' "
			"to access this database.\a\n"),
			PROGNAME"444");
		break;

	    default:
		printf (catgets (dtsearch_catd, MS_misc, 10,
			"%sIgnored unknown command line argument '%s'.\n"),
		    PROGNAME " ", ptr);
		break;
	}	/* end switch */
    }	/* end parse of cmd line options beginning with '-' */

    /* Only required arg is new database name,
     * including optional path prefix.
     * Load newpath and newextp, leaving room
     * for long dbnames and .xxx extensions.
     */
    if (argc <= 0) {
	print_usage();
	printf (catgets (dtsearch_catd, MS_misc, 18,
		"%sDatabase name not specified.\n\a"), "\n"PROGNAME" ");
	DtSearchExit(2);
    }
    strncpy (newpath, argv[0], sizeof (newpath));
    newpath [sizeof(newpath) - 12] = 0;
    newextp = newpath + strlen (newpath);

    /* Get just the 1 - 8 char database name by moving ptr
     * backwards until first non-alphanumeric character
     * (such as a ":" in the dos drive id or a slash between directories),
     * or to the beginning of string.
     * Then test database name for validity.
     */
    for (ptr = newpath + strlen(newpath) - 1;  ptr >= newpath;  ptr--)
	if (!isalnum (*ptr)) {
	    ptr++;
	    break;
	}
    if (ptr < newpath)
	ptr = newpath;
    i = strlen (ptr);
    if (i < 1 || i > 8) {
BAD_DBNAME:
	print_usage();
	printf (catgets (dtsearch_catd, MS_misc, 11,
		"%sInvalid database name '%s'.\a\n"),
	    "\n"PROGNAME"346 ", ptr);
	DtSearchExit(2);
    }
    path_offset = ptr - newpath;
    strcpy (dbname, ptr);	/* save it */
    if (strcmp (dbname, "austext") == 0 || strcmp (dbname, "dtsearch") == 0) {
	goto BAD_DBNAME;
    }

    /* Ensure semantic processing specified only for english language */
    if (fzkeysz != 0  && language != DtSrLaENG  &&  language != DtSrLaENG2) {
	print_usage();
	printf ( catgets(dtsearch_catd, MS_initausd, 14,
	    "\n%s semantic processing is only available "
	    "for English language databases.\n\a") ,
	    PROGNAME"340");
	DtSearchExit(2);
    }

    /* Unless overridden by user args,
     * initialize abstract based on flavor.
     * The abstract size defaults to the remaining 
     * space in the final misc slot after the fzkey.
     * However if the user specified a specific
     * abstract size, it may be adjusted later
     * to fill up the last slot.
     */
    if (abstrsz == -1)
	abstrsz = max_ormisc_size - (fzkeysz % max_ormisc_size);

    /* Default maxword size is 'short', except for German */
    if (maxwordsz == INT_MAX)
	maxwordsz = STANDARD_MAXWORD;

    if (debug_mode)
	printf ("002*** userargs: modelpath='%s' newpath='%s'\n"
	    "  fzkeysz=%d abstrsz=%d\n",
	    modelpath, newpath, fzkeysz, abstrsz);
    return;
}  /* user_args_processor() */


/************************************************/
/*						*/
/*               remove_d9x_file		*/
/*						*/
/************************************************/
static void     remove_d9x_file (char *extension)
{
                    strcpy (newextp, extension);
    if (debug_mode)
	printf ("094*** delete '%s'.\n", newpath);
    if (remove (newpath) != 0) {
	/* 'file not found' is not an error */
	if (errno != ENOENT) {
	    printf (catgets (dtsearch_catd, MS_initausd, 244,
		    PROGNAME "244 Unable to remove '%s': %s\n"),
		newpath, strerror (errno));
	    DtSearchExit (5);
	}
    }
    return;
}  /* remove_d9x_file() */


/************************************************/
/*						*/
/*		 create_new_dbd			*/
/*						*/
/************************************************/
/* Copies and moves binary contents in passed, preopened
 * model .dbd file (f) to new dbd file in target directory.
 * Rename the internal .d00, etc filenames to match dbname.
 */
static void     create_new_dbd (FILE *f)
{
    FILE	*g;	/* target dbd file */
    int		i;
    static char	*nocopy_msg =
		"%s Unable to copy '%s' to '%s':\n  %s\a\n";
		/* (Same as dtsearch.msg: MS_initausd, 214) */
    static char	zeros[] =
		"\0\0\0\0\0\0\0\0\0\0\0\0";

    strcpy (newextp, ".dbd");
    if (debug_mode)
	printf (PROGNAME"507 create_new_dbd '%s'\n", newpath);

    /* If new .dbd file preexists, make sure it is writable */
    confirm_ok_to_overwrite (newpath);
    if (chmod (newpath, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) {
	if (errno != ENOENT) {
	    printf (catgets (dtsearch_catd, MS_initausd, 214, nocopy_msg),
		PROGNAME"515", modelpath, newpath, strerror(errno));
	    DtSearchExit (15);
	}
    }

    if ((g = fopen (newpath, "w+b")) == NULL) {
	printf (catgets (dtsearch_catd, MS_initausd, 214, nocopy_msg),
	    PROGNAME"509", modelpath, newpath, strerror(errno));
	DtSearchExit (4);
    }
    errno = 0;
    while ((i = fgetc (f)) != EOF)
	fputc (i, g);
    if (errno) {
	printf (catgets (dtsearch_catd, MS_initausd, 214, nocopy_msg),
	    PROGNAME"531", modelpath, newpath, strerror(errno));
	DtSearchExit (13);
    }

    /* Now reposition the write head in the new dbd file
     * to rename the filenames.  Rename each internal file
     * name to '<newdbname>.xxx'.
     */
    for (i = 0;  exttab[i] != NULL;  i++) {
	fseek (g, START_OF_FT + (i * SIZEOF_FILE_ENTRY), SEEK_SET);	
	fprintf (g, "%s%s", dbname, exttab[i]);
	fwrite (zeros, sizeof(char), sizeof(zeros), g);
    }

    /* The new dbd file only has to be readable */
    fclose (g);
    chmod (newpath, S_IRUSR | S_IRGRP | S_IROTH);
    return;
}  /* create_new_dbd() */


/************************************************/
/*                                              */
/*                    main                      */
/*                                              */
/************************************************/
/* 1. CREATE or find database dictionary (.dbd file).
 * 2. CREATE empty 'dtsearch' database files.
 * 3. OPEN 'dtsearch' database.
 * 4. INITIALIZE the database.
 * 5. WRITE dbrec after initializing it.
 * 6. RENAME each database file.
 * 7. UNLINK (delete) d9x files.
 */
int             main (int argc, char *argv[])
{
    int		i;
    char	*ptr;
    FILE	*f;
    struct or_miscrec	miscrec;
    struct or_swordrec	swordrec;
    struct or_lwordrec	lwordrec;

    setlocale (LC_ALL, "");
    dtsearch_catd = catopen (FNAME_DTSRCAT, 0);

    aa_argv0 = argv[0];
    max_ormisc_size = sizeof (miscrec.or_misc);
    maxwidth_sword = sizeof (swordrec.or_swordkey) - 1;
    maxwidth_lword = sizeof (lwordrec.or_lwordkey) - 1;

    printf (catgets (dtsearch_catd, MS_misc, 4,
	    "%s Version %s.\n"),
	aa_argv0,
	DtSrVERSION
	);

    /* Handle cmd line args.  Init global variables. */
    user_args_processor (argc, argv);

    /* ------- copy model .dbd to new .dbd ------- */

    /* CASE 1: If user specified -d special alternative
     * directory for model .dbd, it should be there.
     */
    if (modelpath[0] != 0) {
	if (debug_mode)
	    printf (PROGNAME"628 Try opening '%s' (-d dir).\n", modelpath);
	if ((f = fopen (modelpath, "rb")) != NULL) {
	    if (debug_mode)
		puts (PROGNAME"638 Found it!");
	    create_new_dbd (f);
	    fclose (f);
	    goto DBD_OKAY;
	}
	else {
	    print_usage();
	    printf (catgets (dtsearch_catd, MS_initausd, 213,
		    default_unable_to_open_msg),
		"\n"PROGNAME"302", modelpath, strerror(errno));
	    DtSearchExit (4);
	}
    } /* end CASE 1 */

    /* CASE 2: If model .dbd is in current directory, use it.
     * If error is anything other than 'cant find file', quit now.
     */
    if (debug_mode)
	printf (PROGNAME"649 Try opening '%s' (curr dir).\n", FNAME_MODEL);
    if ((f = fopen (FNAME_MODEL, "rb")) != NULL) {
	if (debug_mode)
	    puts (PROGNAME"660 Found it!");
	create_new_dbd (f);
	fclose (f);
	goto DBD_OKAY;
    }
    else if (errno != ENOENT) {
	print_usage();
	printf (catgets (dtsearch_catd, MS_initausd, 213,
		default_unable_to_open_msg),
	    "\n"PROGNAME"655", FNAME_MODEL, strerror(errno));
	DtSearchExit (4);
    } /* end else CASE 2 */

    /* CASE 3: Last chance.  Look for model .dbd in target directory.
     * At this point have to quit on any error.
     */
    strcpy (modelpath, newpath);
    strcpy (modelpath + path_offset, FNAME_MODEL);
    if (debug_mode)
	printf (PROGNAME"672 Try opening '%s' (new dir).\n", modelpath);
    if ((f = fopen (modelpath, "rb")) != NULL) {
	if (debug_mode)
	    puts (PROGNAME"675 Found it!");
	create_new_dbd (f);
	fclose (f);
	goto DBD_OKAY;
    }

    if (debug_mode)
	puts (PROGNAME"682 Never found it!");
    print_usage();
    printf (catgets (dtsearch_catd, MS_initausd, 213,
	    default_unable_to_open_msg),
	"\n"PROGNAME"686", FNAME_MODEL,
	"Not found in either current or target directories.  Use -d option\a");
    DtSearchExit (4);


DBD_OKAY:

    /* Open a new database */
    *newextp = 0;	/* use no extension when opening database */
    if (debug_mode)
	printf ("040*** d_open newpath = '%s'.\n", newpath);
    d_open (newpath, "o");
    if (db_status != S_OKAY) {
	printf (catgets (dtsearch_catd, MS_initausd, 230,
		PROGNAME "230 Could not open database '%s'.\n"), newpath);
	puts (vista_msg (PROGNAME "231"));
	DtSearchExit (3);
    }
    austext_exit_dbms = (void (*) (int)) d_close;	/* emerg exit func */

    /* initialize the 'dtsearch' database */
    if (debug_mode)
	printf ("042*** d_initialize.\n");
    d_initialize (0);
    if (db_status != S_OKAY) {
	printf (catgets (dtsearch_catd, MS_initausd, 239,
		PROGNAME "239 Could not initialize database '%s'.\n"), newpath);
	puts (vista_msg (PROGNAME "240"));
	DtSearchExit (3);
    }


    /* Create and initialize dbrec database header record in first slot.
     * First fill entire record with binary zeros.
     * Then set specific values as specified by flavor on command line.
     * For now most values are hard-coded.
     */
    if (debug_mode)
	printf ("050*** create dbrec.\n");
    memset (&dbrec, 0, sizeof (dbrec));

    /* Init fields that are completely independent */
    dbrec.or_language =		(DtSrINT16) language;
    dbrec.or_maxwordsz =	(DtSrINT16) maxwordsz;
    dbrec.or_minwordsz =	(DtSrINT16) minwordsz;
    dbrec.or_fzkeysz =		(DtSrINT16) fzkeysz;
    dbrec.or_abstrsz =		(DtSrINT16) abstrsz;
    dbrec.or_dbflags =		ORD_NONOTES | ORD_NOMARKDEL | ORD_XWORDS;
    strncpy (dbrec.or_version, SCHEMA_VERSION, sizeof(dbrec.or_version));
    dbrec.or_version [sizeof(dbrec.or_version) - 1] = 0;

    /* Load dbrec's recslots fields based on correct number
     * of misc recs required to hold user's abstract.
     * Round abstrsz upward if there is any space left on last misc rec.
     */
    dbrec.or_recslots = 1;	/* start with obj rec itself */
    for (i = dbrec.or_fzkeysz + dbrec.or_abstrsz; i > 0; i -= max_ormisc_size)
	dbrec.or_recslots++;
    if (i < 0) {
	/* Add in difference to INCREASE abstrsz */
	dbrec.or_abstrsz -= i;
	printf (catgets (dtsearch_catd, MS_misc, 433,
		"%1$sAdjusted maximum abstract size upward to %2$hd.\n"),
	    PROGNAME "433 ", dbrec.or_abstrsz);
    }

    /* Init fields that are dependent on language */
    switch (language) {
	case DtSrLaENG:
	case DtSrLaENG2:
	    dbrec.or_dbflags |= ORD_XSTEMS;
	    break;
	default:
	    break;
    }

    /* Init fields that are dependent on flavor */
    if (flavor == AUSTEXT_FLAVOR) {
	dbrec.or_dbaccess = ORA_BLOB;
	dbrec.or_compflags = ORC_COMPBLOB;
	dbrec.or_hufid = -1L;	/* -1 = use huffman compression, but
				 * hufid not yet known. */
	dbrec.or_dbotype = DtSrObjTEXT;
    }
    else {	/* default flavor == DTSEARCH_FLAVOR */
	dbrec.or_dbaccess = ORA_NOTAVAIL;
    }

    if (!quiet_mode) {
	/******putchar ('\n');******/
	print_dbrec (newpath, &dbrec);
	fflush (stdout);
    }
    swab_dbrec (&dbrec, HTON);
    if (debug_mode)
	printf ("060*** fillnew dbrec.\n");
    d_fillnew (OR_DBREC, &dbrec, 0);
    if (db_status != S_OKAY) {
	printf ("%s", catgets (dtsearch_catd, MS_initausd, 509,
		PROGNAME "509 Could not initialize database header record.\n"));
	puts (vista_msg (PROGNAME "510"));
	DtSearchExit (3);
    }

    /* Close the database */
    d_close ();
    austext_exit_dbms = NULL;	/* emerg exit no longer required */

    /* Delete all nonvista (inverted index) database files (.d9x) */
    remove_d9x_file (".d97");
    remove_d9x_file (".d98");
    remove_d9x_file (".d99");

    *newextp = 0;	/* no extension suffixes for next msgs */
    printf (catgets (dtsearch_catd, MS_initausd, 24,
	    PROGNAME " Successfully initialized database '%s'.\n"), newpath);

    return 0;
}  /* main() */

/************************* DTSRCREATE.C **************************/
