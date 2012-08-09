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
 *   FUNCTIONS: DtSearchGetKeytypes
 *		DtSearchHighlight
 *		DtSearchInit
 *		DtSearchQuery
 *		DtSearchRetrieve
 *		aa_categories
 *		aa_is_semantic_db
 *		aa_netrc
 *		aa_reinit
 *		ausapi_get_hirec
 *		ausapi_getexpire
 *		ausapi_getnews
 *		both_valid_dates
 *		build_dbnames_array
 *		pack_navstr
 *		signal_abort
 *		valid_dbname
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
/**************************** DTSRAPI.C *************************
 * $XConsortium: dtsrapi.c /main/9 1996/11/25 18:54:18 drk $
 * Nov 1993.  Originally called "calloe".
 * Implements a simple DtSearch/AusText API at a higher level than OE calls.
 * Substantially isolates caller from knowledge of usrblks,
 * hiding details of searches, possible return codes, etc.
 * Does its own mallocs and frees and controls the usrblk.
 *
 * Function descriptions (ie documentation) is in dtsearch.doc
 * and Search.h.
 *
 * Revision 2.8  1996/03/14  22:54:41  miker
 * Pass backdoor usrblk.debug into DtSearch.
 *
 * Revision 2.7  1996/02/01  18:09:24  miker
 * Deleted ausapi_kwic, ausapi_ping, ausapi_shutdown, signal(SIGUSR1).
 * DtSearchInit: usrblk.debug = high order 16 bits of init_switches arg.
 *
 * Revision 2.6  1995/12/27  16:08:22  miker
 * Added DtSearchReinit().
 *
 * Revision 2.5  1995/10/26  17:34:52  miker
 * Fixed duplicate messages catalog open.
 *
 * Revision 2.4  1995/10/25  21:39:45  miker
 * Renamed from ausapi.c.  Added prolog.
 *
 * Log: ausapi.c,v
 * Revision 2.3  1995/10/19  20:19:34  miker
 * Databases opened read-only unless DtSrInRDWR is specified.
 *
 * Revision 2.2  1995/10/02  20:51:32  miker
 * Bug: Tried to free unmalloced version string after Init (Takuki Kamiya).
 * Added symbolic msg catalog set numbers.
 * Fixed erroneous dbnames assignment.
 *
 * Revision 2.1  1995/09/22  18:48:45  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.20  1995/09/19  21:43:46  miker
 * Fixed minor bug in debugging statements only.
 *
 * Revision 1.19  1995/08/31  21:36:39  miker
 * Added #define to block out code not to be delivered to DtSearch.
 * Changed all msgslists to one global one at ausapi_msglist.
 * Moved function documentation to dtsearch.doc
 * Public functions renamed to match DtSearch conventions.
 * Rearranged some function args to eliminate public global variables.
 * Broke messages catalog in two so DtSearch can have its own.
 * Minor changes to accommodate portability.
 * Reenabled user to change name of site config file.
 *
 * Revision 1.18  1995/07/18  22:05:10  miker
 * Set OE_sitecnfg_fname to arg passed to ausapi_init().
 */
#include "SearchE.h"
#include <ctype.h>
#include <signal.h>
#include <locale.h>
#include <string.h>
#include <fcntl.h>

#define PROGNAME	"DTSRAPI"
#define SPRINTBUFSZ	1024
#define MS_misc		1
#define MS_ausapi	2

/*------------------- PRIVATE GLOBALS ---------------------*/
/* Usrblk should not be visible to user interface code,
 * but must be visible to real engine... */
USRBLK          usrblk = { 0 };
static int      save_search_type = '$';
static char    *sprintbuf = NULL;

/*------------------- EXTERNS (aajoint.c) ---------------------*/
extern int      aa_is_initialized;
extern void     aa_check_initialization (void);
extern long     save_init_switches;

#ifndef db_oflag
extern int	db_oflag;
#endif


/************************************************/
/*						*/
/*		  signal_abort			*/
/*						*/
/************************************************/
/* Interrupt handler for all common 'abort' signals.
 * Shuts down gracefully by ensuring database properly closed.
 * The database close and write to the audit log occur in OE.
 */
