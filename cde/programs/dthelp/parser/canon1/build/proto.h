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
/* $XConsortium: proto.h /main/3 1995/11/08 09:26:24 rswiston $ */
/*
                   Copyright (c) 1988, 1989 Hewlett-Packard Co.
*/

/* Proto.h contains function prototypes for program BUILD. */


int addarc(STATE *from, STATE *to, ELTSTRUCT *label, ANDGROUP *and, LOGICAL optional, 
  int id, LOGICAL minim, ELTSTRUCT **errelt);

void adddefent(M_WCHAR *mapname);

LOGICAL addent(M_WCHAR *name);

void addex(M_NOPAR);

void addkeyword(M_NOPAR);

LOGICAL addmapname(M_WCHAR *p, LOGICAL define);

void addndent(M_WCHAR *p);

void addpar(M_NOPAR);

void addsref(M_WCHAR *p);

LOGICAL m_allwhite(const M_WCHAR *string);

void checkand(ANDGROUP *andstart, ANDGROUP *andptr, STATE *start, TREE *root, 
  ELTSTRUCT **errelt);

LOGICAL checkdefault(const M_WCHAR *string);

int checkdfsa(STATE *from, ELTSTRUCT *label, ANDGROUP *and, int id, ELTSTRUCT **errelt);

int checkrepeat(STATE *from, ANDGROUP *and, ELTSTRUCT **errelt);

void copyintolist(STATELIST *from, STATELIST **to);

void countdown(M_TRIE *parent, int *count);

void defmapname(M_NOPAR);

char *deftype(int n);

void dellist(STATELIST **list);

void delstartarcs(M_NOPAR);

void done(M_NOPAR);

void dumpentnode(FILE *file, M_TRIE *value);

void dumpmapnode(FILE *file, M_TRIE *value);

void dumpsrefnode(FILE *file,
  M_TRIE *value);

void dumpptrie(M_TRIE *intname, char *extname, FILE *file, char *externdef, char *defdef,
  void (*proc)( FILE *, M_TRIE *));

void m_dumptrie(FILE   *file,
  M_TRIE *xtrie,
  char   *extname,
  int    *count,
  void  (*proc)(M_ENTITY *));

void eltblocks(FILE *tempfile);

void eltreeout(M_NOPAR);

void endmodel(M_NOPAR);

void entout(char *fname);

char *enttype(int n);

void m_error(char *text);

void m_err1(const char *text, const M_WCHAR *arg);

void m_mberr1(char *text, const char *arg);

void m_err2(const char *text, const M_WCHAR *arg1, const M_WCHAR *arg2);

void exout(M_NOPAR);

void m_free(void *block, char *msg);

void freetree(TREE *ruletree);

void found(LOGICAL *flag, char *delim);

void fsa(M_NOPAR);

int getachar(M_NOPAR);

ANDGROUP *getand(M_NOPAR);

LOGICAL getname(int first);

STATE *getstate(M_NOPAR);

int gettoken(int *c, int context);

TREE *gettreenode(M_NOPAR);

void m_initctype(M_NOPAR);

void initialize(M_NOPAR);

LOGICAL litproc(int delim);

int main(int argc, char **argv);

void makeand(LOGICAL *canbenull, TREE *root, int optional);

LOGICAL makefsa(TREE *root, int optional);

void makeor(LOGICAL *canbenull, TREE *root);

void makeseq(LOGICAL *canbenull, TREE *root, int optional);

void *m_malloc(int size, char *msg);

void msgline(char *text);

void msg1line(char *text, char *arg1);

void nondeterm(TREE *root, int c, ELTSTRUCT *eltp);

void normdefault(M_WCHAR *string);

LOGICAL notinlist(STATELIST *item, STATELIST *list);

ELTSTRUCT *ntrelt(M_WCHAR *p);

void m_openchk(FILE **ptr, char *name, char *mode);

void parout(M_NOPAR);

char *partype(int n);

LOGICAL permitspcd(ARC *a);

void pop(M_NOPAR);

void prulend(M_NOPAR);

void push(M_NOPAR);

LOGICAL regenerate(TREE *start, TREE *stop);

void repeat(TREE *root);

void ruleinit(M_NOPAR);

void rulend(M_NOPAR);

LOGICAL samelabelarc(ARC *a, STATE *s);

void savelhs(LOGICAL param);

void savestartarcs(M_NOPAR);

int scan(M_NOPAR);

void setdefault(const M_WCHAR *string);

void simplebranch(TREE *root, ELTSTRUCT *value, ANDGROUP *group, int optional);

void skiptoend(M_NOPAR);

int m_sprscon(int i, int j);

void srefout(M_NOPAR);

STATE *startfsa(TREE *root, LOGICAL *canbenull);

void tempelt(ELTSTRUCT *eltp, FILE *tempfile);

void template(M_NOPAR);

char *typecon(int n);

void undodelim(M_WCHAR *delim);

void ungetachar(int c);

int m_wcupstrcmp(const M_WCHAR *p, const M_WCHAR *q);

int m_wcmbupstrcmp(const M_WCHAR *p, const char *q);

int m_mbmbupstrcmp(const char *p, const char *q);

void warning(char *text);

void warning1(char *text, M_WCHAR *arg);

void warning2(char *text, M_WCHAR *arg1, M_WCHAR *arg2);

void warning3(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3);

void warning4(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3, M_WCHAR *arg4);

LOGICAL m_whitespace(M_WCHAR c);

