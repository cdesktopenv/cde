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
/* $XConsortium: custom.c /main/7 1996/11/15 11:55:52 rswiston $ */
/*
Copyright (c) 1988, 1989 Hewlett-Packard Co.
*/

/* Custom.c contains standard PARSER functions, customized for the HP
   HelpTag formatting system. */

#include "userinc.h"
#include "globdec.h"
#include <stdlib.h>

/* Write input file and line number for an error message */
void m_dumpline(M_WCHAR *file, int line)
{
char buffer[10];
char *mbyte;

m_errline("Line ");
sprintf(buffer, "%d", line);
m_errline(buffer);
if (!file)
    { /* no entity file */
    if (inFileName)
	{
	m_errline(" of ");
	m_errline(inFileName);
	}
    }
else
    { /* yes, entity file */
    mbyte = MakeMByteString(file);
    m_errline(" of ");
    m_errline(mbyte);
    m_free(mbyte, "multi-byte string");
    }
}

/* Write error message prefix */
void m_eprefix(void)
{
m_errline("\n*****\n");
m_dumpline(m_thisfile(), m_thisline());
m_errline(",\n");
}

/* Process error message text */
void m_errline(char *p)
{
char c;

for ( ; *p ; p++)
    {
    if (m_errfile) putc(*p, m_errfile);
    putc(*p, stderr);
    }
}

/* Write error message suffix */
void m_esuffix(void)
{
m_errline(":\n");
m_lastchars();
if (++m_errcnt == m_errlim)
    {
    m_error("Too many errors, processing stopped");
    m_exit(TRUE);
    }
}

/* Exit procedure */
void m_exit(int status)
{

if (status)
    {
    if (status == 77) /* tell helptag to re-run for forward xrefs */
	{
	if (stoponerror)
	    {
	    if (m_errcnt == 0)
		exit(77);
	    else
		exit(1);
	    }
	else
	    exit(66);
	}

    if (stoponerror)
	exit(1); /* tell helptag to quit */

    exit(2); /* tell helptag to continue to next phases */
    }

exit(0);
}

/* Get-char procedure */
int m_getc(void *m_ptr)
{
int  c;
M_WCHAR wc;
char badch[2];
char mbyte[32]; /* make this bigger than any possible multi-byte char */
int  length;
static M_WCHAR wcr = 0, wsb, wsp, wtb;
char tab, space;

/* Unix/Dos compatibility: 0D0A handling */
if (!wcr)
    {
    mbtowc(&wcr, "\r", 1);
    mbtowc(&wsb, "\032", 1);

    space = M_SPACE;
    mbtowc(&wsp, &space, 1);

    tab = M_TAB;
    mbtowc(&wtb, &tab, 1);
    }

do  {
    length = 0;

#ifdef NO_IBM_PATCH_U446072
    /* -------------------------------------------------------------- */
    /*  Note that the call to fflush before each call to getc is      */
    /*  a workaround for a bug in the re-entrant (thread-safe) getc   */
    /*  function. Earlier versions of AIX did not have this problem.  */
    /*  Made it very specific to AIX 4.2. The bug might appear again  */
    /*  in future releases of AIX. Hopefully this comment would help  */
    /*  to identify the problem quickly in case it appears again.     */
    /* -------------------------------------------------------------- */
    fflush(NULL);
#endif

    if ((c = getc((FILE *)m_ptr)) == EOF) return(EOF);

#ifdef NO_IBM_PATCH_U446072
    fflush(NULL);
#endif

    while (1)
    {
	mbyte[length++] = c;
	mbyte[length]   = 0;
	if (mblen(mbyte,length) != -1) break; /* hurray! */
	if (length == MB_CUR_MAX)
        { /* reached max without a hit */
	    m_error("An invalid multi-byte character was found in the input");
	    c = ' ';
            length = 1;
	    break;
        }

#ifdef NO_IBM_PATCH_U446072
        /* -------------------------------------------------------------- */
        /*  Note that the call to fflush before each call to getc is      */
        /*  a workaround for a bug in the re-entrant (thread-safe) getc   */
        /*  function. Earlier versions of AIX did not have this problem.  */
        /*  Made it very specific to AIX 4.2. The bug might appear again  */
        /*  in future releases of AIX. Hopefully this comment would help  */
        /*  to identify the problem quickly in case it appears again.     */
        /* -------------------------------------------------------------- */
        fflush(NULL);
#endif

        if ((c = getc((FILE *) m_ptr)) == EOF)
        { /* huh? */
	    m_error("End-of-file found in within a multi-byte character");
	    return(EOF);
        }
    }
    mbtowc(&wc,mbyte,length);
}

while ((wc == wcr) || (wc == wsb));

/* Change tabs to spaces */
if (wc == wtb) return((int) wsp);
return((int) wc);
}

/* Open SYSTEM entity procedure */
void *m_openent(M_WCHAR *entcontent)
{
FILE *open;
char *filename;
SEARCH *searchp;
char *mb_entcontent;

mb_entcontent = MakeMByteString(entcontent);
if (!mb_entcontent || !*mb_entcontent) {
    m_free(mb_entcontent, "multi-byte string");
    return NULL; /* null file name, don't open a directory */
}

open = fopen(mb_entcontent, "r");
if (open)
    {
    m_free(mb_entcontent, "multi-byte string");
    return((void *) open);
    }

for (searchp = path ; searchp ; searchp = searchp->next)
    {
    filename = (char *)
	     m_malloc(strlen(searchp->directory) +
			strlen(mb_entcontent) + 1,
		      "filename");
    strcpy(filename, searchp->directory);
    strcat(filename, mb_entcontent);
    open = fopen(filename, "r");
    m_free(filename, "filename");
    if (open)
	{
	m_free(mb_entcontent, "multi-byte string");
	return((void *) open);
	}
    }

m_free(mb_entcontent, "multi-byte string");
return(NULL);
}

/* Open input file */
void *m_openfirst(void)
{
return((void *) inFile);
}

/* Set program options */
void m_setoptions(void)
{
if (m_argc > 2)
    {
    m_optstring(m_argv[2]);
    }
}

/* Process signon message text, stripping out MARKUP version number, so
   only one version number will appear */
void m_signmsg(char *p)
  {
    char *q;
    char *pCopy;

    if (q = strstr(p, VERSION)) {
      pCopy = strdup(p);
      q = strstr(pCopy, VERSION);
      *q = M_EOS;
      m_errline(pCopy);
      free(pCopy);
      return;
      }
    m_errline(p);
    }

/* All entity declarations have been processed. */
void m_startdoc(void)
{
}

/* Write debugging trace information */
void m_trace(char *p)
{
}


void m_wctrace(M_WCHAR *p)
{
char *mb_p;

mb_p = MakeMByteString(p);
m_trace(mb_p);
m_free(mb_p,"multi-byte string");
}
