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
/* $XConsortium: pardef.c /main/3 1995/11/08 09:40:15 rswiston $ */
/*
                   Copyright 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Function callable by interface designers.  Returns pointer to default
   value of a parameter of an element. Type is set to indicate type of
   default.  Error indication is type is 0 and function returns NULL. */
M_WCHAR *m_pardefault(elt, param, type)
  M_WCHAR *elt ;
  M_WCHAR *param ;
  unsigned char *type ;
  {
    M_ELEMENT eltid ;
    int par ;
    int i ;

    *type = M_NULLVAL ;
    if (! (eltid = m_packedlook(m_entree, elt))) return(NULL) ;

    for (par = m_element[eltid - 1].parptr, i = 0 ;
         i < m_element[eltid - 1].parcount ;
         par++, i++)
      if (! m_wcupstrcmp(&m_pname[m_parameter[par - 1].paramname], param)) {
        *type = (unsigned char) m_parameter[par - 1].deftype ;
        if (*type == M_REQUIRED || *type == M_NULLDEF) return(NULL) ;
        return(m_parameter[par -1].defval) ;
        }

    /* Can't find the parameter */
    return(NULL) ;
    }

