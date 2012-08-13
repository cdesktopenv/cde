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
/* $XConsortium: delfree.c /main/3 1995/11/08 11:21:55 rswiston $ */
/* Copyright (c) 1988 Hewlett-Packard Co. */

/* Versions of m_free and m_malloc for debugging. */

/**/
#include <stdlib.h>
#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtd.h"
#include "parser.h"

m_free(block, msg)
  char *block ;
  char *msg ;
  {
    free(block) ;
    fprintf(m_outfile, "%5u:%5u- Freed     %s\n",
      (unsigned int) ((unsigned long) block >> 16),
      (unsigned int) block, msg) ;
    }

char *m_mallocx(size, msg)
  int size ;
  char *msg ;
  {
    char *p ;

    if (! size) return(NULL) ;
    p = malloc(size) ;
    if (! p) {
      m_err1("Unable to allocate space for %s", msg) ;
      exit(TRUE) ;
      }
    fprintf(m_outfile, "%5u:%5u- Allocated %s\n",
      (unsigned int) ((unsigned long) p >> 16), (unsigned int) p, msg) ;
    return(p) ;
    }
