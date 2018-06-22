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
/* $XConsortium: parvalok.c /main/3 1995/11/08 11:29:47 rswiston $ */
/*
                   Copyright 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Function callable by interface designers.  Returns TRUE if specified value
   is a legal value for the indicated parameter of the given element,
   FALSE otherwise. */
LOGICAL m_parvalok( M_WCHAR *elt , M_WCHAR *param , const M_WCHAR *value )
  {
    M_ELEMENT eltid ;
    int par ;
    int i ;

    if (! (eltid = m_packedlook(m_entree, elt))) return(FALSE) ;
    for (par = m_element[eltid - 1].parptr, i = 1 ;
         i <= m_element[eltid - 1].parcount ;
         par++, i++)
      if (! m_wcupstrcmp(&m_pname[m_parameter[par - 1].paramname], param))
        if (m_partype(par, value)) return(TRUE) ;
        else return(FALSE) ;

    /* No such parameter */
    return(FALSE) ;
    }

