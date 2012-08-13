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
 *   FUNCTIONS: Opera_Engine
 *		alarm_signal_handler
 *		expired
 *		no_keytypes
 *		oe_unblob
 *		oe_write_audit_rec
 *		request_str
 *		retncode_str
 *		save_query
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1991,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/******************************* DTOE.C ********************************
 * $XConsortium: dtoe.c /main/6 1996/11/25 18:52:51 drk $
 * Sept 1991.
 * Universal Opera Engine code.
 * Additional functions in modules named OE...
 * See comments in OE.H for Opera_Engine() function descriptions.
 * References to 'socblk' have all been replaced by usrblk,
 * which is now the universal data structure.
 *
 * $Log$
 * Revision 2.8  1996/03/20  19:25:31  miker
 * Use new hilite_cleartext() function call.
 *
 * Revision 2.7  1996/03/13  22:51:39  miker
 * Changed char to UCHAR several places.
 * Revision 2.6  1996/03/05  19:20:58  miker
 * Replaced vewords with boolyac, boolpars, and boolsrch.
 * oe_unblob no longer converts to uppercase.
 * Revision 2.5  1996/02/01  17:15:18  miker
 * 2.1.11:  Changes to support parsers using readchar cofunctions.
 * Changed hiliting calls to hilite_cleartext.
 * Obsoleted OE_FINDSTR_REC, OE_DITTO2KWIC, OE_FINDSTR_HITL.
 * Revision 2.4  1995/12/27  16:47:12  miker
 * Prolog update.
 * Revision 2.3  1995/10/24  22:31:32  miker
 * Renamed from oe.c.  Added prolog.
 * Log: oe.c,v
 * Revision 2.2  1995/10/03  21:44:24  miker
 * Deleted unsigned attrib from misc variables for portability.
 * Revision 2.1  1995/09/22  21:29:45  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 * Revision 1.19  1995/09/05  18:49:45  miker
 * Changed all socblk refs to usrblk.  Obsoleted several globals.
 * Conflated all msglists to one ausapi_msglist.  Numerous name changes.
 * Added DTSEARCH define.  Remove password processing.
 * Made usrblk a universal global.  ...All for DtSearch.
 * Revision 1.18  1995/07/19  21:02:59  miker
 * 2.1.6c: Removed OE_mail_feature and OE_print_server.
 * Revision 1.17  1995/06/22  20:49:33  miker
 * 2.1.6: Additional debugging messages.
 * Revision 1.16  1995/05/30  19:20:51  miker
 * Print a little more of msglist when debugging engine return.
 */
#include "SearchE.h"
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdlib.h>

#define PROGNAME	"DTOE"
#define MAX_LASTQRY	64
#define QRYBUFSZ	1024
#define MS_misc		1
#define MS_oe		10
/*******#define DUMP_HITWORDS*******/

typedef struct {
    int             num;
    char           *str;
}               NUMSTR;

int  boolean_parse (void);
void boolean_search (void);
void ve_delete (void);

/*------------------ OPERA ENGINE GLOBALS --------------------
 * Default values set by init_globals() in oeinit.c
 * (Some values preinitialized here by compiler because 
 * they may be used before the first call to init_globals()).
 * Most can be overridden by site configuration file.
 * Obviously any changes here should be reflected in init_globals().
 * Other OE_... globals are located in loadocf.c
 */
extern int	debugging_jpn;
extern int	debugging_teskey;
char           *global_memory_ptr = NULL;	/* shared mem, dynam
						 * defrag */
int             shm_id = 0;	/* shared mem, dynam defrag */

int             OE_bmhtab_strlen[DtSrMAX_STEMCOUNT] = { 0 };
size_t          OE_bmhtables[DtSrMAX_STEMCOUNT][MAX_BMHTAB] = { { 0 } };
int             OE_dbn = 0;	/* dynamic */
int             OE_enable_markdel = 0;
int             OE_enable_usernotes = 0;
int             OE_fastdecode = 0;
char           *OE_fileio = NULL;
long            OE_flags = 0L;
long            OE_objsize = 0L;
char           *OE_prodname = PRODNAME;	/* reset only in main() */
float           OE_prox_factor = 0.0;
int             OE_search_type = 0;
char           *OE_sitecnfg_fname = NULL;
time_t          OE_sitecnfg_mtime = 0L;	/* reset only in oeinitialize() */
int             OE_uppercase_keys = 0;
long            OE_words_hitlimit = 0L;

