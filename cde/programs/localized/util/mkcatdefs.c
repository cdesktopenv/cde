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
 * HISTORY
 */
/*
 * COMPONENT_NAME: (CMDMSG) Message Catalogue Facilities
 *
 * FUNCTIONS: main, mkcatdefs, incl, chkcontin, insert, nsearch, hash
 *
 * ORIGINS: 27, 18
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989, 1991
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * @OSF_COPYRIGHT@
 */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <ctype.h>

#ifdef aix
#include <sys/dir.h>
#endif

#ifdef hpux
#ifndef _XPG2
#define _XPG2
#endif
#endif

#include <limits.h>
#include <string.h>

#ifndef NL_TEXTMAX
#define NL_TEXTMAX 8192
#endif

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#define MAXLINELEN NL_TEXTMAX
#define KEY_START '$'
#define MAXIDLEN 64
#ifdef _D_NAME_MAX
#define MDIRSIZ _D_NAME_MAX
#else
#define MDIRSIZ 14
#endif

static int errflg = 0;
static int setno = 1;
static int msgno = 1;
static int symbflg = 0;
static int inclfile = 1;
static FILE *outfp;
static FILE *msgfp;
static FILE *descfile;
static char inname [PATH_MAX];
static char outname [PATH_MAX];
static char catname [PATH_MAX];
static char *mname;
static void mkcatdefs(char *);
static int chkcontin(char *);
static int insert(char *, int);
static int nsearch(char *);
static int hash(char *);


/*
 * NAME: main
 *
 * FUNCTION: Make message catalog defines.
 *
 * EXECUTION ENVIRONMENT:
 *  	User mode.
 *
 * NOTES:  Invoked by:
 *         mkcatdefs <name> <msg_file>
 *
 *  	Results are 1) Creates header file <name>.h.
 *                  2) Displays message file to stdout. The message file is 
 *                     ready to be used as input to gencat.
 *
 *   	mkcatdefs takes a message definition file and produces
 *  	a header file containing #defines for the message catalog,
 * 	the message sets and the messages themselves.  It also
 *  	produces a new message file which has the symbolic message set and
 *  	message identifiers replaced by their numeric values (in the form
 *  	required by gencat).
 *
 * DATA STRUCTURES: Effects on global data structures -- none.
 *
 * RETURNS: 1 - error condition
 */

int
main (int argc, 
      char *argv[]) 
{
  register int i;
  register char *cp;
  int count;
  char *t;
  
  setlocale (LC_ALL,"");
  
  /* usage: handle multiple files; -h option has to be at the end */
  if (argc < 3) {
    fprintf (stderr, 
	     "mkcatdefs: Usage: %s header_file msg_file [msg_file...] [-h]\n", 
	     argv [0]);	
    exit (0);
  }
  
  /* check if  include file should be created; -h is the last argument */
  if (argv[argc-1][0] == '-' && argv[argc-1][1] == 'h') 
    inclfile = 0;
  
  /* open header output file */
  if (inclfile) {
    mname = argv [1];
    if ((int)strlen((t = strrchr(mname,'/')) ? t + 1 : mname) > MDIRSIZ) {
      fprintf (stderr, "mkcatdefs: header file name too long\n");
      exit (1);
    }
    sprintf (outname, "%s", mname);
    if (strrchr(mname,'/'))
      mname = strrchr(mname,'/') + 1;
    if ((outfp = fopen (outname, "w")) == NULL) {
      fprintf (stderr, "mkcatdefs: Cannot open %s\n", outname);
      exit (1);
    } else  {
      /* convert name to upper case */
      for (cp=mname; *cp; cp+=i) {
	i = mblen(cp, MB_CUR_MAX);
	if (i < 0) {
	  fprintf (stderr, "mkcatdefs: filename contains invalid character\n");
	  exit (1);
	}
	if (i == 1) {
	  if (islower(*cp) != 0)
	    *cp = toupper(*cp);
	  else if (!isupper(*cp) && !isdigit(*cp))
	    *cp = '_';
	}
      }
    }
  } else sprintf (outname, "msg.h");
  
  
  /* open new msg output file */
  msgfp = stdout;
  
  /* if message descriptor files were specified then process each one in turn 
   */
  
  if (inclfile == 0 )
    count = argc - 1;
  else
    count = argc;
  for (i = 2; i < count; i++) {
    /* open input file */
    sprintf (inname, "%s", argv[i]);
    if (strcmp(inname,"-") == 0) {
      strcpy(inname,"stdin");
      descfile = stdin;       /* input from stdin if no source files */
      mkcatdefs(inname);
    } else	{
      if ((descfile = fopen(inname,"r")) == NULL) {
	fprintf (stderr, "mkcatdefs: Cannot open %s\n", inname);
	errflg = 1;
      } else  {
	mkcatdefs (inname);
	fclose(descfile);
	descfile = NULL;
      }
    }
  }
  
  if (inclfile) {
    fflush (outfp);
    if (ferror (outfp)) {
      fprintf (stderr, "mkcatdefs: There were write errors on file %s\n", 
	       outname);
      errflg = 1;
    }
    fclose (outfp);
  }
  
  if (errflg) {
    fprintf (stderr, "mkcatdefs: Errors found: no %s created\n", outname);
    if (inclfile)  unlink(outname);
  } else {
    if (inclfile) {
      if (symbflg)
	fprintf (stderr, "mkcatdefs: %s created\n", outname);
      else {
	fprintf (stderr, "mkcatdefs: No symbolic identifiers; no %s created\n",
		 outname);
	unlink (outname);
      }
    } 
    else 
      fprintf(stderr, "mkcatdefs: no %s created\n", outname);
  }
  exit (errflg);
}

