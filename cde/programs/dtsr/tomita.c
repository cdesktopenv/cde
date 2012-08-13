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
 *   FUNCTIONS: browser
 *              change_database
 *              deleter
 *              kill_delete
 *              load_dbatab
 *              main
 *              parse_infbuf
 *              print_exit_code
 *              retncode_abort
 *
 *   ORIGINS: 27
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1992,1995
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/******************* TOMITA.C *******************
 * $TOG: tomita.c /main/9 1998/04/17 11:23:38 mgreess $
 * May 1992.
 * Replaces original tomita but removes curses dependencies
 * and uses Opera Engine for deletes rather than hard
 * coding vista calls.
 * 
 * In effect, tomita is two programs.
 * Program #1 browses an input list of opera record ids and
 * prompts user to confirm that they should be deleted from opera.
 * The confirmed list is written to an output file that is identical in format.
 * Program 1 can be run anytime because it only reads the database.
 * 
 * Program #2 deletes record from an input list, presumably
 * the output of program #1.  Program #2 writes to vista and changes it.
 * It MUST be run offline when no users are logged into opera,
 * in order to prevent database corruption.
 * Currently password is passed on command line.
 * The password is maintained in an encrypted flat file.
 * It can be changed from an undocumented Opera Engine function
 * available in tuiopera.
 * 
 * RECORD ID FILE FORMAT (shdscrd.lst format):
 * One record to be deleted per line.
 * 3 words per line separated by whitespace, everything thereafter is comments.
 * (These programs only use the first 2 words).
 * All words may be optionally enclosed in double quotes
 * to capture embedded blanks ("xxx xxx").
 * 	...
 * 	databasename  recordid  userid  comments...\n
 * 	...
 *
 * $Log$
 * Revision 2.2  1995/10/25  15:21:36  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  22:17:11  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.11  1995/09/05  19:14:39  miker
 * Removed password requirement.  DtSearch header file and function
 * name changes.  Made usrblk universal global.
 */
#include "SearchE.h"
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/stat.h>
#include <locale.h>

#define PRINT_MESSAGES \
    { puts (DtSearchGetMessages()); DtSearchFreeMessages(); }
#define TOKEN_DELIM	" \t\n"
#define PAUSE_ROWS	15
#define DBACOUNT	2000
#define PROGNAME	"TOMITA"
#define MS_tomita	29
#define MS_misc		1

/*------------------ GLOBALS ------------------*/
static int	debug_mode =	FALSE;
static int	prog =		'B';	/* 'D' = deleting, 'B' = browsing */
static int	shutdown_now =	FALSE;
static int	yesarg =	FALSE;
static int	retncode =	0;
static int	max_dbacount =	DBACOUNT;
static FILE	*inf, *outf;
static char	*infname;
static char	*outfname;
static long	maxtime;
static long	records_read;
static char	parsed_dbname [24];
static DBLK	*parsed_dblk;

char		parsed_recid [2048];


/************************************************/
/*                                              */
/*               print_exit_code                */
/*                                              */
/************************************************/
/* Called from inside DtSearchExit() at austext_exit_last */
static void     print_exit_code (int exit_code)
{
    printf ( catgets(dtsearch_catd, MS_tomita, 3,
	"%s: Exit Code = %d.\n") ,
	aa_argv0, exit_code);
    return;
} /* print_exit_code() */


/************************************************/
/*						*/
/*		   kill_delete			*/
/*						*/
/************************************************/
/* Interrupt handler for all termination signals
 * in Delete mode.  Just sets global flag so we
 * can come down gracefully between deletions.
 */
static void	kill_delete (int sig)
{
    shutdown_now = TRUE;
    printf ( catgets(dtsearch_catd, MS_tomita, 1,
	"\n%s Received interrupt %d.\n"
	"  Program will stop after current batch of deletions.\n") ,
	PROGNAME"069", sig);
    return;
}  /* kill_delete() */


/************************************************/
/*						*/
/*		retncode_abort			*/
/*						*/
/************************************************/
static void     retncode_abort (int location)
{
    fputc ('\n', aa_stderr);
    if (DtSearchHasMessages ())
	fprintf (aa_stderr, "%s\n", DtSearchGetMessages ());
    fprintf (aa_stderr,
	PROGNAME "%d Program abort.  usrblk.retncode = %d.  Exit code = 3.\n",
	location, usrblk.retncode);
    DtSearchExit (3);
}  /* retncode_abort() */


