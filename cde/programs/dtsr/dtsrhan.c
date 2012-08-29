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
 *   FUNCTIONS: cleanup
 *		flag_shutdown
 *		is_blank
 *		is_count
 *		is_time
 *		is_whitespace
 *		main
 *		mmm_to_digits
 *		my_strtok
 *		new_rec
 *		open_outfile
 *		process_infile
 *		process_profile
 *		process_record
 *		segregate_dicname
 *		token
 *		usage_msg
 *		user_arg_processor
 *		validate_id
 *		write_record
 *
 *   ORIGINS: 27
 *
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1993,1996
 *   All Rights Reserved
 *   Licensed Materials - Property of IBM
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/************** DTSRHAN.C ***************
 * $XConsortium: dtsrhan.c /main/9 1996/09/23 21:02:27 cde-ibm $
 * Oct 1993.
 * Modification of handel.c for CDE system.
 * Converts free form text in accordance with a profile file
 * into a formal .fzk file.
 *
 * $Log$
 * Revision 2.8  1996/04/10  22:55:27  miker
 * Removed ref to BETA.
 *
 * Revision 2.7  1996/04/10  19:48:52  miker
 * Added support for null dates.
 *
 * Revision 2.6  1996/03/25  18:53:56  miker
 * Changed FILENAME_MAX to _POSIX_PATH_MAX.
 *
 * Revision 2.5  1996/02/01  18:20:02  miker
 * Changed parser/stemmer calls to new readchar format.
 *
 * Revision 2.4  1995/11/07  17:51:46  miker
 * Fixed bug in progress dot processing.
 * Added rec count to err msgs to facilitate finding bad recs.
 *
 * Revision 2.3  1995/10/25  18:57:27  miker
 * Renamed from chandel.c.  Added prolog.
 *
 * Log: chandel.c,v
 * Revision 2.2  1995/10/02  20:00:51  miker
 * Added semantic analysis so original handel.c no longer required.
 *
 * Revision 2.1  1995/09/22  19:20:45  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.7  1995/09/19  21:49:22  miker
 * ifdef DTSEARCH, use DtSrVERSION instead of AUSAPI_VERSION in banner.
 *
 * Revision 1.6  1995/08/31  22:15:33  miker
 * Added MMM fields for date processing like handel.c.
 * Minor changes for DtSearch, mostly msg sets changes..
 * Executable module renamed dtsrload for DtSearch.
 *
 * Revision 1.5  1995/06/08  00:32:43  miker
 * 2.1.5f: Bug fix.  Segfault if constant string not enclosed in quotes.
 * Enable negative field offsets.
 */
#include "SearchP.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <locale.h>

#define PROGNAME	"DTSRHAN"
#define	MS_chandel	5
#define RECS_PER_DOT	10L

/*-- Numerical codes for storing "undefined items" --*/
#define UNDEFINED 0
#define ANY -999

/*-- Codes for "modes" of text buffering --*/
#define INCLUDE 1
#define EXCLUDE 2
#define SET 3
#define CLEAR 4
#define DEFAULT 5
#define NONE 6

/*-- codes for abstract mode --*/
#define FIELDS 2
#define USER 3
#define GENERATE 4

/*-- Exit codes for errors --*/
#define USAGE 2
#define FILE_ERROR 3
#define SYNTAX_ERROR 4
#define BAD_ID 5
#define BAD_DIC 6
#define BAD_PROFILE 7
#define CRIT 9

/*-- Token types --*/
#define UNKNOWN 999
#define LINE 1
#define FIELD 2
#define TEXT 3
/****#define WORDS 4*****/
#define IMAGE 5
#define KEY 6
#define ABSTR 7
#define KEYCHAR 8
#define DELIMITER 9
#define TEXTINCLUDE 10
#define TEXTEXCLUDE 11
#define TFIELDINCLUDE 12
#define TFIELDEXCLUDE 13
#define DELBLANKLINES 14
#define IMAGEINCLUDE 15
#define IMAGEEXCLUDE 16
#define DISCARD 17
#define CONSTANT 18
#define UPPER 19
#define DATEFLD 20

/*-- General Defines --*/
#define EOLN -999
#define EOW -998
#define CNTRL_L 12
#define MAX_ALPHABET_SIZE 256

/*-- definitions of various data structures --*/
struct line_comp {		/*-- Individual components for line ids --*/
    int             column_number;
    char            text[80];
    int             d[MAX_ALPHABET_SIZE];
    int             text_length;
    struct line_comp *next;
};

struct line_id {		/*-- list of line ids to be handled     --*/
    char            name[80];
    struct line_comp *comp;
    struct line_comp *head;
    struct line_id *next;
    struct rec     *line;
    int             word_action;
    int             text_action;
    int             image_action;
};

struct field_id {		/*-- list of field ids to be used       --*/
    struct field_id *next;
    char            name[80];
    struct line_id *line;
    int             offset;
    int             defined_length;
    int             length;
    char            text[80];
    int             d[MAX_ALPHABET_SIZE];
    int             text_length;
    char            value[80];
    int             word_action;
    int             text_action;
    int             image_action;
    char            line_id[80];
    int             constant;
    int             is_month;
};

struct rec {			/*-- holds a complete image of a record --*/
    struct rec     *next;
    struct line_id *line;
    int             line_num;
    char            text[200];
};

struct date_id {
    char            field_id[80];
    struct date_id *next;
    struct field_id *field;
};

struct key_id {
    char            field_id[80];
    struct key_id  *next;
    struct field_id *field;
};

struct finclude {
    char            field_id[80];
    int             value;
    struct finclude *next;
};

struct include {
    char            line_id[80];
    int             value;
    struct include *next;
};

/************************************************/
/*						*/
/*		     GLOBALS			*/
/*						*/
/************************************************/
char            abstracter[100];
int             abstract =		GENERATE;
struct key_id  *abstract_table =	NULL;
/***nl_catd		dtsearch_catd = (nl_catd) -1;***/
int             bad_parm =		FALSE;
int             bad_profile =		FALSE;
int             bot_defined =		FALSE;
struct date_id *date_table =		NULL;
int             date_pos_defined =	FALSE;
static char    *del_string =		" +=,\t\n";
/****static char    *del_string = " +-=,\t\n";**allow neg nums***/
int             del_blanklines =	FALSE;
char            dicname[10] =		{0};
char            dicpath[_POSIX_PATH_MAX] =	{0};
int             discard =		TRUE;
int             discard_record =	FALSE;
struct finclude *finclude_tab =		NULL;
struct field_id *field_table =		NULL;
struct include *i_i_t =			NULL;
int             imagemode =		INCLUDE;
int             imageflag =		INCLUDE;
struct include *include_tab =		NULL;
char           *infile;
static FILE    *instream =		NULL;
static long	key_count =		0;
struct key_id  *key_table =		NULL;
int             key_defined =		FALSE;
int             key_pos_defined =	FALSE;
char            key_char;
char            key_value[80];
static char    *line_mode =		"CTHULHU TOCOMA ZYYXY UTOPIA";
struct line_id *line_table =		NULL;
time_t          now;
char            now_str[24];
struct tm       nowtm;
static int	null_date_specified =	FALSE;
struct tm      *objdate_tmptr;
int             outcount =		0;
char            outmode[8] =		"w";
int             outmode_specified =	FALSE;
char           *outfile;
static FILE    *outstream =		NULL;
char           *profile;
static long	rec_count =		0L;	/* for err msgs */
struct rec     *record_head =		NULL;
struct tm       rectm;
int             screen_width =		79;
int             shutdown_now =		FALSE;
int             started =		FALSE;
int             textflag =		INCLUDE;
int             textmode =		INCLUDE;
int             top_defined =		FALSE;
struct line_id *top_rec;
char            top_rec_name[80];
int             uppercase =		FALSE;
int             uninit_line =		TRUE;
int             uninit_field =		TRUE;
int             warnings =		TRUE;
int             wordmode =		INCLUDE;


/****************************************************************
    VALIDATE_ID - validates an indentifier (first character a
letter, then alpha-numeric, etc)
****************************************************************/
int             validate_id (char *s)
{
    int             i;

    if (s==NULL)
	return FALSE;
    if (!isalpha (s[0])) {
	return FALSE;
    }
    for (i = 1; i < strlen (s); i++)
	if (!isalnum (s[i]) && s[i] != '_') {
	    return FALSE;
	}
    return TRUE;
}


/****************************************************************
   OPEN_OUTFILE - open outputfile - if the file already exists,
  and no mode was specified by the user, ask the user what to do.
****************************************************************/
void            open_outfile ()
{
    FILE           *temp;
    int             i;

    if (!outmode_specified)
	if ((temp = fopen (outfile, "r")) != NULL) {
	    fclose (temp);
	    printf ( catgets(dtsearch_catd, MS_chandel, 3,
		"Output file '%s' already exists.\n") ,
		outfile);
	    printf ( "%s", catgets(dtsearch_catd, MS_chandel, 4,
		"Append, overwrite, or quit? [a,o,q] ") );
	    i = tolower (getchar ());

	    if (i == 'a')
		strcpy (outmode, "a");
	    else if (i == 'o')
		strcpy (outmode, "w");
	    else
		exit (FILE_ERROR);
	}
    if (!strcmp (outfile, "-"))
	outstream = stdout;
    else {
	if ((outstream = fopen (outfile, outmode)) == NULL) {
	    printf ( catgets(dtsearch_catd, MS_chandel, 7,
		"Unable to open output file '%s'.\n") , outfile);
	    exit (FILE_ERROR);
	}
    }
    return;
}  /* open_outfile */


