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
 * $TOG: process.c /main/1 1999/08/30 10:45:37 mgreess $
 *
 * 
Copyright 1989, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.
 * *
 * Original Author of "xauth" : Jim Fulton, MIT X Consortium
 * Modified into "ttauth"    : Ralph Mor, X Consortium
 * Modified into "ttauth"     : Mitch Greess, Solutions Atlantic
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef X_NOT_STDC_ENV
extern int errno;
#endif
#include "ttauth.h"
#include "api/c/api_auth.h"
#include "api/c/tt_c.h"

#include <signal.h>

#define SECURERPC "SUN-DES-1"
#define K5AUTH "KERBEROS-V5-1"

#define TTAUTH_DEFAULT_RETRIES 10	/* number of competitors we expect */
#define TTAUTH_DEFAULT_TIMEOUT 2	/* in seconds, be quick */
#define TTAUTH_DEFAULT_DEADTIME 600L	/* 10 minutes in seconds */

#define add_to_list(h,t,e) {if (t) (t)->next = (e); else (h) = (e); (t) = (e);}

typedef struct _CommandTable {		/* commands that are understood */
    char *name;				/* full name */
    int minlen;				/* unique prefix */
    int maxlen;				/* strlen(name) */
    int (*processfunc)();		/* handler */
    char *helptext;			/* what to print for help */
} CommandTable;

struct _extract_data {			/* for iterating */
    FILE *fp;				/* input source */
    char *filename;			/* name of input */
    Bool used_stdout;			/* whether or not need to close */
    int nwritten;			/* number of entries written */
    char *cmd;				/* for error messages */
};

struct _list_data {			/* for iterating */
    FILE *fp;				/* output file */
};


/*
 * private data
 */
static char *stdin_filename = "(stdin)";  /* for messages */
static char *stdout_filename = "(stdout)";  /* for messages */
static const char *Yes = "yes";		/* for messages */
static const char *No = "no";			/* for messages */

static int do_list(char *inputfilename, int lineno, int argc, char *argv[]);
static int do_merge(char *inputfilename, int lineno, int argc, char *argv[]);
static int do_extract(char *inputfilename, int lineno, int argc, char *argv[]);
static int do_add(char *inputfilename, int lineno, int argc, char *argv[]);
static int do_remove(char *inputfilename, int lineno, int argc, char *argv[]);
static int do_help(char *inputfilename, int lineno, int argc, char *argv[]);
static int do_source(char *inputfilename, int lineno, int argc, char *argv[]);
static int do_info(char *inputfilename, int lineno, int argc, char *argv[]);
static int do_exit(char *inputfilename, int lineno, int argc, char *argv[]);
static int do_quit(char *inputfilename, int lineno, int argc, char *argv[]);
static int do_questionmark(char *inputfilename, int lineno, int argc, char *argv[]);

static CommandTable command_table[] = {	/* table of known commands */
{ "add", 2, 3, do_add,
"\
add       add an entry\n\
          add protoname protodata netid authname authdata"
},

{ "exit", 3, 4, do_exit,
"\
exit      save changes and exit program"
},

{ "extract", 3, 7, do_extract,
"\
extract   extract entries into file\n\
          extract filename <protoname=$> <protodata=$> <netid=$> <authname=$>"
},

{ "help", 1, 4, do_help,
"\
help      print help\n\
          help <topic>"
},

{ "info", 1, 4, do_info,
"\
info      print information about entries"
},

{ "list", 1, 4, do_list,
"\
list      list entries\n\
          list <protoname=$> <protodata=$> <netid=$> <authname=$>"
},

{ "merge", 1, 5, do_merge,
"\
merge     merge entries from files\n\
          merge filename1 <filename2> <filename3> ..."
},

{ "quit", 1, 4, do_quit,
"\
quit      abort changes and exit program" },

{ "remove", 1, 6, do_remove,
"\
remove    remove entries\n\
          remove <protoname=$> <protodata=$> <netid=$> <authname=$>"
},

{ "source", 1, 6, do_source,
"\
source    read commands from file\n\
          source filename"
},

{ "?", 1, 1, do_questionmark,
"\
?         list available commands" },

{ NULL, 0, 0, NULL, NULL },
};

#define COMMAND_NAMES_PADDED_WIDTH 10	/* wider than anything above */


static Bool okay_to_use_stdin = True;	/* set to false after using */