/*
 * NAME: mkcatdefs
 *
 * FUNCTION: Make message catalog definitions.
 *
 * EXECUTION ENVIRONMENT:
 *  	User mode.
 *
 * RETURNS: None
 */
static void
mkcatdefs(char *fname)
     /*---- fname: message descriptor file name ----*/
{
  char msgname [PATH_MAX];
  char line [MAXLINELEN];
  register char *cp;
  register char *cpt;
  register int m;
  register int n;
  int contin = 0;
  int len;		/* # bytes in a character */
  
  
  /* put out header for include file */
  if (inclfile)
    {
      fprintf (outfp, "/* $%s$ */\n", "XConsortium");
      fprintf (outfp, "\n\n/* The following was generated from %s. */\n\n", 
	       fname);
    }
  
  /* process the message file */
  while (fgets(line, MAXLINELEN, descfile)) {
    line[MAXLINELEN-1] = '\0'; /* terminate in case length exceeded */
    /* find first nonblank character */
    for (cp=line; *cp; cp+=len) {
      len = mblen(cp, MB_CUR_MAX);
      if (len < 0) {
	fprintf (stderr, 
		 "mkcatdefs: sourcefile contains invalid character:\n\t%s", 
		 line);
	errflg = 1;
	return;
      }
      if (len == 1 && isspace(*cp) == 0)
	break;
    }
    if (*cp == KEY_START) {
      cp++;
      for (cpt = cp; *cp; cp += len) {
	len = mblen(cp, MB_CUR_MAX);
	if (len < 0) {
	  fprintf (stderr, 
		   "mkcatdefs: sourcefile contains invalid character:\n\t%s",
		   line);
	  errflg = 1;
	  return;
	}
	if (len == 1 && isspace(*cp) == 0)
	  break;
      }
      if (cp != cpt) {
	sscanf (cp, "%s", msgname);
	if ((m = nsearch(msgname)) > 0) {
	  fprintf (msgfp, "$ %d", m);
	  cp += strlen(msgname);
	  fprintf (msgfp, "%s", cp);
	} else
	  fputs (line, msgfp);
	continue; /* line is a comment */
      }
      if ((strncmp (cp, "set", 3) == 0) && 
	  ((len = mblen(&(cp[3]), MB_CUR_MAX)) == 1) && 
	  (isspace(cp[3]) != 0)) {
	char setname [MAXIDLEN];
	
	sscanf (cp+3+len, "%s", setname);
	if (inclfile) 
	  fprintf (outfp, "\n/* definitions for set %s */\n", setname, "");
	if (isdigit(setname[0])) {
	  cpt = setname;
	  do  {
	    if (!isdigit(*cpt)) {
	      fprintf(stderr, "mkcatdefs: %s is an invalid identifier\n", 
		      setname);
	      errflg = 1;
	      return;
	    }
	  }   while (*++cpt);
	  n = atoi (setname);
	  if (n >= setno)
	    setno = n;
	  else {
	    if (n == 0)
	      fprintf(stderr, "mkcatdefs: %s is an invalid identifier\n", 
		      setname);	
	    else
	      fprintf(stderr, 
		      "mkcatdefs: set # %d already assigned or sets not in ascending sequence\n", 
		      n);
	    errflg = 1;
	    return;
	  }
	} else  {
	  cpt = setname;
	  do  {
	    len = mblen(cpt, MB_CUR_MAX);
	    if (len <= 0) {
	      fprintf (stderr, 
		       "mkcatdefs: sourcefile contains invalid character:\n\t%s",
		       line);
	      errflg = 1;
	      return;
	    }
	    if (len == 1 && (isalnum(*cpt) == 0) && (*cpt != '_')) {
	      fprintf(stderr, 
		      "mkcatdefs: %s is an invalid identifier\n", 
		      setname);
	      errflg = 1;
	      return;
	    }
	  } while (*(cpt += len));
	  if (inclfile)
	    fprintf (outfp, "#define %s %d\n\n", setname, setno);
	  symbflg = 1;
	}
#ifdef aix
	fprintf (msgfp,"$delset");
	fprintf (msgfp," %d\n", setno);
#endif
	fprintf (msgfp,"%.4s", line);
	fprintf (msgfp," %d\n", setno++);
	msgno = 1;
	continue;
      } else {
	/* !!!other command */
      }
    } else
      if (contin) {
	if (!chkcontin(line))
	  contin = 0;
      } else if (setno > 1) { /* set must have been seen first */
	char msgname [MAXIDLEN];
	
	msgname [0] = '\0';
	if (sscanf (cp, "%s", msgname) && msgname[0]) {
	  len = mblen(cp, MB_CUR_MAX);
	  if (len < 0) {
	    fprintf (stderr, 
		     "mkcatdefs: sourcefile contains invalid character:\n\t%s",
		     line);
	    errflg = 1;
	    return;
	  }
	  if (len == 1 && isalpha(*cp) != 0) {
	    cpt = msgname;
	    do  {
	      len = mblen(cpt, MB_CUR_MAX);
	      if (len < 0) {
		fprintf (stderr, 
			 "mkcatdefs: sourcefile contains invalid character:\n\t%s",
			 line);
		errflg = 1;
		return;
	      }
	      if (len == 1 && (isalnum(*cpt) == 0) && (*cpt != '_')) {
		fprintf(stderr, "mkcatdefs: %s is an invalid identifier\n", 
			msgname);	
		errflg = 1;
		return;
	      }
	    }   while (*(cpt += len));
	    cp += strlen(msgname);
	    fprintf (msgfp,"%d%s", msgno,cp);
	    if (inclfile)
	      fprintf (outfp, "#define %s %d\n", msgname, msgno);
	    symbflg = 1;
	    if (chkcontin(line))
	      contin = 1;
	    if(insert(msgname,msgno++) < 0) {
	      fprintf(stderr, "mkcatdefs: name %s used more than once\n", 
		      msgname); 
	      errflg = 1;
	      return;
	    }
	    continue;
	  } else if (isdigit (msgname[0])){
	    cpt = msgname;
	    do  {
	      if (!isdigit(*cpt)) {
		fprintf(stderr, "mkcatdefs: invalid syntax in %s\n", line);
		errflg = 1;
		return;
	      }
	    }   while (*++cpt);
	    n = atoi (msgname);
	    if ((n >= msgno) || (n == 0 && msgno == 1))
	      msgno = n + 1;
	    else {
	      errflg = 1;
	      if (n == 0)
		fprintf(stderr, "mkcatdefs: %d is an invalid identifier\n", 
			msgno);
	      else if (n == msgno) 
		fprintf(stderr,
			"mkcatdefs: message id %s already assigned to identifier\n", 
			msgname);
	      else
		fprintf(stderr,
			"mkcatdefs: source messages not in ascending sequence\n");
	      return;
	    }
	  }
	}
	if (chkcontin(line))
	  contin = 1;
      }
    fputs (line, msgfp);
  }
  
  /* make sure the operations read/write operations were successful */
  if (ferror(descfile)) {
    fprintf (stderr, "mkcatdefs: There were read errors on file %s\n", inname);
    errflg = 1;
  }
  return;
}

