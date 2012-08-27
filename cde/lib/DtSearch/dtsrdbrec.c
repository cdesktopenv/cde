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
 *   FUNCTIONS: main
 *		print_dbrec
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
/************************* DTSRDBREC.C **************************
 * $XConsortium: dtsrdbrec.c /main/8 1996/11/21 19:49:44 drk $
 * January 1994.
 * Prints out data in a dbrec system record in human readable form.
 *
 * $Log$
 * Revision 2.4  1996/02/01  18:18:18  miker
 * Added DtSrLaJPN2.  Deleted BETA definition.
 *
 * Revision 2.3  1995/10/25  19:49:12  miker
 * Renamed from dbrec.c.  Added prolog.c.
 *
 * Log: dbrec.c,v
 * Revision 2.2  1995/10/19  20:31:07  miker
 * No longer necessary to rename d00 file.
 * Database files may be read-only.
 *
 * Revision 2.1  1995/09/22  19:34:56  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.12  1995/09/19  21:51:43  miker
 * ifdef DTSEARCH, use DtSrVERSION instead of AUSAPI_VERSION in banner.
 *
 * Revision 1.11  1995/08/31  22:24:50  miker
 * Added report of or_language and other minor changes for DtSearch.
 * DtSearch executable (without semantic report) renamed dtsrdbrec.
 */
#include "SearchP.h"

#define XOS_USE_NO_LOCKING
#define X_INCLUDE_TIME_H
#include <X11/Xos_r.h>

#define PROGNAME	"DTSRDBREC"
#define MS_misc		1
#define MS_dbrec	23

/**#define MAIN_PROGRAM ***/

/************************************************/
/*                                              */
/*                language_name			*/
/*                                              */
/************************************************/
static char    *language_name (int language_number)
{
    switch (language_number) {
	case DtSrLaENG:		return "ENGLISH (ASCII)";
	case DtSrLaENG2:	return "ENGLISH (ISO Latin-1)";
	case DtSrLaESP:		return "SPANISH";
	case DtSrLaFRA:		return "FRENCH";
	case DtSrLaITA:		return "ITALIAN";
	case DtSrLaDEU:		return "GERMAN";
	case DtSrLaJPN:		return "JAPANESE (AUTO COMPOUNDS)";
	case DtSrLaJPN2:	return "JAPANESE (COMPOUNDS FROM LIST)";
	default:		return "USER DEFINED LANGUAGE";
    }
}

