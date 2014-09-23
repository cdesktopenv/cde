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
/* $XConsortium: conutil.c /main/3 1995/11/08 11:03:16 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
               Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Conutil has utility procedures for program CONTEXT. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"
#include "trie.h"

#include "cont.h"

/* Output declaration of dlmptr */
void dumpdlmptr(M_NOPAR)
  {
    int n;

    fprintf(delim, "M_DELIMEXTERN M_WCHAR *m_dlmptr[%d];\n", dcount+1);
    fprintf(delim, "M_DELIMEXTERN char *mb_dlmptr[%d]\n", dcount+1);
    fprintf(delim, "#if defined(M_DELIMDEF)\n  = {\n");
    for (n = 0 ; n < dcount ; n++) {
      if (n) fputs(",\n", delim);
      fprintf(delim, "  %s", dlmptr[n]);
      }
    fprintf(delim, "%s", endif);
    }

/* Output the generated data structures */
#if defined(M_PROTO)
void dumptree(LOGICAL sparse)
#else
void dumptree(sparse)
  LOGICAL sparse;
#endif
  {
    int i, count = 0;

    for (i = 0 ; i < ccount ; i++)
      if (contree[i])
        countdown(contree[i], &count);
    fprintf(delim,
     "M_DELIMEXTERN M_PTRIE m_delimtrie[%d]\n#if defined(M_DELIMDEF)\n = {\n",
      count);
    fprintf(context,
      "M_CONEXTERN int m_contree[%d]\n#if defined(M_CONDEF)\n = {\n  ",
      ccount);

    count = 0;
    for (i = 0 ; i < ccount ; i++) {
      if (contree[i]) {
        fprintf(context, "%d", count + 1);
        dumpnode(&first, delim, contree[i], &count, (void *)printval);
        }
      else fprintf(context, "0");
      if (i < ccount - 1) fprintf(context, ",\n  ");
      }
    fprintf(delim, "%s", endif);
    fprintf(context, "%s", endif);
    nextcon(sparse);
    }

/* Enter a delimiter into the delimiter tree for a particular context */
void enterdelim(n)
  int n;
{
if (! contree[n])
    {
    contree[n] = m_gettrienode();
    }

if (m_ntrtrie(dstruct->string,
	      contree[n],
	      (void *) (unsigned long) dstruct->count))
    {
    char *mb_dstring, *mb_contexts;

    mb_dstring = MakeMByteString(dstring);
    mb_contexts = MakeMByteString(contexts[n]);
    dcount--;
    warning2("Duplicate assignment to delimiter \"%s\" in context \"%s\"",
	     mb_dstring,
	     mb_contexts);
    m_free(mb_dstring,"multi-byte string");
    m_free(mb_contexts,"multi-byte string");
    }
}

