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
/* $XConsortium: appl.h /main/3 1996/06/19 17:14:17 drk $ */
/* appl.h */

enum {
     E_NOMEM = 1,
     E_DOC,
     E_EXEC,
     E_FORK,
     E_WAIT,
     E_SIGNAL,
     E_OPEN,
     E_CAPBOTCH,
     E_SUBDOC
};

VOID process_document P((int));
VOID output_conforming P((void));

UNIV xmalloc P((UNS));
UNIV xrealloc P((UNIV, UNS));
VOID appl_error VP((int, ...));

#ifdef SUPPORT_SUBDOC
int run_process P((char **));
char **make_argv P((UNIV));
VOID get_subcaps P((void));
#endif

#ifdef SUPPORT_SUBDOC
extern int suberr;
#endif

extern int suppsw;
extern int locsw;
