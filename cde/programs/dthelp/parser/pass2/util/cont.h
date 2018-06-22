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
/* $XConsortium: cont.h /main/3 1995/11/08 11:02:46 rswiston $ */
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

void countdown(M_TRIE *parent, int *count);

void dumpdlmptr(void);

void dumpnode(LOGICAL *first, FILE *file, M_TRIE *trieptr, int *count, void (*proc));

void dumptree(LOGICAL sparse);

void enterdelim(int n);

void m_error(char *text);

void m_err1(char *text, char *arg);

void getcode(int n);

void getcolon(void);

int getContext(void);

LOGICAL getdname(void);

void inccon(void);

void incdelim(void);

void loaddelim(void);

int m_lower(int c);

int main(int argc, char **argv);

void *m_malloc(int size, char *msg);

void nextcon(LOGICAL sparse);

void m_openchk(FILE **ptr, char *name, char *mode);

void printval(FILE *file, M_TRIE *value);

void prtctxt(int column, int value);

int readchar(LOGICAL cap);

void skiptoend(void);

void unread(int c);

int m_upper(int c);

void warning(char *text);

void warning1(char *text, char *arg);

void warning2(char *text, char *arg1, char *arg2);

LOGICAL m_whitespace(M_WCHAR c);


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