/****************************************************************
    IS_TIME - returns TRUE if the passed string contains the
string "time" in any case (upper/lower) - without destroying the
original string, in case it was a field name.
****************************************************************/
int             is_time (char *orig)
{
    char           *s;
    int             i;
    /* copy original string to temp buffer, so we can manipulate */
    s = (char *) malloc (sizeof (char) * strlen (orig) +5);
    strcpy (s, orig);
    for (i = 0; i < strlen (s); i++)
	s[i] = tolower (s[i]);

    if (strcmp (s, "time") == 0)
	return TRUE;
    else
	return FALSE;
}

/****************************************************************
    IS_COUNT - returns TRUE if the passed string contains the
string "count" in any case (upper/lower) - without destroying the
original string, in case it was a field name.
****************************************************************/
int             is_count (char *orig)
{
    char           *s;
    int             i;
    /* copy original string to temp buffer, so we can manipulate */
    s = (char *) malloc (sizeof(char) * strlen(orig) +5);
    strcpy (s, orig);
    for (i = 0; i < strlen (s); i++)
	s[i] = tolower (s[i]);

    if (strcmp (s, "count") == 0)
	return TRUE;
    else
	return FALSE;
}

/****************************************************************
    MY_STRTOK - my own version of strtok - why? Because I need
a little flexibility when parsing out the string component -
what if it has quotes embedded? 

s1 = the line to be parsed (= start at beginning)
	or NULL (= start where we last left off).

s2 = the 'delete' string or array of token separators.
	Usually it is either the global 'del_string' (" +-=,\t\n")
	or a string consisting of a single double-quote char.
****************************************************************/
char           *my_strtok (char *s1, const char *s2)
{
    char           *sbegin, *send;
    static char     stringbuf[100];
    static char    *ssave = "";
    int             i = 0;

    memset (stringbuf, 0, sizeof(stringbuf));
    sbegin = (s1) ? s1 : ssave;	/* start of string or where we last left off */

    if (strcmp (s2, "\"") == 0) {	/* parsing for a string */
	if (*sbegin == '\0') {
	    ssave = "";
	    return NULL;
	}
	while (*sbegin != '"' && *sbegin != '\0') {/*-- look for first " --*/
	    sbegin++;
	}
	if (*sbegin == '\0') {
	    ssave = "";
	    return NULL;
	}
	sbegin++;		/*-- skip past "      --*/
	i = 0;
	while (*sbegin != '"') {/*-- until other "    --*/
	    if (*sbegin == '\\')	/*-- escape sequence  --*/
		sbegin++;
	    if (*sbegin == '\0') {	/*-- end of line  --*/
		ssave = "";
		return NULL;
	    }
	    stringbuf[i++] = (*sbegin);
	    sbegin++;
	}
	send = sbegin;
	*send++ = '\0';
	ssave = send;
	stringbuf[i] = 0;
	sbegin = stringbuf;
	return (sbegin);
    }

    else {	/* not parsing for a string */
	sbegin += strspn (sbegin, s2);
	if (*sbegin == '\0') {
	    ssave = "";
	    return NULL;
	}			/* if */
	send = strpbrk (sbegin, s2);
	if (*send != '\0')
	    *send++ = '\0';
	ssave = send;
	return (sbegin);
    }				/* else */
}				/*-- my_strtok --*/

/****************************************************************
    IS_BLANK - determines if a string contains nothing but
   white space.
*****************************************************************/
int             is_blank (char *s)
{
    int             i;
    for (i = 0; i < strlen (s); i++)
	if (!isspace (s[i]))
	    return 0;
    return 1;
}				/*-- is_blank --*/


/***********************************************************************
   TOKEN - returns a numerical token for the defined identifier types
************************************************************************/
int             token (char *s)
{
    int             i;

    if (s == NULL)
        return UNKNOWN;

    /*-- make token all lowercase --*/
    for (i = 0; i < strlen (s); i++)
	s[i] = tolower (s[i]);

    if (!strcmp (s, "line"))
	return LINE;
    if (!strcmp (s, "field"))
	return FIELD;
    if (!strcmp (s, "key"))
	return KEY;
    if (!strcmp (s, "date"))
        return DATEFLD;
    if (!strcmp (s, "text"))
	return TEXT;
    if (!strcmp (s, "keychar"))
	return KEYCHAR;
    if (!strcmp (s, "delimiter"))
	return DELIMITER;
    if (!strcmp (s, "textinclude"))
	return TEXTINCLUDE;
    if (!strcmp (s, "textexclude"))
	return TEXTEXCLUDE;
    if (!strcmp (s, "tfieldinclude"))
	return TFIELDINCLUDE;
    if (!strcmp (s, "tfieldexclude"))
	return TFIELDEXCLUDE;
    if (!strcmp (s, "delblanklines"))
	return DELBLANKLINES;
    if (!strcmp (s, "abstract"))
	return ABSTR;
    if (!strcmp (s, "user"))
	return USER;
    if (!strcmp (s, "fields"))
	return FIELDS;
    if (!strcmp (s, "generate"))
	return GENERATE;
    if (!strcmp (s, "image"))
	return IMAGE;
    if (!strcmp (s, "imageinclude"))
	return IMAGEINCLUDE;
    if (!strcmp (s, "imageexclude"))
	return IMAGEEXCLUDE;
    if (!strcmp (s, "discard"))
	return DISCARD;
    if (!strcmp (s, "constant"))
	return CONSTANT;
    if (!strcmp (s, "upper"))
	return UPPER;
    return UNKNOWN;
}				/*-- token --*/