/****************************************/
/*					*/
/*	     change_database		*/
/*					*/
/****************************************/
/* Changes usrblk.dblk to point to passed database name.
 * Returns TRUE if successful.
 */
static int      change_database (char *newname)
{
    DBLK           *db;

    for (db = usrblk.dblist; db != NULL; db = db->link)
	if (strcmp (db->name, newname) == 0) {
	    usrblk.dblk = db;
	    return TRUE;
	}

    /* Invalid newname.  If deleting, just say which database is invalid. */
    retncode = 1;
    fprintf (aa_stderr,  catgets(dtsearch_catd, MS_tomita, 4,
	"%s Database '%s' not found.\n") ,
	PROGNAME"114", newname);
    if (prog == 'D')
	return FALSE;

    /* If browsing, tell user his options */
    fprintf (aa_stderr,  catgets(dtsearch_catd, MS_tomita, 5,
	"Available choices are:") );
    for (db = usrblk.dblist; db != NULL; db = db->link)
	fprintf (aa_stderr, "   '%s'", db->name);
    fputc ('\n', aa_stderr);
    return FALSE;
}  /* change_database() */


/****************************************/
/*					*/
/*	     parse_infbuf		*/
/*					*/
/****************************************/
/* Parses a line from a standard formatted discard file.
 * If first word indicates different database from usrblk.dblk,
 * changes it.  First token loaded into parsed_dbname
 * (and parsed_dblk will be made to track it), and
 * second token is loaded into parsed_recid.
 * Tokens are separated by blanks and/or tabs, 
 * except 2nd token may have embedded spaces if it is
 * surrounded by double quotes.  Returns TRUE unless
 * database couldn't change or other error, then returns FALSE.
 */
static int      parse_infbuf (char *infbuf)
{
    char           *ptr;
    char            mybuf[1024];

    /* Do all parsing in my own buf so infbuf not peppered with \0's */
    strncpy (mybuf, infbuf, sizeof (mybuf));
    mybuf[sizeof (mybuf) - 1] = 0;

    /* Parse first token (database name) */
    if ((ptr = strtok (mybuf, " \t")) == NULL) {
	/* Msg #8 is used in two places */
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_tomita, 8,
	    "%s Invalid input format: %.30s...\n") ,
	    PROGNAME"152", infbuf);
	retncode = 1;
	return FALSE;
    }

    /* Change database if necessary */
    if (strcmp (ptr, usrblk.dblk->name) != 0)
	if (!change_database (ptr)) {
	    retncode = 1;
	    return FALSE;
	}

    strcpy (parsed_dbname, ptr);
    parsed_dblk = usrblk.dblk;

    /* Hop over to beginning of 2nd token */
    for (ptr += strlen (ptr) + 1; *ptr == ' ' || *ptr == '\t'; ptr++);

    /* Get 2nd token (record id).  Token delimiters depend
     * on whether token begins with a double quote.
     */
    ptr = strtok (ptr, (*ptr == '\"') ? "\"" : " \t");
    if (ptr == NULL) {
	/* Msg #8 is used in two places */
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_tomita, 8,
	    "%s Invalid input format: %.30s...\n") ,
	    PROGNAME"176", infbuf);
	retncode = 1;
	return FALSE;
    }
    strncpy (parsed_recid, ptr, sizeof (parsed_recid));
    parsed_recid[sizeof (parsed_recid) - 1] = 0;

    return TRUE;
}  /* parse_infbuf() */


/****************************************/
/*					*/
/*		 browser		*/
/*					*/
/****************************************/
/* Program 1: displays records in input file,
 * or user selected records, and if confirmed,
 * writes their record ids to output file.
 */
