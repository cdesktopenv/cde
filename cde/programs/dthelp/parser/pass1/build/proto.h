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
/* $XConsortium: proto.h /main/3 1995/11/08 10:03:00 rswiston $ */
/*
                   Copyright (c) 1988, 1989 Hewlett-Packard Co.
*/

/* Proto.h contains function prototypes for program BUILD. */


int addarc(STATE *from, STATE *to, ELTSTRUCT *label, ANDGROUP *and, LOGICAL optional, 
  int id, LOGICAL minim, ELTSTRUCT **errelt);

void adddefent(M_WCHAR *mapname);

LOGICAL addent(M_WCHAR *name);

void addex(void);

void addkeyword(void);

LOGICAL addmapname(M_WCHAR *p, LOGICAL define);

void addndent(M_WCHAR *p);

void addpar(void);

void addsref(M_WCHAR *p);

LOGICAL m_allwhite(const M_WCHAR *string);

void checkand(ANDGROUP *andstart, ANDGROUP *andptr, STATE *start, TREE *root, 
  ELTSTRUCT **errelt);

LOGICAL checkdefault(const M_WCHAR *string);

int checkdfsa(STATE *from, ELTSTRUCT *label, ANDGROUP *and, int id, ELTSTRUCT **errelt);

int checkrepeat(STATE *from, ANDGROUP *and, ELTSTRUCT **errelt);

void copyintolist(STATELIST *from, STATELIST **to);

void countdown(M_TRIE *parent, int *count);

void defmapname(void);

char *deftype(int n);

void dellist(STATELIST **list);

void delstartarcs(void);

void done(void);

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

void eltreeout(void);

void endmodel(void);

void entout(char *fname);

char *enttype(int n);

void m_error(char *text);

void m_err1(const char *text, const M_WCHAR *arg);

void m_mberr1(char *text, const char *arg);

void m_err2(const char *text, const M_WCHAR *arg1, const M_WCHAR *arg2);

void exout(void);

void m_free(void *block, char *msg);

void freetree(TREE *ruletree);

void found(LOGICAL *flag, char *delim);

void fsa(void);

int getachar(void);

ANDGROUP *getand(void);

LOGICAL getname(int first);

STATE *getstate(void);

int gettoken(int *c, int context);

TREE *gettreenode(void);

void m_initctype(void);

void initialize(void);

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

void parout(void);

char *partype(int n);

LOGICAL permitspcd(ARC *a);

void pop(void);

void prulend(void);

void push(void);

LOGICAL regenerate(TREE *start, TREE *stop);

void repeat(TREE *root);

void ruleinit(void);

void rulend(void);

LOGICAL samelabelarc(ARC *a, STATE *s);

void savelhs(LOGICAL param);

void savestartarcs(void);

int scan(void);

void setdefault(const M_WCHAR *string);

void simplebranch(TREE *root, ELTSTRUCT *value, ANDGROUP *group, int optional);

void skiptoend(void);

int m_sprscon(int i, int j);

void srefout(void);

STATE *startfsa(TREE *root, LOGICAL *canbenull);

void tempelt(ELTSTRUCT *eltp, FILE *tempfile);

void template(void);

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