static void     signal_abort (int sig)
{
    fputs (DtSearchGetMessages (), aa_stderr);
    fprintf (aa_stderr, catgets (dtsearch_catd, MS_ausapi, 216,
	"\n%s %s Caught signal %d.\n"),
	PROGNAME"216",
	(aa_argv0) ? aa_argv0 : OE_prodname,
	sig);
    fflush (aa_stderr);
    DtSearchExit (100 + sig);
}  /* signal_abort() */


/************************************************/
/*						*/
/*		   valid_dbname			*/
/*						*/
/************************************************/
/* Sets usrblk.dblk to passed dbname.
 * Name must match dblk.label if it exists,
 * otherwise it must match dblk.name (same
 * algorithm that generates dbnamesv[]).
 * If dbname == NULL, sets dblk to first dblist node.
 * Returns TRUE on success.
 * Returns FALSE and err msg if invalid dbname.
 */
static int      valid_dbname (char *dbname)
{
    DBLK           *db;
    aa_check_initialization ();
    if (dbname == NULL) {
	usrblk.dblk = usrblk.dblist;
	return TRUE;
    }
    if (dbname[0] == 0) {
	usrblk.dblk = usrblk.dblist;
	return TRUE;
    }
    for (db = usrblk.dblist; db != NULL; db = db->link) {
	if (db->label) {
	    if (strcmp (dbname, db->label) == 0)
		break;
	}
	if (strcmp (dbname, db->name) == 0)
	    break;
    }
    if (db == NULL) {
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_ausapi, 1,
		"%1$sInvalid or unavailable database '%2$s'."),
	    PROGNAME "48 ", dbname);
	DtSearchAddMessage (sprintbuf);
	if (DtSrInANY_DEBUG & save_init_switches) {
	    fputs (PROGNAME "49 Available Databases:\n", aa_stderr);
	    for (db = usrblk.dblist; db != NULL; db = db->link)
		fprintf (aa_stderr, "  name='%s'\tlabel='%s'\n",
		    db->name, db->label);
	}
	return FALSE;
    }
    else
	usrblk.dblk = db;
    return TRUE;
}  /* valid_dbname() */


/************************************************/
/*						*/
/*	        build_dbnames_array		*/
/*						*/
/************************************************/
/* Builds array of database name string ptrs with same
 * architecture as command line arguments argv and argc
 * from the shell.  Uses dblk.label if it exists,
 * otherwise uses dblk.name.
 * Called at initialization and reinitialization.
 */
static void     build_dbnames_array (void)
{
    char           *targ, *src;
    DBLK           *db;
    int             i;
    size_t          mallocsz = 0L;

    if (ausapi_dbnamesv)
	free (ausapi_dbnamesv);
    ausapi_dbnamesc = 0;
    for (db = usrblk.dblist; db != NULL; db = db->link) {
	ausapi_dbnamesc++;
	mallocsz += ((db->label) ? strlen (db->label) : sizeof (db->name)) + 2L;
    }
    ausapi_dbnamesv = austext_malloc (
	mallocsz + (sizeof (char *) * (2 + ausapi_dbnamesc)) + 4,
	PROGNAME "106", NULL);
    /* The first part of the malloc is the array of pointers.
     * The strings start just after the array.  Set 'targ' to
     * the beginning of the strings, while 'i' indexes the ptrs.
     */
    targ = (char *) ausapi_dbnamesv + (sizeof (char *) * (1 + ausapi_dbnamesc));
    for (i = 0, db = usrblk.dblist; db != NULL; i++, db = db->link) {
	ausapi_dbnamesv[i] = targ;
	src = (db->label) ? db->label : db->name;
	while (*src != 0)
	    *targ++ = *src++;
	*targ++ = 0;

	if (save_init_switches & DtSrInIDEBUG) {
	    fprintf (aa_stderr, PROGNAME "490 "
		"dbnames[%d] = '%s': dname='%s', dlabel='%s'\n",
		i, ausapi_dbnamesv[i], db->name, db->label);
	    fflush (aa_stderr);
	}

    }
    ausapi_dbnamesv[i] = NULL;	/* terminate the list this way too */
    return;
}  /* build_dbnames_array() */


/************************************************/
/*						*/
/*		  aa_reinit			*/
/*						*/
/************************************************/
/* Performs all reinitialization functions required by OE_REINIT.
 * Always returns DtSrREINIT.
 */