/* Read the code to be executed with a given state transition */
void getcode(n)
int n;
{
int c ; /* c is int instead of char for use with ungetc */
int nested = 1;
LOGICAL comment = FALSE;
char *mb_context;
M_WCHAR wlb = 0, wcm, wnl, wsl, wst;


if (!wlb)
    {
    int
    ret = mbtowc(&wlb, "{", 1); /* keep the "}" balanced */
    ret = mbtowc(&wcm, ",", 1);
    ret = mbtowc(&wnl, "\n", 1);
    ret = mbtowc(&wsl, "/", 1);
    ret = mbtowc(&wst, "*", 1);
    (void) ret;
    }

while (m_whitespace((M_WCHAR) (c = readchar(FALSE))));
if (c != wlb && c != wcm)
    {
    unread(c);
    return;
    }

if (! casestarted)
    {
    char *mb_dname;

    mb_dname = MakeMByteString(dname);
    casestarted = TRUE;
    fprintf(fcase, "      case %s:\n", mb_dname);
    fprintf(fcase, "        switch (m_prevcon) {\n") ; /* balance the } */
    m_free(mb_dname,"multi-byte string");
    }

mb_context = MakeMByteString(contexts[n]);
fprintf(fcase, "          case %s:\n", mb_context);
m_free(mb_context,"multi-byte string");

if (c == wcm) return;

fprintf(fcase, "/* line %d \"context.dat\" */\n", m_line);

while (TRUE)
    {
    char mb_c[32]; /* arbitrarily large */
    int  length;

    c = readchar(FALSE);
    if (c == EOF)
	{
	warning("Unexpected EOF");
	exit(TRUE);
	}

    length = wctomb(mb_c, c);
    mb_c[length] = 0;
    if (length == 1)
	{
	switch (mb_c[0])
	    {
	    case '{':
		if (! comment) nested++;
		break;
	    case '}': 
		if (! comment)
		if (! --nested)
		    {
		    fprintf(fcase,"\n            break ;\n");
		    return;
		    }
		break;
	    case '/':
		if (! comment)
		    {
		    fprintf(fcase, "/");
		    c = readchar(FALSE);
		    if (c == wst) comment = TRUE;
		    length = wctomb(mb_c, c);
		    mb_c[length] = 0;
		    }
		break;
	    case '*':
		if (comment)
		    {
		    fprintf(fcase, "*");
		    c = readchar(FALSE);
		    if (c == wsl) comment = FALSE;
		    length = wctomb(mb_c, c);
		    mb_c[length] = 0;
		    }
	    } /* End case */
	}
    fprintf(fcase, "%s", mb_c);
#if 0
    if (c == wnl)
	fprintf(fcase, "#line %d \"context.dat\"\n", m_line);
#endif
    } /* End while */
} /* End proc getcode() */

/* Read the colon separating the two states in a transition pair */
void getcolon(M_NOPAR)
{
int c ; /* c is int instead of char for use with ungetc */
M_WCHAR wcl;
char unexp[32]; /* arbitraily large */
int  length;

int ret = mbtowc(&wcl, ":", 1);
(void) ret;

while (TRUE)
    {
    c = readchar(FALSE);
    if (! m_whitespace((M_WCHAR) c)) break;
    }
if (c != wcl)
    {
    length = wctomb(unexp, (M_WCHAR) c);
    unexp[length] = 0;
    warning1("Expecting : instead of '%s'\n", unexp);
    }
}

/* Read a context name from an input line */
int getContext(M_NOPAR)
{
M_WCHAR name[CNAMELEN + 1];
int c ; /* c is int instead of char for use with ungetc */
int i = 0;
M_WCHAR wsm = 0, wcl, wcm;
char *mb_name;

if (!wsm)
    {
    int
    ret = mbtowc(&wsm, ";", 1);
    ret = mbtowc(&wcl, ":", 1);
    ret = mbtowc(&wcm, ",", 1);
    (void) ret;
    }

while (TRUE)
    {
    c = readchar(TRUE);
    if (c == EOF) return(NOMORE);
    *name = (M_WCHAR) c;
    if (*name == wsm) return(NOMORE);
    if (! m_whitespace(*name)) break;
    }

while (TRUE)
    {
    i++;
    if (i >= CNAMELEN)
	{
	while (! m_whitespace((M_WCHAR) (c = readchar(TRUE)))
	&& c != wcl
	&& c != wcm
	&& c != EOF
	&& c != wsm);
	unread(c);
	break;
	}
    c = readchar(TRUE);
    if (m_whitespace((M_WCHAR) c) ||
    c == wsm ||
    c == wcl ||
    c == wcm ||
    c == EOF)
	{
	unread(c);
	break;
	}
    name[i] = (M_WCHAR) c;
    }
name[i] = M_EOS;

for (i = 0 ; i < ccount ; i++)
    if (! w_strcmp(name, contexts[i])) return(i);

inccon() ;
i = w_strlen(name) + 1;
contexts[ccount - 1] = (M_WCHAR *) m_malloc(i, "context name");
memcpy(contexts[ccount - 1], name, i * sizeof(M_WCHAR));

mb_name = MakeMByteString(name);
fprintf(context, "#define %s %d\n", mb_name, ccount);
m_free(mb_name,"multi-byte string");

return(ccount - 1);
}

