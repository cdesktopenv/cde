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
/* $XConsortium: proto.h /main/3 1995/11/08 09:28:07 rswiston $ */
/*
                   Copyright (c) 1988, 1989 Hewlett-Packard Co.
*/

/* Proto.h contains function prototypes for program ELTDEF. */


void actptrout(ACTION **array, char *name);

void addent(M_NOPAR);

LOGICAL m_allwhite(const M_WCHAR *string);

void closeiffile(LOGICAL flag, FILE *file, int count, char *table, char *proto);

void countdown(M_TRIE *parent, int *count);

void cvalue(M_NOPAR);


void done(M_NOPAR);

void m_dumptrie(FILE *file,
  M_TRIE *xtrie,
  char *extname,
  int *count,
  void (*proc)(M_ENTITY *));

void endcode(LOGICAL flag, FILE *file);

void endini(M_NOPAR);

void endsignon(M_NOPAR);

void endstring(M_NOPAR);

void entout(char *fname);

void enttype(int type);

void m_error(char *text);

void m_err1(const char *text, const M_WCHAR *arg);

void m_mberr1(char *text, const char *arg);

void m_err2(const char *text, const M_WCHAR *arg1, const M_WCHAR *arg2);

void m_err3(char *text, M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3);

void m_err5(char *text,
  M_WCHAR *arg1, M_WCHAR *arg2, M_WCHAR *arg3, M_WCHAR *arg4, M_WCHAR *arg5);

void m_exit(int status);

void m_free(void *block, char *msg);

void freechain(M_NOPAR);

int getachar(M_NOPAR);

int *getaction(ACTION **array);

ACTION *getactstruct(M_NOPAR);

void getname(int first);

int gettoken(int *c, int context);

void m_initctype(M_NOPAR);

void initialize(M_NOPAR);

LOGICAL m_letter(M_WCHAR c);

LOGICAL litproc(int delim);

int main(int argc, char **argv);

void *m_malloc(int size, char *msg);

void m_openchk(FILE **ptr, char *name, char *mode);

void outpval(M_TRIE *p);

void outstring(M_NOPAR);

int m_packedlook(M_PTRIE *xptrie, M_WCHAR *name);

const M_WCHAR *m_partype(const int par, const M_WCHAR *string);

int scan(M_NOPAR);

void skiptoend(M_NOPAR);

void startcode(int caseno, LOGICAL *flag, FILE *file, char *prefix, char *proto,
  char *formal, char *formtype);

void startelement(M_NOPAR);

void storecvar(M_NOPAR);

void storepname(M_NOPAR);

void undodelim(M_WCHAR *delim);

void ungetachar(int c);

int m_wcupstrcmp(const M_WCHAR *p, const M_WCHAR *q);

int m_wcmbupstrcmp(const M_WCHAR *p, const char *q);

int m_mbmbupstrcmp(const char *p, const char *q);

void value(M_WCHAR *p);

void warning(char *text);

void warning1(char *text, M_WCHAR *arg);

LOGICAL m_whitespace(M_WCHAR c);

