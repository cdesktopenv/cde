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
/* $XConsortium: error.c /main/3 1995/11/08 11:23:01 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Util.c contains general utilities for program PARSER */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(MSDOS)
#include <process.h>
#endif
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"
#include "entext.h"


/* Issue error message (no arguments) */
void m_error(text)
  char *text;
  {
    m_startmsg();
    m_errline(text);
    m_esuffix();
  }

/* Issue error message (one argument) */
#if defined(M_PROTO)
void m_err1(
  const char    *text,
  const M_WCHAR *arg)
#else
void m_err1(text, arg)
  char    *text;
  M_WCHAR *arg;
#endif
{
char *buffer, *mb_arg;

m_startmsg();
/* Subtract 2 characters for the %s and add an EOS*/
mb_arg = MakeMByteString(arg);
buffer = (char *)
    m_malloc(strlen(text) + strlen(mb_arg) - 2 + 1, "error message");
sprintf(buffer, text, mb_arg);
m_errline(buffer);
m_free(buffer, "error message");
m_free(mb_arg,"multi-byte string");
m_esuffix();
}


#if defined(M_PROTO)
void m_mberr1(
  const char *text,
  const char *arg)
#else
void m_err1(text, arg)
  char    *text;
  char *arg;
#endif
  {
  M_WCHAR *wc_arg;

  wc_arg = MakeWideCharString(arg);
  m_err1(text, wc_arg);
  m_free(wc_arg,"wide character string");
  }

/* Issue error message (two arguments) */
#if defined(M_PROTO)
void m_err2(
  const char    *text,
  const M_WCHAR *arg1,
  const M_WCHAR *arg2)
#else
void m_err2(text, arg1, arg2)
  char    *text;
  M_WCHAR *arg1;
  M_WCHAR *arg2;
#endif
  {
    char *buffer;
    char *mb_arg1,*mb_arg2;

    m_startmsg();
    /* Subtract 2 characters for each of 2 %s's and add an EOS*/
    mb_arg1 = MakeMByteString(arg1);
    mb_arg2 = MakeMByteString(arg2);
    buffer = (char *) m_malloc(
		      strlen(text) + strlen(mb_arg1) + strlen(mb_arg2) - 4 + 1,
			       "error message");
    sprintf(buffer, text, mb_arg1, mb_arg2);
    m_errline(buffer);
    m_free(buffer, "error message");
    m_free(mb_arg1,"multi-byte string");
    m_free(mb_arg2,"multi-byte string");
    m_esuffix();
  }

/* Issue error message (three arguments) */
#if defined(M_PROTO)
void m_err3(
  char    *text,
  M_WCHAR *arg1,
  M_WCHAR *arg2,
  M_WCHAR *arg3)
#else
void m_err3(text, arg1, arg2, arg3)
  char    *text;
  M_WCHAR *arg1;
  M_WCHAR *arg2;
  M_WCHAR *arg3;
#endif
  {
    char *buffer;
    char *mb_arg1, *mb_arg2, *mb_arg3;

    m_startmsg();
    /* Subtract 2 characters for each of 3 %s's and add an EOS*/
    mb_arg1 = MakeMByteString(arg1);
    mb_arg2 = MakeMByteString(arg2);
    mb_arg3 = MakeMByteString(arg3);
    buffer = (char *) m_malloc(
      strlen(text) +
	strlen(mb_arg1) +
	strlen(mb_arg2) +
	strlen(mb_arg3) - 6 + 1,
      "error message");
    sprintf(buffer, text, mb_arg1, mb_arg2, mb_arg3);
    m_errline(buffer);
    m_free(buffer, "error message");
    m_free(mb_arg1,"multi-byte string");
    m_free(mb_arg2,"multi-byte string");
    m_free(mb_arg3,"multi-byte string");
    m_esuffix();
  }

/* Issue error message (four arguments) */
#if defined(M_PROTO)
void m_err4(
  char    *text,
  M_WCHAR *arg1,
  M_WCHAR *arg2,
  M_WCHAR *arg3,
  M_WCHAR *arg4)
#else
void m_err4(text, arg1, arg2, arg3, arg4)
  char    *text;
  M_WCHAR *arg1;
  M_WCHAR *arg2;
  M_WCHAR *arg3;
  M_WCHAR *arg4;
#endif
  {
    char *buffer;
    char *mb_arg1, *mb_arg2,*mb_arg3,*mb_arg4;

    m_startmsg();
    /* Subtract 2 characters for each of 4 %s's and add an EOS*/
    mb_arg1 = MakeMByteString(arg1);
    mb_arg2 = MakeMByteString(arg2);
    mb_arg3 = MakeMByteString(arg3);
    mb_arg4 = MakeMByteString(arg4);
    buffer = (char *) m_malloc(
      strlen(text) +
	strlen(mb_arg1) +
	strlen(mb_arg2) +
	strlen(mb_arg3) +
	strlen(mb_arg4) - 8 + 1,
      "error message");
    sprintf(buffer, text, arg1, arg2, arg3, arg4);
    m_errline(buffer);
    m_free(buffer, "error message");
    m_free(mb_arg1,"multi-byte string");
    m_free(mb_arg2,"multi-byte string");
    m_free(mb_arg3,"multi-byte string");
    m_free(mb_arg4,"multi-byte string");
    m_esuffix();
  }

