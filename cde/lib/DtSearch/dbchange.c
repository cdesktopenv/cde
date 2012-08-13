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
 *   FUNCTIONS: database_has_changed
 *		file_has_changed
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/******************************* DBCHANGE.C ********************************
 * $XConsortium: dbchange.c /main/4 1996/05/07 13:26:54 drk $
 * March 1995.
 * Extracted from oe.c so it could also be called by server daemon.
 * When called only in the engine it was only called by a child
 * of the daemon, so the original daemon was never updated and
 * every single engine call resulted in an engine reinitialization.
 *
 * $Log$
 * Revision 2.2  1995/10/26  15:31:24  miker
 * Added prolog.
 *
 * Revision 2.1  1995/09/22  19:47:12  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.2  1995/08/31  22:18:48  miker
 * Minor changes for DtSearch including msg catalogs and
 * replacing socblk with usrblk for head of dblist.
 */
#include "SearchE.h"
#include <errno.h>
#include <sys/stat.h>

#define PROGNAME	"DBCHANGE"

void            oe_uninitialize (void);

/************************************************/
/*						*/
/*		  file_has_changed		*/
/*						*/
/************************************************/
/* If site config file or any d99 file has changed
 * since program initialization, close all databases and reinitialize.
 * This usually happens when the administrator has updated
 * a copy of one of the databases in a different directory.
 * He then changes the PATH parameter for that database in the
 * site config file.  By closing databases and reinitializing,
 * an administrator can swap databases without bringing the users down.
 * The test for the d99 files is included because in some systems,
 * like cose, a database may change without a corresponding
 * change in the site config file.
 * Databases cannot be swapped during an ongoing search.
 * If the passed file's mod time is unaccessible or has changed,
 * appends msg to msglist, reinitializes engine if necessary,
 * sets up usrblk.retncode, and returns TRUE.
 * Otherwise returns FALSE.
 */
static int      file_has_changed (char *fname, time_t origtime)
{
    struct stat     statbuf;
    char            sprintbuf[1024];

    if (stat (fname, &statbuf) == -1) {
	sprintf (sprintbuf, catgets (dtsearch_catd, 10, 1300,
		"%s Unable to comply with request; cannot access status\n"
		"  of database file '%s': %s"),
	    PROGNAME "1300", fname, strerror (errno));
	DtSearchAddMessage (sprintbuf);
	usrblk.retncode = OE_NOOP;
	return TRUE;
    }
    if (origtime != statbuf.st_mtime) {
	strcpy (sprintbuf, nowstring (&origtime));
	fprintf (aa_stderr,
	    "%s %s reinitialized AusText engine:\n"
	    "   changed file:       %s\n"
	    "   old file time:      %s\n"
	    "   current file time:  %s.\n",
	    PROGNAME "1312", aa_argv0, fname,
	    sprintbuf, nowstring (&statbuf.st_mtime));
	if (!(usrblk.flags & USR_NO_INFOMSGS)) {
	    sprintf (sprintbuf, catgets (dtsearch_catd, 10, 1313,
		    "%s *** REQUEST CANCELED *** %s Engine reinitialized\n"
		    "  due to modification of file %s, probably caused by\n"
		    "  update to one or more databases."),
		PROGNAME "1313", OE_prodname, fname);
	    DtSearchAddMessage (sprintbuf);
	}
	oe_uninitialize ();
	oe_initialize ();
	usrblk.retncode = OE_REINIT;
	return TRUE;
    }
    return FALSE;
}  /* file_has_changed() */


/************************************************/
/*						*/
/*	      database_has_changed		*/
/*						*/
/************************************************/
/* Verify that none of the databases has changed since the last call.
 * Don't check until after first initialize.
 * Returns TRUE if any changes, else FALSE.
 */
int             database_has_changed (void)
{
    char            fnamebuf[256];
    DBLK           *db;

    if (OE_sitecnfg_mtime == 0L)
	return FALSE;
    if (file_has_changed (OE_sitecnfg_fname, OE_sitecnfg_mtime))
	return TRUE;
    for (db = usrblk.dblist; db != NULL; db = db->link) {
	sprintf (fnamebuf, "%s%s" EXT_DTBS, db->path, db->name);
	if (file_has_changed (fnamebuf, db->iimtime))
	    return TRUE;
    }
    return FALSE;
}  /* database_has_changed() */

/******************************* DBCHANGE.C ********************************/