static time_t   my_expiration = 0L;
time_t         *OE_expiration = &my_expiration;

char           *OEF_audit = NULL;
char           *OEF_discard = NULL;
char           *OEF_news = NULL;
char           *OEF_notesnot = NULL;
char           *OEF_notessem = NULL;
char           *OEF_readme = NULL;


/*------------ OTHER GLOBALS -----------*/
SAVEUSR         saveusr = { 0 };
static DtSrHitword
		next_hitwords [DtSrMAX_STEMCOUNT];


/****************************************/
/*					*/
/*		expired			*/
/*					*/
/****************************************/
/* This function permanently disables opera 
 * if the license to opera has expired.
 */
static void     expired (char *sprintbuf)
{
    sprintf (sprintbuf, catgets (dtsearch_catd, MS_oe, 71,
	PROGNAME "71 %s has expired."),
	OE_prodname);
    DtSearchAddMessage (sprintbuf);
    OE_flags |= OE_PERMERR;
    usrblk.retncode = OE_ABORT;
    return;
}  /* expired() */


/************************************************/
/*                                              */
/*            alarm_signal_handler              */
/*                                              */
/************************************************/
/* Interrupt handler for SIGALRM */
static void     alarm_signal_handler (int sig)
{
    fprintf (aa_stderr, PROGNAME "32 "
	"%s %s shutdown due to excessive user idle time.\n",
	nowstring (NULL), aa_argv0);
    DtSearchExit (100 + sig);
}  /* alarm_signal_handler() */


/****************************************/
/*					*/
/*		 oe_unblob		*/
/*					*/
/****************************************/
/* Converts a list of compressed text blob records
 * straight out of vista into a single string of clear text.
 * input = OE_objsize, dblk.hufid, passed bloblist (freed after use!).
 * output = usrblk.cleartext, usrblk.clearlen.
 * Returns OE_OK if all goes well, else returns other appropriate retncode.
 */
int	oe_unblob (LLIST *bloblist)
{
    UCHAR	*targ, *src, *stoploc;
    short	blobclearlen;
    LLIST	*lptr, *nextlptr;
    long	mallocsz;
    struct or_blobrec
		*bptr;

    /*  Free previous cleartext, if any, and allocate new buffer */
    if (OE_objsize < 512)
	mallocsz = 512L;
    else
	mallocsz = OE_objsize + 4L;
    if (usrblk.cleartext != NULL)
	free (usrblk.cleartext);
    usrblk.cleartext = austext_malloc (mallocsz, PROGNAME "188", NULL);
    usrblk.clearlen = OE_objsize;

    /* Uncompress/decipher bloblist into cleartext,
     * freeing the blobs as we go.
     */
    targ = (UCHAR *) usrblk.cleartext;
    stoploc = targ + OE_objsize;
    lptr = bloblist;
    while (lptr != NULL) {
	/* Setup ptrs and counters for decoding */
	bptr = (struct or_blobrec *) lptr->data;
	src = (UCHAR *) bptr->or_blob;
	blobclearlen = bptr->or_bloblen;  /* len of cleartext in curr blob */
	if (targ + blobclearlen > stoploc) {
	    DtSearchAddMessage (PROGNAME "242 Logical Error in database. "
		"Object larger than stored size.");
	    free (usrblk.cleartext);
	    usrblk.clearlen = 0;
	    usrblk.retncode = OE_ABORT;
	    OE_flags |= OE_PERMERR;
	    return OE_ABORT;
	}

	/* Decode into clear text buffer */
	hc_decode (src, targ, blobclearlen, usrblk.dblk->dbrec.or_hufid);

	targ += blobclearlen;

	/* free current blob, advance to next blob */
	nextlptr = lptr->link;	/* temp save next blob addr */
	free (lptr);
	lptr = nextlptr;
    }
    *targ = 0;

    if (usrblk.debug & USRDBG_RETRVL)
	fprintf (aa_stderr, PROGNAME "256 "
	    "oe_unblob: actual decompressed length = %ld.\n",
	    targ - (UCHAR *) usrblk.cleartext);
    return OE_OK;
}  /* oe_unblob() */