/* Issue error message (five arguments) */
#if defined(M_PROTO)
void m_err5(
  char    *text,
  M_WCHAR *arg1,
  M_WCHAR *arg2,
  M_WCHAR *arg3,
  M_WCHAR *arg4,
  M_WCHAR *arg5)
#else
void m_err5(text, arg1, arg2, arg3, arg4, arg5)
  char    *text;
  M_WCHAR *arg1;
  M_WCHAR *arg2;
  M_WCHAR *arg3;
  M_WCHAR *arg4;
  M_WCHAR *arg5;
#endif
  {
    char *buffer;
    char *mb_arg1, *mb_arg2, *mb_arg3, *mb_arg4, *mb_arg5;

    m_startmsg();
    /* Subtract 2 characters for each of 5 %s's and add an EOS*/
    mb_arg1 = MakeMByteString(arg1);
    mb_arg2 = MakeMByteString(arg2);
    mb_arg3 = MakeMByteString(arg3);
    mb_arg4 = MakeMByteString(arg4);
    mb_arg5 = MakeMByteString(arg5);
    buffer = (char *) m_malloc(
      strlen(text) +
	strlen(mb_arg1) +
	strlen(mb_arg2) +
	strlen(mb_arg3) +
	strlen(mb_arg4) +
	strlen(mb_arg5) - 10 + 1,
      "error message");
    sprintf(buffer,
	    text,
	    mb_arg1,
	    mb_arg2,
	    mb_arg3,
	    mb_arg4,
	    mb_arg5);
    m_errline(buffer);
    m_free(buffer, "error message");
    m_free(mb_arg1,"multi-byte string");
    m_free(mb_arg2,"multi-byte string");
    m_free(mb_arg3,"multi-byte string");
    m_free(mb_arg4,"multi-byte string");
    m_free(mb_arg5,"multi-byte string");
    m_esuffix();
  }

/* Issue error message (six arguments) */
#if defined(M_PROTO)
void m_err6(
  char    *text,
  M_WCHAR *arg1,
  M_WCHAR *arg2,
  M_WCHAR *arg3,
  M_WCHAR *arg4,
  M_WCHAR *arg5,
  M_WCHAR *arg6)
#else
void m_err6(text, arg1, arg2, arg3, arg4, arg5, arg6)
  char    *text;
  M_WCHAR *arg1;
  M_WCHAR *arg2;
  M_WCHAR *arg3;
  M_WCHAR *arg4;
  M_WCHAR *arg5;
  M_WCHAR *arg6;
#endif
  {
    char *buffer;
    char *mb_arg1, *mb_arg2, *mb_arg3, *mb_arg4, *mb_arg5, *mb_arg6;

    m_startmsg();
    /* Subtract 2 characters for each of 6 %s's and add an EOS*/
    mb_arg1 = MakeMByteString(arg1);
    mb_arg2 = MakeMByteString(arg2);
    mb_arg3 = MakeMByteString(arg3);
    mb_arg4 = MakeMByteString(arg4);
    mb_arg5 = MakeMByteString(arg5);
    mb_arg6 = MakeMByteString(arg6);
    buffer = (char *) m_malloc(
      strlen(text) +
	strlen(mb_arg1) +
	strlen(mb_arg2) +
	strlen(mb_arg3) +
	strlen(mb_arg4) +
	strlen(mb_arg5) +
	strlen(mb_arg6) - 12 + 1,
      "error message");
    sprintf(buffer,
	    text,
	    mb_arg1,
	    mb_arg2,
	    mb_arg3,
	    mb_arg4,
	    mb_arg5,
	    mb_arg6);
    m_errline(buffer);
    m_free(buffer, "error message");
    m_free(mb_arg1,"multi-byte string");
    m_free(mb_arg2,"multi-byte string");
    m_free(mb_arg3,"multi-byte string");
    m_free(mb_arg4,"multi-byte string");
    m_free(mb_arg5,"multi-byte string");
    m_free(mb_arg6,"multi-byte string");
    m_esuffix();
  }

/* Issue error message (seven arguments) */
#if defined(M_PROTO)
void m_err7(
  char    *text,
  M_WCHAR *arg1,
  M_WCHAR *arg2,
  M_WCHAR *arg3,
  M_WCHAR *arg4,
  M_WCHAR *arg5,
  M_WCHAR *arg6,
  M_WCHAR *arg7)