/************************************************/
/*                                              */
/*                 print_dbrec			*/
/*                                              */
/************************************************/
void            print_dbrec (char *dbname, struct or_dbrec * dbrec)
{
    int             i;
    char           *cptr;
    int             blobs_are_possible = FALSE;

    printf (catgets (dtsearch_catd, MS_dbrec, 1,
	    "---------- System Values for Database '%s' ----------\n"),
	dbname);
    printf (catgets (dtsearch_catd, MS_dbrec, 2,
	    "Schema version number (version) is '%s'.\n"),
	dbrec->or_version);

    printf (catgets (dtsearch_catd, MS_dbrec, 3,
	    "Maximum object key size (sizeof(objkey)) is %ld bytes.\n"),
	DtSrMAX_DB_KEYSIZE);
    if (ORD_USEHUGEKEYS & dbrec->or_dbflags)
	printf ("%s", catgets (dtsearch_catd, MS_dbrec, 4,
		"Optional 'Huge' keys enabled.\n"));

    printf (catgets (dtsearch_catd, MS_dbrec, 12,
	    "Maximum length of an abstract string (abstrsz) is %d.\n"),
	dbrec->or_abstrsz);
    if (dbrec->or_abstrsz == 0)
	puts (catgets (dtsearch_catd, MS_dbrec, 14,
		"  (Abstracts are not used in this database)."));
    else {
	/*
	 * if they CAN be compressed, say whether or not they
	 * actually are 
	 */
	if (dbrec->or_hufid != 0L)
	    printf (catgets (dtsearch_catd, MS_dbrec, 20,
		    "Abstracts are %scompressed.\n"),
		(ORC_COMPABSTR & dbrec->or_compflags) ? "" : "not ");
    }

    printf (catgets (dtsearch_catd, MS_dbrec, 22,
	    "Parsing language is number %d, %s.\n"),
	dbrec->or_language, language_name(dbrec->or_language));

    printf (catgets (dtsearch_catd, MS_dbrec, 24,
	    "Minimum word length (minwordsz) is %d.\n"),
	dbrec->or_minwordsz);

    printf (catgets (dtsearch_catd, MS_dbrec, 26,
	    "Maximum word length (maxwordsz) is %d.\n"),
	dbrec->or_maxwordsz);

    printf (catgets (dtsearch_catd, MS_dbrec, 30,
	    "Number of .d00 slots per object (recslots) is %d.\n"),
	dbrec->or_recslots);

    printf (catgets (dtsearch_catd, MS_dbrec, 36,
	    "  (Maximum number of database objects is %ld).\n"),
	0xffffffL / (long) dbrec->or_recslots);

    printf (catgets (dtsearch_catd, MS_dbrec, 40,
	    "Huffman compression table id (hufid) is %ld.\n"),
	dbrec->or_hufid);
    if (dbrec->or_hufid == 0L)
	puts (catgets (dtsearch_catd, MS_dbrec, 42,
		"  (Compression is disabled in this database)."));
    if (dbrec->or_hufid == -1L)
	puts (catgets (dtsearch_catd, MS_dbrec, 44,
		"  (Specific compression table is not yet determined)."));

    blobs_are_possible = FALSE;
    switch (dbrec->or_dbaccess) {
	case ORA_VARIES:
	    puts (catgets (dtsearch_catd, MS_dbrec, 50,
		"Engine accessibility to data may vary from object to object."));
	    blobs_are_possible = TRUE;
	    break;
	case ORA_NOTAVAIL:
	    puts (catgets (dtsearch_catd, MS_dbrec, 54,
		"Data objects are not directly accessible from the engine."));
	    break;
	case ORA_BLOB:
	    puts (catgets (dtsearch_catd, MS_dbrec, 56,
		"Data objects are stored internally as blobs."));
	    blobs_are_possible = TRUE;
	    break;
	case ORA_REFBLOB:
	    puts (catgets (dtsearch_catd, MS_dbrec, 60,
		"Only server file references to objects are stored in the blobs."));
	    break;
	case ORA_CREFBLOB:
	    puts (catgets (dtsearch_catd, MS_dbrec, 64,
		"Only client file references to objects are stored in the blobs."));
	    break;
	case ORA_REFKEY:
	    puts (catgets (dtsearch_catd, MS_dbrec, 68,
		"Object keys are server file references to the objects."));
	    break;
	case ORA_CREFKEY:
	    puts (catgets (dtsearch_catd, MS_dbrec, 72,
		"Object keys are client file references to the objects."));
	    break;
	case ORA_REFHUGEKEY:
	    puts (catgets (dtsearch_catd, MS_dbrec, 74,
		"Server file references to objects are "
		"stored in the 'huge' keys."));
	    break;
	case ORA_REFABSTR:
	    puts (catgets (dtsearch_catd, MS_dbrec, 80,
		"Server file references to objects are stored in the abstracts."));
	    break;
	case ORA_CREFABSTR:
	    puts (catgets (dtsearch_catd, MS_dbrec, 86,
		"Client file references to objects are stored in the abstracts."));
	    break;
	default:
	    printf (catgets (dtsearch_catd, MS_dbrec, 90,
		"Error: meaning of or_dbaccess value (%hd) is unknown.\n"),
		dbrec->or_dbaccess);
	    blobs_are_possible = TRUE;
	    break;
    }	/* end or_dbaccess switch */

    if (blobs_are_possible) {
	/*
	 * if they CAN be compressed, say whether or not they
	 * actually are 
	 */
	if (dbrec->or_hufid != 0L)
	    printf (catgets (dtsearch_catd, MS_dbrec, 100,
		    "Repository blobs are %scompressed.\n"),
		(ORC_COMPBLOB & dbrec->or_compflags) ? "" : "not ");
    }
    else
	puts (catgets (dtsearch_catd, MS_dbrec, 110,
		"Repository blobs are not used in this database."));

    printf (catgets (dtsearch_catd, MS_dbrec, 120,
	    "Database switches (dbflags) are 0x%lx:\n"),
	dbrec->or_dbflags);

    printf (catgets (dtsearch_catd, MS_dbrec, 130,
	    "  Inverted index %s words exactly as parsed.\n"),
	(ORD_XWORDS & dbrec->or_dbflags) ?
	catgets (dtsearch_catd, MS_dbrec, 124, "INCLUDES") :
	catgets (dtsearch_catd, MS_dbrec, 125, "EXCLUDES"));

    printf (catgets (dtsearch_catd, MS_dbrec, 140,
	    "  Inverted index %s word stems.\n"),
	(ORD_XSTEMS & dbrec->or_dbflags) ?
	catgets (dtsearch_catd, MS_dbrec, 124, "INCLUDES") :
	catgets (dtsearch_catd, MS_dbrec, 125, "EXCLUDES"));

    printf (catgets (dtsearch_catd, MS_dbrec, 160,
	    "  Use of optional 'huge' keys is %s.\n"),
	(ORD_USEHUGEKEYS & dbrec->or_dbflags) ?
	catgets (dtsearch_catd, MS_dbrec, 126, "ENABLED") :
	catgets (dtsearch_catd, MS_dbrec, 127, "DISABLED"));

    printf (catgets (dtsearch_catd, MS_dbrec, 162,
	    "  Mark-for-deletion is %s.\n"),
	(ORD_NOMARKDEL & dbrec->or_dbflags) ?
	catgets (dtsearch_catd, MS_dbrec, 127, "DISABLED") :
	catgets (dtsearch_catd, MS_dbrec, 126, "ENABLED"));

    printf (catgets (dtsearch_catd, MS_dbrec, 164,
	    "  Appendable user notes are %s.\n"),
	(ORD_NONOTES & dbrec->or_dbflags) ?
	catgets (dtsearch_catd, MS_dbrec, 127, "DISABLED") :
	catgets (dtsearch_catd, MS_dbrec, 126, "ENABLED"));

    printf (catgets (dtsearch_catd, MS_dbrec, 170,
	    "  Text characters are %s wide.\n"),
	(ORD_WIDECHAR & dbrec->or_dbflags) ?
	catgets (dtsearch_catd, MS_dbrec, 172, "MULTIPLE bytes") :
	catgets (dtsearch_catd, MS_dbrec, 174, "a SINGLE byte"));

    printf (catgets (dtsearch_catd, MS_dbrec, 200,
	    "Current number of database objects (reccount) is %ld.\n"),
	dbrec->or_reccount);

    printf (catgets (dtsearch_catd, MS_dbrec, 210,
	    "Last currently used slot number (maxdba) is %ld.\n"),
	dbrec->or_maxdba);

    for (i = 58; i > 0; i--)
	putchar ('-');
    putchar ('\n');
    return;
}  /* print_dbrec() */