/************************************************/
/*						*/
/*		  save_query			*/
/*						*/
/************************************************/
/* If AUDIT switch is turned on, saves query and start_time in saveusr 
 * for later printing when search process has completed.
 */
static void     save_query (char *prefix, time_t start_time)
{
    char           *src, *targ, *end;

    if (saveusr.lastqry != NULL)
	free (saveusr.lastqry);
    saveusr.lastqry = austext_malloc (MAX_LASTQRY, PROGNAME "500", NULL);

    /* First copy prefix and "=" */
    targ = saveusr.lastqry;
    src = prefix;
    while (*src != 0)
	*targ++ = *src++;
    *targ++ = '=';

    /* Copy query after '=', replacing any ctrl chars
     * with a displayable funny character (tilde ~).
     */
    if (usrblk.query == NULL)
	strcpy (targ, catgets (dtsearch_catd, MS_misc, 1, "<null>"));
    else {
	end = saveusr.lastqry + MAX_LASTQRY - 2;
	src = usrblk.query;
	while (*src != 0 && targ < end) {
	    *targ = *src++;
	    if (*targ < 32)
		*targ = '~';
	    targ++;
	}
	*targ = 0;
    }
    saveusr.start_time = start_time;
    return;
}  /* save_query() */


/************************************************/
/*						*/
/*	        oe_write_audit_rec		*/
/*						*/
/************************************************/
/* Writes out audit data after uninterrupted completion of a search.
 * Caller checks if AUDIT flag is on.
 * Argument is number of hits (may or may not = dittocount).
 * By convention, numhits = -1 means search was canceled by user,
 * numhits = -2 means system canceled search.
 * Requires various saveusr and usrblk fields to be correct.
 */
void            oe_write_audit_rec (long numhits)
{
    char            sprintbuf[1024];
    time_t          now_gmt;
    FILE           *stream;

    if ((stream = fopen (OEF_audit, "a ")) == NULL)
	/* the blank in "a " works around old aix bug */
    {
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_misc, 1596,
		PROGNAME "1596 Cannot open audit file %s: %s"),
	    OEF_audit, strerror (errno));
	DtSearchAddMessage (sprintbuf);
	OE_flags &= ~OE_AUDIT;	/* don't try to audit anything else */
    }
    else {
	time (&now_gmt);
	fprintf (stream, AUDIT_FORMAT "%s\n",
	    usrblk.userid,
	    nowstring (&now_gmt),
	    now_gmt - saveusr.start_time,	/* elapsed search time */
	    usrblk.dblk->name,
	    numhits,
	    (saveusr.lastqry == NULL) ? \
	    catgets (dtsearch_catd, MS_misc, 1, "<null>") : saveusr.lastqry);
	if (saveusr.lastqry != NULL) {
	    free (saveusr.lastqry);
	    saveusr.lastqry = NULL;
	}
	fclose (stream);
    }
    return;
}  /* oe_write_audit_rec() */


/************************************************/
/*						*/
/*		   no_keytypes			*/
/*						*/
/************************************************/
/* Returns FALSE if any keytype in usrblk.dblk is_selected.
 * Otherwise appends an error msg, sets usrblk.retncode
 * to OE_BAD_QUERY, and returns TRUE.
 */
static int      no_keytypes (void)
{
    int             i = usrblk.dblk->ktcount;
    char            sprintbuf[256];

    DtSrKeytype    *keytypes = usrblk.dblk->keytypes;
    while (--i >= 0)
	if (keytypes[i].is_selected)
	    return FALSE;
    sprintf (sprintbuf, catgets (dtsearch_catd, MS_oe, 440,
	    PROGNAME "440 No record keytypes were selected in database '%s'."),
	usrblk.dblk->label);
    DtSearchAddMessage (sprintbuf);
    usrblk.retncode = OE_BAD_QUERY;
    return TRUE;
}  /* no_keytypes() */


