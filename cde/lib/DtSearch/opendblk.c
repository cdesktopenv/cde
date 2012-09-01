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
 *   FUNCTIONS: open_dblk
 *
 *   ORIGINS: 27
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1993,1995
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*********************** OPENDBLK.C ************************
 * $XConsortium: opendblk.c /main/5 1996/05/07 13:43:01 drk $
 * October 1993.
 * Opens all vista and the d99 database files in a dblk list.
 * Most errors are handled by disconnecting the offending
 * dblk from the list.  Of course loss of ALL dblks is fatal.
 *
 * The nonvista d99 files (words inverted index) are opened,
 * mostly to test if they can be found.  If not, we can return
 * a complete error message for the single dblk and unlink it.
 * Vista can only open all databases at once.
 *
 * Does not use dmacros.h for vista calls so error msgs can be
 * constructed and added to global msglist (although hard vista
 * errors will still first send a msg to stdout via dberr.c).
 * 
 * Open_dblk() should be used to replace all OPEN() calls
 * in systems using DBLK structures.
 * (1) It does not open dictionary files because they are not
 *     always needed (in OE, these are opened in oeinit.c).
 * (2) It does not read dbrec system records because the caller may
 *     want to initialize them (in OE, these are read and
 *     validated by ve_initialize() in ve.c).
 * (3) It does not open the other d9x files because they
 *     may not be present, depending on values in dbrec record.
 *
 * INPUT:
 * 'dblist' is linked list of dblks.
 *	Each dblk must have valid 'name' field,
 *      and 'path' field must be NULL or a valid path string.
 * 'numpages' is the number of vista cache pages to be opened.
 *	Where speed is critical it should be 64--
 *	for normal operations it should be no less than 16.
 * 'debugging' is a boolean. It is usually set to (usrblk.debug & USRDBG_RARE)
 *	and if TRUE, trace msgs are written to stdout.
 *
 * OUTPUT:
 * Sets correct vistano, iifile, and iimtime fields in each surviving dblk.
 * Places all err msgs on global msglist as they occur.
 * Sets austext_exit_dbms to d_close() on success (retncode > 0).
 * Returns TRUE if at least some of the databases were opened.
 *    Bad dblks are unlinked from dblist and freed.
 * Returns FALSE if fatal errors.
 *
 * $Log$
 * Revision 2.5  1996/02/01  19:19:41  miker
 * Minor change to err msg.
 *
 * Revision 2.4  1995/10/25  15:26:16  miker
 * Added prolog.
 *
 * Revision 2.3  1995/10/19  20:25:52  miker
 * Renaming database files no longer required.
 * Transaction logging disabled, no space needed for overflow files.
 * Open mode of non-vista database files also tracks new vista db_oflag.
 *
 * Revision 2.2  1995/10/03  21:45:26  miker
 * Cosmetic msg changes only.
 *
 * Revision 2.1  1995/09/22  21:38:03  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.8  1995/09/05  19:04:40  miker
 * Name and msgs changes for DtSearch.  Made ausapi_msglist universal.
 */
#include "SearchE.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "vista.h"

#define PROGNAME	"OPENDBLK"
#define MS_misc		1
#define MS_oeinit	9
#define MS_vista	13