/*
 * NAME: chkcontin
 *
 * FUNCTION: Check for continuation line.
 *
 * EXECUTION ENVIRONMENT:
 *  	User mode.
 *
 * RETURNS: 0 - not a continuation line.
 *          1 - continuation line.
 */
static int
chkcontin(char *line) 
{
  int	len;		/* # bytes in character */
  wchar_t	wc;		/* process code of current character in line */
  wchar_t	wcprev;		/* process code of previous character in line */
  
  for (wc=0; *line; line+=len) {
    wcprev = wc;
    len = mbtowc(&wc, line, MB_CUR_MAX);
    if (len < 0) {
      fprintf (stderr, 
	       "mkcatdefs: sourcefile contains invalid character:\n\t%s",
	       line);
      errflg = 1;
      return (0);
    }
  }
  if (wcprev == '\\' && wc == '\n')
    return (1);
  return (0);
}

#define HASHSIZE 256			/* must be a power of 2 */
#define HASHMAX HASHSIZE - 1

struct name {
  char *regname;
  int   regnr;
  struct name *left;
  struct name *right;
};

static struct name *symtab[HASHSIZE];	/* hashed pointers to binary trees */

/*
 * NAME: insert
 *
 * FUNCTION: Insert symbol
 *
 * EXECUTION ENVIRONMENT: 
 *  	User mode.
 * 
 * NOTES: These routines manipulate a symbol table for the mkcatdefs program.
 *  	  The symbol table is organized as a hashed set of binary trees. If the
 *  	  symbol being passed in is found then a -1 is returned, otherwise the
 *  	  symbol is placed in the symbol table and a 0 is returned. The purpose
 *  	  of the symbol table is to keep mkcatdefs from assigning two different
 *  	  message set / message numbers to the same symbol.
 *        Read the next line from the open message catalog descriptor file.
 *
 * RETURNS: 0 - symbol inserted.
 *         -1 - symbol exists.
 */