#ifdef MAIN_PROGRAM
#include <locale.h>
#include <fcntl.h>
#include "vista.h"
/************************************************/
/*                                              */
/*                    main			*/
/*                                              */
/************************************************/
int	main (int argc, char *argv[])
{
    struct or_dbrec dbrec;
    char            renamebuf[256];
    time_t          now;
    struct tm	    *time_ptr;
    _Xltimeparams   localtime_buf;

    aa_argv0 = argv[0];
    setlocale (LC_ALL, "");
    dtsearch_catd = catopen (FNAME_DTSRCAT, 0);
    austools_catd = catopen (FNAME_AUSCAT, 0);

    time (&now);
    time_ptr = _XLocaltime(&now, localtime_buf);
    strftime (renamebuf, sizeof (renamebuf),
	catgets (dtsearch_catd, MS_misc, 22, "%A, %b %d %Y, %I:%M %p"),
	time_ptr);
    printf (catgets (dtsearch_catd, MS_misc, 23,
	"%s: Version %s.  Run %s.\n"),
	aa_argv0,
	DtSrVERSION,
	renamebuf);

    if (argc < 2) {
	printf (catgets (dtsearch_catd, MS_dbrec, 310,
		"USAGE: %s <dbname>\n"), aa_argv0);
	return 2;
    }
    sprintf (renamebuf, "%s.d00", argv[1]);
    db_oflag = O_RDONLY;	/* db files may be read-only */
    d_open (argv[1], "o");
    if (db_status != S_OKAY) {
	printf (catgets (dtsearch_catd, MS_dbrec, 330,
		"Could not open '%s' database.\n%s\n"),
		argv[1], vista_msg(PROGNAME"293"));
	return 3;
    }
    d_recfrst (OR_DBREC, 0);
    if (db_status != S_OKAY) {
	printf (catgets (dtsearch_catd, MS_dbrec, 340,
		"No dbrec record in database '%s'.\n"),
		argv[1]);
	return 4;
    }
    d_recread (&dbrec, 0);
    if (db_status != S_OKAY) {
	printf (catgets (dtsearch_catd, MS_dbrec, 350,
		"Can't read dbrec record in database '%s'.\n%s\n"),
		argv[1], vista_msg(PROGNAME"306"));
	return 5;
    }
    swab_dbrec (&dbrec, NTOH);
    print_dbrec (argv[1], &dbrec);
    return 0;
}  /* main() */

#endif

/************************* DTSRDBREC.C **************************/
