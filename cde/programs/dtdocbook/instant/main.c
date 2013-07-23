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
 *  Copyright 1993 Open Software Foundation, Inc., Cambridge, Massachusetts.
 *  All rights reserved.
 */
/*
 * Copyright (c) 1994  
 * Open Software Foundation, Inc. 
 *  
 * Permission is hereby granted to use, copy, modify and freely distribute 
 * the software in this file and its documentation for any purpose without 
 * fee, provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation.  Further, provided that the name of Open 
 * Software Foundation, Inc. ("OSF") not be used in advertising or 
 * publicity pertaining to distribution of the software without prior 
 * written permission from OSF.  OSF makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is" 
 * without express or implied warranty. 
 */
/* ________________________________________________________________________
 *
 *  Program to read an SGML document instance, creating any of several things:
 *
 *	"translated" output for formatting applications (given a trans. spec)
 *	validation report (given a appropriate trans spec)
 *	tree of the document's structure
 *	statistics about the element usage
 *	summary of the elements used
 *	context of each element used
 *	IDs of each element
 *
 *  A C structure is created for each element, which includes:
 *	name, attributes, parent, children, content
 *  The tree is descended, and the desired actions performed.
 *
 *  Takes input from James Clark's "sgmls" program (v. 1.1).
 * ________________________________________________________________________
 */

#ifndef lint
static char *RCSid =
  "$XConsortium: main.c /main/14 1996/11/27 11:47:17 cde-hp $";
static char *CopyRt =
 "Copyright 1993 Open Software Foundation, Inc., Cambridge, Mass.  All rights reserved.";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <locale.h>
#include "LocaleXlate.h"
#include "XlationSvc.h"


#define STORAGE
#include "general.h"

/* for backwards compatibility */
#ifndef _MAXNAMLEN
#define _MAXNAMLEN MAXNAMLEN
#endif

static int	do_context, do_tree, do_summ, do_stats, do_validate, do_idlist;
static char	*this_prog;
static char	*in_file, *out_file;
static char	*tranfile;
static char	**cmapfile, **sdatafile;
static char	*start_id;
static char	*last_file;
static int	last_lineno;

/* forward references */
static void	HandleArgs(int, char *[]);
static void	Initialize1();
static void	Initialize2();
static void	ReadInstance(char *);
static void	DoHelpMessage();
extern void	Browse();
static int TclPrintLocation(ClientData clientData,
			    Tcl_Interp *interp,
			    int argc,
			    char *argv[]);
static int DefaultOutputString(ClientData clientData,
			       Tcl_Interp *interp,
			       int argc,
			       char *argv[]);
static int CompareI18NStrings(ClientData clientData,
			      Tcl_Interp *interp,
			      int argc,
			      char *argv[]);
static int TclReadLocaleStrings(ClientData clientData,
			        Tcl_Interp *interp,
			        int argc,
			        char *argv[]);
char		*GetOutFileBaseName();

char *
GetCLocale()
{
_DtXlateDb   myDb = NULL;
char         myPlatform[_DtPLATFORM_MAX_LEN+1];
static char  locale[] = "C.ISO-8859-1";
char        *newLocale;
int          execVer;
int          compVer;
int          ret;

if ((_DtLcxOpenAllDbs(&myDb) != 0)) {
    fprintf(stderr,
            "Warning: could not open databases.\n");
    exit(1);
}

ret = _DtXlateGetXlateEnv(myDb,myPlatform,&execVer,&compVer);
if (ret != 0) {
    fprintf(stderr,
            "Fatal: could not open locale translation database. %d\n", ret);

    exit(1);
}

if (_DtLcxXlateStdToOp(myDb,
			myPlatform,
			execVer,
			DtLCX_OPER_SETLOCALE,
			locale,
			NULL,
			NULL,
			NULL,
			&newLocale)) {
	fprintf(stderr,
		"Warning: could not translate C.ISO-8859-1 to local locale\n");
}

_DtLcxCloseDb(&myDb);

return newLocale;
}

/* ______________________________________________________________________ */
/*  Program entry point.  Look at args, read instance, dispatch to the
 *  correct routines to do the work, and finish.
 */
