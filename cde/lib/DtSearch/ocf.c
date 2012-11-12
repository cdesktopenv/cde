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
 *   FUNCTIONS: add_a_keytype
 *              dumpboth
 *              dumpoef
 *              load_ocf
 *              obsolete_keyword
 *              ocfopen
 *              parse_boolean
 *              read_database
 *              read_filepath
 *              read_keytypes
 *              read_maxhits
 *              read_path
 *              read_rest_of_line
 *              set_boolbit
 *              set_boolint
 *              set_fileio
 *              set_int
 *              set_long
 *
 *   ORIGINS: 27
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1991,1995
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*************************** OCF.C ***************************
 * $TOG: ocf.c /main/9 1998/04/17 11:26:02 mgreess $
 * August 1991.
 * Load_ocf() reads an engine's "site configuration file" (.ocf),
 * parses its contents, and returns a ptr to a list of
 * DBLKs containing the file's info.
 * Returns TRUE if .ocf file format is acceptable and everything worked.
 * Returns FALSE if file contains fatal errors.  If file is ok, also
 * returns a list of dblks in global usrblk and sets some global variables.
 * The name and path of config file can be specified
 * or generated in a number of convenient ways.
 * The theory is: as many options as possible are in the config file
 * rather than command lines, etc.
 * Note: User configuration files are no longer supported.
 * 
 * $Log$
 * Revision 2.6  1996/03/05  21:12:01  miker
 * Reversed meaning of ascii_charmap test.
 *
 * Revision 2.5  1996/03/05  18:02:46  miker
 * Repl ctype functions with refs to ascii_charmap and COMMENT_CHARS.
 *
 * Revision 2.4  1996/02/05  16:34:57  miker
 * Silently try old file name when looking for site config file.
 *
 * Revision 2.3  1995/10/25  15:39:33  miker
 * Added prolog.
 *
 * Revision 2.2  1995/10/03  21:40:15  miker
 * Cosmetic msg change only.
 *
 * Revision 2.1  1995/09/22  21:30:12  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/05  21:41:59  miker
 * Changed environment variable OCFPATH to DTSROCFPATH.
 *
 * Revision 1.2  1995/09/05  18:46:13  miker
 * Moved all documentation to ocf.doc.
 *
 * Revision 1.1  1995/08/31  21:07:25  miker
 * Initial revision (renamed ocf.c from loadocf.c).
 *
 * *Log: loadocf.c,v *
 * Revision 1.19  1995/07/19  21:12:34  miker
 * 2.1.6c: Cleaned up documentation to match actual current capabilities.
 * Obsoleted MAILFEATURE and PRINTSERVER keywords.
 *
 * Revision 1.18  1995/07/18  22:52:17  miker
 * Added ability to open prespecified OE_sitecnfg_fname.
 */

#include "SearchE.h"
#include <ctype.h>
#include <stdlib.h>
#define X_INCLUDE_STRING_H
#define XOS_USE_NO_LOCKING
#include <X11/Xos_r.h>
#include <errno.h>
#include <unistd.h>	/* for POSIX getcwd() */
#include <sys/stat.h>

char    *strupr (char *s);

/*****#define DEBUG_DUMP******/
/********#define DEBUG_OEF**********/

#define PROGNAME	"OCF"
#define	DELIMITERS	" =,\t\n"
#define NEGLOGHALF	(-.69314718F)	/* negative natural log of .5 */
#define FNAME_SITECONFIG_OLD	"austext.ocf"
#define MS_misc		1
#define MS_loadocf	8

/*-------------------- STATIC GLOBALS ----------------------*/
static DBLK    *db;	/* curr database/dict */
static char    *cfgfname;
static char    *sprintbufp;
static char    *token;
static int      fatal_error;
static int      debug_ocf = FALSE;

/*-------------------- ENGINE GLOBALS ----------------------
 * These are globals which must be accessed by both sides. 
 * Globals unique to the engine are in oe.c.
 */
char           *OE_server_dir = NULL;
char           *OE_inittab_dir = NULL;

/*-------------------- OEF TABLE ----------------------*/
/*** (struct typdef located in oe.h)
	{
	char	*id;			 keyword identifier
	char	**OEFptr;		 addr of variable to change
	char	previously_specified;	 bool ensures only one spec
	} OEFTAB;
********/
OEFTAB          oef_table[] =
{
    { "AUDIT", &OEF_audit, FALSE, },
    { "DISCARD", &OEF_discard, FALSE, },
    { "NEWS", &OEF_news, FALSE, },
    { "NOTESNOT", &OEF_notesnot, FALSE, },
    { "NOTESSEM", &OEF_notessem, FALSE, },
    { "README", &OEF_readme, FALSE, },
    { NULL, NULL, FALSE },	/* end of list */
};


