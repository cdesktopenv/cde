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
/* $XConsortium: proto.h /main/3 1995/11/08 09:26:24 rswiston $ */
/*
                   Copyright (c) 1988, 1989 Hewlett-Packard Co.
*/

/* Proto.h contains function prototypes for program BUILD. */


int addarc(
#if defined(M_PROTO)
  STATE *from, STATE *to, ELTSTRUCT *label, ANDGROUP *and, LOGICAL optional, 
  int id, LOGICAL minim, ELTSTRUCT **errelt
#endif
  ) ;

void adddefent(
#if defined(M_PROTO)
  M_WCHAR *mapname
#endif
  ) ;

LOGICAL addent(
#if defined(M_PROTO)
  M_WCHAR *name
#endif
  ) ;

void addex(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void addkeyword(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL addmapname(
#if defined(M_PROTO)
  M_WCHAR *p, LOGICAL define
#endif
  ) ;

void addndent(
#if defined(M_PROTO)
  M_WCHAR *p
#endif
  ) ;

void addpar(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void addsref(
#if defined(M_PROTO)
  M_WCHAR *p
#endif
  ) ;

LOGICAL m_allwhite(
#if defined(M_PROTO)
  const M_WCHAR *string
#endif
  ) ;

void checkand(
#if defined(M_PROTO)
  ANDGROUP *andstart, ANDGROUP *andptr, STATE *start, TREE *root, 
  ELTSTRUCT **errelt
#endif
  ) ;

LOGICAL checkdefault(
#if defined(M_PROTO)
  const M_WCHAR *string
#endif
  ) ;

int checkdfsa(
#if defined(M_PROTO)
  STATE *from, ELTSTRUCT *label, ANDGROUP *and, int id, ELTSTRUCT **errelt
#endif
  ) ;

int checkrepeat(
#if defined(M_PROTO)
  STATE *from, ANDGROUP *and, ELTSTRUCT **errelt
#endif
  ) ;

void copyintolist(
#if defined(M_PROTO)
  STATELIST *from, STATELIST **to
#endif
  ) ;

void countdown(
#if defined(M_PROTO)
  M_TRIE *parent, int *count
#endif
  ) ;

void defmapname(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

char *deftype(
#if defined(M_PROTO)
  int n
#endif
  ) ;

void dellist(
#if defined(M_PROTO)
  STATELIST **list
#endif
  ) ;

void delstartarcs(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void done(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void dumpentnode(
#if defined(M_PROTO)
  FILE *file, M_TRIE *value
#endif
  ) ;

void dumpmapnode(
#if defined(M_PROTO)
  FILE *file, M_TRIE *value
#endif
  ) ;

void dumpsrefnode(
#if defined(M_PROTO)
  FILE *file,
  M_TRIE *value
#endif
  ) ;

void dumpptrie(
#if defined(M_PROTO)
  M_TRIE *intname, char *extname, FILE *file, char *externdef, char *defdef,
  void (*proc)( FILE *, M_TRIE *)
#endif
  ) ;

void m_dumptrie(
#if defined(M_PROTO)
  FILE   *file,
  M_TRIE *xtrie,
  char   *extname,
  int    *count,
  void  (*proc)(M_ENTITY *)
#endif
  ) ;

void eltblocks(
#if defined(M_PROTO)
  FILE *tempfile
#endif
  ) ;

void eltreeout(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void endmodel(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void entout(
#if defined(M_PROTO)
  char *fname
#endif
  ) ;

char *enttype(
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

void exout(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void m_free(
#if defined(M_PROTO)
  void *block, char *msg
#endif
  ) ;

void freetree(
#if defined(M_PROTO)
  TREE *ruletree
#endif
  ) ;

void found(
#if defined(M_PROTO)
  LOGICAL *flag, char *delim
#endif
  ) ;

void fsa(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int getachar(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

ANDGROUP *getand(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL getname(
#if defined(M_PROTO)
  int first
#endif
  ) ;

STATE *getstate(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int gettoken(
#if defined(M_PROTO)
  int *c, int context
#endif
  ) ;

TREE *gettreenode(
#if defined(M_PROTO)
  M_NOPAR
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

void makeand(
#if defined(M_PROTO)
  LOGICAL *canbenull, TREE *root, int optional
#endif
  ) ;

LOGICAL makefsa(
#if defined(M_PROTO)
  TREE *root, int optional
#endif
  ) ;

void makeor(
#if defined(M_PROTO)
  LOGICAL *canbenull, TREE *root
#endif
  ) ;

void makeseq(
#if defined(M_PROTO)
  LOGICAL *canbenull, TREE *root, int optional
#endif
  ) ;

void *m_malloc(
#if defined(M_PROTO)
  int size, char *msg
#endif
  ) ;

void msgline(
#if defined(M_PROTO)
  char *text
#endif
  ) ;

void msg1line(
#if defined(M_PROTO)
  char *text, char *arg1
#endif
  ) ;

void nondeterm(
#if defined(M_PROTO)
  TREE *root, int c, ELTSTRUCT *eltp
#endif
  ) ;

void normdefault(
#if defined(M_PROTO)
  M_WCHAR *string
#endif
  ) ;

LOGICAL notinlist(
#if defined(M_PROTO)
  STATELIST *item, STATELIST *list
#endif
  ) ;

ELTSTRUCT *ntrelt(
#if defined(M_PROTO)
  M_WCHAR *p
#endif
  ) ;

void m_openchk(
#if defined(M_PROTO)
  FILE **ptr, char *name, char *mode
#endif
  ) ;

void parout(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

char *partype(
#if defined(M_PROTO)
  int n
#endif
  ) ;

LOGICAL permitspcd(
#if defined(M_PROTO)
  ARC *a
#endif
  ) ;

void pop(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void prulend(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void push(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL regenerate(
#if defined(M_PROTO)
  TREE *start, TREE *stop
#endif
  ) ;

void repeat(
#if defined(M_PROTO)
  TREE *root
#endif
  ) ;

void ruleinit(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void rulend(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

LOGICAL samelabelarc(
#if defined(M_PROTO)
  ARC *a, STATE *s
#endif
  ) ;

void savelhs(
#if defined(M_PROTO)
  LOGICAL param
#endif
  ) ;

void savestartarcs(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int scan(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

void setdefault(
#if defined(M_PROTO)
  const M_WCHAR *string
#endif
  ) ;

void simplebranch(
#if defined(M_PROTO)
  TREE *root, ELTSTRUCT *value, ANDGROUP *group, int optional
#endif
  ) ;

void skiptoend(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

int m_sprscon(
#if defined(M_PROTO)
  int i, int j
#endif
  ) ;

void srefout(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

STATE *startfsa(
#if defined(M_PROTO)
  TREE *root, LOGICAL *canbenull
#endif
  ) ;

void tempelt(
#if defined(M_PROTO)
  ELTSTRUCT *eltp, FILE *tempfile
#endif
  ) ;

void template(
#if defined(M_PROTO)
  M_NOPAR
#endif
  ) ;

char *typecon(
#if defined(M_PROTO)
  int n
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

void warning2(
#if defined(M_PROTO)
  char *text, M_WCHAR *arg1, M_WCHAR *arg2
#endif
  ) ;

void warning3(
#if defined(M_PROTO)
  char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3
#endif
  ) ;

void warning4(
#if defined(M_PROTO)
  char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3, M_WCHAR *arg4
#endif
  ) ;

LOGICAL m_whitespace(
#if defined(M_PROTO)
  M_WCHAR c
#endif
  ) ;