int
main(
    int		  ac,
    char	 *av[]
)
{
    char        **thisopt;
    Initialize1(av[0]);
    HandleArgs(ac, av);
    Initialize2();

    /* use the current locale for all text but use American English ... */
    setlocale(LC_ALL, "");

    /* ... in expressions (e.g., leave "." as the radix operator) */
    setlocale(LC_NUMERIC, GetCLocale());

    /* Create a Tcl interpreter. */
    interpreter = Tcl_CreateInterp();

    /* Add our output string routine as the default output string routine. */
    Tcl_CreateCommand(interpreter,
		      "OutputString",
		      DefaultOutputString,
		      0,
		      0);

    /* Add a hook so the interpreter can print the location in the
     * source file for user errors */
    Tcl_CreateCommand(interpreter,
                      "PrintLocation",
		      TclPrintLocation,
		      0,
		      0);

    /* Add a function to the interpreter to compare to strings.  Our
     * comparison will unmung any i18n characters (see
     * {Un}EscapeI18NChars()) and uppercase the strings before
     * comparison to insure we get a dictionary sort.  We also use the
     * nl_strcmp() function to get proper i18n collation */
    Tcl_CreateCommand(interpreter,
                      "CompareI18NStrings",
		      CompareI18NStrings,
		      0,
		      0);

    /* Add a function to read a localized set of data from a file.
     * We'll make sure the munging takes place so we can parse it
     * in Tcl and any strings we get will output properly when
     * unmunged. */
    Tcl_CreateCommand(interpreter,
                      "ReadLocaleStrings",
		      TclReadLocaleStrings,
		      0,
		      0);
    
    ReadInstance(in_file);

    if (interactive) {
	Browse();	/* this will handle interactive commands */
    }
    else {
	/* Perform tasks based on command line flags... */
	if (tranfile) {
	    Element_t *e;
	    /* If user wants to start at a particular ID, point to that
	     * element.  Else, point to the top of the tree. */
	    if (start_id) {
		if (!(e=FindElemByID(start_id))) {
		    fprintf(stderr, "Error: Can not find element with ID %s\n",
			start_id);
		    exit(1);
		}
	    }
	    else e = DocTree;
	    if (sdatafile)
		{
		thisopt = sdatafile;
		while (*thisopt)
		    {
		    ReadSDATA(*thisopt);
		    free(*thisopt++);
		    }
		free(sdatafile);
		}
	    if (cmapfile)
		{
		thisopt = cmapfile;
		while (*thisopt)
		    {
		    ReadCharMap(*thisopt);
		    free(*thisopt++);
		    }
		free(cmapfile);
		}
	    DoTranslate(e, tranfile, outfp);
	}
	if (do_summ)		PrintElemSummary(DocTree);
	if (do_tree)		PrintElemTree(DocTree);
	if (do_stats)		PrintStats(DocTree);
	if (do_context)		PrintContext(DocTree);
	if (do_idlist)		PrintIDList();
    }
    if (out_file && outfp) fclose(outfp);

    return 0;
}


/* Undo the munging done in EscapeI18NChars().
 *
 * The parameter may be modified.  It is returned for the convenience
 * of the caller.
 *
 * The algorithm here is:
 *
 *  get the next byte to write;
 *
 *      if the current byte is the chosen character:
 *
 *          get the next byte;
 *
 *          if the current byte is the chosen character:
 *
 *              get the next byte and zero out the 8th bit;
 *
 *              if the current byte is an ASCII "1", emit the chosen
 *              character and continue;
 *
 *              if the current byte is an ASCII "0", emit the chosen
 *              character with the 8th bit turned off and continue;
 *
 *              it's an internal error if we get here
 *
 *          emit the current byte with the 8th bit turned off and
 *          continue;
 *
 *      emit the current byte and continue;
 */
static char *UnEscapeI18NChars(
    char *source
)
{
    unsigned char c;
    char *buf;
    unsigned char *to, *from;

    if (MB_CUR_MAX != 1) {
	from = (unsigned char*)source;
	buf = malloc(strlen(source)+1);
        to = (unsigned char *)buf;
	while (c = *from++) {
	    if (c == I18N_TRIGGER) {
		c = *from++;
		if (c == I18N_TRIGGER) {
		    c = *from++ & ~0x80;
		    if (c == '0') {
			*to++ = I18N_TRIGGER & ~0x80;
		    } else if (c == '1') {
			*to++ = I18N_TRIGGER;
		    } else {
			fprintf(stderr,
				"Error: Unexpected I18N transformation.\n");
			exit(1);
		    }
		} else {
		    *to++ = c & ~0x80;
		}
	    } else {
		*to++ = c;
	    }
	}
	*to = 0;
	strcpy(source, buf);
	free(buf);
    }
    return source;
}