#ifdef DEBUG_DUMP	/* for debugging only */
/****************************************/
/*					*/
/*		 dumpboth		*/
/*					*/
/****************************************/
static void     dumpboth (void)
{
    char            typestring[4 * MAX_KTCOUNT + 2];
    int             i;
    char           *ptr;
    LLIST          *m;
    DBLK           *d;

    printf ("\nDUMPBOTH> fatal_error=%d, msglist=%p, dblist=%p\n",
	fatal_error, ausapi_msglist, usrblk.dblist);
    printf ("%s\n", DtSrGetMessages ());
    DtSrFreeMessages ();
    for (d = usrblk.dblist; d != NULL; d = d->link) {
	ptr = typestring;
	for (i = 0; i < d->ktcount; i++) {
	    *ptr++ = (d->keytypes[i].is_selected) ? '*' : ' ';
	    *ptr++ = d->keytypes[i].ktchar;
	    *ptr++ = ' ';
	}
	*ptr = 0;
	printf ("DBLK at %p link=%p name='%s' max=%d\n"
	    "    keytypes='%s', path='%s'\n",
	    d, d->link, d->name, d->maxhits,
	    typestring, NULLORSTR (d->path));
    }
    printf ("Push any key to continue...");

    *typestring = '\0';
    fgets (typestring, sizeof(typestring), stdin);
    if (strlen(typestring) && typestring[strlen(typestring)-1] == '\n')
      typestring[strlen(typestring)-1] = '\0';

    printf ("\n\n\n");
    return;
}  /* dumpboth() */

#endif

#ifdef DEBUG_OEF	/* for debugging only */
/****************************************/
/*					*/
/*		 dumpoef		*/
/*					*/
/****************************************/
static void     dumpoef (char *before_after)
{
    OEFTAB         *oef;
    fprintf (aa_stderr,
	"\toef_table[] %s (* = changed strings)\n",
	before_after);
    for (oef = oef_table; oef->id != NULL; oef++) {
	fprintf (aa_stderr, "\t%10s = %c'%s'\n",
	    oef->id,
	    (oef->previously_specified) ? '*' : ' ',
	    *(oef->OEFptr));
    }
    fputc ('\n', aa_stderr);
    return;
}  /* dumpoef() */

#endif


/****************************************/
/*					*/
/*	       parse_boolean		*/
/*					*/
/****************************************/
/* Forces token to upper case.
 * Returns +1 for TRUE, -1 for FALSE, and 0 for neither.
 */
static int      parse_boolean (char *token)
{
    /* Completely missing token defaults to affirmative */
    if (token == NULL)
	return +1;

    strupr (token);
    switch (token[0]) {
	case 'O':	/* on, off */
	    if (token[1] == 'N')
		return +1;
	    else if (token[1] == 'F')
		return -1;
	    else
		return 0;

	case 'T':	/* true */
	case 'Y':	/* yes */
	case 'E':	/* enabled */
	case '1':	/* (one) */
	    return +1;

	case 'F':	/* false */
	case 'N':	/* no */
	case 'D':	/* disabled */
	case '0':	/* (zero) */
	    return -1;

	default:
	    return 0;
    }
}  /* parse_boolean() */

/****************************************/
/*					*/
/*	       read_database		*/
/*					*/
/****************************************/
/* Validates next token as database name.
 * If not valid name, returns FALSE.
 * Otherwise chains down through the database list
 * looking for the one whose name matches the token.
 * If a match is found, sets db to it, returns TRUE.
 * If no match is found, creates and initializes
 * new structure, sets db to it, returns TRUE.
 */
static int      read_database (char *keyword, _Xstrtokparams *strtok_buf)
{
    DBLK          **linkptr;

    if ((token = _XStrtok(NULL, DELIMITERS, *strtok_buf)) == NULL) {
	sprintf (sprintbufp,
	    catgets (dtsearch_catd, MS_loadocf, 477,
		"%s %s: Missing database name after '%s' keyword.\n"),
	    PROGNAME"477", cfgfname, keyword);
	DtSearchAddMessage (sprintbufp);
	fatal_error = TRUE;
	return FALSE;
    }

    if (strlen (token) > 8) {
	sprintf (sprintbufp,
	    catgets (dtsearch_catd, MS_loadocf, 184,
	    "%s %s: More than 8 characters in Database name '%.16s'.\n"),
	    PROGNAME"184", cfgfname, token);
	DtSearchAddMessage (sprintbufp);
	fatal_error = TRUE;
	return FALSE;
    }

    /* Chain down the list looking for previously defined database structure */
    linkptr = &usrblk.dblist;
    for (db = usrblk.dblist; db != NULL; db = db->link) {
	if (strcmp (db->name, token) == 0)
	    return TRUE;
	linkptr = &db->link;
    }
    /* Didn't find matching dblk so allocate and initialize new one at
     * end of list (linkptr).  Use system defaults for most values.
     */
    if (debug_ocf)
	fprintf (aa_stderr, PROGNAME "204 "
	    "%s: creating dblk for '%s'\n", cfgfname, token);
    db = austext_malloc (sizeof (DBLK) + 8, PROGNAME "205", NULL);
    memset (db, 0, sizeof (DBLK));
    strcpy (db->name, token);
    db->is_selected = TRUE;
    *linkptr = db;

    return TRUE;
}  /* read_database() */


