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
/* $XConsortium: allwhite.c /main/3 1995/11/08 11:01:49 rswiston $ */
/*
                   Copyright 1987, 1988, 1989 Hewlett-Packard Co.
*/
#include "basic.h"

LOGICAL m_allwhite(
#if defined(M_PROTO)
  const M_WCHAR *string
#endif
  ) ;

LOGICAL m_whitespace(
#if defined(M_PROTO)
  M_WCHAR c
#endif
  ) ;

/* Test if a string contains only white space characters */
#if defined(M_PROTO)
LOGICAL m_allwhite(const M_WCHAR *string )
#else
LOGICAL m_allwhite(string)
M_WCHAR *string ;
#endif /* M_PROTO */
{
const M_WCHAR *p ;

for (p = string; *p ; p++)
    if (! m_whitespace(*p)) return(FALSE) ;
return(TRUE) ;
}