static int DefaultOutputString(ClientData clientData,
			       Tcl_Interp *interp,
			       int argc,
			       char *argv[])
{
#define LOCAL_BUFFER_LENGTH 128
    char *string, *pString, *pArgv;
    char localBuffer[LOCAL_BUFFER_LENGTH];
    int retCode, stringLength;

    if (argc < 2) {
	Tcl_SetResult(interpreter, "Missing string to output", TCL_VOLATILE);
	return TCL_ERROR;
    }

    if (argc > 2) {
	Tcl_SetResult(interpreter, "Too many arguments", TCL_VOLATILE);
	return TCL_ERROR;
    }

    /* leave room for worst case expansion plus quotes plus null */
    pArgv = argv[1];
    stringLength = (2 * strlen(pArgv)) + 3;

    /* try to use automatic buffer; use malloc if string is too large */
    if (stringLength <= LOCAL_BUFFER_LENGTH) {
	string = localBuffer;
    } else {
	string = malloc(stringLength);
    }
    pString = string;


    /* wrap the string in quotes and copy argv[1] over escaping
     * any characters that will throw Tcl for a loop */
    *pString++ = '"';
    while (*pArgv) {
	switch (*pArgv) {
	    case '{':
	    case '}':
	    case '"':
	    case '\'':
	    case '[':
	    case ']':
	    case '$':
	    case '\\':
		*pString++ = '\\';
	}
	*pString++ = *pArgv++;
    }
    *pString++ = '"';
    *pString++ = 0;

    /* put the string to the output */
    retCode = Tcl_VarEval(interpreter, "puts -nonewline ", string, 0);

    /* free the string if we're not using the automatic buffer */
    if (string != localBuffer) {
	free(string);
    }

    /* and ripple up any error code we got from the "puts" */
    return retCode;
}


static int CompareI18NStrings(ClientData clientData,
			      Tcl_Interp *interp,
			      int argc,
			      char *argv[])
{
    int   ret_val, len;
    char *ret_string, *cp;

    if (argc < 3) {
	Tcl_SetResult(interpreter,
		      "Missing string(s) to compare",
		      TCL_VOLATILE);
	return TCL_ERROR;
    }

    if (argc > 3) {
	Tcl_SetResult(interpreter, "Too many arguments", TCL_VOLATILE);
	return TCL_ERROR;
    }

    /* unmung the two strings (see {Un}EscapeI18NChars()) */
    UnEscapeI18NChars(argv[1]);
    UnEscapeI18NChars(argv[2]);

    /* upper case the strings to insure a dictionary sort */
    cp = argv[1];
    while (*cp) {
	if ((len = mblen(cp, MB_CUR_MAX)) == 1) {
	    if (isalpha(*cp)) {
		*cp = toupper(*cp);
	    }
	    cp++;
	} else {
	  if (len > 0)
	    cp += len;
	  else
	    break; /* JET - we should be done here... */
	}
    }
    cp = argv[2];
    while (*cp) {
	if ((len = mblen(cp, MB_CUR_MAX)) == 1) {
	    if (isalpha(*cp)) {
		*cp = toupper(*cp);
	    }
	    cp++;
	} else {
	  if (len > 0)
	    cp += len;
	  else
	    break; /* JET - we should be done here... */
	}
    }

    /* compare the strings using an I18N safe sort */
    ret_val = strcoll(argv[1], argv[2]);
    if (ret_val > 0) {
	ret_string = "1";
    } else if (ret_val < 0) {
	ret_string = "-1";
    } else {
	ret_string = "0";
    }

    Tcl_SetResult(interpreter, ret_string, TCL_VOLATILE);

    return TCL_OK;
}


static int TclPrintLocation(ClientData clientData,
			    Tcl_Interp *interp,
			    int argc,
			    char *argv[])
{
    if (argc > 1) {
	Tcl_SetResult(interpreter, "Too many arguments", TCL_VOLATILE);
	return TCL_ERROR;
    }

    PrintLocation(tclE, stderr);

    return TCL_OK;
}


/* ______________________________________________________________________ */
/* Initialization stuff done before dealing with args.
 *  Arguments:
 *	Name of program (string).
 */

static void
Initialize1(
    char	*myname
)
{
    time_t	tnow;
    struct tm	*nowtm;
    char	*cp, buf[100];
#if HPUX
    extern	int gethostname(char *, int);	/* not in a system .h file... */
#endif

    /* where we try to find data/library files */
    if (!(tpt_lib=getenv(TPT_LIB))) tpt_lib = DEF_TPT_LIB;

    /* set some global variables */
    warnings  = 1;
    fold_case = 1;
    this_prog = myname;

    /* setup global variable mapping */
    Variables = NewMap(IMS_variables);

    /* set some pre-defined variables */
    SetMappingNV(Variables, "user", (cp=getenv("USER")) ? cp : "UnknownUser" );
    time(&tnow);
    nowtm = localtime(&tnow);
    strftime(buf, 100, "%a %d %b %Y, %R", nowtm);
    SetMappingNV(Variables, "date", buf);
#if HPUX
    if (gethostname(buf, 100) < 0) strcpy(buf, "unknown-host");
#else
    strcpy(buf, "unknown-host");
#endif
    SetMappingNV(Variables, "host", buf);
    SetMappingNV(Variables, "transpec", tranfile ? tranfile : "??");
}