static int 
insert(char *tname,
       int seqno)
     /*
       tname - pointer to symbol
       seqno - integer value of symbol
       */
     
{
  register struct name *ptr,*optr;
  int rslt = -1,i,hashval;
  
  hashval = hash(tname);
  ptr = symtab[hashval];
  
  /* search the binary tree for specified symbol */
  while (ptr && (rslt = strcmp(tname,ptr->regname))) {
    optr=ptr;  
    if (rslt<0)
      ptr = ptr->left;
    else
      ptr = ptr->right;
  }
  
  if (rslt == 0)  /* found the symbol already defined */
    return (-1);
  else {          /* symbol not defined yet so put it into symbol table */    
    ptr = (struct name *)calloc(sizeof(struct name), 1);
    ptr->regname = malloc(strlen(tname) + 1);
    strcpy (ptr->regname, tname);
    ptr->regnr = seqno;
    
    /* not first entry in tree so update branch pointer */
    if (symtab[hashval]) {
      if (rslt < 0)
	optr->left = ptr;
      else
	optr->right = ptr;
      
      /* first entry in tree so set the root pointer */
    } else
      symtab[hashval] = ptr;
    
    return (0);
  }
}

/*
 * NAME: nsearch
 *
 * FUNCTION: Search for symbol
 *
 * EXECUTION ENVIRONMENT: 
 *  	User mode.
 * 
 * NOTES: Searches for specific identifier. If found, return allocated number.
 * 	  If not found, return -1.
 *
 * RETURNS: Symbol sequence number if symbol is found.
 *          -1 if symbol is not found.
 */
static int 
nsearch (char *tname)
     /*
       tname - pointer to symbol
       */
     
{
  register struct name *ptr,*optr;
  int rslt = -1,i,hashval;
  
  hashval = hash(tname);
  ptr = symtab[hashval];
  
  /* search the binary tree for specified symbol */
  while (ptr && (rslt = strcmp(tname,ptr->regname))) {
    optr=ptr;  
    if (rslt<0)
      ptr = ptr->left;
    else
      ptr = ptr->right;
  }
  
  if (rslt == 0)		/* found the symbol already defined */
    return(ptr->regnr);
  else
    return (-1);
}


/*
 * NAME: hash
 *
 * FUNCTION: Create hash value from symbol name.
 *
 * EXECUTION ENVIRONMENT: 
 *  	User mode.
 * 
 * NOTES: Hash the symbol name using simple addition algorithm.
 * 	  Make sure that the hash value is in range when it is returned.
 *
 * RETURNS: A hash value.
 */

static int 
hash (char *name) /* pointer to symbol */
{
  register int hashval = 0;
  
  while (*name)
    hashval += *name++;
  
  return (hashval & HASHMAX);
}