static const char *hex_table[] = {		/* for printing hex digits */
    "00", "01", "02", "03", "04", "05", "06", "07", 
    "08", "09", "0a", "0b", "0c", "0d", "0e", "0f", 
    "10", "11", "12", "13", "14", "15", "16", "17", 
    "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", 
    "20", "21", "22", "23", "24", "25", "26", "27", 
    "28", "29", "2a", "2b", "2c", "2d", "2e", "2f", 
    "30", "31", "32", "33", "34", "35", "36", "37", 
    "38", "39", "3a", "3b", "3c", "3d", "3e", "3f", 
    "40", "41", "42", "43", "44", "45", "46", "47", 
    "48", "49", "4a", "4b", "4c", "4d", "4e", "4f", 
    "50", "51", "52", "53", "54", "55", "56", "57", 
    "58", "59", "5a", "5b", "5c", "5d", "5e", "5f", 
    "60", "61", "62", "63", "64", "65", "66", "67", 
    "68", "69", "6a", "6b", "6c", "6d", "6e", "6f", 
    "70", "71", "72", "73", "74", "75", "76", "77", 
    "78", "79", "7a", "7b", "7c", "7d", "7e", "7f", 
    "80", "81", "82", "83", "84", "85", "86", "87", 
    "88", "89", "8a", "8b", "8c", "8d", "8e", "8f", 
    "90", "91", "92", "93", "94", "95", "96", "97", 
    "98", "99", "9a", "9b", "9c", "9d", "9e", "9f", 
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", 
    "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af", 
    "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", 
    "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf", 
    "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", 
    "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf", 
    "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", 
    "d8", "d9", "da", "db", "dc", "dd", "de", "df", 
    "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7", 
    "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef", 
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", 
    "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff", 
};

static unsigned int hexvalues[256];	/* for parsing hex input */

static int original_umask = 0;		/* for restoring */


/*
 * private utility procedures
 */

static char *
copystring(const char *src)
{
    int len = strlen (src);
    char *cp;

    if (!src) return NULL;
    cp = malloc (len + 1);
    if (cp)
	strcpy (cp, src);
    return cp;
}

static int
binaryEqual(const char *a, const char *b, unsigned len)
{
    while (len--)
	if (*a++ != *b++)
	    return 0;
    return 1;
}

static void
prefix(const char *fn, int n)
{
    fprintf (stderr, "%s: %s:%d:  ", ProgramName, fn, n);
}

static void
badcommandline(const char *cmd)
{
    fprintf (stderr, "bad \"%s\" command line\n", cmd);
}

static char *
skip_space(char *s)
{
    if (!s) return NULL;

    for ( ; *s && isascii(*s) && isspace(*s); s++)
	;
    return s;
}


static char *
skip_nonspace(char *s)
{
    if (!s) return NULL;

    /* put quoting into loop if need be */
    for ( ; *s && isascii(*s) && !isspace(*s); s++)
	;
    return s;
}

/* argvify string */
static char **
split_into_words(char *src, int *argcp)
{
    char *jword;
    char savec;
    char **argv;
    int cur, total;

    *argcp = 0;
#define WORDSTOALLOC 4			/* most lines are short */
    argv = (char **) malloc (WORDSTOALLOC * sizeof (char *));
    if (!argv) return NULL;
    cur = 0;
    total = WORDSTOALLOC;

    /*
     * split the line up into separate, nul-terminated tokens; the last
     * "token" will point to the empty string so that it can be bashed into
     * a null pointer.
     */

    do {
	jword = skip_space (src);
	src = skip_nonspace (jword);
	savec = *src;
	*src = '\0';
	if (cur == total) {
	    total += WORDSTOALLOC;
	    argv = (char **) realloc (argv, total * sizeof (char *));
	    if (!argv) return NULL;
	}
	argv[cur++] = jword;
	if (savec) src++;		/* if not last on line advance */
    } while (jword != src);

    argv[--cur] = NULL;			/* smash empty token to end list */
    *argcp = cur;
    return argv;
}


static FILE *
open_file(char **filenamep, char *mode, Bool *usedstdp, char *srcfn, int srcln, char *cmd)
{
    FILE *fp;

    if (strcmp (*filenamep, "-") == 0) {
	*usedstdp = True;
					/* select std descriptor to use */
	if (mode[0] == 'r') {
	    if (okay_to_use_stdin) {
		okay_to_use_stdin = False;
		*filenamep = stdin_filename;
		return stdin;
	    } else {
		prefix (srcfn, srcln);
		fprintf (stderr, "%s:  stdin already in use\n", cmd);
		return NULL;
	    }
	} else {
	    *filenamep = stdout_filename;
	    return stdout;		/* always okay to use stdout */
	}
    }

    fp = fopen (*filenamep, mode);
    if (!fp) {
	prefix (srcfn, srcln);
	fprintf (stderr, "%s:  unable to open file %s\n", cmd, *filenamep);
    }
    return fp;
}


static int
read_auth_entries(FILE *fp, _tt_AuthFileEntryList **headp,
                  _tt_AuthFileEntryList **tailp)
{
    _tt_AuthFileEntry *entry;
    _tt_AuthFileEntryList *head, *tail;
    int n;

    head = tail = NULL;
    n = 0;
					/* put all records into linked list */
    while ((entry = tt_ReadAuthFileEntry (fp)) != NULL) {
	_tt_AuthFileEntryList *l =
	  (_tt_AuthFileEntryList *) malloc (sizeof (_tt_AuthFileEntryList));
	if (!l) {
	    fprintf (stderr,
		     "%s:  unable to alloc entry reading auth file\n",
		     ProgramName);
	    exit (1);
	}
	l->next = NULL;
	l->entry = entry;
	if (tail) 			/* if not first time through append */
	  tail->next = l;
	else
	  head = l;			/* first time through, so assign */
	tail = l;
	n++;
    }
    *headp = head;
    *tailp = tail;
    return n;
}

