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
/* $XConsortium: parent.c /main/3 1995/11/08 10:22:57 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Procedure callable by interface designers.  Returns pointer to name
   of nth level parent of current element (0 is self, 1 is parent, 2
   is grandparent, etc.) */
M_WCHAR *m_parent(int n)
  {
    M_PARSE *stackptr ;

    for (stackptr = m_stacktop ;
         stackptr->oldtop && n >= 0 ;
         stackptr = stackptr->oldtop, n--) {
      if (! n)
        return(m_nameofelt(stackptr->element)) ;
      }
    return(NULL) ;
    }