/* Initialization stuff done after dealing with args. */

static void
Initialize2()
{
    SetMappingNV(Variables, "transpec", tranfile ? tranfile : "??");

    /* If the user wants to send output to a file, reopen stdout as
     * the file.  The interpreter will write its output by default to
     * stdout so the reopen makes sure the output goes to the desired
     * named file. */
    if (do_validate)
	out_file = "/dev/null"; /* toss all but error output */

    if (!out_file) {
	out_file = "out.sdl";
    }

    if (!(outfp = freopen(out_file, "w", stdout))) {
	fprintf(stderr,
		"Could not re-open output '%s' file for writing.\n%s",
		out_file,
		strerror(errno));
	exit(1);
    }
    SetMappingNV(Variables, "basename", GetOutFileBaseName());
}

/* ______________________________________________________________________ */
/* Get the base name of the output file (for transpec "basename" command)
 *  Arguments:
 *
 */
char *GetOutFileBaseName()
{
char *prefix, *suffix, *cp;
static char nameBuf[_MAXNAMLEN+1] = "";

prefix = strchr(out_file, '/');
if (!prefix)
    prefix = out_file;
else
    prefix++;

suffix = strrchr(prefix, '.');
if (!suffix)
    suffix = prefix + strlen(prefix);

if (!*(cp = nameBuf)) {
    while (prefix != suffix) {
	*cp++ = *prefix++;
    }
    *cp = 0;
    }
return nameBuf;
}

/* ______________________________________________________________________ */
/*  Set a variable.  If it is one of the "known" variables, set the
 *  variable in the C code (this program).
 *  Arguments:
 *	Variable name/value string - separated by an '=' (eg, "myname=Sally").
 */
static void
CmdLineSetVariable(
    char	*var
)
{
    char	*cp, buf[100], **tok;
    int		n;

    /* Turn '=' into a space, to isolate the name.  Then set variable. */
    strcpy(buf, var);
    if ((cp=strchr(buf, '='))) {
	/* we have "var=value" */
	*cp = ' ';
	n = 2;
	tok = Split(buf, &n, 0);
	/* see if variable name matches one of our internal ones */
	if (!strcmp(tok[0], "verbose"))		verbose   = atoi(tok[1]);
	else if (!strcmp(tok[0], "warnings"))	warnings  = atoi(tok[1]);
	else if (!strcmp(tok[0], "foldcase"))	fold_case = atoi(tok[1]);
	else SetMappingNV(Variables, tok[0], tok[1]);
    }
    else {
	fprintf(stderr, "Expected an '=' in variable assignment: %s. Ignored\n",
		var);
    }
}

/* ______________________________________________________________________ */
/*  Bounce through arguments, setting variables and flags.
 *  Arguments:
 *	Argc and Argv, as passed to main().
 */
static void
HandleArgs(
    int		 ac,
    char	*av[]
)
{
    int		 c, errflag=0;
    extern char	*optarg;
    extern int	 optind;
    char       **thisopt;
    int          count;

    while ((c=getopt(ac, av, "t:vc:s:o:huSxIl:bHVWi:D:Z")) != EOF) {
	switch (c) {
	    case 't': tranfile		= optarg;	break;
	    case 'v': do_validate	= 1;		break;
	    case 's':
		{
		if (thisopt = sdatafile)
		    {
		    count = 0;
		    while (*thisopt++)
			count++;
		    sdatafile = (char **)
			realloc(sdatafile, (count+2)*sizeof(char *));
		    sdatafile[count+1] = 0;
		    thisopt = sdatafile + count;
		    }
		else
		    {
		    sdatafile = (char **) calloc(2, sizeof(char *));
		    thisopt = sdatafile;
		    }
		*thisopt = strdup(optarg);
		break;
	        }
	    case 'c': 
		{
		if (thisopt = cmapfile)
		    {
		    count = 0;
		    while (*thisopt++)
			count++;
		    cmapfile = (char **)
			realloc(cmapfile, (count+2)*sizeof(char *));
		    cmapfile[count+1] = 0;
		    thisopt = cmapfile + count;
		    }
		else
		    {
		    cmapfile = (char **) calloc(2, sizeof(char *));
		    thisopt = cmapfile;
		    }
		*thisopt = strdup(optarg);
		break;
	        }
	    case 'h': do_tree		= 1;		break;
	    case 'u': do_summ		= 1;		break;
	    case 'S': do_stats		= 1;		break;
	    case 'x': do_context	= 1;		break;
	    case 'I': do_idlist		= 1;		break;
	    case 'l': tpt_lib		= optarg;	break;
	    case 'i': start_id		= optarg;	break;
	    case 'o': out_file		= optarg;	break;
	    case 'b': interactive	= 1;		break;
	    case 'W': warnings		= 0;		break;
	    case 'V': verbose		= 1;		break;
	    case 'Z': slave		= 1;		break;
	    case 'H': DoHelpMessage();	exit(0);	break;
	    case 'D': CmdLineSetVariable(optarg);	break;
	    case '?': errflag		= 1;		break;
	}
	if (errflag) {
	    fprintf(stderr, "Try '%s -H' for help.\n", this_prog);
	    exit(1);
	}
    }

    /* input (ESIS) file name */
    if (optind < ac) in_file = av[optind];

    /* If doing interactive/browsing, we can't take ESIS from stdin. */
    if (interactive && !in_file) {
	fprintf(stderr,
	    "You must specify ESIS file on cmd line for browser mode.\n");
	exit(1);
    }
}