/* turn hex key string into octets */
static int
cvthexkey(char *hexstr, char **ptrp)
{
    int i;
    int len = 0;
    char *retval, *s;
    unsigned char *us;
    char c;
    char savec = '\0';

    /* count */
    for (s = hexstr; *s; s++) {
	if (!isascii(*s)) return -1;
	if (isspace(*s)) continue;
	if (!isxdigit(*s)) return -1;
	len++;
    }

    /* if odd then there was an error */
    if ((len & 1) == 1) return -1;


    /* now we know that the input is good */
    len >>= 1;
    retval = malloc (len);
    if (!retval) {
	fprintf (stderr, "%s:  unable to allocate %d bytes for hexkey\n",
		 ProgramName, len);
	return -1;
    }

    for (us = (unsigned char *) retval, i = len; i > 0; hexstr++) {
	c = *hexstr;
	if (isspace(c)) continue;	 /* already know it is ascii */
	if (isupper(c))
	    c = tolower(c);
	if (savec) {
#define atoh(c) ((c) - (((c) >= '0' && (c) <= '9') ? '0' : ('a'-10)))
	    *us = (unsigned char)((atoh(savec) << 4) + atoh(c));
#undef atoh
	    savec = 0;		/* ready for next character */
	    us++;
	    i--;
	} else {
	    savec = c;
	}
    }
    *ptrp = retval;
    return len;
}

static int
dispatch_command(char *inputfilename, int lineno, int argc, char **argv,
                 CommandTable *tab, int *statusp)
{
    CommandTable *ct;
    char *cmd;
    int n;
					/* scan table for command */
    cmd = argv[0];
    n = strlen (cmd);
    for (ct = tab; ct->name; ct++) {
					/* look for unique prefix */
	if (n >= ct->minlen && n <= ct->maxlen &&
	    strncmp (cmd, ct->name, n) == 0) {
	    *statusp = (*(ct->processfunc))(inputfilename, lineno, argc, argv);
	    return 1;
	}
    }

    *statusp = 1;
    return 0;
}


static _tt_AuthFileEntryList *ttauth_head = NULL; /* list of auth entries */
static Bool ttauth_existed = False;	/* if was present at initialize */
static Bool ttauth_modified = False;	/* if added, removed, or merged */
static Bool ttauth_allowed = True;	/* if allowed to write auth file */
static char *ttauth_filename = NULL;
static Bool dieing = False;

#ifdef SIGNALRETURNSINT
#define _signal_t int
#else
#define _signal_t void
#endif

/* ARGSUSED */
static _signal_t
die(int sig)
{
    dieing = True;
    exit (auth_finalize ());
    /* NOTREACHED */
#ifdef SIGNALRETURNSINT
    return -1;				/* for picky compilers */
#endif
}

static _signal_t
catchsig(int sig)
{
#ifdef SYSV
    if (sig > 0) signal (sig, die);	/* re-establish signal handler */
#endif
    if (verbose && ttauth_modified) printf ("\r\n");
    die (sig);
    /* NOTREACHED */
#ifdef SIGNALRETURNSINT
    return -1;				/* for picky compilers */
#endif
}

static void
register_signals(void)
{
    signal (SIGINT, catchsig);
    signal (SIGTERM, catchsig);
#ifdef SIGHUP
    signal (SIGHUP, catchsig);
#endif
    return;
}


/*
 * public procedures for parsing lines of input
 */

