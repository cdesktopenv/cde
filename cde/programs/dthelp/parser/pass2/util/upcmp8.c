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
/* $XConsortium: upcmp8.c /main/3 1995/11/08 11:09:12 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Compare uppercase versions of two strings of eight-bit characters */

#include <stdio.h>
#include "basic.h"
#include "common.h"
extern M_CHARTYPE m_ctarray[256] ;

int m_wcupstrcmp(const M_WCHAR *p, const M_WCHAR *q);

int m_wcmbupstrcmp(const M_WCHAR *p, const char *q);

int m_mbmbupstrcmp(const char *p, const char *q);

int m_wcupstrcmp(const M_WCHAR *p, const M_WCHAR *q )
  {
    do {
      if (m_ctupper(*p) < m_ctupper(*q)) return(-1) ;
      if (m_ctupper(*p) > m_ctupper(*q)) return(1) ;
      q++ ;
      } while (*p++) ;
    return(0) ;
    }

int m_wcmbupstrcmp(const M_WCHAR *p, const char *q)
  {
  M_WCHAR *wc;
  int      retval;

  wc = MakeWideCharString(q);
  retval = m_wcupstrcmp(p, wc);
  m_free(wc,"wide character string");
  return retval;
  }

int m_mbmbupstrcmp(const char *p, const char *q)
  {
  M_WCHAR *w_p, *w_q;
  int      retval;

  w_p = MakeWideCharString(p);
  w_q = MakeWideCharString(q);
  retval = m_wcupstrcmp(w_p, w_q);
  m_free(w_p,"wide character string");
  m_free(w_q,"wide character string");
  return retval;
  }