/****************************************/
/*					*/
/*	      set_boolbit		*/
/*					*/
/****************************************/
static void     set_boolbit (long *flagvar, long mask, char *flagname,
			     _Xstrtokparams *strtok_buf)
{
    int             i;

    token = _XStrtok(NULL, DELIMITERS, *strtok_buf);
    i = parse_boolean (token);
    if (i > 0)
	*flagvar |= mask;	/* switch ON */
    else if (i < 0)
	*flagvar &= ~mask;	/* switch OFF */
    else {
	sprintf (sprintbufp,
	    catgets (dtsearch_catd, MS_loadocf, 352,
	    "%s %s: Invalid %s value, line ignored.\n"),
	    PROGNAME"352", cfgfname, flagname);
	DtSearchAddMessage (sprintbufp);
    }
    return;
}  /* set_boolbit() */



/****************************************/
/*					*/
/*		set_int			*/
/*					*/
/****************************************/
static void     set_int (int *intvar, char *keyword, _Xstrtokparams *strtok_buf)
{
    int             myint;

    if ((token = _XStrtok(NULL, DELIMITERS, *strtok_buf)) == NULL) {
	token = catgets (dtsearch_catd, MS_loadocf, 140, "(missing)");
ERR_MSG:
	sprintf (sprintbufp, catgets (dtsearch_catd, MS_loadocf, 610,
	    "%s %s: %s value is '%s'. "
	    "Should be positive integer. Line ignored."),
	    PROGNAME"844", cfgfname, keyword, token);
	DtSearchAddMessage (sprintbufp);
	return;
    }
    if ((myint = atoi (token)) <= 0L)
	goto ERR_MSG;
    *intvar = myint;
    return;
}  /* set_int() */


/****************************************/
/*					*/
/*		set_long		*/
/*					*/
/****************************************/
static void     set_long (long *longvar, char *keyword, 
			  _Xstrtokparams *strtok_buf)
{
    long            longi;

    if ((token = _XStrtok(NULL, DELIMITERS, *strtok_buf)) == NULL) {
	token = catgets (dtsearch_catd, MS_loadocf, 140, "(missing)");
ERR_MSG:
	sprintf (sprintbufp, catgets (dtsearch_catd, MS_loadocf, 610,
	    "%s %s: %s value is '%s'. "
	    "Should be positive integer. Line ignored."),
	    PROGNAME"610", cfgfname, keyword, token);
	DtSearchAddMessage (sprintbufp);
	return;
    }
    if ((longi = atol (token)) <= 0L)
	goto ERR_MSG;
    *longvar = longi;
    return;
}  /* set_long() */


/****************************************/
/*					*/
/*	       set_boolint		*/
/*					*/
/****************************************/
static void     set_boolint (int *boolint, char *keyword,
			     _Xstrtokparams *strtok_buf)
{
    int             i;

    token = _XStrtok(NULL, DELIMITERS, *strtok_buf);
    i = parse_boolean (token);
    if (i > 0)
	*boolint = TRUE;	/* switch ON */
    else if (i < 0)
	*boolint = FALSE;	/* switch OFF */
    else {
	sprintf (sprintbufp,
	    catgets (dtsearch_catd, MS_loadocf, 352,
	    "%s %s: Invalid %s value, line ignored.\n"),
	    PROGNAME"352", cfgfname, keyword);
	DtSearchAddMessage (sprintbufp);
    }
    return;
}  /* set_boolint() */

/****************************************/
/*					*/
/*	      set_fileio		*/
/*					*/
/****************************************/
/* sets OE_fileio string pointer */
static void     set_fileio (_Xstrtokparams *strtok_buf)
{
    /* if a value is missing, presume -ON */
    if ((token = _XStrtok(NULL, DELIMITERS, *strtok_buf)) == NULL) {
	OE_fileio = "-ON";
	return;
    }

    /* check for hardcoded path name (anything that doesn't begin with '-') */
    if (token[0] != '-') {
	OE_fileio = austext_malloc (strlen (token) + 2, PROGNAME "592", NULL);
	strcpy (OE_fileio, token);
	return;
    }

    strupr (token);
    if (strcmp (token, "-ON") == 0)
	OE_fileio = "-ON";
    else if (strcmp (token, "-OFF") == 0)
	OE_fileio = "-OFF";
    else if (strcmp (token, "-HOME") == 0)
	OE_fileio = "-HOME";
    else {
	sprintf (sprintbufp, catgets (dtsearch_catd, MS_loadocf, 607,
	    "%s %s: Invalid FILEIO value.  "
	    "User file reads and writes prohibited."),
	    PROGNAME"607", cfgfname);
	DtSearchAddMessage (sprintbufp);
	OE_fileio = "-OFF";
    }
    return;
}  /* set_fileio() */


/****************************************/
/*					*/
/*	    read_rest_of_line		*/
/*					*/
/****************************************/
/* Malloc a copy of the rest of the input line after
 * the current token and set passed string ptr to it.
 */
static void     read_rest_of_line (char *keyword, char **passed_ptr,
				   _Xstrtokparams *strtok_buf)
{
    if ((token = _XStrtok(NULL, "\n", *strtok_buf)) == NULL) {
	sprintf (sprintbufp, catgets (dtsearch_catd, MS_loadocf, 1007,
	    "%s%s: Empty %s string ignored."),
	    PROGNAME"1007 ", cfgfname, keyword);
	DtSearchAddMessage (sprintbufp);
	return;
    }
    *passed_ptr = austext_malloc (strlen(token) + 4, PROGNAME"1020", NULL);
    strcpy (*passed_ptr, token);
    return;
}  /* read_rest_of_line() */ 