static int      aa_reinit (void)
{
    clear_usrblk_record ();
    clear_hitwords ();
    usrblk.dba = 0;
    DtSearchFreeResults (&usrblk.dittolist);
    usrblk.dittocount = 0;
    build_dbnames_array ();
    return DtSrREINIT;
}  /* aa_reinit() */


/************************************************/
/*						*/
/*		  DtSearchInit			*/
/*						*/
/************************************************/
/* Initializes ausapi and the AusText engine (performs OE_INIT).
 * Must be first ausapi call.  Must be called only once (reinit?).
 * See dtsearch.doc for specs.
 */
int             DtSearchInit (
                    char	*argv0,
                    char	*userid,
                    long	switches,
                    char	*config_file,
                    FILE	*err_file,
                    char	***dbnames,
                    int		*dbcount)
{
    if (aa_is_initialized) {
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_ausapi, 621,
	    "%1$s %2%s has already been initialized."),
	    PROGNAME"621", PRODNAME);
	DtSearchAddMessage (sprintbuf);
	return DtSrFAIL;
    }

    aa_is_initialized = TRUE;
    save_init_switches = switches;
    if (argv0)
	aa_argv0 = argv0;
    if (err_file)
	aa_stderr = err_file;
#if defined(linux) || defined(CSRG_BASED)
    else
	aa_stderr = stderr;
#endif

    sprintbuf = austext_malloc (SPRINTBUFSZ, PROGNAME "135", NULL);

    /* Open msgs and help text catalogs. */
    if (switches & DtSrInNOLOCALE == 0) {
	setlocale (LC_ALL, "");
	dtsearch_catd = catopen (FNAME_DTSRCAT, 0);
    }

    /* Register AusText abort signal handlers.
     * This ensures that if caller is killed,
     * engine will shutdown gracefully.
     */
    if (switches & DtSrInSIGNAL) {
	if (!(switches & DtSrInENAB_NOHUP))
	    signal (SIGHUP, signal_abort);	/* trap hangups */
	signal (SIGINT, signal_abort);	/* interrupt, ctrl-c */
	signal (SIGQUIT, signal_abort);	/* quit, ctrl-d */
	signal (SIGKILL, signal_abort);	/* (kill -9, cannot be trapped) */
	signal (SIGTERM, signal_abort);	/* kill [-15], sfwr terminate */
#ifdef SIGPWR
	signal (SIGPWR, signal_abort);	/* power failure imminent */
#endif
#ifdef _AIX
	signal (SIGXCPU, signal_abort);	/* cpu time limit exceeded */
	signal (SIGDANGER, signal_abort);  /* imminent paging space crash */
#endif
    }

    /* If user name was not passed, get it from LOGNAME environment var */
    if (userid == NULL || *userid == 0)
	if ((userid = (char *) getenv ("LOGNAME")) == NULL) {
	    sprintf (sprintbuf, catgets (dtsearch_catd, MS_ausapi, 187,
		"%1$s Missing both userid and LOGNAME environment variable."),
		PROGNAME "187 ");
	    DtSearchAddMessage (sprintbuf);
	    return DtSrFAIL;
	}

    /* initialize usrblk fields */
    memset (&usrblk, 0, sizeof (USRBLK));
    strncpy (usrblk.userid, userid, 8);
    usrblk.userid[8] = 0;
    usrblk.flags |= USR_NO_INFOMSGS;	/* standard for ausapi */
    usrblk.flags |= USR_SORT_WHITL;	/* standard for ausapi */
    usrblk.flags |= USR_NO_ITERATE;	/* must ALWAYS be on in
					 * this api */

    if (switches & DtSrInIDEBUG)
	usrblk.debug |= USRDBG_RARE;
    if (switches & DtSrInSDEBUG)
	usrblk.debug |= USRDBG_SRCHCMPL;
    if (switches & DtSrInRDEBUG)
	usrblk.debug |= USRDBG_RETRVL;
    /* Secret unadvertised feature.  High order 2 bytes
     * are direct settings of lower 2 bytes of usrblk.debug.
     */
    usrblk.debug |= switches >> 16;

    /* Set vista's db_oflag from DtSrInRDWR switch.
     * Note the vista flag and the switch are inverses
     * of each other (RDWR vs. RDONLY).
     */
    if ((switches & DtSrInRDWR) == 0)
	db_oflag = O_RDONLY;

    /* Prespecify site config file to engine */
    if (config_file)
	OE_sitecnfg_fname = (char *) strdup (config_file);

    /* initialize AusText Engine */
    usrblk.request = OE_INITIALIZE;
    usrblk.query = AUSAPI_VERSION;
    Opera_Engine ();
    usrblk.query = NULL;  /* so we don't try to free it at query time */
    if (usrblk.retncode != OE_OK)
	return DtSrFAIL;

    build_dbnames_array ();
    if (dbnames) {
	*dbnames = ausapi_dbnamesv;
	if (!dbcount) {
	    sprintf (sprintbuf, catgets(dtsearch_catd, MS_ausapi, 7,
		"%s dbnames specified but not dbcount."),
		PROGNAME"304");
	    DtSearchAddMessage (sprintbuf);
	    return DtSrFAIL;
	}
	*dbcount = ausapi_dbnamesc;
    }

    return DtSrOK;
}  /* DtSearchInit() */