int
auth_initialize(char *authfilename)
{
    int n;
    _tt_AuthFileEntryList *head, *tail;
    FILE *authfp;
    Bool exists;

    register_signals ();

    bzero ((char *) hexvalues, sizeof hexvalues);
    hexvalues['0'] = 0;
    hexvalues['1'] = 1;
    hexvalues['2'] = 2;
    hexvalues['3'] = 3;
    hexvalues['4'] = 4;
    hexvalues['5'] = 5;
    hexvalues['6'] = 6;
    hexvalues['7'] = 7;
    hexvalues['8'] = 8;
    hexvalues['9'] = 9;
    hexvalues['a'] = hexvalues['A'] = 0xa;
    hexvalues['b'] = hexvalues['B'] = 0xb;
    hexvalues['c'] = hexvalues['C'] = 0xc;
    hexvalues['d'] = hexvalues['D'] = 0xd;
    hexvalues['e'] = hexvalues['E'] = 0xe;
    hexvalues['f'] = hexvalues['F'] = 0xf;

    if (break_locks && verbose) {
	printf ("Attempting to break locks on authority file %s\n",
		authfilename);
    }

    if (ignore_locks) {
	if (break_locks) tt_UnlockAuthFile (authfilename);
    } else {
	n = tt_LockAuthFile (authfilename, TTAUTH_DEFAULT_RETRIES,
			 TTAUTH_DEFAULT_TIMEOUT, 
			 (break_locks ? 0L : TTAUTH_DEFAULT_DEADTIME));
	if (n != _tt_AuthLockSuccess) {
	    char *reason = "unknown error";
	    switch (n) {
	      case _tt_AuthLockError:
		reason = "error";
		break;
	      case _tt_AuthLockTimeout:
		reason = "timeout";
		break;
	    }
	    fprintf (stderr, "%s:  %s in locking authority file %s\n",
		     ProgramName, reason, authfilename);
	    return -1;
	}
    }

    /* these checks can only be done reliably after the file is locked */
    exists = (access (authfilename, F_OK) == 0);
    if (exists && access (authfilename, W_OK) != 0) {
	fprintf (stderr,
	 "%s:  %s not writable, changes will be ignored\n",
		 ProgramName, authfilename);
	ttauth_allowed = False;
    }

    original_umask = umask (0077);	/* disallow non-owner access */

    authfp = fopen (authfilename, "rb");
    if (!authfp) {
	int olderrno = errno;

					/* if file there then error */
	if (access (authfilename, F_OK) == 0) {	 /* then file does exist! */
	    errno = olderrno;
	    return -1;
	}				/* else ignore it */
	fprintf (stderr, 
		 "%s:  creating new authority file %s\n",
		 ProgramName, authfilename);
    } else {
	ttauth_existed = True;
	n = read_auth_entries (authfp, &head, &tail);
	(void) fclose (authfp);
	if (n < 0) {
	    fprintf (stderr,
		     "%s:  unable to read auth entries from file \"%s\"\n",
		     ProgramName, authfilename);
	    return -1;
	}
	ttauth_head = head;
    }

    n = strlen (authfilename);
    ttauth_filename = malloc (n + 1);
    if (ttauth_filename) strcpy (ttauth_filename, authfilename);
    ttauth_modified = False;

    if (verbose) {
	printf ("%s authority file %s\n", 
		ignore_locks ? "Ignoring locks on" : "Using", authfilename);
    }
    return 0;
}

static int
write_auth_file(char *tmp_nam)
{
    FILE *fp;
    _tt_AuthFileEntryList *list;

    strcpy (tmp_nam, ttauth_filename);
    strcat (tmp_nam, "-n");		/* for new */
    (void) unlink (tmp_nam);
    fp = fopen (tmp_nam, "wb");		/* umask is still set to 0077 */
    if (!fp) {
	fprintf (stderr, "%s:  unable to open tmp file \"%s\"\n",
		 ProgramName, tmp_nam);
	return -1;
    } 

    for (list = ttauth_head; list; list = list->next)
	tt_WriteAuthFileEntry (fp, list->entry);

    (void) fclose (fp);
    return 0;
}

int
auth_finalize(void)
{
    char temp_name[1024];			/* large filename size */

    if (ttauth_modified) {
	if (dieing) {
	    if (verbose) {
		printf ("Aborting changes to authority file %s\n",
			ttauth_filename);
	    }
	} else if (!ttauth_allowed) {
	    fprintf (stderr, 
		     "%s:  %s not writable, changes ignored\n",
		     ProgramName, ttauth_filename);
	} else {
	    if (verbose) {
		printf ("%s authority file %s\n", 
			ignore_locks ? "Ignoring locks and writing" :
			"Writing", ttauth_filename);
	    }
	    temp_name[0] = '\0';
	    if (write_auth_file (temp_name) == -1) {
		fprintf (stderr,
			 "%s:  unable to write authority file %s\n",
			 ProgramName, temp_name);
	    } else {
		(void) unlink (ttauth_filename);
#ifdef WIN32
		if (rename(temp_name, ttauth_filename) == -1)
#else
		if (link (temp_name, ttauth_filename) == -1)
#endif
		{
		    fprintf (stderr,
		     "%s:  unable to link authority file %s, use %s\n",
			     ProgramName, ttauth_filename, temp_name);
		} else {
		    (void) unlink (temp_name);
		}
	    }
	}
    }

    if (!ignore_locks) {
	tt_UnlockAuthFile (ttauth_filename);
    }
    (void) umask (original_umask);
    return 0;
}

int
process_command(char *inputfilename, int lineno, int argc, char **argv)
{
    int status;

    if (argc < 1 || !argv || !argv[0]) return 1;

    if (dispatch_command (inputfilename, lineno, argc, argv,
			  command_table, &status))
      return status;

    prefix (inputfilename, lineno);
    fprintf (stderr, "unknown command \"%s\"\n", argv[0]);
    return 1;
}


/*
 * utility routines
 */

static void
fprintfhex(FILE *fp, unsigned len, const char *cp)
{
    const unsigned char *ucp = (const unsigned char *) cp;

    for (; len > 0; len--, ucp++) {
	const char *s = hex_table[*ucp];
	putc (s[0], fp);
	putc (s[1], fp);
    }
    return;
}