static int      browser (void)
{
    int		done = FALSE;
    int		pausing = FALSE;
    int		redisplay_rec = FALSE;
    int		pause_counter;
    time_t	stamp;
    LLIST	*llptr;
    char	*ptr;
    char	datestr[32];	/* "1946/04/17 13:03" */
    char	userbuf[1024];
    char	infbuf[1024];

    /* All writes to output file will have same date string in comment */
    time (&stamp);
    strftime (datestr, sizeof (datestr), "%Y/%m/%d %H:%M", localtime (&stamp));

    /* Main menu loop */
    while (!done) {
	if (DtSearchHasMessages ()) {
	    putchar ('\n');
	    PRINT_MESSAGES
	}

	/* Write main menu prompt */
	printf ( catgets(dtsearch_catd, MS_tomita, 10,
	    "\n---------- SHOW NEXT RECORD ----------- Database = '%s'\n"
	    "q      QUIT.                        Current Record Count = %ld\n"
	    "p      Toggle PAUSE from %s.\n"
	    "n      NEXT INPUT file record.\n"
	    "+      NEXT SEQUENTIAL database record.\n"
	    "-      PRIOR SEQUENTIAL database record.\n"
	    "r      REDISPLAY current record '%s'.\n"
	    "x      CONFIRM DELETION of current record.\n"
	    "dxxx   Change DATABASE to xxx.\n"
	    "\"xxx   GET record id xxx (embedded blanks are ok).\n"
	    "> ") ,
	    usrblk.dblk->name,
	    usrblk.dblk->dbrec.or_reccount,
	    (pausing) ? "on to OFF" : "off to ON",
	    usrblk.objrec.or_objkey
	    );

	/* Read user's response.  Remove user's \n. */
	*userbuf = '\0';
	if ((fgets (userbuf, sizeof (userbuf), stdin)) == NULL) break;
        if (strlen(userbuf) && userbuf[strlen(userbuf)-1] == '\n')
          userbuf[strlen(userbuf)-1] = '\0';

	putchar ('\n');

	/* depending on response, get database address into usrblk */
	redisplay_rec = FALSE;
	switch (tolower (*userbuf)) {
	    case 'q':
		done = TRUE;
		break;

	    case 'd':
		change_database (userbuf + 1);
		continue;
		break;

	    case 'p':
		pausing = !pausing;
		continue;
		break;

	    case 'r':
		if (usrblk.objrec.or_objkey[0] == 0) {
		    fprintf (aa_stderr,
			catgets(dtsearch_catd, MS_tomita, 11,
			"%s Record buffer empty.\n"),
			PROGNAME"267");
		    continue;
		}
		redisplay_rec = FALSE;
		break;

	    case '+':
	    case '-':
		usrblk.request = (*userbuf == '+') ? OE_NEXT_DBA : OE_PREV_DBA;
		Opera_Engine ();
		break;

	    case 'n':
		if (inf == NULL) {
		    fprintf (aa_stderr,
			catgets(dtsearch_catd, MS_tomita, 12,
			"%s Input file unavailable.\n"),
			PROGNAME"282");
		    continue;
		}
		*infbuf = '\0';
		if ((fgets (infbuf, sizeof (infbuf), inf)) == NULL)
		{
		    fprintf (aa_stderr,
			catgets(dtsearch_catd, MS_tomita, 13,
			"%s No more records in input file.\n"),
			PROGNAME"288");
		    fclose (inf);
		    inf = NULL;
		    continue;
		}

    		if (strlen(infbuf) && infbuf[strlen(infbuf)-1] == '\n')
      		  infbuf[strlen(infbuf)-1] = '\0';

		if (!parse_infbuf (infbuf))
		    continue;
		usrblk.request = OE_RECKEY2DBA;
		usrblk.query = parsed_recid;
		Opera_Engine ();
		break;

	    case '\"':
		ptr = strtok (userbuf, "\"");
		if (ptr == NULL || *ptr == 0) {
		    fprintf (aa_stderr,
			catgets(dtsearch_catd, MS_tomita, 14,
			"%s Invalid Record ID.\n"),
			PROGNAME"303");
		    continue;
		}
		usrblk.request = OE_RECKEY2DBA;
		usrblk.query = ptr;
		Opera_Engine ();
		break;

	    case 'x':
		/*
		 * Write record id to output file. Format:
		 * dbasename "recid" userid comments(date)... 
		 */
		fprintf (outf, DISCARD_FORMAT, usrblk.dblk->name,
		    usrblk.objrec.or_objkey, usrblk.userid, datestr);
		printf ( catgets(dtsearch_catd, MS_tomita, 15,
		    "%s '%s' appended to file of confirmed deletions.\n") ,
		    PROGNAME"317", usrblk.objrec.or_objkey);
		continue;

	    default:
		printf (catgets(dtsearch_catd, MS_tomita, 16, "...what?\n"));
		continue;
	}	/* end switch */

	if (done)
	    break;

	/* if user requested redisplay, skip the following OE code */
	if (redisplay_rec)
	    goto DISPLAY_RECORD;

	/*
	 * check return code from attempt to get opera database
	 * address 
	 */
	if (usrblk.retncode == OE_WRAPPED)
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_tomita, 17,
		"%s %s Engine wrapped to next record.\n") ,
		PROGNAME"333", OE_prodname);
	else if (usrblk.retncode != OE_OK)
	    retncode_abort (334);

	/* retrieve the record and uncompress it */
	usrblk.request = OE_GETREC;
	Opera_Engine ();
	if (usrblk.retncode != OE_OK)
	    retncode_abort (339);