/* Read a delimiter name from the input line */
LOGICAL getdname(M_NOPAR)
{
int c;
M_WCHAR *p;

/* Skip leading blanks */
while (TRUE)
    {
    c = readchar(TRUE);
    if (c == EOF) return(FALSE);
    if (! m_whitespace((M_WCHAR) c)) break;
    }
for (p = dname;
! m_whitespace((M_WCHAR) c) && c != EOF;
c = readchar(TRUE))
    {
    if (p - dname >= DNAMELEN)
	{
	while (! m_whitespace((M_WCHAR) c) && c != EOF) c = readchar(TRUE);
	break;
	}
    *p++ = (M_WCHAR) c;
    }
*p = M_EOS;
if ((dstruct = (struct dstruct *) m_lookfortrie(dname, &delimtrie)))
    {
    withdelim = TRUE;
    curdelim = dstruct->count - 1;
    }
else
    {
    char *mb_dname;

    curdelim = dcount;
    incdelim();
    withdelim = FALSE;
    mb_dname = MakeMByteString(dname);
    fprintf(delim, "#define %s %d\n", mb_dname, dcount);
    m_free(mb_dname,"multi-byte string");
    }
return(TRUE);
}

/* Out of context space.  Increase. */
void inccon(M_NOPAR)
  {
    M_TRIE **newtrie = NULL;
    int *newtransit = NULL;
    M_WCHAR **newcontexts = NULL;
    int trysize;
    int i, j;

    if (ccount < NUMCON) {
      ccount++;
      return;
      }
    trysize = m_plus10p(NUMCON);
    newtrie = (M_TRIE **) calloc(trysize, sizeof(M_TRIE *));
    newtransit = (int *) calloc(trysize * NUMDELIM, sizeof(int));
    newcontexts = (M_WCHAR **) calloc(trysize, sizeof(M_WCHAR *));
    if (! newtrie || ! newtransit || ! newcontexts) {
      trysize = NUMCON + 1;
      if (newtrie) free((M_POINTER) newtrie);
      if (newtransit) free((M_POINTER) newtransit);
      if (newcontexts) free((M_POINTER) newcontexts);
      newtrie = (M_TRIE **) calloc(trysize, sizeof(M_TRIE *));
      newtransit = (int *) calloc(trysize * NUMDELIM, sizeof(int));
      newcontexts = (M_WCHAR **) calloc(trysize, sizeof(M_WCHAR *));
      }
    if (! newtrie || ! newtransit || ! newcontexts) {
      m_error("Out of memory for contexts");
      exit(TRUE);
      }
    for (i = 0 ; i < ccount ; i++) 
      for (j = 0 ; j < dcount ; j++)
        newtransit[i * NUMDELIM + j] = transit(i, j);
    NUMCON = trysize;
    free((M_POINTER) xtransit);
    xtransit = newtransit;
    memcpy((M_POINTER) newtrie, (M_POINTER) contree,
           ccount * sizeof(M_TRIE **));
    memcpy((M_POINTER) newcontexts, (M_POINTER) contexts,
	   ccount * sizeof(M_WCHAR **));
    free((M_POINTER) contree);
    free((M_POINTER) contexts);
    contree = newtrie;
    contexts = newcontexts;
    ccount++;
    }

