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
/* $XConsortium: error.c /main/3 1995/11/08 11:04:57 rswiston $ */
/* Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co. */

/* Error.c contains procedures for issuing error messages and warnings in
   BUILD and ELTDEF */

#include <stdio.h>
#include "basic.h"

extern FILE *m_errfile ;
extern int m_line ;
extern LOGICAL errexit ;

void eprefix(LOGICAL flag);

void m_errline(char *text);

void m_error(char *text);

void m_err1(char *text, const M_WCHAR *arg);

void m_mberr1(char *text, const char *arg);

void m_err2(const char *text, const M_WCHAR *arg1, const M_WCHAR *arg2);

void m_err3(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3);

void m_err5(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3, M_WCHAR *arg4,
	      M_WCHAR *arg5);

void esuffix(void);

void msgline(char *text);

void msg1line(char *text, char *arg1);

void skiptoend(void);

void m_wctrace(M_WCHAR *text);

void m_trace(char *text);

void warning(char *text);

void warning1(char *text, M_WCHAR *arg);

void warning2(char *text, M_WCHAR *arg1, M_WCHAR *arg2);

void warning3(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3);

void warning4(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3, M_WCHAR *arg4);

/* Write error message prefix */
void eprefix(LOGICAL flag)
  {
    if (flag) errexit = TRUE ;
    fprintf(stderr, "\n**** ") ;
    fprintf(m_errfile, "\n**** ") ;
    }

/* Writes part of an error message.  Called from m_malloc instead of
   m_err1, since PARSER's version of m_err1 calls m_malloc and recursive
   calls are possible when the heap is exhausted */
void m_errline(char *text)
{
fputs(text, stderr) ;
fputs(text, m_errfile) ;
}

/* Writes an error message to standard error and file "error" */
void m_error(char *text)
  {
    eprefix(TRUE) ;
    fprintf(stderr,"%s", text) ;
    fprintf(m_errfile,"%s", text) ;
    esuffix() ;
    skiptoend() ;
  }

/* Writes an error message with one argument to standard error and file
   "error" */
void m_err1(char *text, const M_WCHAR *arg)
{
char *mb_arg;

mb_arg = MakeMByteString(arg);
eprefix(TRUE) ;
fprintf(stderr, text, mb_arg) ;
fprintf(m_errfile, text, mb_arg) ;
esuffix() ;
skiptoend() ;
m_free(mb_arg,"multi-byte string");
}

void m_mberr1(char *text, const char *arg)
{
eprefix(TRUE) ;
fprintf(stderr, text, arg) ;
fprintf(m_errfile, text, arg) ;
esuffix() ;
skiptoend() ;
}

/* Writes an error message with two arguments to standard error and file
   "error" */
void m_err2( const char *text, const M_WCHAR *arg1, const M_WCHAR *arg2)
{
char *mb_arg1, *mb_arg2;

mb_arg1 = MakeMByteString(arg1);
mb_arg2 = MakeMByteString(arg2);
eprefix(TRUE) ;
fprintf(stderr, text, mb_arg1, mb_arg2) ;
fprintf(m_errfile, text, mb_arg1, mb_arg2) ;
esuffix() ;
skiptoend() ;
m_free(mb_arg1,"multi-byte string");
m_free(mb_arg2,"multi-byte string");
}

/* Writes an error message with three arguments to standard error and file
   "error" */
void m_err3( char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3)
{
char *mb_arg1, *mb_arg2, *mb_arg3;

mb_arg1 = MakeMByteString(arg1);
mb_arg2 = MakeMByteString(arg2);
mb_arg3 = MakeMByteString(arg3);
eprefix(TRUE) ;
fprintf(stderr, text, mb_arg1, mb_arg2, mb_arg3) ;
fprintf(m_errfile, text, mb_arg1, mb_arg2, mb_arg3) ;
esuffix() ;
skiptoend() ;
m_free(mb_arg1,"multi-byte string");
m_free(mb_arg2,"multi-byte string");
m_free(mb_arg3,"multi-byte string");
}

/* Writes an error message with five arguments to standard error and file
   "error" */