DISPLAY_RECORD:
	/* display the record's cleartext, character by character */
	printf ( catgets(dtsearch_catd, MS_tomita, 18,
	    "\n\n"
	    "Record:    '%s'\n"
	    "Abstract:  '%s'\n"
	    "--------------------------------------\n") ,
	    usrblk.objrec.or_objkey,
	    (usrblk.abstrbufsz > 0) ? usrblk.abstrbuf :
                                      catgets (dtsearch_catd, MS_misc, 1, "<null>"));

	pause_counter = 0;
	for (ptr = usrblk.cleartext; *ptr != 0; ptr++) {
	    putchar (*ptr);
	    /*
	     * pause every so many lines so user can browse the
	     * output 
	     */
	    if (pausing && *ptr == '\n') {
		if (++pause_counter >= PAUSE_ROWS) {
		    /* Msg 21 is used in two places */
		    printf ( catgets(dtsearch_catd, MS_tomita, 21,
			"\n...push ENTER to continue... ") );

		    *userbuf = '\0';
		    fgets (userbuf, sizeof (userbuf), stdin);
    		    if (strlen(userbuf) && userbuf[strlen(userbuf)-1] == '\n')
      		      userbuf[strlen(userbuf)-1] = '\0';

		    putchar ('\n');
		    pause_counter = 0;
		}
	    }
	}	/* end of cleartext printing */

	/* display the user notes if any, character by character */
	if (usrblk.notes != NULL) {
	    printf ( catgets(dtsearch_catd, MS_tomita, 20,
		"--------------------------------------\n"
		"End of Text Blob for '%s':\n\n"
		"User Notes:\n"
		"--------------------------------------\n") ,
		usrblk.objrec.or_objkey);
	    pause_counter += 5;
	    for (llptr = usrblk.notes; llptr != NULL; llptr = llptr->link) {
		for (ptr = llptr->data; *ptr != '\0'; ptr++) {
		    putchar (*ptr);
		    if (pausing && *ptr == '\n')
			if (++pause_counter >= PAUSE_ROWS) {
			    /* Msg 21 is used in two places */
			    printf ( catgets(dtsearch_catd, MS_tomita, 21,
				"\n...push ENTER to continue... ") );

		            *userbuf = '\0';
		            fgets (userbuf, sizeof (userbuf), stdin);
    		            if (strlen(userbuf) &&
				userbuf[strlen(userbuf)-1] == '\n')
      		              userbuf[strlen(userbuf)-1] = '\0';

			    putchar ('\n');
			    pause_counter = 0;
			}
		}
	    }
	}	/* end of user notes printing */

	printf ("--------------------------------------\n"
	    "End of Record '%s'.\n", usrblk.objrec.or_objkey);

    }	/* end of main menu loop */
    return 0;
}  /* browser() */


/****************************************/
/*					*/
/*		load_dbatab		*/
/*					*/
/****************************************/
/* Subroutine of deleter().  Reads discard file containing
 * record ids to be deleted, converts to database addresses,
 * loads usrblk.dbatab up to max batch size.
 * Returns number of dba's added to table.
 * Returns 0 when file is empty after last batch.
 */
