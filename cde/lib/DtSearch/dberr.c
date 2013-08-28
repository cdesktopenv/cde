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
 *   FUNCTIONS: dbautorec
 *              dberr
 *              vista_abort
 *              vista_msg
 *
 *   ORIGINS: 27
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1993,1995
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/************************** DBERR.C *************************
 * $XConsortium: dberr.c /main/6 1996/11/25 18:46:36 drk $
 * November 1993.
 * Extensive enhancement to original vista dberr() function.
 * This module MUST be linked into a system before libvista.a
 * so that this version of dberr() will be called rather than
 * the default Raima version.
 *
 * $Log$
 * Revision 2.2  1995/10/24  21:29:25  miker
 * Add IBM prolog.  Update NOFILE msg for enhanced vista.
 *
 * Revision 2.1  1995/09/22  19:26:52  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.7  1995/08/31  22:23:39  miker
 * Minor changes for DtSearch.
 *
 * Revision 1.6  1995/07/20  17:54:22  miker
 * Changed dberr_exit to global function ptr--easier to use or ignore.
 *
 * Revision 1.5  1995/07/18  22:27:16  miker
 * Delete msglist arg from vista_abort() function.
 * Replace exit() with dberr_exit() so applications
 * can clean up after database errors.
 */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE  1	/* for nl_catd */
#endif
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <nl_types.h>		/* for nl_catd */
#include "vista.h"
#include "dberr.h"	/* retained for default vista msgs */

#define MS_vista	13	/* message set number */

/*------------------- GLOBALS -------------------*/
static int      vista_syserrno = 0;
static char     vista_errmsg[1024];
void            (*dberr_exit) (int) = exit;

/* non AusText user should #define to -1 */
#define	dtsearch_catd -1
/* non AusText user should #define to stderr */
#define aa_stderr stderr

/****************************************/
/*					*/
/*		  dberr			*/
/*					*/
/****************************************/
/* Just set db_status and a new global called db_syserrno.
 * This function is called by the vista runtime code itself
 * so the prototype cannot be changed.
 * Original Raima comment for dberr: "vpp -nOS2 -dUNIX -nBSD
 *    -nVANILLA_BSD -nVMS -nMEMLOCK -nWINDOWS -nFAR_ALLOC
 *    -f/usr/users/master/config/nonwin dberr.c".
 */
int             dberr (int verrno)
{
    vista_syserrno = errno;
    db_status = (verrno == S_DEBUG) ? S_OKAY : verrno;
    return db_status;
}


/****************************************/
/*					*/
/*		 dbautorec		*/
/*					*/
/****************************************/
/* Database auto-recovery notification function.
 * Exact copy of original Raima function.
 * Original Raima comments:
 *   This function is called by the db_VISTA runtime whenever
 *   a database recovery is about to occur.  Any desired user
 *   interaction code (such as an "auto-recovery in process" message)
 *   should be included here.
 */
void            dbautorec (void)
{
    fputs (catgets (dtsearch_catd, MS_vista, 304,
	"\n*** db_VISTA auto recovery in process...\n"),
	aa_stderr);
    db_status = S_RECOVERY;
    return;
}  /* dbautorec() */


/****************************************/
/*					*/
/*		vista_msg		*/
/*					*/
/****************************************/
/* Assembles vista error message string corresponding
 * to value in db_status.  Unlike the original dberr(),
 * vista_msg() generates a msg for ALL vista statuses
 * including positive values (presumably programming errors
 * since positive values are normal vista return codes).
 * The passed variable will be prefixed to the string to
 * identify the location (module name, line number) of the error.
 * Returns a pointer to global buffer containing the msg string.
 */
char           *vista_msg (char *location)
{
    int             i;
    char           *defaultmsg;
    char           *msgtarg;

    /* Assemble standard Raima err msg */
    if (location == NULL)
	location = catgets (dtsearch_catd, MS_vista, 303,
	    "(unspecified location)");
    sprintf (vista_errmsg, catgets (dtsearch_catd, MS_vista, 311,
	"*** DB Error at %s, db_status = %d: %n"),
	location, db_status, &i);
    msgtarg = vista_errmsg + i;

    if (db_status == S_UNAVAIL) {	/* +5, usually at d_open() time */
	strcpy (msgtarg, catgets (dtsearch_catd, MS_vista, 315,
	    "Database in use by other users."));
    }
    else if (db_status >= 0)
	strcpy (msgtarg, catgets (dtsearch_catd, MS_vista, 312,
		"Programming Error."));
    else {
	if (db_status < 0 && db_status > -100)
	    defaultmsg = user_error[-(db_status + 1)];
	else if (db_status <= -900)
	    defaultmsg = system_error[-(db_status + 900)];
	else
	    defaultmsg = catgets (dtsearch_catd, MS_vista, 313,
		"Unknown Error.");
	strcpy (msgtarg, catgets (dtsearch_catd, MS_vista, -db_status,
	    defaultmsg));
    }
    msgtarg += strlen (msgtarg);

    /* Append system errno msg */
    sprintf (msgtarg, catgets (dtsearch_catd, MS_vista, 301,
	    "\n*** System I/O errno %d = %s"),
	vista_syserrno, strerror (vista_syserrno));
    msgtarg += strlen (msgtarg);

    /* Append additional information for common user error msgs */
    if (db_status == S_NOFILE) {
	strcpy (msgtarg, catgets (dtsearch_catd, MS_vista, 302,
	    "\n"
	    "*** The usual cause for this kind of error is a missing\n"
	    "*** or read-only database file, or some system limit\n"
	    "*** on the number of open files has been exceeded."));
    }
    msgtarg += strlen (msgtarg);
    *msgtarg++ = '\n';
    return vista_errmsg;
}  /* vista_msg() */


/****************************************/
/*					*/
/*		vista_abort		*/
/*					*/
/****************************************/
/* Writes out vista error message string
 * corresponding to value in db_status to aa_stderr.
 * Conceptually aa_stderr is stderr but it may
 * have been vectored to a file by caller.
 *
 * Then exits using an global exit function pointer.
 * The default is plain ol' exit() but users can map
 * it to anything, like DtSearchExit() or auscur_exit().
 *
 * Called by every vista function macro in dmacros.h,
 * so it exactly replaces former dboops() function.
 */
void            vista_abort (char *location)
{
    fputs (vista_msg (location), aa_stderr);
    fflush (aa_stderr);
    dberr_exit (93);
}

/************************** DBERR.C *************************/