/* ______________________________________________________________________ */
/*  Simply print out a help/usage message.
 */

static char *help_msg[] = {
  "",
  "  -t file   Print translated output using translation spec in <file>",
  "  -s file   <file> contains a list of character mappings",
  "  -c file   <file> contains a list of character mappings",
  "  -v        Validate using translation spec specified with -t",
  "  -i id     Consider only subtree starting at element with ID <id>",
  "  -b        Interactive browser",
  "  -S        Print statistics (how often elements occur, etc.)",
  "  -u        Print element usage summary (# of children, depth, etc.)",
  "  -x        Print context of each element",
  "  -h        Print document hierarchy as a tree",
  "  -o file   Write output to <file>.  Default is standard output.",
  "  -l dir    Set library directory to <dir>. (or env. variable TPT_LIB)",
  "  -I        List all IDs used in the instance",
  "  -W        Do not print warning messages",
  "  -H        Print this help message",
  "  -Dvar=val Set variable 'var' to value 'val'",
  "  file      Take input from named file.  If not specified, assume stdin.",
  "            File should be output from the 'sgmls' program (ESIS).",
  NULL
};

static void
DoHelpMessage()
{
    char	**s = help_msg;
    printf("usage: %s [option ...] [file]", this_prog);
    while (*s) puts(*s++);
}

/* ______________________________________________________________________ */
/*  Remember an external entity for future reference.
 *  Arguments:
 *	Pointer to entity structure to remember.
 */

static void
AddEntity(
    Entity_t	*ent
)
{
    static Entity_t *last_ent;

    if (!Entities) {
        Malloc(1, Entities, Entity_t);
        last_ent = Entities;
    }
    else {
        Malloc(1, last_ent->next, Entity_t);
        last_ent = last_ent->next;
    }
    *last_ent = *ent;
    
}

/*  Find an entity, given its entity name.
 *  Arguments:
 *	Name of entity to retrieve.
 */
static Entity_t *
FindEntity(
    char	*ename
)
{
    Entity_t	*n;
    for (n=Entities; n; n=n->next)
	if (StrEq(ename, n->ename)) return n;
    return 0;
}

/* Check multibyte characters for inner bytes that don't have their
 * 8th bit set - e.g., this may happen in SJIS.  Rather than risk
 * having downstream code mistake that inner byte for an ASCII
 * character, we'll mung it here and undo the mung when we write the
 * character out in DefaultOutputString().
 *
 * A character buffer may be allocated and returned in this routine.
 * That buffer must be free'd by the caller if the return value of
 * this routine is different from its parameter.
 *
 * The algorithm here is:
 *
 *  get a character
 *
 *      if the length of the current character is 1:
 *
 *          if the current character has the 8th bit off, emit it
 *          and continue;
 *
 *          if the current character is the chosen 8-bit
 *          character, emit the chosen character twice and follow
 *          it with the ASCII character "1" or'd with the 8th bit
 *          and continue;
 *
 *          emit the character and continue;
 *
 *      if the length of the current character is greater than
 *      one, for each of the bytes in the character:
 *
 *          if the current byte is the chosen 8-bit character,
 *          emit the chosen character twice and follow it with the
 *          ASCII character "1" or'd with the 8th bit and
 *          continue;
 *
 *          if the current byte is the chosen character except the
 *          8th bit is off, emit the chosen character twice
 *          followed by the ASCII character "0" or'd with the 8th
 *          bit and continue;
 *
 *          if the current byte has the 8th bit set, emit it and
 *          continue;
 *
 *          emit the chosen character followed by the current byte
 *          or'd with the 8th bit.
 */