static int      load_dbatab (void)
{
    static int      read_next_rec = TRUE;
    static char     last_dbname[24] = "";
    static DBLK    *last_dblk;
    DB_ADDR        *next_dba;
    char            buf[1024];
    int             first_err = TRUE;

    if (inf == NULL)
	return 0;

    usrblk.dbacount = 0;
    next_dba = usrblk.dbatab;

KEEP_READING:
    /* MAIN LOOP - break it at EOF, max count, or dbname change */
    while (usrblk.dbacount < max_dbacount) {
	/*
	 * Skip the read of the first record if the reason we left
	 * main loop the last time was because of a database name
	 * change, and the data from the last read is still in
	 * parsed_dbname, _dblk, and _recid. Update usrblk.dblk
	 * because it's based on the last table's database. 
	 */
	if (!read_next_rec) {
	    read_next_rec = TRUE;
	    usrblk.dblk = parsed_dblk;
	}
	else {
	    *buf = '\0';
	    if (fgets (buf, sizeof (buf), inf) == NULL)
	    {
		fclose (inf);
		inf = NULL;
		break;
	    }
	    records_read++;
	    buf[sizeof (buf) - 1] = 0;	/* guarantee termination */
    	    if (strlen(buf) && buf[strlen(buf)-1] == '\n')
      	      buf[strlen(buf)-1] = '\0';


	    /*
	     * Parse line into dbname and recid.  Skip line if
	     * error. 
	     */
	    if (!parse_infbuf (buf))
		continue;

	    /* on very first read, save the database name */
	    if (last_dbname[0] == 0) {
		strcpy (last_dbname, parsed_dbname);
		last_dblk = parsed_dblk;
	    }
	}	/* finished reading next rec in input file */

	/*
	 * Test for change of database name.  Restore usrblk.dblk
	 * to reflect all the records on the dba table so far. Then
	 * save the new dblk for when we are again called. 
	 */
	if (strcmp (last_dbname, parsed_dbname) != 0) {
	    read_next_rec = FALSE;
	    strcpy (last_dbname, parsed_dbname);
	    usrblk.dblk = last_dblk;
	    last_dblk = parsed_dblk;
	    break;
	}

	/*
	 * Call OE to get record's db address. Turn off debug
	 * temporarily so won't flood output with messages. 
	 */
	usrblk.query = parsed_recid;
	usrblk.debug &= ~USRDBG_DELETE;
	usrblk.request = OE_RECKEY2DBA;
	Opera_Engine ();
	if (debug_mode)	/* restore */
	    usrblk.debug |= USRDBG_DELETE;
	if (DtSearchHasMessages ()) {
	    putchar ('\n');
	    PRINT_MESSAGES
	}

	if (usrblk.retncode == OE_WRAPPED) {
	    if (first_err) {
		first_err = FALSE;
		fputc ('\n', aa_stderr);
	    }
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_tomita, 24,
		"%s Database %s, '%s' not found.\n") ,
		PROGNAME"482", parsed_dbname, parsed_recid);
	    continue;
	}
	else if (usrblk.retncode != OE_OK)
	    retncode_abort (486);

	/* add db address to growing table */
	*next_dba = usrblk.dba;
	next_dba++;
	usrblk.dbacount++;

    }	/* end of main record read loop */

    /* It is possible to exit the main loop, because database changed
     * or whatever, but no records were added to usrblk.dbatab.
     * If there are still records to be read from the input file,
     * go back and try another pass.
     */
    if (inf != NULL && usrblk.dbacount == 0)
	goto KEEP_READING;

    return usrblk.dbacount;
}  /* load_dbatab() */


/****************************************/
/*					*/
/*		 deleter		*/
/*					*/
/****************************************/
/* Program 2: deletes records specified in input file.
 * Must be run offline when all online users have logged off.
 */