/****************************************/
/*					*/
/*		open_dblk		*/
/*					*/
/****************************************/
/* dblk.path may be NULL */
int             open_dblk (DBLK ** dblist, int numpages, int debugging)
{
    DBLK           *db, *bad_db, **lastlink;
    int             i;
    size_t          totlen = 0L;
    char           *allnames;
    int             vistano = 0;
    char           *srcptr, *targptr;
    char            temp_file_name[1024];
    char            sprintbuf[1024];
    struct stat     statbuf;
    char	open_mode [8];

    if (debugging)
	fprintf (aa_stderr, PROGNAME"76 "
	    "Entering open_dblks().  db_oflag==%d.\n",
	    db_oflag);
    if (dblist == NULL || numpages < 8) {
BAD_INPUT:
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_oeinit, 99,
		"%s Programming Error: Invalid input to open_dblk()."),
	    PROGNAME "99");
	DtSearchAddMessage (sprintbuf);
	return FALSE;
    }
    if (*dblist == NULL)	/* empty list of dblks */
	goto BAD_INPUT;

    if (debugging) {
	fprintf (aa_stderr, PROGNAME "96 Current list of dblks:\n");
	for (db = *dblist; db != NULL; db = db->link) {
	    targptr = sprintbuf;
	    for (i = 0; i < db->ktcount; i++) {
		*targptr++ = db->keytypes[i].ktchar;
	    }
	    *targptr = 0;
	    fprintf (aa_stderr, "--> DBLK at %p link=%p name='%s' maxhits=%d\n"
		"    keytypes='%s', path='%s'\n",
		(void *) db, (void *) db->link, db->name, db->maxhits,
		sprintbuf, NULLORSTR (db->path));
	}
    }

    /* By doing setpages first, we can trap previously opened databases.
     * Overflow and transaction locking files are not required.
     */
    d_setpages (numpages, 0);
    if (db_status != S_OKAY) {
	DtSearchAddMessage (vista_msg (PROGNAME "389"));
	return FALSE;
    }

    /* ---- PASS #1 ------------------------------------------
     * Open nonvista (d99) files.  If error, unlink dblk from dblist.
     * Add up the total string length of surviving paths and database names.
     * This giant path/file string will be used in the single d_open()
     * below to find the .dbd files.
     * While we're at it, set vistano in each dblk.
     * The open mode depends on the current setting of db_oflag.
     */
    if (db_oflag == O_RDONLY)
	strcpy (open_mode, "rb");
    else
	strcpy (open_mode, "r+b");
    db = *dblist;
    lastlink = dblist;
    while (db != NULL) {
	if (db->path == NULL)
	    db->path = strdup ("");
	strcpy (temp_file_name, db->path);
	strcat (temp_file_name, db->name);
	strcat (temp_file_name, EXT_DTBS);
	if ((db->iifile = fopen (temp_file_name, open_mode)) == NULL) {
	    if (debugging)
		fprintf (aa_stderr, PROGNAME "129 UNLINK: cant open '%s'.\n",
		    temp_file_name);
	    sprintf (sprintbuf, catgets (dtsearch_catd, MS_oeinit, 317,
		    "%s Cannot open database file '%s'.\n"
		    "  Errno %d = %s\n"
		    "  %s is removing '%s' from list of available databases."),
		PROGNAME "317", temp_file_name, errno, strerror (errno),
		OE_prodname, db->name);
	    if (errno == ENOENT)
		strcat (sprintbuf, catgets (dtsearch_catd, MS_oeinit, 318,
		    "\n  This can usually be corrected by specifying a valid\n"
		    "  database PATH in the site configuration file."));
	    DtSearchAddMessage (sprintbuf);
	    goto DELETE_DB;
	}

	/*
	 * Find and save the timestamp for when the d99 file was
	 * last modified. An engine reinit is forced if it changes
	 * while the engine is running. 
	 */
	if (fstat (fileno (db->iifile), &statbuf) == -1) {
	    if (debugging)
		fprintf (aa_stderr,
		    PROGNAME "149 UNLINK: cant get status '%s'.\n",
		    temp_file_name);
	    sprintf (sprintbuf, catgets (dtsearch_catd, MS_oeinit, 1404,
		    "%s Removing database '%s' from list of "
		    "available databases because status is "
		    "unavailable for file %s: %s"),
		PROGNAME "1404", db->name, temp_file_name, strerror (errno));
	    DtSearchAddMessage (sprintbuf);
	    goto DELETE_DB;
	}
	db->iimtime = statbuf.st_mtime;

	/*
	 * This dblk is ok so far.  Increment pointers and
	 * continue. 
	 */
	if (debugging)
	    fprintf (aa_stderr, PROGNAME "163 opened '%s'.\n", temp_file_name);
	db->vistano = vistano++;
	totlen += strlen (db->path) + strlen (db->name) + 16;
	lastlink = &db->link;
	db = db->link;
	continue;

DELETE_DB:
	/*
	 * This dblk failed in one or more ways. Unlink it and
	 * don't increment pointers. If all dblks unlinked, *dblist
	 * will = NULL. 
	 */
	bad_db = db;	/* temp save */
	*lastlink = db->link;
	db = db->link;
	free (bad_db);
    }	/* end PASS #1 */

    /* quit if no dblks remain */
    if (vistano <= 0) {
	sprintf (sprintbuf, catgets (dtsearch_catd, MS_misc, 8,
		"%s No valid databases remain."), PROGNAME "265");
	DtSearchAddMessage (sprintbuf);
	return FALSE;
    }

    allnames = austext_malloc (totlen + 512, PROGNAME "66", NULL);

    /* ---- PASS #2 ------------------------------------------
     * Build string of accumulated path and database names.
     */
    targptr = allnames;
    for (db = *dblist; db != NULL; db = db->link) {
	srcptr = db->path;
	while (*srcptr != 0)
	    *targptr++ = *srcptr++;

	srcptr = db->name;
	while (*srcptr != 0)
	    *targptr++ = *srcptr++;
	*targptr++ = ';';
    }
    *(--targptr) = 0;	/* terminate string */

    if (debugging)
	fprintf (aa_stderr,
	    PROGNAME "150 vista opening databases '%s'\n", allnames);
    d_open (allnames, "o");	/* replaces OPEN() call from dmacros.h */

    if (db_status != S_OKAY) {
	targptr = austext_malloc (totlen + 128, PROGNAME"239", NULL);
	sprintf (targptr, catgets (dtsearch_catd, MS_vista, 378,
	    "%s Could not open following database name string:\n  '%s'"),
	    PROGNAME"378", allnames);
	DtSearchAddMessage (targptr);
	DtSearchAddMessage (vista_msg (PROGNAME"379"));
	free (allnames);
	free (targptr);
	return FALSE;
    }
    else if (debugging)
	fprintf (aa_stderr, " --> vista open successful!\n");

    /* From here on, emergency exits MUST close the databases */
    austext_exit_dbms = (void (*) (int)) d_close;

    free (allnames);
    return TRUE;
}  /* open_dblk() */

/*********************** OPENDBLK.C ************************/