/****************************************/
/*					*/
/*	      read_filepath		*/
/*					*/
/****************************************/
static void     read_filepath (_Xstrtokparams *strtok_buf)
{
    char           *errp;
    char           *newpath;
    int             tabx;
    int             toklen;

    /* Second token on line should be valid id word.
     * Set tabx to matching oef table entry.
     */
    if ((token = _XStrtok(NULL, DELIMITERS, *strtok_buf)) == NULL) {
	token = catgets (dtsearch_catd, MS_loadocf, 142, "rest of line");
	errp = catgets (dtsearch_catd, MS_loadocf, 143, "is empty");

IGNORE_FILEPATH:
	sprintf (sprintbufp,
	    catgets (dtsearch_catd, MS_loadocf, 903,
	    "%s %s: FILEPATH '%s' %s. Line ignored."),
	    PROGNAME"903", cfgfname, token, errp);
	DtSearchAddMessage (sprintbufp);
	return;
    }
    strupr (token);
    for (tabx = 0; oef_table[tabx].id != NULL; tabx++) {
	if (strcmp (oef_table[tabx].id, token) == 0)
	    break;
    }
    if (oef_table[tabx].id == NULL) {
	errp = catgets (dtsearch_catd, MS_loadocf, 165, "unknown id");
	goto IGNORE_FILEPATH;
    }
    if (oef_table[tabx].previously_specified) {
	errp = catgets (dtsearch_catd, MS_loadocf, 166, "previously specified");
	goto IGNORE_FILEPATH;
    }

    /* Third token should be either a directory path (ends in slash)
     * or a directory path plus a file name (does not end in slash).
     */
    if ((token = _XStrtok(NULL, DELIMITERS, *strtok_buf)) == NULL) {
	token = oef_table[tabx].id;
	errp = catgets (dtsearch_catd, MS_loadocf, 168,
	    "missing path specification");
	goto IGNORE_FILEPATH;
    }
    toklen = strlen (token);

    /* Allocate buffer for new path string and initialize it.
     * Leave enough extra room for the default, dos-format file name,
     * i.e. 8 chars, a period, and a 3 char extension.
     */
    newpath = austext_malloc (toklen + 20, PROGNAME "956", NULL);
    strcpy (newpath, token);
    if (token[toklen - 1] == LOCAL_SLASH) {
	strncpy (newpath + toklen, *(oef_table[tabx].OEFptr), 12);
	newpath[toklen + 12] = 0;
    }

    /* Setting the previously_specified flag not only prevents multiple
     * specifications in a single file, but also permits user files
     * to override site files for specific ids because loadocf is always
     * called by the ui before it is called by oe initialization.
     */
    *(oef_table[tabx].OEFptr) = newpath;
    oef_table[tabx].previously_specified = TRUE;
    return;
}  /* read_filepath() */


/****************************************/
/*					*/
/*	       read_path		*/
/*					*/
/****************************************/
static void     read_path (_Xstrtokparams *strtok_buf)
{
#ifdef DEBUG_DUMP
                    printf (PROGNAME "217> Entering read_path().\n");
#endif

    /* parse the path for this dictionary/database */
    if ((token = _XStrtok(NULL, DELIMITERS, *strtok_buf)) == NULL) {
	sprintf (sprintbufp, catgets (dtsearch_catd, MS_loadocf, 221,
	    "%s %s: Database '%s', No PATH specified, statement ignored.\n"),
	    PROGNAME"221", cfgfname, db->name);
	DtSearchAddMessage (sprintbufp);
	return;
    }
    db->path = austext_malloc (strlen (token) + 4, PROGNAME "244", NULL);
    strcpy (db->path, token);
    ensure_end_slash (db->path);
    return;
}  /* read_path() */


/****************************************/
/*					*/
/*	      obsolete_keyword		*/
/*					*/
/****************************************/
/* append warning msg: passed keyword no longer used */
static void     obsolete_keyword (char *keyword)
{
    sprintf (sprintbufp, catgets (dtsearch_catd, MS_loadocf, 1,
	"%1$s%2$s: %3$s keyword is obsolete."),
	PROGNAME "001 ", cfgfname, keyword);
    DtSearchAddMessage (sprintbufp);
    return;
}  /* obsolete_keyword() */


/****************************************/
/*					*/
/*	      read_maxhits		*/
/*					*/
/****************************************/
static void     read_maxhits (_Xstrtokparams *strtok_buf)
{
    int             i;

#ifdef DEBUG_DUMP
    printf (PROGNAME "292> Entering read_maxhits().\n");
#endif

    /* parse the max number of hits for this dictionary */
    if ((token = _XStrtok(NULL, DELIMITERS, *strtok_buf)) == NULL) {
INVALID_MAXHITS:
	sprintf (sprintbufp,
	    catgets (dtsearch_catd, MS_loadocf, 304,
	    "%s %s: Database '%s', invalid MAXHITS value.\n"),
	    PROGNAME"304", cfgfname, db->name);
	DtSearchAddMessage (sprintbufp);
	fatal_error = TRUE;
	return;
    }
    i = atoi (token);
    if (i <= 0)
	goto INVALID_MAXHITS;
    else
	db->maxhits = i;
    return;
}  /* read_maxhits() */


