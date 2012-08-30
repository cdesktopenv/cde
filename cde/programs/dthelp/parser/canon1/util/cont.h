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
/* $XConsortium: cont.h /main/3 1995/11/08 09:47:47 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
               Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Cont.h is the primary include file for program CONTEXT. */

#if defined(CONTDEF)
#define CONTEXTERN
#define CONTINIT(a) = a
#else
#define CONTEXTERN extern
#define CONTINIT(a)
#endif

void countdown(
#if defined(M_PROTO)
  M_TRIE *parent, int *count
#endif
  ) ;

void dumpdlmptr(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void dumpnode(
#if defined(M_PROTO)
  LOGICAL *first, FILE *file, M_TRIE *trieptr, int *count, void (*proc)
#endif
  ) ;

void dumptree(
#if defined(M_PROTO)
  LOGICAL sparse
#endif
  ) ;

void enterdelim(
#if defined(M_PROTO)
  int n
#endif
  ) ;

void m_error(
#if defined(M_PROTO)
  char *text
#endif
  ) ;

void m_err1(
#if defined(M_PROTO)
  char *text, char *arg
#endif
  ) ;

void getcode(
#if defined(M_PROTO)
  int n
#endif
  ) ;

void getcolon(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int getContext(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL getdname(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void inccon(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void incdelim(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void loaddelim(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int m_lower(
#if defined(M_PROTO)
  int c
#endif
  ) ;

int main(
#if defined(M_PROTO)
  int argc, char **argv
#endif
  ) ;

void *m_malloc(
#if defined(M_PROTO)
  int size, char *msg
#endif
  ) ;

void nextcon(
#if defined(M_PROTO)
  LOGICAL sparse
#endif
  ) ;

void m_openchk(
#if defined(M_PROTO)
  FILE **ptr, char *name, char *mode
#endif
  ) ;

void printval(
#if defined(M_PROTO)
  FILE *file, M_TRIE *value
#endif
  ) ;

void prtctxt(
#if defined(M_PROTO)
  int column, int value
#endif
  ) ;

int readchar(
#if defined(M_PROTO)
  LOGICAL cap
#endif
  ) ;

void skiptoend(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void unread(
#if defined(M_PROTO)
  int c
#endif
  ) ;

int m_upper(
#if defined(M_PROTO)
  int c
#endif
  ) ;

void warning(
#if defined(M_PROTO)
  char *text
#endif
  ) ;

void warning1(
#if defined(M_PROTO)
  char *text, char *arg
#endif
  ) ;

void warning2(
#if defined(M_PROTO)
  char *text, char *arg1, char *arg2
#endif
  ) ;

LOGICAL m_whitespace(
#if defined(M_PROTO)
  M_WCHAR c
#endif
  ) ;


#define NOMORE -1

/* Increase by 10 percent */
#define m_plus10p(x) (x + (x > 10 ? x / 10 : 1))

CONTEXTERN int NUMCON CONTINIT(100) ;
#define CNAMELEN 12
#define DNAMELEN 12
#define DELIMLEN 25
CONTEXTERN int NUMDELIM CONTINIT(100) ;

#define contype ccount <= 255 ? "unsigned char" : "int"

CONTEXTERN LOGICAL m_heapchk CONTINIT(FALSE) ;
CONTEXTERN LOGICAL m_malftrace CONTINIT(FALSE) ;

#define ENDIFSTRING "}\n#endif\n  ;\n" 
CONTEXTERN char endif[sizeof(ENDIFSTRING)] CONTINIT(ENDIFSTRING) ;

CONTEXTERN LOGICAL casestarted ;
CONTEXTERN LOGICAL errexit CONTINIT(FALSE) ;

CONTEXTERN FILE *delim ;
CONTEXTERN FILE *context ;
CONTEXTERN FILE *fcase, *cdat, *ddat, *m_errfile ;

CONTEXTERN M_TRIE delimtrie ;

CONTEXTERN M_TRIE **contree ;
CONTEXTERN int *xtransit ;
#define transit(i,j) xtransit[i * NUMDELIM + j]

CONTEXTERN M_WCHAR **contexts ;
CONTEXTERN int ccount CONTINIT(0) ;

CONTEXTERN M_WCHAR dname[DNAMELEN + 1] ;
CONTEXTERN M_WCHAR dstring[DELIMLEN + 1] ;
CONTEXTERN int dcount CONTINIT(0) ;

CONTEXTERN struct dstruct {
  int count ;
  M_WCHAR *string ;
  } *dstruct ;
CONTEXTERN LOGICAL withdelim ;
CONTEXTERN LOGICAL loading ;
CONTEXTERN int curdelim ;

CONTEXTERN char **dlmptr ;

CONTEXTERN LOGICAL first CONTINIT(TRUE) ;
CONTEXTERN int maxd CONTINIT(0) ;

CONTEXTERN int m_line CONTINIT(1) ;

CONTEXTERN int nonzero CONTINIT(0) ;
