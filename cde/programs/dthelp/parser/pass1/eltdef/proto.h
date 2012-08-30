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
/* $XConsortium: proto.h /main/3 1995/11/08 10:05:39 rswiston $ */
/*
                   Copyright (c) 1988, 1989 Hewlett-Packard Co.
*/

/* Proto.h contains function prototypes for program ELTDEF. */


void actptrout(
#if defined(M_PROTO)
  ACTION **array, char *name
#endif
  ) ;

void addent(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL m_allwhite(
#if defined(M_PROTO)
  const M_WCHAR *string
#endif
  ) ;

void closeiffile(
#if defined(M_PROTO)
  LOGICAL flag, FILE *file, int count, char *table, char *proto
#endif
  ) ;

void countdown(
#if defined(M_PROTO)
  M_TRIE *parent, int *count
#endif
  ) ;

void cvalue(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;


void done(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_dumptrie(
#if defined(M_PROTO)
  FILE *file,
  M_TRIE *xtrie,
  char *extname,
  int *count,
  void (*proc)(M_ENTITY *)
#endif
  ) ;

void endcode(
#if defined(M_PROTO)
  LOGICAL flag, FILE *file
#endif
  ) ;

void endini(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void endsignon(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void endstring(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void entout(
#if defined(M_PROTO)
  char *fname
#endif
  ) ;

void enttype(
#if defined(M_PROTO)
  int type
#endif
  ) ;

void m_error(
#if defined(M_PROTO)
  char *text
#endif
  ) ;

void m_err1(
#if defined(M_PROTO)
  const char *text, const M_WCHAR *arg
#endif
  ) ;

void m_mberr1(
#if defined(M_PROTO)
  char *text, const char *arg
#endif
  ) ;

void m_err2(
#if defined(M_PROTO)
  const char *text, const M_WCHAR *arg1, const M_WCHAR *arg2
#endif
  ) ;

void m_err3(
#if defined(M_PROTO)
  char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3
#endif
  ) ;

void m_err5(
#if defined(M_PROTO)
  char *text,
  M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3, M_WCHAR *arg4, M_WCHAR *arg5
#endif
  ) ;

void m_exit(
#if defined(M_PROTO)
  int status
#endif
  ) ;

void m_free(
#if defined(M_PROTO)
  void *block, char *msg
#endif
  ) ;

void freechain(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int getachar(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int *getaction(
#if defined(M_PROTO)
  ACTION **array
#endif
  ) ;

ACTION *getactstruct(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void getname(
#if defined(M_PROTO)
  int first
#endif
  ) ;

int gettoken(
#if defined(M_PROTO)
  int *c, int context
#endif
  ) ;

void m_initctype(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void initialize(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL m_letter(
#if defined(M_PROTO)
  M_WCHAR c
#endif
  ) ;

LOGICAL litproc(
#if defined(M_PROTO)
  int delim
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

void m_openchk(
#if defined(M_PROTO)
  FILE **ptr, char *name, char *mode
#endif
  ) ;

void outpval(
#if defined(M_PROTO)
  M_TRIE *p
#endif
  ) ;

void outstring(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int m_packedlook(
#if defined(M_PROTO)
  M_PTRIE *xptrie, M_WCHAR *name
#endif
  ) ;

const M_WCHAR *m_partype(
#if defined(M_PROTO)
  const int par, const M_WCHAR *string
#endif
  ) ;

int scan(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void skiptoend(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void startcode(
#if defined(M_PROTO)
  int caseno, LOGICAL *flag, FILE *file, char *prefix, char *proto,
  char *formal, char *formtype
#endif
  ) ;

void startelement(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void storecvar(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void storepname(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void undodelim(
#if defined(M_PROTO)
  M_WCHAR *delim
#endif
  ) ;

void ungetachar(
#if defined(M_PROTO)
  int c
#endif
  ) ;

int m_wcupstrcmp(
#if defined(M_PROTO)
  const M_WCHAR *p, const M_WCHAR *q
#endif
  ) ;

int m_wcmbupstrcmp(
#if defined(M_PROTO)
  const M_WCHAR *p, const char *q
#endif
  ) ;

int m_mbmbupstrcmp(
#if defined(M_PROTO)
  const char *p, const char *q
#endif
  ) ;

void value(
#if defined(M_PROTO)
  M_WCHAR *p
#endif
  ) ;

void warning(
#if defined(M_PROTO)
  char *text
#endif
  ) ;

void warning1(
#if defined(M_PROTO)
  char *text, M_WCHAR *arg
#endif
  ) ;

LOGICAL m_whitespace(
#if defined(M_PROTO)
  M_WCHAR c
#endif
  ) ;