/************************************************/
/*						*/
/*	         DtSearchReinit			*/
/*						*/
/************************************************/
/* Returns pointer to dbnames array.
 * Used after database or config file changes to trigger
 * engine reinitialization and to reaccess the database names.
 */
int	DtSearchReinit (char ***dbnames, int *dbcount)
{
    aa_check_initialization();
    usrblk.request = OE_PING;
    Opera_Engine();
    switch (usrblk.retncode) {
	case OE_REINIT:
	    if (save_init_switches & DtSrInANY_DEBUG) {
		fputs (PROGNAME"755 "
		    "DtSearchReinit: Engine did return REINIT.\n",
		    aa_stderr);
		fflush (aa_stderr);
	    }
	    aa_reinit();
	    /*  fall thru to OK... */

	case OE_OK:
	    *dbnames = ausapi_dbnamesv;
	    *dbcount = ausapi_dbnamesc;
	    return DtSrOK;

	default:
	    return DtSrERROR;
    }
}  /* DtSearchReinit() */


/************************************************/
/*						*/
/*	       DtSearchGetKeytypes		*/
/*						*/
/************************************************/
/* Returns pointer to keytypes array of specified database.
 * If dbname == NULL, returns keytypes of first dblist node.
 * Caller may modify is_selected field but should
 * not alter other keytypes fields or pointers.
 */
int	DtSearchGetKeytypes (
		char	    *dbname,	/* 1 - 8 char database name */
		int	    *ktcount,	/* number entries in array */
		DtSrKeytype **keytypes)	/* array of database types */
{
    if (!valid_dbname (dbname))
	return DtSrREINIT;
    *ktcount = usrblk.dblk->ktcount;
    *keytypes = usrblk.dblk->keytypes;
    return DtSrOK;
}  /* DtSearchGetKeytypes() */


/************************************************/
/*						*/
/*		 both_valid_dates		*/
/*						*/
/************************************************/
/* Subroutine of DtSearchQuery().
 * Sets usrblk.objdate1 and objdate2 from passed date strings.
 * Returns TRUE on successful parse and conversion.
 * Returns FALSE and err msg if either date string is invalid.
 */
static int      both_valid_dates (char *date1, char *date2)
{
    int             convert_error = FALSE;

    if ((usrblk.objdate1 = DtSearchValidDateString (date1)) == -1L)
	convert_error = TRUE;

    if ((usrblk.objdate2 = DtSearchValidDateString (date2)) == -1L)
	convert_error = TRUE;

    if (!convert_error &&
	usrblk.objdate1 != 0L &&
	usrblk.objdate2 != 0L &&
	usrblk.objdate1 >= usrblk.objdate2) {
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_ausapi, 198,
		"%s 'Before' date is equal to or after 'After' date.\n"
		"  No records would be returned."),
	    PROGNAME "198");
	DtSearchAddMessage (sprintbuf);
	convert_error = TRUE;
    }

    return !convert_error;
}  /* both_valid_dates() */