/* ARGSUSED */
static int
dump_entry(char *inputfilename, int lineno, _tt_AuthFileEntry *auth, char *data)
{
    struct _list_data *ld = (struct _list_data *) data;
    FILE *fp = ld->fp;

    fprintf (fp, "%s", auth->protocol_name);
    putc (' ', fp);
    if (auth->protocol_data_length > 0)
	fprintfhex (fp, auth->protocol_data_length, auth->protocol_data);
    else
	fprintf (fp, "\"\"");
    putc (' ', fp);
    fprintf (fp, "%s", auth->network_id);
    putc (' ', fp);
    fprintf (fp, "%s", auth->auth_name);
    putc (' ', fp);

    if (auth->auth_data_length == 0)
	fprintf (fp, "\"\"");
    else if (!strcmp(auth->auth_name, SECURERPC) ||
	!strcmp(auth->auth_name, K5AUTH))
	fwrite (auth->auth_data, sizeof (char), auth->auth_data_length, fp);
    else
	fprintfhex (fp, auth->auth_data_length, auth->auth_data);
    putc ('\n', fp);

    return 0;
}

static int
extract_entry(char *inputfilename, int lineno, _tt_AuthFileEntry *auth, char *data)
{
    struct _extract_data *ed = (struct _extract_data *) data;

    if (!ed->fp) {
	ed->fp = open_file (&ed->filename, "wb",
			    &ed->used_stdout,
			    inputfilename, lineno, ed->cmd);
	if (!ed->fp) {
	    prefix (inputfilename, lineno);
	    fprintf (stderr,
		     "unable to open extraction file \"%s\"\n",
		     ed->filename);
	    return -1;
	}
    }
    tt_WriteAuthFileEntry (ed->fp, auth);
    ed->nwritten++;

    return 0;
}


static int
match_auth(_tt_AuthFileEntry *a, _tt_AuthFileEntry *b, int *authDataSame)
{
    int match = strcmp (a->protocol_name, b->protocol_name) == 0 &&
	    strcmp (a->network_id, b->network_id) == 0 &&
            strcmp (a->auth_name, b->auth_name) == 0;

    if (match)
    {
	*authDataSame = (a->auth_data_length == b->auth_data_length &&
	    binaryEqual (a->auth_data, b->auth_data, a->auth_data_length));
    }
    else
	*authDataSame = 0;

    return (match);
}


static int
merge_entries(_tt_AuthFileEntryList **firstp, _tt_AuthFileEntryList *second,
              int *nnewp, int *nreplp, int *ndupp)
{
    _tt_AuthFileEntryList *a, *b, *first, *tail;
    int n = 0, nnew = 0, nrepl = 0, ndup = 0;

    if (!second) return 0;

    if (!*firstp) {			/* if nothing to merge into */
	*firstp = second;
	for (tail = *firstp, n = 1; tail->next; n++, tail = tail->next) ;
	*nnewp = n;
	*nreplp = 0;
	*ndupp = 0;
	return n;
    }

    first = *firstp;
    /*
     * find end of first list and stick second list on it
     */
    for (tail = first; tail->next; tail = tail->next) ;
    tail->next = second;

    /*
     * run down list freeing duplicate entries; if an entry is okay, then
     * bump the tail up to include it, otherwise, cut the entry out of
     * the chain.
     */
    for (b = second; b; ) {
	_tt_AuthFileEntryList *next = b->next;	/* in case we free it */
	int duplicate;

	duplicate = 0;
	a = first;
	for (;;) {
	    int authDataSame;
	    if (match_auth (a->entry, b->entry, &authDataSame)) {
		if (authDataSame)
		{
		    /* found a complete duplicate, ignore */
		    duplicate = 1;
		    break;
		}
		else
		{
		    /* found a duplicate, but auth data differs */

		    _tt_AuthFileEntryList tmp;	/* swap it in for old one */
		    tmp = *a;
		    *a = *b;
		    *b = tmp;
		    a->next = b->next;
		    tt_FreeAuthFileEntry (b->entry);
		    free ((char *) b);
		    b = NULL;
		    tail->next = next;
		    nrepl++;
		    nnew--;
		    break;
		}
	    }
	    if (a == tail) break;	/* if have looked at left side */
	    a = a->next;
	}
	if (!duplicate && b) {		/* if we didn't remove it */
	    tail = b;			/* bump end of first list */
	}
	b = next;

	if (duplicate)
	    ndup++;
	else
	{
	    n++;
	    nnew++;
	}
    }

    *nnewp = nnew;
    *nreplp = nrepl;
    *ndupp = ndup;
    return n;

}


static int
search_and_do(char *inputfilename, int lineno, int start,
              int argc, char *argv[], int (*do_func)(), char *data)
{
    int i;
    int status;
    int errors = 0;
    _tt_AuthFileEntryList *l, *next;
    char *protoname, *protodata, *netid, *authname;

    for (l = ttauth_head; l; l = next)
    {
	next = l->next;

	protoname = protodata = netid = authname = NULL;

	for (i = start; i < argc; i++)
	{
	    if (!strncmp ("protoname=", argv[i], 10))
		protoname = argv[i] + 10;
	    else if (!strncmp ("protodata=", argv[i], 10))
		protodata = argv[i] + 10;
	    else if (!strncmp ("netid=", argv[i], 6))
		netid = argv[i] + 6;
	    else if (!strncmp ("authname=", argv[i], 9))
		authname = argv[i] + 9;
	}

	status = 0;

	if (protoname || protodata || netid || authname)
	{
	    if (protoname && strcmp (protoname, l->entry->protocol_name))
		continue;

	    if (protodata && !binaryEqual (protodata,
		l->entry->protocol_data, l->entry->protocol_data_length))
		continue;

	    if (netid && strcmp (netid, l->entry->network_id))
		continue;

	    if (authname && strcmp (authname, l->entry->auth_name))
		continue;

	    status = (*do_func) (inputfilename, lineno, l->entry, data);

	    if (status < 0)
		break;
	}
    }

    if (status < 0)
	errors -= status;		/* since status is negative */

    return (errors);
}


