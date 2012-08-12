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
 *   FUNCTIONS: UNMALLOC
 *		dump_dblk
 *		oe_initialize
 *		oe_uninitialize
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1991,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/******************************* DTOEINIT.C ********************************
 * $XConsortium: dtoeinit.c /main/6 1996/11/21 19:49:29 drk $
 * Sept 1991.
 * Contains oe_initialize() function from universal "Opera Engine" code.
 * Has been separated out because it is the largest function and
 * needs to be tested separately.
 *
 * $Log$
 * Revision 2.4  1996/03/13  22:52:04  miker
 * Enabled several language loading debug requests from API.
 *
 * Revision 2.3  1995/10/25  21:59:27  miker
 * Renamed from oeinit.c.  Added prolog.
 *
 * Log: oeinit.c,v
 * Revision 2.2  1995/10/02  20:39:22  miker
 * Added zbflags arg to load_semantic().
 *
 * Revision 2.1  1995/09/22  21:38:53  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.14  1995/09/05  18:58:01  miker
 * Changed all socblk refs to universal, global usrblk (no more merges).
 * Deleted numerous globals.  Added DTSEARCH define.
 * Conflated all msglists to one ausapi_msglist.  All for DtSearch...
 */
#include "SearchE.h"
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#include <X11/Xos_r.h>

#define PROGNAME	"DTOEINIT"
#define MS_misc		1
#define MS_oeinit	9

extern int	debugging_loadlang;

/************************************************/
/*						*/
/*		     UNMALLOC			*/
/*						*/
/************************************************/
/* macro to test and free any malloc'ed pointer */
#define UNMALLOC(ptr)  if(ptr){free(ptr);ptr=NULL;}


/********************************/
/*				*/
/*	   dump_dblk		*/
/*				*/
/********************************/
/* Dumps values from passed dblk.
 * Used only for debugging initialization.
 */
static void     dump_dblk (char *msgprefix, DBLK *d)
{
    int		i;
    fprintf (aa_stderr, "%s: DBLK v#%d name='%s' label='%s', path='%s'\n",
	NULLORSTR (msgprefix), d->vistano, NULLORSTR (d->name),
	NULLORSTR (d->label), NULLORSTR (d->path));
    fprintf (aa_stderr, " mx=%d kt=", d->maxhits);
    for (i = 0; i < d->ktcount; i++) {
	fputc (' ', aa_stderr);
	if (d->keytypes[i].is_selected)
	    fputc ('*', aa_stderr);
	fputc (d->keytypes[i].ktchar, aa_stderr);
    }
    fputc ('\n', aa_stderr);
    return;
}  /* dump_dblk() */


/************************************************/
/*						*/
/*		  oe_initialize			*/
/*						*/
/************************************************/
/* - verify version number compatibilities.
 * - initialize socblk and OE_... globals.
 * - load site defaults ocf file and create database list.
 * - load dictionaries.
 * - merge site defaults into usrblk.
 * - open database(s).
 */