void m_err5( char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3,
	     M_WCHAR *arg4, M_WCHAR *arg5)
{
char *mb_arg1, *mb_arg2, *mb_arg3, *mb_arg4, *mb_arg5;

mb_arg1 = MakeMByteString(arg1);
mb_arg2 = MakeMByteString(arg2);
mb_arg3 = MakeMByteString(arg3);
mb_arg4 = MakeMByteString(arg4);
mb_arg5 = MakeMByteString(arg5);
fprintf(stderr, text, mb_arg1, mb_arg2, mb_arg3, mb_arg4, mb_arg5) ;
fprintf(m_errfile, text, mb_arg1, mb_arg2, mb_arg3, mb_arg4, mb_arg5) ;
esuffix() ;
skiptoend() ;
m_free(mb_arg1,"multi-byte string");
m_free(mb_arg2,"multi-byte string");
m_free(mb_arg3,"multi-byte string");
m_free(mb_arg4,"multi-byte string");
m_free(mb_arg5,"multi-byte string");
}

/* Write error message suffix */
void esuffix(void)
  {
    fprintf(stderr, " (line %d) ****\n", m_line) ;
    fprintf(m_errfile, " (line %d) ****\n", m_line) ;
    }

/* Print something to both stderr and m_errfile */
void msgline(char *text)
  {
    fprintf(stderr, "%s", text) ;
    fprintf(m_errfile, "%s", text) ;
    }

/* Print something to both stderr and m_errfile */
void msg1line(char *text, char *arg1)
  {
    fprintf(stderr, text, arg1) ;
    fprintf(m_errfile, text, arg1) ;
    }

/* Trace used for m_malloc trace output */
void m_trace(char *text)
{
m_errline(text) ;
}

void m_wctrace(M_WCHAR *text)
{
char *mb_text;

mb_text = MakeMByteString(text);
m_trace(mb_text);
m_free(mb_text,"multi-byte string");
}

/* Writes a nonfatal error message to standard error and file "error" */
void warning(char *text)
  {
    eprefix(FALSE) ;
    fprintf(stderr,"%s", text) ;
    fprintf(m_errfile,"%s", text) ;
    esuffix() ;
  }

/* Writes a nonfatal error message with one argument to standard error
   and file "error" */
void warning1( char *text, M_WCHAR *arg)
{
char *mb_arg;

mb_arg = MakeMByteString(arg);
eprefix(FALSE) ;
fprintf(stderr, text, mb_arg) ;
fprintf(m_errfile, text, mb_arg) ;
esuffix() ;
m_free(mb_arg,"multi-byte string");
}

/* Writes a nonfatal error message with two arguments to standard error
   and file "error" */
void warning2(char *text, M_WCHAR *arg1, M_WCHAR *arg2)
{
char *mb_arg1, *mb_arg2;

mb_arg1 = MakeMByteString(arg1);
mb_arg2 = MakeMByteString(arg2);
eprefix(FALSE) ;
fprintf(stderr, text, mb_arg1, mb_arg2) ;
fprintf(m_errfile, text, mb_arg1, mb_arg2) ;
esuffix() ;
m_free(mb_arg1,"multi-byte string");
m_free(mb_arg2,"multi-byte string");
}

/* Writes a nonfatal error message with three arguments to standard error
   and file "error" */
void warning3(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3)
{
char *mb_arg1, *mb_arg2, *mb_arg3;

mb_arg1 = MakeMByteString(arg1);
mb_arg2 = MakeMByteString(arg2);
mb_arg3 = MakeMByteString(arg3);
eprefix(FALSE) ;
fprintf(stderr, text, mb_arg1, mb_arg2, mb_arg3) ;
fprintf(m_errfile, text, mb_arg1, mb_arg2, mb_arg3) ;
esuffix() ;
m_free(mb_arg1,"multi-byte string");
m_free(mb_arg2,"multi-byte string");
m_free(mb_arg3,"multi-byte string");
}

/* Writes a nonfatal error message with four arguments to standard error
   and file "error" */
void warning4(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3,
	      M_WCHAR *arg4)
{
char *mb_arg1, *mb_arg2, *mb_arg3, *mb_arg4;

mb_arg1 = MakeMByteString(arg1);
mb_arg2 = MakeMByteString(arg2);
mb_arg3 = MakeMByteString(arg3);
mb_arg4 = MakeMByteString(arg4);
eprefix(FALSE) ;
fprintf(stderr, text, mb_arg1, mb_arg2, mb_arg3, mb_arg4) ;
fprintf(m_errfile, text, mb_arg1, mb_arg2, mb_arg3, mb_arg4) ;
esuffix() ;
m_free(mb_arg1,"multi-byte string");
m_free(mb_arg2,"multi-byte string");
m_free(mb_arg3,"multi-byte string");
m_free(mb_arg4,"multi-byte string");
}