/***********************************************************************
    PROCESS_PROFILE - process 'profile' file.
************************************************************************/
void            process_profile ()
{
    FILE           *prof;
    char            prof_line[200];
    int             line_num = 0;
    int             i;
    char           *tok;
    struct line_id *line_current;
    struct field_id *field_current;
    struct key_id  *key_current;
    struct date_id *date_current;
    struct key_id  *abstract_current;
    struct finclude *finclude_current;
    struct include *include_current;
    struct include *i_i_current;
    int             found;
    int             tok_type;

    /*-- open file --*/
    if ((prof = fopen (profile, "r")) == NULL) {
	printf ( catgets(dtsearch_catd, MS_chandel, 11,
		"\nError - unable to open profile file '%s'.\n") , profile);
	exit (FILE_ERROR);
    }
    /*-- Get next line --*/
    do {
	fgets (prof_line, 200, prof);
	line_num++;
	if (feof (prof))
	    break;
	if (prof_line[0] == '#' || is_blank (prof_line))
	    continue;
	tok = my_strtok (prof_line, del_string);
	if (tok == NULL)
	    continue;	/* ignore blank line */
	tok_type = token (tok);
	switch (tok_type) {
	    case UPPER:
		uppercase = TRUE;
		break;

	    case LINE:
		/*-- build node --*/
		if (line_table == NULL) {
		    line_table = (struct line_id *) malloc (
			sizeof (struct line_id));
		    line_current = line_table;
		    line_table->next = NULL;
		    line_table->comp = (struct line_comp *) malloc (
			sizeof (struct line_comp));
		    line_table->head = line_table->comp;
		    line_table->comp->next = NULL;
		}
		/* if */
		else {
		    line_current->next = (struct line_id *) malloc (
			sizeof (struct line_id));
		    line_current = line_current->next;
		    line_current->next = NULL;
		    line_current->comp = (struct line_comp *) malloc (
			sizeof (struct line_comp));
		    line_current->head = line_current->comp;
		    line_current->comp->next = NULL;
		}		/* else */
		line_current->image_action = NONE;
		line_current->word_action = NONE;
		line_current->text_action = NONE;
		line_current->line = NULL;
		line_current->name[0] = 0;
		/*-- get identifier --*/
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok))
		    strcpy (line_current->name, tok);
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 12,
			"Error line %d: invalid identifier '%s'.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		    break;
		}
		/*-- get first value token --*/
		tok = my_strtok ('\0', del_string);
		if (!tok) {
		    printf ( catgets(dtsearch_catd, MS_chandel, 13,
			"Error line %d - identifier '%s' missing value(s).\n") ,
			line_num, line_current->name);
		    bad_profile = TRUE;
		    continue;
		}		/* if */
		if (!strcmp (tok, "*"))
		    line_current->comp->column_number = ANY;
		else
		    line_current->comp->column_number = atoi (tok);
		if (line_current->comp->column_number == 0) {
		    printf ( catgets(dtsearch_catd, MS_chandel, 14,
			"Error line %d - zero or bad value for '%s'.\n"
			"  offensive token: %s.\n") ,
			line_num, line_current->name, tok);
		    bad_profile = TRUE;
		    continue;
		}		/* if */
		/* d d d-- get second token of pair -- */
		tok = my_strtok ('\0', "\"");
		if (!tok) {
		    if (line_current->comp->column_number == ANY) {
			printf ( catgets(dtsearch_catd, MS_chandel, 15,
			"Error line %d - for identifier '%s', column has "
			"been set to ANY\n  but there is no "
			"identifying signature string.\n") , 
			    line_num, line_current->name);
			bad_profile = TRUE;
			continue;
		    }
		    /* if (line.. */
		    else {
			strcpy (line_current->comp->text, line_mode);
			continue;
		    }		/* else */
		}
		/* if (!tok) */
		else {
		    strcpy (line_current->comp->text, tok);
		    line_current->comp->text_length =
			strlen (line_current->comp->text);
		}
		/*-- check for more tokens for LINE type of line --*/
		for(;;) {
		    tok = my_strtok ('\0', del_string);
		    if (!tok)
			break;
		    /*-- build component node --*/
		    line_current->comp->next = (struct line_comp *) malloc (
			sizeof (struct line_comp));
		    line_current->comp = line_current->comp->next;
		    line_current->comp->next = NULL;
		    /*-- check # and store --*/
		    if (!strcmp (tok, "*"))
			line_current->comp->column_number = ANY;
		    else
			line_current->comp->column_number = atoi (tok);
		    if (line_current->comp->column_number == 0) {
			printf ( catgets(dtsearch_catd, MS_chandel, 16,
			"Error line %d - zero or bad value for "
			"identifier '%s'\n  offensive token: %s.\n") ,
			    line_num, line_current->name, tok);
			bad_profile = TRUE;
			continue;
		    }		/* if (line.. */
		    /* -  --- get second of pair -- */
		    tok = my_strtok ('\0', "\"");
		    if (!tok) {
			if (line_current->comp->column_number == ANY)
			    printf ( catgets(dtsearch_catd, MS_chandel, 15,
			        "Error line %d - for identifier '%s', column has "
			        "been set to ANY\n  but there is no "
			        "identifying signature string.\n") , 
				line_num, line_current->name);
			else
			    printf ( catgets(dtsearch_catd, MS_chandel, 18,
				"Error line %d - missing value for "
				"identifier '%s'\n") ,
				line_num, line_current->name);
			bad_profile = TRUE;
			continue;
		    }		/* if (!tok) */
		    strcpy (line_current->comp->text, tok);
		    line_current->comp->text_length =
			strlen (line_current->comp->text);
		}  /* end for(;;) loop for continuing LINE token pairs */
		break;	/* end case LINE */

	    case CONSTANT:
	    case FIELD:
		if (field_table == NULL) {
		    field_table = (struct field_id *)
			malloc (sizeof (struct field_id));
		    field_current = field_table;
		    field_table->next = NULL;
		}
		else {
		    field_current->next = (struct field_id *) malloc (
			sizeof (struct field_id));
		    field_current = field_current->next;
		    field_current->next = NULL;
		}
		memset (field_current, 0, sizeof(struct field_id));
		field_current->word_action = NONE;
		field_current->image_action = NONE;
		field_current->text_action = NONE;
		if (tok_type == CONSTANT)
		    field_current->constant = TRUE;

		/*-- get identifier --*/
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok)) {
		    strcpy (field_current->name, tok);
		    if (strncmp (tok, "MMM", 3) == 0)
			field_current->is_month = TRUE;
		}
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 12,
			"Error line %d: invalid identifier '%s'.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		    break;
		}
		if (field_current->constant) {
		    /*-- get constant value --*/
		    tok = my_strtok (NULL, "\"");
		    if (!tok) {
			printf ( catgets(dtsearch_catd, MS_chandel, 93,
			    "Error line %d - '%s' string not "
			    "enclosed in double quotes.\n"),
			    line_num, field_current->name);
			bad_profile = TRUE;
			continue;
		    }
		    strcpy (field_current->value, tok);
		}	/* end CONSTANT */

		else {	/* ...must be FIELD */
		    /*-- get line id --*/
		    tok = my_strtok ('\0', del_string);
		    if (!tok) {
			printf ( catgets(dtsearch_catd, MS_chandel, 13,
			    "Error line %d - identifier '%s' missing value(s).\n") ,
			    line_num, line_current->name);
			bad_profile = TRUE;
			continue;
		    }		/* if */
		    strcpy (field_current->line_id, tok);

		    /*-- get "string"  --*/
		    tok = my_strtok ('\0', "\"");
		    if (!tok) {
			printf ( catgets(dtsearch_catd, MS_chandel, 93,
			    "Error line %d - '%s' string not "
			    "enclosed in double quotes.\n"),
			    line_num, field_current->name);
			bad_profile = TRUE;
			continue;
		    }
		    strcpy (field_current->text, tok);
		    field_current->text_length = strlen (field_current->text);

		    /*-- get offset --*/
		    tok = my_strtok ('\0', del_string);
		    if (!tok) {
			printf ( catgets(dtsearch_catd, MS_chandel, 13,
			"Error line %d - identifier '%s' missing value(s).\n") ,
			    line_num, line_current->name);
			bad_profile = TRUE;
			continue;
		    }		/* if */
		    field_current->offset = atoi (tok);

		    /*-- get length --*/
		    tok = my_strtok ('\0', del_string);
	/*******if (!tok && field_current->length == ANY) ************/
		    if (!tok) {
			printf ( catgets(dtsearch_catd, MS_chandel, 13,
			"Error line %d - identifier '%s' missing value(s).\n") ,
			    line_num, line_current->name);
			bad_profile = TRUE;
			continue;
		    }		/* if */
		    if (!strcmp (tok, "eoln"))
			field_current->defined_length = EOLN;
		    else if (!strcmp (tok, "eow"))
			field_current->defined_length = EOW;
		    else
			field_current->defined_length = atoi (tok);
		} /* end FIELD */
		break;

	    case DELIMITER:
		/*-- get next token - should be name of line --*/
		if (warnings &&(bot_defined || top_defined))
		    printf ( catgets(dtsearch_catd, MS_chandel, 23,
			"Warning line %d: Delimiter redefined.\n") ,
			line_num);
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok))
		    strcpy (top_rec_name, tok);
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 12,
			"Error line %d: invalid identifier '%s'.\n"),
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		}
		tok = my_strtok ('\0', del_string);
		if (tok == NULL)
		    goto BAD_DELIM_VAL;
		/* convert tok to lowercase */
		for (i = 0; i < strlen (tok); i++)
		    tok[i] = tolower (tok[i]);
		if (!strcmp (tok, "top")) {
		    top_defined = TRUE;
		    bot_defined = FALSE;
		} else if (!strcmp (tok, "bottom")) {
		    top_defined = FALSE;
		    bot_defined = TRUE;
		} else {
BAD_DELIM_VAL:
		    printf ( catgets(dtsearch_catd, MS_chandel, 25,
			"Error line %d: delimiter not specified as "
			"'top' or 'bottom'.\n") ,
		    line_num);
		    bad_profile = TRUE;
		}
		break;

	    case TFIELDEXCLUDE:
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok)) {
		    if (finclude_tab == NULL) {
			finclude_tab = (struct finclude *) malloc (
			    sizeof (struct finclude));
			finclude_current = finclude_tab;
		    } else {
			finclude_current->next = (struct finclude *) malloc (
			    sizeof (struct finclude));
			finclude_current = finclude_current->next;
		    }
		    finclude_current->next = NULL;
		    strcpy (finclude_current->field_id, tok);
		    finclude_current->value = EXCLUDE;
		}
		/* if it was a valid token */
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 26,
			"Error line %d: invalid token '%s'.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		}
		break;

	    case TFIELDINCLUDE:
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok)) {
		    if (finclude_tab == NULL) {
			finclude_tab = (struct finclude *) malloc (
			    sizeof (struct finclude));
			finclude_tab->next = NULL;
			finclude_current = finclude_tab;
		    }
		    /* if valid */
		    else {
			finclude_current->next = (struct finclude *) malloc (
			    sizeof (struct finclude));
			finclude_current = finclude_current->next;
			finclude_current->next = NULL;
		    }		/* else valid */
		    strcpy (finclude_current->field_id, tok);
		    finclude_current->value = INCLUDE;
		}
		/* if it was a valid token */
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 27,
			"Error line %d: invalid token '%s'.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		}
		break;

	    case TEXTEXCLUDE:
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok)) {
		    if (include_tab == NULL) {
			include_tab = (struct include *) malloc (
				sizeof (struct include));
			include_tab->next = NULL;
			include_current = include_tab;
		    }
		    /* if valid */
		    else {
			include_current->next = (struct include *) malloc (
			    sizeof (struct include));
			include_current = include_current->next;
			include_current->next = NULL;
		    }		/* else in valid */
		    strcpy (include_current->line_id, tok);
		    tok = my_strtok ('\0', del_string);
		    if (tok == NULL)
			include_current->value = EXCLUDE;
		    else {
			if (validate_id (tok)) {
			    include_current->value = SET;
			    include_current->next = (struct include *) malloc (
				sizeof (struct include));
			    include_current = include_current->next;
			    include_current->next = NULL;
			    strcpy (include_current->line_id, tok);
			    include_current->value = DEFAULT;
			}
			/* if in else */
			else {
			    printf ( catgets(dtsearch_catd, MS_chandel, 12,
				"Error line %d: invalid identifier '%s'.\n") ,
				line_num, tok);
			    bad_profile = TRUE;
			}	/* else in else */
		    }		/* else tok wasn't NULL */
		}
		/* if validate... */
		else {
		    printf (catgets(dtsearch_catd, MS_chandel, 12,
			"Error line %d: invalid identifier '%s'.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		}		/* else not a valid token */
		break;

	    case IMAGEEXCLUDE:
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok)) {
		    if (i_i_t == NULL) {
			i_i_t = (struct include *) malloc (
				sizeof (struct include));
			i_i_t->next = NULL;
			i_i_current = i_i_t;
		    }
		    /* if in valid */
		    else {
			i_i_current->next = (struct include *) malloc (
			    sizeof (struct include));
			i_i_current = i_i_current->next;
			i_i_current->next = NULL;
		    }		/* else in valid */
		    strcpy (i_i_current->line_id, tok);
		    tok = my_strtok ('\0', del_string);
		    if (tok == NULL)
			i_i_current->value = EXCLUDE;
		    else {
			if (validate_id (tok)) {
			    i_i_current->value = SET;
			    i_i_current->next = (struct include *) malloc (
				sizeof (struct include));
			    i_i_current = i_i_current->next;
			    i_i_current->next = NULL;
			    strcpy (i_i_current->line_id, tok);
			    i_i_current->value = DEFAULT;
			}
			/* if in else */
			else {
			    printf ( catgets(dtsearch_catd, MS_chandel, 12,
				"Error line %d: invalid identifier '%s'.\n") ,
				line_num, tok);
			    bad_profile = TRUE;
			}	/* else in else */
		    }		/* else tok wasn't NULL */
		}
		/* if validate... */
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 12,
			"Error line %d: invalid identifier '%s'.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		}		/* else not a valid token */
		break;

	    case IMAGEINCLUDE:
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok)) {
		    if (i_i_t == NULL) {
			i_i_t = (struct include *) malloc (
				sizeof (struct include));
			i_i_t->next = NULL;
			i_i_current = i_i_t;
		    }
		    /* if in valid */
		    else {
			i_i_current->next = (struct include *) malloc (
			    sizeof (struct include));
			i_i_current = i_i_current->next;
			i_i_current->next = NULL;
		    }		/* else in valid */
		    strcpy (i_i_current->line_id, tok);
		    tok = my_strtok ('\0', del_string);
		    if (tok == NULL)
			i_i_current->value = INCLUDE;
		    else {
			if (validate_id (tok)) {
			    i_i_current->value = CLEAR;
			    i_i_current->next = (struct include *) malloc (
				sizeof (struct include));
			    i_i_current = i_i_current->next;
			    i_i_current->next = NULL;
			    strcpy (i_i_current->line_id, tok);
			    i_i_current->value = DEFAULT;
			}
			/* if in else */
			else {
			    printf ( catgets(dtsearch_catd, MS_chandel, 12,
				"Error line %d: invalid identifier '%s'.\n") ,
				line_num, tok);
			    bad_profile = TRUE;
			}	/* else in else */
		    }		/* else tok wasn't NULL */
		}
		/* if validate... */
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 12,
			"Error line %d: invalid identifier '%s'.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		}		/* else not a valid token */
		break;

	    case TEXTINCLUDE:
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok)) {
		    if (include_tab == NULL) {
			include_tab = (struct include *) malloc (
				sizeof (struct include));
			include_tab->next = NULL;
			include_current = include_tab;
		    }
		    /* if in valid */
		    else {
			include_current->next = (struct include *) malloc (
			    sizeof (struct include));
			include_current = include_current->next;
			include_current->next = NULL;
		    }		/* else in valid */
		    strcpy (include_current->line_id, tok);
		    tok = my_strtok ('\0', del_string);
		    if (tok == NULL)
			include_current->value = INCLUDE;
		    else {
			if (validate_id (tok)) {
			    include_current->value = CLEAR;
			    include_current->next = (struct include *) malloc (
				sizeof (struct include));
			    include_current = include_current->next;
			    include_current->next = NULL;
			    strcpy (include_current->line_id, tok);
			    include_current->value = DEFAULT;
			}
			/* if in else */
			else {
			    printf ( catgets(dtsearch_catd, MS_chandel, 12,
				"Error line %d: invalid identifier '%s'.\n") ,
				line_num, tok);
			    bad_profile = TRUE;
			}	/* else in else */
		    }		/* else tok wasn't NULL */
		}
		/* if validate... */
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 12,
			"Error line %d: invalid identifier '%s'.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		}		/* else not a valid token */
		break;

	    case IMAGE:
		tok = my_strtok ('\0', del_string);
		if (tok == NULL)
		    goto BAD_IMAGE;
		for (i = 0; i < strlen (tok); i++)
		    tok[i] = tolower (tok[i]);
		if (strcmp (tok, catgets (dtsearch_catd, MS_chandel, 34,"all")) == 0)
		    imagemode = INCLUDE;
		else if (strcmp (tok, catgets (dtsearch_catd, MS_chandel, 35, "none")) == 0)
		    imagemode = EXCLUDE;
		else {
BAD_IMAGE:
		    printf ( catgets(dtsearch_catd, MS_chandel, 36,
			"Error line %d: image mode must be 'all' or "
			"'none' -'%s' not recognized.\n") ,
			NULLORSTR(tok));
		    bad_profile = TRUE;
		}
		imageflag = imagemode;
		break;

	    case TEXT:
		tok = my_strtok ('\0', del_string);
		if (tok == NULL)
		    goto BAD_TEXT;
		for (i = 0; i < strlen (tok); i++)
		    tok[i] = tolower (tok[i]);
		if (strcmp (tok, "all") == 0)
		    textmode = INCLUDE;
		else if (strcmp (tok, "none") == 0)
		    textmode = EXCLUDE;
		else {
BAD_TEXT:
		    printf ( catgets(dtsearch_catd, MS_chandel, 37,
			"Error line %d: text mode must be 'all' or "
			"'none' - '%s' not recognized.\n") , NULLORSTR(tok));
		    bad_profile = TRUE;
		}
		textflag = textmode;
		break;

	    case KEYCHAR:
		/*-- get next token - should be character for key type --*/
		if (warnings && key_defined)
		    printf ( catgets(dtsearch_catd, MS_chandel, 38,
			"Warning line %d: Key character redefined.\n") ,
			line_num);
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok))
		    key_char = tok[0];
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 39,
			"Error line %d: invalid Key Character:'%c'.\n") ,
			line_num, (tok)?tok[0]:'?');
		    bad_profile = TRUE;
		}
		key_defined = TRUE;
		break;


	    case DATEFLD:
		if (date_pos_defined) {
		    printf ( catgets(dtsearch_catd, MS_chandel, 110,
			"Warning line %d - date field redefined.\n") ,
			line_num);
		    null_date_specified = FALSE;
		}
		date_table = (struct date_id *) malloc
			(sizeof (struct date_id));
		date_current = date_table;
		date_current->next = NULL;
		tok = my_strtok ('\0', del_string);
		/* validate_id() just does syntax chk on name string */
		if (validate_id (tok))
		    strcpy (date_current->field_id, tok);
		else {
		    /* Msg #111 used two places */
		    printf ( catgets(dtsearch_catd, MS_chandel, 111,
			"Error line %d - bad identifier '%s' for date.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		    break;
		}

		/* Test for special "null" date value */
		if (strcmp (date_current->field_id, "null") == 0) {
		    date_pos_defined = TRUE;
		    null_date_specified = TRUE;
		    break;
		}

		tok = my_strtok ('\0', del_string);
		while (tok != NULL) {
		    date_current->next = (struct date_id *) malloc
			(sizeof (struct date_id));
		    date_current = date_current->next;
		    date_current->next = NULL;
		    if (validate_id (tok))
			strcpy (date_current->field_id, tok);
		    else {
			/* Msg #111 used two places */
			printf ( catgets(dtsearch_catd, MS_chandel, 111,
			    "Error line %d - bad identifier '%s' for date.\n"),
			    line_num, NULLORSTR(tok));
			bad_profile = TRUE;
			break;
		    }
		    tok = my_strtok ('\0', del_string);
		}
		date_pos_defined = TRUE;
		break;		/* end case DATEFLD */

	    case KEY:
		/*-- building the key --*/
		if (warnings && key_pos_defined)
		    printf ( catgets(dtsearch_catd, MS_chandel, 40,
			"Warning line %d - key field redefined.\n") ,
			line_num);
		key_table = (struct key_id *) malloc (sizeof (struct key_id));
		key_current = key_table;
		key_current->next = NULL;
		tok = my_strtok ('\0', del_string);
		if (validate_id (tok)) {
		    if (is_time (tok)) {
			strcpy (key_current->field_id, "time");
			key_pos_defined = TRUE;
			break;
		    } else if (is_count (tok)) {
			strcpy (key_current->field_id, "count");
			key_pos_defined = TRUE;
			break;
		    } else
			strcpy (key_current->field_id, tok);
		}
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 43,
			"Error line %d - bad identifier '%s' for key.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		    break;
		}
		tok = my_strtok ('\0', del_string);
		while (tok != NULL) {
		    key_current->next = (struct key_id *) malloc (
			sizeof (struct key_id));
		    key_current = key_current->next;
		    key_current->next = NULL;
		    if (validate_id (tok))
			strcpy (key_current->field_id, tok);
		    else {
			printf ( catgets(dtsearch_catd, MS_chandel, 43,
			    "Error line %d - bad identifier '%s' for key.\n") ,
			    line_num, tok);
			bad_profile = TRUE;
			break;
		    }
		    tok = my_strtok ('\0', del_string);
		}
		key_pos_defined = TRUE;
		break;

	    case DISCARD:
		tok = my_strtok ('\0', del_string);
		if (tok == NULL)
		    goto BAD_DISCARD;
		for (i = 0; i < strlen (tok); i++)
		    tok[i] = toupper (tok[i]);
		if (!strcmp (tok, "TRUE"))
		    discard = TRUE;
		else if (!strcmp (tok, "FALSE"))
		    discard = FALSE;
		else {
BAD_DISCARD:
		    printf ( catgets(dtsearch_catd, MS_chandel, 45,
			"Error line %d: unknown option for 'discard': "
			"'%s'.\n") , line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		}
		break;

	    case DELBLANKLINES:
		tok = my_strtok ('\0', del_string);
		if (tok == NULL)
		    goto BAD_DELBLANKLINES;
		for (i = 0; i < strlen (tok); i++)
		    tok[i] = toupper (tok[i]);
		if (!strcmp (tok, "TRUE"))
		    del_blanklines = TRUE;
		else if (!strcmp (tok, "FALSE"))
		    del_blanklines = FALSE;
		else {
BAD_DELBLANKLINES:
		    printf ( catgets(dtsearch_catd, MS_chandel, 46,
			"Error line %d: unknown option for "
			"'delblanklines': '%s'.\n") ,
			line_num, NULLORSTR(tok));
		    bad_profile = TRUE;
		}
		break;

	    case ABSTR:
		tok = my_strtok ('\0', del_string);
		abstract = token (tok);
		switch (abstract) {
		    case GENERATE:
			break;
		    case USER:
			tok = my_strtok ('\0', del_string);
			if (tok == NULL)
			    goto BAD_ABSTR;
			strcpy (abstracter, tok);
			break;
		    case FIELDS:
			tok = my_strtok ('\0', del_string);
			while (tok != NULL) {
			    if (abstract_table == NULL) {
				abstract_table = (struct key_id *) malloc
				    (sizeof (struct key_id));
				abstract_current = abstract_table;
			    } else {
				abstract_current->next =
				    (struct key_id *) malloc (
					sizeof (struct key_id));
				abstract_current = abstract_current->next;
			    }
			    strcpy (abstract_current->field_id, tok);
			    abstract_current->next = NULL;
			    tok = my_strtok ('\0', del_string);
			}
			break;
		    default:
BAD_ABSTR:
			printf ( catgets(dtsearch_catd, MS_chandel, 47,
			"Error line %d: Unknown option for abstract :'%s'\n"),
			line_num, NULLORSTR(tok));
			bad_profile = TRUE;
			break;
		}		/* 'abstract' subswitch */
		break;

	    default:
		printf ( catgets(dtsearch_catd, MS_chandel, 48,
			"Error line %d -unknown identifier type '%s'.\n") ,
		    line_num,  NULLORSTR(tok));
		bad_profile = TRUE;
		break;
	}			/* main switch for each line in profile */
    } while (TRUE);		/* read-a-line do loop */
    if (!date_pos_defined)
        fprintf (aa_stderr,  catgets(dtsearch_catd, MS_chandel, 115,
            "%s Default object dates will be '%s'.\n") ,
	    PROGNAME"1288", now_str);
    if (bad_profile)
	return;

    /*---- Process tables, and check for identifiers referenced ----*/
    if (!top_defined && !bot_defined) {
	bad_profile = TRUE;
	printf ( "%s", catgets(dtsearch_catd, MS_chandel, 49,
		"Error - delimiter not defined.\n") );
    }
    if (!key_defined) {
	bad_profile = TRUE;
	printf ( "%s", catgets(dtsearch_catd, MS_chandel, 50,
		"Error - key-type character never defined.\n") );
    }
    if (!key_pos_defined) {
	bad_profile = TRUE;
	printf ( "%s", catgets(dtsearch_catd, MS_chandel, 51,
		"Error - key never defined.\n") );
    }
    if (bad_profile)
	return;

    top_rec = NULL;
    line_current = line_table;
    while (line_current != NULL) {
	if (strcmp (line_current->name, top_rec_name) == 0)
	    top_rec = line_current;
	line_current = line_current->next;
    }
    if (top_rec_name[0] != 0 && top_rec == NULL) {
	printf ( catgets(dtsearch_catd, MS_chandel, 52,
		"Error - delimiter defined as '%s' was never found.\n") ,
	    top_rec_name);
	bad_profile = TRUE;
    } else if (strcmp (top_rec->head->text, line_mode) == 0) {
	printf ( catgets(dtsearch_catd, MS_chandel, 53,
	    "Error - delimiter defined as '%s' references a physical "
	    "line in the record.\n   Since the delimiter defines the "
	    "physical lines\n  it cannot be referenced as a physical line.\n"),
	    top_rec_name);
	bad_profile = TRUE;
    }
    field_current = field_table;
    while (field_current != NULL) {
	found = FALSE;
	line_current = line_table;
	while (line_current != NULL) {
	    if (!strcmp (field_current->line_id, line_current->name)) {
		found = TRUE;
		field_current->line = line_current;
	    }
	    line_current = line_current->next;
	}
	if (!found && !field_current->constant) {
	    printf ( catgets(dtsearch_catd, MS_chandel, 54,
		"Error - for field '%s', no line identifer matches '%s'.\n") ,
		field_current->name, field_current->line_id);
	    bad_profile = TRUE;
	}
	field_current = field_current->next;
    }
    finclude_current = finclude_tab;
    while (finclude_current != NULL) {
	/* find field, and set text_value */
	field_current = field_table;
	found = FALSE;
	while (field_current != NULL) {
	    if (!strcmp (field_current->name, finclude_current->field_id)) {
		field_current->text_action = finclude_current->value;
		found = TRUE;
	    }
	    field_current = field_current->next;
	}
	if (!found) {
	    printf ( catgets(dtsearch_catd, MS_chandel, 55,
		"Error - field include/exclude list included\n"
		"  the field '%s', which was never defined.\n") ,
		finclude_current->field_id);
	    bad_profile = TRUE;
	}
	finclude_current = finclude_current->next;
    }
    i_i_current = i_i_t;
    while (i_i_current != NULL) {
	/* find line, and set text_action */
	line_current = line_table;
	found = FALSE;
	while (line_current != NULL) {
	    if (!strcmp (line_current->name, i_i_current->line_id)) {
		line_current->image_action = i_i_current->value;
		found = TRUE;
	    }
	    line_current = line_current->next;
	}
	if (!found) {
	    printf ( catgets(dtsearch_catd, MS_chandel, 56,
		"Error - image include/exclude list included\n"
		"  the line '%s', which was never defined.\n") ,
		include_current->line_id);
	    bad_profile = TRUE;
	}
	i_i_current = i_i_current->next;
    }
    include_current = include_tab;
    while (include_current != NULL) {
	/* find line, and set text_action */
	line_current = line_table;
	found = FALSE;
	while (line_current != NULL) {
	    if (!strcmp (line_current->name, include_current->line_id)) {
		line_current->text_action = include_current->value;
		found = TRUE;
	    }
	    line_current = line_current->next;
	}
	if (!found) {
	    printf ( catgets(dtsearch_catd, MS_chandel, 57,
		"Error - text include/exclude list included\n"
		"  the line '%s', which was never defined.\n") ,
		include_current->line_id);
	    bad_profile = TRUE;
	}
	include_current = include_current->next;
    }

    /* If "null" dates specified, no need to look for other date fields */
    if (null_date_specified) 
	goto END_DATE_TABLE;

    /* loop thru date table and link each field id to its structure */
    for (date_current = date_table; date_current != NULL;
        date_current = date_current->next) {
        found = FALSE;

        for (field_current = field_table; field_current != NULL;
            field_current = field_current->next) {
            if (strcmp (field_current->name, date_current->field_id) == 0) {
                date_current->field = field_current;
                found = TRUE;
                break;
            }
        }
        if (!found) {
            printf ( catgets(dtsearch_catd, MS_chandel, 116,
		"Error - date references undefined field '%s'.\n"),
                date_current->field_id);
            bad_profile = TRUE;
        }
    }