/* ARGSUSED */
static int
remove_entry(char *inputfilename, int lineno, _tt_AuthFileEntry *entry, char *data)
{
    int *nremovedp = (int *) data;
    _tt_AuthFileEntryList **listp = &ttauth_head;
    _tt_AuthFileEntryList *list;

    /*
     * unlink the auth we were asked to
     */
    while ((list = *listp)->entry != entry)
	listp = &list->next;
    *listp = list->next;
    tt_FreeAuthFileEntry (list->entry);                    /* free the auth */
    free (list);				    /* free the link */
    ttauth_modified = True;
    (*nremovedp)++;
    return 1;
}

/*
 * action routines
 */

/*
 * help
 */
int
print_help(FILE *fp, const char *cmd)
{
    CommandTable *ct;
    int n = 0;

    fprintf (fp, "\n");
    if (!cmd) {				/* if no cmd, print all help */
	for (ct = command_table; ct->name; ct++) {
	    fprintf (fp, "%s\n\n", ct->helptext);
	    n++;
	}
    } else {
	int len = strlen (cmd);
	for (ct = command_table; ct->name; ct++) {
	    if (strncmp (cmd, ct->name, len) == 0) {
		fprintf (fp, "%s\n\n", ct->helptext);
		n++;
	    }
	}
    }
	
    return n;
}

static int
do_help(char *inputfilename, int lineno, int argc, char *argv[])
{
    char *cmd = (argc > 1 ? argv[1] : NULL);
    int n;

    n = print_help (stdout, cmd);

    if (n < 0 || (n == 0 && !cmd)) {
	prefix (inputfilename, lineno);
	fprintf (stderr, "internal error with help");
	if (cmd) {
	    fprintf (stderr, " on command \"%s\"", cmd);
	}
	fprintf (stderr, "\n");
	return 1;
    }

    if (n == 0) {
	prefix (inputfilename, lineno);
	/* already know that cmd is set in this case */
	fprintf (stderr, "no help for noexistent command \"%s\"\n", cmd);
    }

    return 0;
}

/*
 * questionmark
 */
/* ARGSUSED */
static int
do_questionmark(char *inputfilename, int lineno, int argc, char *argv[])
{
    CommandTable *ct;
    int i;
#define WIDEST_COLUMN 72
    int col = WIDEST_COLUMN;

    printf ("Commands:\n");
    for (ct = command_table; ct->name; ct++) {
	if ((col + ct->maxlen) > WIDEST_COLUMN) {
	    if (ct != command_table) {
		putc ('\n', stdout);
	    }
	    fputs ("        ", stdout);
	    col = 8;			/* length of string above */
	}
	fputs (ct->name, stdout);
	col += ct->maxlen;
	for (i = ct->maxlen; i < COMMAND_NAMES_PADDED_WIDTH; i++) {
	    putc (' ', stdout);
	    col++;
	}
    }
    if (col != 0) {
	putc ('\n', stdout);
    }

    /* allow bad lines since this is help */
    return 0;
}

/*
 * list [displayname ...]
 */
static int
do_list(char *inputfilename, int lineno, int argc, char *argv[])
{
    struct _list_data ld;

    ld.fp = stdout;

    if (argc == 1) {
	register _tt_AuthFileEntryList *l;

	if (ttauth_head) {
	    for (l = ttauth_head; l; l = l->next) {
		dump_entry (inputfilename, lineno, l->entry, (char *) &ld);
	    }
	}
	return 0;
    }
    else
    {
	return (search_and_do (inputfilename, lineno, 1, argc, argv,
	    dump_entry, (char *) &ld));
    }
}

/*
 * merge filename [filename ...]
 */
static int
do_merge(char *inputfilename, int lineno, int argc, char *argv[])
{
    int i;
    int errors = 0;
    _tt_AuthFileEntryList *head, *tail, *listhead, *listtail;
    int nentries, nnew, nrepl, ndup;

    if (argc < 2) {
	prefix (inputfilename, lineno);
	badcommandline (argv[0]);
	return 1;
    }

    listhead = listtail = NULL;

    for (i = 1; i < argc; i++) {
	char *filename = argv[i];
	FILE *fp;
	Bool used_stdin = False;

	fp = open_file (&filename, "rb",
			&used_stdin, inputfilename, lineno,
			argv[0]);
	if (!fp) {
	    errors++;
	    continue;
	}

	head = tail = NULL;
	nentries = read_auth_entries (fp, &head, &tail);
	if (nentries == 0) {
	    prefix (inputfilename, lineno);
	    fprintf (stderr, "unable to read any entries from file \"%s\"\n",
		     filename);
	    errors++;
	} else {			/* link it in */
	    add_to_list (listhead, listtail, head);
 	}

	if (!used_stdin) (void) fclose (fp);
    }

    /*
     * if we have new entries, merge them in (freeing any duplicates)
     */
    if (listhead) {
	nentries = merge_entries (&ttauth_head, listhead,
	    &nnew, &nrepl, &ndup);
	if (verbose) 
	  printf ("%d entries read in:  %d new, %d replacement%s\n", 
	  	  nentries, nnew, nrepl, nrepl != 1 ? "s" : "");
	if (nentries > 0) ttauth_modified = True;
    }

    return 0;
}