/************************************************/
/*						*/
/*		  request_str			*/
/*						*/
/************************************************/
/* Returns string identifier for OE_... request numbers for debugging */
static char    *request_str (int reqnum)
{
    static NUMSTR	numstr[] = {
	{OE_INITIALIZE,		"INITIALIZE"},		/*  1 */
	{OE_TEXT2FZKEY,		"TEXT2FZKEY"},		/*  2 */
	{OE_SRCH_FZKEY,		"SRCH_FZKEY"},		/*  3 */
	{OE_SRCH_STEMS,		"SRCH_STEMS"},		/*  4 */
	{OE_SRCH_WORDS,		"SRCH_WORDS"},		/*  5 */
	{OE_STOP_SRCH,		"STOP_SRCH"},		/*  6 */
	{OE_APPEND_NOTES,	"APPEND_NOTES"},	/*  7 */
	{OE_GETREC,		"GETREC"},		/*  8 */
	{OE_GETREC_STEMS,	"GETREC_STEMS"},	/*  9 */
	{OE_GETREC_WORDS,	"GETREC_WORDS"},	/* 10 */
	{OE_NEXT_DBA,		"NEXT_DBA"},		/* 11 */
	{OE_PREV_DBA,		"PREV_DBA"},		/* 12 */
	{OE_RECKEY2DBA,		"RECKEY2DBA"},		/* 13 */
	{OE_MARK_DELETION,	"MARK_DELETION"},	/* 14 */
	{OE_GETREC_DIC,		"GETREC_DIC"},		/* 15 */
	{OE_DITTO2KWIC,		"DITTO2KWIC"},		/* 16 */
	{OE_VALIDATE_PWD,	"VALIDATE_PWD"},	/* 17 */
	{OE_CHANGE_PWD,		"CHANGE_PWD"},		/* 18 */
	{OE_DELETE_RECID,	"DELETE_RECID"},	/* 19 */
	{OE_DELETE_BATCH,	"DELETE_BATCH"},	/* 20 */
	{OE_ASSIST,		"ASSIST"},		/* 21 */
	{OE_FINDSTR_REC,	"FINDSTR_REC"},		/* 22 */
	{OE_FINDSTR_HITL,	"FINDSTR_HITL"},	/* 23 */
	{OE_SRCH_STATISTICAL,	"SRCH_STATISTICAL"},	/* 24 */
	{OE_HILITE_STEMS,	"HILITE_STEMS"},	/* 25 */
	{OE_GET_EXPIRE,		"GET_EXPIRE"},		/* 26 */
	{OE_KILL,		"KILL"},		/* 9997 */
	{OE_PING,		"PING"},		/* 9998 */
	{OE_SHUTDOWN,		"SHUTDOWN"},		/* 9999 */
	{0,			"<unknown>"}
	};
    NUMSTR		*ptr = numstr;

    while (reqnum != ptr->num  &&  ptr->num != 0)
	ptr++;
    return ptr->str;
} /* request_str() */


/************************************************/
/*						*/
/*		  retncode_str			*/
/*						*/
/************************************************/
/* Returns string identifier for OE_... retncode numbers for debugging */
char	*retncode_str (int num)
{
    static char		buf [16];
    static NUMSTR	numstr[] = {
	{OE_OK,		"OE_OK"},		/*  1 */
	{OE_REINIT,	"OE_REINIT"},		/*  2 */
	{OE_SEARCHING,	"OE_SEARCHING"},	/*  3 */
	{OE_BAD_DBLK,	"OE_BAD_DBLK"},		/*  4 */
	{OE_BAD_REQUEST,"OE_BAD_REQUEST"},	/*  5 */
	{OE_BAD_QUERY,	"OE_BAD_QUERY"},	/*  6 */
	{OE_NOTAVAIL,	"OE_NOTAVAIL"},		/*  7 */
	{OE_TIMEOUT,	"OE_TIMEOUT"},		/*  8 */
	{OE_WRAPPED,	"OE_WRAPPED"},		/*  9 */
	{OE_SYSTEM_STOP,"OE_SYSTEM_STOP"},	/*  10 */
	{OE_BAD_PASSWD,	"OE_BAD_PASSWD"},	/*  11 */
	{OE_BAD_HITLIST,"OE_BAD_HITLIST"},	/*  12 */
	{OE_DISABLED,	"OE_DISABLED"},		/*  13 */
	{OE_USER_STOP,	"OE_USER_STOP"},	/*  14 */
	{OE_BAD_COMM,	"OE_BAD_COMM"},		/*  15 */
	{OE_NOOP,	"OE_NOOP"},		/*  888 */
	{OE_ABORT,	"OE_ABORT"},		/*  999 */
	{0,		buf}
	};
    NUMSTR		*ptr = numstr;

    while (num != ptr->num  &&  ptr->num != 0)
        ptr++;
    if (ptr->num == 0)
	sprintf (buf, "%d(?)", num);
    return ptr->str;
} /* retncode_str() */