/* Increase delimiter space. */
void incdelim(M_NOPAR)
{
int *newtransit = NULL;
char **newdlm = NULL;
int trysize;
int i, j;

if (dcount < NUMDELIM)
    {
    dcount++;
    return;
    }

trysize = m_plus10p(NUMDELIM);
newtransit = (int *) calloc(NUMCON * trysize, sizeof(int));
if (loading) newdlm = (char **) calloc(trysize, sizeof(M_WCHAR *));
if (! newtransit || (loading && ! newdlm))
    {
    trysize = NUMDELIM + 1;
    newtransit = (int *) calloc(NUMCON * trysize, sizeof(int));
    if (loading) newdlm = (char **) calloc(trysize, sizeof(M_WCHAR *));
    }
if (! newtransit || (loading && ! newdlm))
    {
    m_error("Out of memory for delimiters");
    exit(TRUE);
    }
for (i = 0 ; i < ccount ; i++) 
for (j = 0 ; j < dcount ; j++)
newtransit[i * trysize + j] = transit(i, j);
free((M_POINTER) xtransit);
if (loading)
    {
    memcpy((M_POINTER) newdlm, (M_POINTER) dlmptr, dcount * sizeof(M_TRIE **));
    free((M_POINTER) dlmptr);
    dlmptr = newdlm;
    }
NUMDELIM = trysize;
xtransit = newtransit;
dcount++;
}

/* Read delimiter definitions from delim.dat */
void loaddelim(M_NOPAR)
{
int c;
M_WCHAR *p;
int i;
M_WCHAR wnl;
char *mb_dname, *mb_dstring;

int ret = mbtowc(&wnl, "\n", 1);
(void) ret;

loading = TRUE;
while ((c = getc(ddat)) != EOF)
    {
    /* Skip leading white space */
    if (c == wnl) continue;
    if (m_whitespace((M_WCHAR) c))
	{
	while ((c = getc(ddat)) != wnl)
	if (c == EOF) return;
	continue;
	}
    /* Delimiter name into dname */
    for (p = dname ; ! m_whitespace((M_WCHAR) c) ; c = getc(ddat))
	{
	if (c == EOF)
	    {
	    warning("Unexpected EOF");
	    exit(TRUE);
	    }
	if (p - dname >= DNAMELEN)
	    {
	    while (! m_whitespace((M_WCHAR) c) && c != EOF) c = getc(ddat);
	    if (c == EOF)
		{
		warning("Unexpected EOF");
		exit(TRUE);
		}
	    break;
	    }
	*p++ = m_upper(c);
	}
    *p = M_EOS;
    /* Skip intervening white space */
    while (m_whitespace((M_WCHAR) c) && c != EOF) c = getc(ddat);
    if (c == EOF)
	{
	warning("Unexpected EOF");
	exit(TRUE);
	}
    /* Delimiter string into dstring */
    for (p = dstring;
    ! m_whitespace((M_WCHAR) c) && c != EOF;
    c = getc(ddat))
	{
	if (p - dstring >= DELIMLEN)
	    {
	    m_error("Delimiter string too long");
	    exit(TRUE);
	    }
	*p++ = c;
	}
    *p = M_EOS;
    if (w_strlen(dstring) > maxd) maxd = w_strlen(dstring);
    /* Write definitions to output file.  Save data for later. */
    incdelim();
    dstruct = (struct dstruct *)
	m_malloc(sizeof(struct dstruct), "delimiter structure");
    dstruct->string = (M_WCHAR *) m_malloc(w_strlen(dstring) + 1, "delimiter");
    w_strcpy(dstruct->string, dstring);
    dstruct->count = dcount;

    mb_dname = MakeMByteString(dname);
    fprintf(delim, "#define %s %d\n", mb_dname, dcount);
    m_free(mb_dname,"multi-byte string");

    for (i = 0 ; dname[i] ; i++)
	dname[i] = m_lower(dname[i]);
    mb_dname = MakeMByteString(dname);

    mb_dstring = MakeMByteString(dstring);
    fprintf(delim,
	    "M_DELIMEXTERN char %s[%d] M_DELIMINIT(\"",
	    mb_dname,
	    (int)strlen(mb_dstring) + 1);

    for (p = dstring ; *p ; p++)
	{
	char *pc;
	char mb_p[32]; /* arbitrarily large */
	int  length;

	length = wctomb(mb_p, *p);
	mb_p[length] = 0;

	pc = mb_p;
	while (*pc)
	    {
	    if (*pc == '"' || *pc == '\\') putc('\\', delim);
	    putc(*pc, delim);
	    pc++;
	    }
	}
    fputs("\") ;\n", delim);

    dlmptr[dcount - 1] = MakeMByteString(dname);
    if (m_ntrtrie(dname, &delimtrie, dstruct)) 
	m_err1("Duplicate definition of %s", mb_dname);
    while (c != wnl)
	{
	if (c == EOF) break;
	c = getc(ddat);
	}
    m_free(mb_dname,"multi-byte string");
    m_free(mb_dstring,"multi-byte string");
    }
dumpdlmptr();
loading = FALSE;
}

