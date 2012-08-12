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
/* $XConsortium: eltree.c /main/3 1995/11/08 09:25:13 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright (c) 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Eltree.c contains procedures that manipulate element names */

#include <string.h>
#include <stdlib.h>
#include "build.h"

/* Enters an element name into the element name tree */
ELTSTRUCT *ntrelt(p)
  M_WCHAR *p ;
  {
    ELTSTRUCT *new ;
    ELTSTRUCT *old ;
    int length ;

    new = (ELTSTRUCT *) m_malloc(sizeof(ELTSTRUCT), "element structure") ;
    if (old = (ELTSTRUCT *) m_ntrtrie(p, &eltree, (M_TRIE *) new)) {
      m_free((M_POINTER) new, "element structure") ;
      return(old) ;
      }
    *nextelt = new ;
    nextelt = &new->next ;
    new->eltno = ++ecount ;
    length = w_strlen(p) + 1 ;
    enamelen += length ;
    new->enptr = (M_WCHAR *) m_malloc(length, "element name") ;
    w_strcpy(new->enptr, p) ;
    new->model = M_NULLVAL ;
    new->content = M_NULLVAL ;
    new->inptr = new->exptr = NULL ;
    new->parptr = NULL ;
    new->parindex = M_NULLVAL ;
    new->paramcount = M_NULLVAL ;
    new->stmin = new->etmin = FALSE ;
    new->srefptr = M_NULLVAL ;
    new->useoradd = TRUE ; 
    new->next = NULL ;
    return(new) ;
    }