/************************************************/
/*						*/
/*		  Opera_Engine			*/
/*						*/
/************************************************/
void            Opera_Engine (void)
{
    int		i, len, ws_flag;
    char	sprintbuf [1024];
    char	*ptr, *ptr2;
    LLIST	*bloblist;
    LLIST	*llp;
    FILE	*stream;
    DBLK	*db;
    PARG	parg;
    DB_ADDR	dba;
    static int	first_idletime_call = TRUE;
    static time_t
		start_time = 0L;

    extern int      database_has_changed (void);

    time (&start_time);	/* time that current call began */

    if (usrblk.debug != 0L) {
	if ((usrblk.debug & USRDBG_PARSE) != 0) {
	    debugging_jpn = TRUE;
	    debugging_teskey = TRUE;
	}
	/*
	 * Place strings for 3 interesting time stamps at sprintbuf
	 * +0, +100, and +200. 
	 */
	strcpy (sprintbuf, nowstring (&start_time));
	if (*OE_expiration != 0L)
	    strcpy (sprintbuf + 100, nowstring (OE_expiration));
	else
	    strcpy (sprintbuf + 100, "0");
	if (OE_sitecnfg_mtime != 0)
	    strcpy (sprintbuf + 200, nowstring (&OE_sitecnfg_mtime));
	else
	    strcpy (sprintbuf + 200, "0");
	fprintf (aa_stderr,
	    "\n" PROGNAME "444  Opera_Engine Request %d (%s) at %s.\n"
	    "  user='%s', usrblk.flags=%ld(x%04lx), usrblk.debug=%ld(x%04lx).\n"
	    "  OE_flags=%ld(x%04lx), exp=%s, sitecnfg=%s.\n"
	    ,usrblk.request, request_str (usrblk.request), sprintbuf
	    ,usrblk.userid, usrblk.flags, usrblk.flags
	    ,usrblk.debug, usrblk.debug
	    ,OE_flags, OE_flags, sprintbuf + 100, sprintbuf + 200
	    );
	i = 0;
	for (db = usrblk.dblist; db != NULL; db = db->link) {
	    if (db == usrblk.dblk)
		break;
	    i++;
	}
	if (db == NULL)
	    fprintf (aa_stderr, "  dblk is %s\n",
		(i) ? "INVALID!" : "null.");
	else
	    fprintf (aa_stderr, "  dblk #%d: name='%s', vistano=%d.\n"
		,i, usrblk.dblk->name, usrblk.dblk->vistano
		);
	fflush (aa_stderr);
    }

    /* Check if this copy of opera has expired.
     * If *OE_expiration == 0, expiration checking disabled.
     * If current time < expiration time, permit request.
     * Otherwise disable this and all further requests. 
     */

    if (*OE_expiration != 0L)
	if (start_time > *OE_expiration)
	    expired (sprintbuf);

    if (OE_flags & OE_PERMERR) {
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_oe, 490,
		PROGNAME "490 %s Engine permanently disabled."), OE_prodname);
	DtSearchAddMessage (sprintbuf);
	usrblk.retncode = OE_ABORT;
	goto ENGINE_RETURN;
    }

    /* Ensure that the first call is always an OE_INITIALIZE call */
    if ((usrblk.request != OE_INITIALIZE) && !(OE_flags & OE_INITOK)) {
	DtSearchAddMessage (catgets (dtsearch_catd, MS_oe, 523,
		PROGNAME "523 Request Denied: First request must "
		"be Engine Initialization."));
	usrblk.retncode = OE_NOOP;
	goto ENGINE_RETURN;
    }

    /* Verify that none of the databases has changed since the last call.  */
    if (database_has_changed ())
	goto ENGINE_RETURN;

    /* Make usrblk ready.  Basically ensure client side
     * has not destroyed usrblk.  Set OE_dbn to match user's dblk.
     * The make-ready activity is not called for OE_INITIALIZE
     * because the site's config file has not yet been called
     * to create the OE's dblks.
     */
    if (usrblk.request != OE_INITIALIZE) {
	/* Set OE_dbn to match selected dblk */
	for (	db = usrblk.dblist,  OE_dbn = 0;
		db != NULL;
		db = db->link,  OE_dbn++)
	    if (strcmp (usrblk.dblk->name, db->name) == 0)
		break;
	if (db == NULL) {
	    sprintf (sprintbuf, catgets (dtsearch_catd, MS_oe, 48,
		    PROGNAME "48 Request Aborted: "
		    "'%s' database not available at this site."),
		usrblk.dblk->name);
	    DtSearchAddMessage (sprintbuf);
	    usrblk.retncode = OE_ABORT;
	    OE_flags |= OE_PERMERR;
	    goto ENGINE_RETURN;
	}
	/* Override user switches if required by the engine */
	if ((OE_flags & OE_NO_ITERATE) != 0)
	    usrblk.flags |= USR_NO_ITERATE;
    }