static char *
EscapeI18NChars(
    char 	*source
)
{
    char *retval;
    unsigned char *from, *to;
    int len;

    if (MB_CUR_MAX == 1) {
	return source;
    } else {
	/* worst case, the string will expand by a factor of 3 */
	from = (unsigned char *)source;
	retval = malloc(3 * strlen(source) + 1);
	to = (unsigned char *)retval;
	while (*from) {
	    if ((len = mblen(from, MB_CUR_MAX)) < 0) {
		fprintf(stderr,
			"Bad multibyte character '%c' (0x%x) in source file\n",
			*from,
			*from);
		from++;
	    } else if ((len = mblen(from, MB_CUR_MAX)) == 1) {
		if (*from & 0x80) {
		    if (*from == I18N_TRIGGER) {
			*to++ = I18N_TRIGGER;
			*to++ = I18N_TRIGGER;
			*to++ = '1' | 0x80;
			from++;
		    } else {
			*to++ = *from++;
		    }
		} else {
		    *to++ = *from++;
		}
	    } else {
		while (--len >= 0) {
		    if (*from == I18N_TRIGGER) {
			*to++ = I18N_TRIGGER;
			*to++ = I18N_TRIGGER;
			*to++ = '1' | 0x80;
			from++;
		    } else if (*from == (I18N_TRIGGER & ~0x80)) {
			*to++ = I18N_TRIGGER;
			*to++ = I18N_TRIGGER;
			*to++ = '0' | 0x80;
			from++;
		    } else if (*from & 0x80) {
			*to++ = *from++;
		    } else {
			*to++ = I18N_TRIGGER;
			*to++ = *from++ | 0x80;
		    }
		}
	    }
	}
	*to = 0;
    }
    return retval;
}


static char *
ReadLocaleStrings(char *file_name, int *ret_code) {
int          fd;
char        *pBuf;
char        *i18nBuf;
off_t        size;
struct stat  stat_buf;

    fd = open(file_name, O_RDONLY);
    if (fd == -1) {
	*ret_code = 1;
	return "";
    }

    fstat(fd, &stat_buf);
    size = stat_buf.st_size;
    pBuf = malloc(size+1);
    pBuf[size] = 0;

    if (read(fd, pBuf, size) != size) {
	*ret_code = 2;
	return "";
    }

    i18nBuf = EscapeI18NChars(pBuf);
    if (i18nBuf != pBuf) {
	free(pBuf);
    }

    *ret_code = 0;
    return i18nBuf;
}

static int TclReadLocaleStrings(ClientData  clientData,
			        Tcl_Interp *interp,
				int         argc,
				char       *argv[]) {
char *pBuf;
int   ret_code;
char  errorBuf[512];

    if (argc > 2) {
        Tcl_SetResult(interpreter, "Too many arguments", TCL_VOLATILE);
        return TCL_ERROR;
    }
    if (argc < 2) {
        Tcl_SetResult(interpreter, "Missing file name", TCL_VOLATILE);
        return TCL_ERROR;
    }

    pBuf = ReadLocaleStrings(argv[1], &ret_code);

    if (ret_code != 0) {
	if (ret_code == 1) {
	    sprintf(errorBuf,
		    "Could not open locale strings file \"%s\" for reading",
		    argv[1]);
	}
	if (ret_code == 2) {
	    sprintf(errorBuf,
		    "Error reading locale strings file \"%s\"",
		    argv[1]);
	}
	Tcl_SetResult(interpreter, errorBuf, TCL_VOLATILE);
	return TCL_ERROR;
    }

    Tcl_SetResult(interpreter, pBuf, TCL_DYNAMIC);
    return TCL_OK;
}

/*  Accumulate lines up to the open tag.  Attributes, line number,
 *  entity info, notation info, etc., all come before the open tag.
 */
