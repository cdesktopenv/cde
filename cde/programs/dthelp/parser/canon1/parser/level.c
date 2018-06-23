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
/* $XConsortium: level.c /main/3 1995/11/08 09:38:25 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Procedure callable by interface designer.  Returns number of occurrences
   of element on parse stack */
int m_level(M_WCHAR *elt)
  {
    int i = 0 ;
    M_PARSE *stackptr ;

    for (stackptr = m_stacktop ;
         stackptr->oldtop ;
         stackptr = stackptr->oldtop) {
      if (! m_wcupstrcmp(elt, m_nameofelt(stackptr->element))) i++ ;
      }
    return(i) ;
    }


int m_mblevel(char *elt)
  {
  int     retval;
  M_WCHAR *wc_elt;

  wc_elt = MakeWideCharString(elt);
  retval = m_level(wc_elt);
  m_free(wc_elt,"wide character string");
  return(retval);
  }