/* Output transition matrix */
#if defined(M_PROTO)
void nextcon(LOGICAL sparse)
#else
void nextcon(sparse)
  LOGICAL sparse;
#endif
  {
    int i, j;
    int firstinrow;

    if (sparse) {
      for (i = 0 ; i < ccount ; i++) {
        for (j = 0 ; j < dcount ; j++)
          if (transit(i, j)) nonzero++;
        nonzero++;
        }
      fprintf(delim,
        "M_DELIMEXTERN struct {\n  %s context ;\n  %s nextcon ;\n",
          contype, contype);
      fprintf(delim, "  } m_trnsit [%d]\n", nonzero);

      nonzero = 0;
      fprintf(delim, "#if defined(M_DELIMDEF)\n  = {\n");
      fprintf(context, "M_CONEXTERN int m_trnsptr[%d]\n", ccount);
      fprintf(context, "#if defined(M_CONDEF)\n  = {\n");
      for (i = 0 ; i < ccount ; i++) {
        firstinrow = 0;
        for (j = 0 ; j < dcount ; j++) {
          if (! transit(i, j)) continue;
          prtctxt(j, transit(i, j));
          if (! firstinrow) firstinrow = nonzero;
          }
        prtctxt(0, 0);
        if (i > 0) fprintf(context, ",\n");
        fprintf(context, "  %d", firstinrow);
        }
      fprintf(delim, "%s", endif);
      fprintf(context, "%s", endif);
      }
    else {
      fprintf(context,
        "M_CONEXTERN %s m_nextcon[%d][%d]\n#if defined(M_CONDEF)\n  = {\n",
        contype, ccount, dcount);
      for (i = 0 ; i < ccount ; i++) {
        fprintf(context, "  {\n");
        for (j = 0 ; j < dcount ; j++) {
          fprintf(context, "    %d", transit(i, j));
          if (j < dcount - 1) fprintf(context, ",\n");
          }
        fprintf(context, "}");
        if (i < ccount - 1) fprintf(context, ",");
        fprintf(context, "\n");
        }
      fprintf(context, "%s", endif);
      }
    }
  
/* If sparse matrix output option, generate a single element of transit
   array */
void prtctxt(column, value)
  int column, value;
  {
    static LOGICAL first = TRUE;

    if (! first) fprintf(delim, ",\n");
    first = FALSE;
    fprintf(delim, "  {%d, %d}", column, value);
    nonzero++;
    }


/* Read the next input character */
#if defined(M_PROTO)
int readchar(LOGICAL cap)
#else
int readchar(cap)
  LOGICAL cap;
#endif
{
int c;
M_WCHAR wnl;

int ret = mbtowc(&wnl, "\n", 1);
(void) ret;

c = mb_getwc(cdat); /* use mb_getwc so we read multi-byte chars */
if (cap && c != EOF) c = m_upper(c);

if (c == wnl) m_line++;

return(c);
}

/* Called by utility procedure m_error() -- has content in other programs
   that use m_error() */
void skiptoend(M_NOPAR)
{
}

/* Return a character to the input stream for re-reading */
void unread(c)
int c;
{
M_WCHAR wnl;

int ret = mbtowc(&wnl, "\n", 1);
(void) ret;

ungetc(c, cdat);
if (c == wnl) m_line--;
}