void            oe_initialize (void)
{
    char	*ptr;
    int		good_dblk_count = 0;
    DBLK	*db, *bad_db, **lastlink;
    char	sprintbuf[1024];
    FILE	*stream;
    OEFTAB	*oef;

    if (usrblk.debug & USRDBG_RARE)
	fprintf (aa_stderr, PROGNAME "555  Entered Engine Initialization.\n");
    debugging_loadlang = (usrblk.debug & USRDBG_RARE);

    /* Initialize most of the engine's globals.
     * OE_bit_vector_size is set in ve_initialize().
     * OE_bmhtab... are set by boyer-moore string searches only.
     * OE_prodname is initialized by compiler and maybe changed by main().
     * OE_expiration is initialized by compiler and never reset.
     * OE_sitecnfg_fname is initialized by ausapi or loadocf and never reset.
     * OE_sitecnfg_mtime is initialized by oe_initialize() and never reset.
     */
    global_memory_ptr =		NULL;
    austext_exit_mem =		(void (*) (int)) release_shm_mem;

    OE_flags =			0L;
    OE_objsize =		0L;
    OE_search_type =		'P';	/* default is statistical searches */
    OE_words_hitlimit =		WORDS_HITLIMIT;
    OE_enable_markdel =		FALSE;	/* former lvl2 default: TRUE */
    OE_enable_usernotes =	FALSE;	/* former lvl2 default: TRUE */
    OE_fastdecode =		FALSE;	/* former lvl2 default: TRUE */
    OE_fileio =			"-OFF";	/* former lvl2 default: "-ON" */
    OE_uppercase_keys =		FALSE;	/* former lvl2 default: TRUE */

    OEF_audit =			FNAME_AUDIT;
    OEF_discard =		FNAME_DISCARD_DATA;
    OEF_news =			FNAME_SITENEWS;
    OEF_notesnot =		FNAME_NOTES_BAC;
    OEF_notessem =		FNAME_NOTES_SEM;
    OEF_readme =		FNAME_README;

    for (oef = oef_table; oef->id != NULL; oef++)
	oef->previously_specified = FALSE;


    /* Verify version number compatibility between engine and UI.
     * Only checked if request is OE_INITIALIZE, not on reinits.
     */
    if (usrblk.request == OE_INITIALIZE) {
	if (!is_compatible_version (usrblk.query, SCHEMA_VERSION) ||
	    !is_compatible_version (usrblk.query, PROTOCOL_VERSION)) {
	    if (usrblk.query != NULL)
		ptr = usrblk.query;
	    else
		ptr = catgets (dtsearch_catd, MS_misc, 6, "<unknown>");
	    sprintf (sprintbuf, catgets (dtsearch_catd, MS_oeinit, 137,
		    "%s User Interface version %s and Engine "
		    "version %s are incompatible."),
		PROGNAME"137", ptr, AUSAPI_VERSION);
	    DtSearchAddMessage (sprintbuf);
	    usrblk.retncode = OE_NOTAVAIL;
	    return;
	}
    }

    /* Load site configuration (ocf) file and create dblks list */
    if (!load_ocf()) {
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_oeinit, 202,
	    "%s Initialization failed due to errors in configuration file."),
	    PROGNAME"202");
	DtSearchAddMessage (sprintbuf);
	usrblk.retncode = OE_NOTAVAIL;
	return;
    }

    /* Had to wait to write audit file till after site config set the flags */
    if (OE_flags & OE_AUDIT || usrblk.debug & USRDBG_RARE) {
	sprintf (sprintbuf,
	    "ENGINEINIT='%s' UVER='%s' " AUDIT_WHOWHEN "\n",
	    AUSAPI_VERSION, usrblk.query, usrblk.userid,
	    nowstring (NULL));
	if (usrblk.debug & USRDBG_RARE)
	    fprintf (aa_stderr, PROGNAME "362 %s", sprintbuf);
	if (OE_flags & OE_AUDIT) {
	    if ((stream = fopen (OEF_audit, "a ")) != NULL)
		/* the blank in "a " works around old aix bug */
	    {
		fputs (sprintbuf, stream);
		fclose (stream);
	    }
	}
    }	/* endif to write to audit file */


    /* ---- DATABASES PASSES #1 - #3 ------------------------------------
     * Call vista to open databases and load system (dbrec) records.
     * Where possible, this call will forgive errors by just
     * unlinking the offending dblk from the dblist.
     * However loss of all dblks equals a fatal error.
     */
    if (!ve_initialize()) {
INIT_FAILED:
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_oeinit, 266,
		"%s Initialization failed due to errors in database,\n"
		"  language, or related files."),
	    PROGNAME "266");
	DtSearchAddMessage (sprintbuf);
	usrblk.retncode = OE_NOTAVAIL;
	return;
    }

    /* ---- DATABASES PASS #4 ------------------------------------
     * Load each database's language and semantic files.
     * If an error is discovered in any one dblk,
     * it is unlinked from dblist.
     * However loss of all dblks on dblist is fatal.
     */
    good_dblk_count = 0;
    db = usrblk.dblist;	/* could already = NULL after ve_initialize() */
    lastlink = &usrblk.dblist;
    while (db != NULL) {
	if (!load_language (db, usrblk.dblist))
	    goto DELETE_DB;
	/*
	 * This dblk successfully loaded its language and and semantic
	 * files.  If no gui label was provided, set it to the
	 * database name.  Then increment pointers and continue. 
	 */
	if (db->label == NULL)
	    db->label = strdup (db->name);
	else if (db->label[0] == 0)
	    db->label = strdup (db->name);

	good_dblk_count++;
	lastlink = &db->link;
	db = db->link;
	continue;

DELETE_DB:
	/*
	 * One or more language or semantic files could not be loaded for
	 * this dblk.  Unlink it and don't increment pointers. 
	 */
	bad_db = db;	/* temp save */
	*lastlink = db->link;
	db = db->link;
	free (bad_db);
    }	/* end loop that loads all database files */

    /* Abort if fatal dictionary load errors */
    if (good_dblk_count <= 0) {
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_misc, 8,
	    "%s No valid databases remain."),
	    PROGNAME"265");
	DtSearchAddMessage (sprintbuf);
	goto INIT_FAILED;
    }

    memset (&saveusr, 0, sizeof (SAVEUSR));

    OE_flags |= OE_INITOK;
    if (usrblk.debug & USRDBG_RARE) {
        struct tm 	*time_ptr;
	_Xltimeparams	localtime_buf;

	if (*OE_expiration != 0)
	  {
	    time_ptr = _XLocaltime(OE_expiration, localtime_buf);
	    strftime (sprintbuf, 100, "%x", time_ptr);
	  }
	else
	    strcpy (sprintbuf, "0");
	if (OE_sitecnfg_mtime != 0)
	  {
	    time_ptr = _XLocaltime(&OE_sitecnfg_mtime, localtime_buf);
	    strftime (sprintbuf + 100, 100, "%x,%X", time_ptr);
	  }
	else
	    strcpy (sprintbuf + 100, "0");
	fprintf (aa_stderr, PROGNAME "666  Engine Initialization Completed.\n"
	    "  usrblk.flags=%ld(x%04lx), usrblk.debug=%ld(x%04lx).\n"
	    "  OE_flags=%ld(x%04lx), expiration=%s, sitecnfg=%s.\n"
	    ,usrblk.flags, usrblk.flags, usrblk.debug, usrblk.debug
	    ,OE_flags, OE_flags, sprintbuf, sprintbuf + 100
	    );
    }

    usrblk.retncode = OE_OK;
    return;
}  /* oe_initialize() */