END_DATE_TABLE:

    key_current = key_table;
    while (key_current != NULL) {
	field_current = field_table;
	found = FALSE;
	if (!strcmp ("time", key_current->field_id)) {
	    found = TRUE;
	    key_current->field = NULL;
	} else if (!strcmp ("count", key_current->field_id)) {
	    found = TRUE;
	    key_current->field = NULL;
	}
	while (field_current != NULL) {
	    if (!strcmp (field_current->name, key_current->field_id)) {
		found = TRUE;
		key_current->field = field_current;
	    } else if (!strcmp ("time", key_current->field_id)) {
		found = TRUE;
		key_current->field = NULL;
	    } else if (!strcmp ("count", key_current->field_id)) {
		found = TRUE;
		key_current->field = NULL;
	    }
	    field_current = field_current->next;
	}
	if (!found) {
	    printf ( catgets(dtsearch_catd, MS_chandel, 58,
		"Error - key definition references field '%s'\n"
		"  which was never defined.\n") ,
		key_current->field_id);
	    bad_profile = TRUE;
	}
	key_current = key_current->next;
    }
    abstract_current = abstract_table;
    while (abstract_current != NULL) {
	field_current = field_table;
	found = FALSE;
	while (field_current != NULL) {
	    if (!strcmp (field_current->name, abstract_current->field_id)) {
		found = TRUE;
		abstract_current->field = field_current;
	    }
	    field_current = field_current->next;
	}
	if (!found) {
	    printf ( catgets(dtsearch_catd, MS_chandel, 59,
		"Error - abstract definition references field '%s'\n"
		"  which was never defined.\n") ,
		abstract_current->field_id);
	    bad_profile = TRUE;
	}
	abstract_current = abstract_current->next;
    }

} /*--process_profile--*/