static Element_t *
AccumElemInfo(
    FILE	*fp
)
{
    char	buf[LINESIZE+1];
    char	*i18nBuf;
    int		c;
    int		i, na;
    char	*cp, *atval;
    Mapping_t	a[100];
    Element_t	*e;
    Entity_t	ent, *ent2;
    char	**tok;
    static int	Index=0;
    static Element_t	*last_e;
    

    Calloc(1, e, Element_t);
    memset(&ent, 0, sizeof ent);	/* clean space for entity info */

    /* Also, keep a linked list of elements, so we can easily scan through */
    if (last_e) last_e->next = e;
    last_e = e;

    e->index = Index++;		/* just a unique number for identification */

    /* in case these are not set for this element in the ESIS */
    e->lineno = last_lineno;
    e->infile = last_file;

    na = 0;
    while (1) {
	if ((c = getc(fp)) == EOF) break;
	fgets(buf, LINESIZE, fp);
	i18nBuf = EscapeI18NChars(buf);
	stripNL(i18nBuf);
	switch (c) {
	    case EOF:		/* End of input */
		fprintf(stderr, "Error: Unexpectedly reached end of ESIS.\n");
		exit(1);
		break;

	    case CMD_OPEN:	/* (gi */
		e->gi = AddElemName(i18nBuf);
		if (na > 0) {
		    Malloc(na, e->atts, Mapping_t);
		    memcpy(e->atts, a, na*sizeof(Mapping_t));
		    e->natts = na;
		    na = 0;
		}
		/*  Check if this elem has a notation attr.  If yes, and there
		    is no notation specified, recall the previous one. (feature
		    of sgmls - it does not repeat notation stuff if we the same
		    is used twice in a row) */
		if ((atval=FindAttValByName(e, "ENTITYREF")) &&	/* HACK */
					(ent2=FindEntity(atval))) {
		    e->entity = ent2;
		}

		return e;
		break;

	    case CMD_ATT:	/* Aname val */
		i = 3;
		tok = Split(i18nBuf, &i, 0);
		if (!strcmp(tok[1], "IMPLIED")) break;	/* skip IMPLIED atts. */
		if (!strcmp(tok[1], "CDATA") || !strcmp(tok[1], "TOKEN") ||
		    !strcmp(tok[1], "ENTITY") ||!strcmp(tok[1], "NOTATION"))
		{
		    a[na].name = AddAttName(tok[0]);
		    a[na].sval = AddAttName(tok[2]);
		    na++;
		}
		else {
		    fprintf(stderr, "Error: Bad attr line (%d): A%s %s...\n",
			e->lineno, tok[0], tok[1]);
		}
		break;

	    case CMD_LINE:	/* Llineno */
		/* These lines come in 2 forms: "L123" and "L123 file.sgml".
		 * Filename is given only at 1st occurance. Remember it.
		 */
		if ((cp = strchr(i18nBuf, ' '))) {
		    cp++;
		    last_file = strdup(cp);
		}
		last_lineno = e->lineno = atoi(i18nBuf);
		e->infile = last_file;
		break;

	    case CMD_DATA:	/* -data */
		fprintf(stderr, "Error: Data in AccumElemInfo, line %d:\n%c%s\n",
			e->lineno, c,i18nBuf);
		/*return e;*/
		exit(1);
		break;

	    case CMD_D_ATT:	/* Dename name val */

	    case CMD_NOTATION:	/* Nnname */
		break;

	    case CMD_EXT_ENT:	/* Eename typ nname */
		i = 3;
		tok = Split(i18nBuf, &i, 0);
		ent.ename = strdup(tok[0]);
		ent.type  = strdup(tok[1]);
		ent.nname = strdup(tok[2]);
		AddEntity(&ent);
		break;
	    case CMD_INT_ENT:	/* Iename typ text */
		fprintf(stderr, "Error: Got CMD_INT_ENT in AccumElemInfo: %s\n", i18nBuf);
		break;
	    case CMD_SYSID:	/* ssysid */
		ent.sysid = strdup(i18nBuf);
		break;
	    case CMD_PUBID:	/* ppubid */
		ent.pubid = strdup(i18nBuf);
		break;
	    case CMD_FILENAME:	/* ffilename */
		ent.fname = strdup(i18nBuf);
		break;

	    case CMD_CLOSE:	/* )gi */
	    case CMD_PI:	/* ?pi */
	    case CMD_SUBDOC:	/* Sename */
	    case CMD_SUBDOC_S:	/* {ename */
	    case CMD_SUBDOC_E:	/* }ename */
	    case CMD_EXT_REF:	/* &name */
	    case CMD_APPINFO:	/* #text */
	    case CMD_CONFORM:	/* C */
	    default:
		fprintf(stderr, "Error: Unexpected input in AccumElemInfo, %d:\n%c%s\n",
			e->lineno, c,i18nBuf);
		exit(1);
		break;
	}
	if (i18nBuf != buf) {
	    free(i18nBuf);
	}
    }
    fprintf(stderr, "Error: End of AccumElemInfo - should not be here: %s\n",
	e->gi);
/*    return e;*/
    exit(1);
}

/*  Read ESIS lines.
 *  Limitation?  Max 5000 children per node.  (done for efficiency --
 *  should do some malloc and bookkeeping games later).
 */