/************************************************/
/*						*/
/*		  DtSearchQuery			*/
/*						*/
/************************************************/
/* Returns hitlist (dittolist) if search successful.
 * If dbname is NULL, assumes name from first dblk on dblist.
 * Will work correctly even if arg dittolist == &usrblk.dittolist.
 * Only return hits 'after' date1 and 'before' date2.
 * NULL in one date field means search not limited in that direction.
 * NULL in both date fields means search not restricted by dates.
 * NULL is also permitted in stems, but if stems is
 * not NULL, the passed stems array MUST be defined:
 *      char   stems [DtSrMAX_STEMCOUNT] [DtSrMAXWIDTH_HWORD].
 * If query is fzkeyi, it must point to
 * array of FZKEYSZ integers.
 * DtSearchQuery() was formerly named ausapi_search().
 */
int             DtSearchQuery (
		void	*qry,		/* query, fzkeyi, nav string */
		char	*dbname,	/* database name from dbnamesv */
		int	search_type,	/* 'P', 'W', 'S', 'T', 'Z', or 'N' */
		char	*date1,		/* "yyyy mm dd", 3 numeric tokens */
		char	*date2,		/* date1 earlier than date2 */
		DtSrResult
			**dittolist,	/* put hitlist here */
		long	*dittocount,	/* put num items on returned hitlist */
		char	*stems,		/* put stems array here */
		int	*stemcount)	/* put size of stems array */
{
    int             final_request;
    int             i, maxbkt;
    char           *ptr;
    DBLK           *db;
    LLIST          *llp;
    enum { TEXT, NAVSTRING, FZKEYI }
                    qryarg;
    char            date1str[24];

    if (!valid_dbname (dbname))
	return DtSrREINIT;

    /* Verify valid search_type and set flag
     * to tell us how to interpret 'qry' arg.
     */
    switch (search_type) {
	case 'P':	/* statistical search */
	    qryarg = TEXT;
	    final_request = OE_SRCH_STATISTICAL;
	    break;

	case 'W':	/* exact words search */
	    qryarg = TEXT;
	    final_request = OE_SRCH_WORDS;
	    break;

	case 'S':	/* exact stems search */
	    qryarg = TEXT;
	    final_request = OE_SRCH_STEMS;
	    break;

	default:
	    sprintf (sprintbuf, catgets (dtsearch_catd, MS_ausapi, 20,
		    "%1$s Invalid search_type '%2$c'."),
		PROGNAME "172 ", search_type);
	    DtSearchAddMessage (sprintbuf);
	    return DtSrERROR;

    }	/* end switch on search_type */

    /* Validate the 'qry' argument */
    if (qry == NULL) {
QUERY_ERROR:
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_ausapi, 30,
		"%s Null query.  No search performed."),
	    PROGNAME"81");
	DtSearchAddMessage (sprintbuf);
	return DtSrFAIL;
    }

    /* If qry is a char string, it must be nonempty. */
    if (qryarg != FZKEYI && ((char *) qry)[0] == 0)
	goto QUERY_ERROR;

    /* If qry is text to be sent as is to engine,
     * copy it over now to userblk.
     */
    if (qryarg == TEXT && qry != usrblk.query) {
	if (usrblk.query != NULL)
	    free (usrblk.query);
	usrblk.query = austext_malloc (strlen (qry) + 16, PROGNAME "102", NULL);
	strcpy (usrblk.query, qry);
    }

    /* Validate the 'date' args and place them into usrblk. */
    if (!both_valid_dates (date1, date2))
	return DtSrFAIL;

    /* Finish setup usrblk for final search */
    usrblk.flags |= USR_NO_ITERATE;	/* must ALWAYS be ON in this api */
    usrblk.request = final_request;
    db = usrblk.dblk;
    db->maxhits = aa_maxhits;
    if (qryarg == TEXT)
	save_search_type = usrblk.search_type = search_type;

    if (usrblk.debug & USRDBG_SRCHCMPL) {
	ptr = sprintbuf;
	for (i = 0; i < db->ktcount; i++) {
	    if (db->keytypes[i].is_selected)
		*ptr++ = '*';
	    *ptr++ = db->keytypes[i].ktchar;
	    *ptr++ = ' ';
	}
	*(--ptr) = 0;
	strcpy (date1str, objdate2fzkstr (usrblk.objdate1));
	fprintf (aa_stderr,
	    PROGNAME "353 DtSearchQuery(): dbname='%s' srchtype='%c'\n"
	    "  maxhits=%d keytypes='%s'\n"
	    "  date1='%s' -> %s.  date2='%s' -> %s.\n"
	    "  query='%.60s'\n",
	    db->name, search_type, db->maxhits, sprintbuf,
	    NULLORSTR (date1), date1str,
	    NULLORSTR (date2), objdate2fzkstr (usrblk.objdate2),
	    NULLORSTR (usrblk.query));
	fflush (aa_stderr);
    }

    /* Final engine call, the search itself... */
    Opera_Engine ();