/*----------------- BIG SWITCH ON REQUEST CODE ------------------*/
    switch (usrblk.request) {
	case OE_SRCH_STEMS:
	case OE_SRCH_WORDS:
	    /*
	     * Builds dittolist from query of stems/words +
	     * booleans. Swap stoplists to fool search functions. 
	     */
	    if (no_keytypes ())
		break;
	    usrblk.search_type =
		(usrblk.request == OE_SRCH_WORDS)? 'W' : 'S';
	    if (OE_flags & OE_AUDIT)
		save_query ("WORDS", start_time);
	    /****ve_word_search ();*****/
	    if (!boolean_parse()) {
		usrblk.retncode = OE_BAD_QUERY;
		break;
	    }
	    boolean_search();
	    if (usrblk.debug & USRDBG_SRCHCMPL)
		print_stems (usrblk.stemcount, usrblk.stems, PROGNAME"637");
	    if (usrblk.debug & (USRDBG_SRCHCMPL | USRDBG_HITLIST))
		print_dittolist (usrblk.dittolist, PROGNAME "657");
	    break;

	case OE_SRCH_STATISTICAL:
	    /*
	     * Builds dittolist from query string of natural
	     * language text whose words cause statistical doc
	     * retrieval. 
	     */
	    if (no_keytypes ())
		break;
	    if (OE_flags & OE_AUDIT)
		save_query ("STAT", start_time);
	    ve_statistical ();
	    if (usrblk.debug & USRDBG_SRCHCMPL)
		print_stems (usrblk.stemcount, usrblk.stems,
		    PROGNAME"770 SRCH_STATISTICAL:");
	    if (usrblk.debug & (USRDBG_SRCHCMPL | USRDBG_HITLIST))
		print_dittolist (usrblk.dittolist, PROGNAME "772");
	    break;

	case OE_HILITE_STEMS:
	    if (usrblk.debug & (USRDBG_RETRVL | USRDBG_HILITE)) {
		fprintf (aa_stderr, PROGNAME "819 HILITE_STEMS: "
		    "srchtyp=%c clrln=%ld clrtxt='%.30s'\n",
		    usrblk.search_type, usrblk.clearlen,
		    NULLORSTR (usrblk.cleartext));
		print_stems (usrblk.stemcount, usrblk.stems, " ");
	    }
	    if (usrblk.cleartext == NULL  ||  usrblk.clearlen <= 0) {
NO_TEXT:
		DtSearchAddMessage (
		    PROGNAME "839 Client Error: No Text to highlight.");
		usrblk.retncode = OE_BAD_QUERY;
		break;
	    }
	    if (usrblk.cleartext[0] == '\0')
		goto NO_TEXT;
	    if (usrblk.stemcount <= 0) {
		DtSearchAddMessage (PROGNAME "846 Client Error: "
		    "Cannot highlight words, stemcount is zero.");
		usrblk.retncode = OE_BAD_QUERY;
		break;
	    }
	    /* Subswitch: depending on request, load hitwords array */
	    switch (usrblk.search_type) {
		case 'W':
		    i = 'W';
		    break;
		case 'S':
		case 'P':
		    i = 'S';
		    break;
		default:
		    i = 0;
		    DtSearchAddMessage (
			PROGNAME "708 Word Highlighting is not available "
			"for semantic searches.");
		    usrblk.retncode = OE_BAD_QUERY;
		    break;
	    }
	    if (!i)
		break;
	    hilite_cleartext (i, (char *) usrblk.stems, usrblk.stemcount);
	    if (OE_flags & OE_PERMERR)
		usrblk.retncode = OE_ABORT;
	    else
		usrblk.retncode = OE_OK;
	    if (usrblk.debug & USRDBG_RETRVL) {
		fprintf (aa_stderr,
		    PROGNAME "820 HILITE_STEMS: hitwcount=%ld\n",
		    usrblk.hitwcount);
	    }
	    break;	/* end OE_HILITE_STEMS */


	case OE_STOP_SRCH:
	    /* interrupts long, ongoing search of database */
	    usrblk.flags |= USR_STOPSRCH;
	    usrblk.retncode = OE_OK;
	    break;


	case OE_GETREC:
	case OE_GETREC_WORDS:
	case OE_GETREC_STEMS:
	case OE_GETREC_DIC:
	    /*
	     * Retrieve database record, text blobs, notes, etc.
	     * Note that ve_getrec_dba() may return OE_OK even if
	     * there are no blobs.  That will happen when it can
	     * return everything else about the record, but by
	     * design the text itself is not stored in the
	     * repository. In other words, no blobs is not an error
	     * so check if blobs were returned and adjust retncode
	     * accordingly. 
	     */
	    usrblk.retncode = ve_getrec_dba (&bloblist);
	    if (usrblk.retncode != OE_OK)
		break;
	    /*
	     * If no text blobs, ensure cleartext and hitwords
	     * array are empty (which will cause OE_NOTAVAIL return
	     * below). Don't create a "no text" msg because many
	     * clients will want to retrieve the record text
	     * locally and they won't want an "error" msg
	     * cluttering up the msglist. 
	     */
	    if (bloblist == NULL) {
		usrblk.clearlen = 0;
		if (usrblk.cleartext != NULL) {
		    free (usrblk.cleartext);
		    usrblk.cleartext = NULL;
		}
		clear_hitwords ();
	    }
	    /*
	     * Otherwise if text blobs do exist, convert them into
	     * clear text and a hitwords array for hiliting. 
	     */
	    else {
		usrblk.retncode = oe_unblob (bloblist);
		if (usrblk.retncode != OE_OK)
		    break;

		/* Subswitch: depending on request, load hitwords array */
		switch (usrblk.request) {
		    case OE_GETREC_WORDS:
			hilite_cleartext ('W',
			    (char*) usrblk.stems, usrblk.stemcount);
			break;

		    case OE_GETREC_STEMS:
			hilite_cleartext ('S',
			    (char*) usrblk.stems, usrblk.stemcount);
			break;

		    case OE_GETREC_DIC:
			DtSearchAddMessage (PROGNAME "783 "
			    "Dictionary word hiliting function "
			    "is no longer supported.");
			clear_hitwords ();
			break;

		    case OE_GETREC:
		    default:
			clear_hitwords ();	/* ensure no hiliting */
		}	/* end hitwords subswitch */

#ifdef DUMP_HITWORDS	/* dump the stems and hitwords arrays */
		print_stems (usrblk.stemcount, usrblk.stems, "\nSTEMS:");
		fprintf (aa_stderr, "HITWORDS: hitwcount = %d\n",
		    usrblk.hitwcount);
		for (i = 0; i < usrblk.hitwcount; i++)
		    fprintf (aa_stderr, "offset = %4ld, len=%2d, '%.10s...'\n",
			usrblk.hitwords[i].offset,
			usrblk.hitwords[i].length,
			usrblk.cleartext + usrblk.hitwords[i].offset);
#endif
	    }	/* end if where blobs exist */

	    if (OE_flags & OE_PERMERR)
		usrblk.retncode = OE_ABORT;
	    else {
		usrblk.retncode = ((usrblk.clearlen) ? OE_OK : OE_NOTAVAIL);
		if (usrblk.debug & USRDBG_RETRVL)
		    print_usrblk_record (PROGNAME "957 final: ");
	    }
	    break;	/* end cases OE_GETREC... */


	case OE_NEXT_DBA:
	    /*
	     * increments dba address field (not associated with
	     * hitlist) 
	     */
	    ve_browse_dba (+1);
	    break;

	case OE_PREV_DBA:
	    /*
	     * decrements dba address field (not associated with
	     * hitlist) 
	     */
	    ve_browse_dba (-1);
	    break;

	case OE_APPEND_NOTES:
	    /* appends user notes to record at dba */
	    usrblk.retncode = ve_append_notes ();
	    break;

	case OE_RECKEY2DBA:
	    /* converts vista record key to database address */
	    usrblk.dba = ve_reckey2dba ();
	    if ((usrblk.debug & USRDBG_DELETE) != 0L)
		fprintf (aa_stderr, PROGNAME "1089 RECKEY2DBA: "
		    "retncode=%d, reckey='%s' ->\tdba=%ld:%ld\n",
		    usrblk.retncode, usrblk.query,
		    (usrblk.dba) >> 24, (usrblk.dba) & 0xffffff);
	    break;

	case OE_DELETE_RECID:
	    /*
	     * First converts recid to a db address.  Then deletes
	     * entire record: blobs, notes, and words. Presumes
	     * only 2 retncodes from reckey2dba are OK and WRAPPED.
	     * (bad presumption). 
	     */
	    dba = ve_reckey2dba ();
	    if (usrblk.retncode != OE_OK) {
		usrblk.retncode = OE_NOTAVAIL;
		break;
	    }
	    usrblk.dba = dba;
	    usrblk.dbatab = &usrblk.dba;
	    usrblk.dbacount = 1;
	    ve_delete ();
	    break;

	case OE_DELETE_BATCH:
	    /* deletes all records in a table of dba's */
	    ve_delete ();
	    break;

	case OE_INITIALIZE:
	    /*
	     * first call from UI.  returns info used to build
	     * users screen 
	     */
	    oe_initialize ();
	    break;

	case OE_GET_EXPIRE:
	    usrblk.dba = *OE_expiration;
	    usrblk.retncode = OE_OK;
	    break;

	case OE_PING:
	    /* Null function.  Just does REINIT checks etc. */
	    usrblk.retncode = OE_OK;
	    break;

	case OE_SHUTDOWN:
	case OE_KILL:
	case OE_VALIDATE_PWD:
	case OE_CHANGE_PWD:
	    /* These functions are obsolete and harmless */
	    usrblk.retncode = OE_OK;
	    break;

	case OE_FINDSTR_REC:
	case OE_FINDSTR_HITL:
	case OE_DITTO2KWIC:
	    /* These functions are just obsolete */
	    sprintf (sprintbuf,
		PROGNAME"1027: User Interface Error.  "
		"%d is obsolete request code.\n",
		usrblk.request);
	    DtSearchAddMessage (sprintbuf);
	    usrblk.retncode = OE_BAD_REQUEST;
	    break;

	default:
	    sprintf (sprintbuf, catgets (dtsearch_catd, MS_oe, 367,
		    PROGNAME "367: User Interface Error.  "
		    "%d is invalid request code.\n"),
		usrblk.request);
	    DtSearchAddMessage (sprintbuf);
	    usrblk.retncode = OE_BAD_REQUEST;
	    break;

    }	/* end switch */

ENGINE_RETURN:
    if (usrblk.debug != 0L) {
	if (!DtSearchHasMessages())
	    fprintf (aa_stderr, PROGNAME"998 Msglist is empty.\n");
	else {
	    fprintf (aa_stderr,
		"mmmmmmmmmm Msglist mmmmmmmmm\n%s\n"
		"mmmmmmmmmmmmmmmmmmmmmmmmmmmm\n",
		DtSearchGetMessages());
	}
	fprintf (aa_stderr, PROGNAME "999 usrblk.retncode = %d (%s).\n",
	    usrblk.retncode, retncode_str (usrblk.retncode));
	fflush (aa_stderr);
    }
    return;
}  /* Opera_Engine() */

/******************************* DTOE.C ********************************/