/************************************************/
/*						*/
/*		  oe_uninitialize		*/
/*						*/
/************************************************/
/* Called by Opera_Engine() whenever the site config file
 * has been altered.  Closes databases, frees all allocated storage,
 * and resets everything so oe_initialize() can be re-called. 
 * Allows administrator to swap to newer, updated databases
 * without forcing users to shutdown by changing PATH value
 * in site config file.
 */
void            oe_uninitialize (void)
{
    DBLK           *db, *nextdb;
    OEFTAB         *oef;

    ve_shutdown ();	/* close all databases */
    UNMALLOC (usrblk.abstrbuf);

    /* release shared memory, suffixes array, etc, if any */
    release_shm_mem ();

    /* Free allocated storage in oef table and reset the default filenames. */
    for (oef = oef_table; oef->id != NULL; oef++) {
	if (oef->previously_specified) {
	    oef->previously_specified = FALSE;
	    free (*(oef->OEFptr));
	}
    }

    db = usrblk.dblist;
    while (db != NULL) {

	/* Save next dblk so we'll know where to go after freeing this one */
	nextdb = db->link;

	UNMALLOC (db->path);
	UNMALLOC (db->keytypes);

	if (db->iifile != NULL) {
	    fclose (db->iifile);
	    db->iifile = NULL;
	}
	if (db->syofile != NULL) {
	    fclose (db->syofile);
	    db->syofile = NULL;
	}
	if (db->syifile != NULL) {
	    fclose (db->syifile);
	    db->syifile = NULL;
	}

	unload_language (db);

	free (db);
	db = nextdb;
    }
    usrblk.dblist = NULL;
    usrblk.dblk = NULL;
    return;
}  /* oe_uninitialize() */

/******************************* DTOEINIT.C ********************************/