END_OF_SEARCH:
    if (usrblk.debug & USRDBG_SRCHCMPL) {
	/* count msgs */
	i = 0;
	for (llp = ausapi_msglist; llp != NULL; llp = llp->link)
	    i++;
	fprintf (aa_stderr, PROGNAME "380 Return from Search:  "
	    "retncode=%d  hitcount=%ld  srchmsgs=%d.\n",
	    usrblk.retncode, usrblk.dittocount, i);
	fflush (aa_stderr);
    }

    /* Set this func's retn value based on engine's retncode, and return. */
    switch (usrblk.retncode) {
	case OE_OK:
	    /*
	     * Transfer usrblk.dittolist to user's own dittolist
	     * pointer. Also he MUST use DtSearchFreeResults()
	     * between calls. Otherwise the code below will cause a
	     * memory leak. 
	     */
	    *dittolist = usrblk.dittolist;
	    *dittocount = usrblk.dittocount;
	    usrblk.dittolist = NULL;
	    usrblk.dittocount = 0;
	    if (stems) {
		*stemcount = usrblk.stemcount;
		for (i = 0; i < *stemcount; i++) {
		    strcpy (stems, usrblk.stems[i]);
		    stems += DtSrMAXWIDTH_HWORD;
		}
	    }
	    return DtSrOK;

	case OE_NOTAVAIL:
	    return DtSrNOTAVAIL;

	case OE_ABORT:
	    return DtSrABORT;

	case OE_REINIT:
	    return aa_reinit ();

	case OE_BAD_QUERY:
	    /* Query was invalid.  Tell the user why. */
	    if (ausapi_msglist == NULL) {
		sprintf (sprintbuf,
		    catgets (dtsearch_catd, MS_ausapi, 806,
			"%s Query insufficient or search options "
			"incompatible with database '%s' to commence search."),
		    PROGNAME "806", usrblk.dblk->name);
		DtSearchAddMessage (sprintbuf);
	    }
	    return DtSrFAIL;

	case OE_NOOP:
	    /* Search was unsuccessful. Msgs should say why. */
	    return DtSrFAIL;

	default:
	    /*
	     * Includes OE_SEARCHING, OE_USER_STOP, etc. Probable
	     * program error.  Msgs may or may not say why. 
	     */
	    return DtSrERROR;
    }	/* end switch */
}  /* DtSearchQuery() */


/************************************************/
/*						*/
/*		 DtSearchRetrieve		*/
/*						*/
/************************************************/
/* Mallocs and returns cleartext of a record given database address (dba).
 * fzkey integers start at bkt #2, just like categories.
 * WARNING! USER SHOULD NEITHER MALLOC NOR FREE HIS CLEARTEXT POINTER!
 */
int             DtSearchRetrieve (
                    char *dbname,	/* 1 - 8 char database name */
                    DB_ADDR dba,	/* database address from dittolist */
                    char **cleartext,	/* cleartext put here (freed first) */
                    long *clearlen,	/* length of returned cleartext */
                    int *fzkeyi)	/* ptr to array of FZKEYSZ integers */
{
    int             i;

    if (!valid_dbname (dbname))
	return DtSrREINIT;
    usrblk.dba = dba;
    usrblk.request = OE_GETREC;
    Opera_Engine ();

    /* Set this func's retn value based on engine's retncode, and return. */
    switch (usrblk.retncode) {
	case OE_OK:
	    /*
	     * WARNING! USER SHOULD NEITHER MALLOC NOR FREE HIS
	     * CLEARTEXT POINTER. 
	     */
	    *cleartext = usrblk.cleartext;	/* user MUST leave
						 * cleartext alone */
	    *clearlen = usrblk.clearlen;
	    return DtSrOK;

	case OE_NOTAVAIL:
	    return DtSrNOTAVAIL;

	case OE_ABORT:
	    return DtSrABORT;

	case OE_REINIT:
	    return aa_reinit ();

	case OE_NOOP:
	    return DtSrFAIL;

	default:
	    /*
	     * Includes all unexpected return codes. Probable
	     * program error.  Msgs may or may not say why. 
	     */
	    return DtSrERROR;
    }	/* end switch */
}  /* DtSearchRetrieve() */