/**********************************************************************
   CLEANUP - frees memory used by record
***********************************************************************/
void            cleanup ()
{
    struct line_id *line_current;
    struct field_id *field_current;
    struct rec     *record;

    /*-- Reset line_table --*/
    line_current = line_table;
    while (line_current != NULL) {
	line_current->line = UNDEFINED;
	line_current = line_current->next;
    }

    /*-- Reset field table --*/
    field_current = field_table;
    while (field_current != NULL) {
	if (field_current->constant == FALSE)
	    field_current->value[0] = 0;
	field_current = field_current->next;
    }

    /*-- clean up record, free memory for reuse --*/
    record = record_head;
    while (record_head != NULL) {
	record_head = record->next;
	free (record);
	record = record_head;
    }
}

/**************************************************************************
   WRITE_RECORD - writes the final form of the record - key, fzkey, abstract,
 and image.
**************************************************************************/
void            write_record ()
{
    static int	    dotcount = 0;
    char           *ptr;
    struct key_id  *abst;
    struct rec     *record;
    char            value[200];
    int             lvalue;
    int             i;
    int             good = FALSE;
    char            buffer[200];

    /* Line #1 is fzkey */
    fprintf (outstream, " 0,2\n"); /* hardcoded null fzkey */

    /* Line #2 is abstract */
    value[0] = 0;
    if (abstract == FIELDS) {
	abst = abstract_table;
	while (abst != NULL) {
	    strcat (value, abst->field->value);
	    abst = abst->next;
	}
	for (i = 0; i < strlen (value); i++)
	    if (value[i] == '\n')
		value[i] = ' ';
    }
    fprintf (outstream, "ABSTRACT: %s\n", value);

    /* Line #3 is unique database key */
    if (key_value[strlen (key_value) - 1] == '\n')
	key_value[strlen (key_value) - 1] = 0;
    if (uppercase)
	strupr (key_value);
    fprintf (outstream, "%c%s\n", key_char, key_value);

    /* Line #4 is object date in objdate string format.
     * Prior to version 2.0.8 this would be the first line of text.
     */
    if (null_date_specified)
	fputs (NULLDATESTR"\n", outstream);
    else
	fprintf (outstream, "%s\n",
	    objdate2fzkstr (tm2objdate (objdate_tmptr)));

    /* Lines #5 and thereafter (text) of .fzk rec */
    record = record_head;
    while (record != NULL) {
	/*
	 * Strip out any control-l's, as we put our own later,
	 * and any extras might freak out something that wants
	 * a control-l as a delimeter.
	 */
	for (i = 0; i < strlen (record->text); i++)
	    if (record->text[i] == CNTRL_L)
		record->text[i] = ' ';

	lvalue = (record->line)? record->line->image_action : 0;
	switch (lvalue) {
	    case CLEAR:
		imageflag = INCLUDE;
		break;
	    case SET:
		imageflag = EXCLUDE;
		break;
	    default:
		break;
	}
	if ((imageflag == INCLUDE && lvalue != EXCLUDE)
		|| (lvalue == INCLUDE)) {
	    /* trim to fit in screen */
	    strcpy (buffer, record->text);
	    if (strlen (buffer) > screen_width) {
		buffer[screen_width] = '\n';
		buffer[screen_width + 1] = 0;
	    }
	    fprintf (outstream, "%s", buffer);
	}
	record = record->next;
	if (lvalue == DEFAULT)
	    imageflag = imagemode;
    }

    /* Test final record write to check for full filesystem */
    if (fprintf (outstream, "%c\n", CNTRL_L) <= 0) {
	printf ( catgets(dtsearch_catd, MS_chandel, 124,
	    "%s Unable to write to output file '%s':\n  %s\n") ,
	    PROGNAME"1663", outfile, strerror(errno));
	DtSearchExit (2);
    }
    return;
} /* write_record */


