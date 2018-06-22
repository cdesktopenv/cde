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
/* $XConsortium: findpar.c /main/3 1995/11/08 10:52:30 rswiston $ */
/*
                   Copyright 1989 Hewlett-Packard Co.
*/

#include <stdio.h>
#include "basic.h"
#include "trie.h"
#include "dtdext.h"
#include "parser.h"

/* Function callable by interface designers.  Returns index in parameter
   list of a specified parameter for an element (i.e., 1 for first parameter
   specified in the DTD, 2 for second, etc.).  First parameter, elt, is
   the name of the element.  Second parameter is the name of the parameter
   or NULL.  Third parameter is used only if second parameter is NULL.  In
   the latter case, function returns first keyword parameter, if any, for
   which the specified  value is a legal value and otherwise first
   Returns 0 if elt is not a valid element name, if param is specified
   but is not a valid parameter name, or if param is NULL and value is
   not a valid value of any of elt's parameters. */
int m_findpar( const char *elt , const char *param , const M_WCHAR *value )
{
M_ELEMENT eltid ;
int par ;
int i ;
M_WCHAR *wc_elt;

wc_elt = MakeWideCharString(elt);
if (! (eltid = m_packedlook(m_entree, wc_elt)))
    {
    m_free(wc_elt,"wide character string");
    return(M_NULLVAL) ;
    }
m_free(wc_elt,"wide character string");

/* A parameter name specified */
if (param)
    {
    for (par = m_element[eltid - 1].parptr, i = 1 ;

    i <= m_element[eltid - 1].parcount ;
    par++, i++)
    if (!m_wcmbupstrcmp(&m_pname[m_parameter[par - 1].paramname], param))
    return(i) ;
    return(M_NULLVAL) ;
    }

/* No parameter name specified */

/* First check is it a valid keyword? */
for (par = m_element[eltid - 1].parptr, i = 1 ;
     i <= m_element[eltid - 1].parcount ;
     par++, i++)
if (m_parameter[par - 1].type == M_KEYWORD)
if (m_partype(par, value)) return(i) ;

/* It wasn't a keyword.  Check for valid value for some other parameter.*/
for (par = m_element[eltid - 1].parptr, i = 1 ;
i <= m_element[eltid - 1].parcount ;
par++, i++)
    {
    if (m_parameter[par - 1].type == M_KEYWORD) continue ;
    if (m_partype(par, value)) return(i) ;
    }

/* Not a valid value for any parameter */
return(M_NULLVAL) ;
}

