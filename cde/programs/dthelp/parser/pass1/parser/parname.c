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
/* $XConsortium: parname.c /main/3 1995/11/08 10:23:17 rswiston $ */
/*
                   Copyright 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Function callable by interface designers.  Returns name of nth parameter
   of specified element (NULL indicates error). */
M_WCHAR *m_parname(elt, n)
  M_WCHAR *elt ;
  int n ;
  {
    M_ELEMENT eltid ;

    if (! (eltid = m_packedlook(m_entree, elt))) return(NULL) ;
    if (n < 1 || n > m_element[eltid - 1].parcount) return(NULL) ;
    return(&m_pname[m_parameter[(n - 1) + m_element[eltid - 1].parptr - 1].
                    paramname]) ;
    }

M_WCHAR *m_mbparname(elt, n)
char *elt ;
int n ;
{
M_WCHAR *wc_elt, *retval;

wc_elt = MakeWideCharString(elt);
retval = m_parname(wc_elt, n);
m_free(wc_elt,"wide character string");
}

