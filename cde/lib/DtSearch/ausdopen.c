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
 *   FUNCTIONS: austext_dopen
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1994,1995
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*************************** AUSDOPEN.C **************************
 * $XConsortium: ausdopen.c /main/6 1996/08/12 13:17:32 cde-ibm $
 * April 1994.
 * Austext_dopen() performs vista d_open() for an AusText database.
 * Optionally sets vista page cache size, renames the files,
 * opens the database, and optionally reads the dbrec.
 * Intended to be used by offline programs like cravel, cborodin,
 * and various utilities.  Similar to opendblk.c.
 *
 * $Log$
 * Revision 2.3  1995/10/25  22:22:45  miker
 * Added prolog.
 *
 * Revision 2.2  1995/10/19  20:20:51  miker
 * Deleted all RENFILE calls--database files no longer have to be renamed.
 *
 * Revision 2.1  1995/09/22  19:04:52  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.5  1995/08/31  21:53:28  miker
 * Minor changes for DtSearch.
 *
 * Revision 1.4  1995/05/30  18:19:21  miker
 * write all err msgs to aa_stderr (write_msg) rather than stdout.
 */
#include "SearchP.h"
#include "vista.h"

#define PROGNAME	"AUSDOPEN"


/****************************************/
/*					*/
/*	       austext_dopen		*/
/*					*/
/****************************************/
/* dbname:	1 - 8 char database name.
 * path:	Optional path prefix for .dbd file.
 *		If NULL is passed, default is vista
 *		default (current working directory).
 * d2xpath:	Used only for mrclean; should be NULL for all other callers.
 *		Specifies path prefix for rename of d2x files only.
 * cache:	Optional vista paging size.
 *		If zero is passed, default is global CACHE_SIZE.
 *		If < 16 specified, silently adjusted up to minimum 16.
 *		If cache specified is not a power of 2,
 *		cache adjusted upward to nearest power of 2.
 * dbrec:	If NULL is passed, dbrec is not read.  Otherwise
 *		database's dbrec is read into passed buffer.
 *
 * RETURNS:	TRUE if all went well.
 *		FALSE and writes error msg to ausapi_msglist if could not open.
 *		(vista abort prints out its own error messages).
 */
int	austext_dopen (
		char	*dbname,
		char	*path,
		char	*d2xpath,
		int	cache,
		DBREC   *dbrec)
{
    char            dbdbuf[2048];
    char            d2xbuf[2048];
    int             i;
    char            sprintbuf[2048];
    char           *d2xptr, *ptr, *src;

    /* Test dbname */
    if (dbname == NULL) {
INVALID_DBNAME:
	sprintf (sprintbuf, catgets (dtsearch_catd, 13, 348,
	    "%s Invalid database name '%s'."),
	    PROGNAME"348", NULLORSTR(dbname));
	DtSearchAddMessage (sprintbuf);
	return FALSE;
    }
    i = strlen (dbname);
    if (i < 1 || i > 8)
	goto INVALID_DBNAME;

    /* Test cache, silently rounding up to nearest power of 2.
     * 2^4 = 16 = minimum cache.  2^12 = 4096 = maximum cache.
     */
    if (cache == 0)
	cache = CACHE_SIZE;
    else {
	for (i = 4; i < 12; i++)
	    if (1 << i >= cache)
		break;
	cache = 1 << i;
    }

    /* If mrclean needs special d2x renames, build them now.
     * (d2xptr is where the ".d2x" extensions will be copied.)
     */
    if (d2xpath) {
	d2xptr = d2xbuf;
	for (i = 0; i < sizeof (d2xbuf) - 14; i++) {
	    if (d2xpath[i] == 0)
		break;
	    *d2xptr++ = d2xpath[i];
	}
	if (i > 0 && *(d2xptr - 1) != LOCAL_SLASH)
	    *d2xptr++ = LOCAL_SLASH;
	src = dbname;
	while (*src != 0)
	    *d2xptr++ = *src++;
    }

    /* Copy path, if any, to name buffer leaving room for the slash
     * which the caller may not have originally specified,
     * the 8 char database name, the 3 char file name extensions,
     * and the terminating \0.  Then set 'ptr' to the place
     * where the dbdname should be appended.
     */
    ptr = dbdbuf;
    if (path != NULL) {
	for (i = 0; i < sizeof (dbdbuf) - 14; i++) {
	    if (path[i] == 0)
		break;
	    *ptr++ = path[i];
	}
	if (i > 0 && *(ptr - 1) != LOCAL_SLASH)
	    *ptr++ = LOCAL_SLASH;
    }

    /* Now concatenate the dbname and set ptr to where
     * the file name extensions should go.
     */
    src = dbname;
    while (*src != 0)
	*ptr++ = *src++;

    /* Specify the cache size and open the database.
     * I use the original d_open() call so I can print
     * a good error msg if it fails.
     */
    *ptr = 0;	/* no extension used for .dbd file in OPEN */
    SETPAGES (PROGNAME "283", cache, 4);
    d_open (dbdbuf, "o");
    if (db_status != S_OKAY) {
	sprintf (sprintbuf, catgets (dtsearch_catd, 13, 379,
	    "%s Could not open database '%s':\n  %s."),
	    PROGNAME"379", dbdbuf, vista_msg (PROGNAME"379"));
	DtSearchAddMessage (sprintbuf);
	return FALSE;
    }

    /* From here on, emergency exits MUST close the database. */
    austext_exit_dbms = (void (*) (int)) d_close;

    /* If requested, read the dbrec into caller's buffer. */
    if (dbrec != NULL) {
	RECFRST (PROGNAME "285", OR_DBREC, 0);	/* seqtl retrieval */
	if (db_status != S_OKAY) {
NO_DBREC:
	    sprintf (sprintbuf,
		PROGNAME "289 Database '%s' has not been initialized.",
		dbname);
	    DtSearchAddMessage (sprintbuf);
	    return FALSE;
	}
	RECREAD (PROGNAME "302", dbrec, 0);
	if (db_status != S_OKAY)
	    goto NO_DBREC;
	swab_dbrec (dbrec, NTOH);
    }

    return TRUE;
}  /* austext_dopen() */

/*************************** AUSDOPEN.C **************************/