/*
 * extract filename displayname [displayname ...]
 */
static int
do_extract(char *inputfilename, int lineno, int argc, char *argv[])
{
    int errors;
    struct _extract_data ed;

    if (argc < 3) {
	prefix (inputfilename, lineno);
	badcommandline (argv[0]);
	return 1;
    }

    ed.fp = NULL;
    ed.filename = argv[1];
    ed.nwritten = 0;
    ed.cmd = argv[0];

    errors = search_and_do (inputfilename, lineno, 2, argc, argv, 
	extract_entry, (char *) &ed);

    if (!ed.fp) {
	fprintf (stderr, 
		 "No matches found, authority file \"%s\" not written\n",
		 ed.filename);
    } else {
	if (verbose) {
	    printf ("%d entries written to \"%s\"\n", 
		    ed.nwritten, ed.filename);
	}
	if (!ed.used_stdout) {
	    (void) fclose (ed.fp);
	}
    }

    return errors;
}


/*
 * add protoname protodata netid authname authdata
 */
static int
do_add(char *inputfilename, int lineno, int argc, char *argv[])
{ 
    int n, nnew, nrepl, ndup;
    char *protoname;
    char *protodata_hex;
    char *protodata = NULL; /* not required */
    char *netid;
    char *authname;
    char *authdata_hex;
    char *authdata = NULL;
    int protodata_len, authdata_len;
    _tt_AuthFileEntry *entry = NULL;
    _tt_AuthFileEntryList *list;
    int status = 0;

    if (argc != 6 || !argv[1] || !argv[2] ||
	!argv[3] || !argv[4] || !argv[5])
    {
	prefix (inputfilename, lineno);
	badcommandline (argv[0]);
	return 1;
    }

    protoname = argv[1];
    protodata_hex = argv[2];
    netid = argv[3];
    authname = argv[4];
    authdata_hex = argv[5];

    protodata_len = strlen (protodata_hex);
    if (protodata_len > 0)
    {
	if (protodata_hex[0] == '"' && protodata_hex[protodata_len - 1] == '"')
	{
	    protodata = malloc (protodata_len - 1);
	    if (protodata)
	    {
		strncpy (protodata, protodata_hex + 1, protodata_len - 2);
		protodata_len -= 2;
	    }
	    else
		goto add_bad_malloc;
	}
	else
	{
	    protodata_len = cvthexkey (protodata_hex, &protodata);
	    if (protodata_len < 0)
	    {
		prefix (inputfilename, lineno);
		fprintf (stderr,
	       "protodata_hex contains odd number of or non-hex characters\n");
		return (1);
	    }
	}
    }

    authdata_len = strlen (authdata_hex);
    if (authdata_hex[0] == '"' && authdata_hex[authdata_len - 1] == '"')
    {
	authdata = malloc (authdata_len - 1);
	if (authdata)
	{
	    strncpy (authdata, authdata_hex + 1, authdata_len - 2);
	    authdata_len -= 2;
	}
	else
	    goto add_bad_malloc;
    }
    else if (!strcmp (protoname, SECURERPC) || !strcmp (protoname, K5AUTH))
    {
	authdata = malloc (authdata_len + 1);
	if (authdata)
	    strcpy (authdata, authdata_hex);
	else
	    goto add_bad_malloc;
    }
    else
    {
	authdata_len = cvthexkey (authdata_hex, &authdata);
	if (authdata_len < 0)
	{
	    prefix (inputfilename, lineno);
	    fprintf (stderr,
	       "authdata_hex contains odd number of or non-hex characters\n");
	    free (protodata);
	    return (1);
	}
    }

    entry = (_tt_AuthFileEntry *) malloc (sizeof (_tt_AuthFileEntry));

    if (!entry)
	goto add_bad_malloc;

    entry->protocol_name = copystring (protoname);
    entry->protocol_data_length = protodata_len;
    entry->protocol_data = protodata;
    entry->network_id = copystring (netid);
    entry->auth_name = copystring (authname);
    entry->auth_data_length = authdata_len;
    entry->auth_data = authdata;

    /* Avoid a double free later on in the event of an error */
    authdata = NULL;

    if (!entry->protocol_name ||
	(!entry->protocol_data && entry->protocol_data_length > 0) ||
        !entry->network_id || !entry->auth_name ||
	(!entry->auth_data && entry->auth_data_length > 0))
    {
	goto add_bad_malloc;
    }

    list = (_tt_AuthFileEntryList *) malloc (sizeof (_tt_AuthFileEntryList));

    if (!list)
	goto add_bad_malloc;

    list->next = NULL;
    list->entry = entry;

    /*
     * merge it in; note that merge will deal with allocation
     */

    n = merge_entries (&ttauth_head, list, &nnew, &nrepl, &ndup);

    if (n > 0)
	ttauth_modified = True;
    else
    {
	prefix (inputfilename, lineno);
	if (ndup > 0)
	{
	    status = 0;
	    fprintf (stderr, "no records added - all duplicate\n");
	}
	else
	{
	    status = 1;
	    fprintf (stderr, "unable to merge in added record\n");
	}
	goto cant_add;
    }

    return 0;


add_bad_malloc:

    status = 1;
    prefix (inputfilename, lineno);
    fprintf (stderr, "unable to allocate memory to add an entry\n");

cant_add:

    if (protodata)
	free (protodata);
    if (authdata)
	free (authdata);
    if (entry)
    {
	if (entry->protocol_name)
	    free (entry->protocol_name);
	if (entry->protocol_data)
	    free (entry->protocol_data);
	if (entry->network_id)
	    free (entry->network_id);
	if (entry->auth_name)
	    free (entry->auth_name);
	if (entry->auth_data)
	    free (entry->auth_data);
	free ((char *) entry);
    }

    return status;
}