/************************************************/
/*						*/
/*		  mmm_to_digits			*/
/*						*/
/************************************************/
/* Translates a field value which is a recognizable month
 * name string into a two-char digit string from "01" to "12".
 */
static void	mmm_to_digits (struct field_id *fld)
{
    static char	valbuf[8];
    static char	*months = NULL;
    int		i;

    if (months == NULL)
	months = strdup (catgets(dtsearch_catd, MS_chandel, 125,
	    "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC"));
    for (i=0; i<3; i++)
	valbuf[i] = toupper (fld->value[i]);
    for (i=0; i<12; i++)
	if (strncmp (valbuf, months + (i*3), 3) == 0) {
	    sprintf (fld->value, "%02d", ++i);
	    break;
	}
return;
} /* mmm_to_digits() */


/************************************************/
/*						*/
/*		  process_record		*/
/*						*/
/************************************************/
/* PROCESS_RECORD - does all processing for the record currently stored
 * in the data structure pointed to by record_head.  If record_head points
 * to NULL, it can safely be assumed that no record is currently awaiting
 * processing.
 */
void	process_record (void)
{
    struct field_id
		*field_current;
    struct key_id
		*key_current;
    struct date_id
		*date_current;
    struct rec	*record;
    char	*pos;
    char	value[200];
    int		lvalue;
    int		i;
    int		linelen;
    char	date_value[256];
    int		dummy;
    int		meaningless;
    static int	dotcount = 0;

    if (record_head == NULL)
	return;

    /* Print progress dots and messages */
    rec_count++;
    if (rec_count % RECS_PER_DOT == 0L) {
	putchar('.');
	if (++dotcount % 10 == 0)
	    putchar(' ');
	if (dotcount % 50 == 0) {
	    putchar('\n');
	    dotcount = 0;
	}
	fflush(stdout);
    }

    discard_record = FALSE;
    meaningless = FALSE;

    /* Main loop on every line in record */
    record = record_head;
    while (record != NULL) {
	lvalue = (record->line)? record->line->text_action : 0;
	switch (lvalue) {
	    case CLEAR:
		textflag = INCLUDE;
		break;
	    case SET:
		textflag = EXCLUDE;
		break;
	    default:
		break;
	}			/* switch */

	field_current = field_table;
	while (field_current != NULL) {
	    if (field_current->line == record->line) {
		/* this field is defined within this line */
		memset (value, 0, sizeof(value));

		/* If profile pattern str was empty ("") ...*/
		if (field_current->text[0] == 0) {
		    if (field_current->defined_length == EOLN)
			strncpy (value, record->text +
			    (field_current->offset - 1), sizeof(value)-1);
		    else if (field_current->defined_length == EOW)
			/* copy until end of word only */
			for (dummy = 0;
			     (!isspace ((record->text +
				(field_current->offset - 1))[dummy]))
				&&  dummy < sizeof(value);
			     dummy++
			     )
				value[dummy] = (record->text +
				    (field_current->offset - 1))[dummy];
		    else {
			i = (field_current->defined_length < sizeof(value))?
			    field_current->defined_length : sizeof(value)-1;
			strncpy (value,
			    record->text + (field_current->offset - 1),
			    i);
		    }
		}

		/* ...else if profile pattern str was not empty ("xxx") */
		else {
		    pos = strstr (record->text, field_current->text);
		    if (pos != NULL) {	/* pattern found... */
			if (field_current->defined_length == EOLN)
			    strncpy (value,
				pos + (field_current->offset - 1),
				sizeof(value) - 1);
			else if (field_current->defined_length == EOW)
			    /* copy until end of word only */
			    for (dummy = 0;
				 (!isspace ((pos +
				    (field_current->offset - 1))[dummy]))
				    &&  dummy < sizeof(value);
				 dummy++
				 )
				    value[dummy] = (pos +
					(field_current->offset - 1))[dummy];
			else {
			    i = (field_current->defined_length<sizeof(value))?
				field_current->defined_length : sizeof(value)-1;
			    strncpy (value,
				pos + (field_current->offset - 1),
				i);
			}
		    }	/* end pattern found */
		} /* end else where pattern str not empty */

		/* strip \n's out of value */
		for (i = 0; i < strlen (value); i++)
		    if (value[i] == '\n')
			value[i] = 0;
		if (field_current->constant == FALSE) {
		    strncpy (field_current->value, value,
			sizeof(field_current->value));
		    field_current->value [sizeof(field_current->value)-1] = 0;
		    field_current->length = strlen (field_current->value);
		    if (field_current->is_month)
                        mmm_to_digits (field_current);
		}
	    }
	    field_current = field_current->next;
	} /* end while loop on each field within each line */

	if (lvalue == DEFAULT)
	    textflag = textmode;
	record = record->next;
    } /* end while loop on each record line */


    /* Build a handel date_value string from specified fields.
     * If 'date' was not specified, uses current date/time.
     * If 'date = null' was specified, uses special constant string.
     * If value error in specified date fields,
     * uses current date/time and prints err msg. 
     */
    objdate_tmptr = &nowtm;		/* default */
    if (date_pos_defined && !null_date_specified) {
	date_value[0] = 0;
	for (date_current = date_table; date_current != NULL;
	    date_current = date_current->next) {
	    if (date_current->field->value[0] != 0) {
		strcat (date_value, date_current->field->value);
	    }
	    else {
		date_value[0] = 0;	/* flags error msg */
		break;
	    }
	}

	/*
	 * Validate format for date_value of this record.
	 * Date value format: YYYYMMDDhhmm (exactly 12 digits).
	 * The area at date_value + 100 is just a little atoi buffer. 
	 */
	if (date_value[0] == 0)
	    goto BAD_DATE_VALUE;
	if (strlen (date_value) != 12)
	    goto BAD_DATE_VALUE;
	for (i = 0; i < 12; i++)
	    if (!isdigit (date_value[i]))
		goto BAD_DATE_VALUE;

	/* year = YYYY */
	strncpy (date_value + 100, date_value, 4);
	date_value[104] = 0;
	i = atoi (date_value + 100);
	if (i < 1900 || i > 5995)	/* valid OBJDATE years */
	    goto BAD_DATE_VALUE;
	else
	    rectm.tm_year = i - 1900;

	/* month = MM */
	strncpy (date_value + 100, date_value + 4, 2);
	date_value[102] = 0;
	i = atoi (date_value + 100);
	if (i < 1 || i > 12)
	    goto BAD_DATE_VALUE;
	else
	    rectm.tm_mon = i - 1;	/* tm values = 0 - 11 */

	/* day = DD */
	strncpy (date_value + 100, date_value + 6, 2);
	date_value[102] = 0;
	i = atoi (date_value + 100);
	if (i < 1 || i > 31)
	    goto BAD_DATE_VALUE;
	else
	    rectm.tm_mday = i;

	/* hours = hh */
	strncpy (date_value + 100, date_value + 8, 2);
	date_value[102] = 0;
	i = atoi (date_value + 100);
	if (i < 0 || i > 23)
	    goto BAD_DATE_VALUE;
	else
	    rectm.tm_hour = i;

	/* minutes = mm */
	strncpy (date_value + 100, date_value + 10, 2);
	date_value[102] = 0;
	i = atoi (date_value + 100);
	if (i < 0 || i > 59)
	    goto BAD_DATE_VALUE;
	else
	    rectm.tm_min = i;

	objdate_tmptr = &rectm;
	goto GOOD_DATE_VALUE;

    BAD_DATE_VALUE:
	objdate_tmptr = &nowtm;
	printf ( catgets(dtsearch_catd, MS_chandel, 133,
	    "Warning - '%s' is invalid date specification.\n"
	    "  Using '%s' date for record number %ld that began: %.30s\n") ,
	    date_value, now_str, rec_count, record_head->text);
    GOOD_DATE_VALUE:
	;
    }	/* end if (date_pos_defined) */

    key_current = key_table;
    key_count++;
    for (dummy = 0; dummy < 80; dummy++)
	key_value[dummy] = 0;
    dummy = FALSE;
    while (key_current != NULL) {
	if (key_current->field == NULL) {
	    if (strcmp (key_current->field_id, "time") == 0)
		sprintf (key_value, "%ld%06ld", now, key_count);
	    else	/* must be 'count' */
		sprintf (key_value, "%09ld", key_count);
	} else if (key_current->field->value[0] != 0) {
	    strcat (key_value, key_current->field->value);
	} else {
	    dummy = TRUE;
	}
	key_current = key_current->next;
    }
    if (dummy && warnings) {
	printf ( catgets(dtsearch_catd, MS_chandel, 68,
		"Warning - fields necessary for key not found.\n"
		"  discarding record #%ld that began:\n  %s\n") ,
	    rec_count, record_head->text);
    } else if (discard && meaningless && warnings) {
	printf ( catgets(dtsearch_catd, MS_chandel, 69,
		"Warning - record #ld deemed meaningless, discarding...\n"
		"  record began: %.60s\n") ,
	    rec_count, record_head->text);
    } else {
	outcount++;
	write_record ();
    }
    cleanup ();
} /*--process_record--*/


