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
/* $XConsortium: triepdmp.c /main/3 1995/11/08 09:56:11 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"

void countdown(M_TRIE *parent, int *count);

void dumpnode(LOGICAL *first, FILE *file, M_TRIE *trieptr, int *count,
    void (*proc)(FILE *file, void *value));

void dumpptrie(M_TRIE *intname, char *extname, FILE *file, char *externdef, char *defdef,
  void (*proc)(FILE *file, void *value));

/* Write the declaration of a packed trie to the specified files */
void dumpptrie(M_TRIE *intname, char *extname, FILE *file,
               char *externdef, char *defdef,
               void (*proc) (FILE *file, void *value))
  {
    LOGICAL first = TRUE ;
    int count = 0 ;
    int treelen = 1 ;

    /* Count nodes in tree */
    countdown(intname, &treelen); 

    if (treelen > 1) {
      fprintf(file, "%s M_PTRIE %s[%d]\n#if defined(%s)\n = {\n",
        externdef, extname, treelen - 1, defdef) ;
      dumpnode(&first, file, intname, &count, proc) ;
      fprintf(file, "}\n#endif\n  ;\n\n") ;
      }
    else {
      fprintf(file, "%s M_PTRIE %s[1] ;\n\n", externdef, extname) ;
      }
    }