static Element_t *
ReadESIS(
    FILE	*fp,
    int		depth
)
{
    char	*buf, *i18nBuf;
    int		i, c, ncont;
    Element_t	*e;
    Content_t	cont[5000];

    Malloc( LINESIZE+1, buf, char );

    /* Read input stream - the output of "sgmls", called "ESIS".  */
    e = AccumElemInfo(fp);
    e->depth = depth;

    ncont = 0;
    while (1) {
	if ((c = getc(fp)) == EOF) break;
	switch (c) {
	    case EOF:		/* End of input */
		break;

	    case CMD_DATA:	/* -data */
		fgets(buf, LINESIZE, fp);
		i18nBuf = EscapeI18NChars(buf);
		stripNL(i18nBuf);
		cont[ncont].ch.data = strdup(i18nBuf);
		cont[ncont].type = CMD_DATA;
		ncont++;
		if (i18nBuf != buf) {
		    free(i18nBuf);
		}
		break;

	    case CMD_PI:	/* ?pi */
		fgets(buf, LINESIZE, fp);
		stripNL(buf);
		cont[ncont].type = CMD_PI;
		cont[ncont].ch.data = strdup(buf);
		ncont++;
		break;

	    case CMD_CLOSE:	/* )gi */
		fgets(buf, LINESIZE, fp);
		stripNL(buf);
		if (ncont) {
		    e->ncont = ncont;
		    Malloc(ncont, e->cont, Content_t);
		    for (i=0; i<ncont; i++) e->cont[i] = cont[i];
		}
		free(buf);
		return e;
		break;

	    case CMD_OPEN:	/* (gi */
/*fprintf(stderr, "+++++ OPEN +++\n");*/
/*		break;*/

	    case CMD_ATT:	/* Aname val */
	    case CMD_D_ATT:	/* Dename name val */
	    case CMD_NOTATION:	/* Nnname */
	    case CMD_EXT_ENT:	/* Eename typ nname */
	    case CMD_INT_ENT:	/* Iename typ text */
	    case CMD_SYSID:	/* ssysid */
	    case CMD_PUBID:	/* ppubid */
	    case CMD_FILENAME:	/* ffilename */
		ungetc(c, fp);
		cont[ncont].ch.elem = ReadESIS(fp, depth+1);
		cont[ncont].type = CMD_OPEN;
		cont[ncont].ch.elem->parent = e;
		ncont++;
		break;

	    case CMD_LINE:	/* Llineno */
		fgets(buf, LINESIZE, fp);
		break;		/* ignore these here */

	    case CMD_SUBDOC:	/* Sename */
	    case CMD_SUBDOC_S:	/* {ename */
	    case CMD_SUBDOC_E:	/* }ename */
	    case CMD_EXT_REF:	/* &name */
	    case CMD_APPINFO:	/* #text */
	    case CMD_CONFORM:	/* C */
	    default:
		fgets(buf, LINESIZE, fp);
		fprintf(stderr, "Error: Unexpected input at %d: '%c%s'\n",
			e->lineno, c, buf);
		exit(1);
		break;
	}
    }
    fprintf(stderr, "Error: End of ReadESIS - should not be here: %s\n", e->gi);
    free(buf);
    return NULL;
}

/* ______________________________________________________________________ */
/*  Read input stream, creating a tree in memory of the elements and data.
 *  Arguments:
 *	Filename where instance's ESIS is.
 */
static void
ReadInstance(
    char	*filename
)
{
    int		i, n;
    FILE	*fp;
    Element_t	*e;
    char	*idatt;

    if (filename) {	/* if we specified input file.  else stdin */
	if ((fp=fopen(filename, "r")) == NULL) {
	    perror(filename);
	    exit(1);
	}
    }
    else fp = stdin;
    last_file = filename;
    DocTree = ReadESIS(fp, 0);
    if (filename) fclose(fp);

    /* Traverse tree, filling in econt and figuring out which child
     * (ie. what birth order) each element is. */
    DocTree->my_eorder = -1;
    for (e=DocTree; e; e=e->next) {

	/* count element children */
	for (i=0,n=0; i<e->ncont; i++) if (IsContElem(e,i)) n++;
	if (n > 0) Calloc(n, e->econt, Element_t *);
	for (i=0; i<e->ncont; i++)
	    if (IsContElem(e,i)) e->econt[e->necont++] = ContElem(e,i);

	/* count data children */
	for (i=0,n=0; i<e->ncont; i++) if (IsContData(e,i)) n++;
	if (n > 0) Calloc(n, e->dcont, char *);
	for (i=0; i<e->ncont; i++)
	    if (IsContData(e,i)) e->dcont[e->ndcont++] = ContData(e,i);

	/* where in child order order */
	for (i=0; i<e->necont; i++)
	    e->econt[i]->my_eorder = i;

	/* Does this element have an ID? */
	for (i=0; i<e->natts; i++) {
	    if ((idatt=FindAttValByName(e, "ID"))) {
		AddID(e, idatt);
		/* remember ID value for quick reference */
		e->id = idatt;
		break;
	    }
	}
    }
    return;
}

/* ______________________________________________________________________ */