static void     deleter (char *infname)
{
    int             i;
    long            records_deleted;
    time_t          start_time, minutes, hours, seconds, elapsed;
    char            buf[128];

    if (!yesarg) {
	printf ( catgets(dtsearch_catd, MS_tomita, 25,
    "\nDO NOT CONTINUE under any of the following circumstances:\n"
    "-> If the input file which lists record ids to be deleted is not\n"
    "      named '%s'.\n"
    "-> If any users are still accessing the affected database(s).\n"
    "-> If any database files have not been backed up.\n\n"
    "If you are sure you are ready to start deleting, enter 'y' now... ") ,
	    infname, OE_prodname);

	fgets (buf, sizeof(buf)-1, stdin);
	if (tolower (*buf) != 'y')
	    return;
    }

    /* Make sure engine doesn't abort because of
     * recurring changes to d99 files.
     */
    OE_sitecnfg_mtime = 0L;

    /* Init table of db addrs */
    usrblk.dbatab = austext_malloc
	(sizeof (DB_ADDR) * (max_dbacount + 2), PROGNAME "531", NULL);
    usrblk.dbacount = 0;	/* number of recs currently in table */

    /* Init status msg stuff */
    records_read = 0L;
    records_deleted = 0L;
    time (&start_time);

    signal (SIGINT, kill_delete);
    signal (SIGQUIT, kill_delete);
    signal (SIGTRAP, kill_delete);
    signal (SIGTERM, kill_delete);
#ifdef SIGPWR
    signal (SIGPWR, kill_delete);
#endif
#ifdef _AIX
    signal (SIGXCPU, kill_delete);	/* cpu time limit exceeded */
    signal (SIGDANGER, kill_delete);	/* imminent paging space
					 * crash */
#endif

    /* MAIN LOOP */
    while (load_dbatab ()) {
	/*
	 * Stop now if we have exceeded user specified time limit
	 * or if user sent termination or interrupt signal. 
	 */
	if (shutdown_now)
	    break;
	elapsed = time (NULL) - start_time;
	if (maxtime > 0L && elapsed >= maxtime)
	    break;

	/* echo status for humans who might be watching */
	hours = elapsed / 3600L;
	seconds = elapsed - (3600L * hours);	/* remaining after hours */
	minutes = seconds / 60L;
	seconds = seconds - (60L * minutes);
	printf ( catgets(dtsearch_catd, MS_tomita, 26,
	    "%s %ld read, %ld deleted, %ldh %2ldm %2lds elapsed.\n"
	    "  Database '%s': Current record count = %ld, Batch size = %d.\n") ,
	    aa_argv0, records_read, records_deleted,
	    hours, minutes, seconds,
	    usrblk.dblk->name, usrblk.dblk->dbrec.or_reccount, usrblk.dbacount);
	/*****fflush (stdout);*****/

	/* call OE to delete batch of records */
	usrblk.request = OE_DELETE_BATCH;
	Opera_Engine ();
	if (DtSearchHasMessages ()) {
	    putchar ('\n');
	    PRINT_MESSAGES
	}
	if (usrblk.retncode != OE_OK)
	    retncode_abort (572);
	records_deleted += usrblk.dbacount;

    }	/* end main loop */


    /* Print final status messages */
    elapsed = time (NULL) - start_time;	/* total elapsed time */

    hours = elapsed / 3600L;
    seconds = elapsed - (3600L * hours);	/* remaining after hours */
    minutes = seconds / 60L;
    seconds = seconds - (60L * minutes);	/* remaining after hours
						 * & mins */
    printf ( catgets(dtsearch_catd, MS_tomita, 27,
	"%s %ld records read from input file.  %ld were deleted and\n"
	"  %ld were not found in %ld hours, %ld minutes, %ld seconds,\n") ,
	aa_argv0, records_read, records_deleted,
	records_read - records_deleted,
	hours, minutes, seconds);

    /* Figure average time for a deletion */
    elapsed = (records_deleted) ? elapsed / records_deleted : 0L;
    minutes = elapsed / 60L;
    seconds = elapsed - (60L * minutes);
    printf ( catgets(dtsearch_catd, MS_tomita, 28,
	"  or an average of %ld minutes, %ld seconds per record deleted.\n"),
	minutes, seconds);
    return;
}  /* deleter() */