#else
void m_err7(text, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
  char    *text;
  M_WCHAR *arg1;
  M_WCHAR *arg2;
  M_WCHAR *arg3;
  M_WCHAR *arg4;
  M_WCHAR *arg5;
  M_WCHAR *arg6;
  M_WCHAR *arg7;
#endif
  {
    char *buffer;
    char *mb_arg1, *mb_arg2, *mb_arg3, *mb_arg4, *mb_arg5, *mb_arg6, *mb_arg7;

    m_startmsg();
    /* Subtract 2 characters for each of 7 %s's and add an EOS*/
    mb_arg1 = MakeMByteString(arg1);
    mb_arg2 = MakeMByteString(arg2);
    mb_arg3 = MakeMByteString(arg3);
    mb_arg4 = MakeMByteString(arg4);
    mb_arg5 = MakeMByteString(arg5);
    mb_arg6 = MakeMByteString(arg6);
    mb_arg7 = MakeMByteString(arg7);
    buffer = (char *) m_malloc(
      strlen(text) +
	strlen(mb_arg1) +
	strlen(mb_arg2) +
	strlen(mb_arg3) +
	strlen(mb_arg4) +
	strlen(mb_arg5) +
	strlen(mb_arg6) +
	strlen(mb_arg7) - 14 + 1,
      "error message");
    sprintf(buffer,
	    text,
	    mb_arg1,
	    mb_arg2,
	    mb_arg3,
	    mb_arg4,
	    mb_arg5,
	    mb_arg6,
	    mb_arg7);
    m_errline(buffer);
    m_free(buffer, "error message");
    m_free(mb_arg1,"multi-byte string");
    m_free(mb_arg2,"multi-byte string");
    m_free(mb_arg3,"multi-byte string");
    m_free(mb_arg4,"multi-byte string");
    m_free(mb_arg5,"multi-byte string");
    m_free(mb_arg6,"multi-byte string");
    m_free(mb_arg7,"multi-byte string");
    m_esuffix();
  }

/* Getline.c returns the name of the current input file and the number
   of the current line */
void m_getline(file, line)
  M_WCHAR **file;
  int *line;
  {
    int i;

    if (m_sysecnt) {
      for (i = m_eopencnt ; i ; i--)
        if (m_opene[i - 1]->type == M_SYSTEM) break;
      *file = m_opene[i - 1]->content;
      }
    else *file = NULL;
    *line = m_line[m_sysecnt];
    }

/* Dump last characters read from input stream to give context to
   an error message */
void m_lastchars(M_NOPAR)
{
int i;
M_WCHAR buffer[2];

buffer[1] = M_EOS;
if (m_svlnwrap[m_sysecnt] || m_svlncnt[m_sysecnt])
m_errline("...");
if (m_svlnwrap[m_sysecnt])
for (i = m_svlncnt[m_sysecnt] ; i < M_LINELENGTH ; i++)
    {
    char *mb_buffer;

    buffer[0] = (M_WCHAR) m_saveline[i][m_sysecnt];
    if (buffer[0] == (M_WCHAR) -1)
	{
	m_errline("\n");
	}
    else
	{
	mb_buffer = MakeMByteString(buffer);
	m_errline(mb_buffer);
	m_free(mb_buffer,"multi-byte string");
	}
    }
for (i = 0 ; i < m_svlncnt[m_sysecnt] ; i++)
    {
    char *mb_buffer;
    buffer[0] = (M_WCHAR) m_saveline[i][m_sysecnt];

    if (buffer[0] == (M_WCHAR) -1)
	{
	m_errline("\n");
	}
    else
	{
	mb_buffer = MakeMByteString(buffer);
	m_errline(mb_buffer);
	m_free(mb_buffer,"multi-byte string");
	}
    }
if ((m_svlncnt[m_sysecnt] &&
     m_saveline[m_svlncnt[m_sysecnt] - 1][m_sysecnt] != M_RE) ||
    (! m_svlncnt[m_sysecnt] && m_svlnwrap[m_sysecnt] &&
     m_saveline[M_LINELENGTH - 1][m_sysecnt] != M_RE)
   ) m_errline("...\n");
}

/* Display information about the current element */
void m_showcurelt(M_NOPAR)
  {
    if (m_stacktop->oldtop)
      m_dispcurelt(m_stacktop->file, m_stacktop->line);
    }    

/* Start an error message */
void m_startmsg(M_NOPAR)
  {
    m_eprefix();
    m_errexit = TRUE;
    }    

/* Return the name of the current input file (most recently opened, but
   unclosed SYSTEM entity) */
M_WCHAR *m_thisfile(M_NOPAR)
  {
    int i;

    if (m_sysecnt) {
      for (i = m_eopencnt ; i ; i--)
        if (m_opene[i - 1]->type == M_SYSTEM)
          return(m_opene[i - 1]->content);
      }
    return(NULL);
    }

/* Return the line number in the current input file */
int m_thisline(M_NOPAR)
  {
    return(m_line[m_sysecnt]);
    }