/**************************************************************************
   NEW_REC - determines if the string sent represents a new record or
  not.
**************************************************************************/
int             new_rec (char *buffer)
{
    int             cant_be = FALSE;
    top_rec->comp = top_rec->head;
    while (top_rec->comp != NULL) {
	if (strcmp (top_rec->comp->text, line_mode) != 0) {
	    if (top_rec->comp->column_number == ANY) {
            if (strstr(buffer,top_rec->comp->text)==NULL) 
		    cant_be = TRUE;
	    }
	    /* if */
	    else if (strncmp (buffer + (top_rec->comp->column_number - 1),
			top_rec->comp->text,
		    strlen (top_rec->comp->text)) != 0)
		cant_be = TRUE;
	}			/* if */
	top_rec->comp = top_rec->comp->next;
    }				/* while */
    if (cant_be == FALSE) {
	return TRUE;
    } else
	return FALSE;
}				/*--new_rec--*/

/**************************************************************************
   IS_WHITESPACE - returns true if the string passed contains only
 whitespace
**************************************************************************/
int             is_whitespace (char *s)
{
    int             i;
    for (i = 0; i < strlen (s); i++)
	if (!isspace (s[i]) || s[i] == 12)
	    return FALSE;
    return TRUE;
}

/**************************************************************************
   PROCESS_INFILE - processes the input file, 1 record at a time.  For each
record, the lines must first be compared against the line id's we have, to
try and identify each line.  Then fields are processed against these lines,
and finally fzk processing can begin on any lines that are indicated being
important.
***************************************************************************/
void            process_infile ()
{
    int             line_num = 0;
    struct line_id *line_current;
    struct rec     *record;
    char            buffer[200];
    int             cant_be;
    time_t          startime = 0L;
    int             i;
    int             rc;

    if (!strcmp (infile, "-"))
	instream = stdin;
    else {
	if ((instream = fopen (infile, "rt")) == NULL) {
	    printf ( catgets(dtsearch_catd, MS_chandel, 70,
		" Unable to open input file '%s'.\n") , infile);
	    exit (FILE_ERROR);
	}
    }

    record_head = NULL;
    time (&startime);
    while (fgets (buffer, sizeof (buffer) - 1, instream) != NULL) {
	/* clean any non ASCII characters out of buffer */
	delete_whitespace (buffer);
	if (feof (instream))
	    continue;		/*-* end of file *-*/
	if (del_blanklines && is_whitespace (buffer))
	    continue;
	if (new_rec (buffer)) {
	    if (bot_defined) {
		if (record_head == NULL) {
		    record_head = (struct rec *) malloc (sizeof (struct rec));
		    record = record_head;
		} else {
		    record->next = (struct rec *) malloc (sizeof (struct rec));
		    record = record->next;
		}
		record->next = NULL;
		line_num++;
		record->line_num = line_num;
		strcpy (record->text, buffer);
		record->line = NULL;
		/*-- search list of line_id's for a possible match --*/
		line_current = line_table;
		while (line_current != NULL) {
		    cant_be = FALSE;
		    line_current->comp = line_current->head;
		    while (line_current->comp != NULL) {
			if (strcmp (line_current->comp->text, line_mode) != 0) {
			    if (line_current->comp->column_number == ANY) {
	                        if (strstr(buffer,line_current->comp->text)
					== NULL)
				    cant_be = TRUE;
			    } else if (
				    strncmp (buffer +
					(line_current->comp->column_number - 1),
					line_current->comp->text,
				    strlen (line_current->comp->text)) != 0)
				cant_be = TRUE;
			} else if (line_current->comp->column_number
				!= line_num)
			    cant_be = TRUE;
			line_current->comp = line_current->comp->next;
		    }
		    if (cant_be == FALSE) {
			/* found a hit, set pointers */
			line_current->line = record;
			record->line = line_current;
		    }		/* if */
		    line_current = line_current->next;
		}		/* while - line_current loop */
	    }			/* if bot_defined */
	    process_record ();
	    if (shutdown_now)
		break;
	    line_num = 0;
	    started = TRUE;
	    if (bot_defined)
		continue;
	}
	if (record_head == NULL) {
	    record_head = (struct rec *) malloc (sizeof (struct rec));
	    record = record_head;
	} else {
	    record->next = (struct rec *) malloc (sizeof (struct rec));
	    record = record->next;
	}
	record->next = NULL;
	line_num++;
	record->line_num = line_num;
	strcpy (record->text, buffer);
	record->line = NULL;
	/*-- search list of line_id's for a possible match --*/
	line_current = line_table;
	while (line_current != NULL) {
	    cant_be = FALSE;
	    line_current->comp = line_current->head;
	    while (line_current->comp != NULL) {
		if (strcmp (line_current->comp->text, line_mode) != 0) {
		    if (line_current->comp->column_number == ANY) {
                  if (strstr(buffer,line_current->comp->text)==NULL){
			    cant_be = TRUE;
			}
		    } else if (strncmp (buffer +
				    (line_current->comp->column_number - 1),
				line_current->comp->text,
			    strlen (line_current->comp->text)) != 0) {
			cant_be = TRUE;
		    }
		} else if (line_current->comp->column_number != line_num) {
		    cant_be = TRUE;
		}
		line_current->comp = line_current->comp->next;
	    }
	    if (cant_be == FALSE) {
		/* found a hit, set pointers */
		line_current->line = record;
		record->line = line_current;
	    }			/* if */
	    line_current = line_current->next;
	}			/* while - line_current loop */
	for (rc = 0; rc < 200; rc++)
	    buffer[rc] = 0;
    }				/* while - buffer read loop */
    /*- check for any leftover records -*/
    process_record ();
}   /*-- process_infile --*/