/****************************************/
/*					*/
/*		   main			*/
/*					*/
/****************************************/
int             main (int argc, char *argv[])
{
    char           *arg;
    time_t          mytime;
    char            timebuf[80];

    aa_argv0 = argv[0];
    setlocale (LC_ALL, "");
    dtsearch_catd = catopen (FNAME_DTSRCAT, 0);
    time (&mytime);
    strftime (timebuf, sizeof (timebuf),
        catgets(dtsearch_catd, MS_misc, 22, "%A, %b %d %Y, %I:%M %p"),
	localtime (&mytime));
    printf (catgets(dtsearch_catd, MS_tomita, 29,
	"%s.  Run %s.\n") ,
	aa_argv0, timebuf);
    austext_exit_last = print_exit_code;

    signal (SIGINT, DtSearchExit);
    signal (SIGTERM, DtSearchExit);
/****memset (&usrblk, 0, sizeof(USRBLK));****/

    /* Validate program number argument */
    if (argc < 2) {
BAD_ARGS:
	fprintf (aa_stderr, catgets(dtsearch_catd, MS_tomita, 30,
	    "\nUSAGE: %s [options]\n"
	    "  -i    Input file name.  If not specified, defaults to %s.\n"
	    "  -d[v] Print debug statements.\n"
	    "        -dv turns on verbose (record-by-record) debugging.\n"
	    "  -t<N> Max desired number of seconds of run time.\n"
	    "        Ctrl-C/Break will also stop deletion at next record.\n"
	    "  -n<N> Change number of records in a batch from %d to <N>.\n"
	    "  -y    Automatically answers 'yes' to Delete mode confirm prompt.\n"
	    "  -d    trace deletion operations.\n") ,
	    aa_argv0, FNAME_DISCARD_DATA,
	    FNAME_CONFIRM_LIST, FNAME_CONFIRM_LIST, DBACOUNT);
	DtSearchExit (2);
    }
    prog = toupper (argv[1][0]);
    if (prog != 'B' && prog != 'D')
	goto BAD_ARGS;

    /* Initialize defaults depending on program mode */
    if (prog == 'B') {
	infname = FNAME_DISCARD_DATA;
	outfname = FNAME_CONFIRM_LIST;
    }
    else {
	infname = FNAME_CONFIRM_LIST;
	outfname = PROGNAME "654";
    }
    maxtime = 0L;

    /* Save rest of command line arguments */
    for (argc -= 2, argv += 2; argc > 0; argc--, argv++) {
	arg = *argv;
	switch (tolower (arg[1])) {
	    case 'i':
		infname = arg + 2;
		break;

	    case 'o':
		outfname = arg + 2;
		break;

	    case 'd':
		debug_mode = TRUE;
		usrblk.debug |= USRDBG_DELETE;
		if (arg[2] == 'v')
		    usrblk.debug |= USRDBG_VERBOSE;
		break;

	    case 'y':
		yesarg = TRUE;
		break;

	    case 't':
		maxtime = atol (arg + 2);
		break;

	    case 'n':
		max_dbacount = atol (arg + 2);
		break;

	    default:
		fprintf (aa_stderr, catgets(dtsearch_catd, MS_tomita, 31,
		    "\n%s Unknown argument '%s'.\n") ,
		    PROGNAME"689", arg);
		goto BAD_ARGS;

	}	/* end switch */
    }	/* end arg parsing */

    /* Open input file to test for its existence.
     * For the Browse program, file ptr 'inf' == NULL
     * means the file is not open.
     */
    if ((inf = fopen (infname, "r")) == NULL) {
	if (prog == 'D') {
	    fprintf (aa_stderr, catgets(dtsearch_catd, MS_tomita, 32,
		"%s Unable to open input file '%s'.\n") ,
		PROGNAME"710", infname);
	    goto BAD_ARGS;
	}
    }

    /* If browsing, get output file name and
     * open it to test for write permission.
     */
    if (prog == 'B') {
	if ((outf = fopen (outfname, "a ")) == NULL)
	    /* the blank in "a " works around old aix bug */
	{
	    fprintf (aa_stderr,  catgets(dtsearch_catd, MS_tomita, 33,
		"\n%s Unable to open output file '%s'.\n") ,
		PROGNAME"721", outfname);
	    goto BAD_ARGS;
	}
    }

    /* Initialize the opera engine, i.e. open the database */
    printf ( catgets(dtsearch_catd, MS_tomita, 34,
	"Initializing %s engine...\n"),
	OE_prodname);
    strcpy (usrblk.userid, "ToMiTa");
    usrblk.request = OE_INITIALIZE;
    usrblk.query = AUSAPI_VERSION;
    Opera_Engine ();
    if (usrblk.retncode != OE_OK)
	retncode_abort (733);

    PRINT_MESSAGES
	if (prog == 'B')
	browser ();
    else
	deleter (infname);

    usrblk.request = OE_SHUTDOWN;
    Opera_Engine ();
    printf ( catgets(dtsearch_catd, MS_tomita, 36,
	"Normal engine shutdown.\n") );
    DtSearchExit (0);
}  /* main() */

/******************* TOMITA.C *******************/