/****************************************/
/*					*/
/*	      add_a_keytype		*/
/*					*/
/****************************************/
/* Subroutine of read_keytypes().
 * Concatenates a new DtSrKeytype node to end of current
 * db->keytypes array (in effect, remallocs db->keytypes).
 * Initializes ktchar with passed character.
 * Initializes name string with a generic label derived
 * from ktchar, which may later be changed by read_keytypes().
 * Keeps db->ktcount current.
 * If duplicate ktchar already exists, adds warning msg to
 * msglist, but changes nothing and returns index of old node.
 * Returns index of DtSrKeytype node or -1 on error.
 */
static int      add_a_keytype (char ktchar)
{
    DtSrKeytype        *kt;
    int             i, newindex;

    /* Make sure we have not exceeded max number of keytypes */
    if (db->ktcount >= MAX_KTCOUNT) {
	sprintf (sprintbufp,
	    catgets (dtsearch_catd, MS_loadocf, 732,
	    "%s %s: Database '%s', No more than %d keytypes allowed.\n"),
	    PROGNAME"732", cfgfname, db->name, MAX_KTCOUNT);
	DtSearchAddMessage (sprintbufp);
	fatal_error = TRUE;
	return -1;
    }

    /* Make sure ktchar is alphanumeric */
    if ( (ascii_charmap[ktchar] & (CONSONANT | VOWEL | NUMERAL)) == 0) {
	if (!isprint (ktchar))
	    ktchar = '?';
	sprintf (sprintbufp,
	    catgets (dtsearch_catd, MS_loadocf, 684,
	    "%s %s: Database '%s',\n  Invalid KEYTYPE character '%c'."),
	    PROGNAME"684", cfgfname, db->name, ktchar);
	DtSearchAddMessage (sprintbufp);
	fatal_error = TRUE;
	return -1;
    }

    /* Test for duplicate, preexisting keytype */
    kt = db->keytypes;
    for (i = 0; i < db->ktcount; i++)
	if (ktchar == kt[i].ktchar) {
	    sprintf (sprintbufp,
		catgets (dtsearch_catd, MS_loadocf, 1002,
		"%s %s: Database '%s', Duplicate KEYTYPE character '%c'.\n"),
		PROGNAME"1002", cfgfname, db->name, ktchar);
	    DtSearchAddMessage (sprintbufp);
	    return i;
	}

    /* Append valid new keytype to the keytypes array */
    i = sizeof(DtSrKeytype) * db->ktcount;	/* size of current array */
    kt = austext_malloc (i + sizeof(DtSrKeytype) + 4, PROGNAME "699", NULL);
    if (db->keytypes != NULL) {
	memcpy (kt, db->keytypes, i);
	free (db->keytypes);
    }
    newindex = db->ktcount;
    kt[newindex].is_selected = TRUE;
    kt[newindex].ktchar = ktchar;
    sprintf (kt[newindex].name,
	catgets (dtsearch_catd, MS_loadocf, 457, "'%c' Records"), ktchar);

    /* Warn about inaccessible lowercase ktchars */
    if ( ((ascii_charmap[ktchar] & 0xff) != ktchar) && OE_uppercase_keys ) {
	sprintf (sprintbufp,
	    catgets (dtsearch_catd, MS_loadocf, 1011,
	    "%s %s: Database '%s': Records with lowercase\n"
	    "  KEYTYPE character '%c' will be inaccessible.\n"
	    "  Set UPPERKEYS = FALSE if lowercase keys are intended."),
	    PROGNAME"1011", cfgfname, db->name, ktchar);
	DtSearchAddMessage (sprintbufp);
    }

    db->keytypes = kt;
    db->ktcount++;
    return newindex;
}  /* add_a_keytype() */


/****************************************/
/*					*/
/*	      read_keytypes		*/
/*					*/
/****************************************/
/* Builds keytypes arrays in current dblk.
 * Parameter 'how_many' limits the number keytype additions.
 * For example if how_many == 1, only 1 keytype will be added
 * and the balance of the line will be ignored.
 * We also quit adding keytypes when the total reaches
 * MAX_KTCOUNT or when we hit end of line or commments.
 */