/************************************************/
/*						*/
/*		    usage_msg			*/
/*						*/
/************************************************/
static void	usage_msg (void)
{
    static char       *default_text =
      "\nUSAGE: %s [options] <profile> <infile> [<outfile>]\n"
      "  -m         Turn off all but error messages.\n"
      "  -wN        Change target screen width to <N>.\n"
      "  -oo        Preapprove overwrite of outfile.\n"
      "  -oa        Preapprove append to outfile.\n"
      "  <profile>  Input file containing profile of records\n"
      "             to be processed.\n"
      "  <infile>   Input file containing actual records.\n"
      "  <outfile>  Output file name, .fzk format (%s).\n\n";

    if (!warnings)
	return;
    printf (catgets (dtsearch_catd, MS_chandel, 71, default_text),
      aa_argv0, EXT_FZKEY);
    return;
} /* usage_msg() */


/************************************************/
/*						*/
/*		user_arg_processor		*/
/*						*/
/************************************************/
/* Process any user arguments passed thru the command line parameters. */
static void	user_arg_processor (int argc, char **argv)
{
    char	*cptr, *argptr;
    char	*pos;

    if (argc <= 1) {
	bad_parm = TRUE;
	return;
    }				/* if */
    while (--argc > 0 && (*++argv)[0] == '-') {
	argptr = argv[0];
	argptr[1] = tolower (argptr[1]);
	switch (argptr[1]) {

	    case 'w':
		if ((screen_width = atoi (argptr + 2)) == 0) {
		    printf ( "%s", catgets(dtsearch_catd, MS_chandel, 72,
			"Invalid screen width specified.\n") );
		    bad_parm = TRUE;
		}
		break;

	    case 'm':
		warnings = FALSE;
		break;

	    case 'o':
		argptr[2] = tolower (argptr[2]);
		if (argptr[2] == 'o')
		    strcpy (outmode, "w");
		else if (argptr[2] == 'a')
		    strcpy (outmode, "a");
		else {
		    printf ( catgets(dtsearch_catd, MS_chandel, 75,
			"'%s' is invalid output mode.\n") , argptr);
		    bad_parm = TRUE;
		}		/* else */
		outmode_specified = TRUE;
		break;

	    default:
		printf ( catgets(dtsearch_catd, MS_chandel, 76,
			"Unknown command line argument '%s'.\n") , argptr);
		bad_parm = TRUE;
	}			/*--switch--*/
    }				/*--while--*/

    if (argc-- <= 0) {
	printf ( "%s", catgets(dtsearch_catd, MS_chandel, 77,
		"Missing required profile-file name.\n") );
	bad_parm = TRUE;
    }
    /* if */
    else {
/******
    strcpy(profile,argv[0]);
    profile=argv[0];
*******/
	profile = (char *) malloc (sizeof (char) * (strlen (argv[0])) +10);
	strcpy (profile, argv[0]);
	if (strchr (profile, '.') == NULL)
	    strcat (profile, EXT_HANDEL);
    }

    if (argc-- <= 0) {
	printf ( "%s", catgets(dtsearch_catd, MS_chandel, 78,
		"Missing required input-file name.\n") );
	bad_parm = TRUE;
    }
    /* if */
    else
/********    strcpy(infile,argv[1]); ********/
	infile = argv[1];

    if (argc-- <= 0) {
/*****************
      strcpy(outfile,infile);
      if (strchr(outfile,'.')!=NULL)
         strcpy(strchr(outfile,'.'), EXT_FZKEY);
      else
         strcat(outfile,EXT_FZKEY);
*****************/
	if (strcmp (infile, "-") == 0) {
	    printf ( "%s", catgets(dtsearch_catd, MS_chandel, 79, "Error - using "
		"stdin as input, output filename is required!\n") );
	    exit (FILE_ERROR);
	} else {
	    outfile = (char *) malloc (sizeof (char) * (strlen (infile) + 10));
	    strcpy (outfile, infile);
	    pos = strrchr (outfile, (int) '.');
	    if (pos == NULL)
		strcat (outfile, EXT_FZKEY);
	    else {
		pos++;
		pos[0] = 'f';
		pos++;
		pos[0] = 'z';
		pos++;
		pos[0] = 'k';
		pos++;
		pos[0] = 0;
	    }
	}
    }
    /* if */
    else
/*    strcpy(outfile,argv[2]); */
	outfile = argv[2];

/*-- Sanity checks --*/
    /*-- duplicates? --*/
    if (strcmp (infile, profile) == 0) {
	printf ( catgets(dtsearch_catd, MS_chandel, 80,
		"Profile file and input file have same name:'%s'.\n") ,
	    infile);
	bad_parm = TRUE;
    }				/* if */
    if (strcmp (infile, outfile) == 0 && strcmp (infile, "-")) {
	printf ( catgets(dtsearch_catd, MS_chandel, 81,
		"Input file and output file have same name:'%s'.\n") ,
	    infile);
	bad_parm = TRUE;
    }				/* if */
    if (strcmp (profile, outfile) == 0) {
	printf ( catgets(dtsearch_catd, MS_chandel, 82,
		"Profile file and output file have same name:'%s'.\n") ,
	    profile);
	bad_parm = TRUE;
    }				/* if */
    if(warnings)
	{
	printf ( catgets(dtsearch_catd, MS_chandel, 83,
		"        Profile file: %s\n") , profile);
	if (strcmp (infile, "-") == 0)
	    printf ( "%s", catgets(dtsearch_catd, MS_chandel, 84,
		"          Input file: stdin\n") );
	else
	    printf ( catgets(dtsearch_catd, MS_chandel, 85,
		"          Input file: %s\n") , infile);
	if (strcmp (outfile, "-") == 0)
	    printf ( "%s", catgets(dtsearch_catd, MS_chandel, 86,
		"         Output file: stdout\n") );
	else
	    printf ( catgets(dtsearch_catd, MS_chandel, 87,
		"         Output file: %s\n") , outfile);
	}
}				/*--user_args_processor--*/

/****************************************************************************
     FLAG_SHUTDOWN - signal handler - to allow for graceful exiting
****************************************************************************/
static void     flag_shutdown (int sig)
{
                    shutdown_now = TRUE;
}

/****************************************************************************
     MAIN - Body of Handel.  Checks user parameters, processes the profile
 file, and then goes into the main loop, running each record from the input
 file against the profile and thru the text processor.  
*****************************************************************************/
int             main (int argc, char **argv)
{
    int		oops;

    /*-- Initialization --*/
    aa_argv0 = argv[0];
    setlocale (LC_ALL, "");
    dtsearch_catd = catopen (FNAME_DTSRCAT, 0);
    printf ( catgets(dtsearch_catd, MS_chandel, 88,
	"%s. %s %s Text Filter.\n") ,
	aa_argv0, PRODNAME, DtSrVERSION);

    time (&now);
    memcpy (&nowtm, localtime(&now), sizeof(struct tm));
    strftime (now_str, sizeof(now_str), "%y/%m/%d~%H:%M", &nowtm);
    memset (&rectm, 0, sizeof(struct tm));

    /* check user arguments */
    user_arg_processor (argc, argv);
    if (bad_parm) {
	usage_msg ();
	exit (USAGE);
    }

    process_profile ();
    if (warnings && !bad_profile) {
	signal (SIGINT, flag_shutdown);
	signal (SIGTERM, flag_shutdown);
	open_outfile ();
	printf ( catgets(dtsearch_catd, MS_chandel, 89,
		"\nInterrupt with CTRL-C to exit gracefully "
		"at record boundary.\n  Each dot is %ld records...\n"),
	    RECS_PER_DOT);
	process_infile ();	/* process the input file */
	oops = fclose (outstream);
	fclose (instream);
	if (oops < 0) {
	    printf ( "%s", catgets(dtsearch_catd, MS_chandel, 90,
		"\nError closing output file - disk full?\n") );
	    exit (FILE_ERROR);
	}
    }
    else {
	printf ( "%s", catgets(dtsearch_catd, MS_chandel, 91,
		"Quitting due to errors in profile file.\n") );
	exit (BAD_PROFILE);
    }
    printf ( catgets(dtsearch_catd, MS_chandel, 92,
	"\n%s: Normal completion.  %ld records processed.  Exit code = 0.\n"),
	aa_argv0, rec_count);
    return 0;
}
/*************** DTSRHAN.C ****************/