/************************************************/
/*						*/
/*		 DtSearchHighlight		*/
/*						*/
/************************************************/
/* Mallocs and returns hitwords array for passed text string
 * using database and stems array from last search.
 * The stems ptr may be NULL, but if specified
 * the passed stems array MUST be defined:
 *      char   stems [DtSrMAX_STEMCOUNT] [DtSrMAXWIDTH_HWORD].
 */
int             DtSearchHighlight (
                    char *dbname,	/* database name */
                    char *cleartext,	/* text to be hilited */
                    DtSrHitword ** hitwords,
					/* where to put hitwords array */
                    long *hitwcount,	/* num items in hitwords array */
                    int search_type,	/* [opt] override save_search_type */
                    char *stems,	/* [opt] override last search stems */
                    int stemcount	/* num stems in stems array */
)
{
    int             i;
    char           *cptr;

    if (!valid_dbname (dbname))
	return DtSrREINIT;

    /* copy cleartext to usrblk if necessary */
    if (cleartext == NULL || cleartext[0] == 0) {
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_ausapi, 40,
		"%1$s Null cleartext.  No highlighting performed."),
	    PROGNAME "349 ", dbname);
	DtSearchAddMessage (sprintbuf);
	return DtSrERROR;
    }
    if (cleartext != usrblk.cleartext) {
	if (usrblk.cleartext != NULL)
	    free (usrblk.cleartext);
	usrblk.clearlen = strlen (cleartext);
	usrblk.cleartext = austext_malloc (usrblk.clearlen + 16,
	    PROGNAME "267", NULL);
	strcpy (usrblk.cleartext, cleartext);
    }

    if (search_type)
	usrblk.search_type = search_type;
    else
	usrblk.search_type = save_search_type;

    if (stems) {
	if (stemcount > DtSrMAX_STEMCOUNT) {
	    sprintf (sprintbuf, catgets (dtsearch_catd, MS_ausapi, 1072,
		"%s Program Error: Stem count (%d) greater than maximum (%d)."),
		PROGNAME"1072", stemcount, DtSrMAX_STEMCOUNT);
	    DtSearchAddMessage (sprintbuf);
	    return DtSrERROR;
	}
	usrblk.stemcount = stemcount;
	cptr = stems;
	for (i = 0; i < stemcount; i++) {
	    strcpy (usrblk.stems[i], cptr);
	    cptr += DtSrMAXWIDTH_HWORD;
	}
    }

    usrblk.request = OE_HILITE_STEMS;
    Opera_Engine ();

    /* Set this func's retn value based on engine's retncode, and return. */
    switch (usrblk.retncode) {
	case OE_OK:
	    /*
	     * Note that the following assignment works even if the
	     * user's hitwords arg is in fact &usrblk.hitwords and
	     * hitwcount is in fact &usrblk.hitwcount. 
	     */
	    *hitwords = usrblk.hitwords;
	    *hitwcount = usrblk.hitwcount;
	    return DtSrOK;

	case OE_ABORT:
	    return DtSrABORT;

	case OE_REINIT:
	    return aa_reinit ();

	case OE_NOOP:
	case OE_NOTAVAIL:
	    return DtSrFAIL;

	default:
	    /*
	     * Includes OE_BAD_QUERY and all unexpected return
	     * codes. Probable program error.  Msgs may or may not
	     * say why. In case they don't, put OE's retncode on
	     * msglist. 
	     */
	    if (!ausapi_msglist) {
		sprintf (sprintbuf, catgets (dtsearch_catd, MS_ausapi, 1342,
		    "%s Search Engine Error %d for highlight request for "
		    "database '%s', hit word count=%ld, search type='%c', text='%.30s'"),
		    PROGNAME "1342",
		    usrblk.retncode, dbname, *hitwcount, search_type,
		    NULLORSTR (cleartext));
		DtSearchAddMessage (sprintbuf);
	    }
	    return DtSrERROR;
    }	/* end switch */
}  /* DtSearchHighlight() */

/**************************** DTSRAPI.C *************************/