static void     read_keytypes (int how_many, _Xstrtokparams *strtok_buf)
{
    /* 'last_ktchar' is index to db->keytypes.  if last_ktchar < 0 
     * then last token was not a ktchar, or it was an invalid ktchar.
     */
    int             last_ktchar = -1;
    DtSrKeytype        *kt;
    char           *charptr;

#ifdef DEBUG_DUMP
    printf (PROGNAME "361> Entering read_keytypes().\n");
#endif

    /* parsing loop for each keytypes token */
    while ((token = _XStrtok(NULL, DELIMITERS, *strtok_buf)) != NULL) {
	/* quit when comments begin */
	if (*token == '*' || *token == ':')
	    break;

	/* Test for ktchar token (beginning with a single quote) */
	if (*token == '\'') {
	    if (how_many <= 0)
		break;	/* don't exceed our quota */
	    last_ktchar = add_a_keytype (token[1]);
	    if (last_ktchar >= 0)
		how_many--;
	    continue;
	}

	/*
	 * Token is a name string.  If the last token was NOT a ktchar in
	 * single quotes, or it was an invalid ktchar, then create a new
	 * keytypes entry with the ktchar equal to the first char of this
	 * label. If any err, don't update the name string. 
	 */
	if (last_ktchar < 0) {
	    if (how_many <= 0)
		break;	/* don't exceed our quota */
	    last_ktchar = add_a_keytype (token[0]);
	    if (last_ktchar < 0)
		continue;
	    else
		how_many--;
	}

	/*
	 * Update the appropriate table entry with the label string.
	 * Convert underscores to spaces. 
	 */
	kt = &db->keytypes[last_ktchar];
	strncpy (kt->name, token, DtSrMAX_KTNAME);
	kt->name[DtSrMAX_KTNAME] = 0;
	for (charptr = kt->name; *charptr != 0; charptr++)
	    if (*charptr == '_')
		*charptr = ' ';

	last_ktchar = -1;
    }	/* end token parsing loop */

    return;
}  /* read_keytypes() */


/****************************************/
/*					*/
/*		 ocfopen		*/
/*					*/
/****************************************/
/* Subroutine of load_ocf().
 * Attempts to open config file created by concatenating
 * file name to passed pathname prefix.
 * If both fields are NULL, tries to open fully qualified
 * fname prespecified in OE_sitecnfg_fname.
 * Messages may be appended if some information, like an
 * environment variable, was found but still couldn't open file.
 * Loads OE_sitecnfg... fields after successful sitecnfg open.
 * Returns TRUE if fopen() successful, else FALSE.
 */

static int      ocfopen (char *prefix, char *fname, FILE ** stream)
{
    char            fullname[1024];
    FILE           *fptr;
    struct stat     statbuf;
    int             is_prespecified_fname = (prefix == NULL && fname == NULL);

    /*
     * Build a complete path/file name from passed args into fullname
     * buffer. 
     */
    if (is_prespecified_fname) {
	strncpy (fullname, OE_sitecnfg_fname, sizeof (fullname));
	fullname[sizeof (fullname) - 1] = 0;
	goto GOT_FULLNAME;
    }

    if (prefix == NULL) {
	if (debug_ocf)
	    fprintf (aa_stderr, " --> open not attempted: null prefix.\n");
	return FALSE;
    }

    strncpy (fullname, prefix, sizeof (fullname));
    fullname[sizeof (fullname) - 2] = 0;
    strcpy (ensure_end_slash (fullname), fname);


GOT_FULLNAME:
    fptr = fopen (fullname, "rt");
    if (debug_ocf) {
	if (fptr == NULL)
	    fprintf (aa_stderr, " --> %s\n", strerror (errno));
	else
	    fprintf (aa_stderr, " --> open succeeded!\n");
    }

    /*
     * If open failed: (1) Append information message if error is
     * something other than 'cant find file'. (Always retn msg if user
     * prespecified file name). (2) Return now. 
     */
    if (fptr == NULL) {
	if (errno != ENOENT || is_prespecified_fname) {
	    sprintf (sprintbufp, catgets (dtsearch_catd, MS_loadocf, 414,
		"%s Unable to open configuration file '%s':\n  %s."),
		PROGNAME"414", fullname, strerror(errno));
	    DtSearchAddMessage (sprintbufp);
	}
	return FALSE;
    }

    /* Load OE_... fields associated with site config */
    if (!is_prespecified_fname) {
	OE_sitecnfg_fname = austext_malloc (strlen (fullname) + 2,
	    PROGNAME "941", NULL);
	strcpy (OE_sitecnfg_fname, fullname);
    }
    if (fstat (fileno (fptr), &statbuf) == -1) {
	sprintf (sprintbufp, catgets (dtsearch_catd, MS_loadocf, 1404,
	    "%s Status unavailable for configuration file '%s': %s"),
	    PROGNAME"1404", fullname, strerror(errno));
	DtSearchAddMessage (sprintbufp);
	return FALSE;
    }
    OE_sitecnfg_mtime = statbuf.st_mtime;

    /* Save file name globally for error msgs */
    strcpy (cfgfname, fullname);
    *stream = fptr;
    return TRUE;
}  /* ocfopen() */


/****************************************/
/*					*/
/*	        load_ocf		*/
/*					*/
/****************************************/
/* This is the only function in this module seen by the outside world.
 * Returns TRUE if .ocf file format is acceptable and everything worked.
 * Returns FALSE if file contains fatal errors.  If file is ok, also
 * returns a list of dblks in global usrblk and sets some global variables.
 */
