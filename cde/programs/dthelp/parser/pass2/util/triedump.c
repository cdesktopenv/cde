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
/* $XConsortium: triedump.c /main/3 1995/11/08 11:08:37 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"

void countdown(
#if defined(M_PROTO)
  M_TRIE *parent, int *count
#endif
  ) ;

void m_dumptrie(
#if defined(M_PROTO)
  FILE *file, M_TRIE *xtrie, char *extname, int *count,
  void (*proc)(void *value)
#endif
  ) ;

/* Write the C declaration of a trie */
void m_dumptrie(file, xtrie, extname, count, proc)
  FILE *file ;
  M_TRIE *xtrie ;
  char *extname ;
  int *count ;
  void (*proc)(
#if defined(M_PROTO)
    void *value
#endif
    ) ;
  {
    int firstson ;
    M_TRIE *p ;

    firstson = *count + 1 ;
    for (p = xtrie ; p ; p = p->next) firstson++ ;

    for (p = xtrie ; p ; p = p->next) {
      (*count)++ ;
      fprintf(file, ",\n  {%d, ", (int)p-> symbol) ;
      if (p->next) fprintf(file, "&%s[%d], ", extname, *count + 1) ;
      else fputs("NULL, ", file) ;
      if (p->symbol) {
        fprintf(file, "&%s[%d]}", extname, firstson) ;
        countdown(p, &firstson) ;
        }
      else { (*proc) (p->data) ; fprintf(file, "}") ; }
      }

    for (p = xtrie ; p ; p = p->next)
      if (p->symbol)
        m_dumptrie(file, p->data, extname, count, proc) ;
    } 