/*
 * remove displayname
 */
static int
do_remove(char *inputfilename, int lineno, int argc, char *argv[])
{
    int nremoved = 0;
    int errors;

    if (argc < 2) {
	prefix (inputfilename, lineno);
	badcommandline (argv[0]);
	return 1;
    }

    errors = search_and_do (inputfilename, lineno, 1, argc, argv,
	remove_entry, (char *) &nremoved);
    if (verbose) printf ("%d entries removed\n", nremoved);
    return errors;
}

/*
 * info
 */
static int
do_info(char *inputfilename, int lineno, int argc, char *argv[])
{
    int n;
    _tt_AuthFileEntryList *l;

    if (argc != 1) {
	prefix (inputfilename, lineno);
	badcommandline (argv[0]);
	return 1;
    }

    for (l = ttauth_head, n = 0; l; l = l->next, n++) ;

    printf ("Authority file:       %s\n",
	    ttauth_filename ? ttauth_filename : "(none)");
    printf ("File new:             %s\n", ttauth_existed ? No : Yes);
    printf ("File locked:          %s\n", ignore_locks ? No : Yes);
    printf ("Number of entries:    %d\n", n);
    printf ("Changes honored:      %s\n", ttauth_allowed ? Yes : No);
    printf ("Changes made:         %s\n", ttauth_modified ? Yes : No);
    printf ("Current input:        %s:%d\n", inputfilename, lineno);
    return 0;
}


/*
 * exit
 */
static Bool alldone = False;

/* ARGSUSED */
static int
do_exit(char *inputfilename, int lineno, int argc, char *argv[])
{
    /* allow bogus stuff */
    alldone = True;
    return 0;
}

/*
 * quit
 */
/* ARGSUSED */
static int
do_quit(char *inputfilename, int lineno, int argc, char *argv[])
{
    /* allow bogus stuff */
    die (0);
    /* NOTREACHED */
    return -1;				/* for picky compilers */
}


/*
 * source filename
 */
static int
do_source(char *inputfilename, int lineno, int argc, char *argv[])
{
    char *script;
    char buf[BUFSIZ];
    FILE *fp;
    Bool used_stdin = False;
    int len;
    int errors = 0, status;
    int sublineno = 0;
    char **subargv;
    int subargc;
    Bool prompt = False;		/* only true if reading from tty */

    if (argc != 2 || !argv[1]) {
	prefix (inputfilename, lineno);
	badcommandline (argv[0]);
	return 1;
    }

    script = argv[1];

    fp = open_file (&script, "r", &used_stdin, inputfilename, lineno, argv[0]);
    if (!fp) {
	return 1;
    }

    if (verbose && used_stdin && isatty (fileno (fp))) prompt = True;

    while (!alldone) {
	buf[0] = '\0';
	if (prompt) {
	    printf ("ttauth> ");
	    fflush (stdout);
	}
	if (fgets (buf, sizeof buf, fp) == NULL) break;
	sublineno++;
	len = strlen (buf);
	if (len == 0 || buf[0] == '#') continue;
	if (buf[len-1] != '\n') {
	    prefix (script, sublineno);
	    fprintf (stderr, "line too long\n");
	    errors++;
	    break;
	}
	buf[--len] = '\0';		/* remove new line */
	subargv = split_into_words (buf, &subargc);
	if (subargv) {
	    status = process_command (script, sublineno, subargc, subargv);
	    free ((char *) subargv);
	    errors += status;
	} else {
	    prefix (script, sublineno);
	    fprintf (stderr, "unable to break line into words\n");
	    errors++;
	}
    }

    if (!used_stdin) {
	(void) fclose (fp);
    }
    return errors;
}