int             load_ocf (void)
{
    char           *p, *q;
    char            sprintbuf[1024];
    char            cfgfnamebuf[1024];
    char            inbuf[1024];
    char            uprtoken[128];
    FILE           *stream;
    _Xstrtokparams  strtok_buf;

    /* Initialize program globals */
    usrblk.dblist = NULL;	/* just to be sure */
    fatal_error = FALSE;
    sprintbufp = sprintbuf;
    cfgfname = cfgfnamebuf;

    debug_ocf = (usrblk.debug & USRDBG_RARE);

#ifdef DEBUG_DUMP
    puts ("\nentering load_ocf");
    dumpboth ();
#endif

#ifdef DEBUG_OEF
    dumpoef ("BEFORE");
#endif

    /* Construct the full pathname/filename and open it.
     * Try various optional path prefixes depending on
     * existence of certain environment and global variables.
     * Also silently try the old filename in each directory.
     * If the site config file is never found, return FALSE,
     * meaning 'fatal error', and an error msg.
     *
     * (1) The first place to look is a prespecified, fully qualified
     * name passed by the initialization function.  That occurs when
     * the global OE_sitecnfg_fname, which otherwise would be NULL,
     * points to the passed arg.  It's a fatal error if this name
     * was specified but cannot be opened.
     */
    if (OE_sitecnfg_fname) {
	if (debug_ocf)
	    fprintf (aa_stderr,
		PROGNAME "1446 ocfopen '%s', prespecified file name.\n",
		OE_sitecnfg_fname);
	if (ocfopen (NULL, NULL, &stream))
	    goto OCFOPEN_OK;
	else
	    return FALSE;
    }

    /* (2) try to find file in DTSROCFPATH directory */
    p = getenv ("DTSROCFPATH");
    if (debug_ocf)
	fprintf (aa_stderr, PROGNAME "1753 ocfopen '%s'. DTSROCFPATH='%s'.\n",
	    FNAME_SITECONFIG, NULLORSTR (p));
    if (ocfopen (p, FNAME_SITECONFIG, &stream))
	goto OCFOPEN_OK;
    if (ocfopen (p, FNAME_SITECONFIG_OLD, &stream))
	goto OCFOPEN_OK;

    /* (3) try to find file in current working directory */
    if (getcwd (inbuf, sizeof (inbuf)) == NULL)
	strcpy (inbuf, "./");	/* default to 'local' dir */
    else if (inbuf[0] == 0)
	strcpy (inbuf, "/");	/* presume 'root' dir */
    if (debug_ocf)
	fprintf (aa_stderr, PROGNAME "1771 ocfopen '%s'. cwd='%s'.\n",
	    FNAME_SITECONFIG, inbuf);
    if (ocfopen (inbuf, FNAME_SITECONFIG, &stream))
	goto OCFOPEN_OK;
    if (ocfopen (inbuf, FNAME_SITECONFIG_OLD, &stream))
	goto OCFOPEN_OK;

    /* (4) try to find file in HOME directory */
    q = getenv ("HOME");
    if (debug_ocf)
	fprintf (aa_stderr, "ocfopen '%s'. HOME='%s'.\n",
	    FNAME_SITECONFIG, NULLORSTR (q));
    if (ocfopen (q, FNAME_SITECONFIG, &stream))
	goto OCFOPEN_OK;
    if (ocfopen (q, FNAME_SITECONFIG_OLD, &stream))
	goto OCFOPEN_OK;

    /* (5) try to find file in server daemon's inittab directory */
    if (debug_ocf)
	fprintf (aa_stderr, "ocfopen '%s'. OE_inittab_dir='%s'.\n",
	    FNAME_SITECONFIG, NULLORSTR (OE_inittab_dir));
    if (ocfopen (OE_inittab_dir, FNAME_SITECONFIG, &stream))
	goto OCFOPEN_OK;
    if (ocfopen (OE_inittab_dir, FNAME_SITECONFIG_OLD, &stream))
	goto OCFOPEN_OK;

    /* (6) try to find file in server daemon's inetd.conf directory */
    if (debug_ocf)
	fprintf (aa_stderr, "ocfopen '%s'. OE_server_dir='%s'.\n",
	    FNAME_SITECONFIG, NULLORSTR (OE_server_dir));
    if (ocfopen (OE_server_dir, FNAME_SITECONFIG, &stream))
	goto OCFOPEN_OK;
    if (ocfopen (OE_server_dir, FNAME_SITECONFIG_OLD, &stream))
	goto OCFOPEN_OK;

    /* If we can't find site config on OE side we must quit.  */
    sprintf (sprintbuf, catgets (dtsearch_catd, MS_loadocf, 1643,
	    "%1$sCannot find or open '%2$s'\n"
	    "  configuration file in any of the following locations:\n"
	    "    DTSROCFPATH directory =     %3$s,\n"
	    "    current working directory = %4$s,\n"
	    "    HOME directory =            %5$s,\n"
	    "    /etc/inittab directory =    %6$s\n"
	    "    /etc/inetd.conf directory = %7$s\n"
	)
	,PROGNAME "1643 "
	,FNAME_SITECONFIG
	,(p) ? p : catgets (dtsearch_catd, MS_misc, 16,
	    "<no environment variable>")
	,inbuf
	,(q) ? q : catgets (dtsearch_catd, MS_misc, 16,
	    "<no environment variable>")
	,(OE_inittab_dir) ? OE_inittab_dir :
	    catgets (dtsearch_catd, MS_misc, 17,
	    "<no server daemon>")
	,(OE_server_dir) ? OE_server_dir :
	    catgets (dtsearch_catd, MS_misc, 17,
	    "<no server daemon>")
	);
    DtSearchAddMessage (sprintbuf);
    return FALSE;

OCFOPEN_OK:

    /* MAIN LOOP ---- Read each line from file */
    *inbuf = '\0';
    while (fgets (inbuf, sizeof (inbuf) - 1, stream) != NULL) {
	/* terminate string just to be sure */
        if (strlen(inbuf) && inbuf[strlen(inbuf)-1] == '\n')
          inbuf[strlen(inbuf)-1] = '\0';

	/* test for comment line */
	if (strchr (COMMENT_CHARS, inbuf[0]))
	    continue;

	/* parse KEYWORD */
	if ((token = _XStrtok(inbuf, DELIMITERS, strtok_buf)) == NULL)
	    continue;
	strcpy (uprtoken, token);
	strupr (uprtoken);

	if (strcmp (uprtoken, "PATH") == 0) {
	    if (read_database (uprtoken, &strtok_buf))
		read_path (&strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "KEYTYPES") == 0) {
	    if (read_database (uprtoken, &strtok_buf))
		read_keytypes (MAX_KTCOUNT, &strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "KEYTYPE") == 0) {
	    if (read_database (uprtoken, &strtok_buf))
		read_keytypes (1, &strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "FILEIO") == 0) {
	    set_fileio (&strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "USERNOTES") == 0) {
	    set_boolint (&OE_enable_usernotes, uprtoken, &strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "MARKDEL") == 0) {
	    set_boolint (&OE_enable_markdel, uprtoken, &strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "FASTDECODE") == 0) {
	    set_boolint (&OE_fastdecode, uprtoken, &strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "NOITERATE") == 0) {
	    set_boolbit (&OE_flags, OE_NO_ITERATE, uprtoken, &strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "UPPERKEYS") == 0) {
	    set_boolint (&OE_uppercase_keys, uprtoken, &strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "AUDIT") == 0) {
	    set_boolbit (&OE_flags, OE_AUDIT, uprtoken, &strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "FILEPATH") == 0) {
	    read_filepath (&strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "WHITLIM") == 0) {
	    set_long (&OE_words_hitlimit, uprtoken, &strtok_buf);
	    continue;
	}
	if (strcmp (token, "d3bug") == 0) {
	    usrblk.debug |= atol (token + strlen (token) + 1);
	    printf (PROGNAME "1630 %s: usrblk.debug = %ld, x%08lx.\n",
		cfgfname, usrblk.debug, usrblk.debug);
	    continue;
	}
	if (strcmp (uprtoken, "MAXHITS") == 0) {
	    if (read_database (uprtoken, &strtok_buf))
		read_maxhits (&strtok_buf);
	    continue;
	}
	if (strcmp (uprtoken, "LABEL") == 0) {
	    if (read_database (uprtoken, &strtok_buf))
		read_rest_of_line (uprtoken, &db->label, &strtok_buf);
	    continue;
	}

	/* if we made it this far, the first token is unknown */
	sprintf (sprintbufp,
	    catgets (dtsearch_catd, MS_loadocf, 495,
	    "%s %s: '%.16s' is invalid keyword.\n"),
	    PROGNAME"495", cfgfname, token);
	DtSearchAddMessage (sprintbufp);

#ifdef DEBUG_DUMP
	dumpboth ();
#endif
    }	/* ...keep reading next line in file until eof */

    fclose (stream);

    /* In a site config file, SOME database has to have been specified.
     * Also for each database, KEYTYPES must be present and perfect.
     * If no keytypes survived the KEYTYPES parse for each database,
     * or if there was no KEYTYPES line for a database
     * in site config file, remove the dblk and mark fatal error.
     */
    if (usrblk.dblist == NULL) {
	sprintf (sprintbufp, catgets (dtsearch_catd, MS_loadocf, 1869,
	    "%s %s: Configuration file must specify at least one database."),
	    PROGNAME"1869", cfgfname);
	DtSearchAddMessage (sprintbufp);
	fatal_error = TRUE;
    }
    else
	for (db = usrblk.dblist; db != NULL; db = db->link) {
	    if (db->ktcount <= 0) {
		sprintf (sprintbufp, catgets (dtsearch_catd, MS_loadocf, 986,
		    "%s %s: KEYTYPES missing for database '%s'."),
		    PROGNAME"986", cfgfname, db->name);
		DtSearchAddMessage (sprintbufp);
		cutnode_llist ((LLIST *)db, (LLIST **)&usrblk.dblist);
		free (db);
		fatal_error = TRUE;
	    }
	}

#ifdef DEBUG_OEF
    dumpoef ("AFTER");
#endif

#ifdef DEBUG_DUMP
    printf (PROGNAME "516 leaving load_ocf().\n");
    dumpboth ();
#endif

    if (fatal_error)
	return FALSE;
    usrblk.dblk = usrblk.dblist;	/* init usrblk.dblk */
    return TRUE;
}  /* load_ocf() */

/*************************** OCF.C ****************************/
